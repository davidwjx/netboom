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

#ifndef __CAMERA_DEVICE_API_HPP__
#define __CAMERA_DEVICE_API_HPP__

#include <json/json.h>
#include "cam_device_buf_defs.h"
#include "cam_device_sensor_defs.h"
#include "cam_device_ispcore_defs.h"
#include "cam_device_module_ids.h"
#include <bufferpool/media_buffer_pool.h>
#include <ebase/offline_trace.h>
#include <list>
// Interface class definitions
#define CAM_VDEV_ID_SUPPORTED 4
#define CAM_HPPL_ID_SUPPORTED 1
#define MAX_PICTUER_PATH_LEN 256

enum CAMDEV_SnapshotType { CAMDEV_RGB=0, CAMDEV_RAW8, CAMDEV_RAW12, CAMDEV_JPEG, CAMDEV_RAW10 };

typedef enum _CAM_VIRTUAL_DEVICE_ID_ {
    CAM_VDEV_ID_0 = 0,
    CAM_VDEV_ID_1 = 1,
    CAM_VDEV_ID_2 = 2,
    CAM_VDEV_ID_3 = 3,
    CAM_VDEV_ID_MAX = 4
} CAM_VIRTUAL_DEVICE_ID;


typedef enum _CAM_HARDWARE_PIPELINE_ID_ {
    CAM_HWPPLINE_ID_0 = 0,
    CAM_HWPPLINE_ID_1 = 1,
    CAM_HWPPLINE_ID_MAX = 2
} CAM_HARDWARE_PIPELINE_ID;

//MCM configuration context
enum CAMDEV_WORK_MODE { CAMDEV_STREAM_MODE=0, CAMDEV_MCM_MODE, CAMDEV_TILE_MODE, CAMDEV_RDMA_MODE, CAMDEV_NO_CONFIG};

enum CAMDEV_INPUT_TYPE {CAMDEV_INVALID_TYPE = 0, CAMDEV_SENSOR_TYPE, CAMDEV_USER_TYPE};

enum CAM_HW_STATUS { CAM_HW_IDLE=0, CAM_HW_BUSING };

enum CAMDEV_STREAM_PRIORITY { CAMDEV_STREAM_PRIORITY_RUNETIME=0, CAMDEV_STREAM_PRIORITY_HIGH, CAMDEV_STREAM_PRIORITY_MEDIUM, CAMDEV_STREAM_PRIORITY_LOW };

typedef struct camdev_picinfo_s{
    uint32_t width;
    uint32_t height;
    uint32_t layout;
    uint32_t format;
}camdev_picinfo_t;

typedef struct camdev_work_cfg_s {
    CAMDEV_WORK_MODE work_mode;
    CAMDEV_INPUT_TYPE input_type;
    union cfg_u {                      // the type and layout dependent meta data
        struct stream_s {      // stream configure
            uint32_t         nop;
        } stream;
        struct mcm_s {      // mcm configure
            CAMDEV_STREAM_PRIORITY      priority;
        } mcm;
        struct tile_s {      // tile configure
            CAM_HARDWARE_PIPELINE_ID    tiled_hw_pipeline;
            CAMDEV_STREAM_PRIORITY      priority;
        } tile;
        struct rdma_s{       // rdma configure from Xim

            uint32_t         nop;
        }rdma;
    }config;
}camdev_work_cfg;

//suppot tile mode
//MCM schedule context
#define MCM_SCHEDULER_FIFO_DEPTH 3
typedef struct vdev_cache_s
{
    int vdid;
    CAMDEV_WORK_MODE work_mode;
}vdev_cache_t;

typedef struct camdev_schedule_status_s {
    vdev_cache_t vd_cache_q[MCM_SCHEDULER_FIFO_DEPTH];
    uint32_t wr;
    uint32_t rd;
}camdev_schedule_status;

enum VDEV_QUE_PROPERTY{
    VD_QUE_READ  = 0,
    VD_QUE_WRITE = 1,
    VD_QUE_NUM
};

typedef struct camdev_schedule_pipeline_s{
    int    stream_total;
    int    active_tag[CAM_VDEV_ID_MAX];
    int    water_mark[CAM_VDEV_ID_MAX];
    int    drop_flag[CAM_VDEV_ID_MAX];
    CAMDEV_STREAM_PRIORITY    mcm_priority[CAM_VDEV_ID_MAX];
    CAMDEV_WORK_MODE          mcm_mode[CAM_VDEV_ID_MAX];
    int    time_tag[CAM_VDEV_ID_MAX];
    CAM_HW_STATUS isp_hw_status;
    vdev_cache_t next_stream;//output decision of next slot
    int    next_stream_valid;
    camdev_schedule_status  schedule_fifo;

}camdev_schedule_pipeline_context;

typedef struct camdev_schedule_ctx_s{
    int    pipeline_total;
    int    schedule_cnt;
    CAM_HARDWARE_PIPELINE_ID trig_hpId;
    CAM_VIRTUAL_DEVICE_ID    trig_vdId;
    camdev_schedule_pipeline_context* schedule_hwctx;
}camdev_schedule_context;

