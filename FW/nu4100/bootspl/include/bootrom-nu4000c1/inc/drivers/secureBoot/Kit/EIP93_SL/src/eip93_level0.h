/* eip93_level0.h
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

#ifndef INCLUDE_GUARD_EIP93_LEVEL0_H
#define INCLUDE_GUARD_EIP93_LEVEL0_H

#include "basic_defs.h"           // BIT definitions, bool, uint32_t
#include "device_rw.h"            // Read32, Write32, Device_Handle_t
#include "dmares_rw.h"            // Read32, Write32, HWPAL_DMAResource_t
#include "eip93_arm.h"            // the API we will implement
#include "eip93_hw_interface.h"   // the HW interface (register map)

/*-----------------------------------------------------------------------------
 * EIP93_Read32
 *
 * This routine writes to a Register  location in the EIP93.
 */
static inline uint32_t
EIP93_Read32(
    Device_Handle_t Device,
    const unsigned int Offset)
{
    return Device_Read32(Device, Offset);
}


/*-----------------------------------------------------------------------------
 * EIP93_Write32
 *
 * This routine writes to a Register location in the EIP93.
 */
static inline void
EIP93_Write32(
    Device_Handle_t Device,
    const unsigned int  Offset,
    const uint32_t Value)
{
    Device_Write32(Device, Offset, Value);
}


/*----------------------------------------------------------------------------
 * EIP93_Read32Array
 *
 * This routine reads from a array of Register/OUT_RAM  memory locations in
 * the EIP93.
 */
static inline void
EIP93_Read32Array(
    Device_Handle_t Device,
    unsigned int Offset,               // read starts here, +4 increments
    uint32_t * MemoryDst_p,            // writing starts here
    const int Count)                   // number of uint32's to transfer
{
    Device_Read32Array(Device, Offset, MemoryDst_p, Count);
}


/*----------------------------------------------------------------------------
 * EIP93_Write32Array
 *
 * This routine writes to a array of Register/IN_RAM memory locations in
 * the EIP93.
 */
static inline void
EIP93_Write32Array(
    Device_Handle_t Device,
    unsigned int Offset,                // write starts here, +4 increments
    uint32_t * MemorySrc_p,             // writing starts here
    const int Count)                    // number of uint32's to transfer
{
    Device_Write32Array(Device, Offset, MemorySrc_p, Count);
}


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
EIP93_Write32_PE_CFG(
    Device_Handle_t Device,
    const uint8_t fResetPacketEngine,
    const uint8_t fResetRing,
    const uint8_t PE_Mode,
    const uint8_t fBO_CRD_en,
    const uint8_t fBO_SA_en,
    const uint8_t fBO_Data_en,
    const uint8_t fEnableCDRUpdate)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_CONFIG,
                  (fResetPacketEngine ? BIT_0 : 0) |
                  (fResetRing ? BIT_1 : 0) |
                  ((PE_Mode & MASK_2_BITS) <<  8) |
                  (fEnableCDRUpdate ? BIT_10 : 0) |
                  (fBO_CRD_en ? BIT_16 : 0) |
                  (fBO_SA_en ? BIT_17 : 0) |
                  (fBO_Data_en ? BIT_18 : 0));
}


static inline void
EIP93_Read32_PE_CFG(
    Device_Handle_t Device,
    uint8_t * const PE_Mode,
    uint8_t * const fBO_CRD_en,
    uint8_t * const fBO_SA_en,
    uint8_t * const fBO_Data_en,
    uint8_t * const fEnableCDRUpdate)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_CONFIG);
    if (PE_Mode)
    {
        *PE_Mode = (word >> 8) & MASK_2_BITS;
    }
    if (fEnableCDRUpdate)
    {
        *fEnableCDRUpdate = (word >> 10) & MASK_1_BIT;
    }
    if (fBO_CRD_en)
    {
        *fBO_CRD_en = (word >> 16) & MASK_1_BIT;
    }
    if (fBO_SA_en)
    {
        *fBO_SA_en = (word >> 17) & MASK_1_BIT;
    }
    if (fBO_Data_en)
    {
        *fBO_Data_en = (word >> 18) & MASK_1_BIT;
    }
}


