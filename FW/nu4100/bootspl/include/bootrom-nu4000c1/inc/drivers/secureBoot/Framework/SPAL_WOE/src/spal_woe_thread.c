/* spal_woe_thread.c
 *
 * Description: Win32 specific thread APIs
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

#include <windows.h>

COMPILE_GLOBAL_ASSERT(sizeof(void *) == sizeof(DWORD));
COMPILE_GLOBAL_ASSERT(voidsizeof(SPAL_Thread_t) == sizeof(HANDLE));

SPAL_Thread_t
SPAL_Thread_Self(void)
{
    return (SPAL_Thread_t)GetCurrentThreadId();
}


SPAL_Result_t
SPAL_Thread_Create(SPAL_Thread_t * const Thread_p,
                   const void * const Reserved_p,
                   void * (* StartFunction_p)(void * const Param_p),
                   void * const ThreadParam_p)
{
    HANDLE ThreadHandle;

    PARAMETER_NOT_USED(Reserved_p);

    ThreadHandle = CreateThread(/* lpThreadAttributes: */ NULL,
                                /* dwStackSize: */ 0,
                                (LPTHREAD_START_ROUTINE)StartFunction_p,
                                ThreadParam_p,
                                /* dwCreationFlags: */ 0,
                                /* lpThreadId: */ NULL);
    if (ThreadHandle == NULL)
    {
        return SPAL_RESULT_NORESOURCE;
    }

    *Thread_p = (SPAL_Thread_t)ThreadHandle;

    return SPAL_SUCCESS;
}

SPAL_Result_t
SPAL_Thread_Detach(const SPAL_Thread_t Thread)
{
    PARAMETER_NOT_USED(Thread);

    return SPAL_SUCCESS;
}


SPAL_Result_t
SPAL_Thread_Join(const SPAL_Thread_t Thread,
                 void ** const Status_p)
{
    HANDLE ThreadHandle = (HANDLE)Thread;
    BOOL rbool;
    DWORD Status;
    DWORD rval;

    rval = WaitForSingleObject(ThreadHandle, INFINITE);
    if (rval != WAIT_OBJECT_0)
    {
        return SPAL_RESULT_INVALID;
    }

    rbool = GetExitCodeThread(ThreadHandle, &Status);
    if (rbool == 0)
    {
        return SPAL_RESULT_INVALID;
    }

    if (Status_p != NULL)
    {
        *Status_p = (void *)Status;
    }

    return SPAL_SUCCESS;
}


void
SPAL_Thread_Exit(void * const Status)
{
    ExitThread((DWORD)Status);
}


/* end of file spal_woe_thread.c */
