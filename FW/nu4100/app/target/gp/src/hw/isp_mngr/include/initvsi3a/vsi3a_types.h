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
#ifndef _VSI_3AV2_TYPES_
#define _VSI_3AV2_TYPES_

#include <stdint.h>

#ifndef MAX
#define MIN(x, y) (x) < (y) ? (x) : (y)
#define MAX(x, y) (x) > (y) ? (x) : (y)
#endif

typedef uint32_t uint;
typedef signed char schar;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef int64_t int64;
typedef uint64_t uint64;

struct Vsi3ARect { 
    int x;    // horizontal start position 
    int y;    // vertical start position
    int w;   // rect width
    int h;   // rect height
};

struct Vsi3AROI { 
    float fx;    // horizontal start position 
    float fy;    // vertical start position
    float fw;   // rect width
    float fh;   // rect height
    float weight;
};

enum eReturnCode_t {
    VSI_3A_RESULT_SUCCESS = 0,
    VSI_3A_RESULT_FAILED,
    VSI_3A_RESULT_INVALID_PARAM,
    VSI_3A_RESULT_NULL_POINTER,
    VSI_3A_RESULT_UNSUPPORTTED_MODE,
};


#define AE_MEAN_SIZE 25
#define AEV2_MEAN_SIZE 4096
#define HIST_MEAN_SIZE 16
#define HIST256_MEAN_SIZE 256

struct Aev1EventData {
    int mode;
    struct Vsi3ARect rc;
    uchar mean[AE_MEAN_SIZE];
};

struct Aev2EventData {
    int bitwidth;
    int select;
    struct Vsi3ARect rc;
    ushort r, gr, gb, b;
    float awb_r, awb_gr, awb_gb, awb_b;
    uchar mean[4096*2];
};

struct AehdrEventData {
    int mean_mode;
    int select;
    struct Vsi3ARect rc;
    uchar mean[AE_MEAN_SIZE*3];
};

struct AwbEventData {
    int mode;
    struct Vsi3ARect rc;
    uint r, g, b, w;
    struct {
        uint m[9];
        uint r, g, b;
    } ccm;
    struct {
        uint r, gr, gb, b;
    } gain;
};

struct AfEventData {
    struct Vsi3ARect rc[3];
    uint sum[3];
    uint lum[3];
};

struct HistEventData {
    struct Vsi3ARect rc;
	uchar weight[25];
    uint mean[16];
};

struct Hist256EventData {
    struct Vsi3ARect rc;
	uint weight[25];
    uint mean[HIST256_MEAN_SIZE];
};

struct HisthdrEventData {
    int mode;
    struct Vsi3ARect rc;
	uint weight[25];
    uint mean[HIST_MEAN_SIZE*3];
};

struct Hist64EventData {
    int channel;
    int mode;
    struct Vsi3ARect rc;
    uchar weight[25];
    uint mean[32];
};

struct VsmEventData {
    int mode;
    struct Vsi3ARect rc;
    uint segment_h, segment_v;
    int delta_x, delta_y;
};

// all status
enum e3aStatus_t {
    VSI_AEC_STATUS = 0,
    VSI_AWB_STATUS,
    VSI_AF_STATUS,
    VSI_AVS_STATUS,
};

enum e3aRuntimeData_t {
    VSI_AEC_RUNTIME_DATA = 0,
    VSI_AEC_RUNTIME_DATA_HIST16,
    VSI_AEC_RUNTIME_DATA_HIST256,
    VSI_AEC_RUNTIME_DATA_HISTHDR,
    VSI_AEC_RUNTIME_DATA_HIST64,
    VSI_AEC_RUNTIME_DATA_AEV1,
    VSI_AEC_RUNTIME_DATA_AEV2,
    VSI_AEC_RUNTIME_DATA_AEHDR,
    VSI_AEC_RUNTIME_DATA_OBJECTREGION,
    VSI_AWB_RUNTIME_DATA,
    VSI_SENSOR_RUNTIME_DATA,
    VSI_DCI_HIST_CURVE,
};



