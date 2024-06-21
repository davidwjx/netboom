/* nist-sp-800-90.c
 *
 * Description: Deterministic Random Bit Generator (DRBG) using AES-CTR-128.
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
#include "nist-sp-800-90.h"
#include "rijndael.h"
#include "c_lib.h"

#define SSHDIST_CRYPT_NIST_SP_800_90
#ifdef SSHDIST_CRYPT_NIST_SP_800_90

#define DRBG_AES128_SEEDLEN 32
#define DRBG_AES128_KEYLEN 16
#define DRBG_AES128_SECURITY_STRENGTH 128
#define DRBG_AES128_NONCELEN 8

#define DRBG_AES_BLOCKLEN 16

#define DRBG_RESEED_INTERVAL 1024

#define DRBG_MAX_PERSONALIZATION_STRING_LENGTH DRBG_AES128_SEEDLEN
#define DRBG_MAX_ADDITIONAL_INPUT_LENGTH DRBG_AES128_SEEDLEN
#define DRBG_MAX_REQUEST_SIZE 65536

/* NIST SP 800-90 10.2.1.1 */
typedef struct SshDrbgStateRec
{
    SshDrbgDataInput nonce_function;
    SshDrbgDataInput entropy_function;
    void *function_arg;
    bool prediction_resistance;
    uint32_t security_strength;
    uint32_t reseed_counter;
    unsigned char v[DRBG_AES_BLOCKLEN];
    unsigned char key[DRBG_AES128_KEYLEN];
    SshDrbgAllocRec alloc_interface;
} SshDrbgStateStruct;

/* Ensure SshDrbgStateEstimateStruct is at least as large as the
   actual structure. */
COMPILE_GLOBAL_ASSERT(sizeof(SshDrbgStateEstimateStruct) >=
                      sizeof(SshDrbgStateStruct));

/* *************************** Util **************************************/

static void
increment_block(
    unsigned char *block,
    uint32_t block_len)
{
    uint32_t i = block_len - 1;

    do
    {
        /* Increment */
        block[i]++;

        /* Check overflow */
        if (block[i] != 0x00)
        {
            break;
        }
        i--;
    } while (i != 0);
}

/* ********* Minimal cipher interface ************************************/

typedef struct CipherStruct
{
    uint32_t RK[RKLENGTH(256)];
    int nRounds;
} CipherRec;

static SshRngStatus
cipher_init(CipherRec *AESState_p,
            const unsigned char *Key_p,
            int KeyLength)
{
    ASSERT(KeyLength == 16 || KeyLength == 24 || KeyLength == 32);
    AESState_p->nRounds = rijndaelSetupEncrypt(AESState_p->RK,
                                               Key_p, KeyLength * 8);
    return SSH_RNG_OK;
}

static SshRngStatus
cipher_transform_block_128(CipherRec *AESState_p,
                           unsigned char *OutputBytes_p,
                           const unsigned char *InputBytes_p)
{
    rijndaelEncrypt(AESState_p->RK,
                    AESState_p->nRounds,
                    InputBytes_p, OutputBytes_p);
    return SSH_RNG_OK;
}

static void
cipher_clean(CipherRec *AESState_p)
{
    c_memset(AESState_p, 0, sizeof(*AESState_p));
}

/* ************************* CTR_DRBG ************************************/

/* NIST SP 800-90 10.2.1.2 */
static SshRngStatus
ctr_drbg_update(
    unsigned char *provided_data,
    const unsigned char *key,
    const unsigned char *v,
    SshDrbgState state)
{
    SshRngStatus status;
    CipherRec cipher;
    unsigned char temp[DRBG_AES128_SEEDLEN * 2];
    uint32_t i, temp_len;

    c_memset(temp, 0x00, DRBG_AES128_SEEDLEN * 2);

    status = cipher_init(&cipher, key, DRBG_AES128_KEYLEN);
    if (status != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to allocate cipher");
        goto fail;
    }

    for (temp_len = 0; temp_len < DRBG_AES128_SEEDLEN; temp_len += DRBG_AES_BLOCKLEN)
    {
        increment_block((unsigned char *) v, DRBG_AES_BLOCKLEN);

        status = cipher_transform_block_128(&cipher, &temp[temp_len], v);
        if (status != SSH_RNG_OK)
        {
            L_DEBUG(LF_DRBG_ERROR, "Failed transform");
            goto fail;
        }
    }

    for (i = 0; i < DRBG_AES128_SEEDLEN; i++)
    {
        temp[i] ^= provided_data[i];
    }

    c_memcpy(state->key, temp, DRBG_AES128_KEYLEN);
    c_memcpy(state->v, temp + DRBG_AES128_KEYLEN, DRBG_AES_BLOCKLEN);

fail:
    cipher_clean(&cipher);
    return status;
}

