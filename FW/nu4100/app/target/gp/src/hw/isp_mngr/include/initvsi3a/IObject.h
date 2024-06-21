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
#ifndef _VSI_3AV2_BASE_OBJECT_H_
#define _VSI_3AV2_BASE_OBJECT_H_
#include <math.h>
#include "EventParallelBus.h"
#include <GlobalDataArea.h>
#include "json/json.h"

namespace vsi3av2 {

template<typename _Tp, typename _Td>
class IObject {
public:
    virtual ~IObject() {}
    virtual void init(const _Tp& inBus, const _Tp& outBus, _Td& dataArea) = 0;
    virtual void load(const Json::Value& node) { }
};

typedef std::shared_ptr<IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>>> (*createCallback)(void);

class ObjectRuntimeTable {
 public:
    ObjectRuntimeTable() {}
    void add(const char* key, createCallback fun) {
        _classTable[key] = fun;
    }
    std::shared_ptr<IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>>> create(const char* name);
    static ObjectRuntimeTable* inst() {
        if (!mRegistry)
            mRegistry = new ObjectRuntimeTable();
        return mRegistry;
    }
    static ObjectRuntimeTable* mRegistry;
 private:
    std::map<std::string, createCallback> _classTable;
};

class ObjectCreator {
 public:
    ObjectCreator(const char* key, createCallback fun) {
        ObjectRuntimeTable::inst()->add(key, fun);
    }
};

#define DECLARE_DYNAMIC_CLASS() public:\
    static std::shared_ptr<IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>>> create();

#define IMPLEMENT_DYNAMIC_CLASS(name) \
    extern const  ObjectCreator  registerClass##name(#name, name::create); \
    std::shared_ptr<IObject<shared_ptr<EventParallelBus>, shared_ptr<GlobalDataArea>>> name::create() { return make_shared<name>(); }
}

#endif
