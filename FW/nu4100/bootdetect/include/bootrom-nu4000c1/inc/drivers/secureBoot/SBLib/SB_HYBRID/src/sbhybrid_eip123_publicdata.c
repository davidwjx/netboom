/* sbhybrid_eip123_publicdata.c
 *
 * Description: Implementation of Secure Boot Public Data API for EIP123
 */

/*****************************************************************************
* Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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
#ifndef SBLIB_CF_REMOVE_PUBLICDATA

#include "sbif_attributes.h"

#include "basic_defs.h"
#include "c_lib.h"                       // c_memcpy

#include "cm_tokens_misc.h"
#include "cm_tokens_asset.h"
#include "cm_tokens_errdetails.h"

#include "sb_publicdata.h"              // API to implement
#include "eip123.h"

#include "device_mgmt.h"
#include "dmares_mgmt.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"

// Types locally used for clarity.
typedef uint32_t assetid_t;


/*----------------------------------------------------------------------------
 * SBHYBRID_Init_AllocDMABuf
 *
 * This function allocates a DMA-safe buffer of requested length.
 * The handle, EIP-123 DMA address and pointer are returned.
 *
 * Return Value:
 *     0    Success
 *     -1   Error
 */
static int
SBHYBRID_Init_AllocDMABuf(const unsigned int Size,
                          DMAResource_Handle_t * const DMAHandle_p,
                          uint32_t * const PhysAddr_p,
                          void ** Host_pp)
{
    // allocate buffers for the DMA buffer descriptor chains
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    int res;

    Props.Size = Size;
    Props.Alignment = 4;

    res = DMAResource_Alloc(Props, &AddrPair, DMAHandle_p);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "SBHYBRID_Init_AllocDMABuf alloc failed (%d)", res);
        return -1;
    }

    if (Host_pp != NULL)
    {
        *Host_pp = AddrPair.Address.Native_p;
        L_TRACE(LF_SBHYBRID,
                "SBHYBRID_Init_AllocDMABuf - DMAHandle=%p HostAddr=%p  Size=%u",
                *DMAHandle_p,
                AddrPair.Address.Native_p,
                (unsigned int)Size);
    }

    res = DMAResource_Translate(*DMAHandle_p, DMARES_DOMAIN_EIP12xDMA, &AddrPair);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_Init_AllocDMABuf translate failed (%d)",
                res);
        return -1;
    }

    *PhysAddr_p = AddrPair.Address.Value32;
    L_TRACE(LF_SBHYBRID,
            "SBHYBRID_Init_AllocDMABuf - DMAHandle=%p PhysAddr=%u  Size=%u",
            *DMAHandle_p,
            (unsigned int)AddrPair.Address.Value32,
            (unsigned int)Size);

    return 0;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_PublicData_ReadData
 *
 * This function reads the Public Data from the SM into the buffer within the
 * context and returns the length of the OTP data.
 */
static SB_Result_t
SBHYBRID_PublicData_ReadData(const Device_Handle_t Device_EIP123,
                             const assetid_t AssetId,
                             uint8_t * Data_p,
                             const uint32_t DataLen)
{
    CMTokens_Command_t t_cmd;
    CMTokens_Response_t t_res;
    DMAResource_Handle_t DMAHandle;
    uint32_t TokenDataAddr = 0;
    void * HostDataAddr_p = NULL;
    SB_Result_t sbres = SB_ERROR_HARDWARE;
    int res;

    res = SBHYBRID_Init_AllocDMABuf(((DataLen + 3) & ~3),
                                    &DMAHandle,
                                    &TokenDataAddr,
                                    &HostDataAddr_p);
    if (res == 0)
    {
        // Format command token
        CMTokens_MakeToken_Clear(&t_cmd);
        CMTokens_MakeCommand_NVM_Read(&t_cmd, AssetId, DataLen);
        CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
        t_cmd.W[4] = TokenDataAddr;
        t_cmd.W[5] = DataLen;

        DMAResource_PreDMA(DMAHandle, 0, 0);

        // Send command token to the CM
        res = EIP123_WriteAndSubmitToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR, &t_cmd);
        if (res == 0)
        {
            // Loop while waiting for reply
            while (EIP123_CanReadToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR) == false)
            {
                // Do nothing while waiting
            }

            // Read response token
            res = EIP123_ReadToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR, &t_res);
            if (res == 0)
            {
                // Check reponse for errors (generic part of header)
                if (CMTokens_ParseResponse_Generic(&t_res) == 0)
                {
                    DMAResource_PostDMA(DMAHandle, 0, 0);
                    c_memcpy(Data_p, HostDataAddr_p, DataLen);
                    sbres = SB_SUCCESS;
                }
                else
                {
                    res = CMTokens_ParseResponse_ErrorDetails(&t_res, NULL);
                    if (res == CMTOKENS_RESULT_SEQ_INVALID_ASSET)
                    {
                        sbres = SB_ERROR_ARGUMENTS;
                    }
                }
            }
        }

        // Release DMA handle
        DMAResource_Release(DMAHandle);
    }

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
SBHYBRID_PublicData_Search(const Device_Handle_t Device_EIP123,
                           uint32_t ObjectNr,
                           assetid_t * const AssetId_p,
                           uint32_t * const DataLen_p)
{
    CMTokens_Command_t t_cmd;
    CMTokens_Response_t t_res;
    int res;

    *AssetId_p = 0;

    // Format command token
    CMTokens_MakeToken_Clear(&t_cmd);
    CMTokens_MakeCommand_AssetSearch(&t_cmd, (uint8_t)ObjectNr);

    // Send command token
    res = EIP123_WriteAndSubmitToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR, &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_PublicData_Search command failed (%d).",
                res);
        return SB_ERROR_HARDWARE;
    }

    // Loop while waiting for reply
    while (EIP123_CanReadToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR) == false)
    {
        // Do nothing while waiting
    }

    // Read response token
    res = EIP123_ReadToken(Device_EIP123, SBLIB_CFG_XM_MAILBOXNR, &t_res);
    if (res == 0)
    {
        // Check reponse for errors (generic part of header)
        if (CMTokens_ParseResponse_Generic(&t_res) == 0)
        {
            // Read the AssetId and Length from the response
            CMTokens_ParseResponse_AssetSearch(&t_res, AssetId_p, DataLen_p);
            return SB_SUCCESS;
        }

        res = CMTokens_ParseResponse_ErrorDetails(&t_res, NULL);
        if (res == CMTOKENS_RESULT_SEQ_INVALID_ASSET)
        {
            // No asset with this index, use special return value
            L_DEBUG(LF_SBHYBRID,
                    "SBHYBRID_PublicData_Search asset number %d invalid.",
                    ObjectNr);
            return SB_ERROR_ARGUMENTS;
        }
    }

    L_DEBUG(LF_SBHYBRID, "SBHYBRID_PublicData_Search failed (%d).", res);
    return SB_ERROR_HARDWARE;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_PublicData_Read
 *
 * Uses the NVM_Read token to retrieve NVM data from the CM.
 */
