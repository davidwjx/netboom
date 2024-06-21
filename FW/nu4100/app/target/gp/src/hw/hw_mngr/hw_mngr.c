/****************************************************************************
 *
 *   FileName: hw_mngr.c
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: HW control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "hw_mngr.h"
#include "gme_mngr.h"
#include "ppe_mngr.h"
#include "dpe_mngr.h"
#include "cva_mngr.h"
#include "hcg_mngr.h"
#include "isp_mngr.hpp"
#include "depth_post_mngr.h"
#include "iae_mngr.h"
#include "dpe_mngr.h"
#include "cde_mngr_new.h"
#include "sensors_mngr.h"
#include "mipi_mngr.h"
#include "sequence_mngr.h"
#include "lut_mngr.h"
#include "os_lyr.h"
#include "io_pal.h"
#include "hw_regs.h"
#include "spi_flash_drv.h"
#include "proj_drv.h"
#include "pss_drv.h"
#include "data_base.h"
#include "dw_umctl2.h"
#include "nu4100_regs.h"
#include "iae_drv.h"
#include "i2s.h"
#include "inu_storage_layout.h"
#include "spi_flash.h"
#include "inu_str.h"
#include "rtc.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

#define HW_MNGRP_RECOVERY_COUNTER_THRESHOLD        (2)
#define HW_MNGRP_ZERO_RECOVERY_CNTR_TIME_INTERVAL  (15000)  // 15sec
#define HW_MNGRP_USB_STATUS_REGISTER_ADDR          (0x0900C70C)

#define DSE_PREFER_WR (1)
#define DSE_RD_WR_IDLE_GAP (0x32)
#define DSE_LPR_NUM_ENTRIES (0x1e)

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
   HW_MNGRP_USB_HIGH_SPEED_E     = 0,
   HW_MNGRP_USB_FULL_SPEED_1_E   = 1,
   HW_MNGRP_USB_LOW_SPEED_E      = 2,
   HW_MNGRP_USB_FULL_SPEED_2_E   = 3,
   HW_MNGRP_USB_SUPER_SPEED_E    = 4
} HW_MNGRP_usbSpeedFieldE;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static HW_MNGRG_generalInfoT  HW_MNGRP_info;
static UINT32                 recoveryStatus = 0;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/
void inu_device__setFlashGpio(UINT8 flashNum);
/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

// general functions
static ERRG_codeE HW_MNGRP_initHwInfo();
static ERRG_codeE HW_MNGRP_initHw();
static void       HW_MNGRP_lockHw();
static void       HW_MNGRP_unlockHw();
// SPI FLASH functions
static ERRG_codeE HW_MNGRP_openSpiDriver();
static ERRG_codeE HW_MNGRP_closeSpiDriver();

static ERRG_codeE HW_MNGRP_openProjDriver();
static ERRG_codeE HW_MNGRP_closeProjDriver();
/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: HW_MNGRP_initHwInfo
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_MNGRP_initHwInfo()
{
   ERRG_codeE  retCode = HW_MNGR__RET_SUCCESS;

// reset all peripheral control module info structure
   memset(&HW_MNGRP_info, 0, sizeof(HW_MNGRG_generalInfoT));

   HW_MNGRP_info.hwVersion.val = GME_MNGRG_getHwVersion();


   HW_MNGRG_getUsbSpeed(&HW_MNGRP_info.usbSpeed);
   HW_MNGRP_info.imgMode         = INU_DEFSG_CAMERA_MODE_SENSOR_E;
   HW_MNGRP_info.hwRecoveryCntr  = 0;

   OS_LYRG_getTime(&HW_MNGRP_info.recoveryFilterTimeSec, &HW_MNGRP_info.recoveryFilterTimeMs);
   OS_LYRG_aquireMutex(&HW_MNGRP_info.sysHwMutex);

   DW_MCTLG_setSched(DSE_PREFER_WR, DSE_RD_WR_IDLE_GAP, DSE_LPR_NUM_ENTRIES);

   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRP_initHw
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_MNGRP_initHw()
{
   ERRG_codeE           retCode = HW_MNGR__RET_SUCCESS;

   LOGG_PRINT(LOG_INFO_E, NULL, "init HW info\n");

   retCode = HW_REGSG_init();

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_MNGRG_open();
   }

#ifndef __UART_ON_FPGA__
   if(ERRG_SUCCEEDED(retCode))
   {
      // init HW general configuration
      retCode = HW_MNGRP_initHwInfo();
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = GME_MNGRG_powerUp(HW_MNGRP_info.usbSpeed);
      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = HCG_MNGRG_initVoteSystem();
         LOGG_PRINT(LOG_INFO_E, NULL, "Init HCG voting system\n");
      }

      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = GME_MNGRG_resetHw(TRUE);
      }
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      // init sensors configuration
      SENSORS_MNGRG_init();
   }
#endif
   return(retCode);
}

#ifdef OLD_API
/****************************************************************************
*
*  Function Name: HW_MNGRP_checkRecoveryFilter
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
static BOOL HW_MNGRP_checkRecoveryFilter()
{
    UINT32 deltaMsSinceLastRecovery = OS_LYRG_deltaMsec(HW_MNGRP_info.recoveryFilterTimeSec, HW_MNGRP_info.recoveryFilterTimeMs);

   if(deltaMsSinceLastRecovery > HW_MNGRP_ZERO_RECOVERY_CNTR_TIME_INTERVAL)
   {
      HW_MNGRP_info.recoveryFilterCntr = 1;
      OS_LYRG_getTime(&HW_MNGRP_info.recoveryFilterTimeSec, &HW_MNGRP_info.recoveryFilterTimeMs);
   }
   else
   {
      HW_MNGRP_info.recoveryFilterCntr++;
      if(HW_MNGRP_info.recoveryFilterCntr >= HW_MNGRP_RECOVERY_COUNTER_THRESHOLD)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Recovery number %d within %d ms --> Reset sensors too\n", HW_MNGRP_info.recoveryFilterCntr, deltaMsSinceLastRecovery);
         return(TRUE);
      }
   }
   return(FALSE);
}
#endif

/****************************************************************************
*
*  Function Name: HW_MNGRP_lockHw
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
static void HW_MNGRP_lockHw()
{
   OS_LYRG_lockMutex(&HW_MNGRP_info.sysHwMutex);
}

/****************************************************************************
*
*  Function Name: HW_MNGRP_unlockHw
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
static void HW_MNGRP_unlockHw()
{
   OS_LYRG_unlockMutex(&HW_MNGRP_info.sysHwMutex);
}

/****************************************************************************
*
*  Function Name: HW_MNGRP_openSocDrivers
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
/*static ERRG_codeE HW_MNGRP_openSocDrivers()
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = CDE_MNGRG_openCdeMngrDrv();
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "open CDE driver fail.\n");
      }
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = DPE_MNGRG_openDpeDrv();
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "open DPE driver fail.\n");
      }
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = IAE_MNGRG_openIaeDrv();
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "open IAE driver fail.\n");
      }
   }

   return(retCode);
}*/

