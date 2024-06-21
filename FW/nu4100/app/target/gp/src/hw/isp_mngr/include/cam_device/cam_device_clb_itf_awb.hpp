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

#ifndef __CAM_DEVICE_CLB_ITF_AWB_HPP__
#define __CAM_DEVICE_CLB_ITF_AWB_HPP__

namespace clb_itf {

struct Awb{
  struct Config {
    inline void reset(){
      REFSET(*this, 0);

      illuminationProfileIndex = 0;
      isAuto = true;
      isDamping = true;
    };

    int32_t illuminationProfileIndex;

    bool isAuto;
    bool isDamping;

    int32_t profilesCount;

#define AWB_PROFILE_COUNT 10
#define AWB_PROFILE_NAME_LENGTH 20
    char profilesName[AWB_PROFILE_COUNT][AWB_PROFILE_NAME_LENGTH];
  };

  struct Status {
    double indoorMin;
    double max;
    double maxSky;
    double outdoorMin;
  };

  struct Window {
    uint16_t hOffset;
    uint16_t vOffset;
    uint16_t width;
    uint16_t height;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_AWB_HPP__
