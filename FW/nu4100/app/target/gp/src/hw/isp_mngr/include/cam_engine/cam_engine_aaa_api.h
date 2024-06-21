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
 * @cond    cam_engine_aaa
 *
 * @file    cam_engine_aaa_api.h
 *
 * @brief
 *
 *   Interface description of the CamEngine Auto Algorithms.
 *
 *****************************************************************************/
/**
 *
 * @defgroup cam_engine_aaa_api CamEngine Auto Algorithms API
 * @{
 *
 */

#ifndef __CAM_ENGINE_AAA_API_H__
#define __CAM_ENGINE_AAA_API_H__

#include <ebase/types.h>
#include <common/return_codes.h>
#include <cam_device/cam_device_ispcore_defs.h>

#ifdef __cplusplus
extern "C"
{
#endif



/*****************************************************************************/
/**
 *          CamEngineAwbRgProj_t
 *
 * @brief   AWB Projection Configuration in R/G Layer
 *
 *****************************************************************************/
typedef struct CamEngineAwbRgProj_s
{
    float   fRgProjIndoorMin;
    float   fRgProjOutdoorMin;
    float   fRgProjMax;
    float   fRgProjMaxSky;
} CamEngineAwbRgProj_t;
RESULT CamEngineSetMode(CamEngineHandle_t hCamEngine, int moduleName, uint8_t mode);
RESULT CamEngineGetMode(CamEngineHandle_t hCamEngine, int moduleName, uint8_t *mode);





/*****************************************************************************/
/**
 * @brief   Auto-Exposure-Control histogram.
 *
 *****************************************************************************/
#ifdef ISP_HIST256
#define CAM_ENGINE_AEC_HIST_NUM_BINS           256 /**< number of bins */
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
 * @brief   Auto-Exposure-Control roi.
 *
 *****************************************************************************/
typedef struct CamEngine3ARoiWindow_s
{
    uint32_t xStart;    // horizontal start position 
    uint32_t yStart;    // vertical start position
    uint32_t width;     // rect width
    uint32_t height;    // rect height
    float weight;       // weight
}CamEngine3ARoiWindow_t;

#define CAM_ENGINE_AEC_ROI_WINDOWS_MAX 16
#define CAM_ENGINE_AEC_ROI_WEIGHT_FRAC_BITS 10 //Q10
typedef struct CamEngineAecRoi_s
{
    uint32_t windowNum;        // window number of ROI
    CamEngine3ARoiWindow_t roiWindow[CAM_ENGINE_AEC_ROI_WINDOWS_MAX];
} CamEngineAecRoi_t;

#define CAM_ENGINE_3A_METADATA_AWBGAIN_NUM 4
#define CAM_ENGINE_3A_METADATA_LSCTABLE_NUM 1156 //17*17*4
#define CAM_ENGINE_3A_METADATA_CCM_NUM 9
#define CAM_ENGINE_3A_METADATA_OFFSET_NUM 3
typedef struct CamEngine3AMetaData_s
{
    //AWB sync
    bool awbSet;
    int awbGainSize;
    float *awbGain;//rGain grGain gbGain bGain
    int CCMSize;
    float *CCM;
    int offsetSize;
    float *offset;
    //awb OTP data
    bool awbOTP;
    float  ct;  //color tempture

    //AE sync
    bool aeSet;
    float expsoureTime;//sensor exposure time
    float expsoureGain;//sensor Expsoure  total Gain
    float ispDgain;  // isp Digit Gain

    //AWB LSC table sync
    bool lscSet;
    bool lscOTP;
    int lscTableSize;
    uint16_t *lscTable;//

    /*Enable flags*/
    bool metadataEnableAWB;
    bool metadataEnableAE;
    /*Flag to say if we should copy the metadataEnable flags (Added for conveniance at)*/
    bool hasmetadataEnableAE;
    bool hasmetadataEnableAWB;
    int (*ae_cb)(void *);   /*Callback when AE has been finished*/
    int (*awb_cb)(void *);   /*Callback when AE has been finished*/
} CamEngine3AMetaData_t;

/*****************************************************************************/
/**
 * @brief   Auto-Focus-Control mode.
 *
 *****************************************************************************/
typedef enum CamEngineAfMode_e
{
    CAM_ENGINE_AUTOFOCUS_MODE_INVALID       = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_AUTOFOCUS_MODE_ONESHOT       = 1,    /**< one-shot mode (runs autofocus search once and stops atomatically after positioing the lense to max) */
    CAM_ENGINE_AUTOFOCUS_MODE_CONTINOUSLY   = 2,    /**< continuously observe the focus */
    CAM_ENGINE_AUTOFOCUS_MODE_EXTERNAL      = 3,    /**< attach an external algorithm */
    CAM_ENGINE_AUTOFOCUS_MODE_STOP          = 4,    /**< stop current autofocus search */
    CAM_ENGINE_AUTOFOCUS_MODE_MAX
} CamEngineAfMode_t;



/*****************************************************************************/
/**
 *          CamEngineA2dnrParamNode_t
 *
 * @brief   A2DNR parameters node structure
 *
 *****************************************************************************/
typedef struct CamEngineA2dnrParamNode_s
{
	float					gain;
	float					integrationTime;
    float                 	sigma;       	    /**< sigma */
    uint8_t               	strength;  		  	/**< strength */
    uint8_t               	pregmaStrength;     /**< pregmaStrength */
} CamEngineA2dnrParamNode_t;



/*****************************************************************************/
/**
 * @brief   Auto-3Dimension-Noise-Reduction mode.
 *
 *****************************************************************************/
typedef enum CamEngineA3dnrMode_e
{
    CAM_ENGINE_A3DNR_MODE_INVALID = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_A3DNR_MODE_MANUAL  = 1,    /**< manual mode */
    CAM_ENGINE_A3DNR_MODE_AUTO    = 2,    /**< run auto mode */
    CAM_ENGINE_A3DNR_MODE_MAX
} CamEngineA3dnrMode_t;



/*****************************************************************************/
/**
 *          CamEngineA3dnrParamNode_t
 *
 * @brief   A3DNR parameters node structure
 *
 *****************************************************************************/
typedef struct CamEngineA3dnrParamNode_s
{
	float					gain;				/**< gain */
	float					integrationTime;	/**< integrationTime */
    uint8_t               	strength;			/**< strength */
    uint16_t              	motionFactor;		/**< motionFactor */
    uint16_t              	deltaFactor;		/**< deltaFactor */
} CamEngineA3dnrParamNode_t;



/*****************************************************************************/
/**
 * @brief   Auto-Wide-Daynamic-Range(Version3) mode.
 *
 *****************************************************************************/
typedef enum CamEngineAwdr3Mode_e
{
    CAM_ENGINE_AWDR3_MODE_INVALID = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_AWDR3_MODE_MANUAL  = 1,    /**< manual mode */
    CAM_ENGINE_AWDR3_MODE_AUTO    = 2,    /**< run auto mode */
    CAM_ENGINE_AWDR3_MODE_MAX
} CamEngineAwdr3Mode_t;



/*****************************************************************************/
/**
 *          CamEngineAwdr3ParamNode_t
 *
 * @brief   AWDR3 parameters node structure
 *
 *****************************************************************************/
typedef struct CamEngineAwdr3ParamNode_s
{
	float					gain;				/**< gain */
	float					integrationTime;	/**< integrationTime */
    uint8_t					strength;			/**< strength */
    uint8_t					globalStrength;		/**< globalStrength */
    uint8_t					maxGain;			/**< maxGain */
} CamEngineAwdr3ParamNode_t;



/*****************************************************************************/
/**
 * @brief   Auto-Edge-Enhancement mode.
 *
 *****************************************************************************/
typedef enum CamEngineAeeMode_e
{
    CAM_ENGINE_AEE_MODE_INVALID = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_AEE_MODE_MANUAL  = 1,    /**< manual mode */
    CAM_ENGINE_AEE_MODE_AUTO    = 2,    /**< run auto mode */
    CAM_ENGINE_AEE_MODE_MAX
} CamEngineAeeMode_t;



/*****************************************************************************/
/**
 *          CamEngineAeeParamNode_t
 *
 * @brief   AEE parameters node structure
 *
 *****************************************************************************/
typedef struct CamEngineAeeParamNode_s
{
	float					gain;				/**< gain */
	float					integrationTime;	/**< integrationTime */
    uint8_t					strength;			/**< strength */
	uint16_t				yUpGain;			/**< yUpGain */
	uint16_t				yDownGain;			/**< yDownGain */
	uint16_t				uvGain;				/**< uvGain */
	uint16_t				edgeGain;			/**< edgeGain */
} CamEngineAeeParamNode_t;



/*****************************************************************************/
/**
 * @brief   Auto-Filter mode.
 *
 *****************************************************************************/
typedef enum CamEngineAfltMode_e
{
    CAM_ENGINE_AFLT_MODE_INVALID = 0,    /**< invalid mode (only for initialization) */
    CAM_ENGINE_AFLT_MODE_MANUAL  = 1,    /**< manual mode */
    CAM_ENGINE_AFLT_MODE_AUTO    = 2,    /**< run auto mode */
    CAM_ENGINE_AFLT_MODE_MAX
} CamEngineAfltMode_t;



/*****************************************************************************/
/**
 *          CamEngineAfltParamNode_t
 *
 * @brief   AFLT parameters node structure
 *
 *****************************************************************************/
typedef struct CamEngineAfltParamNode_s
{
	float					gain;				/**< gain */
	float					integrationTime;	/**< integrationTime */
    uint8_t					denoiseLevel;		/**< denoiseLevel */
    uint8_t					sharpenLevel;		/**< sharpenLevel */
} CamEngineAfltParamNode_t;



/*****************************************************************************/
/**
 * @brief   This functions starts the Auto-White-Balance.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   mode                working mode ( Manual | Auto )
 * @param   index               illumination profile index
 *                              Manual: profile to run
 *                              Auto: start-profile
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_INVALID_PARM    invalid configuration
 * @retval  RET_OUTOFRANGE      a configuration parameter is out of range
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwbStart
(
    CamEngineHandle_t           hCamEngine,     /**< handle CamEngine */
    const CamEngineAwbMode_t    mode,           /**< run-mode */
    const uint32_t              index,          /**< AUTO: start-profile, MANUAL: profile to run */
    const bool_t                damp            /**< damping on/off */
);



