/* sbhybrid_eip130_aes_if.c
 *
 * Description: Implementation of SB_AES_API/AES_IF.
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

/* This source file is intended for EIP130 only. */
#ifdef SBHYBRID_WITH_EIP130
#include "c_lib.h"
#include "aes_if.h"
#include "sbcr_key.h"
#include "cfg_sbif.h"

#include "eip130.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "device_swap.h"
#include "eip130_token_nop.h"
#include "eip130_token_common.h"
#include "eip130_token_asset.h"
#include "eip130_token_crypto.h"

// Enable logging for key data and image data when debugging is enabled
#ifdef CFG_ENABLE_DEBUG
//#define ENABLE_KEY_INFO_LOGGING
//#define ENABLE_DATA_INFO_LOGGING
#if defined(ENABLE_DATA_INFO_LOGGING) || defined(ENABLE_KEY_INFO_LOGGING)
#define LOG_SEVERITY_MAX LOG_SEVERITY_INFO
#include "log.h"
#endif
#endif

// Select the correct index in OTP of the unwrap or derive key.
#ifdef SBIF_CFG_DERIVE_WRAPKEY_FROM_KDK
#if SBIF_CFG_CONFIDENTIALITY_BITS == 128
#define SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_DERIVE_KEY_128
#else
#define SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_DERIVE_KEY_256
#endif
#else
#if SBIF_CFG_CONFIDENTIALITY_BITS == 128
#define SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY_128
#else
#define SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY_256
#endif
#endif

// Define AES IV and block size.
#define SBHYBRID_AES_IV_BYTES 16

// Asset Search functionality, if using Secure Boot key from OTP.
#ifndef SBLIB_CF_REMOVE_IMAGE_TYPE_W
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
#ifndef SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY
#error "SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY is not defined."
#endif
#endif
#endif

#ifdef SBLIB_CFG_SM_SECURE_ACCESS
#define SBHYBRID_ACCESS_POLICY  0
#else
#define SBHYBRID_ACCESS_POLICY  EIP130TOKEN_ASSET_POLICY_SOURCE_NON_SECURE
#endif

#define SBHYBRID_AESCBCKEY_POLICY (SBHYBRID_ACCESS_POLICY                   | \
                                   EIP130TOKEN_ASSET_POLICY_ALGO_CIPHER_AES | \
                                   EIP130TOKEN_ASSET_POLICY_MODE2           | \
                                   EIP130TOKEN_ASSET_POLICY_DECRYPT)

#define SBHYBRID_AESWRAPKEY_POLICY (SBHYBRID_ACCESS_POLICY            | \
                                    EIP130TOKEN_ASSET_POLICY_AES_WRAP | \
                                    EIP130TOKEN_ASSET_POLICY_DECRYPT)

#ifdef ENABLE_NOT_USED_FUNCTION
/*----------------------------------------------------------------------------
 * SBHYBRID_CryptoECB_SendCommand
 *
 * This function requests the SM to perform a AES-ECB en/decrypt operation.
 */
static SB_Result_t
SBHYBRID_CryptoECB_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                               const void * DataIn_p,
                               void * DataOut_p,
                               uint32_t Size,
                               bool EncryptFlag,
                               uint32_t AssetId)
{
    Eip130Token_Command_t t_cmd;
    uint64_t InputDataAddr;
    uint64_t OutputDataAddr;

    L_TRACE(LF_SBHYBRID, "EIP130 WriteToken ECB Encrypt.");

    // Get the DMA address of the data
    InputDataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                         DataIn_p, Size);
    if (InputDataAddr == 0)
    {
        return SB_ERROR_HARDWARE;
    }
    OutputDataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                          DataOut_p, Size);
    if (OutputDataAddr == 0)
    {
        return SB_ERROR_HARDWARE;
    }

    // Create and submit command token
    Eip130Token_Command_Crypto_Operation(&t_cmd,
                                         EIP130TOKEN_CRYPTO_ALGO_AES,
                                         EIP130TOKEN_CRYPTO_MODE_ECB,
                                         EncryptFlag,
                                         Size);
    Eip130Token_Command_Crypto_SetDataAddresses(&t_cmd,
                                                InputDataAddr, Size,
                                                OutputDataAddr, Size);
    Eip130Token_Command_Crypto_SetKeyLength(&t_cmd,
                                            (SBIF_CFG_CONFIDENTIALITY_BITS / 8));
    Eip130Token_Command_Crypto_SetASLoadKey(&t_cmd, AssetId);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}

