/* smsim_hash_sha256.c
 *
 * Implementation of the SHA-2 hash algorithms SHA-384 and SHA-512.
 */

/*****************************************************************************
* Copyright (c) 2014-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "sb_sw_hash.h"

#define SHR(x, n)     ((x) >> (n))
#define ROTR(x, n)   (((x) >> (n)) | ((x) << (64 - n)))

#define Ch(x,y,z)  ( ((x) & (y)) | (~(x) & (z))               )
#define Maj(x,y,z) ( ((x) & (y)) | ( (x) & (z)) | ((y) & (z)) )

#define SHA512_F1(x) ( (ROTR(x,28)) ^ (ROTR(x,34)) ^ (ROTR(x,39)))
#define SHA512_F2(x) ( (ROTR(x,14)) ^ (ROTR(x,18)) ^ (ROTR(x,41)))
#define SHA512_F3(x) ( (ROTR(x,1)) ^ (ROTR(x,8)) ^ (SHR(x,7)))
#define SHA512_F4(x) ( (ROTR(x,19)) ^ (ROTR(x,61)) ^ (SHR(x,6)))

#define sha2_expand(j) \
    W[j] = SHA512_F4(W[j - 2]) + W[j - 7] + SHA512_F3(W[j - 15]) + W[j - 16]

static const uint64_t SB_SW_HASH_SHA512_K[80] =
{
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
};

static uint32_t
Swap32(
    uint32_t Value)
{
#if uECC_VLI_NATIVE_LITTLE_ENDIAN==0
    return (((Value & 0x000000FFU) << 24) |
            ((Value & 0x0000FF00U) <<  8) |
            ((Value & 0x00FF0000U) >>  8) |
            ((Value & 0xFF000000U) >> 24));
#else
    return Value;
#endif
}

/*----------------------------------------------------------------------------
 * SB_SW_HASH_SHA512_ProcessBlock
 */
