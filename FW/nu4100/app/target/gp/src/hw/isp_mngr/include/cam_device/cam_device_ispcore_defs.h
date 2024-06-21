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

#ifndef __CAMERA_DEVICE_ISPCORE_DEFS_H__
#define __CAMERA_DEVICE_ISPCORE_DEFS_H__

#include "types.h"

/*****************************************************************************/
/**
 * @brief Lock types for the auto algorithms. Can be OR combined.
 *
 *****************************************************************************/
typedef enum CamEngineLockType_e {
    CAM_ENGINE_LOCK_NO      = 0x00,
    CAM_ENGINE_LOCK_AF      = 0x01,
    CAM_ENGINE_LOCK_AEC     = 0x02,
    CAM_ENGINE_LOCK_AWB     = 0x04,

    CAM_ENGINE_LOCK_ALL     = (CAM_ENGINE_LOCK_AF | CAM_ENGINE_LOCK_AEC | CAM_ENGINE_LOCK_AWB)
} CamEngineLockType_t;

/*****************************************************************************/
/**
 * @brief Flicker period types for the AEC algorithm.
 *
 *****************************************************************************/
typedef enum CamEngineFlickerPeriod_e
{
    CAM_ENGINE_FLICKER_OFF   = 0x00,
    CAM_ENGINE_FLICKER_100HZ = 0x01,
    CAM_ENGINE_FLICKER_120HZ = 0x02
} CamEngineFlickerPeriod_t;


/**
 * @brief   Enumeration type to input data format select.
 *
 */
typedef enum CamerIcIspEeInputDataFormatSelect_e
{
    CAMERIC_ISP_EE_INPUT_DATA_FORMAT_SELECT_INVALID   = 0,
    CAMERIC_ISP_EE_INPUT_DATA_FORMAT_SELECT_RGB       = 1,
    CAMERIC_ISP_EE_INPUT_DATA_FORMAT_SELECT_YUV       = 2,
    CAMERIC_ISP_EE_INPUT_DATA_FORMAT_SELECT_MAX
} CamerIcIspEeInputDataFormatSelect_t;

/*****************************************************************************/
/**
 * @brief   This macro defines the size of lens shading grad table in 16 Bit
 * 			words.
 *
 *****************************************************************************/
#define CAEMRIC_GRAD_TBL_SIZE        8


/*****************************************************************************/
/**
 * @brief   This macro defines the size of lens shading data table in 16 Bit
 * 			words.
 *
 *****************************************************************************/
#define CAMERIC_DATA_TBL_SIZE      289


/******************************************************************************/
/**
 * @brief   Structure to configure the lense shade correction
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineLscConfig_s
{
    struct
    {
        uint16_t LscXGradTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< multiplication factors of x direction  */
        uint16_t LscYGradTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< multiplication factors of y direction  */
        uint16_t LscXSizeTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< sector sizes of x direction            */
        uint16_t LscYSizeTbl[CAEMRIC_GRAD_TBL_SIZE];    /**< sector sizes of y direction            */
    } grid;

    struct
    {
        uint16_t LscRDataTbl[CAMERIC_DATA_TBL_SIZE];    /**< correction values of R color part */
        uint16_t LscGRDataTbl[CAMERIC_DATA_TBL_SIZE];   /**< correction values of G (red lines) color part */
        uint16_t LscGBDataTbl[CAMERIC_DATA_TBL_SIZE];   /**< correction values of G (blue lines) color part  */
        uint16_t LscBDataTbl[CAMERIC_DATA_TBL_SIZE];    /**< correction values of B color part  */
    } gain;

} CamEngineLscConfig_t;


typedef struct CamEngineWdrCurve_s
{
    uint16_t            Ym[33];
    uint8_t             dY[33];
} CamEngineWdrCurve_t;

typedef enum CamEngineRgbirOutBpt_e {
    CAM_ENGINE_RGBIR_OUT_BPT_RGGB = 0,
    CAM_ENGINE_RGBIR_OUT_BPT_GRBG,
    CAM_ENGINE_RGBIR_OUT_BPT_GBRG,
    CAM_ENGINE_RGBIR_OUT_BPT_BGGR,
} CamEngineRgbirOutBpt_t;