/*----------------------------------------------------------------------------
 * SBHYBRID_CryptoECB_ReadResult
 */
static SB_Result_t
SBHYBRID_CryptoECB_ReadResult(SBHYBRID_SymmContext_t * const Context_p)
{
    Eip130Token_Result_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR);

    SBHYBRID_EIP130_Common_DmaRelease(Context_p);

    return sbres;
}
#endif


/*----------------------------------------------------------------------------
 * SBHYBRID_CryptoCBC_SendCommand
 *
 * This function requests the SM to perform a AES-CBC en/decrypt operation.
 */
static SB_Result_t
SBHYBRID_CryptoCBC_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                               const void * DataIn_p,
                               void * DataOut_p,
                               uint32_t Size,
                               uint32_t * IV_p,
                               bool EncryptFlag,
                               uint32_t AssetId)
{
    Eip130Token_Command_t t_cmd;
    uint64_t InputDataAddr;
    uint64_t OutputDataAddr;

    L_TRACE(LF_SBHYBRID, "EIP130 WriteToken CBC Encrypt.");

    // Get the DMA address of the data
    InputDataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                         DataIn_p, Size);
    if (InputDataAddr == 0)
    {
        return SB_ERROR_HARDWARE;
    }
    OutputDataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                          DataOut_p, Size);
    if (OutputDataAddr == 0)
    {
        return SB_ERROR_HARDWARE;
    }

    // Create and submit command token
    Eip130Token_Command_Crypto_Operation(&t_cmd,
                                         EIP130TOKEN_CRYPTO_ALGO_AES,
                                         EIP130TOKEN_CRYPTO_MODE_CBC,
                                         EncryptFlag,
                                         Size);
    Eip130Token_Command_Crypto_SetDataAddresses(&t_cmd,
                                                InputDataAddr, Size,
                                                OutputDataAddr, Size);
    Eip130Token_Command_Crypto_SetKeyLength(&t_cmd,
                                            (SBIF_CFG_CONFIDENTIALITY_BITS / 8));
    Eip130Token_Command_Crypto_SetASLoadKey(&t_cmd, AssetId);
    Eip130Token_Command_Crypto_CopyIV(&t_cmd, (const uint8_t *)IV_p);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}

/*----------------------------------------------------------------------------
 * SBHYBRID_LongCrypto_ReadResult
 */
SB_Result_t
SBHYBRID_CryptoCBC_ReadResult(SBHYBRID_SymmContext_t * const Context_p,
                              uint32_t * IV_p)
{
    Eip130Token_Result_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR);
    if (sbres == SB_SUCCESS)
    {
        Eip130Token_Result_Crypto_CopyIV(&t_res, (uint8_t *)IV_p);
    }

    SBHYBRID_EIP130_Common_DmaRelease(Context_p);

    return sbres;
}


#define NOT_IMPLEMENTED PANIC("NOT IMPLEMENTED: %s", __func__);


/* This file requires SBIF_CFG_CONFIDENTIALITY_BITS has been predefined. */

#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
#if (SBIF_CFG_CONFIDENTIALITY_BITS != 128) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 192) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 256)
#error "SBIF_CFG_CONFIDENTIALITY_BITS must be one of 128, 192, or 256."
#endif
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */

/* Features of this implementation:
   + This implementation is only suitable for Secure Boot processing.
   + Not for signing. */
#define AES_IF_OMIT_KEYGEN /* Always omit key generation. */
#define AES_IF_OMIT_CBC_ENCRYPT /* CBC only supported for decryption */
#define AES_IF_OMIT_ECB_DECRYPT /* ECB only supported for encryption and
                                   unwrapping */
#define AES_IF_OMIT_WRAP        /* AES-WRAP only supports unwrap, not wrap. */

