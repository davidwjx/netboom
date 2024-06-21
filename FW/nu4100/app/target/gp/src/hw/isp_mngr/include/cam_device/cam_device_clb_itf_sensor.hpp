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

#ifndef __CAM_DEVICE_CLB_ITF_SENSOR_HPP__
#define __CAM_DEVICE_CLB_ITF_SENSOR_HPP__

#define SENSOR_ID_LEN 11
#define MODULE_SN_LEN 12

namespace clb_itf {

struct Sensor {
    struct Config {
        inline void reset(){ REFSET(*this, 0);};

        enum BayerPattern {
            Bggr,
            Gbrg,
            Grbg,
            Rggb,

            Sum,
        } bayerPattern;

        int framesPerSecond;

        bool isTestPattern;

        struct Gain {
            float aGain;
            float dGain;
        } ;

        struct Exposure {
            float gain;
            float integrationTime;
        } _long, _short, veryShort;

        struct Resolution {
            uint32_t height;
            uint32_t width;
        } resolution;
    } config;

    struct Status {
        inline void reset(){ REFSET(*this, 0);};
        char driverName[30];
        bool isConnected;

        struct Range {
            float max;
            float min;
            float step;
        } gain, integrationTime;

        struct ResolutionCollection {
            int index;
            Config::Resolution resolution;
        } resolutionCollection;
    };

    struct OtpModuleInfo{
        uint16_t HWVersion;
        uint16_t EEPROMRevision;
        uint16_t SensorRevision;
        uint16_t TLensRevision;
        uint16_t IRCFRevision;
        uint16_t LensRevision;
        uint16_t CARevision;
        uint16_t ModuleInteID;
        uint16_t FactoryID;
        uint16_t MirrorFlip;
        uint16_t TLensSlaveID;
        uint16_t EEPROMSlaveID;
        uint16_t SensorSlaveID;
        uint8_t  SensorID[SENSOR_ID_LEN];
        uint16_t ManuDateYear;
        uint16_t ManuDateMonth;
        uint16_t ManuDateDay;
        uint8_t  BarcodeModuleSN[MODULE_SN_LEN];
        uint16_t MapTotalSize;
    };
};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_SENSOR_HPP__