/*****************************************************************************/
/**
 * @brief   This functions stops the Auto-White-Balance.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwbStop
(
    CamEngineHandle_t           hCamEngine      /**< handle CamEngine */
);



/*****************************************************************************/
/**
 * @brief   This functions resets the Auto-White-Balance.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwbReset
(
    CamEngineHandle_t           hCamEngine      /**< handle CamEngine */
);



/*****************************************************************************/
/**
 * @brief   This functions returns the Auto-White-Balance status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 * @param   pMode               working mode ( Manual | Auto )
 * @param   pCieProfile         illumination profile
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwbStatus
(
    CamEngineHandle_t           hCamEngine,     /**< handle CamEngine */
    bool_t                      *pRunning,      /**< BOOL_TRUE: running, BOOL_FALSE: stopped */
    CamEngineAwbMode_t          *pMode,
    uint32_t                    *pCieProfile,
    CamEngineAwbRgProj_t        *pRgProj,
    bool_t                      *pDamping       /**< BOOL_TRUE: damping on, BOOL_FALSE: damping off */
);



/*****************************************************************************/
/**
 * @brief   This functions starts the Auto-Exposure-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecStart
(
    CamEngineHandle_t           hCamEngine      /**< handle CamEngine */
);



/*****************************************************************************/
/**
 * @brief   This functions stops the Auto-Exposure-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecStop
(
    CamEngineHandle_t           hCamEngine      /**< handle CamEngine */
);