/* Generic macro for send command-wait reply-read reply */
#define SBHYBRID_EIP130_PROCESS_IN_OUT(r, ctx_p, infunc_invoke, outfunc_invoke) \
    do                                                                          \
    {                                                                           \
        SB_Result_t sbres_tmp = infunc_invoke;                                  \
        L_DEBUG(LF_SBHYBRID, "EIP130 Request: %s => %d", #infunc_invoke, sbres_tmp); \
        if (sbres_tmp == SBHYBRID_PENDING)                                      \
        {                                                                       \
            while (!EIP130_MailboxCanReadToken(ctx_p->SymmContext.Device_EIP130, SBLIB_CFG_XM_MAILBOXNR)) { /* Wait. */ } \
            sbres_tmp = outfunc_invoke;                                         \
            L_DEBUG(LF_SBHYBRID, "EIP130 Response: %s => %d", #outfunc_invoke, sbres_tmp); \
        }                                                                       \
        r = sbres_tmp; /* Return value (via macro argument) */                  \
    } while(0)

#define SBHYBRID_EIP130_PROCESS_IN1_OUT0(r, ctx_p, infunc, inp1, outfunc) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP130_PROCESS_IN2_OUT0(r, ctx_p, infunc, inp1, inp2, outfunc) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP130_PROCESS_IN3_OUT0(r, ctx_p, infunc, inp1, inp2, inp3, outfunc) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP130_PROCESS_IN4_OUT0(r, ctx_p, infunc, inp1, inp2, inp3, inp4, outfunc) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3), (inp4)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP130_PROCESS_IN5_OUT0(r, ctx_p, infunc, inp1, inp2, inp3, inp4, inp5, outfunc) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3), (inp4), (inp5)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP130_PROCESS_IN0_OUT1(r, ctx_p, infunc, outfunc, outp1) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

#define SBHYBRID_EIP130_PROCESS_IN1_OUT1(r, ctx_p, infunc, inp1, outfunc, outp1) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1)), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

#define SBHYBRID_EIP130_PROCESS_IN2_OUT1(r, ctx_p, infunc, inp1, inp2, outfunc, outp1) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2)), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

#define SBHYBRID_EIP130_PROCESS_IN6_OUT1(r, ctx_p, infunc, inp1, inp2, inp3, inp4, inp5, inp6, outfunc, outp1) \
    SBHYBRID_EIP130_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3), (inp4), (inp5), (inp6)), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

static int
AES_IF_Ctx_LoadKeyData(struct AES_IF_Ctx * Ctx_p,
                       const void * KeyData_p,
                       uint64_t Policy,
                       unsigned int KeyDataBits)
{
    SB_Result_t res = SB_SUCCESS;

    L_DEBUG(LF_SBHYBRID, " ");
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    if (KeyDataBits != SBIF_CFG_CONFIDENTIALITY_BITS)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: Key data bits is not equal to confidentiality data bits (%d)",
                KeyDataBits);
        return 0;
    }
#else
    if ((KeyDataBits != 128) &&
        (KeyDataBits != 192) &&
        (KeyDataBits != 256))
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: The wrong key is been loaded (%d)",
                KeyDataBits);
        return 0;
    }
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */

    if (Ctx_p->AssetId != 0)
    {
        if ((Ctx_p->AssetId & 0xFF03FF03) != 0x5A02A501)
        {
            SBHYBRID_EIP130_PROCESS_IN1_OUT0(res,
                                             Ctx_p,
                                             SBHYBRID_AssetDelete_SendCommand,
                                             Ctx_p->AssetId,
                                             SBHYBRID_AssetDelete_ReadResult);
        }
        Ctx_p->AssetId = 0;
    }
    if (res == SB_SUCCESS)
    {
        Ctx_p->AssetBits = KeyDataBits;
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
        /* If not using SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY, NULL
           data means "load" wrapping key. */
        if (KeyData_p == NULL)
        {
            SBHYBRID_EIP130_PROCESS_IN1_OUT1(res,
                                             Ctx_p,
                                             SBHYBRID_AssetSearch_SendCommand,
                                             SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY,
                                             SBHYBRID_AssetCreateSearch_ReadResult,
                                             &Ctx_p->AssetId);
            /* Note: Ctx_p->AssetFilled is not set to 1. */
            goto noAssetValueLoad;
        }
#endif
        SBHYBRID_EIP130_PROCESS_IN2_OUT1(res,
                                         Ctx_p,
                                         SBHYBRID_AssetCreate_SendCommand,
                                         Policy,
                                         KeyDataBits / 8,
                                         SBHYBRID_AssetCreateSearch_ReadResult,
                                         &Ctx_p->AssetId);
    }
    if (res == SB_SUCCESS)
    {
        /* Next: Load asset value. */
#ifdef ENABLE_KEY_INFO_LOGGING
        Log_HexDump("Key", 0, KeyData_p, KeyDataBits / 8);
#endif
        SBHYBRID_EIP130_PROCESS_IN3_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_AssetLoadPlaintext_SendCommand,
                                         Ctx_p->AssetId,
                                         KeyData_p,
                                         (KeyDataBits / 8),
                                         SBHYBRID_AssetLoad_ReadResult);
    }

