/* cs_sbsim.h
 *
 * Configuration Switches for the SBSim Application
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

// the following switch asks SBSim to allocate the image memory provided to
// SB_ECDSA_Image_DecryptAndVerify() from the DMA Resource API of Driver
// Framework. This is required for the Crypto Module DMA buffers.
// The Driver Framework _implementation_ can be customized for your solution
// without having to touch the Secure Boot Libary.
// Details can be found in the Driver Framework Porting Guide
#define SBSIM_ALLOCATOR_DMARESOURCE

// This switch selects between calling
//   SB_ECDSA_Image_DecryptAndVerify and
//   SB_ECDSA_Image_Verify
// SB_CM only supports the first
#define SBSIM_WITH_DECRYPT

// this switch select between a single buffer (in-place) and using two buffers
// in case of decryption, in-place overwrites the original image.
// This switch is meaningful only when SBSIM_WITH_DECRYPT is set
#define SBSIM_NOT_INPLACE

/* end of file cs_sbsim.h */
