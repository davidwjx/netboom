/****************************************************************************
 *
 *   FileName: client_api.c
 *
 *   Author:  Benny V.
 *
 *   Date: 
 *
 *   Description: API for client process
 *
 ****************************************************************************/
 
#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "client_api.h"
#include "client_api_int.h"
#include "stdops.h"
#include "log.h"
#include "icc_common.h"
#include "icc.h"
#include "data_base.h"
//#include "version.h"


#if DEFSG_IS_GP
//#include "format_convert.h"
#include "app_init.h"
#include "hw_mngr.h"
#ifdef OLD_API
#include "hw_regs.h"
#endif
#include "gpio_drv.h"
#include "cde_drv.h"
#include "cde_mngr.h"
#ifdef OLD_API
#include "mem_mngr.h"
#endif
#include "cmem.h"
#include "service.h"
#include "internal_cmd.h"
#include "data_base.h"
#include "gme_mngr.h"
#include "sequence_mngr.h"
#include "xml_db.h"
#include "sensors_mngr.h"
#include "iae_drv.h"

#else
#if DEFSG_IS_CEVA
#include "ceva_init.h"
#include "cevaxm.h"
#include "csl.h"
#include "dma_driver.h"
#include "dma_sync_fifo.h"
#include "xm4_mem_alloc.h"


extern dma_queue_base_t DDMA_DRIVERG_queue_base;
#else
#if DEFSG_IS_EV72
#include "ev_init.h"
#include "evthreads.h"
#include "sched.h"
#include "inu_cnn_pipe_api.h"
#endif
#include <stdarg.h>

#endif

#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define  MAIL_BOX_DONE                    0x02001000 
#define  MAIL_BOX_ERROR                   0x02001004


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
   CLIENT_APIG_hwEventCbsT hwEventsCbFunc;
   CLIENT_APIG_hostCbsT    hostMsgCbFunc;
   CLIENT_APIG_ctrlMsgCbT  gpMsgCbFunc;
   CLIENT_APIG_cevaCbsT    cevaMsgCbFunc;
} CLIENT_APIP_callBackFunctionsT;

typedef struct
{
   void     *bufDesc[INU_DEFSG_NUM_OF_CEVA_IDS_E];
   UINT64   timeStamp[INU_DEFSG_NUM_OF_CEVA_IDS_E];
   BOOL     chanEvtRegister;
   BOOL     invalidateCache;
   CLIENT_APIG_bufferPolicyE  bufferReleasePolicy;
} CLIENT_APIP_chanParamsT;

typedef struct
{
   CLIENT_APIP_callBackFunctionsT   callBackFunctions;
} CLIENT_APIP_infoT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
#if DEFSG_IS_CEVA || DEFSG_IS_EV72
static CLIENT_APIP_infoT         CLIENT_APIP_info;
#endif
#if DEFSG_IS_GP
#ifdef OLD_API
static CLIENT_APIP_chanParamsT   CLIENT_APIP_gpChannelParams[INU_DEFSG_NUM_OF_SYS_CHANNELS_E];
#endif
#endif

/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
#if DEFSG_IS_GP
#ifdef OLD_API
static void CLIENT_APIP_gpFromCevaMsgAckRcv(UINT16 jobDescriptorIndex, ERRG_codeE result);
#endif
#endif

#if DEFSG_IS_CEVA
ERRG_codeE CLIENT_APIG_deinit() PRAGMA_CSECT("client_api_LRAM_csect");
//ERRG_codeE CLIENT_APIG_getVersion(CLIENT_APIG_getVersionT *versionP) PRAGMA_CSECT("client_api_LRAM_csect");
#endif


/****************************************************************************
 ***************        L O C A L         F U N C T I O N S                               ***************
 ****************************************************************************/


#if DEFSG_IS_GP