#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
noAssetValueLoad:                       /* For searched asset, no value will be loaded. */
#endif
    if (res == SB_SUCCESS)
    {
        /* Load operation has been successful. */
        L_TRACE(LF_SBHYBRID, "Successful key load operation");
        return 1;
    }

    /* Failed */
    L_DEBUG(LF_SBHYBRID, "ERROR: Key load operation failed");
    return 0;
}

/* This implementation of aes_if.h uses packet engine.
   When a Secure Boot Confidentiality Root Key (for AES-Wrap) is needed,
   the implementation uses key material defined in sbcr_key.c.
   The default key in sbcr_key.c SHALL be customized.
*/
#define AES_IF_SB_RESULT(x) \
     ((x)==(SB_SUCCESS)?AES_IF_RESULT_SUCCESS:AES_IF_RESULT_GENERIC_ERROR)

uint32_t
AES_IF_Ctx_GetSize(void)
{
    return sizeof(struct AES_IF_Ctx);
}

AES_IF_ResultCode_t
AES_IF_Ctx_Init(AES_IF_Ctx_Ptr_t Ctx_p)
{
    L_TRACE(LF_SBHYBRID, "Initializing EIP130 AES_IF implementation");

    c_memset(Ctx_p, 0, sizeof(struct AES_IF_Ctx));
    return AES_IF_SB_RESULT(SBHYBRID_Initialize_HW(Ctx_p));
}

void
AES_IF_Ctx_Uninit(AES_IF_Ctx_Ptr_t Ctx_p)
{
    struct AES_IF_Ctx * pContext = (struct AES_IF_Ctx *)Ctx_p;

    L_TRACE(LF_SBHYBRID, "Uninitializing EIP130 AES_IF implementation");

    if ((pContext->AssetId != 0) && ((pContext->AssetId & 0xFF03FF03) != 0x5A02A501))
    {
        SB_Result_t res;

        SBHYBRID_EIP130_PROCESS_IN1_OUT0(res,
                                         pContext,
                                         SBHYBRID_AssetDelete_SendCommand,
                                         pContext->AssetId,
                                         SBHYBRID_AssetDelete_ReadResult);
        PARAMETER_NOT_USED(res);
    }
    SBHYBRID_Uninitialize_HW(Ctx_p);
}

AES_IF_ResultCode_t
AES_IF_Ctx_GetError(AES_IF_Ctx_Ptr_t Ctx_p)
{
    AES_IF_ResultCode_t result = Ctx_p->Result;
    return result;
}

void
AES_IF_Ctx_LoadKey(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const Key_p,
                   const uint32_t KeyLength)
{
    if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                               Key_p,
                               SBHYBRID_AESCBCKEY_POLICY,
                               KeyLength) == 0)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}


void
AES_IF_Ctx_LoadIV(AES_IF_Ctx_Ptr_t Ctx_p,
                  const uint32_t IV[4])
{
    c_memcpy(Ctx_p->IV, IV, 16);
}

void
AES_IF_Ctx_SetKEK(AES_IF_Ctx_Ptr_t Ctx_p,
                  const void * const KEKIdentification,
                  const uint32_t KEKIdentificationSize)
{
    if (KEKIdentificationSize == 0)
    {
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
        /* Search for asset (SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY) and
           use it for AES Key Unwrap. */
        if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                                   NULL,
                                   SBHYBRID_AESWRAPKEY_POLICY,
                                   SBIF_CFG_CONFIDENTIALITY_BITS) == 0)
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
            return;
        }
