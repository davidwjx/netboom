/****************************************************************************
 *
 *   FileName: i2c_drv.c
 *
 *   Author:  Danny B.
 *
 *   Date: 
 *
 *   Description: Aptina sensor driver
 *   
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "i2c_drv.h"
#include "os_lyr.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#ifdef __KERNEL_I2C__
//#define I2C_TEST
//#define I2C_DEBUG
#endif


#ifdef __KERNEL_I2C__
#include <errno.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

#define I2C_DRVP_MAX_ITERATIONS           (5)

#define I2C_DRVP_NUM_OF_INIT_RETRIES      (10)
#define I2C_DRVP_INIT_SLEEP               (100)

// The input clocks used otherwise.
#define I2C_DRVP_I2C_CLOCK                (165)

/*
 * These minimum high and low times are in nanoseconds.  They represent
 * the minimum amount of time a bus signal must remain either high or
 * low to be interpreted as a logical high or low as per the I2C bus
 * protocol.  These values are used in conjunction with an I2C input
 * clock frequency to determine the correct values to be written to the
 * clock count registers.
 */
#define I2C_DRVP_SS_MIN_SCL_HIGH          (4000)
#define I2C_DRVP_SS_MIN_SCL_LOW           (4700)
#define I2C_DRVP_FS_MIN_SCL_HIGH          (600)
#define I2C_DRVP_FS_MIN_SCL_LOW           (1300)
#define I2C_DRVP_HS_MIN_SCL_HIGH_100PF    (60)
#define I2C_DRVP_HS_MIN_SCL_LOW_100PF     (120)

#define I2C_DRVP_NO_STOP                  (0)
#define I2C_DRVP_ISSUE_STOP               (1)

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct {
   struct dw_device            i2cDev;
   struct dw_i2c_instance      i2cInstance;
   struct dw_i2c_param         i2c_param;
#ifdef __KERNEL_I2C__
   int fd;
#endif
} I2C_DRVP_i2cDeviceDescT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   I2C_DRVP_i2cDeviceDescT    I2C_DRVP_i2cDeviceDesc[I2C_DRVG_NUM_OF_I2C_INSTANCES];
#ifndef __KERNEL_I2C__
static   OS_LYRG_mutexT             I2C_DRVP_localMutex;
static   UINT8                      I2C_DRVP_moduleAccessCount = 0;
#endif
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

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: I2C_DRVP_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
static ERRG_codeE I2C_DRVP_init(I2C_DRVP_i2cDeviceDescT *dev, void* i2cInstanceAddress)
{
   // I2C 1 device structure setup.
   dev->i2cDev.name         = "i2c";
   dev->i2cDev.comp_type    = Dw_apb_i2c;
   dev->i2cDev.data_width   = BYTES_IN_DWORD;
   dev->i2cDev.base_address = i2cInstanceAddress;
   dev->i2cDev.comp_param   = &(dev->i2c_param);
   dev->i2cDev.instance     = &(dev->i2cInstance);
   dev->i2cDev.os           = NULL;

   return I2C__RET_SUCCESS;
}

