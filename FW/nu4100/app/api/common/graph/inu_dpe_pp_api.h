#ifndef _INU_DPE_PP_API_H_
#define _INU_DPE_PP_API_H_

#include "inu2.h"

#define DPE_PP_CEVA_BLOB_SIZE          (16*1024*1024)
#define DPE_PP_CEVA_BLOB_BUFFER_SIZE   (4*1024*1024)
#define DPE_PP_EV_BLOB_SIZE          (16*1024*1024)
#define DPE_PP_EV_BLOB_BUFFER_SIZE   (4*1024*1024)

typedef void* inu_dpe_ppH;

typedef struct
{
   inu_graphH  graphH;
   inu_deviceH deviceH;
}inu_dpe_pp__initParams;

typedef enum
{
   INU_DPE_PP_TEMPORAL_FILTER,
   INU_DPE_PP_BLOB_OUTLIERS_REMOVAL,
}inu_dpe_pp_algTypeE;

typedef struct
{
   INT32 algType;
   INT32 BlobMaxSize;
   INT32 BlobMaxHeight;
   INT32 ConfidenceThresholdRegion1;//CONFIDENCE
   INT32 BlobDispDiffSameBlob;
   INT32 TemporolRapidTh;
   INT32 TemporolStableTh;
}inu_dpe_pp_hostGpMsgStructT;

ERRG_codeE inu_dpe_pp__new(inu_dpe_ppH *meH, inu_dpe_pp__initParams *initParamsP);
void inu_dpe_pp__delete(inu_dpe_ppH meH);
ERRG_codeE inu_dpe_pp__send_data_async(inu_dpe_ppH meH, char *bufP, unsigned int len);

#endif


