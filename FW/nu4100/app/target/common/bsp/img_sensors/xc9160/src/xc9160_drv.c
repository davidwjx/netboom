/****************************************************************************
 *
 *   FileName: XC9160_drv.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: XC9160 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "xc9160_drv.h"
#include "xc9160_cfg_tbl.h"

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

static ERRG_codeE XC9160_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE XC9160_DRVP_close(IO_HANDLE handle);
static ERRG_codeE XC9160_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static ERRG_codeE XC9160_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE XC9160_DRVP_setSensorState(IO_HANDLE handle, void *params);
static ERRG_codeE XC9160_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_trigger(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_setExposureMode(IO_HANDLE handle, void *pParams);
static ERRG_codeE XC9160_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams);


///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define XC9160_DRVP_SC_CHIP_ID                                       (1111)
#define XC9160_DRVP_I2C_XC9160_ADDRESS                               (0x36) //XC9160
#define XC9160_DRVP_I2C_MASTER_ADDRESS                               (XC9160_DRVP_I2C_XC9160_ADDRESS)
#define XC9160_DRVP_I2C_OV13850_ADDRESS                              (0x20) //OV13850

///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////



/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  XC9160_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   XC9160_DRVP_deviceDesc[INU_DEFSG_SENSOR_2_E];
static GEN_SENSOR_DRVG_ioctlFuncListT        XC9160_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      XC9160_DRVP_resetSensors,            \
                                                                                                      XC9160_DRVP_initSensor,              \
                                                                                                      XC9160_DRVP_configSensors,           \
                                                                                                      XC9160_DRVP_startSensors,            \
                                                                                                      XC9160_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      XC9160_DRVP_getDeviceId,             \
                                                                                                      XC9160_DRVP_loadPresetTable,         \
                                                                                                      XC9160_DRVP_setFrameRate,            \
                                                                                                      XC9160_DRVP_setExposureTime,         \
                                                                                                      XC9160_DRVP_getExposureTime,         \
                                                                                                      XC9160_DRVP_setExposureMode,         \
                                                                                                      XC9160_DRVP_setImgOffsets,           \
                                                                                                      XC9160_DRVP_getAvgBrighness,         \
                                                                                                      XC9160_DRVP_setOutFormat,            \
                                                                                                      XC9160_DRVP_setPowerFreq,            \
                                                                                                      XC9160_DRVP_setSensorState,          \
                                                                                                      XC9160_DRVP_setAutoExposureAvgLuma,  \
                                                                                                      XC9160_DRVP_setGain,                 \
                                                                                                      XC9160_DRVP_getGain,                 \
                                                                                                      XC9160_DRVP_trigger,                 \
                                                                                                      XC9160_DRVP_mirrorFlip,              \
                                                                                                      XC9160_DRVP_powerup ,                \
                                                                                                      XC9160_DRVP_powerdown ,              \
                                                                                                      XC9160_DRVP_changeSensorAddress,     \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub};

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: XC9160_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                               sensorInstanceId  = 0; //fixme
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   FIX_UNUSED_PARAM_WARN(deviceId);

   deviceDescriptorP = &XC9160_DRVP_deviceDesc[sensorInstanceId];
   deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
   deviceDescriptorP->sensorAddress   = XC9160_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
   deviceDescriptorP->ioctlFuncList   = XC9160_DRVP_ioctlFuncList;
   deviceDescriptorP->i2cInstanceId   = pOpenParams->i2cInstanceId;
   deviceDescriptorP->i2cSpeed        = pOpenParams->i2cSpeed;
   deviceDescriptorP->sensorType      = pOpenParams->sensorType; //todo remove
   deviceDescriptorP->tableType       = pOpenParams->tableType;
   deviceDescriptorP->powerGpioMaster = pOpenParams->powerGpioMaster;
   deviceDescriptorP->fsinGpio        = pOpenParams->fsinGpio;
   deviceDescriptorP->sensorClk       = pOpenParams->sensorClk;

   *handleP = (IO_HANDLE)deviceDescriptorP;
   LOGG_PRINT(LOG_INFO_E,NULL,"open sensor (type = %d, address = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);

   return(retCode);
}




/****************************************************************************
*
*  Function Name: XC9160_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
***************************************************************************/
static ERRG_codeE XC9160_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return retVal;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_INFO_E,NULL,"Not supported yet!\n");

   return retVal;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              NumOfRegs;
   IO_HANDLE                           *handleP;
   handleP = (IO_HANDLE*)deviceDescriptorP;

   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_INFO_E, NULL, "XC9160_DRVP_loadPresetTable\n");

   LOGG_PRINT(LOG_INFO_E, NULL, "XC9160 Clock Config STARTED!\n");
   deviceDescriptorP->sensorAddress = XC9160_DRVP_I2C_XC9160_ADDRESS;
   NumOfRegs = sizeof(XC9160_ClockConfig_Camera_regs)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)XC9160_ClockConfig_Camera_regs, NumOfRegs);

   return retVal;
}



