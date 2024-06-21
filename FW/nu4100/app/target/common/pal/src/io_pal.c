/****************************************************************************
 *
 *   FileName: io_pal.h
 *
 *   Author:  Danny B.
 *
 *   Date: 
 *
 *   Description: Inuitive IO PAL
 *   
 ****************************************************************************/
#include "inu_common.h"
#include "io_pal.h"
#if (DEFSG_PROCESSOR == DEFSG_CEVA)
#include "MM3K_defines.h"
#endif

#ifdef __cplusplus
   extern "C" {
#endif


/****************************************************************************
***************      L O C A L     T Y P E D E F S               ***************
****************************************************************************/


/****************************************************************************
***************      L O C A L     D A T A                        ***************
****************************************************************************/
#if (DEFSG_PROCESSOR == DEFSG_CEVA)
static IO_PALG_deviceDesc IO_PAL_deviceInfoList[IO_NUM_OF_DEVICE_TYPES_E] PRAGMA_DSECT_NO_LOAD("io_pal_dataBase");
#else
static IO_PALG_deviceDesc IO_PAL_deviceInfoList[IO_NUM_OF_DEVICE_TYPES_E];
#endif

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

// GEN SENSOR
ERRG_codeE GEN_SENSOR_DRVG_init(IO_PALG_apiCommandT *palP);
ERRG_codeE I2C_HL_DRVG_init(IO_PALG_apiCommandT *palP);

ERRG_codeE GPIO_DRVG_init(IO_PALG_apiCommandT *palP);

ERRG_codeE PROJ_DRVG_init(IO_PALG_apiCommandT *palP);

ERRG_codeE SPI_FLASH_DRVG_init(IO_PALG_deviceDesc *deviceDscr,IO_PALG_deviceIdE deviceId);

ERRG_codeE MIPI_DRVG_init(IO_PALG_apiCommandT *palP);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
 
static ERRG_codeE IO_PALP_invalidHandleErrorRead(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytesToRead, void *numBytes, void *params)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(bufP);
   FIX_UNUSED_PARAM_WARN(numBytesToRead);
   FIX_UNUSED_PARAM_WARN(numBytes);
   FIX_UNUSED_PARAM_WARN(params);
   return PAL__ERR_INVALID_READ_FUNC;
}

static ERRG_codeE IO_PALP_invalidHandleErrorWrite(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytes, void *params)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(bufP);
   FIX_UNUSED_PARAM_WARN(numBytes);
   FIX_UNUSED_PARAM_WARN(params);
   return PAL__ERR_INVALID_WRITE_FUNC;
}

static ERRG_codeE IO_PALP_invalidHandleErrorIoctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(cmd);
   FIX_UNUSED_PARAM_WARN(argP);
   return PAL__ERR_INVALID_IOCTL_FUNC;
}

static ERRG_codeE IO_PALP_invalidHandleErrorOpen(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   FIX_UNUSED_PARAM_WARN(handleP);
   FIX_UNUSED_PARAM_WARN(deviceId);
   FIX_UNUSED_PARAM_WARN(params);
   return PAL__ERR_INVALID_OPEN_FUNC;
}

static ERRG_codeE IO_PALP_invalidHandleErrorClose(IO_HANDLE handle)
{
   FIX_UNUSED_PARAM_WARN(handle);
   return PAL__ERR_INVALID_CLOSE_FUNC;
}

static ERRG_codeE IO_PALP_invalidDeviceError()
{
   return PAL__ERR_INVALID_DEVICE_ID;
}