#ifndef __KERNEL_I2C__
/****************************************************************************
*
*  Function Name: I2C_DRVP_clockSetup
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
static ERRG_codeE I2C_DRVP_clockSetup(I2C_DRVG_instanceIdE instanceId, unsigned ic_clk)
{
   uint16_t ss_scl_high, ss_scl_low;
   uint16_t fs_scl_high, fs_scl_low;
   uint16_t hs_scl_high, hs_scl_low;

   // ic_clk is the clock speed (in MHz) that is being supplied to the
   // DW_apb_i2c device.  The correct clock count values are determined
   // by using this inconjunction with the minimum high and low signal
   // hold times as per the I2C bus specification.
   ss_scl_high = ((uint16_t) (((I2C_DRVP_SS_MIN_SCL_HIGH * ic_clk) / 1000) + 1));
   ss_scl_low = ((uint16_t) (((I2C_DRVP_SS_MIN_SCL_LOW * ic_clk) / 1000) + 1));
   fs_scl_high = ((uint16_t) (((I2C_DRVP_FS_MIN_SCL_HIGH * ic_clk) / 1000) + 1));
   fs_scl_low = ((uint16_t) (((I2C_DRVP_FS_MIN_SCL_LOW * ic_clk) / 1000) + 1));
   hs_scl_high = ((uint16_t) (((I2C_DRVP_HS_MIN_SCL_HIGH_100PF * ic_clk) / 1000) + 1));
   hs_scl_low = ((uint16_t) (((I2C_DRVP_HS_MIN_SCL_LOW_100PF * ic_clk) / 1000) + 1));

   dw_i2c_setSclCount(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_speed_standard, I2c_scl_high, ss_scl_high);
   dw_i2c_setSclCount(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_speed_standard, I2c_scl_low,  ss_scl_low);
   dw_i2c_setSclCount(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_speed_fast, I2c_scl_high, fs_scl_high);
   dw_i2c_setSclCount(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_speed_fast, I2c_scl_low, fs_scl_low);
   dw_i2c_setSclCount(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_speed_high, I2c_scl_high, hs_scl_high);
   dw_i2c_setSclCount(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_speed_high, I2c_scl_low, hs_scl_low);

//   dw_i2c_setSpkLen(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), 16);
   dw_i2c_setSdaSetupHoldTime(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev),100, 60);

   return I2C__RET_SUCCESS;
}
#endif

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: I2C_DRVG_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
ERRG_codeE I2C_DRVG_open(I2C_DRVG_instanceIdE instanceId, UINT16 targetAddress, void* i2cInstanceAddress, enum dw_i2c_speed_mode speed,  enum dw_i2c_address_mode addresingMode)
{
   ERRG_codeE  retVal = I2C__RET_SUCCESS;

#ifdef __KERNEL_I2C__

   FIX_UNUSED_PARAM_WARN(targetAddress);
   FIX_UNUSED_PARAM_WARN(speed);
   FIX_UNUSED_PARAM_WARN(addresingMode);

   if (I2C_DRVP_i2cDeviceDesc[instanceId].fd <= 0)
   {

      I2C_DRVP_init(&I2C_DRVP_i2cDeviceDesc[instanceId], i2cInstanceAddress);

      switch(instanceId)
      {
         case(I2C_DRVG_I2C_INSTANCE_0_E):
         {
            I2C_DRVP_i2cDeviceDesc[instanceId].fd = open( "/dev/i2c-0", O_RDWR );
         }break;

         case(I2C_DRVG_I2C_INSTANCE_1_E):
         {
            I2C_DRVP_i2cDeviceDesc[instanceId].fd = open( "/dev/i2c-1", O_RDWR );
         }break;

         case(I2C_DRVG_I2C_INSTANCE_2_E):
         {
            I2C_DRVP_i2cDeviceDesc[instanceId].fd = open( "/dev/i2c-2", O_RDWR );
         }break;

         case(I2C_DRVG_I2C_INSTANCE_3_E):
         {
            I2C_DRVP_i2cDeviceDesc[instanceId].fd = open( "/dev/i2c-3", O_RDWR );
         }break;

         case(I2C_DRVG_I2C_INSTANCE_4_E):
         {
            I2C_DRVP_i2cDeviceDesc[instanceId].fd = open( "/dev/i2c-4", O_RDWR );
         }break;

         case(I2C_DRVG_I2C_INSTANCE_5_E):
         {
            I2C_DRVP_i2cDeviceDesc[instanceId].fd = open( "/dev/i2c-5", O_RDWR );
         }break;

         default:
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid I2C instance\n");
            return I2C__ERR_INIT_FAIL;
         }
      }


      if (I2C_DRVP_i2cDeviceDesc[instanceId].fd < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "fail to open i2c errno = %s\n",strerror(errno));
         return I2C__ERR_INIT_FAIL;
      }

      LOGG_PRINT(LOG_DEBUG_E, NULL, "Success to open i2c %d, fd = %d\n", instanceId, I2C_DRVP_i2cDeviceDesc[instanceId].fd);
   }

   return retVal;

#else
   UINT8       numOfIterations = 0;

   I2C_DRVP_moduleAccessCount++;

   if(I2C_DRVP_moduleAccessCount == 1)
   {
      OS_LYRG_aquireMutex(&I2C_DRVP_localMutex);
   }

   OS_LYRG_lockMutex(&I2C_DRVP_localMutex);
   
   I2C_DRVP_init(&I2C_DRVP_i2cDeviceDesc[instanceId], i2cInstanceAddress);

   dw_i2c_autoCompParams(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));

   // initialize the I2C device driver
   while(dw_i2c_init(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)))
   {
      numOfIterations++;
      if(numOfIterations >= I2C_DRVP_NUM_OF_INIT_RETRIES)
      {
         retVal = I2C__ERR_INIT_FAIL;
         break;
      }
      OS_LYRG_usleep(I2C_DRVP_INIT_SLEEP);
   }

   // Set up the clock count register.  The argument I2C1_CLOCK is
   // specified as the I2C master input clock.
   I2C_DRVP_clockSetup(instanceId, I2C_DRVP_I2C_CLOCK);

   // set the speed mode to standard
   dw_i2c_setSpeedMode(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), speed);
   // use 7-bit addressing
   dw_i2c_setMasterAddressMode(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), addresingMode);
   // enable master FSM
   dw_i2c_enableMaster(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));
   // use target address when initiating transfer
   dw_i2c_setTxMode(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), I2c_tx_target);
   // set target address to the I2C slave address
   dw_i2c_setTargetAddress(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), targetAddress>>1);
   // enable master restert state
   dw_i2c_enableRestart(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));
   // enable the master I2C device
   dw_i2c_enable(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));

   OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
#endif

   return I2C__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: I2C_DRVG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
ERRG_codeE I2C_DRVG_close(I2C_DRVG_instanceIdE instanceId)
{
   ERRG_codeE  retVal = I2C__RET_SUCCESS;

#ifdef __KERNEL_I2C__

   if(I2C_DRVP_i2cDeviceDesc[instanceId].fd > 0)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "close i2c %d, fd-%d\n" ,instanceId, I2C_DRVP_i2cDeviceDesc[instanceId].fd);
      close(I2C_DRVP_i2cDeviceDesc[instanceId].fd);
      I2C_DRVP_i2cDeviceDesc[instanceId].fd = -1;
   }
#else
   UINT8       numOfIterations = 0;
   
   OS_LYRG_lockMutex(&I2C_DRVP_localMutex);

   while(dw_i2c_init(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)))
   {
      numOfIterations++;
      if(numOfIterations >= I2C_DRVP_NUM_OF_INIT_RETRIES)
      {
         retVal = I2C__ERR_INIT_FAIL;
         break;
      }
      OS_LYRG_usleep(I2C_DRVP_INIT_SLEEP);
   }

   OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
#endif
   return(retVal);
}

#ifndef __KERNEL_I2C__
/****************************************************************************
*
*  Function Name: I2C_DRVG_writeByte
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
static UINT32 I2C_DRVP_writeByte(I2C_DRVG_instanceIdE instanceId, UINT8 data, UINT8 stop, UINT32 usDelay)
{
   enum dw_i2c_tx_abort ret;
   UINT32   iteration = 0;

   
   while(dw_i2c_isTxFifoFull(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)) && (iteration++ < I2C_DRVP_MAX_ITERATIONS))
   {
      OS_LYRG_usleep(usDelay);
   }

   if(iteration >= I2C_DRVP_MAX_ITERATIONS)
   {
      return I2C__ERR_WRITE_I2C_BUSY;
   }

   dw_i2c_write(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), data, stop);
   iteration = 0;
   while(!dw_i2c_isTxFifoEmpty(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)) && (iteration++ < I2C_DRVP_MAX_ITERATIONS))
   {
      OS_LYRG_usleep(usDelay);
   }
   if(iteration >= I2C_DRVP_MAX_ITERATIONS)
   {
      return I2C__ERR_WRITE_I2C_BUSY;
   }

   ret = dw_i2c_getTxAbortSource(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));

   //LOGG_PRINT(LOG_DEBUG_E,NULL,"IC_TX_ABRT_SOURCE: 0x%x\n", ret);
//   printf("IC_TX_ABRT_SOURCE: 0x%x\n", ret);
   
   if((ret & I2c_abrt_7b_addr_noack) || (ret & I2c_abrt_txdata_noack))
   {
      dw_i2c_clearIrq(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev),I2c_irq_tx_abrt);
      return I2C__ERR_NO_ACK;
   }
   else
   {
      return I2C__RET_SUCCESS;
   }  
}

/****************************************************************************
*
*  Function Name: I2C_DRVG_requestReadByte
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
static void I2C_DRVP_requestReadByte(I2C_DRVG_instanceIdE instanceId, UINT8 stop)
{
   dw_i2c_issueRead(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), stop);
}

/****************************************************************************
*
*  Function Name: I2C_DRVG_readByte
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
static ERRG_codeE I2C_DRVP_readByte(I2C_DRVG_instanceIdE instanceId, UINT8 *data, UINT32 uSdelay)
{
   UINT32   iteration = 0;

   while((dw_i2c_isRxFifoEmpty(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)) && (iteration++ < I2C_DRVP_MAX_ITERATIONS)))
   {
      OS_LYRG_usleep(uSdelay);
   }

   if(iteration >= I2C_DRVP_MAX_ITERATIONS)
   {
      return I2C__ERR_READ_I2C_BUSY;
   }

   *data = dw_i2c_read(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));

   return I2C__RET_SUCCESS;
}
#endif

/****************************************************************************
*
*  Function Name: I2C_DRVG_updateSlaveAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
static ERRG_codeE I2C_DRVP_updateSlaveAddress(I2C_DRVG_instanceIdE instanceId, UINT16 address, UINT32 uSdelay)
{
#ifdef __KERNEL_I2C__
   ERRG_codeE ret = I2C__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(uSdelay);

   if( ioctl(I2C_DRVP_i2cDeviceDesc[instanceId].fd, I2C_SLAVE_FORCE, address ) < 0 )
   {
       LOGG_PRINT(LOG_ERROR_E, NULL, "fail to ioctl i2c, address = %x, errno = %s, fd-%d\n", address, strerror(errno), I2C_DRVP_i2cDeviceDesc[instanceId].fd);
       ret = I2C__ERR_INIT_FAIL;
   } 

   return ret;
#else
   UINT32 iteration = 0;

   while((dw_i2c_isBusy(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)) || !dw_i2c_isTxFifoEmpty(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev))) && (iteration++ < I2C_DRVP_MAX_ITERATIONS))
   {
      OS_LYRG_usleep(uSdelay);
   }

   if(iteration >= I2C_DRVP_MAX_ITERATIONS)
   {
      return I2C__ERR_UPDATE_ADDRESS_I2C_BUSY;
   }

   dw_i2c_disable(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));
   dw_i2c_setTargetAddress(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev), address>>1);
   dw_i2c_enable(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));

   return I2C__RET_SUCCESS;
#endif   
}

/****************************************************************************
*
*  Function Name: I2C_DRVG_write
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
ERRG_codeE I2C_DRVG_write(I2C_DRVG_instanceIdE instanceId, UINT32 regAddress, UINT8 regAddressAccessSize, UINT8 dataAccessSize, UINT32 data, UINT16 tarAddress, UINT32 uSdelay)
{
   ERRG_codeE retVal = I2C__RET_SUCCESS;

#ifdef __KERNEL_I2C__
   int size;
   UINT8 buf[10] = {0};

   tarAddress = tarAddress >> 1;

   retVal = I2C_DRVP_updateSlaveAddress(instanceId, tarAddress, uSdelay);

   if(ERRG_FAILED(retVal))
   {
      return I2C__ERR_INIT_FAIL;
   }

   /* write address + register */
   switch (regAddressAccessSize)
   {
      case(4):
      {
         buf[0] = ((regAddress >> 24) & 0xFF);
         buf[1] = ((regAddress >> 16) & 0xFF);
         buf[2] = ((regAddress >> 8) & 0xFF);
         buf[3] = ((regAddress >> 0) & 0xFF);
      }break;

      case(3):
      {
         buf[0] = ((regAddress >> 16) & 0xFF);
         buf[1] = ((regAddress >> 8) & 0xFF);
         buf[2] = ((regAddress >> 0) & 0xFF);         
      }break;

      case(2):
      {
         buf[0] = ((regAddress >> 8) & 0xFF);
         buf[1] = ((regAddress >> 0) & 0xFF);
      }break;

      case(1):
      {
         buf[0] = ((regAddress >> 0) & 0xFF);
      }break;

      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "regAddressAccessSize not supported, regAddressAccessSize = %d\n",regAddressAccessSize);
         return I2C__ERR_WRITE_I2C_ABORT;         
      }
   }


   switch (dataAccessSize)
   {
      case(4):
      {
         buf[regAddressAccessSize + 0] = ((data >> 24) & 0xFF);
         buf[regAddressAccessSize + 1] = ((data >> 16) & 0xFF);
         buf[regAddressAccessSize + 2] = ((data >> 8) & 0xFF);
         buf[regAddressAccessSize + 3] = ((data >> 0) & 0xFF);
      }break;

      case(3):
      {
         buf[regAddressAccessSize + 0] = ((data >> 16) & 0xFF);
         buf[regAddressAccessSize + 1] = ((data >> 8) & 0xFF);
         buf[regAddressAccessSize + 2] = ((data >> 0) & 0xFF);         
      }break;

      case(2):
      {
         buf[regAddressAccessSize + 0] = ((data >> 8) & 0xFF);
         buf[regAddressAccessSize + 1] = ((data >> 0) & 0xFF);
      }break;

      case(1):
      {
         buf[regAddressAccessSize + 0] = ((data >> 0) & 0xFF);
      }break;

      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "dataAccessSize not supported, dataAccessSize = %d\n",dataAccessSize);
         return I2C__ERR_WRITE_I2C_ABORT;         
      }
   }

   size = write(I2C_DRVP_i2cDeviceDesc[instanceId].fd, buf , (regAddressAccessSize + dataAccessSize));
   
   if (size != (regAddressAccessSize + dataAccessSize))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to write i2c,instanceId %x tarAddress %x regAddress = 0x%x, regAddressAccessSize = %d, data = 0x%x, dataAccessSize = %d size = %d, errno = %s\n",instanceId, tarAddress,regAddress,regAddressAccessSize,data,dataAccessSize,size,strerror(errno));
      return I2C__ERR_READ_FAIL_TO_RECEIVE_ALL_DATA;
   }
   else
   {
#ifdef I2C_DEBUG
      LOGG_PRINT(LOG_ERROR_E, NULL, "write success! instanceId %d tarAddress = 0x%x, regAddressAccessSize = %d, dataAccessSize = %d, size = %d, data = %x,%x\n",instanceId, tarAddress,regAddressAccessSize,dataAccessSize,size,buf[0],buf[1]);
#endif
   }

