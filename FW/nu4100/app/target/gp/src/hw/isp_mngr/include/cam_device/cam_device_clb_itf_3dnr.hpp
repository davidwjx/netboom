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

#ifndef __CAM_DEVICE_CLB_ITF_3DNR3_HPP__
#define __CAM_DEVICE_CLB_ITF_3DNR3_HPP__

namespace clb_itf {

struct Dnr3{
    enum Generation { V1, V3, V3_2, VMax };

    struct Config {
        int32_t moduleVersion;
        inline void reset(){
            REFSET(*this, 0);
            tnr.autoLevel = 1.0;
            tnr.filterLength = 20;
            tnr.filterLength2 = 10;
            tnr.motionSlope = 4;
            tnr.noiseLevel = 60;
            tnr.sadWeight = 16;
        };

        bool isEnableNoiseCurve;

        struct Tnr {
            bool isEnable;
            bool isAuto;
            double autoLevel;
            uint16_t filterLength;
            uint8_t filterLength2;
            uint8_t motionSlope;
            uint16_t noiseLevel;
            uint8_t sadWeight;
        } tnr;

    };

    struct Status {
        double gain;
        double integrationTime;
    };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_3DNR3_HPP__

