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

#ifndef __CAM_DEVICE_CLB_ITF_3DLUT_HPP__
#define __CAM_DEVICE_CLB_ITF_3DLUT_HPP__

namespace clb_itf {

struct Lut3d{
  struct Config {
    inline void reset(){
      REFSET(*this, 0);
    };

    bool isEnable;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_3DLUT_HPP__
