/* inline_gen.inc
 *
 * Generic inlining of VLI API operations.
 */

/*****************************************************************************
* Copyright (c) 2017 INSIDE Secure Oy. All Rights Reserved.
*
* This confidential and proprietary software may be used only as authorized
* by a licensing agreement from INSIDE Secure.
*
* The entire notice above must be reproduced on all authorized copies that
* may only be made to the extent permitted by a licensing agreement from
* INSIDE Secure.
*****************************************************************************/

/* This file is extensions to uECC.c/h. Do not include the file directly. */
#ifndef UECC_INLINE_INC
#define UECC_INLINE_INC 1

#include "asm_x86.inc"

#if uECC_VLI_ADD_INLINE

#define uECC_vli_add uECC_vli_add_dispatch
#define uECC_vli_add_implement uECC_vli_add_generic
#define uECC_vli_sub uECC_vli_sub_dispatch
#define uECC_vli_sub_implement uECC_vli_sub_generic

/* May evaluate to 1 on compile time if a and b are always the same
   pointer. Otherwise 0. */
#define is_same_ptr(a, b)                                               \
    __builtin_choose_expr(__builtin_constant_p((a) - (b)), ((a) - (b)) == 0, 0)

/* May evaluate to 1 on compiler time if a and n are the same value.
   Otherwise 0. */
#define is_constant_n(a, n) \
    __builtin_choose_expr(__builtin_constant_p((a) - (n)), ((a) - (n)) == 0, 0)

#if uECC_ENABLE_VLI_API
# define uECC_VLI_STATIC
#else
# define uECC_VLI_STATIC static
#endif

#include <stdio.h>

uECC_VLI_STATIC uECC_word_t uECC_vli_add_implement(uECC_word_t *result,
                                                   const uECC_word_t *left,
                                                   const uECC_word_t *right,
                                                   wordcount_t num_words);

uECC_VLI_STATIC uECC_word_t uECC_vli_sub_implement(uECC_word_t *result,
                                                   const uECC_word_t *left,
                                                   const uECC_word_t *right,
                                                   wordcount_t num_words);

#if uECC_SUPPORTS_secp224r1
static inline
uECC_word_t uECC_vli_accumulate128bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    unsigned char carry;

    uECC_vli_add4_carry_start(left[0], left[1], left[2], left[3],
                              right[0], right[1], right[2], right[3],
                              carry);
    return (uECC_word_t) carry;
}
#endif

#if uECC_SUPPORTS_secp192r1
static inline
uECC_word_t uECC_vli_accumulate192bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    unsigned char carry;

    uECC_vli_add3_carry_start(left[0], left[1], left[2],
                              right[0], right[1], right[2],
                              carry);
    uECC_vli_add3_carry_cont(left[3], left[4], left[5],
                             right[3], right[4], right[5],
                             carry);
    return (uECC_word_t) carry;
}
#endif

#if uECC_SUPPORTS_secp224r1
static inline
uECC_word_t uECC_vli_accumulate224bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    unsigned char carry;

    uECC_vli_add4_carry_start(left[0], left[1], left[2], left[3],
                              right[0], right[1], right[2], right[3],
                              carry);
    uECC_vli_add3_carry_cont(left[4], left[5], left[6],
                             right[4], right[5], right[6],
                             carry);
    return (uECC_word_t) carry;
}
#endif

#if uECC_SUPPORTS_secp256r1
static inline
uECC_word_t uECC_vli_accumulate256bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    unsigned char carry;

    uECC_vli_add4_carry_start(left[0], left[1], left[2], left[3],
                              right[0], right[1], right[2], right[3],
                              carry);
    uECC_vli_add4_carry_cont(left[4], left[5], left[6], left[7],
                              right[4], right[5], right[6], right[7],
                              carry);
    return (uECC_word_t) carry;
}
#endif

#if uECC_VERIFY_ONLY == 0
static inline
uECC_word_t uECC_vli_accumulate_neg128bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    unsigned char borrow;

    uECC_vli_sub4_borrow_start(left[0], left[1], left[2], left[3],
                               right[0], right[1], right[2], right[3],
                               borrow);
    return (uECC_word_t) borrow;
}
#endif

#if uECC_SUPPORTS_secp192r1
static inline
uECC_word_t uECC_vli_accumulate_neg192bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    unsigned char borrow;

    uECC_vli_sub3_borrow_start(left[0], left[1], left[2],
                               right[0], right[1], right[2],
                               borrow);
    uECC_vli_sub3_borrow_cont(left[3], left[4], left[5],
                              right[3], right[4], right[5],
                              borrow);
    return (uECC_word_t) borrow;
}
#endif

