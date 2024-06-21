/* This code performs board_end() and prints some debug prints */
#include "common.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_pss_regs.h"
#include "nu4000_c0_lram_regs.h"
#include "usb_drv.h"
#include "nu4000_c0_ddrc_regs.h"
#include "nu4000_c0_gme_regs.h"

#include "board.h"
#include "spl.h"
#include "inu_storage_layout.h"
#include "inu_storage.h"
#include "storage.h"
#include "verify.h"

#include "inu_mem_def.h"
#define DEFSG_IS_TARGET 1
#include "inu_common_cp.h"
#include "pm.h"
//#define FPGA_BOARD
//#define __NAND_FLASH__

#ifdef __NAND_FLASH__
#include "spi-nand-driver.h"
#endif   //__NAND_FLASH__

#ifdef FPGA_BOARD
#include "PL330_dma_instruction.h"
#include "PL330_dma_misc.h"
#endif
#include "inu_boot_common.h"

/**********************************************
 * global data
***********************************************/

InuBootsplInfoHeaderT   bootsplInfo;

/**********************************************
 * local defines
***********************************************/

uint32 DSPB_D_READ_QOS  = 0xA045100;
uint32 DSPB_D_WRITE_QOS = 0xA045104;
uint32 DSPB_I_READ_QOS  = 0xA046100;
uint32 DSPB_I_WRITE_QOS = 0xA046104;
//#define DDR_TEST  1
#define OVERRIDE_DEBUG_UART_STRAP 1


#define WDOG_CONTROL_REG_OFFSET             (0x00)
#define WDOG_CONTROL_REG_WDT_EN_MASK        (0x01)
#define WDOG_CONTROL_REG_WDT_RPL_MASK       (0x1C)
#define WDOG_TIMEOUT_RANGE_REG_OFFSET       (0x04)
#define WDOG_CURRENT_COUNT_REG_OFFSET       (0x08)
#define WDOG_COUNTER_RESTART_REG_OFFSET     (0x0c)
#define WDOG_COUNTER_RESTART_KICK_VALUE     (0x76)
#define WDOG_BASE_ADDRESS                   (0x080F0000)
#define OFFSET_LOAD_FROM_FLASH              (4)
typedef void (*kernel_entry_arg_t)(int, int, void *) __attribute__ ((noreturn));

#define CEVA_BOOTP_BOOT_PARAMS_OFFSET      (0x7D)  //boot params offset from boot copier code pointer, in UINT32 units (=0x1f4)
#define TCM_ADDRESS                        (0xA4000000)

#define SB_OFFSET                          (0x00100000)
#define SB_HDR_SIZE                        (264)
unsigned int audioFreqG;
/**********************************************
 * functions
***********************************************/

extern void icache_disable(void);
extern void invalidate_icache_all(void);

extern void dcache_enable(void);
extern void icache_enable(void);
extern void dcache_disable (void);
unsigned int ddrm_umctl_read_mrr (unsigned int mr_addr, unsigned int channel);
static void *uart_boot_dev= NULL;


unsigned char updateSecurityVersion = 0;
unsigned char securityVersionG;

//#define DISABLE_CACHE
/* Enable D-cache */
void enable_caches()
{
#ifndef DISABLE_CACHE
   /* Enable D-cache. I-cache is already enabled in start.S */
   dcache_enable();
   icache_enable();
#endif
}

void spl_enable_dcache()
{
#ifndef DISABLE_CACHE
   //Enable Data cache & MMU to increase secure boot performance
   enable_caches();
   //As soon as the MMU is enabled, these flat-mapped addresses will become invalid as the cache is now looking for virtual addresses as defined in the MMU page table, so it is important to remember to invalidate the I-Cache when enabling the MMU
   invalidate_icache_all();
#endif
}


void spl_disable_dcache()
{
   //disable data cache & MMU
   dcache_disable();
}
static void setup_ceva_qos()
{
   *((uint32 *)DSPB_D_READ_QOS) = 5;
   *((uint32 *)DSPB_D_WRITE_QOS) = 5;
   *((uint32 *)DSPB_I_READ_QOS) = 6;
   *((uint32 *)DSPB_I_WRITE_QOS) = 6;
}

static void restore_board_mode()
{
   strap_info_t strap_info;

   //This is a workaround to bootrom b0 bug:
   //In case of suspend/resume in bootrom the board_mode variable is set to NO_PLL mode
   //read the bootstraps again and set the board mode
   board_bootstrap_get(&strap_info);
   board_set_mode(BOARD_CLK_PLL);

}

static void cleanup_before_linux(void)
{
   /*
    * this function is called just before we call linux
    * it prepares the processor for linux
    *
    * we turn off caches etc ...
    */
   disable_interrupts();

   /*
    * Turn off I-cache and invalidate it
    */
   invalidate_icache_all();

   /*
    * Note: BootROM  runs without D-cache so no need to invalidate it here
    */
}


#ifdef FPGA_BOARD

#if 1
static void spl_zero_ddr(UINT32 addrs, UINT32 size, UINT32 magic_number)
{
   volatile unsigned int *ptr      =(volatile unsigned int *)addrs;
   volatile unsigned int *lram_base=(volatile unsigned int *)LRAM_START_ADDRESS;
   unsigned int ind;

   if (lram_base[LRAM_TOTAL_SIZE/4 - 1] != magic_number) // last address of lram
   {
      spl_log("spl_zero_ddr: erase addr %x size %x\n", ptr, size);

      for (ind = 0; ind < size; ind++)
      {
         ptr[ind] = 0;
      }
      lram_base[LRAM_TOTAL_SIZE/4 - 1] = magic_number;
   }
   else
   {
      spl_log("spl_zero_ddr: nothing to do - ddr already zeroed\n");
   }

}
#else
static void spl_zero_ddr_dma(UINT32 addrs, UINT32 size, UINT32 magic_number)
{
   volatile unsigned int *lram_base=(volatile unsigned int *)LRAM_START_ADDRESS;
   uint8 programm[32];
   dmaChannelProg dmaProg = (dmaChannelProg)programm;
   PL330_DMA_StatusE status = PL330_DMA_STATUS_FAULTING_E;
   UINT32 i = 0, dar, pc;


   if (lram_base[LRAM_TOTAL_SIZE/4 - 1] != magic_number) // last address of lram
   {
      spl_log("spl_zero_ddr_dma: erase addr %x size %x.\n", addrs, size);

      PL330DMAP_ConstrInit(/*&programm[0]*/dmaProg);
      PL330DMAP_DMAMOV(PL330DMA_REG_CCR, 0x003E40F9);//0x003E4000);
      PL330DMAP_DMAMOV(PL330DMA_REG_DAR, addrs);
      PL330DMAP_DMALP((size / 0x100));               // Assuming size alignment 256 byte(size / 0x100)
      PL330DMAP_DMASTZ();
      PL330DMAP_DMAWMB();
      PL330DMAP_DMALPEND();
      PL330DMAP_DMAEND();

      PSS_DMA_1_CTRL_BOOT_FROM_PC_W(0);
      //   Enable dma clocks
      GME_CLOCK_ENABLE_DMA_1_CLK_EN_W(1);
      while(!(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R));
      spl_log("spl_zero_ddr_dma: dma enabled. Prog ptr %x. Number of loops %x \n", dmaProg, (size / 0x100));

      PL330DMAG_SendCommand(/*&programm[0]*/dmaProg, 1, 0, PL330DMA_CMD_DMAGO, 0);

      while(status != PL330_DMA_STATUS_STOPPED_E)
      {
         status = PL330_DMA_getChannelStatus(1, 0);
         dar  = PL330_DMA_getDAR(1, 0);
         pc = PL330_DMA_getPC(1, 0);
         i++;
         if((i%0x40000) == 0)
            spl_log("spl_zero_ddr_dma: waiting DMA status - %d, %x , pc - %x\n", status, dar, pc);
      }
      spl_log("spl_zero_ddr_dma: Done\n");

      lram_base[LRAM_TOTAL_SIZE/4 - 1] = magic_number;
   }
   else
   {
      spl_log("spl_zero_ddr_dma: nothing to do - ddr already zeroed\n");
   }

}
#endif
#endif

