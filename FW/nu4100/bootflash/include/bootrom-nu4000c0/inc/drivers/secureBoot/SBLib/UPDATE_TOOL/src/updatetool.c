/**
*  File: updatetool.c
*
*  Description: Implementation of application which processes BLp/BLx/BLe to
*               BLw images (i.e. attachs various secure boot image formats to
*               confidentiality root of trust.)
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

#include "implementation_defs.h"
#include "sbif_tools_convert.h"
#include "sb_result.h"
#include "sb_ecdsa.h"
#include "sb_sw_endian.h"
#include "c_lib.h"              // c_strcmp
#include <stdio.h>
#include <stdlib.h>

const char *
Sbiftc_Result_Map(const SBIFTC_Result_t Res)
{
    switch (Res)
    {
    case SBIFTC_SUCCESS:
        return "success";
    case SBIFTC_ERROR_ARGUMENTS:
        return "illegal arguments";
    case SBIFTC_ERROR_CRYPTO:
        return "crypto operation failure";
    case SBIFTC_ERROR_IMAGE_VERSION:
        return "wrong boot image version";
    case SBIFTC_ERROR_IMAGE_TYPE:
        return "wrong boot image type";
    }
    return "internal error";
}


/* Read binary file. */
bool
read_binary(const char * const FileName,
            uint8_t ** Bin_pp,
            size_t * BinLength_p)
{
    FILE * f;
    uint8_t * p = NULL;
    void * new_p;
    size_t sz = 32768;
    size_t start = 0;

    *Bin_pp = NULL;
    *BinLength_p = 0;

    f = fopen(FileName, "r");
    if (f == NULL)
    {
        return false;
    }

    /* Allocate space for object, always allocates a power of two. */
    do
    {
        sz *= 2;
        new_p = realloc(p, sz);
        if (new_p == NULL)
        {
            free(p);
            fclose(f);
            return false;
        }
        p = new_p;
        start += fread(p + start, 1, sz - start, f);
    } while (start == sz);

    /* Check errors. */
    if (ferror(f))
    {
        fclose(f);
        free(p);
        return false;
    }

    /* Close file. */
    fclose(f);

    /* Release unused memory (truncate buffer to actual used size) */
    if (start > 0)
    {
        new_p = realloc(p, start);
        if (new_p)
        {
            p = new_p;
        }
    }
    else
    {
        free(new_p);                    /* Release all memory allocated */
        p = (uint8_t *)"";              /* Provide non-null pointer. */
    }

    /* Return buffer. */
    *Bin_pp = p;
    *BinLength_p = start;

    return true;
}

/* Write binary file. */
bool
write_binary(const char * const FileName,
             const uint8_t * const Bin_p,
             const size_t Size)
{
    FILE * f;
    bool success = true;

    f = fopen(FileName, "w");
    if (f == NULL)
    {
        return false;
    }

    if (Size != 0)
    {
        if (fwrite(Bin_p, Size, 1, f) != 1)
        {
            success = false;
        }
    }

    fclose(f);

    /* Attempt to remove broken file on errors. */
    if (success == false)
    {
        remove(FileName);
    }

    return success;
}

static SB_StorageArea_t StorageArea;

static bool
header_is_integrity_only(SBIF_ECDSA_Header_t * const Header_p)
{
    uint32_t image_type = SB_SW_BE32_READ(&Header_p->Type);

    image_type = SBIF_TYPE_TYPE(image_type);

    return image_type == SBIF_IMAGE_BLTp;
}

static
SB_Result_t sbverify(SBIF_ECDSA_Header_t * const Header_p,
                     const size_t ImageSize,
                     const void * const PublicKey_p)
{
    SBIF_SGVector_t dataVectors[1];
    const unsigned int headerLen = SBIF_ECDSA_GET_HEADER_SIZE(Header_p, ImageSize);
    const unsigned int dataLen = ImageSize - headerLen;

    if (headerLen == 0)
    {
        return SB_ERROR_ARGUMENTS;
    }

    dataVectors[0].Data_p = (void *)(((char *)Header_p) + headerLen);
    dataVectors[0].DataLen = dataLen;

    return SB_ECDSA_Image_Verify(&StorageArea,
                                 NULL,
                                 PublicKey_p,
                                 NULL,
                                 Header_p,
                                 dataVectors, 1);
}

