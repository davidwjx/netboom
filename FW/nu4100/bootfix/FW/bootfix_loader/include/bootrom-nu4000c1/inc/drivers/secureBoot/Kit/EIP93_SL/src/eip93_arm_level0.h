/* eip93_arm_level0.h
 *
 * This file contains all the macros and  functions that allow
 * access to the EIP93 registers and to build the values
 * read or written to the registers.
 *
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

#ifndef INCLUDE_GUARD_EIP93_ARM_LEVEL0_H
#define INCLUDE_GUARD_EIP93_ARM_LEVEL0_H

#include "basic_defs.h"             // BIT definitions, bool, uint32_t
#include "device_rw.h"              // Read32, Write32, Device_Handle_t
#include "dmares_rw.h"          // Read32, Write32, DMAResource_t
#include "eip93_level0.h"         // the generic level0 functions
#include "eip93_hw_interface.h"   // the HW interface (register map)

/*-----------------------------------------------------------------------------
 * EIP93 register routines
 *
 * These routines write/read register values in EIP93 registers
 * in HW specific format.
 *
 * Note: if a function argument implies a flag ('f' is a prefix),
 *       then only the values 0 or 1 are allowed for this argument.
 */

static inline void
EIP93_Write32_PE_CDR_BASE(
    Device_Handle_t Device,
    const uint32_t CDRBaseAddress)
{
    EIP93_Write32(Device, EIP93_REG_PE_CDR_BASE, CDRBaseAddress);
}


static inline void
EIP93_Read32_PE_CDR_BASE(
    Device_Handle_t Device,
    uint32_t * const CDRBaseAddress)
{
    if (CDRBaseAddress)
    {
        *CDRBaseAddress = EIP93_Read32(Device, EIP93_REG_PE_CDR_BASE);
    }
}


static inline void
EIP93_Write32_PE_RDR_BASE(
    Device_Handle_t Device,
    const uint32_t RDRBaseAddress)
{
    EIP93_Write32(Device, EIP93_REG_PE_RDR_BASE, RDRBaseAddress);
}


static inline void
EIP93_Read32_PE_RDR_BASE(
    Device_Handle_t Device,
    uint32_t * const RDRBaseAddress)
{
    if (RDRBaseAddress)
    {
        *RDRBaseAddress = EIP93_Read32(Device, EIP93_REG_PE_RDR_BASE);
    }
}

static inline void
EIP93_Write32_PE_RING_SIZE(
    Device_Handle_t Device,
    const uint16_t RingSize,
    const bool fExternalTrigger)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_RING_CFG,
                  ((RingSize - 1) & MASK_10_BITS) |
                  (fExternalTrigger ? BIT_31 : 0));
}


static inline void
EIP93_Read32_PE_RING_SIZE(
    Device_Handle_t Device,
    uint16_t * const RingSize,
    bool * const fExternalTrigger)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_RING_CFG);
    if (RingSize)
    {
         *RingSize = (word & MASK_10_BITS) + 1;
    }
    if (fExternalTrigger)
    {
         *fExternalTrigger = (word >> 31) & MASK_1_BIT;
    }
}


static inline void
EIP93_Write32_PE_RING_THRESH(
    Device_Handle_t Device,
    const uint16_t CmdDescrThreshCnt,
    const uint16_t ResDescrThreshCnt,
    const uint16_t ResRingTimeOut,
    const bool fTimeoutEnable)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_RING_THRESH,
                  (CmdDescrThreshCnt & MASK_10_BITS) |
                  ((ResDescrThreshCnt & MASK_10_BITS) << 16) |
                  ((ResRingTimeOut  & MASK_4_BITS) << 26) |
                  (fTimeoutEnable ? BIT_31 : 0));
}


static inline void
EIP93_Read32_PE_RING_THRESH(
    Device_Handle_t Device,
    uint16_t * const CmdDescrThreshCnt_p,
    uint16_t * const ResDescrThreshCnt_p,
    uint16_t * const ResRingTimeOut_p,
    bool * const fTimeoutEnable_p)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_RING_THRESH);

    if (CmdDescrThreshCnt_p)
    {
        *CmdDescrThreshCnt_p = word  & MASK_10_BITS;
    }
    if (ResDescrThreshCnt_p)
    {
        *ResDescrThreshCnt_p = (word >> 16) & MASK_10_BITS;
    }
    if (ResRingTimeOut_p)
    {
        *ResRingTimeOut_p = (word >> 26) & MASK_6_BITS;
    }
    if (fTimeoutEnable_p)
    {
        *fTimeoutEnable_p = (word >> 31) & MASK_1_BIT;
    }
}


