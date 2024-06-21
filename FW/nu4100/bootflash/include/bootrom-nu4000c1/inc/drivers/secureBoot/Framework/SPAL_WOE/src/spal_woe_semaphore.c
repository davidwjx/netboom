/* spal_woe_semaphore.c
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

#include "spal_semaphore.h"
#include "implementation_defs.h"

#include <windows.h>

#define SPAL_MAGIC_SEMAPHORE 0x34523624U

struct SPAL_SemaphoreWoe
{
    uint32_t Magic;
    HANDLE Sem;
};

typedef struct SPAL_SemaphoreWoe SPAL_SemaphoreWoe_t;

COMPILE_GLOBAL_ASSERT(sizeof(SPAL_SemaphoreWoe_t)<= sizeof(SPAL_Semaphore_t));


SPAL_Result_t
SPAL_Semaphore_Init(SPAL_Semaphore_t * const Semaphore_p,
                    const unsigned int InitialCount)
{
    SPAL_SemaphoreWoe_t * const Sem_p = (SPAL_SemaphoreWoe_t * const)Semaphore_p;
    HANDLE Sem;

    PRECONDITION(Semaphore_p != NULL);

    Sem = CreateSemaphore(NULL, InitialCount, 100, NULL);
    if (Sem == NULL)
    {
        Sem_p->Magic = ~SPAL_MAGIC_SEMAPHORE;

        return SPAL_RESULT_NORESOURCE;
    }

    Sem_p->Sem = Sem;
    Sem_p->Magic = SPAL_MAGIC_SEMAPHORE;

    return SPAL_SUCCESS;
}


void
SPAL_Semaphore_Wait(SPAL_Semaphore_t * const Semaphore_p)
{
    SPAL_SemaphoreWoe_t * const Sem_p = (SPAL_SemaphoreWoe_t * const)Semaphore_p;
    DWORD rval;

    PRECONDITION(Semaphore_p != NULL);
    PRECONDITION(Sem_p->Magic == SPAL_MAGIC_SEMAPHORE);

    rval = WaitForSingleObject(Sem_p->Sem, INFINITE);

    ASSERT(rval == WAIT_OBJECT_0);
}


SPAL_Result_t
SPAL_Semaphore_TryWait(SPAL_Semaphore_t * const Semaphore_p)
{
    SPAL_SemaphoreWoe_t * const Sem_p = (SPAL_SemaphoreWoe_t * const)Semaphore_p;
    DWORD rval;

    PRECONDITION(Semaphore_p != NULL);
    PRECONDITION(Sem_p->Magic == SPAL_MAGIC_SEMAPHORE);

    rval = WaitForSingleObject(Sem_p->Sem, 0);
    if (rval == WAIT_TIMEOUT)
    {
        return SPAL_RESULT_LOCKED;
    }

    ASSERT(rval == WAIT_OBJECT_0);

    return SPAL_SUCCESS;
}



SPAL_Result_t
SPAL_Semaphore_TimedWait(SPAL_Semaphore_t * const Semaphore_p,
                         const unsigned int TimeoutMilliSeconds)
{
    SPAL_SemaphoreWoe_t * const Sem_p = (SPAL_SemaphoreWoe_t * const)Semaphore_p;
    DWORD rval;

    PRECONDITION(Semaphore_p != NULL);
    PRECONDITION(Sem_p->Magic == SPAL_MAGIC_SEMAPHORE);

    rval = WaitForSingleObject(Sem_p->Sem, TimeoutMilliSeconds);
    if (rval == WAIT_TIMEOUT)
    {
        return SPAL_RESULT_TIMEOUT;
    }

    ASSERT(rval == WAIT_OBJECT_0);

    return SPAL_SUCCESS;
}



void
SPAL_Semaphore_Post(SPAL_Semaphore_t * const Semaphore_p)
{
    SPAL_SemaphoreWoe_t * const Sem_p = (SPAL_SemaphoreWoe_t * const)Semaphore_p;
    BOOL rbool;

    PRECONDITION(Semaphore_p != NULL);
    PRECONDITION(Sem_p->Magic == SPAL_MAGIC_SEMAPHORE);

    rbool = ReleaseSemaphore(Sem_p->Sem, 1, NULL);

    ASSERT(rbool != 0);
}



void
SPAL_Semaphore_Destroy(SPAL_Semaphore_t * const Semaphore_p)
{
    SPAL_SemaphoreWoe_t * const Sem_p = (SPAL_SemaphoreWoe_t * const)Semaphore_p;
    BOOL rbool;

    PRECONDITION(Semaphore_p != NULL);
    PRECONDITION(Sem_p->Magic == SPAL_MAGIC_SEMAPHORE);

    rbool = CloseHandle(Sem_p->Sem);

    Sem_p->Magic = ~SPAL_MAGIC_SEMAPHORE;

    ASSERT(rbool != 0);
}

/* end of file spal_woe_semaphore.c */
