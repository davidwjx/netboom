/* hwpal_device_bare.c
 *
 * Implementation of Driver Framework v4 Device API for a Secure Boot Loader.
 *
 * This 'barebones' implementation has support for multiple devices.
 */

/*****************************************************************************
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*
* For more information or support, please go to our online support system at
* https://customersupport.insidesecure.com.
* In case you do not have an account for this system, please send an e-mail
* to ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

#include "implementation_defs.h" // implementation definitions
#include "cs_hwpal_bare.h"      // configuration

#include "basic_defs.h"         // uint32_t, NULL, inline, etc.
#include "c_lib.h"              // c_strcmp
#include "device_mgmt.h"        // the API to implement
#include "device_rw.h"

#ifdef DEVICE_SWAP
#include "device_swap.h"        // Device_SwapEndian32
#endif

#ifdef HWPAL_BARE_ENABLE_SBLPERF
#include "sblperf.h"            // SBLperf_Clear, SBLperf_Track_EIP_Write
#endif

#ifdef DEVICE_TRACE_RW
#include <stdio.h>              // printf
#endif

#ifdef HWPAL_BARE_DEBUGCHECKS
#include <stdio.h>              // printf
#include <stdlib.h>             // exit
#endif


/* The devices configured are built into a table: */
// Build table of devices (based on names and addresses provided.)
#ifdef HWPAL_BARE_EIP130_NAME
#define HWPAL_BARE_EIP130_PART \
        HWPAL_BARE_DEVICE_ADD(HWPAL_BARE_EIP130_NAME, \
                              HWPAL_BARE_EIP130_ADDRESS, \
                              HWPAL_BARE_EIP130_ADDRESS + \
                              HWPAL_BARE_EIP130_WINDOWEND)
#undef HWPAL_BARE_LIST_SEPARATOR
#define HWPAL_BARE_EIP130_LIST_SEPARATOR ,
#define HWPAL_BARE_EIP130_ENABLED 1
#else
#define HWPAL_BARE_EIP130_LIST_SEPARATOR
#define HWPAL_BARE_EIP130_PART
#define HWPAL_BARE_EIP130_ENABLED 0
#endif /* HWPAL_BARE_EIP130_NAME */

#ifdef HWPAL_BARE_EIP123_NAME
#define HWPAL_BARE_EIP123_PART \
        HWPAL_BARE_EIP130_LIST_SEPARATOR \
        HWPAL_BARE_DEVICE_ADD(HWPAL_BARE_EIP123_NAME, \
                              HWPAL_BARE_EIP123_ADDRESS, \
                              HWPAL_BARE_EIP123_ADDRESS + \
                              HWPAL_BARE_EIP123_WINDOWEND)
#undef HWPAL_BARE_LIST_SEPARATOR
#define HWPAL_BARE_EIP123_LIST_SEPARATOR ,
#define HWPAL_BARE_EIP123_ENABLED 1
#else
#define HWPAL_BARE_EIP123_LIST_SEPARATOR HWPAL_BARE_EIP130_LIST_SEPARATOR
#define HWPAL_BARE_EIP123_PART
#define HWPAL_BARE_EIP123_ENABLED 0
#endif /* HWPAL_BARE_EIP123_NAME */

#ifdef HWPAL_BARE_EIP93_NAME
#define HWPAL_BARE_EIP93_PART \
        HWPAL_BARE_EIP123_LIST_SEPARATOR \
        HWPAL_BARE_DEVICE_ADD(HWPAL_BARE_EIP93_NAME, \
                              HWPAL_BARE_EIP93_ADDRESS, \
                              HWPAL_BARE_EIP93_ADDRESS + \
                              HWPAL_BARE_EIP93_WINDOWEND)
#define HWPAL_BARE_EIP93_LIST_SEPARATOR ,
#define HWPAL_BARE_EIP93_ENABLED 1
#else
#define HWPAL_BARE_EIP93_LIST_SEPARATOR HWPAL_BARE_EIP123_LIST_SEPARATOR
#define HWPAL_BARE_EIP93_PART
#define HWPAL_BARE_EIP93_ENABLED 0
#endif /* HWPAL_BARE_EIP93_NAME */

#ifdef HWPAL_BARE_EIP28_NAME
#define HWPAL_BARE_EIP28_PART \
        HWPAL_BARE_EIP93_LIST_SEPARATOR \
        HWPAL_BARE_DEVICE_ADD(HWPAL_BARE_EIP28_NAME, \
                              HWPAL_BARE_EIP28_ADDRESS, \
                              HWPAL_BARE_EIP28_ADDRESS + \
                              HWPAL_BARE_EIP28_WINDOWEND)
#define HWPAL_BARE_EIP28_ENABLED 1
#else
#define HWPAL_BARE_EIP28_PART
#define HWPAL_BARE_EIP28_ENABLED 0
#endif /* HWPAL_BARE_EIP28_NAME */

#define HWPAL_BARE_DEVICES \
        HWPAL_BARE_EIP130_PART \
        HWPAL_BARE_EIP123_PART \
        HWPAL_BARE_EIP93_PART \
        HWPAL_BARE_EIP28_PART

