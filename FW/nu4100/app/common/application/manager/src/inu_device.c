#include "assert.h"

#include "inu_device.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "svc_cfg.h"
#include "internal_cmd.h"
#include "inu_lm.h"
#include "log.h"
#include "inu2_internal.h"
#include "inu_common.h"
#include "internal_cmd.h"
#include "version.h"
#include <string.h>
#include "stats.h"
#include "calibration.h"
#include "inu_time.h"
#include <stdarg.h>
#include <signal.h>
#include "inu_defs.h"
#include "inu_sensorsync_serializer.h"

#if DEFSG_IS_GP
#include "spi_flash_drv.h"
#include "hw_mngr.h"
#include "gme_mngr.h"
#include "gme_drv.h"
#include "gpio_drv.h"
#include "lut_mngr.h"
#include "eeprom_ctrl.h"
#include "sensors_mngr.h"
#include "sequence_mngr.h"
#include "inu_storage_layout.h"
#include "spi_flash.h"
#include "wd.h"
#include "inu_alt.h"
#include "hw_regs.h"
#include "app_init.h"
#include "proj_drv.h"
#include "cde_mngr_new.h"
#include "hcg_mngr.h"
#include "hca_mngr.h"
#include "trigger_mngr.h"
#include "nand_raw_storage.h"
#include "icc.h"
#include "data_base.h"
#include "pmm_mngr.h"
#include "pwr_mngr.h"
#include "io_pal.h"
#include "isp_tuning_server.hpp"
#include "rtc.h"

#include "metadata_updater.h"
#include "metadata_sensor_updater.h"
#include "sensorsync_updater.h"
#include "helsinki.h"
#elif DEFSG_IS_HOST
#include "inu_usb.h"
#define LUT_MNGR_NUM_OF_BUFF     (2) // Also defined in "lut_mngr.h". Once updated, need to update it in there as well.
#endif
#include "inu_metadata.h"
#include "inu_metadata_serializer.h"

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
#define INU_DEVICE__BOOTPATH_MAX_CHAR_NUM               (200)

#define SYSTEMP_LUT_CHUNK_DATA_LEN                      (8*1024)
// flash params
#define SYSTEMP_FLASH_CHUNK_DATA_LEN                    (8*1024)
#define SYSTEMP_FLASH_CHUNK_SIZE_FACTOR                 (13)

#define INU_DEVICE__MEM_POOL_BUFF_SIZE_BYTES  (10000)
#define INU_DEVICE__MEM_POOL_BUFF_NUM  (40)

#define INU_DEVICE__NUM_IAE_LUT       (4)
#define INU_DEVICE__NUM_IAE_SUB_LUT   (4)
#define INU_DEVICE__NUM_SENSORS       (8)
#define SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN              (8*1024)

#define INU_DEVICE__DSP_SYNC_TIMEOUT_MSEC  (1000)
#define INU_DEVICE__SLEEP_USEC           (1000)
#define INU_DEVICE__ACK_TIMEOUT_MSEC     (200)

typedef struct inu_device__privData
{
   inu_device__params           deviceParams;
   inu_device__CtorParams       ctorParams;
   inu_ref                      *ref_list[INU_DEVICE__MAX_NUM_OF_REFS];
   OS_LYRG_mutexT               refListMutex;
   int                          receivedDeviceInfo;
   inu_device__version          deviceInfo;
   char                         bootPath[INU_DEVICE__BOOTPATH_MAX_CHAR_NUM];
   MEM_POOLG_handleT            poolH;
#if DEFSG_IS_GP
   inu_altH                     altH;
#endif
   char                        *calibPath;
   UINT32                      calibMode;

   //link monitor
   LINK_MONITORG_handleT        lmH;
   inu_device_connect_state_e   connectState; //used for dtor when no connection to target

   //flash handling
   INUG_ioctlCalibrationHdrRdT  calibrationHdr;
   BOOL                         ioctlFlashAck;
   BOOL                         sendLutAck;

   UINT32                       numOfReadFlashChunkToRecieve;
   UINT32                       numOfReadFlashChunkRecieved;
   inu_device_standalone_mode_e standAloneMode;

   //intermediate buffer, used for flash/i2c read
   UINT8                        tempReadBuf[SYSTEMP_FLASH_CHUNK_DATA_LEN];
   UINT8                        *tempReadBufP;

   //read/write buf from/to memory
   void                        *readBuffPtr;
   UINT32                       writeBufVirtAddress; 
   UINT32                       calibBufVirtAddress;
   BOOL                         sendBufAck;
   BOOL                         recBufAck;
   BOOL                         pdIsActive;
   UINT32                       rwBuffSize;
#if DEFSG_IS_GP
   OS_LYRG_event                dspSyncMsgEvent;
   ERRG_codeE                   dspSyncMsgResult;
#endif
}inu_device__privData;


/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static void inu_device__init(inu_device *me);
static void inu_device__readFlashChunk(inu_device *me, INTERNAL_CMDG_flashChunkHdrT *flashChunkHdrP, UINT8 *dataP, UINT32 dataLen);
static ERRG_codeE inu_device__handleReadBufChunkMsg(inu_device *me, INTERNAL_CMDG_readBufChunkHdrT *bufChunkHdrP, UINT8 *chunkP);
static ERRG_codeE inu_device__handleEepromAccess(inu_device *me, inu_device__eepromAccessT *eepromAccess, UINT8* data);
static BOOLEAN inu_device__readBinFile(const char *fileName, inu_device_memory_t *buffer);
int NUCFG_getSensorsFromIau(unsigned int *iauToSensor);
static ERRG_codeE inu_device__waitForAck(BOOL *ackFlag, UINT32  timeoutMsec);
static ERRG_codeE inu_device__gpBurnDynCalibration(inu_device *me, InuSectionHeaderT *pDynCalibSectionHeader,UINT8 *calibrationDataPtr);

static inu_ref__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_DEVICE";
static inu_device__powerDownCmdE inu_device_powerDownStatus = INU_DEVICE_POWER_DOWN_FIRST_BOOT_E;

#if DEFSG_IS_GP
static inu_device__setflashSectionHdr savedflashHeader;
#if 0
static char filePath[1024];
#endif
static char configFolderFullPath[1024];

void inu_device__setFlashGpio(UINT8 flashNum)
{
   UINT32 gpioNum=60;
   GPIO_DRVG_gpioSetDirParamsT         gpioDir;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   static UINT8 lastRdFlashNum=MAIN_FLASH;

   if (flashNum != lastRdFlashNum)
   {
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = gpioNum;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);
      OS_LYRG_usleep(50);
      gpioParams.gpioNum = gpioNum;
      if (flashNum != MAIN_FLASH)
         gpioParams.val = 1;
      else gpioParams.val = 0;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }
   lastRdFlashNum = flashNum;
}

/****************************************************************************
*
*  Function Name: inu_device__gpReadFlashChunk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__gpReadFlashChunk(UINT8 flashNum, UINT32 flashOffset, UINT32 flashSize, BOOLEAN send2peer, UINT8 *flashBufP, inu_device *me)
{
   inu_device__privData            *privP = (inu_device__privData*)me->privP;
   ERRG_codeE                      retCode     = SYSTEM__RET_SUCCESS;
   SPI_FLASH_DRVG_flashParametersT flashParameters;
   UINT32                          rdFlashSize;
   UINT32                          numReadFlashChunk;
   UINT32                          numReadFlashChunkFinal;
   UINT32                          lastReadFlashChunkLen;
   UINT32                          count;
   UINT32                          startFlashAddress;
   INTERNAL_CMDG_flashChunkHdrT    flashChunkHdr;
   IO_PALG_deviceIdE               deviceId = IO_SPI_FLASH_1_E;

   //It is read flash command
   //Read flash in cunk of 8KBYTE
   rdFlashSize = flashSize;
   inu_device__setFlashGpio(flashNum);
   if (flashNum == SECONDARY_FLASH)
      deviceId = IO_SPI_FLASH_2_E;
   numReadFlashChunk     = (rdFlashSize) / SYSTEMP_FLASH_CHUNK_DATA_LEN;
   lastReadFlashChunkLen = (rdFlashSize) % SYSTEMP_FLASH_CHUNK_DATA_LEN;
   numReadFlashChunkFinal = numReadFlashChunk;
   if (lastReadFlashChunkLen)
      numReadFlashChunkFinal++;

   startFlashAddress = flashOffset;
   for (count = 0; count < numReadFlashChunk; count++)
   {
      flashParameters.flashRead.startFlashAddress  = startFlashAddress;
      flashParameters.flashRead.readFlashSize      = SYSTEMP_FLASH_CHUNK_DATA_LEN;
      flashParameters.flashRead.flashBufP          = flashBufP;

      retCode = IO_PALG_ioctl(IO_PALG_getHandle(deviceId), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
      if (ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
      }

      if (send2peer)
      {
         flashChunkHdr.flashAck     = TRUE;
         flashChunkHdr.chunkId      = count;
         flashChunkHdr.chunkLen     = SYSTEMP_FLASH_CHUNK_DATA_LEN;
         flashChunkHdr.numOfChunks  = numReadFlashChunkFinal;
         flashChunkHdr.flashBufSize = rdFlashSize;
         do
         {
            retCode = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_READ_FLASH_HDR_E,&flashChunkHdr,&privP->tempReadBuf[0],flashChunkHdr.chunkLen);
            OS_LYRG_usleep(10);
         }
         while(ERRG_FAILED(retCode));
      }
      startFlashAddress += SYSTEMP_FLASH_CHUNK_DATA_LEN;
   }

   if ( lastReadFlashChunkLen != 0)
   {
      flashParameters.flashRead.startFlashAddress  = startFlashAddress;
      flashParameters.flashRead.readFlashSize      = lastReadFlashChunkLen;
      flashParameters.flashRead.flashBufP          = flashBufP;

      retCode = IO_PALG_ioctl(IO_PALG_getHandle(deviceId), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
      if (ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
      }
      else
      {
         LOGG_PRINT(LOG_INFO_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
      }

      if (send2peer)
      {
         flashChunkHdr.flashAck     = TRUE;
         flashChunkHdr.chunkId      = count;
         flashChunkHdr.chunkLen     = lastReadFlashChunkLen;
         flashChunkHdr.numOfChunks  = numReadFlashChunkFinal;
         flashChunkHdr.flashBufSize = rdFlashSize;
         retCode = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_READ_FLASH_HDR_E,&flashChunkHdr,&privP->tempReadBuf[0],lastReadFlashChunkLen);
      }
   }
   return(retCode);
}



ERRG_codeE inu_device__gpHandleReadBufMsg(inu_device *me, inu_device__readWriteBuf *bufChunkHdrP)
{
   ERRG_codeE                        retCode= SYSTEM__RET_SUCCESS;
   UINT32                            bufSize;
   UINT32                            numOfChunks;
   UINT32                            chunkLen = SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN;
   UINT32                            lastchunkLen;
   UINT32                            chunkId;
   INTERNAL_CMDG_readBufChunkHdrT    readBufferChunkHdr;
   UINT32                            destVirtAddr;

   if(bufChunkHdrP->address != 0)	// if requested buffer is physical memory
   {
       retCode =  MEM_MAPG_mapRegion(&(bufChunkHdrP->address), bufChunkHdrP->bufLen, 0, &destVirtAddr, -1);
   }
   else if(bufChunkHdrP->buf != NULL) // requested buffer is virual memory
   {
       destVirtAddr = (UINT32)bufChunkHdrP->buf;      
   }
   else
   {
       return INU_DEVICE__ERR_NOT_SUPPORTED;
   }

    //Read buffer in chunks of 8KB
   bufSize = bufChunkHdrP->bufLen;
   numOfChunks    = (bufSize + chunkLen -1)/chunkLen;
   lastchunkLen   = bufSize % chunkLen;

   if ( lastchunkLen == 0 )
   {
      lastchunkLen = SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN;
   }

   for(chunkId = 0; chunkId < numOfChunks; )
   {
      if (chunkId == (numOfChunks-1))
      {
         chunkLen = lastchunkLen; //last chunk can be shorter
      }

      readBufferChunkHdr.chunkId     = chunkId;
      readBufferChunkHdr.chunkLen    = chunkLen;
      readBufferChunkHdr.numOfChunks = numOfChunks;
      readBufferChunkHdr.buffSize    = bufChunkHdrP->bufLen;

      retCode = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_READ_BUF_HDR_E, &readBufferChunkHdr, (UINT8 *)destVirtAddr,chunkLen);
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "failed channel read buffer -retry\n");
      }
      else
      {
         chunkId++; //next chunk to send
         destVirtAddr = destVirtAddr + chunkLen;
         OS_LYRG_usleep(10000);
      }
   }
   return retCode;
}


ERRG_codeE inu_device__gpSetAlternate(inu_device *me, inu_device__alternateProjCfgT *cfgP)
{
   ERRG_codeE                        retCode = SYSTEM__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP;
   inu_device__privData             *privP = (inu_device__privData*)me->privP;

   if (cfgP->alternateModeEnable)
   {
      ALTG_start(&privP->altH, cfgP->numFramesPattern, cfgP->numFramesFlood);
      //hack - should be done in soc channel?
      sensorInfoP = SENSORS_MNGRG_getSensorInfo(INU_DEFSG_SENSOR_0_E);
      sensorInfoP->altH = privP->altH;
   }
   else
   {
      ALTG_stop(privP->altH);
      privP->altH = NULL;
      //hack - should be done in soc channel?
      sensorInfoP = SENSORS_MNGRG_getSensorInfo(INU_DEFSG_SENSOR_0_E);
      sensorInfoP->altH = privP->altH;
   }

   return retCode;
}



/****************************************************************************
*
*  Function Name: inu_device__gpReadBootfixMeta
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/

#define FLASH_ERAZE_BLOCK_SIZE   (0x10000)

ERRG_codeE inu_device__gpReadBootfixMetaSection( UINT8 *flashBufP, UINT32 flashBufSize)
{
   ERRG_codeE  retCode  = SYSTEM__RET_SUCCESS;
   SPI_FLASH_DRVG_flashParametersT flashParameters;

   memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

   flashParameters.flashRead.startFlashAddress  = SECTION_BOOTFIX_META_START_BLOCK * FLASH_ERAZE_BLOCK_SIZE;
   flashParameters.flashRead.readFlashSize    = flashBufSize;
   flashParameters.flashRead.flashBufP        = flashBufP;

   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
   }

    return(retCode);
}

/****************************************************************************
*
*  Function Name: inu_device__gpWriteBootfixMetaSection
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/


ERRG_codeE inu_device__gpWriteBootfixMetaSection(UINT8 *flashBufP, UINT32 flashBufSizeP)
{
   ERRG_codeE  retCode  = SYSTEM__RET_SUCCESS;
   SPI_FLASH_DRVG_flashParametersT flashParameters;

   inu_device__setFlashGpio(MAIN_FLASH);

   memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

   flashParameters.flashProgram.startFlashAddress  = SECTION_BOOTFIX_META_START_BLOCK * FLASH_ERAZE_BLOCK_SIZE;
   flashParameters.flashProgram.writeFlashSize  = flashBufSizeP;

   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E, &flashParameters);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
   }


   memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

   flashParameters.flashProgram.startFlashAddress  = SECTION_BOOTFIX_META_START_BLOCK * FLASH_ERAZE_BLOCK_SIZE;
   flashParameters.flashProgram.writeFlashSize  = flashBufSizeP;
   flashParameters.flashProgram.flashBufP          = flashBufP;

   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
   }

    return(retCode);
}

/****************************************************************************
*  Function Name: inu_device__gpHandleDynCalibMsg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__gpHandleDynCalibMsg(inu_device *me, UINT32 msgCode, INTERNAL_CMDG_readWriteBufHdrT *bufChunkHdrP, UINT8 *chunkP)
{
   ERRG_codeE             retCode     = SYSTEM__RET_SUCCESS;
   MEM_MAPG_addrT         virAddr;
   UINT32 phyAddress;

   inu_device__privData *privP = (inu_device__privData*)((inu_device*)me)->privP;
   InuSectionHeaderT       SectionHeader,*pDynCalibSectionHeader;

   pDynCalibSectionHeader = &SectionHeader;
   if(bufChunkHdrP->chunkId == 0)
   {

      retCode =  MEM_MAPG_alloc(bufChunkHdrP->bufLen, &phyAddress, &privP->calibBufVirtAddress, 0);
      savedflashHeader.sectionDataSize = bufChunkHdrP->bufLen;
   }
   virAddr = (MEM_MAPG_addrT)(privP->calibBufVirtAddress + (bufChunkHdrP->chunkId * SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN));//update memory offset
   if (ERRG_SUCCEEDED(retCode))
   {
      memcpy((void *)virAddr, (void *)chunkP, bufChunkHdrP->chunkLen);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "write buffer chunk=%d size=%d saved to %p\n",
      bufChunkHdrP->chunkId, bufChunkHdrP->chunkLen,(UINT32)virAddr);

      if(bufChunkHdrP->chunkId == ( bufChunkHdrP->numOfChunks - 1 ))
      {
          pDynCalibSectionHeader->sectionDataSize = savedflashHeader.sectionDataSize;
          pDynCalibSectionHeader->versionNumber=savedflashHeader.versionNumber;     
          pDynCalibSectionHeader->sectionFormat=savedflashHeader.sectionFormat;
          pDynCalibSectionHeader->timestamp=savedflashHeader.timestamp;
          strncpy(pDynCalibSectionHeader->versionString,savedflashHeader.versionString, VERSION_STRING_SIZE);
          inu_device__gpBurnDynCalibration(me,pDynCalibSectionHeader,(UINT8 *)privP->calibBufVirtAddress);
          //Send receive chunk Ack to host
          INTERNAL_CMDG_readWriteBufHdrT sendBufAck;

          sendBufAck.chunkId = bufChunkHdrP->chunkId;
          inu_ref__copyAndSendDataAsync((inu_ref*)me,msgCode,&sendBufAck, NULL, 0);
          MEM_MAPG_free((UINT32 *)privP->calibBufVirtAddress);
          privP->calibBufVirtAddress=0;
      }
   }
   return(retCode);
}


/****************************************************************************
*  Function Name: inu_device__gpHandleWriteBufChunkMsg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__gpHandleWriteBufChunkMsg(inu_device *me, UINT32 msgCode, INTERNAL_CMDG_readWriteBufHdrT *bufChunkHdrP, UINT8 *chunkP)
{
   ERRG_codeE             retCode     = SYSTEM__RET_SUCCESS;
   MEM_MAPG_addrT         virAddr;

   inu_device__privData *privP = (inu_device__privData*)((inu_device*)me)->privP;

   if(bufChunkHdrP->chunkId == 0)//mapping and save virtual address just once
      retCode =  MEM_MAPG_mapRegion(&(bufChunkHdrP->address), bufChunkHdrP->bufLen, 0, &privP->writeBufVirtAddress, -1);

   virAddr = (MEM_MAPG_addrT)(privP->writeBufVirtAddress + (bufChunkHdrP->chunkId * SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN));//update memory offset
   if (ERRG_SUCCEEDED(retCode))
   {
      memcpy((void *)virAddr, (void *)chunkP, bufChunkHdrP->chunkLen);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "write buffer chunk=%d size=%d saved to %p\n",
      bufChunkHdrP->chunkId, bufChunkHdrP->chunkLen,(UINT32)virAddr);

      if(bufChunkHdrP->chunkId == ( bufChunkHdrP->numOfChunks - 1 ))
      {
          //Send receive chunk Ack to host
          INTERNAL_CMDG_readWriteBufHdrT sendBufAck;

          sendBufAck.chunkId = bufChunkHdrP->chunkId;
          inu_ref__copyAndSendDataAsync((inu_ref*)me,msgCode,&sendBufAck, NULL, 0);
      }
   }
   return(retCode);
}

/****************************************************************************
*
*  Function Name: inu_device__gpGetFlashOffset
*
*  Description: calculate block size
*
*  Inputs: flash block num
*
*  Outputs: related flash block size
*
*  Returns: blockSize
*
*  Context: system service
*
****************************************************************************/
static int inu_device__gpGetFlashOffset(UINT32 flashNum, int blockNum)
{
   int blockSize = SPI_FLASHG_getBlockSize(flashNum)*blockNum;
   return blockSize;
}

