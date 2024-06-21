/*
 * Copyright 2022 IMDT
 * All rights reserved.
 *
 */
#pragma once
#include "inu_device.h"
#include "inu_device_api.h"
#include <stdint.h>
#include "os_lyr.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum
{
    RTC_DRVG_TrackingMode_ImmediateMode = 0,
    RTC_DRVG_TrackingMode_PendingMode = 1,
} RTC_DRVG_TrackingModeE;
typedef struct 
{   
   UINT32 seconds;             /*Seconds since epoch (coarse time)*/
   UINT32 fracSeconds;         /*Fractional part of a second (least significant bit = 1/(1<<32)- 1) seconds*/
} RTC_DRVG_RtcTimeT;      /*Time which comes from the RTC register*/

/*RTC Time, top 32 bits are the seconds part and the bottom 32 bits are the fractional part*/

// RTC Driver requires the RTC_DRVG_init() function to be invoked before using the
// RTC interface.
// Since the RTC get and set functions perform simple PSS register writes and reads, 
// the function will hardly fail. The only failure scenario is that if RTC
// Tracking has been enabled and RTC fails to acquire lock in certain time interval.
// the Time interval for acquiring lock has to be inferred from testing trials.
// RTC Driver Init, sets the IO registers virtual base address
ERRG_codeE RTC_DRVG_init(UINT32 memVirtAddr);

// Read the RTC coarse time
// Return 32 bit coarse time value. This is in seconds without fractional part.
UINT32 RTC_DRVG_coarseTimeGet(void);

// Set RTC coarse time and start RTC Tracking if TC tracking feature is configured
ERRG_codeE RTC_DRVG_setTime(UINT32 seconds, UINT32 microseconds,const RTC_DRVG_TrackingModeE trackingMode );

/**
 * @brief Returns the RTC time 
 * 
 * @param timeParam RTC Time pointer
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE RTC_DRVG_getTime(RTC_DRVG_RtcTimeT *timeParam);

// Return the state of the RTC lock: 
// RTC_INACTIVE: RTC module not enabled.
// RTC_LOCK_IN_PROGRESS: RTc tracking is in progress, lock not yet acquired
// RTC_LOCK_DONE: RTC tracking has acquired alock with Remote RTC (host RTC)
void RTC_DRVG_status(inu_device__get_rtc_lock_t *rtcLock);

// Function to dump RTC register values, helper function for debugging
ERRG_codeE RTC_DRVG_dumpRegs(void);
/**
 * @brief Gets the RTC Time in terms of nanoseconds since epoch
 * 
 *
 * @param result Pointer to store result in
 * @return Returns an error code
 */
ERRG_codeE RTC_DRVG_getTimeNs(UINT64 *result);
/**
 * @brief Converts nanoseconds into an RTC time
 * 
 */
ERRG_codeE RTC_DRVG_convertNsTimeToRTCTime(UINT64 nsTime, RTC_DRVG_RtcTimeT *rtcTime);
/**
 * @brief Converts RTC time to nanoseconds
 * 
 *
 * @param result Pointer to store result in
 * @param time RTC Time pointer
 * @return Returns an error code
 */
ERRG_codeE RTC_DRVG_convertRTCTimeToNs(UINT64 *result, RTC_DRVG_RtcTimeT * time);
void  RTC_REG_dump();

UINT64 RTC_convertRTCTimeToNS(UINT64 rtcTime);
ERRG_codeE RTC_DRVG_getTimeRaw(UINT64 *result);
#ifdef __cplusplus
}
#endif