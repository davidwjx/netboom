#ifdef ENABLE_VISION_EFFECT
/****************************************************************************
 *
 *   FileName: ve_mngr.c
 *
 *   Author: Oshri A.
 *
 *   Date:
 *
 *   Description: Vision Effect manager
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "assert.h"

#include "ve_mngr.h"
#include "uvc_com.h"
#include "inu_common.h"
#include "inu_device.h"
#include "inu_streamer.h"
#include "inu_function.h"
#include "sensors_mngr.h"
#include "inu_sensors_group.h"
#include "data_base.h"
#include "log.h"
#include "gpio_drv.h"
#include "isp_mngr.hpp"

#define VE_MNGRP_SWITCH_VISION_PROC_ALG

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define VE_MNGRP_REGULAR_STREAM_OUT_NAME        "Stream_YUV8mp"
#define VE_MNGRP_VISION_PROC_NODE_NAME          "vision_proc_bokeh"
#define VE_MNGRP_BOKEH_STREAM_OUT_NAME          "Sout_vision_bokeh"
#define VE_MNGRP_SENSOR_CONTROL_OUT_NAME        "SENSOR_CONTROL_0"

#define VE_MNGRP_READY_TIMEOUT_MS               (60 * 1000)

#define VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED     ((UINT8)0x08)
#define VE_MNGRP_AUTO_EXPOSURE_CTRL_DISABLED    ((UINT8)0x01)

#define VE_MNGRP_POWER_LINE_FREQ_60HZ           (0x02)
#define VE_MNGRP_POWER_LINE_FREQ_50HZ           (0x01)
#define VE_MNGRP_POWER_LINE_FREQ_DISABLED       (0x00)

#define VE_MNGRP_UVC_REQ_STR(type)              ((type) == VE_MNGRG_UVC_REQ_GET_CURR_E  ? "get current" :   \
                                                 (type) == VE_MNGRG_UVC_REQ_GET_MIN_E   ? "get min" :       \
                                                 (type) == VE_MNGRG_UVC_REQ_GET_MAX_E   ? "get max" :       \
                                                 (type) == VE_MNGRG_UVC_REQ_GET_RES_E   ? "get res" :       \
                                                 (type) == VE_MNGRG_UVC_REQ_GET_INFO_E  ? "get info" : "get default")


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
    VE_MNGRP_STATE_IDLE_E,              // 0
    VE_MNGRP_STATE_RUNNING_E,           // 1
} VE_MNGRP_stateE;

typedef struct
{
    BOOLEAN                             initialized;
    inu_vision_proc__parameter_list_t   params;
} VE_MNGRP_veParamsT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static VE_MNGRG_streamIndexE    activeStreamIndex;
static VE_MNGRG_resolutionE     currResolution;
static VE_MNGRP_veParamsT       veParams;
static inu_streamerH*           veStreamers;
static BOOLEAN                  veIsReady = FALSE;
static UINT8                    veAutoExposureMode = VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED;
static UINT8                    vePowerLineFreq = VE_MNGRP_POWER_LINE_FREQ_DISABLED;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/


/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE VE_MNGRP_recursiveStopFunctions(inu_functionH functionH)
{
    inu_function__stopParamsT stopParams;
    inu_nodeH inputNode;
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    if (!functionH)
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "invalid stream\n");
        return VE_MNGR__ERR_INVALID_STREAM_ID;
    }

    if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(functionH)))
    {
        ret = inu_function__stop(functionH, &stopParams);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_WARN_E, NULL, "failed stopping %s\n", inu_ref__getUserName(functionH));
        }
    }

    inputNode = inu_node__getNextInputNode(functionH, NULL);
    while (inputNode)
    {
        ret = VE_MNGRP_recursiveStopFunctions(inputNode);
        inputNode = inu_node__getNextInputNode(functionH, inputNode);
    }
    return ret;
}

static ERRG_codeE VE_MNGRP_recursiveStartFunctions(inu_functionH functionH)
{
    inu_function__startParamsT startParams;
    inu_nodeH inputNode;
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    if (!functionH)
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "invalid stream\n");
        return VE_MNGR__ERR_INVALID_STREAM_ID;
    }

    if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(functionH)))
    {
        ret = inu_function__start(functionH, &startParams);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_WARN_E, NULL, "failed starting %s\n", inu_ref__getUserName(functionH));
        }
    }

    inputNode = inu_node__getNextInputNode(functionH, NULL);

    while (inputNode)
    {
        ret = VE_MNGRP_recursiveStartFunctions(inputNode);
        inputNode = inu_node__getNextInputNode(functionH, inputNode);
    }
    return ret;
}

static ERRG_codeE VE_MNGRP_stopAllStreams(inu_streamerH* streamerList)
{
    ERRG_codeE retVal = VE_MNGR__RET_SUCCESS;
    UINT32 i;

    for (i = 0; i < VE_MNGRG_STREAM_INDEX_MAX_E; i++)
    {
        if (streamerList[i] && inu_streamer__isUvcActive(streamerList[i]))
        {
            ERRG_codeE ret = VE_MNGRP_recursiveStopFunctions(streamerList[i]);
            if (ERRG_FAILED(ret))
            {
                retVal = ret;
                LOGG_PRINT(LOG_ERROR_E, NULL, "failed to stop stream out %s\n", inu_ref__getUserName(streamerList[i]));
            }
            inu_streamer__setResetUvcActive(streamerList[i], 0);
        }
    }
    return retVal;
}

static ERRG_codeE VE_MNGRP_getSensorGroupHandle(inu_streamerH streamerH, IO_HANDLE *sensorGroupHandle)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    inu_nodeH sensorGroupP;

    if (streamerH == NULL)
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "invalid stream\n");
        return VE_MNGR__ERR_INVALID_STREAM_ID;
    }

    ret = inu_graph__getOrigSensGroup(streamerH, &sensorGroupP);
    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group\n");
    }
    else
    {
        ret = inu_sensors_group__getSensorHandle(sensorGroupP, sensorGroupHandle);
        if (ERRG_FAILED(ret))
        {
            *sensorGroupHandle = NULL;

            LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor handle\n");
        }
    }
    return ret;
}

static ERRG_codeE VE_MNGRP_waitUntilReady(void)
{
   UINT32 startSec, deltaMsec = 0;
   UINT16 startMsec;

   OS_LYRG_getTime(&startSec, &startMsec);
   do
   {
      if (VE_MNGRP_readyToWork())
      {
         return VE_MNGR__RET_SUCCESS;
      }
      OS_LYRG_usleep(10000);
      deltaMsec = OS_LYRG_deltaMsec(startSec, startMsec);

   } while (deltaMsec <= VE_MNGRP_READY_TIMEOUT_MS);

   return VE_MNGR__ERR_TIMEOUT;
}

static void VE_MNGRP_printCalibrationInfo(inu_streamerH streamerH)
{
    inu_deviceH deviceH = inu_ref__getDevice(streamerH);
    UINT32      calibMode;
    char*       calibPath;

    inu_device__getCalibPath(deviceH, (const char**)&calibPath);
    inu_device__getCalibMode(deviceH, &calibMode);

    if (calibMode == 0)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "calibration was not found (searched in %s)\n", calibPath);

    }
    else
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "calibration was taken from %s (operating mode is %s)\n", calibPath, calibMode == 2 ? "nu4100" : "nu4000c0");
    }
}

ERRG_codeE VE_MNGRP_updateAlgType(VISION_PROCG_visionProcAlgTypeE algType)
{
    ERRG_codeE              ret = VE_MNGR__RET_SUCCESS;
    VE_MNGRG_bokehParamsT   bokehParams;

    LOGG_PRINT(LOG_INFO_E, NULL, "updating algType to %d\n", algType);

    VE_MNGRG_getBokehParams(&bokehParams);
    veParams.params.algType = algType;
    VE_MNGRG_setBokehParams(&bokehParams);

    return ret;
}

static ERRG_codeE VE_MNGRP_setAutoExposure(inu_streamerH streamerH, UINT8 enable)
{
    ERRG_codeE          ret = VE_MNGR__ERR_INVALID_STREAM_ID;
    inu_sensor_controlH sensorControl;

    if (streamerH != NULL)
    {
        ret =ISP_MNGRG_setEnableAllAe(enable);
    }
    else
    {
        LOGG_PRINT(LOG_WARN_E, NULL, "stream is empty\n");
        ret = VE_MNGR__ERR_INVALID_STREAM_ID;
    }
    return ret;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE VE_MNGRG_init(inu_streamerH* streamerList)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

#ifdef VE_MNGRP_SWITCH_VISION_PROC_ALG
    activeStreamIndex       = VE_MNGRG_STREAM_INDEX_BOKEH_E;
#else
    activeStreamIndex       = VE_MNGRG_STREAM_INDEX_REGULAR_E;
#endif
    currResolution          = VE_MNGRG_RES_1920_1080_E;
    veAutoExposureMode      = VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED;

    veParams.params.algType = VISION_PROC_NO_ALG;
    veParams.initialized    = FALSE;

    LOGG_PRINT(LOG_INFO_E, NULL, "initializing VE module\n");

    return ret;
}

ERRG_codeE VE_MNGRG_deinit(void)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_INFO_E, NULL, "de-initializing VE module\n");

    ret = VE_MNGRG_init(NULL);

    return ret;
}

ERRG_codeE VE_MNGRG_SetCameraLed(int value)
{
    ERRG_codeE                  ret = VE_MNGR__RET_SUCCESS;
    GPIO_DRVG_gpioSetValParamsT gpioParams;

    LOGG_PRINT(LOG_INFO_E, NULL, "set camera led control to %d\n", value);

    gpioParams.gpioNum = GPIO_DRVG_GPIO_11_E;
    if (value)
    {
        gpioParams.val = 0; //0 opens LED
    }
    else
    {
        gpioParams.val = 1; // closes LED
    }
    ret=IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

    return ret;
}

inu_streamerH VE_MNGRP_anyStreamIsRunning(void)
{
    unsigned int uvcCon;

    for (uvcCon = 0; uvcCon < UVC_NUM_OF_DEVICES; uvcCon++)
    {
        inu_streamerH *veStreamers = UVC_MAING_GetStreamTable(INU_FACTORY__CONN_UVC1_E + uvcCon);

        if (veStreamers)
        {
            UINT32 i;

            for (i = 0; i < VE_MNGRG_STREAM_INDEX_MAX_E; i++)
            {
                if (veStreamers[i] && inu_streamer__isUvcActive(veStreamers[i]))
                {
                    return veStreamers[i];
                }
            }
        }
    }
    return NULL;
}

BOOLEAN VE_MNGRP_readyToWork(void)
{
    UINT32          i;
    inu_streamerH   *veStreamers = UVC_MAING_GetStreamTable(INU_FACTORY__CONN_UVC1_E);

    if (veStreamers)
    {
        for (i = 0; i < VE_MNGRG_STREAM_INDEX_MAX_E; i++)
        {
            if (!veStreamers[i])
            {
                return FALSE;
            }
        }
        return TRUE;
    }
    return FALSE;
}

ERRG_codeE VE_MNGRG_startStream(inu_streamerH* streamerList)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    ret = VE_MNGRP_waitUntilReady();
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "timeout waiting to start streaming\n");
    }

    if(streamerList != NULL)
    {
        inu_streamerH streamToRun = streamerList[activeStreamIndex];

        if (streamToRun && inu_streamer__isUvcActive(streamToRun) == 0)
        {
            VE_MNGRP_printCalibrationInfo(streamToRun);

#ifdef VE_MNGRP_SWITCH_VISION_PROC_ALG
            ret = VE_MNGRP_updateAlgType(veParams.params.algType);
#endif
            ret = VE_MNGRP_recursiveStartFunctions(streamToRun);
            if(ERRG_FAILED(ret))
            {
                LOGG_PRINT(LOG_ERROR_E, NULL, "failed to start stream out %s\n", inu_ref__getUserName(streamToRun));
            }
            else
            {
                inu_streamer__setResetUvcActive(streamToRun, 1);
            }

            VE_MNGRG_setAutoExposure(streamToRun, VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED);
        }

        // start camera led
        VE_MNGRG_SetCameraLed(TRUE);
        LOGG_PRINT(LOG_INFO_E, NULL, "VE_MNGRG_SetCameraLed(TRUE)\n");
    }
    else
    {
        LOGG_PRINT(LOG_WARN_E, NULL, "stream list is empty\n");

        ret = VE_MNGR__ERR_INVALID_STREAM_ID;
    }
    return ret;
}

ERRG_codeE VE_MNGRG_stopStream(inu_streamerH* streamerList)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    if (streamerList[activeStreamIndex] && inu_streamer__isUvcActive(streamerList[activeStreamIndex]))
    {
        inu_streamerH streamToStop = streamerList[activeStreamIndex];

        ret = VE_MNGRP_recursiveStopFunctions(streamToStop);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to stop stream out %s\n", inu_ref__getUserName(streamToStop));
        }
        else
        {
            inu_streamer__setResetUvcActive(streamToStop, 0);
        }

        VE_MNGRG_setAutoExposure(streamToStop, VE_MNGRP_AUTO_EXPOSURE_CTRL_DISABLED);

        // close camera led
        VE_MNGRG_SetCameraLed(FALSE);
        LOGG_PRINT(LOG_INFO_E, NULL, "VE_MNGRG_SetCameraLed(FALSE)\n");
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "running stream is not available\n");
        ret = VE_MNGR__ERR_INVALID_STREAM_ID;
    }

    return ret;
}

ERRG_codeE VE_MNGRG_testSceneario(void)
{
    ERRG_codeE     ret = VE_MNGR__RET_SUCCESS;
    inu_streamerH  activeStream;
    UINT32         req, val;
    inu_streamerH *veStreamers = UVC_MAING_GetStreamTable(INU_FACTORY__CONN_UVC1_E);

    if(veStreamers)
    {
        activeStream = veStreamers[activeStreamIndex];
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "stream is not active\n");
        return VE_MNGR__ERR_INVALID_STREAM_ID;
    }

    for(req = 0; req < VE_MNGRG_NUM_OF_UVC_REQUESTS_E; req++)
    {
        ret = VE_MNGRG_getFrameRate(activeStream, (VE_MNGRG_uvcRequestE)req, &val);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update expsure\n");
        }
        LOGG_PRINT(LOG_INFO_E, NULL, "value = %d\n", val);
    }

    ret = VE_MNGRG_setFrameRate(activeStream, 10);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update expsure\n");
    }

    for(req = 0; req < VE_MNGRG_NUM_OF_UVC_REQUESTS_E; req++)
    {
        ret = VE_MNGRG_getExposure(activeStream, (VE_MNGRG_uvcRequestE)req, &val);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update expsure\n");
        }
        LOGG_PRINT(LOG_INFO_E, NULL, "Exposure value = %d\n", val);
    }

    ret = VE_MNGRG_setExposure(activeStream, 30000);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update expsure\n");
    }

    for(req = 0; req < VE_MNGRG_NUM_OF_UVC_REQUESTS_E; req++)
    {
        ret = VE_MNGRG_getBrightness(activeStream, (VE_MNGRG_uvcRequestE)req, &val);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update brightness\n");
        }
        LOGG_PRINT(LOG_INFO_E, NULL, "value = %d\n", val);
    }

    ret = VE_MNGRG_setBrightness(activeStream, 40);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update brightness\n");
    }

    for(req = 0; req < VE_MNGRG_NUM_OF_UVC_REQUESTS_E; req++)
    {
        ret = VE_MNGRG_getGain(activeStream, (VE_MNGRG_uvcRequestE)req, &val);
        if(ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update expsure\n");
        }
        LOGG_PRINT(LOG_INFO_E, NULL, "value = %d\n", val);
    }

    ret = VE_MNGRG_setGain(activeStream, 5);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "failed to update expsure\n");
    }
    return ret;
}

ERRG_codeE VE_MNGRG_getFrameRate(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *fps)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_INFO_E, NULL, "%s FPS\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            inu_nodeH sensorGroupP;
            IO_HANDLE sensorGroupHandle;
            SENSORS_MNGRG_sensorInfoT *sensorInfoP;

            ret = VE_MNGRP_getSensorGroupHandle(streamerH, &sensorGroupHandle);
            if(ERRG_FAILED(ret))
            {
                LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group\n");
            }
            else
            {
                ret = SENSORS_MNGRP_getSensorFps(sensorGroupHandle, fps, INU_DEFSG_SENSOR_CONTEX_A);
            }
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *fps = INU_DEFSG_FR_20_E;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        {
            *fps = 10;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        {
            *fps = 60;
             return VE_MNGR__RET_SUCCESS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}

ERRG_codeE VE_MNGRG_setFrameRate(inu_streamerH streamerH, UINT32 fps)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    IO_HANDLE  sensorGroupHandle;

    LOGG_PRINT(LOG_INFO_E, NULL, "setting frame rate (%d)\n", fps);

    ret = VE_MNGRP_getSensorGroupHandle(streamerH, &sensorGroupHandle);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group\n");
    }
    else
    {
        ret = SENSORS_MNGRP_setSensorFps(sensorGroupHandle, &fps, INU_DEFSG_SENSOR_CONTEX_A);
    }
    return ret;
}

ERRG_codeE VE_MNGRG_getResolutionMode(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, VE_MNGRG_resolutionE* res)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_INFO_E, NULL, "%s resolution mode\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            *res = currResolution;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *res = VE_MNGRG_RES_1920_1080_E;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        {
            *res = VE_MNGRG_RES_1280_720_E;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        {
            *res = VE_MNGRG_RES_1920_1080_E;
             return VE_MNGR__RET_SUCCESS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}

ERRG_codeE VE_MNGRG_setResolutionMode(inu_streamerH streamerH, VE_MNGRG_resolutionE res)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_INFO_E, NULL, "setting resolution (mode %d)\n", res);

    // TBD
    currResolution = res;

    return ret;
}

ERRG_codeE VE_MNGRG_getBrightness(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *brightness)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "%s brightness\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            UINT32 min, max, exposure;

            ret = VE_MNGRG_getExposure(streamerH, VE_MNGRG_UVC_REQ_GET_MIN_E, &min);
            if(ERRG_SUCCEEDED(ret))
            {
                ret = VE_MNGRG_getExposure(streamerH, VE_MNGRG_UVC_REQ_GET_MAX_E, &max);
                if(ERRG_SUCCEEDED(ret))
                {
                    ret = VE_MNGRG_getExposure(streamerH, VE_MNGRG_UVC_REQ_GET_CURR_E, &exposure);
                    if(ERRG_SUCCEEDED(ret) && min != max)
                    {
                        /*                                                                        */
                        /* Translate brightness (range 0->100) to exposure (range min -> max):    */
                        /*                                                                        */
                        /*                     exposure - min                                     */
                        /*       brightness = ----------------  *  100                            */
                        /*                        max - min                                       */
                        /*                                                                        */
                        *brightness = (((exposure - min) * 100) / (max - min));
                    }
                }
            }
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *brightness = 50;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        {
            *brightness = 0;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        {
            *brightness = 100;
             return VE_MNGR__RET_SUCCESS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}

ERRG_codeE VE_MNGRG_setBrightness(inu_streamerH streamerH, UINT32 brightness)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    UINT32 min, max;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "setting brightness (%d)\n", brightness);

    ret = VE_MNGRG_getExposure(streamerH, VE_MNGRG_UVC_REQ_GET_MIN_E, &min);
    if(ERRG_SUCCEEDED(ret))
    {
        ret = VE_MNGRG_getExposure(streamerH, VE_MNGRG_UVC_REQ_GET_MAX_E, &max);
        if(ERRG_SUCCEEDED(ret) && min != max)
        {
            /*                                                                             */
            /* Translate exposure (range min -> max) to brightness (range 0->100):         */
            /*                                                                             */
            /*                   brightness * (max - min)                                  */
            /*       exposure = --------------------------  - min                          */
            /*                             100                                             */
            /*                                                                             */
            UINT32 exposure = (brightness * (max - min)) / 100 + min;

            ret = VE_MNGRG_setExposure(streamerH, exposure);
        }
    }
    return ret;
}

