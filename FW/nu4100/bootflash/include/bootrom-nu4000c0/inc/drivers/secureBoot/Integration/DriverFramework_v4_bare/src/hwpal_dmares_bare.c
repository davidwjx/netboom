/* hwpal_dmares_bare.c
 *
 * Implementation of Driver Framework v4 DMA Resource APIs for
 * Boot Monitor Platform. This minimal implementation supports the use cases
 * for the Secure Boot Loader only.
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

#include "implementation_defs.h" // implementation definitions
#include "cs_hwpal_bare.h"      // configuration

#include "basic_defs.h"         // uint32_t, NULL, inline, etc.
#include "c_lib.h"
#include "dmares_mgmt.h"        // the API to implement
#include "dmares_buf.h"         // the API to implement
#include "dmares_rw.h"          // the API to implement
#include "dmares_addr.h"        // the API to implement
#include "device_swap.h"        // Device_SwapEndian32

#ifdef DEVICE_TRACE_RW
#include <stdio.h>              // printf
#include <stdlib.h>             // exit
#endif

// in this implementation the DMAResource_Record is not used
// instead, the Device_Handle_t is a pointer to the buffer
// this limits the validity-checks we can do on the handle
// but reduces footprint and increases performance

static bool DMARes_Buffer_InUse[DMARES_BUFFER_COUNT];

static uint32_t DMARes_Buffer[DMARES_BUFFER_COUNT][DMARES_BUFFER_SIZE_BYTES / 4];


/*----------------------------------------------------------------------------
 * DMAResource_Init
 */
bool
DMAResource_Init(void)
{
    c_memset(&DMARes_Buffer_InUse, 0, sizeof(DMARes_Buffer_InUse));

    return true;
}


/*----------------------------------------------------------------------------
 * DMAResource_IsValidHandle
 */
void
DMAResource_UnInit(void)
{
}


/*------------------------------------------------------------------------------
 * DMAResource_Read32
 */
uint32_t
DMAResource_Read32(const DMAResource_Handle_t Handle,
                   const unsigned int WordOffset)
{
    uint32_t * p = Handle;
    uint32_t WordRead;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (p < DMARes_Buffer[0] || p >= DMARes_Buffer[DMARES_BUFFER_COUNT])
    {
        return 0xEEEEEEE1;
    }

    if (WordOffset * 4 >= DMARES_BUFFER_SIZE_BYTES)
    {
        return 0xEEEEEEE2;
    }
#endif

    WordRead = p[WordOffset];

#ifdef DMARESOURCE_SWAP
    WordRead = Device_SwapEndian32(WordRead);
#endif

    return WordRead;
}


/*------------------------------------------------------------------------------
 * DMAResource_Read32Array
 */
void
DMAResource_Read32Array(const DMAResource_Handle_t Handle,
                        const unsigned int StartWordOffset,
                        const unsigned int WordCount,
                        uint32_t * Values_p)
{
    uint32_t * p = Handle;
    unsigned int Words = WordCount;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (p < DMARes_Buffer[0] || p >= DMARes_Buffer[DMARES_BUFFER_COUNT])
    {
        return;
    }

    if ((StartWordOffset + WordCount) * 4 >= DMARES_BUFFER_SIZE_BYTES)
    {
        return;
    }
#endif

    p += StartWordOffset;

    while (Words > 0)
    {
        uint32_t WordRead = *p++;

#ifdef DMARESOURCE_SWAP
        WordRead = Device_SwapEndian32(WordRead);
#endif

        *Values_p++ = WordRead;
        Words--;
    }
}


/*----------------------------------------------------------------------------
 * DMAResource_Write32
 */
void
DMAResource_Write32(const DMAResource_Handle_t Handle,
                    const unsigned int WordOffset,
                    const uint32_t Value)
{
    uint32_t * p = Handle;
    uint32_t WordWrite;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (p < DMARes_Buffer[0] || p >= DMARes_Buffer[DMARES_BUFFER_COUNT])
    {
        return;
    }

    if (WordOffset * 4 >= DMARES_BUFFER_SIZE_BYTES)
    {
        return;
    }
#endif

    WordWrite = Value;

#ifdef DMARESOURCE_SWAP
    WordWrite = Device_SwapEndian32(WordWrite);
#endif

    p[WordOffset] = WordWrite;
}


