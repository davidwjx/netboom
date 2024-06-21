/* lib-sbswecdsa256.c
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

#include "implementation_defs.h"
#include "sfzutf.h"
#include "sfzutf-utils.h"
#ifdef SBIF_ECDSA256_WORDS
#undef SBIF_ECDSA256_WORDS
#endif
#define SBIF_ECDSA256_WORDS 8
#ifdef SBIF_ECDSA256_BYTES
#undef SBIF_ECDSA256_BYTES
#endif
#define SBIF_ECDSA256_BYTES 32
#include "sb_sw_ecdsa.h"
#ifndef SBSW_CF_HASH_SHA2
#define SBSW_CF_HASH_SHA2
#endif
#include "sb_sw_hash.h"
#include "c_lib.h"

#ifdef PERF
#include "sfzutf-perf.h"
#endif /* PERF */

#include "lib-sbswecdsatest.h"

#include <stdio.h>
static void debug288(const char *s, const uint32_t *v)
{
    IDENTIFIER_NOT_USED(s);
    IDENTIFIER_NOT_USED(v);
    if (v)
    {
        L_DEBUG(SB_SW_HASH_TEST,
                "%s: %08X%08X%08X%08X%08X%08X%08X%08X%08X", s,
                v[8], v[7], v[6], v[5], v[4], v[3], v[2], v[1], v[0]);
    }
}

void sbswecdsa(const char * const data_p,
               int order_data,
               const uint32_t * const n,
               const uint32_t * const s,
               const uint32_t * const r,
               const uint32_t * const px,
               const uint32_t * const py,
               const uint32_t * const qx,
               const uint32_t * const qy,
               bool result_expected)
{
    SB_SW_HASH_Context_t ctx;
    SB_SW_ECDSA_Verify_Workspace_t wks;
    struct SB_SW_ECDSA_RS256 RS;
    struct SB_SW_ECDSA_Point256 Q;
    SfzUtfPtrSize data;
    bool res = false;
    uint32_t tmp[9] =
    {
        0x11111111, 0x22222222,
        0x33333333, 0x44444444,
        0x55555555, 0x66666666,
        0x77777777, 0x88888888,
        0x99999999,
    };
#ifdef PERF
    PERF_TEST_BEGIN(PERF_TEST_SLOW);
#endif /* PERF */

    PARAMETER_NOT_USED(n);
    PARAMETER_NOT_USED(px);
    PARAMETER_NOT_USED(py);

    c_memcpy(&Q.x, qx, sizeof(Q.x));
    c_memcpy(&Q.y, qy, sizeof(Q.y));

    c_memcpy(&RS.x, r, sizeof(RS.x));
    c_memcpy(&RS.y, s, sizeof(RS.y));

    data = sfzutf_ptrsize_from_str(data_p, order_data);

#ifdef PERF
    /* Notice: HASHing is not timed. */
#endif /* PERF */
    SB_SW_HASH_Init(&ctx, SBIF_ECDSA_BITS);
    SB_SW_HASH_FinalUpdate(&ctx, data.ptr, data.len, tmp);

    debug288("res_hash", tmp);

#ifdef PERF
    PERF_EXECUTE_TEST_SLOW(
    {
        res = SB_SW_ECDSA_Verify256(&wks, tmp, &RS, &Q);
    });
#else
    res = SB_SW_ECDSA_Verify256(&wks, tmp, &RS, &Q);
#endif /* PERF */

    fail_if(res != result_expected, "Test failed");

    sfzutf_ptrsize_free(data);
}

/* end of file lib-sbswecdsa256.c */
