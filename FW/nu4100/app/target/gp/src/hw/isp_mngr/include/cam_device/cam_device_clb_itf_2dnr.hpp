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

#ifndef __CAM_DEVICE_CLB_ITF_2DNR5_HPP__
#define __CAM_DEVICE_CLB_ITF_2DNR5_HPP__

namespace clb_itf {

struct Dnr2{
    enum Generation { V1, V5, VMax };

    struct Config {
        int32_t moduleVersion;
        inline void reset(){
            REFSET(*this, 0);

            blendOpacity = {20, 80}; // FIXME
            denoiseStrength = 2.0;   // FIXME
            sigma = 5.0;
        };

        bool isAuto;
        int32_t autoLevel;

        struct BlendOpacity {
            int32_t _static;
            int32_t moving;
        } blendOpacity;

        double denoiseStrength;
        double sigma;
    };

    struct Status {
        double gain;
        double integrationTime;
    };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_2DNR5_HPP__

