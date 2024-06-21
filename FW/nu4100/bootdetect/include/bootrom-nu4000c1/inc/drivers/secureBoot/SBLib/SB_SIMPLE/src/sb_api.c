/* sb_api.c
 *
 * Description: Secure boot APIs
 *              Allows calls that look just like Hardware-assisted Secure
 *              Boot, but converts the parameters to new scheme.
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

#include "sb_ecdsa.h"
#include "sb_sw_hash.h"
#include "sb_sw_ecdsa.h"
#include "sb_sw_endian.h"
#include "sbif_ecdsa.h"
#include "sbif_attributes.h"
#include "cfg_sblib.h"
#include "c_lib.h"

#ifndef SBIF_ECDSA_WORDS
#error "SBIF_ECDSA_WORDS missing."
#endif /* !SBIF_ECDSA_WORDS */

#ifdef IMPLDEFS_CF_DISABLE_L_TRACE
#define L_TRACE_DIGEST(_d)
#define L_TRACE_PUBKEY(_k)
#define L_TRACE_SIGNATURE(_s)
#else
#define L_TRACE_DIGEST(_d) L_TRACE(LF_SBPK, \
            "Digest: " \
            "%02X %02X %02X %02X %02X %02x %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X", \
            (_d)[0],  (_d)[1],  (_d)[2],  (_d)[3],  (_d)[4],  (_d)[5],  (_d)[6],  \
            (_d)[7],  (_d)[8],  (_d)[9],  (_d)[10], (_d)[11], (_d)[12], (_d)[13], \
            (_d)[14], (_d)[15], (_d)[16], (_d)[17], (_d)[18], (_d)[19], (_d)[20], \
            (_d)[21], (_d)[22], (_d)[23], (_d)[24], (_d)[25], (_d)[26], (_d)[27])

#define L_TRACE_PUBKEY(_k) L_TRACE(LF_SBPK, \
            "PubKey: " \
            "Qx{" \
            "%02X %02X %02X %02X %02X %02x %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X} " \
            "Qy{" \
            "%02X %02X %02X %02X %02X %02x %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X}", \
            (_k)->Qx[0],  (_k)->Qx[1],  (_k)->Qx[2],  (_k)->Qx[3],  (_k)->Qx[4],  (_k)->Qx[5],  (_k)->Qx[6],  \
            (_k)->Qx[7],  (_k)->Qx[8],  (_k)->Qx[9],  (_k)->Qx[10], (_k)->Qx[11], (_k)->Qx[12], (_k)->Qx[13], \
            (_k)->Qx[14], (_k)->Qx[15], (_k)->Qx[16], (_k)->Qx[17], (_k)->Qx[18], (_k)->Qx[19], (_k)->Qx[20], \
            (_k)->Qx[21], (_k)->Qx[22], (_k)->Qx[23], (_k)->Qx[24], (_k)->Qx[25], (_k)->Qx[26], (_k)->Qx[27], \
            (_k)->Qy[0],  (_k)->Qy[1],  (_k)->Qy[2],  (_k)->Qy[3],  (_k)->Qy[4],  (_k)->Qy[5],  (_k)->Qy[6],  \
            (_k)->Qy[7],  (_k)->Qy[8],  (_k)->Qy[9],  (_k)->Qy[10], (_k)->Qy[11], (_k)->Qy[12], (_k)->Qy[13], \
            (_k)->Qy[14], (_k)->Qy[15], (_k)->Qy[16], (_k)->Qy[17], (_k)->Qy[18], (_k)->Qy[19], (_k)->Qy[20], \
            (_k)->Qy[21], (_k)->Qy[22], (_k)->Qy[23], (_k)->Qy[24], (_k)->Qy[25], (_k)->Qy[26], (_k)->Qy[27])

