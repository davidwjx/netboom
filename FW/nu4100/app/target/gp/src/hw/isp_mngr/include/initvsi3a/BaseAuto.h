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
#ifndef VSI_3AV2_BASEAUTO_H_
#define VSI_3AV2_BASEAUTO_H_

#include <Base3A.h>
#include <mutex>

namespace vsi3av2 {
class BaseAuto : public Base3A {
public:
    BaseAuto() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea) {
        Base3A::init(inBus, outBus, dataArea);
    }
protected:
    std::mutex mMutex;
    bool bEnabled;
    bool bDisable;
};

}

#endif
