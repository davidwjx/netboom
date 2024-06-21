#ifndef _INU_HISTOGRAM_DATA_API_H_
#define _INU_HISTOGRAM_DATA_API_H_

#include "inu2.h"

typedef void* inu_histogram_dataH;

#define INU_HISTOGRAM_DATA__COLORS_NUM (3)
#define INU_HISTOGRAM_DATA__BINS_NUM (256)

/****************************************************************************
*
*  Histogram in memory (one side):
*
*                    ------- roi 0 -------
*                    0x000 - 0x3ff red
*                    0x400 - 0x7ff green
*                    0x800 - 0xbff blue
*
*                    ------- roi 1 -------
*                    0xc00 - 0xfff red
*                    0x1000 - 0x13ff green      
*                    0x1400 - 0x17ff blue
*
*                    ------- roi 2 -------                    
*                    0x1800 - 0x1bff red
*                    0x1c00 - 0x1fff green      
*                    0x2000 - 0x23ff blue
*
****************************************************************************/
typedef struct
{
   inu_data__hdr_t dataHdr;
   inu_histogram__roiT roi[INU_HISTOGRAM__ROI_NUM];
   UINT32 accumulator[INU_HISTOGRAM__ROI_NUM];
   UINT32 hwFrameCounter;
}inu_histogram_data__hdr_t;

typedef struct
{
   UINT32 histogram[INU_HISTOGRAM__ROI_NUM][INU_HISTOGRAM_DATA__COLORS_NUM][INU_HISTOGRAM_DATA__BINS_NUM];
}inu_histogram_data_t;

#endif