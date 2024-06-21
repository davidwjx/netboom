#ifndef FLASH_H
#define FLASH_H
#include "common.h"
#define FLASH_WRITE_STATUS_OPCODE           0x01
#define FLASH_PAGE_PROGRAM_OPCODE           0x02
#define FLASH_READ_OPCODE                   0x03
#define FLASH_WRITE_DISABLE_OPCODE          0x04
#define FLASH_STATUS_GET_OPCODE             0x05
#define FLASH_WRITE_ENABLE_OPCODE           0x06
#define FLASH_ERASE_CHIP_OPCODE             0x60
#define FLASH_ERASE_BLOCK_OPCODE            0xD8
#define BLOCK_NUM                           128
#define BLOCK_MEMORY_SIZE                   65536
#define FLASH_PAGE_SIZE                     0x80
#define FLASH_LAST_ADDRESS                  (BLOCK_MEMORY_SIZE * BLOCK_NUM - FLASH_PAGE_SIZE)
#define FLASH_SIZE                          BLOCK_NUM*BLOCK_MEMORY_SIZE

#ifdef __cplusplus
extern "C" {
#endif

UINT32 SPI_FLASHG_chipErase(); //75 sec
UINT32 SPI_FLASHG_blockErase(UINT32 address); //64kb - start address 0   3 sec
UINT32 SPI_FLASHG_Read(UINT32 address, UINT32 len, BYTE *pRetBuff);
UINT32 SPI_FLASHG_program(UINT32 address, BYTE *pBuff, UINT32 len); // chunk max len 256 bytes
UINT32 SPI_FLASHG_sizeGet();
UINT32 SPI_FLASHG_init(UINT8 pol, UINT8 phase, unsigned int apb_spi_freq_hz);
UINT32 SPI_FLASHG_deInit();




#ifdef __cplusplus
}
#endif

#endif