static inline void
EIP93_Read32_PE_CD_COUNT(
    Device_Handle_t Device,
    uint32_t * const CmdDescrCount)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_CD_COUNT);
    if (CmdDescrCount)
    {
        *CmdDescrCount = word & MASK_10_BITS;
    }
}

static inline void
EIP93_Write32_PE_CD_COUNT(
    Device_Handle_t Device,
    const uint32_t CmdDescrCount)
{
    EIP93_Write32(Device, EIP93_REG_PE_CD_COUNT, CmdDescrCount & MASK_8_BITS);
}

static inline void
EIP93_Read32_PE_RD_COUNT(
    Device_Handle_t Device,
    uint32_t * const ResDescrCount)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_RD_COUNT);
    if (ResDescrCount)
    {
        *ResDescrCount = word & MASK_10_BITS;
    }
}

static inline void
EIP93_Write32_PE_RD_COUNT(
    Device_Handle_t Device,
    const uint32_t ResDescrCount)
{
    EIP93_Write32(Device, EIP93_REG_PE_RD_COUNT, ResDescrCount & MASK_8_BITS);
}


static inline void
EIP93_Read32_PE_RING_PNTR(
    Device_Handle_t Device,
    uint16_t * const NextCDROffset,
    uint16_t * const NextRDROffset)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_RING_RW_PNTR);
    if (NextCDROffset)
    {
        *NextCDROffset = word  & MASK_10_BITS;
    }
    if (NextRDROffset)
    {
        *NextRDROffset = (word >> 16) & MASK_10_BITS;
    }
}


static inline void
EIP93_Read32_PE_DMA_STAT(
    Device_Handle_t Device,
    uint8_t * const fPeInputDone,
    uint8_t * const fPeOutputDone,
    uint8_t * const fEncryptionDone,
    uint8_t * const fInnerHashDone,
    uint8_t * const fOuterHashDone,
    uint8_t * const fCryptoPadFault,
    uint8_t * const fSPIMismatch,
    uint8_t * const fEXTError,
    uint8_t * const fPeOperationDone,
    uint8_t * const fInputRequestActive,
    uint8_t * const fOutputRequestActive,
    uint16_t * const PeInputSize,
    uint16_t * const PeOutputSize)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_STATUS);
    if (fPeInputDone)
    {
        *fPeInputDone = word & MASK_1_BIT;
    }
    if (fPeOutputDone)
    {
        *fPeOutputDone = (word >> 1) & MASK_1_BIT;
    }
    if (fEncryptionDone)
    {
        *fEncryptionDone = (word >> 2) & MASK_1_BIT;
    }
    if (fInnerHashDone)
    {
        *fInnerHashDone = (word >> 3) & MASK_1_BIT;
    }
    if (fOuterHashDone)
    {
        *fOuterHashDone = (word >> 4) & MASK_1_BIT;
    }
    if (fCryptoPadFault)
    {
        *fCryptoPadFault = (word >> 5) & MASK_1_BIT;
    }
    if (fSPIMismatch)
    {
        *fSPIMismatch = (word >> 7) & MASK_1_BIT;
    }
    if (fEXTError)
    {
        *fEXTError = (word >> 8) & MASK_1_BIT;
    }
    if (fPeOperationDone)
    {
        *fPeOperationDone = (word >> 9) & MASK_1_BIT;
    }
    if (fInputRequestActive)
    {
        *fInputRequestActive = (word >> 10) & MASK_1_BIT;
    }
    if (fOutputRequestActive)
    {
        *fOutputRequestActive = (word >> 11) & MASK_1_BIT;
    }
    if (PeInputSize)
    {
        *PeInputSize = (word >> 12) & MASK_10_BITS;
    }
    if (PeOutputSize)
    {
        *PeOutputSize = (word >> 22) & MASK_10_BITS;
    }
}


static inline void
EIP93_Read32_INT_UNMASK_STAT(
    Device_Handle_t Device,
    uint8_t * const fPeCommandIRQ,
    uint8_t * const fPeResultIRQ,
    uint8_t * const fPeOpDoneIRQ,
    uint8_t * const fPeInputBufferIRQ,
    uint8_t * const fPeOutputBufferIRQ,
    uint8_t * const fPeHaltIRQ,
    uint8_t * const fPeRingErrIRQ,
    uint8_t * const fPeProcErrIRQ,
    uint8_t * const fInterfaceErrIRQ)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_INT_UNMASK_STAT);
    if (fPeCommandIRQ)
    {
        *fPeCommandIRQ = word & MASK_1_BIT;
    }
    if (fPeResultIRQ)
    {
        *fPeResultIRQ = (word >> 1) & MASK_1_BIT;
    }
    if (fPeOpDoneIRQ)
    {
        *fPeOpDoneIRQ = (word >> 9) & MASK_1_BIT;
    }
    if (fPeInputBufferIRQ)
    {
        *fPeInputBufferIRQ = (word >> 10) & MASK_1_BIT;
    }
    if (fPeOutputBufferIRQ)
    {
        *fPeOutputBufferIRQ = (word >> 11) & MASK_1_BIT;
    }
    if (fPeHaltIRQ)
    {
        *fPeHaltIRQ = (word >> 15) & MASK_1_BIT;
    }
    if (fPeRingErrIRQ)
    {
        *fPeRingErrIRQ = (word >> 16) & MASK_1_BIT;
    }
    if (fPeProcErrIRQ)
    {
        *fPeProcErrIRQ = (word >> 17) & MASK_1_BIT;
    }
    if (fInterfaceErrIRQ)
    {
        *fInterfaceErrIRQ = (word >> 18) & MASK_1_BIT;
    }
}

