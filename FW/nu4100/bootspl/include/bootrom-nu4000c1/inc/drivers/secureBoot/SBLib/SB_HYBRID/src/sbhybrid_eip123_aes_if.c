/* sbhybrid_eip123_aes_if.c
 *
 * Description: Implementation of SB_AES_API/AES_IF.
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

#include "sbhybrid_internal.h"

/* This source file is intended for EIP123 only. */
#ifdef SBHYBRID_WITH_EIP123
#include "c_lib.h"
#include "aes_if.h"
#include "sbcr_key.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "device_swap.h"

#include "eip123.h"
#include "cm_tokens_asset.h"
#include "cm_tokens_common.h"
#include "cm_tokens_misc.h"
#include "cm_tokens_crypto.h"
#include "cfg_sbif.h"

// Select the correct index in NVM of the unwrap or derive key.
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


/* Define AES IV and block size. */
#define SBHYBRID_AES_IV_BYTES 16

/* Token identifier value used on EIP-123 tokens. */
#define SBHYBRID_TOKENID_VALUE     0x53CA

/* Encryption and decryption uses assets. */
#if !defined(SBLIB_CF_REMOVE_IMAGE_TYPE_E) || \
    !defined(SBLIB_CF_REMOVE_IMAGE_TYPE_W)
#define SBHYBRID_ENABLE_ASSETCREATE
#define SBHYBRID_ENABLE_ASSETDELETE
#endif

/* Asset Search functionality, if using Secure Boot key from NVM. */
#ifndef SBLIB_CF_REMOVE_IMAGE_TYPE_W
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
#define SBHYBRID_ENABLE_ASSETSEARCH
#define SBHYBRID_NO_SBCR

#ifndef SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY
#error "SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY is not defined."
#endif
#endif
#endif


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetCreate_SendCommand
 */
#ifdef SBHYBRID_ENABLE_ASSETCREATE
SB_Result_t
SBHYBRID_AssetCreate_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                                 uint32_t Policy,
                                 uint32_t KeyBytes)
{
    CMTokens_Command_t t_cmd;
    int res;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken create asset.");

    CMTokens_MakeCommand_AssetCreate(&t_cmd, Policy, KeyBytes);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to send AssetCreate command. res = %d.",
                res);

        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}
#endif /* SBHYBRID_ENABLE_ASSETCREATE */


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetSearch_SendCommand
 */
#ifdef SBHYBRID_ENABLE_ASSETSEARCH
SB_Result_t
SBHYBRID_AssetSearch_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p)
{
    CMTokens_Command_t t_cmd;
    int res;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken search asset.");

    CMTokens_MakeCommand_AssetSearch(&t_cmd,
                                     /*Index:*/SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to send AssetSearch command. res = %d.",
                res);

        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}
#endif /* SBHYBRID_ENABLE_ASSETSEARCH */


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetCreateSearch_ReadResult
 */
#if defined(SBHYBRID_ENABLE_ASSETCREATE) || defined(SBHYBRID_ENABLE_ASSETSEARCH)
SB_Result_t
SBHYBRID_AssetCreateSearch_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p,
                                      uint32_t * AssetId_p)
{
    CMTokens_Response_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR,
                                                      "AssetCreate");
    if (sbres != SB_SUCCESS)
    {
        return sbres;
    }

    // get AS_ID from response message
    CMTokens_ParseResponse_AssetCreate(&t_res, AssetId_p);

    L_TRACE(LF_SBHYBRID, "AssetID = 0x%x", *AssetId_p);

    return SB_SUCCESS;
}
#endif /* SBHYBRID_ENABLE_ASSETCREATE || SCBCM_ENABLE_ASSETSEARCH */


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetDelete_SendCommand
 */
#ifdef SBHYBRID_ENABLE_ASSETDELETE
SB_Result_t
SBHYBRID_AssetDelete_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                                 const uint32_t AssetId)
{
    CMTokens_Command_t t_cmd;
    int res;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken delete asset (0x%x).", AssetId);

    CMTokens_MakeCommand_AssetDelete(&t_cmd, AssetId);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to send AssetDelete command. "
                "res = %d.",
                res);

        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}
#endif /* SBHYBRID_ENABLE_ASSETDELETE */


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetDelete_ReadResult
 */
#ifdef SBHYBRID_ENABLE_ASSETDELETE
SB_Result_t
SBHYBRID_AssetDelete_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p)
{
    CMTokens_Response_t t_res;

    return SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                     &t_res,
                                                     SBLIB_CFG_XM_MAILBOXNR,
                                                     "AssetDelete");
}
#endif /* SBHYBRID_ENABLE_ASSETDELETE */