/* Byte write primitive for all supported environments */
void uart0_writeb(unsigned char byte)
{
/*******************************************/
/*********  For BootROM release  ***********/
/*******************************************/
   // Enable UART debug according to strap pin
   if(uart_boot_dev)
   {
      uart_write(uart_boot_dev, byte);
   }
}

static inline void fill_string(char *p)
{
   while (*p != 0) {
       uart0_writeb(*p++);
   }
}

static inline void fill_int(unsigned int data)
{
   if ((data/10) > 0) {
      fill_int(data/10);
   }

   uart0_writeb((data%10) + '0');
}

static inline void fill_hex_int(unsigned int data)
{
   if ((data >> 4) > 0) {
      fill_hex_int(data >> 4);
   }

   if ((data & 0xF) < 10)
       uart0_writeb((data & 0xF) + '0');
   else
       uart0_writeb((data & 0xF) - 10 + 'a');
}

int uart0_printf(const char *fmt_str, ...)
{
   va_list ap;

   if (uart_boot_dev == NULL)
      return 0;

   va_start(ap, fmt_str);

   while (*fmt_str != 0) {

#ifdef CONFIG_BUFFER_DEBUG_MSG
      if(debug_buffer_overflow())
         break;
#endif

      if (*fmt_str == '%') {
         if (*(fmt_str + 1) == '%') {
             uart0_writeb('%');
            fmt_str += 2;
         }
         else {
            fmt_str++;   /* skip % */
            while(*fmt_str != 0) {

               switch (*fmt_str) {
               case 'd':
               case 'i':
                  fill_int(va_arg(ap, unsigned int));
                  break;
               case 'u':
               case 'x':
                  uart0_writeb('0');
                  uart0_writeb('x');
                  fill_hex_int(va_arg(ap, unsigned int));
                  break;
               case 's':
                  fill_string(va_arg(ap, char *));
                  break;
               case 'c':
                  uart0_writeb ((unsigned char)va_arg(ap, signed long));
                  break;
               default:
               {
                  fmt_str++;
                  continue; //skip over unsupported stuff
               }

               }

               fmt_str++;
               break;
            }
         }
      }
      else if(*fmt_str == '\n') {
         uart0_writeb('\r');
         uart0_writeb(*fmt_str++);
      }
      else {
          uart0_writeb(*fmt_str++);
      }
   }
   va_end(ap);

   return 0;
}

void spl_board_lowlevel_init()
{

   /* Init Debug UART, by overriding bootrom setup */
#ifdef OVERRIDE_DEBUG_UART_STRAP
//   board_dbg_uart_init(0);
//   debug_init(1);
#endif
   board_boot_uart_init();
   uart_boot_dev = uart_boot_init();
   //COMMENTED OUT BY IMDT spl_log ("bootspl: version=%d.%d.%d %s %s\n", MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION,__DATE__,__TIME__);
   restore_board_mode();
   board_timer_init();
}

void spl_board_init()
{
   setup_ceva_qos();
}


void spl_spi_board_init()
{
   board_usb_init();
   board_usb_phy_init();

}


void spl_ddr_init()
{
    unsigned int freq=350;
#ifdef DDR_FULL_BUS_WIDTH_FREQ334_MODE
   freq = 334;
#elif DDR_FULL_BUS_WIDTH_FREQ533_MODE
   freq = 533;
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE
    freq = 800;
#elif DDR_FULL_BUS_WIDTH_FREQ1000_MODE
    freq = 1000;
#elif DDR_FULL_BUS_WIDTH_FREQ1067_MODE
    freq = 1067;
#elif DDR_FULL_BUS_WIDTH_FREQ1200_MODE
    freq = 1200;
#elif DDR_FULL_BUS_WIDTH_FREQ1334_MODE
    freq = 1334;
#elif DDR_FULL_BUS_WIDTH_FREQ1400_MODE
    freq = 1400;
#elif DDR_FULL_BUS_WIDTH_FREQ1600_MODE
    freq = 1600;
#endif
    spl_log("bootspl: ddr init...freq %d\n",freq);

#ifdef FPGA_BOARD
   // Work around in FPGA - DDR has to be zeroed in order to boot Linux
#if 1
   spl_zero_ddr(DDR_START_ADDRESS, 0x2000000, 0x12345);
#else //if 0
   spl_zero_ddr_dma(DDR_START_ADDRESS, 0x2000000, 0x12345);
#endif
#else   // not FPGA_BOARD
   ddr_pll_config(freq);
   // Call DDR Phy and controller initialization
   ddrm_umctl_init_seq(freq);
#ifdef BOOT_500
   run_mem_screening_tests();
#endif //BOOT_500
#endif

   spl_log("bootspl: ddr init ... done. DDR size is: %d MB\n", (DDR_SIZE >> 20));

#ifdef DDR_TEST
   spl_log("bootspl: testing ddr\n");

#ifdef DDR_TEST_SIMPLE
   MEM_TESTG_simple(DDR_START_ADDRESS, 0x10000);
#else
   MEM_TESTG_start(DDR_START_ADDRESS, 0x10000);
#endif

   spl_log("bootspl: ddrtest done\n");
#endif
}

#ifdef __NAND_FLASH__

#define NAND_BOOT_RETRIES_MAX    (3)
static InuStorageNandHeaderT nandHdr;

