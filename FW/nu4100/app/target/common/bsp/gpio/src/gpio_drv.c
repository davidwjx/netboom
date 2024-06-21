/****************************************************************************
 *
 *   FileName: gpio_drv.c
 *
 *   Author:  Danny B.
 *
 *   Date:
 *
 *   Description: GPIO driver
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "gpio_drv.h"
#include "os_lyr.h"


/****************************************************************************
 Current files support two modes of GPIO access:
    1) via kernel (#define __KERNEL_GPIO__)
   2) via mmap registers and synopsys driver
 For perfomance reason this driver uses user space synopsys driver.
 Important:
      On GPIOG_Open we export GPIO for user space usage in kernel.
      Such policy gives protection for GPIOs that are in use by kerenl drivers
 Warning:
     Current gpio driver in kernel uses shadow registrers, this prevents
    direct modification of GPIO registers from user space.
 ****************************************************************************/

//#define __DRIVER_MODE_KERNEL__

#ifdef __KERNEL_GPIO__
#include <errno.h>
#include <unistd.h>
#endif

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
typedef enum
{
   GPIO_DRVP_MODULE_STATUS_DUMMY_E = 0,
   GPIO_DRVP_MODULE_STATUS_CLOSE_E = 1,
   GPIO_DRVP_MODULE_STATUS_OPEN_E  = 2,
   GPIO_DRVP_MODULE_STATUS_LAST_E
} GPIO_DRVG_moduleStatusE;

typedef ERRG_codeE (*GPIO_DRVP_ioctlFuncListT)(void *argP);

#ifdef __KERNEL_GPIO__
#define BUF_MAX_SIZE (50)
/* base chip gpio is 256 - number of gpios as set in the DTS file */
#define CHIP_BASE_GPIO (256 - GPIO_DRVG_NUM_OF_GPIO_E)

typedef struct {
   int fdGpioVal;
   int fdGpioDir;
} GPIO_DRVP_gpioFdT;


#endif

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct {
   struct dw_device              gpioDev;
   struct dw_gpio_instance       gpioInstance;
   struct dw_gpio_param          gpio_param;
   GPIO_DRVG_moduleStatusE       moduleStatus;
   GPIO_DRVP_ioctlFuncListT      *ioctlFuncList;
} GPIO_DRVP_gpioDeviceDescT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   GPIO_DRVP_gpioDeviceDescT     GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_NUM_INST];
static   GPIO_DRVP_ioctlFuncListT      GPIO_DRVP_ioctlFuncList[GPIO_DRVG_NUM_OF_IOCTLS_E];
#ifdef __KERNEL_GPIO__
#ifdef __DRIVER_MODE_KERNEL__
static   GPIO_DRVP_gpioFdT             GPIO_DRVP_gpioFdList[GPIO_DRVG_NUM_OF_GPIO_E];
#endif
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

static ERRG_codeE GPIO_DRVP_openGpio(GPIO_DRVG_gpioOpenParamsT *params);
static ERRG_codeE GPIO_DRVP_setGpioDir(GPIO_DRVG_gpioSetDirParamsT *params);
static ERRG_codeE GPIO_DRVP_setGpioVal(GPIO_DRVG_gpioSetValParamsT *params);
static ERRG_codeE GPIO_DRVP_getGpioVal(GPIO_DRVG_gpioGetValParamsT *params);
static ERRG_codeE GPIO_DRVP_setGpioPortDir(GPIO_DRVG_gpioSetPortDirParamsT *params);
static ERRG_codeE GPIO_DRVP_setGpioPortVal(GPIO_DRVG_gpioSetPortValParamsT *params);
static ERRG_codeE GPIO_DRVP_getGpioPortVal(GPIO_DRVG_gpioGetPortValParamsT *params);
#ifndef __DRIVER_MODE_KERNEL__
static ERRG_codeE GPIO_DRVP_init(GPIO_DRVP_gpioDeviceDescT *dev, void* gpioInstanceAddress);
#endif
static ERRG_codeE GPIO_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE GPIO_DRVP_close(IO_HANDLE handle);
static ERRG_codeE GPIO_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static void GPIO_DRVP_dataBaseReset(void);
static void GPIO_DRVP_dataBaseInit(void);

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static ERRG_codeE GPIO_DRVP_openGpio(GPIO_DRVG_gpioOpenParamsT *params)
{
   ERRG_codeE ret = GPIO__RET_SUCCESS;
   (void)params;
#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   int fd,gpio,size;
   char buf[BUF_MAX_SIZE];

   gpio = CHIP_BASE_GPIO + params->gpioNum;

   /* Acquire control over GPIO */
   fd = open("/sys/class/gpio/export", O_WRONLY);
   if (fd < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_openGpio fail opening export, gpio = %d, fd = %d, errno = %s\n",gpio,fd,strerror(errno));
   }
   else
   {
      sprintf(buf, "%d", gpio);

      size = write(fd, buf, strlen(buf));
      if (close(fd) < 0)
      {
         ret = GPIO__ERR_IO_ERROR;
         LOGG_PRINT(LOG_ERROR_E, ret, "fail closing a file descriptor, fd = %d, errno = %s\n", fd,strerror(errno));
         return ret;
      }
      if (size < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "GPIO_DRVP_openGpio fail to acquire, gpio = %d, buf = %s, errno = %s\n",gpio,buf,strerror(errno));
         ret = GPIO__ERR_IO_ERROR;
         return ret;
      }
   }

   sprintf(buf, "/sys/class/gpio/gpio%d/direction", gpio);

   GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioDir = open(buf, O_WRONLY);

   if (GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioDir < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_openGpio fail, open direction fd fail, gpio = %d, errno = %s\n",gpio,strerror(errno));
   }


   sprintf(buf, "/sys/class/gpio/gpio%d/value", gpio);

   GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal = open(buf, O_WRONLY);

   if(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_openGpio fail, open value fd fail, gpio = %d, errno = %s\n",gpio,strerror(errno));
   }
