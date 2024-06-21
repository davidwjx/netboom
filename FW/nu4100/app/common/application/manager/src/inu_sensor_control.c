#ifdef ENABLE_SENSOR_CONTROL
#include "inu_sensor_control.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_common.h"

#if DEFSG_IS_GP
#ifdef ENABLE_SENSOR_CONTROL
#include "sensor_control.h"
#endif
#include "isp_mngr.hpp"
#endif

#define USEC_TO_SEC (1000000UL)

typedef struct inu_sensors_control__privData
{
#if DEFSG_IS_GP
#ifdef ENABLE_SENSOR_CONTROL
   SENSORS_CONTROLG_handleT  algH;
   inu_sensor_control__parameter_list_t params;
#else
   inu_sensor_control__operation_mode_e operationMode;
   #endif
#endif
}inu_sensor_control__privData;


static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_SENSOR_CONTROL";

static const char* inu_sensor_control__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_sensor_control__dtor(inu_ref *me)
{
#if DEFSG_IS_GP
   inu_sensor_control__privData *privP = (inu_sensor_control__privData*)((inu_sensor_control*)me)->privP;
#endif
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
#ifdef ENABLE_SENSOR_CONTROL
   if (privP->algH)
   {
      SENSORS_CTRLG_close(privP->algH);
   }
#endif
#endif
}

static int inu_sensor_control__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
#if DEFSG_IS_GP
#ifdef ENABLE_SENSOR_CONTROL
   inu_sensor_control__privData *privP = (inu_sensor_control__privData*)((inu_sensor_control*)me)->privP;
   switch( msgCode )
   {
      case(INTERNAL_CMDG_SENSOR_CONTROL_UPDATE_E):
      {
         SENSORS_CTRLG_updateParams(privP->algH, (inu_sensor_control__parameter_list_t*)msgP);
      }break;
      case(INTERNAL_CMDG_SENSOR_CONTROL_GET_PARAMS_E):
      {
         SENSORS_CTRLG_getParams(privP->algH, (inu_sensor_control__parameter_list_t*)msgP);
      }break;
      default:
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "invalid msgCode %d\n",msgCode);
      }break;
   }
#endif
#endif
   return ret;
}


/* Constructor */
ERRG_codeE inu_sensor_control__ctor(inu_sensor_control *me, inu_sensor_control__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_sensor_control__privData *privP;
   ctorParamsP->functionCtorParams.workPriority = INU_FUNCTION__WORK_THR_PRIORITY_LOW;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_sensor_control__privData*)malloc(sizeof(inu_sensor_control__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_sensor_control__privData));
         me->privP = privP;
#if DEFSG_IS_GP
#ifdef ENABLE_SENSOR_CONTROL
         memcpy(&privP->params,&ctorParamsP->params,sizeof(inu_sensor_control__parameter_list_t));
#else
         privP->operationMode = ctorParamsP->params.operationMode;
