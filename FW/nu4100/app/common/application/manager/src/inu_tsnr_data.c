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


#define INU_TSNR__SIZE (TSNR_MAX_BLOB_SIZE)

typedef struct
{
   char name[35];
}inu_tsnr_data__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_tsnr_data__name(inu_ref *me)
{
   inu_tsnr_data *tsnr_data = (inu_tsnr_data*)me;
   inu_tsnr_data__privData *privP = (inu_tsnr_data__privData*)tsnr_data->privP;
   return privP->name;
}

static void inu_tsnr_data__dtor(inu_ref *me)
{
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_tsnr_data*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_tsnr_data__ctor(inu_tsnr_data *me, inu_tsnr_data__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_tsnr_data__privData *privP;
   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_tsnr_data__privData*)malloc(sizeof(inu_tsnr_data__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_tsnr_data__privData));
         me->privP = privP;
         strcpy(privP->name, "INU_TSNR_DATA");
         inu_data__sizeSet((inu_data*)me, INU_TSNR__SIZE,INU_TSNR__SIZE);
      }
      else
      {
         ret = INU_DATA__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static UINT32 inu_tsnr_data__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_TSNR_DATA_SEND_E;
}

static void *inu_tsnr_data__hdrGet(inu_data *me)
{
   inu_tsnr_data *tsnr_data = (inu_tsnr_data*)me;
   return &tsnr_data->tsnr_dataHdr;
}

static void inu_tsnr_data__hdrSet(inu_data *me, void *hdrP)
{
   inu_tsnr_data *tsnr_data = (inu_tsnr_data*)me;
   memcpy(&tsnr_data->tsnr_dataHdr, hdrP, sizeof(inu_tsnr_data__hdr_t));
}

static inu_data *inu_tsnr_data__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_tsnr_data *clone = (inu_tsnr_data*)bufP;
   if (!clone)
   {
      return NULL;
   }

   memcpy(clone,me,sizeof(inu_tsnr_data));
   inu_data__vtable_get()->p_dataDuplicate(me,bufP);
   clone->privP = (inu_tsnr_data__privData*)malloc(sizeof(inu_tsnr_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_tsnr_data*)me)->privP, sizeof(inu_tsnr_data__privData));

   return (inu_data*)clone;
}

static void inu_tsnr_data__free_data(inu_data* me)
{
   inu_tsnr_data* clone = (inu_tsnr_data*)me;
   free(clone->privP);
   inu_data__vtable_get()->p_freeData(me);
}

void inu_tsnr_data__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_tsnr_data__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_tsnr_data__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_tsnr_data__ctor;

      _vtable.p_hdrMsgCodeGet = inu_tsnr_data__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_tsnr_data__hdrGet;
      _vtable.p_dataHdrSet    = inu_tsnr_data__hdrSet;
      _vtable.p_dataDuplicate = inu_tsnr_data__duplicate;
#if DEFSG_IS_HOST
      _vtable.p_freeData      = inu_tsnr_data__free_data;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_tsnr_data__vtable_get(void)
{
   inu_tsnr_data__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}
