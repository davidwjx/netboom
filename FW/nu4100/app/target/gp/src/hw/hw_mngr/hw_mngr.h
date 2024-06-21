/****************************************************************************
 *
 *   FileName: hw_mngr.h
 *
 *   Author: Benny V.
 *
 *   Date: 
 *
 *   Description: HW control
 *   
 ****************************************************************************/
#ifndef HW_MNGR_H
#define HW_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "inu_defs.h"
#include "data_base.h"
#include "inu2.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


typedef struct
{
   inu_device__hwVersionIdU   hwVersion;
   inu_device__usbSpeedE      usbSpeed;
   INU_DEFSG_cameraModeE      imgMode;       // sensor/injection/generator
   UINT32                     hwRecoveryCntr;
   UINT32                     recoveryFilterCntr;
   UINT32                     recoveryFilterTimeSec;
   UINT16                     recoveryFilterTimeMs;
   OS_LYRG_mutexT             sysHwMutex;
   UINT32                     stereoActivity;
   UINT32                     webcamActivity;
} HW_MNGRG_generalInfoT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
// main control commands
HW_MNGRG_generalInfoT *HW_MNGRG_getGeneralInfo();
ERRG_codeE  HW_MNGRG_earlyInit();
ERRG_codeE  HW_MNGRG_init();
ERRG_codeE  HW_MNGRG_close();
ERRG_codeE  HW_MNGRG_startSystem();
ERRG_codeE  HW_MNGRG_recovery(BOOL restartSensors);
ERRG_codeE  HW_MNGRG_getHwVersion(inu_device__hwVersionIdU *hwVersionP);
ERRG_codeE  HW_MNGRG_getUsbSpeed(inu_device__usbSpeedE *usbSpeedP);
void        HW_MNGRG_printHwVersion();
void        HW_MNGRG_zeroRecoveryFilter();
void        HW_MNGRG_showStats();
UINT32      HW_MNGRG_getRecoveryStatus();
ERRG_codeE  HW_MNGRG_OPEN_calib_file(UINT8 flashNum, UINT32 fileSize, UINT32 offset);

#define TMP_CALIB_FILE "/tmp/calib_file.7z"
#define CALIB_DIRECTORY "/media/inuitive/Calib"
#define CALIB_FILE "CalibFile"

#define SPI_FLASH_DRVP_FLASH_READ_SIZE 0x800 // need to test this limitation

#ifdef __cplusplus
}
#endif

#endif //   HW_MNGR_H


