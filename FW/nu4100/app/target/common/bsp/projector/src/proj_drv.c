/****************************************************************************
 *
 *   FileName: proj_drv.c
 *
 *   Author:  Giyora A.
 *
 *   Date:
 *
 *   Description: PROJ driver
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "proj_drv.h"
#include "os_lyr.h"
#include "nu4100_regs.h"
#include "gpio_drv.h"
#include "gme_mngr.h"
#include <unistd.h>//for close functioin at PROJ_DRVP_close



/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
typedef enum
{
   PROJ_DRVP_MODULE_STATUS_DUMMY_E = 0,
   PROJ_DRVP_MODULE_STATUS_CLOSE_E = 1,
   PROJ_DRVP_MODULE_STATUS_OPEN_E  = 2,
   PROJ_DRVP_MODULE_STATUS_LAST_E
} PROJ_DRVG_moduleStatusE;

typedef ERRG_codeE (*PROJ_DRVP_ioctlFuncListT)(void *argP);


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct {
   PROJ_DRVG_moduleStatusE       moduleStatus;
   PROJ_DRVP_ioctlFuncListT      *ioctlFuncList;
} PROJ_DRVP_projDeviceDescT;

typedef struct
{
   MEM_MAPG_addrT deviceBaseAddress;
} PROJ_DRVP_gmeDeviceDescT;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   PROJ_DRVP_gmeDeviceDescT      GME_DRVP_deviceDesc;
static   PROJ_DRVP_projDeviceDescT     PROJ_DRVP_projDeviceDesc;
static   PROJ_DRVP_ioctlFuncListT      PROJ_DRVP_ioctlFuncList[PROJ_DRVG_NUM_OF_IOCTLS_E];
static   PROJ_DRVG_projStateE          PROJ_DRVP_projState = PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E;
static   int projfd;

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

static ERRG_codeE PROJ_DRVP_openProj(PROJ_DRVG_projOpenParamsT *params);
static ERRG_codeE PROJ_DRVP_setProjVal(PROJ_DRVG_projSetValParamsT *params);
static ERRG_codeE PROJ_DRVP_getProjVal(PROJ_DRVG_projGetValParamsT *params);
static ERRG_codeE PROJ_DRVP_checkSupport(PROJ_DRVG_projSetValParamsT *params);
static ERRG_codeE PROJ_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, PROJ_DRVG_openParamsT *params);
static ERRG_codeE PROJ_DRVP_close(IO_HANDLE handle);
static ERRG_codeE PROJ_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static void PROJ_DRVP_dataBaseReset(void);
static void PROJ_DRVP_dataBaseInit(void);


/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static ERRG_codeE PROJ_DRVP_openProj(PROJ_DRVG_projOpenParamsT *params)
{
   FIX_UNUSED_PARAM_WARN(params);

   ERRG_codeE ret = PROJ__RET_SUCCESS;


   return ret;
}

static ERRG_codeE PROJ_DRVP_closeProj(PROJ_DRVG_projCloseParamsT *params)
{
   FIX_UNUSED_PARAM_WARN(params);

   ERRG_codeE ret = PROJ__RET_SUCCESS;

   return ret;
}


static ERRG_codeE PROJ_DRVP_configGpio(GPIO_DRVG_gpioNumberE gpioNumber,GPIO_DRVG_gpioStateE state, GPIO_DRVG_gpioDirectionE direction)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GPIO_DRVG_gpioSetDirParamsT         gpioDir;

   gpioDir.direction   = direction;
   gpioDir.gpioNum     = gpioNumber;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

   gpioParams.val = state;
   gpioParams.gpioNum = gpioNumber;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

   return (retVal);
}

static ERRG_codeE PROJ_DRVP_handle_proj0(PROJ_DRVG_projStateE projState)
{
   INU_DEFSG_moduleTypeE model;
   INU_DEFSG_baseVersionE baseVersion;

   model = GME_MNGRG_getModelType();
   baseVersion = GME_MNGRG_getBaseVersion();

   ERRG_codeE ret = PROJ__RET_SUCCESS;
   switch (model)
   {
      case(INU_DEFSG_BOOT0_E):
      {
         if (projState == PROJ_DRVG_PROJ_STATE_CLEAR_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_19_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_19_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;

      case(INU_DEFSG_BOOT200_E):
      {
         if (projState == PROJ_DRVG_PROJ_STATE_CLEAR_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_19_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_19_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_19_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;

      case(INU_DEFSG_BOOT30_E):
      case(INU_DEFSG_BOOT60_E):
      case(INU_DEFSG_BOOT65_E):
      case(INU_DEFSG_BOOT66_E):
      case(INU_DEFSG_BOOT70_E):
      case(INU_DEFSG_BOOT80_E):
      case(INU_DEFSG_BOOT90_E):
      {
         if (projState == PROJ_DRVG_PROJ_STATE_CLEAR_E)
         {
            if (baseVersion == INU_DEFSG_BASE_VERSION_1_E)
            {
               PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
            }
            else if (baseVersion == INU_DEFSG_BASE_VERSION_2_E)
            {
               PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
            }
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;

      case(INU_DEFSG_BOOT40_E):
      {
         printf("PROJ_DRVP_handle_proj0 boot40 state %d\n",projState);
         if (projState == PROJ_DRVG_PROJ_STATE_CLEAR_E)
         {
            system("echo 0 > /sys/bus/i2c/drivers/as3648/0-0030/led_current1");
            system("echo 0 > /sys/bus/i2c/drivers/as3648/0-0030/led_current2");
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E)
         {
            system("echo 100 > /sys/bus/i2c/drivers/as3648/0-0030/led_current1");
            system("echo 100 > /sys/bus/i2c/drivers/as3648/0-0030/led_current2");
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E)
         {
            system("echo 150 > /sys/bus/i2c/drivers/as3648/0-0030/led_current1");
            system("echo 150 > /sys/bus/i2c/drivers/as3648/0-0030/led_current2");
         }
      }
      break;

      case(INU_DEFSG_BOOT50_E):
      case(INU_DEFSG_BOOT51_E):
      case(INU_DEFSG_BOOT52_E):
      case(INU_DEFSG_BOOT53_E):
      {
         if (projState == PROJ_DRVG_PROJ_STATE_CLEAR_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_39_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_39_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_FLOOD_LOW_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_39_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if (projState == PROJ_DRVG_PROJ_STATE_FLOOD_HIGH_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_39_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_IN_DIRECTION_E);
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_23_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;


      case(INU_DEFSG_BOOT310_E):
      case(INU_DEFSG_BOOT311_E):
      case(INU_DEFSG_BOOT330_E):
      case(INU_DEFSG_BOOT340_E):
      case(INU_DEFSG_BOOT360_E):
      {
         if (projState == PROJ_DRVG_PROJ_STATE_CLEAR_E)
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_SET_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else if ((projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E))
         {
            PROJ_DRVP_configGpio(GPIO_DRVG_GPIO_59_E,GPIO_DRVG_GPIO_STATE_CLEAR_E,GPIO_DRVG_OUT_DIRECTION_E);
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }

      break;

      default:
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         break;
   }
   if (ret == PROJ__RET_SUCCESS)
      PROJ_DRVP_projState = projState;
   return ret;
}

static ERRG_codeE PROJ_DRVP_checkSupport(PROJ_DRVG_projSetValParamsT *params)
{
   INU_DEFSG_moduleTypeE model = GME_MNGRG_getModelType();
   PROJ_DRVG_projStateE projState = params->projState;
   ERRG_codeE ret = PROJ__RET_SUCCESS;

   switch (model)
   {
      case(INU_DEFSG_BOOT0_E):
      case(INU_DEFSG_BOOT200_E):
      case(INU_DEFSG_BOOT30_E):
      case(INU_DEFSG_BOOT60_E):
      case(INU_DEFSG_BOOT65_E):
      case(INU_DEFSG_BOOT66_E):
      case(INU_DEFSG_BOOT70_E):
      case(INU_DEFSG_BOOT80_E):
      case(INU_DEFSG_BOOT90_E):
      {
         if ((projState == PROJ_DRVG_PROJ_STATE_CLEAR_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E))
         {
            ret = PROJ__RET_SUCCESS;
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;

      case(INU_DEFSG_BOOT50_E):
      case(INU_DEFSG_BOOT51_E):
      case(INU_DEFSG_BOOT52_E):
      case(INU_DEFSG_BOOT53_E):
      {
         if ((projState == PROJ_DRVG_PROJ_STATE_CLEAR_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E) ||
             (projState == PROJ_DRVG_PROJ_STATE_FLOOD_LOW_E) || (projState == PROJ_DRVG_PROJ_STATE_FLOOD_HIGH_E))
         {
            ret = PROJ__RET_SUCCESS;
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;

      case(INU_DEFSG_BOOT310_E):
      case(INU_DEFSG_BOOT311_E):
      case(INU_DEFSG_BOOT330_E):
      case(INU_DEFSG_BOOT340_E):
      case(INU_DEFSG_BOOT360_E):
      {
         if ((projState == PROJ_DRVG_PROJ_STATE_CLEAR_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E) || (projState == PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E))
         {
            ret = PROJ__RET_SUCCESS;
         }
         else
         {
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         }
      }
      break;

      default:
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
         break;
   }
   return ret;
}


static ERRG_codeE PROJ_DRVP_setProjVal(PROJ_DRVG_projSetValParamsT *params)
{
   ERRG_codeE ret = PROJ__RET_SUCCESS;

   switch (params->projNum)
   {
         case PROJ_DRVG_PROJ_0_E:
            ret = PROJ_DRVP_handle_proj0(params->projState);
            break;
         default:
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
            break;
   }

   return ret;
}

static ERRG_codeE PROJ_DRVP_getProjVal(PROJ_DRVG_projGetValParamsT *params)
{
   ERRG_codeE ret = PROJ__RET_SUCCESS;

   switch (params->projNum)
   {
         case PROJ_DRVG_PROJ_0_E:
            params->projState = PROJ_DRVP_projState;
            break;
         default:
            ret = PROJ__ERR_PROJ_NUM_NOT_SUPPORTED;
            break;
   }

   return ret;
}

static ERRG_codeE PROJ_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, PROJ_DRVG_openParamsT *params)
{
   ERRG_codeE retCode = PROJ__RET_SUCCESS;
   INU_DEFSG_moduleTypeE modelType =  GME_MNGRG_getModelType();

   FIX_UNUSED_PARAM_WARN(deviceId);

   *handleP = (IO_HANDLE)NULL;

   if(PROJ_DRVP_projDeviceDesc.moduleStatus == PROJ_DRVP_MODULE_STATUS_CLOSE_E)
   {
      PROJ_DRVG_projSetValParamsT initParams;

      if (modelType == INU_DEFSG_BOOT40_E)
      {
         projfd = open("/dev/as3648_0",O_RDWR);
         if (projfd<0 )
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "open drvice as3648 fail!!!\n");
            return ERR_IO_ERROR;
         }

      }

      PROJ_DRVP_projDeviceDesc.moduleStatus = PROJ_DRVP_MODULE_STATUS_OPEN_E;
      *handleP = (IO_HANDLE)(&PROJ_DRVP_projDeviceDesc);
      GME_DRVP_deviceDesc.deviceBaseAddress  = params->gmeBaseAddress;
      //set init value
      initParams.projNum = PROJ_DRVG_PROJ_0_E;
      initParams.projState = PROJ_DRVG_PROJ_STATE_PATTERN_LOW_E;
      PROJ_DRVP_setProjVal(&initParams);
   }
   else // if(GME_DRVP_deviceDesc.moduleStatus == GME_DRVP_MODULE_STATUS_CLOSE_E)
   {
      retCode = PROJ__ERR_OPEN_FAIL_DRV_NOT_CLOSED;
      LOGG_PRINT(LOG_ERROR_E, retCode, "open PROJ driver fail!!!\n");
   }

   return PROJ__RET_SUCCESS;
}

static ERRG_codeE PROJ_DRVP_close(IO_HANDLE handle)
{
   ERRG_codeE  retVal = PROJ__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
   close(projfd);
   PROJ_DRVP_projDeviceDesc.moduleStatus = PROJ_DRVP_MODULE_STATUS_CLOSE_E;
   return(retVal);
}



static ERRG_codeE PROJ_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   ERRG_codeE  projIoctlRetCode = PROJ__RET_SUCCESS;
   UINT16      ioctl;

   if(((PROJ_DRVP_projDeviceDescT*)handle)->moduleStatus == PROJ_DRVP_MODULE_STATUS_OPEN_E)
   {
      ioctl = IO_CMD_GET_IOCTL(cmd);
      if (IO_CMD_IS_DEVICE_ID(cmd, IO_PROJ_0_E))
      {
         if(handle == &PROJ_DRVP_projDeviceDesc)
         {
            projIoctlRetCode = (((PROJ_DRVP_projDeviceDescT*)handle))->ioctlFuncList[ioctl](argP);
         }
         else
         {
            projIoctlRetCode = PROJ__ERR_IOCTL_FAIL_WRONG_HANDLE;
         }
      }
      else  // if (IO_CMD_IS_DEVICE_ID(cmd, IO_PROJ_E))
      {
         projIoctlRetCode = PROJ__ERR_IOCTL_FAIL_UNKNOWN_CMD_CODE;
      }
   }
   else  // if(PROJ_DRVP_deviceDesc.moduleStatus == PROJ_DRVP_MODULE_STATUS_OPEN_E)
   {
      projIoctlRetCode = PROJ__ERR_IOCTL_FAIL_DRV_NOT_OPEN;
   }

   return(projIoctlRetCode);
}


static void PROJ_DRVP_dataBaseReset(void)
{
   memset(&PROJ_DRVP_projDeviceDesc, 0, sizeof(PROJ_DRVP_projDeviceDesc));
}

static void PROJ_DRVP_dataBaseInit(void)
{

   PROJ_DRVP_projDeviceDesc.moduleStatus       = PROJ_DRVP_MODULE_STATUS_CLOSE_E;
   PROJ_DRVP_projDeviceDesc.ioctlFuncList      = PROJ_DRVP_ioctlFuncList;

   PROJ_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(PROJ_DRVG_OPEN_PROJ_CMD_E)]                = (PROJ_DRVP_ioctlFuncListT) PROJ_DRVP_openProj;
   PROJ_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(PROJ_DRVG_CLOSE_PROJ_CMD_E)]               = (PROJ_DRVP_ioctlFuncListT) PROJ_DRVP_closeProj;
   PROJ_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(PROJ_DRVG_SET_PROJ_VAL_CMD_E)]             = (PROJ_DRVP_ioctlFuncListT) PROJ_DRVP_setProjVal;
   PROJ_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(PROJ_DRVG_GET_PROJ_VAL_CMD_E)]             = (PROJ_DRVP_ioctlFuncListT) PROJ_DRVP_getProjVal;
   PROJ_DRVP_ioctlFuncList[IO_CMD_GET_IOCTL(PROJ_DRVG_CHECK_SUPPORT_CMD_E)]            = (PROJ_DRVP_ioctlFuncListT) PROJ_DRVP_checkSupport;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/

ERRG_codeE PROJ_DRVG_init(IO_PALG_apiCommandT *palP)
{
   ERRG_codeE projInitRetCode = PROJ__RET_SUCCESS;
   if(palP)
   {
      palP->close =  (IO_PALG_closeT)  &PROJ_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT)  &PROJ_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)   &PROJ_DRVP_open;
   }
   else
   {
      projInitRetCode = PROJ__ERR_INIT_FAIL_WRONG_PAL_HANDLE;
   }

   if(ERRG_SUCCEEDED(projInitRetCode))
   {
      PROJ_DRVP_dataBaseReset();
      PROJ_DRVP_dataBaseInit();
   }
   return projInitRetCode;
}


#ifdef __cplusplus
}
#endif

