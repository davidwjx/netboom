/* nist-health-test.c
 *
 * Description: Functionality for testing NIST DRBG health during its use.
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

#include "implementation_defs.h"
#include "c_lib.h"
#include "nist-sp-800-90.h"

#define SSHDIST_CRYPT_NIST_SP_800_90
#ifdef SSHDIST_CRYPT_NIST_SP_800_90

#define DRBG_TEST_BUFFER_SIZE 64
#define DRBG_TEST_SECURITY_STRENGTH 128
#define DRBG_TEST_ENTROPY_SIZE 32
#define PSEUDORANDOM_BUFFER_LEN 16

/* ********************** Test vectors ****************************** */

typedef struct SshDrbgKnownAnswerTestRec
{
    rngsize_t entropy_len;
    rngsize_t nonce_len;
    rngsize_t personalization_string_len;
    rngsize_t additional_input_len;
    unsigned char entropy_input[DRBG_TEST_BUFFER_SIZE];
    unsigned char nonce[DRBG_TEST_BUFFER_SIZE];
    unsigned char personalization_string[DRBG_TEST_BUFFER_SIZE];
    unsigned char additional_input[DRBG_TEST_BUFFER_SIZE];
    unsigned char entropy_input_reseed[DRBG_TEST_BUFFER_SIZE];
    unsigned char additional_input_reseed[DRBG_TEST_BUFFER_SIZE];
    unsigned char additional_input_final[DRBG_TEST_BUFFER_SIZE];
    unsigned char returned_bits[DRBG_TEST_BUFFER_SIZE];
} SshDrbgKnownAnswerTestStruct;

/* Taken from The NIST SP 800-90 Deterministic Random Bit Generator
   Validation System (DRBGVS) */
