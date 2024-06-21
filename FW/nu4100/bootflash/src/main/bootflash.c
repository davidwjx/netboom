/* This code performs board_end() and prints some debug prints */

#include "os_defs.h"
#include "common.h"
#include "nu4000_c0_gme_regs.h"
#include "spi_flash.h"
#include "gme.h"
#include "flash_info.h"
#include "bootflash.h"
#include "spi-nand-driver.h"
#include "ispi.h"

#define SPI_FLASH_SUCCESS  (0)
#define SPI_FLASH_FAIL     (1)
void board_spi_init();
void setGpioForCs3(void);
void resetGpioForCS3(void);

extern const struct flash_info spi_nor_ids[];
extern UINT32 spi_nor_ids_arr_size;
//#define EFUSE_WRITE_DISABLE 0

//#define DDR_TEST
static unsigned int nand_flash_present = 0;

int spi_connection_init=0;
int eeprom_connection_init=0;
int efuse_connection_init=0;
unsigned int spi_clk_div=1;

unsigned int spi_erase_size = SPI_BLOCK_SIZE;
unsigned int spi_num_sectors =  128;
unsigned short spi_flags = 0;
unsigned char *sector_copy_data=(unsigned char *)0x2040000;
bootflash_data_t *boot_data = (bootflash_data_t *)0x202ff00;

#define BOOTFLASH_STATUS_READY      (0)
#define BOOTFLASH_STATUS_ACTIVE     (1)
#define BOOTFLASH_STATUS_FINISH     (2)

#define BOOTFLASH_COMMAND_READ         (1)
#define BOOTFLASH_COMMAND_WRITE        (2)
#define BOOTFLASH_COMMAND_INFO         (3)
#define BOOTFLASH_COMMAND_ERASE        (4)
#define BOOTFLASH_COMMAND_NAND_READ    (5)
#define BOOTFLASH_COMMAND_NAND_WRITE   (6)
#define BOOTFLASH_COMMAND_NAND_ERASE   (7)

#define BOOTFLASH_STORAGE_SPIFLASH  (0)
#define BOOTFLASH_STORAGE_EEPROM (1)
#define BOOTFLASH_STORAGE_EFUSE     (2)
#define BOOTFLASH_STORAGE_NAND      (3)
#define BOOTFLASH_STORAGE_SPI       (4)
#define BOARD_OSC_CLK_HZ (24000000U)

static spinand_io_param_t io;

typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
        refdiv   :6,
        fbdiv    :12,
        unused   :4,
        postdiv1 :3,
        postdiv2 :3,
        dac      :1,
        dsm      :1;
} BOARD_pllControlT;



static unsigned int BOARD_calc_sys_pll()
{

   unsigned int freq;

   BOARD_pllControlT pllControl;

   *((unsigned int *)&pllControl) = GME_SYS_PLL_STATUS_VAL;

   // no supprt for fraction mode
   freq = ((((UINT32 )(BOARD_OSC_CLK_HZ/pllControl.refdiv))*(pllControl.fbdiv))/pllControl.postdiv1)/pllControl.postdiv2;

   rel_log("sys freq %d\n",freq);
   return freq;
}

static unsigned int getNewFcuAndDiv(unsigned int spiFreq,unsigned int *spi_div)
{
   unsigned int sysFreq,fcu,minFcu;

   fcu = GME_SYS_CLOCK_CONFIG_FCU_CLK_DIV_R>>1;
   sysFreq = BOARD_calc_sys_pll();
   minFcu = sysFreq/spiFreq;
   minFcu = minFcu/1024;  // spi freq (sys_pll/2)/(fcu*(1+spi_div)*2)  max spi div 255

   if (minFcu%1000)
      minFcu+=1;
   if (minFcu > 7)
      minFcu =7;  // max value for Fcu
   else if (minFcu<fcu)
      minFcu=fcu; // don't change if not needed
   *spi_div = (sysFreq + spiFreq - 1)/spiFreq;
   *spi_div = (*spi_div + 3)/4;
   *spi_div = (*spi_div + minFcu - 1)/minFcu;
    if (*spi_div > 0)
      *spi_div -=1;
   if (*spi_div > 255)
      *spi_div=255;
   rel_log("spi new freq %d fcu %d spi div %d\n",sysFreq/(minFcu*(1+*spi_div)*4),minFcu,*spi_div);
   return minFcu;
}

