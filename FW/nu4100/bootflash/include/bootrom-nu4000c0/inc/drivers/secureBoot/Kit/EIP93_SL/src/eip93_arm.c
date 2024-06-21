/* eip93_arm.c
 *
 * Driver Library for the EIP93 packet Engines:
 * ARM API implementation.
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

#include "basic_defs.h"        // uint8_t, IDENTIFIER_NOT_USED, etc.
#include "device_mgmt.h"         // HW access API
#include "dmares_mgmt.h"         // HW access API
#include "eip93_arm.h"       // the API we will implement
#include "c_eip93.h"         // configration options
#include "eip93_arm_level0.h" // macros and functions to access EIP93 reg
#include "eip93_internal.h"  // internal API


#define EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM \
    EIP93_Status_t res = EIP93_STATUS_OK; \
    EIP93_Device_t* Device_p = NULL; \
    EIP93_ARM_Mode_t* ARM_p = NULL; \
    EIP93_CHECK_POINTER(IOArea_p); \
    Device_p = (EIP93_Device_t*)IOArea_p; \
    ARM_p = &Device_p->extras.ARM_mode; \
    IDENTIFIER_NOT_USED(ARM_p);

#ifdef EIP93_STRICT_ARGS

#define EIP93_CHECK_ARM_IS_READY \
    EIP93_CHECK_POINTER(Device_p); \
    if (Device_p->CurrentMode != EIP93_MODE_ARM) \
    { \
        res = EIP93_ERROR_UNSUPPORTED_IN_THIS_STATE; \
        goto FUNC_RETURN; \
    }

#else

#define EIP93_CHECK_ARM_IS_READY

#endif //EIP93_STRICT_ARGS


/*----------------------------------------------------------------------------
 * EIP93_WriteCB
 * A write callback for the Ring Helper
 */
static int
EIP93_WriteCB(
    void * const CallbackParam1_p,
    const int CallbackParam2,
    const unsigned int WriteIndex,
    const unsigned int WriteCount,
    const unsigned int AvailableSpace,
    const void * Descriptors_p,
    const int DescriptorCount,
    const unsigned DescriptorSkipCount)
{
    unsigned int nDescrSize;
    unsigned int i;
    int nWritten = 0;
    EIP93_IOArea_t * IOArea_p = (EIP93_IOArea_t *) CallbackParam1_p;
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;
    EIP93_CHECK_POINTER(Descriptors_p);

    nDescrSize = ARM_p->Settings.nDescriptorSize;

    IDENTIFIER_NOT_USED(CallbackParam2);
    IDENTIFIER_NOT_USED(AvailableSpace);
    IDENTIFIER_NOT_USED(DescriptorCount);

    // first we write all descriptors we can to PDR
    if (ARM_p->RingHelper.fSeparate) // separate rings
    {
        for(i = WriteIndex; i < WriteIndex + WriteCount; i++)
        {
            // write without checking ownership bits
            EIP93_ARM_Level0_WriteDescriptor(
                ARM_p->CommandRingHandle,
                i * nDescrSize,
                ((const EIP93_ARM_CommandDescriptor_t *)Descriptors_p) +
                DescriptorSkipCount + nWritten);

            nWritten++;
        }
    }
    else // combined rings
    {
        for(i = WriteIndex; i < WriteIndex + WriteCount; i++)
        {
            // just always write it
            EIP93_ARM_Level0_WriteDescriptor(
                ARM_p->CommandRingHandle,
                i * nDescrSize,
                ((const EIP93_ARM_CommandDescriptor_t *)Descriptors_p) +
                DescriptorSkipCount + nWritten);
            nWritten++;
        }
    }

    // now we call PreDMA to provide descriptors written for
    // the EIP93 DMA Master
    if (nWritten > 0)
    {
        DMAResource_PreDMA(ARM_p->CommandRingHandle,
                           WriteIndex * nDescrSize * 4,
                           nWritten * nDescrSize * 4);

        // now we have to kick the device by writing to Force Read register
        EIP93_Write32_PE_CD_COUNT(Device_p->Device, (uint32_t)nWritten);
    }

    goto FUNC_RETURN;
FUNC_RETURN:
    if (res)
    {
        return -res;
    }
    else
    {
        return nWritten;
    }
}


/*----------------------------------------------------------------------------
 * EIP93_ReadCB
 * A read callback for the Ring Helper
 */
