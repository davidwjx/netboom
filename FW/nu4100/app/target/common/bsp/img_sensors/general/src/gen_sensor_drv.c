/****************************************************************************
 *
 *   FileName: gen_sensor_drv.c
 *
 *   Author:  Danny B.
 *
 *   Date: 
 *
 *   Description: General sensor layer
 *   
 ****************************************************************************/
#include "inu_types.h"
#include "log.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "io_pal.h"
#include "err_defs.h"
#include "gen_sensor_drv.h"
#include "gme_mngr.h"
#include "gpio_drv.h"
#include "assert.h"

/****************************************************************************
 ***************      L O C A L        D E F N I T I O N S    **************
 ****************************************************************************/
static ERRG_codeE GEN_SENSOR_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE GEN_SENSOR_DRVP_close(IO_HANDLE handle);
static ERRG_codeE GEN_SENSOR_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);


#define GEN_SENSOR_DRVP_MAX_NUMBER_OF_TRIES (2)
#define GEN_SENSOR_DRVP_I2C_DELAY           (1000) // 1ms delay

//#define GEN_SESNOR_DRVP_DEBUG

/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
   IO_PALG_apiCommandT  sensorApiCommand;
   IO_HANDLE            sensorSpecificHandle;
} GEN_SENSOR_DRVP_deviceDescT;

/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static GEN_SENSOR_DRVP_deviceDescT GEN_SENSOR_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
static ERRG_codeE GEN_SENSOR_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   GEN_SENSOR_DRVG_openParametersT  *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INU_DEFSG_senSelectE             sensorInstanceId  = deviceId - IO_SENSOR_CAM0_E;
   if(pOpenParams != NULL)
   {
      pOpenParams->sensorInitFunc(&(GEN_SENSOR_DRVP_deviceDesc[sensorInstanceId].sensorApiCommand));
      GEN_SENSOR_DRVP_deviceDesc[sensorInstanceId].sensorApiCommand.open(&(GEN_SENSOR_DRVP_deviceDesc[sensorInstanceId].sensorSpecificHandle), deviceId, params);
      *handleP = (IO_HANDLE) &GEN_SENSOR_DRVP_deviceDesc[sensorInstanceId];
   }
   else
   {
      return SENSOR__ERR_NULL_PTR;
   }

   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
