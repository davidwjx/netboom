/* umdevxs_lkm.c
 *
 * Loadable Kernel Module (LKM) support the Linux UMPCI driver.
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

#include "c_umdevxs.h"          // config options
#include "umdevxs_internal.h"

#include <linux/errno.h>
#include <linux/module.h>

MODULE_LICENSE(UMDEVXS_LICENSE);


/*----------------------------------------------------------------------------
 * UMDevXS_module_init
 */
static int
UMDevXS_module_init(void)
{
    int Status;
#if (!defined(UMDEVXS_REMOVE_PCI) || \
     !defined(UMDEVXS_REMOVE_INTERRUPT) || \
     !defined(UMDEVXS_REMOVE_DEVICE_OF))
    int nIRQ = UMDEVXS_INTERRUPT_STATIC_IRQ;
#endif

    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "loading driver\n");

#ifndef UMDEVXS_REMOVE_PCI
    Status = UMDevXS_PCIDev_Init();
    if (Status < 0)
    {
        return Status;
    }
    nIRQ = Status;
#endif

#ifndef UMDEVXS_REMOVE_DEVICE_OF
    Status = UMDevXS_OFDev_Init();
    if (Status < 0)
    {
        return Status;
    }
    nIRQ = Status;
#endif

#ifndef UMDEVXS_REMOVE_INTERRUPT
    UMDevXS_Interrupt_Init(nIRQ);
#endif

    Status = UMDevXS_ChrDev_Init();
    if (Status < 0)
    {
        return Status;
    }

#ifndef UMDEVXS_REMOVE_DMABUF
    if (!BufAdmin_Init(UMDEVXS_DMARESOURCE_HANDLES_MAX, NULL, 0))
    {
        return -ENOMEM;
    }
#endif

#ifndef UMDEVXS_REMOVE_SMBUF
    Status = UMDevXS_SMBuf_Init();
    if (Status < 0)
    {
        return Status;
    }
#endif

    return 0;
}


/*----------------------------------------------------------------------------
 * UMDevXS_module_exit
 */
static void
UMDevXS_module_exit(void)
{
    LOG_INFO(
        UMDEVXS_LOG_PREFIX
        "unloading driver\n");

#ifndef UMDEVXS_REMOVE_SMBUF
    UMDevXS_SMBuf_UnInit();
#endif

#ifndef UMDEVXS_REMOVE_INTERRUPT
    UMDevXS_Interrupt_UnInit();
#endif

#ifndef UMDEVXS_REMOVE_PCI
    UMDevXS_PCIDev_UnInit();
#endif

#ifndef UMDEVXS_REMOVE_DEVICE_OF
    UMDevXS_OFDev_UnInit();
#endif

    UMDevXS_ChrDev_UnInit();

#ifndef UMDEVXS_REMOVE_DMABUF
    BufAdmin_UnInit();
#endif
}

module_init(UMDevXS_module_init);
module_exit(UMDevXS_module_exit);

/* end of file umdevxs_lkm.c */
