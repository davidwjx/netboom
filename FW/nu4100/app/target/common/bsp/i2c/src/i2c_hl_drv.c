/****************************************************************************
 *
 *   FileName: i2c_hl_drv.c
 *
 *   Author:  Danny B.
 *
 *   Date: 
 *
 *   Description: Inuitive IAE Driver
 *   
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#ifdef __cplusplus
      extern "C" {
#endif

#include "io_pal.h"
#include "os_lyr.h"
#include "i2c_hl_drv.h"
#include "i2c_drv.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define I2C_HL_DRVP_MAX_DATA_BURST           (4)
#define I2C_HL_DRVP_3_BYTES_BURST            (3)
#define I2C_HL_DRVP_MAX_ADDRESS_SIZE_BYTES   (2)

#define I2C_HL_DRVP_I2C_DELAY           (5*1000) // 5ms delay

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
   I2C_HL_DRVP_MODULE_STATUS_DUMMY_E = 0,
   I2C_HL_DRVP_MODULE_STATUS_CLOSE_E = 1,
   I2C_HL_DRVP_MODULE_STATUS_OPEN_E  = 2,
   I2C_HL_DRVP_MODULE_STATUS_LAST_E
} I2C_HL_DRVG_moduleStatusE;

typedef ERRG_codeE (*I2C_HL_DRVP_ioctlFuncListT)(IO_HANDLE handle, void *argP);

typedef struct
{
   I2C_HL_DRVG_moduleStatusE     moduleStatus;
   I2C_HL_DRVP_ioctlFuncListT    *ioctlFuncList;
   void                          *deviceBaseAddress;
   I2C_HL_DRVG_instanceIdE       i2cInstanceId;
} I2C_HL_DRVG_deviceDescT;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static I2C_HL_DRVG_deviceDescT       I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_NUM_OF_I2C_INSTANCES];
static I2C_HL_DRVP_ioctlFuncListT    I2C_HL_DRVP_ioctlFuncList[I2C_HL_DRVG_NUM_OF_IOCTLS_E];


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

// general functions
static void I2C_HL_DRVP_dataBaseReset(void);
static void I2C_HL_DRVP_dataBaseInit(void);

// IOCTL commands functions
static ERRG_codeE I2C_HL_DRVP_startI2c(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params);
static ERRG_codeE I2C_HL_DRVP_stopI2c(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params);
static ERRG_codeE I2C_HL_DRVP_getI2cStatus(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params);
static ERRG_codeE I2C_HL_DRVP_reconfigureI2c(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params);

// PAL interface functions
static ERRG_codeE I2C_HL_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, I2C_HL_DRVG_openParamsT *params);
static ERRG_codeE I2C_HL_DRVP_close(IO_HANDLE handle);
static ERRG_codeE I2C_HL_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_dataBaseReset
*
*  Description: reset all I2C_HL_DRVP_deviceDesc structure
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static void I2C_HL_DRVP_dataBaseReset(void)
{
   memset(&I2C_HL_DRVP_deviceDesc, 0, sizeof(I2C_HL_DRVP_deviceDesc));
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_dataBaseInit
*
*  Description: init all I2C_HL_DRVP_dataBase structure for module operation.
*               set module status to close
*               init all ioctl function pointers
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context:
*
****************************************************************************/
static void I2C_HL_DRVP_dataBaseInit(void)
{
   UINT8 i;
   
   for(i = 0; i < I2C_HL_DRVG_NUM_OF_I2C_INSTANCES; i++)
   {
      I2C_HL_DRVP_deviceDesc[i].moduleStatus       = I2C_HL_DRVP_MODULE_STATUS_CLOSE_E;
      I2C_HL_DRVP_deviceDesc[i].deviceBaseAddress  = 0;
      I2C_HL_DRVP_deviceDesc[i].ioctlFuncList      = I2C_HL_DRVP_ioctlFuncList;
   }

   I2C_HL_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(I2C_HL_DRVG_I2C_START_CMD_E)]             = (I2C_HL_DRVP_ioctlFuncListT) I2C_HL_DRVP_startI2c;
   I2C_HL_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(I2C_HL_DRVG_I2C_STOP_CMD_E)]              = (I2C_HL_DRVP_ioctlFuncListT) I2C_HL_DRVP_stopI2c;
   I2C_HL_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(I2C_HL_DRVG_I2C_GET_STATUS_CMD_E)]        = (I2C_HL_DRVP_ioctlFuncListT) I2C_HL_DRVP_getI2cStatus;
   I2C_HL_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(I2C_HL_DRVG_I2C_RECONFIGURE_CMD_E)]       = (I2C_HL_DRVP_ioctlFuncListT) I2C_HL_DRVP_reconfigureI2c;

}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_startI2c
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
static ERRG_codeE I2C_HL_DRVP_startI2c(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params)
{
   FIX_UNUSED_PARAM_WARN(params);
   return (I2C_DRVG_enableI2c(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId));
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_stopI2c
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
static ERRG_codeE I2C_HL_DRVP_stopI2c(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params)
{
   FIX_UNUSED_PARAM_WARN(params);
   return(I2C_DRVG_disableI2c(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId));
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_getI2cStatus
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
static ERRG_codeE I2C_HL_DRVP_getI2cStatus(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params)
{
   FIX_UNUSED_PARAM_WARN(params);
   return(I2C_DRVG_getI2cStatus(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId, params->data));
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_initI2cPeripheral
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
static ERRG_codeE I2C_HL_DRVP_initI2cPeripheral(IO_HANDLE handle, I2C_HL_DRVG_openParamsT *params)
{
   return(I2C_DRVG_open(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId, params->i2cParameters.SlaveAddress, params->i2cRegistersVirtualAddr,params->i2cParameters.i2cSpeed, params->i2cParameters.addressMode));
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_i2cEnable
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
static ERRG_codeE I2C_HL_DRVP_reconfigureI2c(IO_HANDLE handle, I2C_HL_DRVG_i2cParametersT *params)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(params);
/*
   I2C_HL_DRVG_openParamsT openParams;

   openParams.i2cParameters.addressMode   = params->addressMode;
   openParams.i2cParameters.i2cSpeed      = params->i2cSpeed;
   openParams.i2cParameters.SlaveAddress  = params->SlaveAddress;
   return(I2C_HL_DRVP_initI2cPeripheral(handle, &openParams));
*/
   return(I2C__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_open
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
static ERRG_codeE I2C_HL_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, I2C_HL_DRVG_openParamsT *params)
{
   ERRG_codeE              retCode = I2C__RET_SUCCESS;
   I2C_HL_DRVG_instanceIdE i2cDevideId;

   *handleP = (IO_HANDLE)NULL;

   switch(deviceId)
   {
      case IO_I2C_0_E:
         *handleP = (IO_HANDLE)(&I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_0_E]);
         I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_0_E].i2cInstanceId = I2C_HL_DRVG_I2C_INSTANCE_0_E;
         i2cDevideId = I2C_HL_DRVG_I2C_INSTANCE_0_E;
         break;

      case IO_I2C_1_E:
         *handleP = (IO_HANDLE)(&I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_1_E]);
         I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_1_E].i2cInstanceId = I2C_HL_DRVG_I2C_INSTANCE_1_E;
         i2cDevideId = I2C_HL_DRVG_I2C_INSTANCE_1_E;
         break;

      case IO_I2C_2_E:
         *handleP = (IO_HANDLE)(&I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_2_E]);
         I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_2_E].i2cInstanceId = I2C_HL_DRVG_I2C_INSTANCE_2_E;
         i2cDevideId = I2C_HL_DRVG_I2C_INSTANCE_2_E;
         break;

      case IO_I2C_3_E:
         *handleP = (IO_HANDLE)(&I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_3_E]);
         I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_3_E].i2cInstanceId = I2C_HL_DRVG_I2C_INSTANCE_3_E;
         i2cDevideId = I2C_HL_DRVG_I2C_INSTANCE_3_E;
         break;

      case IO_I2C_4_E:
         *handleP = (IO_HANDLE)(&I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_4_E]);
         I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_4_E].i2cInstanceId = I2C_HL_DRVG_I2C_INSTANCE_4_E;
         i2cDevideId = I2C_HL_DRVG_I2C_INSTANCE_4_E;
         break;

      case IO_I2C_5_E:
         *handleP = (IO_HANDLE)(&I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_5_E]);
         I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_5_E].i2cInstanceId = I2C_HL_DRVG_I2C_INSTANCE_5_E;
         i2cDevideId = I2C_HL_DRVG_I2C_INSTANCE_5_E;
         break;


      default:
         retCode = I2C__ERR_OPEN_HL_DRV_FAIL_WRONG_DEVICE_ID;
         break;
   }

   if(ERRG_SUCCEEDED(retCode))
   {
      if(I2C_HL_DRVP_deviceDesc[i2cDevideId].moduleStatus == I2C_HL_DRVP_MODULE_STATUS_CLOSE_E)
      {
         if(params != NULL)
         {
            if(params->i2cRegistersVirtualAddr != 0)
            {
               I2C_HL_DRVP_deviceDesc[i2cDevideId].moduleStatus      = I2C_HL_DRVP_MODULE_STATUS_OPEN_E;
               I2C_HL_DRVP_deviceDesc[i2cDevideId].deviceBaseAddress = params->i2cRegistersVirtualAddr;
               I2C_HL_DRVP_initI2cPeripheral((IO_HANDLE)&I2C_HL_DRVP_deviceDesc[i2cDevideId], params);
            }
            else
            {
               retCode = I2C__ERR_OPEN_FAIL_NULL_PERIPH_ADDRESS;
            }
         }
         else
         {
            retCode = I2C__ERR_OPEN_FAIL_NULL_PARAMS;
         }
      }
      else // if(I2C_HL_DRVP_deviceDesc.moduleStatus == I2C_HL_DRVP_MODULE_STATUS_CLOSE_E)
      {
         retCode = I2C__ERR_OPEN_FAIL_DRV_NOT_CLOSED;
      }
   }

   return (retCode);
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_close
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
static ERRG_codeE I2C_HL_DRVP_close(IO_HANDLE handle)
{
   FIX_UNUSED_PARAM_WARN(handle);
   I2C_HL_DRVP_dataBaseInit();
// I2C_HL_DRVP_iaeResetRegisters();
   return(I2C_DRVG_close(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId));
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_write
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
static ERRG_codeE I2C_HL_DRVP_write(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytes, void *params)
{
   UINT32 *bufDummyP;

   bufDummyP = (UINT32*)bufP;
   if(numBytes > I2C_HL_DRVP_MAX_DATA_BURST)
   {
      return I2C__ERR_ILLEGAL_NUM_OF_BYTES_TO_WRITE;
   }
   else
   {
      return (I2C_DRVG_write(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId,((I2C_HL_DRVG_readWriteParametersT*)params)->regAddress, ((I2C_HL_DRVG_readWriteParametersT*)params)->regAddressAccessSize, numBytes, *bufDummyP, ((I2C_HL_DRVG_readWriteParametersT*)params)->tarAddress,I2C_HL_DRVP_I2C_DELAY));
   }
}

/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_read
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
static ERRG_codeE I2C_HL_DRVP_read(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytesToRead, void *numBytes, void *params)
{
   FIX_UNUSED_PARAM_WARN(numBytes);

   if(numBytesToRead > I2C_HL_DRVP_MAX_DATA_BURST)
   {
      return I2C__ERR_ILLEGAL_NUM_OF_BYTES_TO_READ;
   }
   else
   {
      return (I2C_DRVG_read(((I2C_HL_DRVG_deviceDescT*)handle)->i2cInstanceId, ((I2C_HL_DRVG_readWriteParametersT*)params)->regAddress, ((I2C_HL_DRVG_readWriteParametersT*)params)->regAddressAccessSize, numBytesToRead, bufP, ((I2C_HL_DRVG_readWriteParametersT*)params)->tarAddress,I2C_HL_DRVP_I2C_DELAY));
   }
}


/****************************************************************************
*
*  Function Name: I2C_HL_DRVP_ioctl
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
static ERRG_codeE I2C_HL_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   ERRG_codeE  i2cIoctlRetCode = I2C__RET_SUCCESS;
   UINT16      ioctl;

   if(((I2C_HL_DRVG_deviceDescT*)handle)->moduleStatus == I2C_HL_DRVP_MODULE_STATUS_OPEN_E)
   {
      ioctl = IO_CMD_GET_IOCTL(cmd);
      if (IO_CMD_IS_DEVICE_ID(cmd, IO_I2C_0_E))
      {
         if((handle == &I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_0_E]) || (handle == &I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_1_E]) \
            || (handle == &I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_2_E]) || (handle == &I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_3_E]) \
            || (handle == &I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_4_E]) || (handle == &I2C_HL_DRVP_deviceDesc[I2C_HL_DRVG_I2C_INSTANCE_5_E]))
         {
            i2cIoctlRetCode = (((I2C_HL_DRVG_deviceDescT*)handle))->ioctlFuncList[ioctl](handle, argP);
         }
         else
         {
            i2cIoctlRetCode = I2C__ERR_IOCTL_FAIL_WRONG_HANDLE;
         }
      }
      else  // if (IO_CMD_IS_DEVICE_ID(cmd, IO_I2C_HL_E))
      {
         i2cIoctlRetCode = I2C__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE;
      }
   }
   else  // if(I2C_HL_DRVP_deviceDesc.moduleStatus == I2C_HL_DRVP_MODULE_STATUS_OPEN_E)
   {
      i2cIoctlRetCode = I2C__ERR_IOCTL_FAIL_DRV_NOT_OPEN;
   }

   return(i2cIoctlRetCode);
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: I2C_HL_DRVG_init
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
ERRG_codeE I2C_HL_DRVG_init(IO_PALG_apiCommandT *palP)
{
   ERRG_codeE i2cInitRetCode = I2C__RET_SUCCESS;

   if(palP)
   {
      palP->close =  (IO_PALG_closeT)  &I2C_HL_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT)  &I2C_HL_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)   &I2C_HL_DRVP_open;
      palP->write =  (IO_PALG_writeT)  &I2C_HL_DRVP_write;
      palP->read  =  (IO_PALG_readT)   &I2C_HL_DRVP_read;
   }
   else
   {
      i2cInitRetCode = I2C__ERR_INIT_FAIL_WRONG_PAL_HANDLE;
   }

   if(ERRG_SUCCEEDED(i2cInitRetCode))
   {
      I2C_HL_DRVP_dataBaseReset();
      I2C_HL_DRVP_dataBaseInit();
   }

   return i2cInitRetCode;
}


#ifdef __cplusplus
 }
#endif


