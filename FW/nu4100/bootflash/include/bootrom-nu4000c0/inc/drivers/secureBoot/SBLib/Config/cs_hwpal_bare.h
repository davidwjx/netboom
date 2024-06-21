/* cs_hwpal_bare.h
 *
 * Configuration Settings for the 'barebones' Driver Framework Implementation.
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

// Active debug checks
// This define must be disabled for footprint optimization
//#define HWPAL_BARE_DEBUGCHECKS

// The device names supported by Device_Find()
// remember that all listed devices will be supported by hwpal
// for footprint not used devices should be disabled.
#define HWPAL_BARE_EIP130_NAME      "EIP130"
#define HWPAL_BARE_EIP123_NAME      "EIP123"
#define HWPAL_BARE_EIP93_NAME       "EIP93"
#define HWPAL_BARE_EIP28_NAME       "EIP28"

// Definition of hardware access addresses
// These addresses must match the hardware addresses configured for the device.
//#define HWPAL_BARE_EIP130_ADDRESS   0x40000000 // Secure
#define HWPAL_BARE_EIP130_ADDRESS   0x40004000 // Non-secure
#define HWPAL_BARE_EIP123_ADDRESS   0x80000000
#define HWPAL_BARE_EIP93_ADDRESS    0x40010000
#define HWPAL_BARE_EIP28_ADDRESS    0x40004000

// Define size of the memory window for each device.
#define HWPAL_BARE_EIP130_WINDOWEND 0x3FFF
#define HWPAL_BARE_EIP123_WINDOWEND 0x3FFF
#define HWPAL_BARE_EIP93_WINDOWEND  0xFFFF
#define HWPAL_BARE_EIP28_WINDOWEND  0x3FFF

// Active trace of all device find operations.
//#define DEVICE_TRACE_FIND

// Active trace of all device register reads/writes
//#define DEVICE_TRACE_RW

// Activate 32bit endianess swaps when accessing device registers
//#define DEVICE_SWAP

// Number of DMA-safe buffers to support
#define DMARES_BUFFER_COUNT       9

// Fixed size of each DMA-safe buffer
#define DMARES_BUFFER_SIZE_BYTES  2048

// Activate 32bit endianess swaps when accessing dma buffers
//#define DMARESOURCE_SWAP

// Activate performance measurements
//#define HWPAL_BARE_ENABLE_SBLPERF

/* end of file cs_hwpal_bare.h */
