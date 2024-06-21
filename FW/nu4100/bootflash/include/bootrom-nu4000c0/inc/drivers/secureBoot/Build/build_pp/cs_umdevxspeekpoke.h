/* cs_umdevxspeekpoke.h
 *
 * Configuration Switches
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

#define UMDEVXSPEEKPOKE_SHMEM_PROVIDER
#define UMDEVXSPEEKPOKE_SHMEM_OBTAINER
#define UMDEVXSPEEKPOKE_INTERRUPT

// some platforms need to manipulate the address retrieved from the GBI
// comment-out if your platform does not need this
//#define UMDEVXSPEEKPOKE_GBI_ADDR_ADD  0x00000000

/* end of file cs_umdevxspeekpoke.h */
