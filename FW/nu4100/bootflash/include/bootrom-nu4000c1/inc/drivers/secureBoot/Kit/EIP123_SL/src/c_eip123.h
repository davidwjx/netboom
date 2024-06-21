/* c_eip123.h
 *
 * Configuration options for the EIP123 module.
 * The project-specific cs_eip123.h file is included,
 * whereafter defaults are provided for missing parameters.
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

/*----------------------------------------------------------------
 * Defines that can be used in the cs_xxx.h file
 */

/* currently none */


/*----------------------------------------------------------------
 * inclusion of cs_eip123.h
 */
#include "cs_eip123.h"


/*----------------------------------------------------------------
 * provide backup values for all missing configuration parameters
 */

#ifndef EIP123_MAX_PHYSICAL_FRAGMENTS
#define EIP123_MAX_PHYSICAL_FRAGMENTS  8
#endif


/*----------------------------------------------------------------
 * other configuration parameters that cannot be set in cs_xxx.h
 * but are considered product-configurable anyway
 */


/* end of file c_eip123.h */
