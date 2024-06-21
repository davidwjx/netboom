/* cs_eip123.h
 *
 * Configuration Settings for the EIP123_SL module.
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

// set this option to enable checking of all arguments to all EIP123 functions
// disable it to reduce code size and reduce overhead
//#define EIP123_STRICT_ARGS

#define EIP123_MAX_PHYSICAL_FRAGMENTS  33

// footprint reduction switches
//#define EIP123_REMOVE_VERIFYDEVICECOMMS
#define EIP123_REMOVE_GETOPTIONS
#define EIP123_REMOVE_MAILBOXACCESSCONTROL
//#define EIP123_REMOVE_LINK
//#define EIP123_REMOVE_UNLINK
//#define EIP123_REMOVE_CANWRITETOKEN

/* end of file cs_eip123.h */
