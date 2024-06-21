/* log_impl.h
 *
 * Log Module, implementation for redirecting to a file.
 */

/*****************************************************************************
* Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_LOG_IMPL_H
#define INCLUDE_GUARD_LOG_IMPL_H

#include <stdio.h>      // fprintf

// this file must be opened and assigned to Log_FILE before
// any of the Log API functions may be used.
extern FILE * Log_FILE;

// following implementation requires Variadic Macro support
#define Log_Message(...)          fprintf(Log_FILE, __VA_ARGS__)
#define Log_FormattedMessage      Log_Message

// backwards compatible implementation
#define Log_FormattedMessageINFO  Log_FormattedMessage
#define Log_FormattedMessageWARN  Log_FormattedMessage
#define Log_FormattedMessageCRIT  Log_FormattedMessage

#endif /* Include Guard */

/* end of file log_impl.h */
