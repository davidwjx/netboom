/* sbhybrid_internal.h
 *
 * Description: Secure boot library: Internal definitions for SB_HYBRID
 *              implementation of Secure Boot API.
 *              SB_HYBRID allows to choose implementation of symmetric
 *              cryptography and asymmetric cryptography independently.
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

#ifndef INCLUSION_GUARD_SBHYBRID_INTERNAL_H
#define INCLUSION_GUARD_SBHYBRID_INTERNAL_H

#include "sb_ecdsa.h"              // SB_Result_t, SBIF_ECDSA_BYTES
#include "implementation_defs.h"
#include "device_types.h"          // Device_Handle_t
#include "cf_sblib.h"              // Configuration toggles
#include "cfg_sblib.h"             // Configuration values
#include "aes_if.h"                // AES_IF_ResultCode_t

/* Configure according to SB_SW, SB_PK, SB_PEPK, ... */
#ifdef SB_SW
#define SBHYBRID_WITH_SW
#define SBHYBRID_WITH_SWPK
#endif
#ifdef SB_PK
#define SBHYBRID_WITH_SW
#define SBHYBRID_WITH_EIP28
#define SBHYBRID_DO_DMARESOURCE_RELEASE
#endif
#ifdef SB_PE
#define SBHYBRID_WITH_EIP93
#define SBHYBRID_WITH_SWPK
#define SBHYBRID_DO_DMARESOURCE_RELEASE
#endif
#ifdef SB_PEPK
#define SBHYBRID_WITH_EIP93
#define SBHYBRID_WITH_EIP28
#define SBHYBRID_DO_DMARESOURCE_RELEASE
#endif
#ifdef SB_CM
#define SBHYBRID_WITH_EIP123
#define SBHYBRID_WITH_SWPK
#define SBHYBRID_DO_DMARESOURCE_RELEASE
#endif
#ifdef SB_CMPK
#define SBHYBRID_WITH_EIP123
#define SBHYBRID_WITH_EIP28
#define SBHYBRID_DO_DMARESOURCE_RELEASE
#endif
#ifdef SB_SM
#define SBHYBRID_WITH_EIP130
#define SBHYBRID_WITH_EIP130PK
#define SBHYBRID_DO_DMARESOURCE_RELEASE
#endif

#if defined(SBHYBRID_WITH_SW) + defined(SBHYBRID_WITH_EIP93) + \
    defined(SBHYBRID_WITH_EIP123) + defined(SBHYBRID_WITH_EIP130) != 1
#error "Define one of: SBHYBRID_WITH_SW, SBHYBRID_WITH_EIP93, SBHYBRID_WITH_EIP123 or SBHYBRID_WITH_EIP130 ."
#endif /* SBHYBRID_WITH_* */

#if defined(SBHYBRID_WITH_SWPK) + defined(SBHYBRID_WITH_EIP28) + \
    defined(SBHYBRID_WITH_EIP130PK) != 1
#error "Define one of: SBHYBRID_WITH_SWPK, SBHYBRID_WITH_EIP28 or SBHYBRID_WITH_EIP130PK."
#endif /* SBHYBRID_WITH_* */

#if (defined(SBHYBRID_WITH_EIP130PK) && !defined(SBHYBRID_WITH_EIP130)) || \
    (defined(SBHYBRID_WITH_EIP130) && !defined(SBHYBRID_WITH_EIP130PK))
#error "SBHYBRID_WITH_EIP130PK must be used in combination with SBHYBRID_WITH_EIP130."
#endif /* SBHYBRID_WITH_* */

#if SBIF_ECDSA_WORDS != 7 && SBIF_ECDSA_WORDS != 8 && SBIF_ECDSA_WORDS != 12 && SBIF_ECDSA_WORDS != 17
#error "Unsupported SBIF_ECDSA_WORDS"
#else
#if SBIF_ECDSA_WORDS == 17
#define SBHYBRID_DIGEST_BYTES           64
#else
#define SBHYBRID_DIGEST_BYTES           SBIF_ECDSA_BYTES
#endif
#endif

#ifdef SBHYBRID_WITH_SW
#include "sb_sw_hash.h"
#endif /* SBHYBRID_WITH_SW */

#ifdef SBHYBRID_WITH_SWPK
#include "sbhybrid_sw_ecdsa_verify.h"
#endif /* SBHYBRID_WITH_SWPK */

#ifdef SBHYBRID_WITH_EIP28
#include "sbhybrid_eip28_ecdsa_verify.h"
#endif /* SBHYBRID_WITH_EIP28 */

#ifdef SBHYBRID_WITH_EIP93
#include "dmares_buf.h"
#include "dmares_types.h"          // DMAResource_Handle_t
#include "eip93.h"

#define SBHYBRID_SASTATE_BYTES              ((32 + 14) * 4) // size of SA + State
#define SBHYBRID_SASTATE_WORDS              (SBHYBRID_SASTATE_BYTES / 4)
#define SBHYBRID_SASTATE_WORDOFFSET_KEY     (2)
#define SBHYBRID_SASTATE_WORDOFFSET_IV      (32 + 0)
#define SBHYBRID_SASTATE_WORDOFFSET_DIGEST  (32 + 6)
/* These are for single block ECB: State is reused as single block buffer. */
#define SBHYBRID_SASTATE_WORDOFFSET_INPUT   (32 + 0)
#define SBHYBRID_SASTATE_WORDOFFSET_OUTPUT  SBHYBRID_SASTATE_WORDOFFSET_IV

