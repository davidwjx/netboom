/* dftest_basic_defs.c
 *
 * Description: test Basic Defines of Driver Framework.
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
#include "c_lib.h"
#include "sfzutf.h"
#include "cs_dftest.h"

#ifndef DFTEST_REMOVE_BASIC_DEFINITIONS

START_TEST(test_uints)
{
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;


    fail_unless(sizeof(u8) == 1, "uint8_t requires too much storage");

    u8 = 0;
    fail_unless(u8 == 0, "Check assign to uint8_t");

    u8 = UINT8_MAX;
    fail_unless(u8 == UINT8_MAX, "Check assign to uint8_t");


    fail_unless(sizeof(u16) == 2, "uint16_t requires too much storage");

    u16 = 0;
    fail_unless(u16 == 0, "Check assign to uint16_t");

    u16 = UINT16_MAX;
    fail_unless(u16 == UINT16_MAX, "Check assign to uint16_t");


    fail_unless(sizeof(u32) == 4, "uint32_t requires too much storage");

    u32 = 0;
    fail_unless(u32 == 0, "Check assign to uint32_t");

    u32 = UINT32_MAX;
    fail_unless(u32 == UINT32_MAX, "Check assign to uint32_t");

}
END_TEST

START_TEST(test_bool)
{
    bool boolean_value;

    boolean_value = true;
    fail_unless(boolean_value, "Boolean value true behaving unexpectedly");
    fail_unless(boolean_value == true,
                "Boolean value true behaving unexpectedly");
    fail_unless(boolean_value != 0,
                "Boolean value true behaving unexpectedly");

    boolean_value = false;
    fail_if(boolean_value, "Boolean value false behaving unexpectedly");
    fail_unless(boolean_value == false,
                "Boolean value true behaving unexpectedly");
    fail_unless(boolean_value == 0,
                "Boolean value true behaving unexpectedly");
}
END_TEST

START_TEST(test_null)
{
    /* Test null pointer is equivalent to zero. */

    void *ptr_array[2];
    uint8_t *ptr_u8;
    uint16_t *ptr_u16;
    uint32_t *ptr_u32;
    uintptr_t null_value;

    null_value = (uintptr_t) NULL;

    fail_unless(null_value == 0, "NULL != 0");

    c_memset(ptr_array, 0, sizeof(ptr_array));
    fail_unless(ptr_array[0] == NULL,
                "Init to zero didn't produce null pointers");

    fail_unless(ptr_array[1] == NULL,
                "Init to zero didn't produce null pointers");
    ptr_u8 = (uint8_t *)NULL;
    fail_unless(ptr_u8 == (uint8_t *)NULL,
                "Null was not assigned to uint8_t * pointer");
    ptr_u16 = (uint16_t *)NULL;
    fail_unless(ptr_u16 == (uint16_t *)NULL,
                "Null was not assigned to uint16_t * pointer");
    ptr_u32 = (uint32_t *)NULL;
    fail_unless(ptr_u32 == (uint32_t *)NULL,
                "Null was not assigned to uint32_t * pointer");
}
END_TEST

START_TEST(test_min)
{
    fail_if(MIN(2, 3) != 2, "MIN() failed");
    fail_if(MIN(-3, 1) != -3, "MIN() failed with negative numbers");
    fail_if(MIN(-1, -3) != -3, "MIN() failed with all negative numbers");

    fail_if(MIN(INT32_MIN, INT32_MAX) != INT32_MIN,
            "MIN failed with 32-bit numbers");

    fail_if(MIN(0, UINT32_MAX) != 0,
            "MIN failed with unsigned 32-bit numbers");

}
END_TEST


START_TEST(test_max)
{
    fail_if(MAX(2, 3) != 3, "MAX() failed");
    fail_if(MAX(-3, 1) != 1, "MAX() failed with negative numbers");
    fail_if(MAX(-3, -1) != -1, "MAX() failed with all negative numbers");

    fail_if(MAX(INT32_MAX, INT32_MAX) != INT32_MAX,
            "MAX failed with 32-bit numbers");

    fail_if(MAX(INT32_MAX, UINT32_MAX) != UINT32_MAX,
            "MAX failed with unsigned 32-bit numbers");
}
END_TEST

