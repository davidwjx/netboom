#ifndef __CAMERIC_ISP_TDNR_DRV_API_H__
#define __CAMERIC_ISP_TDNR_DRV_API_H__

/**
* @file    cameric_isp_3dnr_drv_api.h
*
* @brief   This file contains the CamerIC ISP TDNR driver API definitions.
*
*****************************************************************************/
/**
* @cond cameric_isp_3dnr
*
* @defgroup cameric_isp_3dnr_drv_api CamerIC ISP TDNR Driver API definitions
* @{
*
*/
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif
typedef struct CamerIcIspTdnr3Config_s {
    bool enable;
    bool enable_inv_gamma;
    bool enable_pre_gamma;
    bool enable_motion_erosion;
    bool enable_motion_converage;
    bool enable_motion_dilation;
    bool enable_tnr;
    bool enable_2dnr;
    uint8_t strength; // [0, 128]
    uint16_t noise_level;   // [0, 100]  -> [0, 65535]
    uint16_t motion_slope;   // [0, 100]  -> [0, 2047]
    uint16_t sad_weight;  // [0, 16]
    uint16_t update_factor;  // [0, 1024]
    uint16_t motion_update_factor;  // [0, 1024]

    uint16_t dnr2_sigma; // [0, 8]
    uint16_t dnr2_strength;  // [0, 32]
    uint16_t motion_inv_factor;
    uint16_t noise_mean;
    uint16_t motion_mean;
    uint16_t pre_motion_weight;
    uint16_t range_h;
    uint16_t range_v;
    uint16_t dilate_range_h;
    uint16_t noise_threshold;
} CamerIcIspTdnr3Config_t;

typedef struct CamerIcIspTdnr3Stats_s{
	uint32_t bg_sum;
	uint32_t motion_sum;
	uint32_t bg_pixel_cnt;
	uint32_t motion_pixel_cnt;
	uint32_t frame_avg;
} CamerIcIspTdnr3Stats_t;

/*****************************************************************************/
/**
*          CamerIcIsp3DnrV3Enable()
*
* @brief   Enable 3DNRv3 Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIsp3DnrV3Enable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
*          CamerIcIsp3DnrV3Disable()
*
* @brief   Enable 3DNRv3 Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIsp3DnrV3Disable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
*          CamerIcIspTdnr3Enable()
*
* @brief   Enable TDNR Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspTdnr3Enable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
*          CamerIcIspTdnr3Disable()
*
* @brief   Disable TDNR Module
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspTdnr3Disable
(
	CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
*          CamerIcIspTdnr3UpdateCurve()
*
* @brief   Config 3dnr of TDNR Module Curve
*
* @param   handle          CamerIc driver handle
* @param   DENOISE3D_model_a        noise model parameters, calibrated according to gain
* @param   DENOISE3D_model_b        noise model parameters, calibrated according to gain
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspTdnr3UpdateCurve
(
    CamerIcDrvHandle_t  handle,
    float DENOISE3D_model_a,
    float DENOISE3D_model_b
);

/*****************************************************************************/
/**
*          CamerIcIspTdnr3NoiseUpdate()
*
* @brief   update  TDNR Module register noise config
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspTdnr3NoiseUpdate
(
	CamerIcDrvHandle_t  handle,
	float autoStrength
);

/*****************************************************************************/
/**
*          CamerIcIspTdnr3Update()
*
* @brief   update  TDNR Module threshold update & motion threshold update
*
* @param   handle          CamerIc driver handle
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspTdnr3ThrUpdate
(
	CamerIcDrvHandle_t  handle,
    int tnr_update_factor,
    int tnr_motion_update_factor
);

/*****************************************************************************/
/**
*          CamerIcIspTdnr3SetStrength()
*
* @brief   set TDNR Module's denoiseStrength
*
* @param   handle          CamerIc driver handle
* @param   denoiseStrength strength
*
* @return                  Return the result of the function call.
* @retval                  RET_SUCCESS
* @retval                  RET_FAILURE
*
*****************************************************************************/
RESULT CamerIcIspTdnr3SetStrength(
	CamerIcDrvHandle_t  handle,
	unsigned int denoiseStrength
);


RESULT CamerIcIspTdnr3GetConfig(
	CamerIcDrvHandle_t  handle,
	CamerIcIspTdnr3Config_t *pConfig
);

RESULT CamerIcIspTdnr3SetConfig(
	CamerIcDrvHandle_t  handle,
	const CamerIcIspTdnr3Config_t *pConfig
);

RESULT CamerIcIspTdnr3GetStats(
	CamerIcDrvHandle_t  handle,
    CamerIcIspTdnr3Stats_t *pStats
);

RESULT CamerIcIspTdnr3GetWindow(
	CamerIcDrvHandle_t  handle,
    CamerIcWindow_t *pWindow
);

void CamerIcIspTdnr3Reset(
    CamerIcDrvHandle_t handle,
    bool reset
);

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_3dnr_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_TDNR_DRV_API_H__ */

