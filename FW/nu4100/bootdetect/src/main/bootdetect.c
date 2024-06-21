/* This code performs board_end() and prints some debug prints */
#include "os_defs.h"
#include "common.h"
#include "nu4000_c0_gme_regs.h"
#include "gme.h"
#include "inu_storage_layout.h"
#include "inu_storage.h"
#include "inu_boot_common.h"

/**********************************************
 * local defines
unsigned short spi_flags = 0;
***********************************************/
#define BOOTFLASH_STATUS_READY   (0)
#define BOOTFLASH_STATUS_ACTIVE  (1)
#define BOOTFLASH_STATUS_FINISH  (2)
#define BOOTDETECT_STATUS_FINISH  (BOOTFLASH_STATUS_FINISH)

#define LRAM_START (0x2000000)
// production data must be in LRAM  in order to survive suspend resume cycle in bootfix
//#define DDR_TEST

int spi_connection_init=0;
#define SPI_BLOCK_SIZE (64*1024)
#define SPI_PAGE_SIZE 	256

/**********************************************
 * functions
***********************************************/
 
int spiflash_read(unsigned int spi_offset, unsigned char *spi_data_buf, unsigned int spi_data_size)
{
	return SPI_FLASHG_Read(spi_offset, spi_data_size, spi_data_buf);	
}

void bootdetect_spi_init(void)
{

   GMEG_enableClk(GMEG_HW_UNIT_SPI_E);
   GMEG_setFcuClockDiv(2);
   GMEG_changeUnitFreq(GMEG_HW_UNIT_SPI_E);
}

void SetGpioByVersion(void)
{

    volatile unsigned int *gpioRegAddr,*gpioDdrRegAddr,*pinMuxReg;
    unsigned int vers,count;
    GME_IO_MUX_CTRL_4_IO_MUX_CTRL_6_W(1);
    GME_IO_MUX_CTRL_4_IO_MUX_CTRL_7_W(1);

    for(count=0;count<1000;count++)
    {
       /* prevent compiler optimization */
       if(count > 800)
           count++;
    }


    gpioRegAddr = 0x80b8050;
    gpioDdrRegAddr = 0x80b8004;

    *gpioDdrRegAddr &= 0xfffffff9; // gpio 33 34
    vers = (*gpioRegAddr&0x6)>>1;
    debug_printf("vers1 %d gpio ddr addr %x %x dr addr %x %x\n",vers,gpioDdrRegAddr,*gpioDdrRegAddr,gpioRegAddr,*gpioRegAddr);
    if (vers != 1)
    {
        GME_IO_MUX_CTRL_3_IO_MUX_CTRL_3_W(1);
        GME_IO_MUX_CTRL_3_IO_MUX_CTRL_4_W(1);

        gpioRegAddr = 0x80b0000;
        gpioDdrRegAddr = 0x80b0004;

        *gpioDdrRegAddr &= 0xff3fffff;// gpio 22,23
        *gpioDdrRegAddr |= 0xc00000;
        *gpioRegAddr &= 0xff3fffff;
        *gpioRegAddr |= 0xc00000;
    }
}

__attribute__((__section__(".boot")))
void bootdetect_init()
{
   uint32 ret=0;
   uint32 magic_number=0;
   unsigned char *prod_data_address=(unsigned char *)BOOTDETECT_PRODUCTION_DATA_ADDRESS;
   InuSectionHeaderT  sectoinHeader;
   InuProductionHeaderT *prodHeader;
   spi_connection_init=0;
   debug_printf ("-------------------------------\n");
   debug_printf ("bootdetect: version=%d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION);
   debug_printf ("-------------------------------\n");

   board_set_mode(BOARD_CLK_PLL);

   SetGpioByVersion();
   bootdetect_spi_init();
   SPI_FLASHG_init(0,0,get_board_info()->spi_freq_hz);
   
   // use spiflash_read to the production data and store to @BOOTDETECT_PRODUCTION_DATA_ADDRESS + 4, keep valid magic at @BOOTDETECT_PRODUCTION_DATA_ADDRESS
   // mark layout invalid by default
   *((uint32 *)prod_data_address)=0;
   
   ret = spiflash_read(0, (unsigned char *)&magic_number, sizeof(UINT32));
   debug_printf("bootdetect: magic number %x\n",magic_number);
   if (magic_number==LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER)
   {      
      ret = spiflash_read(0, prod_data_address+sizeof(UINT32), sizeof(InuStorageLegacyLayoutT));
      if (ret==0)
      {
		  *((uint32 *)prod_data_address)=LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER;
		  debug_printf("bootdetect: LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER flash read succeeded\n");
		  GME_SAVE_AND_RESTORE_1_VAL = BOOTDETECT_STATUS_FINISH;
	      // When this function returns it enters process_bg_process loop in ROM
	      return;
      }	  
   }
   
   ret = spiflash_read(SECTION_PRODUCTION_START_BLOCK*SPI_BLOCK_SIZE, (unsigned char *) &sectoinHeader, sizeof(InuSectionHeaderT));
   if (ret==0)
   {
		if (sectoinHeader.magicNumber == SECTION_PRODUCTION_MAGIC_NUMBER)
		{
			ret = spiflash_read(SECTION_PRODUCTION_START_BLOCK*SPI_BLOCK_SIZE+sizeof(InuSectionHeaderT), prod_data_address+sizeof(UINT32), sizeof(InuProductionHeaderT));
			if (ret==0)
			{
				*((uint32 *)prod_data_address)=BOOTROM_STORAGE_NU3000_MAGIC_NUMBER;
				prodHeader = (InuProductionHeaderT *)(prod_data_address+sizeof(UINT32));
				debug_printf("bootdetect: BOOTROM_STORAGE_NU3000_MAGIC_NUMBER flash read succeeded bootId %d\n",prodHeader->bootId);
			}
		}
    }
   GME_SAVE_AND_RESTORE_1_VAL = BOOTDETECT_STATUS_FINISH;
   // When this function returns it enters process_bg_process loop in ROM
   return;
}


