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

#ifndef __CAM_DEVICE_CLB_ITF_CPROC_HPP__
#define __CAM_DEVICE_CLB_ITF_CPROC_HPP__

namespace clb_itf {

struct Cproc{
  struct Config {
    inline void reset(){
      REFSET(*this, 0);

      config.ChromaOut = CAMERIC_CPROC_CHROM_RANGE_OUT_FULL_RANGE;
      config.LumaOut = CAMERIC_CPROC_LUM_RANGE_OUT_FULL_RANGE;
      config.LumaIn = CAMERIC_CPROC_LUM_RANGE_IN_FULL_RANGE;

      config.contrast = 1.6;
      config.brightness = 50;
      config.saturation = 1.4;
      config.hue = 20;
    };

    CamEngineCprocConfig_t config;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_CPROC_HPP__