static ERRG_codeE GEN_SENSOR_DRVP_close(IO_HANDLE handle)
{
   return (((GEN_SENSOR_DRVP_deviceDescT*)handle)->sensorApiCommand.close(((GEN_SENSOR_DRVP_deviceDescT*)handle)->sensorSpecificHandle));
}

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
static ERRG_codeE GEN_SENSOR_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   UINT32   retVal;
   UINT16   ioctl = IO_CMD_GET_IOCTL(cmd);
   if(IO_CMD_IS_DEVICE_ID(cmd, IO_SENSOR_CAM0_E))
   {
     retVal = (((GEN_SENSOR_DRVP_deviceDescT*)handle)->sensorApiCommand.ioctl(((GEN_SENSOR_DRVP_deviceDescT*)handle)->sensorSpecificHandle, ioctl, argP));
   }
   else
   {
      retVal = SENSOR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_ERROR_E, NULL, "unknown ioctl command. cmd=0x%X\n", cmd);
   }


   return (ERRG_codeE)retVal;
}

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_init(IO_PALG_apiCommandT *palP)
{
   if (palP)
   {
      palP->close = (IO_PALG_closeT) &GEN_SENSOR_DRVP_close;
      palP->ioctl = (IO_PALG_ioctlT) &GEN_SENSOR_DRVP_ioctl;
      palP->open  = (IO_PALG_openT)  &GEN_SENSOR_DRVP_open;
   }
   else
   {
      return SENSOR__ERR_NULL_PTR;
   }
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_accessSensorReg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_ioctlAccessReg(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_sensorParametersT   *params  = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   /*
   *  Warning: 
   *  The cmdCtrl parameter allows accessing the slave handle for external ioctl access.
   *  Do not use this function directly in sensor drivers. 
   *  Use the GEN_SENSOR_DRVG_accessSensorReg() instead.
   */
   if(params->accessRegParams.cmdCtrl >= GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_NUM_CMDS_E)
   {
      return SENSOR__ERR_INVALID_ARGS;
   }
   
   return GEN_SENSOR_DRVG_accessSensorReg(handle, pParams);
}

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_accessSensorReg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_accessSensorReg(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal   = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params  = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   UINT8                               i = 0;

   for (i=0; (i < GEN_SENSOR_DRVP_MAX_NUMBER_OF_TRIES); i++)
   {
      if(params->accessRegParams.rdWr == WRITE_ACCESS_E)
      {
          retVal = (I2C_DRVG_write(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE, params->accessRegParams.accessSize, params->accessRegParams.data, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress,GEN_SENSOR_DRVP_I2C_DELAY));
#ifdef GEN_SESNOR_DRVP_DEBUG
         LOGG_PRINT(LOG_DEBUG_E, NULL, "WRITE to Sensor: 0x%x ; I2c#: %d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
#endif
      }
      else
      {
         params->accessRegParams.data = 0;
         retVal = (I2C_DRVG_read(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE, params->accessRegParams.accessSize, (UINT8*)(&params->accessRegParams.data), ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress,GEN_SENSOR_DRVP_I2C_DELAY));
#ifdef GEN_SESNOR_DRVP_DEBUG
         LOGG_PRINT(LOG_DEBUG_E, NULL, "READ from Sensor: 0x%x ; I2c#: %d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
#endif
      }

      if(ERRG_SUCCEEDED(retVal))
      {
         break;
      }
      else
      {
#ifdef GEN_SESNOR_DRVP_DEBUG
         LOGG_PRINT(LOG_DEBUG_E, retVal, "Failed to write(0)/read(1):%d; Sensor address:0x%X; Reg.:0x%X=0x%X; i2c#:%d\n",params->accessRegParams.rdWr, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, params->accessRegParams.regAdd, params->accessRegParams.data, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId);
#endif
         OS_LYRG_usleep(GEN_SENSOR_DRVG_SENSOR_TABLE_DELAY);
      }
   }

   if(ERRG_FAILED(retVal))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed (ret=0x%08x) to write(0)/read(1):%d (for %d iterations); Sensor address:0x%X; Reg.:0x%X=0x%X; i2c#:%d \n", retVal, params->accessRegParams.rdWr, i, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, params->accessRegParams.regAdd, params->accessRegParams.data, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId);
   }
#ifdef GEN_SESNOR_DRVP_DEBUG
   else
   {
      if(params->accessRegParams.rdWr == WRITE_ACCESS_E)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "WRITE to Sensor: 0x%x ; I2c#: %d ; ctrl=%d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, 
            ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.cmdCtrl, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "READ from Sensor: 0x%x ; I2c#: %d ; ctrl=%d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, 
            ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.cmdCtrl, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
      }
   }
#endif
   return retVal;
}

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_regTableLoad
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_regTableLoad(IO_HANDLE handle, GEN_SENSOR_DRVG_regTblParamsT *tableP, UINT16 regsInTable)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              row;
   UINT8 msDelay;

   for(row = 0; row < regsInTable; row++)
   {
      config.accessRegParams.rdWr         = WRITE_ACCESS_E;
      config.accessRegParams.regAdd       = (UINT16)tableP[row].regAdd;
      config.accessRegParams.accessSize   = tableP[row].accessSizeAndMsecDelay&((1<<GEN_SENSOR_DRVG_DelayOffset)-1);
      config.accessRegParams.data         = (UINT32)tableP[row].data;
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &config);
      if (ERRG_FAILED(retVal))
      {
         return retVal;
      }
      msDelay = tableP[row].accessSizeAndMsecDelay>>GEN_SENSOR_DRVG_DelayOffset;
      if (msDelay)
      {
          OS_LYRG_usleep(msDelay*1000);
      }
  }
   return retVal;
}

