/* dftest_clib.c
 *
 * Description: test Clib APIs of Driver Framework.
 */

/*****************************************************************************
* Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "basic_defs.h"
#include "sfzutf.h"
#include "c_lib.h"
#include "cs_dftest.h"

#ifndef DFTEST_REMOVE_CLIB

/* Small helper for non-constant filling.
   Non constant fill is bit better in detecting errors in
   mem* implementations than constant fill (memset). */
static void
test_helper_memset_incr(
    void *ptr,
    int initial_char,
    size_t len)
{
    unsigned char c = (unsigned char)initial_char;
    unsigned char *p = ptr;

    while(len > 0)
    {
        *p = c;
        p++;
        c++;
        len--;
    }
}

/* Range parameter for tests.
   Notice that some tests are of complexity this parameter ^4. */
#define TEST_MEMXXX_RANGE 64

START_TEST(test_memset)
{
    uint8_t array[TEST_MEMXXX_RANGE * 3];
    uint8_t array_bak[TEST_MEMXXX_RANGE * 3];
    int c;
    int dst_idx;
    int idx;
    int length;

    /* Try all constant values. */
    for(c = 0; c < 256; c++)
    {
        c_memset(array, c, sizeof(array));

        for(dst_idx = 0; dst_idx < sizeof(array); dst_idx++)
        {
            fail_if(array[dst_idx] != c,
                    "c_memset failed to fill array with specified value");
        }
    }

    /* Test over dst_idx and length */
    for(dst_idx = 0; dst_idx < TEST_MEMXXX_RANGE; dst_idx++)
    {
        for(length = 0; length < TEST_MEMXXX_RANGE; length++)
        {
            /* Initialize */
            test_helper_memset_incr(array, 0x3, sizeof(array));
            test_helper_memset_incr(array_bak, 0x3, sizeof(array));

            c_memset(array + dst_idx, 0xee, length);

            /* Validate target */
            for(idx = 0; idx < sizeof(array); idx++)
            {
                if (idx < dst_idx ||
                        idx >= dst_idx + length)
                {
                    /* Outsize modified range. */
                    fail_if(array[idx] != array_bak[idx],
                            "c_memset over/underflow.");
                }
                else
                {
                    /* Inside modified range. */
                    fail_if(array[idx] != 0xee,
                            "c_memset copied area contents unexpected");
                }
            }
        }
    }
}
END_TEST

/* variant = 0, test c_memcmp
   variant = 1, tset c_strcmp */
static void
test_helper_xxxcmp(
    int variant,
    uint8_t start_pad_byte,
    uint8_t end_pad_byte)
{
    /* Tests c_memcmp,
       test is similar to c_memcmp but ensure no alteration happens.
       tests only "memeq" capabilities. */

    uint8_t src[TEST_MEMXXX_RANGE * 3];
    uint8_t dst[TEST_MEMXXX_RANGE * 3];
    uint8_t src_bak[TEST_MEMXXX_RANGE * 3];
    int src_idx, dst_idx, length, idx;
    bool same_idx;

    for(src_idx = 0; src_idx < TEST_MEMXXX_RANGE; src_idx++)
    {
        for(length = 0; length < TEST_MEMXXX_RANGE; length++)
        {
            for(dst_idx = 0; dst_idx < TEST_MEMXXX_RANGE; dst_idx++)
            {

                /* Initialize */
                test_helper_memset_incr(src, 0x1, sizeof(src));
                test_helper_memset_incr(dst, 0x1, sizeof(dst));
                test_helper_memset_incr(src_bak, 0x1, sizeof(src_bak));

                /* Zero termiantion for possible string operations. */
                src[sizeof(src) - 1] = 0;
                dst[sizeof(dst) - 1] = 0;
                src_bak[sizeof(src_bak) - 1] = 0;

                /* Outside CMP range are differences */
                if (dst_idx > 0)
                {
                    dst[dst_idx - 1] = start_pad_byte;
                }
                dst[dst_idx + length] = end_pad_byte;

                same_idx = (dst_idx == src_idx);

                if (variant == 0)
                {
                    if (!c_memcmp(src + src_idx, dst + dst_idx, length) !=
                            (same_idx || length == 0))
                    {
                        fail("c_memcmp unexpected result.");
                    }
                }
                else if (variant == 1)
                {
                    int res;
                    uint8_t ctmp;

                    ctmp = src[src_idx + length];
                    src[src_idx + length] = end_pad_byte;

                    res = c_strcmp((const void *)(src + src_idx),
                                   (const void *)(dst + dst_idx));

                    src[src_idx + length] = ctmp;

                    if (length == 0)
                    {
                        fail_if(res != 0,
                                "zero length strings not equivalent");
                    }
                    else
                    {
                        fail_if((!res) != same_idx,
                                "c_strcmp unexpected result.");
                    }
                }

                if (dst_idx > 0)
                {
                    fail_if(dst[dst_idx - 1] != start_pad_byte,
                            "c_memcmp modified memory");
                    dst[dst_idx - 1] = src_bak[dst_idx - 1];
                }

                fail_if(dst[dst_idx + length] != end_pad_byte,
                        "c_memcmp modified memory");

                dst[dst_idx + length] = src_bak[dst_idx + length];

                /* Check no modifications to memory. */
                for(idx = 0; idx < sizeof(dst); idx++)
                {
                    fail_if(src[idx] != src_bak[idx],
                            "c_memcmp modified memory.");

                    fail_if(dst[idx] != src[idx],
                            "c_memcmp modified memory.");
                }

            }

            /* Check that c_memcmp works for same address */
            if (c_memcmp(src + src_idx, src + src_idx, length) != 0)
            {
                fail("Comparing itself with c_memcmp didn't return 0");

            }
        }
    }
}


