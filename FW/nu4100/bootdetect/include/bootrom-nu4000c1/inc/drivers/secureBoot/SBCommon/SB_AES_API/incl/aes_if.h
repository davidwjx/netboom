/**
*  File: aes_if.h
*
*  Description: Minimalistic interface to AES algorithm.
*               This interface can be implemented by both SW and HW
*               AES implementations. Interface is minimalistic to allow
*               small footprint.
*               This interface is used by SBIF_TOOLS to get access to
*               cryptographic hardware or software implementation.
*
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
* to ESSEmbeddedHW-Support@insidesecure.com.
*/


#ifndef INCLUSION_GUARD_AES_IF_H
#define INCLUSION_GUARD_AES_IF_H

#include "public_defs.h"

/** About implementing AES_IF:

    AES_IF requires AES (Rijndael) implementation and support
    for AES ECB/CBC encryption/decryption and AES-WRAP mechanisms.
    Key handling for AES-WRAP is performed by reference, therefore
    making it possible to use on-HW HUK.

    Contexts: AES_IF implementation needs to support only single
    initialized context at a time.

    Keys: Each context allows using single key at a time.
    Key loading and key generation operations make newly loaded/generated
    key current key.

    Error handling: The implementation is allowed to provide only
    basic error handling. The errors that must be detected are:
    all possible failures in key generation, any failure in
    AES_IF_Ctx_SetKEK() operation, and detected integrity failure in
    wrapped key. Implementation is not restricted by defined return
    values for returning errors, instead the implementation may return
    any 32-bit non-zero values as errors.

    Multithreading: AES_IF implementation does not need to support
    multithreading. It is only allowed to have single active function
    call at once with any AES_IF context, therefore if only single
    context is supported multithreading cannot occur.

    IV: IV can be loaded with special function for IV loading.
    The IV is only for use by AES_IF_CBC_*crypt() functions, and IV has to
    be loaded before such operations. IV only needs to remain valid for
    sequence of same operations, i.e. IV shall be reloaded after any
    other cryptographic operation.

    Encryption/decryption: All cryptographic operations must support cases
    where input and output is provided on any byte boundary. However,
    only case of overlapping input and output that needs to be supported is
    when input and output pointers are exactly the same pointers, arbitrary
    overlap need not be supported.
    Implementation does not need to check operation sizes to be multiples of
    16, it is responsibility of user of the interface to make sure they are.
*/


/** Define struct AES_IF_Ctx which is pointed by
    AES_IF_Ctx_Ptr_t and const_AES_IF_Ctx_Ptr_t types.

    Note: This header file does not define struct AES_IF_Ctx,
          the type internals depend AES_IF implementation
*/
struct AES_IF_Ctx;
typedef struct AES_IF_Ctx * const AES_IF_Ctx_Ptr_t;
typedef const struct AES_IF_Ctx * const const_AES_IF_Ctx_Ptr_t;

/** Error codes od AES_IF operations. */
typedef enum
{
    AES_IF_RESULT_SUCCESS,
    AES_IF_RESULT_GENERIC_ERROR
    /* Other result codes may be defined as suitable for layer below AES_IF. */
} AES_IF_ResultCode_t;

/* Block size for AES_IF: 16 bytes (the blocksize of AES cipher). */
#define AES_IF_BLOCK_SIZE 16

/** Function to determine size of context structure
    this AES_IF implementation needs.

    @return
    Returns size of context required for AES_IF implementation operation. */
uint32_t AES_IF_Ctx_GetSize(void);


/** Function to initialize allocated context for AES_IF.

  @param Ctx_p
  Context pointer that points to memory area containing at least
  AES_IF_Ctx_GetSize() available bytes.
  Similarly than malloc() allocated memory, the pointer Ctx_p must be
  suitably aligned for any kind of variable.

  @return
  Returns AES_IF_SUCCESS if context has been successfully initialized.
  The initialized context should be uninitialized with AES_IF_Ctx_Uninit()
  once the user of the context is finished with it.

  @note
  AES_IF implementation only needs to allow at most one initialized
  context at any time.
 */
AES_IF_ResultCode_t AES_IF_Ctx_Init(AES_IF_Ctx_Ptr_t Ctx_p);


/** Function to free context for AES_IF.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().
  After this call the pointer is no longer initialized, but the
  memory area is not freed to free memory pool.
 */
void AES_IF_Ctx_Uninit(AES_IF_Ctx_Ptr_t Ctx_p);


/** Function to check if error has occurred in cryptographic operations
    performed with a context. This function is useful to centralize
    error handling to key locations within the software instead of having
    to check results after each call.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @note The result code stored in Ctx_p is reset once its read.
 */
AES_IF_ResultCode_t AES_IF_Ctx_GetError(AES_IF_Ctx_Ptr_t Ctx_p);