/*----------------------------------------------------------------------------
 * SBHYBRID_LoadKey_SendCommand
 *
 * This function loads the key into the CM for use with AES.
 * The key can be a plain key or a HUK-wrapped key.
 */
SB_Result_t
SBHYBRID_LoadKey_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                             const uint32_t AssetId,
                             const uint8_t * Data_p,
                             const uint16_t DataLen)
{
    CMTokens_Command_t t_cmd;
    int res;
    EIP123_Fragment_t Frag;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken load plaintext asset.");

    CMTokens_MakeCommand_AssetLoad_Plaintext(&t_cmd, AssetId, DataLen);

    // copy the key data into the small DMA buffer
    c_memcpy(ImageProcess_p->SmallDMABuf.Host_p, Data_p, DataLen);

    Frag.StartAddress = ImageProcess_p->SmallDMABuf.PhysAddr;
    Frag.Length = DataLen;

    DMAResource_PreDMA(ImageProcess_p->DC_CipherIn.DMAHandle, 0, 0);
    if (EIP123_DescriptorChain_Populate(&ImageProcess_p->DC_CipherIn.DC,
                                        ImageProcess_p->DC_CipherIn.DMAHandle,
                                        ImageProcess_p->DC_CipherIn.PhysAddr,
                                        /*fIsInput:*/true,
                                        /*FragmentCount:*/1,
                                        &Frag,
                                        /*AlgorithmicBlockSize:*/4,
                                        /*TokenIDPhysAddr:*/0) // not supported
        != EIP123_STATUS_SUCCESS)
    {
        return SB_ERROR_HARDWARE;
    }

    CMTokens_MakeCommand_AssetLoad_WriteInDescriptor(&t_cmd,
                                                     &ImageProcess_p->DC_CipherIn.DC);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to send LoadKey command. res = %d.",
                res);
        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_LoadKey_ReadResult
 */
SB_Result_t
SBHYBRID_LoadKey_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p)
{
    CMTokens_Response_t t_res;

    return SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                     &t_res,
                                                     SBLIB_CFG_XM_MAILBOXNR,
                                                     "LoadKey");
}


/*----------------------------------------------------------------------------
 * SBHYBRID_AESUnwrap_SendCommand
 *
 * This function loads the key into the CM for use with AES.
 * The key can be a plain key or a HUK-wrapped key.
 */
SB_Result_t
SBHYBRID_AESUnwrap_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                               const uint32_t TargetAssetId,
                               const uint32_t KEKAssetId,
                               const uint8_t * Data_p,
                               const uint16_t DataLen)
{
    CMTokens_Command_t t_cmd;
    int res;
    EIP123_Fragment_t Frag;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken load plaintext asset.");

    CMTokens_MakeCommand_AssetLoad_AesUnwrap(&t_cmd,
                                             TargetAssetId,
                                             KEKAssetId,
                                             DataLen);

    // copy the key data into the small DMA buffer
    c_memcpy(ImageProcess_p->SmallDMABuf.Host_p, Data_p, DataLen);

    Frag.StartAddress = ImageProcess_p->SmallDMABuf.PhysAddr;
    Frag.Length = DataLen;

    DMAResource_PreDMA(ImageProcess_p->DC_CipherIn.DMAHandle, 0, 0);
    if (EIP123_DescriptorChain_Populate(&ImageProcess_p->DC_CipherIn.DC,
                                        ImageProcess_p->DC_CipherIn.DMAHandle,
                                        ImageProcess_p->DC_CipherIn.PhysAddr,
                                        /*fIsInput:*/true,
                                        /*FragmentCount:*/1,
                                        &Frag,
                                        /*AlgorithmicBlockSize:*/4,
                                        /*TokenIDPhysAddr:*/0) // not supported
        != EIP123_STATUS_SUCCESS)
    {
        return SB_ERROR_HARDWARE;
    }

    CMTokens_MakeCommand_AssetLoad_WriteInDescriptor(&t_cmd,
                                                     &ImageProcess_p->DC_CipherIn.DC);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to send AESUnwrap command. res = %d.",
                res);
        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_AESUnwrap_ReadResult
 */
SB_Result_t
SBHYBRID_AESUnwrap_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p)
{
    CMTokens_Response_t t_res;

    return SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                     &t_res,
                                                     SBLIB_CFG_XM_MAILBOXNR,
                                                     "AESUnwrap");
}



