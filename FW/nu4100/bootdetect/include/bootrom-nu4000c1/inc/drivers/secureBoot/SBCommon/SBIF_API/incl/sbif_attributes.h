/* sbif_attributes.h
 *
 * Description: API for dealing with different SBIF attributes in
 *              Secure Boot image.
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

#ifndef INCLUDE_GUARD_SBIF_ATTRIBUTES_H
#define INCLUDE_GUARD_SBIF_ATTRIBUTES_H

#include "public_defs.h"
#include "cfg_sbif.h"
#include "sbif_ecdsa.h"

bool SBIF_Attributes_Add(SBIF_Attributes_t * const   Attributes_p,
                         SBIF_AttributeElementType_t ElementType,
                         uint32_t                    ElementValue);

bool SBIF_Attribute_Fetch(const SBIF_Attributes_t * const Attributes_p,
                          SBIF_AttributeElementType_t     ElementType,
                          uint32_t * const                ElementValue_p);

bool SBIF_Attribute_Check(const SBIF_Attributes_t * const Attributes_p);

#endif /* Include Guard */

/* end of file sbif_attributes.h */
