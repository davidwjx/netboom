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

#ifndef __CAM_DEVICE_CLB_ITF_IE_HPP__
#define __CAM_DEVICE_CLB_ITF_IE_HPP__

namespace clb_itf {

struct Ie{
  struct Config {
    inline void reset(){
      config.mode = CAMERIC_IE_MODE_COLOR;
      config.range = CAMERIC_IE_RANGE_BT601;

      if (config.mode == CAMERIC_IE_MODE_COLOR) {
        config.ModeConfig.ColorSelection.col_selection =
          CAMERIC_IE_COLOR_SELECTION_RGB;
      }
    };

    CamerIcIeConfig_t config;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_IE_HPP__