typedef int (*cam_device_scheduler_callback)(camdev_schedule_context * schedule_ctx);

typedef struct camdev_schedule_config_s{
    cam_device_scheduler_callback callback;
    int mcm_raw_buf_queue_depth;
    CAMDEV_WORK_MODE schedule_mode;
}camdev_schedule_config;


// Event related operations
struct EventContext {
    int FrameID = 0;
};

enum CITF_EVENT {
    CITF_EVENT_FRAME_DONE,
};

typedef int (* CITFEventCallback)
(
    CITF_EVENT   citf_event_id,
    void         *pContext,   //!< Opaque sample data pointer that was passed in on xom control creation.
    EventContext event_ctx
);


typedef enum buffCtrl_event_e {
    BUFF_CTRL_CMD_START                  = 0,
    BUFF_CTRL_CMD_STOP                   = 1,
    // Output chain Event
    BUFF_CTRL_CMD_BUFFER_BYPASS          = 2,
    BUFF_CTRL_CMD_BUFFER_READY           = 3,
    BUFF_CTRL_CMD_TIMEOUT                = 4,
    BUFF_CTRL_CMD_MAX                    = 5,
} BuffCtrlEventID_t;

typedef struct buffCtrlEvent_s {
    BuffCtrlEventID_t eventID;   //!< The command to execute.
    unsigned int buffer_id;
    unsigned int buffer_skip_cnt;
} buffCtrlEvent_t;

struct CamStatus {
    enum State { Invalid, Init, Idle, Running };
    State state = Invalid;
};

struct MetaData3A{

  struct AeMetaData {
      float expsoureTime;//sensor exposure time
      float expsoureGain;//sensor Expsoure  total Gain
      float ispDgain;  // isp Digit Gain
      bool aeSet;
      bool metaDataEnableAE;  /*True = Disable AE, false = Enable AE*/
      bool hasmetaDataEnableAE;
  }aeMetaData;

  struct AwbMetaData {
      #define CAMDEV_3A_METADATA_AWBGAIN_NUM 4
      #define CAMDEV_3A_METADATA_CCM_NUM 9
      #define CAMDEV_3A_METADATA_OFFSET_NUM 3
      float awbGain[CAMDEV_3A_METADATA_AWBGAIN_NUM];//rGain grGain gbGain bGain
      float CCM[CAMDEV_3A_METADATA_CCM_NUM];
      float offset[CAMDEV_3A_METADATA_OFFSET_NUM];

    //awb OTP data
    bool enableAwbOTP;
    float  colorTemp;  //color tempture

      bool awbSet;
      bool metaDataEnableAWB;  /*True = Disable AWB, false = Enable AWB*/
      bool hasmetaDataEnableAWB;
  }awbMetaData;

  struct LscMetaData {
      #define CAMDEV_3A_METADATA_LSCTABLE_NUM 1156  //17*17*4
      uint16_t lscTable[CAMDEV_3A_METADATA_LSCTABLE_NUM];
      bool enableLscOTP;
      bool lscSet;
  }lscMetaData;
    int (*ae_cb)(void *) = NULL;    /*Callback when AE has been finished*/
    int (*awb_cb)(void *) = NULL;   /*Callback when AWB has been finished*/
    bool has_ae_cb = false;
    bool has_awb_cb = false;
};



class CAM_DEVICE {
public:
    static int getHwResources(Json::Value &jRequest, Json::Value &jResponse);
    static int getHwStatus(Json::Value &jRequest, Json::Value &jResponse);
    static int getIspFeature(Json::Value &jRequest, Json::Value &jResponse);
    static int getFeatureVersion(Json::Value &jRequest, Json::Value &jResponse);
    static int getVersions(Json::Value &jRequest, Json::Value &jResponse);

    static int initMcmScheduler(camdev_schedule_config* cfg);
    static int deinitMcmScheduler();

    static int registerMcmScheduleCallback();

    static int halIdMapping(CAM_HARDWARE_PIPELINE_ID hpId, CAM_VIRTUAL_DEVICE_ID vdId);
    static int getHpId(int instanceId);
    static int getVdId(int instanceId);

    /*
     * ISP control cmds operatiuon guide:
     * query getHwResources
     * check getHwStatus
     * initHardware(CAM_VIRTUAL_DEVICE_ID ..)
     * prepare buffer, bitfSetBufferPoolCtrl for path.

     * ioctl(ISPCORE_MODULE_DEVICE_CALIBRATIONLOAD..)
     * ioctl(ISPCORE_MODULE_DEVICE_INITENGINEOPERATION..)
     * ioctl(ISPCORE_MODULE_SENSOR_CAPS..)
     * ioctl(ISPCORE_MODULE_SENSOR_RESOLUTION_LIST_GET..)
     *
     * releaseHardware
     */
    CAM_DEVICE();
    ~CAM_DEVICE();
    int initHardware(CAM_HARDWARE_PIPELINE_ID hpId, CAM_VIRTUAL_DEVICE_ID vdId, void ** handle, camdev_work_cfg* mode_cfg);
    int releaseHardware();
    int getSensorInfo(Json::Value &jRequest, Json::Value &jResopnse);
    int ioctl(int32_t ctrlId, Json::Value &jRequest, Json::Value &jResponse);
    /**
     * Added for the Helsinki project due to the JSON API being CPU expensive
    */
    int ioctl(int32_t ctrlId, MetaData3A &in, MetaData3A &out);

#if 0
    template<typename T>
    int post(int32_t ctrlId, const char* section, T value) {
        Json::Value jRequest, jResponse;
        if (section != NULL) {
            jRequest[section] = value;
        }
        return ioctl(ctrlId, jRequest, jResponse);
    }

