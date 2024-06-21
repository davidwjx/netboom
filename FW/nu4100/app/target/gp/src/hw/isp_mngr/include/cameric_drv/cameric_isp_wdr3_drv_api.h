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

#ifndef __CAMERIC_ISP_WDR3_DRV_API_H__
#define __CAMERIC_ISP_WDR3_DRV_API_H__

/**
 * @file    cameric_isp_wdr3_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP WDR driver API definitions.
 *
 *****************************************************************************/
/**
 * @cond cameric_isp_wdr3
 *
 * @defgroup cameric_isp_wdr3_drv_api CamerIC ISP WDR3 Driver API definitions
 * @{
 *
 */
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct CamerIcIspWdr3Config_s {
    uint8_t               strength;			/**< strength */
    uint8_t               globalStrength;	/**< globalStrength */
    uint8_t               maxGain;			/**< maxGain */
} CamerIcIspWdr3Config_t;


/*****************************************************************************/
/**
 *          CamerIcIspWdr3Enable()
 *
 * @brief   Enable WDR3 Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspWdr3Enable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 *          CamerIcIspWdr3Disable()
 *
 * @brief   Disable WDR3 Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspWdr3Disable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
*          CamerIcIspWdr3Config()
*
* @brief   Config WDR3 Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3Config
(
	CamerIcDrvHandle_t 		handle,
    CamerIcIspWdr3Config_t  *pWdr3Cfg
);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetLowlightStrength()
*
* @brief   set WDR3 Module  low light strength properities
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetLowlightStrength
(
	CamerIcDrvHandle_t 		handle,
    float strength
);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetGammadownCurve()
*
* @brief   set WDR3 Module  gamma down curve
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetGammadownCurve
(
	CamerIcDrvHandle_t 		handle,
    unsigned short* pGammadownCurve
);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetLocalStrength()
*
* @brief   set WDR3 Module  local strength
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetLocalStrength
(
	CamerIcDrvHandle_t 		handle,
    int strength
);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetPixelSlope()
*
* @brief   set WDR3 Module  pixel slope
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetPixelSlope
(
	CamerIcDrvHandle_t handle,
    unsigned char merge_slope,
    unsigned char merge_base,
    unsigned char adjust_slope,
    unsigned char adjust_base
);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetEntropySlope()
*
* @brief   set WDR3 Module  entropy slope
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetEntropySlope
(
	CamerIcDrvHandle_t 		handle,
    unsigned short slope,
    unsigned short base
);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetHistogram()
*
* @brief   Get WDR3 Module  histogram bins
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetHistogram(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetHistogram()
*
* @brief   Set WDR3 Module  histogram bins
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetHistogram(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetGammaup()
*
* @brief   Get WDR3 Module gamma up curve
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetGammaup(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetGammaup()
*
* @brief   Set WDR3 Module gamma up curve
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetGammaup(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetEntropy()
*
* @brief   Get WDR3 Module entropy
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetEntropy(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetEntropy()
*
* @brief   Set WDR3 Module entropy
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetEntropy(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetDistanceWeight()
*
* @brief   Get WDR3 Module distance weight
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetDistanceWeight(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetDistanceWeight()
*
* @brief   Set WDR3 Module distance weight
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetDistanceWeight(CamerIcDrvHandle_t handle, unsigned int* bin);


/*****************************************************************************/
/**
*          CamerIcIspWdr3GetDiffWeight()
*
* @brief   get WDR3 Module difference weight
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetDiffWeight(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetDiffWeight()
*
* @brief   Set WDR3 Module difference weight
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetDiffWeight(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetInvertLinear()
*
* @brief   Get WDR3 Module invert linear 
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetInvertLinear(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetInvertLinear()
*
* @brief   Set WDR3 Module invert linear 
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetInvertLinear(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetInvertCurve()
*
* @brief   get WDR3 Module invert curve
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetInvertCurve(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetInvertCurve()
*
* @brief   Set WDR3 Module invert curve
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetInvertCurve(CamerIcDrvHandle_t handle, unsigned int* bin);

/*****************************************************************************/
/**
*          CamerIcIspWdr3GetSigma()
*
* @brief   get WDR3 Module sigma
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3GetSigma(CamerIcDrvHandle_t handle, int* pSigmaWidth, int* pSigmaHeight, int* pSigmaValue);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetSigma()
*
* @brief   Set WDR3 Module sigma
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr3SetSigma(CamerIcDrvHandle_t handle, int w, int h, int val);

/*****************************************************************************/
/**
*          CamerIcIspWdr3SetSigma()
*
* @brief   get WDR3 Module status
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
bool CamerIcIspWdr3IsEnable(CamerIcDrvHandle_t handle);

#ifdef __cplusplus
}
#endif



#endif /* __CAMERIC_ISP_WDR3_DRV_API_H__ */

