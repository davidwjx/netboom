/* hwpal_dmares_addr_cm_fpga.c
 *
 * Implementation of the DMAResource address translation API specific for
 * the CM-FPGA environment. In this environment:
 *
 * 1. DMA-buffers are always located in regular host system memory;
 * 2. DMA-buffers are never shared between applications, i.e. the
 *    DMARES_DOMAIN_INTERHOST domain is not used;
 * 3. For each DMA-buffer allocated, the buffer's
 *    address is returned for 2 domains:
 *    a. DMARES_DOMAIN_HOST -- application virtual address
 *    b. DMARES_DOMAIN_BUS -- kernel physical address
 * 4. The EIP123's DMA engine is setup using addresses in the DMARES_DOMAIN_
 *    EIP123DMA domain. For this environment, the EIP123 DMA engine must be
 *    setup with address A + PCI_REGION_1_START in order to access address A
 *    in the host system memory.
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

#include "c_hwpal_dmares_umdevxs.h" // get the configuration options
#include "dmares_addr.h"            // the API to implement

#define LOG_SEVERITY_MAX  HWPAL_LOG_SEVERITY
#include "log.h"

#ifndef PCI_REGION_1_START
#define PCI_REGION_1_START 0x50000000
#endif

// code shared with the main part of the DMAResource API implementation
extern DMAResource_Record_t *
DMAResourceLib_Handle2RecordPtr(const DMAResource_Handle_t Handle);

extern DMAResource_AddrPair_t *
DMAResourceLib_LookupDomain(const DMAResource_Record_t * Rec_p,
                            const DMAResource_AddrDomain_t Domain);

extern bool
DMAResourceLib_IsSaneInput(const DMAResource_AddrPair_t * AddrPair_p,
                           const DMAResource_Properties_t * Props_p);


/*----------------------------------------------------------------------------
 * DMAResource_Translate
 */
int
DMAResource_Translate(const DMAResource_Handle_t Handle,
                      const DMAResource_AddrDomain_t DestDomain,
                      DMAResource_AddrPair_t * const PairOut_p)
{
    DMAResource_Record_t * Rec_p;
    DMAResource_AddrPair_t * Pair_p;

    if (PairOut_p == NULL)
    {
        return -1;
    }

    Rec_p = DMAResourceLib_Handle2RecordPtr(Handle);
    if (Rec_p == NULL)
    {
        LOG_WARN("DMAResource_Translate: Invalid handle %p\n", Handle);
        return -1;
    }

    switch (DestDomain)
    {
    case DMARES_DOMAIN_HOST:
    case DMARES_DOMAIN_BUS:
        Pair_p = DMAResourceLib_LookupDomain(Rec_p, DestDomain);
        if (Pair_p != NULL)
        {
            *PairOut_p = *Pair_p;
            return 0;
        }
        break;

    case DMARES_DOMAIN_EIP13xDMA:
        Pair = DMAResourceLib_LookupDomain(Rec_p, DMARES_DOMAIN_BUS);
        if (Pair != NULL)
        {
            uint64_t p = (uint64_t)(uintptr_t)Pair_p->Address.Native_p;

            PairOut_p->Domain = DestDomain;
            PairOut_p->Address.Value64 = (p + PCI_REGION_1_START);
            return 0;
        }
        break;

#ifndef CFG_ENABLE_TARGET_VTBAL
    case DMARES_DOMAIN_EIP93DMA:
#endif
    case DMARES_DOMAIN_EIP12xDMA:
        Pair_p = DMAResourceLib_LookupDomain(Rec_p, DMARES_DOMAIN_BUS);
        if (Pair_p != NULL)
        {
            uint32_t p = (uint32_t)(uintptr_t)Pair_p->Address.Native_p;

            PairOut_p->Domain = DestDomain;
            PairOut_p->Address.Value32 = (p + PCI_REGION_1_START);
            return 0;
        }
        break;

    default:
        break;
    }

    LOG_WARN("DMAResource_Translate: No address for domain %u (Handle=%p)\n",
             DestDomain, Handle);

    PairOut_p->Domain = DMARES_DOMAIN_UNKNOWN;
    PairOut_p->Address.Native_p = NULL;
    return -1;
}


/*----------------------------------------------------------------------------
 * DMAResource_AddPair
 */
int
DMAResource_AddPair(const DMAResource_Handle_t Handle,
                    const DMAResource_AddrPair_t Pair)
{
    DMAResource_Record_t * Rec_p;
    DMAResource_AddrPair_t * AddrPair_p;

    Rec_p = DMAResourceLib_Handle2RecordPtr(Handle);
    if (Rec_p == NULL)
    {
        LOG_WARN("DMAResource_AddPair: Invalid handle %p\n", Handle);
        return -1;
    }

    /* Check if this pair already exists */
    AddrPair_p = DMAResourceLib_LookupDomain(Rec_p, Pair.Domain);
    if (AddrPair_p)
    {
        LOG_INFO("DMAResource_AddPair: Replacing address for handle %p?\n",
                 Handle);
    }
    else
    {
        /* Find a free slot to store this domain info */
        AddrPair_p = DMAResourceLib_LookupDomain(Rec_p, 0);
        if (AddrPair_p == NULL)
        {
            LOG_WARN("DMAResource_AddPair: Table overflow for handle %p\n",
                     Handle);
            return -2;
        }
    }

    if (!DMAResourceLib_IsSaneInput(&Pair, &Rec_p->Props))
    {
        return -3;
    }

    *AddrPair_p = Pair;
    return 0;
}

/* end of file hwpal_dmares_addr_cm_fpga.c */