#define L_TRACE_SIGNATURE(_s) L_TRACE(LF_SBPK, \
            "Signature: " \
            "r{" \
            "%02X %02X %02X %02X %02X %02x %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "} s{" \
            "%02X %02X %02X %02X %02X %02x %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X}", \
            (_s)->r[0],  (_s)->r[1],  (_s)->r[2],  (_s)->r[3],  (_s)->r[4],  (_s)->r[5],  (_s)->r[6],  \
            (_s)->r[7],  (_s)->r[8],  (_s)->r[9],  (_s)->r[10], (_s)->r[11], (_s)->r[12], (_s)->r[13], \
            (_s)->r[14], (_s)->r[15], (_s)->r[16], (_s)->r[17], (_s)->r[18], (_s)->r[19], (_s)->r[20], \
            (_s)->r[21], (_s)->r[22], (_s)->r[23], (_s)->r[24], (_s)->r[25], (_s)->r[26], (_s)->r[27], \
            (_s)->s[0],  (_s)->s[1],  (_s)->s[2],  (_s)->s[3],  (_s)->s[4],  (_s)->s[5],  (_s)->s[6],  \
            (_s)->s[7],  (_s)->s[8],  (_s)->s[9],  (_s)->s[10], (_s)->s[11], (_s)->s[12], (_s)->s[13], \
            (_s)->s[14], (_s)->s[15], (_s)->s[16], (_s)->s[17], (_s)->s[18], (_s)->s[19], (_s)->s[20], \
            (_s)->s[21], (_s)->s[22], (_s)->s[23], (_s)->s[24], (_s)->s[25], (_s)->s[26], (_s)->s[27])
#endif

typedef struct
{
    SB_SW_ECDSA_Verify_Workspace_t verify_wks;
    SB_SW_HASH_Context_t hash_ctx;

    /* Verification parameters. */
    uint32_t e[SBIF_ECDSA_WORDS];
#if SBIF_ECDSA_WORDS == 8
    struct SB_SW_ECDSA_RS256 RS;
    struct SB_SW_ECDSA_Point256 Q;
#else
    struct SB_SW_ECDSA_RS224 RS;
    struct SB_SW_ECDSA_Point224 Q;
#endif
} SB_SW_Verify_Workspace_t;

/* Static array for processing. */
static SB_SW_Verify_Workspace_t workspace;

