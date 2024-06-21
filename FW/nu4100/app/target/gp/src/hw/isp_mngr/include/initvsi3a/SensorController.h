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
#ifndef _VSI_3AV2_SENSOR_CONTROLLER_H_
#define _VSI_3AV2_SENSOR_CONTROLLER_H_
#include "IController.h"

namespace vsi3av2 {

class SensorController : public IController {
    DECLARE_DYNAMIC_CLASS()
public:
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea) final;
    void updateSensorStatus();

private:
    using mHandleType = void*;
    using mSettingsType = shared_ptr<VVSensorSettings>;
    void onMessageSetHandle(mSettingsType& s);
    void onMessageSetExposure(float& expoTime, float& gain);
    mHandleType mSensorHandle;
    mHandleType mCalibHandle;
};

}

#endif