static const SshDrbgKnownAnswerTestStruct drbg_test_cases[] =
{
    {
        32,
        8,
        0,
        0,
        {
            0x42, 0x0e, 0xdb, 0xaf, 0xf7, 0x87, 0xfd, 0xbd,
            0x72, 0x9e, 0x12, 0xc2, 0xf3, 0xcf, 0xc0, 0xec,
            0x67, 0x04, 0xde, 0x59, 0xbf, 0x28, 0xed, 0x43,
            0x8b, 0xf0, 0xd8, 0x6d, 0xdd, 0xe7, 0xeb, 0xcc
        },
        {0xbe, 0x29, 0x3b, 0x97, 0x28, 0x94, 0x53, 0x3b},
        {0x00},
        {0x00},
        {
            0xa8, 0x21, 0xc3, 0x4b, 0x75, 0x05, 0x29, 0x1f,
            0x80, 0x34, 0x1e, 0x37, 0xf9, 0x30, 0x45, 0x16,
            0x59, 0x09, 0x15, 0x50, 0xbe, 0xf0, 0x4c, 0xb6,
            0x8a, 0x01, 0xb1, 0xbe, 0x39, 0x4b, 0x10, 0x37
        },
        {0x00},
        {0x00},
        {
            0x26, 0x3c, 0x1c, 0xf3, 0xfd, 0x8c, 0x0b, 0xcb,
            0x1e, 0xd7, 0x54, 0xce, 0x10, 0xcf, 0xc2, 0xfc
        }
    },
    {
        32,
        8,
        0,
        32,
        {
            0x55, 0xd7, 0xac, 0xe6, 0x86, 0xa5, 0xdd, 0x1a,
            0x90, 0x03, 0x08, 0xcf, 0x61, 0x5d, 0x03, 0x4f,
            0xaf, 0xda, 0x66, 0x4d, 0xbb, 0xcc, 0x68, 0xf1,
            0x46, 0x55, 0xf5, 0x27, 0xd2, 0x5d, 0x13, 0x28
        },
        {0xed, 0xf3, 0xb6, 0xfc, 0xc1, 0xf1, 0xd2, 0x8e},
        {0x00},
        {
            0xe0, 0xec, 0x72, 0x44, 0x6b, 0x74, 0xc8, 0x2d,
            0xdb, 0x5d, 0xfc, 0x77, 0x47, 0x06, 0x40, 0x4d,
            0x71, 0xe7, 0x65, 0xa2, 0x82, 0x04, 0xba, 0x42,
            0x07, 0x93, 0x29, 0x1b, 0x2b, 0xea, 0x8e, 0xc6
        },
        {
            0x64, 0x1d, 0x51, 0xb8, 0x13, 0x94, 0x03, 0x2c,
            0x71, 0x08, 0x6c, 0x45, 0xc0, 0x1f, 0xf7, 0x17,
            0x73, 0xc2, 0xfd, 0x6b, 0x6b, 0x0c, 0x1b, 0x48,
            0x78, 0x71, 0xcf, 0xbe, 0x0f, 0x32, 0x53, 0xf4
        },
        {
            0x0b, 0x36, 0x4a, 0x2b, 0x1c, 0x9d, 0xc3, 0x3a,
            0x53, 0x70, 0x63, 0x06, 0xb2, 0x68, 0x8f, 0x54,
            0x1c, 0xf5, 0xc9, 0x89, 0x97, 0x24, 0x06, 0x37,
            0xbf, 0xd2, 0x4e, 0xed, 0x8c, 0x12, 0x93, 0x3e
        },
        {
            0x3f, 0xf9, 0x57, 0xdb, 0x57, 0x14, 0x1f, 0xbd,
            0xbf, 0xe7, 0x0e, 0x8e, 0x84, 0xd4, 0xa9, 0xb0,
            0xab, 0x93, 0x01, 0xae, 0x29, 0xe2, 0xd4, 0x83,
            0x27, 0x6f, 0xe0, 0x2b, 0x76, 0x05, 0x8a, 0x6a
        },
        {
            0x7b, 0x43, 0x97, 0x82, 0x0e, 0x74, 0xdc, 0x2b,
            0xe0, 0x3b, 0x61, 0xcc, 0x6f, 0xfa, 0xf8, 0x00
        }
    },
    {
        32,
        8,
        32,
        0,
        {
            0xb3, 0x04, 0xf8, 0x32, 0x6b, 0x4f, 0xab, 0xe4,
            0x67, 0xa8, 0x85, 0x42, 0x5f, 0x0e, 0xad, 0x4e,
            0xf6, 0x84, 0x9c, 0xf0, 0xda, 0x61, 0x5a, 0xe0,
            0x1e, 0xd9, 0xfb, 0xd1, 0xd6, 0xcb, 0x5b, 0xe1
        },
        {0xde, 0xad, 0xff, 0x50, 0x7d, 0x91, 0x6b, 0x59},
        {
            0x8c, 0x19, 0x1a, 0x44, 0x69, 0xea, 0x36, 0xf7,
            0xcc, 0x55, 0x6b, 0xf9, 0x12, 0xc9, 0x64, 0xcd,
            0x62, 0x75, 0x36, 0xd2, 0x41, 0x12, 0x1f, 0xac,
            0x72, 0xee, 0x80, 0xf6, 0x12, 0x3b, 0x17, 0x2c
        },
        {0x00},
        {
            0xd0, 0xdd, 0x96, 0xff, 0xca, 0x0e, 0xc5, 0x61,
            0x7c, 0xf7, 0xf2, 0xcd, 0xef, 0x25, 0x14, 0xaa,
            0xa6, 0xa8, 0xa2, 0xe0, 0xf9, 0x9c, 0x0b, 0x06,
            0xb1, 0xfa, 0x78, 0xc8, 0xe3, 0xa0, 0xc4, 0x99
        },
        {0x00},
        {0x00},
        {
            0xe5, 0x9f, 0xb3, 0x1e, 0xcd, 0x78, 0xa2, 0xb5,
            0xff, 0x85, 0xb7, 0x40, 0x32, 0xc4, 0x4a, 0x97
        }
    },
    {
        32,
        8,
        32,
        32,
        {
            0x0c, 0xe1, 0xea, 0x79, 0x93, 0x10, 0xb4, 0x02,
            0xe1, 0xe2, 0x35, 0x1c, 0x49, 0xcc, 0x15, 0x49,
            0x99, 0x18, 0x60, 0x40, 0x3a, 0x35, 0xc2, 0xd8,
            0x8a, 0x3d, 0x02, 0x34, 0x77, 0xc6, 0xbe, 0x2e
        },
        {0x1e, 0xe4, 0x70, 0x46, 0xf8, 0x16, 0x4c, 0xa6},
        {
            0x47, 0xd2, 0xf8, 0xfb, 0x7e, 0xb6, 0xa7, 0x82,
            0xed, 0x56, 0x62, 0x7b, 0x9d, 0x14, 0x58, 0x11,
            0xfa, 0x85, 0x50, 0x90, 0x11, 0x60, 0x09, 0xf5,
            0x07, 0xaa, 0xd2, 0xb5, 0xdb, 0x1c, 0x67, 0xcc
        },
        {
            0x4a, 0xc6, 0xc6, 0xc8, 0xa5, 0xf7, 0x3f, 0x4f,
            0xe2, 0x2a, 0xd6, 0x6c, 0xd3, 0xd5, 0xb5, 0x4c,
            0x90, 0xfe, 0x7d, 0x9e, 0xad, 0x2f, 0xa7, 0x20,
            0xbd, 0x9e, 0x1c, 0x69, 0x06, 0x97, 0x11, 0xf8
        },
        {
            0x81, 0x09, 0x62, 0xb6, 0x36, 0x4b, 0x37, 0xc4,
            0x2a, 0x9f, 0x16, 0x74, 0x3f, 0xcc, 0xb6, 0x6f,
            0x0d, 0xad, 0x1d, 0x43, 0xdf, 0xe6, 0xbd, 0xaf,
            0xf0, 0xc8, 0xc8, 0x32, 0xe6, 0x80, 0xd5, 0xd4
        },
        {
            0x3a, 0x64, 0x3f, 0x8e, 0x66, 0xc1, 0x83, 0x6b,
            0xc4, 0x62, 0xcd, 0x5a, 0xc0, 0x60, 0x3a, 0x60,
            0xa9, 0xd9, 0x8f, 0x4b, 0x8a, 0x06, 0x35, 0x5f,
            0x46, 0x7e, 0x73, 0x3d, 0xf9, 0xcb, 0xec, 0x16
        },
        {
            0x06, 0x81, 0xc3, 0x9a, 0xa8, 0xc1, 0x63, 0x5a,
            0x9e, 0xc8, 0x74, 0x29, 0x0a, 0xf0, 0x2a, 0x33,
            0x05, 0x96, 0x60, 0x12, 0xd5, 0x08, 0xf6, 0x62,
            0x6c, 0xb0, 0x6e, 0x86, 0x22, 0x80, 0xc7, 0xba
        },
        {
            0xe1, 0x19, 0xa5, 0x98, 0x61, 0x15, 0x37, 0x72,
            0xd2, 0x04, 0x92, 0x83, 0x57, 0xcc, 0x71, 0xc8
        }
    },
    {
        32,
        8,
        0,
        0,
        {
            0xaa, 0xed, 0xf0, 0xbd, 0x12, 0x37, 0xe5, 0x0b,
            0x2e, 0xa3, 0x3e, 0xbd, 0xe0, 0xf5, 0x08, 0x5e,
            0xc8, 0xb3, 0x69, 0xc9, 0xc5, 0x4d, 0x34, 0xf0,
            0xa9, 0x8c, 0xcd, 0xee, 0x3d, 0x3f, 0x3e, 0x4f
        },
        {0x7b, 0xbd, 0x11, 0x9b, 0x3f, 0xef, 0x62, 0x59},
        {0x00},
        {0x00},
        {
            0xc2, 0x2e, 0x29, 0xcd, 0x30, 0x5e, 0xbd, 0x08,
            0xd4, 0x70, 0xec, 0x6e, 0x84, 0xb0, 0x19, 0x42,
            0xcf, 0x27, 0x65, 0x89, 0x57, 0x2f, 0x2d, 0x35,
            0x72, 0x3c, 0xc5, 0x8e, 0x02, 0xe7, 0x75, 0xe2
        },
        {0x00},
        {0x00},
        {
            0x39, 0x05, 0x20, 0xb2, 0xa6, 0x64, 0x43, 0x91,
            0xb0, 0xeb, 0xc1, 0xf3, 0x22, 0xca, 0x16, 0x50
        }
    }
};