#ifdef I2C_DEBUG
   {
      char temp[30];

      tarAddress = tarAddress << 1;
      I2C_DRVG_read(instanceId,regAddress,regAddressAccessSize,dataAccessSize,(UINT8*)temp,tarAddress,uSdelay);
   }
#endif

//   OS_LYRG_usleep(uSdelay);

   return retVal;

#else
   UINT8    currentAddress;

   OS_LYRG_lockMutex(&I2C_DRVP_localMutex);

   currentAddress = ((UINT8)dw_i2c_getTargetAddress(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)))<<1;

   if(currentAddress != tarAddress)
   {
      retVal = I2C_DRVP_updateSlaveAddress(instanceId, tarAddress, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }

   if(regAddressAccessSize > 1)
   {
      retVal = I2C_DRVP_writeByte(instanceId, SHIFT_RIGHT(regAddress,BITS_IN_BYTE), I2C_DRVP_NO_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }
   
   retVal = I2C_DRVP_writeByte(instanceId, regAddress, I2C_DRVP_NO_STOP, uSdelay);
   if(ERRG_FAILED(retVal))
   {
      OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
      return retVal;
   }

   if(dataAccessSize == 1)
   {
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)data, I2C_DRVP_ISSUE_STOP, uSdelay);      
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }
   else if(dataAccessSize == 4)
   {
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)SHIFT_RIGHT(data, BITS_IN_BYTE*3), I2C_DRVP_NO_STOP, uSdelay);      
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)SHIFT_RIGHT(data, BITS_IN_BYTE*2), I2C_DRVP_NO_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)SHIFT_RIGHT(data, BITS_IN_BYTE*1), I2C_DRVP_NO_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)data, I2C_DRVP_ISSUE_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }
   else
   {
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)SHIFT_RIGHT(data, BITS_IN_BYTE*1), I2C_DRVP_NO_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
      retVal = I2C_DRVP_writeByte(instanceId, (UINT8)data, I2C_DRVP_ISSUE_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }

   OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
