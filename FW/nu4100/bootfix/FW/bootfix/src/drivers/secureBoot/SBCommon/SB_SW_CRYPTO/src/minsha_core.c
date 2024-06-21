/**
* File: minsha_core.c
*
* Description : SHA-1 calculation APIs
*
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
*/

#include "implementation_defs.h"
#include "sb_sw_hash.h"
#include "sb_sw_endian.h"

#if defined(SB_SW_HASH_SHA224) || defined(SB_SW_HASH_SHA256)

#define SHR(imm,x) ((x) >> (imm))
#define ROTR(imm,x) (((x) >> (imm)) | ((x) << (32-(imm))))

/* For SHA-1 */
#define ROTL(imm,x) (((x) << (imm)) | ((x) >> (32-(imm))))

#define Ch(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define Parity(x, y, z) ((x) ^ (y) ^ (z))
#define Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

/* Worker function for forming Sigma0/1 or sigma0/1 functions. */
static uint32_t
rotr2shr(
    uint32_t x,
    uint32_t shift)
{
    uint32_t a, b, c;
    a = ROTR(shift & 0xff, x);
    b = ROTR((shift & 0xff00) >> 8, x);
    c = SHR(shift >> 16, x);

    return a ^ b ^ c;
}

#define Sigma0(x) (rotr2shr(x, 2 + 13*256 + 22*65536) ^ (x << 10))
#define Sigma1(x) (rotr2shr(x, 6 + 11*256 + 25*65536) ^ (x << 7))
#define sigma0(x) rotr2shr(x, 7 + 18*256 + 3*65536)
#define sigma1(x) rotr2shr(x, 17 + 19*256 + 10*65536)

#define CH8(a,b,c,d,e,f,g,h) \
        0x##a##u, 0x##b##u, 0x##c##u, 0x##d##u, 0x##e##u, 0x##f##u, \
        0x##g##u, 0x##h##u

static const uint32_t K[64] =
{
    CH8(428a2f98, 71374491, b5c0fbcf, e9b5dba5, 3956c25b, 59f111f1, 923f82a4, ab1c5ed5),
    CH8(d807aa98, 12835b01, 243185be, 550c7dc3, 72be5d74, 80deb1fe, 9bdc06a7, c19bf174),
    CH8(e49b69c1, efbe4786, 0fc19dc6, 240ca1cc, 2de92c6f, 4a7484aa, 5cb0a9dc, 76f988da),
    CH8(983e5152, a831c66d, b00327c8, bf597fc7, c6e00bf3, d5a79147, 06ca6351, 14292967),
    CH8(27b70a85, 2e1b2138, 4d2c6dfc, 53380d13, 650a7354, 766a0abb, 81c2c92e, 92722c85),
    CH8(a2bfe8a1, a81a664b, c24b8b70, c76c51a3, d192e819, d6990624, f40e3585, 106aa070),
    CH8(19a4c116, 1e376c08, 2748774c, 34b0bcb5, 391c0cb3, 4ed8aa4a, 5b9cca4f, 682e6ff3),
    CH8(748f82ee, 78a5636f, 84c87814, 8cc70208, 90befffa, a4506ceb, bef9a3f7, c67178f2)
};

struct abcdefgh
{
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t e;
    uint32_t f;
    uint32_t g;
    uint32_t h;
};

#endif /* SB_SW_HASH_SHA224 || SB_SW_HASH_SHA256 */

void
SB_SW_HASH_Transform(
    SB_SW_HASH_Context_t * const ctx_p,
    const void * const data_block_p)
{
#if defined(SB_SW_HASH_SHA224) || defined(SB_SW_HASH_SHA256)
    if ((ctx_p->type == 224) || (ctx_p->type == 256))
    {
        const uint32_t * const data_p = data_block_p;
        uint32_t T1;
        uint32_t T2;
        int i;
        uint32_t * state_new;
#ifdef SHA2_CONTEXT_WITH_EMBEDDED_WORKSPACE
#define W (ctx_p->W)
#define abcdefgh ctx_p->workspace
#define a (abcdefgh[0])
#define b (abcdefgh[1])
#define c (abcdefgh[2])
#define d (abcdefgh[3])
#define e (abcdefgh[4])
#define f (abcdefgh[5])
#define g (abcdefgh[6])
#define h (abcdefgh[7])
#else
        uint32_t W[64];
        struct abcdefgh abcdefgh;
#define a (abcdefgh.a)
#define b (abcdefgh.b)
#define c (abcdefgh.c)
#define d (abcdefgh.d)
#define e (abcdefgh.e)
#define f (abcdefgh.f)
#define g (abcdefgh.g)
#define h (abcdefgh.h)
#endif /* SHA2_CONTEXT_WITH_EMBEDDED_WORKSPACE */

        state_new = (uint32_t *)&a;
        for(i = 0; i < 8; i++)
        {
            state_new[i] = ctx_p->state[i];
        }

        for(i = 0; i < 16; i++)
        {
            W[i] = SB_SW_BE32_TO_CPU(data_p[i]);
        }

        L_TRACE(SB_SW_HASH, "IN %02d: SHA2: %08x %08x %08x %08x",
                0, W[0], W[1], W[2], W[3]);

        L_TRACE(SB_SW_HASH, "IN %02d: SHA2: %08x %08x %08x %08x",
                1, W[4], W[5], W[6], W[7]);

        L_TRACE(SB_SW_HASH, "IN %02d: SHA2: %08x %08x %08x %08x",
                2, W[8], W[9], W[10], W[11]);

        L_TRACE(SB_SW_HASH, "IN %02d: SHA2: %08x %08x %08x %08x",
                3, W[12], W[13], W[14], W[15]);

        for(; i < 64; i++)
        {
            W[i] = sigma1(W[i - 2]) + W[i - 7] + sigma0(W[i - 15]) + W[i - 16];
        }

        for(i = 0; i < 64; i++)
        {
            T1 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];
            T2 = Sigma0(a) + Maj(a, b, c);

            h = g;
            g = f;
            f = e;
            e = d + T1;
            d = c;
            c = b;
            b = a;
            a = T1 + T2;

            L_TRACE(SB_SW_HASH,
                    "HASH: R%02d Key: %08x T1: %08x T2: %08x abe=%08x %08x %08x",
                    i, K[i], T1, T2, a, b, e);
        }

        for(i = 0; i < 8; i++)
        {
            ctx_p->state[i] += state_new[i];
        }
    }
#endif
#if defined(SB_SW_HASH_SHA384) || defined(SB_SW_HASH_SHA512)
#if defined(SB_SW_HASH_SHA224) || defined(SB_SW_HASH_SHA256)
    else
#endif
    {
        /* sha384 or sha512 */
        const uint8_t * data_p = data_block_p;
        SB_SW_HASH_SHA512_ProcessBlock(ctx_p->state, data_p);
    }
#endif
}
