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

#ifndef __CAM_DEVICE_CLB_ITF_SIMP_HPP__
#define __CAM_DEVICE_CLB_ITF_SIMP_HPP__

namespace clb_itf {

struct Simp{

  struct Config {
    inline void reset(){
      fileName = std::string();

      REFSET(config, 0);
    };

    std::string fileName;
    CamEngineSimpConfig_t config;

  };
};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_SIMP_HPP__

