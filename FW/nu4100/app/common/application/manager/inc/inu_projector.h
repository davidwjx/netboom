#ifndef _INU_PROJECTOR_H_
#define _INU_PROJECTOR_H_

#include "inu2_internal.h"

typedef struct
{
   inu_function                 function;
   inu_projector__parameter_list_t params;

   void                        *privP;
}inu_projector;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   inu_projector__parameter_list_t params;
}inu_projector__CtorParams;

void inu_projector__vtable_init( void );
const inu_function__VTable *inu_projector__vtable_get(void);
void inu_projector__getProjectorHandle(inu_projector * meH, IO_HANDLE *projectorHandle);

#endif //__INU_PROJECTOR_H__
