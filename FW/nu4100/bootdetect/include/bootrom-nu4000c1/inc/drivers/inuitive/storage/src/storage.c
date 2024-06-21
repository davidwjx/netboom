/*
 * Storage.c
 *
 *  Created on 20.10.2014
 *  Author:  Danny b
 */
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "common.h"
#include "zr_common.h"
#include "os_defs.h"
#include "nu_regs.h"
#include "storage.h"
#include "spi_flash.h"
#include "flash_boot.h"



/****************************************************************************
 ***************       G L O B A L         D A T A              ***************
 ****************************************************************************/

SECTION_PBSS  storage_header_t storage_header;
SECTION_PBSS  storage_boot_header_t storage_boot_header;

/****************************************************************************
 ***************       L O C A L     F U N C T I O N  S        ***************
 ****************************************************************************/

/**
 * Routine: storage_flash_boot_check
 *
 * Load EEPROM/FLASH header and than boot if boot code is detected (EEPROM is primary boot source)
 */
void storage_flash_boot_check(void)
{
	// Init FLASH SPI
	board_spi_init();
	storage_flash_init(get_board_info()->spi_freq_hz);

	//check for boot
	if(flash_is_valid() && flash_is_bootable())
	{
		flash_boot(); //flash boot does not return
	}
}


/****************************************************************************
 ***************       G L O B A L         F U N C T I O N S                                 ***************
 ****************************************************************************/

void storage_boot(strap_info_t *strap_info)
{
   if(strap_info->bootMode == BOARD_BOOT_MODE_FLASH)
   {
   	rel_log("bootrom: flash mode\n");
   	/* Warning: The function below will not return in case of boot from eeprom */
   	storage_flash_boot_check();
   }
}

void storage_flash_init(unsigned int apb_spi_freq_hz)
{
   flash_init(apb_spi_freq_hz);
}