static int inu_device__loadCalibParams(char *configFolder, CONFG_modeE mode)
{
   strcpy(configFolderFullPath, configFolder);
   strcat(configFolderFullPath,SLESH"Rev001");
   strcat(configFolderFullPath,SLESH);
   switch (mode)
   {
      case CONFG_BINNNG:
         strcat(configFolderFullPath,"Binning");
         break;
      case CONFG_FULL:
         strcat(configFolderFullPath,"Full");
         break;
      case CONFG_ALTERNATING:
         strcat(configFolderFullPath,"Alternating");
        break;
      case CONFG_VERT_BINNING:
         strcat(configFolderFullPath,"VerticalBinning");
         break;
      case CONFG_USER:
         strcat(configFolderFullPath,"user");
         break;
   }
   strcat(configFolderFullPath,SLESH"NU4K");

   LOGG_PRINT(LOG_INFO_E, NULL, "inu_device__loadCalibParams Not loading LUTS due to new design in B0\n");
   return 0;
}

ERRG_codeE inu_device__readCalib(inu_deviceH device,CONFG_modeE configMode)
{
   ERRG_codeE       ret = SYSTEM__RET_SUCCESS;
   InuSectionHeaderT       SectionHeader;
   UINT32                  flashOffset;
   char configFolder[512];
   UINT8 flashNum = MAIN_FLASH;

   sprintf(configFolder,"%s/%s",CALIB_DIRECTORY,CALIB_FILE);
   LOGG_PRINT(LOG_INFO_E, ret, "message config mode 0x%x\n",configMode);
   flashOffset=SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) -  sizeof(InuSectionHeaderT);
   ret = inu_device__gpReadFlashChunk(flashNum, flashOffset, sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&SectionHeader, device);
   flashOffset=SPI_FLASHG_getBlockSize(flashNum)*(SPI_FLASHG_getNumBlocks(flashNum) -  SectionHeader.sectionSize);
   HW_MNGRG_OPEN_calib_file(flashNum,SectionHeader.sectionDataSize,flashOffset);

   HW_REGSG_clearCommandTable();
   inu_device__loadCalibParams(configFolder,configMode);
   return ret;
}
/****************************************************************************
*
*  Function Name: inu_device__gpCalibrationHdrIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpCalibrationHdrIoctl(inu_device *me, INUG_ioctlCalibrationHdrRdT *calibrationHdrParamsP)
{
   ERRG_codeE        ret = SYSTEM__RET_SUCCESS;
   InuStorageLegacyLayoutT StorageLegacyLayout;
   InuSectionHeaderT       SectionHeader,dynCalibSectionHeader,*currCalibHdr=&SectionHeader;
   UINT8 flashNum = MAIN_FLASH;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();

   if ((modelType >= INU_DEFSG_BOOT82_E) && (modelType <= INU_DEFSG_BOOT87_E))
      flashNum=SECONDARY_FLASH;

   calibrationHdrParamsP->calibrationDataFormat = 0;
   calibrationHdrParamsP->calibrationDataSize   = 0;
   calibrationHdrParamsP->calibrationTimestamp  = -1;
       //read calibration section header
   ret = inu_device__gpReadFlashChunk(flashNum, SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) - sizeof(InuSectionHeaderT), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&SectionHeader, me);

   if(SectionHeader.magicNumber==SECTION_CALIBRATION_MAGIC_NUMBER)
   {
      ret = inu_device__gpReadFlashChunk(flashNum,
                                       (SPI_FLASHG_getBlockSize(flashNum) )*(SPI_FLASHG_getNumBlocks(flashNum)-SectionHeader.sectionSize)- sizeof(InuSectionHeaderT),
                                       sizeof(InuSectionHeaderT), FALSE,
                                       (UINT8 *)&dynCalibSectionHeader,
                                       me);
      if (dynCalibSectionHeader.magicNumber == SECTION_DYN_CALIBRATION_MAGIC_NUMBER)
      {
         currCalibHdr  = &dynCalibSectionHeader;
         LOGG_PRINT(LOG_INFO_E, NULL, "Loading Dynamic calibration HDR offset 0x%x\n",(SPI_FLASHG_getBlockSize(flashNum) )*(SPI_FLASHG_getNumBlocks(flashNum)-SectionHeader.sectionSize)- sizeof(InuSectionHeaderT));
      }
      else
      {
        LOGG_PRINT(LOG_INFO_E, NULL, "Loading Factory Calibration HDR offset 0x%x\n",SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) - sizeof(InuSectionHeaderT));
      }

      calibrationHdrParamsP->calibrationDataFormat = currCalibHdr->sectionFormat;
      calibrationHdrParamsP->calibrationDataSize   = currCalibHdr->sectionDataSize;
      calibrationHdrParamsP->calibrationTimestamp  = currCalibHdr->timestamp;
      return ret;

   }
   else LOGG_PRINT(LOG_ERROR_E, NULL, "No valid calibration data \n");
   return ret;
}

static ERRG_codeE inu_device__gpEraseDynCalibration(inu_device *me, INUG_ioctlEraseDynCalibrationT *EraseDynCalibration,UINT32 dynSectionNewSize)
{
   ERRG_codeE        ret = SYSTEM__RET_SUCCESS;
   InuSectionHeaderT       SectionHeader,dynCalibhdr;
   UINT8 flashNum = MAIN_FLASH;
   UINT32 dynCalibHdrAddr,numBlocks,dynEraseSize=dynSectionNewSize;
   SPI_FLASH_DRVG_flashParametersT flashParameters;
    

   ret = inu_device__gpReadFlashChunk(flashNum, SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) - sizeof(InuSectionHeaderT), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&SectionHeader, me);

   if(SectionHeader.magicNumber == SECTION_CALIBRATION_MAGIC_NUMBER)
   {
      ret = inu_device__gpReadFlashChunk(flashNum, 
                            SPI_FLASHG_getBlockSize(flashNum)*(SPI_FLASHG_getNumBlocks(flashNum) - SectionHeader.sectionSize) - sizeof(InuSectionHeaderT), 
                            sizeof(InuSectionHeaderT),
                            FALSE, 
                            (UINT8 *)&dynCalibhdr,
                            me); 
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "inu_device__gpReadFlashChunk Fail \n");
         if (dynSectionNewSize == 0)
            return ret;
      }
      if (dynCalibhdr.magicNumber != SECTION_DYN_CALIBRATION_MAGIC_NUMBER)
      {
         return ret; // no dynamic calibration
      }
  
   }
   else 
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "No valid calibration data \n");
      return SYSTEM__ERR_NOT_SUPPORTED;
   }
   if (dynCalibhdr.sectionSize > dynSectionNewSize )
       dynEraseSize = dynCalibhdr.sectionSize;
   inu_device__setFlashGpio(MAIN_FLASH);


   memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

   flashParameters.flashProgram.startFlashAddress  =   SPI_FLASHG_getBlockSize(flashNum)*(SPI_FLASHG_getNumBlocks(flashNum) - SectionHeader.sectionSize - dynEraseSize);
   flashParameters.flashProgram.writeFlashSize     =   SPI_FLASHG_getBlockSize(flashNum)*dynEraseSize;

  
   ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E, &flashParameters);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "fail: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, ret, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E address %x size %x\n",
                 flashParameters.flashProgram.startFlashAddress,
                 flashParameters.flashProgram.writeFlashSize);
   }


}
/****************************************************************************
*
*  Function Name: inu_device__gpCalibrationRdIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpCalibrationRdIoctl(inu_device *me, INUG_ioctlCalibrationRdT *calibrationRdParamsP)
{
   inu_device__privData    *privP = (inu_device__privData*)me->privP;
   ERRG_codeE              ret = INU_DEVICE__RET_SUCCESS;
   InuStorageLegacyLayoutT StorageLegacyLayout;
   InuSectionHeaderT       SectionHeader,dynCalibSectionHeader;
   UINT8 flashNum = MAIN_FLASH;
   UINT32 blockOffset;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();

   if ((modelType >= INU_DEFSG_BOOT82_E) && (modelType <= INU_DEFSG_BOOT87_E))
     flashNum=SECONDARY_FLASH;


   (void )calibrationRdParamsP;
   ret = inu_device__gpReadFlashChunk(flashNum, SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) -  sizeof(InuSectionHeaderT), 
                                      sizeof(InuSectionHeaderT), 
                                      FALSE, 
                                      (UINT8 *)&SectionHeader, 
                                      me);
   if (SectionHeader.magicNumber == SECTION_CALIBRATION_MAGIC_NUMBER)
   {
      blockOffset =  SPI_FLASHG_getBlockSize(flashNum)*(SPI_FLASHG_getNumBlocks(flashNum) -  SectionHeader.sectionSize); // calib offset
      ret = inu_device__gpReadFlashChunk(flashNum,  blockOffset - sizeof(InuSectionHeaderT), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&dynCalibSectionHeader, me);

       if (dynCalibSectionHeader.magicNumber == SECTION_DYN_CALIBRATION_MAGIC_NUMBER)
      {

        blockOffset = blockOffset - SPI_FLASHG_getBlockSize(flashNum)*(dynCalibSectionHeader.sectionSize);
        ret = inu_device__gpReadFlashChunk(flashNum, blockOffset, dynCalibSectionHeader.sectionDataSize, TRUE, &privP->tempReadBuf[0], me);
        LOGG_PRINT(LOG_INFO_E, NULL, "Loading dynamic calibration Data offset 0x%x\n",blockOffset);
        return ret;
      }
      else
      {
        ret = inu_device__gpReadFlashChunk(flashNum, blockOffset, SectionHeader.sectionDataSize, TRUE, &privP->tempReadBuf[0], me);
        LOGG_PRINT(LOG_INFO_E, NULL, "Loading factory calibration Data offset 0x%x\n",blockOffset);
        return ret;
      }

   }
   else LOGG_PRINT(LOG_ERROR_E, NULL, "wrong calibration header \n");
   return SYSTEM__ERR_FLASH_IO_FLASH_HANDLE_IS_NULL;
}

static ERRG_codeE inu_device__gpBurnDynCalibration(inu_device *me, InuSectionHeaderT *pDynCalibSectionHeader,UINT8 *calibrationDataPtr)
 {

  
   ERRG_codeE        ret = SYSTEM__RET_SUCCESS;
   InuSectionHeaderT       SectionHeader;
   UINT8 flashNum = MAIN_FLASH,*hdrBlock;
   UINT32 dynCalibHdrAddr,numBlocks;
   SPI_FLASH_DRVG_flashParametersT flashParameters;
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   INUG_ioctlEraseDynCalibrationT eraseDynCaliBurn;
   
   eraseDynCaliBurn.calibType=0;
   ret = inu_device__gpReadFlashChunk(flashNum, SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) - sizeof(InuSectionHeaderT), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&SectionHeader, me);

   if(SectionHeader.magicNumber==SECTION_CALIBRATION_MAGIC_NUMBER)
   {
      pDynCalibSectionHeader->magicNumber = SECTION_DYN_CALIBRATION_MAGIC_NUMBER;
  
   }
   else 
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "No valid calibration data \n");
      return ret;
   }

  
   inu_device__setFlashGpio(MAIN_FLASH);

 
   numBlocks = (pDynCalibSectionHeader->sectionDataSize + sizeof(InuSectionHeaderT)) / SPI_FLASHG_getBlockSize(flashNum);
   if ((pDynCalibSectionHeader->sectionDataSize + sizeof(InuSectionHeaderT)) % SPI_FLASHG_getBlockSize(flashNum))
      numBlocks++;
   pDynCalibSectionHeader->sectionSize=numBlocks;
   dynCalibHdrAddr = SPI_FLASHG_getBlockSize(flashNum)*(SPI_FLASHG_getNumBlocks(flashNum) - SectionHeader.sectionSize - 1); 

   inu_device__gpEraseDynCalibration(me, &eraseDynCaliBurn, pDynCalibSectionHeader->sectionSize);
   
   memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

   
   flashParameters.flashProgram.startFlashAddress  = dynCalibHdrAddr - SPI_FLASHG_getBlockSize(flashNum)*(numBlocks-1);
   flashParameters.flashProgram.writeFlashSize     = pDynCalibSectionHeader->sectionDataSize - pDynCalibSectionHeader->sectionDataSize%SPI_FLASHG_getBlockSize(flashNum);
   flashParameters.flashProgram.flashBufP          = calibrationDataPtr;
   
   if (numBlocks > 1) // header is not included in the block
   {
      ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
      if (ERRG_FAILED(ret))
      { 
           LOGG_PRINT(LOG_ERROR_E, NULL, "fail: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E adresss %x size %x\n",
                    flashParameters.flashProgram.startFlashAddress,
                    flashParameters.flashProgram.writeFlashSize);
           return ret;
      }
      else
      {
          LOGG_PRINT(LOG_INFO_E, NULL, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E adresss %x size %x\n",
                    flashParameters.flashProgram.startFlashAddress,
                    flashParameters.flashProgram.writeFlashSize);
      }
   }
   hdrBlock = malloc(SPI_FLASHG_getBlockSize(flashNum));
   if (!hdrBlock)
      return SYSTEM__ERR_OUT_OF_MEM;
   memset(hdrBlock,0,SPI_FLASHG_getBlockSize(flashNum));
   memcpy(hdrBlock,
         calibrationDataPtr+(numBlocks-1)*SPI_FLASHG_getBlockSize(flashNum),
         pDynCalibSectionHeader->sectionDataSize%SPI_FLASHG_getBlockSize(flashNum));

   memcpy(hdrBlock + SPI_FLASHG_getBlockSize(flashNum) - sizeof(InuSectionHeaderT),pDynCalibSectionHeader,sizeof(InuSectionHeaderT));
   
   flashParameters.flashProgram.startFlashAddress  = dynCalibHdrAddr;
   flashParameters.flashProgram.writeFlashSize     = SPI_FLASHG_getBlockSize(flashNum);
   flashParameters.flashProgram.flashBufP          = hdrBlock;

 
   if (numBlocks > 1) // header is not included in the block
   {
      ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
      if (ERRG_FAILED(ret))
      {
           LOGG_PRINT(LOG_ERROR_E, NULL, "fail: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E adresss %x size %x\n",
                    flashParameters.flashProgram.startFlashAddress,
                    flashParameters.flashProgram.writeFlashSize);
           return ret;
      }
      else
      {
          LOGG_PRINT(LOG_INFO_E, ret, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E adresss %x size %x\n",
                    flashParameters.flashProgram.startFlashAddress,
                    flashParameters.flashProgram.writeFlashSize);
      }
   }

   free(hdrBlock);
   return ret;

 }
       
/****************************************************************************
*
*  Function Name: inu_device__gpPowerMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpPowerMode(inu_device *me, inu_device__powerModeT *powerModeP)
{
   FIX_UNUSED_PARAM_WARN(me);
   powerModeP->disableUnits &= HCG_MNGRG_HW_ALL_UNITS;
   LOGG_PRINT(LOG_INFO_E, NULL, "power mode: disable units voting 0x%x\n",powerModeP->disableUnits);
   HCG_MNGRG_voteUnits(powerModeP->disableUnits);
   HCA_MNGRG_disableEnable(powerModeP->disableUnits);
   return INU_DEVICE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: inu_device__gpGetVersionIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpGetVersionIoctl(inu_device *me, inu_device__version *getVersionParamsP)
{
   ERRG_codeE               ret = INU_DEVICE__RET_SUCCESS;
   UINT32                   ind;
   inu_device__hwVersionIdU hwVersion;
   InuStorageLegacyLayoutT  StorageLegacyLayout;
   InuProductionHeaderT         ProductionHeader,*regPrdhdrPtr;
   InuExtendedProductionHeaderT ExtendedProductionHeader;
   InuSectionHeaderT        BootfixSectionHeader, ProductionSectionHeader;
   INU_DEFSG_moduleTypeE modelType=GME_MNGRG_getModelType();
   UINT8 flashNum = MAIN_FLASH;

   char *freqStr[INU_DEVICE__PLL_NUM_PLLS_E]={"AUDIO","CPU","DDR","DSP","SYS","USB2","USB3"};


   // FW version
   getVersionParamsP->fwVerId.fields.major    = MAJOR_VERSION;
   getVersionParamsP->fwVerId.fields.minor    = MINOR_VERSION;
   getVersionParamsP->fwVerId.fields.build    = BUILD_VERSION;
   getVersionParamsP->fwVerId.fields.subBuild = SUB_BUILD_VERSION;

   // HW version
   HW_MNGRG_getHwVersion(&hwVersion);
   getVersionParamsP->hwVerId.val = hwVersion.val;

   memset (getVersionParamsP->masterSensorId,0,sizeof(getVersionParamsP->masterSensorId));
   memset (getVersionParamsP->slaveSensorId,0,sizeof(getVersionParamsP->slaveSensorId));
   memset (getVersionParamsP->ispVersion,0,sizeof(getVersionParamsP->ispVersion));

   // interface data
   ret = HW_MNGRG_getUsbSpeed(&getVersionParamsP->usbSpeed);

   VERSION_PRINT;
   HW_MNGRG_printHwVersion();

   for (ind=0;ind<INU_DEVICE__PLL_NUM_PLLS_E;ind++)
       getVersionParamsP->pllFreq[ind] = GME_DRVG_calc_pll(ind);
   LOGG_PRINT(LOG_INFO_E, NULL, "Frequencies: %s=%dM %s=%dM %s=%dM %s=%dM %s=%dM %s=%dM %s=%dM\n",
           freqStr[INU_DEVICE__PLL_AUDIO_E], getVersionParamsP->pllFreq[INU_DEVICE__PLL_AUDIO_E] /1000000 ,
           freqStr[INU_DEVICE__PLL_CPU_E],   getVersionParamsP->pllFreq[INU_DEVICE__PLL_CPU_E]   /1000000 ,
           freqStr[INU_DEVICE__PLL_DDR_E],   getVersionParamsP->pllFreq[INU_DEVICE__PLL_DDR_E]   /1000000 ,
           freqStr[INU_DEVICE__PLL_DSP_E],   getVersionParamsP->pllFreq[INU_DEVICE__PLL_DSP_E]   /1000000 ,
           freqStr[INU_DEVICE__PLL_SYS_E],   getVersionParamsP->pllFreq[INU_DEVICE__PLL_SYS_E]   /1000000 ,
           freqStr[INU_DEVICE__PLL_USB2_E],  getVersionParamsP->pllFreq[INU_DEVICE__PLL_USB2_E]  /1000000 ,
           freqStr[INU_DEVICE__PLL_USB3_E],  getVersionParamsP->pllFreq[INU_DEVICE__PLL_USB3_E]  /1000000);


   memset(getVersionParamsP->modelNumber, 0, INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH);
   memset(getVersionParamsP->partNumber, 0, INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH);
   memset(getVersionParamsP->serialNumber, 0, INU_DEFSG_IOCTL_EXT_PRODUCTION_STRING_LENGTH);
   memset(getVersionParamsP->sensorRevision, 0, INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH);
   memset(getVersionParamsP->bootfixVersion, 0, INU_DEFSG_IOCTL_BOOTFIX_STRING_LENGTH);
   getVersionParamsP->bootfixTimestamp = -1;
   getVersionParamsP->bootId  = GME_MNGRG_getFullModelType(); //take from DTB (bootId can be forced to other then flash) Contains Boot ID and Base version
   getVersionParamsP->baseVersion  = GME_MNGRG_getBaseVersion();

   // Set the type of the device (Master/Slave)
   if(HELSINKI_getMaster())
   {
      getVersionParamsP->isMaster = true;
   }
      //read bootfix section header
   ret = inu_device__gpReadFlashChunk(flashNum, BOOTFIX_DATA_SIZE, sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&BootfixSectionHeader, me);

   //read production section header
   ret = inu_device__gpReadFlashChunk(flashNum, inu_device__gpGetFlashOffset(flashNum, SECTION_PRODUCTION_START_BLOCK), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&ProductionSectionHeader, me);


   if(ProductionSectionHeader.magicNumber==SECTION_PRODUCTION_MAGIC_NUMBER)
   {
      //read production
      ret = inu_device__gpReadFlashChunk(flashNum, inu_device__gpGetFlashOffset(flashNum, SECTION_PRODUCTION_START_BLOCK) + sizeof(InuSectionHeaderT), sizeof(InuProductionHeaderT), FALSE, (UINT8 *)&ProductionHeader, me);
      strcpy((char *)getVersionParamsP->modelNumber, ProductionHeader.modelNumber);
      strcpy((char *)getVersionParamsP->partNumber, ProductionHeader.partNumber);
      strcpy((char *)getVersionParamsP->serialNumber, ProductionHeader.serialNumber);
      strcpy((char *)getVersionParamsP->sensorRevision, ProductionHeader.sensorRevision);
      if (BootfixSectionHeader.magicNumber == SECTION_BOOTFIX_MAGIC_NUMBER)
      {
         strcpy((char *)getVersionParamsP->bootfixVersion,BootfixSectionHeader.versionString);
         getVersionParamsP->bootfixTimestamp = BootfixSectionHeader.timestamp;
         BootfixSectionHeader.versionString[VERSION_STRING_SIZE-1]=0;
         LOGG_PRINT(LOG_INFO_E, NULL, "Bootfix version %s bootfixTimestamp 0x%x\n",BootfixSectionHeader.versionString,BootfixSectionHeader.timestamp);
      }

      if ((modelType >= INU_DEFSG_BOOT82_E) && (modelType <= INU_DEFSG_BOOT87_E))
      {
         ret = inu_device__gpReadFlashChunk(SECONDARY_FLASH, inu_device__gpGetFlashOffset(SECONDARY_FLASH, SECTION_PRODUCTION_START_BLOCK), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&ProductionSectionHeader, me);
         if(ProductionSectionHeader.magicNumber==SECTION_PRODUCTION_MAGIC_NUMBER)
         {
            //read production
            ret = inu_device__gpReadFlashChunk(SECONDARY_FLASH, inu_device__gpGetFlashOffset(SECONDARY_FLASH, SECTION_PRODUCTION_START_BLOCK) + sizeof(InuSectionHeaderT), sizeof(InuExtendedProductionHeaderT), FALSE, (UINT8 *)&ExtendedProductionHeader, me);
            // overwrite serial number from small flash
            if (ExtendedProductionHeader.extKey == EXT_KEY_NUM)
               strncpy((char *)getVersionParamsP->serialNumber,ExtendedProductionHeader.serialNumber, sizeof(getVersionParamsP->serialNumber)-1);
            else
            {
               regPrdhdrPtr = (InuProductionHeaderT *)&ExtendedProductionHeader; // regular production header
               strncpy((char *)getVersionParamsP->serialNumber,regPrdhdrPtr->serialNumber, sizeof(getVersionParamsP->serialNumber)-1);
            }
         }
      }
      LOGG_PRINT(LOG_INFO_E, NULL, "Production: modelNumber %s partNumber %s serialNumber %s\n",
            getVersionParamsP->modelNumber,getVersionParamsP->partNumber, getVersionParamsP->serialNumber);
      return ret;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "wrong storage magic number \n");
   }

   LOGG_PRINT(LOG_ERROR_E, NULL, "No valid production data \n");

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_device__gpProjectorSetIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__gpProjectorSetIoctl(INUG_ioctlProjectorSetModeT *projectorMode)
{
   ERRG_codeE       ret = SYSTEM__RET_SUCCESS;
   IO_HANDLE        projHandle = IO_PALG_getHandle(IO_PROJ_0_E + projectorMode->projSelect);
   PROJ_DRVG_projSetValParamsT projSetValParams;

   LOGG_PRINT(LOG_DEBUG_E, ret, "SYSTEMP_gpProjectorSetIoctl message mode 0x%x projector number %d\n",projectorMode->projSetMode,projectorMode->projSelect);

   projSetValParams.projNum    = projectorMode->projSelect;
   projSetValParams.projState  = projectorMode->projSetMode;
   ret = IO_PALG_ioctl(projHandle, PROJ_DRVG_CHECK_SUPPORT_CMD_E, &projSetValParams);
   if (ERRG_SUCCEEDED(ret))
   {
      //setting the projector affect depth processing time
      HCG_MNGRG_startPrimePeriod();

      if(projHandle)
      {
         //TODO - bind through HW xml the projector to the sensor. for now assume sensor 0 is the group for the IR
         SENSORS_MNGRG_sensorInfoT *sensorInfoP = SENSORS_MNGRG_getSensorInfo(INU_DEFSG_SENSOR_0_E);

          //if sensor is active, then we schedule the change to the strobe interrupt
         if ((sensorInfoP) && (sensorInfoP->sensorActivity))
         {
            sensorInfoP->pendingProjectorChange = 1;
            sensorInfoP->pendingProjectorState = projectorMode->projSetMode;
         }
         else
         {
            ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_PROJ_0_E + projectorMode->projSelect), PROJ_DRVG_SET_PROJ_VAL_CMD_E, &projSetValParams);
         }
      }
      else
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "can't set projector. No projector connected");
      }
   }
   return ret;

}

/****************************************************************************
*
*  Function Name: inu_device__gpProjectorGetIoctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpProjectorGetIoctl(INUG_ioctlProjectorGetModeT *projectorMode)
{
   ERRG_codeE       ret = SYSTEM__RET_SUCCESS;
   IO_HANDLE        projHandle = IO_PALG_getHandle(IO_PROJ_0_E + projectorMode->projSelect);
   PROJ_DRVG_projGetValParamsT projGetValParams;

   LOGG_PRINT(LOG_INFO_E, ret, "SYSTEMP_gpProjectorGetIoctl message mode 0x%x projector number %d\n",projectorMode->projGetMode,projectorMode->projSelect);

   if(projHandle)
   {
      projGetValParams.projNum    = projectorMode->projSelect;
      ret = IO_PALG_ioctl(projHandle, PROJ_DRVG_GET_PROJ_VAL_CMD_E, &projGetValParams);
      if(ERRG_SUCCEEDED(ret))
      {
         projectorMode->projGetMode = projGetValParams.projState;
         LOGG_PRINT(LOG_INFO_E, NULL, "Get projector SUCCESS. Projector number 0x%x mode %d \n",projectorMode->projSelect, projectorMode->projGetMode);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "fail to Get projector. Projector number 0x%x mode %d \n",projectorMode->projSelect, projectorMode->projGetMode);
      }

   }
   else
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "can't Get projector. No projector connected");
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_device__allocRemoteBuf
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpAllocBuf(inu_device *me, INTERNAL_CMDG_AllocGpBufHdrT *hdrP)
{
   ERRG_codeE ret;
   inu_device__privData *privP = (inu_device__privData*)me->privP;


   LOGG_PRINT(LOG_INFO_E, NULL, "Trying to allocate CMEM buffer -> ID %d, size %d\n", hdrP->bufId, hdrP->bufSize);

   ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_CMEM_E, hdrP->bufSize + 2048, 1, NULL, 0, &privP->poolH);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to alloc memory for ID %d, size %d\n",hdrP->bufId,hdrP->bufSize);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "alloc success! ID %d, size %d\n",hdrP->bufId,hdrP->bufSize);
   }
   return ret;
}


static ERRG_codeE inu_device__gpFreeBuf(inu_device *me, INTERNAL_CMDG_FreeGpBufHdrT *hdrP)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)me->privP;

   MEM_POOLG_free((MEM_POOLG_bufDescT *)hdrP->bufDescrP);
   ret = inu_ref__removeMemPool((inu_ref*)me, privP->poolH);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "free failed! Descr %p\n",hdrP->bufDescrP);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "free success! Descr %p\n",hdrP->bufDescrP);
   }
   return ret;
}



static ERRG_codeE inu_device__gpHandleFileWrite(inu_device *me, INTERNAL_CMDG_SendGpFileHdrT *msgHdrP, MEM_POOLG_bufDescT *bufDescP)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   size_t     sizeSaved;

   //create a file
   FILE* newFile = fopen(msgHdrP->filename, "wb");
   if (!newFile)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Could not create a file %s\n", msgHdrP->filename);
      return INU_DEVICE__ERR_UNEXPECTED;
   }

   sizeSaved = fwrite(bufDescP->dataP, 1, bufDescP->dataLen, newFile);
   if (sizeSaved != bufDescP->dataLen)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Could not save content file %s. size %d, sizeSaved %d\n", msgHdrP->filename,bufDescP->size,sizeSaved);
      return INU_DEVICE__ERR_UNEXPECTED;
   }
   fflush(newFile);
   fclose(newFile);

   LOGG_PRINT(LOG_INFO_E, NULL, "file %s size %d saved\n", msgHdrP->filename, bufDescP->dataLen);
   if (!(inu_ref__standAloneMode(me)))
   {
      ret = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_SEND_FILE_TO_GP_BUF_E, msgHdrP, NULL, 0);
   }

   return ret;
}

static ERRG_codeE inu_device__gpRequestFile(inu_device *me, INTERNAL_CMDG_SendGpFileHdrT *msgHdrP, MEM_POOLG_bufDescT *bufDescP)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__readWriteBuf bufChunkHdr;
   //create a file
   FILE* file = fopen(msgHdrP->filename, "rb");
   if (!file)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Could not open file %s\n", msgHdrP->filename);
      return INU_DEVICE__ERR_UNEXPECTED;
   }

   fseek(file, 0, SEEK_END);
   bufChunkHdr.bufLen = ftell(file);
   fseek(file, 0, SEEK_SET);
   bufChunkHdr.buf = (UINT8*)malloc(bufChunkHdr.bufLen); 
   fread(bufChunkHdr.buf , bufChunkHdr.bufLen, 1, file);

   fflush(file);
   fclose(file);

   LOGG_PRINT(LOG_INFO_E, NULL, "File %s, size %d\n", msgHdrP->filename, bufChunkHdr.bufLen);
   
   inu_device__gpHandleReadBufMsg(me, &bufChunkHdr);

   free(bufChunkHdr.buf);

   return ret;
}

static ERRG_codeE inu_device__gpPowerDown(inu_device *me, inu_device__powerDownT* pdParams)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)me->privP;

   switch (pdParams->cmd)
   {
      case INU_DEVICE_POWER_DOWN_WAIT_FOR_EVENT_E:
         LOGG_PRINT(LOG_INFO_E, NULL, "start waiting for power down event\n");
         PWR_MNGRG_waitForEvent((inu_deviceH)me);
         break;

      case INU_DEVICE_POWER_DOWN_PREPARE_E:
         LOGG_PRINT(LOG_INFO_E, NULL, "prepare GP to power down\n");
         privP->pdIsActive = TRUE;
         break;

      default:
         ret = SYSTEM__ERR_INVALID_ARGS;
         LOGG_PRINT(LOG_ERROR_E, ret, "command %d is not valid option\n", pdParams->cmd);
   }

   return ret;
}

static ERRG_codeE inu_device__gpEnableTuningServer(inu_device *me, inu_device__tuningParamsT* tsParams)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   inu_refH node = NULL;
   char name[10];
   sprintf(&name[0], "SENSOR_%d", tsParams->sensorNum);
   inu_device__findNode(me, name, &node);

   ret = TUNING_SERVERG_startTuningThread(tsParams->sensorNum, node);

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_device__allocRemoteBuf
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_device__gpRawNandWrite(INTERNAL_CMDG_UpdateRawNandSecT *nandUpdtParam)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   NAND_STRGG_ioParamT nandParam;
   InuStorageNandImgVerT ver;
   UINT32 i = 0;
   UINT8 *bootfixMetaSectionP;
   InuBootfixHeaderT *bootfixHdrP;
   UINT32 bootfixMetaSize;
   InuSectionHeaderT* secthdr;
   inu_device_memory_t buf;

   if (inu_device__readBinFile(nandUpdtParam->filename, &buf) == FALSE)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"failed to open file %s for %d\n",nandUpdtParam->filename, nandUpdtParam->type);
      return INU_DEVICE__ERR_UNEXPECTED;
   }

   nandParam.buf = buf.bufP;
   nandParam.length = buf.bufsize;
   nandParam.type = nandUpdtParam->type;

   ver.major = nandUpdtParam->verMajor;
   ver.minor = nandUpdtParam->verMinor;
   ver.build = nandUpdtParam->verBuild;

   ret =  NAND_STRGG_updateSection(&nandParam, &ver);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "Failure in NAND update section, abort\n");
      return ret;
   }
   if( nandUpdtParam->updatedSectionsFlags != 0)   // Update the last section of the fwUpdate request
   {

      bootfixMetaSize = (sizeof(InuSectionHeaderT) + sizeof(InuBootfixHeaderT)); // meta section contains only two stuctures
      bootfixMetaSectionP = malloc(bootfixMetaSize);
      ret = inu_device__gpReadBootfixMetaSection(bootfixMetaSectionP, bootfixMetaSize);
      bootfixHdrP = (InuBootfixHeaderT*)(bootfixMetaSectionP + sizeof(InuSectionHeaderT));
      secthdr = (InuSectionHeaderT*)(bootfixMetaSectionP);
      printf("inu_device__gpReadBootfixMetaSection: Magic %x, dataSize %x\n", secthdr->magicNumber, secthdr->sectionDataSize);

      //bootfixHdrP->nandNewImages = 1;
      bootfixHdrP->bootCounter = 0;
      bootfixHdrP->nandBootSuccess = 0;

      for(i = 0; i < INU_DEVICE_PRODUCT_TYPE_MAXIMUM_E; i++)
      {

         if((nandUpdtParam->updatedSectionsFlags >> i) & 0x01)
         {
            if(i == INU_DEVICE_PRODUCT_TYPE_BOOTSPL_E)
            {
               bootfixHdrP->loadSplFromNor = 0;
            }
            else if((i >= INU_DEVICE_PRODUCT_TYPE_LXKERNEL_E) && (i <= INU_DEVICE_PRODUCT_TYPE_APPLICATION_E))
            {
               bootfixHdrP->bootFromFactoryDefault = 0;
            }
         }
      }

      printf("\tinu_device__gpRawNandWrite: FacDef %d, BS %d, Counter %d\n", bootfixHdrP->bootFromFactoryDefault, bootfixHdrP->nandBootSuccess, bootfixHdrP->bootCounter);
      ret = inu_device__gpWriteBootfixMetaSection(bootfixMetaSectionP, bootfixMetaSize);
      LOGG_PRINT(LOG_INFO_E, NULL,YELLOW("Finished to write NOR flags\n"));
      free(bootfixMetaSectionP);
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_device__gpEepromAccess
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__gpEepromAccess(inu_device *me, inu_device__eepromAccessT *eepromAccess, UINT8 *data)
{
   inu_device__privData       *privP = (inu_device__privData*)me->privP;
   ERRG_codeE                 retCode = SYSTEM__RET_SUCCESS;
   UINT32                     i;

   if (eepromAccess->operation == INU_DEVICE_EEPROM_READ_E)
   {
      retCode = EEPROM_CTRLG_readFromEeprom((INU_DEFSG_senSelectE)eepromAccess->sensorId, 0, eepromAccess->size, privP->tempReadBuf);
      if (ERRG_SUCCEEDED(retCode))
      {
         retCode = inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_EEPROM_READ_E, eepromAccess, &privP->tempReadBuf[0],eepromAccess->size);
      }
   }
   else if (eepromAccess->operation == INU_DEVICE_EEPROM_WRITE_E)
   {
      retCode = EEPROM_CTRLG_writeToEeprom((INU_DEFSG_senSelectE)eepromAccess->sensorId, 0, eepromAccess->size, data);
   }
   return retCode;
}

/****************************************************************************
*
*  Function Name: inu_device__gpCheckFwUpdateBootSuccess
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
#define NAND_BOOT_RETRIES_MAX  (3)  //2 NOTE: the same define as in bootSPL


ERRG_codeE inu_device__gpCheckFwUpdateBootSuccess(void)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   UINT8 *bootfixMetaSectionP;
   InuBootfixHeaderT *bootfixHdrP;
   UINT32 bootfixMetaSize;
   InuSectionHeaderT* secthdr;

   if(NAND_STRGG_isDevPresent() == FALSE)
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "NAND storage is not present\n");
      return INU_DEVICE__RET_SUCCESS;
   }

   bootfixMetaSize = (sizeof(InuSectionHeaderT) + sizeof(InuBootfixHeaderT));   // meta section contains only two stuctures
   bootfixMetaSectionP = malloc(bootfixMetaSize);
   ret = inu_device__gpReadBootfixMetaSection(bootfixMetaSectionP, bootfixMetaSize);
   bootfixHdrP = (InuBootfixHeaderT*)(bootfixMetaSectionP + sizeof(InuSectionHeaderT));
   secthdr = (InuSectionHeaderT*)bootfixMetaSectionP;
   printf("inu_device__gpReadBootfixMetaSection: Magic %x, dataSize %x\n", secthdr->magicNumber, secthdr->sectionDataSize);

   if(bootfixHdrP->nandBootSuccess == 1)  // Nothing to do. Previous boot already succeeded
   {
      return INU_DEVICE__RET_SUCCESS;
   }
   else                          //bootfixHdrP->nandBootSuccess = 0
   {
      if(bootfixHdrP->bootCounter < NAND_BOOT_RETRIES_MAX)  // FW update succeeded. Set nandBootSuccess
      {
         bootfixHdrP->bootFromFactoryDefault = 0;
      }
      else                                      // something wrong. FW update did not succeeded. Switch to factory
      {
         bootfixHdrP->bootFromFactoryDefault = 1;
      }

      bootfixHdrP->bootCounter  = 0;
      bootfixHdrP->nandBootSuccess = 1;

      ret = inu_device__gpWriteBootfixMetaSection(bootfixMetaSectionP, bootfixMetaSize);
      LOGG_PRINT(LOG_INFO_E, NULL,"Finished to write NOR flags\n");
   }
   return ret;
}

ERRG_codeE inu_device__gpSetFlashSectionHdr(inu_device *me, inu_device__setflashSectionHdr *flashHdr)
{
    inu_device__privData* privP = (inu_device__privData*)(me)->privP;
    ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
    
    if (privP->calibBufVirtAddress != 0)
      LOGG_PRINT(LOG_ERROR_E, NULL,"Burning dynamic calibration before last burn finished\n");
    memcpy(&savedflashHeader, flashHdr, sizeof(inu_device__setflashSectionHdr));
    return ret;
}

#endif   //DEFSG_IS_GP

#if DEFSG_IS_HOST


#endif

static BOOLEAN inu_device__readBinFile(const char *fileName, inu_device_memory_t *buffer)
{
    FILE *file;
    file = fopen(fileName, "rb");  // r for read, b for binary
    if (file)
    {
        fseek(file, 0, SEEK_END);
        buffer->bufsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer->bufP = (char *)malloc((buffer->bufsize) + 1);
        fread(buffer->bufP, buffer->bufsize, 1, file);
        fclose(file);
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

static ERRG_codeE inu_device__waitForAck(BOOL *ackFlag, UINT32  timeoutMsec)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   UINT32  startSec, deltaMsec = 0;
   UINT16  startMsec;
   //wait for ACK
   OS_LYRG_getTime(&startSec, &startMsec);
   do
   {
       if (*ackFlag == TRUE)
       {
           *ackFlag = FALSE;
           break;
       }
       OS_LYRG_usleep(INU_DEVICE__SLEEP_USEC);
       deltaMsec = OS_LYRG_deltaMsec(startSec, startMsec);
   } while (deltaMsec <= (timeoutMsec* INU_DEVICE__SLEEP_USEC));

   ret = (deltaMsec > timeoutMsec) ? (INU_DEVICE__ERR_TIMEOUT) : (INU_DEVICE__RET_SUCCESS);
   return ret;
}

static ERRG_codeE inu_device__createNew(inu_device *me, void *msgP, int msgCode)
{
   inu_ref *newObj;
   inu_ref__CtorParams *refParamsP = (inu_ref__CtorParams*)msgP;
   refParamsP->device = me;
   return inu_factory__new(&newObj, msgP, inu_factory__remoteNewCmdToType(msgCode));
}

 void inu_device__fillDeviceRefList(void *me, void *msgP)
{
   UINT32 ind;
   inu_device__refList *refList=(inu_device__refList *)msgP;
   inu_device__privData *privP = (inu_device__privData*)(((inu_device *)me)->privP);
   refList->numberOfRefs = 0;
   for (ind=0;ind<INU_DEVICE__MAX_NUM_OF_REFS;ind++)
   {
      if (privP->ref_list[ind] != NULL)
      {
         refList->refMember[ind].id =  privP->ref_list[ind]->id;
         refList->refMember[ind].refType = privP->ref_list[ind]->refType;
         refList->numberOfRefs++;
      }
   }
}

void inu_device__getDeviceRefList(void *me)
{
   inu_device__refList refList;
   UINT32 ind;

   inu_ref__sendCtrlSync((inu_ref*)me, INTERNAL_CMDG_GET_DEVICE_REF_LIST_E, &refList, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   printf("----NUM REFS %d----\n",refList.numberOfRefs);
   for (ind=0;ind<refList.numberOfRefs;ind++)
   {
      printf("id %d type %d\n",refList.refMember[ind].id,refList.refMember[ind].refType);
   }
   printf("----------------\n");
}


static ERRG_codeE inu_device__deleteRef(inu_device *me, void *msgP)
{
   inu_ref__DtorParams *refDtorParamsP = (inu_ref__DtorParams*)msgP;
   inu_ref *ref;
   ref = inu_device__getRefById(me, refDtorParamsP->ref.id);
   return inu_factory__delete(ref, 0);
}

static const char* inu_device__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}


/* destructor */
void inu_device__dtor(inu_ref *me)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)me)->privP;
   int i = 0, force = 0;
