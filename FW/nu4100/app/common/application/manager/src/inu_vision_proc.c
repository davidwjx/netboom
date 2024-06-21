#ifdef VISION_PROC
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
#include "low_light_isp.h"
#endif

#define GAMMA_LUT_LENGTH_BYTES         ((1 << (CAMERA_BIT_DEPTH)) * sizeof(UINT8))
#define GAIN_CURVE_LENGTH_BYTES        ((1 << (NET_BIT_DEPTH)) * sizeof(UINT16))
#define BACKGROUND_IMAGE_IN_ZIP (1)
#define BACKGROUND_IMAGE_FILE_SIZE (4147200)
#define BACKGROUND_IMAGE_BINARY_FILE_NAME   "/media/inuitive/bg_image.bin"

typedef struct inu_vision_proc__privData
{
   MEM_POOLG_handleT imagePoolH;
   int outputSize;
   int pendingUpdate;
#if DEFSG_IS_GP
   UINT32 inu_visionProcBgVirtAddr;
   UINT32 inu_visionProcBgPhAddr;
   CDE_MNGRG_channnelInfoT *dmaChanInfoP;
#endif
}inu_vision_proc__privData;

extern UINT16 cdnnDmaCore;

static VISION_PROCG_lowLightIspCalcT inu_vision_proc_lowLightIspCalc;
static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_VISION_PROC";

inu_vision_proc__parameter_list_t inu_vision_proc_hostGpMsg;

static const char* inu_vision_proc__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_vision_proc__dtor(inu_ref *me)
{
   inu_vision_proc__privData *privP = (inu_vision_proc__privData*)((inu_vision_proc*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   if (privP->imagePoolH)
   {
      MEM_POOLG_closePool(privP->imagePoolH);
      privP->imagePoolH = NULL;
   }
#endif
   free(privP);
}

/* Constructor */
ERRG_codeE inu_vision_proc__ctor(inu_vision_proc *me, inu_vision_proc__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_vision_proc__privData *privP;

   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_vision_proc__privData*)malloc(sizeof(inu_vision_proc__privData));
      if (!privP)
      {
         return INU_VISION_PROC__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_vision_proc__privData));
      me->privP = privP;

#if DEFSG_IS_GP
      // initialize defaults
      DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP;

      DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);
      visionProcDataBaseP->algType = VISION_PROC_NO_ALG;
      visionProcDataBaseP->bokehParams.blurLevel = 0.8f;
      visionProcDataBaseP->bokehParams.visionProcResolution = VISION_PROC_1080P;
      visionProcDataBaseP->bokehParams.faceBoxDraw = false;
      DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);
#endif
   }
   return ret;
}

static int inu_vision_proc__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
   inu_vision_proc__privData *privP = (inu_vision_proc__privData*)((inu_vision_proc*)me)->privP;

#if DEFSG_IS_GP
   switch(msgCode)
   {
      case(INTERNAL_CMDG_VISION_PROC_UPDATE_E):
      {
         //Copy host message
         memcpy (&inu_vision_proc_hostGpMsg, msgP, sizeof(inu_vision_proc_hostGpMsg));
         privP->pendingUpdate = 1;

      }break;

      default:
      break;
   }
#endif

   return ret;
}

static int inu_vision_proc__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
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
static int inu_vision_proc__getOutputSize(inu_function *me)
{
   int size = 0;

   inu_nodeH  currentH;

   currentH = inu_node__getNextOutputNode(me, NULL);
   while (currentH)
   {
       if (inu_ref__instanceOf(currentH, INU_IMAGE_REF_TYPE)||inu_ref__instanceOf(currentH, INU_CDNN_DATA_REF_TYPE))
       {
           break;
       }
       currentH = inu_node__getNextOutputNode(currentH, NULL);
   }
   if (currentH)
   {
      inu_image__hdr_t *outputImgHdrP = (inu_image__hdr_t*)inu_data__getHdr(currentH);
      size = IMAGE_SIZE_BYTES(outputImgHdrP);
      if (inu_ref__instanceOf(currentH, INU_CDNN_DATA_REF_TYPE))
      {
         size = CDNNG_MAX_BLOB_SIZE;
      }
      INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"OUTPUT SIZE %d %d %d\n", outputImgHdrP->imgDescriptor.stride, outputImgHdrP->imgDescriptor.bufferHeight, outputImgHdrP->imgDescriptor.bitsPerPixel);
   }
   return size;
}

