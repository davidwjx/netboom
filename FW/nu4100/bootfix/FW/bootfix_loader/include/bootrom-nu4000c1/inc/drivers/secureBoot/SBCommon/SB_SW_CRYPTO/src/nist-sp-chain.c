/* nist-sp-chain.c
 *
 * Description: File backed chaining of NIST-SP-800-90 random number generators.
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
#include "cfg_random.h"
#include "c_lib.h"
#include "nist-sp-chain.h"


#include <stdio.h>
#include <stdlib.h>

#ifdef CFG_RANDOM_STATE_FILENAME
#include <limits.h>

#ifdef CFG_RANDOM_STATE_FILE_UNIX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>
#endif /* CFG_RANDOM_STATE_FILE_UNIX */

#ifndef PATH_MAX
#define PATH_MAX 512
#endif

static bool
statefile_retrieve(void *state_data_p,
                   rngsize_t state_length)
{
    char filename[PATH_MAX];
    int length;
    const char *home;
    FILE *file;
    bool success_flag = false;

    home = getenv("HOME");
    length = snprintf(filename, PATH_MAX, "%s%s%s",
                      home ? home : "",
                      home ? "/" : "",
                      CFG_RANDOM_STATE_FILENAME);
    if (length >= PATH_MAX)
    {
        return false;
    }

    file = fopen(filename, "rb");
    if (file)
    {
#ifdef CFG_RANDOM_STATE_FILE_UNIX
        if (flock(fileno(file), LOCK_EX) == 0)
#endif /* CFG_RANDOM_STATE_FILE_UNIX */
        {
            if (fread(state_data_p, state_length, 1, file) == 1)
            {
                success_flag = true;
            }

            /* Remove the file to prevent accidentally using same state
               multiple times. */
            remove(filename);
        }
        /* Close the file [note: the file is usually remove by now.] */
        fclose(file);
    }

    return success_flag;
}

static bool
statefile_store(void *state_data_p, rngsize_t state_length)
{
    char filename[PATH_MAX];
    int length;
    const char *home;
    FILE *file;
#ifdef CFG_RANDOM_STATE_FILE_UNIX
    int fd;
#endif /* CFG_RANDOM_STATE_FILE_UNIX */
    bool success_flag = false;

    home = getenv("HOME");
    length = snprintf(filename, PATH_MAX, "%s%s%s",
                      home ? home : "",
                      home ? "/" : "",
                      CFG_RANDOM_STATE_FILENAME);

    if (length >= PATH_MAX)
    {
        return false;
    }

#ifdef CFG_RANDOM_STATE_FILE_UNIX
    /* Ensure permissions are current owner only, request synchronous I/O. */

    fd = open(filename,
              O_CREAT | O_NOCTTY | O_SYNC | O_TRUNC | O_WRONLY,
              S_IRUSR | S_IWUSR);
    if (fd >= 0)
    {
        file = fdopen(fd, "wb");

        /* Close the file if fdopen failed. If fdopen succeeded,
           fclose will close the file. */
        if (file == NULL)
        {
            close(fd);
        }
    }
    else
    {
        file = NULL;
    }
#else
    file = fopen(filename, "wb");
#endif /* CFG_RANDOM_STATE_FILE_UNIX */
    if (file)
    {
#ifdef CFG_RANDOM_STATE_FILE_UNIX
        if (flock(fileno(file), LOCK_EX) == 0)
#endif /* CFG_RANDOM_STATE_FILE_UNIX */
        {
            if (fwrite(state_data_p, state_length, 1, file) == 1)
            {
                success_flag = true;
            }
        }
        fclose(file);
    }

    return success_flag;
}
#else
/* State file not used. */
static bool
statefile_retrieve(void *state_data_p,
                   rngsize_t state_length)
{
    return false; /* Always fail. */
}

static bool
statefile_store(void *state_data_p,
                rngsize_t state_length)
{
    return true; /* Always succeed. */
}
#endif /* CFG_RANDOM_STATE_FILENAME */