#if DEFSG_IS_GP
   OS_LYRG_releaseEvent(privP->dspSyncMsgEvent,0);
#endif//DEFSG_IS_GP
   if (privP->connectState == INU_DEVICE__DISCONNECT)
   {
      force = 1;
   }

   for (i = (INU_DEVICE__MAX_NUM_OF_REFS-1); i >= 0; i--)
   {
      if (privP->ref_list[i] && (me != privP->ref_list[i]))
      {
         inu_factory__delete(privP->ref_list[i], force);
         //inu_ref dtor should remove it from the ref_list, verify it
         assert(privP->ref_list[i] == NULL);
      }
   }

   if (privP->lmH)
      LINK_MONITORG_delete(privP->lmH);

   if (privP->calibPath)
      free(privP->calibPath);

   OS_LYRG_releaseMutex(&privP->refListMutex);

   if (privP->poolH)
   {
      MEM_POOLG_closePool(privP->poolH);
      privP->poolH = NULL;
   }

   free(privP);
}


/* Constructor */
ERRG_codeE inu_device__ctor(inu_ref *ref, inu_device__CtorParams *ctorParamsP)
{
   inu_device *me = (inu_device*)ref;
   inu_device__CtorParams *deviceCtorParamsP = (inu_device__CtorParams*)ctorParamsP;
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__privData *privP;
   privP = (inu_device__privData*)malloc(sizeof(inu_device__privData));
   me->privP = privP;
   memset(privP, 0, sizeof(inu_device__privData));
   privP->standAloneMode = ctorParamsP->standAloneMode;
 //  deviceCtorParamsP->deviceInitParams.useSharedMemory = 0; // wait for host integration
   if (deviceCtorParamsP->deviceInitParams.useSharedMemory)
      MEM_POOLG_assignShardCBFunc(deviceCtorParamsP->deviceInitParams.sharedMallocFunc,deviceCtorParamsP->deviceInitParams.sharedFreeFunc);

   deviceCtorParamsP->ref_params.device = me;
   OS_LYRG_aquireMutex(&privP->refListMutex);
   sprintf(deviceCtorParamsP->ref_params.userName, "DEVICE");
   ret = inu_ref__ctor(&me->ref, &deviceCtorParamsP->ref_params);

   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_HEAP_E, INU_DEVICE__MEM_POOL_BUFF_SIZE_BYTES, INU_DEVICE__MEM_POOL_BUFF_NUM,NULL,0, &privP->poolH);
      if (ERRG_SUCCEEDED(ret))
      {
         memcpy(&privP->ctorParams, deviceCtorParamsP, sizeof(inu_device__CtorParams));
#if DEFSG_IS_HOST
         strcpy(privP->bootPath, deviceCtorParamsP->deviceInitParams.bootPath);
#endif
      }
   }