/****************************************************************************
*
*  Function Name: XC9160_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);

    LOGG_PRINT(LOG_INFO_E, NULL, "GENMODEL_DRVP_configSensors\n");
    return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/

static ERRG_codeE XC9160_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{

   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return retVal;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
***************************************************************************/
static ERRG_codeE XC9160_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT16                              NumOfRegs;
    IO_HANDLE                           *handleP;

    handleP = (IO_HANDLE*)deviceDescriptorP;

    FIX_UNUSED_PARAM_WARN(pParams);

    LOGG_PRINT(LOG_INFO_E, NULL, "XC9160 CONFIG REGS STREAM CAMERA STARTED!\n");
    deviceDescriptorP->sensorAddress = XC9160_DRVP_I2C_XC9160_ADDRESS;
    NumOfRegs = sizeof(XC9160_StreamConfig_Camera_regs)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)XC9160_StreamConfig_Camera_regs, NumOfRegs);

    OS_LYRG_usleep(10*1000); //10 msec

    LOGG_PRINT(LOG_INFO_E, NULL, "XC9160 CONFIG REGS STREAM CAMERA FINISHED!\n");

    LOGG_PRINT(LOG_INFO_E, NULL, "OV13850 CONFIG REGS MIPI REGS STARTED!\n");
    deviceDescriptorP->sensorAddress = XC9160_DRVP_I2C_OV13850_ADDRESS;
    NumOfRegs = sizeof(OV13850mipi_default_regs)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)OV13850mipi_default_regs, NumOfRegs);

    OS_LYRG_usleep(10*1000); //10 msec

    LOGG_PRINT(LOG_INFO_E, NULL, "OV13850 CONFIG REGS MIPI REGS FINISHED!\n");

    LOGG_PRINT(LOG_INFO_E, NULL, "XC9160 CONFIG REGS IQ STARTED!\n");
    deviceDescriptorP->sensorAddress = XC9160_DRVP_I2C_XC9160_ADDRESS;
    NumOfRegs = sizeof(XC9160_IQ_regs)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)XC9160_IQ_regs, NumOfRegs);

    LOGG_PRINT(LOG_INFO_E, NULL, "XC9160_CONFIG_REGS_IQ FINISHED!\n");

    OS_LYRG_usleep(10*1000); //10 msec

    return retVal;
}




/****************************************************************************
*
*  Function Name: XC9160_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_INFO_E, NULL, "NOT Supported yet!\n");
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_INFO_E, NULL, "NOT Supported yet!\n");

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_INFO_E, NULL, "NOT Supported yet!\n");

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setGain(IO_HANDLE handle, void *pParams)
{

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_INFO_E, NULL, "NOT Supported yet!\n");
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   LOGG_PRINT(LOG_INFO_E, NULL, "NOT Supported yet!\n");
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: XC9160_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   return retVal;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: XC9160_DRVP_setSensorState
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setSensorState(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: XC9160_DRVP_setAutoExposureAvgLuma
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;


   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   return retVal;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;

    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);

    return retVal;
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return retVal;
}



/****************************************************************************
*
*  Function Name: XC9160_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;

   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: XC9160_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
static ERRG_codeE XC9160_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{

   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: XC9160_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: XC9160 sensor driver
*
****************************************************************************/
ERRG_codeE XC9160_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(XC9160_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_SENSOR_2_E; sensorInstanceId++)
      {
         deviceDescriptorP = &XC9160_DRVP_deviceDesc[sensorInstanceId];
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
      XC9160_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {

      palP->close =  (IO_PALG_closeT) &XC9160_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &XC9160_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &XC9160_DRVP_open;
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

