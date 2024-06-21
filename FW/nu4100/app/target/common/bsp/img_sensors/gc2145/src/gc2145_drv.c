/****************************************************************************
 *
 *   FileName: GC2145_drv.c
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: GC2145 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "gc2145_drv.h"
#include "gc2145_cfg_tbl.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include <unistd.h>
#include <errno.h>

/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE GC2145_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_setFrameRate(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE GC2145_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE GC2145_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_open_direct_gpio(INU_DEFSG_sensorTypeE   sensorType);
static ERRG_codeE GC2145_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE GC2145_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE GC2145_DRVP_close(IO_HANDLE handle);
static ERRG_codeE GC2145_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static ERRG_codeE GC2145_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE GC2145_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);



/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   GC_2145P_XSHUTDOWN_R_E,
   GC_2145P_XSHUTDOWN_L_E,
   GC_2145P_FSIN_CV_E,
   GC_2145P_nRESET_CV_E,
   GC_2145P_XSHUTDOWN_W_E,
   GC_2145P_FSIN_CLR_E,
   GC_2145P_MAX_NUM_E
}GC_2145P_gpioNumberE;
int gpiosGC2145[GC_2145P_MAX_NUM_E] = {-1,-1,-1,-1,-1,-1};

#define CHECK_GPIO(GC_2145P_gpioNumberE) if (gpiosGC2145[GC_2145P_gpioNumberE] != -1)

/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define GC2145_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define GC2145_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define GC2145_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define GC2145_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define GC2145_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define GC2145_DRVP_16_BIT_ACCESS                                     (16)
#define GC2145_DRVP_8_BIT_ACCESS                                      (8)
#define GC2145_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)

///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define GC2145_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define GC2145_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0xfe)
#define GC2145_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0xf0)
#define GC2145_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0xf1)

#define GC2145_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define GC2145_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define GC2145_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)
#define GC2145_DRVP_AEC_GAIN_1_ADDRESS                                (0x350A)
#define GC2145_DRVP_AEC_GAIN_2_ADDRESS                                (0x350B)
#define GC2145_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define GC2145_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define GC2145_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define GC2145_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define GC2145_DRVP_PLL_PRE_DIV_ADDRESS                               (0x3080)
#define GC2145_DRVP_PLL_MULTIPLIER_HIGH_ADDRESS                       (0x3081)
#define GC2145_DRVP_PLL_MULTIPLIER_LOW_ADDRESS                        (0x3082)
#define GC2145_DRVP_PLL_MIPI_DIVIDER_ADDRESS                          (0x3083)
#define GC2145_DRVP_PLL_SYS_CLK_DIVIDER_ADDRESS                       (0x3084)
#define GC2145_DRVP_PLL_DAC_DIVIDER_ADDRESS                           (0x3085)
#define GC2145_DRVP_PLL_SP_DIVIDER_ADDRESS                            (0x3086)
#define GC2145_DRVP_PLL_LANE_DIVIDER_ADDRESS                          (0x3087)
#define GC2145_DRVP_PLL_CONTROL_ADDRESS                               (0x3088)

#define GC2145_DRVP_AEC_CONTROL2_ADDRESS                              (0x3A02)

#define GC2145_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define GC2145_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)


///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define GC2145_DRVP_SC_CHIP_ID                                        (0x2145)
#define GC2145_DRVP_I2C_DEFAULT_ADDRESS                               (0x78)
#define GC2145_DRVP_I2C_MASTER_ADDRESS                                (GC2145_DRVP_I2C_DEFAULT_ADDRESS)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////
#define GC2145_DRVP_FREQ_MODE_BIT                                     (1 << 7)

/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static   UINT16 GC2145_HBlank;
static   UINT16 GC2145_VBlank;
static   UINT16 GC2145_WinWidth;
static   UINT16 GC2145_WinHeight;
static   UINT16 GC2145_SHDelay;
static   UINT32 GC2145_RowTime;//row time = HB+SHDELAY+WINWIDTH/2 +4
static   UINT16 GC2145_Vt;//winheight-8
static   UINT16 GC2145_exp_time;
static   UINT16 GC2145_max_exp_time;
static   UINT8 GC2145_GlobalGain;
static   UINT16 GC2145_Ft;
static   UINT16 GC2145_FtNew;
static   UINT8 GC2145_framerate;
static   UINT8 GC2145_framerateInitial;
static   UINT32 GC2145_calculateF;
static   UINT16 GC2145_RowTimeNew;
static   UINT16 GC2145_VBlankNew;//13bits

static BOOL                                  GC2145_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   GC2145_DRVP_deviceDesc[GC2145_DRVP_MAX_NUMBER_SENSOR_INSTANCE];
static GEN_SENSOR_DRVG_ioctlFuncListT        GC2145_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      GC2145_DRVP_resetSensors,            \
                                                                                                      GC2145_DRVP_initSensor,              \
                                                                                                      GC2145_DRVP_configSensors,           \
                                                                                                      GC2145_DRVP_startSensors,            \
                                                                                                      GC2145_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      GC2145_DRVP_getDeviceId,             \
                                                                                                      GC2145_DRVP_loadPresetTable,         \
                                                                                                      GC2145_DRVP_setFrameRate,            \
                                                                                                      GC2145_DRVP_setExposureTime,         \
                                                                                                      GC2145_DRVP_getExposureTime,         \
                                                                                                      GC2145_DRVP_setExposureMode,         \
                                                                                                      GC2145_DRVP_setImgOffsets,           \
                                                                                                      GC2145_DRVP_getAvgBrighness,         \
                                                                                                      GC2145_DRVP_setOutFormat,            \
                                                                                                      GC2145_DRVP_setPowerFreq,            \
                                                                                                      GC2145_DRVP_setStrobe,               \
                                                                                                      GC2145_DRVP_getSensorClks,           \
                                                                                                      GC2145_DRVP_setGain,                 \
                                                                                                      GC2145_DRVP_getGain,                 \
                                                                                                      GC2145_DRVP_trigger,                 \
                                                                                                      GC2145_DRVP_mirrorFlip,              \
                                                                                                      GC2145_DRVP_powerup,                 \
                                                                                                      GC2145_DRVP_powerdown,               \
                                                                                                      GC2145_DRVP_changeSensorAddress,     \
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


static UINT32                                GC2145_DRVP_pre_div_mapTblx2[8] = {2,3,4,5,6,8,12,16};

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: GC2145_DRVP_changeSensorAddress
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
static ERRG_codeE GC2145_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_getPixClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static UINT16 GC2145_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16 pll_prediv, pll_mult, pll_sp_div, pll_mipi_div, pixClk, ref_clk, lineLengthPclk, x_out_size;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   //corsair assume pixclk is 100MHz
   deviceDescriptorP->pixClk = 100;
#if 0
   if (!deviceDescriptorP->pixClk)
   {
      UINT8  mipiDiv[4]   = {4,5,6,8};
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pixClk = ref_clk;

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_PRE_DIV_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_MULTIPLIER_LOW_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_MULTIPLIER_HIGH_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult |= (( config.accessRegParams.data ) & 0x1 << 8);


      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_SP_DIVIDER_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_sp_div = (( config.accessRegParams.data ) & 0xF) + 1;

      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_MIPI_DIVIDER_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mipi_div = ( config.accessRegParams.data ) & 0xF;

      if (pll_prediv != 0)
      {
         /* to stage as integer, we keep the mapTbl as x2 of the value. here we also x2 the sysclk to reduce the fractions*/
         pixClk = (2 * pixClk) / GC2145_DRVP_pre_div_mapTblx2[pll_prediv];
      }

      pixClk = pixClk * pll_mult;

      if (pll_sp_div != 0)
      {
         pixClk = pixClk / pll_sp_div;
      }

      pixClk = pixClk / mipiDiv[pll_mipi_div];

      deviceDescriptorP->pixClk = pixClk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll_prediv = %d, pll_mult = %d, pll_sp_div = %d, pll_mipi_div = %d, pixClk = %d, ref_clk = %d\n",
                                  pll_prediv, pll_mult, pll_sp_div, pll_mipi_div,deviceDescriptorP->pixClk,ref_clk);
   }
