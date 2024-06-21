/* sb_ecdsa.h
 *
 * Description: Secure Boot API for Elliptic Curve Digital Signature
 *              Algorithm (ECDSA) images.
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

#ifndef INCLUDE_GUARD_SB_ECDSA_H
#define INCLUDE_GUARD_SB_ECDSA_H

#include "public_defs.h"
#include "sbif_ecdsa.h"         // Image format
#include "cfg_sblib.h"
#include "sbif_vector.h"
#include "sb_result.h"

/** Confidentiality key type */
typedef enum
{
    SB_CONFIDENTIALITY_KEY_TYPE_AUTO, /* For BLp, BLe, BLw */
    SB_CONFIDENTIALITY_KEY_TYPE_BLX,  /* For BLx, key provided via independent channel */
    SB_CONFIDENTIALITY_KEY_TYPE_BLW,  /* For BLw, not using built-in key */
} SB_ConfidentialityKeyType_t;

/** Storage area for SB_ECDSA_*. */
typedef struct
{
    union
    {
        uint32_t Alignment;                     /** Alignment */
        uint8_t  Size[SBLIB_CFG_STORAGE_SIZE];  /** Workspace size */
    }
    Union;
} SB_StorageArea_t;

/* SB_StorageArea_t as required for Secure Boot ECDSA verification. */
typedef SB_StorageArea_t SB_ECDSA_StorageArea_t;


/** Verify an ECDSA format image.

    @param Storage_p
    Pointer to caller allocated memory area. This memory area is used
    as work memory by the function. No initialisation required.

    @param PollParam_p
    An value passed to polling functions when enabled.

    @param PublicKey_p
    Must be initialised to ECDSA public key by the
    caller. The signing tool can generate a char array compatible with
    this structure from a DER format ECDSA public key.

    @param Hash_p
    Points to the hash of the public key. If the public key is not included
    in the image, this may be NULL. If the public key is included in the image,
    check the actual hash of the public key against the externally supplied
    hash.

    @param Header_p
    Must point to the header of the image. Must be
    followed by a number of certificates shown in the CertificateCount
    field in the header. SBIF_ECSDA_HEADER_SIZE() macro can be used to
    get the size of the header including certificates in bytes.

    @param DataVectors_p
    Must point to an initialised array of
    SBIF_SGVector_t structures. The sum of DataLen fields in the array
    shall match the ImageLen field in the header.

    @param VectorCount
    The number of entries in the array pointed to by DataVectors_p.

    @return
    Returns SB_SUCCESS on success. All other values indicate that
    an error has occurred.

*/
SB_Result_t
SB_ECDSA_Image_Verify(
    SB_StorageArea_t * const             Storage_p,
    void *                               PollParam_p,
    const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
    const uint8_t * const                Hash_p,
    const SBIF_ECDSA_Header_t *          Header_p,
    const SBIF_SGVector_t *              DataVectors_p,
    uint32_t                             VectorCount);


