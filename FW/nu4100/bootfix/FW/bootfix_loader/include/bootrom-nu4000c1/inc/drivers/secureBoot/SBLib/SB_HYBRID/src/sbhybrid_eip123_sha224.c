/* sbhybrid_eip123_sha224.c
 *
 * Hash service for Secure Boot Library, using Crypto Module for acceleration.
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

#include "sbhybrid_internal.h"
#ifdef SBHYBRID_WITH_EIP123
#if SBHYBRID_DIGEST_BYTES == 28

#include "cs_cm_tokens.h"
#include "cm_tokens_hash.h"
#include "cm_tokens_errdetails.h"
#include "eip123.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"

#include "c_lib.h"

#undef L_TRACE
#define L_TRACE L_DEBUG

/* This file implements SHA-224. */
#define SBHYBRID_HASH_ALGORITHM  CMTOKENS_HASH_ALGORITHM_SHA224

#ifdef IMPLDEFS_CF_DISABLE_L_DEBUG
/*----------------------------------------------------------------------------
 * SBHYBRID_EIP123_Common_ReadResultAndCheckFunc
 *
 * This function reads the EIP-123 reply message from the mailbox, checks for
 * errors and optionally reports the error. This function is sufficient for
 * most responses and is called from most SBHYBRID_*_ReadResult functions.
 * This function is shared by hashing and decryption.
 */
SB_Result_t
SBHYBRID_EIP123_Common_ReadResultAndCheckFunc(Device_Handle_t const Device_EIP123,
                                              CMTokens_Response_t * t_res_p,
                                              uint8_t MailboxNr)
{
    int res = EIP123_ReadToken(Device_EIP123, MailboxNr, t_res_p);
    if (res == 0)
    {
        if (CMTokens_ParseResponse_Generic(t_res_p) == 0)
        {
            return SB_SUCCESS;
        }
    }

    return SB_ERROR_HARDWARE;
}
#endif /* IMPLDEFS_CF_DISABLE_L_DEBUG */

#ifndef IMPLDEFS_CF_DISABLE_L_DEBUG
/*----------------------------------------------------------------------------
 * SBHYBRID_EIP123_Common_ReadResultAndCheckFuncDebug
 *
 * This function reads the EIP-123 reply message from the mailbox, checks for
 * errors and optionally reports the error. This function is sufficient for
 * most responses and is called from most SBHYBRID_*_ReadResult functions.
 * This function is shared by hashing and decryption.
 * Same as SBHYBRID_EIP123_Common_ReadResultAndCheckFunc but produces
 * debug logging with provided operation string.
 */
SB_Result_t
SBHYBRID_EIP123_Common_ReadResultAndCheckFuncDebug(Device_Handle_t const Device_EIP123,
                                                   CMTokens_Response_t * t_res_p,
                                                   uint8_t MailboxNr,
                                                   const char * OperationStr_p)
{
    int res;

    res = EIP123_ReadToken(Device_EIP123, MailboxNr, t_res_p);
    if (res == 0)
    {
        if (CMTokens_ParseResponse_Generic(t_res_p) != 0)
        {
#if !defined(CMTOKENS_REMOVE_ERROR_DESCRIPTIONS)
            const char * Text_p = NULL;

            res = CMTokens_ParseResponse_ErrorDetails(t_res_p, &Text_p);

            L_DEBUG(LF_SBHYBRID, "%s error: %s (%d).",
                    OperationStr_p, Text_p, res);
#else
            PARAMETER_NOT_USED(OperationStr_p);
            res = CMTokens_ParseResponse_ErrorDetails(t_res_p, NULL);
#endif

            res = 0 - res;
        }
    }
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "%s failed: res = %d.", OperationStr_p, res);

        return SB_ERROR_HARDWARE;
    }

    return SB_SUCCESS;
}
#endif /* IMPLDEFS_CF_DISABLE_L_DEBUG */

/*----------------------------------------------------------------------------
 * SBHYBRID_Hash_SendCommand
 */
