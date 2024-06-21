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
#ifndef VSI_3AV2_AEV1_H_
#define VSI_3AV2_AEV1_H_

#include <BaseAec.h>

namespace vsi3av2 {
class Aev1 : public BaseAec {
    DECLARE_DYNAMIC_CLASS()
public:
    Aev1() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using statsType = shared_ptr<Aev1EventData>;
    void onMessageAeStats(statsType& data);
    using histType = shared_ptr<HistEventData>;
    void onMessageHistStats(histType& data);
    using hist256Type = shared_ptr<Hist256EventData>;
    void onMessageHist256Stats(hist256Type& data);
};

}
#endif