#endif
   return deviceDescriptorP->pixClk;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_getSysClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static UINT16 GC2145_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16 pll_prediv, pll_mult, pll_sp_div, pll_sys_div, sys_clk, ref_clk;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   //corsair assume system clock is 24MHz
   deviceDescriptorP->sysClk = 24;
#if 0
   if (!deviceDescriptorP->sysClk)
   {
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      sys_clk = ref_clk;

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_PRE_DIV_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_MULTIPLIER_LOW_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_MULTIPLIER_HIGH_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult |= (( config.accessRegParams.data ) & 0x1 << 8);


      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_SP_DIVIDER_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_sp_div = (( config.accessRegParams.data ) & 0xF) + 1;

      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_PLL_SYS_CLK_DIVIDER_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_sys_div = (( config.accessRegParams.data ) & 0xF) + 1;

      if (pll_prediv != 0)
      {
         /* to stage as integer, we keep the mapTbl as x2 of the value. here we also x2 the sysclk to reduce the fractions*/
         sys_clk = (2 * sys_clk) / GC2145_DRVP_pre_div_mapTblx2[pll_prediv];
      }

      sys_clk = sys_clk * pll_mult;

      if (pll_sp_div != 0)
      {
         sys_clk = sys_clk / pll_sp_div;
      }

      if (pll_sys_div != 0)
      {
         sys_clk = sys_clk / (pll_sys_div);
      }

      deviceDescriptorP->sysClk = sys_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll_prediv = %d, pll_mult = %d, pll_sp_div = %d, pll_sys_div = %d, sysClk = %d, ref_clk = %d\n",
                                  pll_prediv, pll_mult, pll_sp_div, pll_sys_div,deviceDescriptorP->sysClk,ref_clk);
   }
