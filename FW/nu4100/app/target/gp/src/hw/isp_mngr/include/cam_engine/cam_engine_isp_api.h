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
 * @cond    cam_engine_isp
 *
 * @file    cam_engine_isp_api.h
 *
 * @brief
 *
 *   Interface description of the CamEngine ISP.
 *
 *****************************************************************************/
/**
 *
 * @defgroup cam_engine_isp_api CamEngine ISP API
 * @{
 *
 */

#ifndef __CAM_ENGINE_ISP_API_H__
#define __CAM_ENGINE_ISP_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

//FIXME
#include <cameric_drv/cameric_isp_drv_api.h>
#include <cameric_drv/cameric_isp_bls_drv_api.h>
#include <cameric_drv/cameric_isp_wdr_drv_api.h>
#include <cameric_drv/cameric_isp_wdr2_drv_api.h>
#include <cameric_drv/cameric_isp_wdr3_drv_api.h>
#include <cameric_drv/cameric_isp_wdr4_drv_api.h>
#include <cameric_drv/cameric_isp_wdr5_drv_api.h>

#include <cameric_drv/cameric_isp_compand_drv_api.h>
#include <cameric_drv/cameric_isp_tpg_drv_api.h>
#if defined(MRV_STITCHING2_VERSION)||defined(ISP_HDR_STITCH2)
#include <cameric_drv/cameric_isp_stitching2_drv_api.h>
#else
#include <cameric_drv/cameric_isp_stitching_drv_api.h>
#endif

#include <cameric_drv/cameric_isp_ee_drv_api.h>
#include <cameric_drv/cameric_isp_2dnr_drv_api.h>
#include <cameric_drv/cameric_isp_3dnr_drv_api.h>

#include <cameric_drv/cameric_isp_lsc_drv_api.h>
#include <cameric_drv/cameric_isp_dmsc2_drv_api.h>
#include <cameric_drv/cameric_isp_rgbgamma_drv_api.h>
#include <cameric_drv/cameric_isp_tdnr3_drv_api.h>
#include <cameric_drv/cameric_isp_color_adjust_drv_api.h>
#include <cameric_drv/cameric_isp_stitching_exp_drv_api.h>
#include <cameric_drv/cameric_isp_stitching_hist_drv_api.h>
#include <cameric_drv/cameric_isp_pdaf_drv_api.h>
#include <cameric_drv/cameric_isp_3dnr32_drv_api.h>
#include <cameric_drv/cameric_isp_rgbir_drv_api.h>
#include <cameric_drv/cameric_isp_cnr2_drv_api.h>


#define MAX_SENSOR_GAIN_NUM 3
#define MAX_EXPOSURE_TIME_NUM 3

/*****************************************************************************/
/**
 * @brief   This macro defines the number of used bins.
 *
 *****************************************************************************/
#ifdef ISP_HIST256
#define CAMENGINE_ISP_HIST_NUM_BINS           256  /**< number of bins */
#else
#define CAMENGINE_ISP_HIST_NUM_BINS           16  /**< number of bins */
#endif

/*****************************************************************************/
/**
 * @brief   This macro defines the number of used grid items ofr weightening
 *          measured pixels.
 *
 *****************************************************************************/
#define CAMENGINE_ISP_HIST_GRID_ITEMS         25  /**< number of grid sub windows */

#define CAMENGINE_AEV2_STATIC                 (4096 * 4)

/*****************************************************************************/
/**
 * @brief   Enumeration type to configure CamerIC ISP histogram measuring mode.
 *
 *****************************************************************************/
typedef enum CamEngineHistMode_e
{
    CAMENGINE_HIST_MODE_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    CAMENGINE_HIST_MODE_RGB_COMBINED  = 1,    /**< RGB combined histogram */
    CAMENGINE_HIST_MODE_R             = 2,    /**< R histogram */
    CAMENGINE_HIST_MODE_G             = 3,    /**< G histogram */
    CAMENGINE_HIST_MODE_B             = 4,    /**< B histogram */
    CAMENGINE_HIST_MODE_Y             = 5,    /**< luminance histogram */
    CAMENGINE_HIST_MODE_MAX,     				/**< upper border (only for an internal evaluation) */
} CamEngineHistMode_t;

/*****************************************************************************/
/**
 * @brief   This typedef specifies an array type to configure the grid weights
 *          of CamerIC ISP historgam module.
 *
 *****************************************************************************/
typedef uint8_t CamEngineHistWeights_t[CAMENGINE_ISP_HIST_GRID_ITEMS];

/*****************************************************************************/
/**
 * @brief   This typedef represents the histogram which is measured by the
 *          CamerIC ISP histogram module.
 *
 *****************************************************************************/
typedef uint32_t CamEngineHistBins_t[CAMENGINE_ISP_HIST_NUM_BINS];


typedef struct CamEngineAev2Static_s
{
    uint8_t                         Aev2Static[CAMENGINE_AEV2_STATIC];
} CamEngineAev2Static_t;


/******************************************************************************/
/**
 * @brief   Enumeration type to configure the horizontal clip mode
 *
 * @note    Defines the maximum red/blue pixel shift in horizontal direction
 *          At pixel positions, that require a larger displacement, the maximum
 *          shift value is used instead (vector clipping)
 *
 *****************************************************************************/
typedef enum CamEngineCacHorizontalClipMode_e
{
    CAM_ENGINE_CAC_H_CLIPMODE_INVALID  = 0,    /**< lower border (only for an internal evaluation) */
    CAM_ENGINE_CAC_H_CLIPMODE_FIX4     = 1,    /**< horizontal vector clipping +/-4 pixel displacement (default) */
    CAM_ENGINE_CAC_H_CLIPMODE_DYN5     = 2,    /**< horizontal vector clipping to +/-4 or +/-5 pixel displacement
                                                 depending on pixel position inside the bayer raster (dynamic
                                                 switching between +/-4 and +/-5) */
    CAM_ENGINE_CAC_H_CLIPMODE_MAX              /**< upper border (only for an internal evaluation) */
} CamEngineCacHorizontalClipMode_t;


/******************************************************************************/
/**
 * @brief   Enumeration type to configure the vertical clip mode
 *
 * @note    Defines the maximum red/blue pixel shift in vertical direction
 *
 *****************************************************************************/
typedef enum CamEngineCacVerticalClipMode_e
{
    CAM_ENGINE_CAC_V_CLIPMODE_INVALID  = 0,    /**< lower border (only for an internal evaluation) */
    CAM_ENGINE_CAC_V_CLIPMODE_FIX2     = 1,    /**< vertical vector clipping to +/-2 pixel */
    CAM_ENGINE_CAC_V_CLIPMODE_FIX3     = 2,    /**< vertical vector clipping to +/-3 pixel */
    CAM_ENGINE_CAC_V_CLIBMODE_DYN4     = 3,    /**< vertical vector clipping +/-3 or +/-4 pixel displacement
                                                 depending on pixel position inside the bayer raster (dynamic
                                                 switching between +/-3 and +/-4) */
    CAM_ENGINE_CAC_V_CLIPMODE_MAX              /**< upper border (only for an internal evaluation) */
} CamEngineCacVerticalClipMode_t;