#endif
#endif
   return ret;
}

static ERRG_codeE GPIO_DRVP_closeGpio(GPIO_DRVG_gpioCloseParamsT *params)
{
   ERRG_codeE ret = GPIO__RET_SUCCESS;
   (void)params;
#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   int fd,gpio,size;
   char buf[BUF_MAX_SIZE];

   gpio = CHIP_BASE_GPIO + params->gpioNum;

   /* Release control over GPIO */
   fd = open("/sys/class/gpio/unexport", O_WRONLY);
   if (fd < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_openGpio fail opening unexport, gpio = %d, errno = %s\n",gpio,strerror(errno));
   }
   else
   {
      if (GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioDir > 0)
         close(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioDir);

      if (GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal > 0)
         close(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal);

      sprintf(buf, "%d", gpio);
      size = write(fd, buf, strlen(buf));

      if (size < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "GPIO_DRVP_openGpio fail to release, gpio = %d, buf = %s, errno = %s\n",gpio,buf,strerror(errno));
         ret = GPIO__ERR_IO_ERROR;
      }
      if (close(fd) < 0)
      {
         ret = SENSOR__ERR_OPEN_DEVICE_FAIL;
         LOGG_PRINT(LOG_ERROR_E, ret, "fail closing a file descriptor, fd = %d, errno = %s\n", fd,strerror(errno));
      }
   }
#endif
#endif
   return ret;
}

static ERRG_codeE GPIO_DRVP_setGpioDir(GPIO_DRVG_gpioSetDirParamsT *params)
{
   ERRG_codeE ret = GPIO__RET_SUCCESS;
   (void)params;
#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   int size;

   if (params->direction == GPIO_DRVG_IN_DIRECTION_E)
   {
      /* Set in direction */
      size = write(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioDir, "in", 2);
   }
   else
   {
      /* Set out direction */
      size = write(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioDir, "out", 3);
   }

   if (size < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_setGpioDir fail to write to gpio direction, gpio = %d, errno = %s\n",params->gpioNum,strerror(errno));
   }
#endif
#else

   INT32 val;
   GPIO_DRVG_numInstE instNum;
   if (params->gpioNum < GPIO_DRVG_GPIO_32_E)
      instNum=GPIO_DRVG_INST0_E;
   else
   {
      instNum=GPIO_DRVG_INST1_E;
      params->gpioNum -= GPIO_DRVG_GPIO_32_E;
   }
   if(params->gpioNum <= GPIO_DRVP_gpioDeviceDesc[instNum].gpio_param.pwidth_a)
   {
      val = dw_gpio_setBitDirection(&GPIO_DRVP_gpioDeviceDesc[instNum].gpioDev ,Gpio_port_a, params->direction , (1 << params->gpioNum));
      if(val)
      {
         ret = GPIO__ERR_IO_ERROR;
         LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_setGpioDir fail to write to gpio direction, val = %d, direction = %d, gpio = %d, errno = %s\n",val,params->direction,(1 << params->gpioNum),strerror(errno));
      }
   }
   else
   {
      ret = GPIO__ERR_GPIO_NUM_EXCEED_MAX;
   }
#endif
   return ret;
}

