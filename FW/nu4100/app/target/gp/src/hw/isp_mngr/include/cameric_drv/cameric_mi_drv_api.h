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

#ifndef __CAMERIC_MI_DRV_API_H__
#define __CAMERIC_MI_DRV_API_H__

/**
 * @file cameric_mi_drv_api.h
 *
 * @brief   This file contains the CamerIC MI driver API definitions.
 *
 *****************************************************************************/
/**
 * @cond cameric_mi
 *
 * @defgroup cameric_mi_drv_api CamerIc MI Driver API definitions
 * @{
 *
 * The Memory Interface module is able to write data from the main picture path
 * to the system memory. For the main picture path alternatively one of raw data,
 * main picture path resize output paths could be used. The MI
 * module is also able to write data from the self picture path to the system memory.
 * Further on the DMA read port path could be used to read an image from the system
 * memory, e.g. as input for the Super Impose (SI) module. The Memory Interface Driver
 * serves as an abstraction layer, so the application does not need to know which bit
 * has to be set where in the registers of the MI module.
 *
 * @image html mi.png "Overview of the CamerIC MI driver" width=\textwidth
 * @image latex mi.png "Overview of the CamerIC MI driver" width=\textwidth
 *
 * The Memory interface module driver also abstracts the scaler-units in the
 * main and self path (see MRSZ or SRSZ).
 *
 */
#include <ebase/types.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CAMERIC_ALIGN_SIZE          ALIGN_SIZE_1K
#define CAMERIC_MI_ALIGN            ALIGN_UP_1K



/*****************************************************************************/
/**
 * @brief   Enumeration type to identify the CamerIC output path
 *
 *****************************************************************************/
typedef enum CamerIcMiPath_e
{
    CAMERIC_MI_PATH_INVALID         = -1,       /**< lower border (only for an internal evaluation) */
    CAMERIC_MI_PATH_MAIN            =  0,       /**< main path index */
    CAMERIC_MI_PATH_SELF            =  1,       /**< self path index */
    CAMERIC_MI_PATH_SELF2_BP           =  2,       /**< self path index */
    CAMERIC_MI_PATH_MCM_WR0          ,       /**<mcm wr0 path index */
    CAMERIC_MI_PATH_MCM_WR1          ,       /**<mcm wr1 path index */
    CAMERIC_MI_PATH_MCM_WR2          ,       /**<mcm wr2 path index */
    CAMERIC_MI_PATH_MCM_WR3          ,       /**<mcm wr3 path index */
#ifdef ISP_MI_PP_WRITE
    CAMERIC_MI_PATH_POST_PROCESS          ,       /**< post process path index */
#endif
#ifdef ISP_MI_HDR
    CAMERIC_MI_PATH_HDR_L                   ,       /**< mi hdr long path index */
    CAMERIC_MI_PATH_HDR_S                   ,       /**< mi hdr short path index */
    CAMERIC_MI_PATH_HDR_VS                  ,       /**< mi hdr very short path index */
#ifdef ISP_HDR_2_SENSORS
    CAMERIC_MI_PATH_HDR1_L                   ,       /**< mi hdr1 long path index */
    CAMERIC_MI_PATH_HDR1_S                   ,       /**< mi hdr1 short path index */
    CAMERIC_MI_PATH_HDR1_VS                  ,       /**< mi hdr1 very short path index */
#endif
#endif
    CAMERIC_MI_PATH_MAX                         /**< upper border (only for an internal evaluation) */
} CamerIcMiPath_t;




/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the data output mode on a given path
 *
 * @note    This enumeration type is used to specify the organization of the image data
 *          for each pixel, whether in planar format or packed format
 *
 *****************************************************************************/
