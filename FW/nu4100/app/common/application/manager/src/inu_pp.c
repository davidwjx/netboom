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

typedef struct 
{
   INU_PP__actionE          actionMap;
#if DEFSG_IS_GP
   MEM_POOLG_handleT poolH;
   MEM_POOLG_handleT secondPoolH;
   UINT32            inu_ppWorkSpacePhyAddress;
   UINT32            inu_ppWorkSpaceVirtAddress;
#endif
}inu_pp__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_PP";

inu_pp__parameter_list_t inu_point_cloud_hostGpMsg;

static const char* inu_pp__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_pp__dtor(inu_ref *me)
{
   inu_pp__privData *privP = (inu_pp__privData*)((inu_pp*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   if (privP->poolH)
   {
      MEM_POOLG_closePool(privP->poolH);
      privP->poolH = NULL;
   }
   if (privP->secondPoolH)
   {
      MEM_POOLG_closePool(privP->secondPoolH);
      privP->secondPoolH = NULL;
   }
#endif
   free(privP);
}


/* Constructor */
ERRG_codeE inu_pp__ctor(inu_pp *me, inu_pp__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_pp__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_pp__privData*)malloc(sizeof(inu_pp__privData));
      if (!privP)
      {
         return INU_PP__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_pp__privData));
      me->privP = privP;
      privP->actionMap = ctorParamsP->actionMap;
   }

   return ret;
}

static int inu_pp__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);

#if DEFSG_IS_GP
   switch(msgCode)
   {
      case(INTERNAL_CMDG_PP_UPDATE_E):
      {
         //Copy host message 
         memcpy (&inu_point_cloud_hostGpMsg, msgP, sizeof(inu_point_cloud_hostGpMsg));

      }break;
      default:
      break;
   }
#endif

   return ret;
}

static int inu_pp__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(msgP);
   FIX_UNUSED_PARAM_WARN(bufDescP);
   FIX_UNUSED_PARAM_WARN(dataP);
   FIX_UNUSED_PARAM_WARN(dataLen);
   FIX_UNUSED_PARAM_WARN(msgCode);
   return 0;
}

#if DEFSG_IS_GP
static UINT32 inu_pp__calcWorkspaceMemSize( inu_data *dataP, UINT32 actionMap )
{
   UINT32 memSize = 32;

   switch(actionMap)
   {
      case  INU_PP_SPLIT:
      case  INU_PP_CROP:
      case  INU_PP_CSC:
      case  INU_PP_REFORMAT:
      case  INU_PP_SCALE:
      case  INU_PP_VERTICAL_BINNING:
      default:
      {
      }
      case  INU_PP_ED2D:
      case  INU_PP_ED2PLY:
      {
         memSize = 64*1024;
      }
      case  INU_PP_DEPTH2PLY:
      {
         memSize = 200*1280*4;
      }
   }
   
   return memSize;
}


static UINT32 inu_pp__calcOutputMemSize( inu_data *dataP, UINT32 actionMap )
{
   UINT32 memSize = 32;
   
   switch(actionMap)
   {
      case  INU_PP_SPLIT:
      case  INU_PP_CROP:
      case  INU_PP_CSC:
      case  INU_PP_REFORMAT:
      default:
      {
      }
      case  INU_PP_SCALE:
      {
         memSize = 1280*800*2;
      }
      case  INU_PP_VERTICAL_BINNING:
      {
         memSize = 3040*760*2;
      }
      case  INU_PP_ED2D:
      {
         memSize = 1280*1080*2;
      }
      case  INU_PP_ED2PLY:
      {
         memSize = POINT_CLOUDG_MAX_BLOB_SIZE;
      }
      case  INU_PP_DEPTH2PLY:
      {
         memSize = POINT_CLOUDG_MAX_BLOB_SIZE;
      }
   }
   
   return memSize;
}
#endif

static ERRG_codeE inu_pp__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_pp__privData        *privP = (inu_pp__privData*)((inu_pp*)me)->privP;
   DATA_BASEG_ppDataDataBaseT *ppDataBaseP;
   UINT32 dspTarget = 0;//TO DO ppDb.dspTarget;
   DATA_BASEG_databaseE dataBase = DATA_BASEG_PP;
   UINT32 memSize;
