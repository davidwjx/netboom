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
#ifndef _VSI_3AV2_BASE_CONTROLLER_H_
#define _VSI_3AV2_BASE_CONTROLLER_H_
#include "IObject.h"

namespace vsi3av2 {

class IController : public IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>> {
public:
    IController() {}
    virtual void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea) {
        mOutputBus = outBus;
        mDataArea = dataArea;
    }

protected:
    PBUS mOutputBus;
    shared_ptr<GlobalDataArea> mDataArea;
};

}


#endif
