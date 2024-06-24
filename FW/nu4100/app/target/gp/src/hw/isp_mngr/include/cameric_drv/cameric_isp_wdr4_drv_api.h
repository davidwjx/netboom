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

#ifndef __CAMERIC_ISP_WDR4_DRV_API_H__
#define __CAMERIC_ISP_WDR4_DRV_API_H__

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


typedef struct CamerIcIspWdr4Config_s {
    uint8_t               strength;			/**< strength */
    uint8_t               highStrength;		/**< highStrength */
    uint8_t               lowStrength;		/**< lowStrength */
    uint8_t               globalStrength;	/**< globalStrength */
    int                   contrast;         /**< contrast */
    uint8_t               flatStrength;     /**< flatStrength */
    uint8_t               flatThreshold;    /**< flatThreshold */
} CamerIcIspWdr4Config_t;

RESULT CamerIcIspWdr4Init
(
    CamerIcDrvHandle_t handle
);

RESULT CamerIcIspWdr4Release
(
    CamerIcDrvHandle_t handle
);
/*****************************************************************************/
/**
 *          CamerIcIspWdr4Enable()
 *
 * @brief   Enable WDR4 Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspWdr4Enable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 *          CamerIcIspWdr4Disable()
 *
 * @brief   Disable WDR4 Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspWdr4Disable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
*          CamerIcIspWdr4Config()
*
* @brief   Config WDR4 Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4Config
(
	CamerIcDrvHandle_t 		handle,
    CamerIcIspWdr4Config_t  *pWdr4Cfg
);

RESULT CamerIcIspWdr4SetFlat
(
	CamerIcDrvHandle_t handle,
    uint8_t strength,
    uint8_t threshold
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4StoreHdrRatio()
*
* @brief   store hdr  ration information
*
* @param   handle          CamerIc driver handle
* @param   ratioLSVS,      hdr long exposure and very short exposure
* @param   ratioLS         hdr long exposure and short exposure
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4StoreHdrRatio
(
	CamerIcDrvHandle_t  handle,
	uint8_t  ratioLSVS,
    uint8_t   ratioLS
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4GetEnable()
*
* @brief   get wdr4  enable status
* @param   handle          CamerIc driver handle
* @param   pEnabled        pointer to the wdr4 enable
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4GetEnable
(
	CamerIcDrvHandle_t handle,
    bool *pEnabled
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4GetLowStrength()
*
* @brief   get wdr4  low strength
* @param   handle          CamerIc driver handle
* @param   low_strength        pointer to the wdr4 low strength
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4GetLowStrength
(
	CamerIcDrvHandle_t handle,
    uint32_t *low_strength
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4SetEntropy()
*
* @brief   set wdr4  entropy
* @param   handle          CamerIc driver handle
* @param   entropy        pointer to entropy
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4SetEntropy
(
	CamerIcDrvHandle_t handle,
    uint32_t *entropy
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4SetGammaPre()
*
* @brief   set wdr4  gamma pre
* @param   handle          CamerIc driver handle
* @param   gammapre        pointer to gammapre
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4SetGammaPre
(
	CamerIcDrvHandle_t handle,
    uint32_t *gammapre
);


/*****************************************************************************/
/**
*          CamerIcIspWdr4SetGammaUp()
*
* @brief   set wdr4  gamma up
* @param   handle          CamerIc driver handle
* @param   entropy        pointer to gammaup
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4SetGammaUp
(
	CamerIcDrvHandle_t handle,
    uint32_t *gammaup
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4SetGammaDown()
*
* @brief   set wdr4  gamma down
* @param   handle          CamerIc driver handle
* @param   entropy        pointer to gammadown
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4SetGammaDown
(
	CamerIcDrvHandle_t handle,
    uint32_t *gammadown
);

/*****************************************************************************/
/**
*          CamerIcIspWdr4SetEntropySlope()
*
* @brief   set wdr4  entropy slope
* @param   handle          CamerIc driver handle
* @param   entropy        pointer to gammadown
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr4SetEntropySlope
(
	CamerIcDrvHandle_t handle,
    uint32_t slope,
    uint32_t base
);

#ifdef __cplusplus
}
#endif



#endif /* __CAMERIC_ISP_WDR4_DRV_API_H__ */


