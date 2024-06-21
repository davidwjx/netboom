/* sbsm_eip130_asset.c
 *
 * Description: Include appropriate file from SB_HYBRID.
 */

/*****************************************************************************
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
* ESSEmbeddedHW-Support@insidesecure.com.
*****************************************************************************/

/* Request SB_HYBRID's "personality" SB_SM. */
#ifndef SB_SM
#define SB_SM
#endif /* !defined SB_SM */

/* The functionality is actually implemented in SB_HYBRID. */
#include "sbhybrid_eip130_asset.c"

/* end of file sbsm_eip130_asset.c */
