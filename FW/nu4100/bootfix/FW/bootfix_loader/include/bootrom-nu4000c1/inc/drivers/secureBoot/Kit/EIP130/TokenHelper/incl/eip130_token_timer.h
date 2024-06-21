/* eip130_token_timer.h
 *
 * Security Module Token helper functions
 * - Secure Timer related functions and definitions
 *
 * This module can convert a set of parameters into a Security Module Command
 * token, or parses a set of parameters from a Security Module Result token.
 */

/*****************************************************************************
* Copyright (c) 2014-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_EIP130TOKEN_TIMER_H
#define INCLUDE_GUARD_EIP130TOKEN_TIMER_H

#include "basic_defs.h"             // uint32_t, bool, inline, etc.

#include "eip130_token_common.h"    // Eip130Token_Command/Result_t


/*----------------------------------------------------------------------------
 * Eip130Token_Command_SecureTimer
 *
 * This function initializes the command token for a Secure Timer (Re)Start,
 * Stop and read operation.
 *
 * CommandToken_p
 *     Pointer to the command token buffer.
 *
 * AssetId (optional for start)
 *      AssetId of the secure timer to stop, read or restart.
 *
 * fSecond
 *      Second timer indication otherwise 100 us timer is used.
 *
 * Operation
 *      Secure timer operation to perform (re)start, stop or read.
 */
static inline void
Eip130Token_Command_SecureTimer(
        Eip130Token_Command_t * const CommandToken_p,
        const uint32_t AssetId,
        const bool fSecond,
        const uint16_t Operation)
{
    CommandToken_p->W[0] = (EIP130TOKEN_OPCODE_ASSETMANAGEMENT << 24) |
                           (EIP130TOKEN_SUBCODE_SECURETIMER << 28);
    CommandToken_p->W[2] = AssetId;
    CommandToken_p->W[3] = Operation & MASK_2_BITS;
    if (fSecond)
    {
        CommandToken_p->W[3] |= BIT_15;
    }
}


/*----------------------------------------------------------------------------
 * Eip130Token_Result_SecureTimer
 *
 * ResultToken_p
 *     Pointer to the result token buffer.
 *
 * AssetId_p (optional)
 *      Pointer to the variable in which the AssetId must be returned.
 *
 * ElapsedTime_p (optional)
 *      Pointer to the variable in which the elapsed time must be returned.
 */
static inline void
Eip130Token_Result_SecureTimer(
        const Eip130Token_Result_t * const ResultToken_p,
        uint32_t * const AssetId_p,
        uint32_t * const ElapsedTime_p)
{
    if (AssetId_p != NULL)
    {
        *AssetId_p = ResultToken_p->W[1];
    }
    if (ElapsedTime_p != NULL)
    {
        *ElapsedTime_p = ResultToken_p->W[2];
    }
}


#endif /* Include Guard */

/* end of file eip130_token_timer.h */