#define SBHYBRID_MAX_SIZE_DATA_BLOCKS       (0x3FFF * 64)
#define SBHYBRID_MAX_SIZE_DATA_BYTES        0x0FFFFF
#define SBHYBRID_MAX_DESCRIPTORS_PER_VECTOR (SBLIB_CFG_DATASIZE_MAX / SBHYBRID_MAX_SIZE_DATA_BLOCKS)

#define SA_STATE_HASH 0
#define SA_STATE_CIPHER 1
#define SA_STATE_CIPHER_OFFSET (SBHYBRID_SASTATE_BYTES / 4)

#endif /* SBHYBRID_WITH_EIP93 */

#ifdef SBHYBRID_WITH_EIP123
#include "eip123.h"
#include "eip123_dma.h"

#define SBHYBRID_MAX_SIZE_DATA_BLOCKS       (0x3FFF * 64)
#define SBHYBRID_MAX_SIZE_DATA_BYTES        0x1FFFFF
#define SBHYBRID_MAX_DMA_HANDLES            (SBLIB_CFG_DATASIZE_MAX / SBHYBRID_MAX_SIZE_DATA_BYTES)

#endif /* SBHYBRID_WITH_EIP123 */

#ifdef SBHYBRID_WITH_EIP130
#include "dmares_buf.h"
#include "dmares_types.h"          // DMAResource_Handle_t
#include "eip130.h"

#if (SBIF_ECDSA_BYTES <= 32)
#define SBHYBRID_HASH_BLOCK (64)
#else
#define SBHYBRID_HASH_BLOCK (128)
#endif

#define SBHYBRID_MAX_SIZE_LAST_DATA_BLOCK   (2048)
#define SBHYBRID_MAX_SIZE_DATA_BLOCKS       (0x3FFF * SBHYBRID_HASH_BLOCK)
#define SBHYBRID_MAX_SIZE_DATA_BYTES        0x1FFFFF
#define SBHYBRID_MAX_DMA_HANDLES            (SBLIB_CFG_DATASIZE_MAX / SBHYBRID_MAX_SIZE_DATA_BYTES)

#ifdef SBHYBRID_WITH_EIP130PK
#define SBHYBRID_IMMEDIATE_CERTIFICATE_CHECK
#endif

#endif /* SBHYBRID_WITH_EIP130 */

#ifdef SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
#undef  SBLIB_CFG_CERTIFICATES_MAX
#define SBLIB_CFG_CERTIFICATES_MAX 0
#endif

// Use count of error codes as internal PENDING status return code
#define SBHYBRID_PENDING SB_ERROR_COUNT

typedef struct
{
#ifdef SBHYBRID_WITH_SW
    void * AesIfCtx[100 / sizeof(void *)];
    SB_SW_HASH_Context_t hash_ctx;
    uint32_t hash_out[16];               // Reserved for 512 bit (SHA-224, SHA-256, SHA-384 and SHA-512 will fit)
#endif /* SBHYBRID_WITH_SW */
    uint8_t Digest[SBIF_ECDSA_BYTES]; // plain 8bit byte array format
#ifndef SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT
    uint8_t CertDigest[SBLIB_CFG_CERTIFICATES_MAX][SBIF_ECDSA_BYTES];
#endif /* !SBLIB_CF_REMOVE_CERTIFICATE_SUPPORT */
#ifdef SBHYBRID_WITH_EIP93
    unsigned int PendingCount;
    Device_Handle_t Device_EIP93;
    EIP93_IOArea_t EIP93_IOArea;
    struct
    {
        DMAResource_Handle_t Handle;
        uint32_t   SA_PhysAddr;
        uint32_t   State_PhysAddr;      // state follows SA
        uint32_t * SA_Host_p;
        uint32_t * State_Host_p;        // state follows SA
    } SA_States[2];
    struct
    {
        /* Handles: According to Maximum Ring Size * 1,5, because
           half of the operations have both input and output handle.
           +1 for the ring itself. */
        DMAResource_Handle_t Handles[((SBHYBRID_MAX_DESCRIPTORS_PER_VECTOR + 2)* 3)+ 1];
        unsigned int Count;
    } DMAHandles;
#endif /* SBHYBRID_WITH_EIP93 */
#ifdef SBHYBRID_WITH_EIP123
    Device_Handle_t Device_EIP123;

    uint8_t  hash_initialized;
    uint8_t  hash_finalize;
    struct
    {
        uint32_t DataLen;
        uint32_t TotalLen;

        DMAResource_Handle_t      DMAHandle;
        EIP123_DescriptorChain_t  DC;
        uint32_t                  PhysAddr;
    } DC_Hash;
    struct
    {
        DMAResource_Handle_t      DMAHandle;
        EIP123_DescriptorChain_t  DC;
        uint32_t                  PhysAddr;
    } DC_CipherIn;
    struct
    {
        DMAResource_Handle_t      DMAHandle;
        EIP123_DescriptorChain_t  DC;
        uint32_t                  PhysAddr;
    } DC_CipherOut;
    struct
    {
        // this buffer is used for LoadKey and EncryptIV
        // it also has extra space for the TokenID
        // the size is 48 bytes
        DMAResource_Handle_t      DMAHandle;
        uint32_t                  PhysAddr;
        uint8_t *                 Host_p;
    } SmallDMABuf;
    uint32_t DMAHandleCount;
    DMAResource_Handle_t DMAHandles[(SBHYBRID_MAX_DMA_HANDLES + 1)* 2];
    EIP123_Fragment_t Frags[SBLIB_CFG_DATAFRAGMENTS_MAX + 1];
    uint32_t FragCount;
    uint8_t Hash[32];
#endif /* SBHYBRID_WITH_EIP123 */
#ifdef SBHYBRID_WITH_EIP130
    Device_Handle_t   Device_EIP130;
    uint16_t          InitCount;
    uint16_t          TokenId;

    uint32_t DMAHandleCount;
    DMAResource_Handle_t DMAHandles[(SBHYBRID_MAX_DMA_HANDLES + 1)* 2];

    // Members related to the hash operation
    bool HashInitialize;
    bool HashFinalize;
    bool HashDigestValid;
    uint32_t HashTempAssetId;
    uint64_t HashTotalLength;
#if (SBIF_ECDSA_BYTES <= 32)
    uint8_t Hash[32];
#else
    uint8_t Hash[64];
#endif
#ifdef SBHYBRID_WITH_EIP130PK
    const uint8_t * HashData_p;
    uint16_t HashDataLength;

    // Members related to the ECDSA verify operation
    uint32_t KeyAssetId;
    uint32_t ParamsAssetId;
    uint32_t ParamsInBytes;
    uint32_t PublicKeyInBytes;
    uint32_t SignatureInBytes;
#ifdef SBLIB_CFG_SM_COPY_CURVEPARAMS
    uint8_t * Params;
    uint8_t  PublicKeyParams_Buffer[(6 * (4 + SBIF_ECDSA_WORDS * 4)) + 8];
#else
    const uint8_t * Params;
#endif /* SBLIB_CFG_SM_COPY_CURVEPARAMS */
    uint8_t  PublicKey_Buffer[2 * (4 + SBIF_ECDSA_WORDS * 4)];
    uint8_t  Signature_Buffer[2 * (4 + SBIF_ECDSA_WORDS * 4)];
#endif /* SBHYBRID_WITH_EIP130PK */
#endif /* SBHYBRID_WITH_EIP130 */
} SBHYBRID_SymmContext_t;


