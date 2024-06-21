/* sbhybrid_hw_init.c
 *
 * Initialization of HW engines for use by Secure Boot.
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

#ifndef __MODULE__
#define __MODULE__ "sbhybrid_hw_init.c"
#endif

#include "sbhybrid_internal.h"      // the API to implement

#if defined(SBHYBRID_WITH_EIP28) || defined(SBHYBRID_WITH_EIP93) || defined(SBHYBRID_WITH_EIP123) || defined(SBHYBRID_WITH_EIP130)
#include "device_mgmt.h"
#include "device_rw.h"
#endif
#ifdef SBHYBRID_WITH_EIP28
#include "eip28.h"
#endif /* SBHYBRID_WITH_EIP28 */
#ifdef SBHYBRID_WITH_EIP93
#include "dmares_mgmt.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "eip93.h"
#include "eip93_arm.h"
#endif /* SBHYBRID_WITH_EIP93 */
#ifdef SBHYBRID_WITH_EIP123
#include "dmares_mgmt.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "eip123.h"
#endif /* SBHYBRID_WITH_EIP123 */
#ifdef SBHYBRID_WITH_EIP130
#include "dmares_mgmt.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "eip130.h"
#endif /* SBHYBRID_WITH_EIP130 */

#ifdef SBHYBRID_WITH_EIP28
/*----------------------------------------------------------------------------
 * SBHYBRID_Initialize_HW_EIP28
 */
static int
SBHYBRID_Initialize_HW_EIP28(SBHYBRID_EcdsaContext_t * const Context_p)
{
    EIP28_Status_t res28;

    L_DEBUG(LF_SBHYBRID, "EIP28 initialization");

    // EIP-28 Initialization
    Context_p->Device_EIP28 = Device_Find(SBLIB_CFG_PKA_DEVICE);
    if (Context_p->Device_EIP28 == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: PKA Device (%s) not found.",
                SBLIB_CFG_PKA_DEVICE);
        return -1;
    }

    // Initialize the driver library
    // Note this also checks if it is really talking to an EIP-28
    res28 = EIP28_Initialize_CALLATOMIC(&Context_p->EIP28_IOArea,
                                        Context_p->Device_EIP28,
                                        NULL/*Firmware:*/,
                                        0);
    if (res28 != EIP28_STATUS_OK)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: EIP28_Initialize failed (%d)",
                (int)res28);
        return -2;
    }

    return 0;                           // 0 = success
}
#endif /* SBHYBRID_WITH_EIP28 */

#ifdef SBHYBRID_WITH_EIP93
/*----------------------------------------------------------------------------
 * SBHYBRID_Init_AllocDMABuf
 *
 * This function allocates a DMA-safe buffer of requested length.
 * The handle, EIP-93 DMA address and pointer are returned.
 *
 * Return Value:
 *     0    Success
 *     -1   Error
 */
static int
SBHYBRID_Init_AllocDMABuf(const unsigned int Size,
                          DMAResource_Handle_t * const DMAHandle_p, // out
                          uint32_t * const PhysAddr_p, // out
                          uint8_t ** const Host_pp) // out
{
    // allocate buffers for the DMA buffer descriptor chains
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    int res;

    Props.Size = Size;
    Props.Alignment = 4;

    res = DMAResource_Alloc(Props, &AddrPair, DMAHandle_p);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Alloc failed (%d)", res);
        return -1;
    }

    if (Host_pp != NULL)
    {
        *Host_pp = (uint8_t *)AddrPair.Address.Native_p;
    }

    res = DMAResource_Translate(*DMAHandle_p, DMARES_DOMAIN_EIP93DMA, &AddrPair);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Translate failed (%d)", res);
        return -1;
    }

    *PhysAddr_p = AddrPair.Address.Value32;

    return 0;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_Initialize_HW_EIP93
 */