#ifdef SBIF_CFG_DERIVE_WRAPKEY_FROM_KDK
/*----------------------------------------------------------------------------
 * SBHYBRID_Derive_SendCommand
 *
 * This function loads the key into the CM for use with AES.
 * The key can be a plain key or a HUK-wrapped key.
 */
SB_Result_t
SBHYBRID_Derive_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                            const uint32_t TargetAssetId,
                            const uint32_t KDKAssetId,
                            const uint8_t * Data_p,
                            const uint16_t DataLen)
{
    CMTokens_Command_t t_cmd;
    int res;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken load plaintext asset.");

    CMTokens_MakeCommand_AssetLoad_Derive(&t_cmd,
                                          TargetAssetId,
                                          KDKAssetId,
                                          0);

    CMTokens_MakeCommand_AssetLoad_SetAad(&t_cmd, Data_p, DataLen);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to send Derive command. res = %d.",
                res);
        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_Derive_ReadResult
 */
SB_Result_t
SBHYBRID_Derive_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p)
{
    CMTokens_Response_t t_res;

    return SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                     &t_res,
                                                     SBLIB_CFG_XM_MAILBOXNR,
                                                     "Derive");
}
#endif


#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
/*----------------------------------------------------------------------------
 * SBHYBRID_Crypto_SendCommand
 *
 * This function requests the CM to perform a single-block AES-ECB encrypt the
 * image length to get the initial IV to use with Decryption of the image.
 */
SB_Result_t
SBHYBRID_Crypto_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                            const void * DataSingleBlock_p,
                            bool EncryptFlag,
                            uint32_t AssetId)
{
    CMTokens_Command_t t_cmd;
    int res;
    uint32_t TokenID_PhysAddr;
    EIP123_Fragment_t Frag;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken Encrypt to crypt a block.");

    // write the ImageLen to the IV buffer as a big-endian number
    // also zero the WriteTokenID field
    c_memset(ImageProcess_p->SmallDMABuf.Host_p,
             0,
             16 /* AES BLOCK */ + 4/*WriteTokenID field:*/);

    c_memcpy(ImageProcess_p->SmallDMABuf.Host_p,
             DataSingleBlock_p,
             16);                       /* AES BLOCK */

    Frag.StartAddress = ImageProcess_p->SmallDMABuf.PhysAddr;
    Frag.Length = 16;

    TokenID_PhysAddr = Frag.StartAddress + 16;

    DMAResource_PreDMA(ImageProcess_p->DC_CipherIn.DMAHandle, 0, 0);
    if (EIP123_DescriptorChain_Populate(&ImageProcess_p->DC_CipherIn.DC,
                                        ImageProcess_p->DC_CipherIn.DMAHandle,
                                        ImageProcess_p->DC_CipherIn.PhysAddr,
                                        /*fIsInput:*/true,
                                        /*FragmentCount:*/1,
                                        &Frag,
                                        /*AlgorithmicBlockSize:*/16,
                                        /*TokenIDPhysAddr:*/0) // not supported
        != EIP123_STATUS_SUCCESS)
    {
        return SB_ERROR_HARDWARE;
    }

    if (EIP123_DescriptorChain_Populate(&ImageProcess_p->DC_CipherOut.DC,
                                        ImageProcess_p->DC_CipherOut.DMAHandle,
                                        ImageProcess_p->DC_CipherOut.PhysAddr,
                                        /*fIsInput:*/false,
                                        /*FragmentCount:*/1,
                                        &Frag,
                                        /*AlgorithmicBlockSize:*/16,
                                        /*TokenIDPhysAddr:*/TokenID_PhysAddr)
        != EIP123_STATUS_SUCCESS)
    {
        return SB_ERROR_HARDWARE;
    }

    CMTokens_MakeCommand_Crypto_AES(&t_cmd,
                                    EncryptFlag,
                                    /*Mode:*/CMTOKENS_CRYPTO_MODE_ECB,
                                    /*DataLengthInBytes:*/SBHYBRID_AES_IV_BYTES);

    // refer to the AES-key available inside the CM
    CMTokens_MakeCommand_Crypto_SetASLoadKey(&t_cmd, AssetId);

    CMTokens_MakeCommand_Crypto_AES_SetKeyLength(&t_cmd,
                                                 SBIF_CFG_CONFIDENTIALITY_BITS / 8);

    CMTokens_MakeCommand_Crypto_WriteInDescriptor(&t_cmd,
                                                  &ImageProcess_p->DC_CipherIn.DC);

    CMTokens_MakeCommand_Crypto_WriteOutDescriptor(&t_cmd,
                                                   &ImageProcess_p->DC_CipherOut.DC);

    CMTokens_MakeCommand_SetTokenID(&t_cmd,
                                    SBHYBRID_TOKENID_VALUE,
                                    /*fWriteTokenID:*/true);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to start Crypto. res = %d.",
                res);

        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}
