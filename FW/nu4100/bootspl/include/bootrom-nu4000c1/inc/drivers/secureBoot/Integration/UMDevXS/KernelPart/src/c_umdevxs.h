/* c_umdevxs.h
 *
 * Configuration options for UMDevXS driver.
 *
 * This file includes cs_umdevxs.h (from the product-level) and then provides
 * defaults for missing configuration switches.
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

#ifndef INCLUDE_GUARD_C_UMDEVXS_H
#define INCLUDE_GUARD_C_UMDEVXS_H

#define HWPAL_LOCK_SLEEPABLE
#define HWPAL_TRACE_DMARESOURCE_LEAKS

// get the product-level configuration
#include "cs_umdevxs.h"

// provide backup values

#ifndef UMDEVXS_LICENSE
#define UMDEVXS_LICENSE "Proprietary"
#endif

#ifndef UMDEVXS_DMARESOURCE_HANDLES_MAX
#define UMDEVXS_DMARESOURCE_HANDLES_MAX 128
#endif

#ifndef UMDEVXS_SMBUF_DCDUS
#define UMDEVXS_SMBUF_DCDUS 32
#endif

#ifndef UMDEVXS_PCI_VENDOR_ID
#define UMDEVXS_PCI_VENDOR_ID  0
#endif

#ifndef UMDEVXS_PCI_DEVICE_ID
#define UMDEVXS_PCI_DEVICE_ID  0
#endif

#ifndef UMDEVXS_PCI_BAR0_SUBSET_START
#define UMDEVXS_PCI_BAR0_SUBSET_START  0
#endif
#ifndef UMDEVXS_PCI_BAR1_SUBSET_START
#define UMDEVXS_PCI_BAR1_SUBSET_START  0
#endif
#ifndef UMDEVXS_PCI_BAR2_SUBSET_START
#define UMDEVXS_PCI_BAR2_SUBSET_START  0
#endif
#ifndef UMDEVXS_PCI_BAR3_SUBSET_START
#define UMDEVXS_PCI_BAR3_SUBSET_START  0
#endif

#ifndef UMDEVXS_PCI_BAR0_SUBSET_SIZE
#define UMDEVXS_PCI_BAR0_SUBSET_SIZE   1*1024*1024
#endif
#ifndef UMDEVXS_PCI_BAR1_SUBSET_SIZE
#define UMDEVXS_PCI_BAR1_SUBSET_SIZE   1*1024*1024
#endif
#ifndef UMDEVXS_PCI_BAR2_SUBSET_SIZE
#define UMDEVXS_PCI_BAR2_SUBSET_SIZE   1*1024*1024
#endif
#ifndef UMDEVXS_PCI_BAR3_SUBSET_SIZE
#define UMDEVXS_PCI_BAR3_SUBSET_SIZE   1*1024*1024
#endif

#ifndef UMDEVXS_INTERRUPT_STATIC_IRQ
#define UMDEVXS_INTERRUPT_STATIC_IRQ -1
#endif

#ifndef UMDEVXS_INTERRUPT_TRACE_FILTER
#define UMDEVXS_INTERRUPT_TRACE_FILTER 0
#endif

// logging level
#ifndef LOG_SEVERITY_MAX
#define LOG_SEVERITY_MAX LOG_SEVERITY_CRIT
#endif


#ifndef UMDEVXS_LOG_PREFIX
#define UMDEVXS_LOG_PREFIX "UMDevXS: "
#endif

#ifndef UMDEVXS_MODULENAME
#define UMDEVXS_MODULENAME "umdevxs"
#endif

// if UMDEVXS_REMOVE_DEVICE is defined, make sure that the PCI and Simulation
// devices are removed as well
#ifdef UMDEVXS_REMOVE_DEVICE
#define UMDEVXS_REMOVE_PCI
#define UMDEVXS_REMOVE_SIMULATION
#define UMDEVXS_REMOVE_DEVICE_PCICFG
#define UMDEVXS_REMOVE_DEVICE_OF
#endif

// if UMDEVXS_REMOVE_PCI is defined, make sure that the PCI Config Space
// device is removed as well
#ifdef UMDEVXS_REMOVE_PCI
#define UMDEVXS_REMOVE_DEVICE_PCICFG
#endif

#endif /* INCLUDE_GUARD_C_UMDEVXS_H */

/* end of file c_umdevxs.h */
