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

#pragma once

#include "cam_device_api.hpp"
#include <oslayer/oslayer.h>
#include <string>

#include <sys/stat.h>
#include <ebase/trace.h>
#include "common/tracer.hpp"
#include <cameric_drv/cameric_drv_api.h>
namespace fs {

inline bool isExists(const std::string &name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

}

#define ISP_MAX 2
//#define ISP_INPUT_MAX 4


/* BufferCallback functions */
struct ItfBufferCb {
    virtual void bufferCb(ISPCORE_BUFIO_ID , MediaBuffer_t *) = 0;
};


struct IOCtrl : CamStatus {
    IOCtrl();
    virtual ~IOCtrl();

    virtual int32_t start(void *) = 0;
    virtual int32_t stop() = 0;

    osEvent eventStarted;
    osEvent eventStopped;
};

class HalHolder {
public:
    HalHolder(CAM_HARDWARE_PIPELINE_ID hpId, CAM_VIRTUAL_DEVICE_ID vdId, CAMDEV_WORK_MODE workMode);
    ~HalHolder();

public:

    HalHandle_t hHal = NULL;
    BinderHandle_t hBinder=NULL;
    CAM_HARDWARE_PIPELINE_ID mHpId;
    CAM_VIRTUAL_DEVICE_ID mVdId;
    CAMDEV_WORK_MODE mWorkMode;

#ifdef USE_3AV2
    int index;
#endif
};


class FEHolder {
public:
    FEHolder(CAM_HARDWARE_PIPELINE_ID hpId);
    ~FEHolder();

	int32_t Hal_FEInit(HalFEHandle_t feHandle, int vdid_max);
	int32_t Hal_FEDeInit(HalFEHandle_t feHandle);
	int32_t Hal_FESwitch(HalFEHandle_t feHandle, vdev_cache_t *vd_info);

public:
	CAM_HARDWARE_PIPELINE_ID isp_hwid;
    HalFEHandle_t hFE = NULL;
};
