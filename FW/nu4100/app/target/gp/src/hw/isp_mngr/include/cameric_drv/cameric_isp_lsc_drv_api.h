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

#ifndef __CAMERIC_ISP_LSC_DRV_API_H__
#define __CAMERIC_ISP_LSC_DRV_API_H__

/**
 * @cond    cameric_isp_lsc
 *
 * @file 	cameric_isp_lsc_drv_api.h
 *
 * @brief	This file contains the CamerIc ISP LSC driver API definitions
 *
 *****************************************************************************/
/**
 * @defgroup cameric_isp_lsc_drv_api CamerIc ISP LSC driver API definitions
 * @{
 *
 * A significant challenge for low profile lenses is to minimize lens shading
 * artifacts, also known as "vignetting". Lens shading artifacts are caused by
 * decreased light intensity in the peripheral regions of the sensor due to
 * the inability of the lens to focus the light in those areas in comparison
 * to the centre region. Lens shading artifacts are minimized by image data
 * processing with a lens shading correction algorithm.
 *
 * @image html lsc.png "Overview of the CamerIC ISP LSC driver" width=\textwidth
 * @image latex lsc.png "Overview of the CamerIC ISP LSC driver" width=\textwidth
 *
 * Lens shading is different for each of its color components, since sensor
 * produces no perfect response in all three color channels. Therefore the
 * lens shading correction is alternatively performed identically for the
 * four color components of the RGB Bayer input signal or calibrated
 * independently for the color components RGB.
 *
 * The aim of the lens shading correction algorithm is to achieve a constant
 * sensitivity across the entire frame after correction. Therefore each incoming
 * pixel value PIN(x,y) is multiplied by a correction factor F(x,y). The
 * correction factor depends on the coordinates of the pixel within the frame.
 *
 * The corrected pixel value PCOR(x,y) is calculated according:
 *
 * PCOR(x,y) = PIN(x,y) * F(x,y)                     (1)
 *
 * In order to increase the precision of the correction function, the frame
 * is divided into 16 sectors in x and y dimension. The coordinates of each
 * sector are programmable. Furthermore the lens shading correction parameters
 * are programmable independently for each sector and for each color component
 * within a sector. The sector coordinates apply for each color component.
 * This unit will be designed so that either the lens shading correction parameters
 * apply for all color components or each color component gets its own lens shading
 * correction parameters.
 *
 * @image html lsc_sectors.png "Lens Shading Correction Sectors" width=\textwidth
 * @image latex lsc_sectors.png "Lens Shading Correction Sectors" width=\textwidth
 *
 * To reduce hardware effort, position and size of mirrored sectors related to the
 * frame center are equivalent. This means for example that size and position of
 * sector X1 is equivalent to X16 related to frame center and Y3 to Y14. However
 * the correction factors are independent for each sector. To reduce the memory
 * for storing the coordinates of the sensor area, the size of each sector is
 * programmed. Furthermore only the sizes of one quarter of the frame is programmed,
 * because the sectors are symmetrical relative to the picture center. The coordinates
 * are calculated by hardware during processing.
 *
 * Within each sector, the correction function F(x,y) can be expressed as a Bilinear
 * Interpolation Function. These functions in different areas are correlated, because
 * of the requirement that the correction function must be continuous and smooth.
 *
 * The lens shading correction unit works on RGB Bayer pattern and is placed after
 * the sensor degamma and the black level correction units.
 *
 * @subsection Functional description
 *
 * The lens shading correction is based on one or more reference frames which have
 * to be captured under dedicated light conditions and a dedicated position of the
 * sensor. The pixels of the captured frame are then evaluated by software and the
 * calculated parameters for the lens shading correction are stored in different
 * tables. It is also possible to use different lens shading correction parameters
 * for different environment conditions, e.g. lightness, light direction or sensor
 * position. Therefore additional reference frames for the different conditions are
 * to be captured and evaluated. The calculated parameters including sector settings
 * can be stored in multiple tables at external memory or Flash device. Thus user
 * can select one of multiple lens shading correction algorithms so that software
 * can update the current required tables within the lens shading correction block.
 * The correction factors at the sector corners as well as the size of the sectors
 * are stored. The correction value for each pixel position is calculated using the
 * bilinear interpolation.
 *
 */

#include <ebase/types.h>
#include <common/return_codes.h>
#include <cam_device/cam_device_ispcore_defs.h>