/*****************************************************************************/
/**
 * @brief   This functions resets the Auto-Exposure-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecReset
(
    CamEngineHandle_t           hCamEngine      /**< handle CamEngine */
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Auto-Exposure-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecConfigure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const CamEngineAecSemMode_t mode,            /**< scene evaluation mode */
    const float                 setPoint,        /**< set point to hit by the ae control system */
    const float                 clmTolerance,    /**< tolerance */
    const float                 dampOver,        /**< damping */
    const float                 dampUnder        /**< damping */
);


/*****************************************************************************/
/**
 * @brief   This functions configures the ROI(Region of Interest).
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecRoiSet
(
    CamEngineHandle_t           hCamEngine,
    CamEngineAecRoi_t           *pRoi
);


/*****************************************************************************/
/**
 * @brief   This functions Get the ROI(Region of Interest) windows.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecRoiGet
(
    CamEngineHandle_t           hCamEngine,
    CamEngineAecRoi_t           *pRoi
);


/*****************************************************************************/
/**
 * @brief   This functions Set the MetaData.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineSet3AMetaData
(
    CamEngineHandle_t           hCamEngine,
    CamEngine3AMetaData_t      *pMetaData
);

/*****************************************************************************/
/**
 * @brief   This functions Get MetaData.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGet3AMetaData
(
    CamEngineHandle_t           hCamEngine,
    CamEngine3AMetaData_t      *pMetaData
);

/*****************************************************************************/
/**
 * @brief   This functions returns the Auto-Exposure-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecStatus
(
    CamEngineHandle_t           hCamEngine,        /**< handle CamEngine */
    bool_t                      *pRunning,         /**< BOOL_TRUE: running, BOOL_FALSE: stopped */
    CamEngineAecSemMode_t       *pMode,            /**< scene evaluation mode */
    float                       *pSetPoint,        /**< set point to hit by the ae control system */
    float                       *pClmTolerance,    /**< tolerance */
    float                       *pDampOver,        /**< damping */
    float                       *pDampUnder        /**< damping */
);


