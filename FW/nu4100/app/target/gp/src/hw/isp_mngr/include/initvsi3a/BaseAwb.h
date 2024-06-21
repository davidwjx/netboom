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
|* Created by zhiye.yin@verisilicon.com                                       *|
|*   You are not authorized to modify this file                               *|
\******************************************************************************/
#ifndef VSI_3AV2_BASEAWB_H_
#define VSI_3AV2_BASEAWB_H_

#include <Base3A.h>
#include <cmath>

namespace vsi3av2 {

struct AwbCenterLine {
    float rg ,bg, d;
};

struct AwbClipParam {
    vector<float> rg;
    vector<float> maxDist;
    void clear() {
        rg.clear();
        maxDist.clear();
    }
};

struct AwbFadeParam {
    vector<float> fade;
    vector<float> gainDist;
    void clear() {
        fade.clear();
        gainDist.clear();
    }
};

struct AwbFade2Param {
    vector<float> fade;
    vector<float> cbMinMax, cbMinMin;
    vector<float> crMinMax, crMinMin;
    vector<float> maxCSumMax, maxCSumMin;

    void clear() {
        fade.clear();
        cbMinMax.clear();
        cbMinMin.clear();
        crMinMax.clear();
        crMinMin.clear();
        maxCSumMax.clear();
        maxCSumMin.clear();
    }
};

struct AwbCCProfile {
    string name;
    float saturation;
    Matf ccmCoeff;
    Matf ccmOffset;
    Matf componentGain;
    bool operator< (const struct AwbCCProfile& r) { return saturation > r.saturation; }
};

struct AwbLscSector {
    vector<ushort> xgrad, ygrad, xsize, ysize;
    void clear() {
        xgrad.clear();
        ygrad.clear();
        xsize.clear();
        ysize.clear();
    }
};

struct AwbLscProfile {
    string name;
    string res;
    string illum;
    float vignetting;
    ushort sectors;
    uint32_t number, x, y;
    struct AwbLscSector sector;
    Matf mat[4];
    bool operator< (const struct AwbLscProfile& r) { return vignetting > r.vignetting; }
};

struct AwbIlluProfile {
    string name;
    uint id;
    int doorType;
    int awbType;
    Matf ccmCoeff;
    Matf ccmOffset;
    Matf componentGain;
    Matf gaussMean;
    Matf convariance;
    float gaussFactor;
    Matf threshold;
    vector<float> sensorGainSat;
    vector<float> saturationCurve;
    vector<float> sensorGainVig;
    vector<float> vignetting;
    vector<string> ccProfilesNames;
    vector<AwbCCProfile> ccProfiles;
    map<int, vector<string>> lscProfileNames;
    map<int, vector<AwbLscProfile>> lscProfiles;
};

struct AwbExpPrior {
    float iirAdd, iirSub, iirThreshold;
    float iirMin, iirMax, iirInit;
    vector<float> filterItems;
    int index = 0;
};

enum {
    AWB_EXPPRIOR_INDOOR = 1,
    AWB_EXPPRIOR_OUTDOOR,
    AWB_EXPPRIOR_TRANSITION_RANGE,
};

enum {
    AWB_ILLUEST_REGION_A = 1,
    AWB_ILLUEST_REGION_B,
    AWB_ILLUEST_REGION_C,
};

enum {
    AWB_WP_NUMBER_INVALID = 0,
    AWB_WP_NUMBER_MIN,
    AWB_WP_NUMBER_MAX,
    AWB_WP_NUMBER_TARGET_RANGE,
};


class BaseAwb : public Base3A {
public:
    using Base3A::Base3A;
    virtual void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);

protected:
    using mConfigType = shared_ptr<struct VVAwbSettings>;
    mConfigType mSettings;
    using otpConfigType = shared_ptr<VVOtpSettings>;
    otpConfigType otpSettings;

    bool load();
    bool calcExposurePrior();
    void onMessageAwbConfig(mConfigType& s);
    void onMessageOtpConfig(otpConfigType& o);
    void onMessageStart(int& index, int& ret);
    void onMessageAwbGetRuntimeStatus(struct VVAwbRuntimeStatus*& pStatus);
    void normalizeGains(Matf& m);
    void colorCorrection();
    void lensShadingCorrection();
    void updateValues();

    uint minWhitePixels, maxWhitePixels;
    char resName[15];
    char illuminationName[20];
    int resIndex;
    int illuIdx;
    int numberIlluProfiles;
    int D50IlluProfileIdx;
    int currentDoorType = AWB_EXPPRIOR_INDOOR;

    struct Calib {
        float regionSizeStart;
        float regionSize;
        float rgProjMaxSky;
        float rgProjIndoorMin;
        float rgProjOutdoorMin;
        float rgProjMax;
        float regionSizeIncrease;
        float regionSizeDecrease;
        float kFactor;
        Matf pca = Matf(2, 3, 0.0f);
        Matf svd = Matf(1, 3, 0.0f);
        AwbCenterLine centerLine;
        AwbClipParam clipCurve1, clipCurve2;
        AwbFadeParam fade1, fade2;
        AwbFade2Param fadeCrCb;
        vector<AwbIlluProfile> illuProfiles;
        struct AwbExpPrior iir;
        void addIlluProfiles(void *Profiles, void* handle);

        void clear() {
            fadeCrCb.clear();
            illuProfiles.clear();
            fade1.clear();
            fade2.clear();
            clipCurve1.clear();
            clipCurve2.clear();
        }
    } calib;

    int measureMode;
    Matf revertedMeanRgb = Matf(1, 3);
    int DominateIlluProfileIdx = -1;
    vector<float> mWeight;
    vector<float> mWeightTrans;
    Matf wbGains;
    Matf dampedGains = Matf(1, 4, 1.0f);
    bool WbGainsOutOfRange = false;
    bool RgProjClippedToOutDoorMin = false;
    float mRgProj;
    uint mWhitePixelEvaluation = 0;

    struct {
        float r, b, min_c, max_c, max_y, min_y;
    } MeasWdw;
};

}

#endif
