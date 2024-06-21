/* asm_x86.inc
 *
 * X86 architecture specific assembly for VLI API operations.
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
#ifndef ASM_X86_INC
#define ASM_X86_INC 1

#define uECC_VLI_ADD_INLINE 0

#if defined __x86__ || defined(i386) || defined(__i386__)
#if !defined(__x86_64__)
#ifdef __GNUC__

/* GNU specific inline assembly. */

#undef uECC_VLI_ADD_INLINE
#define uECC_VLI_ADD_INLINE 1

#ifdef uECC_vli_add4_carry_start
#error "uECC_vli_add4_carry_start already defined!"
#endif

/* Inline assembly for */

#ifndef ASM_LINE
#define ASM_LINE(x) x "\n\t"
#endif /* ASM_LINE */

#ifdef uECC_X86_REGISTER_CARRY
/* Keep carries in registers: allocates one more register.
   Has been noticed to fail compilation on gcc 4.x series compilers. */
#define uECC_X86_CARRY_INOUT_CONSTRAINT "+&q"
#define uECC_X86_CARRY_IN_CONSTRAINT "q"
#else
/* Keep carries in memory (in the 4 values variant.) */
#define uECC_X86_CARRY_INOUT_CONSTRAINT "+m"
#define uECC_X86_CARRY_IN_CONSTRAINT "m"
#endif

