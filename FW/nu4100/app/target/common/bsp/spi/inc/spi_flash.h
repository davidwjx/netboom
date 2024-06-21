#ifndef FLASH_H
#define FLASH_H

#include "inu_types.h"


#define FLASH_WRITE_STATUS_OPCODE           0x01
#define FLASH_PAGE_PROGRAM_OPCODE           0x02
#define FLASH_READ_OPCODE                   0x03
#define FLASH_DUAL_READ_OPCODE              0x3B
#define FLASH_4B_PAGE_PROGRAM_OPCODE        0x12
#define FLASH_4B_READ_OPCODE                0x13
#define FLASH_WRITE_DISABLE_OPCODE          0x04
#define FLASH_STATUS_GET_OPCODE             0x05
#define FLASH_WRITE_ENABLE_OPCODE           0x06
#define FLASH_ERASE_CHIP_OPCODE             0x60
#define FLASH_ERASE_BLOCK_OPCODE            0xD8
#define FLASH_4B_ERASE_BLOCK_OPCODE         0xDC
#define BLOCK_NUM                           512
#define BLOCK_MEMORY_SIZE                   65536
#define FLASH_LAST_ADDRESS                  (BLOCK_MEMORY_SIZE * BLOCK_NUM)
#define FLASH_SIZE                          BLOCK_NUM*BLOCK_MEMORY_SIZE
#define FLASH_OP_RDID                       0x9f    /* Read JEDEC ID */

#ifdef __cplusplus
extern "C" {
#endif

ERRG_codeE SPI_FLASHG_chipErase(UINT8 flashNum); //75 sec
ERRG_codeE SPI_FLASHG_blockErase(UINT8 flashNum, UINT32 address); //64kb - start address 0   3 sec
ERRG_codeE SPI_FLASHG_Read(UINT8 flashNum, UINT32 address, UINT32 len, BYTE *pRetBuff);
ERRG_codeE SPI_FLASHG_program(UINT8 flashNum, UINT32 address, BYTE *pBuff, UINT32 len); // chunk max len 256 bytes
UINT32 SPI_FLASHG_sizeGet(UINT8 flashNum);
UINT32 SPI_FLASHG_getBlockSize( UINT8 flashNum);
UINT32 SPI_FLASHG_getNumBlocks(UINT8 flashNum);
ERRG_codeE SPI_FLASHG_init(UINT8 flashNum, char *deviceName);
ERRG_codeE SPI_FLASHG_deInit(UINT8 flashNum);
#define NUM_OF_FLASH (2)
#define MAIN_FLASH (0)
#define SECONDARY_FLASH (1)



#ifdef __cplusplus
}
#endif

#endif
