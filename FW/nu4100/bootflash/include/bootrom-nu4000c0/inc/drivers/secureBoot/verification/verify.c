#ifdef __cplusplus
      extern "C" {
#endif
#include <string.h>
#include "nu_regs.h"
#include "verify.h"
#include "debug.h"
#include "system.h"
#include "sbif_attributes.h" //to be removed
#include "apb_timer.h"


/**********************************************
 * extern functions
***********************************************/
extern void invalidate_icache_all(void);
extern void dcache_disable(void);

/**************************************************************
 ***************       G L O B A L         D A T A              ***************
 ***************************************************************/
#define SB_TOTAL_NUM_BYTES (SB_ECDSA_BYTES + SB_CONFIDENTIALITY_BYTES)
SECTION_PBSS SB_EfuseArea_t eFUSEDB;

static inline UINT32
Load_BE32(
    const void * const Value_p)
{
    const UINT8 * const p = (const UINT8 *)Value_p;

    return (p[0] << 24 |
            p[1] << 16 |
            p[2] << 8  |
            p[3]);
}

#ifdef B0_TEST
/* SIMULATE_EFUSE - define used to simulating efuse. instead of actual reading of efuse, read from the global variable below */
//#define SIMULATE_EFUSE
#ifdef SIMULATE_EFUSE
//efuse reading
UINT8 imageDecryption[] = { 0x1D, 0x2D, 0x2D, 0x5B, 0x16, 0x16, 0x20, 0x5A, 0xC3, 0xBF, 0xD5, 0x56, 0xE0, 0x75, 0x41, 0x94};
UINT8 secureBootPublic[]= 
{
   /* the Qx value */
    0x5d, 0xc0, 0x7a, 0x67,
    0x72, 0xd8, 0x8f, 0xdf,
    0x6a, 0xdb, 0xca, 0x75,
    0x82, 0xc3, 0x8a, 0x0f,
    0x3b, 0xc0, 0x2b, 0xa8,
    0x6f, 0x4f, 0x1f, 0xc8,
    0x12, 0xb5, 0xec, 0xbc,
    0x29, 0xd2, 0xae, 0x04,

    /* the Qy value */
    0xa9, 0xb1, 0xe3, 0x74,
    0x03, 0xa8, 0xaf, 0x92,
    0x63, 0x8b, 0x3a, 0xc7,
    0xf3, 0x5b, 0x98, 0x54,
    0xef, 0xb1, 0x50, 0xaf,
    0xab, 0x2c, 0x3c, 0xab,
    0x5a, 0x99, 0x44, 0x70,
    0x87, 0x88, 0x48, 0xca
};
#endif
static void Print_OctetString(const char * const Description_p, const void * const Data_p, const unsigned int DataLen)
{
    const UINT8 * Byte_p = Data_p;
    int Column = 16;
    unsigned int PrintedLen;

    rel_log("%s:", Description_p);

    for (PrintedLen = 0; PrintedLen < DataLen; PrintedLen++)
    {
        if (Column++ == 16)
        {
            rel_log("\n\t");
            Column = 1;
        }

        rel_log("%.2x ", (int) *Byte_p);
        Byte_p++;
    }

    rel_log("\n");
}

static void
Print_EcdsaSignature(
    const char * const Description_p,
    const SBIF_ECDSA_Signature_t * const Signature_p)
{
    rel_log("%s:\n", Description_p);
    Print_OctetString(
        "r", &Signature_p->r,
        sizeof(Signature_p->r));

    Print_OctetString(
        "s",
        &Signature_p->s,
        sizeof(Signature_p->s));
}

static void Print_EcdsaPublicKey(const char * const Description_p, const SBIF_ECDSA_PublicKey_t * const PublicKey_p)
{
    rel_log("%s:\n", Description_p);
    Print_OctetString("Qx", PublicKey_p->Qx, sizeof(PublicKey_p->Qx));
    Print_OctetString("Qy", PublicKey_p->Qy, sizeof(PublicKey_p->Qy));
}


static void Print_ImageInfo(const SBIF_ECDSA_Header_t * const Header_p)
{
    UINT32 TypeWord;
    UINT32 PubKeyType;
    UINT32 Type;
    UINT8 TypeChar[3];
    int header_image_len;

    TypeWord = Load_BE32(&Header_p->Type);
    TypeChar[2] = (TypeWord >> 8) & 0xff;
    TypeChar[1] = (TypeWord >> 16) & 0xff;
    TypeChar[0] = (TypeWord >> 24) & 0xff;

    rel_log("Image header:\n");
    if (TypeChar[0] >= 'A' && TypeChar[0] <= 'Z' &&
            TypeChar[1] >= 'A' && TypeChar[1] <= 'Z' &&
            TypeChar[2] >= 'a' && TypeChar[2] <= 'z')
    {
        rel_log("Type:\t%c%c%c\n", TypeChar[0], TypeChar[1], TypeChar[2]);
    }
    else
    {
        rel_log("Type:\tUnknown (corrupt header)\n");
    }
    rel_log("Version: %d\n", (int) SBIF_TYPE_VERSION(TypeWord));

    PubKeyType = Load_BE32(&Header_p->PubKeyType);
    {
        if (PubKeyType == SBIF_PUBKEY_TYPE_ROM)
        {
            rel_log("Public key in ROM\n");
        }
        else if (PubKeyType == SBIF_PUBKEY_TYPE_OTP)
        {
            rel_log("Public key in OTP\n");
        }
        else if (PubKeyType == SBIF_PUBKEY_TYPE_IMAGE)
        {
            rel_log("Public key in image header\n");
        }
        else
        {
            rel_log("Unknow public key type\n");
        }

    }

    Print_OctetString(
        "r",
        Header_p->Signature.r,
        sizeof(Header_p->Signature.r));

    Print_OctetString(
        "s",
        Header_p->Signature.s,
        sizeof(Header_p->Signature.r));

    Print_OctetString(
        "key",
        Header_p->EncryptionKey,
        sizeof(Header_p->EncryptionKey));

    Print_OctetString(
        "iv",
        Header_p->EncryptionIV,
        sizeof(Header_p->EncryptionIV));

    header_image_len = Load_BE32(&Header_p->ImageLen);
    rel_log("len:\t%u\n", (UINT32) header_image_len);

    Type = SBIF_TYPE_TYPE(TypeWord);

    if (Type == SBIF_IMAGE_BLTp  ||
            Type == SBIF_IMAGE_BLTw  ||
            Type == SBIF_IMAGE_BLTe)
    {
        SBIF_ECDSA_Certificate_t *Certs_p;
        int CertCount;
        int CertIndex;

        Certs_p = (void *) (Header_p + 1);

        CertCount = Load_BE32(&Header_p->CertificateCount);
        rel_log("cert count: %d\n", CertCount);

        for (CertIndex = 0; CertIndex < CertCount; CertIndex++)
        {
            if (CertIndex == 4 && CertIndex < CertCount)
            {
                rel_log("%u more certificates\n", CertCount - CertIndex + 1);
                break;
            }

            rel_log("cert %d\n", CertIndex);
            Print_EcdsaPublicKey(
                "public key",
                &Certs_p[CertIndex].PublicKey);

            Print_EcdsaSignature(
                "signature",
                &Certs_p[CertIndex].Signature);
        }
    }

    if (SBIF_Attribute_Check(&Header_p->ImageAttributes))
    {
        uint32_t Value32 = 0;

        rel_log("Image attributes are valid\n");

        // check for the rollback attribute
        if (SBIF_Attribute_Fetch(
                    &Header_p->ImageAttributes,
                    SBIF_ATTRIBUTE_ROLLBACK_ID,
                    &Value32))
        {
            rel_log("  RollbackID: 0x%08x\n", Value32);
        }
        else
        {
            rel_log("  RollbackID: [not found]\n");
        }
    }
    else
    {
        rel_log("Image attributes check failed\n");
    }
}

   void init_cpu_perf() {
   
     // Disable all counters for configuration (PCMCNTENCLR)
     asm volatile ("MCR p15, 0, %0, c9, c12, 2\t\n" :: "r"(0x8000003f));
   
     // disable counter overflow interrupts
     asm volatile ("MCR p15, 0, %0, c9, c14, 2\n\t" :: "r"(0x8000003f));
   
   
     // Select which events to count in the 6 configurable counters
     // Note that both of these examples come from the list of required events.
//     _setup_cpu_counter(0, 0x04, "L1DACC");
//     _setup_cpu_counter(1, 0x03, "L1DFILL");
   
   }

   inline void reset_cpu_perf() {
   
     // Disable all counters (PMCNTENCLR):
     asm volatile ("MCR p15, 0, %0, c9, c12, 2\t\n" :: "r"(0x8000003f));
   
     UINT32 pmcr  = 0x1    // enable counters
               | 0x2    // reset all other counters
               | 0x4    // reset cycle counter
               | 0x8    // enable "by 64" divider for CCNT.
               | 0x10;  // Export events to external monitoring
   
     // program the performance-counter control-register (PMCR):
     asm volatile ("MCR p15, 0, %0, c9, c12, 0\t\n" :: "r"(pmcr));
   
     // clear overflows (PMOVSR):
     asm volatile ("MCR p15, 0, %0, c9, c12, 3\t\n" :: "r"(0x8000003f));
   
     // Enable all counters (PMCNTENSET):
     asm volatile ("MCR p15, 0, %0, c9, c12, 1\t\n" :: "r"(0x8000003f));
   
   }

   inline UINT32 _read_cpu_counter()
   {
     // Read PMXEVCNTR #r
     // This is done by first writing the counter number to PMSELR and then reading PMXEVCNTR
     UINT32 ret;
   //  asm volatile ("MCR p15, 0, %0, c9, c12, 5\t\n" :: "r"(r));      // Select event counter in PMSELR
     asm volatile ("MRC p15, 0, %0, c9, c13, 0\t\n" : "=r"(ret));    // Read from PMXEVCNTR
     return ret;
   }
#endif

/****************************************************************************
*
*  Function Name: decrypt_and_verify_blw_image
*
*  Description:
*
*  Inputs: Image_p- pointer to a file with signature and encryption,  ImageTarget- A decrypted and without signature file, ImageSize- input file size
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
SB_Result_t decrypt_and_verify_blw_image(const UINT8 *Image_p,UINT8 **ImageTarget,UINT32 *ImageSize,SBIF_ECDSA_PublicKey_t *secureBootPublicKey ,UINT8 *imageDecryptionKey,SB_ConfidentialityKeyType_t ConfidentialityKeyType)
{
   SB_ECDSA_StorageArea_t StorageArea;//a work memroy for the secure boot 

   //For calculating header size (as the header size varies according to the number of certificates present)
   unsigned int HeaderSize;
   unsigned int imageLen;

   // Define variables for SB_ECDSA_Image_DecryptAndVerify
   SBIF_SGVector_t DataVectorIn[1];
   SBIF_SGVector_t DataVectorOut[1];
   SBIF_ECDSA_Header_t *Header_p = (SBIF_ECDSA_Header_t *) Image_p;
#ifdef B0_TEST
//   Print_ImageInfo(Header_p);
#endif
   imageLen       = Load_BE32(&Header_p->ImageLen);

   // Calculate header size
   HeaderSize = SBIF_ECDSA_GET_HEADER_SIZE(Image_p, sizeof(SBIF_ECDSA_Header_t));
   // Check the header size is valid and image is large enough
   if (HeaderSize == 0 || imageLen == 0)
   {
      return SB_ERROR_ARGUMENTS;
   }

   *(ImageSize)   = HeaderSize + imageLen;
   *(ImageTarget) = (UINT8*) (Image_p + *(ImageSize));

   //align to 4 if not aligned
   if (((UINT32)*(ImageTarget)) % 4)
   {
      *(ImageTarget)-= (((UINT32)*(ImageTarget)) % 4);
      *(ImageTarget)+= 4;
   }

   // Fill in data vectors. (Single part data in this example.)
   DataVectorIn[0].Data_p  = (void*) (Image_p + HeaderSize);
   DataVectorIn[0].DataLen = imageLen;
   DataVectorOut[0].Data_p = (void*) *(ImageTarget); //A memory to copy the decrypted file
   DataVectorOut[0].DataLen= imageLen;

   // Actual Image decryption and verification
   return SB_ECDSA_Image_DecryptWithKeyAndVerify(&StorageArea, NULL /* PollParam_p - not used*/, secureBootPublicKey, NULL /* Hash_p - not used*/, Header_p, DataVectorIn, DataVectorOut, 1 /* VectorCount*/, ConfidentialityKeyType, imageDecryptionKey);
}


