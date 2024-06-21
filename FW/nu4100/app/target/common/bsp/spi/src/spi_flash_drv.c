/****************************************************************************
 *
 *   FileName:  SPI_FLASH_drv.c
 *
 *   Author:  Yaron B.
 *
 *   Date:
 *
 *   Description:  SPI_FLASH driver
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#ifdef __cplusplus
      extern "C" {
#endif

#include "spi_flash_drv.h"
#include "spi_flash.h"
#include "os_lyr.h"


/****************************************************************************
 ***************        L O C A L      D E F N I T I O N S  ***************
 ****************************************************************************/
typedef enum
{
   SPI_FLASH_DRVP_MODULE_STATUS_DUMMY_E = 0,
   SPI_FLASH_DRVP_MODULE_STATUS_CLOSE_E,
   SPI_FLASH_DRVP_MODULE_STATUS_OPEN_E,
   SPI_FLASH_DRVP_MODULE_STATUS_LAST_E
}  SPI_FLASH_DRVG_moduleStatusE;

typedef ERRG_codeE (* SPI_FLASH_DRVP_ioctlFuncListT)(UINT8 flashNum, void *argP);


#define SPI_FLASH_DRVP_FLASH_WRITE_SIZE                        (256)
#define SPI_FLASH_DRVP_FLASH_WRITE_FACTOR                      (8)
#ifdef __KERNEL_SPI__
#define SPI_FLASH_DRVP_FLASH_READ_SIZE                         (1 * 1024)
#else
#define SPI_FLASH_DRVP_FLASH_READ_SIZE                         (4 * 1024)
#endif
#define SPI_FLASH_DRVP_FLASH_READ_FACTOR                       (12)


/****************************************************************************
 ***************          L O C A L     T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct {
   UINT8 flashNum;
   SPI_FLASH_DRVG_moduleStatusE    moduleStatus;
   SPI_FLASH_DRVP_ioctlFuncListT   *ioctlFuncList;
}   SPI_FLASH_DRVP_DeviceDescT;

/****************************************************************************
 ***************      L O C A L        D A T A            ***************
 ****************************************************************************/
static    SPI_FLASH_DRVP_DeviceDescT        SPI_FLASH_DRVP_deviceDesc[NUM_OF_FLASH];
static    SPI_FLASH_DRVP_ioctlFuncListT    SPI_FLASH_DRVP_ioctlFuncList[ SPI_FLASH_DRVG_NUM_OF_IOCTLS_E];

/****************************************************************************
 ***************      G L O B A L      D A T A            ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L    F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N      OF      ***************
 ***************     L O C A L       F U N C T I O N S      ***************
 ****************************************************************************/


static ERRG_codeE  SPI_FLASH_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, SPI_FLASH_DRVG_openParamsT *params);
static ERRG_codeE  SPI_FLASH_DRVP_close(IO_HANDLE handle);
static ERRG_codeE  SPI_FLASH_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
// IOCTL commands functions

static ERRG_codeE  SPI_FLASH_DRVP_read( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP);
static ERRG_codeE  SPI_FLASH_DRVP_program( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP);
static ERRG_codeE  SPI_FLASH_DRVP_sizeGet( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP);
static ERRG_codeE  SPI_FLASH_DRVP_eraseBlock( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP);
static ERRG_codeE  SPI_FLASH_DRVP_eraseChip( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP);


/****************************************************************************
 ***************     L O C A L       F U N C T I O N S    ***************
 ****************************************************************************/

