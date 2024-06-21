#ifndef _INU_DSO_FDK_H_
#define _INU_DSO_FDK_H_

#include "inu2.h"
#include "inu_function.h"
#include "inu2_types.h"

typedef struct
{
	inu_function                 function;

	void                        *privP;
}inu_dso_fdk;

typedef struct
{
	inu_function__CtorParams functionCtorParams;
	char                     pluginName[INU_DSO_FDK__MAX_NAME_STRING_LEN];
}inu_dso_fdk__CtorParams;

void inu_dso_fdk__vtable_init(inu_ref *me);

#endif //__INU_DSO_FDK_H__
