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

#ifndef __CAM_DEVICE_CLB_ITF__WB_HPP__
#define __CAM_DEVICE_CLB_ITF__WB_HPP__

namespace clb_itf {

struct Wb{

  struct Config {
    inline void reset(){
      REFSET(*this, 0);

      int32_t i = 0;

      ccMatrix.Coeff[i++] = 1.586;
      ccMatrix.Coeff[i++] = -0.070;
      ccMatrix.Coeff[i++] = -0.461;
      ccMatrix.Coeff[i++] = -0.609;
      ccMatrix.Coeff[i++] = 1.984;
      ccMatrix.Coeff[i++] = -0.234;
      ccMatrix.Coeff[i++] = 0.008;
      ccMatrix.Coeff[i++] = -1.219;
      ccMatrix.Coeff[i++] = 2.508;

      wbGains.Blue = 2.930;
      wbGains.GreenB = 1.016;
      wbGains.GreenR = 1.016;
      wbGains.Red = 1.367;
    };

    CamEngineCcMatrix_t ccMatrix;
    CamEngineCcOffset_t ccOffset;
    CamEngineWbGains_t wbGains;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF__WB_HPP__
