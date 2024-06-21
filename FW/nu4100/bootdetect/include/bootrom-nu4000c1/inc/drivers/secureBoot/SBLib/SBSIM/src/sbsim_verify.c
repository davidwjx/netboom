/* sbsim_verify.c
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
#define __MODULE__ "sbsim_verify.c"
#endif

#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 199309L
#endif /* _POSIX_C_SOURCE */

#include "sb_ecdsa.h"               // verify [and decrypt] API
#include "sbif_ecdsa.h"             // header fields and consts
#include "sbif_attributes.h"        // header attribute processing API

#include "implementation_defs.h"
#include "c_lib.h"                  // c_memcpy, c_strncmp

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

#define SBSIM_MAX_SG 64
static SBIF_SGVector_t SG_In[1 + SBSIM_MAX_SG];
//static SBIF_SGVector_t SG_Out[1 + SBSIM_MAX_SG];

#ifdef SBSIM_ALLOCATOR_DMARESOURCE
static DMAResource_Handle_t DMAHandleInput[1 + SBSIM_MAX_SG];
#endif

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
    unsigned int HeaderLen;
    unsigned int DataLen;
    SB_Result_t Result;
    uint8_t * AllocatedInputBuffer_p = NULL;
    const uint8_t * Hash_p = NULL;
    uint8_t HashBuf[SBIF_ECDSA_BYTES];
    unsigned int FragCount = 1;
    unsigned int FragSize = 0;
    unsigned int FragSizeLast = 0;
#ifdef SBSIM_TIME_INFO
    struct timeval T1;
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
            OPTION_HANDLED;
        }
        else if ((argc > 1) &&
                 ((c_strcmp(argv[1], "-2") == 0) ||
                  (c_strcmp(argv[1], "2") == 0)))
        {
            fprintf(stderr, "SBSIM ERROR: Two buffers mode not supported.\n");
            exit(1);
        }
        else if ((argc > 1) &&
                 (c_strncmp(argv[1], "--deckeyfile=", 13) == 0))
        {
            fprintf(stderr, "SBSIM ERROR: Decryption key not supported.\n");
            exit(1);
        }
        else if ((argc > 1) &&
                 (c_strncmp(argv[1], "--unwrapkeyfile=", 16) == 0))
        {
            fprintf(stderr, "SBSIM ERROR: Unwrapping key not supported.\n");
            exit(1);
        }
        else
        {
            break;                      /* No match: options handled. */
        }
    }

    if ((argc < 3) || (argc > 5))
    {
        fprintf(stderr,
                "usage:\n"
                "\t%s <options> <public key file> <image file> [<output file> [<fragcount>]]\n\n"
                "The available options are:\n"
                "    --polldelay=seconds  wait specified delay during polling HW crypto operations\n"
                "    --readblock=bytes    set the size of block to process at once\n",
                argv[0]);
        exit(1);
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
            fprintf(stderr, "SBSIM ERROR: Memory allocation\n");
            exit(2);
        }

        // in this example we always read the image into the fixed-size
        // static buffer and later try to allocate an exact-size buffer
        ImageSize = fread(AllocatedInputBuffer_p,
                          1, SBSIM_MAX_IMAGE_SIZE,
                          ImageFile);

        if (ferror(ImageFile))
        {
            fprintf(stderr, "SBSIM ERROR: Reading image file %s\n", argv[2]);
            exit(1);
        }

        if (ImageSize < sizeof(SBIF_ECDSA_Header_t))
        {
            fprintf(stderr, "SBSIM ERROR: Header bigger then image file %s\n",
                    argv[2]);
            exit(1);
        }

        if (ImageSize == SBSIM_MAX_IMAGE_SIZE)
        {
            fprintf(stderr, "SBSIM ERROR: Image file %s exceeds max size\n",
                    argv[2]);
            exit(1);
        }

        fclose(ImageFile);
    }

    HeaderLen = SBIF_ECDSA_GET_HEADER_SIZE(AllocatedInputBuffer_p, ImageSize);
    DataLen = ImageSize - HeaderLen;

    // Check for overflows in image size calculation
    if ((HeaderLen == 0) ||
        (HeaderLen > ImageSize) ||
        (DataLen > ImageSize) ||
        ((DataLen + HeaderLen) > ImageSize))
    {
        fprintf(stderr, "SBSIM ERROR: Invalid image header\n");
        exit(1);
    }

    if (argc == 5)
    {
        int temp = atoi(argv[4]);
        if (temp < 1 || temp > SBSIM_MAX_SG)
        {
            fprintf(stderr,
                    "SBSIM ERROR: Invalid FragCount: %d (min=1; max=%u)\n",
                    temp, SBSIM_MAX_SG);
            exit(1);
        }

        FragCount = 0 + temp;
    }

    if (FragCount == 1)
    {
        FragSize = 0;
        FragSizeLast = DataLen;
    }
    else
    {
        // divide up the image into FragCount pieces
        FragSize = DataLen / FragCount;

#if ((SBIF_ECDSA_BYTES == 48) || (SBIF_ECDSA_BYTES == 66))
        // round up to nearest 128-byte aligned
        if (FragSize & 127)
        {
            FragSize = FragSize + 128 - (FragSize & 127);
        }
#else
        // round up to nearest 64-byte aligned
        if (FragSize & 63)
        {
            FragSize = FragSize + 64 - (FragSize & 63);
        }
#endif

        FragCount = DataLen / FragSize;
        if (FragSize * FragCount < DataLen)
        {
            FragCount++;                // partial last fragment
            FragSizeLast = DataLen - (FragCount - 1) * FragSize;
        }
        else
        {
            FragSizeLast = FragSize;
        }
    }

    printf("SBSIM: ImageSize=%u; HeaderLen=%u; DataLen=%u; FragSize=%u; FragSizeLast=%u; FragCount=%u; %u+%u*%u+%u=%u\n",
           ImageSize, HeaderLen, DataLen,
           FragSize, FragSizeLast, FragCount,
           HeaderLen, FragCount - 1, FragSize, FragSizeLast,
           HeaderLen + (FragCount - 1) * FragSize + FragSizeLast);

