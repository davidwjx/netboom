/* framework_testsuite.c
 *
 * Description: Test Suite for the Framework APIs
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
#include "spal_mutex.h"
#include "spal_semaphore.h"
#include "spal_thread.h"
#include "spal_sleep.h"
#include "spal_memory.h"
#include "c_lib.h"
#include "sfzutf.h"

START_TEST(test_ints)
{
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;

    fail_unless(sizeof(i8) == 1, "int8_t requires too much storage");

    i8 = INT8_MIN;
    fail_unless(i8 == INT8_MIN, "Check assign to int8_t");

    i8 = INT8_MAX;
    fail_unless(i8 == INT8_MAX, "Check assign to int8_t");


    fail_unless(sizeof(i16) == 2, "int16_t requires too much storage");

    i16 = INT16_MIN;
    fail_unless(i16 == INT16_MIN, "Check assign to int16_t");

    i16 = INT16_MAX;
    fail_unless(i16 == INT16_MAX, "Check assign to int16_t");


    fail_unless(sizeof(i32) == 4, "int32_t requires too much storage");

    i32 = INT32_MIN;
    fail_unless(i32 == INT32_MIN, "Check assign to int32_t");

    i32 = INT32_MAX;
    fail_unless(i32 == INT32_MAX, "Check assign to int32_t");


    fail_unless(sizeof(i64) == 8, "int64_t requires too much storage");

    i64 = INT64_MIN;
    fail_unless(i64 == INT64_MIN, "Check assign to int64_t");

    i64 = INT64_MAX;
    fail_unless(i64 == INT64_MAX, "Check assign to int64_t");
}
END_TEST

START_TEST(test_uints)
{
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;

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


    fail_unless(sizeof(u64) == 8, "uint64_t requires too much storage");

    u64 = 0;
    fail_unless(u64 == 0, "Check assign to uint64_t");

    u64 = UINT64_MAX;
    fail_unless(u64 == UINT64_MAX, "Check assign to uint64_t");
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
    fail_if(boolean_value, "Boolean value true behaving unexpectedly");
    fail_unless(boolean_value == false,
                "Boolean value true behaving unexpectedly");
    fail_unless(boolean_value == 0,
                "Boolean value true behaving unexpectedly");
}
END_TEST

START_TEST(test_ptr_size)
{
    size_t sz = 1;
    uintptr_t int_p;
    const char *c = "abc";

    /* Side effect of following comparison:
       Check size_t is (to some degree) type
       compatible with sizeof() result. */
    fail_if(sz != sizeof(*c), "unexpected size of char");

    /* Check pointer arithmetic is possible via uintptr_t. */
    int_p = (uintptr_t) c;
    int_p += 1;
    c = (char *) int_p;

    fail_unless(*c == 'b', "Arithmetic with uintpr_t unexpected result");
}
END_TEST

START_TEST(test_min)
{
    fail_if(MIN(2, 3) != 2, "MIN() failed");
    fail_if(MIN(-3, 1) != -3, "MIN() failed with negative numbers");
    fail_if(MIN(-1, -3) != -3, "MIN() failed with all negative numbers");

    fail_if(MIN(INT32_MIN, INT32_MAX) != INT32_MIN,
            "Min failed with 64-bit numbers");

    fail_if(MIN(INT64_MIN, INT64_MAX) != INT64_MIN,
            "Min failed with 64-bit numbers");

    fail_if(MIN(0, UINT64_MAX) != 0,
            "Min failed with signed 64-bit numbers");
}
END_TEST


START_TEST(test_max)
{
    fail_if(MAX(2, 3) != 3, "MAX() failed");
    fail_if(MAX(-3, 1) != 1, "MAX() failed with negative numbers");
    fail_if(MAX(-3, -1) != -1, "MAX() failed with all negative numbers");

    fail_if(MAX(INT32_MAX, INT32_MAX) != INT32_MAX,
            "Min failed with 64-bit numbers");

    fail_if(MAX(INT64_MAX, INT64_MAX) != INT64_MAX,
            "Min failed with 64-bit numbers");

    fail_if(MAX(INT64_MAX, UINT64_MAX) != UINT64_MAX,
            "Min failed with unsigned 64-bit numbers");
}
END_TEST