static inline void
EIP93_Read32_INT_MASK_STAT(
    Device_Handle_t Device,
    uint8_t * const fPeCommandIRQ,
    uint8_t * const fPeResultIRQ,
    uint8_t * const fPeOpDoneIRQ,
    uint8_t * const fPeInputBufferIRQ,
    uint8_t * const fPeOutputBufferIRQ,
    uint8_t * const fPeHaltIRQ,
    uint8_t * const fPeRingErrIRQ,
    uint8_t * const fPeProcErrIRQ,
    uint8_t * const fInterfaceErrIRQ)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_INT_MASK_STAT);
    if (fPeCommandIRQ)
    {
        *fPeCommandIRQ = word & MASK_1_BIT;
    }
    if (fPeResultIRQ)
    {
        *fPeResultIRQ = (word >> 1) & MASK_1_BIT;
    }
    if (fPeOpDoneIRQ)
    {
        *fPeOpDoneIRQ = (word >> 9) & MASK_1_BIT;
    }
    if (fPeInputBufferIRQ)
    {
        *fPeInputBufferIRQ = (word >> 10) & MASK_1_BIT;
    }
    if (fPeOutputBufferIRQ)
    {
        *fPeOutputBufferIRQ = (word >> 11) & MASK_1_BIT;
    }
    if (fPeHaltIRQ)
    {
        *fPeHaltIRQ = (word >> 15) & MASK_1_BIT;
    }
    if (fPeRingErrIRQ)
    {
        *fPeRingErrIRQ = (word >> 16) & MASK_1_BIT;
    }
    if (fPeProcErrIRQ)
    {
        *fPeProcErrIRQ = (word >> 17) & MASK_1_BIT;
    }
    if (fInterfaceErrIRQ)
    {
        *fInterfaceErrIRQ = (word >> 18) & MASK_1_BIT;
    }
}


static inline void
EIP93_Write32_INT_CLR(
    Device_Handle_t Device,
    const uint8_t fPeCommandIRQ,
    const uint8_t fPeResultIRQ,
    const uint8_t fPeOpDoneIRQ,
    const uint8_t fPeInputBufferIRQ,
    const uint8_t fPeOutputBufferIRQ,
    const uint8_t fPeHaltIRQ,
    const uint8_t fPeRingErrIRQ,
    const uint8_t fPeProcErrIRQ,
    const uint8_t fInterfaceErrIRQ)
{
    EIP93_Write32(Device,
                  EIP93_REG_INT_CLR,
                  (fPeCommandIRQ ? BIT_0 : 0) |
                  (fPeResultIRQ ? BIT_1 : 0) |
                  (fPeOpDoneIRQ ? BIT_9 : 0) |
                  (fPeInputBufferIRQ ? BIT_10 : 0) |
                  (fPeOutputBufferIRQ ? BIT_11 : 0) |
                  (fPeHaltIRQ ? BIT_15 : 0) |
                  (fPeRingErrIRQ ? BIT_16 : 0) |
                  (fPeProcErrIRQ ? BIT_17 : 0) |
                  (fInterfaceErrIRQ ? BIT_18 : 0));
}


/*-----------------------------------------------------------------------------
 * ARM routines
 *
 * These routines write/read descriptors in a descriptor ring for ARM
 */

