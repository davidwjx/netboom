/****************************************************************************
 *
 *   FileName: data_base.h
 *
 *   Author: Eyal Amiel
 *
 *   Date: 13/10/2013
 *
 *   Description: Define shared data base bitween GP and CEVA
 *
 ****************************************************************************/
#ifndef DATA_BASE_H
#define DATA_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu2.h"
#include "inu_types.h"
#include "icc_common.h"

#include "os_lyr.h"
#include "mem_map.h"


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
#define DATA_BASE_ALG_COMMAND_START_BIT (0)
#define DATA_BASE_ALG_COMMAND_STOP_BIT  (1)
#define DATA_BASE_ALG_COMMAND_FRAME_BIT (2)
#define DATA_BASE__MAX_NUM_LIST (INU_FUNCTION__MAX_NUM_INPUTS)

typedef enum
{
   DATA_BASEG_INVALID_SECTION = 0xFFFFFFFF,
   DATA_BASEG_ICC_BOX_GP_DSPA = 0,
   DATA_BASEG_ICC_BOX_GP_DSPB,
   DATA_BASEG_ICC_BOX_DSPA_GP,
   DATA_BASEG_ICC_BOX_DSPB_GP,
   DATA_BASEG_JOB_DESCRIPTORS,
   DATA_BASEG_FAST,
   DATA_BASEG_CDNN,
   DATA_BASEG_CDNN_EV,
   DATA_BASEG_VISION_PROC,
   DATA_BASEG_SLAM,
   DATA_BASEG_SLAM_EV,
   DATA_BASEG_DPE_PP,
   DATA_BASEG_PP,
   DATA_BASEG_TSNR,
   DATA_BASEG_GP_TO_XM4_FDK_MSG,
   DATA_BASEG_GP_TO_EV62_FDK_MSG,
   DATA_BASEG_HISTOGRAM,
   DATA_BASEG_NUM_DATABASE_E
} DATA_BASEG_databaseE;

typedef enum
{
   DATA_BASEG_ALG_GENERAL_MSG,
   DATA_BASEG_ALG_TEST_SHORT,
   DATA_BASEG_ALG_TEST_LONG,
   DATA_BASEG_ALG_HISTOGRAM,
   DATA_BASEG_ALG_FAST_ORB,
   DATA_BASEG_ALG_CDNN,
   DATA_BASEG_ALG_VISION_PROC,
   DATA_BASEG_ALG_SLAM,
   DATA_BASEG_ALG_DPE_PP,
   DATA_BASEG_ALG_PP,
   DATA_BASEG_ALG_TSNR,
   DATA_BASEG_SLEEP,
   DATA_BASEG_TARGET_ALG_NUM
} DATA_BASEG_targetAlgJobE;


typedef struct __attribute__((packed, aligned(4)))
{
   UINT32   msgReady;
   UINT32   msgSize;
   UINT8    msgBuf[INU_FDK__GENERAL_MSG_MAX_SIZE];
} DATA_BASEG_clientMsgDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32                        workspaceAddress;
   UINT32                        workspaceSize;
} DATA_BASEG_ddrWorkspaceDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32                rightShift;
   UINT32                x0;
   UINT32                y0;
   UINT32                x1;
   UINT32                y1;
   inu_image__descriptor imgDescriptor;
} DATA_BASEG_histogramDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32 dummy;
} DATA_BASEG_fastDataDataBaseT;

typedef enum
{
   CDNN_CEVAG_START_ALG_E = 0,
   CDNN_CEVAG_STOP_ALG_E  = 1,
   CDNN_CEVAG_FRAME_ALG_E  = 2
} CDNN_CEVAG_opcodeE;


typedef struct __attribute__((packed, aligned(4)))//  Algorithm Parameters
{
   UINT32 enableAlg;    // enable/disable
   UINT32 startX;       // ROI start
   UINT32 startY;       // ROI start
   UINT32 width;        // ROI width
   UINT32 height;       // ROI height
   UINT32 channel;      // Video/Webcam
} CDNN_CEVAG_algConfigT;