START_TEST(test_bit)
{
    uint32_t bit_value;
    fail_unless(BIT_0 == 1, "Unexpected value for BIT_0.");
    bit_value = BIT_0;
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

START_TEST(test_precondition)
{
#ifdef IMPLDEFS_CF_DISABLE_PRECONDITION
    /* Preconditions disabled, test is half unsupported. */
    if (_i > 0)
    {
        unsupported_quick("Preconditions disabled.");
    }
    else
    {
        PRECONDITION(_i == 0);
    }
#else /* IMPLDEFS_CF_DISABLE_PRECONDITION */
    if (_i > 0)
    {
        SFZUTF_EXPECT_PRECONDITION;
    }
    PRECONDITION(_i == 0); /* Should trigger on second execution */
    if (_i > 0)
    {
        fail("Expected failure due to precondition did not trigger.");
    }
#endif /* IMPLDEFS_CF_DISABLE_PRECONDITION */
}
END_TEST


START_TEST(test_postcondition)
{
#ifdef IMPLDEFS_CF_DISABLE_POSTCONDITION
    /* Postconditions disabled, test is half unsupported. */
    if (_i > 0)
    {
        unsupported_quick("Postconditions disabled.");
    }
    else
    {
        POSTCONDITION(_i == 0);
    }
#else /* IMPLDEFS_CF_DISABLE_POSTCONDITION */
    if (_i > 0)
    {
        SFZUTF_EXPECT_POSTCONDITION;
    }
    POSTCONDITION(_i == 0); /* Should trigger on second execution */
    if (_i > 0)
    {
        fail("Expected failure due to postcondition did not trigger.");
    }
#endif /* IMPLDEFS_CF_DISABLE_POSTCONDITION */
}
END_TEST

START_TEST(test_assertion)
{
#ifdef IMPLDEFS_CF_DISABLE_ASSERT
    /* Asserts disabled, test is half unsupported. */
    if (_i > 0)
    {
        unsupported_quick("Asserts disabled.");
    }
    else
    {
        ASSERT(_i == 0);
    }
#else /* IMPLDEFS_CF_DISABLE_ASSERT */
    if (_i > 0)
    {
        SFZUTF_EXPECT_ASSERT;
    }
    ASSERT(_i == 0); /* Should trigger on second execution */
    if (_i > 0)
    {
        fail("Expected failure due to precondition did not trigger.");
    }
#endif /* IMPLDEFS_CF_DISABLE_ASSERT */
}
END_TEST

START_TEST(test_bit_ops)
{
    uint32_t bitmask;

    bitmask = BIT_3 | BIT_7 | BIT_9 | BIT_14;
    BIT_CLEAR(bitmask, BIT_9);
    fail_unless(bitmask == (BIT_3 | BIT_7 | BIT_14),
                "BIT_CLEAR() on set bit failed");

    BIT_CLEAR(bitmask, BIT_9); /* No change, bit already cleared. */
    fail_unless(bitmask == (BIT_3 | BIT_7 | BIT_14),
                "BIT_CLEAR() on clear bit failed");

    BIT_CLEAR(bitmask, (BIT_7 | BIT_14 | BIT_15)); /* Multibit. */
    fail_unless(bitmask == BIT_3,
                "BIT_CLEAR() with multiple bits failed");

    BIT_SET(bitmask, BIT_8); /* Single bit. */
    fail_unless(bitmask == (BIT_3 | BIT_8),
                "BIT_SET() single bit failed");

    BIT_SET(bitmask, BIT_8); /* Single bit, already set. */
    fail_unless(bitmask == (BIT_3 | BIT_8),
                "BIT_SET() single already set bit failed");

    BIT_SET(bitmask, BIT_3 | BIT_14 | BIT_15); /* Multiple bits. */
    fail_unless(bitmask == (BIT_3 | BIT_8 | BIT_14 | BIT_15),
                "BIT_SET() multiple failed");

    fail_unless(BIT_IS_SET(bitmask, BIT_3), "BIT_IS_SET() single bit failed");
    fail_if(BIT_IS_SET(bitmask, BIT_2), "BIT_IS_SET() clear bit failed");
    fail_unless(BIT_IS_SET(bitmask, (BIT_2 | BIT_14)),
                "BIT_IS_SET() multi bit failed");
}
END_TEST

typedef struct BitSetTestSessionRec
{
    enum { BIT_FLA1 = 1024, BIT_FLA2 = 2048 } flags;
} *BitSetTestSessionPtr_t;

/* Note: these functions may not be static. It affects code generation. */
void framework_testsuite_bitset_sequence(BitSetTestSessionPtr_t test_p);
void framework_testsuite_bitclear_sequence(BitSetTestSessionPtr_t test_p);
void framework_testsuite_bitops_sequence(BitSetTestSessionPtr_t test_p);

typedef void (*BitSetTest_Func)(BitSetTestSessionPtr_t test_p);

static BitSetTest_Func bitset_dispatch_table[3] =
{
    framework_testsuite_bitset_sequence,
    framework_testsuite_bitclear_sequence,
    framework_testsuite_bitops_sequence
};

/* Jump to bit*_sequence according to idx provided. */
static void bitops_dispatch(int idx, BitSetTestSessionPtr_t test_p)
{
    /* This test uses jump table because for produced code, it
       matters if bitset_sequence etc. functions are inlined or not.
       The jump table prevents inlining on most compilers. */

    /* Invoke function chosen by idx. */
    (*bitset_dispatch_table[idx])(test_p);
}

START_TEST(test_bit_ops_combinations)
{
    struct BitSetTestSessionRec testmat = { 0 };

    bitops_dispatch(0, &testmat); /* SET bits */
    fail_if(!BIT_IS_SET(testmat.flags, BIT_FLA1) ||
            !BIT_IS_SET(testmat.flags, BIT_FLA2),
            "Bit ops sequences misoperate");

    bitops_dispatch(1, &testmat); /* CLEAR bits */
    fail_if(testmat.flags,
            "Bit ops sequences misoperate");

    BIT_SET(testmat.flags, BIT_FLA2);
    bitops_dispatch(2, &testmat); /* Set bit, clear bit. */
    fail_unless(BIT_IS_SET(testmat.flags, BIT_FLA1) ||
                !BIT_IS_SET(testmat.flags, BIT_FLA2),
                "Bit ops sequences misoperate");
}
END_TEST

void framework_testsuite_bitset_sequence(BitSetTestSessionPtr_t test_p)
{
    /* Test multiple bitsets in sequence. */
    BIT_SET (test_p->flags, BIT_FLA1);
    BIT_SET (test_p->flags, BIT_FLA2);
}

void framework_testsuite_bitclear_sequence(BitSetTestSessionPtr_t test_p)
{
    /* Test multiple bitsets in sequence. */
    BIT_CLEAR (test_p->flags, BIT_FLA1);
    BIT_CLEAR (test_p->flags, BIT_FLA2);
}

void framework_testsuite_bitops_sequence(BitSetTestSessionPtr_t test_p)
{
    /* Test multiple bitsets in sequence. */
    BIT_SET (test_p->flags, BIT_FLA1);
    BIT_CLEAR (test_p->flags, BIT_FLA2);
}

START_TEST(test_aligned_to)
{
    char *ptr_p;
    fail_unless(ALIGNED_TO(0x55ffee30, 4), "Test alignment four");

    /* Notice this address may not be addressible, we are just interested in
       its alignment. */
    ptr_p = (char *) (uintptr_t) 0xc0e030d0;
    fail_unless(ALIGNED_TO(ptr_p, 16), "Test alignment sixteen");

    /* Notice this address may not be addressible, we are just interested in
       its alignment. */
    ptr_p = (char *) (uintptr_t) 0xc0e030d8;
    fail_if(ALIGNED_TO(ptr_p, 16), "Test misalignment sixteen");
}
END_TEST


START_TEST(test_null)
{
    /* Test null pointer is equivalent to zero. */

    void *ptr_array[2];
    uintptr_t null_value;

    null_value = (uintptr_t) NULL;

    fail_unless(null_value == 0, "NULL != 0");

    c_memset(ptr_array, 0, sizeof(ptr_array));
    fail_unless(ptr_array[0] == NULL,
                "Init to zero didn't produce null pointers");

    fail_unless(ptr_array[1] == NULL,
                "Init to zero didn't produce null pointers");
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

struct test_char_int
{
    char a;
    uint32_t b;
};

START_TEST(test_alignmentof)
{
    /* Tests alignmentof.
       If fails, may fail also due to structure alignment on this platform.
       (char in structs not aligned to minimal padding).
       In this case, you may need to provide additional flags to compiler to
       fix alignment issues. */

    fail_if(alignmentof(char) != 1,
            "alignmentof() or structure padding failure [char]");
    fail_if(alignmentof(int8_t) != 1,
            "alignmentof() or structure padding failure [int8_t]");
    fail_if(alignmentof(uint8_t) != 1,
            "alignmentof() or structure padding failure [uint8_t]");

    /* Test alignment of struct test_char_int vs alignments of types.
       Allow paddings of 0, 1 and 3, as long as size of struct is correct. */

    fail_unless((alignmentof(uint32_t) == 1 &&
                 offsetof(struct test_char_int, b) == 1 &&
                 sizeof(struct test_char_int) == 5) ||
                (alignmentof(uint32_t) == 2 &&
                 offsetof(struct test_char_int, b) == 2 &&
                 sizeof(struct test_char_int) == 6) ||
                (alignmentof(uint32_t) == 4 &&
                 offsetof(struct test_char_int, b) == 4 &&
                 sizeof(struct test_char_int) == 8),
                "Unexpected memory layout for struct test_char_int or "
                "misoperation of alignmentof or offsetof or sizeof.");

}
END_TEST

/* These asserts run at compilation time => cannot be written into a test. */
COMPILE_GLOBAL_ASSERT(sizeof(char) == sizeof(uint8_t));

START_TEST(test_compile_static_assert)
{
    /* Notice this assert generates no code, this test shall be empty. */
    COMPILE_STATIC_ASSERT(sizeof(char) == sizeof(uint8_t));
}
END_TEST

/* Make C compiler assume resources are needed, to omit warnings
   on unused parameters. */
static void testsuite_needed_resource(const void * const Resource_p)
{
    PARAMETER_NOT_USED(Resource_p);
}

START_TEST(test_l_debug)
{
    const char *string = "testing debug printouts";

    testsuite_needed_resource(string);

    /* Do testing via L_DEBUG, include string formatting: */
    L_DEBUG(LF_TEST_L_DEBUG, "%s", string);
}
END_TEST


START_TEST(test_l_trace)
{
    const char *string = "testing debug printouts";

    testsuite_needed_resource(string);

    /* Do testing via L_TRACE, include string formatting: */
    L_DEBUG(LF_TEST_L_TRACE, "%s", string);
}
END_TEST


START_TEST(test_spal_mutex)
{
    SPAL_Result_t Result;
    SPAL_Mutex_t Mutex;
    bool IsLocked;

    Result = SPAL_Mutex_Init(&Mutex);
    ASSERT(Result == SPAL_SUCCESS);

    SPAL_Mutex_Lock(&Mutex);

    IsLocked = SPAL_Mutex_IsLocked(&Mutex);
    ASSERT(IsLocked);

    SPAL_Mutex_UnLock(&Mutex);

    Result = SPAL_Mutex_TryLock(&Mutex);
    ASSERT(Result == 0);

    SPAL_Mutex_UnLock(&Mutex);

    SPAL_Mutex_Destroy(&Mutex);
}
END_TEST

START_TEST(test_spal_semaphore)
{
    SPAL_Result_t Result;
    SPAL_Semaphore_t Semaphore;

#define TRACE(x) L_DEBUG(LF_PROGRESS, "Calling: %s", #x)

    TRACE(SPAL_Semaphore_Init);
    Result = SPAL_Semaphore_Init(&Semaphore, 1);
    ASSERT(Result == SPAL_SUCCESS);

    TRACE(SPAL_Semaphore_Wait);
    SPAL_Semaphore_Wait(&Semaphore);

    TRACE(SPAL_Semaphore_TryWait);
    Result = SPAL_Semaphore_TryWait(&Semaphore);
    ASSERT(Result == SPAL_RESULT_LOCKED);

    TRACE(SPAL_Semaphore_TimedWait);
    Result = SPAL_Semaphore_TimedWait(&Semaphore, 500);
    ASSERT(Result == SPAL_RESULT_TIMEOUT);

    TRACE(SPAL_Semaphore_Post);
    SPAL_Semaphore_Post(&Semaphore);

    TRACE(SPAL_Semaphore_Wait);
    SPAL_Semaphore_Wait(&Semaphore);

    TRACE(SPAL_Semaphore_Destroy);
    SPAL_Semaphore_Destroy(&Semaphore);
    L_DEBUG(LF_PROGRESS, "SPAL semaphore test finished");

#undef TRACE
}
END_TEST

/* This semaphore shall be posted after successful thread start. */
static SPAL_Semaphore_t ThreadStartSemaphore;

static void *ThreadStartFunction(
    void * const Param_p)
{
    uintptr_t Value = (uintptr_t) Param_p;

    if (Value < 100)
    {
        SPAL_Semaphore_Post(&ThreadStartSemaphore);
        SPAL_Thread_Exit((void *) (Value + 1));
    }
    else if (Value > 200)
    {
        SPAL_Thread_Detach(SPAL_Thread_Self());
        SPAL_Semaphore_Post(&ThreadStartSemaphore);
    }
    else
    {
        SPAL_Semaphore_Post(&ThreadStartSemaphore);
    }

    return Param_p;
}


START_TEST(test_spal_thread)
{
    SPAL_Result_t Result;
    SPAL_Thread_t Thread;
    uintptr_t TestValue = 123;
    uintptr_t ReturnValue = 0;

    Result = SPAL_Semaphore_Init(&ThreadStartSemaphore, 0);
    ASSERT(Result == SPAL_SUCCESS);

    Result =
        SPAL_Thread_Create(
            &Thread,
            /* Reserved_p: */ NULL,
            ThreadStartFunction,
            (void *) TestValue);

    ASSERT(Result == SPAL_SUCCESS);
    ASSERT(Thread != SPAL_Thread_Self());
    SPAL_Semaphore_Wait(&ThreadStartSemaphore); /* Wait thread started. */

    Result =
        SPAL_Thread_Join(
            Thread,
            (void *) &ReturnValue);

    ASSERT(Result == SPAL_SUCCESS);
    ASSERT(ReturnValue == TestValue);

    Result =
        SPAL_Thread_Create(
            &Thread,
            /* Reserved_p: */ NULL,
            ThreadStartFunction,
            (void *) 90);

    ASSERT(Result == SPAL_SUCCESS);
    ASSERT(Thread != SPAL_Thread_Self());
    SPAL_Semaphore_Wait(&ThreadStartSemaphore); /* Wait thread started. */

    Result =
        SPAL_Thread_Join(
            Thread,
            (void *) &ReturnValue);

    ASSERT(Result == SPAL_SUCCESS);
    ASSERT(ReturnValue == 91);

    Result =
        SPAL_Thread_Create(
            &Thread,
            /* Reserved_p: */ NULL,
            ThreadStartFunction,
            (void *) 201);

    ASSERT(Result == SPAL_SUCCESS);
    ASSERT(Thread != SPAL_Thread_Self());
    SPAL_Semaphore_Wait(&ThreadStartSemaphore); /* Wait thread started. */

    SPAL_Semaphore_Destroy(&ThreadStartSemaphore);
}
END_TEST

START_TEST(test_spal_sleep)
{
    SPAL_SleepMS(250);
}
END_TEST

START_TEST(test_spal_memory_alloc_and_free)
{
    int i;
    void *m[8];

    for(i = 0; i < 8; i++)
    {
        m[i] = SPAL_Memory_Alloc(i + 1);
        fail_if(m[i] == NULL, "SPAL_Memory_Alloc found no free memory");
        c_memset(m[i], 0xee, i + 1);
    }

    /* Free allocated memory. */
    for(i = 0; i < 8; i++)
    {
        SPAL_Memory_Free(m[i]);
    }
}
END_TEST

START_TEST(test_spal_memory_calloc_and_free)
{
    int i;
    int l;
    uint32_t *m[8];

    /* Calloc memory */
    for(i = 0; i < 8; i++)
    {
        m[i] = SPAL_Memory_Calloc(i + 1, sizeof(uint32_t));
        fail_if(m[i] == NULL, "SPAL_Memory_Calloc found no free memory");
        for(l = 0; l <= i; l++)
        {
            fail_if(m[i][l] != 0,
                    "SPAL_Memory_Calloc memory misinitialized.");
        }
    }

    /* Free allocated memory. */
    for(i = 0; i < 8; i++)
    {
        SPAL_Memory_Free(m[i]);
    }
}
END_TEST

START_TEST(test_spal_memory_realloc_and_free)
{
    unsigned char *ptr1_p;
    void *ptr2_p;
    int i;

    ptr1_p = SPAL_Memory_Alloc(1024);
    ptr2_p = SPAL_Memory_Alloc(1024);
    fail_if(ptr1_p == NULL, "Memory allocation unexpectably failed.");
    fail_if(ptr2_p == NULL, "Memory allocation unexpectably failed.");

    c_memset(ptr1_p, 0xaa, 1024);
    c_memset(ptr2_p, 0xab, 1024);

    for(i = 0; i < 1024; i++)
    {
        fail_if(ptr1_p[i] != 0xaa,
                "Memory not initialized correctly.");
    }

    ptr1_p = SPAL_Memory_ReAlloc(ptr1_p, 4096);
    fail_if(ptr1_p == NULL, "Memory reallocation unexpectably failed.");
    for(i = 0; i < 1024; i++)
    {
        fail_if(ptr1_p[i] != 0xaa,
                "SPAL_Memory_ReAlloc didn't copy the memory correctly.");
    }

    ptr1_p = SPAL_Memory_ReAlloc(ptr1_p, 512);
    fail_if(ptr1_p == NULL, "Memory reallocation unexpectably failed.");
    for(i = 0; i < 512; i++)
    {
        fail_if(ptr1_p[i] != 0xaa,
                "SPAL_Memory_ReAlloc didn't copy the memory correctly.");
    }

    /* Finish by freeing the allocated memory. */
    SPAL_Memory_Free(ptr1_p);
    SPAL_Memory_Free(ptr2_p);
}
END_TEST

#ifndef CFG_ENABLE_COVERAGE
START_TEST(test_spal_memory_calloc_misusage)
{
#ifdef IMPLDEFS_CF_DISABLE_PRECONDITION
    unsupported_quick("Calloc misusage cannot be tested "
                      "when preconditions are not enabled.");
#else /* IMPLDEFS_CF_DISABLE_PRECONDITION */
    unsigned char *ptr_p;

    SFZUTF_EXPECT_PRECONDITION;
    ptr_p = SPAL_Memory_Calloc(65537, 65537);
    PARAMETER_NOT_USED(ptr_p);
    fail("Expected failure due to precondition did not trigger.");
#endif /* IMPLDEFS_CF_DISABLE_PRECONDITION */
}
END_TEST
#endif

START_TEST(test_spal_memory_calloc_large)
{
    unsigned char *ptr1_p;
    unsigned char *ptr2_p;
    unsigned char *ptr_p;

    ptr1_p = SPAL_Memory_Calloc(10000, 2);
    fail_if(ptr1_p == NULL, "Largish (20k) Calloc failed.");
    ptr2_p = SPAL_Memory_Calloc(10000, 2);
    fail_if(ptr2_p == NULL, "Largish (20k) Calloc failed.");

    SPAL_Memory_Free(ptr1_p);
    SPAL_Memory_Free(ptr2_p);

    ptr_p = SPAL_Memory_Calloc(65537, 2);
    fail_if(ptr_p == NULL, "Large (128k) Calloc failed.");
    SPAL_Memory_Free(ptr_p);
}
END_TEST

#ifndef CFG_ENABLE_COVERAGE
START_TEST(test_spal_memory_calloc_misusage_2)
{
#ifdef IMPLDEFS_CF_DISABLE_PRECONDITION
    unsupported_quick("Calloc misusage cannot be tested "
                      "when preconditions are not enabled.");
#else /* IMPLDEFS_CF_DISABLE_PRECONDITION */
    unsigned char *ptr_p;

    SFZUTF_EXPECT_PRECONDITION;
    ptr_p = SPAL_Memory_Calloc(65537 * 2, 32768);
    PARAMETER_NOT_USED(ptr_p);
    fail("Expected failure due to precondition did not trigger.");
#endif /* IMPLDEFS_CF_DISABLE_PRECONDITION */
}
END_TEST
#endif

#ifndef CFG_ENABLE_COVERAGE
START_TEST(test_spal_memory_calloc_misusage_3)
{
#ifdef IMPLDEFS_CF_DISABLE_PRECONDITION
    unsupported_quick("Calloc misusage cannot be tested "
                      "when preconditions are not enabled.");
#else /* IMPLDEFS_CF_DISABLE_PRECONDITION */
    unsigned char *ptr_p;

    SFZUTF_EXPECT_PRECONDITION;
    ptr_p = SPAL_Memory_Calloc(32768, 65537 * 3);
    PARAMETER_NOT_USED(ptr_p);
    fail("Expected failure due to precondition did not trigger.");
#endif /* IMPLDEFS_CF_DISABLE_PRECONDITION */
}
END_TEST
#endif


/* Small helper for non-constant filling.
   Non constant fill is bit better in detecting errors in
   mem* implementations than constant fill (c_memset). */
static void
test_helper_memset_incr(void *ptr, int initial_char, size_t len)
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
   Notice that some tests are of complexity this parameter ^4.
   On select fast platforms, the amount of testing is increased.
*/
#if defined(__x86_64__)
#define TEST_C_MEMXXX_RANGE 64
#else
#define TEST_C_MEMXXX_RANGE 16
#endif

START_TEST(test_c_memset)
{
    uint8_t arr[TEST_C_MEMXXX_RANGE * 3];
    uint8_t arr_bak[TEST_C_MEMXXX_RANGE * 3];
    int c;
    int dst_idx;
    int idx;
    int length;

    /* Try all constant values. */
    for(c = 0; c < 256; c++)
    {
        c_memset(arr, c, sizeof(arr));

        for(dst_idx = 0; dst_idx < (int) sizeof(arr); dst_idx++)
        {
            fail_if(arr[dst_idx] != c,
                    "c_memset failed to fill array with specified value");
        }
    }

    /* Test over dst_idx and length */
    for(dst_idx = 0; dst_idx < TEST_C_MEMXXX_RANGE; dst_idx++)
    {
        for(length = 0; length < TEST_C_MEMXXX_RANGE; length++)
        {
            /* Initialize */
            test_helper_memset_incr(arr, 0x3, sizeof(arr));
            test_helper_memset_incr(arr_bak, 0x3, sizeof(arr));

            c_memset(arr + dst_idx, 0xee, length);

            /* Validate target */
            for(idx = 0; idx < (int) sizeof(arr); idx++)
            {
                if (idx < dst_idx ||
                        idx >= dst_idx + length)
                {
                    /* Outsize modified range. */
                    fail_if(arr[idx] != arr_bak[idx],
                            "c_memset over/underflow.");
                }
                else
                {
                    /* Inside modified range. */
                    fail_if(arr[idx] != 0xee,
                            "c_memset copied area contents unexpected");
                }
            }
        }
    }

}
END_TEST


static
void test_helper_c_xxxcmp(int variant,
                          uint8_t start_pad_byte, uint8_t end_pad_byte)
{
    /* Tests c_memcmp,
       test is similar to c_memcmp but ensure no alteration happens.
       tests only "memeq" capabilities. */

    uint8_t src[TEST_C_MEMXXX_RANGE * 3];
    uint8_t dst[TEST_C_MEMXXX_RANGE * 3];
    uint8_t src_bak[TEST_C_MEMXXX_RANGE * 3];
    int src_idx, dst_idx, length, idx;
    bool same_idx;

    for(src_idx = 0; src_idx < TEST_C_MEMXXX_RANGE; src_idx++)
    {
        for(length = 0; length < TEST_C_MEMXXX_RANGE; length++)
        {
            for(dst_idx = 0; dst_idx < TEST_C_MEMXXX_RANGE; dst_idx++)
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
                    fail_if((!c_memcmp(src + src_idx, dst + dst_idx, length))
                            != (same_idx || length == 0),
                            "c_memcmp unexpected result.");
                }
                else if (variant == 1)
                {
                    int res;
                    uint8_t ctmp;

                    ctmp = src[src_idx + length];
                    src[src_idx + length] = end_pad_byte;

                    res = c_strcmp((const void *) (src + src_idx),
                                   (const void *) (dst + dst_idx));

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
                for(idx = 0; idx < (int) sizeof(dst); idx++)
                {
                    fail_if(src[idx] != src_bak[idx],
                            "c_memcmp modified memory.");

                    fail_if(dst[idx] != src[idx],
                            "c_memcmp modified memory.");
                }

            }

            /* Check that c_memcmp works for same address */
            fail_if(c_memcmp(src + src_idx, src + src_idx, length) != 0,
                    "Comparing itself with c_memcmp didn't return 0");
        }
    }
}


START_TEST(test_c_memcmp)
{
    test_helper_c_xxxcmp(0, 0xee, 0xef);

    /* Check ordering. */
    fail_unless(c_memcmp("ABC", "abc", 3) < 0,
                "Ordering of characters");
    fail_unless(c_memcmp("ABCDEFGHIJ", "0123456789", 10) > 0,
                "Ordering of characters/numbers");
    fail_unless(c_memcmp("ABCDEFGHIJ", "ABCDEFGHIJK", 11) < 0,
                "Ordering of strings with different length");
    fail_unless(c_memcmp("abcd", "abc\277", 4) < 0,
                "c_memcmp between ordinary characters and high-ASCII");
    fail_unless(c_memcmp("a\000bc", "a\000bd", 4) < 0,
                "c_memcmp with ascii zero");
}
END_TEST


START_TEST(test_c_strcmp)
{
    test_helper_c_xxxcmp(1, 0xee, 0);

    /* Check ordering. */
    fail_unless(c_strcmp("ABC", "abc") < 0,
                "Ordering of characters");
    fail_unless(c_strcmp("ABCDEFGHIJ", "0123456789") > 0,
                "Ordering of characters/numbers");
    fail_unless(c_strcmp("ABCDEFGHIJ", "ABCDEFGHIJK") < 0,
                "Ordering of strings with different length");
    fail_unless(c_strcmp("abcd", "abc\277") < 0,
                "c_strcmp between ordinary characters and high-ASCII");
    fail_unless(c_strcmp("a\000bc", "a\000bd") == 0,
                "c_strcmp with ascii zero");
}
END_TEST


START_TEST(test_c_strncmp)
{
    test_helper_c_xxxcmp(1, 0xee, 0);

    /* Check ordering. */
    fail_unless(c_strncmp("ABC", "abc", 3) < 0,
                "Ordering of characters");
    fail_unless(c_strncmp("ABCDEFGHIJ", "0123456789", 5) > 0,
                "Ordering of characters/numbers");
    fail_unless(c_strncmp("ABCDEFGHIJ", "ABCDEFGHIJK", 15) < 0,
                "Compare strings with different length (s1 < s2)");
    fail_unless(c_strncmp("ABCDEFGHIJ", "ABCDEFGHIJK", 10) == 0,
                "Compare first part of strings with different length");
    fail_unless(c_strncmp("ABCDEFGHIJ", "ABCDEFGHIJKLMNOP", 9) == 0,
                "Compare part of strings with different length");
    fail_unless(c_strncmp("ABCDEFGHIJ", "ABCDEF", 5) == 0,
                "Compare part of strings with different length");
    fail_unless(c_strncmp("ABXDEFGHIJ", "ABCDEFGHIJ", 2) == 0,
                "Stop compare prior diff in first string");
    fail_unless(c_strncmp("ABCDEFGHIJ", "ABCXEFGHIJ", 3) == 0,
                "Stop compare prior diff in first string");
    fail_unless(c_strncmp("ABC", "ABC", 4) == 0,
                "Exclude terminating zero");
    fail_unless(c_strncmp("abcd", "abc\277", 10) < 0,
                "c_strcmp between ordinary characters and high-ASCII");
    fail_unless(c_strncmp("a\000bc", "a\000bd", 10) == 0,
                "c_strcmp with ascii zero");
    fail_unless(c_strncmp("", "a\000bd", 0) == 0,
                "c_strcmp with zero length");
}
END_TEST


static
void test_helper_c_xxxcpy(int variant)
{
    /* Tests c_memcpy and c_memmove, */

    uint8_t src[TEST_C_MEMXXX_RANGE * 3];
    uint8_t dst[TEST_C_MEMXXX_RANGE * 3];
    uint8_t src_bak[TEST_C_MEMXXX_RANGE * 3];
    uint8_t dst_bak[TEST_C_MEMXXX_RANGE * 3];
    int src_idx, dst_idx, length, idx;

    for(src_idx = 0; src_idx < TEST_C_MEMXXX_RANGE; src_idx++)
    {
        for(dst_idx = 0; dst_idx < TEST_C_MEMXXX_RANGE; dst_idx++)
        {
            for(length = 0; length < TEST_C_MEMXXX_RANGE; length++)
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
                else if (variant == 2)
                {
                    if (length > 0)
                    {
                        /* Mark source arrays with end marker. */
                        src[src_idx + length - 1] = 0;
                        src_bak[src_idx + length - 1] = 0;
                        c_strcpy((char *) (dst + dst_idx),
                                 (const char *) (src + src_idx));
                    }
                }
                else if (variant == 3)
                {
                    if (length > 0)
                    {
                        /* Mark source arrays with end marker. */
                        src[src_idx + length - 1] = 0;
                        src_bak[src_idx + length - 1] = 0;

                        /* Mark dst with target spot to cat. */
                        dst[dst_idx] = 0;
                        c_strcat((char *) dst,
                                 (const char *) (src + src_idx));
                    }
                }
                else if (variant == 4)
                {
                    /* c_strncpy without endmarker */
                    c_strncpy((char *) dst + dst_idx,
                              (const char *) src + src_idx, length);
                }
                else if (variant == 5)
                {
                    /* c_strncpy with endmarker */
                    if (length > 0)
                    {
                        /* Mark source arrays with end marker. */
                        src[src_idx + length - 1] = 0;
                        src_bak[src_idx + length - 1] = 0;
                        c_strncpy((char *) dst + dst_idx,
                                  (const char *) src + src_idx, length);
                    }
                }
                else if (variant == 6)
                {
                    /* c_strncpy with endmarker, length + 1 */
                    if (length > 0)
                    {
                        /* Mark source arrays with end marker. */
                        src[src_idx + length - 1] = 0;
                        src_bak[src_idx + length - 1] = 0;
                        c_strncpy((char *) dst + dst_idx,
                                  (const char *) src + src_idx, length + 1);

                        fail_if(dst[dst_idx + length] != 0,
                                "c_strncpy did not clear extra buffer area");

                        dst[dst_idx + length] = dst_bak[dst_idx + length];
                    }
                }
                else if (variant == 7)
                {
                    /* c_memcpy with c_strlen tested aside. */
                    if (length > 0)
                    {
                        /* Mark source arrays with end marker. */
                        src[src_idx + length - 1] = 0;
                        src_bak[src_idx + length - 1] = 0;
                        c_memcpy(dst + dst_idx, src + src_idx, length);

                        fail_if(c_strlen((char *) src + src_idx) !=
                                (size_t) length - 1,
                                "c_strlen failed");

                        fail_if(c_strlen((char *) dst + dst_idx) !=
                                (size_t) length - 1,
                                "c_strlen or c_memcpy failed");
                    }
                }

                /* Check results, with c_memcmp. */
                fail_if(c_memcmp(src, src_bak, sizeof(src)) != 0,
                        "c_memcpy/c_memmove altered source data");

                if (length == 0)
                {
                    fail_if(c_memcmp(dst_bak, dst, sizeof(dst)) != 0,
                            "Zero length c_memcpy/c_memmove altered dst");
                }

                /* Validate target */
                for(idx = 0; idx < (int) sizeof(dst); idx++)
                {
                    if (idx < dst_idx ||
                            idx >= dst_idx + length)
                    {
                        /* Outsize modified range. */
                        fail_if(dst[idx] != dst_bak[idx],
                                "c_memcpy/c_memmove over/underflow.");
                    }
                    else
                    {
                        /* Inside modified range. */
                        fail_if(dst[idx] != src[idx - dst_idx + src_idx],
                                "c_memcpy/c_memmove copied area "
                                "contents unexpected");
                    }
                }
            }
        }
    }
}