static inline void
EIP93_Write32_PE_IO_THRESHOLD(
    Device_Handle_t Device,
    const uint16_t InputThreshold,
    const uint16_t OutputThreshold)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_BUF_THRESH,
                  ( InputThreshold  & MASK_8_BITS) |
                  ((OutputThreshold & MASK_8_BITS) << 16));
}


static inline void
EIP93_Read32_PE_IO_THRESHOLD(
    Device_Handle_t Device,
    uint16_t * const InputThreshold,
    uint16_t * const OutputThreshold)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_BUF_THRESH);
    if (InputThreshold)
    {
        *InputThreshold = word & MASK_10_BITS;
    }
    if (OutputThreshold)
    {
        *OutputThreshold = (word >> 16) & MASK_10_BITS;
    }
}


static inline void
EIP93_Write32_INT_CFG(
    Device_Handle_t Device,
    const uint8_t fIntHostOutputType,
    const uint8_t fIntPulseClear)
{
    EIP93_Write32(Device, EIP93_REG_INT_CFG,
                  (fIntHostOutputType ? BIT_0 : 0) |
                  (fIntPulseClear ? BIT_1 : 0));
}


static inline void
EIP93_Read32_INT_CFG(
    Device_Handle_t Device,
    uint8_t * const fIntHostOutputType,
    uint8_t * const fIntPulseClear)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_INT_CFG);
    if (fIntHostOutputType)
    {
        *fIntHostOutputType = word & MASK_1_BIT;
    }
    if (fIntPulseClear)
    {
        *fIntPulseClear = (word >> 1) & MASK_1_BIT;
    }
}


static inline void
EIP93_Write32_AHB_DMA_CFG(
    Device_Handle_t Device,
    const uint8_t MaxBurstSize,
    const uint8_t MasterBigEndian,
    const uint8_t LockEnable,
    const uint8_t IncrEnable,
    const uint8_t IdleEnable)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_DMA_CONFIG,
                  (MaxBurstSize & MASK_4_BITS) |
                  (MasterBigEndian ? BIT_16 : 0) |
                  (LockEnable ? BIT_18 : 0) |
                  (IncrEnable ? BIT_19 : 0) |
                  (IdleEnable ? BIT_20 : 0));
}


static inline void
EIP93_Read32_AHB_DMA_CFG(
    Device_Handle_t Device,
    uint8_t * const MaxBurstSize,
    uint8_t * const MasterBigEndian,
    uint8_t * const LockEnable,
    uint8_t * const IncrEnable,
    uint8_t * const IdleEnable)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_DMA_CONFIG);
    if (MaxBurstSize)
    {
        *MaxBurstSize = word & MASK_4_BITS;
    }
    if (MasterBigEndian)
    {
        *MasterBigEndian = (word >> 16) & MASK_1_BIT;
    }
    if (LockEnable)
    {
        *LockEnable = (word >> 18) & MASK_1_BIT;
    }
    if (IncrEnable)
    {
        *IncrEnable = (word >> 19) & MASK_1_BIT;
    }
    if (IdleEnable)
    {
        *IdleEnable = (word >> 20) & MASK_1_BIT;
    }
}


static inline void
EIP93_Write32_AHB_ENDIAN_CFG(
    Device_Handle_t Device,
    const uint8_t MasterByteSwap,
    const uint8_t TargetByteSwap)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_ENDIAN_CONFIG,
                  ( MasterByteSwap & MASK_8_BITS) |
                  ((TargetByteSwap & MASK_8_BITS) << 16));
}


static inline void
EIP93_Read32_AHB_ENDIAN_CFG(
    Device_Handle_t Device,
    uint8_t * const MasterByteSwap,
    uint8_t * const TargetByteSwap)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_ENDIAN_CONFIG);
    if (MasterByteSwap)
    {
        *MasterByteSwap = word & MASK_8_BITS;
    }
    if (TargetByteSwap)
    {
        *TargetByteSwap = (word >> 16) & MASK_8_BITS;
    }
}