#else /* !SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */

        /* Load Key from SBCommon_SBCRK. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT((SBIF_CFG_CONFIDENTIALITY_BITS / 8) == sizeof(SBCommon_SBCRK));
#else
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT((sizeof(SBCommon_SBCRK) == 16) ||
                              (sizeof(SBCommon_SBCRK) == 24) ||
                              (sizeof(SBCommon_SBCRK) == 32));
#endif

        /* Load Key from SBCommon_SBCRK. */
        if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                                   SBCommon_SBCRK,
                                   SBHYBRID_AESWRAPKEY_POLICY,
                                   (sizeof(SBCommon_SBCRK) * 8)) == 0)
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
            return;
        }
#endif /* SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */
    }
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    /* Allow one of (16, 24, or 32). */
    else if (KEKIdentificationSize == (SBIF_CFG_CONFIDENTIALITY_BITS / 8))
#else
    /* Allow any size (16, 24, or 32) */
    else if ((KEKIdentificationSize == 16) ||
             (KEKIdentificationSize == 24) ||
             (KEKIdentificationSize == 32))
#endif
    {
        /* Assume KEK provided is the key to use as raw data. */
        if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                                   KEKIdentification,
                                   SBHYBRID_AESWRAPKEY_POLICY,
                                   (KEKIdentificationSize * 8)) == 0)
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
            return;
        }
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

#ifdef SBIF_CFG_DERIVE_WRAPKEY_FROM_KDK
void
AES_IF_Ctx_DeriveKEK(AES_IF_Ctx_Ptr_t Ctx_p,
                     const void * const KDKIdentification_p,
                     const uint32_t KDKIdentificationSize,
                     const uint8_t * const DeriveInfo_p,
                     const uint32_t DeriveInfoSize)
{
    uint32_t NewAssetId = 0;
    SB_Result_t res = SB_SUCCESS;

    /* First load the Key Decryption Key. */
    if (KDKIdentificationSize == 0)
    {
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
        /* Search for asset (SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY) and
           use it for AES Key Unwrap. */
        if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                                   NULL,
                                   EIP130TOKEN_ASSET_POLICY_TRUSTED_KEY_DERIVE,
                                   SBIF_CFG_CONFIDENTIALITY_BITS) == 0)
        {
            res = SB_ERROR_HARDWARE;
        }
#else /* !SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */
        /* Ensure SBCommon_SBCRK has correct size.
           Note: An unusable size will result in a compile time error. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
        COMPILE_STATIC_ASSERT((SBIF_CFG_CONFIDENTIALITY_BITS / 8) == sizeof(SBCommon_SBCRK));
#else
        COMPILE_STATIC_ASSERT((sizeof(SBCommon_SBCRK) == 16) ||
                              (sizeof(SBCommon_SBCRK) == 24) ||
                              (sizeof(SBCommon_SBCRK) == 32));
#endif

        /* Load Key from SBCommon_SBCRK. */
        if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                                   SBCommon_SBCRK,
                                   EIP130TOKEN_ASSET_POLICY_TRUSTED_KEY_DERIVE,
                                   (sizeof(SBCommon_SBCRK) * 8)) == 0)
        {
            res = SB_ERROR_HARDWARE;
        }
#endif /* SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */
    }
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    /* Allow one of (16, 24, or 32). */
    else if (KDKIdentificationSize == (SBIF_CFG_CONFIDENTIALITY_BITS / 8))
#else
    /* Allow any size (16, 24, or 32) */
    else if ((KDKIdentificationSize == 16) ||
             (KDKIdentificationSize == 24) ||
             (KDKIdentificationSize == 32))