/******************************************************************************/
/**
 * @brief   Structure to configure the chromatic aberration correction
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineCacConfig_s
{
    uint16_t                            width;          /**< width of the input image in pixel */
    uint16_t                            height;         /**< height of the input image in pixel */

    int16_t                             hCenterOffset;  /**< horizontal offset between image center and optical center of the input image in pixels */
    int16_t                             vCenterOffset;  /**< vertical offset between image center and optical center of the input image in pixels */

    CamEngineCacHorizontalClipMode_t    hClipMode;      /**< maximum red/blue pixel shift in horizontal direction */
    CamEngineCacVerticalClipMode_t      vClipMode;      /**< maximum red/blue pixel shift in vertical direction */

    float                            aBlue;          /**< parameters for radial shift calculation 9 bit twos complement with 4 fractional digits, valid range -16..15.9375 */
    float                            aRed;           /**< parameters for radial shift calculation 9 bit twos complement with 4 fractional digits, valid range -16..15.9375 */

    float                           bBlue;          /**< parameters for radial shift calculation 9 bit twos complement with 4 fractional digits, valid range -16..15.9375 */
    float                           bRed;           /**< parameters for radial shift calculation 9 bit twos complement with 4 fractional digits, valid range -16..15.9375 */

    float                           cBlue;          /**< parameters for radial shift calculation 9 bit twos complement with 4 fractional digits, valid range -16..15.9375 */
    float                          cRed;           /**< parameters for radial shift calculation 9 bit twos complement with 4 fractional digits, valid range -16..15.9375 */

    uint8_t                             Xns;            /**< horizontal normal shift parameter */
    uint8_t                             Xnf;            /**< horizontal scaling factor */

    uint8_t                             Yns;            /**< vertical normal shift parameter */
    uint8_t                             Ynf;            /**< vertical scaling factor */
} CamEngineCacConfig_t;


typedef struct CamEngineCnr2Config {
	bool ynr_enable;
	bool cnr_enable;
	float c_sigma_layer[SIGMA_LAYER_NUM];
	float c_sigma_merge[SIMGA_MERGE_NUM];
	float y_sigma_layer;
	uint8_t bit;
	uint8_t cnr_strength1;
	uint8_t cnr_strength2;
	uint8_t ynr_strength1;
	uint8_t ynr_strength2;
    uint8_t texture_mask_select;
} CamEngineCnr2Config_t;


typedef enum CamEngineWdrMode_e
{
    CAM_ENGINE_WDR_MODE_INVALID  = 0,
    CAM_ENGINE_WDR_MODE_FIX      = 1,
    CAM_ENGINE_WDR_MODE_ADAPTIVE = 2,
    CAM_ENGINE_WDR_MODE_MAX
} CamEngineWdrMode_t;



typedef struct CamEngineWdrConfig
{
    CamEngineWdrMode_t  mode;
    uint16_t            RgbFactor;
    uint16_t            RgbOffset;
    uint16_t            LumOffset;
    uint16_t            DminStrength;
    uint16_t            DminThresh;
} CamEngineWdrConfig_t;





/******************************************************************************/
/**
 * @brief   Structure to configure the gamma curve.
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef enum CamEngineGammaOutXScale_e
{
    CAM_ENGINE_GAMMAOUT_XSCALE_INVALID  = 0,    /**< lower border (only for an internal evaluation) */
    CAM_ENGINE_GAMMAOUT_XSCALE_LOG      = 1,    /**< logarithmic segmentation from 0 to 4095
                                                     (64,64,64,64,128,128,128,128,256,256,256,512,512,512,512,512) */
    CAM_ENGINE_GAMMAOUT_XSCALE_EQU      = 2,    /**< equidistant segmentation from 0 to 4095
                                                     (256, 256, ... ) */
    CAM_ENGINE_GAMMAOUT_XSCALE_MAX              /**< upper border (only for an internal evaluation) */
} CamEngineGammaOutXScale_t;


typedef struct CamEngineGammaOutCurve_s
{
#ifndef ISP_RGBGC

    CamEngineGammaOutXScale_t   xScale;
    uint16_t                    GammaY[CAMERIC_ISP_GAMMA_CURVE_SIZE];
#else
    uint32_t gammaRPx[CAMERIC_ISP_RGBGAMMA_PX_NUM];                         /**< rgb gamma px table */
    uint32_t gammaRDataX[CAMERIC_ISP_RGBGAMMA_DATA_X_NUM];                         /**< rgb gamma data x table */
    uint32_t gammaRDataY[CAMERIC_ISP_RGBGAMMA_DATA_Y_NUM];                         /**< rgb gamma data y table */
    uint32_t gammaGPx[CAMERIC_ISP_RGBGAMMA_PX_NUM];                         /**< rgb gamma px table */
    uint32_t gammaGDataX[CAMERIC_ISP_RGBGAMMA_DATA_X_NUM];                         /**< rgb gamma data x table */
    uint32_t gammaGDataY[CAMERIC_ISP_RGBGAMMA_DATA_Y_NUM];                         /**< rgb gamma data y table */
    uint32_t gammaBPx[CAMERIC_ISP_RGBGAMMA_PX_NUM];                         /**< rgb gamma px table */
    uint32_t gammaBDataX[CAMERIC_ISP_RGBGAMMA_DATA_X_NUM];                         /**< rgb gamma data x table */
    uint32_t gammaBDataY[CAMERIC_ISP_RGBGAMMA_DATA_Y_NUM];
#endif
} CamEngineGammaOutCurve_t;



typedef struct CamEngineGammaOutRGBCurve_s
{
    uint16_t GammaRX[CAMERIC_ISP_GAMMA_RGB_CURVE_SIZE];
    uint16_t GammaRY[CAMERIC_ISP_GAMMA_RGB_CURVE_SIZE];
    uint16_t GammaGX[CAMERIC_ISP_GAMMA_RGB_CURVE_SIZE];
    uint16_t GammaGY[CAMERIC_ISP_GAMMA_RGB_CURVE_SIZE];
	uint16_t GammaBX[CAMERIC_ISP_GAMMA_RGB_CURVE_SIZE];
    uint16_t GammaBY[CAMERIC_ISP_GAMMA_RGB_CURVE_SIZE];
} CamEngineGammaOutRGBCurve_t;