/*****************************************************************************/
/**
* @brief   This functions returns the Auto-Exposure-Control status of gain and exposure.
*
* @param   hCamEngine          Handle to the CamEngine instance.
*
* @return  Return the result of the function call.
* @retval  RET_SUCCESS         function succeed
* @retval  RET_WRONG_HANDLE    invalid instance handle
*
*****************************************************************************/
RESULT CamEngineAecGetGainAndIntegrationTime
(
	CamEngineHandle_t           hCamEngine,
	float                       *pGain,
	float                       *pIntegrationTime
);

/*****************************************************************************/
/**
 * @brief   This function returns the current Auto-Exposure-Control histogram.
 *
 * @param   handle              Handle to the CamEngine instance.
 * qparam   pHistogram          pointer to the histogram bins
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecGetHistogram
(
    CamEngineHandle_t        hCamEngine,
    CamEngineAecHistBins_t   *pHistogram
);



/*****************************************************************************/
/**
 * @brief   This function returns the current Auto-Exposure-Control luminance grid.
 *
 * @param   handle              Handle to the CamEngine instance.
 * qparam   pLuma               pointer to the luminance grid
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecGetLuminance
(
    CamEngineHandle_t        hCamEngine,
    CamEngineAecMeanLuma_t   *pLuma
);



/*****************************************************************************/
/**
 * @brief   This function returns the current Auto-Exposure-Control object region.
 *
 * @param   handle              Handle to the CamEngine instance.
 * qparam   pLuma               pointer to the object region
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAecGetObjectRegion
(
    CamEngineHandle_t        hCamEngine,
    CamEngineAecMeanLuma_t   *pObjectRegion
);



/*****************************************************************************/
/**
 * @brief   This function returns if Auto-Focus-Control is available with the
 *          connected sensor board.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: available, BOOL_FALSE: not available
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfAvailable
(
    CamEngineHandle_t       hCamEngine,
    bool_t                  *pAvailable
);



/*****************************************************************************/
/**
 * @brief   This function starts the Auto-Focus-Control (continous mode).
 *
 * @param   hCamEngine              Handle to the CamEngine instance.
 * @param   AutoFocusSearchAgoritm  search algorithm ( ADAPTIVE_RANGE | HILL_CLIMBING )
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfStart
(
    CamEngineHandle_t                    hCamEngine,
    const CamEngineAfSearchAlgorithm_t   searchAgoritm
);



/*****************************************************************************/
/**
 * @brief   This function starts the Auto-Focus-Control (one-shot mode).
 *
 * @param   hCamEngine              Handle to the CamEngine instance.
 * @param   AutoFocusSearchAgoritm  search algorithm ( ADAPTIVE_RANGE | HILL_CLIMBING )
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfOneShot
(
    CamEngineHandle_t                    hCamEngine,
    const CamEngineAfSearchAlgorithm_t   searchAgoritm
);



/*****************************************************************************/
/**
 * @brief   This function stops the Auto-Focus-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function returns the Auto-Focus-Control status.
 *
 * @param   hCamEngine              Handle to the CamEngine instance.
 * @param   pRunning                BOOL_TRUE: running, BOOL_FALSE: stopped
 * @param   pAutoFocusSearchAgoritm search algorithm ( ADAPTIVE_RANGE | HILL_CLIMBING )
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfStatus
(
    CamEngineHandle_t                   hCamEngine,
    bool_t                              *pRunning,
    CamEngineAfSearchAlgorithm_t        *pSearchAgoritm
);



/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-DPF-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpfStart
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-DPF-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpfStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-DPF-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpfConfigure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const float                 gradient,        /**< gradient */
    const float                 offset,          /**< offset */
    const float                 min,             /**< upper bound */
    const float                 div,             /**< division factor */
    const uint8_t               sigmaGreen,      /**< sigma green */
    const uint8_t               sigmaRedBlue     /**< sigma red/blue */
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-DPF-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpfStatus
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pRunning,
    float               *pGradient,        /**< gradient */
    float               *pOffset,          /**< offset */
    float               *pMin,             /**< upper bound */
    float               *pDiv,             /**< division factor */
    uint8_t             *pSigmaGreen,      /**< sigma green */
    uint8_t             *pSigmaRedBlue     /**< sigma red/blue */

);

