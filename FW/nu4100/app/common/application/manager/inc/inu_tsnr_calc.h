#ifndef _INU_TSNR_CALC_H_
#define _INU_TSNR_CALC_H_

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inu_tsnr_calc
{
   inu_function function;
   void *privP;
}inu_tsnr_calc;

typedef struct
{
	inu_function__CtorParams functionCtorParams;
	inu_tsnr_calc__parameter_list_t params;
}inu_tsnr_calc__CtorParams;

void inu_tsnr_calc__vtable_init(void);
const inu_function__VTable *inu_tsnr_calc__vtable_get(void);

#ifdef __cplusplus
}
#endif
#endif //_INU_TSNR_CALC_H_