#define DRBG_CHILD_SECURITY_STATE_LENGTH 32
#define DRBG_CHILD_SECURITY_STRENGTH 128

void
sfz_random_nist_chain_uninit(RngChain RngChain_p)
{
    if (RngChain_p->state_main)
    {
        ssh_drbg_uninstantiate(RngChain_p->state_main);
        RngChain_p->state_main = NULL;
    }

    if (RngChain_p->state_child)
    {
        ssh_drbg_uninstantiate(RngChain_p->state_child);
        RngChain_p->state_child = NULL;
    }

    /* Clear memory used for random number generation. */
    c_memset(RngChain_p, 0, sizeof(*RngChain_p));
}

static void
default_entropy_input(void *function_arg,
                      unsigned char *buffer,
                      rngsize_t buffer_size,
                      rngsize_t *input_size)
{
#ifdef CFG_RANDOM_ENTROPY_DEVICE
    FILE *inputRng_p;
    inputRng_p = fopen(CFG_RANDOM_ENTROPY_DEVICE, "rb");
    if (!inputRng_p)
    {
        fprintf(stderr, "Unable to open entropy device %s.\n",
                CFG_RANDOM_ENTROPY_DEVICE);
        exit(EXIT_FAILURE);
    }
    setvbuf(inputRng_p, NULL, _IONBF, 0);
    if (fread(buffer, buffer_size, 1, inputRng_p) < 1)
    {
        /* Unable to read random numbers. */
        fprintf(stderr, "Unable to read entropy from %s.\n",
                CFG_RANDOM_ENTROPY_DEVICE);
        exit(EXIT_FAILURE);
    }
    fclose(inputRng_p);
    *input_size = buffer_size;
#else
#error "CFG_RANDOM_ENTROPY_DEVICE not defined."
#endif

    IDENTIFIER_NOT_USED(function_arg);
}

static void
default_nonce_input(void *function_arg,
                    unsigned char *buffer,
                    rngsize_t buffer_size,
                    rngsize_t *input_size)
{
    /* Use CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE for nonces. */
#ifdef CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE
    FILE *inputRng_p;
    inputRng_p = fopen(CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE, "rb");
    if (!inputRng_p)
    {
        fprintf(stderr, "Unable to open entropy device %s.\n",
                CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE);
        exit(EXIT_FAILURE);
    }
    setvbuf(inputRng_p, NULL, _IONBF, 0);
    if (fread(buffer, buffer_size, 1, inputRng_p) < 1)
    {
        /* Unable to read random numbers. */
        fprintf(stderr, "Unable to read entropy from %s.\n",
                CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE);
        exit(EXIT_FAILURE);
    }
    fclose(inputRng_p);
    *input_size = buffer_size;
#else
    /* As long as AES-128 CTR without derivation function is the only
       supported mechanism, nonce does not have to contain entropy  */
    c_memset(buffer, 0x00, buffer_size);
    *input_size = buffer_size;
#endif

    IDENTIFIER_NOT_USED(function_arg);
}

static void
statefile_entropy_input(void *random_state,
                        unsigned char *buf_p,
                        rngsize_t buf_size,
                        rngsize_t *input_size)
{
    RngChain RngChain_p = random_state;

    *input_size = 0;
    if (RngChain_p->read_state)
    {
        RngChain_p->read_state = false;
        if (statefile_retrieve(buf_p, buf_size))
        {
            *input_size = buf_size;
            return;
        }
    }
    if (RngChain_p->entropy_slow)
    {
        if (*input_size < buf_size)
        {
            default_entropy_input(RngChain_p, buf_p, buf_size, input_size);
        }
    }
    else
    {
        *input_size = 0;
    }
}

