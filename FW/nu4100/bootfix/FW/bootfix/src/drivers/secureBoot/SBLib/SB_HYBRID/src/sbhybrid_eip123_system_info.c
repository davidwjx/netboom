/* sbhybrid_eip123_system_info.c
 *
 * Reads EIP123 system info.
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

#include "implementation_defs.h"
#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_EIP123
#ifndef SBLIB_CF_REMOVE_SYSTEMINFO_READ

#include "basic_defs.h"

#include "cm_tokens_misc.h"
#include "cm_tokens_errdetails.h"
#include "cm_tokens_systeminfo.h"

#include "sb_system.h"                  // API to implement
#include "eip123.h"
#include "device_mgmt.h"

/*----------------------------------------------------------------------------
 * SBHYBRID_InitializeSystemInfo_HW
 *
 * Initialize hardware for system info read.
 */
static SB_Result_t
SBHYBRID_InitializeSystemInfo_HW(Device_Handle_t * const  Device_EIP123)
{
    int res;

    L_TRACE(LF_SBHYBRID, "Initializing hardware.");

    // initialize Driver Framework
    res = Device_Initialize(NULL);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to initialize Driver Framework. Res = %d.",
                res);

        return SB_ERROR_HARDWARE;
    }

    // EIP123 Initialisation
    L_TRACE(LF_SBHYBRID,
            "HWPAL query for Device CM with identifier %s.",
            SBLIB_CFG_CM_DEVICE);

    *Device_EIP123 = Device_Find(SBLIB_CFG_CM_DEVICE);
    if (*Device_EIP123 == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "CM Device not found with identifier %s.",
                SBLIB_CFG_CM_DEVICE);

        return SB_ERROR_HARDWARE;
    }

    // link the mailbox we want to use
    // request access to this mailbox from this host
    // note: after each reset, the Master CPU must allow this explicitly
    // see EIP123_MailboxAccessControl in eip123.h
    // see LOCKOUT register in SafeXcel-IP-123 HW Reference Manual
    res = EIP123_Link(*Device_EIP123, SBLIB_CFG_XM_MAILBOXNR);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to link mailbox %d. Res = %d.",
                (int)SBLIB_CFG_XM_MAILBOXNR,
                res);

        return SB_ERROR_HARDWARE;
    }

    return SB_SUCCESS;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_UninitializeSystemInfo_HW
 *
 * Uninitialize hardware after system info read.
 */
static void
SBHYBRID_UninitializeSystemInfo_HW(Device_Handle_t * const  Device_EIP123)
{
    int res;

    if (*Device_EIP123 == NULL)
    {
        // SBHYBRID_Initialize_HW has not been called, so there is nothing to uninit.
        return;
    }
    L_TRACE(LF_SBHYBRID, "Uninitializing hardware.");

    res = EIP123_Unlink(*Device_EIP123, SBLIB_CFG_XM_MAILBOXNR);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to unlink mailbox %d. Res = %d.",
                (int)SBLIB_CFG_XM_MAILBOXNR,
                res);
    }
}


/*----------------------------------------------------------------------------
 * SBHYBRID_SystemInfo_Read
 */
static SB_Result_t
SBHYBRID_SystemInfo_Read(const Device_Handle_t Device_EIP123,
                         uint32_t * const HWVersion,
                         uint32_t * const FWVersion,
                         uint32_t * const MemSize,
                         uint32_t * const ErrorInfo)
{
    CMTokens_Command_t t_cmd;
    CMTokens_Response_t t_res;
    int res;

    CMTokens_MakeToken_Clear(&t_cmd);
    CMTokens_MakeCommand_ReadSystemInfo(&t_cmd);

    // send message to the CM
    res = EIP123_WriteAndSubmitToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR, &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "Failed to process SystemInfo. res = %d.", res);

        return SB_ERROR_HARDWARE;
    }

    // Loop while waiting for reply.
    while (EIP123_CanReadToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR) == false)
    {
        /* Do nothing while waiting. */
    }

    // read the response token
    res = EIP123_ReadToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR, &t_res);
    if (res == 0)
    {
        if (CMTokens_ParseResponse_Generic(&t_res) == 0)
        {
            // success
            *FWVersion = t_res.W[1];
            *HWVersion = t_res.W[2];
            *MemSize   = (uint16_t)t_res.W[3];
            *ErrorInfo = t_res.W[5];

            return SB_SUCCESS;
        }
    }

    L_DEBUG(LF_SBHYBRID, "SystemInfo failed: res = %d.", res);

    return SB_ERROR_HARDWARE;
}

/*----------------------------------------------------------------------------
 * SB_SystemInfo_Read
 */
SB_Result_t
SB_SystemInfo_Read(uint32_t * const HWVersion,
                   uint32_t * const FWVersion,
                   uint32_t * const MemSize,
                   uint32_t * const ErrorInfo)
{
    Device_Handle_t Device_EIP123 = NULL;
    SB_Result_t res;

    // Setup up contexts for image processing and verification
    res = SBHYBRID_InitializeSystemInfo_HW(&Device_EIP123);

    // Actual processing, if initialization was successful
    if (res == SB_SUCCESS)
    {
        res = SBHYBRID_SystemInfo_Read(Device_EIP123,
                                       HWVersion,
                                       FWVersion,
                                       MemSize,
                                       ErrorInfo);
    }

    // Uninitialize
    SBHYBRID_UninitializeSystemInfo_HW(&Device_EIP123);

    return res;
}

#endif /* !SBLIB_CF_REMOVE_SYSTEMINFO_READ */
#endif /* SBHYBRID_WITH_EIP123 */
/* end of file sbhybrid_eip123_system_info.c */
