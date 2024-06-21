#ifndef _INU_TSNR_CALC_API_H_
#define _INU_TSNR_CALC_API_H_

#include "inu2.h"

typedef void* inu_tsnr_calcH;

typedef struct
{
   UINT32 saturatedGrayLevelValue;
   UINT32 saturatedThreshold;
   UINT32 allowedBadSNRBlocksThreshold;
   UINT32 saturationThresholdInImage;
   float snrTarget;
   float slopeWeight;
   UINT32 sideToCalc;
} inu_tsnr_calc__parameter_list_t;


#endif