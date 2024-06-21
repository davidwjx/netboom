/* sbswecdsa224.c
 *
 * Description: Tests for SB using ECDSA 224
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
#include "sfzutf.h"
#include "sfzutf-utils.h"
#include "sb_sw_hash.h"
#include "c_lib.h"

#include "lib-sbswecdsatest.h"
#include "sb_sw_ecdsa.h"

#define IX224(w6,w5,w4,w3,w2,w1,w0) \
    (uint32_t)0x##w0##u, \
    (uint32_t)0x##w1##u, \
    (uint32_t)0x##w2##u, \
    (uint32_t)0x##w3##u, \
    (uint32_t)0x##w4##u, \
    (uint32_t)0x##w5##u, \
    (uint32_t)0x##w6##u

/* Co-prime */
static uint32_t n224[224 / 32] =
{
    IX224(FFFFFFFF, FFFFFFFF, FFFFFFFF, FFFF16A2, E0B8F03E, 13DD2945, 5C5C2A3D)
};

/* Coordinates of generator point. */
static uint32_t px224[224 / 32] =
{
    IX224(b70e0cbd, 6bb4bf7f, 321390b9, 4a03c1d3, 56c21122, 343280d6, 115c1d21)
};

static uint32_t py224[224 / 32] =
{
    IX224(bd376388, b5f723fb, 4c22dfe6, cd4375a0, 5a074764, 44d58199, 85007e34)
};


