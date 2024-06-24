/******************************************************************************\
|* Copyright 2010, Dream Chip Technologies GmbH. used with permission by      *|
|* VeriSilicon.                                                               *|
|* Copyright (c) <2022> by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")     *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

/* VeriSilicon 2022 */

/**
 * @file isi.h
 *
 * @brief Interface description for image sensor specific implementation (iss).
 *
 *****************************************************************************/
/**
 * @page module_name_page Module Name
 * Describe here what this module does.
 *
 * For a detailed list of functions and implementation detail refer to:
 * - @ref module_name
 *
 * @defgroup isi Independent Sensor Interface
 * @{
 *
 */
#ifndef __ISI_H__
#define __ISI_H__

#include <ebase/types.h>
#include <hal/hal_api.h>

#include <isi/isi_common.h>
#include <cameric_drv/cameric_drv_common.h>
#include <cam_device/cam_device_sensor_defs.h>
#include <vvsensor.h>
#include <isi/isi_otp.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct IsiCamDrvConfig_s;

typedef void *IsiSensorHandle_t;
typedef struct IsiSensor_s IsiSensor_t;


/*****************************************************************************/
/**
 *          IsiSensorInstanceConfig_t
 *
 * @brief   Config structure to create a new sensor instance
 *
 */
/*****************************************************************************/
typedef struct IsiSensorInstanceConfig_s
{
    HalHandle_t           HalHandle;          /**< Handle of HAL session to use. */
    uint32_t              HalDevID;           /**< HAL device ID of this sensor. */
    uint8_t               I2cBusNum;          /**< The I2C bus the sensor is connected to. */
    uint16_t              SlaveAddr;          /**< The I2C slave addr the sensor is configured to. */
    uint8_t               I2cAfBusNum;        /**< The I2C bus the ad module is connected to. */
    uint16_t              SlaveAfAddr;        /**< The I2C slave addr of the af module is configured to */
    uint32_t              SensorModeIndex;
    IsiSensor_t           *pSensor;           /**< Sensor driver interface */
    IsiSensorHandle_t     hSensor;            /**< Sensor handle returned by IsiCreateSensorIss */
    char szSensorNodeName[32];
} IsiSensorInstanceConfig_t;

typedef struct vvsensor_mode_s          IsiMode_t;
typedef struct vvsensor_enum_mode_s     IsiEnumMode_t;
typedef struct vvsensor_auto_fps_s      IsiAutoFps_t;
typedef struct vvsensor_blc_s           IsiBlc_t;
typedef struct vvsensor_white_balance_s IsiWB_t;
typedef struct vvsensor_test_pattern_s  IsiTpg_t;
typedef struct vvsensor_compand_curve_s IsiCompandCurve_t;

typedef struct IsiResolution_s
{
    uint16_t width;
    uint16_t height;
}IsiResolution_t;

/*****************************************************************************/
/**
 * @brief   This structure defines the sensor capabilities.
 */
/*****************************************************************************/
typedef struct IsiCaps_s {
    uint32_t BusWidth;                  /**< supported bus-width */
    uint32_t Mode;                      /**< supported operating modes */
    uint32_t FieldSelection;            /**< sample fields */
    uint32_t YCSequence;
    uint32_t Conv422;
    uint32_t BPat;                      /**< bayer pattern */
    uint32_t HPol;                      /**< horizontal polarity */
    uint32_t VPol;                      /**< vertical polarity */
    uint32_t Edge;                      /**< sample edge */
    IsiResolution_t Resolution;         /**< supported resolutions */
    uint32_t SmiaMode;
    uint32_t MipiMode;
    uint32_t MipiLanes;
    uint32_t enableHdr;
} IsiCaps_t;

typedef struct IsiIspStatus_s {
    bool_t useSensorAE;
    bool_t useSensorBLC;
    bool_t useSensorAWB;
} IsiIspStatus_t;

typedef enum IsiFocusPosMode_enum {
    ISI_FOCUS_POS_ABSOLUTE = 0,
    ISI_FOCUS_POS_RELATIVE,
} IsiFocusPosMode_e;

typedef struct IsiFocusCalibAttr_s {
    uint32_t minPos;
    uint32_t maxPos;
    uint32_t minStep;
} IsiFocusCalibAttr_t;

typedef struct IsiFocusPos_s {
    IsiFocusPosMode_e PosType;
    uint32_t Pos;
} IsiFocusPos_t;

