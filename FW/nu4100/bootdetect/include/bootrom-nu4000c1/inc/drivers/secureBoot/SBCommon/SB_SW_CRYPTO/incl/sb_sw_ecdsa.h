/* sb_sw_ecdsa.h
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

/*
    Secure Boot ECDSA Verification uses this header to specify interface
    it uses to interact with ECDSA verification.

    Other software may also use this interface to access other parts of
    the ECDSA functionality, including key generation and signing.

    The interface depends on SBIF_ECDSA_WORDS, which declares the size of
    ECDSA calculated (and therefore) the size of output of output signature
    and domain parameters. The SBIF_ECDSA_WORDS parameter shall be 7/8/12/17
    for this source.
*/

#ifndef INCLUDE_GUARD_SB_SW_ECDSA_H
#define INCLUDE_GUARD_SB_SW_ECDSA_H

#include "public_defs.h"
#include "sbif_ecdsa.h"

#if SBIF_ECDSA_WORDS == 7               /* P-224 */

#include "sb_sw_ecdsa224.h"

#elif SBIF_ECDSA_WORDS == 8             /* P-256 */

#include "sb_sw_ecdsa256.h"

#elif SBIF_ECDSA_WORDS == 12            /* P-384 */

#include "sb_sw_ecdsa384.h"

#elif SBIF_ECDSA_WORDS == 17            /* P-521 */

#include "sb_sw_ecdsa521.h"

#else
#error "Unsupported SBIF_ECDSA_WORDS"
#endif /* Include Guard */

#endif /* Include Guard */

/* end of file sb_sw_ecdsa.h */
