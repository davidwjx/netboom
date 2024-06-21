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

#ifndef __CAM_DEVICE_CLB_ITF_COMMON_HPP__
#define __CAM_DEVICE_CLB_ITF_COMMON_HPP__

#include "cam_device_ispcore_defs.h"

#ifdef REFSET
#undef REFSET
#endif
#define REFSET(_DST_REF_, _VAL_) memset(&_DST_REF_, _VAL_, sizeof(_DST_REF_))

#ifdef REFCPY
#undef REFCPY
#endif
#define REFCPY(_DST_REF_, _SRC_) memcpy(&_DST_REF_, _SRC_, sizeof(_DST_REF_))

namespace clb_itf {

struct RoiWindow {
    unsigned int xStart;    // horizontal start position
    unsigned int yStart;    // vertical start position
    unsigned int width;     // rect width
    unsigned int height;    // rect height
    float weight;    // weight
};

struct CamDeviceWindow
{
    uint16_t    hOffset;
    uint16_t    vOffset;
    uint16_t    width;
    uint16_t    height;
};

}// namespace camdevice_clb

#endif    // __CAM_DEVICE_CLB_ITF_COMMON_HPP__

