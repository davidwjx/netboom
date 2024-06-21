/****************************************************************************
 *
 *   FileName: AR0234_drv.c
 *
 *   Author:  Giyora A.
 *
 *   Date:
 *
 *   Description: AR0234 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ar0234_drv.h"
#include "ar0234_cfg_tbl.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include <unistd.h>
#include <errno.h>


/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   AR0234_XSHUTDOWN_R_E,
   AR0234_XSHUTDOWN_L_E,
   AR0234_FSIN_CV_E,
   AR0234_XSHUTDOWN_W_E,
   AR0234_FSIN_CLR_E,
   AR0234_MAX_NUM_E
}AR0234_gpioNumEntryE;
int gpiosAR0234[AR0234_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(AR0234_gpioNumEntryE) if (gpiosAR0234[AR0234_gpioNumEntryE] != -1)


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE AR0234_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE AR0234_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE AR0234_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE AR0234_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE AR0234_DRVP_close(IO_HANDLE handle);
static ERRG_codeE AR0234_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      AR0234_DRVP_findFreeSlot();
static ERRG_codeE AR0234_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_configGpio(AR0234_gpioNumEntryE gpioNumEntry,GPIO_DRVG_gpioStateE state);

static ERRG_codeE AR0234_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE AR0234_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE AR0234_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR0234_DRVP_powerup(IO_HANDLE handle, void *pParams);
static UINT16     AR0234_DRVP_getSysClk(IO_HANDLE handle);





/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define AR0234_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define AR0234_DRVP_CHANGE_ADDR_SLEEP_TIME                            (1*1000)   // 1ms
#define AR0234_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define AR0234_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define AR0234_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define AR0234_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define AR0234_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define AR0234_DRVP_16_BIT_ACCESS                                     (16)
#define AR0234_DRVP_8_BIT_ACCESS                                      (8)
#define AR0234_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)
//#define AR0234_DRVP_GROUP_MODE_EXP_CHANGE
///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define AR0234_DRVP_SC_MODE_SELECT_ADDRESS                            (0x301A)	//[Dima]
#define AR0234_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)
#define AR0234_DRVP_SC_SCCB_ID1_ADDRESS                               (0x0109)
#define AR0234_DRVP_SC_SCCB_ID2_ADDRESS                               (0x302B)
#define AR0234_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x3001)
#define AR0234_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x3000)

//#define AR0234_DRVP_SC_CLKRST6_ADDRESS                                (0x301C)	//[Dima]
#define AR0234_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define AR0234_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define AR0234_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define AR0234_DRVP_GRP_SW_CTRL_ADDRESS                               (0x320B)

#define AR0234_DRVP_AEC_EXPO_MIN_ADDRESS                              (0x311C)
#define AR0234_DRVP_AEC_EXPO_MAX_ADDRESS                              (0x311E)


#define AR0234_DRVP_AEC_GAIN_1_ADDRESS                                (0x3060)
#define AR0234_DRVP_AEC_GAIN_2_ADDRESS                                (0x350B)
#define AR0234_DRVP_TIMING_X_OUTPUT_SIZES_LOW_ADDRESS                 (0x3808)
#define AR0234_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x300C)	//[Dima] - new (0x380C)
#define AR0234_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define AR0234_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define AR0234_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define AR0234_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define AR0234_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define AR0234_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)

#define AR0234_DRVP_STROBE_EN_ADDRESS                                 (0x3005)
#define AR0234_DRVP_STROBE_POLARITY_ADDRESS                           (0x3B96)
#define AR0234_DRVP_STROBE_OFFSET_ADDRESS                             (0x3B8B)
#define AR0234_DRVP_STROBE_DURATION_HIGH_ADDRESS                      (0x3B8E)
#define AR0234_DRVP_STROBE_DURATION_LOW_ADDRESS                       (0x3B8F)
#define AR0234_DRVP_STROBE_MASK_ADDRESS                               (0x3B81)
#define AR0234_DRVP_PWM_CTRL_29_ADDRESS                               (0x3929)
#define AR0234_DRVP_PWM_CTRL_2A_ADDRESS                               (0x392A)


#define AR0234_DRVP_LOWPWR00_ADDRESS                                  (0x3C00)
#define AR0234_DRVP_LOWPWR01_ADDRESS                                  (0x3C01)
#define AR0234_DRVP_LOWPWR02_ADDRESS                                  (0x3C02)
#define AR0234_DRVP_LOWPWR03_ADDRESS                                  (0x3C03)
#define AR0234_DRVP_LOWPWR04_ADDRESS                                  (0x3C04)
#define AR0234_DRVP_LOWPWR05_ADDRESS                                  (0x3C05)
#define AR0234_DRVP_LOWPWR06_ADDRESS                                  (0x3C06)
#define AR0234_DRVP_LOWPWR07_ADDRESS                                  (0x3C07)
#define AR0234_DRVP_LOWPWR0E_ADDRESS                                  (0x3C0E)
#define AR0234_DRVP_LOWPWR0F_ADDRESS                                  (0x3C0F)
#define AR0234_DRVP_ANA_CORE_6_ADDRESS                                (0x3666)

///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define AR0234_DRVP_SENSOR_TYPE_ADDRESS                               (0x3000)
#define AR0234_DRVP_Y_ADDR_START_ADDRESS                              (0x3002)
#define AR0234_DRVP_X_ADDR_START_ADDRESS                              (0x3004)
#define AR0234_DRVP_Y_ADDR_END_ADDRESS                                (0x3006)
#define AR0234_DRVP_X_ADDR_END_ADDRESS                                (0x3008)
#define AR0234_DRVP_FRAME_LENGTH_LINES_ADDRESS                        (0x300A)
#define AR0234_DRVP_LINE_LENGTH_PCK_ADDRESS                           (0x300C)
#define AR0234_DRVP_COARSE_INTEGRATION_TIME_A_ADDRESS                 (0x3012)
#define AR0234_DRVP_FINE_INTEGRATION_TIME_A_ADDRESS                   (0x3014)
#define AR0234_DRVP_COARSE_INTEGRATION_TIME_B_ADDRESS                 (0x3016)
#define AR0234_DRVP_FINE_INTEGRATION_TIME_B_ADDRESS                   (0x3018)
#define AR0234_DRVP_RESET_AND_MISC_CONTROL_ADDRESS                    (0x301A)
#define AR0234_DRVP_VT_PIX_CLK_DIVIDER_ADDRESS                        (0x302A)
#define AR0234_DRVP_VT_SYS_CLK_DIVIDER_ADDRESS                        (0x302C)
#define AR0234_DRVP_OP_PIX_CLK_DIVIDER_ADDRESS                        (0x3036)
#define AR0234_DRVP_OP_SYS_CLK_DIVIDER_ADDRESS                        (0x3038)
#define AR0234_DRVP_PRE_PLL_DIVIDER_ADDRESS                           (0x302E)
#define AR0234_DRVP_PLL_MULTIPLIER_ADDRESS                            (0x3030)
#define AR0234_DRVP_DIGITAL_BINNING_ADDRESS                           (0x3032)
#define AR0234_DRVP_READ_MODE_ADDRESS                                 (0x3040)
#define AR0234_DRVP_FLASH_ADDRESS                                     (0x3046)
#define AR0234_DRVP_GREEN1_GAIN_ADDRESS                               (0x3056)
#define AR0234_DRVP_BLUE_GAIN_ADDRESS                                 (0x3058)
#define AR0234_DRVP_RED_GAIN_ADDRESS                                  (0x305A)
#define AR0234_DRVP_GREEN2_GAIN_ADDRESS                               (0x305C)
#define AR0234_DRVP_GLOBAL_GAIN_ADDRESS                               (0x305E)
#define AR0234_DRVP_ANALOG_GAIN_ADDRESS                               (0x3060)
#define AR0234_DRVP_SLEW_RATE_ADDRESS                                 (0x306E)
#define AR0234_DRVP_X_ADDR_START_CB_ADDRESS                           (0x308A)
#define AR0234_DRVP_Y_ADDR_START_CB_ADDRESS                           (0x308C)
#define AR0234_DRVP_X_ADDR_END_CB_ADDRESS                             (0x308E)
#define AR0234_DRVP_Y_ADDR_END_CB_ADDRESS                             (0x3090)
#define AR0234_DRVP_X_ODD_INCR_ADDRESS                                (0x30A2)
#define AR0234_DRVP_Y_ODD_INCR_ADDRESS                                (0x30A6)
#define AR0234_DRVP_Y_ODD_INCR_CB_ADDRESS                             (0x30A8)
#define AR0234_DRVP_FRAME_LENGTH_LINES_B_ADDRESS                      (0x30AA)
#define AR0234_DRVP_GREEN1_GAIN_CB_ADDRESS                            (0x30BC)
#define AR0234_DRVP_BLUE_GAIN_CB_ADDRESS                              (0x30BE)
#define AR0234_DRVP_RED_GAIN_CB_ADDRESS                               (0x30C0)
#define AR0234_DRVP_GREEN2_GAIN_CB_ADDRESS                            (0x30C2)
#define AR0234_DRVP_GLOBAL_GAIN_CB_ADDRESS                            (0x30C4)
#define AR0234_DRVP_GRR_CONTROL1_ADDRESS                              (0x30CE)


#define AR0234_DRVP_LINE_LENGTH_PCK_VAL                               (1390)
#define AR0234_DRVP_ANALOG_GAIN_MASK                                   (0x3)
#define AR0234_DRVP_ANALOG_GAIN_START_BIT                              (4)
#define AR0234_DRVP_ANALOG_GAIN_CB_START_BIT                           (8)


///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define AR0234_DRVP_SC_CHIP_ID                                        (0x0A56)
#define AR0234_DRVP_I2C_DEFAULT_ADDRESS                               (0x20)
#define AR0234_DRVP_I2C_MASTER_ADDRESS                                (AR0234_DRVP_I2C_DEFAULT_ADDRESS)
#define AR0234_DRVP_I2C_SLAVE_ADDRESS                                 (0x30)
#define AR0234_DRVP_I2C_GLOBAL_ADDRESS                                (0xC0)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  AR0234_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   AR0234_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        AR0234_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      AR0234_DRVP_resetSensors,            \
                                                                                                      AR0234_DRVP_initSensor,              \
                                                                                                      AR0234_DRVP_configSensors,           \
                                                                                                      AR0234_DRVP_startSensors,            \
                                                                                                      AR0234_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      AR0234_DRVP_getDeviceId,             \
                                                                                                      AR0234_DRVP_loadPresetTable,         \
                                                                                                      AR0234_DRVP_setFrameRate,            \
                                                                                                      AR0234_DRVP_setExposureTime,         \
                                                                                                      AR0234_DRVP_getExposureTime,         \
                                                                                                      AR0234_DRVP_setExposureMode,         \
                                                                                                      AR0234_DRVP_setImgOffsets,           \
                                                                                                      AR0234_DRVP_getAvgBrighness,         \
                                                                                                      AR0234_DRVP_setOutFormat,            \
                                                                                                      AR0234_DRVP_setPowerFreq,            \
                                                                                                      AR0234_DRVP_setStrobe,               \
                                                                                                      AR0234_DRVP_getSensorClks,           \
                                                                                                      AR0234_DRVP_setGain,                 \
                                                                                                      AR0234_DRVP_getGain,                 \
                                                                                                      AR0234_DRVP_trigger,                 \
                                                                                                      AR0234_DRVP_mirrorFlip,              \
                                                                                                      AR0234_DRVP_powerup ,                \
                                                                                                      AR0234_DRVP_powerdown ,              \
                                                                                                      AR0234_DRVP_changeSensorAddress,     \
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

static UINT16 AR0234_DRVP_lineLengthPclk;
static UINT16 AR0234_DRVP_x_output_size;
static UINT16 AR0234_DRVP_vts;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static UINT16 AR0234_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                           retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT    config;
   GEN_SENSOR_DRVG_specificDeviceDescT  *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->pixClk)
   {
      UINT16 pll_prediv, pll_mult, vt_sys_clk_div, vt_pix_clk_div, sys_clk, pxlClk, ref_clk;

      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_PLL_MULTIPLIER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult = ( config.accessRegParams.data ) & 0xFFFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_PRE_PLL_DIVIDER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_prediv = ( config.accessRegParams.data ) & 0xFFFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_VT_SYS_CLK_DIVIDER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      vt_sys_clk_div = ( config.accessRegParams.data ) & 0xFFFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_VT_PIX_CLK_DIVIDER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      vt_pix_clk_div = ( config.accessRegParams.data ) & 0xFFFF;

      //sys_clk = AR0234_DRVP_getSysClk(handle);


      if (ERRG_SUCCEEDED(retVal))
      {
         if (( pll_prediv != 0 ) && ( vt_sys_clk_div != 0) &&  (vt_pix_clk_div != 0))
         {
            deviceDescriptorP->pixClk = (ref_clk * pll_mult) / (pll_prediv * vt_sys_clk_div * vt_pix_clk_div);
            deviceDescriptorP->pixClk = deviceDescriptorP->pixClk/2; // /2=(numLanes/4)
         }
         else
         {
            deviceDescriptorP->pixClk = 0;
            LOGG_PRINT(LOG_ERROR_E, NULL, "pll_mult = %d, pll_prediv = %d, vt_sys_clk_div = %d, vt_pix_clk_div = %d, ref_clk = %d\n", pll_mult, pll_prediv, vt_sys_clk_div, vt_pix_clk_div, ref_clk);
            return ERR_UNEXPECTED;
         }
      }
   }
   return deviceDescriptorP->pixClk;
}


static UINT16 AR0234_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                           retVal = SENSOR__RET_SUCCESS;
   UINT16                               ref_clk;
   GEN_SENSOR_DRVG_sensorParametersT    config;
   GEN_SENSOR_DRVG_specificDeviceDescT  *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (!deviceDescriptorP->sysClk)
   {
      UINT16 pll_prediv = 0, pll_mult = 0, sysClk_div = 0, pxlClk_div = 0, sys_clk = 0;
      config.accessRegParams.data = 0;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_PRE_PLL_DIVIDER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_prediv = ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_PLL_MULTIPLIER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll_mult = ( config.accessRegParams.data ) & 0xFF;


      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_OP_SYS_CLK_DIVIDER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      sysClk_div = ( config.accessRegParams.data ) & 0xFF;


      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_OP_PIX_CLK_DIVIDER_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pxlClk_div = ( config.accessRegParams.data ) & 0xFF;


      deviceDescriptorP->sysClk = ref_clk*pll_mult;
      deviceDescriptorP->sysClk = (deviceDescriptorP->sysClk/(pll_prediv*pxlClk_div*sysClk_div))*2;
//      deviceDescriptorP->sysClk = deviceDescriptorP->sysClk/2;// /2=(numLanes/4)

      printf(GREEN("pll2_prediv = %d, pll_mult = %d, pll_div = %d, sysClk_div = %d, sysClk = %d, ref_clk = %d\n"),
                                  pll_prediv, pll_mult, sysClk_div, pxlClk_div,deviceDescriptorP->sysClk,ref_clk);
   }
   return deviceDescriptorP->sysClk;
   /// return system clock to the sys
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = AR0234_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = AR0234_DRVP_getPixClk(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   static unsigned int master=0;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = AR0234_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &AR0234_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     if ((pOpenParams->sensorSelect%2) == 0)
         deviceDescriptorP->sensorAddress   = AR0234_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
     else deviceDescriptorP->sensorAddress   = AR0234_DRVP_I2C_SLAVE_ADDRESS;
     deviceDescriptorP->ioctlFuncList   = AR0234_DRVP_ioctlFuncList;
     deviceDescriptorP->i2cInstanceId   = pOpenParams->i2cInstanceId;
     deviceDescriptorP->i2cSpeed        = pOpenParams->i2cSpeed;
     deviceDescriptorP->sensorType      = pOpenParams->sensorType; //todo remove
     deviceDescriptorP->tableType       = pOpenParams->tableType;
     deviceDescriptorP->powerGpioMaster = pOpenParams->powerGpioMaster;
     deviceDescriptorP->fsinGpio        = pOpenParams->fsinGpio;
     deviceDescriptorP->sensorClk       = pOpenParams->sensorClk;
     GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->powerGpioMaster);
     GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->fsinGpio);
     GEN_SENSOR_DRVP_setSensorRefClk(deviceDescriptorP->sensorClk, pOpenParams->sensorClkDiv);
     *handleP = (IO_HANDLE)deviceDescriptorP;
     LOGG_PRINT(LOG_INFO_E,NULL,"open sensor %d (pOpenParams->sensorSelect %x i2cNum %d type = %d, address = 0x%x inst %d deviceDescriptorP->sensorType %d)\n",
             (pOpenParams->sensorSelect%2),pOpenParams->sensorSelect,deviceDescriptorP->i2cInstanceId, deviceDescriptorP->sensorType,
             deviceDescriptorP->sensorAddress,deviceDescriptorP->i2cInstanceId,deviceDescriptorP->sensorType);

   }
   else
   {
      retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
   }

   return(retCode);
}




/****************************************************************************
*
*  Function Name: AR0234_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
***************************************************************************/
static ERRG_codeE AR0234_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn off the sensor
   retVal = AR0234_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