static SB_Result_t
SBHYBRID_Hash_SendPartialCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                                 EIP123_DescriptorChain_t * const DC_p,
                                 const uint32_t DataLen,
                                 const uint32_t TotalLen,
                                 const uint8_t Finalize)
{
    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken Hash.");
    L_DEBUG(LF_SBHYBRID, "EIP123 WriteToken Hash (%d bytes)%s",
            DataLen, Finalize ? " (final)" : "");

    {
        CMTokens_Command_t t_cmd;

        CMTokens_MakeCommand_Hash_SetLengthAlgoMode(&t_cmd,
                                                    DataLen,
                                                    SBHYBRID_HASH_ALGORITHM,
                                                    /*fInitWithDefault:*/false,
                                                    Finalize);

        CMTokens_MakeCommand_Hash_SetTotalMessageLength(&t_cmd,
                                                        /*LSW:*/TotalLen,
                                                        /*MSW:*/0);

        CMTokens_MakeCommand_Hash_WriteInDescriptor(&t_cmd, DC_p);

        CMTokens_MakeCommand_Hash_CopyDigest(&t_cmd,
                                             32,
                                             ImageProcess_p->Hash);
        {
            int res;

            CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

            res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                             SBLIB_CFG_XM_MAILBOXNR,
                                             &t_cmd);
            if (res < 0)
            {
                L_DEBUG(LF_SBHYBRID, "Failed to start Hash. res = %d.", res);

                return SB_ERROR_HARDWARE;
            }
        }

        L_DEBUG(LF_SBHYBRID, "Successfully started Hash.");
    }

    return SBHYBRID_PENDING;
}

/* SBHYBRID interface to EIP-123 hashes. */

SB_Result_t
SBHYBRID_EIP123_SHA224_Init(SBHYBRID_SymmContext_t * const Hash_p)
{
    static const uint8_t SHA224_InitialDigest[32] =
    {
        0xc1, 0x05, 0x9e, 0xd8,
        0x36, 0x7c, 0xd5, 0x07,
        0x30, 0x70, 0xdd, 0x17,
        0xf7, 0x0e, 0x59, 0x39,
        0xff, 0xc0, 0x0b, 0x31,
        0x68, 0x58, 0x15, 0x11,
        0x64, 0xf9, 0x8f, 0xa7,
        0xbe, 0xfa, 0x4f, 0xa4
    };

    Hash_p->hash_initialized = false;
    Hash_p->hash_finalize = false;
    Hash_p->FragCount = 0;
    Hash_p->DC_Hash.DataLen = 0;
    Hash_p->DC_Hash.TotalLen = 0;
    c_memcpy(&Hash_p->Hash, SHA224_InitialDigest, 32);

    return SB_SUCCESS;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_AddVectorToFragList
 */
static int
SBHYBRID_AddVectorToFragList(SBHYBRID_SymmContext_t * const ImageProcess_p,
                             const SBIF_SGVector_t * const Vector_p,
                             EIP123_Fragment_t * const Frag_p)
{
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Handle_t DMAHandle;
    int res;

    ASSERT(Vector_p->DataLen != 0);

    Props.Size = Vector_p->DataLen;
    Props.Alignment = 4;

    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = Vector_p->Data_p;

    res = DMAResource_CheckAndRegister(Props, AddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return -1;
    }

    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP12xDMA, &AddrPair);
    if (res != 0)
    {
        return -1;
    }

#ifdef SBHYBRID_DO_DMARESOURCE_RELEASE
    // remember the handle
    ImageProcess_p->DMAHandles[ImageProcess_p->DMAHandleCount++] = DMAHandle;
#else
    IDENTIFIER_NOT_USED(ImageProcess_p);
#endif

    DMAResource_PreDMA(DMAHandle, 0, 0);

    // copy the DMA address details to the FragList
    Frag_p->Length = Props.Size;
    Frag_p->StartAddress = AddrPair.Address.Value32;

    return 0;                           // no error
}

