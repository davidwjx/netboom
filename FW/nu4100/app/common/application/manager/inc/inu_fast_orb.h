#ifndef _INU_FAST_ORB_H_
#define _INU_FAST_ORB_H_

#include "inu2.h"
#include "inu_function.h"
#include "inu2_types.h"

typedef struct
{
   inu_function                 function;

   void                        *privP;
}inu_fast_orb;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
}inu_fast_orb__CtorParams;

void inu_fast_orb__vtable_init(void);
const inu_function__VTable *inu_fast_orb__vtable_get(void);

#endif //__INU_FAST_ORB_H__
