/* Copyright 2015, Kenneth MacKay. Licensed under the BSD 2-clause license. */
/* Changes by INSIDE Secure. */

#ifndef ASM_ARM_MULT8_H_DEFINED
#define ASM_ARM_MULT8_H_DEFINED 1

#include "osdep-types.h"

void uECC_vli_mult8(uint32_t *result,
                    const uint32_t *left,
                    const uint32_t *right);

void uECC_vli_square8(uint32_t *result,
                      const int32_t *left);

#endif /* ASM_ARM_MULT8_H_DEFINED */
