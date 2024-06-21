/**
*  File: sbiftools_convert_test.c
*
*  Description : Testing conversion.
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


#include "implementation_defs.h"
#include "sfzutf.h"
#include "sbif_tools_convert.h"
#include "sbif_ecdsa.h"
#include "c_lib.h"
#include <stdlib.h>

static const uint8_t test_array224_in[] =
{
    0x42, 0x4c, 0x70, 0x02, 0x00, 0x00, 0x00, 0x01,
    0x9b, 0x0c, 0x6b, 0x90,
    0xc4, 0x2a, 0xd7, 0x8d, 0x5b, 0x8d, 0x48, 0xa3,
    0x62, 0x03, 0xe8, 0x5e, 0x2b, 0x1a, 0xc8, 0xee,
    0x87, 0x5e, 0xd6, 0xcb, 0xcb, 0x2b, 0x60, 0x16,
    0x35, 0x01, 0xbe, 0xe6, 0x25, 0xea, 0x67, 0x61,
    0xbd, 0x53, 0x7f, 0x95, 0x57, 0x8d, 0x6b, 0x28,
    0xf7, 0xc9, 0x71, 0xc5, 0xf0, 0x86, 0x10, 0x38,
    0x4b, 0xcf, 0x79, 0x8e, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xb9, 0x9f, 0xff, 0x30,
    0x0d, 0x2a, 0x8d, 0x54, 0xb6, 0x70, 0xeb, 0xf9,
    0xed, 0x9a, 0xa6, 0x6b, 0x6a, 0x35, 0x59, 0x36,
    0x4f, 0xea, 0x5e, 0x7f, 0xc6, 0xb3, 0x04, 0xcd,
    0xe6, 0x01, 0x3a, 0x89, 0xdc, 0x23, 0x9d, 0x1d,
    0xd1, 0x73, 0x1f, 0x3c, 0xea, 0xe9, 0x70, 0xcb,
    0xec, 0x6a, 0xe5, 0x25, 0x90, 0xe6, 0x7c, 0xbc,
    0xa9, 0xd0, 0x1c, 0x26, 0x46, 0xf9, 0xa6, 0x1d,
    0x06, 0x01, 0x3e, 0x00
};

static uint32_t test_array224_aligned[sizeof(test_array224_in) /
                                      sizeof(uint32_t)];

static uint32_t test_array224_out[sizeof(test_array224_in) /
                                  sizeof(uint32_t)];

static uint32_t test_array224_out_2[sizeof(test_array224_in) /
                                    sizeof(uint32_t)];

static void *get_workspace(void)
{
    SBIFTC_Result_t res;
    void *workspace_p = NULL;
    uint32_t workspace_size = 0;

    res = SBIFTOOLS_Image_Convert_InitWorkspace(workspace_p, &workspace_size);
    if (workspace_size > 0)
    {
        /* Retrieved desired workspace size. */
        workspace_p = malloc(workspace_size);
        fail_if(!workspace_p && workspace_size > 0, "Memory exhausted");

        res = SBIFTOOLS_Image_Convert_InitWorkspace(workspace_p,
                &workspace_size);
    }

    fail_unless(res == SBIFTC_SUCCESS,
                "Error: Unable to perform cryptographic operations");

    return workspace_p;
}

static void free_workspace(void *Workspace_p)
{
    SBIFTOOLS_Image_Convert_UninitWorkspace(Workspace_p);
}

START_TEST(test_SBIFTOOLS_Image_Convert_Workspace)
{
    void *workspace_p;
    workspace_p = get_workspace();
    free_workspace(workspace_p);
}
END_TEST

