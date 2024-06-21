/* sbswecdsa521.c
 *
 * Description: Tests for SB using ECDSA 521
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

#define IX521(w16,w15,w14,w13,w12,w11,w10,w9,w8,w7,w6,w5,w4,w3,w2,w1,w0) \
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
    (uint32_t)0x##w11##u, \
    (uint32_t)0x##w12##u, \
    (uint32_t)0x##w13##u, \
    (uint32_t)0x##w14##u, \
    (uint32_t)0x##w15##u, \
    (uint32_t)0x##w16##u

/* Co-prime */
static uint32_t n521[17] =
{
    IX521(01ffffff, ffffffff, ffffffff, ffffffff, ffffffff, ffffffff,
          ffffffff, ffffffff, fffa5186, 8783bf2f, 966b7fcc, 0148f709,
          a5d03bb5, c9b8899c, 47aebb6f, b71e9138, 64090000)
};

/* Coordinates of generator point. */
static uint32_t px521[17] =
{
    IX521(00c6858e, 06b70404, e9cd9e3e, cb662395, b4429c64, 8139053f,
          b521f828, af606b4d, 3dbaa14b, 5e77efe7, 5928fe1d, c127a2ff,
          a8de3348, b3c1856a, 429bf97e, 7e31c2e5, bd660000)
};

static uint32_t py521[17] =
{
    IX521(01183929, 6a789a3b, c0045c8a, 5fb42c7d, 1bd998f5, 4449579b,
          446817af, bd17273e, 662c97ee, 72995ef4, 2640c550, b9013fad,
          0761353c, 7086a272, c24088be, 94769fd1, 66500000)
};