static void inu_vision_proc__getOutputImageDesc(DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP, inu_image__descriptor *outputImageDesc)
{
   VISION_PROCG_visionProcAlgTypeE algType = visionProcDataBaseP->algType;
   VISION_PROCG_visionProcResolutionE res;

   switch (algType)
   {
      case  VISION_PROC_BOKEH:
      case  VISION_PROC_NO_ALG:
      {
         res = visionProcDataBaseP->bokehParams.visionProcResolution;
         bool depthOutput = visionProcDataBaseP->bokehParams.depthOutput;
         VISION_PROCG_visionProcCameraE camera = visionProcDataBaseP->bokehParams.visionProcCamera;
         switch(res)
         {
         case VISION_PROC_1080P:
         {
            outputImageDesc->stride= 1920;
            outputImageDesc->bufferHeight = 1080;
         }break;
         case VISION_PROC_4K:
         {
            outputImageDesc->stride = 3840;
            outputImageDesc->bufferHeight = 2160;
         }break;
         default:
         {
            outputImageDesc->stride = 0;
            outputImageDesc->bufferHeight = 0;
         }break;
         }
         outputImageDesc->bitsPerPixel = 16;
         outputImageDesc->format = NUCFG_FORMAT_YUV422_8BIT_E;
         if (depthOutput)
         {
            if (camera == VISION_PROC_SINGLE)
            {
               outputImageDesc->stride= 640;
               outputImageDesc->bufferHeight = 360;
            }
            else //camera == VISION_PROC_DUAL
            {
               outputImageDesc->stride= 960;
               outputImageDesc->bufferHeight = 540;
            }
            outputImageDesc->bitsPerPixel = 16;
            outputImageDesc->format = NUCFG_FORMAT_DISPARITY_E;
         }
      }break;
      case  VISION_PROC_BACKGROUND_REMOVAL:
      {
         res = visionProcDataBaseP->backgroundRemovalParams.visionProcResolution;
         switch(res)
         {
         case VISION_PROC_1080P:
         {
            outputImageDesc->stride = 1920;
            outputImageDesc->bufferHeight = 1080;
         }break;
         case VISION_PROC_4K:
         {
            outputImageDesc->stride = 3840;
            outputImageDesc->bufferHeight = 2160;
         }break;
         default:
         {
            outputImageDesc->stride = 0;
            outputImageDesc->bufferHeight = 0;
         }break;
         }
         outputImageDesc->bitsPerPixel = 16;
         outputImageDesc->format = NUCFG_FORMAT_YUV422_8BIT_E;
      }break;
      case  VISION_PROC_AUTO_FRAMING:
      {
         res = visionProcDataBaseP->autoFramingParams.visionProcResolution;
         switch(res)
         {
         case VISION_PROC_1080P:
         {
            outputImageDesc->stride = 1920;
            outputImageDesc->bufferHeight = 1080;
         }break;
         case VISION_PROC_4K:
         {
            outputImageDesc->stride = 3840;
            outputImageDesc->bufferHeight = 2160;
         }break;
         default:
         {
            outputImageDesc->stride = 0;
            outputImageDesc->bufferHeight = 0;
         }break;
         }
         outputImageDesc->bitsPerPixel = 24;
      }break;
      case VISION_PROC_LOW_LIGHT_ISP:
      {
         res = visionProcDataBaseP->lowLightIspCalc.visionProcResolution;
         switch(res)
         {
         case VISION_PROC_1080P:
         {
            outputImageDesc->stride = 1600;
            outputImageDesc->bufferHeight = 904;
         }break;
         case VISION_PROC_4K:
         {
            outputImageDesc->stride = 3264;
            outputImageDesc->bufferHeight = 2448;
         }break;
         default:
         {
            outputImageDesc->stride = 0;
            outputImageDesc->bufferHeight = 0;
         }break;
         }
         outputImageDesc->bitsPerPixel = 16;
         outputImageDesc->format = NUCFG_FORMAT_YUV422_8BIT_E;
      }break;
      default:
      {
         outputImageDesc->stride = 0;
         outputImageDesc->bufferHeight = 0;
         outputImageDesc->bitsPerPixel = 0;
      }break;
   }
}