typedef struct IsiUserGain_s {
    float again;
    float dgain;
} IsiUserGain_t;

/*****************************************************************************/
/**
 *          IsiSetCsiConfig
 *
 * @brief   mipi config.
 *
 * @param   handle      Sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 * @retval  RET_OUTOFMEM
 *
 *****************************************************************************/
RESULT IsiSetCsiConfig
(
    IsiSensorHandle_t   handle,
    uint32_t            clk
);


/*****************************************************************************/
/**
 *          IsiCreateIss
 *
 * @brief   This function creates a new sensor instance.
 *
 * @param   pConfig     configuration of the new sensor
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 * @retval  RET_OUTOFMEM
 *
 *****************************************************************************/
RESULT IsiCreateIss
(
    IsiSensorInstanceConfig_t   *pConfig
);


/*****************************************************************************/
/**
 *          IsiReleaseIss
 *
 * @brief   The function destroys/releases a sensor instance.
 *
 * @param   handle      sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NOTSUPP
 *
 *****************************************************************************/
RESULT IsiReleaseIss
(
    IsiSensorHandle_t   handle
);


/*****************************************************************************/
/**
 *          IsiReadRegIss
 *
 * @brief   reads a given number of bytes from the image sensor device
 *
 * @param   handle              Handle to image sensor device
 * @param   Addr                register address
 * @param   pValue              value to read
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_NOTSUPP
 *
 *****************************************************************************/
RESULT IsiReadRegIss
(
    IsiSensorHandle_t   handle,
    const uint32_t      Addr,
    uint32_t            *pValue
);


/*****************************************************************************/
/**
 *          IsiWriteRegIss
 *
 * @brief   writes a given number of bytes to the image sensor device by
 *          calling the corresponding sensor-function
 *
 * @param   handle              Handle to image sensor device
 * @param   Addr                register address
 * @param   Value               value to write
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NOTSUPP
 *
 *****************************************************************************/
RESULT IsiWriteRegIss
(
    IsiSensorHandle_t   handle,
    const uint32_t      Addr,
    const uint32_t      Value
);


/*****************************************************************************/
/**
 *          IsiGetModeIss
 *
 * @brief   get cuurent sensor mode info.
 *
 * @param   handle      Sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 *
 *****************************************************************************/
RESULT IsiGetModeIss
(
    IsiSensorHandle_t   handle,
    IsiMode_t           *pMode
);


/*****************************************************************************/
/**
 *          IsiSetModeIss
 *
 * @brief   set cuurent sensor mode info.
 *
 * @param   handle      Sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 *
 *****************************************************************************/
RESULT IsiSetModeIss
(
    IsiSensorHandle_t   handle,
    IsiMode_t           *pMode
);


/*****************************************************************************/
/**
 *          IsiEnumModeIss
 *
 * @brief   query sensor info.
 *
 * @param   handle                  sensor instance handle
 * @param   pEnumMode               sensor query mode
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiEnumModeIss
(
    IsiSensorHandle_t   handle,
    IsiEnumMode_t       *pEnumMode
);


/*****************************************************************************/
/**
 *          IsiGetCapsIss
 *
 * @brief   fills in the correct pointers for the sensor description struct
 *
 * @param   handle      Sensor instance handle
 * @param   pCaps       Sensor caps pointer
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetCapsIss
(
    IsiSensorHandle_t   handle,
    IsiCaps_t           *pCaps
);


/*****************************************************************************/
/**
 *          IsiSetupIss
 *
 * @brief   Setup of the image sensor considering the given configuration.
 *
 * @param   handle      Sensor instance handle
 * @param   pConfig     pointer to sensor configuration structure
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetupIss
(
    IsiSensorHandle_t   handle,
    const IsiCaps_t     *pIsiCaps
);


/*****************************************************************************/
/**
 *          IsiCheckConnectionIss
 *
 * @brief   Checks the connection to the camera sensor, if possible.
 *
 * @param   handle      Sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiCheckConnectionIss
(
    IsiSensorHandle_t   handle
);


/*****************************************************************************/
/**
 *          IsiGetRevisionIss
 *
 * @brief   This function reads the sensor revision register and returns it.
 *
 * @param   handle      sensor instance handle
 * @param   pRevision   pointer to revision
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_NOTSUPP
 *
 *****************************************************************************/
RESULT IsiGetRevisionIss
(
    IsiSensorHandle_t   handle,
    uint32_t            *pRevision
);