/*----------------------------------------------------------------------------
 * DMAResource_Write32Array
 */
void
DMAResource_Write32Array(const DMAResource_Handle_t Handle,
                         const unsigned int StartWordOffset,
                         const unsigned int WordCount,
                         const uint32_t * Values_p)
{
    uint32_t * p = Handle;
    unsigned int Words = WordCount;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (p < DMARes_Buffer[0] || p >= DMARes_Buffer[DMARES_BUFFER_COUNT])
    {
        return;
    }

    if ((StartWordOffset + WordCount) * 4 >= DMARES_BUFFER_SIZE_BYTES)
    {
        return;
    }
#endif

    p += StartWordOffset;

    while (Words > 0)
    {
        uint32_t WordWrite;

        WordWrite = *Values_p++;

#ifdef DMARESOURCE_SWAP
        WordWrite = Device_SwapEndian32(WordWrite);
#endif
        *p++ = WordWrite;
        Words--;
    }
}


/*----------------------------------------------------------------------------
 * DMAResource_Alloc
 *
 * This function can return one of few fixed-size buffers supported by this
 * implementation. SB_CM uses this function to get DMA-safe buffers
 */
int
DMAResource_Alloc(const DMAResource_Properties_t RequestedProperties,
                  DMAResource_AddrPair_t * const AddrPair_p,
                  DMAResource_Handle_t * const Handle_p)
{
    int j;

#ifdef HWPAL_BARE_DEBUGCHECKS
    if (AddrPair_p == NULL || Handle_p == NULL)
    {
#ifdef DEVICE_TRACE_RW
        PANIC("DMAResource_Alloc: Invalid Arguments");
#endif
        return -1;
    }

    // we only support 1 buffer size
#ifdef DEVICE_TRACE_RW
    printf("DMA buffer allocation: %d bytes: ", (int)RequestedProperties.Size);
#endif
    if (RequestedProperties.Size > DMARES_BUFFER_SIZE_BYTES)
    {
        return -2;
    }
#else
    IDENTIFIER_NOT_USED(RequestedProperties.Size);
#endif

    // find a free buffer
    for (j = 0; j < DMARES_BUFFER_COUNT; j++)
    {
        if (DMARes_Buffer_InUse[j] == false)
        {
            void * Buf_p = DMARes_Buffer[j];

            DMARes_Buffer_InUse[j] = true;

            AddrPair_p->Domain = DMARES_DOMAIN_UNIFIED;
            AddrPair_p->Address.Native_p = Buf_p;

            *Handle_p = Buf_p;

#ifdef DEVICE_TRACE_RW
            printf("got: %p (#%d) sz=%d [%p is base]\n", Buf_p, j,
                   (int)DMARES_BUFFER_SIZE_BYTES,
                   (void *)&DMARes_Buffer[0][0]);
#endif
            return 0;                   // 0 = success
        }
    }

#ifdef DEVICE_TRACE_RW
    PANIC("DMAResource_Alloc: No more buffers");
#endif

    // no free buffer
    return -3;
}


/*----------------------------------------------------------------------------
 * DMAResource_Release
 */
int
DMAResource_Release(const DMAResource_Handle_t Handle)
{
    int j;

    for (j = 0; j < DMARES_BUFFER_COUNT; j++)
    {
        if (DMARes_Buffer_InUse[j])
        {
            void * Buf_p = DMARes_Buffer[j];
            if (Buf_p == Handle)
            {
                // found the buffer; now free it
                DMARes_Buffer_InUse[j] = false;

                return 0;               // 0 = success
            }
        }
    }

    // not found
    return -1;
}


/*----------------------------------------------------------------------------
 * DMAResource_CheckAndRegister
 *
 * This implementation assumes the buffers provided fulfill the requirements
 * set by the API. Only a single address domain is supported, so it must be
 * the correct one.
 */
