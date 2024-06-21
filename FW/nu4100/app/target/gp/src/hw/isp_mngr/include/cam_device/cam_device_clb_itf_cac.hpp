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

#ifndef __CAM_DEVICE_CLB_ITF_CAC_HPP__
#define __CAM_DEVICE_CLB_ITF_CAC_HPP__

namespace clb_itf {

struct Cac{
  struct Status {
    inline void reset(){ REFSET(*this, 0); };

    int32_t hCenterOffset;
    int32_t vCenterOffset;

    enum {
      Blue,
      Red,

      Sum,
    };

    struct Coeff {
      double linear;
      double square;
      double cubical;
    } coeff[Sum];
  };
};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_CAC_HPP__