/** Function loads specified key to AES_IF implementation for purpose
    of performing encryption and/or decryption operations.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @param Key_p
  Array of octets containing a key.

  @param KeyLength
  Length of key provided in bits. Implementation only needs to support 128.

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note The key loaded with this function shall overwrite existing key
        loaded.
 */
void AES_IF_Ctx_LoadKey(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const Key_p,
                        const uint32_t KeyLength);


/** Function loads specified wrapped key to AES_IF implementation for purpose
    of performing encryption and/or decryption operations.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @param Wrap_p
  Array of octets wrapped with HUK containing a key.

  @param WrapLength
  Length of wrapped key provided in bits.
  Implementation only needs to support 192.

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note The key loaded with this function shall overwrite existing key
        loaded. The KEK set with AES_IF_Ctx_SetKEK() function (which must
        be called just prior this call) can not be used after this call.
 */
void AES_IF_Ctx_LoadWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                               const void * const Wrap_p,
                               const uint32_t WrapLength);


/** Function loads specified KEK (Key Encryption Key) key to
    AES_IF implementation for purpose of performing a wrapping or
    unwrapping operation.
    Because the hardware or software on platform decides what KEK keys
    can be used, the function accepts generic implementation dependent
    description of KEK to use.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @param KEKIdentification_p
  Optional array of octets to identify KEK key to use.
  It is up-to AES_IF implementation to how the the array of
  octets are used.

  @param KEKIdentificationSize
  Size of KEKIdentification_p array provided in bytes.
  If KEKIdentification_p is NULL the size must be zero.

  @note
  If KEKIdentificationSize is zero, the implementation should attempt to
  use primary hardware unique key available on the platform.
  If KEKIdentificationSize is 16 or 32, the implementation may attempt to use
  KEKIdentification_p as the raw data containing key to use.

  @note
  This function does not return error code.
  AES_IF_Ctx_GetError() can be called user to detect errors.

  @note
  The key loaded with this function shall overwrite existing key
  loaded. Encryption or decryption calls may not be performed after this
  call no matter if the call failed or succeeded.
 */
void AES_IF_Ctx_SetKEK(AES_IF_Ctx_Ptr_t Ctx_p,
                       const void * const KEKIdentification_p,
                       const uint32_t KEKIdentificationSize);


/** Function loads KEK (Key Encryption Key) key to
    AES_IF implementation for purpose of performing a wrapping or
    unwrapping operation. This KEK key is derived from a specified KDK
    (Key Derivation Key).
    Because the hardware or software on platform decides what KDK keys
    can be used, the function accepts generic implementation dependent
    description of KEK to use.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @param KDKIdentification_p
  Optional array of octets to identify KDK key to use.
  It is up-to AES_IF implementation to how the the array of
  octets are used.

  @param KDKIdentificationSize
  Size of KDKIdentification_p array provided in bytes.
  If KDKIdentification_p is NULL the size must be zero.

  @param DeriveInfo_p
  Array of octets that are used to derive the KEK from the KDK.

  @param DeriveInfoSize
  Size of DeriveInfo_p provided in bytes.

  @note
  If KDKIdentificationSize is zero, the implementation should attempt to
  use primary hardware unique key available on the platform.
  If KDKIdentificationSize is 16 or 32, the implementation may attempt to use
  KDKIdentification_p as the raw data containing key to use.

  @note
  This function does not return error code.
  AES_IF_Ctx_GetError() can be called user to detect errors.

  @note
  The key loaded with this function shall overwrite existing key
  loaded. Encryption or decryption calls may not be performed after this
  call no matter if the call failed or succeeded.
 */
void AES_IF_Ctx_DeriveKEK(AES_IF_Ctx_Ptr_t Ctx_p,
                          const void * const KDKIdentification_p,
                          const uint32_t KDKIdentificationSize,
                          const uint8_t * const DeriveInfo_p,
                          const uint32_t DeriveInfoSize);


/** Function loads random key to AES_IF implementation for purpose
    of performing encryption and/or decryption operations, and provides
    the generated key as output.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @param Key_p
  Array of octets reserved for resulting key.

  @param KeyLength
  Length of key expected in bits. Implementation only needs to support 128.

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note The key generated with this function shall overwrite existing key
        loaded to the context.
 */
void AES_IF_Ctx_GenerateKey(AES_IF_Ctx_Ptr_t Ctx_p,
                            void * const Key_p,
                            const uint32_t KeyLength);

