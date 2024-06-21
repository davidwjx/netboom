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
#include "commitf_citf.hpp"

#include "commitf_features.hpp"
#include "commitf_pipeline.hpp"
#include "commitf_sensor.hpp"
#include "commitf_event.hpp"
#include "cam_operations.hpp"
#include "buf_io.hpp"

using namespace camdev;
#include <vector>

namespace camdev {


class CitfApi {
public:
    CitfApi(CAM_HARDWARE_PIPELINE_ID hpId, CAM_VIRTUAL_DEVICE_ID vdId, CAMDEV_WORK_MODE workMode);
    ~CitfApi();

    Citf_Handle ctifHandle;

    static RESULT CitfGetHwResources(Json::Value &jRequest, Json::Value &jResponse);
    static RESULT CitfGetHwStatus(Json::Value &jRequest, Json::Value &jResponse);
    static RESULT CitfGetIspFeature(Json::Value &jRequest, Json::Value &jResponse);
    static RESULT CitfGetFeatureVersion(Json::Value &jRequest, Json::Value &jResponse);
    static RESULT CitfGetVersions(Json::Value &jRequest, Json::Value &jResponse);

    RESULT CitfGetSensorInfo(Json::Value &jRequest, Json::Value &jResopnse);
    RESULT CitfIoctl(int32_t ctrlId, Json::Value &jRequest, Json::Value &jResponse);

    Bitf &CitfCreateBuffItf();
    RESULT CitfDelBuffItf();
    CitfEvent &CitfCreateEvent();
    RESULT CitfDelEvent();

    /*output buffer controls*/
    RESULT CitfInitBufChain(ISPCORE_BUFIO_ID chain, uint8_t skip_interval);
    RESULT CitfDeinitBufChain(ISPCORE_BUFIO_ID chain);

    RESULT CitfStartBufChain(ISPCORE_BUFIO_ID chain);
    RESULT CitfStopBufChain(ISPCORE_BUFIO_ID chain);

    RESULT CitfAttachBufChain(ISPCORE_BUFIO_ID chain);
    RESULT CitfDetachBufChain(ISPCORE_BUFIO_ID chain);

    RESULT CitfWaitForBufferEvent(ISPCORE_BUFIO_ID chain, buffCtrlEvent_t * bufCtrlEvent, int timeout);

    RESULT CitfDQBUF(ISPCORE_BUFIO_ID chain, MediaBuffer_t ** pBuf);
    RESULT CitfQBUF(ISPCORE_BUFIO_ID chain, MediaBuffer_t * pBuf);

    /*INPUT functions*/
    RESULT CitfLoadBufToChain(MediaBuffer_t * pBuf);
    RESULT CitfGetPendingInputBufCount(uint32_t * buffer_num);

    RESULT CitfInitBufferPoolCtrl(ISPCORE_BUFIO_ID chain);
    RESULT CitfDeInitBufferPoolCtrl(ISPCORE_BUFIO_ID chain);

    RESULT CitfSetBufferParameters(BUFF_MODE buff_mode);
    RESULT CitfBufferPoolAddEntity(ISPCORE_BUFIO_ID chain, BufIdentity* buf);

    RESULT CitfBufferPoolClearBufList(ISPCORE_BUFIO_ID chain);
    RESULT CitfBufferPoolSetBufToEngine(ISPCORE_BUFIO_ID chain);

    RESULT CitfBufferPoolKernelAddrMap(ISPCORE_BUFIO_ID chain, uint32_t needRemap);
    RESULT CitfBufferPoolGetBufIdList(ISPCORE_BUFIO_ID chain, std::list<BufIdentity *> * poolList);

	RESULT CitfUserWritePhyBuffer(uint32_t phy_address, uint32_t byte_size, uint8_t* pWriteBuff);
	RESULT CitfUserReadPhyBuffer(uint32_t phy_address, uint32_t byte_size, uint8_t* pReadBuff);

	RESULT CitfGetMediaBufPool(ISPCORE_BUFIO_ID chain, MediaBufPool_t **pbufpool);

public:
    std::vector<Citf *> list;
    Bitf * pBitf = nullptr;
    CitfEvent * pEvent = nullptr;

    CAM_HARDWARE_PIPELINE_ID mHpId;
    CAM_VIRTUAL_DEVICE_ID mVdId;
    CAMDEV_WORK_MODE mWorkMode;
};

} // namespace camdev
