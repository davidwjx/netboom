#ifndef _INU_CDNN_DATA_H_
#define _INU_CDNN_DATA_H_


#include "inu_data.h"
#include "inu2_types.h"

struct inu_cdnn_data;

typedef struct inu_cdnn_data
{
   inu_data data;
   inu_cdnn_data__hdr_t  cdnn_dataHdr;
   void *privP;
}inu_cdnn_data;

typedef struct inu_cdnn_data_CtorParamsTag
{
   inu_data__CtorParams dataCtorParams;
}inu_cdnn_data__CtorParams;

void inu_cdnn_data__vtable_init(void);
const inu_data__VTable *inu_cdnn_data__vtable_get(void);

#endif

