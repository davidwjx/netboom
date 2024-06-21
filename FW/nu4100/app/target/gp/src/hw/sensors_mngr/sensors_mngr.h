/****************************************************************************
 *
 *   FileName: iae_mngr.c
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: iae control
 *
 ****************************************************************************/

#ifndef SENSORS_MNGR_H
#define SENSORS_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "nufld.h"
#include "xml_db.h"
#include "proj_drv.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void (*SENSORS_MNGRP_customGpioCb)(int direction, int value);
typedef void (*SENSORS_MNGRP_customStrobeIsrCb)(UINT64 timestamp, UINT64 count, void *cookie);

typedef struct                          //Initial ISP parameters
{
    UINT32                  bayerFormat;
    UINT32                  hts;
    UINT32                  sclk;
    UINT32                  frmLengthLines;
    UINT32                  minFps;
    UINT32                  maxFps;
    UINT32                  minIntegrationLine;
    UINT32                  maxIntegrationLine;
    float                   minGain;
    float                   maxGain;
    float                   aecMaxIntegrationTime;
}SENSORS_MNGRG_ispInitParamsT;

typedef struct
{
   UINT32                  sensorActivity;
   UINT32                  pendingChange;
   UINT32                  pendingContext;
   UINT32                  pendingExposure[INU_DEFSG_SENSOR_NUM_CONTEXTS]; //in usec
   UINT32                  currentFps;
   UINT32                  exposure[INU_DEFSG_SENSOR_NUM_CONTEXTS]; //in usec
   float                   analogGain[INU_DEFSG_SENSOR_NUM_CONTEXTS];
   float                   digitalGain[INU_DEFSG_SENSOR_NUM_CONTEXTS];
   UINT64                  gpioBitmap;
   UINT32                  pulseTime;
   INUG_ioctlSensorConfigT sensorCfg;
   IO_HANDLE               sensorHandle;
   UINT32                  sensorTableHandleIndex;
   UINT64                  strobeTs;
   UINT64                  readOutTs;
   UINT64                  strobeCtr;
   OS_LYRG_mutexT          strobeTsMutex;
   void                   *triggerH;
   void                   *altH;
   void                   *isrForTuningDelay;
   UINT32                  projectorState;
   INU_DEFSG_projectorModeE pendingProjectorChange;
   INU_DEFSG_projectorModeE pendingProjectorState;
   SENSORS_MNGRG_ispInitParamsT isp;
   UINT32                  focusCtrlSupport;
} SENSORS_MNGRG_sensorInfoT;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 *************************************************************************/



void       SENSORS_MNGRG_init();

SENSORS_MNGRG_sensorInfoT *SENSORS_MNGRG_getSensorInfo(INU_DEFSG_senSelectE sensorSelect);
SENSORS_MNGRG_sensorInfoT *SENSORS_MNGRG_getSensorInfoByModel(INU_DEFSG_sensorModelE sensorModel);

void       SENSORS_MNGRG_showAllSensorsCfg();


