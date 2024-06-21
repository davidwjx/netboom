#ifndef _INU_MIPI_CHANNEL_H_
#define _INU_MIPI_CHANNEL_H_

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inu_mipi_channel
{
   inu_function function;
   void *privP;
}inu_mipi_channel;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   unsigned int nuCfgChId;
   char         configName[128];
}inu_mipi_channel__CtorParams;

void inu_mipi_channel__vtable_init(void);
const inu_function__VTable *inu_mipi_channel__vtable_get(void);

#ifdef __cplusplus
}
#endif
#endif //_INU_MIPI_CHANNEL_H_