#ifdef OLD_API   
/****************************************************************************
*
*   Function Name:  CLIENT_APIG_getFormat
*
*   Description: This routine returns the video/webcam format for current sensor
*
*   Returns:
*      Success or specific error code.
*
*   Context:
*      Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_getFormat(INU_DEFSG_sysChannelIdE channel, INU_DEFSG_formatConvertE *formatP)
{
   FORMAT_CONVERTG_statusE ret;

   ret = FORMAT_CONVERTG_getFormat(channel, formatP);
      if(ret != FORMAT_CONVERTG_OK)
   {
      return CLIENT_API__CONVERT_ERROR;
   }
   else
   {
      return (ERRG_codeE)0;
   }
}

/****************************************************************************
*
*  Function Name:  CLIENT_API_INTG_gpFromHostMsgRcv
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_API_INTG_gpFromHostMsgRcv(UINT8 *dataP, UINT32 dataLen)
{
   ERRG_codeE                    retCode = (ERRG_codeE)0;
   CLIENT_APIG_hostMsgParamsT    hostMsgParams;

   hostMsgParams.msgP    = (void *)dataP;
   hostMsgParams.msgSize = dataLen;

   if(CLIENT_APIP_info.callBackFunctions.hostMsgCbFunc.hostMsgCb)
   {
      CLIENT_APIP_info.callBackFunctions.hostMsgCbFunc.hostMsgCb(&hostMsgParams);
   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIP_gpFromCevaMsgAckRcv
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
static void CLIENT_APIP_gpFromCevaMsgAckRcv(UINT16 jobDescriptorIndex, ERRG_codeE result)
{

   DATA_BASEG_iccJobsDescriptorDataBaseT   *jobsDbP;
   CLIENT_APIG_ctrlMsgOpcodeE              msgOpcode;

   DATA_BASEG_databaseE                   dataBaseSection;
   INU_DEFSG_cevaIdE                       cevaId;

   if(ERRG_FAILED(result))
   {
      LOGG_PRINT(LOG_ERROR_E, result, "receive ack for ceva message with error\n");
   }
//   LOGG_PRINT(LOG_INFO_E, NULL, "process done: jobDescriptorIndex=%d, result=0x%X\n", jobDescriptorIndex, result);

   DATA_BASEG_accessDataBase((UINT8**)(&jobsDbP), DATA_BASEG_JOB_DESCRIPTORS);
   dataBaseSection   = jobsDbP->jobDescriptorPool[jobDescriptorIndex].databaseType;
   cevaId            = jobsDbP->jobDescriptorPool[jobDescriptorIndex].arg;
   msgOpcode         = jobsDbP->jobDescriptorPool[jobDescriptorIndex].data;
   DATA_BASEG_accessDataBaseRelease(DATA_BASEG_JOB_DESCRIPTORS);

   switch(dataBaseSection)
   {
      case DATA_BASEG_GP_TO_CEVA_A_CLIENT_MSG:
      {
         DATA_BASEG_clientMsgDataBaseT    *cevaMsgDbP;

         DATA_BASEG_accessDataBase((UINT8**)&cevaMsgDbP, dataBaseSection);
         cevaMsgDbP->msgReady = 0;   
         DATA_BASEG_accessDataBaseRelease(dataBaseSection);
         
         if(msgOpcode == CLIENT_APIG_MSG_CODE_START_PROCESS_E)
         {
            //This is CB from frame processing
            CLIENT_APIG_cevaMsgParamsT   generalMsgForCeva;
            generalMsgForCeva.cevaId    = cevaId;
            generalMsgForCeva.msgSize   = cevaMsgDbP->msgSize;
            generalMsgForCeva.msgP      = (void *)cevaMsgDbP->msgBuf;
           
            if(CLIENT_APIP_info.callBackFunctions.cevaMsgCbFunc.cevaEventCb)
            {
               CLIENT_APIP_info.callBackFunctions.cevaMsgCbFunc.cevaEventCb(&generalMsgForCeva);
            }
         }
         else
         {
            //LOGG_PRINT(LOG_INFO_E, NULL, "receive ack for ceva message jobDescriptorIndex=%d, dataBaseSection=%d\n", jobDescriptorIndex, dataBaseSection);
         }
      }
      break;

      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "ERROR!!! receive ack for ceva message with wrong data base section =%d, jobDescriptorIndex=%d\n", dataBaseSection, jobDescriptorIndex);
      }
      break;

   }

   ICCG_returnJobDescriptor(jobDescriptorIndex);

}

UINT32 CLIENT_APIG_bufferOpen(UINT32 bufSize)
{
   return MEM_MNGRG_bufferOpen(bufSize);
}

ERRG_codeE CLIENT_APIG_bufferClose(UINT32 bufIdx)
{
   ERRG_codeE retval;
   retval = MEM_MNGRG_bufferClose(bufIdx);
   if(MEM_MNGR__RET_SUCCESS != retval)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Memory manager buffer close returned =%d\n", retval);
      return (ERRG_codeE)1;
   }
   return (ERRG_codeE)0;
}

void CLIENT_APIG_getBufferAddr(UINT32 bufIdx, UINT32 *physicalBaseAddrP, UINT32 *virtualBaseAddrP)
{
   return MEM_MNGRG_getBufferAddr(bufIdx, physicalBaseAddrP, virtualBaseAddrP);
}

UINT32 CLIENT_APIG_getBufferSize(UINT32 bufIdx)
{
   return MEM_MNGRG_getBufferSize(bufIdx);
}
#endif
#else

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_cevaFromGpMsgRcv
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
void CLIENT_APIG_dspFromGpMsgRcv(UINT32 dataBaseSection, INU_DEFSG_cevaIdE cevaId, inu_function__operateParamsT *paramsP, UINT16 jobDescriptor)
{
   DATA_BASEG_databaseE sectionId = (DATA_BASEG_databaseE)dataBaseSection;

   switch(sectionId)
   {
       case DATA_BASEG_GP_TO_XM4_FDK_MSG:
       case DATA_BASEG_GP_TO_EV62_FDK_MSG:
      {
         DATA_BASEG_clientMsgDataBaseT *generalMsgDbP;
         CLIENT_APIG_cevaMsgParamsT    generalMsgParams;

         LOGG_PRINT(LOG_DEBUG_E, NULL, "receive general msg from gp. sectionId=%d\n", sectionId);

         DATA_BASEG_accessDataBaseNoMutex((UINT8**)&generalMsgDbP, sectionId);
         memcpy(generalMsgParams.msg, generalMsgDbP->msgBuf, generalMsgDbP->msgSize);
         generalMsgParams.msgSize   = generalMsgDbP->msgSize;
         generalMsgParams.cevaId    = cevaId;

         generalMsgDbP->msgReady      = 0;

         if(CLIENT_APIP_info.callBackFunctions.gpMsgCbFunc)
         {
            CLIENT_APIP_info.callBackFunctions.gpMsgCbFunc(&generalMsgParams, paramsP, jobDescriptor);
         }
      }
      break;
      
      default:
      break;
   }

}

#endif

/****************************************************************************
 ***************     C L I E N T   A P I   F U N C T I O N S                            ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_deinit
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_deinit()
{
   ERRG_codeE retCode = (ERRG_codeE)0;
#if DEFSG_IS_GP
#ifdef OLD_API
   retCode = APP_INITG_close();
#endif
#endif
   return(retCode);
}
#ifdef OLD_API
/****************************************************************************
*
*  Function Name:  CLIENT_APIG_getVersion
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_getVersion(CLIENT_APIG_getVersionT *versionP)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   // FW version
   versionP->fwVerId.fields.major    = MAJOR_VERSION;
   versionP->fwVerId.fields.minor    = MINOR_VERSION;
   versionP->fwVerId.fields.build    = BUILD_VERSION;
   versionP->fwVerId.fields.subBuild = SUB_BUILD_VERSION;
   // HW version
#if DEFSG_IS_GP
   retCode = HW_MNGRG_getHwVersion(&versionP->hwVerId);
#endif
   return(retCode);
}
#endif
/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpSendLog
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
#define CLIENT_APIP_FMT_STRING_LEN            (300)
#if DEFSG_IS_GP
void CLIENT_APIG_gpSendLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...)
{
   char fmtStr  [CLIENT_APIP_FMT_STRING_LEN];
   VA_PACK(fmt, fmtStr);
   LOGG_outputStrLog(CLIENT_GP_ID, level, errCode, fileP, funcP, lineNum, fmtStr);
}
#else
#if DEFSG_IS_CEVA
void CLIENT_APIG_cevaSendLog(INU_DEFSG_logLevelE level, const char * fmt, ...)
{
   char fmtStr  [CLIENT_APIP_FMT_STRING_LEN];
   VA_PACK(fmt, fmtStr);
   LOGG_PRINT(level, NULL, "%s", fmtStr);
}
#else
#if DEFSG_IS_EV72
void CLIENT_APIG_evSendLog(INU_DEFSG_logLevelE level, const char * fmt, ...)
{
   char fmtStr  [CLIENT_APIP_FMT_STRING_LEN];
   VA_PACK(fmt, fmtStr);
   LOGG_PRINT(level, NULL, "%s", fmtStr);
}
#endif
#endif
#endif

#if DEFSG_IS_GP
#ifdef OLD_API
/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpInit
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpInit( CLIENT_APIG_gpCallBackFunctionsT cbsRegister,
                                    int                              wdTimeout,
                                    UINT32                           clientDdrBufSize,
                                    UINT32                           *clientDdrBufPhyAddrP,
                                    UINT32                           *clientDdrBufVirtAddrP,
                                    BOOL                             standAloneMode,
                                    inu_deviceH                      *devicePtr)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   memset(&CLIENT_APIP_info.callBackFunctions,0,sizeof(CLIENT_APIP_callBackFunctionsT));
   CLIENT_APIP_info.callBackFunctions.hwEventsCbFunc  = cbsRegister.hwEventsCbsFunc;
   CLIENT_APIP_info.callBackFunctions.hostMsgCbFunc   = cbsRegister.hostMsgCbsFunc;
   CLIENT_APIP_info.callBackFunctions.cevaMsgCbFunc   = cbsRegister.cevaMsgCbsFunc;

   retCode = APP_INITG_init(wdTimeout, clientDdrBufSize, clientDdrBufPhyAddrP, clientDdrBufVirtAddrP,INU_DEVICE__NORMAL_MODE,devicePtr);

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "INU GP lib init failed !!!\n");
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "INU GP lib init done. Ready for operation. \n\n");

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpGetErrString
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpGetErrString(ERRG_codeE errCode, char *stringP)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   
   INU_STRG_getErrString(errCode, stringP);

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpConfig
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpConfig(CLIENT_APIG_configOpcodeE configOpcode, void *configCbParamsP)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   switch(configOpcode)
   {

      case CLIENT_APIG_OPCODE_GET_CHAN_BUF_CFG_E:
      {
         CLIENT_APIG_chanStreamCfgParamsT *streamClientConfigP = (CLIENT_APIG_chanStreamCfgParamsT *)configCbParamsP;
         CDE_MNGRG_chanStreamConfigT      getStreamConfigParams;

         CDE_MNGRG_getChannelStreamConfig(streamClientConfigP->channelId, &getStreamConfigParams);
         streamClientConfigP->numOfBuffers         = getStreamConfigParams.numBuffers[CDE_MNGRG_RESOLUTION_TYPE_BINNING_E];
         streamClientConfigP->bufferReleasePolicy  = CLIENT_APIP_gpChannelParams[streamClientConfigP->channelId].bufferReleasePolicy;
      }
      break;

      case CLIENT_APIG_OPCODE_SET_CHAN_BUF_CFG_E:
      {
         CLIENT_APIG_chanStreamCfgParamsT *streamClientConfigP = (CLIENT_APIG_chanStreamCfgParamsT *)configCbParamsP;
         CDE_MNGRG_chanStreamConfigT      setStreamConfigParams;

         setStreamConfigParams.numBuffers[CDE_MNGRG_RESOLUTION_TYPE_BINNING_E]       = streamClientConfigP->numOfBuffers;
         setStreamConfigParams.numBuffers[CDE_MNGRG_RESOLUTION_TYPE_FULL_E]          = streamClientConfigP->numOfBuffers;

         CDE_MNGRG_setChannelStreamConfig(streamClientConfigP->channelId, &setStreamConfigParams);

         CLIENT_APIP_gpChannelParams[streamClientConfigP->channelId].bufferReleasePolicy = streamClientConfigP->bufferReleasePolicy;
         CLIENT_APIP_gpChannelParams[streamClientConfigP->channelId].chanEvtRegister     = streamClientConfigP->getChanEvt;
         CLIENT_APIP_gpChannelParams[streamClientConfigP->channelId].invalidateCache     = streamClientConfigP->invalidateCache;
      }
      break;

      case CLIENT_APIG_CFG_OPCODE_IAE_INT_E:
      {
         CLIENT_APIG_GP_LOG(LOG_INFO_E, NULL, "receive IAE_INT config cmd\n");
      }
      break;

      case CLIENT_APIG_CFG_OPCODE_DPE_INT_E:
      {
         CLIENT_APIG_GP_LOG(LOG_INFO_E, NULL, "receive DPE_INT config cmd\n");
      }
      break;

      case CLIENT_APIG_CFG_OPCODE_CDE_INT_E:
      {
         CLIENT_APIG_GP_LOG(LOG_INFO_E, NULL, "receive CDE_INT config cmd\n");
      }
      break;

      default:
      {
         CLIENT_APIG_GP_LOG(LOG_ERROR_E, NULL, "ERR!!! receive unknown config opcode = %d\n", configOpcode);
      }
      break;

   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpToCevaMsgSend
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpToCevaMsgSend(CLIENT_APIG_ctrlMsgOpcodeE msgOpcode, CLIENT_APIG_cevaMsgParamsT *msgParamsP)
{
    
    ERRG_codeE                              retCode = (ERRG_codeE)0;
    UINT16                                  jobDescriptorIndex;
    ICCG_cmdT                               cmd;
    DATA_BASEG_iccJobsDescriptorDataBaseT   *jobsDbP;
    DATA_BASEG_databaseE                    dataBaseSection;
    DATA_BASEG_clientMsgDataBaseT           cevaMsgDb;
    UINT32                                  msgSize = msgParamsP->msgSize;
    INU_DEFSG_cevaIdE                       cevaId = msgParamsP->cevaId;

    if(cevaId == INU_DEFSG_CEVA_ID_A_E)
    {
    dataBaseSection   = DATA_BASEG_GP_TO_CEVA_A_CLIENT_MSG;
    }
    else
    {
    return(CLIENT__ERR_WRONG_CEVA_ID);
    }

    DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, dataBaseSection, 0);
    while(cevaMsgDb.msgReady == 1)
    {
    DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, dataBaseSection, 0);
    }
    if(msgSize <= DATA_BASEG_GENERAL_MSG_MAX_SIZE)
    {
        cevaMsgDb.msgReady   = 1;
        cevaMsgDb.msgSize    = msgSize;
        memcpy(cevaMsgDb.msgBuf, msgParamsP->msgP, msgSize);
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "client msgSize is greater than max size %d>%d\n", msgSize, DATA_BASEG_GENERAL_MSG_MAX_SIZE);
        retCode = 1;
    }
    DATA_BASEG_writeDataBase((UINT8*)(&cevaMsgDb), dataBaseSection, 0);

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = ICCG_getJobDescriptor(&jobDescriptorIndex);

   //   LOGG_PRINT(LOG_INFO_E, NULL, "Send job: dataBaseSection = %d, desciptorIdx =  %d\n", DATA_BASEG_GP_TO_CEVA_A_CLIENT_MSG, jobDescriptorIndex);

      if(ERRG_SUCCEEDED(retCode))
      {
         DATA_BASEG_accessDataBase((UINT8**)&jobsDbP, DATA_BASEG_JOB_DESCRIPTORS);
         jobsDbP->jobDescriptorPool[jobDescriptorIndex].alg         = DATA_BASEG_ALG_GENERAL_MSG;
         jobsDbP->jobDescriptorPool[jobDescriptorIndex].cb          = CLIENT_APIP_gpFromCevaMsgAckRcv;
         jobsDbP->jobDescriptorPool[jobDescriptorIndex].databaseType= dataBaseSection;
         jobsDbP->jobDescriptorPool[jobDescriptorIndex].data        = msgOpcode;
         jobsDbP->jobDescriptorPool[jobDescriptorIndex].arg         = cevaId;
         DATA_BASEG_accessDataBaseRelease(DATA_BASEG_JOB_DESCRIPTORS);
         
         cmd.cmdType  = ICCG_CMD_REQ;
         cmd.descriptorHandle = jobDescriptorIndex;
         if(cevaId == INU_DEFSG_CEVA_ID_A_E)
         {
            cmd.dspTarget = ICCG_CMD_TARGET_DSPA;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Error sending CMD to DSP (wrong ceva ID = %d)\n", cevaId);
            return(CLIENT__ERR_WRONG_CEVA_ID);
         }

         ICCG_sendIccCmd(&cmd);

      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "Error sending CMD to DSP (stack is empty)\n" );
      }
   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpToHostMsgSend
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpToHostMsgSend(void *msgP, UINT32 msgSize)
{
   INTERNAL_CMDG_sendFrameHdrT hdr;
   ERRG_codeE retCode;
   //Sending frame pointer to Client service
   memset(&hdr,0,sizeof(hdr)); //to be filled and forwarded in client service
   retCode = SERVICEG_copyAndSendDataChLocal(SERVICEG_getHandle(INUG_SERVICE_CLIENT_STREAM_E),
         INTERNAL_CMDG_SEND_FRAME_HDR_E, &hdr,(UINT8 *)msgP,msgSize);
   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpToHostFrameSend
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpToHostFrameSend(CLIENT_APIG_frameToHostParamsT *userStreamParamsP)
{
   ERRG_codeE                    retCode = (ERRG_codeE)0;
   CDE_MNGRG_chanStreamConfigT   getStreamConfigParams;


   CDE_MNGRG_getChannelStreamConfig(userStreamParamsP->channelId, &getStreamConfigParams);
   if(getStreamConfigParams.streamerH && userStreamParamsP->channelId != INU_DEFSG_SYS_CH_9_DEPTH_ID_E)
   {
      STREAMERG_send(getStreamConfigParams.streamerH, getStreamConfigParams.streamId, userStreamParamsP->bufDescP, userStreamParamsP->frameCntr, userStreamParamsP->timeStamp);
   }
   else
   {
      retCode = CDE_MNGRG_notifyDone(userStreamParamsP->dVal);
      if(ERRG_FAILED(retCode))
      {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed notify %x\n",userStreamParamsP->dVal);
      }    
       MEM_POOLG_free(userStreamParamsP->bufDescP);
   }

   return(retCode);
}


/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpToHostFrameRelease
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpBufferRelease(CLIENT_APIP_eventHandleT *handle)
{
   ERRG_codeE                    retCode = (ERRG_codeE)0;
   
   retCode = CDE_MNGRG_notifyDone(handle->dVal);
   if(ERRG_FAILED(retCode))
   {
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed notify %x\n",handle->dVal);
   }    
   MEM_POOLG_free(handle->bufDesc);

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpReadRegister
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpReadRegister(UINT32 regAddr, UINT32 *regValP)
{
   ERRG_codeE           retCode = (ERRG_codeE)0;
   INUG_ioctlReadRegT   readRegParams;

   readRegParams.regType      = INU_DEVICE__REG_SOC_E;
   readRegParams.socReg.addr  = regAddr;

   retCode = HW_REGSG_readReg(&readRegParams);
   *regValP = readRegParams.socReg.val;

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpWriteRegister
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpWriteRegister(UINT32 regAddr, UINT32 regVal)
{
   ERRG_codeE           retCode = (ERRG_codeE)0;
   inu_device__writeRegT  writeRegParams;

   writeRegParams.phase       = INU_DEFSG_DB_PHASE_0_E;
   writeRegParams.regType     = INU_DEVICE__REG_SOC_E;
   writeRegParams.socReg.addr = regAddr;
   writeRegParams.socReg.val  = regVal;

   retCode = HW_REGSG_writeReg(&writeRegParams);

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpStartDma
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpStartDma(UINT32 srcAddr, UINT32 dstAddr, UINT32 transferSize)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   (void)(srcAddr);
   (void)(dstAddr);
   (void)(transferSize);
   //TODO
   LOGG_PRINT(LOG_ERROR_E, NULL, "Client API DMA operation not supported\n");
   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpPollingDma
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpPollingDma()
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   //TODO
   LOGG_PRINT(LOG_ERROR_E, NULL, "Client API DMA operation not supported\n");
   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpCacheWr
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpCacheWr(void *ptr, UINT32 size)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   CMEM_cacheWb(ptr, size);

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_gpCacheInv
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpCacheInv(void *ptr, UINT32 size)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   CMEM_cacheInv(ptr, size);

   return(retCode);
}


/****************************************************************************
*
*  Function Name:  CLIENT_APIG_convertPhysicalToVirtual
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_convertPhysicalToVirtual(UINT32 physicalAddr, UINT32 *oVirtualAddrP)
{
   return MEM_MAPG_convertPhysicalToVirtual(physicalAddr, oVirtualAddrP);
}


/****************************************************************************
*
*  Function Name: CLIENT_APIG_gpGetExposure
*
*  Description: Get exposure in [uS] of a sensor's context
*
*  Inputs:
*         sensorSelect - the sensor to get the exposure from
*         context - the sensor context (if alternate mode is not used, context should be INU_DEFSG_SENSOR_CONTEX_A
*
*  Outputs: expTimeUsP - actual value
*
*  Returns: Error in case of a failure
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpGetExposure(UINT32 *expTimeUsP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context)
{
   return 0;//SENSORS_MNGRG_getSensorExpTime(expTimeUsP, sensorSelect, context);
}


/****************************************************************************
*
*  Function Name: CLIENT_APIG_gpSetExposure
*
*  Description: Set exposure in [uS] of a sensor's context
*
*  Inputs: expTimeUsP - points to value to set
*         sensorSelect - the sensor to perform the exposure change
*         context - the sensor context (if alternate mode is not used, context should be INU_DEFSG_SENSOR_CONTEX_A
*
*  Outputs: expTimeUsP - actual value
*
*  Returns: Error in case of a failure
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpSetExposure(UINT32 *expTimeUsP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context, UINT8 stereo)
{
   return 0;//SENSORS_MNGRG_setSensorExpTime(expTimeUsP, sensorSelect, context, stereo);
}


/****************************************************************************
*
*  Function Name: CLIENT_APIG_gpGetGain
*
*  Description: Get gain of a sensor's context. Gain value is the actual value written to the gain register
*
*  Inputs: sensorSelect - the sensor to get the gain
*         context - the sensor context (if alternate mode is not used, context should be INU_DEFSG_SENSOR_CONTEX_A
*
*  Outputs: analogGainP,digitalGainP - actual value
*
*  Returns: Error in case of a failure
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpGetGain(UINT32 *analogGainP, UINT32 *digitalGainP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context)
{
   return 0;//return SENSORS_MNGRG_getSensorGain(analogGainP, digitalGainP, sensorSelect, context);
}


/****************************************************************************
*
*  Function Name: CLIENT_APIG_gpSetGain
*
*  Description: Set gain of a sensor's context. Gain value is the actual value written to the gain register
*
*  Inputs: analogGainP,digitalGainP - points to value to set
*         sensorSelect - the sensor to get the gain
*         context - the sensor context (if alternate mode is not used, context should be INU_DEFSG_SENSOR_CONTEX_A
*
*  Outputs: analogGainP,digitalGainP - actual value
*
*  Returns: Error in case of a failure
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_gpSetGain(UINT32 *analogGainP, UINT32 *digitalGainP, INU_DEFSG_senSelectE sensorSelect, INU_DEFSG_sensorContextE context, UINT8 stereo)
{
   return 0;//return SENSORS_MNGRG_setSensorGain(analogGainP, digitalGainP, sensorSelect, context, stereo); todo
}


/****************************************************************************
*
*  Function Name: CLIENT_APIG_cfgHistRoi
*
*  Description: Config the histogram's ROI
*
*  Inputs: histSelect - the iau to config
*         cfgP - the 3 ROI's configuration
*
*  Outputs: 
*
*  Returns: Error in case of a failure
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_cfgHistRoi(UINT32 histSelect, CLIENT_APIG_histRoiCfgT *cfgP)
{
   return IAE_DRVG_histRoiCfg(histSelect, cfgP);
}
#endif //OLD_API
#endif //DEFSG_IS_GP

#if DEFSG_IS_CEVA

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_cevaInit
*
*  Description:
*
*  Returns:
*     Success or error.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_cevaInit(CLIENT_APIG_ctrlMsgCbT  gpMsgCbFunc,
                                     CLIENT_APIG_initCbT     initCbFunc)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   memset(&CLIENT_APIP_info.callBackFunctions,0,sizeof(CLIENT_APIP_callBackFunctionsT));
   CLIENT_APIP_info.callBackFunctions.gpMsgCbFunc     = gpMsgCbFunc;

   CEVA_INITG_init(initCbFunc);

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "INU CEVA lib init failed !!!\n");
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "INU CEVA lib init done. Ready for operation. \n\n");

   return(retCode);
}



/****************************************************************************
*
*  Function Name:  CLIENT_APIG_dma_create_1d_desc
*
*  Description:
*
*  Returns:     Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_dma_create_1d_desc(CLIENT_APIG_dmaDescT* desc, UINT32 length ,CLIENT_APIG_dmaTransferDirE direction )
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   if (DMA_STATUS_OK != dma_create_1d_desc((dma_desc_t*)desc, length, (dma_transfer_dir_e)direction, DMA_TYPE_LINEAR))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error creating 1d descriptor\n");
      retCode = (ERRG_codeE)1;
   }
   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_dma_create_2d_desc
*
*  Description:
*
*  Returns:     Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_dma_create_2d_desc(CLIENT_APIG_dmaDescT* desc, UINT32 twoD_transfer_width, UINT32 twoD_transfer_height,  UINT32 twoD_src_stride , UINT32 twoD_dst_stride, CLIENT_APIG_dmaTransferDirE direction )
{
   ERRG_codeE retCode = (ERRG_codeE)0;
	dma_status_e dmaStatus = dma_create_2d_desc((dma_desc_t*)desc, twoD_transfer_width, twoD_transfer_height, twoD_src_stride, twoD_dst_stride, (dma_transfer_dir_e)direction, DMA_TYPE_LINEAR);

   if (DMA_STATUS_OK != dmaStatus)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error creating 2d descriptor %d\n", dmaStatus);
      retCode = (ERRG_codeE)1;
   }
   return(retCode);
}



/****************************************************************************
*
*  Function Name:  CLIENT_APIG_dma_enqueue_desc
*
*  Description:
*
*  Returns:     Success or error.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_dma_enqueue_desc(CLIENT_APIG_dmaDescT* desc ,void* src, void* dst )
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   //enqueue task
   if (DMA_STATUS_OK != dma_enqueue_desc(&DDMA_DRIVERG_queue_base, (dma_desc_t*)desc, src, dst))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error enqueuing 1d descriptor src 0x%x dst 0x%x\n",src,dst);
      retCode = (ERRG_codeE)1;
   }
   return(retCode);
}
/****************************************************************************
*
*  Function Name:  CLIENT_APIG_dma_enqueue_sync_point
*
*  Description:
*
*  Returns:     Success or error.
*
*  Context:
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_dma_enqueue_sync_point(UINT32* value)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   //Set sync point
   if (DMA_STATUS_OK != dma_enqueue_sync_point(&DDMA_DRIVERG_queue_base, value))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error enqueuing sync point\n");
      retCode = (ERRG_codeE)1; 
   }
   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_dma_wait_sync_point
*
*  Description:
*
*  Returns:     Success or error.
*
*  Context:
*
****************************************************************************/
void CLIENT_APIG_dma_wait_sync_point(UINT32 value)
{
   //Poll on sync point
   dma_wait_sync_point(&DDMA_DRIVERG_queue_base, value);
}

