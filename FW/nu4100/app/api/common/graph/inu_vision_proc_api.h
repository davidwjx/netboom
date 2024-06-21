#ifndef _INU_VISION_PROC_API_H_
#define _INU_VISION_PROC_API_H_

#include "inu2.h"

typedef void* inu_vision_procH;

typedef struct
{
   inu_graphH  graphH;
   inu_deviceH deviceH;
}inu_vision_proc__initParams;

typedef enum
{
   VISION_PROC_BOKEH,
   VISION_PROC_BACKGROUND_REMOVAL,
   VISION_PROC_LOW_LIGHT_ISP,
   VISION_PROC_AUTO_FRAMING,
   VISION_PROC_NO_ALG,
}VISION_PROCG_visionProcAlgTypeE;

typedef enum
{
   VISION_PROC_1080P,
   VISION_PROC_4K,
}VISION_PROCG_visionProcResolutionE;

typedef enum
{
   VISION_PROC_SINGLE,
   VISION_PROC_DUAL,
}VISION_PROCG_visionProcCameraE;

typedef struct
{
   VISION_PROCG_visionProcResolutionE visionProcResolution;
   VISION_PROCG_visionProcCameraE visionProcCamera;
   float blurLevel;
   UINT32 depthOutput;
   UINT32 faceBoxDraw;// � If to draw the face box. Default false
   float faceSmoothFactor;// � Default 0.1
   UINT32 noFaceFramesToKeepBlur; // <- default to 1000
   UINT32 multiFaceMode; // � default false. Indicates that the bokeh will be in multiface mode. (Future feature).
} VISION_PROCG_bokehParamsT;

typedef struct
{
   VISION_PROCG_visionProcResolutionE visionProcResolution;
   UINT32 backgroundImage;
} VISION_PROCG_backgroundRemovalParamsT;

typedef struct
{
   VISION_PROCG_visionProcResolutionE visionProcResolution;
   UINT32 stride;
} VISION_PROCG_lowLightIspParamsT;

typedef struct
{
   VISION_PROCG_visionProcResolutionE visionProcResolution;
   UINT32 frameIndex;
   float whiteBalance[3];
   UINT16 ccmBitPrecision;
   INT16 ccm[3][3];
   UINT16 gammaLutSize;
   UINT8* gammaLutPhysAddr;
   UINT8* gammaLutVirtAddr;
   UINT16 gainCurveBitPrecision;
   UINT16 gainCurveSize;
   UINT16* gainCurvePhysAddr;
   UINT16* gainCurveVirtAddr;
   float exposure;
   float analogGain;
   float digitalGain;
} VISION_PROCG_lowLightIspCalcT;

typedef struct
{
   VISION_PROCG_visionProcResolutionE visionProcResolution;
   UINT32 maxZoom;
   UINT32 trackingSpeed;
   UINT32 multiplePersonDetection;
} VISION_PROCG_autoFramingParamsT;

typedef struct
{
   VISION_PROCG_visionProcAlgTypeE algType;
   VISION_PROCG_bokehParamsT bokehParams;
   VISION_PROCG_backgroundRemovalParamsT backgroundRemovalParams;
   VISION_PROCG_lowLightIspParamsT lowLightIspParams;
   VISION_PROCG_autoFramingParamsT autoFramingParams;
} inu_vision_proc__parameter_list_t;

ERRG_codeE inu_vision_proc__new(inu_vision_procH *meH, inu_vision_proc__initParams *initParamsP);
void inu_vision_proc__delete(inu_vision_procH meH);
ERRG_codeE inu_vision_proc__updateParams(inu_vision_procH meH, inu_vision_proc__parameter_list_t *cfgParamsP);

#endif