static int
EIP93_ReadCB(
    void * const CallbackParam1_p,
    const int CallbackParam2,
    const unsigned int ReadIndex,
    const unsigned int ReadLimit,
    void * Descriptors_p,
    const unsigned int DescriptorSkipCount)
{
    unsigned int nDescrSize;
    unsigned int i;
    int nRead = 0;
    EIP93_IOArea_t * const IOArea_p = (EIP93_IOArea_t *) CallbackParam1_p;
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;
    EIP93_CHECK_POINTER(Descriptors_p);

    IDENTIFIER_NOT_USED(CallbackParam2);

    nDescrSize = ARM_p->Settings.nDescriptorSize;

    // now we read all descriptors we can from RDR

    for(i = ReadIndex; i < ReadIndex + ReadLimit; i++)
    {
        EIP93_ARM_ResultDescriptor_t * CurrentResultDesc_p =
            ((EIP93_ARM_ResultDescriptor_t *)Descriptors_p) +
            DescriptorSkipCount + nRead;

        // first we call PostDMA to obtain descriptors to be read from
        // the EIP93 DMA Master
        DMAResource_PostDMA(ARM_p->ResultRingHandle,
                            i * nDescrSize * 4,
                            nDescrSize * 4);

        // read it if ready
        if (EIP93_ARM_Level0_ReadDescriptor_IfReady(CurrentResultDesc_p,
                                                    ARM_p->ResultRingHandle,
                                                    i * nDescrSize))
        {
            // just clear this descriptor
            EIP93_ARM_Level0_ClearDescriptor(ARM_p->ResultRingHandle,
                                             i * nDescrSize);

            // make sure our next "PostDMA" does not undo this
            DMAResource_PreDMA(ARM_p->ResultRingHandle,
                               i * nDescrSize * 4,
                               4);

#ifdef EIP93_ARM_NUM_OF_DESC_PADDING_WORDS
            CurrentResultDesc_p->fPaddingWordValuesAreValid =
                !ARM_p->RingHelper.fSeparate;
#endif //EIP93_ARM_NUM_OF_DESC_PADDING_WORDS

            nRead++;
        }
        else
        {
            break;
        }
    }

    if (nRead > 0)
    {
        EIP93_Write32_PE_RD_COUNT(Device_p->Device, (uint32_t)nRead);
    }

    goto FUNC_RETURN;
FUNC_RETURN:
    if (res)
    {
        return -res;
    }
    else
    {
        return nRead;
    }
}


/*----------------------------------------------------------------------------
 * EIP93_StatusCB
 * A status callback for the Ring Helper
 */
static int
EIP93_StatusCB(
    void * const CallbackParam1_p,
    const int CallbackParam2,
    int * const DeviceReadPos_p)
{
    uint16_t CmdIndex;
    uint16_t ResIndex;

    EIP93_IOArea_t * IOArea_p = (EIP93_IOArea_t *) CallbackParam1_p;
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;
    IDENTIFIER_NOT_USED(CallbackParam2);

    EIP93_Read32_PE_RING_PNTR(Device_p->Device,
                              (uint16_t *)&CmdIndex,
                              (uint16_t *)&ResIndex);
    *DeviceReadPos_p = (int)CmdIndex;

    IDENTIFIER_NOT_USED(res);

    goto FUNC_RETURN;
FUNC_RETURN:
    return 0;
}


/*----------------------------------------------------------------------------
 * EIP93_ARM_Activate
 *
 *  See header file for function specification.
 */
