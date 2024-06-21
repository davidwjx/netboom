/* sbhybrid_eip130_asset.c
 *
 * Description: Implementation of commonly used asset related function.
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
#include "eip130_token_asset.h"

// Asset Search functionality, if using Secure Boot key from OTP.
#ifndef SBLIB_CF_REMOVE_IMAGE_TYPE_W
#ifndef SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY
#define SBHYBRID_ENABLE_ASSETSEARCH
#endif
#endif


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetCreate_SendCommand
 */
SB_Result_t
SBHYBRID_AssetCreate_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 uint64_t Policy,
                                 uint32_t AssetSizeInBytes)
{
    Eip130Token_Command_t t_cmd;

    L_DEBUG(LF_SBHYBRID, "EIP130 AssetCreate token write");

    Eip130Token_Command_AssetCreate(&t_cmd, Policy, AssetSizeInBytes);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}


#ifdef SBHYBRID_ENABLE_ASSETSEARCH
/*----------------------------------------------------------------------------
 * SBHYBRID_AssetSearch_SendCommand
 */
SB_Result_t
SBHYBRID_AssetSearch_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 const uint32_t AssetNumber)
{
    Eip130Token_Command_t t_cmd;

    L_DEBUG(LF_SBHYBRID, "EIP130 AssetSearch token write (%u)", AssetNumber);

    Eip130Token_Command_AssetSearch(&t_cmd, AssetNumber);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}
#endif /* SBHYBRID_ENABLE_ASSETSEARCH */


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetCreateSearch_ReadResult
 */
SB_Result_t
SBHYBRID_AssetCreateSearch_ReadResult(SBHYBRID_SymmContext_t * const Context_p,
                                      uint32_t * AssetId_p)
{
    Eip130Token_Result_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR);
    if (sbres == SB_SUCCESS)
    {
        // Get AS_ID from response message
        Eip130Token_Result_AssetCreate(&t_res, AssetId_p);
    }
    else
    {
        *AssetId_p = 0;
    }
    L_DEBUG(LF_SBHYBRID, "AssetID = 0x%X", (unsigned int)*AssetId_p);
    return sbres;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetDelete_SendCommand
 */
SB_Result_t
SBHYBRID_AssetDelete_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 const uint32_t AssetId)
{
    Eip130Token_Command_t t_cmd;

    L_DEBUG(LF_SBHYBRID, "EIP130 AssetDelete token write 0x%X", (unsigned int)AssetId);

    Eip130Token_Command_AssetDelete(&t_cmd, AssetId);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetDelete_ReadResult
 */
SB_Result_t
SBHYBRID_AssetDelete_ReadResult(SBHYBRID_SymmContext_t * const Context_p)
{
    Eip130Token_Result_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR);
    return sbres;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_AssetLoadPlaintext_SendCommand
 *
 * This function loads the asset data via plaintext into the SM asset store.
 * Note that no KeyBlob is generated.
 */
SB_Result_t
SBHYBRID_AssetLoadPlaintext_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                        const uint32_t AssetId,
                                        const uint8_t * Data_p,
                                        const uint16_t DataLengthInbytes)
{
    Eip130Token_Command_t t_cmd;
    uint64_t DataAddr;

    L_DEBUG(LF_SBHYBRID, "EIP130 AssetLoadPlaintext token write");

    // Get the DMA address of the data
    DataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                    Data_p,
                                                    (uint32_t)DataLengthInbytes);
    if (DataAddr == 0)
    {
        void * Host_p;

        DataAddr = SBHYBRID_EIP130_Common_DmaAlloc(Context_p,
                                                   (uint32_t)DataLengthInbytes,
                                                   &Host_p);
        if (DataAddr == 0)
        {
            return SB_ERROR_HARDWARE;
        }

        c_memcpy(Host_p, Data_p, DataLengthInbytes);
        SBHYBRID_EIP130_Common_PreDma(Context_p, NULL);
    }

    Eip130Token_Command_AssetLoad_Plaintext(&t_cmd, AssetId);
    Eip130Token_Command_AssetLoad_SetInput(&t_cmd, DataAddr, DataLengthInbytes);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}

/*----------------------------------------------------------------------------
 * SBHYBRID_AssetLoadAESUnwrap_SendCommand
 *
 * This function loads the asset data via AES unwrap into the SM asset store.
 */
SB_Result_t
SBHYBRID_AssetLoadAESUnwrap_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                        const uint32_t AssetId,
                                        const uint32_t KEKAssetId,
                                        const uint8_t * Data_p,
                                        const uint16_t DataLengthInbytes)
{
    Eip130Token_Command_t t_cmd;
    uint64_t DataAddr;

    L_DEBUG(LF_SBHYBRID, "EIP130 AssetLoadAESUnwrap token write");

    // Get the DMA address of the data
    DataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                    Data_p,
                                                    (uint32_t)DataLengthInbytes);
    if (DataAddr == 0)
    {
        void * Host_p;

        DataAddr = SBHYBRID_EIP130_Common_DmaAlloc(Context_p,
                                                   (uint32_t)DataLengthInbytes,
                                                   &Host_p);
        if (DataAddr == 0)
        {
            return SB_ERROR_HARDWARE;
        }

        c_memcpy(Host_p, Data_p, DataLengthInbytes);
        SBHYBRID_EIP130_Common_PreDma(Context_p, NULL);
    }

    Eip130Token_Command_AssetLoad_AesUnwrap(&t_cmd, AssetId, KEKAssetId);
    Eip130Token_Command_AssetLoad_SetInput(&t_cmd, DataAddr, DataLengthInbytes);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}

#ifdef SBIF_CFG_DERIVE_WRAPKEY_FROM_KDK
/*----------------------------------------------------------------------------
 * SBHYBRID_AssetLoadDerive_SendCommand
 *
 * This function loads the asset data via a derivation operation into the SM
 * asset store.
 */
SB_Result_t
SBHYBRID_AssetLoadDerive_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                     const uint32_t AssetId,
                                     const uint32_t KDKAssetId,
                                     const uint8_t * Aad_p,
                                     const uint16_t AadLengthInBytes)
{
    Eip130Token_Command_t t_cmd;

    L_DEBUG(LF_SBHYBRID, "EIP130 AssetLoadDerive token write");

    Eip130Token_Command_AssetLoad_Derive(&t_cmd, AssetId, KDKAssetId,
                                         false, false);
    Eip130Token_Command_AssetLoad_SetInput(&t_cmd, 0, 0);
    Eip130Token_Command_AssetLoad_SetOutput(&t_cmd, 0, 0);
    Eip130Token_Command_AssetLoad_SetAad(&t_cmd, Aad_p, AadLengthInBytes);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}
#endif

/*----------------------------------------------------------------------------
 * SBHYBRID_AssetLoad_ReadResult
 */
SB_Result_t
SBHYBRID_AssetLoad_ReadResult(SBHYBRID_SymmContext_t * const Context_p)
{
    Eip130Token_Result_t t_res;
    SB_Result_t sbres;

    sbres = SBHYBRID_EIP130_Common_ReadResultAndCheck(Context_p->Device_EIP130,
                                                      &t_res,
                                                      SBLIB_CFG_XM_MAILBOXNR);

    SBHYBRID_EIP130_Common_DmaRelease(Context_p);

    return sbres;
}


#endif /* SBHYBRID_WITH_EIP130 */

/* end of file sbhybrid_eip130_asset.c */
