/* sbhybrid_eip130_hash.c
 *
 * Hash services for the Secure Boot Library, using Security Module for
 * acceleration.
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

#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_EIP130
#include "c_lib.h"

#include "eip130.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "cs_eip130_token.h"
#include "eip130_token_hash.h"
#include "eip130_token_asset.h"

#undef L_TRACE
#define L_TRACE L_DEBUG

#ifdef SBLIB_CFG_SM_SECURE_ACCESS
#define SBHYBRID_ACCESS_POLICY  0
#else
#define SBHYBRID_ACCESS_POLICY  EIP130TOKEN_ASSET_POLICY_SOURCE_NON_SECURE
#endif

#if SBHYBRID_DIGEST_BYTES == 64
// Implements SHA-512
#define SBHYBRID_HASH_ALGORITHM  EIP130TOKEN_HASH_ALGORITHM_SHA512
#define SBHYBRID_HASH_POLICY     (SBHYBRID_ACCESS_POLICY            | \
                                  EIP130TOKEN_ASSET_POLICY_TEMP_MAC | \
                                  EIP130TOKEN_ASSET_POLICY_SHA512)
#elif SBHYBRID_DIGEST_BYTES == 48
// Implements SHA-384
#define SBHYBRID_HASH_ALGORITHM  EIP130TOKEN_HASH_ALGORITHM_SHA384
#define SBHYBRID_HASH_POLICY     (SBHYBRID_ACCESS_POLICY            | \
                                  EIP130TOKEN_ASSET_POLICY_TEMP_MAC | \
                                  EIP130TOKEN_ASSET_POLICY_SHA384)
#elif SBHYBRID_DIGEST_BYTES == 32
// Implements SHA-256
#define SBHYBRID_HASH_ALGORITHM  EIP130TOKEN_HASH_ALGORITHM_SHA256
#define SBHYBRID_HASH_POLICY     (SBHYBRID_ACCESS_POLICY            | \
                                  EIP130TOKEN_ASSET_POLICY_TEMP_MAC | \
                                  EIP130TOKEN_ASSET_POLICY_SHA256)
#else
// Implements SHA-224
#define SBHYBRID_HASH_ALGORITHM  EIP130TOKEN_HASH_ALGORITHM_SHA224
#define SBHYBRID_HASH_POLICY     (SBHYBRID_ACCESS_POLICY            | \
                                  EIP130TOKEN_ASSET_POLICY_TEMP_MAC | \
                                  EIP130TOKEN_ASSET_POLICY_SHA224)
#endif


/*----------------------------------------------------------------------------
 * SBHYBRID_Hash_SendPartialCommand
 * ------------------------------------------------------------------------- */
static SB_Result_t
SBHYBRID_Hash_SendPartialCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 const uint8_t Initial,
                                 const uint32_t AssetId,
                                 const uint64_t DataAddress,
                                 const uint32_t DataLength,
                                 const uint64_t TotalDataLength,
                                 const uint8_t Finalize)
{
    Eip130Token_Command_t t_cmd;

    L_DEBUG(LF_SBHYBRID, "EIP130 Hash token write (%u bytes %s%s)",
            (unsigned int)DataLength, Initial ? "I" : "c", Finalize ? "F" : "c");

    Eip130Token_Command_Hash(&t_cmd, SBHYBRID_HASH_ALGORITHM,
                             Initial, Finalize,
                             DataAddress, DataLength);
    Eip130Token_Command_Hash_SetTempDigestASID(&t_cmd, AssetId);
    Eip130Token_Command_Hash_SetTotalMessageLength(&t_cmd, TotalDataLength);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}


SB_Result_t
SBHYBRID_EIP130_Hash_Init(SBHYBRID_SymmContext_t * const Context_p)
{
    SB_Result_t res = SBHYBRID_PENDING;

    // write token for asset creation digest
    if (EIP130_MailboxCanWriteToken(Context_p->Device_EIP130,
                                    SBLIB_CFG_XM_MAILBOXNR))
    {
        if (Context_p->HashTempAssetId == 0)
        {
#if SBHYBRID_DIGEST_BYTES > 32
            res = SBHYBRID_AssetCreate_SendCommand(Context_p,
                                                   SBHYBRID_HASH_POLICY,
                                                   64);
#else
            res = SBHYBRID_AssetCreate_SendCommand(Context_p,
                                                   SBHYBRID_HASH_POLICY,
                                                   32);
#endif
            if (res == SBHYBRID_PENDING)
            {
                while (!EIP130_MailboxCanReadToken(Context_p->Device_EIP130,
                                                   SBLIB_CFG_XM_MAILBOXNR))
                {
                    // Wait until it is possible to read the token
                }

                // Read response token after token for asset creation
                res = SBHYBRID_AssetCreateSearch_ReadResult(Context_p,
                                                            &Context_p->HashTempAssetId);
            }
        }
        else
        {
            res = SB_SUCCESS;
        }
    }
    else
    {
        return res;
    }

    if (res == SB_SUCCESS)
    {
        Context_p->HashInitialize  = true;
        Context_p->HashFinalize    = false;
        Context_p->HashDigestValid = false;
        Context_p->HashTotalLength = 0;
#ifdef SBHYBRID_WITH_EIP130PK
        Context_p->HashData_p      = NULL;
        Context_p->HashDataLength  = 0;
#endif /* SBHYBRID_WITH_EIP130PK */
    }
    else
    {
        res = SB_ERROR_HARDWARE;
    }
    return res;
}


