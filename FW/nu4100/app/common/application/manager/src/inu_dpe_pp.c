#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "internal_cmd.h"
#include "err_defs.h"
#include "log.h"
#include "assert.h"

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#endif

#define DPE_PP_DSP_TARGET            ICCG_CMD_TARGET_DSPB
#define DPE_PP_DSP_BLOB_SIZE         DPE_PP_EV_BLOB_SIZE
#define DPE_PP_DSP_BLOB_BUFFER_SIZE  DPE_PP_EV_BLOB_BUFFER_SIZE

typedef struct 
{
#if DEFSG_IS_GP
   UINT32            inu_dpe_ppWorkSpacePhyAddress;
   UINT32            inu_dpe_ppWorkSpaceVirtAddress;
   int               pendingUpdate;
#endif
}inu_dpe_pp__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_DPE_PP";

inu_dpe_pp_hostGpMsgStructT inu_dpe_pp_hostGpMsg;

static const char* inu_dpe_pp__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_dpe_pp__dtor(inu_ref *me)
{
   inu_dpe_pp__privData *privP = (inu_dpe_pp__privData*)((inu_dpe_pp*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(privP);
}


/* Constructor */
ERRG_codeE inu_dpe_pp__ctor(inu_dpe_pp *me, inu_dpe_pp__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_dpe_pp__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_dpe_pp__privData*)malloc(sizeof(inu_dpe_pp__privData));
      if (!privP)
      {
         return INU_DPE_PP__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_dpe_pp__privData));
      me->privP = privP;
   
#if DEFSG_IS_GP
      //initialize to default values
      inu_dpe_pp_hostGpMsg.ConfidenceThresholdRegion1 = 0;
      inu_dpe_pp_hostGpMsg.BlobDispDiffSameBlob = 16;
      inu_dpe_pp_hostGpMsg.BlobMaxHeight = 20;
      inu_dpe_pp_hostGpMsg.BlobMaxSize = 500;
      inu_dpe_pp_hostGpMsg.TemporolRapidTh = 6 * 16;
      inu_dpe_pp_hostGpMsg.TemporolStableTh = 1 * 16;
#endif
   }

   return ret;
}

static int inu_dpe_pp__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   return inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
}

static int inu_dpe_pp__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
#if DEFSG_IS_GP
   inu_dpe_pp__privData *privP = (inu_dpe_pp__privData*)((inu_dpe_pp*)me)->privP;

   switch(msgCode)
   {
      case(INTERNAL_CMDG_DATA_SEND_E):
      {
         //Copy host message 
         memcpy (&inu_dpe_pp_hostGpMsg, dataP, dataLen);
         privP->pendingUpdate = 1;

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

ERRG_codeE inu_dpe_pp__send_data_async(inu_dpe_ppH meH, char *bufP, unsigned int len)
{
   inu_data__hdr_t hdr;

   hdr.timestamp = 0;

   return inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_DATA_SEND_E, &hdr, bufP, len);
}


static ERRG_codeE inu_dpe_pp__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_dpe_pp__privData        *privP = (inu_dpe_pp__privData*)((inu_dpe_pp*)me)->privP;
   DATA_BASEG_dpeDataDataBaseT *dpe_ppDataBaseP;
   UINT32 dspTarget = DPE_PP_DSP_TARGET;
   DATA_BASEG_databaseE dataBase = DATA_BASEG_DPE_PP;
#endif
   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_dpe_pp__start\n");
   }

#if DEFSG_IS_GP
   ret = MEM_MAPG_alloc(DPE_PP_DSP_BLOB_SIZE + DPE_PP_DSP_BLOB_BUFFER_SIZE, 
                        &privP->inu_dpe_ppWorkSpacePhyAddress,
                        &privP->inu_dpe_ppWorkSpaceVirtAddress,
                        0);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for dpe_pp workspace (%d)\n",DPE_PP_DSP_BLOB_SIZE + DPE_PP_DSP_BLOB_BUFFER_SIZE);
      return INU_DPE_PP__ERR_OUT_OF_MEM;
   }

   DATA_BASEG_accessDataBase((UINT8**)&dpe_ppDataBaseP, dataBase);
   dpe_ppDataBaseP->confidence_th = inu_dpe_pp_hostGpMsg.ConfidenceThresholdRegion1;
   dpe_ppDataBaseP->BlobDispDiffSameBlob = inu_dpe_pp_hostGpMsg.BlobDispDiffSameBlob;
   dpe_ppDataBaseP->BlobMaxHeight = inu_dpe_pp_hostGpMsg.BlobMaxHeight;
   dpe_ppDataBaseP->BlobMaxSize = inu_dpe_pp_hostGpMsg.BlobMaxSize;
   #ifdef DISABLED_UNTIL_EV72_CODE_REBUILT
   dpe_ppDataBaseP->TemporolStableTh = inu_dpe_pp_hostGpMsg.TemporolStableTh;
   dpe_ppDataBaseP->TemporolRapidTh = inu_dpe_pp_hostGpMsg.TemporolRapidTh;
   dpe_ppDataBaseP->algType = inu_dpe_pp_hostGpMsg.algType;
   #endif
   dpe_ppDataBaseP->ddrP = (UINT8 *)privP->inu_dpe_ppWorkSpacePhyAddress;
   DATA_BASEG_accessDataBaseRelease(dataBase);
   privP->pendingUpdate = 0;
   
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_DPE_PP,1,dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

