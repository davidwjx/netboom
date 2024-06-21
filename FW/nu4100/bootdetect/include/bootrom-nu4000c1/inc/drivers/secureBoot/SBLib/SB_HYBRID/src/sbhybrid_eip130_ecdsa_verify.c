/* sbhybrid_eip130_ecdsa-verify.c
 *
 * Description: Secure Boot ECDSA-Verify acceleration using EIP-130.
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

#ifndef __MODULE__
#define __MODULE__ "sbhybrid_eip130_ecdsa_verify.c"
#endif

#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_EIP130
#ifdef SBHYBRID_WITH_EIP130PK
#include "c_lib.h"

#include "eip130.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "device_swap.h"
#include "cs_eip130_token.h"
#include "eip130_token_common.h"
#include "eip130_token_system.h"
#include "eip130_token_asset.h"
#include "eip130_token_pk.h"

// Enable logging for ECDSA parameters, public key and signature when debugging
// is enabled
#ifdef CFG_ENABLE_DEBUG
//#define ENABLE_KEY_INFO_LOGGING
#ifdef ENABLE_KEY_INFO_LOGGING
#define LOG_SEVERITY_MAX LOG_SEVERITY_INFO
#include "log.h"
#endif
#endif

#ifdef SBLIB_CFG_SM_SECURE_ACCESS
#define SBHYBRID_ACCESS_POLICY  0
#else
#define SBHYBRID_ACCESS_POLICY  EIP130TOKEN_ASSET_POLICY_SOURCE_NON_SECURE
#endif

#if SBIF_ECDSA_WORDS == 17
#define SBHYBRID_PUBLICKEY_POLICY (SBHYBRID_ACCESS_POLICY                     | \
                                   EIP130TOKEN_ASSET_POLICY_PK_ECC_ECDSA_SIGN | \
                                   EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY        | \
                                   EIP130TOKEN_ASSET_POLICY_SHA512)

// P-521 curve parameters
static const uint8_t PublicKeyParameters[] =
{
    // P-521 : p
    0x09, 0x02, 0x00, 0x07,             // Header
    0xff, 0xff, 0xff, 0xff,             // Value ...
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0x01, 0x00, 0x00,

    // P-521 : a
    0x09, 0x02, 0x01, 0x07,             // Header
    0xfc, 0xff, 0xff, 0xff,             // Value ...
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0x01, 0x00, 0x00,

    // P-521 : b
    0x09, 0x02, 0x02, 0x07,             // Header
    0x00, 0x3f, 0x50, 0x6b,             // Value ...
    0xd4, 0x1f, 0x45, 0xef,
    0xf1, 0x34, 0x2c, 0x3d,
    0x88, 0xdf, 0x73, 0x35,
    0x07, 0xbf, 0xb1, 0x3b,
    0xbd, 0xc0, 0x52, 0x16,
    0x7b, 0x93, 0x7e, 0xec,
    0x51, 0x39, 0x19, 0x56,
    0xe1, 0x09, 0xf1, 0x8e,
    0x91, 0x89, 0xb4, 0xb8,
    0xf3, 0x15, 0xb3, 0x99,
    0x5b, 0x72, 0xda, 0xa2,
    0xee, 0x40, 0x85, 0xb6,
    0xa0, 0x21, 0x9a, 0x92,
    0x1f, 0x9a, 0x1c, 0x8e,
    0x61, 0xb9, 0x3e, 0x95,
    0x51, 0x00, 0x00, 0x00,

    // P-521 : n
    0x09, 0x02, 0x03, 0x07,             // Header
    0x09, 0x64, 0x38, 0x91,             // Value ...
    0x1e, 0xb7, 0x6f, 0xbb,
    0xae, 0x47, 0x9c, 0x89,
    0xb8, 0xc9, 0xb5, 0x3b,
    0xd0, 0xa5, 0x09, 0xf7,
    0x48, 0x01, 0xcc, 0x7f,
    0x6b, 0x96, 0x2f, 0xbf,
    0x83, 0x87, 0x86, 0x51,
    0xfa, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0x01, 0x00, 0x00,

    // P-521 : Gx point
    0x09, 0x02, 0x04, 0x07,             // Header
    0x66, 0xbd, 0xe5, 0xc2,             // Value ...
    0x31, 0x7e, 0x7e, 0xf9,
    0x9b, 0x42, 0x6a, 0x85,
    0xc1, 0xb3, 0x48, 0x33,
    0xde, 0xa8, 0xff, 0xa2,
    0x27, 0xc1, 0x1d, 0xfe,
    0x28, 0x59, 0xe7, 0xef,
    0x77, 0x5e, 0x4b, 0xa1,
    0xba, 0x3d, 0x4d, 0x6b,
    0x60, 0xaf, 0x28, 0xf8,
    0x21, 0xb5, 0x3f, 0x05,
    0x39, 0x81, 0x64, 0x9c,
    0x42, 0xb4, 0x95, 0x23,
    0x66, 0xcb, 0x3e, 0x9e,
    0xcd, 0xe9, 0x04, 0x04,
    0xb7, 0x06, 0x8e, 0x85,
    0xc6, 0x00, 0x00, 0x00,

    // P-521 : Gy point
    0x09, 0x02, 0x05, 0x07,             // Header
    0x50, 0x66, 0xd1, 0x9f,             // Value ...
    0x76, 0x94, 0xbe, 0x88,
    0x40, 0xc2, 0x72, 0xa2,
    0x86, 0x70, 0x3c, 0x35,
    0x61, 0x07, 0xad, 0x3f,
    0x01, 0xb9, 0x50, 0xc5,
    0x40, 0x26, 0xf4, 0x5e,
    0x99, 0x72, 0xee, 0x97,
    0x2c, 0x66, 0x3e, 0x27,
    0x17, 0xbd, 0xaf, 0x17,
    0x68, 0x44, 0x9b, 0x57,
    0x49, 0x44, 0xf5, 0x98,
    0xd9, 0x1b, 0x7d, 0x2c,
    0xb4, 0x5f, 0x8a, 0x5c,
    0x04, 0xc0, 0x3b, 0x9a,
    0x78, 0x6a, 0x29, 0x39,
    0x18, 0x01, 0x00, 0x00,

    // P-521 : h
    0x01, 0x00, 0x06, 0x07,             // Header
    0x01, 0x00, 0x00, 0x00,             // Value ...
};
#elif SBIF_ECDSA_WORDS == 12
#define SBHYBRID_PUBLICKEY_POLICY (SBHYBRID_ACCESS_POLICY                     | \
                                   EIP130TOKEN_ASSET_POLICY_PK_ECC_ECDSA_SIGN | \
                                   EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY        | \
                                   EIP130TOKEN_ASSET_POLICY_SHA384)

// P-384 curve parameters
static const uint8_t PublicKeyParameters[] =
{
    // P-384 : p
    0x80, 0x01, 0x00, 0x07,             // Header
    0xff, 0xff, 0xff, 0xff,             // Value ...
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff,
    0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,

    // P-384 : a
    0x80, 0x01, 0x01, 0x07,             // Header
    0xfc, 0xff, 0xff, 0xff,             // Value ...
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff,
    0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,

    // P-384 : b
    0x80, 0x01, 0x02, 0x07,             // Header
    0xef, 0x2a, 0xec, 0xd3,             // Value ...
    0xed, 0xc8, 0x85, 0x2a,
    0x9d, 0xd1, 0x2e, 0x8a,
    0x8d, 0x39, 0x56, 0xc6,
    0x5a, 0x87, 0x13, 0x50,
    0x8f, 0x08, 0x14, 0x03,
    0x12, 0x41, 0x81, 0xfe,
    0x6e, 0x9c, 0x1d, 0x18,
    0x19, 0x2d, 0xf8, 0xe3,
    0x6b, 0x05, 0x8e, 0x98,
    0xe4, 0xe7, 0x3e, 0xe2,
    0xa7, 0x2f, 0x31, 0xb3,

    // P-384 : n
    0x80, 0x01, 0x03, 0x07,             // Header
    0x73, 0x29, 0xc5, 0xcc,             // Value ...
    0x6a, 0x19, 0xec, 0xec,
    0x7a, 0xa7, 0xb0, 0x48,
    0xb2, 0x0d, 0x1a, 0x58,
    0xdf, 0x2d, 0x37, 0xf4,
    0x81, 0x4d, 0x63, 0xc7,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,

    // P-384 : Gx point
    0x80, 0x01, 0x04, 0x07,             // Header
    0xb7, 0x0a, 0x76, 0x72,             // Value ...
    0x38, 0x5e, 0x54, 0x3a,
    0x6c, 0x29, 0x55, 0xbf,
    0x5d, 0xf2, 0x02, 0x55,
    0x38, 0x2a, 0x54, 0x82,
    0xe0, 0x41, 0xf7, 0x59,
    0x98, 0x9b, 0xa7, 0x8b,
    0x62, 0x3b, 0x1d, 0x6e,
    0x74, 0xad, 0x20, 0xf3,
    0x1e, 0xc7, 0xb1, 0x8e,
    0x37, 0x05, 0x8b, 0xbe,
    0x22, 0xca, 0x87, 0xaa,

    // P-384 : Gy point
    0x80, 0x01, 0x05, 0x07,             // Header
    0x5f, 0x0e, 0xea, 0x90,             // Value ...
    0x7c, 0x1d, 0x43, 0x7a,
    0x9d, 0x81, 0x7e, 0x1d,
    0xce, 0xb1, 0x60, 0x0a,
    0xc0, 0xb8, 0xf0, 0xb5,
    0x13, 0x31, 0xda, 0xe9,
    0x7c, 0x14, 0x9a, 0x28,
    0xbd, 0x1d, 0xf4, 0xf8,
    0x29, 0xdc, 0x92, 0x92,
    0xbf, 0x98, 0x9e, 0x5d,
    0x6f, 0x2c, 0x26, 0x96,
    0x4a, 0xde, 0x17, 0x36,

    // P-384 : h
    0x01, 0x00, 0x06, 0x07,             // Header
    0x01, 0x00, 0x00, 0x00,             // Value ...
};
#elif SBIF_ECDSA_WORDS == 8
#define SBHYBRID_PUBLICKEY_POLICY (SBHYBRID_ACCESS_POLICY                     | \
                                   EIP130TOKEN_ASSET_POLICY_PK_ECC_ECDSA_SIGN | \
                                   EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY        | \
                                   EIP130TOKEN_ASSET_POLICY_SHA256)

// P-256 curve parameters
static const uint8_t PublicKeyParameters[] =
{
    // P-256 : p
    0x00, 0x01, 0x00, 0x07,             // Header
    0xff, 0xff, 0xff, 0xff,             // Value ...
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff,

    // P-256 : a
    0x00, 0x01, 0x01, 0x07,             // Header
    0xfc, 0xff, 0xff, 0xff,             // Value ...
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0x01, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff,

    // P-256 : b
    0x00, 0x01, 0x02, 0x07,             // Header
    0x4b, 0x60, 0xd2, 0x27,             // Value ...
    0x3e, 0x3c, 0xce, 0x3b,
    0xf6, 0xb0, 0x53, 0xcc,
    0xb0, 0x06, 0x1d, 0x65,
    0xbc, 0x86, 0x98, 0x76,
    0x55, 0xbd, 0xeb, 0xb3,
    0xe7, 0x93, 0x3a, 0xaa,
    0xd8, 0x35, 0xc6, 0x5a,

    // P-256 : n
    0x00, 0x01, 0x03, 0x07,             // Header
    0x51, 0x25, 0x63, 0xfc,             // Value ...
    0xc2, 0xca, 0xb9, 0xf3,
    0x84, 0x9e, 0x17, 0xa7,
    0xad, 0xfa, 0xe6, 0xbc,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff,

    // P-256 : Gx point
    0x00, 0x01, 0x04, 0x07,             // Header
    0x96, 0xc2, 0x98, 0xd8,             // Value ...
    0x45, 0x39, 0xa1, 0xf4,
    0xa0, 0x33, 0xeb, 0x2d,
    0x81, 0x7d, 0x03, 0x77,
    0xf2, 0x40, 0xa4, 0x63,
    0xe5, 0xe6, 0xbc, 0xf8,
    0x47, 0x42, 0x2c, 0xe1,
    0xf2, 0xd1, 0x17, 0x6b,

    // P-256 : Gy point
    0x00, 0x01, 0x05, 0x07,             // Header
    0xf5, 0x51, 0xbf, 0x37,             // Value ...
    0x68, 0x40, 0xb6, 0xcb,
    0xce, 0x5e, 0x31, 0x6b,
    0x57, 0x33, 0xce, 0x2b,
    0x16, 0x9e, 0x0f, 0x7c,
    0x4a, 0xeb, 0xe7, 0x8e,
    0x9b, 0x7f, 0x1a, 0xfe,
    0xe2, 0x42, 0xe3, 0x4f,

    // P-256 : h
    0x01, 0x00, 0x06, 0x07,             // Header
    0x01, 0x00, 0x00, 0x00,             // Value ...
};
#else
#define SBHYBRID_PUBLICKEY_POLICY (SBHYBRID_ACCESS_POLICY                     | \
                                   EIP130TOKEN_ASSET_POLICY_PK_ECC_ECDSA_SIGN | \
                                   EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY        | \
                                   EIP130TOKEN_ASSET_POLICY_SHA224)

// P-224 curve parameters
static const uint8_t PublicKeyParameters[] =
{
    // P-224 : p
    0xe0, 0x00, 0x00, 0x07,             // Header
    0x01, 0x00, 0x00, 0x00,             // Value ...
    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,

    // P-224 : a
    0xe0, 0x00, 0x01, 0x07,             // Header
    0xfe, 0xff, 0xff, 0xff,             // Value ...
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xfe, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,

    // P-224 : b
    0xe0, 0x00, 0x02, 0x07,             // Header
    0xb4, 0xff, 0x55, 0x23,             // Value ...
    0x43, 0x39, 0x0b, 0x27,
    0xba, 0xd8, 0xbf, 0xd7,
    0xb7, 0xb0, 0x44, 0x50,
    0x56, 0x32, 0x41, 0xf5,
    0xab, 0xb3, 0x04, 0x0c,
    0x85, 0x0a, 0x05, 0xb4,

    // P-224 : n
    0xe0, 0x00, 0x03, 0x07,             // Header
    0x3d, 0x2a, 0x5c, 0x5c,             // Value ...
    0x45, 0x29, 0xdd, 0x13,
    0x3e, 0xf0, 0xb8, 0xe0,
    0xa2, 0x16, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,

    // P-224 : Gx point
    0xe0, 0x00, 0x04, 0x07,             // Header
    0x21, 0x1d, 0x5c, 0x11,             // Value ...
    0xd6, 0x80, 0x32, 0x34,
    0x22, 0x11, 0xc2, 0x56,
    0xd3, 0xc1, 0x03, 0x4a,
    0xb9, 0x90, 0x13, 0x32,
    0x7f, 0xbf, 0xb4, 0x6b,
    0xbd, 0x0c, 0x0e, 0xb7,

    // P-224 : Gy point
    0xe0, 0x00, 0x05, 0x07,             // Header
    0x34, 0x7e, 0x00, 0x85,             // Value ...
    0x99, 0x81, 0xd5, 0x44,
    0x64, 0x47, 0x07, 0x5a,
    0xa0, 0x75, 0x43, 0xcd,
    0xe6, 0xdf, 0x22, 0x4c,
    0xfb, 0x23, 0xf7, 0xb5,
    0x88, 0x63, 0x37, 0xbd,

    // P-224 : h
    0x01, 0x00, 0x06, 0x07,             // Header
    0x01, 0x00, 0x00, 0x00,             // Value ...
};
#endif

#define SBHYBRID_ECDSAVerify_ReadResult  SBHYBRID_AssetLoad_ReadResult

/*------------------------------------------------------
* ECDSA with asset send verify command
-------------------------------------------------------*/
static SB_Result_t
SBHYBRID_ECDSAVerify_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 const uint32_t KeyAssetId,
                                 const uint32_t ParamsAssetId,
                                 const uint32_t IOAssetId,
                                 const uint8_t * Signature_p,
                                 const uint16_t SignatureLengthInBytes,
                                 const uint8_t * HashData_p,
                                 const uint16_t HashDataLengthInBytes,
                                 const uint64_t TotalHashLengthInBytes)
{
    Eip130Token_Command_t t_cmd;
    uint64_t HashDataAddr;
    uint64_t SigDataAddr;

    // Get the DMA address of the data
    HashDataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                        HashData_p,
                                                        (uint32_t)HashDataLengthInBytes);
    if (HashDataAddr == 0)
    {
        void * Host_p;

        HashDataAddr = SBHYBRID_EIP130_Common_DmaAlloc(Context_p,
                                                       (uint32_t)HashDataLengthInBytes,
                                                       &Host_p);
        if (HashDataAddr == 0)
        {
            return SB_ERROR_HARDWARE;
        }

        c_memcpy(Host_p, HashData_p, HashDataLengthInBytes);
        SBHYBRID_EIP130_Common_PreDma(Context_p, NULL);
    }

    SigDataAddr = SBHYBRID_EIP130_Common_GetDmaAddress(Context_p,
                                                       Signature_p,
                                                       (uint32_t)SignatureLengthInBytes);
    if (SigDataAddr == 0)
    {
        void * Host_p;

        SigDataAddr = SBHYBRID_EIP130_Common_DmaAlloc(Context_p,
                                                      (uint32_t)SignatureLengthInBytes,
                                                      &Host_p);
        if (SigDataAddr == 0)
        {
            return SB_ERROR_HARDWARE;
        }

        c_memcpy(Host_p, Signature_p, SignatureLengthInBytes);
        SBHYBRID_EIP130_Common_PreDma(Context_p, NULL);
    }

    // Create token
    Eip130Token_Command_Pk_Asset_Command(&t_cmd,
                                         EIP130TOKEN_PK_ASSET_CMD_ECDSA_VERIFY,
                                         SBIF_ECDSA_WORDS,
                                         SBIF_ECDSA_WORDS,
                                         0,
                                         KeyAssetId,
                                         ParamsAssetId,
                                         IOAssetId,
                                         HashDataAddr,
                                         HashDataLengthInBytes,
                                         SigDataAddr,
                                         SignatureLengthInBytes);
    Eip130Token_Command_Pk_Asset_SetAdditionalLength(&t_cmd,
                                                     TotalHashLengthInBytes);
    Eip130Token_Command_Identity(&t_cmd, SBLIB_CFG_XM_TOKEN_IDENTITY);
    Eip130Token_Command_SetTokenID(&t_cmd, ++Context_p->TokenId, false);

    return SBHYBRID_EIP130_Common_WriteCommand(Context_p->Device_EIP130,
                                               &t_cmd,
                                               SBLIB_CFG_XM_MAILBOXNR);
}