//   deviceDescriptorP->sensorAddress = AR0234_DRVP_I2C_MASTER_ADDRESS;
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
UINT8 low, high;
UINT16 test, test_1;
   FIX_UNUSED_PARAM_WARN(pParams);


   gpioParams.val = GPIO_DRVG_GPIO_STATE_SET_E;
   gpioParams.gpioNum = deviceDescriptorP->powerGpioMaster;
   retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(AR0234_DRVP_RESET_SLEEP_TIME);


   //read chipID
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR0234_DRVP_SC_CHIP_ID_LOW_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (config.accessRegParams.data != AR0234_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

   LOGG_PRINT(LOG_INFO_E,NULL,("power up (type = %d, address = 0x%x, handle = 0x%x),gpio %d, CHIP ID 0x%x\n"),
   deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle, gpioParams.gpioNum, config.accessRegParams.data);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 address = *(UINT8*)pParams;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
 /*  GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_SC_CTRL_2B_ADDRESS, address, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   deviceDescriptorP->sensorAddress = address;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_SC_CTRL_2B_ADDRESS, address, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(AR0234_DRVP_CHANGE_ADDRESS_SLEEP_TIME);

   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR0234_DRVP_SC_CTRL_2B_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Function Name: AR0234_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
   IO_HANDLE *handleP;
   GEN_SENSOR_DRVG_exposureTimeCfgT expParams;
   UINT16 regNum;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   FIX_UNUSED_PARAM_WARN(pParams);
   handleP = (IO_HANDLE*) deviceDescriptorP;

   LOGG_PRINT(LOG_INFO_E, NULL, "addr %x tableType:  %d, res %d\n",deviceDescriptorP->sensorAddress, deviceDescriptorP->tableType,params->loadTableParams.sensorResolution);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      sensorTablePtr = AR0234_CFG_TBLG_single_configTable;
      regNum = sizeof(AR0234_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = AR0234_CFG_TBLG_1280x800_configTable;
         regNum = sizeof(AR0234_CFG_TBLG_1280x800_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = AR0234_CFG_TBLG_stereo_vertical_binning_configTable;
         regNum = sizeof(AR0234_CFG_TBLG_stereo_vertical_binning_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {

         sensorTablePtr = AR0234_CFG_TBLG_stereo_full_vga_configTable;
         regNum = sizeof(AR0234_CFG_TBLG_stereo_full_vga_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {
         GEN_SENSOR_DRVG_sensorParametersT   config;
         config.accessRegParams.data = 0;
         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_TIMING_HTS_LOW_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         AR0234_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;

         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_TIMING_X_OUTPUT_SIZES_LOW_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         AR0234_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

         expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
         AR0234_DRVP_getExposureTime(handle,&expParams);
         AR0234_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
         AR0234_DRVP_enStrobe(deviceDescriptorP);

#ifdef AR0234_DRVP_GROUP_MODE_EXP_CHANGE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3017, 0xf2, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_SC_CLKRST6_ADDRESS, 0x04, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3200, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3201, 0x04, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         // enable group mode
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GRP_SW_CTRL_ADDRESS, 0x5, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
      }
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Error loading present table\n");
   }

   return retVal;
}



/****************************************************************************
*
*  Function Name: AR0234_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;


   retVal = AR0234_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16                              sysClk;
   (void)pParams;(void)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   LOGG_PRINT(LOG_INFO_E, NULL,"address %x params->triggerMode src %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.triggerSrc);

   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
 /*   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x300a, 0x62, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3222, 0x02, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3223, 0x44, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3231, 0x28, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3225, 0x04, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3226, 0x06, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3227, 0x06, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x322b, 0x0b, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3228, 0x0f, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3229, 0xcc, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
     GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x320c, 0x8F, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302c, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302d, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302e, 0x50, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302f, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x303f, 0x02, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x4242, 0x01, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3030, 0x80, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3030, 0x84, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR0234_DRVP_STROBE_MASK_ADDRESS, 0x55, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      //trial and error - if value is higher, then frame is curropted in full mode. when value is lower, can't reach high  then
      //the calculation was taken from the AR0234_DRVP_setFrameRate function.
      sysClk = AR0234_DRVP_getSysClk(handle);
      AR0234_DRVP_vts = (1000000 * sysClk) / (AR0234_DRVP_lineLengthPclk * 23);

      AR0234_DRVP_vts = AR0234_DRVP_vts/3; //TODO: verify strobe duration on scope
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR0234_DRVP_TIMING_VTS_LOW_ADDRESS, AR0234_DRVP_vts, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
 */
//       GEN_SENSOR_DRVG_ERROR_TEST(retVal);


     /*  IO_HANDLE *handleP;
       GEN_SENSOR_DRVG_exposureTimeCfgT expParams;
       UINT16 regNum;
       GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
       FIX_UNUSED_PARAM_WARN(pParams);
       handleP = (IO_HANDLE*) deviceDescriptorP;

    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)AR0234_CFG_trig_configTable, sizeof(AR0234_CFG_trig_configTable)/sizeof(AR0234_CFG_trig_configTable));


    */



       //     ();
      params->triggerModeParams.isTriggerSupported = 1;
   }
   else
   {
      params->triggerModeParams.isTriggerSupported = 0;
   }
   LOGG_PRINT(LOG_INFO_E, NULL,"address %x params->triggerModeParams.isTriggerSupported %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);

   params->triggerModeParams.gpioBitmap |= (((UINT64)1) << deviceDescriptorP->fsinGpio);
   params->triggerModeParams.pulseTime = 300;

   return retVal;

}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE AR0234_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  AR0234_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x2058, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE AR0234_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   LOGG_PRINT(LOG_INFO_E, NULL,"address %x params->triggerModeParams.isTriggerSupported %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);

//   	GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR0234_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x2058, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
//   	GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   	GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR0234_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x158, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   	GEN_SENSOR_DRVG_ERROR_TEST(retVal);
//   	GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR0234_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x2958, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
//   	GEN_SENSOR_DRVG_ERROR_TEST(retVal);
//   	GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR0234_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x0000, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
//   	GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //In trigger mode start and stop should be implemented with fsin gpio
   // AR0234_DRVP_configGpio(deviceDescriptorP->fsinGpio, GPIO_DRVG_GPIO_STATE_SET_E);
   return retVal;
}




