/**
*  File: rijndael.h
*
*  Description: Declaration of interface to rijndael algorithm implementation.
*
* NOTE: This file has been customized by INSIDE Secure to fit into its products.
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

/**
 * This file is has been placed to public domain by
 * Philip J. Erdelsky <pje@efgh.com>
 * http://www.alumni.caltech.edu/~pje/
 *
 * The Rijndael encryption algorithm has been designed to replace the
 * aging DES algorithm. Like DES, it is a block cipher.
 * It uses 128-bit, 192-bit or 256-bit keys.
 * This implementation encrypts 128-bit blocks.
 * (DES used 56-bit keys and 64-bit blocks.)
 *
 * The code in this package is a modified version of an implementation
 * placed in the public domain by the following persons:
 *
 * * Vincent Rijmen <vincent.rijmen@esat.kuleuven.ac.be>
 * * Antoon Bosselaers <antoon.bosselaers@esat.kuleuven.ac.be>
 * * Paulo Barreto <paulo.barreto@terra.com.br>
 *
 */

#ifndef H__RIJNDAEL
#define H__RIJNDAEL

#include "public_defs.h"

int
rijndaelSetupEncrypt(uint32_t *rk,
                     const uint8_t *key,
                     int keybits);
int
rijndaelSetupDecrypt(uint32_t *rk,
                     const uint8_t *key,
                     int keybits);
void
rijndaelEncrypt(const uint32_t *rk,
                int nrounds,
                const uint8_t plaintext[16],
                uint8_t ciphertext[16]);
void
rijndaelDecrypt(const uint32_t *rk,
                int nrounds,
                const uint8_t ciphertext[16],
                uint8_t plaintext[16]);

#define KEYLENGTH(keybits) ((keybits)/8)
#define RKLENGTH(keybits)  ((keybits)/8+28)
#define NROUNDS(keybits)   ((keybits)/32+6)

#endif
