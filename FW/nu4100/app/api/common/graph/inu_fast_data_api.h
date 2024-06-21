#ifndef _INU_FAST_ORB_DATA_API_H_
#define _INU_FAST_ORB_DATA_API_H_

/* Features from Accelerated Segment Test */

#include "inu2.h"


#define MAX_KEYPOINTS_NUM         (1024)
#define POS_SIZE_IN_BYTES         (2)
#define FEATURE_POS_SIZE_IN_BYTES (2 * POS_SIZE_IN_BYTES)
#define SCORE_SIZE_IN_BYTES       (4)
#define DESCRIPTORS_SIZE_IN_BYTES (32)
#define FAST_ORB_OUT_BUF_SIZE     (MAX_KEYPOINTS_NUM * (FEATURE_POS_SIZE_IN_BYTES + SCORE_SIZE_IN_BYTES + DESCRIPTORS_SIZE_IN_BYTES))

typedef void* inu_fast_orb_dataH;

typedef struct
{
   inu_data__hdr_t           dataHdr;       //header (timestamp & index)
   UINT32                    numKeyPoints;  //number of keypoints
   UINT32                    yOffset;       //keypoints y position offset in bytes from fast orb data buffer start
   UINT32                    scoreOffset;   //keypoints score offset in bytes from fast orb data buffer start
   UINT32                    descOffset;    //orb descriptors offset in bytes from fast orb data buffer start 
}inu_fast_orb_data__hdr_t;

typedef struct
{
   inu_data__initParams    dataInitParams;
}inu_fast_orb_data__initParams;

ERRG_codeE inu_fast_orb_data__new(inu_fast_orb_dataH *fast_dataH, inu_fast_orb_data__initParams *initParamsP);
void inu_fast_orb_data__delete(inu_fast_orb_dataH meH);

#endif
