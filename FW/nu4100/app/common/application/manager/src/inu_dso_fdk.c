#if 0
#include "inu_dso_fdk.h"
#include "inu2_types.h"
#include "inu_types.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "mem_pool.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#if DEFSG_IS_GP
#include <dlfcn.h>
#include "inu_graph.h"
#endif

#define INU_DSO_FDK__FULL_PATH "/media/inuitive/"
#define INU_DSO_FDK__PREFIX_LIB "lib"
#define INU_DSO_FDK__POSTFIX_SO ".so"



typedef struct inu_dso_fdk__privData
{
   inu_dso_fdk__ctor_func_t       *ctorFunc;
   inu_dso_fdk__dtor_func_t       *dtorFunc;
   inu_dso_fdk__setState_func_t   *setStateFunc;
   inu_dso_fdk__operate_func_t    *operateFunc;
   inu_dso_fdk__rxData_func_t     *rxDataFunc;
   inu_dso_fdk__txDataDone_func_t *txDataDoneFunc;
   char                       pluginName[INU_DSO_FDK__MAX_NAME_STRING_LEN];
}inu_dso_fdk__privData;

static ERRG_codeE inu_dso_fdk__loadPlugin(inu_dso_fdk *me);
static void inu_dso_fdk__complete(inu_dso_fdkH fdkH);
static void inu_dso_fdk__forward_data(inu_dso_fdkH fdkH, inu_dataH dataH);
static void inu_dso_fdk__new_data(inu_dso_fdkH fdkH, inu_dataH dataH);



static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_dso_fdk__name(inu_ref *me)
{
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)((inu_dso_fdk*)me)->privP;
   return privP->pluginName;
}

static void inu_dso_fdk__dtor(inu_ref *me)
{
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)((inu_dso_fdk*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   if (privP->dtorFunc)
   {
      privP->dtorFunc(me);
   }
   free(((inu_dso_fdk*)me)->privP);
}

/* Constructor */
void inu_dso_fdk__ctor(inu_dso_fdk *me, inu_dso_fdk__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_dso_fdk__privData *privP;
   inu_function__ctor(&me->function, &ctorParamsP->functionCtorParams);
   privP = (inu_dso_fdk__privData*)malloc(sizeof(inu_dso_fdk__privData));
   memset(privP,0,sizeof(inu_dso_fdk__privData));
   me->privP = privP;
   memcpy(privP->pluginName, ctorParamsP->pluginName, sizeof(privP->pluginName)); //keep null terminating char
#if DEFSG_IS_GP
   ret = inu_dso_fdk__loadPlugin(me);

   if (ERRG_FAILED(ret))
   {
      printf("error finding symbols, exit\n");
      return;
   }

   if (privP->ctorFunc)
   {
      inu_dso_fdk__callbacks_p_t cbs;
      cbs.operate_complete_cb_p = inu_dso_fdk__complete;
      cbs.new_data_cb_p         = inu_dso_fdk__new_data;
      cbs.done_data_cb_p        = inu_dso_fdk__forward_data;
      privP->ctorFunc(me, cbs);
   }
#endif
}

static int inu_dso_fdk__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   FIX_UNUSED_PARAM_WARN(msgCode);
   FIX_UNUSED_PARAM_WARN(msgP);
   FIX_UNUSED_PARAM_WARN(bufDescP);
   //todo - keep buffer until release by the fdk
   //todo - check hdr, pass hdr? validate the frame ctr?
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)((inu_dso_fdk*)me)->privP;
   if (privP->rxDataFunc)
   {
      privP->rxDataFunc(me, dataP, dataLen);
   }
   return 0;
}

static int inu_dso_fdk__txDone(inu_ref *me, void *bufP)
{
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)((inu_dso_fdk*)me)->privP;
   if (privP->txDataDoneFunc)
   {
      privP->txDataDoneFunc(me, (unsigned char*)bufP);
   }
   return 0;
}

static void inu_dso_fdk__operate(inu_function *me, inu_data *dataInput[])
{
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)((inu_dso_fdk*)me)->privP;
   if (privP->operateFunc)
   {
      privP->operateFunc(me, (void**)dataInput);
   }
}

static void inu_dso_fdk__setState(inu_function *me, inu_function__stateParam *new_stateParamP)
{
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)((inu_dso_fdk*)me)->privP;
   inu_function__vtable_get()->p_setState(me, new_stateParamP);
   if (privP->setStateFunc)
   {
      privP->setStateFunc(me,*new_stateParamP);
   }
}

