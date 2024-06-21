/******************************************************************************\
|* Copyright (c) 2020 by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")       *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
\******************************************************************************/
#ifndef _VSI_3AV2_H_
#define _VSI_3AV2_H_

#include <stdint.h>
#include <stdlib.h>
#include <type_traits>
#include <map>
#include <vector>
#include <iostream>
#include <deque>
#include <thread>

#include "EventParallelBus.h"
#include "Base3A.h"
#include "GlobalDataArea.h"
#include "EAutoLock.h"

namespace vsi3av2 {

typedef struct Variant {
    uchar* data = nullptr;
    size_t size = 0;
    Variant(void *param, size_t _size) {
        size = _size;
        data = new uchar[size];
        memcpy(data, param, size);
    }
    ~Variant() {
        if (data) {
            delete []data;
            data = nullptr;
        }
    }
} Variant;

class Vsi3AInterface {
public:
    Vsi3AInterface(int id);
    Vsi3AInterface(int id, const char* filename);
    virtual ~Vsi3AInterface();
    bool stop();
    int postEvent(int type, void *params);
    int config(int type, void *params);
    int getConfig(int type, void *params);
    int setIspHandle(void *ispHandle);
    int setSensorHandle(void *sensorHandle, void *calibHandle);
    int setMode(int type, int mode);
    int getMode(int type, int *mode);
    int getStatus(int type, int *status);
    int getData(int type, void *data);
    void* getGlobalData() { return (void*)mDataArea.get(); }
	int setROI(enum e3aMode_t type, int num, struct Vsi3AROI *roi);
    int getROI(enum e3aMode_t type, int* num, struct Vsi3AROI *roi);
private:
    int m_id = 0;
    shared_ptr<EventParallelBus> inBus, outBus;
    shared_ptr<GlobalDataArea> mDataArea;
    vector<shared_ptr<IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>>>> mlist;

    template<typename T>
    int sendInputBus(void* params) {
        shared_ptr<T> data = make_shared<T>(*((T*)params));
        inBus->send<void, shared_ptr<T>&>(data);
        data = nullptr;
        return 0;
    }
    template<typename T>
    int sendOutputBus(void* params) {
        shared_ptr<T> data = make_shared<T>(*((T*)params));
        outBus->send<void, shared_ptr<T>&>(data);
        data = nullptr;
        return 0;
    }

    map<int, Variant*> mDataQueue;
    map<int, EMutex> mLockList;
    condition_variable cv;
    mutex mtx; // mutex for the Condition Variable
    std::thread mThread;
    std::thread mReloadThread;
    bool bRunning = false;
    char* FileName = NULL;
    void loop();
    void loopReload();
    void loopReload(char* filename);
};

}

#endif
