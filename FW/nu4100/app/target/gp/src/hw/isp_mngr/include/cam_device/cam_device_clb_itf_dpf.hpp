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

#ifndef __CAM_DEVICE_CLB_ITF_DPF_HPP__
#define __CAM_DEVICE_CLB_ITF_DPF_HPP__

namespace clb_itf {

struct Dpf{
  struct Config {
  inline void reset(){
    REFSET(*this, 0);

    isAdaptive = false;

    divisionFactor = 64;
    gradient = 0.15;
    minimumBound = 2;
    offset = 0;

    sigmaGreen = 4;
    sigmaRedBlue = 4;
};

    bool isAdaptive;

    float divisionFactor;
    float gradient;
    float minimumBound;
    float offset;

    uint8_t sigmaGreen;
    uint8_t sigmaRedBlue;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_DPF_HPP__

