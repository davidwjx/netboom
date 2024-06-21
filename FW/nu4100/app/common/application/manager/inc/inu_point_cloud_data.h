#ifndef _INU_POINT_CLOUD_DATA_H_
#define _INU_POINT_CLOUD_DATA_H_


#include "inu_data.h"
#include "inu2_types.h"

struct inu_point_cloud_data;

typedef struct inu_point_cloud_data
{
   inu_data data;
   inu_point_cloud_data__hdr_t  point_cloud_dataHdr;
   void *privP;
}inu_point_cloud_data;

typedef struct inu_point_cloud_data_CtorParamsTag
{
   inu_data__CtorParams dataCtorParams;
}inu_point_cloud_data__CtorParams;

void inu_point_cloud_data__vtable_init(void);
const inu_data__VTable *inu_point_cloud_data__vtable_get(void);

#endif

