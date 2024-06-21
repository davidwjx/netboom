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

/**
 * @cond    cam_engine_reg
 *
 * @file    cam_engine_reg_api.h
 *
 * @brief
 *
 *   Interface description of the CamEngine reg.
 *
 *****************************************************************************/
/**
 *
 * @defgroup cam_engine_reg_api CamEngine reg API
 * @{
 *
 */

#ifndef __CAM_ENGINE_REG_API_H__
#define __CAM_ENGINE_REG_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>
#include <cameric_reg_drv/cameric_reg_description.h>

#ifdef __cplusplus
extern "C"
{
#endif


/*****************************************************************************/
/**
 *          CamEngineRegDescriptionDrv_t
 *
 * @brief   Configuration to setup CamEngine Register Description Driver
 *
 *****************************************************************************/

typedef struct CamEngineRegDescriptionDrv_s
{
    HalHandle_t     HalHandle;          /**< HAL handle to access CamerIc Hardware */
} CamEngineRegDescriptionDrv_t;


typedef struct CamEngineDrvContext_s
{
    HalHandle_t HalHandle;
} CamEngineDrvContext_t;

/******************************************************************************/
/**
 *          CamEngineRegDescription_t
 *
 * @brief   Type for defining a register description
 *
 *****************************************************************************/
typedef struct CamEngineRegDescription_s
{
    uint32_t        Address;            /**< adddress of the register */
    Permissions_t   Permission;         /**< access mode to the register */
    RegName_t       Name;               /**< name string of the register */
    RegHint_t       Hint;               /**< hint string of the register */
    uint32_t        ResetValue;         /**< reset value of the register */
    uint32_t        UsedBits;           /**< bitmask of used bits */
    uint32_t        WriteAbleBits;      /**< bitmask of writeable bits */
} CamEngineRegDescription_t;

/*******************************************************************************
 *
 *          CamEngineInitRegDescriptionDrv
 *
 * @brief   Initializes the CamEngine Register Description
 *
 * @param	pConfig    pointes to configuration structure
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS
 * @retval	RET_FAILURE
 *
 *****************************************************************************/
RESULT CamEngineInitRegDescriptionDrv
(
    const CamEngineRegDescriptionDrv_t  *pConfig,
    CamEngineDrvContext_t *regDesctx
);

/*******************************************************************************
 *
 *          CamEngineReleaseRegDescriptionDrv
 *
 * @brief   Releases the Camengine Register Description
 *
 * @param	CamEngineDrvContext_t *regDesctx
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS
 * @retval	RET_FAILURE
 *
 *****************************************************************************/
RESULT CamEngineReleaseRegDescriptionDrv
(
    CamEngineDrvContext_t *regDesctx
);

/*******************************************************************************
 *
 *          CamEngineGetRegister
 *
 * @brief   Returns the register content
 *
 * @param	address     register to read
 *          value       pointer to a value
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS
 * @retval	RET_FAILURE
 *
 *****************************************************************************/
RESULT CamEngineGetRegister
(
    CamEngineDrvContext_t *regDesctx,
    const uint32_t  address,
    uint32_t        *value
);


/*******************************************************************************
 *
 *          CamEngineSetRegister
 *
 * @brief   Set register content
 *
 * @param	address     register to set
 *          value       value to write
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS
 * @retval	RET_FAILURE
 *
 *****************************************************************************/
RESULT CamEngineSetRegister
(
    CamEngineDrvContext_t *regDesctx,
    const uint32_t  address,
    const uint32_t  value
);

/*******************************************************************************
 *
 *          CamEngineGetRegisterDescription
 *
 * @brief  	Returns the number and Register-Description
 *
 * @param[in]   Module          Module Identifier
 * @param[out]  NumRegisters    Number of Registers
 * @param[out]  Registers       Pointer to Register-Description
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS
 * @retval	RET_FAILURE
 * @return 	RET_NULL_POINTER
 *
 *****************************************************************************/
RESULT CamEngineGetRegisterDescription
(
    const int        ModuleId,
    uint32_t         *NumRegisters,
    RegDescription_t **Registers
);

#ifdef __cplusplus
}
#endif


/* @} cam_engine_reg_api */


#endif /* __CAM_ENGINE_REG_API_H__ */