#endif
#endif


#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
/*----------------------------------------------------------------------------
 * SBHYBRID_Crypto_ReadResult
 */
SB_Result_t
SBHYBRID_Crypto_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p)
{
    CMTokens_Response_t t_res;
    SB_Result_t sbres;
    // NOTE: Needed for DMA/memory delays
    //volatile uint32_t *tokenid_p =
    //    (volatile uint32_t *) ImageProcess_p->SmallDMABuf.Host_p;
    //tokenid_p += 4;

    sbres = SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR,
                                                      "Crypto");
    if (sbres != SB_SUCCESS)
    {
        return sbres;
    }

    // NOTE: In case of DMA/memory delays a wait for TokenID shall be inserted here

    // we leave the result of the operation in the SmallDMABuf

    DMAResource_PostDMA(ImageProcess_p->SmallDMABuf.DMAHandle, 0, 0);

    return SB_SUCCESS;
}
#endif
#endif

/*----------------------------------------------------------------------------
 * SBHYBRID_LongCrypto_SendCommand
 *
 * This function requests the CM to perform a single-block AES-ECB encrypt the
 * image length to get the initial IV to use with Decryption of the image.
 */
SB_Result_t
SBHYBRID_LongCrypto_SendCommand(SBHYBRID_SymmContext_t * const ImageProcess_p,
                                const void * DataIn_p,
                                void * DataOut_p,
                                uint32_t Size,
                                uint32_t * IV,
                                bool EncryptFlag,
                                uint32_t AssetId)
{
    CMTokens_Command_t t_cmd;
    int res;
    uint32_t TokenID_PhysAddr;
    EIP123_Fragment_t Frag;
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t InputAddrPair;
    DMAResource_AddrPair_t OutputAddrPair;
    DMAResource_Handle_t DMAHandle;

    L_TRACE(LF_SBHYBRID, "EIP123 WriteToken Encrypt to crypt a block.");

    // write the ImageLen to the IV buffer as a big-endian number
    // also zero the WriteTokenID field
    c_memset(ImageProcess_p->SmallDMABuf.Host_p,
             0,
             16 /* AES BLOCK */ + 4/*WriteTokenID field:*/);

    TokenID_PhysAddr = ImageProcess_p->SmallDMABuf.PhysAddr;
    TokenID_PhysAddr += 16;

    // get the physical address of the source buffer
    Props.Size = Size;
    if (Props.Size < 4)
    {
        Props.Size = 4;
    }
    Props.Alignment = 4;

    InputAddrPair.Domain = DMARES_DOMAIN_HOST;
    InputAddrPair.Address.Native_p = (void *)DataIn_p;

    res = DMAResource_CheckAndRegister(Props, InputAddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    DMAResource_PreDMA(DMAHandle, 0, 0);
    // remember the handle
    ImageProcess_p->DMAHandles[ImageProcess_p->DMAHandleCount++] = DMAHandle;

    res = DMAResource_Translate(DMAHandle,
                                DMARES_DOMAIN_EIP12xDMA,
                                &InputAddrPair);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    Frag.StartAddress = InputAddrPair.Address.Value32;
    Frag.Length = Size;

    DMAResource_PreDMA(ImageProcess_p->DC_CipherIn.DMAHandle, 0, 0);
    if (EIP123_DescriptorChain_Populate(&ImageProcess_p->DC_CipherIn.DC,
                                        ImageProcess_p->DC_CipherIn.DMAHandle,
                                        ImageProcess_p->DC_CipherIn.PhysAddr,
                                        /*fIsInput:*/true,
                                        /*FragmentCount:*/1,
                                        &Frag,
                                        /*AlgorithmicBlockSize*/16,
                                        /*TokenIDPhysAddr:*/0)
        != EIP123_STATUS_SUCCESS)
    {
        return SB_ERROR_HARDWARE;
    }

    OutputAddrPair.Domain = DMARES_DOMAIN_HOST;
    OutputAddrPair.Address.Native_p = (void *)DataOut_p;

    res = DMAResource_CheckAndRegister(Props, OutputAddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }


    DMAResource_PreDMA(DMAHandle, 0, 0);
    // remember the handle
    ImageProcess_p->DMAHandles[ImageProcess_p->DMAHandleCount++] = DMAHandle;

    res = DMAResource_Translate(DMAHandle,
                                DMARES_DOMAIN_EIP12xDMA,
                                &OutputAddrPair);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    Frag.StartAddress = OutputAddrPair.Address.Value32;

    if (EIP123_DescriptorChain_Populate(&ImageProcess_p->DC_CipherOut.DC,
                                        ImageProcess_p->DC_CipherOut.DMAHandle,
                                        ImageProcess_p->DC_CipherOut.PhysAddr,
                                        /*fIsInput:*/false,
                                        /*FragmentCount:*/1,
                                        &Frag,
                                        /*AlgorithmicBlockSize*/16,
                                        /*TokenIDPhysAddr:*/TokenID_PhysAddr)
        != EIP123_STATUS_SUCCESS)
    {
        return SB_ERROR_HARDWARE;
    }

    CMTokens_MakeCommand_Crypto_AES(&t_cmd,
                                    EncryptFlag,
                                    /*Mode:*/CMTOKENS_CRYPTO_MODE_CBC,
                                    /*DataLengthInBytes:*/Size);

    // refer to the AES-key available inside the CM
    CMTokens_MakeCommand_Crypto_SetASLoadKey(&t_cmd, AssetId);

    CMTokens_MakeCommand_Crypto_AES_SetKeyLength(&t_cmd,
                                                 SBIF_CFG_CONFIDENTIALITY_BITS / 8);

    CMTokens_MakeCommand_Crypto_CopyIV(&t_cmd, (const uint8_t *)IV);

    CMTokens_MakeCommand_Crypto_WriteInDescriptor(&t_cmd,
                                                  &ImageProcess_p->DC_CipherIn.DC);

    CMTokens_MakeCommand_Crypto_WriteOutDescriptor(
                                                   &t_cmd,
                                                   &ImageProcess_p->DC_CipherOut.DC);

    CMTokens_MakeCommand_SetTokenID(&t_cmd,
                                    SBHYBRID_TOKENID_VALUE,
                                    /*fWriteTokenID:*/true);

    CMTokens_MakeToken_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);

    res = EIP123_WriteAndSubmitToken(ImageProcess_p->Device_EIP123,
                                     SBLIB_CFG_XM_MAILBOXNR,
                                     &t_cmd);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "Failed to start Crypto. res = %d.",
                res);

        return SB_ERROR_HARDWARE;
    }

    return SBHYBRID_PENDING;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_LongCrypto_ReadResult
 */