RESULT CamEngineAdpccConfigure
(
	CamEngineHandle_t           hCamEngine,
	const uint32_t index_set
);

RESULT CamEngineAdpccGetConfigure
(
	CamEngineHandle_t           hCamEngine,
    uint32_t *index_set
);

/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-DPCC-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpccStart
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-DPCC-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpccStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-DPF-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdpccStatus
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pRunning
);


/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-3DNR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA3dnrStart
(
    CamEngineHandle_t 			hCamEngine,
    const CamEngineA3dnrMode_t  mode           /**< run-mode */
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-3DNR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA3dnrStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-3DNR-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA3dnrConfigure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const uint8_t               strength,  		 /**< strength */
    const int32_t               motionFactor,  	 /**< motionFactor */
    const uint16_t              deltaFactor      /**< deltaFactor */
);



/*****************************************************************************/
/**
 * @brief   This functions set the level value base on Adaptive-3DNR-Control
 *	        mode.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA3dnrSetAutoLevel
(
    CamEngineHandle_t           hCamEngine,	/**< handle CamEngine */
    const uint8_t               level		/**< level */
);



/*****************************************************************************/
/**
 * @brief   This functions set the Adaptive-3DNR-Control auto mode table.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA3dnrSetAutoTable
(
    CamEngineHandle_t           		hCamEngine,      	/**< handle CamEngine */
    const CamEngineA3dnrParamNode_t 	*pParamNodeTable,	/**< CamEngineA3dnrParamNode_t */
    const uint8_t               		nodeNum   			/**< nodeNum */
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-3DNR-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineA3dnrStatus
(
    CamEngineHandle_t   	hCamEngine,
    bool_t              	*pRunning,
    CamEngineA3dnrMode_t 	*pMode,				/**< run-mode */
    float					*pGain,				/**< gain */
	float					*pIntegrationTime,	/**< integrationTime */
    uint8_t             	*pStrength,			/**< strength */
    int32_t            		*pMotionFactor,		/**< motionFactor */
    uint16_t            	*pDeltaFactor		/**< deltaFactor */
);



/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-WDR_V3-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwdr3Start
(
    CamEngineHandle_t 			hCamEngine,
    const CamEngineAwdr3Mode_t  mode           /**< run-mode */
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-WDR_V3-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwdr3Stop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-WDR_V3-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwdr3Configure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const uint8_t               strength,  		 /**< strength */
    const uint8_t               globalStrength,  /**< globalStrength */
    const uint8_t               maxGain          /**< maxGain */
);

/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-WDR_V5-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWdr5Status
(
    CamEngineHandle_t   	hCamEngine,
    bool_t              	*pRunning,
    CamEngineWdr5Config_t   *pWdr5Config
);

/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-WDR_V5-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/

RESULT CamEngineWdr5Configure
(
    CamEngineHandle_t                hCamEngine,      /**< handle CamEngine */
    const CamEngineWdr5Config_t        *pWdr5Config
);


/*****************************************************************************/
/**
 * @brief   This functions set the level value base on Adaptive-WDR_V3-Control
 *	        mode.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwdr3SetAutoLevel
(
    CamEngineHandle_t           hCamEngine,	/**< handle CamEngine */
    const uint8_t               level		/**< level */
);



