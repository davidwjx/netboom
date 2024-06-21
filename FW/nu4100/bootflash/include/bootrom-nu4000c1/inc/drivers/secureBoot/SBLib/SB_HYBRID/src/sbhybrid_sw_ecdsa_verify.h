/* sbhybrid_sw_ecdsa_verify.h
 *
 * Description: Secure boot library: Internal API for ECDSA operations.
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

#ifndef INCLUSION_GUARD_SBHYBRID_SW_ECDSA_VERIFY_H
#define INCLUSION_GUARD_SBHYBRID_SW_ECDSA_VERIFY_H

#include "sb_sw_ecdsa.h"           // SB_SW_ECDSA_Point224/256/384/521

// internal ECDSA Verification state structure
typedef struct
{
    SB_SW_ECDSA_Verify_Workspace_t Verify_Workspace;

    /* Verification parameters, including domain parameters. */
#if SBIF_ECDSA_WORDS == 17
    uint8_t e[SBIF_ECDSA_BYTES];
#else
    uint32_t e[SBIF_ECDSA_WORDS];
#endif
    bool     has_e;
#if SBIF_ECDSA_WORDS == 17
    struct SB_SW_ECDSA_RS521 RS;
    struct SB_SW_ECDSA_Point521 Public;
#elif SBIF_ECDSA_WORDS == 12
    struct SB_SW_ECDSA_RS384 RS;
    struct SB_SW_ECDSA_Point384 Public;
#elif SBIF_ECDSA_WORDS == 8
    struct SB_SW_ECDSA_RS256 RS;
    struct SB_SW_ECDSA_Point256 Public;
#elif SBIF_ECDSA_WORDS == 7
    struct SB_SW_ECDSA_RS224 RS;
    struct SB_SW_ECDSA_Point224 Public;
#endif
}
SBHYBRID_ECDSA_Verify_t;


void
SBHYBRID_SW_Ecdsa_Verify_SetPublicKey(
    SBHYBRID_ECDSA_Verify_t * const Verify_p,
    const SBIF_ECDSA_PublicKey_t * const PublicKey_p);

void
SBHYBRID_SW_Ecdsa_Verify_SetSignature(
    SBHYBRID_ECDSA_Verify_t * const Verify_p,
    const SBIF_ECDSA_Signature_t * const Signature_p);

void
SBHYBRID_SW_Ecdsa_Verify_SetDigest(
    SBHYBRID_ECDSA_Verify_t * const Verify_p,
    uint8_t * Digest_p);

SB_Result_t
SBHYBRID_SW_Ecdsa_Verify(
    SBHYBRID_ECDSA_Verify_t * const Verify_p);

// A front-end for SBHYBRID_Ecdsa_Verify*:
//    Set public key and signature and mark digest as not yet available.
static inline
void
SBHYBRID_SW_Ecdsa_Verify_Init(
    SBHYBRID_ECDSA_Verify_t * const Verify_p,
    const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
    const SBIF_ECDSA_Signature_t * const Signature_p)
{
    SBHYBRID_SW_Ecdsa_Verify_SetPublicKey(Verify_p, PublicKey_p);
    SBHYBRID_SW_Ecdsa_Verify_SetSignature(Verify_p, Signature_p);
    Verify_p->has_e = false;
}

// A front-end for SBHYBRID_Ecdsa_Verify:
//    Run verify at once when digest is known.
static inline
SB_Result_t
SBHYBRID_SW_Ecdsa_Verify_RunFsm(
    SBHYBRID_ECDSA_Verify_t * const Verify_p)
{
    /* Return pending if digest is not yet available. */
    if (!Verify_p->has_e)
    {
        return SB_ERROR_COUNT;
    }

    /* Calculate ECDSA Verify if the digest is available. */
    return SBHYBRID_SW_Ecdsa_Verify(Verify_p);
}

#endif /* Include Guard */

/* end of file sbhybrid_sw_ecdsa_verify.h */
