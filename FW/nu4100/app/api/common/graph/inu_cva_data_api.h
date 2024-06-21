#ifndef _INU_CVA_DATA_API_H_
#define _INU_CVA_DATA_API_H_

#include "inu2.h"

typedef void* inu_cva_dataH;

typedef enum
{
   INU_CVA_DATA_INVALID,
   INU_CVA_DATA_DOG,
   INU_CVA_DATA_DOG_FREAK,
   INU_CVA_DATA_DIFF,
   INU_CVA_DATA_NORM_EST,
   INU_CVA_DATA_TYPE_LAST
}inu_cva_data__type_e;

typedef struct inu_cva_data__dimsTag
{
   UINT32               width;
   UINT32               height;
   UINT32               format;
   UINT32               kpNum;
   UINT32               bpp;
    inu_cva_data__type_e   type;
}inu_cva_data__descriptor_t;

typedef struct
{
   UINT32 kp_descriptor[16];  // descriptor - 512 bit
   UINT32 xs;
   UINT32 ys;
   float  angle;
   UINT32 patternSize;
   UINT32 notImg1:8;
   UINT32 imgIsOdd:8;
   UINT32 jobId:16;
}inu_cva_data__freak_descriptor_t;


typedef struct
{
   inu_data__hdr_t             dataHdr;
   inu_cva_data__descriptor_t  cvaDescriptor;
   UINT64                      readoutTs;
}inu_cva_data__hdr_t;

#endif
