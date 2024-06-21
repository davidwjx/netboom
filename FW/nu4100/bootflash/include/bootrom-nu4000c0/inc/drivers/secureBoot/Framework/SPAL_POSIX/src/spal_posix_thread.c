/* spal_posix_thread.c
 *
 * Description: Posix thread APIs
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

#include "spal_thread.h"
#include "implementation_defs.h"

#ifdef __CC_ARM
// Use stubbed implemtation - no pthread implementation available
SPAL_Thread_t
SPAL_Thread_Self(void)
{
    return (SPAL_Thread_t)0;
}


SPAL_Result_t
SPAL_Thread_Create(SPAL_Thread_t * const Thread_p,
                   const void * const Reserved_p,
                   void * (* StartFunction_p)(void * const Param_p),
                   void * const ThreadParam_p)
{
    IDENTIFIER_NOT_USED(Thread_p);
    IDENTIFIER_NOT_USED(Reserved_p);
    IDENTIFIER_NOT_USED(ThreadParam_p);

    return SPAL_RESULT_NORESOURCE;
}

SPAL_Result_t
SPAL_Thread_Detach(const SPAL_Thread_t Thread)
{
    IDENTIFIER_NOT_USED(Thread);

    return SPAL_SUCCESS;
}

SPAL_Result_t
SPAL_Thread_Join(const SPAL_Thread_t Thread,
                 void ** const Status_p)
{
    IDENTIFIER_NOT_USED(Thread);
    IDENTIFIER_NOT_USED(Status_p);

    return SPAL_SUCCESS;
}

void
SPAL_Thread_Exit(void * const Status)
{
    IDENTIFIER_NOT_USED(Status);
}

#else

#include <pthread.h>
#include <stdlib.h>
#include <errno.h>


COMPILE_GLOBAL_ASSERT(sizeof(SPAL_Thread_t)>= sizeof(pthread_t));


SPAL_Thread_t
SPAL_Thread_Self(void)
{
    return (SPAL_Thread_t)pthread_self();
}


SPAL_Result_t
SPAL_Thread_Create(SPAL_Thread_t * const Thread_p,
                   const void * const Reserved_p,
                   void * (* StartFunction_p)(void * const Param_p),
                   void * const ThreadParam_p)
{
    pthread_t NewThread;
    int rval;

    PRECONDITION(Thread_p != NULL);
    PRECONDITION(Reserved_p == NULL);
    PRECONDITION(StartFunction_p != NULL);

#ifdef IMPLDEFS_CF_DISABLE_PRECONDITION
    PARAMETER_NOT_USED(Reserved_p);
#endif /* IMPLDEFS_CF_DISABLE_PRECONDITION */

    rval = pthread_create(&NewThread,
                          /* attr: */ NULL,
                          StartFunction_p,
                          ThreadParam_p);
    if (rval != 0)
    {
        ASSERT(errno == EAGAIN);

        return SPAL_RESULT_NORESOURCE;
    }

    *Thread_p = NewThread;

    return SPAL_SUCCESS;
}


SPAL_Result_t
SPAL_Thread_Detach(const SPAL_Thread_t Thread)
{
    SPAL_Result_t Result = SPAL_SUCCESS;
    pthread_t th = (pthread_t)Thread;
    int rval;

    rval = pthread_detach(th);
    if (rval != 0)
    {
        switch (errno)
        {
        case ESRCH:
            Result = SPAL_RESULT_NOTFOUND;
            break;

        case EINVAL:
            Result = SPAL_RESULT_INVALID;
            break;
        }

        POSTCONDITION(Result != SPAL_SUCCESS);

        return Result;
    }

    POSTCONDITION(Result == SPAL_SUCCESS);

    return Result;
}



SPAL_Result_t
SPAL_Thread_Join(const SPAL_Thread_t Thread,
                 void ** const Status_p)
{
    SPAL_Result_t Result = SPAL_SUCCESS;
    pthread_t th = Thread;
    void * thread_return;
    int rval;

    PRECONDITION(Thread != SPAL_Thread_Self());

    rval = pthread_join(th, &thread_return);
    if (rval != 0)
    {
        switch (errno)
        {
        case ESRCH:
            // No thread with the ID
            Result = SPAL_RESULT_NOTFOUND;
            break;

        case EINVAL:
            // The thread was detached or
            // another thread already waiting the thread.
            Result = SPAL_RESULT_INVALID;
            break;

        case EDEADLOCK:
            PANIC("pthread join returned EDEADLOCK");
            break;
        }

        POSTCONDITION(Result != SPAL_SUCCESS);

        return Result;
    }

    POSTCONDITION(Result == SPAL_SUCCESS);

    if (Status_p != NULL)
    {
        *Status_p = thread_return;
    }

    return SPAL_SUCCESS;
}


void
SPAL_Thread_Exit(void * const Status)
{
    pthread_exit(Status);
}

#endif


/* end of file spal_posix_thread.c */
