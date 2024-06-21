/* sb_sw_ecdsa256.h
 *
 * Description: Secure Boot ECDSA P-256 Interface.
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

/*
    Secure Boot ECDSA Verification uses this header to specify interface
    it uses to interact with ECDSA signing/verification using curve P-256.

    Other software may also use this interface to access other parts of
    the ECDSA functionality, including key generation and signing.

    The interface defines SBIF_ECDSA256_WORDS, which declares the size of
    ECDSA calculated (and therefore) the size of output of output signature
    and domain parameters. The value is always 8.
*/

#ifndef INCLUDE_GUARD_SB_SW_ECDSA256_H
#define INCLUDE_GUARD_SB_SW_ECDSA256_H

#include "public_defs.h"
#include "sbif_ecdsa.h"
#include "sb_sw_ecdsa_common.h"

#define SBIF_ECDSA256_WORDS 8
#define SBIF_ECDSA256_BYTES 32

/* Point */
struct SB_SW_ECDSA_Point256
{
    uint32_t x[SBIF_ECDSA256_WORDS];
    uint32_t y[SBIF_ECDSA256_WORDS];
};

/* Handle R+S pair as alias for point. */
#define SB_SW_ECDSA_RS256 SB_SW_ECDSA_Point256


/* Prototypes of ECDSA functions (with P-256). */
bool
SB_SW_ECDSA_KeyPair256(
    uint32_t * randomVal,
    struct SB_SW_ECDSA_Point256 * const Q_p,
    bool * const again);

bool
SB_SW_ECDSA_Sign256(
    const uint8_t * e,
    const uint32_t e_size,
    const uint32_t da[SBIF_ECDSA256_WORDS],
    struct SB_SW_ECDSA_RS256 * const RS_p,
    bool * const again);

bool
SB_SW_ECDSA_Verify256(
    SB_SW_ECDSA_Verify_Workspace_t * const wks_p,
    const uint32_t e[SBIF_ECDSA256_WORDS],
    const struct SB_SW_ECDSA_RS256 * const RS_p,
    const struct SB_SW_ECDSA_Point256 * const Q_p);

bool
SB_SW_ECDSA_Point_Check256(
    const uint8_t * const Q_p);


#endif /* Include Guard */

/* end of file sb_sw_ecdsa256.h */
