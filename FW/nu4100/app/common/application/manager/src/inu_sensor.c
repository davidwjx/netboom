#include "inu_sensor.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if DEFSG_IS_GP
#include "sensors_mngr.h"
#include "focus_ctrl.h"
#include "sequence_mngr.h"
#include "focus_ctrl.h"
#include "gme_drv.h"
#include "gme_mngr.h"
#include "iae_mngr.h"
#endif

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_SENSOR";

typedef struct
{
   IO_HANDLE sensorHandle;
   unsigned int nuCfgChId;
}inu_sensor__privData;


static const char* inu_sensor__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_sensor__dtor(inu_ref *me)
{
#if DEFSG_IS_GP
   inu_sensor__privData *privP = (inu_sensor__privData*)((inu_sensor*)me)->privP;
#endif
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   SENSORS_MNGRG_close(privP->sensorHandle);
   free(((inu_soc_channel*)me)->privP);
#endif
}

static int inu_sensor__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret = INU_SENSOR__RET_SUCCESS;
#if DEFSG_IS_GP
   inu_sensor__privData *privP = (inu_sensor__privData*)((inu_sensor*)me)->privP;
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
         ret = SENSORS_MNGRP_runtimeSetConfigSensor(privP->sensorHandle, (inu_sensor__runtimeCfg_t*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_SENSOR_FOCUS_GET_PARAMS):
      {
         ret = FOCUS_CTRLG_getFocusParams(privP->sensorHandle, (inu_sensor__getFocusParams_t*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_SENSOR_FOCUS_SET_PARAMS):
      {
         ret = FOCUS_CTRLG_setFocusParams(privP->sensorHandle, (inu_sensor__setFocusParams_t*)msgP);
         break;
      }

      case(INUG_IOCTL_SYSTEM_SENSOR_CROP_SET_PARAMS):
      {
         printf("INUG_IOCTL_SYSTEM_SENSOR_CROP_SET_PARAMS case in rxioctl inu_sensor, p=%p\n",privP->sensorHandle);
         ret = SENSORS_MNGRP_setCropParams(privP->sensorHandle, (inu_sensor__setCropParams_t*)msgP);
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


void inu_sensor__getSensorHandle(inu_sensor *me, IO_HANDLE *sensorHandle)
{
   inu_sensor__privData *privP = (inu_sensor__privData*)me->privP;
   *sensorHandle =  privP->sensorHandle;
}


/* Constructor */
ERRG_codeE inu_sensor__ctor(inu_sensor *me, inu_sensor__CtorParams *ctorParamsP)
{
   ERRG_codeE ret=INU_SENSOR__RET_SUCCESS;
   inu_sensor__privData *privP;
#if DEFSG_IS_GP
   IO_HANDLE ioHandle;
#endif
   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if(ERRG_SUCCEEDED(ret))
   {
      privP = (inu_sensor__privData*)malloc(sizeof(inu_sensor__privData));

      if (privP)
      {
         me->privP = privP;
         privP->nuCfgChId                 = ctorParamsP->params.id;
#if DEFSG_IS_GP
         INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "id = %d, function = %d, fps = %d, op_mode = %d\n", ctorParamsP->params.id, ctorParamsP->params.function, ctorParamsP->params.fps, ctorParamsP->params.op_mode);
         INUG_ioctlSensorConfigT setSensorCfg = {0};
         setSensorCfg.sensorSelect        = ctorParamsP->params.id;
         setSensorCfg.sensorFunc          = ctorParamsP->params.function;//INU_DEFSG_SEN_SLAVE_E;
         setSensorCfg.resolutionId        = ctorParamsP->params.op_mode;
         setSensorCfg.tableType           = ctorParamsP->params.tableType;
         setSensorCfg.fps                 = ctorParamsP->params.fps;
         setSensorCfg.powerFreqHz         = ctorParamsP->params.power_hz;
         setSensorCfg.defaultExp          = ctorParamsP->params.defaultExp;
         setSensorCfg.defaultGain         = ctorParamsP->params.gain.analog;
         setSensorCfg.totalExpTimePerSec  = ctorParamsP->params.totalExpTimePerSec;
         setSensorCfg.expTimePerDutyCycle = ctorParamsP->params.expTimePerDutyCycle;
         setSensorCfg.orientation         = ctorParamsP->params.orientation;
         setSensorCfg.sensorModel         = ctorParamsP->params.model;
         setSensorCfg.i2cNum              = ctorParamsP->params.i2cNum;
         setSensorCfg.powerGpioMaster     = ctorParamsP->params.power_gpio_master;
         setSensorCfg.powerGpioSlave      = ctorParamsP->params.power_gpio_slave;
         setSensorCfg.fsinGpio            = ctorParamsP->params.fsin_gpio;
         setSensorCfg.expMode             = ctorParamsP->params.exp_mode;
         setSensorCfg.sensorClkSrc        = ctorParamsP->params.sensor_clk_src + GME_DRVG_HW_CV0_REF_CLK_EN_E;
         setSensorCfg.sensorClkDiv        = ctorParamsP->params.sensor_clk_divider;
         setSensorCfg.strobeNum           = ctorParamsP->params.strobeNum;
         setSensorCfg.triggerSrc          = ctorParamsP->params.trigger_src;
         setSensorCfg.triggerDelay        = ctorParamsP->params.trigger_delay;
         setSensorCfg.groupId             = ctorParamsP->params.groupId;
         setSensorCfg.sensorId            = privP->nuCfgChId;
         setSensorCfg.isSensorGroup       = 0;
         setSensorCfg.senWidth            = ctorParamsP->params.sensor_width;
         setSensorCfg.senHeight           = ctorParamsP->params.sensor_height;

      // printf ("-------------- inu_sensor__ctor %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",setSensorCfg.sensorSelect,setSensorCfg.sensorFunc,setSensorCfg.resolutionId
      // ,setSensorCfg.fps,setSensorCfg.powerFreqHz,setSensorCfg.expMode,setSensorCfg.maxExposureTime,setSensorCfg.orientation,setSensorCfg.sensorModel,setSensorCfg.i2cNum,
      // setSensorCfg.powerGpioMaster,setSensorCfg.powerGpioSlave,setSensorCfg.fsinGpio,setSensorCfg.sensorClkSrc,setSensorCfg.sensorClkDiv);
         if(ERRG_SUCCEEDED(ret))
         {
             ret = SENSORS_MNGRG_open(&ioHandle,&setSensorCfg);
             if(ERRG_SUCCEEDED(ret))
             {
                privP->sensorHandle = ioHandle;
             }
         }
#endif
      }
   }
   return ret;
}

void inu_sensor__vtable_init( void )
{
   if (!_bool_vtable_initialized)
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_sensor__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_sensor__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_sensor__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_sensor__rxIoctl;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_sensor__vtable_get(void)
{
   inu_sensor__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

#if DEFSG_IS_GP
ERRG_codeE inu_sensor__getExposureGainInfo(inu_sensor *me, UINT32 context, UINT32 *exposureTimeP, float *digitalGainP, float *analogGainP)
{
   IO_HANDLE sensorHandle;
   ERRG_codeE ret;
   inu_sensor__getSensorHandle(me,&sensorHandle);
   ret = SENSORS_MNGRG_getSensorExpTime(sensorHandle, exposureTimeP, context);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = SENSORS_MNGRG_getSensorGain(sensorHandle, analogGainP, digitalGainP, context);
   }
   return ret;
}

ERRG_codeE  inu_sensor__getReadoutTs(inu_sensor *me, UINT64 *readOutTsP)
{
   IAE_MNGRG_iaeInfoT *iaeInfoP = IAE_MNGRG_getIaeInfo();
   *readOutTsP = iaeInfoP->sluFirstPixelTs[inu_sensor__getId(me)];
   return INU_SENSOR__RET_SUCCESS;
}
#endif

ERRG_codeE  inu_sensor__setRuntimeCfg(inu_sensorH meH, inu_sensor__runtimeCfg_t *cfgP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE  inu_sensor__getRuntimeCfg(inu_sensorH meH, inu_sensor__runtimeCfg_t *cfgP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE  inu_sensor__getFocusParams(inu_sensorH meH, inu_sensor__getFocusParams_t *paramsP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_FOCUS_GET_PARAMS, paramsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE  inu_sensor__setFocusParams(inu_sensorH meH, inu_sensor__setFocusParams_t *paramsP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_FOCUS_SET_PARAMS, paramsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE  inu_sensor__setCropParams(inu_sensorH meH, inu_sensor__setCropParams_t *paramsP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_CROP_SET_PARAMS, paramsP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

UINT32 inu_sensor__getId(inu_sensorH meH)
{
   return ((inu_sensor__privData*)((inu_sensor*)meH)->privP)->nuCfgChId;
}
