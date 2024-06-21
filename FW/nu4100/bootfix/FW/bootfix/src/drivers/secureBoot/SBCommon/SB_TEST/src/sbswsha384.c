/* sbswsha384.c
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
#include "c_lib.h"              // c_memset

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

START_TEST(SB_SW_HASH_384_FIPS180_2_B1)
{
    static const uint32_t result_SHA1_FIPS180_2_B1_384[12] =
    {
        0x34c825a7, 0x58baeca1, 0xa1e7cc23, 0x8086072b, 0x43ff5bed, 0x1a8b605a,
        0x0eded163, 0x272c32ab, 0x9ac65007, 0xb5a03d69, 0x45a35e8b, 0xcb00753f
    };

    sbswhashtest("616263", SUPS_LSBF,
                 result_SHA1_FIPS180_2_B1_384, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_384_FIPS180_2_B2)
{
    static const uint32_t result_SHA1_FIPS180_2_B2_384[12] =
    {
        0xe3c8452b, 0x5fe95b1f, 0x20bc4e6f, 0xb0455a85, 0xe5f36bc6, 0xfe8f450d,
        0x62af05ab, 0x7cf8b1d1, 0x1b470939, 0x3707a65b, 0xfc8dc739, 0x3391fddd
    };

    sbswhashtest("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                 SUPS_TEXT,
                 result_SHA1_FIPS180_2_B2_384, 12 * 4);
}
END_TEST


START_TEST(SB_SW_HASH_384_FIPS180_2_B3)
{
    static const uint32_t result_SHA1_FIPS180_2_B3_384[12] =
    {
        0x7f3d8985, 0xae97ddd8, 0x38ecc4eb, 0x07b8b3dc, 0x704c2a5b, 0x7972cec5,
        0x00f24852, 0xed149e9c, 0x310a4a1c, 0x086e834e, 0x716474cb, 0x9d0e1809
    };

    char *million_a = spal_malloc(1000001);
    fail_if(million_a == NULL, "Allocation %d bytes", (int)1000001);

    c_memset(million_a, 'a', 1000000);
    million_a[1000000] = 0;

    sbswhashtest(million_a, SUPS_TEXT,
                 result_SHA1_FIPS180_2_B3_384, 12 * 4);
    spal_free(million_a);
}
END_TEST

#ifdef PERF
static char test[4096];

START_TEST(SB_SW_HASH_SHA384_1)
{
    c_memset(test, 0x61, 1);
    test[1] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA384_8)
{
    c_memset(test, 0x61, 8);
    test[8] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA384_16)
{
    c_memset(test, 0x61, 16);
    test[16] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA384_52)
{
    c_memset(test, 0x61, 52);
    test[52] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA384_104)
{
    c_memset(test, 0x61, 104);
    test[104] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA384_1000)
{
    c_memset(test, 0x61, 1000);
    test[1000] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA384_4072)
{
    c_memset(test, 0x61, 4072);
    test[4072] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 12 * 4);
}
END_TEST
#endif /* PERF */

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_HASH_Tests_2");
    sfzutf_tcase_create("SB_SW_HASH_384");
    sfzutf_test_add(SB_SW_HASH_384_FIPS180_2_B1);
    sfzutf_test_add(SB_SW_HASH_384_FIPS180_2_B2);
    sfzutf_test_add(SB_SW_HASH_384_FIPS180_2_B3);

#ifdef PERF
    sfzutf_test_add(SB_SW_HASH_SHA384_1);
    sfzutf_test_add(SB_SW_HASH_SHA384_8);
    sfzutf_test_add(SB_SW_HASH_SHA384_16);
    sfzutf_test_add(SB_SW_HASH_SHA384_52);
    sfzutf_test_add(SB_SW_HASH_SHA384_104);
    sfzutf_test_add(SB_SW_HASH_SHA384_1000);
    sfzutf_test_add(SB_SW_HASH_SHA384_4072);
#endif /* PERF*/
}

/* end of file sbswsha384.c */