// typedef enum CamEngineRgbirOutBpt_e {
//     CAM_ENGINE_RGBIR_OUT_BPT_RGGB = 0,
//     CAM_ENGINE_RGBIR_OUT_BPT_GRBG,
//     CAM_ENGINE_RGBIR_OUT_BPT_GBRG,
//     CAM_ENGINE_RGBIR_OUT_BPT_BGGR,
// } CamEngineRgbirOutBpt_t;

// typedef enum CamEngineRgbirBpt_e{
//     CAM_ENGINE_RGBIR_BPT_BGGIR= 0,
//     CAM_ENGINE_RGBIR_BPT_GRIRG,
//     CAM_ENGINE_RGBIR_BPT_RGGIR,
//     CAM_ENGINE_RGBIR_BPT_GBIRG,
//     CAM_ENGINE_RGBIR_BPT_GIRRG,
//     CAM_ENGINE_RGBIR_BPT_IRGGB,
//     CAM_ENGINE_RGBIR_BPT_GIRBG,
//     CAM_ENGINE_RGBIR_BPT_IRGGR,
//     CAM_ENGINE_RGBIR_BPT_RGIRB,
//     CAM_ENGINE_RGBIR_BPT_GRBIR,
//     CAM_ENGINE_RGBIR_BPT_IRBRG,
//     CAM_ENGINE_RGBIR_BPT_BIRGR,
//     CAM_ENGINE_RGBIR_BPT_BGIRR,
//     CAM_ENGINE_RGBIR_BPT_GBRIR,
//     CAM_ENGINE_RGBIR_BPT_IRRBG,
//     CAM_ENGINE_RGBIR_BPT_RIRGB,
// } CamEngineRgbirBpt_t;

// typedef enum CamEngineRgbirFiltMode_e{
//     CAM_ENGINE_RGBIR_GREEN_FILT_STATIC= 0,
//     CAM_ENGINE_RGBIR_GREEN_FILT_DYNAMIC,
// } CamEngineRgbirFiltMode_t;
// /******************************************************************************/
// /**
//  * @brief   Structure to configure the  rgbir
//  *
//  * @note    This structure needs to be converted to driver structure
//  *
//  *****************************************************************************/
// typedef struct CamEngineRgbirConfig_s
// {
//     bool_t enable;
//     bool_t enable_ir_raw_out;
//     bool_t part1Enable;
//     bool_t part2Enable;
//     uint8_t dmscThreshold;
//     uint8_t stageSelect;
//     CamEngineRgbirOutBpt_t outRgbBpt;
//     CamEngineRgbirBpt_t rgbirBpt;
//     CamEngineRgbirFiltMode_t filtMode;
//     bool_t filtEnable;
//     double irSigmas;
//     uint16_t ir_threshold, l_threshold;
//     uint16_t bls_r, bls_g, bls_b, bls_ir;
//     double gain_r, gain_g, gain_b, gain_ir;
//     double cc_mtx[12];
//     uint16_t width, height;
// }CamEngineRgbirConfig_t;

