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
#ifndef VSI_3AV2_ACPROC_H_
#define VSI_3AV2_ACPROC_H_

#include <BaseAuto.h>

namespace vsi3av2 {
class ACproc : public BaseAuto {
    DECLARE_DYNAMIC_CLASS()
public:
    ACproc() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    void load(const Json::Value& node);
private:
    using statsType = shared_ptr<Aev2EventData>;
    void onMessageAeStats(statsType& data);

    typedef struct AutoTable {
        vector<float> gain;
        vector<float> contrast;
        vector<float> brightness;
        vector<float> saturation;
        vector<float> hue;
        vector<float> chromaout;
        vector<float> lumain;
        vector<float> lumaout;
    } AutoTable;

    map<int, AutoTable> autoTable;
};

}

#endif
