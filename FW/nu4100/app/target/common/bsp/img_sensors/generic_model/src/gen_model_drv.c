/****************************************************************************
 *
 *   FileName: gen_model_drv.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: genral sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "inu2_internal.h"
#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "gen_model_drv.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include <unistd.h>
#include <errno.h>


/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE GENMODEL_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE GENMODEL_DRVP_close(IO_HANDLE handle);
static ERRG_codeE GENMODEL_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static ERRG_codeE GENMODEL_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE GENMODEL_DRVP_setSensorState(IO_HANDLE handle, void *params);
static ERRG_codeE GENMODEL_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_trigger(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_setExposureMode(IO_HANDLE handle, void *pParams);
static ERRG_codeE GENMODEL_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams);


///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define GEN_MODEL_DRVP_I2C_DEFAULT_ADDRESS                               (0x36)
#define GEN_MODEL_DRVP_I2C_MASTER_ADDRESS                                (GEN_MODEL_DRVP_I2C_DEFAULT_ADDRESS)

///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  GENMODEL_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   GENMODEL_DRVP_deviceDesc[6];
static GEN_SENSOR_DRVG_ioctlFuncListT        GENMODEL_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      GENMODEL_DRVP_resetSensors,            \
                                                                                                      GENMODEL_DRVP_initSensor,              \
                                                                                                      GENMODEL_DRVP_configSensors,           \
                                                                                                      GENMODEL_DRVP_startSensors,            \
                                                                                                      GENMODEL_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,        \
                                                                                                      GENMODEL_DRVP_getDeviceId,             \
                                                                                                      GENMODEL_DRVP_loadPresetTable,         \
                                                                                                      GENMODEL_DRVP_setFrameRate,            \
                                                                                                      GENMODEL_DRVP_setExposureTime,         \
                                                                                                      GENMODEL_DRVP_getExposureTime,         \
                                                                                                      GENMODEL_DRVP_setExposureMode,         \
                                                                                                      GENMODEL_DRVP_setImgOffsets,           \
                                                                                                      GENMODEL_DRVP_getAvgBrighness,         \
                                                                                                      GENMODEL_DRVP_setOutFormat,            \
                                                                                                      GENMODEL_DRVP_setPowerFreq,            \
                                                                                                      GENMODEL_DRVP_setSensorState,          \
                                                                                                      GENMODEL_DRVP_setAutoExposureAvgLuma,  \
                                                                                                      GENMODEL_DRVP_setGain,                 \
                                                                                                      GENMODEL_DRVP_getGain,                 \
                                                                                                      GENMODEL_DRVP_trigger,                 \
                                                                                                      GENMODEL_DRVP_mirrorFlip,              \
                                                                                                      GENMODEL_DRVP_powerup ,                \
                                                                                                      GENMODEL_DRVP_powerdown ,              \
                                                                                                      GENMODEL_DRVP_changeSensorAddress,     \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub,                  \
                                                                                                      GEN_SENSOR_DRVG_stub};

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                               sensorInstanceId  = 0;
   BOOL                                foundNewInstance  = FALSE;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   FIX_UNUSED_PARAM_WARN(deviceId);

   for(sensorInstanceId = 0; sensorInstanceId < 6; sensorInstanceId++)
   {
      deviceDescriptorP = &GENMODEL_DRVP_deviceDesc[sensorInstanceId];
      if(deviceDescriptorP->deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
         foundNewInstance = TRUE;
         break;
      }
   }

   if(foundNewInstance == TRUE)
   {
      deviceDescriptorP = &GENMODEL_DRVP_deviceDesc[sensorInstanceId];
      deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
      deviceDescriptorP->sensorAddress   = GEN_MODEL_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
      deviceDescriptorP->ioctlFuncList   = GENMODEL_DRVP_ioctlFuncList;
      deviceDescriptorP->i2cInstanceId   = pOpenParams->i2cInstanceId;
      deviceDescriptorP->i2cSpeed        = pOpenParams->i2cSpeed;
      deviceDescriptorP->sensorType      = pOpenParams->sensorType;
      deviceDescriptorP->tableType       = pOpenParams->tableType;
      deviceDescriptorP->powerGpioMaster = pOpenParams->powerGpioMaster;
      deviceDescriptorP->fsinGpio        = pOpenParams->fsinGpio;
      deviceDescriptorP->sensorClk       = pOpenParams->sensorClk;
      deviceDescriptorP->groupId         = pOpenParams->groupId;
      deviceDescriptorP->sensorId        = pOpenParams->sensorId;

      *handleP = (IO_HANDLE)deviceDescriptorP;
      LOGG_PRINT(LOG_INFO_E,NULL,"open sensor (type = %d, address = 0x%x, group = %d, sensor = %d)\n",
                 deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,deviceDescriptorP->groupId,deviceDescriptorP->sensorId);
   }
   else
   {
      retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
   }


   return(retCode);
}




/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
***************************************************************************/
static ERRG_codeE GENMODEL_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_POWER_DOWN_SENSOR_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       NULL);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_POWER_UP_SENSOR_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       NULL);
   return retVal;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   return retVal;
}



