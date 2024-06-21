#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "version.h"
#include "log.h"
#if DEFSG_IS_GP
#include "app_init.h"
#include "hw_mngr.h"
#include "hcg_mngr.h"
#include <stdarg.h> 
#include "cmem.h"
#include "mem_map.h"
#include "icc.h"
#include "assert.h"
#endif

#define INU_FDK__MAX_CACHE_SIZE_OP (288*1024)
#define INU_FDK__CACHE_SIZE_CHUNK  (16*1024)
UINT16 fdkDmaCore;

typedef struct inu_fdk__privData
{
   inu_fdk__initNodeParamsT *paramsP;
   UINT32                   frameNum;
   UINT64                   dataCtr;
   MEM_POOLG_handleT        userPoolH;
}inu_fdk__privData;

typedef struct
{
   inu_fdk__initNodeParamsT cbs[INU_FDK__MAX_NODES_NUMBER];
   int numUsed;
}inu_fdk__callbacksT;

#if DEFSG_IS_GP
static ERRG_codeE inu_fdk__getNodeCbTbl(int nodeId, inu_fdk__initNodeParamsT **cbsRegister);
static ERRG_codeE inu_fdk__userDataReleased (void *arg, MEM_POOLG_bufDescT *bufDescP);
#endif

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_FDK";

#if DEFSG_IS_GP
/* the table is used to hold the callback functions for each fdk node */
static bool _bool_cbsTable_initialized = 0;
static inu_fdk__callbacksT cbsTbl;
static inu_fdk__sensorIoctlFuncListT sensorIoctlFuncList[FDK_SENSOR_DRVG_NUM_OF_IOCTLS_E] = { 0 };
#endif
static inu_fdk__initNodeParamsT nullCbsTbl;

static const char* inu_fdk__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_fdk__dtor(inu_ref *me)
{
   inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   if(privP->paramsP->dtorCb)
   {
      privP->paramsP->dtorCb(me);
   }
#endif
   free(privP);
}

/* Constructor */
ERRG_codeE inu_fdk__ctor(inu_fdk *me, inu_fdk__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_fdk__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_fdk__privData*)malloc(sizeof(inu_fdk__privData));
      if (!privP)
      {
         return INU_FDK__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_fdk__privData));
      me->privP = privP;

#if DEFSG_IS_GP
      ret = inu_fdk__getNodeCbTbl(ctorParamsP->nodeId, &privP->paramsP);
      if (ERRG_SUCCEEDED(ret))
      {
         if (ctorParamsP->numBuffs)
         {
            //create mempool for fdk created data.
            MEM_POOLG_cfgT cfg;
            cfg.bufferSize= ctorParamsP->buffSize;
            cfg.numBuffers = ctorParamsP->numBuffs;
            cfg.resetBufPtrInAlloc = 0;
            cfg.freeCb = inu_fdk__userDataReleased;
            cfg.freeArg = me;
            cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
            cfg.memP = NULL;
            ret = MEM_POOLG_initPool(&privP->userPoolH, &cfg);
         }
         if (ERRG_SUCCEEDED(ret))
         {         
            if (privP->paramsP->ctorCb)
            {
               privP->paramsP->ctorCb(me);
            }
         }
      }
#endif
   }
   return ret;
}


static int inu_fdk__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   return inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
}

static int inu_fdk__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
#if DEFSG_IS_GP
   switch(msgCode)
   {
      case(INTERNAL_CMDG_DATA_SEND_E):
      {
         //todo - keep buffer until release by the fdk
         //todo - check hdr, pass hdr? validate the frame ctr?
         inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
         if (privP->paramsP->rxDataCb)
         {
            privP->paramsP->rxDataCb(me, dataP, dataLen);
         }

      }break;
      default:
      break;
   }
#endif
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(msgP);
   FIX_UNUSED_PARAM_WARN(bufDescP);
   FIX_UNUSED_PARAM_WARN(dataP);
   FIX_UNUSED_PARAM_WARN(dataLen);
   FIX_UNUSED_PARAM_WARN(msgCode);
   return 0;
}