/*****************************************************************************/
/**
 * @brief   This functions set the Adaptive-WDR_V3-Control auto mode table.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwdr3SetAutoTable
(
    CamEngineHandle_t           		hCamEngine,      	/**< handle CamEngine */
    const CamEngineAwdr3ParamNode_t 	*pParamNodeTable,	/**< CamEngineAwdr3ParamNode_t */
    const uint8_t               		nodeNum   			/**< nodeNum */
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-WDR_V3-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAwdr3Status
(
    CamEngineHandle_t   	hCamEngine,
    bool_t              	*pRunning,
    CamEngineAwdr3Mode_t 	*pMode,				/**< run-mode */
    float					*pGain,				/**< gain */
	float					*pIntegrationTime,	/**< integrationTime */
    uint8_t             	*pStrength,			/**< strength */
    uint8_t             	*pGlobalStrength,	/**< globalStrength */
    uint8_t             	*pMaxGain			/**< maxGain */
);



/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-HDR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrStart
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-HDR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-HDR-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrConfigure
(
    CamEngineHandle_t           hCamEngine,
    const uint8_t               extBit,
    const float                 hdrRatio,
    const float                 transrangeStart,
    const float                 transrangeEnd
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-HDR-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrStatus
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pRunning,
    uint8_t             *pExtBit,			/**< strength */
	float				*pHdrRatio			/**< sigma */
);



/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-EE-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAeeStart
(
    CamEngineHandle_t 			hCamEngine,
    const CamEngineAeeMode_t  	mode           /**< run-mode */
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-EE-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAeeStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-EE-Control
 *			manual mode parameters.
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAeeConfigure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const uint8_t               strength,  		 /**< strength */
	const uint16_t 				yUpGain,		 /**< yUpGain */
	const uint16_t 				yDownGain,		 /**< yDownGain */
	const uint16_t 				uvGain,			 /**< uvGain */
	const uint16_t 				edgeGain		 /**< edgeGain */
);



/*****************************************************************************/
/**
 * @brief   This functions set the Adaptive-EE-Control auto mode table.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAeeSetAutoTable

(
    CamEngineHandle_t           		hCamEngine,      	/**< handle CamEngine */
    const CamEngineAeeParamNode_t 		*pParamNodeTable,	/**< CamEngineAeeParamNode_t */
    const uint8_t               		nodeNum   			/**< nodeNum */
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-EE-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAeeStatus
(
    CamEngineHandle_t   hCamEngine,
    bool_t              *pRunning,
    CamEngineAeeMode_t 	*pMode,				/**< run-mode */
    float				*pGain,				/**< gain */
	float				*pIntegrationTime,	/**< integrationTime */
    uint8_t             *pStrength,			/**< strength */
    uint16_t            *pYUpGain,			/**< pYUpGain */
    uint16_t            *pYDownGain,		/**< pYDownGain */
    uint16_t            *pUvGain,			/**< pUvGain */
    uint16_t            *pEdgeGain			/**< pEdgeGain */
);



/*****************************************************************************/
/**
 * @brief   This function starts the Adaptive-Filter-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfltStart
(
    CamEngineHandle_t 			hCamEngine,
    const CamEngineAfltMode_t  	mode           /**< run-mode */
);



/*****************************************************************************/
/**
 * @brief   This function stops the Adaptive-Filter-Control.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfltStop
(
    CamEngineHandle_t hCamEngine
);



/*****************************************************************************/
/**
 * @brief   This functions configures the Adaptive-Filter-Control
 *			manual mode parameters.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfltConfigure
(
    CamEngineHandle_t           hCamEngine,      /**< handle CamEngine */
    const uint8_t               denoiseLevel,	 /**< denoiseLevel */
    const uint8_t               sharpenLevel,	 /**< sharpenLevel */
    const uint8_t               chrVMode,
    const uint8_t               chrHMode
);



/*****************************************************************************/
/**
 * @brief   This functions set the Adaptive-Filter-Control auto mode table.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfltSetAutoTable
(
    CamEngineHandle_t           		hCamEngine,      	/**< handle CamEngine */
    const CamEngineAfltParamNode_t 		*pParamNodeTable,	/**< CamEngineAfltParamNode_t */
    const uint8_t               		nodeNum   			/**< nodeNum */
);



