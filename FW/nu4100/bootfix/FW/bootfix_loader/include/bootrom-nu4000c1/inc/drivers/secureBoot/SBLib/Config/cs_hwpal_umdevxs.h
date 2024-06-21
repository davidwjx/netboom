/* cs_hwpal_umdevxs.h
 *
 * Configuration Settings for Driver Framework Implementation
 * for the SafeXcel-IP-123 HW2.0 Crypto Module.
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

// logging level for HWPAL Device
// Choose from LOG_SEVERITY_INFO, LOG_SEVERITY_WARN, LOG_SEVERITY_CRIT
#define HWPAL_LOG_SEVERITY  LOG_SEVERITY_WARN

// maximum allowed length for a device name
#define HWPAL_MAX_DEVICE_NAME_LENGTH 64

// debug checking and trace code
#define HWPAL_STRICT_ARGS_CHECK
#define HWPAL_DEVICE_MAGIC        54333
#define HWPAL_TRACE_DEVICE_READ
#define HWPAL_TRACE_DEVICE_WRITE

// device to request from UMDevXS driver
#define HWPAL_DEVICE0_UMDEVXS  "EIP123_HOST0"
#define HWPAL_DEVICE1_UMDEVXS  "EIP123_FPGA"

// definition of static resources inside the above device
// Refer to the data sheet of device for the correct values
//                   Name            DeviceNr   Start    Last     Flags (see below)
/*
#define HWPAL_DEVICES \
    HWPAL_DEVICE_ADD("EIP93",        0,         0x00000, 0x00FFF, 0), \
    HWPAL_DEVICE_ADD("EIP150",       0,         0x10000, 0x0FFFF, 0), \
    HWPAL_DEVICE_ADD("EIP150_TRNG",  1,         0x10000, 0x00080, 0), \
    HWPAL_DEVICE_ADD("EIP150_PKA",   1,         0x14000, 0x07FFF, 0), \
    HWPAL_DEVICE_ADD("EIP150_AIC",   1,         0x18000, 0x0801F, 0)
#endif
*/

#define HWPAL_DEVICES \
    HWPAL_DEVICE_ADD("EIP123",       0,         0x00000, 0x03FFF, 0), \
    HWPAL_DEVICE_ADD("EIP201",       0,         0x03E00, 0x03E1F, 0), \
    HWPAL_DEVICE_ADD("EIP123_CTRL",  1,         0x00000, 0x00FFF, 0), \
    HWPAL_DEVICE_ADD("EIP201_FPGA",  1,         0x00100, 0x0011F, 0)

// Flags:
// (binary OR of the following)
//   0 = Disable trace
//   1 = Trace reads  (requires HWPAL_TRACE_DEVICE_READ)
//   2 = Trace writes (requires HWPAL_TRACE_DEVICE_WRITE)
//   4 = Swap word endianess before write / after read

// no remapping required
#define HWPAL_REMAP_ADDRESSES
/* device address remapping is done like this:
#define HWPAL_REMAP_ADDRESS \
      HWPAL_REMAP_ONE(_old, _new) \
      HWPAL_REMAP_ONE(_old, _new)
*/

// #of supported DMA resources
#define HWPAL_DMA_NRESOURCES 128

// only define this if the platform hardware guarantees cache coherency of
// DMA buffers, i.e. when SW does not need to do coherency management.
#undef HWPAL_ARCH_COHERENT

// DMARES_DOMAIN_DEVICE address is in the FPGA
// we must offset it from the local DMARES_DOMAIN_BUS address
#define HWPAL_DMARES_DEVICE_ADDR_OFFSET 0x50000000

/* end of file cs_hwpal_umdevxs.h */
