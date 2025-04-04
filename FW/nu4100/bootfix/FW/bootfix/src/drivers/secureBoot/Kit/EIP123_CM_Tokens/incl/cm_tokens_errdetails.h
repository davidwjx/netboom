/* cm_tokens_errdetails.h
 *
 * Crypto Module Tokens Parser/Generator - Error Details
 *
 * This module can converts a set of parameters into a Crypto Module Command
 * token, or parses a set of parameters from a Crypto Module Response token.
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

#ifndef INCLUDE_GUARD_CM_TOKENS_ERRDETAILS_H
#define INCLUDE_GUARD_CM_TOKENS_ERRDETAILS_H

#include "basic_defs.h"         // uint32_t, bool, inline, etc.
#include "cm_tokens_common.h"   // CMTokens_Response_t

enum
{
    CMTOKENS_RESULT_SEQ_INVALID_TOKEN = 1,
    CMTOKENS_RESULT_SEQ_INVALID_PARAMETER = 2,
    CMTOKENS_RESULT_SEQ_INVALID_KEYSIZE = 3,
    CMTOKENS_RESULT_SEQ_INVALID_LENGTH = 4,
    CMTOKENS_RESULT_SEQ_INVALID_LOCATION = 5,
    CMTOKENS_RESULT_SEQ_CLOCK_ERROR = 6,
    CMTOKENS_RESULT_SEQ_ASSET_BLOCKED = 7,
    CMTOKENS_RESULT_SEQ_UNWRAP_ERROR = 10,
    CMTOKENS_RESULT_SEQ_DATA_OVERRUN = 11,
    CMTOKENS_RESULT_SEQ_ASSET_CHECKSUM = 12,
    CMTOKENS_RESULT_SEQ_INVALID_ASSET = 13,
    CMTOKENS_RESULT_SEQ_ASSETSTORE_FULL = 14,
    CMTOKENS_RESULT_SEQ_INVALID_ADDRESS = 15,
    CMTOKENS_RESULT_SEQ_ENGINE_NOT_AVAILABLE = 16,
    CMTOKENS_RESULT_SEQ_INVALID_MODULUS = 17,
    CMTOKENS_RESULT_SEQ_VERIFY_ERROR = 18,
    CMTOKENS_RESULT_SEQ_PANIC = 31
};


/*----------------------------------------------------------------------------
 * CMTokens_ParseResponse_ErrorDetails
 *
 * This function translates the error code in a response token to an
 * enumerated list of errors plus a textual description. The textual
 * description can be disabled compile-time, in which case the string "?" will
 * be returned.
 *
 * ErrorText_pp (optional)
 *     Returns a pointer to descriptive text for the error code.
 *     Error message = zero-terminated string.
 *     Optional, thus OK to pass NULL.
 *     Returns "n/a" when CMTOKENS_REMOVE_ERROR_DESCRIPTIONS is defined.
 *
 * Return Value:
 *     <0    Error processing request
 *     >=0   The Error Number found in the response token
 *
 * Example:
 *    const char * p = NULL;
 *    int ErrorNr = CMTokens_ParseResponse_ErrorDetails(ResponseToken_p, &p);
 *    printf("Error: %d (%s)\n", ErrorNr, p);
 */
int
CMTokens_ParseResponse_ErrorDetails(
    CMTokens_Response_t * const ResponseToken_p,
    const char ** const ErrorText_pp);


/*----------------------------------------------------------------------------
 * CMTokens_ParseResponse_NvmErrorDetails
 *
 * This function translates the specific NVM error code (returned in the
 * Version Info token) into an error message. A pointer to the string is
 * returned, or a pointer to "n/a" when the detailed error messages are not
 * present, or the code is unknown.
 */
const char *
CMTokens_ParseResponse_NvmErrorDetails(
    unsigned int NvmErrorCode);


#endif /* Include Guard */

/* end of file cm_tokens_errdetails.h */
