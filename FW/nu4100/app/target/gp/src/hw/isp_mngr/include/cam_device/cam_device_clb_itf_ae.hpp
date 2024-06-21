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

#ifndef __CAM_DEVICE_CLB_ITF_AE_HPP__
#define __CAM_DEVICE_CLB_ITF_AE_HPP__

namespace clb_itf {

struct Ae{
  enum Generation { V1, V2, VMax };

  union Config {
    struct V1 {
      inline void reset(){
        REFSET(*this, 0);
        isBypass = true;
        ecm.reset();
      };

      bool isBypass;

      enum Mode {
        ModeDisable,
        ModeFix,
        ModeAdaptive,

        ModeSum,
      } mode;

      float dampingOver;
      float dampingUnder;
      float setPoint;
      float tolerance;

      struct Ecm {
        inline void reset(){ REFSET(*this, 0);};

        enum FlickerPeriod {
          FlickerPeriodOff,
          FlickerPeriod100Hz,
          FlickerPeriod120Hz,
        } flickerPeriod;

        double flickerPeriodValue() {
          switch (flickerPeriod) {
          case CAM_ENGINE_FLICKER_OFF:
          default:
            return 0.0000001;

          case CAM_ENGINE_FLICKER_100HZ:
            return 1.0 / 100.0;

          case CAM_ENGINE_FLICKER_120HZ:
            return 1.0 / 120.0;
          }
        }

        bool isAfps;
      } ecm;
    } v1;

    struct V2 : V1 {
      int dummy;
    } v2;
  };

  struct Status {
    inline void reset(){ REFSET(*this, 0);};

    union histogram {
      struct V1 {
        unsigned int _16[16];
      } v1;

      struct V2 {
        unsigned int _16[16];
        unsigned int _64[32];
        unsigned int _256[256];

        struct Hdr {
          unsigned char aeData[75];

          unsigned int green[16];
          unsigned int blue[16];
          unsigned int red[16];
        } hdr;
      } v2;
    } histogram;

    unsigned char luminance[25];
    unsigned char objectRegion[25];
  };

  struct AeRoi {
    inline void reset(){
      REFSET(*this, 0);
      windowNum = 16;
    };
        unsigned int windowNum;     // window number of ROI
#define AE_ROI_WINDOW_MAX 16
        RoiWindow roiWindow[AE_ROI_WINDOW_MAX];
  };
};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_AE_HPP__