#ifdef __cplusplus
extern "C"
{
#endif



/*****************************************************************************/
/**
 * @brief   This macro defines the size of lens shading sectors in x or y
 * 			direction
 *
 *****************************************************************************/
#define CAMERIC_MAX_LSC_SECTORS     16



/*****************************************************************************/
/**
 * @brief   Lens shade correction configuration structure
 *
 *****************************************************************************/
typedef struct CamerIcIspLscConfig_s
{
    uint16_t LscRDataTbl[CAMERIC_DATA_TBL_SIZE];    /**< correction values of R color part */
    uint16_t LscGRDataTbl[CAMERIC_DATA_TBL_SIZE];   /**< correction values of G (red lines) color part */
    uint16_t LscGBDataTbl[CAMERIC_DATA_TBL_SIZE];   /**< correction values of G (blue lines) color part  */
    uint16_t LscBDataTbl[CAMERIC_DATA_TBL_SIZE];    /**< correction values of B color part  */
    uint16_t LscXGradTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< multiplication factors of x direction  */
    uint16_t LscYGradTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< multiplication factors of y direction  */
    uint16_t LscXSizeTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< sector sizes of x direction            */
    uint16_t LscYSizeTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< sector sizes of y direction            */
} CamerIcIspLscConfig_t;



/*****************************************************************************/
/**
 * @brief   Lens shade sector configuration structure
 *
 *****************************************************************************/
typedef struct CamerIcIspLscSectorConfig_s
{
    uint16_t LscXGradTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< multiplication factors of x direction  */
    uint16_t LscYGradTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< multiplication factors of y direction  */
    uint16_t LscXSizeTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< sector sizes of x direction            */
    uint16_t LscYSizeTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< sector sizes of y direction            */
} CamerIcIspLscSectorConfig_t;



/*****************************************************************************/
/**
 * @brief   This function enables the ISP LSC Module.
 *
 * @param   handle          	CamerIc driver handle
 *
 * @return                  	Return the result of the function call.
 * @retval	RET_SUCCESS			operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscEnable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This function disables the ISP LSC module (bypass the lens shade
 * 			correction).
 *
 * @param   handle          	CamerIc driver handle
 *
 * @return                  	Return the result of the function call.
 * @retval	RET_SUCCESS			operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscDisable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This function returns the status of the ISP LSC module.
 *
 * @param   handle              CamerIc driver handle
 * @param   pIsEnabled          Pointer to value to store current state
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_NULL_POINTER    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscIsEnabled
(
    CamerIcDrvHandle_t      handle,
    bool_t                  *pIsEnabled
);



/*****************************************************************************/
/**
 * @brief   This function reads out the current configred lens shade
 *          correction configuration.
 *
 * @note    This function also reads out the current segmentation and
 *          gradient tables.
 *
 * @param   handle              CamerIc driver handle
 * @param   pLscConfig          Pointer to store the current configuration
 *                              (@ref  CamerIcIspLscConfig_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_NULL_POINTER    pLscConfig is a NULL pointer
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscGetLenseShadeCorrection
(
    CamerIcDrvHandle_t              handle,
    CamerIcIspLscConfig_t           *pLscConfig
);



/*****************************************************************************/
/**
 * @brief   This function sets the lens shade correction configuration.
 *
 * @note    This function also sets the current segmentation and gradient
 *          tables.
 *
 * @param   handle              CamerIc driver handle.
 * @param   pLscConfig          Pointer to lens shade correction configuration
 *                              (@ref  CamerIcIspLscConfig_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_NULL_POINTER    pLscConfig is a NULL pointer
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscSetLenseShadeCorrection
(
    CamerIcDrvHandle_t              handle,
    const CamerIcIspLscConfig_t     *pLscConfig
);



/*****************************************************************************/
/**
 * @brief   This function reads out the current configred lens shade
 *          correction configuration.
 *
 * @note    This function also reads out the current segmentation and
 *          gradient tables.
 *
 * @param   handle              CamerIc driver handle
 * @param   pLscConfig          Pointer to store the current configuration
 *                              (@ref  CamerIcIspLscConfig_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_NULL_POINTER    pLscConfig is a NULL pointer
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscGetLenseShadeSectorConfig
(
    CamerIcDrvHandle_t          handle,
    CamerIcIspLscConfig_t       *pLscConfig
);



/*****************************************************************************/
/**
 * @brief   This function sets the lens shade correction configuration.
 *
 * @note    This function also sets the current segmentation and gradient
 *          tables.
 *
 * @param   handle              CamerIc driver handle.
 * @param   pLscConfig          Pointer to lens shade correction configuration
 *                              (@ref  CamerIcIspLscConfig_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_NULL_POINTER    pLscConfig is a NULL pointer
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscSetLenseShadeSectorConfig
(
    CamerIcDrvHandle_t                  handle,
    const CamerIcIspLscSectorConfig_t   *pLscConfig
);



/*****************************************************************************/
/**
 * @brief   This function sets the lens shade correction matrix for all
 *          RGB color channels.
 *
 * @note    The segmentation and gradient tables are unaffected by this
 *          function.
 *
 * @param   handle              CamerIc driver handle.
 * @param   pLscRDataTbl        Array of lens shade correction values for red
 *                              channel
 * @param   pLscGRDataTbl       Array of lens shade correction values for green
 *                              channel in red lines
 * @param   pLscGBDataTbl       Array of lens shade correction values for green
 *                              channel in blue lines
 * @param   pLscBDataTbl        Array of lens shade correction values for blue
 *                              channel
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspLscSetLenseShadeCorrectionMatrix
(
    CamerIcDrvHandle_t              handle,
    const uint16_t                  *pLscRDataTbl,      /**< correction values of R color part  */
    const uint16_t                  *pLscGRDataTbl,     /**< correction values of G color part  */
    const uint16_t                  *pLscGBDataTbl,     /**< correction values of G color part  */
    const uint16_t                  *pLscBDataTbl       /**< correction values of B color part  */
);



#ifdef __cplusplus
}
#endif

/* @} cameric_isp_lsc_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_LSC_DRV_API_H__ */