/****************************************************************************
*
*  Function Name:  XM4_MEM_ALLOCG_allocBuffer
*
*  Description: allocates internal memory
*
*  Returns:     Success or error code.
*
*  Context:
*
****************************************************************************/

CLIENT_APIG_XM4_MEM_ALLOCG_retvalE	CLIENT_APIG_XM4_MEM_ALLOCG_allocBuffer(UINT32 requestedSize, CLIENT_APIG_XM4_MEM_ALLOCG_allocationTypeE type, CLIENT_APIG_XM4_MEM_ALLOCG_startAddrAlignmentE alignment, CLIENT_APIG_XM4_MEM_ALLOCG_handleT *handleP)
{
	return XM4_MEM_ALLOCG_allocBuffer(requestedSize, (XM4_MEM_ALLOCG_allocationTypeE)type, (XM4_MEM_ALLOCG_startAddrAlignmentE)alignment, (XM4_MEM_ALLOCG_handleT*)handleP);
}
/****************************************************************************
*
*  Function Name:  XM4_MEM_ALLOCG_freeBuffer
*
*  Description: free allocatated memory
*
*  Returns:     Success or error code.
*
*  Context:
*
****************************************************************************/

CLIENT_APIG_XM4_MEM_ALLOCG_retvalE	CLIENT_APIG_XM4_MEM_ALLOCG_freeBuffer(CLIENT_APIG_XM4_MEM_ALLOCG_handleT *handleP)
{
	return XM4_MEM_ALLOCG_freeBuffer((XM4_MEM_ALLOCG_handleT*)handleP);
}
/****************************************************************************
*
*  Function Name:  XM4_MEM_ALLOCG_getMaxFreeBuffer
*
*  Description: this routin gets the max buffer size available, user can use this routine before calling to XM4_MEM_ALLOCG_allocBuffer()
*
*  Returns:     Success or error code.
*
*  Context:
*
****************************************************************************/

