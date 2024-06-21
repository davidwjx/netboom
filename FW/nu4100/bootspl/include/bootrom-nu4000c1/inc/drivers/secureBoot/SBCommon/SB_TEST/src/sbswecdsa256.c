/**
*  File: sbswecdsa256.c
*
*  Description : Tests for SB using ECDSA 256
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
#include "sfzutf.h"
#include "sfzutf-utils.h"
#include "sb_sw_hash.h"
#include "c_lib.h"

#include "lib-sbswecdsatest.h"
#include "sb_sw_ecdsa.h"

#define IX256(w7,w6,w5,w4,w3,w2,w1,w0)          \
    (uint32_t)0x##w0##u,                        \
    (uint32_t)0x##w1##u,                        \
    (uint32_t)0x##w2##u,                        \
    (uint32_t)0x##w3##u,                        \
    (uint32_t)0x##w4##u,                        \
    (uint32_t)0x##w5##u,                        \
    (uint32_t)0x##w6##u,                        \
    (uint32_t)0x##w7##u

/* Co-prime */
static uint32_t n256[256 / 32] =
{
    IX256(FFFFFFFF, 00000000, FFFFFFFF, FFFFFFFF,
          BCE6FAAD, A7179E84, F3B9CAC2, FC632551)
};

/* Coordinates of generator point. */
static uint32_t px256[256 / 32] =
{
    IX256(6B17D1F2, E12C4247, F8BCE6E5, 63A440F2,
          77037D81, 2DEB33A0, F4A13945, D898C296)
};

static uint32_t py256[256 / 32] =
{
    IX256(4FE342E2, FE1A7F9B, 8EE7EB4A, 7C0F9E16,
          2BCE3357, 6B315ECE, CBB64068, 37BF51F5)
};

/* This vector is from RFC 4754 (Section 8.1) */