ERRG_codeE VE_MNGRG_getAutoExposure(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT8 *autoExposureMode)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "%s auto exposure\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            *autoExposureMode = veAutoExposureMode;
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *autoExposureMode = VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED;
            return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_RES_E:
        {
            *autoExposureMode = VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED | VE_MNGRP_AUTO_EXPOSURE_CTRL_DISABLED;
            return VE_MNGR__RET_SUCCESS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}
/*input values: enable=VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED=0x8, disable =VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED=0x8=0x1*/
ERRG_codeE VE_MNGRG_setAutoExposure(inu_streamerH streamerH, UINT8 autoExposureMode)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    if (autoExposureMode != VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED && autoExposureMode != VE_MNGRP_AUTO_EXPOSURE_CTRL_DISABLED)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "illegal auto exposure mode %d \n", autoExposureMode);
        return VE_MNGR__ERR_INVALID_ARGS;
    }

    LOGG_PRINT(LOG_DEBUG_E, NULL, "setting auto exposure to 0x%x\n", autoExposureMode);

    if (VE_MNGRP_anyStreamIsRunning() != NULL) // start/stop auto exposure during running stream
    {
        if (autoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED && veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_DISABLED)
        {
            ret = VE_MNGRP_setAutoExposure(streamerH, TRUE);
            if (veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
            if (vePowerLineFreq!=0)
            {
                ISP_MNGRG_setFlickerAe(vePowerLineFreq); //set power line frequency if configured, relevant to AE mode
                LOGG_PRINT(LOG_DEBUG_E, NULL, "called ISP_MNGRG_setFlickerAe after setting AE\n");
            }
        }
        else if (autoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_DISABLED && veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
        {
            ret = VE_MNGRP_setAutoExposure(streamerH, FALSE);
        }
    }

    if(ERRG_SUCCEEDED(ret))
    {
        veAutoExposureMode = autoExposureMode;
    }

    return ret;
}

ERRG_codeE VE_MNGRG_getExposure(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *exposure)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "%s exposure\n", VE_MNGRP_UVC_REQ_STR(req));

    /* range exposure of nu4100 is in milliseconds while in uvc control the units are in 0.1ms, so the range will be X10 */
    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            inu_nodeH sensorGroupP;
            IO_HANDLE sensorGroupHandle;
            SENSORS_MNGRG_sensorInfoT *sensorInfoP;
            UINT32 expValue=0;

            ret = VE_MNGRP_getSensorGroupHandle(streamerH, &sensorGroupHandle);
            if(ERRG_FAILED(ret))
            {
                *exposure = 15000 * 10;
                LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group, return defaut\n");
            }
            else
            {
                ret = SENSORS_MNGRG_getSensorExpTime(sensorGroupHandle, &expValue, INU_DEFSG_SENSOR_CONTEX_A);
                *exposure = expValue * 10;
                LOGG_PRINT(LOG_DEBUG_E, NULL, "exposure get response = %d \n\r", *exposure);
            }
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *exposure = 15000 * 10;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        {
            *exposure = 2*1000 * 10; /*2 msec*/
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        {
            *exposure = 33*1000 * 10; /*33 msec*/
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_RES_E:
        {
            *exposure = 1 * 10;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_INFO_E:
        {
            if (veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
            {
                *exposure &= ~0x04; // clear bit 2 (Disabled due to automatic mode)
            }
            return VE_MNGR__RET_SUCCESS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}


ERRG_codeE VE_MNGRG_setExposure(inu_streamerH streamerH, UINT32 exposure)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    IO_HANDLE sensorGroupHandle;
    UINT32 exposure_l;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "setting exposure (%d)\n", exposure);

    if (veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "auto-exposure is set, manual setting is ignored\n");
        return VE_MNGR__RET_SUCCESS;
    }

    /* range exposure of nu4100 is in milliseconds while in uvc control the units are in 0.1ms, so the range will be X0.1 */
    exposure_l = exposure / 10;

    ret = VE_MNGRP_getSensorGroupHandle(streamerH, &sensorGroupHandle);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group\n");
    }
    else
    {
        ret = SENSORS_MNGRG_setSensorExpTime(sensorGroupHandle, &exposure_l, INU_DEFSG_SENSOR_CONTEX_A);
    }
    return ret;
}

