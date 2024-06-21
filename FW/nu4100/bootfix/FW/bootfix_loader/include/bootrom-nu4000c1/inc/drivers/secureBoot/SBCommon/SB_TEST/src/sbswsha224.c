/* sbswsha224.c
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

START_TEST(SB_SW_HASH_224_FIPS180_2_CH1_Ex1)
{
    static const uint32_t result_SHA1_FIPS180_2_CH1_Ex1_224[7] =
    {
        0xe36c9da7, 0xbda0b3f7, 0x2aadbce4, 0xbda255b3,
        0x8642a477, 0x3405d822, 0x23097d22
    };

    sbswhashtest("616263", SUPS_LSBF,
                 result_SHA1_FIPS180_2_CH1_Ex1_224, 7 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_224_FIPS180_2_CH1_Ex2)
{
    static const uint32_t result_SHA1_FIPS180_2_CH1_Ex2_224[7] =
    {
        0x52522525, 0xb4f58b19, 0xb0c6455c, 0xfd890150,
        0x5dba5da1, 0x512776cc, 0x75388b16
    };

    sbswhashtest("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
                 SUPS_TEXT,
                 result_SHA1_FIPS180_2_CH1_Ex2_224, 7 * 4);
}
END_TEST


START_TEST(SB_SW_HASH_224_FIPS180_2_CH1_Ex3)
{
    static const uint32_t result_SHA1_FIPS180_2_CH1_Ex3_224[7] =
    {
        0x4ee7ad67, 0x1948b2ee, 0xf03f4258, 0x97618a4b,
        0xbbb4c1ea, 0x980c91d8, 0x20794655
    };

    char *million_a = spal_malloc(1000001);
    fail_if(million_a == NULL, "Allocation %d bytes", (int)1000001);

    c_memset(million_a, 'a', 1000000);
    million_a[1000000] = 0;

    sbswhashtest(million_a, SUPS_TEXT,
                 result_SHA1_FIPS180_2_CH1_Ex3_224, 7 * 4);
    spal_free(million_a);
}
END_TEST


#ifdef PERF
static char test[4096];

START_TEST(SB_SW_HASH_SHA224_1)
{
    c_memset(test, 0x61, 1);
    test[1] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA224_8)
{
    c_memset(test, 0x61, 8);
    test[8] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA224_16)
{
    c_memset(test, 0x61, 16);
    test[16] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA224_52)
{
    c_memset(test, 0x61, 52);
    test[52] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA224_104)
{
    c_memset(test, 0x61, 104);
    test[104] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA224_1000)
{
    c_memset(test, 0x61, 1000);
    test[1000] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST

START_TEST(SB_SW_HASH_SHA224_4072)
{
    c_memset(test, 0x61, 4072);
    test[4072] = 0;
    sbswhashtest(test, SUPS_TEXT, NULL, 8 * 4);
}
END_TEST
#endif /* PERF */

void build_suite(void)
{
    sfzutf_suite_create("SB_SW_HASH_Tests_1");
    sfzutf_tcase_create("SB_SW_HASH_224");
    sfzutf_test_add(SB_SW_HASH_224_FIPS180_2_CH1_Ex1);
    sfzutf_test_add(SB_SW_HASH_224_FIPS180_2_CH1_Ex2);
    sfzutf_test_add(SB_SW_HASH_224_FIPS180_2_CH1_Ex3);

#ifdef PERF
    sfzutf_test_add(SB_SW_HASH_SHA224_1);
    sfzutf_test_add(SB_SW_HASH_SHA224_8);
    sfzutf_test_add(SB_SW_HASH_SHA224_16);
    sfzutf_test_add(SB_SW_HASH_SHA224_52);
    sfzutf_test_add(SB_SW_HASH_SHA224_104);
    sfzutf_test_add(SB_SW_HASH_SHA224_1000);
    sfzutf_test_add(SB_SW_HASH_SHA224_4072);
#endif /* PERF*/
}

/* end of file sbswsha224.c */
