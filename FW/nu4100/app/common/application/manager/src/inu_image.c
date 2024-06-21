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
#include "data_base.h"
#endif
#include "inu_metadata.h"

typedef struct
{
   char name[35];
   UINT16 numLinesPerChunk;
}inu_image__privData;

static inu_data__VTable _vtable;
static bool _bool_vtable_initialized = 0;

static const char* inu_image__name(inu_ref *me)
{
   inu_image *image = (inu_image*)me;
   inu_image__privData *privP = (inu_image__privData*)image->privP;

   return privP->name;
}

static void inu_image__dtor(inu_ref *me)
{
   inu_data__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
   free(((inu_image*)me)->privP);
}

/* Constructor */
ERRG_codeE inu_image__ctor(inu_image *me, inu_image__CtorParams *ctorParamsP)
{
   char format[20];
   ERRG_codeE ret;
   inu_image__privData *privP;
   inu_image__hdr_t hdr;

   ret = inu_data__ctor(&me->data, &ctorParamsP->dataCtorParams);
   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_image__privData*)malloc(sizeof(inu_image__privData));
      if (privP)
      {
         memset(privP, 0, sizeof(inu_image__privData));
         me->privP = privP;
         strcpy(privP->name, "INU_IMAGE");
         sprintf(format, ", format: %d", me->imageHdr.imgDescriptor.format);
         strcat(privP->name, format);
         memset(&hdr, 0, sizeof(hdr));
         memcpy(&hdr.imgDescriptor, &ctorParamsP->imageDescriptor, sizeof(inu_image__descriptor));
         privP->numLinesPerChunk = ctorParamsP->numLinesPerChunk;
         ((inu_data__VTable*)(me->data.node.ref.p_vtable))->p_dataHdrSet((inu_data*)me,&hdr);
      }
      else
      {
         ret = INU_IMAGE__ERR_OUT_OF_MEM;
      }
   }
   return ret;
}

static UINT32 inu_image__hdrMsgCodeGet(inu_data *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return INTERNAL_CMDG_IMAGE_SEND_E;
}

static void *inu_image__hdrGet(inu_data *me)
{
   inu_image *image = (inu_image*)me;
   return &image->imageHdr;
}

static void inu_image__hdrSet(inu_data *me, void *hdrP)
{
   UINT32 size=0, chunkSize;
#ifdef METADATA
   UINT32 metadataSize = 0;
   UINT32 metadataRows = inu_metadata__getNumberMetadataRows();
#endif
   inu_image__hdr_t *imageHdrP = (inu_image__hdr_t*)hdrP;
   inu_image *image = (inu_image*)me;
   inu_image__privData *privP = (inu_image__privData*)image->privP;

   //printf("stride: %d, bpp: %d\n", imageHdrP->imgDescriptor.stride, imageHdrP->imgDescriptor.bitsPerPixel);
   chunkSize = privP->numLinesPerChunk * imageHdrP->imgDescriptor.stride * ((imageHdrP->imgDescriptor.bitsPerPixel + 7) / 8);
   size = (imageHdrP->imgDescriptor.bufferHeight * imageHdrP->imgDescriptor.stride * ((imageHdrP->imgDescriptor.bitsPerPixel + 7) / 8));
   //printf("numLinesPerChunk: %d, chunkSize: %d, bufferHeight: %d, size: %d\n", privP->numLinesPerChunk, chunkSize, imageHdrP->imgDescriptor.bufferHeight, size);
   inu_data__sizeSet((inu_data*)me, size, chunkSize);
#ifdef METADATA
   /*
    At startup we don't actually know how much metadata the target is going to send so ALLOCATED_NUMBER_OF_METADATA_ROWS will have to be increased if enough space isn't allocated
    */
   metadataSize = (metadataRows * imageHdrP->imgDescriptor.stride * ((imageHdrP->imgDescriptor.bitsPerPixel + 7) / 8));
   //printf("metadata_rows: %d metadata_size: %d\n", metadataRows, metadataSize);
   inu_data__metadata_sizeSet((inu_data*)me, metadataSize);
#endif
   memcpy(&image->imageHdr, hdrP, sizeof(inu_image__hdr_t));
}

static void inu_image__free_data(inu_data* me)
{
   inu_image* clone = (inu_image*)me;
   free(clone->privP);
   inu_data__vtable_get()->p_freeData(me);
}

static inu_data *inu_image__duplicate(inu_data *me, UINT8 *bufP)
{
   inu_image *clone = (inu_image*)bufP;
   if (!clone)
   {
      return NULL;
   }
   //copy all first
   memcpy(clone,me,sizeof(inu_image));
   inu_data__vtable_get()->p_dataDuplicate(me,bufP);
   clone->privP = (inu_image__privData*)malloc(sizeof(inu_image__privData));
   if (!clone->privP)
   {
      return NULL;
   }
   memcpy(clone->privP, ((inu_image*)me)->privP, sizeof(inu_image__privData));

   return (inu_data*)clone;
}

void inu_image__vtable_init( void )
{
   if (!_bool_vtable_initialized) 
   {
      inu_data__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_image__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_image__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_image__ctor;

      _vtable.p_hdrMsgCodeGet = inu_image__hdrMsgCodeGet;
      _vtable.p_dataHdrGet    = inu_image__hdrGet;
      _vtable.p_dataHdrSet    = inu_image__hdrSet;
      _vtable.p_dataDuplicate = inu_image__duplicate;
#if DEFSG_IS_HOST
      _vtable.p_freeData      = inu_image__free_data;
#endif
      _bool_vtable_initialized = true;
   }
}

const inu_data__VTable *inu_image__vtable_get(void)
{
   inu_image__vtable_init();
   return (const inu_data__VTable*)&_vtable;
}

