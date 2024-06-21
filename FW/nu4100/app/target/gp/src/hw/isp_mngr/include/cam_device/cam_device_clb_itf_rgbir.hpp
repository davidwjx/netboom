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

#ifndef __CAM_DEVICE_CLB_ITF_RGBIR_HPP__
#define __CAM_DEVICE_CLB_ITF_RGBIR_HPP__

namespace clb_itf {

struct Rgbir{
  struct Config {
    inline void reset(){
      REFSET(*this, 0);

      // config.outRgbBpt = Sensor;
      // config.rgbirBpt = CAMERIC_CPROC_LUM_RANGE_OUT_BT601;

      outRgbBpt = CAM_ENGINE_RGBIR_OUT_BPT_RGGB;
      rgbirBpt = CAM_ENGINE_RGBIR_BPT_BGGIR;

      dmscThreshold = 4;
      ir_threshold = 4096;
      l_threshold = 4096;
      irSigmas = 3.00;
      cc_mtx[0]= 1;
      cc_mtx[1]= 0;
      cc_mtx[2]= 0;
      cc_mtx[3]= 0;
      cc_mtx[4]= 0;
      cc_mtx[5]= 1;
      cc_mtx[6]= 0;
      cc_mtx[7]= 0;
      cc_mtx[8]= 0;
      cc_mtx[9]= 0;
      cc_mtx[10]= 1;
      cc_mtx[11]= 0;
      gain_r = 1;
      gain_g = 1;
      gain_b = 1;
      width = 1920;
      height = 1080;
      stageSelect = 4;
      filtMode = CAM_ENGINE_RGBIR_GREEN_FILT_DYNAMIC;
    };

    bool_t enable;
    bool_t enable_ir_raw_out;
    bool_t part1Enable;
    bool_t part2Enable;
    bool_t rcccEnable;
    uint8_t dmscThreshold;
    uint8_t stageSelect;
    CamEngineRgbirOutBpt_t outRgbBpt;
    CamEngineRgbirBpt_t rgbirBpt;
    CamEngineRgbirFiltMode_t filtMode;
    bool_t filtEnable;
    double irSigmas;
    uint32_t ir_threshold, l_threshold;
    uint16_t bls_r, bls_g, bls_b, bls_ir;
    double gain_r, gain_g, gain_b, gain_ir;
    double cc_mtx[12];
    uint16_t width, height;
    uint16_t dpccTh[4], dpccMidTh[4];
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_RGBIR_HPP__