UINT32 spl_nand_type_to_boot_addr(InuStorageNandSectionTypE type)
{
   switch(type)
   {
      case NAND_SECTION_TYPE_KERNEL:       return DDR_KERNEL_START_ADDRESS;
      case NAND_SECTION_TYPE_DTB:       return DDR_DTB_START_ADDRESS;
      case NAND_SECTION_TYPE_CEVA:       return DDR_CEVA_START_ADDRESS;
      case NAND_SECTION_TYPE_EV62_BOOT:    return LRAM_BASE;               // TODO: create define
      case NAND_SECTION_TYPE_EV62_APP:    return DDR_CNN_START_ADDRESS;
      case NAND_SECTION_TYPE_INITRAMFS:   return DDR_INITRAMFS_START_ADDRESS;
      case NAND_SECTION_TYPE_APP:       return DDR_APP_START_ADDRESS;

      case NAND_SECTION_TYPE_HEADER:
      case NAND_SECTION_TYPE_BOOTSPL:
      case NAND_SECTION_TYPE_NONE:
      default:
         return    NAND_EMPTY_UINT32;
   }
}

int spl_nand_boot(UINT8 isDefaultPartition, InuBootfixHeaderT *bootfixHeader)
{
   int section, i;
   spinand_io_param_t nand_io;
   UINT32 nand_block_size, partition_off;
   UINT32    int_enable_h = *(UINT32*)0x8170004 ;
   int_enable_h &= ~0x40;    //Disable USB interrupt (38)
   *(UINT32*)0x8170004  = int_enable_h & (~0x40);

   spinand_init();
   nand_block_size = (UINT32)spinand_get_block_size();

   //Read NAND Header
   partition_off = (isDefaultPartition) ? (NAND_PARTITION_FACTORY_START_BLOCK) : (NAND_PARTITION_FW_UPDATE_START_BLOCK);
   nand_io.buf = (UINT8 *)&nandHdr;
   nand_io.offset = ((partition_off + NAND_SECTION_HEADER_START_BLOCK) * nand_block_size);
   nand_io.length =  sizeof(InuStorageNandHeaderT);

   spl_log("spl_nand_boot :io->offset %x, io->length %x, io->buf %x. Block size %x\n",
            nand_io.offset, nand_io.length, nand_io.buf, nand_block_size);
   spinand_read(&nand_io);

   printf("\t==Magic Number 0x%x, Header Version 0x%x==\n", nandHdr.magicNumber, nandHdr.headerVersion);
   for(i=0; i<NAND_SECTION_MAX_SECTIONS; i++)
   {
      printf("\tSection type %d, Start 0x%x, Size 0x%x\n", nandHdr.section[i].type, nandHdr.section[i].startAddress, nandHdr.section[i].sizeBytes);
   }

   if(nandHdr.magicNumber != NAND_HEADER_MAGIC_NUMBER)
   {
      spl_log("spl_nand_boot :NAND header magic %x is not valid.\n", nandHdr.magicNumber);

      if(isDefaultPartition)
      {
         spl_log("spl_nand_boot: Factory default partition is corrupted\n");
      }
      else
      {
         bootfixHeader->bootFromFactoryDefault = 1;
         bootfixHeader->nandBootSuccess = 0;
         storage_update_bootfix_header(bootfixHeader);
      }

      return -1;
   }

   for(section = 0; section < NAND_SECTION_MAX_SECTIONS; section++)
   {
      //find the sections which need to booted in SPL
      if((spl_nand_type_to_boot_addr(nandHdr.section[section].type) !=  NAND_EMPTY_UINT32) &&
         (nandHdr.section[section].imgStartAddress != NAND_EMPTY_UINT32))
         {
            spl_log("Found Section type %d. Loading to address %x from nand address %x\n",
               nandHdr.section[section].type, spl_nand_type_to_boot_addr(nandHdr.section[section].type),
               nandHdr.section[section].imgStartAddress);

            if( (nandHdr.section[section].type ==  NAND_SECTION_TYPE_APP) ||
               (nandHdr.section[section].type ==  NAND_SECTION_TYPE_INITRAMFS) )
            {
               nand_io.buf = (UINT8 *)(spl_nand_type_to_boot_addr(nandHdr.section[section].type) + 4);
               nand_io.offset = nandHdr.section[section].imgStartAddress;
               nand_io.length = nandHdr.section[section].imgSizeBytes;
               *(UINT32 *)spl_nand_type_to_boot_addr(nandHdr.section[section].type) = nand_io.length;
               GME_SAVE_AND_RESTORE_5_VAL = spl_nand_type_to_boot_addr(nandHdr.section[section].type);
            }
            else
            {
               nand_io.buf = (UINT8 *)spl_nand_type_to_boot_addr(nandHdr.section[section].type);
               nand_io.offset = nandHdr.section[section].imgStartAddress;
               nand_io.length = nandHdr.section[section].imgSizeBytes;
            }

            spinand_read(&nand_io);
#if 0
            spl_log(" Dumping address: %x\n", (UINT32)nand_io.buf);
            for(i = 0; i < 100; i += 4)
            {
               spl_log(" %d: %x %x %x %x\n", i, *(nand_io.buf + i), *(nand_io.buf + i + 1), *(nand_io.buf + i + 2), *(nand_io.buf + i + 3));
            }

            spl_log(" %d: %x %x %x %x\n", i, *(nand_io.buf + nand_io.length - 12), *(nand_io.buf + nand_io.length - 11), *(nand_io.buf + nand_io.length - 10), *(nand_io.buf + nand_io.length - 9));
            spl_log(" %d: %x %x %x %x\n", i, *(nand_io.buf + nand_io.length - 8), *(nand_io.buf + nand_io.length - 7), *(nand_io.buf + nand_io.length - 6), *(nand_io.buf + nand_io.length - 5));
            spl_log(" %d: %x %x %x %x\n", i, *(nand_io.buf + nand_io.length - 4), *(nand_io.buf + nand_io.length - 3), *(nand_io.buf + nand_io.length - 2), *(nand_io.buf + nand_io.length - 1));
            spl_log("\n");
#endif
         }
      }

   spl_disable_dcache();
   spl_boot_linux();

   return 0;
}



#endif   //__NAND_FLASH__

