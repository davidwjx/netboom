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

#ifndef __CAM_DEVICE_CLB_ITF_FILTER_HPP__
#define __CAM_DEVICE_CLB_ITF_FILTER_HPP__

namespace clb_itf {

struct Filter{
  struct Config {
    inline void reset(){
#if defined(CTRL_DEMOSAIC)
      REFSET(*this, 0);

      isAuto = false;

      denoise = 1;
      sharpen = 3;
      chrV = 3;
      chrH = 3;
#endif
	};

    bool isAuto;

    int32_t denoise;
    int32_t sharpen;
    int32_t chrV;
    int32_t chrH;
  };

  struct Status {
    inline void reset(){REFSET(*this, 0);};

    double gain;
    double integrationTime;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_FILTER_HPP__