static ERRG_codeE  SPI_FLASH_DRVP_read(UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP)
{
   UINT32   startFlashAddress;
   UINT32   readFlashSize;
   UINT8    *flashBufP;
   UINT32   numReadFlashChunk;
   UINT32   lastReadFlashCunkLen;
   UINT32   count;

   startFlashAddress = paramsP->flashRead.startFlashAddress;
   readFlashSize     = paramsP->flashRead.readFlashSize;
   flashBufP         = paramsP->flashRead.flashBufP;

   numReadFlashChunk     = (readFlashSize) / SPI_FLASH_DRVP_FLASH_READ_SIZE;
   lastReadFlashCunkLen  = (readFlashSize) % SPI_FLASH_DRVP_FLASH_READ_SIZE;

   for (count = 0; count < numReadFlashChunk; count++)
   {
      SPI_FLASHG_Read(flashNum, startFlashAddress, SPI_FLASH_DRVP_FLASH_READ_SIZE, flashBufP);
      startFlashAddress += (SPI_FLASH_DRVP_FLASH_READ_SIZE);
      flashBufP         += (SPI_FLASH_DRVP_FLASH_READ_SIZE);
   }

   if ( lastReadFlashCunkLen != 0)
   {
      SPI_FLASHG_Read(flashNum, startFlashAddress, lastReadFlashCunkLen, flashBufP);
   }

   return  SPI_FLASH__RET_SUCCESS;
}

static ERRG_codeE  SPI_FLASH_DRVP_program( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP)
{
   UINT32   startFlashAddress;
   UINT32   writeFlashSize;
   UINT32   numWriteFlashChunk;
   UINT32   lastWriteFlashChunkLen;
   UINT8    *flashBufP;
   UINT32   count = 0;
   UINT32   startOffset = 0;
   UINT8    flashWriteBuf[SPI_FLASH_DRVP_FLASH_WRITE_SIZE];

   startFlashAddress = paramsP->flashProgram.startFlashAddress;
   writeFlashSize    = paramsP->flashProgram.writeFlashSize;
   flashBufP         = paramsP->flashProgram.flashBufP;

   startOffset = startFlashAddress % SPI_FLASH_DRVP_FLASH_WRITE_SIZE;

   numWriteFlashChunk     = (writeFlashSize) / SPI_FLASH_DRVP_FLASH_WRITE_SIZE;
   lastWriteFlashChunkLen = (writeFlashSize) % SPI_FLASH_DRVP_FLASH_WRITE_SIZE;

   if(startOffset)
   {
      count = 1;
      SPI_FLASHG_program(flashNum, startFlashAddress, flashBufP, (SPI_FLASH_DRVP_FLASH_WRITE_SIZE - startOffset));
      startFlashAddress += (SPI_FLASH_DRVP_FLASH_WRITE_SIZE - startOffset);
      flashBufP         += (SPI_FLASH_DRVP_FLASH_WRITE_SIZE - startOffset);
   }

   for (count = 0; count < numWriteFlashChunk; count++)
   {
      SPI_FLASHG_program(flashNum, startFlashAddress, flashBufP, SPI_FLASH_DRVP_FLASH_WRITE_SIZE);
      // TODO: Need to check if this is relevant for all cases.
      // Was added to prevent WD
      if (!(count % 256))
      {
         OS_LYRG_usleep(5000);
      }
      startFlashAddress += (SPI_FLASH_DRVP_FLASH_WRITE_SIZE);
      flashBufP         += (SPI_FLASH_DRVP_FLASH_WRITE_SIZE);
   }

   if (lastWriteFlashChunkLen != 0)
   {
      // memset ((void*)(&flashWriteBuf), 0 ,lastWriteFlashChunkLen);
      // memcpy((void*)(&flashWriteBuf), (UINT8*)(flashBufP), lastWriteFlashChunkLen);
      // flashBufP = flashWriteBuf;
      LOGG_PRINT(LOG_INFO_E, NULL, "LAST BEFORE CALL TO SPI_FLASHG_program. startFlashAddress=0x%x lastWriteFlashChunkLen = %d\n", startFlashAddress, lastWriteFlashChunkLen);
      SPI_FLASHG_program(flashNum, startFlashAddress, flashBufP, lastWriteFlashChunkLen);
   }

   return  SPI_FLASH__RET_SUCCESS;
}

static ERRG_codeE  SPI_FLASH_DRVP_sizeGet( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP)
{
   paramsP->flashSizeGet.flashSize = SPI_FLASHG_sizeGet(flashNum);

   return  SPI_FLASH__RET_SUCCESS;

}

