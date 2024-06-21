/* cs_eip93.h
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
 * Select type of driver
 */
/*  */
#define EIP93_BUS_VERSION_AHB
//#define EIP93_BUS_VERSION_AXI
//#define EIP93_ENABLE_SWAP_MASTER
//#define EIP93_ENABLE_SWAP_SLAVE


/*---------------------------------------------------------------------------
 * flag to control paramater check
 */
//#define EIP93_STRICT_ARGS 1         // uncomment when paramters check needed

// Enable the applicable bus interface
#if defined(EIP93_BUS_VERSION_AXI)

// AXI bus interface with following parameters:
#define EIP93_BUS_MAX_BURST_SIZE 4      // 16 beats
//#define EIP93_BUS_POSTED_TRANSFER 1     // Posted tranfer enabled
//#define EIP93_BUS_MASTER_BIGENDIAN 1    // Master Big Endian

// Note: Next defines are based on the EIP93_BUS_MASTER_BIGENDIAN setting
//#define EIP93_BUS_MASTER_BYTE_SWAP 0x01 // Swap for AXI Master
//#define EIP93_BUS_TARGET_BYTE_SWAP 0x01 // Swap for AXI Slave

#elif defined(EIP93_BUS_VERSION_AHB)

// AHB bus interface with following parameters:
#define EIP93_BUS_MAX_BURST_SIZE 6      // Note depends on Burst type (64 bytes)
#define EIP93_BUS_INCR_ENABLE 1         // Burst type (INC4, INC8 and INC16)
//#define EIP93_BUS_LOCK_ENABLE 1         // Locked transfers for AHB Master
#define EIP93_BUS_IDLE_ENABLE 1         // Insert IDL transfer for AHB Master
//#define EIP93_BUS_MASTER_BIGENDIAN 1    // Master Big Endian

// Note: Next defines are based on the EIP93_BUS_MASTER_BIGENDIAN setting
//#define EIP93_BUS_MASTER_BYTE_SWAP 0x1B // Swap for AHB Master
//#define EIP93_BUS_TARGET_BYTE_SWAP 0x1B // Swap for AHB Slave

#else
#error "ERROR: EIP93_BUS_VERSION_[AXI|AHB] not configured"
#endif

// Enable the applicable flags when swapping of Command & Result Descriptors,
// Security Associations or data read/writes must be performed by the packet engine
#ifdef EIP93_ENABLE_SWAP_MASTER
//#define EIP93_BUS_MASTER_BIGENDIAN 1    // Master Big Endian
#ifdef EIP93_BUS_VERSION_AXI
#define EIP93_BUS_MASTER_BYTE_SWAP 1
#else
#define EIP93_BUS_MASTER_BYTE_SWAP 0x1b
#endif
#define EIP93_ENABLE_SWAP_CD_RD 1        // Swap Command & Result Descriptors
#define EIP93_ENABLE_SWAP_SA 1           // Swap Security Associations
//#define EIP93_ENABLE_SWAP_DATA 1         // Swap data
#endif
#ifdef EIP93_ENABLE_SWAP_SLAVE
#ifdef EIP93_BUS_VERSION_AXI
#define EIP93_BUS_TARGET_BYTE_SWAP 1
#else
#define EIP93_BUS_TARGET_BYTE_SWAP 0x1b
#endif
#endif

// Size of buffer for Direct Host Mode
#define EIP93_RAM_BUFFERSIZE_BYTES 256  // for EIP93-IESW
//#define EIP93_RAM_BUFFERSIZE_BYTES 2048 // for EIP93-I

/* end of file cs_eip93.h */
