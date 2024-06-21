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

#ifndef __CAM_DEVICE_CLB_ITF_GC2_HPP__
#define __CAM_DEVICE_CLB_ITF_GC2_HPP__

namespace clb_itf {

struct Gc{
    enum Generation { V1, V2, VMax };
    enum Channel {
        ChannelBlue,
        ChannelGreen,
        ChannelRed,
        ChannelSum
    };

    struct Curve2  {
        double gamma;
        enum XScale {
            Logarithmic,
            Equidistant,
        } xScale;

        enum Mode {
            Standard,
            User
        } mode;
#define GC2_CURVE_POINT_COUNT 64
        uint32_t pX[GC2_CURVE_POINT_COUNT];
        uint32_t x[GC2_CURVE_POINT_COUNT];
        uint32_t y[GC2_CURVE_POINT_COUNT];

        Curve2() {}
        Curve2(double gamma, XScale scale, uint32_t pX[GC2_CURVE_POINT_COUNT],
            uint32_t x[GC2_CURVE_POINT_COUNT],
            uint32_t y[GC2_CURVE_POINT_COUNT]) {
            this->gamma = gamma;
            this->xScale = scale;

            REFCPY(this->pX, pX);
            REFCPY(this->x, x);
            REFCPY(this->y, y);
        }

        inline void reset(){
        gamma = 2.2;
        xScale = Equidistant;
        mode = Standard;

        uint32_t const standardPx[] = {
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6};

        REFCPY(pX, standardPx);

        uint32_t const standardX[] = {
            0,    64,   128,  192,  256,  320,  384,  448,  512,  576,  640,
            704,  768,  832,  896,  960,  1024, 1088, 1152, 1216, 1280, 1344,
            1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048,
            2112, 2176, 2240, 2304, 2368, 2432, 2496, 2560, 2624, 2688, 2752,
            2816, 2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328, 3392, 3456,
            3520, 3584, 3648, 3712, 3776, 3840, 3904, 3968, 4032};

        REFCPY(x, standardX);

        uint32_t const standardY[] = {
            0,   154, 211, 254, 290, 321, 348, 374, 397, 419,  440,  459, 478,
            495, 512, 529, 544, 560, 574, 589, 602, 616, 629,  642,  655, 667,
            679, 691, 702, 713, 725, 735, 746, 757, 767, 777,  787,  797, 807,
            816, 826, 835, 844, 853, 862, 871, 880, 889, 897,  906,  914, 922,
            930, 939, 947, 955, 962, 970, 978, 985, 993, 1001, 1008, 1015};

        REFCPY(y, standardY);

        };
    };

    struct Config {
        Config() {}
        int32_t moduleVersion;
        Curve2 curves[ChannelSum];
    };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_GC2_HPP__