ERRG_codeE VE_MNGRG_getGain(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *gain)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "%s gain\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            inu_nodeH sensorGroupP;
            IO_HANDLE sensorGroupHandle;
            SENSORS_MNGRG_sensorInfoT *sensorInfoP;
            float analogGain, digitalGain;

            ret = VE_MNGRP_getSensorGroupHandle(streamerH, &sensorGroupHandle);
            if(ERRG_FAILED(ret))
            {
                LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group\n");
            }
            else
            {
                ret = SENSORS_MNGRG_getSensorGain(sensorGroupHandle, &analogGain, &digitalGain, INU_DEFSG_SENSOR_CONTEX_A);
                if(ERRG_SUCCEEDED(ret))
                {
                    *gain = (UINT32)analogGain;
                }
            }
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *gain = 4;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        {
            *gain = 1;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        {
            *gain = 12;
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_INFO_E:
        {
            if (veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
            {
                *gain &= ~0x04; // clear bit 2 (Disabled due to automatic mode)
            }
            return VE_MNGR__RET_SUCCESS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}
ERRG_codeE VE_MNGRG_getPowerFreq(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT32 *powerFreq)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "%s power line frequency\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            *powerFreq = vePowerLineFreq;
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *powerFreq = VE_MNGRP_POWER_LINE_FREQ_60HZ; // 60 hz
             return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_INFO_E:
        {
            *powerFreq &= ~0x08; // clear bit 4 (Disable automatic mode) 0x3
            return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        case VE_MNGRG_UVC_REQ_GET_RES_E:
        {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "type %d is not valid\n", req);
            return VE_MNGR__ERR_INVALID_ARGS;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}

ERRG_codeE VE_MNGRG_setPowerFreq(inu_streamerH streamerH, UINT32 powerFreq)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    IO_HANDLE sensorGroupHandle;

    LOGG_PRINT(LOG_INFO_E, NULL, "setting power line frequency (%d)\n", powerFreq);
    if ( (powerFreq==VE_MNGRP_POWER_LINE_FREQ_60HZ) || (powerFreq==VE_MNGRP_POWER_LINE_FREQ_50HZ) || (powerFreq==VE_MNGRP_POWER_LINE_FREQ_DISABLED) )
    {
        if (vePowerLineFreq!=powerFreq)
        {
            vePowerLineFreq=powerFreq;
            if (veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
            {
                LOGG_PRINT(LOG_DEBUG_E, NULL, "setting power line frequency (%d) to ISP, value has changed\n", powerFreq);
                ISP_MNGRG_setFlickerAe(vePowerLineFreq);
            }
        }
    }
    else
    {
         LOGG_PRINT(LOG_ERROR_E, NULL, "recived illegal value for power line frequency\n");
         ret=VE_MNGR__ERR_INVALID_ARGS;
    }

    return ret;
}

ERRG_codeE VE_MNGRG_setGain(inu_streamerH streamerH, UINT32 gain)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    IO_HANDLE sensorGroupHandle;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "setting gain (%d)\n", gain);

    if (veAutoExposureMode == VE_MNGRP_AUTO_EXPOSURE_CTRL_ENABLED)
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "auto-exposure is set, manual setting is ignored\n");
        return VE_MNGR__ERR_ILLEGAL_STATE;
    }

    ret = VE_MNGRP_getSensorGroupHandle(streamerH, &sensorGroupHandle);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "failed to find sensor group\n");
    }
    else
    {
        float analogGain, digitalGain;

        ret = SENSORS_MNGRG_getSensorGain(sensorGroupHandle, &analogGain, &digitalGain, INU_DEFSG_SENSOR_CONTEX_A);

        analogGain = (float)gain;
        ret = SENSORS_MNGRG_setSensorGain(sensorGroupHandle, &analogGain, &digitalGain, INU_DEFSG_SENSOR_CONTEX_A);
    }
    return ret;
}

