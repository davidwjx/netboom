#ifndef STORAGE_H
#define STORAGE_H

#include "inu_storage_layout.h"

typedef struct
{
	unsigned int blockSize;
	unsigned int numOfBlocks;
	unsigned int spi_flags;
}FlashInfoT;

typedef enum
{
	// FW Update operation succeeded to validate the data on Avctive Partition
	// Copy Active Partiton to Backup Partition, and clear UPDATE Flag from flashFlags
	FW_UPDATE_SUCCESS,
	// FW Update operation succeeded to validate the data on Backup Partition
	// Copy Backup Partiton to Active Partition, and clear BTBP Flag from flashFlags
	FW_UPDATE_BTBP_VALID,
	// Error occured while trying to validate a Partition
	// If error occuered on Active Partiton ==> Cancel Update flag, and raise BTBP flag
	// If error occuered on Backup Partiton ==> Cancel BTBP flag, and raise FATAL flag
	FW_UPDATE_ERROR,
}
FwUpdateE;

#ifdef __cplusplus
extern "C" {
#endif

// get flash info
void storage_init();

// get section data
int storage_get_section_data(InuStorageSectionTypeE sectionType,unsigned char *buffer);

// check for full system boot from flash
int storage_check_boot_from_flash();
int storage_update_bootfix_header(InuBootfixHeaderT *buffer);
void storage_set_flash_quad();

int storage_copy_flash_info_to_other_partition();
void storage_update_failure_flash_flags();

#ifdef __cplusplus
}
#endif

#endif
