/******************************************************************************\
|* Copyright 2010, Dream Chip Technologies GmbH. used with permission by      *|
|* VeriSilicon.                                                               *|
|* Copyright (c) <2020> by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")     *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

/* VeriSilicon 2020 */

#ifndef __CAMERIC_ISP_3DNR32_DRV_API_H__
#define __CAMERIC_ISP_3DNR32_DRV_API_H__

/**
 * @file    cameric_isp_wdr4_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP WDR driver API definitions.
 *
 *****************************************************************************/
/**
 * @cond cameric_isp_wdr4
 *
 * @defgroup cameric_isp_wdr4_drv_api CamerIC ISP WDR4 Driver API definitions
 * @{
 *
 */
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define NOISE_MODULE_NUM 3
typedef struct CamerIcIsp3Dnr32Config_s {
    bool enable;
	bool tnrEn;
	bool nlmEn;
	bool motionDilateEn;
	bool motionErodeEn;
	bool pregammaEn;
	uint32_t filterLen;      //[0,1024] 20
    uint32_t filterLen2;     //[0,1024] 10
    uint16_t thrMotionSlope; //[1,1023] 32
    uint16_t noiseLevel;     //[0,1024] 60
    uint8_t sadWeight;       //[0,16] 8
    uint32_t noiselevelA;    //[0,10000] 4
    uint32_t noiselevelB;    //[0,10000] 16
    uint16_t strength;       //[0,128] 120
    uint8_t rangeH;          //[3,7] 7
    uint8_t rangeV;          //[3,7] 7
    uint8_t dilateH;         //[3,7] 3
    uint8_t preweight;       //[0,32] 1
    float noisemodelTransStart;    //0
    uint16_t fixCurveStart;       //0
    uint8_t lumaCompEn;          
    uint8_t lumaPixelSlop;       //[0,255] 0
    uint8_t lumaPixelSlopShift;  //[0,15] 1
    uint8_t lumaPixelSlopMin;  //[0,128] 128
    uint8_t lumaPixelValShift;   //[0,255] 0

    uint8_t noiseAutoEst;
    float  noiseBoundFactor;
    float   nlUpdateFactor;
    float  noiseThreshFactor; //[1, 8] 2.0

    double noiseModuleA[NOISE_MODULE_NUM];
    double noiseModuleB[NOISE_MODULE_NUM];
} CamerIcIsp3Dnr32Config_t;
RESULT CamerIcIsp3Dnr32Init
(
	CamerIcDrvHandle_t handle
);

RESULT CamerIcIsp3Dnr32Release
(
	CamerIcDrvHandle_t handle
);

RESULT CamerIcIsp3Dnr32Enable
(
    CamerIcDrvHandle_t handle
);

RESULT CamerIcIsp3Dnr32Disable
(
    CamerIcDrvHandle_t handle
);

/******************************************************************************/
/**
 *          CamerIcIsp3Dnr32NoiseUpdate
 *
 * @brief   update the relative registers that need do change in every frames.
 *         
 *
 * @note
 *
 *****************************************************************************/
RESULT CamerIcIsp3Dnr32NoiseUpdate
(
	CamerIcDrvHandle_t  handle
);


RESULT CamerIcIsp3Dnr32GetConfig(
	CamerIcDrvHandle_t  handle,
	CamerIcIsp3Dnr32Config_t *pConfig
);

RESULT CamerIcIsp3Dnr32SetConfig(
	CamerIcDrvHandle_t  handle,
	const CamerIcIsp3Dnr32Config_t *pConfig
);


RESULT CamerIcIsp3Dnr32UpdateCurve(
	CamerIcDrvHandle_t  handle,
const	double *noiseModelA,
const	double *noiseModelB,
	float transStart,
    uint16_t fixCurveStart	
);

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_denoise3dnV32_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_3DNR32_DRV_API_H__ */