int
DMAResource_CheckAndRegister(const DMAResource_Properties_t ActualProperties,
                             const DMAResource_AddrPair_t AddrPair,
                             const char AllocatorRef,
                             DMAResource_Handle_t * const Handle_p)
{
    IDENTIFIER_NOT_USED(ActualProperties.Size);
    IDENTIFIER_NOT_USED(AddrPair.Domain);
    IDENTIFIER_NOT_USED(AllocatorRef);
    IDENTIFIER_NOT_USED(Handle_p);

    return 0;
}


/*----------------------------------------------------------------------------
 * DMAResource_Translate
 *
 * This implementation supports a flat address map where DMA addresses and
 * CPU addresses are alike (all physical addresses).
 */
int
DMAResource_Translate(const DMAResource_Handle_t Handle,
                      const DMAResource_AddrDomain_t DestDomain,
                      DMAResource_AddrPair_t * const PairOut_p)
{
    switch (DestDomain)
    {
    case DMARES_DOMAIN_HOST:
    case DMARES_DOMAIN_UNIFIED:
        if (PairOut_p->Domain == DMARES_DOMAIN_EIP13xDMA)
        {
            PairOut_p->Domain = DestDomain;
            PairOut_p->Address.Native_p = (void *)(uintptr_t)PairOut_p->Address.Value64;
        }
        else if (PairOut_p->Domain == DMARES_DOMAIN_EIP12xDMA)
        {
            PairOut_p->Domain = DestDomain;
            PairOut_p->Address.Native_p = (void *)(uintptr_t)PairOut_p->Address.Value32;
        }
        else if (PairOut_p->Domain == DMARES_DOMAIN_EIP93DMA)
        {
            PairOut_p->Domain = DestDomain;
            PairOut_p->Address.Native_p = (void *)(uintptr_t)PairOut_p->Address.Value32;
        }
        break;

    case DMARES_DOMAIN_EIP13xDMA:
        if ((PairOut_p->Domain == DMARES_DOMAIN_HOST) ||
            (PairOut_p->Domain == DMARES_DOMAIN_UNIFIED))
        {
            PairOut_p->Domain = DMARES_DOMAIN_EIP13xDMA;
            PairOut_p->Address.Value64 = (uint64_t)(uintptr_t)PairOut_p->Address.Native_p;
        }
        break;

    case DMARES_DOMAIN_EIP12xDMA:
        if ((PairOut_p->Domain == DMARES_DOMAIN_HOST) ||
            (PairOut_p->Domain == DMARES_DOMAIN_UNIFIED))
        {
            PairOut_p->Domain = DMARES_DOMAIN_EIP12xDMA;
            PairOut_p->Address.Value32 = (uint32_t)(uintptr_t)PairOut_p->Address.Native_p;
        }
        break;

    case DMARES_DOMAIN_EIP93DMA:
        if ((PairOut_p->Domain == DMARES_DOMAIN_HOST) ||
            (PairOut_p->Domain == DMARES_DOMAIN_UNIFIED))
        {
            PairOut_p->Domain = DMARES_DOMAIN_EIP93DMA;
            PairOut_p->Address.Value32 = (uint32_t)(uintptr_t)PairOut_p->Address.Native_p;
        }
        break;

    default:
        IDENTIFIER_NOT_USED(Handle);
        break;
    }

    if (DestDomain == PairOut_p->Domain)
    {
        return 0;
    }
    return -1;
}


/*----------------------------------------------------------------------------
 * DMAResource_PreDMA/PostDMA
 *
 * This trivial implementation assumes memory is uncached, so no cache
 * coherency management needed.
 */
void
DMAResource_PreDMA(const DMAResource_Handle_t Handle,
                   const unsigned int ByteOffset,
                   const unsigned int ByteCount)
{
    IDENTIFIER_NOT_USED(Handle);
    IDENTIFIER_NOT_USED(ByteOffset);
    IDENTIFIER_NOT_USED(ByteCount);
}


/*----------------------------------------------------------------------------
 * DMAResource_PostDMA
 */
void
DMAResource_PostDMA(const DMAResource_Handle_t Handle,
                    const unsigned int ByteOffset,
                    const unsigned int ByteCount)
{
    IDENTIFIER_NOT_USED(Handle);
    IDENTIFIER_NOT_USED(ByteOffset);
    IDENTIFIER_NOT_USED(ByteCount);
}


/* end of file hwpal_dmares_bare.c */