/*
  0, Entropy len
  0, Nonce len
  0, Personalization len
  0, Additional len
  {0x00}, Entropy
  {0x00}, Nonce
  {0x00}, Personalization string
  {0x00}, Additional input
  {0x00}, Entropy input reseed
  {0x00}, Additional input reseed
  {0x00}, Additional input final
  {0x00}  Result
*/

/* ********************* Data input functions *********************** */

static unsigned char default_entropy_block[] =
{
    0xaa, 0xed, 0xf0, 0xbd, 0x12, 0x37, 0xe5, 0x0b,
    0x2e, 0xa3, 0x3e, 0xbd, 0xe0, 0xf5, 0x08, 0x5e,
    0xc8, 0xb3, 0x69, 0xc9, 0xc5, 0x4d, 0x34, 0xf0,
    0xa9, 0x8c, 0xcd, 0xee, 0x3d, 0x3f, 0x3e, 0x4f
};

static uint32_t entropy_function_used;

static unsigned char *next_entropy;
static rngsize_t next_entropy_len;

static void
entropy_input(void *function_arg,
              unsigned char *buffer,
              rngsize_t buffer_size,
              rngsize_t *input_size)
{
    ASSERT(buffer_size >= next_entropy_len);
    IDENTIFIER_NOT_USED(function_arg);

    c_memcpy(buffer, next_entropy, next_entropy_len);
    *input_size = next_entropy_len;
    entropy_function_used++;
}

