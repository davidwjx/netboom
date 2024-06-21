/****************************************************************************
 *
 *   FileName: ov7251_drv.c
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: OV7251 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ov7251_drv.h"
#include "ov7251_cfg_tbl.h"
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

static ERRG_codeE OV7251_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_setFrameRate(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE OV7251_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE OV7251_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_close_gpio();

static ERRG_codeE OV7251_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE OV7251_DRVP_close(IO_HANDLE handle);
static ERRG_codeE OV7251_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static ERRG_codeE OV7251_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV7251_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);


/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   OV_7251P_XSHUTDOWN_R_E,
   OV_7251P_XSHUTDOWN_L_E,
   OV_7251P_FSIN_CV_E,
   OV_7251P_XSHUTDOWN_W_E,
   OV_7251P_FSIN_CLR_E,
   OV_7251P_MAX_NUM_E
}OV_7251P_gpioNumberE;
int gpiosOv7251[OV_7251P_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(OV_7251P_gpioNumberE) if (gpiosOv7251[OV_7251P_gpioNumberE] != -1)

/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define OV7251_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define OV7251_DRVP_CHANGE_ADDR_SLEEP_TIME                            (1*1000)   // 1ms
#define OV7251_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define OV7251_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define OV7251_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define OV7251_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define OV7251_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define OV7251_DRVP_16_BIT_ACCESS                                     (16)
#define OV7251_DRVP_8_BIT_ACCESS                                      (8)
#define OV7251_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)
//#define OV7251_DRVP_GROUP_MODE_EXP_CHANGE
///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define OV7251_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define OV7251_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)
#define OV7251_DRVP_SC_SCCB_ID1_ADDRESS                               (0x0109)
#define OV7251_DRVP_SC_SCCB_ID2_ADDRESS                               (0x302B)
#define OV7251_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x300A)
#define OV7251_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x300B)

#define OV7251_DRVP_SC_CLKRST6_ADDRESS                                (0x301C)
#define OV7251_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define OV7251_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define OV7251_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define OV7251_DRVP_GRP_SW_CTRL_ADDRESS                               (0x320B)

#define OV7251_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define OV7251_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define OV7251_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)
#define OV7251_DRVP_AEC_GAIN_1_ADDRESS                                (0x350A)
#define OV7251_DRVP_AEC_GAIN_2_ADDRESS                                (0x350B)
#define OV7251_DRVP_TIMING_X_OUTPUT_SIZES_LOW_ADDRESS                 (0x3808)
#define OV7251_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define OV7251_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define OV7251_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define OV7251_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define OV7251_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define OV7251_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define OV7251_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)


#define OV7251_DRVP_PLL1_PRE_DIV_ADDRESS                              (0x30B4)
#define OV7251_DRVP_PLL1_MULTIPLIER_ADDRESS                           (0x30B3)
#define OV7251_DRVP_PLL1_DIVIDER_ADDRESS                              (0x30B1)
#define OV7251_DRVP_PLL1_PIX_DIVIDER_ADDRESS                          (0x30B0)
#define OV7251_DRVP_PLL2_PRE_DIV_ADDRESS                              (0x3098)
#define OV7251_DRVP_PLL2_MULTIPLIER_ADDRESS                           (0x3099)
#define OV7251_DRVP_PLL2_DIVIDER_ADDRESS                              (0x309D)
#define OV7251_DRVP_PLL2_SYS_DIVIDER_ADDRESS                          (0x309A)

#define OV7251_DRVP_STROBE_EN_ADDRESS                                 (0x3005)
#define OV7251_DRVP_STROBE_POLARITY_ADDRESS                           (0x3B96)
#define OV7251_DRVP_STROBE_OFFSET_ADDRESS                             (0x3B8B)
#define OV7251_DRVP_STROBE_DURATION_HIGH_ADDRESS                      (0x3B8E)
#define OV7251_DRVP_STROBE_DURATION_LOW_ADDRESS                       (0x3B8F)
#define OV7251_DRVP_STROBE_MASK_ADDRESS                               (0x3B81)
#define OV7251_DRVP_PWM_CTRL_29_ADDRESS                               (0x3929)
#define OV7251_DRVP_PWM_CTRL_2A_ADDRESS                               (0x392A)


#define OV7251_DRVP_LOWPWR00_ADDRESS                                  (0x3C00)
#define OV7251_DRVP_LOWPWR01_ADDRESS                                  (0x3C01)
#define OV7251_DRVP_LOWPWR02_ADDRESS                                  (0x3C02)
#define OV7251_DRVP_LOWPWR03_ADDRESS                                  (0x3C03)
#define OV7251_DRVP_LOWPWR04_ADDRESS                                  (0x3C04)
#define OV7251_DRVP_LOWPWR05_ADDRESS                                  (0x3C05)
#define OV7251_DRVP_LOWPWR06_ADDRESS                                  (0x3C06)
#define OV7251_DRVP_LOWPWR07_ADDRESS                                  (0x3C07)
#define OV7251_DRVP_LOWPWR0E_ADDRESS                                  (0x3C0E)
#define OV7251_DRVP_LOWPWR0F_ADDRESS                                  (0x3C0F)
#define OV7251_DRVP_ANA_CORE_6_ADDRESS                                (0x3666)

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define OV7251_DRVP_SC_CHIP_ID                                        (0x7750)
#define OV7251_DRVP_I2C_DEFAULT_ADDRESS                               (0xE0)
#define OV7251_DRVP_I2C_MASTER_ADDRESS                                (OV7251_DRVP_I2C_DEFAULT_ADDRESS)
#define OV7251_DRVP_I2C_SLAVE_ADDRESS                                 (0xA0)
#define OV7251_DRVP_I2C_GLOBAL_ADDRESS                                (0xC0)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  OV7251_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   OV7251_DRVP_deviceDesc[6];//[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        OV7251_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      OV7251_DRVP_resetSensors,            \
                                                                                                      OV7251_DRVP_initSensor,              \
                                                                                                      OV7251_DRVP_configSensors,           \
                                                                                                      OV7251_DRVP_startSensors,            \
                                                                                                      OV7251_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      OV7251_DRVP_getDeviceId,             \
                                                                                                      OV7251_DRVP_loadPresetTable,         \
                                                                                                      OV7251_DRVP_setFrameRate,            \
                                                                                                      OV7251_DRVP_setExposureTime,         \
                                                                                                      OV7251_DRVP_getExposureTime,         \
                                                                                                      OV7251_DRVP_setExposureMode,         \
                                                                                                      OV7251_DRVP_setImgOffsets,           \
                                                                                                      OV7251_DRVP_getAvgBrighness,         \
                                                                                                      OV7251_DRVP_setOutFormat,            \
                                                                                                      OV7251_DRVP_setPowerFreq,            \
                                                                                                      OV7251_DRVP_setStrobe,               \
                                                                                                      OV7251_DRVP_getSensorClks,           \
                                                                                                      OV7251_DRVP_setGain,                 \
                                                                                                      OV7251_DRVP_getGain,                 \
                                                                                                      OV7251_DRVP_trigger,                 \
                                                                                                      OV7251_DRVP_mirrorFlip,              \
                                                                                                      OV7251_DRVP_powerup,                 \
                                                                                                      OV7251_DRVP_powerdown,               \
                                                                                                      OV7251_DRVP_changeSensorAddress,     \
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

static UINT16 OV7251_DRVP_lineLengthPclk;
static UINT16 OV7251_DRVP_x_output_size;
static UINT16 OV7251_DRVP_vts;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static UINT16 OV7251_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                           retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT    config;
   GEN_SENSOR_DRVG_specificDeviceDescT  *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->pixClk)
   {
      UINT16 pll1_prediv, pll1_mult, pll1_div, pll1_pix_div, pix_clk,ref_clk;

      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL1_PRE_DIV_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll1_prediv = ( config.accessRegParams.data ) & 0x0F;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL1_MULTIPLIER_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll1_mult = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL1_DIVIDER_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll1_div = ( config.accessRegParams.data ) & 0x1F;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL1_PIX_DIVIDER_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll1_pix_div = ( config.accessRegParams.data ) & 0xF;

      pix_clk = ref_clk;
      switch(pll1_prediv)
      {
         case(1):
         case(2):
         case(3):
         case(4):
         case(6):
         case(8):
            pix_clk = pix_clk / pll1_prediv;
            break;
         case(5):
            pix_clk = pix_clk / 1.5;
            break;
         case(7):
            pix_clk = pix_clk / 2.5;
            break;
         default:
            pix_clk = pix_clk;
            break;
      }

      pix_clk = pix_clk * pll1_mult;
      pix_clk = pix_clk / pll1_div;
      pix_clk = pix_clk / pll1_pix_div;
      //Arnon - the calculation brings the pix_clk to 53Mhz at full mode configuration. In practice, the system works with this clk but only if the DSR is not in
      //bypass (buffering), which suggest the clk is actually higher? lets not risk it, and use the entire line+blank for pix clk calc on this sensor
      //pix_clk = (pix_clk * OV7251_DRVP_x_output_size) / OV7251_DRVP_lineLengthPclk;

      deviceDescriptorP->pixClk = pix_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll1_prediv = %d, pll1_mult = %d, pll1_div = %d, pll1_sys_div = %d, pixClk = %d, ref_clk = %d\n",
                                  pll1_prediv, pll1_mult, pll1_div, pll1_pix_div,deviceDescriptorP->pixClk,ref_clk);
   }
   return deviceDescriptorP->pixClk;
}


static UINT16 OV7251_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                           retVal = SENSOR__RET_SUCCESS;
   UINT16                               ref_clk;
   GEN_SENSOR_DRVG_sensorParametersT    config;
   GEN_SENSOR_DRVG_specificDeviceDescT  *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (!deviceDescriptorP->sysClk)
   {
      UINT16 pll2_prediv, pll2_mult, pll2_div, pll2_sys_div, sys_clk;
      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL2_PRE_DIV_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_prediv = ( config.accessRegParams.data ) & 0x1F;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL2_MULTIPLIER_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_mult = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL2_DIVIDER_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_div = ( config.accessRegParams.data ) & 0xF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_PLL2_SYS_DIVIDER_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_div = ( config.accessRegParams.data ) & 0xF;

      sys_clk = ref_clk;
      if (pll2_prediv != 0)
      {
         sys_clk = (2 * sys_clk) / (pll2_prediv);
      }

      sys_clk = sys_clk * pll2_mult;

      if ((pll2_div & 0x4) == 0x4)
      {
         sys_clk = (sys_clk*2) / 3;
      }

      if (pll2_sys_div != 0)
      {
         sys_clk = sys_clk / (pll2_sys_div * 2);
      }

      deviceDescriptorP->sysClk = sys_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll2_prediv = %d, pll2_mult = %d, pll2_div = %d, pll2_sys_div = %d, sysClk = %d, ref_clk = %d\n",
                                  pll2_prediv, pll2_mult, pll2_div, pll2_sys_div,deviceDescriptorP->sysClk,ref_clk);
   }
   return deviceDescriptorP->sysClk;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_getSensorClks
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
static ERRG_codeE OV7251_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                           retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT  *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = OV7251_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = OV7251_DRVP_getPixClk(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_setStrobeDuration
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
static ERRG_codeE OV7251_DRVP_setStrobeDuration(IO_HANDLE handle, UINT16 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum,sysClk = 0;
#ifdef OV7251_DRVP_GROUP_MODE_EXP_CHANGE
   UINT16                              strobeStartPoint;
#endif

   sysClk = OV7251_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (OV7251_DRVP_lineLengthPclk - 1)) / (OV7251_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_STROBE_DURATION_HIGH_ADDRESS, (strobeUpTicksNum & 0xFFFF), OV7251_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#ifdef OV7251_DRVP_GROUP_MODE_EXP_CHANGE
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = OV7251_DRVP_vts - usec - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_PWM_CTRL_29_ADDRESS, (strobeStartPoint & 0xFFFF), OV7251_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   LOGG_PRINT(LOG_DEBUG_E,NULL,"strobeUpTicksNum = %d, usec = %d, lineLengthPclk = %d, pixClk = %d\n", strobeUpTicksNum, usec, OV7251_DRVP_lineLengthPclk, sysClk);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_enStrobe
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
static ERRG_codeE OV7251_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_STROBE_POLARITY_ADDRESS, 0x40, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_STROBE_OFFSET_ADDRESS, 0x0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_STROBE_MASK_ADDRESS, 0xFF, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_STROBE_EN_ADDRESS, 0x08, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_resetSensors
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
static ERRG_codeE OV7251_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   LOGG_PRINT(LOG_INFO_E, NULL, "reset Sensor (type = %d, address = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);

   return SENSOR__RET_SUCCESS;
}



/****************************************************************************
*
*  Function Name: OV7251_DRVP_changeSensorAddress
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
static ERRG_codeE OV7251_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 address = *(UINT8*)pParams;

   FIX_UNUSED_PARAM_WARN(pParams);
   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_SC_SCCB_ID2_ADDRESS, address, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   deviceDescriptorP->sensorAddress = (UINT8)address;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_SC_SCCB_ID2_ADDRESS, address, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(OV7251_DRVP_CHANGE_ADDR_SLEEP_TIME);


   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OV7251_DRVP_SC_SCCB_ID2_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   if (config.accessRegParams.data != address)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Error changing address! (%x) (requested %x)\n",config.accessRegParams.data,address);
   }

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
static ERRG_codeE OV7251_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   FIX_UNUSED_PARAM_WARN(pParams);


   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(OV7251_DRVP_RESET_SLEEP_TIME);

   // reset sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //workaround
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OV7251_DRVP_SC_CHIP_ID_HIGH_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (config.accessRegParams.data != OV7251_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

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
static ERRG_codeE OV7251_DRVP_powerdown(IO_HANDLE handle, void *pParams)
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
   deviceDescriptorP->sensorAddress = OV7251_DRVP_I2C_MASTER_ADDRESS;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_initSensor
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
static ERRG_codeE OV7251_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_syncSensors
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
static ERRG_codeE OV7251_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_TIMING_VTS_LOW_ADDRESS, 0xFF, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_TIMING_VTS_HIGH_ADDRESS, 0xFE, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      OV7251_DRVP_vts = 0xFFFE;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR01_ADDRESS, 0x63, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR02_ADDRESS, 0x3, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR03_ADDRESS, 0x17, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR05_ADDRESS, 0x1, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR06_ADDRESS, 0x0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR07_ADDRESS, 0x1, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_ANA_CORE_6_ADDRESS, 0x0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR0E_ADDRESS, 0xFF, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV7251_DRVP_LOWPWR0F_ADDRESS, 0xFE, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      params->triggerModeParams.gpioBitmap |= (((UINT64)1) << deviceDescriptorP->fsinGpio);
      /* FSIN pulse width should be no shorter than 5 input clock cycles. */
      params->triggerModeParams.pulseTime = 300;
      params->triggerModeParams.isTriggerSupported = 1;

   }

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_startSensors
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
static ERRG_codeE OV7251_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_stopSensors
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
static ERRG_codeE OV7251_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV7251_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_getDeviceId
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
static ERRG_codeE OV7251_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
   config.accessRegParams.accessSize  = OV7251_DRVP_SENSOR_ACCESS_2_BYTE;
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
*  Function Name: OV7251_DRVP_loadPresetTable
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
static ERRG_codeE OV7251_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr=NULL;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   *params              = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   UINT16                              regNum = 0;
   GEN_SENSOR_DRVG_exposureTimeCfgT    expParams;

   LOGG_PRINT(LOG_INFO_E, NULL, "tableType: %d, res %d\n",deviceDescriptorP->tableType,params->loadTableParams.sensorResolution);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      sensorTablePtr = OV7251_CFG_TBLG_single_configTable;
      regNum = sizeof(OV7251_CFG_TBLG_single_configTable)
               / sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else if (deviceDescriptorP->tableType == INU_DEFSG_STEREO_E)
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = OV7251_CFG_TBLG_stereo_qvga_configTable;
         regNum = sizeof(OV7251_CFG_TBLG_stereo_qvga_configTable)
                  /sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_HD_E)
      {
         sensorTablePtr = OV7251_CFG_TBLG_stereo_full_vga_configTable;
         regNum = sizeof(OV7251_CFG_TBLG_stereo_full_vga_configTable)
                  /sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = OV7251_CFG_TBLG_stereo_vertical_binning_configTable;
         regNum = sizeof(OV7251_CFG_TBLG_stereo_vertical_binning_configTable)
                  /sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
   }
   else
   {
      sensorTablePtr = OV7251_CFG_TBLG_stereo_vga_HTC_configTable;
      regNum = sizeof(OV7251_CFG_TBLG_stereo_vga_HTC_configTable)
               / sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }


   retVal = GEN_SENSOR_DRVG_regTableLoad(deviceDescriptorP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
   if (ERRG_SUCCEEDED(retVal))
   {
      GEN_SENSOR_DRVG_sensorParametersT   config;
      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_TIMING_HTS_LOW_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      OV7251_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_TIMING_X_OUTPUT_SIZES_LOW_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      OV7251_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

      OV7251_DRVP_getSysClk(handle);
      OV7251_DRVP_getPixClk(handle);

      expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
      OV7251_DRVP_getExposureTime(deviceDescriptorP,&expParams);
      OV7251_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
      OV7251_DRVP_enStrobe(deviceDescriptorP);

#ifdef OV7251_DRVP_GROUP_MODE_EXP_CHANGE
      // reset  group mode
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_SC_CLKRST6_ADDRESS, 0x2, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_SC_CLKRST6_ADDRESS, 0x0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_TIMING_REG_2B_ADDRESS, 0x2A, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // enable the clock for group write function
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_SC_CLKRST6_ADDRESS, 0xF0, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // enable group mode
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_GRP_SW_CTRL_ADDRESS, 0x5, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_setFrameRate
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
static ERRG_codeE OV7251_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              sysClk = 0,frameLengthLine;

   if (ERRG_SUCCEEDED(retVal) && OV7251_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
      sysClk = OV7251_DRVP_getSysClk(handle);
      frameLengthLine = (1000000 * sysClk) / (OV7251_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, OV7251_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      OV7251_DRVP_vts = frameLengthLine;
      LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. OV7251_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,OV7251_DRVP_lineLengthPclk,frameLengthLine,sysClk);
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_setExposureTime
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
static ERRG_codeE OV7251_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
#ifdef OV7251_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32                              groupHold;
#endif
   UINT16                              sysClk = 0;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

#ifdef OV7251_DRVP_GROUP_MODE_EXP_CHANGE
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#endif

   if (OV7251_DRVP_lineLengthPclk != 0)
   {
      sysClk = OV7251_DRVP_getSysClk(handle);
      numExpLines = ((params->exposureTime * sysClk) + (OV7251_DRVP_lineLengthPclk - 1)) / OV7251_DRVP_lineLengthPclk; /* setting same exp to both sensor using stereo handle */
      numExpLines = numExpLines << 4;
      numExpLines = numExpLines & 0xFFF0;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_EXPO_1_ADDRESS, numExpLines, OV7251_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      //LOGG_PRINT(LOG_DEBUG_E,NULL,"pixClk = %d, OV7251_DRVP_lineLengthPclk = %d, numExpLines = %d, time = %d, address = 0x%x, context = %d\n",pixClk,OV7251_DRVP_lineLengthPclk,numExpLines,params->exposureTime,deviceDescriptorP->sensorAddress,params->context);
   }

#ifdef OV7251_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_setExposureTime
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
static ERRG_codeE OV7251_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              sysClk;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   params->exposureTime = 0;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_EXPO_1_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines |= ((config.accessRegParams.data & 0xF) << 12);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_EXPO_2_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines |= ((config.accessRegParams.data & 0xFF) << 4);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_EXPO_3_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines |= ((config.accessRegParams.data & 0xF0) >> 4);

   sysClk = OV7251_DRVP_getSysClk(handle);
   params->exposureTime = (numExpLines * OV7251_DRVP_lineLengthPclk) / sysClk;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d, OV7251_DRVP_lineLengthPclk = %d, pixClk = %d, context = %d\n",params->exposureTime,numExpLines,OV7251_DRVP_lineLengthPclk, sysClk,params->context);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_setGain
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
static ERRG_codeE OV7251_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32 analogGain = (UINT32)params->analogGain;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   if (analogGain > 0x3FF)
   {
      //clip to max value
      analogGain = 0x3FF;
   }
   params->digitalGain = 0; //no digital gain in this sensor

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_GAIN_1_ADDRESS, (analogGain >> 8) & 0x3, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_GAIN_2_ADDRESS, analogGain & 0xFF, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_getGain
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
static ERRG_codeE OV7251_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT            *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32 analogGain;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
      return SENSOR__RET_SUCCESS;

   params->digitalGain = 0; //no digital gain in this sensor

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_GAIN_2_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   analogGain = config.accessRegParams.data & 0xFF;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_AEC_GAIN_1_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   analogGain |= (config.accessRegParams.data & 0x3) << 8;

   params->analogGain = (float)analogGain;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_setExposureMode
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
static ERRG_codeE OV7251_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_setImgOffsets
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
static ERRG_codeE OV7251_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_getAvgBrighness
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
static ERRG_codeE OV7251_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_setOutFormat
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
static ERRG_codeE OV7251_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_setPowerFreq
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
static ERRG_codeE OV7251_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_setStrobe
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
static ERRG_codeE OV7251_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   retVal = OV7251_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_setAutoExposureAvgLuma
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
static ERRG_codeE OV7251_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_trigger
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
static ERRG_codeE OV7251_DRVP_trigger(IO_HANDLE handle, void *pParams)
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
*  Function Name: OV7251_DRVP_mirrorFlip
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
static ERRG_codeE OV7251_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV7251_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, OV7251_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV7251_DRVP_open
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
static ERRG_codeE OV7251_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   BOOL                                foundNewInstance  = FALSE;
   UINT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   GME_DRVG_unitClkDivT                gmeClkConfig;

   FIX_UNUSED_PARAM_WARN(deviceId);

   for(sensorInstanceId = 0; sensorInstanceId < OV7251_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
   {
      deviceDescriptorP = &OV7251_DRVP_deviceDesc[sensorInstanceId];
      if(deviceDescriptorP->deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
         foundNewInstance = TRUE;
         break;
      }
   }

   if(foundNewInstance == TRUE)
   {
      deviceDescriptorP->deviceStatus     = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
      deviceDescriptorP->sensorAddress    = OV7251_DRVP_I2C_MASTER_ADDRESS;
      deviceDescriptorP->ioctlFuncList    = OV7251_DRVP_ioctlFuncList;
      deviceDescriptorP->i2cInstanceId    = pOpenParams->i2cInstanceId;
      deviceDescriptorP->i2cSpeed         = pOpenParams->i2cSpeed;
      deviceDescriptorP->sensorType       = pOpenParams->sensorType;
      deviceDescriptorP->tableType        = pOpenParams->tableType;
      deviceDescriptorP->powerGpioMaster  = pOpenParams->powerGpioMaster;
      deviceDescriptorP->fsinGpio         = pOpenParams->fsinGpio;
      deviceDescriptorP->sensorClk        = pOpenParams->sensorClk;
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
      if(pOpenParams->sensorType == INU_DEFSG_SENSOR_TYPE_STEREO_E)
      {
         deviceDescriptorP->sensorAddress = OV7251_DRVP_I2C_GLOBAL_ADDRESS;
      }

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
*  Function Name: OV7251_DRVP_close_gpio
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
static ERRG_codeE OV7251_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd, size_read, i=0, j=OV_7251P_MAX_NUM_E;
   char        buf[128];
   GPIO_DRVG_gpioOpenParamsT     params;
   char *fileName;
   if (sensorType==INU_DEFSG_SENSOR_TYPE_SINGLE_E)
      i=OV_7251P_XSHUTDOWN_W_E;
   else
      j=OV_7251P_XSHUTDOWN_W_E;
   for (; i < j; i++)
   {
      switch ( i )
      {
         case OV_7251P_XSHUTDOWN_R_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_R";
         break;
         case OV_7251P_XSHUTDOWN_L_E:
            fileName = "/proc/device-tree/sensor_on/stereo/XSHUTDOWN_L";
         break;
         case OV_7251P_FSIN_CV_E:
            fileName = "/proc/device-tree/sensor_on/stereo/FSIN_CV";
         break;
         case OV_7251P_XSHUTDOWN_W_E:
            fileName = "/proc/device-tree/sensor_on/webcam/XSHUTDOWN_W";
         break;
         case OV_7251P_FSIN_CLR_E:
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
            gpiosOv7251[i]=atoi(buf);
            if(gpiosOv7251[i]!=-1)
            {
               params.gpioNum = gpiosOv7251[i];
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
*  Function Name: OV7251_DRVP_close
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
static ERRG_codeE OV7251_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;

   OV7251_DRVP_powerdown(handle, NULL);
   OV7251_DRVP_close_gpio(deviceDescriptorP->sensorType);

   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: OV7251_DRVP_ioctl
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
static ERRG_codeE OV7251_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: OV7251_DRVG_init
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
ERRG_codeE OV7251_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(OV7251_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < OV7251_DRVP_MAX_NUMBER_SENSOR_INSTANCE; sensorInstanceId++)
      {
         deviceDescriptorP = &OV7251_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_OV_7251_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
      }
      OV7251_DRVP_isDrvInitialized = TRUE;
   }


   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &OV7251_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &OV7251_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &OV7251_DRVP_open;
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