#ifdef SBHYBRID_WITH_SW
static inline SB_Result_t
SBHYBRID_SW_SHA512_Init(SBHYBRID_SymmContext_t * const Hash_p)
{
    SB_SW_HASH_Init(&Hash_p->hash_ctx, 512);
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA512_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                            const uint8_t * DataBytes_p,
                            unsigned int DataByteCount,
                            bool fFinal)
{
    if (fFinal)
    {
        SB_SW_HASH_FinalUpdate(&Hash_p->hash_ctx,
                               DataBytes_p, DataByteCount,
                               Hash_p->hash_out);
    }
    else
    {
        SB_SW_HASH_Update(&Hash_p->hash_ctx, DataBytes_p, DataByteCount);
    }
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA512_RunFsm(SBHYBRID_SymmContext_t * const Hash_p)
{
    IDENTIFIER_NOT_USED(Hash_p);
    return SB_SUCCESS;                  /* There is never pending operations. */
}

static inline void
SBHYBRID_SW_SHA512_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                             uint8_t * Digest_p)
{
    int i = 0;
    for (i = 0; i < 16; i++)
    {
        uint32_t word = Hash_p->hash_out[15 - i];
        Digest_p[0] = word >> 24;
        Digest_p[1] = (word >> 16) & 255;
        Digest_p[2] = (word >> 8) & 255;
        Digest_p[3] = word & 255;
        Digest_p += 4;
    }
}

static inline SB_Result_t
SBHYBRID_SW_SHA384_Init(SBHYBRID_SymmContext_t * const Hash_p)
{
    SB_SW_HASH_Init(&Hash_p->hash_ctx, 384);
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA384_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                            const uint8_t * DataBytes_p,
                            unsigned int DataByteCount,
                            bool fFinal)
{
    if (fFinal)
    {
        SB_SW_HASH_FinalUpdate(&Hash_p->hash_ctx,
                               DataBytes_p, DataByteCount,
                               Hash_p->hash_out);
    }
    else
    {
        SB_SW_HASH_Update(&Hash_p->hash_ctx, DataBytes_p, DataByteCount);
    }
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA384_RunFsm(SBHYBRID_SymmContext_t * const Hash_p)
{
    IDENTIFIER_NOT_USED(Hash_p);
    return SB_SUCCESS;                  /* There is never pending operations. */
}

static inline void
SBHYBRID_SW_SHA384_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                             uint8_t * Digest_p)
{
    int i = 0;
    for (i = 0; i < 12; i++)
    {
        uint32_t word = Hash_p->hash_out[11 - i];
        Digest_p[0] = word >> 24;
        Digest_p[1] = (word >> 16) & 255;
        Digest_p[2] = (word >> 8) & 255;
        Digest_p[3] = word & 255;
        Digest_p += 4;
    }
}

static inline SB_Result_t
SBHYBRID_SW_SHA256_Init(SBHYBRID_SymmContext_t * const Hash_p)
{
    SB_SW_HASH_Init(&Hash_p->hash_ctx, 256);
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA256_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                            const uint8_t * DataBytes_p,
                            unsigned int DataByteCount,
                            bool fFinal)
{
    if (fFinal)
    {
        SB_SW_HASH_FinalUpdate(&Hash_p->hash_ctx,
                               DataBytes_p, DataByteCount,
                               Hash_p->hash_out);
    }
    else
    {
        SB_SW_HASH_Update(&Hash_p->hash_ctx, DataBytes_p, DataByteCount);
    }
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA256_RunFsm(SBHYBRID_SymmContext_t * const Hash_p)
{
    IDENTIFIER_NOT_USED(Hash_p);
    return SB_SUCCESS;                  /* There is never pending operations. */
}

static inline void
SBHYBRID_SW_SHA256_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                             uint8_t * Digest_p)
{
    int i = 0;
    for (i = 0; i < 8; i++)
    {
        uint32_t word = Hash_p->hash_out[7 - i];
        Digest_p[0] = word >> 24;
        Digest_p[1] = (word >> 16) & 255;
        Digest_p[2] = (word >> 8) & 255;
        Digest_p[3] = word & 255;
        Digest_p += 4;
    }
}