/******************************************************************************/
/**
 * @brief   Structure to configure the  tdnr3 module
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineTdnr3Config_s {
    bool enable;
    bool enableInvGamma;
    bool enablePreGamma;
    bool enableMotionErosion;
    bool enableMotionCoverage;
    bool enableMotionDilation;
    bool enableTdnr;
    bool enable2Tdnr;
    uint8_t strength; // [0, 128]
    uint16_t noiseLvl;   // [0, 100]  -> [0, 65535]
    uint16_t motionSlope;   // [0, 100]  -> [0, 2047]
    uint16_t sadWeight;  // [0, 16]
    uint16_t updateFactor;  // [0, 1024]
    uint16_t motionUpdateFactor;  // [0, 1024]

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
    double modelA;
    double modelB;
} CamEngineTdnr3Config_t;


/******************************************************************************/
/**
 * @brief   Structure to configure the  pdaf module
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEnginePdafConfig_s
{
    bool_t enable;
	bool_t correct_en;
	CamerIcIspPdafSensorBayerPattern_t bayer_pattern;
	CamerIcIspPdafSensorType_t senor_type;
	uint32_t image_h, image_v;
	uint8_t pix_00_shift_hw[2];
	uint8_t correct_threshold;
	uint32_t wcorrect_rect[4];
	uint16_t pd_area[4];
	uint8_t num_per_area[2];
	uint8_t shift_mark[32];
	uint8_t shift_lr[2];
	uint32_t roi[4];
	uint8_t correct_r_channel, correct_l_channel;
	uint8_t correct_r_limit_base, correct_l_limit_base;
	uint8_t correct_r_change_down, correct_l_change_down;
	uint8_t correct_r_change_up, correct_l_change_up;

} CamEnginePdafConfig_t;


/******************************************************************************/
/**
 * @brief   Structure to configure the 3dnr3 module
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngine3Dnr32Config_s {
    bool enable;
    bool tnrEn;
    bool nlmEn;
    bool motionDilateEn;
    bool motionErodeEn;
    bool pregammaEn;
    uint32_t filterLen;      //[0,1024] 20
    uint32_t filterLen2;     //[0,1024] 10
    uint16_t thrMotionSlope; //[1,1023] 32
    uint16_t noiseLevel;     //[0,1024] 60
    uint8_t sadWeight;       //[0,16] 8
    uint32_t noiselevelA;    //[0,10000] 4
    uint32_t noiselevelB;    //[0,10000] 16
    uint16_t strength;       //[0,128] 120
    uint8_t rangeH;          //[3,7] 7
    uint8_t rangeV;          //[3,7] 7
    uint8_t dilateH;         //[3,7] 3
    uint8_t preweight;       //[0,32] 1
    float noisemodelTransStart;    //0
    uint16_t fixCurveStart;       //0
    uint8_t lumaCompEn;          
    uint8_t lumaPixelSlop;       //[0,255] 0
    uint8_t lumaPixelSlopShift;  //[0,15] 1
    uint8_t lumaPixelSlopMin;  //[0,128] 128
    uint8_t lumaPixelValShift;   //[0,255] 0

    uint8_t noiseAutoEst;
    float  noiseBoundFactor;
    float   nlUpdateFactor;
    float  noiseThreshFactor; //[1, 8] 2.0

    double noiseModuleA[NOISE_MODULE_NUM];
    double noiseModuleB[NOISE_MODULE_NUM];
} CamEngine3Dnr32Config_t;

/*****************************************************************************/
/**
 * @brief   This function sets the black-level.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Red                 red cells
 * @param   GreenR              green (red neighbors) cells
 * @param   GreenB              green (blue neighbors) cells
 * @param   Blue                blue cells
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineBlsSet
(
    CamEngineHandle_t   hCamEngine,
    const uint16_t      Red,
    const uint16_t      GreenR,
    const uint16_t      GreenB,
    const uint16_t      Blue
);



/*****************************************************************************/
/**
 * @brief   This function returns the black-level.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Red                 red cells
 * @param   GreenR              green (red neighbors) cells
 * @param   GreenB              green (blue neighbors) cells
 * @param   Blue                blue cells
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineBlsGet
(
    CamEngineHandle_t   hCamEngine,
    uint16_t            *Red,
    uint16_t            *GreenR,
    uint16_t            *GreenB,
    uint16_t            *Blue
);



/*****************************************************************************/
/**
 * @brief   This function sets the white balance gains.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Gains               white balance gains
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWbSetGains
(
    CamEngineHandle_t           hCamEngine,
    const CamEngineWbGains_t    *Gains
);



/*****************************************************************************/
/**
 * @brief   This function returns the white balance gains.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Gains               white balance gains
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWbGetGains
(
    CamEngineHandle_t   hCamEngine,
    CamEngineWbGains_t  *Gains
);



/*****************************************************************************/
/**
 * @brief   This function sets the cross talk matrix.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   CcMatrix            cross talk matrix
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWbSetCcMatrix
(
    CamEngineHandle_t           hCamEngine,
    const CamEngineCcMatrix_t   *CcMatrix
);



/*****************************************************************************/
/**
 * @brief   This function returns the cross talk matrix.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   CcMatrix            cross talk matrix
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWbGetCcMatrix
(
    CamEngineHandle_t   hCamEngine,
    CamEngineCcMatrix_t *CcMatrix
);



/*****************************************************************************/
/**
 * @brief   This function sets the cross talk offset.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   CcOffset            cross talk offset
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWbSetCcOffset
(
    CamEngineHandle_t           hCamEngine,
    const CamEngineCcOffset_t   *CcOffset
);



/*****************************************************************************/
/**
 * @brief   This function returns the cross talk offset.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   CcOffset            cross talk offset
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWbGetCcOffset
(
    CamEngineHandle_t   hCamEngine,
    CamEngineCcOffset_t *CcOffset
);

/*****************************************************************************/
/**
 * @brief   This function sets the measuring window for AWB.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   win                 pointer to a measuring window
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwbSetMeasuringWindow
(
    CamEngineHandle_t           hCamEngine,
    CamEngineWindow_t           *win
);

#ifndef ISP_DEMOSAIC2
/*****************************************************************************/
/**
 * @brief   This function sets the demosaicing.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Bypass              bypass mode
 * @param   Threshold           threshold
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDemosaicSet
(
    CamEngineHandle_t   hCamEngine,
    const bool_t        Bypass,
    const uint8_t       Threshold
);

/*****************************************************************************/
/**
 * @brief   This function returns the demosaicing.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   pBypass             bypass mode
 * @param   pThreshold          threshold
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDemosaicGet
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pBypass,
    uint8_t             *pThreshold
);
#else
/*****************************************************************************/
/**
 * @brief   This function sets the demosaic2.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Bypass              bypass mode
 * @param   Threshold           threshold
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDemosaic2Set
(
    CamEngineHandle_t   hCamEngine,
    const bool_t        Bypass,
    const uint8_t       Threshold
);
/*****************************************************************************/
/**
 * @brief   This function returns the demosaic2.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   pBypass             bypass mode
 * @param   pThreshold          threshold
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDemosaic2Get
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pBypass,
    uint8_t             *pThreshold
);

#endif


/*****************************************************************************/
/**
 * @brief   This function returns the status of the LSC module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineLscStatus
(
    CamEngineHandle_t       hCamEngine,
    bool_t                  *pRunning,
    CamEngineLscConfig_t    *pConfig
);



/*****************************************************************************/
/**
 * @brief   This function enables the LSC module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineLscEnable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the LSC module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineLscDisable
(
    CamEngineHandle_t hCamEngine
);


RESULT CamEngineLscConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineLscConfig_t *lscconfig
);


/*****************************************************************************/
/**
 * @brief   This function initializes the wide dynamic range module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineInitWdr
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function enables the wide dynamic range module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableWdr
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the wide dynamic range module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDisableWdr
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function loads a WDR curve.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   WdrCurve            WDR curve to load
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWdrSetCurve
(
    CamEngineHandle_t hCamEngine,
    const CamEngineWdrCurve_t *WdrCurve
);



/*****************************************************************************/
/**
 * @brief   This function returns the status of the gamma correction module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaStatus
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pRunning
);



/*****************************************************************************/
/**
 * @brief   This function enables the gamma correction module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaEnable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the gamma correction module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaDisable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function loads a correction curve into gamma correction
 *          module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   WdrCurve            WDR curve to load
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaSetCurve
(
    CamEngineHandle_t           hCamEngine,
    CamEngineGammaOutCurve_t    *GammaCurve
);



/*****************************************************************************/
/**
 * @brief   This function returns the status of the Cac module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineCacStatus
(
    CamEngineHandle_t       hCamEngine,
    bool_t                  *pRunning,
    CamEngineCacConfig_t    *pConfig
);



/*****************************************************************************/
/**
 * @brief   This function enables the CAC module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineCacEnable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function enables the CAC module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineCacDisable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function returns the status of the filter module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineFilterStatus
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pRunning,
    uint8_t             *pDenoiseLevel,
    uint8_t             *pSharpenLevel
);



/*****************************************************************************/
/**
 * @brief   This function enables the filter module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineFilterEnable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the filter module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineFilterDisable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function sets the levels of the filter module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineFilterSetLevel
(
    CamEngineHandle_t           hCamEngine,
    const uint8_t               DenoiseLevel,
    const uint8_t               SharpenLevel,
    const uint8_t               chrVMode,
    const uint8_t               chrHMode
);



/*****************************************************************************/
/**
 * @brief   This function enables the color noise reduction module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_WRONG_CONFIG    image effects isn't configured
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineCnrEnable
(
    CamEngineHandle_t   hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the color noise reduction module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineCnrDisable
(
    CamEngineHandle_t   hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function returns the color noise reduction status.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 * @param   pThreshold1         Threshold Color Channel 1
 * @param   pThreshold2         Threshold Color Channel 2
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineCnrStatus
(
    CamEngineHandle_t       hCamEngine,
    bool_t                  *pRunning,
    uint32_t                *pThreshold1,
    uint32_t                *pThreshold2
);



/*****************************************************************************/
/**
 * @brief   This function sets the thresholds of the color noise reduction
 *          module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   Threshold1          Threshold Color Channel 1
 * @param   Threshold2          Threshold Color Channel 2
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineCnrSetThresholds
(
    CamEngineHandle_t   hCamEngine,
    const uint32_t      Threshold1,
    const uint32_t      Threshold2
);


RESULT CamEngineCnr2EnableY
(
    CamEngineHandle_t   hCamEngine
);
RESULT CamEngineCnr2DisableY
(
    CamEngineHandle_t   hCamEngine
);
RESULT CamEngineCnr2EnableC
(
    CamEngineHandle_t   hCamEngine
);
RESULT CamEngineCnr2DisableC
(
    CamEngineHandle_t   hCamEngine
);

RESULT CamEngineCnr2Status
(
    CamEngineHandle_t hCamEngine,
    CamEngineCnr2Config_t *pConfig
);

RESULT CamEngineCnr2Configure
(
    CamEngineHandle_t   hCamEngine,
    CamEngineCnr2Config_t *pConfig
);


/*****************************************************************************/
/**
 * @brief   This function initializes the compand module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineInitCmpd
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function set the compand module expand curve.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   xData          pointer to the xdata
 * @param   yData          pointer to the ydata
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineSetCmpdExpandCurve
(
    CamEngineHandle_t hCamEngine,
    uint32_t           *xData,
    uint32_t            *yData
);

/*****************************************************************************/
/**
 * @brief   This function enables the compand module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableCmpd
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableCmpdBls
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableCmpdExpand
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableCmpdCompress
(
    CamEngineHandle_t hCamEngine
);


/*****************************************************************************/
/**
 * @brief   This function disables the compand module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDisableCmpd
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableCmpdBls
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableCmpdExpand
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableCmpdCompress
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the compand module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineCmpdConfig
(
    CamEngineHandle_t   hCamEngine,
    bool_t              blsEnable,
    bool_t              compressEnable,
    bool_t              expandEnable,
    int                *pBlsPara
);



/*****************************************************************************/
/**
 * @brief   This function returns the status of the Ee module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEeStatus
(
    CamEngineHandle_t       hCamEngine,
    bool_t                  *pRunning,
    CamEngineEeConfig_t     *pConfig
);



/*****************************************************************************/
/**
 * @brief   This function enables the edge enhance module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEeEnable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the edge enhance module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEeDisable
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function set strength and Gain of the edge enhance module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEeSetConfig
(
    CamEngineHandle_t       hCamEngine,
    CamEngineEeConfig_t     *pConfig
);



//===========================================================================
// Temporary API, not used for tuning tool
//===========================================================================

RESULT CamEngineInitWdr2
(
    CamEngineHandle_t hCamEngine
);
RESULT CamEngineInitWdr5
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineReleaseWdr5
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableWdr2
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableWdr2
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineWdr2SetStrength
(
    CamEngineHandle_t hCamEngine,
    float             WdrStrength
);

RESULT CamEngineEnableWdr3
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableWdr3
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineWdr3SetStrength
(
    CamEngineHandle_t   hCamEngine,
    uint8_t             Wdr3Strength,
    uint8_t             Wdr3MaxGain,
    uint8_t             Wdr3GlobalStrength
);

RESULT CamEngineEnableWdr4
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableWdr4
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableWdr5
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableWdr5
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineWdr5EnableGet
(
   CamEngineHandle_t hCamEngine,
   bool_t            *pisRunning
);

RESULT CamEngineIspWdr4StoreHdrRatio
(
    CamEngineHandle_t hCamEngine,
	uint8_t  ratioLSVS,
    uint8_t   ratioLS
);

RESULT CamEngineCmpdSetTbl
(
    CamEngineHandle_t   hCamEngine,
    const int          *pExpandTbl,
    const int          *pCompressTbl
);

RESULT CamEngineEnableTpg
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableTpg
(
    CamEngineHandle_t hCamEngine
);

typedef struct CamEngineTpgConfig_s
{
    CamerIcISPTpgImageType_t        imageType;
    CamerIcIspTpgBayerPattern_t     bayerPattern;
    CamerIcIspTpgColorDepth_t       colorDepth;
    CamerIcIspTpgResolution_t       resolution;

    uint16_t                        pixleGap;       // image 0 (3x3 block)
    uint16_t                        lineGap;        // image 0 (3x3 block)
    uint16_t                        gapStandard;    // image 1, 2, 3.
    uint32_t                        randomSeed;     // image 4
    uint32_t                        frame_num;

    CamerIcIspTpgUserDefineMode_t   userMode;

} CamEngineTpgConfig_t;

RESULT CamEngineTpgConfig
(
    CamEngineHandle_t       hCamEngine,
    CamEngineTpgConfig_t    *pTpgConfig
);

RESULT CamEngineInitStitching
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableStitching
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableStitching
(
    CamEngineHandle_t hCamEngine
);

#if defined(MRV_STITCHING2_VERSION)||defined(ISP_HDR_STITCH2)

/******************************************************************************
 * CamEngineStitchingConfig()
 *****************************************************************************/

