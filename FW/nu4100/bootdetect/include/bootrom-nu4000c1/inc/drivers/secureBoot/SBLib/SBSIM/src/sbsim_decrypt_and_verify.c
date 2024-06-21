/* sbsim_decrypt_and_verify.c
 *
 * Description: Secure Boot Simulator verification tool.
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

#ifndef __MODULE__
#define __MODULE__ "sbsim_decrypt_and_verify.c"
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif /* _POSIX_C_SOURCE */

#include "sb_ecdsa.h"               // verify [and decrypt] API
#include "sbif_ecdsa.h"             // header fields and consts
#include "sbif_attributes.h"        // header attribute processing API

#include "implementation_defs.h"
#include "c_lib.h"

#ifdef SBSIM_ALLOCATOR_DMARESOURCE
#include "dmares_mgmt.h"    // Init
#include "dmares_buf.h"     // Alloc
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef SBSIM_TIME_INFO
#include <sys/time.h>

#define GetTimeSample(a) gettimeofday(a, NULL);

static unsigned long
GetTimeDiff(const struct timeval * T1)
{
    struct timeval T2;
    unsigned long Diff;
    long D;

    gettimeofday(&T2, NULL);

    // Calculate the usec difference
    D = T2.tv_usec - T1->tv_usec;
    if (D < 0)
    {
        D += 1000000;
    }
    Diff = (unsigned long)D;

    // Calculate the sec difference
    D = T2.tv_sec - T1->tv_sec;
    if (D < 0)
    {
        D += -1;
    }
    Diff += ((unsigned long)D) * 1000000;

    return Diff;
}
#endif

#ifndef SBSIM_MAX_IMAGE_SIZE
// support 16MB images
#define SBSIM_MAX_IMAGE_SIZE (16 * 1024 * 1024 + 1024)
#endif

/* Configuration values loaded from command line options. */
struct
{
#ifndef __CC_ARM
    struct timespec polldelay;
    bool fPolldelay;
#endif
    int readblock;
} sbsim_configuration;

static inline uint32_t
Load_BE32(const void * const Value_p)
{
    const uint8_t * const p = (const uint8_t *)Value_p;

    return ((p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3]);
}


static void
Print_OctetString(const char * const Description_p,
                  const void * const Data_p,
                  const unsigned int DataLen)
{
    const uint8_t * Byte_p = Data_p;
    int Column = 16;
    unsigned int PrintedLen;

    printf("%s:", Description_p);

    for (PrintedLen = 0; PrintedLen < DataLen; PrintedLen++)
    {
        if (Column++ == 16)
        {
            printf("\n\t");
            Column = 1;
        }

        printf("%.2x ", (int)*Byte_p);
        Byte_p++;
    }

    printf("\n");
}


static void
Print_EcdsaPublicKey(const char * const Description_p,
                     const SBIF_ECDSA_PublicKey_t * const PublicKey_p)
{
    printf("%s:\n", Description_p);
    Print_OctetString("Qx", PublicKey_p->Qx, sizeof(PublicKey_p->Qx));
    Print_OctetString("Qy", PublicKey_p->Qy, sizeof(PublicKey_p->Qy));
}

static void
Print_EcdsaSignature(const char * const Description_p,
                     const SBIF_ECDSA_Signature_t * const Signature_p)
{
    printf("%s:\n", Description_p);
    Print_OctetString("r", &Signature_p->r, sizeof(Signature_p->r));
    Print_OctetString("s", &Signature_p->s, sizeof(Signature_p->s));
}


