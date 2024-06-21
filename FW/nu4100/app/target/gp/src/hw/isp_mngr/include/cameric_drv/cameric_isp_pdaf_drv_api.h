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

#ifndef __CAMERIC_ISP_PDAF_DRV_API_H__
#define __CAMERIC_ISP_PDAF_DRV_API_H__

/**
 * @cond    cameric_isp_pdaf
 *
 * @file    cameric_isp_pdaf_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP Pdaf Driver API definitions
 *
 *****************************************************************************/

#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum CamercIcIspPdafSensorType_e {
    CAMERIC_ISP_PDAF_SENSOR_OCL2X1 = 0,
    CAMERIC_ISP_PDAF_SENSOR_DUAL_PIXEL = 1,
    CAMERIC_ISP_PDAF_SENSOR_TYPE_MAX,
}CamerIcIspPdafSensorType_t;

typedef enum CamerIcIspPdafSensorBayerPattern_e {
    CAMERIC_ISP_PDAF_BPT_BGGR = 0,
    CAMERIC_ISP_PDAF_BPT_GBRG,
    CAMERIC_ISP_PDAF_BPT_GRBG,
    CAMERIC_ISP_PDAF_BPT_RGGB,
    CAMERIC_ISP_PDAF_BPT_MAX,
}CamerIcIspPdafSensorBayerPattern_t;

#define SHIFT_HW_AREA_NUM 2
#define PD_AERA_NUM 4
#define NUM_PER_AERA 2
#define SHIFT_MARK_NUM 32
#define SHIFT_LR_NUM 2
typedef struct CamericIspPdafParam_s {
    bool_t enable;                                          //on/off
    bool_t correctEnable;                                   //on/off correction
    CamerIcIspPdafSensorType_t sensorType;                            //OCL2x1 or dual pixel   
    CamerIcIspPdafSensorBayerPattern_t bayerPattern;        //bggr gbrg grbg
    uint32_t width;
    uint32_t height;
    uint8_t pix00ShiftHw[SHIFT_HW_AREA_NUM];                                //H/V shift of the (0,0)pixel of raw in a pd area
    uint8_t correctThresHold;                              //the threshold to judge iso/aniso
    CamerIcWindow_t wcorrectRectWin;                              //correction rect windows
    uint16_t  pdArea[PD_AERA_NUM];                                       
    uint8_t    numPerArea[NUM_PER_AERA];                            //H/V pd number of each pd area
    uint8_t    shiftMark[SHIFT_MARK_NUM] ;                             //H/V pd shift
    uint8_t    shiftLr[SHIFT_LR_NUM];                                   //H/V shift from left to right
    CamerIcWindow_t roiWin;                            //roi windows
    uint8_t     correctRChannel;                       //color filter channel
    uint8_t     correctRLimitBase;                       //the base ration of change limit
    uint8_t     correctRChangeDown;                       //the max change of down limit
    uint8_t     correctRChangeUp;                       //the max change of up limit

    uint8_t     correctLChannel;                       //color filter channel
    uint8_t     correctLLimitBase;                       //the base ration of change limit
    uint8_t     correctLChangeDown;                       //the max change of down limit
    uint8_t     correctLChangeUp;                       //the max change of up limit
}CamericIspPdafParam_t;
/*****************************************************************************/
/**
 * @brief   This functions registers an Event-Callback at CamerIC ISP  PDAF
 *          module. An event callback is called if the driver needs
 *          to inform the application layer about an asynchronous event or an
 *          error situation (i.e. please also @see CamerIcEventId_e).
 *
 * @param   handle              CamerIc driver handle
 * @param   func                Callback function
 * @param   pUserContext        User-Context
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_BUSY            already a callback registered
 * @retval  RET_WRONG_HANDLE    handle is invalid
 * @retval  RET_INVALID_PARM    a parameter is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to register a
 *                              event callback (maybe the driver is already
 *                              running)
 *
 *****************************************************************************/
extern RESULT CamerIcIspPdafRegisterEventCb
(
    CamerIcDrvHandle_t  handle,
    CamerIcEventFunc_t  func,
    void 			    *pUserContext
);



/*****************************************************************************/
/**
 * @brief   This functions deregisters/releases a registered Event-Callback
 *          at CamerIc ISP Pdaf module.
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
extern RESULT CamerIcIspPdafDeRegisterEventCb
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   This functions enables the CamerIC ISP Pdaf
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspPdafEnable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP Pdaf
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspPdafDisable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   Get CamerIC ISP Pdaf module status.
 *
 * @param   handle          CamerIc driver handle.
 * @param   pIsEnabled
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspPdafIsEnabled
(
    CamerIcDrvHandle_t      handle,
    bool_t                  *pIsEnabled
);

/*****************************************************************************/
/**
 * @brief   config CamerIC ISP Pdaf module.
 *
 * @param   handle          CamerIc driver handle.
 * @param   pArg            CamericIspPdafParam_t pointer  
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspPdafSetConfig
(
    CamerIcDrvHandle_t      handle,
    CamericIspPdafParam_t     *pArg
);

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_pdaf_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_PDAF_DRV_API_H__ */