/****************************************************************************
*
*  Function Name: OV9292_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              sysClk;
   UINT16                              frameLengthLine;

   sysClk = AR0234_DRVP_getSysClk(handle);
   if (ERRG_SUCCEEDED(retVal) && AR0234_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
        //frameLengthLine = (1000000 * sysClk) / (AR0234_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

#ifdef AR0234_DRVP_GROUP_MODE_EXP_CHANGE
        GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
        UINT32 groupHold;
        //change to next group
        deviceDescriptorP->group = !deviceDescriptorP->group;

        // group hold start
        groupHold = 0x00 | deviceDescriptorP->group;
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


      frameLengthLine = ((0x600*25)/params->setFrameRateParams.frameRate);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR0234_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      AR0234_DRVP_vts = frameLengthLine;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR0234_DRVP_TIMING_HTS_HIGH_ADDRESS, frameLengthLine - 6, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. AR0234_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,AR0234_DRVP_lineLengthPclk,frameLengthLine,sysClk);



#ifdef AR0234_DRVP_GROUP_MODE_EXP_CHANGE
      //group hold end
      groupHold = 0x10 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // group hold launch
      groupHold = 0xA0 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines,maxExpLines, pixel_line;
   UINT16                              sysClk = 0, pix_clk = 0;



	if(deviceDescriptorP)
	{
		pix_clk = AR0234_DRVP_getPixClk(handle);
		GEN_SENSOR_DRVG_ERROR_TEST(retVal);

		GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_LINE_LENGTH_PCK_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
		GEN_SENSOR_DRVG_ERROR_TEST(retVal);

		pixel_line = config.accessRegParams.data & 0xFFFF;

		if ((pixel_line != 0) && (pix_clk != 0))
		{
			numExpLines = params->exposureTime * pix_clk / pixel_line ;

         if (params->context == INU_DEFSG_SENSOR_CONTEX_A)
         {
			   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_COARSE_INTEGRATION_TIME_A_ADDRESS, numExpLines, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   			GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         }
         else
         {
            GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_COARSE_INTEGRATION_TIME_B_ADDRESS, numExpLines, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
            GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         }
		}
		else
		{
		    numExpLines = 0;
			LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR set exposure. numLines = %d, exposureTime = %d, pixel_line = %d,pix_clk = %d, context=%d\n", numExpLines, params->exposureTime, pixel_line, pix_clk, params->context);
			return ERR_UNEXPECTED;
		}
	}
	else
	{
		LOGG_PRINT(LOG_ERROR_E,NULL,"set exposure with bad handle!\n");
		return ERR_UNEXPECTED;
	}

   LOGG_PRINT(LOG_INFO_E,NULL,"SET EXPOSURE. Addr 0x%x, numLines = %d, exposureTime = %d, pixel_line = %d,pix_clk = %d,context=%d\n",deviceDescriptorP->sensorAddress, numExpLines,params->exposureTime,pixel_line,pix_clk,params->context);

   return SENSOR__RET_SUCCESS;

}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0, pixel_line;
   UINT16                              sysClk, pxl_clk;

//   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
//      return SENSOR__RET_SUCCESS;

   pixel_line = AR0234_DRVP_lineLengthPclk;//config.accessRegParams.data & 0xFFFF;

   params->exposureTime = 0;

   pxl_clk = AR0234_DRVP_getPixClk(handle);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (params->context == INU_DEFSG_SENSOR_CONTEX_A)
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_COARSE_INTEGRATION_TIME_A_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
   else
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_COARSE_INTEGRATION_TIME_B_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }

   numExpLines = config.accessRegParams.data & 0xFFFF;

   //GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_LINE_LENGTH_PCK_ADDRESS, AR0234_DRVP_2_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (pxl_clk != 0)
   {
      params->exposureTime = ((numExpLines * pixel_line) + (pxl_clk - 1)) / pxl_clk;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,ERR_UNEXPECTED,"Failed to get exposure. context = %d, num_lines = %d, pixel_line = %d, pxl_clk = %d, exposure = %d\n",params->context,numExpLines,pixel_line,pxl_clk,params->exposureTime);
      return ERR_UNEXPECTED;
   }

   LOGG_PRINT(LOG_DEBUG_E,NULL,"context = %d, num_lines = %d, pixel_line = %d, pxl_clk = %d, exposure = %d\n",params->context,numExpLines,pixel_line,pxl_clk,params->exposureTime);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT            *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              analogGain  = (UINT32)params->analogGain;
   UINT32                              digitalGain = (UINT32)params->digitalGain;

   if ((analogGain > 3) || (digitalGain > 0xFF))
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"set gain with bad values. analog:0x%x, digital:0x%x\n", analogGain, digitalGain);
      return ERR_UNEXPECTED;
   }

   if (params->context == INU_DEFSG_SENSOR_CONTEX_A)
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GLOBAL_GAIN_ADDRESS, digitalGain, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_ANALOG_GAIN_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data &= ~(AR0234_DRVP_ANALOG_GAIN_MASK << AR0234_DRVP_ANALOG_GAIN_START_BIT); //reset the gain bits
      config.accessRegParams.data |= (analogGain & AR0234_DRVP_ANALOG_GAIN_MASK) << AR0234_DRVP_ANALOG_GAIN_START_BIT; //set new value
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_ANALOG_GAIN_ADDRESS, config.accessRegParams.data, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
   else
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GLOBAL_GAIN_CB_ADDRESS, digitalGain, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_ANALOG_GAIN_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data &= ~(AR0234_DRVP_ANALOG_GAIN_MASK << AR0234_DRVP_ANALOG_GAIN_CB_START_BIT); //reset the gain bits
      config.accessRegParams.data |= (analogGain & AR0234_DRVP_ANALOG_GAIN_MASK) << AR0234_DRVP_ANALOG_GAIN_CB_START_BIT; //set new value
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_ANALOG_GAIN_ADDRESS, config.accessRegParams.data, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }

    LOGG_PRINT(LOG_INFO_E,NULL,"set gain.Address 0x%x, analog = %d, digital = %d\n", deviceDescriptorP->sensorAddress, analogGain, digitalGain);

   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;

   if (params->context == INU_DEFSG_SENSOR_CONTEX_A)
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_ANALOG_GAIN_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      params->analogGain = (config.accessRegParams.data >> AR0234_DRVP_ANALOG_GAIN_START_BIT) & AR0234_DRVP_ANALOG_GAIN_MASK ;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GLOBAL_GAIN_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      params->digitalGain = config.accessRegParams.data;
   }
   else
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_ANALOG_GAIN_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      params->analogGain = (config.accessRegParams.data >> AR0234_DRVP_ANALOG_GAIN_CB_START_BIT) & AR0234_DRVP_ANALOG_GAIN_MASK ;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_GLOBAL_GAIN_CB_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      params->digitalGain = config.accessRegParams.data;
   }

	LOGG_PRINT(LOG_INFO_E,NULL,"context = %d, analog = %d, digital = %d\n", params->context,params->analogGain,params->digitalGain);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
*  Function Name: AR0234_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/

static INT32 AR0234_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(AR0234_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(AR0234_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum, sysClk = 0;

#ifdef GGGG
   sysClk = AR0234_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (AR0234_DRVP_lineLengthPclk - 1)) / (AR0234_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_PWM_CTRL_27_ADDRESS, (strobeUpTicksNum & 0xFFFF), AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#ifdef AR0234_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 strobeStartPoint;
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = AR0234_DRVP_vts - strobeUpTicksNum - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_PWM_CTRL_29_ADDRESS, (strobeStartPoint & 0xFFFF), AR0234_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   LOGG_PRINT(LOG_DEBUG_E,NULL,"usec = %d, AR0234_DRVP_lineLengthPclk = %d, sysClk = %d, strobeUpTicksNum = 0x%x\n", usec, AR0234_DRVP_lineLengthPclk, sysClk, strobeUpTicksNum);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR0234_DRVP_STROBE_EN_ADDRESS, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E,NULL,"strobe reg = 0x%x)\n",config.accessRegParams.data);

#ifdef GGGG
#ifndef AR0234_DRVP_GROUP_MODE_EXP_CHANGE

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_PWM_CTRL_2F_ADDRESS, 0x40, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_PWM_CTRL_24_ADDRESS, 0x0, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_STROBE_MASK_ADDRESS, 0xFF, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_STROBE_EN_ADDRESS, 0x08, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#else
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3006, 0x0c, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3210, 0x04, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3007, 0x02, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR0234_DRVP_SC_CLKRST6_ADDRESS, 0xf2, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3025, 0x02, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382c 07  ; hts global tx msb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382c, 0x07, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382d 10 ; hts global tx lsb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382d, 0x10, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3920 ff ; strobe pattern
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3920, 0xff, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3923 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3923, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3924 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3924, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3925 00 ; width frame span effective width
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3925, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3926, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3927, 0x01, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3928, 0x80, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3929, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392a, 0x14, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392b, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392c, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392d, 0x04, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392e, 0x00, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392f, 0xcb, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x38b3, 0x07, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3885, 0x07, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382b, 0x5a, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3670, 0x68, AR0234_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#endif
#endif

   return retVal;
}

static ERRG_codeE AR0234_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GPIO_DRVG_gpioSetDirParamsT         gpioDir;

   if (gpioNum != 0xFFFF)
   {
       gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
       gpioDir.gpioNum = gpioNum;
       IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

       gpioParams.val = state;
       gpioParams.gpioNum =gpioNum;
       retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }
    return (retVal);

}


/****************************************************************************
*
*  Function Name: AR0234_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;
   return retVal;
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
   return retCode;
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   AR0234_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   AR0234_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: AR0234_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
static ERRG_codeE AR0234_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
     return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: AR0234_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR0234 sensor driver
*
****************************************************************************/
ERRG_codeE AR0234_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(AR0234_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {

         deviceDescriptorP = &AR0234_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_AR_0234_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      AR0234_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &AR0234_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &AR0234_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &AR0234_DRVP_open;
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