#endif
   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_pp__start\n");
   }

#if DEFSG_IS_GP
   /* Allocate memory for workspace */
   memSize = inu_pp__calcWorkspaceMemSize(inu_node__getNextInputNode(me,NULL), privP->actionMap);
   ret = MEM_MAPG_alloc(memSize, 
                        &privP->inu_ppWorkSpacePhyAddress,
                        &privP->inu_ppWorkSpaceVirtAddress,
                        0);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for pp workspace action 0x%x (%d)\n",privP->actionMap,memSize);
      return INU_PP__ERR_OUT_OF_MEM;
   }

   /* Allocate memory output result */
   memSize = inu_pp__calcOutputMemSize(inu_node__getNextInputNode(me,NULL), privP->actionMap);
   MEM_POOLG_cfgT cfg;

   cfg.bufferSize = memSize;
   cfg.numBuffers = 2;
   cfg.resetBufPtrInAlloc = 0;
   cfg.freeCb = NULL;
   cfg.freeArg = NULL;
   cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
   cfg.memP = NULL;

   if (privP->poolH == NULL)
   {
      ret = MEM_POOLG_initPool(&privP->poolH, &cfg);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for output action 0x%x (%d)\n",privP->actionMap,memSize);
         return INU_PP__ERR_OUT_OF_MEM;
      }
   }
   
   if (privP->actionMap & (1<<INU_PP_ED2D))
   {
      cfg.bufferSize = inu_pp__calcOutputMemSize(inu_node__getNextInputNode(me,NULL), INU_PP_ED2D);
		if (privP->secondPoolH == NULL)
		{
         ret = MEM_POOLG_initPool(&privP->secondPoolH, &cfg);
         if (ERRG_FAILED(ret))
         {
           LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for output action 0x%x (%d)\n",privP->actionMap,memSize);
           return INU_PP__ERR_OUT_OF_MEM;
         }
      }
   }

   DATA_BASEG_accessDataBase((UINT8**)&ppDataBaseP, dataBase);
   ppDataBaseP->ddrP      = (UINT8*)privP->inu_ppWorkSpacePhyAddress;
   ppDataBaseP->actionMap = privP->actionMap;
   ppDataBaseP->sensorGroup = inu_point_cloud_hostGpMsg.sensorGroup;
   ppDataBaseP->baseline = inu_point_cloud_hostGpMsg.baseline;
   ppDataBaseP->fx = inu_point_cloud_hostGpMsg.fx;
   ppDataBaseP->fy = inu_point_cloud_hostGpMsg.fy;
   ppDataBaseP->cxL = inu_point_cloud_hostGpMsg.cxL;
   ppDataBaseP->cxR = inu_point_cloud_hostGpMsg.cxR;
   ppDataBaseP->cy = inu_point_cloud_hostGpMsg.cy;
   ppDataBaseP->maxDepthMm = inu_point_cloud_hostGpMsg.maxDepthMm;
   ppDataBaseP->voxel_leaf_x_size = inu_point_cloud_hostGpMsg.voxel_leaf_x_size;
   ppDataBaseP->voxel_leaf_y_size = inu_point_cloud_hostGpMsg.voxel_leaf_y_size;
   ppDataBaseP->voxel_leaf_z_size = inu_point_cloud_hostGpMsg.voxel_leaf_z_size;
   ppDataBaseP->vgf_flag = inu_point_cloud_hostGpMsg.vgf_flag;
   ppDataBaseP->flip_x = inu_point_cloud_hostGpMsg.flip_x;
   ppDataBaseP->flip_y = inu_point_cloud_hostGpMsg.flip_y;
   ppDataBaseP->flip_z = inu_point_cloud_hostGpMsg.flip_z;
   ppDataBaseP->model = inu_point_cloud_hostGpMsg.model;
   DATA_BASEG_accessDataBaseRelease(dataBase);
   
   if (privP->actionMap & (1<<INU_PP_DEPTH2PLY))
   {
      dspTarget = ICCG_CMD_TARGET_DSPB;
   }
   
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_PP,1,dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

static ERRG_codeE inu_pp__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_pp__stop\n");
   }
      
