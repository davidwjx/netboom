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

/**
 * @cond    cam_engine_aaa_2dnr5
 *
 * @file    cam_engine_aaa_2dnr5_api.h
 *
 * @brief
 *
 *   Interface description of the CamEngine Auto Algorithms.
 *
 *****************************************************************************/
/**
 *
 * @defgroup cam_engine_aaa_2dnr_api CamEngine 2DNR5 Algorithms API
 * @{
 *
 */

#ifndef __CAM_ENGINE_AAA_2DNR5_API_H__
#define __CAM_ENGINE_AAA_2DNR5_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/**
 * @brief    This functions set the Adaptive-2DNRv5-Control auto mode table.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrV5SetAutoTable
(
    CamEngineHandle_t                     hCamEngine,         /**< handle CamEngine */
    const CamEngineA2dnrV5ParamNode_t     *pParamNodeTable,    /**< CamEngineA2dnrV5ParamNode_t */
    const uint8_t                          nodeNum             /**< nodeNum */
);

typedef struct CamEngine2DnrConfig_s {
    bool     enable;
    float    sigma;       	                /**< sigma */
    uint8_t  strength;  		            /**< strength */
    uint8_t  pregmaStrength;                /**< pregmaStrength */
    uint16_t coeff[ISP_2DNR_SIGMA_BIN];     /**< denoise sigma Y */

    uint16_t strengthOffset, strengthMax;
    uint8_t  strengthSlope;
    uint16_t sigmaLumaPx[ISP_2DNR_SIGMA_LUMA_NUM];
    uint16_t sigmaLumaY[ISP_2DNR_SIGMA_LUMA_NUM];
} CamEngine2DnrConfig_t;


/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-2DNRv5-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngine2DnrConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngine2DnrConfig_t * cfgPara
);

RESULT CamEngineGet2DnrConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngine2DnrConfig_t *cfgPara
);


RESULT CamEngine2DnrCfgLumaCurve
(
    CamEngineHandle_t hCamEngine,
    CamEngine2DnrConfig_t *cfgPara
);

#ifdef __cplusplus
}
#endif


/* @} cam_engine_aaa_2dnr5_api */


#endif /* __CAM_ENGINE_AAA_2DNR5_API_H__ */