static void *
reverse_memcpy(void * Dest_p,
               const void * Src_p,
               size_t Size)
{
    char * dp = Dest_p;
    const char * sp = Src_p;

    sp += (Size - 1);
    while (Size--)
    {
        *dp++ = *sp--;
    }
    return Dest_p;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_EIP130_EcdsaVerify_Init
 */
void
SBHYBRID_EIP130_EcdsaVerify_Init(SBHYBRID_SymmContext_t * const Context_p,
                                 const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                 const SBIF_ECDSA_Signature_t * const Signature_p)
{
    Context_p->ParamsInBytes = sizeof(PublicKeyParameters);
#ifdef SBLIB_CFG_SM_COPY_CURVEPARAMS
    Context_p->Params = Context_p->PublicKeyParams_Buffer;
    c_memcpy(Context_p->Params, PublicKeyParameters, Context_p->ParamsInBytes);
#else
    Context_p->Params = PublicKeyParameters;
#endif

    // Create Public Key representation for pla1ntext asset load
    Context_p->PublicKey_Buffer[0] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 0);
    Context_p->PublicKey_Buffer[1] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 8);
    Context_p->PublicKey_Buffer[2] = 0;
    Context_p->PublicKey_Buffer[3] = 2;
    reverse_memcpy(&Context_p->PublicKey_Buffer[4],
                   PublicKey_p->Qx,
                   SBIF_ECDSA_BYTES);

    Context_p->PublicKey_Buffer[(SBIF_ECDSA_WORDS * 4) + 4] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 0);
    Context_p->PublicKey_Buffer[(SBIF_ECDSA_WORDS * 4) + 5] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 8);
    Context_p->PublicKey_Buffer[(SBIF_ECDSA_WORDS * 4) + 6] = 1;
    Context_p->PublicKey_Buffer[(SBIF_ECDSA_WORDS * 4) + 7] = 2;
    reverse_memcpy(&Context_p->PublicKey_Buffer[(SBIF_ECDSA_WORDS * 4) + 8],
                   PublicKey_p->Qy,
                   SBIF_ECDSA_BYTES);

    Context_p->PublicKeyInBytes = 8 + (2 * (SBIF_ECDSA_WORDS * 4));

    // Create Signature representation
    Context_p->Signature_Buffer[0] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 0);
    Context_p->Signature_Buffer[1] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 8);
    Context_p->Signature_Buffer[2] = 0;
    Context_p->Signature_Buffer[3] = 2;
    reverse_memcpy(&Context_p->Signature_Buffer[4],
                   Signature_p->r,
                   SBIF_ECDSA_BYTES);

    Context_p->Signature_Buffer[(SBIF_ECDSA_WORDS * 4) + 4] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 0);
    Context_p->Signature_Buffer[(SBIF_ECDSA_WORDS * 4) + 5] = (uint8_t)((SBIF_ECDSA_BYTES * 8) >> 8);
    Context_p->Signature_Buffer[(SBIF_ECDSA_WORDS * 4) + 6] = 1;
    Context_p->Signature_Buffer[(SBIF_ECDSA_WORDS * 4) + 7] = 2;
    reverse_memcpy(&Context_p->Signature_Buffer[(SBIF_ECDSA_WORDS * 4) + 8],
                   Signature_p->s,
                   SBIF_ECDSA_BYTES);

    Context_p->SignatureInBytes = 8 + (2 * (SBIF_ECDSA_WORDS * 4));