/*****************************************************************************/
/**
 * @brief   This function returns the Adaptive-Filter-Control status.
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAfltStatus
(
    CamEngineHandle_t   	hCamEngine,
    bool_t              	*pRunning,
    CamEngineAfltMode_t 	*pMode,				/**< run-mode */
    float					*pGain,				/**< gain */
	float					*pIntegrationTime,	/**< integrationTime */
    uint8_t             	*pDenoiseLevel,		/**< pDenoiseLevel */
    uint8_t             	*pSharpenLevel,		/**< pSharpenLevel */
    uint8_t                 *pchrVMode,
    uint8_t                 *pchrHMode
);

/*****************************************************************************/
/**
 * @brief    This functions set the Adaptive-TDNR-Control auto mode table.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAtdnrSetAutoTable
(
    CamEngineHandle_t                    hCamEngine,         /**< handle CamEngine */
    const CamEngineAtdnrParamNode_t     *pParamNodeTable,    /**< CamEngineA3dnrParamNode_t */
    const uint8_t                        nodeNum             /**< nodeNum */
);

/*****************************************************************************/
/**
 * @brief    This function returns the Adaptive-TDNR-Control status.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 * @param    pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAtdnrStatus
(
    CamEngineHandle_t                hCamEngine,
    bool_t                            *pRunning,
    CamEngineAtdnrMode_t            *pMode,             /**< run-mode */
    CamEngineAtdnrParamNode_t         *pCurrentStatus
);

/*****************************************************************************/
/**
 * @brief    This function starts the Adaptive-HDRv1.2-Control.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrV12Start
(
    CamEngineHandle_t            hCamEngine,
    const CamEngineAhdrMode_t    mode           /**< run-mode */
);

/*****************************************************************************/
/**
 * @brief    This function stops the Adaptive-HDRv1.2-Control.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrV12Stop
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief    This functions configures Adaptive-HDRv1.2-Control
 *            manual mode parameters.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrV12Configure
(
    CamEngineHandle_t                hCamEngine,     /**< handle CamEngine */
    const CamEngineHdrConfig_t         *pHdrConfig        /**< point of hdrConfig */
);

/*****************************************************************************/
/**
 * @brief    This functions set the Adaptive-HDRv1.2-Control auto mode table.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrV12SetAutoTable
(
    CamEngineHandle_t                    hCamEngine,         /**< handle CamEngine */
    const CamEngineAhdrParamNode_t         *pParamNodeTable,    /**< CamEngineAhdrParamNode_t */
    const uint8_t                        nodeNum             /**< nodeNum */
);

/*****************************************************************************/
/**
 * @brief    This function returns the Adaptive-HDRv1.2-Control config.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 * @param    pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrV12GetConfig
(
    CamEngineHandle_t        hCamEngine,
    CamEngineHdrConfig_t    *pConfig
);

#if 0 //maybe need get hdr dol type at initial with a indepent API
/*****************************************************************************/
/**
 * @brief    This function returns the Adaptive-HDRv1.2-Control Dol type.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 * @param    pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAhdrV12GetDolType
(
    CamEngineHandle_t        hCamEngine,
    CamEngineHdrDolType_t    *pDolType            /**< Dol type */
);
#endif


/*****************************************************************************/
/**
 * @brief    This functions configures the Adaptive-WDR_V4-Control
 *            manual mode parameters.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWdr4Configure
(
    CamEngineHandle_t                hCamEngine,      /**< handle CamEngine */
    const CamEngineWdr4Config_t        *pWdr4Config
);

/*****************************************************************************/
/**
 * @brief    This function returns the Adaptive-WDR_V4-Control status.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 * @param    pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineWdr4GetStatus
(
    CamEngineHandle_t hCamEngine,
    bool_t *pRunning,
    CamEngineAwdr4ParamNode_t *pCurrentStatus
);

/*****************************************************************************/
/**
 * @brief    This function starts the Adaptive-CA-Control.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAcaStart
(
    CamEngineHandle_t            hCamEngine,
    const CamEngineAcaMode_t    mode           /**< run-mode */
);

/*****************************************************************************/
/**
 * @brief    This function stops the Adaptive-CA-Control.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAcaStop
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief    This functions configures the Adaptive-CA-Control
 *            manual mode parameters.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAcaConfigure
(
    CamEngineHandle_t                hCamEngine,      /**< handle CamEngine */
    const CamEngineCaConfig_t         *pCaConfig
);

