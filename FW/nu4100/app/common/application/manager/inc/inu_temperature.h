#ifndef _INU_TEMPERATURE_H_
#define _INU_TEMPERATURE_H_

#include "inu2.h"
#include "inu_function.h"

typedef struct
{
   inu_function   function;
   void           *privP;
}inu_temperature;

typedef struct
{
   inu_function__CtorParams   functionCtorParams;
   UINT32                     fps;
}inu_temperature__CtorParams;

void inu_temperature__vtable_init(void);
const inu_function__VTable *inu_temperature__vtable_get(void);
#endif