static int
SBHYBRID_Initialize_HW_EIP93(SBHYBRID_SymmContext_t * const Context_p)
{
    EIP93_ARM_Settings_t Settings = { 0, 0, 0, 0, 0, false, 0 };
    EIP93_ARM_RingMemory_t RingMemory = { false, 0, { 0, 0 }, 0, { 0, 0 }, 0 };
    EIP93_Status_t res93;
    int res;

    L_DEBUG(LF_SBHYBRID, "EIP93 initialization");

    // Initialize DMA handles
    Context_p->DMAHandles.Count    = 0;
    Context_p->SA_States[0].Handle = NULL;

    // EIP-93 Initialization
    Context_p->Device_EIP93 = Device_Find(SBLIB_CFG_PE_DEVICE);
    if (Context_p->Device_EIP93 == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: PE Device (%s) not found.",
                SBLIB_CFG_PE_DEVICE);
        return -1;
    }

    res93 = EIP93_Initialize(&Context_p->EIP93_IOArea, Context_p->Device_EIP93);
    if (res93 != EIP93_STATUS_OK)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: EIP93_Initialize failed (%d)",
                (int)res93);
        return -2;
    }

    // check the AES and SHA-2 functionality is present
    // (this also ensures long SA format is expected)
    {
        // Read PE_OPTIONS_1
        uint32_t V32 = Device_Read32(Context_p->Device_EIP93, 0x1F4);

        /* Requires SHA-256, SHA-224, AES, AES-128, AES-192, AES-256. */
        if ((V32 | (BIT_19 | BIT_18 | BIT_13 | BIT_14 | BIT_15 | BIT_2)) != V32)
        {
            L_DEBUG(LF_SBHYBRID, "No AES or SHA-2 in PE_OPTIONS_1 (0x%x)", V32);
            return -3;
        }
    }

    {
        // each descriptor is 8 words (32 bytes)
        // There are at least SBHYBRID_MAX_DESCRIPTORS_PER_VECTOR per operation
        // added to the descriptor list. There can be two types of
        // operations done at once and image attributes can be added as well.
        // In addition the Ring handling may need 1 extra entry.
        unsigned int RingSizeInBytes = (SBHYBRID_MAX_DESCRIPTORS_PER_VECTOR + 2) * 2 * 32;
        uint8_t * Host_p;

        res = SBHYBRID_Init_AllocDMABuf(RingSizeInBytes,
                                        &RingMemory.CommandRingHandle,
                                        &RingMemory.CommandRingAddr.Addr,
                                        &Host_p);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID,
                    "ERROR: RingMemory allocate failed (%d)",
                    res);
            return -4;
        }

        Context_p->DMAHandles.Handles[Context_p->DMAHandles.Count++] = RingMemory.CommandRingHandle;

        // bytes to words conversion
        RingMemory.RingSizeInWords = RingSizeInBytes >> 2;
    }

    // create the engine settings block
    Settings.nPEInputThreshold = 128;
    Settings.nPEOutputThreshold = 128;
    Settings.nDescriptorSize = 8;

    res93 = EIP93_ARM_Activate(&Context_p->EIP93_IOArea,
                               &Settings,
                               &RingMemory);
    if (res93 != EIP93_STATUS_OK)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: EIP93_ARM_Activate failed (%d)",
                (int)res93);

        // free the ring memory blocks
        return -5;
    }

    // allocate the SA buffer needed for the SHA-224/SHA-256 operations
    {
        uint8_t * Host_p;

        res = SBHYBRID_Init_AllocDMABuf(SBHYBRID_SASTATE_BYTES * 2,
                                        &Context_p->SA_States[0].Handle,
                                        &Context_p->SA_States[0].SA_PhysAddr,
                                        &Host_p);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID, "ERROR: SA buffer allocate failed (%d)", res);
            return -6;
        }

        Context_p->SA_States[0].SA_Host_p = (uint32_t *)Host_p;

        // State follows 32 words SA in the same memory
        Context_p->SA_States[0].State_Host_p   = (uint32_t *)(Host_p + (32 * 4));
        Context_p->SA_States[0].State_PhysAddr = Context_p->SA_States[0].SA_PhysAddr + (32 * 4);

        // Set up second SA_State, just after the first state
        Context_p->SA_States[1].Handle         = Context_p->SA_States[0].Handle;
        Context_p->SA_States[1].SA_Host_p      = Context_p->SA_States[0].SA_Host_p + SBHYBRID_SASTATE_WORDS;
        Context_p->SA_States[1].State_Host_p   = Context_p->SA_States[0].State_Host_p + SBHYBRID_SASTATE_WORDS;
        Context_p->SA_States[1].SA_PhysAddr    = Context_p->SA_States[0].SA_PhysAddr + SBHYBRID_SASTATE_BYTES;
        Context_p->SA_States[1].State_PhysAddr = Context_p->SA_States[0].State_PhysAddr + SBHYBRID_SASTATE_BYTES;
    }

    Context_p->PendingCount = 0;

    L_TRACE(LF_SBHYBRID, "EIP93 in ARM mode");

    return 0;                           // 0 = success
}