/* Number of bare devices enabled.
   This value can be used by implementation of hwpal_device_bare.c to
   provide optimal code. */
#define HWPAL_BARE_NUM_DEVICES ( \
        HWPAL_BARE_EIP130_ENABLED + \
        HWPAL_BARE_EIP123_ENABLED + \
        HWPAL_BARE_EIP93_ENABLED + \
        HWPAL_BARE_EIP28_ENABLED )

/*------------------------------------------------------------------------------
 * Device_Initialize
 */
int
Device_Initialize(
    void * CustomInitData_p)
{
    IDENTIFIER_NOT_USED(CustomInitData_p);

    return 0;
}

/* Helper function to match device name. */
#if HWPAL_BARE_NUM_DEVICES > 1
static inline void name_match(
    const char *name,
    const char *s,
    unsigned long a,
    unsigned long *lp)
{
    if (c_strcmp(name, s) == 0)
    {
        *lp = a;
    }
}
#endif

#if defined(HWPAL_BARE_DEBUGCHECKS) | defined(DEVICE_TRACE_RW)
/* Helper function to match device name. */
static inline void handle_match(
    Device_Handle_t handle,
    const char *s,
    unsigned long a,
    const char **lp)
{
    if (handle == (Device_Handle_t) a)
    {
        *lp = s;
    }
}

static inline const char *
Device_GetName(Device_Handle_t Handle)
{
    const char *Name_p = "[unknown device]";

    /* Redefine HWPAL_BARE_DEVICE_ADD macro to do address matching. */
#undef HWPAL_BARE_DEVICE_ADD
#define HWPAL_BARE_DEVICE_ADD(name, address, end) \
    handle_match(Handle, name, address, &Name_p)
    HWPAL_BARE_DEVICES;
#undef HWPAL_BARE_DEVICE_ADD

    return Name_p;
}
#endif

#ifdef HWPAL_BARE_DEBUGCHECKS
/* Helper function to match device name. */
static inline void range_match(
    Device_Handle_t handle,
    unsigned long offset,
    unsigned long a,
    unsigned long range,
    bool *lp)
{
    if (handle == (Device_Handle_t) a && offset < range)
    {
        *lp = true;
    }
}

static inline bool
Device_InRange(Device_Handle_t Handle, const unsigned int ByteOffset)
{
    bool inRange = false;

    /* Redefine HWPAL_BARE_DEVICE_ADD macro to do address matching. */
#undef HWPAL_BARE_DEVICE_ADD
#define HWPAL_BARE_DEVICE_ADD(name, address, end) \
    range_match(Handle, (unsigned long) ByteOffset, address, \
                ((end) - (address)), &inRange)
    HWPAL_BARE_DEVICES;
#undef HWPAL_BARE_DEVICE_ADD

    return inRange;
}
#endif

/*------------------------------------------------------------------------------
 * Device_Find
 *
 * This simple implementation supports single-character names only.
 */
Device_Handle_t
Device_Find(
    const char * DeviceName_p)
{
    unsigned long Address = 0;
#ifdef HWPAL_BARE_DEBUGCHECKS
    if (DeviceName_p == NULL)
    {
        // not supported, thus not found
        return NULL;
    }

    /* Redefine HWPAL_BARE_DEVICE_ADD macro to do name matching. */
#undef HWPAL_BARE_DEVICE_ADD
#define HWPAL_BARE_DEVICE_ADD(name, address, end) \
    name_match(DeviceName_p, name, address, &Address)
    HWPAL_BARE_DEVICES;
#undef HWPAL_BARE_DEVICE_ADD
#elif HWPAL_BARE_NUM_DEVICES == 0
    // Address remains as 0.
    IDENTIFIER_NOT_USED(DeviceName_p);
#elif HWPAL_BARE_NUM_DEVICES == 1
    // Extract the address of the first device.
    IDENTIFIER_NOT_USED(DeviceName_p);
#undef HWPAL_BARE_DEVICE_ADD
#define HWPAL_BARE_DEVICE_ADD(name, address, end) \
    Address = (unsigned long) address
    HWPAL_BARE_DEVICES;
#undef HWPAL_BARE_DEVICE_ADD
#else /* HWPAL_BARE_NUM_DEVICES > 1 */
#undef HWPAL_BARE_DEVICE_ADD
#define HWPAL_BARE_DEVICE_ADD(name, address, end) \
    name_match(DeviceName_p, name, address, &Address)
    HWPAL_BARE_DEVICES;
#undef HWPAL_BARE_DEVICE_ADD
#endif

#ifdef DEVICE_TRACE_FIND
    if (Address == 0UL)
    {
        printf("The device \"%s\" not detected.", DeviceName_p);
        return (Device_Handle_t) Address;
    }
#endif /* DEVICE_TRACE_FIND */

#ifdef DEVICE_TRACE_FIND
    printf("Detected the device \"%s\" at address: %p\n",
           DeviceName_p, (void*)Address);
#endif /* DEVICE_TRACE_FIND */

    return (Device_Handle_t) Address;
}


