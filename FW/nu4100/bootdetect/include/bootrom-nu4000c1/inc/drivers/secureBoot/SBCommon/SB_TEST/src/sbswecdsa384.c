/* sbswecdsa384.c
 *
 * Description: Tests for SB using ECDSA 384
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

#define IX384(w11,w10,w9,w8,w7,w6,w5,w4,w3,w2,w1,w0) \
    (uint32_t)0x##w0##u,  \
    (uint32_t)0x##w1##u,  \
    (uint32_t)0x##w2##u,  \
    (uint32_t)0x##w3##u,  \
    (uint32_t)0x##w4##u,  \
    (uint32_t)0x##w5##u,  \
    (uint32_t)0x##w6##u,  \
    (uint32_t)0x##w7##u,  \
    (uint32_t)0x##w8##u,  \
    (uint32_t)0x##w9##u,  \
    (uint32_t)0x##w10##u, \
    (uint32_t)0x##w11##u

/* Co-prime */
static uint32_t n384[384 / 32] =
{
    IX384(ffffffff, ffffffff, ffffffff, ffffffff, ffffffff, ffffffff,
          c7634d81, f4372ddf, 581a0db2, 48b0a77a, ecec196a, ccc52973)
};

/* Coordinates of generator point. */
static uint32_t px384[384 / 32] =
{
    IX384(aa87ca22, be8b0537, 8eb1c71e, f320ad74, 6e1d3b62, 8ba79b98,
          59f741e0, 82542a38, 5502f25d, bf55296c, 3a545e38, 72760ab7)
};

static uint32_t py384[384 / 32] =
{
    IX384(3617de4a, 96262c6f, 5d9e98bf, 9292dc29, f8f41dbd, 289a147c,
          e9da3113, b5f0b8c0, 0a60b1ce, 1d7e819d, 7a431d7c, 90ea0e5f)
};


