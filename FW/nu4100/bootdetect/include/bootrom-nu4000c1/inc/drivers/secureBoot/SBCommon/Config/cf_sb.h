/* cf_sb.h
 *
 * Description: Common secure boot definitions
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

/* These common definitions describe the platform and (potentially) advice in
 * platform specific optimizations.
 */

/* Specify processor endianness
 * Define either of following if not using autoconf.
 * With autoconf, none should be defined as endian
 * is detected automatically.
 */
#define SB_CF_LITTLEENDIAN
/* #define SB_CF_BIGENDIAN */

/* Specific for the software implementation of AES (Rijndael). This define
 * specifies if the code must be optimized either for speed (default) or size.
 * If the define is uncommented the software implementation of AES is optimized
 * for size, with which roughly 4k bytes of ROM size is saved but the encrypt/
 * decrypt speed will be impacted.
 */
#define AES_OPTIMIZE_ROM_SIZE


/* end of file cf_sb.h */