/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_regTableRead
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_regTableRead(IO_HANDLE handle, GEN_SENSOR_DRVG_regTblParamsT *tableP, UINT16 regsInTable)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              row;
   UINT8 msDelay;

   for(row = 0; row < regsInTable; row++)
   {
      config.accessRegParams.rdWr         = READ_ACCESS_E;
      config.accessRegParams.regAdd       = (UINT16)tableP[row].regAdd;
      config.accessRegParams.accessSize   = tableP[row].accessSizeAndMsecDelay&((1<<GEN_SENSOR_DRVG_DelayOffset)-1);
      //config.accessRegParams.data         = (UINT32)tableP[row].data;
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &config);
      if (ERRG_FAILED(retVal))
      {
         return retVal;
      }
      msDelay = tableP[row].accessSizeAndMsecDelay>>GEN_SENSOR_DRVG_DelayOffset;
      if (msDelay)
      {
          OS_LYRG_usleep(msDelay*1000);
      }

	  //printf("Reg 0x%04x = 0x%04x\n", config.accessRegParams.regAdd, config.accessRegParams.data);
  }
   return retVal;
}


/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_getRefClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_getRefClk(UINT16 *sys_clkP, GME_DRVG_hwUnitE sensorClk)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GME_DRVG_unitClkDivT                gmeClkConfig;

   gmeClkConfig.unit = sensorClk;   
   retVal = GME_DRVG_getUnitClockDiv(&gmeClkConfig);
   if (ERRG_SUCCEEDED(retVal))
   {
      if (gmeClkConfig.div==0) //divider==0 means no division at all
         gmeClkConfig.div=1;
      *sys_clkP = (24 / gmeClkConfig.div ); //Assuming ref clk is always 24Mhz
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVP_gpioInit
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVP_gpioInit(UINT32 gpioNum)
{
  ERRG_codeE                    retCode           = SENSOR__RET_SUCCESS;
  GPIO_DRVG_gpioOpenParamsT     params;
  GPIO_DRVG_gpioSetDirParamsT   directionParams; 
  params.gpioNum = gpioNum;
  retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_OPEN_GPIO_CMD_E, &params);
  //setting direction
  if(ERRG_SUCCEEDED(retCode))
  {
     LOGG_PRINT(LOG_DEBUG_E, NULL, "GPIO: Succeed to open GPIO %d\n", params.gpioNum);
     directionParams.direction = GPIO_DRVG_OUT_DIRECTION_E;
     directionParams.gpioNum   = gpioNum;
     retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
     LOGG_PRINT(LOG_DEBUG_E, NULL, "GPIO: Set GPIO %d direction to %d\n", directionParams.gpioNum, directionParams.direction);
  }
  else
  {
     LOGG_PRINT(LOG_ERROR_E, NULL, "GPIO: Fail to open GPIO %d\n", params.gpioNum);
  }
  return retCode;

}


/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVP_setSensorRefClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVP_setSensorRefClk(INT32 clk, UINT16 div)
{

   ERRG_codeE                          retVal           = SENSOR__RET_SUCCESS;
   GME_DRVG_unitClkDivT                gmeClkConfig;
   gmeClkConfig.div  = div;
   gmeClkConfig.unit = clk;
   retVal = GME_DRVG_changeUnitFreq(&gmeClkConfig);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_getSensorPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
UINT16 GEN_SENSOR_DRVG_getSensorPixelClk(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->pixClk)
   {
      printf("invalid pixel clk for handle %p (model %d)\n",handle,deviceDescriptorP->sensorModel);
      assert(0);
   }

   printf("pixel clk %d (%p)\n",deviceDescriptorP->pixClk,deviceDescriptorP);
   return deviceDescriptorP->pixClk;
}

void *GEN_SENSOR_DRVG_getSensorSpecificHandle(IO_HANDLE sensorHandle)
{
    IO_PALG_deviceDesc *handle = ((IO_PALG_deviceDesc *)sensorHandle)->handle;

    return (((GEN_SENSOR_DRVP_deviceDescT*)handle)->sensorSpecificHandle);
}