static inline SB_Result_t
SBHYBRID_SW_SHA224_Init(SBHYBRID_SymmContext_t * const Hash_p)
{
    SB_SW_HASH_Init(&Hash_p->hash_ctx, 224);
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA224_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                            const uint8_t * DataBytes_p,
                            unsigned int DataByteCount,
                            bool fFinal)
{
    if (fFinal)
    {
        SB_SW_HASH_FinalUpdate(&Hash_p->hash_ctx,
                               DataBytes_p, DataByteCount,
                               Hash_p->hash_out);
    }
    else
    {
        SB_SW_HASH_Update(&Hash_p->hash_ctx, DataBytes_p, DataByteCount);
    }
    return SB_SUCCESS;
}

static inline SB_Result_t
SBHYBRID_SW_SHA224_RunFsm(SBHYBRID_SymmContext_t * const Hash_p)
{
    IDENTIFIER_NOT_USED(Hash_p);
    return SB_SUCCESS;                  /* There is never pending operations. */
}

static inline void
SBHYBRID_SW_SHA224_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                             uint8_t * Digest_p)
{
    int i = 0;
    for (i = 0; i < 7; i++)
    {
        uint32_t word = Hash_p->hash_out[6 - i];
        Digest_p[0] = word >> 24;
        Digest_p[1] = (word >> 16) & 255;
        Digest_p[2] = (word >> 8) & 255;
        Digest_p[3] = word & 255;
        Digest_p += 4;
    }
}
#endif /* SBHYBRID_WITH_SW */

#ifdef SBHYBRID_WITH_EIP93
SB_Result_t
SBHYBRID_EIP93_SHA256_Init(SBHYBRID_SymmContext_t * const Hash_p);

SB_Result_t
SBHYBRID_EIP93_SHA256_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                               const uint8_t * DataBytes_p,
                               unsigned int DataByteCount,
                               bool fFinal);

SB_Result_t
SBHYBRID_EIP93_SHA256_RunFsm(SBHYBRID_SymmContext_t * const Hash_p);

void
SBHYBRID_EIP93_SHA256_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                                uint8_t * Digest_p);

SB_Result_t
SBHYBRID_EIP93_SHA224_Init(SBHYBRID_SymmContext_t * const Hash_p);

SB_Result_t
SBHYBRID_EIP93_SHA224_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                               const uint8_t * DataBytes_p,
                               unsigned int DataByteCount,
                               bool fFinal);

SB_Result_t
SBHYBRID_EIP93_SHA224_RunFsm(SBHYBRID_SymmContext_t * const Hash_p);

void
SBHYBRID_EIP93_SHA224_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                                uint8_t * Digest_p);
#endif /* SBHYBRID_WITH_EIP93 */

/* Common function for handling EIP123 result tokens. */
#ifdef SBHYBRID_WITH_EIP123
SB_Result_t
SBHYBRID_EIP123_Common_ReadResultAndCheckFunc(Device_Handle_t const Device_EIP123,
                                              CMTokens_Response_t * t_res_p,
                                              uint8_t MailboxNr);

SB_Result_t
SBHYBRID_EIP123_Common_ReadResultAndCheckFuncDebug(Device_Handle_t const Device_EIP123,
                                                   CMTokens_Response_t * t_res_p,
                                                   uint8_t MailboxNr,
                                                   const char * OperationStr_p);

#ifdef IMPLDEFS_CF_DISABLE_L_DEBUG
#define SBHYBRID_EIP123_Common_ReadResultAndCheck(_p1, _p2, _p3, _p4)       \
    SBHYBRID_EIP123_Common_ReadResultAndCheckFunc(_p1, _p2, _p3)
#else
#define SBHYBRID_EIP123_Common_ReadResultAndCheck(_p1, _p2, _p3, _p4)       \
    SBHYBRID_EIP123_Common_ReadResultAndCheckFuncDebug(_p1, _p2, _p3, _p4)
#endif

SB_Result_t
SBHYBRID_EIP123_SHA256_Init(SBHYBRID_SymmContext_t * const Hash_p);

SB_Result_t
SBHYBRID_EIP123_SHA256_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                                const uint8_t * DataBytes_p,
                                unsigned int DataByteCount,
                                bool fFinal);

SB_Result_t
SBHYBRID_EIP123_SHA256_RunFsm(SBHYBRID_SymmContext_t * const Hash_p);

void
SBHYBRID_EIP123_SHA256_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                                 uint8_t * Digest_p);

void
SBHYBRID_EIP123_SHA256_DmaRelease(SBHYBRID_SymmContext_t * const Hash_p);

SB_Result_t
SBHYBRID_EIP123_SHA224_Init(SBHYBRID_SymmContext_t * const Hash_p);

SB_Result_t
SBHYBRID_EIP123_SHA224_AddBlock(SBHYBRID_SymmContext_t * const Hash_p,
                                const uint8_t * DataBytes_p,
                                unsigned int DataByteCount,
                                bool fFinal);

SB_Result_t
SBHYBRID_EIP123_SHA224_RunFsm(SBHYBRID_SymmContext_t * const Hash_p);

void
SBHYBRID_EIP123_SHA224_GetDigest(SBHYBRID_SymmContext_t * const Hash_p,
                                 uint8_t * Digest_p);

void
SBHYBRID_EIP123_SHA224_DmaRelease(SBHYBRID_SymmContext_t * const Hash_p);
#endif /* SBHYBRID_WITH_EIP123 */

