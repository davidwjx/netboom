/* sbswsha256.c
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

START_TEST(SB_SW_HASH_256_FIPS180_2_B1)
{
    static const uint32_t result_SHA1_FIPS180_2_B1_256[8] =
    {
        0xf20015ad, 0xb410ff61, 0x96177a9c, 0xb00361a3,
        0x5dae2223, 0x414140de, 0x8f01cfea, 0xba7816bf
    };

    sbswhashtest("616263", SUPS_LSBF,
                 result_SHA1_FIPS180_2_B1_256, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_256_FIPS180_2_B2)
{
    static const uint32_t result_SHA1_FIPS180_2_B2_256[8] =
    {
        0x19db06c1, 0xf6ecedd4, 0x64ff2167, 0xa33ce459,
        0x0c3e6039, 0xe5c02693, 0xd20638b8, 0x248d6a61
    };

    sbswhashtest("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                 SUPS_TEXT,
                 result_SHA1_FIPS180_2_B2_256, 8 * 4);
}
END_TEST


START_TEST(SB_SW_HASH_256_FIPS180_2_B3)
{
    static const uint32_t result_SHA1_FIPS180_2_B3_256[8] =
    {
        0xc7112cd0, 0x046d39cc, 0xa497200e, 0xf1809a48,
        0x84d73e67, 0x81a1c7e2, 0x9914fb92, 0xcdc76e5c
    };

    char *million_a = spal_malloc(1000001);
    fail_if(million_a == NULL, "Allocation %d bytes", (int)1000001);

    c_memset(million_a, 'a', 1000000);
    million_a[1000000] = 0;

    sbswhashtest(million_a, SUPS_TEXT,
                 result_SHA1_FIPS180_2_B3_256, 8 * 4);
    spal_free(million_a);
}
END_TEST

#ifdef PERF
static char test[4096];

START_TEST(SB_SW_HASH_SHA256_1)
{
    c_memset(test, 0x61, 1);
    test[1] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA256_8)
{
    c_memset(test, 0x61, 8);
    test[8] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA256_16)
{
    c_memset(test, 0x61, 16);
    test[16] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA256_52)
{
    c_memset(test, 0x61, 52);
    test[52] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA256_104)
{
    c_memset(test, 0x61, 104);
    test[104] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA256_1000)
{
    c_memset(test, 0x61, 1000);
    test[1000] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA256_4072)
{
    c_memset(test, 0x61, 4072);
    test[4072] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST
#endif /* PERF */

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_HASH_Tests_2");
    sfzutf_tcase_create("SB_SW_HASH_256");
    sfzutf_test_add(SB_SW_HASH_256_FIPS180_2_B1);
    sfzutf_test_add(SB_SW_HASH_256_FIPS180_2_B2);
    sfzutf_test_add(SB_SW_HASH_256_FIPS180_2_B3);

#ifdef PERF
    sfzutf_test_add(SB_SW_HASH_SHA256_1);
    sfzutf_test_add(SB_SW_HASH_SHA256_8);
    sfzutf_test_add(SB_SW_HASH_SHA256_16);
    sfzutf_test_add(SB_SW_HASH_SHA256_52);
    sfzutf_test_add(SB_SW_HASH_SHA256_104);
    sfzutf_test_add(SB_SW_HASH_SHA256_1000);
    sfzutf_test_add(SB_SW_HASH_SHA256_4072);
#endif /* PERF*/
}

/* end of file sbswsha256.c */