static int inu_fdk__txDone(inu_ref *me, void *bufP)
{
#if DEFSG_IS_GP
   inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
   if (privP->paramsP->txDataDoneCb)
   {
      privP->paramsP->txDataDoneCb(me, (unsigned char*)bufP);
   }
#else
   FIX_UNUSED_PARAM_WARN(bufP);
   FIX_UNUSED_PARAM_WARN(me);
#endif
   return 0;
}

static void inu_fdk__newInput(inu_node *me, inu_node *input)
{
   inu_function__vtable_get()->node_vtable.p_newInput(me, input);
}

static void inu_fdk__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me, output);

}

static void inu_fdk__removeInput(inu_node *me, inu_node *input)
{
   inu_function__vtable_get()->node_vtable.p_removeInput(me, input);
}

static void inu_fdk__removeOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_removeOutput(me, output);
}

static void inu_fdk__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
#if DEFSG_IS_GP
   int i;
   inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
   inu_function__vtable_get()->p_operate(me, inputParamsP);

   if (privP->paramsP->flags & INU_FDK__AUTO_CACHE_INV_FLAG)
   {
      for (i = 0; i < inputParamsP->dataInputsNum; i++)
      {
         MEM_POOLG_bufDescT *bufP;
         inu_data__bufDescPtrGet(inputParamsP->dataInputs[i],&bufP);
         inu_fdk__gpCacheInv(bufP->dataP,bufP->dataLen);
      }
   }

   if (privP->paramsP->operateCb)
   {
      privP->paramsP->operateCb(me, inputParamsP);
   }
   
   if (privP->paramsP->flags & INU_FDK__AUTO_CACHE_WB_FLAG)
   {
       for (i = 0; i < inputParamsP->dataInputsNum; i++)
       {
          MEM_POOLG_bufDescT *bufP;
          inu_data__bufDescPtrGet(inputParamsP->dataInputs[i],&bufP);
          inu_fdk__gpCacheWr(bufP->dataP,bufP->dataLen);
       }
   }

   if (privP->paramsP->flags & INU_FDK__AUTO_DONE_DATA_FLAG)
   {
      for (i = 0; i < inputParamsP->dataInputsNum; i++)
      {
         inu_function__doneData(me,inputParamsP->dataInputs[i]);
      }
   }

   if (privP->paramsP->flags & INU_FDK__AUTO_OPERATE_DONE_FLAG)
   {
      inu_function__complete(me);
   }
#else
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(inputParamsP);
#endif
}

static ERRG_codeE inu_fdk__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
   inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
   ret = inu_function__vtable_get()->p_start(me, startParamP);
#if DEFSG_IS_GP
   if (privP->paramsP->startCb)
   {
      privP->paramsP->startCb(me,startParamP);
   }
#else
   FIX_UNUSED_PARAM_WARN(privP);
#endif
   return ret;
}

static ERRG_codeE inu_fdk__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
#if DEFSG_IS_GP
   if (privP->paramsP->stopCb)
   {
      privP->paramsP->stopCb(me,stopParamP);
   }
#else
   FIX_UNUSED_PARAM_WARN(privP);
#endif
   return ret;
}


static void inu_fdk__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   FIX_UNUSED_PARAM_WARN(inputParamsP);
#if DEFSG_IS_GP
   inu_fdk__privData *privP = (inu_fdk__privData*)((inu_fdk*)me)->privP;
   //This is CB from frame processing
   inu_fdk__dspMsgParamsT  generalMsgForDsp;
   DATA_BASEG_clientMsgDataBaseT    cevaMsgDb;

   
   if(dspSource == INU_FUNCTION__EV62_CORE_E)
   {       
       HCG_MNGRG_devoteUnit(fdkDmaCore);
   }
         
   DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, DATA_BASEG_GP_TO_XM4_FDK_MSG + dspSource, 0);
   generalMsgForDsp.msgSize   = cevaMsgDb.msgSize;
   generalMsgForDsp.msgP      = (void*)cevaMsgDb.msgBuf;

   if(privP->paramsP->dspMsgCb)
   {
      privP->paramsP->dspMsgCb(me, &generalMsgForDsp, inputParamsP, dspSource);
   }
