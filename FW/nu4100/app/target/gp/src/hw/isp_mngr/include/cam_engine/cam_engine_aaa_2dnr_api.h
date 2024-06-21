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
 * @cond    cam_engine_aaa_2dnr
 *
 * @file    cam_engine_aaa_2dnr_api.h
 *
 * @brief
 *
 *   Interface description of the CamEngine Auto Algorithms.
 *
 *****************************************************************************/
/**
 *
 * @defgroup cam_engine_aaa_2dnr_api CamEngine 2DNR Algorithms API
 * @{
 *
 */

#ifndef __CAM_ENGINE_AAA_2DNR_API_H__
#define __CAM_ENGINE_AAA_2DNR_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>
#include <cam_device/cam_device_ispcore_defs.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************/
/**
 * @brief   Auto-2Dimension-Noise-Reduction mode.
 *
 *****************************************************************************/
typedef enum CamEngineA2dnrMode_e
{
    CAM_ENGINE_A2DNR_MODE_INVALID = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_A2DNR_MODE_MANUAL  = 1,    /**< manual mode */
    CAM_ENGINE_A2DNR_MODE_AUTO    = 2,    /**< run auto mode */
    CAM_ENGINE_A2DNR_MODE_MAX
} CamEngineA2dnrMode_t;

/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-2DNR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrStart
(
    CamEngineHandle_t 			hCamEngine,
    const CamEngineA2dnrMode_t  mode           /**< run-mode */
);

/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-2DNR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrStop
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-2DNR-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrConfigure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const float                 sigma,        	 /**< sigma */
    const uint8_t               strength,  		 /**< strength */
    const uint8_t               pregmaStrength   /**< pregmaStrength */
);

/*****************************************************************************/
/**
 * @brief   This functions set the level value base on Adaptive-2DNR-Control
 *	        mode.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrSetAutoLevel
(
    CamEngineHandle_t           hCamEngine,	/**< handle CamEngine */
    const uint8_t               level		/**< level */
);

/*****************************************************************************/
/**
 * @brief   This functions set the Adaptive-2DNR-Control auto mode table.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrSetAutoTable
(
    CamEngineHandle_t           		hCamEngine,      	/**< handle CamEngine */
    const CamEngineA2dnrParamNode_t 	*pParamNodeTable,	/**< CamEngineA2dnrParamNode_t */
    const uint8_t               		nodeNum   			/**< nodeNum */
);

/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-2DNR-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA2dnrStatus
(
    CamEngineHandle_t   	hCamEngine,
    bool_t              	*pRunning,
    CamEngineA2dnrMode_t  	*pMode,				/**< run-mode */
    float					*pGain,				/**< gain */
	float					*pIntegrationTime,	/**< integrationTime */
    float               	*pSigma,			/**< sigma */
    uint8_t             	*pStrength,			/**< strength */
    uint8_t             	*pPregmaStrength	/**< pregmaStrength */
);

#ifdef __cplusplus
}
#endif


/* @} cam_engine_aaa_api */


#endif /* __CAM_ENGINE_AAA_2DNR_API_H__ */

