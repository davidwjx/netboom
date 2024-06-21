/* lib-sbswhashtest.c
 *
 * Description: Common test helper for SOSB hashes
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

/* Common test helper for SOSB hashes. */

#include "implementation_defs.h"
#include "sfzutf.h"
#include "sfzutf-utils.h"
#include "sb_sw_hash.h"
#include "c_lib.h"

#ifdef PERF
#include "sfzutf-perf.h"
#endif /* PERF */

#include "lib-sbswhashtest.h"

#if SBIF_ECDSA_BITS == 521
#define HASH_BITS 512
#else
#define HASH_BITS SBIF_ECDSA_BITS
#endif

#include <stdio.h>
static void debug288(const char *s, const uint32_t *v)
{
    IDENTIFIER_NOT_USED(s);
    IDENTIFIER_NOT_USED(v);
    if (v)
    {
        L_DEBUG(SB_SW_HASH_TEST,
                "%s: %08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X%08X", s,
                v[15], v[14], v[13], v[12], v[11], v[10], v[9], v[8],
                v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
    }
}

static void sbswhashtest_fast(const char * const data_p,
                              int order_data,
                              const void * const result_p,
                              const uint32_t cmplen)
{
    SB_SW_HASH_Context_t ctx;
    SfzUtfPtrSize data;
    uint32_t result_expect[16] =
    {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444,
        0x55555555, 0x66666666,
        0x77777777, 0x88888888,
        0x99999999, 0xaaaaaaaa,
        0xbbbbbbbb, 0xcccccccc,
        0xdddddddd, 0xffffffff,
        0x10101010, 0x20202020
    };
    uint32_t result_got[16] =
    {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444,
        0x55555555, 0x66666666,
        0x77777777, 0x88888888,
        0x99999999, 0xaaaaaaaa,
        0xbbbbbbbb, 0xcccccccc,
        0xdddddddd, 0xffffffff,
        0x10101010, 0x20202020
    };

#ifdef PERF
    PERF_TEST_BEGIN(PERF_TEST_FAST);
#endif /* PERF */

    data = sfzutf_ptrsize_from_str(data_p, order_data);
    if (result_p != NULL)
    {
        c_memcpy((void *)result_expect, result_p, cmplen);
    }

#ifdef PERF
    PERF_EXECUTE_TEST_FAST(
    {
        SB_SW_HASH_Init(&ctx, HASH_BITS);
        SB_SW_HASH_FinalUpdate(&ctx, data.ptr, data.len, result_got);
    });
#else
    SB_SW_HASH_Init(&ctx, HASH_BITS);
    SB_SW_HASH_FinalUpdate(&ctx, data.ptr, data.len, result_got);
#endif /* PERF */

    debug288("res_expect", result_expect);
    debug288("res_received", result_got);

    if (result_p != NULL)
    {
        fail_if(c_memcmp(result_expect, result_got, sizeof(result_got)) != 0,
                "Result differs");
    }

    sfzutf_ptrsize_free(data);
}

void sbswhashtest(const char * const data_p,
                  int order_data,
                  const void * const result_p,
                  const uint32_t cmplen)
{
    SB_SW_HASH_Context_t ctx;
    SfzUtfPtrSize data;
    uint32_t result_expect[16] =
    {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444,
        0x55555555, 0x66666666,
        0x77777777, 0x88888888,
        0x99999999, 0xaaaaaaaa,
        0xbbbbbbbb, 0xcccccccc,
        0xdddddddd, 0xffffffff,
        0x10101010, 0x20202020
    };
    uint32_t result_got[16] =
    {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444,
        0x55555555, 0x66666666,
        0x77777777, 0x88888888,
        0x99999999, 0xaaaaaaaa,
        0xbbbbbbbb, 0xcccccccc,
        0xdddddddd, 0xffffffff,
        0x10101010, 0x20202020
    };

#ifdef PERF
    PERF_TEST_BEGIN(PERF_TEST_SLOW);
#endif /* PERF */

    data = sfzutf_ptrsize_from_str(data_p, order_data);
    if (data.len < 4096)
    {
        sfzutf_ptrsize_free(data);
        sbswhashtest_fast(data_p, order_data, result_p, cmplen);
        return;
    }

    if (result_p != NULL)
    {
        c_memcpy((void *)result_expect, result_p, cmplen);
    }

#ifdef PERF
    PERF_EXECUTE_TEST_SLOW(
    {
        SB_SW_HASH_Init(&ctx, HASH_BITS);
        SB_SW_HASH_FinalUpdate(&ctx, data.ptr, data.len, result_got);
    });
#else
    SB_SW_HASH_Init(&ctx, HASH_BITS);
    SB_SW_HASH_FinalUpdate(&ctx, data.ptr, data.len, result_got);
#endif /* PERF */

    debug288("res_expect", result_expect);
    debug288("res_received", result_got);

    if (result_p != NULL)
    {
        fail_if(c_memcmp(result_expect, result_got, sizeof(result_got)) != 0,
                "Result differs");
    }

    sfzutf_ptrsize_free(data);
}

/* end of file lib-sbswhashtest.c */
