#include "inu2_internal.h" 
#include "inu2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "inu_comm.h"
#include "os_lyr.h"
#include "marshal.h"
#include "inu_device.h"
#include "log.h"
#include <stdarg.h>
#include "helsinki.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INU_REF__MEM_POOL_SIZE  (8)
#define INU_REF__MEM_POOL_BUFF_SIZE_BYTES  (3*512)
#define INU_REF__MEM_POOL_BUFF_NUM  (64)

typedef struct
{
   inu_device                *device;
   inu_comm                  *ctrlCommH;
   inu_comm                  *dataCommH;
   OS_LYRG_mutexT            ioctlMutex;
   OS_LYRG_mutexT            allocBufMutex;
   OS_LYRG_event             ioctlEvent;
   int                       ioctlRetVal;
   MEM_POOLG_handleT         ctrlMemPoolH[INU_REF__MEM_POOL_SIZE];
   MEM_POOLG_bufDescT        *syncCtrlBufDesc;
   inu_ref__statsT           refStats;
   BOOLEAN                   dataChanDirect;
   char                      userName[MAX_NAME_LEN];
   #ifdef HELSINKI_DISABLE_MULTI_INPUT_SUPPORT
   int                       disableMultiInputSuportOverride;
   #endif
}inu_ref__privData;

static inu_ref__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_REF";
static MEM_POOLG_handleT memPoolRefStatic = NULL;
static bool longerTimeout = 0;

static ERRG_codeE inu_ref__deallocMemPool(inu_ref *me);
static ERRG_codeE inu_ref__addExistingMemPool(inu_ref *me, MEM_POOLG_handleT poolH);
static ERRG_codeE inu_ref__allocMemPool(inu_ref *me,MEM_POOLG_handleT *poolH, UINT32 type, UINT32 bufferSize, UINT32 numBuffers, UINT8* buf, int rstptr);
static int inu_ref__findFreeMemPool(inu_ref *me);


static const char* inu_ref__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_ref__dtor(inu_ref *me)
{
   ERRG_codeE ret;
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;

   ret = inu_ref__deallocMemPool(me);
   if (ERRG_SUCCEEDED(ret))
   {
      inu_device__removeRef(privP->device, me);
      
      OS_LYRG_releaseEvent(privP->ioctlEvent,0);
      OS_LYRG_releaseMutex(&privP->ioctlMutex);
      OS_LYRG_releaseMutex(&privP->allocBufMutex);

   if (me->privP)
      free(me->privP);
   }
}


