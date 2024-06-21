#include "inu_types.h"
#include "inu2.h"
#include "inu2_internal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "err_defs.h"
#include "internal_cmd.h"
#include "sys_defs.h"
#include "mem_pool.h"
#if DEFSG_IS_GP
#include "mem_map.h"
#include "data_base.h"
#endif

//taken from MEM_MAPG_IAE_LUT_HIST_0_E
#define INU_HISTOGRAM__SIZE (0x2428)

typedef struct
{
   char name[35];
}inu_histogram_data__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_histogram_data__name(inu_ref *me)
{
   inu_histogram_data *histogram_data = (inu_histogram_data*)me;
   inu_histogram_data__privData *privP = (inu_histogram_data__privData*)histogram_data->privP;
   return privP->name;
}

static void inu_histogram_data__dtor(inu_ref *me)
{
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_histogram_data*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_histogram_data__ctor(inu_histogram_data *me, inu_histogram_data__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_histogram_data__privData *privP;
   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_histogram_data__privData*)malloc(sizeof(inu_histogram_data__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_histogram_data__privData));
         me->privP = privP;
         strcpy(privP->name, "INU_HISTOGRAM_DATA");
         inu_data__sizeSet((inu_data*)me, INU_HISTOGRAM__SIZE, INU_HISTOGRAM__SIZE);
      }
      else
      {
         ret = INU_DATA__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static UINT32 inu_histogram_data__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_HISTOGRAM_DATA_SEND_E;
}

static void *inu_histogram_data__hdrGet(inu_data *me)
{
   inu_histogram_data *histogram_data = (inu_histogram_data*)me;
   return &histogram_data->histogram_dataHdr;
}

static void inu_histogram_data__hdrSet(inu_data *me, void *hdrP)
{
   inu_histogram_data *histogram_data = (inu_histogram_data*)me;
   memcpy(&histogram_data->histogram_dataHdr, hdrP, sizeof(inu_histogram_data__hdr_t));
}

static inu_data *inu_histogram_data__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_histogram_data *clone = (inu_histogram_data*)bufP;
   if (!clone)
   {
      return NULL;
   }

   memcpy(clone,me,sizeof(inu_histogram_data));
   inu_data__vtable_get()->p_dataDuplicate(me,bufP);
   clone->privP = (inu_histogram_data__privData*)malloc(sizeof(inu_histogram_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_histogram_data*)me)->privP, sizeof(inu_histogram_data__privData));

   return (inu_data*)clone;
}

void inu_histogram_data__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_histogram_data__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_histogram_data__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_histogram_data__ctor;

      _vtable.p_hdrMsgCodeGet = inu_histogram_data__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_histogram_data__hdrGet;
      _vtable.p_dataHdrSet    = inu_histogram_data__hdrSet;
      _vtable.p_dataDuplicate = inu_histogram_data__duplicate;

      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_histogram_data__vtable_get(void)
{
   inu_histogram_data__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}
