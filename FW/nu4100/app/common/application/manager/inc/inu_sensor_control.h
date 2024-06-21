#ifndef _INU_SENSOR_CONTROL_H_
#define _INU_SENSOR_CONTROL_H_

#include "inu2_internal.h"

typedef struct
{
   inu_function                 function;
   void                        *privP;
}inu_sensor_control;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   inu_sensor_control__parameter_list_t params;
}inu_sensor_control__CtorParams;

void inu_sensor_control__vtable_init( void );
const inu_function__VTable *inu_sensor_control__vtable_get(void);

#endif //_INU_SENSOR_CONTROL_H_