ERRG_codeE VE_MNGRG_getPrivacy(inu_streamerH streamerH, VE_MNGRG_uvcRequestE req, UINT8 *privacy)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

    LOGG_PRINT(LOG_DEBUG_E, NULL, "%s privacy\n", VE_MNGRP_UVC_REQ_STR(req));

    switch(req)
    {
        case VE_MNGRG_UVC_REQ_GET_CURR_E:
        {
            if (streamerH)
            {
                *privacy = (UINT8)(inu_streamer__isShtterActive(streamerH) ? 1 : 0);
            }
            else
            {
                *privacy = 0;
            }
            break;
        }
        case VE_MNGRG_UVC_REQ_GET_DEFAULT_E:
        {
            *privacy = 0;
            return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MIN_E:
        {
            *privacy = 0;
            return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_MAX_E:
        {
            *privacy = 1;
            return VE_MNGR__RET_SUCCESS;
        }
        case VE_MNGRG_UVC_REQ_GET_RES_E:
        {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "type %d is not valid\n", req);
            return VE_MNGR__ERR_INVALID_ARGS;
        }
        case VE_MNGRG_UVC_REQ_GET_INFO_E:
        {
            *privacy &= ~0x02;   // remove bit #1 (SET request is not supported)
            break;
        }
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "type %d is not valid\n", req);
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    return ret;
}

