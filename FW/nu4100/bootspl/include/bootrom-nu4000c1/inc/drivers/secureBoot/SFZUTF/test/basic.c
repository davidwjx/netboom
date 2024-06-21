/* basic.c
 *
 * Description: SFZUTF test suite.
 */

/*****************************************************************************
* Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "sfzutf.h"

static const int value5 = 5;

START_TEST(empty_test)
{
}
END_TEST

START_TEST(value5_cmp_1)
{
    fail_unless(value5 == 5, "Expected 5!");
}
END_TEST

START_TEST(value5_cmp_2)
{
    fail_if(value5 != 5, "Expected 5!");
}
END_TEST

START_TEST(value5_cmp_many)
{
    fail_if(value5 != 5, "Expected 5!");
}
END_TEST

void build_suite(void)
{
    sfzutf_suite_create("SFZUTF basics");
    sfzutf_tcase_create("fail_unless");
    sfzutf_test_add(empty_test);
    sfzutf_test_add(value5_cmp_1);
    sfzutf_test_add(value5_cmp_2);
    sfzutf_loop_test_add(value5_cmp_many, 1, 5);
}

/* end of file basic.c */
