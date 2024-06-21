/* aes_if.c
 *
 * Description: Implementation of SB_AES_API/AES_IF for rijndael.c.
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

#include "rijndael.h"
#include "aes_if_internal.h"
#include "aes_if.h"
#include "c_lib.h"
#include "sbcr_key.h"
#include "cfg_sbif.h"

#ifndef AES_IF_OMIT_KEYGEN
#include <stdio.h>
#include "nist-sp-chain.h"
#endif /* AES_IF_OMIT_KEYGEN */

#if (SBIF_CFG_CONFIDENTIALITY_BITS != 128) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 192) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 256)
#error "SBIF_CFG_CONFIDENTIALITY_BITS must be one of 128, 192, or 256."
#endif

/* This implementation of aes_if.h uses rijndael.c.
   When a Secure Boot Confidentiality Root Key (for AES-Wrap) is needed,
   the implementation uses key material defined in sbcr_key.c.
   The default key in sbcr_key.c SHALL be customized.
*/
uint32_t
AES_IF_Ctx_GetSize(void)
{
    return sizeof(struct AES_IF_Ctx);
}

AES_IF_ResultCode_t
AES_IF_Ctx_Init(AES_IF_Ctx_Ptr_t Ctx_p)
{
    c_memset(Ctx_p, 0, sizeof(struct AES_IF_Ctx));
    return AES_IF_RESULT_SUCCESS;
}

void
AES_IF_Ctx_Uninit(AES_IF_Ctx_Ptr_t Ctx_p)
{
    /*PARAMETER_NOT_USED:*/if(Ctx_p) {}
}

AES_IF_ResultCode_t
AES_IF_Ctx_GetError(AES_IF_Ctx_Ptr_t Ctx_p)
{
    AES_IF_ResultCode_t result = Ctx_p->Result;
    return result;
}