START_TEST(test_bit)
{
    uint32_t bit_value;

    bit_value = 1;
    fail_unless(BIT_0 == bit_value, "BIT_0 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_1 == bit_value, "BIT_1 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_2 == bit_value, "BIT_2 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_3 == bit_value, "BIT_3 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_4 == bit_value, "BIT_4 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_5 == bit_value, "BIT_5 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_6 == bit_value, "BIT_6 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_7 == bit_value, "BIT_7 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_8 == bit_value, "BIT_8 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_9 == bit_value, "BIT_9 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_10 == bit_value, "BIT_10 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_11 == bit_value, "BIT_11 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_12 == bit_value, "BIT_12 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_13 == bit_value, "BIT_13 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_14 == bit_value, "BIT_14 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_15 == bit_value, "BIT_15 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_16 == bit_value, "BIT_16 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_17 == bit_value, "BIT_17 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_18 == bit_value, "BIT_18 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_19 == bit_value, "BIT_19 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_20 == bit_value, "BIT_20 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_21 == bit_value, "BIT_21 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_22 == bit_value, "BIT_22 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_23 == bit_value, "BIT_23 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_24 == bit_value, "BIT_24 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_25 == bit_value, "BIT_25 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_26 == bit_value, "BIT_26 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_27 == bit_value, "BIT_27 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_28 == bit_value, "BIT_28 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_29 == bit_value, "BIT_29 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_30 == bit_value, "BIT_30 value unexpected.");
    bit_value <<= 1;
    fail_unless(BIT_31 == bit_value, "BIT_31 value unexpected.");
}
END_TEST

START_TEST(test_mask_n_bits)
{
    uint32_t bit_value;

    bit_value = BIT_0;
    fail_unless(MASK_1_BIT == bit_value, "MASK_1_BIT value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_2_BITS == bit_value, "MASK_2_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_3_BITS == bit_value, "MASK_3_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_4_BITS == bit_value, "MASK_4_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_5_BITS == bit_value, "MASK_5_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_6_BITS == bit_value, "MASK_6_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_7_BITS == bit_value, "MASK_7_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_8_BITS == bit_value, "MASK_8_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_9_BITS == bit_value, "MASK_9_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_10_BITS == bit_value, "MASK_10_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_11_BITS == bit_value, "MASK_11_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_12_BITS == bit_value, "MASK_12_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_13_BITS == bit_value, "MASK_13_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_14_BITS == bit_value, "MASK_14_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_15_BITS == bit_value, "MASK_15_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_16_BITS == bit_value, "MASK_16_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_17_BITS == bit_value, "MASK_17_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_18_BITS == bit_value, "MASK_18_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_19_BITS == bit_value, "MASK_19_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_20_BITS == bit_value, "MASK_20_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_21_BITS == bit_value, "MASK_21_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_22_BITS == bit_value, "MASK_22_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_23_BITS == bit_value, "MASK_23_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_24_BITS == bit_value, "MASK_24_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_25_BITS == bit_value, "MASK_25_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_26_BITS == bit_value, "MASK_26_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_27_BITS == bit_value, "MASK_27_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_28_BITS == bit_value, "MASK_28_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_29_BITS == bit_value, "MASK_29_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_30_BITS == bit_value, "MASK_30_BITS value unexpected.");
    bit_value = (bit_value << 1) + 1;
    fail_unless(MASK_31_BITS == bit_value, "MASK_31_BITS value unexpected.");
}
END_TEST

static bool
test_identifier_helper(
    uint32_t a,
    uint32_t b,
    uint32_t c,
    uint32_t d)
{
    uint32_t x;
    uint32_t y;

    IDENTIFIER_NOT_USED(a);
    IDENTIFIER_NOT_USED(b);
    x = c >> 1;
    y = x++;
    IDENTIFIER_NOT_USED(d);

    return ((y > 0) ? true : false);
}

START_TEST(test_identifier_not_used)
{
    fail_unless(test_identifier_helper(1, 2, 3, 4),
                "IDENTIFIER_NOT_USED failed");
}
END_TEST


static inline uint32_t
test_inline_helper(
    uint32_t a,
    uint32_t b)
{
    return (a & b) ;
}

START_TEST(test_inline)
{
    fail_unless(test_inline_helper(1, 1) > 0, "Inline function failed");
}
END_TEST

void suite_add_test_basic_defs(void)
{
    sfzutf_tcase_create("Basic_Definitions_Tests");
    sfzutf_test_add(test_uints);
    sfzutf_test_add(test_bool);
    sfzutf_test_add(test_null);
    sfzutf_test_add(test_min);
    sfzutf_test_add(test_max);
    sfzutf_test_add(test_bit);
    sfzutf_test_add(test_mask_n_bits);
    sfzutf_test_add(test_identifier_not_used);
    sfzutf_test_add(test_inline);
}

#endif /* DFTEST_REMOVE_BASIC_DEFINITIONS */

/* end of file dftest_basic_defs.c */
