/* log_safezone.c
 *
 * Log implementation for specific environment
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

#define LOG_SEVERITY_MAX  LOG_SEVERITY_NO_OUTPUT

#include "log.h"            // the API to implement


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
void
Log_HexDump_SafeZone(
    const char * szPrefix_p,
    const unsigned int PrintOffset,
    const uint8_t * Buffer_p,
    const unsigned int ByteCount,
    const char * FileLineStr_p)
{
    int BytesInThisLine = 16;
    unsigned int i;

    for(i = 0; i < ByteCount; i += 16)
    {
        // if we do not have enough data for a full line
        // then modify the line to cut off the printout

        // work-around due to the SafeZone macros
        // we have also dropped the newline
        if (i + 16 > ByteCount)
        {
            BytesInThisLine = ByteCount - i;
        }

        {
            char LineBuf[(3 * 16) + 1];
            char * p = LineBuf;
            int lp;

            for (lp = 0; lp < BytesInThisLine; lp++)
            {
                uint8_t b = Buffer_p[i + lp];
                char c;

                *p++ = ' ';

                c = (b >> 4) + '0';
                if (c > '9')
                {
                    c = c - '0' - 10 + 'A';
                }
                *p++ = c;

                c = (b & MASK_4_BITS) + '0';
                if (c > '9')
                {
                    c = c - '0' - 10 + 'A';
                }
                *p++ = c;
            }

            *p = 0;     // zero-terminate the string

            if (FileLineStr_p != NULL)
            {
                DEBUG_printf(
                    "LL_DEBUG, LF_LOG, %s: "
                    "%s %08d:%s\n",
                    FileLineStr_p,
                    szPrefix_p,
                    PrintOffset + i,
                    LineBuf);
            }
            else
            {
                DEBUG_printf(
                    "%s %08d:%s\n",
                    szPrefix_p,
                    PrintOffset + i,
                    LineBuf);
            }
        }
    }
}


/* end of file log_safezone.c */