#endif
    {
        /* Assume provided KDK is the key to use as raw data. */
        if (AES_IF_Ctx_LoadKeyData(Ctx_p,
                                   KDKIdentification_p,
                                   EIP130TOKEN_ASSET_POLICY_TRUSTED_KEY_DERIVE,
                                   (KDKIdentificationSize * 8)) == 0)
        {
            res = SB_ERROR_HARDWARE;
        }
    }
    else
    {
        res = SB_ERROR_HARDWARE;
    }

    if (res == SB_SUCCESS)
    {
        /* Create asset for the AES unwarp key */
        L_TRACE(LF_SBHYBRID, "Create AES unwarp key asset");
        SBHYBRID_EIP130_PROCESS_IN2_OUT1(res,
                                         Ctx_p,
                                         SBHYBRID_AssetCreate_SendCommand,
                                         SBHYBRID_AESWRAPKEY_POLICY,
                                         (SBIF_CFG_CONFIDENTIALITY_BITS / 8),
                                         SBHYBRID_AssetCreateSearch_ReadResult,
                                         &NewAssetId);
    }

    if (res == SB_SUCCESS)
    {
        /* Call key derive function */
        L_TRACE(LF_SBHYBRID, "Load AES unwarp key asset");
        SBHYBRID_EIP130_PROCESS_IN4_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_AssetLoadDerive_SendCommand,
                                         NewAssetId,
                                         Ctx_p->AssetId,
                                         DeriveInfo_p,
                                         DeriveInfoSize,
                                         SBHYBRID_AssetLoad_ReadResult);
    }

    if ((res == SB_SUCCESS) && (Ctx_p->AssetId != 0))
    {
        /* Delete KDK asset */
        if ((Ctx_p->AssetId & 0xFF03FF03) != 0x5A02A501)
        {
            L_TRACE(LF_SBHYBRID, "Delete KDK asset");
            SBHYBRID_EIP130_PROCESS_IN1_OUT0(res,
                                             Ctx_p,
                                             SBHYBRID_AssetDelete_SendCommand,
                                             Ctx_p->AssetId,
                                             SBHYBRID_AssetDelete_ReadResult);
        }
        Ctx_p->AssetId = 0;
    }

    if (res == SB_SUCCESS)
    {
        /* Set AES unwrapped key as Asset to use */
        Ctx_p->AssetId = NewAssetId;
    }
    else
    {
        if (NewAssetId != 0)
        {
            SBHYBRID_EIP130_PROCESS_IN1_OUT0(res,
                                             Ctx_p,
                                             SBHYBRID_AssetDelete_SendCommand,
                                             NewAssetId,
                                             SBHYBRID_AssetDelete_ReadResult);
        }
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}
#endif

void
AES_IF_Ctx_LoadWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                          const void * const Wrap_p,
                          const uint32_t WrapLength)
{
    uint32_t NewAssetId = 0;
    SB_Result_t res = SB_SUCCESS;

    IDENTIFIER_NOT_USED(WrapLength);

    /* Create asset for the image decrypt key (AES-CBC) */
    L_TRACE(LF_SBHYBRID, "Create image decrypt key Asset");
    SBHYBRID_EIP130_PROCESS_IN2_OUT1(res,
                                     Ctx_p,
                                     SBHYBRID_AssetCreate_SendCommand,
                                     SBHYBRID_AESCBCKEY_POLICY,
                                     (SBIF_CFG_CONFIDENTIALITY_BITS / 8),
                                     SBHYBRID_AssetCreateSearch_ReadResult,
                                     &NewAssetId);
    if (res == SB_SUCCESS)
    {
        // Call the asset load through AES unwrap
        L_TRACE(LF_SBHYBRID, "Load image decrypt key Asset");
#ifdef ENABLE_KEY_INFO_LOGGING
        Log_HexDump("WrappedKey", 0, Wrap_p, ((SBIF_CFG_CONFIDENTIALITY_BITS / 8) + 8));
#endif
        SBHYBRID_EIP130_PROCESS_IN4_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_AssetLoadAESUnwrap_SendCommand,
                                         NewAssetId,
                                         Ctx_p->AssetId,
                                         Wrap_p,
                                         ((SBIF_CFG_CONFIDENTIALITY_BITS / 8) + 8),
                                         SBHYBRID_AssetLoad_ReadResult);
    }

    if ((res == SB_SUCCESS) && (Ctx_p->AssetId != 0))
    {
        /* Delete KEK asset */
        if ((Ctx_p->AssetId & 0xFF03FF03) != 0x5A02A501)
        {
            L_TRACE(LF_SBHYBRID, "Delete KEK asset");
            SBHYBRID_EIP130_PROCESS_IN1_OUT0(res,
                                             Ctx_p,
                                             SBHYBRID_AssetDelete_SendCommand,
                                             Ctx_p->AssetId,
                                             SBHYBRID_AssetDelete_ReadResult);
        }
        Ctx_p->AssetId = 0;
    }

    if (res == SB_SUCCESS)
    {
        /* Set image decrypt key as Asset to use */
        Ctx_p->AssetId = NewAssetId;
    }
    else
    {
        if (NewAssetId != 0)
        {
            SBHYBRID_EIP130_PROCESS_IN1_OUT0(res,
                                             Ctx_p,
                                             SBHYBRID_AssetDelete_SendCommand,
                                             NewAssetId,
                                             SBHYBRID_AssetDelete_ReadResult);
        }
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}