#ifdef ENABLE_KEY_INFO_LOGGING
    Log_HexDump("Public Key parameters", 0, Context_p->Params, Context_p->ParamsInBytes);
    Log_HexDump("Public Key", 0, Context_p->PublicKey_Buffer, Context_p->PublicKeyInBytes);
    Log_HexDump("Signature", 0, Context_p->Signature_Buffer, Context_p->SignatureInBytes);
#endif
    L_DEBUG(LF_SBHYBRID, "Successful");
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP130_EcdsaVerify_RunFsm
 *
 */
SB_Result_t
SBHYBRID_EIP130_EcdsaVerify_RunFsm(SBHYBRID_SymmContext_t * const Context_p)
{
    unsigned int Step = 0;              // FSM step
    SB_Result_t sbres;
    SB_Result_t pass_fail = SB_SUCCESS;

    if (!Context_p->HashInitialize && !Context_p->HashFinalize)
    {
        // Last hash block not yet handled
        return SBHYBRID_PENDING;
    }

    L_TRACE(LF_SBHYBRID, "Step %u", Step);
    for (;;)
    {
        if ((Step & 1) == 0)
        {
            // Command step
            if (EIP130_MailboxCanWriteToken(Context_p->Device_EIP130,
                                            SBLIB_CFG_XM_MAILBOXNR))
            {
                switch (Step)
                {
                case 0:
                    // Create asset for Public Key parameters
                    sbres = SBHYBRID_AssetCreate_SendCommand(Context_p,
                                                             (SBHYBRID_ACCESS_POLICY |
                                                              EIP130TOKEN_ASSET_POLICY_PUBLIC_KEY_PARAM),
                                                             Context_p->ParamsInBytes);
                    break;

                case 2:
                    // Load asset with Public Key parameters
                    sbres = SBHYBRID_AssetLoadPlaintext_SendCommand(Context_p,
                                                                    Context_p->ParamsAssetId,
                                                                    Context_p->Params,
                                                                    Context_p->ParamsInBytes);
                    break;

                case 4:
                    // Create asset for Public Key
                    sbres = SBHYBRID_AssetCreate_SendCommand(Context_p,
                                                             SBHYBRID_PUBLICKEY_POLICY,
                                                             Context_p->PublicKeyInBytes);
                    break;

                case 6:
                    // Load asset with Public Key
                    sbres = SBHYBRID_AssetLoadPlaintext_SendCommand(Context_p,
                                                                    Context_p->KeyAssetId,
                                                                    Context_p->PublicKey_Buffer,
                                                                    Context_p->PublicKeyInBytes);
                    break;

                case 8:
                    {
                        uint32_t HashTempAssetId = Context_p->HashTempAssetId;
                        if (Context_p->HashInitialize)
                        {
                            // Complete hash required (init -> final),
                            // so no temporary hash digest asset use
                            HashTempAssetId = 0;
                            L_DEBUG(LF_SBHYBRID, "Complete hash");
                        }
                        sbres = SBHYBRID_ECDSAVerify_SendCommand(Context_p,
                                                                 Context_p->KeyAssetId,
                                                                 Context_p->ParamsAssetId,
                                                                 HashTempAssetId,
                                                                 Context_p->Signature_Buffer,
                                                                 Context_p->SignatureInBytes,
                                                                 Context_p->HashData_p,
                                                                 Context_p->HashDataLength,
                                                                 Context_p->HashTotalLength);
                    }
                    break;

                case 10:
                    sbres = SBHYBRID_AssetDelete_SendCommand(Context_p,
                                                             Context_p->KeyAssetId);
                    break;

                case 12:
                    sbres = SBHYBRID_AssetDelete_SendCommand(Context_p,
                                                             Context_p->ParamsAssetId);
                    break;

                case 14:
                    if (!Context_p->HashInitialize)
                    {
                        // Clean-up used temporary hash digest asset
                        sbres = SBHYBRID_AssetDelete_SendCommand(Context_p,
                                                                 Context_p->HashTempAssetId);
                        break;
                    }
                    // ECDSA Verify sequence ready
                    L_DEBUG(LF_SBHYBRID, "Skipped HashTempAssetId delete");
                    return pass_fail;

                case 16:
                    // ECDSA Verify sequence ready
                    return pass_fail;

                default:
                    return SB_ERROR_HARDWARE;
                }

                if (sbres != SBHYBRID_PENDING)
                {
                    return SB_ERROR_HARDWARE;
                }
                Step++;
                L_TRACE(LF_SBHYBRID, "Step %u", Step);
            }
            else
            {
                // Command not yet ready
            }
        }
        else
        {
            // Response step
            if (EIP130_MailboxCanReadToken(Context_p->Device_EIP130,
                                           SBLIB_CFG_XM_MAILBOXNR))
            {
                switch (Step)
                {
                case 1:                 // Asset create
                                        // Validate response and read AssetId of Public Key parameters
                    sbres = SBHYBRID_AssetCreateSearch_ReadResult(Context_p,
                                                                  &Context_p->ParamsAssetId);
                    break;

                case 5:                 // Asset create
                                        // Validate response and read AssetId of Public Key
                    sbres = SBHYBRID_AssetCreateSearch_ReadResult(Context_p,
                                                                  &Context_p->KeyAssetId);
                    break;

                case 3:                 // Asset load
                case 7:                 // Asset load
                case 9:                 // ECDSA Verify
                case 11:                // Asset delete
                case 13:                // Asset delete
                case 15:                // Asset delete
                                        // Validate asset load/asset delete/ECDSA verify response
                    sbres = SBHYBRID_AssetLoad_ReadResult(Context_p);
                    break;

                default:
                    return SB_ERROR_HARDWARE;
                }

                // Clean-up asset references if needed
                switch (Step)
                {
                default:
                    break;
                case 11:
                    Context_p->KeyAssetId = 0;
                    break;
                case 13:
                    Context_p->ParamsAssetId = 0;
                    break;
                case 15:
                    Context_p->HashTempAssetId = 0;
                    break;
                }

                if (sbres != SB_SUCCESS)
                {
                    L_DEBUG(LF_SBHYBRID, "Step %u Result %d", Step, sbres);

                    // Control clean-up in case of an error
                    // - Select the next applicable step
                    switch (Step)
                    {
                    case 1:             // Asset create
                        pass_fail = SB_ERROR_HARDWARE;
                        Step = 14;
                        break;
                    case 3:             // Asset load
                    case 5:             // Asset create
                        pass_fail = SB_ERROR_HARDWARE;
                        Step = 12;
                        break;
                    case 7:             // Asset load
                        pass_fail = SB_ERROR_HARDWARE;
                        Step = 10;
                        break;
                    case 9:             // ECDSA Verify failed
                        pass_fail = SB_ERROR_VERIFICATION;
                        Step++;
                        break;
                    case 11:            // Asset delete
                    case 13:            // Asset delete
                    case 15:            // Asset delete
                        pass_fail = SB_ERROR_HARDWARE;
                        Step++;
                        break;
                    default:
                        return SB_ERROR_HARDWARE;
                    }
                }
                else
                {
                    // Simple goto the next step
                    Step++;
                }
                L_TRACE(LF_SBHYBRID, "Step %u", Step);
            }
            else
            {
                // Command not yet ready
            }
        }
    }
    // Will never be reached
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP130_EcdsaVerify_SetDigest
 *
 * This function is called when the Digest is available.
 * For the EIP130 this is a dummy function.
 */
void
SBHYBRID_EIP130_EcdsaVerify_SetDigest(SBHYBRID_SymmContext_t * const Context_p,
                                      uint8_t * Digest_p)
{
    IDENTIFIER_NOT_USED(Context_p);
    IDENTIFIER_NOT_USED(Digest_p);
}

#endif /* SBHYBRID_WITH_EIP130PK */
#endif /* SBHYBRID_WITH_EIP130 */

/* end of file sbhybrid_eip130_ecdsa_verify.c */
