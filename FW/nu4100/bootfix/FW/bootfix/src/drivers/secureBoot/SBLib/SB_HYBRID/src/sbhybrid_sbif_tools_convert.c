/* sbhybrid_sbif_tools_convert.c
 *
 * Description: Secure boot format conversion: only convert from
 *              BLw/BLe/BLx to BLp.
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
#include "c_lib.h"
#include "sbif_tools_convert.h"
#include "aes_if.h"
#include "sb_sw_endian.h"

#if (SBIF_CFG_CONFIDENTIALITY_BITS != 128) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 192) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 256)
#error "SBIF_CFG_CONFIDENTIALITY_BITS must be one of 128, 192, or 256."
#endif


/* Image header conversion. */
SBIFTC_Result_t
SBIFTOOLS_Image_ConvertHeader(const SBIF_ECDSA_Header_t * const Header_p,
                              const uint32_t NewSbifType,
                              const uint8_t NewSbifVersion,
                              SBIF_ECDSA_Header_t * const HeaderNew_p,
                              size_t * const HeaderNewSize_p,
                              SBIFTOOLS_ConvertOption_t * const ConvertOptions_p,
                              const uint32_t ConvertOptionCount,
                              void * const CryptoCtx)
{
    const size_t hdrSizeNew = SBIF_ECDSA_GET_HEADER_SIZE(Header_p, (uint32_t)-1);
    const uint32_t imgTypeVers = SB_SW_BE32_READ(&Header_p->Type);
    const uint32_t imgType = SBIF_TYPE_TYPE(imgTypeVers);
    const uint32_t imgVers = SBIF_TYPE_VERSION(imgTypeVers);
    const void * keyInPtr = NULL;
    const void * kekPtr = NULL;

    PRECONDITION(ConvertOptionCount == 0 ||
                 (ConvertOptionCount == 1 &&
                  ConvertOptions_p->ConvertOption ==
                      SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY) ||
                 (ConvertOptionCount == 1 &&
                  ConvertOptions_p->ConvertOption ==
                      SBIFTOOLS_CONVERT_OPTION_AES_WRAPPING_KEY));

    // Validate old image
    if (imgType != SBIF_IMAGE_BLTw &&
        imgType != SBIF_IMAGE_BLTe &&
        imgType != SBIF_IMAGE_BLTx)
    {
        return SBIFTC_ERROR_IMAGE_TYPE;
    }

    if (imgVers != SBIF_VERSION)
    {
        return SBIFTC_ERROR_IMAGE_VERSION;
    }

    // Validate requested image equivalently.
    if (NewSbifType != SBIF_IMAGE_BLTp)
    {
        return SBIFTC_ERROR_IMAGE_TYPE;
    }

    if (NewSbifVersion != SBIF_VERSION)
    {
        return SBIFTC_ERROR_IMAGE_VERSION;
    }

    if (*HeaderNewSize_p < hdrSizeNew)
    {
        *HeaderNewSize_p = hdrSizeNew;
        return SBIFTC_ERROR_ARGUMENTS;
    }

    if (imgType == SBIF_IMAGE_BLTe)
    {
        keyInPtr = Header_p->EncryptionKey;
    }

    if (ConvertOptionCount == 1)
    {
        if (ConvertOptions_p->ConvertOption == SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY)
        {
            keyInPtr = ConvertOptions_p->ConvertOptionPointer;
        }

        if (ConvertOptions_p->ConvertOption == SBIFTOOLS_CONVERT_OPTION_AES_WRAPPING_KEY)
        {
            kekPtr = ConvertOptions_p->ConvertOptionPointer;
        }
    }

    if ((imgType == SBIF_IMAGE_BLTx) && (keyInPtr == NULL))
    {
        // Key not specified for BLx format.
        return SBIFTC_ERROR_ARGUMENTS;
    }

    if (keyInPtr != NULL)
    {
        // Convert Blw/BLe/BLx to BLp
        // Note: A part of this path is shared with BLw

        // Check for non-inplace operations
        if (HeaderNew_p != Header_p)
        {
            // Construct new header using old header as a template
            c_memcpy(HeaderNew_p, Header_p, hdrSizeNew);

            // Ensure HeaderNew_p matches the original header in size
            if (SBIF_ECDSA_GET_HEADER_SIZE(HeaderNew_p, hdrSizeNew) != hdrSizeNew)
            {
                return SBIFTC_ERROR_ARGUMENTS;
            }
        }

        // Key used in source is known. Use the key to translate image to BLp
        L_TRACE(LF_SBIFTC, "BLe/BLx => BLp conversion\n");
        AES_IF_Ctx_LoadKey(CryptoCtx, keyInPtr, SBIF_CFG_CONFIDENTIALITY_BITS);

        // BLw => BLp conversion continues here, after making a header copy
processImageToBLTp:
        AES_IF_Ctx_LoadIV(CryptoCtx, Header_p->EncryptionIV);
        c_memset(HeaderNew_p->EncryptionIV, 0, sizeof(HeaderNew_p->EncryptionIV));

        SB_SW_BE32_WRITE(&HeaderNew_p->Type, (NewSbifType << 8) | NewSbifVersion);

        /* Construct empty encryption key field */
        c_memset(&HeaderNew_p->EncryptionKey, 0, SBIF_ENCRYPTIONKEY_LEN * 4);

        // Successful conversion. Still needs to process output options
    }
    else if ((imgType == SBIF_IMAGE_BLTw) &&
             (NewSbifType == SBIF_IMAGE_BLTp))
    {
        // Check for non-inplace operations
        if (HeaderNew_p != Header_p)
        {
            // Construct new header using old header as a template
            c_memcpy(HeaderNew_p, Header_p, hdrSizeNew);

            // Ensure HeaderNew_p matches the original header in size
            if (SBIF_ECDSA_GET_HEADER_SIZE(HeaderNew_p, hdrSizeNew) != hdrSizeNew)
            {
                return SBIFTC_ERROR_ARGUMENTS;
            }
        }

        // Begin decrypt BLw image
#ifdef SBIF_CFG_DERIVE_WRAPKEY_FROM_KDK
        AES_IF_Ctx_DeriveKEK(CryptoCtx, kekPtr,
                             kekPtr ? SBIF_CFG_CONFIDENTIALITY_BITS / 8 : 0,
                             (uint8_t *)&Header_p->Signature,
                             64);
#else
        AES_IF_Ctx_SetKEK(CryptoCtx, kekPtr,
                          kekPtr ? SBIF_CFG_CONFIDENTIALITY_BITS / 8 : 0);
#endif
        AES_IF_Ctx_LoadWrappedKey(CryptoCtx,
                                  &HeaderNew_p->EncryptionKey,
                                  (SBIF_CFG_CONFIDENTIALITY_BITS + 64));

        // Jump to Bw/e => BLp processing
        goto processImageToBLTp;
    }
    else
    {
        // Unsupported format conversion
        return SBIFTC_ERROR_ARGUMENTS;
    }

    // Check for errors detected by AES_IF
    if (AES_IF_Ctx_GetError(CryptoCtx))
    {
        return SBIFTC_ERROR_CRYPTO;
    }

    return SBIFTC_SUCCESS;
}