typedef struct CamEngineStitchingConfig_s
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
    // bool_t                            motion_enable[MOTION_NUM];
    // uint32_t                          motion_weight[MOTION_NUM];
    // uint32_t                          motion_weight_shorter;               
    // uint32_t                          motion_sat_threshold;               
    // uint32_t                          motion_weight_update_threshold;    
    // uint32_t                          motion_lower_threshold_ls[MOTION_THRESHOLD_NUM];
    // uint32_t                          motion_upper_threshold_ls[MOTION_THRESHOLD_NUM];
    // uint32_t                          motion_lower_threshold_lsvs[MOTION_THRESHOLD_NUM];
    // uint32_t                          motion_upper_threshold_lsvs[MOTION_THRESHOLD_NUM];       
    // uint32_t                          dark_lower_threshold_ls[DARK_THRESHOLD_NUM];
    // uint32_t                          dark_upper_threshold_ls[DARK_THRESHOLD_NUM];
    // uint32_t                          dark_lower_threshold_lsvs[DARK_THRESHOLD_NUM];
    // uint32_t                          dark_upper_threshold_lsvs[DARK_THRESHOLD_NUM];
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

} CamEngineStitchingConfig_t;

typedef struct CamEngineStitching2DpfConfig_s
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
} CamEngineStitching2DpfConfig_t;

typedef struct CamEngineStitching2DeghostConfig_s
{
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
} CamEngineStitching2DeghostConfig_t;

RESULT CamEngineStitchingConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineStitchingConfig_t *pStitchingCfg
);

RESULT CamEngineStitching2DpfConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineStitching2DpfConfig_t *pStitching2DpfCfg
);

RESULT CamEngineStitching2DeghostConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineStitching2DeghostConfig_t *pStitching2DeghostCfg
);

