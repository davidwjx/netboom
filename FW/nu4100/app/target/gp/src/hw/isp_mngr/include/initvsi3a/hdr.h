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
#ifndef VSI_3AV2_AUTOHDR_H_
#define VSI_3AV2_AUTOHDR_H_

#include <Base3A.h>

namespace vsi3av2 {
class AutoHdr : public Base3A {
    DECLARE_DYNAMIC_CLASS()
public:
    AutoHdr() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
private:
    using mConfigType = shared_ptr<VVHdrSettings>;
    void onMessageHdrConfig(mConfigType& config);
    void onMessageSetExposure(float& expoTime, float& gain);
};

}

#endif
