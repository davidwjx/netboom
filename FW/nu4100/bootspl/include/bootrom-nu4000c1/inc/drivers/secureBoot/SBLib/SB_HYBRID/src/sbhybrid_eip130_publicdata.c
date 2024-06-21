/* sbhybrid_eip130_publicdata.c
 *
 * Description: Implementation of Secure Boot Public Data API for EIP130
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
#ifndef SBLIB_CF_REMOVE_PUBLICDATA

#include "basic_defs.h"
#include "sbif_attributes.h"
#include "sb_publicdata.h"              // API to implement

#include "eip130.h"
#include "device_mgmt.h"
#include "dmares_mgmt.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "eip130_token_common.h"
#include "eip130_token_asset.h"
#include "eip130_token_publicdata.h"
#include "eip130_token_result.h"

// For tracing/debugging purpose, dump the command and result tokens
//#define TOKENS_VERBOSE

// Types locally used for clarity.
typedef uint32_t assetid_t;


/*----------------------------------------------------------------------------
 * SBHYBRID_PublicData_ReadData
 *
 * This function reads the Public Data from the SM into the buffer within the
 * context and returns the length of the OTP data.
 */
static SB_Result_t
SBHYBRID_PublicData_ReadData(const Device_Handle_t Device_EIP130,
                             const assetid_t AssetId,
                             uint8_t * Data_p,
                             const uint32_t DataLen)
{
    Eip130Token_Command_t t_cmd;
    Eip130Token_Result_t t_res;
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Handle_t DMAHandle;
    uint64_t DataAddr;
    SB_Result_t sbres = SB_ERROR_HARDWARE;
    int res;

    // Get the DMA address of the data (DMA handle)
    Props.Alignment = 4;
    Props.Size = ((DataLen + 3) & ~3);

    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = Data_p;

    res = DMAResource_CheckAndRegister(Props, AddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return sbres;
    }

    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP13xDMA, &AddrPair);
    if (res != 0)
    {
        return sbres;
    }
    DataAddr = AddrPair.Address.Value64;

    // Format command token
    Eip130Token_Command_PublicData_Read(&t_cmd, AssetId, DataAddr, DataLen);
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
    if (res == 0)
    {
        // Loop while waiting for reply
        while (EIP130_MailboxCanReadToken(Device_EIP130,
                                          SBLIB_CFG_XM_MAILBOXNR) == false)
        {
            // Do nothing while waiting
        }

        // Read response token
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

            // Check reponse for errors (generic part of header)
            res = Eip130Token_Result_Code(&t_res);
            if (res >= 0)
            {
                // Length is already known
                sbres = SB_SUCCESS;
            }
            else if ((res == EIP130TOKEN_RESULT_INVALID_ASSET) ||
                     (res == EIP130TOKEN_RESULT_ACCESS_ERROR))
            {
                sbres = SB_ERROR_ARGUMENTS;
            }
        }
    }

    // Release DMA handle
    DMAResource_Release(DMAHandle);

    if (res != 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_PublicData_ReadData failed (%d, %d).",
                res, sbres);
    }
    return sbres;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_PublicData_Search
 */
static
SB_Result_t
SBHYBRID_PublicData_Search(const Device_Handle_t Device_EIP130,
                           uint32_t ObjectNr,
                           assetid_t * const AssetId_p,
                           uint32_t * const DataLen_p)
{
    Eip130Token_Command_t t_cmd;
    Eip130Token_Result_t t_res;
    int res;

    *AssetId_p = 0;

    // Format command token
    Eip130Token_Command_AssetSearch(&t_cmd, (uint8_t)ObjectNr);

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

    // Send command token
    res = EIP130_MailboxWriteAndSubmitToken(Device_EIP130,
                                            SBLIB_CFG_XM_MAILBOXNR,
                                            &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_PublicData_Search command failed (%d).",
                res);
        return SB_ERROR_HARDWARE;
    }

    // Loop while waiting for reply
    while (EIP130_MailboxCanReadToken(Device_EIP130,
                                      SBLIB_CFG_XM_MAILBOXNR) == false)
    {
        // Do nothing while waiting
    }

    // Read response token
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

        // Check reponse for errors (generic part of header)
        res = Eip130Token_Result_Code(&t_res);
        if (res >= 0)
        {
            // Read the AssetId and Length from the response
            Eip130Token_Result_AssetSearch(&t_res, AssetId_p, DataLen_p);
            return SB_SUCCESS;
        }
        if (res == EIP130TOKEN_RESULT_INVALID_ASSET)
        {
            // No asset with this index, use special return value
            L_DEBUG(LF_SBHYBRID,
                    "SBHYBRID_PublicData_Search asset number %u invalid.",
                    (unsigned int)ObjectNr);
            return SB_ERROR_ARGUMENTS;
        }
    }

    L_DEBUG(LF_SBHYBRID, "SBHYBRID_PublicData_Search failed (%d).", res);
    return SB_ERROR_HARDWARE;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_PublicData_Read
 *
 * Uses the OTP_Read token to retrieve OTP data from the SM.
 */