RESULT CamEngineStitching2GetConfig(
    CamEngineHandle_t hCamEngine,
    CamEngineStitchingConfig_t *pStitching2Cfg
);

RESULT CamEngineStitching2DpfGetConfig(
    CamEngineHandle_t hCamEngine,
    CamEngineStitching2DpfConfig_t *pStitching2DpfCfg
);

RESULT CamEngineStitching2DeghostGetConfig(
    CamEngineHandle_t hCamEngine,
    CamEngineStitching2DeghostConfig_t *pStitching2DeghostCfg
);

#elif defined(MRV_STITCHING_VERSION) ||defined(ISP_HDR_STITCH) || defined(ISP_HDR_STITCH_V2) || defined(ISP_HDR_STITCH_V3)

RESULT CamEngineStitchingConfig
(
    CamEngineHandle_t       hCamEngine,
    int                     StitchingPara

);

#endif // defined(MRV_STITCHING2_VERSION)

RESULT CamEngineSetStitchingRdWrStr
(
    CamEngineHandle_t hCamEngine,
    int               rdStr
);


RESULT CamEngineBypassMcm
(
    CamEngineHandle_t hCamEngine,
    bool               enable
);

/******************************************************************************
 * CamEngineSetStitchingColorWeight()
 *****************************************************************************/
RESULT CamEngineSetStitchingColorWeight
(
    CamEngineHandle_t hCamEngine,
    const uint8_t                weight0,
    const uint8_t                weight1,
    const uint8_t                weight2
);

typedef struct CamEngineStitchingProperties_s
{
    bool regsInformEnable;
    bool awbGainEnable;
    uint16_t width;
    uint16_t height;
    uint8_t longBitDepth;
    uint8_t shortBitDepth;
    uint8_t vsBitDepth;
    uint8_t lsBitDepth;
    uint16_t longExposureTime;
    uint16_t shortExposureTime;
    uint16_t vsExposureTime;
    uint16_t dummyShortHBlank;
    uint16_t outHBlank;
    uint16_t dummyVsHBlank;
    uint16_t outVBlank;
    CamerIcIspStitchingPolarityType_t hSyncPol;
    CamerIcIspStitchingPolarityType_t vSyncPol;
    CamerIcIspStitchingBayerPattern_t bayerPattern;
    CamerIcIspStitchingBypassSelectMode_t bypassSelectMode;
    CamerIcIspStitchingCombinationMode_t combinationMode;
    CamerIcIspStitchingBaseFrameSelection_t baseFrameSelection;
    CamerIcIspStitchingHdrMode_t hdrMode;
} CamEngineStitchingProperties_t;

RESULT CamEngineSetStitchingProperties
(
    CamEngineHandle_t hCamEngine,
    const CamEngineStitchingProperties_t *pProperties
);

RESULT CamEngineStitchingExpConfig
(
    CamEngineHandle_t       hCamEngine,
    int                     StitchingPara,
    float                   SensorGain[MAX_SENSOR_GAIN_NUM],
    float                   ExposureTime[MAX_EXPOSURE_TIME_NUM],
    float* hdr_ratio
);

RESULT CamEngineStitchingShortExpSet
(
    CamEngineHandle_t hCamEngine,
    bool_t           ShortExpSet
);

RESULT CamEngineStitchingExtBitGet
(
    CamEngineHandle_t hCamEngine,
    uint8_t           *ext_bit
);
RESULT CamEngineInitEe
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnableEe
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisableEe
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEeConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineEeConfig_t *EePara
);

RESULT CamEngineInit2Dnr
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnable2Dnr
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisable2Dnr
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineEnable3Dnr
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDisable3Dnr
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngine3DnrUpdate
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngine3DnrCompute
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngine3DnrSetStrength
(
       CamEngineHandle_t hCamEngine,
       uint32_t denoiseStrength
);

typedef struct CamEngine3dnrCompress_s {
    uint8_t weightUpY[WEIGHT_UP_Y_NUM];
    uint8_t weightDown[WEIGHT_DOWN_NUM];
    uint8_t weightUpX[WEIGHT_UP_X_NUM];
} CamEngine3DnrCompress_t;

RESULT CamEngine3DnrSetCompress
(
    CamEngineHandle_t hCamEngine,
    CamEngine3DnrCompress_t *compress
);

/*****************************************************************************/
/**
 * @brief   This function eanble the afm module
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableAfm
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function disable the afm module
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDisableAfm
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function config the afm  threshold
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   AfmThresHold        afm threshold
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfmConfig
(
    CamEngineHandle_t hCamEngine,
    uint32_t AfmThresHold
);

/*****************************************************************************/
/**
 * @brief   This function config the afm windows
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   AfmWinId            windows id  range 0-2
 * @param   pWin                windows
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfmSetWindows
(
    CamEngineHandle_t hCamEngine,
    uint32_t AfmWinId,
    CamEngineWindow_t *pWin
);

RESULT CamEngineDemosaicConfig
(
    CamEngineHandle_t hCamEngine,
    uint32_t DemosaicMode,
    uint32_t DemosaicThreshold
);

typedef struct CamEngineDemosaicConfig_s {
    bool enable;
    uint8_t demosaic_thr;
    uint8_t denoise_stren;
    CamericIspDmscIntp_t intp;
    CamericIspDmscDemoire_t demoire;
    CamericIspDmscSharpen_t sharpen;
    CamericIspDmscGFilter_t gFilter;
    CamericIspDmscSharpenLine_t sharpenLine;
    CamericIspDmscSkin_t skin;
    CamericIspDmscDepurple_t depurple;
} CamEngineDemosaicConfig_t;

RESULT CamEngineDmsc2Configure
(
    CamEngineHandle_t hCamEngine,
    CamEngineDemosaicConfig_t *pConfig
);

RESULT CamEngineDmsc2GetConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineDemosaicConfig_t *pConfig
);

RESULT CamEngineDmsc2Enable
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineDmsc2Disable
(
    CamEngineHandle_t hCamEngine
);

typedef struct CamEngineRgbGammaConfig_s
{
    bool                  enabled;                                              /**< rgb gamma enabled */
    uint32_t              rgbgammaRPx[CAMERIC_ISP_RGBGAMMA_PX_NUM];             /**< rgb gamma px table */
    uint32_t              rgbgammaRDataX[CAMERIC_ISP_RGBGAMMA_DATA_X_NUM];      /**< rgb gamma data x table */
    uint32_t              rgbgammaRDataY[CAMERIC_ISP_RGBGAMMA_DATA_Y_NUM];      /**< rgb gamma data y table */

    uint32_t              rgbgammaGPx[CAMERIC_ISP_RGBGAMMA_PX_NUM];             /**< rgb gamma px table */
    uint32_t              rgbgammaGDataX[CAMERIC_ISP_RGBGAMMA_DATA_X_NUM];      /**< rgb gamma data x table */
    uint32_t              rgbgammaGDataY[CAMERIC_ISP_RGBGAMMA_DATA_Y_NUM];      /**< rgb gamma data y table */

    uint32_t              rgbgammaBPx[CAMERIC_ISP_RGBGAMMA_PX_NUM];             /**< rgb gamma px table */
    uint32_t              rgbgammaBDataX[CAMERIC_ISP_RGBGAMMA_DATA_X_NUM];      /**< rgb gamma data x table */
    uint32_t              rgbgammaBDataY[CAMERIC_ISP_RGBGAMMA_DATA_Y_NUM];      /**< rgb gamma data y table */
} CamEngineRgbGammaConfig_t;

