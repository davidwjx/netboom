
#include "inu2_types.h"
#include "inu2_internal.h"
#include "inu_types.h"
#include "internal_cmd.h"
#include "err_defs.h"
#include "log.h"
#include "assert.h"
#include <math.h>

#if DEFSG_IS_GP
#include "data_base.h"
#include "cmem.h"
#endif

#if 1

#define SLAM_LUT_IN_ZIP (0)
#define UNDISTORT_LUT_COLS (640)
#define UNDISTORT_LUT_ROWS (480)
#define SLAM_LUT_SIZE (UNDISTORT_LUT_COLS*UNDISTORT_LUT_ROWS*8)
#define INU_SLAM_LUT_LEFT_BINARY_FILE_NAME            "/media/nu3000/inu_slam_left.bin"
#define INU_SLAM_LUT_RIGHT_BINARY_FILE_NAME           "/media/nu3000/inu_slam_right.bin"

typedef struct inu_slam__privData
{
   MEM_POOLG_handleT poolH;
   inu_slam_data     *slamDataP;
#if DEFSG_IS_GP
   UINT32            inu_slamImuDataPhyAddress;
   UINT32            inu_slamImuDataVirtAddress;
   UINT32            inu_slamImuDataSampleNum;
   UINT32            inu_slamPrevFrameBufferPhyAddress;
   UINT32            inu_slamPrevFrameBufferVirtAddress;
   UINT32            inu_slamKeyframesBufferPhyAddress;
   UINT32            inu_slamKeyframesBufferVirtAddress;
   UINT32            inu_slamMappointsBufferPhyAddress;
   UINT32            inu_slamMappointsBufferVirtAddress;
   UINT32            inu_slamCovisibilityGraphBufferPhyAddress;
   UINT32            inu_slamCovisibilityGraphBufferVirtAddress;
#endif
}inu_slam__privData;

typedef struct inu_slam__KpY
{
   INT32 KpYval;
   INT32 KpYindex;
}inu_slam__KpY;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_SLAM";
inu_slam__KpY slamSortedKpY[SLAM_MAX_KP];
#if SLAM_LUT_IN_ZIP
static UINT32           slamLeftUndistortLUTPhAddr;
static UINT32           slamLeftUndistortLUTVirtAddr;
static UINT32           slamRightUndistortLUTPhAddr;
static UINT32           slamRightUndistortLUTVirtAddr;
#endif
inu_slam__parameter_list_t inu_slam_hostGpMsg;

#define INU_SLAM__NUM_RESULTS_BUFFER (7)

static const char* inu_slam__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}


static void inu_slam__dtor(inu_ref *me)
{
   inu_slam__privData *privP = (inu_slam__privData*)((inu_slam*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   MEM_POOLG_closePool(privP->poolH);
#endif
   free(privP);
}


/* Constructor */
ERRG_codeE inu_slam__ctor(inu_slam *me, inu_slam__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_slam__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_slam__privData*)malloc(sizeof(inu_slam__privData));
      if (!privP)
      {
         return INU_SLAM__ERR_OUT_OF_MEM;
      }
      memset(privP,0,sizeof(inu_slam__privData));
      me->privP = privP;
#if DEFSG_IS_GP
      MEM_POOLG_cfgT cfg;

      cfg.bufferSize = SLAMG_MAX_BLOB_SIZE;
      cfg.numBuffers = INU_SLAM__NUM_RESULTS_BUFFER;
      cfg.resetBufPtrInAlloc = 0;
      cfg.freeCb = NULL;
      cfg.freeArg = NULL;
      cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
      cfg.memP = NULL;

      ret = MEM_POOLG_initPool(&privP->poolH, &cfg);
      if (ERRG_FAILED(ret))
      {
         free(privP);
      }
      memcpy(&inu_slam_hostGpMsg, &(ctorParamsP->params) , sizeof(inu_slam_hostGpMsg));
#endif
   }
   return ret;
}

static void inu_slam__newOutput(inu_node *me, inu_node *output)
{
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
   if (output->ref.refType == INU_SLAM_DATA_REF_TYPE)
   {
      inu_slam *slam = (inu_slam*)me;
      inu_slam__privData *privP = (inu_slam__privData*)slam->privP;
      //save fast_data node for easy access
      privP->slamDataP = (inu_slam_data*)output;
   }
}