#else
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(dspSource);
#endif
}


void inu_fdk__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name        = inu_fdk__name;
      _vtable.node_vtable.ref_vtable.p_dtor        = inu_fdk__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor        = (inu_ref__Ctor*)inu_fdk__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl  = inu_fdk__rxIoctl;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_fdk__rxData;
      _vtable.node_vtable.ref_vtable.p_txDone      = inu_fdk__txDone;

      _vtable.node_vtable.p_newInput     = inu_fdk__newInput;
      _vtable.node_vtable.p_newOutput    = inu_fdk__newOutput;
      _vtable.node_vtable.p_removeInput  = inu_fdk__removeInput;
      _vtable.node_vtable.p_removeOutput = inu_fdk__removeOutput;

      _vtable.p_operate = inu_fdk__operate;
      _vtable.p_start   = inu_fdk__start;
      _vtable.p_stop    = inu_fdk__stop;
      _vtable.p_dspAck  = inu_fdk__dspAck;

       memset(&nullCbsTbl,0,sizeof(nullCbsTbl));

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_fdk__vtable_get(void)
{
   inu_fdk__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

#if DEFSG_IS_GP
static ERRG_codeE inu_fdk__getNodeCbTbl(int nodeId, inu_fdk__initNodeParamsT **cbsRegister)
{
   int i;
   ERRG_codeE ret = INU_FDK__RET_SUCCESS;

   for (i = 0; i < cbsTbl.numUsed; i++)
   {
      if (cbsTbl.cbs[i].nodeId == nodeId)
      {
         *cbsRegister = &cbsTbl.cbs[i];
         return ret;
      }
   }

   if (i == cbsTbl.numUsed)
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "nodeId %d not found in table\n",nodeId);
      *cbsRegister = &nullCbsTbl;
   }
   return ret;
}


ERRG_codeE inu_fdk__initNodeCbFunctions(inu_fdk__initNodeParamsT cbsRegister)
{
   int i;

   if (!_bool_cbsTable_initialized)
   {
      memset(&cbsTbl,0,sizeof(cbsTbl));
      _bool_cbsTable_initialized = true;
   }

   /* check table is not full */
   if (cbsTbl.numUsed == INU_FDK__MAX_NODES_NUMBER)
   {
      printf("init node cb table is full (%d)\n",cbsTbl.numUsed);
      return INU_FDK__ERR_OUT_OF_RSRCS;
   }

   /* check nodeId is not used */
   for (i = 0; i < cbsTbl.numUsed; i++)
   {
      if (cbsTbl.cbs[i].nodeId == cbsRegister.nodeId)
      {
         printf("node id is already used! (entry %d, nodeId %d)\n",i,cbsTbl.cbs[i].nodeId);
         return INU_FDK__ERR_INVALID_ARGS;
      }
   }
   
   memcpy(&cbsTbl.cbs[cbsTbl.numUsed], &cbsRegister, sizeof(cbsRegister));
   printf("inu_fdk__initNodeCbFunctions: entry %d, nodeId %d functions registered\n",cbsTbl.numUsed,cbsTbl.cbs[cbsTbl.numUsed].nodeId);
   cbsTbl.numUsed++;

   return INU_FDK__RET_SUCCESS;
}
#endif

ERRG_codeE inu_fdk__send_data_async(inu_fdkH meH, char *bufP, unsigned int len)
{
   inu_fdk__privData *privP = (inu_fdk__privData*)(((inu_fdk*)meH)->privP);
   inu_data__hdr_t hdr;

   hdr.dataIndex = privP->frameNum;
   hdr.timestamp = 0;
   privP->frameNum++;

   return inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_DATA_SEND_E, &hdr, bufP, len);
}
#if DEFSG_IS_GP
ERRG_codeE inu_fdk__complete(inu_fdkH meH)
{
   return inu_function__complete((inu_function*)meH);
}

ERRG_codeE inu_fdk__doneData(inu_fdkH meH, inu_dataH dataH)
{
   return inu_function__doneData((inu_function*)meH, (inu_data*)dataH);
}