EIP93_Status_t
EIP93_ARM_Activate(
    EIP93_IOArea_t * const IOArea_p,
    const EIP93_ARM_Settings_t * const Settings_p,
    const EIP93_ARM_RingMemory_t * const Ring_p)
{
    unsigned int RingSizeInDescr = 0;

    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_DEVICE_IS_READY;
    EIP93_CHECK_POINTER(Settings_p);
    EIP93_CHECK_POINTER(Ring_p);

    EIP93_CHECK_INT_ATMOST(Settings_p->nDescriptorDoneCount,
                           EIP93_MAX_DESC_DONE_COUNT);

    EIP93_CHECK_INT_ATMOST(Settings_p->nDescriptorPendingCount,
                           EIP93_MAX_DESC_PENDING_COUNT);

    EIP93_CHECK_INT_ATMOST(Settings_p->nDescriptorDoneTimeout,
                           EIP93_MAX_TIMEOUT_COUNT);

    EIP93_CHECK_INT_INRANGE(Settings_p->nPEInputThreshold,
                            EIP93_MIN_PE_INPUT_THRESHOLD,
                            EIP93_MAX_PE_INPUT_THRESHOLD);

    EIP93_CHECK_INT_INRANGE(Settings_p->nPEOutputThreshold,
                            EIP93_MIN_PE_OUTPUT_THRESHOLD,
                            EIP93_MAX_PE_OUTPUT_THRESHOLD);

    EIP93_CHECK_INT_INRANGE(Settings_p->nDescriptorSize,
                            EIP93_MIN_PE_DESCRIPTOR_SIZE,
                            EIP93_MAX_PE_DESCRIPTOR_SIZE);

    // first we configure the Ring Helper

    ARM_p->RingHelperCallbacks.WriteFunc_p = &EIP93_WriteCB;
    ARM_p->RingHelperCallbacks.ReadFunc_p = &EIP93_ReadCB;
    ARM_p->RingHelperCallbacks.StatusFunc_p = &EIP93_StatusCB;
    ARM_p->RingHelperCallbacks.CallbackParam1_p = IOArea_p;
    ARM_p->RingHelperCallbacks.CallbackParam2 = 0;

    RingSizeInDescr = Ring_p->RingSizeInWords / Settings_p->nDescriptorSize;

    EIP93_CHECK_INT_INRANGE(RingSizeInDescr,
                            EIP93_MIN_PE_RING_SIZE,
                            EIP93_MAX_PE_RING_SIZE);

    RingHelper_Init(&ARM_p->RingHelper,
                    &ARM_p->RingHelperCallbacks,
                    Ring_p->fSeparateRings,
                    RingSizeInDescr,
                    RingSizeInDescr);

    // now we initialize the EIP PE and ring registers

    ARM_p->CommandRingHandle = Ring_p->CommandRingHandle;
    EIP93_Write32_PE_CDR_BASE(Device_p->Device,
                              Ring_p->CommandRingAddr.Addr);
    if(Ring_p->fSeparateRings)
    {
        ARM_p->ResultRingHandle = Ring_p->ResultRingHandle;
        EIP93_Write32_PE_RDR_BASE(Device_p->Device,
                                  Ring_p->ResultRingAddr.Addr);
    }
    else
    {
        ARM_p->ResultRingHandle = Ring_p->CommandRingHandle;
        EIP93_Write32_PE_RDR_BASE(Device_p->Device,
                                  Ring_p->CommandRingAddr.Addr);
    }

    EIP93_Write32_PE_RING_SIZE(Device_p->Device,
                               (uint16_t)RingSizeInDescr,
                               false);

    EIP93_Write32_PE_RING_THRESH(Device_p->Device,
                                 (uint16_t)Settings_p->nDescriptorDoneCount,
                                 (uint16_t)Settings_p->nDescriptorPendingCount,
                                 (uint16_t)Settings_p->nDescriptorDoneTimeout,
                                 Settings_p->fTimeoutEnable);

    EIP93_Write32_PE_IO_THRESHOLD(Device_p->Device,
                                  (uint16_t)Settings_p->nPEInputThreshold,
                                  (uint16_t)Settings_p->nPEOutputThreshold);

    // prepare the ring buffers

    // Initialize all descriptors with zero for command ring
    EIP93_ARM_Level0_ClearAllDescriptors(ARM_p->CommandRingHandle,
                                         Settings_p->nDescriptorSize,
                                         RingSizeInDescr);

    // Call PreDMA to make sure engine sees it
    DMAResource_PreDMA(ARM_p->CommandRingHandle,
                       0,
                       Ring_p->RingSizeInWords * 4);

    if(Ring_p->fSeparateRings)
    {
        EIP93_ARM_Level0_ClearAllDescriptors(ARM_p->ResultRingHandle,
                                             Settings_p->nDescriptorSize,
                                             RingSizeInDescr);

        // we do PreDMA for the whole RDR buffer, to make sure
        // the EIP93 DMA Master gets the full control over the buffer
        // (for instance, dirty cache lines are flushed now,
        // so they will not overwrite possible new result descriptors
        // written by EIP93 Packet Engine later)
        DMAResource_PreDMA(ARM_p->ResultRingHandle,
                           0,
                           Ring_p->RingSizeInWords * 4);
    }

    // Initialize the bus interface
#if defined(EIP93_BUS_VERSION_AXI)
    EIP93_Write32_AXI_DMA_CFG(Device_p->Device,
                              EIP93_BUS_MAX_BURST_SIZE,
                              EIP93_BUS_MASTER_BIGENDIAN,
                              EIP93_BUS_POSTED_TRANSFER);
    {
        uint8_t OldMasterSwap,OldTargetSwap;
        EIP93_Read32_AXI_ENDIAN_CFG(Device_p->Device,
                                    &OldMasterSwap,
                                    &OldTargetSwap);
        EIP93_Write32_AXI_ENDIAN_CFG(Device_p->Device,
                                     EIP93_BUS_MASTER_BYTE_SWAP,
                                     OldTargetSwap);
    }
#else
    {
        uint8_t OldMasterSwap,OldTargetSwap;
        EIP93_Read32_AHB_ENDIAN_CFG(Device_p->Device,
                                    &OldMasterSwap,
                                    &OldTargetSwap);
        EIP93_Write32_AHB_ENDIAN_CFG(Device_p->Device,
                                     EIP93_BUS_MASTER_BYTE_SWAP,
                                     OldTargetSwap);
    }
    EIP93_Write32_AHB_DMA_CFG(Device_p->Device,
                              EIP93_BUS_MAX_BURST_SIZE,
                              EIP93_BUS_MASTER_BIGENDIAN,
                              EIP93_BUS_LOCK_ENABLE,
                              EIP93_BUS_INCR_ENABLE,
                              EIP93_BUS_IDLE_ENABLE);
#endif

    // Initizalize and start up the PE
    EIP93_Write32_PE_CFG(Device_p->Device,
                         0,             // Reset PE: no
                         0,             // Reset Ring: no
                         3,             // ARM mode on
                         EIP93_ENABLE_SWAP_CD_RD,
                         EIP93_ENABLE_SWAP_SA,
                         EIP93_ENABLE_SWAP_DATA,
                         0);            // CDR Update is off

    // now PE is running and we are ready to accept command descriptors
    // and process packet data

    ARM_p->Settings = *Settings_p;

    Device_p->CurrentMode = EIP93_MODE_ARM;

    EIP93_INSERTCODE_FUNCTION_EXIT_CODE;
}


