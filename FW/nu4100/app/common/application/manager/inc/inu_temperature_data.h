#ifndef _INU_TEMPERATURE_DATA_H_
#define _INU_TEMPERATURE_DATA_H_

#include "inu_data.h"
#include "inu2_types.h"


struct inu_temperature_data;

typedef struct inu_temperature_data
{
   inu_data             data;
   inu_temperature_data__hdr_t tempHdr;
   void                 *privP;
}inu_temperature_data;

typedef struct inu_temperature_data_CtorParamsTag
{
   inu_data__CtorParams       dataCtorParams;
}inu_temperature_data__CtorParams;

void inu_temperature_data__vtable_init(void);
const inu_data__VTable *inu_temperature_data__vtable_get(void);

#endif

