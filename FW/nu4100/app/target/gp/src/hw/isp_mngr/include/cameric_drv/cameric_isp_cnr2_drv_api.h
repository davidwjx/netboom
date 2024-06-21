#ifndef __CAMERIC_ISP_CNR2_DRV_API_H__
#define __CAMERIC_ISP_CNR2_DRV_API_H__

/**
* @file    cameric_isp_cnr2_drv_api.h
*
* @brief   This file contains the CamerIC ISP CNR2 driver API definitions.
*
*****************************************************************************/
/**
* @cond cameric_isp_cnr2
*
* @defgroup cameric_isp_cnr2_drv_api CamerIC ISP CNR2 Driver API definitions
* @{
*
*/
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define SIGMA_LAYER_NUM 4
#define SIMGA_MERGE_NUM 3
typedef struct CamerIcIspCnr2Config_s {
	bool ynr_enable;
	bool cnr_enable;
	float c_sigma_layer[SIGMA_LAYER_NUM];
	float c_sigma_merge[SIMGA_MERGE_NUM];
	float y_sigma;
	uint8_t bit;
	uint8_t cnr_strength1;
	uint8_t cnr_strength2;
	uint8_t ynr_strength1;
	uint8_t ynr_strength2;
    uint8_t texture_mask_select;
} CamerIcIspCnr2Config_t;


/*****************************************************************************/
/**
 * @brief   This function enable the CamerIC ISP CNR2.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspCnr2Enable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 * @brief   This function disable the CamerIC ISP CNR2.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspCnr2Disable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP CNR2 enable statues.
 *
 * @param   handle              CamerIc driver handle
 * @param   pIsEnabled          cnr2 enable status
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspCnr2IsEnabled
(
    CamerIcDrvHandle_t      handle,
    bool_t                  *pIsEnabled
);

/*****************************************************************************/
/**
 * @brief  This function enable the CamerIC ISP YNR.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspYnrEnable
(
    CamerIcDrvHandle_t handle
);

/*****************************************************************************/
/**
 * @brief   This function disable the CamerIC ISP YNR.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspYnrDisable
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP YNR enable statues.
 *
 * @param   handle              CamerIc driver handle
 * @param   pIsEnabled          Ynr enable status
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspYnrIsEnabled
(
    CamerIcDrvHandle_t      handle,
    bool_t                  *pIsEnabled
);

/*****************************************************************************/
/**
 * @brief   This function config the CamerIC ISP CNR2 module.
 *
 * @param   handle              CamerIc driver handle
 * @param   pConfig             configure of the CNR2 module
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspCnr2SetConfig
( 
	CamerIcDrvHandle_t  handle,
	CamerIcIspCnr2Config_t *pConfig
);

/*****************************************************************************/
/**
 * @brief   This function set the CamerIC ISP CNR2 strength.
 *
 * @param   handle              CamerIc driver handle
 * @param   cnr_strength1       cnr_strength1
 * @param   cnr_strength2       cnr_strength2
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspSetCnr2Strength
(
    CamerIcDrvHandle_t  handle,
    uint8_t cnr_strength1,
    uint8_t cnr_strength2
);

/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP CNR2 strength.
 *
 * @param   handle              CamerIc driver handle
 * @param   cnr_strength1       cnr_strength1
 * @param   cnr_strength2       cnr_strength2
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspGetCnr2Strength
(
    CamerIcDrvHandle_t  handle,
    uint8_t *cnr_strength1,
    uint8_t *cnr_strength2
);

/*****************************************************************************/
/**
 * @brief   This function set the CamerIC ISP YNR strength.
 *
 * @param   handle              CamerIc driver handle
 * @param   ynr_strength1       ynr_strength1
 * @param   ynr_strength2       ynr_strength2
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspSetYnrStrength
(
    CamerIcDrvHandle_t  handle,
    uint8_t ynr_strength1,
    uint8_t ynr_strength2
);

/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP YNR strength.
 *
 * @param   handle              CamerIc driver handle
 * @param   ynr_strength1       ynr_strength1
 * @param   ynr_strength2       ynr_strength2
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspGetYnrStrength
(
    CamerIcDrvHandle_t  handle,
    uint8_t *ynr_strength1,
    uint8_t *ynr_strength2
);

/*****************************************************************************/
/**
 * @brief   This function set the CamerIC ISP cnr2 table.
 *
 * @param   handle              CamerIc driver handle
 * @param   c_sigma_layer       c_sigma_layer
 * @param   c_sigma_merge       c_sigma_merge
 * @param   bit                 bit
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspSetCnr2WeightTable
(
    CamerIcDrvHandle_t  handle,
    float *c_sigma_layer,
    float *c_sigma_merge,
    uint8_t bit
);

/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP cnr2 table.
 *
 * @param   handle              CamerIc driver handle
 * @param   c_sigma_layer       c_sigma_layer
 * @param   c_sigma_merge       c_sigma_merge
 * @param   bit                 bit
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspGetCnr2WeightTable
(
    CamerIcDrvHandle_t  handle,
    float *c_sigma_layer,
    float *c_sigma_merge,
    uint8_t *bit
);

/*****************************************************************************/
/**
 * @brief   This function set the CamerIC ISP ynr table.
 *
 * @param   handle              CamerIc driver handle
 * @param   y_sigma_layer       y_sigma_layer
 * @param   bit                 bit
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspSetYnrWeightTable
(
    CamerIcDrvHandle_t  handle,
    float y_sigma_layer,
    uint8_t bit
);

/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP ynr table.
 *
 * @param   handle              CamerIc driver handle
 * @param   y_sigma_layer       y_sigma_layer
 * @param   bit                 bit
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspGetYnrWeightTable
(
    CamerIcDrvHandle_t  handle,
    float *y_sigma,
    uint8_t *bit    
);

/*****************************************************************************/
/**
 * @brief   This function set the CamerIC ISP cnr2 filter.
 *
 * @param   handle              CamerIc driver handle
 * @param   texture_mask_select    texture_mask_select
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspSetCnr2Filter
(
    CamerIcDrvHandle_t  handle,
    uint8_t texture_mask_select
);


/*****************************************************************************/
/**
 * @brief   This function get the CamerIC ISP cnr2 filter.
 *
 * @param   handle              CamerIc driver handle
 * @param   texture_mask_select    texture_mask_select
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    pIsEnabled is a NULL pointer
 *
 *****************************************************************************/
RESULT CamerIcIspGetCnr2Filter
(
    CamerIcDrvHandle_t  handle,
    uint8_t *texture_mask_select
);

#ifdef __cplusplus
}
#endif

#endif