START_TEST(test_SBIFTOOLS_Image_Convert_to_BLe)
{
    SBIF_SGVector_t vec[1];
    SBIF_SGVector_t vec2[1];
    SBIFTC_Result_t res;
    void *workspace_p;

    const SBIF_ECDSA_Header_t * const header_p =
        (const SBIF_ECDSA_Header_t *) test_array224_aligned;

    SBIF_ECDSA_Header_t *headerNew_p =
        (SBIF_ECDSA_Header_t *) test_array224_out;
    size_t headerNewSize = sizeof(*headerNew_p);

    c_memcpy(test_array224_aligned, test_array224_in, sizeof(test_array224_aligned));

    c_memset(test_array224_out, 0xAA, sizeof(test_array224_out));

    vec[0].Data_p = (uint32_t *)(((char *)(test_array224_aligned)) + sizeof(*header_p));
    vec[0].DataLen = sizeof(test_array224_aligned) - sizeof(*header_p);

    vec2[0].Data_p = (uint32_t *)(((char *)(test_array224_out)) + sizeof(*header_p));
    vec2[0].DataLen = sizeof(test_array224_out) - sizeof(*header_p);

    workspace_p = get_workspace();
    res = SBIFTOOLS_Image_Convert(header_p,
                                  SBIF_IMAGE_BLTe,
                                  SBIF_VERSION,
                                  headerNew_p,
                                  &headerNewSize,
                                  vec, vec2, 1, NULL, 0,
                                  workspace_p);
    free_workspace(workspace_p);

    fail_unless(res == SBIFTC_SUCCESS, "Image conversion failed.");
    fail_if(c_memcmp(vec[0].Data_p, vec2[0].Data_p, vec[0].DataLen) == 0,
            "No encryption performed in BLp => BLe conversion.");
}
END_TEST

START_TEST(test_SBIFTOOLS_Image_Convert_to_BLp)
{
    /* Notice: this test is based on output of
       test_SBIFTOOLS_Image_Convert_to_BLe. Therefore, if that test failed,
       this test shall fail as well. Also, in some cases, it is possible that
       error of previous test is detected by this test. */

    SBIF_SGVector_t vec[1];
    SBIF_SGVector_t vec2[1];
    SBIFTC_Result_t res;
    void *workspace_p;

    const SBIF_ECDSA_Header_t * const header_p =
        (const SBIF_ECDSA_Header_t *) test_array224_out;

    SBIF_ECDSA_Header_t *headerNew_p =
        (SBIF_ECDSA_Header_t *) test_array224_out_2;
    size_t headerNewSize = sizeof(*headerNew_p);

    c_memset(test_array224_out_2, 0xAA, sizeof(test_array224_out_2));

    vec[0].Data_p = (uint32_t *)(((char *)(test_array224_out)) + sizeof(*header_p));
    vec[0].DataLen = sizeof(test_array224_out) - sizeof(*header_p);

    vec2[0].Data_p = (uint32_t *)(((char *)(test_array224_out_2)) + sizeof(*header_p));
    vec2[0].DataLen = sizeof(test_array224_out_2) - sizeof(*header_p);

    workspace_p = get_workspace();
    res = SBIFTOOLS_Image_Convert(header_p,
                                  SBIF_IMAGE_BLTp,
                                  SBIF_VERSION,
                                  headerNew_p,
                                  &headerNewSize,
                                  vec, vec2, 1, NULL, 0,
                                  workspace_p);
    free_workspace(workspace_p);

    fail_unless(res == SBIFTC_SUCCESS, "Image conversion failed.");
    fail_unless(c_memcmp(test_array224_in, test_array224_out_2,
                         sizeof(test_array224_in)) == 0,
                "Image comparison failed BLp->BLe->BLp");
}
END_TEST

void build_suite(void)
{
    sfzutf_suite_create("SBIFTOOLS_tests");
    sfzutf_tcase_create("SBIFTOOLS_Image_Convert_tests");
    sfzutf_test_add(test_SBIFTOOLS_Image_Convert_Workspace);
    sfzutf_test_add(test_SBIFTOOLS_Image_Convert_to_BLe);
    sfzutf_test_add(test_SBIFTOOLS_Image_Convert_to_BLp);
}
