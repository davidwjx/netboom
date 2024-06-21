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

#ifndef __CAM_DEVICE_CLB_ITF_AVS_HPP__
#define __CAM_DEVICE_CLB_ITF_AVS_HPP__

#define AVS_DAMP_DATA_MAX 50
#define AVS_NUM_ITP_POINTS_DEFAULT 17
#define AVS_THETA_DEFAULT 0.5
#define AVS_BASE_GAIN_DEFAULT 1.0
#define AVS_FALL_OFF_DEFAULT 0.5
#define AVS_ACCELERATION_DEFAULT 4.0

namespace clb_itf {

struct Avs{
  struct Config {
    inline void reset(){
      acceleration = AVS_ACCELERATION_DEFAULT;
      baseGain = AVS_BASE_GAIN_DEFAULT;
      fallOff = AVS_FALL_OFF_DEFAULT;
      numItpPoints = AVS_NUM_ITP_POINTS_DEFAULT;
      theta = AVS_THETA_DEFAULT;
    };

    bool isUseParams;

    float acceleration;
    float baseGain;
    float fallOff;
    uint16_t numItpPoints;
    float theta;

    int32_t dampCount;
    double x[AVS_DAMP_DATA_MAX];
    double y[AVS_DAMP_DATA_MAX];
  };

  struct Status {
    std::pair<int16_t, int16_t> displacement;
    std::pair<int16_t, int16_t> offset;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_AVS_HPP__