START_TEST(test_memcmp)
{
    int CmpResult;

    test_helper_xxxcmp(0, 0xee, 0xef);

    /* Check ordering. */
    CmpResult = c_memcmp("ABC", "abc", 3);
    fail_unless(CmpResult < 0, "Ordering of characters");
    CmpResult = c_memcmp("ABCDEFGHIJ", "0123456789", 10);
    fail_unless(CmpResult > 0, "Ordering of characters/numbers");
    CmpResult = c_memcmp("ABCDEFGHIJ", "ABCDEFGHIJK", 11);
    fail_unless(CmpResult < 0, "Ordering of strings with different length");
    CmpResult = c_memcmp("abcd", "abc\277", 4);
    fail_unless(CmpResult < 0,
                "c_memcmp between ordinary characters and high-ASCII");
    CmpResult = c_memcmp("a\000bc", "a\000bd", 4);
    fail_unless(CmpResult < 0, "c_memcmp with ascii zero");
}
END_TEST


START_TEST(test_strcmp)
{
    int CmpResult;

    test_helper_xxxcmp(1, 0xee, 0);

    /* Check ordering. */
    CmpResult = c_strcmp("ABC", "abc");
    fail_unless(CmpResult < 0, "Ordering of characters");

    CmpResult = c_strcmp("ABCDEFGHIJ", "0123456789");
    fail_unless(CmpResult > 0, "Ordering of characters/numbers");

    CmpResult = c_strcmp("ABCDEFGHIJ", "ABCDEFGHIJK");
    fail_unless(CmpResult < 0, "Ordering of strings with different length");

    CmpResult = c_strcmp("abcd", "abc\277");
    fail_unless(CmpResult < 0, "c_strcmp between ordinary characters and high-ASCII");

    CmpResult = c_strcmp("a\000bc", "a\000bd");
    fail_unless(CmpResult == 0, "c_strcmp with ascii zero");
}
END_TEST

/* variant = 0, test c_memcpy
   variant = 1, tset c_memmove */
