#ifndef _INU_STREAMER_H_
#define _INU_STREAMER_H_

#include "inu_function.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   INU_STREAMER__IN = 0,
   INU_STREAMER__OUT
}inu_streamer__direction_e;


typedef struct inu_streamer
{
   inu_function function;

   void *privP;
}inu_streamer;

typedef struct
{
   inu_function__CtorParams  functionCtorParams;
   inu_streamer__direction_e streamDirection; 
   inu_streamer__connIdE     connId;
   UINT32                    numBuffers;
}inu_streamer__CtorParams;

void inu_streamer__vtable_init(void);
const inu_function__VTable *inu_streamer__vtable_get(void);

#ifdef __cplusplus
}
#endif
#endif //_INU_STREAMER_H_

