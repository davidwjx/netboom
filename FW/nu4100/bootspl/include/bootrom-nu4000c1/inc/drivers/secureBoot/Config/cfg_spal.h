/* cfg_spal.h
 *
 * Description: SPAL configuration constants.
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

#ifndef INCLUSION_GUARD_CFG_SPAL_H
#define INCLUSION_GUARD_CFG_SPAL_H

/* These are configuration constants for SPAL.
   The values have been tested with 32-bit and 64-bit Linux environments.
   Depending on target OS values may have to be adjusted. */

#if defined(__x86_64__)
/* For 64-bit environments: use wider pthread type. */
#define SPAL_CFG_THREAD_TYPE long int
#endif

#if defined(__x86_64__)
/* For 64 bit environments: try doubling the storage size. */
#define SPAL_CFG_MUTEX_SIZE 56
#define SPAL_CFG_MUTEX_ALIGN_TYPE long int
#else
#ifdef WIN32
/* This value is needed by the Win32 build */
#define SPAL_CFG_MUTEX_SIZE 36
#else
/* These value are large enough for encountered 32-bit linux variants. */
#define SPAL_CFG_MUTEX_SIZE 28
#endif
#define SPAL_CFG_MUTEX_ALIGN_TYPE long int
#endif


#if defined(__x86_64__)
/* For 64 bit environments: try doubling the storage size. */
#define SPAL_CFG_SEMAPHORE_SIZE       40
#define SPAL_CFG_SEMAPHORE_ALIGN_TYPE void *
#else
/* These value are large enough for encountered 32-bit linux variants. */
#define SPAL_CFG_SEMAPHORE_SIZE       20
#define SPAL_CFG_SEMAPHORE_ALIGN_TYPE void *
#endif

#endif /* Include Guard */

/* end of file cfg_spal.h */