static void
chain_entropy_input(void *random_state,
                    unsigned char *buf_p,
                    rngsize_t buf_size,
                    rngsize_t *input_size)
{
    RngChain RngChain_p = random_state;
    SshRngStatus status;
    SshRngStatus status2;
    unsigned char state_store[DRBG_CHILD_SECURITY_STATE_LENGTH];
    bool res;

    /* Generate requested result. */
    status = ssh_drbg_generate(buf_size * 8,
                               DRBG_CHILD_SECURITY_STRENGTH,
                               false,
                               NULL,
                               0,
                               buf_p,
                               RngChain_p->state_main);

    /* Store state in randomness state file. */
    status2 = ssh_drbg_generate(DRBG_CHILD_SECURITY_STATE_LENGTH * 8,
                                DRBG_CHILD_SECURITY_STRENGTH,
                                false,
                                NULL,
                                0,
                                state_store,
                                RngChain_p->state_main);

    res = statefile_store(state_store, DRBG_CHILD_SECURITY_STATE_LENGTH);
    c_memset(state_store, 0, sizeof(state_store));

    if (status == SSH_RNG_OK && status2 == SSH_RNG_OK && res == true)
    {
        *input_size = buf_size;
    }
    else
    {
        *input_size = 0;
    }
}

static void *
sfz_random_nist_use_ctx_malloc(size_t memory_size,
                               void * Context_p)
{
    ASSERT(memory_size <= sizeof(SshDrbgStateEstimateStruct));
    return Context_p;
}

static void
sfz_random_nist_use_ctx_free(void * Data_p,
                             void * Context_p)
{
    if (Data_p)
    {
        ASSERT(Data_p == Context_p);
    }
}

RngChain
sfz_random_nist_chain_init(RngChain RngChain_p,
                           bool nonblock)
{
    SshRngStatus status;
    SshDrbgAllocRec allocrec;

    c_memset(RngChain_p, 0, sizeof(*RngChain_p));

    /* First test health. */
    if (!ssh_drbg_health_test())
    {
        goto fail;
    }

    allocrec.Context_p = &(RngChain_p->state_main_storage.structure);
    allocrec.MallocFunc_p = sfz_random_nist_use_ctx_malloc;
    allocrec.FreeFunc_p = sfz_random_nist_use_ctx_free;

    /* Then initialize random number generator instance. */
    RngChain_p->read_state = true;
    RngChain_p->entropy_slow = !nonblock;
    status = ssh_drbg_instantiate(CFG_RANDOM_QUALITY,
                                  false,
                                  NULL,
                                  0,
                                  statefile_entropy_input,
                                  default_nonce_input,
                                  RngChain_p,
                                  &RngChain_p->state_main,
                                  &allocrec);
    if (status != SSH_RNG_OK)
    {
        goto fail;
    }

    allocrec.Context_p = &(RngChain_p->state_child_storage.structure);

    status = ssh_drbg_instantiate(CFG_RANDOM_QUALITY,
                                  false,
                                  NULL,
                                  0,
                                  chain_entropy_input,
                                  default_nonce_input,
                                  RngChain_p,
                                  &RngChain_p->state_child,
                                  &allocrec);
    if (status != SSH_RNG_OK)
    {
        goto fail;
    }

    return RngChain_p;

fail:
    sfz_random_nist_chain_uninit(RngChain_p);
    return NULL;
}

bool
sfz_random_nist_chain_seed(RngChain RngChain_p,
                           void *seed_p,
                           rngsize_t seed_size)
{
    SshRngStatus status = ssh_drbg_reseed(seed_p,
                                          seed_size,
                                          RngChain_p->state_child);

    return status == SSH_RNG_OK;
}

bool
sfz_random_nist_chain_generate(RngChain RngChain_p,
                               void *buf_p,
                               rngsize_t buf_size,
                               unsigned int quality_bits)
{
    SshRngStatus status = ssh_drbg_generate(buf_size * 8,
                                            quality_bits,
                                            false,
                                            NULL,
                                            0,
                                            buf_p,
                                            RngChain_p->state_child);

    return status == SSH_RNG_OK;
}

/* end of file nist-sp-chain.c */
