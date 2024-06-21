
#ifndef __VE_MNGR_H__
#define __VE_MNGR_H__


#include "inu_defs.h"
#include "inu_vision_proc_api.h"

#ifdef __cplusplus
    extern "C" {
#endif

//#define VE_MNGRP_TEST
#define VE_MNGRG_MAX_VE_MSG_LENGTH      32

typedef enum
{
    VE_MNGRG_ENABLE_REGULAR_E = 20,    // 20
    VE_MNGRG_ENABLE_BOKEH_E,           // 21
    VE_MNGRG_ENABLE_LOW_LIGHT_ISP_E,   // 22
    VE_MNGRG_ENABLE_BG_REMOVAL_E,      // 23

    VE_MNGRG_DISABLE_REGULAR_E = 40,   // 40
    VE_MNGRG_DISABLE_BOKEH_E,          // 41
    VE_MNGRG_DISABLE_LOW_LIGHT_ISP_E,  // 42
    VE_MNGRG_DISABLE_BG_REMOVAL_E,     // 43

    VE_MNGRG_SET_BOKEH_PARAMS_E = 60,  // 60
    VE_MNGRG_GET_BOKEH_PARAMS_E,       // 61

    /* Test commands */
    VE_MNGRG_INIT_E = 200,             // 200
    VE_MNGRG_START_STREAM_E,           // 201
    VE_MNGRG_STOP_STREAM_E,            // 202
    VE_MNGRG_TEST_E,                   // 203
} VE_MNGRG_messageCmdE;

typedef enum
{
    VE_MNGRG_STREAM_INDEX_REGULAR_E,   // 0
    VE_MNGRG_STREAM_INDEX_BOKEH_E,     // 1

    VE_MNGRG_STREAM_INDEX_MAX_E,       // 2
} VE_MNGRG_streamIndexE;

typedef enum
{
    VE_MNGRG_UVC_REQ_GET_CURR_E,
    VE_MNGRG_UVC_REQ_GET_DEFAULT_E,
    VE_MNGRG_UVC_REQ_GET_MIN_E,
    VE_MNGRG_UVC_REQ_GET_MAX_E,
    VE_MNGRG_UVC_REQ_GET_RES_E,
    VE_MNGRG_UVC_REQ_GET_INFO_E,
    VE_MNGRG_NUM_OF_UVC_REQUESTS_E,
} VE_MNGRG_uvcRequestE;

typedef enum
{
    VE_MNGRG_RES_3264_2448_E,
    VE_MNGRG_RES_3200_1800_E,
    VE_MNGRG_RES_2560_1440_E,
    VE_MNGRG_RES_1920_1080_E,
    VE_MNGRG_RES_1280_720_E,
    VE_MNGRG_NUM_OF_RESOLUTIONS_E,
} VE_MNGRG_resolutionE;

typedef enum
{
    VE_MNGRG_FW_UPDATE_BOOTFIX_E,
    VE_MNGRG_FW_UPDATE_BOOTSPL_E,
    VE_MNGRG_FW_UPDATE_CNN_DATA_E,
    VE_MNGRG_FW_UPDATE_DTB_E,
} VE_MNGRG_fwUpdateSectionE;

typedef struct
{
    UINT32                    dummy;
} VE_MNGRG_regularParamsT;

typedef struct
{
    VISION_PROCG_bokehParamsT  visionBokehParams;
} VE_MNGRG_bokehParamsT;

typedef struct
{
    UINT32                     dummy;
} VE_MNGRG_disableParamsT;

typedef struct
{
    VE_MNGRG_messageCmdE        cmd;
    UINT32                      msgLen;
} VE_MNGRG_messageHdrT;

typedef struct
{
    VE_MNGRG_messageHdrT        hdr;
    UINT8                       data[VE_MNGRG_MAX_VE_MSG_LENGTH];
} VE_MNGRG_messageT;

ERRG_codeE VE_MNGRG_init(inu_streamerH* streamerList);
ERRG_codeE VE_MNGRG_deinit(void);

inu_streamerH VE_MNGRP_anyStreamIsRunning(void);
BOOLEAN    VE_MNGRP_readyToWork(void);
ERRG_codeE VE_MNGRG_startStream(inu_streamerH* streamerList);
ERRG_codeE VE_MNGRG_stopStream(inu_streamerH* streamerList);

ERRG_codeE VE_MNGRG_getFrameRate(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *fps);
ERRG_codeE VE_MNGRG_setFrameRate(inu_streamerH streamerH, UINT32 fps);
ERRG_codeE VE_MNGRG_getResolutionMode(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, VE_MNGRG_resolutionE* res);
ERRG_codeE VE_MNGRG_setResolutionMode(inu_streamerH streamerH, VE_MNGRG_resolutionE res);
ERRG_codeE VE_MNGRG_getBrightness(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *brightness);
ERRG_codeE VE_MNGRG_setBrightness(inu_streamerH streamerH, UINT32 brightness);
ERRG_codeE VE_MNGRG_setPowerFreq(inu_streamerH streamerH, UINT32 powerFreq); 
ERRG_codeE VE_MNGRG_getPowerFreq(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *powerFreq);
ERRG_codeE VE_MNGRG_getAutoExposure(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT8 *autoExposureMode);
ERRG_codeE VE_MNGRG_setAutoExposure(inu_streamerH streamerH, UINT8 autoExposureMode);
ERRG_codeE VE_MNGRG_getExposure(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *exposure);
ERRG_codeE VE_MNGRG_setExposure(inu_streamerH streamerH, UINT32 exposure);
ERRG_codeE VE_MNGRG_getGain(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *gain);
ERRG_codeE VE_MNGRG_setGain(inu_streamerH streamerH, UINT32 gain);
ERRG_codeE VE_MNGRG_getPrivacy(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT8 *privacy);

ERRG_codeE VE_MNGRG_enableBokeh(VE_MNGRG_bokehParamsT* bokehParams);
ERRG_codeE VE_MNGRG_disableBokeh(VE_MNGRG_disableParamsT* disableParams);
ERRG_codeE VE_MNGRG_setBokehParams(VE_MNGRG_bokehParamsT* bokehParams);
ERRG_codeE VE_MNGRG_getBokehParams(VE_MNGRG_bokehParamsT* bokehParams);

ERRG_codeE VE_MNGRG_execute(VE_MNGRG_messageT* msg, UINT8* output, UINT32* outputLen);

#ifdef __cplusplus
}
#endif

#endif //__VE_MNGR_H__