static inline void
EIP93_Write32_AXI_DMA_CFG(
    Device_Handle_t Device,
    const uint8_t MaxBurstSize,
    const uint8_t MasterBigEndian,
    const uint8_t PostedTransferEnable)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_DMA_CONFIG,
                  (MaxBurstSize & MASK_3_BITS) |
                  (PostedTransferEnable ? BIT_15 : 0) |
                  (MasterBigEndian ? BIT_16 : 0));
}


static inline void
EIP93_Read32_AXI_DMA_CFG(
    Device_Handle_t Device,
    uint8_t * const MaxBurstSize,
    uint8_t * const MasterBigEndian,
    uint8_t * const PostedTransferEnable)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_DMA_CONFIG);
    if (MaxBurstSize)
    {
        *MaxBurstSize = word & MASK_3_BITS;
    }
    if (PostedTransferEnable)
    {
        *PostedTransferEnable = (word >> 15) & MASK_1_BIT;
    }
    if (MasterBigEndian)
    {
        *MasterBigEndian = (word >> 16) & MASK_1_BIT;
    }
}


static inline void
EIP93_Write32_AXI_ENDIAN_CFG(
    Device_Handle_t Device,
    const uint8_t MasterByteSwap,
    const uint8_t TargetByteSwap)
{
    EIP93_Write32(Device,
                  EIP93_REG_PE_ENDIAN_CONFIG,
                  (MasterByteSwap ? BIT_0 : 0) |
                  (TargetByteSwap ? BIT_16 : 0));
}


static inline void
EIP93_Read32_AXI_ENDIAN_CFG(
    Device_Handle_t Device,
    uint8_t * const MasterByteSwap,
    uint8_t * const TargetByteSwap)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_ENDIAN_CONFIG);
    if (MasterByteSwap)
    {
        *MasterByteSwap = word & MASK_1_BIT;
    }
    if (TargetByteSwap)
    {
        *TargetByteSwap = (word >> 16) & MASK_1_BIT;
    }
}


static inline void
EIP93_Read32_PE_OPTIONS_1(
    Device_Handle_t Device,
    uint8_t * const fDesTdes,
    uint8_t * const fARC4,
    uint8_t * const fAES,
    uint8_t * const fAES128,
    uint8_t * const fAES192,
    uint8_t * const fAES256,
    uint8_t * const fKasumiF8,
    uint8_t * const fDesOfgCfg,
    uint8_t * const fAesCfg,
    uint8_t * const fMD5,
    uint8_t * const fSHA1,
    uint8_t * const fSHA224,
    uint8_t * const fSHA256,
    uint8_t * const fSHA384,
    uint8_t * const fSHA512,
    uint8_t * const fKasumiF9,
    uint8_t * const fAesXcbc,
    uint8_t * const fGCM,
    uint8_t * const fGMAC,
    uint8_t * const fAesCbcMac,
    uint8_t * const fAesCbcMac128,
    uint8_t * const fAesCbcMac192,
    uint8_t * const fAesCbcMac256)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_OPTION_1);
    if (fDesTdes)
    {
        *fDesTdes = word & MASK_1_BIT;
    }
    if (fARC4)
    {
        *fARC4 = (word >> 1) & MASK_1_BIT;
    }
    if (fAES)
    {
        *fAES = (word >> 2) & MASK_1_BIT;
    }
    if (fAES128)
    {
        *fAES128 = (word >> 13) & MASK_1_BIT;
    }
    if (fAES192)
    {
        *fAES192 = (word >> 14) & MASK_1_BIT;
    }
    if (fAES256)
    {
        *fAES256 = (word >> 15) & MASK_1_BIT;
    }
    if (fKasumiF8)
    {
        *fKasumiF8 = 0;
    }
    if (fDesOfgCfg)
    {
        *fDesOfgCfg = 0;
    }
    if (fAesCfg)
    {
        *fAesCfg = 0;
    }
    if (fMD5)
    {
        *fMD5 = (word >> 16) & MASK_1_BIT;
    }
    if (fSHA1)
    {
        *fSHA1 = (word >> 17) & MASK_1_BIT;
    }
    if (fSHA224)
    {
        *fSHA224 = (word >> 18) & MASK_1_BIT;
    }
    if (fSHA256)
    {
        *fSHA256 = (word >> 19) & MASK_1_BIT;
    }
    if (fSHA384)
    {
        *fSHA384 = 0;
    }
    if (fSHA512)
    {
        *fSHA512 = 0;
    }
    if (fKasumiF9)
    {
        *fKasumiF9 = 0;
    }
    if (fAesXcbc)
    {
        *fAesXcbc = (word >> 23) & MASK_1_BIT;
    }
    if (fGCM)
    {
        *fGCM = 0;
    }
    if (fGMAC)
    {
        *fGMAC = 0;
    }
    if (fAesCbcMac)
    {
        *fAesCbcMac = (word >> 28) & MASK_1_BIT;
    }
    if (fAesCbcMac128)
    {
         *fAesCbcMac128 = (word >> 29) & MASK_1_BIT;
    }
    if (fAesCbcMac192)
    {
        *fAesCbcMac192 = (word >> 30) & MASK_1_BIT;
    }
    if (fAesCbcMac256)
    {
        *fAesCbcMac256 = (word >> 31) & MASK_1_BIT;
    }
}