// all modes
enum e3aMode_t {
    VSI_AE_MODE = 0,
    VSI_AE_ANTIBANDING_MODE,
    VSI_AE_SCENE_EVALUATION_MODE,
    VSI_AF_MODE,
    VSI_AWB_MODE,
    VSI_AWB_MODE_CC_OFFSET,
    VSI_AWB_MODE_USE_DAMPING,
    VSI_AWB_MODE_ILLUM_INDEX,
    VSI_AHDR_MODE,
    VSI_AVS_MODE,
    VSI_AWDR3_MODE,
    VSI_DCIHIST_MODE,
    VSI_AEHDR_AUTORATIO_MODE,
    VSI_AEE_MODE,
    VSI_ATDNR_MODE,
    VSI_A2DNRV5_MODE,
    VSI_ACA_MODE,
    VSI_ADCI_MODE,
    VSI_ACPROC_MODE,
    VSI_ADPF_MODE,
    VSI_ADGAIN_MODE,
    VSI_ADMSC2_MODE,
    VSI_AGAMMA64_MODE,
    VSI_AWDR4_MODE,
};

// event and data
enum e3aStatistic_t {
    VSISTAT_EVENT_TYPE_BEGIN = 0,
    VSISTAT_EVENT_TYPE_AEV1,
    VSISTAT_EVENT_TYPE_AEV2,
    VSISTAT_EVENT_TYPE_AEHDR,
    VSISTAT_EVENT_TYPE_AWB,
    VSISTAT_EVENT_TYPE_AFM,
    VSISTAT_EVENT_TYPE_HIST,
    VSISTAT_EVENT_TYPE_HIST256,
    VSISTAT_EVENT_TYPE_HISTHDR,
    VSISTAT_EVENT_TYPE_HIST64,
    VSISTAT_EVENT_TYPE_VSM,
    VSISTAT_EVENT_TYPE_SENSOREXP,
    VSISTAT_EVENT_TYPE_END,
};

enum {
    VSI_AE_ANTIBANDING_MODE_OFF = 0,
    VSI_AE_ANTIBANDING_MODE_50HZ,
    VSI_AE_ANTIBANDING_MODE_60HZ,
    VSI_AE_ANTIBANDING_MODE_AUTO,
};

enum {
    VSI_AE_SCENE_EVALUATION_OFF = 0,
    VSI_AE_SCENE_EVALUATION_FIX,
    VSI_AE_SCENE_EVALUATION_ADAPTIVE,
    VSI_AE_SCENE_EVALUATION_MAX,
};

enum {
    VSI_COMMON_AUTO_MODE_OFF = 0,
    VSI_COMMON_AUTO_MODE_MANUAL,
    VSI_COMMON_AUTO_MODE_AUTO,
};

enum {
    VSI_AE_MODE_OFF = 0,
    VSI_AE_MODE_AUTO,
    VSI_AE_MODE_MANUAL,
};


enum {
    VSI_AWB_MODE_OFF = 0,
    VSI_AWB_MODE_MANUAL,
    VSI_AWB_MODE_AUTO,  
};

enum {
    VSI_AF_MODE_OFF = 0,
    VSI_AF_MODE_MANUAL,
    VSI_AF_MODE_AUTO,
};

enum {
    VSI_DCI_MODE_MANUAL = 0,
    VSI_DCI_MODE_AUTO,
    VSI_DCI_MODE_HIST,
};

enum {
    AEHDR_AUTORATIO_MODE_OFF = 0,
    AEHDR_AUTORATIO_MODE_MANUAL,
    AEHDR_AUTORATIO_MODE_AUTO,
};

// status
enum {
    VSI_AE_STATUS_INIT = 0,
    VSI_AE_STATUS_RUNNING,
    VSI_AE_STATUS_LOCKED,
    VSI_AE_STATUS_STOPPED,
};

enum {
    VSI_AWB_STATUS_INIT = 0,
    VSI_AWB_STATUS_RUNNING,
    VSI_AWB_STATUS_LOCKED,
    VSI_AWB_STATUS_STOPPED,
};

enum {
    VSI_AF_STATUS_INIT = 0,
    VSI_AF_STATUS_SEARCHFOCUS,
    VSI_AF_STATUS_FOCUSFOUND,
};


enum e3aConfig_t {
    VSI_CONFIG_TYPE_BEGIN = 0,
    VSI_CONFIG_TYPE_AE,
    VSI_CONFIG_TYPE_AWB,
    VSI_CONFIG_TYPE_HDR,
    VSI_CONFIG_TYPE_AF,
    VSI_CONFIG_TYPE_AVS,
    VSI_CONFIG_TYPE_DCI,
	VSI_CONFIG_TYPE_OTP,
    VSI_CONFIG_TYPE_AEHDR_AUTORATIO,
    VSI_CONFIG_TYPE_AEHDR_AUTORATIO_AUTO,
    VSI_CONFIG_TYPE_END,
};

