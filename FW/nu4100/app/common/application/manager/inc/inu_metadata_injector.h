#pragma once

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct 
{
   inu_function function;
   void *privP;
}inu_metadata_injector;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   INT32                   readerID;
}inu_metadata_injector__CtorParams;

void inu_metadata_injector__vtable_init(void);
const inu_function__VTable *inu_metadata_injector__vtable_get(void);

#ifdef __cplusplus
}
#endif

