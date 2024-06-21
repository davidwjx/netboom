/* sbhybrid_eip93_sha224.c
 *
 * Secure Boot SHA-224 Hash acceleration using EIP-93.
 */

/*****************************************************************************
* Copyright (c) 2012-2018 INSIDE Secure B.V. All Rights Reserved.
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
#define __MODULE__ "sbhybrid_eip93_sha224.c"
#endif

#include "sbhybrid_internal.h"

/* This source file is intended for EIP93 only. */
#ifdef SBHYBRID_WITH_EIP93
#include "c_lib.h"

#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "device_swap.h"

#include "eip93_arm.h"


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_SHA224_Init
 */
SB_Result_t
SBHYBRID_EIP93_SHA224_Init(SBHYBRID_SymmContext_t * const Hash_p)
{
    static const uint32_t SHA224_InitialDigest[8] =
    {
        0xc1059ed8,
        0x367cd507,
        0x3070dd17,
        0xf70e5939,
        0xffc00b31,
        0x68581511,
        0x64f98fa7,
        0xbefa4fa4
    };
    uint32_t V32;

    L_TRACE(LF_SBHYBRID, "Initializing");

    // initialize the SA and State memory
    c_memset(Hash_p->SA_States[SA_STATE_HASH].SA_Host_p,
             0,
             SBHYBRID_SASTATE_BYTES);

    // set the initial SHA-224 digest
    // this allows the remainder of this code to assume 'hash continuation'
    DMAResource_Write32Array(Hash_p->SA_States[SA_STATE_HASH].Handle,
                             SBHYBRID_SASTATE_WORDOFFSET_DIGEST,
                             8,
                             SHA224_InitialDigest);

    // SA_CMD_0
    V32 = 0x3 |                         // OpCode = 0b011
          (15 << 8) |                   // Cipher = Null
          (2 << 12) |                   // Hash = SHA-224
          (2 << 26) |                   // HashSource = FromState
          BIT_29;                       // SaveHashState
    DMAResource_Write32(Hash_p->SA_States[SA_STATE_HASH].Handle, 0, V32);

    // Hash message length is now initialized to zero
    return SB_SUCCESS;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_SHA224_AddBlock
 */
SB_Result_t
SBHYBRID_EIP93_SHA224_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                               const uint8_t * DataBytes_p,
                               unsigned int DataByteCount,
                               bool fFinal)
{
    EIP93_ARM_CommandDescriptor_t Cmds[SBHYBRID_MAX_DESCRIPTORS_PER_VECTOR];
    unsigned int CmdCount = 0;
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t AddrPair;
    DMAResource_Handle_t DMAHandle;
    uint32_t SrcAddr;
    int res;

    L_TRACE(LF_SBHYBRID,
            "fFinal=%u; DataByteCount=%u; Data=0x%02x 0x%02x 0x%02x 0x%02x",
            (unsigned int)fFinal,
            DataByteCount,
            DataBytes_p[0], DataBytes_p[1], DataBytes_p[2], DataBytes_p[3]);

    if (DataByteCount > SBHYBRID_MAX_SIZE_DATA_BYTES)
    {
        return SB_ERROR_HARDWARE;
    }

    // get the physical address of the source buffer
    Props.Size = DataByteCount;
    if (Props.Size < 4)
    {
        Props.Size = 4;
    }
    Props.Alignment = 4;

    AddrPair.Domain = DMARES_DOMAIN_HOST;
    AddrPair.Address.Native_p = (void *)DataBytes_p;

    res = DMAResource_CheckAndRegister(Props, AddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    // remember the handle
    Hash_p->DMAHandles.Handles[Hash_p->DMAHandles.Count++] = DMAHandle;

    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP93DMA, &AddrPair);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    c_memset(Cmds, 0, sizeof(Cmds));

    SrcAddr = AddrPair.Address.Value32;
    while (DataByteCount > 0)
    {
        // control fields for the command descriptor
        // EIP93_CommandDescriptor_Control_MakeWord helper function
        // can be used for obtaining this word
        Cmds[CmdCount].ControlWord = BIT_0; // Host_Ready=1;

        if (DataByteCount > SBHYBRID_MAX_SIZE_DATA_BLOCKS)
        {
            Cmds[CmdCount].SrcPacketByteCount = SBHYBRID_MAX_SIZE_DATA_BLOCKS;
        }
        else
        {
            if (fFinal)
            {
                Cmds[CmdCount].ControlWord |= BIT_4;
            }

            Cmds[CmdCount].SrcPacketByteCount = DataByteCount;
        }

        Cmds[CmdCount].SADataAddr.Addr = Hash_p->SA_States[SA_STATE_HASH].SA_PhysAddr;
        Cmds[CmdCount].SAStateDataAddr.Addr = Hash_p->SA_States[SA_STATE_HASH].State_PhysAddr;

        Cmds[CmdCount].SrcPacketAddr.Addr = SrcAddr;

        SrcAddr += Cmds[CmdCount].SrcPacketByteCount;
        DataByteCount -= Cmds[CmdCount].SrcPacketByteCount;

        CmdCount++;
    }

    // put the command in the ring (and start processing)
    {
        unsigned int DoneCount = 0;

        res = EIP93_ARM_PacketPut(&Hash_p->EIP93_IOArea,
                                  Cmds, CmdCount,
                                  &DoneCount);

        if (res < 0 || DoneCount != CmdCount)
        {
            return SB_ERROR_HARDWARE;
        }

        Hash_p->PendingCount += CmdCount;
    }

    return SB_SUCCESS;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_SHA224_RunFsm
 */
SB_Result_t
SBHYBRID_EIP93_SHA224_RunFsm(SBHYBRID_SymmContext_t * const Hash_p)
{
    if (Hash_p->PendingCount > 0)
    {
        EIP93_ARM_ResultDescriptor_t Res;
        EIP93_Status_t res93;
        unsigned int DoneCount = 0;

        res93 = EIP93_ARM_PacketGet(&Hash_p->EIP93_IOArea,
                                    &Res, 1,
                                    &DoneCount);

        if (res93 != EIP93_STATUS_OK)
        {
            return SB_ERROR_HARDWARE;
        }

        if (DoneCount > 0)
        {
            Hash_p->PendingCount -= 1;
        }
    }

    if (Hash_p->PendingCount == 0)
    {
        return SB_SUCCESS;
    }

    return SBHYBRID_PENDING;
}


/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_SHA224_GetDigest
 */
void
SBHYBRID_EIP93_SHA224_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                                uint8_t * Digest_p)
{
    uint32_t Digest[7];
    unsigned int i;

    DMAResource_Read32Array(Hash_p->SA_States[SA_STATE_HASH].Handle,
                            SBHYBRID_SASTATE_WORDOFFSET_DIGEST,
                            7,
                            Digest);

    for (i = 0; i < 7; i++)
    {
        uint32_t V = Digest[i];

        *Digest_p++ = (uint8_t)(V >> 24);
        *Digest_p++ = (uint8_t)(V >> 16);
        *Digest_p++ = (uint8_t)(V >> 8);
        *Digest_p++ = (uint8_t)V;
    }
}

#endif /* SBHYBRID_WITH_EIP93 */

/* end of file sbhybrid_eip93_sha224.c */
