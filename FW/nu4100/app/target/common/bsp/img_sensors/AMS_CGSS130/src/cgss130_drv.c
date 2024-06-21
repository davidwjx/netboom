/****************************************************************************
 *
 *   FileName: cgss130_drv.c
 *
 *   Author:  Elad R.
 *
 *   Date:
 *
 *   Description: AMS cgss130 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "cgss130_drv.h"
#include "cgss130_cfg_tbl.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include <unistd.h>
#include <errno.h>
#include "assert.h"


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE CGSS130_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_setFrameRate(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE CGSS130_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE CGSS130_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_close_gpio();

static ERRG_codeE CGSS130_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE CGSS130_DRVP_close(IO_HANDLE handle);
static ERRG_codeE CGSS130_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static ERRG_codeE CGSS130_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGSS130_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);


/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   CGSS130P_XSHUTDOWN_R_E,
   CGSS130P_XSHUTDOWN_L_E,
   CGSS130P_FSIN_CV_E,
   CGSS130P_XSHUTDOWN_W_E,
   CGSS130P_FSIN_CLR_E,
   CGSS130P_MAX_NUM_E
}CGSS130P_gpioNumberE;
int gpiosCGSS130[CGSS130P_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(CGSS130P_gpioNumberE) if (gpiosCGSS130[CGSS130P_gpioNumberE] != -1)

/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define CGSS130_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define CGSS130_DRVP_CHANGE_ADDR_SLEEP_TIME                            (1*1000)   // 1ms
#define CGSS130_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define CGSS130_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define CGSS130_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define CGSS130_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define CGSS130_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define CGSS130_DRVP_16_BIT_ACCESS                                     (16)
#define CGSS130_DRVP_8_BIT_ACCESS                                      (8)
#define CGSS130_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)
#define CGSS130_DRVP_SYS_CLK                                           (108)//(0x66FF300)//108M
#define ROW_BLANK_30FPS_VAL                                            (0xFEA)
#define VTS_30FPS_VAL                                                  (0xFF0)

//#define CGSS130_DRVP_GROUP_MODE_EXP_CHANGE
///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define CGSS130_DRVP_SC_SLEEP_MODE_ADDRESS                             (0x0100)//
#define CGSS130_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)//
//#define CGSS130_DRVP_SC_SLEEP_ADDRESS                                  (0x106)//
//#define CGSS130_DRVP_SC_SCCB_ID1_ADDRESS                               (0x0109)
//#define CGSS130_DRVP_SC_SCCB_ID2_ADDRESS                               (0x302B)
#define CGSS130_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x3107)//
#define CGSS130_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x3108)//

#define CGSS130_DRVP_LINE_LENGTH_HIGH                                  (0x320c)//
#define CGSS130_DRVP_LINE_LENGTH_LOW                                   (0x320d)//

#define CGSS130_DRVP_BLANK_ROW_HIGH                                    (0x3228)//
#define CGSS130_DRVP_BLANK_ROW_LOW                                     (0x3229)//

#define CGSS130_DRVP_EXP_TIME_1_ADDRESS                                (0x3e00)//
#define CGSS130_DRVP_EXP_TIME_2_ADDRESS                                (0x3e01)//
#define CGSS130_DRVP_EXP_TIME_3_ADDRESS                                (0x3e02)//

/*
#define CGSS130_DRVP_SC_CLKRST6_ADDRESS                                (0x301C)
#define CGSS130_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define CGSS130_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define CGSS130_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define CGSS130_DRVP_GRP_SW_CTRL_ADDRESS                               (0x320B)

#define CGSS130_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define CGSS130_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define CGSS130_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)
#define CGSS130_DRVP_AEC_GAIN_1_ADDRESS                                (0x350A)
#define CGSS130_DRVP_AEC_GAIN_2_ADDRESS                                (0x350B)*/
#define CGSS130_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define CGSS130_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define CGSS130_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x320E)//
#define CGSS130_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x320F)//
/*
#define CGSS130_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define CGSS130_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define CGSS130_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)

#define CGSS130_DRVP_PLL2_PRE_DIV_ADDRESS                              (0x3098)
#define CGSS130_DRVP_PLL2_MULTIPLIER_ADDRESS                           (0x3099)
#define CGSS130_DRVP_PLL2_DIVIDER_ADDRESS                              (0x309D)
#define CGSS130_DRVP_PLL2_SYS_DIVIDER_ADDRESS                          (0x309A)*/

