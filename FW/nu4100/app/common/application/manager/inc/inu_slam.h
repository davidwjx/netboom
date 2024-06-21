#ifndef _INU_SLAM_H_
#define _INU_SLAM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "inu2.h"
#include "inu_function.h"
#include "inu2_types.h"

typedef struct
{
   inu_function                 function;

   void                        *privP;
}inu_slam;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   inu_slam__parameter_list_t params;
}inu_slam__CtorParams;

void inu_slam__vtable_init(void);
const inu_function__VTable *inu_slam__vtable_get(void);
void SLAM_CEVAG_app(void *jobDescriptorP, UINT16 jobDescriptor);

#ifdef __cplusplus
}
#endif

#endif //__INU_SLAM_H__

