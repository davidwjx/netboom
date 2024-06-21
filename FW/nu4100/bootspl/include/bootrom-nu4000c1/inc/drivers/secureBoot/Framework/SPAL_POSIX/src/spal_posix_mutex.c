/* spal_posix_mutex.c
 *
 * Description: Posix Mutex APIs
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

#include "spal_mutex.h"
#include "implementation_defs.h"

#ifdef __CC_ARM
// Use stubbed implemtation - no pthread implementation available

SPAL_Result_t
SPAL_Mutex_Init(SPAL_Mutex_t * const Mutex_p)
{
    IDENTIFIER_NOT_USED(Mutex_p);

    return SPAL_RESULT_NORESOURCE;
}

void
SPAL_Mutex_Lock(SPAL_Mutex_t * const Mutex_p)
{
    IDENTIFIER_NOT_USED(Mutex_p);
}

void
SPAL_Mutex_UnLock(SPAL_Mutex_t * const Mutex_p)
{
    IDENTIFIER_NOT_USED(Mutex_p);
}


void
SPAL_Mutex_Destroy(SPAL_Mutex_t * const Mutex_p)
{
    IDENTIFIER_NOT_USED(Mutex_p);
}

bool
SPAL_Mutex_IsLocked(SPAL_Mutex_t * const Mutex_p)
{
    IDENTIFIER_NOT_USED(Mutex_p);

    return false;
}

SPAL_Result_t
SPAL_Mutex_TryLock(SPAL_Mutex_t * const Mutex_p)
{
    IDENTIFIER_NOT_USED(Mutex_p);

    return SPAL_SUCCESS;
}

#else

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define SPAL_MAGIC_MUTEX 0x55555555

struct SPAL_MutexPosix
{
    uint32_t Magic;
    pthread_mutex_t Mutex;
};

typedef struct SPAL_MutexPosix SPAL_MutexPosix_t;

COMPILE_GLOBAL_ASSERT(sizeof(SPAL_MutexPosix_t)<= sizeof(SPAL_Mutex_t));

SPAL_Result_t
SPAL_Mutex_Init(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexPosix_t * const m_p = (SPAL_MutexPosix_t * const)Mutex_p;

    PRECONDITION(m_p != NULL);

    if (pthread_mutex_init(&m_p->Mutex, NULL) != 0)
    {
        m_p->Magic = 0;

        POSTCONDITION(m_p->Magic != SPAL_MAGIC_MUTEX);

        return SPAL_RESULT_NORESOURCE;
    }

    m_p->Magic = SPAL_MAGIC_MUTEX;

    POSTCONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    return SPAL_SUCCESS;
}


void
SPAL_Mutex_Lock(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexPosix_t * const m_p = (SPAL_MutexPosix_t * const)Mutex_p;
    int rval;

    PRECONDITION(m_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    rval = pthread_mutex_lock(&m_p->Mutex);

    ASSERT(rval == 0);
}


void
SPAL_Mutex_UnLock(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexPosix_t * const m_p = (SPAL_MutexPosix_t * const)Mutex_p;
    int rval;

    PRECONDITION(m_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    rval = pthread_mutex_unlock(&m_p->Mutex);

    ASSERT(rval == 0);
}


void
SPAL_Mutex_Destroy(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexPosix_t * const m_p = (SPAL_MutexPosix_t * const)Mutex_p;
    int rval;

    PRECONDITION(m_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    rval = pthread_mutex_destroy(&m_p->Mutex);
    m_p->Magic = 0;

    ASSERT(rval == 0);

    POSTCONDITION(m_p->Magic != SPAL_MAGIC_MUTEX);
}


bool
SPAL_Mutex_IsLocked(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexPosix_t * const m_p = (SPAL_MutexPosix_t * const)Mutex_p;
    int rval;

    PRECONDITION(m_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    rval = pthread_mutex_trylock(&m_p->Mutex);

    ASSERT(rval == EBUSY || rval == 0);

    if (rval == EBUSY)
    {
        return true;
    }

    rval = pthread_mutex_unlock(&m_p->Mutex);

    ASSERT(rval == 0);

    return false;
}


SPAL_Result_t
SPAL_Mutex_TryLock(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexPosix_t * const m_p = (SPAL_MutexPosix_t * const)Mutex_p;
    int rval;

    PRECONDITION(m_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    rval = pthread_mutex_trylock(&m_p->Mutex);

    ASSERT(rval == EBUSY || rval == 0);

    if (rval == EBUSY)
    {
        return SPAL_RESULT_LOCKED;
    }

    return SPAL_SUCCESS;
}

#endif


/* end of file spal_posix_mutex.c */
