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

#ifndef __CAM_DEVICE_WDR4_HPP__
#define __CAM_DEVICE_WDR4_HPP__


namespace clb_itf {

struct Wdr{
    enum Generation { V1, V2, V3, V4, V5,VMax };
    struct Config {
        int32_t moduleVersion;
        inline void reset(){
          REFSET(*this, 0);
          strength = 100;
          strengthHigh = 90;
          strengthLow = 32;
          flatStrength = 8;
          flatThreshold = 1;
        };

        bool isAuto;
        int autoLevel;

        int contrast;
        int flatStrength;
        int flatThreshold;
        int strength;
        int strengthHigh;
        int strengthLow;
        int strengthGlobal;
    };


    struct Status {
      double gain;
      double integrationTime;
    };

};

}


#endif   // __CAM_DEVICE_WDR4_HPP__