/* sb_system.h
 *
 * Description: Secure Boot API: System info access
 */

/*****************************************************************************
* Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_SB_SYSTEM_H
#define INCLUDE_GUARD_SB_SYSTEM_H

/* Internal includes. */
#include "public_defs.h"

/** Obtain system information.

    Secure Boot supports system info for Secure Boot purposes.

    @param HWVersion
    Pointer to the variable that contains the hardware version
    upon successful return.

    @param FWVersion
    Pointer to the variable that contains the firmware version
    upon successful return.

    @param MemSize
    Pointer to the variable that contains the size of the memory
    of the SB hardware upon successful return.

    @param ErrorInfo
    Pointer to the variable that contains the error information
    and location upon successful return.

    @return
    Returns SB_SUCCESS on success. All other values indicate that
    an error has occurred. There is one error code that can be
    seen:
     * SB_ERROR_HARDWARE when there is error fetching the object.
       Error may occur due to object type (not PublicData) or access
       permissions or HW, DMA, software misconfiguration.
 */
SB_Result_t
SB_SystemInfo_Read(
    uint32_t * const HWVersion,
    uint32_t * const FWVersion,
    uint32_t * const MemSize,
    uint32_t * const ErrorInfo);

#endif /* Include guard */

/* end of file sb_system.h */
