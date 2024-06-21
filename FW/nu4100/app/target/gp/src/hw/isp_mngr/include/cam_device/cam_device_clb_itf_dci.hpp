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

#ifndef __CAM_DEVICE_CLB_ITF_DCI_HPP__
#define __CAM_DEVICE_CLB_ITF_DCI_HPP__

namespace clb_itf {

struct Dci{
  struct Config {
    inline void reset(){ REFSET(*this, 0);
      isAuto = false;
      start.x = 0;
      start.y = 0;
      inflection.x = 20;
      inflection.y = 100;
      inflection2.x = 40;
      inflection2.y = 200;
      end.x = 64;
      end.y = 500;
      mode = Mode3Sections;
      exponent.low = 1.0;
      exponent.middle = 1.0;
      exponent.high = 1.0;
     };

    enum Mode {
      Mode2Sections,
      Mode3Sections,
      ModeHistogram,

      ModeSum,
    } mode;

    struct Exponent {
      float high;
      float low;
      float middle;
    } exponent;

    struct Histogram {
      float scale;
      float step;
      float base;
      float strength;
      float damp;
    } histogram;

    bool isAuto;

    struct Point {
      uint16_t x;
      uint16_t y;
    } start, inflection, inflection2, end;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_DCI_HPP__
