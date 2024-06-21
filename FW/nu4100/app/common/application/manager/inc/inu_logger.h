#ifndef _INU_LOGGER_H_
#define _INU_LOGGER_H_

#include "inu2_types.h"

typedef struct inu_logger
{
   inu_ref ref;
   void *privP;
}inu_logger;

typedef struct
{
   inu_ref__CtorParams     ref_params;
   inu_log__configT        logConfig;
}inu_logger__CtorParams;

void inu_logger__vtable_init();
const inu_ref__VTable *inu_logger__vtable_get(void);

#endif