static inline void
EIP93_Read32_PE_OPTIONS_0(
    Device_Handle_t Device,
    uint8_t * const fInterfaceType,
    uint8_t * const f64BitAdrIndicator,
    uint8_t * const fExtInterupt,
    uint8_t * const fPRNG,
    uint8_t * const fSARev1,
    uint8_t * const fSARev2,
    uint8_t * const fDynamicSA,
    uint8_t * const fESN,
    uint8_t * const fESP,
    uint8_t * const fAH,
    uint8_t * const fSSL,
    uint8_t * const fTLS,
    uint8_t * const fDTLS,
    uint8_t * const fSRTP,
    uint8_t * const fMacSec)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_OPTION_0);
    if (fInterfaceType)
    {
        *fInterfaceType = word & MASK_3_BITS;
    }
    if (f64BitAdrIndicator)
    {
        *f64BitAdrIndicator = 0;
    }
    if (fExtInterupt)
    {
        *fExtInterupt = 0;
    }
    if (fPRNG)
    {
        *fPRNG = (word >> 6) & MASK_1_BIT;
    }
    if (fSARev1)
    {
        *fSARev1 =  1;
    }
    if (fSARev2)
    {
        *fSARev2 = 0;
    }
    if (fDynamicSA)
    {
        *fDynamicSA = 0;
    }
    if (fESN)
    {
        *fESN = (word >> 15) & MASK_1_BIT;
    }
    if (fESP)
    {
        *fESP = (word >> 16) & MASK_1_BIT;
    }
    if (fAH)
    {
        *fAH = (word >> 17) & MASK_1_BIT;
    }
    if (fSSL)
    {
        *fSSL = (word >> 20) & MASK_1_BIT;
    }
    if (fTLS)
    {
        *fTLS = (word >> 21) & MASK_1_BIT;
    }
    if (fDTLS)
    {
        *fDTLS = (word >> 22) & MASK_1_BIT;
    }
    if (fSRTP)
    {
        *fSRTP = (word >> 24) & MASK_1_BIT;
    }
    if (fMacSec)
    {
        *fMacSec = (word >> 25) & MASK_1_BIT;
    }
}


static inline void
EIP93_Read32_REVISION_REG(
    Device_Handle_t Device,
    uint8_t * const EIPNumber,
    uint8_t * const ComplEIPNumber,
    uint8_t * const HWPatchLevel,
    uint8_t * const MinHWRevision,
    uint8_t * const MajHWRevision)
{
    uint32_t word = EIP93_Read32(Device, EIP93_REG_PE_REVISION);
    if (EIPNumber)
    {
        *EIPNumber = word & MASK_8_BITS;
    }
    if (ComplEIPNumber)
    {
        *ComplEIPNumber = (word >> 8) & MASK_8_BITS;
    }
    if (HWPatchLevel)
    {
        *HWPatchLevel = (word >> 16) & MASK_4_BITS;
    }
    if (MinHWRevision)
    {
        *MinHWRevision = (word >> 20) & MASK_4_BITS;
    }
    if (MajHWRevision)
    {
        *MajHWRevision = (word >> 24) & MASK_4_BITS;
    }
}


#endif /* Include Guard */

/* end of file eip93_level0.h */