/* Constructor */
ERRG_codeE inu_ref__ctor(inu_ref *me, inu_ref__CtorParams *ctorParamsP)
{
   inu_ref__CtorParams *refCtorParamsP = (inu_ref__CtorParams*)ctorParamsP;
   ERRG_codeE ret;
   INT32      status;
   inu_ref__privData *privP;

   me->statsEnable = ctorParamsP->statsEnable;
   privP = (inu_ref__privData*)malloc(sizeof(inu_ref__privData));
   if (!privP)
   {
      return INU_REF__ERR_OUT_OF_MEM;
   }
   memset(privP,0,sizeof(inu_ref__privData));
   me->privP = privP;

   if (me->statsEnable)
   {
      OS_LYRG_getTime(&privP->refStats.prevStatsTimeSec, &privP->refStats.prevStatsTimeMs);
   }

   privP->device    = (inu_device*)refCtorParamsP->device;
   privP->ctrlCommH = (inu_comm*)ctorParamsP->ctrlCommH;
   privP->dataCommH = (inu_comm*)ctorParamsP->dataCommH;
   privP->dataChanDirect = ctorParamsP->dataChanDirectFlag;
   memcpy (privP->userName , ctorParamsP->userName,sizeof(privP->userName));

#ifdef HELSINKI_DISABLE_MULTI_INPUT_SUPPORT
   // Exceptional Override for EV72 nodes. EV72 nodes require multple input support.
   if(!strcmp(ctorParamsP->userName, "Stream_STEREO_FE") || !strcmp(ctorParamsP->userName, "Stream_STEREO_FE2") 
      || !strcmp(ctorParamsP->userName, "Sout_stereo_fdk") || !strcmp(ctorParamsP->userName, "FDK_STEREO_DEMO_FE"))
      {
         privP->disableMultiInputSuportOverride = 1;
      }
      else
      {
         privP->disableMultiInputSuportOverride = 0;
      }
#endif


#if DEFSG_IS_HOST
   ret = inu_device__addRef(privP->device, me);
   if (ERRG_FAILED(ret))
      assert(0);
   refCtorParamsP->id = me->id;
#else
    if (!inu_ref__standAloneMode(me))
    {
        me->id = refCtorParamsP->id;
        ret = inu_device__registerRef(privP->device, me);
        if (ERRG_FAILED(ret))
        assert(0);
   
    }
    else
    {
         ret = inu_device__addRef(privP->device, me);
        if (ERRG_FAILED(ret))
          assert(0);
    }
#endif

   //Mutex for accessing mempool list
   status = OS_LYRG_aquireMutex(&privP->allocBufMutex);
   if (status != SUCCESS_E)
      assert(0);

   //Mutex for synchronizing IOCTL calls
   status = OS_LYRG_aquireMutex(&privP->ioctlMutex);
   if (status != SUCCESS_E)
      assert(0);

   //Event for synchronizing IOCTL calls
   privP->ioctlEvent = OS_LYRG_createEvent(0);
   if (privP->ioctlEvent == NULL)
      assert(0);

   //one static message mempool for all refs
   if (!memPoolRefStatic)
   {
      ret = inu_ref__allocMemPool(me,&memPoolRefStatic, MEM_POOLG_TYPE_ALLOC_HEAP_E, INU_REF__MEM_POOL_BUFF_SIZE_BYTES, INU_REF__MEM_POOL_BUFF_NUM,NULL, 0);
      if (ERRG_FAILED(ret))
         assert(0);
   }

   //mempools for comm usage
   ret = inu_ref__addExistingMemPool(me, memPoolRefStatic);
   if (ERRG_FAILED(ret))
      assert(0);

   if (privP->ctrlCommH)
   {
      ret = inu_comm__register(privP->ctrlCommH, me);
      if (ERRG_FAILED(ret))
         assert(0);
   }

   if (privP->dataCommH)
   {
      ret = inu_comm__register(privP->dataCommH, me);
      if (ERRG_FAILED(ret))
         assert(0);
   }

   OS_LYRG_getTime(&privP->refStats.openTimeSec, &privP->refStats.openTimeMs);
   return ret;
}

#ifdef HELSINKI_DISABLE_MULTI_INPUT_SUPPORT
inline int inu_ref__getDisableMultiInputSuportOverride(inu_ref *me)
{
   inu_ref__privData *privP = (inu_ref__privData*)((inu_ref*)me)->privP;
   return privP->disableMultiInputSuportOverride;
}
#endif


static int inu_ref__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(msgP);
   FIX_UNUSED_PARAM_WARN(msgCode);
   return 0;
}

static int inu_ref__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   FIX_UNUSED_PARAM_WARN(msgP);
   FIX_UNUSED_PARAM_WARN(msgCode);
   FIX_UNUSED_PARAM_WARN(dataP);
   FIX_UNUSED_PARAM_WARN(dataLen);
   FIX_UNUSED_PARAM_WARN(bufDescP);
   inu_device__rxNotify(inu_ref__getDevice(me));
   return 0;
}

static int inu_ref__txDone(inu_ref *me, void *bufP)
{
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(bufP);
   return 0;
}

