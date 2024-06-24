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
 * @file mom_ctrl_api.h
 *
 * @brief
 *   ADD_DESCRIPTION_HERE
 *
 *****************************************************************************/
/**
 * @page module_name_page Module Name
 * Describe here what this module does.
 *
 * For a detailed list of functions and implementation detail refer to:
 * - @ref module_name
 *
 * @defgroup module_name Module Name
 * @{
 *
 */
#ifndef __MOM_CTRL_API_H__
#define __MOM_CTRL_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>

#include <hal/hal_api.h>

#include <bufferpool/media_buffer.h>
#include <bufferpool/media_buffer_queue_ex.h>

#include <cameric_drv/cameric_drv_api.h>

#include "mom_ctrl_common.h"



/**
 * @brief   Configuration structure of the mom-ctrl
 *
 * @note
 *
 */
typedef struct MomCtrlConfig_s
{
    uint32_t                MaxPendingCommands;     /**< Number of commands that can be queued and thus be pending at a time. */

    uint32_t                NumBuffersLargePic;     /**< Number of buffers in large bufferpool */
    uint32_t                NumBuffersSmallPic;     /**< Number of buffers in small bufferpool */

    MediaBufPool_t          *pPicBufPoolLargePic;   /**< Reference to output large buffer pool */
    MediaBufPool_t          *pPicBufPoolSmallPic;   /**< Reference to output small buffer pool */

    MediaBufQueueEx_t       *pPicBufQueueLargePic;  /**< Reference to output queue to connect to. */
    MediaBufQueueEx_t       *pPicBufQueueSmallPic;  /**< Reference to output queue to connect to. */

    uint32_t                NumBuffersUserPic[ISPCORE_BUFIO_WRITEMAX];        /**< Number of buffers in user bufferpool */
    MediaBufPool_t          *pUserPicBufPoolPic[ISPCORE_BUFIO_WRITEMAX];   /**< Reference to output user buffer pool */
    MediaBufQueueEx_t       *pUserPicBufQueuePic[ISPCORE_BUFIO_WRITEMAX];  /**< Reference to user output queue to connect to. */

    MomCtrlCompletionCb_t   momCbCompletion;        /**< Callback function for command completion. */
    void                    *pUserContext;          /**< User context passed on to completion callback. */

    CamerIcDrvHandle_t      hCamerIc;               /**< CamerIc Driver handle */
    HalHandle_t             HalHandle;
    BUFF_MODE               buf_work_mode;

    MomCtrlContextHandle_t  hMomContext;            /**< Handle to mom control context, set by @ref MomCtrlInit if successfull, undefined otherwise. */
} MomCtrlConfig_t;



/*****************************************************************************/
/**
 * @brief   Initialize the MOM-Control
 *
 * This functions initializes the Memory-Output-Modul .
 *
 * Some detailed description goes here ...
 *
 * @param   param1      Describe the parameter 1.
 * @param   param2      Describe the parameter 2
 *
 * @return              Return the result of the function call.
 * @retval              RET_VAL1
 * @retval              RET_VAL2
 *
 *****************************************************************************/
RESULT MomCtrlInit
(
    MomCtrlConfig_t         *pConfig
);



/*****************************************************************************/
/**
 * @brief   Shutdown the MOM-Control
 *
 * This functions releases and shutdowns the Memory-Output-Modul .
 *
 * Some detailed description goes here ...
 *
 * @param   param1      Describe the parameter 1.
 * @param   param2      Describe the parameter 2
 *
 * @return              Return the result of the function call.
 * @retval              RET_VAL1
 * @retval              RET_VAL2
 *
 *****************************************************************************/
RESULT MomCtrlShutDown
(
    MomCtrlContextHandle_t hMomContext
);



/*****************************************************************************/
/**
 * @brief   Start the MOM-Control
 *
 * This functions starts the Memory-Output-Modul .
 *
 * Some detailed description goes here ...
 *
 * @param   param1      Describe the parameter 1.
 * @param   param2      Describe the parameter 2
 *
 * @return              Return the result of the function call.
 * @retval              RET_VAL1
 * @retval              RET_VAL2
 *
 *****************************************************************************/
RESULT MomCtrlStart
(
    MomCtrlContextHandle_t hMomContext
);



/*****************************************************************************/
/**
 * @brief   Initialize the MOM-Control
 *
 * This functions initializes the Memory-Output-Modul .
 *
 * Some detailed description goes here ...
 *
 * @param   param1      Describe the parameter 1.
 * @param   param2      Describe the parameter 2
 *
 * @return              Return the result of the function call.
 * @retval              RET_VAL1
 * @retval              RET_VAL2
 *
 *****************************************************************************/
RESULT MomCtrlStop
(
    MomCtrlContextHandle_t hMomContext
);



/*****************************************************************************/
/**
 * @brief   attach a media buffer queue to an output path
 *
 * This functions initializes the Memory-Output-Modul .
 *
 * Some detailed description goes here ...
 *
 * @param   param1      Describe the parameter 1.
 * @param   param2      Describe the parameter 2
 *
 * @return              Return the result of the function call.
 * @retval              RET_VAL1
 * @retval              RET_VAL2
 *
 *****************************************************************************/
RESULT MomCtrlAttachQueueToPath
(
    MomCtrlContextHandle_t  hMomContext,
    const uint32_t          path,
    osQueue                 *pQueue
);



/*****************************************************************************/
/**
 * @brief   detach a media buffer queue from an output path
 *
 * This functions initializes the Memory-Output-Modul .
 *
 * Some detailed description goes here ...
 *
 * @param   param1      Describe the parameter 1.
 * @param   param2      Describe the parameter 2
 *
 * @return              Return the result of the function call.
 * @retval              RET_VAL1
 * @retval              RET_VAL2
 *
 *****************************************************************************/
RESULT MomCtrlDetachQueueToPath
(
    MomCtrlContextHandle_t  hMomContext,
    const uint32_t          path,
    osQueue                 *pQueue
);



#if 0
/*****************************************************************************/
/**
 * @brief   TODO
 *
 *****************************************************************************/
RESULT  MomCtrlStartDraw
(
    MomCtrlContextHandle_t  hMomContext,
    MomCtrlDrawConfig_t     *pDrawConfig
);



/*****************************************************************************/
/**
 * @brief   TODO
 *
 *****************************************************************************/
RESULT  MomCtrlStopDraw
(
    MomCtrlContextHandle_t  hMomContext,
    MomCtrlDrawHandle_t     hDrawContext
);


/*****************************************************************************/
/**
 * @brief   TODO
 *
 *****************************************************************************/
RESULT  MomCtrlDraw
(
    MomCtrlContextHandle_t  hMomContext,
    MomCtrlDrawHandle_t     hDrawContext,
    uint32_t                numCmds,
    MomCtrlDrawCmd_t        *pDrawCmds
);
#endif


/*****************************************************************************/
/**
 * @brief   TODO
 *
 *****************************************************************************/
RESULT  MomCtrlRegisterBufferCb
(
    MomCtrlContextHandle_t  hMomContext,
    const uint32_t          poolId,
    MomCtrlBufferCb_t       fpCallback,
    void                    *pBufferCbCtx
);



/*****************************************************************************/
/**
 * @brief   TODO
 *
 *****************************************************************************/
RESULT  MomCtrlDeRegisterBufferCb
(
    MomCtrlContextHandle_t  hMomContext,
    const uint32_t          path
);

/* @} module_name*/

#endif /* __MOM_CTRL_API_H__ */

