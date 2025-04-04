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

#ifndef __CAMERIC_ISP_DPCC_DRV_API_H__
#define __CAMERIC_ISP_DPCC_DRV_API_H__

/**
 * @file    cameric_isp_dpcc_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP DPCC driver API definitions.
 *
 *****************************************************************************/
/**
 * @cond cameric_isp_dpcc
 *
 * @defgroup cameric_isp_dpcc_drv_api CamerIC ISP DPCC Driver API definitions
 * @{
 *
 * The Defect Pixel Cluster Correction (DPCC) detects and corrects single pixel
 * and small cluster defects on raw Bayer image data, i.e. impulsive noise.
 * An integrated defect pixel table allows the correction of up to 2048 fix
 * positions independently on the on the fly detection. The unit performs two tasks:
 *
 * Defect Pixel Detection \n
 * For each pixel threshold values are calculated by several methods, using
 * the correlation of neighbor pixel of the same color (red, green or blue)
 * with exception of the peak gradient estimation for red and blue that also
 * uses the green pixel values in the 5x5 neighborhood of the raw Bayer image.
 * These methods use statistical properties and linear prediction to determine
 * if a pixel needs to be marked as defect. A 3x3 sorting algorithm with rank
 * estimation including the calculation of median values of some pixel groups
 * is a central Unit. Output of the detection unit is a marker signal for the
 * following correction stage to indicate if the current pixel is defect. The
 * detection can be controlled by programmable threshold values, factors and
 * options which methods should be used. The correct setting of the thresholds
 * is important for a good separation between defects and keeping high resolution
 * and detail features in the image.
 *
 * Defect Pixel Table control	\n
 * A defect pixel table is implemented as SRAM including 2048 entries with
 * defect pixel coordinates. The table control generates a replace flag
 * independently from the on the fly detection. The defect pixel coordinates
 * can be located in a maximum window of 8192 horizontal by 4096 vertical.
 *
 * Defect Pixel Replacement	\ņ
 * Basic algorithm is a switching median filter [1], which performs sorting and
 * rank ordering. The replacement unit takes the information of the Defect Pixel
 * Detection as input. It uses a statistical sorting filter (median filter)
 * separately for each color to determine the nearest neighbor value for replacement.
 * The filter size is 4 (upper, lower, left and right neighbor for red/blue or
 * diagonal neighbors for green) optionally 5, including the center pixel.
 * The center pixel should only be included if the probability is high that
 * the center pixel is not defect (depending on parameter settings). Normally
 * the detection unit mostly marks true defect pixels. Especially the replacement
 * value for defect pixels on edges will be more accurate (getting smooth edges)
 * if the 4 Pixel kernel is selected.
 */
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif



/*****************************************************************************/
/**
 * @brief   This type defines the static configuration structure of the
 *          CamerIc DPCC module (register configuration).
 */
/*****************************************************************************/
typedef struct CamerIcDpccStaticConfig_s
{
    uint32_t isp_dpcc_mode;
    uint32_t isp_dpcc_output_mode;
    uint32_t isp_dpcc_set_use;

    uint32_t isp_dpcc_methods_set_1;
    uint32_t isp_dpcc_methods_set_2;
    uint32_t isp_dpcc_methods_set_3;

    uint32_t isp_dpcc_line_thresh_1;
    uint32_t isp_dpcc_line_mad_fac_1;
    uint32_t isp_dpcc_pg_fac_1;
    uint32_t isp_dpcc_rnd_thresh_1;
    uint32_t isp_dpcc_rg_fac_1;

    uint32_t isp_dpcc_line_thresh_2;
    uint32_t isp_dpcc_line_mad_fac_2;
    uint32_t isp_dpcc_pg_fac_2;
    uint32_t isp_dpcc_rnd_thresh_2;
    uint32_t isp_dpcc_rg_fac_2;

    uint32_t isp_dpcc_line_thresh_3;
    uint32_t isp_dpcc_line_mad_fac_3;
    uint32_t isp_dpcc_pg_fac_3;
    uint32_t isp_dpcc_rnd_thresh_3;
    uint32_t isp_dpcc_rg_fac_3;

    uint32_t isp_dpcc_ro_limits;
    uint32_t isp_dpcc_rnd_offs;
} CamerIcDpccStaticConfig_t;


#define MAX_BAD_PIXEL_CNT 10
/*****************************************************************************/
/**
 * @brief   This type defines the bad pixel table configuration structure of the
 *          CamerIc DPCC module (register configuration).
 */
/*****************************************************************************/
typedef struct CamerIcDpccBpt_s
{
	uint16_t badPixelTbl[MAX_BAD_PIXEL_CNT * 2]; //the bad pixel table
                                        //arrange  by vitcal addr and horizontal addr.
	uint16_t bpNumber;  //number of the bad pixels contained in the list
	uint8_t replace;  //replacement method
	bool enabled;

}CamerIcDpccBpt_t;

/*****************************************************************************/
/**
 * @brief   This function enables the DPCC Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval  RET_SUCCESS     operation succeded
 * @retval  RET_FAILURE     handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspDpccEnable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This function disables the DPCC Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval  RET_SUCCESS     operation succeded
 * @retval  RET_FAILURE     handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspDpccDisable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This function applies a static register configuration in
 *          the DPCC Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval  RET_SUCCESS     operation succeded
 * @retval  RET_FAILURE     handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspDpccSetStaticConfig
(
    CamerIcDrvHandle_t          handle,
    CamerIcDpccStaticConfig_t   *pConfig
);

/*****************************************************************************/
/**
 * @brief   This function applies a bad pixel table configuration in
 *          the DPCC Module
 *
 * @param   handle          CamerIc driver handle
 * @param   pConfig        dpcc bpt configuration
 *
 * @return                  Return the result of the function call.
 * @retval  RET_SUCCESS     operation succeded
 * @retval  RET_FAILURE     handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspDpccConfigBpt
(
    CamerIcDrvHandle_t          handle,
    CamerIcDpccBpt_t   *pConfig
);

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_dpcc_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_DPCC_DRV_API_H__ */

