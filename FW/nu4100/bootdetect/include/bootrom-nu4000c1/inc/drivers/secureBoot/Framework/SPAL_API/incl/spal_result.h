/* spal_result.h
 *
 * Description: This header spal_result.h defines enumerated type
 *              SPAL_Resul_t, a common return type for SPAL API functions.
 */

/*****************************************************************************
* Copyright (c) 2007-2018 INSIDE Secure B.V. All Rights Reserved.
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

#ifndef INCLUDE_GUARD_SPAL_RESULT_H_
#define INCLUDE_GUARD_SPAL_RESULT_H_

#include "public_defs.h"

enum SPAL_ResultCodes
{
    SPAL_SUCCESS,
    SPAL_RESULT_NOMEM,
    SPAL_RESULT_NORESOURCE,
    SPAL_RESULT_LOCKED,
    SPAL_RESULT_INVALID,
    SPAL_RESULT_CANCELED,
    SPAL_RESULT_TIMEOUT,
    SPAL_RESULT_NOTFOUND,
    SPAL_RESULT_COUNT
};

typedef enum SPAL_ResultCodes SPAL_Result_t;

#endif /* Include guard */

/* end of file spal_result.h */