ERRG_codeE VE_MNGRG_setBokehParams(VE_MNGRG_bokehParamsT* bokehParams)
{
    ERRG_codeE   ret = VE_MNGR__RET_SUCCESS;
    inu_nodeH    inputNode;

    if (bokehParams)
    {
        inu_streamerH *veStreamers = UVC_MAING_GetStreamTable(INU_FACTORY__CONN_UVC1_E);

        LOGG_PRINT(LOG_INFO_E, NULL, "updating bokeh params\n");

        inputNode = inu_node__getNextInputNode(veStreamers[VE_MNGRG_STREAM_INDEX_BOKEH_E], NULL);

        while (inputNode)
        {
            if(strcmp(inu_ref__getUserName(inputNode), VE_MNGRP_VISION_PROC_NODE_NAME) == 0)
            {
                break;
            }
            inputNode = inu_node__getNextInputNode(inputNode, NULL);
        }

        if (inputNode)
        {
#ifndef VE_MNGRP_SWITCH_VISION_PROC_ALG
            veParams.params.algType = VISION_PROC_BOKEH;
#endif
            memcpy(&veParams.params.bokehParams, bokehParams, sizeof(veParams.params.bokehParams));
            #ifdef VISION_PROC
            inu_vision_proc__updateParams(inputNode, &veParams.params);
            #endif

            veParams.initialized = TRUE;
        }
        else
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "stream was not found\n");
            ret = VE_MNGR__ERR_INVALID_STREAM_ID;
        }
    }
    else
    {
        LOGG_PRINT(LOG_WARN_E, NULL, "params are invalid\n");
    }

    return ret;
}