SBIFTC_Result_t
SBIFTOOLS_Image_Convert_InitWorkspace(void * const Workspace_p,
                                      uint32_t * const Workspace_size_p)
{
    /* Check crypto context */
    if (Workspace_size_p == NULL)
    {
        return SBIFTC_ERROR_ARGUMENTS;
    }

    if ((Workspace_p == NULL) || (*Workspace_size_p < AES_IF_Ctx_GetSize()))
    {
        /* Get crypto context size */
        *Workspace_size_p = AES_IF_Ctx_GetSize();
        return SBIFTC_ERROR_ARGUMENTS;
    }

    /* Initialize crypto context */
    if (AES_IF_Ctx_Init(Workspace_p) != AES_IF_RESULT_SUCCESS)
    {
        return SBIFTC_ERROR_CRYPTO;
    }

    return SBIFTC_SUCCESS;
}

void
SBIFTOOLS_Image_Convert_UninitWorkspace(void * const Workspace_p)
{
    /* Try to uninitialize cryptographic library. */
    AES_IF_Ctx_Uninit(Workspace_p);

    /* Remove any stored potentially cryptographically sensitive data. */
    c_memset(Workspace_p, 0, AES_IF_Ctx_GetSize());
}

/* end of file sbhybrid_sbif_tools_convert.c */
