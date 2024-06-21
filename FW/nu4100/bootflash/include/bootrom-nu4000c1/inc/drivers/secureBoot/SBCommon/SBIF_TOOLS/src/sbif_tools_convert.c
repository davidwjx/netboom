/* sbif_tools_convert.c
 *
 * Description: Secure boot sim utilities for dumping info
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


/* Option parser. */
#define SBIFTOOL_INTERATE_OPTIONS(option_p, \
                                  option_count, \
                                  var_type,   \
                                  var_p, \
                                  var_length_p) \
    do { \
        SBIFTOOLS_ConvertOption_t *__option_p = option_p;       \
        uint32_t __option_count = option_count;           \
        \
            for(;__option_p && __option_count; \
                 __option_p++, __option_count--) { \
            const SBIFTOOLS_ConvertOptionType_t var_type = \
                                          __option_p->ConvertOption; \
            void * const var_p = (void *) \
                                       __option_p->ConvertOptionPointer; \
            uint32_t *const var_length_p = &(__option_p->ConvertOptionLength);\
            \
            if (var_type == SBIFTOOLS_CONVERT_OPTION_NEXT_OPTIONS) \
            { \
                /* Next options seen => process them. */ \
                __option_p = var_p;                           \
                __option_count = *var_length_p / sizeof(*__option_p); \
                __option_p--; \
                __option_count++; \
                continue; \
            } \
            do

#define SBIFTOOL_INTERATE_OPTIONS_END \
            while(0); \
        } \
    } while(0)


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
    const uint32_t pubkeyType = SB_SW_BE32_READ(&Header_p->PubKeyType);
    const void * keyPtr = NULL;
    const void * keyInPtr = NULL;
    uint32_t iv[SBIF_CFG_CONFIDENTIALITY_BITS / 32];
    uint32_t key[SBIF_CFG_CONFIDENTIALITY_BITS / 32];
    uint32_t keyWrap[SBIF_CFG_CONFIDENTIALITY_BITS / 32 + 2];
    bool optOnly = false;
    bool decryptOut = false;
    bool encryptOut = false;
    const void * kekPtr = NULL;

    // Validate old image
    if (imgType != SBIF_IMAGE_BLTp &&
        imgType != SBIF_IMAGE_BLTw &&
        imgType != SBIF_IMAGE_BLTe &&
        imgType != SBIF_IMAGE_BLTx)
    {
        return SBIFTC_ERROR_IMAGE_TYPE;
    }

    if (imgVers != SBIF_VERSION)
    {
        return SBIFTC_ERROR_IMAGE_VERSION;
    }

    /* Validate public key type */
    if (pubkeyType != SBIF_PUBKEY_TYPE_ROM &&
        pubkeyType != SBIF_PUBKEY_TYPE_OTP &&
        pubkeyType != SBIF_PUBKEY_TYPE_IMAGE)
    {
        return SBIFTC_ERROR_IMAGE_TYPE;
    }


    // Validate requested image equivalently.
    if (NewSbifType != SBIF_IMAGE_BLTp &&
        NewSbifType != SBIF_IMAGE_BLTw &&
        NewSbifType != SBIF_IMAGE_BLTe &&
        NewSbifType != SBIF_IMAGE_BLTx)
    {
        return SBIFTC_ERROR_IMAGE_TYPE;
    }

    if (NewSbifVersion != SBIF_VERSION)
    {
        return SBIFTC_ERROR_IMAGE_VERSION;
    }

    if (NewSbifType == imgType && NewSbifVersion == imgVers)
    {
        /* No conversion, begin and end are same data type. */
        return SBIFTC_ERROR_ARGUMENTS;
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

    SBIFTOOL_INTERATE_OPTIONS(ConvertOptions_p,
                              ConvertOptionCount,
                              optType,
                              optData_p,
                              optLen_p)
    {
        switch (optType)
        {
        case SBIFTOOLS_CONVERT_OPTION_AES_ENCRYPTION_KEY:
            if ((NewSbifType != SBIF_IMAGE_BLTe &&
                 NewSbifType != SBIF_IMAGE_BLTx) ||
                keyPtr != NULL ||
                optData_p == NULL ||
                *optLen_p != SBIF_CFG_CONFIDENTIALITY_BITS / 8)
            {
                *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
                return SBIFTC_ERROR_ARGUMENTS;
            }
            keyPtr = optData_p;
            break;
        case SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY:
            if (imgType != SBIF_IMAGE_BLTx || keyInPtr != NULL ||
                optData_p == NULL ||
                *optLen_p != SBIF_CFG_CONFIDENTIALITY_BITS / 8)
            {
                *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
                return SBIFTC_ERROR_ARGUMENTS;
            }
            keyInPtr = optData_p;
            break;
        case SBIFTOOLS_CONVERT_OPTION_AES_WRAPPING_KEY:
            if (kekPtr != NULL ||
                optData_p == NULL ||
                *optLen_p != SBIF_CFG_CONFIDENTIALITY_BITS / 8)
            {
                *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
                return SBIFTC_ERROR_ARGUMENTS;
            }
            kekPtr = optData_p;
            break;
        case SBIFTOOLS_CONVERT_OPTION_AES_ENCRYPTION_KEY_OUT:
            if (optData_p == NULL)
            {
                optOnly = true;
                *optLen_p = SBIF_CFG_CONFIDENTIALITY_BITS / 8;
            }
            else if (*optLen_p != SBIF_CFG_CONFIDENTIALITY_BITS / 8)
            {
                *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
                return SBIFTC_ERROR_ARGUMENTS;
            }
            break;
        case SBIFTOOLS_CONVERT_OPTION_NEED_AES_ENCRYPTION_OUT:
        case SBIFTOOLS_CONVERT_OPTION_NEED_AES_DECRYPTION_OUT:
            if (optData_p == NULL)
            {
                optOnly = true;
                *optLen_p = sizeof(bool);
            }
            else if (*optLen_p != sizeof(bool))
            {
                *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
                return SBIFTC_ERROR_ARGUMENTS;
            }
            break;
        default:
            // Unknown argument.
            *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
            return SBIFTC_ERROR_ARGUMENTS;
        }
    }
    SBIFTOOL_INTERATE_OPTIONS_END;

    // Was only option handling requested?
    if (optOnly)
    {
        return SBIFTC_SUCCESS;
    }

    if ((imgType == SBIF_IMAGE_BLTx) && (keyInPtr == NULL))
    {
        // Key not specified for BLx format.
        return SBIFTC_ERROR_ARGUMENTS;
    }

    if (keyInPtr != NULL)
    {
        // Convert BLe/BLx/BLw to BLp

        if (keyPtr != NULL)
        {
            // Output key is specified, this is not supported, the conversion
            // shall either remove encryption or keep the same key.
            return SBIFTC_ERROR_ARGUMENTS;
        }

        if (NewSbifType == SBIF_IMAGE_BLTw)
        {
            /* Conversion from BLe/BLx to BLw must go through BLp. */
            return SBIFTC_ERROR_ARGUMENTS;
        }

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
        if (NewSbifType == SBIF_IMAGE_BLTp)
        {
            L_TRACE(LF_SBIFTC, "BLe/BLx => BLp conversion\n");
            AES_IF_Ctx_LoadKey(CryptoCtx, keyInPtr, SBIF_CFG_CONFIDENTIALITY_BITS);

        // BLw => BLp conversion continues here, after making a header copy
processImageToBLTp:
            c_memcpy(iv, Header_p->EncryptionIV, 16);
            c_memset(HeaderNew_p->EncryptionIV, 0, sizeof(HeaderNew_p->EncryptionIV));
            AES_IF_Ctx_LoadIV(CryptoCtx, iv);
            decryptOut = true;
        }
        else
        {
            keyPtr = keyInPtr;
        }

        SB_SW_BE32_WRITE(&HeaderNew_p->Type,
                         (NewSbifType << 8) | NewSbifVersion);

        /* Construct Encryption key field. */
        if (NewSbifType == SBIF_IMAGE_BLTe)
        {
            c_memset(&HeaderNew_p->EncryptionKey, 0, SBIF_ENCRYPTIONKEY_LEN * 4);
            c_memcpy(&HeaderNew_p->EncryptionKey, keyPtr,
                     SBIF_CFG_CONFIDENTIALITY_BITS / 8);
        }
        else
        {
            c_memset(&HeaderNew_p->EncryptionKey, 0, SBIF_ENCRYPTIONKEY_LEN * 4);
        }

        /* Successful conversion. Still needs to process output options. */
    }
    else if (imgType == SBIF_IMAGE_BLTp)
    {
        /* Convert BLp to other formats */

        /* Generate IV for encryption
           Note: The same function as for key generation is used. The function
                 may reject request to generate other number of bits than
                 SBIF_CFG_CONFIDENTIALITY_BITS, thus generate
                 SBIF_CFG_CONFIDENTIALITY_BITS, which can be more than 128. */
        AES_IF_Ctx_GenerateKey(CryptoCtx, iv, SBIF_CFG_CONFIDENTIALITY_BITS);

        /* Generate image key */
        if (keyPtr == NULL)
        {
            if (NewSbifType == SBIF_IMAGE_BLTw)
            {
#ifdef SBIF_CFG_DERIVE_WRAPKEY_FROM_KDK
                AES_IF_Ctx_DeriveKEK(CryptoCtx, kekPtr,
                                     kekPtr ? SBIF_CFG_CONFIDENTIALITY_BITS / 8 : 0,
                                     (uint8_t*)&Header_p->Signature,
                                     64);
#else
                AES_IF_Ctx_SetKEK(CryptoCtx, kekPtr,
                                  kekPtr ? SBIF_CFG_CONFIDENTIALITY_BITS / 8 : 0);
#endif
                AES_IF_Ctx_GenerateWrappedKey(CryptoCtx, keyWrap,
                                              SBIF_CFG_CONFIDENTIALITY_BITS + 64);
            }
            else
            {
                AES_IF_Ctx_GenerateKey(CryptoCtx, key, SBIF_CFG_CONFIDENTIALITY_BITS);
                keyPtr = key;
            }
        }
        else
        {
            /* Load image key */
            AES_IF_Ctx_LoadKey(CryptoCtx, keyPtr, SBIF_CFG_CONFIDENTIALITY_BITS);
        }

        /* Check for non-inplace operations. */
        if (HeaderNew_p != Header_p)
        {
            /* Construct new header using old header as a template. */
            c_memcpy(HeaderNew_p, Header_p, hdrSizeNew);

            /* Ensure HeaderNew_p matches the original header in size. */
            if (SBIF_ECDSA_GET_HEADER_SIZE(HeaderNew_p, hdrSizeNew) !=
                    hdrSizeNew)
            {
                return SBIFTC_ERROR_ARGUMENTS;
            }
        }

        c_memcpy(HeaderNew_p->EncryptionIV, iv, sizeof(iv));
        AES_IF_Ctx_LoadIV(CryptoCtx, iv);
        encryptOut = true;

        SB_SW_BE32_WRITE(&HeaderNew_p->Type, (NewSbifType << 8) | NewSbifVersion);

        /* Construct empty encryption key field */
        c_memset(&HeaderNew_p->EncryptionKey, 0, sizeof(HeaderNew_p->EncryptionKey));
        if (NewSbifType == SBIF_IMAGE_BLTe)
        {
            c_memcpy(&HeaderNew_p->EncryptionKey, keyPtr, SBIF_CFG_CONFIDENTIALITY_BITS / 8);
        }
        else if (NewSbifType == SBIF_IMAGE_BLTw)
        {
            c_memcpy(&HeaderNew_p->EncryptionKey, keyWrap, (SBIF_CFG_CONFIDENTIALITY_BITS + 64) / 8);
        }

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

        // Jump to BLw/e => BLp processing
        goto processImageToBLTp;
    }
    else
    {
        // Unsupported format conversion
        return SBIFTC_ERROR_ARGUMENTS;
    }

    SBIFTOOL_INTERATE_OPTIONS(ConvertOptions_p,
                              ConvertOptionCount,
                              optType,
                              optData_p,
                              optLen_p)
    {
        switch (optType)
        {
        case SBIFTOOLS_CONVERT_OPTION_AES_ENCRYPTION_KEY_OUT:
            if (!keyPtr)
            {
                *optLen_p = SBIFTOOLS_CONVERT_OPTION_ERROR;
                return SBIFTC_ERROR_ARGUMENTS;
            }
            c_memcpy(optData_p, keyPtr, SBIF_CFG_CONFIDENTIALITY_BITS / 8);
            break;
        case SBIFTOOLS_CONVERT_OPTION_NEED_AES_ENCRYPTION_OUT:
            c_memcpy(optData_p, &encryptOut, sizeof(bool));
            break;
        case SBIFTOOLS_CONVERT_OPTION_NEED_AES_DECRYPTION_OUT:
            c_memcpy(optData_p, &decryptOut, sizeof(bool));
            break;
        case SBIFTOOLS_CONVERT_OPTION_AES_ENCRYPTION_KEY:
        case SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY:
        case SBIFTOOLS_CONVERT_OPTION_NEXT_OPTIONS:
        default:
            break;
        }
    }
    SBIFTOOL_INTERATE_OPTIONS_END;

    // Check for errors detected by AES_IF
    if (AES_IF_Ctx_GetError(CryptoCtx))
    {
        return SBIFTC_ERROR_CRYPTO;
    }

    return SBIFTC_SUCCESS;
}

