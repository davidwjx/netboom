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
#ifndef VSI_3AV2_ALGORITHM_INTERFACE_
#define VSI_3AV2_ALGORITHM_INTERFACE_
#include <atomic>
#include <cstring>
#include <vector>

#include "IObject.h"
#include "EAutoLock.h"

namespace vsi3av2 {

class Base3A : public IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>> {
public:
    Base3A() {}
    virtual void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);

protected:
    map<int, int> mModes;
    map<int, int> mStatus;
    PBUS mInputBus, mOutputBus;
    shared_ptr<GlobalDataArea> mDataArea;
};

}

#endif
