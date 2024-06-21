/* Copyright 2015, Kenneth MacKay. Licensed under the BSD 2-clause license. */

#ifndef _UECC_CURVE_SPECIFIC_H_
#define _UECC_CURVE_SPECIFIC_H_

#define num_bytes_secp160r1 20
#define num_bytes_secp192r1 24
#define num_bytes_secp224r1 28
#define num_bytes_secp256r1 32
#define num_bytes_secp256k1 32
#define num_bytes_secp384r1 48
#define num_bytes_secp521r1 66

#if (uECC_WORD_SIZE == 1)

#define num_words_secp160r1 20
#define num_words_secp192r1 24
#define num_words_secp224r1 28
#define num_words_secp256r1 32
#define num_words_secp256k1 32
/* Note: secp384r1 or secp521r1 not supported on 8-bit platforms. */

#define BYTES_TO_WORDS_8(a, b, c, d, e, f, g, h) \
    0x##a, 0x##b, 0x##c, 0x##d, 0x##e, 0x##f, 0x##g, 0x##h
#define BYTES_TO_WORDS_4(a, b, c, d) 0x##a, 0x##b, 0x##c, 0x##d

#elif (uECC_WORD_SIZE == 4)

#define num_words_secp160r1 5
#define num_words_secp192r1 6
#define num_words_secp224r1 7
#define num_words_secp256r1 8
#define num_words_secp256k1 8
#define num_words_secp384r1 12
#define num_words_secp521r1 17

#define BYTES_TO_WORDS_8(a, b, c, d, e, f, g, h) 0x##d##c##b##a, 0x##h##g##f##e
#define BYTES_TO_WORDS_4(a, b, c, d) 0x##d##c##b##a

#elif (uECC_WORD_SIZE == 8)

#define num_words_secp160r1 3
#define num_words_secp192r1 3
#define num_words_secp224r1 4
#define num_words_secp256r1 4
#define num_words_secp256k1 4
#define num_words_secp384r1 6
#define num_words_secp521r1 9

#define BYTES_TO_WORDS_8(a, b, c, d, e, f, g, h) 0x##h##g##f##e##d##c##b##a##ull
#define BYTES_TO_WORDS_4(a, b, c, d) 0x##d##c##b##a##ull

#endif /* uECC_WORD_SIZE */

#if uECC_SUPPORTS_secp160r1 || uECC_SUPPORTS_secp192r1 || \
    uECC_SUPPORTS_secp224r1 || uECC_SUPPORTS_secp256r1 || \
    uECC_SUPPORTS_secp384r1 || uECC_SUPPORTS_secp521r1
static void double_jacobian_default(uECC_word_t * X1,
                                    uECC_word_t * Y1,
                                    uECC_word_t * Z1,
                                    uECC_Curve curve) {
    /* t1 = X, t2 = Y, t3 = Z */
    uECC_word_t t4[uECC_MAX_WORDS];
    uECC_word_t t5[uECC_MAX_WORDS];
    wordcount_t num_words = curve->num_words;

    if (uECC_vli_isZero(Z1, num_words)) {
        return;
    }

    uECC_vli_modSquare_fast(t4, Y1, curve);   /* t4 = y1^2 */
    uECC_vli_modMult_fast(t5, X1, t4, curve); /* t5 = x1*y1^2 = A */
    uECC_vli_modSquare_fast(t4, t4, curve);   /* t4 = y1^4 */
    uECC_vli_modMult_fast(Y1, Y1, Z1, curve); /* t2 = y1*z1 = z3 */
    uECC_vli_modSquare_fast(Z1, Z1, curve);   /* t3 = z1^2 */

    uECC_vli_modAdd(X1, X1, Z1, curve->p, num_words); /* t1 = x1 + z1^2 */
    uECC_vli_modAdd(Z1, Z1, Z1, curve->p, num_words); /* t3 = 2*z1^2 */
    uECC_vli_modSub(Z1, X1, Z1, curve->p, num_words); /* t3 = x1 - z1^2 */
    uECC_vli_modMult_fast(X1, X1, Z1, curve);                /* t1 = x1^2 - z1^4 */

    uECC_vli_modAdd(Z1, X1, X1, curve->p, num_words); /* t3 = 2*(x1^2 - z1^4) */
    uECC_vli_modAdd(X1, X1, Z1, curve->p, num_words); /* t1 = 3*(x1^2 - z1^4) */
    if (uECC_vli_testBit(X1, 0)) {
        uECC_word_t l_carry = uECC_vli_accumulate(X1, curve->p, num_words);
        uECC_vli_rshift1(X1, num_words);
        X1[num_words - 1] |= l_carry << (uECC_WORD_BITS - 1);
    } else {
        uECC_vli_rshift1(X1, num_words);
    }
    /* t1 = 3/2*(x1^2 - z1^4) = B */

    uECC_vli_modSquare_fast(Z1, X1, curve);                  /* t3 = B^2 */
    uECC_vli_modSub(Z1, Z1, t5, curve->p, num_words); /* t3 = B^2 - A */
    uECC_vli_modSub(Z1, Z1, t5, curve->p, num_words); /* t3 = B^2 - 2A = x3 */
    uECC_vli_modSub(t5, t5, Z1, curve->p, num_words); /* t5 = A - x3 */
    uECC_vli_modMult_fast(X1, X1, t5, curve);                /* t1 = B * (A - x3) */
    uECC_vli_modSub(t4, X1, t4, curve->p, num_words); /* t4 = B * (A - x3) - y1^4 = y3 */

    uECC_vli_set(X1, Z1, num_words);
    uECC_vli_set(Z1, Y1, num_words);
    uECC_vli_set(Y1, t4, num_words);
}

#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
/* Computes result = x^3 + ax + b. result must not overlap x. */
static void x_side_default(uECC_word_t *result, const uECC_word_t *x, uECC_Curve curve) {
    uECC_word_t _3[uECC_MAX_WORDS] = {3}; /* -a = 3 */
    wordcount_t num_words = curve->num_words;

    uECC_vli_modSquare_fast(result, x, curve);                             /* r = x^2 */
    uECC_vli_modSub(result, result, _3, curve->p, num_words);       /* r = x^2 - 3 */
    uECC_vli_modMult_fast(result, result, x, curve);                       /* r = x^3 - 3x */
    uECC_vli_modAdd(result, result, curve->b, curve->p, num_words); /* r = x^3 - 3x + b */
}
#endif
#endif /* uECC_SUPPORTS_secp... */

#if uECC_SUPPORT_COMPRESSED_POINT
#if uECC_SUPPORTS_secp160r1 || uECC_SUPPORTS_secp192r1 || \
    uECC_SUPPORTS_secp256r1 || uECC_SUPPORTS_secp256k1
/* Compute a = sqrt(a) (mod curve_p). */
static void mod_sqrt_default(uECC_word_t *a, uECC_Curve curve) {
    bitcount_t i;
    uECC_word_t p1[uECC_MAX_WORDS] = {1};
    uECC_word_t l_result[uECC_MAX_WORDS] = {1};
    wordcount_t num_words = curve->num_words;

    /* When curve->p == 3 (mod 4), we can compute
       sqrt(a) = a^((curve->p + 1) / 4) (mod curve->p). */
    uECC_vli_add(p1, curve->p, p1, num_words); /* p1 = curve_p + 1 */
    for (i = uECC_vli_numBits(p1, num_words) - 1; i > 1; --i) {
        uECC_vli_modSquare_fast(l_result, l_result, curve);
        if (uECC_vli_testBit(p1, i)) {
            uECC_vli_modMult_fast(l_result, l_result, a, curve);
        }
    }
    uECC_vli_set(a, l_result, num_words);
}
#endif /* uECC_SUPPORTS_secp... */
#endif /* uECC_SUPPORT_COMPRESSED_POINT */

#if uECC_SUPPORTS_secp160r1

#if (uECC_OPTIMIZATION_LEVEL > 0)
static void vli_mmod_fast_secp160r1(uECC_word_t *result, uECC_word_t *product);
#endif