#if DEFSG_IS_GP
   inu_pp__privData        *privP = (inu_pp__privData*)((inu_pp*)me)->privP;
   UINT32 dspTarget = 0;//TODO ppDb.dspTarget;
   if (privP->actionMap & (1<<INU_PP_DEPTH2PLY))
   {
      dspTarget = ICCG_CMD_TARGET_DSPB;
   }
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_PP,2, dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }

   MEM_MAPG_free((UINT32*)privP->inu_ppWorkSpaceVirtAddress);
#endif
   return ret;
}

#if DEFSG_IS_GP
UINT64 timeStart, timeStop;
static void inu_pp__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   ERRG_codeE                  ret;
   int                         i, numOuput = 1;
   UINT32                      dspTarget;
   inu_data                    *inu_dataP = NULL;
   MEM_POOLG_bufDescT          *bufDescP = NULL;   
   inu_pp__privData            *privP = (inu_pp__privData*)((inu_pp*)me)->privP;   
   inu_function__vtable_get()->p_operate(me, inputParamsP);

   //if split, check the interleave num
   inu_image__hdr_t *inputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(inputParamsP->dataInputs[0]);
   inu_image__hdr_t *outputImgHdrP;

   INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"inu_pp__operate %d\n", privP->actionMap);
   if (privP->actionMap & (1<<INU_PP_SPLIT))
   {
      if (inputImgHdrP->imgDescriptor.numInterleaveImages < 2 )
      {
         INU_REF__LOGG_PRINT(me,LOG_WARN_E,NULL,"error in graph! activated split on none interleave input\n");
      }

      numOuput = inputImgHdrP->imgDescriptor.numInterleaveImages;
   }

   for (i = 0; i < numOuput; i++)
   {
      inu_dataP = inu_node__getNextOutputNode(me,inu_dataP);
      if (!inu_dataP)
      {
         INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error in graph! numInterleave = %d, but no output is available (%d)\n",numOuput,i);
         assert(0);
         return;         
      }

      /* maintain same timestamp & index */
      outputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(inu_dataP);
      outputImgHdrP->dataHdr = inputImgHdrP->dataHdr;
      
      ret = MEM_POOLG_alloc(privP->poolH, MEM_POOLG_getBufSize(privP->poolH), &bufDescP);
      if (ERRG_SUCCEEDED(ret) && bufDescP)
      {
       if ((privP->actionMap & (1<<INU_PP_ED2PLY)) || (privP->actionMap & (1<<INU_PP_DEPTH2PLY)) )
       {
         bufDescP->dataLen = IMAGE_SIZE_BYTES(inputImgHdrP)*3; //PLY is not image data
       }
       else
       {
         bufDescP->dataLen = IMAGE_SIZE_BYTES(outputImgHdrP);
       }
//         INU_REF__LOGG_PRINT(me,LOG_INFO_E,NULL,"operate on %llu %llu output size %d actionMap 0x%x (input:%dx%d)\n",
//                             outputImgHdrP->dataHdr.dataIndex,outputImgHdrP->dataHdr.timestamp,bufDescP->dataLen, privP->actionMap,
//                             inputImgHdrP->imgDescriptor.stride,inputImgHdrP->imgDescriptor.bufferHeight);
         ret = inu_function__newData(me, inu_dataP, bufDescP, outputImgHdrP, (inu_data**)&inputParamsP->dataInputs[inputParamsP->dataInputsNum++]);
         if (ERRG_FAILED(ret))
         {
            assert(0);
            return;
         }
      }
      else
      {
         INU_REF__LOGG_PRINT(me,LOG_WARN_E,NULL,"1 no buffers for output\n");
         for (i = 0; i<inputParamsP->dataInputsNum; i++)
         {
            inu_function__doneData(me, inputParamsP->dataInputs[i]);
         }
         inu_function__complete(me);
         return;
      }
   }
   
   if (privP->actionMap & (1<<INU_PP_ED2D))
   {

      inu_dataP = inu_node__getNextOutputNode(me,inu_dataP);
      if (!inu_dataP)
      {
         INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error in graph! no output is available\n");
         assert(0);
         return;         
      }

      /* maintain same timestamp & index */
      outputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(inu_dataP);
      outputImgHdrP->dataHdr = inputImgHdrP->dataHdr;
      
     //use second pool
      ret = MEM_POOLG_alloc(privP->secondPoolH, MEM_POOLG_getBufSize(privP->secondPoolH), &bufDescP);
      if (ERRG_SUCCEEDED(ret) && bufDescP)
      {
         bufDescP->dataLen = IMAGE_SIZE_BYTES(outputImgHdrP);
//         INU_REF__LOGG_PRINT(me,LOG_INFO_E,NULL,"operate on %llu %llu output size %d actionMap 0x%x (input:%dx%d)\n",
//                             outputImgHdrP->dataHdr.dataIndex,outputImgHdrP->dataHdr.timestamp,bufDescP->dataLen, privP->actionMap,
//                             inputImgHdrP->imgDescriptor.stride,inputImgHdrP->imgDescriptor.bufferHeight);
         ret = inu_function__newData(me, inu_dataP, bufDescP, outputImgHdrP, (inu_data**)&inputParamsP->dataInputs[inputParamsP->dataInputsNum++]);
         if (ERRG_FAILED(ret))
         {
            assert(0);
            return;
         }
      }
      else
      {
         INU_REF__LOGG_PRINT(me,LOG_WARN_E,NULL,"2 no buffers for output\n");
         for (i = 0; i<inputParamsP->dataInputsNum; i++)
         {
            inu_function__doneData(me, inputParamsP->dataInputs[i]);
         }
         inu_function__complete(me);
         return;
      }
   }
   

   dspTarget = 0;// TODO ppDb.dspTarget;
   if (privP->actionMap & (1<<INU_PP_DEPTH2PLY))
   {
      dspTarget = ICCG_CMD_TARGET_DSPB;
   }
   

   //OS_LYRG_getUsecTime(&timeStart);
   ret = inu_function__sendDspMsg(me, inputParamsP, DATA_BASEG_ALG_PP, 0, privP->actionMap, dspTarget);
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