ERRG_codeE inu_fdk__newUserData(inu_fdkH meH, inu_dataH dataH, UINT32 len, inu_dataH *cloneH, inu_data__hdr_t *hdrP)
{
   inu_fdk__privData *privP = (inu_fdk__privData*)(((inu_fdk*)meH)->privP);
   ERRG_codeE ret = INU_FDK__RET_SUCCESS;
   MEM_POOLG_bufDescT *bufDescP = NULL;

   ret = MEM_POOLG_alloc(privP->userPoolH,len,&bufDescP);
   if (ERRG_SUCCEEDED(ret) && bufDescP)
   {
      bufDescP->dataLen = len;
      ret = inu_function__newData((inu_function*)meH, (inu_data*)dataH, bufDescP, hdrP, (inu_data**)cloneH);
   }

   return ret;
}

static ERRG_codeE inu_fdk__userDataReleased (void *arg, MEM_POOLG_bufDescT *bufDescP)
{
   inu_fdk__privData *privP = (inu_fdk__privData*)(((inu_fdk*)arg)->privP);
   if (privP->paramsP->userDataFreeCb)
   {
      privP->paramsP->userDataFreeCb((inu_fdkH)arg,bufDescP->dataP);
   }
   return INU_FDK__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name:  inu_fdk__gpInit
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
ERRG_codeE inu_fdk__gpInit(    int       wdTimeout,
                               UINT32    clientDdrBufSize,
                               UINT32    *clientDdrBufPhyAddrP,
                               UINT32    *clientDdrBufVirtAddrP,
                               inu_device_standalone_mode_e standAloneMode,
                               inu_device_interface_0_e ctrlInterface,
                               inu_deviceH *devicePtr)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   ERRG_codeE ret = INU_FDK__RET_SUCCESS;

   retCode = APP_INITG_init(wdTimeout, clientDdrBufSize, clientDdrBufPhyAddrP, clientDdrBufVirtAddrP,standAloneMode,devicePtr, ctrlInterface,0);

   if(ERRG_FAILED(retCode))
   {
      printf("INU GP lib init failed !!!\n");
      ret = INU_FDK__ERR_NOT_SUPPORTED;
   }

   printf("INU GP lib init done. Ready for operation. \n\n");

   return(ret);
}