ERRG_codeE VE_MNGRG_getBokehParams(VE_MNGRG_bokehParamsT* bokehParams)
{
    ERRG_codeE                          ret = VE_MNGR__RET_SUCCESS;
    DATA_BASEG_visionProcDataDataBaseT  *visionProcDataBaseP;

    if (bokehParams)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "get bokeh params\n");

        if (!veParams.initialized)
        {
            DATA_BASEG_visionProcDataDataBaseT *visionProcDataBaseP;

            DATA_BASEG_accessDataBase((UINT8**)&visionProcDataBaseP, DATA_BASEG_VISION_PROC);
            memcpy(&veParams.params.bokehParams, &visionProcDataBaseP->bokehParams, sizeof(VISION_PROCG_bokehParamsT));
            DATA_BASEG_accessDataBaseRelease(DATA_BASEG_VISION_PROC);
        }

        memcpy(bokehParams, &veParams.params.bokehParams, sizeof(VE_MNGRG_bokehParamsT));
    }
    else
    {
        LOGG_PRINT(LOG_WARN_E, NULL, "params are invalid\n");
    }
    return ret;
}

ERRG_codeE VE_MNGRG_enableBokeh(VE_MNGRG_bokehParamsT* bokehParams)
{
    ERRG_codeE      ret = VE_MNGR__RET_SUCCESS;
    inu_streamerH   *veStreamers = UVC_MAING_GetStreamTable(INU_FACTORY__CONN_UVC1_E);

    LOGG_PRINT(LOG_INFO_E, NULL, "enable bokeh\n");

    if(!veStreamers)
    {
        LOGG_PRINT(LOG_WARN_E, NULL, "stream list is empty\n");

        return VE_MNGR__ERR_INVALID_STREAM_ID;
    }

    ret = VE_MNGRG_setBokehParams(bokehParams);
    if(ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "stream %s was not found\n", VE_MNGRP_VISION_PROC_NODE_NAME);
        ret = VE_MNGR__ERR_INVALID_STREAM_ID;
    }

