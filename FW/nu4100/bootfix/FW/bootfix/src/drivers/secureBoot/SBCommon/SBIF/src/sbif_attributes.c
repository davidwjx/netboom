/* sbif_attributes.c
 *
 * Description: Tools for dealing with different SBIF attributes in
 *              Secure Boot image. Implementation.
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

#include "implementation_defs.h"
#include "cfg_sbif.h"
#include "sbif_ecdsa.h"


/*----------------------------------------------------------------------------
 * Load_BE32
 */
static inline uint32_t
Load_BE32(
        const void * const Value_p)
{
    const uint8_t * const p = (const uint8_t *)Value_p;

    return (p[0] << 24 | p[1] << 16 | p[2] << 8  | p[3]);
}


/*----------------------------------------------------------------------------
 * SBIF_Attribute_Fetch
 */
bool
SBIF_Attribute_Fetch(
        const SBIF_Attributes_t * const Attributes_p,
        SBIF_AttributeElementType_t     ElementType,
        uint32_t * const                ElementValue_p)
{
    int i;

    /* Scan for insertion point. */
    for(i = 0; i < SBIF_NUM_ATTRIBUTES; i++)
    {
        SBIF_AttributeElementType_t type =
            Load_BE32(&Attributes_p->AttributeElements[i].ElementType);

        if (type == ElementType)
        {
            *ElementValue_p =
                Load_BE32(&Attributes_p->AttributeElements[i].ElementValue);
            return true;
        }
    }

    return false;
}


/*----------------------------------------------------------------------------
 * SBIF_Attribute_Check
 */
bool
SBIF_Attribute_Check(
        const SBIF_Attributes_t * const Attributes_p)
{
    SBIF_AttributeElementType_t type;
    int i;

    /* Get first attribute. */
    type = Load_BE32(&Attributes_p->AttributeElements[0].ElementType);

    /* Check there is version attribute with correct value. */
    if ((type != SBIF_ATTRIBUTE_VERSION) ||
        (Load_BE32(&Attributes_p->AttributeElements[0].ElementValue) !=
         SBIF_ATTRIBUTE_VERSION_CURRENT))
    {
        return false;
    }

    for(i = 1; i < SBIF_NUM_ATTRIBUTES; i++)
    {
        SBIF_AttributeElementType_t nextType;

        nextType = Load_BE32(&Attributes_p->AttributeElements[i].ElementType);
        if (nextType != SBIF_ATTRIBUTE_UNUSED)
        {
            /* Check attribute order or dup. */
            if (nextType <= type)
            {
                return false;
            }

            /* Check for unknown non-extension attributes. */
            if ((nextType > SBIF_ATTRIBUTE_ROLLBACK_ID) &&
                ((nextType & 0x80000000) == 0))
            {
                return false;
            }

#ifdef SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID
            /* Check attribute value (ROLLBACK ID attribute only) */
#if SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID > 0
            if (nextType == SBIF_ATTRIBUTE_ROLLBACK_ID)
            {
                if (Load_BE32(&Attributes_p->AttributeElements[i].ElementValue) <
                        SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID)
                {
                    return false;
                }
            }
#endif /* SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID > 0 */
#endif /* SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID */
        }

        type = nextType;
    }

    return true;
}

/* end of file sbif_attributes.c */
