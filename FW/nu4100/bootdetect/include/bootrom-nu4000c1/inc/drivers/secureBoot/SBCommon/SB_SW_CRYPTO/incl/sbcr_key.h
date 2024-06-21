/* sbcr_key.h
 *
 * Description: Internal declaration of array containing Secure Boot
 *              Confidentiality Root Key.
 *              This declaration is for aes_if.c implementation in SW,
 *              thus used by SB_SW and UpdateTool.
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

#ifndef INCLUDE_GUARD_SBCR_KEY_H
#define INCLUDE_GUARD_SBCR_KEY_H

#include "public_defs.h"
#include "cfg_sbif.h"

/* Array containing Secure Boot Confidentiality Root Key.
   The value is defined in sbcr_key.c. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
extern const uint8_t SBCommon_SBCRK[SBIF_CFG_CONFIDENTIALITY_BITS / 8];
#else
extern const uint8_t SBCommon_SBCRK[4 * 4];
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */

#endif /* Include Guard */

/* end of file sbcr_key.h */
