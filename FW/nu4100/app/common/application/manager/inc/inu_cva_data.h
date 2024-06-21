#ifndef _INU_CVA_DATA_H_
#define _INU_CVA_DATA_H_

#include "inu_data.h"
#include "inu2_types.h"



struct inu_cva_data;

typedef struct inu_cva_data
{
   inu_data data;
   inu_cva_data__hdr_t cvaHdr;
   void *privP;
}inu_cva_data;

typedef struct inu_cva_data_CtorParamsTag
{
   inu_data__CtorParams 	  dataCtorParams;
   inu_cva_data__descriptor_t cvaDescriptor;
}inu_cva_data__CtorParams;

void inu_cva_data__vtable_init(void);
const inu_data__VTable *inu_cva_data__vtable_get(void);


#endif //_INU_CVA_DATA_H_