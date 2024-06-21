/**
*  File: lib-sbswecdsatest.h
*
*  Description : Secure Boot ECDSA Verification Test Helper Interface.
*
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
*/

#ifndef _SB_SW_ECDSATEST_H_
#define _SB_SW_ECDSATEST_H_

void sbswecdsa(const char * const data_p,
               int order_data,
               const uint32_t * const n,
               const uint32_t * const s,
               const uint32_t * const r,
               const uint32_t * const px,
               const uint32_t * const py,
               const uint32_t * const qx,
               const uint32_t * const qy,
               bool result_expected);

#endif

/* end of file lib-sbswecdsatest.h */
