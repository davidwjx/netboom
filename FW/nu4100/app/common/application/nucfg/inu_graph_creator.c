
#include "log.h"
#include "inu_graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sxmlc.h"
#include "sxmlsearch.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "inu2.h"
#include "inu2_internal.h"
#include "nucfg.h"
#include "calibration.h"
#include <inttypes.h>
#include "assert.h"
#include <errno.h>
#include "helsinki.h"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************/

#define MAX_FUNC_INPUTS  INU_FUNCTION__MAX_NUM_INPUTS
#define MAX_HW_INJECTS   2
#define MAX_FUNC_PARAMS  20
#define DEFAULT_INPUT_QUE_DEPTH (10)
#define DEFAULT_MIN_INPUTS_OPERATE (0xffff) //  wait for all inputs

typedef enum
{
   INU_SW_GRAPH_ERROR_E = -1,
   INU_SW_GRAPH_FUNC_TYPE_E = 0,
   INU_SW_GRAPH_DATA_TYPE_E,
   INU_STREAMER__NUM_TYPSS_E
} inu_sw_graph_element_typeE;

typedef struct
{
   inu_sw_graph_element_typeE element_type;
   char *func_data_type;
   char *func_data_name;
   int   func_pipe_size;
   int   func_work_priority;
   int   func_input_que_depth;
   int   func_min_inputs_oprt;
   unsigned int mode;
   int   conn_id;
   int   num_inputs;
   char *func_data_inputs[MAX_FUNC_INPUTS];
   char *source;
   int   num_hw_injects;
   char *HW_inject[MAX_HW_INJECTS];
   int   num_params;
   char *params_field[MAX_FUNC_PARAMS];
   int   params_val[MAX_FUNC_PARAMS];
   inu_image__descriptor Data_obj_img;
   inu_cva_data__descriptor_t cva_data;
   int   active;
   int   actionMap;
   int   offlinerWriterInst;
}inu_sw_graph__element_struct;
/************************************************************************/
static void inu_graph__fillFunctionCtorDefault(inu_graph *graph, inu_function__CtorParams *functionCtorP)
{
   functionCtorP->nodeCtorParams.ref_params.device = inu_ref__getDevice((inu_ref*)graph);
   functionCtorP->nodeCtorParams.graph_ref_id = graph->ref.id;
   functionCtorP->pipeMaxSize    = MIN_FUNCTION_PIPE_SIZE;
   functionCtorP->inputQueDepth  = DEFAULT_INPUT_QUE_DEPTH;
   functionCtorP->minInptsToOprt = DEFAULT_MIN_INPUTS_OPERATE;
   functionCtorP->mode           = 0xFFFFFFFF;
   functionCtorP->workPriority   = INU_FUNCTION__WORK_THR_PRIORITY_NO_THR;
   functionCtorP->syncedFunc     = 0;
}