START_TEST(test_c_memcpy)
{
    test_helper_c_xxxcpy(0);
}
END_TEST


START_TEST(test_c_memmove)
{
    uint8_t arr[TEST_C_MEMXXX_RANGE * 3];
    uint8_t arr_bak[TEST_C_MEMXXX_RANGE * 3];
    int src_idx, dst_idx, length, idx;

    /* Perform the same tests than for c_memcpy. */
    test_helper_c_xxxcpy(1);

    /* Continue with c_memmove specific testing. */
    for(src_idx = 0; src_idx < TEST_C_MEMXXX_RANGE; src_idx++)
    {
        for(dst_idx = 0; dst_idx < TEST_C_MEMXXX_RANGE; dst_idx++)
        {
            for(length = 0; length < TEST_C_MEMXXX_RANGE; length++)
            {
                /* Initialize */
                test_helper_memset_incr(arr, 0x1, sizeof(arr));
                test_helper_memset_incr(arr_bak, 0x1, sizeof(arr_bak));

                /* Actual c_memmove function. */
                c_memmove(arr + dst_idx, arr + src_idx, length);

                /* Validate target */
                for(idx = 0; idx < (int) sizeof(arr); idx++)
                {
                    if (idx < dst_idx ||
                            idx >= dst_idx + length)
                    {
                        /* Outsize modified range. */
                        fail_if(arr[idx] != arr_bak[idx],
                                "c_memmove over/underflow.");
                    }
                    else
                    {
                        /* Inside modified range. */
                        fail_if(arr[idx] !=
                                arr_bak[idx - dst_idx + src_idx],
                                "c_memmove copied area "
                                "contents unexpected");
                    }
                }
            }
        }
    }
}
END_TEST


