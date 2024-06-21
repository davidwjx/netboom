#ifndef _INU_FAST_ORB_DATA_H_
#define _INU_FAST_ORB_DATA_H_

/* Features from Accelerated Segment Test */

#include "inu_data.h"
#include "inu2_types.h"

struct inu_fast_orb_data;

typedef struct inu_fast_orb_data
{
   inu_data data;
   inu_fast_orb_data__hdr_t  fast_orb_dataHdr;
   void *privP;
}inu_fast_orb_data;

typedef struct inu_fast_orb_data_CtorParamsTag
{
   inu_data__CtorParams dataCtorParams;
}inu_fast_orb_data__CtorParams;

void inu_fast_orb_data__vtable_init(void);
const inu_data__VTable *inu_fast_orb_data__vtable_get(void);

#endif