#ifdef VE_MNGRP_SWITCH_VISION_PROC_ALG
    VE_MNGRP_updateAlgType(VISION_PROC_BOKEH);
#else
    if(activeStreamIndex != VE_MNGRG_STREAM_INDEX_BOKEH_E && inu_streamer__isUvcActive(veStreamers[activeStreamIndex]))
    {
        ret = VE_MNGRP_recursiveStartFunctions(veStreamers[VE_MNGRG_STREAM_INDEX_BOKEH_E]);
        inu_streamer__setResetUvcActive(veStreamers[VE_MNGRG_STREAM_INDEX_REGULAR_E], 0);
        inu_streamer__setResetUvcActive(veStreamers[VE_MNGRG_STREAM_INDEX_BOKEH_E], 1);
        ret = VE_MNGRP_recursiveStopFunctions(veStreamers[VE_MNGRG_STREAM_INDEX_REGULAR_E]);
    }

    activeStreamIndex = VE_MNGRG_STREAM_INDEX_BOKEH_E;
#endif

    return ret;
}

ERRG_codeE VE_MNGRG_disableBokeh(VE_MNGRG_disableParamsT* disableParams)
{
    ERRG_codeE      ret = VE_MNGR__RET_SUCCESS;
    inu_streamerH   *veStreamers = UVC_MAING_GetStreamTable(INU_FACTORY__CONN_UVC1_E);

    LOGG_PRINT(LOG_INFO_E, NULL, "disable bokeh\n");

    if(!veStreamers)
    {
        LOGG_PRINT(LOG_WARN_E, NULL, "stream list is empty\n");

        return VE_MNGR__ERR_INVALID_STREAM_ID;
    }

#ifdef VE_MNGRP_SWITCH_VISION_PROC_ALG
    VE_MNGRP_updateAlgType(VISION_PROC_NO_ALG);
#else
    if(activeStreamIndex == VE_MNGRG_STREAM_INDEX_BOKEH_E && inu_streamer__isUvcActive(veStreamers[activeStreamIndex]))
    {
        ret = VE_MNGRP_recursiveStartFunctions(veStreamers[VE_MNGRG_STREAM_INDEX_REGULAR_E]);
        inu_streamer__setResetUvcActive(veStreamers[VE_MNGRG_STREAM_INDEX_BOKEH_E], 0);
        inu_streamer__setResetUvcActive(veStreamers[VE_MNGRG_STREAM_INDEX_REGULAR_E], 1);
        ret = VE_MNGRP_recursiveStopFunctions(veStreamers[VE_MNGRG_STREAM_INDEX_BOKEH_E]);
    }

    activeStreamIndex = VE_MNGRG_STREAM_INDEX_REGULAR_E;
#endif

    return ret;
}