static ERRG_codeE inu_dpe_pp__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_dpe_pp__stop\n");
   }
      
#if DEFSG_IS_GP
   inu_dpe_pp__privData        *privP = (inu_dpe_pp__privData*)((inu_dpe_pp*)me)->privP;
   UINT32 dspTarget = DPE_PP_DSP_TARGET;
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_DPE_PP,2, dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }

   MEM_MAPG_free((UINT32*)privP->inu_dpe_ppWorkSpaceVirtAddress);
#endif
   return ret;
}

#if DEFSG_IS_GP
static void inu_dpe_pp__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   ERRG_codeE                  ret;
   int                         i;
   UINT32                      dspTarget;
   inu_function__vtable_get()->p_operate(me, inputParamsP);
   inu_dpe_pp__privData        *privP = (inu_dpe_pp__privData*)((inu_dpe_pp*)me)->privP;
   DATA_BASEG_dpeDataDataBaseT *dpe_ppDataBaseP;
   DATA_BASEG_databaseE dataBase = DATA_BASEG_DPE_PP;

   if (privP->pendingUpdate)
   {
      DATA_BASEG_accessDataBase((UINT8**)&dpe_ppDataBaseP, dataBase);
      dpe_ppDataBaseP->confidence_th = inu_dpe_pp_hostGpMsg.ConfidenceThresholdRegion1;
      dpe_ppDataBaseP->BlobDispDiffSameBlob = inu_dpe_pp_hostGpMsg.BlobDispDiffSameBlob;
      dpe_ppDataBaseP->BlobMaxHeight = inu_dpe_pp_hostGpMsg.BlobMaxHeight;
      dpe_ppDataBaseP->BlobMaxSize = inu_dpe_pp_hostGpMsg.BlobMaxSize;
      #ifdef DISABLED_UNTIL_EV72_CODE_REBUILT
      dpe_ppDataBaseP->TemporolStableTh = inu_dpe_pp_hostGpMsg.TemporolStableTh;
      dpe_ppDataBaseP->TemporolRapidTh = inu_dpe_pp_hostGpMsg.TemporolRapidTh;
     dpe_ppDataBaseP->algType = inu_dpe_pp_hostGpMsg.algType;
      #endif
      dpe_ppDataBaseP->ddrP = (UINT8 *)privP->inu_dpe_ppWorkSpacePhyAddress;
      DATA_BASEG_accessDataBaseRelease(dataBase);
      privP->pendingUpdate = 0;
   }

   dspTarget = DPE_PP_DSP_TARGET;
   ret = inu_function__sendDspMsg(me, inputParamsP, DATA_BASEG_ALG_DPE_PP, 0, 0, dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "error sending dsp message\n");
   }

   if (ERRG_FAILED(ret))
   {
      for (i = 0; i<inputParamsP->dataInputsNum; i++)
      {
         inu_function__doneData(me, inputParamsP->dataInputs[i]);
      }
      inu_function__complete(me);
   }
}

static void inu_dpe_pp__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32 i;
   (void)dspSource;
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"dpe_pp done\n");
   
   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_dpe_pp__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_dpe_pp__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_dpe_pp__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_dpe_pp__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_dpe_pp__rxIoctl;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_dpe_pp__rxData;

      _vtable.p_start   = inu_dpe_pp__start;
      _vtable.p_stop    = inu_dpe_pp__stop;
#if DEFSG_IS_GP
      _vtable.p_dspAck  = inu_dpe_pp__dspAck;
      _vtable.p_operate = inu_dpe_pp__operate;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_dpe_pp__vtable_get(void)
{
   inu_dpe_pp__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

