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
#ifndef _VSI_3AV2_BASEAEC_H_
#define _VSI_3AV2_BASEAEC_H_

#include <Base3A.h>
#include "asem.h"

const float FLT_EPSILON = 0.00000011920928955078125;

namespace vsi3av2 {

class BaseAec : public Base3A {
public:
    using Base3A::Base3A;
    virtual void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);

protected:
    float damping(float newExposure);
    virtual void ecmConvert(float newExposure);
    virtual float sceneEvaluation();
    virtual float semFix();
    virtual float semAdaptive();
    using mConfigType = shared_ptr<VVAecSettings>;
    void onMessageAecConfig(mConfigType& s);
    float c1 = 50;
    float c2 = 150;
protected:
    float mEcmFlicker = 0.01;
    float mBlackLevel = 0;
    const float ECM_FLICKER_50HZ = 1.0/100.0;
    const float ECM_FLICKER_60HZ = 1.0/120.0;
    bool reset_ae = false;
    map<int, vector<int>> hdrMode2AehdrIndex = {
        {0, {1, 2}},
        {1, {1, 2}},
        {2, {1, 2}},
        {3, {0, 2}},
        {4, {0, 1}},
        {5, {0, 2}},
        {6, {0, 1}},
    };
};



}

#endif