RESULT CamEngineRgbGammaConfigure
(
    CamEngineHandle_t hCamEngine,
    CamEngineRgbGammaConfig_t *pConfig
);

RESULT CamEngineRgbGammaGetConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineRgbGammaConfig_t *pConfig
);

/******************************************************************************
 * CamEngineRgbGammaEnable()
 *****************************************************************************/
RESULT CamEngineRgbGammaEnable
(
    CamEngineHandle_t hCamEngine
);

/******************************************************************************
 * CamEngineRgbGammaDisable()
 *****************************************************************************/
RESULT CamEngineRgbGammaDisable
(
    CamEngineHandle_t hCamEngine
);

typedef struct CamEngineColorConfig_s
{
    bool enable;
    unsigned char caMode;
    unsigned short CaLutXTbl[CA_CURVE_DATA_TABLE_LEN];
    unsigned short CaLutLumaTbl[CA_CURVE_DATA_TABLE_LEN];
    unsigned short CaLutChromaTbl[CA_CURVE_DATA_TABLE_LEN];
    unsigned short CaLutShiftTbl[CA_CURVE_DATA_TABLE_LEN];
} CamEngineColorConfig_t;

RESULT CamEngineColorAdjustConfigure
(
    CamEngineHandle_t hCamEngine,
    CamEngineColorConfig_t *pConfig
);

RESULT CamEngineColorAdjustGetConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineColorConfig_t *pConfig
);

RESULT CamEngineColorAdjustEnable
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineColorAdjustDisable
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function enable the rgbir submodule
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableRgbir
(
    CamEngineHandle_t hCamEngine,
    bool bEnable
) ;

/*****************************************************************************/
/**
 * @brief   This function get rgbir enable status
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineRgbirEnableGet
(
    CamEngineHandle_t hCamEngine,
    bool* bEnable
);

/*****************************************************************************/
/**
 * @brief   This function enable the ir image of rgbir  output.
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableRgbirIrRawOut(
    CamEngineHandle_t hCamEngine,
    bool bEnable
) ;

/*****************************************************************************/
/**
 * @brief   This function config the  rgbir.
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineConfigRgbir
(
    CamEngineHandle_t hCamEngine,
    CamEngineRgbirConfig_t* pRgbirCfg
);

/*****************************************************************************/
/**
 * @brief   This function get the rgbir config.
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineRgbirGetConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineRgbirConfig_t* pRgbirCfg
);

/*****************************************************************************/
/**
 * @brief   This function enable the rccc of rgbir
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableRgbirRccc
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function disable the rccc of rgbir
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDisableRgbirRccc
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function config the dpcc of rgbir
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   pRgbirCfg           pointer to the rgbir configuration

 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineCfgRgbirDpcc
(
    CamEngineHandle_t hCamEngine,
    CamEngineRgbirConfig_t* pRgbirCfg

);

/*****************************************************************************/
/**
 * @brief   This function enable the tdnr3
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableTdnr3
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function disable the tdnr3
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDisableTdnr3
(
    CamEngineHandle_t hCamEngine
);


#ifndef  ISP_3DNR_V32
/*****************************************************************************/
/**
 * @brief   This function config the tdnr3 invgamma pregamma curve
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   modelA              noise model parameters, calibrated according to gain
 * @param   modelB              noise model parameters, calibrated according to gain
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineUpdateTdnr3Curve
(
    CamEngineHandle_t hCamEngine,
     double modelA,
     double modelB
);

RESULT CamEngineTdnr3GetConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngineTdnr3Config_t *pConfig
);

RESULT CamEngineTdnr3SetConfig
(
    CamEngineHandle_t hCamEngine,
    const CamEngineTdnr3Config_t *pConfig
);

#else
/*****************************************************************************/
/**
 * @brief   This function config the tdnr3 fe be curve
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   modelA              noise model parameters, calibrated according to gain
 * @param   modelB              noise model parameters, calibrated according to gain
 * @param   tranStart
 * @param   fixCurveStart
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineUpdateTdnr3Curve(
    CamEngineHandle_t hCamEngine,
    double *noiseModelA,
    double *noiseModelB,
    double transStart,
    uint8_t fixCurveStart
);

RESULT CamEngineTdnr3GetConfig
(
    CamEngineHandle_t hCamEngine,
    CamEngine3Dnr32Config_t *pConfig
);

RESULT CamEngineTdnr3SetConfig
(
    CamEngineHandle_t hCamEngine,
    const CamEngine3Dnr32Config_t *pConfig
);

#endif


/*****************************************************************************/
/**
 * @brief   This function config the tdnr3 strength
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   strength            strength parameter
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineSetTdnr3Strength
(
    CamEngineHandle_t hCamEngine,
    unsigned int strength
);

/*****************************************************************************/
/**
 * @brief   This function config the  2dnr of tdnr3 coeff curve
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   pCoeff              pinter to the coneff curve
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineCfgTdnr3Nlm
(
    CamEngineHandle_t hCamEngine,
    unsigned int *pCoeff
);

RESULT CamEngineEnableDigitalGain(CamEngineHandle_t hCamEngine);

RESULT CamEngineDisableDigitalGain(CamEngineHandle_t hCamEngine);

RESULT CamEngineSetupDigitalGain(CamEngineHandle_t hCamEngine,
                                    const uint16_t rGain,
                                    const uint16_t grGain,
                                    const uint16_t gbGain,
                                    const uint16_t bGain);

RESULT CamEngineEnableGreenEquilibration(CamEngineHandle_t hCamEngine);

RESULT CamEngineDisableGreenEquilibration(CamEngineHandle_t hCamEngine);

RESULT CamEngineSetupGreenEquilibration(CamEngineHandle_t hCamEngine, uint16_t threshold, uint16_t dummyLine);



/*****************************************************************************/
/**
 * @brief	This function enables the 3DNRv3 module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 * @param	pCoeff				pinter to the coneff curve
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngine3DNRv3Enable
(
	CamEngineHandle_t hCamEngine,
	const bool_t	  noiseCurveEnable

);



/*****************************************************************************/
/**
 * @brief	This function disables the 3DNRv3 module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngine3DNRv3Disable
(
	CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief	This function enables the green equilibrate module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 * @retval	RET_WRONG_CONFIG	image effects isn't configured
 * @retval	RET_NOTAVAILABLE	module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineGeEnable
(
	CamEngineHandle_t	hCamEngine
);



/*****************************************************************************/
/**
 * @brief	This function disables the green equilibrate module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 * @retval	RET_NOTAVAILABLE	module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineGeDisable
(
	CamEngineHandle_t	hCamEngine
);



/*****************************************************************************/
/**
 * @brief	This function sets the threshold of the green equilibrate
 *			module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 * @param	threshold			threshold
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 * @retval	RET_NOTAVAILABLE	module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineGeSetThreshold
(
	CamEngineHandle_t	hCamEngine,
	const uint16_t		threshold
);



/*****************************************************************************/
/**
 * @brief	This function returns the green equilibrate status.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 * @param	pRunning			BOOL_TRUE: running, BOOL_FALSE: stopped
 * @param	pThreshold			Threshold
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 * @retval	RET_NOTAVAILABLE	module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineGeStatus
(
	CamEngineHandle_t		hCamEngine,
	bool_t					*pRunning,
	uint16_t				*pThreshold
);



/*****************************************************************************/
/**
 * @brief	This function enables the RGB gamma correction module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaRGBEnable
(
	CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief	This function disables the RGB gamma correction module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaRGBDisable
(
	CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief	This function loads a correction curve into RGB gamma correction
 *			module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 * @param	GammaCurve			Gamma curve to load
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaRGBSetCurve
(
	CamEngineHandle_t				hCamEngine,
	CamEngineGammaOutRGBCurve_t 	gammaCurve
);



/*****************************************************************************/
/**
 * @brief	This function returns the status of the RGB gamma correction module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGammaRGBStatus
(
	CamEngineHandle_t	hCamEngine,
	bool_t				*pRunning
);



/*****************************************************************************/
/**
 * @brief   This function enables the digital gain module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_WRONG_CONFIG    image effects isn't configured
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineDgEnable
(
    CamEngineHandle_t   hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function disables the digital gain module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineDgDisable
(
    CamEngineHandle_t   hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function returns the digital gain status.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 * @param   pGain         		digital gain value
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineDgStatus
(
    CamEngineHandle_t       hCamEngine,
    bool_t                  *pRunning,
    float                	*pGain
);



/*****************************************************************************/
/**
 * @brief   This function sets the gain value of the digital gain module.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 * @param   gain          		digital gain value

 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NOTAVAILABLE    module not available by driver or hardware
 *
 *****************************************************************************/