static void inu_ref__showStats(inu_ref *me)
{
   inu_ref__statsT *statsInfoP = &((inu_ref__privData*)me->privP)->refStats;
   UINT32          refDurationMs     = 0;
   UINT32          curTimeDurationMs = 0;

   refDurationMs     = OS_LYRG_deltaMsec(statsInfoP->openTimeSec,       statsInfoP->openTimeMs);
   curTimeDurationMs = OS_LYRG_deltaMsec(statsInfoP->prevStatsTimeSec,  statsInfoP->prevStatsTimeMs);
   OS_LYRG_getTime(&statsInfoP->prevStatsTimeSec, &statsInfoP->prevStatsTimeMs);  //Remember current time

   LOGG_PRINT(LOG_INFO_E, NULL, "%s statistics:\n",me->p_vtable->p_name(me));
   LOGG_PRINT(LOG_INFO_E, NULL, "Ref timing: Exist for %u sec, %u sec from last statistics\n", refDurationMs/1000, curTimeDurationMs/1000);
   LOGG_PRINT(LOG_INFO_E, NULL, "Channel count %u, dropped %u\n", me->stats.chCnt,me->stats.chDropCnt);
}

void inu_ref__vtableInitDefaults( inu_ref__VTable *vtableP )
{
   vtableP->p_name = inu_ref__name;
   vtableP->p_dtor = inu_ref__dtor;
   vtableP->p_ctor = inu_ref__ctor;

   vtableP->p_rxSyncCtrl = inu_ref__rxIoctl;
   vtableP->p_rxAsyncData = inu_ref__rxData;
   vtableP->p_txDone = inu_ref__txDone;
   vtableP->p_showStats = inu_ref__showStats;
}

static void inu_ref__vtable_init()
{
   if (!_bool_vtable_initialized) {
      inu_ref__vtableInitDefaults(&_vtable);
      _bool_vtable_initialized = true;
   }
}

void inu_ref__setVTable(inu_ref* me, const inu_ref__VTable *vtable)
{
   me->p_vtable = vtable;
}


const inu_ref__VTable *inu_ref__vtable_get(void)
{
   inu_ref__vtable_init();
   return &_vtable;
}

void *inu_ref__getCtrlComm(inu_ref *me)
{
   return ((inu_ref__privData*)me->privP)->ctrlCommH;
}

void *inu_ref__getDataComm(inu_ref *me)
{
   return ((inu_ref__privData*)me->privP)->dataCommH;
}


void *inu_ref__getDevice(inu_ref *me)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   return (void*)privP->device;
}

static ERRG_codeE inu_ref__getMinMempool(inu_ref *me, UINT32 len, MEM_POOLG_handleT *mem_pool)
{
   ERRG_codeE retCode = INU_REF__RET_SUCCESS;
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   int i=0, minPoolIndex = -1;
   UINT32 minPoolSize = 0xffffffff;
   for (i=0;i<INU_REF__MEM_POOL_SIZE;++i)
   {
       if (privP->ctrlMemPoolH[i])
       {
          if ((len <= MEM_POOLG_getBufSize(privP->ctrlMemPoolH[i])) && 
               MEM_POOLG_getNumFree(privP->ctrlMemPoolH[i]) &&
               MEM_POOLG_getBufSize(privP->ctrlMemPoolH[i]) < minPoolSize)
          {
             minPoolIndex = i;
             minPoolSize = MEM_POOLG_getBufSize(privP->ctrlMemPoolH[i]);
          }
       }
   }

   if (minPoolIndex != -1)
   {
      *mem_pool = privP->ctrlMemPoolH[minPoolIndex];
      return retCode;
   }

   //printf("ref %s (id=%d) requested len = %d, max =%d\n",me->p_vtable->p_name(me),me->id,len, MEM_POOLG_getBufSize(privP->ctrlMemPoolH[0]));
   return MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL;
}

ERRG_codeE inu_ref__allocBuf(inu_ref *me, UINT32 len, MEM_POOLG_bufDescT **bufP)
{
   ERRG_codeE retCode = INU_REF__RET_SUCCESS;
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   MEM_POOLG_handleT poolH;

   OS_LYRG_lockMutex(&privP->allocBufMutex);
   retCode = inu_ref__getMinMempool(me,len,&poolH);
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = MEM_POOLG_alloc(poolH,len,bufP);
      if(*bufP == NULL)
      {
         assert(0);
      }
   }
   OS_LYRG_unlockMutex(&privP->allocBufMutex);

   return retCode;
}