#if DEFSG_IS_GP
   //Event for synchronizing DSP sync msg calls
   privP->dspSyncMsgEvent = OS_LYRG_createEvent(0);
   if (privP->dspSyncMsgEvent == NULL)
      assert(0);
#endif //DEFSG_IS_GP

   inu_device__init(me);

   return ret;
}

static int inu_device__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
   inu_device *me = (inu_device*)ref;
   int ret = INU_DEVICE__RET_SUCCESS;

   ret = inu_ref__vtable_get()->p_rxSyncCtrl(ref, msgP, msgCode);
   switch (msgCode)
   {
#if DEFSG_IS_GP
      case(INUG_IOCTL_SYSTEM_GET_VERSION_E):
      {
         ret = inu_device__gpGetVersionIoctl(me,(inu_device__version*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_CALIBRATION_HDR_RD_E):
      {
         ret = inu_device__gpCalibrationHdrIoctl(me,(INUG_ioctlCalibrationHdrRdT*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_CALIBRATION_RD_E):
      {
         ret = inu_device__gpCalibrationRdIoctl(me,(INUG_ioctlCalibrationRdT*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_ERASE_DYN_CALIBRATION_E):
      {
         INUG_ioctlEraseDynCalibrationT *eraseDynCaliBurn = (INUG_ioctlEraseDynCalibrationT*)msgP;
         inu_device__gpEraseDynCalibration(me, eraseDynCaliBurn,0);
      }
      break;

      case(INUG_IOCTL_SYSTEM_POWER_MODE_E):
      {
         ret = inu_device__gpPowerMode(me, (inu_device__powerModeT*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_GET_STATS_E):
      {
         inu_device__showStats(me);
         break;
      }

      case(INTERNAL_CMDG_CONFIG_WD_E):
      {
         ret = WDG_cmd(((inu_device__watchdog_timeout_t*)msgP)->sec);
         break;
      }

      case(INUG_IOCTL_SYSTEM_SET_TIME_E):
      {
         inu_device__set_time_t *setTimeParamsP = (inu_device__set_time_t*)msgP;
         if (setTimeParamsP->method != INU_DEVICE__SET_TIME_PLL_LOCK_DISABLE_E)
         {
            ret = TIMEG_start(setTimeParamsP, NULL);
         }
         else
         {
            ret = TIMEG_stop();
         }
         break;
      }

      case(INUG_IOCTL_SYSTEM_GET_TIME_E):
      {
         inu_device__get_time_t *getTimeParamsP = (inu_device__get_time_t*)msgP;
         OS_LYRG_getUsecTime(&getTimeParamsP->usec);
         break;
      }  

      case(INUG_IOCTL_SYSTEM_GET_RTC_LOCK_E):
      {
         inu_device__get_rtc_lock_t *getLockStatusP = (inu_device__get_rtc_lock_t*)msgP;
         RTC_DRVG_status(getLockStatusP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_WR_REG_E):
      {
         ret = HW_REGSG_writeReg(((inu_device__writeRegT*)msgP));
         break;
      }

      case(INUG_IOCTL_SYSTEM_RD_REG_E):
      {
         ret = HW_REGSG_readReg(((inu_device__readRegT*)msgP));
         break;
      }

      case(INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E):
      {
         ret = inu_device__gpProjectorSetIoctl((INUG_ioctlProjectorSetModeT*)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_PROJECTOR_GET_MODE_E):
      {
         ret = inu_device__gpProjectorGetIoctl((INUG_ioctlProjectorGetModeT*)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_TUNING_SERVER_E):
      {
         ret = inu_device__gpEnableTuningServer(me, (inu_device__tuningParamsT*)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_POWER_DOWN_E):
      {
         ret = inu_device__gpPowerDown(me, (inu_device__powerDownT*)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_READ_BUFFER_E):
      {
         ret = inu_device__gpHandleReadBufMsg(me, (inu_device__readWriteBuf*)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_ALTERNATE_SET_CONFIG_E):
      {
         ret = inu_device__gpSetAlternate(me, (inu_device__alternateProjCfgT*)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_EEPROM_READ_E):
      {
         ret = inu_device__gpEepromAccess(me, (inu_device__eepromAccessT*)msgP, NULL);
      }
      break;

      case(INTERNAL_CMDG_SEND_ALLOC_GP_BUF_E):
      {
         ret = inu_device__gpAllocBuf(me, (INTERNAL_CMDG_AllocGpBufHdrT *)msgP);
      }
      break;

      case(INTERNAL_CMDG_SEND_FREE_GP_BUF_E):
      {
         ret = inu_device__gpFreeBuf(me, (INTERNAL_CMDG_FreeGpBufHdrT *)msgP);
      }
      break;

      case(INTERNAL_CMDG_UPDATE_RAW_NAND_SECTION_E):
      {
         inu_device__gpRawNandWrite((INTERNAL_CMDG_UpdateRawNandSecT *)msgP);
      }
      break;

      case(INUG_IOCTL_SYSTEM_CONFIG_PWM_E):
      {
        inu_device__pwmConfigT *pwmConfig = (inu_device__pwmConfigT*)msgP;
        TRIGGER_MNGRG_configPwm(pwmConfig->pwmNum,pwmConfig->fps,pwmConfig->widthInUsec,pwmConfig->pwmCmd);
      }
      break;

      case(INUG_IOCTL_SYSTEM_UPDATE_CALIBRATION_E):
      {
         inu_device__calibUpdateT *calibUpdate = (inu_device__calibUpdateT*)msgP;
         SEQ_MNGRG_updateCalibration(me, calibUpdate);
      }
      break;
      case (INUG_IOCTL_SYSTEM_SELECT_TEMP_SENSOR_E):

         inu_temperature__selectTempSensor(((inu_device__selectTempSensor_t *)msgP)->tempSensorNum);
      break;
      case (INUG_IOCTL_SYSTEM_SET_FLASH_SECTION_E):
         inu_device__gpSetFlashSectionHdr(me, (inu_device__setflashSectionHdr *)msgP);
      break;
      case(INUG_IOCTL_SENSOR_SYNC_E):
      {
         inu_device__sensorSync *sensorSyncCfg = (inu_device__sensorSync *) msgP;
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Received sensor sync configuration command \n");
         ret = SENSORSYNC_UPDATER_processIncomingSensorSyncCfgUpdate(sensorSyncCfg->buffer,sizeof(inu_device__sensorSync));
      }
      break;

      case(INUG_IOCTL_SYSTEM_SENSORUPDATE):
      {
         inu_device__sensorUpdateT *sensorUpdate = (inu_device__sensorUpdateT *) msgP;
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Received sensor update command \n");
         ret = METADATA_UPDATER_processIncomingSensorUpdateRequest(sensorUpdate->buffer,sizeof(inu_device__sensorUpdateT),true,true);
      }
      break;
#endif

      case(INTERNAL_CMDG_NEW_LOGGER_E):
      case(INTERNAL_CMDG_NEW_GRAPH_E):
      case(INTERNAL_CMDG_NEW_IMAGE_E):
      case(INTERNAL_CMDG_NEW_SOC_CH_E):
      case(INTERNAL_CMDG_NEW_STREAMER_E):
      case(INTERNAL_CMDG_NEW_SENSOR_E):
      case(INTERNAL_CMDG_NEW_SENSORS_GROUP_E):
      case(INTERNAL_CMDG_NEW_IMU_E):
      case(INTERNAL_CMDG_NEW_IMU_DATA_E):
      case(INTERNAL_CMDG_NEW_TEMPERATURE_E):
      case(INTERNAL_CMDG_NEW_TEMPERATURE_DATA_E):
      case(INTERNAL_CMDG_NEW_FDK_E):
      case(INTERNAL_CMDG_NEW_DATA_E):
      case(INTERNAL_CMDG_NEW_SOC_WRITER_E):
      case(INTERNAL_CMDG_NEW_HISTOGRAM_E):
      case(INTERNAL_CMDG_NEW_HISTOGRAM_DATA_E):
      case(INTERNAL_CMDG_NEW_SENSOR_CONTROL_E):
      case(INTERNAL_CMDG_NEW_FAST_ORB_E):
      case(INTERNAL_CMDG_NEW_FAST_ORB_DATA_E):
      case(INTERNAL_CMDG_NEW_CVA_CH_E):
      case(INTERNAL_CMDG_NEW_CVA_DATA_E):
      case(INTERNAL_CMDG_NEW_CDNN_E):
      case(INTERNAL_CMDG_NEW_CDNN_DATA_E):
      case(INTERNAL_CMDG_NEW_VISION_PROC_E):
      case(INTERNAL_CMDG_NEW_SLAM_E):
      case(INTERNAL_CMDG_NEW_SLAM_DATA_E):
      case(INTERNAL_CMDG_NEW_TSNR_CALC_E):
      case(INTERNAL_CMDG_NEW_TSNR_DATA_E):
      case(INTERNAL_CMDG_NEW_POINT_CLOUD_DATA_E):
      case(INTERNAL_CMDG_NEW_LOAD_NETWORK_E):
      case(INTERNAL_CMDG_NEW_LOAD_BACKGROUND_E):
      case(INTERNAL_CMDG_NEW_COMPRESS_E):
      case(INTERNAL_CMDG_NEW_DPE_PP_E):
      case(INTERNAL_CMDG_NEW_PP_E):
      case(INTERNAL_CMDG_NEW_MIPI_CH_E):
      case(INTERNAL_CMDG_NEW_METADATA_INJECTOR_E):
      case(INTERNAL_CMDG_NEW_ISP_CH_E):
      {
         ret = inu_device__createNew(me,msgP,msgCode);
         break;
      }

      case(INTERNAL_CMDG_DELETE_REF_E):
      {
         ret = inu_device__deleteRef(me, msgP);
      }
      case INTERNAL_CMDG_GET_DEVICE_REF_LIST_E:
      {
       inu_device__fillDeviceRefList((inu_device*)me,msgP);
       break;
      }


      default:
      break;
   }
   return ret;
}

static int inu_device__rxData(inu_ref *ref, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   inu_device           *me = (inu_device*)ref;
#if DEFSG_IS_HOST
   inu_device__privData *privP = (inu_device__privData*)me->privP;
#endif
   int ret = 0;

   ret = inu_ref__vtable_get()->p_rxAsyncData(ref, msgCode, msgP, dataP, dataLen, bufDescP);
   switch (msgCode)
   {
      case(INTERNAL_CMDG_PING_E):
      {
         INTERNAL_CMDG_pingT ping;
         OS_LYRG_getUsecTime(&ping.timestamp);
         inu_ref__copyAndSendDataAsync(ref, INTERNAL_CMDG_PING_RESPOND_E, &ping, NULL, 0);
         break;
      }

      case(INTERNAL_CMDG_READ_FLASH_HDR_E):
      {
         inu_device__readFlashChunk(me, (INTERNAL_CMDG_flashChunkHdrT*)msgP, dataP, dataLen);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
         break;
      }
      case(INTERNAL_CMDG_READ_BUF_HDR_E):
      {
         inu_device__handleReadBufChunkMsg(me, (INTERNAL_CMDG_readBufChunkHdrT*)msgP, dataP);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
         break;
      }
      case(INTERNAL_CMDG_EEPROM_READ_E):
      {
         inu_device__handleEepromAccess(me,(inu_device__eepromAccessT*)msgP, dataP);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
         break;
      }
      case(INTERNAL_CMDG_EEPROM_WRITE_E):
      {
#if DEFSG_IS_GP
         inu_device__gpEepromAccess(me,(inu_device__eepromAccessT*)msgP, dataP);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
#endif
         break;
      }
      case(INTERNAL_CMDG_SEND_FILE_TO_GP_BUF_E):
      {
#if DEFSG_IS_GP
         ret = inu_device__gpHandleFileWrite(me, (INTERNAL_CMDG_SendGpFileHdrT*)msgP, (MEM_POOLG_bufDescT*)*bufDescP);
#else
         privP->sendBufAck = TRUE;
#endif
         break;
      }

      case(INTERNAL_CMDG_WRITE_BUF_TRANSFER_E):
      {
 #if DEFSG_IS_GP
         inu_device__gpHandleWriteBufChunkMsg(me, msgCode, (INTERNAL_CMDG_readWriteBufHdrT*)msgP, dataP);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
#else
         privP->sendBufAck = TRUE;
#endif
         break;
        
      }
      case(INTERNAL_CMDG_BURN_DYN_CALIB_E):
      {
#if DEFSG_IS_GP
         inu_device__gpHandleDynCalibMsg(me, msgCode, (INTERNAL_CMDG_readWriteBufHdrT*)msgP, (UINT8 *)dataP);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
#else
         privP->sendBufAck = TRUE;
#endif
         break;
      }

      case(INTERNAL_CMDG_SEND_LUT_HDR_E):
      {
#if DEFSG_IS_GP
         lut_mngr_gpHandleLutChunkMsg((inu_deviceH)me, (void*)msgP, dataP);
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
#else
         privP->ioctlFlashAck = TRUE;
#endif
         break;
      }

      case(INTERNAL_CMDG_REQUEST_FILE_FROM_GP_E):
      {
#if DEFSG_IS_GP
         inu_device__gpRequestFile(me, (INTERNAL_CMDG_SendGpFileHdrT*)msgP, (MEM_POOLG_bufDescT*)*bufDescP);
#endif         
         break;            
      }

   }
   return ret;
}

void inu_device__vtable_init()
{
   if (!_bool_vtable_initialized)
   {
      inu_ref__vtableInitDefaults(&_vtable);
      _vtable.p_name = inu_device__name;
      _vtable.p_dtor = inu_device__dtor;
      _vtable.p_ctor = (inu_ref__Ctor*)inu_device__ctor;

      _vtable.p_rxSyncCtrl = inu_device__rxIoctl;
      _vtable.p_rxAsyncData = inu_device__rxData;

      _bool_vtable_initialized = true;
   }
}

const inu_ref__VTable *inu_device__vtable_get(void)
{
   inu_device__vtable_init();
   return &_vtable;
}


inu_ref *inu_device__getRefById(inu_device *me, inu_ref__id_t refId)
{
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   return privP->ref_list[refId];
}

void inu_device__rxNotify(void *me)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)me)->privP;
   LINK_MONITORG_rxEvent(privP->lmH);
}


static void inu_device__readFlashChunk(inu_device *me, INTERNAL_CMDG_flashChunkHdrT *flashChunkHdrP, UINT8 *dataP, UINT32 dataLen)
{
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   //UINT16   chunkId;
   UINT32   numOfChunks;
   UINT32   flashBufSize;
   UINT32   bufOffset;
   UINT8   *flashReadBufP;

   flashReadBufP = privP->tempReadBufP;

   //chunkId      = flashChunkHdrP->chunkId;
   numOfChunks  = flashChunkHdrP->numOfChunks;
   flashBufSize = flashChunkHdrP->flashBufSize;

   //LOGG_PRINT(LOG_DEBUG_E, NULL, "receive read flash chunk: Chunk number %d from %d chunks.\n", chunkId+1, numOfChunks);
   //printf("receive read flash chunk: Chunk number %d from %d chunks. flashReadBufP = %p, dataP = %p\n", chunkId+1, numOfChunks,flashReadBufP,dataP);

   bufOffset = (flashChunkHdrP->chunkId << SYSTEMP_FLASH_CHUNK_SIZE_FACTOR);

   if( bufOffset < flashBufSize )
   {
      flashReadBufP += bufOffset;
      memcpy(flashReadBufP, (UINT8*)(dataP), dataLen);
      bufOffset += dataLen;
   }

   privP->numOfReadFlashChunkToRecieve = numOfChunks;
   privP->numOfReadFlashChunkRecieved++;
}

int inu_device__linkEventCallbackWrapper(int sid, int serviceLinkEvent, void *argP)
{
   inu_device *me = (inu_device*)argP;
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   FIX_UNUSED_PARAM_WARN(sid);

   if (serviceLinkEvent == 0)
   {
      privP->connectState = INU_DEVICE__CONNECT;
      privP->ctorParams.deviceInitParams.eventCb(me, INU_DEVICE__CONNECT);
   }
   else
   {
      privP->connectState = INU_DEVICE__DISCONNECT;
      privP->ctorParams.deviceInitParams.eventCb(me, INU_DEVICE__DISCONNECT);
   }
   return 0;
}


static void inu_device__init(inu_device *me)
{
   if (me && !inu_ref__standAloneMode(me))
   {
      inu_device__privData *privP = (inu_device__privData*)me->privP;
      LINK_MONITORG_cfg cfg;

      cfg.linkEventCallBack = inu_device__linkEventCallbackWrapper;
      cfg.linkEventCBparam = me;
      cfg.linkMonitor = TRUE;
      cfg.ref = (inu_ref*)me;
      LINK_MONITORG_create(&privP->lmH, &cfg);
   }
   else if(inu_device__linkEventCallbackWrapper)
   {
      inu_device__linkEventCallbackWrapper(0, INUG_SERVICE_LINK_CONNECT_E, me);
   }
}

static ERRG_codeE inu_device__handleReadBufChunkMsg(inu_device *me, INTERNAL_CMDG_readBufChunkHdrT *bufChunkHdrP, UINT8 *chunkP)
{
   ERRG_codeE retCode = SYSTEM__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)me)->privP;

   if (privP->readBuffPtr == NULL)
   {
       privP->rwBuffSize = bufChunkHdrP->buffSize;
       privP->readBuffPtr = malloc(privP->rwBuffSize);
       LOGG_PRINT(LOG_DEBUG_E, NULL, "Allocated buffer of %d bytes. \n", privP->rwBuffSize);

   }
   LOGG_PRINT(LOG_DEBUG_E, NULL, "receive read buffer chunk: %d from %d chunks (%d bytes)\n", bufChunkHdrP->chunkId+1, bufChunkHdrP->numOfChunks, bufChunkHdrP->chunkLen);
   UINT8 *tempAddr = (UINT8*)privP->readBuffPtr + (bufChunkHdrP->chunkId * SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN);
   memcpy(tempAddr, chunkP, bufChunkHdrP->chunkLen);

   if(bufChunkHdrP->chunkId == ( bufChunkHdrP->numOfChunks - 1 ))
   {
      privP->recBufAck = TRUE;
   }

   return(retCode);
}

static ERRG_codeE inu_device__handleEepromAccess(inu_device *me, inu_device__eepromAccessT *eepromAccess, UINT8* dataP)
{
   ERRG_codeE retCode = SYSTEM__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)me)->privP;

   if(eepromAccess->operation == INU_DEVICE_EEPROM_READ_E)
   {
      memcpy(privP->tempReadBufP, dataP, eepromAccess->size);
   }
   return retCode;
}

static BOOLEAN inu_device__readLutFileAndSend(inu_deviceH deviceH, CALIB_sensorLutModeE sensorMode,char filebin[], inu_device_lut_hdr_t *lutHdr)
{
   ERRG_codeE status;
   inu_device_memory_t lutfile;

   if (inu_device__readBinFile(filebin, &lutfile) == TRUE)
   {
      status = inu_device__loadLuts(deviceH, sensorMode, &lutfile, lutHdr);
      if (ERRG_FAILED(status))
      {
         LOGG_PRINT(LOG_ERROR_E, status, "inu_device__loadLuts failed\n");
      }
      free(lutfile.bufP);
      return TRUE;
   }
   return FALSE;
}
#if (defined _WIN32) || (defined _WIN64)
#define PATH_SEPARATOR  "\\"
#elif defined(__linux__)
#define PATH_SEPARATOR  "/"
#endif

void inu_device__readLutFiles(inu_deviceH deviceH, char *calibPath, int dblBuffId)
{
   char filebin[500];
   unsigned int lutInd, sensorInd, dsrIbInd;
   unsigned int startLutOffset[2] = {INU_DEVICE__NUM_SENSORS*INU_DEVICE__NUM_IAE_SUB_LUT, 0};
   unsigned int startIdsrLutOffset = 2 * (INU_DEVICE__NUM_SENSORS * INU_DEVICE__NUM_IAE_SUB_LUT); //ib, dsr and then idsr
   unsigned int startLscLutOffset = startIdsrLutOffset + INU_DEVICE__NUM_SENSORS * 1; //ib, dsr, idsr and then lsc
                                                                                      //8*4,8*4,8*1,8*2

   CALIB_sensorLutModeE sensorMode;
   char *dsrIbStr[2]={"dsr","ib"};
   char *rightLeft[2]={"right","left"};
   BOOLEAN fileExists=TRUE;

   inu_device_lut_hdr_t lutHdr;

   for (sensorInd =0; sensorInd<INU_DEVICE__NUM_SENSORS; sensorInd++)
   {
      for (sensorMode = CALIB_LUT_MODE_BIN_E;sensorMode<CALIB_NUM_LUT_MODES_E;sensorMode = (CALIB_sensorLutModeE)(sensorMode + 1))
      {
         for (dsrIbInd =0; dsrIbInd<2; dsrIbInd++)
         {
            for (lutInd =0; lutInd<INU_DEVICE__NUM_IAE_SUB_LUT; lutInd++)
            {
               lutHdr.lutId     = startLutOffset[dsrIbInd] + sensorInd * INU_DEVICE__NUM_IAE_SUB_LUT + lutInd;
               lutHdr.sensorInd = sensorInd;
               lutHdr.dblBuffId = dblBuffId;

               sprintf(filebin,"%s%s%s%sNU4K%ssensor_%d_%d_%s.bin",calibPath, PATH_SEPARATOR,CALIB_getSensorModeStr(sensorMode), PATH_SEPARATOR, PATH_SEPARATOR, sensorInd, lutInd,dsrIbStr[dsrIbInd]);
               if ( inu_device__readLutFileAndSend(deviceH, sensorMode,filebin, &lutHdr) == FALSE)
               {
                 fileExists=FALSE;
               }
            }

            if ((fileExists == FALSE) && (sensorInd < 2)) // backword compatability
            {
               fileExists=TRUE;
               for (lutInd = 0; lutInd < INU_DEVICE__NUM_IAE_SUB_LUT; lutInd++)
               {
                  lutHdr.lutId     = startLutOffset[dsrIbInd] + sensorInd * INU_DEVICE__NUM_IAE_SUB_LUT + lutInd;
                  lutHdr.sensorInd = sensorInd;
                  lutHdr.dblBuffId = dblBuffId;

                  sprintf(filebin, "%s%s%s%sNU4K%siae_%s_%s_%d.bin", calibPath, PATH_SEPARATOR, CALIB_getSensorModeStr(sensorMode), PATH_SEPARATOR, PATH_SEPARATOR, dsrIbStr[dsrIbInd], rightLeft[sensorInd], lutInd);
                  inu_device__readLutFileAndSend(deviceH, sensorMode, filebin, &lutHdr);
               }
            }
         }
         //read idsr luts, we have only one lut
         lutHdr.lutId = startIdsrLutOffset + sensorInd ;
         sprintf(filebin, "%s%s%s%sNU4K%ssensor_%d_%s.bin", calibPath, PATH_SEPARATOR, CALIB_getSensorModeStr(sensorMode), PATH_SEPARATOR, PATH_SEPARATOR, sensorInd, "idsr");
         BOOLEAN idsrFound = inu_device__readLutFileAndSend(deviceH, sensorMode, filebin, &lutHdr);
         if (idsrFound)
         {
            inu_device* me = (inu_device*)deviceH;
            inu_device__privData *privP = (inu_device__privData*)me->privP;

            privP->calibMode = 2;
         }

         //read 'lens shading Correction' lut
         lutHdr.lutId = startLscLutOffset + sensorInd;
         sprintf(filebin, "%s%s%s%sNU4K%ssensor_%d_%s.bin", calibPath, PATH_SEPARATOR, CALIB_getSensorModeStr(sensorMode), PATH_SEPARATOR, PATH_SEPARATOR, sensorInd, "lsc");//TBD: tmp lsc, change if needed
         inu_device__readLutFileAndSend(deviceH, sensorMode, filebin, &lutHdr);
      }
   }
}

/****************************************************************************
*
*  Function Name: inu_device__reloadLutFiles
*
*  Description:   In case inu_device__reloadLutFiles() is called, need to send LUT files to target while working,
*                 so need to send the LUT files to a double buffer, and when all files are sent, need to switch the
*                 the index of the double buffer, so that the new values will be taken from the correct index.
*                 The base LUT files are sent to index 0 (via inu_graph__new()), so in case this function is invoked,
*                 for the first time, it'll use index=1, and from there on, it will toggle between index 0 and 1.
*
*  Inputs: inu_deviceH deviceH, char *calibPath
*
*  Outputs: None
*
*  Returns: None
*
*  Context: system service
*
****************************************************************************/
void inu_device__reloadLutFiles(inu_deviceH deviceH, char *calibPath)
{
   // dblBuffId is static, so it'll remember the last index that have been used, and toggle it to the next index
   static int dblBuffId = 1;
   inu_device__calibUpdateT calibUpdate;

   // First, send all LUT files content to the new double-buffer indext in lut_mngr
   inu_device__readLutFiles(deviceH, calibPath, dblBuffId);

   // Once all LUT files were sent to the new double-buffer indext in lut_mngr,
   // toggle the lut_mngr index to use the new index
   calibUpdate.dblBuffId = dblBuffId;
   inu_device__calibUpdate(deviceH, &calibUpdate);

   // Toggle the index for next time that this function will be called.
   // For now supports only 2 value - [0, 1]
   dblBuffId++;
   if (dblBuffId >= LUT_MNGR_NUM_OF_BUFF)
   {
      dblBuffId = 0;
   }
}
#if DEFSG_IS_HOST
/* TODO: put in inu_comm */
#ifndef DEFSG_GP_HOST
   #include "inu_usb.h"
#endif

char log_message[1024];
// Send inu_usb_lib logs to LOGG_PRINT (the default verbosity is info)
static void inu_log(InuUsbLogLevelE level, const char * fmt, ...)
{
   INU_DEFSG_logLevelE inuLogLevel = (INU_DEFSG_logLevelE)level;
   va_list  args;
   if (level <= INU_USB_LOG_INFO)
   {
      va_start(args, fmt);
      // variable args string
      vsprintf(log_message, fmt, args);
      LOGG_PRINT(inuLogLevel, NULL, log_message);
      va_end(args);
   }
}

ERRG_codeE inu_device__usb_monitor(int(*hotplugCallbackFunc)(INU_DEFSG_usbStatusE, inu_device__hwVersionIdU), int usbId, int externalHp, int filterId, int thrdSleepUs)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   InuUsbParamsT inuParams;
   memset(&inuParams, 0, sizeof(inuParams));

   inuParams.hp_callback = (InuHotplugCallback)hotplugCallbackFunc;
   inuParams.logLevel = INU_USB_LOG_INFO;
   inuParams.log = inu_log;
   inuParams.usbDeviceAddress = usbId;
   inuParams.usbUseExternalHpCallback = externalHp;
   inuParams.bootId = -1;
   inuParams.filterId = filterId;
   inuParams.thrSleepTimeUs = thrdSleepUs;
   inu_usb_boot_set_reset_in_progress(0);

   if (inu_usb_init(&inuParams) != INU_USB_ERR_SUCCESS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "monitor: can't initialize inu_usb_lib\n");
      ret = INU_DEVICE__ERR_UNEXPECTED;
   }

   return ret;
}


ERRG_codeE inu_device__usb_boot(int (*hotplugCallbackFunc)(INU_DEFSG_usbStatusE, inu_device__hwVersionIdU), const char *bootPath, int usbId, int externalHp, int bootid_override, int filterId, int thrdSleepUs)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   InuUsbParamsT inuParams;
   OS_LYRG_usleep(500000);      // changed from 2 seconds to reduce boot time
   								//0.5 seconds delay before reboot to ensure keep alive sync

   memset(&inuParams, 0, sizeof(inuParams));

   inuParams.hp_callback = (InuHotplugCallback)hotplugCallbackFunc;
   strcpy(inuParams.boot_path, bootPath);
   inuParams.logLevel = INU_USB_LOG_INFO;
   inuParams.log = inu_log;
   inuParams.usbFd = 0;
   inuParams.usbDeviceAddress = usbId;
   inuParams.usbUseExternalHpCallback = externalHp;
   inuParams.bootId = bootid_override;
   inuParams.filterId = filterId;
   inuParams.thrSleepTimeUs = thrdSleepUs;
   inu_usb_boot_set_reset_in_progress(0);

   if (inu_usb_init(&inuParams) != INU_USB_ERR_SUCCESS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "boot: can't initialize inu_usb_lib\n");
      ret = INU_DEVICE__ERR_UNEXPECTED;
   }

   return ret;
}

ERRG_codeE inu_device__boot_change_params(const char *bootDirPath)
{
#ifndef DEFSG_GP_HOST
   InuUsbParamsT inuParams;

   memset(&inuParams, 0, sizeof(inuParams));
   strcpy(inuParams.boot_path, bootDirPath);

   if(inu_usb_change_params(&inuParams) != INU_USB_ERR_SUCCESS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "boot: can't change inu_usb_lib params\n");
      return INU_DEVICE__ERR_INUUSB_INIT_FAIL;
   }
   return INU_DEVICE__ERR_UNEXPECTED;
#endif
}

ERRG_codeE inu_device__boot_hotplug_callback_external(INU_DEFSG_usbRawStatusE usbEvent, int usbFd)
{
#ifndef DEFSG_GP_HOST
   if (inu_usb_hotplug_callback_external((InuUsbRawStatusE)usbEvent,usbFd) != INU_USB_ERR_SUCCESS)
      return  INU_DEVICE__ERR_INUUSB_HP_CALLBACK_FAIL;
#endif
   return INU_DEVICE__ERR_UNEXPECTED;
}


ERRG_codeE inu_device__boot_deinit()
{
   if (inu_usb_deinit() != INU_USB_ERR_SUCCESS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "boot: failed to deinit inu_usb_lib\n");
      return INU_DEVICE__ERR_INUUSB_DEINIT_FAIL;
   }
   LOGG_PRINT(LOG_INFO_E, NULL, "boot: inu_usb_lib stoped successfully\n");
   return INU_DEVICE__RET_SUCCESS;
}

ERRG_codeE inu_device__usb_monitor_deinit()
{
   if (inu_usb_deinit() != INU_USB_ERR_SUCCESS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "monitor: failed to deinit inu_usb_lib\n");
      return INU_DEVICE__ERR_INUUSB_DEINIT_FAIL;
   }
   LOGG_PRINT(LOG_INFO_E, NULL, "monitor: inu_usb_lib stoped successfully\n");
   return INU_DEVICE__RET_SUCCESS;
}
#else
//stubs
ERRG_codeE inu_device__usb_boot(int (*hotplugCallbackFunc)(INU_DEFSG_usbStatusE, inu_device__hwVersionIdU), const char *bootPath, int usbId, int externalHp, int bootid_override, int filterId, int thrdSleepUs)
{
   FIX_UNUSED_PARAM_WARN(bootPath);
   FIX_UNUSED_PARAM_WARN(usbId);
   FIX_UNUSED_PARAM_WARN(externalHp);
   FIX_UNUSED_PARAM_WARN(bootid_override);
   FIX_UNUSED_PARAM_WARN(filterId);
   FIX_UNUSED_PARAM_WARN(thrdSleepUs);

   inu_device__hwVersionIdU hwVersion;
   hwVersion.val = 0;// todo - use gme register (need virt address)
   hotplugCallbackFunc(INU_DEFSG_USB_EVT_BOOT_COMPLETE, hwVersion);
   return INU_DEVICE__RET_SUCCESS;
}