void bootflash_spi_init(void)
{
   unsigned int fcu;


   rel_log("boot_data freq %d spi num %d cs %d\n",boot_data->spi_freq,boot_data->spi_num,boot_data->spi_cs);
   if (boot_data->spi_freq != 0xffffffff)
   {
      fcu = getNewFcuAndDiv(boot_data->spi_freq,&spi_clk_div) << 1;
   }
   else fcu =4;
   GME_CLOCK_ENABLE_FCU_1_CLK_EN_W(1);
   while(GME_CLOCK_ENABLE_STATUS_FCU_1_CLK_EN_R == 0){};
   GMEG_enableClk(GMEG_HW_UNIT_SPI_E);
   GME_SYS_CLOCK_CONFIG_FCU_CLK_DIV_W(fcu);
   GMEG_changeUnitFreq(GMEG_HW_UNIT_SPI_E);
}

void init_spi_connection()
{  	
	UINT32 retVal,tmp,ind;
	UINT8			id[SPI_NOR_MAX_ID_LEN];
	const struct flash_info	*info;
   volatile unsigned int count=0;

   BOARD_calc_sys_pll();

	if (GME_SAVE_AND_RESTORE_2_VAL == BOOTFLASH_STORAGE_SPI)
	{
	   *((volatile unsigned int *)0x802018c)|=0x0100000;while (count++<0x2000) {};count=0;
	   *((volatile unsigned int *)0x80B8004)|=0x01;while (count++<0x2000) {};count=0;
	   *((volatile unsigned int *)0x80B8000)&=0xfffffffe;while (count++<0x2000) {};count=0;
	}
	bootflash_spi_init();
    if ((boot_data->spi_num == 0) && (boot_data->spi_cs == 3))
      setGpioForCs3();
	retVal = SPI_DRVG_open(0,0,get_board_info()->spi_freq_hz);
	if  ((retVal == SPI_FLASH_SUCCESS) && (GME_SAVE_AND_RESTORE_2_VAL == BOOTFLASH_STORAGE_SPIFLASH))
	{
		retVal = SPI_FLASHG_read_reg(SPINOR_OP_RDID,id,SPI_NOR_MAX_ID_LEN);
		debug_printf("FLASH version %x %x %x %x %x %x\n",id[0],id[1],id[2],id[3],id[4],id[5]);


		//for (tmp = 0; tmp < spi_nor_ids_arr_size - 1; tmp++)
		for (tmp = 0; tmp < spi_nor_ids_arr_size - 1; tmp++)
		{
			info = &spi_nor_ids[tmp];
			if (info->id_len)
			{
				for (ind=0;ind<info->id_len;ind++)
				{
					if (info->id[ind]!=id[ind])
						break;

				}
				if (ind==info->id_len)
				{
					spi_erase_size = info->sector_size;
					spi_num_sectors = info->n_sectors;
					spi_flags = info->flags;
					spi_flags &= ~(SPI_NOR_QUAD_READ|SPI_NOR_DUAL_READ);
					debug_printf("flash found name %s erase size 0x%x numb of sectors %d flags %x(bits:dual-6 quad-7 4bytes-11)\n",info->name,spi_erase_size,spi_num_sectors,spi_flags);
					break;
				}

			}
		}
		if (spi_flags & SPI_NOR_4B_OPCODES)
		    SPI_FLASHG_write_cmd(FLASH_EN4B_OPCODE);

	}
}

void init_efuse_connection(void)
{
   if (efuse_connection_init == 0)
   {
      debug_printf("%s %d\n",__FUNCTION__,__LINE__);
      //make sure programming disabled
      GME_FUSE_CONTROL_PROGRAM_ENABLE_W(0);
      //enable clock
      GME_PERIPH_CLOCK_ENABLE_FUSE_PROGRAM_CLK_EN_W(1);
      while(GME_PERIPH_CLOCK_ENABLE_STATUS_FUSE_PROGRAM_CLK_EN_R != 1);
      efuse_connection_init++;
      debug_printf("%s %d\n",__FUNCTION__,__LINE__);
    }
}