/*------------------------------------------------------------------------------
 * Device_Read32
 */
uint32_t
Device_Read32(
    Device_Handle_t Device,
    const unsigned int ByteOffset)
{
    const unsigned int Idx = ByteOffset >> 2;
    uint32_t WordRead;
    volatile unsigned int *Device_EIP_Mem32_p = Device;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (!Device_InRange(Device, ByteOffset))
    {
        PANIC("READ OUT OF RANGE: %s: 0x%08x",
              Device_GetName(Device), ByteOffset);
    }
#endif

#ifdef DEVICE_TRACE_RW
    printf("r %s 0x%08x=", Device_GetName(Device), ByteOffset);
#endif

    WordRead = Device_EIP_Mem32_p[Idx];

#ifdef DEVICE_SWAP
    WordRead = Device_SwapEndian32(WordRead);
#endif

#ifdef DEVICE_TRACE_RW
    printf("0x%08x\n", (unsigned int)WordRead);
#endif

    return WordRead;
}


/*------------------------------------------------------------------------------
 * Device_Write32
 */
void
Device_Write32(
    Device_Handle_t Device,
    const unsigned int ByteOffset,
    const uint32_t Value)
{
    const unsigned int Idx = ByteOffset >> 2;
    uint32_t WordWrite = Value;
    volatile unsigned int *Device_EIP_Mem32_p = Device;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (!Device_InRange(Device, ByteOffset))
    {
        PANIC("WRITE OUT OF RANGE: %s: 0x%08x",
              Device_GetName(Device), ByteOffset);
    }
#endif

#ifdef HWPAL_BARE_ENABLE_SBLPERF
    SBLperf_Track_EIP_Write(Idx, Value);
#endif

#ifdef DEVICE_SWAP
    WordWrite = Device_SwapEndian32(WordWrite);
#endif

#ifdef DEVICE_TRACE_RW
    printf("w %s 0x%08x=0x%08x\n",
           Device_GetName(Device),
           ByteOffset,
           (unsigned int)WordWrite);
#endif

    Device_EIP_Mem32_p[Idx] = WordWrite;
}


/*------------------------------------------------------------------------------
 * Device_Read32Array
 */
void
Device_Read32Array(
    const Device_Handle_t Device,
    const unsigned int StartByteOffset,
    uint32_t * MemoryDst_p,
    const int Count)
{
    unsigned int Idx = StartByteOffset >> 2;
    uint32_t WordRead;
    int Nwords;
    volatile unsigned int *Device_EIP_Mem32_p = Device;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (!Device_InRange(Device, StartByteOffset + Count * 4 - 1))
    {
        PANIC("READ OUT OF RANGE: %s: 0x%08x[len=0x%x]",
              Device_GetName(Device), StartByteOffset, Count * 4);
    }

    if (Count <= 0)
    {
        return;
    }
#endif

    for (Nwords = 0; Nwords < Count; ++Nwords, ++Idx)
    {
#ifdef DEVICE_TRACE_RW
        printf("ra %s 0x%08x=", Device_GetName(Device), Idx);
#endif

        WordRead = Device_EIP_Mem32_p[Idx];

#ifdef DEVICE_SWAP
        WordRead = Device_SwapEndian32(WordRead);
#endif

        MemoryDst_p[Nwords] = WordRead;

#ifdef DEVICE_TRACE_RW
        printf("0x%08x\n", (unsigned int)WordRead);
#endif
    }
}


/*------------------------------------------------------------------------------
 * Device_Write32Array
 */
void
Device_Write32Array(
    const Device_Handle_t Device,
    const unsigned int StartByteOffset,
    const uint32_t * MemorySrc_p,
    const int Count)
{
    unsigned int Idx = StartByteOffset >> 2;
    uint32_t WordWrite;
    int Nwords;
    volatile unsigned int *Device_EIP_Mem32_p = Device;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (!Device_InRange(Device, StartByteOffset + Count * 4 - 1))
    {
        PANIC("WRITE OUT OF RANGE: %s: 0x%08x[len=0x%x]",
              Device_GetName(Device), StartByteOffset, Count * 4 - 1);
    }

    if (Count <= 0)
    {
        return;
    }
#endif

    for (Nwords = 0; Nwords < Count; ++Nwords, ++Idx)
    {
        WordWrite = MemorySrc_p[Nwords];

#ifdef HWPAL_BARE_ENABLE_SBLPERF
        SBLperf_Track_EIP_Write(Idx, WordWrite);
#endif

#ifdef DEVICE_SWAP
        WordWrite = Device_SwapEndian32(WordWrite);
#endif

#ifdef DEVICE_TRACE_RW
        printf("wa %s 0x%08x=0x%08x\n",
               Device_GetName(Device),
               Idx,
               (unsigned int) WordWrite);
#endif

        Device_EIP_Mem32_p[Idx] = WordWrite;
    }
}


/* end of file hwpal_device_bare.c */
