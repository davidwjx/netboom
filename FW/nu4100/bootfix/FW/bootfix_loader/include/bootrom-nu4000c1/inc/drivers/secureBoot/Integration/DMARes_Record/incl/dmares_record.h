/* dmares_record.h
 *
 * DMAResource Record Definition.
 *
 * The document "Driver Framework Porting Guide" contains the detailed
 * specification of this API.
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

#include "umdevxsproxy_shmem.h" // UMDevXSProxy_SHMem_Handle_t

/*----------------------------------------------------------------------------
 * AddrTrans_Domain_t
 *
 * This is a list of domains that can be supported by the implementation. The
 * exact meaning can be different for different EIP devices and different
 * environments.
 */
enum
{
    DMARES_DOMAIN_UNKNOWN = 0,
    DMARES_DOMAIN_HOST,
    DMARES_DOMAIN_HOST_UNALIGNED,
    DMARES_DOMAIN_BUS,
    DMARES_DOMAIN_INTERHOST,
    DMARES_DOMAIN_EIP12xDMA,
    DMARES_DOMAIN_EIP13xDMA,
#ifndef CFG_ENABLE_TARGET_VTBAL
    DMARES_DOMAIN_EIP93DMA,
#endif
    DMARES_DOMAIN_ALTERNATIVE,
} AddrTrans_Domain_t;

#ifdef CFG_ENABLE_TARGET_VTBAL
#define DMARES_DOMAIN_EIP93DMA DMARES_DOMAIN_BUS
#endif

// Maximum number of address/domain pairs stored per DMA resource.
#define DMARES_ADDRPAIRS_CAPACITY 3

typedef struct
{
    // signature used to validate handles
    uint32_t Magic;

    // DMA resource properties: Size, Alignment, Bank & fCached
    DMAResource_Properties_t Props;

    // Storage for upto N address/domain pairs.
    DMAResource_AddrPair_t AddrPairs[DMARES_ADDRPAIRS_CAPACITY];

    // if true, 32-bit words are swapped when transferred to/from
    // the DMA resource
    bool fSwapEndianess;

    // this implementation supports the following allocator references:
    // 'A' -> this DMA resource has been obtained through DMAResource_Alloc
    // 'R' -> this DMA resource has been obtained through DMAResource_Register
    // 'T' -> this DMA resource has been obtained through DMAResource_Attach
    char AllocatorRef;

    // maximum data amount that can be stored in bytes, e.g. allocated size
    unsigned int BufferSize;

    // kernel driver handle for this DMA resource
    UMDevXSProxy_SHMem_Handle_t DriverHandle;

} DMAResource_Record_t;

#define DMARES_RECORD_MAGIC 0xde42b5e7

/* end of file dmares_record.h */