#endif

   return I2C__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: I2C_DRVG_read
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
ERRG_codeE I2C_DRVG_read(I2C_DRVG_instanceIdE instanceId, UINT32 regAddress, UINT8 regAddressAccessSize, UINT8 dataAccessSize, UINT8 *data, UINT16 tarAddress, UINT32 uSdelay)
{
#ifdef __KERNEL_I2C__
   ERRG_codeE  retVal = I2C__RET_SUCCESS;
   INT32 nmsgs=0;
   UINT8 buf[10];
   memset(buf,0,sizeof(buf));
   struct i2c_rdwr_ioctl_data packets;
   struct i2c_msg messages[2];

   tarAddress = tarAddress >> 1;
   retVal = I2C_DRVP_updateSlaveAddress(instanceId, tarAddress, uSdelay);

   if(ERRG_FAILED(retVal))
   {
      return I2C__ERR_INIT_FAIL;
   }

   switch (regAddressAccessSize)
   {
      case(4):
      {
         buf[0] = ((regAddress >> 24) & 0xFF);
         buf[1] = ((regAddress >> 16) & 0xFF);
         buf[2] = ((regAddress >> 8) & 0xFF);
         buf[3] = ((regAddress >> 0) & 0xFF);
      }break;

      case(3):
      {
         buf[0] = ((regAddress >> 16) & 0xFF);
         buf[1] = ((regAddress >> 8) & 0xFF);
         buf[2] = ((regAddress >> 0) & 0xFF);
      }break;

      case(2):
      {
         buf[0] = ((regAddress >> 8) & 0xFF);
         buf[1] = ((regAddress >> 0) & 0xFF);
      }break;

      case(1):
      {
         buf[0] = ((regAddress >> 0) & 0xFF);
      }break;

      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "regAddressAccessSize not supported, regAddressAccessSize = %d\n",regAddressAccessSize);
         return I2C__ERR_WRITE_I2C_ABORT;
      }
   }