/* Image full conversion (header + data). */
SBIFTC_Result_t
SBIFTOOLS_Image_Convert(const SBIF_ECDSA_Header_t * Header_p,
                        uint32_t NewSbifType,
                        uint8_t  NewSbifVersion,
                        SBIF_ECDSA_Header_t * HeaderNew_p,
                        size_t * const HeaderNewSize_p,
                        const SBIF_SGVector_t * DataVectorsInput_p,
                        const SBIF_SGVector_t * DataVectorsOutput_p,
                        uint32_t VectorCount,
                        SBIFTOOLS_ConvertOption_t * ConvertOptions_p,
                        uint32_t ConvertOptionCount,
                        void * const CryptoCtx)
{
    uint32_t i;
    SBIFTC_Result_t sbiftc_res;
    bool decryptFlag;
    bool encryptFlag;
    SBIFTOOLS_ConvertOption_t convertOptionsNew[3] =
    {
        {
            SBIFTOOLS_CONVERT_OPTION_NEED_AES_ENCRYPTION_OUT, NULL,
            sizeof(bool)
        },
        {
            SBIFTOOLS_CONVERT_OPTION_NEED_AES_DECRYPTION_OUT, NULL,
            sizeof(bool)
        },
        { SBIFTOOLS_CONVERT_OPTION_NEXT_OPTIONS, NULL, 0 }
    };

    convertOptionsNew[0].ConvertOptionPointer = &encryptFlag;
    convertOptionsNew[1].ConvertOptionPointer = &decryptFlag;
    convertOptionsNew[2].ConvertOptionPointer = ConvertOptions_p;
    convertOptionsNew[2].ConvertOptionLength =
        sizeof(SBIFTOOLS_ConvertOption_t) * ConvertOptionCount;

    sbiftc_res = SBIFTOOLS_Image_ConvertHeader(Header_p,
                                               NewSbifType,
                                               NewSbifVersion,
                                               HeaderNew_p,
                                               HeaderNewSize_p,
                                               &convertOptionsNew[0],
                                               3,
                                               CryptoCtx);

    /* Header has been processed and CryptoCtx is prepared for handling
       image data. */
    if (sbiftc_res == SBIFTC_SUCCESS)
    {
        /* Encrypt / Decrypt image data as needed. */
        for(i = 0; i < VectorCount; i++)
        {
            if ((DataVectorsInput_p[i].DataLen & 15) != 0)
            {
                return SBIFTC_ERROR_ARGUMENTS;
            }

            if (DataVectorsInput_p[i].DataLen !=
                    DataVectorsOutput_p[i].DataLen)
            {
                return SBIFTC_ERROR_ARGUMENTS;
            }

            if (decryptFlag)
            {
                AES_IF_CBC_Decrypt(CryptoCtx,
                                   DataVectorsInput_p[i].Data_p,
                                   DataVectorsOutput_p[i].Data_p,
                                   DataVectorsInput_p[i].DataLen);

                /* Either decrypt or encrypt can be set. */
                ASSERT(encryptFlag == false);
            }
            else if (encryptFlag)
            {
                AES_IF_CBC_Encrypt(CryptoCtx,
                                   DataVectorsInput_p[i].Data_p,
                                   DataVectorsOutput_p[i].Data_p,
                                   DataVectorsInput_p[i].DataLen);
            }
        }
    }

    /* Check for errors detected by AES_IF. */
    if (AES_IF_Ctx_GetError(CryptoCtx))
    {
        return SBIFTC_ERROR_CRYPTO;
    }

    return sbiftc_res;
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

/* end of file sbif_tools_convert.c */
