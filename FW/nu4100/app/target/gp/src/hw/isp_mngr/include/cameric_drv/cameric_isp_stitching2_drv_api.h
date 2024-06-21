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

#ifndef __CAMERIC_ISP_STITCHING2_DRV_API_H__
#define __CAMERIC_ISP_STITCHING2_DRV_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>
#include <hal/hal_api.h>

#include "cameric_isp_stitching_drv_api.h"
#include <cameric_drv/cameric_drv_api.h>
#if defined(MRV_STITCHING2_VERSION)||defined(ISP_HDR_STITCH2)
#ifdef __cplusplus
extern "C"
{
#endif

/******************************************************************************/
/**
 * @brief   Enumeration type to sensor id in stitching.
 *
 *****************************************************************************/
typedef enum CamerIcIspStitchingSensorId_e
{
    CAMERIC_ISP_STITCHING_SENSOR_0            = 0,			/**< Sensor 0 */
    CAMERIC_ISP_STITCHING_SENSOR_1            = 1,			/**< Sensor 1 */
    CAMERIC_ISP_STITCHING_SENSOR_MAX            			/**< Sensor MAX */
} CamerIcIspStitchingSensorId_t;

#define RATIO_NUM 2
#define TRANS_RANGE_HOR 4
#define TRANS_RANGE_VER 2
#define EXTEND_BIT_NUM 2
#define COLOR_WEIGHT_NUM 3
#define BLS_NUM 4
#define MOTION_NUM 2
#define MOTION_THRESHOLD_NUM 4
#define DARK_THRESHOLD_NUM 4
#define DGAIN_NUM 4
typedef struct CamerIcIspStitching2Context_s
{
    bool_t                            bypass;                            /**< measuring enabled */
    uint8_t                           bypass_select;                            
    CamerIcIspStitchingHdrMode_t       sensor_type;
    CamerIcIspStitchingCombinationMode_t        stitching_mode;
    uint8_t                           base_frame;
    float                             ratio[RATIO_NUM];
    float                             trans_range[TRANS_RANGE_HOR][TRANS_RANGE_VER];
    float                             extend_bit[EXTEND_BIT_NUM];
    uint32_t                          color_weight[COLOR_WEIGHT_NUM];
    uint32_t                          bls_long[BLS_NUM];
    uint32_t                          bls_short[BLS_NUM];
    uint32_t                          bls_very_short[BLS_NUM];
    bool_t                            motion_enable[MOTION_NUM];
    uint32_t                          motion_weight[MOTION_NUM];
    uint32_t                          motion_weight_shorter;               
    uint32_t                          motion_sat_threshold;               
    uint32_t                          motion_weight_update_threshold;    
    uint32_t                          motion_lower_threshold_ls[MOTION_THRESHOLD_NUM];
    uint32_t                          motion_upper_threshold_ls[MOTION_THRESHOLD_NUM];
    uint32_t                          motion_lower_threshold_lsvs[MOTION_THRESHOLD_NUM];
    uint32_t                          motion_upper_threshold_lsvs[MOTION_THRESHOLD_NUM];       
    uint32_t                          dark_lower_threshold_ls[DARK_THRESHOLD_NUM];
    uint32_t                          dark_upper_threshold_ls[DARK_THRESHOLD_NUM];
    uint32_t                          dark_lower_threshold_lsvs[DARK_THRESHOLD_NUM];
    uint32_t                          dark_upper_threshold_lsvs[DARK_THRESHOLD_NUM];
    uint8_t                           hdr_id;
    CamerIcIspStitchingHdrRdStr_t 			  rdWrStr;         //used for mi hdr dma
														      //0 invalid
															  //1 for L;
															  //2 for L and S;
															  //3 for L, S and VS
															  //4 for L and VS
    CamerIcGains_t                  awbGains;

#if defined(ISP_HDR_STITCH2_V1)
    bool_t dgain_enable;
    float dgain[DGAIN_NUM][DGAIN_NUM];

#endif //ISP_HDR_STITCH2_V1

} CamerIcIspStitching2Context_t;

#define NOISE_CURVE_NUM 17
typedef struct CamerIcIspStitching2DpfContext_s
{
    bool_t                            dpf_enable;
    float                             gain;    
    float                             gradient;    
    float                             offset;    
    float                             min;    
    float                             div;    
    float                             sigma_g;    
    float                             sigma_rb;    
    uint32_t                          noise_curve[NOISE_CURVE_NUM];  
} CamerIcIspStitching2DpfContext_t;

typedef struct CamerIcIspStitching2DeghostContext_s
{
    bool_t                            motion_enable[2];
    uint32_t                          motion_weight[2];
    uint32_t                          motion_weight_shorter;               
    uint32_t                          motion_sat_threshold;               
    uint32_t                          motion_weight_update_threshold;    
    uint32_t                          motion_lower_threshold_ls[4];
    uint32_t                          motion_upper_threshold_ls[4];
    uint32_t                          motion_lower_threshold_lsvs[4];
    uint32_t                          motion_upper_threshold_lsvs[4];       
    uint32_t                          dark_lower_threshold_ls[4];
    uint32_t                          dark_upper_threshold_ls[4];
    uint32_t                          dark_lower_threshold_lsvs[4];
    uint32_t                          dark_upper_threshold_lsvs[4];
} CamerIcIspStitching2DeghostContext_t;
/*****************************************************************************/
/**
 * @brief   config CamerIC ISP stitching module.
 *
 * @param   handle          CamerIc driver handle.
 * @param   pArg            CamericIspHdr V2Param_t pointer  
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingSetConfig
(
    CamerIcDrvHandle_t      handle,
    CamerIcIspStitching2Context_t     *pArg
);

/*****************************************************************************/
/**
 * @brief   This functions enables the CamerIC ISP Hdr V2 Dpf
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingDpfEnable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP Hdr V2 Dpf
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingDpfDisable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   config CamerIC ISP Hdr V2 dpf module.
 *
 * @param   handle          CamerIc driver handle.
 * @param   pArg            CamericIspHdr V2Param_t pointer  
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingDpfSetConfig
(
    CamerIcDrvHandle_t  handle,
    CamerIcIspStitching2DpfContext_t     *pArg
);

/*****************************************************************************/
/**
 * @brief   This functions enables the CamerIC ISP Hdr V2 Deghost
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingDeghostEnable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP Hdr V2 Deghost
 *          module. 
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingDeghostDisable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   config CamerIC ISP Hdr V2 deghost module.
 *
 * @param   handle          CamerIc driver handle.
 * @param   pArg            CamericIspHdr V2Param_t pointer  
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingDeghostSetConfig
(
    CamerIcDrvHandle_t  handle,
    CamerIcIspStitching2DeghostContext_t     *pArg
);



/*****************************************************************************/
/**
 *          CamerIcIspStitchingSetBlackLevel()
 *
 * @brief   Disable STITCHING Module
 *
 * @param   handle          CamerIc driver handle
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcIspStitchingSetBlackLevel(
    CamerIcDrvHandle_t handle,
    const uint16_t isp_bls_a_fixed,
    const uint16_t isp_bls_b_fixed,
    const uint16_t isp_bls_c_fixed,
    const uint16_t isp_bls_d_fixed);


/*****************************************************************************/
/**
 *          CamerIcIspStitchingSetAwbGain()
 *
 * @brief   set Stitching awb gain
 * @param   handle          CamerIc driver handle.
 * @param   pGains         gains config parameter
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingSetAwbGain(
    CamerIcDrvHandle_t handle,
    const CamerIcGains_t *pGains);

/**
 *          CamerIcIspStitchingGetAwbGain()
 *
 * @brief   get Stitching awb gain
 * @param   handle          CamerIc driver handle.
 * @param   pGains         awb gains
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingGetAwbGain(
    CamerIcDrvHandle_t handle,
    CamerIcGains_t *pGains);

RESULT CamerIcIspStitchingDeghostInit
(
    CamerIcDrvHandle_t handle
);

RESULT CamerIcIspSetStitchingPathProperties
(
    CamerIcDrvHandle_t handle,
    const CamerIcIspStitchingProperties_t *pProperties,
    uint8_t     hdr_id
);

RESULT CamerIcIspStitchingGetConfig(
    CamerIcDrvHandle_t handle,
    CamerIcIspStitching2Context_t     *stitching2_config
);

RESULT CamerIcIspStitchingDpfGetConfig(
    CamerIcDrvHandle_t handle,
    CamerIcIspStitching2DpfContext_t     *dpf_config
);

RESULT CamerIcIspStitchingDeghostGetConfig(
    CamerIcDrvHandle_t handle,
    CamerIcIspStitching2DeghostContext_t     *deghost_config
);


#ifdef __cplusplus
}
#endif

#endif /*ifndef MRV_STITCHING2_VERSION*/
#endif /* __CAMERIC_ISP_STITCHING2_DRV_API_H__ */