ERRG_codeE VE_MNGRG_execute(VE_MNGRG_messageT* msg, UINT8* output, UINT32* outputLen)
{
    ERRG_codeE ret = VE_MNGR__RET_SUCCESS;
    void* input = msg->hdr.msgLen == 0 ? NULL : msg->data;

    if(!VE_MNGRP_readyToWork())
    {
        *outputLen = 0;
        return VE_MNGR__ERR_NOT_READY;
    }

    switch(msg->hdr.cmd)
    {
        case VE_MNGRG_ENABLE_BOKEH_E:
        {
            ret = VE_MNGRG_enableBokeh((VE_MNGRG_bokehParamsT*)input);
            *outputLen = 0;
            break;
        }

        case VE_MNGRG_DISABLE_BOKEH_E:
        {
            ret = VE_MNGRG_disableBokeh((VE_MNGRG_disableParamsT*)input);
            *outputLen = 0;
            break;
        }

        case VE_MNGRG_SET_BOKEH_PARAMS_E:
        {
            ret = VE_MNGRG_setBokehParams((VE_MNGRG_bokehParamsT*)input);
            *outputLen = 0;
            break;
        }

        case VE_MNGRG_GET_BOKEH_PARAMS_E:
        {
            if(output && outputLen && *outputLen >= sizeof(VE_MNGRG_bokehParamsT))
            {
                ret = VE_MNGRG_getBokehParams((VE_MNGRG_bokehParamsT*)output);
                *outputLen = sizeof(VE_MNGRG_bokehParamsT);
            }
            else
            {
                ret = VE_MNGR__ERR_INVALID_ARGS;
                *outputLen = 0;
            }
            break;
        }

#ifdef VE_MNGRP_TEST_INTERNAL_STREAMS
        case VE_MNGRG_INIT_E:
        {
            ret = VE_MNGRG_init();
            *outputLen = 0;
            break;
        }

        case VE_MNGRG_START_STREAM_E:
        {
            ret = VE_MNGRG_startStream(streamPool);
            *outputLen = 0;
            break;
        }

        case VE_MNGRG_STOP_STREAM_E:
        {
            ret = VE_MNGRG_stopStream(streamPool);
            *outputLen = 0;
            break;
        }

        case VE_MNGRG_TEST_E:
        {
            ret = VE_MNGRG_testSceneario();
            *outputLen = 0;
            break;
        }
#endif
        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "command %d is not valid\n", msg->hdr.cmd);

            ret = VE_MNGR__ERR_INVALID_ARGS;
            *outputLen = 0;
        }
    }

    return ret;
}

#endif