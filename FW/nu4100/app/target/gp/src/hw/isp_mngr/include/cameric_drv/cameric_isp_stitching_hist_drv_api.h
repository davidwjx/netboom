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

#ifndef __CAMERIC_ISP_STITCHING_HIST_DRV_API_H__
#define __CAMERIC_ISP_STITCHING_HIST_DRV_API_H__

/**
 * @cond    cameric_isp_hist
 *
 * @file    cameric_isp_hist_drv_api.h
 *
 * @brief   This file contains the CamerIC ISP HIST driver API definitions.
 *
 *****************************************************************************/
/**
 * @defgroup cameric_isp_hist_drv_api CamerIC ISP HIST driver API definitions
 * @{
 *
 * In general, a histogram is a graphical representation of the pattern of
 * variation  that exists in the intensity values of the color or luminance
 * planes. Usually it is displayed by vertical bars drawn to indicate frequency
 * levels of data collected within specific ranges.
 *
 * The following figure shows the position of the histogram measuring module in
 * the CamerIC ISP pipeline.
 *
 * @image html hist.png "Overview of the CamerIC ISP HIST driver" width=\textwidth
 * @image latex hist.png "Overview of the CamerIC ISP HIST driver" width=\textwidth
 *
 * The complete range of possible intensity values is divided into a number
 * (@ref CAMERIC_ISP_HIST_NUM_BINS) of equally-sized ranges, so called @b bins .
 * Each incoming intensity value is associated to one of these bins and gets
 * counted for that bin only.
 *
 * The histogram measurement can be configured to work in one of five modes
 * (@ref CamerIcIspStitchingHistMode_e):
 *
 * @arg R separated histogram: only the red color component of incoming RGB
 *      triples is measured.
 * @arg G separated histogram: only the green color component of incoming RGB
 *      triples is measured.
 * @arg B separated histogram: only the blue color component of incoming RGB
 *      triples is measured.
 * @arg RGB combined histogram: The sum of red, green and blue component of
 *      the incoming RGB triples is measured. Note that it is not possible
 *      to calculate a luminance or grayscale histogram from an RGB histogram
 *      since the position information is lost during its generation.
 * @arg Y (luminance) histogram: the luminance values of incoming RGB triples
 *      are measured.
 *
 * Further, the histogram measurement block can be configured to measure
 * not the whole incoming image, but the pixels in a smaller window only.
 * Size and position of that measuring window can be set with @ref
 * CamerIcIspStitchingHistSetMeasuringWindow.
 *
 * The histogram measurement window is also divided into a number (@ref
 * CAMERIC_ISP_HIST_GRID_ITEMS) of sub-windows. Each sub-window may be
 * assigned a different weight factor, so that the contribution of the
 * sub-window to the histogram bins is weighted.
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
 * @brief   This functions registers an Event-Callback at CamerIC ISP histogram
 *          measurement module. An event callback is called if the driver needs
 *          to inform the application layer about an asynchronous event or an
 *          error situation (i.e. please also @see CamerIcEventId_e).
 *
 * @param   handle              CamerIC driver handle
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
extern RESULT CamerIcIspStitchingHistRegisterEventCb
(
    CamerIcDrvHandle_t  handle,
    CamerIcEventFunc_t  func,
    void 			    *pUserContext
);



/*****************************************************************************/
/**
 * @brief   This functions deregisters/releases a registered Event-Callback
 *          at CamerIc ISP histogram measurement module.
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
extern RESULT CamerIcIspStitchingHistDeRegisterEventCb
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This functions enables the CamerIC ISP histogram measurement
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingHistEnable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This functions disables the CamerIC ISP histogram measurement
 *          module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingHistDisable
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   Get CamerIC ISP histogram measurement module status.
 *
 * @param   handle          CamerIc driver handle.
 * @param   pIsEnabled
 *
 * @return                  Return the result of the function call.
 * @retval                  RET_SUCCESS
 * @retval                  RET_FAILURE
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingHistIsEnabled
(
    CamerIcDrvHandle_t      handle,
    bool_t                  *pIsEnabled
);



/*****************************************************************************/
/**
 * @brief   This functions configures the measurement mode of the CamerIC
 *          ISP histogram measurement module.
 *
 * @param   handle              CamerIc driver handle
 * @param   mode                measurement mode (see @ref CamerIcIspStitchingHistMode_e)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_OUTOFRANGE      At least one perameter of out range
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
RESULT CamerIcIspStitchingHistSetMeasuringMode
(
    CamerIcDrvHandle_t          handle,
    const CamerIcIspHistMode_t    mode
);



/*****************************************************************************/
/**
 * @brief   This function sets the position and size of a the measurement
 *          window in the CamerIC ISP histogram measurement module.
 *
 * @param   handle              CamerIc driver handle
 * @param   x                   start x position of measuring window
 * @param   y                   start y position of measuring window
 * @param   width               width of measuring window
 * @param   height              height of measuring window
 *
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_INVALID_PARM    invalid window identifier
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/
extern RESULT CamerIcIspStitchingHistSetMeasuringWindow
(
    CamerIcDrvHandle_t  handle,
    const uint16_t      x,
    const uint16_t      y,
    const uint16_t      width,
    const uint16_t      height
);



/*****************************************************************************/
/**
 * @brief   This function configures the grid weights in the CamerIC ISP
 *          histogram measurement module.
 *
 * @param   handle              CamerIc driver handle
 * @param   weights             measurement mode (@see CamerIcHistWeights_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         Configuration successfully applied
 * @retval  RET_OUTOFRANGE      At least one perameter of out range
 * @retval  RET_WRONG_HANDLE    handle is invalid
 *
 *****************************************************************************/

#ifdef __cplusplus
}
#endif

/* @} cameric_isp_hist_drv_api */

/* @endcond */

#endif /* __CAMERIC_ISP_HIST_DRV_API_H__ */

