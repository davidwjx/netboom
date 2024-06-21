/* c_eip93.h
 *
 * Configuration options for the EIP93 module.
 */

/*****************************************************************************
* Copyright (c) 2008-2018 INSIDE Secure B.V. All Rights Reserved.
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

/*---------------------------------------------------------------------------
 * Defines that can be used in cs_eip93.h.
 * Defaults that should be set if not defined in cs_eip93.h
 */
#include "cs_eip93.h"

/*---------------------------------------------------------------------------
 * flag to control paramater check
 */

//#define EIP93_STRICT_ARGS 1  //uncomment when paramters check needed

#if defined(EIP93_BUS_VERSION_AXI)

// AXI bus interface
// Allowed (required) bus interface parameters:
#ifndef EIP93_BUS_MAX_BURST_SIZE
#define EIP93_BUS_MAX_BURST_SIZE 4      // Set an AXI default
#endif
#if (EIP93_BUS_MAX_BURST_SIZE < 0) || (EIP93_BUS_MAX_BURST_SIZE > 4)
#error "ERROR: Invalid maximum burst size"
#endif
#ifndef EIP93_BUS_POSTED_TRANSFER
#define EIP93_BUS_POSTED_TRANSFER 0     // No posted transfers
#endif
#ifndef EIP93_BUS_MASTER_BIGENDIAN
#define EIP93_BUS_MASTER_BIGENDIAN 0    // Master Little Endian
#endif
#ifndef EIP93_BUS_MASTER_BYTE_SWAP
#define EIP93_BUS_MASTER_BYTE_SWAP 0x00 // No swap for AXI Master
#endif
#ifndef EIP93_BUS_TARGET_BYTE_SWAP
#define EIP93_BUS_TARGET_BYTE_SWAP 0x00 // No swap for AXI Slave
#endif

#elif defined(EIP93_BUS_VERSION_AHB)

// AHB bus interface
// Allowed (required) bus interface parameters:
#ifndef EIP93_BUS_MAX_BURST_SIZE
#define EIP93_BUS_MAX_BURST_SIZE 0x6    // Set an AHB default
#endif
#if (EIP93_BUS_MAX_BURST_SIZE < 1) || (EIP93_BUS_MAX_BURST_SIZE > 12)
#error "ERROR: Invalid maximum burst size"
#endif
#ifndef EIP93_BUS_INCR_ENABLE
#define EIP93_BUS_INCR_ENABLE 0         // Burst type INCR
#endif
#ifndef EIP93_BUS_LOCK_ENABLE
#define EIP93_BUS_LOCK_ENABLE 0         // No locked transfers for AHB Master
#endif
#ifndef EIP93_BUS_IDLE_ENABLE
#define EIP93_BUS_IDLE_ENABLE 0         // No IDL transfer insertion for AHB Master
#endif
#ifndef EIP93_BUS_MASTER_BIGENDIAN
#define EIP93_BUS_MASTER_BIGENDIAN 0    // Master Little Endian
#endif
#ifndef EIP93_BUS_MASTER_BYTE_SWAP
#define EIP93_BUS_MASTER_BYTE_SWAP 0xE4 // No swap for AHB Master
#endif
#if (EIP93_BUS_MASTER_BYTE_SWAP != 0xE4) && (EIP93_BUS_MASTER_BYTE_SWAP != 0x1B)
#warning "WARNING: Make sure that the Master swap setting is correct"
#endif
#ifndef EIP93_BUS_TARGET_BYTE_SWAP
#define EIP93_BUS_TARGET_BYTE_SWAP 0xE4 // No swap for AHB Slave
#endif
#if (EIP93_BUS_TARGET_BYTE_SWAP != 0xE4) && (EIP93_BUS_TARGET_BYTE_SWAP != 0x1B)
#warning "WARNING: Make sure that the Target swap setting is correct"
#endif

#else
#error "ERROR: EIP93_BUS_VERSION_[AXI|AHB] not configured"
#endif

// Enable the applicable flags in cs_eip93.h,
// when swapping of Command & Result Descriptors, Security Associations or
// data read/writes must be performed by the packet engine
#ifndef EIP93_ENABLE_SWAP_CD_RD
#define EIP93_ENABLE_SWAP_CD_RD 0       // No Command & Result Descriptors swap
#else
#undef EIP93_ENABLE_SWAP_CD_RD
#define EIP93_ENABLE_SWAP_CD_RD 1       // Swap Command & Result Descriptors
#endif
#ifndef EIP93_ENABLE_SWAP_SA
#define EIP93_ENABLE_SWAP_SA 0          // No Security Associations swap
#else
#undef EIP93_ENABLE_SWAP_SA
#define EIP93_ENABLE_SWAP_SA 1          // Swap Security Associations
#endif
#ifndef EIP93_ENABLE_SWAP_DATA
#define EIP93_ENABLE_SWAP_DATA 0        // No data swap
#else
#undef EIP93_ENABLE_SWAP_DATA
#define EIP93_ENABLE_SWAP_DATA 1        // Swap data
#endif

// Size of buffer for Direct Host Mode
#ifndef EIP93_RAM_BUFFERSIZE_BYTES
#define EIP93_RAM_BUFFERSIZE_BYTES 256 // assume EIP93-IESW
// #define EIP93_RAM_BUFFERSIZE_BYTES 2048 - for EIP93-I
#endif


/* end of file c_eip93.h */
