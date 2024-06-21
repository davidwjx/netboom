#ifndef _INU_IMAGE_H_
#define _INU_IMAGE_H_

#include "inu_data.h"
#include "inu2_types.h"



struct inu_image;

typedef struct inu_image
{
   inu_data data;
   inu_image__hdr_t imageHdr;
   void *privP;
}inu_image;

typedef struct inu_image_CtorParamsTag
{
   inu_data__CtorParams  dataCtorParams;
   inu_image__descriptor imageDescriptor;
   UINT16                numLinesPerChunk; //consider moving this into data_ctor
}inu_image__CtorParams;

void inu_image__vtable_init(void);
const inu_data__VTable *inu_image__vtable_get(void);

#endif

