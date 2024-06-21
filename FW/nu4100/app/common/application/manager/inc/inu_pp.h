#ifndef _INU_PP_H_
#define _INU_PP_H_

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
}inu_pp;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   INU_PP__actionE          actionMap;
   inu_pp__parameter_list_t params;
}inu_pp__CtorParams;


void inu_pp__vtable_init(void);
const inu_function__VTable *inu_pp__vtable_get(void);
void PP_CEVAG_app(void *jobDescriptorP, UINT16 jobDescriptor);
void PP_EVG_app(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);


#ifdef __cplusplus
}
#endif

#endif //__INU_PP_H__


