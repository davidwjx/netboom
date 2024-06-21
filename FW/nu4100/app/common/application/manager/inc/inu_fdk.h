#ifndef _INU_FDK_H_
#define _INU_FDK_H_

#include "inu2.h"
#include "inu_function.h"
#include "inu2_types.h"

typedef struct
{
   inu_function                 function;

   void                        *privP;
}inu_fdk;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   UINT32                   nodeId;													//to get the callbacks
   UINT32                   numBuffs;												//if fdk will generate data, choose how many buffers to allocate
   UINT32                   buffSize;												//if fdk will generate data, size of each buffer
}inu_fdk__CtorParams;

void inu_fdk__vtable_init(void);
const inu_function__VTable *inu_fdk__vtable_get(void);

ERRG_codeE inu_fdk__invokeSensorIoctl( FDK_SENSOR_DRVG_ioctlListE ioctlNum, UINT16 groupId, UINT16 sensorId, void *argP );

#endif //__INU_FDK_H__
