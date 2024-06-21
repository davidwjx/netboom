/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
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

#ifndef __CAMERIC_ISP_SBI_MI_API_H__
#define __CAMERIC_ISP_SBI_MI_API_H__

/**
 * @file cameric_isp_sbi_mi_api.h
 *
 * @brief   This file contains the CamerIC ISP SBI MI driver API definitions.
 *
 *****************************************************************************/

/**
 * @cond cameric_isp_sbi_mi
 *
 * @defgroup cameric_isp_sbi_mi_api CamerIC ISP SBI MI Driver API definitions
 * @{
 *
 */

#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct CamerIcEzSbiMiCfg_s {
  uint32_t available;
  uint32_t slice_per_frame;
  uint32_t Y_segment_entry_size;
  uint32_t Y_segment_entry_count;
  uint32_t CB_segment_entry_size;
  uint32_t CB_segment_entry_count;
  uint32_t CR_segment_entry_size;
  uint32_t CR_segment_entry_count;
  int valid_width;
  uint32_t g_sizeY;
  uint32_t g_sizeCb;
}CamerIcEzSbiMiCfg_t;	

/*****************************************************************************/
/**
 * @brief   This function init the SBI mi module
 * @param   handle          	CamerIc driver handle
 *
 * @return                  	Return the result of the function call.
 * @retval	RET_SUCCESS			  operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspSbiMiCfgSet(CamerIcDrvHandle_t handle,        CamerIcEzSbiMiCfg_t *pEzSbiMiCfg);

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_sbi_mi_api */

/* @endcond */

#endif /* __CAMERIC_ISP_SBI_MI_API_H__ */

