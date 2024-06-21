/* cs_eip130_token.h
 *
 * Configuration Settings for the EIP130 Token helper functions.
 */

/*****************************************************************************
* Copyright (c) 2014-2018 INSIDE Secure B.V. All Rights Reserved.
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

// include cf_impldefs.h to get IMPLDEFS_CF_DISABLE_L_DEBUG
#include "cf_impldefs.h"

// this option enables function call parameter checking
// disable it to reduce code size and reduce overhead
// make sure upper layer does not rely on these checks!
//#define EIP130TOKEN_STRICT_ARGS

// Use the options below to selectively enable specific functions
//#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_CCM
//#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_GCM
//#define EIP130TOKEN_ENABLE_SYM_ALGO_AES_F8
//#define EIP130TOKEN_ENABLE_SYM_ALGO_CHACHA20
//#define EIP130TOKEN_ENABLE_ENCRYPTED_VECTOR

/* end of file cs_eip130_token.h */

