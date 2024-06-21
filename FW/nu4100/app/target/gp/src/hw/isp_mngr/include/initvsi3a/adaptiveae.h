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
#ifndef VSI_3AV2_ADAPTIVE_AE_H_
#define VSI_3AV2_ADAPTIVE_AE_H_

#include <BaseAec.h>

namespace vsi3av2 {
class AdaptiveAe : public BaseAec {
    DECLARE_DYNAMIC_CLASS()
public:
    AdaptiveAe() {}
    virtual void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using statsType = shared_ptr<Aev2EventData>;
    virtual void onMessageAeStats(statsType& data);
    virtual void load(const Json::Value& node);
    virtual float getRoiLuma();
    void calcMotionFactor();

private:
    enum {
        AUTO_BANDING_STATE_OFF = 0,
        AUTO_BANDING_STATE_RUNNING,
        AUTO_BANDING_STATE_LOCKED,
    };

    enum {
        TIMING_STATUS_INIT = 0,
        TIMING_STATUS_AE_LOCKING,
        TIMING_STATUS_AE_LOCKED,
    };

    vector<vector<float>> lines;
    virtual void updateFlicker(uchar* mean);
    int mBandingState = AUTO_BANDING_STATE_OFF;
    bool switching = false;

    float grayData[1024];
    float motionFactor = 0;

    struct {
        float motionFilter = 0.5;
        float targetFilter = 0.5;
        vector<float> lowlight_linear_repress = {1, 0.7, 0.4};
        vector<float> lowlight_linear_gain = {4, 8, 16};
        vector<float> lowlight_hdr_repress = {1, 0.7, 0.4};
        vector<float> lowlight_hdr_gain = {4, 8, 16};
        float cMotionThreshold = 0.5;
        float cRoiWeight = 0.6;
    } tune;
    
    uchar lastLuma[1024] = {0};
    float lastDiff = 0;
    float currentTarget = 0;
    int mTimingStatus = TIMING_STATUS_INIT;

    float LE[10] = {-1};
    int skipFrame = 2;
    int exposureHistoryNumber = 0;


};

}

#endif