typedef enum CamEngineRgbirBpt_e{
    CAM_ENGINE_RGBIR_BPT_BGGIR= 0,
    CAM_ENGINE_RGBIR_BPT_GRIRG,
    CAM_ENGINE_RGBIR_BPT_RGGIR,
    CAM_ENGINE_RGBIR_BPT_GBIRG,
    CAM_ENGINE_RGBIR_BPT_GIRRG,
    CAM_ENGINE_RGBIR_BPT_IRGGB,
    CAM_ENGINE_RGBIR_BPT_GIRBG,
    CAM_ENGINE_RGBIR_BPT_IRGGR,
    CAM_ENGINE_RGBIR_BPT_RGIRB,
    CAM_ENGINE_RGBIR_BPT_GRBIR,
    CAM_ENGINE_RGBIR_BPT_IRBRG,
    CAM_ENGINE_RGBIR_BPT_BIRGR,
    CAM_ENGINE_RGBIR_BPT_BGIRR,
    CAM_ENGINE_RGBIR_BPT_GBRIR,
    CAM_ENGINE_RGBIR_BPT_IRRBG,
    CAM_ENGINE_RGBIR_BPT_RIRGB,
} CamEngineRgbirBpt_t;

typedef enum CamEngineRgbirFiltMode_e{
    CAM_ENGINE_RGBIR_GREEN_FILT_STATIC= 0,
    CAM_ENGINE_RGBIR_GREEN_FILT_DYNAMIC,
} CamEngineRgbirFiltMode_t;
/******************************************************************************/
/**
 * @brief   Structure to configure the  rgbir
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineRgbirConfig_s
{
    bool_t enable;
    bool_t enable_ir_raw_out;
    bool_t part1Enable;
    bool_t part2Enable;
    bool_t rcccEnable;
    uint8_t dmscThreshold;
    uint8_t stageSelect;
    CamEngineRgbirOutBpt_t outRgbBpt;
    CamEngineRgbirBpt_t rgbirBpt;
    CamEngineRgbirFiltMode_t filtMode;
    bool_t filtEnable;
    double irSigmas;
    uint32_t ir_threshold, l_threshold;
    uint16_t bls_r, bls_g, bls_b, bls_ir;
    double gain_r, gain_g, gain_b, gain_ir;
    double cc_mtx[12];
    uint16_t width, height;
    uint16_t dpccTh[4], dpccMidTh[4];

}CamEngineRgbirConfig_t;

/******************************************************************************/
/**
 * @brief   Structure to configure the edge enhancement
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineEeConfig_s
{
    bool                                    enabled;
    uint8_t                                 strength;

    uint16_t                                yUpGain;
    uint16_t                                yDownGain;

    uint16_t                                uvGain;
    uint16_t                                edgeGain;

    uint8_t                                 srcStrength;
    CamerIcIspEeInputDataFormatSelect_t     inputDataFormatSelect;

}CamEngineEeConfig_t;

/*****************************************************************************/
/**
 * @brief   Auto-Focus-Control search algorithm.
 *
 *****************************************************************************/
typedef enum CamEngineAfSearchAlgorithm_e {
    CAM_ENGINE_AUTOFOCUS_SEARCH_ALGORITHM_INVALID           = 0,    /**< invalid search algorithm */
    CAM_ENGINE_AUTOFOCUS_SEARCH_ALGORITHM_FULL_RANGE        = 1,    /**< full range */
    CAM_ENGINE_AUTOFOCUS_SEARCH_ALGORITHM_ADAPTIVE_RANGE    = 2,    /**< adaptive range */
    CAM_ENGINE_AUTOFOCUS_SEARCH_ALGORITHM_HILL_CLIMBING     = 3,    /**< hill climbing */
    CAM_ENGINE_AUTOFOCUS_SEARCH_ALGORITHM_MAX
} CamEngineAfSearchAlgorithm_t;


/*****************************************************************************/
/**
 * @brief   Auto-Exposure-Control scene evaluation mode.
 *
 *****************************************************************************/