/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_CONFIG_SENSOR_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       NULL);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_STOP_SENSOR_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       NULL);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
***************************************************************************/
static ERRG_codeE GENMODEL_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                    retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_START_SENSOR_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       NULL);
   return retVal;
}




/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                        retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   inu_fdk__setFrameRateParamsT      fdkFrameRateParams;
   fdkFrameRateParams.frameRate = params->setFrameRateParams.frameRate;
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_SET_FRAME_RATE_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       &fdkFrameRateParams);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                        retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_exposureTimeCfgT  *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   inu_fdk__exposureTimeCfgT         exposureParams;
   exposureParams.exposureTime = params->exposureTime;
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_SET_EXPOSURE_TIME_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       &exposureParams);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODELl_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                    retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_exposureTimeCfgT  *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   inu_fdk__exposureTimeCfgT         exposureParams;
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_GET_EXPOSURE_TIME_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       &exposureParams);

   if (ERRG_SUCCEEDED(retVal))
   {
      params->exposureTime = exposureParams.exposureTime;
   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                        retVal  = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_gainCfgT          *params =(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   inu_fdk__gainCfgT                 gainParams;
   gainParams.analogGain  = params->analogGain;
   gainParams.digitalGain = params->analogGain;
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_SET_GAIN_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       &gainParams);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                    retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_gainCfgT      *params =(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   inu_fdk__gainCfgT             gainParams;
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_GET_GAIN_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       &gainParams);
   if (ERRG_SUCCEEDED(retVal))
   {
      params->analogGain = gainParams.analogGain;
      params->digitalGain = gainParams.digitalGain;
   }
   return retVal;

}


/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                    retVal = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return retVal;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setSensorState
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setSensorState(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_setAutoExposureAvgLuma
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                           retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_TRIGGER_SENSOR_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       NULL);
   return retVal;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   inu_fdk__orientationParamsT         orientation;

   orientation.orientation = params->orientationParams.orientation;
   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_ORIENTATION_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       &orientation);
   return retVal;
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                    retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   retVal = inu_fdk__invokeSensorIoctl(FDK_SENSOR_DRVG_CHANGE_ADDRES_IOCTL_E,
                                       deviceDescriptorP->groupId,
                                       deviceDescriptorP->sensorId,
                                       pParams);
   return retVal;
}



/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;

   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: GENMODEL_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
static ERRG_codeE GENMODEL_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{

   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: GENMODEL_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GENMODEL sensor driver
*
****************************************************************************/
ERRG_codeE GENMODEL_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(GENMODEL_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < 6; sensorInstanceId++)
      {
         deviceDescriptorP = &GENMODEL_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_GENERIC_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      GENMODEL_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &GENMODEL_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &GENMODEL_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &GENMODEL_DRVP_open;
   }
   else
   {
      return SENSOR__ERR_NULL_PTR;
   }

   return SENSOR__RET_SUCCESS;
}


#ifdef __cplusplus
}
#endif