/* Common function for handling EIP130 result tokens. */
#ifdef SBHYBRID_WITH_EIP130
SB_Result_t
SBHYBRID_EIP130_Common_WriteCommand(Device_Handle_t const Device_EIP130,
                                    Eip130Token_Command_t * t_cmd_p,
                                    uint8_t MailboxNr);

SB_Result_t
SBHYBRID_EIP130_Common_ReadResultAndCheck(Device_Handle_t const Device_EIP130,
                                          Eip130Token_Result_t * t_res_p,
                                          uint8_t MailboxNr);

uint64_t
SBHYBRID_EIP130_Common_GetDmaAddress(SBHYBRID_SymmContext_t * const Context_p,
                                     const void * Data_p,
                                     const uint32_t DataLength);

uint64_t
SBHYBRID_EIP130_Common_DmaAlloc(SBHYBRID_SymmContext_t * const Context_p,
                                const uint32_t DataLength,
                                void ** Host_pp);

void
SBHYBRID_EIP130_Common_DmaRelease(SBHYBRID_SymmContext_t * const Context_p);

void
SBHYBRID_EIP130_Common_PreDma(SBHYBRID_SymmContext_t * const Context_p,
                              const DMAResource_Handle_t Handle);

SB_Result_t
SBHYBRID_AssetCreate_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 uint64_t Policy,
                                 uint32_t AssetSizeInBytes);

SB_Result_t
SBHYBRID_AssetSearch_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 const uint32_t AssetNumber);

SB_Result_t
SBHYBRID_AssetCreateSearch_ReadResult(SBHYBRID_SymmContext_t * const Context_p,
                                      uint32_t * AssetId_p);

SB_Result_t
SBHYBRID_AssetDelete_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                 const uint32_t AssetId);

SB_Result_t
SBHYBRID_AssetDelete_ReadResult(SBHYBRID_SymmContext_t * const Context_p);

SB_Result_t
SBHYBRID_AssetLoadPlaintext_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                        const uint32_t AssetId,
                                        const uint8_t * Data_p,
                                        const uint16_t DataLengthInbytes);

SB_Result_t
SBHYBRID_AssetLoadAESUnwrap_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                        const uint32_t AssetId,
                                        const uint32_t KEKAssetId,
                                        const uint8_t * Data_p,
                                        const uint16_t DataLengthInbytes);

SB_Result_t
SBHYBRID_AssetLoadDerive_SendCommand(SBHYBRID_SymmContext_t * const Context_p,
                                     const uint32_t AssetId,
                                     const uint32_t KDKAssetId,
                                     const uint8_t * Aad_p,
                                     const uint16_t AadLengthInBytes);

SB_Result_t
SBHYBRID_AssetLoad_ReadResult(SBHYBRID_SymmContext_t * const Context_p);

SB_Result_t
SBHYBRID_EIP130_Hash_Init(SBHYBRID_SymmContext_t * const Context_p);

SB_Result_t
SBHYBRID_EIP130_Hash_AddBlock(SBHYBRID_SymmContext_t * const Context_p,
                              const uint8_t * DataBytes_p,
                              const uint32_t DataByteCount,
                              const bool fFinal);

SB_Result_t
SBHYBRID_EIP130_Hash_RunFsm(SBHYBRID_SymmContext_t * const Context_p);

void
SBHYBRID_EIP130_Hash_GetDigest(SBHYBRID_SymmContext_t * const Context_p,
                               uint8_t * Digest_p);

#endif /* SBHYBRID_WITH_EIP130 */

// Internal type of image used by PK Image processing code.
typedef enum
{
    SBHYBRID_IMAGE_NONE,
    SBHYBRID_IMAGE_P
}
SBHYBRID_ImageType_t;

/* SBHYBRID_Context_t

   Internal context containing all required work memory. The
   SB_StorageArea_t type pointer is casted to SBHYBRID_Context_t for
   internal use. The size must be smaller than SB_StorageArea_t.

   The structure needs to be identical to AES_IF_Ctx, to simplify internal
   operation of Secure Boot.
 */
struct AES_IF_Ctx
{
#if defined(SBHYBRID_WITH_SW) || defined(SBHYBRID_WITH_EIP93)
#ifdef SBIF_CFG_CONFIDENTIALITY_BITS
    uint8_t KeyData[SBIF_CFG_CONFIDENTIALITY_BITS / 8];
#else
    uint8_t KeyDataBuffer[256 / 8];
    uint32_t KeyDataBytes;
#endif
#endif
    AES_IF_ResultCode_t Result;
    uint32_t IV[4];

    /* Context for asymmetric and symmetric crypto */
#if defined(SBHYBRID_WITH_SWPK)
    SBHYBRID_ECDSA_Verify_t * EcdsaContext_p; /* Reference to SW context */
    SBHYBRID_ECDSA_Verify_t EcdsaContext; /* Context for asymmetric crypto */
#elif defined(SBHYBRID_WITH_EIP28)
    SBHYBRID_EcdsaContext_t * EcdsaContext_p; /* Reference to EIP-28 context */
    SBHYBRID_EcdsaContext_t EcdsaContext; /* Context for asymmetric crypto */
#elif defined(SBHYBRID_WITH_EIP130)
    SBHYBRID_SymmContext_t * EcdsaContext_p; /* Reference to context of EIP-130 */
#endif
    SBHYBRID_SymmContext_t SymmContext; /* Context for symmetric crypto */

#if defined(SBHYBRID_WITH_EIP123) || defined(SBHYBRID_WITH_EIP130)

    uint32_t AssetId;                   /* Asset Identifier */
    uint32_t AssetBits;                 /* Asset data size */
#if defined(SBHYBRID_WITH_EIP123)
    uint32_t AssetFilled;
#endif
#endif

