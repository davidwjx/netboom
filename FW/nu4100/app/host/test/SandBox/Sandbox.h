#pragma once

#include <stdio.h>
#include <time.h>
#if (defined _WIN32) || (defined _WIN64)
#include <Windows.h>
#include <stdint.h> 
#endif
#include <vector>
#define MAX_STREAM_OUT_NAME_LEN 255

extern UINT8 testResult;

typedef struct SANDBOX_channelParams
{
   //stats
   int statsEnable;
   UINT64        startUs;
   UINT64        endUs;
   UINT64        totalFramesReceived;
   UINT32        printPeriod;
   UINT32        periodReceived;
   UINT64        prevCtr;
   UINT64        bytesReceived;
   UINT64        latencyMax;
   UINT64        latencyMin;
   UINT64        latencySum;
   UINT64        totalNumDropped;
   UINT64        periodNumDropped;
   float         minFPS;
   float         maxFPS;
   std::vector<UINT64> metadata_time_value;
   std::vector<UINT64> metadata_time_CVA_Left_value;
   std::vector<UINT64> metadata_time_CVA_Right_value;

   //record output
   int recordOutput;
   UINT32 recordOutputFrames;

   int recordPNG; /*Record as PNG for conveniance*/

   //display output
   int displayOutput;
   UINT32 bayerPattern;
   UINT32 bayer2RgbConvert;
   UINT32 typ;
   UINT32 fmt;
   char *bufferP = NULL;
   int yLoc;
   char winName[100];
   char stream_out_name[MAX_STREAM_OUT_NAME_LEN];
   FWLib_chH chH;
   int printMetadata;
   int sendSensorUpdates;
   int sendSampleSensorSyncUpdate;
   int sendROIUpdate;
   int sendSampleSwitchToTrackingSyncUpdate; /* 0 = Do not send switch to extern tracking sync signal synchronization
                                                1 = Send switch to external tracking sync signal synchronization*/
   int sendSampleSwitchToInternalTrackingSyncUpdate; /* 0 = Do not send switch to unternal tracking sync signal synchronization
                                                1 = Send switch to internal tracking sync signal synchronization*/
   int sendSampleAFVSTCropWindowUpdate; /*0 = Do not send AF VST Crop window update,  1= Send AF VST Crop window update */
   int recordMetadata;                          /* 1 = Record metadata, 0 = Don't record metadata*/
   int sendSampleAFVSTFocusPosition;            /* 1 = Send sample AF VST Focus change commands 0 = Don't send sample AF VST Focus change*/
   int sendSampleAFVSTWBGain;                   /* 1 = Send AF VST gain example, 0 = don't send AF VST wb gain*/
   int sendSampleAFVSTExpoDelayedUpdate;        /* 1 = Send Example where Expo update is delayed*/
   int recordCSVMetadata;                       /*Basic CSV of metadata*/
   int startAllStreams;
}SANDBOX_channelParamsT;

typedef struct SANDBOX_channelParamNode
{
   SANDBOX_channelParamsT *channelParam;
   struct SANDBOX_channelParamNode *next;
}SANDBOX_channelParamNodeT;

ERRG_codeE findConfigInTable(const char *name, void **configH);
ERRG_codeE setStreamRecordOutputFrames(const char* stream_name, UINT32 nFrames);