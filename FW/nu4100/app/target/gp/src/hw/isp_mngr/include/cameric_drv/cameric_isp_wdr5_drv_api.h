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

#ifndef __CAMERIC_ISP_WDR5_DRV_API_H__
#define __CAMERIC_ISP_WDR5_DRV_API_H__

/**
 * @file    cameric_isp_wdr5_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP WDR driver API definitions.
 *
 *****************************************************************************/
/**
 * @cond cameric_isp_wdr5
 *
 * @defgroup cameric_isp_wdr5_drv_api CamerIC ISP WDR5 Driver API definitions
 * @{
 *
 */
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct CamerIcIspWdr5Config_s {
    bool                  enable;
    uint8_t               strength;			/**< strength range [0,128]*/
    uint8_t               highStrength;		/**< highStrength range [0,128] */
    uint16_t               lowStrength;		/**< lowStrength range [0,256]*/
    int16_t               contrast;         /**< contrast range [-1023,1023]*/
    bool                  flatMode;         /**< flatMode [0, 1]*/
    uint8_t               flatLevel;        /**< flatLevel [0, 15]*/
    uint16_t              entropySlope;     /**< entropySlope [0, 1023]*/
    uint16_t              entropyBase;      /**< entropyBase [0, 1023]*/
    bool                  globalCureveMode; /**< globalCureveMode [0, 1]*/ 

    uint8_t               flatLevelGlobal;  /**< flatLevelGlobal range [0,15]*/
    uint16_t              fixedWeight;      /**< fixedWeight [0, 1023]*/
    uint16_t               maxGain;          /**< maxGain [1, 256]*/
    uint16_t               minGain;          /**< minGain [1, 256]*/
    float                 degamma;          /**< degamma [1, 4]*/
    float                 diffHigh;         /**< diffHigh [1, 100]*/ 
    float                 diffLow;          /**< diffLow [1, 100]*/ 

    float                 logWeight;        /**< logWeight [0, 10]*/
    float               satRange;           /**< satRange [0, 1]*/
    uint16_t               satThrGainUp;    /**< satThrGainUp [0, 256]*/ 
    uint16_t               satThrGainDown;  /**< satThrGainDown [0, 256]*/ 
    // bool                 entropyEnable;    /**< reserved*/ 

} CamerIcIspWdr5Config_t;

/*****************************************************************************/
/**
 *          CamerIcIspWdr5Enable()
 *
 * @brief   Enable WDR5 Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspWdr5Enable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 *          CamerIcIspWdr5Disable()
 *
 * @brief   Disable WDR5 Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspWdr5Disable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5Config()
*
* @brief   Config WDR5 Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5Config
(
	CamerIcDrvHandle_t 		handle,
    CamerIcIspWdr5Config_t  *pWdr5Cfg
);

RESULT CamerIcIspWdr5GetConfig
(
    CamerIcDrvHandle_t 		handle,
    CamerIcIspWdr5Config_t  *pWdr5Cfg
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5SetFlat()
*
* @brief   Config WDR5 Module flat mode and level
*
* @param   handle          CamerIc driver handle
* @param   flatMode        flat mode 
* @param   flatLevel       flat levle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5SetFlat
(
	CamerIcDrvHandle_t handle,
    uint8_t flatMode,
    uint8_t flatLevel
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5StoreHdrRatio()
*
* @brief   Store HDR ration
*
* @param   handle          CamerIc driver handle
* @param   ratioLSVS,      hdr long exposure and very short exposure
* @param   ratioLS         hdr long exposure and short exposure
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5StoreHdrRatio
(
	CamerIcDrvHandle_t  handle,
	uint8_t  ratioLSVS,
    uint8_t   ratioLS
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5IsEnabled()
*
* @brief   get WDR5 Module whether if enabled
*
* @param   handle          CamerIc driver handle
* @param   pEnabled,       enabled pointer
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5IsEnabled
(
	CamerIcDrvHandle_t handle,
    bool *pEnabled
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5GetLowStrength()
*
* @brief   get WDR5 Module low strength
*
* @param   handle          CamerIc driver handle
* @param   plowStrength    low strength pointer
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5GetLowStrength
(
	CamerIcDrvHandle_t handle,
    uint32_t *plowStrength
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5SetSatGainUp()
*
* @brief   set WDR5 Module upper threshold of wdr gain
*
* @param   handle          CamerIc driver handle
* @param   gainUp          upper threshold of wdr gain
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5SetSatGainUp
(
	CamerIcDrvHandle_t handle,
    uint16_t gainUp
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5SetSatGainDown()
*
* @brief   set WDR5 Module lower threshold of wdr gain
*
* @param   handle          CamerIc driver handle
* @param   gammadown       lower threshold of wdr gain
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5SetSatGainDown
(
	CamerIcDrvHandle_t handle,
    uint32_t *gammadown
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5SetEntropySlope()
*
* @brief   set WDR5 Module entropy slope and base
*
* @param   handle          CamerIc driver handle
* @param   slope          entropy slope 
* @param   base           entorpy base 
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5SetEntropySlope
(
	CamerIcDrvHandle_t handle,
    uint16_t slope,
    uint16_t base
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5GetEntropySlope()
*
* @brief   get WDR5 Module entropy slope and base
*
* @param   handle          CamerIc driver handle
* @param   pSlope          entropy slope pointer
* @param   pBase           entorpy base pointer
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5GetEntropySlope
(
	CamerIcDrvHandle_t handle,
    uint16_t *pSlope,
    uint16_t *pBase
);


/*****************************************************************************/
/**
*          CamerIcIspWdr5Init()
*
* @brief   init  WDR5 Module
*
* @param   handle          CamerIc driver handle
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5Init
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
*          CamerIcIspWdr5Release()
*
* @brief   release  WDR5 Module
*
* @param   handle          CamerIc driver handle
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspWdr5Release
(
    CamerIcDrvHandle_t handle
);


#ifdef __cplusplus
}
#endif



#endif /* __CAMERIC_ISP_WDR5_DRV_API_H__ */


