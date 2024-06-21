/* sbhybrid_eip93_aes_if.c
 *
 * Description: Implementation of SB_AES_API/AES_IF.
 */

/*****************************************************************************
* Copyright (c) 2009-2018 INSIDE Secure B.V. All Rights Reserved.
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

/* This source file is intended for EIP93 only. */
#ifdef SBHYBRID_WITH_EIP93
#include "c_lib.h"
#include "aes_if.h"
#include "sbcr_key.h"
#include "dmares_buf.h"
#include "dmares_addr.h"
#include "dmares_rw.h"
#include "device_swap.h"

#include "eip93_arm.h"
#include "cfg_sbif.h"

/* If SBIF_CFG_CONFIDENTIALITY_BITS is defined, this file will only
   allow single AES size. Otherwise, a bit larger generic implementation
   supporting all AES sizes will result. */
// #undef SBIF_CFG_CONFIDENTIALITY_BITS

#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
#if (SBIF_CFG_CONFIDENTIALITY_BITS != 128) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 192) && \
    (SBIF_CFG_CONFIDENTIALITY_BITS != 256)
#error "SBIF_CFG_CONFIDENTIALITY_BITS must be one of 128, 192, or 256."
#endif
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */

/* Features of this implementation:
   + This implementation is only suitable for Secure Boot processing.
   + Not for signing. */
#define AES_IF_OMIT_KEYGEN /* Always omit key generation. */
#define AES_IF_OMIT_CBC_ENCRYPT /* CBC only supported for decryption */
#define AES_IF_OMIT_ECB_DECRYPT /* ECB only supported for encryption and
                                   unwrapping */
#define AES_IF_OMIT_WRAP        /* AES-WRAP only supports unwrap, not wrap. */

#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
static inline int
AES_IF_Ctx_LoadKeyData(struct AES_IF_Ctx * Ctx_p,
                       const void * KeyData_p,
                       unsigned int KeyDataBits)
{
    if (KeyDataBits == SBIF_CFG_CONFIDENTIALITY_BITS)
    {
        c_memcpy(Ctx_p->KeyData, KeyData_p, SBIF_CFG_CONFIDENTIALITY_BITS / 8);
        return 1;
    }
    return 0;
}

#define AES_IF_CTX_SET_KEY_DATA(Ctx_p, KeyData_p, KeyDataBits) \
    AES_IF_Ctx_LoadKeyData(Ctx_p, KeyData_p, KeyDataBits)
#define AES_IF_CTX_KEY_DATA(Ctx_p) ((Ctx_p)->KeyData)
#define AES_IF_CTX_KEY_BYTES(Ctx_p) ((SBIF_CFG_CONFIDENTIALITY_BITS) / 8)
#define AES_IF_CTX_KEY_BITS(Ctx_p) (SBIF_CFG_CONFIDENTIALITY_BITS)
#else
static inline int
AES_IF_Ctx_LoadKeyData(struct AES_IF_Ctx * Ctx_p,
                       const void * KeyData_p,
                       unsigned int KeyDataBits)
{
    if (KeyDataBits == 128 || KeyDataBits == 192 || KeyDataBits == 256)
    {
        c_memcpy(Ctx_p->KeyDataBuffer, KeyData_p, KeyDataBits / 8);
        Ctx_p->KeyDataBytes = KeyDataBits / 8;
        return 1;
    }
    return 0;
}

#define AES_IF_CTX_SET_KEY_DATA(Ctx_p, KeyData_p, KeyDataBits) \
    AES_IF_Ctx_LoadKeyData(Ctx_p, KeyData_p, KeyDataBits)
#define AES_IF_CTX_KEY_DATA(Ctx_p) ((Ctx_p)->KeyDataBuffer)
#define AES_IF_CTX_KEY_BYTES(Ctx_p) ((Ctx_p)->KeyDataBytes)
#define AES_IF_CTX_KEY_BITS(Ctx_p) (((Ctx_p)->KeyDataBytes) * 8)
#endif

/* This implementation of aes_if.h uses packet engine.
   When a Secure Boot Confidentiality Root Key (for AES-Wrap) is needed,
   the implementation uses key material defined in sbcr_key.c.
   The default key in sbcr_key.c SHALL be customized.
*/

/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_AES_Init
 */