void spl_spi_boot()
{
   int rc;
   InuBootfixHeaderT inuBootfixHeader;
   unsigned int sbOffset=0;
#ifdef __NAND_FLASH__
   unsigned int isBootFactoryDefault = 0;
   int ret = 0;
#else
   int app_size;
#endif   //__NAND_FLASH__

   if (*(GME_SAVE_AND_RESTORE_0_REG + OFFSET_LOAD_FROM_FLASH) == 1)
   {
      storage_init();
      rc = storage_get_section_data(INU_STORAGE_SECTION_BOOTFIX_METADATA, (unsigned char *) &inuBootfixHeader);
      if (rc != -1)
      {
#ifdef __NAND_FLASH__
         //inuBootfixHeader.isBootfromFlash = 1;   //2 NOTE: For test only !!!!

         spl_log("__NAND_FLASH__: isBootfromFlash %x, bootFromFactoryDefault %x, nandBootSuccess %x, bootCounter %x nandNewImages %x\n",
            inuBootfixHeader.isBootfromFlash, inuBootfixHeader.bootFromFactoryDefault, inuBootfixHeader.nandBootSuccess, inuBootfixHeader.bootCounter, inuBootfixHeader.nandNewImages);

         if ((inuBootfixHeader.isBootfromFlash == 1) && (inuBootfixHeader.bootFromFactoryDefault == 1))   // Boot from Factory default partition
         {
            spl_log("Boot from Factory default partition\n");
            ret = spl_nand_boot(1, &inuBootfixHeader);
         }

         if ((inuBootfixHeader.isBootfromFlash == 1) && (inuBootfixHeader.bootFromFactoryDefault == 0))   // Boot from FW update partition
         {
            spl_log("Boot from FW update partition:");

            if(inuBootfixHeader.nandBootSuccess == 1)
            {
               spl_log("\nBooting FW update partition with boot success\n");
               ret = spl_nand_boot(0, &inuBootfixHeader);
            }
            else
            {
               if(inuBootfixHeader.bootCounter < NAND_BOOT_RETRIES_MAX)
               {
                  isBootFactoryDefault = 0;
                  inuBootfixHeader.bootCounter++;
               }
               else
               {
                    isBootFactoryDefault = 1;
               }



               //inuBootfixHeader.isBootfromFlash = 0;   //2 NOTE: For test only !!!!
               spl_log(" Count = %d, isBootFactoryDefault = %d\n", inuBootfixHeader.bootCounter, isBootFactoryDefault);

               storage_update_bootfix_header(&inuBootfixHeader);
               spl_nand_boot(isBootFactoryDefault, &inuBootfixHeader);
            }
         }

         spl_log("Nothing to boot: isBootfromFlash %x, bootFromFactoryDefault %x, nandBootSuccess %x, bootCounter %x nandNewImages %x\n",
            inuBootfixHeader.isBootfromFlash, inuBootfixHeader.bootFromFactoryDefault, inuBootfixHeader.nandBootSuccess, inuBootfixHeader.bootCounter, inuBootfixHeader.nandNewImages);
#else
         if (inuBootfixHeader.isQuadFlash == 1)
           storage_set_flash_quad();
      if (inuBootfixHeader.isBootfromFlash == 1)
      {
         UINT32 cnnOffset, cnn_size;

         if (getEfuseSecureControlData() != NOT_SECURED_IMAGE)
            sbOffset = SB_OFFSET;
         app_size = storage_get_section_data(INU_STORAGE_SECTION_DTB, (unsigned char *)(DDR_DTB_START_ADDRESS + sbOffset));
         app_size = storage_get_section_data(INU_STORAGE_SECTION_KERNEL, (unsigned char *)(DDR_KERNEL_START_ADDRESS+ sbOffset));
         if (sbOffset)
         {
            app_size = storage_get_section_data(INU_STORAGE_SECTION_CNN_LD, (unsigned char *)LRAM_BASE + 0x4000);
         }
         else
         {
            app_size = storage_get_section_data(INU_STORAGE_SECTION_CNN_LD, (unsigned char *)LRAM_BASE);
         }
         cnn_size = storage_get_section_data(INU_STORAGE_SECTION_CNN, (unsigned char *) (DDR_CNN_START_ADDRESS + sbOffset));

         //store backup at cave mem area. In case of secured image, skip SB offset and SB header
         cnnOffset = (getEfuseSecureControlData() != NOT_SECURED_IMAGE) ? (sbOffset + SB_HDR_SIZE) : 0;
         memcpy((void*)DDR_CNN_BCK_START_ADDRESS, (void*)DDR_CNN_START_ADDRESS + cnnOffset, cnn_size - SB_HDR_SIZE);
         bootsplInfo.cevaStartAddress = (UINT32) (DDR_CNN_BCK_START_ADDRESS);

         app_size = storage_get_section_data(INU_STORAGE_SECTION_APP, (unsigned char *) (DDR_APP_START_ADDRESS + sizeof(app_size) + sbOffset));
         *((uint32 *) (DDR_APP_START_ADDRESS)) = app_size;

         app_size = storage_get_section_data(INU_STORAGE_SECTION_ROOTFS, (unsigned char *) (DDR_INITRAMFS_START_ADDRESS + sbOffset + sizeof(app_size)));
         *((uint32 *) (DDR_INITRAMFS_START_ADDRESS)) = app_size;

         bootsplInfo.kernelStartAddress = (UINT32) (DDR_KERNEL_START_ADDRESS) + sbOffset;
         bootsplInfo.dtbStartAddress = (UINT32) (DDR_DTB_START_ADDRESS) + sbOffset;
         bootsplInfo.appStartAddress = (UINT32) (DDR_APP_START_ADDRESS) + sbOffset;
         bootsplInfo.initramfsStartAddress = (UINT32) (DDR_INITRAMFS_START_ADDRESS) + sbOffset;
         bootsplInfo.cnnStartAddress = (UINT32) (DDR_CNN_START_ADDRESS) + sbOffset;

         GME_SAVE_AND_RESTORE_5_VAL = DDR_APP_START_ADDRESS;
         if (sbOffset == 0)
            spl_disable_dcache();

         if (app_size != -1)
            spl_boot_linux();
}
#endif   //__NAND_FLASH__
      }
   }
}

void spl_usb_boot()
{
   unsigned int sbOffset=0;
   if (getEfuseSecureControlData() != NOT_SECURED_IMAGE)
     sbOffset = SB_OFFSET;
   bootsplInfo.invokeLinuxBootAddress = (UINT32) (spl_boot_linux);
   bootsplInfo.kernelStartAddress = (UINT32) (DDR_KERNEL_START_ADDRESS) + sbOffset;
   bootsplInfo.dtbStartAddress = (UINT32) (DDR_DTB_START_ADDRESS) + sbOffset;
   bootsplInfo.appStartAddress = (UINT32) (DDR_APP_START_ADDRESS) + sbOffset;
   bootsplInfo.initramfsStartAddress = (UINT32) (DDR_INITRAMFS_START_ADDRESS) + sbOffset;
   bootsplInfo.cnnStartAddress = (UINT32) (DDR_CNN_START_ADDRESS) + sbOffset;
   bootsplInfo.cevaStartAddress = (UINT32) (DDR_CNN_BCK_START_ADDRESS);
   GME_SAVE_AND_RESTORE_3_VAL = (UINT32) (&bootsplInfo);
   *(UINT32*) 0x2000000 = 0;
   *(UINT32*) DDR_CEVA_START_ADDRESS = 0;

   spl_log("bootspl: jump addr           %x\n", bootsplInfo.invokeLinuxBootAddress);
   spl_log("bootspl: kernel load addr    %x\n", bootsplInfo.kernelStartAddress);
   spl_log("bootspl: dtb load addr       %x\n", bootsplInfo.dtbStartAddress);
   spl_log("bootspl: app load addr       %x\n", bootsplInfo.appStartAddress);
   spl_log("bootspl: initramfs load addr %x\n", bootsplInfo.initramfsStartAddress);
   spl_log("bootspl: cnn load addr       %x\n", bootsplInfo.cnnStartAddress);
   spl_log("bootspl: cnn load addr (bck) %x\n", bootsplInfo.cevaStartAddress);
#ifndef _ZEBU
   //Wait for host to write linux and filesystem to DDR
   spl_log("bootspl: waiting...\n");
   //write state to GME register for host to read
   GME_SAVE_AND_RESTORE_5_VAL = BOOTSPL_CMD_SPL_INIT_DONE;
   // When this function returns it enters process_bg_process loop in ROM
   if (sbOffset != 0)
   {
      do
      {
  //     spl_log("bootspl:waiting2\n");
      }while (GME_SAVE_AND_RESTORE_6_VAL != 0x1234);
      GME_SAVE_AND_RESTORE_5_VAL = DDR_APP_START_ADDRESS;
      spl_boot_linux();
   }
#else
   GME_SAVE_AND_RESTORE_5_VAL = DDR_APP_START_ADDRESS;
   spl_boot_linux();

#endif //_ZEBU
}