SB_Result_t
SBHYBRID_LongCrypto_ReadResult(SBHYBRID_SymmContext_t * const ImageProcess_p,
                               uint32_t * IV)
{
    CMTokens_Response_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP123_Common_ReadResultAndCheck(ImageProcess_p->Device_EIP123,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR,
                                                      "Crypto");
    if (sbres != SB_SUCCESS)
    {
        return sbres;
    }

    CMTokens_ParseResponse_Crypto_CopyIV(&t_res, (uint8_t *)IV);

    // NOTE: In case of DMA/memory delays a wait for TokenID shall be inserted here

    // we leave the result of the operation in the SmallDMABuf
    DMAResource_PostDMA(ImageProcess_p->DC_CipherOut.DMAHandle, 0, 0);

    return SB_SUCCESS;
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
#define SBHYBRID_EIP123_PROCESS_IN_OUT(r, ctx_p, infunc_invoke, outfunc_invoke)\
    do                                                                  \
    {                                                                   \
        SB_Result_t sbres_tmp;                                          \
        sbres_tmp = infunc_invoke;                                      \
        L_DEBUG(LF_SBHYBRID, "Processed EIP123: %s => %d", #infunc_invoke, sbres_tmp); \
        if (sbres_tmp == SBHYBRID_PENDING)                              \
        {                                                               \
            while (!EIP123_CanReadToken(ctx_p->SymmContext.Device_EIP123, \
                                SBLIB_CFG_XM_MAILBOXNR)) { /* Wait. */ } \
            sbres_tmp = outfunc_invoke;                                 \
            L_DEBUG(LF_SBHYBRID, "Processed EIP123: %s => %d", #outfunc_invoke, sbres_tmp); \
        }                                                               \
        r = sbres_tmp; /* Return value (via macro argument) */          \
    } while(0)

#define SBHYBRID_EIP123_PROCESS_IN1_OUT0(r, ctx_p, infunc, inp1, outfunc) \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP123_PROCESS_IN3_OUT0(r, ctx_p, infunc, inp1, inp2, inp3, outfunc) \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3)), \
                                   outfunc(&(ctx_p)->SymmContext))

#define SBHYBRID_EIP123_PROCESS_IN4_OUT0(r, ctx_p, infunc, inp1, inp2, inp3, inp4, outfunc) \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3), (inp4)), \
                                   outfunc(&(ctx_p)->SymmContext))


#define SBHYBRID_EIP123_PROCESS_IN0_OUT1(r, ctx_p, infunc, outfunc, outp1)    \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

#define SBHYBRID_EIP123_PROCESS_IN1_OUT1(r, ctx_p, infunc, inp1, outfunc, outp1) \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1)), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

#define SBHYBRID_EIP123_PROCESS_IN2_OUT1(r, ctx_p, infunc, inp1, inp2, outfunc, outp1) \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2)), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))



#define SBHYBRID_EIP123_PROCESS_IN6_OUT1(r, ctx_p, infunc, inp1, inp2, inp3, inp4, inp5, inp6, outfunc, outp1) \
    SBHYBRID_EIP123_PROCESS_IN_OUT(r, (ctx_p), infunc(&(ctx_p)->SymmContext, (inp1), (inp2), (inp3), (inp4), (inp5), (inp6)), \
                                   outfunc(&(ctx_p)->SymmContext, (outp1)))

static int
AES_IF_Ctx_LoadKeyData(struct AES_IF_Ctx * Ctx_p,
                       const void * KeyData_p,
                       uint32_t Policy,
                       unsigned int KeyDataBits)
{
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    if (KeyDataBits != SBIF_CFG_CONFIDENTIALITY_BITS)
    {
        return 0;
    }
#else
    if (KeyDataBits != 128 &&
        KeyDataBits != 192 &&
        KeyDataBits != 256)
    {
        return 0;
    }
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */
    SB_Result_t res = SB_SUCCESS;
    if (Ctx_p->AssetFilled)
    {
        SBHYBRID_EIP123_PROCESS_IN1_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_AssetDelete_SendCommand,
                                         Ctx_p->AssetId,
                                         SBHYBRID_AssetDelete_ReadResult);
    }
    if (res == SB_SUCCESS)
    {
        Ctx_p->AssetBits = KeyDataBits;
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
        /* If not using SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY, NULL
           data means "load" wrapping key. */
        if (KeyData_p == NULL)
        {
            SBHYBRID_EIP123_PROCESS_IN0_OUT1(res,
                                             Ctx_p,
                                             SBHYBRID_AssetSearch_SendCommand,
                                             SBHYBRID_AssetCreateSearch_ReadResult,
                                             &Ctx_p->AssetId);
            /* Note: Ctx_p->AssetFilled is not set to 1. */
            goto noAssetValueLoad;
        }
#endif
        SBHYBRID_EIP123_PROCESS_IN2_OUT1(res,
                                         Ctx_p,
                                         SBHYBRID_AssetCreate_SendCommand,
                                         Policy, KeyDataBits / 8,
                                         SBHYBRID_AssetCreateSearch_ReadResult,
                                         &Ctx_p->AssetId);
    }
    if (res == SB_SUCCESS)
    {
        /* Next: Load asset value. */
        SBHYBRID_EIP123_PROCESS_IN3_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_LoadKey_SendCommand,
                                         Ctx_p->AssetId,
                                         KeyData_p,
                                         KeyDataBits / 8,
                                         SBHYBRID_LoadKey_ReadResult);
        Ctx_p->AssetFilled = 1;
    }

