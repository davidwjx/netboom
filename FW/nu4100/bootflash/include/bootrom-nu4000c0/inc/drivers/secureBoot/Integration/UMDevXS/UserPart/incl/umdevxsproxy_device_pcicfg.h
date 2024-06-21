/* umdevxsproxy_device_pcicfg.h
 *
 * UMDevXS Proxy interface for reading and writing the PCI Configuration Space.
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

#ifndef UMDEVXSPROXY_DEVICE_PCICFG_H_
#define UMDEVXSPROXY_DEVICE_PCICFG_H_

/*----------------------------------------------------------------------------
 * This module uses (requires) the following interface(s):
 */

#include "basic_defs.h"



/*----------------------------------------------------------------------------
 * UMDevXSProxy_Device_PciCfg_Read32
 *
 * This function can be used to read a 32-bit integer at the specified
 * byte offset from the PCI Configuration Space
 *
 * ByteOffset
 *     Byte offset to read at.
 *
 * Int32_p (output)
 *     Pointer to memory location where the 32-bit integer will be stored.
 *
 * Return Value
 *     0  Success
 *    -1  Failed to execute the request
 */
int
UMDevXSProxy_Device_PciCfg_Read32(
    const unsigned int ByteOffset,
    uint32_t * const Int32_p);


/*----------------------------------------------------------------------------
 * UMDevXSProxy_Device_PciCfg_Write32
 *
 * This function can be used to write a 32-bit integer at the specified
 * byte offset from the PCI Configuration Space
 *
 * ByteOffset
 *     Byte offset to write at.
 *
 * Int32
 *     32-bit integer value to write.
 *
 * Return Value
 *     0  Success
 *    -1  Failed to execute the request
 */
int
UMDevXSProxy_Device_PciCfg_Write32(
    const unsigned int ByteOffset,
    const uint32_t Int32);


#endif /* UMDEVXSPROXY_DEVICE_PCICFG_H_ */

/* end of file umdevxsproxy_device_pcicfg.h */
