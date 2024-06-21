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

#ifndef __CAMERIC_ISP_GTM_DRV_API_H__
#define __CAMERIC_ISP_GTM_DRV_API_H__

/**
 * @cond    cameric_isp_gtm
 *
 * @file    cameric_isp_gtm_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP GTM driver API definitions.
 *
 *****************************************************************************/
/**
 * @defgroup cameric_isp_gtm_drv_api CamerIC ISP GTM driver API definitions
 * @{
 *
 * GTM performs global tone mapping on the input image. It compresses the input 
 * image from 24 bit to 20 bit, and then through the WDR block.
 *
 * The following figure shows the position of the histogram measuring module in
 * the CamerIC ISP pipeline.
 *
 *
 * The complete range of possible intensity values is divided into a number
 * (@ref CAMERIC_ISP_GTM_HIST_BINS) of equally-sized ranges, so called @b bins .
 * Each incoming intensity value is associated to one of these bins and gets
 * counted for that bin only.
 *
 *
 *
 */
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************/
/**
 * @brief   This typedef represents the global tone mapping which is measured by  
 *          the CamerIc ISP Gtm module.
 *
 *****************************************************************************/
#define CAMERIC_ISP_GTM_HIST_BINS    128

typedef struct CamerIcGtmSta_s {
    uint32_t    min;
    uint32_t    max;
    uint32_t    hist[CAMERIC_ISP_GTM_HIST_BINS];
}CamerIcGtmSta_t;


/*****************************************************************************/
/**
 * @brief   This typedef represents the global tone mapping curve of CamerIc 
 *          ISP Gtm module.
 *
 *****************************************************************************/
#define CAMERIC_ISP_GTM_CURVE_SIZE    129

typedef struct CamerIcIspGtmCurve_s
{
    uint32_t luma[CAMERIC_ISP_GTM_CURVE_SIZE];   
} CamerIcIspGtmCurve_t;


/*****************************************************************************/
/**
 * @brief   This typedef specifies an array type to configure the weights
 *          of CamerIC ISP GTM module.
 *
 *****************************************************************************/
typedef struct CamerIcGtmWeights_s{
    uint8_t         lightnessWeight;
    uint8_t         weight0;
    uint8_t         weight1;
    uint8_t         weight2;
}CamerIcGtmWeights_t;


/*****************************************************************************/
/**
 * @brief   This typedef specifies an array type to configure the rgb coefficient
 *          of CamerIC ISP GTM module.
 *
 *****************************************************************************/
typedef struct CamerIcGtmRgbCoeff_s{
    uint8_t              rCoeff;
    uint8_t              gCoeff;
    uint8_t              bCoeff;
}CamerIcGtmRgbCoeff_t;


/*****************************************************************************/
/**
 * @brief   This functions enables the CamerIC ISP global tone mapping
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspGtmEnable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP global tone mapping
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspGtmDisable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP global tone mapping
 *          module.
 *
 * @param   handle              CamerIc driver handle
 * @param   pIsEnabled
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspGtmIsEnabled
(
    CamerIcDrvHandle_t      handle,
    bool_t                  *pIsEnabled
);

/*****************************************************************************/
/**
 * @brief   This functions enables the CamerIC ISP global tone mapping
 *          black white color module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspGtmBWCorEnable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP global tone mapping
 *          black white color module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspGtmBWCorDisable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 * @brief   This function sets the position and size of a the measurement
 *          window in the CamerIC ISP global tone mapping module.
 *
 * @param   handle              CamerIc driver handle
 * @param   weights             Weight of luma compent (@see CamerIcGtmWeights_t)
 *
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_INVALID_PARM    invalid window identifier
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspGtmSetWeights
(
    CamerIcDrvHandle_t          handle,
    const CamerIcGtmWeights_t   weights
);

/*****************************************************************************/
/**
 * @brief   This function configures the grid weights in the CamerIC ISP
 *          global tone mapping module.
 *
 * @param   handle              CamerIc driver handle
 * @param   base                Black white correction base param
 * @param   slope               Black white correction slope param
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_OUTOFRANGE      At least one perameter of out range
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspGtmSetBWCorBaseSlope
(
    CamerIcDrvHandle_t          handle,
    const uint32_t              base,
    const uint32_t              slope
);

/*****************************************************************************/
/**
 * @brief   This function configures the subsample range in the CamerIC ISP
 *          global tone mapping module.
 *
 * @param   handle                  CamerIc driver handle
 * @param   CamerIcGtmRgbCoeff_t    This param is set to calculate luma as coefficient for r, g, b.
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_OUTOFRANGE      At least one perameter of out range
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspGtmSetCoeff
(
    CamerIcDrvHandle_t          handle,
    const CamerIcGtmRgbCoeff_t  coeff
);

/*****************************************************************************/
/**
 * @brief   This function configures the subsample range in the CamerIC ISP
 *          global tone mapping module.
 *
 * @param   handle               CamerIc driver handle
 * @param   CamerIcGtmCurve_t    This param is set to gtm curve.
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_OUTOFRANGE      At least one perameter of out range
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspSetGtmCurve
(
    CamerIcDrvHandle_t                      handle,
    const CamerIcIspGtmCurve_t              *pGtmCurve
);

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_gtm_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_GTM64_DRV_API_H__ */

