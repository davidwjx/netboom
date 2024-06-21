/****************************************************************************
 *
 *   FileName: internal_cmd.h
 *
 *   Author: Ram B.
 *
 *   Date:
 *
 *   Description: Interna IOCTL/msg for services
 *
 ****************************************************************************/
#ifndef INU_INTERNAL_CMD_H
#define INU_INTERNAL_CMD_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_types.h"
#include "inu_defs.h"
#include "inu_cmd.h"
#include "xml_db.h"
#include "inu2.h"

#include "inu_storage_layout.h"
#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define MAX_FILE_NAME (256)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
//Service IOCTL commands
//8 MSbits are used for service ID mask
typedef enum
{
   //Private general service commands
   INTERNAL_CMDG_OPEN_SVC_E            = INU_CMDG_GEN_LAST_E,
   INTERNAL_CMDG_CLOSE_SVC_E,
   INTERNAL_CMDG_GET_BUF_E,
   INTERNAL_CMDG_SEND_BUF_E,
   INTERNAL_CMDG_WAIT_BUF_ACK_E,
   INTERNAL_CMDG_REQUEST_FILE_FROM_GP_E,
   INTERNAL_CMDG_SEND_FRAME_HDR_E,
   INTERNAL_CMDG_SEND_LUT_HDR_E,
   INTERNAL_CMDG_BURN_DYN_CALIB_E,
   INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_E,
   INTERNAL_CMDG_SEND_CDNN_RELEASE_ALL_E,
   INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_ALLOC_E,
   INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_E,
   INTERNAL_CMDG_SEND_BACKGROUND_RELEASE_E,
   INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_ALLOC_E,
   INTERNAL_CMDG_SEND_ALLOC_GP_BUF_E,
   INTERNAL_CMDG_SEND_FREE_GP_BUF_E,
   INTERNAL_CMDG_SEND_FILE_TO_GP_BUF_E,
   INTERNAL_CMDG_UPDATE_RAW_NAND_SECTION_E,
   INTERNAL_CMDG_SEND_CONFIG_DB_E,
   INTERNAL_CMDG_LOAD_CEVA_HDR_E,
   INTERNAL_CMDG_SEND_BUF_HDR_E,
   INTERNAL_CMDG_READ_BUF_HDR_E,
   INTERNAL_CMDG_EEPROM_READ_E,
   INTERNAL_CMDG_EEPROM_WRITE_E,
   INTERNAL_CMDG_GET_READ_FUNC_E,
   INTERNAL_CMDG_WRITE_FLASH_HDR_E,
   INTERNAL_CMDG_READ_FLASH_HDR_E,
   INTERNAL_CMDG_PING_E,
   INTERNAL_CMDG_PING_RESPOND_E,
   INTERNAL_CMDG_CONFIG_WD_E,
   INTERNAL_CMDG_GET_WRITE_FUNC_E,
   INTERNAL_CMDG_NEW_GRAPH_E,
   INTERNAL_CMDG_NEW_LOGGER_E,
   INTERNAL_CMDG_NEW_IMAGE_E,
   INTERNAL_CMDG_NEW_SOC_CH_E,
   INTERNAL_CMDG_NEW_CVA_CH_E,
   INTERNAL_CMDG_NEW_ISP_CH_E,
   INTERNAL_CMDG_NEW_HISTOGRAM_E,
   INTERNAL_CMDG_NEW_HISTOGRAM_DATA_E,
   INTERNAL_CMDG_NEW_SOC_WRITER_E,
   INTERNAL_CMDG_NEW_STREAMER_E,
   INTERNAL_CMDG_NEW_SENSOR_E,
   INTERNAL_CMDG_NEW_SENSORS_GROUP_E,
   INTERNAL_CMDG_NEW_IMU_E,
   INTERNAL_CMDG_NEW_IMU_DATA_E,
   INTERNAL_CMDG_NEW_TEMPERATURE_E,
   INTERNAL_CMDG_NEW_TEMPERATURE_DATA_E,
   INTERNAL_CMDG_NEW_FDK_E,
   INTERNAL_CMDG_NEW_DATA_E,
   INTERNAL_CMDG_NEW_CVA_DATA_E,
   INTERNAL_CMDG_NEW_SENSOR_CONTROL_E,
   INTERNAL_CMDG_NEW_FAST_ORB_E,
   INTERNAL_CMDG_NEW_FAST_ORB_DATA_E,
   INTERNAL_CMDG_NEW_CDNN_E,
   INTERNAL_CMDG_NEW_CDNN_DATA_E,
   INTERNAL_CMDG_NEW_VISION_PROC_E,
   INTERNAL_CMDG_CDNN_NETWORK_ID_SET_E,
   INTERNAL_CMDG_NEW_SLAM_E,
   INTERNAL_CMDG_NEW_SLAM_DATA_E,
   INTERNAL_CMDG_NEW_LOAD_NETWORK_E,
   INTERNAL_CMDG_NEW_LOAD_BACKGROUND_E,
   INTERNAL_CMDG_NEW_TSNR_CALC_E,
   INTERNAL_CMDG_NEW_TSNR_DATA_E,
   INTERNAL_CMDG_NEW_POINT_CLOUD_DATA_E,
   INTERNAL_CMDG_NEW_COMPRESS_E,
   INTERNAL_CMDG_NEW_DPE_PP_E,
   INTERNAL_CMDG_NEW_PP_E,
   INTERNAL_CMDG_NEW_MIPI_CH_E,
   INTERNAL_CMDG_TSNR_DATA_SEND_E,
   INTERNAL_CMDG_POINT_CLOUD_DATA_SEND_E,
   INTERNAL_CMDG_BIND_NODES_E,
   INTERNAL_CMDG_FUNCTION_START_E,
   INTERNAL_CMDG_FUNCTION_STOP_E,
   INTERNAL_CMDG_DATA_SEND_E,
   INTERNAL_CMDG_IMAGE_SEND_E,
   INTERNAL_CMDG_CVA_DATA_SEND_E,
   INTERNAL_CMDG_FAST_ORB_SEND_E,
   INTERNAL_CMDG_CDNN_DATA_SEND_E,
   INTERNAL_CMDG_SLAM_DATA_SEND_E,
   INTERNAL_CMDG_HISTOGRAM_DATA_SEND_E,
   INTERNAL_CMDG_IMU_DATA_SEND_E,
   INTERNAL_CMDG_TEMPERATURE_DATA_SEND_E,
   INTERNAL_CMDG_HISTOGRAM_ROI_CFG_E,
   INTERNAL_CMDG_DELETE_REF_E,
   INTERNAL_CMDG_GET_DEVICE_REF_LIST_E,
   INTERNAL_CMDG_GET_IMU_TEMPERATURE_E,
   INTERNAL_CMDG_READ_BUF_TRANSFER_E,
   INTERNAL_CMDG_WRITE_BUF_TRANSFER_E,
   INTERNAL_CMDG_CHANGE_CROP_WINDOW_E,
   INTERNAL_CMDG_SENSOR_CONTROL_UPDATE_E,
   INTERNAL_CMDG_SENSOR_CONTROL_GET_PARAMS_E,
   INTERNAL_CMDG_RESET_FRAME_CNT_E,
   INTERNAL_CMDG_UPDATE_CHANNEL_USER_PARAM_E,
   INTERNAL_CMDG_PP_UPDATE_E,
   INTERNAL_CMDG_VISION_PROC_UPDATE_E,
   INTERNAL_CMDG_SLAM_UPDATE_E,
   INTERNAL_CMDG_SEND_ISP_COMMAND_E,
   INTERNAL_CMDG_NEW_METADATA_INJECTOR_E,
   INTERNAL_CMDG_LAST_E,

   //////////////////////////////////////////////////
   //Specific private service commands
   //Private System Service
   INTERNAL_SYSTEM_SET_CONFIG      = INUG_IOCTL_SYSTEM_LAST_E,
   INTERNAL_SYSTEM_LAST_E,

   //Logger Service
   INTERNAL_CMDG_LOG_DATA_HDR_E     = INUG_IOCTL_LOGGER_LAST_E,
} INTERNAL_CMDG_ioctlCmdE;

