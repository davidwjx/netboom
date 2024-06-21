/*
 * File: sbif_ecdsa.h
 *
 * Description: Secure Boot image format with Elliptic Curve Digital Signature
 *              Algorithm (ECDSA).
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

#ifndef INCLUSION_GUARD_SBIF_ECSDA_H
#define INCLUSION_GUARD_SBIF_ECSDA_H

#include "public_defs.h"
#include "cfg_sbif.h"


/** Tag for image type BLp. */
#define SBIF_IMAGE_BLTp            0x424c70

/** Tag for image type BLw. */
#define SBIF_IMAGE_BLTw            0x424c77

/** Tag for image type BLe. */
#define SBIF_IMAGE_BLTe            0x424c65

/** Tag for image type BLx. */
#define SBIF_IMAGE_BLTx            0x424c78

/** Current image version number for BL images.  */
#define SBIF_VERSION          2U

/** Values for PubKeyType field: describes the location of the public key. */
#define SBIF_PUBKEY_TYPE_ROM       0x1
#define SBIF_PUBKEY_TYPE_OTP       0x2
#define SBIF_PUBKEY_TYPE_IMAGE     0x3


/** Macro to get version from the type field. */
#define SBIF_TYPE_VERSION(type)    ((type) & 0xff)

/** Macro to get type from the type field. */
#define SBIF_TYPE_TYPE(type)       ((type) >> 8)

/** Encryption key length. */
#define SBIF_ENCRYPTIONKEY_LEN ((SBIF_CFG_CONFIDENTIALITY_BITS / 32) + 2)
#define SBIF_ENCRYPTIONKEY256_LEN ((256 / 32) + 2)

/** Encryption key iv length. */
#define SBIF_ENCRYPTIONIV_LEN (128 / 32)

#ifdef SBIF_CFG_ECDSA_BITS
#define SBIF_ECDSA_BITS_DO_U(a) a##U
#define SBIF_ECDSA_BITS_U(a)    SBIF_ECDSA_BITS_DO_U(a)
#define SBIF_ECDSA_BITS         SBIF_ECDSA_BITS_U(SBIF_CFG_ECDSA_BITS)
#else /* !SBIF_CFG_ECDSA_BITS */
/* Default value for SBIF_ECDSA_BITS. */
#define SBIF_ECDSA_BITS         224U
#endif /* SBIF_CFG_ECDSA_BITS */

#if SBIF_CFG_ECDSA_BITS == 224
#define SBIF_ECDSA_PAD_BITS 32
#elif !defined(SBIF_CFG_ECDSA_BITS)
#define SBIF_ECDSA_PAD_BITS 32
#endif /* SBIF_CFG_ECDSA_BITS */


/** ECDSA bytes. */
#define SBIF_ECDSA_BYTES        (((SBIF_ECDSA_BITS) + 7) >> 3)

/** ECDSA words. */
#define SBIF_ECDSA_WORDS        (((SBIF_ECDSA_BITS) + 31) >> 5)

/** Maximum number of attribute elements. */
#if SIGNTOOL == 1
#define SBIF_NUM_ATTRIBUTES 16
#else
#if (SBIF_ECDSA_BYTES == 28) || (SBIF_ECDSA_BYTES == 32)
#define SBIF_NUM_ATTRIBUTES 8
#else
#define SBIF_NUM_ATTRIBUTES 16
#endif /* (SBIF_ECDSA_BYTES == 28) || (SBIF_ECDSA_BYTES == 32) */
#endif /* SIGNTOOL == 1 */


/** ECDSA signature. */
typedef struct
{
    uint8_t r[SBIF_ECDSA_BYTES];    /** r. */
    uint8_t s[SBIF_ECDSA_BYTES];    /** s. */
#ifdef SBIF_ECDSA_PAD_BITS
    /* Notice: add padding to get the same size for 224 as for 256-bit ECC */
    uint8_t pad[SBIF_ECDSA_PAD_BITS / 8 * 2];
#endif /* SBIF_ECDSA_PAD_BITS */
}
SBIF_ECDSA_Signature_t;


/** ECDSA public key. */
typedef struct
{
    uint8_t Qx[SBIF_ECDSA_BYTES];    /** Qx. */
    uint8_t Qy[SBIF_ECDSA_BYTES];    /** Qy. */
}
SBIF_ECDSA_PublicKey_t;