/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_accessSensorReg
*
*  Description:rw address 8 register sensor corsair
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_accessSensorReg8(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal   = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params  = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   UINT8                               i = 0;

   for (i=0; (i < GEN_SENSOR_DRVP_MAX_NUMBER_OF_TRIES); i++)
   {
      if(params->accessRegParams.rdWr == WRITE_ACCESS_E)
      {
          retVal = (I2C_DRVG_write(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE8, params->accessRegParams.accessSize, params->accessRegParams.data, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress,GEN_SENSOR_DRVP_I2C_DELAY));
#ifdef GEN_SESNOR_DRVP_DEBUG
         LOGG_PRINT(LOG_DEBUG_E, NULL, "WRITE to Sensor: 0x%x ; I2c#: %d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
#endif
      }
      else
      {
         params->accessRegParams.data = 0;
         retVal = (I2C_DRVG_read(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE8, params->accessRegParams.accessSize, (UINT8*)(&params->accessRegParams.data), ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress,GEN_SENSOR_DRVP_I2C_DELAY));
#ifdef GEN_SESNOR_DRVP_DEBUG
         LOGG_PRINT(LOG_DEBUG_E, NULL, "READ from Sensor: 0x%x ; I2c#: %d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
#endif
      }

      if(ERRG_SUCCEEDED(retVal))
      {
         break;
      }
      else
      {
#ifdef GEN_SESNOR_DRVP_DEBUG
         LOGG_PRINT(LOG_DEBUG_E, retVal, "Failed to write(0)/read(1):%d; Sensor address:0x%X; Reg.:0x%X=0x%X; i2c#:%d\n",params->accessRegParams.rdWr, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, params->accessRegParams.regAdd, params->accessRegParams.data, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId);
#endif
         OS_LYRG_usleep(GEN_SENSOR_DRVG_SENSOR_TABLE_DELAY);
      }
   }

   if(ERRG_FAILED(retVal))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Failed (ret=0x%08x) to write(0)/read(1):%d (for %d iterations); Sensor address:0x%X; Reg.:0x%X=0x%X; i2c#:%d \n", retVal, params->accessRegParams.rdWr, i, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, params->accessRegParams.regAdd, params->accessRegParams.data, ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId);
   }
#ifdef GEN_SESNOR_DRVP_DEBUG
   else
   {
      if(params->accessRegParams.rdWr == WRITE_ACCESS_E)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "WRITE to Sensor: 0x%x ; I2c#: %d ; ctrl=%d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, 
            ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.cmdCtrl, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "READ from Sensor: 0x%x ; I2c#: %d ; ctrl=%d ; Reg.:0x%x = 0x%x (dataLen: %d)\n", ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->sensorAddress, 
            ((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->i2cInstanceId, params->accessRegParams.cmdCtrl, params->accessRegParams.regAdd, params->accessRegParams.data, params->accessRegParams.accessSize);
      }
   }
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: GEN_SENSOR_DRVG_regTableLoad
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: generic sensor layer
*
****************************************************************************/
ERRG_codeE GEN_SENSOR_DRVG_regTableLoad8(IO_HANDLE handle, GEN_SENSOR_DRVG_regTblParamsT *tableP, UINT16 regsInTable)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              row;

   UINT8 msDelay;

   for(row = 0; row < regsInTable; row++)
   {
      config.accessRegParams.rdWr         = WRITE_ACCESS_E;
      config.accessRegParams.regAdd       = (UINT16)tableP[row].regAdd;
      config.accessRegParams.accessSize   = tableP[row].accessSizeAndMsecDelay&((1<<GEN_SENSOR_DRVG_DelayOffset)-1);
      config.accessRegParams.data         = (UINT32)tableP[row].data;
      retVal = GEN_SENSOR_DRVG_accessSensorReg8(handle, &config);
      if (ERRG_FAILED(retVal))
      {
         return retVal;
      }
      msDelay = tableP[row].accessSizeAndMsecDelay>>GEN_SENSOR_DRVG_DelayOffset;
      if (msDelay)
      {
          OS_LYRG_usleep(msDelay*1000);
      }
  }
   return retVal;
}



/****general stub******/
ERRG_codeE GEN_SENSOR_DRVG_stub(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_sensorParametersT   *params  = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

  
   if(params->accessRegParams.cmdCtrl >= GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_NUM_CMDS_E)
   {
      return SENSOR__ERR_INVALID_ARGS;
   }
   
   // this function is a stub , the feature is not supported in this sensor!
   return SENSOR__ERR_NOT_SUPPORTED;
}


#ifdef __cplusplus
}
#endif