    // Common variables
    int CertNr;
    int CertificateCount;
    int NeedFinalDigest;
};

typedef struct AES_IF_Ctx SBHYBRID_Context_t;

static inline uint32_t
SBHYBRID_Load_BE32(const void * const Value_p)
{
    const uint8_t * const p = (const uint8_t *)Value_p;

    return (p[0] << 24 | p[1] << 16 | p[2] << 8  | p[3]);
}


SB_Result_t
SBHYBRID_Initialize_HW(SBHYBRID_Context_t * const Context_p);

void
SBHYBRID_Uninitialize_HW(SBHYBRID_Context_t * const Context_p);

SB_Result_t
SBHYBRID_Initialize_FW(SBHYBRID_Context_t * const Context_p,
                       const uint32_t * const Firmware_p,
                       const uint32_t FirmwareWord32Size);


#ifdef IMPLDEFS_CF_DISABLE_L_TRACE

#define L_TRACE_DIGEST(_d)
#define L_TRACE_PUBKEY(_k)
#define L_TRACE_SIGNATURE(_s)

#elif SBIF_ECDSA_WORDS == 8

#define L_TRACE_DIGEST(_d) L_TRACE(LF_SBHYBRID, \
            "Digest: " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X ", \
            (_d)[0],  (_d)[1],  (_d)[2],  (_d)[3],  (_d)[4],  (_d)[5],  (_d)[6],  (_d)[7],  \
            (_d)[8],  (_d)[9],  (_d)[10], (_d)[11], (_d)[12], (_d)[13], (_d)[14], (_d)[15], \
            (_d)[16], (_d)[17], (_d)[18], (_d)[19], (_d)[20], (_d)[21], (_d)[22], (_d)[23], \
            (_d)[24], (_d)[25], (_d)[26], (_d)[27], (_d)[28], (_d)[29], (_d)[30], (_d)[31])

#define L_TRACE_PUBKEY(_k) L_TRACE(LF_SBHYBRID, \
            "PubKey: " \
            "Qx{" \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X} " \
            "Qy{" \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X}", \
            (_k)->Qx[0],  (_k)->Qx[1],  (_k)->Qx[2],  (_k)->Qx[3],  (_k)->Qx[4],  (_k)->Qx[5],  (_k)->Qx[6],  (_k)->Qx[7],  \
            (_k)->Qx[8],  (_k)->Qx[9],  (_k)->Qx[10], (_k)->Qx[11], (_k)->Qx[12], (_k)->Qx[13], (_k)->Qx[14], (_k)->Qx[15], \
            (_k)->Qx[16], (_k)->Qx[17], (_k)->Qx[18], (_k)->Qx[19], (_k)->Qx[20], (_k)->Qx[21], (_k)->Qx[22], (_k)->Qx[23], \
            (_k)->Qx[24], (_k)->Qx[25], (_k)->Qx[26], (_k)->Qx[27], (_k)->Qx[28], (_k)->Qx[29], (_k)->Qx[30], (_k)->Qx[31], \
            (_k)->Qy[0],  (_k)->Qy[1],  (_k)->Qy[2],  (_k)->Qy[3],  (_k)->Qy[4],  (_k)->Qy[5],  (_k)->Qy[6],  (_k)->Qy[7],  \
            (_k)->Qy[8],  (_k)->Qy[9],  (_k)->Qy[10], (_k)->Qy[11], (_k)->Qy[12], (_k)->Qy[13], (_k)->Qy[14], (_k)->Qy[15], \
            (_k)->Qy[16], (_k)->Qy[17], (_k)->Qy[18], (_k)->Qy[19], (_k)->Qy[20], (_k)->Qy[21], (_k)->Qy[22], (_k)->Qy[23], \
            (_k)->Qy[24], (_k)->Qy[25], (_k)->Qy[26], (_k)->Qy[27], (_k)->Qy[28], (_k)->Qy[29], (_k)->Qy[30], (_k)->Qy[31])

#define L_TRACE_SIGNATURE(_s) L_TRACE(LF_SBHYBRID, \
            "Signature: " \
            "r{" \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X} " \
            "s{" \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X %02X}", \
            (_s)->r[0],  (_s)->r[1],  (_s)->r[2],  (_s)->r[3],  (_s)->r[4],  (_s)->r[5],  (_s)->r[6],  (_s)->r[7],  \
            (_s)->r[8],  (_s)->r[9],  (_s)->r[10], (_s)->r[11], (_s)->r[12], (_s)->r[13], (_s)->r[14], (_s)->r[15], \
            (_s)->r[16], (_s)->r[17], (_s)->r[18], (_s)->r[19], (_s)->r[20], (_s)->r[21], (_s)->r[22], (_s)->r[23], \
            (_s)->r[24], (_s)->r[25], (_s)->r[26], (_s)->r[27], (_s)->r[28], (_s)->r[29], (_s)->r[30], (_s)->r[31], \
            (_s)->s[0],  (_s)->s[1],  (_s)->s[2],  (_s)->s[3],  (_s)->s[4],  (_s)->s[5],  (_s)->s[6],  (_s)->s[7],  \
            (_s)->s[8],  (_s)->s[9],  (_s)->s[10], (_s)->s[11], (_s)->s[12], (_s)->s[13], (_s)->s[14], (_s)->s[15], \
            (_s)->s[16], (_s)->s[17], (_s)->s[18], (_s)->s[19], (_s)->s[20], (_s)->s[21], (_s)->s[22], (_s)->s[23], \
            (_s)->s[24], (_s)->s[25], (_s)->s[26], (_s)->s[27], (_s)->s[28], (_s)->s[29], (_s)->s[30], (_s)->s[31])

#elif SBIF_ECDSA_WORDS == 7

