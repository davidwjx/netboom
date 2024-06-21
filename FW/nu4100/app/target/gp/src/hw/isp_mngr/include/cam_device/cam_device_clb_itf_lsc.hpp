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

#ifndef __CAM_DEVICE_CLB_ITF_LSC_HPP__
#define __CAM_DEVICE_CLB_ITF_LSC_HPP__

namespace clb_itf {

struct Lsc{
  struct Config {
    inline void reset(){ REFSET(*this, 0); };

    struct
    {
        uint16_t LscXGradTbl[CAEMRIC_GRAD_TBL_SIZE];
        uint16_t LscYGradTbl[CAEMRIC_GRAD_TBL_SIZE];
        uint16_t LscXSizeTbl[CAEMRIC_GRAD_TBL_SIZE];
        uint16_t LscYSizeTbl[CAEMRIC_GRAD_TBL_SIZE];
    } grid;

    struct
    {
        uint16_t LscRDataTbl[CAMERIC_DATA_TBL_SIZE];
        uint16_t LscGRDataTbl[CAMERIC_DATA_TBL_SIZE];
        uint16_t LscGBDataTbl[CAMERIC_DATA_TBL_SIZE];
        uint16_t LscBDataTbl[CAMERIC_DATA_TBL_SIZE];
    } gain;

    bool isAdaptive;
  };

  struct Status {
    CamEngineLscConfig_t config;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_LSC_HPP__

