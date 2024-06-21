/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err_defs.h"
#include "internal_cmd.h"

#include "inu2.h"
#include "inu2_internal.h"

#include "log.h"


/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
static inu_ref__VTable _vtable;
static bool _bool_vtable_initialized            = 0;
static const char *name                         = "INU_LOGGER";
static inu_logger *logger                       = NULL;

#define INU_LOGGER__READ_TIMEOUT_MSEC     1000
#define INU_LOGGERP_NAME_STR              "LOGGER"
#define INU_LOGGERP_DATA_MSGQ_NAME_HOST   "/HostLoggerDataQue"

#define INU_LOGGER_SIZE_BYTES       ((400) + 64)
#define INU_LOGGER_NUM_BUFS         (1024) //check if there is an issue in host linux

#if (DEFSG_IS_HOST_LINUX)
#define INU_LOGGERP_MSGQ_BUFFERS         (10)
#else
#define INU_LOGGERP_MSGQ_BUFFERS         (200)
#endif
#define INU_LOGGERP_DATA_MSGQ_MAX   (INU_LOGGERP_MSGQ_BUFFERS)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct
{
   OS_LYRG_msgQueT      msgQue;
   inu_log__configT     logConfig;
   OS_LYRG_threadParams threadParams;
   MEM_POOLG_handleT    poolH;
}inu_logger__privData;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE inu_logger__createQue(OS_LYRG_msgQueT *msgQue);
static ERRG_codeE inu_logger__enque(inu_logger *me, logDataT  *logDataP);
static ERRG_codeE inu_logger__deque(inu_logger *me, logDataT  *logDataP, UINT32 timeout_msec);

#if DEFSG_IS_HOST
#else
   static ERRG_codeE  inu_logger__createThread(inu_logger *me);
#endif


/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void inu_logger__dtor(inu_ref *me)
{
   inu_logger__privData *privP   = (inu_logger__privData*)((inu_logger*)me)->privP;
   inu_logger__stop();
#if DEFSG_IS_HOST
   OS_LYRG_deleteMsgQue(&privP->msgQue);
#else
   OS_LYRG_deleteMsgQue(&privP->msgQue);
   OS_LYRG_closeThread(privP->threadParams.threadHandle);
#endif
   inu_ref__vtable_get()->p_dtor(me);
   free(privP);
}