ERRG_codeE inu_ref__timedAllocBuf(inu_ref *me, UINT32 len, UINT32 timeoutMsec, MEM_POOLG_bufDescT **outBufP)
{
   ERRG_codeE  ret = CONN_LYR__RET_SUCCESS;
   INT32       leftMsec = (INT32)timeoutMsec;
   UINT32      sleepMsec;
   MEM_POOLG_bufDescT *bufP = NULL;

   do
   {
      ret = inu_ref__allocBuf(me, len, &bufP);
      if (ERRG_FAILED(ret))
      {
         if (ret == MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL)
         {
            sleepMsec = (leftMsec > 10) ? 10 : leftMsec;
            LOGG_PRINT(LOG_DEBUG_E, NULL, "warn alloc timeout left=%d %d\n", leftMsec, sleepMsec);
            if (sleepMsec > 0)
            {
               OS_LYRG_usleep(sleepMsec * 1000);
               leftMsec -= sleepMsec;
            }
         }
         else
            break; //non-timeout error
      }
   } while ((!bufP) && (leftMsec > 0));

   *outBufP = bufP;
   return ret;
}

static ERRG_codeE inu_ref__addExistingMemPool(inu_ref *me, MEM_POOLG_handleT poolH)
{
   ERRG_codeE ret = INU_REF__RET_SUCCESS;
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   int memPoolI=inu_ref__findFreeMemPool(me);
   if (memPoolI>=0)
   {
      privP->ctrlMemPoolH[memPoolI] = poolH;
   }
   else
   {
      ret = INU_REF__ERR_OUT_OF_RSRCS;
   }
   return ret;
}

static ERRG_codeE inu_ref__allocMemPool( inu_ref *me,MEM_POOLG_handleT *poolH, UINT32 type, UINT32 bufferSize, UINT32 numBuffers, UINT8* buf, int rstptr)
{
   ERRG_codeE ret = INU_REF__RET_SUCCESS;
   MEM_POOLG_cfgT poolCfg;

   poolCfg.bufferSize = bufferSize;
   poolCfg.numBuffers = numBuffers;
   poolCfg.resetBufPtrInAlloc = rstptr;
   poolCfg.freeCb = NULL;
   poolCfg.memP = buf;
   poolCfg.type = (MEM_POOLG_typeE)type;
   poolCfg.refPtr = me;
   ret = MEM_POOLG_initPool(poolH, &poolCfg);
   if (!poolH)
      return INU_REF__ERR_OUT_OF_RSRCS;

   return ret;
}

static ERRG_codeE inu_ref__deallocMemPool(inu_ref *me)
{
   ERRG_codeE ret = INU_REF__RET_SUCCESS;
   int i=0;
   inu_ref__privData *privDataP = (inu_ref__privData*)me->privP;
   
   for (i=0;i<INU_REF__MEM_POOL_SIZE;++i)
   {
      if (privDataP->ctrlMemPoolH[i] && (privDataP->ctrlMemPoolH[i] != memPoolRefStatic))
      {
         MEM_POOLG_closePool(privDataP->ctrlMemPoolH[i]);
         privDataP->ctrlMemPoolH[i] = NULL;
      }
   }
   return ret;
}

BOOLEAN inu_ref__dataChanDirect(inu_ref *ref)
{
   inu_ref__privData *privP = (inu_ref__privData*)ref->privP;
   return privP->dataChanDirect;
}

//consider setting as static and calling to it via callback
void inu_ref__ctrlSyncDone(inu_ref *me, int retVal, void *bufDesc)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   privP->ioctlRetVal     = retVal;
   privP->syncCtrlBufDesc = (MEM_POOLG_bufDescT*)bufDesc;
   OS_LYRG_setEvent(privP->ioctlEvent);
   
   // once disconnected, notify device->ioctrlEvent
   if (retVal == USB_CTRL_COM__ERR_LOST_CONNECTION)
   {
       inu_device* dev = privP->device;
       privP = (inu_ref__privData*)(((inu_ref*)dev)->privP);
       //iinfo("notify device %p ioctlEvent: %p", dev, privP->ioctlEvent);
       OS_LYRG_setEvent(privP->ioctlEvent);
   }
}

