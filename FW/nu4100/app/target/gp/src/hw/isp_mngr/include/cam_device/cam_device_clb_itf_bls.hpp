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

#ifndef __CAM_DEVICE_CLB_ITF_BLS_HPP__
#define __CAM_DEVICE_CLB_ITF_BLS_HPP__

namespace clb_itf {

struct Bls{
  struct Config {
    inline void reset(){
      isBypass = true;

      red = 64;
      greenB = 64;
      greenR = 64;
      blue = 64;
    };

    bool isBypass;

    uint16_t red;
    uint16_t greenB;
    uint16_t greenR;
    uint16_t blue;
  };
};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_BLS_HPP__