void spl_wd_init()
{
#ifdef __NAND_FLASH__
   *((uint32*)(WDOG_BASE_ADDRESS+WDOG_TIMEOUT_RANGE_REG_OFFSET))   = 0xDF;
#else
   *((uint32*)(WDOG_BASE_ADDRESS+WDOG_TIMEOUT_RANGE_REG_OFFSET))   = 0xDF;
#endif   //__NAND_FLASH__
   *((uint32*)(WDOG_BASE_ADDRESS+WDOG_COUNTER_RESTART_REG_OFFSET)) = WDOG_COUNTER_RESTART_KICK_VALUE;
   *((uint32*)(WDOG_BASE_ADDRESS+WDOG_CONTROL_REG_OFFSET))         = WDOG_CONTROL_REG_WDT_EN_MASK | WDOG_CONTROL_REG_WDT_RPL_MASK;
}

void spl_wd_deinit()
{
   spl_log("spl: %s(): Stop WD.\n",__func__);
   *((uint32*)(WDOG_BASE_ADDRESS+WDOG_CONTROL_REG_OFFSET))         = 0;
}

void spl_wd_reinit()
{
   spl_log("spl: %s(): Restart WD\n",__func__);
   *((uint32*)(WDOG_BASE_ADDRESS+WDOG_CONTROL_REG_OFFSET))         = WDOG_CONTROL_REG_WDT_EN_MASK | WDOG_CONTROL_REG_WDT_RPL_MASK;
}

static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum)
{
   OS_LYRG_mutexCpT* cpMutexP;

   /* check mutex num legality */
   if ( ( (INT32)mutexNum < 0 ) || ( (UINT32)mutexNum >= OS_LYRG_NUM_CP_MUTEX_E) )
   {
      spl_log("MUTEX Index error: mutexNum = %d \n", (UINT32)mutexNum);
      return 1;
   }
   cpMutexP = ((OS_LYRG_mutexCpT*)MEM_MAPG_CP_MUTEX_ADDRESS);
   cpMutexP = cpMutexP + mutexNum;
   *cpMutexhandleP = cpMutexP;

   if ( cpMutexP->indx == 0 )
      cpMutexP->indx = mutexNum;

   return 0;
}

static void spl_init_cp_log(UINT32 logBufferAddress, OS_LYRG_cpMutexIdE mutexNum)
{
   LOGG_cevaLogbufferT *cevaLogbufferP;

   cevaLogbufferP = (LOGG_cevaLogbufferT*)logBufferAddress;
   memset(cevaLogbufferP->logBuffer,0,sizeof(cevaLogbufferP->logBuffer));

   OS_LYRP_aquireMutexCp(&(cevaLogbufferP->cpMutex),mutexNum);
   cevaLogbufferP->rdBufferIndex = 0;
   cevaLogbufferP->wrBufferIndex = 0;
   cevaLogbufferP->state = LOGG_CEVA_LOG_STATE_EMPTY;
}

static void spl_boot_cnn_startArc( void )
{
   UINT32           reg = 0x1000000;

   //End of memory reset

   // BOOT the CNN
   PSS_ARC_CTRL_VAL = 0x44;   //c0 and c1_run
}

void evp_init(unsigned int evFreq)
{
   UINT32 arc0_intvbase_Addr = 0x80D0280 ;
   UINT32 arc1_intvbase_Addr = 0x80D0284 ;

#ifndef single
   // set arc0_intvbase to 0x2000000
   *(UINT32*)arc0_intvbase_Addr = 0x8000;  //was 0x80A4
   *(UINT32*)arc1_intvbase_Addr = 0x8000;
   *(UINT32*)0x02000000 = 0x02000060;
#else
   // set arc0_intvbase to 0x42000000
   *(UINT32*)arc0_intvbase_Addr = 0x107FFC;
   *(UINT32*)arc1_intvbase_Addr = 0x107FFC;
   *(UINT32*)0x41FFF000 = 0x41FFF060;
#endif

   BOARD_set_freq(BOARD_PLL_DSP_E, evFreq, 0);
   //while(!GME_DSP_PLL_STATUS_LOCK_R == 0);
   GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_W(1);
   while(GME_FRQ_CHG_STATUS_DSP_FULL_FREQ_CHG_DONE_R == 0);
   GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_W(0);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FULL_FREQ_CHG_DONE_W(1);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FULL_FREQ_CHG_DONE_W(0);

    // from evp_init()
   GME_POWER_MODE_DSP_POWER_UP_W(1);
   GME_CONTROL_START_POWER_CHANGE_W(1);
   while(!GME_POWER_MODE_STATUS_DSP_POWER_UP_R);
   GME_CONTROL_START_POWER_CHANGE_W(0);

   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);    // The DSP CLK EN is 1 by default (for the lram) / DSPA clock is for the processor/memories
   while(!GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R);


}

void spl_boot_cnn(void)
{
   UINT32 bootmailBoxAddr = 0x2001000;
   unsigned int evFreq = BOARD_DSP_CLK_M_HZ;
   INU_BOOT_boardConfigT *bootCfg = BOOTSPLB0_C0_PARAM_ADDRESS;

   if (bootCfg->magicNum == BOOTSPLB0_C0_PARAM_MAGIC)
   {
       if (bootCfg->param[INU_BOOT_PARAM_DSPFREQ] != 0xffffffff)
           evFreq = bootCfg->param[INU_BOOT_PARAM_DSPFREQ];
   }

   evp_init(evFreq);
   //zero error and done
   *(UINT32*)bootmailBoxAddr = 0;
   *(UINT32*)(bootmailBoxAddr + 4) = 0;

   //init log buffer
   spl_init_cp_log(MEM_MAPG_EV61_LOG_ADDRESS,OS_LYRG_CP_MUTEX_EV61_LOG_E);

   spl_boot_cnn_startArc();
}