void
SB_SW_HASH_SHA512_ProcessBlock(
    uint32_t * Digest_p,
    const uint8_t * DataBytes_p)
{
    uint64_t W[80] = {0};
    uint64_t A,B,C,D,E,F,G,H;

    //get the endianness correct
    {
        for (int i = 0; i <= 15; i++)
        {
            uint64_t V64 = (uint64_t)(*DataBytes_p++) << 56;
            V64 |= (uint64_t)(*DataBytes_p++) << 48;
            V64 |= (uint64_t)(*DataBytes_p++) << 40;
            V64 |= (uint64_t)(*DataBytes_p++) << 32;
            V64 |= (uint64_t)(*DataBytes_p++) << 24;
            V64 |= (uint64_t)(*DataBytes_p++) << 16;
            V64 |= (uint64_t)(*DataBytes_p++) << 8;
            V64 |= (uint64_t)*DataBytes_p++;
            W[i] = V64;
        }
    }

    // expand the 16 words into 80 temporary data words
    // (called message schedule)
    sha2_expand(16);
    sha2_expand(17);
    sha2_expand(18);
    sha2_expand(19);
    sha2_expand(20);
    sha2_expand(21);
    sha2_expand(22);
    sha2_expand(23);
    sha2_expand(24);
    sha2_expand(25);
    sha2_expand(26);
    sha2_expand(27);
    sha2_expand(28);
    sha2_expand(29);
    sha2_expand(30);
    sha2_expand(31);
    sha2_expand(32);
    sha2_expand(33);
    sha2_expand(34);
    sha2_expand(35);
    sha2_expand(36);
    sha2_expand(37);
    sha2_expand(38);
    sha2_expand(39);
    sha2_expand(40);
    sha2_expand(41);
    sha2_expand(42);
    sha2_expand(43);
    sha2_expand(44);
    sha2_expand(45);
    sha2_expand(46);
    sha2_expand(47);
    sha2_expand(48);
    sha2_expand(49);
    sha2_expand(50);
    sha2_expand(51);
    sha2_expand(52);
    sha2_expand(53);
    sha2_expand(54);
    sha2_expand(55);
    sha2_expand(56);
    sha2_expand(57);
    sha2_expand(58);
    sha2_expand(59);
    sha2_expand(60);
    sha2_expand(61);
    sha2_expand(62);
    sha2_expand(63);
    sha2_expand(64);
    sha2_expand(65);
    sha2_expand(66);
    sha2_expand(67);
    sha2_expand(68);
    sha2_expand(69);
    sha2_expand(70);
    sha2_expand(71);
    sha2_expand(72);
    sha2_expand(73);
    sha2_expand(74);
    sha2_expand(75);
    sha2_expand(76);
    sha2_expand(77);
    sha2_expand(78);
    sha2_expand(79);

    // load digest into addition variables

    uint32_t AT,BT,CT,DT,ET,FT,GT,HT,AHT,BHT,CHT,DHT,EHT,FHT,GHT,HHT;

    AT = Swap32(Digest_p[0]);
    BT = Swap32(Digest_p[1]);
    CT = Swap32(Digest_p[2]);
    DT = Swap32(Digest_p[3]);
    ET = Swap32(Digest_p[4]);
    FT = Swap32(Digest_p[5]);
    GT = Swap32(Digest_p[6]);
    HT = Swap32(Digest_p[7]);
    AHT = Swap32(Digest_p[8]);
    BHT = Swap32(Digest_p[9]);
    CHT = Swap32(Digest_p[10]);
    DHT = Swap32(Digest_p[11]);
    EHT = Swap32(Digest_p[12]);
    FHT = Swap32(Digest_p[13]);
    GHT = Swap32(Digest_p[14]);
    HHT = Swap32(Digest_p[15]);

    // load addition variables into work variables
    A = (uint64_t)AT << 32 | (uint64_t)BT;
    B = (uint64_t)CT << 32 | (uint64_t)DT;
    C = (uint64_t)ET << 32 | (uint64_t)FT;
    D = (uint64_t)GT << 32 | (uint64_t)HT;
    E = (uint64_t)AHT << 32 | (uint64_t)BHT;
    F = (uint64_t)CHT << 32 | (uint64_t)DHT;
    G = (uint64_t)EHT << 32 | (uint64_t)FHT;
    H = (uint64_t)GHT << 32 | (uint64_t)HHT;

    // compression function
    {
        int j;
        for (j = 0; j < 80; j++)
        {
            uint64_t T1, T2;
            T1 = H + SHA512_F2(E) + Ch(E, F, G) + SB_SW_HASH_SHA512_K[j] + W[j];
            T2 = SHA512_F1(A) + Maj(A, B, C);

            H = G;
            G = F;
            F = E;
            E = D + T1;
            D = C;
            C = B;
            B = A;
            A = T1 + T2;
        }
    }
    A += ((uint64_t)AT << 32 | (uint64_t)BT);
    B += ((uint64_t)CT << 32 | (uint64_t)DT);
    C += ((uint64_t)ET << 32 | (uint64_t)FT);
    D += ((uint64_t)GT << 32 | (uint64_t)HT);
    E += ((uint64_t)AHT << 32 | (uint64_t)BHT);
    F += ((uint64_t)CHT << 32 | (uint64_t)DHT);
    G += ((uint64_t)EHT << 32 | (uint64_t)FHT);
    H += ((uint64_t)GHT << 32 | (uint64_t)HHT);

    AT = (uint32_t)(A >> 32);
    BT = (uint32_t)A;

    CT = (uint32_t)(B >> 32);
    DT = (uint32_t)B;

    ET = (uint32_t)(C >> 32);
    FT = (uint32_t)C;

    GT = (uint32_t)(D >> 32);
    HT = (uint32_t)D;

    AHT = (uint32_t)(E >> 32);
    BHT = (uint32_t)E;

    CHT = (uint32_t)(F >> 32);
    DHT = (uint32_t)F;

    EHT = (uint32_t)(G >> 32);
    FHT = (uint32_t)G;

    GHT = (uint32_t)(H >> 32);
    HHT = (uint32_t)H;

    // update the digest
    Digest_p[0] = Swap32(AT);
    Digest_p[1] = Swap32(BT);
    Digest_p[2] = Swap32(CT);
    Digest_p[3] = Swap32(DT);
    Digest_p[4] = Swap32(ET);
    Digest_p[5] = Swap32(FT);
    Digest_p[6] = Swap32(GT);
    Digest_p[7] = Swap32(HT);
    Digest_p[8] = Swap32(AHT);
    Digest_p[9] = Swap32(BHT);
    Digest_p[10] = Swap32(CHT);
    Digest_p[11] = Swap32(DHT);
    Digest_p[12] = Swap32(EHT);
    Digest_p[13] = Swap32(FHT);
    Digest_p[14] = Swap32(GHT);
    Digest_p[15] = Swap32(HHT);
}

