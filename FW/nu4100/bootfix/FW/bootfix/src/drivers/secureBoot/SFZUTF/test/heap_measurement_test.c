/* heap_measurement_test.c
 *
 * Description: SFZUTF heap measurement functions
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
#include "spal_memory.h"

#ifdef HEAP_MEASUREMENT

START_TEST(Heap_Measurement_Test_Alloc_Free_1000)
{
    char * dynmem;
    char * dynmem1;

    dynmem = SPAL_Memory_Alloc(200);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Alloc(300);
    dynmem1 = SPAL_Memory_Alloc(300);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Alloc(500);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Alloc(700);
    SPAL_Memory_Free(dynmem);

    SPAL_Memory_Free(dynmem1);
}
END_TEST

START_TEST(Heap_Measurement_Test_Calloc_Free_1000)
{
    char *dynmem;

    dynmem = SPAL_Memory_Calloc(1, 200);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Calloc(2, 300);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Calloc(2, 500);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Calloc(1, 50);
    SPAL_Memory_Free(dynmem);

    dynmem = SPAL_Memory_Calloc(10, 10);
    SPAL_Memory_Free(dynmem);
}
END_TEST

START_TEST(Heap_Measurement_Test_ReAlloc_Free_1000)
{
    char *dynmem;

    dynmem = SPAL_Memory_Alloc(500);

    dynmem = SPAL_Memory_ReAlloc(dynmem, 1000);

    SPAL_Memory_Free(dynmem);
}
END_TEST

build_suite(void)
{
    sfzutf_suite_create ("Heap_Measurement_Test");
    sfzutf_tcase_create ("Tests");
    sfzutf_test_add(Heap_Measurement_Test_Alloc_Free_1000);
    sfzutf_test_add(Heap_Measurement_Test_Calloc_Free_1000);
    sfzutf_test_add(Heap_Measurement_Test_ReAlloc_Free_1000);
}

#else

#warning heap_measurement_test.c does not work without -DHEAP_MEASUREMENT
void
build_suite(void)
{
}

#endif /* HEAP_MEASUREMENT */

/* end o ffile heap_measurement_test.c */
