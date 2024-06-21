/* sbhybrid_eip130_common.c
 *
 * Common Security Module services for Secure Boot Library
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

#include "sbhybrid_internal.h"

#ifdef SBHYBRID_WITH_EIP130
#include "eip130.h"
#include "cs_eip130_token.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"

// For tracing/debugging purpose, dump the command and result tokens
//#define TOKENS_VERBOSE

/*----------------------------------------------------------------------------
 * SBHYBRID_EIP130_Common_ReadResultAndCheck
 *
 * This function reads the EIP-130 reply message from the mailbox, checks for
 * errors and optionally reports the error. This function is sufficient for
 * most responses and is called from most SBHYBRID_*_ReadResult functions.
 * This function is shared by hashing and decryption.
 */
SB_Result_t
SBHYBRID_EIP130_Common_WriteCommand(Device_Handle_t const Device_EIP130,
                                    Eip130Token_Command_t * t_cmd_p,
                                    uint8_t MailboxNr)
{
    int res;

#ifdef TOKENS_VERBOSE
    L_PRINTF(LL_VERBOSE, LF_TOKEN, "Command (non-zero words only):");
    for (res = 0; res < EIP130TOKEN_COMMAND_WORDS; res++)
    {
        if (t_cmd_p->W[res])
        {
            L_PRINTF(LL_VERBOSE, LF_TOKEN, "  W%02d=0x%08X",
                     res, t_cmd_p->W[res]);
        }
    }
#endif

    res = EIP130_MailboxWriteAndSubmitToken(Device_EIP130, MailboxNr, t_cmd_p);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Command (0x%08X) write failed (%d).",
                t_cmd_p->W[0], res);
        return SB_ERROR_HARDWARE;
    }
    return SBHYBRID_PENDING;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP130_Common_ReadResultAndCheck
 *
 * This function reads the EIP-130 reply message from the mailbox, checks for
 * errors and optionally reports the error. This function is sufficient for
 * most responses and is called from most SBHYBRID_*_ReadResult functions.
 * This function is shared by hashing and decryption.
 */
SB_Result_t
SBHYBRID_EIP130_Common_ReadResultAndCheck(Device_Handle_t const Device_EIP130,
                                          Eip130Token_Result_t * t_res_p,
                                          uint8_t MailboxNr)
{
    int res;

    res = EIP130_MailboxReadToken(Device_EIP130, MailboxNr, t_res_p);
    if (res == 0)
    {
#ifdef TOKENS_VERBOSE
        if (t_res_p->W[0] & BIT_31)
        {
            // Error - only first word is relevant
            L_PRINTF(LL_VERBOSE, LF_TOKEN, "Error Result W00=0x%08X",
                     t_res_p->W[0]);
        }
        else
        {
            L_PRINTF(LL_VERBOSE, LF_TOKEN, "Result (non-zero words only):");
            for (res = 0; res < EIP130TOKEN_RESULT_WORDS; res++)
            {
                if (t_res_p->W[res])
                {
                    L_PRINTF(LL_VERBOSE, LF_TOKEN, "  W%02d=0x%08X",
                             res, t_res_p->W[res]);
                }
            }
        }
#endif

        res = Eip130Token_Result_Code(t_res_p);
        if (res >= 0)
        {
            return SB_SUCCESS;
        }

        L_DEBUG(LF_SBHYBRID, "ERROR: Result respone %d.", res);
    }
    else
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Read result failed (%d).", res);
    }
    return SB_ERROR_HARDWARE;
}


uint64_t
SBHYBRID_EIP130_Common_GetDmaAddress(SBHYBRID_SymmContext_t * const Context_p,
                                     const void * Data_p,
                                     const uint32_t DataLength)
{
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Handle_t DMAHandle = NULL;
    int res;

    // Get the physical address of the data
    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = (void *)Data_p;

    Props.Alignment = 4;
    Props.Size = ((DataLength + 3) & ~3);

    res = DMAResource_CheckAndRegister(Props, AddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: CheckAndRegister failed (%d).", res);
        return 0;
    }

    // Remember the handle
    Context_p->DMAHandles[Context_p->DMAHandleCount++] = DMAHandle;

    // Translate address
    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP13xDMA, &AddrPair);
    if (res != 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Translate failed (%d).", res);
        return 0;
    }

    L_DEBUG(LF_SBHYBRID, "Address %u.", (unsigned int)AddrPair.Address.Value64);
    return AddrPair.Address.Value64;
}


uint64_t
SBHYBRID_EIP130_Common_DmaAlloc(SBHYBRID_SymmContext_t * const Context_p,
                                const uint32_t DataLength,
                                void ** Host_pp)
{
    // allocate buffers for the DMA buffer descriptor chains
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Handle_t DMAHandle = NULL;
    int res;

    // Allocate a buffer for the data
    Props.Alignment = 4;
    Props.Size      = ((DataLength + 3) & ~3);

    res = DMAResource_Alloc(Props, &AddrPair, &DMAHandle);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: DmaAlloc failed (%d)", res);
        return 0;
    }

    // Remember the handle and return host related address if requested
    Context_p->DMAHandles[Context_p->DMAHandleCount++] = DMAHandle;

    if (Host_pp != NULL)
    {
        *Host_pp = (uint8_t *)AddrPair.Address.Native_p;
    }

    // Translate address
    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP13xDMA, &AddrPair);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Translate failed (%d).", res);
        return 0;
    }

    L_DEBUG(LF_SBHYBRID, "Address %u.", (unsigned int)AddrPair.Address.Value64);
    return AddrPair.Address.Value64;
}


void
SBHYBRID_EIP130_Common_DmaRelease(SBHYBRID_SymmContext_t * const Context_p)
{
    while (Context_p->DMAHandleCount > 0)
    {
        Context_p->DMAHandleCount--;
        DMAResource_Release(Context_p->DMAHandles[Context_p->DMAHandleCount]);
    }
}


void
SBHYBRID_EIP130_Common_PreDma(SBHYBRID_SymmContext_t * const Context_p,
                              const DMAResource_Handle_t Handle)
{
    if (Handle == NULL)
    {
        if (Context_p->DMAHandleCount > 0)
        {
            DMAResource_PreDMA(Context_p->DMAHandles[Context_p->DMAHandleCount - 1],
                               0, 0);
        }
    }
    else
    {
        DMAResource_PreDMA(Handle, 0, 0);
    }
}


#endif /* SBHYBRID_WITH_EIP130 */