/*
 In order to read a register, we first do a "dummy write" by writing
  * 0 bytes to the register we want to read from.  This is similar to
  * the packet in set_i2c_register, except it's 1 byte rather than 2.
*/
   messages[0].addr  = tarAddress;
   messages[0].flags = 0;
   messages[0].len   = regAddressAccessSize;
   messages[0].buf   = buf;

   /* The data will get returned in this structure */
   messages[1].addr  = tarAddress;
   messages[1].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
   messages[1].len   = dataAccessSize;
   messages[1].buf   = buf;
   /* Send the request to the kernel and get the result back */
   packets.msgs      = messages;
   packets.nmsgs     = 2;

   nmsgs=ioctl(I2C_DRVP_i2cDeviceDesc[instanceId].fd, I2C_RDWR, &packets);
   if(nmsgs<0) 
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to ioctl i2c%d, address = %x, errno = %s, fd-%d\n",instanceId, regAddress, strerror(errno), I2C_DRVP_i2cDeviceDesc[instanceId].fd);
      retVal = I2C__ERR_INIT_FAIL;
   }

   if (packets.nmsgs != (UINT32)nmsgs)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to read i2c, nmsgs = %d\n",nmsgs);
   }
   else
   {
#ifdef I2C_DEBUG
      LOGG_PRINT(LOG_ERROR_E, NULL, "read success! instanceId = %d, tarAddress = 0x%x, regAddres = 0x%x, nmsgs = %d, %x,%x,%x,%x\n",instanceId,tarAddress, regAddress,nmsgs,buf[0],buf[1],buf[2],buf[3]);
#endif

      switch (dataAccessSize)
      {
         case(4):
         {
            data[0] = buf[3];
            data[1] = buf[2];
            data[2] = buf[1];
            data[3] = buf[0];
         }break;

         case(3):
         {
            data[0] = buf[2];
            data[1] = buf[1];
            data[2] = buf[0];
         }break;

         case(2):
         {
            data[0] = buf[1];
            data[1] = buf[0];
         }break;

         case(1):
         {
            data[0] = buf[0];
         }break;

         default:
         {
         }
      }
   }