/*----------------------------------------------------------------------------
 * EIP93_ARM_FinalizeSA
 *
 *  See header file for function specification.
 */
EIP93_Status_t
EIP93_ARM_FinalizeSA(
    EIP93_IOArea_t * const IOArea_p,
    const DMAResource_Handle_t SADataHandle)
{
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;
    EIP93_CHECK_HANDLE(SADataHandle);

    EIP93_INSERTCODE_FUNCTION_EXIT_CODE;
}



/*----------------------------------------------------------------------------
 * EIP93_ARM_PacketPut
 *
 *  See header file for function specification.
 */
EIP93_Status_t
EIP93_ARM_PacketPut(
    EIP93_IOArea_t * const IOArea_p,
    const EIP93_ARM_CommandDescriptor_t *  CmdDescriptors_p,
    const unsigned int CmdDescriptorCount,
    unsigned int * const DoneCount_p)
{
    uint32_t CDtodoCnt = 0;
    bool ExternalTrigger;
    uint16_t RingSizeInDescriptor = 0;
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;
    EIP93_CHECK_POINTER(CmdDescriptors_p);
    EIP93_CHECK_POINTER(DoneCount_p);

    EIP93_Read32_PE_CD_COUNT(Device_p->Device, &CDtodoCnt);
    EIP93_Read32_PE_RING_SIZE(Device_p->Device,
                              &RingSizeInDescriptor,
                              &ExternalTrigger);

    if(CDtodoCnt < (uint32_t)RingSizeInDescriptor)
    {
        *DoneCount_p = RingHelper_Put(&ARM_p->RingHelper,
                                      CmdDescriptors_p,
                                      CmdDescriptorCount);
    }
    else
    {
        *DoneCount_p = 0;
    }
    EIP93_INSERTCODE_FUNCTION_EXIT_CODE;
}


/*----------------------------------------------------------------------------
 * EIP93_ARM_PacketGet
 *
 *  See header file for function specification.
 */
EIP93_Status_t
EIP93_ARM_PacketGet(
    EIP93_IOArea_t * const IOArea_p,
    EIP93_ARM_ResultDescriptor_t *  ResDescriptors_p,
    const unsigned int ResDescriptorLimit,
    unsigned int * const DoneCount_p)
{
    uint32_t DoneCnt = 0;
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;
    EIP93_CHECK_POINTER(ResDescriptors_p);
    EIP93_CHECK_POINTER(DoneCount_p);

    EIP93_Read32_PE_RD_COUNT(Device_p->Device, &DoneCnt);
    if(DoneCnt)
    {
        *DoneCount_p = RingHelper_Get(&ARM_p->RingHelper,
                                      -1, /* ReadyCount we don't know */
                                      ResDescriptors_p,
                                      ResDescriptorLimit);
    }
    else
    {
        *DoneCount_p = 0;
    }
    EIP93_INSERTCODE_FUNCTION_EXIT_CODE;
}


/*----------------------------------------------------------------------------
 * EIP93_ARM_PacketExternalGet
 *
 *  See header file for function specification.
 */
EIP93_Status_t
EIP93_ARM_PacketExternalGet(
    EIP93_IOArea_t * const IOArea_p,
    const unsigned int ExternalDoneCount)
{
    EIP93_INSERTCODE_FUNCTION_ENTRY_CODE_ARM;
    EIP93_CHECK_ARM_IS_READY;

#ifdef EIP93_USE_RING_NOTIFY
    RingHelper_Notify(&ARM_p->RingHelper, ExternalDoneCount);
#else
    IDENTIFIER_NOT_USED(ExternalDoneCount);
#endif

    EIP93_INSERTCODE_FUNCTION_EXIT_CODE;
}


/* end of file eip93_arm.c */
