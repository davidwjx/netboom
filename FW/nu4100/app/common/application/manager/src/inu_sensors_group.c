#include "inu2_types.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_sensors_group.h"
#include "log.h"

#if DEFSG_IS_GP
#include "sensors_mngr.h"
#include "sequence_mngr.h"
#include "gme_drv.h"
#include "gme_mngr.h"
#include "hcg_mngr.h"
#endif


typedef struct inu_sensors_group__privData
{
   unsigned int nuCfgChId;
   IO_HANDLE sensorHandle;
}inu_sensors_group__privData;

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_SENSOR_GROUP";

static const char* inu_sensors_group__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_sensors_group__dtor(inu_ref *me)
{
   inu_sensors_group__privData *privP = (inu_sensors_group__privData*)((inu_sensors_group*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   SENSORS_MNGRG_close(privP->sensorHandle);
#endif
   free(privP);
}

static int inu_sensors_group__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret = INU_SENSOR__RET_SUCCESS;
#if DEFSG_IS_GP
   inu_sensors_group__privData *privP = (inu_sensors_group__privData*)((inu_sensors_group*)me)->privP;
   inu_sensor *sensor = NULL;
#endif
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
#if DEFSG_IS_GP
   switch (msgCode)
   {
      case(INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG):
      {
         ret = SENSORS_MNGRP_runtimeGetConfigSensor(privP->sensorHandle, (inu_sensor__runtimeCfg_t*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG):
      {
         inu_sensor__runtimeCfg_t *runTimeCfgP = (inu_sensor__runtimeCfg_t*)msgP;
         HCG_MNGRG_startPrimePeriod();
         ret = SENSORS_MNGRP_runtimeSetConfigSensor(privP->sensorHandle, runTimeCfgP);

         if(ERRG_SUCCEEDED(ret))
         {
            //need to update all sensors now
            do
            {
               sensor = inu_node__getNextOutputNode(me,sensor);
               if (sensor)
               {
                  IO_HANDLE sensorHandle;
                  inu_sensor__getSensorHandle(sensor,&sensorHandle);
                  SENSORS_MNGRG_updateSensorExpTime(sensorHandle,runTimeCfgP->exposure,runTimeCfgP->context);
                  SENSORS_MNGRG_updateSensorGain(sensorHandle,runTimeCfgP->gain.analog,runTimeCfgP->gain.digital,runTimeCfgP->context);
               }
            }while(sensor);
         }
         break;
      }
      default:
      {
         break;
      }
   }
#endif

   return ret;
}


/* Constructor */
ERRG_codeE inu_sensors_group__ctor(inu_sensors_group *me, inu_sensors_group__CtorParams *ctorParamsP)
{
#if DEFSG_IS_GP
   IO_HANDLE ioHandle;
#endif
   ERRG_codeE ret;
   inu_sensors_group__privData *privP;
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);

   if(ERRG_SUCCEEDED(ret))
   {
      privP = (inu_sensors_group__privData*)malloc(sizeof(inu_sensors_group__privData));
      if(privP)
      {
         memset(privP, 0, sizeof(inu_sensors_group__privData));
         me->privP = privP;
         privP->nuCfgChId = ctorParamsP->nuCfgChId;
#if DEFSG_IS_GP
         me->privP = privP;
         INUG_ioctlSensorConfigT setSensorCfg = { 0 };
         setSensorCfg.sensorSelect = ctorParamsP->masterParamsList.id;
         setSensorCfg.sensorFunc = ctorParamsP->masterParamsList.function;
         setSensorCfg.resolutionId = ctorParamsP->masterParamsList.op_mode;
         setSensorCfg.tableType = ctorParamsP->masterParamsList.tableType;
         setSensorCfg.fps = ctorParamsP->masterParamsList.fps;
         setSensorCfg.powerFreqHz = ctorParamsP->masterParamsList.power_hz;
         setSensorCfg.defaultExp= ctorParamsP->masterParamsList.defaultExp;
         setSensorCfg.defaultGain= ctorParamsP->masterParamsList.gain.analog;
         setSensorCfg.totalExpTimePerSec = ctorParamsP->masterParamsList.totalExpTimePerSec;
         setSensorCfg.expTimePerDutyCycle = ctorParamsP->masterParamsList.expTimePerDutyCycle;
         setSensorCfg.orientation = ctorParamsP->masterParamsList.orientation;
         setSensorCfg.sensorModel = ctorParamsP->masterParamsList.model;
         setSensorCfg.i2cNum = ctorParamsP->masterParamsList.i2cNum;
         setSensorCfg.powerGpioMaster = ctorParamsP->masterParamsList.power_gpio_master;
         setSensorCfg.powerGpioSlave = ctorParamsP->masterParamsList.power_gpio_slave;
         setSensorCfg.fsinGpio = ctorParamsP->masterParamsList.fsin_gpio;
         setSensorCfg.expMode = ctorParamsP->masterParamsList.exp_mode;
         setSensorCfg.sensorClkSrc = ctorParamsP->masterParamsList.sensor_clk_src + GME_DRVG_HW_CV0_REF_CLK_EN_E;
         setSensorCfg.sensorClkDiv = ctorParamsP->masterParamsList.sensor_clk_divider;
         setSensorCfg.strobeNum = ctorParamsP->masterParamsList.strobeNum;
         setSensorCfg.triggerSrc= ctorParamsP->masterParamsList.trigger_src;
         setSensorCfg.triggerDelay = ctorParamsP->masterParamsList.trigger_delay;
         setSensorCfg.projector_inst = ctorParamsP->masterParamsList.projector_inst;
         setSensorCfg.groupId = privP->nuCfgChId;
         setSensorCfg.sensorId = 0xFFFF;
         setSensorCfg.senWidth  = ctorParamsP->masterParamsList.sensor_width;
         setSensorCfg.senHeight= ctorParamsP->masterParamsList.sensor_height;
         setSensorCfg.isSensorGroup = 1;
         ret = SENSORS_MNGRG_open(&ioHandle, &setSensorCfg);
         if (ERRG_SUCCEEDED(ret))
         {
            privP->sensorHandle = ioHandle;
         }
#endif
      }
      else
      {
         ret = INU_SENSORS_GROUP__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}


void inu_sensors_group__vtable_init( void )
{
   if (!_bool_vtable_initialized)
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_sensors_group__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_sensors_group__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_sensors_group__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_sensors_group__rxIoctl;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_sensors_group__vtable_get(void)
{
   inu_sensors_group__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

#if DEFSG_IS_GP
ERRG_codeE inu_sensors_group__getSensorHandle(inu_sensors_group *me, IO_HANDLE *sensorHandle)
{
   ERRG_codeE ret = INU_SENSORS_GROUP__RET_SUCCESS;

   if ((me != NULL) && (me->privP != NULL))
   {
      inu_sensors_group__privData *privP = (inu_sensors_group__privData*)me->privP;
      *sensorHandle = privP->sensorHandle;
   }
   else
   {
      ret = INU_SENSORS_GROUP__ERR_NULL_PTR;
   }
   return ret;
}

void *inu_sensors_group__getAltHandle(inu_sensors_group *me)
{
   inu_sensors_group__privData *privP = (inu_sensors_group__privData*)me->privP;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT*)privP->sensorHandle;
   return sensorInfoP->altH;
}


ERRG_codeE inu_sensors_group__getExposureGainInfo(inu_sensors_group *me, UINT32 context, UINT32 *exposureTimeP, float *digitalGainP, float *analogGainP)
{
   IO_HANDLE sensorHandle;
   ERRG_codeE ret;
   ret = inu_sensors_group__getSensorHandle(me,&sensorHandle);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = SENSORS_MNGRG_getSensorExpTime(sensorHandle, exposureTimeP, context);
      if (ERRG_SUCCEEDED(ret))
      {
         ret = SENSORS_MNGRG_getSensorGain(sensorHandle, analogGainP, digitalGainP, context);
      }
   }
   return ret;
}

ERRG_codeE  inu_sensors_group__getStrobeData(inu_sensors_group *me, UINT64 *timestampP, UINT64 *counterP, UINT32 *projMode)
{
   IO_HANDLE sensorHandle;
   ERRG_codeE ret;
   ret = inu_sensors_group__getSensorHandle(me, &sensorHandle);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = SENSORS_MNGRG_getStrobeData(sensorHandle, timestampP, counterP, projMode);
   }
   return ret;
}


UINT16 inu_sensors_group__getPixelClock(inu_sensors_group *me)
{
   inu_sensors_group__privData *privP = (inu_sensors_group__privData*)me->privP;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT*)privP->sensorHandle;
   return SENSORS_MNGRG_getSensorPixelClock(sensorInfoP->sensorHandle);
}
#endif

UINT32 inu_sensors_group__getId(inu_sensors_groupH meH)
{
   return ((inu_sensors_group__privData*)((inu_sensors_group*)meH)->privP)->nuCfgChId;
}

ERRG_codeE  inu_sensors_group__setRuntimeCfg(inu_sensors_groupH meH, inu_sensor__runtimeCfg_t *cfgP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

//this api is somehow problematic. for example this flow:
//Set 500 exp to sensor 0
//Set 1000 exp to sensor 1
//Get sensor grouo exposure, what value should return?
ERRG_codeE  inu_sensors_group__getRuntimeCfg(inu_sensors_groupH meH, inu_sensor__runtimeCfg_t *cfgP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}