/*****************************************************************************/
/**
 *          IsiSetStreamingIss
 *
 * @brief   Enables/disables streaming of sensor data, if possible.
 *
 * @param   handle      Sensor instance handle
 * @param   on          new streaming state (BOOL_TRUE=on, BOOL_FALSE=off)
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 * @retval  RET_WRONG_STATE
 *
 *****************************************************************************/
RESULT IsiSetStreamingIss
(
    IsiSensorHandle_t   handle,
    bool_t              on
);


/*****************************************************************************/
/**
 *          IsiExposureControlIss
 *
 * @brief   Sets the exposure values (gain & integration time) of a sensor
 *          instance
 *
 * @param   handle                  sensor instance handle
 * @param   NewGain                 newly calculated gain to be set
 * @param   NewIntegrationTime      newly calculated integration time to be set
 * @param   pNumberOfFramesToSkip   number of frames to skip until AE is
 *                                  executed again
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiExposureControlIss
(
    IsiSensorHandle_t   handle,
    float               NewGain,
    float               NewIntegrationTime,
    uint8_t             *pNumberOfFramesToSkip,
    float               *pSetGain,
    float               *pSetIntegrationTime,
    float               *hdr_ratio
);


/*****************************************************************************/
/**
 *          IsiGetGainLimitsIss
 *
 * @brief   Returns the gain minimal and maximal values of a sensor
 *          instance
 *
 * @param   handle      sensor instance handle
 * @param   pMinGain    Pointer to a variable receiving minimal gain value
 * @param   pMaxGain    Pointer to a variable receiving maximal gain value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetGainLimitsIss
(
    IsiSensorHandle_t   handle,
    float               *pMinGain,
    float               *pMaxGain
);


/*****************************************************************************/
/**
 *          IsiGetIntegrationTimeLimitsIss
 *
 * @brief   Returns the integration time minimal and maximal values of a sensor
 *          instance
 *
 * @param   handle                  sensor instance handle
 * @param   pMinIntegrationTime     Pointer to a variable receiving minimal integration time value
 * @param   pMaxIntegrationTime     Pointer to a variable receiving maximal integration time value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetIntegrationTimeLimitsIss
(
    IsiSensorHandle_t   handle,
    float               *pMinIntegrationTime,
    float               *pMaxIntegrationTime
);


/*****************************************************************************/
/**
 *          IsiGetCurrentExposureIss
 *
 * @brief   Returns the currently adjusted AE values
 *
 * @param   handle       sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetCurrentExposureIss
(
    IsiSensorHandle_t   handle,
    float               *pCurGain,
    float               *pCurIntegrationTime,
    float               *pCurHdrRatio
);


/*****************************************************************************/
/**
 *          IsiGetGainIss
 *
 * @brief   Reads gain values from the image sensor module.
 *
 * @param   handle                  sensor instance handle
 * @param   pSetGain                set gain
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetGainIss
(
    IsiSensorHandle_t   handle,
    float               *pGain
);

RESULT IsiGetVSGainIss
(
    IsiSensorHandle_t   handle,
    float               *pGain
);

RESULT IsiGetLongGainIss
(
    IsiSensorHandle_t   handle,
    float               *pGain
);


/*****************************************************************************/
/**
 *          IsiGetGainIncrementIss
 *
 * @brief   Get smalles possible gain increment.
 *
 * @param   handle                  sensor instance handle
 * @param   pIncr                   gain increment
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetGainIncrementIss
(
    IsiSensorHandle_t   handle,
    float               *pIncr
);


/*****************************************************************************/
/**
 *          IsiSetGainIss
 *
 * @brief   Writes gain values to the image sensor module.
 *
 * @param   handle                  sensor instance handle
 * @param   NewGain                 gain to be set
 * @param   pSetGain                set gain
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetGainIss
(
    IsiSensorHandle_t   handle,
    float               NewGain,
    float               *pSetGain,
    float               *hdr_ratio
);


/*****************************************************************************/
/**
 *          IsiGetIntegrationTimeIss
 *
 * @brief   Reads integration time values from the image sensor module.
 *
 * @param   handle                  sensor instance handle
 * @param   pSetIntegrationTime     set integration time
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetIntegrationTimeIss
(
    IsiSensorHandle_t   handle,
    float               *pSetIntegrationTime
);

RESULT IsiGetVSIntegrationTimeIss
(
    IsiSensorHandle_t   handle,
    float               *pSetIntegrationTime
);

RESULT IsiGetLongIntegrationTimeIss
(
    IsiSensorHandle_t   handle,
    float               *pSetIntegrationTime
);


/*****************************************************************************/
/**
 *          IsiGetIntegrationTimeIncrementIss
 *
 * @brief   Get smalles possible integration time increment.
 *
 * @param   handle                  sensor instance handle
 * @param   pIncr                   increment
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetIntegrationTimeIncrementIss
(
    IsiSensorHandle_t   handle,
    float               *pIncr
);


/*****************************************************************************/
/**
 *          IsiSetIntegrationTimeIss
 *
 * @brief   Writes integration time values to the image sensor module.
 *
 * @param   handle                  sensor instance handle
 * @param   NewIntegrationTime      integration time to be set
 * @param   pSetIntegrationTime     set integration time
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetIntegrationTimeIss
(
    IsiSensorHandle_t   handle,
    float               NewIntegrationTime,
    float               *pSetIntegrationTime,
    float               *hdr_ratio
);


/*****************************************************************************/
/**
 *          IsiGetFpsIss
 *
 * @brief   Get Sensor Fps Config.
 *
 * @param   handle                  sensor instance handle
 * @param   pFps                    current fps
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetFpsIss
(
    IsiSensorHandle_t   handle,
    uint32_t            *pFps
);


/*****************************************************************************/
/**
 *          IsiSetFpsIss
 *
 * @brief   set Sensor Fps Config.
 *
 * @param   handle                  sensor instance handle
 * @param   Fps                     Setfps
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetFpsIss
(
    IsiSensorHandle_t   handle,
    uint32_t            Fps
);


/*****************************************************************************/
/**
 *          IsiGetAutoFpsInfoIss
 *
 * @brief   Get sensor auto fps information.
 *
 * @param   handle                  sensor instance handle
 * @param   pAutoFpsInfo             auto fps information
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetAutoFpsInfoIss
(
    IsiSensorHandle_t   handle,
    IsiAutoFps_t        *pAutoFpsInfo
);


/*****************************************************************************/
/**
 *          IsiGetStartEvIss
 *
 * @brief   Get sensor start exposure value.
 *
 * @param   handle                  sensor instance handle
 * @param   pStartEv                start exposure value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetStartEvIss
(
    IsiSensorHandle_t   handle,
    uint64_t            *pStartEv
);

/*****************************************************************************/
/**
 *          IsiGetAgainDgainIss
 *
 * @brief   Get Sensor Fps Config.
 *
 * @param   handle                  sensor instance handle
 * @param   pGain                   sensor gain pointer to get
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetAgainDgainIss
(
    IsiSensorHandle_t   handle,
    IsiUserGain_t       *pGain
);


/*****************************************************************************/
/**
 *          IsiSetAgainDgainIss
 *
 * @brief   Get Sensor Fps Config.
 *
 * @param   handle                  sensor instance handle
 * @param   Gain                    sensor gain pointer user to set
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetAgainDgainIss
(
    IsiSensorHandle_t   handle,
    IsiUserGain_t       Gain
);


/*****************************************************************************/
/**
 *          IsiGetIspStatusIss
 *
 * @brief   Get sensor isp status.
 *
 * @param   handle                  sensor instance handle
 * @param   pSensorIspStatus        sensor isp status
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetIspStatusIss
(
    IsiSensorHandle_t    handle,
    IsiIspStatus_t       *pIspStatus
);


/*****************************************************************************/
/**
 *          IsiSetBlcIss
 *
 * @brief   set sensor linear mode black level
 *
 *
 * @param   handle          sensor instance handle
 * @param   pBlc            blc params pointer
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT IsiSetBlcIss
(
	IsiSensorHandle_t handle,
	IsiBlc_t          *pBlc
);


/*****************************************************************************/
/**
 *          IsiSetWBIss
 *
 * @brief   set sensor linear mode white balance
 *          or hdr mode normal exp frame white balance
 *
 * @param   handle          sensor instance handle
 * @param   pWb             wb params pointer
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT IsiSetWBIss
(
	IsiSensorHandle_t handle,
	IsiWB_t           *pWb
);


/*****************************************************************************/
/**
 *          IsiSetPowerIss
 *
 * @brief   Performs the power-up/power-down sequence of the camera, if possible.
 *
 * @param   handle      Sensor instance handle
 * @param   on          new power state (BOOL_TRUE=on, BOOL_FALSE=off)
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetPowerIss
(
    IsiSensorHandle_t   handle,
    bool_t              on
);


/*****************************************************************************/
/**
 *          IsiSetTpgIss
 *
 * @brief   set sensor test pattern.
 *
 * @param   handle      Sensor instance handle
 * @param   Tpg         Sensor test pattern
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiSetTpgIss
(
    IsiSensorHandle_t   handle,
    IsiTpg_t            Tpg
);

/*****************************************************************************/
/**
 *          IsiGetTpgIss
 *
 * @brief   set sensor test pattern.
 *
 * @param   handle      Sensor instance handle
 * @param   *Tpg         Sensor test pattern ptr
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/

RESULT IsiGetTpgIss
(
    IsiSensorHandle_t   handle,
    IsiTpg_t            *Tpg
);

/*****************************************************************************/
/**
 *          IsiGetExpandCurveIss
 *
 * @brief   get sensor expand curve
 *
 * @param   handle          sensor instance handle
 * @param   pCurve          expand curve pointer
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT IsiGetExpandCurveIss
(
	IsiSensorHandle_t       handle,
	IsiCompandCurve_t       *pCurve
);


/*****************************************************************************/
/**
 *          IsiGetCompressCurveIss
 *
 * @brief   get sensor compress curve
 *
 * @param   handle          sensor instance handle
 * @param   pCurve          compress curve pointer
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT IsiGetCompressCurveIss
(
	IsiSensorHandle_t       handle,
	IsiCompandCurve_t       *pCurve
);


/*****************************************************************************/
/**
 *          IsiExtendFuncIss
 *
 * @brief   sensor extend function.
 *
 * @param   handle                  sensor instance handle
 * @param   pUserData               sensor extend info
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiExtendFuncIss
(
    IsiSensorHandle_t   handle,
    void                *pUserData
);


/*****************************************************************************/
/**
 *          IsiGetOtpDataIss
 *
 * @brief   get sensor otp data.
 *
 * @param   handle                  sensor instance handle
 * @param   ctx                     sensor otp data
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiGetOtpDataIss
(
    IsiSensorHandle_t   handle,
    uint32_t 		*length,
    void                *pOtpData
);


/*****************************************************************************/
/**
 *          IsiFocusCreateIss
 *
 * @brief   create sensor focus
 *
 * @param   handle          sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiFocusCreateIss
(
    IsiSensorHandle_t   handle
);


/*****************************************************************************/
/**
 *          IsiFocusReleaseIss
 *
 * @brief   release sensor focus.
 *
 * @param   handle          sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiFocusReleaseIss
(
    IsiSensorHandle_t   handle
);


/*****************************************************************************/
/**
 *          IsiFocusGetCalibrateIss
 *
 * @brief   Triggers a forced calibration of the focus hardware.
 *
 * @param   handle          sensor instance handle
 * @param   pFocusCalib     sensor focus calib pointor
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiFocusGetCalibrateIss
(
    IsiSensorHandle_t   handle,
    IsiFocusCalibAttr_t *pFocusCalib
);


/*****************************************************************************/
/**
 *          IsiFocusSetIss
 *
 * @brief   Drives the lens system to a certain focus point.
 *
 * @param   handle          sensor instance handle
 * @param   pPos            focus position pointer
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiFocusSetIss
(
    IsiSensorHandle_t   handle,
    IsiFocusPos_t       *pPos
);


/*****************************************************************************/
/**
 *          IsiFocusGetIss
 *
 * @brief   Retrieves the currently applied focus point.
 *
 * @param   handle          sensor instance handle
 * @param   pPos            pointer to a variable to receive the current
 *                          focus point
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiFocusGetIss
(
    IsiSensorHandle_t   handle,
    IsiFocusPos_t       *pPos
);


/*****************************************************************************/
/**
 *          IsiDumpAllRegisters
 *
 * @brief   Activates or deactivates sensor's test-pattern (normally a defined
 *          colorbar )
 *
 * @param   handle          sensor instance handle
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_WRONG_HANDLE
 * @retval  RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT IsiDumpAllRegisters
(
    IsiSensorHandle_t   handle,
    const uint8_t       *filename
);



struct IsiCamDrvConfig_s *IsiCamDriverName2DrvConfig(const char* drvName);

#ifdef __cplusplus
}
#endif


/* @} isi */


#endif /* __ISI_H__ */