#if 0
void spl_boot_ceva()
{
   UINT32 copierSize,tmp;
   UINT32 *tcmAddr, *imgAddr, *bypass;
//   UINT32 imageSize;
   unsigned int dspFreq=BOARD_DSP_CLK_M_HZ;
   unsigned int audioFreq=BOARD_AUDIO_CLK_M_HZ;
   INU_BOOT_boardConfigT *bootCfg=BOOTSPLB0_C0_PARAM_ADDRESS;

   if (bootCfg->magicNum == BOOTSPLB0_C0_PARAM_MAGIC)
   {
       if (bootCfg->param[INU_BOOT_PARAM_DSPFREQ] != 0xffffffff)
           dspFreq = bootCfg->param[INU_BOOT_PARAM_DSPFREQ];
   }

   //reset ceva
   GME_SW_RESET_0_SW_DSPA_RESET_N_W(1);

   //tcmAddr = (UINT32*)TCM_ADDRESS;
   //copierSize = *(UINT32*)(DDR_CEVA_START_ADDRESS+4);
   //spl_log("copierSize = %d\n",copierSize);
   //memcpy(tcmAddr,(UINT32*)(DDR_CEVA_START_ADDRESS+8),copierSize);
   //spl_log("copy TCM done\n");
   //imgAddr = (UINT32*)(DDR_CEVA_START_ADDRESS + 8 + copierSize + 4);
   //imageSize = *(UINT32*)(DDR_CEVA_START_ADDRESS + 8 + copierSize);
   //spl_log("imageSize %d imgAddr = %x\n",imageSize,imgAddr);
   //tcmAddr += CEVA_BOOTP_BOOT_PARAMS_OFFSET;
   //*tcmAddr++ = 0;
   //*tcmAddr++ = (UINT32)imgAddr;
   //*tcmAddr++ = 0;
   //spl_log("finished preperaing TCM\n");

   //BOARD_set_freq(BOARD_PLL_DSP_E, dspFreq, 0);
   BOARD_set_freq(BOARD_PLL_DSP_E, audioFreq, 0);


   // - the following was taken from inside pll set in the application
   GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_W(1);
   while(GME_FRQ_CHG_STATUS_DSP_FULL_FREQ_CHG_DONE_R == 0);
   GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_W(0);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FULL_FREQ_CHG_DONE_W(1);
   GME_FRQ_CHG_STATUS_CLEAR_DSP_FULL_FREQ_CHG_DONE_W(0);
   // - end

   //spl_log("finished configuring pll\n");
   BOARD_setPowerMode(BOARD_POWER_DSP_E);
   //spl_log("finished power mode\n");

   //reset ceva boot flag to 0 (CEVA_BOOTG_bootSyncParamsT)
   //*(UINT32*)0x0205FFFC = 0;

   //Update XM4 with with the boot_loader code address
  // *(UINT32*)0x80D0268 = TCM_ADDRESS;//Update PSS_DSP_VECTOR_ADDRESS_W with the boot_loader code address
   //NU4000 A0 bypass, need to write to 0x80D0260 in order to use vector register
   bypass = (UINT32*)0x80D0260;
   tmp = *bypass;
   *bypass = tmp;

   //init log buffer
   //spl_init_cp_log(MEM_MAPG_CEVA_LOG_ADDRESS,OS_LYRG_CP_MUTEX_CEVA_LOG_E);

   //enable the CPU and AXI clocks
   GME_CLOCK_ENABLE_DSP_CLK_EN_W(1);      // enable dsp clock for AXI access
   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);     // enable dsp cpu clock
   while(GME_CLOCK_ENABLE_DSP_CLK_EN_R == 0);
   while(GME_CLOCK_ENABLE_DSPA_CLK_EN_R == 0);
}
#endif // 0

void spl_trigger_wd()
{
   //Simple function to generate a system reset via the GPP WDT (which is connected to GME)

   //First set Timeout range register to minimum - which is 2^(16+i) where i is 0.
   //At 24MHz APB clk - this gives a 2.73msec timeout.
   //APB_WDTP_VAL(APB_WDTP_TORR) = 0;

   *((uint32*)(WDOG_BASE_ADDRESS + WDOG_TIMEOUT_RANGE_REG_OFFSET)) = 0;

   //Set Control Register: Bit 0 - enable, Bit 1 - system reset mode, Bit 2-4 - 2pclk cycles reset
   //APB_WDTP_VAL(APB_WDTP_CR) = 1;
   *((uint32*)(WDOG_BASE_ADDRESS + WDOG_CONTROL_REG_OFFSET)) = 1;
}

void spl_hard_reset()
{
   spl_trigger_wd();
   while(1){asm volatile("nop");};
}

unsigned char validateSecurityNumber(unsigned int sectionAddress, unsigned int sectionDataSize, unsigned char securityVersion, unsigned char checkSecurityVersionUpdate)
{
   unsigned char ret = 0;
   InuSecurityFooterT inuSecurityFooter = {0};

   memcpy(&inuSecurityFooter, (unsigned int*)(sectionAddress + sectionDataSize - sizeof(InuSecurityFooterT)), sizeof(InuSecurityFooterT));

   if (securityVersion > inuSecurityFooter.securityVersion)
   {
      spl_log("spl: ERROR! Security Version in eFuse [%d] is larger than Section's Security Version.\n", securityVersion, inuSecurityFooter.securityVersion);
      ret = 1;
   }
   else if (checkSecurityVersionUpdate && (securityVersion > inuSecurityFooter.securityVersion))
   {
      spl_log("spl: fwVersion [%x] securityVersion = %d eFuse.securityVersion = %d.\n", inuSecurityFooter.fwVersion, inuSecurityFooter.securityVersion, securityVersion);
      updateSecurityVersion   = 1;
      securityVersionG         = inuSecurityFooter.securityVersion;
   }

   return ret;
}

