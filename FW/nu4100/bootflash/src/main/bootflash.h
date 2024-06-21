#ifndef BOOTFLASH_H
#define BOOTFLASH_H


/**********************************************
 * local defines
***********************************************/
#define BOOTFLASH_DATA_ADDRESS (0x2038000)
#define DDR_START_ADDRESS  (0x80000000)
#define DDR_SIZE (0x04000000) //64 MB
#define SPL_DDR_INIT_DONE   (0x1)
#define EFUSE_SIZE (0x200)
#define ENABLE_M3_DEBUG 1

#define SPI_BLOCK_SIZE (64*1024)
#define SPI_PAGE_SIZE 	256


#endif