static void
Print_ImageInfo(const SBIF_ECDSA_Header_t * const Header_p)
{
    uint32_t TypeWord;
    uint32_t Type;
    uint32_t PubKeyType;
    uint8_t TypeChar[3];
    uint32_t header_image_len;

    TypeWord = Load_BE32(&Header_p->Type);
    TypeChar[2] = (TypeWord >> 8) & 0xff;
    TypeChar[1] = (TypeWord >> 16) & 0xff;
    TypeChar[0] = (TypeWord >> 24) & 0xff;

    printf("Image header:\n");
    if ((TypeChar[0] >= 'A') && (TypeChar[0] <= 'Z') &&
        (TypeChar[1] >= 'A') && (TypeChar[1] <= 'Z') &&
        (TypeChar[2] >= 'a') && (TypeChar[2] <= 'z'))
    {
        printf("Type:\t%c%c%c\n", TypeChar[0], TypeChar[1], TypeChar[2]);
    }
    else
    {
        printf("Type:\tUnknown (corrupt header)\n");
    }
    printf("Version: %d\n", (int)SBIF_TYPE_VERSION(TypeWord));

    PubKeyType = Load_BE32(&Header_p->PubKeyType) & 0xffffff;
    if (PubKeyType == SBIF_PUBKEY_TYPE_ROM)
    {
        printf("Public key in ROM\n");
    }
    else if (PubKeyType == SBIF_PUBKEY_TYPE_OTP)
    {
        printf("Public key in OTP\n");
    }
    else if (PubKeyType == SBIF_PUBKEY_TYPE_IMAGE)
    {
        printf("Public key in image header\n");
    }
    else
    {
        printf("Unknow public key type\n");
    }

    Print_OctetString("r", Header_p->Signature.r, sizeof(Header_p->Signature.r));
    Print_OctetString("s", Header_p->Signature.s, sizeof(Header_p->Signature.r));
    Print_OctetString("key", Header_p->EncryptionKey, sizeof(Header_p->EncryptionKey));
    Print_OctetString("iv", Header_p->EncryptionIV, sizeof(Header_p->EncryptionIV));

    header_image_len = Load_BE32(&Header_p->ImageLen);
    printf("len:\t%u\n", (unsigned int)header_image_len);

    Type = SBIF_TYPE_TYPE(TypeWord);
    if ((Type == SBIF_IMAGE_BLTp) ||
        (Type == SBIF_IMAGE_BLTw) ||
        (Type == SBIF_IMAGE_BLTe))
    {
        SBIF_ECDSA_Certificate_t * Certs_p;
        uint32_t CertCount;
        uint32_t CertIndex;

        Certs_p = (void *)(Header_p + 1);

        CertCount = Load_BE32(&Header_p->CertificateCount);
        printf("cert count: %u\n", (unsigned int)CertCount);

        for (CertIndex = 0; CertIndex < CertCount; CertIndex++)
        {
            if (CertIndex == 4 && CertIndex < CertCount)
            {
                printf("%u more certificates\n", (unsigned int)(CertCount - CertIndex + 1));
                break;
            }

            printf("cert %u\n", (unsigned int)CertIndex);
            Print_EcdsaPublicKey("public key", &Certs_p[CertIndex].PublicKey);
            Print_EcdsaSignature("signature", &Certs_p[CertIndex].Signature);
        }
    }

    if (SBIF_Attribute_Check(&Header_p->ImageAttributes))
    {
        uint32_t Value32 = 0;

        printf("Image attributes are valid\n");

        // check for the rollback attribute
        if (SBIF_Attribute_Fetch(&Header_p->ImageAttributes,
                                 SBIF_ATTRIBUTE_ROLLBACK_ID,
                                 &Value32))
        {
            printf("  RollbackID: 0x%08x\n", (unsigned int)Value32);
        }
        else
        {
            printf("  RollbackID: [not found]\n");
        }
    }
    else
    {
        printf("Image attributes check failed\n");
    }
}


static SB_StorageArea_t StorageArea;

static unsigned int PollCount = 0;