/* These vectors are from FIPS ecdsatestvectors.zip. */

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_1)
{
    static const uint32_t s[17] =
    {
        IX521(00000087, 488c859a, 96fea266, ea13bf6d, 114c429b, 163be97a,
              57559086, edb64aed, 4a18594b, 46fb9efc, 7fd25d8b, 2de8f09c,
              a0587f54, bd287299, f47b2ff1, 24aac566, e8efa15e)
    };
    static const uint32_t r[17] =
    {
        IX521(0000004d, e826ea70, 4ad10bc0, f7538af8, a3843f28, 4f55c8b9,
              46af9235, af5af74f, 2b76e099, e4bc72fd, 79d28a38, 0f8d4b4c,
              919ac290, d248c379, 83ba05ae, a42e2dd7, 9fdd33e8)
    };
    static const uint32_t qx[17] =
    {
        IX521(00000061, 387fd6b9, 5914e885, f912edfb, b5fb2746, 55027f21,
              6c4091ca, 83e19336, 740fd81a, edfe047f, 51b42bdf, 68161121,
              013e0d55, b117a14e, 4303f926, c8debb77, a7fdaad1)
    };
    static const uint32_t qy[17] =
    {
        IX521(000000e7, d0c75c38, 626e895c, a21526b9, f9fdf84d, cecb93f2,
              b2333905, 50d2b146, 3b7ee3f5, 8df73464, 35ff0434, 199583c9,
              7c665a97, f12f706f, 2357da4b, 40288def, 888e59e6)
    };

    sbswecdsa("9ecd500c60e701404922e58ab20cc002651fdee7cbc9336adda33e4c1088f"
              "ab1964ecb7904dc6856865d6c8e15041ccf2d5ac302e99d346ff2f686531d"
              "25521678d4fd3f76bbf2c893d246cb4d7693792fe18172108146853103a51"
              "f824acc621cb7311d2463c3361ea707254f2b052bc22cb8012873dcbb95bf"
              "1a5cc53ab89f", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_2)
{
    static const uint32_t s[17] =
    {
        IX521(0000016a, 997f81aa, 0bea2e14, 69c8c1da, b7df02a8, b2086ba4,
              82c43af0, 4f217483, 1f2b1761, 658795ad, fbdd4419, 0a9b06fe,
              10e57898, 7369f3a2, eced147c, ff89d8c2, 818f7471)
    };
    static const uint32_t r[17] =
    {
        IX521(000001a3, c4a6386c, 4fb614fb, a2cb9e74, 201e1aaa, 0001aa93,
              1a2a939c, 92e04b83, 44535a20, f53c6e3c, 69c75c2e, 5d2fe354,
              9ed27e67, 13cb0f4a, 9a94f618, 9eb33bff, 7d4fa15e)
    };
    static const uint32_t qx[17] =
    {
        IX521(0000004d, 5c8afee0, 38984d2e, a96681ec, 0dccb6b5, 2dfa4ee2,
              e2a77a23, c8cf43ef, 19905a34, d6f5d8c5, cf0981ed, 804d89d1,
              75b17d1a, 63522ceb, 1e785c0f, 5a1d2f3d, 15e51352)
    };
    static const uint32_t qy[17] =
    {
        IX521(00000014, 368b8e74, 6807b2b6, 8f3615cd, 78d761a4, 64ddd791,
              8fc8df51, d225962f, df1e3dc2, 43e26510, 0ff0ec13, 3359e332,
              e44dd49a, fd8e5f38, fe861335, 73432d33, c02fa0a3)
    };

    sbswecdsa("b3c63e5f5a21c4bfe3dbc644354d9a949186d6a9e1dd873828782aa6a0f1d"
              "f2f64114a430b1c13fe8a2e09099e1ed05ef70de698161039ded73bcb50b3"
              "12673bb073f8a792ac140a78a8b7f3586dffb1fc8be4f54516d57418ccc99"
              "45025ce3acf1eb84f69ceee5e9bd10c18c251dbc481562cd3aae54b54ab61"
              "8cb1eeda33cf", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_3)
{
    static const uint32_t s[17] =
    {
        IX521(0000008c, 3457fe1f, 93d635bb, 52df9218, bf3b49a7, a345b8a8,
              a988ac0a, 25434054, 6752cddf, 02e6ce47, eee58ea3, 98fdc913,
              0e55a4c0, 9f5ae548, c715f5bc, d539f07a, 34034d78)
    };
    static const uint32_t r[17] =
    {
        IX521(0000010e, d3ab6d07, a15dc337, 6494501c, 27ce5f78, c8a2b30c,
              c809d3f9, c3bf1aef, 437e590e, f66abae4, e49065ea, d1af5f75,
              2ec145ac, fa98329f, 17bca999, 1a199579, c41f9229)
    };
    static const uint32_t qx[17] =
    {
        IX521(000000c2, d540a755, 7f4530de, 35bbd94d, a8a6defb, ff783f54,
              a65292f8, f76341c9, 96cea387, 95805a1b, 97174a91, 47a86442,
              82e0d704, 0a6f8342, 3ef2a045, 32481563, 93afa15e)
    };
    static const uint32_t qy[17] =
    {
        IX521(00000119, f746c5df, 8cec24e4, 849ac187, 0d0d8594, c799d2ce,
              b6c3bdf8, 91dfbd22, 42e7ea24, d6aec316, 6214734a, cc4cbf4d,
              a8f71e24, 29c5c187, b2b3a048, 527c861f, 58a9b97f)
    };

    sbswecdsa("6e0f96d56505ffd2d005d5677dbf926345f0ff0a5da456bbcbcfdc2d33c8d"
              "878b0bc8511401c73168d161c23a88b04d7a9629a7a6fbcff241071b0d212"
              "248fcc2c94fa5c086909adb8f4b9772b4293b4acf5215ea2fc72f8cec57b5"
              "a13792d7859b6d40348fc3ba3f5e7062a19075a9edb713ddcd391aefc90f4"
              "6bbd81e2557b", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_4)
{
    static const uint32_t s[17] =
    {
        IX521(0000012f, ed45cc87, 4dc3ed3a, 11dd70f7, d5c61451, fbea497d,
              d63e226e, 10364e07, 18d3722c, 27c7b4e5, 027051d5, 4b8f2a57,
              fc58bc07, 0a55b1a5, 877b0f38, 8d768837, ef2e9cec)
    };
    static const uint32_t r[17] =
    {
        IX521(000001e2, bf98d118, 6d7bd350, 9f517c22, 0de51c92, 00981e9b,
              344b9fb0, d36f34d9, 69026c80, 311e7e73, bb13789a, 99e0d59e,
              82ebe0e9, 595d9747, 204c5f55, 50c30d93, 4aa30c05)
    };
    static const uint32_t qx[17] =
    {
        IX521(00000160, d7ea2e12, 8ab3fabd, 1a3ad545, 5cb45e2f, 977c2354,
              a1345d4a, e0c7ce4e, 492fb9ff, 958eddc2, aa61735e, 5c1971fa,
              6c99beda, 0f424a20, c3ce9693, 80aaa52e, f5f5daa8)
    };
    static const uint32_t qy[17] =
    {
        IX521(0000014e, 4c83f90d, 196945fb, 4fe1e419, 13488aa5, 3e24c1d2,
              142d35a1, eed69fed, 784c0ef4, 4d71bc21, afe0a006, 5b3b8706,
              9217a5ab, ab4355cf, 8f4ceae5, 657cd4b9, c80fa15e)
    };

    sbswecdsa("3f12ab17af3c3680aad22196337cedb0a9dba22387a7c555b46e84176a6f8"
              "418004552386ada4deec59fdabb0d25e1c6668a96f100b352f8dabd24b226"
              "2bd2a3d0f825602d54150bdc4bcbd5b8e0ca52bc8d2c70ff2af9b03e20730"
              "d6bd9ec1d091a3e5c877259bcff4fd2c17a12bfc4b08117ec39fe4762be12"
              "8d0883a37e9d", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_5)
{
    static const uint32_t s[17] =
    {
        IX521(00000120, be63bd97, 691f6258, b5e78817, f2dd6bf5, a7bf79d0,
              1b8b1c33, 82860c4b, 00f89894, c72f93a6, 9f3119cb, 74c90b03,
              e9ede27b, d298b357, b9616a72, 82d176f3, 899aaa24)
    };
    static const uint32_t r[17] =
    {
        IX521(0000014d, fa430463, 02b81fd9, a34a454d, ea25ccb5, 94ace8df,
              4f9d9855, 6ca5076b, cd44b2a9, 775dfaca, 50282b2c, 8988868e,
              5a31d9eb, 08e79401, 69969420, 88d43ad3, 379eb9a1)
    };
    static const uint32_t qx[17] =
    {
        IX521(000001ce, ee0be329, 3d8c0fc3, e38a78df, 55e85e6b, 4bbce0b9,
              995251f0, ac552341, 40f82ae0, a434b2bb, 41dc0aa5, ecf950d4,
              628f82c7, f4f67651, b804d55d, 844a02c1, da6606f7)
    };
    static const uint32_t qy[17] =
    {
        IX521(000001f7, 75eb6b3c, 5e43fc75, 4052d1f7, fc5b9913, 7afc15d2,
              31a0199a, 702fc065, c917e628, a54e038c, bfebe05c, 90988b65,
              183b368a, 2061e5b5, c1b025bb, f2b748fa, e00ba297)
    };

    sbswecdsa("a1eed24b3b7c33296c2491d6ee092ec6124f85cf566bb5bc35bffb5c734e3"
              "4547242e57593e962fb76aee9e800eed2d702cc301499060b76406b347f3d"
              "1c86456978950737703c8159001e6778f69c734a56e5ce5938bd0e0de0877"
              "d55adeee48b0d8dfa4ac65fd2d3ce3e12878bac5c7014f9284d161b2a3e7d"
              "5c88569fa15e", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, false);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_6)
{
    static const uint32_t s[17] =
    {
        IX521(00000038, 6a5a0fc5, 5d36ca72, 31a9537f, ee6b9e51, c2255363,
              d9c9e7cb, 7185669b, 302660e2, 3133eb21, eb56d305, d36e69a7,
              9f5b6fa2, 5b46ec61, b7f699e1, e9e927fb, 0bceca06)
    };
    static const uint32_t r[17] =
    {
        IX521(0000011a, 1323f613, 2d85482d, 9b0f73be, 838d8f9e, 78647934,
              f2570fed, edca7c23, 4cc46aa1, b97da5ac, 1b27b714, f7a171dc,
              4209cbb0, d90e4f79, 3c4c192d, c039c313, 10d6d99b)
    };
    static const uint32_t qx[17] =
    {
        IX521(0000014c, ab9759d4, 487987b8, a00afd16, d7199585, b730fb0b,
              fe637962, 72dde913, 5e7cb9e2, 7cec5120, 7c876d92, 14214b8c,
              76f82e73, 63f50869, 02a577e1, c50b4fbf, 35ce9966)
    };
    static const uint32_t qy[17] =
    {
        IX521(000001a8, 3f0caa01, ca2166e1, 20629234, 2f47f358, 009e8b89,
              1d3cb817, aec290e0, cf2f47e7, fc637e39, dca03949, 39183968,
              4f76b94d, 34e5abc7, bb750cb4, 4486cce5, 25eb0093)
    };

    sbswecdsa("9aace26837695e6596007a54e4bccdd5ffb16dc6844140e2eeeb584b15acb"
              "2bbffd203c74440b6ee8db676fd200b4186a8c3e957c19e74d4d865ada83f"
              "80655323dfa3570907ed3ce853b6e8cc375ed2d758a2f5ad265dd3b476505"
              "17a49b3d02df9e0c60c21576378c2b3a08481eec129b2a75608e13e642012"
              "7a3a63c8a3f1", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_7)
{
    static const uint32_t s[17] =
    {
        IX521(000000c6, f1c7774c, af198fc1, 89beb7e2, 1ca92cec, cc3f9875,
              f0e2d07d, c1d15bcc, 8f210b6d, d376bf65, bb6a454b, f563d7f5,
              63c1041d, 62d60788, 28a57538, b25ba547, 23170665)
    };
    static const uint32_t r[17] =
    {
        IX521(0000000d, bf787ce0, 7c453c6c, 6a67b0bf, 6850c8d6, ca693a3e,
              9818d745, 3487844c, 9048a7a2, e48ff982, b64eb971, 2461b26b,
              5127c4dc, 57f9a6ad, 1e15d8cd, 56d4fd6d, a7186429)
    };
    static const uint32_t qx[17] =
    {
        IX521(0000009d, a1536154, b46e3169, 265ccba2, b4da9b4b, 06a7462a,
              067c6909, f6c0dd8e, 19a7bc2a, c1a47763, ec4be06c, 1bec57d2,
              8c55ee93, 6cb19588, cc1398fe, 4ea3bd07, e6676b7f)
    };
    static const uint32_t qy[17] =
    {
        IX521(00000141, 50cdf25d, a0925926, 422e1fd4, dcfcffb0, 5bdf8682,
              c54d67a9, bd438d21, de5af43a, 15d979b3, 20a84768, 3b6d12ac,
              1383a718, 3095e9da, 491c3b4a, 7c288746, 25e70f87)
    };

    sbswecdsa("ac2175940545d4fbab6e2e651c6830aba562e0c11c919e797c43eff9f187a"
              "68a9e5a128e3e2a330b955a3f4577d3f826529ad1b03d7b60f7ad678f0050"
              "53b41dc0f8d267f3685c6abe1a0e9a733c44b2f3ca48b90806f935141c842"
              "e3a6c06a58f5343d75e3585971a734f4ae1074ce5b54f74bd9342f4bbca73"
              "8d260393f43e", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_8)
{
    static const uint32_t s[17] =
    {
        IX521(0000005d, 1bcf2295, 240ce441, 5042306a, bd494b4b, da7cf36f,
              2ee29315, 18d2454f, aa01c606, be120b05, 7062f2f3, a174cb09,
              c14f57ab, 6ef41cb3, 802140da, 22074d0e, 46f908d4)
    };
    static const uint32_t r[17] =
    {
        IX521(0000014b, f63bdbc0, 14aa3525, 44bd1e83, ede48480, 7ed76061,
              9fa6bc38, c4f86408, 40195e1f, 2f149b29, 903ca4b6, 934404fb,
              1f7de5e3, 9b1ea04d, ba42819c, 75dbef6a, 93ebe269)
    };
    static const uint32_t qx[17] =
    {
        IX521(0000000b, bd4e8a01, 6b0c254e, 754f68f0, f4ed0813, 20d529ec,
              dc7899cf, b5a67dd0, 4bc85b3a, a6891a3e, d2c9861a, e76c3847,
              d81780c2, 3ad84153, ea2042d7, fd5d517a, 26ff3ce4)
    };
    static const uint32_t qy[17] =
    {
        IX521(00000064, 5953afc3, c1b3b74f, df503e7d, 3f982d7e, e17611d6,
              0f8eb42a, 4bddbec2, b67db1f0, 9b54440c, 30b44e80, 71d40465,
              8285cb57, 14620012, 18fc8c5e, 5b98b9fa, e28272e6)
    };

    sbswecdsa("6266f09710e2434cb3da3b15396556765db2ddcd221dce257eab7399c7c49"
              "0135925112932716af1434053b8b9fe340563e57a0b9776f9ac92cbb5fba1"
              "8b05c0a2fafbed7240b3f93cd1780c980ff5fe92610e36c0177cabe82367c"
              "84cee9020cf26c1d74ae3eb9b9b512cb8b3cb3d81b17cf20dc76591b2b394"
              "ef1c62ac12ee", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_9)
{
    static const uint32_t s[17] =
    {
        IX521(000001bc, c1d211eb, c120a97d, 465b603a, 1bb1e470, 109e0a55,
              d2f1b5c5, 97803931, bd6d7718, f010d7d2, 89b31533, e9fcef3d,
              141974e5, 955bc7f0, ee342b9c, ad05e29a, 3dded30e)
    };
    static const uint32_t r[17] =
    {
        IX521(0000007e, 315d8d95, 8b8ce27e, af4f3782, 294341d2, a46fb145,
              7a60eb9f, e93a9ae8, 6f376471, 6c4f5f12, 4bd6b114, 781ed59c,
              3f24e18a, a35c9032, 11b2f203, 9d858629, 32987d68)
    };
    static const uint32_t qx[17] =
    {
        IX521(00000015, 7d80bd42, 6f6c3cee, 903c24b7, 3faa02e7, 58607c3e,
              102d6e64, 3b7269c2, 99684fda, ba1acddb, 83ee686a, 60acca53,
              cddb2fe9, 76149205, c8b8ab6a, d1458bc0, 0993cc43)
    };
    static const uint32_t qy[17] =
    {
        IX521(0000016e, 33cbed05, 721b284d, acc8c8fb, e2d118c3, 47fc2e26,
              70e691d5, d53daf6e, f2dfec46, 4a5fbf46, f8efce81, ac226915,
              e11d43c1, 1c8229fc, a2327815, e1f8da5f, e95021fc)
    };

    sbswecdsa("3de9e617a6868dca1a1432d503f923535da3f9b34426b2a4822174399c73b"
              "1c1ee67311410a58c17202ac767844b2024d8aa21a205707d93865693ac25"
              "a24fc87034fa3a7a7e27c3344cb03b87602c15180a5fe6a9dd90cd11af4a0"
              "f150207bf2d83f55b12c088adae99aa8cfa659311b3a25beb99056643760d"
              "6a282126b9b2", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_10)
{
    static const uint32_t s[17] =
    {
        IX521(00000134, ac5e1ee3, 39727df8, 0c35ff5b, 2891596d, d14d6cfd,
              137bafd5, 0ab98e2c, 1ab4008a, 0bd03552, 618d2179, 12a9ec50,
              2a902f23, 53e757c3, b5776309, f7f2cfeb, f913e9cd)
    };
    static const uint32_t r[17] =
    {
        IX521(000000d7, 32ba8b3e, 9c9e0a49, 5249e152, e5bee69d, 94e9ff01,
              2d001b14, 0d4b5d08, 2aa9df77, e10b65f1, 15a594a5, 0114722d,
              b42fa5fb, e457c5bd, 05e7ac7e, e510aa68, fe7b1e7f)
    };
    static const uint32_t qx[17] =
    {
        IX521(00000070, 02872c20, 0e16d57e, 8e53f7bc, e6e9a783, 2c387f6f,
              9c29c6b7, 5526262c, 57bc2b56, d63e9558, c5761c1d, 62708357,
              f586d3aa, b41c6a7c, a3bf6c32, d9c3ca40, f9a2796a)
    };
    static const uint32_t qy[17] =
    {
        IX521(000001fe, 3e52472e, f224fb38, d5a0a148, 75b52c2f, 50b82b99,
              eea98d82, 6c77e6a9, ccf798de, 5ffa92a0, d65965f7, 40c702a3,
              027be66b, 9c844f1b, 2e96c134, eb3fdf3e, dddcf11c)
    };

    sbswecdsa("aa48851af7ef17abe233163b7185130f4646203c205e22bcc2a5a3697bcab"
              "998c73a9ffe1d3ea0b7978ce7df937a72586eb5ca60b0d939a7d1c115c820"
              "171c89c8116b7e2c7b98cf0f14e4c4df3cb2f319ad3ab0ea25ff14526ddc0"
              "37469f000bf82100acd4cdf94feb4eba4ea1726f0569336604a473aee67d7"
              "1afebb569209", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_11)
{
    static const uint32_t s[17] =
    {
        IX521(000000b7, 9ffcdc33, e028b1ab, 894cb751, ec792a69, e3011b20,
              1a76f3b8, 78655bc3, 1efd1c0b, f3b98aea, 2b14f262, c19d142e,
              008b98e8, 90ebbf46, 4d3b0257, 64dd2f73, c4251b1a)
    };
    static const uint32_t r[17] =
    {
        IX521(000001dc, e45ea592, b34d0164, 97882c48, dc0c7afb, 1c8e0f81,
              a051800d, 7ab8da9d, 237efd89, 2207bc94, 01f1d306, 50f66af8,
              d5349fc5, b1972775, 6270722d, 5a8adb0a, 49b72d0a)
    };
    static const uint32_t qx[17] =
    {
        IX521(000000c9, 7a4ebcbb, e701c9f7, be127e87, 079edf47, 9b76d3c1,
              4bfbee69, 3e1638e5, bff8d470, 5ac0c145, 97529dbe, 13356ca8,
              5eb03a41, 8edfe144, ce6cbf35, 33016d4e, fc29dbd4)
    };
    static const uint32_t qy[17] =
    {
        IX521(0000011c, 75b7a889, 4ef64109, ac2dea97, 2e7fd5f7, 9b75dab1,
              bf9441a5, b8b86f1d, c1324426, fa6cf4e7, b973b44e, 3d0576c5,
              2e5c9edf, 8ce2fc18, cb3c2874, 2d44419f, 044667f8)
    };

    sbswecdsa("b0d5d52259af364eb2d1a5027e5f7d0afe4b999cc5dd2268cfe76f51d2f17"
              "b541bdd7867e23a1bb897705153d9432a24012108979c6a2c9e2567c9531d"
              "012f9e4be764419491a52eae2e127430b0ab58cb8e216515a821b3db20644"
              "7c235bf44ee304201b483b2a88844abaa18bca0147dfff7e502397dd62e15"
              "524f67eb2df2", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_12)
{
    static const uint32_t s[17] =
    {
        IX521(000000d0, 5ee3e64b, ac4e56d9, d8bd511c, 8a43941e, 953cba4e,
              5d83c055, 3acb8709, 1ff54f3a, ad4d69d9, f15e520a, 2551cc14,
              f2c86bb4, 5513fef0, 295e381a, 7635486b, d3917b50)
    };
    static const uint32_t r[17] =
    {
        IX521(00000183, bddb46c2, 49e868ef, 231a1ebd, 85d0773b, f8105a09,
              2ab7d884, d677a1e9, b7d6014d, 6358c095, 38a99d9d, ca8f36f1,
              63ac1827, df420c3f, 9360cc66, 900a9737, a7f756f3)
    };
    static const uint32_t qx[17] =
    {
        IX521(000001f2, 69692c47, a55242bb, 08731ff9, 20f4915b, fcecf4d4,
              431a8b48, 7c90d085, 65272c52, ca90c473, 97f7604b, c643982e,
              34d05178, e979c2cf, f7ea1b9e, aec18d69, ca7382de)
    };
    static const uint32_t qy[17] =
    {
        IX521(00000075, 0bdd866f, ba3e92c2, 9599c002, ac6f9e2b, f39af852,
              1b7b133f, 70510e99, 18a94d3c, 279edec9, 7ab75ecd, a95e3dd7,
              861af84c, 543371c0, 55dc74ee, eff70617, 26818327)
    };

    sbswecdsa("9599788344976779383a7a0812a096943a1f771ee484d586af1a06207478e"
              "4c0be9c200d42460fe837e24b266c8852d80d3c53cc52ffb1913fc3261145"
              "fc6da575611efd16c026059a2e64f802517ffd1b6b34de10ad2909c65c215"
              "5e8d939b8115400c1d793d23955b15f5d1c13c962ff92b4a815cee0e10f8e"
              "14e1f6e6cd38", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_13)
{
    static const uint32_t s[17] =
    {
        IX521(00000161, 963a6237, b8955a8a, 756d8df5, dbd30314, 0bb90143,
              b1da5f07, b32f9cb6, 4733dc63, 16080924, 733f1e2c, 81ade9d0,
              be71b5b9, 5b556660, 26a035a9, 3ab3004d, 0bc0b19f)
    };
    static const uint32_t r[17] =
    {
        IX521(000001d7, ce382295, a2a10906, 4ea03f0a, d8761dd6, 0eefb9c2,
              07a20e3c, 5551e82a, c6d2ee59, 22b3e965, 5a65ba6c, 359dcbf8,
              fa843fbe, 87239a5c, 3e3eaece, c0407d2f, cdb687c2)
    };
    static const uint32_t qx[17] =
    {
        IX521(00000128, 57c2244f, a04db3b7, 3db48479, 27db63cc, e2fa6cb2,
              2724466d, 3e20bc95, 0a9250a1, 5eafd99f, 236a801e, 5271e8f9,
              0d9e8a97, f37c12f7, da65bce8, a2c93bcd, 25526205)
    };
    static const uint32_t qy[17] =
    {
        IX521(000000f3, 94e37c17, d5b8e35b, 488fa05a, 607dbc74, 26496504,
              3a1fb60e, 92edc212, 296ae72d, 7d6fe2e3, 457e67be, 853664e1,
              da64f57e, 44bd2590, 76b3bb2b, 06a2c604, fea1be9d)
    };

    sbswecdsa("fdde51acfd04eb0ad892ce9d6c0f90eb91ce765cbe3ce9d3f2defe8f69132"
              "4d26b968b8b90e77706b068585f2a3ee7bf3e910528f7403c5af745a6f9d7"
              "ba6c53abd885c3b1be583415b128f4d3f224daf8563476bd9aa61e9c8518c"
              "144335f8f879c03696bddbe3ac37a8fbede29861611feaa87e325e2f60278"
              "b4893ed57fb0", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_14)
{
    static const uint32_t s[17] =
    {
        IX521(00000072, fd88bb16, 84c4ca95, 31748dfc, e4c16103, 7fcd6ae5,
              c2803b71, 17fb60d3, db5df7df, 380591aa, f3073a30, 31306b76,
              f062dcc5, 47ded23f, 6690293c, 34a710e7, e9a226c3)
    };
    static const uint32_t r[17] =
    {
        IX521(000001c3, 262a3a3f, b74fa512, 4b71a6c7, f7b7e6d5, 6738eaba,
              f7666b37, 2b299b0c, 99ee8a16, be3df88d, d955de09, 3fc8c049,
              f76ee83a, 4138cee4, 1e5fe947, 55d27a52, ee44032f)
    };
    static const uint32_t qx[17] =
    {
        IX521(000001d7, f1e9e610, 619daa9d, 2efa5636, 10a37167, 7fe8b580,
              48fdc55a, 98a49970, f6afa664, 9c516f9c, 72085ca3, 722aa595,
              f45f2803, 402b01c8, 32d28aac, 63d9941f, 1a25dfea)
    };
    static const uint32_t qy[17] =
    {
        IX521(00000157, 1facce3f, cfe733a8, eef4e830, 5dfe9910, 3a370f82,
              b3f8d750, 85414f25, 92ad4496, 9a2ef819, 6c8b9809, f0eca2f7,
              ddc71c47, 879e3f37, a40b9fec, f97992b9, 7af29721)
    };

    sbswecdsa("beb34c997f905c77451ac392f7957a0ab8b23325bd5c63ca31c109ac8f655"
              "a1e3094240cb8a99284f8091de2ab9a7db2504d16251980b86be89ec3a3f4"
              "1162698bab51848880633e0b71a38f8896335853d8e836a2454ecab2acdcc"
              "052c8f659be1d703b13ae1b090334ac50ab0137ddb5e8b924c0e3d2e5789d"
              "aaef2fdd4a1e", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

START_TEST(SB_SW_ECDSA_Verify_NIST_P521_15)
{
    static const uint32_t s[17] =
    {
        IX521(00000009, 7ed9e005, 416fc944, e26bcc36, 61a09b35, c128fccc,
              dc274273, 9c8a301a, 338dd77d, 9d135716, 12a3b952, 4a6164b0,
              9fe73643, bbc31447, ee31ef44, a490843e, 4e7db23f)
    };
    static const uint32_t r[17] =
    {
        IX521(000000ce, f3f4babe, 6f9875e5, db28c27d, 6a197d60, 7c3641a9,
              0f10c2cc, 2cb302ba, 658aa151, dc76c507, 488b99f4, b3c8bb40,
              4fb5c852, f959273f, 412cbdd5, e713c5e3, f0e67f94)
    };
    static const uint32_t qx[17] =
    {
        IX521(0000009e, c1a3761f, e3958073, b9647f34, 202c5e8c, a2428d05,
              6facc4f3, fedc7077, fa87f1d1, eb30cc74, f6e3ff3d, 3f82df26,
              41cea1eb, 3ff1529e, 8a3866ae, 2055aace, c0bf68c4)
    };
    static const uint32_t qy[17] =
    {
        IX521(000000be, d0261b91, f664c3ff, 53e337d8, 321cb988, c3edc03b,
              46754680, 097e5a85, 85245d80, d0b7045c, 75a9c5be, 7f599d3b,
              5eea08d8, 28acb629, 4ae515a3, df57a37f, 903ef62e)
    };

    sbswecdsa("543c374af90c34f50ee195006d5f9d8dd986d09ad182fcbefa085567275ee"
              "e1e742bfe0af3d058675adeb5b9f87f248b00a9fbd2aa779129123a5b983f"
              "2f26fc3caf2ea34277550c22fe8c814c739b46972d50232993cddd63a3c99"
              "e20f5c5067d9b57e2d5db94317a5a16b5c12b5c4cafbc79cbc2f9940f074b"
              "bc7d0dc71e90", SUPS_LSBF, n521, s, r, px521, py521,
              qx, qy, true);
}
END_TEST

bool pointcheck(const uint32_t *qx,
                const uint32_t *qy,
                bool full_check,
                const uint32_t *order)
{
    struct SB_SW_ECDSA_Point521 Q;

    PARAMETER_NOT_USED(order);
    PARAMETER_NOT_USED(full_check);

    c_memcpy(&Q.x, qx, sizeof(Q.x));
    c_memcpy(&Q.y, qy, sizeof(Q.y));

    return SB_SW_ECDSA_Point_Check521((const uint8_t*)&Q);
}

START_TEST(SB_SW_ECDSA_PointCheck)
{
    static const uint32_t qx1[17] =
    {
        IX521(000001f2, 69692c47, a55242bb, 08731ff9, 20f4915b, fcecf4d4,
              431a8b48, 7c90d085, 65272c52, ca90c473, 97f7604b, c643982e,
              34d05178, e979c2cf, f7ea1b9e, aec18d69, ca7382de)
    };
    static const uint32_t qy1[17] =
    {
        IX521(00000075, 0bdd866f, ba3e92c2, 9599c002, ac6f9e2b, f39af852,
              1b7b133f, 70510e99, 18a94d3c, 279edec9, 7ab75ecd, a95e3dd7,
              861af84c, 543371c0, 55dc74ee, eff70617, 26818327)
    };

    static const uint32_t qx2[17] =
    {
        IX521(000001d7, f1e9e610, 619daa9d, 2efa5636, 10a37167, 7fe8b580,
              48fdc55a, 98a49970, f6afa664, 9c516f9c, 72085ca3, 722aa595,
              f45f2803, 402b01c8, 32d28aac, 63d9941f, 1a25dfea)
    };
    static const uint32_t qy2[17] =
    {
        IX521(00000157, 1facce3f, cfe733a8, eef4e830, 5dfe9910, 3a370f82,
              b3f8d750, 85414f25, 92ad4496, 9a2ef819, 6c8b9809, f0eca2f7,
              ddc71c47, 879e3f37, a40b9fec, f97992b9, 7af29721)
    };

    static const uint32_t qx3[17] =
    {
        IX521(000000c2, d540a755, 7f4530de, 35bbd94d, a8a6defb, ff783f54,
              a65292f8, f76341c9, 96cea387, 95805a1b, 97174a91, 47a86442,
              82e0d704, 0a6f8342, 3ef2a045, 32481563, 93a1782e)
    };
    static const uint32_t qy3[17] =
    {
        IX521(00000119, f746c5df, 8cec24e4, 849ac187, 0d0d8594, c799d2ce,
              b6c3bdf8, 91dfbd22, 42e7ea24, d6aec316, 6214734a, cc4cbf4d,
              a8f71e24, 29c5c187, b2b3a048, 527c861f, 58a9b97f)
    };

    /* Single bit changed. */
    static const uint32_t qx3b[17] =
    {
        IX521(000000c2, d540a755, 7f4530de, 35bbd94d, a8a6defb, ff783f54,
              a65292f8, f76341c9, 96cea387, 95805a1b, 97174a91, 47a86442,
              82e0d704, 0a6f8342, 3ef2a045, 32481563, 93a1782f)
    };
    static const uint32_t qy3b[17] =
    {
        IX521(00000119, f746c5df, 8cec24e4, 849ac187, 0d0d8594, c799d2ce,
              b6c3bdf8, 91dfbd22, 42e7ea24, d6aec316, 6214734a, cc4cbf4d,
              a8f71e24, 29c5c187, b2b3a048, 527c861f, 58a9b97f)
    };

    fail_if(pointcheck(qx1, qy1, false, n521) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, false, n521) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, false, n521) == false,
            "Qx/Qy from vector 3 (functional vector)");

    fail_if(pointcheck(qx1, qy1, true, n521) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx2, qy2, true, n521) == false,
            "Qx/Qy correct");
    fail_if(pointcheck(qx3, qy3, true, n521) == false,
            "Qx/Qy from vector 3 (functional vector)");

    /* Same as qx3/qy3, but with single bit changed from qx. */
    fail_if(pointcheck(qx3b, qy3b, false, n521) == true,
            "Qx/Qy accepted from altered public key");
    fail_if(pointcheck(qx3b, qy3b, true, n521) == true,
            "Qx/Qy accepted from altered public key");

    /* Odd combination: correct Qx and Qy, but from mixed vectors: */
    fail_if(pointcheck(qx2, qy3, false, n521) == true,
            "Qx/Qy accepted from mixed public key");
    fail_if(pointcheck(qx2, qy3, true, n521) == true,
            "Qx/Qy accepted from mixed public key");

}
END_TEST

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_ECDSA_Tests");
    sfzutf_tcase_create("SB_SW_ECDSA_Verify_NIST_P521");

    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_1);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_2);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_3);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_4);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_5);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_6);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_7);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_8);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_9);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_10);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_11);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_12);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_13);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_14);
    sfzutf_test_add(SB_SW_ECDSA_Verify_NIST_P521_15);
    sfzutf_test_add(SB_SW_ECDSA_PointCheck);
}

/* end of file sbswecdsa521.c */
