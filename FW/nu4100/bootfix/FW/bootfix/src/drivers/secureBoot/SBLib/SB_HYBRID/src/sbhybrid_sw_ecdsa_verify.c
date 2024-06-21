/* sbhybrid_sw_ecdsa_verify.c
 *
 * SW-only ECDSA Verify service for Secure Boot Library.
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

#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_SWPK

#if SBIF_ECDSA_BYTES == 66
static void
uint8_to_sb_sw_number_digest(const uint8_t src[64],
                             uint8_t dst[64])
{
    uint32_t i;

    /* Swap ordering, after this order is least significant bit first. */
    for (i = 0; i < 64; i++)
    {
        dst[64 - i - 1] = src[i];
    }

#ifdef SB_CF_BIGENDIAN
    uint32_t * dstw = (uint32_t *)dst;
    /* Swap all words */
    for (i = 0; i < 16; i++)
    {
        uint32_t tmp = dstw[i];
        dstw[i] = SB_SW_BYTEORDER_SWAP32(tmp);
    }
#endif /* SB_CF_BIGENDIAN */
}

static void
uint8_to_sb_sw_number(const uint8_t src[SBIF_ECDSA_BYTES],
                      uint8_t dst[SBIF_ECDSA_BYTES])
{
    uint32_t i;

    /* Swap ordering, after this order is least significant bit first. */
    for (i = 0; i < SBIF_ECDSA_BYTES; i++)
    {
        dst[SBIF_ECDSA_BYTES - i - 1] = src[i];
    }

#ifdef SB_CF_BIGENDIAN
    uint32_t * dstw = (uint32_t *)dst;
    uint8_t * dstb = (uint8_t *)dst;
    uint8_t tmpb;
    /* Swap all words */
    for (i = 0; i < 16; i++)
    {
        uint32_t tmp = dstw[i];
        dstw[i] = SB_SW_BYTEORDER_SWAP32(tmp);
    }
    tmpb = dstb[65];
    dstb[65] = dstb[64];
    dstb[64] = tmpb;
#endif /* SB_CF_BIGENDIAN */
}
#else
static void
uint8_to_sb_sw_number(const uint8_t src[SBIF_ECDSA_BYTES],
                      uint32_t dst[SBIF_ECDSA_WORDS])
{
    uint32_t i;
    uint8_t * dstb = (uint8_t *)dst;

    /* Swap ordering, after this order is least significant bit first. */
    for (i = 0; i < SBIF_ECDSA_BYTES; i++)
    {
        dstb[SBIF_ECDSA_BYTES - i - 1] = src[i];
    }

#ifdef SB_CF_BIGENDIAN
    /* Swap all words */
    for (i = 0; i < SBIF_ECDSA_WORDS; i++)
    {
        uint32_t tmp = dst[i];
        dst[i] = SB_SW_BYTEORDER_SWAP32(tmp);
    }
#endif /* SB_CF_BIGENDIAN */
}
#endif

void
SBHYBRID_SW_Ecdsa_Verify_SetPublicKey(SBHYBRID_ECDSA_Verify_t * const Verify_p,
                                      const SBIF_ECDSA_PublicKey_t * const PublicKey_p)
{
    PRECONDITION(PublicKey_p != NULL);

    uint8_to_sb_sw_number(PublicKey_p->Qx, Verify_p->Public.x);
    uint8_to_sb_sw_number(PublicKey_p->Qy, Verify_p->Public.y);
}

void
SBHYBRID_SW_Ecdsa_Verify_SetSignature(SBHYBRID_ECDSA_Verify_t * const Verify_p,
                                      const SBIF_ECDSA_Signature_t * const Signature_p)
{
    PRECONDITION(Signature_p != NULL);

    uint8_to_sb_sw_number(Signature_p->r, Verify_p->RS.x);
    uint8_to_sb_sw_number(Signature_p->s, Verify_p->RS.y);
}


void
SBHYBRID_SW_Ecdsa_Verify_SetDigest(SBHYBRID_ECDSA_Verify_t * const Verify_p,
                                   uint8_t * Digest_p)
{
    PRECONDITION(Digest_p != NULL);

#if SBIF_ECDSA_BYTES == 66
    uint8_to_sb_sw_number_digest(Digest_p, Verify_p->e);
#else
    uint8_to_sb_sw_number(Digest_p, Verify_p->e);
#endif
    Verify_p->has_e = true;
}


/* SBHYBRID_SW_Ecdsa_Verify
 */
SB_Result_t
SBHYBRID_SW_Ecdsa_Verify(SBHYBRID_ECDSA_Verify_t * const Verify_p)
{
    bool res;

    /* Optional check for correctness of the public key. */
#ifdef SBLIB_CFG_PUBLIC_KEY_VERIFY
#if SBLIB_CFG_PUBLIC_KEY_VERIFY > 0
#if SBIF_ECDSA_WORDS == 7
    res = SB_SW_ECDSA_Point_Check224((const uint8_t*)&Verify_p->Public);
#elif SBIF_ECDSA_WORDS == 8
    res = SB_SW_ECDSA_Point_Check256((const uint8_t*)&Verify_p->Public);
#elif SBIF_ECDSA_WORDS == 12
    res = SB_SW_ECDSA_Point_Check384((const uint8_t*)&Verify_p->Public);
#elif SBIF_ECDSA_WORDS == 17
    res = SB_SW_ECDSA_Point_Check521((const uint8_t*)&Verify_p->Public);
#else
    res = false;
#endif /* */
    if (res == false)
    {
        /* Incorrect public key, verification failed. */
        return SB_ERROR_VERIFICATION;
    }
#endif /* SBLIB_CFG_PUBLIC_KEY_VERIFY > 0 */
#endif /* SBLIB_CFG_PUBLIC_KEY_VERIFY */

    /* The symmetric processing is ready, do asymmetric processing. */
#if SBIF_ECDSA_WORDS == 7
    res = SB_SW_ECDSA_Verify224(&Verify_p->Verify_Workspace,
                                Verify_p->e,
                                &Verify_p->RS,
                                &Verify_p->Public);
#elif SBIF_ECDSA_WORDS == 8
    res = SB_SW_ECDSA_Verify256(&Verify_p->Verify_Workspace,
                                Verify_p->e,
                                &Verify_p->RS,
                                &Verify_p->Public);
#elif SBIF_ECDSA_WORDS == 12
    res = SB_SW_ECDSA_Verify384(&Verify_p->Verify_Workspace,
                                Verify_p->e,
                                &Verify_p->RS,
                                &Verify_p->Public);
#elif SBIF_ECDSA_WORDS == 17
    res = SB_SW_ECDSA_Verify521(&Verify_p->Verify_Workspace,
                                Verify_p->e,
                                &Verify_p->RS,
                                &Verify_p->Public);
#else
    res = false;
#endif

    return res ? SB_SUCCESS : SB_ERROR_VERIFICATION;
}

#else
extern const char * sbhybrid_empty_file; /* C forbids empty source files. */
#endif /* SBHYBRID_WITH_SWPK */

/* end of file sbhybrid_sw_ecdsa_verify.c */