#if uECC_SUPPORTS_secp224r1
static inline
uECC_word_t uECC_vli_accumulate_neg224bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    unsigned char borrow;

    uECC_vli_sub4_borrow_start(left[0], left[1], left[2], left[3],
                               right[0], right[1], right[2], right[3],
                               borrow);
    uECC_vli_sub3_borrow_cont(left[4], left[5], left[6],
                              right[4], right[5], right[6],
                              borrow);
    return (uECC_word_t) borrow;
}
#endif

#if uECC_SUPPORTS_secp256r1
static inline
uECC_word_t uECC_vli_accumulate_neg256bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    unsigned char borrow;

    uECC_vli_sub4_borrow_start(left[0], left[1], left[2], left[3],
                               right[0], right[1], right[2], right[3],
                               borrow);
    uECC_vli_sub4_borrow_cont(left[4], left[5], left[6], left[7],
                              right[4], right[5], right[6], right[7],
                              borrow);
    return (uECC_word_t) borrow;
}
#endif

static inline
uECC_word_t uECC_vli_accumulate(uECC_word_t *left,
                                const uECC_word_t *right,
                                wordcount_t num_words)
    __attribute__((__always_inline__));
static inline
uECC_word_t uECC_vli_accumulate(uECC_word_t *left,
                                const uECC_word_t *right,
                                wordcount_t num_words)
{
#if uECC_SUPPORTS_secp192r1
    if (num_words == 192 / 8 / uECC_WORD_SIZE)
    {
        /* Specific length function. */
        return uECC_vli_accumulate192bit(left, right);
    }
    else
#endif
#if uECC_SUPPORTS_secp224r1
    if (num_words == 224 / 8 / uECC_WORD_SIZE)
    {
        /* Specific length function. */
        return uECC_vli_accumulate224bit(left, right);
    }
    else
#endif
#if uECC_SUPPORTS_secp256r1
    if (num_words == 256 / 8 / uECC_WORD_SIZE)
    {
        /* Specific length function. */
        return uECC_vli_accumulate256bit(left, right);
    }
    else
#endif
    {
        /* Generic function. */
        return uECC_vli_add_generic(left, left, right, num_words);
    }
}

static inline
uECC_word_t uECC_vli_accumulate_neg(uECC_word_t *left,
                                    const uECC_word_t *right,
                                    wordcount_t num_words)
    __attribute__((__always_inline__));
static inline
uECC_word_t uECC_vli_accumulate_neg(uECC_word_t *left,
                                    const uECC_word_t *right,
                                    wordcount_t num_words)
{
#if uECC_SUPPORTS_secp192r1
    if (num_words == 192 / 8 / uECC_WORD_SIZE)
    {
        /* Specific length function. */
        return uECC_vli_accumulate_neg192bit(left, right);
    }
    else
#endif
#if uECC_SUPPORTS_secp224r1
    if (num_words == 224 / 8 / uECC_WORD_SIZE)
    {
        /* Specific length function. */
        return uECC_vli_accumulate_neg224bit(left, right);
    }
    else
#endif
#if uECC_SUPPORTS_secp256r1
    if (num_words == 256 / 8 / uECC_WORD_SIZE)
    {
        /* Specific length function. */
        return uECC_vli_accumulate_neg256bit(left, right);
    }
    else
#endif
    {
        /* Generic function. */
        return uECC_vli_sub_generic(left, left, right, num_words);
    }
}

static inline
uECC_word_t uECC_vli_add_dispatch(uECC_word_t *result,
                                  const uECC_word_t *left,
                                  const uECC_word_t *right,
                                  wordcount_t num_words)
    __attribute__((__always_inline__));

static inline
uECC_word_t uECC_vli_add_dispatch(uECC_word_t *result,
                                  const uECC_word_t *left,
                                  const uECC_word_t *right,
                                  wordcount_t num_words)
{
    //printf("uECC_vli_add: STATIC: same=%d len=%d DYN: same=%d len=%d\n", is_same_ptr(result, left), (int) __builtin_choose_expr(__builtin_constant_p(num_words), num_words, -1), result==left, num_words);
    return uECC_vli_add_generic(result, left, right, num_words);
}

static inline
uECC_word_t uECC_vli_sub_dispatch(uECC_word_t *result,
                                  const uECC_word_t *left,
                                  const uECC_word_t *right,
                                  wordcount_t num_words)
    __attribute__((__always_inline__));

