/*
 * flash_boot.h
 *
 *  Created on 20.10.2014
 *  Author:  Danny B.
 */
#ifndef _FLASH_BOOT_H_
#define _FLASH_BOOT_H_

#include "zr_common.h"
#include "system.h"

/****************************************************************************
 ***************       F U N C T I O N    D E F N I T I O N S     ***********
 ****************************************************************************/

int flash_is_valid();
int flash_usb_descriptors_is_valid();
int flash_usb_phy_is_valid();
int flash_is_bootable();

void flash_init(unsigned int flashAddress);
void flash_parse_usb_descriptors();
void flash_boot();
void * flash_fill_desc_data(uint16 id, uint16 sub_id, uint16 * length);

#endif //_EEPROM_H_

