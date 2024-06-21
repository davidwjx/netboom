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

#ifndef __CAM_DEVICE_CLB_ITF_EE_HPP__
#define __CAM_DEVICE_CLB_ITF_EE_HPP__

namespace clb_itf {

struct Ee{
  struct Config {
    inline void reset(){
#if defined(CTRL_EE)
		REFSET(*this, 0);

		isAuto = false;

		config.edgeGain = 1800;
		config.strength = 100;
		config.uvGain = 512;
		config.yDownGain = 10000;
		config.yUpGain = 10000;
#endif
	};

    bool isAuto;

    CamEngineEeConfig_t config;
  };

  struct Status {
    inline void reset() { REFSET(*this, 0) ;};

    double gain;
    double integrationTime;
  };

};

}// namespace camdevice_clb
#endif    // __CAM_DEVICE_CLB_ITF_EE_HPP__

