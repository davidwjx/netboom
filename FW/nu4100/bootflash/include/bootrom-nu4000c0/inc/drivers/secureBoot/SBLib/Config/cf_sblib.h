/* cf_sblib.h
 *
 * Description: Configuration template for Secure Boot implementation.
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

// Define to remove calls to SB_Poll()
//#define SBLIB_CF_REMOVE_POLLING_CALLS

// Define to remove support for certificates in images.
//#define SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT

// Define to remove support for wrapped images.
//#define SBLIB_CF_REMOVE_IMAGE_TYPE_W

// Define to remove support for encrypted images.
//#define SBLIB_CF_REMOVE_IMAGE_TYPE_E

// Define to remove support for plaintext images.
//#define SBLIB_CF_REMOVE_IMAGE_TYPE_P

// Define to remove support for encrypted images without key.
//#define SBLIB_CF_REMOVE_IMAGE_TYPE_X

// Define to remove support for public data read.
//#define SBLIB_CF_REMOVE_PUBLICDATA

// Define to use the SBCR key from SBCommon/SB_SW_CRYPTO/src/scbr_key.c as the
// AES-WRAP unwrap key or derivation key (both refered to as SBCR) for a BLw
// image.
// This define is only meaningful for the Secure Boot variants SB_SM and SB_CM.
// These variants have access to OTP or NVM storage that can be used for
// securely storing the SBCR, for other Secure Boot targets
// SBCommon/SB_SW_CRYPTO/src/scbr_key.c is always used.
// See also: cfg_sblib.h:SBLIB_CFG_XM_IMAGE_TYPE_W_ASSET_KEY
//#define SBLIB_CF_IMAGE_TYPE_W_SBCR_KEY

/* end of file cf_sblib.h */