typedef enum CamerIcMiDataMode_e
{
    CAMERIC_MI_DATAMODE_INVALID         = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_MI_DATAMODE_DISABLED        = 1,    /**< disables the path */
    CAMERIC_MI_DATAMODE_JPEG            = 2,    /**< data output format is JPEG (only valid for mainpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_YUV444          = 3,    /**< data output format is YUV444 */
    CAMERIC_MI_DATAMODE_YUV422          = 4,    /**< data output format is YUV422 */
    CAMERIC_MI_DATAMODE_YUV420          = 5,    /**< data output format is YUV420 */
    CAMERIC_MI_DATAMODE_YUV400          = 6,    /**< data output format is YUV400 */
    CAMERIC_MI_DATAMODE_RGB888          = 7,    /**< data output format is RGB888 (only valid for selpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_RGB666          = 8,    /**< data output format is RGB666 (only valid for selpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_RGB565          = 9,    /**< data output format is RGB565 (only valid for selpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_RAW8            = 10,   /**< data output format is RAW8 (only valid for mainpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_RAW12           = 11,   /**< data output format is RAW12 (only valid for mainpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_DPCC            = 12,   /**< path dumps out the current measured defect pixel table */
    CAMERIC_MI_DATAMODE_RAW10           = 13,   /**< data output format is RAW10 (only valid for mainpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_RAW14           = 14,   /**< data output format is RAW14 (only valid for mainpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_RAW16           = 15,   /**< data output format is RAW16 (only valid for mainpath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_META            = 16,   /**< data output format is META (only valid for metapath @ref CamerIcMiPath_e) */
    CAMERIC_MI_DATAMODE_MAX                     /**< upper border (only for an internal evaluation) */
} CamerIcMiDataMode_t;



/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the data storage layout
 *
 * @note    This enumeration type is used to specify the organization of the
 *          image data for each pixel, whether in planar format or packed
 *          format. The valid domain is bordered from
 *          CAMERIC_MI_DATASTORAGE_INVALID to CAMERIC_MI_DATASTORAGE_MAX.
 *
 *****************************************************************************/
typedef enum CamerIcMiDataLayout_e
{
    CAMERIC_MI_DATASTORAGE_INVALID      = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_MI_DATASTORAGE_PLANAR       = 1,    /**< Y values for all pixels are put together, as well as U and V,
                                                     like: YYYYYY......, UUUUUUU......., VVVVVV...... */
    CAMERIC_MI_DATASTORAGE_SEMIPLANAR   = 2,    /**< YUV values are packed together as: YYYY......, UVUVUVUV...... */
    CAMERIC_MI_DATASTORAGE_INTERLEAVED  = 3,    /**< YUV values are packed together as: UYVY UYVY...... */
    CAMERIC_MI_DATASTORAGE_MAX                  /**< upper border (only for an internal evaluation) */
} CamerIcMiDataLayout_t;

/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the data storage align mode.
 *
 * @note    This enumeration type is used to specify the align mode of the
 *          image data for each pixel
 *
 *****************************************************************************/
typedef enum CamerIcMiDataAlignMode_e {
	CAMERIC_MI_PIXEL_ALIGN_INVALID = -1,	/**< lower border (only for an internal evaluation) */
	CAMERIC_MI_PIXEL_UN_ALIGN = 0,		/**< The value of pixel is not align. */
	CAMERIC_MI_PIXEL_ALIGN_128BIT = 1,	/**< The value of pixel raw is align with 128 bit.*/
	CAMERIC_MI_PIXEL_ALIGN_DOUBLE_WORD = 1, /**< The value of yuv pixel is double word align.*/
	CAMERIC_MI_PIXEL_ALIGN_WORD = 2,  /**< The value of yuv pixel is  word align.*/
	CAMERIC_MI_PIXEL_ALIGN_16BIT = 2, /**< The value of raw pixel is align with 16bit.*/
	CAMERIC_MI_PIXEL_ALIGN_MAX	  /**< upper border (only for an internal evaluation) */
} CamerIcMiDataAlignMode_t;

/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the data storage yuv bit width.
 *
 * @note    This enumeration type is used to specify the yuv bit width of the
 *          image data for each pixel
 *
 *****************************************************************************/
