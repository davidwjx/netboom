/* spal_woe_mutex.c
 *
 * Description: Semaphore API implementation for Win32
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

#include <windows.h>

#define SPAL_MAGIC_MUTEX 0x34523623U
#define NO_OWNER (DWORD)(-1)

struct SPAL_MutexWoe
{
    uint32_t Magic;
    bool Locked;
    DWORD Owner;
    CRITICAL_SECTION CriticalSection;
};

typedef struct SPAL_MutexWoe SPAL_MutexWoe_t;

COMPILE_GLOBAL_ASSERT(sizeof(SPAL_MutexWoe_t) <= sizeof(SPAL_Mutex_t));

SPAL_Result_t
SPAL_Mutex_Init(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexWoe_t * const m_p = (SPAL_MutexWoe_t * const)Mutex_p;

    PRECONDITION(Mutex_p != NULL);

    InitializeCriticalSection(&m_p->CriticalSection);

    m_p->Owner = NO_OWNER;
    m_p->Magic = SPAL_MAGIC_MUTEX;
    m_p->Locked = false;

    return SPAL_SUCCESS;
}


void
SPAL_Mutex_Lock(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexWoe_t * const m_p = (SPAL_MutexWoe_t * const)Mutex_p;

    PRECONDITION(Mutex_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);
    PRECONDITION(m_p->Owner != GetCurrentThreadId());

    EnterCriticalSection(&m_p->CriticalSection);

    ASSERT(m_p->Locked == false);
    m_p->Locked = true;
    m_p->Owner = GetCurrentThreadId();
}


void
SPAL_Mutex_UnLock(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexWoe_t * const m_p = (SPAL_MutexWoe_t * const)Mutex_p;

    PRECONDITION(Mutex_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);
    PRECONDITION(m_p->Locked == true);
    PRECONDITION(m_p->Owner == GetCurrentThreadId());

    m_p->Locked = false;
    m_p->Owner = NO_OWNER;

    LeaveCriticalSection(&m_p->CriticalSection);
}

void
SPAL_Mutex_Destroy(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexWoe_t * const m_p = (SPAL_MutexWoe_t * const)Mutex_p;

    PRECONDITION(Mutex_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);
    PRECONDITION(m_p->Locked == false);

    DeleteCriticalSection(&m_p->CriticalSection);

    m_p->Magic = ~SPAL_MAGIC_MUTEX;
}

bool
SPAL_Mutex_IsLocked(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexWoe_t * const m_p = (SPAL_MutexWoe_t * const)Mutex_p;

    PRECONDITION(Mutex_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);

    return m_p->Locked;
}


SPAL_Result_t
SPAL_Mutex_TryLock(SPAL_Mutex_t * const Mutex_p)
{
    SPAL_MutexWoe_t * const m_p = (SPAL_MutexWoe_t * const)Mutex_p;

    PRECONDITION(Mutex_p != NULL);
    PRECONDITION(m_p->Magic == SPAL_MAGIC_MUTEX);
    PRECONDITION(m_p->Owner != GetCurrentThreadId());

    if (m_p->Locked)
    {
        return SPAL_RESULT_LOCKED;
    }

    if (TryEnterCriticalSection(&m_p->CriticalSection) != 0)
    {
        m_p->Locked = true;
        m_p->Owner = GetCurrentThreadId();

        return SPAL_SUCCESS;
    }

    return SPAL_RESULT_LOCKED;
}

/* end of file spal_woe_mutex.c */