static void inu_pp__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32 i;
   (void)dspSource;
   inu_pp__privData *privP = (inu_pp__privData*)((inu_pp*)me)->privP;

   //OS_LYRG_getUsecTime(&timeStop);
   //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "time from GP %lld\n", (timeStop - timeStart));
   if (privP->actionMap & (1<<INU_PP_ED2PLY))
   {
       inu_point_cloud_data   *dataP = inputParamsP->dataInputs[1];
       MEM_POOLG_bufDescT     *bufDescP = NULL;
       CMEM_cacheInv((void*)&dataP->point_cloud_dataHdr,sizeof(inu_point_cloud_data__hdr_t));
       INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL, "output size %d\n", dataP->point_cloud_dataHdr.pointCloudListSize);
       //inu_data__bufDescPtrGet((inu_data*)dataP,&bufDescP);
       //bufDescP->dataLen = dataP->point_cloud_dataHdr.pointCloudListSize;
   }
   if (privP->actionMap & (1<<INU_PP_DEPTH2PLY))
   {
       inu_point_cloud_data   *dataP = inputParamsP->dataInputs[1];
       MEM_POOLG_bufDescT     *bufDescP = NULL;
       CMEM_cacheInv((void*)&dataP->point_cloud_dataHdr,sizeof(inu_point_cloud_data__hdr_t));
       inu_data__bufDescPtrGet((inu_data*)dataP,&bufDescP);
       bufDescP->dataLen = dataP->point_cloud_dataHdr.pointCloudListSize;
       if (bufDescP->dataLen > POINT_CLOUDG_MAX_BLOB_SIZE)
       {
           INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "output size %d should be less than %d\n", bufDescP->dataLen, POINT_CLOUDG_MAX_BLOB_SIZE);
       }
   }

   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_pp__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_pp__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_pp__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_pp__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_pp__rxIoctl;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_pp__rxData;

      _vtable.p_start   = inu_pp__start;
      _vtable.p_stop    = inu_pp__stop;
#if DEFSG_IS_GP
      _vtable.p_dspAck  = inu_pp__dspAck;
      _vtable.p_operate = inu_pp__operate;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_pp__vtable_get(void)
{
   inu_pp__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

ERRG_codeE inu_pp__updateParams(inu_ppH meH, inu_pp__parameter_list_t *cfgParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_PP_UPDATE_E,cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