ERRG_codeE inu_device__boot_deinit()
{
   return INU_DEVICE__RET_SUCCESS;
}
#endif

ERRG_codeE inu_device__getPowerDownStatus(inu_device__powerDownCmdE* powerDownStatus)
{
    *powerDownStatus = inu_device_powerDownStatus;

    return INU_DEVICE__RET_SUCCESS;
}

ERRG_codeE inu_device__updatePowerDownStatus(inu_device__powerDownCmdE powerDownStatus)
{
    LOGG_PRINT(LOG_INFO_E, NULL, "Updating pd state from %d to %d\n", inu_device_powerDownStatus, powerDownStatus);

    inu_device_powerDownStatus = powerDownStatus;

    return INU_DEVICE__RET_SUCCESS;
}

ERRG_codeE inu_device__new(inu_deviceH *deviceH, inu_device__initParams *deviceInitParamsP)
{
   inu_device__CtorParams ctorParams;

   ctorParams.standAloneMode = deviceInitParamsP->standAloneMode;
   memcpy(&ctorParams.deviceInitParams, deviceInitParamsP,sizeof(inu_device__initParams));
   ctorParams.ref_params.id = 0;

   inu_factory__initChanConfigTbl(&deviceInitParamsP->interface_0);
   return inu_factory__new((inu_ref**)deviceH, &ctorParams, INU_DEVICE_REF_TYPE);
}