static int inu_slam__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);

#if DEFSG_IS_GP
    switch (msgCode)
    {
    case(INTERNAL_CMDG_SLAM_UPDATE_E):
    {
      //Copy host message 
      memcpy(&inu_slam_hostGpMsg, msgP, sizeof(inu_slam_hostGpMsg));

    }break;
    default:
      break;
    }
#endif

   return ret;
}

static int inu_slam__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(msgP);
   FIX_UNUSED_PARAM_WARN(bufDescP);
   FIX_UNUSED_PARAM_WARN(dataP);
   FIX_UNUSED_PARAM_WARN(dataLen);
   FIX_UNUSED_PARAM_WARN(msgCode);
   return 0;
}

ERRG_codeE inu_slam__send_data_async(inu_slamH meH, char *bufP, unsigned int len)
{
   inu_data__hdr_t hdr;

   hdr.timestamp = 0;

   return inu_ref__copyAndSendDataAsync((inu_ref*)meH, INTERNAL_CMDG_DATA_SEND_E, &hdr, bufP, len);
}

#if DEFSG_IS_GP

#if 0
static int inu_slam__cmpKpY(const void *a, const void *b)
{   
   inu_slam__KpY *KpYA = (inu_slam__KpY *)a;
   inu_slam__KpY *KpYB = (inu_slam__KpY *)b;
   return (int)(KpYA->KpYval - KpYB->KpYval);
}
#endif

#if SLAM_LUT_IN_ZIP
static void inu_slam__Undistort(inu_cva_data *cva_data, float *leftLUT, float *rightLUT)
{
   inu_cva_data__hdr_t *cvaDataHdr = &(cva_data->cvaHdr);
   UINT32 key_points_num = cvaDataHdr->cvaDescriptor.kpNum;
   inu_cva_data__freak_descriptor_t *pData;
   UINT32 i;
   float *pUndistortLUT;

   pData = (inu_cva_data__freak_descriptor_t *)inu_data__getMemPtr((inu_dataH)cva_data);
   
   CMEM_cacheInv((void*)pData, key_points_num*sizeof(inu_cva_data__freak_descriptor_t));

   for (i = 0; i < key_points_num; i++)
   { 
      if (pData->xs > ((UNDISTORT_LUT_COLS-1)<< SLAM_Q_KP) || pData->ys > ((UNDISTORT_LUT_ROWS-1)<< SLAM_Q_KP))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"KEYPOINTS OUT OF FRAME in frame %llu index %d x %d y %d\n", cvaDataHdr->dataHdr.dataIndex, i, pData->xs, pData->ys);
      }
      //if (pData->xs == 0 && pData->ys == 0 && pData->patternSize == 0)
      //{
      // LOGG_PRINT(LOG_ERROR_E, NULL,"ZERO FRAME\n");
      //}

      pData->xs = MIN(pData->xs, (UNDISTORT_LUT_COLS-1) << SLAM_Q_KP);
      pData->ys = MIN(pData->ys, (UNDISTORT_LUT_ROWS-1) << SLAM_Q_KP);

      float x = (float)(pData->xs / 16384.f);
      int x1 = (pData->xs) >> SLAM_Q_KP;   //     floor
      int x2 = MIN(x1 + 1, UNDISTORT_LUT_COLS - 1);
      float y = (float)(pData->ys / 16384.f);
      int y1 = (pData->ys) >> SLAM_Q_KP;   //     floor;
      int y2 = MIN(y1 + 1, UNDISTORT_LUT_ROWS - 1);
      int idx1 = 2*(y1 * UNDISTORT_LUT_COLS + x1);
      int idx2 = 2*(y1 * UNDISTORT_LUT_COLS + x2);
      int idx3 = 2*(y2 * UNDISTORT_LUT_COLS + x1);
      int idx4 = 2*(y2 * UNDISTORT_LUT_COLS + x2);
      float coeffA = ((x2 - x) * (y2 - y));
      float coeffB = ((x - x1) * (y2 - y));
      float coeffC = ((x2 - x) * (y - y1));
      float coeffD = ((x - x1) * (y - y1));

      pUndistortLUT = leftLUT;
      if (pData->notImg1)
      {
         pUndistortLUT = rightLUT;
      }


      float undistX = *(pUndistortLUT + idx1) * coeffA +
                      *(pUndistortLUT + idx2) * coeffB +
                      *(pUndistortLUT + idx3) * coeffC +
                      *(pUndistortLUT + idx4) * coeffD;

      float undistY = *(pUndistortLUT + idx1 + 1) * coeffA +
                      *(pUndistortLUT + idx2 + 1) * coeffB +
                      *(pUndistortLUT + idx3 + 1) * coeffC +
                      *(pUndistortLUT + idx4 + 1) * coeffD;

      *(INT32*)(&(pData->xs)) = (INT32)(undistX * (1<<SLAM_Q_KP));
      *(INT32*)(&(pData->ys)) = (INT32)(undistY * (1<<SLAM_Q_KP));
      pData ++;
   }
}
#endif

