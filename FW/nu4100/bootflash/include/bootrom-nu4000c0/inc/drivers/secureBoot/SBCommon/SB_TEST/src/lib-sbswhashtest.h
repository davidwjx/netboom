/**
*  File: lib-sbswhashtest.h
*
*  Description : Prototype for sbswhashtest function, that is shared
*                by all sbswhash implementations
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

void sbswhashtest(const char * const data_p,
                  int order_data,
                  const void * const result_p,
                  const uint32_t cmplen);

/* end of file lib-sbswhashtest.h */
