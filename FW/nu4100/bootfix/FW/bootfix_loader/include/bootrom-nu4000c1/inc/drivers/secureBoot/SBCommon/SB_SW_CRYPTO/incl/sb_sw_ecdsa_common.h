/* sb_sw_ecdsa_common.h
 *
 * Description: Secure Boot ECDSA Interface.
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

#ifndef INCLUDE_GUARD_SB_SW_ECDSA_COMMON_H
#define INCLUDE_GUARD_SB_SW_ECDSA_COMMON_H

typedef struct SB_SW_ECDSA_Verify_Workspace_
{
    char unused; /* Workspace not currently used.
                    Single member included because
                    memberless structs are not allowed in ISO C. */
} SB_SW_ECDSA_Verify_Workspace_t;


#ifndef SB_CF_BIGENDIAN
static inline void
memcpyrev(void *dest,
          const void *src,
          size_t bytes)
{
    unsigned char *d = dest;
    const unsigned char *s = src;
    size_t i;

    for (i = 0; i < bytes; i++)
    {
        d[bytes - i - 1] = s[i];
    }
}
#endif

#endif /* Include Guard */

/* end of file sb_sw_ecdsa_common.h */
