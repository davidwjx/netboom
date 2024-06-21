/* sbcr_key.c
 *
 * Description: Example definition of Secure Boot Confidentiality Root Key.
 *              This file defines confidentiality root key for Secure Boot
 *              BLw images. The customer SHALL need to customize this file:
 *              i.e. replace the key with random key they shall use.
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

#include "public_defs.h"
#include "sbcr_key.h"           /* the API to implement */

#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
#define SBCR_KEY_BYTES (SBIF_CFG_CONFIDENTIALITY_BITS / 8)
#else
#define SBCR_KEY_BYTES 16
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */

/* Demonstration key for Secure Boot Confidentiality. (128 bit) */
/* The key material below shall be customized. */
#if SBCR_KEY_BYTES == 16
const uint8_t SBCommon_SBCRK[SBCR_KEY_BYTES] =
{
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F
};
#elif SBCR_KEY_BYTES == 24
const uint8_t SBCommon_SBCRK[SBCR_KEY_BYTES] =
{
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17
};
#elif SBCR_KEY_BYTES == 32
const uint8_t SBCommon_SBCRK[SBCR_KEY_BYTES] =
{
    0x00, 0x01, 0x02, 0x03,
    0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B,
    0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13,
    0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1A, 0x1B,
    0x1C, 0x1D, 0x1E, 0x1F
};
#else
#error "Unsupported value for SBCR key length."
#endif /* SBCR_KEY_BYTES */

/* end of file sbcr_key.c */