SB_Result_t
SBHYBRID_EIP93_AES_Init(SBHYBRID_SymmContext_t * const Context_p,
                        const void * Key_p,
                        unsigned int KeyByteCount,
                        unsigned int V32_0,
                        unsigned int V32_1)
{
    L_TRACE(LF_SBHYBRID, "Initializing AES operation [CMD=0x%08x:0x%08x]",
            V32_0, V32_1);

    // initialize the SA and State memory
    c_memset(Context_p->SA_States[SA_STATE_CIPHER].SA_Host_p,
             0,
             SBHYBRID_SASTATE_BYTES);

    // Load Key
    DMAResource_Write32Array(Context_p->SA_States[SA_STATE_CIPHER].Handle,
                             SBHYBRID_SASTATE_WORDOFFSET_KEY + SA_STATE_CIPHER_OFFSET,
                             KeyByteCount / 4,
                             Key_p);

#ifndef SBIF_CFG_CONFIDENTIALITY_BITS
    /* Add key length to SA_CMD_1. */
    V32_1 |= KeyByteCount << 21;
#endif /*SBIF_CFG_CONFIDENTIALITY_BITS */

    // SA_CMD_0
    DMAResource_Write32(Context_p->SA_States[SA_STATE_CIPHER].Handle,
                        0 + SA_STATE_CIPHER_OFFSET, V32_0);

    // SA_CMD_1
    DMAResource_Write32(Context_p->SA_States[SA_STATE_CIPHER].Handle,
                        1 + SA_STATE_CIPHER_OFFSET, V32_1);
    return SB_SUCCESS;
}

void
SBHYBRID_EIP93_AES_WriteIV(SBHYBRID_SymmContext_t * const Context_p,
                           const uint32_t IV_p[4])
{
    // Load IV
    DMAResource_Write32Array(Context_p->SA_States[SA_STATE_CIPHER].Handle,
                             SBHYBRID_SASTATE_WORDOFFSET_IV + SA_STATE_CIPHER_OFFSET,
                             4,
                             IV_p);
}

#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
/* If only single size, prefill the CMD_1 mask with size. */
#define SBHYBRID_EIP93_CMD_AES_SIZE ((SBIF_CFG_CONFIDENTIALITY_BITS / 64) << 24)
#else
/* If supporting multiple sizes, compute size mask as needed. */
#define SBHYBRID_EIP93_CMD_AES_SIZE 0
#endif

#define SBHYBRID_EIP93_CMD_AES_ECB_ENCRYPT_0 \
    /* savehash | saveiv | hash_source | iv_source | digest_length | */ \
    (0 << 29) | (0 << 28) | (3 << 26) | (0 << 24) | (0 << 20) | \
    /* proc+pad hash | cipher | pad | opgroup | direction | opcode */ \
    (0 << 17) | (1 << 12) | (3 << 8) | (0 << 6) | (0 << 4) | (0 << 3) | 0

#define SBHYBRID_EIP93_CMD_AES_ECB_ENCRYPT_1 \
    /* cipher_mode | aes_key_length [in 64 bit blocks] */ \
    (0 << 8) | SBHYBRID_EIP93_CMD_AES_SIZE

#define SBHYBRID_EIP93_CMD_AES_ECB_DECRYPT_0 \
    /* savehash | saveiv | hash_source | iv_source | digest_length | */ \
    (0 << 29) | (0 << 28) | (3 << 26) | (0 << 24) | (0 << 20) | \
    /* proc+pad hash | cipher | pad | opgroup | direction | opcode */ \
    (0 << 17) | (1 << 12) | (3 << 8) | (0 << 6) | (0 << 4) | (1 << 3) | 0

#define SBHYBRID_EIP93_CMD_AES_ECB_DECRYPT_1 \
    /* cipher_mode | aes_key_length [in 64 bit blocks] */ \
    (0 << 8) | SBHYBRID_EIP93_CMD_AES_SIZE

#define SBHYBRID_EIP93_CMD_AES_CBC_DECRYPT_0 \
    /* savehash | saveiv | hash_source | iv_source | digest_length | */ \
    (0 << 29) | (0 << 28) | (3 << 26) | (2 << 24) | (0 << 20) | \
    /* proc+pad hash | cipher | pad | opgroup | direction | opcode */ \
    (0 << 17) | (1 << 12) | (3 << 8) | (0 << 6) | (0 << 4) | (1 << 3) | 0