static void inu_vision_proc__setDisableDspBokeh(inu_vision_procH meH)
{
   INU_REF__LOGG_PRINT(meH,LOG_INFO_E,NULL,"reset DSP bokeh process \n");

   inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)meH),DATA_BASEG_ALG_VISION_PROC,2, ICCG_CMD_TARGET_DSPB);
   OS_LYRG_usleep(1000);
   inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)meH),DATA_BASEG_ALG_VISION_PROC,1, ICCG_CMD_TARGET_DSPB);
}

static void inu_vision_proc_updateDatabase(inu_vision_procH meH, DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP)
{
   visionProcDataBaseP->algType = inu_vision_proc_hostGpMsg.algType;

   switch(visionProcDataBaseP->algType)
   {
      case(VISION_PROC_BOKEH):
      case(VISION_PROC_NO_ALG):
      {
         visionProcDataBaseP->bokehParams.multiFaceMode= inu_vision_proc_hostGpMsg.bokehParams.multiFaceMode;
         visionProcDataBaseP->bokehParams.noFaceFramesToKeepBlur= inu_vision_proc_hostGpMsg.bokehParams.noFaceFramesToKeepBlur;
         visionProcDataBaseP->bokehParams.faceSmoothFactor= inu_vision_proc_hostGpMsg.bokehParams.faceSmoothFactor;
         visionProcDataBaseP->bokehParams.faceBoxDraw= inu_vision_proc_hostGpMsg.bokehParams.faceBoxDraw;
         visionProcDataBaseP->bokehParams.depthOutput= inu_vision_proc_hostGpMsg.bokehParams.depthOutput;
         visionProcDataBaseP->bokehParams.blurLevel = inu_vision_proc_hostGpMsg.bokehParams.blurLevel;
         visionProcDataBaseP->bokehParams.visionProcCamera = inu_vision_proc_hostGpMsg.bokehParams.visionProcCamera;
         visionProcDataBaseP->bokehParams.visionProcResolution = inu_vision_proc_hostGpMsg.bokehParams.visionProcResolution;
      }break;

      case(VISION_PROC_BACKGROUND_REMOVAL):
      {
         if (inu_vision_proc_hostGpMsg.backgroundRemovalParams.backgroundImage == 0)//check if disabled
         {
            visionProcDataBaseP->backgroundRemovalParams.backgroundImage = 0;
            visionProcDataBaseP->handle = 1;
         }
         visionProcDataBaseP->backgroundRemovalParams.visionProcResolution = inu_vision_proc_hostGpMsg.backgroundRemovalParams.visionProcResolution;
      }break;

      case(VISION_PROC_LOW_LIGHT_ISP):
      {
      }break;

      case(VISION_PROC_AUTO_FRAMING):
      {
         visionProcDataBaseP->autoFramingParams.multiplePersonDetection= inu_vision_proc_hostGpMsg.autoFramingParams.multiplePersonDetection;
         visionProcDataBaseP->autoFramingParams.trackingSpeed= inu_vision_proc_hostGpMsg.autoFramingParams.trackingSpeed;
         visionProcDataBaseP->autoFramingParams.maxZoom= inu_vision_proc_hostGpMsg.autoFramingParams.maxZoom;
         visionProcDataBaseP->autoFramingParams.visionProcResolution = inu_vision_proc_hostGpMsg.autoFramingParams.visionProcResolution;
      }break;

      default:
      break;
   }
}
#endif

static void inu_vision_proc__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   inu_function__vtable_get()->p_operate(me, inputParamsP);

