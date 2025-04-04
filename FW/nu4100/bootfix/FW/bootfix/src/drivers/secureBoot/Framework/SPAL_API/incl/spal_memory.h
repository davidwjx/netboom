/* spal_memory.h
 *
 * Description: Memory management routines
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

#ifndef INCLUDE_GUARD_SPAL_MEMORY_H_
#define INCLUDE_GUARD_SPAL_MEMORY_H_

#include "public_defs.h"

void *
SPAL_Memory_Alloc(
    const size_t Size);


void
SPAL_Memory_Free(
    void * const Memory_p);


void *
SPAL_Memory_Calloc(
    const size_t MemberCount,
    const size_t MemberSize);


void *
SPAL_Memory_ReAlloc(
    void * const Mem_p,
    size_t NewSize);

#endif /* Include guard */

/* end of file spal_memory.h */
