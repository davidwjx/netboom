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

enum META_TYPE {
    META_EXP,
    META_WB,
    META_LSC
};

struct Vsi3AMetaExp {
    float expsoureTime;//sensor exposure time
    float expsoureGain;//sensor Expsoure  total Gain
    float ispDgain;  // isp Digit Gain
    bool metaDataEnableAE;  /*False - Enable AE, True = Disable AE*/
    bool hasmetaDataEnableAE;
    int (*ae_cb)(void *);   /*Callback when AE has been finished*/
    bool has_ae_cb;         /*True = Write callback value*/
};

struct Vsi3AMetaWb {
    float awbGain[4];//rGain grGain gbGain bGain
    float CCM[9];
    float offset[3];
    //awb OTP data
    int enableOTP;
    float  ct;  //color tempture
    bool metaDataEnableAWB; /*False=Enable AWB, True = Disable AWB*/
    bool hasmetaDataEnableAWB;
    int (*awb_cb)(void *);   /*Callback when AWB has been finished*/
    bool has_awb_cb;         /*True = Write callback value*/
};

#define LSCTABLE_SIZE (17*17*4)
struct Vsi3AMetaLsc {
    int enableOTP;
    uint16_t lscTable[LSCTABLE_SIZE];//
};

struct Vsi3AMetaData {
    int type; //META_TYPE

    union {
        //awb sync
        struct Vsi3AMetaWb wb;
        //AE sync
        struct Vsi3AMetaExp exp;
        // LSC table
        struct Vsi3AMetaLsc wb_lsc;
    } meta;
};

struct PD_out
{
	int pd_shift;                                      //final pd shift.
	float corrected_pd;                                //correcte pd shift.
	int confidence;                                    //final confidence of pd_shift.
	int MSE;                                           //reference data for contrast 
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
#define ROI_MAX_NUM 25
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
    uchar *mean;
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
	struct PD_out pd_output;
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

enum {
    AF_AUTO_MODE_OFF = 0,
    AF_AUTO_MODE_MANUAL,
    AF_AUTO_MODE_AUTO,
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

struct VsiOTPLsc {
    uint16_t r[17][17];
    uint16_t gr[17][17];
    uint16_t gb[17][17];
    uint16_t b[17][17];
};
 struct VsiOTPAwb {
    uint16_t r;
    uint16_t gr;
    uint16_t gb;
    uint16_t b;
    uint16_t rg_ratio;
    uint16_t bg_ratio;
};

struct VsiOTPLightSource {
    uint16_t CIE_x;
    uint16_t CIE_y;
    uint16_t Intensity;
};

struct VsiOTPFocus {
    uint16_t Macro;
    uint16_t Inif;
};
struct VsiOTPModeInformation {
    uint16_t HWVersion;
    uint16_t EEPROMRevision;
    uint16_t SensorRevision;
    uint16_t TLensRevision;
    uint16_t IRCFRevision;
    uint16_t LensRevision;
    uint16_t CARevision;
    uint16_t ModuleInteID;
    uint16_t FactoryID;
    uint16_t MirrorFlip;
    uint16_t TLensSlaveID;
    uint16_t EEPROMSlaveID;
    uint16_t SensorSlaveID;
    uint8_t  SensorID[11];
    uint16_t ManuDateYear;
    uint16_t ManuDateMonth;
    uint16_t ManuDateDay;
    uint8_t  BarcodeModuleSN[12];
    uint16_t MapTotalSize;

};

struct VVOtpSettings{
   uint32_t OTP_version;
   struct VsiOTPModeInformation OTPInformation;
   bool   OTPLscEnable;
   bool   OTPAwbEnable;
   bool   OTPLightSourceEnable;
   bool   OTPFocusEnable;
   struct VsiOTPLsc LscCTM;
   struct VsiOTPAwb AwbCTL;
   struct VsiOTPAwb AwbCTM;
   struct VsiOTPAwb AwbCTH; 
   struct VsiOTPAwb GoldenAwbCTL;
   struct VsiOTPAwb GoldenAwbCTM;
   struct VsiOTPAwb GoldenAwbCTH; 
   struct VsiOTPLightSource LightSourceCTL;
   struct VsiOTPLightSource LightSourceCTM;
   struct VsiOTPLightSource LightSourceCTH;  
   struct VsiOTPFocus Focus;
   bool DataCheckResult;
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
struct VVAfSettings {
    int mode; 
    bool enable; 
    float stableThreshold;
    float stableTolerance; 
    int maxFocal; 
    int minFocal; 
    int step;	
	float pdMseTolerance;   // PDAF tuning parameter: tolerance of MSE for stability judgement.
	float pdConfThreshold;   // PDAF tuning parameter: threshold of PDAF valid.
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
