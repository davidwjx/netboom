#include "inu_projector.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if DEFSG_IS_GP
#include "projectors_mngr.h"
#include "sequence_mngr.h"
#endif

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_PROJECTOR";

typedef struct
{
   IO_HANDLE projectorHandle;
   unsigned int nuCfgChId;
}inu_projector__privData;


static const char* inu_projector__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_projector__dtor(inu_ref *me)
{
#if DEFSG_IS_GP  
   inu_projector__privData *privP = (inu_projector__privData*)((inu_projector*)me)->privP;
#endif
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_GP   
   PROJECTORS_MNGRG_close(privP->projectorHandle);
   free(((inu_soc_channel*)me)->privP);   
#endif   
}

static int inu_projector__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret = INU_SENSOR__RET_SUCCESS;
#if DEFSG_IS_GP
   inu_projector__privData *privP = (inu_projector__privData*)((inu_projector*)me)->privP;
#endif
   ret = inu_function__vtable_get()->node_vtable.ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
#if DEFSG_IS_GP
   switch (msgCode)
   {
      default:
      {
         break;
      }
   }
#endif
   return ret;
}


void inu_projector__getProjectorHandle(inu_projector *me, IO_HANDLE *projectorHandle)
{
   inu_projector__privData *privP = (inu_projector__privData*)me->privP;
   *projectorHandle =  privP->projectorHandle;
}


/* Constructor */
ERRG_codeE inu_projector__ctor(inu_projector *me, inu_projector__CtorParams *ctorParamsP)
{
   ERRG_codeE ret=INU_SENSOR__RET_SUCCESS;
   inu_projector__privData *privP;
#if DEFSG_IS_GP
   IO_HANDLE ioHandle;
#endif

   ret = inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   if(ERRG_SUCCEEDED(ret))
   {
      privP = (inu_projector__privData*)malloc(sizeof(inu_projector__privData));
      
      if (privP)
      {
         me->privP = privP;
         privP->nuCfgChId                 = ctorParamsP->params.id;
#if DEFSG_IS_GP
         INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL, "id = %d, model = %d, type = %d, state = %d\n", ctorParamsP->params.id, ctorParamsP->params.model, ctorParamsP->params.type, ctorParamsP->params.state);
         INUG_ioctlProjectorConfigT setProjectorCfg = {0};
         setProjectorCfg.model = ctorParamsP->params.model;
         setProjectorCfg.type= ctorParamsP->params.type;
         setProjectorCfg.state= ctorParamsP->params.state;

         if(ERRG_SUCCEEDED(ret))
         {
             ret = PROJECTORS_MNGRG_open(&ioHandle,&setProjectorCfg);
             if(ERRG_SUCCEEDED(ret))
             {
                privP->projectorHandle = ioHandle;
             }
         } 
#endif
      }
   }
   return ret;
}

void inu_projector__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_projector__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_projector__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_projector__ctor;
      _vtable.node_vtable.ref_vtable.p_rxSyncCtrl = inu_projector__rxIoctl;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_projector__vtable_get(void)
{
   inu_projector__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}

/*
ERRG_codeE  inu_projector__setRuntimeCfg(inu_sensorH meH, inu_sensor__runtimeCfg_t *cfgP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}

ERRG_codeE  inu_projector__getRuntimeCfg(inu_sensorH meH, inu_sensor__runtimeCfg_t *cfgP )
{
   return inu_ref__sendCtrlSync((inu_ref*)meH, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG, cfgP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
}
*/
UINT32 inu_projector__getId(inu_projectorH meH)
{
   return ((inu_projector__privData*)((inu_projector*)meH)->privP)->nuCfgChId;
}
