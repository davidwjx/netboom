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
#ifndef _VSI_3AV2_EVENT_PARALLEL_BUS_H_
#define _VSI_3AV2_EVENT_PARALLEL_BUS_H_
#include <string>
#include <functional>
#include <map>
#include <memory>
#include <typeindex>
#include <type_traits>
#include <cassert>

using namespace std;

namespace vsi3av2 {
template <typename T> struct anotify;
template<typename R, typename...A> struct anotify<R(A...)> { using snt = function<R(A...)>; typedef R(*pointer)(A...); };
template<typename R, typename...A> struct anotify<R(*)(A...)> : anotify<R(A...)>{};
template<typename R,typename... A> struct anotify<function<R(A...)>> : anotify<R(A...)>{};
#define DEFNOTIFY(...) template<typename R, typename T, typename...A> struct anotify<R(T::*)(A...) __VA_ARGS__> : anotify<R(A...)>{};
DEFNOTIFY()
DEFNOTIFY(const)
template<typename v> struct anotify : anotify<decltype(&v::operator())>{};
template<typename v> typename anotify<v>::snt f2n(const v& f) { return  static_cast<typename anotify<v>::snt>(f); }
template<typename v> typename anotify<v>::snt f2n(v&& f) { return static_cast<typename anotify<v>::snt>(forward<v>(f)); }

class IBase
{
public:
    IBase(void) : m_type(type_index(typeid(void))) {}
    IBase(const IBase& r) : bp(r.clone()), m_type(r.m_type) {}
    IBase(IBase&& r) : bp(move(r.bp)), m_type(r.m_type) {}
    template<typename _Tp, typename = typename enable_if<!is_same<typename decay<_Tp>::type, IBase>::value, _Tp>::type>
    IBase(_Tp &&value) : bp(new BBase<typename decay<_Tp>::type>(forward<_Tp>(value))), m_type(type_index(typeid(typename decay<_Tp>::type))) {}
    template<typename _Tp> bool type_match() const { return m_type == type_index(typeid(_Tp)); }
    template<typename _Tp> _Tp& cast() { assert(type_match<_Tp>()); return dynamic_cast<BBase<_Tp>*> (bp.get())->v; }
    IBase operator = (const IBase& a) {
        if (bp == a.bp) return *this;
        bp = a.clone();
        m_type = a.m_type;
        return *this;
    }

private:
    struct Base;
    typedef unique_ptr<Base> BasePtr;
    struct Base {
        virtual ~Base() {}
        virtual BasePtr clone() const = 0;
    };

    template<typename _T> struct BBase : Base {
        template<typename _Tp>
        BBase(_Tp&& value) : v(forward<_Tp>(value)) {}
        BasePtr clone() const { return BasePtr(new BBase<_T>(v)); }
        _T v;
    };

    BasePtr clone() const {
        if (bp != nullptr)
            return bp->clone();
        return nullptr;
    }

    BasePtr bp;
    type_index m_type;
};

class EventParallelBus
{
public:
    EventParallelBus() = default;
    ~EventParallelBus() = default;
    EventParallelBus(const EventParallelBus&) = delete;
    EventParallelBus& operator = (const EventParallelBus&) = delete;
    template<typename _Tp> void add(_Tp&& f, const string& key = "") { insert(move(f2n(forward<_Tp>(f))), key); }
    template<typename _Tp, typename...Args> void remove(const string& key) {
        auto r = flist.equal_range(key+typeid(function<_Tp(Args...)>).name());
        flist.erase(r.first, r.second);
    }
    template<typename _Tp> void send(const string& key="") {
        using ft = function<_Tp()>;
        auto r = flist.equal_range(key+typeid(ft).name());
        for (auto iter = r.first; iter != r.second; iter++) {
            iter->second.cast<ft>()();
        }
    }
    template<typename _Tp, typename... Args> void send(Args&&...args, const string& key="") {
        using ft = function<_Tp(Args...)>;
        auto r = flist.equal_range(key+typeid(ft).name());
        for (auto iter = r.first; iter != r.second; iter++) {
            iter->second.cast<ft>()(forward<Args>(args)...);
        }
    }

private:
    template<typename _Tp> void insert(_Tp&& f, const string& key) {
        flist.emplace(move(key + typeid(_Tp).name()), forward<_Tp>(f));
    }
    multimap<string, IBase> flist;
};

typedef std::shared_ptr<EventParallelBus> PBUS;

#define ON_MESSAGE_VOID_1(A, T, F) A->add(function<void(T&)>(bind(F, this, placeholders::_1)));
#define ON_MESSAGE_VOID_1X(A, T, C, F) A->add(function<void(T&)>(bind(&C::F, this, placeholders::_1)), #F);
#define ON_MESSAGE_VOID_2(A, T1, T2, F) A->add(function<void(T1&, T2&)>(bind(F, this, placeholders::_1, placeholders::_2)));

}

#endif