#if DEFSG_IS_GP
   ERRG_codeE                  ret = INU_VISION_PROC__RET_SUCCESS;
   MEM_POOLG_bufDescT          *bufDescP = NULL;
   DATA_BASEG_visionProcDataDataBaseT  *visionProcDataBaseP;
   inu_vision_proc__privData *privP = (inu_vision_proc__privData*)((inu_vision_proc*)me)->privP;
   inu_nodeH  currentH = NULL;
   int i, algType;
   UINT32 bufDescSrcP ;
   UINT32 bufDescDstP ;
   inu_image__hdr_t *inputImgHdrP = NULL;
   inu_image__hdr_t *outputImgHdrP = NULL;

   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"received input\n");

   INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"OPERATE free buf num %d\n", MEM_POOLG_getNumFree(privP->imagePoolH));
   ret = MEM_POOLG_alloc(privP->imagePoolH, MEM_POOLG_getBufSize(privP->imagePoolH), &bufDescP);
   if(ERRG_SUCCEEDED(ret) && bufDescP)
   {
      //create output image data
      currentH = inu_node__getNextOutputNode(me,currentH);
      while (currentH)
      {
         if (inu_ref__instanceOf(currentH, INU_IMAGE_REF_TYPE)||inu_ref__instanceOf(currentH, INU_CDNN_DATA_REF_TYPE))
         {
            break;
         }
         currentH = inu_node__getNextOutputNode(me,currentH);
      }
      if (currentH)
      {
         inputImgHdrP  = inu_data__getHdr(inputParamsP->dataInputs[0]);
         outputImgHdrP = inu_data__getHdr((inu_dataH)currentH);

         outputImgHdrP->dataHdr.timestamp = inputImgHdrP->dataHdr.timestamp;
         outputImgHdrP->dataHdr.dataIndex= inputImgHdrP->dataHdr.dataIndex;

         ret = inu_function__newData(me, (inu_data*)currentH, bufDescP, outputImgHdrP, (inu_data**)&inputParamsP->dataInputs[inputParamsP->dataInputsNum++]);
         if (ERRG_FAILED(ret))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E, NULL, "failed to allocate new IMAGE data\n");
            assert(0);
            return;
         }
      }
   }
   else
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,ret,"image data mem alloc failed %d\n", ERRG_GET_ERR_CODE(ret));
   }
   if(ERRG_SUCCEEDED(ret))
   {
      VISION_PROCG_visionProcAlgTypeE previousAlg;

      DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);
      previousAlg = visionProcDataBaseP->algType;
      if (privP->pendingUpdate)
      {
         inu_vision_proc_updateDatabase(me, visionProcDataBaseP);
         privP->pendingUpdate = 0;
      }

      algType = visionProcDataBaseP->algType;

      visionProcDataBaseP->networkResultsBlockSize = 0;
      if(inu_ref__instanceOf(currentH, INU_CDNN_DATA_REF_TYPE))
      {
         visionProcDataBaseP->networkResultsBlockSize = CDNNG_MAX_BLOB_SIZE;
      }
      DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);

      if (algType == VISION_PROC_NO_ALG)
      {
         //in case we stop bokeh, we have to send to dsp stop and start commands
         if (previousAlg == VISION_PROC_BOKEH)
         {
            inu_vision_proc__setDisableDspBokeh(me);
         }

         //in case  we asked just streaming without any alg, we just copy with DMA from inputp[0] to output.
         //we choosed mem copy since the kernelSpc allocation for UVC, vision_proc already alloc from there
         UINT32 dstPhys,srcPhys;

         UINT32 dataSize = inu_data__getMemSize(inputParamsP->dataInputs[0]);
         MEM_POOLG_bufDescT *bufDescP;

         //src
         inu_data__bufDescPtrGet(inputParamsP->dataInputs[0], &bufDescP);
         ret = MEM_POOLG_getDataPhyAddr(bufDescP,&srcPhys);

         //dst
         inu_data__bufDescPtrGet(inputParamsP->dataInputs[(inputParamsP->dataInputsNum)-1], &bufDescP);
         ret = MEM_POOLG_getDataPhyAddr(bufDescP,&dstPhys);

         ret = CDE_MNGRG_memcpyBlock(privP->dmaChanInfoP, dstPhys, srcPhys, dataSize );
         if (ERRG_FAILED(ret))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E,ret,"fail to copy data\n");
         }

         bufDescP->dataLen = IMAGE_SIZE_BYTES(outputImgHdrP);
         for(i = 0; i < inputParamsP->dataInputsNum; i++)
         {
            inu_function__doneData(me, inputParamsP->dataInputs[i]);
         }
         inu_function__complete(me);
      }
      else//asked alg:BOKEH and other (low light isp etc..)
      {
         HCG_MNGRG_voteUnit(HCG_MNGRG_DMA0 + cdnnDmaCore);
         ret = inu_function__sendDspMsg(me,inputParamsP, DATA_BASEG_ALG_VISION_PROC, 0, 0, ICCG_CMD_TARGET_DSPB);
         if (ERRG_FAILED(ret))
         {
            INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
         }

         switch(visionProcDataBaseP->algType)
         {
            case(VISION_PROC_LOW_LIGHT_ISP):
            {
               UINT16    *inDataP = (UINT16*)inu_data__getMemPtr(inputParamsP->dataInputs[0]);
               UINT32    size = inu_data__getMemSize(inputParamsP->dataInputs[0]);
               inu_nodeH sensorGroupP;
               IO_HANDLE sensorHandle;

               inu_graph__getOrigSensGroup(me, &sensorGroupP);
               inu_sensors_group__getSensorHandle(sensorGroupP,&sensorHandle);
               CMEM_cacheInv(inDataP, size);
               LOW_LIGHT_ISPG_calculate(inDataP, &inu_vision_proc_lowLightIspCalc, sensorHandle);
               CMEM_cacheWb(inu_vision_proc_lowLightIspCalc.gainCurveVirtAddr, GAIN_CURVE_LENGTH_BYTES);

               DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);
               memcpy(&visionProcDataBaseP->lowLightIspCalc, &inu_vision_proc_lowLightIspCalc, sizeof(VISION_PROCG_lowLightIspCalcT));
               DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);
            }break;
         }
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