static const struct uECC_Curve_t curve_secp160r1 = {
    num_words_secp160r1,
    num_bytes_secp160r1,
    161, /* num_n_bits */
    { BYTES_TO_WORDS_8(FF, FF, FF, 7F, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_4(FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(57, 22, 75, CA, D3, AE, 27, F9),
        BYTES_TO_WORDS_8(C8, F4, 01, 00, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(00, 00, 00, 00, 01, 00, 00, 00) },
    { BYTES_TO_WORDS_8(82, FC, CB, 13, B9, 8B, C3, 68),
        BYTES_TO_WORDS_8(89, 69, 64, 46, 28, 73, F5, 8E),
        BYTES_TO_WORDS_4(68, B5, 96, 4A),

        BYTES_TO_WORDS_8(32, FB, C5, 7A, 37, 51, 23, 04),
        BYTES_TO_WORDS_8(12, C9, DC, 59, 7D, 94, 68, 31),
        BYTES_TO_WORDS_4(55, 28, A6, 23) },
    { BYTES_TO_WORDS_8(45, FA, 65, C5, AD, D4, D4, 81),
        BYTES_TO_WORDS_8(9F, F8, AC, 65, 8B, 7A, BD, 54),
        BYTES_TO_WORDS_4(FC, BE, 97, 1C) },
    &double_jacobian_default,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_default,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_default,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
    &vli_mmod_fast_secp160r1
#endif
};

uECC_Curve uECC_secp160r1(void) { return &curve_secp160r1; }

#if (uECC_OPTIMIZATION_LEVEL > 0 && !asm_mmod_fast_secp160r1)
/* Computes result = product % curve_p
    see http://www.isys.uni-klu.ac.at/PDF/2001-0126-MT.pdf page 354

    Note that this only works if log2(omega) < log2(p) / 2 */
static void omega_mult_secp160r1(uECC_word_t *result, const uECC_word_t *right);
#if uECC_WORD_SIZE == 8
static void vli_mmod_fast_secp160r1(uECC_word_t *result, uECC_word_t *product) {
    uECC_word_t tmp[2 * num_words_secp160r1];
    uECC_word_t copy;

    uECC_vli_clear(tmp, num_words_secp160r1);
    uECC_vli_clear(tmp + num_words_secp160r1, num_words_secp160r1);

    omega_mult_secp160r1(tmp, product + num_words_secp160r1 - 1); /* (Rq, q) = q * c */

    product[num_words_secp160r1 - 1] &= 0xffffffff;
    copy = tmp[num_words_secp160r1 - 1];
    tmp[num_words_secp160r1 - 1] &= 0xffffffff;
    uECC_vli_add(result, product, tmp, num_words_secp160r1); /* (C, r) = r + q */
    uECC_vli_clear(product, num_words_secp160r1);
    tmp[num_words_secp160r1 - 1] = copy;
    omega_mult_secp160r1(product, tmp + num_words_secp160r1 - 1); /* Rq*c */
    uECC_vli_accumulate(result, product, num_words_secp160r1); /* (C1, r) = r + Rq*c */

    while (uECC_vli_cmp_unsafe(result, curve_secp160r1.p, num_words_secp160r1) > 0) {
        uECC_vli_accumulate_neg(result, curve_secp160r1.p, num_words_secp160r1);
    }
}

static void omega_mult_secp160r1(uint64_t *result, const uint64_t *right) {
    uint32_t carry;
    unsigned i;

    /* Multiply by (2^31 + 1). */
    carry = 0;
    for (i = 0; i < num_words_secp160r1; ++i) {
        uint64_t tmp = (right[i] >> 32) | (right[i + 1] << 32);
        result[i] = (tmp << 31) + tmp + carry;
        carry = (tmp >> 33) + (result[i] < tmp || (carry && result[i] == tmp));
    }
    result[i] = carry;
}
#else
static void vli_mmod_fast_secp160r1(uECC_word_t *result, uECC_word_t *product) {
    uECC_word_t tmp[2 * num_words_secp160r1];
    uECC_word_t carry;

    uECC_vli_clear(tmp, num_words_secp160r1);
    uECC_vli_clear(tmp + num_words_secp160r1, num_words_secp160r1);

    omega_mult_secp160r1(tmp, product + num_words_secp160r1); /* (Rq, q) = q * c */

    carry = uECC_vli_add(result, product, tmp, num_words_secp160r1); /* (C, r) = r + q */
    uECC_vli_clear(product, num_words_secp160r1);
    omega_mult_secp160r1(product, tmp + num_words_secp160r1); /* Rq*c */
    carry += uECC_vli_accumulate(result, product, num_words_secp160r1); /* (C1, r) = r + Rq*c */

    while (carry > 0) {
        --carry;
        uECC_vli_accumulate_neg(result, curve_secp160r1.p, num_words_secp160r1);
    }
    if (uECC_vli_cmp_unsafe(result, curve_secp160r1.p, num_words_secp160r1) > 0) {
        uECC_vli_accumulate_neg(result, curve_secp160r1.p, num_words_secp160r1);
    }
}
#endif

#if uECC_WORD_SIZE == 1
static void omega_mult_secp160r1(uint8_t *result, const uint8_t *right) {
    uint8_t carry;
    uint8_t i;

    /* Multiply by (2^31 + 1). */
    uECC_vli_set(result + 4, right, num_words_secp160r1); /* 2^32 */
    uECC_vli_rshift1(result + 4, num_words_secp160r1); /* 2^31 */
    result[3] = right[0] << 7; /* get last bit from shift */

    carry = uECC_vli_accumulate(result, right, num_words_secp160r1); /* 2^31 + 1 */
    for (i = num_words_secp160r1; carry; ++i) {
        uint16_t sum = (uint16_t)result[i] + carry;
        result[i] = (uint8_t)sum;
        carry = sum >> 8;
    }
}
#elif uECC_WORD_SIZE == 4
static void omega_mult_secp160r1(uint32_t *result, const uint32_t *right) {
    uint32_t carry;
    unsigned i;

    /* Multiply by (2^31 + 1). */
    uECC_vli_set(result + 1, right, num_words_secp160r1); /* 2^32 */
    uECC_vli_rshift1(result + 1, num_words_secp160r1); /* 2^31 */
    result[0] = right[0] << 31; /* get last bit from shift */

    carry = uECC_vli_accumulate(result, right, num_words_secp160r1); /* 2^31 + 1 */
    for (i = num_words_secp160r1; carry; ++i) {
        uint64_t sum = (uint64_t)result[i] + carry;
        result[i] = (uint32_t)sum;
        carry = sum >> 32;
    }
}
#endif /* uECC_WORD_SIZE */
#endif /* (uECC_OPTIMIZATION_LEVEL > 0 && !asm_mmod_fast_secp160r1) */

#endif /* uECC_SUPPORTS_secp160r1 */

#if uECC_SUPPORTS_secp192r1

#if (uECC_OPTIMIZATION_LEVEL > 0)
static void vli_mmod_fast_secp192r1(uECC_word_t *result, uECC_word_t *product);
#endif

static const struct uECC_Curve_t curve_secp192r1 = {
    num_words_secp192r1,
    num_bytes_secp192r1,
    192, /* num_n_bits */
    { BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FE, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(31, 28, D2, B4, B1, C9, 6B, 14),
        BYTES_TO_WORDS_8(36, F8, DE, 99, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(12, 10, FF, 82, FD, 0A, FF, F4),
        BYTES_TO_WORDS_8(00, 88, A1, 43, EB, 20, BF, 7C),
        BYTES_TO_WORDS_8(F6, 90, 30, B0, 0E, A8, 8D, 18),

        BYTES_TO_WORDS_8(11, 48, 79, 1E, A1, 77, F9, 73),
        BYTES_TO_WORDS_8(D5, CD, 24, 6B, ED, 11, 10, 63),
        BYTES_TO_WORDS_8(78, DA, C8, FF, 95, 2B, 19, 07) },
    { BYTES_TO_WORDS_8(B1, B9, 46, C1, EC, DE, B8, FE),
        BYTES_TO_WORDS_8(49, 30, 24, 72, AB, E9, A7, 0F),
        BYTES_TO_WORDS_8(E7, 80, 9C, E5, 19, 05, 21, 64) },
    &double_jacobian_default,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_default,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_default,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
    &vli_mmod_fast_secp192r1
#endif
};

uECC_Curve uECC_secp192r1(void) { return &curve_secp192r1; }

#if (uECC_OPTIMIZATION_LEVEL > 0)
/* Computes result = product % curve_p.
   See algorithm 5 and 6 from http://www.isys.uni-klu.ac.at/PDF/2001-0126-MT.pdf */
#if uECC_WORD_SIZE == 1
static void vli_mmod_fast_secp192r1(uint8_t *result, uint8_t *product) {
    uint8_t tmp[num_words_secp192r1];
    uint8_t carry;

    uECC_vli_set(result, product, num_words_secp192r1);

    uECC_vli_set(tmp, &product[24], num_words_secp192r1);
    carry = uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    tmp[0] = tmp[1] = tmp[2] = tmp[3] = tmp[4] = tmp[5] = tmp[6] = tmp[7] = 0;
    tmp[8] = product[24]; tmp[9] = product[25]; tmp[10] = product[26]; tmp[11] = product[27];
    tmp[12] = product[28]; tmp[13] = product[29]; tmp[14] = product[30]; tmp[15] = product[31];
    tmp[16] = product[32]; tmp[17] = product[33]; tmp[18] = product[34]; tmp[19] = product[35];
    tmp[20] = product[36]; tmp[21] = product[37]; tmp[22] = product[38]; tmp[23] = product[39];
    carry += uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    tmp[0] = tmp[8] = product[40];
    tmp[1] = tmp[9] = product[41];
    tmp[2] = tmp[10] = product[42];
    tmp[3] = tmp[11] = product[43];
    tmp[4] = tmp[12] = product[44];
    tmp[5] = tmp[13] = product[45];
    tmp[6] = tmp[14] = product[46];
    tmp[7] = tmp[15] = product[47];
    tmp[16] = tmp[17] = tmp[18] = tmp[19] = tmp[20] = tmp[21] = tmp[22] = tmp[23] = 0;
    carry += uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    while (carry || uECC_vli_cmp_unsafe(curve_secp192r1.p, result, num_words_secp192r1) != 1) {
        carry -= uECC_vli_accumulate_neg(result, curve_secp192r1.p, num_words_secp192r1);
    }
}
#elif uECC_WORD_SIZE == 4
static void vli_mmod_fast_secp192r1(uint32_t *result, uint32_t *product) {
    uint32_t tmp[num_words_secp192r1];
    int carry;

    uECC_vli_set(result, product, num_words_secp192r1);

    uECC_vli_set(tmp, &product[6], num_words_secp192r1);
    carry = uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    tmp[0] = tmp[1] = 0;
    tmp[2] = product[6];
    tmp[3] = product[7];
    tmp[4] = product[8];
    tmp[5] = product[9];
    carry += uECC_vli_accumulate192bit(result, tmp);

    tmp[0] = tmp[2] = product[10];
    tmp[1] = tmp[3] = product[11];
    tmp[4] = tmp[5] = 0;
    carry += uECC_vli_accumulate192bit(result, tmp);

    while (carry || uECC_vli_cmp_unsafe(curve_secp192r1.p, result, num_words_secp192r1) != 1) {
        carry -= uECC_vli_accumulate_neg192bit(result, curve_secp192r1.p);
    }
}
#else
static void vli_mmod_fast_secp192r1(uint64_t *result, uint64_t *product) {
    uint64_t tmp[num_words_secp192r1];
    int carry;

    uECC_vli_set(result, product, num_words_secp192r1);

    uECC_vli_set(tmp, &product[3], num_words_secp192r1);
    carry = (int)uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    tmp[0] = 0;
    tmp[1] = product[3];
    tmp[2] = product[4];
    carry += uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    tmp[0] = tmp[1] = product[5];
    tmp[2] = 0;
    carry += uECC_vli_accumulate(result, tmp, num_words_secp192r1);

    while (carry || uECC_vli_cmp_unsafe(curve_secp192r1.p, result, num_words_secp192r1) != 1) {
        carry -= uECC_vli_accumulate_neg(result, curve_secp192r1.p, num_words_secp192r1);
    }
}
#endif /* uECC_WORD_SIZE */
#endif /* (uECC_OPTIMIZATION_LEVEL > 0) */

#endif /* uECC_SUPPORTS_secp192r1 */

#if uECC_SUPPORTS_secp224r1

#if uECC_SUPPORT_COMPRESSED_POINT
static void mod_sqrt_secp224r1(uECC_word_t *a, uECC_Curve curve);
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
static void vli_mmod_fast_secp224r1(uECC_word_t *result, uECC_word_t *product);
#endif

static const struct uECC_Curve_t curve_secp224r1 = {
    num_words_secp224r1,
    num_bytes_secp224r1,
    224, /* num_n_bits */
    { BYTES_TO_WORDS_8(01, 00, 00, 00, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(00, 00, 00, 00, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_4(FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(3D, 2A, 5C, 5C, 45, 29, DD, 13),
        BYTES_TO_WORDS_8(3E, F0, B8, E0, A2, 16, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_4(FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(21, 1D, 5C, 11, D6, 80, 32, 34),
        BYTES_TO_WORDS_8(22, 11, C2, 56, D3, C1, 03, 4A),
        BYTES_TO_WORDS_8(B9, 90, 13, 32, 7F, BF, B4, 6B),
        BYTES_TO_WORDS_4(BD, 0C, 0E, B7),

        BYTES_TO_WORDS_8(34, 7E, 00, 85, 99, 81, D5, 44),
        BYTES_TO_WORDS_8(64, 47, 07, 5A, A0, 75, 43, CD),
        BYTES_TO_WORDS_8(E6, DF, 22, 4C, FB, 23, F7, B5),
        BYTES_TO_WORDS_4(88, 63, 37, BD) },
    { BYTES_TO_WORDS_8(B4, FF, 55, 23, 43, 39, 0B, 27),
        BYTES_TO_WORDS_8(BA, D8, BF, D7, B7, B0, 44, 50),
        BYTES_TO_WORDS_8(56, 32, 41, F5, AB, B3, 04, 0C),
        BYTES_TO_WORDS_4(85, 0A, 05, B4) },
    &double_jacobian_default,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_secp224r1,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_default,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
    &vli_mmod_fast_secp224r1
#endif
};

uECC_Curve uECC_secp224r1(void) { return &curve_secp224r1; }


#if uECC_SUPPORT_COMPRESSED_POINT
/* Routine 3.2.4 RS;  from http://www.nsa.gov/ia/_files/nist-routines.pdf */
static void mod_sqrt_secp224r1_rs(uECC_word_t *d1,
                                  uECC_word_t *e1,
                                  uECC_word_t *f1,
                                  const uECC_word_t *d0,
                                  const uECC_word_t *e0,
                                  const uECC_word_t *f0) {
    uECC_word_t t[num_words_secp224r1];

    uECC_vli_modSquare_fast(t, d0, &curve_secp224r1);                    /* t <-- d0 ^ 2 */
    uECC_vli_modMult_fast(e1, d0, e0, &curve_secp224r1);                 /* e1 <-- d0 * e0 */
    uECC_vli_modAdd(d1, t, f0, curve_secp224r1.p, num_words_secp224r1);  /* d1 <-- t  + f0 */
    uECC_vli_modAdd(e1, e1, e1, curve_secp224r1.p, num_words_secp224r1); /* e1 <-- e1 + e1 */
    uECC_vli_modMult_fast(f1, t, f0, &curve_secp224r1);                  /* f1 <-- t  * f0 */
    uECC_vli_modAdd(f1, f1, f1, curve_secp224r1.p, num_words_secp224r1); /* f1 <-- f1 + f1 */
    uECC_vli_modAdd(f1, f1, f1, curve_secp224r1.p, num_words_secp224r1); /* f1 <-- f1 + f1 */
}

/* Routine 3.2.5 RSS;  from http://www.nsa.gov/ia/_files/nist-routines.pdf */
static void mod_sqrt_secp224r1_rss(uECC_word_t *d1,
                                   uECC_word_t *e1,
                                   uECC_word_t *f1,
                                   const uECC_word_t *d0,
                                   const uECC_word_t *e0,
                                   const uECC_word_t *f0,
                                   const bitcount_t j) {
    bitcount_t i;

    uECC_vli_set(d1, d0, num_words_secp224r1); /* d1 <-- d0 */
    uECC_vli_set(e1, e0, num_words_secp224r1); /* e1 <-- e0 */
    uECC_vli_set(f1, f0, num_words_secp224r1); /* f1 <-- f0 */
    for (i = 1; i <= j; i++) {
        mod_sqrt_secp224r1_rs(d1, e1, f1, d1, e1, f1); /* RS (d1,e1,f1,d1,e1,f1) */
    }
}

/* Routine 3.2.6 RM;  from http://www.nsa.gov/ia/_files/nist-routines.pdf */
static void mod_sqrt_secp224r1_rm(uECC_word_t *d2,
                                  uECC_word_t *e2,
                                  uECC_word_t *f2,
                                  const uECC_word_t *c,
                                  const uECC_word_t *d0,
                                  const uECC_word_t *e0,
                                  const uECC_word_t *d1,
                                  const uECC_word_t *e1) {
    uECC_word_t t1[num_words_secp224r1];
    uECC_word_t t2[num_words_secp224r1];

    uECC_vli_modMult_fast(t1, e0, e1, &curve_secp224r1); /* t1 <-- e0 * e1 */
    uECC_vli_modMult_fast(t1, t1, c, &curve_secp224r1);  /* t1 <-- t1 * c */
    /* t1 <-- p  - t1 */
    uECC_vli_modSub(t1, curve_secp224r1.p, t1, curve_secp224r1.p, num_words_secp224r1);
    uECC_vli_modMult_fast(t2, d0, d1, &curve_secp224r1);                 /* t2 <-- d0 * d1 */
    uECC_vli_modAdd(t2, t2, t1, curve_secp224r1.p, num_words_secp224r1); /* t2 <-- t2 + t1 */
    uECC_vli_modMult_fast(t1, d0, e1, &curve_secp224r1);                 /* t1 <-- d0 * e1 */
    uECC_vli_modMult_fast(e2, d1, e0, &curve_secp224r1);                 /* e2 <-- d1 * e0 */
    uECC_vli_modAdd(e2, e2, t1, curve_secp224r1.p, num_words_secp224r1); /* e2 <-- e2 + t1 */
    uECC_vli_modSquare_fast(f2, e2, &curve_secp224r1);                   /* f2 <-- e2^2 */
    uECC_vli_modMult_fast(f2, f2, c, &curve_secp224r1);                  /* f2 <-- f2 * c */
    /* f2 <-- p  - f2 */
    uECC_vli_modSub(f2, curve_secp224r1.p, f2, curve_secp224r1.p, num_words_secp224r1);
    uECC_vli_set(d2, t2, num_words_secp224r1); /* d2 <-- t2 */
}

/* Routine 3.2.7 RP;  from http://www.nsa.gov/ia/_files/nist-routines.pdf */
static void mod_sqrt_secp224r1_rp(uECC_word_t *d1,
                                  uECC_word_t *e1,
                                  uECC_word_t *f1,
                                  const uECC_word_t *c,
                                  const uECC_word_t *r) {
    wordcount_t i;
    wordcount_t pow2i = 1;
    uECC_word_t d0[num_words_secp224r1];
    uECC_word_t e0[num_words_secp224r1] = {1}; /* e0 <-- 1 */
    uECC_word_t f0[num_words_secp224r1];

    uECC_vli_set(d0, r, num_words_secp224r1); /* d0 <-- r */
    /* f0 <-- p  - c */
    uECC_vli_modSub(f0, curve_secp224r1.p, c, curve_secp224r1.p, num_words_secp224r1);
    for (i = 0; i <= 6; i++) {
        mod_sqrt_secp224r1_rss(d1, e1, f1, d0, e0, f0, pow2i); /* RSS (d1,e1,f1,d0,e0,f0,2^i) */
        mod_sqrt_secp224r1_rm(d1, e1, f1, c, d1, e1, d0, e0);  /* RM (d1,e1,f1,c,d1,e1,d0,e0) */
        uECC_vli_set(d0, d1, num_words_secp224r1); /* d0 <-- d1 */
        uECC_vli_set(e0, e1, num_words_secp224r1); /* e0 <-- e1 */
        uECC_vli_set(f0, f1, num_words_secp224r1); /* f0 <-- f1 */
        pow2i *= 2;
    }
}

/* Compute a = sqrt(a) (mod curve_p). */
/* Routine 3.2.8 mp_mod_sqrt_224; from http://www.nsa.gov/ia/_files/nist-routines.pdf */
static void mod_sqrt_secp224r1(uECC_word_t *a, uECC_Curve curve) {
    bitcount_t i;
    uECC_word_t e1[num_words_secp224r1];
    uECC_word_t f1[num_words_secp224r1];
    uECC_word_t d0[num_words_secp224r1];
    uECC_word_t e0[num_words_secp224r1];
    uECC_word_t f0[num_words_secp224r1];
    uECC_word_t d1[num_words_secp224r1];

    /* s = a; using constant instead of random value */
    mod_sqrt_secp224r1_rp(d0, e0, f0, a, a);           /* RP (d0, e0, f0, c, s) */
    mod_sqrt_secp224r1_rs(d1, e1, f1, d0, e0, f0);     /* RS (d1, e1, f1, d0, e0, f0) */
    for (i = 1; i <= 95; i++) {
        uECC_vli_set(d0, d1, num_words_secp224r1);          /* d0 <-- d1 */
        uECC_vli_set(e0, e1, num_words_secp224r1);          /* e0 <-- e1 */
        uECC_vli_set(f0, f1, num_words_secp224r1);          /* f0 <-- f1 */
        mod_sqrt_secp224r1_rs(d1, e1, f1, d0, e0, f0); /* RS (d1, e1, f1, d0, e0, f0) */
        if (uECC_vli_isZero(d1, num_words_secp224r1)) {     /* if d1 == 0 */
                break;
        }
    }
    uECC_vli_modInv(f1, e0, curve_secp224r1.p, num_words_secp224r1); /* f1 <-- 1 / e0 */
    uECC_vli_modMult_fast(a, d0, f1, &curve_secp224r1);              /* a  <-- d0 / e0 */
}
#endif /* uECC_SUPPORT_COMPRESSED_POINT */

#if (uECC_OPTIMIZATION_LEVEL > 0)
/* Computes result = product % curve_p
   from http://www.nsa.gov/ia/_files/nist-routines.pdf */
#if uECC_WORD_SIZE == 1
static void vli_mmod_fast_secp224r1(uint8_t *result, uint8_t *product) {
    uint8_t tmp[num_words_secp224r1];
    int8_t carry;

    /* t */
    uECC_vli_set(result, product, num_words_secp224r1);

    /* s1 */
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;
    tmp[4] = tmp[5] = tmp[6] = tmp[7] = 0;
    tmp[8] = tmp[9] = tmp[10] = tmp[11] = 0;
    tmp[12] = product[28]; tmp[13] = product[29]; tmp[14] = product[30]; tmp[15] = product[31];
    tmp[16] = product[32]; tmp[17] = product[33]; tmp[18] = product[34]; tmp[19] = product[35];
    tmp[20] = product[36]; tmp[21] = product[37]; tmp[22] = product[38]; tmp[23] = product[39];
    tmp[24] = product[40]; tmp[25] = product[41]; tmp[26] = product[42]; tmp[27] = product[43];
    carry = uECC_vli_accumulate(result, tmp, num_words_secp224r1);

    /* s2 */
    tmp[12] = product[44]; tmp[13] = product[45]; tmp[14] = product[46]; tmp[15] = product[47];
    tmp[16] = product[48]; tmp[17] = product[49]; tmp[18] = product[50]; tmp[19] = product[51];
    tmp[20] = product[52]; tmp[21] = product[53]; tmp[22] = product[54]; tmp[23] = product[55];
    tmp[24] = tmp[25] = tmp[26] = tmp[27] = 0;
    carry += uECC_vli_accumulate(result, tmp, num_words_secp224r1);

    /* d1 */
    tmp[0]  = product[28]; tmp[1]  = product[29]; tmp[2]  = product[30]; tmp[3]  = product[31];
    tmp[4]  = product[32]; tmp[5]  = product[33]; tmp[6]  = product[34]; tmp[7]  = product[35];
    tmp[8]  = product[36]; tmp[9]  = product[37]; tmp[10] = product[38]; tmp[11] = product[39];
    tmp[12] = product[40]; tmp[13] = product[41]; tmp[14] = product[42]; tmp[15] = product[43];
    tmp[16] = product[44]; tmp[17] = product[45]; tmp[18] = product[46]; tmp[19] = product[47];
    tmp[20] = product[48]; tmp[21] = product[49]; tmp[22] = product[50]; tmp[23] = product[51];
    tmp[24] = product[52]; tmp[25] = product[53]; tmp[26] = product[54]; tmp[27] = product[55];
    carry -= uECC_vli_accumulate_neg(result, tmp, num_words_secp224r1);

    /* d2 */
    tmp[0]  = product[44]; tmp[1]  = product[45]; tmp[2]  = product[46]; tmp[3]  = product[47];
    tmp[4]  = product[48]; tmp[5]  = product[49]; tmp[6]  = product[50]; tmp[7]  = product[51];
    tmp[8]  = product[52]; tmp[9]  = product[53]; tmp[10] = product[54]; tmp[11] = product[55];
    tmp[12] = tmp[13] = tmp[14] = tmp[15] = 0;
    tmp[16] = tmp[17] = tmp[18] = tmp[19] = 0;
    tmp[20] = tmp[21] = tmp[22] = tmp[23] = 0;
    tmp[24] = tmp[25] = tmp[26] = tmp[27] = 0;
    carry -= uECC_vli_accumulate_neg(result, tmp, num_words_secp224r1);

    if (carry < 0) {
        do {
            carry += uECC_vli_accumulate(result, curve_secp224r1.p, num_words_secp224r1);
        } while (carry < 0);
    } else {
        while (carry || uECC_vli_cmp_unsafe(curve_secp224r1.p, result, num_words_secp224r1) != 1) {
            carry -= uECC_vli_accumulate_neg(result, curve_secp224r1.p, num_words_secp224r1);
        }
    }
}
#elif uECC_WORD_SIZE == 4
static void vli_mmod_fast_secp224r1(uint32_t *result, uint32_t *product)
{
    uint32_t tmp[num_words_secp224r1];
    int carry;

    /* t */
    uECC_vli_set(result, product, num_words_secp224r1);

    /* s1 */
    tmp[0] = tmp[1] = tmp[2] = 0;
    tmp[3] = product[7];
    tmp[4] = product[8];
    tmp[5] = product[9];
    tmp[6] = product[10];
    /* Use 128-bit accumulation as there are only 4 * last terms set. */
    carry = uECC_vli_accumulate128bit(result + 3, tmp + 3);

    /* s2 */
    tmp[3] = product[11];
    tmp[4] = product[12];
    tmp[5] = product[13];
    tmp[6] = 0;
    carry += uECC_vli_accumulate128bit(result + 3, tmp + 3);

    /* d1 */
    tmp[0] = product[7];
    tmp[1] = product[8];
    tmp[2] = product[9];
    tmp[3] = product[10];
    tmp[4] = product[11];
    tmp[5] = product[12];
    tmp[6] = product[13];
    carry -= uECC_vli_accumulate_neg224bit(result, tmp);

    /* d2 */
    tmp[0] = product[11];
    tmp[1] = product[12];
    tmp[2] = product[13];
    tmp[3] = tmp[4] = tmp[5] = tmp[6] = 0;
    carry -= uECC_vli_accumulate_neg224bit(result, tmp);

    if (carry < 0) {
        do {
            carry += uECC_vli_accumulate224bit(result, curve_secp224r1.p);
        } while (carry < 0);
    } else {
        while (carry || uECC_vli_cmp_unsafe(curve_secp224r1.p, result, num_words_secp224r1) != 1) {
            carry -= uECC_vli_accumulate_neg224bit(result, curve_secp224r1.p);
        }
    }
}
#else
static void vli_mmod_fast_secp224r1(uint64_t *result, uint64_t *product)
{
    uint64_t tmp[num_words_secp224r1];
    int carry = 0;

    /* t */
    uECC_vli_set(result, product, num_words_secp224r1);
    result[num_words_secp224r1 - 1] &= 0xffffffff;

    /* s1 */
    tmp[0] = 0;
    tmp[1] = product[3] & 0xffffffff00000000ull;
    tmp[2] = product[4];
    tmp[3] = product[5] & 0xffffffff;
    uECC_vli_accumulate(result, tmp, num_words_secp224r1);

    /* s2 */
    tmp[1] = product[5] & 0xffffffff00000000ull;
    tmp[2] = product[6];
    tmp[3] = 0;
    uECC_vli_accumulate(result, tmp, num_words_secp224r1);

    /* d1 */
    tmp[0] = (product[3] >> 32) | (product[4] << 32);
    tmp[1] = (product[4] >> 32) | (product[5] << 32);
    tmp[2] = (product[5] >> 32) | (product[6] << 32);
    tmp[3] = product[6] >> 32;
    carry -= uECC_vli_accumulate_neg(result, tmp, num_words_secp224r1);

    /* d2 */
    tmp[0] = (product[5] >> 32) | (product[6] << 32);
    tmp[1] = product[6] >> 32;
    tmp[2] = tmp[3] = 0;
    carry -= uECC_vli_accumulate_neg(result, tmp, num_words_secp224r1);

    if (carry < 0) {
        do {
            carry += uECC_vli_accumulate(result, curve_secp224r1.p, num_words_secp224r1);
        } while (carry < 0);
    } else {
        while (uECC_vli_cmp_unsafe(curve_secp224r1.p, result, num_words_secp224r1) != 1) {
            uECC_vli_accumulate_neg(result, curve_secp224r1.p, num_words_secp224r1);
        }
    }
}
#endif /* uECC_WORD_SIZE */
#endif /* (uECC_OPTIMIZATION_LEVEL > 0) */

#endif /* uECC_SUPPORTS_secp224r1 */

#if uECC_SUPPORTS_secp256r1

#if (uECC_OPTIMIZATION_LEVEL > 0)
static void vli_mmod_fast_secp256r1(uECC_word_t *result, uECC_word_t *product);
#endif

static const struct uECC_Curve_t curve_secp256r1 = {
    num_words_secp256r1,
    num_bytes_secp256r1,
    256, /* num_n_bits */
    { BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(00, 00, 00, 00, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(01, 00, 00, 00, FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(51, 25, 63, FC, C2, CA, B9, F3),
        BYTES_TO_WORDS_8(84, 9E, 17, A7, AD, FA, E6, BC),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(00, 00, 00, 00, FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(96, C2, 98, D8, 45, 39, A1, F4),
        BYTES_TO_WORDS_8(A0, 33, EB, 2D, 81, 7D, 03, 77),
        BYTES_TO_WORDS_8(F2, 40, A4, 63, E5, E6, BC, F8),
        BYTES_TO_WORDS_8(47, 42, 2C, E1, F2, D1, 17, 6B),

        BYTES_TO_WORDS_8(F5, 51, BF, 37, 68, 40, B6, CB),
        BYTES_TO_WORDS_8(CE, 5E, 31, 6B, 57, 33, CE, 2B),
        BYTES_TO_WORDS_8(16, 9E, 0F, 7C, 4A, EB, E7, 8E),
        BYTES_TO_WORDS_8(9B, 7F, 1A, FE, E2, 42, E3, 4F) },
    { BYTES_TO_WORDS_8(4B, 60, D2, 27, 3E, 3C, CE, 3B),
        BYTES_TO_WORDS_8(F6, B0, 53, CC, B0, 06, 1D, 65),
        BYTES_TO_WORDS_8(BC, 86, 98, 76, 55, BD, EB, B3),
        BYTES_TO_WORDS_8(E7, 93, 3A, AA, D8, 35, C6, 5A) },
    &double_jacobian_default,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_default,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_default,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
    &vli_mmod_fast_secp256r1
#endif
};

uECC_Curve uECC_secp256r1(void) { return &curve_secp256r1; }

#if (uECC_OPTIMIZATION_LEVEL > 0 && !asm_mmod_fast_secp256r1)
/* Computes result = product % curve_p
   from http://www.nsa.gov/ia/_files/nist-routines.pdf */
#if uECC_WORD_SIZE == 1
static void vli_mmod_fast_secp256r1(uint8_t *result, uint8_t *product) {
    uint8_t tmp[num_words_secp256r1];
    int8_t carry;

    /* t */
    uECC_vli_set(result, product, num_words_secp256r1);

    /* s1 */
    tmp[0] = tmp[1] = tmp[2] = tmp[3] = 0;
    tmp[4] = tmp[5] = tmp[6] = tmp[7] = 0;
    tmp[8] = tmp[9] = tmp[10] = tmp[11] = 0;
    tmp[12] = product[44]; tmp[13] = product[45]; tmp[14] = product[46]; tmp[15] = product[47];
    tmp[16] = product[48]; tmp[17] = product[49]; tmp[18] = product[50]; tmp[19] = product[51];
    tmp[20] = product[52]; tmp[21] = product[53]; tmp[22] = product[54]; tmp[23] = product[55];
    tmp[24] = product[56]; tmp[25] = product[57]; tmp[26] = product[58]; tmp[27] = product[59];
    tmp[28] = product[60]; tmp[29] = product[61]; tmp[30] = product[62]; tmp[31] = product[63];
    carry = uECC_vli_accumulate256bit(tmp, tmp);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s2 */
    tmp[12] = product[48]; tmp[13] = product[49]; tmp[14] = product[50]; tmp[15] = product[51];
    tmp[16] = product[52]; tmp[17] = product[53]; tmp[18] = product[54]; tmp[19] = product[55];
    tmp[20] = product[56]; tmp[21] = product[57]; tmp[22] = product[58]; tmp[23] = product[59];
    tmp[24] = product[60]; tmp[25] = product[61]; tmp[26] = product[62]; tmp[27] = product[63];
    tmp[28] = tmp[29] = tmp[30] = tmp[31] = 0;
    carry += uECC_vli_accumulate256bit(tmp, tmp);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s3 */
    tmp[0] = product[32]; tmp[1] = product[33]; tmp[2] = product[34]; tmp[3] = product[35];
    tmp[4] = product[36]; tmp[5] = product[37]; tmp[6] = product[38]; tmp[7] = product[39];
    tmp[8] = product[40]; tmp[9] = product[41]; tmp[10] = product[42]; tmp[11] = product[43];
    tmp[12] = tmp[13] = tmp[14] = tmp[15] = 0;
    tmp[16] = tmp[17] = tmp[18] = tmp[19] = 0;
    tmp[20] = tmp[21] = tmp[22] = tmp[23] = 0;
    tmp[24] = product[56]; tmp[25] = product[57]; tmp[26] = product[58]; tmp[27] = product[59];
    tmp[28] = product[60]; tmp[29] = product[61]; tmp[30] = product[62]; tmp[31] = product[63];
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s4 */
    tmp[0] = product[36]; tmp[1] = product[37]; tmp[2] = product[38]; tmp[3] = product[39];
    tmp[4] = product[40]; tmp[5] = product[41]; tmp[6] = product[42]; tmp[7] = product[43];
    tmp[8] = product[44]; tmp[9] = product[45]; tmp[10] = product[46]; tmp[11] = product[47];
    tmp[12] = product[52]; tmp[13] = product[53]; tmp[14] = product[54]; tmp[15] = product[55];
    tmp[16] = product[56]; tmp[17] = product[57]; tmp[18] = product[58]; tmp[19] = product[59];
    tmp[20] = product[60]; tmp[21] = product[61]; tmp[22] = product[62]; tmp[23] = product[63];
    tmp[24] = product[52]; tmp[25] = product[53]; tmp[26] = product[54]; tmp[27] = product[55];
    tmp[28] = product[32]; tmp[29] = product[33]; tmp[30] = product[34]; tmp[31] = product[35];
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* d1 */
    tmp[0] = product[44]; tmp[1] = product[45]; tmp[2] = product[46]; tmp[3] = product[47];
    tmp[4] = product[48]; tmp[5] = product[49]; tmp[6] = product[50]; tmp[7] = product[51];
    tmp[8] = product[52]; tmp[9] = product[53]; tmp[10] = product[54]; tmp[11] = product[55];
    tmp[12] = tmp[13] = tmp[14] = tmp[15] = 0;
    tmp[16] = tmp[17] = tmp[18] = tmp[19] = 0;
    tmp[20] = tmp[21] = tmp[22] = tmp[23] = 0;
    tmp[24] = product[32]; tmp[25] = product[33]; tmp[26] = product[34]; tmp[27] = product[35];
    tmp[28] = product[40]; tmp[29] = product[41]; tmp[30] = product[42]; tmp[31] = product[43];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d2 */
    tmp[0] = product[48]; tmp[1] = product[49]; tmp[2] = product[50]; tmp[3] = product[51];
    tmp[4] = product[52]; tmp[5] = product[53]; tmp[6] = product[54]; tmp[7] = product[55];
    tmp[8] = product[56]; tmp[9] = product[57]; tmp[10] = product[58]; tmp[11] = product[59];
    tmp[12] = product[60]; tmp[13] = product[61]; tmp[14] = product[62]; tmp[15] = product[63];
    tmp[16] = tmp[17] = tmp[18] = tmp[19] = 0;
    tmp[20] = tmp[21] = tmp[22] = tmp[23] = 0;
    tmp[24] = product[36]; tmp[25] = product[37]; tmp[26] = product[38]; tmp[27] = product[39];
    tmp[28] = product[44]; tmp[29] = product[45]; tmp[30] = product[46]; tmp[31] = product[47];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d3 */
    tmp[0] = product[52]; tmp[1] = product[53]; tmp[2] = product[54]; tmp[3] = product[55];
    tmp[4] = product[56]; tmp[5] = product[57]; tmp[6] = product[58]; tmp[7] = product[59];
    tmp[8] = product[60]; tmp[9] = product[61]; tmp[10] = product[62]; tmp[11] = product[63];
    tmp[12] = product[32]; tmp[13] = product[33]; tmp[14] = product[34]; tmp[15] = product[35];
    tmp[16] = product[36]; tmp[17] = product[37]; tmp[18] = product[38]; tmp[19] = product[39];
    tmp[20] = product[40]; tmp[21] = product[41]; tmp[22] = product[42]; tmp[23] = product[43];
    tmp[24] = tmp[25] = tmp[26] = tmp[27] = 0;
    tmp[28] = product[48]; tmp[29] = product[49]; tmp[30] = product[50]; tmp[31] = product[51];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d4 */
    tmp[0] = product[56]; tmp[1] = product[57]; tmp[2] = product[58]; tmp[3] = product[59];
    tmp[4] = product[60]; tmp[5] = product[61]; tmp[6] = product[62]; tmp[7] = product[63];
    tmp[8] = tmp[9] = tmp[10] = tmp[11] = 0;
    tmp[12] = product[36]; tmp[13] = product[37]; tmp[14] = product[38]; tmp[15] = product[39];
    tmp[16] = product[40]; tmp[17] = product[41]; tmp[18] = product[42]; tmp[19] = product[43];
    tmp[20] = product[44]; tmp[21] = product[45]; tmp[22] = product[46]; tmp[23] = product[47];
    tmp[24] = tmp[25] = tmp[26] = tmp[27] = 0;
    tmp[28] = product[52]; tmp[29] = product[53]; tmp[30] = product[54]; tmp[31] = product[55];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    if (carry < 0) {
        do {
            carry += uECC_vli_accumulate256bit(result, curve_secp256r1.p);
        } while (carry < 0);
    } else {
        while (carry || uECC_vli_cmp_unsafe(curve_secp256r1.p, result, num_words_secp256r1) != 1) {
            carry -= uECC_vli_accumulate_neg256bit(result, curve_secp256r1.p);
        }
    }
}
#elif uECC_WORD_SIZE == 4
static void vli_mmod_fast_secp256r1(uint32_t *result, uint32_t *product) {
    uint32_t tmp[num_words_secp256r1];
    int carry;

    /* t */
    uECC_vli_set(result, product, num_words_secp256r1);

    /* s1 */
    tmp[0] = tmp[1] = tmp[2] = 0;
    tmp[3] = product[11];
    tmp[4] = product[12];
    tmp[5] = product[13];
    tmp[6] = product[14];
    tmp[7] = product[15];
    carry = uECC_vli_accumulate256bit(tmp, tmp);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s2 */
    tmp[3] = product[12];
    tmp[4] = product[13];
    tmp[5] = product[14];
    tmp[6] = product[15];
    tmp[7] = 0;
    carry += uECC_vli_accumulate256bit(tmp, tmp);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s3 */
    tmp[0] = product[8];
    tmp[1] = product[9];
    tmp[2] = product[10];
    tmp[3] = tmp[4] = tmp[5] = 0;
    tmp[6] = product[14];
    tmp[7] = product[15];
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s4 */
    tmp[0] = product[9];
    tmp[1] = product[10];
    tmp[2] = product[11];
    tmp[3] = product[13];
    tmp[4] = product[14];
    tmp[5] = product[15];
    tmp[6] = product[13];
    tmp[7] = product[8];
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* d1 */
    tmp[0] = product[11];
    tmp[1] = product[12];
    tmp[2] = product[13];
    tmp[3] = tmp[4] = tmp[5] = 0;
    tmp[6] = product[8];
    tmp[7] = product[10];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d2 */
    tmp[0] = product[12];
    tmp[1] = product[13];
    tmp[2] = product[14];
    tmp[3] = product[15];
    tmp[4] = tmp[5] = 0;
    tmp[6] = product[9];
    tmp[7] = product[11];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d3 */
    tmp[0] = product[13];
    tmp[1] = product[14];
    tmp[2] = product[15];
    tmp[3] = product[8];
    tmp[4] = product[9];
    tmp[5] = product[10];
    tmp[6] = 0;
    tmp[7] = product[12];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d4 */
    tmp[0] = product[14];
    tmp[1] = product[15];
    tmp[2] = 0;
    tmp[3] = product[9];
    tmp[4] = product[10];
    tmp[5] = product[11];
    tmp[6] = 0;
    tmp[7] = product[13];
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* Perform multiple additions or subtractions of the prime.
       Combination of constants and carry, ~carry and -carry is used
       to build multiplies of the prime. */
    if (carry < 0)
    {
        tmp[0] = (unsigned int) (int) carry;
        tmp[1] = 0xffffffffu;
        tmp[2] = 0xffffffffu;
        tmp[3] = ~tmp[0];
        tmp[4] = 0;
        tmp[5] = 0;
        tmp[6] = (unsigned int)-carry;
        tmp[7] = tmp[0];
        carry = uECC_vli_accumulate256bit(result, tmp) - 1;
    }
    else if (carry > 0)
    {
        tmp[0] = (unsigned int) (int) -carry;
        tmp[1] = 0xffffffffu;
        tmp[2] = 0xffffffffu;
        tmp[3] = ~tmp[0];
        tmp[4] = 0;
        tmp[5] = 0;
        tmp[6] = (unsigned int)carry;
        tmp[7] = tmp[0];
        carry = 1 - uECC_vli_accumulate_neg256bit(result, tmp);
    }

    /* Typically values are now normalized but it is possible that they are
       off by multiple(s) of p. Normalize one multiple of p at a time. */
    while (carry != 0 ||
           uECC_vli_cmp_unsafe(curve_secp256r1.p, result, num_words_secp256r1) != 1)
    {
        if (carry < 0) {
            carry += uECC_vli_accumulate256bit(result, curve_secp256r1.p);
        } else {
            carry -= uECC_vli_accumulate_neg256bit(result, curve_secp256r1.p);
        }
    }
}
#else

#ifdef __GNUC__
/* Tell compiler this function is a hot spot. */
void vli_mmod_fast_secp256r1(uint64_t *result, uint64_t *product)
    __attribute__((__hot__));
#endif

void vli_mmod_fast_secp256r1(uint64_t *result, uint64_t *product) {
    uint64_t tmp[num_words_secp256r1];
    int carry;

#ifdef uECC_USE_SLP_ECDSA_PREDUCE_512_TO_256
    /* Use the pre-existing reduction function from SL. */
    void SLP_ECDSA_preduce512_to_256(uECC_word_t *product,
                                     uECC_word_t *result);
    SLP_ECDSA_preduce512_to_256(product, result);
    return;
#endif /* uECC_USE_SLP_ECDSA_PREDUCE_512_TO_256 */

#if defined __x86_64__ && !defined uECC_NO_X86_64_ASM
    /* t */
    uECC_vli_set(result, product, num_words_secp256r1);

    /* s1 */
    tmp[0] = 0;
    tmp[1] = product[5] & 0xffffffff00000000ull;
    tmp[2] = product[6];
    tmp[3] = product[7];
    UECC_VLI_ACCUMULATE4(tmp, tmp, 0, carry);
    UECC_VLI_ACCUMULATE4(result, tmp, carry, carry);

    /* s2 */
    tmp[1] = product[6] << 32;
    tmp[2] = (product[6] >> 32) | (product[7] << 32);
    tmp[3] = product[7] >> 32;
    UECC_VLI_ACCUMULATE4(tmp, tmp, carry, carry);
    UECC_VLI_ACCUMULATE4(result, tmp, carry, carry);

    /* s3 */
    tmp[0] = product[4];
    tmp[1] = product[5] & 0xffffffff;
    tmp[2] = 0;
    tmp[3] = product[7];
    UECC_VLI_ACCUMULATE4(result, tmp, carry, carry);

    /* s4 */
    tmp[0] = (product[4] >> 32) | (product[5] << 32);
    tmp[1] = (product[5] >> 32) | (product[6] & 0xffffffff00000000ull);
    tmp[2] = product[7];
    tmp[3] = (product[6] >> 32) | (product[4] << 32);
    UECC_VLI_ACCUMULATE4(result, tmp, carry, carry);

    /* d1 */
    tmp[0] = (product[5] >> 32) | (product[6] << 32);
    tmp[1] = (product[6] >> 32);
    tmp[2] = 0;
    tmp[3] = (product[4] & 0xffffffff) | (product[5] << 32);
    UECC_VLI_ACCUMULATE4_NEG(result, tmp, carry, carry);

    /* d2 */
    tmp[0] = product[6];
    tmp[1] = product[7];
    tmp[2] = 0;
    tmp[3] = (product[4] >> 32) | (product[5] & 0xffffffff00000000ull);
    UECC_VLI_ACCUMULATE4_NEG(result, tmp, carry, carry);

    /* d3 */
    tmp[0] = (product[6] >> 32) | (product[7] << 32);
    tmp[1] = (product[7] >> 32) | (product[4] << 32);
    tmp[2] = (product[4] >> 32) | (product[5] << 32);
    tmp[3] = (product[6] << 32);
    UECC_VLI_ACCUMULATE4_NEG(result, tmp, carry, carry);

    /* d4 */
    tmp[0] = product[7];
    tmp[1] = product[4] & 0xffffffff00000000ull;
    tmp[2] = product[5];
    tmp[3] = product[6] & 0xffffffff00000000ull;
    UECC_VLI_ACCUMULATE4_NEG(result, tmp, carry, carry);

    ASSERT(carry >= -4 && carry <= 6);

    /* Table containing small multiples of NIST P-256's p. */
    static const uECC_word_t pmult[7][4] =
    {
        {
            0, 0, 0, 0
        },
        {
            0xfffffffffffffffful, 0x00000000fffffffful,
            0x0000000000000000ul, 0xffffffff00000001ul,
        },
        {
            0xfffffffffffffffeul, 0x00000001fffffffful,
            0x0000000000000000ul, 0xfffffffe00000002ul
        },
        {
            0xfffffffffffffffdul, 0x00000002fffffffful,
            0x0000000000000000ul, 0xfffffffd00000003ul
        },
        {
            0xfffffffffffffffcul, 0x00000003fffffffful,
            0x0000000000000000ul, 0xfffffffc00000004ul
        },
        {
            0xfffffffffffffffbul, 0x00000004fffffffful,
            0x0000000000000000ul, 0xfffffffb00000005ul
        },
        {
            0xfffffffffffffffaul, 0x00000005fffffffful,
            0x0000000000000000ul, 0xfffffffa00000006ul
        }
    };

    if (carry < 0)
    {
        tmp[0] = (unsigned long long) (long long) carry; /* the same than
                                                            pmult[-carry][0] */
        tmp[1] = pmult[-carry][1];
        tmp[2] = 0; /* the same than pmult[-carry][2] */
        tmp[3] = pmult[-carry][3];
        UECC_VLI_ACCUMULATE4(result, tmp, -1, carry);
    }
    else if (carry > 0)
    {
        tmp[0] = (unsigned long long) 0x0 - carry; /* the same than
                                                      pmult[-carry][0] */
        tmp[1] = pmult[carry][1];
        tmp[2] = 0; /* this is always 0 */
        tmp[3] = pmult[carry][3];
        UECC_VLI_ACCUMULATE4_NEG(result, tmp, 1, carry);
    }

    if (carry != 0 || result[3] >= 0xFFFFFFFF00000001UL)
    {
        /* Last reductions: rare path. */
        ASSERT(carry >= -1 && carry <= 1);

        /* NIST P-256 */
        tmp[0] = 0xFFFFFFFFFFFFFFFFUL;
        tmp[1] = 0x00000000FFFFFFFFUL;
        tmp[2] = 0x0000000000000000UL;
        tmp[3] = 0xFFFFFFFF00000001UL;

        if (carry < 0) {
            do {
                UECC_VLI_ACCUMULATE4(result, tmp, carry, carry);
            } while (carry < 0);
        } else if (carry == 0 && result[3] < 0xFFFFFFFF00000001UL) {
            /* Within range => no adjustment. */
        } else {
            while (carry >= 0)
            {
                UECC_VLI_ACCUMULATE4_NEG(result, tmp, carry, carry);
            }
            /* One too many negations: */
            UECC_VLI_ACCUMULATE4(result, tmp, carry, carry);
        }
    }
#else
    /* t */
    uECC_vli_set(result, product, num_words_secp256r1);

    /* s1 */
    tmp[0] = 0;
    tmp[1] = product[5] & 0xffffffff00000000ull;
    tmp[2] = product[6];
    tmp[3] = product[7];
    carry = (int)uECC_vli_accumulate256bit(tmp, tmp);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s2 */
    tmp[1] = product[6] << 32;
    tmp[2] = (product[6] >> 32) | (product[7] << 32);
    tmp[3] = product[7] >> 32;
    carry += uECC_vli_accumulate256bit(tmp, tmp);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s3 */
    tmp[0] = product[4];
    tmp[1] = product[5] & 0xffffffff;
    tmp[2] = 0;
    tmp[3] = product[7];
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* s4 */
    tmp[0] = (product[4] >> 32) | (product[5] << 32);
    tmp[1] = (product[5] >> 32) | (product[6] & 0xffffffff00000000ull);
    tmp[2] = product[7];
    tmp[3] = (product[6] >> 32) | (product[4] << 32);
    carry += uECC_vli_accumulate256bit(result, tmp);

    /* d1 */
    tmp[0] = (product[5] >> 32) | (product[6] << 32);
    tmp[1] = (product[6] >> 32);
    tmp[2] = 0;
    tmp[3] = (product[4] & 0xffffffff) | (product[5] << 32);
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d2 */
    tmp[0] = product[6];
    tmp[1] = product[7];
    tmp[2] = 0;
    tmp[3] = (product[4] >> 32) | (product[5] & 0xffffffff00000000ull);
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d3 */
    tmp[0] = (product[6] >> 32) | (product[7] << 32);
    tmp[1] = (product[7] >> 32) | (product[4] << 32);
    tmp[2] = (product[4] >> 32) | (product[5] << 32);
    tmp[3] = (product[6] << 32);
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    /* d4 */
    tmp[0] = product[7];
    tmp[1] = product[4] & 0xffffffff00000000ull;
    tmp[2] = product[5];
    tmp[3] = product[6] & 0xffffffff00000000ull;
    carry -= uECC_vli_accumulate_neg256bit(result, tmp);

    if (carry < 0) {
        do {
            carry += uECC_vli_accumulate256bit(result, curve_secp256r1.p);
        } while (carry < 0);
    } else {
        while (carry || uECC_vli_cmp_unsafe(curve_secp256r1.p, result, num_words_secp256r1) != 1) {
            carry -= uECC_vli_accumulate_neg256bit(result, curve_secp256r1.p);
        }
    }
#endif
}
#endif /* uECC_WORD_SIZE */
#endif /* (uECC_OPTIMIZATION_LEVEL > 0 && !asm_mmod_fast_secp256r1) */

#endif /* uECC_SUPPORTS_secp256r1 */

#if uECC_SUPPORTS_secp256k1

static void double_jacobian_secp256k1(uECC_word_t * X1,
                                      uECC_word_t * Y1,
                                      uECC_word_t * Z1,
                                      uECC_Curve curve);
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
static void x_side_secp256k1(uECC_word_t *result, const uECC_word_t *x, uECC_Curve curve);
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
static void vli_mmod_fast_secp256k1(uECC_word_t *result, uECC_word_t *product);
#endif

static const struct uECC_Curve_t curve_secp256k1 = {
    num_words_secp256k1,
    num_bytes_secp256k1,
    256, /* num_n_bits */
    { BYTES_TO_WORDS_8(2F, FC, FF, FF, FE, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(41, 41, 36, D0, 8C, 5E, D2, BF),
        BYTES_TO_WORDS_8(3B, A0, 48, AF, E6, DC, AE, BA),
        BYTES_TO_WORDS_8(FE, FF, FF, FF, FF, FF, FF, FF),
        BYTES_TO_WORDS_8(FF, FF, FF, FF, FF, FF, FF, FF) },
    { BYTES_TO_WORDS_8(98, 17, F8, 16, 5B, 81, F2, 59),
        BYTES_TO_WORDS_8(D9, 28, CE, 2D, DB, FC, 9B, 02),
        BYTES_TO_WORDS_8(07, 0B, 87, CE, 95, 62, A0, 55),
        BYTES_TO_WORDS_8(AC, BB, DC, F9, 7E, 66, BE, 79),

        BYTES_TO_WORDS_8(B8, D4, 10, FB, 8F, D0, 47, 9C),
        BYTES_TO_WORDS_8(19, 54, 85, A6, 48, B4, 17, FD),
        BYTES_TO_WORDS_8(A8, 08, 11, 0E, FC, FB, A4, 5D),
        BYTES_TO_WORDS_8(65, C4, A3, 26, 77, DA, 3A, 48) },
    { BYTES_TO_WORDS_8(07, 00, 00, 00, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(00, 00, 00, 00, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(00, 00, 00, 00, 00, 00, 00, 00),
        BYTES_TO_WORDS_8(00, 00, 00, 00, 00, 00, 00, 00) },
    &double_jacobian_secp256k1,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_default,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_secp256k1,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0)
    &vli_mmod_fast_secp256k1
#endif
};

uECC_Curve uECC_secp256k1(void) { return &curve_secp256k1; }


/* Double in place */
static void double_jacobian_secp256k1(uECC_word_t * X1,
                                      uECC_word_t * Y1,
                                      uECC_word_t * Z1,
                                      uECC_Curve curve) {
    /* t1 = X, t2 = Y, t3 = Z */
    uECC_word_t t4[num_words_secp256k1];
    uECC_word_t t5[num_words_secp256k1];

    if (uECC_vli_isZero(Z1, num_words_secp256k1)) {
        return;
    }

    uECC_vli_modSquare_fast(t5, Y1, curve);   /* t5 = y1^2 */
    uECC_vli_modMult_fast(t4, X1, t5, curve); /* t4 = x1*y1^2 = A */
    uECC_vli_modSquare_fast(X1, X1, curve);   /* t1 = x1^2 */
    uECC_vli_modSquare_fast(t5, t5, curve);   /* t5 = y1^4 */
    uECC_vli_modMult_fast(Z1, Y1, Z1, curve); /* t3 = y1*z1 = z3 */

    uECC_vli_modAdd(Y1, X1, X1, curve->p, num_words_secp256k1); /* t2 = 2*x1^2 */
    uECC_vli_modAdd(Y1, Y1, X1, curve->p, num_words_secp256k1); /* t2 = 3*x1^2 */
    if (uECC_vli_testBit(Y1, 0)) {
        uECC_word_t carry = uECC_vli_accumulate(Y1, curve->p, num_words_secp256k1);
        uECC_vli_rshift1(Y1, num_words_secp256k1);
        Y1[num_words_secp256k1 - 1] |= carry << (uECC_WORD_BITS - 1);
    } else {
        uECC_vli_rshift1(Y1, num_words_secp256k1);
    }
    /* t2 = 3/2*(x1^2) = B */

    uECC_vli_modSquare_fast(X1, Y1, curve);                     /* t1 = B^2 */
    uECC_vli_modSub(X1, X1, t4, curve->p, num_words_secp256k1); /* t1 = B^2 - A */
    uECC_vli_modSub(X1, X1, t4, curve->p, num_words_secp256k1); /* t1 = B^2 - 2A = x3 */

    uECC_vli_modSub(t4, t4, X1, curve->p, num_words_secp256k1); /* t4 = A - x3 */
    uECC_vli_modMult_fast(Y1, Y1, t4, curve);                   /* t2 = B * (A - x3) */
    uECC_vli_modSub(Y1, Y1, t5, curve->p, num_words_secp256k1); /* t2 = B * (A - x3) - y1^4 = y3 */
}

#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
/* Computes result = x^3 + b. result must not overlap x. */
static void x_side_secp256k1(uECC_word_t *result, const uECC_word_t *x, uECC_Curve curve) {
    uECC_vli_modSquare_fast(result, x, curve);                                /* r = x^2 */
    uECC_vli_modMult_fast(result, result, x, curve);                          /* r = x^3 */
    uECC_vli_modAdd(result, result, curve->b, curve->p, num_words_secp256k1); /* r = x^3 + b */
}
#endif

#if (uECC_OPTIMIZATION_LEVEL > 0 && !asm_mmod_fast_secp256k1)
static void omega_mult_secp256k1(uECC_word_t *result, const uECC_word_t *right);
static void vli_mmod_fast_secp256k1(uECC_word_t *result, uECC_word_t *product) {
    uECC_word_t tmp[2 * num_words_secp256k1];
    uECC_word_t carry;

    uECC_vli_clear(tmp, num_words_secp256k1);
    uECC_vli_clear(tmp + num_words_secp256k1, num_words_secp256k1);

    omega_mult_secp256k1(tmp, product + num_words_secp256k1); /* (Rq, q) = q * c */

    carry = uECC_vli_add(result, product, tmp, num_words_secp256k1); /* (C, r) = r + q       */
    uECC_vli_clear(product, num_words_secp256k1);
    omega_mult_secp256k1(product, tmp + num_words_secp256k1); /* Rq*c */
    carry += uECC_vli_accumulate(result, product, num_words_secp256k1); /* (C1, r) = r + Rq*c */

    while (carry > 0) {
        --carry;
        uECC_vli_accumulate_neg(result, curve_secp256k1.p, num_words_secp256k1);
    }
    if (uECC_vli_cmp_unsafe(result, curve_secp256k1.p, num_words_secp256k1) > 0) {
        uECC_vli_accumulate_neg(result, curve_secp256k1.p, num_words_secp256k1);
    }
}

#if uECC_WORD_SIZE == 1
static void omega_mult_secp256k1(uint8_t * result, const uint8_t * right) {
    /* Multiply by (2^32 + 2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    wordcount_t k;

    /* Multiply by (2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    muladd(0xD1, right[0], &r0, &r1, &r2);
    result[0] = r0;
    r0 = r1;
    r1 = r2;
    /* r2 is still 0 */

    for (k = 1; k < num_words_secp256k1; ++k) {
        muladd(0x03, right[k - 1], &r0, &r1, &r2);
        muladd(0xD1, right[k], &r0, &r1, &r2);
        result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    muladd(0x03, right[num_words_secp256k1 - 1], &r0, &r1, &r2);
    result[num_words_secp256k1] = r0;
    result[num_words_secp256k1 + 1] = r1;
    /* add the 2^32 multiple */
    result[4 + num_words_secp256k1] =
        uECC_vli_accumulate(result + 4, right, num_words_secp256k1);
}
#elif uECC_WORD_SIZE == 4
static void omega_mult_secp256k1(uint32_t * result, const uint32_t * right) {
    /* Multiply by (2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    uint32_t carry = 0;
    wordcount_t k;

    for (k = 0; k < num_words_secp256k1; ++k) {
        uint64_t p = (uint64_t)0x3D1 * right[k] + carry;
        result[k] = (uint32_t) p;
        carry = p >> 32;
    }
    result[num_words_secp256k1] = carry;
    /* add the 2^32 multiple */
    result[1 + num_words_secp256k1] =
        uECC_vli_accumulate(result + 1, right, num_words_secp256k1);
}
#else
static void omega_mult_secp256k1(uint64_t * result, const uint64_t * right) {
    uECC_word_t r0 = 0;
    uECC_word_t r1 = 0;
    uECC_word_t r2 = 0;
    wordcount_t k;

    /* Multiply by (2^32 + 2^9 + 2^8 + 2^7 + 2^6 + 2^4 + 1). */
    for (k = 0; k < num_words_secp256k1; ++k) {
        muladd(0x1000003D1ull, right[k], &r0, &r1, &r2);
        result[k] = r0;
        r0 = r1;
        r1 = r2;
        r2 = 0;
    }
    result[num_words_secp256k1] = r0;
}
#endif /* uECC_WORD_SIZE */
#endif /* (uECC_OPTIMIZATION_LEVEL > 0 &&  && !asm_mmod_fast_secp256k1) */

#endif /* uECC_SUPPORTS_secp256k1 */

#if uECC_SUPPORTS_secp384r1

#undef NDEBUG
#include <assert.h>

/* Enable ASSERT_MATHF() which will verify results of reduction using python.
   This option is only for debugging of vli_mmod_fast_secp384r1_almost_pow2reduction. */
// #define USE_ASSERT_MATHF
#ifdef USE_ASSERT_MATHF
#undef NDEBUG
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
/* #include "slmnt.h" */
/* #include "psPrnf.h" */

#define PSA_UECC_HEX(x,l) PSA_SLMW_HEX((SLMWord*)(x), ((l)*sizeof(uECC_word_t)/sizeof(SLMWord)))

/* Evaluate expression and fail if expression returns 1.
   The expressions are evaluted using shell and they are typically python commands.
   This function is only intended for debugging and verification, the function is
   too slow for practical uses. */
static void assertSysPsPrnf_worker(char *str, const char *file, int line)
{
    int res = system(str);
    if (res != 0)
    {
        fprintf(stderr, "%s:%d: ASSERT FAIL: Command=%s\n", file, line, str);
        assert(0);
    }
    free(str);
}
//#define assertSysPsPrnf(...) assertSysPsPrnf_worker(psAsprnf(NULL, __VA_ARGS__), __FILE__, __LINE__)
#define assertSysPsPrnf(...) do { } while(0)
#define ASSERTSysPsPrnf(...) assertSysPsPrnf_worker(psAsprnf(NULL, __VA_ARGS__), __FILE__, __LINE__)

#define ASSERT_MATHF(fmt, ...) ASSERTSysPsPrnf("python -c 'exit(1-("fmt"))'", __VA_ARGS__)
#else
#define ASSERT_MATHF(fmt, ...) do { /* not verifying results. */ } while(0)
#endif /* USE_ASSERT_MATHF */

static inline void vli_mmod_fast_secp384r1_almost_pow2reduction(uECC_word_t *result, uECC_word_t *product)
{
    uECC_Curve curve = uECC_secp384r1();
    int carry;
    uECC_word_t tmp[BITS_TO_WORDS(128) + BITS_TO_WORDS(384) + BITS_TO_WORDS(128)];
    uECC_word_t tmp2[BITS_TO_WORDS(384) + BITS_TO_WORDS(128)];

#ifdef USE_ASSERT_MATHF
    PS_PRNF_CTX;
#endif

    /* use product in two parts: a * 2**n + b => a == high bits and b == low bits. */

    /* tmp == 2 ** 128 * a (with additional reserved space for 640 bits) */
    uECC_vli_clear(tmp, BITS_TO_WORDS(128));
    uECC_vli_set(tmp + BITS_TO_WORDS(128), product + BITS_TO_WORDS(384), BITS_TO_WORDS(384));
    uECC_vli_clear(tmp + BITS_TO_WORDS(512), BITS_TO_WORDS(128));

    /* Allow referencing high and low parts of the product briefly in ASSERT_MATHF(). */
#define PR_A psPrnfDupf(&psPrnfCtx, "[A]", "0x%s", PSA_UECC_HEX(product + BITS_TO_WORDS(384), \
                                                                BITS_TO_WORDS(384)))
#define PR_B psPrnfDupf(&psPrnfCtx, "[B]", "0x%s", PSA_UECC_HEX(product, BITS_TO_WORDS(384)))

    /* tmp2 = (a * 2 ** 128) - (a * 2 ** 64) */
    uECC_vli_sub(tmp2, tmp, tmp + BITS_TO_WORDS(64), BITS_TO_WORDS(384) + BITS_TO_WORDS(128));
    ASSERT_MATHF("0x%s == (2 ** 128 - 2 ** 64) * %s",
                 PSA_UECC_HEX(tmp2, BITS_TO_WORDS(384) + BITS_TO_WORDS(128)), PR_A);

    /* tmp2 = (a * 2 ** 96) - (a * 2 ** 32) */
    /* Note: Following two statements are equivalent of right shift 32. (Little-endian only.) */
    memmove(tmp2, 4 + (unsigned char *)tmp2, (512 - 32) / 8);
    memset(((unsigned char *) (tmp2 + BITS_TO_WORDS(512))) - 4, 0, 4);

    ASSERT_MATHF("0x%s == (2 ** 96 - 2 ** 32) * %s",
                 PSA_UECC_HEX(tmp2, BITS_TO_WORDS(384) + BITS_TO_WORDS(128)), PR_A);

    /* tmp2 = (a * 2 ** 96) - (a * 2 ** 32) + a */
    uECC_vli_accumulate(tmp2, tmp + BITS_TO_WORDS(128), BITS_TO_WORDS(384) + BITS_TO_WORDS(128));

    /* carry || tmp = a * (2**128 + 2**96 - 2**32 + 1) */
    carry = uECC_vli_accumulate(tmp, tmp2, BITS_TO_WORDS(384) + BITS_TO_WORDS(128));
    ASSERT_MATHF("2**512 * %d + 0x%s == (2**128 + 2**96 - 2**32 + 1) * %s",
                 carry, PSA_UECC_HEX(tmp, BITS_TO_WORDS(512)), PR_A);

    /* carry || tmp = a * (2**128 + 2**96 - 2**32 + 1) + b */
    uECC_vli_set(tmp2, product, BITS_TO_WORDS(384));
    uECC_vli_clear(tmp2 + BITS_TO_WORDS(384), BITS_TO_WORDS(128));
    carry += uECC_vli_accumulate(tmp, tmp2, BITS_TO_WORDS(384) + BITS_TO_WORDS(128));
    ASSERT_MATHF("2**512 * %d + 0x%s == %s * (2**128 + 2**96 - 2**32 + 1) + %s",
                 carry, PSA_UECC_HEX(tmp, BITS_TO_WORDS(512)), PR_A, PR_B);

    ASSERT_MATHF("(0x%s %% 0x%s) == (2**512 * %d + 0x%s) %% 0x%s",
                 PSA_UECC_HEX(product, BITS_TO_WORDS(768)),
                 PSA_UECC_HEX(curve->p, BITS_TO_WORDS(384)),
                 carry, PSA_UECC_HEX(tmp, BITS_TO_WORDS(512)),
                 PSA_UECC_HEX(curve->p, BITS_TO_WORDS(384)));

#ifdef USE_ASSERT_MATHF
    /* Copy temporary result for next ASSERT_MATHF statements. */
    uECC_word_t product_phase2[BITS_TO_WORDS(768)];
    uECC_vli_clear(product_phase2, BITS_TO_WORDS(768));
    uECC_vli_set(product_phase2, tmp, BITS_TO_WORDS(512));
    product_phase2[BITS_TO_WORDS(512)] = carry;

#undef PR_A
#undef PR_B
#define PR_A psPrnfDupf(&psPrnfCtx, "[A]", "0x%s", PSA_UECC_HEX(product_phase2 + BITS_TO_WORDS(384), \
                                                                BITS_TO_WORDS(384)))
#define PR_B psPrnfDupf(&psPrnfCtx, "[B]", "0x%s", PSA_UECC_HEX(product_phase2, BITS_TO_WORDS(384)))

#endif

    /* Second reduction: a * 2**n + b - now a is less than 2**128 + 2**96 - 2**32 + 1. */
    uECC_vli_set(result, tmp, BITS_TO_WORDS(384)); /* Intermediate result. */

    /* tmp == 2 ** 128 * a */
    uECC_vli_set(tmp + BITS_TO_WORDS(128), tmp + BITS_TO_WORDS(384), BITS_TO_WORDS(128));
    uECC_vli_clear(tmp, BITS_TO_WORDS(128));
    uECC_vli_clear(tmp + BITS_TO_WORDS(256), BITS_TO_WORDS(384));
    tmp[BITS_TO_WORDS(256)] = carry; /* length of tmp in bits is 257 bits. */

    /* tmp2 = (a * 2 ** 128) - (a * 2 ** 64) */
    uECC_vli_sub(tmp2, tmp, tmp + BITS_TO_WORDS(64), BITS_TO_WORDS(257));
    ASSERT_MATHF("0x%s == (2 ** 128 - 2 ** 64) * %s", PSA_UECC_HEX(tmp2, BITS_TO_WORDS(257)), PR_A);

    /* tmp2 = (a * 2 ** 96) - (a * 2 ** 32) */
    /* Note: Following statements is equivalent of right shift 32. (Little-endian only.) */
    memmove(tmp2, 4 + (unsigned char *)tmp2, BITS_TO_WORDS(257) * sizeof(uECC_word_t));

    /* tmp2 = (a * 2 ** 96) - (a * 2 ** 32) + a */
    uECC_vli_accumulate(tmp2, tmp + BITS_TO_WORDS(128), BITS_TO_WORDS(257));

    /* tmp = a * (2**128 + 2**96 - 2**32 + 1) */
    tmp[BITS_TO_WORDS(256)] = uECC_vli_accumulate(tmp, tmp2, BITS_TO_WORDS(256));
    uECC_vli_clear(tmp + BITS_TO_WORDS(257), BITS_TO_WORDS(384) - BITS_TO_WORDS(257));
    ASSERT_MATHF("0x%s == (2**128 + 2**96 - 2**32 + 1) * %s",
                 PSA_UECC_HEX(tmp, BITS_TO_WORDS(384)), PR_A);

    /* carry || tmp = a * (2**128 + 2**96 - 2**32 + 1) + b */
    uECC_vli_set(tmp2, result, BITS_TO_WORDS(384));
    carry = uECC_vli_accumulate(tmp, tmp2, BITS_TO_WORDS(384));
    ASSERT_MATHF("2**384 * %d + 0x%s == %s * (2**128 + 2**96 - 2**32 + 1) + %s",
                 carry, PSA_UECC_HEX(tmp, BITS_TO_WORDS(384)), PR_A, PR_B);
    (void)carry; /* to suppress clang warning */

    /* Result can be over p => one more reduce. */
    if (tmp[BITS_TO_WORDS(384)] || uECC_vli_cmp_unsafe(tmp, curve->p, num_words_secp384r1) > 0)
    {
        /* Still one reduce. */
        uECC_vli_sub(result, tmp, curve->p, num_words_secp384r1);
    }
    else
    {
        /* No more reduction. */
        uECC_vli_set(result, tmp, num_words_secp384r1);
    }

    /* If ASSERT_MATH is used, check the result is correctly reduced. */
    ASSERT_MATHF("0x%s == 0x%s %% 0x%s",
                 PSA_UECC_HEX(result, BITS_TO_WORDS(384)),
                 PSA_UECC_HEX(product, BITS_TO_WORDS(768)),
                 PSA_UECC_HEX(curve->p, BITS_TO_WORDS(384)));
}
#undef PSA_UECC_HEX

#if uECC_WORD_SIZE == 4 && defined __x86__ || defined __i686__
#ifdef uECC_OPTIMIZATION_COMPARE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
/* #include "slmnt.h" */
#endif /* uECC_OPTIMIZATION_COMPARE */

void vli_mmod_fast_secp384r1_solinasreduction(uECC_word_t *result, uECC_word_t *product);
#define HAVE_SECP384R1_SOLINAS_REDUCTION 1
void vli_mmod_fast_secp384r1_solinasreduction(uECC_word_t *result, uECC_word_t *product)
{
        /*
          The prime used is:
          0xfffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffeffffffff0000000000000000ffffffffL
            (2**384 - 2**128 - 2**96 + 2**32 - 1).

          For reduction we can use this expression:
          B = T + 2S1 + S2 + S3 + S4 + S5 + S6 - D1 - D2 - D3 (mod p).

          These are defined as follows:
          T =  A11 || A10 ||  A9 ||  A8 ||  A7 ||  A6 ||  A5 ||  A4 ||  A3 ||  A2 ||  A1 ||  A0
          S1=    0 ||   0 ||   0 ||   0 ||   0 || A23 || A22 || A21 ||   0 ||   0 ||   0 ||   0
          S2=  A23 || A22 || A21 || A20 || A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12
          S3=  A20 || A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12 || A23 || A22 || A21
          S4=  A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12 || A20 ||   0 || A23 || A20
          S5=    0 ||   0 ||   0 ||   0 || A23 || A22 || A21 || A20 || A21 ||   0 ||   0 ||   0
          S6=    0 ||   0 ||   0 ||   0 ||   0 ||   0 || A23 || A22 ||   0 ||   0 ||   0 ||   0
          D1=  A22 || A21 || A20 || A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12 || A23
          D2=    0 ||   0 ||   0 ||   0 ||   0 ||   0 ||   0 || A23 || A22 || A21 || A20 ||   0
          D3=    0 ||   0 ||   0 ||   0 ||   0 ||   0 ||   0 || A23 || A23 ||   0 ||   0 ||   0
        */

    /* This function is "large memory usage implementation". Please fix. */

    uECC_word_t T[BITS_TO_WORDS(384)];
    uECC_word_t S1[BITS_TO_WORDS(384)];
    uECC_word_t S2[BITS_TO_WORDS(384)];
    uECC_word_t S3[BITS_TO_WORDS(384)];
    uECC_word_t S4[BITS_TO_WORDS(384)];
    uECC_word_t S5[BITS_TO_WORDS(384)];
    uECC_word_t S6[BITS_TO_WORDS(384)];
    uECC_word_t D1[BITS_TO_WORDS(384)];
    uECC_word_t D2[BITS_TO_WORDS(384)];
    uECC_word_t D3[BITS_TO_WORDS(384)];
    uECC_Curve curve = uECC_secp384r1();
    unsigned char carry, carry1, carry2, carry3, carry4, carry5;
    uECC_word_t hibits;

#define A(x) product[x]
#define A23 A(23)
#define A22 A(22)
#define A21 A(21)
#define A20 A(20)
#define A19 A(19)
#define A18 A(18)
#define A17 A(17)
#define A16 A(16)
#define A15 A(15)
#define A14 A(14)
#define A13 A(13)
#define A12 A(12)
#define A11 A(11)
#define A10 A(10)
#define A9 A(9)
#define A8 A(8)
#define A7 A(7)
#define A6 A(6)
#define A5 A(5)
#define A4 A(4)
#define A3 A(3)
#define A2 A(2)
#define A1 A(1)
#define A0 A(0)
#define ECDSA_ASSIGN12(v, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0) \
    do { \
        v[11] = (v11);  v[10] = (v10);           \
        v[9] = (v9);  v[8] = (v8); \
        v[7] = (v7);  v[6] = (v6);           \
        v[5] = (v5);  v[4] = (v4); \
        v[3] = (v3);  v[2] = (v2); \
        v[1] = (v1);  v[0] = (v0); \
    } while (0)

    ECDSA_ASSIGN12(T,  A11, A10,  A9,  A8,  A7,  A6,  A5,  A4,  A3,  A2,  A1,  A0);
    ECDSA_ASSIGN12(S1,   0,   0,   0,   0,   0, A23, A22, A21,   0,   0,   0,   0);
    ECDSA_ASSIGN12(S2, A23, A22, A21, A20, A19, A18, A17, A16, A15, A14, A13, A12);
    ECDSA_ASSIGN12(S3, A20, A19, A18, A17, A16, A15, A14, A13, A12, A23, A22, A21);
    ECDSA_ASSIGN12(S4, A19, A18, A17, A16, A15, A14, A13, A12, A20,   0, A23, A20);
    ECDSA_ASSIGN12(S5,   0,   0,   0,   0, A23, A22, A21, A20, A21,   0,   0,   0);
    ECDSA_ASSIGN12(S6,   0,   0,   0,   0,   0,   0, A23, A22,   0,   0,   0,   0);
    ECDSA_ASSIGN12(D1, A22, A21, A20, A19, A18, A17, A16, A15, A14, A13, A12, A23);
    ECDSA_ASSIGN12(D2,   0,   0,   0,   0,   0,   0,   0, A23, A22, A21, A20,   0);
    ECDSA_ASSIGN12(D3,   0,   0,   0,   0,   0,   0,   0, A23, A23,   0,   0,   0);

    /* Reduction starts by summing the small terms. */

    /* Combine D2 and D3: high words. */
    uECC_vli_add2_carry_start(D2[3], D2[4],
                              D3[3], D3[4], carry);
    D2[5] = carry;

    /* Remove D2 low from S5. */
    uECC_vli_sub3_borrow_start(S5[1], S5[2], S5[3],
                               D2[1], D2[2], 0, carry);
    /* ignore carry: it cannot be set, because A20 || A21 || 0 || 0 >= A21 || A20. */

    /* Remove D2/D3 high from S6. */
    uECC_vli_sub3_borrow_start(S6[3], S6[4], S6[5],
                               D2[3], D2[4], D2[5], carry);
    /* ignore carry: it cannot be set, because A23 || A22 || 0 >= A23+A23 || A23+A22. */

    /* double S1. */
    uECC_vli_add3_carry_start(S1[4], S1[5], S1[6],
                              S1[4], S1[5], S1[6], carry);
    S1[7] = carry;

    /* Combine S6 to S1. */
    uECC_vli_add4_carry_start(S1[3], S1[4], S1[5], S1[6],
                              S6[3], S6[4], S6[5], 0, carry);
    S1[7] += carry;

    /* Combine S5 to S1. */
    S1[1] = S5[1];
    S1[2] = S5[2];
    uECC_vli_add4_carry_start(S1[3], S1[4], S1[5], S1[6],
                              S5[3], S5[4], S5[5], S5[6], carry);
    uECC_vli_add1_carry_cont(S1[7], S5[7], carry);
    S1[8] = carry;

    /* Now we have reduced the set to words T, S1, S2, S3, S4 and D1.
       (New S1 replaces previous words S1, S5, S1, D2 and D3.)
       B = T + S1 + S2 + S3 + S4 - D1 (mod p). */

#ifndef UECC_VLI_ADD3_WORDS_AT_ONCE
    /* Summing: 4 words at once. */
    uECC_vli_add4_carry_start(T[0], T[1], T[2], T[3], S1[0], S1[1], S1[2], S1[3], carry1);
    uECC_vli_add4_carry_start(T[0], T[1], T[2], T[3], S2[0], S2[1], S2[2], S2[3], carry2);
    uECC_vli_add4_carry_start(T[0], T[1], T[2], T[3], S3[0], S3[1], S3[2], S3[3], carry3);
    uECC_vli_add4_carry_start(T[0], T[1], T[2], T[3], S4[0], S4[1], S4[2], S4[3], carry4);
    uECC_vli_sub4_borrow_start(T[0], T[1], T[2], T[3], D1[0], D1[1], D1[2], D1[3], carry5);

    uECC_vli_add4_carry_cont(T[4], T[5], T[6], T[7], S1[4], S1[5], S1[6], S1[7], carry1);
    uECC_vli_add4_carry_cont(T[4], T[5], T[6], T[7], S2[4], S2[5], S2[6], S2[7], carry2);
    uECC_vli_add4_carry_cont(T[4], T[5], T[6], T[7], S3[4], S3[5], S3[6], S3[7], carry3);
    uECC_vli_add4_carry_cont(T[4], T[5], T[6], T[7], S4[4], S4[5], S4[6], S4[7], carry4);
    uECC_vli_sub4_borrow_cont(T[4], T[5], T[6], T[7], D1[4], D1[5], D1[6], D1[7], carry5);

    uECC_vli_add4_carry_finish(T[8], T[9], T[10], T[11], S1[8], S1[9], S1[10], S1[11], carry1, 0, hibits);
    uECC_vli_add4_carry_finish(T[8], T[9], T[10], T[11], S2[8], S2[9], S2[10], S2[11], carry2, hibits, hibits);
    uECC_vli_add4_carry_finish(T[8], T[9], T[10], T[11], S3[8], S3[9], S3[10], S3[11], carry3, hibits, hibits);
    uECC_vli_add4_carry_finish(T[8], T[9], T[10], T[11], S4[8], S4[9], S4[10], S4[11], carry4, hibits, hibits);
    uECC_vli_sub4_borrow_finish(T[8], T[9], T[10], T[11], D1[8], D1[9], D1[10], D1[11], carry5, hibits, hibits);
#else /* !UECC_VLI_ADD3_WORDS_AT_ONCE */
    /* Summing: 3 words at once (free more registers for processing.) */
    uECC_vli_add3_carry_start(T[0], T[1], T[2], S1[0], S1[1], S1[2], carry1);
    uECC_vli_add3_carry_start(T[0], T[1], T[2], S2[0], S2[1], S2[2], carry2);
    uECC_vli_add3_carry_start(T[0], T[1], T[2], S3[0], S3[1], S3[2], carry3);
    uECC_vli_add3_carry_start(T[0], T[1], T[2], S4[0], S4[1], S4[2], carry4);
    uECC_vli_sub3_borrow_start(T[0], T[1], T[2], D1[0], D1[1], D1[2], carry5);

    uECC_vli_add3_carry_cont(T[3], T[4], T[5], S1[3], S1[4], S1[5], carry1);
    uECC_vli_add3_carry_cont(T[3], T[4], T[5], S2[3], S2[4], S2[5], carry2);
    uECC_vli_add3_carry_cont(T[3], T[4], T[5], S3[3], S3[4], S3[5], carry3);
    uECC_vli_add3_carry_cont(T[3], T[4], T[5], S4[3], S4[4], S4[5], carry4);
    uECC_vli_sub3_borrow_cont(T[3], T[4], T[5], D1[3], D1[4], D1[5], carry5);

    uECC_vli_add3_carry_cont(T[6], T[7], T[8], S1[6], S1[7], S1[8], carry1);
    uECC_vli_add3_carry_cont(T[6], T[7], T[8], S2[6], S2[7], S2[8], carry2);
    uECC_vli_add3_carry_cont(T[6], T[7], T[8], S3[6], S3[7], S3[8], carry3);
    uECC_vli_add3_carry_cont(T[6], T[7], T[8], S4[6], S4[7], S4[8], carry4);
    uECC_vli_sub3_borrow_cont(T[6], T[7], T[8], D1[6], D1[7], D1[8], carry5);

    uECC_vli_add3_carry_finish(T[9], T[10], T[11], S1[9], S1[10], S1[11], carry1, 0, hibits);
    uECC_vli_add3_carry_finish(T[9], T[10], T[11], S2[9], S2[10], S2[11], carry2, hibits, hibits);
    uECC_vli_add3_carry_finish(T[9], T[10], T[11], S3[9], S3[10], S3[11], carry3, hibits, hibits);
    uECC_vli_add3_carry_finish(T[9], T[10], T[11], S4[9], S4[10], S4[11], carry4, hibits, hibits);
    uECC_vli_sub3_borrow_finish(T[9], T[10], T[11], D1[9], D1[10], D1[11], carry5, hibits, hibits);
#endif /* !UECC_VLI_ADD3_WORDS_AT_ONCE */

    /* If we have underflow. */
    if (hibits >= (uECC_word_t) 8) /* Maximum is 4, larger numbers are
                                      negative. */
    {
        /* Underflow: One round nearly always suffices. */
        do
        {
            uECC_vli_add4_carry_start(T[0], T[1], T[2], T[3], 0xffffffffu, 0x00000000u, 0x00000000u, 0xffffffffu, carry);
            uECC_vli_add4_carry_cont(T[4], T[5], T[6], T[7], 0xfffffffeu, 0xffffffffu, 0xffffffffu, 0xffffffffu, carry);
            uECC_vli_add4_carry_finish(T[8], T[9], T[10], T[11],
                                       0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, carry, hibits, hibits);
        }
        while (hibits != 0);
    }
    else
    {
        /* Overflow: Remove duplicates of p, 1*p to 4*p. */
        if (hibits > 0)
        {
            /* The multiples of p can be constructed from 0, 1, -1 and hibits with addition and subtraction. */
            uECC_vli_sub4_borrow_start(T[0], T[1], T[2], T[3],
                                       -hibits, hibits - 1, 0x00000000u, -hibits, carry);
            uECC_vli_sub4_borrow_cont(T[4], T[5], T[6], T[7],
                                      0xffffffffu - hibits, 0xffffffffu, 0xffffffffu, 0xffffffffu, carry);
            uECC_vli_sub4_borrow_finish(T[8], T[9], T[10], T[11],
                                        0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu,
                                        carry, 1, hibits);
        }

        /* If the result is bigger than P-384 then substract still once. */
        if (hibits || uECC_vli_cmp_unsafe(T, curve->p, 12) >= 0)
        {
            uECC_vli_sub4_borrow_start(T[0], T[1], T[2], T[3], 0xffffffffu, 0x00000000u, 0x00000000u, 0xffffffffu, carry);
            uECC_vli_sub4_borrow_cont(T[4], T[5], T[6], T[7], 0xfffffffeu, 0xffffffffu, 0xffffffffu, 0xffffffffu, carry);
            uECC_vli_sub4_borrow_finish(T[8], T[9], T[10], T[11],
                                        0xffffffffu, 0xffffffffu, 0xffffffffu, 0xffffffffu, carry, hibits, hibits);
        }
    }

    uECC_vli_set(result, T, BITS_TO_WORDS(384));
}
#endif
#undef ECDSA_ASSIGN12
#undef A
#undef A23
#undef A22
#undef A21
#undef A20
#undef A19
#undef A18
#undef A17
#undef A16
#undef A15
#undef A14
#undef A13
#undef A12
#undef A11
#undef A10
#undef A9
#undef A8
#undef A7
#undef A6
#undef A5
#undef A4
#undef A3
#undef A2
#undef A1
#undef A0

static void vli_mmod_fast_secp384r1(uECC_word_t *result, uECC_word_t *product)
{
        /*
          The prime used is:
          0xffffffffffffffffffffffffffffffffffffffffffffffff
            fffffffffffffffeffffffff0000000000000000ffffffffL

            or 2**384 - 2**128 - 2**96 + 2**32 - 1.

          For reduction we can use this expression:
          B = T + 2S1 + S2 + S3 + S4 + S5 + S6 - D1 - D2 - D3 (mod p).

          These are defined as follows:
          T =  A11 || A10 ||  A9 ||  A8 ||  A7 ||  A6 ||  A5 ||  A4 ||  A3 ||  A2 ||  A1 ||  A0
          S1=    0 ||   0 ||   0 ||   0 ||   0 || A23 || A22 || A21 ||   0 ||   0 ||   0 ||   0
          S2=  A23 || A22 || A21 || A20 || A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12
          S3=  A20 || A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12 || A23 || A22 || A21
          S4=  A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12 || A20 ||   0 || A23 ||   0
          S5=    0 ||   0 ||   0 ||   0 || A23 || A22 || A21 || A20 ||   0 ||   0 ||   0 ||   0
          S6=    0 ||   0 ||   0 ||   0 ||   0 ||   0 || A23 || A22 || A21 ||   0 ||   0 || A20
          D1=  A22 || A21 || A20 || A19 || A18 || A17 || A16 || A15 || A14 || A13 || A12 || A23
          D2=    0 ||   0 ||   0 ||   0 ||   0 ||   0 ||   0 || A23 || A22 || A21 || A20 ||   0
          D3=    0 ||   0 ||   0 ||   0 ||   0 ||   0 ||   0 || A23 || A23 ||   0 ||   0 ||   0

          Alternatively, identity operation a * 2**n + b == a * c + b (mod 2**n - c) can be used
          for reduction. Here c is 2**128 + 2**96 - 2**32 + 1.
       */

#ifdef UECC_USE_SLP_ECDSA_PREDUCE768_TO_384
        /* Use the pre-existing reduction function from SL. */
        void SLP_ECDSA_preduce768_to_384(uECC_word_t *product,
                                         uECC_word_t *result);
        SLP_ECDSA_preduce768_to_384(product, result);
#elif !defined HAVE_SECP384R1_SOLINAS_REDUCTION
        vli_mmod_fast_secp384r1_almost_pow2reduction(result, product);
#else
        vli_mmod_fast_secp384r1_solinasreduction(result, product);
#ifdef uECC_OPTIMIZATION_COMPARE
        uECC_word_t result2[BITS_TO_WORDS(384)];
        vli_mmod_fast_secp384r1_almost_pow2reduction(result2, product);
        if (memcmp(result2, result, BITS_TO_BYTES(384)))
        {
            printf("Reduction MISMATCH!\n");
            exit(1);
        }
        else
        {
            static int counter = 0;
            if (counter < 10)
            {
                printf("Reduction ok!\n");
                counter++;
            }
        }
#endif /* uECC_OPTIMIZATION_COMPARE */
#endif
}
#define uECC_HAVE_MMOD_FAST_SECP384R1 1

static const struct uECC_Curve_t curve_secp384r1 = {
    num_words_secp384r1,
    num_bytes_secp384r1,
    384, /* num_n_bits */
    { BYTES_TO_WORDS_8(ff, ff, ff, ff, 00, 00, 00, 00), /* p (little-endian) */
        BYTES_TO_WORDS_8(00, 00, 00, 00, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(fe, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff) },
    { BYTES_TO_WORDS_8(73, 29, c5, cc, 6a, 19, ec, ec), /* n (le) */
        BYTES_TO_WORDS_8(7a, a7, b0, 48, b2, 0d, 1a, 58),
        BYTES_TO_WORDS_8(df, 2d, 37, f4, 81, 4d, 63, c7),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff) },
    { BYTES_TO_WORDS_8(b7, 0a, 76, 72, 38, 5e, 54, 3a), /* Gx (le) */
        BYTES_TO_WORDS_8(6c, 29, 55, bf, 5d, f2, 02, 55),
        BYTES_TO_WORDS_8(38, 2a, 54, 82, e0, 41, f7, 59),
        BYTES_TO_WORDS_8(98, 9b, a7, 8b, 62, 3b, 1d, 6e),
        BYTES_TO_WORDS_8(74, ad, 20, f3, 1e, c7, b1, 8e),
        BYTES_TO_WORDS_8(37, 05, 8b, be, 22, ca, 87, aa),

        BYTES_TO_WORDS_8(5f, 0e, ea, 90, 7c, 1d, 43, 7a), /* Gy (le) */
        BYTES_TO_WORDS_8(9d, 81, 7e, 1d, ce, b1, 60, 0a),
        BYTES_TO_WORDS_8(c0, b8, f0, b5, 13, 31, da, e9),
        BYTES_TO_WORDS_8(7c, 14, 9a, 28, bd, 1d, f4, f8),
        BYTES_TO_WORDS_8(29, dc, 92, 92, bf, 98, 9e, 5d),
        BYTES_TO_WORDS_8(6f, 2c, 26, 96, 4a, de, 17, 36) },
    { BYTES_TO_WORDS_8(ef, 2a, ec, d3, ed, c8, 85, 2a),
        BYTES_TO_WORDS_8(9d, d1, 2e, 8a, 8d, 39, 56, c6),
        BYTES_TO_WORDS_8(5a, 87, 13, 50, 8f, 08, 14, 03),
        BYTES_TO_WORDS_8(12, 41, 81, fe, 6e, 9c, 1d, 18),
        BYTES_TO_WORDS_8(19, 2d, f8, e3, 6b, 05, 8e, 98),
        BYTES_TO_WORDS_8(e4, e7, 3e, e2, a7, 2f, 31, b3) },
    &double_jacobian_default,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_default,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_default,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0) && uECC_HAVE_MMOD_FAST_SECP384R1
    &vli_mmod_fast_secp384r1
#endif
};

uECC_Curve uECC_secp384r1(void) { return &curve_secp384r1; }

#endif /* uECC_SUPPORTS_secp384r1 */

#if uECC_SUPPORTS_secp521r1

/* This function requires > 9-bit word size. */
static void uECC_vli_rshift9_copy(uECC_word_t *dest,
                                  uECC_word_t *vli,
                                  wordcount_t num_words) {
    uECC_word_t *end = vli;
    uECC_word_t carry = 0;

    vli += num_words;
    dest += num_words;
    while (vli-- > end) {
        uECC_word_t temp = *vli;
        dest--;
        *dest = (temp >> 9) | carry;
        carry = temp << (uECC_WORD_BITS - 9);
    }
}

/* #include "slmnt.h" */
#include "osdep-types.h"
/* #include "psPrnf.h" */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PSA_UECC_HEX(in, len) PSA_SLMW_HEX((const SLMWord *)(in), (len)*(2))

#include <string.h>
static void vli_mmod_fast_secp521r1(uECC_word_t *result, uECC_word_t *product)
{
    uECC_Curve curve = uECC_secp521r1();
    uECC_word_t carry;
    static uECC_word_t one[BITS_TO_WORDS(521)] = { 1, /* ... */ };

#ifdef FUTURE
    /* Alternative PATH: using SLP_ECDSA reduction function. */
    /* Use the pre-existing reduction function from SL. */
    /* The reduction function has been measured to currently do worse
       than the implementation below. */
    void SLP_ECDSA_preduce1088_to_544(const uint32_t *product,
                                      uint32_t *result);

    /* Conversion between uECC words and SLP_ECDSA words. */
    uint32_t prod32[17 * 2];
    uint32_t res32[18];

    memcpy(prod32, product, 17 * 2 * sizeof(uint32_t));
    SLP_ECDSA_preduce1088_to_544(prod32, res32);
    res32[17] = 0;
    memcpy(result, res32, BITS_TO_WORDS(521) * uECC_WORD_SIZE);
    return;
#endif /* FUTURE */

    /* Take highest 521 bits to the result.
       (Note: WORDS_TO_BITS(num_words_secp521r1) >= 544.) */
    uECC_vli_rshift9_copy(result, &product[BITS_TO_WORDS(512)],
                          num_words_secp521r1);

    /* Sum high bits with the first 521 bits. */
    carry = uECC_vli_accumulate(result, product, BITS_TO_WORDS(512));
    result[BITS_TO_WORDS(512)] += (product[BITS_TO_WORDS(512)] & 0x1ff) + carry;

    /* The result can be 1 bit larger than 521 bits, if initial value was
       in range 0 ... p^2. Loop here so that even larger values are reduced. */
    while(result[BITS_TO_WORDS(512)] > 0x1ff)
    {
        result[BITS_TO_WORDS(512)] -= 0x200;
        uECC_vli_accumulate(result, one, BITS_TO_WORDS(521));
    }

    /* Last: Handle the case that result == curve->p, i.e.
       the result has 520 bits set to 1. */
    if (result[0] == (uECC_word_t) (unsigned long long) -1)
    {
        while (uECC_vli_cmp_unsafe(result, curve->p, BITS_TO_WORDS(521)) >= 0)
        {
            uECC_vli_accumulate_neg(result, curve->p, BITS_TO_WORDS(521));
        }
    }

#ifdef FUTURE
    /* If comparing alternative reduction paths. */
    if (memcmp(result, res32, 17*4) != 0)
    {
        printf("Reduce P-521 function misoperates!\n");
        exit(1);
    }
#endif /* FUTURE */
}
#define uECC_HAVE_MMOD_FAST_SECP521R1 1 /* TODO */

static const struct uECC_Curve_t curve_secp521r1 = {
    num_words_secp521r1,
    num_bytes_secp521r1,
    521, /* num_n_bits */
    { BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_4(ff, 01, 00, 00) },
    { BYTES_TO_WORDS_8(09, 64, 38, 91, 1e, b7, 6f, bb),
        BYTES_TO_WORDS_8(ae, 47, 9c, 89, b8, c9, b5, 3b),
        BYTES_TO_WORDS_8(d0, a5, 09, f7, 48, 01, cc, 7f),
        BYTES_TO_WORDS_8(6b, 96, 2f, bf, 83, 87, 86, 51),
        BYTES_TO_WORDS_8(fa, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_8(ff, ff, ff, ff, ff, ff, ff, ff),
        BYTES_TO_WORDS_4(ff, 01, 00, 00) },
    { BYTES_TO_WORDS_8(66, bd, e5, c2, 31, 7e, 7e, f9),
        BYTES_TO_WORDS_8(9b, 42, 6a, 85, c1, b3, 48, 33),
        BYTES_TO_WORDS_8(de, a8, ff, a2, 27, c1, 1d, fe),
        BYTES_TO_WORDS_8(28, 59, e7, ef, 77, 5e, 4b, a1),
        BYTES_TO_WORDS_8(ba, 3d, 4d, 6b, 60, af, 28, f8),
        BYTES_TO_WORDS_8(21, b5, 3f, 05, 39, 81, 64, 9c),
        BYTES_TO_WORDS_8(42, b4, 95, 23, 66, cb, 3e, 9e),
        BYTES_TO_WORDS_8(cd, e9, 04, 04, b7, 06, 8e, 85),
        BYTES_TO_WORDS_4(c6, 00, 00, 00),

        BYTES_TO_WORDS_8(50, 66, d1, 9f, 76, 94, be, 88),
        BYTES_TO_WORDS_8(40, c2, 72, a2, 86, 70, 3c, 35),
        BYTES_TO_WORDS_8(61, 07, ad, 3f, 01, b9, 50, c5),
        BYTES_TO_WORDS_8(40, 26, f4, 5e, 99, 72, ee, 97),
        BYTES_TO_WORDS_8(2c, 66, 3e, 27, 17, bd, af, 17),
        BYTES_TO_WORDS_8(68, 44, 9b, 57, 49, 44, f5, 98),
        BYTES_TO_WORDS_8(d9, 1b, 7d, 2c, b4, 5f, 8a, 5c),
        BYTES_TO_WORDS_8(04, c0, 3b, 9a, 78, 6a, 29, 39),
        BYTES_TO_WORDS_4(18, 01, 00, 00) },
    { BYTES_TO_WORDS_8(00, 3f, 50, 6b, d4, 1f, 45, ef),
        BYTES_TO_WORDS_8(f1, 34, 2c, 3d, 88, df, 73, 35),
        BYTES_TO_WORDS_8(07, bf, b1, 3b, bd, c0, 52, 16),
        BYTES_TO_WORDS_8(7b, 93, 7e, ec, 51, 39, 19, 56),
        BYTES_TO_WORDS_8(e1, 09, f1, 8e, 91, 89, b4, b8),
        BYTES_TO_WORDS_8(f3, 15, b3, 99, 5b, 72, da, a2),
        BYTES_TO_WORDS_8(ee, 40, 85, b6, a0, 21, 9a, 92),
        BYTES_TO_WORDS_8(1f, 9a, 1c, 8e, 61, b9, 3e, 95),
        BYTES_TO_WORDS_4(51, 00, 00, 00) },
    &double_jacobian_default,
#if uECC_SUPPORT_COMPRESSED_POINT
    &mod_sqrt_default,
#endif
#if uECC_SUPPORT_POINT_VALIDATION || uECC_SUPPORT_COMPRESSED_POINT
    &x_side_default,
#endif
#if (uECC_OPTIMIZATION_LEVEL > 0) && uECC_HAVE_MMOD_FAST_SECP521R1
    &vli_mmod_fast_secp521r1
#endif
};

uECC_Curve uECC_secp521r1(void) { return &curve_secp521r1; }

#endif /* uECC_SUPPORTS_secp521r1 */

#endif /* _UECC_CURVE_SPECIFIC_H_ */
