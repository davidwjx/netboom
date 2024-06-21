/* sbswsha512.c
 *
 * Description: FIPS tests for SB
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

#include "lib-sbswhashtest.h"

#ifdef OLD_SPAL
#include "spal_memory.h"
#else /* !OLD_SPAL */
#ifdef __CC_ARM
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#define spal_malloc malloc
#define spal_free free
#endif /* OLD_SPAL */

/* These vectors are from FIPS 180-2 with change notice. */

START_TEST(SB_SW_HASH_512_FIPS180_2_B1)
{
    static const uint32_t result_SHA1_FIPS180_2_B1_512[16] =
    {
        0xa54ca49f, 0x2a9ac94f, 0x643ce80e, 0x454d4423,
        0xa3feebbd, 0x36ba3c23, 0x274fc1a8, 0x2192992a,
        0x4b55d39a, 0x0a9eeee6, 0x89a97ea2, 0x12e6fa4e,
        0xae204131, 0xcc417349, 0x93617aba, 0xddaf35a1
    };

    sbswhashtest("616263", SUPS_LSBF,
                 result_SHA1_FIPS180_2_B1_512, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_512_FIPS180_2_B2)
{
    static const uint32_t result_SHA1_FIPS180_2_B2_512[16] =
    {
        0x38ca3445, 0x54ec6312, 0xa71dd703, 0x31ad85c7,
        0x57789ca0, 0xaa1d3bea, 0x3b1b07f9, 0x96fd15c1,
        0xa703c335, 0x279be331, 0x68b228a8, 0x57c16ef4,
        0x8e08a416, 0x0ced7beb, 0xdda82f0a, 0x204a8fc6
    };

    sbswhashtest("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                 SUPS_TEXT,
                 result_SHA1_FIPS180_2_B2_512, 16 * 4);
}
END_TEST


START_TEST(SB_SW_HASH_512_FIPS180_2_B3)
{
    static const uint32_t result_SHA1_FIPS180_2_B3_512[16] =
    {
        0xad8cc09b, 0x4eadb217, 0x2c49aa2e, 0xeb009c5c,
        0xe577c31b, 0x4cb0432c, 0x877ea60a, 0xde0ff244,
        0xafa973eb, 0x5632a803, 0x3b204428, 0x8e1f98b1,
        0xbc15b463, 0x4e2e42c7, 0x0ce76964, 0xe718483d
    };

    char *million_a = spal_malloc(1000001);
    fail_if(million_a == NULL, "Allocation %d bytes", (int)1000001);

    c_memset(million_a, 'a', 1000000);
    million_a[1000000] = 0;

    sbswhashtest(million_a, SUPS_TEXT,
                 result_SHA1_FIPS180_2_B3_512, 16 * 4);
    spal_free(million_a);
}
END_TEST

#ifdef PERF
static char test[4096];

START_TEST(SB_SW_HASH_SHA512_1)
{
    c_memset(test, 0x61, 1);
    test[1] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA512_8)
{
    c_memset(test, 0x61, 8);
    test[8] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA512_16)
{
    c_memset(test, 0x61, 16);
    test[16] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA512_52)
{
    c_memset(test, 0x61, 52);
    test[52] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA512_104)
{
    c_memset(test, 0x61, 104);
    test[104] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA512_1000)
{
    c_memset(test, 0x61, 1000);
    test[1000] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA512_4072)
{
    c_memset(test, 0x61, 4072);
    test[4072] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 16 * 4);
}
END_TEST
#endif /* PERF */

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_HASH_Tests_2");
    sfzutf_tcase_create("SB_SW_HASH_512");
    sfzutf_test_add(SB_SW_HASH_512_FIPS180_2_B1);
    sfzutf_test_add(SB_SW_HASH_512_FIPS180_2_B2);
    sfzutf_test_add(SB_SW_HASH_512_FIPS180_2_B3);

#ifdef PERF
    sfzutf_test_add(SB_SW_HASH_SHA512_1);
    sfzutf_test_add(SB_SW_HASH_SHA512_8);
    sfzutf_test_add(SB_SW_HASH_SHA512_16);
    sfzutf_test_add(SB_SW_HASH_SHA512_52);
    sfzutf_test_add(SB_SW_HASH_SHA512_104);
    sfzutf_test_add(SB_SW_HASH_SHA512_1000);
    sfzutf_test_add(SB_SW_HASH_SHA512_4072);
#endif /* PERF*/
}

/* end of file sbswsha512.c */
