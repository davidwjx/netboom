#ifndef _INU_COMPRESS_H_
#define _INU_COMPRESS_H_

#include "inu2.h"
#include "inu_function.h"

typedef struct
{
   inu_function                 function;
   void                        *privP;
}inu_compress;

typedef struct
{
   inu_function__CtorParams    functionCtorParams;
}inu_compress__CtorParams;

void inu_compress__vtable_init(void);
const inu_function__VTable *inu_compress__vtable_get(void);
#endif