static inline void
EIP93_ARM_Level0_WriteDescriptor(
    DMAResource_Handle_t Handle,
    const unsigned int WordOffset,
    const EIP93_ARM_CommandDescriptor_t * const Descr_p)
{
    uint32_t word;

    // Ctrl/stat word
    // mask out reserved and status fields.
    word = Descr_p->ControlWord & 0xFF00FFF8; //& 0xFF00FF10;
    word |= 1;                          // Host Ready is set
    DMAResource_Write32(Handle, WordOffset, word);

    // Source address
    DMAResource_Write32(Handle, WordOffset + 1, Descr_p->SrcPacketAddr.Addr);

    // Destination address
    DMAResource_Write32(Handle, WordOffset + 2, Descr_p->DstPacketAddr.Addr);

    // SA data address
    DMAResource_Write32(Handle, WordOffset + 3, Descr_p->SADataAddr.Addr);

    // SA State data address
    DMAResource_Write32(Handle, WordOffset + 4, Descr_p->SAStateDataAddr.Addr);


    DMAResource_Write32(Handle, WordOffset + 5, Descr_p->SAStateDataAddr.Addr);

    // User ID address
    DMAResource_Write32(Handle, WordOffset + 6, Descr_p->UserId);

    // Length word
    word = 0;
    word |= Descr_p->BypassWordLength << 24;
    word |= 1 << 22;                    // Host Ready is set
    word |= ((BIT_20 - 1) & Descr_p->SrcPacketByteCount); //can not exceed 1MB
    DMAResource_Write32(Handle, WordOffset + 7, word);

    // padding words
#ifdef EIP93_ARM_NUM_OF_DESC_PADDING_WORDS
    {
        unsigned i;
        for (i = 0; i < EIP93_ARM_NUM_OF_DESC_PADDING_WORDS; i++)
        {
            DMAResource_Write32(Handle,
                                WordOffset + i + 8,
                                Descr_p->PaddingWords[i]);
        }
    }
#endif
}


static inline bool
EIP93_ARM_Level0_WriteDescriptor_IfFree(
    DMAResource_Handle_t Handle,
    const unsigned int WordOffset,
    const EIP93_ARM_CommandDescriptor_t * const Descr_p)
{
    uint32_t word0 = 0;
    uint32_t lastword = 0;
    uint32_t pe_done1 = 0;
    uint32_t pe_done2 = 0;

    word0 = DMAResource_Read32(Handle, WordOffset);
    lastword = DMAResource_Read32(Handle, WordOffset + 7);

    pe_done1 = word0 & MASK_2_BITS;
    pe_done2 = (lastword >> 22) & MASK_2_BITS;
    if (pe_done1 == pe_done2 && pe_done1 == 0)
    {
        EIP93_ARM_Level0_WriteDescriptor(Handle, WordOffset, Descr_p);
        return true;
    }
    return false;
}


static inline void
EIP93_ARM_Level0_ClearDescriptor(
    DMAResource_Handle_t Handle,
    const unsigned int WordOffset)
{
    unsigned i = 0;

    for (i = 0; i < EIP93_ARM_DESCRIPTOR_SIZE(); i++)
    {
        DMAResource_Write32(Handle, WordOffset + i, 0);
    }
}


static inline void
EIP93_ARM_Level0_ClearAllDescriptors(
    DMAResource_Handle_t Handle,
    const unsigned int DescriptorSpacing,
    const unsigned int NumberOfDescriptors)
{
    const uint32_t Words[8] = { 0 };
    const int nDescrSize = EIP93_ARM_DESCRIPTOR_SIZE();
    unsigned int i;

    for (i = 0; i < NumberOfDescriptors; i++)
    {
        DMAResource_Write32Array(Handle,
                                 i * DescriptorSpacing,
                                 nDescrSize,
                                 Words);
    }
}


static inline bool
EIP93_ARM_Level0_ReadDescriptor_IfReady(
    EIP93_ARM_ResultDescriptor_t * const Descr_p,
    const DMAResource_Handle_t Handle,
    const unsigned int WordOffset)
{
    uint32_t word0 = 0;
    uint32_t lastword = 0;
    uint32_t pe_done1 = 0;
    uint32_t pe_done2 = 0;
    word0 = DMAResource_Read32(Handle, WordOffset);
    lastword = DMAResource_Read32(Handle, WordOffset + 7);

    pe_done1 = word0 & MASK_2_BITS;
    pe_done2 = (lastword >> 22) & MASK_2_BITS;
    if (pe_done1 == pe_done2 && pe_done1 == 2)
    {
        // Stat word
        Descr_p->StatusWord = word0;

        // Destination data length and bypass length
        Descr_p->BypassWordLength = (lastword >> 24) & MASK_8_BITS;
        Descr_p->DstPacketByteCount = lastword & MASK_20_BITS;

        // padding words
#ifdef EIP93_ARM_NUM_OF_DESC_PADDING_WORDS
        {
            unsigned i;
            for (i = 0; i < EIP93_ARM_NUM_OF_DESC_PADDING_WORDS; i++)
            {
                Descr_p->PaddingWords[i] = DMAResource_Read32(Handle,
                                                              WordOffset + i + 8);
            }
        }
#endif
        return true;
    }
    return false;
}


#endif /* Include Guard */


/* end of file eip93_level0.h */
