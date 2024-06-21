/* sbhybrid_eip130_system_info.c
 *
 * Reads EIP130 system info.
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

#include "implementation_defs.h"
#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_EIP130
#ifndef SBLIB_CF_REMOVE_SYSTEMINFO_READ

#include "basic_defs.h"
#include "sb_system.h"                  // API to implement

#include "eip130.h"
#include "device_mgmt.h"
#include "eip130_token_common.h"
#include "eip130_token_system.h"

// For tracing/debugging purpose, dump the command and result tokens
//#define TOKENS_VERBOSE


/*----------------------------------------------------------------------------
 * SBHYBRID_InitializeSystemInfo_HW
 *
 * Initialize hardware for system info read.
 */
static SB_Result_t
SBHYBRID_InitializeSystemInfo_HW(Device_Handle_t * const  Device_EIP130_p)
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

    // EIP130 Initialisation
    L_TRACE(LF_SBHYBRID,
            "HWPAL query for Device SM with identifier %s.",
            SBLIB_CFG_SM_DEVICE);

    *Device_EIP130_p = Device_Find(SBLIB_CFG_SM_DEVICE);
    if (*Device_EIP130_p == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "SM Device not found with identifier %s.",
                SBLIB_CFG_SM_DEVICE);
        return SB_ERROR_HARDWARE;
    }

    // link the mailbox we want to use for requests from this host
    // Note: After each reset, the Master CPU must allow this explicitly.
    //       Please refer to EIP130_MailboxAccessControl in eip130.h
    //       and LOCKOUT register in SafeXcel-IP-130 HW Reference Manual
    res = EIP130_MailboxLink(*Device_EIP130_p, SBLIB_CFG_XM_MAILBOXNR);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to link mailbox %d. Res = %d.",
                (int)SBLIB_CFG_XM_MAILBOXNR, res);
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
SBHYBRID_UninitializeSystemInfo_HW(Device_Handle_t * const  Device_EIP130)
{
    int res;

    if (*Device_EIP130 == NULL)
    {
        // SBHYBRID_Initialize_HW has not been called, so there is nothing to uninit.
        return;
    }

    L_TRACE(LF_SBHYBRID, "Uninitializing hardware.");

    res = EIP130_MailboxUnlink(*Device_EIP130, SBLIB_CFG_XM_MAILBOXNR);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to unlink mailbox %d. Res = %d.",
                (int)SBLIB_CFG_XM_MAILBOXNR,
                res);
    }
}


/*----------------------------------------------------------------------------
 * SBHYBRID_Get_SystemInfo
 */
static SB_Result_t
SBHYBRID_SystemInfo_Read(const Device_Handle_t Device_EIP130,
                         uint32_t * const HWVersion,
                         uint32_t * const FWVersion,
                         uint32_t * const MemSize,
                         uint32_t * const ErrorInfo)
{
    Eip130Token_Command_t t_cmd;
    Eip130Token_Result_t t_res;
    int res;

    // Format command token
    Eip130Token_Command_SystemInfo(&t_cmd);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

#ifdef TOKENS_VERBOSE
    L_PRINTF(LL_VERBOSE, LF_TOKEN, "Command (non-zero words only):");
    for (res = 0; res < EIP130TOKEN_COMMAND_WORDS; res++)
    {
        if (t_cmd.W[res])
        {
            L_PRINTF(LL_VERBOSE, LF_TOKEN, "  W%02d=0x%08X", res, t_cmd.W[res]);
        }
    }
#endif

    // Send command token to the SM
    res = EIP130_MailboxWriteAndSubmitToken(Device_EIP130,
                                            SBLIB_CFG_XM_MAILBOXNR,
                                            &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "Failed to process SystemInfo. res = %d.", res);
        return SB_ERROR_HARDWARE;
    }

    // Loop while waiting for reply
    while (EIP130_MailboxCanReadToken(Device_EIP130,
                                      SBLIB_CFG_XM_MAILBOXNR) == false)
    {
        // Do nothing while waiting
    }

    // Read the response token
    res = EIP130_MailboxReadToken(Device_EIP130,
                                  SBLIB_CFG_XM_MAILBOXNR,
                                  &t_res);
    if (res == 0)
    {
#ifdef TOKENS_VERBOSE
        if (t_res.W[0] & BIT_31)
        {
            // Error - only first word is relevant
            L_PRINTF(LL_VERBOSE, LF_TOKEN, "Error Result W00=0x%08X",
                     t_res.W[0]);
        }
        else
        {
            L_PRINTF(LL_VERBOSE, LF_TOKEN, "Result (non-zero words only):");
            for (res = 0; res < EIP130TOKEN_RESULT_WORDS; res++)
            {
                if (t_res.W[res])
                {
                    L_PRINTF(LL_VERBOSE, LF_TOKEN, "  W%02d=0x%08X",
                             res, t_res.W[res]);
                }
            }
        }
#endif
        if (Eip130Token_Result_Code(&t_res) >= 0)
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
 * SB_OTP_Read_PublicData
 */
SB_Result_t
SB_SystemInfo_Read(uint32_t * const HWVersion,
                   uint32_t * const FWVersion,
                   uint32_t * const MemSize,
                   uint32_t * const ErrorInfo)
{
    Device_Handle_t Device_EIP130 = NULL;
    SB_Result_t res;

    // Setup up contexts for retrieving the system information
    res = SBHYBRID_InitializeSystemInfo_HW(&Device_EIP130);
    if (res == SB_SUCCESS)
    {
        // Retrieving the system information
        res = SBHYBRID_SystemInfo_Read(Device_EIP130,
                                       HWVersion,
                                       FWVersion,
                                       MemSize,
                                       ErrorInfo);
    }

    // Uninitialize
    SBHYBRID_UninitializeSystemInfo_HW(&Device_EIP130);

    return res;
}

#endif /* !SBLIB_CF_REMOVE_SYSTEMINFO_READ */
#endif /* SBHYBRID_WITH_EIP130 */
/* end of file sbhybrid_eip130_system_info.c */
