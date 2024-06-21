/* sb_sw_ecdsa_sign256.c
 *
 * Description: ECDSA related calculations, for signing and key generation.
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

#include "implementation_defs.h"
#include "sb_sw_ecdsa256.h"
#include "uECC.h"
#include "p11mep.h"


/* Generate ECDSA Key Pair (P-256).

   Implementation follows algorithm:
   FIPS 186-3: B.4.2 Key Pair Generation by Testing Candidates.

   Steps 1-3 are preprocessed: the SW only operates with curves having
   acceptable security strength.
   Steps 4-5 must be preprocessed and resulting value stored in randomVal.
   The implementation starts at step 6. The randomVal must be prefilled
   with material from the random number generator.
   If the function returns with returns with again = false, it means the
   caller needs to generate new random number and retry. The need to retry
   is very rare. */
bool
SB_SW_ECDSA_KeyPair256(
    uint32_t * randomVal,
    struct SB_SW_ECDSA_Point256 * const Q_p,
    bool * const again)
{
    int status = 0;
    bool ret = false;
    const struct uECC_Curve_t * curve = uECC_secp256r1();

    uECC_set_rng((uECC_RNG_Function)P11MEP_GenerateRandom);

    status = uECC_make_key((uint8_t *)Q_p, (uint8_t *)randomVal, curve);
    if (status)
    {
        *again = false;
        ret = true;
    }
    else
    {
        *again = true;
        ret = false;
    }

    return ret;
}

/* NIST P-256 signature generation. */
bool
SB_SW_ECDSA_Sign256(
    const uint8_t * e,
    const uint32_t e_size,
    const uint32_t da[SBIF_ECDSA256_WORDS],
    struct SB_SW_ECDSA_RS256 * const RS_p,
    bool * const again)
{
    int status = 0;
    bool ret = false;
    const struct uECC_Curve_t * curve = uECC_secp256r1();

    uECC_set_rng((uECC_RNG_Function)P11MEP_GenerateRandom);

    status = uECC_sign((const uint8_t *)da, (const uint8_t *)e, e_size, (uint8_t *)RS_p, curve);
    if (status)
    {
        *again = false;
        ret = true;
    }
    else
    {
        *again = true;
        ret = false;
    }

    return ret;
}

#undef W

/* end of file sb_sw_ecdsa_sign256.c */
