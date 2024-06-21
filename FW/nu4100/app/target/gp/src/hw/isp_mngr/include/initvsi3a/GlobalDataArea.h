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
#ifndef VSI_3AV2_GLOBAL_DATA_AREA_H_
#define VSI_3AV2_GLOBAL_DATA_AREA_H_
#include "vsi3a_types.h"
#include "Common3ADefs.h"

namespace vsi3av2 {
struct GlobalDataArea {
    struct {
        struct Vsi3ARect rc;
        uchar aev1Mean[AE_MEAN_SIZE];
        uchar aev2Mean[AEV2_MEAN_SIZE];
        uchar aehdrMean[3][AE_MEAN_SIZE];
        LimitedScalar3f integrationTime;
        LimitedScalar3f gain;
        LimitedScalar3f exposure;
        float backLightRatio;
        float meanLuma;
        float vsGain;
        float vsIntegrationTime;
        float setPoint = 50;
        float dampOver = 0.7;
        float dampUnder = 0.7;
        float tolerance = 20;
        float semSetPoint = 50;
        Matrix<uchar> semObject = Matrix<uchar>(5, 5, uchar(0));
        bool asemModeHigh = false;  // high/low
        int thresh_S, thresh_L, thresh_VS;
        bool aev1Updated = false;
        bool aev2Updated = false;
        bool aehdrUpdated = false;
        bool histhdrUpdated = false;
        bool bScaleAev2Window = true;
        float semLuma[25];
        bool initExposure = false;
        float motion = 0;
        float entropy = 0;
        int status;
        float maxSensorAgain = 15.5;
        float maxSensorDgain = 15;
        float maxIspDgain = 1;
        vector<struct Vsi3AROI> rois;
    } aec;

    struct {
        struct Vsi3ARect rc;
	    uint weight[25];
        uint histMean[HIST_MEAN_SIZE];
        uint hist256Mean[HIST256_MEAN_SIZE];
        uint histhdrMean[3][HIST_MEAN_SIZE];
        uint hist64Mean[32];
    } hist;

    struct {
        struct Vsi3ARect rc;
        struct {
            uint r, g, b, w;
        } mean;
        struct {
            float m[9];
            float r, g, b;
        } ccm;
        struct {
            float r, gr, gb, b;
        } gain;
        struct {
            uint no_white_pixels = 0;
            uint d_no_white_pixels;
        } ctx;

        struct {
            float r, g, b;
        } avg;

        Matf DampedCcMatrix = Matf(3, 3, 0.f);
        Matf DampedCcOffset = Matf(1, 3, 0.f);
        Matf wbGains;
        vector<Matf> vecGains;
        std::string nearestName;
        int regionType;
        float regionSize;
        float expPriorIndoor;
        float expPriorOutdoor;
        float iIRDampCoeff;
        vector<struct Vsi3AROI> rois;
    } awb;

    struct {
        bool OTPLscEnable;
        bool OTPAwbEnable;
        bool OTPFocusEnable;
        uint16_t lsc[4][289];
	    
        uint16_t r[6];
        uint16_t gr[6];
        uint16_t gb[6];
        uint16_t b[6];
        uint16_t rg_ratio[6];
        uint16_t bg_ratio[6];

        uint16_t Macro;
        uint16_t Inif;	
    } otp;

    struct {
        bool enabled;
        int extBit;
        float ratioFixed;
        float ratio;
        float transangeStart;
        float transangeEnd;
        int mode;
    } hdr;

    struct {
        bool enabled;
    } dehaze;

    struct {
        float gammapre = 0.1;
        ushort gammadown[14];
        uint histogram[14];
        uint entropy[14];
        uint gammaup[14];
        uint distanceweight[14];
        uint diffweight[14];
        uint invertlinear[14];
        uint invertcurve[14];
        int localStrength = 127;
        uchar merge_slope=127, merge_base=0;
        uchar adjust_slope=127, adjust_base=0;
        ushort entropy_slope=204, entropy_base=716;
        int sigmawidth, sigmaheight, sigmavalue;
    } wdr3;

    struct {
        vector<vsi3av2::Point<float>> centers;
        bool enabled = false;
    } face;

    struct {
        bool enabled;
        bool workWithAwb = true;
        Matf DampedLscMatrix[4];
    } lsc;

    struct {
        float cur_x = 0;
        float cur_y = 0;
        float smooth_x = 0;
        float smooth_y = 0;
    } vsm;

    struct {
        bool enable;
        uint sharpness[3];
        uint luma[3];
        float lensFocal;
        int status;
        int internalStatus;
    } af;

    struct {
        uint16_t curve[65];
    } dcihist;

    struct {
        bool enable = false;
        float gain = 1;
    } dgain;

    struct {
        bool enable = false;
        float gain = 1;
    } wdr4;

    struct {
        bool enable = false;
    } a2dnrv5;
    struct {
        uint32_t bg_sum;
        uint32_t motion_sum;
        uint32_t bg_pixel_cnt;
        uint32_t motion_pixel_cnt;
        uint32_t frame_avg;
        int bStatus = -1;
        int noiseLevel = 0;
    } tdnr;
};

}
#endif