/** Verify an ECDSA format image.
    If image has been encrypted (BLe or BLw) the image shall be first
    decrypted (to target buffer) and then verified.

    @param Storage_p
    Pointer to caller allocated memory area. This memory area is used
    as work memory by the function. No initialisation required.

    @param PollParam_p
    An value passed to polling functions when enabled.

    @param PublicKey_p
    Must be initialised to ECDSA public key by the
    caller. The signing tool can generate a char array compatible with
    this structure from a DER format ECDSA public key.

    @param Hash_p
    Points to the hash of the public key. If the public key is not included
    in the image, this may be NULL. If the public key is included in the image,
    check the actual hash of the public key against the externally supplied
    hash.

    @param Header_p
    Must point to the header of the image. Must be
    followed by a number of certificates shown in the CertificateCount
    field in the header. SBIF_ECSDA_HEADER_SIZE() macro can be used to
    get the size of the header including certificates in bytes.

    @param DataVectorsInput_p
    Must point to an initialised array of
    SBIF_SGVector_t structures. The sum of DataLen fields in the array
    shall match the ImageLen field in the header.
    The contents of these arrays are not modified.

    @param DataVectorsOutput_p
    Must point to an initialised array of
    SBIF_SGVector_t structures. Each vector entry size must be equal to
    corresponding DataVectorsInput_p entry.

    @param VectorCount
    The number of entries in the array pointed to by DataVectors_p.

    @return
    Returns SB_SUCCESS on success. All other values indicate that
    an error has occurred.

    @note DataVectorsOutput_p can be used by the function for processing.
    Therefore, during function execution, the data to be validated can
    be available via vectors pointed by DataVectorsOutput_p. However,
    if the function fails, the contents of DataVectorsOutput_p segments
    have been invalidated. Therefore, the caller does not need to clear
    DataVectorsOutput_p after the call, but cannot trust its contents unless
    the call succeeded.

    @note It is allowed to specify DataVectorsInput_p == DataVectorsOutput_p
    for inline operation.
*/
SB_Result_t
SB_ECDSA_Image_DecryptAndVerify(
    SB_StorageArea_t * const             Storage_p,
    void *                               PollParam_p,
    const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
    const uint8_t * const                Hash_p,
    const SBIF_ECDSA_Header_t *          Header_p,
    const SBIF_SGVector_t *              DataVectorsInput_p,
    const SBIF_SGVector_t *              DataVectorsOutput_p,
    uint32_t                             VectorCount);


/** Verify an ECDSA format image using specified key.
    This function decrypts specified image with specified key and then
    verifies the image. ConfidentialityKeyType describes the type of
    provided key material.
    If ConfidentialityKeyType is SB_CONFIDENTIALITY_KEY_TYPE_AUTO, the
    function behaves identically to SB_ECDSA_Image_DecryptAndVerify.

    If ConfidentialityKeyType is SB_CONFIDENTIALITY_KEY_TYPE_BLW, the
    function must be passed in unwrapping key for BLw image. If the image
    is not BLw image, the decryption will fail. The length of the key
    is defined in SBCommon/Config/cfg_sbif.h (currently 16 bytes).

    If ConfidentialityKeyType is SB_CONFIDENTIALITY_KEY_TYPE_BLX, the
    function must be passed in unwrapping key for BLx image. If the image
    is not BLx image, the decryption will fail. The length of the key
    is defined in SBCommon/Config/cfg_sbif.h (currently 16 bytes).

    Note: This API is not supported by SB_SIMPLE.

    @param Storage_p
    Pointer to caller allocated memory area. This memory area is used
    as work memory by the function. No initialisation required.

    @param PollParam_p
    An value passed to polling functions when enabled.

    @param PublicKey_p
    Must be initialised to ECDSA public key by the
    caller. The signing tool can generate a char array compatible with
    this structure from a DER format ECDSA public key.

    @param Hash_p
    Points to the hash of the public key. If the public key is not included
    in the image, this may be NULL. If the public key is included in the image,
    check the actual hash of the public key against the externally supplied
    hash.

    @param Header_p
    Must point to the header of the image. Must be
    followed by a number of certificates shown in the CertificateCount
    field in the header. SBIF_ECSDA_HEADER_SIZE() macro can be used to
    get the size of the header including certificates in bytes.

    @param DataVectorsInput_p
    Must point to an initialised array of
    SBIF_SGVector_t structures. The sum of DataLen fields in the array
    shall match the ImageLen field in the header.
    The contents of these arrays are not modified.

    @param DataVectorsOutput_p
    Must point to an initialised array of
    SBIF_SGVector_t structures. Each vector entry size must be equal to
    corresponding DataVectorsInput_p entry.

    @param VectorCount
    The number of entries in the array pointed to by DataVectors_p.

    @param ConfidentialityKeyType
    Type of key data pointed by ConfidentialityKey. Also determines allowed
    boot image formats.

    @param ConfidentialityKey
    Pointer to key data of key to use for confidentiality protection (BLw/BLx).

    @return
    Returns SB_SUCCESS on success. All other values indicate that
    an error has occurred.

    @note DataVectorsOutput_p can be used by the function for processing.
    Therefore, during function execution, the data to be validated can
    be available via vectors pointed by DataVectorsOutput_p. However,
    if the function fails, the contents of DataVectorsOutput_p segments
    have been invalidated. Therefore, the caller does not need to clear
    DataVectorsOutput_p after the call, but cannot trust its contents unless
    the call succeeded.

    @note It is allowed to specify DataVectorsInput_p == DataVectorsOutput_p
    for inline operation.
*/
SB_Result_t
SB_ECDSA_Image_DecryptWithKeyAndVerify(
    SB_StorageArea_t * const             Storage_p,
    void *                               PollParam_p,
    const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
    const uint8_t * const                Hash_p,
    const SBIF_ECDSA_Header_t *          Header_p,
    const SBIF_SGVector_t *              DataVectorsInput_p,
    const SBIF_SGVector_t *              DataVectorsOutput_p,
    uint32_t                             VectorCount,
    SB_ConfidentialityKeyType_t          ConfidentialityKeyType,
    const uint8_t *                      ConfidentialityKey);