    template<typename inType, typename outType>
    int post(int32_t ctrlId, const char* section, inType value, const char* retSection, outType& retVal) {
        Json::Value jRequest, jResponse;
        if (section != NULL) {
            jRequest[section] = value;
        }
        int ret = ioctl(ctrlId, jRequest, jResponse);
        readValueFromNode<outType>(jResponse[retSection], retVal);
        return ret;
    }
#endif
    /*
     * output buffer controls operation guide:
     * initBufChain
     * startBufChain
     * attachBufChain
     * waitForBufferEvent
     * DQBUF
     * ..Processing..
     * QBUF
     * loop to waitForBufferEvent
     * detachBufChain
     * stopBufChain
     * deInitvChain
     */
    int initBufChain(ISPCORE_BUFIO_ID chain, uint8_t skip_interval);
    int deInitBufChain(ISPCORE_BUFIO_ID chain);

    int startBufChain(ISPCORE_BUFIO_ID chain);
    int stopBufChain(ISPCORE_BUFIO_ID chain);

    int attachBufChain(ISPCORE_BUFIO_ID chain);
    int detachBufChain(ISPCORE_BUFIO_ID chain);

    int waitForBufferEvent(ISPCORE_BUFIO_ID chain, buffCtrlEvent_t * bufCtrlEvent, int timeout);
    // If successful, enqueues a BUFF_CTRL_CMD_TIMEOUT event, which can unblock waitForBufferEvent().
    int sendBufferEventWakeTimeout(ISPCORE_BUFIO_ID chain);

    bool checkBufChainIdle();
    int resetBufChainReadyMask();
    int loadBufToChain(MediaBuffer_t **pbufList, uint32_t frame_cnt);
    int configMetaDataForMediaBuf(MediaBuffer_t *pMediabuf, camdev_picinfo_t pic_info);
    int DQBUF(ISPCORE_BUFIO_ID chain, MediaBuffer_t ** pBuf);
    int QBUF(ISPCORE_BUFIO_ID chain, MediaBuffer_t * pBuf);

    /*
     * Input chain specific functions
     */
    int getPendingInputBufCount(uint32_t * buffer_num);

    /*
     * Buffer Pool init control guide:
     * initBufferPoolCtrl
     * bitfSetBufferParameters
     * Loop begin
     *   buffPoolAddEntity
     * Loop end
     * bufferPoolIplMap
     * bufferPoolSetBufToEngine
     * deInitBufferPoolCtrl
    */

    int initBufferPoolCtrl(ISPCORE_BUFIO_ID chain);
    int deInitBufferPoolCtrl(ISPCORE_BUFIO_ID chain);

    int setBufferParameters(BUFF_MODE buff_mode);
    int bufferPoolAddEntity(ISPCORE_BUFIO_ID chain, BufIdentity* buf);

    int bufferPoolClearBufList(ISPCORE_BUFIO_ID chain);
    int bufferPoolSetBufToEngine(ISPCORE_BUFIO_ID chain);

    int bufferPoolIplMap(ISPCORE_BUFIO_ID chain, uint32_t needRemap);
    int bufferPoolIplUnMap(ISPCORE_BUFIO_ID chain, uint32_t mapped);

    int bufferPoolGetBufIdList(ISPCORE_BUFIO_ID chain, std::list<BufIdentity *> * poolList);

    int userWritePhyBuffer(uint32_t phy_address, uint32_t byte_size, uint8_t *pWriteBuf);
    int userReadPhyBuffer(uint32_t phy_address, uint32_t byte_size, uint8_t *pReadBuf);

    int getMediaBufPool(ISPCORE_BUFIO_ID chain, MediaBufPool_t **pbufpool);

#ifdef ISP_OFFLINE_TEST
    void setCasePrefix(char *pCasePrefix);
    const char *CasePrefix;
    uint32_t instanceId;
#endif

    void * camdev_ctx = nullptr;
    CAM_VIRTUAL_DEVICE_ID isp_vd_idx;

    CAM_HARDWARE_PIPELINE_ID isp_hp_idx;
    CAM_HARDWARE_PIPELINE_ID isp_hp_tile_friend_idx;
    CAMDEV_WORK_MODE mWorkMode;
    CAMDEV_INPUT_TYPE mInputType;
};

bool PGM_ReadRaw(std::string, PicBufMetaData_t *);

#endif  // __CAMERA_DEVICE_API_HPP__