#endif
   return deviceDescriptorP->sysClk;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = GC2145_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = GC2145_DRVP_getPixClk(handle);

   return retVal;

}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_powerup
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
static ERRG_codeE GC2145_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
#if 0  //corsair GC2145 do not have FSYNC
   gpioParams.gpioNum = deviceDescriptorP->fsinGpio;
   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
#endif
   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(3000);

   // reset sensor
   GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0x80, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor
   GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, GC2145_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, GC2145_DRVP_SC_CHIP_ID_HIGH_ADDRESS, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //LOGG_PRINT(LOG_INFO_E,NULL,"GC2145 ID read %4x\n",config.accessRegParams.data);
   if (config.accessRegParams.data != GC2145_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_powerdown
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
static ERRG_codeE GC2145_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
//corsair delete fsync pin for GC2145
#if 0
   gpioParams.gpioNum = deviceDescriptorP->fsinGpio;
   gpioParams.val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
#endif
   gpioParams.val = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}



/****************************************************************************
*
*  Function Name: GC2145_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   LOGG_PRINT(LOG_INFO_E,NULL,"init sensor\n");

   return retVal;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->triggerModeParams.isTriggerSupported = 0;

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
//corsair delete 2 lines
   //GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP,   GC2145_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
//corsair delete 2 lines
   //GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP,   GC2145_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT32                              regVal[4];
   GEN_SENSOR_DRVG_getDeviceIdParamsT  *deviceIdParamsP = (GEN_SENSOR_DRVG_getDeviceIdParamsT*)pParams;

   FIX_UNUSED_PARAM_WARN(handle);

   regVal[0] = 0;
   regVal[1] = 0;
   regVal[2] = 0;
   regVal[3] = 0;

   if(ERRG_SUCCEEDED(retVal))
   {
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
*  Function Name: GC2145_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              regNum;
   IO_HANDLE                           *handleP;
   GEN_SENSOR_DRVG_exposureTimeCfgT    expParams;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   FIX_UNUSED_PARAM_WARN(pParams);

   handleP = (IO_HANDLE*)deviceDescriptorP;
   LOGG_PRINT(LOG_INFO_E,NULL,"res=%d\n",params->loadTableParams.sensorResolution);
   if (params->loadTableParams.sensorResolution == 2)/*CALIB_MODE_VERTICAL_BINNING_E=3*/
   {
      GC2145_framerateInitial = 15;
      regNum = sizeof(GC2145_CFG_TBLG_configTableFullHd)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad8(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)GC2145_CFG_TBLG_configTableFullHd, regNum);
      LOGG_PRINT(LOG_INFO_E,NULL,"loading GC2145 full size present table\n");
   }
   else if (params->loadTableParams.sensorResolution == 1)/*CALIB_MODE_BIN_E=0*/
   {
      GC2145_framerateInitial = 30;
      regNum = sizeof(GC2145_CFG_TBLG_configTableSvga)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad8(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)GC2145_CFG_TBLG_configTableSvga, regNum);
      LOGG_PRINT(LOG_INFO_E,NULL,"loading GC2145 SVGA size present table\n");
   }
   else if (params->loadTableParams.sensorResolution == 3)/*CALIB_MODE_FULL_E=1*/
   {
      GC2145_framerateInitial = 15;
      regNum = sizeof(GC2145_CFG_TBLG_configTableFullHd)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad8(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)GC2145_CFG_TBLG_configTableFullHd, regNum);
      LOGG_PRINT(LOG_INFO_E,NULL,"loading GC2145 full size present table\n");
   }
