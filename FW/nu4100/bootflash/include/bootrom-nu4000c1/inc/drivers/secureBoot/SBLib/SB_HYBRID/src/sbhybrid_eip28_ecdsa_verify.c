/* sbhybrid_eip28_ecdsa-verify.c
 *
 * Secure Boot ECDSA-Verify acceleration using EIP-28.
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

#ifndef __MODULE__
#define __MODULE__ "sbhybrid_eip28_ecdsa-verify.c"
#endif

#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_EIP28

#include "eip28.h"

/* P-224 curve parameters. */

#define SB_ECDSA_CFG_P224_p        \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x01

#define SB_ECDSA_CFG_P224_r        \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x16, 0xa2, \
    0xe0, 0xb8, 0xf0, 0x3e, 0x13, 0xdd, 0x29, 0x45, \
    0x5c, 0x5c, 0x2a, 0x3d

#define SB_ECDSA_CFG_P224_a         \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xfe

#define SB_ECDSA_CFG_P224_b        \
    0xb4, 0x05, 0x0a, 0x85, 0x0c, 0x04, 0xb3, 0xab, \
    0xf5, 0x41, 0x32, 0x56, 0x50, 0x44, 0xb0, 0xb7, \
    0xd7, 0xbf, 0xd8, 0xba, 0x27, 0x0b, 0x39, 0x43, \
    0x23, 0x55, 0xff, 0xb4

#define SB_ECDSA_CFG_P224_Gx       \
    0xb7, 0x0e, 0x0c, 0xbd, 0x6b, 0xb4, 0xbf, 0x7f, \
    0x32, 0x13, 0x90, 0xb9, 0x4a, 0x03, 0xc1, 0xd3, \
    0x56, 0xc2, 0x11, 0x22, 0x34, 0x32, 0x80, 0xd6, \
    0x11, 0x5c, 0x1d, 0x21

#define SB_ECDSA_CFG_P224_Gy       \
    0xbd, 0x37, 0x63, 0x88, 0xb5, 0xf7, 0x23, 0xfb, \
    0x4c, 0x22, 0xdf, 0xe6, 0xcd, 0x43, 0x75, 0xa0, \
    0x5a, 0x07, 0x47, 0x64, 0x44, 0xd5, 0x81, 0x99, \
    0x85, 0x00, 0x7e, 0x34

/* P-256 curve parameters. */

#define SB_ECDSA_CFG_P256_p        \
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff

#define SB_ECDSA_CFG_P256_r        \
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xbc, 0xe6, 0xfa, 0xad, 0xa7, 0x17, 0x9e, 0x84, \
    0xf3, 0xb9, 0xca, 0xc2, 0xfc, 0x63, 0x25, 0x51

#define SB_ECDSA_CFG_P256_a        \
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x01, \
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc

#define SB_ECDSA_CFG_P256_b        \
    0x5a, 0xc6, 0x35, 0xd8, 0xaa, 0x3a, 0x93, 0xe7, \
    0xb3, 0xeb, 0xbd, 0x55, 0x76, 0x98, 0x86, 0xbc, \
    0x65, 0x1d, 0x06, 0xb0, 0xcc, 0x53, 0xb0, 0xf6, \
    0x3b, 0xce, 0x3c, 0x3e, 0x27, 0xd2, 0x60, 0x4b

#define SB_ECDSA_CFG_P256_Gx       \
    0x6b, 0x17, 0xd1, 0xf2, 0xe1, 0x2c, 0x42, 0x47, \
    0xf8, 0xbc, 0xe6, 0xe5, 0x63, 0xa4, 0x40, 0xf2, \
    0x77, 0x03, 0x7d, 0x81, 0x2d, 0xeb, 0x33, 0xa0, \
    0xf4, 0xa1, 0x39, 0x45, 0xd8, 0x98, 0xc2, 0x96

#define SB_ECDSA_CFG_P256_Gy       \
    0x4f, 0xe3, 0x42, 0xe2, 0xfe, 0x1a, 0x7f, 0x9b, \
    0x8e, 0xe7, 0xeb, 0x4a, 0x7c, 0x0f, 0x9e, 0x16, \
    0x2b, 0xce, 0x33, 0x57, 0x6b, 0x31, 0x5e, 0xce, \
    0xcb, 0xb6, 0x40, 0x68, 0x37, 0xbf, 0x51, 0xf5

/* P-384 curve parameters. */

#define SB_ECDSA_CFG_P384_p        \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, \
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff

#define SB_ECDSA_CFG_P384_r        \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xc7, 0x63, 0x4d, 0x81, 0xf4, 0x37, 0x2d, 0xdf, \
    0x58, 0x1a, 0x0d, 0xb2, 0x48, 0xb0, 0xa7, 0x7a, \
    0xec, 0xec, 0x19, 0x6a, 0xcc, 0xc5, 0x29, 0x73

#define SB_ECDSA_CFG_P384_a        \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, \
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xfc

#define SB_ECDSA_CFG_P384_b        \
    0xb3, 0x31, 0x2f, 0xa7, 0xe2, 0x3e, 0xe7, 0xe4, \
    0x98, 0x8e, 0x05, 0x6b, 0xe3, 0xf8, 0x2d, 0x19, \
    0x18, 0x1d, 0x9c, 0x6e, 0xfe, 0x81, 0x41, 0x12, \
    0x03, 0x14, 0x08, 0x8f, 0x50, 0x13, 0x87, 0x5a, \
    0xc6, 0x56, 0x39, 0x8d, 0x8a, 0x2e, 0xd1, 0x9d, \
    0x2a, 0x85, 0xc8, 0xed, 0xd3, 0xec, 0x2a, 0xef