static ERRG_codeE GPIO_DRVP_setGpioVal(GPIO_DRVG_gpioSetValParamsT *params)
{
   ERRG_codeE ret = GPIO__RET_SUCCESS;
#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   int size;

   if (params->val == GPIO_DRVG_GPIO_STATE_SET_E)
   {
      /* Set GPIO high status */
      size = write(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal, "1", 1);
   }
   else
   {
      /* Set GPIO low status */
      size = write(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal, "0", 1);
   }

   if (size < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_getGpioVal fail to write to gpio value, gpio = %d, errno = %s\n",params->gpioNum,strerror(errno));
   }
#endif
#else
   INT32 val;
   GPIO_DRVG_numInstE instNum;
   if (params->gpioNum < GPIO_DRVG_GPIO_32_E)
      instNum=GPIO_DRVG_INST0_E;
   else
   {
      instNum=GPIO_DRVG_INST1_E;
      params->gpioNum -= GPIO_DRVG_GPIO_32_E;
   }

   if(params->gpioNum <= GPIO_DRVP_gpioDeviceDesc[instNum].gpio_param.pwidth_a)
   {
      val = dw_gpio_setBit(&GPIO_DRVP_gpioDeviceDesc[instNum].gpioDev, Gpio_port_a, params->val, (1 << params->gpioNum));
      if(val)
      {
         ret = GPIO__ERR_IO_ERROR;
         LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_getGpioVal fail to write to gpio direction, val = %d, params->val = %d, gpio = %d\n",val,params->val,(1 << params->gpioNum));
      }
   }
   else
   {
      ret = GPIO__ERR_GPIO_NUM_EXCEED_MAX;
   }

#endif
   return ret;
}
static ERRG_codeE GPIO_DRVP_getGpioVal(GPIO_DRVG_gpioGetValParamsT *params)
{
   ERRG_codeE ret = GPIO__RET_SUCCESS;

#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   int size;
   char value;

   size = read(GPIO_DRVP_gpioFdList[params->gpioNum].fdGpioVal, &value , 1);

   if (size < 0)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_getGpioVal fail to read gpio, gpio = %d,  errno = %s\n",params->gpioNum,strerror(errno));
   }
   else
   {
      if(value == '0')
      {
         /* Current GPIO status low */
         params->val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
      }
      else
      {
         /* Current GPIO status high */
         params->val = GPIO_DRVG_GPIO_STATE_SET_E;
      }
   }
#endif
#else
   UINT32 val;
   GPIO_DRVG_numInstE instNum;
   if (params->gpioNum < GPIO_DRVG_GPIO_32_E)
      instNum=GPIO_DRVG_INST0_E;
   else
   {
      instNum=GPIO_DRVG_INST1_E;
      params->gpioNum -= GPIO_DRVG_GPIO_32_E;
   }

   if(params->gpioNum <= GPIO_DRVP_gpioDeviceDesc[instNum].gpio_param.pwidth_a)
   {
      val = dw_gpio_getExtPort(&GPIO_DRVP_gpioDeviceDesc[instNum].gpioDev, Gpio_port_a);
      params->val = (GPIO_DRVG_gpioStateE) ((val >> (params->gpioNum)) & 0x1);
   }
   else
   {
      ret = GPIO__ERR_GPIO_NUM_EXCEED_MAX;
   }
#endif

   return (ret);
}


static ERRG_codeE GPIO_DRVP_setGpioPortDir(GPIO_DRVG_gpioSetPortDirParamsT *params)
{
   ERRG_codeE  ret = GPIO__RET_SUCCESS;
   UINT32 direction = 0;
#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   GPIO_DRVG_gpioNumberE gpioNum;
   GPIO_DRVG_gpioSetDirParamsT gpioParams;

   gpioParams.direction = params->direction;

   for (gpioNum = GPIO_DRVG_GPIO_0_E; gpioNum < GPIO_DRVG_NUM_OF_GPIO_E; gpioNum++)
   {
      gpioParams.gpioNum = gpioNum;
      GPIO_DRVP_setGpioDir(&gpioParams);
   }
#endif
#else
   INT32       val;
   if ( params->direction == GPIO_DRVG_OUT_DIRECTION_E)
      direction = 0xffffffff;
   val = dw_gpio_setDirection(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].gpioDev ,Gpio_port_a, direction);
   if(val)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_setGpioPortDir fail to write to gpio 0 direction, val = %d, params->direction = %d\n",val,params->direction);
   }
   val = dw_gpio_setDirection(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST1_E].gpioDev ,Gpio_port_a, direction);
   if(val)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_setGpioPortDir fail to write to gpio 1 direction, val = %d, params->direction = %d\n",val,params->direction);
   }