typedef struct
{
   UINT32 serviceId;
} INTERNAL_CMDG_openSvcT;

typedef struct
{
   UINT32 serviceId;
} INTERNAL_CMDG_closeSvcT;

// INTERNAL_CMDG_GET_BUF_E
typedef struct
{
   void     *bufDescP;
   UINT16   mode;
   UINT32   bufCntr;
   UINT64   gpTimeNs;
   UINT32   hostTimeSec;
   UINT32   hostTimeMs;
   void     *specificStreamDataP;
} INTERNAL_CMDG_getBufT;

typedef INT32 (*INTERNAL_CMDG_readFuncT)(INTERNAL_CMDG_getBufT *bufP, UINT32 id, UINT32 msec);

typedef struct
{
   INTERNAL_CMDG_readFuncT read;
} INTERNAL_CMDG_getReadFuncT;

// INTERNAL_CMDG_SEND_BUF_E
typedef struct
{
   INUG_dataTypeE dataType;
   UINT32         numBytes;
   void          *bufP;
} INTERNAL_CMDG_sendBufT;

typedef ERRG_codeE (*INTERNAL_CMDG_writeFuncT)(INTERNAL_CMDG_sendBufT *bufP, UINT32 msec);

typedef struct
{
   INTERNAL_CMDG_writeFuncT write;
}INTERNAL_CMDG_getWriteFuncT;