/****************************************************************************
*
*  Function Name: HW_MNGRP_closeSocDrivers
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
/*static ERRG_codeE HW_MNGRP_closeSocDrivers()
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = IAE_MNGRG_closeIaeDrv();
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = DPE_MNGRG_closeDpeDrv();
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = CDE_MNGRG_closeCdeDrv();
   }

   return(retCode);
}*/

/****************************************************************************
*
*  Function Name: HW_MNGRP_openSpiDriver
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_MNGRP_openSpiDriver()
{
   ERRG_codeE                    retCode = HW_MNGR__RET_SUCCESS;
   SPI_FLASH_DRVG_openParamsT    flashDrvOpenParams;
   IO_HANDLE                     ioHandle;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
   char *deviceNames[3]={"/dev/spidev0.0","/dev/spidev0.3","/dev/spidev1.0"};

   LOGG_PRINT(LOG_INFO_E, NULL, "open SPI FLASH driver\n");

   flashDrvOpenParams.deviceName = deviceNames[0];
   retCode = IO_PALG_open(&ioHandle, IO_SPI_FLASH_1_E, &flashDrvOpenParams);
   if ((modelType >= INU_DEFSG_BOOT82_E) && (modelType <= INU_DEFSG_BOOT87_E))
   {
      inu_device__powerDownCmdE powerDownStatus;

      // skip opening secondary flash in case we are exiting from suspend/pd state, two reasons:
      // 1. there is an issue (wasn't investigated) in secondary flash recovery from power down
      // 2. secondary flash stores only calibartion and device info, which we already have from
      //    the initial boot, so we don't need to access them again
      inu_device__getPowerDownStatus(&powerDownStatus);
      if (powerDownStatus != INU_DEVICE_POWER_DOWN_EXIT_E)
      {
         flashDrvOpenParams.deviceName = deviceNames[1];
         if ((modelType == INU_DEFSG_BOOT86_E) || (modelType == INU_DEFSG_BOOT87_E))
         {
            flashDrvOpenParams.deviceName = deviceNames[2];
         }
         else
         {
            inu_device__setFlashGpio(SECONDARY_FLASH);
         }
         retCode = IO_PALG_open(&ioHandle, IO_SPI_FLASH_2_E, &flashDrvOpenParams);
      }
   }
   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRP_closeSpiDriver
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_MNGRP_closeSpiDriver()
{
   ERRG_codeE  retCode = HW_MNGR__RET_SUCCESS;
   INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();

   IO_PALG_close(IO_PALG_getHandle(IO_SPI_FLASH_1_E));
   if ((modelType >= INU_DEFSG_BOOT82_E) && (modelType <= INU_DEFSG_BOOT87_E))
      IO_PALG_close(IO_PALG_getHandle(IO_SPI_FLASH_2_E));

   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRP_openProjDriver
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE HW_MNGRP_openProjDriver()
{
   ERRG_codeE                    retCode = HW_MNGR__RET_SUCCESS;
   PROJ_DRVG_openParamsT         projDrvOpenParams;
   IO_HANDLE                     ioHandle;

   retCode = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, &projDrvOpenParams.gmeBaseAddress);

   if(ERRG_SUCCEEDED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "open Proj driver\n");
      retCode = IO_PALG_open(&ioHandle, IO_PROJ_0_E, &projDrvOpenParams);
   }

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open Proj driver fail.\n");
   }

   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRP_closeProjDriver
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
static ERRG_codeE HW_MNGRP_closeProjDriver()
{
   ERRG_codeE  retCode = HW_MNGR__RET_SUCCESS;

   IO_PALG_close(IO_PALG_getHandle(IO_PROJ_0_E));

   return(retCode);
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: HW_MNGRG_showStats
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
void HW_MNGRG_showStats()
{

}

/****************************************************************************
*
*  Function Name: HW_MNGRG_getHwVersion
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_getHwVersion(inu_device__hwVersionIdU     *hwVersionP)
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;

   if(hwVersionP)
   {
      hwVersionP->val         = HW_MNGRP_info.hwVersion.val;
   }

   return (retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_printHwVersion
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
void HW_MNGRG_printHwVersion()
{
   inu_device__hwVersionIdU   hwVersion;
   char                       revisionString[INU_STRG_IOCTL_OPCODE_STR_LEN] = {0};
   char                       versionString[INU_STRG_IOCTL_OPCODE_STR_LEN] = {0};

   HW_MNGRG_getHwVersion(&hwVersion);
   LOGG_PRINT(LOG_INFO_E, NULL, "HW version: 0x%X (Version: %s - %s).\n", hwVersion.val, INU_STRG_revisionId2String(hwVersion.fields.revisionId, revisionString), INU_STRG_versionId2String(hwVersion.fields.versionId, versionString));
   LOGG_PRINT(LOG_INFO_E, NULL, "\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "#     # #     # #          #      ###     ###\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "##    # #     # #    #    ##     #   #   #   #\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "# #   # #     # #    #   # #    # #   # # #   #\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "#  #  # #     # #######    #    #  #  # #  #  #\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "#   # # #     #      #     #    #   # # #   # #\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "#    ## #     #      #     #     #   #   #   #\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "#     #  #####       #   #####    ###     ###\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "USB speed: USB-%d\n", HW_MNGRP_info.usbSpeed);
   LOGG_PRINT(LOG_INFO_E, NULL, "BootId: %d\n", GME_MNGRG_getModelType());
   LOGG_PRINT(LOG_INFO_E, NULL, "Base version: %d\n", GME_MNGRG_getBaseVersion());
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_OPEN_calib_file
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_OPEN_calib_file(UINT8 flashNum, UINT32 fileSize, UINT32 offset)
{
   FILE *calibFile;
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;
   char buff[SPI_FLASH_DRVP_FLASH_READ_SIZE];
   UINT32 ind, readSize;

   calibFile = fopen(TMP_CALIB_FILE, "wb");
   if (calibFile == NULL)
      return HW_MNGR__ERR_CALIB_FAIL;

   for (ind = 0; ind < fileSize; ind += SPI_FLASH_DRVP_FLASH_READ_SIZE)
   {
      if ((fileSize - ind ) > SPI_FLASH_DRVP_FLASH_READ_SIZE)
         readSize = SPI_FLASH_DRVP_FLASH_READ_SIZE;
      else
         readSize = fileSize - ind;

      retCode = SPI_FLASHG_Read(flashNum, offset + ind, readSize, (BYTE *)buff);
      if (ERRG_SUCCEEDED(retCode))
      {
         fwrite(buff, 1,readSize,calibFile);
      }
      else break;
   }
   fclose(calibFile);

   if (ERRG_SUCCEEDED(retCode))
   {
      sprintf(buff,"mkdir %s",CALIB_DIRECTORY);
      system(buff);
      LOGG_PRINT(LOG_INFO_E, NULL, "Read calibration commands: %s\n",buff);

      sprintf(buff,"unzip %s -d %s",TMP_CALIB_FILE,CALIB_DIRECTORY);
      system(buff);
      LOGG_PRINT(LOG_INFO_E, NULL, "Read calibration commands: %s\n",buff);

      sprintf(buff, "mv %s/* %s/%s",CALIB_DIRECTORY,CALIB_DIRECTORY,CALIB_FILE);
      system(buff);
      LOGG_PRINT(LOG_INFO_E, NULL, "Read calibration commands: %s\n",buff);

      sprintf(buff, "rm -f %s",TMP_CALIB_FILE);
      system(buff);
      LOGG_PRINT(LOG_INFO_E, NULL, "Read calibration commands: %s\n",buff);
   }
   return retCode;
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_earlyInit
*
*  Description: init and prepare all system peripherals.
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_earlyInit()
{
   ERRG_codeE        retCode = HW_MNGR__RET_SUCCESS;
   MEM_MAPG_addrT    memVirtAddr;
   LOGG_PRINT(LOG_INFO_E, NULL, "early init HW manager\n");

   retCode = IO_PALG_init();

   if(ERRG_SUCCEEDED(retCode))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
      retCode = GME_MNGRG_init((UINT32)memVirtAddr);
   }
   return retCode;
}


/****************************************************************************
*
*  Function Name: HW_MNGRG_init
*
*  Description: init and prepare all system peripherals.
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_init()
{
   ERRG_codeE        retCode = HW_MNGR__RET_SUCCESS;
   MEM_MAPG_addrT    memVirtAddr,memVirtAddrIsp0, memVirtAddrIsp1, memVirtAddrDdr, memVirtAddrIpe;
   LOGG_PRINT(LOG_INFO_E, NULL, "init HW manager\n");

   if(ERRG_SUCCEEDED(retCode))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_IAE_E, (&memVirtAddr));
      retCode = IAE_MNGRG_init((UINT32)memVirtAddr);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_PPE_E, (&memVirtAddr));
      retCode = PPE_MNGRG_init((UINT32)memVirtAddr);
      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = DEPTH_POSTG_init((UINT32)memVirtAddr);
      }
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_CVA_E, (&memVirtAddr));
      retCode = CVA_MNGRG_init((UINT32)memVirtAddr);
   }
   if(ERRG_SUCCEEDED(retCode))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_PSS_E, (&memVirtAddr));
      retCode = PSS_DRVG_init((UINT32)memVirtAddr);
   }
   if(ERRG_SUCCEEDED(retCode))
   {      
      retCode = RTC_DRVG_init((UINT32)memVirtAddr);
   }
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " RTC_DRVG_init failed \n");
   }   
   
   retCode = SEQ_MNGRG_init( );
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " SEQ_MNGRG_init failed \n");
   }
   else
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, " SEQ_MNGRG_init done \n");
   }

   retCode = CDE_MNGRG_init();
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " CDE_MNGRG_init failed \n");
   }
   else
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, " CDE_MNGRG_init done \n");
   }

   retCode = CDE_MNGRG_open();
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " CDE_MNGRG_open failed \n");
   }
   else
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, " CDE_MNGRG_open done \n");
   }

   retCode = lut_mngr_init();
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, " lut_mngr_init failed \n");
   }
   else
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, " lut_mngr_init done \n");
   }

   // Enable ISP
   if(ERRG_SUCCEEDED(retCode))
   {
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_ISP0_E, (&memVirtAddrIsp0));
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_ISP1_E, (&memVirtAddrIsp1));
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_IPE_E,  (&memVirtAddrIpe));

      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_DDRC_MP_E, (&memVirtAddrDdr));
      retCode = ISP_MNGRG_init((UINT32)memVirtAddrIsp0, (UINT32)memVirtAddrIsp1, (UINT32)memVirtAddrIpe, (UINT32)memVirtAddrDdr);
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = HW_MNGRP_initHw();
      //HW_MNGRG_printHwVersion();
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = HW_MNGRP_openSpiDriver();
   }
#ifndef __UART_ON_FPGA__
   if(ERRG_SUCCEEDED(retCode))
   {
      retCode = HW_MNGRP_openProjDriver();
   }
#endif
   // Open sensor drivers, the autodetection is performed automatically if sensor model is not specified
 /*  if(ERRG_SUCCEEDED(retCode))
   {
      retCode = SENSORS_MNGRG_openSensorsDrv();
   }
   */

   lut_mngr_gpClearLutDb();

   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_close