int main(int argc, char * argv[])
{
    SBIF_ECDSA_PublicKey_t PublicKey;
    SBIF_ECDSA_Header_t * Header_p = NULL;
    unsigned int ImageSize;             // as read from file
    SB_Result_t Result;
    uint8_t * AllocatedInputBuffer_p = NULL;
    uint8_t * AllocatedOutputBuffer_p = NULL;
#ifdef SBSIM_ALLOCATOR_DMARESOURCE
    DMAResource_Handle_t DMAHandleInput = NULL;
    DMAResource_Handle_t DMAHandleOutput = NULL;
#endif
    uint8_t * BufferInput_p = NULL;
    uint8_t * BufferOutput_p = NULL;
    uint8_t * VerifiedImage_p = NULL;
    unsigned int VerifiedImageLen = 0;
    bool fInPlace = true;
    const char * DecKeyFile_p = NULL;
    const char * WrapKeyFile_p = NULL;
    const uint8_t * Hash_p = NULL;
    uint8_t HashBuf[SBIF_ECDSA_BYTES];
    uint8_t ConfidentialityKey[SBIF_CFG_CONFIDENTIALITY_BITS / 8];
    SB_ConfidentialityKeyType_t
        ConfidentialityKeyType = SB_CONFIDENTIALITY_KEY_TYPE_AUTO;
#ifdef SBSIM_TIME_INFO
    struct timeval T1;
    char * Tinfo_p = "";
#endif

    /* Macro: Remove handled command line option from argc/argv. */
#define OPTION_HANDLED \
    do { argv[1] = argv[0]; argc--; argv++; } while (0)

    /* Handle options until no option found */
    while (1)
    {
        if ((argc > 1) && (c_strcmp(argv[1], "--") == 0))
        {
            OPTION_HANDLED;
            break;                      /* No more options. */
        }
#ifndef __CC_ARM
        else if ((argc > 1) &&
                 (c_strncmp(argv[1], "--polldelay=", 12) == 0))
        {
            /* Store parsed option and remove it from the command line. */
            double polldelay = atof(argv[1] + 12);
            if (polldelay < 0.0)
            {
                polldelay = 0.0;
            }
            sbsim_configuration.polldelay.tv_sec  = (time_t)polldelay;
            sbsim_configuration.polldelay.tv_nsec = (time_t)((polldelay - (time_t)polldelay) * 1000000000U);
            sbsim_configuration.fPolldelay = true;
            OPTION_HANDLED;
        }
#endif
        else if ((argc > 1) &&
                 (c_strncmp(argv[1], "--readblock=", 12) == 0))
        {
            int readblock = atoi(argv[1] + 12);
            if (readblock > SBSIM_MAX_IMAGE_SIZE)
            {
                readblock = 0;
            }
            else if (readblock > 0)
            {
                /* Make readblock multiple of nearest blocksize. */
                readblock += SB_READ_DATA_BLOCKSIZE / 2;
                readblock /= SB_READ_DATA_BLOCKSIZE;
                if (readblock == 0)
                {
                    readblock = 1;
                }
                readblock *= SB_READ_DATA_BLOCKSIZE;
            }
            sbsim_configuration.readblock = readblock;
            OPTION_HANDLED;
        }
        else if ((argc > 1) &&
                 ((c_strcmp(argv[1], "-1") == 0) ||
                  (c_strcmp(argv[1], "1") == 0)))
        {
            fInPlace = true;
            OPTION_HANDLED;
        }
        else if (argc > 1 &&
                 (c_strcmp(argv[1], "-2") == 0 ||
                  c_strcmp(argv[1], "2") == 0))
        {
            fInPlace = false;
            OPTION_HANDLED;
        }
        else if ((argc > 1) &&
                 (c_strncmp(argv[1], "--deckeyfile=", 13) == 0))
        {
            DecKeyFile_p = argv[1] + 13;
            OPTION_HANDLED;
        }
        else if ((argc > 1) &&
                 (c_strncmp(argv[1], "--unwrapkeyfile=", 16) == 0))
        {
            WrapKeyFile_p = argv[1] + 16;
            OPTION_HANDLED;
        }
        else
        {
            break;                      /* No match: options handled. */
        }
    }

    if (WrapKeyFile_p != NULL && DecKeyFile_p != NULL)
    {
        fprintf(stderr,
                "SBSIM ERROR: Options --deckeyfile and --unwrapkeyfile are mutually exclusive.\n");
        exit(1);
    }

    if ((argc < 3) || (argc > 5))
    {
        fprintf(stderr,
                "usage:\n"
                "\t%s <options> <public key file> <image file> [<output file> [<fragcount>]]\n\n"
                "The available options are:\n"
                "    -1 (dash optional)        use inplace processing (default)\n"
                "    -2 (dash optional)        use two buffers\n"
                "    --polldelay=seconds       wait specified delay during polling HW crypto operations\n"
                "    --readblock=bytes         set the size of block to process at once\n"
                "    --deckeyfile=file.bin     Pass binary file containing BLx key\n"
                "    --unwrapkeyfile=file.bin  Pass binary file containing BLw key\n",
                argv[0]);
        exit(1);
    }

    // load the unwrapping key
    if (WrapKeyFile_p)
    {
        FILE * WrapKeyFile;
        int Length;

        WrapKeyFile = fopen(WrapKeyFile_p, "rb");
        if (!WrapKeyFile)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not open unwrapping key file %s\n",
                    WrapKeyFile_p);
            exit(1);
        }

        Length = fread(&ConfidentialityKey,
                       1,
                       (SBIF_CFG_CONFIDENTIALITY_BITS / 8),
                       WrapKeyFile);

        ConfidentialityKeyType = SB_CONFIDENTIALITY_KEY_TYPE_BLW;

        if (Length != (SBIF_CFG_CONFIDENTIALITY_BITS / 8))
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not read unwrapping key file %s\n",
                    WrapKeyFile_p);
            exit(1);
        }

        fclose(WrapKeyFile);
    }

    // load the decryption key
    if (DecKeyFile_p)
    {
        FILE * DecKeyFile;
        int Length;

        DecKeyFile = fopen(DecKeyFile_p, "rb");
        if (!DecKeyFile)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not open decryption key file %s\n",
                    DecKeyFile_p);
            exit(1);
        }

        Length = fread(&ConfidentialityKey,
                       1,
                       (SBIF_CFG_CONFIDENTIALITY_BITS / 8),
                       DecKeyFile);

        ConfidentialityKeyType = SB_CONFIDENTIALITY_KEY_TYPE_BLX;

        if (Length != (SBIF_CFG_CONFIDENTIALITY_BITS / 8))
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not read decryption key file %s\n",
                    DecKeyFile_p);
            exit(1);
        }

        fclose(DecKeyFile);
    }

    // load the image
    {
        FILE * ImageFile;

        ImageFile = fopen(argv[2], "rb");
        if (!ImageFile)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not open image file %s\n",
                    argv[2]);
            exit(1);
        }

        // allocate the input buffer
        AllocatedInputBuffer_p = malloc(SBSIM_MAX_IMAGE_SIZE);
        if (!AllocatedInputBuffer_p)
        {
            fprintf(stderr, "SBSIM ERROR: Memory allocation (input)\n");
            exit(2);
        }

        // in this example we always read the image into the fixed-size
        // static buffer and later try to allocate an exact-size buffer
        ImageSize = fread(AllocatedInputBuffer_p,
                          1, SBSIM_MAX_IMAGE_SIZE,
                          ImageFile);

        if (ferror(ImageFile))
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not read image file %s\n",
                    argv[2]);
            exit(1);
        }

        if (ImageSize < sizeof(SBIF_ECDSA_Header_t))
        {
            fprintf(stderr,
                    "SBSIM ERROR: Header bigger than image file %s\n",
                    argv[2]);
            exit(1);
        }

        if (ImageSize == SBSIM_MAX_IMAGE_SIZE)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Image file %s exceeds max size\n",
                    argv[2]);
            exit(1);
        }

        fclose(ImageFile);
    }