void inu_device__delete(inu_deviceH meH)
{
   inu_device *device = (inu_device*)meH;
   inu_ref    *ref    = (inu_ref*)meH;
   //to delete a device, we first call deconstructor, to release all the refs
   ref->p_vtable->p_dtor(ref);
   //unbind it
   inu_comm__unregister((inu_comm*)inu_ref__getCtrlComm(ref), ref);
   inu_comm__unregister((inu_comm*)inu_ref__getDataComm(ref), ref);
#if 1
   //workaround: implementation in the usb lib is not per interface. The conn_lyr thread will not exit read until the usb lib closes the device.
   //so we cannot close one interface (conn_lyr) if we have another which is active.
   //to workaround this, we close all inu_comms when device is closed.
   inu_factory__releaseAllComm();
#else
   inu_factory__releaseComm((inu_comm*)inu_ref__getCtrlComm(ref));
   inu_factory__releaseComm((inu_comm*)inu_ref__getDataComm(ref));
#endif
   //free mem
   free(device);

   inu_ref__deallocStaticPool();
}


ERRG_codeE inu_device__addRef(inu_device *me, inu_ref *ref)
{
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   int i;

   OS_LYRG_lockMutex(&privP->refListMutex);
   for (i = 0; i < INU_DEVICE__MAX_NUM_OF_REFS; i++)
   {
      if (privP->ref_list[i] == NULL)
      {
         privP->ref_list[i] = ref;
         ref->id = i;
         OS_LYRG_unlockMutex(&privP->refListMutex);
         return INU_DEVICE__RET_SUCCESS;
      }
   }
   OS_LYRG_unlockMutex(&privP->refListMutex);

   return INU_DEVICE__ERR_OUT_OF_RSRCS;
}

ERRG_codeE inu_device__registerRef(inu_device *me, inu_ref *ref)
{
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   int i = ref->id;

   OS_LYRG_lockMutex(&privP->refListMutex);
   if (privP->ref_list[i] == NULL)
   {
      privP->ref_list[i] = ref;
      OS_LYRG_unlockMutex(&privP->refListMutex);
      return INU_DEVICE__RET_SUCCESS;
   }
   OS_LYRG_unlockMutex(&privP->refListMutex);

   return INU_DEVICE__ERR_OUT_OF_RSRCS;
}

void inu_device__removeRef(inu_device *me, inu_ref *ref)
{
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   int i;

   OS_LYRG_lockMutex(&privP->refListMutex);
   for (i = 0; i < INU_DEVICE__MAX_NUM_OF_REFS; i++)
   {
      if (privP->ref_list[i] == ref)
      {
         privP->ref_list[i] = NULL;
         break;
      }
   }
   OS_LYRG_unlockMutex(&privP->refListMutex);
}

const char *inu_device__getBootPathFromRef(inu_ref *ref)
{
   inu_device *me = (inu_device*)inu_ref__getDevice(ref);
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   return privP->ctorParams.deviceInitParams.bootPath;
}

int inu_device__connState(inu_device *me)
{
#ifdef __UART_ON_FPGA__
   return 1;
#else
 #if DEFSG_IS_HOST
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   return (privP->connectState == INU_DEVICE__DISCONNECT ? 0 : 1);
 #else
   (void)me;
   return 1;
 #endif
#endif
}


ERRG_codeE inu_device__getDeviceInfo(inu_deviceH meH, const inu_device__version **versionP)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   if (!privP->receivedDeviceInfo)
   {
      ret = inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_GET_VERSION_E, &privP->deviceInfo, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
      privP->receivedDeviceInfo = 1;
   }

   *versionP = (inu_device__version*)&privP->deviceInfo;
   return ret;
}

INT32 inu_device__getBootIdFromRef(inu_ref *ref)
{
   inu_device *me = (inu_device*)inu_ref__getDevice(ref);
   const inu_device__version *versionP;

   inu_device__getDeviceInfo(me, &versionP);

   return versionP->bootId;
}

ERRG_codeE inu_device__getCalibrationHeader(inu_deviceH meH, inu_device_calibration_header_t *calib_header)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_CALIBRATION_HDR_RD_E, &privP->calibrationHdr, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);

   calib_header->calib_data_size = privP->calibrationHdr.calibrationDataSize;
   calib_header->calib_format = privP->calibrationHdr.calibrationDataFormat;
   calib_header->calib_timestamp = privP->calibrationHdr.calibrationTimestamp;

   return INU_DEVICE__RET_SUCCESS;
}

ERRG_codeE inu_device__getCalibrationData(inu_deviceH meH, inu_device_calibration_data_t *calib_data)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   INUG_ioctlCalibrationRdT  calibrationRd;

   calibrationRd.calibrationRdBufP = (UINT8*)calib_data->data;
   calibrationRd.calibrationRdSize = calib_data->len;
   privP->tempReadBufP = (UINT8*)calib_data->data;

#if DEFSG_IS_GP
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();

   if (inu_ref__standAloneMode(meH) && (modelType >= INU_DEFSG_BOOT85_E) && (modelType <= INU_DEFSG_BOOT87_E))
   {
      ERRG_codeE        retCode = INU_DEVICE__RET_SUCCESS;
      UINT8             flashNum = SECONDARY_FLASH;
      InuSectionHeaderT SectionHeader;

      retCode = inu_device__gpReadFlashChunk(flashNum, SPI_FLASHG_getBlockSize(flashNum)*SPI_FLASHG_getNumBlocks(flashNum) - sizeof(InuSectionHeaderT), sizeof(InuSectionHeaderT), FALSE, (UINT8 *)&SectionHeader, meH);
      if (SectionHeader.magicNumber == SECTION_CALIBRATION_MAGIC_NUMBER && calib_data->len >= SectionHeader.sectionDataSize)
      {
         SPI_FLASH_DRVG_flashParametersT flashParameters;

         flashParameters.flashRead.startFlashAddress  = SPI_FLASHG_getBlockSize(flashNum)*(SPI_FLASHG_getNumBlocks(flashNum) - SectionHeader.sectionSize);
         flashParameters.flashRead.readFlashSize      = SectionHeader.sectionDataSize;
         flashParameters.flashRead.flashBufP          = calib_data->data;

         retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_2_E), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
         LOGG_PRINT(LOG_INFO_E, retCode, "Loading calibration from flash (0x%x bytes)\n", SectionHeader.sectionDataSize);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "wrong calibration size \n");
         retCode = SYSTEM__ERR_FLASH_IO_FLASH_HANDLE_IS_NULL;
      }
      return retCode;
   }
