/**
*  File: sbif_vector.h
*
*  Description: Definition of data type to describe
*               multipart data (scatter-gather).
*
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
*/

#ifndef INCLUSION_GUARD_SBIF_VECTOR_H
#define INCLUSION_GUARD_SBIF_VECTOR_H

#include "public_defs.h"

/** Vector. */
typedef struct
{
    uint32_t  *Data_p;          /** Data. */
    uint32_t   DataLen;         /** Data length. */
}
SBIF_SGVector_t;

#endif /* INCLUSION_GUARD_SBIF_VECTOR_H */
