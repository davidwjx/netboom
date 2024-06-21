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
#ifndef _ISP_VERSION_H_
#define _ISP_VERSION_H_

#define ISP_EE_SUPPORT 1
#define ISP_2DNR_SUPPORT (1 << 1)
#define ISP_3DNR_SUPPORT (1 << 2)
#define ISP_WDR3_SUPPORT (1 << 3)
#define ISP_MIV2_SUPPORT (1 << 4)
#define ISP_AEV2_SUPPORT (1 << 5)
#define ISP_COMPAND_SUPPORT (1 << 6)
#define ISP_HDR_STITCH_SUPPORT (1 << 7)


/*****************************************************************************/
/**
 * @brief   HDR module version type
 */
/*****************************************************************************/
#define VSI_ISP_HDR_NOTSUPPORT  0   //!< Not support HDR
#define VSI_ISP_HDR_V10         1   //!< Version V10
#define VSI_ISP_HDR_V11         2   //!< Version V11
#define VSI_ISP_HDR_V12         3   //!< Version V12
#define VSI_ISP_HDR_V13         4   //!< Version V13
#define VSI_ISP_HDR_V20         5   //!< Version V20


/////////////////////////////////////////
//add Version defines
/////////////////////////////////////////

/* add Version defines(Mrv_all_regs.h)
ISP8000L_V2002、ISP8000_V2003、
ISP8000NANO_V1801、ISP8000NANO_V1802、
ISP8000_V1901、ISP8000L_V1901、ISP8000L_V1902、
ISP8000L_V1903、ISP8000L_V1905、ISP8000L_V2001、ISP8000L_V2006
*/
#define MRV_IMAGE_EFFECTS_VERSION       1
#define MRV_SUPER_IMPOSE_VERSION        1
#define MRV_BLACK_LEVEL_VERSION         1
#define MRV_AUTO_EXPOSURE_VERSION       1
#define MRV_AEV2_VERSION                1
#define MRV_OUTPUT_FORMATTER_VERSION    1
#define MRV_GAMMA_OUT_VERSION           1
#define MRV_FLASH_LIGHT_VERSION         1
#define MRV_SHUTTER_VERSION             1
#define MRV_MAINPATH_SCALER_VERSION     1
#define MRV_SELFPATH_SCALER_VERSION     1
#define MRV_MI_VERSION                  1
//#define MRV_JPE_VERSION                 1
#define MRV_SMIA_VERSION                1
#define MRV_MIPI_VERSION                1
#define MRV_AUTOFOCUS_VERSION           1
#define MRV_LSC_VERSION                 1
#define MRV_IS_VERSION                  1
#define MRV_HISTOGRAM_VERSION           1
#ifdef ISP_HIST64
#define MRV_HISTOGRAM64_VERSION         1
#endif
#define MRV_FILTER_VERSION              1
#define MRV_CAC_VERSION                 1
#define MRV_DPF_VERSION                 1
#define MRV_DPCC_VERSION                1
#define MRV_WDR_VERSION                 1
#define MRV_CSM_VERSION                 1
#define MRV_AWB_VERSION                 1
#define MRV_ELAWB_VERSION               1
#define MRV_GAMMA_IN_VERSION            1
#define MRV_SHUTTER_CTRL_VERSION        1
#define MRV_CT_VERSION                  1
#define MRV_COLOR_PROCESSING_VERSION    1
#define MRV_VSM_VERSION                 1
#define MRV_CNR_VERSION                 1

#define MRV_WDR2_VERSION                1
#define MRV_WDR3_VERSION                1
// #ifdef ISP_WDR_V5
// #define MRV_WDR5_VERSION                1
// #endif
#define MRV_CMPD_VERSION                1	/* compand */
#define MRV_TPG_VERSION                 1
//#define MRV_STITCHING_VERSION           1
#define MRV_EE_VERSION                  1
#define MRV_2DNR_VERSION                1

#ifdef ISP_3DNR
#define MRV_3DNR_VERSION                1
#endif

#ifdef ISP_GCMONO
#define MRV_GCMONO_VERSION              1
#endif
#ifdef ISP_RGBGC
#define MRV_RGBGAMMA_VERSION              1
#endif
#ifdef ISP_DEC
#define MRV_DEC_VERSION                 1
#endif
#ifdef ISP_DEMOSAIC2
#define MRV_DEMOSAIC_VERSION            1
#endif
#ifdef ISP_GREENEQUILIBRATE
#define MRV_GREENEQUILIBRATION_VERSION            1
#endif
#ifdef ISP_CA
#define MRV_COLOR_ADJUST_VERSION            1
#endif

#ifdef ISP_SP2
#define MRV_SP2_VERSION                 1
#endif
#ifdef ISP_HDR_STITCH_V2
#undef MRV_STITCHING_VERSION
#define MRV_STITCHING_VERSION           2    //1--V11, 2--V12

#elif defined(ISP_HDR_STITCH_V3)
#undef MRV_STITCHING_VERSION
#define MRV_STITCHING_VERSION           3    //1--V11, 2--V12 3--V13
#elif defined(ISP_HDR_STITCH)

#undef MRV_STITCHING_VERSION
#define MRV_STITCHING_VERSION           1    //1--V11, 2--V12 3--V13
#endif

#ifdef ISP_3DNR_V2
#define MRV_3DNR_VERSION                1
#endif

#ifdef ISP_WDR_V4
#define MRV_WDR4_VERSION                1
#endif

#ifdef ISP_3DNR_V3
#define MRV_TDNR3_VERSION                1
#endif

#ifdef ISP_RGBIR
#define MRV_RGBIR_VERSION                1
#endif

#ifdef ISP_HIST64

#define MRV_HISTOGRAM64_VERSION                1
#endif

#ifdef ISP_PDAF
#define MRV_PDAF_VERSION                1
#endif

#if defined(ISP_HDR_STITCH2)

#undef MRV_STITCHING2_VERSION
#define MRV_STITCHING2_VERSION 1 // 1--stitching V20, 2--stitching V21
#elif ISP_HDR_STITCH2_V1
#undef MRV_STITCHING2_VERSION
#define MRV_STITCHING2_VERSION 2 // 1--stitching V20, 2--stitching V21
#endif

#if defined(ISP_HDR_STITCH3)
#undef MRV_STITCHING3_VERSION
#define MRV_STITCHING3_VERSION 1 // 1--stitching V30, 2--stitching v3 lite (v31)
#elif ISP_HDR_STITCH3_V1
#undef MRV_STITCHING3_VERSION
#define MRV_STITCHING3_VERSION 2 // 1--stitching V30, 2--stitching v3 lite (v31)
#endif

#ifdef ISP_GTM
#define MRV_GTM_VERSION                1
#endif

#endif /* _ISP_VERSION_H_ */
