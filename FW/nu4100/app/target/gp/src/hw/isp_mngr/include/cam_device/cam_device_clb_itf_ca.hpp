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

#ifndef __CAM_DEVICE_CLB_ITF_CA_HPP__
#define __CAM_DEVICE_CLB_ITF_CA_HPP__

namespace clb_itf {

struct Ca{
  struct Config {
    inline void reset(){
      isAuto = false;
      mode = Brightness;
      curve = S;

      curveSInflection = 0;
      curveSExponent = 20;
      curveParabolicFactor = 1.0;
    };

    enum Curve {
      S,
      Parabolic,
    } curve;

    enum Mode {
      Brightness,
      Saturation,
      BrightnessSaturation,
    } mode;

    int32_t curveSInflection;
    int32_t curveSExponent;
    double curveParabolicFactor;

    bool isAuto;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_CA_HPP__

