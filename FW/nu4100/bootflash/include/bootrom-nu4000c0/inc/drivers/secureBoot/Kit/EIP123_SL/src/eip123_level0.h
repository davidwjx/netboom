/* eip123_level0.h
 *
 * This file contains all the macros and inline functions that allow
 * access to the EIP123 registers and to build the values read or
 * written to the registers as well as the tokens.
 */

/*****************************************************************************
* Copyright (c) 2007-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_EIP123_LEVEL0_H
#define INCLUDE_GUARD_EIP123_LEVEL0_H

#include "basic_defs.h"         // uint32_t, bool, inline, BIT_* etc.
#include "device_types.h"       // Device_Handle_t
#include "device_rw.h"          // Read32, Write32


/* EIP123 mailbox memory locations as offset from a base address */
/* note: IN-mailbox is write-only, OUT-mailbox is read-only */
#define EIP123_MAILBOX_IN_BASE        0
#define EIP123_MAILBOX_OUT_BASE       0
#define EIP123_MAILBOX_SPACING_BYTES  0x400

/* EIP123 mailbox register locations as offset from a base address */
#define EIP123_REGISTEROFFSET_MAILBOX_CTRL    0x3F00
#define EIP123_REGISTEROFFSET_MAILBOX_STAT    0x3F00
#define EIP123_REGISTEROFFSET_MAILBOX_LOCKOUT 0x3F10
#define EIP123_REGISTEROFFSET_EIP_OPTIONS     0x3FF8
#define EIP123_REGISTEROFFSET_EIP_VERSION     0x3FFC

static uint32_t
EIP123Lib_ReadReg_MailboxStat(
    Device_Handle_t Device)
{
    return Device_Read32(Device, EIP123_REGISTEROFFSET_MAILBOX_STAT);
}

static void
EIP123Lib_WriteReg_MailboxCtrl(
    Device_Handle_t Device,
    uint32_t Value)
{
    Device_Write32(Device, EIP123_REGISTEROFFSET_MAILBOX_CTRL, Value);
}

static uint32_t
EIP123Lib_ReadReg_Options(
    Device_Handle_t Device)
{
    return Device_Read32(Device, EIP123_REGISTEROFFSET_EIP_OPTIONS);
}


static uint32_t
EIP123Lib_ReadReg_Version(
    Device_Handle_t Device)
{
    return Device_Read32(Device, EIP123_REGISTEROFFSET_EIP_VERSION);
}

#ifndef EIP123_REMOVE_MAILBOXACCESSCONTROL
static uint32_t
EIP123Lib_ReadReg_Lockout(
    Device_Handle_t Device)
{
    return Device_Read32(Device, EIP123_REGISTEROFFSET_MAILBOX_LOCKOUT);
}
#endif

#ifndef EIP123_REMOVE_MAILBOXACCESSCONTROL
static void
EIP123Lib_WriteReg_Lockout(
    Device_Handle_t Device,
    uint32_t Value)
{
    Device_Write32(Device, EIP123_REGISTEROFFSET_MAILBOX_LOCKOUT, Value);
}
#endif

#endif /* Include Guard */

/* end of file eip123_level0.h */