static SB_Result_t
SBHYBRID_PublicData_Read(const Device_Handle_t Device_EIP123,
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

    sbres = SBHYBRID_PublicData_Search(Device_EIP123, ObjectNr, &AssetId, &FoundLen);
    if (sbres != SB_SUCCESS)
    {
        return sbres;
    }

    if (AssetId == 0)
    {
        // NVM object was not found
        return SB_ERROR_ARGUMENTS;
    }

    if (Data_p == NULL)
    {
        // Only update the length
        *DataLen_p = FoundLen;
        return SB_SUCCESS;
    }

    sbres = SBHYBRID_PublicData_ReadData(Device_EIP123, AssetId, Data_p, *DataLen_p);
    return sbres;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_InitializePublicData_HW
 *
 * Initialize hardware for Public Data read.
 */
static SB_Result_t
SBHYBRID_InitializePublicData_HW(Device_Handle_t * const  Device_EIP123_p)
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
    *Device_EIP123_p = Device_Find(SBLIB_CFG_CM_DEVICE);
    if (*Device_EIP123_p == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "SBHYBRID_InitializePublicData_HW %s not found.",
                SBLIB_CFG_CM_DEVICE);
        return SB_ERROR_HARDWARE;
    }

    // link the mailbox we want to use for requests from this host
    // Note: After each reset, the Master CPU must allow this explicitly.
    //       Please refer to EIP123_MailboxAccessControl in eip123.h
    //       and LOCKOUT register in SafeXcel-IP-123 HW Reference Manual
    res = EIP123_Link(*Device_EIP123_p, SBLIB_CFG_XM_MAILBOXNR);
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
SBHYBRID_UninitializePublicData_HW(Device_Handle_t * const  Device_EIP123_p)
{
    if (*Device_EIP123_p)
    {
        int res = EIP123_Unlink(*Device_EIP123_p, SBLIB_CFG_XM_MAILBOXNR);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID,
                    "SBHYBRID_UninitializePublicData_HW mailbox %d unlink failed (%d).",
                    (int)SBLIB_CFG_XM_MAILBOXNR, res);
        }
        *Device_EIP123_p = NULL;
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
    Device_Handle_t Device_EIP123 = NULL;
    SB_Result_t sbres;

    L_TRACE(LF_SBHYBRID, "SB_PublicData_Read: ObjectNr = %d", ObjectNr);

    // Setup up contexts for retrieving the Public Data
    sbres = SBHYBRID_InitializePublicData_HW(&Device_EIP123);
    if (sbres == SB_SUCCESS)
    {
        // Retrieve the Public Data
        sbres = SBHYBRID_PublicData_Read(Device_EIP123,
                                         ObjectNr,
                                         Data_p, DataLen_p);
    }

    // Uninitialize
    SBHYBRID_UninitializePublicData_HW(&Device_EIP123);

    return sbres;
}

#endif /* !SBLIB_CF_REMOVE_PUBLICDATA */
#endif /* SBHYBRID_WITH_EIP123 */

/* end of file sbhybrid_eip123_publicdata.c */
