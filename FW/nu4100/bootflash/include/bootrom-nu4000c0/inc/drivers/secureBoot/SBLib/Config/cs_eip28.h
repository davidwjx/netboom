/* cs_eip28.h
 *
 * Configuration Settings for the EIP28 Driver Library module.
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

// set this option to enable checking of all arguments to all EIP28 functions,
// disable it to reduce code size and reduce overhead
//#define EIP28_STRICT_ARGS

// set this option to enable checking the vectors in PKA RAM
//#define EIP28_STRICT_VECTORS

// Offset of PKA RAM with respect to address of PKA registers.
#define EIP28_OFFSET_PKARAM         0x2000

// Enable the Montgomery Ladder feature.
// Other values still use the ModExpVar method using the programmed number of odd powers.
// Can only be used on EIP-28A and EIP-28B configurations with at least HW2.2/FW3.2.
//#define EIP28_MONTGOMERY_LADDER_OPTION

// define the maximum length of a vector (in bits) that is supported by the EIP28 device
//#define EIP28_VECTOR_BITS_MAX   2048        /* 2048 bits */
#define EIP28_VECTOR_BITS_MAX   4096+128
//#define EIP28_VECTOR_BITS_MAX   8192        /* 8192 bits */

// Uncomment to remove the support of Firmware download
#define EIP28_REMOVE_FIRMWARE_DOWNLOAD

// use the options below to selectively remove unused features
// optimized for Secure Boot PKA requirements
#define EIP28_REMOVE_ADD
#define EIP28_REMOVE_SUBTRACT
#define EIP28_REMOVE_SUBADD
#define EIP28_REMOVE_SHIFTRIGHT
#define EIP28_REMOVE_SHIFTLEFT
//#define EIP28_REMOVE_MULTIPLY
#define EIP28_REMOVE_MODULO_OR_COPY
//#define EIP28_REMOVE_MODULO
#define EIP28_REMOVE_DIVIDE
//#define EIP28_REMOVE_MODULUS
#define EIP28_REMOVE_COPY
//#define EIP28_REMOVE_COMPARE
//#define EIP28_REMOVE_MODINV
#define EIP28_REMOVE_MODEXP
#define EIP28_REMOVE_MODEXPCRT
//#define EIP28_REMOVE_ECCADD
//#define EIP28_REMOVE_ECCMUL
#define EIP28_REMOVE_ECCMULMONT
#define EIP28_REMOVE_DSASIGN
#define EIP28_REMOVE_DSAVERIFY
#define EIP28_REMOVE_ECCDSASIGN
#define EIP28_REMOVE_ECCDSAVERIFY

//#define EIP28_REMOVE_EIPNR_CHECK
//#define EIP28_REMOVE_BIGUINT_HELPER_FUNC
#define EIP28_REMOVE_SCAP


/* end of file cs_eip28.h */
