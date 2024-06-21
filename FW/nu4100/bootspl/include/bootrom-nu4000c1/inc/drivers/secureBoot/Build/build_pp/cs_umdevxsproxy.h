/* cs_umdevxsproxy.h
 *
 * Configuration Switches for the UMDevXS Proxy Library.
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

#define UMDEVXSPROXY_LOG_PREFIX "UMDevXSProxy: "

#define UMDEVXSPROXY_NODE_NAME "//dev//umdevxs_c"

// uncomment to remove selected functionality
//#define UMDEVXSPROXY_REMOVE_DEVICE
//#define UMDEVXSPROXY_REMOVE_SMBUF
//#define UMDEVXSPROXY_REMOVE_INTERRUPT
#define UMDEVXSPROXY_REMOVE_PCICFG

/* end of file cs_umdevxsproxy.h */