static ERRG_codeE inu_vision_proc__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;

   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_vision_proc__start\n");
   }

#if DEFSG_IS_GP
   DATA_BASEG_cdnnDataDataBaseT cdnnDataBaseP;
   DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP;

   DATA_BASEG_readDataBase((UINT8*)&cdnnDataBaseP, DATA_BASEG_CDNN_EV, 0);
   cdnnDmaCore = cdnnDataBaseP.dmaCopyParams.dmaCore;
   INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"cdnnDmaCore %d\n", cdnnDmaCore);

   DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);

   MEM_POOLG_cfgT cfg;
   inu_vision_proc__privData *privP = (inu_vision_proc__privData*)((inu_vision_proc*)me)->privP;

   cfg.numBuffers = 8;//TBD: num buffers?
   cfg.resetBufPtrInAlloc = 0;
   cfg.freeCb = NULL;
   cfg.freeArg = NULL;
   // bypass for HP new bokeh graph - need to allocate this channel (UVC) from kernel space pool
   cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
   cfg.memP = NULL;

   //init pool for image data
   cfg.bufferSize = inu_vision_proc__getOutputSize(me);
   privP->outputSize = cfg.bufferSize;
   INU_REF__LOGG_PRINT(me,LOG_INFO_E,NULL,"cfg.bufferSize %d\n", cfg.bufferSize);
   if (privP->imagePoolH == NULL)
   {
      ret = MEM_POOLG_initPool(&privP->imagePoolH, &cfg);

      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, 0,  "Failed to init memory for image output (%d)\n", cfg.bufferSize);
         return INU_VISION_PROC__ERR_OUT_OF_MEM;
      }
   }

   void *idsrVirtAddr = NULL;
   void * idsrPhAddr = NULL;

   ret = inu_device__getIdsrLutAddress(&idsrVirtAddr, &idsrPhAddr);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"Can't find Idsr Lut\n");
   }
   else
   {
      INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"Idsr Lut virt 0x%x phys 0x%x\n", (UINT32)idsrVirtAddr, (UINT32)idsrPhAddr);
      visionProcDataBaseP->idsrLutAddr = (UINT32)idsrPhAddr;
   }

   inu_vision_proc_updateDatabase(me, visionProcDataBaseP);