typedef enum CamerIcMiDataYuvBitWidth_e {
	CAMERIC_MI_PIXEL_YUV_BIT_INVALID = -1, /**< lower border (only for an internal evaluation) */
	CAMERIC_MI_PIXEL_YUV_8_BIT = 0,		/**< The value of yuv pixel is 8 bit. */
	CAMERIC_MI_PIXEL_YUV_10_BIT = 1,	/**< The value of yuv pixel is 10 bit.*/
	CAMERIC_MI_PIXEL_YUV_12_BIT = 2,	/**< The value of yuv pixel is 12 bit.*/
	CAMERIC_MI_PIXEL_YUV_BIT_MAX		/**< upper border (only for an internal evaluation) */
} CamerIcMiDataYuvBitWidth_t;

/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the burst length for writing to or
 *          reading from memory
 *
 *****************************************************************************/
typedef enum CamerIcMiBurstLength_e
{
    CAMERIC_MI_BURSTLENGTH_INVALID        = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_MI_BURSTLENGTH_4            = 1,    /**< burstlength of 4 or smaller */
    CAMERIC_MI_BURSTLENGTH_8            = 2,    /**< burstlength of 8 or smaller */
    CAMERIC_MI_BURSTLENGTH_16           = 3,    /**< burstlength of 16 or smaller */
    CAMERIC_MI_BURSTLENGTH_MAX                  /**< upper border (only for an internal evaluation) */
} CamerIcMiBurstLength_t;


#ifdef ISP_MI_FIFO_DEPTH_NANO
/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the MI fifo depth of ISP Nano
 *
 *
 *****************************************************************************/
typedef enum CamerIcMiMpOutputFifoDepth_e
{
    CAMERIC_MI_MP_OUTPUT_FIFO_DEPTH_FULL  = 0,   /** 00: FULL(2KBytes) */
    CAMERIC_MI_MP_OUTPUT_FIFO_DEPTH_HALF  = 1,   /** 01: HALF(1KBytes) */
    CAMERIC_MI_MP_OUTPUT_FIFO_DEPTH_1_4   = 2,   /** 10: 1/4(512Bytes) */
    CAMERIC_MI_MP_OUTPUT_FIFO_DEPTH_1_8   = 3,   /** 11: 1/8(256Bytes) */
    CAMERIC_MI_MP_OUTPUT_FIFO_DEPTH_MAX          /**< upper border (only for an internal evaluation) */
}CamerIcMiMpOutputFifoDepth_t;
#endif

#ifdef ISP_MI_ALIGN_NANO
/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the MI output MSB/LSB align of ISP Nano
 *
 *
 *****************************************************************************/
typedef enum CamerIcMiMpOutputLsbAlign_e
{
    CAMERIC_MI_MP_OUTPUT_LSB_ALIGN_MSB    = 0,  /** 0: MSB aligned for RAW10 and RAW12 formats */
    CAMERIC_MI_MP_OUTPUT_LSB_ALIGN_LSB    = 1,  /** 1: LSB aligned for RAW10 and RAW12 formats */
    CAMERIC_MI_MP_OUTPUT_LSB_ALIGN_MAX          /**< upper border (only for an internal evaluation) */
}CamerIcMiMpOutputLsbAlign_t;
#endif

#ifdef ISP_MI_BYTESWAP
/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the MI output Byte Swap settings
 * bit 0 to swap bytes
 * bit 1 to swap words
 * bit 2 to swap dwords
 *
 *****************************************************************************/