START_TEST(test_c_strcpy)
{
    test_helper_c_xxxcpy(2);
}
END_TEST

START_TEST(test_c_strncpy)
{
    test_helper_c_xxxcpy(4);
    test_helper_c_xxxcpy(5);
    test_helper_c_xxxcpy(6);
}
END_TEST

START_TEST(test_c_strcat)
{
    test_helper_c_xxxcpy(3);
}
END_TEST


START_TEST(test_c_strlen)
{
    test_helper_c_xxxcpy(7);
}
END_TEST


START_TEST(test_c_strchr)
{
    /* And c_strstr for single character inputs.
       c_memchr, for characters 1-255. */
    uint8_t arr[256];
    uint8_t needle[2];
    int idx;

    /* array contains all characters, ending with 0x00. */
    test_helper_memset_incr(arr, 0x01, sizeof(arr));

    /* Find each character. */
    for(idx = 1; idx < 256; idx++)
    {
        const uint8_t *target;
        const uint8_t *target2;
        const uint8_t *target3;

        target = (const void *) c_strchr((const void *) arr, idx);
        fail_if(target == NULL, "c_strchr failed to find character");

        fail_if(target - arr + 1 != idx,
                "c_strchr found wrong position");

        needle[0] = (uint8_t)idx;
        needle[1] = 0;

        target2 = (const void *) c_strstr((const void *) arr,
                                          (const void *) needle);
        fail_if(target != target2, "c_strstr failed.");

        target3 = (const void *) c_memchr((const void *) arr, idx, 256);
        fail_if(target != target3, "c_memchr failed to find character");
    }

    /* Start looking at array just after characters position. */
    for(idx = 1; idx < 256; idx++)
    {
        const uint8_t *target;
        const uint8_t *target2;
        const uint8_t *target3;

        target = (const void *) c_strchr((const void *) (arr + idx), idx);
        fail_if(target != NULL, "c_strchr found character it cannot find");

        needle[0] = (uint8_t)idx;
        needle[1] = 0;

        target2 = (const void *) c_strstr((const void *) (arr + idx),
                                          (const void *) needle);
        fail_if(target != target2, "c_strstr failed.");

        target3 = (const void *) c_memchr((const void *) (arr + idx), idx,
                                          256 - idx);
        fail_if(target != target3, "c_memchr failed.");
    }
}
END_TEST