void spl_validate_secure_boot_sections()
{
   SB_Result_t result;
   unsigned char ret = 0;
   unsigned int size;
   securityVersionG = getEfuseSecurityNumber();

   spl_log("****************************secureBootOn\n");

   size = getUnsecuredImgLen(bootsplInfo.dtbStartAddress);
   result = startSecureBoot(bootsplInfo.dtbStartAddress, SB_OFFSET, 1);
   ret = validateSecurityNumber(bootsplInfo.dtbStartAddress - SB_OFFSET, size, securityVersionG, 0);
   if (result || ret)
   {
      spl_log("*** Failed to validate DTS Section. Aborting\n");
      goto HANDLE_ERROR;
   }

   size = getUnsecuredImgLen(bootsplInfo.kernelStartAddress);
   result = startSecureBoot(bootsplInfo.kernelStartAddress, SB_OFFSET, 1);
   ret = validateSecurityNumber(bootsplInfo.kernelStartAddress - SB_OFFSET, size, securityVersionG, 0);
   if (result || ret)
   {
      spl_log("*** Failed to validate Kernel Section. Aborting\n");
      goto HANDLE_ERROR;
   }

   // verify the CNN_LD
   size = getUnsecuredImgLen(LRAM_BASE + 0x4000);
   result = startSecureBoot(LRAM_BASE + 0x4000, 0x4000, 1);
   ret = validateSecurityNumber(LRAM_BASE, size, securityVersionG, 0);
   if (result|| ret)
   {
      spl_log("*** Failed to validate CNN_LD Section. Aborting\n");
      goto HANDLE_ERROR;
   }

   size = getUnsecuredImgLen(bootsplInfo.initramfsStartAddress + sizeof(size));
   result = startSecureBoot(bootsplInfo.initramfsStartAddress + sizeof(size), SB_OFFSET, 1);
   ret = validateSecurityNumber(bootsplInfo.initramfsStartAddress + sizeof(size) - SB_OFFSET, size, securityVersionG, 0);
   if (result || ret)
   {
      spl_log("*** Failed to validate initramfs Section. Aborting\n");
      goto HANDLE_ERROR;
   }
   *((unsigned int *)(bootsplInfo.initramfsStartAddress - SB_OFFSET)) = size;

   size = getUnsecuredImgLen(bootsplInfo.appStartAddress + sizeof(size));
   result = startSecureBoot(bootsplInfo.appStartAddress + sizeof(size), SB_OFFSET, 1);
   ret = validateSecurityNumber(bootsplInfo.appStartAddress + sizeof(size) - SB_OFFSET, size, securityVersionG, 0);
   if (result || ret)
   {
      spl_log("*** Failed to validate App Section. Aborting\n");
      goto HANDLE_ERROR;
   }
   *((unsigned int *)(bootsplInfo.appStartAddress - SB_OFFSET)) = size;

   size = getUnsecuredImgLen(bootsplInfo.cnnStartAddress);
   result = startSecureBoot(bootsplInfo.cnnStartAddress, SB_OFFSET, 1);
   // Also run check Security Version Update, to see if need to Update the eFuse's Security Version.
   ret = validateSecurityNumber(bootsplInfo.cnnStartAddress - SB_OFFSET, size, securityVersionG, 1);
   if (result || ret)
   {
      spl_log("*** Failed to validate CNN Section. Aborting\n");
      goto HANDLE_ERROR;
   }

   spl_log("**************************** All sections were validated successfully.\n");
   return;

HANDLE_ERROR:
   storage_update_failure_flash_flags();
   abort_log("Restart\n");
   //spl_hard_reset();
}

/*
* Host will force jump to this function when it detects DDR init done after
* loading the ddr code
*/

void spl_boot_linux()
{
   kernel_entry_arg_t kernel_entry;
   void *dtb_arg;
   UINT32 magicNum;
   FwUpdateE updateState;
   unsigned char efuseSecureControlData = getEfuseSecureControlData();
    //early init, need to reset CP mutex
   memset((UINT32*) MEM_MAPG_CP_MUTEX_ADDRESS, 0x00, 100);

   //check if Secure Boot exists
   if (efuseSecureControlData != NOT_SECURED_IMAGE)
   {
      spl_validate_secure_boot_sections();
   }

   spl_wd_deinit();

   // Copy Currnt Partition to Other Partition
   updateState = storage_copy_flash_info_to_other_partition();

   if (efuseSecureControlData != NOT_SECURED_IMAGE)
   {
      //In case FW UPDATE ended successfully and the module is in Secured Mode, Need to check if eFuse's Security version should be updated.
      if (updateSecurityVersion && (updateState == INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS))
      {
         writeEfuseSecurityNumber(securityVersionG);
      }
   }

   spl_wd_reinit();

   magicNum = *(UINT32*) LRAM_START_ADDRESS;

   UINT32 arc0_intvbase_Addr = 0x80D0280 ;
   UINT32 arc1_intvbase_Addr = 0x80D0284 ;

   // set arc0_intvbase to 0x2000000
   *(UINT32*)arc0_intvbase_Addr = 0x8000;  //was 0x80A4
   *(UINT32*)arc1_intvbase_Addr = 0x8000;

   // high start address should be the same as magic
   if (magicNum == 0x2000060)
   {
      spl_log("cnn image detected, perform boot\n");
      spl_boot_cnn();
   }


   BOARD_print_freq();
   /*
    * Boot Linux from DDR
    */
   spl_log("bootspl: %s begin\n", __func__);
   dtb_arg = (void *) DDR_DTB_START_ADDRESS;
   kernel_entry = (kernel_entry_arg_t) DDR_KERNEL_START_ADDRESS;

   spl_log("bootspl: cleanup before Linux...\n");
   cleanup_before_linux();

   spl_log("bootspl: jump to Linux (entry=%x, mach_type=%x, dtb_arg=%x)...\n\n", DDR_KERNEL_START_ADDRESS, INUITIVE_MACH_TYPE, (unsigned int) dtb_arg);
   kernel_entry(0, INUITIVE_MACH_TYPE, dtb_arg);
}

unsigned int getAudioFreq()
{

    return audioFreqG;

}

void spl_set_freq()
{
   unsigned int sysMHzFreq=BOARD_SYS_CLK_M_HZ,cpuMHzFreq=BOARD_CPU_CLK_M_HZ,cpuFraq=BOARD_CPU_CLK_FRAQ;
   INU_BOOT_boardConfigT *bootCfg=BOOTSPLB0_C0_PARAM_ADDRESS;
   audioFreqG = BOARD_AUDIO_CLK_M_HZ;

   if (bootCfg->magicNum == BOOTSPLB0_C0_PARAM_MAGIC)
   {
       if (bootCfg->param[INU_BOOT_PARAM_SYSFREQ] != 0xffffffff)
           sysMHzFreq = bootCfg->param[INU_BOOT_PARAM_SYSFREQ];
       if (bootCfg->param[INU_BOOT_PARAM_CPUFREQ] != 0xffffffff)
       {
           cpuMHzFreq = bootCfg->param[INU_BOOT_PARAM_CPUFREQ];
           cpuFraq = 0;
      }
       if (bootCfg->param[INU_BOOT_PARAM_CPUFRAQ] != 0xffffffff)
           cpuFraq = bootCfg->param[INU_BOOT_PARAM_CPUFRAQ];
      if (bootCfg->param[INU_BOOT_PARAM_AUDIOFREQ] != 0xffffffff)
          audioFreqG = bootCfg->param[INU_BOOT_PARAM_AUDIOFREQ];

   }
   board_spi_init();

   //GME_SYS_CLOCK_CONFIG_USB_SYS_CLK_DIV_W(6); //clock divider set to 6, means division by 2 (according to Amit's mail, due to potential bug in the USB AXI clock divider)
   //board_changeUnitFreq(GMEG_HW_UNIT_USB_SYS_E);
   BOARD_set_freq(BOARD_PLL_SYS_E,sysMHzFreq, 0);
   GMEG_setSysClockSrc(GMEG_SYS_CLK_SRC_PLL);
   board_changeUnitFreq(GMEG_HW_UNIT_SYS_E);
   BOARD_set_freq(BOARD_PLL_CPU_E,cpuMHzFreq,cpuFraq);
   GMEG_setGppClockSrc(GMEG_CPU_CLK_SRC_PLL);
   board_changeUnitFreq(GMEG_HW_UNIT_GPP_E);
   //set audio clock as source clock
   BOARD_set_freq(BOARD_PLL_AUDIO_E, audioFreqG, 0);
   while(!GME_AUDIO_PLL_STATUS_LOCK_R == 0);
   GME_AUDIO_CLOCK_CONFIG_CLK_DIV_W(1);
   GME_AUDIO_CLOCK_CONFIG_CLK_SRC_W(1);
   GME_CLOCK_ENABLE_AUDIO_CLK_EN_W(1);

   board_changeUnitFreq(GMEG_HW_UNIT_AUDIO_E);
   BOARD_setPowerMode(BOARD_POWER_EVP_E);
}