#define SBHYBRID_EIP93_CMD_AES_CBC_DECRYPT_1 \
    /* cipher_mode | aes_key_length [in 64 bit blocks] */ \
    (1 << 8) | SBHYBRID_EIP93_CMD_AES_SIZE

/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_AES_AddBlock
 */
SB_Result_t
SBHYBRID_EIP93_AES_AddBlock(SBHYBRID_SymmContext_t * const Context_p,
                            const uint8_t InputBytes_p[16])
{
    EIP93_ARM_CommandDescriptor_t Cmds[1];
    unsigned int CmdCount = 0;
    int res;
    uint32_t tmp_input[4];

    /* SBHYBRID_EIP93_AES_AddBlock currently allows only adding 1 block. */
    PRECONDITION(Context_p->PendingCount == 0);

    L_TRACE(LF_SBHYBRID,
            "ByteCount=%u; "
            "Data="
            "0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
            16,
            InputBytes_p[0], InputBytes_p[1], InputBytes_p[2], InputBytes_p[3],
            InputBytes_p[4], InputBytes_p[5], InputBytes_p[6], InputBytes_p[7],
            InputBytes_p[8], InputBytes_p[9], InputBytes_p[10], InputBytes_p[11],
            InputBytes_p[12], InputBytes_p[13], InputBytes_p[14], InputBytes_p[15]);

    c_memset(Cmds, 0, sizeof(Cmds));

    // set the initial value (AES-ECB input)
    c_memcpy(tmp_input, InputBytes_p, 16);
    DMAResource_Write32Array(Context_p->SA_States[SA_STATE_CIPHER].Handle,
                             SBHYBRID_SASTATE_WORDOFFSET_INPUT + SA_STATE_CIPHER_OFFSET,
                             4,
                             tmp_input);
    c_memset(tmp_input, 0, 16);

    // control fields for the command descriptor
    // EIP93_CommandDescriptor_Control_MakeWord helper function
    // can be used for obtaining this word
    Cmds[CmdCount].ControlWord = BIT_0; // Host_Ready=1;

    Cmds[CmdCount].SrcPacketByteCount = 16;
    Cmds[CmdCount].SADataAddr.Addr = Context_p->SA_States[SA_STATE_CIPHER].SA_PhysAddr;
    Cmds[CmdCount].SAStateDataAddr.Addr = Context_p->SA_States[SA_STATE_CIPHER].State_PhysAddr;
    Cmds[CmdCount].SrcPacketAddr.Addr = Cmds[CmdCount].SAStateDataAddr.Addr;
    Cmds[CmdCount].DstPacketAddr.Addr = Cmds[CmdCount].SrcPacketAddr.Addr +
                                        (SBHYBRID_SASTATE_WORDOFFSET_OUTPUT - SBHYBRID_SASTATE_WORDOFFSET_INPUT) * 4;
    CmdCount++;

    // put the command in the ring (and start processing)
    {
        unsigned int DoneCount = 0;

        res = EIP93_ARM_PacketPut(&Context_p->EIP93_IOArea,
                                  Cmds, CmdCount,
                                  &DoneCount);
        if (res < 0 || DoneCount != CmdCount)
        {
            return SB_ERROR_HARDWARE;
        }

        Context_p->PendingCount += CmdCount;
    }

    return SB_SUCCESS;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_AES_AddBlocks
 * Note: InputBytes_p:InputByteCount and
 * OutputBytes_p:InputByteCount must be DMAable memory locations.
 * Note2: Output byte count is always exactly the same than input byte count.
 */
SB_Result_t
SBHYBRID_EIP93_AES_AddBlocks(SBHYBRID_SymmContext_t * const Context_p,
                             const uint8_t * InputBytes_p,
                             uint8_t * OutputBytes_p,
                             unsigned int InputByteCount)
{
    EIP93_ARM_CommandDescriptor_t Cmds[SBHYBRID_MAX_DESCRIPTORS_PER_VECTOR];
    unsigned int CmdCount = 0;
    DMAResource_Properties_t Props = { 0, 0, 0, 0 };
    DMAResource_AddrPair_t InputAddrPair;
    DMAResource_AddrPair_t OutputAddrPair;
    DMAResource_Handle_t DMAHandle;
    uint32_t SrcAddr;
    uint32_t DstAddr;
    int res;

    L_TRACE(LF_SBHYBRID,
            "ByteCount=%u; Data="
            "0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x "
            "...",
            InputByteCount,
            InputBytes_p[0], InputBytes_p[1], InputBytes_p[2], InputBytes_p[3],
            InputBytes_p[4], InputBytes_p[5], InputBytes_p[6], InputBytes_p[7],
            InputBytes_p[8], InputBytes_p[9], InputBytes_p[10], InputBytes_p[11],
            InputBytes_p[12], InputBytes_p[13], InputBytes_p[14], InputBytes_p[15]);

    if (InputByteCount > SBHYBRID_MAX_SIZE_DATA_BYTES)
    {
        return SB_ERROR_HARDWARE;
    }

    // get the physical address of the source buffer
    Props.Size = InputByteCount;
    if (Props.Size < 4)
    {
        Props.Size = 4;
    }
    Props.Alignment = 4;

    InputAddrPair.Domain = DMARES_DOMAIN_HOST;
    InputAddrPair.Address.Native_p = (void *)InputBytes_p;

    res = DMAResource_CheckAndRegister(Props, InputAddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    // remember the handle
    Context_p->DMAHandles.Handles[Context_p->DMAHandles.Count++] = DMAHandle;

    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP93DMA, &InputAddrPair);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    SrcAddr = InputAddrPair.Address.Value32;

    OutputAddrPair.Domain = DMARES_DOMAIN_HOST;
    OutputAddrPair.Address.Native_p = (void *)OutputBytes_p;

    res = DMAResource_CheckAndRegister(Props, OutputAddrPair, 'R', &DMAHandle);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    // remember the handle
    Context_p->DMAHandles.Handles[Context_p->DMAHandles.Count++] = DMAHandle;

    res = DMAResource_Translate(DMAHandle, DMARES_DOMAIN_EIP93DMA, &OutputAddrPair);
    if (res != 0)
    {
        return SB_ERROR_HARDWARE;
    }

    c_memset(Cmds, 0, sizeof(Cmds));

    DstAddr = OutputAddrPair.Address.Value32;
    while (InputByteCount > 0)
    {
        // control fields for the command descriptor
        // EIP93_CommandDescriptor_Control_MakeWord helper function
        // can be used for obtaining this word
        Cmds[CmdCount].ControlWord = BIT_0; // Host_Ready=1;

        if (InputByteCount > SBHYBRID_MAX_SIZE_DATA_BLOCKS)
        {
            Cmds[CmdCount].SrcPacketByteCount = SBHYBRID_MAX_SIZE_DATA_BLOCKS;
        }
        else
        {
            Cmds[CmdCount].SrcPacketByteCount = InputByteCount;
        }

        Cmds[CmdCount].SADataAddr.Addr = Context_p->SA_States[SA_STATE_CIPHER].SA_PhysAddr;
        Cmds[CmdCount].SAStateDataAddr.Addr = Context_p->SA_States[SA_STATE_CIPHER].State_PhysAddr;

        Cmds[CmdCount].SrcPacketAddr.Addr = SrcAddr;
        Cmds[CmdCount].DstPacketAddr.Addr = DstAddr;

        SrcAddr += Cmds[CmdCount].SrcPacketByteCount;
        DstAddr += Cmds[CmdCount].SrcPacketByteCount;
        InputByteCount -= Cmds[CmdCount].SrcPacketByteCount;

        CmdCount++;
    }

    // put the command in the ring (and start processing)
    {
        unsigned int DoneCount = 0;

        res = EIP93_ARM_PacketPut(&Context_p->EIP93_IOArea,
                                  Cmds, CmdCount,
                                  &DoneCount);
        if (res < 0 || DoneCount != CmdCount)
        {
            return SB_ERROR_HARDWARE;
        }

        Context_p->PendingCount += CmdCount;
    }

    return SB_SUCCESS;
}

/*----------------------------------------------------------------------------
 * SBHYBRID_EIP93_SHA256_GetDigest
 */
void
SBHYBRID_EIP93_AES_GetIV(SBHYBRID_SymmContext_t * const Context_p,
                         uint8_t * IV_p)
{
    uint32_t IV[4];

    DMAResource_Read32Array(Context_p->SA_States[SA_STATE_CIPHER].Handle,
                            SBHYBRID_SASTATE_WORDOFFSET_IV + SA_STATE_CIPHER_OFFSET,
                            4,
                            IV);

    c_memcpy(IV_p, IV, 16);
    c_memset(IV, 0, 16);
}

SB_Result_t
SBHYBRID_EIP93_AES_FinishIV(SBHYBRID_SymmContext_t * const Context_p,
                            uint8_t IVOutput_p[16])
{
    SB_Result_t res;

    /* Keep processing while state is pending. */
    do
    {
        /* The fsm is shared with SHA224 or SHA256. */
#if SBIF_ECDSA_WORDS == 7
        res = SBHYBRID_EIP93_SHA224_RunFsm(Context_p);
#elif SBIF_ECDSA_WORDS == 8
        res = SBHYBRID_EIP93_SHA256_RunFsm(Context_p);
#else
#error "Unsupported SBIF_ECDSA_WORDS."
#endif /* SBIF_ECDSA_WORDS */

        if (res != 0 && res != SBHYBRID_PENDING)
        {
            L_DEBUG(LF_SBHYBRID,
                    "SBHYBRID_EIP93_SHA256_RunFsm unexpected result code: %u\n",
                    (unsigned int)res);
        }
    } while (res == SBHYBRID_PENDING);

    if (res == SB_SUCCESS && IVOutput_p)
    {
        /* GetIV (CBC mode) or output bytes (ECB mode). */
        SBHYBRID_EIP93_AES_GetIV(Context_p, IVOutput_p);
    }

    while (Context_p->DMAHandles.Count > 1)
    {
        Context_p->DMAHandles.Count--;
        DMAResource_Release(Context_p->DMAHandles.Handles[Context_p->DMAHandles.Count]);
    }

    return res;
}

#define AES_IF_SB_RESULT(x) \
     ((x)==(SB_SUCCESS)?AES_IF_RESULT_SUCCESS:AES_IF_RESULT_GENERIC_ERROR)

uint32_t
AES_IF_Ctx_GetSize(void)
{
    return sizeof(struct AES_IF_Ctx);
}

AES_IF_ResultCode_t
AES_IF_Ctx_Init(AES_IF_Ctx_Ptr_t Ctx_p)
{
    L_DEBUG(LF_SBHYBRID, "Initializing EIP93 AES_IF implementation");

    c_memset(Ctx_p, 0, sizeof(struct AES_IF_Ctx));
    return AES_IF_SB_RESULT(SBHYBRID_Initialize_HW(Ctx_p));
}

void
AES_IF_Ctx_Uninit(AES_IF_Ctx_Ptr_t Ctx_p)
{
    L_TRACE(LF_SBHYBRID, "Uninitializing EIP93 AES_IF implementation");

    SBHYBRID_Uninitialize_HW(Ctx_p);
}

AES_IF_ResultCode_t
AES_IF_Ctx_GetError(AES_IF_Ctx_Ptr_t Ctx_p)
{
    AES_IF_ResultCode_t result = Ctx_p->Result;
    return result;
}

void
AES_IF_Ctx_LoadKey(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const Key_p,
                   const uint32_t KeyLength)
{
    if (AES_IF_CTX_SET_KEY_DATA(Ctx_p, Key_p, KeyLength))
    {
        /* Key loading successful */
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

void
AES_IF_Ctx_LoadIV(AES_IF_Ctx_Ptr_t Ctx_p,
                  const uint32_t IV[4])
{
    c_memcpy(Ctx_p->IV, IV, 16);
}

void
AES_IF_Ctx_SetKEK(AES_IF_Ctx_Ptr_t Ctx_p,
                  const void * const KEKIdentification,
                  const uint32_t KEKIdentificationSize)
{
    if (KEKIdentificationSize == 0)
    {
        /* Load Key from Sbhybrid_SBCRK. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
        /* Ensure Sbhybrid_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT(sizeof(Ctx_p->KeyData) == sizeof(SBCommon_SBCRK));
#else
        /* Ensure SBCommon_SBCRK has correct size.
           Note: unusable size will result in a compile time error. */
        COMPILE_STATIC_ASSERT(sizeof(SBCommon_SBCRK) == 16 ||
                              sizeof(SBCommon_SBCRK) == 24 ||
                              sizeof(SBCommon_SBCRK) == 32);
#endif

        /* Load Key from SBCommon_SBCRK. */
        AES_IF_Ctx_LoadKey(Ctx_p, SBCommon_SBCRK, sizeof(SBCommon_SBCRK) * 8);
    }
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    /* Allow one of (16, 24, or 32). */
    else if (KEKIdentificationSize == SBIF_CFG_CONFIDENTIALITY_BITS / 8)
#else
    /* Allow any size (16, 24, or 32) */
    else if (KEKIdentificationSize == 16 ||
             KEKIdentificationSize == 24 ||
             KEKIdentificationSize == 32)
#endif
    {
        /* Assume KEK provided is the key to use as raw data. */
        AES_IF_Ctx_LoadKey(Ctx_p, KEKIdentification,
                           KEKIdentificationSize * 8);
    }
    else
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

static bool
SBHYBRID_EIP93_AES_generic_unwrap(AES_IF_Ctx_Ptr_t Ctx_p,
                                  const void * const WrappedIn_p,
                                  void * const KeyOut_p,
                                  uint32_t KeyBits,
                                  const void * KWK_p,
                                  const uint32_t KWKBits)
{
    uint32_t R[256 * 8 / 64][2];
    union
    {
        /* A and B variables, combined storage */
        uint8_t A_bytes[16];
        uint32_t A[2];
        uint8_t B_bytes[16];
        uint32_t B[4];
    } u;
    int i;
    int j;
    uint32_t n = KeyBits / 64;
    uint16_t cnt;
    SB_Result_t res;

    /* Confidentiality key size must much configured key size. */
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    PRECONDITION(SBIF_CFG_CONFIDENTIALITY_BITS == KWKBits);
#endif /* SBIF_CFG_CONFIDENTIALITY_BITS */

    c_memcpy(u.A, WrappedIn_p, 8);

    res = SBHYBRID_EIP93_AES_Init(&(Ctx_p->SymmContext),
                                  KWK_p, KWKBits / 8,
                                  SBHYBRID_EIP93_CMD_AES_ECB_DECRYPT_0,
                                  SBHYBRID_EIP93_CMD_AES_ECB_DECRYPT_1);
    if (res != SB_SUCCESS)
    {
        return 0;
    }

    c_memcpy(R, ((char *)WrappedIn_p) + 8, n * 8);

    for (j = 5; j >= 0; j--)
    {
        for (i = n; i >= 1; i--)
        {
            u.B[2] = R[i - 1][0];
            u.B[3] = R[i - 1][1];
            cnt = (uint16_t)(n * j + i);
            u.A_bytes[7] ^= cnt & 255;
            u.A_bytes[6] ^= cnt >> 8;
            res = SBHYBRID_EIP93_AES_AddBlock(&(Ctx_p->SymmContext), u.B_bytes);
            if (res == SB_SUCCESS)
            {
                res = SBHYBRID_EIP93_AES_FinishIV(&(Ctx_p->SymmContext), u.B_bytes);
            }
            if (res != SB_SUCCESS)
            {
                return 0;
            }
            R[i - 1][0] = u.B[2];
            R[i - 1][1] = u.B[3];
        }
    }

    c_memcpy(KeyOut_p, R, n * 8);

    return u.A[0] == u.A[1] && u.A[0] == 0xA6A6A6A6;
}

void
AES_IF_Ctx_LoadWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                          const void * const Wrap_p,
                          const uint32_t WrapLength)
{
    bool ok = false;

#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    if (WrapLength == (SBIF_CFG_CONFIDENTIALITY_BITS + 64))
    {
        ok = SBHYBRID_EIP93_AES_generic_unwrap(Ctx_p,
                                               Wrap_p,
                                               AES_IF_CTX_KEY_DATA(Ctx_p),
                                               SBIF_CFG_CONFIDENTIALITY_BITS,
                                               AES_IF_CTX_KEY_DATA(Ctx_p),
                                               AES_IF_CTX_KEY_BITS(Ctx_p));
    }
#else
    if ((WrapLength <= ((sizeof(Ctx_p->KeyDataBuffer) * 8) + 64)) &&
        (WrapLength & 63) == 0)
    {
        ok = SBHYBRID_EIP93_AES_generic_unwrap(Ctx_p,
                                               Wrap_p,
                                               AES_IF_CTX_KEY_DATA(Ctx_p),
                                               WrapLength - 64,
                                               AES_IF_CTX_KEY_DATA(Ctx_p),
                                               AES_IF_CTX_KEY_BITS(Ctx_p));

        Ctx_p->KeyDataBytes = (WrapLength - 64) / 8;
    }
#endif

    if (ok == false)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_Ctx_GenerateKey(AES_IF_Ctx_Ptr_t Ctx_p,
                       void * const Key_p,
                       const uint32_t KeyLength)
{
#ifdef AES_IF_OMIT_KEYGEN
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(Key_p);
    IDENTIFIER_NOT_USED(KeyLength);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP93_AES_IF: Key generation cannot be enabled."
#endif
}
#endif

#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_Ctx_GenerateWrappedKey(AES_IF_Ctx_Ptr_t Ctx_p,
                              void * const Wrap_p,
                              const uint32_t WrapLength)
{
#ifdef AES_IF_OMIT_KEYGEN
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(Wrap_p);
    IDENTIFIER_NOT_USED(WrapLength);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP93_AES_IF: Key generation / wrapping cannot be enabled."
#endif
}
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_ECB_EncryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p)
{
    SB_Result_t res;

    res = SBHYBRID_EIP93_AES_Init(&(Ctx_p->SymmContext),
                                  AES_IF_CTX_KEY_DATA(Ctx_p),
                                  AES_IF_CTX_KEY_BYTES(Ctx_p),
                                  SBHYBRID_EIP93_CMD_AES_ECB_ENCRYPT_0,
                                  SBHYBRID_EIP93_CMD_AES_ECB_ENCRYPT_1);
    if (res == SB_SUCCESS)
    {
        res = SBHYBRID_EIP93_AES_AddBlock(&Ctx_p->SymmContext, DataIn_p);
        if (res == SB_SUCCESS)
        {
            res = SBHYBRID_EIP93_AES_FinishIV(&Ctx_p->SymmContext, DataOut_p);
        }
    }
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}
#endif
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_ECB_DecryptBlock(AES_IF_Ctx_Ptr_t Ctx_p,
                        const void * const DataIn_p,
                        void * const DataOut_p)
{
#ifdef AES_IF_OMIT_ECB_DECRYPT
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(DataIn_p);
    IDENTIFIER_NOT_USED(DataOut_p);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP93_AES_IF: ECB Decryption cannot be enabled."
#endif
}
#endif
#endif

#ifdef ENABLE_NOT_USED_FUNCTION
#ifdef SBSWCRYPTO_CF_FULL_FUNCTIONALITY
void
AES_IF_CBC_Encrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const DataIn_p,
                   void * const DataOut_p,
                   const uint32_t Size)
{
#ifdef AES_IF_OMIT_CBC_ENCRYPT
    IDENTIFIER_NOT_USED(Ctx_p);
    IDENTIFIER_NOT_USED(DataIn_p);
    IDENTIFIER_NOT_USED(DataOut_p);
    IDENTIFIER_NOT_USED(Size);
    Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
#else
#error "SBHYBRID_EIP93_AES_IF: CBC Encryption cannot be enabled."
#endif
}
#endif
#endif


/* Note: This implementation of AES_IF_CBC_Decrypt requires
   that both DataIn_p and DataOut_p are DMA-able memory. */
void
AES_IF_CBC_Decrypt(AES_IF_Ctx_Ptr_t Ctx_p,
                   const void * const DataIn_p,
                   void * const DataOut_p,
                   const uint32_t Size)
{
    SB_Result_t res;

    res = SBHYBRID_EIP93_AES_Init(&(Ctx_p->SymmContext),
                                  AES_IF_CTX_KEY_DATA(Ctx_p),
                                  AES_IF_CTX_KEY_BYTES(Ctx_p),
                                  SBHYBRID_EIP93_CMD_AES_CBC_DECRYPT_0,
                                  SBHYBRID_EIP93_CMD_AES_CBC_DECRYPT_1);
    if (res == SB_SUCCESS)
    {
        SBHYBRID_EIP93_AES_WriteIV(&(Ctx_p->SymmContext), Ctx_p->IV);

        /* Get next IV. */
        c_memcpy(Ctx_p->IV, ((const char *)DataIn_p) + Size - 16, 16);

        res = SBHYBRID_EIP93_AES_AddBlocks(&(Ctx_p->SymmContext),
                                           DataIn_p, DataOut_p, Size);
        if (res == SB_SUCCESS)
        {
            res = SBHYBRID_EIP93_AES_FinishIV(&Ctx_p->SymmContext, NULL);
        }
    }
    if (res != SB_SUCCESS)
    {
        Ctx_p->Result = AES_IF_RESULT_GENERIC_ERROR;
    }
}

#endif /* SBHYBRID_WITH_EIP93 */

/* end of file sbhybrid_eip93_aes_if.c */