/** Get polled during verification.

    Function shall be implemented by the caller of the function
    SB_ECDSA_Image_Verify and is called from the implementation of the
    function SB_ECDSA_Image_Verify.

    @param PollParam_p
    The value pass to SB_ECDSA_Image_Verify function as PollParam_p
    parameter.


    @return
    Should return true while it is OK for the implementation to
    continue execution. Can return false to break execution of
    SB_ECDSA_Image_Verify function. In this case SB_ECSDA_Image_Verify
    will return SB_ERROR_POLL.
 */
extern bool
SB_Poll(void * PollParam_p);

/** Block size for SB_ReadData function. */
#define SB_READ_DATA_BLOCKSIZE 256
#define SB_READ_DATA_FAILURE ((uint32_t) 0)

/** SB_ReadData: Request data to be available.

    When Secure Boot is about to read data via DataVectors_p or
    DataVectorsIn_p, it uses this function to ensure the data is readable.
    This allows the caller of SB_ECDSA_Image_Verify etc functions to
    parallelize verification (and optional decryption) with loading of the
    Secure Boot image from (potentially) slow storage media.

    Function shall be implemented by the caller of the function
    SB_Image_Verify and is called from the implementation of the
    function SB_Image_Verify.

    This function is called before attempting to read
    input vectors in DataVectorsInput_p or DataVectors_p.
    The addresses given for this function are taken from
    DataVectorsInput_p.

    Before Secure Boot accesses any data within
    DataVectorsInput_p, it'll call this function. If the
    data is not yet available on the memory, this function
    must make it available.

    Instead of making entire requested available, this
    function is allowed to read any multiple of
    SB_READ_DATA_BLOCKSIZE. (The size to read is not
    neccessarily multiple of SB_READ_DATA_BLOCKSIZE, and
    therefore, the last block is allowed to be
    non-multiple of SB_READ_DATA_BLOCKSIZE.)

    @param PollParam_p
    The value passed to SB_Image_Verify function as PollParam_p
    parameter.

    @param Data_p
    The Data address in beginning of the range to make available
    for reading.

    @param DataLen
    The amount of databytes to make available for reading.

    @return
    On successful execution this function shall return
    DataLen (entire range readable) or
    0 < SB_READ_DATA_BLOCKSIZE * n < DataLen.
    If function execution is not successful, the function is allowed
    to return SB_READ_DATA_FAILURE.
    In this case, SB_Image_Verify will return SB_ERROR_POLL.
*/
extern uint32_t
SB_ReadData(
    void *     PollParam_p,
    uint32_t * Data_p,
    uint32_t   DataLen);

#endif /* Include guard */

/* end of file sb_ecdsa.h */
