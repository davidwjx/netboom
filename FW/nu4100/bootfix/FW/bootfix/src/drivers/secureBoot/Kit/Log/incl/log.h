/* log.h
 *
 * Logging API
 *
 * The service provided by this interface allows the caller to output trace
 * messages. The implementation can use whatever output channel is available
 * in a specific environment.
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

#ifndef INCLUDEGUARD_LOG_H
#define INCLUDEGUARD_LOG_H

#include "basic_defs.h"
#include "log_impl.h"           // implementation specifics


/*----------------------------------------------------------------------------
 * LOG_SEVERITY_MAX
 *
 * This preprocessor symbol is used to control the definition of three macros
 * that can be used to selectively compile three classes of log messages:
 * Informational, Warnings and Critical. Define this symbol before including
 * this header file. When absent, full logging is assumed.
 */

// set LOG_SEVERITY_MAX to one of the following values
#define LOG_SEVERITY_NO_OUTPUT  0
#define LOG_SEVERITY_CRIT       1
#define LOG_SEVERITY_CRITICAL   1
#define LOG_SEVERITY_WARN       2
#define LOG_SEVERITY_WARNING    2
#define LOG_SEVERITY_INFO       3

#ifndef LOG_SEVERITY_MAX
#ifdef _MSC_VER
#pragma message("LOG_SEVERITY_MAX is undefined; assuming LOG_SEVERITY_INFO")
#else
#warning "LOG_SEVERITY_MAX is undefined; assuming LOG_SEVERITY_INFO"
#endif
#define LOG_SEVERITY_MAX  LOG_SEVERITY_INFO
#endif


/*----------------------------------------------------------------------------
 * LOG_CRIT_ENABLED
 * LOG_WARN_ENABLED
 * LOG_INFO_ENABLED
 *
 * This preprocessor symbols can be used to test if a specific class of log
 * message has been enabled by the LOG_SEVERITY_MAX selection.
 *
 * Example usage:
 *
 * #ifdef LOG_SEVERITY_INFO
 * // dump command descriptor details to log
 * #endif
 */

#if LOG_SEVERITY_MAX >= LOG_SEVERITY_CRITICAL
#define LOG_CRIT_ENABLED
#endif

#if LOG_SEVERITY_MAX >= LOG_SEVERITY_WARNING
#define LOG_WARN_ENABLED
#endif

#if LOG_SEVERITY_MAX >= LOG_SEVERITY_INFO
#define LOG_INFO_ENABLED
#endif


/*----------------------------------------------------------------------------
 * Log_Message
 *
 * This function adds a simple constant message to the log buffer.
 *
 * Message_p
 *     Pointer to the zero-terminated log message. The message must be
 *     complete and terminated with a newline character ("\n"). This avoids
 *     blending of partial messages.
 *
 * Return Value
 *     None.
 */
#ifndef Log_Message
void
Log_Message(
    const char * szMessage_p);
#endif


/*----------------------------------------------------------------------------
 * Log_HexDump
 *
 * This function logs Hex Dump of a Buffer
 *
 * szPrefix
 *     Prefix to be printed on every row.
 *
 * PrintOffset
 *     Offset value that is printed at the start of every row. Can be used
 *     when the byte printed are located at some offset in another buffer.
 *
 * Buffer_p
 *     Pointer to the start of the array of bytes to hex dump.
 *
 * ByteCount
 *     Number of bytes to include in the hex dump from Buffer_p.
 *
 * Return Value
 *     None.
 */
#ifndef Log_HexDump
void
Log_HexDump(
    const char * szPrefix_p,
    const unsigned int PrintOffset,
    const uint8_t * Buffer_p,
    const unsigned int ByteCount);
#endif


/*----------------------------------------------------------------------------
 * Log_FormattedMessage
 *
 * This function allows a message to be composed and output using a format
 * specifier in line with printf. Caller should be restrictive in the format
 * options used since not all platforms support all exotic variants.
 *
 * szFormat_p
 *     Pointer to the zero-terminated format specifier string.
 *
 * ...
 *     Variable number of additional arguments. These will be processed
 *     according to the specifiers found in the format specifier string.
 *
 * Return Value
 *     None.
 */
#ifndef Log_FormattedMessage
void
Log_FormattedMessage(
    const char * szFormat_p,
    ...);
#endif


/*----------------------------------------------------------------------------
 * LOG_CRIT
 * LOG_WARN
 * LOG_INFO
 *
 * These three helper macros can be used to conditionally compile code that
 * outputs log messages and make the actual log line more compact.
 * Each macro is enabled when the class of messages is activated with the
 * LOG_SEVERITY_MAX setting.
 *
 * Example usage:
 *
 * LOG_INFO("MyFunc: selected mode %u (%s)\n", mode, Mode2Str[mode]);
 *
 * LOG_INFO(
 *      "MyFunc: "
 *      "selected mode %u (%s)\n",
 *      mode,
 *      Mode2Str[mode]);
 *
 * LOG_WARN("MyFunc: Unexpected return value %d\n", res);
 */

#undef LOG_CRIT

#ifdef LOG_CRIT_ENABLED
#define LOG_CRIT Log_FormattedMessageCRIT
#else
#define LOG_CRIT(...)
#endif

#undef LOG_WARN
#ifdef LOG_WARN_ENABLED
#define LOG_WARN Log_FormattedMessageWARN
#else
#define LOG_WARN(...)
#endif

#undef LOG_INFO
#ifdef LOG_INFO_ENABLED
#define LOG_INFO Log_FormattedMessageINFO
#else
#define LOG_INFO(...)
#endif

#endif /* Include Guard */

/* end of file log.h */