#endif

   //privP->ioctlFlashAck = FALSE;
   privP->numOfReadFlashChunkRecieved = 0;

   inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_CALIBRATION_RD_E, &calibrationRd, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);

   if ( privP->numOfReadFlashChunkRecieved == ( privP->numOfReadFlashChunkToRecieve ) )
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "read calibration from flash buf success.\n");
      //printf("read calibration from flash buf success.\n");
      LOGG_PRINT(LOG_INFO_E, NULL, "receive ack for read calibration buf.\n");
      //printf("receive ack for read calibration buf.\n");
      //privP->ioctlFlashAck = TRUE;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "read calibration buf from flash fail.\n");
      //printf("read calibration buf from flash fail.\n");
      LOGG_PRINT(LOG_ERROR_E, NULL, "Host not receive all read calibration chunk. . numOfReadFlashChunkRecieved %d, numOfReadFlashChunkToRecieve %d\n",
          privP->numOfReadFlashChunkRecieved, privP->numOfReadFlashChunkToRecieve);
      /*printf("Host not receive all read calibration chunk. numOfReadFlashChunkRecieved %d, numOfReadFlashChunkToRecieve %d\n",
              privP->numOfReadFlashChunkRecieved,privP->numOfReadFlashChunkToRecieve);*/
   }

   return INU_DEVICE__RET_SUCCESS;
}

void inu_device__EraseDynCalibration(inu_deviceH meH)
{
   INUG_ioctlEraseDynCalibrationT eraseCalib;
   
   
   inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_ERASE_DYN_CALIBRATION_E, &eraseCalib, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__accessEepromData(inu_deviceH meH, inu_device__eepromAccessT *eepromAccess)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   privP->tempReadBufP = eepromAccess->data;

   if(eepromAccess->operation == INU_DEVICE_EEPROM_READ_E)
   {
      ret = inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_EEPROM_READ_E, eepromAccess, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   }
   else if(eepromAccess->operation == INU_DEVICE_EEPROM_WRITE_E)
   {
      ret = inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_EEPROM_WRITE_E, eepromAccess,eepromAccess->data,eepromAccess->size);
   }
   return ret;
}

ERRG_codeE inu_device__saveCalibPath(inu_device *me, char* path[], BOOLEAN calibrationLoaded)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;

   inu_device__privData *privP = (inu_device__privData*)me->privP;
   privP->calibPath = strdup(path[0]);
   if (!privP->calibPath)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "failed to save calibpath %s\n", path[0]);
      assert(0);
   }

   if (calibrationLoaded)
   {
      privP->calibMode = 1;
   }
   return ret;
}

ERRG_codeE inu_device__getCalibPath(inu_device *me, const char **path)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;

   inu_device__privData *privP = (inu_device__privData*)me->privP;
   *path = privP->calibPath;
   return ret;
}

ERRG_codeE inu_device__getCalibMode(inu_device *me, UINT32* calibMode)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;

   inu_device__privData *privP = (inu_device__privData*)me->privP;
   *calibMode = privP->calibMode;
   return ret;
}

ERRG_codeE inu_device__readBuff(inu_deviceH meH, inu_device__buff_info_t *bufP)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;

   inu_device__readWriteBuf msgP;
   privP->readBuffPtr = bufP->buf;
   msgP.address = bufP->address;
   msgP.bufLen = bufP->len;

   ret = inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_READ_BUFFER_E, &msgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);

   if (ERRG_SUCCEEDED(ret))
   {
       //wait for ACK
       ret = inu_device__waitForAck(&privP->recBufAck, INU_DEVICE__ACK_TIMEOUT_MSEC * 15);
   }
   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Read buffer size %d from address 0x%x done\n", bufP->len, bufP->address);
   }

   return ret;
}

 
ERRG_codeE inu_device__writeCalib(inu_deviceH meH, inu_device__setflashSectionHdr *flashHdr, char *dynCalibFullName)
{
    inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
    ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
    UINT16                     chunkId;
    UINT32                     chunkLen = SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN;
    UINT32                     numOfChunks;
    UINT32                     lastchunkLen;
    UINT32                     lutByteSend = 0;
    inu_device__readWriteBuf   lutChunkHdr;
    UINT8                     *buf;
    UINT32                     bufSize;
   inu_device_memory_t DynCalibFile;
 

   ret = inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SET_FLASH_SECTION_E, (void *)flashHdr, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   if (ERRG_FAILED(ret))
      return ret;
   
   if (inu_device__readBinFile(dynCalibFullName, &DynCalibFile) != TRUE)
   {
      return ret;
   }


   
    bufSize =               DynCalibFile.bufsize;
    buf = (UINT8*)DynCalibFile.bufP;

    privP->ioctlFlashAck = FALSE;
    numOfChunks = (bufSize + chunkLen - 1) / chunkLen;
    lastchunkLen = bufSize % chunkLen;
    if (lastchunkLen == 0)
        lastchunkLen = SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN;

    for (chunkId = 0; chunkId < numOfChunks; )
    {
        if (chunkId == (numOfChunks - 1))
        {
            chunkLen = lastchunkLen; //last chunk can be shorter
        }

        lutChunkHdr.chunkId = chunkId;
        lutChunkHdr.chunkLen = chunkLen;
        lutChunkHdr.numOfChunks = numOfChunks;
        lutChunkHdr.bufLen = bufSize;
        lutChunkHdr.address = 0;//aallocated internally
        ret = inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_BURN_DYN_CALIB_E, &lutChunkHdr, &buf[lutByteSend], chunkLen);
        //printf("write buf chunk %d\n", chunkId);
        if (ERRG_SUCCEEDED(ret))
        {
            chunkId++;
            lutByteSend += chunkLen;
            OS_LYRG_usleep(10000);
        }
        else
        {
            if (ERRG_GET_MID(ret) == MEM_POOL)
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "failed alloc buf for chunkId = %d retry\n", chunkId);
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E, ret, "failed channel write calib - retry\n");
            }
            OS_LYRG_usleep(1000);
        }

    }
    LOGG_PRINT(LOG_INFO_E, NULL, "wait for Ack\n");
    //wait for ACK
   ret = inu_device__waitForAck(&privP->sendBufAck, INU_DEVICE__ACK_TIMEOUT_MSEC * 5000); // calibration mode burning is long time

   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Write dynamic calib size %d\n", bufSize);
   }
   free(DynCalibFile.bufP);
   return ret;
}



ERRG_codeE inu_device__writeBuff(inu_deviceH meH, inu_device__buff_info_t *bufP)
{
    inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
    ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
    UINT16                     chunkId;
    UINT32                     chunkLen = SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN;
    UINT32                     numOfChunks;
    UINT32                     lastchunkLen;
    UINT32                     lutByteSend = 0;
    inu_device__readWriteBuf   lutChunkHdr;
    UINT8                     *buf = (UINT8*)bufP->buf;
    UINT32                     bufSize = bufP->len;
    UINT32                     memAddress = bufP->address;

    privP->ioctlFlashAck = FALSE;
    numOfChunks = (bufSize + chunkLen - 1) / chunkLen;
    lastchunkLen = bufSize % chunkLen;
    if (lastchunkLen == 0)
        lastchunkLen = SYSTEMP_READ_BUFFER_CHUNK_DATA_LEN;

    for (chunkId = 0; chunkId < numOfChunks; )
    {
        if (chunkId == (numOfChunks - 1))
        {
            chunkLen = lastchunkLen; //last chunk can be shorter
        }

        lutChunkHdr.chunkId = chunkId;
        lutChunkHdr.chunkLen = chunkLen;
        lutChunkHdr.numOfChunks = numOfChunks;
        lutChunkHdr.bufLen = bufSize;
        lutChunkHdr.address = memAddress;
        ret = inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_WRITE_BUF_TRANSFER_E, &lutChunkHdr, &buf[lutByteSend], chunkLen);
        //printf("write buf chunk %d\n", chunkId);
        if (ERRG_SUCCEEDED(ret))
        {
            chunkId++;
            lutByteSend += chunkLen;
            OS_LYRG_usleep(10000);
        }
        else
        {
            if (ERRG_GET_MID(ret) == MEM_POOL)
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "failed alloc buf for chunkId = %d retry\n", chunkId);
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E, ret, "failed channel send buf data - retry\n");
            }
            OS_LYRG_usleep(1000);
        }

    }

    //wait for ACK
   ret = inu_device__waitForAck(&privP->sendBufAck, INU_DEVICE__ACK_TIMEOUT_MSEC * 5);

   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Write buffer size %d to address 0x%x done\n", bufSize, memAddress);
   }

    return ret;
}

ERRG_codeE inu_device__loadLuts(inu_deviceH meH, UINT32 sensorLutMode,inu_device_memory_t *memP, inu_device_lut_hdr_t *lutHdrP)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
#if DEFSG_IS_HOST
#endif
   UINT16                     chunkId;
   UINT32                     chunkLen = SYSTEMP_LUT_CHUNK_DATA_LEN;
   UINT32                     numOfChunks;
   UINT32                     lastchunkLen;
   UINT32                     lutByteSend = 0;
   //INU_DEFSG_lutHeaderT        *lutHeader = (INU_DEFSG_lutHeaderT *)(bufP);
   INTERNAL_CMDG_lutChunkHdrT lutChunkHdr;
   UINT8                     *bufP = (UINT8*)memP->bufP;
   UINT32                     bufSize = memP->bufsize;

   lutChunkHdr.dblBuffId = (UINT16)lutHdrP->dblBuffId;

   privP->ioctlFlashAck = FALSE;
   numOfChunks = (bufSize + chunkLen - 1) / chunkLen;
   lastchunkLen = bufSize % chunkLen;
   if (lastchunkLen == 0)
      lastchunkLen = SYSTEMP_LUT_CHUNK_DATA_LEN;

   for (chunkId = 0; chunkId < numOfChunks; )
   {
      if (chunkId == (numOfChunks - 1))
      {
         chunkLen = lastchunkLen; //last lut chunk can be shorter
      }

      lutChunkHdr.calibMode = (CALIB_sensorLutModeE)sensorLutMode;
      lutChunkHdr.lutId = lutHdrP->lutId;
      lutChunkHdr.sensorInd = (UINT16)lutHdrP->sensorInd;
      lutChunkHdr.mode = 0;// getLutModeAB(lutHeader->lutId);
      lutChunkHdr.chunkId = chunkId;
      lutChunkHdr.chunkLen = chunkLen;
      lutChunkHdr.numOfChunks = numOfChunks;
      lutChunkHdr.lutSize = bufSize;
#if DEFSG_IS_GP
      ret = lut_mngr_gpHandleLutChunkMsg(meH, &lutChunkHdr, &bufP[lutByteSend]);
#else
      ret = inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_SEND_LUT_HDR_E, &lutChunkHdr, &bufP[lutByteSend], chunkLen);
#endif
      if (ERRG_SUCCEEDED(ret))
      {
         chunkId++;
         lutByteSend += chunkLen;
#if 0
         /* On most of the system this delay takes ~12-15ms although only 1us was required.
            However, it seems like it is here only for historical reasons and it is actually redundant.
            QA tests for all the modules have passed after removing this delay */
         OS_LYRG_usleep(1);
#endif
      }
      else
      {
         if (ERRG_GET_MID(ret) == MEM_POOL)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed alloc lut buf for chunkId = %d retry\n",  chunkId);
         }
         else if (ret == INU_REF__ERR_CONN_DOWN)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "connection down\n");
            return ret;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "failed channel send lut data -retry\n");
         }
         OS_LYRG_usleep(1000);
      }
   }

#if DEFSG_IS_HOST
   //wait for ACK
   ret = inu_device__waitForAck(&privP->ioctlFlashAck, INU_DEVICE__ACK_TIMEOUT_MSEC);
#endif
   return ret;
}


ERRG_codeE inu_device__setTime(inu_deviceH meH, inu_device__set_time_t *time_data, inu_device__getUsecTimeFuncT* getUsecTimeFunc)
{
    ERRG_codeE status;

    status = inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SET_TIME_E, time_data, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
    if (ERRG_SUCCEEDED(status) && (time_data->method == INU_DEVICE__SET_TIME_PLL_LOCK_UART_E))
    {
        status = TIMEG_start(time_data, getUsecTimeFunc);
    }
    else if (ERRG_SUCCEEDED(status) && (time_data->method == INU_DEVICE__SET_TIME_PLL_LOCK_DISABLE_E))
    {
        status = TIMEG_stop();
    }

    return status;
}