#ifndef SBSIM_ALLOCATOR_DMARESOURCE
    if (fInPlace)
    {
        BufferOutput_p = AllocatedInputBuffer_p;
    }
    else
    {
        // allocate the output buffer
        AllocatedOutputBuffer_p = malloc(ImageSize);
        if (!AllocatedOutputBuffer_p)
        {
            fprintf(stderr, "SBSIM ERROR: Memory allocation (output)\n");
            exit(2);
        }
        BufferOutput_p = AllocatedOutputBuffer_p;
    }

    BufferInput_p = AllocatedInputBuffer_p;
#else
    if (!DMAResource_Init())
    {
        fprintf(stderr, "SBSIM ERROR: DMAResource initialization failed\n");
        exit(1);
    }

    // Allocate the DMA buffers
    {
        DMAResource_Properties_t Props = { 0, 0, 0, 0 };
        DMAResource_AddrPair_t AddrPair;
        int res;

        Props.Size = ImageSize;
        Props.Alignment = 4;

        res = DMAResource_Alloc(Props, &AddrPair, &DMAHandleInput);
        if (res != 0)
        {
            fprintf(stderr,
                    "SBSIM ERROR: DMAResource allocation failed (input-%d)\n",
                    res);
            exit(2);
        }

        // copy the image into this input buffer
        BufferInput_p = AddrPair.Address.Native_p;
        c_memcpy(BufferInput_p, AllocatedInputBuffer_p, ImageSize);

        free(AllocatedInputBuffer_p);
        AllocatedInputBuffer_p = NULL;

        if (fInPlace)
        {
            BufferOutput_p = BufferInput_p;
        }
        else
        {
            res = DMAResource_Alloc(Props, &AddrPair, &DMAHandleOutput);
            if (res != 0)
            {
                fprintf(stderr,
                        "SBSIM ERROR: DMAResource allocation failed (output-%d)\n",
                        res);
                exit(2);
            }

            BufferOutput_p = AddrPair.Address.Native_p;
        }
    }
