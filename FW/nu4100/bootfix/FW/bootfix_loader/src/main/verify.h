#include "sb_ecdsa.h"
#include "defs.h"

/****************************************************************************
 ***************       G L O B A L   T Y P E D E F S           ***************
 ****************************************************************************/
#define SB_CONFIDENTIALITY_BYTES    (16)   //Ksbcr is 128 bit
/*The public key used for Elliptic Curve Cryptography is a point on the selected curve. For a curve with the order / group size of 256 bits, such a point consists of an (x, y) coordinate where x and y are each 256-bit numbers. Hence the total size of 512 bits for the public key.*/
#define SB_ECDSA_BYTES              64         //Kpublic is 256*2 bit
#define NOT_SECURED_IMAGE           (0xFF)

/****************************************************************************
 ***************       G L O B A L         D A T A              ***************
 ****************************************************************************/
   /* Bit 0 - GPP JTAG Disable
        Bit 1 - CEVA JTAG Disable
        Bit 2 - EV62 JTAG Disable
        Bit 3 - Test pins control (access disable to Efuse) 
        Bit 4 - GPP burn control (disable burn via FW)  
        Bit 5 - Integrity force 
        Bit 6 - Encryption force 
        Bits 7 - reserved
   */
typedef struct
{
   UINT8 gpp_jtag_disable  : 1;
   UINT8 ceva_jtag_disable : 1;
   UINT8 ev62_jtag_disable : 1;
   UINT8 test_pins_control : 1;
   UINT8 gpp_burn_control  : 1;
   UINT8 integrity_force   : 1;
   UINT8 encryption_force  : 1;
   UINT8 reserved          : 1;
}securityControlT;

// EFUSE Storage area for public key and sbcr key
typedef struct
{
   UINT8                   generalUse[20]; //Available for general use
   securityControlT        securityControl;
   SBIF_ECDSA_PublicKey_t  publicKey; //64 bytes
   UINT8                   confidentialityKey[SB_CONFIDENTIALITY_BYTES];//16 bytes
}
SB_EfuseArea_t;

/****************************************************************************
 ***************       G L O B A L         F U N C T I O N S                                 ***************
 ****************************************************************************/
void startSecureBoot(UINT32 securedImageAddress,UINT32 chunkSize);

void fillSBEfuseDB();
UINT8 getEfuseSecureControlData();




