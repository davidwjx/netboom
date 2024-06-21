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

#ifndef __CAM_DEVICE_CLB_ITF_HDR_HPP__
#define __CAM_DEVICE_CLB_ITF_HDR_HPP__

namespace clb_itf {

struct Hdr{
  enum Generation { V1, V1_2, V2, V2_LITE, VMax };

  union Config {
    struct V1 {
      inline void reset(){
#if defined(CTRL_HDR)
          REFSET(*this, 0);
#endif // CTRL_HDR
      };

      float exposureRatio;
      int extensionBit;
    } v1;

    struct V1_2 {
      void reset(){
#if defined(CTRL_HDR1_2)
        REFSET(*this, 0);
#endif // CTRL_HDR1_2
        mode = LongShortVeryShort;

        for (auto i = 0; i < Sum; i++) {
          auto &parameter = parameters[i];

          parameter.exposureRatio = 32;
          parameter.trans.end = 0.85;
        }
      };

      enum Mode {
        LongShort,
        LongShortVeryShort,

        Sum,
      } mode;

      struct Parampeter {
        float exposureRatio;
        int extensionBit;

        struct Range {
          double begin;
          double end;
        } trans;
      } parameters[Sum];
    } v1_2;

    struct V2 {
      inline void reset(){
#if defined(CTRL_HDR2)
          REFSET(*this, 0);
#endif // CTRL_HDR2
      bypass = true;
      bypass_select = 0;
      sensor_type = 4;
      stitching_mode = 0;
      base_frame = 0;
      ratio[0] = 16;
      ratio[1] = 16;
      float temp_trans_range[4][2] = {{0.2, 0.9}, {0, 0.9}, {0.95, 1}, {0, 0.1}};
      memcpy(trans_range, temp_trans_range, sizeof(trans_range));
      extend_bit[0] = 1;
      extend_bit[1] = 4;
      color_weight[0] = 255;
      color_weight[1] = 0;
      color_weight[2] = 1;
      uint32_t temp_bls[4] = {0, 0, 0, 0};
      memcpy(bls_long, temp_bls, sizeof(bls_long));
      memcpy(bls_short, temp_bls, sizeof(bls_short));
      memcpy(bls_very_short, temp_bls, sizeof(bls_very_short));
      rdWrStr = 1;
      awbGains.Red = 1;
      awbGains.GreenR = 1;
      awbGains.GreenB = 1;
      awbGains.Blue = 1;
      hdr_id = 0;

      deghost.motion_enable[0] = false;
      deghost.motion_enable[1] = false;
      deghost.motion_weight[0] = 0;
      deghost.motion_weight[1] = 0;
      deghost.motion_weight_short = 0;
      deghost.motion_sat_threshold = 0;
      deghost.motion_weight_update_threshold = 0;
      for(int i=0; i<4; i++){
        deghost.motion_lower_threshold_ls[i] = 0;
        deghost.motion_upper_threshold_ls[i] = 0;
        deghost.motion_lower_threshold_lsvs[i] = 0;
        deghost.motion_upper_threshold_lsvs[i] = 0;
        deghost.dark_lower_threshold_ls[i] = 0;
        deghost.dark_upper_threshold_ls[i] = 0;
        deghost.dark_lower_threshold_lsvs[i] = 0;
        deghost.dark_upper_threshold_lsvs[i] = 0;
      }

      dpf.dpf_enable = false;
      dpf.gain = 1;
      dpf.gradient = 1;
      dpf.offset = 1;
      dpf.min = 16;
      dpf.div = 64;
      dpf.sigma_g = 4;
      dpf.sigma_rb = 4;
      uint32_t temp_noise_curve[17] = {3688,2169, 1687, 1428, 1260, 1050, 918, 827,\
                                  758, 660, 592, 542, 470, 421, 385, 357, 335};
      memcpy(dpf.noise_curve, temp_noise_curve, sizeof(dpf.noise_curve));
      };

      bool                              bypass;
      uint8_t                           bypass_select;
      uint32_t                          sensor_type;
      uint32_t                          stitching_mode;
      uint8_t                           base_frame;
      float                             ratio[2];
      float                             trans_range[4][2];
      float                             extend_bit[2];
      uint32_t                          color_weight[3];
      uint32_t                          bls_long[4];
      uint32_t                          bls_short[4];
      uint32_t                          bls_very_short[4];
      uint32_t                          rdWrStr;
      uint8_t                           hdr_id;
      struct AwbGains{
        uint16_t  Red;
        uint16_t  GreenR;
        uint16_t  GreenB;
        uint16_t  Blue;
      } awbGains;


      struct Dpf{
        bool                              dpf_enable;
        float                             gain;
        float                             gradient;
        float                             offset;
        float                             min;
        float                             div;
        float                             sigma_g;
        float                             sigma_rb;
        uint32_t                          noise_curve[17];
      } dpf;

      struct Deghost{
        bool                              motion_enable[2];
        uint32_t                          motion_weight[2];
        uint32_t                          motion_weight_short;
        uint32_t                          motion_sat_threshold;
        uint32_t                          motion_weight_update_threshold;
        uint32_t                          motion_lower_threshold_ls[4];
        uint32_t                          motion_upper_threshold_ls[4];
        uint32_t                          motion_lower_threshold_lsvs[4];
        uint32_t                          motion_upper_threshold_lsvs[4];
        uint32_t                          dark_lower_threshold_ls[4];
        uint32_t                          dark_upper_threshold_ls[4];
        uint32_t                          dark_lower_threshold_lsvs[4];
        uint32_t                          dark_upper_threshold_lsvs[4];
      } deghost;
    } v2;

    struct V2_LITE {
      inline void reset(){
#if defined(CTRL_HDR2_LITE)
          REFSET(*this, 0);
#endif // CTRL_HDR2_LITE
      bypass = false;
      bypass_select = 0;
      sensor_type = 0;
      bls[0] = 0;
      bls[1] = 0;
      bls[2] = 0;
      bls[3] = 0;
      dgain_enable = false;
      for(int i=0; i<4; i++){
        for(int j = 0; j<4; j++){
          dgain[i][j] = 1.0;
        }
      }
      color_weight[0] = 255;
      color_weight[1] = 0;
      color_weight[2] = 1;
      stitching_mode = 0;
      base_frame = 0;
      ratio[0] = 16.0;
      ratio[1] = 16.0;
      trans_range[0][0] = 0.85;
      trans_range[0][1] = 0.95;
      trans_range[1][0] = 0.85;
      trans_range[1][1] = 0.95;
      trans_range[2][0] = 0;
      trans_range[2][1] = 0.1;
      trans_range[3][0] = 0;
      trans_range[3][1] = 0.1;
      trans_range[4][0] = 0.85;
      trans_range[4][1] = 0.95;
      trans_range[5][0] = 0;
      trans_range[5][1] = 0.1;
      extend_bit[0] = -1;
      extend_bit[1] = -1;
      hdr_id = 0;
      };

      bool bypass;
      uint8_t bypass_select;
      uint32_t sensor_type;
      uint32_t bls[4];
      bool dgain_enable;
      float dgain[4][4];
      uint32_t color_weight[3];
      uint32_t stitching_mode;
      uint8_t base_frame;
      float ratio[2];
      float trans_range[6][2];
      float extend_bit[2];
      uint8_t hdr_id;
    } v2_lite;
  };

  union Status {
    struct V1_2 {
      enum Mode { _2Dol, _3Dol } mode;
    } v1_2;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_HDR_HPP__
