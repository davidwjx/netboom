/* stack_measurement_test.c
 *
 * Description: SFZUTF stack measurement functions
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
#include <stdio.h>

#ifdef STACK_MEASUREMENT

void
use_0byte(void)
{
    __asm__ volatile ("":::"memory");   /* Make GCC believe this function
                                           has some effects to prevent
                                           it optimizing this out. */
}

void
use_1byte(void)
{
    char c[1];
    c[0] = 1;
    __asm__ volatile (""::"r" (c));     /* Compiler sees something related to c =>
                                           cannot optimize c[] out. */
}

void
use_100byte(void)
{
    int i;
    char c[100];
    for (i = 0; i < 100; i++)
    {
        c[i] = 1;
    }
    __asm__ volatile (""::"r" (c));     /* Compiler sees something related to c =>
                                           cannot optimize c[] out. */
}

void
use_1000byte(void)
{
    int i;
    char c[1000];
    for (i = 0; i < 1000; i++)
    {
        c[i] = 1;
    }
    __asm__ volatile (""::"r" (c));     /* Compiler sees something related to c =>
                                           cannot optimize c[] out. */
}

START_TEST(Stack_Measurement_Test_Flat)
{
}
END_TEST

START_TEST(Stack_Measurement_Test_Flat_1000_byte)
{
    int i;
    char c[1000];
    for (i = 0; i < 1000; i++)
    {
        c[i] = 1;
    }
    __asm__ volatile (""::"r" (c));     /* Compiler sees something related to c =>
                                           cannot optimize c[] out. */
}
END_TEST

START_TEST(Stack_Measurement_Test_Flat_1000_byte_Call_100_byte)
{
    int i;
    char c[1000];
    for (i = 0; i < 1000; i++)
    {
        c[i] = 1;
    }
    __asm__ volatile (""::"r" (c));     /* Compiler sees something related to c =>
                                           cannot optimize c[] out. */
    use_100byte();
}
END_TEST

START_TEST (Stack_Measurement_Test_Call_0_byte)
{
    use_0byte();
}
END_TEST

START_TEST (Stack_Measurement_Test_Call_1_byte)
{
    use_1byte();
}
END_TEST

START_TEST (Stack_Measurement_Test_Call_100_byte)
{
    use_100byte();
}
END_TEST

START_TEST(Stack_Measurement_Test_Call_1000_byte)
{
    use_1000byte();
}
END_TEST

START_TEST(Stack_Measurement_Test_MultiCall_100_byte)
{
    use_1000byte();
    STACK_MEASUREMENT_START;
    use_100byte();
    STACK_MEASUREMENT_END;
    use_1000byte();
}
END_TEST

char *mystr = "";

START_TEST (Stack_Measurement_Test_printf)
{
    printf ("%.400s", mystr);
}
END_TEST

void
build_suite(void)
{
    sfzutf_suite_create ("Stack_Measurement_Test");
    sfzutf_tcase_create ("Tests");
    sfzutf_test_add (Stack_Measurement_Test_Flat);
    sfzutf_test_add (Stack_Measurement_Test_Flat_1000_byte);
    sfzutf_test_add (Stack_Measurement_Test_Call_0_byte);
    sfzutf_test_add (Stack_Measurement_Test_Call_1_byte);
    sfzutf_test_add (Stack_Measurement_Test_Call_100_byte);
    sfzutf_test_add (Stack_Measurement_Test_Call_1000_byte);
    sfzutf_test_add (Stack_Measurement_Test_Flat_1000_byte_Call_100_byte);
    sfzutf_test_add (Stack_Measurement_Test_MultiCall_100_byte);
    sfzutf_test_add (Stack_Measurement_Test_printf);
}

#else

#warning stack_measurement_test.c does not work without -DSTACK_MEASUREMENT
void
build_suite(void)
{
}

#endif /* STACK_MEASUREMENT */

/* end of file stack_measurement_test.c */