#endif

UINT64 startUsec, currentUsec;


static ERRG_codeE inu_slam__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#if DEFSG_IS_GP
   inu_slam__privData        *privP = (inu_slam__privData*)((inu_slam*)me)->privP;
   DATA_BASEG_slamDataDataBaseT *slamDataBaseP;
   UINT32 dspTarget = 0;//TO DO slamDb.dspTarget;
   DATA_BASEG_databaseE dataBase = DATA_BASEG_SLAM;
#endif
   ret = inu_function__vtable_get()->p_start(me, startParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_slam__start\n");
   }
   

#if DEFSG_IS_GP

#if SLAM_LUT_IN_ZIP
   FILE *undistortLutFile;
   UINT32 size;

   //open left LUT
   undistortLutFile   = fopen(INU_SLAM_LUT_LEFT_BINARY_FILE_NAME, "rb");
   if(!undistortLutFile)
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Can't find undistort LUT files\n");
      return INU_SLAM__ERR_IO_ERROR;
   }
   
   ret = MEM_MAPG_alloc(SLAM_LUT_SIZE,&slamLeftUndistortLUTPhAddr,&slamLeftUndistortLUTVirtAddr,1);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for undistort LUT\n");
      return INU_SLAM__ERR_OUT_OF_MEM;
   }

   size = fread( (void*)slamLeftUndistortLUTVirtAddr, 1, SLAM_LUT_SIZE, undistortLutFile);
   if(!size)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading undistort LUT File\n");
      return INU_SLAM__ERR_IO_ERROR;
   }
   CMEM_cacheWb((void*)slamLeftUndistortLUTVirtAddr, SLAM_LUT_SIZE);

   //Close file
   if(undistortLutFile)
   {
      fclose(undistortLutFile);
   }
   
   //open right LUT
   undistortLutFile   = fopen(INU_SLAM_LUT_RIGHT_BINARY_FILE_NAME, "rb");
   if(!undistortLutFile)
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Can't find undistort LUT files\n");
      return INU_SLAM__ERR_IO_ERROR;
   }
   
   ret = MEM_MAPG_alloc(SLAM_LUT_SIZE,&slamRightUndistortLUTPhAddr,&slamRightUndistortLUTVirtAddr,1);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for undistort LUT\n");
      return INU_SLAM__ERR_OUT_OF_MEM;
   }

   size = fread( (void*)slamRightUndistortLUTVirtAddr, 1, SLAM_LUT_SIZE, undistortLutFile);
   if(!size)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR reading undistort LUT File\n");
      return INU_SLAM__ERR_IO_ERROR;
   }
   CMEM_cacheWb((void*)slamRightUndistortLUTVirtAddr, SLAM_LUT_SIZE);

   //Close file
   if(undistortLutFile)
   {
      fclose(undistortLutFile);
   }
#endif

   // allocation of imu data buffer
   ret = MEM_MAPG_alloc(SLAM_DATA_IMU_SIZE, 
                        &privP->inu_slamImuDataPhyAddress,
                        &privP->inu_slamImuDataVirtAddress,
                        1);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for slam IMU (%d)\n",SLAM_DATA_IMU_SIZE);
      return INU_SLAM__ERR_OUT_OF_MEM;
   }
   privP->inu_slamImuDataSampleNum = 0;
   LOGG_PRINT(LOG_INFO_E, 0,  "privP->inu_slamImuDataPhyAddress = %x size=%d\n",privP->inu_slamImuDataPhyAddress, SLAM_DATA_IMU_SIZE);