#if BACKGROUND_IMAGE_IN_ZIP
   if (visionProcDataBaseP->algType == VISION_PROC_BACKGROUND_REMOVAL && visionProcDataBaseP->handle != 1)
   {
      FILE *bgFile;
      UINT32 size;

      //open background image file
      bgFile = fopen(BACKGROUND_IMAGE_BINARY_FILE_NAME, "rb");
      if(!bgFile)
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"Can't find background image file\n");
         //return INU_VISION_PROC__ERR_IO_ERROR;
         visionProcDataBaseP->backgroundRemovalParams.backgroundImage = 0;
      }
      else
      {
         ret = MEM_MAPG_alloc(BACKGROUND_IMAGE_FILE_SIZE,&privP->inu_visionProcBgPhAddr,&privP->inu_visionProcBgVirtAddr,1);
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for background image\n");
            return INU_VISION_PROC__ERR_OUT_OF_MEM;
         }
         LOGG_PRINT(LOG_INFO_E,NULL,"background image PhAddr 0x%x\n", privP->inu_visionProcBgPhAddr);

         size = fread( (void*)(privP->inu_visionProcBgVirtAddr), 1, BACKGROUND_IMAGE_FILE_SIZE, bgFile);
         if(!size)
         {
            LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading background image File\n");
            return INU_VISION_PROC__ERR_IO_ERROR;
         }
         CMEM_cacheWb((void*)(privP->inu_visionProcBgVirtAddr), BACKGROUND_IMAGE_FILE_SIZE);

         fclose(bgFile);

         visionProcDataBaseP->backgroundRemovalParams.backgroundImage = privP->inu_visionProcBgPhAddr;
      }
   }
#endif

   privP->pendingUpdate = 0;

   DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);

   if (visionProcDataBaseP->algType == VISION_PROC_LOW_LIGHT_ISP)
   {
      inu_image__descriptor outputImageDesc;

         inu_vision_proc_lowLightIspCalc.visionProcResolution = inu_vision_proc_hostGpMsg.lowLightIspParams.visionProcResolution;
         inu_vision_proc__getOutputImageDesc(visionProcDataBaseP, &outputImageDesc);
         int stride = inu_vision_proc_hostGpMsg.lowLightIspParams.stride;

      ret = MEM_MAPG_alloc(GAMMA_LUT_LENGTH_BYTES, (UINT32*)&inu_vision_proc_lowLightIspCalc.gammaLutPhysAddr, (UINT32*)&inu_vision_proc_lowLightIspCalc.gammaLutVirtAddr, 1);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, 0, "ERROR: Failed to init memory for gama LUT\n");
         return ret;
      }
      inu_vision_proc_lowLightIspCalc.gammaLutSize = GAMMA_LUT_LENGTH_BYTES;

      ret = MEM_MAPG_alloc(GAIN_CURVE_LENGTH_BYTES, (UINT32*)&inu_vision_proc_lowLightIspCalc.gainCurvePhysAddr, (UINT32*)&inu_vision_proc_lowLightIspCalc.gainCurveVirtAddr, 1);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, 0, "ERROR: Failed to init memory for gain curve\n");
         return ret;
      }
      inu_vision_proc_lowLightIspCalc.gainCurveSize = GAIN_CURVE_LENGTH_BYTES;

      LOW_LIGHT_ISPG_init(outputImageDesc.stride, outputImageDesc.bufferHeight, stride, &inu_vision_proc_lowLightIspCalc);
      CMEM_cacheWb(inu_vision_proc_lowLightIspCalc.gammaLutVirtAddr, GAMMA_LUT_LENGTH_BYTES);
      memcpy(&visionProcDataBaseP->lowLightIspCalc, &inu_vision_proc_lowLightIspCalc, sizeof(VISION_PROCG_lowLightIspCalcT));
   }

   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_VISION_PROC,1,ICCG_CMD_TARGET_DSPB);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
   ret = CDE_MNGRG_memcpyOpenChan(&privP->dmaChanInfoP, cfg.bufferSize, NULL, me);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error open DMA channel\n");
   }
#endif
   return ret;
}

static ERRG_codeE inu_vision_proc__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_vision_proc__stop\n");
   }