#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_Ctx_GenerateKey(AES_IF_Ctx_Ptr_t Ctx_p,
                       void * const Key_p,
                       const uint32_t KeyLength)
{
#ifdef AES_IF_OMIT_KEYGEN
    PARAMETER_NOT_USED(Ctx_p);
    PARAMETER_NOT_USED(Key_p);
    PARAMETER_NOT_USED(KeyLength);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP130_AES_IF: Key generation cannot be enabled."
#endif
}
#endif

#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_Ctx_GenerateWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                              void * const Wrap_p,
                              const uint32_t WrapLength)
{
#ifdef AES_IF_OMIT_KEYGEN
    PARAMETER_NOT_USED(Ctx_p);
    PARAMETER_NOT_USED(Wrap_p);
    PARAMETER_NOT_USED(WrapLength);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP130_AES_IF: Key generation / wrapping cannot be enabled."
#endif
}
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_ECB_EncryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p)
{
    SB_Result_t res;

#ifdef ENABLE_DATA_INFO_LOGGING
    Log_FormattedMessage("AES_ECB_Encrypt\n");
    Log_HexDump("DataIn", 0, DataIn_p, SBHYBRID_AES_IV_BYTES);
#endif
    SBHYBRID_EIP130_PROCESS_IN5_OUT0(res,
                                     Ctx_p,
                                     SBHYBRID_CryptoECB_SendCommand,
                                     DataIn_p,
                                     DataOut_p,
                                     SBHYBRID_AES_IV_BYTES,
                                     true,
                                     Ctx_p->AssetId,
                                     SBHYBRID_CryptoECB_ReadResult);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
#ifdef ENABLE_DATA_INFO_LOGGING
    else
    {
        Log_HexDump("DataOut", 0, DataOut_p, SBHYBRID_AES_IV_BYTES);
    }
#endif
}
#endif
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_ECB_DecryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p)
{
#ifdef AES_IF_OMIT_ECB_DECRYPT
    PARAMETER_NOT_USED(Ctx_p);
    PARAMETER_NOT_USED(DataIn_p);
    PARAMETER_NOT_USED(DataOut_p);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP130_AES_IF: ECB Decryption cannot be enabled."
#endif
}
#endif
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void AES_IF_CBC_Encrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p,
                        const uint32_t Size)
{
#ifdef AES_IF_OMIT_CBC_ENCRYPT
    PARAMETER_NOT_USED(Ctx_p);
    PARAMETER_NOT_USED(DataIn_p);
    PARAMETER_NOT_USED(DataOut_p);
    PARAMETER_NOT_USED(Size);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP130_AES_IF: CBC Encryption cannot be enabled."
#endif
}
#endif
#endif


/* Note: This implementation of AES_IF_CBC_Decrypt requires
   that both DataIn_p and DataOut_p are DMA-able memory. */
void
AES_IF_CBC_Decrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const DataIn_p,
                   void * const DataOut_p,
                   const uint32_t Size)
{
    SB_Result_t res;

#ifdef ENABLE_DATA_INFO_LOGGING
    Log_FormattedMessage("AES_CBC_Decrypt\n");
    Log_HexDump("DataIn", 0, DataIn_p, Size);
    Log_HexDump("IV", 0, (const uint8_t *)Ctx_p->IV, 16);
#endif
    SBHYBRID_EIP130_PROCESS_IN6_OUT1(res,
                                     Ctx_p,
                                     SBHYBRID_CryptoCBC_SendCommand,
                                     DataIn_p,
                                     DataOut_p,
                                     Size,
                                     Ctx_p->IV,
                                     false,
                                     Ctx_p->AssetId,
                                     SBHYBRID_CryptoCBC_ReadResult,
                                     Ctx_p->IV);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
#ifdef ENABLE_DATA_INFO_LOGGING
    else
    {
        Log_HexDump("DataOut", 0, DataOut_p, Size);
        Log_HexDump("IV", 0, (const uint8_t *)Ctx_p->IV, 16);
    }
#endif
}

#endif /* SBHYBRID_WITH_EIP130 */

/* end of file sbhybrid_eip130_aes_if.c */