void close_efuse_connection(void)
{
   //disable clock
   GME_PERIPH_CLOCK_ENABLE_FUSE_PROGRAM_CLK_EN_W(0);
   while(GME_PERIPH_CLOCK_ENABLE_STATUS_FUSE_PROGRAM_CLK_EN_R == 1);
   efuse_connection_init = 0;
   debug_printf("%s %d\n",__FUNCTION__,__LINE__);
}

void efuse_write(uint32 offset,uint8 *buff,int32 size)
{
    unsigned int *efuse_data = (unsigned int *)buff;
   unsigned int ind;


#ifndef EFUSE_WRITE_DISABLE
   init_efuse_connection();
   for(ind = 0; ind < EFUSE_SIZE/4; ind++)
   {
      if(efuse_data[ind] != 0)
      {
         debug_printf("write... ind %d %x row %d ",ind,efuse_data[ind]);
         GME_FUSE_PROGRAM_VAL_VAL = efuse_data[ind];
         GME_FUSE_CONTROL_PROGRAM_ROW_ADDR_W(ind);//rows are 8-bits
         GME_FUSE_CONTROL_PROGRAM_ENABLE_W(0xffffffff);
         while(GME_GENERAL_STATUS_0_FUSE_PROGRAM_DONE_R != 1);

         GME_FUSE_CONTROL_PROGRAM_ENABLE_W(0);
         while(GME_GENERAL_STATUS_0_FUSE_PROGRAM_DONE_R != 1);

         debug_printf("done\n");

      }
     // add some delay for 2 ms
      system_udelay(1000*2);
   }
   close_efuse_connection();
#else
   for(ind = 0; ind < EFUSE_SIZE/4; ind++)
   {
      if (efuse_data[ind] != 0)
      {
        debug_printf("write... %d %x ",ind,efuse_data[ind]);
        debug_printf("done\n");
      }
   }
#endif

}



int32 spiflash_erase_and_write_block(uint32 write_addr,uint8 *buff,int32  size)
{
	int32 rc=SPI_FLASH_SUCCESS;
    uint32 block_ind,block_addr,block_offset,max_write_size=SPI_PAGE_SIZE,ind;           
	uint32 size_to_write;
	uint32 erase_size = spi_erase_size;

	while (size > 0)
	{

		block_ind = write_addr/erase_size;  
		block_addr=block_ind*erase_size;    
		block_offset = write_addr%erase_size;        


 		rc = SPI_FLASHG_Read(block_addr,spi_erase_size,sector_copy_data);
		if (rc == SPI_FLASH_FAIL)
		{
			debug_printf("fail: file read error\n");
			goto write_err;
		}
		size_to_write = erase_size - block_offset;
		if (size_to_write > (uint32)size)
			size_to_write = (uint32)size;
				
		memcpy(sector_copy_data+block_offset, buff, size_to_write);
		
		rc = SPI_FLASHG_blockErase(block_addr);	
		if (rc == SPI_FLASH_FAIL)
		{
			debug_printf("fail: flash erase error 0x%x\n",rc);
			goto write_err;
		}
		for (ind=0;ind<erase_size;ind+=max_write_size)
		{

			SPI_FLASHG_program(block_addr + ind, sector_copy_data + ind, max_write_size);
			if (rc == SPI_FLASH_FAIL)
			{
				debug_printf("fail: flash program error 0x%x\n",rc);
				goto write_err;
			}
		}
		buff+=size_to_write;
		write_addr+=size_to_write;
		size -= size_to_write;
	}

write_err:	
    return rc;
}
void spiflash_write(unsigned int spi_offset, unsigned char *spi_data_buf, unsigned int spi_data_size)
{
	int block_number, block_offset, page_number, page_offset, i;
//	init_spi_connection();
	debug_printf("bootflash: spi: writing %d bytes from address %x\n",spi_data_size, spi_data_buf); 
	
	/*for(i=0; i< 1024; i++)
		spi_data_buf[i] = 0xbb;
	spi_data_size = 1024;
	*/	
	block_offset = spi_offset/spi_erase_size;
	// erase flash 
	block_number = ((spi_offset + spi_data_size)%spi_erase_size == 0) ? (spi_offset+ spi_data_size)/spi_erase_size : (spi_offset+ spi_data_size)/spi_erase_size +1;
	for (i=block_offset; i< block_number; i++)
		if (SPI_FLASHG_blockErase(i*spi_erase_size) != 0) {
			debug_printf("bootflash: spi: block erase failed (address=%x)\n",i*spi_erase_size); 
			return;
		} else {
			debug_printf("bootflash: spi: block erase (address=%x) success\n",i*spi_erase_size); 	
	}
	
	// program flash page by page
	page_offset = spi_offset/SPI_PAGE_SIZE;
	page_number = (spi_data_size + page_offset)/SPI_PAGE_SIZE;
	for (i=page_offset; i< page_number; i++)	
		if (SPI_FLASHG_program(i*SPI_PAGE_SIZE,spi_data_buf + i*SPI_PAGE_SIZE, SPI_PAGE_SIZE) != 0) {
			debug_printf("bootflash: spi: block erase failed (address=%x)\n",i*spi_erase_size,SPI_PAGE_SIZE); 
			return;
		}
	// program last partial page if needed 
	if ((spi_data_size+page_offset)%SPI_PAGE_SIZE)
		if (SPI_FLASHG_program(i*SPI_PAGE_SIZE,spi_data_buf + i*SPI_PAGE_SIZE, (spi_data_size + page_offset)%SPI_PAGE_SIZE) != 0) {
			debug_printf("bootflash: spi: block erase failed (address=%x)\n",i*spi_erase_size,SPI_PAGE_SIZE); 
			return;
		}
	
	debug_printf("bootflash: write succeed...\n");	
}