static unsigned char *next_nonce;
static rngsize_t next_nonce_len;

static void
nonce_input(void *function_arg,
            unsigned char *buffer,
            rngsize_t buffer_size,
            rngsize_t *input_size)
{
    ASSERT(buffer_size >= next_nonce_len);
    IDENTIFIER_NOT_USED(function_arg);

    c_memcpy(buffer, next_nonce, next_nonce_len);
    *input_size = next_nonce_len;
}

/* Provide specified memory area only once allocator,
   used by the Drbg health tests. */
typedef struct SshDrbgAllocOnceRec
{
    SshDrbgAllocRec alloc_rec;
    bool allocated;
    void *memory_base;
    size_t maximum_size;
} SshDrbgAllocOnceStruct;
typedef SshDrbgAllocOnceStruct *SshDrbgAllocOnce;

static void *
drbg_alloc_once_malloc(size_t memory_size,
                       void * Context_p)
{
    SshDrbgAllocOnce allocator = Context_p;
    PRECONDITION(allocator != NULL);
    PRECONDITION(allocator->alloc_rec.Context_p == Context_p);
    PRECONDITION(allocator->alloc_rec.MallocFunc_p == &drbg_alloc_once_malloc);

    if (allocator->allocated == false &&
            allocator->maximum_size >= memory_size)
    {
        allocator->allocated = true;
        return allocator->memory_base;
    }

    return NULL;
}

static void
drbg_alloc_once_free(void * Data_p,
                     void * Context_p)
{
    SshDrbgAllocOnce allocator = Context_p;

    /* Skip function if Data_p is NULL. */
    if (Data_p == NULL)
    {
        return;
    }

    ASSERT(allocator != NULL);
    ASSERT(allocator->alloc_rec.Context_p == Context_p);
    ASSERT(allocator->alloc_rec.FreeFunc_p == &drbg_alloc_once_free);
    ASSERT(allocator->allocated == true);
    ASSERT(allocator->memory_base == Data_p);

    allocator->allocated = false;
}

#ifndef IMPLDEFS_CF_DISABLE_ASSERT
static bool
drbg_alloc_once_is_allocated(SshDrbgAllocOnce allocator)
{
    ASSERT(allocator != NULL);
    return allocator->allocated;
}
#endif /* IMPLDEFS_CF_DISABLE_ASSERT */

static SshDrbgAlloc
drbg_alloc_once_init(SshDrbgAllocOnce allocator,
                     void *           memory_base,
                     size_t           memory_size)
{
    allocator->allocated = false;
    allocator->memory_base = memory_base;
    allocator->maximum_size = memory_size;
    allocator->alloc_rec.Context_p = allocator;
    allocator->alloc_rec.MallocFunc_p = &drbg_alloc_once_malloc;
    allocator->alloc_rec.FreeFunc_p = &drbg_alloc_once_free;
    return &(allocator->alloc_rec);
}