#if 1
   // allocation of previous frame buffer
   ret = MEM_MAPG_alloc(sizeof(FrameXM4_frameT), 
                        &privP->inu_slamPrevFrameBufferPhyAddress,
                        &privP->inu_slamPrevFrameBufferVirtAddress,
                        0);
LOGG_PRINT(LOG_INFO_E, 0,  "privP->inu_slamPrevFrameBufferPhyAddress = %x size=%d\n",privP->inu_slamPrevFrameBufferPhyAddress, sizeof(FrameXM4_frameT));
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for slam prev buffer (%d)\n",sizeof(FrameXM4_frameT));
      return INU_SLAM__ERR_OUT_OF_MEM;
   }

   // allocation of keyframes buffer
   ret = MEM_MAPG_alloc(XM4_MAX_KEYFRAMES * sizeof(FrameXM4_longTermFrameT), 
                        &privP->inu_slamKeyframesBufferPhyAddress,
                        &privP->inu_slamKeyframesBufferVirtAddress,
                        0);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for slam keyframes buffer (%d)\n",XM4_MAX_KEYFRAMES * sizeof(FrameXM4_longTermFrameT));
      return INU_SLAM__ERR_OUT_OF_MEM;
   }
   LOGG_PRINT(LOG_INFO_E, 0,  "privP->inu_slamKeyframesBufferPhyAddress = %x size=%d\n",privP->inu_slamKeyframesBufferPhyAddress, XM4_MAX_KEYFRAMES * sizeof(FrameXM4_longTermFrameT));
      

   // allocation of mappoints buffer
   ret = MEM_MAPG_alloc(sizeof(MapXM4_mappointsT), 
                        &privP->inu_slamMappointsBufferPhyAddress,
                        &privP->inu_slamMappointsBufferVirtAddress,
                        0);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for slam mappoint buffer (%d)\n",sizeof(MapXM4_mappointsT));
      return INU_SLAM__ERR_OUT_OF_MEM;
   }
   LOGG_PRINT(LOG_INFO_E, 0,  "privP->inu_slamMappointsBufferPhyAddress = %x size=%d\n",privP->inu_slamMappointsBufferPhyAddress, sizeof(MapXM4_mappointsT));
      


   // allocation of covisibility graph buffer
   ret = MEM_MAPG_alloc(XM4_MAX_KEYFRAMES * sizeof(KeyframesXM4_connectedKeyframesT), 
                        &privP->inu_slamCovisibilityGraphBufferPhyAddress,
                        &privP->inu_slamCovisibilityGraphBufferVirtAddress,
                        0);
   LOGG_PRINT(LOG_INFO_E, 0,  "privP->inu_slamCovisibilityGraphBufferPhyAddress = %x size=%d\n",privP->inu_slamCovisibilityGraphBufferPhyAddress, XM4_MAX_KEYFRAMES * sizeof(KeyframesXM4_connectedKeyframesT));
   
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, 0,  "ERROR: Failed to init memory for slam covosibility buffer (%d)\n",XM4_MAX_KEYFRAMES * sizeof(KeyframesXM4_connectedKeyframesT));
      return INU_SLAM__ERR_OUT_OF_MEM;
   }
