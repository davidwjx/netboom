/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
\******************************************************************************/
#ifndef VSI_3AV2_AEV2_H_
#define VSI_3AV2_AEV2_H_

#include <BaseAec.h>

namespace vsi3av2 {
class Aev2 : public BaseAec {
    DECLARE_DYNAMIC_CLASS()
public:
    Aev2() {}
    virtual void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using statsType = shared_ptr<Aev2EventData>;
    virtual void onMessageAeStats(statsType& data);
    virtual float getRoiLuma();
    void calcMotionFactor();

    void load(const Json::Value& node);

    enum {
        AUTO_BANDING_STATE_OFF = 0,
        AUTO_BANDING_STATE_RUNNING,
        AUTO_BANDING_STATE_LOCKED,
    };

    float grayData[1024];
    float motionFactor = 0;
    uchar lastLuma[1024] = {0};
    float lastDiff = 0;
    float motionFilter = 0.5;
    float cMotionThreshold = 0.7;
    float cRoiWeight = 0.6;

};

}

#endif
