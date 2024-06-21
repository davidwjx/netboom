#ifndef __INU_LOAD_BACKGROUND__H__
#define __INU_LOAD_BACKGROUND__H__

#include "inu2.h"
#include "inu_ref.h"
#include "inu_load_background_api.h"

#ifdef __cplusplus
extern "C" {
#endif


struct inu_load_background;

typedef struct inu_load_background_CtorParamsTag
{
   inu_ref__CtorParams ref_params; 
}inu_load_background__CtorParams;

typedef struct inu_load_background
{
   inu_ref ref;
   void    *privP;
}inu_load_background;


void inu_load_background__vtable_init();
const inu_ref__VTable *inu_load_background__vtable_get(void);

#ifdef __cplusplus
}
#endif

#endif //__INU_LOAD_BACKGROUND__H__



