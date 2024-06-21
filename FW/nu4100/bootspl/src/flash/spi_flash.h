#ifndef FLASH_H
#define FLASH_H
#include "common.h"
#define FLASH_WRITE_STATUS_OPCODE           0x01
#define FLASH_PAGE_PROGRAM_OPCODE           0x02
#define FLASH_READ_OPCODE                   0x03
#define FLASH_DUAL_READ_OPCODE              0x3B
#define FLASH_QUAD_READ_OPCODE              0x6B
#define FLASH_QUAD_ENABLE_OPCODE            0x35
#define FLASH_QUAD_DISABLE_OPCODE           0xf5
#define FLASH_4B_PAGE_PROGRAM_OPCODE        0x12
#define FLASH_4B_READ_OPCODE                0x13
#define FLASH_WRITE_DISABLE_OPCODE          0x04
#define FLASH_STATUS_GET_OPCODE             0x05
#define FLASH_WRITE_ENABLE_OPCODE           0x06
#define FLASH_ERASE_CHIP_OPCODE             0x60
#define FLASH_ERASE_BLOCK_OPCODE            0xD8
#define FLASH_4B_ERASE_BLOCK_OPCODE         0xDC
#define BLOCK_NUM                           1024
#define BLOCK_MEMORY_SIZE                   65536
#define FLASH_LAST_ADDRESS                  (BLOCK_MEMORY_SIZE * BLOCK_NUM)
#define FLASH_SIZE                          BLOCK_NUM*BLOCK_MEMORY_SIZE
#define SPINOR_OP_RDID		                0x9f	/* Read JEDEC ID */
#define MIN_QUAD_LEN                        200
#ifdef __cplusplus
extern "C" {
#endif

UINT32 SPI_FLASHG_chipErase(); //75 sec
UINT32 SPI_FLASHG_blockErase(UINT32 address); //64kb - start address 0   3 sec
UINT32 SPI_FLASHG_Read(UINT32 address, UINT32 len, BYTE *pRetBuff);
UINT32 SPI_FLASHG_program(UINT32 address, BYTE *pBuff, UINT32 len); // chunk max len 256 bytes
UINT32 SPI_FLASHG_sizeGet();
UINT32 SPI_FLASHG_init(UINT8 pol, UINT8 phase, unsigned int apb_spi_freq_hz,unsigned int spi_div);
UINT32 SPI_FLASHG_deInit();
UINT32 SPI_FLASHG_read_reg(UINT8 opcode, UINT8 *pRetBuff, UINT32 len);




#ifdef __cplusplus
}
#endif

#endif
