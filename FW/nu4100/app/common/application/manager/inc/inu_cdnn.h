#ifndef _INU_CDNN_H_
#define _INU_CDNN_H_

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
}inu_cdnn;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
}inu_cdnn__CtorParams;

void inu_cdnn__vtable_init(void);
const inu_function__VTable *inu_cdnn__vtable_get(void);
void CDNN_EVG_app(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);
#ifdef __cplusplus
}
#endif

#endif //__INU_CDNN_H__
