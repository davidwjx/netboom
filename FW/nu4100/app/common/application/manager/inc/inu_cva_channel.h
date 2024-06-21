#ifndef _INU_CVA_CHANNEL_H_
#define _INU_CVA_CHANNEL_H_

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inu_cva_channel
{
   inu_function function;
   void *privP;
}inu_cva_channel;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   unsigned int nuCfgChId;
   char         configName[128];
}inu_cva_channel__CtorParams;

void inu_cva_channel__vtable_init(void);
const inu_function__VTable *inu_cva_channel__vtable_get(void);

void inu_cva__iicFrameDoneCb(void *userParams, void *arg);

#ifdef __cplusplus
}
#endif
#endif //_INU_CVA_CHANNEL_H_