// INTERNAL_CMDG_WAIT_BUF_ACK_E
typedef struct
{
   INUG_dataTypeE    dataType;   //dataType
} INTERNAL_CMDG_waitBufAckT;

typedef struct
{
   UINT32   interruptStatus;
   UINT32   interrupt1Status;
   UINT32   interrupt2Status;
} INTERNAL_CMDG_iaeIntMsgT;

typedef struct
{
   UINT32   status;
} INTERNAL_CMDG_dpeIntMsgT;

typedef struct
{
   UINT32   dmaChannel;
   UINT32   status;
   UINT64   timeStamp;
} INTERNAL_CMDG_cdeIntMsgT;

// INTERNAL_CMDG_SEND_FRAME_HDR_E
typedef struct
{
   UINT32   streamId;
   UINT32   channelId;
   UINT16   mode;
   UINT16   frameNum;
   UINT32   frameWidth;
   UINT32   frameHeight;
   UINT32   dmaStartX;
   UINT32   dmaStartY;
   UINT32   bufferWidth;
   UINT32   bufferHeight;
   UINT16   fragNum;
   UINT32   fragLen;
   UINT32   fragsInFrame;
   UINT32   frameCntr;
   UINT64   gpTimeNs;
} INTERNAL_CMDG_sendFrameHdrT;

//INTERNAL_CMDG_VIDEO_CH_DONE_E
typedef struct
{
   INTERNAL_CMDG_sendFrameHdrT hdr;
   UINT32   exposureLeft;
   UINT32   exposureRight;
   UINT32   analogGainLeft;
   UINT32   analogGainRight;
   UINT32   digitalGainLeft;
   UINT32   digitalGainRight;
} INTERNAL_CMDG_videoFrameHdrT;

//INTERNAL_CMDG_DEPTH_CH_DONE_E
typedef struct
{
   INTERNAL_CMDG_sendFrameHdrT hdr;
   UINT32   exposureLeft;
   UINT32   exposureRight;
   UINT32   analogGainLeft;
   UINT32   analogGainRight;
   UINT32   digitalGainLeft;
   UINT32   digitalGainRight;
   UINT32   minDisparity;
   UINT32   maxDisparity;
   UINT32   nonValidDepthCnt;
} INTERNAL_CMDG_depthFrameHdrT;

//INTERNAL_CMDG_WEBCAM_CH_DONE_E
typedef struct
{
   INTERNAL_CMDG_sendFrameHdrT hdr;
   UINT32   exposure;
   UINT32   analogGain;
   UINT32   digitalGain;
} INTERNAL_CMDG_webcamFrameHdrT;

//INTERNAL_CMDG_ALG_DONE_E
typedef struct
{
   INTERNAL_CMDG_sendFrameHdrT hdr;
} INTERNAL_CMDG_algFrameHdrT;

//INTERNAL_CMDG_AUDIO_DONE_E
typedef struct
{
   INTERNAL_CMDG_sendFrameHdrT hdr;
} INTERNAL_CMDG_audioFrameHdrT;

// INTERNAL_CMDG_INJECT_FRAME_HDR_E
typedef struct
{
   UINT32   frameNum;
   UINT32   frameSize;
   UINT8    calcCrc;
   UINT8    crcVal;
   UINT8    rsvd[102]; //  align to 128 bytes (together with cls+svc_com headers)
} INTERNAL_CMDG_injectFrameHdrT;

typedef struct
{
   CALIB_sensorLutModeE       calibMode;
   UINT32                     lutId;
   UINT16                     sensorInd;
   UINT16                     dblBuffId;
   UINT16                     mode;
   UINT16                     chunkId;
   UINT32                     chunkLen;
   UINT32                     numOfChunks;
   UINT32                     lutSize;
} INTERNAL_CMDG_lutChunkHdrT;

