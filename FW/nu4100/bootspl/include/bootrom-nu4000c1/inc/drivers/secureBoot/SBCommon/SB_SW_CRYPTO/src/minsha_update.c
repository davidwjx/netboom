/* minsha_update.c
 *
 * Description: Hash Update APIs
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
#include "sb_sw_hash.h"
#include "sb_sw_endian.h"

static void
SB_SW_HASH_FinalBlock(
    SB_SW_HASH_Context_t * const ctx_p,
    const uint8_t * const data_p,
    const uint32_t input_part_len,
    uint32_t * digest)
{
    unsigned int i;
    union
    {
        uint32_t words[128 / 4];
        uint8_t bytes[128];
    } workblock;

    for(i = 0; i < input_part_len; i++)
    {
        workblock.bytes[i] = data_p[i];
    }
    workblock.bytes[i++] = 0x80;

    for( ; i < ctx_p->blocksize; i++)
    {
        workblock.bytes[i] = 0;
    }

    if (ctx_p->blocksize == 64)
    {
        if (input_part_len > 440 / 8)
        {
            /* Dual block. */
            SB_SW_HASH_Transform(ctx_p, workblock.bytes);
            for(i = 0; i < 60 / sizeof(uint32_t); i++)
            {
                workblock.words[i] = 0;
            }
        }
    }
    else
    {
        if (input_part_len > 888 / 8)
        {
            /* Dual block. */
            SB_SW_HASH_Transform(ctx_p, workblock.bytes);
            for(i = 0; i < 124 / sizeof(uint32_t); i++)
            {
                workblock.words[i] = 0;
            }
        }
    }

    /* Add length */
    ctx_p->bitcount += input_part_len << 3;
    workblock.words[(ctx_p->blocksize >> 2) - 1] = SB_SW_CPU_TO_BE32(ctx_p->bitcount);
    SB_SW_HASH_Transform(ctx_p, workblock.bytes);

    {
        unsigned int n = ctx_p->type / sizeof(uint32_t) / 8;
        for(i = 0; i < n; i++)
        {
            digest[i] = ctx_p->state[n - i - 1];
        }
    }
}

void SB_SW_HASH_Update(
    SB_SW_HASH_Context_t * const ctx_p,
    const uint8_t * const data_p,
    const uint32_t input_part_len)
{
    uint32_t i;

    for(i = 0; i < input_part_len; i += ctx_p->blocksize)
    {
        SB_SW_HASH_Transform(ctx_p, &data_p[i]);
    }
    ctx_p->bitcount += input_part_len << 3;
}

void SB_SW_HASH_FinalUpdate(
    SB_SW_HASH_Context_t * const ctx_p,
    const uint8_t * const data_p,
    const uint32_t len,
    uint32_t * digest)
{
    uint32_t first_len;
    uint32_t final_len = len;

    if (len >= ctx_p->blocksize)
    {
        first_len = len & (~(ctx_p->blocksize - 1));
        final_len -= first_len;

        L_TRACE(SB_SW_HASH, "Partitioned hash: %u bytes update", first_len);

        SB_SW_HASH_Update(ctx_p, data_p, first_len);
    }
    else
    {
        first_len = 0;
    }

    L_TRACE(SB_SW_HASH, "Final hash: %u bytes final len", final_len);

    SB_SW_HASH_FinalBlock(ctx_p, &data_p[first_len], final_len, (uint32_t *)digest);
}

/* end of file minsha_update.c */