/****************************************************************************
*
*  Function Name:  inu_fdk__deinit
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
ERRG_codeE inu_fdk__deinit(inu_deviceH deviceH)
{
   ERRG_codeE ret;
   ret = APP_INITG_close(deviceH);
   return(ret);
}


/****************************************************************************
*
*  Function Name:  inu_fdk__getVersion
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
ERRG_codeE inu_fdk__getVersion(inu_fdk__getVersionT *versionP)
{
   ERRG_codeE retCode = (ERRG_codeE)0;

   // FW version
   versionP->fwVerId.fields.major    = MAJOR_VERSION;
   versionP->fwVerId.fields.minor    = MINOR_VERSION;
   versionP->fwVerId.fields.build    = BUILD_VERSION;
   versionP->fwVerId.fields.subBuild = SUB_BUILD_VERSION;
   // HW version
   retCode = HW_MNGRG_getHwVersion(&versionP->hwVerId);

   return(retCode);
}


/****************************************************************************
*
*  Function Name:  inu_fdk__gpCacheWr
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
ERRG_codeE inu_fdk__gpCacheWr(void *ptr, UINT32 size)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   UINT32 sizeToInvalidate;

   if (size > INU_FDK__MAX_CACHE_SIZE_OP)
      size = INU_FDK__MAX_CACHE_SIZE_OP;

   while(size > 0)
   {
      if (size < INU_FDK__CACHE_SIZE_CHUNK)
         sizeToInvalidate = size;
      else
         sizeToInvalidate = INU_FDK__CACHE_SIZE_CHUNK;
      CMEM_cacheWb(ptr,sizeToInvalidate);

      ptr+=sizeToInvalidate;
      size-=sizeToInvalidate;

      //give some cpu to other operations
      if (size)         
        OS_LYRG_usleep(1);
   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name:  inu_fdk__gpCacheInv
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
ERRG_codeE inu_fdk__gpCacheInv(void *ptr, UINT32 size)
{
   ERRG_codeE retCode = (ERRG_codeE)0;
   UINT32 sizeToInvalidate;

   if (size > INU_FDK__MAX_CACHE_SIZE_OP)
      size = INU_FDK__MAX_CACHE_SIZE_OP;

   while(size > 0)
   {
      if (size < INU_FDK__CACHE_SIZE_CHUNK)
         sizeToInvalidate = size;
      else
         sizeToInvalidate = INU_FDK__CACHE_SIZE_CHUNK;
      CMEM_cacheInv(ptr,sizeToInvalidate);

      ptr+=sizeToInvalidate;
      size-=sizeToInvalidate;

      //give some cpu to other operations
      if (size)         
        OS_LYRG_usleep(1);
   }

   return(retCode);
}

ERRG_codeE inu_fdk__convertPhysicalToVirtual(UINT32 physicalAddr, UINT32 *oVirtualAddrP)
{
   return MEM_MAPG_convertPhysicalToVirtual(physicalAddr, oVirtualAddrP);
}

ERRG_codeE inu_fdk__convertVirtualToPhysical(UINT32 virtualAddr, UINT32 *oPhysicalAddrP)
{
   return MEM_MAPG_convertVirtualToPhysical(virtualAddr, oPhysicalAddrP);
}

ERRG_codeE inu_fdk__getPhysicalAddr(inu_dataH dataH, UINT32 *oPhysicalAddrP)
{
   MEM_POOLG_bufDescT *bufP;
   inu_data__bufDescPtrGet(dataH,&bufP);
   return MEM_POOLG_getDataPhyAddr(bufP,oPhysicalAddrP);
}

/****************************************************************************
*
*  Function Name:  inu_fdk__gpToDspMsgSend
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
ERRG_codeE inu_fdk__gpToDspMsgSend(inu_fdkH fdkH, inu_fdk__dspMsgParamsT *msgParamsP, inu_function__operateParamsT *paramsP, inu_function__coreE coreTarget)
{
   ERRG_codeE                              retCode = (ERRG_codeE)0;
   DATA_BASEG_databaseE                    dataBaseSection = DATA_BASEG_GP_TO_XM4_FDK_MSG + coreTarget;
   DATA_BASEG_clientMsgDataBaseT           cevaMsgDb;
   UINT32                                  msgSize = msgParamsP->msgSize;

   DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, dataBaseSection, 0);
   while(cevaMsgDb.msgReady == 1)
   {
      OS_LYRG_usleep(100);
      DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, dataBaseSection, 0);
   }
   if(msgSize <= INU_FDK__GENERAL_MSG_MAX_SIZE)
   {
     cevaMsgDb.msgReady   = 1;
     cevaMsgDb.msgSize    = msgSize;
     memcpy(cevaMsgDb.msgBuf, msgParamsP->msgP, msgSize);
   }
   else
   {
     LOGG_PRINT(LOG_ERROR_E, NULL, "client msgSize is greater than max size %d>%d\n", msgSize, INU_FDK__GENERAL_MSG_MAX_SIZE);
     retCode = INU_FDK__ERR_OUT_OF_MEM;
   }
   DATA_BASEG_writeDataBase((UINT8*)(&cevaMsgDb), dataBaseSection, 0);
   
   if(coreTarget == INU_FUNCTION__EV62_CORE_E)
   {
        DATA_BASEG_cdnnDataDataBaseT cdnnDataBaseP;
        DATA_BASEG_readDataBase((UINT8*)&cdnnDataBaseP, DATA_BASEG_CDNN_EV, 0);
        fdkDmaCore = cdnnDataBaseP.dmaCopyParams.dmaCore;
        HCG_MNGRG_voteUnit(fdkDmaCore);
   }

   
   retCode = inu_function__sendDspMsg(fdkH, paramsP, DATA_BASEG_ALG_GENERAL_MSG, 0, 0, coreTarget);

   if(!ERRG_SUCCEEDED(retCode))
   {
      if (coreTarget == INU_FUNCTION__EV62_CORE_E)
      {
         HCG_MNGRG_devoteUnit(fdkDmaCore);
      }
   }     
   return(retCode);
}


/****************************************************************************
*
*  Function Name:  inu_fdk__gpToDspMsgSyncSend
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
ERRG_codeE inu_fdk__gpToDspMsgSyncSend(inu_fdkH fdkH, inu_fdk__dspMsgParamsT *msgParamsP, inu_function__coreE coreTarget)
{
   ERRG_codeE                              retCode = (ERRG_codeE)0;
   DATA_BASEG_databaseE                    dataBaseSection = DATA_BASEG_GP_TO_XM4_FDK_MSG + coreTarget;
   DATA_BASEG_clientMsgDataBaseT           cevaMsgDb;
   UINT32                                  msgSize = msgParamsP->msgSize;

   DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, dataBaseSection, 0);
   while(cevaMsgDb.msgReady == 1)
   {
      OS_LYRG_usleep(100);
      DATA_BASEG_readDataBase((UINT8*)&cevaMsgDb, dataBaseSection, 0);
   }
   if(msgSize <= INU_FDK__GENERAL_MSG_MAX_SIZE)
   {
     cevaMsgDb.msgReady   = 1;
     cevaMsgDb.msgSize    = msgSize;
     memcpy(cevaMsgDb.msgBuf, msgParamsP->msgP, msgSize);
   }
   else
   {
     LOGG_PRINT(LOG_ERROR_E, NULL, "client msgSize is greater than max size %d>%d\n", msgSize, INU_FDK__GENERAL_MSG_MAX_SIZE);
     retCode = INU_FDK__ERR_OUT_OF_MEM;
   }
   DATA_BASEG_writeDataBase((UINT8*)(&cevaMsgDb), dataBaseSection, 0);

   retCode = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)fdkH), DATA_BASEG_ALG_GENERAL_MSG, 0, coreTarget);

   return(retCode);
}


#define INU_FDK__FMT_STRING_LEN            (300)
#if DEFSG_IS_GP
void inu_fdk__gpSendLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...)
{
   char fmtStr  [INU_FDK__FMT_STRING_LEN];
   VA_PACK(fmt, fmtStr);
   LOGG_outputStrLog(CLIENT_GP_ID, level, errCode, fileP, funcP, lineNum, fmtStr);
}
#else
void inu_fdk__cevaSendLog(INU_DEFSG_logLevelE level, const char * fmt, ...)
{
   char fmtStr  [INU_FDK__FMT_STRING_LEN];
   VA_PACK(fmt, fmtStr);
   LOGG_PRINT(level, NULL, "%s", fmtStr);
}
#endif

ERRG_codeE inu_fdk__setSensorIoctlFunctionTbl( FDK_SENSOR_DRVG_ioctlListE ioctlNum, inu_fdk__sensorIoctlFuncListT ioctl )
{
   ERRG_codeE ret = INU_FDK__RET_SUCCESS;

   if (ioctlNum < FDK_SENSOR_DRVG_NUM_OF_IOCTLS_E)
   {
      printf("%s: set %d with func %p\n",__FUNCTION__,ioctlNum,ioctl);
      sensorIoctlFuncList[ioctlNum] = ioctl;
   }
   else
   {
      ret = INU_FDK__ERR_INVALID_ARGS;
   }
   return ret;
}

ERRG_codeE inu_fdk__invokeSensorIoctl( FDK_SENSOR_DRVG_ioctlListE ioctlNum, UINT16 groupId, UINT16 sensorId, void *argP )
{
   ERRG_codeE ret = INU_FDK__RET_SUCCESS;

   if ((ioctlNum < FDK_SENSOR_DRVG_NUM_OF_IOCTLS_E) && (sensorIoctlFuncList[ioctlNum]))
   {
      sensorIoctlFuncList[ioctlNum](groupId, sensorId, argP);
   }
   else
   {
      if (ioctlNum >= FDK_SENSOR_DRVG_NUM_OF_IOCTLS_E)
      {
         ret = INU_FDK__ERR_INVALID_ARGS;
      }
   }
   return ret;
}
#endif

