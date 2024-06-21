/* sbhybrid_eip28_ecdsa_verify.h
 *
 * Description: Secure boot library: Internal definitions for
 *              ECDSA verify with EIP-28.
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

#ifndef INCLUSION_GUARD_EIP28_ECDSA_VERIFY_H
#define INCLUSION_GUARD_EIP28_ECDSA_VERIFY_H

#include "eip28.h"

typedef struct
{
    uint32_t Step;                  // FSM step of ECDSA calculation
    uint32_t Value_w_Len;           // Length of w value; needed on multiple steps
    uint8_t * Value_e_p;            // Pointer to digest value (length SBHYBRID_DIGEST_BYTES)

    Device_Handle_t Device_EIP28;   // Device handle
    EIP28_IOArea_t EIP28_IOArea;    // Device IO area
}
SBHYBRID_EcdsaContext_t;

void
SBHYBRID_EIP28_EcdsaVerify_Init(
    SBHYBRID_EcdsaContext_t * const Verify_p,
    const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
    const SBIF_ECDSA_Signature_t * const Signature_p);

SB_Result_t
SBHYBRID_EIP28_EcdsaVerify_RunFsm(
    SBHYBRID_EcdsaContext_t * const Verify_p);

void
SBHYBRID_EIP28_EcdsaVerify_SetDigest(
    SBHYBRID_EcdsaContext_t * const Verify_p,
    uint8_t * Digest_p);     // length SBHYBRID_DIGEST_BYTES

#endif /* Include Guard */

/* end of file sbhybrid_eip28_ecdsa_verify.h */
