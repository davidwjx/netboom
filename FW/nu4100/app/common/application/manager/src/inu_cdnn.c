#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "log.h"
#include "assert.h"
#include "internal_cmd.h"

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#include "cde_mngr_new.h"
#include "hcg_mngr.h"
#endif

typedef struct inu_cdnn__privData
{
   MEM_POOLG_handleT poolH;
   inu_cdnn_data     *cdnnDataP;
   inu_load_network_cdnnIdE networkId;
}inu_cdnn__privData;

UINT16 cdnnDmaCore;

#ifdef __UART_ON_FPGA__
#define INU_FPGA_IMAGE_ADDRESS 0x8fe00000 
#endif

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_CDNN";

#define INU_CDNN__NUM_RESULTS_BUFFER (4)

static const char* inu_cdnn__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_cdnn__dtor(inu_ref *me)
{
   inu_cdnn__privData *privP = (inu_cdnn__privData*)((inu_cdnn*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   if (privP->poolH)
   {
      MEM_POOLG_closePool(privP->poolH);
   }
#endif
   free(privP);
}

//#define PIPE_LINE_STATS
#ifdef PIPE_LINE_STATS
#define NUM_OPERATE (8)
UINT64 timeArray[2][NUM_OPERATE];


static void funcStart( inu_data *data )
{
   int i;

   for (i = 0; i < NUM_OPERATE; i++)
   {
      if (timeArray[0][i] == 0)
      {
         OS_LYRG_getUsecTime(&timeArray[0][i]); 
         timeArray[1][i] = (UINT64)data;
         return;
      }
   }
}
static void funcEnd( inu_data *data )
{
   int i;

   for (i = 0; i < NUM_OPERATE; i++)
   {
      if (timeArray[1][i] == ((UINT64)data))
      {
         UINT64 usec;
         OS_LYRG_getUsecTime(&usec);
         printf("%llu: time for %p(%d): %llu\n",usec, data,i, usec - timeArray[0][i]);
         timeArray[0][i] = 0;
         return;
      }
   }   
}
#endif

/* Constructor */
ERRG_codeE inu_cdnn__ctor(inu_cdnn *me, inu_cdnn__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_cdnn__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_cdnn__privData*)malloc(sizeof(inu_cdnn__privData));
      if (!privP)
      {
         return INU_CDNN__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_cdnn__privData));
      me->privP = privP;
#if DEFSG_IS_GP
      privP->networkId = INU_LOAD_NETWORK__CDNN_INVALID_E;
#ifdef PIPE_LINE_STATS
      int k;
      for (k = 0; k < NUM_OPERATE; k++)
      {
         timeArray[0][k] = 0;
      }
#endif
#endif
   }
   return ret;
}

static void inu_cdnn__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
   if (output->ref.refType == INU_CDNN_DATA_REF_TYPE)
   {
      inu_cdnn *cdnn = (inu_cdnn*)me;
      inu_cdnn__privData *privP = (inu_cdnn__privData*)cdnn->privP;
      //save fast_data node for easy access
      privP->cdnnDataP = (inu_cdnn_data*)output;
   }
}


static int inu_cdnn__rxIoctl(inu_ref *ref, void *msgP, int msgCode)
{
   int ret = INU_CDNN__RET_SUCCESS;

   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(ref, msgP, msgCode);

#if DEFSG_IS_GP
   inu_cdnn *me = (inu_cdnn*)ref;
   inu_cdnn__privData *privP = (inu_cdnn__privData*)me->privP;

   switch (msgCode)
   {
      case(INTERNAL_CMDG_CDNN_NETWORK_ID_SET_E):
      {
         inu_cdnn__networkIdSetT *networkIdParams = (inu_cdnn__networkIdSetT*)msgP;
         UINT32 network_id = networkIdParams->network_id;
         if (inu_load_network__get_isNetworkLoaded(ref, network_id))
         {
            privP->networkId = network_id;
            INU_REF__LOGG_PRINT(me, LOG_INFO_E, ret,"Update networkId = %d\n",network_id);
         }
      }
      break;
   }
#endif
   return ret;
}