static void
uint8_to_sb_sw_number(const uint8_t src[SBIF_ECDSA_BYTES],
                      uint32_t dst[SBIF_ECDSA_WORDS])
{
    int i;
    uint8_t * dstb = (uint8_t *)dst;

    /* Swap ordering, after this order is least significant bit first. */
    for (i = 0; i < (int)SBIF_ECDSA_BYTES; i++)
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


// ensure there is enough space in the caller-allocated buffer
COMPILE_GLOBAL_ASSERT(sizeof(SB_SW_Verify_Workspace_t) <= sizeof(SB_StorageArea_t));

SB_Result_t
SB_ECDSA_Image_Verify(SB_StorageArea_t * const             Storage_p,
                      void *                               PollParam_p,
                      const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                      const uint8_t * const                Hash_p,
                      const SBIF_ECDSA_Header_t *          Header_p,
                      const SBIF_SGVector_t *              DataVectors_p,
                      uint32_t VectorCount)
{
    SB_SW_Verify_Workspace_t * const wks_p = &workspace;
#if SBLIB_CFG_CERTIFICATES_MAX > 0
    const SBIF_ECDSA_Certificate_t * Certs = (const SBIF_ECDSA_Certificate_t *)&Header_p[1];
#endif
    uint32_t image_len_left;
    bool res;
    uint32_t idx;
    uint32_t certcount;
    uint32_t image_type;

    /* SecureBoot currently is only able to support up-to 255 certificates. */
    COMPILE_STATIC_ASSERT(SBLIB_CFG_CERTIFICATES_MAX <= 255);

    image_type = SB_SW_BE32_TO_CPU(Header_p->Type);
    if (SBIF_TYPE_VERSION(image_type) != SBIF_VERSION)
    {
        return SB_ERROR_IMAGE_VERSION;
    }

    image_type = SBIF_TYPE_TYPE(image_type);
    if (image_type != SBIF_IMAGE_BLTp)
    {
        return SB_ERROR_IMAGE_TYPE;
    }

    L_TRACE_PUBKEY(PublicKey_p);
    uint8_to_sb_sw_number(PublicKey_p->Qx, wks_p->Q.x);
    uint8_to_sb_sw_number(PublicKey_p->Qy, wks_p->Q.y);

#ifdef SBLIB_CFG_PUBLIC_KEY_VERIFY
#if SBLIB_CFG_PUBLIC_KEY_VERIFY > 0
#if SBIF_ECDSA_WORDS == 8
    res = SB_SW_ECDSA_Point_Check256((const uint8_t*)&wks_p->Q);
#else
    res = SB_SW_ECDSA_Point_Check224((const uint8_t*)&wks_p->Q);
#endif
    if (res == false)
    {
        /* Incorrect public key, verification failed. */
        return SB_ERROR_VERIFICATION;
    }
#endif /* SBLIB_CFG_PUBLIC_KEY_VERIFY > 0 */
#endif /* SBLIB_CFG_PUBLIC_KEY_VERIFY */

    /* Prepare calculate hash over data. */
    image_len_left = SB_SW_BE32_TO_CPU(Header_p->ImageLen);
    if (image_len_left > SBLIB_CFG_DATASIZE_MAX)
    {
        /* Too large image supplied for verification. */
        return SB_ERROR_VERIFICATION;
    }

    if (VectorCount == 0)
    {
        return SB_ERROR_ARGUMENTS;
    }
    else if (VectorCount > SBLIB_CFG_DATAFRAGMENTS_MAX)
    {
        return SB_ERROR_ARGUMENTS;
    }

    certcount = SB_SW_BE32_TO_CPU(Header_p->CertificateCount);
    if (certcount > (uint32_t)SBLIB_CFG_CERTIFICATES_MAX)
    {
        return SB_ERROR_CERTIFICATE_COUNT;
    }

    /* If there are any certificates, process them. */
#if SBLIB_CFG_CERTIFICATES_MAX > 0
    for (idx = 0; idx < certcount; idx++)
    {
        /* Hash Certificate. */
        SB_SW_HASH_Init(&wks_p->hash_ctx, SBIF_ECDSA_BITS);
        SB_SW_HASH_FinalUpdate(&wks_p->hash_ctx,
                               (const void *)&Certs[idx],
                               sizeof(SBIF_ECDSA_PublicKey_t),
                               wks_p->e);

        L_TRACE_DIGEST((uint8_t *)wks_p->e);

        /* Load signature from certificate for processing with
           SB_SW_ECDSA_Verify. */
        L_TRACE_SIGNATURE(&Certs[idx].Signature);
        uint8_to_sb_sw_number(Certs[idx].Signature.r, wks_p->RS.x);
        uint8_to_sb_sw_number(Certs[idx].Signature.s, wks_p->RS.y);

        /* Verify certificate. */
#if SBIF_ECDSA_WORDS == 8
        res = SB_SW_ECDSA_Verify256(&wks_p->verify_wks,
                                    wks_p->e,
                                    &wks_p->RS,
                                    &wks_p->Q);
#else
        res = SB_SW_ECDSA_Verify224(&wks_p->verify_wks,
                                    wks_p->e,
                                    &wks_p->RS,
                                    &wks_p->Q);
#endif

        /* On certificate failure, lets exit. */
        if (res == false)
        {
            return SB_ERROR_VERIFICATION;
        }

        /* If certificate check succeeded, we shall take
           the certified public key into use and use it for
           checking the next certificate or
           (in the case of the last certificate) the actual image. */
        L_TRACE_PUBKEY(&Certs[idx].PublicKey);
        uint8_to_sb_sw_number(Certs[idx].PublicKey.Qx, wks_p->Q.x);
        uint8_to_sb_sw_number(Certs[idx].PublicKey.Qy, wks_p->Q.y);
    }
#endif /* SBLIB_CFG_CERTIFICATES_MAX > 0 */

    /* Check the attributes. */
    if (SBIF_Attribute_Check(&Header_p->ImageAttributes) == false)
    {
        /* Invalid attributes. */
        return SB_ERROR_VERIFICATION;
    }

    if (Hash_p != NULL)
    {
        uint8_t Digest[SBIF_ECDSA_WORDS * 4];
        unsigned int i;

        /* Initialize hash_ctx. */
        SB_SW_HASH_Init(&wks_p->hash_ctx, SBIF_ECDSA_BITS);

        /* Hashing: Hash image attributes first. */
        SB_SW_HASH_FinalUpdate(&wks_p->hash_ctx,
                               (const void *)&Header_p->PublicKey,
                               sizeof(Header_p->PublicKey),
                               wks_p->e);

        /* Convert digest to byte string in the correct way. */
        for (i = 0; i < SBIF_ECDSA_WORDS; i++)
        {
            Digest[4 * i + 0] = wks_p->e[SBIF_ECDSA_WORDS - 1 - i] >> 24;
            Digest[4 * i + 1] = wks_p->e[SBIF_ECDSA_WORDS - 1 - i] >> 16;
            Digest[4 * i + 2] = wks_p->e[SBIF_ECDSA_WORDS - 1 - i] >> 8;
            Digest[4 * i + 3] = wks_p->e[SBIF_ECDSA_WORDS - 1 - i] >> 0;
        }
        if (c_memcmp(Digest, Hash_p, SBIF_ECDSA_WORDS * 4) != 0)
        {
            /* Public key hash mismatch. */
            return SB_ERROR_VERIFICATION;
        }
    }

    /* Initialize hash_ctx. */
    SB_SW_HASH_Init(&wks_p->hash_ctx, SBIF_ECDSA_BITS);

    /* Hashing: Hash image attributes first. */
    SB_SW_HASH_Update(&wks_p->hash_ctx,
                      (const void *)&Header_p->ImageAttributes,
                      sizeof(Header_p->ImageAttributes));

    /* Then continue with image blocks. */
    for (idx = 0; idx < VectorCount - 1; idx++)
    {
        /* Non-final blocks */
        SB_SW_HASH_Update(&wks_p->hash_ctx,
                          (const void *)(DataVectors_p[idx].Data_p),
                          DataVectors_p[idx].DataLen);
        if (image_len_left < DataVectors_p[idx].DataLen)
        {
            goto size_error;
        }
        image_len_left -= DataVectors_p[idx].DataLen;
    }

    /* Final block.
       Notice: because VectorCount was not zero, there is always final block.
       The length of the block must be exactly image_len_left. */

    if (image_len_left != DataVectors_p[VectorCount - 1].DataLen)
    {
size_error:
        return SB_ERROR_HARDWARE;
    }

    SB_SW_HASH_FinalUpdate(&wks_p->hash_ctx,
                           (const void *)(DataVectors_p[VectorCount - 1].Data_p),
                           image_len_left,
                           wks_p->e);

    L_TRACE_DIGEST((uint8_t *)wks_p->e);

    /* Load signature from header for processing with SB_SW_ECDSA_Verify. */
    L_TRACE_SIGNATURE(&Header_p->Signature);
    uint8_to_sb_sw_number(Header_p->Signature.r, wks_p->RS.x);
    uint8_to_sb_sw_number(Header_p->Signature.s, wks_p->RS.y);

    /* Now signature has been calculated.
       Next we shall verify it with ECDSA verification function. */

    /* Verify calculated hash with ECDSA. */
#if SBIF_ECDSA_WORDS == 8
    res = SB_SW_ECDSA_Verify256(&wks_p->verify_wks,
                                wks_p->e,
                                &wks_p->RS,
                                &wks_p->Q);
#else
    res = SB_SW_ECDSA_Verify224(&wks_p->verify_wks,
                                wks_p->e,
                                &wks_p->RS,
                                &wks_p->Q);
#endif

    PARAMETER_NOT_USED(PollParam_p);
    PARAMETER_NOT_USED(Storage_p);

    return res ? SB_SUCCESS : SB_ERROR_VERIFICATION;
}

/* end of file sb_api.c */