//-----------------------new interface
ERRG_codeE SENSORS_MNGRG_setConfigSensor( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE SENSORS_MNGRG_setEnableSensor( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE SENSORS_MNGRG_setDisableSensor( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
ERRG_codeE SENSORS_MNGRG_open(IO_HANDLE *ioHandleP,INUG_ioctlSensorConfigT *sensoConfig);
ERRG_codeE SENSORS_MNGRG_close(IO_HANDLE ioHandle);
ERRG_codeE SENSORS_MNGRG_startSensor(IO_HANDLE ioHandle);
ERRG_codeE SENSORS_MNGRG_stopSensor(IO_HANDLE ioHandle);
void       SENSORS_MNGRG_updateSensorFPS(IO_HANDLE ioHandle, UINT32 FPS);
void       SENSORS_MNGRG_updateSensorGain(IO_HANDLE ioHandle, float analogGain, float digitalGain, inu_sensor__sensorContext_e context);
void       SENSORS_MNGRG_updateSensorExpTime(IO_HANDLE ioHandle, UINT32 expTime, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRG_getSensorExpTime(IO_HANDLE ioHandle, UINT32 *expTimeP, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRG_setSensorExpTime(IO_HANDLE ioHandle, UINT32 *expTimeP, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRG_setSensorExpTimeAndGain(IO_HANDLE ioHandle, UINT32 *expTimeP,float *analogGainP, float *digitalGainP, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRG_getSensorGain(IO_HANDLE sensorHandle, float *analogGainP, float *digitalGainP, inu_sensor__sensorContext_e context);
ERRG_codeE SENSORS_MNGRG_setSensorGain(IO_HANDLE sensorHandle, float *analogGainP, float *digitalGainP, inu_sensor__sensorContext_e context);
ERRG_codeE SENSORS_MNGRG_getStrobeData(IO_HANDLE ioHandle, UINT64 *timestampP, UINT64 *ctrP, UINT32 *projMode);
ERRG_codeE SENSORS_MNGRP_getSensorFps(IO_HANDLE ioHandle, UINT32 *fpsP, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRP_setSensorFps(SENSORS_MNGRG_sensorInfoT *sensorInfoP, UINT32 *fpsP, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRP_runtimeSetConfigSensor(IO_HANDLE sensorHandle, inu_sensor__runtimeCfg_t *cfgP);
ERRG_codeE SENSORS_MNGRP_runtimeGetConfigSensor(IO_HANDLE sensorHandle, inu_sensor__runtimeCfg_t *cfgP);
UINT16     SENSORS_MNGRG_getSensorPixelClock(IO_HANDLE ioHandle);
ERRG_codeE SENSORS_MNGRG_resetStrobeCnt(IO_HANDLE ioHandle);
ERRG_codeE SENSORS_MNGRG_changeExpMode(IO_HANDLE ioHandle, INU_DEFSG_sensorExpModeE expMode);


ERRG_codeE SENSORS_MNGRG_getChipID(IO_HANDLE sensorHandle, UINT32 *chipID);
ERRG_codeE SENSORS_MNGRG_readReg(IO_HANDLE sensorHandle, UINT16 address, UINT32 *value);
ERRG_codeE SENSORS_MNGRG_writeReg(IO_HANDLE sensorHandle, UINT16 address, UINT32 value);
ERRG_codeE SENSORS_MNGRG_activateTestPattern(IO_HANDLE sensorHandle, BOOLEAN enable);
ERRG_codeE SENSORS_MNGRG_setBayerPattern(IO_HANDLE sensorHandle, UINT8 pattern);
ERRG_codeE SENSORS_MNGRG_setBLC(IO_HANDLE sensorHandle, UINT32 r, UINT32 gr, UINT32 gb, UINT32 b);
ERRG_codeE SENSORS_MNGRG_setWB(IO_HANDLE sensorHandle, UINT32 r, UINT32 gr, UINT32 gb, UINT32 b);
ERRG_codeE SENSORS_MNGRG_changeResolution(IO_HANDLE sensorHandle, UINT32 width, UINT32 height);
ERRG_codeE SENSORS_MNGRG_exposureControl(IO_HANDLE sensorHandle, void *param);
ERRG_codeE SENSORS_MNGRG_getIspInitParams(IO_HANDLE sensorHandle);
ERRG_codeE SENSORS_MNGRP_setCropWindow(SENSORS_MNGRG_sensorInfoT * sensorInfoP, UINT32 startX, UINT32 startY, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRP_getCropWindow(SENSORS_MNGRG_sensorInfoT * sensorInfoP, UINT32 *startX, UINT32 *startY, INU_DEFSG_sensorContextE context);
ERRG_codeE SENSORS_MNGRP_setCropParams(IO_HANDLE sensorHandle, inu_sensor__setCropParams_t *paramsP);ERRG_codeE SENSORS_MNGRP_powerdown(SENSORS_MNGRG_sensorInfoT     *sensorInfoP);
ERRG_codeE SENSORS_MNGRP_powerup(SENSORS_MNGRG_sensorInfoT     *sensorInfoP);
ERRG_codeE SENSORS_MNGRG_resetSensor(SENSORS_MNGRG_sensorInfoT     *sensorInfoP );
void *SENSORS_MNGRG_findSensorInfo(INU_DEFSG_sensorModelE model);
ERRG_codeE SENSORS_MNGRP_getEepromInfo(SENSORS_MNGRG_sensorInfoT* sensorInfo, inu_device__eepromInfoT *eepromInfo);
#ifdef __cplusplus
}
#endif

#endif //   IAE_MNGR_H