START_TEST(SB_SW_ECDSA_Verify_RFC_4754)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(86FA3BB4, E26CAD5B, F90B7F81, 899256CE,
              7594BB1E, A0C89212, 748BFF3B, 3D5B0315)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(CB28E099, 9B9C7715, FD0A80D8, E47A7707,
              9716CBBF, 917DD72E, 97566EA1, C066957C)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(2442A5CC, 0ECD015F, A3CA31DC, 8E2BBC70,
              BF42D60C, BCA20085, E0822CB0, 4235E970)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(6FC98BD7, E50211A4, A27102FA, 3549DF79,
              EBCB4BF2, 46B80945, CDDFE7D5, 09BBFD7D)
    };

    sbswecdsa("abc", SUPS_TEXT, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

/* These vectors are from FIPS ecdsatestvectors.zip. */

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_1)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(fa15e819, ca05a6b2, 786c7626, 2bf7371c,
              ef97b218, e96f175a, 3ccdda2a, cc058903)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(f3ac8061, b514795b, 8843e3d6, 629527ed,
              2afd6b1f, 6a555a7a, cabb5e6f, 79c8c2ac)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(1ccbe91c, 075fc7f4, f033bfa2, 48db8fcc,
              d3565de9, 4bbfb12f, 3c59ff46, c271bf83)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(ce4014c6, 8811f9a2, 1a1fdb2c, 0e6113e0,
              6db7ca93, b7404e78, dc7ccd5c, a89a4ca9)
    };

    sbswecdsa("5905238877c77421f73e43ee3da6f2d9e2ccad5fc942dcec0cbd25482935f"
              "aaf416983fe165b1a045ee2bcd2e6dca3bdf46c4310a7461f9a37960ca672"
              "d3feb5473e253605fb1ddfd28065b53cb5858a8ad28175bf9bd386a5e471e"
              "a7a65c17cc934a9d791e91491eb3754d03799790fe2d308d16146d5c9b0d0"
              "debd97d79ce8", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_2)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(1b766e9c, eb71ba6c, 01dcd46e, 0af462cd,
              4cfa652a, e5017d45, 55b8eeef, e36e1932)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(fa15ea4e, 9d23326d, c0baa9fa, 560b7c4e,
              53f42864, f508483a, 6473b6a1, 1079b2db)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(e266ddfd, c12668db, 30d4ca3e, 8f774943,
              2c416044, f2d2b8c1, 0bf3d401, 2aeffa8a)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(bfa86404, a2e9ffe6, 7d47c587, ef7a97a7,
              f456b863, b4d02cfc, 6928973a, b5b1cb39)
    };

    sbswecdsa("c35e2f092553c55772926bdbe87c9796827d17024dbb9233a545366e2e598"
              "7dd344deb72df987144b8c6c43bc41b654b94cc856e16b96d7a821c8ec039"
              "b503e3d86728c494a967d83011a0e090b5d54cd47f4e366c0912bc808fbb2"
              "ea96efac88fb3ebec9342738e225f7c7c2b011ce375b56621a20642b4d36e"
              "060db4524af1", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_3)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(ee59d81b, c9db1055, cc0ed97b, 159d8784,
              af04e985, 11d0a9a4, 07b99bb2, 92572e96)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(35fb60f5, ca0f3ca0, 8542fb3c, c641c826,
              3a2cab7a, 90ee6a5e, 1583fac2, bb6f6bd1)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(fa15e8a6, 2fba0e66, 7c50929a, 53f78c21,
              b8ff0c3c, 737b0b40, b1750b23, 02b0bde8)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(29074e21, f3a0ef88, b9efdf10, d06aa4c2,
              95cc1671, f758ca0e, 4cd10880, 3d0f2614)
    };

    sbswecdsa("3c054e333a94259c36af09ab5b4ff9beb3492f8d5b4282d16801daccb29f7"
              "0fe61a0b37ffef5c04cd1b70e85b1f549a1c4dc672985e50f43ea037efa99"
              "64f096b5f62f7ffdf8d6bfb2cc859558f5a393cb949dbd48f269343b5263d"
              "cdb9c556eca074f2e98e6d94c2c29a677afaf806edf79b15a3fcd46e7067b"
              "7669f83188ee", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_4)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(b46d9f2d, 8c4bf835, 46ff178f, 1d78937c,
              008d64e8, ecc5cbb8, 25cb21d9, 4d670d89)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(d7c56237, 0af617b5, 81c84a24, 68cc8bd5,
              0bb1cbf3, 22de41b7, 887ce07c, 0e5884ca)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(322f8037, 1bf6e044, bc49391d, 97c1714a,
              b87f990b, 949bc178, cb7c43b7, c22d89e1)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(fa15e54a, 5cc6b9f0, 9de8457e, 873eb3de,
              b1fceb54, b0b295da, 6050294f, ae7fd999)
    };

    sbswecdsa("0989122410d522af64ceb07da2c865219046b4c3d9d99b01278c07ff63eaf"
              "1039cb787ae9e2dd46436cc0415f280c562bebb83a23e639e476a02ec8cff"
              "7ea06cd12c86dcc3adefbf1a9e9a9b6646c7599ec631b0da9a60debeb9b3e"
              "19324977f3b4f36892c8a38671c8e1cc8e50fcd50f9e51deaf98272f9266f"
              "c702e4e57c30", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_5)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(77c68928, ac3b88d9, 85fb43fb, 615fb7ff,
              45c18ba5, c81af796, c613dfa9, 8352d29c)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(18caaf7b, 663507a8, bcd992b8, 36dec9dc,
              5703c080, af5e51df, a3a9a7c3, 87182604)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(1bcec457, 0e1ec243, 6596b8de, d58f60c3,
              b1ebc6a4, 03bc5543, 040ba829, 63057244)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(8af62a4c, 683f096b, 28558320, 737bf83b,
              9959a46a, d2521004, ef74cf85, e67494e1)
    };

    sbswecdsa("dc66e39f9bbfd9865318531ffe9207f934fa615a5b285708a5e9c46b77751"
              "50e818d7f24d2a123df3672fff2094e3fd3df6fbe259e3989dd5edfcccbe7"
              "d45e26a775a5c4329a084f057c42c13f3248e3fd6f0c76678f890f513c322"
              "92dd306eaa84a59abe34b16cb5e38d0e885525d10336ca443e1682aa04a7a"
              "f832b0efa15e", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_6)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(d18c2caf, 3b1072f8, 7064ec5e, 8953f513,
              01cada03, 469c6402, 44760328, eb5a05cb)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(8524c502, 4e2d9a73, bde8c72d, 9129f578,
              73bbad0e, d05215a3, 72a84fdb, c78f2e68)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(a32e50be, 3dae2c8b, a3f5e4bd, ae14cf76,
              45420d42, 5ead9403, 6c22dd6c, 4fc59e00)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(d623bf64, 1160c289, d6742c62, 57ae6ba5,
              74446dd1, d0e74db3, aaa80900, b78d4ae9)
    };

    sbswecdsa("600974e7d8c5508e2c1aab0783ad0d7c4494ab2b4da265c2fe496421c4df2"
              "38b0be25f25659157c8a225fb03953607f7df996acfd402f147e37aee2f16"
              "93e3bf1c35eab3ae360a2bd91d04622ea47f83d863d2dfecb618e8b8bdc39"
              "e17d15d672eee03bb4ce2cc5cf6b217e5faf3f336fdd87d972d3a8b8a593b"
              "a85955cc9d71", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_7)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(84a58f9e, 9d9e7353, 44b316b1, aa1ab518,
              5665b851, 47dc82d9, 2e969d7b, ee31ca30)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(c5a186d7, 2df45201, 5480f7f3, 38970bfe,
              825087f0, 5c0088d9, 5305f87a, acc9b254)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(8bcfe2a7, 21ca6d75, 3968f564, ec4315be,
              4857e28b, ef1908f6, 1a366b1f, 03c97479)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(0f67576a, 30b8e20d, 4232d853, 0b52fb4c,
              89cbc589, ede291e4, 99ddd15f, e870ab96)
    };

    sbswecdsa("dfa6cb9b39adda6c74cc8b2a8b53a12c499ab9dee01b4123642b4f11af336"
              "a91a5c9ce0520eb2395a6190ecbf6169c4cba81941de8e76c9c908eb843b9"
              "8ce95e0da29c5d4388040264e05e07030a577cc5d176387154eabae2af52a"
              "83e85c61c7c61da930c9b19e45d7e34c8516dc3c238fddd6e450a77455d53"
              "4c48a152010b", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_8)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(d7f9ddd1, 91f1f412, 86942920, 9ee3814c,
              75c72fa4, 6a9cccf8, 04a2f5cc, 0b7e739f)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(9d0c6afb, 6df3bced, 455b459c, c21387e1,
              49293926, 64bb8741, a3693a17, 95ca6902)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(a88bc843, 0279c8c0, 400a77d7, 51f26c0a,
              bc93e5de, 4ad9a416, 6357952f, e041e767)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(2d365a1e, ef25ead5, 79cc9a06, 9b6abc1b,
              16b81c35, f18785ce, 26a10ba6, d1381185)
    };

    sbswecdsa("51d2547cbff92431174aa7fc7302139519d98071c755ff1c92e4694b58587"
              "ea560f72f32fc6dd4dee7d22bb7387381d0256e2862d0644cdf2c277c5d74"
              "0fa089830eb52bf79d1e75b8596ecf0ea58a0b9df61e0c9754bfcd62efab6"
              "ea1bd216bf181c5593da79f10135a9bc6e164f1854bc8859734341aad237b"
              "a29a81a3fc8b", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_9)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(f5413bfd, 85949da8, d83de83a, b0d19b29,
              86613e22, 4d1901d7, 6919de23, ccd03199)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(2f9e2b4e, 9f747c65, 7f705bff, d124ee17,
              8bbc5391, c86d0567, 17b140c1, 53570fd9)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(1bc48757, 0f040dc9, 4196c9be, fe8ab2b6,
              de77208b, 1f38bdaa, e28f9645, c4d2bc3a)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(ec81602a, bd8345e7, 1867c821, 03137378,
              65b8aa18, 6851e1b4, 8eaca140, 320f5d8f)
    };

    sbswecdsa("558c2ac13026402bad4a0a83ebc9468e50f7ffab06d6f981e5db1d0820980"
              "65bcff6f21a7a74558b1e8612914b8b5a0aa28ed5b574c36ac4ea5868432a"
              "62bb8ef0695d27c1e3ceaf75c7b251c65ddb268696f07c16d2767973d85be"
              "b443f211e6445e7fe5d46f0dce70d58a4cd9fe70688c035688ea8c6baec65"
              "a5fc7e2c93e8", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_10)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(880e0bbf, 82a8cf81, 8ed46ba0, 3cf0fc6c,
              898e36fc, a36cc7fd, b1d2db75, 03634430)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(1cc62853, 3d0004b2, b20e7f4b, aad0b8bb,
              5e0673db, 159bbccf, 92491aef, 61fc9620)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(b8188bd6, 8701fc39, 6dab5312, 5d4d28ea,
              33a91daf, 6d21485f, 4770f6ea, 8c565dde)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(423f0588, 10f277f8, fe076f6d, b56e9285,
              a1bf2c2a, 1dae1450, 95edd9c0, 4970bc4a)
    };

    sbswecdsa("4d55c99ef6bd54621662c3d110c3cb627c03d6311393b264ab97b90a4b152"
              "14a5593ba2510a53d63fb34be251facb697c973e11b665cb7920f1684b003"
              "1b4dd370cb927ca7168b0bf8ad285e05e9e31e34bc24024739fdc10b78586"
              "f29eff94412034e3b606ed850ec2c1900e8e68151fc4aee5adebb066eb6da"
              "4eaa5681378e", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_11)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(2bf3a80b, c04faa35, ebecc0f4, 864ac02d,
              349f6f12, 6e0f9885, 01b8d307, 5409a26c)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(9886ae46, c1415c3b, c959e82b, 760ad760,
              aab66885, a84e620a, a339fdf1, 02465c42)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(51f99d2d, 52d4a6e7, 34484a01, 8b7ca2f8,
              95c2929b, 6754a3a0, 3224d07a, e61166ce)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(4737da96, 3c6ef724, 7fb88d19, f9b0c667,
              cac7fe12, 837fdab8, 8c66f10d, 3c14cad1)
    };

    sbswecdsa("f8248ad47d97c18c984f1f5c10950dc1404713c56b6ea397e01e6dd925e90"
              "3b4fadfe2c9e877169e71ce3c7fe5ce70ee4255d9cdc26f6943bf48687874"
              "de64f6cf30a012512e787b88059bbf561162bdcc23a3742c835ac144cc141"
              "67b1bd6727e940540a9c99f3cbb41fb1dcb00d76dda04995847c657f4c19d"
              "303eb09eb48a", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_12)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(644300fc, 0da4d40f, b8c6ead5, 10d14f0b,
              d4e1321a, 469e9c0a, 581464c7, 186b7aa7)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(490efd10, 6be11fc3, 65c7467e, b89b8d39,
              e15d6517, 5356775d, eab21116, 3c2504cb)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(8fb287f0, 202ad57a, e841aea3, 5f29b2e1,
              d53e196d, 0ddd9aec, 24813d64, c0922fb7)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(1f6daff1, aa2dd2d6, d3741623, eecb5e7b,
              612997a1, 039aab2e, 5cf2de96, 9cfea573)
    };

    sbswecdsa("3b6ee2425940b3d240d35b97b6dcd61ed3423d8e71a0ada35d47b322d17b3"
              "5ea0472f35edd1d252f87b8b65ef4b716669fc9ac28b00d34a9d66ad118c9"
              "d94e7f46d0b4f6c2b2d339fd6bcd351241a387cc82609057048c12c4ec3d8"
              "5c661975c45b300cb96930d89370a327c98b67defaa89497aa8ef994c77f1"
              "130f752f94a4", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_13)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(9ae6ba6d, 63770684, 9a6a9fc3, 88cf0232,
              d85c26ea, 0d1fe743, 7adb48de, 58364333)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(e67a9717, ccf96841, 489d6541, f4f6adb1,
              2d17b59a, 6bef847b, 6183b8fc, f16a32eb)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(68229b48, c2fe19d3, db034e4c, 15077eb7,
              471a6603, 1f28a980, 82187391, 5298ba76)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(303e8ee3, 742a893f, 78b81099, 1da69708,
              3dd8f111, 28c47651, c27a5674, 0a80c24c)
    };

    sbswecdsa("c5204b81ec0a4df5b7e9fda3dc245f98082ae7f4efe81998dcaa286bd4507"
              "ca840a53d21b01e904f55e38f78c3757d5a5a4a44b1d5d4e480be3afb5b39"
              "4a5d2840af42b1b4083d40afbfe22d702f370d32dbfd392e128ea4724d66a"
              "3701da41ae2f03bb4d91bb946c7969404cb544f71eb7a49eb4c4ec55799bd"
              "a1eb545143a7", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_14)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(d7c246dc, 7ad0e677, 00c373ed, cfdd1c0a,
              0495fc95, 4549ad57, 9df6ed14, 38840851)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(b53ce4da, 1aa7c0dc, 77a1896a, b716b921,
              499aed78, df725b15, 04aba159, 7ba0c64b)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(0a7dbb8b, f50cb605, eb2268b0, 81f26d6b,
              08e012f9, 52c4b70a, 5a1e6e7d, 46af98bb)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(f26dd7d7, 99930062, 48084996, 2ccf5004,
              edcfd307, c044f4e8, f667c9ba, a834eeae)
    };

    sbswecdsa("72e81fe221fb402148d8b7ab03549f1180bcc03d41ca59d7653801f0ba853"
              "add1f6d29edd7f9abc621b2d548f8dbf8979bd16608d2d8fc3260b4ebc0dd"
              "42482481d548c7075711b5759649c41f439fad69954956c9326841ea64929"
              "56829f9e0dc789f73633b40f6ac77bcae6dfc7930cfe89e526d1684365c5b"
              "0be2437fdb01", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P256_15)
{
    static const uint32_t s[256 / 32] =
    {
        IX256(4f7f6530, 5e24a6bb, b5cff714, ba8f5a2c,
              ee5bdc89, ba8d75dc, bf21966c, e38eb66f)
    };
    static const uint32_t r[256 / 32] =
    {
        IX256(542c40a1, 8140a626, 6d6f0286, e24e9a7b,
              ad7650e7, 2ef0e213, 1e629c07, 6d962663)
    };
    static const uint32_t qx[256 / 32] =
    {
        IX256(105d22d9, c626520f, aca13e7c, ed382dcb,
              e9349831, 5f00cc0a, c39c4821, d0d73737)
    };
    static const uint32_t qy[256 / 32] =
    {
        IX256(6c47f3cb, bfa97dfc, ebe16270, b8c7d5d3,
              a5900b88, 8c42520d, 751e8faf, 3b401ef4)
    };

    sbswecdsa("21188c3edd5de088dacc1076b9e1bcecd79de1003c2414c3866173054dc82"
              "dde85169baa77993adb20c269f60a5226111828578bcc7c29e6e8d2dae818"
              "06152c8ba0c6ada1986a1983ebeec1473a73a04795b6319d48662d40881c1"
              "723a706f516fe75300f92408aa1dc6ae4288d2046f23c1aa2e54b7fb6448a"
              "0da922bd7f34", SUPS_LSBF, n256, s, r, px256, py256,
              qx, qy, true);
}
END_TEST

