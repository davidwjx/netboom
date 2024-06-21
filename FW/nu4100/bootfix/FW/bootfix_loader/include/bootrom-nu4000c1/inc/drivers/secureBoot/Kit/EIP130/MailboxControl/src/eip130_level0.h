/* eip130_level0.h
 *
 * This file contains all the macros and inline functions that allow
 * access to the EIP-13x Root of Trust / Security Module registers
 * and to build the values read or written to the registers as well
 * as the tokens.
 */

/*****************************************************************************
* Copyright (c) 2014-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_EIP130_LEVEL0_H
#define INCLUDE_GUARD_EIP130_LEVEL0_H

#include "basic_defs.h"             // uint32_t, bool, inline, BIT_* etc.
#include "device_types.h"           // Device_Handle_t
#include "device_rw.h"              // Read32, Write32


/* EIP130 mailbox memory locations as offset from a base address */
/* Note: IN-mailbox is write-only, OUT-mailbox is read-only */
#define EIP130_MAILBOX_IN_BASE                  0x0000
#define EIP130_MAILBOX_OUT_BASE                 0x0000
#define EIP130_MAILBOX_SPACING_BYTES            0x0400 // Actual mailbox size independent

/* EIP130 mailbox register locations as offset from a base address */
#define EIP130_REGISTEROFFSET_MAILBOX_STAT      0x3F00
#define EIP130_REGISTEROFFSET_MAILBOX_CTRL      0x3F00
#define EIP130_REGISTEROFFSET_MAILBOX_RAWSTAT   0x3F04
#define EIP130_REGISTEROFFSET_MAILBOX_RESET     0x3F04
#define EIP130_REGISTEROFFSET_MAILBOX_LINKID    0x3F08
#define EIP130_REGISTEROFFSET_MAILBOX_OUTID     0x3F0C
#define EIP130_REGISTEROFFSET_MAILBOX_LOCKOUT   0x3F10

/* EIP130 module register locations as offset from a base address */
#define EIP130_REGISTEROFFSET_MODULE_STATUS     0x3FE0
#define EIP130_REGISTEROFFSET_EIP_OPTIONS2      0x3FF4
#define EIP130_REGISTEROFFSET_EIP_OPTIONS       0x3FF8
#define EIP130_REGISTEROFFSET_EIP_VERSION       0x3FFC

/* EIP130 Firmware RAM location as offset from a base address */
#define EIP130_FIRMWARE_RAM_BASE                0x4000

/* EIP130 Firmware download related synchronisation defines */
#define EIP130_FIRMWARE_WRITTEN                 BIT_20
#define EIP130_FIRMWARE_CHECKS_DONE             BIT_22
#define EIP130_FIRMWARE_ACCEPTED                BIT_23

/* EIP130 Hardware error indications */
#define EIP130_CRC24_OK                         BIT_9
#define EIP130_FATAL_ERROR                      BIT_31


#ifndef EIP130_REMOVE_READMAILBOXSTATUS
static uint32_t
EIP130_RegisterReadMailboxStatus(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_STAT);
}
#endif

#ifndef EIP130_REMOVE_WRITEMAILBOXCONTROL
static void
EIP130_RegisterWriteMailboxControl(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MAILBOX_CTRL, Value);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXRAWSTATUS
static uint32_t
EIP130_RegisterReadMailboxRawStatus(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_RAWSTAT);
}
#endif

#if !defined(EIP130_REMOVE_MAILBOXRESET) || \
    !defined(EIP130_REMOVE_MAILBOXLINKRESET) || \
    !defined(EIP130_REMOVE_FIRMWAREDOWNLOAD)
static void
EIP130_RegisterWriteMailboxReset(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MAILBOX_RESET, Value);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXLINKID
static uint32_t
EIP130_RegisterReadMailboxLinkId(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_LINKID);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXOUTID
static uint32_t
EIP130_RegisterReadMailboxOutId(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_OUTID);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXACCESSCONTROL
static uint32_t
EIP130_RegisterReadMailboxLockout(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MAILBOX_LOCKOUT);
}
#endif

#ifndef EIP130_REMOVE_MAILBOXACCESSCONTROL
static void
EIP130_RegisterWriteMailboxLockout(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MAILBOX_LOCKOUT, Value);
}
#endif

static uint32_t
EIP130_RegisterReadVersion(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_EIP_VERSION);
}

#if !defined(EIP130_REMOVE_MAILBOXGETOPTIONS) || \
    !defined(EIP130_REMOVE_MAILBOXACCESSVERIFY) || \
    !defined(EIP130_REMOVE_FIRMWAREDOWNLOAD)
static uint32_t
EIP130_RegisterReadOptions(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_EIP_OPTIONS);
}
#endif

static uint32_t
EIP130_RegisterReadOptions2(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_EIP_OPTIONS2);
}

static uint32_t
EIP130_RegisterReadModuleStatus(
        Device_Handle_t Device)
{
    return Device_Read32(Device, EIP130_REGISTEROFFSET_MODULE_STATUS);
}

#if !defined(EIP130_REMOVE_MODULEFIRMWAREWRITTEN) || \
    !defined(EIP130_REMOVE_FIRMWAREDOWNLOAD)
static void
EIP130_RegisterWriteModuleStatus(
        Device_Handle_t Device,
        uint32_t Value)
{
    Device_Write32(Device, EIP130_REGISTEROFFSET_MODULE_STATUS, Value);
}
#endif

#endif

/* end of file eip130_level0.h */
