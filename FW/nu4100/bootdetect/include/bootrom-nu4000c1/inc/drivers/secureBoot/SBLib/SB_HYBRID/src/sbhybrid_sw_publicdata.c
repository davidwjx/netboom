/* sbhybrid_sw_publicdata.c
 *
 * Description: Stub implementation of Secure Boot API.
 */

/*****************************************************************************
* Copyright (c) 2012-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "sbhybrid_internal.h"

// This stub implementation of SB_PublicData_read is for non-EIP123 and
// non-EIP130 based implementation
#if !defined(SBHYBRID_WITH_EIP123) && !defined(SBHYBRID_WITH_EIP130)
#ifndef SBLIB_CF_REMOVE_PUBLICDATA

#include "implementation_defs.h"
#include "sb_publicdata.h"              /* API to implement */

/* Interface function for reading Public Data. */
SB_Result_t
SB_PublicData_Read(uint32_t ObjectNr,
                   uint8_t * Data_p,
                   uint32_t * const DataLen_p)
{
    PARAMETER_NOT_USED(ObjectNr);
    PARAMETER_NOT_USED(Data_p);
    PARAMETER_NOT_USED(DataLen_p);

    return SB_ERROR_ARGUMENTS;
}

#endif /* !SBLIB_CF_REMOVE_PUBLICDATA */
#endif /* !defined(SBHYBRID_WITH_EIP123) && !defined(SBHYBRID_WITH_EIP130) */

/* end of file sbhybrid_sw_publicdata.c */
