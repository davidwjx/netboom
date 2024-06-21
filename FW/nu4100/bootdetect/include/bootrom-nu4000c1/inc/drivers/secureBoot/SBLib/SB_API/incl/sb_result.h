/* sb_result.h
 *
 * Description: Secure Boot API return codes.
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

#ifndef INCLUSION_GUARD_SB_RESULT_H
#define INCLUSION_GUARD_SB_RESULT_H

/**
  Result codes. Error values are returned to the library caller.
 */
typedef enum
{
    SB_SUCCESS,                 /** Returned when a call has been successful. */

    SB_ERROR_ARGUMENTS,         /** Error: illegal arguments to function. */
    SB_ERROR_HARDWARE,          /** Error: Hardware access failure. */
    SB_ERROR_IMAGE_VERSION,     /** Error: Wrong image version. */
    SB_ERROR_IMAGE_TYPE,        /** Error: Unsupported image type. */
    SB_ERROR_VERIFICATION,      /** Error: Signature Verification failed. */
    SB_ERROR_CERTIFICATE_COUNT, /** Error: Unsupported count of certificates. */
    SB_ERROR_POLL,              /** Error: Poll function returned false. */

    SB_ERROR_COUNT              /** Number of return codes. */
}
SB_Result_t;

#endif /* Include Guard */

/* end of file sb_result.h */