typedef struct __attribute__((packed, aligned(4)))//  Frame Parameters
{
   UINT32                  inPhysicalAddr;
   UINT32                  outPhysicalAddr;
   UINT32                  bufferWidth;
   UINT32                  bufferHeight;
   UINT32                  frameWidth;
   UINT32                  frameHeight;
} CDNN_CEVAG_FrameParamsT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32                 baseAddr;
   UINT32                 sourceAddr;
   UINT32                 destinationAddr;
   UINT32                 loopCounter;
   UINT16                 dmaCore;
   UINT16                 dmaChannel;
} DATA_BASEG_cnnReloadDmaCopyParamsT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32 networkId;
   UINT8 *ddrP;
   UINT32 ddrSize;
   UINT8 *networkP;
   UINT32 networkSize;
   UINT8 *networkResultsBlockP;
   UINT32 networkResultsBlockSize;
   UINT8* cdnnEngineExtMemoryP;
   UINT32 cdnnEngineExtMemorySize;
   UINT32 cdnnEngineIntMemorySize;
   UINT32 isRGB;
   UINT32 numImages;
   UINT32 pixelSize;
   INT32 handle;
   CDNNG_inputImageRawScaleE rawScale;
   CDNNG_channelSwapE channelSwap;
   CDNNG_resultsFormatE resultsFormat;
   UINT32 makeImageSquare;
   CDNNG_bitAccuracyE bitAccuracy;
   UINT32 numChannels;
   CDNNG_channelPixelTypeE pixelTypeE;
   CDNN_CEVAG_opcodeE      opcode;
   CDNN_CEVAG_algConfigT   algConfig;
   CDNN_CEVAG_FrameParamsT FrameParams;
   DATA_BASEG_cnnReloadDmaCopyParamsT  dmaCopyParams;
   UINT32                              pipeDepth;
   UINT32                              ProcesssingFlags;

} DATA_BASEG_cdnnDataDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32 networkId;
   UINT8 *ddrP;
   UINT32 ddrSize;
   UINT8 *networkP;
   UINT32 networkSize;
   UINT8 *networkResultsBlockP;
   UINT32 networkResultsBlockSize;
   UINT8* cdnnEngineExtMemoryP;
   UINT32 cdnnEngineExtMemorySize;
   UINT32 cdnnEngineIntMemorySize;
   UINT32 isRGB;
   UINT32 numImages;
   UINT32 pixelSize;
   INT32 handle;
   CDNNG_inputImageRawScaleE rawScale;
   CDNNG_channelSwapE channelSwap;
   CDNNG_resultsFormatE resultsFormat;
   UINT32 makeImageSquare;
   CDNNG_bitAccuracyE bitAccuracy;
   UINT32 numChannels;
   CDNNG_channelPixelTypeE pixelTypeE;
   CDNN_CEVAG_opcodeE      opcode;
   CDNN_CEVAG_algConfigT   algConfig;
   CDNN_CEVAG_FrameParamsT FrameParams;
   VISION_PROCG_visionProcAlgTypeE algType;
   VISION_PROCG_bokehParamsT bokehParams;
   VISION_PROCG_backgroundRemovalParamsT backgroundRemovalParams;
   VISION_PROCG_lowLightIspCalcT lowLightIspCalc;
   VISION_PROCG_autoFramingParamsT autoFramingParams;
   UINT32 idsrLutAddr;
}DATA_BASEG_visionProcDataDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   float baseline;
   float fx;
   float fy;
   float cx;
   float cy;
   float IMU_freq;
   float IMU_Ng;
   float IMU_Na;
   float IMU_Ngw;
   float IMU_Naw;
   TrackingT TrackingSt;
   LocalBundleAdjustmentT LocalBundleAdjustmentSt;
   LocalizationT LocalizationSt;
   int fps;
   int width;
   int height;
   int x_start;
   int y_start;
   float thDepth; 
   UINT32            inu_slamPrevFrameBufferPhyAddress;
   UINT32            inu_slamKeyframesBufferPhyAddress;
   UINT32            inu_slamMappointsBufferPhyAddress;
   UINT32            inu_slamCovisibilityGraphBufferPhyAddress;
} DATA_BASEG_slamDataDataBaseT; 

