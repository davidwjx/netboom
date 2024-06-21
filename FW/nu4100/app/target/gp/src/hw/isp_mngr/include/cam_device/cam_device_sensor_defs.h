/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

#ifndef __CAMERA_DEVICE_SENSOR_DEFS_H__
#define __CAMERA_DEVICE_SENSOR_DEFS_H__

#include "types.h"

#define DEVICE_SENSOR_MODE_MAX 28
typedef struct Resolution_s
{
    uint16_t width;
    uint16_t height;
}Resolution_t;

typedef struct DevSensorSize_s {
	uint32_t BoundsWidth;
	uint32_t BoundsHeight;
	uint32_t top;
	uint32_t left;
	uint32_t width;
	uint32_t height;
}DevSensorSize_t;

typedef struct DevSensorCompress_s {
	uint32_t enable;
	uint32_t xBit;
	uint32_t yBit;
}DevSensorCompress_t;

typedef struct DevSensorMipi_s {
	uint32_t mipiLane;
}DevSensorMipi_t;


/*****************************************************************************/
/**
 * @brief   This structure defines the sensor capabilities.
 */
/*****************************************************************************/
typedef struct DevSensorCaps_s {
    uint32_t BusWidth;                  /**< supported bus-width */
    uint32_t Mode;                      /**< supported operating modes */
    uint32_t FieldSelection;            /**< sample fields */
    uint32_t YCSequence;
    uint32_t Conv422;
    uint32_t BPat;                      /**< bayer pattern */
    uint32_t HPol;                      /**< horizontal polarity */
    uint32_t VPol;                      /**< vertical polarity */
    uint32_t Edge;                      /**< sample edge */
    Resolution_t Resolution;         /**< supported resolutions */
    uint32_t SmiaMode;
    uint32_t MipiMode;
    uint32_t MipiLanes;
    uint32_t enableHdr;
}DevSensorCaps_t;


typedef struct DevSensorMode_s {
	uint32_t index;
	DevSensorSize_t size;
	uint32_t HdrMode;
	uint32_t StitchingMode;
	uint32_t BitWidth;
	DevSensorCompress_t compress;
	uint32_t BayerPattern;
	DevSensorMipi_t MipiInfo;
	uint32_t OtpFlag;
}DevSensorMode_t;


#endif  // __CAMERA_DEVICE_SENSOR_DEFS_H__