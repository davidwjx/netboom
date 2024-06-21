/* debug_abort.c
 *
 * Description: Implementation of DEBUG_abort.
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

#include "implementation_defs.h"

#include <stdlib.h>
#include <stdio.h>      // fflush, stderr


/* This logic is to make it possible to get coverage reports on
   software runs that end-up (intentionally) to abort. */
#ifdef DEBUG_CF_ABORT_WRITE_PROFILE
void __gcov_flush(void);                    /* Function to write profiles on disk. */
#define DEBUG_ABORT_WRITE_PROFILE __gcov_flush()
#else
#define DEBUG_ABORT_WRITE_PROFILE do { /* Not written. */ } while(0)
#endif


void
DEBUG_abort(void)
{
#ifdef WIN32
    // avoid the "report to microsoft?" dialog and the
    // "your program seems to have stopped abnormally" message
    _set_abort_behavior(0, _WRITE_ABORT_MSG + _CALL_REPORTFAULT);
#endif

    /* flush stderr before calling abort() to make sure
       out is not cut off due to buffering. */
    fflush(stderr);

    DEBUG_ABORT_WRITE_PROFILE;

    abort();
}

/* end of file debug_abort.c */
