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
}inu_point_cloud_data__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_point_cloud_data__name(inu_ref *me)
{
   inu_point_cloud_data *point_cloud_data = (inu_point_cloud_data*)me;
   inu_point_cloud_data__privData *privP = (inu_point_cloud_data__privData*)point_cloud_data->privP;

   return privP->name;
}

static void inu_point_cloud_data__dtor(inu_ref *me)
{
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_point_cloud_data*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_point_cloud_data__ctor(inu_point_cloud_data *me, inu_point_cloud_data__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_point_cloud_data__privData *privP;
   UINT32 size=0;
   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_point_cloud_data__privData*)malloc(sizeof(inu_point_cloud_data__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_point_cloud_data__privData));
         me->privP = privP;
         strcpy(privP->name, "INU_POINT_CLOUD_DATA");
         size = POINT_CLOUDG_MAX_BLOB_SIZE;
         inu_data__sizeSet((inu_data*)me, size, size);
      }
      else
      {
         ret = INU_IMAGE__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static UINT32 inu_point_cloud_data__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_POINT_CLOUD_DATA_SEND_E;
}

static void *inu_point_cloud_data__hdrGet(inu_data *me)
{
   inu_point_cloud_data *point_cloudData = (inu_point_cloud_data*)me;
   return &point_cloudData->point_cloud_dataHdr;
}

static void inu_point_cloud_data__hdrSet(inu_data *me, void *hdrP)
{
   inu_point_cloud_data *point_cloudData = (inu_point_cloud_data*)me;
   memcpy(&point_cloudData->point_cloud_dataHdr, hdrP, sizeof(inu_point_cloud_data__hdr_t));
}

static inu_data *inu_point_cloud_data__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_point_cloud_data *clone = (inu_point_cloud_data*)bufP;
   if (!clone)
   {
      return NULL;
   }

   memcpy(clone, me, sizeof(inu_point_cloud_data));
   inu_data__vtable_get()->p_dataDuplicate(me, bufP);
   clone->privP = (inu_point_cloud_data__privData*)malloc(sizeof(inu_point_cloud_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_point_cloud_data*)me)->privP, sizeof(inu_point_cloud_data__privData));

   return (inu_data*)clone;
}

static void inu_point_cloud_data__free_data(inu_data* me)
{
   inu_point_cloud_data* clone = (inu_point_cloud_data*)me;
   free(clone->privP);
   inu_data__vtable_get()->p_freeData(me);
}

void inu_point_cloud_data__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_point_cloud_data__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_point_cloud_data__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_point_cloud_data__ctor;

      _vtable.p_hdrMsgCodeGet = inu_point_cloud_data__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_point_cloud_data__hdrGet;
      _vtable.p_dataHdrSet    = inu_point_cloud_data__hdrSet;
      _vtable.p_dataDuplicate = inu_point_cloud_data__duplicate;
#if DEFSG_IS_HOST
      _vtable.p_freeData      = inu_point_cloud_data__free_data;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_point_cloud_data__vtable_get(void)
{
   inu_point_cloud_data__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}