static SB_Result_t
SBHYBRID_PublicData_Read(const Device_Handle_t Device_EIP130,
                         const uint32_t ObjectNr,
                         uint8_t * Data_p,
                         uint32_t * const DataLen_p)
{
    assetid_t AssetId = 0;
    uint32_t FoundLen = 0;
    SB_Result_t sbres;

    if (DataLen_p == NULL)
    {
        return SB_ERROR_ARGUMENTS;
    }

    if (ObjectNr > 62)
    {
        return SB_ERROR_ARGUMENTS;
    }

    sbres = SBHYBRID_PublicData_Search(Device_EIP130,
                                       ObjectNr, &AssetId, &FoundLen);
    if (sbres != SB_SUCCESS)
    {
        return sbres;
    }

    if (AssetId == 0)
    {
        // OTP object was not found
        return SB_ERROR_ARGUMENTS;
    }

    if (Data_p == NULL)
    {
        // Only update the length
        *DataLen_p = FoundLen;
        return SB_SUCCESS;
    }

    sbres = SBHYBRID_PublicData_ReadData(Device_EIP130,
                                         AssetId, Data_p, *DataLen_p);
    return sbres;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_InitializePublicData_HW
 *
 * Initialize hardware for Public Data read.
 */
static SB_Result_t
SBHYBRID_InitializePublicData_HW(Device_Handle_t * const  Device_EIP130_p)
{
    int res;

    // initialize Driver Framework
    res = Device_Initialize(NULL);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_InitializePublicData_HW framework failed (%d).",
                res);
        return SB_ERROR_HARDWARE;
    }

    // EIP130 Initialisation
    *Device_EIP130_p = Device_Find(SBLIB_CFG_SM_DEVICE);
    if (*Device_EIP130_p == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_InitializePublicData_HW %s not found.",
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
                "SBHYBRID_InitializePublicData_HW mailbox %d link failed (%d).",
                (int)SBLIB_CFG_XM_MAILBOXNR, res);
        return SB_ERROR_HARDWARE;
    }
    return SB_SUCCESS;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_UninitializePublicData_HW
 *
 * Uninitialize hardware after Public Data read.
 */
static void
SBHYBRID_UninitializePublicData_HW(Device_Handle_t * const  Device_EIP130_p)
{
    if (*Device_EIP130_p)
    {
        int res = EIP130_MailboxUnlink(*Device_EIP130_p, SBLIB_CFG_XM_MAILBOXNR);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID,
                    "SBHYBRID_UninitializePublicData_HW mailbox %d unlink failed (%d).",
                    (int)SBLIB_CFG_XM_MAILBOXNR, res);
        }
        *Device_EIP130_p = NULL;
    }
}


/*----------------------------------------------------------------------------
 * SB_PublicData_Read
 */
SB_Result_t
SB_PublicData_Read(const uint32_t ObjectNr,
                   uint8_t * Data_p,
                   uint32_t * const DataLen_p)
{
    Device_Handle_t Device_EIP130 = NULL;
    SB_Result_t sbres;

    L_TRACE(LF_SBHYBRID, "SB_PublicData_Read: ObjectNr = %d", ObjectNr);

    // Setup up contexts for retrieving the Public Data
    sbres = SBHYBRID_InitializePublicData_HW(&Device_EIP130);
    if (sbres == SB_SUCCESS)
    {
        // Retrieve the Public Data
        sbres = SBHYBRID_PublicData_Read(Device_EIP130,
                                         ObjectNr, Data_p, DataLen_p);
    }

    // Uninitialize
    SBHYBRID_UninitializePublicData_HW(&Device_EIP130);

    return sbres;
}

#endif /* !SBLIB_CF_REMOVE_PUBLICDATA */
#endif /* SBHYBRID_WITH_EIP130 */

/* end of file sbhybrid_eip130_publicdata.c */
