/*
 * flash_boot.c
 *
 *  Created on 10.07.2013
 *  Author:  Konstantin S. Danny B.
 */
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "common.h"
#include "zr_common.h"
#include "os_defs.h"
#include "spi_flash.h"
#include "flash_boot.h"
#include "gme.h"
#include "nu_regs.h"
#include "storage.h"
#include "verify.h"

/****************************************************************************
 ***************       G L O B A L         D A T A              ***************
 ****************************************************************************/

SECTION_PBSS  extern storage_header_t storage_header;
SECTION_PBSS  extern storage_boot_header_t storage_boot_header;


/****************************************************************************
 ***************       L O C A L    T Y P E D E F S       ***************
 ****************************************************************************/



/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/



/****************************************************************************
 ***************     LOCAL         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     GLOBAL         F U N C T I O N S      ***************
 ****************************************************************************/

/**
 * Returns 1 if valid (expected magic) and 0 if not
 */
int flash_is_valid()
{
	if (storage_header.magic == STORAGE_MAGIC)
		return 1;
	return 0;
}

/**
 * Returns 1 if bootable and 0 if not
 */
int flash_is_bootable()
{
	if (flash_is_valid())
		if (storage_header.storage_type == STORAGE_TYPE_BOOT_E)
			return 1;
   return 0;
}


void flash_init(unsigned int apb_spi_freq_hz)
{
	uint32 ret;
   //TODO: this API assumed a mHz clock. In veloce clock is less than 1 MHz 
	SPI_FLASHG_init(0,0, apb_spi_freq_hz);
	rel_log("flash: init done\n");

	/* Read FLASH header */
	rel_log("flash: header read ... ");
	ret = SPI_FLASHG_Read(0, sizeof(storage_header), (uint8 *)&storage_header);
	if (ret)
		abort_log("SPI error: %d",ret);
	rel_log("done\n");

	if (flash_is_valid())
	{
		rel_log("flash parameters: (size_kb=%d,page_size=%d,mode=%d)\n",storage_header.storage_size_in_kb, storage_header.storage_page_size, storage_header.storage_mode);
	}

#if 0
	/* Update FLASH access parameters */
	if (flash_is_valid())
	{
		SPI_FLASHG_setParameters(storage_header.flash_size_in_mb, storage_header.flash_page_size, storage_header.flash_mode);
		rel_log("flash: set parameters done (size_kb=%d,page_size=%d,mode=%d)\n",storage_header.flash_size_in_kb, storage_header.flash_page_size, storage_header.flash_mode);
	}
#endif
}

void flash_boot()
{
   uint32 flash_offset, flash_boot_address;
   uint32 ret=0;

   /* Read boot header */
   flash_offset = sizeof(storage_header_t);
   rel_log("flash: read boot header ... ");
   ret = SPI_FLASHG_Read(flash_offset, sizeof(storage_boot_header_t), (uint8 *)&storage_boot_header);
   if (ret)
      abort_log("SPI error: %d",ret);
   rel_log("done (address=%x, size=%x)\n",storage_boot_header.bootcode_load_address, storage_boot_header.bootcode_size);

   /* Read boot code */
   flash_offset += sizeof(storage_boot_header_t);
   flash_boot_address = storage_boot_header.bootcode_load_address;
   rel_log("flash: read boot code ... ");
   ret = SPI_FLASHG_Read(flash_offset, storage_boot_header.bootcode_size, (uint8 *)flash_boot_address);
   if (ret)
      abort_log("flash error: %x",ret);
   rel_log("done\n");

   if (getEfuseSecureControlData()!=NOT_SECURED_IMAGE)//support integrity format or integrity & encryption
   {
      //increase cpu clk to 800 MHz to run SB faster
      board_clk_pll_basic_init();
      secureBoot(flash_boot_address);
   }

   /* Run the flash boot code from LRAM
         Warning:
         The new code should be compact to fit LRAM and leave place for U-boot. */
   rel_log("flash: jump to boot code at %x\n",flash_boot_address);
   system_jump_to_address(flash_boot_address);
   rel_log("flash: boot code returns\n");
}