/* These vectors are from FIPS ecdsatestvectors.zip. */

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_1)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(fa15e3d3, 0c6463b6, 46e8d3bf, 24558303, 14611cbd, e404be51,
              8b14464f, db195fdc, c92eb222, e61f426a, 4a592c00, a6a89721)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(50835a92, 51bad008, 106177ef, 004b091a, 1e4235cd, 0da84fff,
              54542b0e, d755c1d6, f251609d, 14ecf18f, 9e1ddfe6, 9b946e32)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(c2b47944, fb5de342, d0328588, 0177ca5f, 7d0f2fca, d7678cce,
              4229d6e1, 932fcac1, 1bfc3c3e, 97d942a3, c56bf341, 23013dbf)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(37257906, a8223866, eda0743c, 519616a7, 6a758ae5, 8aee81c5,
              fd35fbf3, a855b775, 4a36d4a0, 672df95d, 6c44a81c, f7620c2d)
    };

    sbswecdsa("6b45d88037392e1371d9fd1cd174e9c1838d11c3d6133dc17e65fa0c485dc"
              "ca9f52d41b60161246039e42ec784d49400bffdb51459f5de654091301a09"
              "378f93464d52118b48d44b30d781eb1dbed09da11fb4c818dbd442d161aba"
              "4b9edc79f05e4b7e401651395b53bd8b5bd3f2aaa6a00877fa9b45cadb8e6"
              "48550b4c6cbe", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_2)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(588e3d7a, f5da03ea, e255ecb1, 813100d9, 5edc2434, 76b724b2,
              2db8e853, 77660d76, 45ddc1c2, c2ee4eae, a8b683db, e22f86ca)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(fa15ef4c, b1276282, bb43f733, a7fb7c56, 7ce94f4d, 02924fc7,
              58635ab2, d1107108, bf159b85, db080cdc, 3b30fbb5, 400016f3)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(5d42d630, 1c54a438, f65970ba, e2a098cb, c567e988, 40006e35,
              6221966c, 86d82e8e, ca515bca, 850eaa3c, d41f175f, 03a0cbfd)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(4aef5a0c, eece95d3, 82bd70ab, 5ce1cb77, 408bae42, b51a0881,
              6d5e5e1d, 3da8c18f, cc95564a, 752730b0, aabea983, ccea4e2e)
    };

    sbswecdsa("d768f41e6e8ec2125d6cf5786d1ba96668ac6566c5cdbbe407f7f2051f3ad"
              "6b1acdbfe13edf0d0a86fa110f405406b69085219b5a234ebdb93153241f7"
              "85d45811b3540d1c37424cc7194424787a51b79679266484c787fb1ded6d1"
              "a26b9567d5ea68f04be416caf3be9bd2cafa208fe2a9e234d3ae557c65d3f"
              "e6da4cb48da4", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_3)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(4eb51db8, 004e46d4, 38359abf, 060a9444, 616cb46b, 4f99c9a0,
              5b53ba6d, f02e914c, 9c0b6cc3, a9791d80, 4d2e4c09, 84dab1cc)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(b11db592, e4ebc75b, 6472b879, b1d8ce57, 452c615a, ef20f67a,
              280f8bca, 9b11a30a, d4ac9d69, 541258c7, dd5d0b4a, b8dd7d49)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(fa15e2a3, a95e12d8, 7c72b5ea, 0a8a7cb8, 9f56b3bd, 46342b23,
              03608d72, 16301c21, b5d2921d, 80b6628d, c512ccb8, 4e2fc278)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(e4c1002f, 1828abae, c768cadc, b7cf42fb, f93b1709, ccae6df5,
              b134c41f, ae2b9a18, 8bfbe1ec, cff0bd34, 8517d722, 7f2071a6)
    };

    sbswecdsa("6af6652e92a17b7898e40b6776fabaf0d74cf88d8f0ebfa6088309cbe09fa"
              "c472eeac2aa8ea96b8c12e993d14c93f8ef4e8b547afe7ae5e4f3973170b3"
              "5deb3239898918c70c1056332c3f894cd643d2d9b93c2561aac069577bbab"
              "45803250a31cd62226cab94d8cba7261dce9fe88c210c212b54329d76a273"
              "522c8ba91ddf", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_4)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(8056c5bb, 57f41f73, 082888b2, 34fcda32, 0a33250b, 5da012ba,
              1fdb4924, 355ae679, 012d81d2, c08fc0f8, 634c708a, 4833232f)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(c2fbdd6a, 56789024, 08217372, 5d797ef9, fd6accb6, ae664b72,
              60f9e83c, b8ab2490, 428c8b9c, 52e15361, 2295432f, ec4d59cd)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(f1fabafc, 01fec7e9, 6d982528, d9ef3a2a, 18b7fe8a, e0fa0673,
              977341c7, ae4ae8d8, d3d67420, 343d013a, 984f5f61, da29ae38)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(fa15ef90, 2c46343d, 01b2ebb6, 14bc789c, 313b5f91, f9302ad9,
              418e9c79, 7563e2fa, 3d44500f, 47b4e26a, d8fdec1a, 816d1dcf)
    };

    sbswecdsa("b96d74b2265dd895d94e25092fb9262dc4f2f7a328a3c0c3da134b2d0a4e2"
              "058ca994e3445c5ff4f812738e1b0c0f7a126486942a12e674a21f22d0886"
              "d68df2375f41685d694d487a718024933a7c4306f33f1a4267d469c530b0f"
              "ed4e7dea520a19dd68bf0203cc87cad652260ed43b7b23f6ed140d3085875"
              "190191a0381a", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_5)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(651b8c3d, 5c9d5b93, 6d300802, a06d82ad, 54f7b1ba, 4327b2f0,
              31c0c5b0, cb215ad4, 354edc7f, 932d934e, 877dfa1c, f51b13fe)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(c38ef30f, 55624e89, 35680c29, f8c24824, 877cf48f, fc0ef015,
              e62de106, 88933530, 30d1193b, f9d34237, d7ce6ba9, 2c98b0fe)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(19501669, 89164cbf, d97968c7, e8adb6fb, ca1873eb, ef811ea2,
              59eb48b7, d584627f, 0e6d6c64, defe23cb, c9523650, 5a252aa1)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(41ef424b, 5cb076d4, e32accd9, 250ea75f, cf4ffd81, 814040c0,
              50d58c0a, 29b06be1, 1edf67c9, 11b403e4, 18b72774, 17e52906)
    };

    sbswecdsa("fa15e480a037ff40c232c1d2d6e8cd4c080bbeecdaf3886fccc9f129bb6d2"
              "02c316eca76c8ad4e76079afe622f833a16f4907e817260c1fa68b10c7a15"
              "1a37eb8c036b057ed4652c353db4b4a34b37c9a2b300fb5f5fcfb8aa8adae"
              "13db359160f70a9241546140e550af0073468683377e6771b6508327408c2"
              "45d78911c2cc", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_6)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(11b783d8, 79a6de05, 4b316af7, d56e526c, 3dce96c8, 5289122e,
              3ad927cf, a77bfc50, b4a96c97, f85b1b82, 21be2df0, 83ff58fb)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(abab6530, 8f0b79c4, f3a9ff28, dd490acb, 0c320434, 094cef93,
              e75adfe1, 7e5820dc, 1f77544c, faaacdc8, cf9ac8b3, 8e174bef)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(2e01c5b5, 9e619e00, b79060a1, e8ef6954, 72e23bf9, a511fc3d,
              5ed77a33, 4a242557, 098e4097, 2713732c, 5291c97a, df9cf2cf)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(563e3fe4, ad807e80, 3b9e961b, 08da4dde, 4cea8925, 649da0d9,
              3221ce4c, dceabc6a, 1db76121, 80a8c6be, f3579c65, 539b97e9)
    };

    sbswecdsa("00ce978603229710345c9ad7c1c2dba3596b196528eea25bd822d43ca8f76"
              "a024e29217703dd0652c8a615284fc3edcc1c5ad1c8d5a8521c8e104c016a"
              "24e50c2e25066dcb56596f913b872767e3627aa3e55ec812e9fdac7c2f1be"
              "ade83aef093e24c9c953982adf431a776880ae4583be158e11cdab1cbca3a"
              "d3a66900213d", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_7)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(7c26be29, 569ef954, 80a6d0c1, af49dc10, a51a0a89, 31345e48,
              c0c39498, bfb94d62, 962980b5, 6143a7b4, 1a2fddc8, 794c1b7f)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(f4f47785, 5819ad8b, 1763f536, 91b76afb, c4a31a63, 8b1e08c2,
              93f9bcd5, 5decf797, f9913ca1, 28d4b45b, 2e2ea3e8, 2c6cf565)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(51c78c97, 9452edd5, 3b563f63, eb3e854a, 5b23e87f, 1b210394,
              2b65f77d, 024471f7, 5c8ce1cc, 0dfef832, 92b36811, 2aa5126e)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(313e6aaf, 09caa3ba, 30f13072, b2134878, f14a4a01, ee86326c,
              ccbff3d0, 79b4df09, 7dc57985, e8c8c834, a10cb9d7, 66169366)
    };

    sbswecdsa("54a255c18692c6162a46add176a0ae8361dcb8948f092d8d7bac83e160431"
              "794d3b9812849bf1994bcdcfba56e8540c8a9ee5b93414548f2a653191b6b"
              "b28bda8dc70d45cc1b92a489f58a2d54f85766cb3c90de7dd88e690d8ebc9"
              "a79987eee1989df35af5e35522f83d85c48dda89863171c8b0bf4853ae28c"
              "2ac45c764416", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_8)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(a0d6b10c, effd0e1b, 29cf7844, 76f9173b, a6ecd2cf, c7929725,
              f2d6e24e, 0db5a472, 1683640e, aa2bbe15, 1fb57560, f9ce594b)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(dda994b9, c428b57e, 9f8bbaeb, ba0d682e, 3aac6ed8, 28e3a1e9,
              9a7fc4c8, 04bff8df, 151137f5, 39c7389d, 80e23d9f, 3ee497bf)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(ef948daa, e6824233, 0a7358ef, 73f23b56, c07e3712, 6266db3f,
              a6eea233, a04a9b3e, 4915233d, d6754427, cd4b71b7, 5854077d)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(009453ef, 1828eaff, 9e17c856, d4fc1895, ab600513, 12c3e1db,
              1e376656, 6438b299, 0cbf9945, c2545619, e3e0145b, c6a79004)
    };

    sbswecdsa("692a78f90d4f9d5aee5da536314a78d68c1feabbfe5d1ccea7f6059a66c4b"
              "310f8051c411c409ccf6e19a0cbd8b8e100c48317fe8c6d4f8a638b9551ce"
              "7ee178020f04f7da3001a0e6855225fb3c9b375e4ed964588a1a41a095f3f"
              "476c42d52ffd23ce1702c93b56d4425d3befcf75d0951b6fd5c05b05455bd"
              "af205fe70ca2", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_9)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(4a9a38af, da04c0a6, b0058943, b679bd02, 205b14d0, f3d49b8f,
              31aac289, 129780cd, b1c555de, f8c3f910, 6b478729, e0c7efaa)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(d1364689, 5afb1bfd, 1953551b, b922809c, 95ad65d6, abe94eb3,
              719c899a, a1f6dba6, b01222c7, f283900f, e98628b7, 597b6ea6)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(5709ec43, 05a9c327, 1c304fac, e6c14814, 2490b827, a73a4c17,
              affcfd01, fffd7eaa, 65d2fded, fa2419fc, 64ed9108, 23513faf)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(b083cda1, cf3be637, 1b6c06e7, 29ea6299, 213428db, 57119347,
              247ec1fc, d4420438, 6cc0bca3, f452d9d8, 64b39efb, fc89d6b2)
    };

    sbswecdsa("3b309bb912ab2a51681451ed18ad79e95d968abc35423a67036a02af92f57"
              "5a0c89f1b668afe22c7037ad1199e757a8f06b281c33e9a40bab69c9874e0"
              "bb680b905d909b9dc24a9fe89bb3d7f7d47082b25093c59754f8c19d1f81f"
              "30334a8cdd50a3cb72f96d4b3c305e60a439a7e93aeb640dd3c8de37d63c6"
              "0fb469c2d3ed", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_10)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(2cc10023, bf1bf8cc, fd14b06b, 82cc2114, 449a3523, 89c8ff9f,
              6f78cdc4, e32bde69, f3869da0, e17f691b, 329682ae, 7a36e1aa)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(5886078d, 3495767e, 330c7507, b7ca0fa0, 7a50e599, 12a416d8,
              9f0ab1aa, 4e88153d, 6eaf0088, 2d1b4aa6, 41531533, 52d853b5)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(06c037a0, cbf43fdf, 335dff33, de06d343, 48405353, f9fdf2ce,
              1361efba, 30fb204a, ea9dbd2e, 30da0a10, fd2d8761, 88371be6)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(360d38f3, 940e3467, 9204b98f, bf70b8a4, d97f2544, 3e46d080,
              7ab634ed, 5891ad86, 4dd77035, 57aa933c, d380e26e, ea662a43)
    };

    sbswecdsa("f072b72b8783289463da118613c43824d11441dba364c289de03ff5fab3a6"
              "f60e85957d8ff211f1cb62fa90216fb727106f692e5ae0844b11b710e5a12"
              "c69df3ed895b94e8769ecd15ff433762d6e8e94d8e6a72645b213b0231344"
              "e2c968056766c5dd6b5a5df41971858b85e99afbf859400f839b42cd12906"
              "8efabeea4a26", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_11)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(e5c64ed9, 8d7f3701, 193f25dd, 237d59c9, 1c0da6e2, 6215e088,
              9d82e6d3, e416693f, 8d58843c, f30ab10a, b8d0edd9, 170b53ad)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(66f92b39, aa3f4aeb, 9e2dc03a, c3855406, fa3ebbab, 0a6c88a7,
              8d7a0348, 2f0c9868, d7b78bc0, 81ede094, 7c7f37bf, 193074ba)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(49a1c631, f31cf5c4, 5b2676b1, f130cbf9, be683d0a, 50dffae0,
              d147c1e9, 913ab109, 0c6529a8, 4f47ddc7, cf025921, b771355a)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(1e207eec, e62f2bcc, 6bdabc11, 13158145, 170be974, 69a2904e,
              aaa93aad, 85b86a19, 719207f3, e423051f, 5b9cbbe2, 754eefcb)
    };

    sbswecdsa("cf4945350be8133b575c4ad6c9585e0b83ff1ed17989b6cd6c71b41b5264e"
              "828b4e115995b1ae77528e7e9002ac1b5669064442645929f9d7dd70927cb"
              "93f95edeb73e8624f4bc897ec4c2c7581cb626916f29b2d6e6c2fba8c59a7"
              "1e30754b459d81b912a12798182bcff4019c7bdfe929cc769bcc2414befe7"
              "d2906add4271", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_12)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(50ef923f, b217c4cf, 65a48b94, 412fda43, 0fac685f, 0da7bd57,
              4557c6c5, 0f5b22e0, c8354d99, f2c2f2c2, 691f252f, 93c7d84a)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(ee2923f9, b9999ea0, 5b5e57f5, 05bed5c6, ba0420de, f42c6fa9,
              0eef7a6e, f7707865, 25546de2, 7cdeb2f8, 586f8f29, fb4ee67c)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(70a0f16b, 6c611726, 59b027ed, 19b18fd8, f57bd28d, c0501f20,
              7bd6b0bb, 065b5671, cf3dd1ed, 13d388dc, f6ccc766, 597aa604)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(4f845bf0, 1c3c3f61, 26a7368c, 3454f514, 25801ee0, b72e63fb,
              6799b442, 0bfdebe3, e37c7246, db627cc8, 2c096549, 79c700bb)
    };

    sbswecdsa("d9b5cf0b50416573ff3c63133275a18394dd4326be2041e8d97e6e4e3855a"
              "4a177e9d26dfd223fe8aa74564edb49bd72de19916fb6f001f44530d5c18e"
              "2c332bce1b7415df5927ece5f3824f34d174b963136b53aef1fb78fb0c06a"
              "201a40b2db38e4d8216fc1e392a798c8ab4b3a314496b7f1087804ebfa89b"
              "f96e9cdb80c0", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_13)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(fcc504e0, f00ef295, 87e4bc22, faada4db, 30e2cb1a, c552680a,
              65785ae8, 7beb666c, 792513f2, be7a3180, fc544296, 841a0e27)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(db054add, b6161ee4, 9c6ce2e4, d646d767, 0754747b, 6737ca85,
              16e9d1e8, 7859937c, 3ef9b1d2, 663e10d7, e4bd00ec, 85b7a97a)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(cadbacef, 44060993, 16db2ce3, 206adc63, 6c2bb0a8, 35847ed7,
              941efb02, 862472f3, 150338f1, 3f4860d4, 7f39b7e0, 98f0a390)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(752ad0f2, 2c9c2643, 36cde11b, bc95d181, 6ed4d1b1, 500db6b8,
              dce259a4, 2832e613, c31178c2, c7995206, a62e201b, a108f570)
    };

    sbswecdsa("9e4042d8438a405475b7dab1cd783eb6ce1d1bffa46ac9dfda622b23ac310"
              "57b922eced8e2ed7b3241efeafd7c9ab372bf16230f7134647f2956fb7939"
              "89d3c885a5ae064e85ed971b64f5f561e7ddb79d49aa6ebe727c671c67879"
              "b794554c04de0e05d68264855745ef3c9567bd646d5c5f8728b797c181b6b"
              "6a876e167663", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_14)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(d40e09d3, 468b4669, 9948007e, 8f598457, 66dbf694, b9c62066,
              890dd055, c0cb9a0c, af0aa611, fb9f466a, d0bbb00d, be29d7eb)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(009c7406, 3e206a42, 59b53dec, ff544568, 3a03f44f, a67252b7,
              6bd35810, 81c714f8, 82f882df, 915e97db, eab061fa, 8b3cc4e7)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(33093a05, 68757e8b, 58df5b72, ea5fe5bf, 26e6f7ae, b541b4c6,
              a8c189c9, 3721749b, caceccf2, 982a2f07, 02586a9f, 812fc66f)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(ebe320d0, 9e1f0662, 189d50b8, 5a20403b, 821ac0d0, 00afdbf6,
              6a0a33f3, 04726c69, e354d81c, 50b94ba3, a5250efc, 31319cd1)
    };

    sbswecdsa("0b14a7484a40b68a3ce1273b8a48b8fdb65ba900d98541c4bbd07b97e31bc"
              "c4c85545a03e9deab3c563f47a036ff60d0361684ba241b5aa68bb46f440d"
              "a22181ee328a011de98eff34ba235ec10612b07bdfa6b3dc4ccc5e82d3a8d"
              "057e1862fef3def5a1804696f84699fda2ec4175a54a4d08bcb4f0406fdac"
              "4eddadf5e29b", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P384_15)
{
    static const uint32_t s[384 / 32] =
    {
        IX384(af10b90f, 203af23b, 7500e070, 536e6462, 9ba19245, d6ef39aa,
              b57fcdb1, b73c4c6b, f7070c62, 63544633, d3d358c1, 2a178138)
    };
    static const uint32_t r[384 / 32] =
    {
        IX384(ee82c0f9, 0501136e, b0dc0e45, 9ad17bf3, be1b1c8b, 8d05c600,
              68a9306a, 346326ff, 7344776a, 95f1f7e2, e2cf9477, 130e735c)
    };
    static const uint32_t qx[384 / 32] =
    {
        IX384(a39ac353, ca787982, c577aff1, e8601ce1, 92aa90fd, 0de4c0ed,
              627f66a8, b6f02ae5, 1315543f, 72ffc1c4, 8a7269b2, 5e7c289a)
    };
    static const uint32_t qy[384 / 32] =
    {
        IX384(9064a507, b66b340b, 6e0e0d5f, faa67dd2, 0e6dafc0, ea6a6fae,
              e1635177, af256f91, 08a22e9e, df736ab4, ae8e96dc, 207b1fa9)
    };

    sbswecdsa("0e646c6c3cc0f9fdedef934b7195fe3837836a9f6f263968af95ef84cd035"
              "750f3cdb649de745c874a6ef66b3dd83b66068b4335bc0a97184182e3965c"
              "722b3b1aee488c3620adb835a8140e199f4fc83a88b02881816b366a09316"
              "e25685217f9221157fc05b2d8d2bc855372183da7af3f0a14148a09def37a"
              "332f8eb40dc9", SUPS_LSBF, n384, s, r, px384, py384,
              qx, qy, true);
}
END_TEST

