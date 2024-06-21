#ifndef _INU_SLAM_API_H_
#define _INU_SLAM_API_H_

#include "inu2.h"
#include "inu_slam_data_api.h"

#define SLAM_MAX_KP       (1600)
#define SLAM_MAX_PIC_KP   (800)
#define DESCRIPTOR_SIZE   (64)
#define SLAM_Q_SCALE      (8)
#define SLAM_Q_KP         (14)
#define SLAM_DATA_IMU_SAMPLES (20)

typedef void* inu_slamH;

typedef struct
{
   inu_graphH  graphH;
   inu_deviceH deviceH;
}inu_slam__initParams;

typedef struct 
{
   float acc[3];
   float gyro[3];
   UINT64 timeStamp;
}inu_slam_imuDataInT;

typedef struct
{
   int secondBestTh_100;
   int radiusThreshold;
   int HammingDistanceThreshold;
   int scaleDifferenceThreshold;
} StereoMatcherT;

typedef struct
{
   int ratioThreshold_100;
   int radiusThreshold;
   int HammingDistanceThreshold;
   float scaleDifferenceThreshold;
} FrameMatcherT;

typedef struct
{
   int UseKeyframes;
   int numberOfKeyframesForMatching;
   // minimum weight for two keyframes to be connected in the covisibility graph
   int connectedKeyframesThreshold;
   int numTrackedClosePointsThreshold;
   int numNonTrackedClosePointsThreshold;
} KeyframesT;

typedef struct
{
   float mapPointCullRatioThr;
   int observingKeyframesThr;
   int keyframeIndexDiffThr;
   int keyframeIndexDiffThr2;
} MapPointCullingT;

typedef struct
{
   int observationThreshold;
   int redundantMapPointThreshold_100;
} KeyframeCullingT;

// Tracking parameters
typedef struct
{
   int MaximumDepthThreshold;
   int minimumDepthThreshold;
   float ClosePointsDepthThreshold;
   int rhoThresholdMono;
   int rhoThresholdStereo;
   ///////////////
   StereoMatcherT StereoMatcherSt;
   FrameMatcherT FrameMatcherSt;
   KeyframesT KeyframesSt;
   FrameMatcherT KeyframeMatcherSt;
   MapPointCullingT MapPointCullingSt;
   KeyframeCullingT KeyframeCullingSt;
} TrackingT;

// Local Bundle Adjustment parameters 
typedef struct
{
   int RunLocalBundleAdjustment;
   int numberOfKeyframesForLBA;
} LocalBundleAdjustmentT;

// Localization parameters 
typedef struct
{
   // 0/1 run relocalization
   int RunRelocalization;
} LocalizationT;

typedef struct
{
   float baseline;
   float fx;
   float fy;
   float cx;
   float cy;
   float IMU_freq;
   float NoiseGyro;
   float NoiseAcc;
   float GyroWalk;
   float AccWalk;
   TrackingT TrackingSt;
   LocalBundleAdjustmentT LocalBundleAdjustmentSt;
   LocalizationT LocalizationSt;
   int fps;
}inu_slam__parameter_list_t;

#define SLAM_DATA_IMU_SIZE (SLAM_DATA_IMU_SAMPLES * sizeof(inu_slam_imuDataInT))

ERRG_codeE inu_slam__new(inu_slamH *meH, inu_slam__initParams *initParamsP);
void inu_slam__delete(inu_slamH meH);
ERRG_codeE inu_slam__send_data_async(inu_slamH meH, char *bufP, unsigned int len);
ERRG_codeE inu_slam__updateParams(inu_slamH meH, inu_slam__parameter_list_t* cfgParamsP);

#endif

