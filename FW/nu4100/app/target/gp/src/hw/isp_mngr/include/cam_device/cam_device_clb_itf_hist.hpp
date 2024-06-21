/******************************************************************************\
|* Copyright (c) 2023 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/

#ifndef __CAM_DEVICE_CLB_ITF_HIST_HPP__
#define __CAM_DEVICE_CLB_ITF_HIST_HPP__

namespace clb_itf {

struct Hist{

#ifdef CTRL_HIST256
#define CAMDEV_ISP_HIST_NUM_BINS           256  /**< number of bins */
#else
#define CAMDEV_ISP_HIST_NUM_BINS           16  /**< number of bins */
#endif

#define CAMDEV_ISP_HIST_GRID_ITEMS         25  /**< number of grid sub windows */

  typedef enum CamDeviceHistMode_e
  {
      CAMDEV_HIST_MODE_INVALID       = 0,    /**< lower border (only for an internal evaluation) */
      CAMDEV_HIST_MODE_RGB_COMBINED  = 1,    /**< RGB combined histogram */
      CAMDEV_HIST_MODE_R             = 2,    /**< R histogram */
      CAMDEV_HIST_MODE_G             = 3,    /**< G histogram */
      CAMDEV_HIST_MODE_B             = 4,    /**< B histogram */
      CAMDEV_HIST_MODE_Y             = 5,    /**< luminance histogram */
      CAMDEV_HIST_MODE_MAX,                  /**< upper border (only for an internal evaluation) */
  } CamDeviceHistMode_t;

  struct Config {
    inline void reset(){
        REFSET(*this, 0);
    };

    CamDeviceHistMode_t  histMode;
    clb_itf::CamDeviceWindow measureWindow;
    uint8_t  gridWeights[CAMDEV_ISP_HIST_GRID_ITEMS];
  };

  struct Status {
    uint32_t histBins[CAMDEV_ISP_HIST_NUM_BINS];
  };
};

}// namespace clb
#endif    // __CAM_DEVICE_CLB_ITF_HIST_HPP__

