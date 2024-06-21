#ifndef _INU_CDNN_API_H_
#define _INU_CDNN_API_H_

#include "inu2.h"

typedef void* inu_cdnnH;

#define CDNN_MAX_NETWORKS                       (15)
#define CDNN_CEVA_WEBCAM_IMAGE_MAX_WIDTH  		(1280) 
#define CDNN_CEVA_WEBCAM_IMAGE_MAX_HEIGHT 		(1280)
#define CDNN_CEVA_WEBCAM_RGB_IMAGE_SIZE  	      (3 * CDNN_CEVA_WEBCAM_IMAGE_MAX_HEIGHT * CDNN_CEVA_WEBCAM_IMAGE_MAX_HEIGHT)
#define CDNN_CEVA_ENGINE_MAX_SIZE				   (80*1024*1024)
#define CDNN_CEVA_INTERNAL_MAX_SIZE				   (512*1024)

typedef struct
{
   inu_graphH  graphH;
   inu_deviceH deviceH;
}inu_cdnn__initParams;

typedef enum
{
	/*! \brief Sets the input fraction bits to 8. */
	CDNNG_INPUT_IMAGE_RAW_SCALE_1,
	/*! \brief Sets the input fraction bits to 0. */
	CDNNG_INPUT_IMAGE_RAW_SCALE_256
} CDNNG_inputImageRawScaleE;

typedef enum
{
	/*! \brief Decides whether or not to swap the channel order of the input image. */
	CDNNG_NETWORK_CHANNEL_NO_SWAP,
	CDNNG_NETWORK_CHANNEL_SWAP
} CDNNG_channelSwapE;

typedef enum
{
	/*! \brief Choose network output format between fixed-point 16 bit element to floating-point 32 bit*/
	CDNNG_FIXED_POINT_QUERY,
	CDNNG_FLOATING_POINT_QUERY
}CDNNG_resultsFormatE;

typedef enum
{
	/*! \brief Choose network output format between fixed-point 16 bit element to floating-point 32 bit*/
	CDNNG_BIT_ACCURACY_8BIT,
	CDNNG_BIT_ACCURACY_16BIT,
	CDNNG_BIT_ACCURACY_INVALID
}CDNNG_bitAccuracyE;

typedef enum
{
   /*! \brief Choose network type */
   CDNNG_OTHER_CNN,
	CDNNG_YOLO_CNN
} CDNNG_netType;


typedef enum
{
	CDNNG_1_CHANNEL = 1,	//	for example: YUV image
	CDNNG_2_CHANNELS = 2,
	CDNNG_3_CHANNELS = 3,	//	for example: RGB image
	CDNNG_4_CHANNELS = 4,
	CDNNG_DELAULT_CHANNELS = CDNNG_3_CHANNELS
}CDNNG_numOfChannelsE;

typedef enum
{	
	CDNNG_PIXEL_TYPE_UINT8,
	CDNNG_PIXEL_TYPE_INT8,
	CDNNG_PIXEL_TYPE_UINT16,
	CDNNG_PIXEL_TYPE_INT16,
	CDNNG_PIXEL_TYPE_UINT32,
	CDNNG_PIXEL_TYPE_INT32,
	CDNNG_PIXEL_TYPE_FLOAT32,
	CDNNG_PIXEL_TYPE_FLOAT64,
	CDNNG_PIXEL_TYPE_12UINT16,
	CDNNG_PIXEL_TYPE_12INT16
}CDNNG_channelPixelTypeE;

typedef enum
{
	CDNNG_VERSION_2017_0		= 20170,
	CDNNG_VERSION_2017_ED1	= 20171,
	CDNNG_VERSION_2017_ED2	= 20172,
	CDNNG_VERSION_2018_0		= 20180,
	CDNNG_VERSION_2018_3		= 20183,
	CDNNG_CURRENT_VERSION	= CDNNG_VERSION_2018_3
}CDNNG_version_E;

typedef struct
{
	CDNNG_numOfChannelsE numChannels : 8;
	CDNNG_channelPixelTypeE pixelType : 8;
	UINT32 reserved1 : 8;
	UINT32 reserved2 : 8;
} CDNNG_inputFormatT;

typedef struct
{
   UINT32    network_id;
} inu_cdnn__networkIdSetT;

ERRG_codeE inu_cdnn__new(inu_cdnnH *meH, inu_cdnn__initParams *initParamsP);
void inu_cdnn__delete(inu_cdnnH meH);
ERRG_codeE inu_cdnn__setNetworkId(inu_cdnnH meH, inu_cdnn__networkIdSetT *networkIdParamP);

#endif