ERRG_codeE inu_ref__sendCtrlSync(inu_ref *me, int cmd, void *argP, UINT32 timeout)
{
   ERRG_codeE ret;
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   INT32      waitEventRet = SUCCESS_E;

   if (!inu_device__connState((inu_device*)inu_ref__getDevice(me)))
      return INU_REF__ERR_CONN_DOWN;

   if (inu_ref__standAloneModeCodeIgnr(me,cmd))
      return INU_REF__RET_SUCCESS;
   //Serialize access for IOCTLs 
   OS_LYRG_lockMutex(&privP->ioctlMutex);
   ret = inu_comm__sendIoctl(privP->ctrlCommH, me, cmd, argP);
   if (CONN_LYR__ERR_CONNECTION_IS_DOWN != ret)
   {
       //iinfo("before waiting ref(%p)->ioctlEvent: %p with cmd: %x", me, privP->ioctlEvent, cmd);
       waitEventRet = OS_LYRG_waitEvent(privP->ioctlEvent, timeout);
       //iinfo("ref %p cmd %x over!", me, cmd);
   }
   else
   {
       INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "connection has broken down, don't need to wait for resp...", cmd); 
       waitEventRet = FAIL_E;
   }

   if(waitEventRet == SUCCESS_E)
   {
      ret = (ERRG_codeE)privP->ioctlRetVal;
      if (ERRG_SUCCEEDED(ret) && privP->syncCtrlBufDesc)
      {
         memcpy(argP, privP->syncCtrlBufDesc->dataP, privP->syncCtrlBufDesc->dataLen);
      }
      MEM_POOLG_free(privP->syncCtrlBufDesc);
   }
   else
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E, NULL, "Timeout on command 0x%x ret 0x%x\n",cmd, ret);
      ret = INU_REF__ERR_TIMEOUT;
   }

   privP->syncCtrlBufDesc = NULL;
   OS_LYRG_unlockMutex(&privP->ioctlMutex);
   if (INU_REF__ERR_TIMEOUT == ret) {
       inu_device__linkEventCallbackWrapper(0, INUG_SERVICE_LINK_DISCONNECT_E, privP->device);
   }
   
   return ret;
}

//Remote new uses device for creating new instances of the objects on target.
ERRG_codeE inu_ref__remoteNew(inu_ref *me, int cmd, void *argP)
{
   ERRG_codeE ret = INU_REF__RET_SUCCESS;
#if DEFSG_IS_HOST
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   inu_device *device = privP->device;
   ret = inu_ref__sendCtrlSync((inu_ref*)device, cmd, argP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
   ((inu_ref__CtorParams*)argP)->device = device;   //restore to local
#else
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(cmd);
   FIX_UNUSED_PARAM_WARN(argP);
#endif
   return ret;
}

//Remote delete uses device for removing instances of the objects on target.
ERRG_codeE inu_ref__remoteDelete(inu_ref *me, int cmd, void *argP)
{
   ERRG_codeE ret = INU_REF__RET_SUCCESS;
#if DEFSG_IS_HOST
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   inu_device *device = privP->device;
   ret = inu_ref__sendCtrlSync((inu_ref*)device, cmd, argP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
#else
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(cmd);
   FIX_UNUSED_PARAM_WARN(argP);
#endif
   return ret;
}

ERRG_codeE inu_ref__copyAndSendDataAsync(inu_ref *me, int cmd, void *argP, void *bufferP, int bufferLen)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   if (!inu_device__connState((inu_device*)inu_ref__getDevice(me)))
      return INU_REF__ERR_CONN_DOWN;

   return inu_comm__sendData(privP->dataCommH, me, cmd, argP, NULL, bufferP, bufferLen);
}

ERRG_codeE inu_ref__sendDataAsync(inu_ref *me, int cmd, void *argP, void *dataBufDescP)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;

   if (!inu_device__connState((inu_device*)inu_ref__getDevice(me)))
      return INU_REF__ERR_CONN_DOWN;

   return inu_comm__sendData(privP->dataCommH, me, cmd, argP, (MEM_POOLG_bufDescT*)dataBufDescP, NULL, 0);
}