SB_Result_t
SBHYBRID_EIP123_SHA224_RunFsm(SBHYBRID_SymmContext_t * const Hash_p)
{
    if (Hash_p->hash_initialized == false)
    {
        /* Issue command. */
        EIP123_Status_t res123;

        DMAResource_PreDMA(Hash_p->DC_Hash.DMAHandle, 0, 0);

        res123 = EIP123_DescriptorChain_Populate(&Hash_p->DC_Hash.DC,
                                                 Hash_p->DC_Hash.DMAHandle,
                                                 Hash_p->DC_Hash.PhysAddr,
                                                 /*fIsInput:*/true,
                                                 Hash_p->FragCount,
                                                 Hash_p->Frags,
                                                 EIP123_ALGOBLOCKSIZE_HASH,
                                                 /*TokenIDPhysAddr:*/0); // n/a (not output)
        if (res123 != EIP123_STATUS_SUCCESS)
        {
            L_DEBUG(LF_SBHYBRID, "Failed to populate hash descriptor chain");
            return SB_ERROR_HARDWARE;
        }

        L_TRACE(LF_SBHYBRID,
                "Populated hash descriptor chain: DMAHandle=%p "
                "DescChain: Entries=%d wrtok=%d handle=%p src0=%x dst0=%x "
                "li0=%x dmal0=%x (DataLen=%d)", Hash_p->DC_Hash.DMAHandle,
                Hash_p->DC_Hash.DC.EntriesUsed,
                (int)Hash_p->DC_Hash.DC.fHasWriteTokenID,
                Hash_p->DC_Hash.DC.DMAHandle,
                Hash_p->DC_Hash.DC.not_exposed[0],
                Hash_p->DC_Hash.DC.not_exposed[1],
                Hash_p->DC_Hash.DC.not_exposed[2],
                Hash_p->DC_Hash.DC.not_exposed[3],
                (int)Hash_p->DC_Hash.DataLen);

        if (SBHYBRID_Hash_SendPartialCommand(Hash_p,
                                             &Hash_p->DC_Hash.DC,
                                             Hash_p->DC_Hash.DataLen,
                                             Hash_p->DC_Hash.TotalLen,
                                             Hash_p->hash_finalize) != SBHYBRID_PENDING)
        {
            return SB_ERROR_HARDWARE;
        }

        Hash_p->hash_initialized = true;
        return SBHYBRID_PENDING;        /* Command successfully issued. */
    }
    else
    {
        /* Command is processing. */
        CMTokens_Response_t t_res;
        SB_Result_t sbres;

        if (!EIP123_CanReadToken(Hash_p->Device_EIP123,
                                 SBLIB_CFG_XM_MAILBOXNR))
        {
            /* Command not yet ready. */
            return SBHYBRID_PENDING;
        }

        sbres = SBHYBRID_EIP123_Common_ReadResultAndCheck(Hash_p->Device_EIP123,
                                                          &t_res,
                                                          SBLIB_CFG_XM_MAILBOXNR,
                                                          "Hash");

        L_TRACE(LF_SBHYBRID, "Hash result gotten: %d", (int)sbres);

        Hash_p->FragCount = 0;
        Hash_p->hash_initialized = false;
        Hash_p->DC_Hash.DataLen = 0;

        CMTokens_ParseResponse_Hash_CopyDigest(&t_res, 32, &Hash_p->Hash[0]);

        DMAResource_PostDMA(Hash_p->DC_Hash.DMAHandle, 0, 0);
        return sbres;
    }
}

SB_Result_t
SBHYBRID_EIP123_SHA224_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                                const uint8_t * DataBytes_p,
                                unsigned int DataByteCount,
                                bool fFinal)
{
    SBIF_SGVector_t vector;
    int res;

    vector.Data_p = (uint32_t *)DataBytes_p;
    vector.DataLen = (uint32_t)DataByteCount;

    res = SBHYBRID_AddVectorToFragList(Hash_p,
                                       &vector,
                                       &Hash_p->Frags[Hash_p->FragCount]);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "Failed to register input vector %u",
                Hash_p->FragCount);
        return SB_ERROR_HARDWARE;
    }

    L_TRACE(LF_SBHYBRID,
            "DataFragment In %u: address=0x%x  length=0x%x.",
            Hash_p->FragCount,
            Hash_p->Frags[Hash_p->FragCount].StartAddress,
            Hash_p->Frags[Hash_p->FragCount].Length);

    Hash_p->FragCount += 1;
    Hash_p->DC_Hash.DataLen += DataByteCount;
    Hash_p->DC_Hash.TotalLen += DataByteCount;

    if (fFinal)
    {
        Hash_p->hash_finalize = true;
    }

    return SB_SUCCESS;
}

void
SBHYBRID_EIP123_SHA224_DmaRelease(SBHYBRID_SymmContext_t * const Hash_p)
{
    while (Hash_p->DMAHandleCount > 0)
    {
        Hash_p->DMAHandleCount--;
        DMAResource_Release(Hash_p->DMAHandles[Hash_p->DMAHandleCount]);
        Hash_p->DMAHandles[Hash_p->DMAHandleCount] = 0;
    }
}

void
SBHYBRID_EIP123_SHA224_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                                 uint8_t * Digest_p)
{
    SBHYBRID_EIP123_SHA224_DmaRelease(Hash_p);
    c_memcpy(Digest_p, &Hash_p->Hash[0], SBHYBRID_DIGEST_BYTES);
}

#endif /* SBHYBRID_DIGEST_BYTES == 28 */
#endif /* SBHYBRID_WITH_EIP123 */
/* end of file sbhybrid_eip123_sha224.c */
