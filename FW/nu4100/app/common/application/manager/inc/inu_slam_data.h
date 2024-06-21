#ifndef _INU_SLAM_DATA_H_
#define _INU_SLAM_DATA_H_


#include "inu_data.h"
#include "inu2_types.h"


typedef struct inu_slam_data
{
   inu_data data;
   inu_slam_data__hdr_t  slam_dataHdr;
   void *privP;
}inu_slam_data;

typedef struct inu_slam_data_CtorParamsTag
{
   inu_data__CtorParams dataCtorParams;
}inu_slam_data__CtorParams;

void inu_slam_data__vtable_init(void);
const inu_data__VTable *inu_slam_data__vtable_get(void);


#define MAX_KP (800)
#define XM4_MAX_KEYFRAMES (1200)
#define XM4_MAX_MAP_POINTS (32767)
#define XM4_MAX_EXTRA_MAP_POINTS (32767)
#define XM4_OBSERVATIONS_NUM_1 (20)	
#define XM4_OBSERVATIONS_NUM_2 (200)
#define XM4_MAX_RECENTLY_LIST_SIZE (2400)
#define SLAM_MAX_WIDTH (640)
#define SLAM_MAX_HEIGHT (480)


typedef struct
{
	float b_acc_x;
	float b_acc_y;
	float b_acc_z;
	float b_ang_vel_x;
	float b_ang_vel_y;
	float b_ang_vel_z;
} BiasT;

typedef struct
{
	BiasT Inu_b;
	float Inu_dT;
	float Inu_dR[9];
	float Inu_dV[3];
	float Inu_dP[3];
	float Inu_JRg[9];
	float Inu_JVg[9];
	float Inu_JVa[9];
	float Inu_JPg[9];
	float Inu_JPa[9];
	float Inu_C[15 * 15];
	float Inu_avgA[3];
	float Inu_avgW[3];
	float Inu_Nga[6 * 6];
	float Inu_NgaWalk[6 * 6];
} PreIntegratedT;


typedef struct {
	float quaternion[4];
	float translation[3];
}poseT;

typedef struct {
	int quaternion[4];
	int translation[3];
}poseFixedPointT;

typedef struct
{
	unsigned char descriptorsCurRight[MAX_KP][DESCRIPTOR_SIZE];
	int kpCurRightX[MAX_KP];
	int kpCurRightY[MAX_KP];
	int ssRight[MAX_KP];
	short curPrevPairsIdx[MAX_KP];
	int leftRightPairsCurIndex1[MAX_KP];
	int leftRightPairsCurIndex2[MAX_KP];
	int kpCurRightWithLeftIndexX[MAX_KP];
	int curPrevPairsSize;
	int leftRightPairsCurSize;
	int descriptorsCurRightSize;
	int kpCurRightSize;
}FrameXM4_shortTermFrameT;


typedef struct
{
	unsigned char descriptorsCurLeft[MAX_KP][DESCRIPTOR_SIZE];
	int depth[MAX_KP];
	int kpCurLeftX[MAX_KP];
	int kpCurLeftY[MAX_KP];
	int Sigma_s[MAX_KP];
	int ssLeft[MAX_KP];
	int mapPointsX[MAX_KP];
	int mapPointsY[MAX_KP];
	int mapPointsZ[MAX_KP];
	unsigned long mappointDdrIndex[MAX_KP];
	unsigned char isInlier[MAX_KP];
	int pcCur0[MAX_KP];
	int pcCur1[MAX_KP];
	int descriptorsCurLeftSize;
	int descSizeBytes;
	int kpCurLeftSize;
	int numberOfInliers;
	int numberOfTrackedMappoints2;
	int numberOfTrackedMappoints3;
	poseT pose;
	int frameNumber;
	int keyframeIndex;
	int keyframeCreatedOrder;
	int pcCurSize;
	UINT64 timestamp;
	PreIntegratedT pImuPreintegratedFrame;
	PreIntegratedT keyFramePreintegrateIMU;
}FrameXM4_longTermFrameT;


typedef struct
{
	FrameXM4_longTermFrameT longTerm;
	FrameXM4_shortTermFrameT shortTerm;
}FrameXM4_frameT;

typedef struct
{
	unsigned short weight[XM4_MAX_KEYFRAMES];
	unsigned short ConnectedIdx[XM4_MAX_KEYFRAMES];
	int available;
}KeyframesXM4_connectedKeyframesT;



typedef struct
{
	int capacity;
	int available;
	int globalNextFreeIndex;
	int totCreatedKeyframes;
	int indexManager[XM4_MAX_KEYFRAMES];
#ifdef XM4_RUN_FLOAT_KF_INSERTION
	int tempDebugKeyframeID[XM4_MAX_KEYFRAMES];
#endif
	int referenceIndex;
	FrameXM4_longTermFrameT ddrKeyframeBuffer[XM4_MAX_KEYFRAMES];
}KeyframesXM4_keyframesT;





typedef struct
{
	unsigned short Kfindex;
	unsigned short Kpindex;
}MapXM4_observationT;


typedef struct //	128 bytes
{
	unsigned long nextFreeMapPoint; //linked list
	int next; // in case of nObs > OBSERVATIONS_NUM_1
	short nObs; // number of observing key frames
	short isMp; // is map point available
	int mpXYZ[3];
	int trackedFrameIndex;
	int foundMapPointsNumber;
	int visibleMapPointsNumber;
	int mapPointIndex;
	int mapPointCreatedIndex;
	MapXM4_observationT observations[XM4_OBSERVATIONS_NUM_1]; // pairs of KF and KP indices
}MapXM4_mappointT;


typedef struct
{

	int nextFreeMapPoint; //linked list
	int next; // in case of nObs > OBSERVATIONS_NUM_1
	short nObs; // number of observing key frames
	short isM; // is map point available
	MapXM4_observationT observations[XM4_OBSERVATIONS_NUM_2]; // pairs of KF and KP indices
}MapXM4_extraMappointT;


typedef struct
{
	int capacity;
	int available;
	int firstKeyframeIndex[XM4_MAX_RECENTLY_LIST_SIZE];
	unsigned long mapPointIndex[XM4_MAX_RECENTLY_LIST_SIZE];
}MapXM4_recentlyAddedTList;


typedef struct
{
	int capacity;
	int available;
	unsigned long globalNextFreeIndex;
	int capacityExtraMP;
	int availableExtraMp;
	int globalNextFreeIndexExtraMP;
	int totCreatedMappoints;
	MapXM4_mappointT mapPoints[XM4_MAX_MAP_POINTS];
	MapXM4_extraMappointT extraMapPoints[XM4_MAX_EXTRA_MAP_POINTS];
	MapXM4_recentlyAddedTList recentlyAddedSt;
}MapXM4_mappointsT;

#endif


