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

#ifndef __CAM_DEVICE_CLB_ITF_EXP_HPP__
#define __CAM_DEVICE_CLB_ITF_EXP_HPP__

namespace clb_itf {

struct Exp{

#define CAMDEV_AEV2_STATIC                 (4096 * 4)

    typedef enum CamDeviceExpSel_e
    {
      CAMDEV_AEV2_INPUT_SEL_DEGAMMA       = 0,
      CAMDEV_AEV2_INPUT_SEL_AWBGAIN       = 1,
      CAMDEV_AEV2_INPUT_SEL_WDR3          = 2
    } CamDeviceExpSel_t;

    struct Config {
    inline void reset(){
        REFSET(*this, 0);

        inputSelect = CAMDEV_AEV2_INPUT_SEL_AWBGAIN;
        measureWin.vOffset = 0;
        measureWin.hOffset = 0;
        measureWin.height = 1080;
        measureWin.width = 1920;
        ratio = 1.0;
    };

        CamDeviceExpSel_t inputSelect;
        clb_itf::CamDeviceWindow measureWin;
        float ratio;
    };

    struct Status {
        uint8_t expStatistic[CAMDEV_AEV2_STATIC];
    };
};

}// namespace clb
#endif    // __CAM_DEVICE_CLB_ITF_EXP_HPP__

