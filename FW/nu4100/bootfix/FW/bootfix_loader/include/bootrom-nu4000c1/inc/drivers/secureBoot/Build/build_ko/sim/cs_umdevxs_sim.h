/* cs_umdevxs_sim.h
 *
 * Configuration Switches for UMDevXS Kernel driver
 *  for Simulation device only (for testing without hardware).
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

// logging level (choose one)
//#define LOG_SEVERITY_MAX LOG_SEVERITY_CRIT
#define LOG_SEVERITY_MAX LOG_SEVERITY_WARN
//#define LOG_SEVERITY_MAX LOG_SEVERITY_INFO

// uncomment to enable device logging when an error occurs
//#define UMDEVXS_CHRDEV_LOG_ERRORS
// uncomment to enable Pre- and Post-DMA logging
//#define HWPAL_TRACE_DMARESOURCE_PREPOSTDMA

#define UMDEVXS_LOG_PREFIX "UMDevXS_Sim: "

#define UMDEVXS_MODULENAME "umdevxs"

// uncomment to remove selected functionality
//#define UMDEVXS_REMOVE_DEVICE
//#define UMDEVXS_REMOVE_SMBUF
#define UMDEVXS_REMOVE_PCI
//#define UMDEVXS_REMOVE_SIMULATION
#define UMDEVXS_REMOVE_INTERRUPT
#define UMDEVXS_REMOVE_DEVICE_PCICFG
#define UMDEVXS_REMOVE_DEVICE_OF

// Definition of device resources
// UMDEVXS_DEVICE_ADD      Name               Start  Last
// UMDEVXS_DEVICE_ADD_PCI  Name               Bar    Start       Size
// UMDEVXS_DEVICE_ADD_SIM  Name               Size
#define UMDEVXS_DEVICES \
    UMDEVXS_DEVICE_ADD_SIM("EIP150",          0x10000), \
    UMDEVXS_DEVICE_ADD_SIM("EIP123_HOST0",    0x04000), \
    UMDEVXS_DEVICE_ADD_SIM("EIP123_FPGA",     0x01000)

/* end of file cs_umdevxs_sim.h */