typedef enum CamerIcMiMpOutputByteSwap_e
{
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_NORMAL            = 0,  /** 3'b000: ABCDEFGH => ABCDEFGH */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_BYTE              = 1,  /** 3'b001: ABCDEFGH => BADCFEHG */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_WORD              = 2,  /** 3'b010: ABCDEFGH => CDABGHEF */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_WORD_BYTE         = 3,  /** 3'b011: ABCDEFGH => DCBAHGFE */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_DWORD             = 4,  /** 3'b100: ABCDEFGH => EFGHABCD */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_DWORD_BYTE        = 5,  /** 3'b101: ABCDEFGH => FEHGBADC */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_DWORD_WORD        = 6,  /** 3'b110: ABCDEFGH => GHEFCDAB */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_DWORD_WORD_BYTE   = 7,  /** 3'b111: ABCDEFGH => HGFEDCBA */
    CAMERIC_MI_MP_OUTPUT_BYTE_SWAP_MAX                     /**< upper border (only for an internal evaluation) */
}CamerIcMiMpOutputByteSwap_t;
#endif


#ifdef ISP_DEC
//#ifdef HAL_CMODEL

typedef struct CamerIcCmpPrm_s{
    char*  dstFileName;
}CamerIcCmpPrm_t;
//#endif
#endif

/*****************************************************************************/
/**
 * @brief   Enumeration type to specify the picture orientation on the self
 *          path
 *
 *****************************************************************************/
//! self picture operating modes
typedef enum  CamerIcMiOrientation_e
{
    CAMERIC_MI_ORIENTATION_INVALID          = 0,    /**< lower border (only for an internal evaluation) */
    CAMERIC_MI_ORIENTATION_ORIGINAL         = 1,    /**< no rotation, no horizontal or vertical flipping */
    CAMERIC_MI_ORIENTATION_VERTICAL_FLIP    = 2,    /**< vertical   flipping (no additional rotation) */
    CAMERIC_MI_ORIENTATION_HORIZONTAL_FLIP  = 3,    /**< horizontal flipping (no additional rotation) */
    CAMERIC_MI_ORIENTATION_ROTATE90         = 4,    /**< rotation  90 degrees ccw (no additional flipping) */
    CAMERIC_MI_ORIENTATION_ROTATE180        = 5,    /**< rotation 180 degrees ccw (equal to horizontal plus vertical flipping) */
    CAMERIC_MI_ORIENTATION_ROTATE270        = 6,    /**< rotation 270 degrees ccw (no additional flipping) */

#if 0
    eMrvMifSp_Rot_090_V_Flip   = 6, //!< rotation  90 degrees ccw plus vertical flipping
    eMrvMifSp_Rot_270_V_Flip   = 7  //!< rotation 270 degrees ccw plus vertical flipping
#endif

    CAMERIC_MI_ORIENTATION_MAX                      /**< upper border (only for an internal evaluation) */
} CamerIcMiOrientation_t;



/*****************************************************************************/
/**
 * @brief   This function registers a Request-Callback at the CamerIC Memory
 *          Interface Module. A request callback is called if the driver
 *          needs an interaction from the application layer (i.e. a new image
 *          buffer to fill, please also see @ref CamerIcRequestId_e).
 *
 * @param   handle              CamerIc driver handle
 * @param   func                Callback function
 * @param   pUserContext        User-Context
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_BUSY            already a callback registered
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_INVALID_PARM    given parameter is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to register a
 *                              request callback
 *
 *****************************************************************************/
extern RESULT CamerIcMiRegisterRequestCb
(
    CamerIcDrvHandle_t      handle,
    CamerIcRequestFunc_t    func,
    void                     *pUserContext
);

/*****************************************************************************/
/**
 * @brief   This function registers a Request-Callback at the CamerIC Memory
 *          Interface Module. A request callback is called if the driver
 *          needs an interaction from the application layer (i.e. a new image
 *          buffer to fill, please also see @ref CamerIcRequestId_e).
 *
 * @param   handle              CamerIc driver handle
 * @param   func                Callback function
 * @param   pUserContext        User-Context
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_BUSY            already a callback registered
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_INVALID_PARM    given parameter is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to register a
 *                              request callback
 *
 *****************************************************************************/
