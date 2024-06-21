#ifndef _INU_TSNR_DATA_H_
#define _INU_TSNR_DATA_H_

#include "inu_data.h"
#include "inu2_types.h"

struct inu_tsnr_data;

typedef struct inu_tsnr_data
{
   inu_data data;
   inu_tsnr_data__hdr_t  tsnr_dataHdr;
   void *privP;
}inu_tsnr_data;

typedef struct inu_tsnr_data_CtorParamsTag
{
   inu_data__CtorParams dataCtorParams;
}inu_tsnr_data__CtorParams;

void inu_tsnr_data__vtable_init(void);
const inu_data__VTable *inu_tsnr_data__vtable_get(void);

#endif

