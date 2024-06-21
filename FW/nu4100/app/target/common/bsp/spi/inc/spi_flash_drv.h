/****************************************************************************
 *
 *   FileName: i2c_drv.h
 *
 *   Author: Yaron B.
 *
 *   Date: 
 *
 *   Description: SPI driver wrapper
 *   
 ****************************************************************************/
#ifndef SPI_FLASH_DRV_H
#define SPI_FLASH_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "inu_types.h"
#ifdef __cplusplus
      extern "C" {
#endif

#include "io_pal.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   UINT32   startFlashAddress;
   UINT32   readFlashSize;
   UINT8    *flashBufP;
} SPI_FLASH_DRVG_flashReadT;

typedef struct
{
   UINT32   startFlashAddress;
   UINT32   writeFlashSize;
   UINT8    *flashBufP;

} SPI_FLASH_DRVG_flashProgramT;

typedef struct
{
   UINT32   flashSize;
} SPI_FLASH_DRVG_flashSizeGetT;

typedef struct
{
   UINT32   startFlashAddress;
   UINT32   eraseBlockSize;
} SPI_FLASH_DRVG_flashEraseBlockT;

typedef struct
{
   UINT32   startAdrdes;
} SPI_FLASH_DRVG_flashEraseChipT;

typedef union
{
   SPI_FLASH_DRVG_flashReadT          flashRead;
   SPI_FLASH_DRVG_flashProgramT       flashProgram;
   SPI_FLASH_DRVG_flashSizeGetT       flashSizeGet;
   SPI_FLASH_DRVG_flashEraseBlockT    flashEraseBlock;
   SPI_FLASH_DRVG_flashEraseChipT     flashEraseChip;
} SPI_FLASH_DRVG_flashParametersT;

typedef enum
{
   SPI_FLASH_DRVG_ERASE_BLOCK_IOCTL_E = 0,
   SPI_FLASH_DRVG_ERASE_CHIP_IOCTL_E,
   SPI_FLASH_DRVG_FLASH_SIZE_IOCTL_E,
   SPI_FLASH_DRVG_READ_IOCTL_E,
   SPI_FLASH_DRVG_WRITE_IOCTL_E,
   SPI_FLASH_DRVG_NUM_OF_IOCTLS_E
} SPI_FLASH_DRVG_ioctlListE;

typedef enum
{
   SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E             = SPI_FLASH_DRVG_ERASE_BLOCK_IOCTL_E            | IO_CMD_MASK(IO_SPI_FLASH_1_E),
   SPI_FLASH_DRVG_ERASE_CHIP_CMD_E              = SPI_FLASH_DRVG_ERASE_CHIP_IOCTL_E             | IO_CMD_MASK(IO_SPI_FLASH_1_E),
   SPI_FLASH_DRVG_FLASH_SIZE_CMD_E              = SPI_FLASH_DRVG_FLASH_SIZE_IOCTL_E             | IO_CMD_MASK(IO_SPI_FLASH_1_E),
   SPI_FLASH_DRVG_READ_CMD_E                    = SPI_FLASH_DRVG_READ_IOCTL_E                   | IO_CMD_MASK(IO_SPI_FLASH_1_E),
   SPI_FLASH_DRVG_PROGRAM_CMD_E                 = SPI_FLASH_DRVG_WRITE_IOCTL_E                  | IO_CMD_MASK(IO_SPI_FLASH_1_E)
} SPI_FLASH_DRVG_ioctlCmdE;

// module open function params
typedef struct
{
   MEM_MAPG_addrT deviceBaseAddress;
   MEM_MAPG_addrT gmeBaseAddress;
   char           *deviceName;
} SPI_FLASH_DRVG_openParamsT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
//ERRG_codeE SPI_FLASH_DRVG_init(IO_PALG_deviceDesc *deviceDscr,IO_PALG_deviceIdE deviceId);


#ifdef __cplusplus
}
#endif


#endif //SPI_DRV_H




