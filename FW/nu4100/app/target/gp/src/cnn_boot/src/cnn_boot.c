
/****************************************************************************
*
 *   FileName: cnn_boot.c
*
*   Author: Arnon C
*
*   Date:
*
*   Description: boot cnn
*
****************************************************************************/

/****************************************************************************
***************       I N C L U D E    F I L E S             ***************
****************************************************************************/
#include "inu_common.h"
#include "gme_drv.h"
#include "pss_drv.h"
#include "nu4100_regs.h"
#include "nu4100_pss_regs.h"


/****************************************************************************
***************      L O C A L       D E F N I T I O N S     ***************
****************************************************************************/

//   #define  LRAM_BASE_ADDRESS                0x02000000
//   #define  MAIL_BOX_DONE                    0x02030000
//   #define  MAIL_BOX_ERROR                   0x02030004

#define  CNN_BOOTP_BINARY_FILE_NAME            "/media/inuitive/cnn_boot_app.bin"
#define  CNN_BOOTP_LOADER_FILE_NAME            "/media/inuitive/cnn_boot_loader.bin"

#define  CNN_BOOTP_SLEEP_TIME          (10000)

typedef struct
{
   UINT32                     deviceBaseAddress;
} GME_DRVP_deviceDescT;

typedef struct
{
   UINT32                     deviceBaseAddress;
} PSS_DRVP_deviceDescT;

static GME_DRVP_deviceDescT   GME_DRVP_deviceDesc;
static PSS_DRVP_deviceDescT   PSS_DRVP_deviceDesc;
typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
        refdiv   :6,
        fbdiv    :12,
        unused   :4,
        postdiv1 :3,
        postdiv2 :3,
        dac      :1,
        dsm      :1;
} BOARD_pllT;

static BOOL EV61_active = 0;