ERRG_codeE inu_ref__check(inu_refH me)
{
   FIX_UNUSED_PARAM_WARN(me);
   //can verify it exists in context?
   return INU_REF__RET_SUCCESS;
}

static int inu_ref__findFreeMemPool(inu_ref *me)
{
   int memPoolI=0;
    inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   for (memPoolI=0;memPoolI<INU_REF__MEM_POOL_SIZE;++memPoolI)
   {
      if (privP->ctrlMemPoolH[memPoolI]==NULL)
      {
            return memPoolI;
      }
   }
   return -1;
}

ERRG_codeE inu_ref__removeMemPool(inu_ref *me, MEM_POOLG_handleT memPoolH)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   ERRG_codeE status = INU_REF__ERR_OUT_OF_RSRCS;
   int memPoolI;

   for (memPoolI = 0; memPoolI < INU_REF__MEM_POOL_SIZE; memPoolI++)
   {
      if (memPoolH == privP->ctrlMemPoolH[memPoolI])
      {
         MEM_POOLG_closePool(privP->ctrlMemPoolH[memPoolI]);
			privP->ctrlMemPoolH[memPoolI] = NULL;
         status = INU_REF__RET_SUCCESS;
      }
   }
  
   return status;
}


ERRG_codeE inu_ref__addMemPool(inu_ref *me, UINT32 type, UINT32 bufferSize, UINT32 numBuffers,UINT8* buf, int rstptr, MEM_POOLG_handleT *memPoolH)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   ERRG_codeE status = INU_REF__RET_SUCCESS;
   int memPoolI=inu_ref__findFreeMemPool(me);
   if (memPoolI>=0)
   {
      status = inu_ref__allocMemPool(me,&privP->ctrlMemPoolH[memPoolI], type, bufferSize, numBuffers, buf, rstptr);
      if (ERRG_SUCCEEDED(status))
      {
         *memPoolH = privP->ctrlMemPoolH[memPoolI];
      }
      //printf("[%s] ctrlMemPoolH[%d]: %p, type %d, size %d, num %d�� buf %x, stat = %d\n", privP->userName, memPoolI, *memPoolH, type, bufferSize, numBuffers, buf, ERRG_GET_ERR_CODE(status));
   }
   else
   {
      status = INU_REF__ERR_OUT_OF_RSRCS;
   }
   return status;
}

int inu_ref__isSame(inu_ref *ref1, inu_ref *ref2)
{
   return (ref1->p_vtable == ref2->p_vtable);
}

void *inu_ref__changeDataComm(inu_ref *me, void *comm)
{
   inu_ref__privData *privP = (inu_ref__privData*)me->privP;
   void *oldComm = NULL;

   if (privP->dataCommH)
   {
      oldComm = privP->dataCommH;
      if (privP->dataCommH != privP->ctrlCommH)
         inu_comm__unregister((inu_comm*)oldComm,me);
   }

   privP->dataCommH = (inu_comm*)comm;
   inu_comm__register(privP->dataCommH, me);
   return oldComm;
}

const char *inu_ref__getName(inu_refH me)
{
   inu_ref *ref = (inu_ref*)me;
   return ref->p_vtable->p_name(ref);
}

const char *inu_ref__getUserName(inu_refH me)
{
    inu_ref__privData *privP = (inu_ref__privData*)((inu_ref*)me)->privP;
    return privP->userName;
}

inu_ref__types inu_ref__getRefType(inu_refH me)
{
   inu_ref *ref = (inu_ref*)me;
   return ref->refType;
}

unsigned int inu_ref__getRefId(inu_refH me)
{
   inu_ref *ref = (inu_ref*)me;
   return ref->id;
}

BOOLEAN inu_ref__instanceOf(inu_refH refH, inu_ref__types type)
{
   return (((inu_ref*)refH)->refType == type ? TRUE : FALSE);
}

void inu_ref__log(inu_ref *ref, INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *file, const char *func, int line, const char * fmt, ...)
{
   char fmtStr[250];
   char instanceStr[270];

   va_list   args;

   va_start(args,fmt);
   vsprintf(fmtStr ,fmt, args);
   sprintf (instanceStr, "%s(%d): %s ", inu_ref__getName(ref), ref->id, inu_ref__getUserName(ref));
   strcat  (instanceStr, fmtStr);

   LOGG_outputStrLog(CURR_PID, level, errCode, file, func, line, instanceStr);

   va_end(args);
}