#endif
   if (dspTarget == ICCG_CMD_TARGET_DSPB)
   {
      dataBase = DATA_BASEG_SLAM_EV;
   }

   DATA_BASEG_accessDataBase((UINT8**)&slamDataBaseP, dataBase);

   slamDataBaseP->baseline = inu_slam_hostGpMsg.baseline;
   slamDataBaseP->fx = inu_slam_hostGpMsg.fx;
   slamDataBaseP->fy = inu_slam_hostGpMsg.fy;
   slamDataBaseP->cx = inu_slam_hostGpMsg.cx;
   slamDataBaseP->cy = inu_slam_hostGpMsg.cy;
   slamDataBaseP->IMU_freq = sqrt(inu_slam_hostGpMsg.IMU_freq);
   slamDataBaseP->IMU_Na = inu_slam_hostGpMsg.NoiseAcc;
   slamDataBaseP->IMU_Ng = inu_slam_hostGpMsg.NoiseGyro;
   slamDataBaseP->IMU_Naw = inu_slam_hostGpMsg.AccWalk;
   slamDataBaseP->IMU_Ngw = inu_slam_hostGpMsg.GyroWalk;
   //TO DO: following parameters are not used
   //slamDataBaseP->width = inu_slam_hostGpMsg.width;
   //slamDataBaseP->height = inu_slam_hostGpMsg.height;
   //slamDataBaseP->x_start = inu_slam_hostGpMsg.x_start;
   //slamDataBaseP->y_start = inu_slam_hostGpMsg.y_start;
   //slamDataBaseP->thDepth = inu_slam_hostGpMsg.thDepth; 
   
   slamDataBaseP->TrackingSt.rhoThresholdMono = inu_slam_hostGpMsg.TrackingSt.rhoThresholdMono;
   slamDataBaseP->TrackingSt.rhoThresholdStereo = inu_slam_hostGpMsg.TrackingSt.rhoThresholdStereo;
   slamDataBaseP->TrackingSt.MaximumDepthThreshold = inu_slam_hostGpMsg.TrackingSt.MaximumDepthThreshold;
   slamDataBaseP->TrackingSt.minimumDepthThreshold = inu_slam_hostGpMsg.TrackingSt.minimumDepthThreshold;
   slamDataBaseP->TrackingSt.ClosePointsDepthThreshold = inu_slam_hostGpMsg.TrackingSt.ClosePointsDepthThreshold;

   slamDataBaseP->TrackingSt.StereoMatcherSt.secondBestTh_100 = inu_slam_hostGpMsg.TrackingSt.StereoMatcherSt.secondBestTh_100;
   slamDataBaseP->TrackingSt.StereoMatcherSt.radiusThreshold = inu_slam_hostGpMsg.TrackingSt.StereoMatcherSt.radiusThreshold;
   slamDataBaseP->TrackingSt.StereoMatcherSt.HammingDistanceThreshold = inu_slam_hostGpMsg.TrackingSt.StereoMatcherSt.HammingDistanceThreshold;
   slamDataBaseP->TrackingSt.StereoMatcherSt.scaleDifferenceThreshold = inu_slam_hostGpMsg.TrackingSt.StereoMatcherSt.scaleDifferenceThreshold;

   slamDataBaseP->TrackingSt.FrameMatcherSt.ratioThreshold_100 = inu_slam_hostGpMsg.TrackingSt.FrameMatcherSt.ratioThreshold_100;
   slamDataBaseP->TrackingSt.FrameMatcherSt.radiusThreshold = inu_slam_hostGpMsg.TrackingSt.FrameMatcherSt.radiusThreshold;
   slamDataBaseP->TrackingSt.FrameMatcherSt.HammingDistanceThreshold = inu_slam_hostGpMsg.TrackingSt.FrameMatcherSt.HammingDistanceThreshold;
   slamDataBaseP->TrackingSt.FrameMatcherSt.scaleDifferenceThreshold = inu_slam_hostGpMsg.TrackingSt.FrameMatcherSt.scaleDifferenceThreshold;

   slamDataBaseP->TrackingSt.KeyframesSt.UseKeyframes = inu_slam_hostGpMsg.TrackingSt.KeyframesSt.UseKeyframes;
   slamDataBaseP->TrackingSt.KeyframesSt.numberOfKeyframesForMatching = inu_slam_hostGpMsg.TrackingSt.KeyframesSt.numberOfKeyframesForMatching;
   slamDataBaseP->TrackingSt.KeyframesSt.connectedKeyframesThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframesSt.connectedKeyframesThreshold;
   slamDataBaseP->TrackingSt.KeyframesSt.numTrackedClosePointsThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframesSt.numTrackedClosePointsThreshold;
   slamDataBaseP->TrackingSt.KeyframesSt.numNonTrackedClosePointsThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframesSt.numNonTrackedClosePointsThreshold;

   slamDataBaseP->TrackingSt.KeyframeMatcherSt.ratioThreshold_100 = inu_slam_hostGpMsg.TrackingSt.KeyframeMatcherSt.ratioThreshold_100;
   slamDataBaseP->TrackingSt.KeyframeMatcherSt.radiusThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframeMatcherSt.radiusThreshold;
   slamDataBaseP->TrackingSt.KeyframeMatcherSt.HammingDistanceThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframeMatcherSt.HammingDistanceThreshold;
   slamDataBaseP->TrackingSt.KeyframeMatcherSt.scaleDifferenceThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframeMatcherSt.scaleDifferenceThreshold;

   slamDataBaseP->TrackingSt.MapPointCullingSt.mapPointCullRatioThr = inu_slam_hostGpMsg.TrackingSt.MapPointCullingSt.mapPointCullRatioThr;
   slamDataBaseP->TrackingSt.MapPointCullingSt.observingKeyframesThr = inu_slam_hostGpMsg.TrackingSt.MapPointCullingSt.observingKeyframesThr;
   slamDataBaseP->TrackingSt.MapPointCullingSt.keyframeIndexDiffThr = inu_slam_hostGpMsg.TrackingSt.MapPointCullingSt.keyframeIndexDiffThr;
   slamDataBaseP->TrackingSt.MapPointCullingSt.keyframeIndexDiffThr2 = inu_slam_hostGpMsg.TrackingSt.MapPointCullingSt.keyframeIndexDiffThr2;

   slamDataBaseP->TrackingSt.KeyframeCullingSt.observationThreshold = inu_slam_hostGpMsg.TrackingSt.KeyframeCullingSt.observationThreshold;
   slamDataBaseP->TrackingSt.KeyframeCullingSt.redundantMapPointThreshold_100 = inu_slam_hostGpMsg.TrackingSt.KeyframeCullingSt.redundantMapPointThreshold_100;

   slamDataBaseP->LocalBundleAdjustmentSt.RunLocalBundleAdjustment = inu_slam_hostGpMsg.LocalBundleAdjustmentSt.RunLocalBundleAdjustment;
   slamDataBaseP->LocalBundleAdjustmentSt.numberOfKeyframesForLBA = inu_slam_hostGpMsg.LocalBundleAdjustmentSt.numberOfKeyframesForLBA;

   slamDataBaseP->LocalizationSt.RunRelocalization = inu_slam_hostGpMsg.LocalizationSt.RunRelocalization;

   slamDataBaseP->fps = inu_slam_hostGpMsg.fps;
   
   slamDataBaseP->inu_slamPrevFrameBufferPhyAddress = privP->inu_slamPrevFrameBufferPhyAddress;
   slamDataBaseP->inu_slamKeyframesBufferPhyAddress = privP->inu_slamKeyframesBufferPhyAddress;
   slamDataBaseP->inu_slamMappointsBufferPhyAddress = privP->inu_slamMappointsBufferPhyAddress;
   slamDataBaseP->inu_slamCovisibilityGraphBufferPhyAddress = privP->inu_slamCovisibilityGraphBufferPhyAddress;
   
   DATA_BASEG_accessDataBaseRelease(dataBase);
   
   INU_REF__LOGG_PRINT(me,LOG_INFO_E,NULL,"dspTarget %d\n", dspTarget);
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_SLAM,1,dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
#endif
   return ret;
}

