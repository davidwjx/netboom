#ifndef _INU_DATA_API_H_
#define _INU_DATA_API_H_

#include "inu2.h"

typedef struct
{
    UINT64 timestamp; //[ns]
    UINT64 dataIndex;
    UINT32 chunkNum;
    UINT32 flags;
}inu_data__hdr_t;

typedef struct
{
    inu_deviceH deviceH;
    inu_functionH sourceFunctionH;
}inu_data__initParams;

ERRG_codeE inu_data__new(inu_dataH *dataH, inu_data__initParams *initParamsP);
void *inu_data__getMemPtr(inu_dataH dataH);
unsigned int inu_data__getMemSize(inu_dataH dataH);
UINT32 inu_data__chunkSizeGet(inu_dataH dataH);
UINT16 inu_data__chunkTotalNumGet(inu_dataH dataH);
void *inu_data__getHdr(inu_dataH dataH);
UINT32 inu_data__getMode(inu_dataH dataH);
UINT32 inu_data__isCompressed(inu_dataH dataH);
UINT32 inu_data__getMetadataSize(inu_dataH data);
#endif
