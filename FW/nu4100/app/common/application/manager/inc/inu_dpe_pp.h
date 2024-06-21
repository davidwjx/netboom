#ifndef _INU_DPE_PP_H_
#define _INU_DPE_PP_H_

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
}inu_dpe_pp;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
}inu_dpe_pp__CtorParams;

void inu_dpe_pp__vtable_init(void);
const inu_function__VTable *inu_dpe_pp__vtable_get(void);
void DPE_PP_CEVAG_app(void *jobDescriptorP, UINT16 jobDescriptor);
void DPE_PP_EVG_app(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);

#ifdef __cplusplus
}
#endif

#endif //__INU_DPE_PP_H__