typedef struct __attribute__((packed, aligned(4)))
{
   UINT8 *ddrP;
   INT32 confidence_th;
   INT32 BlobMaxSize;
   INT32 BlobMaxHeight;
   INT32 BlobDispDiffSameBlob;
   #ifdef DISABLED_UNTIL_EV72_REBUILT
   INT32 TemporolRapidTh;
   INT32 TemporolStableTh;
   INT32 algType;
   #endif
} DATA_BASEG_dpeDataDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT8 *ddrP;
   INT32  actionMap;
   INT32 sensorGroup;
   float baseline;
   float fx;
   float fy;
   float cxL;
   float cxR;
   float cy;
   int maxDepthMm;
   int voxel_leaf_x_size;
   int voxel_leaf_y_size;
   int voxel_leaf_z_size;
   int vgf_flag;
   int flip_x;
   int flip_y;
   int flip_z;
   INU_PP__modelE model;
} DATA_BASEG_ppDataDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32 imgFormat;
   UINT32 workspacePhyAddr;
   UINT32 workspaceSize;
} DATA_BASEG_tsnrDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   UINT32 box;
} DATA_BASEG_iccBoxDataBaseT;

typedef struct __attribute__((packed, aligned(4)))
{
   void                 *inu_dataVirt[DATA_BASE__MAX_NUM_LIST];
   void                 *inu_dataPhy[DATA_BASE__MAX_NUM_LIST];
   UINT32               databaseListNum;
} DATA_BASEG_inputListT;

typedef struct __attribute__((packed, aligned(4)))
{
#if DEFSG_IS_CEVA
   const ICC_COMMONG_cmdCallbackT    cb; /* Used by GP only, should not be touched by DSP */
#else
   ICC_COMMONG_cmdCallbackT   cb;
#endif
   DATA_BASEG_inputListT      inputList;
   DATA_BASEG_targetAlgJobE   alg;
   UINT32                     data;
   UINT32                     arg;
   UINT32                     command;
} DATA_BASEG_jobDescriptorT;

typedef struct __attribute__((packed, aligned(4)))
{
   DATA_BASEG_jobDescriptorT        jobDescriptorPool[ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE];
} DATA_BASEG_iccJobsDescriptorDataBaseT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE DATA_BASEG_initDataBase();
ERRG_codeE DATA_BASEG_getDataBaseCpMutexId(OS_LYRG_cpMutexIdE *cpMutexIdP, DATA_BASEG_databaseE dataBaseNum);
ERRG_codeE DATA_BASEG_getDataBaseCpMutexHandle(OS_LYRG_mutexCpT **oCpMutexHandle, DATA_BASEG_databaseE dataBaseNum);
#if DEFSG_IS_CEVA
ERRG_codeE DATA_BASEG_readDataBaseNoMutex(UINT8 *outDataBaseP, DATA_BASEG_databaseE dataBaseNum )__attribute__ ((section (".CSECT isr_section")));
ERRG_codeE DATA_BASEG_writeDataBaseNoMutex(UINT8 *inDataBaseP, DATA_BASEG_databaseE dataBaseNum )  __attribute__ ((section (".CSECT isr_section")));
ERRG_codeE DATA_BASEG_readDataBase(UINT8 *outDataBaseP, DATA_BASEG_databaseE dataBaseNum, UINT8 holdDb) __attribute__ ((section (".CSECT isr_section")));
ERRG_codeE DATA_BASEG_writeDataBase(UINT8 *inDataBaseP, DATA_BASEG_databaseE dataBaseNum, UINT8 dbHolding) __attribute__ ((section (".CSECT isr_section")));
#else
void DATA_BASEG_accessDataBase(UINT8 **outDataBaseP, DATA_BASEG_databaseE dataBaseNum);
void DATA_BASEG_accessDataBaseRelease(DATA_BASEG_databaseE dataBaseNum);
void DATA_BASEG_accessDataBaseNoMutex(UINT8 **outDataBaseP, DATA_BASEG_databaseE dataBaseNum);
ERRG_codeE DATA_BASEG_readDataBaseNoMutex(UINT8 *outDataBaseP, DATA_BASEG_databaseE dataBaseNum );
ERRG_codeE DATA_BASEG_writeDataBaseNoMutex(UINT8 *inDataBaseP, DATA_BASEG_databaseE dataBaseNum );
ERRG_codeE DATA_BASEG_readDataBase(UINT8 *outDataBaseP, DATA_BASEG_databaseE dataBaseNum, UINT8 holdDb);
ERRG_codeE DATA_BASEG_writeDataBase(UINT8 *inDataBaseP, DATA_BASEG_databaseE dataBaseNum, UINT8 dbHolding);
#endif
void DATA_BASEG_show();

#ifdef __cplusplus
}
#endif
#endif //DATA_BASE_H
