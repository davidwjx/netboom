/* cs_umdevxs_pci.h
 *
 * Configuration Switches for UMDevXS Kernel driver for
 *  PCI driver for Versatile FPGA
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

// PCI device Vendor ID and Device ID
// used to match the driver to the appropriate device

// FPGA device on ARM Versatile
#define UMDEVXS_PCI_VENDOR_ID  0x10EE
#define UMDEVXS_PCI_DEVICE_ID  0x0300

// logging level (choose one)
#define LOG_SEVERITY_MAX LOG_SEVERITY_CRIT
//#define LOG_SEVERITY_MAX LOG_SEVERITY_WARN
//#define LOG_SEVERITY_MAX LOG_SEVERITY_INFO

// uncomment to enable device logging when an error occurs
//#define UMDEVXS_CHRDEV_LOG_ERRORS

// uncomment to enable Pre- and Post-DMA logging
//#define HWPAL_TRACE_DMARESOURCE_PREPOSTDMA

#define UMDEVXS_LOG_PREFIX "UMDevXS_PCI: "

#define UMDEVXS_MODULENAME "umdevxs"

// uncomment to remove selected functionality
//#define UMDEVXS_REMOVE_DEVICE
//#define UMDEVXS_REMOVE_SMBUF
//#define UMDEVXS_REMOVE_PCI
#define UMDEVXS_REMOVE_SIMULATION
//#define UMDEVXS_REMOVE_INTERRUPT
#define UMDEVXS_REMOVE_DEVICE_PCICFG
#define UMDEVXS_REMOVE_DEVICE_OF

// Definition of device resources
// UMDEVXS_DEVICE_ADD      Name               Start  Last
// UMDEVXS_DEVICE_ADD_PCI  Name               Bar    Start       Size
// UMDEVXS_DEVICE_ADD_SIM  Name               Size
#define UMDEVXS_DEVICES \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST0",    1,     0x00000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST1",    1,     0x10000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST2",    1,     0x20000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST3",    1,     0x30000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST4",    1,     0x40000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST5",    1,     0x50000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST6",    1,     0x60000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_HOST7",    1,     0x70000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP150",          1,    0x100000,   0x10000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP150_PKA",      1,    0x104000,    0x4000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP150_PKA_PRG",  1,    0x106000,    0x2000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP150_AIC",      1,    0x108000,     0x100), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_FPGA",     2,     0x00000,    0x1000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_FPGA_NVM", 2,     0x01000,    0x1000), \
    UMDEVXS_DEVICE_ADD_PCI("EIP123_FPGA_PRG", 2,     0x10000,    0x10000)

// in addition to the above devices, the memory windows of the
// PCI devices can be retrieved as PCI.<bar> with <bar> being
// a single digit and in the valid range for the device (0..n).
// To more easily support large windows, we allow a subset to
// used using the following switches
#define UMDEVXS_PCI_BAR0_SUBSET_START  0
#define UMDEVXS_PCI_BAR0_SUBSET_SIZE   4*1024

// BAR 1 is the design (EIP-123 / EIP-150)
#define UMDEVXS_PCI_BAR1_SUBSET_START  0
#define UMDEVXS_PCI_BAR1_SUBSET_SIZE   2*1024*1024

// BAR 2 is the EIP-123 FPGA glue around the design
#define UMDEVXS_PCI_BAR2_SUBSET_START  0
#define UMDEVXS_PCI_BAR2_SUBSET_SIZE   128*1024

#define UMDEVXS_PCI_BAR3_SUBSET_START  0
#define UMDEVXS_PCI_BAR3_SUBSET_SIZE   4*1024

// Enable when using MSI interrupts on PCI
//#define UMDEVXS_USE_MSI

/* end of file cs_umdevxs_pci.h */
