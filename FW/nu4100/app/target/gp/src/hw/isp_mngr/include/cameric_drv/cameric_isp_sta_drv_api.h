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

#ifndef __CAMERIC_ISP_STA_DRV_API_H__
#define __CAMERIC_ISP_STA_DRV_API_H__

/**
 * @file cameric_isp_sta_drv_api.h
 *
 * @brief
 *
 *****************************************************************************/
/**
 * @page module_name_page Module Name
 * Describe here what this module does.
 *
 * For a detailed list of functions and implementation detail refer to:
 * - @ref module_name
 *
 * @defgroup cameric_isp_sta_drv_api CamerIc ISP STA Driver Internal API
 * @{
 *
 */
#include <ebase/types.h>
#include <common/return_codes.h>
#include <hal/hal_api.h>



/*****************************************************************************/
/**
 *          CamerIcIspStaEnable()
 *
 * @brief   Initialize CamerIc ISP STA driver context
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspStaEnable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 *          CamerIcIspStaDisable()
 *
 * @brief   Release/Free CamerIc ISP STA driver context
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspStaDisable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 *          CamerIcStitchStaEnable()
 *
 * @brief   Initialize CamerIc ISP STA driver context
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcStitchStaEnable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 *          CamerIcStitchStaDisable()
 *
 * @brief   Release/Free CamerIc ISP STA driver context
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcStitchStaDisable
(
    CamerIcDrvHandle_t handle
);


#endif /* __CAMERIC_ISP_STA_DRV_API_H__ */