//   OS_LYRG_usleep(uSdelay);

   return retVal;
#else
   ERRG_codeE  retVal = I2C__RET_SUCCESS;
   UINT8       currentAddress, bytesReceived;
   UINT32      iteration;

   if(dataAccessSize == 0)
   {
      dataAccessSize = 2;
   }
   OS_LYRG_lockMutex(&I2C_DRVP_localMutex);

   data += (dataAccessSize-1);

   iteration = 0;

   currentAddress = ((UINT8)dw_i2c_getTargetAddress(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)))<<1;

   
   if(currentAddress != tarAddress)
   {
      retVal = I2C_DRVP_updateSlaveAddress(instanceId, tarAddress, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);

         return retVal;
      }
   }

   if(regAddressAccessSize > 1)
   {
      retVal = I2C_DRVP_writeByte(instanceId, SHIFT_RIGHT(regAddress,BITS_IN_BYTE), I2C_DRVP_NO_STOP, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }
   
   retVal = I2C_DRVP_writeByte(instanceId, regAddress, I2C_DRVP_NO_STOP, uSdelay);
   if(ERRG_FAILED(retVal))
   {
      OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
      return retVal;
   }

   if(dataAccessSize == 1)
   {
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_ISSUE_STOP);
   }
   else if (dataAccessSize == 4)
   {
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_NO_STOP);
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_NO_STOP);
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_NO_STOP);
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_ISSUE_STOP);
   }
   else
   {
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_NO_STOP);
      I2C_DRVP_requestReadByte(instanceId, I2C_DRVP_ISSUE_STOP);
   }

   bytesReceived = dw_i2c_getRxFifoLevel(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));
   
   do
   {
      bytesReceived = dw_i2c_getRxFifoLevel(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));
      OS_LYRG_usleep(uSdelay);
   } while ((bytesReceived < dataAccessSize) && (iteration++ < I2C_DRVP_MAX_ITERATIONS));
   
   if(iteration >= I2C_DRVP_MAX_ITERATIONS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error reading all data from 0x%x bytesReceived=%d, dataAccessSize=%d, num of iterations=%d\n", regAddress, bytesReceived, dataAccessSize, iteration);
      OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
      return I2C__ERR_READ_FAIL_TO_RECEIVE_ALL_DATA;
   }

   for(; bytesReceived > 0 ; bytesReceived--)
   {
      retVal = I2C_DRVP_readByte(instanceId, data--, uSdelay);
      if(ERRG_FAILED(retVal))
      {
         OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
         return retVal;
      }
   }
   OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);

   return I2C__RET_SUCCESS;
#endif   
}