static ERRG_codeE inu_slam__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_stop(me, stopParamP);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error inu_slam__stop\n");
   }
      
#if DEFSG_IS_GP
   inu_slam__privData        *privP = (inu_slam__privData*)((inu_slam*)me)->privP;
   UINT32 dspTarget = 0;//TODO slamDb.dspTarget;
   ret = inu_device__sendDspSyncMsg((inu_device*)inu_ref__getDevice((inu_ref*)me),DATA_BASEG_ALG_SLAM,2, dspTarget);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error sending dsp message\n");
   }
   MEM_MAPG_free((UINT32*)privP->inu_slamCovisibilityGraphBufferVirtAddress);
   MEM_MAPG_free((UINT32*)privP->inu_slamMappointsBufferVirtAddress);
   MEM_MAPG_free((UINT32*)privP->inu_slamKeyframesBufferVirtAddress);
   MEM_MAPG_free((UINT32*)privP->inu_slamPrevFrameBufferVirtAddress);
   MEM_MAPG_free((UINT32*)privP->inu_slamImuDataVirtAddress);
#if SLAM_LUT_IN_ZIP
   ret = MEM_MAPG_free((UINT32*)slamLeftUndistortLUTVirtAddr);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error free slam left LUT\n");
   }
   ret = MEM_MAPG_free((UINT32*)slamRightUndistortLUTVirtAddr);
   if (ERRG_FAILED(ret))
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E,NULL,"error free slam right LUT\n");
   }
#endif
#endif
   return ret;
}