#define SB_ECDSA_CFG_P384_Gx       \
    0xaa, 0x87, 0xca, 0x22, 0xbe, 0x8b, 0x05, 0x37, \
    0x8e, 0xb1, 0xc7, 0x1e, 0xf3, 0x20, 0xad, 0x74, \
    0x6e, 0x1d, 0x3b, 0x62, 0x8b, 0xa7, 0x9b, 0x98, \
    0x59, 0xf7, 0x41, 0xe0, 0x82, 0x54, 0x2a, 0x38, \
    0x55, 0x02, 0xf2, 0x5d, 0xbf, 0x55, 0x29, 0x6c, \
    0x3a, 0x54, 0x5e, 0x38, 0x72, 0x76, 0x0a, 0xb7

#define SB_ECDSA_CFG_P384_Gy       \
    0x36, 0x17, 0xde, 0x4a, 0x96, 0x26, 0x2c, 0x6f, \
    0x5d, 0x9e, 0x98, 0xbf, 0x92, 0x92, 0xdc, 0x29, \
    0xf8, 0xf4, 0x1d, 0xbd, 0x28, 0x9a, 0x14, 0x7c, \
    0xe9, 0xda, 0x31, 0x13, 0xb5, 0xf0, 0xb8, 0xc0, \
    0x0a, 0x60, 0xb1, 0xce, 0x1d, 0x7e, 0x81, 0x9d, \
    0x7a, 0x43, 0x1d, 0x7c, 0x90, 0xea, 0x0e, 0x5f

/* P-521 curve parameters. */

#define SB_ECDSA_CFG_P521_p        \
    0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff

#define SB_ECDSA_CFG_P521_r        \
    0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xfa, 0x51, 0x86, 0x87, 0x83, 0xbf, 0x2f, \
    0x96, 0x6b, 0x7f, 0xcc, 0x01, 0x48, 0xf7, 0x09, \
    0xa5, 0xd0, 0x3b, 0xb5, 0xc9, 0xb8, 0x89, 0x9c, \
    0x47, 0xae, 0xbb, 0x6f, 0xb7, 0x1e, 0x91, 0x38, \
    0x64, 0x09

#define SB_ECDSA_CFG_P521_a        \
    0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, \
    0xff, 0xfc

#define SB_ECDSA_CFG_P521_b        \
    0x00, 0x51, 0x95, 0x3e, 0xb9, 0x61, 0x8e, 0x1c, \
    0x9a, 0x1f, 0x92, 0x9a, 0x21, 0xa0, 0xb6, 0x85, \
    0x40, 0xee, 0xa2, 0xda, 0x72, 0x5b, 0x99, 0xb3, \
    0x15, 0xf3, 0xb8, 0xb4, 0x89, 0x91, 0x8e, 0xf1, \
    0x09, 0xe1, 0x56, 0x19, 0x39, 0x51, 0xec, 0x7e, \
    0x93, 0x7b, 0x16, 0x52, 0xc0, 0xbd, 0x3b, 0xb1, \
    0xbf, 0x07, 0x35, 0x73, 0xdf, 0x88, 0x3d, 0x2c, \
    0x34, 0xf1, 0xef, 0x45, 0x1f, 0xd4, 0x6b, 0x50, \
    0x3f, 0x00

#define SB_ECDSA_CFG_P521_Gx       \
    0x00, 0xc6, 0x85, 0x8e, 0x06, 0xb7, 0x04, 0x04, \
    0xe9, 0xcd, 0x9e, 0x3e, 0xcb, 0x66, 0x23, 0x95, \
    0xb4, 0x42, 0x9c, 0x64, 0x81, 0x39, 0x05, 0x3f, \
    0xb5, 0x21, 0xf8, 0x28, 0xaf, 0x60, 0x6b, 0x4d, \
    0x3d, 0xba, 0xa1, 0x4b, 0x5e, 0x77, 0xef, 0xe7, \
    0x59, 0x28, 0xfe, 0x1d, 0xc1, 0x27, 0xa2, 0xff, \
    0xa8, 0xde, 0x33, 0x48, 0xb3, 0xc1, 0x85, 0x6a, \
    0x42, 0x9b, 0xf9, 0x7e, 0x7e, 0x31, 0xc2, 0xe5, \
    0xbd, 0x66

#define SB_ECDSA_CFG_P521_Gy       \
    0x01, 0x18, 0x39, 0x29, 0x6a, 0x78, 0x9a, 0x3b, \
    0xc0, 0x04, 0x5c, 0x8a, 0x5f, 0xb4, 0x2c, 0x7d, \
    0x1b, 0xd9, 0x98, 0xf5, 0x44, 0x49, 0x57, 0x9b, \
    0x44, 0x68, 0x17, 0xaf, 0xbd, 0x17, 0x27, 0x3e, \
    0x66, 0x2c, 0x97, 0xee, 0x72, 0x99, 0x5e, 0xf4, \
    0x26, 0x40, 0xc5, 0x50, 0xb9, 0x01, 0x3f, 0xad, \
    0x07, 0x61, 0x35, 0x3c, 0x70, 0x86, 0xa2, 0x72, \
    0xc2, 0x40, 0x88, 0xbe, 0x94, 0x76, 0x9f, 0xd1, \
    0x66, 0x50

typedef struct
{
    uint8_t p[SBIF_ECDSA_BYTES];        /** p. */
    uint8_t a[SBIF_ECDSA_BYTES];        /** a. */
    uint8_t b[SBIF_ECDSA_BYTES];        /** b. */
    uint8_t n[SBIF_ECDSA_BYTES];        /** n. */
    uint8_t Gx[SBIF_ECDSA_BYTES];       /** Gx. */
    uint8_t Gy[SBIF_ECDSA_BYTES];       /** Gy. */
}
SBHYBRID_ECDSA_Domain_t;