/* NIST SP 800-90 10.2.1.3.1 (no df) */
static SshRngStatus
ctr_drbg_instantiate_algorithm(unsigned char *entropy_input,
                               rngsize_t entropy_input_size,
                               unsigned char *personalization_string,
                               rngsize_t personalization_string_size,
                               SshDrbgState *initial_state,
                               SshDrbgAlloc alloc_interface)
{
    SshRngStatus status;
    int i;
    unsigned char seed_material[DRBG_AES128_SEEDLEN];
    unsigned char key[DRBG_AES128_KEYLEN];
    unsigned char v[DRBG_AES_BLOCKLEN];

    ASSERT(entropy_input_size == DRBG_AES128_SEEDLEN);

    /* Initially clear all. */
    c_memset(seed_material, 0x00, DRBG_AES128_SEEDLEN);
    c_memset(key, 0x00, DRBG_AES128_KEYLEN);
    c_memset(v, 0x00, DRBG_AES_BLOCKLEN);

    /* Allocate state. */
    if (initial_state == NULL)
    {
        return SSH_RNG_NO_MEMORY;
    }
    *initial_state = alloc_interface->MallocFunc_p(sizeof(**initial_state),
                                                   alloc_interface->Context_p);
    if (*initial_state == NULL)
    {
        return SSH_RNG_NO_MEMORY;
    }

    c_memset(*initial_state, 0x00, sizeof(**initial_state));

    /* Make copy of allocator interface. */
    c_memcpy(&(*initial_state)->alloc_interface,
             alloc_interface,
             sizeof(*alloc_interface));

    c_memcpy(seed_material,
             personalization_string,
             personalization_string_size);

    for (i = 0; i < DRBG_AES128_SEEDLEN; i++)
    {
        seed_material[i] ^= entropy_input[i];
    }

    status = ctr_drbg_update(seed_material, key, v, *initial_state);

    (*initial_state)->reseed_counter = 1;

    return status;
}

/* NIST SP 800-90 10.2.1.4.1 (no df) */
static SshRngStatus
ctr_drbg_reseed_algorithm(unsigned char *entropy_input,
                          rngsize_t entropy_input_size,
                          unsigned char *additional_input,
                          rngsize_t additional_input_size,
                          SshDrbgState state)
{
    SshRngStatus status;
    int i;
    unsigned char seed_material[DRBG_AES128_SEEDLEN];

    ASSERT(entropy_input_size == DRBG_AES128_SEEDLEN);

    c_memset(seed_material, 0x00, DRBG_AES128_SEEDLEN);
    c_memcpy(seed_material, additional_input, additional_input_size);

    for (i = 0; i < DRBG_AES128_SEEDLEN; i++)
    {
        seed_material[i] ^= entropy_input[i];
    }

    status = ctr_drbg_update(seed_material, state->key, state->v, state);

    state->reseed_counter = 1;

    return status;
}

