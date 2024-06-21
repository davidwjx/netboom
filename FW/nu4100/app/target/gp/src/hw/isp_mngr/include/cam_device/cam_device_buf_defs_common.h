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

#ifndef __CAMERA_DEVICE_BUF_DEFS_COMMON_H__
#define __CAMERA_DEVICE_BUF_DEFS_COMMON_H__

#include <stdint.h>

enum CAMERA_PIX_FMT_e{
    CAMERA_PIX_FMT_YUV422SP = 0,
    CAMERA_PIX_FMT_YUV422I,
    CAMERA_PIX_FMT_YUV420SP,
    CAMERA_PIX_FMT_YUV444P,
    CAMERA_PIX_FMT_YUV444I,
    CAMERA_PIX_FMT_RGB888,
    CAMERA_PIX_FMT_RGB888P,
    CAMERA_PIX_FMT_RAW8,
    CAMERA_PIX_FMT_RAW10,
    CAMERA_PIX_FMT_RAW10_ALIGNED_MODE0,
    CAMERA_PIX_FMT_RAW10_ALIGNED_MODE1,
    CAMERA_PIX_FMT_RAW12,
    CAMERA_PIX_FMT_RAW12_ALIGNED_MODE0,
    CAMERA_PIX_FMT_RAW12_ALIGNED_MODE1,
    CAMERA_PIX_FMT_RAW14,
    CAMERA_PIX_FMT_RAW14_ALIGNED_MODE0,
    CAMERA_PIX_FMT_RAW14_ALIGNED_MODE1,
    CAMERA_PIX_FMT_RAW16,
    CAMERA_PIX_FMT_MAX,
};

enum INPUT_IMAGE_LAYOUT_e{
	INPUT_IMAGE_LAYOUT_RGGB = 0,
	INPUT_IMAGE_LAYOUT_GRBG,
	INPUT_IMAGE_LAYOUT_GBRG,
	INPUT_IMAGE_LAYOUT_BGGR,
	INPUT_IMAGE_LAYOUT_MAX,
};

typedef enum _ISPCORE_BUFIO_ID_ {
    ISPCORE_BUFIO_MP        = 0,
    ISPCORE_BUFIO_SP1       = 1,
    ISPCORE_BUFIO_SP2       = 2,
    ISPCORE_BUFIO_MCMWR     = 3,
#ifdef ISP_MI_HDR
    ISPCORE_BUFIO_HDR,
#endif
    ISPCORE_BUFIO_WRITEMAX,
    ISPCORE_BUFIO_READ,
    ISPCORE_BUFIO_MAX
} ISPCORE_BUFIO_ID;

typedef struct __BufIdentity__ {
    uint32_t buff_address;     //buffer physical address
    uint32_t buff_size;       // biffer size
    int width;
    int height;
    int format;
    uint64_t ipl_address;     //ISP Platform Layer address
} BufIdentity;

#endif  // __CAMERA_DEVICE_BUF_DEFS_COMMON_H__