/*----------------------------------------------------------------------------
 * SBHYBRID_Uninitialize_HW_EIP93
 *
 * This function frees resources allocated by SBHYBRID_Initialize_HW_EIP93.
 */
static void
SBHYBRID_Uninitialize_HW_EIP93(SBHYBRID_SymmContext_t * const Context_p)
{
    if (Context_p->Device_EIP93 != NULL)
    {
        // free all DMA handles
        if (Context_p->SA_States[0].Handle != NULL)
        {
            int i;

            // Erase everything written to SA and state.
            for (i = 0; i < 14 * 2; i++)
            {
                DMAResource_Write32(Context_p->SA_States[0].Handle, i, 0);
            }

            DMAResource_Release(Context_p->SA_States[0].Handle);
        }

        while (Context_p->DMAHandles.Count > 0)
        {
            Context_p->DMAHandles.Count--;
            DMAResource_Release(Context_p->DMAHandles.Handles[Context_p->DMAHandles.Count]);
        }

        EIP93_Shutdown(&Context_p->EIP93_IOArea);
        Context_p->Device_EIP93 = NULL;
    }
}
#endif /* SBHYBRID_WITH_EIP93 */

#ifdef SBHYBRID_WITH_EIP123
/*----------------------------------------------------------------------------
 * SBHYBRID_Init_AllocDMABuf
 *
 * This function allocates a DMA-safe buffer of requested length.
 * The handle, physical DMA address and pointer are returned.
 *
 * Return Value:
 *     0    Success
 *     -1   Error
 */
static int
SBHYBRID_Init_AllocDMABuf(const unsigned int Size,
                          DMAResource_Handle_t * const DMAHandle_p,
                          uint32_t * const PhysAddr_p,
                          uint8_t ** Host_pp)
{
    // allocate buffers for the DMA buffer descriptor chains
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    int res;

    Props.Size = Size;
    Props.Alignment = 4;

    res = DMAResource_Alloc(Props, &AddrPair, DMAHandle_p);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Alloc failed (%d)", res);
        return -1;
    }

    if (Host_pp != NULL)
    {
        *Host_pp = (uint8_t *)AddrPair.Address.Native_p;
    }

    res = DMAResource_Translate(*DMAHandle_p, DMARES_DOMAIN_EIP12xDMA, &AddrPair);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: Translate failed (%d)", res);
        return -1;
    }

    *PhysAddr_p = AddrPair.Address.Value32;

    L_TRACE(LF_SBHYBRID, "AllocDMABuf: dmahandle=%p phys=%u size=%u",
            *DMAHandle_p,
            (unsigned int)*PhysAddr_p,
            (unsigned int)Size);

    return 0;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_Init_AllocDC
 *
 * This function allocates a DMA-safe buffer for use as a Descriptor Chain.
 * The handle and physical DMA address are returned. The host pointer is not
 * kept as access to the DC will be through DMAResource_Read/Write32 via the
 * Driver Library.
 *
 * Return Value:
 *     0    Success
 *     -1   Error
 */
static int
SBHYBRID_Init_AllocDC(DMAResource_Handle_t * const DMAHandle_p,
                      uint32_t * const PhysAddr_p)
{
    return SBHYBRID_Init_AllocDMABuf(EIP123_Get_DC_DMAResource_Size(),
                                     DMAHandle_p, PhysAddr_p, NULL);
}

