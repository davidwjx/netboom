/**
*  File: aes_if_internal.h
*
*  Description: Definition of context structure for aes_if.c.
*
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
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
*/

#ifndef INCLUSION_GUARD_AES_IF_INTERNAL_H
#define INCLUSION_GUARD_AES_IF_INTERNAL_H

#include "public_defs.h"
#include "aes_if.h"
#include "cfg_sbif.h"

/* Define struct AES_IF_Ctx, used internally. */
struct AES_IF_Ctx
{
    uint8_t Key[SBIF_CFG_CONFIDENTIALITY_BITS / 8];
    AES_IF_ResultCode_t Result;
    uint32_t IV[4];
};

#endif /* INCLUSION_GUARD_AES_IF_INTERNAL_H */

/* end of file aes_if_internal.h */
