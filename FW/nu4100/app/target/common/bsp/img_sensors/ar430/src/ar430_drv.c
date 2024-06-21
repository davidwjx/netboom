/****************************************************************************
 *
 *   FileName: AR430_drv.c
 *
 *   Author:  Gyiora A.
 *
 *   Date:
 *
 *   Description: AR430 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ar430_drv.h"
#include "ar430_cfg_tbl.h"
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
   AR_430P_XSHUTDOWN_R_E,
   AR_430P_XSHUTDOWN_L_E,
   AR_430P_FSIN_CV_E,
   AR_430P_XSHUTDOWN_W_E,
   AR_430P_FSIN_CLR_E,
   AR_430P_MAX_NUM_E
}AR_430P_gpioNumEntryE;
int gpiosOv430[AR_430P_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(AR_430P_gpioNumEntryE) if (gpiosOv430[AR_430P_gpioNumEntryE] != -1)


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE AR430_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE AR430_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE AR430_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE AR430_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE AR430_DRVP_close(IO_HANDLE handle);
static ERRG_codeE AR430_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      AR430_DRVP_findFreeSlot();
static ERRG_codeE AR430_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_configGpio(AR_430P_gpioNumEntryE gpioNumEntry,GPIO_DRVG_gpioStateE state);

static ERRG_codeE AR430_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE AR430_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE AR430_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR430_DRVP_powerup(IO_HANDLE handle, void *pParams);





/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define AR430_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define AR430_DRVP_CHANGE_ADDRESS_SLEEP_TIME                         (1*1000)   // 1ms
#define AR430_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define AR430_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define AR430_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define AR430_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define AR430_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define AR430_DRVP_16_BIT_ACCESS                                     (16)
#define AR430_DRVP_8_BIT_ACCESS                                      (8)
#define AR430_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)
//#define AR430_DRVP_GROUP_MODE_EXP_CHANGE

///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define AR430_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define AR430_DRVP_SOFTWARE_RESET                                    (0x0103)

#define AR430_DRVP_I2C_IDS_ADDRESS                                   (0x31FC) //


#define AR430_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x3000)
#define AR430_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x3001)
#define AR430_DRVP_SC_REG5_ADDRESS                                   (0x3027) //


#define AR430_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define AR430_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define AR430_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)

#define AR430_DRVP_AEC_GAIN_1_ADDRESS                                (0x3509) //0-3 fraction  bits 4- 7 gain

#define AR430_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS                      (0x3808)
#define AR430_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define AR430_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define AR430_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define AR430_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define AR430_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define AR430_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define AR430_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)

#define AR430_DRVP_PLL1_PRE_DIV0_ADDRESS                             (0X030A)
#define AR430_DRVP_PLL1_PRE_DIV_ADDRESS                              (0x0300)
#define AR430_DRVP_PLL1_MULT_H_ADDRESS                               (0x0301)
#define AR430_DRVP_PLL1_MULT_L_ADDRESS                               (0x0302)
#define AR430_DRVP_PLL1_SYS_PRE_DIV_ADDRESS                          (0x0305)
#define AR430_DRVP_PLL1_SYS_DIV_ADDRESS                              (0x0306)
#define AR430_DRVP_PLL1_M_DIV_ADDRESS                                (0x0303)
#define AR430_DRVP_PLL1_MIPI_DIV_ADDRESS                             (0x0304)
#define AR430_DRVP_PLL2_PRE_DIV0_ADDRESS                             (0x0314)
#define AR430_DRVP_PLL2_PRE_DIV_ADDRESS                              (0x030B)
#define AR430_DRVP_PLL2_LOOP_DIV_H_ADDRESS                           (0x030C)
#define AR430_DRVP_PLL2_LOOP_DIV_L_ADDRESS                           (0x030D)
#define AR430_DRVP_PLL2_SYS_PRE_DIV_ADDRESS                          (0x030F)
#define AR430_DRVP_PLL2_SYS_DIV_ADDRESS                              (0x030E)
#define AR430_DRVP_PLL2_DAC_DIV_ADDRESS                              (0x0313)
#define AR430_DRVP_PLL2_SA1_DIV_ADDRESS                              (0x0312)


#define AR430_DRVP_SC_CLKRST6_ADDRESS                                (0x301C)
#define AR430_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define AR430_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define AR430_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define AR430_DRVP_GRP_SW_CTRL_ADDRESS                               (0x320B)



#define AR430_DRVP_STROBE_EN_ADDRESS                                 (0x3006)
#define AR430_DRVP_STROBE_MASK_ADDRESS                               (0x3920)
#define AR430_DRVP_PWM_CTRL_2F_ADDRESS                               (0x392F)  // STROBE_POLARITY in AR430 0x3B96
#define AR430_DRVP_PWM_CTRL_24_ADDRESS                               (0x3924)  // STROBE_OFFSET_ADDRESS 0x3B8B
#define AR430_DRVP_PWM_CTRL_27_ADDRESS                               (0x3927)  // STROBE_DURATION_HIGH 0x3B8E
#define AR430_DRVP_PWM_CTRL_28_ADDRESS                               (0x3928)  // STROBE_DURATION_LOW 0x3B8F
#define AR430_DRVP_PWM_CTRL_29_ADDRESS                               (0x3929)



#define AR430_DRVP_LOWPWR00_ADDRESS                                  (0x3C00) //not exist
#define AR430_DRVP_LOWPWR01_ADDRESS                                  (0x4f00) //on 7251 (0x3C01)
#define AR430_DRVP_LOWPWR02_ADDRESS                                  (0x3C02) //not exist
#define AR430_DRVP_LOWPWR03_ADDRESS                                  (0x3C03) //not exist
#define AR430_DRVP_LOWPWR04_ADDRESS                                  (0x3C04) //not exist
#define AR430_DRVP_LOWPWR05_ADDRESS                                  (0x3C05) //not exist
#define AR430_DRVP_LOWPWR06_ADDRESS                                  (0x3C06) //not exist
#define AR430_DRVP_LOWPWR07_ADDRESS                                  (0x3C07) //not exist
#define AR430_DRVP_LOWPWR0E_ADDRESS                                  (0x3C0E) //not exist
#define AR430_DRVP_LOWPWR0F_ADDRESS                                  (0x3C0F) //not exist


#define AR430_DRVP_ANA_CORE_6_ADDRESS                                (0x3666)

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define AR430_DRVP_SC_CHIP_ID                                        (0x1550)
#define AR430_DRVP_I2C_DEFAULT_ADDRESS                               (0x6c)
#define AR430_DRVP_I2C_MASTER_ADDRESS                                (AR430_DRVP_I2C_DEFAULT_ADDRESS)
#define AR430_DRVP_I2C_SLAVE_ADDRESS                                 (0x6c)
#define AR430_DRVP_I2C_GLOBAL_ADDRESS                                (0x6c)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  AR430_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   AR430_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        AR430_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      AR430_DRVP_resetSensors,            \
                                                                                                      AR430_DRVP_initSensor,              \
                                                                                                      AR430_DRVP_configSensors,           \
                                                                                                      AR430_DRVP_startSensors,            \
                                                                                                      AR430_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,     \
                                                                                                      AR430_DRVP_getDeviceId,             \
                                                                                                      AR430_DRVP_loadPresetTable,         \
                                                                                                      AR430_DRVP_setFrameRate,            \
                                                                                                      AR430_DRVP_setExposureTime,         \
                                                                                                      AR430_DRVP_getExposureTime,         \
                                                                                                      AR430_DRVP_setExposureMode,         \
                                                                                                      AR430_DRVP_setImgOffsets,           \
                                                                                                      AR430_DRVP_getAvgBrighness,         \
                                                                                                      AR430_DRVP_setOutFormat,            \
                                                                                                      AR430_DRVP_setPowerFreq,            \
                                                                                                      AR430_DRVP_setStrobe,               \
                                                                                                      AR430_DRVP_getSensorClks,           \
                                                                                                      AR430_DRVP_setGain,                 \
                                                                                                      AR430_DRVP_getGain,                 \
                                                                                                      AR430_DRVP_trigger,                 \
                                                                                                      AR430_DRVP_mirrorFlip,              \
                                                                                                      AR430_DRVP_powerup ,                \
                                                                                                      AR430_DRVP_powerdown ,              \
                                                                                                      AR430_DRVP_changeSensorAddress,     \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub,               \
                                                                                                      GEN_SENSOR_DRVG_stub};

static UINT16 AR430_DRVP_lineLengthPclk;
static UINT16 AR430_DRVP_x_output_size;
static UINT16 AR430_DRVP_vts;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static UINT16 AR430_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
#ifdef GIY
   if (!deviceDescriptorP->pixClk)
   {
      UINT16 pll1_prediv0,pll1_prediv,pll1_mult,pll1_m_div, pll1_mipi_div, pix_clk, ref_clk;
      UINT8  predivVal[8] = {2,3,4,5,6,8,12,16};
      UINT8  mipiDiv[4]   = {4,5,6,8};

      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pix_clk = ref_clk;

      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL1_PRE_DIV0_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_prediv0 = ( config.accessRegParams.data ) & 0x1;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL1_PRE_DIV_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL1_MULT_H_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mult = (( config.accessRegParams.data ) & 0x3) << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL1_MULT_L_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mult |= ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,AR430_DRVP_PLL1_M_DIV_ADDRESS , AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_m_div = ( config.accessRegParams.data ) & 0x0F;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL1_MIPI_DIV_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mipi_div = ( config.accessRegParams.data ) & 0x03;

      pix_clk = pix_clk / (pll1_prediv0 + 1);
      pix_clk = (pix_clk*2) / predivVal[pll1_prediv];
      pix_clk = pix_clk * pll1_mult;
      pix_clk = pix_clk / (pll1_m_div + 1);
      pix_clk = pix_clk / mipiDiv[pll1_mipi_div];

      pix_clk = (pix_clk * AR430_DRVP_x_output_size) / (AR430_DRVP_lineLengthPclk);

      deviceDescriptorP->pixClk = pix_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll1_prediv0 = %d, pll1_prediv = %d, pll1_mult = %d, pll1_m_div %d pll1_mipi_div %d pixClk = %d, ref_clk = %d\n",
                                  pll1_prediv0, pll1_prediv, pll1_mult,pll1_m_div,pll1_mipi_div,deviceDescriptorP->pixClk,ref_clk);
   }
#endif
   return deviceDescriptorP->pixClk;

}

static UINT16 AR430_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
#ifdef GIY

   if (!deviceDescriptorP->sysClk)
   {
      UINT16 pll2_prediv,pll2_mult,pll2_sys_pre_div, pll2_sys_div, sys_clk, ref_clk;
      UINT8  predivVal[8]={2,3,4,5,6,8,12,16};
      UINT8  sysDiv[8]   ={2,3,4,5,6,7,8,10};

      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      sys_clk = ref_clk;

      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL2_PRE_DIV_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL2_SYS_PRE_DIV_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_pre_div = ( config.accessRegParams.data ) & 0xF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL2_SYS_DIV_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_div = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,AR430_DRVP_PLL2_LOOP_DIV_H_ADDRESS , AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_mult = config.accessRegParams.data&0x3 << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_PLL2_LOOP_DIV_L_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_mult |= config.accessRegParams.data&0xff;

      sys_clk = sys_clk*pll2_mult;
      sys_clk = (sys_clk*2) / predivVal[pll2_prediv];
      sys_clk = sys_clk/(pll2_sys_pre_div + 1);

      sys_clk = (sys_clk*2)/sysDiv[pll2_sys_div];
      deviceDescriptorP->sysClk = sys_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll2_prediv = %d, pll2_sys_pre_div = %d, pll2_sys_div = %d, pll2_mult %d sysClk = %d, ref_clk = %d\n", pll2_prediv, pll2_sys_pre_div, pll2_sys_div,pll2_mult,deviceDescriptorP->sysClk,ref_clk);
   }
#endif
   return deviceDescriptorP->sysClk;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = AR430_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = AR430_DRVP_getPixClk(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = AR430_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &AR430_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     deviceDescriptorP->sensorAddress   = AR430_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
     deviceDescriptorP->ioctlFuncList   = AR430_DRVP_ioctlFuncList;
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
     if(pOpenParams->sensorType == INU_DEFSG_SENSOR_TYPE_STEREO_E)
     {
        //need to add another adressfor second slave maybe from XML
        deviceDescriptorP->sensorAddress = AR430_DRVP_I2C_GLOBAL_ADDRESS;
     }
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
*  Function Name: AR430_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
***************************************************************************/
static ERRG_codeE AR430_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_INFO_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn off the sensor
   retVal = AR430_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
   deviceDescriptorP->sensorAddress = AR430_DRVP_I2C_MASTER_ADDRESS;
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_INFO_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x) gpio %d\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle,deviceDescriptorP->powerGpioMaster);

   //Turn on the sensor
   retVal = AR430_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_SET_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(AR430_DRVP_RESET_SLEEP_TIME*80);
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR430_DRVP_SC_CHIP_ID_HIGH_ADDRESS, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);

   LOGG_PRINT(LOG_INFO_E,NULL,
           "power up (type = %d, address = 0x%x, data 0x%x\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,config.accessRegParams.data);
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, 0x301a, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
  LOGG_PRINT(LOG_INFO_E,NULL,
           "power up (type = %d, address = 0x%x^^^^ 301a data %x\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,config.accessRegParams.data);
  config.accessRegParams.data |= 1;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x301a, config.accessRegParams.data&0xffff, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   OS_LYRG_usleep(100000);
  GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x301a, 0x98, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   OS_LYRG_usleep(100000);
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, 0x31fc, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
  LOGG_PRINT(LOG_INFO_E,NULL,
           "power up (type = %d, address = 0x%x^^^^ 31fc data %x\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,config.accessRegParams.data);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x31fc, config.accessRegParams.data&0xffff, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E,NULL,
           "power up (type = %d, address = 0x%x^^^^\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);

 OS_LYRG_usleep(100000);
   // reset sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_SOFTWARE_RESET, 1, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
 OS_LYRG_usleep(100000);

   // set sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_SOFTWARE_RESET, 0, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

 OS_LYRG_usleep(100000);
   //read chipID
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR430_DRVP_SC_CHIP_ID_HIGH_ADDRESS, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   if (config.accessRegParams.data != AR430_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 address = *(UINT8*)pParams;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
/* OS_LYRG_usleep(100000);

   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_I2C_IDS_ADDRESS, address, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(100000);

   deviceDescriptorP->sensorAddress = address;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_I2C_IDS_ADDRESS, address, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(AR430_DRVP_CHANGE_ADDRESS_SLEEP_TIME);

   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR430_DRVP_I2C_IDS_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Function Name: AR430_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
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

   LOGG_PRINT(LOG_INFO_E, NULL, "tableType: %d, res %d\n",deviceDescriptorP->tableType,params->loadTableParams.sensorResolution);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      sensorTablePtr = AR430_CFG_TBLG_single_configTable;
      regNum = sizeof(AR430_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = AR430_CFG_TBLG_stereo_vga_configTable;
         regNum = sizeof(AR430_CFG_TBLG_stereo_vga_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = AR430_CFG_TBLG_stereo_vertical_binning_configTable;
         regNum = sizeof(AR430_CFG_TBLG_stereo_vertical_binning_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {

         sensorTablePtr = AR430_CFG_TBLG_stereo_hd_configTable;
         INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
         regNum = sizeof(AR430_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
         if (modelType == INU_DEFSG_BOOT52_E)
         {
            sensorTablePtr = AR430_CFG_TBLG_mono_hd_raw8_configTable;
            regNum = sizeof(AR430_CFG_TBLG_mono_hd_raw8_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
         }
         else if (modelType == INU_DEFSG_BOOT10_E)
         {
            sensorTablePtr = AR430_CFG_TBLG_stereo_hd_smallreadout_configTable;
            regNum = sizeof(AR430_CFG_TBLG_stereo_hd_smallreadout_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
         }
      }
   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {

         GEN_SENSOR_DRVG_sensorParametersT   config;
         config.accessRegParams.data = 0;
 #ifdef GIY
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_TIMING_HTS_LOW_ADDRESS, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         AR430_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;

         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         AR430_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

         expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
         AR430_DRVP_getExposureTime(handle,&expParams);
         AR430_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
         AR430_DRVP_enStrobe(deviceDescriptorP);

#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3017, 0xf2, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_SC_CLKRST6_ADDRESS, 0x04, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3200, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3201, 0x04, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         // enable group mode
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GRP_SW_CTRL_ADDRESS, 0x5, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
#endif

    OS_LYRG_usleep(10000);
  //  GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x301a, 0x9c, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
//    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x100, 1, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

      OS_LYRG_usleep(10000);
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, 0x301a, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);

      LOGG_PRINT(LOG_INFO_E,NULL,
              "*********(type = %d, address = 0x%x, data 0x%x\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,config.accessRegParams.data);


    OS_LYRG_usleep(10000);
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, 0x3026, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      LOGG_PRINT(LOG_INFO_E,NULL,
              "*********(type = %d, address = 0x%x, 3026 data 0x%x\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,config.accessRegParams.data);
    OS_LYRG_usleep(10000);
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3026, 0xffcf, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
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
*  Function Name: AR430_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   retVal = AR430_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}


/****************************************************************************
*
*  Function Name: AR430_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16                              sysClk;
   (void)pParams;(void)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
#ifdef GIY
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x320c, 0x8F, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302c, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302d, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302e, 0x50, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302f, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x303f, 0x02, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x4242, 0x01, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3030, 0x80, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3030, 0x84, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR430_DRVP_STROBE_MASK_ADDRESS, 0x55, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      //trial and error - if value is higher, then frame is curropted in full mode. when value is lower, can't reach high fps then
      //the calculation was taken from the AR430_DRVP_setFrameRate function.
      sysClk = AR430_DRVP_getSysClk(handle);
      AR430_DRVP_vts = (1000000 * sysClk) / (AR430_DRVP_lineLengthPclk * 23);

      AR430_DRVP_vts = AR430_DRVP_vts/3; //TODO: verify strobe duration on scope
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR430_DRVP_TIMING_VTS_LOW_ADDRESS, AR430_DRVP_vts, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
      params->triggerModeParams.isTriggerSupported = 1;
   }
   else
   {
      params->triggerModeParams.isTriggerSupported = 0;
   }
   params->triggerModeParams.gpioBitmap |= (((UINT64)1) << deviceDescriptorP->fsinGpio);
   params->triggerModeParams.pulseTime = 300;

   return retVal;

}


/****************************************************************************
*
*  Function Name: AR430_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE AR430_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  AR430_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE AR430_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   if (!params->triggerModeParams.isTriggerSupported)
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR430_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
   return retVal;
}




/****************************************************************************
*
*  Function Name: AR9292_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              sysClk;
   UINT16                              frameLengthLine;

#ifdef GIY
   sysClk = AR430_DRVP_getSysClk(handle);
   if (ERRG_SUCCEEDED(retVal) && AR430_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
        //frameLengthLine = (1000000 * sysClk) / (AR430_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);
        frameLengthLine = ((1000000 * sysClk) + ((AR430_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate) - 1)) / (AR430_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
        GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
        UINT32 groupHold;
        //change to next group
        deviceDescriptorP->group = !deviceDescriptorP->group;

        // group hold start
        groupHold = 0x00 | deviceDescriptorP->group;
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR430_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        AR430_DRVP_vts = frameLengthLine;
        LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. AR430_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,AR430_DRVP_lineLengthPclk,frameLengthLine,sysClk);



#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
      //group hold end
      groupHold = 0x10 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // group hold launch
      groupHold = 0xA0 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


   }
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
   UINT16                              sysClk = 0;
#ifdef GIY

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   if (AR430_DRVP_lineLengthPclk != 0)
   {
      sysClk = AR430_DRVP_getSysClk(handle);
      numExpLines = ((params->exposureTime * sysClk) + (AR430_DRVP_lineLengthPclk - 1)) / AR430_DRVP_lineLengthPclk; /* setting same exp to both sensor using stereo handle */
      numExpLines = numExpLines << 4;
      numExpLines = numExpLines & 0xFFF0;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_AEC_EXPO_1_ADDRESS, numExpLines, AR430_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      //LOGG_PRINT(LOG_DEBUG_E,NULL,"sys_clk = %d, AR430_DRVP_lineLengthPclk = %d, numExpLines = %d, time = %d, address = 0x%x, context = %d\n",sysClk,AR430_DRVP_lineLengthPclk,numExpLines,params->exposureTime,deviceDescriptorP->sensorAddress,params->context);
   }

#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              sysClk;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
#ifdef GIY

   params->exposureTime = 0;

   if (ERRG_SUCCEEDED(retVal))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"get exposure (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_AEC_EXPO_1_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xF) << 12);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_AEC_EXPO_2_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xFF) << 4);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_AEC_EXPO_3_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xF0) >> 4);

      sysClk = AR430_DRVP_getSysClk(handle);
      params->exposureTime = (numExpLines * AR430_DRVP_lineLengthPclk) / sysClk;
      LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d, AR430_DRVP_lineLengthPclk = %d, sys_clk = %d, context = %d\n",params->exposureTime,numExpLines,AR430_DRVP_lineLengthPclk, sysClk,params->context);
   }
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
#ifdef GIY

   if (params->analogGain > 0xFF)
   {
      params->analogGain = 0xFF;
   }

#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_AEC_GAIN_1_ADDRESS, params->analogGain , AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);


#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_GROUP_ACCESS_ADDRESS, groupHold, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
#ifdef GIY

   params->analogGain = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_AEC_GAIN_1_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   params->analogGain = config.accessRegParams.data & 0xFF;
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
*  Function Name: AR430_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/

static INT32 AR430_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(AR430_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(AR430_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
#ifdef GIY

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR430_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum, sysClk = 0;
#ifdef GIY

   sysClk = AR430_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (AR430_DRVP_lineLengthPclk - 1)) / (AR430_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_PWM_CTRL_27_ADDRESS, (strobeUpTicksNum & 0xFFFF), AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#ifdef AR430_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 strobeStartPoint;
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = AR430_DRVP_vts - strobeUpTicksNum - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_PWM_CTRL_29_ADDRESS, (strobeStartPoint & 0xFFFF), AR430_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   LOGG_PRINT(LOG_DEBUG_E,NULL,"usec = %d, AR430_DRVP_lineLengthPclk = %d, sysClk = %d, strobeUpTicksNum = 0x%x\n", usec, AR430_DRVP_lineLengthPclk, sysClk, strobeUpTicksNum);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: AR430_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
#ifdef GIY

#ifndef AR430_DRVP_GROUP_MODE_EXP_CHANGE

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_PWM_CTRL_2F_ADDRESS, 0x40, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_PWM_CTRL_24_ADDRESS, 0x0, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_STROBE_MASK_ADDRESS, 0xFF, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_STROBE_EN_ADDRESS, 0x08, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#else
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3006, 0x0c, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3210, 0x04, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3007, 0x02, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR430_DRVP_SC_CLKRST6_ADDRESS, 0xf2, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3025, 0x02, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382c 07  ; hts global tx msb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382c, 0x07, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382d 10 ; hts global tx lsb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382d, 0x10, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3920 ff ; strobe pattern
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3920, 0xff, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3923 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3923, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3924 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3924, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3925 00 ; width frame span effective width
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3925, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3926, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3927, 0x01, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3928, 0x80, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3929, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392a, 0x14, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392b, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392c, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392d, 0x04, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392e, 0x00, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392f, 0xcb, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x38b3, 0x07, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3885, 0x07, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382b, 0x5a, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3670, 0x68, AR430_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#endif
#endif
   return retVal;
}

static ERRG_codeE AR430_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GPIO_DRVG_gpioSetValParamsT         gpioParams;
   GPIO_DRVG_gpioSetDirParamsT         gpioDir;

   if (gpioNum != 0xFFFF)
   {
       gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
       gpioDir.gpioNum = gpioNum;
       IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

       gpioParams.val = !state;
       gpioParams.gpioNum =gpioNum;
       retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
       gpioParams.val = state;
       gpioParams.gpioNum =gpioNum;
       retVal = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   }
    return (retVal);

}


/****************************************************************************
*
*  Function Name: AR430_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;
   return retVal;
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
   return retCode;
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   AR430_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   AR430_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: AR430_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
static ERRG_codeE AR430_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: AR430_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: AR430 sensor driver
*
****************************************************************************/
ERRG_codeE AR430_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(AR430_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {

         deviceDescriptorP = &AR430_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_AR_430_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      AR430_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &AR430_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &AR430_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &AR430_DRVP_open;
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

