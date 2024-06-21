/****************************************************************************
 *
 *   FileName: data_base.c
 *
 *   Author: Eyal Amiel
 *
 *   Date: 03/11/2013
 *
 *   Description: data base API
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "data_base.h"
#include "os_lyr.h"
#include "log.h"
#include "utils.h"
#if DEFSG_IS_CEVA
//   #include "MM3K_defines.h"
#include "cevaxm.h"
#endif

#if DEFSG_IS_GP
#include "assert.h"
#else
#include "ceva_boot_params.h"
#endif


/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

//DATA BASE offset
#define DATA_BASEP_GENERAL_DATABASE_OFFSET            (0)

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

typedef enum 
{
   DATA_BASEP_MODULE_STATUS_CLOSE_E = 0,
   DATA_BASEP_MODULE_STATUS_OPEN_E  = 1

} DATA_BASEP_moduleStatusE;

typedef enum 
{
   DATA_BASEP_ICC_BOX_GP_DSPA_OFFSET   = DATA_BASEP_GENERAL_DATABASE_OFFSET ,
   DATA_BASEP_ICC_BOX_GP_DSPB_OFFSET   = (DATA_BASEP_ICC_BOX_GP_DSPA_OFFSET   + sizeof(DATA_BASEG_iccBoxDataBaseT)),
   DATA_BASEP_ICC_BOX_DSPA_GP_OFFSET   = (DATA_BASEP_ICC_BOX_GP_DSPB_OFFSET   + sizeof(DATA_BASEG_iccBoxDataBaseT)),
   DATA_BASEP_ICC_BOX_DSPB_GP_OFFSET   = (DATA_BASEP_ICC_BOX_DSPA_GP_OFFSET   + sizeof(DATA_BASEG_iccBoxDataBaseT)),
   DATA_BASEP_JOB_DESCRIPTORS_OFFSET   = (DATA_BASEP_ICC_BOX_DSPB_GP_OFFSET   + sizeof(DATA_BASEG_iccBoxDataBaseT)),
   DATA_BASEP_FAST_OFFSET              = (DATA_BASEP_JOB_DESCRIPTORS_OFFSET   + sizeof(DATA_BASEG_iccJobsDescriptorDataBaseT)),
   DATA_BASEP_CDNN_OFFSET              = (DATA_BASEP_FAST_OFFSET              + sizeof(DATA_BASEG_fastDataDataBaseT)),
   DATA_BASEP_CDNN_EV_OFFSET           = (DATA_BASEP_CDNN_OFFSET              + sizeof(DATA_BASEG_cdnnDataDataBaseT)),
   DATA_BASEP_VISION_PROC_OFFSET       = (DATA_BASEP_CDNN_EV_OFFSET           + sizeof(DATA_BASEG_cdnnDataDataBaseT)),
   DATA_BASEP_SLAM_OFFSET              = (DATA_BASEP_VISION_PROC_OFFSET       + sizeof(DATA_BASEG_visionProcDataDataBaseT)),
   DATA_BASEP_SLAM_EV_OFFSET           = (DATA_BASEP_SLAM_OFFSET              + sizeof(DATA_BASEG_slamDataDataBaseT)),
   DATA_BASEP_DPE_PP_OFFSET            = (DATA_BASEP_SLAM_EV_OFFSET           + sizeof(DATA_BASEG_slamDataDataBaseT)),
   DATA_BASEP_PP_OFFSET                = (DATA_BASEP_DPE_PP_OFFSET            + sizeof(DATA_BASEG_dpeDataDataBaseT)),
   DATA_BASEP_TSNR_OFFSET              = (DATA_BASEP_PP_OFFSET                + sizeof(DATA_BASEG_ppDataDataBaseT)),
   DATA_BASEP_GP_TO_XM4_MSG_OFFSET     = (DATA_BASEP_TSNR_OFFSET              + sizeof(DATA_BASEG_tsnrDataBaseT)),
   DATA_BASEP_GP_TO_EV_MSG_OFFSET      = (DATA_BASEP_GP_TO_XM4_MSG_OFFSET     + sizeof(DATA_BASEG_clientMsgDataBaseT)),
   DATA_BASEP_HISTOGRAM_OFFSET         = (DATA_BASEP_GP_TO_EV_MSG_OFFSET      + sizeof(DATA_BASEG_clientMsgDataBaseT)),
} DATA_BASEP_databaseOffsetE;

typedef struct 
{
   OS_LYRG_mutexCpT    *cpMutexHandle;
   OS_LYRG_cpMutexIdE  cpMutexId;
   MEM_MAPG_addrT      dataBaseAddr;
   UINT32 offset;
   UINT32 size;
} DATA_BASEP_dataBaseSectionT;

typedef struct 
{
   DATA_BASEP_moduleStatusE     status;
   void                         *dataBaseBasicVirtAddressP;
   DATA_BASEP_dataBaseSectionT  dataBaseTbl[DATA_BASEG_NUM_DATABASE_E];
} DATA_BASEP_infoT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

#if DEFSG_IS_CEVA
static DATA_BASEP_infoT DATA_BASEP_info PRAGMA_DSECT_LOAD("data_base_sec") =
#else
static DATA_BASEP_infoT DATA_BASEP_info =
#endif
{
   DATA_BASEP_MODULE_STATUS_CLOSE_E,
   NULL,
   {
      {NULL, OS_LYRG_CP_MUTEX_ICC_BOX_GP_DSPA_E,   NULL, DATA_BASEP_ICC_BOX_GP_DSPA_OFFSET,           sizeof(DATA_BASEG_iccBoxDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_ICC_BOX_GP_DSPB_E,   NULL, DATA_BASEP_ICC_BOX_GP_DSPB_OFFSET,           sizeof(DATA_BASEG_iccBoxDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_ICC_BOX_DSPA_GP_E,   NULL, DATA_BASEP_ICC_BOX_DSPA_GP_OFFSET,           sizeof(DATA_BASEG_iccBoxDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_ICC_BOX_DSPB_GP_E,   NULL, DATA_BASEP_ICC_BOX_DSPB_GP_OFFSET,           sizeof(DATA_BASEG_iccBoxDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_JOB_DESCRIPTORS_E,   NULL, DATA_BASEP_JOB_DESCRIPTORS_OFFSET,           sizeof(DATA_BASEG_iccJobsDescriptorDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_FAST_E,              NULL, DATA_BASEP_FAST_OFFSET,                      sizeof(DATA_BASEG_fastDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_CDNN_E,              NULL, DATA_BASEP_CDNN_OFFSET,                      sizeof(DATA_BASEG_cdnnDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_CDNN_EV_E,           NULL, DATA_BASEP_CDNN_EV_OFFSET,                   sizeof(DATA_BASEG_cdnnDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_VISION_PROC_E,       NULL, DATA_BASEP_VISION_PROC_OFFSET,               sizeof(DATA_BASEG_visionProcDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_SLAM_E,              NULL, DATA_BASEP_SLAM_OFFSET,                      sizeof(DATA_BASEG_slamDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_SLAM_EV_E,           NULL, DATA_BASEP_SLAM_EV_OFFSET,                   sizeof(DATA_BASEG_slamDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_DPE_PP_E,            NULL, DATA_BASEP_DPE_PP_OFFSET,                    sizeof(DATA_BASEG_dpeDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_PP_E,                NULL, DATA_BASEP_PP_OFFSET,                        sizeof(DATA_BASEG_ppDataDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_TSNR_E,              NULL, DATA_BASEP_TSNR_OFFSET,                      sizeof(DATA_BASEG_tsnrDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_GP_TO_XM4_E,         NULL, DATA_BASEP_GP_TO_XM4_MSG_OFFSET,             sizeof(DATA_BASEG_clientMsgDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_GP_TO_EV_E,          NULL, DATA_BASEP_GP_TO_EV_MSG_OFFSET,              sizeof(DATA_BASEG_clientMsgDataBaseT)},
      {NULL, OS_LYRG_CP_MUTEX_HISTOGRAM_E,         NULL, DATA_BASEP_HISTOGRAM_OFFSET,                 sizeof(DATA_BASEG_histogramDataBaseT)}
   }
};
#if DEFSG_IS_GP
static OS_LYRG_mutexT DATA_BASEP_typeAccessTblMutex;
#endif

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/
ERRG_codeE DATA_BASEG_initDataBase()
{
   ERRG_codeE                    retCode = DATA_BASE__RET_SUCCESS;
   DATA_BASEG_databaseE          dataBaseNum;
   void                          *dataBaseSpecificAddressP;
   DATA_BASEP_dataBaseSectionT   *dataBaseTblP;
   MEM_MAPG_addrT                oAddressP;
   int                           retVal;
#if DEFSG_IS_GP
   UINT32                        size, dbSize = 0;
#endif
#if (DEFSG_IS_CEVA || DEFSG_IS_EV72)
   CEVA_BOOTG_bootSyncParamsT *CEVA_BOOTP_cevaBootParams;
   CEVA_BOOTP_cevaBootParams = (CEVA_BOOTG_bootSyncParamsT *)MEM_MAPG_CEVA_BOOT_PARAMS_ADDRESS;
   CEVA_BOOTP_cevaBootParams->ddrDataBaseAddress = 0x2001008;
   oAddressP = (void *)CEVA_BOOTP_cevaBootParams->ddrDataBaseAddress;
#else
   retVal = OS_LYRG_aquireMutex(&DATA_BASEP_typeAccessTblMutex);
   if ( retVal == FAIL_E )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "OS_LYRG_aquireMutexCp for DATA_BASEP_typeAccessTblMutex failed !!!\n");
      retCode = DATA_BASE__ERR_UNEXPECTED;
      return retCode;
   }

   MEM_MAPG_getVirtAddr(MEM_MAPG_LRAM_DATA_BASE_E, &oAddressP);
#endif
   DATA_BASEP_info.status = DATA_BASEP_MODULE_STATUS_OPEN_E;
   DATA_BASEP_info.dataBaseBasicVirtAddressP = oAddressP;

   for(dataBaseNum = (DATA_BASEG_databaseE)0 ; dataBaseNum < DATA_BASEG_NUM_DATABASE_E; dataBaseNum++)
   {
      retVal = OS_LYRG_aquireMutexCp(&(DATA_BASEP_info.dataBaseTbl[dataBaseNum].cpMutexHandle), DATA_BASEP_info.dataBaseTbl[dataBaseNum].cpMutexId);
      if ( retVal == FAIL_E )
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,  "OS_LYRG_aquireMutexCp for dataBaseNum %d failed !!!\n", dataBaseNum);
         retCode = DATA_BASE__ERR_CPMUTEX_ACQUIRE_FAIL;
         break;
      }

      dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];
      dataBaseSpecificAddressP = (void *)( (UINT32)(DATA_BASEP_info.dataBaseBasicVirtAddressP) + (dataBaseTblP->offset) );
      dataBaseTblP->dataBaseAddr = dataBaseSpecificAddressP;
#if DEFSG_IS_GP
      //printf("dataBaseNum %d, size %d, address %p (offset %d)\n",dataBaseNum,dataBaseTblP->size,dataBaseTblP->dataBaseAddr,dataBaseTblP->offset);
      memset(dataBaseTblP->dataBaseAddr, 0x00, dataBaseTblP->size);
      dbSize+=dataBaseTblP->size;
#endif
   }

#if DEFSG_IS_GP
   retCode = MEM_MAPG_getSize(MEM_MAPG_LRAM_DATA_BASE_E, &size);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode,  "error getting size of database\n");
   }
   else
   {
      if (dbSize > size)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,  "database size is larger then allocated size in mem_map (allocated %d, actual %d)\n",size,dbSize);
         assert(0);
      }
   }
#endif

#if DEFSG_IS_GP
   OS_LYRG_mutexInitCp();
#endif

   return (retCode);
}

ERRG_codeE DATA_BASEG_getDataBaseCpMutexId(OS_LYRG_cpMutexIdE *cpMutexIdP, DATA_BASEG_databaseE dataBaseNum)
{
   ERRG_codeE retCode = DATA_BASE__RET_SUCCESS;

   if ( DATA_BASEP_info.status == DATA_BASEP_MODULE_STATUS_CLOSE_E )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DATA_BASE module is CLOSE\n");
       retCode = DATA_BASE__ERR_MODULE_NOT_OPEN;
      *cpMutexIdP = 0;
   }
   else
   {
      *cpMutexIdP = (DATA_BASEP_info.dataBaseTbl[dataBaseNum].cpMutexId);
   }

   return (retCode);
}

ERRG_codeE DATA_BASEG_getDataBaseCpMutexHandle(OS_LYRG_mutexCpT **oCpMutexHandle, DATA_BASEG_databaseE dataBaseNum)
{
   ERRG_codeE retCode = DATA_BASE__RET_SUCCESS;

   if ( DATA_BASEP_info.status == DATA_BASEP_MODULE_STATUS_CLOSE_E )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DATA_BASE module is CLOSE\n");
      retCode = DATA_BASE__ERR_MODULE_NOT_OPEN;
      oCpMutexHandle = NULL;
   }
   else
   {
      *oCpMutexHandle = (DATA_BASEP_info.dataBaseTbl[dataBaseNum].cpMutexHandle);
   }
   return (retCode);
}

ERRG_codeE DATA_BASEG_readDataBaseNoMutex(UINT8 *outDataBaseP, DATA_BASEG_databaseE dataBaseNum )
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;
   ERRG_codeE retCode = DATA_BASE__RET_SUCCESS;

   if (DATA_BASEP_info.status == DATA_BASEP_MODULE_STATUS_CLOSE_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DATA_BASE module is CLOSE\n");
      retCode = DATA_BASE__ERR_MODULE_NOT_OPEN;
      outDataBaseP = NULL;
   }

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];
   memcpy(outDataBaseP, (UINT8*)(dataBaseTblP->dataBaseAddr), dataBaseTblP->size);

   return (retCode);
}


ERRG_codeE DATA_BASEG_readDataBase(UINT8 *outDataBaseP, DATA_BASEG_databaseE dataBaseNum, UINT8 holdDb)
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;
   ERRG_codeE retCode = DATA_BASE__RET_SUCCESS;

   if (DATA_BASEP_info.status == DATA_BASEP_MODULE_STATUS_CLOSE_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DATA_BASE module is CLOSE\n");
      retCode = DATA_BASE__ERR_MODULE_NOT_OPEN;
      outDataBaseP = NULL;
   }

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];

   OS_LYRG_lockMutexCp(dataBaseTblP->cpMutexHandle);
   memcpy(outDataBaseP, (UINT8*)(dataBaseTblP->dataBaseAddr), dataBaseTblP->size);
   if(!holdDb)
   {
      OS_LYRG_unlockMutexCp(dataBaseTblP->cpMutexHandle);
   }

   return (retCode);
}

ERRG_codeE DATA_BASEG_writeDataBaseNoMutex(UINT8 *inDataBaseP, DATA_BASEG_databaseE dataBaseNum)
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;
   ERRG_codeE retCode = DATA_BASE__RET_SUCCESS;

   if ( DATA_BASEP_info.status == DATA_BASEP_MODULE_STATUS_CLOSE_E )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DATA_BASE module is CLOSE\n");
      retCode = DATA_BASE__ERR_MODULE_NOT_OPEN;
      inDataBaseP = NULL;
   }

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];
   memcpy((UINT8*)(dataBaseTblP->dataBaseAddr), inDataBaseP, dataBaseTblP->size);

   return (retCode);
}


ERRG_codeE DATA_BASEG_writeDataBase(UINT8 *inDataBaseP, DATA_BASEG_databaseE dataBaseNum, UINT8 dbHolding)
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;
   ERRG_codeE retCode = DATA_BASE__RET_SUCCESS;

   if ( DATA_BASEP_info.status == DATA_BASEP_MODULE_STATUS_CLOSE_E )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "DATA_BASE module is CLOSE\n");
      retCode = DATA_BASE__ERR_MODULE_NOT_OPEN;
      inDataBaseP = NULL;
   }

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];

   if(!dbHolding)
   {
      OS_LYRG_lockMutexCp(dataBaseTblP->cpMutexHandle);
   }
   memcpy((UINT8*)(dataBaseTblP->dataBaseAddr), inDataBaseP, dataBaseTblP->size);
   OS_LYRG_unlockMutexCp(dataBaseTblP->cpMutexHandle);

   return (retCode);
}

void DATA_BASEG_accessDataBaseNoMutex(UINT8 **outDataBaseP, DATA_BASEG_databaseE dataBaseNum)
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];
   *outDataBaseP = (UINT8*)(dataBaseTblP->dataBaseAddr);
}


void DATA_BASEG_accessDataBase(UINT8 **outDataBaseP, DATA_BASEG_databaseE dataBaseNum)
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];
   OS_LYRG_lockMutexCp(dataBaseTblP->cpMutexHandle);
   *outDataBaseP = (UINT8*)(dataBaseTblP->dataBaseAddr);
}


void DATA_BASEG_accessDataBaseRelease(DATA_BASEG_databaseE dataBaseNum)
{
   DATA_BASEP_dataBaseSectionT *dataBaseTblP;

   dataBaseTblP = &DATA_BASEP_info.dataBaseTbl[dataBaseNum];
   OS_LYRG_unlockMutexCp(dataBaseTblP->cpMutexHandle);
}

/*
void DATA_BASEG_show()
{
   DATA_BASEG_databaseE sectionId;

   DATA_BASEG_dpeDataBaseT             *dpeDb  = (DATA_BASEG_dpeDataBaseT *)DATA_BASEP_info.dataBaseTbl[DATA_BASEG_DPE].dataBaseAddr;
   DATA_BASEG_targetAlgJobsDataBaseT   *algJobDb = (DATA_BASEG_targetAlgJobsDataBaseT *)DATA_BASEP_info.dataBaseTbl[DATA_BASEG_TARGET_ALG_JOBS].dataBaseAddr;
   DATA_BASEG_dmaChannelDataBaseT      *chanDbP;
   UINT32 physicalAddr;

   for(sectionId = DATA_BASEG_DMA_CHANNEL_0; sectionId < DATA_BASEG_NUM_DATABASE_E; sectionId++)
   {
      MEM_MAPG_convertVirtualToPhysical((UINT32)DATA_BASEP_info.dataBaseTbl[sectionId].dataBaseAddr, &physicalAddr);
      LOGG_PRINT(LOG_INFO_E, NULL, "Mutex id=%d, physicalAddr=0x%X, size=0x%X, handle=0x%X\n", sectionId, physicalAddr, DATA_BASEP_info.dataBaseTbl[sectionId].size, DATA_BASEP_info.dataBaseTbl[sectionId].cpMutexHandle);
   }

   for(sectionId = DATA_BASEG_DMA_CHANNEL_0; sectionId < DATA_BASEG_DMA_CHANNEL_10; sectionId++)
   {
      chanDbP = (DATA_BASEG_dmaChannelDataBaseT *)DATA_BASEP_info.dataBaseTbl[sectionId].dataBaseAddr;
      LOGG_PRINT(LOG_INFO_E, NULL, "data base picWidth=%d, picHeight=%d, frameMode=%d, frameready=%d, frameNum=%d, inPhyFrameAddress=%d, outPhyFrameAddress=%d, virtualFrameAddress=%d\n",
                                    chanDbP->frameResolution.picWidth,
                                    chanDbP->frameResolution.picHeight,
                                    chanDbP->frameResolution.frameMode,
                                    chanDbP->frameReadyMsg.frameready,
                                    chanDbP->frameReadyMsg.frameNum,
                                    chanDbP->frameReadyMsg.inPhyFrameAddress,
                                    chanDbP->frameReadyMsg.outPhyFrameAddress,
                                    chanDbP->frameReadyMsg.virtualFrameAddress);
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "algJobDb: faceDetectionCevaTargetId=%d, gazeDetectionCevaTargetId=%d, disparityInverseCevaTargetId=%d, disparityToDepthCevaTargetId=%d, depthImprovementCevaTargetId=%d\n",
                                 algJobDb->faceDetectionCevaTargetId,
                                 algJobDb->gazeDetectionCevaTargetId,
                                 algJobDb->disparityInverseCevaTargetId,
                                 algJobDb->disparityToDepthCevaTargetId,
                                 algJobDb->depthImprovementCevaTargetId);

   LOGG_PRINT(LOG_INFO_E, NULL, "disparity data base: disparityOffset=%d, opticalFactor=%d, confidenceThreshold=%d, maxDepth=%d\n",
                                 dpeDb->disparityOffset,
                                 dpeDb->dpeOpticalFactor,
                                 dpeDb->confidenceThreshold,
                                 dpeDb->maxDepth);
}
*/
