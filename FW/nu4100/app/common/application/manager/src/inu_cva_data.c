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
#include "inu_metadata_serializer.h"
#if DEFSG_IS_GP
#include "data_base.h"
#endif

typedef struct
{
   char name[35];
}inu_cva_data__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_cva_data__name(inu_ref *me)
{
   inu_cva_data *cva_data = (inu_cva_data*)me;
   inu_cva_data__privData *privP = (inu_cva_data__privData*)cva_data->privP;

   return privP->name;
}

static void inu_cva_data__dtor(inu_ref *me)
{
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_cva_data*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_cva_data__ctor(inu_cva_data *me, inu_cva_data__CtorParams *ctorParamsP)
{
   char format[20];
   ERRG_codeE ret;
   inu_cva_data__privData *privP;
   UINT32 size=0;
   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_cva_data__privData*)malloc(sizeof(inu_cva_data__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_cva_data__privData));
         me->privP = privP;
         memcpy(&me->cvaHdr.cvaDescriptor, &ctorParamsP->cvaDescriptor, sizeof(inu_cva_data__descriptor_t));

         strcpy(privP->name, "INU_CVA_DATA");
         sprintf(format, ", format: %d", me->cvaHdr.cvaDescriptor.format);
         strcat(privP->name, format);
         size = 134400;//set to max size (hybrid) -> (2 * 800 * 672) / 8 = 134400bytes
         inu_data__sizeSet((inu_data*)me, size, size);
      }
      else
      {
         ret = INU_CVA_DATA__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static UINT32 inu_cva_data__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_CVA_DATA_SEND_E;
}

static void *inu_cva_data__hdrGet(inu_data *me)
{
   inu_cva_data *cva_data = (inu_cva_data*)me;
   return &cva_data->cvaHdr;
}

static void inu_cva_data__hdrSet(inu_data *me, void *hdrP)
{
   inu_cva_data *cva_data = (inu_cva_data*)me;
   memcpy(&cva_data->cvaHdr, hdrP, sizeof(inu_cva_data__hdr_t));
#ifdef METADATA
   UINT32 metadataSize = inu_metadata__getCVAMetadataSize();
   inu_data__metadata_sizeSet((inu_data*)me, metadataSize);
#endif
}

static inu_data *inu_cva_data__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_cva_data *clone = (inu_cva_data*)bufP;
   if (!clone)
   {
      return NULL;
   }
   //copy all first
   memcpy(clone,me,sizeof(inu_cva_data));
   inu_data__vtable_get()->p_dataDuplicate(me,bufP);
   clone->privP = (inu_cva_data__privData*)malloc(sizeof(inu_cva_data__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_cva_data*)me)->privP, sizeof(inu_cva_data__privData));

   return (inu_data*)clone;
}

static void inu_cva_data__free_data(inu_data* me)
{
   inu_cva_data* clone = (inu_cva_data*)me;
   free(clone->privP);
   inu_data__vtable_get()->p_freeData(me);
}

void inu_cva_data__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_cva_data__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_cva_data__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_cva_data__ctor;

      _vtable.p_hdrMsgCodeGet = inu_cva_data__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_cva_data__hdrGet;
      _vtable.p_dataHdrSet    = inu_cva_data__hdrSet;
      _vtable.p_dataDuplicate = inu_cva_data__duplicate;
#if DEFSG_IS_HOST
      _vtable.p_freeData = inu_cva_data__free_data;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_cva_data__vtable_get(void)
{
   inu_cva_data__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}