#endif /* SBSIM_ALLOCATOR_DMARESOURCE */

    Header_p = (SBIF_ECDSA_Header_t *)BufferInput_p;
    Print_ImageInfo(Header_p);

    // load the public key
    if ((Load_BE32(&Header_p->PubKeyType) & 0xffffff) == SBIF_PUBKEY_TYPE_IMAGE)
    {
        FILE * HashFile;
        int Length;

        /* Take public key from header */
        c_memcpy(&PublicKey, &Header_p->PublicKey, sizeof(PublicKey));

        HashFile = fopen(argv[1], "rb");
        if (HashFile == NULL)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not open hash file %s\n",
                    argv[1]);
            exit(1);
        }

        Length = fread(&HashBuf, 1, sizeof(HashBuf), HashFile);
#if SBIF_ECDSA_BYTES == 66
        HashBuf[64] = 0;
        HashBuf[65] = 0;
        Length += 2;
#endif
        if (Length != sizeof(HashBuf))
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not read hash file %s\n",
                    argv[1]);
            exit(1);
        }

        fclose(HashFile);

        Hash_p = HashBuf;
    }
    else
    {
        FILE * PublicKeyFile;
        int Length;

        PublicKeyFile = fopen(argv[1], "rb");
        if (PublicKeyFile == NULL)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not open public key file %s\n",
                    argv[1]);
            exit(1);
        }

        Length = fread(&PublicKey, 1, sizeof(PublicKey), PublicKeyFile);
        if (Length != sizeof(PublicKey))
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not read public key file %s\n",
                    argv[1]);
            exit(1);
        }

        fclose(PublicKeyFile);
    }
    Print_EcdsaPublicKey("Public key input", &PublicKey);

    // do the secure boot [decrypt+]verify
    {
        SBIF_SGVector_t DataVectorsInput[1]; // 1 contiguous block only
        SBIF_SGVector_t DataVectorsOutput[1];
        const unsigned int HeaderLen = SBIF_ECDSA_GET_HEADER_SIZE(Header_p, ImageSize);
        const unsigned int DataLen = ImageSize - HeaderLen;

        // Check for overflows in image size calculation
        if (HeaderLen == 0 ||
            HeaderLen > ImageSize ||
            DataLen > ImageSize ||
            DataLen + HeaderLen > ImageSize)
        {
            fprintf(stderr, "SBSIM ERROR: Invalid image header\n");
            exit(1);
        }

        DataVectorsInput[0].Data_p = (void *)(BufferInput_p + HeaderLen);
        DataVectorsInput[0].DataLen = DataLen;

        if (fInPlace)
        {
            DataVectorsOutput[0].Data_p = DataVectorsInput[0].Data_p;
        }
        else
        {
            DataVectorsOutput[0].Data_p = (void *)BufferOutput_p;
        }
        DataVectorsOutput[0].DataLen = DataLen;

        VerifiedImageLen = DataLen;
        VerifiedImage_p = (uint8_t *)DataVectorsOutput[0].Data_p;

        c_memset(&StorageArea, 0x55, sizeof(StorageArea));

#ifdef SBSIM_TIME_INFO
        GetTimeSample(&T1);
#endif

        if (ConfidentialityKeyType == SB_CONFIDENTIALITY_KEY_TYPE_AUTO)
        {
            Result = SB_ECDSA_Image_DecryptAndVerify(&StorageArea,
                                                     &PollCount,
                                                     &PublicKey,
                                                     Hash_p,
                                                     Header_p,
                                                     DataVectorsInput,
                                                     DataVectorsOutput,
                                                     /*VectorCount:*/1);
        }
        else
        {
            Result = SB_ECDSA_Image_DecryptWithKeyAndVerify(&StorageArea,
                                                            &PollCount,
                                                            &PublicKey,
                                                            Hash_p,
                                                            Header_p,
                                                            DataVectorsInput,
                                                            DataVectorsOutput,
                                                            /*VectorCount:*/1,
                                                            ConfidentialityKeyType,
                                                            ConfidentialityKey);

#ifdef SBSIM_TIME_INFO
            Tinfo_p = "with Key ";
#endif
        }

#ifdef SBSIM_TIME_INFO
        printf("SB image 'Decrypt %sand Verify' took %u us\n", Tinfo_p, (unsigned int)GetTimeDiff(&T1));
#endif
    }

    printf("SB_Poll() was called %u times.\n", PollCount);

    if (Result == SB_SUCCESS)
    {
        printf("Result: PASS\n");
    }
    else
    {
        printf("Result: FAIL\n");
    }

    if ((argc >= 4) && (Result == SB_SUCCESS))
    {
        FILE * Outf_p = fopen(argv[3], "wb");
        if (Outf_p)
        {
            printf("Writing verified image to %s... ", argv[3]);
            fflush(stdout);
            if (0 == fwrite(VerifiedImage_p, VerifiedImageLen, 1, Outf_p))
            {
                printf("Failed\n");
            }
            else
            {
                printf("SUCCESS\n");
            }

            fclose(Outf_p);
        }
        else
        {
            fprintf(stderr,
                    "SBSIM ERROR: Could not open output file %s\n",
                    argv[3]);
            exit(1);
        }
    }

    if (AllocatedInputBuffer_p)
    {
        free(AllocatedInputBuffer_p);
    }

    if (!fInPlace)
    {
        free(AllocatedOutputBuffer_p);
    }

