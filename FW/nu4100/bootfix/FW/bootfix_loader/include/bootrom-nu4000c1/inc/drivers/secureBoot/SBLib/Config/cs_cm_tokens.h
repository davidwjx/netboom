/* cs_cm_tokens.h
 *
 * Configuration Settings for the CM Tokens module.
 */

/*****************************************************************************
* Copyright (c) 2010-2018 INSIDE Secure B.V. All Rights Reserved.
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
// make sure upper layer does noy rely on these checks!
//#define CMTOKENS_STRICT_ARGS


// use the options below to selectively remove unused functions
#ifdef IMPLDEFS_CF_DISABLE_L_DEBUG
#define CMTOKENS_REMOVE_ERROR_DESCRIPTIONS
#endif
//#define CMTOKENS_REMOVE_PARSERESPONSE_ERRORDETAILS
#define CMTOKENS_REMOVE_PARSERESPONSE_NVMERRORDETAILS
#define CMTOKENS_REMOVE_CRYPTO_3DES
#define CMTOKENS_REMOVE_CRYPTO_AES_F8
#define CMTOKENS_REMOVE_CRYPTO_ARC4
#define CMTOKENS_REMOVE_CRYPTO_CAMELLIA
#define CMTOKENS_REMOVE_CRYPTO_C2
#define CMTOKENS_REMOVE_CRYPTO_MULTI2

/* end of file cs_cm_tokens.h */
