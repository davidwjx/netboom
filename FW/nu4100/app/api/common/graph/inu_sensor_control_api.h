#ifndef _INU_SENSOR_CONTROL_API_H_
#define _INU_SENSOR_CONTROL_API_H_

#include "inu2.h"

typedef void* inu_sensor_controlH;

typedef enum
{
   INU_SENSOR_CONTROL__SW_MODE      = 0,
   INU_SENSOR_CONTROL__SENSOR_MODE  = 1,
   INU_SENSOR_CONTROL__ISP_MODE     = 2,
   INU_SENSOR_CONTROL__MODE_MAX     = 3,
} inu_sensor_control__operation_mode_e;
typedef struct
{
   inu_sensor_control__operation_mode_e operationMode;
   UINT32 nSatMax;
   UINT32 glSat;
   UINT32 mdMax;
   UINT32 mdMin;
   UINT32 exposureStepResolution;
   UINT32 deltaSatMax;
   UINT32 aggressiveStep;
   UINT32 noActivationPeriod;
   UINT32 exposureMax;
   UINT32 exposureMin;
   UINT32 debug;    /* enable debug prints */

   //for TSNR alg calc
   float snrTarget;
   float slopeWeight;

   UINT32 algVersion;
} inu_sensor_control__parameter_list_t;

typedef struct
{
   inu_deviceH deviceH;
   inu_sensor_control__parameter_list_t paramsList;
}inu_sensor_control__initParams;

typedef struct
{
   UINT32                                 exposureTimeUsec;
   inu_sensor__gain_t                     gain;
   float                                  ispGain;
   inu_sensor__sensorContext_e            context;
} inu_sensor_control__manualExposure_t;

#define INU_SENSOR_CONTROL__ROI_NUM (16)
typedef struct
{
   UINT32   x0;
   UINT32   x1;
   UINT32   y0;
   UINT32   y1;
   float    weight;
} inu_sensor_control__roi_t;

typedef struct
{
   UINT32                                 numOfRoiPoints;
   inu_sensor_control__roi_t              roi[INU_SENSOR_CONTROL__ROI_NUM];
} inu_sensor_control__roiCfg_t;

ERRG_codeE inu_sensor_control__updateParams(inu_sensor_controlH meH, inu_sensor_control__parameter_list_t *cfgParamsP);
ERRG_codeE inu_sensor_control__getParams(inu_sensor_controlH meH, inu_sensor_control__parameter_list_t *cfgParamsP);
ERRG_codeE inu_sensor_control__setManualExp(inu_sensor_controlH meH, inu_sensor_control__manualExposure_t *cfgParamsP);
ERRG_codeE inu_sensor_control__getManualExp(inu_sensor_controlH meH, inu_sensor_control__manualExposure_t* cfgParamsP);
ERRG_codeE inu_sensor_control__cfgRoi(inu_sensor_controlH meH, inu_sensor_control__roiCfg_t *cfgRoiParamsP);
ERRG_codeE inu_sensor_control__getOperationMode(inu_sensor_controlH meH, inu_sensor_control__operation_mode_e *operationMode);

#endif