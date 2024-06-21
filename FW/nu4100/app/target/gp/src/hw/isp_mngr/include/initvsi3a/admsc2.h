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
#ifndef VSI_3AV2_ADMSC2_H_
#define VSI_3AV2_ADMSC2_H_

#include <BaseAuto.h>


namespace vsi3av2 {
class ADmsc2 : public BaseAuto {
    DECLARE_DYNAMIC_CLASS()
public:
    ADmsc2() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    void load(const Json::Value& node);
private:
    using statsType = shared_ptr<Aev2EventData>;
    void onMessageAeStats(statsType& data);

    typedef struct AutoTable {
        vector<float> enable;
        vector<float> gain;
        vector<float> facblack;
        vector<float> facwhite;
        vector<float> maxblack;
        vector<float> maxwhite;
        vector<float> t1;
        vector<float> t2;
        vector<float> t3;
        vector<float> t4;
        vector<float> gdenoise;
        vector<float> gsharpen;
    } AutoTable;

    map<int, AutoTable> autoTable;
};

}

#endif