*
*  Description: close and reset all system peripherals.
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_close()
{
   ERRG_codeE retCode = HW_MNGR__RET_SUCCESS;

   LOGG_PRINT(LOG_INFO_E, NULL, "close HW mngr\n");

   IAE_MNGRG_deinit();

   lut_mngr_deinit();

   DPE_MNGRG_clearDpeInterrupts();

   if(ERRG_SUCCEEDED(retCode))
   {
      MIPI_MNGRG_deInit();
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      GME_MNGRG_resetHw(TRUE);
   }

   GME_MNGRG_powerDown();
   HW_MNGRP_closeSpiDriver();

   HW_MNGRP_closeProjDriver();

   HW_REGSG_close();

   SEQ_MNGRG_deinit();

   OS_LYRG_releaseMutex(&HW_MNGRP_info.sysHwMutex);

   LOGG_PRINT(LOG_INFO_E, NULL, "close system peripherals\n");
   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_startSystem
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_startSystem()
{
   ERRG_codeE  retCode        = HW_MNGR__RET_SUCCESS;
   BOOL        resetSensors   = FALSE;

   HW_MNGRP_lockHw();
   recoveryStatus = 1;
   if(ERRG_SUCCEEDED(retCode))
   {
      GME_MNGRG_resetHw(resetSensors);
   }


   if(ERRG_SUCCEEDED(retCode))
   {
//      retCode = IAE_MNGRG_clearInterrupts();
      retCode = DPE_MNGRG_clearDpeInterrupts();
   }

   recoveryStatus = 0;
   HW_MNGRP_unlockHw();

   return(retCode);
}
UINT32 HW_MNGRG_getRecoveryStatus()
{
   return(recoveryStatus);
}


/****************************************************************************
*
*  Function Name: HW_MNGRG_recovery
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE HW_MNGRG_recovery(BOOL restartSensors)
{
   ERRG_codeE              retCode = HW_MNGR__RET_SUCCESS;
   INU_DEFSG_logLevelE     logLevel;

   HW_MNGRP_lockHw();
   recoveryStatus = 1;

   LOGG_getDbgLevel(&logLevel);
   LOGG_setDbgLevel(LOG_ERROR_E);

   HW_MNGRP_info.hwRecoveryCntr++;

#ifdef OLD_API
   retCode = SEQ_MNGRG_stopSystem( SYSTEMG_getXMLDB() );

   retCode = SEQ_MNGRG_clearSystemConfig();
   retCode = SEQ_MNGRG_configSystem( SYSTEMG_getXMLDB(), FALSE );
   retCode = SEQ_MNGRG_startSystem( SYSTEMG_getXMLDB() );
#else
   (void)restartSensors;
#endif

   LOGG_setDbgLevel(logLevel);

   recoveryStatus = 0;
   HW_MNGRP_unlockHw();

   LOGG_PRINT(LOG_ERROR_E, NULL, "RECOVERY (number %d)\n", HW_MNGRP_info.hwRecoveryCntr);

   return(retCode);
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_getGeneralInfo
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: pheripherals control
*
****************************************************************************/
HW_MNGRG_generalInfoT *HW_MNGRG_getGeneralInfo()
{
   return(&HW_MNGRP_info);
}

/****************************************************************************
*
*  Function Name: HW_MNGRG_zeroRecoveryFilter
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
void HW_MNGRG_zeroRecoveryFilter()
{
   HW_MNGRP_info.recoveryFilterCntr = 0;
   OS_LYRG_getTime(&HW_MNGRP_info.recoveryFilterTimeSec, &HW_MNGRP_info.recoveryFilterTimeMs);
}


/****************************************************************************
*
*  Function Name: HW_MNGRG_getUsbSpeed
*
*  Description:
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE	HW_MNGRG_getUsbSpeed(inu_device__usbSpeedE *usbSpeedP)
{
   UINT32                  usbStatusReg;
   HW_MNGRP_usbSpeedFieldE connSpeedField;
   ERRG_codeE              retCode = HW_MNGR__RET_SUCCESS;
   HW_REGSG_readSocReg((UINT32)HW_MNGRP_USB_STATUS_REGISTER_ADDR, &usbStatusReg);
   connSpeedField = usbStatusReg & 0x7;

   switch(connSpeedField)
   {
      case HW_MNGRP_USB_HIGH_SPEED_E:
      {
         *usbSpeedP = INU_DEFSG_USB_2_E;
      }
      break;

      case HW_MNGRP_USB_FULL_SPEED_1_E:
      case HW_MNGRP_USB_LOW_SPEED_E:
      case HW_MNGRP_USB_FULL_SPEED_2_E:
      {
         *usbSpeedP = INU_DEFSG_USB_1_E;
      }
      break;

      case HW_MNGRP_USB_SUPER_SPEED_E:
      {
         *usbSpeedP = INU_DEFSG_USB_3_E;
      }
      break;

      default:
      {
         *usbSpeedP = INU_DEFSG_USB_3_E;
      }
      break;
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "detect USB connection. USB type=%d\n", *usbSpeedP);

   return(retCode);
}


#ifdef __cplusplus
 }
#endif

