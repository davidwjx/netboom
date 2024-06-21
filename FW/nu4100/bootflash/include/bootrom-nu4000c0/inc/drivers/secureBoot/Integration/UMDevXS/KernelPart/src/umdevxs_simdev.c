/* umdevxs_simdev.c
 *
 * Simulation device support for the Linux UMDevXS driver.
 */

/*****************************************************************************
* Copyright (c) 2010-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "c_umdevxs.h"                  // config options
#include "umdevxs_internal.h"

#ifndef UMDEVXS_REMOVE_SIMULATION

#include <linux/mm.h>                   // get_order & __get_dma_pages
#include <asm/io.h>                     // virt_to_phys

#include "basic_defs.h"                 // uint8_t, MASK_xx_BITS, etc.

// Simulation device administration
typedef struct
{
    void * Ref_p;           // Device reference
    void * ShMem_p;         // Device shared memory
} UMDevXS_SimDev_t;

static UMDevXS_SimDev_t UMDevXS_SimDev[] =
{
    {0}, {0}
};

#define UMDEVXS_SIMDEV_RESOURCE_COUNT \
        (sizeof(UMDevXS_SimDev) \
         / sizeof(UMDevXS_SimDev_t))


/*----------------------------------------------------------------------------
 * UMDevXS_SimDev_Map
 *
 * This function is called to retrieve the address of the simulation device
 * shared memory.
 */
int
UMDevXS_SimDev_Map(
    void * Reference_p,
    unsigned long * RawAddr_p)
{
    int i;

    // Search for simulation device
    for (i = 0; i < UMDEVXS_SIMDEV_RESOURCE_COUNT; i++)
    {
        UMDevXS_SimDev_t * const p = UMDevXS_SimDev + i;

        if (p->Ref_p == Reference_p)
        {
            // Found it, return address
            *RawAddr_p = virt_to_phys(p->ShMem_p);

            LOG_INFO(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_SimDev_Map: "
                "Reference_p=%p -> Address=%lu\n",
                Reference_p, *RawAddr_p);
            return 0;       // 0 = success
        }
    }

    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "UMDevXS_SimDev_Map: "
        "Reference_p=%p not found\n",
        Reference_p);

    // Unknown device
    return -1;
}


/*----------------------------------------------------------------------------
 * UMDevXS_SimDev_HandleCmd_Find
 *
 * This function is called when the the Device_Find function matched a
 * simulation placeholder device. This function is called with the related BAR
 * and must check it is valid and fill in a few CmdRsp fields.
 * The BAR must be supported by this simulation device.
 * The Size and Handle must be returned.
 * CmdRsp_p->uint1 = configured size of device to map
 */
void
UMDevXS_SimDev_HandleCmd_Find(
    UMDevXS_CmdRsp_t * const CmdRsp_p,
    void * Reference_p,
    unsigned int LastByteOffset)
{
    int i;

    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "UMDevXS_SimDev_HandleCmd_Find: "
        "Reference_p=%p, LastByteOffset=%u\n",
        Reference_p, LastByteOffset);

    if (CmdRsp_p == NULL)
    {
        CmdRsp_p->Error = 1;
        return;
    }

    if (Reference_p == NULL)
    {
        CmdRsp_p->Error = 2;
        return;
    }

    for (i = 0; i < UMDEVXS_SIMDEV_RESOURCE_COUNT; i++)
    {
        UMDevXS_SimDev_t * const p = UMDevXS_SimDev + i;

        if (p->Ref_p == Reference_p)
        {
            // Device is already created, so reuse it
            LOG_INFO(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_SimDev_HandleCmd_Find: "
                "Reuse @ %p\n",
                p->ShMem_p);

            // Limit size to simulation device
            CmdRsp_p->uint1 = LastByteOffset + 1;
            return;
        }

        if (p->Ref_p == 0)
        {
            // Device is not created yet, so allocate shared memory
            // Assume that PAGE_SIZE is a multiple of 2
            uint32_t Size = (LastByteOffset + PAGE_SIZE) & ~(PAGE_SIZE - 1);
            int order;
            void * pDevice;

            if (Size >= 32 * 1024 * 1024)
            {
                // We support up to 32 megabyte buffers
                CmdRsp_p->Error = 3;
                return;
            }

            // The order=n is the smallest integer for which:
            // PAGE_SIZE * (2^n) >= Size
            order = get_order(Size);

            // Use __get_dma_pages() to ensure we get page aligned, DMA-capable
            // memory (i.e. from start of system RAM). Assume this function is
            // called from sleepable context, so no need to pass the GPF_ATOMIC flag.
            pDevice = (void *)__get_dma_pages(GFP_KERNEL, order);
            if (pDevice == NULL)
            {
                LOG_INFO(
                    UMDEVXS_LOG_PREFIX
                    "UMDevXS_SimDev_HandleCmd: "
                    "__get_dma_pages(%u) failed\n",
                    order);

                CmdRsp_p->Error = 4;
                return;
            }

            LOG_INFO(
                UMDEVXS_LOG_PREFIX
                "UMDevXS_SimDev_HandleCmd_Find: "
                "Allocate %u bytes @ %p\n",
                Size, pDevice);

            // Initialize shared memory
            c_memset(pDevice, 0, Size);

            // Make shared memory available for simulation device use
            p->Ref_p   = Reference_p;
            p->ShMem_p = pDevice;

            // Limit size to simulation device
            CmdRsp_p->uint1 = LastByteOffset + 1;
            return;
        }
    }

    // All entries are taken
    CmdRsp_p->Error = 5;
}

#endif /* UMDEVXS_REMOVE_SIMULATION */

/* end of file umdevxs_simdev.c */