#endif
   return ret;
}

static ERRG_codeE GPIO_DRVP_setGpioPortVal(GPIO_DRVG_gpioSetPortValParamsT *params)
{
   ERRG_codeE  ret = GPIO__RET_SUCCESS;
#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   GPIO_DRVG_gpioNumberE gpioNum;
   GPIO_DRVG_gpioSetValParamsT gpioParams;
   UINT32 portVal;

   portVal = params->portVal;

   for (gpioNum = GPIO_DRVG_GPIO_0_E; gpioNum < GPIO_DRVG_NUM_OF_GPIO_E; gpioNum++)
   {
      gpioParams.gpioNum = gpioNum;
      gpioParams.val = (portVal & 0x1);
      portVal = portVal >> 1;

      GPIO_DRVP_setGpioVal(&gpioParams);
   }
#endif
#else
   INT32       val;

   val = dw_gpio_setPort(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].gpioDev, Gpio_port_a, params->portVal);
   if(val)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_setGpioPortVal fail to write to gpio direction, val = %d, params->portVal = %d\n",val,params->portVal);
   }
   val = dw_gpio_setPort(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST1_E].gpioDev, Gpio_port_a, params->portVal>>GPIO_DRVG_GPIO_32_E);
   if(val)
   {
      ret = GPIO__ERR_IO_ERROR;
      LOGG_PRINT(LOG_ERROR_E, ret, "GPIO_DRVP_setGpioPortVal fail to write to gpio direction, val = %d, params->portVal = %d\n",val,params->portVal);
   }
#endif
   return ret;
}
static ERRG_codeE GPIO_DRVP_getGpioPortVal(GPIO_DRVG_gpioGetPortValParamsT *params)
{
   ERRG_codeE  ret = GPIO__RET_SUCCESS;

#ifdef __DRIVER_MODE_KERNEL__
#ifdef __KERNEL_GPIO__
   GPIO_DRVG_gpioNumberE         gpioNum;
   GPIO_DRVG_gpioGetValParamsT   gpioParams;
   UINT32 portVal = 0;

   FIX_UNUSED_PARAM_WARN(params);

   for (gpioNum = GPIO_DRVG_GPIO_0_E; gpioNum < GPIO_DRVG_NUM_OF_GPIO_E; gpioNum++)
   {
      gpioParams.gpioNum = gpioNum;

      ret = GPIO_DRVP_getGpioVal(&gpioParams);

      if (ERRG_FAILED(ret))
      {
         portVal |= gpioParams.val;
      }
   }
#endif
#else
   params->portVal = dw_gpio_getPort(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].gpioDev, Gpio_port_a);
   params->portVal |= (UINT64)dw_gpio_getPort(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST1_E].gpioDev, Gpio_port_a)<<GPIO_DRVG_GPIO_32_E;
#endif

   return (ret);
}

#ifndef __DRIVER_MODE_KERNEL__
static ERRG_codeE GPIO_DRVP_init(GPIO_DRVP_gpioDeviceDescT *dev, void* gpioInstanceAddress)
{
   // GPIO 1 device structure setup.
   dev->gpioDev.comp_type    = Dw_apb_gpio;
   dev->gpioDev.data_width   = BYTES_IN_DWORD;
   dev->gpioDev.base_address = gpioInstanceAddress;
   dev->gpioDev.comp_param   = &(dev->gpio_param);
   dev->gpioDev.instance     = &(dev->gpioInstance);
   dev->gpioDev.os           = NULL;

   return GPIO__RET_SUCCESS;
}
#endif
static ERRG_codeE GPIO_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE retCode = GPIO__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(deviceId);
   FIX_UNUSED_PARAM_WARN(params);

#ifndef __DRIVER_MODE_KERNEL__
   MEM_MAPG_addrT oAddressP0;
   MEM_MAPG_addrT oAddressP1;
#endif
   *handleP = (IO_HANDLE)NULL;

   if(GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].moduleStatus == GPIO_DRVP_MODULE_STATUS_CLOSE_E)
   {
      GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].moduleStatus = GPIO_DRVP_MODULE_STATUS_OPEN_E;
#ifndef __DRIVER_MODE_KERNEL__
#if (INU_DEFS_PLATFORM == CHIP_PLATFORM)
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GIO0_E, &oAddressP0);
      MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GIO1_E, &oAddressP1);