static ERRG_codeE  SPI_FLASH_DRVP_eraseBlock( UINT8 flashNum,SPI_FLASH_DRVG_flashParametersT *paramsP)
{
   UINT32   eraseBlockSize;
   UINT32   numFlashEraseBlock;
   UINT32   lastEraseBlockSize;
   UINT32   count;
   UINT32   startEraseAddress;

   eraseBlockSize = paramsP->flashEraseBlock.eraseBlockSize;

   numFlashEraseBlock = (eraseBlockSize) / SPI_FLASHG_getBlockSize(flashNum);
   lastEraseBlockSize = (eraseBlockSize) % SPI_FLASHG_getBlockSize(flashNum);

   startEraseAddress = paramsP->flashEraseBlock.startFlashAddress;
   for (count = 0; count < numFlashEraseBlock; count++)
   {
      SPI_FLASHG_blockErase(flashNum, startEraseAddress);
      // TODO: Need to check if this is relevant for all cases.
      // Was added to prevent WD
      OS_LYRG_usleep(5000);
      if( !(count % 100))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "count = %d, startEraseAddress = 0x%x\n", count, startEraseAddress);
      }
      startEraseAddress += SPI_FLASHG_getBlockSize(flashNum);
   }

   if (lastEraseBlockSize)
   {
      SPI_FLASHG_blockErase(flashNum, startEraseAddress);
   }

   return  SPI_FLASH__RET_SUCCESS;
}

static ERRG_codeE  SPI_FLASH_DRVP_eraseChip( UINT8 flashNum, SPI_FLASH_DRVG_flashParametersT *paramsP)
{
   ERRG_codeE retVal = SPI_FLASH__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(paramsP);

   retVal =  SPI_FLASHG_chipErase(flashNum);

   return retVal;
}


static ERRG_codeE  SPI_FLASH_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, SPI_FLASH_DRVG_openParamsT *params)
{
   ERRG_codeE retCode =  SPI_FLASH__RET_SUCCESS;
   *handleP = (IO_HANDLE)NULL;
   UINT8 flashNum = deviceId-IO_SPI_FLASH_1_E;
   SPI_FLASH_DRVP_DeviceDescT *deviceDescrP;


   if (( deviceId < IO_SPI_FLASH_1_E) || (deviceId > IO_SPI_FLASH_2_E))
         return SPI_FLASH__ERR_FLASH_INVALID_PARAMS;
   deviceDescrP = &SPI_FLASH_DRVP_deviceDesc[flashNum];


   if( deviceDescrP->moduleStatus ==  SPI_FLASH_DRVP_MODULE_STATUS_CLOSE_E)
   {
      deviceDescrP->moduleStatus =  SPI_FLASH_DRVP_MODULE_STATUS_OPEN_E;

      deviceDescrP->flashNum = flashNum;
      retCode = SPI_FLASHG_init(flashNum, params->deviceName);
      if (ERRG_SUCCEEDED(retCode))
      {
         *handleP = deviceDescrP;
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "fail: SPI_FLASHG_init %s fail.\n", params->deviceName);
      }
   }
   else
   {
      retCode = SPI_FLASH__ERR_OPEN_FAIL_DRV_NOT_CLOSED;
      LOGG_PRINT(LOG_ERROR_E, retCode, "open SPI flash driver fail!!!\n");
   }
   return  SPI_FLASH__RET_SUCCESS;
}

static ERRG_codeE  SPI_FLASH_DRVP_close(IO_HANDLE handle)
{
   ERRG_codeE  retVal =  SPI_FLASH__RET_SUCCESS;
   SPI_FLASH_DRVP_DeviceDescT *deviceDscr = (SPI_FLASH_DRVP_DeviceDescT *)handle;
   retVal = SPI_FLASHG_deInit(deviceDscr->flashNum);
   deviceDscr->moduleStatus    =  SPI_FLASH_DRVP_MODULE_STATUS_CLOSE_E;
   return(retVal);
}

