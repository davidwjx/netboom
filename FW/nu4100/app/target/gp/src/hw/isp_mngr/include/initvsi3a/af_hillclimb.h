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
#ifndef VSI_3AV2_AF_HILLCLIMB_H_
#define VSI_3AV2_AF_HILLCLIMB_H_

#include <Base3A.h>

namespace vsi3av2 {
class AfHillClimb : public Base3A {
    DECLARE_DYNAMIC_CLASS()
public:
    AfHillClimb() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using statsType = shared_ptr<AfEventData>;
    void onMessageAfStats(statsType& data);

private:

    enum {
        AF_INTERNAL_STATUS_INIT = 0,
        AF_INTERNAL_STATUS_SEARCHING,
        AF_INTERNAL_STATUS_TRACKING,
        AF_INTERNAL_STATUS_LOCKED,
    };

    int mInternalStatus = AF_INTERNAL_STATUS_INIT;

    // TUNING PARAMS
    float weightWindow[3] = {0.3, 1.0, 0.7};
    const float cMotionThreshold = 0.5;      // [0, 1]
    const float cStableTolerance = 0.2;      // [0, 1]
    const int   cPointsOfCurve = 12;         // [5, 100]
    ZFilterParams focalFilter = {440, 0, 3, 100, 1};
    ZFilterParams shapFilter = {10, 0, 2, 10, 1};
    // END TUNING PARAMS

    int curFocal;
    int maxFocal;
    int minFocal;

    int maxRange;
    int minRange;

    int direction = -1;
    float topFocal = 0;
    float topSharpness = 0;
    int step;
    float stableSharpness = 0;
};

}
#endif