#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
noAssetValueLoad:                       /* For searched asset, no value will be loaded. */
#endif

    if (res == SB_SUCCESS)
    {
        /* Load operation has been successful. */
        return 1;
    }

    /* Failed */
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
    L_DEBUG(LF_SBHYBRID, "Initializing EIP123 AES_IF implementation");

    c_memset(Ctx_p, 0, sizeof(struct AES_IF_Ctx));
    return AES_IF_SB_RESULT(SBHYBRID_Initialize_HW(Ctx_p));
}

void
AES_IF_Ctx_Uninit(AES_IF_Ctx_Ptr_t Ctx_p)
{
    L_TRACE(LF_SBHYBRID, "Uninitializing EIP123 AES_IF implementation");

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
    if (AES_IF_Ctx_LoadKeyData(Ctx_p, Key_p,
                               (CMTOKENS_ASSET_POLICY_ALGO_CIPHER_AES |
                                CMTOKENS_ASSET_POLICY_FUNCTION_ENCRYPT |
                                CMTOKENS_ASSET_POLICY_FUNCTION_DECRYPT),
                               KeyLength))
    {
        /* Key loading successful */
    }
    else
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
        AES_IF_Ctx_LoadKey(Ctx_p, NULL, SBIF_CFG_CONFIDENTIALITY_BITS);
#else /* !SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */

        /* Load Key from SBCommon_SBCRK. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT(SBIF_CFG_CONFIDENTIALITY_BITS / 8 ==
                                  sizeof(SBCommon_SBCRK));
#else
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT(sizeof(SBCommon_SBCRK) == 16 ||
                              sizeof(SBCommon_SBCRK) == 24 ||
                              sizeof(SBCommon_SBCRK) == 32);
#endif

        /* Load Key from SBCommon_SBCRK. */
        AES_IF_Ctx_LoadKey(Ctx_p, SBCommon_SBCRK, sizeof(SBCommon_SBCRK) * 8);
#endif /* SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */
    }
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    /* Allow one of (16, 24, or 32). */
    else if (KEKIdentificationSize == SBIF_CFG_CONFIDENTIALITY_BITS / 8)