void
AES_IF_Ctx_LoadKey(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const Key_p,
                   const uint32_t KeyLength)
{
    if (KeyLength == SBIF_CFG_CONFIDENTIALITY_BITS)
    {
        c_memcpy(Ctx_p->Key, Key_p, SBIF_CFG_CONFIDENTIALITY_BITS / 8);
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

void
AES_IF_Ctx_LoadIV(AES_IF_Ctx_Ptr_t Ctx_p,
                  const uint32_t IV[4])
{
    c_memcpy(Ctx_p->IV, IV, 16);
}

void
AES_IF_Ctx_SetKEK(AES_IF_Ctx_Ptr_t Ctx_p,
                  const void * const KEKIdentification,
                  const uint32_t KEKIdentificationSize)
{
    if (KEKIdentificationSize == 0)
    {
        c_memcpy(Ctx_p->Key, SBCommon_SBCRK, sizeof(SBCommon_SBCRK));
    }
    else if (KEKIdentificationSize == SBIF_CFG_CONFIDENTIALITY_BITS / 8)
    {
        /* Assume KEK provided is the key to use as raw data. */
        c_memcpy(Ctx_p->Key, KEKIdentification, SBIF_CFG_CONFIDENTIALITY_BITS / 8);
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

static bool
generate_key(void * const Key_p)
{
#ifdef AES_IF_OMIT_KEYGEN
    /*PARAMETER_NOT_USED:*/if(Key_p) {}
    return false;
#else
    /* Note: the usual use of generate_key is that it is invoked once or
       twice per entire duration of operation. Often, AES_IF user does
       not need to generate any randomness. Therefore, this function
       is defined so that random number generator is initialized and
       finalized always upon each use. */
    bool success = false;
    RngChain rng;
    static RngChainStruct preallocated_rngchain;

    rng = sfz_random_nist_chain_init(&preallocated_rngchain, true);
    if (rng == NULL)
    {
        printf("Random number generator statefile not available or unusable.\n");
        printf("Reading entropy from /dev/random, this may take some time.\n");

        rng = sfz_random_nist_chain_init(&preallocated_rngchain, false);
    }

    if (rng)
    {
        if (sfz_random_nist_chain_generate(rng,
                                           Key_p,
                                           SBIF_CFG_CONFIDENTIALITY_BITS / 8,
                                           SBIF_CFG_CONFIDENTIALITY_BITS))
        {
            success = true;
        }

        sfz_random_nist_chain_uninit(rng);
    }

    return success;
#endif
}

static void
wrap(const void * const KeyIn_p,
     void * const WrappedOut_p,
     uint32_t KeyBits,
     const void * KWK_p,
     const uint32_t KWKBits)
{
    uint32_t rk[RKLENGTH(SBIF_CFG_CONFIDENTIALITY_BITS)];
    uint32_t R[256 * 8 / 64][2];
    union
    {
        /* A and B variables, combined storage */
        uint8_t A_bytes[16];
        uint32_t A[2];
        uint8_t B_bytes[16];
        uint32_t B[4];
    } u;
    unsigned int i, j;
    uint32_t n = KeyBits / 64;
    uint16_t cnt = 0;
    int nRounds;

    u.A[0] = u.A[1] = 0xA6A6A6A6;

    nRounds = rijndaelSetupEncrypt(rk, KWK_p, KWKBits);

    c_memcpy(R, KeyIn_p, n * 8);

    for(j = 0; j <= 5; j++)
    {
        for(i = 1; i <= n; i++)
        {
            u.B[2] = R[i - 1][0];
            u.B[3] = R[i - 1][1];
            rijndaelEncrypt(rk, nRounds, u.B_bytes, u.B_bytes);
            cnt++;
            u.A_bytes[7] ^= cnt & 255;
            u.A_bytes[6] ^= cnt >> 8;
            R[i - 1][0] = u.B[2];
            R[i - 1][1] = u.B[3];
        }
    }

    c_memcpy(WrappedOut_p, u.A, 8);
    c_memcpy(((char*)WrappedOut_p) + 8, R, n * 8);
}

static bool
unwrap(const void * const WrappedIn_p,
       void * const KeyOut_p,
       uint32_t KeyBits,
       const void * KWK_p,
       const uint32_t KWKBits)
{
    uint32_t rk[RKLENGTH(SBIF_CFG_CONFIDENTIALITY_BITS)];
    uint32_t R[256 * 8 / 64][2];
    union
    {
        /* A and B variables, combined storage */
        uint8_t A_bytes[16];
        uint32_t A[2];
        uint8_t B_bytes[16];
        uint32_t B[4];
    } u;
    int i;
    int j;
    uint32_t n = KeyBits / 64;
    uint16_t cnt;
    int nRounds;

    c_memcpy(u.A, WrappedIn_p, 8);

    nRounds = rijndaelSetupDecrypt(rk, KWK_p, KWKBits);

    c_memcpy(R, ((char*)WrappedIn_p) + 8, n * 8);

    for(j = 5; j >= 0; j--)
    {
        for(i = n; i >= 1; i--)
        {
            u.B[2] = R[i - 1][0];
            u.B[3] = R[i - 1][1];
            cnt = (uint16_t)(n * j + i);
            u.A_bytes[7] ^= cnt & 255;
            u.A_bytes[6] ^= cnt >> 8;
            rijndaelDecrypt(rk, nRounds, u.B_bytes, u.B_bytes);
            R[i - 1][0] = u.B[2];
            R[i - 1][1] = u.B[3];
        }
    }

    c_memcpy(KeyOut_p, R, n * 8);

    return u.A[0] == u.A[1] && u.A[0] == 0xA6A6A6A6;
}

void
AES_IF_Ctx_LoadWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                          const void * const Wrap_p,
                          const uint32_t WrapLength)
{
    bool ok = false;

    if (WrapLength == SBIF_CFG_CONFIDENTIALITY_BITS + 64)
    {
        ok = unwrap(Wrap_p,
                    Ctx_p->Key,
                    SBIF_CFG_CONFIDENTIALITY_BITS,
                    Ctx_p->Key,
                    SBIF_CFG_CONFIDENTIALITY_BITS);
    }

    if (ok == false)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

void
AES_IF_Ctx_GenerateKey(AES_IF_Ctx_Ptr_t Ctx_p,
                       void * const Key_p,
                       const uint32_t KeyLength)
{
    bool result;

    if (KeyLength == SBIF_CFG_CONFIDENTIALITY_BITS)
    {
        result = generate_key(Ctx_p->Key);
        if (result == true)
        {
            c_memcpy(Key_p, Ctx_p->Key, KeyLength / 8);
        }
        else
        {
            Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
        }
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

void
AES_IF_Ctx_GenerateWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                              void * const Wrap_p,
                              const uint32_t WrapLength)
{
    bool ok = false;

    if (WrapLength == SBIF_CFG_CONFIDENTIALITY_BITS + 64)
    {
        bool result;
        uint8_t TmpKey[SBIF_CFG_CONFIDENTIALITY_BITS / 8];

        result = generate_key(TmpKey);
        if (result == true)
        {
            wrap(TmpKey, Wrap_p, WrapLength - 64,
                 Ctx_p->Key, SBIF_CFG_CONFIDENTIALITY_BITS);
            c_memcpy(Ctx_p->Key, TmpKey, sizeof(TmpKey));
            ok = true;
        }
    }

    if (ok == false)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

#ifdef ENABLE_NOT_USED_FUNCTION
void
AES_IF_ECB_EncryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p)
{
    uint32_t rk[RKLENGTH(SBIF_CFG_CONFIDENTIALITY_BITS)];
    int nRounds;

    nRounds = rijndaelSetupEncrypt(rk, Ctx_p->Key,
                                   SBIF_CFG_CONFIDENTIALITY_BITS);
    rijndaelEncrypt(rk, nRounds, DataIn_p, DataOut_p);
}
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
void
AES_IF_ECB_DecryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p)
{
    uint32_t rk[RKLENGTH(SBIF_CFG_CONFIDENTIALITY_BITS)];
    int nRounds;

    nRounds = rijndaelSetupDecrypt(rk, Ctx_p->Key,
                                   SBIF_CFG_CONFIDENTIALITY_BITS);
    rijndaelDecrypt(rk, nRounds, DataIn_p, DataOut_p);
}
#endif

void
AES_IF_CBC_Encrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const DataIn_p,
                   void * const DataOut_p,
                   const uint32_t Size)
{
    uint32_t rk[RKLENGTH(SBIF_CFG_CONFIDENTIALITY_BITS)];
    int nRounds;
    const uint8_t * bytesIn_p = DataIn_p;
    uint8_t * bytesOut_p = DataOut_p;
    uint32_t size = Size;

    nRounds = rijndaelSetupEncrypt(rk, Ctx_p->Key,
                                   SBIF_CFG_CONFIDENTIALITY_BITS);
    while (size >= 16)
    {
        union
        {
            uint32_t alignment;
            uint32_t block_32[4];
            uint8_t block[16];
        } u;

        c_memcpy(u.block_32, bytesIn_p, 16);
        u.block_32[0] ^= Ctx_p->IV[0];
        u.block_32[1] ^= Ctx_p->IV[1];
        u.block_32[2] ^= Ctx_p->IV[2];
        u.block_32[3] ^= Ctx_p->IV[3];

        rijndaelEncrypt(rk, nRounds, u.block, bytesOut_p);
        c_memcpy(Ctx_p->IV, bytesOut_p, 16);

        bytesIn_p += 16;
        bytesOut_p += 16;
        size -= 16;
    }
}

void
AES_IF_CBC_Decrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const DataIn_p,
                   void * const DataOut_p,
                   const uint32_t Size)
{
    uint32_t rk[RKLENGTH(SBIF_CFG_CONFIDENTIALITY_BITS)];
    int nRounds;
    const uint8_t * bytesIn_p = DataIn_p;
    uint8_t * bytesOut_p = DataOut_p;
    uint32_t size = Size;

    nRounds = rijndaelSetupDecrypt(rk, Ctx_p->Key,
                                   SBIF_CFG_CONFIDENTIALITY_BITS);
    while (size >= 16)
    {
        union
        {
            uint32_t alignment;
            uint32_t block_32[4];
            uint8_t block[16];
        } u;

        rijndaelDecrypt(rk, nRounds, bytesIn_p, u.block);
        u.block_32[0] ^= Ctx_p->IV[0];
        u.block_32[1] ^= Ctx_p->IV[1];
        u.block_32[2] ^= Ctx_p->IV[2];
        u.block_32[3] ^= Ctx_p->IV[3];
        c_memcpy(Ctx_p->IV, bytesIn_p, 16);
        c_memcpy(bytesOut_p, u.block_32, 16);

        bytesIn_p += 16;
        bytesOut_p += 16;
        size -= 16;
    }
}

/* end of file aes_if.c */