static inline
uECC_word_t uECC_vli_sub_dispatch(uECC_word_t *result,
                                  const uECC_word_t *left,
                                  const uECC_word_t *right,
                                  wordcount_t num_words)
{
    //printf("uECC_vli_add: STATIC: same=%d len=%d DYN: same=%d len=%d\n", is_same_ptr(result, left), (int) __builtin_choose_expr(__builtin_constant_p(num_words), num_words, -1), result==left, num_words);
    return uECC_vli_sub_generic(result, left, right, num_words);
}
#else
/* Actual function name to use for uECC_vli_add/sub: all are same. */
#define uECC_vli_add uECC_vli_add
#define uECC_vli_add_implement uECC_vli_add
#define uECC_vli_add_generic uECC_vli_add
#define uECC_vli_sub uECC_vli_sub
#define uECC_vli_sub_implement uECC_vli_sub
#define uECC_vli_sub_generic uECC_vli_sub

#if uECC_ENABLE_VLI_API
# define uECC_VLI_STATIC
#else
# define uECC_VLI_STATIC static
#endif

#ifndef uECC_NO_VLI_ADD_SUB
uECC_VLI_STATIC uECC_word_t uECC_vli_add_implement(uECC_word_t *result,
                                                   const uECC_word_t *left,
                                                   const uECC_word_t *right,
                                                   wordcount_t num_words);

uECC_VLI_STATIC uECC_word_t uECC_vli_sub_implement(uECC_word_t *result,
                                                   const uECC_word_t *left,
                                                   const uECC_word_t *right,
                                                   wordcount_t num_words);

static inline
uECC_word_t uECC_vli_accumulate(uECC_word_t *left,
                                const uECC_word_t *right,
                                wordcount_t num_words)
{
    return uECC_vli_add_generic(left, left, right, num_words);
}

#if uECC_SUPPORTS_secp224r1
static inline
uECC_word_t uECC_vli_accumulate128bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    return uECC_vli_add_generic(left, left, right,
                                128 / 8 / uECC_WORD_SIZE);
}
#endif

#if uECC_SUPPORTS_secp192r1
static inline
uECC_word_t uECC_vli_accumulate192bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    return uECC_vli_add_generic(left, left, right,
                                192 / 8 / uECC_WORD_SIZE);
}
#endif

#if uECC_SUPPORTS_secp224r1
static inline
uECC_word_t uECC_vli_accumulate224bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
#if uECC_WORD_SIZE <= 4
    return uECC_vli_add_generic(left, left, right,
                                224 / 8 / uECC_WORD_SIZE);
#else
    return uECC_vli_add_generic(left, left, right, 4);
#endif
}
#endif

#if uECC_SUPPORTS_secp256r1
static inline
uECC_word_t uECC_vli_accumulate256bit(uECC_word_t *left,
                                      const uECC_word_t *right)
{
    return uECC_vli_add_generic(left, left, right,
                                256 / 8 / uECC_WORD_SIZE);
}
#endif

static inline
uECC_word_t uECC_vli_accumulate_neg(uECC_word_t *left,
                                    const uECC_word_t *right,
                                    wordcount_t num_words)
{
    return uECC_vli_sub_generic(left, left, right, num_words);
}

#if uECC_VERIFY_ONLY == 0
static inline
uECC_word_t uECC_vli_accumulate_neg128bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    return uECC_vli_sub_generic(left, left, right,
                                128 / 8 / uECC_WORD_SIZE);
}
#endif

#if uECC_SUPPORTS_secp192r1
static inline
uECC_word_t uECC_vli_accumulate_neg192bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    return uECC_vli_sub_generic(left, left, right,
                                192 / 8 / uECC_WORD_SIZE);
}
#endif

#if uECC_SUPPORTS_secp224r1
static inline
uECC_word_t uECC_vli_accumulate_neg224bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
#if uECC_WORD_SIZE <= 4
    return uECC_vli_sub_generic(left, left, right,
                                224 / 8 / uECC_WORD_SIZE);
#else
    return uECC_vli_sub_generic(left, left, right, 4);
#endif
}
#endif

#if uECC_SUPPORTS_secp256r1
static inline
uECC_word_t uECC_vli_accumulate_neg256bit(uECC_word_t *left,
                                          const uECC_word_t *right)
{
    return uECC_vli_sub_generic(left, left, right,
                                256 / 8 / uECC_WORD_SIZE);
}
#endif

#endif /* uECC_NO_VLI_ADD_SUB */

#endif

#endif /* UECC_INLINE_INC */