ERRG_codeE inu_device__getTime(inu_deviceH meH, inu_device__get_time_t *time_data)
{
    return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_GET_TIME_E, time_data, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


ERRG_codeE inu_device__getRtcLock(inu_deviceH meH, inu_device__get_rtc_lock_t *rtc_Lock)
{
    return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_GET_RTC_LOCK_E, rtc_Lock, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


ERRG_codeE inu_device__setWatchdog(inu_deviceH meH, inu_device__watchdog_timeout_t *watchdog_timeoutP)
{
   LOGG_PRINT(LOG_INFO_E, NULL, "setWatcdog %d\n", watchdog_timeoutP->sec);
   UINT32 timeout_ms = INU_REF__SYNC_DFLT_TIMEOUT_MSEC;
#if DEFSG_IS_HOST
   if(watchdog_timeoutP->sec == 0xFFFFFFFF) //reboot/reset GP
   {
      inu_usb_boot_set_reset_in_progress(1);
      timeout_ms = 1;
   }
#endif
   return inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_CONFIG_WD_E, watchdog_timeoutP, timeout_ms);
}

ERRG_codeE inu_device__writeRegister(inu_deviceH meH, inu_device__writeRegT *writeRegisterParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_WR_REG_E, writeRegisterParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__readRegister(inu_deviceH meH, inu_device__readRegT *readRegisterParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_RD_REG_E, readRegisterParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__setProjectorMode(inu_deviceH meH, inu_device__projectorSetModeT *projSetModeP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E, projSetModeP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__getProjectorMode(inu_deviceH meH, inu_device__projectorGetModeT *projGetModeP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_PROJECTOR_GET_MODE_E, projGetModeP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__powerDown(inu_deviceH meH, inu_device__powerDownT* powerDownParams)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_POWER_DOWN_E, powerDownParams, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__enableTuningServer(inu_deviceH meH, inu_device__tuningParamsT* tsParams)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_TUNING_SERVER_E, tsParams, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__selectTempSensor(inu_deviceH meH,inu_device__selectTempSensor_t *selectTempSensor)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SELECT_TEMP_SENSOR_E, selectTempSensor, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

BOOLEAN inu_device__isPowerDownActive(inu_deviceH meH)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;

   return (BOOLEAN)privP->pdIsActive;
}

ERRG_codeE inu_device__setPowerMode(inu_deviceH meH, inu_device__powerModeT *powerModeP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_POWER_MODE_E, powerModeP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__pwmConfig(inu_deviceH meH, inu_device__pwmConfigT *pwmConfig)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_CONFIG_PWM_E, pwmConfig, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE inu_device__calibUpdate(inu_deviceH meH, inu_device__calibUpdateT *calibUpdate)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_UPDATE_CALIBRATION_E, calibUpdate, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}
ERRG_codeE inu_device__sensorUpdate(inu_deviceH meH, INU_Metadata_T *metadataUpdate)
{
   /* Serialize metadata update into sensorUpdate */
   inu_device__sensorUpdateT sensorUpdate;
   ERRG_codeE ret = inu_metadata__serialize(sensorUpdate.buffer,sizeof(sensorUpdate.buffer),metadataUpdate);
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Serialization complete, now marshalling metadata message across \n");
      return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSORUPDATE, &sensorUpdate, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   }
   else
   {
      return ret;
   }
}

void inu_device__showStats(inu_deviceH meH)
{
   inu_device *me = (inu_device*)meH;
   inu_device__privData *privP = (inu_device__privData*)me->privP;
   UINT16 i;

   for(i = 0; i < INU_DEVICE__MAX_NUM_OF_REFS; i++)
   {
      if (privP->ref_list[i] && privP->ref_list[i]->statsEnable)
      {
         privP->ref_list[i]->p_vtable->p_showStats(privP->ref_list[i]);
         LOGG_PRINT(LOG_INFO_E, NULL, "\n\n");
      }
   }

   for(i = 0; i < INU_FACTORY__NUM_CONNECTIONS_E; i++)
   {
      CON_LYRG_handleT handle = (CON_LYRG_handleT*)inu_factory__getConnHandle((inu_factory__connIdE)i);
      if (handle)
         CONN_LYRG_showStats(handle,CONN_LYRG_ALL_STATS_E);
   }
#if DEFSG_IS_HOST
   inu_ref__sendCtrlSync((inu_ref*)me,INUG_IOCTL_SYSTEM_GET_STATS_E,NULL, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
#endif
}

void inu_device__flushIoctls(inu_deviceH meH)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   UINT32  i;
   for (i = 0; i < INU_DEVICE__MAX_NUM_OF_REFS; i++)
   {
      if (privP->ref_list[i])
      {
         inu_ref__ctrlSyncDone(privP->ref_list[i], INU_REF__ERR_IO_ERROR, NULL);
      }
   }
}
ERRG_codeE inu_device__sensorSyncCfg(inu_deviceH meH, inu_sensorSync__updateT *sensorSyncCfg)
{
   /* Serialize metadata update into sensorSycCfg */
   inu_device__sensorSync sensorSycCfg;
   ERRG_codeE ret = inu_sensorsync__serialize(sensorSycCfg.buffer,sizeof(sensorSycCfg.buffer),sensorSyncCfg);
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Serialization complete, now marshalling sensor sync message across \n");
      return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SENSOR_SYNC_E, &sensorSycCfg, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   }
   else
   {
      return ret;
   }
}

/****************************************************************************
*
*  Function Name: inu_device__writeFileToGp
*
*  Description:
*
*  Inputs:  file
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__writeFileToGp(inu_deviceH meH, char* fileName, char *dstFilename)
{
   ERRG_codeE ret = SYSTEM__RET_SUCCESS;

   inu_device *me = (inu_device*)meH;
   inu_device__privData *privP = (inu_device__privData*)me->privP;

   UINT8* remoteHostBuf;
   UINT32 buffSize;
   FILE *file = NULL;
   MEM_POOLG_cfgT cfg;
   MEM_POOLG_handleT handleP = NULL;
   MEM_POOLG_bufDescT *remoteHostBufDescP = NULL;
   INTERNAL_CMDG_SendGpFileHdrT sendHdr;

   file = fopen(fileName, "rb");  // r for read, b for binary
   if (!file)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "File %s not found! exit\n",fileName);
      return SYSTEM__ERR_INVALID_ARGS;
   }
   fseek(file, 0, SEEK_END);
   buffSize = ftell(file);
   fseek(file, 0, SEEK_SET);

   if (!inu_ref__standAloneMode(meH))
   {
      //Allocate user defined buff descriptor for host
      cfg.bufferSize = buffSize;
      cfg.type = MEM_POOLG_TYPE_ALLOC_USER_E;
      cfg.numBuffers = 1;
      cfg.freeCb = NULL;
      //buggy - this pool is never released... TODO
      ret = MEM_POOLG_initPool(&handleP, &cfg);
      if (ERRG_FAILED(ret))
         return SYSTEM__ERR_OUT_OF_RSRCS;
      ret = MEM_POOLG_alloc(handleP, buffSize, &remoteHostBufDescP);
      if (ERRG_FAILED(ret))
         return SYSTEM__ERR_OUT_OF_RSRCS;

      // Fill buff descriptor
      remoteHostBuf = (UINT8*)malloc(buffSize); //buggy -this memory is not released.. TODO
      remoteHostBufDescP->dataP  = remoteHostBuf;
      remoteHostBufDescP->startP = remoteHostBuf;
      remoteHostBufDescP->size   = buffSize;
   }
   else
   {
     //should this API be called in standalone?
     assert(0);
   }

   // Read file into the buffer
   fread(remoteHostBufDescP->dataP, remoteHostBufDescP->size, 1, file);
   remoteHostBufDescP->dataLen = buffSize;
   fclose(file);

   if (!inu_ref__standAloneMode(meH))
   {
      // Send file to target
      strcpy(sendHdr.filename,dstFilename);
      ret = inu_ref__sendDataAsync((inu_ref*)meH, INTERNAL_CMDG_SEND_FILE_TO_GP_BUF_E, &sendHdr, remoteHostBufDescP);
      if (ERRG_FAILED(ret))
         return SYSTEM__ERR_IO_ERROR;

      //wait for ACK
      ret = inu_device__waitForAck(&privP->sendBufAck, INU_DEVICE__ACK_TIMEOUT_MSEC);
      LOGG_PRINT(LOG_INFO_E, NULL, "Done writing file %s, Size %d\n",fileName, buffSize);
   }

   //remove the pool
   MEM_POOLG_closePool(handleP);

   return ret;
}

/****************************************************************************
*
*  Function Name: inu_device__readFileFromGp
*
*  Description: Read target file and save it on Host
*
*  Inputs:  targetFileName, hostFileName
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/

ERRG_codeE inu_device__readFileFromGp(inu_deviceH meH, char* targetFileName, char* hostFileName)
{
   ERRG_codeE ret = SYSTEM__RET_SUCCESS;
   inu_device* me = (inu_device*)meH;
   inu_device__privData* privP = (inu_device__privData*)me->privP;
   INTERNAL_CMDG_SendGpFileHdrT sendHdr;
   UINT32 sizeSaved = 0;
   FILE* newFile = fopen(hostFileName, "wb");
   if (!newFile)
   {
       LOGG_PRINT(LOG_DEBUG_E, NULL, "Could not create a file %s\n", hostFileName);
       return INU_DEVICE__ERR_UNEXPECTED;
   }


   if (!inu_ref__standAloneMode(meH))
   {
       strcpy(sendHdr.filename, targetFileName);
       privP->readBuffPtr = NULL;
       printf("file is taken from: %s\n ", sendHdr.filename);

        ret = inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_REQUEST_FILE_FROM_GP_E, &sendHdr, NULL, 0);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Error while sending request to the target\n");
            return SYSTEM__ERR_IO_ERROR;
        }

        //wait for ACK
        ret = inu_device__waitForAck(&privP->recBufAck, INU_DEVICE__ACK_TIMEOUT_MSEC);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Acknowledge timeout\n");
            return SYSTEM__ERR_IO_ERROR;
        }

        if (privP->readBuffPtr != NULL)
        {
            sizeSaved = (UINT32)fwrite(privP->readBuffPtr, 1, privP->rwBuffSize, newFile);
            if (sizeSaved != privP->rwBuffSize)
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "Could not save content file %s. size %d, sizeSaved %d\n", hostFileName, privP->rwBuffSize, sizeSaved);
                return INU_DEVICE__ERR_UNEXPECTED;
            }
            fflush(newFile);
            fclose(newFile);
            free(privP->readBuffPtr);
            LOGG_PRINT(LOG_INFO_E, NULL, "File %s, size %d is saved\n", hostFileName, privP->rwBuffSize);
        }
        else
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Error reading file %s.\n", targetFileName);
            return INU_DEVICE__ERR_UNEXPECTED;
        }
   }
   else
   {
       LOGG_PRINT(LOG_ERROR_E, NULL, "Not implemented\n");
       return INU_DEVICE__ERR_UNEXPECTED;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: inu_device__fwUpdate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_device__fwUpdate(inu_deviceH meH, inu_device__fwUpdateHdrT *updateParams)
{
   ERRG_codeE ret = SYSTEM__RET_SUCCESS;
   UINT32 i, updatedSections = 0;;
   INTERNAL_CMDG_UpdateRawNandSecT sectParam;
   char tempFileName[MAX_FILE_NAME];

   for(i = 0; i < updateParams->numProductsToUpdate; i++)
   {
      memset(tempFileName,0,sizeof(tempFileName));
      sprintf(tempFileName, "filetoflash_%d",i);
      ret = inu_device__writeFileToGp(meH, updateParams->element[i].fileName, tempFileName);
      if (ERRG_FAILED(ret))
         return SYSTEM__ERR_IO_ERROR;

      updatedSections |= (1 << updateParams->element[i].type);

      // Send NAND write command for curren image
      strcpy(sectParam.filename,tempFileName);
      sectParam.type     = (InuStorageNandSectionTypE)updateParams->element[i].type;    // Here need to be case for raw and fs products
      sectParam.verMajor = updateParams->element[i].version.major;
      sectParam.verMinor = updateParams->element[i].version.minor;
      sectParam.verBuild = updateParams->element[i].version.build;
      sectParam.updatedSectionsFlags = (i == (updateParams->numProductsToUpdate - 1)) ? (updatedSections) : (0);

      printf("inu_device__fwUpdate: file %s, Flags %x, i %x, num %x, updtSect %x\n",sectParam.filename, sectParam.updatedSectionsFlags, i, updateParams->numProductsToUpdate, updatedSections);

      ret = inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_UPDATE_RAW_NAND_SECTION_E, &sectParam, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
      if (ERRG_FAILED(ret))
         return ret;
   }
   return ret;
}

#if DEFSG_IS_GP
ERRG_codeE inu_device__getFlashComponentsVersion(inu_device__componenetsVersionT* componentsVersion)
{

   ERRG_codeE ret = SYSTEM__RET_SUCCESS;
   UINT8 *bootfixMetaSectionP;
   InuBootfixHeaderT *bootfixHdrP;
   UINT32 bootfixMetaSize;
   InuStorageNandHeaderT header;
   if(NAND_STRGG_isDevPresent() == FALSE)
   {
     LOGG_PRINT(LOG_WARN_E, NULL, "NAND storage is not present\n");
     return SYSTEM__RET_SUCCESS;
   }

   bootfixMetaSize = (sizeof(InuSectionHeaderT) + sizeof(InuBootfixHeaderT));  // meta section contains only two stuctures
   bootfixMetaSectionP = malloc(bootfixMetaSize);
   ret = inu_device__gpReadBootfixMetaSection(bootfixMetaSectionP, bootfixMetaSize);
   bootfixHdrP = (InuBootfixHeaderT*)(bootfixMetaSectionP + sizeof(InuSectionHeaderT));

   componentsVersion->bootFromFactoryDefault = bootfixHdrP->bootFromFactoryDefault;
   componentsVersion->isBootfromFlash = bootfixHdrP->isBootfromFlash;

   ret = NAND_STRGG_readHeader(&header);
   if (ERRG_SUCCEEDED(ret))
   {
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_BOOTSPL_E].build     = header.section[NAND_SECTION_TYPE_BOOTSPL].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_BOOTSPL_E].major     = header.section[NAND_SECTION_TYPE_BOOTSPL].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_BOOTSPL_E].minor     = header.section[NAND_SECTION_TYPE_BOOTSPL].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_LXKERNEL_E].build    = header.section[NAND_SECTION_TYPE_KERNEL].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_LXKERNEL_E].major    = header.section[NAND_SECTION_TYPE_KERNEL].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_LXKERNEL_E].minor    = header.section[NAND_SECTION_TYPE_KERNEL].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_LXDTB_E].build       = header.section[NAND_SECTION_TYPE_DTB].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_LXDTB_E].major       = header.section[NAND_SECTION_TYPE_DTB].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_LXDTB_E].minor       = header.section[NAND_SECTION_TYPE_DTB].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_CEVADSP_E].build     = header.section[NAND_SECTION_TYPE_CEVA].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_CEVADSP_E].major     = header.section[NAND_SECTION_TYPE_CEVA].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_CEVADSP_E].minor     = header.section[NAND_SECTION_TYPE_CEVA].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_EV62BOOT_E].build    = header.section[NAND_SECTION_TYPE_EV62_BOOT].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_EV62BOOT_E].major    = header.section[NAND_SECTION_TYPE_EV62_BOOT].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_EV62BOOT_E].minor    = header.section[NAND_SECTION_TYPE_EV62_BOOT].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_EV62APP_E].build     = header.section[NAND_SECTION_TYPE_EV62_APP].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_EV62APP_E].major     = header.section[NAND_SECTION_TYPE_EV62_APP].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_EV62APP_E].minor     = header.section[NAND_SECTION_TYPE_EV62_APP].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_INITRAMFS_E].build   = header.section[NAND_SECTION_TYPE_INITRAMFS].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_INITRAMFS_E].major   = header.section[NAND_SECTION_TYPE_INITRAMFS].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_INITRAMFS_E].minor   = header.section[NAND_SECTION_TYPE_INITRAMFS].imgVersion.minor;

      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_APPLICATION_E].build = header.section[NAND_SECTION_TYPE_APP].imgVersion.build;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_APPLICATION_E].major = header.section[NAND_SECTION_TYPE_APP].imgVersion.major;
      componentsVersion->version[INU_DEVICE_PRODUCT_TYPE_APPLICATION_E].minor = header.section[NAND_SECTION_TYPE_APP].imgVersion.minor;
   }

   return(ret);
}
#endif


BOOLEAN inu_dev__standAloneMode(inu_deviceH meH)
{
   inu_device *me = (inu_device*)meH;
   inu_device__privData *privP = (inu_device__privData*)me->privP;

   if (privP->standAloneMode == INU_DEVICE__STANDALONE_MODE)
      return TRUE;
   return FALSE;

}

UINT32 inu_device__useShareMem(inu_deviceH meH)
{
   inu_device *me = (inu_device*)meH;
   inu_device__privData *privP = (inu_device__privData*)me->privP;

   return privP->ctorParams.deviceInitParams.useSharedMemory;
}


ERRG_codeE inu_device__alternateProjector(inu_deviceH meH, inu_device__alternateProjCfgT *cfgP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_ALTERNATE_SET_CONFIG_E, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


ERRG_codeE inu_device__findNode(inu_deviceH meH, const char *userName, inu_refH *inuRefP)
{
   inu_device__privData *privP = (inu_device__privData*)((inu_device*)meH)->privP;
   int i;

   for (i = 0; i < INU_DEVICE__MAX_NUM_OF_REFS; i++)
   {
      if (privP->ref_list[i])
      {
          if (strcmp(userName, inu_ref__getUserName(privP->ref_list[i])) == 0)
          {
             *inuRefP=privP->ref_list[i];
             return INU_DEVICE__RET_SUCCESS;
          }
      }
   }
   return INU_DEVICE__ERR_UNEXPECTED;
}

#if DEFSG_IS_GP
ERRG_codeE inu_device__getIdsrLutAddress(void **idsrLutVirtAddress, void **idsrLutPhysAddress)
{
   return lut_mngr_getIdsrLutAddress(idsrLutVirtAddress, idsrLutPhysAddress);
}

/****************************************************************************
*
*  Function Name:  inu_device__gpFromDspSyncMsgAckRcv
*
*  Description: callback for sending sync messages to dsp. Releases the calling thread
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     ICC thread
*
****************************************************************************/
static void inu_device__gpFromDspSyncMsgAckRcv(UINT16 jobDescriptorIndex, ERRG_codeE result, UINT32 dspTarget)
{
   DATA_BASEG_iccJobsDescriptorDataBaseT  *jobsDbP;
   inu_device                             *me;
   inu_device__privData                   *privP;

   //job descriptor is in our hands, its safe to access with no mutex
   DATA_BASEG_accessDataBaseNoMutex((UINT8**)&jobsDbP, DATA_BASEG_JOB_DESCRIPTORS);

   //extract the function from which the dsp cmd was started
   me = (inu_device*)jobsDbP->jobDescriptorPool[jobDescriptorIndex].arg;
   privP = (inu_device__privData*)me->privP;

   pmm_mngr__dspPmmDeactivate(dspTarget);

   if(ERRG_FAILED(result))
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, result, "receive ack for dsp message with error\n");
   }
   else
   {
      INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL, "dsp done: jobDescriptorIndex=%d, result=0x%X\n", jobDescriptorIndex, result);
   }

   result = ICCG_returnJobDescriptor(jobDescriptorIndex);
   if(ERRG_FAILED(result))
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, result, "failed to return jobDescriptor to pool\n");
   }

   //save result
   privP->dspSyncMsgResult = result;
   //set event to wakeup the calling thread
   OS_LYRG_setEvent(privP->dspSyncMsgEvent);
}

/****************************************************************************
*
*  Function Name:  inu_device__sendDspSyncMsg
*
*  Description: sends a message to the DSP, and will block until DSP completes handling this message.
*                   used for control (start/stop..)
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE inu_device__sendDspSyncMsg(inu_device *me, UINT32 alg, UINT32 data, UINT32 dspTarget)
{
   ERRG_codeE                            retCode = INU_DEVICE__RET_SUCCESS;
   inu_device__privData                  *privP = (inu_device__privData*)me->privP;
   UINT16                                jobDescriptorIndex;
   ICCG_cmdT                             cmd;
   DATA_BASEG_iccJobsDescriptorDataBaseT *jobsDbP;

   retCode = ICCG_getJobDescriptor(&jobDescriptorIndex);

   if(ERRG_SUCCEEDED(retCode))
   {
      //safe to access without mutex, we got the descriptor
      DATA_BASEG_accessDataBaseNoMutex((UINT8**)(&jobsDbP), DATA_BASEG_JOB_DESCRIPTORS);
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].alg             = alg;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].cb              = inu_device__gpFromDspSyncMsgAckRcv;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].data            = data;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].arg             = (UINT32)me;

      cmd.cmdType  = ICCG_CMD_REQ;
      cmd.descriptorHandle = jobDescriptorIndex;
      cmd.dspTarget = dspTarget;

      retCode = pmm_mngr__dspPmmActivate(dspTarget);
      if(ERRG_FAILED(retCode))
      {
         INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error activate dsp 0x%x\n", retCode);
         return retCode;
      }

      retCode = ICCG_sendIccCmd(&cmd);
      if(ERRG_FAILED(retCode))
      {
         INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error sending sync CMD to DSP\n");
      }
      else
      {
         retCode = OS_LYRG_waitEvent(privP->dspSyncMsgEvent, INU_DEVICE__DSP_SYNC_TIMEOUT_MSEC);
         if(ERRG_FAILED(retCode))
         {
            INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error on sync CMD to DSP, no response after %d [msec], exit\n",INU_DEVICE__DSP_SYNC_TIMEOUT_MSEC);
         }
         else
         {
            retCode = privP->dspSyncMsgResult;
         }
      }
   }
   else
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error sending CMD to DSP (stack is empty)\n");
   }

   if(ERRG_FAILED(retCode))
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error occured while sending CMD to DSP [0x%x]]\n", retCode);
   }

   return retCode;
}
#endif //DEFSG_IS_GP
