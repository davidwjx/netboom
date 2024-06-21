/* Copyright 2015, Kenneth MacKay. Licensed under the BSD 2-clause license. */
/* Changes by INSIDE Secure. */

/* This file is not part of default distribution of uECC.
   The file basically provides ARM multiplication and squaring via a simple
   API, which can be used by public key algorithms beyond ECC.
   (Namely, ). */

// #include "pscompilerdep.h"

#ifndef __ARMCC5
#if defined(__aarch32__) || defined(__arm__)

/* Avoid warning on missing vli add/sub, the functions are not included here. */
#ifndef uECC_NO_VLI_ADD_SUB
#define uECC_NO_VLI_ADD_SUB
#endif /* uECC_NO_VLI_ADD_SUB */

// #include "osdep_stdint.h"
#include "uECC.h"
#include "uECC_vli.h"

/* This functionality is only available on ARM target(s) with GNU style
   inline assembly. This file cannot be compiled with ARMCC. */

#ifndef uECC_RNG_MAX_TRIES
    #define uECC_RNG_MAX_TRIES 64
#endif

#if uECC_ENABLE_VLI_API
    #define uECC_VLI_API
#else
    #define uECC_VLI_API
#endif

#define uECC_MIN_WORDS 8

#include "types.h"

#if uECC_ARM_USE_UMAAL
    #include "asm_arm_mult_square_umaal.inc"
#else
    #include "asm_arm_mult_square.inc"
#endif

#if (uECC_PLATFORM == uECC_arm_thumb)
    #define REG_RW "+l"
    #define REG_WRITE "=l"
#else
    #define REG_RW "+r"
    #define REG_WRITE "=r"
#endif

#if (uECC_PLATFORM == uECC_arm_thumb || uECC_PLATFORM == uECC_arm_thumb2)
    #define REG_RW_LO "+l"
    #define REG_WRITE_LO "=l"
#else
    #define REG_RW_LO "+r"
    #define REG_WRITE_LO "=r"
#endif

#if (uECC_PLATFORM == uECC_arm_thumb2)
    #define RESUME_SYNTAX
#else
    #define RESUME_SYNTAX ".syntax divided \n\t"
#endif

#undef uECC_OPTIMIZATION_LEVEL
#define uECC_OPTIMIZATION_LEVEL 4

uECC_VLI_API void uECC_vli_mult8(uint32_t *result,
                                const uint32_t *left,
                                const uint32_t *right)
{
    register uint32_t *r0 __asm__("r0") = result;
    register const uint32_t *r1 __asm__("r1") = left;
    register const uint32_t *r2 __asm__("r2") = right;
    register uint32_t r3 __asm__("r3") = 8;

    __asm__ volatile (
        ".syntax unified \n\t"

#if (uECC_MIN_WORDS == 8)
        FAST_MULT_ASM_8
#endif
        "1: \n\t"
        RESUME_SYNTAX
        : "+r" (r0), "+r" (r1), "+r" (r2)
        : "r" (r3)
        : "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14", "cc", "memory"
    );
}

uECC_VLI_API void uECC_vli_square8(uECC_word_t *result,
                                  const uECC_word_t *left)
{
  register uint32_t *r0 __asm__("r0") = result;
  register const uint32_t *r1 __asm__("r1") = left;
  register uint32_t r2 __asm__("r2") = 8;

  __asm__ volatile (
        ".syntax unified \n\t"
#if (uECC_MIN_WORDS == 8)
        FAST_SQUARE_ASM_8
#endif

        "1: \n\t"
        RESUME_SYNTAX
        : "+r" (r0), "+r" (r1)
        : "r" (r2)
        : "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10", "r11", "r12", "r14", "cc", "memory"
    );
}

#endif /* defined(__aarch32__) || defined(__arm__) */
#endif /* !defined __ARMCC5. */
