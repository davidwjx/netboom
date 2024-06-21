#ifndef _INU_SOC_WRITER_H_
#define _INU_SOC_WRITER_H_

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inu_soc_writer
{
   inu_function function;
   void *privP;
}inu_soc_writer;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   unsigned int nuCfgChId;
   char         configName[128];
   inu_image__descriptor imageDescriptor;
}inu_soc_writer__CtorParams;


void inu_soc_writer__vtable_init( void );
const inu_function__VTable *inu_soc_writer__vtable_get(void);
ERRG_codeE inu_soc_writer__fillOutputChannel(inu_soc_writer *me, void *seqChH);



#ifdef __cplusplus
}
#endif
#endif //_INU_SOC_WRITER_H_