ERRG_codeE I2C_DRVG_direct_read(I2C_DRVG_instanceIdE instanceId, UINT8 dataAccessSize, UINT8 *data, UINT16 tarAddress, UINT32 uSdelay)
{

   ERRG_codeE  retVal = I2C__RET_SUCCESS;
   INT32 nmsgs=0;
   UINT8 buf[10];
   memset(buf,0,sizeof(buf));
   struct i2c_rdwr_ioctl_data packets;
   struct i2c_msg messages[1];

   tarAddress = tarAddress >> 1;
   retVal = I2C_DRVP_updateSlaveAddress(instanceId, tarAddress, uSdelay);

   if(ERRG_FAILED(retVal))
   {
      return I2C__ERR_INIT_FAIL;
   }

  
   /* The data will get returned in this structure */
   messages[0].addr  = tarAddress;
   messages[0].flags = I2C_M_RD/* | I2C_M_NOSTART*/;
   messages[0].len   = dataAccessSize;
   messages[0].buf   = buf;
   /* Send the request to the kernel and get the result back */
   packets.msgs      = messages;
   packets.nmsgs     = 1;

   nmsgs=ioctl(I2C_DRVP_i2cDeviceDesc[instanceId].fd, I2C_RDWR, &packets);
   if(nmsgs<0) 
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to ioctl i2c%d,  errno = %s, fd-%d\n",instanceId, strerror(errno), I2C_DRVP_i2cDeviceDesc[instanceId].fd);
      retVal = I2C__ERR_INIT_FAIL;
   }

   if (packets.nmsgs != (UINT32)nmsgs)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to read i2c, nmsgs = %d\n",nmsgs);
   }
   else
   {
#ifdef I2C_DEBUG
      LOGG_PRINT(LOG_ERROR_E, NULL, "read success! instanceId = %d, tarAddress = 0x%x, nmsgs = %d, %x,%x,%x,%x\n",instanceId,tarAddress,nmsgs,buf[0],buf[1],buf[2],buf[3]);
#endif

      switch (dataAccessSize)
      {
         case(4):
         {
            data[0] = buf[3];
            data[1] = buf[2];
            data[2] = buf[1];
            data[3] = buf[0];
         }break;

         case(3):
         {
            data[0] = buf[2];
            data[1] = buf[1];
            data[2] = buf[0];
         }break;

         case(2):
         {
            data[0] = buf[1];
            data[1] = buf[0];
         }break;

         case(1):
         {
            data[0] = buf[0];
         }break;

         default:
         {
         }
      }
   }

//   OS_LYRG_usleep(uSdelay);

   return retVal;
   
}


/****************************************************************************
*
*  Function Name:  I2C_DRVG_direct_write
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: I2C driver
*
****************************************************************************/
ERRG_codeE I2C_DRVG_direct_write(I2C_DRVG_instanceIdE instanceId, UINT8 dataAccessSize, UINT32 data, UINT16 tarAddress, UINT32 uSdelay)
{
   ERRG_codeE retVal = I2C__RET_SUCCESS;

   int size;
   UINT8 buf[10] = {0};

   tarAddress = tarAddress >> 1;

   retVal = I2C_DRVP_updateSlaveAddress(instanceId, tarAddress, uSdelay);

   if(ERRG_FAILED(retVal))
   {
      return I2C__ERR_INIT_FAIL;
   }

   /* write address + register */
   
   switch (dataAccessSize)
   {
      case(4):
      {
         buf[0] = ((data >> 24) & 0xFF);
         buf[1] = ((data >> 16) & 0xFF);
         buf[2] = ((data >> 8) & 0xFF);
         buf[3] = ((data >> 0) & 0xFF);
      }break;

      case(3):
      {
         buf[0] = ((data >> 16) & 0xFF);
         buf[1] = ((data >> 8) & 0xFF);
         buf[2] = ((data >> 0) & 0xFF);         
      }break;

      case(2):
      {
         buf[0] = ((data >> 8) & 0xFF);
         buf[1] = ((data >> 0) & 0xFF);
      }break;

      case(1):
      {
         buf[0] = ((data >> 0) & 0xFF);
      }break;

      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "dataAccessSize not supported, dataAccessSize = %d\n",dataAccessSize);
         return I2C__ERR_WRITE_I2C_ABORT;         
      }
   }

   size = write(I2C_DRVP_i2cDeviceDesc[instanceId].fd, buf , dataAccessSize);
   
   if (size != dataAccessSize)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to write i2c,instanceId %x tarAddress %x  data = 0x%x, dataAccessSize = %d size = %d, errno = %s\n",instanceId, tarAddress,data,dataAccessSize,size,strerror(errno));
      return I2C__ERR_READ_FAIL_TO_RECEIVE_ALL_DATA;
   }
   else
   {
#ifdef I2C_DEBUG
      LOGG_PRINT(LOG_ERROR_E, NULL, "write success! instanceId %d tarAddress = 0x%x, dataAccessSize = %d, size = %d, data = %x,%x\n",instanceId, tarAddress,dataAccessSize,size,buf[0],buf[1]);
#endif
   }

