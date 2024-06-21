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
#include "spl.h"

//#define SIMULATE_EFUSE
#define CACHES_ON
#define MAX_NUM_SB_CHUNKS 12

#define EFUSE_SIZE_IN_BYTES   (0x200)
#define EFUSE_SIZE            (EFUSE_SIZE_IN_BYTES/sizeof(UINT32))

#define VERIFY_BUSY_NUM_LOOPS         (0xFFFF)
#define VERIFY_MAX_ITERATIONS         (2000)
#define VERIFY_WHILE_LOOP_LIMITED(COND, MAX_ITERATIONS)   {  \
                                                               UINT32   iteration = 0; \
                                                               UINT32   busy_loop = 0; \
                                                               while((COND) && (iteration++ < MAX_ITERATIONS)) \
                                                               { \
                                                                  busy_loop = VERIFY_BUSY_NUM_LOOPS; \
                                                                  while(busy_loop--); \
                                                               }\
                                                               if(iteration >= MAX_ITERATIONS) \
                                                               {\
                                                                  return -1;\
                                                               }\
                                                             };
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

int efuse_connection_init = 0;

#ifdef SIMULATE_EFUSE
//efuse reading

UINT8 imageDecryption2[] =
{
    0xff, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

UINT8 imageDecryption[] =
       {
        0xb8 ,0x2c ,0xa2 ,0x2a ,0x3f ,0xfa ,0x47 ,0xb9 ,
        0xd0 ,0xbc ,0xf6 ,0x9c ,0x6f ,0xec ,0xbb ,0xab
       };

//#define USE_HP_KEY
#ifdef USE_HP_KEY
UINT8 secureBootPublic[]=
{
   /* the Qx value */
   0x7b, 0x69, 0x91, 0x44,
   0x3c, 0x1c, 0x4d, 0xa4,
   0x8d, 0xda, 0x2d, 0x23,
   0xbe, 0xac, 0x8f, 0x27,
   0x01, 0x47, 0x5c, 0x6e,
   0x19, 0x8a, 0x94, 0xde,
   0xc1, 0x3a, 0x38, 0xfe,
   0xa1, 0xc1, 0x77, 0x88,

   /* the Qy value */
   0x5d, 0xc7, 0x83, 0xc7,
   0x8f, 0xea, 0xe5, 0x68,
   0xbb, 0x6e, 0x8f, 0xf7,
   0x3a, 0x29, 0xfd, 0x89,
   0xb1, 0x4b, 0x09, 0xd7,
   0xba, 0xee, 0xdd, 0x6a,
   0x2b, 0xce, 0x59, 0x94,
   0x39, 0x04, 0xd7, 0xec
};
#else //USE_HP_KEY
UINT8 secureBootPublic[]=
{
   /* the Qx value */
   0x41, 0x47, 0xa3, 0xe1,
   0xc7, 0x7f, 0x07, 0xcf,
   0x89, 0x38, 0xe1, 0x97,
   0x15, 0x51, 0xd9, 0xbc,
   0x86, 0xc8, 0x20, 0x58,
   0xfa, 0xf2, 0x4c, 0x5a,
   0x72, 0x9b, 0x5c, 0x3e,
   0xfd, 0xea, 0x15, 0xc5,

   /* the Qy value */
   0x47, 0xdc, 0x90, 0xef,
   0x7b, 0xde, 0x72, 0xc7,
   0xa1, 0xfb, 0xac, 0x5b,
   0xfb, 0x48, 0xf0, 0x7b,
   0x15, 0x4e, 0x4b, 0x50,
   0x06, 0xf6, 0x4a, 0xc9,
   0x44, 0x84, 0x16, 0xf6,
   0x23, 0xbc, 0x8d, 0x34
};
#endif //USE_HP_KEY
#endif //SIMULATE_EFUSE

UINT32 getUnsecuredImgLen(UINT8 *securedImageAddress)
{
   UINT32 ImageLen;

   ImageLen       = Load_BE32(&((SBIF_ECDSA_Header_t *)securedImageAddress)->ImageLen);
   return ImageLen;
}

/****************************************************************************
*
*  Function Name: decrypt_and_verify_blw_image
*
*  Description:
*
*  Inputs: Image_p- pointer to a file with signature and encryption,  ImageTarget- A decrypted and without signature file
*
*  Outputs:
*
*  Returns:
*
*  Context: secure boot Verification
*
****************************************************************************/
SB_Result_t decrypt_and_verify_blw_image(const UINT8 *securedImageAddress,SBIF_ECDSA_PublicKey_t *secureBootPublicKey ,UINT8 *imageDecryptionKey,SB_ConfidentialityKeyType_t ConfidentialityKeyType,unsigned int chunkSize)
{
   SB_ECDSA_StorageArea_t StorageArea;//a work memroy for the secure boot
   SB_Result_t Result                  = SB_SUCCESS;
   SBIF_ECDSA_Header_t ECDSA_Header;
   UINT8 *unSecuredImageAddress=(UINT8 *)(securedImageAddress-chunkSize);

   //For calculating header size (as the header size varies according to the number of certificates present)
   unsigned int HeaderSize,size;
   unsigned int imageLen,ind,numImages=0;
  // Define variables for SB_ECDSA_Image_DecryptAndVerify
   SBIF_SGVector_t DataVectorIn[MAX_NUM_SB_CHUNKS];
   SBIF_SGVector_t DataVectorOut[MAX_NUM_SB_CHUNKS];
   SBIF_ECDSA_Header_t *Header_p = &ECDSA_Header;

     // Calculate header size
   HeaderSize = SBIF_ECDSA_GET_HEADER_SIZE(securedImageAddress, sizeof(SBIF_ECDSA_Header_t));
   memcpy(Header_p,securedImageAddress,HeaderSize);
   imageLen       = Load_BE32(&Header_p->ImageLen);

//   spl_log("+++++++++++++++++++++++%s %d hdr offset %x ImageLen %x %x len %d hdrSize %d \n",__FILE__,__LINE__,Header_p,Header_p->ImageLen,imageLen,imageLen,HeaderSize);
  // Check the header size is valid and image is large enough
   if (HeaderSize == 0 || imageLen == 0)
   {
      return SB_ERROR_ARGUMENTS;
   }


   for (ind=0;ind<imageLen;ind+=chunkSize)
   {

      if (numImages >= MAX_NUM_SB_CHUNKS)
      {
         spl_log("num SB Images  %d reached maximum\n", numImages);
         return SB_ERROR_ARGUMENTS;
      }
      if ((imageLen - ind)>chunkSize)
         size = chunkSize;
      else size = imageLen% chunkSize;

      // Fill in data vectors. (Single part data in this example.)
      DataVectorIn[numImages].Data_p  = (void*) (securedImageAddress + HeaderSize + ind);
      DataVectorIn[numImages].DataLen = size;
      DataVectorOut[numImages].Data_p = (uint32_t *)(unSecuredImageAddress + ind); //A memory to copy the decrypted file
      DataVectorOut[numImages].DataLen= size;
//      spl_log("Vectors In %x size %x out %x size %x \n", DataVectorIn[numImages].Data_p,DataVectorIn[numImages].DataLen,
//            DataVectorOut[numImages].Data_p,DataVectorOut[numImages].DataLen);
      numImages++;
   }
//   spl_log("numImages  %d \n", numImages);
   Result = SB_ECDSA_Image_DecryptWithKeyAndVerify(&StorageArea, NULL /* PollParam_p - not used*/, secureBootPublicKey, NULL /* Hash_p - not used*/, Header_p, DataVectorIn, DataVectorOut, numImages /* VectorCount*/, ConfidentialityKeyType, imageDecryptionKey);
#ifdef CACHES_ON
  //disable data cache & MMU
   disable_caches();
#endif

   return Result;
}

void init_efuse_connection(void)
{
   if (efuse_connection_init == 0)
   {
      spl_log("%s %d\n",__FUNCTION__,__LINE__);
      //make sure programming disabled
      GME_FUSE_CONTROL_PROGRAM_ENABLE_W(0);
      //enable clock
      GME_PERIPH_CLOCK_ENABLE_FUSE_PROGRAM_CLK_EN_W(1);
      while(GME_PERIPH_CLOCK_ENABLE_STATUS_FUSE_PROGRAM_CLK_EN_R != 1);
      efuse_connection_init++;
      spl_log("%s %d\n",__FUNCTION__,__LINE__);
    }
}

void close_efuse_connection(void)
{
   //disable clock
   GME_PERIPH_CLOCK_ENABLE_FUSE_PROGRAM_CLK_EN_W(0);
   while(GME_PERIPH_CLOCK_ENABLE_STATUS_FUSE_PROGRAM_CLK_EN_R == 1);
   efuse_connection_init = 0;
   spl_log("%s %d\n",__FUNCTION__,__LINE__);
}

void efuse_write(UINT32 offset, unsigned int *efuse_data, UINT32 size)
{
   unsigned int ind;

#ifndef SIMULATE_EFUSE
   init_efuse_connection();
   for (ind = 0; ind < EFUSE_SIZE; ind++)
   {
      if(efuse_data[ind] != 0)
      {
         spl_log("write... ind %d %x row %d ",ind,efuse_data[ind]);
         GME_FUSE_PROGRAM_VAL_VAL = efuse_data[ind];
         GME_FUSE_CONTROL_PROGRAM_ROW_ADDR_W(ind);//rows are 8-bits
         GME_FUSE_CONTROL_PROGRAM_ENABLE_W(0xffffffff);
         while(GME_GENERAL_STATUS_0_FUSE_PROGRAM_DONE_R != 1);

         GME_FUSE_CONTROL_PROGRAM_ENABLE_W(0);
         while(GME_GENERAL_STATUS_0_FUSE_PROGRAM_DONE_R != 1);

         spl_log("done\n");

      }
     // add some delay
      VERIFY_WHILE_LOOP_LIMITED(1, VERIFY_MAX_ITERATIONS);
   }
   close_efuse_connection();
#else
   for(ind = 0; ind < EFUSE_SIZE; ind++)
   {
      if (efuse_data[ind] != 0)
      {
        spl_log("write... %d %x ",ind,efuse_data[ind]);
        spl_log("done\n");
      }
   }
#endif

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
*  Function Name: getEfuseSecurityNumber
*
*  Description: gets Security Version from eFuse
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
UINT8 getEfuseSecurityNumber()
{
   UINT8 securityNumber = 0;

   // Security Number is kept in the first 64 bits of the eFuse, that's why the 2 first eFuse registers should be read
   // To reconstruct the Security Number, the number of ones in those registers should be count.
   // For Example:
   // 0 ==> securityNumber[0] = 0x00000000 securityNumber[1] = 0x00000000
   // 1 ==> securityNumber[0] = 0x00000001 securityNumber[1] = 0x00000000
   // 2 ==> securityNumber[0] = 0x00000003 securityNumber[1] = 0x00000000
   // ...
   // 10 ==> securityNumber[0] = 0x000003FF securityNumber[1] = 0x00000000
   //...
   // 23 ==> securityNumber[0] = 0x007FFFFF securityNumber[1] = 0x00000000
   //...
   // 31 ==> securityNumber[0] = 0x7FFFFFFF securityNumber[1] = 0x00000000
   // 32 ==> securityNumber[0] = 0xFFFFFFFF securityNumber[1] = 0x00000000
   // 33 ==> securityNumber[0] = 0xFFFFFFFF securityNumber[1] = 0x00000001
   // 34 ==> securityNumber[0] = 0xFFFFFFFF securityNumber[1] = 0x00000003
   //...
   // 50 ==> securityNumber[0] = 0xFFFFFFFF securityNumber[1] = 0x0003FFFF
   //...
   // 63 ==> securityNumber[0] = 0xFFFFFFFF securityNumber[1] = 0x7FFFFFFF
   // 64 ==> securityNumber[0] = 0xFFFFFFFF securityNumber[1] = 0xFFFFFFFF
   UINT32 securityNumBitwise = eFUSEDB.securityNumber[0];

   while (securityNumBitwise)
   {
      securityNumber++;
      securityNumBitwise >>= 1;
      if (securityNumber == 32)
      {
         securityNumBitwise = eFUSEDB.securityNumber[1];
      }
   }

   return securityNumBitwise;
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
   securityControl=0x2F;//Register 5, byte 0  can be also 0x20
#endif
   spl_log("%s %s %d securityControl %x\n",__FILE__,__FUNCTION__,__LINE__,securityControl);
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
*  Function Name: readEfuseSecurityNumber
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Read Security Number
*
****************************************************************************/
void readEfuseSecurityNumber(SB_EfuseArea_t *eFUSEP)
{
   UINT32 securityNumber[2] = {0};
#ifndef SIMULATE_EFUSE
   // Security Number is kept in the first 64 bits of the eFuse, that's why the 2 first eFuse registers should be read
   securityNumber[0] = GME_FUSE_SHADOW_0_FUSES_R;
   securityNumber[1] = GME_FUSE_SHADOW_1_FUSES_R;
#else
   securityNumber[0] = 0;
   securityNumber[1] = 0;
#endif
   memcpy((void *)&(eFUSEP->securityNumber),(void *)&securityNumber,sizeof(eFUSEP->securityNumber));
}


/****************************************************************************
*
*  Function Name: writeEfuseSecurityNumber
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Read Security Number
*
****************************************************************************/
void writeEfuseSecurityNumber(UINT8 securityNumber)
{
   UINT32 eFuseData[EFUSE_SIZE];

   memset(eFuseData, 0, sizeof(eFuseData));

   if (securityNumber < 32)
   {
      eFuseData[0] = ((1 << securityNumber) - 1);
      eFuseData[1] = 0;
   }
   else if (securityNumber == 32)
   {
      eFuseData[0] = 0xFFFFFFFF;
      eFuseData[1] = 0;
   }
   else if ((securityNumber > 32) && (securityNumber < 64))
   {
      eFuseData[0] = 0xFFFFFFFF;
      eFuseData[1] = ((1 << (securityNumber - 32)) - 1);
   }
   else if (securityNumber == 64)
   {
      eFuseData[0] = 0xFFFFFFFF;
      eFuseData[1] = 0xFFFFFFFF;
   }

   efuse_write(0, eFuseData, EFUSE_SIZE);
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
   readEfuseSecurityNumber(&eFUSEDB);
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
SB_Result_t startSecureBoot(UINT32 securedImageAddress,UINT32 chunkSize,UINT32 neededComponent)
{
   SB_Result_t Result                  = SB_SUCCESS;
   UINT8       ConfidentialityKeyType  = getEfuseSecureControlData();

#ifdef CACHES_ON
   //Enable Data cache & MMU to increase secure boot performance
   enable_caches();
   //As soon as the MMU is enabled, these flat-mapped addresses will become invalid as the cache is now looking for virtual addresses as defined in the MMU page table, so it is important to remember to invalidate the I-Cache when enabling the MMU
   invalidate_icache_all();
#endif
   Result      = decrypt_and_verify_blw_image((const UINT8*)securedImageAddress, &eFUSEDB.publicKey, eFUSEDB.confidentialityKey, (SB_ConfidentialityKeyType_t)ConfidentialityKeyType,chunkSize);
   if (Result==SB_SUCCESS)
   {
      spl_log("SB SUCCESS address %x\n",securedImageAddress);
   }
   else
   {
      spl_log("SB ERROR: %x address %x\n",Result,securedImageAddress);
   }
   return Result;
}