#if 0
   else if (params->loadTableParams.sensorResolution == 5)/*CALIB_MODE_USER_DEFINE_E=2 1600*900 later*/
   {
      GC2145_framerateInitial = 15;
      regNum = sizeof(GC2145_CFG_TBLG_configTableUsrDefine)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad8(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)GC2145_CFG_TBLG_configTableUsrDefine, regNum);
     LOGG_PRINT(LOG_INFO_E,NULL,"loading GC2145 user defined present table\n");
   }
#endif
   else
   {
      GC2145_framerateInitial = 0;
      //LOGG_PRINT(LOG_ERROR_E,NULL,"Error loading present table unsupported resoultion\n");
      return ERR_UNEXPECTED;
   }

   if(handleP)
   {
      GC2145_DRVP_getExposureTime(handle,&expParams);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Error loading present table\n");
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, handleP, 0xfe, 0x00, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);


   //Hblank read
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x05, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GC2145_HBlank = config.accessRegParams.data & 0xfff;

   //Vblank read
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x07, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GC2145_VBlank = config.accessRegParams.data & 0x1fff;

   //win_width read
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x0f, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GC2145_WinWidth = config.accessRegParams.data & 0xffff;

   //win_height read
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x0d, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GC2145_WinHeight = config.accessRegParams.data & 0xffff;

   //SH_delay read
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x11, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GC2145_SHDelay = config.accessRegParams.data & 0x03ff;//bit9:8

   //row time = HB+SHDELAY+WINWIDTH/2 +4
   GC2145_RowTime = GC2145_HBlank + GC2145_SHDelay + (GC2145_WinWidth >> 1) +4;
   GC2145_Vt = GC2145_WinHeight - 8;
   GC2145_max_exp_time = GC2145_WinHeight+GC2145_VBlank;
   GC2145_exp_time = GC2145_max_exp_time;

   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   IO_HANDLE                           *handleP;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              lineLengthPclk;
   UINT16                              frameLengthLine;

   handleP = (IO_HANDLE*)deviceDescriptorP;

   if(handleP)
   {
      GC2145_framerate = params->setFrameRateParams.frameRate;
      LOGG_PRINT(LOG_INFO_E,NULL,"GC2145_framerateInitial=%x, GC2145_framerate=%x \n",GC2145_framerateInitial,GC2145_framerate);
      if((GC2145_framerate<=GC2145_framerateInitial)&(GC2145_framerate >0))
      {
         GC2145_Ft = GC2145_VBlank +GC2145_Vt+ 8;
         GC2145_calculateF = GC2145_Ft;

         GC2145_FtNew = (UINT16)(GC2145_calculateF*GC2145_framerateInitial/GC2145_framerate);
         GC2145_VBlankNew = GC2145_FtNew -GC2145_Vt -8;
         if(GC2145_VBlankNew > 0x1820)//limit this value, otherwise sensor broken vga/svga/uxga min 5
         {
            GC2145_VBlankNew = 0x1820;
            LOGG_PRINT(LOG_INFO_E,NULL,"frame rate set to boarder,use max value\n");
         }
         LOGG_PRINT(LOG_INFO_E,NULL,"Ft=%x, FtNew=%x, VBlank=%x, VBlankNew=%x \n",GC2145_Ft,GC2145_FtNew,GC2145_VBlank,GC2145_VBlankNew);
         GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, handleP, 0x07, GC2145_VBlankNew, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      }
      else
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"Wrong frame rate set \n");
      }

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x07, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GC2145_VBlankNew = config.accessRegParams.data & 0x1fff;
      //LOGG_PRINT(LOG_INFO_E,NULL,"VBlankNew read %x\n",GC2145_VBlankNew);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Error loading present table\n");
   }


   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
   UINT16                              lineLengthPclk,sysClk;

   numExpLines = params->exposureTime; /* setting same exp to both sensor using stereo handle */
   if((numExpLines > GC2145_max_exp_time)|(numExpLines<10))
   {
        numExpLines = GC2145_max_exp_time;
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0x03, numExpLines, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //LOGG_PRINT(LOG_INFO_E,NULL,"exposureTime = %d, numExpLines = %d,  GC2145_RowTime = %d\n",params->exposureTime,numExpLines,GC2145_RowTime);
   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              lineLengthPclk,sysClk;

   params->exposureTime = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, handle, 0x03, GC2145_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GC2145_exp_time = config.accessRegParams.data & 0x1fff;//bit12:8
   numExpLines = GC2145_exp_time;

   params->exposureTime = numExpLines;
   LOGG_PRINT(LOG_INFO_E,NULL,"exposure time = %d, numExpLines = %d, GC2145_RowTime = %d\n",params->exposureTime,numExpLines,GC2145_RowTime);

   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   GC2145_GlobalGain = (UINT32)params->digitalGain & 0xff;
   GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0xb0, GC2145_GlobalGain, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //LOGG_PRINT(LOG_INFO_E,NULL,"Gain set Global=%x\n",GC2145_GlobalGain);

   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT            *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   params->digitalGain = 0;

   GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0xb0, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   params->digitalGain = (float)(config.accessRegParams.data & 0xFF);
   GC2145_GlobalGain = (UINT32)params->digitalGain;
   //LOGG_PRINT(LOG_INFO_E,NULL,"Gain read out Global=%x\n",GC2145_GlobalGain);

   return retVal;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   // Change the mode to manual if setting in the configuration
   if (params->setExposureModeParams.expMode == INU_DEFSG_AE_MANUAL_E) // 0 = manual 1 = auto
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Manual Exposure mode was set.\n");
      GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0xb6, 0x0, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
   else if (params->setExposureModeParams.expMode == INU_DEFSG_AE_AUTO_E)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Auto Exposure mode was set.\n");
      GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0xb6, 0x1, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }

   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   //GC2145 no this kind of register
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_setAutoExposureAvgLuma
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_ERROR_E,NULL,"trigger mode is not supported in GC2145!\n");

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0x17, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1;
      GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0x17,config.accessRegParams.data, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0x17, 0x17, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, 0x17,config.accessRegParams.data, GC2145_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_open_direct_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_open_direct_gpio(INU_DEFSG_sensorTypeE   sensorType)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd, size_read, i=0,j=GC_2145P_MAX_NUM_E;
   char        buf[128];
   GPIO_DRVG_gpioOpenParamsT     params;
   GPIO_DRVG_gpioSetDirParamsT   directionParams;
   GPIO_DRVG_gpioSetValParamsT   valParams;
   char *fileName;
   if (sensorType==INU_DEFSG_SENSOR_TYPE_SINGLE_E)
      i=GC_2145P_XSHUTDOWN_W_E;
   else
      j=GC_2145P_XSHUTDOWN_W_E;
   for (; i < j; i++)
   {
      switch ( i )
      {
         case GC_2145P_XSHUTDOWN_R_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_R";
         break;
         case GC_2145P_XSHUTDOWN_L_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_L";
         break;
         case GC_2145P_FSIN_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/FSIN_CV";
         break;
         case GC_2145P_nRESET_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/nRESET_CV";
         break;
         case GC_2145P_XSHUTDOWN_W_E:
            fileName = "/proc/device-tree/sensor_on/webcam/XSHUTDOWN_W";
         break;
         case GC_2145P_FSIN_CLR_E:
            fileName = "/proc/device-tree/sensor_on/webcam/FSIN_CLR";
         break;
      }
      fd = open( fileName, O_RDONLY );
      if (fd < 0)
      {
         retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
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
            gpiosGC2145[i]=atoi(buf);
            if(gpiosGC2145[i]!=-1)
            {
               params.gpioNum = gpiosGC2145[i];
               retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_OPEN_GPIO_CMD_E, &params);
               //setting direction
               if(ERRG_SUCCEEDED(retCode))
               {
                  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Succeed to open GPIO %d\n", params.gpioNum);
                  if(i==GC_2145P_nRESET_CV_E)
                  {
                     valParams.val     = GPIO_DRVG_GPIO_STATE_SET_E;
                     valParams.gpioNum = gpiosGC2145[i];
                     retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &valParams);
                  }
                  directionParams.direction = GPIO_DRVG_OUT_DIRECTION_E;
                  directionParams.gpioNum   = gpiosGC2145[i];
                  retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
                  LOGG_PRINT(LOG_DEBUG_E, NULL, "GPIO: Set GPIO %d direction to %d\n", directionParams.gpioNum, directionParams.direction);
               }
               else
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "GPIO: Fail to open GPIO %d\n", params.gpioNum);
               }
            }
         }
         if (close(fd) < 0)
         {
            retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
            LOGG_PRINT(LOG_ERROR_E, retCode, "fail closing a file descriptor, fileName = %s, fd = %d, errno = %s\n",fileName,fd,strerror(errno));
         }
      }
   }
   return retCode;
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   BOOL                                foundNewInstance  = FALSE;
   UINT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   FIX_UNUSED_PARAM_WARN(deviceId);

   for(sensorInstanceId = 0; sensorInstanceId < GC2145_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
   {
      deviceDescriptorP = &GC2145_DRVP_deviceDesc[sensorInstanceId];
      if(deviceDescriptorP->deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
         foundNewInstance = TRUE;
         break;
      }
   }
   GC2145_DRVP_open_direct_gpio(pOpenParams->sensorType);

   if(foundNewInstance == TRUE)
   {
      deviceDescriptorP->deviceStatus     = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
      deviceDescriptorP->sensorAddress    = GC2145_DRVP_I2C_MASTER_ADDRESS;
      deviceDescriptorP->ioctlFuncList    = GC2145_DRVP_ioctlFuncList;

      deviceDescriptorP->i2cInstanceId    = pOpenParams->i2cInstanceId;
      deviceDescriptorP->i2cSpeed         = pOpenParams->i2cSpeed;
      deviceDescriptorP->sensorType       = pOpenParams->sensorType;
      deviceDescriptorP->tableType        = pOpenParams->tableType;
      deviceDescriptorP->powerGpioMaster  = pOpenParams->powerGpioMaster;
      deviceDescriptorP->fsinGpio         = pOpenParams->fsinGpio;
      deviceDescriptorP->sensorClk        = pOpenParams->sensorClk;
      GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->powerGpioMaster);
      GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->fsinGpio);
      GEN_SENSOR_DRVP_setSensorRefClk(deviceDescriptorP->sensorClk, pOpenParams->sensorClkDiv);
      *handleP = (IO_HANDLE)deviceDescriptorP;
   }
   else
   {
      retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
   }

   return(retCode);
}