// config
struct VVAecSettings {
    int semMode;
    int antibandingMode;
    float setPoint;
    float clmTolerance;
    float dampOver;
    float dampUnder;
};


struct VVDciSettings {
    int mode;
    float scale;
    float step;
    float base;
    float strength;
    float damp;
};

struct Vsi3AOTPLsc {
   uint16_t r[17][17];
   uint16_t gr[17][17];
   uint16_t gb[17][17];
   uint16_t b[17][17];
   uint16_t CheckSum;
};
 struct Vsi3AOTPAwb {
   uint16_t r;
   uint16_t gr;
   uint16_t gb;
   uint16_t b;
   uint16_t rg_ratio;
   uint16_t bg_ratio;
   uint16_t CheckSum;
};

struct Vsi3AOTPLightSource {
  uint16_t CIE_x;
  uint16_t CIE_y;
  uint16_t Intensity;
  uint16_t CheckSum;
};

struct Vsi3AOTPFocus {
  uint16_t Macro;
  uint16_t Inif;
};
struct Vsi3AOTPModeInformation {
  uint16_t HwVersion;
  uint16_t EEPROMVersion;
  uint16_t SensorVersion;
  uint16_t LensVersion;
  uint16_t CheckSum;

};

struct VVOtpSettings{
   uint32_t OTP_version;
   struct Vsi3AOTPModeInformation OTPInformation;
   bool   OTPLscEnable;
   bool   OTPAwbEnable;
   bool   OTPLightSourceEnable;
   bool   OTPFocusEnable;
   struct Vsi3AOTPLsc Lsc_4000k;
   struct Vsi3AOTPAwb Awb_3100k;
   struct Vsi3AOTPAwb Awb_4000k;
   struct Vsi3AOTPAwb Awb_5800k; 
   struct Vsi3AOTPAwb GoldenAwb_3100k;
   struct Vsi3AOTPAwb GoldenAwb_4000k;
   struct Vsi3AOTPAwb GoldenAwb_5800k; 
   struct Vsi3AOTPLightSource LightSource_3100k;
   struct Vsi3AOTPLightSource LightSource_4000k;
   struct Vsi3AOTPLightSource LightSource_5800k;  
   struct Vsi3AOTPFocus Focus;
   uint16_t CheckSum;
};
struct VVAehdrAutoRatioTable {
    float gain;
    float thy_vs;
    float thy_s;
    float c1, c2, ceil;
    int num;
};

struct VVAehdrAutoRatioConfig {
    float c1, c2, ceil;
    float setPoint2;
    float tolerance;
    float objectFactor;
    float maxRatio, minRatio;
};

struct VVAwbSettings {
    int mode; // manual, auto
    int measureMode; // ycbcr, rgb
    void *calibHandle;
    ushort width;
    ushort height;
    ushort framerate;
    uint flags;
    float mindev;
    float maxdev;
    bool useCCOffset;
    bool useDamping;
    struct {
        uchar maxY;
        uchar refCr_maxR;
        uchar minY_maxG;
        uchar refCb_maxB;
        uchar maxCSum;
        uchar minC;
    } measureConfig;
};

// config
struct VVSensorSettings {
    void *sensorHandle;
    void *calibHandle;
};

// config
struct VVHdrSettings {
    int extBit;
    float hdrRatio;
    float transrangeStart;
    float transrangeEnd;
};

// runtimedata
struct VVAecRuntimeStatus {
    bool running;
    int mode;
    float meanLuma;
    float backlightRatio;
    float motionFactor;
    float semSetPoint;
};

struct VVAwbRuntimeStatus {
    bool running;
    int mode;
    int iProfile;
    bool useDamping;
    bool useCCOffset;
    float fRgProjIndoorMin;
    float fRgProjOutdoorMin;
    float fRgProjMax;
    float fRgProjMaxSky;
};

struct VVSensorExpRuntimeStatus {
    float hdrRatioFixed;
    float hdrRatio;
    float integL, integS, integVS;
    float gainL, gainS, gainVS;
};

#endif