/* These vectors are from FIPS ecdsatestvectors.zip. */

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_1)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(fa15e4c8, ea93e0fd, 9d6431b9, a1fd99b8, 8f281793, 396321b1, 1dac41eb)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(2fc2cff8, cdd4866b, 1d74e45b, 07d333af, 46b7af08, 88049d0f, dbc7b0d6)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(60549575, 6e6e88f1, d07ae5f9, 8787af9b, 4da8a641, d1a9492a, 12174eab)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(f5cc733b, 17decc80, 6ef1df86, 1a42505d, 0af9ef7c, 3df3959b, 8dfc6669)
    };

    sbswecdsa("699325d6fc8fbbb4981a6ded3c3a54ad2e4e3db8a5669201912064c64e700"
              "c139248cdc19495df081c3fc60245b9f25fc9e301b845b3d703a694986e46"
              "41ae3c7e5a19e6d6edbf1d61e535f49a8fad5f4ac26397cfec682f161a5fc"
              "d32c5e780668b0181a91955157635536a22367308036e2070f544ad4fff3d"
              "5122c76fad5d", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_2)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(7302dff1, 2545b069, cf27df49, b26e4781, 27058546, 3656f283, 4917c3ca)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(fa15ef06, b566ec9f, d0fee1b6, c6551a45, 35c7a3bb, fc0fede4, 5f4f5038)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(fa21f85b, 99d3dc18, c6d53351, fbcb1e2d, 029c00fa, 7d1663a3, dd94695e)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(e9e79578, f8988b16, 8edff1a8, b34a5ed9, 598cc20a, cd1f0aed, 36715d88)
    };

    sbswecdsa("7de42b44db0aa8bfdcdac9add227e8f0cc7ad1d94693beb5e1d325e5f3f85"
              "b3bd033fc25e9469a89733a65d1fa641f7e67d668e7c71d736233c4cba20e"
              "b83c368c506affe77946b5e2ec693798aecd7ff943cd8fab90affddf5ad5b"
              "8d1af332e6c5fe4a2df16837700b2781e08821d4fbdd8373517f5b19f9e63"
              "b89cfeeeef6f", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_3)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(a5d71bff, 02dce997, 305dd337, 128046f3, 6714398f, 4ef66475, 99712fae)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(41f8e2b1, ae5add7c, 24da8725, a067585a, 3ad6d5a9, ed9580be, b226f23a)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(fa15e99b, 060afb0d, 9dbf3250, ea3c4da1, 0be94ce6, 27a65874, d8e4a630)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(e8373ab7, 19089032, 6aac4aac, ca3eba89, e15d1086, a05434dd, 033fd3f3)
    };

    sbswecdsa("af0da3adab82784909e2b3dadcecba21eced3c60d7572023dea171044d9a1"
              "0e8ba67d31b04904541b87fff32a10ccc6580869055fec6216a00320a2889"
              "9859a6b61faba58a0bc10c2ba07ea16f214c3ddcc9fc5622ad1253b63fe7e"
              "95227ae3c9caa9962cffc8b1c4e8260036469d25ab0c8e3643a820b8b3a4d"
              "8d43e4b728f9", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_4)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(866fb8e5, 05dea6c9, 09c2c914, 3ec869d1, bac2282c, f1236613, 0ff2146c)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(2258184e, f9f0fa69, 87353799, 72ce9adf, 034af760, 17668bfc, dab978de)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(fad0a349, 91bbf899, 82ad9cf8, 9337b4bd, 2565f84d, 5bdd0042, 89fc1cc3)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(fa15e764, f28c8163, a12855a5, c266efeb, 9388df49, 94b85a8b, 4f1bd3bc)
    };

    sbswecdsa("cfa56ae89727df6b7266f69d6636bf738f9e4f15f49c42a0123edac4b3743"
              "f32ea52389f919ceb90575c4184897773b2f2fc5b3fcb354880f15c933832"
              "15d3c2551fcc1b4180a1ac0f69c969bbc306acd115ce3976eff518540f43a"
              "d4076dbb5fbad9ce9b3234f1148b8f5e059192ff480fc4bcbd00d25f4d9f5"
              "ed4ba5693b6c", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_5)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(26613d3b, 33c70e63, 5d7a998f, 254a5b15, d2a3642b, f321e8cf, f08f1e84)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(86622c37, 6d326cdf, 679bcabf, 8eb034bf, 49f0c188, f3fc3afd, 0006325d)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(0e0fc15e, 775a75d4, 5f872e50, 21b554cc, 0579da19, 125e1a49, 299c7630)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(cb64fe46, 2d025ae2, a1394746, bdbf8251, f7ca5a1d, 6bb13e0e, df6b7b09)
    };

    sbswecdsa("fa15e8009018321b987a615c3414d2bb15954933569ca989de32d6bf11107"
              "bc47a330ab6d88d9b50d106cf5777d1b736b14bc48deda1bc573a9a7dd42c"
              "d061860645306dce7a5ba8c60f135a6a21999421ce8c4670fe7287a7e9ea3"
              "aa1e0fa82721f33e6e823957fe86e2283c89ef92b13cd0333c4bb70865ae1"
              "919bf538ea34", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_6)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(eb6cd586, 50cfb26d, fdf21de3, 2fa17464, a6efc468, 30eedc16, 977342e6)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(57afda51, 39b180de, 96373c3d, 64970068, 2e37efd5, 6ae18233, 5f081013)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(2c070e68, e8478341, 938f3d50, 26a1fe01, e778cdff, bebbdd7a, 4cd29209)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(cde21c9c, 7c6590ba, 300715a7, adac2783, 85a5175b, 6b4ea749, c4b6a681)
    };

    sbswecdsa("1c27273d95182c74c100d85b5c08f4b26874c2abc87f127f304aedbf52ef6"
              "540eba16dd664ae1e9e30ea1e66ff9cc9ab5a80b5bcbd19dde88a29ff10b5"
              "0a6abd73388e8071306c68d0c9f6caa26b7e68de29312be959b9f4a5481f5"
              "a2ad2070a396ed3de21096541cf58c4a13308e08867565bf2df9d649357a8"
              "3cdcf18d2cd9", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_7)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(1e745e80, 948779a5, cc8dc5cb, 193beebb, 550ec9c2, 647f4948, bf58ba7d)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(09bbdd00, 3532d025, d7c3204c, 00747cd5, 2ecdfbc7, ce3dde8f, fbea23e1)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(3a0d4b8e, 5fad1ea1, abb8d3fb, 742cd45c, d0b76d13, 6e5bbb33, 206ad120)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(c90ac832, 76b2fa37, 57b0f226, cd7360a3, 13bc96fd, 8329c76a, 7306cc7d)
    };

    sbswecdsa("069ae374971627f6b8503f3aa63ab52bcf4f3fcae65b98cdbbf917a5b08a1"
              "0dc760056714db279806a8d43485320e6fee0f1e0562e077ee270ace8d3c4"
              "78d79bcdff9cf8b92fdea68421d4a276f8e62ae379387ae06b60af9eb3c40"
              "bd7a768aeffccdc8a08bc78ca2eca18061058043a0e441209c5c594842838"
              "a4d9d778a053", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_8)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(09490be7, 7bc834c1, efaa2341, 0dcbf800, e6fae40d, 62a73721, 4c5a4418)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(ff6d52a0, 9ca4c3b8, 2da04408, 64d6717e, 1be0b50b, 6dcf5e1d, 74c0ff56)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(4772c27c, ca3348b1, 801ae87b, 01cb564c, 8cf9b81c, 23cc7446, 8a907927)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(de9d2539, 35b09617, a1655c42, d385bf48, 504e06fa, 386f5fa5, 33a21dcb)
    };

    sbswecdsa("d0d5ae3e33600aa21c1606caec449eee678c87cb593594be1fbb048cc7cfd"
              "076e5cc7132ebe290c4c014e7a517a0d5972759acfa1438d9d2e5d236d19a"
              "c92136f6252b7e5bea7588dcba6522b6b18128f003ecab5cb4908832fb5a3"
              "75cf820f8f0e9ee870653a73dc2282f2d45622a2f0e85cba05c567baf1b98"
              "62b79a4b244e", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_9)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(e3823699, c355b61a, b1894be3, 371765fa, e2b72040, 5a7ce5e7, 90ca8c00)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(70ccc082, 4542e296, d17a7932, 0d422f1e, dcf92538, 40dafe44, 27033f40)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(1f249911, b125348e, 6e0a4734, 79105cc4, b8cfb4fa, 32d89781, 0fc69ffe)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(a17db03b, 9877d1b6, 32832906, 1ea67aec, 5a38a884, 362e9e5b, 7d7642dc)
    };

    sbswecdsa("79b7375ae7a4f2e4adad8765d14c1540cd9979db38076c157c1837c760ca6"
              "febbb18fd42152335929b735e1a08041bd38d315cd4c6b7dd2729de8752f5"
              "31f07fe4ddc4f1899debc0311eef0019170b58e08895b439ddf09fbf0aeb1"
              "e2fd35c2ef7ae402308c3637733802601dd218fb14c22f57870835b108183"
              "69d57d318405", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_10)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(8c01f016, 2891e4b9, 536243cb, 86a6e5c1, 77323cca, 09777366, caf2693c)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(eef9e842, 81057041, 33e0f196, 36c89e57, 0485e577, 786df2b0, 9f99602a)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(7df67b96, 0ee7a2cb, 62b22932, 457360ab, 1e046c1e, c84b91ae, 65642003)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(c764ca9f, c1b0cc22, 33fa57bd, cfedaab0, 131fb7b5, f557d6ca, 57f4afe0)
    };

    sbswecdsa("8c7de96e6880d5b6efc19646b9d3d56490775cb3faab342e64db2e388c4bd"
              "9e94c4e69a63ccdb7e007a19711e69c06f106b71c983a6d97c4589045666c"
              "6ab5ea7b5b6d096ddf6fd35b819f1506a3c37ddd40929504f9f079c8d8382"
              "0fc8493f97b2298aebe48fdb4ff472b29018fc2b1163a22bfbb1de413e864"
              "5e871291a9f6", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_11)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(517a91b8, 9c4859fc, c1083424, 2e710c5f, 0fed90ac, 938aa5cc, db7c66de)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(1d293b69, 7f297af7, 7872582e, b7f543dc, 250ec79a, d453300d, 264a3b70)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(b10150fd, 797eb870, d377f1db, fa197f7d, 0f0ad299, 65af573e, c13cc42a)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(17b63cce, fbe27fb2, a1139e57, 57b1082a, eaa564f4, 78c23a8f, 631eed5c)
    };

    sbswecdsa("c89766374c5a5ccef5823e7a9b54af835ac56afbbb517bd77bfecf3fea876"
              "bd0cc9ea486e3d685cfe3fb05f25d9c67992cd7863c80a55c7a263249eb39"
              "96c4698ad7381131bf3700b7b24d7ca281a100cf2b750e7f0f933e662a08d"
              "9f9e47d779fb03754bd20931262ff381a2fe7d1dc94f4a0520de73fa72020"
              "494d3133ecf7", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_12)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(fdd81e5d, ca541585, 14f44ba2, 330271ef, f4c61833, 0328451e, 2d93b9fb)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(4bdd3c84, 647bad93, dcaffd1b, 54eb87fc, 61a5704b, 19d7e6d7, 56d11ad0)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(3c9b4ef1, 748a1925, 578658d3, af51995b, 989ad760, 790157b2, 5fe09826)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(55648f4f, f4edfb89, 9e9a13bd, 8d20f5c2, 4b35dc6a, 6a4e42ed, 5983b4a0)
    };

    sbswecdsa("30f0e3b502eec5646929d48fd46aa73991d82079c7bd50a38b38ec0bd8416"
              "7c8cf5ba39bec26999e70208af9b445046cd9d20c82b7629ca1e51bdd00da"
              "ddbc35f9eb036a15ac57898642d9db09479a38cc80a2e41e380c8a766b2d6"
              "23de2de798e1eabc02234b89b85d60154460c3bf12764f3fbf17fcccc82df"
              "516a2fbe4ecf", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_13)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(bf90f930, 5616020a, 0e34ef30, 803fc15f, a97dffc0, 948452bb, f6cb5f66)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(a2601fbb, 9fe89f39, 814735fe, bb349143, baa93417, 0ffb91c6, 448a7823)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(f0acdfbc, 75a748a4, a0ac5528, 1754b5c4, a364b7d6, 1c5390b3, 34daae10)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(86587a67, 68f235bf, 523fbfc6, e062c740, 1ac2b024, 2cfe4e5f, b34f4057)
    };

    sbswecdsa("6bbb4bf987c8e5069e47c1a541b48b8a3e6d14bfd9ac6dfaa7503b64ab5e1"
              "a55f63e91cf5c3e703ac27ad88756dd7fb2d73b909fc15302d0592b974d47"
              "e72e60ed339a40b34d39a49b69ea4a5d26ce86f3ca00a70f1cd416a6a5722"
              "e8f39d1f0e966981803d6f46dac34e4c7640204cd0d9f1e53fc3acf30096c"
              "d00fa80b3ae9", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_14)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(116cfcf0, 965b7bc6, 3aecade7, 1d189d7e, 98a0434b, 124f2afb, e3ccf0a9)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(422e2e9f, e535eb62, f11f5f8c, e87cf2e9, ec65e61c, 06737cf6, a0019ae6)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(5fad3c04, 7074b5de, 1960247d, 0cc216b4, e3fb7f3b, 9cd96057, 5c8479fc)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(e4fc9c7f, 05ff0b04, 0eb171fd, d2a1dfe2, 572c564c, 2003a08c, 3179a422)
    };

    sbswecdsa("05b8f8e56214d4217323f2066f974f638f0b83689fc4ed1201848230efdc1"
              "fbca8f70359cecc921050141d3b02c2f17aa306fc2ce5fc06e7d0f4be162f"
              "cd985a0b687b4ba09b681cb52ffe890bf5bb4a104cb2e770c04df43301360"
              "5eb8c72a09902f4246d6c22b8c191ef1b0bece10d5ce2744fc7345307dd1b"
              "41b6eff0ca89", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P224_15)
{
    static const uint32_t s[224 / 32] =
    {
        IX224(6857e844, 18c1d117, 9333b4e5, 307e92ab, ade0b74f, 7521ad78, 044bf597)
    };
    static const uint32_t r[224 / 32] =
    {
        IX224(127051d8, 53260491, 15f307af, 2bc426f6, c2d08f47, 74a0b496, fb6982b1)
    };
    static const uint32_t qx[224 / 32] =
    {
        IX224(6dd84f4d, 66f36284, 4e41a791, 3c40b4aa, d5fa9ba5, 6bb44c2d, 2ed9efac)
    };
    static const uint32_t qy[224 / 32] =
    {
        IX224(15f65ebc, df2fd9f8, 035385a3, 30bdabec, 0f1cd9cc, 7bc31d2f, adbe7cda)
    };

    sbswecdsa("e5c979f0832242b143077bce6ef146a53bb4c53abfc033473c59f3c4095a6"
              "8b7a504b609f2ab163b5f88f374f0f3bff8762278b1f1c37323b9ed448e3d"
              "e33e6443796a9ecaa466aa75175375418186c352018a57ce874e44ae72401"
              "d5c0f401b5a51804724c10653fded9066e8994d36a137fdeb9364601daeef"
              "09fd174dde4a", SUPS_LSBF, n224, s, r, px224, py224,
              qx, qy, true);
}
END_TEST

