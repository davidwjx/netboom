/* sbif_attributes_add.c
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
Load_BE32(const void * const Value_p)
{
    const uint8_t * const p = (const uint8_t *) Value_p;

    return (p[0] << 24 | p[1] << 16 | p[2] << 8  | p[3]);
}


/*----------------------------------------------------------------------------
 * Store_BE32
 */
static inline void
Store_BE32(uint32_t * const Value_p,
           const uint32_t NewValue)
{
    uint8_t * const Value8_p = (uint8_t *) Value_p;
    Value8_p[0] = NewValue >> 24;
    Value8_p[1] = (uint8_t) (NewValue >> 16);
    Value8_p[2] = (uint8_t) (NewValue >> 8);
    Value8_p[3] = (uint8_t) NewValue;
}


/*----------------------------------------------------------------------------
 * SBIF_Attributes_Add
 */
bool
SBIF_Attributes_Add(SBIF_Attributes_t * const   Attributes_p,
                    SBIF_AttributeElementType_t ElementType,
                    uint32_t                    ElementValue)
{
    int i;

    L_TRACE(LF_SBIF,
            "ElementType=0x%08x; ElementValue=0x%08x",
            (unsigned int)ElementType, ElementValue);

    /* Check extension space (free slots) */
    if (Attributes_p->AttributeElements[SBIF_NUM_ATTRIBUTES - 1].ElementType != 0)
    {
        return false;
    }

#ifdef SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID
    L_TRACE(LF_SBIF,
            "SBIF_ATTRIBUTE_ROLLBACK_ID=0x%08x; "
            "SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID=0x%08x",
            SBIF_ATTRIBUTE_ROLLBACK_ID,
            SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID);

    /* Check attribute value (ROLLBACK ID attribute only) */
#if SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID > 0
    if ((ElementType == SBIF_ATTRIBUTE_ROLLBACK_ID) &&
        (ElementValue < SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID))
    {
        L_DEBUG(LF_SBIF,
                "Rejecting attribute due to minimum rollback ID (%d: %d < %d)\n",
                ElementType, ElementValue, SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID);
        return false;
    }
#endif /* SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID > 0 */
#endif /* SBIF_ATTRIBUTE_MINIMUM_ROLLBACK_ID */

    /* Scan for insertion point. */
    for(i = 0; i < (SBIF_NUM_ATTRIBUTES - 1); i++)
    {
        SBIF_AttributeElementType_t Type;

        Type = Load_BE32(&Attributes_p->AttributeElements[i].ElementType);
        if (Type == ElementType)
        {
            L_TRACE(LF_SBIF, "Rejecting due to dupe\n");
            return false;
        }

        if (Type > ElementType)
        {
            break;
        }

        if (Type == SBIF_ATTRIBUTE_UNUSED)
        {
            break;
        }
    }

    /* If the attribute is not the last attribute. */
    if (i < (SBIF_NUM_ATTRIBUTES -1))
    {
        int l;

        /* Move next attributes one step further away. */
        for(l = (SBIF_NUM_ATTRIBUTES - 1); l > i; l--)
        {
            Attributes_p->AttributeElements[l].ElementType =
                Attributes_p->AttributeElements[l - 1].ElementType;
            Attributes_p->AttributeElements[l].ElementValue =
                Attributes_p->AttributeElements[l - 1].ElementValue;
        }
    }

    /* Insert new attribute. */
    Store_BE32(&Attributes_p->AttributeElements[i].ElementType, ElementType);
    Store_BE32(&Attributes_p->AttributeElements[i].ElementValue, ElementValue);

    return true;
}

/* end of file sbif_attributes_add.c */
