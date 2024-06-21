/**
* File: minicryptoki-suite.c
*
* Description: Tests minicryptoki. Tests include only algorithms appropriate
*              for minicryptoki.a.
*
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
*/

/* Substitute p11-test-utils.c with functions that does not attempt C_Login. */
#include "cryptoki.h"
#include "sfzutf.h"
CK_SESSION_HANDLE P11_TEST_session;

void
P11_TEST_create_session (void)
{
    CK_SLOT_ID slot_id = 3;
    CK_FLAGS flags = CKF_SERIAL_SESSION;
    CK_VOID_PTR app = NULL_PTR;
    CK_NOTIFY notify = NULL_PTR;
    CK_RV rv;

    rv = C_Initialize (0);
    fail_if (rv != CKR_OK, "C_Initialize: Failed");

    rv = C_OpenSession (slot_id, flags, app, notify, &P11_TEST_session);
    fail_if (rv != CKR_OK, "C_OpenSession: Failed");
}

void
P11_TEST_delete_session (void)
{
    CK_RV rv;

    rv = C_CloseSession (P11_TEST_session);
    fail_if (rv != CKR_OK, "C_CloseSession: Failed");

    rv = C_Finalize (0);
    fail_if (rv != CKR_OK, "C_Finalize: Failed");
}

/* Add tests. */
void suite_add_mechanism_aes_nonsensitive(void);
void suite_add_mechanism_digest_sha224_only(void);
void suite_add_mechanism_digest_sha256_only(void);
void suite_add_test_func62_p224_nosha1(void);
void suite_add_test_func62_p256_nosha1(void);

/* Build actual test suite. */
void
build_suite (void)
{
    sfzutf_suite_create ("Mini_PKCS11_TestCase");
    suite_add_mechanism_aes_nonsensitive ();
    suite_add_mechanism_digest_sha224_only ();
    suite_add_mechanism_digest_sha256_only ();
    suite_add_test_func62_p224_nosha1();
    suite_add_test_func62_p256_nosha1();
}

/* end of file minicryptoki-suite.c */