#else
    /* Allow any size (16, 24, or 32) */
    else if (KEKIdentificationSize == 16 ||
             KEKIdentificationSize == 24 ||
             KEKIdentificationSize == 32)
#endif
    {
        /* Assume KEK provided is the key to use as raw data. */
        AES_IF_Ctx_LoadKey(Ctx_p, KEKIdentification,
                           KEKIdentificationSize * 8);
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
    uint32_t KDKPolicy = CMTOKENS_ASSET_POLICY_SECURE_DERIVE;
    uint32_t KEKPolicy = CMTOKENS_ASSET_POLICY_ALGO_CIPHER_AES |
                         CMTOKENS_ASSET_POLICY_FUNCTION_ENCRYPT |
                         CMTOKENS_ASSET_POLICY_FUNCTION_DECRYPT;
    uint32_t NewAssetId;
    SB_Result_t res;

    /* First load the Key Decryption Key. */
    if (KDKIdentificationSize == 0)
    {
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
        /* Search for asset (SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY) and
           use it for AES Key Unwrap. */
        if (!AES_IF_Ctx_LoadKeyData(Ctx_p, NULL, KDKPolicy, SBIF_CFG_CONFIDENTIALITY_BITS))
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
            return;
        }
#else /* !SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */

        /* Load Key from SBCommon_SBCRK. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT(SBIF_CFG_CONFIDENTIALITY_BITS / 8 ==
                                  sizeof(SBCommon_SBCRK));
#else
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT(sizeof(SBCommon_SBCRK) == 16 ||
                              sizeof(SBCommon_SBCRK) == 24 ||
                              sizeof(SBCommon_SBCRK) == 32);
#endif

        /* Load Key from SBCommon_SBCRK. */
        if (!AES_IF_Ctx_LoadKeyData(Ctx_p, SBCommon_SBCRK, KDKPolicy, sizeof(SBCommon_SBCRK) * 8))
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
            return;
        }
#endif /* SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY */
    }
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    /* Allow one of (16, 24, or 32). */
    else if (KDKIdentificationSize == SBIF_CFG_CONFIDENTIALITY_BITS / 8)
#else
    /* Allow any size (16, 24, or 32) */
    else if (KDKIdentificationSize == 16 ||
             KDKIdentificationSize == 24 ||
             KDKIdentificationSize == 32)
#endif
    {
        /* Assume KDK provided is the key to use as raw data. */
        if (!AES_IF_Ctx_LoadKeyData(Ctx_p,
                                    KDKIdentification_p,
                                    KDKPolicy,
                                    KDKIdentificationSize * 8))
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
            return;
        }
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
        return;
    }

    /* Create additional asset for the derived key */
    SBHYBRID_EIP123_PROCESS_IN2_OUT1(res,
                                     Ctx_p,
                                     SBHYBRID_AssetCreate_SendCommand,
                                     KEKPolicy,
                                     SBIF_CFG_CONFIDENTIALITY_BITS / 8,
                                     SBHYBRID_AssetCreateSearch_ReadResult,
                                     &NewAssetId);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
        return;
    }

    /* Call key derive function */
    SBHYBRID_EIP123_PROCESS_IN4_OUT0(res,
                                     Ctx_p,
                                     SBHYBRID_Derive_SendCommand,
                                     NewAssetId,
                                     Ctx_p->AssetId,
                                     DeriveInfo_p,
                                     DeriveInfoSize,
                                     SBHYBRID_Derive_ReadResult);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
        return;
    }

    /* Delete asset corresponding to original KDK, replace it with
       unwrapped key */
    if (Ctx_p->AssetFilled)
    {
        SBHYBRID_EIP123_PROCESS_IN1_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_AssetDelete_SendCommand,
                                         Ctx_p->AssetId,
                                         SBHYBRID_AssetDelete_ReadResult);
    }
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
    else
    {
        Ctx_p->AssetId = NewAssetId;
        Ctx_p->AssetFilled = 1;
    }
}
#endif

