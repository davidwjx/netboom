/* cm_tokens_nop.h
 *
 * Crypto Module Tokens Parser/Generator - NOP Token
 *
 * This module can converts a set of parameters into a Crypto Module Command
 * token, or parses a set of parameters from a Crypto Module Response token.
 */

/*****************************************************************************
* Copyright (c) 2010-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_CM_TOKENS_NOP_H
#define INCLUDE_GUARD_CM_TOKENS_NOP_H

#include "basic_defs.h"         // uint32_t, bool, inline, etc.
#include "cm_tokens_common.h"   // CMTokens_Command_t
#include "eip123_dma.h"         // EIP123_DescriptorChain_t


/*----------------------------------------------------------------------------
 * CMTokens_MakeCommand_Nop
 *
 * DataLength
 *     Number of bytes to copy.
 *     Must be a multiple of 4.
 */
static inline void
CMTokens_MakeCommand_Nop(
    CMTokens_Command_t * const CommandToken_p,
    const uint32_t DataLength)
{
    CommandToken_p->W[0] = 0;   // Opcode = 0 = Nop
    CommandToken_p->W[2] = DataLength;
}


/*----------------------------------------------------------------------------
 * CMTokens_MakeCommand_Nop_WriteIn/OutDescriptor
 */
static inline void
CMTokens_MakeCommand_Nop_WriteInDescriptor(
    CMTokens_Command_t * const CommandToken_p,
    const EIP123_DescriptorChain_t * const Descriptor_p)
{
    CMTokens_MakeCommand_WriteInDescriptor(CommandToken_p, 3, 3, Descriptor_p);
}


static inline void
CMTokens_MakeCommand_Nop_WriteOutDescriptor(
    CMTokens_Command_t * const CommandToken_p,
    const EIP123_DescriptorChain_t * const Descriptor_p)
{
    CMTokens_MakeCommand_WriteOutDescriptor(CommandToken_p, 6, 3, Descriptor_p);
}

#endif /* Include Guard */

/* end of file cm_tokens_nop.h */