typedef enum CamEngineAecSemMode_e {
    CAM_ENGINE_AEC_SCENE_EVALUATION_INVALID       = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_AEC_SCENE_EVALUATION_DISABLED      = 1,    /**< scene evaluation disabled (fix setpoint) */
    CAM_ENGINE_AEC_SCENE_EVALUATION_FIX           = 2,    /**< scene evaluation fix (static ROI) */
    CAM_ENGINE_AEC_SCENE_EVALUATION_ADAPTIVE      = 3,    /**< scene evaluation adaptive (adaptive ROI) */
    CAM_ENGINE_AEC_SCENE_EVALUATION_MAX
} CamEngineAecSemMode_t;


/*****************************************************************************/
/**
 * @brief   Auto-Exposure-Control histogram.
 *
 *****************************************************************************/
#ifdef ISP_HIST256
#define CAM_ENGINE_AEC_HIST_NUM_BINS           256  /**< number of bins */
#else
#define CAM_ENGINE_AEC_HIST_NUM_BINS           16  /**< number of bins */
#endif
typedef uint32_t CamEngineAecHistBins_t[CAM_ENGINE_AEC_HIST_NUM_BINS];


/*****************************************************************************/
/**
 * @brief   Auto-Exposure-Control luminance grid.
 *
 *****************************************************************************/
#define CAM_ENGINE_AEC_EXP_GRID_ITEMS          25  /**< number of grid items (see @ref CamerIcMeanLuma_t) */
typedef uint8_t CamEngineAecMeanLuma_t[CAM_ENGINE_AEC_EXP_GRID_ITEMS];


/*****************************************************************************/
/**
 * @brief   Auto-White-Balance mode.
 *
 *****************************************************************************/
typedef enum CamEngineAwbMode_e {
    CAM_ENGINE_AWB_MODE_INVALID = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_AWB_MODE_MANUAL  = 1,    /**< manual mode */
    CAM_ENGINE_AWB_MODE_AUTO    = 2,    /**< run auto mode */
    CAM_ENGINE_AWB_MODE_MAX
} CamEngineAwbMode_t;

/******************************************************************************/
/**
 * @brief   Enumeration type to configure the chrominace output range in the
 *          CamerIC color processing unit.
 *
 *****************************************************************************/
typedef enum CamerIcCprocChrominanceRangeOut_e {
    CAMERIC_CPROC_CHROM_RANGE_OUT_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_CPROC_CHROM_RANGE_OUT_BT601         = 1,    /**< CbCr_out clipping range 16..240 according to ITU-R BT.601 standard */
    CAMERIC_CPROC_CHROM_RANGE_OUT_FULL_RANGE    = 2,    /**< full UV_out clipping range 0..255 */
    CAMERIC_CPROC_CHROM_RANGE_OUT_MAX                   /**< upper border (only for an internal evaluation) */
} CamerIcCprocChrominaceRangeOut_t;


/******************************************************************************/
/**
 * @brief   Enumeration type to configure the luminance output range in the
 *          CamerIC color processing unit.
 *
 *****************************************************************************/
typedef enum CamerIcCprocLuminanceRangeOut_e {
    CAMERIC_CPROC_LUM_RANGE_OUT_INVALID         = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_CPROC_LUM_RANGE_OUT_BT601           = 1,    /**< Y_out clipping range 16..235; offset of 16 is added to Y_out according to ITU-R BT.601 standard */
    CAMERIC_CPROC_LUM_RANGE_OUT_FULL_RANGE      = 2,    /**< Y_out clipping range 0..255; no offset is added to Y_out */
    CAMERIC_CPROC_LUM_RANGE_OUT_MAX                     /**< upper border (only for an internal evaluation) */
} CamerIcCprocLuminanceRangeOut_t;


/******************************************************************************/
/**
 * @brief   Enumeration type to configure the luminance input range in the
 *          CamerIC color processing unit.
 *
 *****************************************************************************/
typedef enum CamerIcCprocLuminanceRangeIn_e {
    CAMERIC_CPROC_LUM_RANGE_IN_INVALID          = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_CPROC_LUM_RANGE_IN_BT601            = 1,    /**< Y_in range 64..940 according to ITU-R BT.601 standard; offset of 64 will be subtracted from Y_in */
    CAMERIC_CPROC_LUM_RANGE_IN_FULL_RANGE       = 2,    /**< Y_in full range 0..1023; no offset will be subtracted from Y_in */
    CAMERIC_CPROC_LUM_RANGE_IN_MAX                      /**< upper border (only for an internal evaluation) */
} CamerIcCprocLuminanceRangeIn_t;