START_TEST(test_c_strstr)
{
    /* test_c_strchr covers this function well for single character inputs. */
    /* try two character inputs. */

    uint8_t arr[256];
    uint8_t needle[3];
    int idx;
    int idx2;

    /* array contains all characters, ending with 0x00. */
    test_helper_memset_incr(arr, 0x01, sizeof(arr));

    /* Find each character pair. */
    for(idx = 1; idx < 255; idx++)
    {
        for(idx2 = 1; idx2 < 256; idx2++)
        {
            const uint8_t *target;

            needle[0] = (uint8_t)idx;
            needle[1] = (uint8_t)idx2;
            needle[2] = 0;

            target = (const void *) c_strstr((const void *) arr,
                                             (const void *) needle);

            if (target == NULL)
            {
                fail_if(idx2 == idx + 1,
                        "Unexpected c_strstr failure");
            }
            else
            {
                fail_if(idx2 != idx + 1,
                        "Unexpected c_strstr success");

                fail_if(target - arr + 1 != idx,
                        "c_strstr found wrong position");
            }
        }
    }

    /* Longer needle. */
    fail_if(c_strstr("abc", "abcd") != NULL,
            "Test with longer need than input string found something");
}
END_TEST

START_TEST(test_c_strtol)
{
    const char *str_empty = "";
    const char *str_a = "A";
    char *end_ptr;

    /* Empty string */
    (void) c_strtol(str_empty, &end_ptr, 10);
    fail_unless(end_ptr == str_empty, "c_strtol end_ptr wrong");

    /* Single A */
    (void) c_strtol(str_a, &end_ptr, 10);
    fail_unless(end_ptr == str_a, "c_strtol end_ptr wrong");

    (void) c_strtol(str_a, &end_ptr, 8);
    fail_unless(end_ptr == str_a, "c_strtol end_ptr wrong");

    fail_unless(c_strtol(str_a, &end_ptr, 16) == 10,
                "c_strtol unexpectedly failed");
    fail_unless(end_ptr == str_a + 1, "c_strtol end_ptr wrong");

    /* Ensure NULL is allowed for end_ptr. */
    (void) c_strtol(str_empty, NULL, 10);

    fail_unless(c_strtol("10", NULL, 10) == 10, "c_strtol failed");
    fail_unless(c_strtol("65537", NULL, 10) == 65537, "c_strtol failed");
    fail_unless(c_strtol("2147483647", NULL, 10) == 2147483647,
                "c_strtol failed");
    fail_unless(c_strtol(" 1", NULL, 10) == 1, "c_strtol failed padding");

    /* Negative numbers */
    fail_unless(c_strtol("-1", NULL, 10) == -1,
                "c_strtol failed negative numbers");
    fail_unless(c_strtol("-2147483648", NULL, 10) == ((long) - 2147483647) - 1,
                "c_strtol failed negative numbers");

    /* Determine base */
    fail_unless(c_strtol("22", NULL, 0) == 22,
                "c_strtol detect decimal base");
    fail_unless(c_strtol("0x2a", NULL, 0) == 2 * 16 + 10,
                "c_strtol detect hex base");
    fail_unless(c_strtol("022", NULL, 0) == 2 * 8 + 2,
                "c_strtol detect octal base");

    /* Initial zeroes */
    fail_unless(c_strtol("022", NULL, 10) == 22,
                "c_strtol failed initial zeroes");
    fail_unless(c_strtol("0022", NULL, 10) == 22,
                "c_strtol failed initial zeroes");
    fail_unless(c_strtol("022", NULL, 16) == 2 * 16 + 2,
                "c_strtol failed initial zeroes");
    fail_unless(c_strtol("0022", NULL, 16) == 2 * 16 + 2,
                "c_strtol failed initial zeroes");
    fail_unless(c_strtol("022", NULL, 8) == 2 * 8 + 2,
                "c_strtol failed initial zeroes");
    fail_unless(c_strtol("0022", NULL, 8) == 2 * 8 + 2,
                "c_strtol failed initial zeroes");

    /* hex prefix */
    fail_unless(c_strtol("0x22", NULL, 16) == 2 * 16 + 2,
                "c_strtol failed hex prefix");
    fail_unless(c_strtol("0X022", NULL, 16) == 2 * 16 + 2,
                "c_strtol failed hex prefix");
}
END_TEST

