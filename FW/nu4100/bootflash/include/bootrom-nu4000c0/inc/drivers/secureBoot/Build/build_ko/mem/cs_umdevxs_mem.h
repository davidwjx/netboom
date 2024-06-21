/* cs_umdevxs.h
 *
 * Configuration Switches for UMDevXS Kernel driver
 *  for Memory Mapped Peripherals.
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
//#define UMDEVXS_PCI_VENDOR_ID  0x10EE
//#define UMDEVXS_PCI_DEVICE_ID  0x0300

// interrupt from logic tile is routed to INT_VICSOURCE21
#define UMDEVXS_INTERRUPT_STATIC_IRQ  21

// logging level (choose one)
//#define LOG_SEVERITY_MAX LOG_SEVERITY_CRIT
#define LOG_SEVERITY_MAX LOG_SEVERITY_WARN
//#define LOG_SEVERITY_MAX LOG_SEVERITY_INFO

// uncomment to enable device logging when an error occurs
//#define UMDEVXS_CHRDEV_LOG_ERRORS

// uncomment to enable Pre- and Post-DMA logging
//#define HWPAL_TRACE_DMARESOURCE_PREPOSTDMA

#define UMDEVXS_LOG_PREFIX "UMDevXS_Mem: "

#define UMDEVXS_MODULENAME "umdevxs"

// uncomment to remove selected functionality
//#define UMDEVXS_REMOVE_DEVICE
//#define UMDEVXS_REMOVE_SMBUF
#define UMDEVXS_REMOVE_PCI
#define UMDEVXS_REMOVE_SIMULATION
//#define UMDEVXS_REMOVE_INTERRUPT
#define UMDEVXS_REMOVE_DEVICE_PCICFG
#define UMDEVXS_REMOVE_DEVICE_OF

// Definition of device resources
// Note: The definition depends on hardware platform and how various EIP has been configured for it.
// UMDEVXS_DEVICE_ADD      Name           Start       Last
// UMDEVXS_DEVICE_ADD_PCI  Name           Bar         Start       Size
// UMDEVXS_DEVICE_ADD_SIM  Name           Size
#define UMDEVXS_DEVICES UMDEVXS_DEVICES_ZYNQ_FPGA
#define UMDEVXS_DEVICES_VERSATILE_FPGA                             \
    UMDEVXS_DEVICE_ADD("EIP123_HOST0",    0x80000000, 0x80003FFF), \
    UMDEVXS_DEVICE_ADD("EIP150",          0x80100000, 0x8010BFFF), \
    UMDEVXS_DEVICE_ADD("EIP123_FPGA",     0x90000000, 0x90000FFF), \
    UMDEVXS_DEVICE_ADD("EIP123_FPGA_NVM", 0x90001000, 0x90001FFF), \
    UMDEVXS_DEVICE_ADD("EIP123_FPGA_PRG", 0x90010000, 0x9001FFFF), \
    UMDEVXS_DEVICE_ADD("EIP150_PKA",      0x80104000, 0x80105FFF), \
    UMDEVXS_DEVICE_ADD("EIP150_PKA_PRG",  0x80106000, 0x80107FFF), \
    UMDEVXS_DEVICE_ADD("SRAM",            0x38001000, 0x381FFFFF), \
    UMDEVXS_DEVICE_ADD("SYS.FPGA",        0x10000000, 0x100FFFFF)

#define UMDEVXS_DEVICES_ZYNQ_FPGA                                  \
    UMDEVXS_DEVICE_ADD("EIP150",          0x40000000, 0x4000FFFF), \
    UMDEVXS_DEVICE_ADD("EIP28",           0x40004000, 0x40007FFF), \
    UMDEVXS_DEVICE_ADD("EIP93",           0x40010000, 0x4001FFFF), \
    UMDEVXS_DEVICE_ADD("GPIO",            0x41200000, 0x4120FFFF), \
    UMDEVXS_DEVICE_ADD("DDR",             0x00000000, 0x3FFFFFFF), \
    UMDEVXS_DEVICE_ADD("F8",              0xF8000000, 0x0000FFFF)

/* end of file cs_umdevxs.h */