/** ECDSA certificate. */
typedef struct
{
    SBIF_ECDSA_PublicKey_t PublicKey;    /** Public key. */
    SBIF_ECDSA_Signature_t Signature;    /** Signature. */
}
SBIF_ECDSA_Certificate_t;

/** Define Attribute type and its allowed constants. */
typedef uint32_t SBIF_AttributeElementType_t;

#define SBIF_ATTRIBUTE_UNUSED  0     /* All element positions not used. */
#define SBIF_ATTRIBUTE_VERSION 1     /* Version field for attribute array. */
#define SBIF_ATTRIBUTE_ROLLBACK_ID 2 /* Optional rollback identifier */

/** Minimum attribute version. */
#define SBIF_ATTRIBUTE_VERSION_CURRENT 0 /** First version. */

/** Minimum current rollback identifier.
    SecureBoot shall not process images with rollback counter less than this. */
#ifdef SBIF_CFG_ATTRIBUTE_MINIMUM_ROLLBACK_ID
#define SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID \
        SBIF_CFG_ATTRIBUTE_MINIMUM_ROLLBACK_ID
#endif /* SBIF_CFG_ATTRIBUTE_MINIMUM_ROLLBACK_ID */

/** Attribute data (incl. version id). */
typedef struct
{
    SBIF_AttributeElementType_t ElementType;
    uint32_t ElementValue;
}
SBIF_AttributeElement_t;

typedef struct SBIF_Attributes
{
    /** Attribute data element. */
    SBIF_AttributeElement_t AttributeElements[SBIF_NUM_ATTRIBUTES];
}
SBIF_Attributes_t;


/**
    Signing header for the images.
 */
typedef struct
{
    uint32_t               Type;                /** Type. */
    uint32_t               PubKeyType;          /** Type of public key */
    SBIF_ECDSA_Signature_t Signature;           /** Signature. */
    SBIF_ECDSA_PublicKey_t PublicKey;           /** Public key (if included in image). */
#ifdef SBIF_ECDSA_PAD_BITS
    /* Notice: add padding to get the same size for 224 as for 256-bit ECC */
    uint8_t pad[SBIF_ECDSA_PAD_BITS / 8 * 2];
#endif /* SBIF_ECDSA_PAD_BITS */
    uint32_t               EncryptionKey[SBIF_ENCRYPTIONKEY256_LEN]; /** Key. */
    uint32_t               EncryptionIV[SBIF_ENCRYPTIONIV_LEN]; /** IV. */
    uint32_t               ImageLen;            /** Image length. */
    SBIF_Attributes_t      ImageAttributes;     /** Image attributes. */
    uint32_t               CertificateCount;    /** Certificate count. */
} SBIF_ECDSA_Header_t;


/*
  SBIF_ECDSA_GET_HEADER_SIZE

  Return total size of header including the space required by
  certificates. Returns 0 on error.
  Macro needs to be provided with known maximum number of bytes
  it is allowed to examine.
*/

/** Header size. */
#define SBIF_ECDSA_GET_HEADER_SIZE(Header_p, AccessibleByteSize)  \
    SBIF_ECDSA_GetHeaderSize((const void *)(Header_p), (AccessibleByteSize))

/* Helper inline function for fetching image size.
   Conventionally used via SBIF_ECDSA_GET_HEADER_SIZE macro. */
static inline uint32_t SBIF_ECDSA_GetHeaderSize(
    const SBIF_ECDSA_Header_t * const Header_p,
    const uint32_t AccessibleByteSize)
{
    uint32_t size = 0;

    if (AccessibleByteSize >= sizeof(SBIF_ECDSA_Header_t))
    {
        uint8_t certificateCount = *(((uint8_t *)&(Header_p->CertificateCount)) + 3);
        /* NOTE: Currently up-to 8 certificates are supported. */
        if (certificateCount <= 8)
        {
            size = sizeof(SBIF_ECDSA_Header_t) + (certificateCount * sizeof(SBIF_ECDSA_Certificate_t));
        }
    }

    return size;
}


#endif

/* end of file sbif_ecdsa.h */