int
main(int argc, char * argv[])
{
    bool success;
    uint8_t * ptr;
    size_t sz;
    SB_Result_t res_sb;
    SBIFTC_Result_t res_sbiftc;
    void * workspace_p = NULL;
    uint32_t workspace_size = 0;
    uint8_t * aesKey = NULL;
    size_t aesKeyLen;
    uint8_t * pubKey = NULL;
    size_t pubKeySize;
    SBIFTOOLS_ConvertOption_t convOptTable[1];
    uint32_t convOptCount = 0;

    if ((argc != 4) &&
        ((argc != 6) || (c_strcmp(argv[1], "--keyfile") != 0)))
    {
        fprintf(stderr,
                "Usage: %s [--keyfile <keyfile>] <public key file> "
                "<image file> <output file>\n", argv[0]);
        exit(1);
    }

    if (argc == 6)
    {
        /* Read --keyfile and its argument. */
        success = read_binary(argv[2], &aesKey, &aesKeyLen);
        if (!success)
        {
            fprintf(stderr, "ERROR: Could not read key file %s.\n", argv[2]);
            exit(1);
        }

        /* Fill the conversion option. */
        convOptTable[0].ConvertOption = SBIFTOOLS_CONVERT_OPTION_AES_DECRYPTION_KEY;
        convOptTable[0].ConvertOptionPointer = aesKey;
        convOptTable[0].ConvertOptionLength = (uint32_t)aesKeyLen;
        convOptCount = 1;

        /* Remove the option and its argument. */
        //argc -= 2;
        argv += 2;
    }

    success = read_binary(argv[1], &pubKey, &pubKeySize);
    if (!success || pubKey == NULL || pubKeySize == 0)
    {
        fprintf(stderr, "ERROR: Could not read public key file %s.\n", argv[1]);
        exit(1);
    }

    success = read_binary(argv[2], &ptr, &sz);
    if (ptr == NULL)
    {
        fprintf(stderr, "ERROR: Could not read input file %s.\n", argv[2]);
        exit(1);
    }
    if (success && (sz > sizeof(SBIF_ECDSA_Header_t)))
    {
        SBIF_ECDSA_Header_t * const header_p = (void *)ptr;
        size_t hdrSize = SBIF_ECDSA_GET_HEADER_SIZE(header_p, sz);
        SBIF_SGVector_t vec;

        if (hdrSize == 0)
        {
            fprintf(stderr, "ERROR: Input file %s is corrupted.\n", argv[2]);
            exit(1);
        }

        // get the required size of the workspace
        res_sbiftc = SBIFTOOLS_Image_Convert_InitWorkspace(NULL, &workspace_size);
        if (workspace_size > 0)
        {
            /* Retrieved desired workspace size. */
            workspace_p = malloc(workspace_size);
            if (workspace_p == NULL)
            {
                fprintf(stderr, "ERROR: Memory exhausted.\n");
                exit(1);
            }

            res_sbiftc = SBIFTOOLS_Image_Convert_InitWorkspace(workspace_p,
                                                               &workspace_size);
        }
        if (res_sbiftc != SBIFTC_SUCCESS)
        {
            fprintf(stderr,
                    "ERROR: Could not perform cryptographic operations.\n");
            exit(1);
        }

        vec.Data_p = (void *)(ptr + hdrSize);
        vec.DataLen = sz - hdrSize;

        if (convOptCount > 0 || !header_is_integrity_only(header_p))
        {
            /* Convert to BLp format if neccessary. */

            res_sbiftc = SBIFTOOLS_Image_Convert(header_p,
                                                 SBIF_IMAGE_BLTp,
                                                 SBIF_VERSION,
                                                 header_p,
                                                 &hdrSize,
                                                 &vec,
                                                 &vec,
                                                 1,
                                                 convOptTable,
                                                 convOptCount,
                                                 workspace_p);
            if (res_sbiftc != SBIFTC_SUCCESS)
            {
                fprintf(stderr, "ERROR: Image Conversion Failure: %s\n",
                        Sbiftc_Result_Map(res_sbiftc));
                exit(1);
            }
        }

        /* Now the image is decrypted.
           Let us then verify the image integrity. */
        res_sb = sbverify(header_p, sz, pubKey);
        free(pubKey);
        if (res_sb == SB_SUCCESS)
        {
            /* Good, success */
            res_sbiftc = SBIFTOOLS_Image_Convert(header_p,
                                                 SBIF_IMAGE_BLTw,
                                                 SBIF_VERSION,
                                                 header_p,
                                                 &hdrSize,
                                                 &vec,
                                                 &vec,
                                                 1,
                                                 NULL,
                                                 0,
                                                 workspace_p);
            if (res_sbiftc != SBIFTC_SUCCESS)
            {
                fprintf(stderr, "ERROR: Image Conversion Failure: %s\n",
                        Sbiftc_Result_Map(res_sbiftc));
                exit(1);
            }

            (void)write_binary(argv[3], ptr, sz);
            free(ptr);

            /* Uninitialize Workspace */
            SBIFTOOLS_Image_Convert_UninitWorkspace(workspace_p);
        }
        else
        {
            /* Uninitialize Workspace */
            SBIFTOOLS_Image_Convert_UninitWorkspace(workspace_p);

            fprintf(stderr, "ERROR: Image integrity check failed.\n");
            exit(1);
        }
    }
    else
    {
        fprintf(stderr, "ERROR: Reading %s failed\n", argv[2]);
        exit(1);
    }
    return 0;
}

#ifndef SBLIB_CF_REMOVE_POLLING_CALLS
/* Stub implementation of SB_Poll and SB_ReadData */
bool
SB_Poll(void * UnusedParam_p)
{
    IDENTIFIER_NOT_USED(UnusedParam_p);
    return true;
}

uint32_t
SB_ReadData(void *     PollParam_p,
            uint32_t * Data_p,
            uint32_t   DataLen)
{
    PARAMETER_NOT_USED(PollParam_p);
    PARAMETER_NOT_USED(Data_p);
    return DataLen;
}
#endif /* !SBLIB_CF_REMOVE_POLLING_CALLS */


/* end of file updatetool.c */