static const SBHYBRID_ECDSA_Domain_t SBHYBRID_ECDSA_Domain =
{
#if SBIF_ECDSA_WORDS == 7
    { SB_ECDSA_CFG_P224_p },
    { SB_ECDSA_CFG_P224_a },
    { SB_ECDSA_CFG_P224_b },
    { SB_ECDSA_CFG_P224_r },
    { SB_ECDSA_CFG_P224_Gx },
    { SB_ECDSA_CFG_P224_Gy }
#endif
#if SBIF_ECDSA_WORDS == 8
    { SB_ECDSA_CFG_P256_p },
    { SB_ECDSA_CFG_P256_a },
    { SB_ECDSA_CFG_P256_b },
    { SB_ECDSA_CFG_P256_r },
    { SB_ECDSA_CFG_P256_Gx },
    { SB_ECDSA_CFG_P256_Gy }
#endif
#if SBIF_ECDSA_WORDS == 12
    { SB_ECDSA_CFG_P384_p },
    { SB_ECDSA_CFG_P384_a },
    { SB_ECDSA_CFG_P384_b },
    { SB_ECDSA_CFG_P384_r },
    { SB_ECDSA_CFG_P384_Gx },
    { SB_ECDSA_CFG_P384_Gy }
#endif
#if SBIF_ECDSA_WORDS == 17
    { SB_ECDSA_CFG_P521_p },
    { SB_ECDSA_CFG_P521_a },
    { SB_ECDSA_CFG_P521_b },
    { SB_ECDSA_CFG_P521_r },
    { SB_ECDSA_CFG_P521_Gx },
    { SB_ECDSA_CFG_P521_Gy }
#endif
};


#if (SBIF_ECDSA_WORDS == 7) || (SBIF_ECDSA_WORDS == 8)
#define SBHYBRID_EIP28_SIZE                   8
#define SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE 10
#elif SBIF_ECDSA_WORDS == 12
#define SBHYBRID_EIP28_SIZE                  12
#define SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE 14
#elif SBIF_ECDSA_WORDS == 17
#define SBHYBRID_EIP28_SIZE                  18
#define SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE 20
#else
#error "Unsupported SBIF_ECDSA_WORDS"
#endif

#define SBHYBRID_EIP28_PRO_ZERO_OFFSET    0
#define SBHYBRID_EIP28_PRO_E_OFFSET       ((SBHYBRID_EIP28_PRO_ZERO_OFFSET) + (SBHYBRID_EIP28_SIZE))
#define SBHYBRID_EIP28_PRO_R_OFFSET       ((SBHYBRID_EIP28_PRO_E_OFFSET) + (SBHYBRID_EIP28_SIZE))
#define SBHYBRID_EIP28_PRO_S_OFFSET       ((SBHYBRID_EIP28_PRO_R_OFFSET) + (SBHYBRID_EIP28_SIZE))
#define SBHYBRID_EIP28_PRO_P_OFFSET       ((SBHYBRID_EIP28_PRO_S_OFFSET) + (SBHYBRID_EIP28_SIZE))
#define SBHYBRID_EIP28_PRO_A_OFFSET       ((SBHYBRID_EIP28_PRO_P_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_B_OFFSET       ((SBHYBRID_EIP28_PRO_A_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_N_OFFSET       ((SBHYBRID_EIP28_PRO_B_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_QU_X_OFFSET    ((SBHYBRID_EIP28_PRO_N_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_QU_Y_OFFSET    ((SBHYBRID_EIP28_PRO_QU_X_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_G_X_OFFSET     ((SBHYBRID_EIP28_PRO_QU_Y_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_G_Y_OFFSET     ((SBHYBRID_EIP28_PRO_G_X_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_W_OFFSET       ((SBHYBRID_EIP28_PRO_G_Y_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_U2_OFFSET      ((SBHYBRID_EIP28_PRO_W_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_U1_OFFSET      ((SBHYBRID_EIP28_PRO_U2_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_U2QU_X_OFFSET  ((SBHYBRID_EIP28_PRO_U1_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_U2QU_Y_OFFSET  ((SBHYBRID_EIP28_PRO_U2QU_X_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_U1G_X_OFFSET   ((SBHYBRID_EIP28_PRO_U2QU_Y_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_U1G_Y_OFFSET   ((SBHYBRID_EIP28_PRO_U1G_X_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_POINT_X_OFFSET ((SBHYBRID_EIP28_PRO_U1G_Y_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_POINT_Y_OFFSET ((SBHYBRID_EIP28_PRO_POINT_X_OFFSET) + (SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE))
#define SBHYBRID_EIP28_PRO_SCRATCH_OFFSET SBHYBRID_EIP28_PRO_U1G_X_OFFSET

// steps:
//  0 sanity-check inputs
//  1 w = s^-1 % n        (modinv)
//  2 scratch = r * w
//  3 u2 = scratch % n
//  4 u2QU_x:y = eccmul(u2, p, QU_x:y)
//  5 scratch = e * w
//  6 u1 = scratch % n
//  7 u1G_x:y = eccmul(u1, p, G_x:y)
//  8 point_x:y = eccadd(u1G_x:y, u2QU_x:y)
//  9 scratch = point_x % n
// 10 compare(scratch, r)

