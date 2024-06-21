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

#ifndef __CAM_DEVICE_CLB_ITF_AHDR_HPP__
#define __CAM_DEVICE_CLB_ITF_AHDR_HPP__

namespace clb_itf {

struct Ahdr{
  enum Generation { V1, VMax };

  struct Config {
    
      void reset(){
        REFSET(*this, 0);

#if defined(CTRL_AHDR)
        c = 20;
        c2 = 120;
        ceil = 1.0;
        setPoint2 = 170.0;
        tolerance = 15;
        objectFector = 0.6;
        ratio.max = 64.0;
        ratio.min = 4.0;
#endif // CTRL_AHDR
      };

      bool isAuto;
      bool isAutoSetPoint;
      bool isAutoSetPoint2;

      float c, c2, ceil, objectFector, setPoint2, tolerance;

      struct {
        float max, min;
      } ratio;

  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_AHDR_HPP__

