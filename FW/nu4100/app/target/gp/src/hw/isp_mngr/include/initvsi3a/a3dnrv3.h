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
#ifndef VSI_3AV2_3DNRV3_H_
#define VSI_3AV2_3DNRV3_H_

#include <BaseAuto.h>

namespace vsi3av2 {
class A3dnrv3 : public BaseAuto {
    DECLARE_DYNAMIC_CLASS()
public:
    A3dnrv3() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    void load(const Json::Value& node);
private:
    using statsType = shared_ptr<Aev2EventData>;
    void onMessageAeStats(statsType& data);

    typedef struct AutoTable {
        vector<float> gain;
        vector<float> filterLen;
        vector<float> filterLen2;
        vector<float> nlf_lower;
        vector<float> nlf_upper;
        vector<float> sadweight;
        vector<float> noisemodel_a;
        vector<float> noisemodel_b;
        vector<float> autoStrength;
        vector<float> motion_dilate_en;
        vector<float> motion_erode_en;
        vector<float> motion_pregamma_en;
        vector<float> strength;
        vector<float> range_h;
        vector<float> range_v;
        vector<float> dialte_h;
        vector<float> preweight;
        vector<float> noise_thresh_factor;
        vector<float> moving_pixel_thresh;
        vector<float> luma_comp_en;
        vector<float> luma_pixel_slop;
        vector<float> luma_pixel_slop_shift;
        vector<float> luma_pixel_slop_min;
        vector<float> luma_pixel_val_shift;
        vector<float> nl_update_factor;
    } AutoTable;

    map<int, AutoTable> autoTable;
    int autonoiselevel = -1;
    bool bAutoNoiseLevel = false;
    bool tnr_en = true;
    bool nlm_en = true;
    int motion_mean = 200;
    float noise_mean = 200;
    float width = 0;
    float height = 0;
    int frameNumber = 0;
    bool sceneChanged = true;
    float lastGain = 0;
    Matf dampedMatrix;
    bool inited = false;
    float damp = 0.5;
    float kFactor = 1;
};

}

#endif