/******************************************************************************/
/**
 * @brief   Structure to configure the color processing module
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineCprocConfig_s {
    CamerIcCprocChrominaceRangeOut_t    ChromaOut;      /**< configuration of color processing chrominance pixel clipping range at output */
    CamerIcCprocLuminanceRangeOut_t     LumaOut;        /**< configuration of color processing luminance input range (offset processing) */
    CamerIcCprocLuminanceRangeIn_t      LumaIn;         /**< configuration of color processing luminance output clipping range */

    float                               contrast;       /**< contrast value to initially set */
    int8_t                              brightness;     /**< brightness value to initially set */
    float                               saturation;     /**< saturation value to initially set */
    float                               hue;            /**< hue value to initially set */
} CamEngineCprocConfig_t;


/******************************************************************************/
/**
 * @brief   Enumeration type to configure the IE working mode.
 *
 *****************************************************************************/
typedef enum CamerIcIeMode_e {
    CAMERIC_IE_MODE_INVALID                 = 0,        /**< lower border (only for an internal evaluation) */
    CAMERIC_IE_MODE_GRAYSCALE               = 1,        /**< Set a fixed chrominance of 128 (neutral grey) */
    CAMERIC_IE_MODE_NEGATIVE                = 2,        /**< Luminance and chrominance data is being inverted */
    CAMERIC_IE_MODE_SEPIA                   = 3,        /**< Chrominance is changed to produce a historical like brownish image color */
    CAMERIC_IE_MODE_COLOR                   = 4,        /**< Converting picture to grayscale while maintaining one color component. */
    CAMERIC_IE_MODE_EMBOSS                  = 5,        /**< Edge detection, will look like an relief made of metal */
    CAMERIC_IE_MODE_SKETCH                  = 6,        /**< Edge detection, will look like a pencil drawing */
    CAMERIC_IE_MODE_SHARPEN                 = 7,        /**< Edge detection, will look like a sharper drawing */
    CAMERIC_IE_MODE_MAX                                 /**< upper border (only for an internal evaluation) */
} CamerIcIeMode_t;

/******************************************************************************/
/**
 * @brief   Enumeration type to configure the IE working range.
 *
 *****************************************************************************/
typedef enum CamerIcIeRange_e {
    CAMERIC_IE_RANGE_INVALID                = 0,        /**< lower border (only for an internal evaluation) */
    CAMERIC_IE_RANGE_BT601                  = 1,        /**< pixel value range accoring to BT.601 */
    CAMERIC_IE_RANGE_FULL_RANGE             = 2,        /**< YCbCr full range 0..255 */
    CAMERIC_IE_RANG_MAX                                 /**< upper border (only for an internal evaluation) */
} CamerIcIeRange_t;

/******************************************************************************/
/**
 * @brief   Enumeration type to configure the color selection effect
 *
 *****************************************************************************/
typedef enum CamerIcIeColorSelection_e {
    CAMERIC_IE_COLOR_SELECTION_INVALID      = 0,        /**< lower border (only for an internal evaluation) */
    CAMERIC_IE_COLOR_SELECTION_RGB          = 1,        /**< red, green and blue */
    CAMERIC_IE_COLOR_SELECTION_B            = 2,        /**< blue */
    CAMERIC_IE_COLOR_SELECTION_G            = 3,        /**< green */
    CAMERIC_IE_COLOR_SELECTION_GB           = 4,        /**< green and blue */
    CAMERIC_IE_COLOR_SELECTION_R            = 5,        /**< red */
    CAMERIC_IE_COLOR_SELECTION_RB           = 6,        /**< red and blue */
    CAMERIC_IE_COLOR_SELECTION_RG           = 7,        /**< red and green */
    CAMERIC_IE_COLOR_SELECTION_MAX                      /**< upper border (only for an internal evaluation) */
} CamerIcIeColorSelection_t;


/*****************************************************************************/
/**
 * @brief   Generic structure for the cross talk matrix of the four color
 *          components.
 *
 *****************************************************************************/
