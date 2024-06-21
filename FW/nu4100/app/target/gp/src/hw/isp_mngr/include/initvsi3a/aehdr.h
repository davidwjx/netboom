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
#ifndef VSI_3AV2_AEHDR_H_
#define VSI_3AV2_AEHDR_H_

#include <BaseAec.h>

namespace vsi3av2 {
class Aehdr : public BaseAec {
    DECLARE_DYNAMIC_CLASS()
public:
    Aehdr() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using statsType = shared_ptr<AehdrEventData>;
    void onMessageAeStats(statsType& data);
    using histType = shared_ptr<HisthdrEventData>;
    void onMessageHistStats(histType& data);

    using aev2Type = shared_ptr<Aev2EventData>;
    void onMessageAev2Stats(aev2Type& data);
    void load(const Json::Value& node);

private:
    float meanLumaS, meanLumaVS;
    vector<float> lumaS, lumaVS;
    uchar grayData[1024];
    vector<float> vGrayData;
    float motionFactor = 0;
    uchar lastLuma[1024] = {0};
    float lastDiff = 0;
    float motionFilter = 0.5;

    void calcMotionFactor();
};

}
#endif
