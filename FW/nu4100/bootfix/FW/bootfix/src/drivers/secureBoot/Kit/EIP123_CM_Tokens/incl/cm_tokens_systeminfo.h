/* cm_tokens_systeminfo.h
 *
 * Crypto Module Tokens Parser/Generator - System Info Token
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

#ifndef INCLUDE_GUARD_CM_TOKENS_SYSTEMINFO_H
#define INCLUDE_GUARD_CM_TOKENS_SYSTEMINFO_H

#include "basic_defs.h"         // uint32_t, bool, inline, etc.
#include "c_lib.h"              // c_memset

#include "cm_tokens_common.h"   // CMTokens_Command/Response

// NVM error codes
enum
{
    CMTOKENS_SYSINFO_NVM_OK = 0,
    CMTOKENS_SYSINFO_NVM_PROTECTED_ASSET_REPLACED = 1,
    CMTOKENS_SYSINFO_NVM_PROTECTED_ASSET_REMOVED = 2,
    CMTOKENS_SYSINFO_NVM_PREMATURE_END = 3,
    CMTOKENS_SYSINFO_NVM_PROGRAMMED_BIT_MISMATCH = 4
};

typedef struct
{
    struct
    {
        uint8_t Major, Minor, Patch;    // 0..9 each
        uint16_t MemorySizeInBytes;
    } Hardware;

    struct
    {
        uint8_t Major, Minor, Patch;    // 0..9 each
        bool fIsTestFW;
    } Firmware;

    struct
    {
        uint8_t HostID;
        uint32_t Identity;
    } SelfIdentity;

    struct
    {
        uint8_t ErrorCode;
        uint16_t ErrorLocation;
    } NVM;

} CMTokens_SystemInfo_t;


/*----------------------------------------------------------------------------
 * CMTokens_MakeCommand_ReadSystemInfo
 *
 * This function writes the Read System Version command token.
 *
 * CommandToken_p
 *     Pointer to the command token buffer this function will write to.
 */
static inline void
CMTokens_MakeCommand_ReadSystemInfo(
    CMTokens_Command_t * const CommandToken_p)
{
    // Word0
    CommandToken_p->W[0] =
        (15 << 24) |        // Opcode
        (0 << 28);          // Subcode
}


/*----------------------------------------------------------------------------
 * CMTokens_ParseResponse_ReadSystemInfo
 *
 * This function parses the Read Firmware Version response token. It can also
 * be used to query the (fixed) length of the firmware versions string this
 * function can generate.
 *
 * ResponseToken_p
 *     Pointer to the command token buffer this function will write to.
 *
 * Version_p
 *     Pointer to version structure that this function will populate.
 */
static inline void
CMTokens_ParseResponse_ReadSystemInfo(
    const CMTokens_Response_t * const ResponseToken_p,
    CMTokens_SystemInfo_t * const Info_p)
{
    uint32_t MaMiPa;

    c_memset(Info_p, 0, sizeof(CMTokens_SystemInfo_t));

    MaMiPa = ResponseToken_p->W[1];
    Info_p->Firmware.Major = (uint8_t)(MaMiPa >> 16);
    Info_p->Firmware.Minor = (uint8_t)(MaMiPa >> 8);
    Info_p->Firmware.Patch = (uint8_t)MaMiPa;
    if (MaMiPa & BIT_31)
    {
        Info_p->Firmware.fIsTestFW = true;
    }

    MaMiPa = ResponseToken_p->W[2];
    Info_p->Hardware.Major = (uint8_t)(MaMiPa >> 16);
    Info_p->Hardware.Minor = (uint8_t)(MaMiPa >> 8);
    Info_p->Hardware.Patch = (uint8_t)MaMiPa;

    Info_p->Hardware.MemorySizeInBytes = (uint16_t)ResponseToken_p->W[3];

    Info_p->SelfIdentity.HostID = MASK_4_BITS & (ResponseToken_p->W[3] >> 16);
    Info_p->SelfIdentity.Identity = ResponseToken_p->W[4];

    Info_p->NVM.ErrorLocation = MASK_12_BITS & ResponseToken_p->W[5];
    Info_p->NVM.ErrorCode = MASK_4_BITS & (ResponseToken_p->W[5] >> 12);
}

#endif /* Include Guard */

/* end of file cm_tokens_systeminfo.h */