/****************************************************************************
*
*  Function Name: inu_logger__ctor
*
*  Description: creates a logger object
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static ERRG_codeE inu_logger__ctor(inu_logger *me, inu_logger__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_logger__privData *privP;
   sprintf(ctorParamsP->ref_params.userName, "LOGGER");
   inu_ref__ctor(&me->ref, &ctorParamsP->ref_params);
#if DEFSG_IS_HOST && DEFSG_IS_OS_WINDOWS
   //internal implementation of msg que in windows is in c++, and requires a constructor for it
   privP = new inu_logger__privData;
#else
   privP = (inu_logger__privData*)malloc(sizeof(inu_logger__privData));
   memset(privP, 0, sizeof(inu_logger__privData));
#endif
   memcpy((void*)&(privP->logConfig),(void*)&(ctorParamsP->logConfig),sizeof(inu_log__configT));
   me->privP = privP;

   ret = inu_ref__addMemPool(&me->ref, MEM_POOLG_TYPE_ALLOC_HEAP_E, INU_LOGGER_SIZE_BYTES, INU_LOGGER_NUM_BUFS,NULL, 0,&privP->poolH);//temp - need to take sizes from input
   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_logger__createQue(&(privP->msgQue));
#if (DEFSG_IS_HOST)
      LOGG_setDbgLevel(ctorParamsP->logConfig.log_levels[HOST_ID]);
      LOGG_setSendLogFlag(ctorParamsP->logConfig.hostSendLogs);
      LOGG_setPrintLogFlag(ctorParamsP->logConfig.hostPrintLogs);
#else
      LOGG_setDbgLevel(ctorParamsP->logConfig.log_levels[GP_ID]);
      LOGG_setSendLogFlag(ctorParamsP->logConfig.gpSendLogs);
      LOGG_setSendLogCdcFlag(ctorParamsP->logConfig.gpSendCdcLogs);
      LOGG_setPrintLogFlag(ctorParamsP->logConfig.gpPrintLogs);
#endif
   }

   if (ERRG_SUCCEEDED(ret))
   {
#if (DEFSG_IS_GP)
      ret = inu_logger__createThread(me);
      LOGG_start((void*)me, NULL);
#endif
      logger = me;
   }

   return ret;
}


static const char* inu_logger__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static int inu_logger__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   return inu_ref__vtable_get()->p_rxSyncCtrl(me, msgP, msgCode);
}

static int inu_logger__rxData(inu_ref *ref, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   inu_logger           *me         = (inu_logger*)ref;
   inu_logger__privData *privP      = (inu_logger__privData*)me->privP;
   LOGP_dataHdrT        *hdrP    =(LOGP_dataHdrT *)msgP;
   logDataT             logData;
   inu_ref__vtable_get()->p_rxAsyncData(ref, msgCode, msgP, dataP, dataLen, bufDescP);
   if(privP->logConfig.hostPrintGpLogs)
   {
      printf("%s", (const char *)&dataP[0]);
   }

   if(privP->logConfig.hostSendGpLogs)
   {
      logData.freeBufPtr               =*bufDescP;
      logData.dataP                    =(char *)((MEM_POOLG_bufDescT *)(*bufDescP))->dataP;
      logData.dataP[hdrP->msgSize-1]   = '\0';
      logData.dataLen                  = hdrP->msgSize;
      if (ERRG_FAILED(inu_logger__enque((inu_logger*)me,&logData)))
      {
         MEM_POOLG_free((MEM_POOLG_bufDescT *)(*bufDescP));
      }
   }
   return 0;
}

void inu_logger__vtable_init()
{
   if (!_bool_vtable_initialized)
   {
      inu_ref__vtableInitDefaults(&_vtable);
      _vtable.p_name             = inu_logger__name;
      _vtable.p_ctor             = (inu_ref__Ctor*)inu_logger__ctor;
      _vtable.p_dtor             = inu_logger__dtor;
      _vtable.p_rxSyncCtrl       = inu_logger__rxIoctl;
      _vtable.p_rxAsyncData      = inu_logger__rxData;
      _bool_vtable_initialized   = true;
   }
}

const inu_ref__VTable *inu_logger__vtable_get(void)
{
   inu_data__vtable_init();
   return &_vtable;
}


#if (DEFSG_IS_GP)
static ERRG_codeE  inu_logger__createThread(inu_logger *me)
{
   inu_logger__privData *privP=(inu_logger__privData*)me->privP;
   //create thread context for ceva logging transmitting
   privP->threadParams.func          = (OS_LYRG_threadFunction)LOGG_cevaThread;
   privP->threadParams.param         = NULL;
   privP->threadParams.event         = NULL;
   privP->threadParams.id            = OS_LYRG_SERVICE_LOGGER_THREAD_ID_E;
   privP->threadParams.threadHandle  = OS_LYRG_createThread(&privP->threadParams);
   if(!privP->threadParams.threadHandle)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "logger thread create failed\n");
      return INU_LOGGER__ERR_UNEXPECTED;
   }
   return INU_LOGGER__RET_SUCCESS;
}
#endif


static ERRG_codeE inu_logger__createQue(OS_LYRG_msgQueT *msgQue)
{
   int status;
   memcpy(msgQue->name, INU_LOGGERP_DATA_MSGQ_NAME_HOST, strlen(INU_LOGGERP_NAME_STR)+1);
   msgQue->maxMsgs   =INU_LOGGERP_DATA_MSGQ_MAX;
   msgQue->msgSize   =sizeof(logDataT);
   status = OS_LYRG_createMsgQue(msgQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
   if(status != SUCCESS_E)
   {
      printf("que create failed: que %s maxMsg=%d msgSize=%d\n", msgQue->name, msgQue->maxMsgs, msgQue->msgSize);
      return INU_LOGGER__ERR_UNEXPECTED;
   }
   return INU_LOGGER__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: inu_logger__enque
*
*  Description: Enque log data into queue.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static ERRG_codeE inu_logger__enque(inu_logger *me, logDataT  *logDataP)
{
   inu_logger__privData *privP   = (inu_logger__privData*)me->privP;
   int status;
   status = OS_LYRG_sendMsg(&privP->msgQue, (UINT8*)logDataP, sizeof(logDataT));
   if (status == FAIL_E)
   {
      printf("enque error, free data\n");
      return INU_LOGGER__ERR_UNEXPECTED;
   }
   return INU_LOGGER__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: inu_logger__deque
*
*  Description: Dequeue log data from queue.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static ERRG_codeE inu_logger__deque(inu_logger *me, logDataT  *logDataP, UINT32 timeout_msec)
{
   INT32       status;
   UINT32 len                    = sizeof(logDataT);
   inu_logger__privData *privP   = NULL;

   if (me == NULL)
   {
       return INU_LOGGER__ERR_UNEXPECTED;
   }

   privP = (inu_logger__privData*)me->privP;

   status = OS_LYRG_recvMsg(&privP->msgQue,(UINT8*)logDataP,&len,timeout_msec);
   if((status == FAIL_E) || (len != sizeof(logDataT)))
   {
      return INU_LOGGER__ERR_TIMEOUT;
   }
   return INU_LOGGER__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: inu_logger__read
*
*  Description: provides the ability to read log data from host.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
ERRG_codeE inu_logger__read(inu_logH meH, logDataT *logDataP)
{
   return inu_logger__deque((inu_logger*)meH, logDataP, INU_LOGGER__READ_TIMEOUT_MSEC);
}


/****************************************************************************
*
*  Function Name: inu_logger__new
*
*  Description: recieves parameters and creates a new logger object under the ref inheritance
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
ERRG_codeE inu_logger__new(inu_logH *meH, inu_log__initParams *initParamsP)
{
   inu_logger__CtorParams ctorParams;
   ctorParams.ref_params.device  = initParamsP->deviceH;
   memcpy(&(ctorParams.logConfig),&(initParamsP->configParams),sizeof(inu_log__configT));
   return inu_factory__new((inu_ref**)meH, &ctorParams, INU_LOGGER_REF_TYPE);
}

/****************************************************************************
*
*  Function Name: inu_logger__delete
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void inu_logger__delete(inu_logH meH)
{
   inu_factory__delete((inu_ref*)meH, 0);
}

/****************************************************************************
*
*  Function Name: inu_logger__start
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Logger service
*
****************************************************************************/
void inu_logger__start(inu_logH meH)
{
   inu_logger  *me               = (inu_logger*)meH;
   inu_logger__privData *privP   = (inu_logger__privData*)me->privP;
   LOGG_start((void*)me, &privP->msgQue);
}

/****************************************************************************
*
*  Function Name: inu_logger__stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Logger service
*
****************************************************************************/
void inu_logger__stop()
{
   LOGG_stop();
}


/****************************************************************************
*
*  Function Name: inu_logger__free
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Logger service
*
****************************************************************************/
ERRG_codeE inu_logger__free(logDataT *logData)
{
   MEM_POOLG_free((MEM_POOLG_bufDescT *)(logData->freeBufPtr));
   return INU_STREAMER__RET_SUCCESS;
}



