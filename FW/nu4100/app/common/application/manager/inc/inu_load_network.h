#ifndef __INU_LOAD_NETWORK__H__
#define __INU_LOAD_NETWORK__H__

#include "inu2.h"
#include "inu_ref.h"
#include "inu_load_network_api.h"

#ifdef __cplusplus
extern "C" {
#endif


struct inu_load_network;

typedef struct inu_load_network_CtorParamsTag
{
   inu_ref__CtorParams ref_params; 
   UINT32              initWorkSpace;
}inu_load_network__CtorParams;

typedef struct inu_load_network
{
   inu_ref ref;
   void    *privP;
}inu_load_network;

typedef struct 
{
   UINT32 isNetworkLoaded;
   UINT32 engineType;
}CDNN_GPP_networksT;


void inu_load_network__vtable_init();
const inu_ref__VTable *inu_load_network__vtable_get(void);
UINT32 inu_load_network__get_isNetworkLoaded(inu_ref *ref, UINT32 netId);

#ifdef __cplusplus
}
#endif

#endif //__INU_DEVICE__H__