#if DEFSG_IS_GP
   inu_vision_proc__privData          *privP = (inu_vision_proc__privData*)((inu_vision_proc*)me)->privP;
   DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP;

   DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);
   switch(visionProcDataBaseP->algType)
   {
      case(VISION_PROC_LOW_LIGHT_ISP):
      {
         VISION_PROCG_lowLightIspCalcT* lowLightIspCalc = &visionProcDataBaseP->lowLightIspCalc;

         MEM_MAPG_free((UINT32*)lowLightIspCalc->gammaLutVirtAddr);
         MEM_MAPG_free((UINT32*)lowLightIspCalc->gainCurveVirtAddr);

         LOW_LIGHT_ISPG_end();
      }break;

      case(VISION_PROC_BACKGROUND_REMOVAL):
      {
#if BACKGROUND_IMAGE_IN_ZIP
         MEM_MAPG_free((UINT32*)privP->inu_visionProcBgVirtAddr);
#endif
      }break;
   }
   DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);

   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_VISION_PROC,2, ICCG_CMD_TARGET_DSPB);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

#if DEFSG_IS_GP
static void inu_vision_proc__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32                             i;
   UINT32                             dataInputsNum = inputParamsP->dataInputsNum;
   inu_image                          *dataP = inputParamsP->dataInputs[dataInputsNum-1];
   MEM_POOLG_bufDescT                 *bufDescP = NULL;
   inu_vision_proc__privData          *privP = (inu_vision_proc__privData*)((inu_vision_proc*)me)->privP;
   DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP;
   inu_image__descriptor              *outputImageDesc = &(dataP->imageHdr.imgDescriptor);
   inu_nodeH  currentH;
   FIX_UNUSED_PARAM_WARN(dspSource);
   HCG_MNGRG_devoteUnit(HCG_MNGRG_DMA0 + cdnnDmaCore);
   inu_data__bufDescPtrGet((inu_data*)dataP,&bufDescP);

   DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);

   inu_vision_proc__getOutputImageDesc(visionProcDataBaseP, outputImageDesc);

   DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);

   inu_image__hdr_t *outputImgHdrP = (inu_image__hdr_t *)inu_data__getHdr(dataP);
   bufDescP->dataLen = IMAGE_SIZE_BYTES(outputImgHdrP);
   currentH = inu_node__getNextOutputNode(me, NULL);
   if (inu_ref__instanceOf(currentH, INU_CDNN_DATA_REF_TYPE))
   {
      inu_cdnn_data *cnndataP = (inu_cdnn_data *)dataP;
      CMEM_cacheInv((void*)&cnndataP->cdnn_dataHdr,sizeof(inu_cdnn_data__hdr_t));
      bufDescP->dataLen = cnndataP->cdnn_dataHdr.totOutputSize;
   }
   INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"bufDescP->dataLen %d %d %d %d\n", bufDescP->dataLen, dataP->imageHdr.imgDescriptor.stride, dataP->imageHdr.imgDescriptor.bufferHeight, dataP->imageHdr.imgDescriptor.bitsPerPixel);

   for(i = 0; i < inputParamsP->dataInputsNum; i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
   INU_REF__LOGG_PRINT(me,LOG_DEBUG_E,NULL,"DSP ACK free buf num %d\n", MEM_POOLG_getNumFree(privP->imagePoolH));
}
#endif

void inu_vision_proc__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_vision_proc__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_vision_proc__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_vision_proc__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_vision_proc__rxIoctl;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_vision_proc__rxData;

      _vtable.p_operate = inu_vision_proc__operate;
      _vtable.p_start   = inu_vision_proc__start;
      _vtable.p_stop    = inu_vision_proc__stop;
#if DEFSG_IS_GP
      _vtable.p_dspAck  = inu_vision_proc__dspAck;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_vision_proc__vtable_get(void)
{
   inu_vision_proc__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}
ERRG_codeE inu_vision_proc__updateParams(inu_vision_procH meH, inu_vision_proc__parameter_list_t *cfgParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_VISION_PROC_UPDATE_E,cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}
#else
#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "err_defs.h"
#include "log.h"
#include "assert.h"
#include "internal_cmd.h"
ERRG_codeE inu_vision_proc__updateParams(inu_vision_procH meH, inu_vision_proc__parameter_list_t *cfgParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_VISION_PROC_UPDATE_E,cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


#endif