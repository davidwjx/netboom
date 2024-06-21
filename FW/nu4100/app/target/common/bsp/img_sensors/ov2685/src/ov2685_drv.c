/****************************************************************************
 *
 *   FileName: ov2685_drv.c
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: OV2685 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ov2685_drv.h"
#include "ov2685_cfg_tbl.h"
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

static ERRG_codeE OV2685_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_setFrameRate(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE OV2685_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE OV2685_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_open_direct_gpio(INU_DEFSG_sensorTypeE   sensorType);
static ERRG_codeE OV2685_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE OV2685_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE OV2685_DRVP_close(IO_HANDLE handle);
static ERRG_codeE OV2685_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static ERRG_codeE OV2685_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV2685_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);



/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   OV_2685P_XSHUTDOWN_R_E,
   OV_2685P_XSHUTDOWN_L_E,
   OV_2685P_FSIN_CV_E,
   OV_2685P_nRESET_CV_E,
   OV_2685P_XSHUTDOWN_W_E,
   OV_2685P_FSIN_CLR_E,
   OV_2685P_MAX_NUM_E
}OV_2685P_gpioNumberE;
int gpiosOv2685[OV_2685P_MAX_NUM_E] = {-1,-1,-1,-1,-1,-1};

#define CHECK_GPIO(OV_2685P_gpioNumberE) if (gpiosOv2685[OV_2685P_gpioNumberE] != -1)

/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define OV2685_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define OV2685_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define OV2685_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define OV2685_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define OV2685_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define OV2685_DRVP_16_BIT_ACCESS                                     (16)
#define OV2685_DRVP_8_BIT_ACCESS                                      (8)
#define OV2685_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)

///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define OV2685_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define OV2685_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)
#define OV2685_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x300A)
#define OV2685_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x300B)

#define OV2685_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define OV2685_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define OV2685_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)
#define OV2685_DRVP_AEC_GAIN_1_ADDRESS                                (0x350A)
#define OV2685_DRVP_AEC_GAIN_2_ADDRESS                                (0x350B)
#define OV2685_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define OV2685_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define OV2685_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define OV2685_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define OV2685_DRVP_PLL_PRE_DIV_ADDRESS                               (0x3080)
#define OV2685_DRVP_PLL_MULTIPLIER_HIGH_ADDRESS                       (0x3081)
#define OV2685_DRVP_PLL_MULTIPLIER_LOW_ADDRESS                        (0x3082)
#define OV2685_DRVP_PLL_MIPI_DIVIDER_ADDRESS                          (0x3083)
#define OV2685_DRVP_PLL_SYS_CLK_DIVIDER_ADDRESS                       (0x3084)
#define OV2685_DRVP_PLL_DAC_DIVIDER_ADDRESS                           (0x3085)
#define OV2685_DRVP_PLL_SP_DIVIDER_ADDRESS                            (0x3086)
#define OV2685_DRVP_PLL_LANE_DIVIDER_ADDRESS                          (0x3087)
#define OV2685_DRVP_PLL_CONTROL_ADDRESS                               (0x3088)

#define OV2685_DRVP_AEC_CONTROL2_ADDRESS                              (0x3A02)

#define OV2685_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define OV2685_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define OV2685_DRVP_GROUP_ACCESS_ADDRESS 0x3200

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define OV2685_DRVP_SC_CHIP_ID                                        (0x2685)
#define OV2685_DRVP_I2C_DEFAULT_ADDRESS                               (0x78)
#define OV2685_DRVP_I2C_MASTER_ADDRESS                                (OV2685_DRVP_I2C_DEFAULT_ADDRESS)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////
#define OV2685_DRVP_FREQ_MODE_BIT                                     (1 << 7)

/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  OV2685_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   OV2685_DRVP_deviceDesc[OV2685_DRVP_MAX_NUMBER_SENSOR_INSTANCE];
static GEN_SENSOR_DRVG_ioctlFuncListT        OV2685_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      OV2685_DRVP_resetSensors,            \
                                                                                                      OV2685_DRVP_initSensor,              \
                                                                                                      OV2685_DRVP_configSensors,           \
                                                                                                      OV2685_DRVP_startSensors,            \
                                                                                                      OV2685_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      OV2685_DRVP_getDeviceId,             \
                                                                                                      OV2685_DRVP_loadPresetTable,         \
                                                                                                      OV2685_DRVP_setFrameRate,            \
                                                                                                      OV2685_DRVP_setExposureTime,         \
                                                                                                      OV2685_DRVP_getExposureTime,         \
                                                                                                      OV2685_DRVP_setExposureMode,         \
                                                                                                      OV2685_DRVP_setImgOffsets,           \
                                                                                                      OV2685_DRVP_getAvgBrighness,         \
                                                                                                      OV2685_DRVP_setOutFormat,            \
                                                                                                      OV2685_DRVP_setPowerFreq,            \
                                                                                                      OV2685_DRVP_setStrobe,               \
                                                                                                      OV2685_DRVP_getSensorClks,           \
                                                                                                      OV2685_DRVP_setGain,                 \
                                                                                                      OV2685_DRVP_getGain,                 \
                                                                                                      OV2685_DRVP_trigger,                 \
                                                                                                      OV2685_DRVP_mirrorFlip,              \
                                                                                                      OV2685_DRVP_powerup,                 \
                                                                                                      OV2685_DRVP_powerdown,               \
                                                                                                      OV2685_DRVP_changeSensorAddress,     \
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


static UINT32                                OV2685_DRVP_pre_div_mapTblx2[8] = {2,3,4,5,6,8,12,16};

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: OV2685_DRVP_changeSensorAddress
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
static ERRG_codeE OV2685_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_getPixClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static UINT16 OV2685_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16 pll_prediv, pll_mult, pll_sp_div, pll_mipi_div, pixClk, ref_clk, lineLengthPclk, x_out_size;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->pixClk)
   {
      UINT8  mipiDiv[4]   = {4,5,6,8};
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pixClk = ref_clk;

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_PRE_DIV_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_MULTIPLIER_LOW_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_MULTIPLIER_HIGH_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult |= (( config.accessRegParams.data ) & 0x1 << 8);


      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_SP_DIVIDER_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_sp_div = (( config.accessRegParams.data ) & 0xF) + 1;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_MIPI_DIVIDER_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mipi_div = ( config.accessRegParams.data ) & 0xF;

      if (pll_prediv != 0)
      {
         /* to stage as integer, we keep the mapTbl as x2 of the value. here we also x2 the sysclk to reduce the fractions*/
         pixClk = (2 * pixClk) / OV2685_DRVP_pre_div_mapTblx2[pll_prediv];
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
   return deviceDescriptorP->pixClk;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_getSysClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static UINT16 OV2685_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16 pll_prediv, pll_mult, pll_sp_div, pll_sys_div, sys_clk, ref_clk;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->sysClk)
   {
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      sys_clk = ref_clk;

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_PRE_DIV_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_MULTIPLIER_LOW_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_MULTIPLIER_HIGH_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult |= (( config.accessRegParams.data ) & 0x1 << 8);


      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_SP_DIVIDER_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_sp_div = (( config.accessRegParams.data ) & 0xF) + 1;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_PLL_SYS_CLK_DIVIDER_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_sys_div = (( config.accessRegParams.data ) & 0xF) + 1;

      if (pll_prediv != 0)
      {
         /* to stage as integer, we keep the mapTbl as x2 of the value. here we also x2 the sysclk to reduce the fractions*/
         sys_clk = (2 * sys_clk) / OV2685_DRVP_pre_div_mapTblx2[pll_prediv];
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
   return deviceDescriptorP->sysClk;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = OV2685_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = OV2685_DRVP_getPixClk(handle);

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
static ERRG_codeE OV2685_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   gpioParams.gpioNum = deviceDescriptorP->fsinGpio;
   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(3000);

   // reset sensor
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OV2685_DRVP_SC_CHIP_ID_HIGH_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (config.accessRegParams.data != OV2685_DRVP_SC_CHIP_ID)
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
static ERRG_codeE OV2685_DRVP_powerdown(IO_HANDLE handle, void *pParams)
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

   return retVal;
}



/****************************************************************************
*
*  Function Name: OV2685_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   LOGG_PRINT(LOG_INFO_E,NULL,"init sensor\n");

   return retVal;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->triggerModeParams.isTriggerSupported = 0;

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV2685_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV2685_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
*  Function Name: OV2685_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              regNum;
   IO_HANDLE                           *handleP;
   GEN_SENSOR_DRVG_exposureTimeCfgT    expParams;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   FIX_UNUSED_PARAM_WARN(pParams);

   handleP = (IO_HANDLE*)deviceDescriptorP;

   if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
   {
      regNum = sizeof(OV2685_CFG_TBLG_configTableSvga)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)OV2685_CFG_TBLG_configTableSvga, regNum);
   }
   else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
   {
      regNum = sizeof(OV2685_CFG_TBLG_configTableSvga)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)OV2685_CFG_TBLG_configTableSvga, regNum);
   }
   else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_HD_E)
   {
      regNum = sizeof(OV2685_CFG_TBLG_configTableFullHd)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)OV2685_CFG_TBLG_configTableFullHd, regNum);
   }
   else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_FULL_HD_E)
   {
      regNum = sizeof(OV2685_CFG_TBLG_configTableFullHd)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)OV2685_CFG_TBLG_configTableFullHd, regNum);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Error loading present table unsupported resoultion\n");
      return ERR_UNEXPECTED;
   }

   if(handleP)
   {
      OV2685_DRVP_getExposureTime(handle,&expParams);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Error loading present table\n");
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
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
      UINT16 sysClk;

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handleP, OV2685_DRVP_TIMING_HTS_LOW_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      lineLengthPclk = ( config.accessRegParams.data ) & 0xFFFF;

      sysClk = OV2685_DRVP_getSysClk(handle);
      frameLengthLine = (1000000 * sysClk) / (lineLengthPclk * params->setFrameRateParams.frameRate);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handleP, OV2685_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      LOGG_PRINT(LOG_INFO_E,NULL,"set frame rate %d. lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sys_clk = %d\n",
                                  params->setFrameRateParams.frameRate,lineLengthPclk,frameLengthLine,sysClk);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"Error loading present table\n");
   }


   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
   UINT16                              lineLengthPclk,sysClk;
   UINT32                              groupHold;

   sysClk = OV2685_DRVP_getSysClk(handle);
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_TIMING_HTS_LOW_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   lineLengthPclk =  config.accessRegParams.data & 0xFFFF;

   // group hold start
   groupHold = 0x00 ;// deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (lineLengthPclk != 0)
   {
      numExpLines = (params->exposureTime * sysClk) / lineLengthPclk; /* setting same exp to both sensor using stereo handle */

      // verify exposure time is valid

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_EXPO_1_ADDRESS, (numExpLines >> 12) & 0xF, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_EXPO_2_ADDRESS, (numExpLines >> 4) & 0xFF, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_EXPO_3_ADDRESS, ((numExpLines & 0xF) << 4) & 0xF0, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      LOGG_PRINT(LOG_DEBUG_E,NULL,"pixelClk = %d, lineLengthPclk = %d, numExpLines = %d, time = %d, address = 0x%x\n",
                                    sysClk,lineLengthPclk,numExpLines,params->exposureTime,deviceDescriptorP->sensorAddress);
   }
   //group hold end
   groupHold = 0x10 ;//| deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xE0 ;//| deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              lineLengthPclk,sysClk;

   params->exposureTime = 0;
   sysClk = OV2685_DRVP_getSysClk(handle);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_EXPO_1_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines |= ((config.accessRegParams.data & 0xF) << 12);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_EXPO_2_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines |= ((config.accessRegParams.data & 0xFF) << 4);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_EXPO_3_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines |= ((config.accessRegParams.data & 0xF0) >> 4);

   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_TIMING_HTS_LOW_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   lineLengthPclk = config.accessRegParams.data & 0xFFFF;

   params->exposureTime = (numExpLines * lineLengthPclk) / (sysClk);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d, lineLengthPclk = %d, sysClk = %d\n",
                                 params->exposureTime,numExpLines,lineLengthPclk, sysClk);


   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32 digitalGain = (UINT32)params->digitalGain; // digital gain 16 means gain  x1

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_GAIN_1_ADDRESS, (digitalGain >> 8) & 0x3, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_GAIN_2_ADDRESS, digitalGain & 0xFF, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT            *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32 digitalGain = 0; // digital gain 16 means gain  x1

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_GAIN_2_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   digitalGain = config.accessRegParams.data & 0xFF;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_GAIN_1_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   digitalGain |= (config.accessRegParams.data << 8) & 0x3;

   params->digitalGain = (float)digitalGain;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   if (params->setExposureModeParams.expMode==INU_DEFSG_AE_AUTO_E)//AE enable
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3503, 0x0, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      LOGG_PRINT(LOG_INFO_E, NULL, "set enable AE interrior of sensor\n");
   }
   else if (params->setExposureModeParams.expMode == INU_DEFSG_AE_MANUAL_E)
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3503, 0x3, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      LOGG_PRINT(LOG_INFO_E, NULL, "disable AE\n");
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_CONTROL2_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (params->setPowerFreqParams.powerFreq == INU_DEFSG_SEN_POWER_FREQ_50_HZ_E)
   {
      config.accessRegParams.data |= OV2685_DRVP_FREQ_MODE_BIT;
   }
   else
   {
      config.accessRegParams.data &= ~OV2685_DRVP_FREQ_MODE_BIT;
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_AEC_CONTROL2_ADDRESS, config.accessRegParams.data, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_setAutoExposureAvgLuma
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);

   LOGG_PRINT(LOG_ERROR_E,NULL,"trigger mode is not supported in OV2685!\n");

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OV2685_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV2685_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, OV2685_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_open_direct_gpio(INU_DEFSG_sensorTypeE   sensorType)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd, size_read, i=0,j=OV_2685P_MAX_NUM_E;
   char        buf[128];
   GPIO_DRVG_gpioOpenParamsT     params;
   GPIO_DRVG_gpioSetDirParamsT   directionParams;
   GPIO_DRVG_gpioSetValParamsT   valParams;
   char *fileName;
   if (sensorType==INU_DEFSG_SENSOR_TYPE_SINGLE_E)
      i=OV_2685P_XSHUTDOWN_W_E;
   else
      j=OV_2685P_XSHUTDOWN_W_E;
   for (; i < j; i++)
   {
      switch ( i )
      {
         case OV_2685P_XSHUTDOWN_R_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_R";
         break;
         case OV_2685P_XSHUTDOWN_L_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_L";
         break;
         case OV_2685P_FSIN_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/FSIN_CV";
         break;
         case OV_2685P_nRESET_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/nRESET_CV";
         break;
         case OV_2685P_XSHUTDOWN_W_E:
            fileName = "/proc/device-tree/sensor_on/webcam/XSHUTDOWN_W";
         break;
         case OV_2685P_FSIN_CLR_E:
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
            gpiosOv2685[i]=atoi(buf);
            if(gpiosOv2685[i]!=-1)
            {
               params.gpioNum = gpiosOv2685[i];
               retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_OPEN_GPIO_CMD_E, &params);
               //setting direction
               if(ERRG_SUCCEEDED(retCode))
               {
                  LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Succeed to open GPIO %d\n", params.gpioNum);
                  if(i==OV_2685P_nRESET_CV_E)
                  {
                     valParams.val     = GPIO_DRVG_GPIO_STATE_SET_E;
                     valParams.gpioNum = gpiosOv2685[i];
                     retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &valParams);
                  }
                  directionParams.direction = GPIO_DRVG_OUT_DIRECTION_E;
                  directionParams.gpioNum   = gpiosOv2685[i];
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
*  Function Name: OV2685_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   BOOL                                foundNewInstance  = FALSE;
   UINT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   FIX_UNUSED_PARAM_WARN(deviceId);

   for(sensorInstanceId = 0; sensorInstanceId < OV2685_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
   {
      deviceDescriptorP = &OV2685_DRVP_deviceDesc[sensorInstanceId];
      if(deviceDescriptorP->deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
         foundNewInstance = TRUE;
         break;
      }
   }
   OV2685_DRVP_open_direct_gpio(pOpenParams->sensorType);

   if(foundNewInstance == TRUE)
   {
      deviceDescriptorP->deviceStatus     = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
      deviceDescriptorP->sensorAddress    = OV2685_DRVP_I2C_MASTER_ADDRESS;
      deviceDescriptorP->ioctlFuncList    = OV2685_DRVP_ioctlFuncList;

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
*  Function Name: OV2685_DRVP_close_gpio
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
static ERRG_codeE OV2685_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd, size_read, i=0,j=OV_2685P_MAX_NUM_E;
   char        buf[128];
   GPIO_DRVG_gpioOpenParamsT     params;
   char *fileName;
   if (sensorType==INU_DEFSG_SENSOR_TYPE_SINGLE_E)
      i=OV_2685P_XSHUTDOWN_W_E;
   else
      j=OV_2685P_XSHUTDOWN_W_E;
   for (; i < j; i++)
   {
      switch ( i )
      {
         case OV_2685P_XSHUTDOWN_R_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_R";
         break;
         case OV_2685P_XSHUTDOWN_L_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_L";
         break;
         case OV_2685P_FSIN_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/FSIN_CV";
         break;
         case OV_2685P_nRESET_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/nRESET_CV";
         break;
         case OV_2685P_XSHUTDOWN_W_E:
            fileName = "/proc/device-tree/sensor_on/webcam/XSHUTDOWN_W";
         break;
         case OV_2685P_FSIN_CLR_E:
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
            gpiosOv2685[i]=atoi(buf);
            if(gpiosOv2685[i]!=-1)
            {
               params.gpioNum = gpiosOv2685[i];
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
*  Function Name: OV2685_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   OV2685_DRVP_close_gpio(deviceDescriptorP->sensorType);

//   if (deviceDescriptorP->slaveHandle != NULL)
//      (GEN_SENSOR_DRVG_specificDeviceDescT*)(deviceDescriptorP->slaveHandle)->deviceStatus = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;

//   if (deviceDescriptorP->stereoHandle != NULL)
//      (GEN_SENSOR_DRVG_specificDeviceDescT*)(deviceDescriptorP->stereoHandle)->deviceStatus = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");

   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: OV2685_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
static ERRG_codeE OV2685_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: OV2685_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV2685 sensor driver
*
****************************************************************************/
ERRG_codeE OV2685_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(OV2685_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < OV2685_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
      {
         deviceDescriptorP = &OV2685_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_OV_2685_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
      }
      OV2685_DRVP_isDrvInitialized = TRUE;
   }


   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &OV2685_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &OV2685_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &OV2685_DRVP_open;
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