/* ********************** Known answer tests ************************ */

static bool
drbg_known_answer_test_run(SshDrbgKnownAnswerTestStruct test_case)
{
    SshDrbgState state_handle = NULL;
    unsigned char pseudorandom_buffer[PSEUDORANDOM_BUFFER_LEN];
    SshDrbgStateEstimateStruct storageForInstance;
    SshDrbgAllocOnceStruct allocOnce;
    SshDrbgAlloc allocator;

    allocator = drbg_alloc_once_init(&allocOnce,
                                     &storageForInstance,
                                     sizeof(storageForInstance));

    next_entropy = test_case.entropy_input;
    next_entropy_len = test_case.entropy_len;
    next_nonce = test_case.nonce;
    next_nonce_len = test_case.nonce_len;

    if (ssh_drbg_instantiate(DRBG_TEST_SECURITY_STRENGTH,
                             false,
                             test_case.personalization_string,
                             test_case.personalization_string_len,
                             entropy_input,
                             nonce_input,
                             NULL,
                             &state_handle,
                             allocator)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to instantiate");
        return false;
    }

    if (ssh_drbg_generate(PSEUDORANDOM_BUFFER_LEN * 8,
                          128,
                          false,
                          test_case.additional_input,
                          test_case.additional_input_len,
                          pseudorandom_buffer,
                          state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed first generate");
        return false;
    }

    next_entropy = test_case.entropy_input_reseed;

    if (ssh_drbg_reseed(test_case.additional_input_reseed,
                        test_case.additional_input_len,
                        state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed reseed generate");
        return false;
    }

    if (ssh_drbg_generate(PSEUDORANDOM_BUFFER_LEN * 8,
                          128,
                          false,
                          test_case.additional_input_final,
                          test_case.additional_input_len,
                          pseudorandom_buffer,
                          state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed second generate");
        return false;
    }

    if (ssh_drbg_uninstantiate(state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to uninstantiate");
        return false;
    }

    if (c_memcmp(test_case.returned_bits, pseudorandom_buffer,
                 PSEUDORANDOM_BUFFER_LEN))
    {
        L_DEBUG(LF_DRBG_FAIL, "Wrong result in known answer test");
        return false;
    }

#ifndef IMPLDEFS_CF_DISABLE_ASSERT
    ASSERT(!drbg_alloc_once_is_allocated(&allocOnce));
#endif

    return true;
}

/* ************************* Tests ********************************* */

bool
ssh_drbg_known_answer_tests(void)
{
    uint32_t number_of_tests, i;
    uint32_t failed_tests = 0;

    number_of_tests =
        sizeof(drbg_test_cases) / sizeof(SshDrbgKnownAnswerTestStruct);

    for (i = 0; i < number_of_tests; i++)
    {
        if (!drbg_known_answer_test_run(drbg_test_cases[i]))
        {
            failed_tests++;
            L_DEBUG(LF_DRBG_FAIL,
                    "Failed known test case %u/%u",
                    i + 1, number_of_tests);
        }
    }

    if (failed_tests == 0)
    {
        L_TRACE(LF_DRBG_OK, "Passed %u known answer test cases", i);
    }

    return failed_tests == 0;
}

bool
ssh_drbg_instantiate_tests(void)
{
    SshDrbgState state_handle = NULL;

    SshDrbgStateEstimateStruct storageForInstance;
    SshDrbgAllocOnceStruct allocOnce;
    SshDrbgAlloc allocator;

    allocator = drbg_alloc_once_init(&allocOnce,
                                     &storageForInstance,
                                     sizeof(storageForInstance));

    next_entropy = NULL;
    next_entropy_len = 0;

    /* Test with invalid entropy source */
    if (ssh_drbg_instantiate(DRBG_TEST_SECURITY_STRENGTH,
                             false,
                             NULL,
                             0,
                             entropy_input,
                             NULL,
                             NULL,
                             &state_handle,
                             allocator)
            == SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Instantiated without working entropy source");
        return false;
    }

    L_TRACE(LF_DRBG_OK, "Passed instantiate test");

#ifndef IMPLDEFS_CF_DISABLE_ASSERT
    ASSERT(!drbg_alloc_once_is_allocated(&allocOnce));
#endif

    return true;
}

bool
ssh_drbg_generate_tests(void)
{
    SshDrbgState state_handle = NULL;
    uint32_t entropy_count_before;
    unsigned char pseudorandom_buffer[PSEUDORANDOM_BUFFER_LEN];
    SshDrbgStateEstimateStruct storageForInstance;
    SshDrbgAllocOnceStruct allocOnce;
    SshDrbgAlloc allocator;

    allocator = drbg_alloc_once_init(&allocOnce,
                                     &storageForInstance,
                                     sizeof(storageForInstance));

    next_entropy = default_entropy_block;
    next_entropy_len = DRBG_TEST_ENTROPY_SIZE;

    /* Create instantiation, use default entropy source */
    if (ssh_drbg_instantiate(DRBG_TEST_SECURITY_STRENGTH,
                             false,
                             NULL,
                             0,
                             entropy_input,
                             NULL,
                             NULL,
                             &state_handle,
                             allocator)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed instantiation in generate-tests");
        return false;
    }

    /* Invalid security strength */
    if (ssh_drbg_generate(PSEUDORANDOM_BUFFER_LEN * 8,
                          DRBG_TEST_SECURITY_STRENGTH + 1,
                          false,
                          NULL,
                          0,
                          pseudorandom_buffer,
                          state_handle)
            != SSH_RNG_UNSUPPORTED)
    {
        L_DEBUG(LF_DRBG_FAIL, "Accepted invalid security strength");
        return false;
    }

    /* Invalid personalization string strength */
    if (ssh_drbg_generate(PSEUDORANDOM_BUFFER_LEN * 8,
                          DRBG_TEST_SECURITY_STRENGTH,
                          false,
                          NULL,
                          33,
                          pseudorandom_buffer,
                          state_handle)
            != SSH_RNG_UNSUPPORTED)
    {
        L_DEBUG(LF_DRBG_FAIL, "Accepted invalid personalization data size");
        return false;
    }

    if (ssh_drbg_uninstantiate(state_handle) != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to uninstatiate");
        return false;
    }


    /* Reseed frequency, instantiate with test entropy source */
    next_entropy = default_entropy_block;
    next_entropy_len = DRBG_TEST_ENTROPY_SIZE;

    if (ssh_drbg_instantiate(DRBG_TEST_SECURITY_STRENGTH,
                             false,
                             NULL,
                             0,
                             entropy_input,
                             NULL,
                             NULL,
                             &state_handle,
                             allocator)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed instantiation in generate-tests");
        return false;
    }

    if (ssh_drbg_set_state(1025,
                           NULL,
                           0,
                           NULL,
                           0,
                           state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to set state");
        return false;
    }

    entropy_count_before = entropy_function_used;

    if (ssh_drbg_generate(PSEUDORANDOM_BUFFER_LEN * 8,
                          DRBG_TEST_SECURITY_STRENGTH,
                          false,
                          NULL,
                          0,
                          pseudorandom_buffer,
                          state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Generate functionality failed");
        return false;
    }

    if (entropy_count_before + 1 != entropy_function_used)
    {
        L_DEBUG(LF_DRBG_FAIL, "DRBG-state not reseeded");
        return false;
    }

    if (ssh_drbg_uninstantiate(state_handle)
            != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to uninstatiate");
        return false;
    }

    L_TRACE(LF_DRBG_OK, "Passed generate test");

#ifndef IMPLDEFS_CF_DISABLE_ASSERT
    ASSERT(!drbg_alloc_once_is_allocated(&allocOnce));
#endif

    return true;
}

bool
ssh_drbg_health_test(void)
{
    entropy_function_used = 0;

    if (!ssh_drbg_instantiate_tests())
    {
        return false;
    }

    if (!ssh_drbg_known_answer_tests())
    {
        return false;
    }

    if (!ssh_drbg_generate_tests())
    {
        return false;
    }

    /* Tests for reseed and uninstantiate functionality
       are done in the functions above */
    return true;
}

#endif /* SSHDIST_CRYPT_NIST_SP_800_90 */

/* end of file nist-health-test.c */