RESULT CamEngineDgSetGain
(
    CamEngineHandle_t   hCamEngine,
    const float      	gain
);

/*****************************************************************************/
/**
 * @brief	This function enables the PDAF module.
 *
 * @param	hCamEngine			handle to the CamEngine instance
 *
 * @return	Return the result of the function call.
 * @retval	RET_SUCCESS 		function succeed
 * @retval	RET_WRONG_HANDLE	invalid instance handle
 *
 *****************************************************************************/

RESULT CamEnginePdafSetConfig
(
	CamEngineHandle_t hCamEngine,
    CamEnginePdafConfig_t *pConfig
);

/*****************************************************************************/
/**
 * @brief   This function enable the lut3d module
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineEnableLut3d
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief   This function disable the lut3d module
 * stats.
 *
 * @param   hCamEngine          handle to the CamEngine instance
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineDisableLut3d
(
    CamEngineHandle_t hCamEngine
);

RESULT CamEngineLut3dStatus
(
    CamEngineHandle_t hCamEngine,
    bool_t            *pRunning
);

/******************************************************************************
 *
 *          CamEngineFlexaShmInit
 *
 * @brief   Engine Flexa Share Memory Init
 *
 *****************************************************************************/
RESULT CamEngineFlexaShmCfg
(
    CamEngineHandle_t hCamEngine,
    CamEngineFlexaConfig_t *config
);

RESULT CamEngineLoadIsp3ALibrary
(
    CamEngineHandle_t hCamEngine
);

/******************************************************************************
 * CamEngineHistGet()
 *****************************************************************************/
RESULT CamEngineHistGet
(
    CamEngineHandle_t hCamEngine,
    CamEngineHistBins_t *pHistogram
);

/******************************************************************************
 * CamEngineHistSetMeasuringMode()
 *****************************************************************************/
RESULT CamEngineHistSetMeasuringMode
(
    CamEngineHandle_t hCamEngine,
    const CamEngineHistMode_t mode
);

/******************************************************************************
 * CamEngineHistSetMeasuringWindow()
 *****************************************************************************/
RESULT CamEngineHistSetMeasuringWindow
(
    CamEngineHandle_t hCamEngine,
    CamEngineWindow_t *hWindow
);

/******************************************************************************
 * CamEngineHistSetGridWeights()
 *****************************************************************************/
RESULT CamEngineHistSetGridWeights
(
    CamEngineHandle_t hCamEngine,
    const CamEngineHistWeights_t  weights
);

/******************************************************************************
 * CamEngineAev2StaticGet()
 *****************************************************************************/
RESULT CamEngineAev2StaticGet
(
    CamEngineHandle_t hCamEngine,
    CamEngineAev2Static_t *pAev2Static
);

/******************************************************************************
 * CamEngineAev2SetSizeRatio()
 *****************************************************************************/
RESULT CamEngineAev2SetSizeRatio
(
    CamEngineHandle_t hCamEngine,
    float ratio
);

/******************************************************************************
 * CamEngineAev2SetWeight()
 *****************************************************************************/
RESULT CamEngineAev2SetWeight
(
    CamEngineHandle_t hCamEngine,
    const uint8_t      rWeight,
    const uint8_t      grWeight,
    const uint8_t      gbWeight,
    const uint8_t      bWeight
);

/******************************************************************************
 * CamEngineAev2SetMeasuringWindow()
 *****************************************************************************/
RESULT CamEngineAev2SetMeasuringWindow
(
    CamEngineHandle_t hCamEngine,
    CamEngineWindow_t *hWindow
);

/******************************************************************************
 * CamEngineAev2InputSel()
 *****************************************************************************/
RESULT CamEngineAev2InputSel
(
    CamEngineHandle_t hCamEngine,
    CamEngineExpV2Sel_t inputSelect
);
 
#ifdef __cplusplus
}
#endif


/* @} cam_engine_isp_api */


#endif /* __CAM_ENGINE_ISP_API_H__ */