static void
test_helper_xxxcpy(int variant)
{
    /* Tests c_memcpy and c_memmove, */

    uint8_t src[TEST_MEMXXX_RANGE * 3];
    uint8_t dst[TEST_MEMXXX_RANGE * 3];
    uint8_t src_bak[TEST_MEMXXX_RANGE * 3];
    uint8_t dst_bak[TEST_MEMXXX_RANGE * 3];
    int src_idx, dst_idx, length, idx;
    int cmp_result;

    for(src_idx = 0; src_idx < TEST_MEMXXX_RANGE; src_idx++)
    {
        for(dst_idx = 0; dst_idx < TEST_MEMXXX_RANGE; dst_idx++)
        {
            for(length = 0; length < TEST_MEMXXX_RANGE; length++)
            {
                /* Initialize */
                test_helper_memset_incr(src, 0x1, sizeof(src));
                test_helper_memset_incr(dst, 0x82, sizeof(dst));
                test_helper_memset_incr(src + src_idx, 0x44, length);

                /* Same initialization for backups. */
                test_helper_memset_incr(src_bak, 0x1, sizeof(src_bak));
                test_helper_memset_incr(dst_bak, 0x82, sizeof(dst_bak));
                test_helper_memset_incr(src_bak + src_idx, 0x44, length);

                /* Actual c_memcpy/c_memmove. */
                if (variant == 0)
                {
                    c_memcpy(dst + dst_idx, src + src_idx, length);
                }
                else if (variant == 1)
                {
                    c_memmove(dst + dst_idx, src + src_idx, length);
                }

                /* Check results, with c_memcmp. */
                cmp_result = c_memcmp(src, src_bak, sizeof(src));
                fail_if(cmp_result != 0,
                        "c_memcpy/move altered source data");

                if (length == 0)
                {
                    cmp_result = c_memcmp(dst_bak, dst, sizeof(dst));
                    fail_if(cmp_result != 0,
                            "Zero length c_memcpy/move altered dst");
                }

                /* Validate target */
                for(idx = 0; idx < sizeof(dst); idx++)
                {
                    if (idx < dst_idx ||
                            idx >= dst_idx + length)
                    {
                        /* Outsize modified range. */
                        fail_if(dst[idx] != dst_bak[idx],
                                "c_memcpy/move over/underflow.");
                    }
                    else
                    {
                        /* Inside modified range. */
                        fail_if(dst[idx] != src[idx - dst_idx + src_idx],
                                "c_memcpy/move copied area contents unexpected");
                    }
                }
            }
        }
    }
}


START_TEST(test_memcpy)
{
    test_helper_xxxcpy(0);
}
END_TEST


START_TEST(test_memmove)
{
    uint8_t array[TEST_MEMXXX_RANGE * 3];
    uint8_t array_bak[TEST_MEMXXX_RANGE * 3];
    int src_idx, dst_idx, length, idx;

    test_helper_xxxcpy(1);

    /* Continue with c_memmove specific testing. */
    for(src_idx = 0; src_idx < TEST_MEMXXX_RANGE; src_idx++)
    {
        for(dst_idx = 0; dst_idx < TEST_MEMXXX_RANGE; dst_idx++)
        {
            for(length = 0; length < TEST_MEMXXX_RANGE; length++)
            {
                /* Initialize */
                test_helper_memset_incr(array, 0x1, sizeof(array));
                test_helper_memset_incr(array_bak, 0x1, sizeof(array_bak));

                /* Actual c_memmove function. */
                c_memmove(array + dst_idx, array + src_idx, length);

                /* Validate target */
                for(idx = 0; idx < sizeof(array); idx++)
                {
                    if (idx < dst_idx ||
                            idx >= dst_idx + length)
                    {
                        /* Outsize modified range. */
                        fail_if(array[idx] != array_bak[idx],
                                "c_memmove over/underflow.");
                    }
                    else
                    {
                        /* Inside modified range. */
                        fail_if(array[idx] !=
                                array_bak[idx - dst_idx + src_idx],
                                "c_memmove copied area contents unexpected");
                    }
                }
            }
        }
    }
}
END_TEST

struct test_char_struct
{
    char a;
    char b;
    char c;
};

START_TEST(test_offsetof)
{
    /* Tests offsetof.
       If fails, may fail also due to structure alignment on this platform.
       (char in structs not aligned to minimal padding).
       In this case, you may need to provide additional flags to compiler to
       fix alignment issues. */

    fail_if(offsetof(struct test_char_struct, a) != 0,
            "offsetof() or structure padding failure [member=a]");

    fail_if(offsetof(struct test_char_struct, b) != 1,
            "offsetof() or structure padding failure [member=b]");

    fail_if(offsetof(struct test_char_struct, c) != 2,
            "offsetof() or structure padding failure [member=c]");

    /* Would be good to test with other types except char, but
       it varies according to platform how much padding is added. */
}
END_TEST