int spiflash_read(unsigned int spi_offset, unsigned char *spi_data_buf, unsigned int spi_data_size)
{
	//init_spi_connection();
	return SPI_FLASHG_Read(spi_offset, spi_data_size, spi_data_buf);	
}


void read_flash_info(unsigned int command, unsigned char *spi_data_buf, unsigned int spi_data_size)
{
   unsigned int *info = (unsigned int *)spi_data_buf;

 //  init_spi_connection();

   info[0] = spi_erase_size;
   info[1] = spi_num_sectors;
   debug_printf("%s %d bootflash: read_flash_info succeed...init_spi_connection %d spi_erase_size %d spi_num_sectors %d\n",
		   __FILE__,__LINE__,spi_connection_init,spi_erase_size,spi_num_sectors);

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

void setGpioForCs3(void)
{

    volatile unsigned int *gpioRegAddr,*gpioDdrRegAddr;
    unsigned int count;
    gpioDdrRegAddr = 0x80b8004;
    *gpioDdrRegAddr |= 0x10000000; // gpio 60
    GME_IO_MUX_CTRL_8_IO_MUX_CTRL_1_W(1);

    for(count=0;count<1000;count++)
    {
       /* prevent compiler optimization */
       if(count > 800)
           count++;
    }


    gpioRegAddr = 0x80b8000;
    *gpioRegAddr |= 0x10000000;
 }

void resetGpioForCS3(void)
{

    volatile unsigned int *gpioRegAddr;
    gpioRegAddr = 0x80b8000;
    *gpioRegAddr &= 0xefffffff;
}

__attribute__((__section__(".boot")))
void bootflash_init()
{
	do
	{   
		extern unsigned int __app_bss_start__;	 
		extern unsigned int __app_bss_end__;	 
	   	memset((void *)&__app_bss_start__, 0 , (unsigned int)&__app_bss_end__ - (unsigned int)&__app_bss_start__);
	}while(0);

#ifdef DDR_TEST
	int i;
	unsigned int *ddr;
#endif
	unsigned int start_addr,last_addr,rx_addr,ind,size,storageType;
	static unsigned int cnt = 0;
	/* Init Debug UART for M3 */
#ifdef ENABLE_M3_DEBUG
//	board_dbg_uart_init(0);
//	debug_init(1);
#endif
	uint32 ret = 0;
	spi_connection_init = 0;

	debug_printf("-------------------------------\n");
	debug_printf("bootflash: version=%d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION,
			BUILD_VERSION);
	debug_printf("-------------------------------\n");

	debug_printf("bootflash: enable PLL.\n");
	//board_set_mode(get_strap_info()->PllClkMode);
	//board_gpp_clk_set();

	//board_timer_init();
	debug_printf("bootflash: enable timer clock.\n");
/*#ifdef EFUSE_WRITE_DISABLE
	debug_printf("bootflash: EFUSE write disable. SPI FLASH bit %x\n",
			GME_STRAP_PIN_STATUS_VAL);
#else
	debug_printf("bootflash: EFUSE write enable. SPI FLASH bit %x\n",
			GME_STRAP_PIN_STATUS_VAL);
#endif*/
	//Enable LRAM
	//GME_CLOCK_ENABLE_LRAM_CLK_EN_W(1);
	//while(GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R == 0);

	// status: 0 - ready, 1 - acting
	GME_SAVE_AND_RESTORE_0_VAL = 0;
	// 1- read, 2 write
	GME_SAVE_AND_RESTORE_1_VAL = 0;

	storageType = GME_SAVE_AND_RESTORE_2_VAL;
	// offset on storage
	GME_SAVE_AND_RESTORE_3_VAL = 0;
	// data buffer address
	GME_SAVE_AND_RESTORE_4_VAL = 0;
	// data buffer length
	GME_SAVE_AND_RESTORE_5_VAL = 0;

	SetGpioByVersion();
    *((volatile unsigned int *)0x8020188)=0;

	init_spi_connection();
	/*
	 Protocol description:
	 Host:
	 on each data chunk:
	 Wait for target ready (GME0 == 0)
	 Configure command (GME1,GME2,GME3,GME4,GME5)
	 Activate target to pending for commands (GME0 = 1)
	 Configure target to stop pending for commands (GME0 = 2)

	 Target:
	 Target ready (GME0 == 0)
	 Wait for host request ((GME0 == 1))
	 Command finish ready for another command (GME0=0)
	 Exit on receiving stop pending for commands (GME0 == 2)

	 */

	GME_SAVE_AND_RESTORE_0_VAL = BOOTFLASH_STATUS_READY;
	//stop loop
	while (GME_SAVE_AND_RESTORE_0_VAL != BOOTFLASH_STATUS_FINISH)
	{
		if ((cnt++ % 0x1000000) == 0)
			debug_printf("waiting %d\n", GME_SAVE_AND_RESTORE_0_VAL);
		// perform write request
		if (GME_SAVE_AND_RESTORE_0_VAL == BOOTFLASH_STATUS_ACTIVE)
		{

		   if (GME_SAVE_AND_RESTORE_2_VAL == BOOTFLASH_STORAGE_NAND)
		   {
		     if (nand_flash_present == 0)
		     {
		       ret = spinand_init();
		       if(ret == 0)
		       {
		         nand_flash_present = 1;
		       }
		     }
		    }
			if (GME_SAVE_AND_RESTORE_1_VAL == BOOTFLASH_COMMAND_INFO)
			{
				read_flash_info(GME_SAVE_AND_RESTORE_3_VAL,
						(unsigned char *) GME_SAVE_AND_RESTORE_4_VAL,
						GME_SAVE_AND_RESTORE_5_VAL);
				debug_printf("bootflash: %s %d %d\n", __FILE__, __LINE__,
				GME_SAVE_AND_RESTORE_0_VAL,
				GME_SAVE_AND_RESTORE_1_VAL);

			}
			else if (GME_SAVE_AND_RESTORE_1_VAL == BOOTFLASH_COMMAND_WRITE)
			{
				switch (GME_SAVE_AND_RESTORE_2_VAL)
				{
				case BOOTFLASH_STORAGE_SPIFLASH:
					debug_printf(
							"bootflash:performing spi flash write (offset = %x) from %x of %x bytes\n",
							GME_SAVE_AND_RESTORE_3_VAL,
							GME_SAVE_AND_RESTORE_4_VAL,
							GME_SAVE_AND_RESTORE_5_VAL);
					spiflash_erase_and_write_block(
					GME_SAVE_AND_RESTORE_3_VAL,
							(unsigned char *) GME_SAVE_AND_RESTORE_4_VAL,
							GME_SAVE_AND_RESTORE_5_VAL);
					GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
					break;
				case BOOTFLASH_STORAGE_NAND:
					if(nand_flash_present == 0)
					{
						debug_printf("NAND flash not found\n");
						return;
					}
					debug_printf(
							"bootflash:performing NAND flash write (offset = %x) from %x of %x bytes\n",
							GME_SAVE_AND_RESTORE_3_VAL,
							GME_SAVE_AND_RESTORE_4_VAL,
							GME_SAVE_AND_RESTORE_5_VAL);
					
					io.buf = (u8*)GME_SAVE_AND_RESTORE_4_VAL;
					io.offset = GME_SAVE_AND_RESTORE_3_VAL;
					io.length = GME_SAVE_AND_RESTORE_5_VAL;
			
					ret = spinand_write(&io);
					GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
					break;

            case BOOTFLASH_STORAGE_SPI:
              debug_printf(
                     "bootflash:performing spi  write (tx addr = %x)  rx addr %x tx/rx size %d size %d\n",
                     GME_SAVE_AND_RESTORE_3_VAL,
                     GME_SAVE_AND_RESTORE_4_VAL,
                     GME_SAVE_AND_RESTORE_5_VAL,
                     GME_SAVE_AND_RESTORE_7_VAL);
               size = GME_SAVE_AND_RESTORE_7_VAL/GME_SAVE_AND_RESTORE_5_VAL; // num of messages
               start_addr = GME_SAVE_AND_RESTORE_3_VAL;
               rx_addr =    GME_SAVE_AND_RESTORE_4_VAL;
               for (ind =0;ind<size;ind++)
               {

                  SPI_DRVG_spiTxRxTransaction((BYTE *)(start_addr + ind*4),
                                        GME_SAVE_AND_RESTORE_5_VAL,
                                        (BYTE *)(rx_addr + ind*4),
                                        GME_SAVE_AND_RESTORE_5_VAL);
                  volatile unsigned int count =0;
                  if ((((unsigned char *)start_addr)[ind*4] == 0x4C)) // add delay for line
                  {
                     while (count < 20000)
                     {
                        ++count;
                     }
                  }

               }
               GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
               break;
            case BOOTFLASH_STORAGE_EFUSE:

               debug_printf(
                     "bootflash: performing efuse write (offset = %x) from %x of %x bytes\n",
                     GME_SAVE_AND_RESTORE_3_VAL,
                     GME_SAVE_AND_RESTORE_4_VAL,
                     GME_SAVE_AND_RESTORE_5_VAL);

               efuse_write(GME_SAVE_AND_RESTORE_3_VAL,
                     (unsigned char *) GME_SAVE_AND_RESTORE_4_VAL,
                     GME_SAVE_AND_RESTORE_5_VAL);

               GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
               break;

				default:
//                     debug_printf("bootflash: default\n");
					break;
				}
			}
			else if (GME_SAVE_AND_RESTORE_1_VAL == BOOTFLASH_COMMAND_ERASE)
			{
				switch (GME_SAVE_AND_RESTORE_2_VAL)
				{
				case BOOTFLASH_STORAGE_SPIFLASH:
					debug_printf(
							"bootflash:performing spi flash erase (offset = %x) size=%x\n",
							GME_SAVE_AND_RESTORE_3_VAL,GME_SAVE_AND_RESTORE_5_VAL);

					start_addr =  GME_SAVE_AND_RESTORE_3_VAL;
					last_addr  =  GME_SAVE_AND_RESTORE_5_VAL + start_addr;
					while (start_addr < last_addr)
					{
						if (SPI_FLASHG_blockErase(start_addr) != 0)
						{
							debug_printf(
								"bootflash: spi: block erase failed (address=%x)\n",
								start_addr);
							return;
						}
						else
						{
							debug_printf("bootflash: spi: block erase (address=%x)success\n",
								start_addr);
						}
						start_addr += spi_erase_size;
					    debug_printf(
							"bootflash: (start = %x) end=%x\n",
							start_addr,last_addr);
					}

					GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
					break;
				case BOOTFLASH_STORAGE_NAND:
					if(nand_flash_present == 0)
					{
						debug_printf("NAND flash not found\n");
						return;
					}
					debug_printf(
							"bootflash:performing NAND flash erase (offset = %x) from %x of %x bytes\n",
							GME_SAVE_AND_RESTORE_3_VAL,
							GME_SAVE_AND_RESTORE_4_VAL,
							GME_SAVE_AND_RESTORE_5_VAL);
					io.buf = (u8*)GME_SAVE_AND_RESTORE_4_VAL;
					io.offset = GME_SAVE_AND_RESTORE_3_VAL;
					io.length = GME_SAVE_AND_RESTORE_5_VAL;
					ret = spinand_erase(&io);
					GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
					break;					

				default:
//                     debug_printf("bootflash: default\n");
					break;
				}
			}

			// perform read request
			else
			{
				switch (GME_SAVE_AND_RESTORE_2_VAL)
				{
				case BOOTFLASH_STORAGE_SPIFLASH:
					ret = spiflash_read(
					GME_SAVE_AND_RESTORE_3_VAL,
							(unsigned char *) GME_SAVE_AND_RESTORE_4_VAL,
							GME_SAVE_AND_RESTORE_5_VAL);
					// send return value
					GME_SAVE_AND_RESTORE_3_VAL = ret;
					if (ret == 0)
					{
						GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
					}
					break;
				case BOOTFLASH_STORAGE_NAND:
					if(nand_flash_present == 0)
					{
						debug_printf("NAND flash not found\n");
						return;
					}
					debug_printf(
							"bootflash:performing NAND flash read (offset = %x) from %x of %x bytes\n",
							GME_SAVE_AND_RESTORE_3_VAL,
							GME_SAVE_AND_RESTORE_4_VAL,
							GME_SAVE_AND_RESTORE_5_VAL);
					
					io.buf = (u8*)GME_SAVE_AND_RESTORE_4_VAL;
					io.offset = GME_SAVE_AND_RESTORE_3_VAL;
					io.length = GME_SAVE_AND_RESTORE_5_VAL;

					ret = spinand_read(&io);
					GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
					break;
				case BOOTFLASH_STORAGE_EEPROM:
					break;
				case BOOTFLASH_STORAGE_EFUSE:
					break;
            case BOOTFLASH_STORAGE_SPI:
               debug_printf(
                      "bootflash:performing spi  read (tx addr = %x)  rx addr %x tx/rx size %d size %d\n",
                      GME_SAVE_AND_RESTORE_3_VAL,
                      GME_SAVE_AND_RESTORE_4_VAL,
                      GME_SAVE_AND_RESTORE_5_VAL,
                      GME_SAVE_AND_RESTORE_7_VAL);
                 size = GME_SAVE_AND_RESTORE_7_VAL/GME_SAVE_AND_RESTORE_5_VAL; // num of messages
                 start_addr = GME_SAVE_AND_RESTORE_3_VAL;
                 rx_addr = GME_SAVE_AND_RESTORE_4_VAL;
                 for (ind =0;ind<size;ind++)
                 {

                    SPI_DRVG_spiTxRxTransaction((BYTE *)(start_addr + ind*4),
                                          GME_SAVE_AND_RESTORE_5_VAL,
                                          (BYTE *)(rx_addr + ind*4),
                                          GME_SAVE_AND_RESTORE_5_VAL);
                    volatile unsigned int count =0;
                    if ((((unsigned char *)start_addr)[ind*4] == 0x4C)) // add delay for line
                    {
                       while (count < 20000)
                       {
                          ++count;
                       }
                    }

                 }
               GME_SAVE_AND_RESTORE_2_VAL = 0xFF;
               break;
				default:
//                     debug_printf("bootflash: default\n");
					break;
				}
			}
			GME_SAVE_AND_RESTORE_0_VAL = BOOTFLASH_STATUS_READY;
		}
	}
   unsigned int count =0;
   if (storageType == BOOTFLASH_STORAGE_SPI)
   {
      *((volatile unsigned int *)0x80B8004)=0x01;while (count++<0x2000) {};count=0;
      *((volatile unsigned int *)0x80B8000)=0x01;while (count++<0x2000) {};count=0;
   }
   if ((boot_data->spi_num == 0) && (boot_data->spi_cs == 3))
      resetGpioForCS3();
   if (spi_flags & SPI_NOR_4B_OPCODES)
          SPI_FLASHG_write_cmd(FLASH_EX4B_OPCODE);

	debug_printf("bootflash: return \n");
	// Cleanup GME before continue
	GME_SAVE_AND_RESTORE_0_VAL = 0;
	GME_SAVE_AND_RESTORE_1_VAL = 0;
	GME_SAVE_AND_RESTORE_2_VAL = 0;
	GME_SAVE_AND_RESTORE_3_VAL = 0;
	GME_SAVE_AND_RESTORE_4_VAL = 0;
	GME_SAVE_AND_RESTORE_5_VAL = 0;
	invalidate_dcache_all();

	// When this function returns it enters process_bg_process loop in ROM
	return;
}