void inu_dso_fdk__vtable_init(inu_ref *me)
{
   if (!_bool_vtable_initialized) {
      _vtable.node_vtable.ref_vtable.p_name = inu_dso_fdk__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_dso_fdk__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_dso_fdk__ctor;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_dso_fdk__rxData;
      _vtable.node_vtable.ref_vtable.p_txDone = inu_dso_fdk__txDone;

      _vtable.p_operate = inu_dso_fdk__operate;
      _vtable.p_setState = inu_dso_fdk__setState;

      _bool_vtable_initialized = true;
   }
   inu_ref__setVTable((inu_ref*)me, (inu_ref__VTable*)&_vtable);
}

ERRG_codeE inu_dso_fdk__new(inu_dso_fdkH *meH, inu_dso_fdk__initParams *initParamsP)
{
   inu_dso_fdk__CtorParams ctorParams;
   ctorParams.functionCtorParams.nodeCtorParams.ref_params.device = initParamsP->deviceH;
   memcpy(ctorParams.pluginName, initParamsP->name, INU_DSO_FDK__MAX_NAME_STRING_LEN);
   return inu_factory__new((inu_ref**)meH, &ctorParams, INU_FDK_DSO_REF_TYPE);
}

void inu_dso_fdk__delete(inu_dso_fdkH meH)
{
   inu_factory__delete((inu_ref*)meH);
}

#if DEFSG_IS_GP
static ERRG_codeE inu_dso_fdk__getFuncFromPlugin(inu_dso_fdk *me, char *funcName, void *libH, void **funcAddr)
{
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)me->privP;
   char nameBuf[INU_DSO_FDK__MAX_NAME_STRING_LEN + 24]; //24 should be enough for all prefix
   char *error;

   memset(nameBuf, 0, sizeof(nameBuf));
   strncpy(nameBuf, funcName, sizeof(nameBuf)); //without terminating char
   strcat(nameBuf, privP->pluginName); //with terminating char
   *funcAddr = dlsym(libH, nameBuf);
   if ((error = dlerror()) != NULL)  
   {
      printf("error loading %s. %s\n", nameBuf, error);
      return ERR_INVALID_ARGS;
   }
   else
   {
      printf("%s binded successful\n",funcName);
   }

   return RET_SUCCESS;
}

static ERRG_codeE inu_dso_fdk__loadPlugin(inu_dso_fdk *me)
{
   void* libhandle = NULL;
   ERRG_codeE ret;
   inu_dso_fdk__privData *privP = (inu_dso_fdk__privData*)me->privP;
   char libFile[INU_DSO_FDK__MAX_NAME_STRING_LEN + sizeof(INU_DSO_FDK__FULL_PATH) + sizeof(INU_DSO_FDK__PREFIX_LIB) + sizeof(INU_DSO_FDK__POSTFIX_SO)];

   memset(libFile, 0 , sizeof(libFile));
   strcat(libFile,INU_DSO_FDK__FULL_PATH);
   strcat(libFile,INU_DSO_FDK__PREFIX_LIB);
   strcat(libFile,privP->pluginName);
   strcat(libFile,INU_DSO_FDK__POSTFIX_SO);

   printf("inu_dso_fdk__loadPlugin: %s, filename = %s\n",privP->pluginName, libFile);
   
   libhandle = dlopen(libFile, RTLD_NOW);
   if (!libhandle) 
   {
      printf("Error loading DSO: %s, pluginName = %s, fileName = %s\n", dlerror(), privP->pluginName, libFile);
      return ERR_OUT_OF_RSRCS;
   }

   dlerror();     /* Clear any existing error */

   ret  = inu_dso_fdk__getFuncFromPlugin(me, "ctor_",          libhandle, (void**)&privP->ctorFunc);
   ret |= inu_dso_fdk__getFuncFromPlugin(me, "dtor_",          libhandle, (void**)&privP->dtorFunc);
   ret |= inu_dso_fdk__getFuncFromPlugin(me, "setState_",    libhandle, (void**)&privP->setStateFunc);
   ret |= inu_dso_fdk__getFuncFromPlugin(me, "operate_",       libhandle, (void**)&privP->operateFunc);
   ret |= inu_dso_fdk__getFuncFromPlugin(me, "rxData_",       libhandle, (void**)&privP->rxDataFunc);
   ret |= inu_dso_fdk__getFuncFromPlugin(me, "txDataDone_",    libhandle, (void**)&privP->txDataDoneFunc);

   return ret;
}


static void inu_dso_fdk__complete(inu_dso_fdkH fdkH)
{
   inu_graph__function_complete_msg(fdkH);
}

static void inu_dso_fdk__forward_data(inu_dso_fdkH fdkH, inu_dataH dataH)
{
   inu_function__done_data(fdkH,dataH);
}

static void inu_dso_fdk__new_data(inu_dso_fdkH fdkH, inu_dataH dataH)
{
   FIX_UNUSED_PARAM_WARN(fdkH);
   FIX_UNUSED_PARAM_WARN(dataH);
   //alloc buffer for fdk?
   //inu_function__newData();
}
#endif
#endif