/*----------------------------------------------------------------------------
 * SBHYBRID_Initialize_HW_EIP123
 */
static int
SBHYBRID_Initialize_HW_EIP123(SBHYBRID_SymmContext_t * const Context_p)
{
    uint8_t mailboxNr = SBLIB_CFG_XM_MAILBOXNR;
    int res;

    L_DEBUG(LF_SBHYBRID, "EIP123 initialization");

    // Initialize DMA handles
    Context_p->DMAHandleCount = 0;
    Context_p->DC_Hash.DMAHandle = NULL;
    Context_p->DC_CipherIn.DMAHandle = NULL;
    Context_p->DC_CipherOut.DMAHandle = NULL;
    Context_p->SmallDMABuf.DMAHandle = NULL;

    // EIP123 Initialisation
    Context_p->Device_EIP123 = Device_Find(SBLIB_CFG_CM_DEVICE);
    if (Context_p->Device_EIP123 == NULL)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: CM Device (%s) not found.",
                SBLIB_CFG_CM_DEVICE);
        return -1;
    }

    // link the mailbox we want to use for requests from this host
    // Note: After each reset, the Master CPU must allow this explicitly.
    //       Please refer to EIP123_MailboxAccessControl in eip130.h
    //       and LOCKOUT register in SafeXcel-IP-123 HW Reference Manual
    res = EIP123_Link(Context_p->Device_EIP123, mailboxNr);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: Mailbox %u link failed (%d)",
                SBLIB_CFG_XM_MAILBOXNR, res);
        return -2;
    }
    L_DEBUG(LF_SBHYBRID, "Linked mailbox %d", mailboxNr);

    res = SBHYBRID_Init_AllocDC(&Context_p->DC_Hash.DMAHandle,
                                &Context_p->DC_Hash.PhysAddr);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: DC Hash allocate failed");
        return -3;
    }

    res = SBHYBRID_Init_AllocDC(&Context_p->DC_CipherIn.DMAHandle,
                                &Context_p->DC_CipherIn.PhysAddr);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: DC CipherIn allocate failed");
        return -4;
    }

    res = SBHYBRID_Init_AllocDC(&Context_p->DC_CipherOut.DMAHandle,
                                &Context_p->DC_CipherOut.PhysAddr);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: DC CipherOut allocate failed");
        return -5;
    }

    res = SBHYBRID_Init_AllocDMABuf(64 /* sufficient size for now */,
                                    &Context_p->SmallDMABuf.DMAHandle,
                                    &Context_p->SmallDMABuf.PhysAddr,
                                    &Context_p->SmallDMABuf.Host_p);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: DC Small allocate failed");
        return -6;
    }

    res = EIP123_VerifyDeviceComms(Context_p->Device_EIP123, mailboxNr);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: Mailbox %u communication failed (%d)",
                SBLIB_CFG_XM_MAILBOXNR, res);
        return -7;
    }

    return 0;                           // 0 = success
}

/*----------------------------------------------------------------------------
 * SBHYBRID_Uninitialize_HW_EIP123
 *
 * This function frees resources allocated by SBHYBRID_Initialize_HW_EIP123.
 */
static void
SBHYBRID_Uninitialize_HW_EIP123(SBHYBRID_SymmContext_t * const Context_p)
{
    if (Context_p->Device_EIP123 != NULL)
    {
        int res;

        DMAResource_Release(Context_p->DC_Hash.DMAHandle);
        DMAResource_Release(Context_p->DC_CipherIn.DMAHandle);
        DMAResource_Release(Context_p->DC_CipherOut.DMAHandle);
        DMAResource_Release(Context_p->SmallDMABuf.DMAHandle);

#ifdef SBHYBRID_DO_DMARESOURCE_RELEASE
        while (Context_p->DMAHandleCount > 0)
        {
            Context_p->DMAHandleCount--;
            DMAResource_Release(Context_p->DMAHandles[Context_p->DMAHandleCount]);
        }
#endif

        res = EIP123_Unlink(Context_p->Device_EIP123, SBLIB_CFG_XM_MAILBOXNR);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID,
                    "ERROR: Mailbox %u unlink failed (%d)",
                    SBLIB_CFG_XM_MAILBOXNR, res);
        }

        Context_p->Device_EIP123 = NULL;
    }
}
#endif /* SBHYBRID_WITH_EIP123 */

