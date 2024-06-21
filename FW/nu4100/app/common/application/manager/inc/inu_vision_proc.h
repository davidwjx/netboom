#ifndef _INU_VISION_PROC_H_
#define _INU_VISION_PROC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "inu2.h"
#include "inu_function.h"
#include "inu2_types.h"

typedef struct
{
   inu_function                 function;

   void                        *privP;
}inu_vision_proc;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
}inu_vision_proc__CtorParams;

void inu_vision_proc__vtable_init(void);
const inu_function__VTable *inu_vision_proc__vtable_get(void);
void VISION_PROC_EVG_app(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);
#ifdef __cplusplus
}
#endif

#endif //__INU_VISION_PROC_H__