/** Function loads random key to AES_IF implementation for purpose
    of performing encryption and/or decryption operations, and provides
    the generated key as output wrapped with key set as KEK (Key Encryption
    Key) using AES_IF_Ctx_SetKEK() function.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().

  @param Wrapped_p
  Array of octets reserved for resulting key as wrapping.

  @param WrapLength
  Length of key wrapping expected in bits.
  Implementation only needs to support 192.

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note The key generated with this function shall overwrite existing key
        loaded to the context. The KEK set with AES_IF_Ctx_SetKEK() function
        (which must be called just prior this call) can not be used after
        this call.
 */
void AES_IF_Ctx_GenerateWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                                   void * const Wrapped_p,
                                   const uint32_t WrapLength);

/** Function encrypts single block using AES algorithm.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().
  The context must have a key loaded with AES_IF_Ctx_LoadKey(),
  AES_IF_Ctx_LoadWrappedKey(), AES_IF_Ctx_GenerateKey(), or
  AES_IF_Ctx_GenerateWrappedKey() function.

  @param DataIn_p
  Pointer to array of input octets [plaintext] (exactly 16 bytes).

  @param DataOut_p
  Pointer to array of output octets [ciphertext] (exactly 16 bytes).

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note
  The loaded key remains available in the context for more
  ECB/CBC encryption/decryption operations after this call.
*/
void AES_IF_ECB_EncryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                             const void * const DataIn_p,
                             void * const DataOut_p);


/** Function decrypts single block using AES algorithm.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().
  The context must have a key loaded with AES_IF_Ctx_LoadKey(),
  AES_IF_Ctx_LoadWrappedKey(), AES_IF_Ctx_GenerateKey(), or
  AES_IF_Ctx_GenerateWrappedKey() function.

  @param DataIn_p
  Pointer to array of input octets [ciphertext] (exactly 16 bytes).

  @param DataOut_p
  Pointer to array of output octets [plaintext] (exactly 16 bytes).

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note
  The loaded key remains available in the context for more
  ECB/CBC encryption/decryption operations after this call.
*/
void AES_IF_ECB_DecryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                             const void * const DataIn_p,
                             void * const DataOut_p);

/** Function loads IV for use with AES-CBC algorithm.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().
  The context must have a key loaded with AES_IF_Ctx_LoadKey(),
  AES_IF_Ctx_LoadWrappedKey(), AES_IF_Ctx_GenerateKey(), or
  AES_IF_Ctx_GenerateWrappedKey() function.

  @param IV
  Four 32-bit words containing sixteen bytes of IV.
  This array shall be considered as array containing 16 octets,
  the words are required for purpose of efficient alignment.
  The function may alter contents of the array (see below).

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note The key loaded with this function shall overwrite existing key
        loaded.
 */
void AES_IF_Ctx_LoadIV(AES_IF_Ctx_Ptr_t Ctx_p,
                       const uint32_t IV[4]);

/** Function encrypts blocks using AES algorithm in CBC mode.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().
  The context must have a key loaded with AES_IF_Ctx_LoadKey(),
  AES_IF_Ctx_LoadWrappedKey(), AES_IF_Ctx_GenerateKey(), or
  AES_IF_Ctx_GenerateWrappedKey() function.

  @param DataIn_p
  Pointer to array of input octets [plaintext].

  @param DataOut_p
  Pointer to array of output octets [ciphertext].

  @param Size
  Size of input and output arrays (bytes).
  Both arrays must contain same number of bytes available for reading
  and writing, respectively.
  This value must be multiple of 16.

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note
  AES_IF_Ctx_LoadIV() must be called before sequence of
  AES_IF_CBC_Encrypt() calls.

  @note
  The loaded key remains available in the context for more
  ECB/CBC encryption/decryption operations after this call.
*/
void AES_IF_CBC_Encrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p,
                        const uint32_t Size);

/** Function decrypts blocks using AES algorithm in CBC mode.

  @param Ctx_p
  Context pointer initialized with successful call to AES_IF_Ctx_Init().
  The context must have a key loaded with AES_IF_Ctx_LoadKey(),
  AES_IF_Ctx_LoadWrappedKey(), AES_IF_Ctx_GenerateKey(), or
  AES_IF_Ctx_GenerateWrappedKey() function.

  @param DataIn_p
  Pointer to array of input octets [ciphertext].

  @param DataOut_p
  Pointer to array of output octets [plaintext].

  @param Size
  Size of input and output arrays (bytes).
  Both arrays must contain same number of bytes available for reading
  and writing, respectively.
  This value must be multiple of 16.

  @note This function does not return error code.
        AES_IF_Ctx_GetError() can be called user to detect errors.

  @note
  AES_IF_Ctx_LoadIV() must be called before sequence of
  AES_IF_CBC_Decrypt() calls.

  @note
  The loaded key remains available in the context for more
  ECB/CBC encryption/decryption operations after this call.
*/
void AES_IF_CBC_Decrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p,
                        const uint32_t Size);

#endif /* INCLUSION_GUARD_AES_IF_H */