#if DEFSG_IS_GP
static void inu_slam__operate(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   inu_function__vtable_get()->p_operate(me, inputParamsP);

   ERRG_codeE                  ret;
   MEM_POOLG_bufDescT          *bufDescP = NULL;
   inu_data                    *slam_data;
   inu_slam__privData          *privP = (inu_slam__privData *)(((inu_slam*)me)->privP);
   inu_slam_data__hdr_t        slamHdr;
   int                         i;
   UINT32                      dspTarget;
   inu_cva_data                *cva_data;
   inu_slam_imuDataInT  *slamImuDataVirtAddress = (inu_slam_imuDataInT *)(privP->inu_slamImuDataVirtAddress) + privP->inu_slamImuDataSampleNum;
   inu_slam_imuDataInT  *slamImuDataPhyAddress = (inu_slam_imuDataInT *)(privP->inu_slamImuDataPhyAddress);


   //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "received input %d\n", inputParamsP->dataInputsNum);

   inu_data__hdr_t *dataHdrP = inu_data__getHdr(inputParamsP->dataInputs[0]);
   if (dataHdrP->flags == 64)
   {
      inu_imu_data__readableData readableData;
      //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "received imu\n");
      ret = inu_imu_data__convertData(inputParamsP->dataInputs[0],&readableData);
      if (ERRG_SUCCEEDED(ret))
      {
         //for (i=0; i<(INU_DEFSG_POS_SENSOR_CHANNEL_NUM_TYPES_E-2);i++)
         //{
         // if (readableData.imuData[i].name!=NULL)
         //   {
         //    INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"name=%s, value=%f\n",readableData.imuData[i].name,readableData.imuData[i].imuVal);
         //   }
         //}
         //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"timeStamp=%lld \n",readableData.timeStamp);
         
         if (privP->inu_slamImuDataSampleNum < SLAM_DATA_IMU_SAMPLES)
         {
            slamImuDataVirtAddress->acc[0] = readableData.imuData[INU_DEFSG_POS_SENSOR_ACCELAROMETER_X_E].imuVal;
            slamImuDataVirtAddress->acc[1] = readableData.imuData[INU_DEFSG_POS_SENSOR_ACCELAROMETER_Y_E].imuVal;
            slamImuDataVirtAddress->acc[2] = readableData.imuData[INU_DEFSG_POS_SENSOR_ACCELAROMETER_Z_E].imuVal;
            slamImuDataVirtAddress->gyro[0] = readableData.imuData[INU_DEFSG_POS_SENSOR_GYROSCOPE_X_E].imuVal;
            slamImuDataVirtAddress->gyro[1] = readableData.imuData[INU_DEFSG_POS_SENSOR_GYROSCOPE_Y_E].imuVal;
            slamImuDataVirtAddress->gyro[2] = readableData.imuData[INU_DEFSG_POS_SENSOR_GYROSCOPE_Z_E].imuVal;
            slamImuDataVirtAddress->timeStamp = readableData.timeStamp;
            privP->inu_slamImuDataSampleNum ++;
         }
         else
         {
            INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL,"too many IMU samples %d max %d\n", privP->inu_slamImuDataSampleNum, SLAM_DATA_IMU_SAMPLES);
            privP->inu_slamImuDataSampleNum = 0;
         }
      }
      for (i = 0; i<inputParamsP->dataInputsNum; i++)
      {
         inu_function__doneData(me, inputParamsP->dataInputs[i]);
      }
      inu_function__complete(me);
      return;
   }

   ret = MEM_POOLG_alloc(privP->poolH, MEM_POOLG_getBufSize(privP->poolH), &bufDescP);
   if (ERRG_SUCCEEDED(ret) && bufDescP)
   {
      INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL, "received cva %d imu samples\n", privP->inu_slamImuDataSampleNum);
      cva_data = (inu_cva_data *)(inputParamsP->dataInputs[0]);
      //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"CVA timeStamp=%lld \n",cva_data->cvaHdr.dataHdr.timestamp);

      //eliminate imu timestamps recieved after cva time stamp
      int lastSample = 0;
      if (privP->inu_slamImuDataSampleNum)
      {
         slamImuDataVirtAddress --;
         while ((slamImuDataVirtAddress->timeStamp > cva_data->cvaHdr.dataHdr.timestamp) && privP->inu_slamImuDataSampleNum)
         {
            slamImuDataVirtAddress --;
            privP->inu_slamImuDataSampleNum --;
            lastSample = 1;
         }
      }
      
      slamHdr.dataHdr = *dataHdrP;
      slamHdr.ddrP = (UINT8 *)slamImuDataPhyAddress;
      slamHdr.slamImuDataSampleNum = privP->inu_slamImuDataSampleNum + lastSample;
      INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL, "after eliminate %d imu samples\n", privP->inu_slamImuDataSampleNum);
      privP->inu_slamImuDataSampleNum = 0;
      ret = inu_function__newData(me, (inu_data*)privP->slamDataP, bufDescP, &slamHdr, (inu_data**)&slam_data);
      if (ERRG_FAILED(ret))
      {
         assert(0);
         return;
      }
      //OS_LYRG_getUsecTime(&startUsec);
