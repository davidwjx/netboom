#include "inu_types.h"
#include "inu2.h"
#include "inu2_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err_defs.h"
#include "internal_cmd.h"


typedef struct
{
   char name[35];
}inu_fast_orb_data__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_fast_orb_data__name(inu_ref *me)
{
   inu_fast_orb_data *fast_data = (inu_fast_orb_data*)me;
   inu_fast_orb_data__privData *privP = (inu_fast_orb_data__privData*)fast_data->privP;

   return privP->name;
}

static void inu_fast_orb_data__dtor(inu_ref *me)
{
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_fast_orb_data*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_fast_orb_data__ctor(inu_fast_orb_data *me, inu_fast_orb_data__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_fast_orb_data__privData *privP;
   UINT32 size=0;
   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_fast_orb_data__privData*)malloc(sizeof(inu_fast_orb_data__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_fast_orb_data__privData));
         me->privP = privP;
         strcpy(privP->name, "INU_FAST_DATA");
         size = FAST_ORB_OUT_BUF_SIZE;
         inu_data__sizeSet((inu_data*)me, size, size);
      }
      else
      {
         ret = INU_IMAGE__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static UINT32 inu_fast_orb_data__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_FAST_ORB_SEND_E;
}

static void *inu_fast_orb_data__hdrGet(inu_data *me)
{
   inu_fast_orb_data *fastData = (inu_fast_orb_data*)me;
   return &fastData->fast_orb_dataHdr;
}

static void inu_fast_orb_data__hdrSet(inu_data *me, void *hdrP)
{
   inu_fast_orb_data *fastData = (inu_fast_orb_data*)me;
   memcpy(&fastData->fast_orb_dataHdr, hdrP, sizeof(inu_fast_orb_data__hdr_t));
}

static inu_data *inu_fast_orb_data__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_fast_orb_data *clone = (inu_fast_orb_data*)bufP;
   if (!clone)
   {
      return NULL;
   }

   memcpy(clone, me, sizeof(inu_fast_orb_data));
   inu_data__vtable_get()->p_dataDuplicate(me, bufP);
   clone->privP = (inu_fast_orb_data__privData*)malloc(sizeof(inu_fast_orb_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_fast_orb_data*)me)->privP, sizeof(inu_fast_orb_data__privData));

   return (inu_data*)clone;
}

static void inu_fast_data__free_data(inu_data* me)
{
   inu_fast_orb_data* clone = (inu_fast_orb_data*)me;
   free(clone->privP);
   inu_data__vtable_get()->p_freeData(me);
}

void inu_fast_orb_data__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_fast_orb_data__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_fast_orb_data__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_fast_orb_data__ctor;

      _vtable.p_hdrMsgCodeGet = inu_fast_orb_data__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_fast_orb_data__hdrGet;
      _vtable.p_dataHdrSet    = inu_fast_orb_data__hdrSet;
      _vtable.p_dataDuplicate = inu_fast_orb_data__duplicate;
#if DEFSG_IS_HOST
      _vtable.p_freeData      = inu_fast_data__free_data;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_fast_orb_data__vtable_get(void)
{
   inu_fast_orb_data__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}


/* API functions */
ERRG_codeE inu_fast_orb_data__new(inu_fast_orb_dataH *fast_dataH, inu_fast_orb_data__initParams *initParamsP)
{
   inu_fast_orb_data__CtorParams ctorParams;
   ctorParams.dataCtorParams.nodeCtorParams.ref_params.device = initParamsP->dataInitParams.deviceH;
   ctorParams.dataCtorParams.sourceNode.id = ((inu_node*)initParamsP->dataInitParams.sourceFunctionH)->ref.id;
   return inu_factory__new((inu_ref**)fast_dataH, &ctorParams, INU_FAST_ORB_DATA_REF_TYPE);
}

void inu_fast_orb_data__delete(inu_fast_orb_dataH meH)
{
   inu_factory__delete((inu_ref*)meH, 0);
}