/****************************************************************************
*
*  Function Name: initSBEfuseDataBase
*
*  Description: reset data base
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
void initSBEfuseDB()
{
   memset(&eFUSEDB, 0, sizeof(eFUSEDB));
}

/****************************************************************************
*
*  Function Name: fillSBEfuseDataBase
*
*  Description: reads efuse and fills data base
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
UINT8 getEfuseSecureControlData()
{
   if ((eFUSEDB.securityControl.integrity_force) && (eFUSEDB.securityControl.encryption_force))
      return SB_CONFIDENTIALITY_KEY_TYPE_BLW;
   else if (eFUSEDB.securityControl.integrity_force)
      return SB_CONFIDENTIALITY_KEY_TYPE_AUTO;//BLp format
   else 
      return NOT_SECURED_IMAGE;
}

/****************************************************************************
*
*  Function Name: readEfuseSecureControl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
void readEfuseSecureControl(SB_EfuseArea_t *eFUSEP)
{
   UINT8 securityControl;
#ifndef SIMULATE_EFUSE
   securityControl=GME_FUSE_SHADOW_5_SECURITY_CONTROL_R;//Register 5, byte 0 
#else
   securityControl=0x60;//Register 5, byte 0  can be also 0x20
#endif
   memcpy((void *)&(eFUSEP->securityControl),(void *)&securityControl,sizeof(UINT8));
}

/****************************************************************************
*
*  Function Name: readEfuseKeys
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
void readEfuseKeys(SB_EfuseArea_t *eFUSEP)
{
#ifndef SIMULATE_EFUSE
   UINT32 i, eFuseArray[SB_TOTAL_NUM_BYTES / 4];
   UINT8 eFuseArrayBytes[SB_TOTAL_NUM_BYTES];
   for(i=0;i<(SB_TOTAL_NUM_BYTES / 4);i++)
      eFuseArray[i]=*(GME_FUSE_SHADOW_6_REG+i);

   memcpy(eFuseArrayBytes, eFuseArray, SB_TOTAL_NUM_BYTES);
   memcpy(&eFUSEP->publicKey,eFuseArrayBytes ,SB_ECDSA_BYTES);//64 bytes
   memcpy(eFUSEP->confidentialityKey,&eFuseArrayBytes[SB_ECDSA_BYTES],SB_CONFIDENTIALITY_BYTES);//16 bytes
#else
   memcpy(&eFUSEP->publicKey,secureBootPublic ,SB_ECDSA_BYTES);
   memcpy(eFUSEP->confidentialityKey,imageDecryption ,SB_CONFIDENTIALITY_BYTES);
#endif
}

/****************************************************************************
*
*  Function Name: fillSBEfuseDataBase
*
*  Description: reads efuse and fills data base
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
void fillSBEfuseDB()
{
   initSBEfuseDB();
   readEfuseSecureControl(&eFUSEDB);
   readEfuseKeys(&eFUSEDB);
}

/****************************************************************************
*
*  Function Name: secureBoot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
void secureBoot(UINT32 boot_address)
{
   SB_Result_t Result                  = SB_SUCCESS;
   UINT8       ConfidentialityKeyType  = getEfuseSecureControlData();
   UINT32      secure_boot_output_address, ImageSize;

#ifdef B0_TEST
   volatile UINT32 before, after,before_timer,after_timer;
   init_cpu_perf();
   reset_cpu_perf();
   before_timer= APB_TIMERG_read();
   before      = _read_cpu_counter();
//   getCPUstatus();
#endif
   //Enable Data cache & MMU to increase secure boot performance
   enable_caches();
   //As soon as the MMU is enabled, these flat-mapped addresses will become invalid as the cache is now looking for virtual addresses as defined in the MMU page table, so it is important to remember to invalidate the I-Cache when enabling the MMU
   invalidate_icache_all();

   Result      = decrypt_and_verify_blw_image((const UINT8*)boot_address, (UINT8 **)&secure_boot_output_address, &ImageSize, &eFUSEDB.publicKey, eFUSEDB.confidentialityKey, (SB_ConfidentialityKeyType_t)ConfidentialityKeyType);

   //disable data cache & MMU
   disable_caches();

#ifdef B0_TEST
   after       = _read_cpu_counter();
   after_timer = APB_TIMERG_read();
#endif

   if (Result==SB_SUCCESS)
   {
#ifdef B0_TEST
      //pll division 12500000==800MHz divide by 64, 375000 for FLASH
      rel_log("SB SUCCESS after=%d, before=%d, delta=%d, time[ms] =~ %d,after_timer=%d,before_timer=%d,delta_timer[µs]=%d\n",after,before,after-before, (after-before)/12500,after_timer,before_timer,(before_timer-after_timer)/24);
#else
      rel_log("SB SUCCESS\n");
#endif
      memcpy((void *)boot_address,(void *)secure_boot_output_address,ImageSize);
   }
   else
   {
      abort_log("error: %x",Result);
   }
}