SB_Result_t
SBHYBRID_EIP130_Hash_AddBlock(SBHYBRID_SymmContext_t * const Context_p,
                              const uint8_t * DataBytes_p,
                              const uint32_t DataByteCount,
                              const bool fFinal)
{
    if (DataByteCount > SBHYBRID_MAX_SIZE_DATA_BYTES)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Exceeded maximum DMA size");
        return SB_ERROR_HARDWARE;
    }

    // Write token for hash operation
    if (EIP130_MailboxCanWriteToken(Context_p->Device_EIP130,
                                    SBLIB_CFG_XM_MAILBOXNR))
    {
        SB_Result_t sbres;

        Context_p->HashTotalLength += DataByteCount;

        if (fFinal)
        {
            Context_p->HashFinalize = true;

            if (Context_p->HashInitialize)
            {
                // Single operation - assume public key hash
                Context_p->HashDigestValid = true;
            }
            else
            {
#ifdef SBHYBRID_WITH_EIP130PK
                Context_p->HashData_p     = DataBytes_p;
                Context_p->HashDataLength = DataByteCount;

                L_DEBUG(LF_SBHYBRID, "Final hash will be done during the signature verify operation");
                return SB_SUCCESS;
#endif /* SBHYBRID_WITH_EIP130PK */
            }
        }

        // Get the physical address of the data
        uint64_t DataAddress = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                                    DataBytes_p,
                                                                    DataByteCount);
        if (DataAddress == 0)
        {
            return SB_ERROR_HARDWARE;
        }

        sbres = SBHYBRID_Hash_SendPartialCommand(Context_p,
                                                 Context_p->HashInitialize,
                                                 Context_p->HashTempAssetId,
                                                 DataAddress,
                                                 DataByteCount,
                                                 Context_p->HashTotalLength,
                                                 Context_p->HashFinalize);
        if (sbres == SBHYBRID_PENDING)
        {
            Context_p->HashInitialize = false;

#ifdef SBHYBRID_WITH_EIP130PK
            if (!Context_p->HashDigestValid)
            {
                Eip130Token_Result_t t_res;

                while (!EIP130_MailboxCanReadToken(Context_p->Device_EIP130,
                                                   SBLIB_CFG_XM_MAILBOXNR))
                {
                    // Wait until it is possible to read the token
                }

                sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                                  &t_res,
                                                                  SBLIB_CFG_XM_MAILBOXNR);

                SBHYBRID_EIP130_Common_DmaRelease(Context_p);
                return sbres;
            }
#endif /* SBHYBRID_WITH_EIP130PK */
            return SB_SUCCESS;
        }

        L_DEBUG(LF_SBHYBRID, "ERROR: Hash token not written");
        return SB_ERROR_HARDWARE;
    }

    // Previous token still busy
    return SBHYBRID_PENDING;
}


SB_Result_t
SBHYBRID_EIP130_Hash_RunFsm(SBHYBRID_SymmContext_t * const Context_p)
{
    if (Context_p->HashDigestValid)
    {
        if (EIP130_MailboxCanReadToken(Context_p->Device_EIP130,
                                       SBLIB_CFG_XM_MAILBOXNR))
        {
            Eip130Token_Result_t t_res;
            SB_Result_t sbres;

            sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                              &t_res,
                                                              SBLIB_CFG_XM_MAILBOXNR);
            if (sbres == SB_SUCCESS)
            {
#if SBHYBRID_DIGEST_BYTES > 32
                Eip130Token_Result_Hash_CopyDigest(&t_res, 64, Context_p->Hash);
#else
                Eip130Token_Result_Hash_CopyDigest(&t_res, 32, Context_p->Hash);
#endif
            }

            SBHYBRID_EIP130_Common_DmaRelease(Context_p);
            return sbres;
        }
        return SBHYBRID_PENDING;
    }

    // Hash response is already handled during Hash AddBlock or
    // will be handled during the signature verify operation
    return SB_SUCCESS;
}


void
SBHYBRID_EIP130_Hash_GetDigest(SBHYBRID_SymmContext_t * const Context_p,
                               uint8_t * Digest_p)
{
    if (Context_p->HashDigestValid)
    {
        c_memcpy(Digest_p, Context_p->Hash, SBHYBRID_DIGEST_BYTES);

        if (Context_p->HashTempAssetId != 0)
        {
            SB_Result_t res;

            res = SBHYBRID_AssetDelete_SendCommand(Context_p,
                                                   Context_p->HashTempAssetId);
            if (res == SBHYBRID_PENDING)
            {
                while (!EIP130_MailboxCanReadToken(Context_p->Device_EIP130,
                                                   SBLIB_CFG_XM_MAILBOXNR))
                {
                    // Wait until it is possible to read the token
                }

                // Read response token after token for asset delete
                // Note assume it went OK
                (void)SBHYBRID_AssetLoad_ReadResult(Context_p);
            }
            Context_p->HashTempAssetId = 0;
        }
    }
}


#endif /* SBHYBRID_WITH_EIP130 */

/* end of file sbhybrid_eip130_hash.c */