#define uECC_vli_add4_carry_start(r0, r1, r2, r3, s0, s1, s2, s3, carry_out) \
    __asm__(ASM_LINE("addl %5, %0") \
            ASM_LINE("adcl %6, %1")  \
            ASM_LINE("adcl %7, %2")  \
            ASM_LINE("adcl %8, %3")  \
            ASM_LINE("setb %4")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&r" (r3),            \
              "=qm" (carry_out)       \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              "Irm" (s3)              \
            : "cc")

#define uECC_vli_add4_carry_cont(r0, r1, r2, r3, s0, s1, s2, s3, carry_io) \
    __asm__(ASM_LINE("addb $-1, %4") \
            ASM_LINE("adcl %5, %0")      \
            ASM_LINE("adcl %6, %1")  \
            ASM_LINE("adcl %7, %2")  \
            ASM_LINE("adcl %8, %3")  \
            ASM_LINE("setb %4")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&r" (r3),            \
              uECC_X86_CARRY_INOUT_CONSTRAINT (carry_io)       \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              "Irm" (s3)              \
            : "cc")

#define uECC_vli_add1_carry_cont(r0, s0, carry_io) \
    __asm__(ASM_LINE("addb $-1, %1") \
            ASM_LINE("adcl %2, %0")      \
            ASM_LINE("setb %1")  \
            : "+&r" (r0),            \
              "+&q" (carry_io)       \
            : "Irm" (s0)             \
            : "cc")

#define uECC_vli_add4_carry_finish(r0, r1, r2, r3, s0, s1, s2, s3, c_lo, c_in, c_out) \
    __asm__(ASM_LINE("addb $-1, %[clo]") \
            ASM_LINE("adcl %6, %0")  \
            ASM_LINE("adcl %7, %1")  \
            ASM_LINE("adcl %8, %2")  \
            ASM_LINE("adcl %9, %3")  \
            ASM_LINE("adcl $0, %4")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&r" (r3),            \
              "=&r" (c_out)          \
            : "4" (c_in),            \
              "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              "Irm" (s3),             \
              [clo] uECC_X86_CARRY_IN_CONSTRAINT (c_lo)        \
            : "cc")

#define uECC_vli_sub4_borrow_start(r0, r1, r2, r3, s0, s1, s2, s3, borrow_out) \
    __asm__(ASM_LINE("subl %5, %0")                                     \
            ASM_LINE("sbbl %6, %1")  \
            ASM_LINE("sbbl %7, %2")  \
            ASM_LINE("sbbl %8, %3")  \
            ASM_LINE("setc %4")      \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&r" (r3),            \
              "=qm" (borrow_out)      \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              "Irm" (s3) : "cc")

#define uECC_vli_sub4_borrow_cont(r0, r1, r2, r3, s0, s1, s2, s3, borrow_io) \
    __asm__(ASM_LINE("addb $-1, %4") \
            ASM_LINE("sbbl %5, %0")      \
            ASM_LINE("sbbl %6, %1")  \
            ASM_LINE("sbbl %7, %2")  \
            ASM_LINE("sbbl %8, %3")  \
            ASM_LINE("setc %4")      \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&r" (r3),            \
              uECC_X86_CARRY_INOUT_CONSTRAINT (borrow_io)       \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              "Irm" (s3) : "cc")

#define uECC_vli_sub4_borrow_finish(r0, r1, r2, r3, s0, s1, s2, s3, borrow_in, c_in, c_out) \
    __asm__(ASM_LINE("addb $-1, %[blo]") \
            ASM_LINE("sbbl %6, %0")     \
            ASM_LINE("sbbl %7, %1")  \
            ASM_LINE("sbbl %8, %2")  \
            ASM_LINE("sbbl %9, %3")  \
            ASM_LINE("sbbl $0, %4")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&r" (r3),            \
              "=&r" (c_out)          \
            : "4" (c_in),             \
              "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              "Irm" (s3),             \
              [blo] uECC_X86_CARRY_IN_CONSTRAINT (borrow_in)   \
            : "cc")                   \
    
#define uECC_vli_add2_carry_start(r0, r1, s0, s1, carry_out) \
    __asm__(ASM_LINE("addl %3, %0") \
            ASM_LINE("adcl %4, %1")  \
            ASM_LINE("setb %2")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "=q" (carry_out)       \
            : "Irm" (s0),             \
              "Irm" (s1)             \
            : "cc")

#define uECC_vli_add3_carry_start(r0, r1, r2, s0, s1, s2, carry_out)    \
    __asm__(ASM_LINE("addl %4, %0") \
            ASM_LINE("adcl %5, %1")  \
            ASM_LINE("adcl %6, %2")  \
            ASM_LINE("setb %3")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "=q" (carry_out)       \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2)              \
            : "cc")

#define uECC_vli_add3_carry_cont(r0, r1, r2, s0, s1, s2, carry_io) \
    __asm__(ASM_LINE("addb $-1, %3") \
            ASM_LINE("adcl %4, %0")      \
            ASM_LINE("adcl %5, %1")  \
            ASM_LINE("adcl %6, %2")  \
            ASM_LINE("setb %3")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&q" (carry_io)       \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2)             \
            : "cc")

#define uECC_vli_add3_carry_finish(r0, r1, r2, s0, s1, s2, c_lo, c_in, c_out) \
    __asm__(ASM_LINE("addb $-1, %[clo]") \
            ASM_LINE("adcl %5, %0")  \
            ASM_LINE("adcl %6, %1")  \
            ASM_LINE("adcl %7, %2")  \
            ASM_LINE("adcl $0, %3")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "=&r" (c_out)          \
            : "3" (c_in),            \
              "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              [clo] "q" (c_lo)        \
            : "cc")

#define uECC_vli_sub3_borrow_start(r0, r1, r2, s0, s1, s2, borrow_out) \
    __asm__(ASM_LINE("subl %4, %0")                                     \
            ASM_LINE("sbbl %5, %1")  \
            ASM_LINE("sbbl %6, %2")  \
            ASM_LINE("setc %3")      \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "=q" (borrow_out)      \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2)              \
            : "cc")

#define uECC_vli_sub3_borrow_cont(r0, r1, r2, s0, s1, s2, borrow_io) \
    __asm__(ASM_LINE("addb $-1, %3") \
            ASM_LINE("sbbl %4, %0")      \
            ASM_LINE("sbbl %5, %1")  \
            ASM_LINE("sbbl %6, %2")  \
            ASM_LINE("setc %3")      \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "+&q" (borrow_io)      \
            : "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2)              \
            : "cc")

#define uECC_vli_sub3_borrow_finish(r0, r1, r2, s0, s1, s2, borrow_in, c_in, c_out) \
    __asm__(ASM_LINE("addb $-1, %[blo]") \
            ASM_LINE("sbbl %5, %0")     \
            ASM_LINE("sbbl %6, %1")  \
            ASM_LINE("sbbl %7, %2")  \
            ASM_LINE("sbbl $0, %3")  \
            : "+&r" (r0),            \
              "+&r" (r1),            \
              "+&r" (r2),            \
              "=&r" (c_out)          \
            : "3" (c_in),             \
              "Irm" (s0),             \
              "Irm" (s1),             \
              "Irm" (s2),             \
              [blo] "q" (borrow_in)   \
            : "cc")                   \

static inline uECC_word_t uECC_vli_add12(uECC_word_t *r, const uECC_word_t *s,
                                         uECC_word_t carry_in)
{
    unsigned char carry;
    uECC_word_t carry_out;

    uECC_vli_add4_carry_start(r[0], r[1], r[2], r[3], s[0], s[1], s[2], s[3],
                              carry);
    uECC_vli_add4_carry_cont(r[4], r[5], r[6], r[7], s[4], s[5], s[6], s[7],
                             carry);
    uECC_vli_add4_carry_finish(r[8], r[9], r[10], r[11],
                               s[8], s[9], s[10], s[11], carry, carry_in,
                               carry_out);
    return carry_out;
}

static inline uECC_word_t uECC_vli_sub12(uECC_word_t *r, const uECC_word_t *s,
                                         uECC_word_t carry_in)
{
    unsigned char carry;
    uECC_word_t carry_out;

    uECC_vli_sub4_borrow_start(r[0], r[1], r[2], r[3], s[0], s[1], s[2], s[3],
                               carry);
    uECC_vli_sub4_borrow_cont(r[4], r[5], r[6], r[7], s[4], s[5], s[6], s[7],
                              carry);
    uECC_vli_sub4_borrow_finish(r[8], r[9], r[10], r[11],
                                s[8], s[9], s[10], s[11], carry, carry_in,
                                carry_out);
    return carry_out;
}

#endif /* __GNUC__ */
#endif /* !defined(__x86_64__) */
#endif /* defined __x86__ || defined(i386) || defined(__i386__) */

#endif /* UECC_ASM_X86_INC */
