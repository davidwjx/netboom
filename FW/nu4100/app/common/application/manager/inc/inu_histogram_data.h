#ifndef _INU_HISTOGRAM_DATA_H_
#define _INU_HISTOGRAM_DATA_H_

#include "inu_data.h"
#include "inu2_types.h"

struct inu_histogram_data;

typedef struct inu_histogram_data
{
   inu_data data;
   inu_histogram_data__hdr_t  histogram_dataHdr;
   void *privP;
}inu_histogram_data;

typedef struct inu_histogram_data_CtorParamsTag
{
   inu_data__CtorParams dataCtorParams;
}inu_histogram_data__CtorParams;

void inu_histogram_data__vtable_init(void);
const inu_data__VTable *inu_histogram_data__vtable_get(void);

#endif

