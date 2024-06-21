#ifndef _INU_SENSORS_GROUP_H_
#define _INU_SENSORS_GROUP_H_

#include "inu2_internal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   inu_function                 function;

   void                        *privP;
}inu_sensors_group;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   unsigned int nuCfgChId;
   inu_sensor__parameter_list_t masterParamsList;
}inu_sensors_group__CtorParams;

void inu_sensors_group__vtable_init(void);
const inu_function__VTable *inu_sensors_group__vtable_get(void);
ERRG_codeE inu_sensors_group__getSensorHandle(inu_sensors_group *me, IO_HANDLE *sensorHandle);
#if DEFSG_IS_GP
void *inu_sensors_group__getAltHandle(inu_sensors_group *me);
ERRG_codeE inu_sensors_group__getExposureGainInfo(inu_sensors_group *me, UINT32 context, UINT32 *exposureTimeP, float *digitalGainP, float *analogGainP);
ERRG_codeE inu_sensors_group__getStrobeData(inu_sensors_group *me, UINT64 *timestampP, UINT64 *counterP, UINT32 *projMode);
UINT16 inu_sensors_group__getPixelClock(inu_sensors_group *me);
#endif
#ifdef __cplusplus
}
#endif

#endif