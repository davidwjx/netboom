#ifndef _INU_IMAGE_API_H_
#define _INU_IMAGE_API_H_

#include "inu2.h"

typedef void* inu_imageH;

#define IMAGE_BYTES_PER_PIXEL(imgHdrP) ((imgHdrP->imgDescriptor.bitsPerPixel + 7) / 8)
#define IMAGE_SIZE_PIXELS(imgHdrP) (imgHdrP->imgDescriptor.stride * imgHdrP->imgDescriptor.bufferHeight)
#define IMAGE_SIZE_BYTES(imgHdrP) (IMAGE_SIZE_PIXELS(imgHdrP) * IMAGE_BYTES_PER_PIXEL(imgHdrP))


typedef struct inu_image__dimsTag
{
   UINT32                  width;
   UINT32                  height;
   NUCFG_formatE           format;
   NUCFG_formatDiscriptorU formatDiscriptor;
   UINT32                  x;
   UINT32                  y;
   UINT32                  stride;
   UINT32                  bufferHeight;
   UINT32                  bitsPerPixel;
   UINT32                  realBitsMask;
   UINT32                  numInterleaveImages;
   NUCFG_interModeE        interMode;
}inu_image__descriptor;

typedef struct
{
   UINT32                nucfgId;
   UINT32                exposure; //[Us]
   float                 analogGain;
   float                 digitalGain;
}inu_image__sensorInfoT;

typedef struct
{
   UINT32                param1;
   UINT32                param2;
}inu_image__userInfoT;


typedef struct
{
   inu_data__hdr_t            dataHdr;
   inu_image__descriptor      imgDescriptor;
   inu_image__sensorInfoT     exposureGain[NUCFG_MAX_CHAN_INPUTS];
   inu_image__userInfoT       userInfo;
   inu_device__projectorModeE projMode;
   UINT32                     sensorGroup;
   UINT64                     readoutTs; //[ns]
}inu_image__hdr_t;

typedef struct
{
   inu_data__initParams    dataInitParams;
   inu_image__descriptor   imageDescriptor;
}inu_image__initParams;

#endif