//   OS_LYRG_usleep(uSdelay);

   return retVal;


   return I2C__RET_SUCCESS;
}


ERRG_codeE I2C_DRVG_enableI2c(I2C_DRVG_instanceIdE instanceId)
{
#ifdef __KERNEL_I2C__
   FIX_UNUSED_PARAM_WARN(instanceId);
   return(I2C__RET_SUCCESS);
#else
   OS_LYRG_lockMutex(&I2C_DRVP_localMutex);

   if(!dw_i2c_enableMaster(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)))
   {
      OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
      return(I2C__ERR_FAILED_TO_ENABLE_PERIHERAL);
   }
   else
   {
     OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
     return(I2C__RET_SUCCESS);
   }
#endif
}

ERRG_codeE I2C_DRVG_disableI2c(I2C_DRVG_instanceIdE instanceId)
{
#ifdef __KERNEL_I2C__
   FIX_UNUSED_PARAM_WARN(instanceId);
   return I2C__RET_SUCCESS;
#else
    OS_LYRG_lockMutex(&I2C_DRVP_localMutex);
   if(!dw_i2c_disableMaster(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev)))
   {
      OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
      return(I2C__ERR_FAILED_TO_DISABLE_PERIHERAL);
   }
   else
   {
      OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
      return(I2C__RET_SUCCESS);
   }
#endif   
}

ERRG_codeE I2C_DRVG_getI2cStatus(I2C_DRVG_instanceIdE instanceId, UINT32 *data)
{
#ifdef __KERNEL_I2C__
   FIX_UNUSED_PARAM_WARN(instanceId);
   FIX_UNUSED_PARAM_WARN(data);
   return I2C__RET_SUCCESS;
#else
   enum dw_i2c_tx_abort status;

   OS_LYRG_lockMutex(&I2C_DRVP_localMutex);

   status = dw_i2c_getTxAbortSource(&(I2C_DRVP_i2cDeviceDesc[instanceId].i2cDev));
   *data = (UINT32)status;
   OS_LYRG_unlockMutex(&I2C_DRVP_localMutex);
#endif

   return I2C__RET_SUCCESS;
}

#ifdef I2C_TEST
static void I2C_DRVP_testI2C(I2C_DRVG_instanceIdE instanceId)
{
   int size;
   //Set address to needed device
#define I2C_ADDRESS                                      ((0x32)>>1)
   UINT8 who_am_i_reg = 0x20;
   UINT8 cfg_reg_1 = 0x20;
   char  buf[2];

   if (I2C_DRVP_i2cDeviceDesc[instanceId].fd < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "fail to open i2c errno = %s\n",strerror(errno));
   }
   else
   {
      if( ioctl( I2C_DRVP_i2cDeviceDesc[instanceId].fd, I2C_SLAVE, I2C_ADDRESS ) < 0 )
      {
          LOGG_PRINT(LOG_ERROR_E, NULL, "fail to ioctl i2c, errno = %s\n",strerror(errno));
      } 
      else
      {
         int j;
         for (j = 0; j < 5; j++)
         {
            buf[0] = cfg_reg_1;
            buf[1] = 0xAB;

            size = write(I2C_DRVP_i2cDeviceDesc[instanceId].fd, &buf , sizeof(buf));
            if (size != sizeof(buf))
            {
               LOGG_PRINT(LOG_ERROR_E, NULL, "fail to write i2c, size = %d, errno = %s\n",size,strerror(errno));
            }
            else
            {
               size = write(I2C_DRVP_i2cDeviceDesc[instanceId].fd, &cfg_reg_1 , sizeof(cfg_reg_1));
               if (size != sizeof(cfg_reg_1))
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "fail to write (data) i2c, size = %d, errno = %s\n",size,strerror(errno));
               }
               else
               {
                  UINT8 bufRead = 0;

                  size = read(I2C_DRVP_i2cDeviceDesc[instanceId].fd, &bufRead, sizeof(bufRead));

                  if (size!= sizeof(bufRead))
                  {
                     LOGG_PRINT(LOG_ERROR_E, NULL, "fail to read i2c, size = %d\n",size);
                  }
                  else
                  {
                     LOGG_PRINT(LOG_ERROR_E, NULL, "read i2c success!!!! 0x%x\n",bufRead);
                  }
               }
            }
         }
      }

      close(I2C_DRVP_i2cDeviceDesc[instanceId].fd);
   }

}
#endif


#ifdef __cplusplus
}
#endif