static ERRG_codeE IO_PALP_checkHandleValidation(IO_HANDLE handle)
{
   int deviceId;
   ERRG_codeE retVal = PAL__ERR_INVALID_DEVICE_HANDLE;
   for(deviceId = (IO_PALG_deviceIdE)0; deviceId < IO_NUM_OF_DEVICE_TYPES_E; deviceId++)
   {
      if( (&IO_PAL_deviceInfoList[deviceId]) == handle)
      {
         retVal = PAL__RET_SUCCESS;
         break;
      }
   }
   return retVal;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name:  IO_PALG_init
*
*  Description:
*     Initializes all the IO peripherals. 
*  
*  This function is called once during initialization process.
*      
*  Inputs:
*     none.
*     
*  Outputs:
*     none.
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.  
*
****************************************************************************/
ERRG_codeE IO_PALG_init()
{
   UINT8 ind;
   ERRG_codeE retCode = PAL__RET_SUCCESS;

   for(ind = 0; ind < IO_NUM_OF_DEVICE_TYPES_E; ind++)
   {
      IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand.close  = (IO_PALG_closeT)&IO_PALP_invalidHandleErrorClose;
      IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand.open   = (IO_PALG_openT) &IO_PALP_invalidHandleErrorOpen;
      IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand.read   = (IO_PALG_readT) &IO_PALP_invalidHandleErrorRead;
      IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand.write  = (IO_PALG_writeT)&IO_PALP_invalidHandleErrorWrite;
      IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand.ioctl  = (IO_PALG_ioctlT)&IO_PALP_invalidHandleErrorIoctl;
   }

#if (DEFSG_PROCESSOR == DEFSG_GP)
   for(ind = IO_SENSOR_CAM0_E; ind <= IO_SENSOR_CAM23_E; ind++)
   {
        if(ERRG_SUCCEEDED(retCode))
           retCode = GEN_SENSOR_DRVG_init(&(IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand));

   }

   for(ind = IO_I2C_0_E; ind <= IO_I2C_5_E; ind++)
   {
       if(ERRG_SUCCEEDED(retCode))
           retCode = I2C_HL_DRVG_init(&(IO_PAL_deviceInfoList[ind].IO_PALP_apiCommand));
   }
   if(ERRG_SUCCEEDED(retCode))
      retCode = GPIO_DRVG_init(&(IO_PAL_deviceInfoList[IO_GPIO_E].IO_PALP_apiCommand));
   if(ERRG_SUCCEEDED(retCode))
      retCode = PROJ_DRVG_init(&(IO_PAL_deviceInfoList[IO_PROJ_0_E].IO_PALP_apiCommand));
      if(ERRG_SUCCEEDED(retCode))
      retCode = PROJ_DRVG_init(&(IO_PAL_deviceInfoList[IO_PROJ_1_E].IO_PALP_apiCommand));
         if(ERRG_SUCCEEDED(retCode))
      retCode = PROJ_DRVG_init(&(IO_PAL_deviceInfoList[IO_PROJ_2_E].IO_PALP_apiCommand));
   if(ERRG_SUCCEEDED(retCode))
      retCode = SPI_FLASH_DRVG_init((IO_PALG_deviceDesc *)&IO_PAL_deviceInfoList[IO_SPI_FLASH_1_E],IO_SPI_FLASH_1_E);
   if(ERRG_SUCCEEDED(retCode))
      retCode = SPI_FLASH_DRVG_init((IO_PALG_deviceDesc *)&IO_PAL_deviceInfoList[IO_SPI_FLASH_2_E],IO_SPI_FLASH_2_E);
   if(ERRG_SUCCEEDED(retCode))
      retCode = MIPI_DRVG_init(&(IO_PAL_deviceInfoList[IO_MIPI_E].IO_PALP_apiCommand));      
#endif
#if (DEFSG_PROCESSOR == DEFSG_CEVA)
#endif

   return (retCode);
}

/****************************************************************************
*
*  Function Name:  IO_PALG_open
*
*  Description:
*     Open the requested IO connection and return a handle to be used in subsequent connection.
*     
*     This function will block for a timeout (internal) period until the connection has been opened (success) or fails
*     to open in which case a specific error code is returned.
*      
*     This is always the first function to be called for any given connection. Additional opens for the same service 
*     will be ignored and return specific error.  
*     
*  Inputs:
*     connectionId - peripheral connection ID.  
*     
*  Outputs:
*     handleP - On sucsess this is a pointer to a UINT32 to store the handler for this connection. On failure - ignored.
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.  
*
****************************************************************************/
ERRG_codeE IO_PALG_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   IO_HANDLE palP;
   ERRG_codeE retVal;

   if(deviceId < IO_NUM_OF_DEVICE_TYPES_E)
   {
      retVal = IO_PAL_deviceInfoList[deviceId].IO_PALP_apiCommand.open(&palP, deviceId, params); //check retVal
      if(ERRG_SUCCEEDED(retVal))
      {
         IO_PAL_deviceInfoList[deviceId].handle = palP;
         *handleP = (IO_HANDLE *)&(IO_PAL_deviceInfoList[deviceId]);

      }
      else
      {
         *handleP = NULL;
      }
   }
   else
   {
      retVal = IO_PALP_invalidDeviceError();
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: IO_PALG_read
*
*  Description:
*     This function is a wrapper around the GET_BUF ioctl cmd.
*     In blocking mode will block forever until a buffer has been received.
*     In non-blocking mode will return immediately.
*      
*
*  Inputs:
*    handle - connection handle
*     
*  Outputs:
*     numBytesP - Number of bytes read
*     bufP - pointer to buffer with the received data
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single, non-blockable context only in non-blocking mode   
*
****************************************************************************/
ERRG_codeE IO_PALG_read(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytesToRead, void *numBytesP, void *params)
{
   ERRG_codeE retVal = IO_PALP_checkHandleValidation(handle);
   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = ((IO_PALG_deviceDesc *)handle)->IO_PALP_apiCommand.read(((IO_PALG_deviceDesc *)handle)->handle, bufP, numBytesToRead, numBytesP, params);
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: IO_PALG_write
*
*  Description:
*     This function is a wrapper around the PUT_BUF ioctl cmd.
*     In blocking mode will block forever until the buffer has been written.
*     In non-blocking mode will return immediately.
*      
*
*  Inputs:
*    handle - Service handle
*    numBytes - Number of bytes to write
*    bufP - pointer to buffer with data to write
*     
*  Outputs:
*     None
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single, non-blockable context only in non-blocking mode   
*
****************************************************************************/
ERRG_codeE IO_PALG_write(IO_HANDLE handle, UINT8 *bufP, UINT32 numBytes, void *params)
{
   ERRG_codeE retVal = IO_PALP_checkHandleValidation(handle);
   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = ((IO_PALG_deviceDesc *)handle)->IO_PALP_apiCommand.write( ((IO_PALG_deviceDesc *)handle)->handle, bufP, numBytes, params);   
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: IO_PALG_close
*
*  Description:
*     Close the connection associated with the given handle.
*     
*  Inputs:
*     handle - Service handle.
*
*  Outputs:
*     None.
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.  
*
****************************************************************************/
ERRG_codeE IO_PALG_close(IO_HANDLE handle)
{
   ERRG_codeE retVal = IO_PALP_checkHandleValidation(handle);
   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = ((IO_PALG_deviceDesc *)handle)->IO_PALP_apiCommand.close(((IO_PALG_deviceDesc *)handle)->handle);
      ((IO_PALG_deviceDesc *)handle)->handle = NULL;
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: IO_PALG_ioctl
*
*  Description:
*     General commands for mangament of an open connection.
*     This function's behaviour depends on the specific command.
*  Commands are specific for each connection and described at each connection header.
*     
*  Inputs:
*     handle - Service handle (returned in open call).
*     cmd - the command ID - specific to a service
*     argP - pointer to argument supplied by user. If no argument is relevant ignored (may be set to NULL).
*
*  Outputs:
*     None.
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Multiple contexts for all commands.
*     Some commands must be called only from blockable context.  
*
****************************************************************************/
ERRG_codeE IO_PALG_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return ((IO_PALG_deviceDesc *)handle)->IO_PALP_apiCommand.ioctl( ((IO_PALG_deviceDesc *)handle)->handle,cmd, argP);
}

/****************************************************************************
*
*  Function Name: IO_PALG_getHandle
*
*  Description:
*     If the IO is open return handle
*     
*  Inputs:
*     Device ID
*
*  Outputs:
*     handle.
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.  
*
****************************************************************************/
IO_HANDLE IO_PALG_getHandle(IO_PALG_deviceIdE deviceId)
{
   IO_HANDLE handle;

   if(IO_PAL_deviceInfoList[deviceId].handle)
   {
      handle = (IO_HANDLE *)&(IO_PAL_deviceInfoList[deviceId]);
   }
   else
   {
      handle = NULL;
   }

   return(handle);
}

#ifdef __cplusplus
 }
#endif