#define L_TRACE_DIGEST(_d) L_TRACE(LF_SBHYBRID, \
            "Digest: " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X", \
            (_d)[0],  (_d)[1],  (_d)[2],  (_d)[3],  (_d)[4],  (_d)[5],  (_d)[6],  \
            (_d)[7],  (_d)[8],  (_d)[9],  (_d)[10], (_d)[11], (_d)[12], (_d)[13], \
            (_d)[14], (_d)[15], (_d)[16], (_d)[17], (_d)[18], (_d)[19], (_d)[20], \
            (_d)[21], (_d)[22], (_d)[23], (_d)[24], (_d)[25], (_d)[26], (_d)[27])
#define L_TRACE_PUBKEY(_k) L_TRACE(LF_SBHYBRID, \
            "PubKey: " \
            "x{%08X %08X %08X %08X %08X %08X %08X}" \
            "y{%08X %08X %08X %08X %08X %08X %08X}", \
            (_k)->Qx[0],  (_k)->Qx[1],  (_k)->Qx[2],  (_k)->Qx[3],  (_k)->Qx[4],  (_k)->Qx[5],  (_k)->Qx[6],  \
            (_k)->Qy[0],  (_k)->Qy[1],  (_k)->Qy[2],  (_k)->Qy[3],  (_k)->Qy[4],  (_k)->Qy[5],  (_k)->Qy[6])

#define L_TRACE_SIGNATURE(_s) L_TRACE(LF_SBHYBRID, \
            "Signature: " \
            "r{" \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X} " \
            "s{" \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X " \
            "%02X %02X %02X %02X %02X %02X %02X}", \
            (_s)->r[0],  (_s)->r[1],  (_s)->r[2],  (_s)->r[3],  (_s)->r[4],  (_s)->r[5],  (_s)->r[6],  \
            (_s)->r[7],  (_s)->r[8],  (_s)->r[9],  (_s)->r[10], (_s)->r[11], (_s)->r[12], (_s)->r[13], \
            (_s)->r[14], (_s)->r[15], (_s)->r[16], (_s)->r[17], (_s)->r[18], (_s)->r[19], (_s)->r[20], \
            (_s)->r[21], (_s)->r[22], (_s)->r[23], (_s)->r[24], (_s)->r[25], (_s)->r[26], (_s)->r[27], \
            (_s)->s[0],  (_s)->s[1],  (_s)->s[2],  (_s)->s[3],  (_s)->s[4],  (_s)->s[5],  (_s)->s[6],  \
            (_s)->s[7],  (_s)->s[8],  (_s)->s[9],  (_s)->s[10], (_s)->s[11], (_s)->s[12], (_s)->s[13], \
            (_s)->s[14], (_s)->s[15], (_s)->s[16], (_s)->s[17], (_s)->s[18], (_s)->s[19], (_s)->s[20], \
            (_s)->s[21], (_s)->s[22], (_s)->s[23], (_s)->s[24], (_s)->s[25], (_s)->s[26], (_s)->s[27])

#endif

/* Choose SHA implementation to use according to SBIF_ECDSA_WORDS
   (SHA-224 or SHA-256). */
#if defined(SBHYBRID_WITH_SW)
/* No DMA contexts to free. */
#define SBHYBRID_SHA2XX_DmaRelease(SymmContext_p) do { } while(0)

#if SBIF_ECDSA_WORDS == 17
#define SBHYBRID_SHA2XX_Init      SBHYBRID_SW_SHA512_Init
#define SBHYBRID_SHA2XX_AddBlock  SBHYBRID_SW_SHA512_AddBlock
#define SBHYBRID_SHA2XX_RunFsm    SBHYBRID_SW_SHA512_RunFsm
#define SBHYBRID_SHA2XX_GetDigest SBHYBRID_SW_SHA512_GetDigest
#elif SBIF_ECDSA_WORDS == 12
#define SBHYBRID_SHA2XX_Init      SBHYBRID_SW_SHA384_Init
#define SBHYBRID_SHA2XX_AddBlock  SBHYBRID_SW_SHA384_AddBlock
#define SBHYBRID_SHA2XX_RunFsm    SBHYBRID_SW_SHA384_RunFsm
#define SBHYBRID_SHA2XX_GetDigest SBHYBRID_SW_SHA384_GetDigest
#elif SBIF_ECDSA_WORDS == 8
#define SBHYBRID_SHA2XX_Init      SBHYBRID_SW_SHA256_Init
#define SBHYBRID_SHA2XX_AddBlock  SBHYBRID_SW_SHA256_AddBlock
#define SBHYBRID_SHA2XX_RunFsm    SBHYBRID_SW_SHA256_RunFsm
#define SBHYBRID_SHA2XX_GetDigest SBHYBRID_SW_SHA256_GetDigest
#elif SBIF_ECDSA_WORDS == 7
#define SBHYBRID_SHA2XX_Init      SBHYBRID_SW_SHA224_Init
#define SBHYBRID_SHA2XX_AddBlock  SBHYBRID_SW_SHA224_AddBlock
#define SBHYBRID_SHA2XX_RunFsm    SBHYBRID_SW_SHA224_RunFsm
#define SBHYBRID_SHA2XX_GetDigest SBHYBRID_SW_SHA224_GetDigest
#endif
#define SBHYBRID_TEST_ImageLenLeft(x) ((x) != 0)

#elif defined(SBHYBRID_WITH_EIP93)
// free all DMA handles, except
// SymmContext_p->SA_States[SA_STATE_CIPHER].Handle
// and RingMemory.CommandRingHandle
// which is SymmContext_p->DMAHandles.Handles[0]
#define SBHYBRID_SHA2XX_DmaRelease(Context_p)                   \
    while((Context_p)->DMAHandles.Count > 1)                    \
    {                                                           \
        (Context_p)->DMAHandles.Count--;                        \
        DMAResource_Release((Context_p)->DMAHandles.Handles[(Context_p)->DMAHandles.Count]); \
    }