#define CGSS130_DRVP_STROBE_EN_ADDRESS                                 (0x3361)//
/*#define CGSS130_DRVP_STROBE_POLARITY_ADDRESS                           (0x3B96)
#define CGSS130_DRVP_STROBE_OFFSET_ADDRESS                             (0x3B8B)
#define CGSS130_DRVP_STROBE_DURATION_HIGH_ADDRESS                      (0x3B8E)
#define CGSS130_DRVP_STROBE_DURATION_LOW_ADDRESS                       (0x3B8F)
#define CGSS130_DRVP_STROBE_MASK_ADDRESS                               (0x3B81)
#define CGSS130_DRVP_PWM_CTRL_29_ADDRESS                               (0x3929)
#define CGSS130_DRVP_PWM_CTRL_2A_ADDRESS                               (0x392A)*/


/*#define CGSS130_DRVP_LOWPWR00_ADDRESS                                  (0x3C00)
#define CGSS130_DRVP_LOWPWR01_ADDRESS                                  (0x3C01)
#define CGSS130_DRVP_LOWPWR02_ADDRESS                                  (0x3C02)
#define CGSS130_DRVP_LOWPWR03_ADDRESS                                  (0x3C03)
#define CGSS130_DRVP_LOWPWR04_ADDRESS                                  (0x3C04)
#define CGSS130_DRVP_LOWPWR05_ADDRESS                                  (0x3C05)
#define CGSS130_DRVP_LOWPWR06_ADDRESS                                  (0x3C06)
#define CGSS130_DRVP_LOWPWR07_ADDRESS                                  (0x3C07)
#define CGSS130_DRVP_LOWPWR0E_ADDRESS                                  (0x3C0E)
#define CGSS130_DRVP_LOWPWR0F_ADDRESS                                  (0x3C0F)
#define CGSS130_DRVP_ANA_CORE_6_ADDRESS                                (0x3666)*/

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define CGSS130_DRVP_SC_CHIP_ID_LOW                                    (0x32)
#define CGSS130_DRVP_SC_CHIP_ID_HIGH                                   (0x1)
#define CGSS130_DRVP_I2C_DEFAULT_ADDRESS                               (0x62)//ELAD?
#define CGSS130_DRVP_I2C_MASTER_ADDRESS                                (CGSS130_DRVP_I2C_DEFAULT_ADDRESS)
#define CGSS130_DRVP_I2C_SLAVE_ADDRESS                                 (0x60)//ELAD?
//#define CGSS130_DRVP_I2C_GLOBAL_ADDRESS                                (0x62)//ELAD?


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  CGSS130_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   CGSS130_DRVP_deviceDesc[6];//[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        CGSS130_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      CGSS130_DRVP_resetSensors,            \
                                                                                                      CGSS130_DRVP_initSensor,              \
                                                                                                      CGSS130_DRVP_configSensors,           \
                                                                                                      CGSS130_DRVP_startSensors,            \
                                                                                                      CGSS130_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,       \
                                                                                                      CGSS130_DRVP_getDeviceId,             \
                                                                                                      CGSS130_DRVP_loadPresetTable,         \
                                                                                                      CGSS130_DRVP_setFrameRate,            \
                                                                                                      CGSS130_DRVP_setExposureTime,         \
                                                                                                      CGSS130_DRVP_getExposureTime,         \
                                                                                                      CGSS130_DRVP_setExposureMode,         \
                                                                                                      CGSS130_DRVP_setImgOffsets,           \
                                                                                                      CGSS130_DRVP_getAvgBrighness,         \
                                                                                                      CGSS130_DRVP_setOutFormat,            \
                                                                                                      CGSS130_DRVP_setPowerFreq,            \
                                                                                                      CGSS130_DRVP_setStrobe,               \
                                                                                                      CGSS130_DRVP_setAutoExposureAvgLuma,  \
                                                                                                      CGSS130_DRVP_setGain,                 \
                                                                                                      CGSS130_DRVP_getGain,                 \
                                                                                                      CGSS130_DRVP_trigger,                 \
                                                                                                      CGSS130_DRVP_mirrorFlip,              \
                                                                                                      CGSS130_DRVP_powerup,                 \
                                                                                                      CGSS130_DRVP_powerdown,               \
                                                                                                      CGSS130_DRVP_changeSensorAddress,     \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub};