#ifndef SBSIM_ALLOCATOR_DMARESOURCE
    {
        uint8_t * Buf_p = AllocatedInputBuffer_p;
        unsigned int lp;

        SG_In[0].Data_p = (uint32_t *)Buf_p;
        SG_In[0].DataLen = HeaderLen;
        Buf_p += HeaderLen;

        for (lp = 1; lp < FragCount; lp++)
        {
            SG_In[lp].Data_p = (uint32_t *)Buf_p;
            SG_In[lp].DataLen = FragSize;
            Buf_p += FragSize;
        }

        SG_In[FragCount].Data_p = (uint32_t *)Buf_p;
        SG_In[FragCount].DataLen = DataLen - (FragCount - 1) * FragSize;
    }

#else
    if (!DMAResource_Init())
    {
        fprintf(stderr, "SBSIM ERROR: DMAResource initialization failed\n");
        exit(1);
    }

    // Allocate the DMA buffers
    {
        uint8_t * Copy_p = AllocatedInputBuffer_p;
        DMAResource_Properties_t Props = { 0, 0, 0, 0 };
        DMAResource_AddrPair_t AddrPair;
        unsigned int lp;
        int res;

        Props.Alignment = 4;

        // first fragment is for the entire header + certificates
        {
            Props.Size = HeaderLen;

            res = DMAResource_Alloc(Props, &AddrPair, DMAHandleInput + 0);
            if (res != 0)
            {
                fprintf(stderr,
                        "SBSIM ERROR: DMAResource allocation failed (%d)\n",
                        res);
                exit(2);
            }

            c_memcpy(AddrPair.Address.Native_p, Copy_p, Props.Size);
            Copy_p += Props.Size;

            SG_In[0].Data_p = (uint32_t *)AddrPair.Address.Native_p;
            SG_In[0].DataLen = Props.Size;
        }

        // allocate the fragments
        Props.Size = FragSize;
        for (lp = 1; lp <= FragCount; lp++)
        {
            if (lp == FragCount)
            {
                Props.Size = FragSizeLast;
            }

            res = DMAResource_Alloc(Props, &AddrPair, DMAHandleInput + lp);
            if (res != 0)
            {
                fprintf(stderr,
                        "SBSIM ERROR: DMAResource allocation failed (%u, %d)\n",
                        lp, res);
                exit(2);
            }

            // copy part of the image into this input buffer
            c_memcpy(AddrPair.Address.Native_p, Copy_p, Props.Size);
            Copy_p += Props.Size;

            SG_In[lp].Data_p = (uint32_t *)AddrPair.Address.Native_p;
            SG_In[lp].DataLen = Props.Size;
        }

        // we no longer need the buffer the file was read into
        free(AllocatedInputBuffer_p);
        AllocatedInputBuffer_p = NULL;
    }
#endif /* SBSIM_ALLOCATOR_DMARESOURCE */

    Header_p = (SBIF_ECDSA_Header_t *)SG_In[0].Data_p;
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
    c_memset(&StorageArea, 0x55, sizeof(StorageArea));

#ifdef SBSIM_TIME_INFO
    GetTimeSample(&T1);
#endif

    Result = SB_ECDSA_Image_Verify(&StorageArea,
                                   /*PollParam_p:*/&PollCount,
                                   &PublicKey,
                                   Hash_p,
                                   Header_p,
                                   SG_In + 1,
                                   FragCount);

#ifdef SBSIM_TIME_INFO
    printf("SB image 'Verify' took %u us\n", (unsigned int)GetTimeDiff(&T1));
#endif

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
            unsigned int lp;

            printf("Writing verified image to %s... ", argv[3]);
            fflush(stdout);

            // skip the header part
            for (lp = 1; lp <= FragCount; lp++)
            {
                if (0 == fwrite(SG_In[lp].Data_p,
                                SG_In[lp].DataLen, 1,
                                Outf_p))
                {
                    break;              // from the for
                }
            }

            if (lp > FragCount)
            {
                printf("SUCCESS\n");
            }
            else
            {
                printf("Failed\n");
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

#ifdef SBSIM_ALLOCATOR_DMARESOURCE
    {
        unsigned int lp;
        for (lp = 0; lp <= FragCount; lp++)
        {
            DMAResource_Release(DMAHandleInput[lp]);
        }
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

/* end of file sbsim_verify.c */
