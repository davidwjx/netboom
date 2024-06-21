/* rng-health.c
 *
 * Description: Perform a health-check on the RNG.
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

#include "public_defs.h"
#include "c_lib.h"
#include "nist-sp-800-90.h"
#include "nist-sp-chain.h"
#include <stdio.h>
#include <stdlib.h>

static RngChainStruct preallocated_rngchain;
static uint8_t result[32];

int main(int argc, char *argv[])
{
    bool success1 = false;
    bool success2 = false;
    int empty = 0;
    int i;
    RngChain chain;

    /* Perform health testing */
    success1 = ssh_drbg_health_test();

    chain = sfz_random_nist_chain_init(&preallocated_rngchain, true);
    if (!chain)
    {
        printf("Random number generator statefile not available or unusable.\n"
               "Reading entropy from /dev/random, this may take some time.\n");
        chain = sfz_random_nist_chain_init(&preallocated_rngchain, false);
    }

    if (chain)
    {
        if (argc == 1)
        {
            sfz_random_nist_chain_generate(chain, result, sizeof(result), 128);

            for(i = 0; i < (int)sizeof(result); i++)
            {
                if (result[i] == 0)
                {
                    empty++;
                }
            }

            if (empty < 4)
            {
                success2 = true;
            }
        }

        if (argc == 2)
        {
            int n = atoi(argv[1]);
            bool status;

            if (n)
            {
                unsigned char *mem = malloc(n);
                if (mem)
                {
                    c_memset(mem, 0, n);
                    status = sfz_random_nist_chain_generate(chain, mem, n, 128);
                    if (status != true)
                    {
                        printf("RNG generation failed.\n");
                    }
                    else
                    {
                        printf("Random bytes: ");
                        for(i = 0; i < n; i++)
                        {
                            printf("%02x", (int) mem[i]);
                        }
                        free(mem);
                        printf("\n");

                        /* Mark chained RNG as success, bytes have been given
                           for the user to analyze. */
                        success2 = true;
                    }
                }
            }
        }

        if (success2)
        {
            /* Seed chain
             * Note: Intended to cover the function, the functionality that is
             *       called in this function is already check/covered as part
             *       of ssh_drbg_health_test().
             */
            static uint8_t seed[] =
            {
                0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C
            };

            if (sfz_random_nist_chain_seed(chain, seed, (rngsize_t)sizeof(seed)))
            {
                printf("RNG chain seed test passed.\n");
            }
            else
            {
                printf("RNG chain seed test failed.\n");
            }
        }

        sfz_random_nist_chain_uninit(chain);
    }

    /* Analyze results. */

    if (success1 && success2)
    {
        if (argc == 1)
        {
            printf("Random number generator health test passed.\n");
        }
        return 0;
    }

    if (!success1)
    {
        printf("Random number generator health test FAILED!\n");
        return 1;
    }

    printf("Random number generator chain health test FAILED!\n"
           "It is extremely rare for this test to fail.\n"
           "Please rerun test to see if the failure is temporary "
           "or persistent\n.");
    return 1;
}

/* end of file rng-health.c */