typedef enum
{
    PRO_ZERO     = SBHYBRID_EIP28_PRO_ZERO_OFFSET,      // SBHYBRID_EIP28_SIZE = maximum size for r, s, e, etc.
    PRO_e        = SBHYBRID_EIP28_PRO_E_OFFSET,         // digest (input)
    PRO_r        = SBHYBRID_EIP28_PRO_R_OFFSET,         // signature component (input)
    PRO_s        = SBHYBRID_EIP28_PRO_S_OFFSET,         // signature component (input)

    // SBHYBRID_EIP28_DOMAIN_COMPONENT_SIZE = M = maximum size for domain components and public key

    PRO_p        = SBHYBRID_EIP28_PRO_P_OFFSET,         // domain component (input)
    PRO_a        = SBHYBRID_EIP28_PRO_A_OFFSET,         // domain component (input)
    PRO_b        = SBHYBRID_EIP28_PRO_B_OFFSET,         // domain component (input)
    PRO_n        = SBHYBRID_EIP28_PRO_N_OFFSET,         // domain component (input)
    PRO_QU_x     = SBHYBRID_EIP28_PRO_QU_X_OFFSET,      // public key component (input)
    PRO_QU_y     = SBHYBRID_EIP28_PRO_QU_Y_OFFSET,      // public key component (input)
    PRO_G_x      = SBHYBRID_EIP28_PRO_G_X_OFFSET,       // domain component (input)
    PRO_G_y      = SBHYBRID_EIP28_PRO_G_Y_OFFSET,       // domain component (input)

    PRO_w        = SBHYBRID_EIP28_PRO_W_OFFSET,         // output from ModInv operation
                                                        // also workspace = 12 + (5 * M) + 1

    PRO_u2       = SBHYBRID_EIP28_PRO_U2_OFFSET,        // output from Modulo operation
    PRO_u1       = SBHYBRID_EIP28_PRO_U1_OFFSET,        // output from Modulo operation

    PRO_u2QU_x   = SBHYBRID_EIP28_PRO_U2QU_X_OFFSET,    // output for ECC-Mul operation
    PRO_u2QU_y   = SBHYBRID_EIP28_PRO_U2QU_Y_OFFSET,    // also workspace = 20 + (20 * M)

    PRO_u1G_x    = SBHYBRID_EIP28_PRO_U1G_X_OFFSET,     // output for ECC-Mul operation
    PRO_u1G_y    = SBHYBRID_EIP28_PRO_U1G_Y_OFFSET,     // also workspace = 20 + (20 * M)

    PRO_point_x  = SBHYBRID_EIP28_PRO_POINT_X_OFFSET,   // output for ECC-Add operation
    PRO_point_y  = SBHYBRID_EIP28_PRO_POINT_Y_OFFSET,   // also workspace = 20 + (20 * M)

    PRO_scratch  = SBHYBRID_EIP28_PRO_SCRATCH_OFFSET    // output from Multiply (and Modulus)
                                                        // also workspace = (M * 2) + 6
}
SBHYBRID_EIP28_PKA_Offset_t;


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_Write_OctetString
 *
 * Write octet string formatted big integer to given WordOffset in PKA RAM.
 */