static UINT16 CGSS130_DRVP_lineLengthPclk;
static UINT16 CGSS130_DRVP_vts;
static UINT16 CGSS130_DRVP_hts;


/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static ERRG_codeE CGSS130_DRVP_writeReg(IO_HANDLE handle, UINT16 address, UINT32 dataVal, UINT8 accessSize)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;


   if (deviceDescriptorP->isSensorGroup)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "CGSS130_DRVP_writeReg group. address 0x%x val 0x%x sensor address 0x%x\n",address,dataVal,CGSS130_DRVP_I2C_MASTER_ADDRESS);
      retVal = I2C_DRVG_write(0, address, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE,accessSize, dataVal,CGSS130_DRVP_I2C_MASTER_ADDRESS ,1000/*GEN_SENSOR_DRVP_I2C_DELAY*/);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      retVal = I2C_DRVG_write(1, address, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE,accessSize, dataVal,CGSS130_DRVP_I2C_MASTER_ADDRESS ,1000/*GEN_SENSOR_DRVP_I2C_DELAY*/);
   }
   else
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "CGSS130_DRVP_writeReg i2cInstanceId %d address 0x%x val 0x%x sensor address 0x%x\n",deviceDescriptorP->i2cInstanceId,address,dataVal,deviceDescriptorP->sensorAddress);
      retVal = I2C_DRVG_write(deviceDescriptorP->i2cInstanceId, address, GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE,accessSize, dataVal, deviceDescriptorP->sensorAddress,1000/*GEN_SENSOR_DRVP_I2C_DELAY*/);
   }

   return retVal;
   /*for (i...
   if (deviceDescriptorP->sensors[i])
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP->sensors[i], CGSS130_DRVP

   if no write hhapened
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,*/
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_getSysClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_getSysClk(IO_HANDLE handle, UINT32 *pixClkP)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   *pixClkP = CGSS130_DRVP_SYS_CLK;

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setStrobeDuration(IO_HANDLE handle, UINT16 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   #if 0
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum,pixClk = 0;
#ifdef CGSS130_DRVP_GROUP_MODE_EXP_CHANGE
   UINT16                              strobeStartPoint;
#endif

   CGSS130_DRVP_getSysClk(handle, &pixClk);
   strobeUpTicksNum = ((usec * pixClk) + (CGSS130_DRVP_lineLengthPclk - 1)) / (CGSS130_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_STROBE_DURATION_HIGH_ADDRESS, (strobeUpTicksNum & 0xFFFF), CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#ifdef CGSS130_DRVP_GROUP_MODE_EXP_CHANGE
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = CGSS130_DRVP_vts - usec - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_PWM_CTRL_29_ADDRESS, (strobeStartPoint & 0xFFFF), CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   LOGG_PRINT(LOG_INFO_E,NULL,"$$$$$$$$strobeUpTicksNum = %d, usec = %d, lineLengthPclk = %d, pixClk = %d\n", strobeUpTicksNum, usec, CGSS130_DRVP_lineLengthPclk, pixClk);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
   /*

   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_STROBE_POLARITY_ADDRESS, 0x40, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_STROBE_OFFSET_ADDRESS, 0x0, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_STROBE_MASK_ADDRESS, 0xFF, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_STROBE_EN_ADDRESS, 0x08, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
*/
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   LOGG_PRINT(LOG_INFO_E, NULL, "reset Sensor (type = %d, address = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);

   return SENSOR__RET_SUCCESS;
}



/****************************************************************************
*
*  Function Name: CGSS130_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV9282 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   /*LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_SC_SCCB_ID2_ADDRESS, address, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   deviceDescriptorP->sensorAddress = (UINT8)address;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_SC_SCCB_ID2_ADDRESS, address, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(CGSS130_DRVP_CHANGE_ADDR_SLEEP_TIME);


   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, CGSS130_DRVP_SC_SCCB_ID2_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   if (config.accessRegParams.data != address)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Error changing address! (%x) (requested %x)\n",config.accessRegParams.data,address);
   }
*/
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_powerup
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV9282 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   LOGG_PRINT(LOG_INFO_E,NULL,"power up (type = %d, instId %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->i2cInstanceId,deviceDescriptorP->sensorAddress,handle);

   FIX_UNUSED_PARAM_WARN(pParams);


   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(CGSS130_DRVP_RESET_SLEEP_TIME);

   // reset sensor(s)
   retVal = CGSS130_DRVP_writeReg(handle,CGSS130_DRVP_SC_SOFTWARE_RESET_ADDRESS,1,CGSS130_DRVP_SENSOR_ACCESS_2_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor(s)
   retVal = CGSS130_DRVP_writeReg(handle,CGSS130_DRVP_SC_SOFTWARE_RESET_ADDRESS,0,CGSS130_DRVP_SENSOR_ACCESS_2_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //workaround
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, CGSS130_DRVP_SC_CHIP_ID_LOW_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (config.accessRegParams.data != CGSS130_DRVP_SC_CHIP_ID_LOW)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_powerup
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV9282 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   gpioParams.gpioNum = deviceDescriptorP->fsinGpio;
   gpioParams.val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

   gpioParams.val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
   //deviceDescriptorP->sensorAddress = CGSS130_DRVP_I2C_MASTER_ADDRESS;

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
      params->triggerModeParams.gpioBitmap |= (((UINT64)1) << deviceDescriptorP->fsinGpio);
      /* FSIN pulse width should be no shorter than 5 input clock cycles. */
      params->triggerModeParams.pulseTime = 300;
      params->triggerModeParams.isTriggerSupported = 1;

   }

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   retVal = CGSS130_DRVP_writeReg(handle, CGSS130_DRVP_SC_SLEEP_MODE_ADDRESS,0x1,CGSS130_DRVP_SENSOR_ACCESS_1_BYTE);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GPIO_DRVG_gpioSetValParamsT         gpioParams;

   FIX_UNUSED_PARAM_WARN(pParams);

  // gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
  // gpioParams.gpioNum = 38;
  // IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   //OS_LYRG_usleep(500);
   //gpioParams.val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   //IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);



   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGSS130_DRVP_SC_SLEEP_MODE_ADDRESS, 0x0, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   retVal = CGSS130_DRVP_writeReg(handle, CGSS130_DRVP_SC_SLEEP_MODE_ADDRESS,0x0,CGSS130_DRVP_SENSOR_ACCESS_1_BYTE);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT32                              regVal[4];
   GEN_SENSOR_DRVG_getDeviceIdParamsT  *deviceIdParamsP = (GEN_SENSOR_DRVG_getDeviceIdParamsT*)pParams;
//   GEN_SENSOR_DRVG_sensorParametersT   config;
//   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(handle);

/*
   if(deviceIdParamsP->cmdCtrl == GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_SLAVE_E)
   {
      handle = deviceDescriptorP->slaveHandle;
   }

   config.accessRegParams.rdWr        = READ_ACCESS_E;
   config.accessRegParams.accessSize  = CGSS130_DRVP_SENSOR_ACCESS_2_BYTE;
   config.accessRegParams.regAdd      = 0x31F4;
   retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &config);

   if(ERRG_SUCCEEDED(retVal))
   {
      regVal[0] = config.accessRegParams.data;
      config.accessRegParams.regAdd = 0x31F6;
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &config);
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      regVal[1] = config.accessRegParams.data;
      config.accessRegParams.regAdd = 0x31F8;
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &config);
   }

   if(ERRG_SUCCEEDED(retVal))
   {
      regVal[2] = config.accessRegParams.data;
      config.accessRegParams.regAdd = 0x31Fa;
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &config);
   }*/

   regVal[0] = 0;
   regVal[1] = 0;
   regVal[2] = 0;
   regVal[3] = 0;

   if(ERRG_SUCCEEDED(retVal))
   {
//      regVal[3] = config.accessRegParams.data;
      sprintf((char *)deviceIdParamsP->sensorIdStr, "%04x%04x%04x%04x", (UINT16)regVal[3], (UINT16)regVal[2], (UINT16)regVal[1], (UINT16)regVal[0]);
      deviceIdParamsP->sensorId[0]  = (UINT16)regVal[3];
      deviceIdParamsP->sensorId[1]  = (UINT16)regVal[2];
      deviceIdParamsP->sensorId[2]  = (UINT16)regVal[1];
      deviceIdParamsP->sensorId[3]  = (UINT16)regVal[0];
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr=NULL;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   *params              = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   UINT16                              regNum = 0;
   GEN_SENSOR_DRVG_exposureTimeCfgT    expParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   LOGG_PRINT(LOG_INFO_E, NULL, "tableType: %d, res %d\n",deviceDescriptorP->tableType,params->loadTableParams.sensorResolution);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      //TODO:?
   }
   else if (deviceDescriptorP->tableType == INU_DEFSG_STEREO_E)
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = CGSS130_CFG_TBLG_stereo_binning_configTable;
         regNum = sizeof(CGSS130_CFG_TBLG_stereo_binning_configTable)
                  /sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_HD_E)
      {
         sensorTablePtr = CGSS130_CFG_TBLG_stereo_full_configTable;
         regNum = sizeof(CGSS130_CFG_TBLG_stereo_full_configTable)
                  /sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         //TODO
      }
   }
   else if (deviceDescriptorP->tableType == 2)//4lanes
   {
      sensorTablePtr = CGSS130_CFG_TBLG_4lanes_full_configTable;
      regNum = sizeof(CGSS130_CFG_TBLG_4lanes_full_configTable)
               /sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {

   }


   retVal = GEN_SENSOR_DRVG_regTableLoad(deviceDescriptorP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
   if (ERRG_SUCCEEDED(retVal))
   {
      //validate vts value and row blank value. we are using them on 'setFrameRate' function
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_TIMING_VTS_HIGH_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      if (config.accessRegParams.data != VTS_30FPS_VAL)
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"VTS value 0x%x is changed from the original value 0x%x\n",config.accessRegParams.data,VTS_30FPS_VAL);
         assert(0);
      }
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_BLANK_ROW_HIGH, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      if (config.accessRegParams.data != ROW_BLANK_30FPS_VAL)
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"row blank value 0x%x is changed from the original value 0x%x\n",config.accessRegParams.data,ROW_BLANK_30FPS_VAL);
         assert(0);
      }
   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              pixClk = 0,vts = 0;
   UINT32   rowBlank = 0;

   // this sensor works in trigger mode. frame rate is controlled by trigger. according of AMS configuration we saw tha those registers are modified (vts and row blank).
   //we change their values based on the values of the 30FPS, in the appropriate ratio of the requested frame rate and 30FPS
   retVal = CGSS130_DRVP_writeReg(handle, CGSS130_DRVP_BLANK_ROW_HIGH, ROW_BLANK_30FPS_VAL * 30 / params->setFrameRateParams.frameRate, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_BLANK_ROW_HIGH, ROW_BLANK_30FPS_VAL * 30 / params->setFrameRateParams.frameRate, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   retVal = CGSS130_DRVP_writeReg(handle, CGSS130_DRVP_TIMING_VTS_HIGH_ADDRESS, VTS_30FPS_VAL * 30 / params->setFrameRateParams.frameRate, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_TIMING_VTS_HIGH_ADDRESS, VTS_30FPS_VAL * 30 / params->setFrameRateParams.frameRate, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E,NULL,"req fps %d set val: row blank 0x%x vts 0x%x\n",params->setFrameRateParams.frameRate,ROW_BLANK_30FPS_VAL * 30 / params->setFrameRateParams.frameRate,VTS_30FPS_VAL * 30 / params->setFrameRateParams.frameRate);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
   UINT32                              expReg =0;
   UINT32                              lineLength =0;
   UINT16                               tmp =0;
#ifdef CGSS130_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32                              groupHold;
#endif
   UINT32                              pixClk = 0;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   //EXP = exp_reg *HTS/(SYSCLK *16)

   //get line length - HTS
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_LINE_LENGTH_HIGH, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   lineLength |= ((config.accessRegParams.data & 0xFF) << 8);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_LINE_LENGTH_LOW, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   lineLength |= (config.accessRegParams.data & 0xFF);

   //get sysClk
   CGSS130_DRVP_getSysClk(handle, &pixClk);

   //expReg = (expReq * pixClk * 16) / linelength
   expReg = (params->exposureTime * pixClk * 16) / lineLength;

   tmp = expReg & 0xFF;
   CGSS130_DRVP_writeReg(handle,CGSS130_DRVP_EXP_TIME_3_ADDRESS,tmp,CGSS130_DRVP_SENSOR_ACCESS_1_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_EXP_TIME_3_ADDRESS, tmp, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   tmp = 0;
   tmp = expReg & 0xFF00;
   tmp = tmp >> 8;
   CGSS130_DRVP_writeReg(handle,CGSS130_DRVP_EXP_TIME_2_ADDRESS,tmp,CGSS130_DRVP_SENSOR_ACCESS_1_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_EXP_TIME_2_ADDRESS, tmp, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   tmp = 0;
   tmp = expReg & 0xF0000;
   tmp = tmp >> 16;
   CGSS130_DRVP_writeReg(handle,CGSS130_DRVP_EXP_TIME_1_ADDRESS,tmp,CGSS130_DRVP_SENSOR_ACCESS_1_BYTE);
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_EXP_TIME_1_ADDRESS, tmp, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              pixClk;
   UINT32                              lineLength =0;
   UINT32                              expReg =0;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   params->exposureTime = 0;
//exp_reg *HTS/(SYSCLK *16)

//get HTS
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_LINE_LENGTH_HIGH, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   lineLength |= ((config.accessRegParams.data & 0xFF) << 8);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_LINE_LENGTH_LOW, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   lineLength |= (config.accessRegParams.data & 0xFF);

   //get exp_reg
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_EXP_TIME_1_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   expReg |= ((config.accessRegParams.data & 0xF) << 16);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_EXP_TIME_2_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   expReg |= ((config.accessRegParams.data & 0xFF) << 8);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_EXP_TIME_3_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   expReg |= ((config.accessRegParams.data & 0xFF) );


   //Trow  = 1 / fps / VTS
   //total exposure = Trow * (number of lines to perform exposure)
   //GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_TIMING_VTS_HIGH_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //UINT16 vts = config.accessRegParams.data;
   //UINT32 trow = 1000000 / 10 / vts; //TODO: change 10-->FPS
   //params->exposureTime = trow * ( expReg / 16 );
   //printf("vts 0x%x, trow %d, expReg 0x%x, exposure %d\n",vts,trow,expReg,params->exposureTime);


   CGSS130_DRVP_getSysClk(handle, &pixClk);
   params->exposureTime = (expReg * lineLength) / (pixClk*16);
   LOGG_PRINT(LOG_INFO_E,NULL,"exposure time = %d, hts = 0x%x, expReg 0x%x, pixClk = %d, context = %d\n",params->exposureTime,lineLength, expReg, pixClk,params->context);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   #if 0
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   if (params->analogGain > 0x3FF)
   {
      //clip to max value
      params->analogGain = 0x3FF;
   }
   params->digitalGain = 0; //no digital gain in this sensor

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_AEC_GAIN_1_ADDRESS, (params->analogGain >> 8) & 0x3, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_AEC_GAIN_2_ADDRESS, params->analogGain & 0xFF, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   #if 0
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT            *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   params->analogGain = 0;
   params->digitalGain = 0; //no digital gain in this sensor

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_AEC_GAIN_2_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   params->analogGain = config.accessRegParams.data & 0xFF;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_AEC_GAIN_1_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   params->analogGain |= (config.accessRegParams.data & 0x3) << 8;
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   retVal = CGSS130_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_setAutoExposureAvgLuma
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;

   FIX_UNUSED_PARAM_WARN(pParams);

   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   gpioParams.gpioNum = deviceDescriptorP->fsinGpio;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   OS_LYRG_usleep(500);
   gpioParams.val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   #if 0
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGSS130_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, CGSS130_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);
   #endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{

   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   BOOL                                foundNewInstance  = FALSE;
   UINT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   GME_DRVG_unitClkDivT                gmeClkConfig;

   FIX_UNUSED_PARAM_WARN(deviceId);
   LOGG_PRINT(LOG_INFO_E,NULL,"open sensor function i2cInstanceId %d sensorClk %d powerGpioMaster %d is sensor group %d\n",pOpenParams->i2cInstanceId,pOpenParams->sensorClk,pOpenParams->powerGpioMaster,pOpenParams->isSensorGroup);

   for(sensorInstanceId = 0; sensorInstanceId < CGSS130_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
   {
      deviceDescriptorP = &CGSS130_DRVP_deviceDesc[sensorInstanceId];
      if(deviceDescriptorP->deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
         foundNewInstance = TRUE;
         break;
      }
   }

   if(foundNewInstance == TRUE)
   {
      deviceDescriptorP->deviceStatus     = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
      #if 0
      if (pOpenParams->i2cInstanceId == 0)//TODO: pass i2c address from XML
      {
         deviceDescriptorP->sensorAddress    = CGSS130_DRVP_I2C_MASTER_ADDRESS;
      }
      else if (pOpenParams->i2cInstanceId == 1)
      {
         deviceDescriptorP->sensorAddress    = CGSS130_DRVP_I2C_SLAVE_ADDRESS;
      }
      #else
      deviceDescriptorP->sensorAddress = CGSS130_DRVP_I2C_MASTER_ADDRESS;
      #endif
      deviceDescriptorP->ioctlFuncList    = CGSS130_DRVP_ioctlFuncList;
      deviceDescriptorP->i2cInstanceId    = pOpenParams->i2cInstanceId;
      deviceDescriptorP->i2cSpeed         = pOpenParams->i2cSpeed;
      deviceDescriptorP->sensorType       = pOpenParams->sensorType;
      deviceDescriptorP->tableType        = pOpenParams->tableType;
      deviceDescriptorP->powerGpioMaster  = pOpenParams->powerGpioMaster;
      deviceDescriptorP->fsinGpio         = pOpenParams->fsinGpio;
      deviceDescriptorP->sensorClk        = pOpenParams->sensorClk;
      deviceDescriptorP->isSensorGroup    = pOpenParams->isSensorGroup;

      GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->powerGpioMaster);
      GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->fsinGpio);
      // GME_DRVG_HW_AUDIO_CLK_EN_E case need to open pll
      if (deviceDescriptorP->sensorClk==GME_DRVG_HW_AUDIO_CLK_EN_E)
      {
         GME_DRVG_pllConfigT pllConfig;
         pllConfig.bypass = 1;
         pllConfig.dacpd = 0;
         pllConfig.dsmpd = 0;
         pllConfig.fbdiv = 134;
         pllConfig.frac = 0;
         pllConfig.on = 1;
         pllConfig.postdiv1 = 2;
         pllConfig.postdiv2 = 1;
         pllConfig.refdiv = 4;
         GME_DRVG_configAudioPll(&pllConfig);
      }
      GEN_SENSOR_DRVP_setSensorRefClk(deviceDescriptorP->sensorClk, pOpenParams->sensorClkDiv);

      *handleP = (IO_HANDLE)deviceDescriptorP;
      /*if(pOpenParams->sensorType == INU_DEFSG_SENSOR_TYPE_STEREO_E)
      {
         printf("is stereo\n");
         deviceDescriptorP->sensorAddress = CGSS130_DRVP_I2C_GLOBAL_ADDRESS;
      }
      else
      {
         printf("is not stereo\n");
      }*/

      LOGG_PRINT(LOG_DEBUG_E, NULL, "Set CS_REF_CLK div. to %d\n", gmeClkConfig.div);
      LOGG_PRINT(LOG_INFO_E,NULL,"open sensor (type = %d, address = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);
   }
   else
   {
      retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
   }

   return(retCode);
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd, size_read, i=0, j=CGSS130P_MAX_NUM_E;
   char        buf[128];
   GPIO_DRVG_gpioOpenParamsT     params;
   char *fileName;
   if (sensorType==INU_DEFSG_SENSOR_TYPE_SINGLE_E)
      i=CGSS130P_XSHUTDOWN_W_E;
   else
      j=CGSS130P_XSHUTDOWN_W_E;
   for (; i < j; i++)
   {
      switch ( i )
      {
         case CGSS130P_XSHUTDOWN_R_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_R";
         break;
         case CGSS130P_XSHUTDOWN_L_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_L";
         break;
         case CGSS130P_FSIN_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/FSIN_CV";
         break;
         case CGSS130P_XSHUTDOWN_W_E:
            fileName = "/proc/device-tree/sensor_on/webcam/XSHUTDOWN_W";
         break;
         case CGSS130P_FSIN_CLR_E:
            fileName = "/proc/device-tree/sensor_on/webcam/FSIN_CLR";
         break;
      }
      fd = open( fileName, O_RDONLY );
      if (fd < 0)
      {
         retCode = SENSOR__ERR_CLOSE_DEVICE_FAIL;
         LOGG_PRINT(LOG_DEBUG_E, retCode, "fail opening a file descriptor, fileName = %s, fd = %d, errno = %s\n",fileName,fd,strerror(errno));
      }
      else
      {
         memset(buf,0,sizeof(buf));
         size_read=read(fd, buf, sizeof(buf));
         if (size_read < 0)
            LOGG_PRINT(LOG_DEBUG_E, NULL, "reading %s! errno = %s\n",fileName,strerror(errno));
         else
         {
            gpiosCGSS130[i]=atoi(buf);
            if(gpiosCGSS130[i]!=-1)
            {
               params.gpioNum = gpiosCGSS130[i];
               retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
            }
         }
         LOGG_PRINT(LOG_DEBUG_E, NULL, "GPIO: Close GPIO %d\n", params.gpioNum);
         if (close(fd) < 0)
         {
            retCode = SENSOR__ERR_CLOSE_DEVICE_FAIL;
            LOGG_PRINT(LOG_ERROR_E, retCode, "fail closing a file descriptor, fileName = %s, fd = %d, errno = %s\n",fileName,fd,strerror(errno));
         }
      }
   }
   return retCode;
}


/****************************************************************************
*
*  Function Name: CGSS130_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;

   CGSS130_DRVP_powerdown(handle, NULL);
   CGSS130_DRVP_close_gpio(deviceDescriptorP->sensorType);

   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: CGSS130_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
static ERRG_codeE CGSS130_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: CGSS130_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGSS130 sensor driver
*
****************************************************************************/
ERRG_codeE CGSS130_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(CGSS130_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < CGSS130_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
      {
         deviceDescriptorP = &CGSS130_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = sensorInstanceId;//I2C_DRVG_I2C_INSTANCE_0_E;//
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_AMS_CGSS130_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->isSensorGroup = 0;
      }
      CGSS130_DRVP_isDrvInitialized = TRUE;
   }


   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &CGSS130_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &CGSS130_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &CGSS130_DRVP_open;
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