static ERRG_codeE  SPI_FLASH_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   ERRG_codeE  SPIIoctlRetCode =  SPI_FLASH__RET_SUCCESS;
   SPI_FLASH_DRVP_DeviceDescT *deviceDscr = (SPI_FLASH_DRVP_DeviceDescT *)handle;
   UINT16      ioctl;

   if((( SPI_FLASH_DRVP_DeviceDescT*)handle)->moduleStatus ==  SPI_FLASH_DRVP_MODULE_STATUS_OPEN_E)
   {
      ioctl = IO_CMD_GET_IOCTL(cmd);
      if (IO_CMD_IS_DEVICE_ID(cmd, IO_SPI_FLASH_1_E))
      {
            SPIIoctlRetCode = ((( SPI_FLASH_DRVP_DeviceDescT*)handle))->ioctlFuncList[ioctl](deviceDscr->flashNum, argP);
      }
      else  // if (IO_CMD_IS_DEVICE_ID(cmd, IO_SPI_E))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "IN SPI_FLASH_DRVP_ioctl, SPI_FLASH__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE\n");
         SPIIoctlRetCode =  SPI_FLASH__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE;
      }
   }
   else  // if(SPI_DRVP_deviceDesc.moduleStatus == SPI_DRVP_MODULE_STATUS_OPEN_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "IN SPI_FLASH_DRVP_ioctl, SPI_FLASH__ERR_IOCTL_FAIL_DRV_NOT_OPEN\n");
      SPIIoctlRetCode =  SPI_FLASH__ERR_IOCTL_FAIL_DRV_NOT_OPEN;
   }

return(SPIIoctlRetCode);
}

/****************************************************************************
 ***************     G L O B A L        F U N C T I O N S    **************
 ****************************************************************************/

ERRG_codeE  SPI_FLASH_DRVG_init(IO_PALG_deviceDesc *deviceDscr,IO_PALG_deviceIdE deviceId)
{
   ERRG_codeE SPIInitRetCode =  SPI_FLASH__RET_SUCCESS;
   IO_PALG_apiCommandT *palP = &deviceDscr->IO_PALP_apiCommand;
   UINT8 flashNum;
   if(palP)
   {
      palP->close =  (IO_PALG_closeT)& SPI_FLASH_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT)& SPI_FLASH_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT) & SPI_FLASH_DRVP_open;
   }
   else
   {
      SPIInitRetCode =  SPI_FLASH__ERR_INIT_FAIL_WRONG_PAL_HANDLE;
   }

   if ((deviceId >=IO_SPI_FLASH_1_E)&&(deviceId <=IO_SPI_FLASH_2_E))
   {
      flashNum = deviceId - IO_SPI_FLASH_1_E;
      memset(&SPI_FLASH_DRVP_deviceDesc[flashNum], 0, sizeof(SPI_FLASH_DRVP_DeviceDescT));

      // Need to add this minimal sleep to prevent errors while using SPI_FLASH_DRVP_ioctlFuncList functions.
      OS_LYRG_usleep(1);

      SPI_FLASH_DRVP_deviceDesc[flashNum].moduleStatus      =  SPI_FLASH_DRVP_MODULE_STATUS_CLOSE_E;
      SPI_FLASH_DRVP_deviceDesc[flashNum].ioctlFuncList      =  SPI_FLASH_DRVP_ioctlFuncList;
      SPI_FLASH_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL( SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E)]      = ( SPI_FLASH_DRVP_ioctlFuncListT) SPI_FLASH_DRVP_eraseBlock;
      SPI_FLASH_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL( SPI_FLASH_DRVG_ERASE_CHIP_CMD_E)]       = ( SPI_FLASH_DRVP_ioctlFuncListT) SPI_FLASH_DRVP_eraseChip;
      SPI_FLASH_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL( SPI_FLASH_DRVG_FLASH_SIZE_CMD_E)]       = ( SPI_FLASH_DRVP_ioctlFuncListT) SPI_FLASH_DRVP_sizeGet;
      SPI_FLASH_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL( SPI_FLASH_DRVG_READ_CMD_E)]             = ( SPI_FLASH_DRVP_ioctlFuncListT) SPI_FLASH_DRVP_read;
      SPI_FLASH_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL( SPI_FLASH_DRVG_PROGRAM_CMD_E)]          = ( SPI_FLASH_DRVP_ioctlFuncListT) SPI_FLASH_DRVP_program;
   }

   return SPIInitRetCode;
}

#ifdef __cplusplus
}
#endif