/****************************************************************************
*
*  Function Name: GC2145_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV7251 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd, size_read, i=0,j=GC_2145P_MAX_NUM_E;
   char        buf[128];
   GPIO_DRVG_gpioOpenParamsT     params;
   char *fileName;
   if (sensorType==INU_DEFSG_SENSOR_TYPE_SINGLE_E)
      i=GC_2145P_XSHUTDOWN_W_E;
   else
      j=GC_2145P_XSHUTDOWN_W_E;
   for (; i < j; i++)
   {
      switch ( i )
      {
         case GC_2145P_XSHUTDOWN_R_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_R";
         break;
         case GC_2145P_XSHUTDOWN_L_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_L";
         break;
         case GC_2145P_FSIN_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/FSIN_CV";
         break;
         case GC_2145P_nRESET_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/nRESET_CV";
         break;
         case GC_2145P_XSHUTDOWN_W_E:
            fileName = "/proc/device-tree/sensor_on/webcam/XSHUTDOWN_W";
         break;
         case GC_2145P_FSIN_CLR_E:
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
            gpiosGC2145[i]=atoi(buf);
            if(gpiosGC2145[i]!=-1)
            {
               params.gpioNum = gpiosGC2145[i];
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
*  Function Name: GC2145_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   GC2145_DRVP_close_gpio(deviceDescriptorP->sensorType);

//   if (deviceDescriptorP->slaveHandle != NULL)
//      (GEN_SENSOR_DRVG_specificDeviceDescT*)(deviceDescriptorP->slaveHandle)->deviceStatus = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;

//   if (deviceDescriptorP->stereoHandle != NULL)
//      (GEN_SENSOR_DRVG_specificDeviceDescT*)(deviceDescriptorP->stereoHandle)->deviceStatus = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");

   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: GC2145_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
static ERRG_codeE GC2145_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: GC2145_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: GC2145 sensor driver
*
****************************************************************************/
ERRG_codeE GC2145_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(GC2145_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < GC2145_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
      {
         deviceDescriptorP = &GC2145_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_GC_2145_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
      }
      GC2145_DRVP_isDrvInitialized = TRUE;
   }


   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &GC2145_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &GC2145_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &GC2145_DRVP_open;
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

