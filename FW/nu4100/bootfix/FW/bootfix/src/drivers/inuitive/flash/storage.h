#ifndef STORAGE_H
#define STORAGE_H


typedef struct
{
	unsigned int blockSize;
	unsigned int numOfBlocks;
	unsigned int spi_flags;
}FlashInfoT;

#ifdef __cplusplus
extern "C" {
#endif

// get flash info
void storage_init();

// get section data
int storage_get_section_data(InuStorageSectionTypeE sectionType,unsigned char *buffer);

// check for full system boot from flash
int storage_check_boot_from_flash();

void storage_update_flash_flags();

#define OFFSET_LOAD_FROM_FLASH 4

#ifdef __cplusplus
}
#endif

#endif
