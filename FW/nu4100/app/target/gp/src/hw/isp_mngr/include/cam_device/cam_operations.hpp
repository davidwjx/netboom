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

#include "cam_common.hpp"
#include <vector>
#include "buf_io.hpp"
#include <string>
#include <deque>
#include <mutex>
#include "calibration/calibration.hpp"
#include "camera/sensor.hpp"
#include "camera/image.hpp"
#include "camera/engine.hpp"
#include "utility/mapcaps.hpp"

namespace camdev {

typedef void(AfpsResChangeCb_t)(const void *);

struct CAM_CFG_SET{
    HalHolder *pHalHolder = nullptr;
    clb::Calibration *pCalibration = nullptr;
};

struct Operation : CamStatus {
    Operation(CAM_CFG_SET * cam_cfg);
    ~Operation();

    static int32_t afpsResChangeRequestCb(uint32_t, uint32_t, const void *);

    uint bitstreamId() const;

    uint camerIcId() const;

    int32_t dehazeEnableGet(bool &);
    int32_t dehazeEnableSet(bool);

    int32_t ecmSet(bool = false);

    t_camera::Image &image() {
        return *images[pOperationHandle->pCalibration->module<clb::Inputs>().config.index];
    }

    int32_t inputConnect();
    int32_t inputDisconnect();
    int32_t inputSwitch(int32_t);

    int32_t pipelineEnableSet(bool);

    int32_t reset();

    int32_t resolutionGet(uint16_t &, uint16_t &);
    int32_t resolutionSet(uint16_t , uint16_t );

    t_camera::Sensor &sensor() {
        pOperationHandle->pCalibration->module<clb::Inputs>().config.index = 0;//To be align
        return *sensors[pOperationHandle->pCalibration->module<clb::Inputs>().config.index];
    };

    std::string softwareVersion() const;

    int32_t streamingStart(uint frames = 0);
    int32_t streamingStop();

    std::vector<t_camera::Image *> images;
    std::vector<t_camera::Sensor *> sensors;

    std::function<int32_t()> fEventSensorDriverChanged;
    std::function<int32_t()> fEventSensorDriverPrechange;

    AfpsResChangeCb_t *pAfpsResChangeCb = nullptr;

    t_camera::Engine *pEngine = nullptr;

    const void *pUserCbCtx = nullptr;

    // device operations
    static void bufferCb(CamEnginePathType_t path, MediaBuffer_t *pMediaBuffer, void *pCtx);

    typedef struct ConnectInfo {
        int width, height, format, fps, hdr;
        ConnectInfo& operator=(const ConnectInfo& ri) {
            width = ri.width;
            height = ri.height;
            format = ri.format;
            hdr = ri.hdr;
			fps    = ri.fps;
            return *this;
        }
    } ConnectInfo;

    int32_t cameraConnect(bool preview, ItfBufferCb * bufferCb, const ConnectInfo& connectInfo);
    int32_t cameraDisconnect();
    int32_t cameraReset();

    int32_t captureDma(std::string, CAMDEV_SnapshotType);
    int32_t captureSensor(std::string, CAMDEV_SnapshotType, uint32_t, CamEngineLockType_t = CAM_ENGINE_LOCK_ALL);

    int32_t previewStart();
    int32_t previewStop();

    int32_t streamingStandby(bool enable);

    // friend class PfidItf;

    struct BufferCbContext {
        std::list<ItfBufferCb *> mainPath;
        std::list<ItfBufferCb *> selfPath1;
        std::list<ItfBufferCb *> selfPath2;
        std::list<ItfBufferCb *> mcmWr;
    };

	void (*ximOpNoticeFunc)(CamEngineCmdId_t , int32_t, void const *);
	void const *pximCtx;

    BufferCbContext *pBufferCbContext = nullptr;
    CAM_CFG_SET * pOperationHandle = nullptr;

    void *pSampleCbContext = nullptr;
    uint8_t sampleSkip = 0;

    int miMode, miLayout, miAlign;
    ConnectInfo mConnectInfo;

    bool bStandby = false;

    CAM_HARDWARE_PIPELINE_ID mHpId;
    CAM_VIRTUAL_DEVICE_ID mVdId;
    CAMDEV_WORK_MODE mWorkMode;
};

}
