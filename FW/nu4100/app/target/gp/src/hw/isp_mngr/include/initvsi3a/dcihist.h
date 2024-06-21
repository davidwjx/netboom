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
#ifndef VSI_3AV2_WDR4_H_
#define VSI_3AV2_WDR4_H_

#include <Base3A.h>

namespace vsi3av2 {
class DciHist : public Base3A {
    DECLARE_DYNAMIC_CLASS()
public:
    DciHist() {}
    void init(const PBUS& inBus, const PBUS& outBus, shared_ptr<GlobalDataArea>& dataArea);
    using mConfigType = shared_ptr<VVDciSettings>;
    void onMessageDciConfig(mConfigType& s);
    void onMessageGetDciConfig(VVDciSettings*& s);
    void getDciCurve(Matrix<uint16_t>& m);
    void load(const Json::Value& node);

private:
    using histType = shared_ptr<Hist64EventData>;
    void onMessageHist64Stats(histType& data);

    void pdf_clip(int *hist_bin, double *P, int n);

    void CDF(double *P, int maxValue, double *cdf, int *curve_HE, int n);

    void curve32to65(int *curve_HE, int *curve_Y);

    void oneSection_process(int *hist_bin, int *curve_HE);

    void twoSection_process(int *hist_bin, int *curve_HE);

    void threeSection_process(int *hist_bin, int *curve_HE);

    void gaussCoeff(int gauss_mean, int gauss_sigma, double gauss_amp, double *coeff);

    void curveDelta(int *curve_Y, double *coeff_pos, double *coeff_neg, double *coeff);

    void curve_merge(int *curve_Y, double *coeff, int* curve_cur);

    void curve_merge2(int *curve_cur, float base, float strength);

    bool enable = false;
    float scale = 1;
    float step = 4.0f;
    float base = 1.3f;
    float strength = 0.5f;
    float damp = 0.5f;
    #define DCI_BIN 65
    int curve_cur[DCI_BIN];
    Matf dampedCurve = Matf(1, DCI_BIN, 0.f);
    bool updatedCurve = false;

    float DCI_gauss_mean_pos = 64;
	float DCI_gauss_sigma_pos = 128;
	float DCI_gauss_amp_pos = 1.0;
	float DCI_gauss_mean_neg = 0;
	float DCI_gauss_sigma_neg = 128;
	float DCI_gauss_amp_neg = 1.0;
};

}
#endif