/*****************************************************************************/
/**
 * @brief    This functions set the Adaptive-CA-Control auto mode table.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAcaSetAutoTable
(
    CamEngineHandle_t                    hCamEngine,         /**< handle CamEngine */
    const CamEngineAcaParamNode_t         *pParamNodeTable,    /**< CamEngineAcaParamNode_t */
    const uint8_t                        nodeNum             /**< nodeNum */
);

/*****************************************************************************/
/**
 * @brief    This function returns the Adaptive-CA-Control status.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 * @param    pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAcaStatus
(
    CamEngineHandle_t            hCamEngine,
    bool_t                        *pRunning,
    CamEngineAcaMode_t            *pMode,             /**< run-mode */
    CamEngineAcaStatus_t        *pCurrentStatus
);

//DCI(Dynamic Contrast Improvement)
/*****************************************************************************/
/**
 * @brief    This function starts the Adaptive-DCI-Control.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdciStart
(
    CamEngineHandle_t            hCamEngine,
    const CamEngineAdciMode_t    mode           /**< run-mode */
);

/*****************************************************************************/
/**
 * @brief    This function stops the Adaptive-DCI-Control.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdciStop
(
    CamEngineHandle_t hCamEngine
);

/*****************************************************************************/
/**
 * @brief    This functions configures the Adaptive-DCI-Control
 *            manual mode parameters.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdciConfigure
(
    CamEngineHandle_t                hCamEngine,      /**< handle CamEngine */
    const CamEngineDciConfig_t        *pDciConfig
);

/*****************************************************************************/
/**
 * @brief    This functions set the Adaptive-DCI-Control auto mode table.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdciSetAutoTable
(
    CamEngineHandle_t                    hCamEngine,         /**< handle CamEngine */
    const CamEngineAdciParamNode_t        *pParamNodeTable,    /**< CamEngineAdciParamNode_t */
    const uint8_t                        nodeNum             /**< nodeNum */
);

/*****************************************************************************/
/**
 * @brief    This function returns the Adaptive-DCI-Control status.
 *
 * @param    hCamEngine            Handle to the CamEngine instance.
 * @param    pRunning            BOOL_TRUE: running, BOOL_FALSE: stopped
 *
 * @return    Return the result of the function call.
 * @retval    RET_SUCCESS         function succeed
 * @retval    RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineAdciStatus
(
    CamEngineHandle_t            hCamEngine,
    bool_t                        *pRunning,
    CamEngineAdciMode_t            *pMode,             /**< run-mode */
    CamEngineAdciStatus_t        *pCurrentStatus
);

/*****************************************************************************/
/**
 * @brief   This functions get otp data
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGetOtpData
(
    CamEngineHandle_t        hCamEngine      /**< handle CamEngine */
);


#define SENSOR_ID_LEN 11
#define MODULE_SN_LEN 12
typedef struct CamEngineOtpModuleInfo_s
{
  uint16_t HWVersion;
  uint16_t EEPROMRevision;
  uint16_t SensorRevision;
  uint16_t TLensRevision;
  uint16_t IRCFRevision;
  uint16_t LensRevision;
  uint16_t CARevision;
  uint16_t ModuleInteID;
  uint16_t FactoryID;
  uint16_t MirrorFlip;
  uint16_t TLensSlaveID;
  uint16_t EEPROMSlaveID;
  uint16_t SensorSlaveID;
  uint8_t  SensorID[SENSOR_ID_LEN];
  uint16_t ManuDateYear;
  uint16_t ManuDateMonth;
  uint16_t ManuDateDay;
  uint8_t  BarcodeModuleSN[MODULE_SN_LEN];
  uint16_t MapTotalSize;
} CamEngineOtpModuleInfo_t;


/*****************************************************************************/
/**
 * @brief   This functions get otp module info
 *
 * @param   hCamEngine          Handle to the CamEngine instance.
 * @param   OtpModuleInfo       Otp module info struct
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 *
 *****************************************************************************/
RESULT CamEngineGetOtpModuleInfo
(
    CamEngineHandle_t        hCamEngine,      /**< handle CamEngine */
    CamEngineOtpModuleInfo_t *OtpModuleInfo
);

#ifdef __cplusplus
}
#endif


/* @} cam_engine_aaa_api */


#endif /* __CAM_ENGINE_AAA_API_H__ */