#if SLAM_LUT_IN_ZIP
      inu_slam__Undistort(cva_data, (float *)slamLeftUndistortLUTVirtAddr, (float *)slamRightUndistortLUTVirtAddr);
#endif
      //    inu_slam__sortData(cva_data, slamDataIn);
      inu_cva_data__hdr_t *cvaDataHdr = &(cva_data->cvaHdr);
      UINT32 key_points_num = cvaDataHdr->cvaDescriptor.kpNum;
      inu_cva_data__freak_descriptor_t *pData;
      if (key_points_num > SLAM_MAX_KP)
      {
         key_points_num = SLAM_MAX_KP;
         LOGG_PRINT(LOG_WARN_E, NULL, "SLAM keypoints CLIPPED to %d\n", key_points_num);
      }
      pData = (inu_cva_data__freak_descriptor_t *)inu_data__getMemPtr((inu_dataH)cva_data);
      CMEM_cacheInv((void*)pData, key_points_num*sizeof(inu_cva_data__freak_descriptor_t));

      CMEM_cacheWb((void*)(privP->inu_slamImuDataVirtAddress), SLAM_DATA_IMU_SIZE);

      
      inputParamsP->dataInputs[inputParamsP->dataInputsNum] = slam_data;
      inputParamsP->dataInputsNum++;
      dspTarget = 0;// TODO slamDb.dspTarget;
      ret = inu_function__sendDspMsg(me, inputParamsP, DATA_BASEG_ALG_SLAM, 0, 0, dspTarget);
      if (ERRG_FAILED(ret))
      {
         INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "error sending dsp message\n");
      }
   }
   else
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, NULL, "mem alloc failed\n");
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


static void inu_slam__dspAck(inu_function *me, inu_function__operateParamsT *inputParamsP, inu_function__coreE dspSource)
{
   UINT32 i;
   (void)dspSource;
   inu_cva_data *cvaDataP = inputParamsP->dataInputs[0];
   UINT16 dataInputsNum = inputParamsP->dataInputsNum;
   inu_slam_data *slamDataP = inputParamsP->dataInputs[dataInputsNum-1];
   MEM_POOLG_bufDescT *bufDescP = NULL;

       
   CMEM_cacheInv((void*)&slamDataP->slam_dataHdr,sizeof(inu_slam_data__hdr_t));
   inu_data__bufDescPtrGet((inu_data*)slamDataP,&bufDescP);
   bufDescP->dataLen = SLAMG_MAX_BLOB_SIZE;

   slamDataP->slam_dataHdr.frameId = cvaDataP->cvaHdr.dataHdr.dataIndex;
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E,NULL,"slam done\n");
   
   for(i=0;i<inputParamsP->dataInputsNum;i++)
   {
      inu_function__doneData(me, inputParamsP->dataInputs[i]);
   }
   inu_function__complete(me);
}
#endif

void inu_slam__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_slam__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_slam__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_slam__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_slam__rxIoctl;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_slam__rxData;

     _vtable.node_vtable.p_newOutput = inu_slam__newOutput;

      _vtable.p_start   = inu_slam__start;
      _vtable.p_stop    = inu_slam__stop;
#if DEFSG_IS_GP
      _vtable.p_dspAck  = inu_slam__dspAck;
      _vtable.p_operate = inu_slam__operate;
#endif

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_slam__vtable_get(void)
{
   inu_slam__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}
#endif

ERRG_codeE inu_slam__updateParams(inu_slamH meH, inu_slam__parameter_list_t* cfgParamsP)
{
    return inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_SLAM_UPDATE_E, cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}