extern RESULT CamerIcMiRegisterRequestCbForUser
(
    CamerIcDrvHandle_t       handle,
    CamerIcRequestFuncUser_t func,
    void                     *pUserContext
);


/*****************************************************************************/
/**
 * @brief   This functions deregisters/releases a registered Request-Callback
 *          at CamerIC Memory Interface Module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              request callback
 *
 *****************************************************************************/
extern RESULT CamerIcMiDeRegisterRequestCb
(
    CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief	This functions deregisters/releases a registered Request-Callback
 *			at CamerIC Memory Interface Module.
 *
 * @param	handle				CamerIc driver handle
 *
 * @return						Return the result of the function call.
 * @retval	RET_SUCCESS 		operation succeded
 * @retval	RET_WRONG_HANDLE	given handle is invalid
 * @retval	RET_WRONG_STATE 	driver is in wrong state to deregister the
 *								request callback
 *
 *****************************************************************************/
extern RESULT CamerIcMiDeRegisterRequestCbForUser
(
	CamerIcDrvHandle_t handle
);


/*****************************************************************************/
/**
 * @brief   This functions registers an Event-Callback at CamerIC Memory
 *          Interface Module. An event callback is called if the driver
 *          needs to inform the application layer about an asynchronous event
 *          or an error situation (i.e. please also see @ref CamerIcEventId_e).
 *
 * @param   handle              CamerIc driver handle
 * @param   func                Callback function
 * @param   pUserContext        User-Context
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_BUSY            already a callback registered
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_INVALID_PARM    given parameter is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to register a
 *                              event callback
 *
 *****************************************************************************/
extern RESULT CamerIcMiRegisterEventCb
(
    CamerIcDrvHandle_t  handle,
    CamerIcEventFunc_t  func,
    void                 *pUserContext
);



/*****************************************************************************/
/**
 * @brief   This functions deregisters/releases a registered Event-Callback
 *          at CamerIC Memory Interface Module.
 *
 * @param   handle              CamerIc driver handle
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to deregister the
 *                              event callback
 *
 *****************************************************************************/
extern RESULT CamerIcMiDeRegisterEventCb
(
    CamerIcDrvHandle_t handle
);



/*****************************************************************************/
/**
 * @brief   This functions configures the burstlength to use for luminance
 *          and chrominance planes.
 *
 * @note    It could be more efficient to configure different burstlength
 *          depending on the pixel subsampling (YUV422, YUV400, ... )
 *
 * @param   handle              CamerIc driver handle
 * @param   y_burstlength       burstlegth of luminace plane
 * @param   c_burstlength       burstlegth of chrominace plane
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to set burstlength
 *
 *****************************************************************************/
extern RESULT CamerIcMiSetBurstLength
(
    CamerIcDrvHandle_t              handle,
    const CamerIcMiBurstLength_t    y_burstlength,
    const CamerIcMiBurstLength_t    c_burstlength
);



/*****************************************************************************/
/**
 * @brief   This function configures the input and output resolution of
 *          a given path.
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   in_width            input width
 * @param   in_height           input height
 * @param   out_width           output width
 * @param   out_height          output height
 * @param   in_mode             mi input mode (@ref CamerIcMiDataMode_e)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to set burstlength
 *
 *****************************************************************************/
extern RESULT CamerIcMiSetResolution
(
    CamerIcDrvHandle_t          handle,
    const CamerIcMiPath_t       path,
    const uint32_t              in_width,
    const uint32_t              in_height,
    const uint32_t              out_width,
    const uint32_t              out_height,
    const uint32_t              in_mode
);



/*****************************************************************************/
/**
 * @brief   This function configures the data mode for the given path.
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   mode                Data output mode (@ref CamerIcMiDataMode_e)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
extern RESULT CamerIcMiSetDataMode
(
    CamerIcDrvHandle_t          handle,
    const CamerIcMiPath_t       path,
    const CamerIcMiDataMode_t   mode
);

/*****************************************************************************/
/**
 * @brief   This function configures the pixel data align mode for the given path.
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   alignMode           Data output align mode (@ref CamerIcMiDataAlignMode_e)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/

extern RESULT CamerIcMiSetDataAlignMode
(
    CamerIcDrvHandle_t          handle,
    const CamerIcMiPath_t       path,
    const CamerIcMiDataAlignMode_t alignMode
);

/*****************************************************************************/
/**
 * @brief   This function configures the pixel data yuv output bit for the given path.
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   yuv_bit             0: 8 bit YUV/RGB; 1: 10 bit YUV/RGB
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/

extern RESULT CamerIcMiSetDataYUVMode
(
    CamerIcDrvHandle_t          handle,
    const CamerIcMiPath_t       path,
    const bool_t                yuv_bit
);

#ifdef ISP_MI_FIFO_DEPTH_NANO
/*****************************************************************************/
/**
 * @brief   Register read function configures the MP output fifo depth
 *
 * @param   handle              CamerIc driver handle
 * @param   address             register address of the fifo depth
 * @param   data                register data of the fifo depth
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
RESULT CamerIcMiMpOutputFifoDepthRegRead
(
    CamerIcDrvHandle_t handle,
    unsigned int * address,
    unsigned int * data
);

/*****************************************************************************/
/**
 * @brief   This function configures the MP output fifo depth
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   depth               Fifo depth mode (@ref CamerIcMiMpOutputFifoDepth_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
RESULT CamerIcMiMpOutputFifoSetDepth
(
    CamerIcDrvHandle_t                   handle,
    const CamerIcMiPath_t                path,
    const CamerIcMiMpOutputFifoDepth_t   depth
);
#endif

#ifdef ISP_MI_ALIGN_NANO
/*****************************************************************************/
/**
 * @brief   This function configures the MP output alignment configuration
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   align               lsb align cfg(@ref CamerIcMiMpOutputLsbAlign_t)
 * @param   swap                byte swap set(@ref CamerIcMiMpOutputByteSwap_t)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
RESULT CamerIcMiMpAlignSetCfg
(
    CamerIcDrvHandle_t                   handle,
    const CamerIcMiPath_t                path,
    const CamerIcMiMpOutputLsbAlign_t    align,
    const CamerIcMiMpOutputByteSwap_t    swap
);
#endif


/*****************************************************************************/
/**
 * @brief   Dump configuration function for MI media buffer dump.
 *
 * @param   hCamerIcDrv          handle to the CamerIcDrvHandle_t instance
 * @param   path                MI path cfg
 * @param   dump_name        dump file name
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_NULL_POINTER    handle is NULL
 * @retval  RET_FAILURE         module not available by driver or hardware
 *
 *****************************************************************************/
extern RESULT CamerIcMiDumpBufCfg
(
    CamerIcDrvHandle_t       hCamerIcDrv,
    const CamerIcMiPath_t   path,
    const char *            dump_name
);

/*****************************************************************************/
/**
 * @brief   Call Dump function for MI media buffer dump to file.
 *
 * @param   hCamerIcDrv          handle to the CamerIcDrvHandle_t instance
 * @param   path                MI path cfg
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_WRONG_CONFIG    FP handle not configured
 * @retval  RET_FAILURE         module not available by driver or hardware
 *
 *****************************************************************************/
extern RESULT CamerIcMiDumpToFile
(
    CamerIcDrvHandle_t       hCamerIcDrv,
    const CamerIcMiPath_t   path
);

#ifdef ISP_DEC
//#ifdef HAL_CMODEL

/*****************************************************************************/
/**
 * @brief   Call Dump function for MI media buffer compress dump to file.
 *
 * @param   hCamEngine          handle to the CamerIcDrvHandle_t instance
 * @param   path                MI path cfg
 * @param   CamerIcCmpPrm_t     compress parm
 *
 * @return  Return the result of the function call.
 * @retval  RET_SUCCESS         function succeed
 * @retval  RET_WRONG_HANDLE    invalid instance handle
 * @retval  RET_WRONG_CONFIG    FP handle not configured
 * @retval  RET_FAILURE         module not available by driver or hardware
 *
 *****************************************************************************/
extern RESULT CamerIcMiCompressToFile
(
    CamerIcDrvHandle_t       hCamerIcDrv,
    const CamerIcMiPath_t    path,
    const CamerIcCmpPrm_t  *pParm
);
//#endif
#endif

/*****************************************************************************/
/**
 * @brief   This function configures the data layout for the given path.
 *
 * @param   handle              CamerIc driver handle
 * @param   path                Path index of CamerIC (@ref CamerIcMiPath_e)
 * @param   layout              Data layout (@ref CamerIcMiDataLayout_e)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
extern RESULT CamerIcMiSetDataLayout
(
    CamerIcDrvHandle_t          handle,
    const CamerIcMiPath_t       path,
    const CamerIcMiDataLayout_t layout
);



/*****************************************************************************/
/**
 * @brief   This function configures the picture orientation for the
 *          Self path.
 *
 * @param   handle              CamerIc driver handle
 * @param   path                mi path (@ref CamerIcMiPath_t)
 * @param   orientation         Picture orientation (@ref CamerIcMiDataLayout_e)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
extern RESULT CamerIcMiSetPictureOrientation
(
    CamerIcDrvHandle_t              handle,
    const CamerIcMiPath_t           path,
    const CamerIcMiOrientation_t    orientation
);



/*****************************************************************************/
/**
 * @brief   This function configures the picture orientation for the
 *          Self path.
 *
 * @param   handle              CamerIc driver handle
 * @param   numFramesToSkip     Number of frames to skip (e.g. after sensor resolution change)
 *
 * @return                      Return the result of the function call.
 * @retval  RET_SUCCESS         operation succeded
 * @retval  RET_FAILURE         common error occured
 * @retval  RET_WRONG_HANDLE    given handle is invalid
 * @retval  RET_WRONG_STATE     driver is in wrong state to change datamode
 * @retval  RET_INVALID_PARM    invalid parameter
 *
 *****************************************************************************/
extern RESULT CamerIcMiSetFramesToSkip
(
    CamerIcDrvHandle_t              handle,
    uint32_t                        numFramesToSkip
);

#ifdef ISP_MI_PP_WRITE

/*****************************************************************************/
/**
 * @brief  config the post process write line number
 *
 * @param   handle  cameric drv handle
 * @param   num  line nume
 * @return                    Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcMiSetPpwLineNum(CamerIcDrvHandle_t handle, uint16_t num);

/*****************************************************************************/
/**
 * @brief  get the post process write line count
 *
 * @param   handle  cameric drv handle
 * @param   pCnt   pointer to the counter
 * @return                    Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcMiGetPpwLineCnt(CamerIcDrvHandle_t handle, uint16_t *pCnt);

/*****************************************************************************/
/**
 * @brief  set post process path
 * @param   handle  cameric drv handle
 * @return                    Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcMiSetPpPathConfig(CamerIcDrvHandle_t handle);

#endif

#ifdef ISP_MI_PP_READ
/*****************************************************************************/
/**
 * @brief  set the post process dma line and buf number
 *
 * @param   handle  cameric drv handle
 * @param   bufNum   the line count in all entries
 * @param   lineNum  the line count in all entries
 * @return                    Return the result of the function call.
 * @retval  RET_SUCCESS
 * @retval  RET_FAILURE
 *
 *****************************************************************************/
RESULT CamerIcMiCfgPpDmaLineEntry(CamerIcDrvHandle_t handle, uint16_t bufNum, uint16_t lineNum);
#endif


#ifdef __cplusplus
}
#endif

/* @} cameric_mi_drv_api */

/* @endcond */

#endif /* __CAMERIC_MI_DRV_API_H__ */

