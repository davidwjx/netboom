/* c_umdevxsproxy.h
 *
 * Configuration options for UMDevXS Proxy Library.
 *
 * This file includes cs_umdevxsproxy.h (from the product-level) and then
 * provides defaults for missing configuration switches.
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

#ifndef INCLUDE_GUARD_C_UMDEVXSPROXY_H
#define INCLUDE_GUARD_C_UMDEVXSPROXY_H

// get the product-level configuration
#include "cs_umdevxsproxy.h"

#ifndef UMDEVXSPROXY_LOG_PREFIX
#define UMDEVXSPROXY_LOG_PREFIX "UMDevXSProxy: "
#endif

#ifndef UMDEVXSPROXY_NODENAME
#define UMDEVXSPROXY_NODENAME "//dev//umpci_c"
#endif

#endif /* INCLUDE_GUARD_C_UMDEVXSPROXY_H */

/* end of file c_umdevxsproxy.h */