typedef struct CamEngineCcMatrix_s
{
    float Coeff[9U];
} CamEngineCcMatrix_t;

/*****************************************************************************/
/**
 * @brief   Generic structure for the cross talk offset of the four color
 *          components.
 *
 *****************************************************************************/
typedef struct CamEngineCcOffset_s
{
    int16_t Red;
    int16_t Green;
    int16_t Blue;
} CamEngineCcOffset_t;


/*****************************************************************************/
/**
 * @brief   Generic structure for the white balance gains of the four color
 *          components.
 *
 *****************************************************************************/
typedef struct CamEngineWbGains_s
{
    float Red;
    float GreenR;
    float GreenB;
    float Blue;
} CamEngineWbGains_t;


/******************************************************************************/
/**
 * @brief   Structure to configure the Image Effects module
 *
 *****************************************************************************/
typedef struct CamerIcIeConfig_s {
    CamerIcIeMode_t                     mode;           /**< working mode (see @ref CamerIcIeMode_e) */
    CamerIcIeRange_t                    range;          /**< working range (see @ref CamerIcIeRange_e) */

    union ModeConfig_u {
        struct Sepia_s {                                /**< active when sepia effect */
            uint8_t                     TintCb;
            uint8_t                     TintCr;
        } Sepia;

        struct ColorSelection_s {                       /**< active when color selection effect */
            CamerIcIeColorSelection_t   col_selection;
            uint8_t                     col_threshold;
        } ColorSelection;

        struct Emboss_s {                               /**< active when emboss effect */
            int8_t                      coeff[9];
        } Emboss;

        struct Sketch_s {                               /**< active when sketch effect */
            int8_t                      coeff[9];
        } Sketch;

        struct Sharpen_s {                              /**< active when sharpen */
            uint8_t                     factor;         /**< sharpen factor */
            uint8_t                     threshold;      /**< corring threshold */
            int8_t                      coeff[9];       /**< convolution coefficients */
        } Sharpen;
    } ModeConfig;
} CamerIcIeConfig_t;


/******************************************************************************/
/**
 * @brief   Enumeration type to configure the super impose working mode.
 *
 *****************************************************************************/
typedef enum CamEngineSimpMode_e
{
    CAM_ENGINE_SIMP_MODE_INVALID            = 0,        /**< lower border (only for an internal evaluation) */
    CAM_ENGINE_SIMP_MODE_OVERLAY            = 1,        /**< overlay mode */
    CAM_ENGINE_SIMP_MODE_KEYCOLORING        = 2,        /**< keycoloring mode */
    CAM_ENGINE_SIMP_MODE_MAX                            /**< upper border (only for an internal evaluation) */
} CamEngineSimpMode_t;



/******************************************************************************/
/**
 * @brief   Structure to configure the super impose module
 *
 * @note    This structure needs to be converted to driver structure
 *
 *****************************************************************************/
typedef struct CamEngineSimpConfig_s
{
    CamEngineSimpMode_t     Mode;

    union SimpModeConfig_u
    {
        struct Overlay_s
        {
            uint32_t OffsetX;
            uint32_t OffsetY;
        } Overlay;

        struct KeyColoring_s
        {
            uint8_t Y;
            uint8_t Cb;
            uint8_t Cr;
        } KeyColoring;
    } SimpModeConfig;

    PicBufMetaData_t      *pPicBuffer;

} CamEngineSimpConfig_t;

typedef struct  CamEngineEzsbiMiConfig_s {
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
}CamEngineEzsbiMiConfig_t;

typedef struct CamEngineFlexaConfig_s {
    bool enable;
    bool use_flexa_cfg;
    bool input_use;
    bool sbi_mi_cfg_use;
    uint32_t sbi_segment;
    uint32_t slice_size;
    uint32_t height;
    uint32_t buffer_number;
    int input_fds[PIC_BUFFER_NUM_INPUT];
    int small_fds[PIC_BUFFER_NUM_SMALL_IMAGE];
    int large_fds[PIC_BUFFER_NUM_LARGE_IMAGE];
    CamEngineEzsbiMiConfig_t mi_cfg;
}CamEngineFlexaConfig_t;

#endif  // __CAMERA_DEVICE_SENSOR_DEFS_H__