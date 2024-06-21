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
|* Created by zhiye.yin@verisilicon.com                                       *|
|*   You are not authorized to modify this file                               *|
\******************************************************************************/
#ifndef VSI_3AV2_AWBV2_H_
#define VSI_3AV2_AWBV2_H_

#include <BaseAwb.h>

namespace vsi3av2 {
class Awbv2 : public BaseAwb {
    DECLARE_DYNAMIC_CLASS()
public:
    Awbv2() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using statsType = shared_ptr<AwbEventData>;
    void onMessageAwbStats(statsType& data);
    using aev2Type = shared_ptr<Aev2EventData>;
    void onMessageAev2Stats(aev2Type& data);
    virtual void load(const Json::Value& node);
private:
    bool bGainInit = false;

    map<int, vector<float>> illuweight;
    float grayworldWeight = 0.0f;
    vector<float> colorTemperature;
};

}
#endif