#if SBIF_ECDSA_WORDS == 8
#define SBHYBRID_SHA2XX_Init       SBHYBRID_EIP93_SHA256_Init
#define SBHYBRID_SHA2XX_AddBlock   SBHYBRID_EIP93_SHA256_AddBlock
#define SBHYBRID_SHA2XX_RunFsm     SBHYBRID_EIP93_SHA256_RunFsm
#define SBHYBRID_SHA2XX_GetDigest  SBHYBRID_EIP93_SHA256_GetDigest
#elif SBIF_ECDSA_WORDS == 7
#define SBHYBRID_SHA2XX_Init       SBHYBRID_EIP93_SHA224_Init
#define SBHYBRID_SHA2XX_AddBlock   SBHYBRID_EIP93_SHA224_AddBlock
#define SBHYBRID_SHA2XX_RunFsm     SBHYBRID_EIP93_SHA224_RunFsm
#define SBHYBRID_SHA2XX_GetDigest  SBHYBRID_EIP93_SHA224_GetDigest
#endif /* SBIF_ECDSA_WORDS */
#define SBHYBRID_TEST_ImageLenLeft(x) ((x) != 0)

#elif defined(SBHYBRID_WITH_EIP123)
#if SBIF_ECDSA_WORDS == 8
#define SBHYBRID_SHA2XX_Init       SBHYBRID_EIP123_SHA256_Init
#define SBHYBRID_SHA2XX_AddBlock   SBHYBRID_EIP123_SHA256_AddBlock
#define SBHYBRID_SHA2XX_RunFsm     SBHYBRID_EIP123_SHA256_RunFsm
#define SBHYBRID_SHA2XX_GetDigest  SBHYBRID_EIP123_SHA256_GetDigest
#define SBHYBRID_SHA2XX_DmaRelease SBHYBRID_EIP123_SHA256_DmaRelease
#elif SBIF_ECDSA_WORDS == 7
#define SBHYBRID_SHA2XX_Init       SBHYBRID_EIP123_SHA224_Init
#define SBHYBRID_SHA2XX_AddBlock   SBHYBRID_EIP123_SHA224_AddBlock
#define SBHYBRID_SHA2XX_RunFsm     SBHYBRID_EIP123_SHA224_RunFsm
#define SBHYBRID_SHA2XX_GetDigest  SBHYBRID_EIP123_SHA224_GetDigest
#define SBHYBRID_SHA2XX_DmaRelease SBHYBRID_EIP123_SHA224_DmaRelease
#endif /* SBIF_ECDSA_WORDS */
#define SBHYBRID_TEST_ImageLenLeft(x) ((x) != 0)

#elif defined(SBHYBRID_WITH_EIP130)
#define SBHYBRID_SHA2XX_Init       SBHYBRID_EIP130_Hash_Init
#define SBHYBRID_SHA2XX_AddBlock   SBHYBRID_EIP130_Hash_AddBlock
#define SBHYBRID_SHA2XX_RunFsm     SBHYBRID_EIP130_Hash_RunFsm
#define SBHYBRID_SHA2XX_GetDigest  SBHYBRID_EIP130_Hash_GetDigest
#define SBHYBRID_SHA2XX_DmaRelease SBHYBRID_EIP130_Common_DmaRelease
#define SBHYBRID_TEST_ImageLenLeft(x) ((x) != 0) //  > 4095
#endif /* SBHYBRID_WITH_xxx */

/* Choose Public Key implementation according to options. */
#if defined(SBHYBRID_WITH_SWPK)
#define SBHYBRID_Verify_Init      SBHYBRID_SW_Ecdsa_Verify_Init
#define SBHYBRID_Verify_RunFsm    SBHYBRID_SW_Ecdsa_Verify_RunFsm
#define SBHYBRID_Verify_SetDigest SBHYBRID_SW_Ecdsa_Verify_SetDigest

#elif defined(SBHYBRID_WITH_EIP28)
#define SBHYBRID_Verify_Init      SBHYBRID_EIP28_EcdsaVerify_Init
#define SBHYBRID_Verify_RunFsm    SBHYBRID_EIP28_EcdsaVerify_RunFsm
#define SBHYBRID_Verify_SetDigest SBHYBRID_EIP28_EcdsaVerify_SetDigest

#elif defined(SBHYBRID_WITH_EIP130PK)
#define SBHYBRID_Verify_Init      SBHYBRID_EIP130_EcdsaVerify_Init
#define SBHYBRID_Verify_RunFsm    SBHYBRID_EIP130_EcdsaVerify_RunFsm
#define SBHYBRID_Verify_SetDigest SBHYBRID_EIP130_EcdsaVerify_SetDigest

void
SBHYBRID_EIP130_EcdsaVerify_Init(SBHYBRID_SymmContext_t * const Context_p,
                                 const SBIF_ECDSA_PublicKey_t * const PublicKey_p,
                                 const SBIF_ECDSA_Signature_t * const Signature_p);

SB_Result_t
SBHYBRID_EIP130_EcdsaVerify_RunFsm(SBHYBRID_SymmContext_t * const Context_p);

void
SBHYBRID_EIP130_EcdsaVerify_SetDigest(SBHYBRID_SymmContext_t * const Context_p,
                                      uint8_t * Digest_p);

#endif /* SBHYBRID_WITH_XXX */

#endif /* Include Guard */

/* end of file sbhybrid_internal.h */
