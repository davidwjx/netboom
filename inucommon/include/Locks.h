
#ifndef __LOCKS_H__
#define __LOCKS_H__

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
#include <shared_mutex>
#include <mutex>

typedef std::shared_mutex BLock;
typedef std::unique_lock< BLock > WriteLock;
typedef std::shared_lock< BLock > ReadLock;

#else
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

typedef boost::shared_mutex BLock;
typedef boost::unique_lock< BLock > WriteLock;
typedef boost::shared_lock< BLock > ReadLock;
#endif

#endif
