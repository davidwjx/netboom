/* nist-sp-800-90.h
 *
 * Description: Interface to use NIST SP 800-90 DRBG functionality.
 */

/*****************************************************************************
* Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "implementation_defs.h"

#ifndef INCLUDE_GUARD_NIST_SP_800_90_H
#define INCLUDE_GUARD_NIST_SP_800_90_H

/* Random number generation: result codes. */
typedef enum SshRngStatusEnum
{
    SSH_RNG_OK = 0,
    SSH_RNG_UNSUPPORTED = 2,
    SSH_RNG_INVALID_OPERATION = 3,
    SSH_RNG_HANDLE_INVALID = 4,
    SSH_RNG_RNG_ENTROPY_NEEDED = 5,
    SSH_RNG_NO_MEMORY = 6
} SshRngStatus;

/** Size_t variant used for RNG. */
typedef unsigned long rngsize_t;

/** Handle for the current DRBG state. */
typedef struct SshDrbgStateRec *SshDrbgState;

/** Generic memory allocation interface */
typedef struct SshDrbgAllocStruct
{
    void *Context_p;
    void *(*MallocFunc_p)(size_t Size, void *Context_p);
    void (*FreeFunc_p)(void *Data_p, void *Context_p);
} SshDrbgAllocRec;

typedef SshDrbgAllocRec *SshDrbgAlloc;
typedef const SshDrbgAllocRec *SshConstDrbgAlloc;

/** Estimated size of struct SshDrbgStateRec.
    This structure is always at least as large as
    struct SshDrbgStateRec. The structure can only be used for
    purpose of preallocating memory for SshDrbgStateRec, the
    members of the structure cannot be used. */
typedef struct SshDrbgStateEstimateRec
{
    void (*funcptr[5]);
    void *generic_ptr[5];
    uint32_t integers[5];
    unsigned char state[16 * 3];
    SshDrbgAllocRec allocrec;
} SshDrbgStateEstimateStruct;
typedef SshDrbgStateEstimateStruct *SshDrbgStateEstimate;

/** Function to be called when entropy or nonce is needed by the DRBG.

    @param function_arg
    Generic argument for passing context.

    @param buffer
    Buffer for the data.

    @param buffer_size
    Size of the buffer.

    @param input_size
    Amount of data inserted to the buffer. If input_size is 0, the
    call failed.

 */
typedef void (*SshDrbgDataInput)(void *function_arg,
                                 unsigned char *buffer,
                                 rngsize_t buffer_size,
                                 rngsize_t *input_size);

/** Function to instantiate a DRBG state.

    @param requested_security_strength
    Security strength needed to be used with this instantiation.

    @param prediction_resistance
    Boolean to indicate if prediction resistance should be used.

    @param personalization_string
    Optional personalization information.

    @param personalization_string_size
    Size of personalization string, must be less than
    the DRBG_MAX_PERSONALIZATION_STRING_LENGTH value.

    @param entropy_func
    Function to be used for getting entropy for the instantiation,
    if NULL the default function is used.

    @param nonce_func
    Function to be used for getting nonce for the instantiation,
    if NULL the default function is used.

    @param function_arg
    Function argument for entropy_func and nonce_func.

    @param state_handle
    Pointer for the returned state handle on success.

    @param alloc_interface
    Memory allocation interface to use for DRBG.
    The interface (including) Context_p will be memory copied.

    @return
    SSH_RNG_OK on successful operation.

*/
SshRngStatus
ssh_drbg_instantiate(uint32_t requested_security_strength,
                     bool prediction_resistance,
                     unsigned char *personalization_string,
                     rngsize_t personalization_string_size,
                     SshDrbgDataInput entropy_func,
                     SshDrbgDataInput nonce_func,
                     void *function_arg,
                     SshDrbgState *state_handle,
                     SshDrbgAlloc alloc_interface);


/** Function to reseed the DRBG state.

    @param additional_input
    Optional input for the seed.

    @param additional_input_size
    Size of additional_input.

    @param state_handle
    Handle for the DRBG state used.

    @return
    SSH_RNG_OK on successful operation.

*/
SshRngStatus
ssh_drbg_reseed(unsigned char *additional_input,
                rngsize_t additional_input_size,
                SshDrbgState state_handle);

/** Function to generate pseudorandom data.

    @param requested_number_of_bits
    Requested size of the pseudorandom data.

    @param requested_security_strength
    Security strength needed to be used with this operation.

    @param prediction_resistance_request
    Boolean to indicate if prediction resistance should be used.

    @param additional_input
    Optional input for the seed.

    @param additional_input_size
    Size of additional_input.

    @param pseudorandom_bits
    Array to store the pseudorandom data, must be at least the size of
    the requested_number_of_bits value.

    @param state_handle
    Handle for the DRBG state used.

    @return
    SSH_RNG_OK on successful operation.
*/

SshRngStatus
ssh_drbg_generate(uint32_t requested_number_of_bits,
                  uint32_t requested_security_strength,
                  bool prediction_resistance_request,
                  unsigned char *additional_input,
                  rngsize_t additional_input_size,
                  unsigned char *pseudorandom_bits,
                  SshDrbgState state_handle);

/** Function to set DRBG state. This is needed e.g. during
    DRBG health check.

    @param reseed_counter
    Counter for the reseed-operations

    @param v
    New value for v, if NULL the value is unchanged

    @param v_len
    Size of v

    @param key
    New value for key, if NULL the value is unchanged

    @param key_len
    Size of key

    @param state_handle
    Handle for the DRBG state

    @return
    SSH_RNG_OK on successfull operation

*/

SshRngStatus
ssh_drbg_set_state(uint32_t reseed_counter,
                   unsigned char *v,
                   rngsize_t v_len,
                   unsigned char *key,
                   rngsize_t key_len,
                   SshDrbgState state_handle);


/** Function to uninstantiate the DRBG.

    @param state_handle
    Handle for the DRBG state.

    @return
    SSH_RNG_OK on successful operation.

*/
SshRngStatus
ssh_drbg_uninstantiate(SshDrbgState state_handle);

/** Run the health check self-test function.
 */
bool ssh_drbg_health_test(void);

#endif /* Include Guard */

/* end of file nist-sp-800-90.h */