/* Let's define EOF for these tests if not yet defined. */
#ifndef EOF
#define EOF (-1)
#endif /* EOF */

START_TEST(test_c_tolower)
{
    int i;
    int chr;
    int chr_out;

    for(i = 0; i < 256 + 1; i++)
    {
        chr = i;
        if (chr == 256)
        {
            chr = EOF;
        }

        chr_out = c_tolower(chr);

        fail_if((chr < 'A' || chr > 'Z') &&
                chr_out != chr,
                "c_tolower affected non uppercase character");

        fail_if((chr >= 'A' && chr <= 'Z') &&
                chr_out != chr + 32,
                "c_tolower misconverted a character");
    }
}
END_TEST

START_TEST(test_c_toupper)
{
    int i;
    int chr;
    int chr_out;

    for(i = 0; i < 256 + 1; i++)
    {
        chr = i;
        if (chr == 256)
        {
            chr = EOF;
        }

        chr_out = c_toupper(chr);

        fail_if((chr < 'a' || chr > 'z') &&
                chr_out != chr,
                "c_toupper affected non uppercase character");

        fail_if((chr >= 'a' && chr <= 'z') &&
                chr_out != chr - 32,
                "c_toupper misconverted a character");
    }
}
END_TEST


START_TEST(test_c_memchr)
{
    /* test_c_strchr tests most c_memchr cases, already. */

    const char * str = "a\000bca";

    /* Check c_memchr zero handling, ie. that c_memchr is
       not handling zero character specially. */
    fail_unless(c_memchr(str, 'a', 5) == (void *) str,
                "c_memchr failed");
    fail_unless(c_memchr(str, '\000', 5) == (void *) (str + 1),
                "c_memchr failed");
    fail_unless(c_memchr(str, 'b', 5) == (void *) (str + 2),
                "c_memchr failed");
    fail_unless(c_memchr(str, 'c', 5) == (void *) (str + 3),
                "c_memchr failed");
    fail_unless(c_memchr(str, 'c', 3) == NULL,
                "c_memchr failed");
}
END_TEST