static inline void
SBHYBRID_EIP28_Write_OctetString(EIP28_IOArea_t * const IOArea_p,
                                 const SBHYBRID_EIP28_PKA_Offset_t WordOffset,
                                 const uint32_t FillWords,
                                 const uint8_t * const Bytes_p,
                                 const uint32_t ByteCount)
{
    EIP28_BigUInt_t BigUInt;
    EIP28_Status_t EIP28_Status;

    L_TRACE(LF_SBHYBRID,
            "Writing octet string to PKARAM, "
            "WordOffset = %u, FillWords = %u, "
            "Bytes_p %p, ByteCount %u",
            WordOffset,
            (unsigned int)FillWords,
            Bytes_p,
            (unsigned int)ByteCount);

    PRECONDITION(FillWords >= (ByteCount >> 2));

    BigUInt.StoreAsMSB = true;
    BigUInt.Bytes_p = (uint8_t *)Bytes_p;
    BigUInt.ByteCount = ByteCount;

    EIP28_Status = EIP28_Memory_PutBigUInt_CALLATOMIC(IOArea_p,
                                                      WordOffset,
                                                      FillWords,
                                                      &BigUInt,
                                                      NULL/* LastWordUsed_p: */);

    ASSERT(EIP28_Status == EIP28_STATUS_OK);
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_Write_EccOs
 *
 * Write octet string formatted big number to PKA RAM with proper length
 * padding for ECC values.
 */
static inline void
SBHYBRID_EIP28_Write_EccOs(EIP28_IOArea_t * const IOArea_p,
                           SBHYBRID_EIP28_PKA_Offset_t WordOffset,
                           const uint8_t * const Bytes_p,
                           uint32_t ByteCount)
{
    uint32_t FillWords;

    FillWords = (ByteCount + 3) >> 2;
    FillWords = ((FillWords + 1) & ~1) + 2;

    L_TRACE(LF_SBHYBRID,
            "Writing EccOs, setting FillWords to %u for ByteCount %u.",
            (unsigned int)FillWords,
            (unsigned int)ByteCount);

    SBHYBRID_EIP28_Write_OctetString(IOArea_p,
                                     WordOffset,
                                     FillWords,
                                     Bytes_p,
                                     ByteCount);
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_Write_Zero
 *
 * Write vector of given WordCount of zeroes to given WordOffset in PKA RAM.
 */
static inline void
SBHYBRID_EIP28_Write_Zero(EIP28_IOArea_t * const IOArea_p,
                          const SBHYBRID_EIP28_PKA_Offset_t WordOffset,
                          const uint32_t WordCount)
{
    EIP28_BigUInt_t BigUInt;
    EIP28_Status_t EIP28_Status;
    const uint8_t ZeroByte = 0;

    L_TRACE(LF_SBHYBRID,
            "Writing %u zerowords to PKA RAM WordOffset %u.",
            (unsigned int)WordCount,
            (unsigned int)WordOffset);

    BigUInt.StoreAsMSB = true;
    BigUInt.Bytes_p = (uint8_t *)&ZeroByte;
    BigUInt.ByteCount = 1;

    EIP28_Status = EIP28_Memory_PutBigUInt_CALLATOMIC(IOArea_p,
                                                      WordOffset,
                                                      WordCount,
                                                      &BigUInt,
                                                      NULL/* LastWordUsed_p: */);

    ASSERT(EIP28_Status == EIP28_STATUS_OK);
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_EcdsaVerify_Init
 *
 * Initialises Verification state structure and procedure.
 * Writes input values to fixed offsets in EIP28 PKA RAM.
 */
void
SBHYBRID_EIP28_EcdsaVerify_Init(SBHYBRID_EcdsaContext_t * const Verify_p,
                                const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                const SBIF_ECDSA_Signature_t * const Signature_p)
{
    const SBHYBRID_ECDSA_Domain_t * const Domain_p = &SBHYBRID_ECDSA_Domain;
    EIP28_IOArea_t * const IOArea_p = &Verify_p->EIP28_IOArea;
    const uint32_t EcdsaWords = SBIF_ECDSA_WORDS;
    const uint32_t EcdsaBytes = SBIF_ECDSA_BYTES;

    L_TRACE(LF_SBHYBRID,
            "EIP28 ECDSA Verify init, IOArea_p = %p, Verify_p = %p, "
            "Domain_p = %p, PublicKey_p = %p, Signature_p = %p.",
            (void *)IOArea_p,
            (void *)Verify_p,
            (void *)Domain_p,
            (void *)PublicKey_p,
            (void *)Signature_p);

    /* Load zero value to check against */
    L_TRACE(LF_SBHYBRID, "Write Zero value.");
    SBHYBRID_EIP28_Write_Zero(IOArea_p, PRO_ZERO, EcdsaWords);

    /* Load signature values */
    L_TRACE(LF_SBHYBRID, "Write signature r value.");
    SBHYBRID_EIP28_Write_OctetString(IOArea_p, PRO_r, EcdsaWords,
                                     Signature_p->r, EcdsaBytes);

    L_TRACE(LF_SBHYBRID, "Write signature s value.");
    SBHYBRID_EIP28_Write_OctetString(IOArea_p, PRO_s, EcdsaWords,
                                     Signature_p->s, EcdsaBytes);

    /* load curve parameters */
    L_TRACE(LF_SBHYBRID, "Write domain n value.");
    SBHYBRID_EIP28_Write_OctetString(IOArea_p, PRO_n, EcdsaWords,
                                     Domain_p->n, EcdsaBytes);

    L_TRACE(LF_SBHYBRID, "Write domain p value.");
    SBHYBRID_EIP28_Write_EccOs(IOArea_p, PRO_p, Domain_p->p, EcdsaBytes);

    L_TRACE(LF_SBHYBRID, "Write domain a value.");
    SBHYBRID_EIP28_Write_OctetString(IOArea_p, PRO_a, EcdsaWords,
                                     Domain_p->a, EcdsaBytes);

    L_TRACE(LF_SBHYBRID, "Write domain b value.");
    SBHYBRID_EIP28_Write_EccOs(IOArea_p, PRO_b, Domain_p->b, EcdsaBytes);

    /* load base point G */
    L_TRACE(LF_SBHYBRID, "Write domain g.x value.");
    SBHYBRID_EIP28_Write_EccOs(IOArea_p, PRO_G_x, Domain_p->Gx, EcdsaBytes);

    L_TRACE(LF_SBHYBRID, "Write domain g.y value.");
    SBHYBRID_EIP28_Write_EccOs(IOArea_p, PRO_G_y, Domain_p->Gy, EcdsaBytes);

    /* load public key QU */
    L_TRACE(LF_SBHYBRID, "Write public key q.x value.");
    SBHYBRID_EIP28_Write_EccOs(IOArea_p, PRO_QU_x, PublicKey_p->Qx, EcdsaBytes);

    L_TRACE(LF_SBHYBRID, "Write public key q.y value.");
    SBHYBRID_EIP28_Write_EccOs(IOArea_p, PRO_QU_y, PublicKey_p->Qy, EcdsaBytes);

    /* We dont need e value, the digest, yet.  A lot of computation
       for ECDSA verify can be done before e value is needed.
    */
    Verify_p->Value_e_p = NULL;
    Verify_p->Step = 0;

    L_TRACE(LF_SBHYBRID, "EIP28 ECDSA Verify init successful.");
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_EcdsaVerify_SetDigest
 *
 * This function is called when the Digest is available.
 * The Digest_p pointer will be stored and used by the FSM.
 */
void
SBHYBRID_EIP28_EcdsaVerify_SetDigest(SBHYBRID_EcdsaContext_t * const Verify_p,
                                     uint8_t * Digest_p) // length SBHYBRID_DIGEST_BYTES
{
    Verify_p->Value_e_p = Digest_p;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_CompareResult_LessThan
 *
 * Returns true is result of an EIP28 COMPARE operation was LESSTHAN,
 *        false otherwise.
 */
static inline bool
SBHYBRID_EIP28_CompareResult_LessThan(EIP28_IOArea_t * const IOArea_p)
{
    EIP28_CompareResult_t Result;
    bool LessThan;

    EIP28_ReadResult_Compare(IOArea_p, &Result);

    LessThan = (Result == EIP28_COMPARERESULT_A_LESSTHAN_B);

    L_TRACE(LF_SBHYBRID,
            "EIP28 compare result %s lessthan.",
            (LessThan ? "is" : "is not"));

    return LessThan;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_Ecc_ResultValid
 *
 * Return true if result of ECC operation in EIP28 gave a valid result.
 * That is, result value is not undefined and not point at infinity.
 */
static inline bool
SBHYBRID_EIP28_Ecc_ResultValid(EIP28_IOArea_t * const IOArea_p)
{
    bool IsAtInfinity;
    bool IsUndefined;

    EIP28_ReadResult_ECCPoint_Status(IOArea_p, &IsAtInfinity, &IsUndefined);

    L_TRACE(LF_SBHYBRID,
            "Ecc result %s at infinity and %s undefined.",
            (IsAtInfinity ? "is" : "is not"),
            (IsUndefined ? "is" : "is not"));

    return !(IsAtInfinity || IsUndefined);
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP28_EcdsaVerify_RunFsm
 *
 * Advance ECDSA verification procedure. Polls the EIP28 to check if the
 * operation is still running. If not, checks the result and starts the next
 * operation and returns SBHYBRID_PENDING.
 * When verification procedure has completed successfully return SB_SUCCESS.
 * Other return values denote an error.
 */
SB_Result_t
SBHYBRID_EIP28_EcdsaVerify_RunFsm(SBHYBRID_EcdsaContext_t * const Verify_p)
{
    EIP28_IOArea_t * const IOArea_p = &Verify_p->EIP28_IOArea;
    EIP28_Status_t EIP28_Status = EIP28_STATUS_OK;
    const unsigned int EcdsaWords = SBIF_ECDSA_WORDS;
    SB_Result_t res = SBHYBRID_PENDING;

    unsigned int result_len = 0;
    uint32_t     next_step = 0;
    bool IsReady;

    EIP28_CheckIfDone(IOArea_p, &IsReady);
    if (IsReady)
    {
        next_step = Verify_p->Step + 1;

        // in step 9 we can only proceed when the digest is available
        if (next_step == 9 && Verify_p->Value_e_p == NULL)
        {
            next_step = 0;
        }
        else
        {
            Verify_p->Step++;
        }
    }

    if (next_step != 0)
    {
        /*
          This function is called in polling manner so print traces
          log only when EIP28 is ready and we can do something.
        */
        L_TRACE(LF_SBHYBRID, "FSM step %u", next_step);
    }

    switch (next_step)
    {
    case 0:
        break;

    case 1:
        L_TRACE(LF_SBHYBRID,
                "EIP28 Compare ZERO %d, r %d, length %u.",
                PRO_ZERO, PRO_r, EcdsaWords);

        /* compare 0 < r */
        EIP28_Status = EIP28_StartOp_Compare_AcmpB_CALLATOMIC(IOArea_p,
                                                              PRO_ZERO,
                                                              PRO_r,
                                                              EcdsaWords);
        break;

    case 2:
        /* check that 0 < r */
        if (!SBHYBRID_EIP28_CompareResult_LessThan(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: 0 not less than r.");
            res = SB_ERROR_VERIFICATION;
        }

        /* compare 0 < s */
        if (res == SBHYBRID_PENDING)
        {
            L_TRACE(LF_SBHYBRID,
                    "EIP28 Compare ZERO %d, s %d, length %u.",
                    PRO_ZERO, PRO_s, EcdsaWords);

            EIP28_Status = EIP28_StartOp_Compare_AcmpB_CALLATOMIC(IOArea_p,
                                                                  PRO_ZERO,
                                                                  PRO_s,
                                                                  EcdsaWords);
        }
        break;

    case 3:
        /* check that 0 < s */
        if (!SBHYBRID_EIP28_CompareResult_LessThan(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: 0 not less than s.");
            res = SB_ERROR_VERIFICATION;
        }

        /* compare s < n */
        if (res == SBHYBRID_PENDING)
        {
            L_TRACE(LF_SBHYBRID,
                    "EIP28 Compare s %d, n %d, length %u.",
                    PRO_s, PRO_n, EcdsaWords);

            EIP28_Status = EIP28_StartOp_Compare_AcmpB_CALLATOMIC(IOArea_p,
                                                                  PRO_s,
                                                                  PRO_n,
                                                                  EcdsaWords);
        }
        break;

    case 4:
        /* check that s < n */
        if (!SBHYBRID_EIP28_CompareResult_LessThan(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: s not less than n.");
            res = SB_ERROR_VERIFICATION;
        }

        /* compare r < n */
        if (res == SBHYBRID_PENDING)
        {
            L_TRACE(LF_SBHYBRID,
                    "EIP28 Compare r %d, n %d, length %u.",
                    PRO_r, PRO_n, EcdsaWords);

            EIP28_Status = EIP28_StartOp_Compare_AcmpB_CALLATOMIC(IOArea_p,
                                                                  PRO_r,
                                                                  PRO_n,
                                                                  EcdsaWords);
        }
        break;

    case 5:
        /* check that r < n */
        if (!SBHYBRID_EIP28_CompareResult_LessThan(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: r not less than n.");
            res = SB_ERROR_VERIFICATION;
        }

        /* compute w = s^-1 (mod n) */
        if (res == SBHYBRID_PENDING)
        {
            L_TRACE(LF_SBHYBRID,
                    "EIP28 ModInv s %d, s_len %u, n %d, n_len %u, result %d.",
                    PRO_s, EcdsaWords, PRO_n, EcdsaWords, PRO_w);

            EIP28_Status = EIP28_StartOp_ModInv_invAmodB_CALLATOMIC(IOArea_p,
                                                                    PRO_s,
                                                                    PRO_n,
                                                                    EcdsaWords,
                                                                    EcdsaWords,
                                                                    PRO_w);
        }
        break;

    case 6:
        {
            bool ModInv_IsUndefined;

            EIP28_ReadResult_ModInv_Status(IOArea_p, &ModInv_IsUndefined);
            if (ModInv_IsUndefined)
            {
                L_DEBUG(LF_SBHYBRID,
                        "Verification failed: s^(-1) (mod n) undefined.");
                res = SB_ERROR_VERIFICATION;
            }
        }

        if (res == SBHYBRID_PENDING)
        {
            EIP28_ReadResult_WordCount_CALLATOMIC(IOArea_p, &result_len);
            if (result_len == 0)
            {
                L_DEBUG(LF_SBHYBRID,
                        "Verification failed: w = s^(-1) (mod n) = 0.");

                res = SB_ERROR_VERIFICATION;
            }
        }

        /* compute r * w */
        if (res == SBHYBRID_PENDING)
        {
            Verify_p->Value_w_Len = result_len;

            L_TRACE(LF_SBHYBRID,
                    "EIP28 MUL r %d, r_len %u, w %d, w_len %u, result %d.",
                    PRO_r, EcdsaWords,
                    PRO_w, Verify_p->Value_w_Len,
                    PRO_scratch);

            EIP28_Status = EIP28_StartOp_Multiply_AmulB_CALLATOMIC(IOArea_p,
                                                                   PRO_r,
                                                                   PRO_w,
                                                                   EcdsaWords,
                                                                   Verify_p->Value_w_Len,
                                                                   PRO_scratch);
        }
        break;

    case 7:
        if (res == SBHYBRID_PENDING)
        {
            EIP28_ReadResult_WordCount_CALLATOMIC(IOArea_p, &result_len);
        }

        if (result_len == 0)
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: r * w = 0.");
            res = SB_ERROR_VERIFICATION;
        }

        if (res == SBHYBRID_PENDING)
        {
            /*  u2 = (r * w) (mod p) */

            L_TRACE(LF_SBHYBRID,
                    "EIP28 MOD scratch %d length %u, n %d length %u, "
                    "result u2 %d",
                    PRO_scratch, result_len, PRO_n, EcdsaWords, PRO_u2);

            EIP28_Status = EIP28_StartOp_Modulo_AmodB_CALLATOMIC(IOArea_p,
                                                                 PRO_scratch,
                                                                 PRO_n,
                                                                 result_len,
                                                                 EcdsaWords,
                                                                 PRO_u2);
        }
        break;

    case 8:
        if (res == SBHYBRID_PENDING)
        {
            EIP28_ReadResult_RemainderWordCount_CALLATOMIC(IOArea_p,
                                                           &result_len);
        }

        if (result_len == 0)
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: (r * w) mod n = 0.");
            res = SB_ERROR_VERIFICATION;
        }

        if (res == SBHYBRID_PENDING)
        {
            /* ecc multiply: u2 * QU */

            L_TRACE(LF_SBHYBRID,
                    "EIP28 ECCMUL u2 %d length %u, QU %d length %u, "
                    "curve at %d, result u2QU %d",
                    PRO_u2, result_len, PRO_QU_x, EcdsaWords, PRO_p,
                    PRO_u2QU_x);

            EIP28_Status = EIP28_StartOp_EccMul_Affine_kmulC_CALLATOMIC(IOArea_p,
                                                                        PRO_u2,
                                                                        PRO_p,
                                                                        PRO_QU_x,
                                                                        result_len,
                                                                        EcdsaWords,
                                                                        PRO_u2QU_x);
        }
        break;

    case 9:
        ASSERT(Verify_p->Value_e_p != NULL);

        if (!SBHYBRID_EIP28_Ecc_ResultValid(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: u2 x QU not valid.");
            res = SB_ERROR_VERIFICATION;
        }

        if (res == SBHYBRID_PENDING)
        {
            L_TRACE(LF_SBHYBRID, "Write digest e value.");

            /* load e (the digest) */
            SBHYBRID_EIP28_Write_OctetString(IOArea_p,
                                             PRO_e,
                                             EcdsaWords,
                                             Verify_p->Value_e_p,
                                             SBHYBRID_DIGEST_BYTES);

            /* compute e * w */
            L_TRACE(LF_SBHYBRID,
                    "EIP28 MUL e %d, e_len %u, w %d, w_len %u, result %d.",
                    PRO_e, EcdsaWords,
                    PRO_w, Verify_p->Value_w_Len,
                    PRO_scratch);

            EIP28_Status = EIP28_StartOp_Multiply_AmulB_CALLATOMIC(IOArea_p,
                                                                   PRO_e,
                                                                   PRO_w,
                                                                   EcdsaWords,
                                                                   Verify_p->Value_w_Len,
                                                                   PRO_scratch);
        }
        break;

    case 10:
        if (res == SBHYBRID_PENDING)
        {
            EIP28_ReadResult_WordCount_CALLATOMIC(IOArea_p, &result_len);
        }

        if (result_len == 0)
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: e * w = 0.");
            res = SB_ERROR_VERIFICATION;
        }

        if (res == SBHYBRID_PENDING)
        {
            /*  u1 = (e * w) (mod p) */
            L_TRACE(LF_SBHYBRID,
                    "EIP28 MOD scratch %d length %u, n %d length %u, "
                    "result u1 %d",
                    PRO_scratch, result_len, PRO_n, EcdsaWords, PRO_u1);

            EIP28_Status = EIP28_StartOp_Modulo_AmodB_CALLATOMIC(IOArea_p,
                                                                 PRO_scratch,
                                                                 PRO_n,
                                                                 result_len,
                                                                 EcdsaWords,
                                                                 PRO_u1);

            ASSERT(EIP28_Status == EIP28_STATUS_OK);
        }
        break;

    case 11:
        if (res == SBHYBRID_PENDING)
        {
            EIP28_ReadResult_RemainderWordCount_CALLATOMIC(IOArea_p,
                                                           &result_len);
            if (result_len == 0)
            {
                L_DEBUG(LF_SBHYBRID, "Verification failed: (e * w) mod n = 0.");

                res = SB_ERROR_VERIFICATION;
            }
        }

        if (res == SBHYBRID_PENDING)
        {
            /* ecc multiply: u1 * G */

            L_TRACE(LF_SBHYBRID,
                    "EIP28 ECCMUL u1 %d length %u, G %d length %u, "
                    "curve at %d, result u1G %d",
                    PRO_u1, result_len, PRO_G_x, EcdsaWords, PRO_p, PRO_u1G_x);

            EIP28_Status = EIP28_StartOp_EccMul_Affine_kmulC_CALLATOMIC(IOArea_p,
                                                                        PRO_u1,
                                                                        PRO_p,
                                                                        PRO_G_x,
                                                                        result_len,
                                                                        EcdsaWords,
                                                                        PRO_u1G_x);
        }
        break;

    case 12:
        if (!SBHYBRID_EIP28_Ecc_ResultValid(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID, "Verification failed: u1 x G not valid.");

            res = SB_ERROR_VERIFICATION;
        }

        if (res == SBHYBRID_PENDING)
        {
            /* ecc add: (u1 * G) + (u2 * QU) */
            L_TRACE(LF_SBHYBRID,
                    "EIP28 ECCADD u1G %d, u2QU %d, lengths %u, "
                    "curve at %d, result point %d",
                    PRO_u1G_x, PRO_u2QU_x, EcdsaWords, PRO_p, PRO_point_x);

            EIP28_Status = EIP28_StartOp_EccAdd_Affine_AplusC_CALLATOMIC(IOArea_p,
                                                                         PRO_u1G_x,
                                                                         PRO_p,
                                                                         PRO_u2QU_x,
                                                                         EcdsaWords,
                                                                         PRO_point_x);

            ASSERT(EIP28_Status == EIP28_STATUS_OK);
        }
        break;

    case 13:
        if (!SBHYBRID_EIP28_Ecc_ResultValid(IOArea_p))
        {
            L_DEBUG(LF_SBHYBRID,
                    "Verification failed: (u1 x G) + (u2 x QU) not valid.");

            res = SB_ERROR_VERIFICATION;
        }

        if (res == SBHYBRID_PENDING)
        {
            /*  scratch = point_x mod n */
            L_TRACE(LF_SBHYBRID,
                    "EIP28 MOD point_x %d, n %d length %u, "
                    "result scratch %d",
                    PRO_point_x, PRO_n, EcdsaWords, PRO_scratch);

            EIP28_Status = EIP28_StartOp_Modulo_AmodB_CALLATOMIC(IOArea_p,
                                                                 PRO_point_x,
                                                                 PRO_n,
                                                                 EcdsaWords,
                                                                 EcdsaWords,
                                                                 PRO_scratch);

            ASSERT(EIP28_Status == EIP28_STATUS_OK);
        }
        break;

    case 14:
        if (res == SBHYBRID_PENDING)
        {
            EIP28_ReadResult_RemainderWordCount_CALLATOMIC(IOArea_p,
                                                           &result_len);
            if (result_len == 0)
            {
                L_DEBUG(LF_SBHYBRID, "Verification failed: point_x mod n = 0.");

                res = SB_ERROR_VERIFICATION;
            }
        }

        if (res == SBHYBRID_PENDING)
        {
            /* compare (point_x mod n) with r */
            L_TRACE(LF_SBHYBRID,
                    "EIP28 Compare scratch %d, signature r %d, "
                    "lengths %u.",
                    PRO_scratch, PRO_r, EcdsaWords);

            EIP28_Status = EIP28_StartOp_Compare_AcmpB_CALLATOMIC(IOArea_p,
                                                                  PRO_scratch,
                                                                  PRO_r,
                                                                  EcdsaWords);
        }
        break;

    case 15:
        {
            EIP28_CompareResult_t CompareResult;

            EIP28_ReadResult_Compare(IOArea_p, &CompareResult);

            if (CompareResult != EIP28_COMPARERESULT_A_EQUALS_B)
            {
                L_DEBUG(LF_SBHYBRID, "Verification failed: result point x != r.");

                res = SB_ERROR_VERIFICATION;
            }
            else
            {
                L_TRACE(LF_SBHYBRID, "Verification successful.");
                res = SB_SUCCESS;
            }
        }
        break;

    default:
        L_DEBUG(LF_SBHYBRID,
                "ECDSA Verification Proceeded to invalid step %u",
                (unsigned int)next_step);
        res = SB_ERROR_VERIFICATION;
        break;
    }

    if (next_step != 0)
    {
        /*
          This function is called in polling manner so print traces
          log only when EIP28 is ready and we can do something.
        */

        if (res == SB_SUCCESS)
        {
            L_TRACE(LF_SBHYBRID, "Success.");
        }
        else if (res == SBHYBRID_PENDING)
        {
            L_TRACE(LF_SBHYBRID, "Pending.");
        }
        else
        {
            L_TRACE(LF_SBHYBRID, "ERROR %d.", res);
        }
    }

    ASSERT(EIP28_Status == EIP28_STATUS_OK);

    return res;
}

#else
extern const char * sbhybrid_empty_file; /* C forbids empty source files. */
#endif /* SBHYBRID_WITH_EIP28 */

/* end of file sbhybrid_eip28_ecdsa-verify.c */
