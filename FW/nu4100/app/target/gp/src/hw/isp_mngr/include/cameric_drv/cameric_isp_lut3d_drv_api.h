/******************************************************************************
*
* Copyright 2010, Dream Chip Technologies GmbH. All rights reserved.
* No part of this work may be reproduced, modified, distributed, transmitted,
* transcribed, or translated into any language or computer format, in any form
* or by any means without written permission of:
* Dream Chip Technologies GmbH, Steinriede 10, 30827 Garbsen / Berenbostel,
* Germany
*
*****************************************************************************/
#ifndef __CAMERIC_ISP_LUT3D_DRV_API_H__
#define __CAMERIC_ISP_LUT3D_DRV_API_H__

/**
* @file    cameric_isp_lut3d_drv_api.h
*
* @brief   This file contains the CamerIC ISP LUT3D driver API definitions.
*
*****************************************************************************/
/**
* @cond cameric_isp_lut3d
*
* @defgroup cameric_isp_lut3d_drv_api CamerIC ISP LUT3D Driver API definitions
* @{
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
 * @brief   lut3d data configuration structure
 *
 *****************************************************************************/
typedef struct CamerIcIspLut3dTblConfig_s
{
    uint32_t Lut3dDataTbl[17*17*17];    /**< lut3d table data */

} CamerIcIspLut3dTblConfig_t;

/*****************************************************************************/
/**
 * @brief   This functions enable the CamerIc ISP lut3d module.
 * 
 * @param   handle              CamerIC driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              request callback
 *
 *****************************************************************************/
RESULT CamerIcIspLut3dEnable
(
    CamerIcDrvHandle_t handle    
);


/*****************************************************************************/
/**
 * @brief   This functions disable the CamerIc ISP lut3d module.
 * 
 * @param   handle              CamerIC driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              request callback
 *
 *****************************************************************************/
RESULT CamerIcIspLut3dDisable
(
    CamerIcDrvHandle_t handle    
);


/*****************************************************************************/
/**
 * @brief   Get CamerIC ISP lut3d module status.
 * 
 * @param   handle              CamerIC driver handle
 * @param   pIsEnable           the lut3d status
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              request callback
 *
 *****************************************************************************/
RESULT CamerIcIspLut3dIsEnable
(
    CamerIcDrvHandle_t handle,
    bool_t       *pIsEnabled    
);


/*****************************************************************************/
/**
 * @brief   set CamerIC ISP lut3d table select.
 * 
 * @param   handle              CamerIC driver handle
 * @param   lut3dsel            the lut3d table select
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              request callback
 *
 *****************************************************************************/
// RESULT CamerIcIspLut3dTblSel
// (
//     CamerIcDrvHandle_t handle,
//     u_int8_t    lut3dsel    
// );


/*****************************************************************************/
/**
 * @brief   set CamerIC ISP lut3d table.
 * 
 * @param   handle              CamerIC driver handle
 * @param   lut3d_rdata          the lut3d table rdata
 * @param   lut3d_gdata          the lut3d table gdata
 * @param   lut3d_bdata          the lut3d table bdata
 * @param   lut3d_r_lut_access_vld          the lut3d table r channel access 
 * @param   lut3d_g_lut_access_vld          the lut3d table g channel access 
 * @param   lut3d_b_lut_access_vld          the lut3d table b channel access 
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              request callback
 *
 *****************************************************************************/
RESULT CamerIcIspLut3dSetTbl
(
    CamerIcDrvHandle_t handle,
    CamerIcIspLut3dTblConfig_t    *lut3d_rdata,
    CamerIcIspLut3dTblConfig_t    *lut3d_gdata,
    CamerIcIspLut3dTblConfig_t    *lut3d_bdata,
    bool  lut3d_r_lut_access_vld,
    bool  lut3d_g_lut_access_vld,   
    bool  lut3d_b_lut_access_vld      
);




#ifdef __cplusplus
}
#endif

/* @} cameric_isp_lut3d_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_LUT3D_DRV_API_H__ */


