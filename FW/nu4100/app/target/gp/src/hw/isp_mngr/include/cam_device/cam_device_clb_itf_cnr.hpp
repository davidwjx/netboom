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

#ifndef __CAM_DEVICE_CLB_ITF_CNR_HPP__
#define __CAM_DEVICE_CLB_ITF_CNR_HPP__

namespace clb_itf {

struct Cnr{
  struct Config {
    inline void reset(){ REFSET(*this, 0); };

    uint32_t tc1;
    uint32_t tc2;
  };
};

struct Cnr2{
  struct Config {
    inline void reset(){
      REFSET(*this, 0);
      ynr_enable = true;
      cnr_enable = true;
      c_sigma_layer[0] = 16.0;
      c_sigma_layer[1] = 16.0;
      c_sigma_layer[2] = 12.0;
      c_sigma_layer[3] = 8.0;

      c_sigma_merge[0] = 16.0;
      c_sigma_merge[1] = 16.0;
      c_sigma_merge[2] = 12.0;
      y_sigma_layer = 2.0;
      bit = 1;
      cnr_strength1 = 128;
      cnr_strength2 = 0;
      ynr_strength1 = 128;
      ynr_strength2 = 0;
      texture_mask_select = 0;
    };

    bool ynr_enable;
    bool cnr_enable;
    float c_sigma_layer[4];
    float c_sigma_merge[3];
    float y_sigma_layer;
    uint8_t bit;
    uint8_t cnr_strength1;
    uint8_t cnr_strength2;
    uint8_t ynr_strength1;
    uint8_t ynr_strength2;
    uint8_t texture_mask_select;
  };
};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_CNR_HPP__
