/* dftest.c
 *
 * Description: test suite for Driver Framework.
 */

/*****************************************************************************
* Copyright (c) 2011-2018 INSIDE Secure B.V. All Rights Reserved.
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

#include "sfzutf.h"
#include "cs_dftest.h"

#ifndef DFTEST_REMOVE_BASIC_DEFINES
void suite_add_test_basic_defs(void);
#endif
#ifndef DFTEST_REMOVE_DEVICE_APIS
void suite_add_test_device_apis(void);
#endif
#ifndef DFTEST_REMOVE_DMA_RESOURCE_APIS
void suite_add_test_dma_resource_apis(void);
#endif
#ifndef DFTEST_REMOVE_CLIB
void suite_add_test_clib(void);
#endif

void build_suite(void)
{
    sfzutf_suite_create("Driver Framework Test Suite");
#ifndef DFTEST_REMOVE_BASIC_DEFINES
    suite_add_test_basic_defs();
#endif
#ifndef DFTEST_REMOVE_CLIB
    suite_add_test_clib();
#endif
#ifndef DFTEST_REMOVE_DEVICE_APIS
    suite_add_test_device_apis();
#endif
#ifndef DFTEST_REMOVE_DMA_RESOURCE_APIS
    suite_add_test_dma_resource_apis();
#endif
}

/* end of file dftest.c */