#ifdef SBHYBRID_WITH_EIP130
/*----------------------------------------------------------------------------
 * SBHYBRID_Initialize_HW_EIP130
 */
static int
SBHYBRID_Initialize_HW_EIP130(SBHYBRID_SymmContext_t * const Context_p)
{
    if (Context_p->Device_EIP130 == NULL)
    {
        int res;

        L_DEBUG(LF_SBHYBRID, "EIP130 initialization");

        Context_p->InitCount = 0;
        Context_p->TokenId   = 0;

        // Initialize DMA handles
        Context_p->DMAHandleCount = 0;

        // Initialize Asset Ids
        Context_p->HashTempAssetId = 0;
        Context_p->KeyAssetId      = 0;
        Context_p->ParamsAssetId   = 0;

        // EIP130 Initialisation
        Context_p->Device_EIP130 = Device_Find(SBLIB_CFG_SM_DEVICE);
        if (Context_p->Device_EIP130 == NULL)
        {
            L_DEBUG(LF_SBHYBRID,
                    "ERROR: SM Device (%s) not found.",
                    SBLIB_CFG_SM_DEVICE);
            return -1;
        }

        // link the mailbox we want to use for requests from this host
        // Note: After each reset, the Master CPU must allow this explicitly.
        //       Please refer to EIP130_MailboxAccessControl in eip130.h
        //       and LOCKOUT register in SafeXcel-IP-130 HW Reference Manual
        res = EIP130_MailboxLink(Context_p->Device_EIP130, SBLIB_CFG_XM_MAILBOXNR);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID,
                    "ERROR: Mailbox %u link failed (%d)",
                    SBLIB_CFG_XM_MAILBOXNR, res);
            return -2;
        }
        L_DEBUG(LF_SBHYBRID, "Linked mailbox %u", SBLIB_CFG_XM_MAILBOXNR);

        res = EIP130_MailboxAccessVerify(Context_p->Device_EIP130,
                                         SBLIB_CFG_XM_MAILBOXNR);
        if (res < 0)
        {
            L_DEBUG(LF_SBHYBRID,
                    "ERROR: Mailbox %u communication failed (%d)",
                    SBLIB_CFG_XM_MAILBOXNR, res);
            return -3;
        }
    }
    else
    {
        L_DEBUG(LF_SBHYBRID, "EIP130 initialization Skipped");
        Context_p->InitCount++;
    }

    return 0;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_Uninitialize_HW_EIP130
 *
 * This function frees resources allocated by SBHYBRID_Initialize_HW_EIP130.
 */
static void
SBHYBRID_Uninitialize_HW_EIP130(SBHYBRID_SymmContext_t * const Context_p)
{
    if (Context_p->Device_EIP130 != NULL)
    {
        if (Context_p->InitCount == 0)
        {
            int res = EIP130_MailboxUnlink(Context_p->Device_EIP130,
                                           SBLIB_CFG_XM_MAILBOXNR);
            if (res < 0)
            {
                L_DEBUG(LF_SBHYBRID,
                        "ERROR: Mailbox %u unlink failed (%d)",
                        SBLIB_CFG_XM_MAILBOXNR, res);
            }

#ifdef SBHYBRID_DO_DMARESOURCE_RELEASE
            SBHYBRID_EIP130_Common_DmaRelease(Context_p);
#endif

            Context_p->Device_EIP130 = NULL;
        }
        else
        {
            Context_p->InitCount--;
        }
    }
}
#endif /* SBHYBRID_WITH_EIP130 */

/*----------------------------------------------------------------------------
 * SBHYBRID_Initialize_HW
 */