UINT32 CLIENT_APIG_XM4_MEM_ALLOCG_getMaxFreeBuffer(CLIENT_APIG_XM4_MEM_ALLOCG_startAddrAlignmentE alignment, CLIENT_APIG_XM4_MEM_ALLOCG_allocationTypeE type)
{
	return XM4_MEM_ALLOCG_getMaxFreeBuffer((XM4_MEM_ALLOCG_startAddrAlignmentE)alignment, (XM4_MEM_ALLOCG_allocationTypeE)type);
}


#if 0

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_cevaVdmaChannelOpen
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_cevaVdmaChannelOpen(UINT32 *channelIdP)
{
   ERRG_codeE                    retCode = (ERRG_codeE)0;
   VDMA_DRVG_chOpenIoctlParamsT  vdmaChOpenParams;

   retCode = VDMA_DRVG_channelOpen(&vdmaChOpenParams);
   *channelIdP = vdmaChOpenParams.returnChannelIdx;

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_cevaVdmaChannelOpen
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_cevaVdmaChannelclose(UINT32 channelId)
{
   ERRG_codeE                    retCode = (ERRG_codeE)0;
   VDMA_DRVG_chCloseIoctlParamsT vdmaChCloseParams;

   vdmaChCloseParams.channelIdx = channelId;
   retCode = VDMA_DRVG_channelClose(&vdmaChCloseParams);

   return(retCode);
}
#endif

#if OLD_API
/****************************************************************************
*
*  Function Name:  CLIENT_APIG_formatConvertInit
*
*  Description: this routine initializes the parameters to  CLIENT_APIG_formatConvertFrame
*
*   Parameters:    startVmemP - the VMEM space to use for VDMA loads/stores
*                  vmemSize   - the size of VMEM buffer
*                  width - the width of the area to convert
*                  height - the height of the area to convert
*                  srcStride - source stride in pixels
*                  dstStride - destination stride in pixels
*                  formatType - the supported conversion type (enumerator)
*                  handleP - handle for this conversion
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_formatConvertInit(UINT8 *startVmemP, UINT32 vmemSize, UINT32 width, UINT32 height, UINT32 srcStride, UINT32 dstStride, INU_DEFSG_formatConvertE formatType, INT32 *handleP)
{
   FORMAT_CONVERTG_statusE ret;
   ret = FORMAT_CONVERTG_init(startVmemP, vmemSize, width, height, srcStride, dstStride, formatType, handleP);
   if(ret != FORMAT_CONVERTG_OK)
   {
      return CLIENT_API__CONVERT_HANLDE_ALLOCATION;
   }
   else
   {
      return (ERRG_codeE)0;
   }
}



/****************************************************************************
*
*  Function Name:  CLIENT_APIG_getReverseFormat
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_getReverseFormat(INU_DEFSG_formatConvertE format, INU_DEFSG_formatConvertE *reverseFormatP)
{   
   FORMAT_CONVERTG_statusE ret;
   
   ret = FORMAT_CONVERTG_getReverseFormat(format, reverseFormatP);
      if(ret != FORMAT_CONVERTG_OK)
   {
      return CLIENT_API__CONVERT_ERROR;
   }
   else
   {
      return (ERRG_codeE)0;
   }
}


/****************************************************************************
*
*  Function Name:  CLIENT_APIG_formatConvertFrame
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_formatConvertFrame(UINT8 *inImage, UINT8 *outImage, INT32 handle)
{   
   FORMAT_CONVERTG_statusE ret;
   
   ret = FORMAT_CONVERTG_convertFrame(inImage, outImage, handle);
      if(ret != FORMAT_CONVERTG_OK)
   {
      return CLIENT_API__CONVERT_ERROR;
   }
   else
   {
      return (ERRG_codeE)0;
   }
}

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_formatConvertClose
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_formatConvertClose(INT32 handle)
{
   FORMAT_CONVERTG_statusE ret;
   ret = FORMAT_CONVERTG_close(handle);
   if(ret != FORMAT_CONVERTG_OK)
   {
      return CLIENT_API__CONVERT_CLOSE_ERROR;
   }
   else
   {
      return (ERRG_codeE)0;   
   }
}
#endif


//CEVA profiling functions
/****************************************************************************
*
*  Function Name:  CLIENT_APIG_mssGetClock
*
*  Description: gets free running XM4 clock ticks
*
*  Returns:
*     
*
*  Context:
*     
*
****************************************************************************/
void CLIENT_APIG_mssGetClock(UINT64 * tick)
{
   *tick = mss_get_clock();
}
#endif

#if DEFSG_IS_EV72

/****************************************************************************
*
*  Function Name:  CLIENT_APIG_evInit
*
*  Description:
*
*  Returns:
*     Success or error.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE CLIENT_APIG_evInit(CLIENT_APIG_ctrlMsgCbT  gpMsgCbFunc,
                                     CLIENT_APIG_initCbT     initCbFunc)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   memset(&CLIENT_APIP_info.callBackFunctions,0,sizeof(CLIENT_APIP_callBackFunctionsT));
   CLIENT_APIP_info.callBackFunctions.gpMsgCbFunc     = gpMsgCbFunc;

	volatile int *mail_box_done  = (volatile int *)MAIL_BOX_DONE;
	volatile int *mail_box_error = (volatile int *)MAIL_BOX_ERROR;

	*mail_box_done = 0x1;
	*mail_box_error = 0x0;
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "Start client api! %d CPUs\n", evGetMaxCpu());

	if(initCbFunc)
	{
		initCbFunc();
	}
	 
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "INU EV lib init done. Ready for operation. \n");

   EV_INITG_init();

   return(retCode);
}

/****************************************************************************
*
*  Function Name: CLIENT_APIG_schedPushFinishedwJob
*
*  Description: Update  scheduler a job is done, by pushing its descriptor to the Finished jobs que
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: ALGs/CNN threads
*
****************************************************************************/

void CLIENT_APIG_schedPushFinishedwJob(UINT32 jobDescriptor)
{
   SCHEDG_pushFinishedJob(jobDescriptor);
}

int CLIENT_APIG_createMsgQue(CLIENT_APIG_msgQueT *msgQueP, UINT32 numOfEvents)
{
   return OS_LYRG_createMsgQue((OS_LYRG_msgQueT *)msgQueP, numOfEvents);
}

int CLIENT_APIG_deleteMsgQue(CLIENT_APIG_msgQueT *msgQueP)
{
   return OS_LYRG_deleteMsgQue((OS_LYRG_msgQueT *)msgQueP);
}

int CLIENT_APIG_sendMsg(CLIENT_APIG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len)
{
   return OS_LYRG_sendMsg((OS_LYRG_msgQueT *)msgQueP, msgP, len);   
}

int CLIENT_APIG_recvMsg(CLIENT_APIG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeoutMsec)
{
   return OS_LYRG_recvMsg((OS_LYRG_msgQueT *)msgQueP, msgP, lenP, timeoutMsec);
}

int CLIENT_APIG_cnnHandleMessage(void *message)
{
		return ev_inu_handle_message(message);
}

void CLIENT_APIG_cnnHandleReleaseNetMessage(void *message)
{
		ev_inu_handle_release_net_pipe(message);
}

UINT32 CLIENT_APIG_netInfoGetChannelSwap(UINT32 netId)
{
	return CDNN_EVG_netInfoGetChannelSwap(netId);
}

UINT32 CLIENT_APIG_netInfoGetProcessingFlags(UINT32 netId)
{
	return CDNN_EVG_netInfoGetProcesssingFlags(netId);
}

void CLIENT_APIG_evCnnInit(UINT32 coreId)
{
	ev_inu_api_init(coreId);
}

#endif