/* NIST SP 800-90 10.2.1.5.1 (no df) */
static SshRngStatus
ctr_drbg_generate(unsigned char *return_buffer,
                  rngsize_t requested_size,
                  rngsize_t *returned_size,
                  unsigned char *additional_input,
                  rngsize_t additional_input_size,
                  SshDrbgState state)
{
    CipherRec cipher;
    SshRngStatus status;
    unsigned char seed_material[DRBG_AES128_SEEDLEN];
    unsigned char temporary_block[DRBG_AES_BLOCKLEN];
    uint32_t buffer_index;

    c_memset(seed_material, 0x00, DRBG_AES128_SEEDLEN);

    if (state->reseed_counter > DRBG_RESEED_INTERVAL)
    {
        return SSH_RNG_RNG_ENTROPY_NEEDED;
    }

    if (additional_input != NULL && additional_input_size > 0)
    {
        c_memcpy(seed_material, additional_input, additional_input_size);

        status = ctr_drbg_update(seed_material, state->key, state->v, state);
        if (status != SSH_RNG_OK)
        {
            L_DEBUG(LF_DRBG_ERROR, "Failed to insert additional input");
            return status;
        }
    }

    status = cipher_init(&cipher, state->key, DRBG_AES128_KEYLEN);
    if (status != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to allocate cipher");
        goto fail;
    }

    for (buffer_index = 0;
         buffer_index < requested_size;
         buffer_index += DRBG_AES_BLOCKLEN)
    {
        increment_block(state->v, DRBG_AES_BLOCKLEN);

        if (requested_size - buffer_index < 16)
        {
            /* Last partial block. */
            status = cipher_transform_block_128(&cipher,
                                                temporary_block,
                                                state->v);

            c_memcpy(&return_buffer[buffer_index],
                     temporary_block,
                     requested_size - buffer_index);
            c_memset(temporary_block, 0, sizeof(temporary_block));

            ASSERT(buffer_index + (requested_size - buffer_index) ==
                   requested_size);
        }
        else
        {
            /* Full block. */
            status = cipher_transform_block_128(&cipher,
                                                &return_buffer[buffer_index],
                                                state->v);

            ASSERT(buffer_index + DRBG_AES_BLOCKLEN <= requested_size);
        }

        if (status != SSH_RNG_OK)
        {
            L_DEBUG(LF_DRBG_ERROR, "Failed transform");
            goto fail;
        }
    }

    status = ctr_drbg_update(seed_material, state->key, state->v, state);
    if (status != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_ERROR, "Failed to update");
        goto fail;
    }

    state->reseed_counter++;

    *returned_size = requested_size;

fail:
    cipher_clean(&cipher);
    return status;
}

/* ******************** Default entropy function *************************/
#ifdef USE_DEFAULT_ENTROPY_FUNC
static void
drbg_default_entropy_input(void *function_arg,
                           unsigned char *buffer,
                           rngsize_t buffer_size,
                           rngsize_t *input_size)
{
    /* No default entropy input. */
    *input_size = 0;

    IDENTIFIER_NOT_USED(function_arg);
    IDENTIFIER_NOT_USED(buffer);
    IDENTIFIER_NOT_USED(buffer_size);
}
#endif

static void
drbg_default_nonce_input(void *function_arg,
                         unsigned char *buffer,
                         rngsize_t buffer_size,
                         rngsize_t *input_size)
{
    /* As long as AES-128 CTR without derivation function is the only
       supported mechanism, nonce does not have to contain entropy  */
    c_memset(buffer, 0x00, buffer_size);
    *input_size = buffer_size;

    IDENTIFIER_NOT_USED(function_arg);
    return;
}


/* ************************** DRBG API ***********************************/

