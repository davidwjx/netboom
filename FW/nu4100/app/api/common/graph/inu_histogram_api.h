#ifndef _INU_HISTOGRAM_API_H_
#define _INU_HISTOGRAM_API_H_

#include "inu2.h"

typedef void* inu_histogramH;

#define INU_HISTOGRAM__ROI_NUM (3)

/* 
   X0,Y0 - North west, X1,Y1 - South east
   Each one. max value - 10 bits, min value - 1
*/
typedef struct
{
   UINT32   x0;
   UINT32   x1;
   UINT32   y0;
   UINT32   y1;
} inu_histogram__roiT;

typedef struct
{
   inu_histogram__roiT    roi[INU_HISTOGRAM__ROI_NUM];
} inu_histogram__roiCfgT;

ERRG_codeE inu_histogram__cfgRoi(inu_histogramH meH, inu_histogram__roiCfgT *cfgParamsP);

#endif