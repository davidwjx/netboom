#ifndef _INU_ISP_CHANNEL_H_
#define _INU_ISP_CHANNEL_H_

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct inu_isp_channel
{
   inu_function function;
   void *privP;
}inu_isp_channel;

typedef struct
{
   unsigned int cmd;
   unsigned int cmdParams[32];
}inu_isp_channel__commandT;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   unsigned int nuCfgChId;
   char         configName[128];
}inu_isp_channel__CtorParams;

void inu_isp_channel__vtable_init(void);
const inu_function__VTable *inu_isp_channel__vtable_get(void);

#ifdef __cplusplus
}
#endif
#endif //_INU_ISP_CHANNEL_H_

