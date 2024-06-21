/* sb_sw_ecdsa521.h
 *
 * Description: Secure Boot ECDSA P-521 Interface.
 */

/*****************************************************************************
* Copyright (c) 2018 INSIDE Secure B.V. All Rights Reserved.
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
    it uses to interact with ECDSA signing/verification using curve P-521.

    Other software may also use this interface to access other parts of
    the ECDSA functionality, including key generation and signing.

    The interface defines SBIF_ECDSA521_BYTES, which declares the size of
    ECDSA calculated (and therefore) the size of output of output signature
    and domain parameters. The value is always 66.
*/

#ifndef INCLUDE_GUARD_SB_SW_ECDSA521_H
#define INCLUDE_GUARD_SB_SW_ECDSA521_H

#include "public_defs.h"
#include "sbif_ecdsa.h"
#include "sb_sw_ecdsa_common.h"

#define SBIF_ECDSA521_BYTES 66
#define SBIF_ECDSA521_WORDS 17

/* Point */
struct SB_SW_ECDSA_Point521
{
    uint8_t x[SBIF_ECDSA521_BYTES];
    uint8_t y[SBIF_ECDSA521_BYTES];
    uint8_t padding[2 * (SBIF_ECDSA521_WORDS * 4 - SBIF_ECDSA521_BYTES)];
};

/* Handle R+S pair as alias for point. */
#define SB_SW_ECDSA_RS521 SB_SW_ECDSA_Point521


/* Prototypes of ECDSA functions (with P-521). */
bool
SB_SW_ECDSA_KeyPair521(
    uint32_t * randomVal,
    struct SB_SW_ECDSA_Point521 * const Q_p,
    bool * const again);

bool
SB_SW_ECDSA_Sign521(
    const uint8_t * e,
    const uint32_t e_size,
    const uint8_t da[SBIF_ECDSA521_BYTES],
    struct SB_SW_ECDSA_RS521 * const RS_p,
    bool * const again);

bool
SB_SW_ECDSA_Verify521(
    SB_SW_ECDSA_Verify_Workspace_t * const wks_p,
    uint8_t e[SBIF_ECDSA521_BYTES],
    const struct SB_SW_ECDSA_RS521 * const RS_p,
    const struct SB_SW_ECDSA_Point521 * const Q_p);

bool
SB_SW_ECDSA_Point_Check521(
    const uint8_t * const Q_p);


#endif /* Include Guard */

/* end of file sb_sw_ecdsa521.h */