bool pointcheck(const uint32_t *qx,
                const uint32_t *qy,
                bool full_check,
                const uint32_t *order)
{
    struct SB_SW_ECDSA_Point256 Q;

    PARAMETER_NOT_USED(order);
    PARAMETER_NOT_USED(full_check);

    c_memcpy(&Q.x, qx, sizeof(Q.x));
    c_memcpy(&Q.y, qy, sizeof(Q.y));

    return SB_SW_ECDSA_Point_Check256((const uint8_t*)&Q);
}

START_TEST(SB_SW_ECDSA_PointCheck)
{
    static const uint32_t qx1[256 / 32] =
    {
        IX256(1ccbe91c, 075fc7f4, f033bfa2, 48db8fcc,
              d3565de9, 4bbfb12f, 3c59ff46, c271bf83)
    };
    static const uint32_t qy1[256 / 32] =
    {
        IX256(ce4014c6, 8811f9a2, 1a1fdb2c, 0e6113e0,
              6db7ca93, b7404e78, dc7ccd5c, a89a4ca9)
    };

    static const uint32_t qx2[256 / 32] =
    {
        IX256(a32e50be, 3dae2c8b, a3f5e4bd, ae14cf76,
              45420d42, 5ead9403, 6c22dd6c, 4fc59e00)
    };
    static const uint32_t qy2[256 / 32] =
    {
        IX256(d623bf64, 1160c289, d6742c62, 57ae6ba5,
              74446dd1, d0e74db3, aaa80900, b78d4ae9)
    };

    static const uint32_t qx3[256 / 32] =
    {
        IX256(a88bc843, 0279c8c0, 400a77d7, 51f26c0a,
              bc93e5de, 4ad9a416, 6357952f, e041e767)
    };
    static const uint32_t qy3[256 / 32] =
    {
        IX256(2d365a1e, ef25ead5, 79cc9a06, 9b6abc1b,
              16b81c35, f18785ce, 26a10ba6, d1381185)
    };

    /* Single bit changed. */
    static const uint32_t qx3b[256 / 32] =
    {
        IX256(a88bc843, 0279c8c0, 400a77d7, 51f26c0b,
              bc93e5de, 4ad9a416, 6357952f, e041e767)
    };
    static const uint32_t qy3b[256 / 32] =
    {
        IX256(2d365a1e, ef25ead5, 79cc9a06, 9b6abc1b,
              16b81c35, f18785ce, 26a10ba6, d1381185)
    };

    fail_if(pointcheck(qx1, qy1, false, n256) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, false, n256) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, false, n256) == false,
            "Qx/Qy from vector 3 (functional vector)");

    fail_if(pointcheck(qx1, qy1, true, n256) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, true, n256) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, true, n256) == false,
            "Qx/Qy from vector 3 (functional vector)");

    /* Same as qx3/qy3, but with single bit changed from qx. */
    fail_if(pointcheck(qx3b, qy3b, false, n256) == true,
            "Qx/Qy accepted from altered public key");
    fail_if(pointcheck(qx3b, qy3b, true, n256) == true,
            "Qx/Qy accepted from altered public key");

    /* Odd combination: correct Qx and Qy, but from mixed vectors: */
    fail_if(pointcheck(qx2, qy3, false, n256) == true,
            "Qx/Qy accepted from mixed public key");
    fail_if(pointcheck(qx2, qy3, true, n256) == true,
            "Qx/Qy accepted from mixed public key");
}
END_TEST

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_ECDSA256_Tests");
    sfzutf_tcase_create("SB_SW_ECDSA256_Tests_P256");
    sfzutf_test_add(SB_SW_ECDSA_Verify_RFC_4754);

    sfzutf_tcase_create("SB_SW_ECDSA_Verify_NIST_P256");
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_1);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_2);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_3);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_4);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_5);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_6);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_7);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_8);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_9);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_10);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_11);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_12);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_13);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_14);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P256_15);
    sfzutf_test_add(SB_SW_ECDSA_PointCheck);
}

