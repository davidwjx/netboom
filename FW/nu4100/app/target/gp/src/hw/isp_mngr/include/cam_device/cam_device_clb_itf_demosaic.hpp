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

#ifndef __CAM_DEVICE_CLB_ITF_DEMOSAIC2_HPP__
#define __CAM_DEVICE_CLB_ITF_DEMOSAIC2_HPP__

namespace clb_itf {

struct DeMosaic{
    enum Generation { V1, V2, VMax };
    struct Config {
    int32_t moduleVersion;
    inline void reset(){
        REFSET(*this, 0);

        deMoire.isEnable = false;
        deMoire.areaThreshold = 0x03;
        deMoire.saturationShrink = 0x0F;
        deMoire.threshold = {0x0100, 0x00, 0x03, 0x08};
        deMoire.edge = {0x0100, 0x29, 0x04, 0x49};

        sharpen.isEnable = true;
        sharpen.factor = {0x78, 0x78};
        sharpen.clip = {0xC8, 0xC8};
        sharpen.threshold = {0x00, 0x00, 0x00, 0x00};
        sharpen.ratio = {0x0100, 0x80, 0x00};

        sharpenLine.isEnable = false;
        sharpenLine.shift1 = 0x05;
        sharpenLine.shift2 = 0x06;
        sharpenLine.t1 = 0x03E8;
        sharpenLine.strength = 0x0FFF;
        sharpenLine.ratio = {0xC8, 0x05};

        sharpenFilter = {0x02, 0x04, 0x02, 0x04, 0x0A, 0x04, 0x02, 0x04, 0x02};

        dePurple.isEnable = true;
        dePurple.cbCrMode = Config::DePurple::RedBlue;
        dePurple.saturationShrink = 0x03;
        dePurple.threshold = 0x08;

        cac.count = {0x1000, 0x1000};
        cac.x = {0x08, 0x10};
        cac.y = {0x08, 0x10};

        threshold = 0x04;
        denoiseStrength = 0x00;
        sharpenSize = 0x10;
        isBypass = true;
    };

    struct InterplationDirectionThreshold {
        uint16_t min;
        uint16_t max;
    } interplationDirectionThreshold;

    struct Demoire {
        bool isEnable;

        uint8_t areaThreshold;
        uint8_t saturationShrink;

        struct {
            uint16_t r2;
            uint16_t r1;
            uint8_t t2Shift;
            uint8_t t1;
        } threshold;

        struct {
            uint16_t r2;
            uint16_t r1;
            uint8_t t2Shift;
            uint16_t t1;
        } edge;
    } deMoire;

    struct Sharpen {
        bool isEnable;

        struct {
            uint16_t black;
            uint16_t white;
        } factor, clip;

        struct Threshold {
            uint16_t t4Shift;
            uint16_t t3;
            uint8_t t2Shift;
            uint16_t t1;
        } threshold;

        struct Ratio {
            uint16_t r3;
            uint16_t r2;
            uint16_t r1;
        } ratio;
    } sharpen;

    struct SharpenLine {
        bool isEnable;

        uint8_t shift1;
        uint8_t shift2;
        uint16_t t1;
        uint16_t strength;

        struct Ratio {
            uint16_t r2;
            uint16_t r1;
        } ratio;
    } sharpenLine;

    struct SharpenFilter {
        uint8_t _00;
        uint8_t _01;
        uint8_t _02;
        uint8_t _10;
        uint8_t _11;
        uint8_t _12;

        uint8_t _20;
        uint8_t _21;
        uint8_t _22;
    } sharpenFilter;

    struct DePurple {
        bool isEnable;

        enum CbCrMode {
            Off,
            Red,
            Blue,
            RedBlue,
        } cbCrMode;

        uint8_t saturationShrink;
        uint8_t threshold;
    } dePurple;

    struct Skin {
        bool isEnable;

        struct {
            uint16_t max2047;
            uint16_t min2047;
        } cbThreshold, crThreshold, yThreshold;
    } skin;

    struct Cac {
        bool isEnable;

        struct Count {
            uint16_t v;
            uint16_t h;
        } count;

        struct {
            uint16_t red;
            uint16_t blue;
        } a, b, c;

        struct Normal {
            uint8_t s;
            uint8_t f;
        } x, y;
    } cac;

    uint8_t threshold;
    uint8_t denoiseStrength;
    uint8_t sharpenSize;
    bool isBypass;

  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_DEMOSAIC_HPP__