SB_Result_t
SBHYBRID_Initialize_HW(SBHYBRID_Context_t * const Context_p)
{
#if defined(SBHYBRID_WITH_EIP28) || defined(SBHYBRID_WITH_EIP93) || defined(SBHYBRID_WITH_EIP123) || defined(SBHYBRID_WITH_EIP130)
    int res;
#endif

//    L_DEBUG(LF_SBHYBRID, "Initializing hardware.");

#if defined(SBHYBRID_WITH_EIP28) || defined(SBHYBRID_WITH_EIP93) || defined(SBHYBRID_WITH_EIP123) || defined(SBHYBRID_WITH_EIP130)
    // Initialize Driver Framework
    res = Device_Initialize(NULL);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID,
                "ERROR: Driver Framework initialization failed (%d)",
                res);
        return SB_ERROR_HARDWARE;
    }
#endif

#ifdef SBHYBRID_WITH_SW
    /* No initialization needed */
#endif /* SBHYBRID_WITH_SW */

#ifdef SBHYBRID_WITH_SWPK
    Context_p->EcdsaContext_p = &Context_p->EcdsaContext;
#endif /* SBHYBRID_WITH_SWPK */

#ifdef SBHYBRID_WITH_EIP28
    // EIP-28 Initialization
    Context_p->EcdsaContext_p = &Context_p->EcdsaContext;
    res = SBHYBRID_Initialize_HW_EIP28(Context_p->EcdsaContext_p);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: EIP28 initialization failed (%d)", res);
        return SB_ERROR_HARDWARE;
    }
#endif /* SBHYBRID_WITH_EIP28 */

#ifdef SBHYBRID_WITH_EIP93
    // EIP-93 Initialisation
    //   Setup memory for the overlapping ring and activate ARM mode
    res = SBHYBRID_Initialize_HW_EIP93(&Context_p->SymmContext);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: EIP93 initialization failed (%d)", res);
        return SB_ERROR_HARDWARE;
    }
#endif /* SBHYBRID_WITH_EIP93 */

#ifdef SBHYBRID_WITH_EIP123
    // EIP-123 Initialisation
    res = SBHYBRID_Initialize_HW_EIP123(&Context_p->SymmContext);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: EIP123 initialization failed (%d)", res);
        return SB_ERROR_HARDWARE;
    }
#endif /* SBHYBRID_WITH_EIP123 */

#ifdef SBHYBRID_WITH_EIP130
    // EIP-130 Initialisation
#ifdef SBHYBRID_WITH_EIP130PK
    Context_p->EcdsaContext_p = &Context_p->SymmContext;
#endif /* SBHYBRID_WITH_EIP130PK */
    res = SBHYBRID_Initialize_HW_EIP130(&Context_p->SymmContext);
    if (res < 0)
    {
        L_DEBUG(LF_SBHYBRID, "ERROR: EIP130 initialization failed (%d)", res);
        return SB_ERROR_HARDWARE;
    }
#endif /* SBHYBRID_WITH_EIP130 */

    return SB_SUCCESS;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_Uninitialize_HW
 *
 * This function frees resources allocated by SBHYBRID_Initialize_HW.
 */
void
SBHYBRID_Uninitialize_HW(SBHYBRID_Context_t * const Context_p)
{
//    L_DEBUG(LF_SBHYBRID, "Uninitializing hardware.");

#if defined(SBHYBRID_WITH_SW) || defined(SBHYBRID_WITH_SWPK) || defined(SBHYBRID_WITH_EIP28)
    /* No uninitialization needed */
    IDENTIFIER_NOT_USED(Context_p);
#endif /* defined(SBHYBRID_WITH_SW) || defined(SBHYBRID_WITH_SWPK) || defined(SBHYBRID_WITH_EIP28) */

#ifdef SBHYBRID_WITH_EIP93
    SBHYBRID_Uninitialize_HW_EIP93(&Context_p->SymmContext);
#endif /* SBHYBRID_WITH_EIP93 */

#ifdef SBHYBRID_WITH_EIP123
    SBHYBRID_Uninitialize_HW_EIP123(&Context_p->SymmContext);
#endif /* SBHYBRID_WITH_EIP123 */

#ifdef SBHYBRID_WITH_EIP130
    SBHYBRID_Uninitialize_HW_EIP130(&Context_p->SymmContext);
#endif /* SBHYBRID_WITH_EIP130 */
}

/* end of file sbhybrid_hw_init.c */