/* NIST SP 800-90 9.1 */
SshRngStatus
ssh_drbg_instantiate(uint32_t requested_security_strength,
                     bool prediction_resistance,
                     unsigned char *personalization_string,
                     rngsize_t personalization_string_size,
                     SshDrbgDataInput entropy_func,
                     SshDrbgDataInput nonce_func,
                     void *function_arg,
                     SshDrbgState *state_handle,
                     SshDrbgAlloc alloc_interface)
{
    SshRngStatus status;
    uint32_t security_strength = 0;
    SshDrbgState initial_working_state = NULL;
    unsigned char entropy_input[DRBG_AES128_SEEDLEN];
    unsigned char nonce_input[DRBG_AES128_NONCELEN];
    rngsize_t nonce_return_size = 0;
    rngsize_t entropy_return_size = 0;

    if (requested_security_strength > DRBG_AES128_SECURITY_STRENGTH)
    {
        L_DEBUG(LF_DRBG_FAIL,
                "Invalid requested security strength '%u'",
                requested_security_strength);

        return SSH_RNG_UNSUPPORTED;
    }

    if (prediction_resistance)
    {
        L_DEBUG(LF_DRBG_FAIL, "Prediction resistance not supported");
        return SSH_RNG_UNSUPPORTED;
    }

    if (personalization_string_size > DRBG_MAX_PERSONALIZATION_STRING_LENGTH)
    {
        L_DEBUG(LF_DRBG_FAIL, "Too long personalization string");
        return SSH_RNG_UNSUPPORTED;
    }

    if (requested_security_strength > DRBG_AES128_SECURITY_STRENGTH)
    {
        L_DEBUG(LF_DRBG_FAIL,
                "Requested too high security strength: '%d', maximum "
                "supported is %d",
                requested_security_strength,
                DRBG_AES128_SECURITY_STRENGTH);
        return SSH_RNG_UNSUPPORTED;
    }
    else
    {
        /* Only AES128 is currently supported */
        security_strength = DRBG_AES128_SECURITY_STRENGTH;
    }

    if (entropy_func != NULL)
    {
        (*entropy_func)(function_arg, entropy_input,
                        DRBG_AES128_SEEDLEN,
                        &entropy_return_size);
    }
#ifdef USE_DEFAULT_ENTROPY_FUNC
    else
    {
        drbg_default_entropy_input(NULL, entropy_input,
                                   DRBG_AES128_SEEDLEN,
                                   &entropy_return_size);
    }
#endif
    if (entropy_return_size == 0)
    {
        /* The following is trace as it is standard part of health test. */
        L_TRACE(LF_DRBG_FAIL, "Failed to get entropy");
        return SSH_RNG_INVALID_OPERATION;
    }

    if (nonce_func != NULL)
    {
        (*nonce_func)(function_arg,
                      nonce_input,
                      DRBG_AES128_NONCELEN,
                      &nonce_return_size);
    }
    else
    {
        drbg_default_nonce_input(function_arg,
                                 nonce_input,
                                 DRBG_AES128_NONCELEN,
                                 &nonce_return_size);
    }
    if (nonce_return_size == 0)
    {
        L_DEBUG(LF_DRBG_ERROR, "Failed to get nonce");
        return SSH_RNG_INVALID_OPERATION;
    }

    status = ctr_drbg_instantiate_algorithm(entropy_input,
                                            DRBG_AES128_SEEDLEN,
                                            personalization_string,
                                            personalization_string_size,
                                            &initial_working_state,
                                            alloc_interface);
    if (status != SSH_RNG_OK)
    {
        L_DEBUG(LF_DRBG_ERROR, "Failed to initiate algorithm");
        return status;
    }

    /* Store values, if nonce and entropy functions are not set use
       default entropy function instead. */
    initial_working_state->prediction_resistance = prediction_resistance;
    initial_working_state->security_strength = security_strength;
    initial_working_state->nonce_function =
        nonce_func ? nonce_func : drbg_default_nonce_input;
#ifdef USE_DEFAULT_ENTROPY_FUNC
    initial_working_state->entropy_function =
        entropy_func ? entropy_func : drbg_default_entropy_input;
#else
    initial_working_state->entropy_function = entropy_func;
#endif
    initial_working_state->function_arg = function_arg;

    *state_handle = initial_working_state;

    return status;
}

/* NIST SP 800-90 9.2 */
SshRngStatus
ssh_drbg_reseed(
    unsigned char *additional_input,
    rngsize_t additional_input_size,
    SshDrbgState state_handle)
{
    SshRngStatus status;
    unsigned char entropy_input[DRBG_AES128_SEEDLEN];
    rngsize_t entropy_input_size = 0;

    if (state_handle == NULL)
    {
        L_DEBUG(LF_DRBG_ERROR, "Invalid handle");
        return SSH_RNG_HANDLE_INVALID;
    }

    if (additional_input_size > DRBG_MAX_ADDITIONAL_INPUT_LENGTH)
    {
        L_DEBUG(LF_DRBG_FAIL, "Too long additional input");
        return SSH_RNG_UNSUPPORTED;
    }

    ASSERT(state_handle->entropy_function != NULL);

    (*state_handle->entropy_function)(state_handle->function_arg,
                                      entropy_input,
                                      DRBG_AES128_SEEDLEN,
                                      &entropy_input_size);
    if (entropy_input_size == 0)
    {
        L_DEBUG(LF_DRBG_FAIL, "Failed to get entropy");
        return SSH_RNG_INVALID_OPERATION;
    }

    status = ctr_drbg_reseed_algorithm(entropy_input,
                                       DRBG_AES128_SEEDLEN,
                                       additional_input,
                                       additional_input_size,
                                       state_handle);
    return status;
}