void
AES_IF_Ctx_LoadWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                          const void * const Wrap_p,
                          const uint32_t WrapLength)
{
    uint32_t NewAssetId;
    uint32_t Policy = CMTOKENS_ASSET_POLICY_ALGO_CIPHER_AES |
                      CMTOKENS_ASSET_POLICY_FUNCTION_ENCRYPT |
                      CMTOKENS_ASSET_POLICY_FUNCTION_DECRYPT;
    SB_Result_t res = SB_SUCCESS;

    IDENTIFIER_NOT_USED(WrapLength);

    /* Create additional asset for the unwrapped key */
    SBHYBRID_EIP123_PROCESS_IN2_OUT1(res,
                                     Ctx_p,
                                     SBHYBRID_AssetCreate_SendCommand,
                                     Policy,
                                     SBIF_CFG_CONFIDENTIALITY_BITS / 8,
                                     SBHYBRID_AssetCreateSearch_ReadResult,
                                     &NewAssetId);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
        return;
    }

    /* Call unwrap function */
    SBHYBRID_EIP123_PROCESS_IN4_OUT0(res,
                                     Ctx_p,
                                     SBHYBRID_AESUnwrap_SendCommand,
                                     NewAssetId,
                                     Ctx_p->AssetId,
                                     Wrap_p,
                                     SBIF_CFG_CONFIDENTIALITY_BITS / 8 + 8,
                                     SBHYBRID_AESUnwrap_ReadResult);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
        return;
    }

    /* Delete asset corresponding to original KEK, replace it with
       unwrapped key */
    if (Ctx_p->AssetFilled)
    {
        SBHYBRID_EIP123_PROCESS_IN1_OUT0(res,
                                         Ctx_p,
                                         SBHYBRID_AssetDelete_SendCommand,
                                         Ctx_p->AssetId,
                                         SBHYBRID_AssetDelete_ReadResult);
    }
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
    else
    {
        Ctx_p->AssetId = NewAssetId;
        Ctx_p->AssetFilled = 1;
    }
}

#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_Ctx_GenerateKey(AES_IF_Ctx_Ptr_t Ctx_p,
                       void * const Key_p,
                       const uint32_t KeyLength)
{
#ifdef AES_IF_OMIT_KEYGEN
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(Key_p);
    IDENTIFIER_NOT_USED(KeyLength);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP123_AES_IF: Key generation cannot be enabled."
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
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(Wrap_p);
    IDENTIFIER_NOT_USED(WrapLength);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP123_AES_IF: Key generation / wrapping cannot be enabled."
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

    SBHYBRID_EIP123_PROCESS_IN3_OUT0(res,
                                     Ctx_p,
                                     SBHYBRID_Crypto_SendCommand,
                                     DataIn_p,
                                     true,
                                     Ctx_p->AssetId,
                                     SBHYBRID_Crypto_ReadResult);
    if (res == SB_SUCCESS)
    {
        /* Write out the resulting value. */
        c_memcpy(DataOut_p, Ctx_p->SymmContext.SmallDMABuf.Host_p, 16);
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
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
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(DataIn_p);
    IDENTIFIER_NOT_USED(DataOut_p);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP123_AES_IF: ECB Decryption cannot be enabled."
#endif
}
#endif
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_CBC_Encrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const DataIn_p,
                   void * const DataOut_p,
                   const uint32_t Size)
{
#ifdef AES_IF_OMIT_CBC_ENCRYPT
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(DataIn_p);
    IDENTIFIER_NOT_USED(DataOut_p);
    IDENTIFIER_NOT_USED(Size);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP123_AES_IF: CBC Encryption cannot be enabled."
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

    SBHYBRID_EIP123_PROCESS_IN6_OUT1(res,
                                     Ctx_p,
                                     SBHYBRID_LongCrypto_SendCommand,
                                     DataIn_p,
                                     DataOut_p,
                                     Size,
                                     Ctx_p->IV,
                                     false,
                                     Ctx_p->AssetId,
                                     SBHYBRID_LongCrypto_ReadResult,
                                     Ctx_p->IV);
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

#endif /* SBHYBRID_WITH_EIP123 */

/* end of file sbhybrid_eip123_aes_if.c */
