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

//! Powered by Yong Yang

#pragma once

//#ifndef CAMERA_DEVICE
//#include <ISP_APP_SHELL-config.hpp>
//#endif

#define PROJECT_NAME_TUNING_SERVER "tuning-camera-old"
#define PROJECT_VERSION_TUNING_SERVER "4.3.0"
#define PROJECT_TIME_TUNING_SERVER "2022-09-21 13:51:42"

#if !defined(UNIX)
#define UNIX
#endif // UNIX

#if !defined(WIN32)
/* #undef WIN32 */
#endif // WIN32

// clang-format off
#define HAL_PORT 0

#define ISP_MAX 2
#define ISP_INPUT_MAX 2

/* #undef CTRL_CMODEL */
/* #undef CTRL_DEHAZE */
/* #undef CTRL_DEWARP */
#define CTRL_DOM
/* #undef CTRL_DRM */
/* #undef CTRL_EXA */
#define CTRL_FPGA
#define CTRL_SENSOR
/* #undef CTRL_USB */
#define CTRL_VOM
// clang-format on

#if defined(ISP_VERSION)
#define SDK_VERSION ISP_VERSION
#else
#define SDK_VERSION "Unknown"
#endif // ISP_VERSION

#if defined(ISP_2DNR)
#define CTRL_2DNR
#endif // ISP_2DNR

#if defined(ISP_2DNR_V5)
#define CTRL_2DNR5
#endif // ISP_2DNR_V5

#if defined(ISP_3DNR) || defined(ISP_3DNR_V2)
#define CTRL_3DNR
#endif // ISP_3DNR

#if (defined(ISP_3DNR_V3) && !defined(ISP_3DNR_V32))
#define CTRL_3DNR3
#elif defined(ISP_3DNR_V32)
#define CTRL_3DNR32
#endif // ISP_3DNR_V3

#if defined(ISP_AE)
#define CTRL_AE
#endif // ISP_AE

#if defined(ISP_AEV2)
#define CTRL_AE2
#endif // ISP_AEV2

#if defined(ISP_AF)
#define CTRL_AF
#endif // ISP_AF

#define CTRL_AHDR

#if defined(ISP_VSM)
#define CTRL_AVS
#endif // ISP_VSM

#define CTRL_AWB

#if defined(ISP_BLS) || defined(ISP_COMPAND)
#define CTRL_BLS
#endif // ISP_BLS

#if defined(ISP_CA)
#define CTRL_CA
#endif // ISP_CA

#define CTRL_CAC

#define CTRL_CNR

#if defined(ISP_CNR2)
#define CTRL_CNR2
#endif // ISP_CNR2

#define CTRL_CPROC

#if defined(ISP_CA)
#define CTRL_DCI
#endif // ISP_CA

#define CTRL_DG

#if defined(ISP_DEMOSAIC)
#define CTRL_DEMOSAIC
#endif // ISP_DEMOSAIC

#if defined(ISP_DEMOSAIC2)
#define CTRL_DEMOSAIC2
#endif // ISP_DEMOSAIC2

#define CTRL_DPCC

#define CTRL_DPF

#if defined(ISP_EE)
#define CTRL_EE
#endif // ISP_EE

#if defined(ISP_GC)
#define CTRL_GC
#endif // ISP_GC

#if defined(ISP_RGBGC)
#define CTRL_GC2
#endif // ISP_RGBGC

#if defined(ISP_GREENEQUILIBRATE)
#define CTRL_GE
#endif // ISP_GREENEQUILIBRATE

#if defined(ISP_HDR_STITCH) || defined(ISP_HDR_STITCH2)
#define CTRL_HDR1_2
#endif // ISP_HDR_STITCH

#if defined(ISP_HDR_STITCH2)
#define CTRL_HDR2
#endif // ISP_HDR_STITCH2

#if defined(ISP_HDR_STITCH2_V1)
#define CTRL_HDR2_LITE
#endif // ISP_HDR_STITCH2_V1


#if defined(ISP_IE)
#define CTRL_IE
#endif // ISP_IE

#if defined(ISP_3D_LUT)
#define CTRL_3D_LUT
#endif // ISP_3D_LUT

#define CTRL_LSC

#define CTRL_SI

#define CTRL_RGBIR

#define CTRL_WB

#if defined(ISP_WDR)
#define CTRL_WDR
#endif // ISP_WDR

#if defined(ISP_WDR_V2)
#define CTRL_WDR2
#endif // ISP_WDR_V2

#if defined(ISP_WDR_V3)
#define CTRL_WDR3
#endif // ISP_WDR_V3

#if defined(ISP_WDR_V4)
#define CTRL_WDR4
#endif // ISP_WDR_V4

#if defined(ISP_WDR_V5)
#define CTRL_WDR5
#endif // ISP_WDR_V5

#if defined(CTRL_FPGA) && !defined(HAL_ALTERA)
#define HAL_ALTERA
#endif // CTRL_FPGA

#if defined(CTRL_CMODEL)
#define HAL_CMODEL
#endif // CTRL_CMODEL

#if defined(CTRL_AE) && defined(CTRL_AE2)
#error "Conflict: AE & AE2 can't work together"
#endif

#if defined(CTRL_CMODEL) && defined(CTRL_FPGA)
#error "Conflict: CModel & FPGA can't work together"
#endif

#if defined(CTRL_DEMOSAIC) && defined(CTRL_DEMOSAIC2)
#error "Conflict: DeMosaic & DeMosaic 2 can't work together"
#endif

#if defined(CTRL_HDR) && defined(CTRL_HDR1_2)
#error "Conflict: HDR & HDR 1.2 can't work together"
#endif