/* NIST SP 800-90 9.3 */
SshRngStatus
ssh_drbg_generate(
    uint32_t requested_number_of_bits,
    uint32_t requested_security_strength,
    bool prediction_resistance_request,
    unsigned char *additional_input,
    rngsize_t additional_input_size,
    unsigned char *pseudorandom_bits,
    SshDrbgState state_handle)
{
    SshRngStatus status;
    rngsize_t returned_size;
    bool reseed_required = false;

    if (state_handle == NULL)
    {
        L_DEBUG(LF_DRBG_ERROR, "Invalid handle");
        return SSH_RNG_HANDLE_INVALID;
    }

    if (requested_number_of_bits > DRBG_MAX_REQUEST_SIZE * 8)
    {
        L_DEBUG(LF_DRBG_ERROR,
                "Too large random buffer required (%u)",
                requested_number_of_bits / 8);
        return SSH_RNG_UNSUPPORTED;
    }

    if (requested_security_strength > state_handle->security_strength)
    {
        /* The following is trace as it is standard part of health test. */
        L_TRACE(LF_DRBG_FAIL,
                "Requested security strength of '%d', but %d is the "
                "maximum supported by this instantiation.",
                requested_security_strength,
                state_handle->security_strength);
        return SSH_RNG_UNSUPPORTED;
    }

    if (additional_input_size > DRBG_MAX_ADDITIONAL_INPUT_LENGTH)
    {
        /* The following is trace as it is standard part of health test. */
        L_TRACE(LF_DRBG_FAIL, "Too long additional input");
        return SSH_RNG_UNSUPPORTED;
    }

    if (prediction_resistance_request)
    {
        L_DEBUG(LF_DRBG_FAIL, "Prediction resistance not supported");
        return SSH_RNG_UNSUPPORTED;
    }

reseed:
    if (reseed_required)
    {
        status = ssh_drbg_reseed(additional_input,
                                 additional_input_size,
                                 state_handle);
        if (status != SSH_RNG_OK)
        {
            L_DEBUG(LF_DRBG_ERROR, "Reseeding for generate-function failed");
            return status;
        }

        additional_input = NULL;
        additional_input_size = 0;
    }

    status = ctr_drbg_generate(pseudorandom_bits,
                               ((requested_number_of_bits + 7) / 8),
                               &returned_size,
                               additional_input,
                               additional_input_size,
                               state_handle);
    if (status == SSH_RNG_RNG_ENTROPY_NEEDED)
    {
        L_TRACE(LF_DRBG_RESEED,
                "Reseed required before drbg can operate");
        reseed_required = true;
        goto reseed;
    }

    return status;
}

/* NIST SP 800-90 9.3 */
SshRngStatus
ssh_drbg_uninstantiate(
    SshDrbgState state_handle)
{
    void *Context_p;
    void (*FreeFunc_p)(void *Data_p, void *Context_p);

    if (state_handle == NULL)
    {
        L_DEBUG(LF_DRBG_ERROR,  "Invalid handle");
        return SSH_RNG_HANDLE_INVALID;
    }

    Context_p = state_handle->alloc_interface.Context_p;
    FreeFunc_p = state_handle->alloc_interface.FreeFunc_p;

    c_memset(state_handle, 0x00, sizeof(*state_handle));
    (*FreeFunc_p)(state_handle, Context_p);

    return SSH_RNG_OK;
}

SshRngStatus
ssh_drbg_set_state(
    uint32_t reseed_counter,
    unsigned char *v,
    rngsize_t v_len,
    unsigned char *key,
    rngsize_t key_len,
    SshDrbgState state_handle)
{
    if (state_handle == NULL)
    {
        L_DEBUG(LF_DRBG_ERROR, "Invalid handle");
        return SSH_RNG_HANDLE_INVALID;
    }

    if (v != NULL)
    {
        ASSERT(v_len == DRBG_AES_BLOCKLEN);
        c_memcpy(state_handle->v, v, v_len);
    }

    if (key != NULL)
    {
        ASSERT(key_len == DRBG_AES128_KEYLEN);
        c_memcpy(state_handle->key, key, key_len);
    }

    state_handle->reseed_counter = reseed_counter;

    return SSH_RNG_OK;
}

#endif /* SSHDIST_CRYPT_NIST_SP_800_90 */

/* end of file nist-sp-800-90.c */