BOOLEAN inu_ref__standAloneMode(inu_refH refH)
{
   inu_ref__privData *privP = (inu_ref__privData *)((inu_ref *)refH)->privP;
   inu_device *device = privP->device;

   return inu_dev__standAloneMode(device);
}
BOOLEAN inu_ref__standAloneModeCodeIgnr(inu_refH refH,UINT32 cmd)
{
   INTERNAL_CMDG_ioctlCmdE internalCmd = (INTERNAL_CMDG_ioctlCmdE)cmd;
   BOOLEAN retVal = FALSE;
   if (!inu_ref__standAloneMode(refH))
      return 0;
   switch (internalCmd)
   {
      //case INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_E:
      //case INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_ALLOC_E:
      case INTERNAL_CMDG_SEND_LUT_HDR_E:
      case INTERNAL_CMDG_BIND_NODES_E:
      case INTERNAL_CMDG_FUNCTION_START_E:
      case INTERNAL_CMDG_FUNCTION_STOP_E:
      case INTERNAL_CMDG_NEW_GRAPH_E:
      case INTERNAL_CMDG_NEW_LOGGER_E:
      case INTERNAL_CMDG_NEW_IMAGE_E:
      case INTERNAL_CMDG_NEW_SOC_CH_E:
      case INTERNAL_CMDG_NEW_ISP_CH_E:
      case INTERNAL_CMDG_NEW_HISTOGRAM_E:
      case INTERNAL_CMDG_NEW_HISTOGRAM_DATA_E:
      case INTERNAL_CMDG_NEW_SOC_WRITER_E:
      case INTERNAL_CMDG_NEW_STREAMER_E:
      case INTERNAL_CMDG_NEW_SENSOR_E:
      case INTERNAL_CMDG_NEW_SENSORS_GROUP_E:
      case INTERNAL_CMDG_NEW_IMU_E:
      case INTERNAL_CMDG_NEW_IMU_DATA_E:
      case INTERNAL_CMDG_NEW_TEMPERATURE_E:
      case INTERNAL_CMDG_NEW_TEMPERATURE_DATA_E:
      case INTERNAL_CMDG_NEW_FDK_E:
      case INTERNAL_CMDG_NEW_DATA_E:
      case INTERNAL_CMDG_NEW_CDNN_E:
      case INTERNAL_CMDG_NEW_CDNN_DATA_E:
      case INTERNAL_CMDG_NEW_VISION_PROC_E:
      case INTERNAL_CMDG_NEW_PP_E:
      case INTERNAL_CMDG_NEW_MIPI_CH_E:
      case INTERNAL_SYSTEM_SET_CONFIG:
      case INTERNAL_CMDG_SEND_CONFIG_DB_E:
      {
            retVal=TRUE;
      }
      break;
      default:
            retVal=FALSE;
         break;
   }
   return retVal;

}

void inu_ref__deallocStaticPool()
{
   MEM_POOLG_closePool(memPoolRefStatic);
   memPoolRefStatic = NULL;
}

//Longer timeout. Default value is (50*1000). For testing of MIPI RX side, we set 
//INU_REF__SYNC_DFLT_TIMEOUT_MSEC by "-t" option in SandBoxFW to (5*1000*1000)
UINT32 inu_ref__getSyncDfltTimeoutMsec()
{
    if (longerTimeout) 
    {
        return (5*1000*1000);
    }
    else
    {
		//here the number 50000 will be treated as 50s with unit of million-sec, that's too huge for synchronization, therefor reduce it to 2000 as 2sec which should be enough
        return (10*1000);
    }
}
 
void inu_ref__useLongerTimeout(bool useLongerTimeout)
{
    longerTimeout = useLongerTimeout;
    LOGG_PRINT(LOG_INFO_E, NULL, "timeout is now %lu ms\n", INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

#ifdef __cplusplus
}
#endif