bool pointcheck(const uint32_t *qx,
                const uint32_t *qy,
                bool full_check,
                const uint32_t *order)
{
    struct SB_SW_ECDSA_Point224 Q;

    PARAMETER_NOT_USED(order);
    PARAMETER_NOT_USED(full_check);

    c_memcpy(&Q.x, qx, sizeof(Q.x));
    c_memcpy(&Q.y, qy, sizeof(Q.y));

    return SB_SW_ECDSA_Point_Check224((const uint8_t*)&Q);
}

START_TEST(SB_SW_ECDSA_PointCheck)
{
    static const uint32_t qx1[224 / 32] =
    {
        IX224(9504b5b8, 2d97a264, d8b3735e, 0568deca, bc4b6ca2, 75bc53cb, adfc1c40)
    };
    static const uint32_t qy1[224 / 32] =
    {
        IX224(03426f80, e477603b, 10dee670, 939623e3, da91a942, 67fc4e51, 726009ed)
    };

    static const uint32_t qx2[224 / 32] =
    {
        IX224(851e3100, 368a2247, 8a002935, 3045ae40, d1d8202e, f4d6533c, fdddafd8)
    };
    static const uint32_t qy2[224 / 32] =
    {
        IX224(205302ac, 69457dd3, 45e86465, afa72ee8, c74ca97e, 2b0b999a, ec1f10c2)
    };

    static const uint32_t qx3[224 / 32] =
    {
        IX224(ad5bda09, d319a717, c1721acd, 6688d170, 20b31b47, eef1edea, 57ceeffc)
    };
    static const uint32_t qy3[224 / 32] =
    {
        IX224(c8ce98e1, 81770a7c, 9418c73c, 63d01494, b8b80a41, 098c5ea5, 0692c984)
    };

    /* Single bit changed. */
    static const uint32_t qx3b[224 / 32] =
    {
        IX224(ad5bda09, d319a717, c1721acd, 6688d170, 20b31b47, eef1edea, 57ceeffd)
    };
    static const uint32_t qy3b[224 / 32] =
    {
        IX224(c8ce98e1, 81770a7c, 9418c73c, 63d01494, b8b80a41, 098c5ea5, 0692c984)
    };

    fail_if(pointcheck(qx1, qy1, false, n224) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, false, n224) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, false, n224) == false,
            "Qx/Qy from vector 3 (functional vector)");

    fail_if(pointcheck(qx1, qy1, true, n224) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, true, n224) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, true, n224) == false,
            "Qx/Qy from vector 3 (functional vector)");

    /* Same as qx3/qy3, but with single bit changed from qx. */
    fail_if(pointcheck(qx3b, qy3b, false, n224) == true,
            "Qx/Qy accepted from altered public key");
    fail_if(pointcheck(qx3b, qy3b, true, n224) == true,
            "Qx/Qy accepted from altered public key");

    /* Odd combination: correct Qx and Qy, but from mixed vectors: */
    fail_if(pointcheck(qx2, qy3, false, n224) == true,
            "Qx/Qy accepted from mixed public key");
    fail_if(pointcheck(qx2, qy3, true, n224) == true,
            "Qx/Qy accepted from mixed public key");

}
END_TEST

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_ECDSA_Tests");
    sfzutf_tcase_create("SB_SW_ECDSA_Verify_NIST_P224");

    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_1);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_2);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_3);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_4);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_5);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_6);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_7);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_8);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_9);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_10);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_11);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_12);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_13);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_14);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P224_15);
    sfzutf_test_add(SB_SW_ECDSA_PointCheck);
}

/* end of file sbswecdsa224.c */