void read_mr()
{
   unsigned int ind,channel;
   unsigned int mr_bitmap=0x00004800; //0x030C59F1;
   for (channel=0;channel<2;channel++)
   {

      for (ind=0;ind<32;ind++)
      {
          if (mr_bitmap&(1<<ind))
          {
              ddrm_umctl_read_mrr(ind,channel);
          }
      }
   }
}

void setHpFlashFlag()
{
  volatile unsigned int *regPtr = 0x8020468;
  *regPtr = 0x33333333;
}

void set_gpios()
{
//#define Camera_1V8_EN (30)//DOVDD
//#define Camera_1V2_EN (54)//DVDD
//#define Camera_VAA_2V8_EN (29)//AVDD

    volatile unsigned int *gpio_ddr=(volatile unsigned int *)0x80b0004;
    volatile unsigned int *gpio_dr=(volatile unsigned int *)0x80b0000;
    volatile unsigned int *gpio_ddr_1=(volatile unsigned int *)0x80b8004;
    volatile unsigned int *gpio_dr_1=(volatile unsigned int *)0x80b8000;
    *gpio_ddr |= 1<<29;
    *gpio_ddr |= 1<<30;
    *gpio_dr |= 1<<29;
    *gpio_dr |= 1<<30;
    *gpio_ddr_1 |= 1<<22;
    *gpio_dr_1 |= 1<<22;
    spl_log("bootspl: setting gpios TCL");

}

#define BSS_INIT                        \
do{                                     \
   extern unsigned int __app_bss_start__;   \
   extern unsigned int __app_bss_end__;     \
   memset((void *)&__app_bss_start__, 0 , (unsigned int)&__app_bss_end__ - (unsigned int)&__app_bss_start__); \
   spl_log("bootspl: bss done\n");\
}while(0)

void spl_init()
{
   BSS_INIT;
//   if (*(GME_SAVE_AND_RESTORE_0_REG + OFFSET_LOAD_FROM_FLASH) == 1)
//      spl_enable_dcache();
   board_set_mode(BOARD_CLK_PLL);
  // BOARD_print_freq();
#ifndef FPGA_BOARD
   GME_SW_RESET_0_SW_UART0_RESET_N_W(0);
   spl_set_freq();
//#define DEBUG_CLK
#ifdef DEBUG_CLK
   GME_IO_MUX_CTRL_0_IO_MUX_CTRL_5_W(2);            // set clock out
   GME_DEBUG_CLK_CONTROL_CLK_DIV_W(16);
   GME_DEBUG_CLK_CONTROL_CLK_SRC_SEL_W(5);// 5 cpu 6  sys_clk
   GME_DEBUG_CLK_CONTROL_DIV_EN_W(1);
#endif
#endif
   // Enable USB controller and init HIE divider before enabling PLL
   // Set sys/cpu/lram frequency, according to the selected mode
   //   board_clks_set();
#ifdef BOOT_400
   set_gpios();
#endif
   spl_wd_init();
   spl_board_lowlevel_init();
   spl_board_init();
   BOARD_setPowerMode(BOARD_POWER_IAE_E);
   BOARD_setIaeClk(1);   // in order to initialize properly DDR port of ISP, we need IAE clock during DDR init
#if !defined(BOOT_51) && !defined (BOOT_500)
   //storage_init();
   pm_init();
   if (pm_boot_from_pd())
   {
      ddr_power_down_resume();
   }
   else
   {
      spl_ddr_init();
   }
#else
   spl_ddr_init();
#endif
   BOARD_setIaeClk(0);
   read_mr();
   fillSBEfuseDB();
#if defined(BOOT_86) || defined (BOOT_87)
   setHpFlashFlag();
#endif
#if !defined(BOOT_51) && !defined (BOOT_500)
   spl_pm();
#endif
   spl_spi_boot();
   spl_usb_boot();
}

__attribute__((__section__(".boot")))

void call_spi_init(void) {
	spl_init();
}

unsigned int ddrm_umctl_read_mrr (unsigned int mr_addr, unsigned int channel)
  {

   int mrstat_busy;
   int mrr_valid;
   int mrr_value;
   unsigned int channel_bit0;

   channel_bit0 = (channel & 0x00000001);

//   inu_printf("MR READ START\n");

   // Selecting the channel in the GME

   GME_DEBUG_DEBUG_BITS_W (channel_bit0)

   // Poll MRSTAT.mr_wr_busy until it is ‘0’. This checks that there is no outstanding MR transaction. No
   // writes should be performed to MRCTRL0 and MRCTRL1 if MRSTAT.mr_wr_busy = 1.

   mrstat_busy = 1;
   while (mrstat_busy)
   {
    mrstat_busy =  DDRC_MRSTAT_MR_WR_BUSY_R;
    }


//   inu_printf("MR READ STAGE 1\n");

   // Write the MRCTRL0.mr_type (1:READ 0:WRITE)
   // MRCTRL1.mr_data to define the MR transaction.
   // MRCTRL1[15:0] are interpreted as
   // [15:8] MR Address
   // [7:0]  MR data for writes, don't care for reads.

   DDRC_MRCTRL0_MR_TYPE_W (0x01);
   DDRC_MRCTRL1_MR_DATA_W (mr_addr<<8);
   DDRC_MRCTRL0_MR_WR_W (0x1);

   mrstat_busy = 1;
   while (mrstat_busy)
   {
    mrstat_busy =  DDRC_MRSTAT_MR_WR_BUSY_R;
//    inu_printf("MRSTAT Busy !!!\n");
    }

//   inu_printf("MR READ STAGE 2\n");

   mrr_valid = 0;

   while (!mrr_valid)
   {
    mrr_valid = GME_DDR_MRR_MRR_VALID_R;
    }


//   inu_printf("MR READ STAGE 3\n");

   mrr_value = GME_DDR_MRR_MRR_VALUE_R;
   spl_log("MR READ channel %d: VALUE of MR%d is 0x%02x\n", channel_bit0, mr_addr, mrr_value);

   GME_DDRM_CONTROL_MRR_VALID_CLR_W(0x1);
   GME_DDRM_CONTROL_MRR_VALID_CLR_W(0x0);
 //  inu_printf("MR READ DONE\n");
   return (mrr_value);

}
