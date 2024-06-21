/**
* File: cfg_sbif.h
*
* Description : Secure boot constants (special template to force 521-bit)
*
* Copyright (c) 2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_CFG_SBIF_H_
#define INCLUDE_GUARD_CFG_SBIF_H_

/* Number of bits to use in ECDSA calculation.
 * Notice this is set to 521 to force support for ECDSA521
 */
#define SBIF_CFG_ECDSA_BITS           521

/* Number of AES bits to use in confidentiality protection.
 * Notice this is set to 128 to force support for AES-128
 */
#define SBIF_CFG_CONFIDENTIALITY_BITS 128

/* Minimum value for ROLLBACK ID attribute.
 * (Optional: if specified, SBIF will enforce values for rollback ID.)
 */
#define SBIF_CFG_ATTRIBUTE_MINIMUM_ROLLBACK_ID  1

#endif /* INCLUDE_GUARD_CFG_SBIF_H_ */
