/* cfg_random.h
 *
 * Description: Configuration for SW random functionality
 */

/*****************************************************************************
* Copyright (c) 2010-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_CFG_RANDOM_H
#define INCLUDE_GUARD_CFG_RANDOM_H

/* Device for acquiring entropy or take the random numbers from.
 * This device may be blocking.
 */
#ifndef CFG_RANDOM_ENTROPY_DEVICE
#define CFG_RANDOM_ENTROPY_DEVICE  "/dev/random"
#endif

/* Device for acquiring entropy or take the random numbers from.
 * This device shall be non-blocking.
 * (If there is no non-blocking randomness device in the system,
 * do not define a file here)
 */
#ifndef CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE
#define CFG_RANDOM_NONBLOCKING_ENTROPY_DEVICE  "/dev/urandom"
#endif

/* Requested level of randomness quality (the only one supported)
 */
#define CFG_RANDOM_QUALITY 128

/* File for storing randomness state.
 * When available, this file is used instead of CFG_RANDOM_ENTROPY_DEVICE.
 */
#define CFG_RANDOM_STATE_FILENAME ".nist_drbg_statefile.bin"

/* Use Unix/posix function calls to ensure state file access is save.
 * umask is used to protect the file (only readable for the user),
 * flock prevents simultaneous access by multiple processes and
 * fsync ensures changes are written to disc in case of unexpected
 * power down or other failure.
 */
//#define CFG_RANDOM_STATE_FILE_UNIX

#endif /* Include Guard */

/* end of file cfg_random.h*/