#endif
#endif
      }
      else
      {
         ret = INU_SENSOR_CONTROL__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

#if DEFSG_IS_GP
static ERRG_codeE inu_sensor_control__changeSensorExpMode(inu_function *me, INU_DEFSG_sensorExpModeE expMode)
{
   inu_nodeH sensorGroupP;
   IO_HANDLE sensorHandle;
   ERRG_codeE ret;

   ret = inu_graph__getOrigSensGroup(me, &sensorGroupP);

   if (ERRG_SUCCEEDED(ret))
   {
      inu_nodeH sensor = NULL;
      //need to update all sensors now
      do
      {
         sensor = inu_node__getNextOutputNode(sensorGroupP, sensor);
         if (sensor)
         {
            IO_HANDLE sensorHandle;
            inu_sensor__getSensorHandle(sensor, &sensorHandle);
            SENSORS_CTRLG_changeExpMode(sensorHandle, expMode);
         }
      }while(sensor);
   }

   return ret;
}
//#else
static ERRG_codeE inu_sensor_control__changeIspExpMode(inu_function *me, INU_DEFSG_sensorExpModeE expMode)
{
   ERRG_codeE ret;
   inu_isp_channel__ispCmdParamU ispCmdParam;
   inu_node *node;
   char imageName[MAX_NAME_LEN];
   unsigned int ispRdNum = 0;

   node = (inu_node*)inu_node__getNextInputNode(me,NULL);

   if (node && (inu_ref__getRefType(node) == INU_IMAGE_REF_TYPE))
   {
      if (node && sscanf( inu_ref__getUserName(node), "IMAGE_%d", &ispRdNum) != 1)
      {
         return INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
      }
      ISP_MNGRG_setEnableAeByRdNum(ispRdNum, (UINT8)expMode);
   }
   else
   {
      ret = INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
   }

   return ret;
}
#endif //DEFSG_IS_GP

static void inu_sensor_control__operate(inu_function *me, inu_function__operateParamsT *paramsP)
{
#if DEFSG_IS_GP
   inu_sensor_control__privData *privP = (inu_sensor_control__privData*)((inu_sensor_control*)me)->privP;
   UINT32 i;
#ifdef ENABLE_SENSOR_CONTROL
   if (privP->params.operationMode == INU_SENSOR_CONTROL__SW_MODE)
   {
      SENSORS_CTRLG_operate(privP->algH,paramsP);
   }
#endif
   for(i = 0; i < paramsP->dataInputsNum; i++)
   {
      inu_function__doneData(me,paramsP->dataInputs[i]);
   }
   inu_function__complete(me);
#else
   FIX_UNUSED_PARAM_WARN(paramsP);
   FIX_UNUSED_PARAM_WARN(me);
#endif
}

static ERRG_codeE inu_sensor_control__start(inu_function *me, inu_function__startParamsT *startParamsP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_start(me, startParamsP);
#if DEFSG_IS_GP
   if (ERRG_SUCCEEDED(ret))
   {
      #ifdef ENABLE_SENSOR_CONTROL
      inu_sensor_control__privData *privP = (inu_sensor_control__privData*)((inu_sensor_control*)me)->privP;

      switch (privP->params.operationMode)
      {
         case (INU_SENSOR_CONTROL__SW_MODE):
         {
            if (!privP->algH)
            {
               inu_nodeH sensorGroupP;

               ret = inu_graph__getOrigSensGroup(me, &sensorGroupP);
               if (ERRG_SUCCEEDED(ret))
               {
                  ret = SENSORS_CTRLG_open(&privP->algH, &privP->params, sensorGroupP, ffs(inu_function__getOperateModes(me)) - 1);
               }
               else//for injection case
               {
                  ret = SENSORS_CTRLG_open(&privP->algH, &privP->params, NULL, ffs(inu_function__getOperateModes(me)) - 1);
               }
            }
            if (ERRG_SUCCEEDED(ret))
            {
               ret = SENSORS_CTRLG_start(privP->algH);
            }
         }
         break;

         case (INU_SENSOR_CONTROL__SENSOR_MODE):
         {
            ret = inu_sensor_control__changeSensorExpMode(me, INU_DEFSG_AE_AUTO_E);
         }
         break;

         case (INU_SENSOR_CONTROL__ISP_MODE):
         {
            ret = inu_sensor_control__changeIspExpMode(me, INU_DEFSG_AE_AUTO_E);
         }
         break;
      }
      #endif
   }
#endif

   return ret;
}

static ERRG_codeE inu_sensor_control__stop(inu_function *me, inu_function__stopParamsT *stopParamsP)
{
   ERRG_codeE ret;
   ret = inu_function__vtable_get()->p_stop(me, stopParamsP);
#ifdef ENABLE_SENSOR_CONTROL
#if DEFSG_IS_GP
   if (ERRG_SUCCEEDED(ret))
   {
      inu_sensor_control__privData *privP = (inu_sensor_control__privData*)((inu_sensor_control*)me)->privP;

      if (privP->params.operationMode == INU_SENSOR_CONTROL__SW_MODE)
      {
         ret = SENSORS_CTRLG_stop(privP->algH);
      }
      else if (privP->params.operationMode == INU_SENSOR_CONTROL__SENSOR_MODE)
      {
         ret = inu_sensor_control__changeSensorExpMode(me, INU_DEFSG_AE_MANUAL_E);
      }
      else if (privP->params.operationMode == INU_SENSOR_CONTROL__ISP_MODE)
      {
         ret = inu_sensor_control__changeIspExpMode(me, INU_DEFSG_AE_MANUAL_E);
      }
   }
#endif
#endif
   return ret;
}

void inu_sensor_control__vtable_init( void )
{
   if (!_bool_vtable_initialized)
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_sensor_control__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_sensor_control__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_sensor_control__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_sensor_control__rxIoctl;

      _vtable.p_operate = inu_sensor_control__operate;
      _vtable.p_start   = inu_sensor_control__start;
      _vtable.p_stop    = inu_sensor_control__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_sensor_control__vtable_get(void)
{
   inu_sensor_control__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

ERRG_codeE inu_sensor_control__updateParams(inu_sensor_controlH meH, inu_sensor_control__parameter_list_t *cfgParamsP)
{
   return inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_SENSOR_CONTROL_UPDATE_E, cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


ERRG_codeE inu_sensor_control__getParams(inu_sensor_controlH meH, inu_sensor_control__parameter_list_t *cfgParamsP)
{
   ERRG_codeE ret = INU_SENSOR_CONTROL__RET_SUCCESS;

   inu_sensor_control__privData* privP = (inu_sensor_control__privData*)((inu_sensor_control*)meH)->privP;
   if (privP)
   {
      inu_sensor_control__operation_mode_e operationMode;
#if DEFSG_IS_HOST
      operationMode = privP->operationMode;
#else
      operationMode = privP->params.operationMode;
#endif //DEFSG_IS_HOST
      if (operationMode == INU_SENSOR_CONTROL__SW_MODE)
      {
         ret = inu_ref__sendCtrlSync((inu_ref*)meH,INTERNAL_CMDG_SENSOR_CONTROL_GET_PARAMS_E,cfgParamsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
      }
      else if ((operationMode == INU_SENSOR_CONTROL__SENSOR_MODE) || (operationMode == INU_SENSOR_CONTROL__ISP_MODE))
      {
         memset(cfgParamsP, 0, sizeof(inu_sensor_control__parameter_list_t));
         cfgParamsP->operationMode = operationMode;
      }

   }
   else
   {
      ret = INU_SENSOR_CONTROL__ERR_OUT_OF_MEM;
   }

   return ret;
}

ERRG_codeE inu_sensor_control__setManualExp(inu_sensor_controlH meH, inu_sensor_control__manualExposure_t *cfgParamsP)
{
   ERRG_codeE ret = INU_SENSOR_CONTROL__RET_SUCCESS;
#if DEFSG_IS_HOST
   inu_nodeH sensorGroupP;
   inu_sensor__runtimeCfg_t runtimeCfg = { 0, 0.0, 0.0, 0, (inu_sensor__sensorContext_e)0};
   inu_isp_channel__ispCmdParamU ispCmdParam = {0};

   ret = inu_graph__getOrigSensGroup(meH, &sensorGroupP);

   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error! Sensor Group doesn't contain Sensor Control Node [0x%x]\n", ret);
      return INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
   }


   inu_sensor_control__privData* privP = (inu_sensor_control__privData*)((inu_sensor_control*)meH)->privP;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "cfgParamsP->operationMode [%d] \n", privP->operationMode);

   switch (privP->operationMode)
   {
   case INU_SENSOR_CONTROL__SW_MODE:
   case INU_SENSOR_CONTROL__SENSOR_MODE:
      runtimeCfg.exposure = cfgParamsP->exposureTimeUsec;
      runtimeCfg.gain = cfgParamsP->gain;
      runtimeCfg.context = cfgParamsP->context;

      ret = inu_sensors_group__setRuntimeCfg(sensorGroupP, &runtimeCfg);

      break;

   case INU_SENSOR_CONTROL__ISP_MODE:
      ispCmdParam.exposureParam.integrationTime = ((float)(cfgParamsP->exposureTimeUsec)/USEC_TO_SEC);
      ispCmdParam.exposureParam.analogGain      = cfgParamsP->gain.analog;
      ispCmdParam.exposureParam.ispGain         = cfgParamsP->ispGain;

      ret = inu_isp_channel__sendIspCommand(sensorGroupP, INU_ISP_COMMAND_SET_EXPOSURE_E, &ispCmdParam);
      break;

   default:
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error! operationMode [%d] is not supported\n", privP->operationMode);
      ret = INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
      break;
   }
#endif
   return ret;
}

ERRG_codeE inu_sensor_control__getManualExp(inu_sensor_controlH meH, inu_sensor_control__manualExposure_t* cfgParamsP)
{
   ERRG_codeE ret = INU_SENSOR_CONTROL__RET_SUCCESS;
#if DEFSG_IS_HOST
   inu_nodeH sensorGroupP;
   inu_sensor__runtimeCfg_t runtimeCfg = { 0, 0.0, 0.0, 0, (inu_sensor__sensorContext_e)0 };
   inu_isp_channel__ispCmdParamU ispCmdParam = { 0 };

   ret = inu_graph__getOrigSensGroup(meH, &sensorGroupP);

   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error! Sensor Group doesn't contain Sensor Control Node [0x%x]\n", ret);
      return INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
   }


   inu_sensor_control__privData* privP = (inu_sensor_control__privData*)((inu_sensor_control*)meH)->privP;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "cfgParamsP->operationMode [%d] \n", privP->operationMode);

   switch (privP->operationMode)
   {
   case INU_SENSOR_CONTROL__SW_MODE:
   case INU_SENSOR_CONTROL__SENSOR_MODE:
      ret = inu_sensors_group__getRuntimeCfg(sensorGroupP, &runtimeCfg);

      if (ERRG_SUCCEEDED(ret))
      {
         cfgParamsP->exposureTimeUsec = runtimeCfg.exposure;
         cfgParamsP->gain = runtimeCfg.gain;
         cfgParamsP->context = runtimeCfg.context;
         cfgParamsP->ispGain = (float)0.0;
      }
      break;

   case INU_SENSOR_CONTROL__ISP_MODE:
      ret = inu_isp_channel__sendIspCommand(sensorGroupP, INU_ISP_COMMAND_GET_EXPOSURE_E, &ispCmdParam);
      
      if (ERRG_SUCCEEDED(ret))
      {
         cfgParamsP->exposureTimeUsec = (UINT32)(ispCmdParam.exposureParam.integrationTime * USEC_TO_SEC);
         cfgParamsP->gain.analog = ispCmdParam.exposureParam.analogGain;
         cfgParamsP->gain.digital = 0;
         cfgParamsP->ispGain = ispCmdParam.exposureParam.ispGain;
      }
      break;

   default:
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error! operationMode [%d] is not supported\n", privP->operationMode);
      ret = INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
      break;
   }
#endif
   return ret;
}

static ERRG_codeE inu_sensor_control__cfgRoiSwMode(inu_sensor_controlH meH, inu_sensor_control__roiCfg_t* cfgRoiParamsP)
{
   ERRG_codeE ret = INU_SENSOR_CONTROL__RET_SUCCESS;
#if DEFSG_IS_HOST
   inu_histogram_dataH histData;
   inu_histogramH histFunc;
   inu_histogram__roiCfgT roiCfg;
   UINT8 i = 0;

   for (i = 0; i < cfgRoiParamsP->numOfRoiPoints; i++)
   {
      roiCfg.roi[i].x0 = cfgRoiParamsP->roi[i].x0;
      roiCfg.roi[i].x1 = cfgRoiParamsP->roi[i].x1;
      roiCfg.roi[i].y0 = cfgRoiParamsP->roi[i].y0;
      roiCfg.roi[i].y1 = cfgRoiParamsP->roi[i].y1;
   }

   histData = inu_node__getNextInputNode(meH, NULL);
   while (histData)
   {
      if (inu_ref__instanceOf(histData, INU_HISTOGRAM_DATA_REF_TYPE))
      {
         histFunc = inu_node__getNextInputNode(histData, NULL);
         ret = inu_histogram__cfgRoi(histFunc, &roiCfg);
      }
      histData = inu_node__getNextInputNode(meH, histData);
   }
#endif
   return ret;
}

ERRG_codeE inu_sensor_control__cfgRoi(inu_sensor_controlH meH, inu_sensor_control__roiCfg_t *cfgRoiParamsP)
{
   ERRG_codeE ret = INU_SENSOR_CONTROL__RET_SUCCESS;
#if DEFSG_IS_HOST
   inu_sensor_control__privData* privP = (inu_sensor_control__privData*)((inu_sensor_control*)meH)->privP;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "cfgParamsP->operationMode [%d] \n", privP->operationMode);

   switch (privP->operationMode)
   {
   case INU_SENSOR_CONTROL__SW_MODE:
      if ((cfgRoiParamsP->numOfRoiPoints > INU_HISTOGRAM__ROI_NUM) || (cfgRoiParamsP->numOfRoiPoints == 0))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Number of ROI points [%d] is invalid, maximum allowed for SW_MODE [%d] \n", cfgRoiParamsP->numOfRoiPoints, INU_HISTOGRAM__ROI_NUM);
         ret = INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
      }
      else
      {
         ret = inu_sensor_control__cfgRoiSwMode(meH, cfgRoiParamsP);
      }
      break;

   case INU_SENSOR_CONTROL__SENSOR_MODE:
   case INU_SENSOR_CONTROL__ISP_MODE:
      LOGG_PRINT(LOG_INFO_E, NULL, "Sensor Control Mode [%d] is not supported\n", privP->operationMode);
      ret = INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
      break;

   default:
      ret = INU_SENSOR_CONTROL__ERR_NOT_SUPPORTED;
      break;
   }
#endif
   return ret;
}

ERRG_codeE inu_sensor_control__getOperationMode(inu_sensor_controlH meH, inu_sensor_control__operation_mode_e *operationMode)
{
   ERRG_codeE ret = INU_SENSOR_CONTROL__RET_SUCCESS;

   inu_sensor_control__privData* privP = (inu_sensor_control__privData*)((inu_sensor_control*)meH)->privP;
   if (privP)
   {
#if DEFSG_IS_HOST
      *operationMode = privP->operationMode;
#else
      * operationMode = privP->params.operationMode;
#endif //DEFSG_IS_HOST
      LOGG_PRINT(LOG_INFO_E, NULL, "Operation mode [%d]\n", *operationMode);
   }
   else
   {
      ret = INU_SENSOR_CONTROL__ERR_OUT_OF_MEM;
   }
   return ret;
}
#endif