#ifdef SBSIM_ALLOCATOR_DMARESOURCE
    DMAResource_Release(DMAHandleInput);

    if (!fInPlace)
    {
        DMAResource_Release(DMAHandleOutput);
    }

    DMAResource_UnInit();
#endif

    return Result;
}

/* Simple implementation of SB_Poll. */
bool
SB_Poll(void * PollParam_p)
{
    unsigned int * const PollCount_p = PollParam_p;

    *PollCount_p = *PollCount_p + 1;

#ifndef __CC_ARM
    /* If poll delay has been configured, cause a small delay. */
    if (sbsim_configuration.fPolldelay)
    {
        nanosleep(&sbsim_configuration.polldelay, NULL);
    }
#endif

    return true;
}

/* Simple implementation of SB_ReadData. */
uint32_t
SB_ReadData(void * PollParam_p,
            uint32_t * Data_p,
            uint32_t DataLen)
{
    /* Check Poll parameter has been provided (ignore by this function). */
    if (PollParam_p == NULL)
    {
        return 0;
    }

    /* Check Data_p is not NULL address (API misuse.) */
    if (Data_p == NULL)
    {
        return 0;
    }

    /* If read block size has been set and the
       DataLen is more than read block size, then return read block size. */
    if ((sbsim_configuration.readblock != 0) &&
        (sbsim_configuration.readblock < (int)DataLen))
    {
        return sbsim_configuration.readblock;
    }

    /* Else: All data requested available for reading. */
    return DataLen;
}

/* end of file sbsim_decrypt_and_verify.c */