/*----------------------------------------------------------------------------
 * SB_SW_HASH_SHA384_LoadDefaultDigest
 */
#ifdef SB_SW_HASH_SHA384
void
SB_SW_HASH_SHA384_LoadDefaultDigest(
    uint32_t * Digest_p)
{
    Digest_p[0] = Swap32((uint32_t)0xcbbb9d5d);
    Digest_p[1] = Swap32((uint32_t)0xc1059ed8);

    Digest_p[2] = Swap32((uint32_t)0x629a292a);
    Digest_p[3] = Swap32((uint32_t)0x367cd507);

    Digest_p[4] = Swap32((uint32_t)0x9159015a);
    Digest_p[5] = Swap32((uint32_t)0x3070dd17);

    Digest_p[6] = Swap32((uint32_t)0x152fecd8);
    Digest_p[7] = Swap32((uint32_t)0xf70e5939);

    Digest_p[8] = Swap32((uint32_t)0x67332667);
    Digest_p[9] = Swap32((uint32_t)0xffc00b31);

    Digest_p[10] = Swap32((uint32_t)0x8eb44a87);
    Digest_p[11] = Swap32((uint32_t)0x68581511);

    Digest_p[12] = Swap32((uint32_t)0xdb0c2e0d);
    Digest_p[13] = Swap32((uint32_t)0x64f98fa7);

    Digest_p[14] = Swap32((uint32_t)0x47b5481d);
    Digest_p[15] = Swap32((uint32_t)0xbefa4fa4);
}
#endif


/*----------------------------------------------------------------------------
 * SB_SW_HASH_SHA512_LoadDefaultDigest
 */
#ifdef SB_SW_HASH_SHA512
void
SB_SW_HASH_SHA512_LoadDefaultDigest(
    uint32_t * Digest_p)
{
    Digest_p[0] = Swap32((uint32_t)0x6a09e667);
    Digest_p[1] = Swap32((uint32_t)0xf3bcc908);

    Digest_p[2] = Swap32((uint32_t)0xbb67ae85);
    Digest_p[3] = Swap32((uint32_t)0x84caa73b);

    Digest_p[4] = Swap32((uint32_t)0x3c6ef372);
    Digest_p[5] = Swap32((uint32_t)0xfe94f82b);

    Digest_p[6] = Swap32((uint32_t)0xa54ff53a);
    Digest_p[7] = Swap32((uint32_t)0x5f1d36f1);

    Digest_p[8] = Swap32((uint32_t)0x510e527f);
    Digest_p[9] = Swap32((uint32_t)0xade682d1);

    Digest_p[10] = Swap32((uint32_t)0x9b05688c);
    Digest_p[11] = Swap32((uint32_t)0x2b3e6c1f);

    Digest_p[12] = Swap32((uint32_t)0x1f83d9ab);
    Digest_p[13] = Swap32((uint32_t)0xfb41bd6b);

    Digest_p[14] = Swap32((uint32_t)0x5be0cd19);
    Digest_p[15] = Swap32((uint32_t)0x137e2179);
}
#endif

/* end of file sb_sw_hash_sha512.c */