static void inu_graph__fillFunctionCtor(inu_graph* graph, inu_function__CtorParams *functionCtorP, inu_sw_graph__element_struct *inu_sw_graph)
{
   functionCtorP->nodeCtorParams.ref_params.device = inu_ref__getDevice((inu_ref*)graph);
   functionCtorP->nodeCtorParams.graph_ref_id = graph->ref.id;
   strcpy(functionCtorP->nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   functionCtorP->pipeMaxSize    = inu_sw_graph->func_pipe_size;
   functionCtorP->workPriority   = (inu_function__workThrdPriority)inu_sw_graph->func_work_priority;
   functionCtorP->inputQueDepth  = inu_sw_graph->func_input_que_depth;
   functionCtorP->minInptsToOprt = inu_sw_graph->func_min_inputs_oprt;
   functionCtorP->mode           = inu_sw_graph->mode;
   functionCtorP->syncedFunc     = 0;
}

static void inu_graph__fillDataCtorDefault(inu_graph *graph, inu_data__CtorParams *dataCtorP)
{
   dataCtorP->nodeCtorParams.ref_params.device = inu_ref__getDevice((inu_ref*)graph);
   dataCtorP->nodeCtorParams.graph_ref_id = graph->ref.id;
}


static ERRG_codeE inu_graph__createXMLStreamIn(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_streamer__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.streamDirection = INU_STREAMER__IN;
   ctorParams.numBuffers = inu_sw_graph->params_val[1];
   ctorParams.connId = (inu_streamer__connIdE)inu_sw_graph->conn_id;
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_STREAMER_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createMetadataInjector(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_metadata_injector__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.readerID = inu_sw_graph->params_val[1];
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_METADATA_INJECT_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLStreamOut(inu_graph* graph, inu_refH *inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_streamer__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.streamDirection = INU_STREAMER__OUT;
   ctorParams.numBuffers = inu_sw_graph->params_val[1];
   ctorParams.connId = (inu_streamer__connIdE)inu_sw_graph->conn_id;
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_STREAMER_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLData(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_data__CtorParams ctorParams;
   inu_refH source;
   inu_graph__fillDataCtorDefault(graph, &ctorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_RAW_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLFDK(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_fdk__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.nodeId = inu_sw_graph->params_val[1];
   ctorParams.numBuffs = inu_sw_graph->params_val[2];
   ctorParams.buffSize = inu_sw_graph->params_val[3];
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_FDK_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLSensorControl(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_sensor_control__CtorParams ctorParams;

   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   memset(&ctorParams.params, 0, sizeof(inu_sensor_control__parameter_list_t));

   ctorParams.params.operationMode = (inu_sensor_control__operation_mode_e)inu_sw_graph->params_val[1];
   if (ctorParams.params.operationMode == INU_SENSOR_CONTROL__SW_MODE)
   {
      ctorParams.params.aggressiveStep         = (UINT32)inu_sw_graph->params_val[2];
      ctorParams.params.nSatMax                = (UINT32)inu_sw_graph->params_val[3];
      ctorParams.params.glSat                  = (UINT32)inu_sw_graph->params_val[4];
      ctorParams.params.mdMax                  = (UINT32)inu_sw_graph->params_val[5];
      ctorParams.params.mdMin                  = (UINT32)inu_sw_graph->params_val[6];
      ctorParams.params.exposureStepResolution = (UINT32)inu_sw_graph->params_val[7];
      ctorParams.params.deltaSatMax            = (UINT32)inu_sw_graph->params_val[8];
      ctorParams.params.noActivationPeriod     = (UINT32)inu_sw_graph->params_val[9];
      ctorParams.params.exposureMax            = (UINT32)inu_sw_graph->params_val[10];
      ctorParams.params.exposureMin            = (UINT32)inu_sw_graph->params_val[11];
      ctorParams.params.debug                  = (UINT32)inu_sw_graph->params_val[12];
      ctorParams.params.algVersion             = (UINT32)inu_sw_graph->params_val[13];
      ctorParams.params.snrTarget              = (float)(inu_sw_graph->params_val[14]);
      ctorParams.params.slopeWeight            = (float)(inu_sw_graph->params_val[15] / (float)100); //for support float numbers. inu_sw_graph->params_val hold int values
   }

   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SENSOR_CONTROL_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLFastORB(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_fast_orb__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_FAST_ORB_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLFastORB_Data(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_data__CtorParams ctorParams;
   inu_refH source;
   inu_graph__fillDataCtorDefault(graph, &ctorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_FAST_ORB_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLCNN(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_cdnn__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_CDNN_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLVisionProc(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_vision_proc__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_VISION_PROC_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLDPE_PP(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_dpe_pp__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_DPE_PP_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLPP(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_pp__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.actionMap = (INU_PP__actionE)inu_sw_graph->actionMap;
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_PP_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLCompress(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_cdnn__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_COMPRESS_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLTsnr_Calc(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_tsnr_calc__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   
   ctorParams.params.saturationThresholdInImage   = (UINT32)inu_sw_graph->params_val[1];
   ctorParams.params.allowedBadSNRBlocksThreshold = (UINT32)inu_sw_graph->params_val[2];
   ctorParams.params.saturatedThreshold           = (UINT32)inu_sw_graph->params_val[3];
   ctorParams.params.saturatedGrayLevelValue      = (UINT32)inu_sw_graph->params_val[4];
   ctorParams.params.sideToCalc                   = (UINT32)inu_sw_graph->params_val[5];
   
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_TSNR_CALC_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLCNN_Data(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_data__CtorParams ctorParams;
   inu_refH source;
   inu_graph__fillDataCtorDefault(graph, &ctorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_CDNN_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLPointCloud_Data(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_data__CtorParams ctorParams;
   inu_refH source;
   inu_graph__fillDataCtorDefault(graph, &ctorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_POINT_CLOUD_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLSLAM(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_slam__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   
   CALIBG_ymlGetSectionFcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_4_E, &(ctorParams.params.fx), &(ctorParams.params.fy));
   CALIBG_ymlGetSectionCcValue(CALIB_LUT_MODE_FULL_E, CALIB_YML_SECTION_SENSOR_4_E, &(ctorParams.params.cx), &(ctorParams.params.cy));
   CALIBG_ymlGetSectionsBLValue(CALIB_LUT_MODE_FULL_E, 3, 4, &(ctorParams.params.baseline));

   ctorParams.functionCtorParams.minInptsToOprt = 1;

   //IMU noise
   ctorParams.params.NoiseGyro = 1.7e-04;
   ctorParams.params.NoiseAcc = 2.0e-03;
   ctorParams.params.GyroWalk = 1.9393e-05;
   ctorParams.params.AccWalk = 3.e-03;
   ctorParams.params.IMU_freq = 200;

   //config
   float rho_threshold_mono = 2.44765193;
   float rho_threshold_stereo = 2.79553214;

   ctorParams.params.TrackingSt.rhoThresholdMono = (int)(rho_threshold_mono * (float)(1 << 6));
   ctorParams.params.TrackingSt.rhoThresholdStereo = (int)(rho_threshold_stereo * (float)(1 << 6));
   ctorParams.params.TrackingSt.MaximumDepthThreshold = (int)(35 * (float)(1 << 14));
   ctorParams.params.TrackingSt.minimumDepthThreshold = (int)((ctorParams.params.baseline / 1000.0) * (float)(1 << 14));
   ctorParams.params.TrackingSt.ClosePointsDepthThreshold = 50;

   ctorParams.params.TrackingSt.StereoMatcherSt.secondBestTh_100 = 80;	//	ratio in percentage
   ctorParams.params.TrackingSt.StereoMatcherSt.radiusThreshold = (int)(2.0f * (float)(1 << 14));
   ctorParams.params.TrackingSt.StereoMatcherSt.HammingDistanceThreshold = 150;
   ctorParams.params.TrackingSt.StereoMatcherSt.scaleDifferenceThreshold = (int)(0.5f * (float)(1 << 8));

   ctorParams.params.TrackingSt.FrameMatcherSt.ratioThreshold_100 = 70;	//	ratio in percentage
   ctorParams.params.TrackingSt.FrameMatcherSt.radiusThreshold = 15;
   ctorParams.params.TrackingSt.FrameMatcherSt.HammingDistanceThreshold = 150;
   ctorParams.params.TrackingSt.FrameMatcherSt.scaleDifferenceThreshold = (int)(0.5f * (float)(1 << 8));

   ctorParams.params.TrackingSt.KeyframesSt.UseKeyframes = 1;
   ctorParams.params.TrackingSt.KeyframesSt.numberOfKeyframesForMatching = 10;
   ctorParams.params.TrackingSt.KeyframesSt.connectedKeyframesThreshold = 15;
   ctorParams.params.TrackingSt.KeyframesSt.numTrackedClosePointsThreshold = 100;
   ctorParams.params.TrackingSt.KeyframesSt.numNonTrackedClosePointsThreshold = 70;

   ctorParams.params.TrackingSt.KeyframeMatcherSt.ratioThreshold_100 = (int)(0.7 * (float)100);	//	ratio in percentage
   ctorParams.params.TrackingSt.KeyframeMatcherSt.radiusThreshold = 3;
   ctorParams.params.TrackingSt.KeyframeMatcherSt.HammingDistanceThreshold = 150;
   ctorParams.params.TrackingSt.KeyframeMatcherSt.scaleDifferenceThreshold = (int)(0.5 * (float)(1 << 8));

   ctorParams.params.TrackingSt.MapPointCullingSt.mapPointCullRatioThr = 0.25;
   ctorParams.params.TrackingSt.MapPointCullingSt.observingKeyframesThr = 3;
   ctorParams.params.TrackingSt.MapPointCullingSt.keyframeIndexDiffThr = 2;
   ctorParams.params.TrackingSt.MapPointCullingSt.keyframeIndexDiffThr2 = 3;

   ctorParams.params.TrackingSt.KeyframeCullingSt.observationThreshold = 3;
   ctorParams.params.TrackingSt.KeyframeCullingSt.redundantMapPointThreshold_100 = (int)(0.9f * (float)100);	//	ratio in percentage

   ctorParams.params.LocalBundleAdjustmentSt.RunLocalBundleAdjustment = 0;
   ctorParams.params.LocalBundleAdjustmentSt.numberOfKeyframesForLBA = 10;

   ctorParams.params.LocalizationSt.RunRelocalization = 0;

   ctorParams.params.fps = 25;

   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SLAM_REF_TYPE);
}


static ERRG_codeE inu_graph__createXMLSLAM_Data(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_data__CtorParams ctorParams;
   inu_refH source;
   inu_graph__fillDataCtorDefault(graph, &ctorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SLAM_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLCVA_Data(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_cva_data__CtorParams ctorParams;
   inu_refH source;
   inu_cva_data__descriptor_t  XMLcvaDescriptor;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   
   XMLcvaDescriptor.bpp      = inu_sw_graph->cva_data.bpp;
   XMLcvaDescriptor.format   = inu_sw_graph->cva_data.format;
   XMLcvaDescriptor.height   = inu_sw_graph->cva_data.height;
   XMLcvaDescriptor.width    = inu_sw_graph->cva_data.width;
   XMLcvaDescriptor.kpNum    = inu_sw_graph->cva_data.kpNum;
   XMLcvaDescriptor.type     = inu_sw_graph->cva_data.type;
   memcpy(&ctorParams.cvaDescriptor, &XMLcvaDescriptor, sizeof(inu_cva_data__descriptor_t));
   
   ctorParams.dataCtorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_CVA_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLImage(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_image__CtorParams ctorParams;
   inu_refH source, image;
   inu_image__descriptor  XMLimageDescriptor;
   inu_image__hdr_t *sourceHdrP;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }

   if (inu_ref__getRefType((inu_ref*)source) == INU_SOC_CHANNEL_REF_TYPE)
   {
      image = inu_node__getNextOutputNode(source, NULL);//jump to image. open issue - if two images are created?
      sourceHdrP = (inu_image__hdr_t*)inu_data__getHdr((inu_dataH)image);
      XMLimageDescriptor = sourceHdrP->imgDescriptor;
   }
   else
   {
      XMLimageDescriptor.bitsPerPixel = inu_sw_graph->Data_obj_img.bitsPerPixel;
      XMLimageDescriptor.format = inu_sw_graph->Data_obj_img.format;
      XMLimageDescriptor.height = inu_sw_graph->Data_obj_img.height;
      XMLimageDescriptor.width = inu_sw_graph->Data_obj_img.width;
      XMLimageDescriptor.x = inu_sw_graph->Data_obj_img.x;
      XMLimageDescriptor.y = inu_sw_graph->Data_obj_img.y;
      XMLimageDescriptor.stride = inu_sw_graph->Data_obj_img.stride;
      XMLimageDescriptor.realBitsMask = inu_sw_graph->Data_obj_img.realBitsMask;
      XMLimageDescriptor.bufferHeight = inu_sw_graph->Data_obj_img.bufferHeight;
      XMLimageDescriptor.numInterleaveImages = inu_sw_graph->Data_obj_img.numInterleaveImages;
      XMLimageDescriptor.interMode = inu_sw_graph->Data_obj_img.interMode;
   }

   ctorParams.dataCtorParams.sourceNode.id = ((inu_ref*)source)->id;
   ctorParams.numLinesPerChunk = (UINT16)XMLimageDescriptor.bufferHeight; //TODO support chunk modes for none DMA image
   memcpy(&ctorParams.imageDescriptor, &XMLimageDescriptor, sizeof(inu_image__descriptor));
   sprintf(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_IMAGE_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLIMU(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_imu__CtorParams ctorParams;
   ERRG_codeE ret;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.odr[0] = (float)inu_sw_graph->params_val[1];
   ctorParams.scale[0] = (float)inu_sw_graph->params_val[2];
   ctorParams.accelBw[0] = inu_sw_graph->params_val[3];
   ctorParams.gyroBw[0] = inu_sw_graph->params_val[4];
   ctorParams.descriptor.shiftTs[0] = inu_sw_graph->params_val[5];
   ctorParams.batchSize[0] = inu_sw_graph->params_val[6];
   ctorParams.odr[1] = (float)inu_sw_graph->params_val[7];    //keep support for IMU's with different odr for
   ctorParams.scale[1] = (float)inu_sw_graph->params_val[8];
   ctorParams.accelBw[1] = inu_sw_graph->params_val[9];
   ctorParams.gyroBw[1] = inu_sw_graph->params_val[10];
   ctorParams.descriptor.shiftTs[1] = inu_sw_graph->params_val[11];
   ctorParams.batchSize[1] = inu_sw_graph->params_val[12];
   ctorParams.odr[2] = (float)inu_sw_graph->params_val[13];    
   ctorParams.scale[2] = (float)inu_sw_graph->params_val[14];
   ctorParams.accelBw[2] = inu_sw_graph->params_val[15];
   ctorParams.gyroBw[2] = inu_sw_graph->params_val[16];
   ctorParams.descriptor.shiftTs[2] = inu_sw_graph->params_val[17];
   ctorParams.batchSize[2] = inu_sw_graph->params_val[18];   
   ctorParams.IMU_paired_batching_batchSize= inu_sw_graph->params_val[19];     

   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_IMU_REF_TYPE);
   if (ERRG_SUCCEEDED(ret))
   {
      inu_imu__setDescriptor((inu_imuH)*inu_refP, &ctorParams.descriptor);
   }
   return ret;
}

static ERRG_codeE inu_graph__createXMLIMUData(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_refH source;
   inu_imu_data__CtorParams ctorParams;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.dataCtorParams.sourceNode.id = ((inu_node*)source)->ref.id;
   inu_imu__getDescriptor((inu_imuH)source, &ctorParams.descriptor);
   strcpy(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_IMU_DATA_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLTemperature(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_temperature__CtorParams ctorParams;
   ERRG_codeE ret;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.fps= inu_sw_graph->params_val[1];
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_TEMPERATURE_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLTemperatureData(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_refH source;
   inu_temperature_data__CtorParams ctorParams;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.dataCtorParams.sourceNode.id = ((inu_node*)source)->ref.id;
   strcpy(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_TEMPERATURE_DATA_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLHistogram(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   inu_histogram__CtorParams ctorParams;
   inu_graph__fillFunctionCtor(graph, &ctorParams.functionCtorParams, inu_sw_graph);
   ctorParams.engine = INU_HISTOGRAM__XM4;
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_HISTOGRAM_REF_TYPE);
}

static ERRG_codeE inu_graph__createXMLHistData(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_data__CtorParams ctorParams;
   inu_refH source;
   inu_graph__fillDataCtorDefault(graph, &ctorParams);
   ret = inu_graph__findNode(graph, inu_sw_graph->source, &source);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   ctorParams.sourceNode.id = ((inu_ref*)source)->id;
   strcpy(ctorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_HISTOGRAM_DATA_REF_TYPE);
   return ret;
}

static ERRG_codeE inu_graph__createXMLTsnrData(inu_graph* graph, inu_refH* inu_refP, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret;
   inu_tsnr_data__CtorParams ctorParams;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   strcpy(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, inu_sw_graph->func_data_name);
   ret = inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_TSNR_DATA_REF_TYPE);
   return ret;
}

// static HW graph creator
ERRG_codeE inu_graph__createCvaChannel(inu_graph *graph, inu_refH *inu_refP, unsigned int chId, const char *name)
{
   inu_cva_channel__CtorParams ctorParams;
   memset(&ctorParams,0,sizeof(ctorParams));
   ctorParams.nuCfgChId = chId;
   strcpy(ctorParams.configName,name);
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "%s_%d", "INU_CVA_CHANNEL", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_CVA_CHANNEL_REF_TYPE);
}


ERRG_codeE inu_graph__createCvaData(inu_graph *graph, inu_refH *inu_refP, inu_functionH sourceFunctionH, inu_cva_data__descriptor_t *cvaDescriptor, unsigned int chId)
{
   inu_cva_data__CtorParams ctorParams;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ctorParams.dataCtorParams.sourceNode.id = ((inu_node*)sourceFunctionH)->ref.id;
   memcpy(&ctorParams.cvaDescriptor, cvaDescriptor, sizeof(inu_cva_data__descriptor_t));
   sprintf(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, "CVA_DATA_%d", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_CVA_DATA_REF_TYPE);
}


ERRG_codeE inu_graph__createMipiChannel(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char *name)
{
   inu_mipi_channel__CtorParams ctorParams;
   memset(&ctorParams,0,sizeof(ctorParams));
   ctorParams.nuCfgChId = chId;
   strcpy(ctorParams.configName,name);
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "%s_%d", "INU_MIPI_CHANNEL", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_MIPI_CHANNEL_REF_TYPE);
}


ERRG_codeE inu_graph__createSocChannel(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char *name)
{
   inu_soc_channel__CtorParams ctorParams;
   memset(&ctorParams,0,sizeof(ctorParams));
   ctorParams.nuCfgChId = chId;
   strcpy(ctorParams.configName,name);
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   if (inu_graph__isExtInterleaveChannel(chId)) 
   {
       sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "%s_%d_EXT", "INU_SOC_CHANNEL", chId);
   }
   else
   {
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "%s_%d", "INU_SOC_CHANNEL", chId);
   }
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SOC_CHANNEL_REF_TYPE);
}

ERRG_codeE inu_graph__createIspChannel(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char* name)
{
    inu_soc_channel__CtorParams ctorParams;
    memset(&ctorParams, 0, sizeof(ctorParams));
    ctorParams.nuCfgChId = chId;
    strcpy(ctorParams.configName, name);
    inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
    sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "%s_%d", "INU_ISP_CHANNEL", chId);
    return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_ISP_CHANNEL_REF_TYPE);
}

ERRG_codeE inu_graph__createImage(inu_graph* graph, inu_refH* inu_refP, inu_functionH sourceFunctionH, inu_image__descriptor* imageDescriptor, unsigned int chId, UINT16 numLinesPerChunk)
{
   inu_image__CtorParams ctorParams;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ctorParams.dataCtorParams.sourceNode.id = ((inu_node*)sourceFunctionH)->ref.id;
   ctorParams.numLinesPerChunk = numLinesPerChunk;
   memcpy(&ctorParams.imageDescriptor, imageDescriptor, sizeof(inu_image__descriptor));
   sprintf(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, "IMAGE_%d", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_IMAGE_REF_TYPE);
}


ERRG_codeE inu_graph__createHistogram(inu_graph* graph, inu_refH* inu_refP, unsigned int chId, const char *name)
{
   inu_histogram__CtorParams ctorParams;
   memset(&ctorParams,0,sizeof(ctorParams));
   ctorParams.nuCfgChId = chId;
   strcpy(ctorParams.configName,name);
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "HISTOGRAM_%d", chId);
   ctorParams.engine = INU_HISTOGRAM__IAU_HW;
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_HISTOGRAM_REF_TYPE);
}

ERRG_codeE inu_graph__createHistData(inu_graph* graph, inu_refH* inu_refP, inu_functionH sourceFunctionH, unsigned int chId)
{
   inu_histogram_data__CtorParams ctorParams;
   inu_graph__fillDataCtorDefault(graph, &ctorParams.dataCtorParams);
   ctorParams.dataCtorParams.sourceNode.id = ((inu_node*)sourceFunctionH)->ref.id;
   sprintf(ctorParams.dataCtorParams.nodeCtorParams.ref_params.userName, "HISTOGRAMN_DATA_%d", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_HISTOGRAM_DATA_REF_TYPE);
}

ERRG_codeE inu_graph__createSensorGroup(inu_graph* graph, inu_refH* inu_refP, inu_sensor__parameter_list_t *masterParamsList, unsigned int chId)
{
   inu_sensors_group__CtorParams ctorParams;
   ctorParams.nuCfgChId = chId;
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   ctorParams.masterParamsList = *masterParamsList;
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "SENSOR_GROUP_%d", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SENSORS_GROUP_REF_TYPE);
}


ERRG_codeE inu_graph__createSensor(inu_graph* graph, inu_refH* inu_refP, inu_sensor__parameter_list_t *paramsList, unsigned int chId)
{
   inu_sensor__CtorParams ctorParams;
   ctorParams.params.id = (inu_sensor__id_e)chId;
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   memcpy(&ctorParams.params, paramsList, sizeof(inu_sensor__parameter_list_t));
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "SENSOR_%d", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SENSOR_REF_TYPE);
}


ERRG_codeE inu_graph__createWriter(inu_graph* graph, inu_refH* inu_refP, inu_image__descriptor* imageDescriptor, unsigned int chId, const char *name)
{
   inu_soc_writer__CtorParams ctorParams;
   memset(&ctorParams,0,sizeof(ctorParams));
   ctorParams.nuCfgChId = chId;
   strcpy(ctorParams.configName,name);
   inu_graph__fillFunctionCtorDefault(graph, &ctorParams.functionCtorParams);
   ctorParams.functionCtorParams.minInptsToOprt = 1;
   ctorParams.functionCtorParams.syncedFunc = 1;
   if(chId == HELSINKI_CVA_SPECIAL_WRITER_ID)
   {
      /*For writer 1 we need to create a worker thread so that the writer grant mechanism doesn't block the main graph thread*/
      ctorParams.functionCtorParams.workPriority = INU_FUNCTION__WORK_THR_PRIORITY_HIGH;
      LOGG_PRINT(LOG_INFO_E,NULL,"HELSINKI: Using worker thread for writer operate function \n");
   }
   memcpy(&ctorParams.imageDescriptor, imageDescriptor, sizeof(inu_image__descriptor));
   sprintf(ctorParams.functionCtorParams.nodeCtorParams.ref_params.userName, "WRITER_%d", chId);
   return inu_factory__new((inu_ref**)inu_refP, &ctorParams, INU_SOC_WRITER_REF_TYPE);
}


///XML parsing


static ERRG_codeE inu_graph__createXMLGraphFuncNode(inu_graph* graph, inu_refH * inu_ref, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;

   if (strstr(inu_sw_graph->func_data_type, "STREAM_OUT") ||
       strstr(inu_sw_graph->func_data_type, "STREAM_OUT_EXT_DMA"))
   {
      ret = inu_graph__createXMLStreamOut(graph, inu_ref, inu_sw_graph); 
   }
   else if (strstr(inu_sw_graph->func_data_type, "STREAM_IN"))
   {
      ret = inu_graph__createXMLStreamIn(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "FAST_ORB"))
   {
      ret = inu_graph__createXMLFastORB(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "CNN"))
   {
      ret = inu_graph__createXMLCNN(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "VISION_PROC"))
   {
      ret = inu_graph__createXMLVisionProc(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "DPE_PP"))
   {
      ret = inu_graph__createXMLDPE_PP(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "PP"))
   {
      ret = inu_graph__createXMLPP(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "SLAM"))
   {
      ret = inu_graph__createXMLSLAM(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "FDK"))
   {
      ret = inu_graph__createXMLFDK(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "SENSOR_CONTROL"))
   {
      ret = inu_graph__createXMLSensorControl(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "IMU"))
   {
      ret = inu_graph__createXMLIMU(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "TEMPERATURE"))
   {
      ret = inu_graph__createXMLTemperature(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "HISTOGRAM"))
   {
      ret = inu_graph__createXMLHistogram(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "TSNR_CALC"))
   {
      ret = inu_graph__createXMLTsnr_Calc(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "COMPRESS"))
   {
      ret = inu_graph__createXMLCompress(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "METADATA_INJECTOR"))
   {
      ret = inu_graph__createMetadataInjector(graph, inu_ref, inu_sw_graph);
   }
   else /* default: */
   {
      ret = INU_GRAPH__ERR_UNSUPPORTED_FUNCTION;
      printf("************* UNSUPPORTED FUNCTION = %s    \n", inu_sw_graph->func_data_type);
   }
   return ret;
}


static ERRG_codeE inu_graph__createXMLGraphDataNode(inu_graph* graph, inu_refH * inu_ref, inu_sw_graph__element_struct *inu_sw_graph)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;

   if (strstr(inu_sw_graph->func_data_type, "IMAGE_DATA"))
   {
      ret = inu_graph__createXMLImage(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "CNN_DATA"))
   {
      ret = inu_graph__createXMLCNN_Data(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "POINT_CLOUD_DATA"))
   {
      ret = inu_graph__createXMLPointCloud_Data(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "CVA_DATA"))
   {
      ret = inu_graph__createXMLCVA_Data(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "SLAM_DATA"))
   {
      ret = inu_graph__createXMLSLAM_Data(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "FAST_ORB_DATA"))
   {
      ret = inu_graph__createXMLFastORB_Data(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "RAW_DATA"))
   {
      ret = inu_graph__createXMLData(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "IMU_DATA"))
   {
      ret = inu_graph__createXMLIMUData(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "TEMPERATURE_DATA"))
   {
      ret = inu_graph__createXMLTemperatureData(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "HISTOGRAM_DATA"))
   {
      ret = inu_graph__createXMLHistData(graph, inu_ref, inu_sw_graph);
   }
   else if (strstr(inu_sw_graph->func_data_type, "TSNR_DATA"))
   {
      ret = inu_graph__createXMLTsnrData(graph, inu_ref, inu_sw_graph);
   }
   else /* default: */
   {
      ret = INU_GRAPH__ERR_UNSUPPORTED_DATA;
      printf("************* UNSUPPORTED DATA OBJECT  = %s    \n", inu_sw_graph->func_data_type);
   }

   return ret;
}


/******************************************************************************/


int convert_image_format_enum_to_num(XMLNode * img_para_node, inu_image__descriptor *Data_obj_img)
{
   int ret_val= NUCFG_FORMAT_GREY_16_E;
   char * node_tag, *node_text;

   node_tag = img_para_node->tag;
   node_text = img_para_node->text;
   if (strstr(node_tag, "format"))
   {
      if (strstr(node_text, "GREY_16"))
         ret_val = NUCFG_FORMAT_GREY_16_E;
      else if (strstr(node_text, "BAYER_16"))
         ret_val = NUCFG_FORMAT_BAYER_16_E;
      else if (strstr(node_text, "RGB888"))
         ret_val = NUCFG_FORMAT_RGB888_E;
      else if (strstr(node_text, "RGB666"))
         ret_val = NUCFG_FORMAT_RGB666_E;
      else if (strstr(node_text, "RGB565"))
         ret_val = NUCFG_FORMAT_RGB565_E;
      else if (strstr(node_text, "RGB555"))
         ret_val = NUCFG_FORMAT_RGB555_E;
      else if (strstr(node_text, "RGB444"))
         ret_val = NUCFG_FORMAT_RGB444_E;
      else if (strstr(node_text, "YUV420_8BIT"))
         ret_val = NUCFG_FORMAT_YUV420_8BIT_E;
      else if (strstr(node_text, "YUV420_8BIT_LEGACY"))
         ret_val = NUCFG_FORMAT_YUV420_8BIT_LEGACY_E;
      else if (strstr(node_text, "YUV420_10BIT"))
         ret_val = NUCFG_FORMAT_YUV420_10BIT_E;
      else if (strstr(node_text, "YUV422_8BIT"))
         ret_val = NUCFG_FORMAT_YUV422_8BIT_E;
      else if (strstr(node_text, "YUV422_10BIT"))
         ret_val = NUCFG_FORMAT_YUV422_10BIT_E;
      else if (strstr(node_text, "RAW6"))
         ret_val = NUCFG_FORMAT_RAW6_E;
      else if (strstr(node_text, "RAW7"))
         ret_val = NUCFG_FORMAT_RAW7_E;
      else if (strstr(node_text, "RAW8"))
         ret_val = NUCFG_FORMAT_RAW8_E;
      else if (strstr(node_text, "RAW10"))
         ret_val = NUCFG_FORMAT_RAW10_E;
      else if (strstr(node_text, "RAW12"))
         ret_val = NUCFG_FORMAT_RAW12_E;
      else if (strstr(node_text, "RAW14"))
         ret_val = NUCFG_FORMAT_RAW14_E;
      else if (strstr(node_text, "GEN_8"))
         ret_val = NUCFG_FORMAT_GEN_8_E;
      else if (strstr(node_text, "GEN_12"))
         ret_val = NUCFG_FORMAT_GEN_12_E;
      else if (strstr(node_text, "GEN_16"))
         ret_val = NUCFG_FORMAT_GEN_16_E;
      else if (strstr(node_text, "GEN_24"))
         ret_val = NUCFG_FORMAT_GEN_24_E;
      else if (strstr(node_text, "GEN_32"))
         ret_val = NUCFG_FORMAT_GEN_32_E;
      else if (strstr(node_text, "GEN_64"))
         ret_val = NUCFG_FORMAT_GEN_64_E;
      else if (strstr(node_text, "GEN_96"))
         ret_val = NUCFG_FORMAT_GEN_96_E;
      else if (strstr(node_text, "DEPTH"))
         ret_val = NUCFG_FORMAT_DEPTH_E;
      else if (strstr(node_text, "DISPARITY"))
         ret_val = NUCFG_FORMAT_DISPARITY_E;
      else if (strstr(node_text, "DISPARITY_DEBUG"))
         ret_val = NUCFG_FORMAT_DISPARITY_DEBUG_E;
      else if (strstr(node_text, "HISTOGRAM"))
         ret_val = NUCFG_FORMAT_HISTOGRAM_E;
      else if(strstr(node_text, "YUV420_SP"))
         ret_val = NUCFG_FORMAT_YUV420_SEMI_PLANAR_E;
      Data_obj_img->format = (NUCFG_formatE)ret_val;
   } // format
   else if (strcmp(node_tag, "height") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->height = ret_val;
   }
   else if (strcmp(node_tag, "width") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->width = ret_val;
   }
   else if (strcmp(node_tag, "x") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->x = ret_val;
   }
   else if (strcmp(node_tag, "y") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->y = ret_val;
   }
   else if (strcmp(node_tag, "stride") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->stride = ret_val;
   }
   else if (strcmp(node_tag, "bufferHeight") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->bufferHeight = ret_val;
   }
   else if (strcmp(node_tag, "realBitsMask") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->realBitsMask = ret_val;
   }
   else if (strcmp(node_tag, "bitsPerPixel") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->bitsPerPixel = ret_val;
   }
   else if (strcmp(node_tag, "bitsPerPixel") == 0)
   {
      ret_val = atoi(node_text);
      Data_obj_img->bitsPerPixel = ret_val;
   }
   else if ((strcmp(node_tag, "numInterleaveImages") == 0) || (strcmp(node_tag, "interleaveType") == 0))
   {
      // should convert the Enum to num - after those enums will defined
      //??????????????? TBD ???????????????????????????
      ret_val = atoi(node_text);
      if (strcmp(node_tag, "numInterleaveImages") == 0)
      {
         Data_obj_img->numInterleaveImages = ret_val;
      }
      else if (strcmp(node_tag, "interleaveType") == 0)
      {
         Data_obj_img->interMode = (NUCFG_interModeE)ret_val;
      }
   }
   else
   {
      ret_val = atoi(node_text);
   }
   return  ret_val;
}

//-----------------------------------------------------------------------
int convert_cva_format_enum_to_num(XMLNode * cva_para_node, inu_cva_data__descriptor_t *cva_data)
{
   int ret_val = NUCFG_FORMAT_GREY_16_E;
   char * node_tag, *node_text;

   node_tag = cva_para_node->tag;
   node_text = cva_para_node->text;
   if (strstr(node_tag, "format"))
   {
      if (strstr(node_text, "GREY_16"))
         ret_val = NUCFG_FORMAT_GREY_16_E;
      else if (strstr(node_text, "BAYER_16"))
         ret_val = NUCFG_FORMAT_BAYER_16_E;
      else if (strstr(node_text, "RGB888"))
         ret_val = NUCFG_FORMAT_RGB888_E;
      else if (strstr(node_text, "RGB666"))
         ret_val = NUCFG_FORMAT_RGB666_E;
      else if (strstr(node_text, "RGB565"))
         ret_val = NUCFG_FORMAT_RGB565_E;
      else if (strstr(node_text, "RGB555"))
         ret_val = NUCFG_FORMAT_RGB555_E;
      else if (strstr(node_text, "RGB444"))
         ret_val = NUCFG_FORMAT_RGB444_E;
      else if (strstr(node_text, "YUV420_8BIT"))
         ret_val = NUCFG_FORMAT_YUV420_8BIT_E;
      else if (strstr(node_text, "YUV420_8BIT_LEGACY"))
         ret_val = NUCFG_FORMAT_YUV420_8BIT_LEGACY_E;
      else if (strstr(node_text, "YUV420_10BIT"))
         ret_val = NUCFG_FORMAT_YUV420_10BIT_E;
      else if (strstr(node_text, "YUV422_8BIT"))
         ret_val = NUCFG_FORMAT_YUV422_8BIT_E;
      else if (strstr(node_text, "YUV422_10BIT"))
         ret_val = NUCFG_FORMAT_YUV422_10BIT_E;
      else if (strstr(node_text, "RAW6"))
         ret_val = NUCFG_FORMAT_RAW6_E;
      else if (strstr(node_text, "RAW7"))
         ret_val = NUCFG_FORMAT_RAW7_E;
      else if (strstr(node_text, "RAW8"))
         ret_val = NUCFG_FORMAT_RAW8_E;
      else if (strstr(node_text, "RAW10"))
         ret_val = NUCFG_FORMAT_RAW10_E;
      else if (strstr(node_text, "RAW12"))
         ret_val = NUCFG_FORMAT_RAW12_E;
      else if (strstr(node_text, "RAW14"))
         ret_val = NUCFG_FORMAT_RAW14_E;
      else if (strstr(node_text, "GEN_8"))
         ret_val = NUCFG_FORMAT_GEN_8_E;
      else if (strstr(node_text, "GEN_12"))
         ret_val = NUCFG_FORMAT_GEN_12_E;
      else if (strstr(node_text, "GEN_16"))
         ret_val = NUCFG_FORMAT_GEN_16_E;
      else if (strstr(node_text, "GEN_24"))
         ret_val = NUCFG_FORMAT_GEN_24_E;
      else if (strstr(node_text, "GEN_32"))
         ret_val = NUCFG_FORMAT_GEN_32_E;
      else if (strstr(node_text, "GEN_64"))
         ret_val = NUCFG_FORMAT_GEN_64_E;
      else if (strstr(node_text, "GEN_96"))
         ret_val = NUCFG_FORMAT_GEN_96_E;
      else if (strstr(node_text, "DEPTH"))
         ret_val = NUCFG_FORMAT_DEPTH_E;
      else if (strstr(node_text, "DISPARITY"))
         ret_val = NUCFG_FORMAT_DISPARITY_E;
      else if (strstr(node_text, "DISPARITY_DEBUG"))
         ret_val = NUCFG_FORMAT_DISPARITY_DEBUG_E;
      else if (strstr(node_text, "HISTOGRAM"))
         ret_val = NUCFG_FORMAT_HISTOGRAM_E;
      cva_data->format = (NUCFG_formatE)ret_val;
   } // format
   else if (strcmp(node_tag, "height") == 0)
   {
      ret_val = atoi(node_text);
      cva_data->height = ret_val;
   }
   else if (strcmp(node_tag, "width") == 0)
   {
      ret_val = atoi(node_text);
      cva_data->width = ret_val;
   }
   else if (strcmp(node_tag, "bpp") == 0)
   {
      ret_val = atoi(node_text);
      cva_data->bpp = ret_val;
   }
   else if ((strcmp(node_tag, "type") == 0))
   {
      if (strstr(node_text, "INU_CVA_DATA_INVALID"))
         ret_val = INU_CVA_DATA_INVALID;
      else if (strstr(node_text, "INU_CVA_DATA_DOG"))
         ret_val = INU_CVA_DATA_DOG;
      else if (strstr(node_text, "INU_CVA_DATA_DOG_FREAK"))
         ret_val = INU_CVA_DATA_DOG_FREAK;
      else if (strstr(node_text, "INU_CVA_DATA_DIFF"))
         ret_val = INU_CVA_DATA_DIFF;
      else if (strstr(node_text, "INU_CVA_DATA_NORM_EST"))
         ret_val = INU_CVA_DATA_NORM_EST;
      cva_data->type = (inu_cva_data__type_e)ret_val;
   }
   else
   {
      ret_val = atoi(node_text);
   }
   return  ret_val;
}

//-----------------------------------------------------------------------
int convert_connection_type_enum_to_num(char * text)
{
   inu_streamer__connIdE ret_val = INU_STREAMER__CONN_USB_IF0_E;
   if (strstr(text, "USB_IF0"))
      ret_val = INU_STREAMER__CONN_USB_IF0_E;
   else if (strstr(text, "USB_IF1"))
      ret_val = INU_STREAMER__CONN_USB_IF1_E;
   else if (strstr(text, "UART"))
      ret_val = INU_STREAMER__CONN_UART_E;
   else if (strstr(text, "TCP"))
      ret_val = INU_STREAMER__CONN_TCP_E;
   else if (strstr(text, "UDP"))
      ret_val = INU_STREAMER__CONN_UDP_E;
   else if (strstr(text, "CDC"))
      ret_val = INU_STREAMER__CONN_CDC_E;
   else if (strstr(text, "STUB"))
      ret_val = INU_STREAMER__CONN_STUB_E;
   else if (strstr(text, "UVC1"))
      ret_val = INU_STREAMER__CONN_UVC1_E;
   else if (strstr(text, "UVC2"))
      ret_val = INU_STREAMER__CONN_UVC2_E;
   else if (strstr(text, "UVC3"))
      ret_val = INU_STREAMER__CONN_UVC3_E;
   else if (strstr(text, "UVC4"))
      ret_val = INU_STREAMER__CONN_UVC4_E;
   else if (strstr(text, "USB_IF2"))
      ret_val = INU_STREAMER__CONN_USB_IF2_E;
   else if (strstr(text, "USB_IF3"))
      ret_val = INU_STREAMER__CONN_USB_IF3_E;
   else if (strstr(text, "USB_IF4"))
      ret_val = INU_STREAMER__CONN_USB_IF4_E;
   else if (strstr(text, "USB_IF5"))
      ret_val = INU_STREAMER__CONN_USB_IF5_E;
   return  (int)ret_val;
}


void parse_element(int i, XMLNode *root, inu_sw_graph__element_struct *inu_sw_graph)
{
   XMLNode *func_node, *feild_node, *params_list_node;
   char *feild_node_tag, *feild_node_text;
   int k,i_in,i_param;

   memset(inu_sw_graph, 0, sizeof(inu_sw_graph__element_struct));
   inu_sw_graph->num_inputs = 0;
   inu_sw_graph->num_params = 0;
   inu_sw_graph->num_hw_injects = 0;
   inu_sw_graph->func_pipe_size = MIN_FUNCTION_PIPE_SIZE;
   inu_sw_graph->func_input_que_depth = DEFAULT_INPUT_QUE_DEPTH;
   inu_sw_graph->func_min_inputs_oprt = DEFAULT_MIN_INPUTS_OPERATE;
   inu_sw_graph->mode = 0xFFFFFFFF; //default - work on all modes

   func_node = XMLNode_get_child(root, i);                     // node of function or data object
   inu_sw_graph->active = 1;
   if (strcmp(func_node->tag, "function") == 0)
      inu_sw_graph->element_type = INU_SW_GRAPH_FUNC_TYPE_E;
   else if (strcmp(func_node->tag, "data") == 0)
      inu_sw_graph->element_type = INU_SW_GRAPH_DATA_TYPE_E;

   for (k = 0; k < func_node->n_children; k++)
   {
      feild_node = XMLNode_get_child(func_node, k);
      if (feild_node->tag_type == TAG_COMMENT) continue;       // skip comments

      feild_node_tag = feild_node->tag;
      feild_node_text = feild_node->text;

      if (strcmp(feild_node_tag, "pipe_size") == 0)
      {
         inu_sw_graph->func_pipe_size = atoi(feild_node_text);
      }
      else if (strcmp(feild_node_tag, "mode") == 0)
      {
         inu_sw_graph->mode = strtol(feild_node_text, NULL , 16);
      }
      else if (strcmp(feild_node_tag, "type") == 0)
      {
         inu_sw_graph->func_data_type = feild_node_text;
      }
      else if (strcmp(feild_node_tag, "name") == 0)
      {
         inu_sw_graph->func_data_name = feild_node_text;
      }
      else if (strcmp(feild_node_tag, "hw_inject") == 0)   // valid for data obj
      {
         inu_sw_graph->HW_inject[inu_sw_graph->num_hw_injects] = feild_node_text;
         inu_sw_graph->num_hw_injects++;
      }
      else if (strcmp(feild_node_tag, "input") == 0)   // valid for data obj
      {
         if (inu_sw_graph->num_inputs >= MAX_FUNC_INPUTS)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Number of inputs exceeds maximum allowed inputs in data node. func_data_name = %s, input = %s, num_inputs = %d, Max inputs = %d\n", inu_sw_graph->func_data_name, feild_node_text, inu_sw_graph->num_inputs, MAX_FUNC_INPUTS);
            assert(0);
         }

         inu_sw_graph->func_data_inputs[inu_sw_graph->num_inputs] = feild_node_text;
         inu_sw_graph->num_inputs++;
      }
      else if (strcmp(feild_node_tag, "inputs") == 0)  // "inputs" valid for functions 
      {
         // check number of params , skip comments
         for (i_in = 0; i_in < feild_node->n_children; i_in++)
         {
            params_list_node = XMLNode_get_child(feild_node, i_in);
            if (params_list_node->tag_type != TAG_COMMENT)
            {
               if (strcmp(params_list_node->text, "NULL") != 0)
               {
                  if (inu_sw_graph->num_inputs >= MAX_FUNC_INPUTS)
                  {
                     LOGG_PRINT(LOG_ERROR_E, NULL, "Number of inputs exceeds maximum allowed inputs in function node. func_data_name = %s, input = %s, num_inputs = %d, Max inputs = %d\n", inu_sw_graph->func_data_name, params_list_node->text, inu_sw_graph->num_inputs, MAX_FUNC_INPUTS);
                     assert(0);
                  }

                  inu_sw_graph->func_data_inputs[inu_sw_graph->num_inputs] = params_list_node->text;
                  inu_sw_graph->num_inputs++;
               }
            }
         }
      }
      else if (strcmp(feild_node_tag, "params") == 0)
      {
         // check number of params , skip comments
         for (i_param = 0; i_param < feild_node->n_children; i_param++)
         {
            params_list_node = XMLNode_get_child(feild_node, i_param);
            if (params_list_node->tag_type != TAG_COMMENT)
            {
               inu_sw_graph->params_field[inu_sw_graph->num_params] = params_list_node->tag;
               if (strstr(inu_sw_graph->func_data_type, "IMAGE_DATA"))
               {
                  inu_sw_graph->params_val[inu_sw_graph->num_params] = convert_image_format_enum_to_num(params_list_node, &inu_sw_graph->Data_obj_img);
               }
               else if (strstr(inu_sw_graph->func_data_type, "CVA_DATA"))
               {
                  inu_sw_graph->params_val[inu_sw_graph->num_params] = convert_cva_format_enum_to_num(params_list_node, &inu_sw_graph->cva_data);
               }
               else if (strcmp(params_list_node->tag, "connIdE") == 0)
               {
                  inu_sw_graph->conn_id = convert_connection_type_enum_to_num(params_list_node->text);
               }
               else if (strcmp(params_list_node->tag, "thread_priority") == 0)
               {
                  inu_sw_graph->func_work_priority = atoi(params_list_node->text);;
               }
               else if (strcmp(params_list_node->tag, "input_que_depth") == 0)
               {
                  inu_sw_graph->func_input_que_depth = atoi(params_list_node->text);
                  printf("input_que_depth = %d\n", inu_sw_graph->func_input_que_depth);
               }
               else if (strcmp(params_list_node->tag, "min_inputs_to_oprt") == 0)
               {
                  inu_sw_graph->func_min_inputs_oprt = atoi(params_list_node->text);
                  printf("min_inputs_oprt = %d\n", inu_sw_graph->func_min_inputs_oprt);
               }
               else
               {
                  char *endptr;
                  intmax_t intval = strtoimax(params_list_node->text, &endptr, 0);//atoi(params_list_node->text);
                  inu_sw_graph->params_val[inu_sw_graph->num_params] = (int)intval;                  
               }
               inu_sw_graph->num_params++;
            }
         }
      }
      else if (strcmp(feild_node_tag, "source") == 0)
      {
         inu_sw_graph->source = feild_node_text;
      }
      else if (strcmp(feild_node_tag, "actionMap") == 0)
      {
         inu_sw_graph->actionMap = strtol(feild_node_text, NULL , 16);
      }
      else if (strcmp(feild_node_tag, "writerInst") == 0)
      {
         inu_sw_graph->offlinerWriterInst = strtol(feild_node_text, NULL , 16);
      }
      else // default:
      {
         printf(" UN expected feild, function  = %s , type=%s , text = %s \n ", inu_sw_graph->func_data_name, inu_sw_graph->func_data_type, feild_node_text);
      }
   }

}

/*Initialze Extended DMA Interleaving Info*/
ERRG_codeE inu_graph__setupExtIntInfo(inu_graph* graph, char* swGraphXMLpath)
{
    ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
    int i, k, j;

    if (graph == NULL || swGraphXMLpath == NULL)
        return INU_GRAPH__ERR_INVALID_ARGS;

    XMLDoc sw_graph_doc;
    XMLDoc_init(&sw_graph_doc);
    XMLDoc_parse_file_DOM(swGraphXMLpath, &sw_graph_doc);
    XMLNode* list_root, * inu_graph_root;
    int num_nodes = sw_graph_doc.n_nodes;   // number of nodes at the root
    if (!num_nodes)
        return INU_GRAPH__ERR_UNEXPECTED;
    inu_graph_root = sw_graph_doc.nodes[num_nodes - 1];
    list_root = XMLNode_get_child(inu_graph_root, 1);
    inu_sw_graph__element_struct inu_sw_graph_array[100];
    int num_elements = XMLNode_get_children_count(list_root);

    if (num_elements > 100)
        assert(0);

    for (i = 0; i < num_elements; i++)
    {
        parse_element(i, list_root, &inu_sw_graph_array[i]);
    }
#if 1
    for (i = 0; i < num_elements; i++)
    {
        if ((inu_sw_graph_array[i].element_type == INU_SW_GRAPH_FUNC_TYPE_E)
            && (strcmp(inu_sw_graph_array[i].func_data_type, "STREAM_OUT_EXT_DMA") == 0))
        {
            const char* func_name = inu_sw_graph_array[i].func_data_name;
            /*For extended DMA Interleaving mode, only accept 1 input such as IMAGE_0 */
            const char* func_input_channel = inu_sw_graph_array[i].func_data_inputs[0];
            const char* channel_num_str = strchr(func_input_channel, '_');
            UINT32 chId;

            if (channel_num_str == NULL)
                return INU_GRAPH__ERR_INVALID_ARGS;
            channel_num_str++;
            chId = atoi(channel_num_str);

            LOGG_PRINT(LOG_INFO_E, NULL, " %s with %s(INU_SOC_CHANNLE_%d) uses extended DMA Interleaving Mode!\n"
                , func_name, func_input_channel, chId);
            inu_graph__markExtInterleaveChannel(chId);
        }
    }
#endif
    return ret;
}

ERRG_codeE inu_graph__createPostProcessNodes(inu_graph* graph,char* graphXMLpath)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   int i, k1,k, j;

   XMLDoc sw_graph_doc;
   XMLDoc_init(&sw_graph_doc);
   XMLDoc_parse_file_DOM(graphXMLpath, &sw_graph_doc);
   XMLNode *list_root, *inu_graph_root;
   int num_nodes = sw_graph_doc.n_nodes;   // number of nodes at the root
   if (!num_nodes)
      return INU_GRAPH__ERR_UNEXPECTED;
   inu_graph_root = sw_graph_doc.nodes[num_nodes - 1];   
   list_root = XMLNode_get_child(inu_graph_root, 1);
   inu_sw_graph__element_struct inu_sw_graph_array[100];
   int num_elements = XMLNode_get_children_count(list_root);
   inu_refH inu_ref;
   inu_refH inu_ref_bind;

   if (num_elements > 100)
      assert(0);

   for (i = 0; i < num_elements; i++)
   {
      parse_element(i, list_root, &inu_sw_graph_array[i]);
   }

   // first pass - adjust graph if configuration has changed
   for (i = 0; i < num_elements; i++)
   {
      //histogram function creates histogram datas according to the 
      //number of interleaved images. User can request to modify interleaved
      //channel, therefor, the SW graph is changed
      if (strcmp(inu_sw_graph_array[i].func_data_type, "HISTOGRAM") == 0)
      {
         inu_image__hdr_t *hdrP;
         UINT32            total = 0;
         ret = inu_graph__findNode(graph, inu_sw_graph_array[i].func_data_inputs[0], &inu_ref_bind);
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to find Data obj input %s\n", inu_sw_graph_array[i].func_data_inputs[0]);
            return ret;
         }
         if (inu_ref__instanceOf(inu_ref_bind, INU_IMAGE_REF_TYPE))
         {
            hdrP = (inu_image__hdr_t*)inu_data__getHdr(inu_ref_bind);
         }
         else
         {
            //HW histogram graph, skip
            continue;
         }

         //count the output datas for this histogram
         for (j = 0; j < num_elements; j++)
         {
            if ((inu_sw_graph_array[j].num_inputs) && (strcmp(inu_sw_graph_array[j].func_data_inputs[0], inu_sw_graph_array[i].func_data_name) == 0))
            {
               total++;
               if (total > hdrP->imgDescriptor.numInterleaveImages)
               {
                  //remove this output
                  inu_sw_graph_array[j].active = 0;
               }
            }
         }
      }
   }

   // second pass - create nodes. data can bind to source
   for (i = 0; i < num_elements; i++)
   {
      if ((inu_sw_graph_array[i].element_type == INU_SW_GRAPH_FUNC_TYPE_E) && (inu_sw_graph_array[i].active))
      {
         ret = inu_graph__createXMLGraphFuncNode(graph, &inu_ref, &inu_sw_graph_array[i]);
         if (ERRG_SUCCEEDED(ret))
         {
            inu_graph__insertNode(graph, inu_ref);
         }
      }
      else if ((inu_sw_graph_array[i].element_type == INU_SW_GRAPH_DATA_TYPE_E) && (inu_sw_graph_array[i].active))
      {
         ret = inu_graph__createXMLGraphDataNode(graph, &inu_ref, &inu_sw_graph_array[i]);
         if (ERRG_SUCCEEDED(ret))
         {
            inu_graph__insertNode(graph, inu_ref);
            // for data object only one input  
            ret = inu_graph__findNode(graph, inu_sw_graph_array[i].func_data_inputs[0], &inu_ref_bind);
            if (ERRG_SUCCEEDED(ret)) ret = inu_node__bindNodes(inu_ref_bind, inu_ref);
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to create Data obj %s\n", inu_sw_graph_array[i].func_data_name);
         }
         // bind HW injects nodes
         for (k = 0; k < inu_sw_graph_array[i].num_hw_injects; k++)
         {
            ret = inu_graph__findNode(graph, inu_sw_graph_array[i].HW_inject[k], &inu_ref_bind);
            if (ERRG_SUCCEEDED(ret))
            {
               ret = inu_node__bindNodes(inu_ref, inu_ref_bind);
            }
         }
      }
   }

   //third pass - bind datas to functions
   for (i = 0; i < num_elements; i++)
   {
      ret = inu_graph__findNode(graph, inu_sw_graph_array[i].func_data_name, &inu_ref);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "Failed to find Data obj input %s\n", inu_sw_graph_array[i].func_data_inputs[0]);
         continue;
      }

      if (inu_sw_graph_array[i].element_type == INU_SW_GRAPH_FUNC_TYPE_E)
      {
         // check if the inputs nodes exist
         for (k1 = 0; k1 < inu_sw_graph_array[i].num_inputs; k1++)
         {
            ret = inu_graph__findNode(graph, inu_sw_graph_array[i].func_data_inputs[k1], &inu_ref_bind);
            if (ERRG_SUCCEEDED(ret))
            {
               ret = inu_node__bindNodes(inu_ref_bind, inu_ref);
            }
         }

         // check if the hw_inject nodes exist
         for (k1 = 0; k1 < inu_sw_graph_array[i].num_hw_injects; k1++)
         {
            ret = inu_graph__findNode(graph, inu_sw_graph_array[i].HW_inject[k1], &inu_ref_bind);
            if (ERRG_SUCCEEDED(ret))
            {
               ret = inu_node__bindNodes(inu_ref, inu_ref_bind);
            }
         }
      }
   }
   return ret;

}



#ifdef __cplusplus
}
#endif

