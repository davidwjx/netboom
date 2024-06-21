/* sb_sw_ecdsa256.c
 *
 * Description: ECDSA related calculations
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
#include "sb_sw_ecdsa256.h"
#include "uECC.h"

#include <stdio.h>
#include <string.h>

#ifdef SUPPORT_POINT_VALIDATION
bool
SB_SW_ECDSA_Point_Check256(
    const uint8_t * const Q_p)
{
    int status = 0;
    bool ret = false;
    const struct uECC_Curve_t * curve = uECC_secp256r1();

    status = uECC_valid_public_key((const uint8_t*)Q_p, curve);
    if (status == 1)
    {
        ret = true;
    }
    /* Point order is incorrect. */
    return ret;
}
#endif

bool
SB_SW_ECDSA_Verify256(
    SB_SW_ECDSA_Verify_Workspace_t * const wks_p,
    const uint32_t e[SBIF_ECDSA256_WORDS],
    const struct SB_SW_ECDSA_RS256 * const RS_p,
    const struct SB_SW_ECDSA_Point256 * const Q_p)
{
    int status = 0;
    bool ret = false;
    const struct uECC_Curve_t * curve = uECC_secp256r1();

    IDENTIFIER_NOT_USED(wks_p);

    status = uECC_verify((const uint8_t*)Q_p, (const uint8_t*)e, SBIF_ECDSA256_BYTES, (const uint8_t*)RS_p, curve);
    if (status == 1)
    {
        ret = true;
    }
    /* Point order is incorrect. */
    return ret;
}

/* end of file sb_sw_ecdsa256.c */