#define PRE_INIT_VALUE 0xaa
START_TEST(test_zero_init)
{
    union
    {
        uint8_t a;
        uint16_t b;
        uint32_t c;
        uint8_t * d;
        struct
        {
            uint8_t a;
            uint16_t b;
        } e;
        uint8_t f[5];
        uint8_t g[32];
    } ZiUnion;
    uint8_t BackArray[32];
    uint32_t width;
    int CmpResult;

    /* initialize uint8_t */
    c_memset((void *)BackArray, PRE_INIT_VALUE, sizeof(BackArray));
    c_memset((void *)&ZiUnion, PRE_INIT_VALUE, sizeof(ZiUnion));
    ZEROINIT(ZiUnion.a);
    fail_unless(ZiUnion.a == 0,
                "uint8_t variable was not initialized to 0");
    width = sizeof(ZiUnion.a);
    CmpResult =
        c_memcmp(&ZiUnion.f[width], &BackArray[width], sizeof(BackArray) - width);
    fail_unless(CmpResult == 0, "unexpected data corrupted");

    /* initialize uint16_t */
    c_memset((void *)&ZiUnion, PRE_INIT_VALUE, sizeof(ZiUnion));
    ZEROINIT(ZiUnion.b);
    width = sizeof(ZiUnion.b);
    fail_unless(ZiUnion.b == 0,
                "uint16_t variable was not initialized to 0");
    CmpResult =
        c_memcmp(&ZiUnion.f[width], &BackArray[width], sizeof(BackArray) - width);
    fail_unless(CmpResult == 0, "unexpected data corrupted");

    /* initialize uint32_t */
    c_memset((void *)&ZiUnion, PRE_INIT_VALUE, sizeof(ZiUnion));
    ZEROINIT(ZiUnion.c);
    width = sizeof(ZiUnion.c);
    fail_unless(ZiUnion.c == 0,
                "uint32_t variable was not initialized to 0");
    CmpResult =
        c_memcmp(&ZiUnion.f[width], &BackArray[width], sizeof(BackArray) - width);
    fail_unless(CmpResult == 0, "unexpected data corrupted");

    /* initialize uint8_t * */
    c_memset((void *)&ZiUnion, PRE_INIT_VALUE, sizeof(ZiUnion));
    ZEROINIT(ZiUnion.d);
    width = sizeof(ZiUnion.d);
    fail_unless(ZiUnion.d == 0,
                "uint8_t * variable was not initialized to 0");
    CmpResult =
        c_memcmp(&ZiUnion.f[width], &BackArray[width], sizeof(BackArray) - width);
    fail_unless(CmpResult == 0, "unexpected data corrupted");

    /* initialize structure */
    c_memset((void *)&ZiUnion, PRE_INIT_VALUE, sizeof(ZiUnion));
    ZEROINIT(ZiUnion.e);
    width = sizeof(ZiUnion.e);
    c_memset(&BackArray, 0x00, sizeof(ZiUnion.e));
    CmpResult = c_memcmp(&ZiUnion, &BackArray, sizeof(ZiUnion.e));
    fail_unless(CmpResult == 0,
                "structure variable was not initialized to 0");
    CmpResult =
        c_memcmp(&ZiUnion.f[width], &BackArray[width], sizeof(BackArray) - width);
    fail_unless(CmpResult  == 0, "unexpected data corrupted");

    /* initialize array */
    c_memset((void *)&ZiUnion, PRE_INIT_VALUE, sizeof(ZiUnion));
    c_memset((void *)BackArray, PRE_INIT_VALUE, sizeof(BackArray));
    ZEROINIT(ZiUnion.f);
    width = sizeof(ZiUnion.f);
    c_memset(&BackArray, 0x00, sizeof(ZiUnion.f));
    CmpResult = c_memcmp(&ZiUnion, &BackArray, sizeof(ZiUnion.f));
    fail_unless(CmpResult == 0,
                "array variable was not initialized to 0");
    CmpResult =
        c_memcmp(&ZiUnion.f[width], &BackArray[width], sizeof(BackArray) - width);
    fail_unless(CmpResult == 0, "unexpected data corrupted");

}
END_TEST

void suite_add_test_clib(void)
{
    sfzutf_tcase_create("Clib_Tests");
    sfzutf_test_add(test_memset);
    sfzutf_test_add(test_memcmp);
    sfzutf_test_add(test_strcmp);
    sfzutf_test_add(test_memcpy);
    sfzutf_test_add(test_memmove);
    sfzutf_test_add(test_offsetof);
    sfzutf_test_add(test_zero_init);
}

#endif /* DFTEST_REMOVE_CLIB */

/* end of file dftest_clib.c */
