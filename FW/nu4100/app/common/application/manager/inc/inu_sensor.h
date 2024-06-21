#ifndef _INU_SENSOR_H_
#define _INU_SENSOR_H_

#include "inu2_internal.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	inu_function                 function;
	inu_sensor__parameter_list_t params;

	void                        *privP;
}inu_sensor;

typedef struct
{
	inu_function__CtorParams functionCtorParams;
	inu_sensor__parameter_list_t params;
}inu_sensor__CtorParams;

void inu_sensor__vtable_init( void );
const inu_function__VTable *inu_sensor__vtable_get(void);
void inu_sensor__getSensorHandle(inu_sensor * meH, IO_HANDLE *sensorHandle);
#if DEFSG_IS_GP
ERRG_codeE inu_sensor__getExposureGainInfo(inu_sensor *me, UINT32 context, UINT32 *exposureTimeP, float *digitalGainP, float *analogGainP);
ERRG_codeE inu_sensor__getReadoutTs(inu_sensor *me, UINT64 *readOutTsP);
#endif
#ifdef __cplusplus
}
#endif

#endif //__INU_SENSOR_H__