bool pointcheck(const uint32_t *qx,
                const uint32_t *qy,
                bool full_check,
                const uint32_t *order)
{
    struct SB_SW_ECDSA_Point384 Q;

    PARAMETER_NOT_USED(order);
    PARAMETER_NOT_USED(full_check);

    c_memcpy(&Q.x, qx, sizeof(Q.x));
    c_memcpy(&Q.y, qy, sizeof(Q.y));

    return SB_SW_ECDSA_Point_Check384((const uint8_t*)&Q);
}

START_TEST(SB_SW_ECDSA_PointCheck)
{
    static const uint32_t qx1[384 / 32] =
    {
        IX384(cadbacef, 44060993, 16db2ce3, 206adc63, 6c2bb0a8, 35847ed7,
              941efb02, 862472f3, 150338f1, 3f4860d4, 7f39b7e0, 98f0a390)
    };
    static const uint32_t qy1[384 / 32] =
    {
        IX384(752ad0f2, 2c9c2643, 36cde11b, bc95d181, 6ed4d1b1, 500db6b8,
              dce259a4, 2832e613, c31178c2, c7995206, a62e201b, a108f570)
    };

    static const uint32_t qx2[384 / 32] =
    {
        IX384(33093a05, 68757e8b, 58df5b72, ea5fe5bf, 26e6f7ae, b541b4c6,
              a8c189c9, 3721749b, caceccf2, 982a2f07, 02586a9f, 812fc66f)
    };
    static const uint32_t qy2[384 / 32] =
    {
        IX384(ebe320d0, 9e1f0662, 189d50b8, 5a20403b, 821ac0d0, 00afdbf6,
              6a0a33f3, 04726c69, e354d81c, 50b94ba3, a5250efc, 31319cd1)
    };

    static const uint32_t qx3[384 / 32] =
    {
        IX384(49a1c631, f31cf5c4, 5b2676b1, f130cbf9, be683d0a, 50dffae0,
              d147c1e9, 913ab109, 0c6529a8, 4f47ddc7, cf025921, b771355a)
    };
    static const uint32_t qy3[384 / 32] =
    {
        IX384(1e207eec, e62f2bcc, 6bdabc11, 13158145, 170be974, 69a2904e,
              aaa93aad, 85b86a19, 719207f3, e423051f, 5b9cbbe2, 754eefcb)
    };

    /* Single bit changed. */
    static const uint32_t qx3b[384 / 32] =
    {
        IX384(49a1c631, f31cf5c4, 5b2676b1, f130cbf9, be683d0a, 50dffae0,
              d147c1e9, 913ab109, 0c6529a8, 4f47ddc7, cf025921, b771355b)
    };
    static const uint32_t qy3b[384 / 32] =
    {
        IX384(1e207eec, e62f2bcc, 6bdabc11, 13158145, 170be974, 69a2904e,
              aaa93aad, 85b86a19, 719207f3, e423051f, 5b9cbbe2, 754eefcb)
    };

    fail_if(pointcheck(qx1, qy1, false, n384) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, false, n384) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, false, n384) == false,
            "Qx/Qy from vector 3 (functional vector)");

    fail_if(pointcheck(qx1, qy1, true, n384) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, true, n384) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, true, n384) == false,
            "Qx/Qy from vector 3 (functional vector)");

    /* Same as qx3/qy3, but with single bit changed from qx. */
    fail_if(pointcheck(qx3b, qy3b, false, n384) == true,
            "Qx/Qy accepted from altered public key");
    fail_if(pointcheck(qx3b, qy3b, true, n384) == true,
            "Qx/Qy accepted from altered public key");

    /* Odd combination: correct Qx and Qy, but from mixed vectors: */
    fail_if(pointcheck(qx2, qy3, false, n384) == true,
            "Qx/Qy accepted from mixed public key");
    fail_if(pointcheck(qx2, qy3, true, n384) == true,
            "Qx/Qy accepted from mixed public key");

}
END_TEST

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_ECDSA_Tests");
    sfzutf_tcase_create("SB_SW_ECDSA_Verify_NIST_P384");

    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_1);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_2);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_3);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_4);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_5);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_6);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_7);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_8);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_9);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_10);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_11);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_12);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_13);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_14);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P384_15);
    sfzutf_test_add(SB_SW_ECDSA_PointCheck);
}

/* end of file sbswecdsa384.c */