static void inu_cdnn__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   inu_function__vtable_get()->p_operate(me, inputParamsP);

#if DEFSG_IS_GP
   ERRG_codeE                  ret = INU_CDNN__RET_SUCCESS;
   MEM_POOLG_bufDescT          *bufDescP = NULL;
   inu_data                    *cdnn_data;
   inu_cdnn__privData          *privP = (inu_cdnn__privData *)(((inu_cdnn*)me)->privP);
   inu_cdnn_data__hdr_t        cdnnHdr;
   int                         i;

   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"received input %p\n",privP->cdnnDataP);

   if (privP->networkId >= INU_LOAD_NETWORK__CDNN_LAST_E || privP->networkId < INU_LOAD_NETWORK__CDNN_0_E)
   {
      ret = INU_CDNN__ERR_INVALID_NETWORK_ID;
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = MEM_POOLG_alloc(privP->poolH, MEM_POOLG_getBufSize(privP->poolH), &bufDescP);
      if(ERRG_SUCCEEDED(ret) && bufDescP)
      {
         inu_data__hdr_t *dataHdrP = inu_data__getHdr(inputParamsP->dataInputs[0]);
         //assumption - only one input. if not, need to search input params for the image
         cdnnHdr.dataHdr = *dataHdrP;
         cdnnHdr.network_id = privP->networkId;
         ret = inu_function__newData(me,(inu_data*)privP->cdnnDataP,bufDescP,&cdnnHdr,(inu_data**)&cdnn_data);
         if (ERRG_FAILED(ret))
         {
            assert(0);
            return;
         }
   #ifdef PIPE_LINE_STATS
         funcStart(inputParamsP->dataInputs[0]);
   #endif
         inputParamsP->dataInputs[inputParamsP->dataInputsNum] = cdnn_data;
         inputParamsP->dataInputsNum++;
         HCG_MNGRG_voteUnit(HCG_MNGRG_DMA0 + cdnnDmaCore);
//         INU_REF__LOGG_PRINT(me, LOG_INFO_E,NULL,"GP send job to CNN %d\n", cdnnHdr.dataHdr.dataIndex);
         ret = inu_function__sendDspMsg(me,inputParamsP, DATA_BASEG_ALG_CDNN, 0, 0, ICCG_CMD_TARGET_DSPB);
         if (ERRG_FAILED(ret))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
         }
      }
      else
      {
         INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"mem alloc failed\n");
      }
   }

   if(ERRG_FAILED(ret))
   {
      for(i=0;i<inputParamsP->dataInputsNum;i++)
      {
         inu_function__doneData(me, inputParamsP->dataInputs[i]);
      }
      inu_function__complete(me);
   }
#endif
}


static ERRG_codeE inu_cdnn__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;

   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_cdnn__start\n");
   }

#if DEFSG_IS_GP
   DATA_BASEG_cdnnDataDataBaseT cdnnDataBaseP;

   inu_cdnn__privData *privP = (inu_cdnn__privData*)((inu_cdnn*)me)->privP;

   if (privP->poolH == NULL)
   {
      MEM_POOLG_cfgT cfg;
   
      cfg.bufferSize = CDNNG_MAX_BLOB_SIZE;
      cfg.numBuffers = INU_CDNN__NUM_RESULTS_BUFFER;
      cfg.resetBufPtrInAlloc = 0;
      cfg.freeCb = NULL;
      cfg.freeArg = NULL;
      cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
      cfg.memP = NULL;
   
      ret = MEM_POOLG_initPool(&privP->poolH, &cfg);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"pool init failed. ret = %x \n", ret);
      }
   }

   DATA_BASEG_readDataBase((UINT8*)&cdnnDataBaseP, DATA_BASEG_CDNN_EV, 0);
   cdnnDmaCore = cdnnDataBaseP.dmaCopyParams.dmaCore;
   INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"cdnnDmaCore %d\n", cdnnDmaCore);

   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_CDNN,1,ICCG_CMD_TARGET_DSPB);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