/****************************************************************************
***************     L O C A L  F U N C T I O N     ***************
****************************************************************************/
static void EVP_BOOTP_updateDspPll()
{

   UINT32 foutvco,freq;
   UINT32 *pllRegPtr;

   BOARD_pllT pllReg;

   GME_DRVG_unitClkDivT gmeClkConfig;

   pllRegPtr = (unsigned int *)&pllReg;

   pllReg.pll_on   = 1;
   pllReg.bypass   = 0;
   pllReg.refdiv   = 12;
   pllReg.fbdiv    = 864; //  valid values from 500MHz-1500MHz
    pllReg.postdiv1 = 2;
    pllReg.postdiv2 = 1;
    pllReg.dac      = 0;
    pllReg.dsm      = 0;

    // --------------------------------- //
   // Configure DSP PLL parameters to 807Mhz
   // --------------------------------- //
   GME_DSP_PLL_CONTROL_VAL =  *pllRegPtr;
   while(!GME_DSP_PLL_STATUS_LOCK_R == 0);
   gmeClkConfig.div   = 1;
   gmeClkConfig.unit = GME_DRVG_HW_AUDIO_CLK_EN_E;
   GME_DRVG_changeUnitFreq(&gmeClkConfig);

   *pllRegPtr = GME_AUDIO_PLL_STATUS_VAL;
   //REading teh status register for printing
   foutvco = (24*pllReg.fbdiv)/pllReg.refdiv;
   freq = (foutvco/pllReg.postdiv1)/pllReg.postdiv2;
   LOGG_PRINT(LOG_INFO_E, NULL, "EVP frequency = %d MHz, pll status = 0x%x\n", freq, *pllRegPtr);

}
/****************************************************************************
*
*  Function Name: CNN_BOOTP_startCore
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE CNN_BOOTP_startArc( void )
{

   ERRG_codeE       ret = EV_CNN_BOOT__RET_SUCCESS;
   UINT32           reg;
   MEM_MAPG_addrT   memVirtAddr;

   //Workaround for EV fail boot at high frequencies (Errata NB-2249)
   PSS_DRVG_arcControl(0x66);
   ret =  GME_DRVG_enableClk(GME_DRVG_HW_UNIT_DSP_E);

   if(ERRG_SUCCEEDED(ret))
   {
      // BOOT the CNN
      PSS_DRVG_arcControl(0x44);
      LOGG_PRINT(LOG_INFO_E,NULL,"CNN core enabled\n");
   }

   return ret;
}



/****************************************************************************
*
*  Function Name: CNN_BOOTP_loadToMemory
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE CNN_BOOTP_loadToMemory( char *fileName, UINT32 virtAddress )
{
   ERRG_codeE     ret = EV_CNN_BOOT__RET_SUCCESS;
   FILE           *file;
   UINT32         size, fileSize;

   file   = fopen(fileName, "rb");
   if(!file)
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Can't find CNN file %s\n",fileName);
      return CNN_BOOT__ERR_FAILED_TO_OPEN_FILE;
   }

   //get image size
   fseek(file, 0L, SEEK_END);
   fileSize = ftell(file);
   fseek(file, 0L, SEEK_SET);

   LOGG_PRINT(LOG_INFO_E,NULL,"opened CNN file %s, file size = %d, to address = %p\n",fileName,fileSize,virtAddress);

   size = fread( (void*)virtAddress, 1, fileSize, file);
   if(!size)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR loading file %s\n",fileName);
      return CNN_BOOT__ERR_FAILED_TO_OPEN_FILE;
   }

   //Close file
   if(file)
   {
      fclose(file);
      remove(fileName);
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: CNN_BOOTG_start
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE CNN_BOOTG_start( void )
{
   ERRG_codeE     ret = EV_CNN_BOOT__RET_SUCCESS;
   UINT32         retry = 0;
   UINT32         mailBoxDoneAddressP,mailBoxErrorAddressP;
   MEM_MAPG_addrT       memVirtAddr;
   //volatile CEVA_BOOTG_bootSyncParamsT *CEVA_BOOTP_cevaBootParams;ELAD4100
   MEM_MAPG_addrT                oAddressP;
   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
   GME_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;

   MEM_MAPG_getVirtAddr(MEM_MAPG_LRAM_CNN_BOOT_MAIL_BOX_E, (MEM_MAPG_addrT*)&mailBoxDoneAddressP);

   mailBoxErrorAddressP = mailBoxDoneAddressP;
   mailBoxErrorAddressP = mailBoxErrorAddressP + 4;

   //Update EV with shared database address
   MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CEVA_BOOT_PARAMS, &oAddressP);
   //CEVA_BOOTP_cevaBootParams = (CEVA_BOOTG_bootSyncParamsT *)oAddressP;ELAD4100

   MEM_MAPG_getPhyAddr(MEM_MAPG_LRAM_DATA_BASE_E, &oAddressP);
   //CEVA_BOOTP_cevaBootParams->ddrDataBaseAddress = (UINT32)oAddressP;ELAD4100

   MEM_MAPG_getVirtAddr(MEM_MAPG_LRAM_CNN_BOOT_E, &memVirtAddr);
   ret = CNN_BOOTP_loadToMemory(CNN_BOOTP_LOADER_FILE_NAME,(UINT32)memVirtAddr);

   if (ERRG_SUCCEEDED(ret))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_EV62_CODE_E, &memVirtAddr);
      ret = CNN_BOOTP_loadToMemory(CNN_BOOTP_BINARY_FILE_NAME,(UINT32)memVirtAddr);
   }

   //if files were found, load them and use them as the boot image
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_initializeCpLog(MEM_MAPG_EV61_LOG_ADDRESS,OS_LYRG_CP_MUTEX_EV61_LOG_E);
      LOGG_PRINT(LOG_INFO_E, NULL, "File found, start boot from application\n");
      EVP_BOOTP_updateDspPll();
      ret = GME_DRVG_setPowerMode(GME_DRVG_POWER_DSP_E);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "error setting power mode for EVP\n");
         return CNN_BOOT__ERR_BOOT_FAIL;
      }

      *(volatile UINT32*)(mailBoxDoneAddressP)  = 0x0;
      *(volatile UINT32*)(mailBoxErrorAddressP) = 0x0;

      ret = CNN_BOOTP_startArc();
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "error init cnn core\n");
         return ret;
      }
   }
   else
   {
      //check if the image was loaded in the SPL. if not, can exit here with success ret code
      UINT32 oVirtualAddrP;
      MEM_MAPG_getVirtAddr(MEM_MAPG_LRAM_CNN_BOOT_E, (MEM_MAPG_addrT*)&oVirtualAddrP);
      if ((*(UINT32*)oVirtualAddrP) != ((UINT32)0x2000060))
      {
         return EV_CNN_BOOT__RET_SUCCESS;
      }
   }

   //ret was used as indication if files exists and we need to use them, now reset the ret var
   ret = EV_CNN_BOOT__RET_SUCCESS;
   //wait until arc boot complete here
   while (retry < 100)
   {
      if ((*(volatile UINT32*)(mailBoxDoneAddressP) == 0x1))
      {
         break;
      }

      OS_LYRG_usleep(CNN_BOOTP_SLEEP_TIME);
      retry++;
   }

   if (retry == 100)
   {
      ret = CNN_BOOT__ERR_BOOT_FAIL;
   }

   //check for any error codes from the ARC
   if (*(volatile UINT32*)(mailBoxErrorAddressP))
   {
      ret = (*(volatile UINT32*)(mailBoxErrorAddressP));
   }

   //OS_LYRG_usleep(CNN_BOOTP_SLEEP_TIME);

   if(ERRG_FAILED(ret))
   {
       LOGG_PRINT(LOG_ERROR_E,ret,"CNN boot finished with Errors\n");
       EV61_active = false;
   }
   else
   {
       EV61_active = true;
       LOGG_PRINT(LOG_INFO_E,NULL,"CNN boot done\n");
   }

   return(ret);
}


/****************************************************************************
*
*  Function Name: CNN_BOOTG_reloadApp
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE CNN_BOOTG_reloadApp( void )
{
   ERRG_codeE       ret = EV_CNN_BOOT__RET_SUCCESS;
   MEM_MAPG_addrT   src, dest;
   UINT32           size;

   // halt the CNN
   PSS_DRVG_arcControl(0x22);

   MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_EV62_CODE_E, &dest);
   MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_EV62_BACKUP_CODE_E, &src);
   MEM_MAPG_getSize(MEM_MAPG_DDR_EV62_BACKUP_CODE_E, &size);
   memcpy(dest, src, size);

   return ret;
}

/****************************************************************************
*
*  Function Name: CNN_BOOTG_isActive
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
BOOL CNN_BOOTG_isActive( void )
{
   return EV61_active;
}


