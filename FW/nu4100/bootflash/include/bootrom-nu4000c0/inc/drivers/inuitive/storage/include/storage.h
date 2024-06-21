/*
 * storage.h
 *
 *  Created on 20.10.2014
 *  Author:  Danny B.
 */
 
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include "zr_common.h"
#include "system.h"

/****************************************************************************
 ***************       F U N C T I O N    D E F N I T I O N S     ***********
 ****************************************************************************/
void storage_boot(strap_info_t *strap_info);
void storage_flash_init(unsigned int apb_spi_freq_hz);
void storage_flash_boot_check(void);



/****************************************************************************
 ***************       G L O B A L  D E F I N I T I O S			   ***********
 ****************************************************************************/

#define STORAGE_MAGIC (0xDA)

/****************************************************************************
 ***************      G L O B A L    T Y P E D E F S       ***************
 ****************************************************************************/

typedef struct PACKED {
	uint8   magic;
	uint8   storage_size_in_kb;
	uint16  storage_page_size;	
	uint8   storage_mode;
	uint8   storage_type;
} storage_header_t;


typedef enum {
	STORAGE_HEADER_E = 0,
} storage_layout_e;

typedef enum {
	STORAGE_MODE_NORMAL_E = 0,
	STORAGE_MODE_BURST_E
} storage_mode_e;

typedef enum {
	STORAGE_TYPE_USB_E = 0,
	STORAGE_TYPE_BOOT_E
} storage_type_e;

typedef struct PACKED {
	uint32  bootcode_load_address;
	uint32  bootcode_size;
} storage_boot_header_t;




#endif

