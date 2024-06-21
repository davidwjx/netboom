/* dmares_record.h
 *
 * DMAResource Record Definition for use with the 'barebones' Driver Framework
 * implementation.
 *
 * The document "Driver Framework Porting Guide" contains the detailed
 * specification of this API.
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


/*----------------------------------------------------------------------------
 * AddrTrans_Domain_t
 *
 * This is a list of domains that can be supported by the implementation. The
 * exact meaning can be different for different EIP devices and different
 * environments.
 */
enum
{
    DMARES_DOMAIN_HOST = 0,
    DMARES_DOMAIN_UNIFIED,
    DMARES_DOMAIN_EIP12xDMA,
    DMARES_DOMAIN_EIP13xDMA,
    DMARES_DOMAIN_EIP93DMA
};

// this implementation does not support a DMAResource Record
typedef void DMAResource_Record_t;


/* end of file dmares_record.h */