static ERRG_codeE inu_cdnn__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_cdnn__stop\n");
   }

#if DEFSG_IS_GP
   DATA_BASEG_cdnnDataDataBaseT *cdnnDataBaseP;
   inu_cdnn__privData           *privP = (inu_cdnn__privData *)(((inu_cdnn*)me)->privP);
   DATA_BASEG_accessDataBase((UINT8**)&cdnnDataBaseP, DATA_BASEG_CDNN_EV);
   cdnnDataBaseP->networkId = privP->networkId;
   DATA_BASEG_accessDataBaseRelease(DATA_BASEG_CDNN_EV);
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_CDNN,2, ICCG_CMD_TARGET_DSPB);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

#if DEFSG_IS_GP
static void inu_cdnn__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32                  i;
   inu_image               *imageP = inputParamsP->dataInputs[0];
   UINT16                  dataInputsNum = inputParamsP->dataInputsNum;
   inu_cdnn_data           *dataP = inputParamsP->dataInputs[dataInputsNum-1];
   MEM_POOLG_bufDescT      *bufDescP = NULL;
   FIX_UNUSED_PARAM_WARN(dspSource);
#ifdef PIPE_LINE_STATS
   funcEnd(inputParamsP->dataInputs[0]);
   static UINT64 Prevusec = 0;
#endif
   HCG_MNGRG_devoteUnit(HCG_MNGRG_DMA0 + cdnnDmaCore);
   CMEM_cacheInv((void*)&dataP->cdnn_dataHdr,sizeof(inu_cdnn_data__hdr_t));
   inu_data__bufDescPtrGet((inu_data*)dataP,&bufDescP);
   bufDescP->dataLen = dataP->cdnn_dataHdr.totOutputSize;

   dataP->cdnn_dataHdr.frameId = imageP->imageHdr.dataHdr.dataIndex;

#ifdef PIPE_LINE_STATS
    UINT64 usec;
    OS_LYRG_getUsecTime(&usec);
    INU_REF__LOGG_PRINT(me, LOG_INFO_E,NULL,"Time from prev result=%llu\n",  (usec - Prevusec));
    Prevusec = usec ; 
    printf("%llu: time for %p(%d): %llu\n",usec, dataP,i, usec - timeArray[0][i]);
#endif
//   INU_REF__LOGG_PRINT(me, LOG_INFO_E,NULL,"GP Received ACK from EV frame ID %d\n",  dataP->cdnn_dataHdr.frameId);
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"out totsize %d %d %d %d frame ID %d\n", dataP->cdnn_dataHdr.totOutputSize, dataP->cdnn_dataHdr.numOfTails, dataP->cdnn_dataHdr.tailHeader[0].elementCount, dataP->cdnn_dataHdr.tailHeader[0].elementSize, dataP->cdnn_dataHdr.frameId);
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"image params %d %d\n", imageP->imageHdr.imgDescriptor.width, imageP->imageHdr.imgDescriptor.height);
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"cdnn done %d\n",bufDescP->dataLen);
   
   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_cdnn__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_cdnn__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_cdnn__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_cdnn__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_cdnn__rxIoctl;

     _vtable.node_vtable.p_newOutput = inu_cdnn__newOutput;

      _vtable.p_operate = inu_cdnn__operate;
      _vtable.p_start   = inu_cdnn__start;
      _vtable.p_stop    = inu_cdnn__stop;
#if DEFSG_IS_GP
      _vtable.p_dspAck  = inu_cdnn__dspAck;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_cdnn__vtable_get(void)
{
   inu_cdnn__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

ERRG_codeE inu_cdnn__setNetworkId(inu_cdnnH meH, inu_cdnn__networkIdSetT *networkIdParamP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_CDNN_NETWORK_ID_SET_E, networkIdParamP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

