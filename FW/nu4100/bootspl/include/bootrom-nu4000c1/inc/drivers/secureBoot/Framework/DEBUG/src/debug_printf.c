/* debug_printf.c
 *
 * Description: Implementation of DEBUG_printf.
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

#include <stdarg.h>
#include <stdio.h>

#ifdef DEBUG_CF_USE_STDOUT
#define DEBUG_fd  stdout
#else
#define DEBUG_fd  stderr
#endif

int
DEBUG_printf(const char * format, ...)
{
    va_list ap;

    va_start(ap, format);

    (void)vfprintf(DEBUG_fd, format, ap);

#ifdef DEBUG_CF_USE_FLUSH
    (void)fflush(DEBUG_fd);
#endif

    va_end(ap);

    return 0;
}

/* end of file debug_printf.c */