#endif
      GPIO_DRVP_init(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E], oAddressP0);
      GPIO_DRVP_init(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST1_E], oAddressP1);

      dw_gpio_init(&(GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].gpioDev));
      dw_gpio_init(&(GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST1_E].gpioDev));
#endif
      *handleP = (IO_HANDLE)(&GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E]);
   }
   else // if(GME_DRVP_deviceDesc.moduleStatus == GME_DRVP_MODULE_STATUS_CLOSE_E)
   {
      retCode = GPIO__ERR_OPEN_FAIL_DRV_NOT_CLOSED;
      LOGG_PRINT(LOG_ERROR_E, retCode, "open GPIO driver fail!!!\n");
   }

   return GPIO__RET_SUCCESS;
}

static ERRG_codeE GPIO_DRVP_close(IO_HANDLE handle)
{
   ERRG_codeE  retVal = GPIO__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
#ifndef __DRIVER_MODE_KERNEL__
   dw_gpio_init(&(GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].gpioDev));
#endif

   GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].moduleStatus = GPIO_DRVP_MODULE_STATUS_CLOSE_E;

   return(retVal);
}



static ERRG_codeE GPIO_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   ERRG_codeE  gpioIoctlRetCode = GPIO__RET_SUCCESS;
   UINT16      ioctl;

   if(((GPIO_DRVP_gpioDeviceDescT*)handle)->moduleStatus == GPIO_DRVP_MODULE_STATUS_OPEN_E)
   {
      ioctl = IO_CMD_GET_IOCTL(cmd);
      if (IO_CMD_IS_DEVICE_ID(cmd, IO_GPIO_E))
      {
         if(handle == &GPIO_DRVP_gpioDeviceDesc)
         {
            gpioIoctlRetCode = (((GPIO_DRVP_gpioDeviceDescT*)handle))->ioctlFuncList[ioctl](argP);
         }
         else
         {
            gpioIoctlRetCode = GPIO__ERR_IOCTL_FAIL_WRONG_HANDLE;
         }
      }
      else  // if (IO_CMD_IS_DEVICE_ID(cmd, IO_GPIO_E))
      {
         gpioIoctlRetCode = GPIO__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE;
      }
   }
   else  // if(GPIO_DRVP_deviceDesc.moduleStatus == GPIO_DRVP_MODULE_STATUS_OPEN_E)
   {
      gpioIoctlRetCode = GPIO__ERR_IOCTL_FAIL_DRV_NOT_OPEN;
   }

   return(gpioIoctlRetCode);
}


static void GPIO_DRVP_dataBaseReset(void)
{
   memset(&GPIO_DRVP_gpioDeviceDesc, 0, sizeof(GPIO_DRVP_gpioDeviceDesc));
}

static void GPIO_DRVP_dataBaseInit(void)
{

   GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].moduleStatus       = GPIO_DRVP_MODULE_STATUS_CLOSE_E;
   GPIO_DRVP_gpioDeviceDesc[GPIO_DRVG_INST0_E].ioctlFuncList      = GPIO_DRVP_ioctlFuncList;

   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_OPEN_GPIO_CMD_E)]                = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_openGpio;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_CLOSE_GPIO_CMD_E)]               = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_closeGpio;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_SET_GPIO_DIR_CMD_E)]             = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_setGpioDir;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_SET_GPIO_VAL_CMD_E)]             = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_setGpioVal;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_GET_GPIO_VAL_CMD_E)]             = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_getGpioVal;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_SET_GPIO_PORT_DIR_CMD_E)]        = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_setGpioPortDir;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_SET_GPIO_PORT_VAL_CMD_E)]        = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_setGpioPortVal;
   GPIO_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(GPIO_DRVG_GET_GPIO_PORT_VAL_CMD_E)]        = (GPIO_DRVP_ioctlFuncListT) GPIO_DRVP_getGpioPortVal;
}



/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/

ERRG_codeE GPIO_DRVG_init(IO_PALG_apiCommandT *palP)
{
   ERRG_codeE gpioInitRetCode = GPIO__RET_SUCCESS;

   if(palP)
   {
      palP->close =  (IO_PALG_closeT)  &GPIO_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT)  &GPIO_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)   &GPIO_DRVP_open;
   }
   else
   {
      gpioInitRetCode = GPIO__ERR_INIT_FAIL_WRONG_PAL_HANDLE;
   }

   if(ERRG_SUCCEEDED(gpioInitRetCode))
   {
      GPIO_DRVP_dataBaseReset();
      GPIO_DRVP_dataBaseInit();
   }

   return gpioInitRetCode;
}


#ifdef __cplusplus
}
#endif

