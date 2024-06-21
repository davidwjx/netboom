/* log_impl.h
 *
 * Log Module, implementation for SafeZone Framework
 */

/*****************************************************************************
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "implementation_defs.h"        // original L_DEBUG and helpers

void
Log_HexDump_SafeZone(
    const char * szPrefix_p,
    const unsigned int PrintOffset,
    const uint8_t * Buffer_p,
    const unsigned int ByteCount,
    const char * FileLineStr_p);

#define Log_HexDump(_szPrefix_p, _PrintOffset, _Buffer_p, _ByteCount) \
    Log_HexDump_SafeZone(_szPrefix_p, _PrintOffset, _Buffer_p, _ByteCount, __FILELINE__)

#define Print_HexDump(_szPrefix_p, _PrintOffset, _Buffer_p, _ByteCount) \
    Log_HexDump_SafeZone(_szPrefix_p, _PrintOffset, _Buffer_p, _ByteCount, NULL)

#ifndef IMPLDEFS_CF_DISABLE_L_DEBUG

#define Log_Message(_str) \
    DEBUG_printf("LL_DEBUG, LF_LOG, " __FILELINE__ ": " _str)

#define Log_FormattedMessage(...) \
    DEBUG_printf("LL_DEBUG, LF_LOG, " __FILELINE__ ": " __VA_ARGS__)

#define Log_FormattedMessageINFO(...) \
    DEBUG_printf("LL_DEBUG, LF_LOG_INFO, " __FILELINE__ ": " __VA_ARGS__)

#define Log_FormattedMessageWARN(...) \
    DEBUG_printf("LL_DEBUG, LF_LOG_WARN, " __FILELINE__ ": " __VA_ARGS__)

#define Log_FormattedMessageCRIT(...) \
    DEBUG_printf("LL_DEBUG, LF_LOG_CRIT, " __FILELINE__ ": " __VA_ARGS__)

#else

// debug logs are disabled
#define Log_Message(_str)
#define Log_FormattedMessage(...)
#define Log_FormattedMessageINFO(...)
#define Log_FormattedMessageWARN(...)
#define Log_FormattedMessageCRIT(...)

#endif /* IMPLDEFS_CF_DISABLE_DEBUG_L_PRINTF */

#endif /* Include Guard */

/* end of file log_impl.h */