typedef struct
{
   UINT32                address;
   UINT32                bufLen;
   UINT32                numOfChunks;
   UINT32                chunkLen;
   UINT16                chunkId;
   char                  *buf;
} INTERNAL_CMDG_readWriteBufHdrT;


typedef struct
{
   UINT32                              networkId;
   UINT32                              engineType;
   UINT32                              mode;
   UINT16                              chunkId;
   UINT32                              chunkLen;
   UINT32                              numOfChunks;
   UINT32                              networkSize;
   CDNNG_inputImageRawScaleE           rawScale;
   CDNNG_channelSwapE                  channelSwap;
   CDNNG_resultsFormatE                resultsFormat;
   UINT32                              isYoloNetwork;
   CDNNG_bitAccuracyE                  bitAccuracy;
   UINT32                              pipeDepth;
   UINT32                              ProcesssingFlags;
   UINT32                              checksumall;
   UINT8                               pad[946]; //padding used for aligning the network
} INTERNAL_CMDG_CdnnLoadNetworkHdrT;

typedef struct
{
   UINT32                       networkId;
   UINT32                       networkSize;
} INTERNAL_CMDG_CdnnAllocNetworkHdrT;


typedef struct
{
   UINT32                       message;
} INTERNAL_CMDG_CdnnReleaseAllHdrT;

typedef struct
{
   UINT32                      backgroundId;
} INTERNAL_CMDG_VisionProcLoadBackgroundHdrT;

typedef struct
{
   UINT32                       backgroundId;
   UINT32                       backgroundSize;
} INTERNAL_CMDG_VisionProcAllocBackgroundHdrT;

typedef struct
{
   UINT32                       message;
} INTERNAL_CMDG_BackgroundReleaseHdrT;

typedef struct
{
   UINT32                       bufId;
   UINT32                       bufSize;
} INTERNAL_CMDG_AllocGpBufHdrT;

typedef struct
{
   UINT32                       bufId;
   UINT32                       bufDescrP;
} INTERNAL_CMDG_FreeGpBufHdrT;

typedef struct
{
   UINT32                       bufId;
   UINT32                       bufSize;
   UINT32                       bufDescrP;
} INTERNAL_CMDG_SendGpBufHdrT;

typedef struct
{
   char                         filename[MAX_FILE_NAME];
} INTERNAL_CMDG_SendGpFileHdrT;

typedef struct
{
   InuStorageNandSectionTypE    type;
   char                         filename[MAX_FILE_NAME];
   UINT32                       verMajor;
   UINT32                       verMinor;
   UINT32                       verBuild;
   UINT32                       updatedSectionsFlags;
} INTERNAL_CMDG_UpdateRawNandSecT;

typedef struct
{
   UINT16   flashAck;
   UINT16   chunkId;
   UINT32   chunkLen;
   UINT32   numOfChunks;
   UINT32   flashBufSize;
} INTERNAL_CMDG_flashChunkHdrT;

typedef struct
{
   INUG_dataTypeE       cevaId;
   UINT16               chunkId;
   UINT32               chunkLen;
   UINT32               numOfChunks;
} INTERNAL_CMDG_cevaAppChunkHdrT;

typedef struct
{
   UINT32               destAddress;
   UINT16               chunkId;
   UINT32               chunkLen;
   UINT32               numOfChunks;
} INTERNAL_CMDG_sendBufChunkHdrT;

typedef struct
{
   UINT16               chunkId;
   UINT32               chunkLen;
   UINT32               numOfChunks;
   UINT32               buffSize;
} INTERNAL_CMDG_readBufChunkHdrT;

typedef struct
{
   void     *bufDescP;
   UINT8    sensorType;
   UINT32   frameCntr;
} INTERNAL_CMDG_getPosDataT;


typedef struct
{
   UINT32 pid;
   UINT32 level;
   UINT32 msgSize;
} INTERNAL_CMDG_logDataHdrT;

typedef struct
{
   UINT64 timestamp;
} INTERNAL_CMDG_pingT;

// INUG_IOCTL_SYSTEM_SET_CONFIG
typedef struct
{
   UINT32 timeout;
} INTERNAL_CMDG_configWdT;

#ifdef __cplusplus
}
#endif

#endif //INU_INTERNAL_CMD