void
build_suite(void)
{
    sfzutf_suite_create("Framework_Test Suite");

    sfzutf_tcase_create("PUBDEFS_Tests");
    sfzutf_test_add(test_ints);
    sfzutf_test_add(test_uints);
    sfzutf_test_add(test_bool);
    sfzutf_test_add(test_ptr_size);

    sfzutf_tcase_create("IMPLDEFS_Tests");
    sfzutf_test_add(test_min);
    sfzutf_test_add(test_max);
    sfzutf_test_add(test_bit);
    sfzutf_test_add(test_bit_ops);
    sfzutf_test_add(test_bit_ops_combinations);
    sfzutf_test_add(test_aligned_to);
    sfzutf_test_add(test_null);
    sfzutf_test_add(test_offsetof);
    sfzutf_test_add(test_alignmentof);
    sfzutf_test_add(test_compile_static_assert);
    sfzutf_loop_test_add(test_precondition, 0, 2);
    sfzutf_loop_test_add(test_postcondition, 0, 2);
    sfzutf_loop_test_add(test_assertion, 0, 2);
    sfzutf_test_add(test_l_debug);
    sfzutf_test_add(test_l_trace);

    sfzutf_tcase_create("CLIB_Tests");
    sfzutf_test_add(test_c_memset);
    sfzutf_test_add(test_c_memcmp);
    sfzutf_test_add(test_c_memcpy);
    sfzutf_test_add(test_c_memmove);
    sfzutf_test_add(test_c_strcmp);
    sfzutf_test_add(test_c_strncmp);
    sfzutf_test_add(test_c_strcpy);
    sfzutf_test_add(test_c_strcat);
    sfzutf_test_add(test_c_strncpy);
    sfzutf_test_add(test_c_strlen);
    sfzutf_test_add(test_c_strchr);
    sfzutf_test_add(test_c_strstr);
    sfzutf_test_add(test_c_strtol);
    sfzutf_test_add(test_c_tolower);
    sfzutf_test_add(test_c_toupper);
    sfzutf_test_add(test_c_memchr);

    sfzutf_tcase_create("SPAL_Mutex_Tests");
    sfzutf_test_add(test_spal_mutex);

    sfzutf_tcase_create("SPAL_Semaphore_Tests");
    sfzutf_test_add(test_spal_semaphore);

    sfzutf_tcase_create("SPAL_Thread_Tests");
    sfzutf_test_add(test_spal_thread);

    sfzutf_tcase_create("SPAL_Sleep_Tests");
    sfzutf_test_add(test_spal_sleep);

    sfzutf_tcase_create("SPAL_Memory_Tests");
    sfzutf_test_add(test_spal_memory_alloc_and_free);
    sfzutf_test_add(test_spal_memory_realloc_and_free);
    sfzutf_test_add(test_spal_memory_calloc_and_free);
    sfzutf_test_add(test_spal_memory_calloc_large);
#ifndef CFG_ENABLE_COVERAGE
    // avoid aborting the test suite during coverage analysis
    sfzutf_test_add(test_spal_memory_calloc_misusage);
    sfzutf_test_add(test_spal_memory_calloc_misusage_2);
    sfzutf_test_add(test_spal_memory_calloc_misusage_3);
#endif
}

/* end of file framework_testsuite.c */
