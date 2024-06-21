/****************************************************************************
 *
 *   FileName: OV8856_drv.c
 *
 *   Author:  Giyora A.
 *
 *   Date:
 *
 *   Description: OV8856 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ov8856_drv.h"
#include "ov8856_cfg_tbl.h"
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
   OV_8856P_XSHUTDOWN_R_E,
   OV_8856P_XSHUTDOWN_L_E,
   OV_8856P_FSIN_CV_E,
   OV_8856P_XSHUTDOWN_W_E,
   OV_8856P_FSIN_CLR_E,
   OV_8856P_MAX_NUM_E
}OV_8856P_gpioNumEntryE;
int gpiosOv8856[OV_8856P_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(OV_8856P_gpioNumEntryE) if (gpiosOv8856[OV_8856P_gpioNumEntryE] != -1)


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE OV8856_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE OV8856_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE OV8856_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE OV8856_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE OV8856_DRVP_close(IO_HANDLE handle);
static ERRG_codeE OV8856_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      OV8856_DRVP_findFreeSlot();
static ERRG_codeE OV8856_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_configGpio(OV_8856P_gpioNumEntryE gpioNumEntry,GPIO_DRVG_gpioStateE state);

static ERRG_codeE OV8856_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE OV8856_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE OV8856_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV8856_DRVP_getIspInitParams(IO_HANDLE handle, void *pParams);


/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define OV8856_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define OV8856_DRVP_CHANGE_ADDRESS_SLEEP_TIME                         (1*1000)   // 1ms
#define OV8856_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define OV8856_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define OV8856_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define OV8856_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define OV8856_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define OV8856_DRVP_16_BIT_ACCESS                                     (16)
#define OV8856_DRVP_8_BIT_ACCESS                                      (8)
#define OV8856_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)


///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define OV8856_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define OV8856_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)

#define OV8856_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x300A)
#define OV8856_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x300B)

#define OV8856_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define OV8856_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define OV8856_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)

#define OV8856_DRVP_AEC_GAIN_1_ADDRESS                                (0x3508) //0-3 fraction  bits 4- 7 gain

#define OV8856_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS                      (0x3808)
#define OV8856_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define OV8856_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define OV8856_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define OV8856_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define OV8856_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define OV8856_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define OV8856_DRVP_PLL1_PRE_DIV0_ADDRESS                             (0X030A)
#define OV8856_DRVP_PLL1_PRE_DIV_ADDRESS                              (0x0300)
#define OV8856_DRVP_PLL1_MULT_H_ADDRESS                               (0x0301)
#define OV8856_DRVP_PLL1_MULT_L_ADDRESS                               (0x0302)
#define OV8856_DRVP_PLL1_DIVPX                                        (0x0303)
#define OV8856_DRVP_PLL1_PIX_BITDIV_ADDRESS                           (0x0304)
#define OV8856_DRVP_PLL1_SYS_BITDIV                                   (0x0305)
#define OV8856_DRVP_PLL1_DIV_S                                        (0x0306)

#define OV8856_DRVP_PLL1_PLL1_BYPASS                                  (0x0308)
#define OV8856_DRVP_PLL1_CP                                           (0x0309)

#define OV8856_DRVP_PLL1_PRE_DIV_P_ADDRESS                            (0x030A)
#define OV8856_DRVP_PLL2_PRE_DIV_ADDRESS                              (0x030B)
#define OV8856_DRVP_PLL2_LOOP_DIV_H_ADDRESS                           (0x030C)
#define OV8856_DRVP_PLL2_LOOP_DIV_L_ADDRESS                           (0x030D)
#define OV8856_DRVP_PLL2_SYS_PRE_DIV_ADDRESS                          (0x030F)
#define OV8856_DRVP_PLL2_SYS_DIV_ADDRESS                              (0x030E)
#define OV8856_DRVP_PLL2_SA1_DIV_ADDRESS                              (0x0312)
#define OV8856_DRVP_PLL2_DAC_DIV_ADDRESS                              (0x0313)
#define OV8856_DRVP_PLL2_PRE_DIV0_ADDRESS                             (0x0314)
#define OV8856_DRVP_PLL2_SELECT_ADDRESS                               (0x3032)
#define OV8856_DRVP_PLL2_SELECT_ADDRESS                               (0x3032)
#define OV8856_DRVP_DAC_SELECT_ADDRESS                                (0x3033)

#define OV8856_DRVP_PLL_PCLK_DIV_ADDRESS                              (0x3020)
#define OV8856_DRVP_PLL_SYSCLK_DIV_ADDRESS                            (0x3106)

#define OV8856_DRVP_SC_CLKRST6_ADDRESS                                (0x301C)
#define OV8856_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define OV8856_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define OV8856_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define OV8856_DRVP_GRP_SW_CTRL_ADDRESS                               (0x320B)

#define OV8856_DRVP_STROBE_MASK_ADDRESS                               (0x3920)
#define OV8856_DRVP_PWM_CTRL_2F_ADDRESS                               (0x392F)  // STROBE_POLARITY in OV8856 0x3B96
#define OV8856_DRVP_PWM_CTRL_24_ADDRESS                               (0x3924)  // STROBE_OFFSET_ADDRESS 0x3B8B
#define OV8856_DRVP_PWM_CTRL_27_ADDRESS                               (0x3927)  // STROBE_DURATION_HIGH 0x3B8E
#define OV8856_DRVP_PWM_CTRL_28_ADDRESS                               (0x3928)  // STROBE_DURATION_LOW 0x3B8F
#define OV8856_DRVP_PWM_CTRL_29_ADDRESS                               (0x3929)

#define OV8856_DRVP_SCCB_ID_ADDRESS                                   (0x3004)
#define OV8856_DRVP_SCCB_ID2_ADDRESS                                  (0x3006)

#define OV8856_DRVP_ANA_CORE_6_ADDRESS                                (0x3666)

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define OV8856_DRVP_SC_CHIP_ID                                        (0x8856)
#define OV8856_DRVP_SC_CHIP_ID_1                                      (0x885A)
#define OV8856_DRVP_I2C_DEFAULT_ADDRESS                               (0x6c)
#define OV8856_DRVP_I2C_MASTER_ADDRESS                                (OV8856_DRVP_I2C_DEFAULT_ADDRESS)
#define OV8856_DRVP_I2C_SLAVE_ADDRESS                                 (0x6c)
#define OV8856_DRVP_I2C_GLOBAL_ADDRESS                                (0x42)
#define OV8856_DRVP_I2C_UNIQUE_ADDRESS                                (0x80)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////
#define Camera_1V8_EN (30)//DOVDD
#define Camera_1V2_EN (54)//DVDD
#define Camera_VAA_2V8_EN (29)//AVDD
#define Camera_VAA_2V8_RGB_EN (31)//RGB AVDD

/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  OV8856_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   OV8856_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        OV8856_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      OV8856_DRVP_resetSensors,            \
                                                                                                      OV8856_DRVP_initSensor,              \
                                                                                                      OV8856_DRVP_configSensors,           \
                                                                                                      OV8856_DRVP_startSensors,            \
                                                                                                      OV8856_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      OV8856_DRVP_getDeviceId,             \
                                                                                                      OV8856_DRVP_loadPresetTable,         \
                                                                                                      OV8856_DRVP_setFrameRate,            \
                                                                                                      OV8856_DRVP_setExposureTime,         \
                                                                                                      OV8856_DRVP_getExposureTime,         \
                                                                                                      OV8856_DRVP_setExposureMode,         \
                                                                                                      OV8856_DRVP_setImgOffsets,           \
                                                                                                      OV8856_DRVP_getAvgBrighness,         \
                                                                                                      OV8856_DRVP_setOutFormat,            \
                                                                                                      OV8856_DRVP_setPowerFreq,            \
                                                                                                      OV8856_DRVP_setStrobe,               \
                                                                                                      OV8856_DRVP_getSensorClks,           \
                                                                                                      OV8856_DRVP_setGain,                 \
                                                                                                      OV8856_DRVP_getGain,                 \
                                                                                                      OV8856_DRVP_trigger,                 \
                                                                                                      OV8856_DRVP_mirrorFlip,              \
                                                                                                      OV8856_DRVP_powerup ,                \
                                                                                                      OV8856_DRVP_powerdown ,              \
                                                                                                      OV8856_DRVP_changeSensorAddress,     \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      OV8856_DRVP_getIspInitParams,        \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub,                \
                                                                                                      GEN_SENSOR_DRVG_stub};

static UINT16 OV8856_DRVP_lineLengthPclk;
static UINT16 OV8856_DRVP_x_output_size;
static UINT16 OV8856_DRVP_vts;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static UINT16 OV8856_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;



  if (!deviceDescriptorP->pixClk)
   {
      UINT16 pll1_predivP,pll1_prediv,pll1_mult,pll1_mipi_div,pix_bit_div,pix_pclk_div, pix_clk, ref_clk;
      UINT8  predivVal[8] = {2,3,4,5,6,8,12,16};
      UINT8  pixBitDivVal[4]   ={4,5,6,8};

      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pix_clk = ref_clk;


      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_PRE_DIV_P_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_predivP = ( config.accessRegParams.data ) & 0x1;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_PRE_DIV_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_MULT_H_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mult = (( config.accessRegParams.data ) & 0x3) << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_MULT_L_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mult |= ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_DIVPX, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mipi_div = ( config.accessRegParams.data ) & 0x0F;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL1_PIX_BITDIV_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pix_bit_div = ( config.accessRegParams.data ) & 0x03;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL_PCLK_DIV_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pix_pclk_div = (( config.accessRegParams.data ) & 0x8) >> 3;


      pix_clk = pix_clk / (pll1_predivP + 1);
      pix_clk = (pix_clk*2) / predivVal[pll1_prediv];
      pix_clk = pix_clk * pll1_mult;
      pix_clk = pix_clk / (pll1_mipi_div + 1);
      pix_clk = pix_clk / pixBitDivVal[pix_bit_div];
      pix_clk = pix_clk / (pix_pclk_div + 1);

      pix_clk = (pix_clk * OV8856_DRVP_x_output_size) / (OV8856_DRVP_lineLengthPclk);

      deviceDescriptorP->pixClk = pix_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll1_predivP = %d, pll1_prediv = %d, pll1_mult = %d, pll1_mipi_div %d pix_bit_div %d pix_pclk_div %d pixClk = %d, ref_clk = %d\n",
                                  pll1_predivP, pll1_prediv, pll1_mult,pll1_mipi_div,pix_bit_div,pix_pclk_div, deviceDescriptorP->pixClk,ref_clk);
   }
   return deviceDescriptorP->pixClk;

}

static UINT16 OV8856_DRVP_getSysClkPll1(IO_HANDLE handle)
{

    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;


    if (!deviceDescriptorP->sysClk)
    {
       UINT16 pll1_predivP,pll1_prediv,pll1_mult,sys_pre_div,sys_pll1_div_s,sys_prediv,sys_pclk_div, sys_clk, ref_clk;
       UINT8  predivVal[8] = {2,3,4,5,6,8,12,16};
       UINT8  sysPreDivVal[4]   ={1,2,4,1};

       retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_clk = ref_clk;


       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_PRE_DIV_P_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll1_predivP = ( config.accessRegParams.data ) & 0x1;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_PRE_DIV_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll1_prediv = ( config.accessRegParams.data ) & 0x7;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_MULT_H_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll1_mult = (( config.accessRegParams.data ) & 0x3) << 8;
       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL1_MULT_L_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll1_mult |= ( config.accessRegParams.data ) & 0xFF;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL1_SYS_BITDIV , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_pre_div = ( config.accessRegParams.data ) & 0x03;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL1_DIV_S , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_pll1_div_s = ( config.accessRegParams.data ) & 0x01;


       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL_SYSCLK_DIV_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_prediv = (( config.accessRegParams.data ) & 0x0c) >> 2;
       sys_pclk_div = (( config.accessRegParams.data ) & 0xf0) >> 4;
       if (sys_pclk_div == 0)
           sys_pclk_div = 1;


       sys_clk = sys_clk / (pll1_predivP + 1);
       sys_clk = (sys_clk*2) / predivVal[pll1_prediv];
       sys_clk = sys_clk * pll1_mult;
       sys_clk = sys_clk / (3 + sys_pre_div);
       sys_clk = sys_clk / (sys_pll1_div_s + 1);
       sys_clk = sys_clk / sysPreDivVal[sys_prediv];
       sys_clk = sys_clk/ sys_pclk_div;


       deviceDescriptorP->sysClk = sys_clk;
       LOGG_PRINT(LOG_INFO_E,NULL,"pll1_predivP = %d, pll1_prediv = %d, pll1_mult = %d, sys_pre_div = %d sys_pll1_div_s %d sys_prediv %d sys_pclk_div %d sysClk = %d, ref_clk = %d\n",
                                   pll1_predivP, pll1_prediv, pll1_mult,sys_pre_div,sys_pll1_div_s,sys_prediv,sys_pclk_div, deviceDescriptorP->sysClk,ref_clk);
    }

    return deviceDescriptorP->sysClk;
}

static UINT16 OV8856_DRVP_getSysClkPll2(IO_HANDLE handle,UINT8 dacSelect)
{

    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;



    if (!deviceDescriptorP->sysClk)
    {
       UINT16 pll2_sa1_div,pll2_predivP,pll2_prediv,pll2_mult,dac_enbl,dac_dividor,sys_pre_div,sys_pll2_div,sys_prediv,sys_pclk_div, sys_clk, ref_clk;
       UINT8  predivVal[8] = {2,3,4,5,6,8,12,16};
       UINT8  pll2DivVal[8] = {2,3,4,5,6,7,8,10};
       UINT8  sysPreDivVal[4]   ={1,2,4,1};

       retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_clk = ref_clk;


       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL2_SA1_DIV_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll2_predivP = (( config.accessRegParams.data ) & 0x10)>>4;

       dac_dividor = (( config.accessRegParams.data ) & 0x0F);

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL2_PRE_DIV_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll2_prediv = ( config.accessRegParams.data ) & 0x7;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL2_LOOP_DIV_H_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll2_mult = (( config.accessRegParams.data ) & 0x3) << 8;
       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL2_LOOP_DIV_L_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll2_mult |= ( config.accessRegParams.data ) & 0xFF;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL2_SYS_PRE_DIV_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_pre_div = ( config.accessRegParams.data ) & 0x0F;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL2_SYS_DIV_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_pll2_div = ( config.accessRegParams.data ) & 0x07;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_PLL_SYSCLK_DIV_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       sys_prediv = (( config.accessRegParams.data ) & 0x0c) >> 2;
       sys_pclk_div = (( config.accessRegParams.data ) & 0xf0) >> 4;
       if (sys_pclk_div == 0)
           sys_pclk_div = 1;

       sys_clk = sys_clk / (pll2_predivP + 1);
       sys_clk = (sys_clk*2) / predivVal[pll2_prediv];
       sys_clk = sys_clk * pll2_mult;
       if (dacSelect)
       {
           sys_clk = sys_clk/(dac_dividor + 1);
       }
       else
       {
           sys_clk = sys_clk / (1 + sys_pre_div);
           sys_clk = (sys_clk *2)/ (pll2DivVal[sys_pll2_div]);
       }
       sys_clk = sys_clk / sysPreDivVal[sys_prediv];
       sys_clk = sys_clk/ sys_pclk_div;


       deviceDescriptorP->sysClk = sys_clk;
       LOGG_PRINT(LOG_INFO_E,NULL,"pll2_predivP = %d, pll2_prediv = %d, pll2_mult = %d, sys_pre_div %d sys_pll2_div %d sys_prediv %d sys_pclk_div %d sysClk = %d, ref_clk = %d\n",
                                   pll2_predivP, pll2_prediv, pll2_mult,sys_pre_div,sys_pll2_div,sys_prediv,sys_pclk_div, deviceDescriptorP->sysClk,ref_clk);
    }
}



static UINT16 OV8856_DRVP_getSysClk(IO_HANDLE handle)
{

    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT8 pll2Select,dacSelect;


    if (!deviceDescriptorP->sysClk)
    {
       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_PLL2_SELECT_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       pll2Select = (( config.accessRegParams.data ) & 0x80)>>7;
       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV8856_DRVP_DAC_SELECT_ADDRESS , OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
       dacSelect = (( config.accessRegParams.data ) & 0x2)>>1;

       if (pll2Select || dacSelect)
       {
           OV8856_DRVP_getSysClkPll2(handle,dacSelect);
       }
       else OV8856_DRVP_getSysClkPll1(handle);

    }
    return deviceDescriptorP->sysClk;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = OV8856_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = OV8856_DRVP_getPixClk(handle);

   return retVal;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_getIspInitParams
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_getIspInitParams(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   params->ispInitParams.hts                = 1932;
   params->ispInitParams.sclk               = 144000000;

   params->ispInitParams.frmLengthLines     = 2448;

   params->ispInitParams.minIntegrationLine = 4;
   params->ispInitParams.maxIntegrationLine = 0;	// will be calculated in ISP side using current fps;
   params->ispInitParams.minGain            = 1.0;
   params->ispInitParams.maxGain            = 16.0;
   params->ispInitParams.minFps             = 3;
   params->ispInitParams.maxFps             = 30;
   params->ispInitParams.bayerFormat        = GEN_SENSOR_DRVG_BAYER_BGGR;
   params->ispInitParams.aecMaxIntegrationTime  = 0;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = OV8856_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &OV8856_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     deviceDescriptorP->sensorAddress   = OV8856_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
     deviceDescriptorP->ioctlFuncList   = OV8856_DRVP_ioctlFuncList;
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

     //we change address 6c->80, config under this address. in start sensor func' we force 0x42 to start 0v 5675 & ov8856 together
     /*if(pOpenParams->sensorType == INU_DEFSG_SENSOR_TYPE_STEREO_E)
     {
         if (pOpenParams->tableType == INU_DEFSG_MONO_E)
         {
            deviceDescriptorP->sensorAddress = OV8856_DRVP_I2C_GLOBAL_ADDRESS;
         }
         else
         {
            deviceDescriptorP->sensorAddress = OV8856_DRVP_I2C_GLOBAL_ADDRESS;
         }
     }*/
     LOGG_PRINT(LOG_INFO_E,NULL,"open sensor (type = %d, address = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);
	 printf("%s::open sensor (type = %d, address = 0x%x)\n", __func__, deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress);
   }
   else
   {
      retCode = SENSOR__ERR_OPEN_DEVICE_FAIL;
   }

   return(retCode);
}




/****************************************************************************
*
*  Function Name: OV8856_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
***************************************************************************/
static ERRG_codeE OV8856_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn off the sensor
   retVal = OV8856_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
   deviceDescriptorP->sensorAddress = OV8856_DRVP_I2C_MASTER_ADDRESS;
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn on the sensor
   retVal = OV8856_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_SET_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(OV8856_DRVP_RESET_SLEEP_TIME);

   // reset sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OV8856_DRVP_SC_CHIP_ID_HIGH_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   if (config.accessRegParams.data != OV8856_DRVP_SC_CHIP_ID_1)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   UINT8 address = OV8856_DRVP_I2C_UNIQUE_ADDRESS;//force uniqe address. the sensor is single at his sensor group, so we force uniqe address and not B0 B2 etc..

   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x303f, 1, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_SCCB_ID_ADDRESS, address, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   deviceDescriptorP->sensorAddress = address;

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_SCCB_ID_ADDRESS, address, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(OV8856_DRVP_CHANGE_ADDRESS_SLEEP_TIME);

   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OV8856_DRVP_SCCB_ID_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   if (config.accessRegParams.data != address)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Error changing address! (%x) (requested %x)\n",config.accessRegParams.data,address);
   }
   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor END\n");

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
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
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_1632x1224configTable;
         regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_1632x1224configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_configTable_1920_1080;
         regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_configTable_1920_1080)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {
          INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
          if (modelType == INU_DEFSG_BOOT84_E)//in calibrtaion borad, we have to config sensor in low mipi speed
          {
             sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_configTable_480M;
             regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_configTable_480M)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
          }
          else
          {
             sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_configTable;
             regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
          }
      }
   }
   else
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_1632x1224configTable;
         regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_1632x1224configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_configTable_1920_1080;
         regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_configTable_1920_1080)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {
         sensorTablePtr = OV8856_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(OV8856_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }

   }
 //  sensorTablePtr = OV8856_CFG_TBLG_stereo_vertical_binning_configTable;
//   regNum = sizeof(OV8856_CFG_TBLG_stereo_vertical_binning_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {
         GEN_SENSOR_DRVG_sensorParametersT   config;
         config.accessRegParams.data = 0;
         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_TIMING_HTS_LOW_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         OV8856_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;

         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         OV8856_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

         expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
         OV8856_DRVP_getExposureTime(handle,&expParams);
         OV8856_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
         OV8856_DRVP_enStrobe(deviceDescriptorP);

#ifdef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3017, 0xf2, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_SC_CLKRST6_ADDRESS, 0x04, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3200, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3201, 0x04, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         // enable group mode
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GRP_SW_CTRL_ADDRESS, 0x5, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Function Name: OV8856_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   retVal = OV8856_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16                              sysClk;
   (void)pParams;(void)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;


   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
      params->triggerModeParams.isTriggerSupported = 1;
      LOGG_PRINT(LOG_WARN_E,NULL,"sensor set to Trigger Mode\n");
   }
   else
   {
	  params->triggerModeParams.isTriggerSupported = 0;
   }
   //in calibration mode (boot10084) we have to config free running mode, at the other boots, for sync between ov5675&ov8856, we have to config
   //ov5675 to slave and how to respond to triger (fsin) that will come from ov 8856. ov 8856 we have to config to master
   if (deviceDescriptorP->tableType == INU_DEFSG_STEREO_E)
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"sensor set to trigger as master\n");
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3000, 0x20, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);//MASTER VSYNC
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x300e, 0x20, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);//MASTER VSYNC
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3819, 0x10, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);//MASTER VSYNC
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"sensor set to free running\n");
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE OV8856_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OV8856_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE OV8856_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!params->triggerModeParams.isTriggerSupported)
   {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV8856_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              sysClk;
   UINT16                              frameLengthLine;
   sysClk = OV8856_DRVP_getSysClk(handle);
   if (ERRG_SUCCEEDED(retVal) && OV8856_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
       frameLengthLine = ((1000000 * sysClk) + ((OV8856_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate) - 1)) / (OV8856_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

#ifdef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
        GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
        UINT32 groupHold;
        //change to next group
        deviceDescriptorP->group = !deviceDescriptorP->group;

        // group hold start
        groupHold = 0x00 | deviceDescriptorP->group;
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, OV8856_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        OV8856_DRVP_vts = frameLengthLine;
        LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. OV8856_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,OV8856_DRVP_lineLengthPclk,frameLengthLine,sysClk);

#ifdef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
      //group hold end
      groupHold = 0x10 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // group hold launch
      groupHold = 0xA0 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines, exp, maxExp;
   float                               expFloat;
   UINT16                              vts, sysClk = 0;
   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

   //read vts to calc max exp
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_TIMING_VTS_LOW_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   vts = config.accessRegParams.data;

#ifndef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   //deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 ;//| deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   if (OV8856_DRVP_lineLengthPclk != 0)
   {
      sysClk = OV8856_DRVP_getSysClk(handle);
      maxExp = (vts - 4);

      expFloat = ((float)params->exposureTime / (float)1000 * (float)sysClk * (float)1000 / (float)OV8856_DRVP_lineLengthPclk);

      if (expFloat > maxExp)
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"exposure reach to max exposure\n");
         exp = maxExp;
      }
      exp = ((UINT32)((float)expFloat + 0.5)) << 4;

      //LOGG_PRINT(LOG_INFO_E,NULL,"exp_req=%d exp_calc=0x%x hts=0x%x vts=0x%x maxExp=0x%x sysClk=%d\n",params->exposureTime, exp, OV8856_DRVP_lineLengthPclk, vts,maxExp,sysClk);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_AEC_EXPO_1_ADDRESS, exp, OV8856_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    }

#ifndef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 ;//| deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xE0 ;//| deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              sysClk;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
   params->exposureTime = 0;

   if (ERRG_SUCCEEDED(retVal))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"get exposure (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_AEC_EXPO_1_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xF) << 16);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_AEC_EXPO_2_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xFF) << 8);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_AEC_EXPO_3_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xFF) );

      sysClk = OV8856_DRVP_getSysClk(handle);
      numExpLines = numExpLines>> 4;
      params->exposureTime = (( OV8856_DRVP_lineLengthPclk * numExpLines) - OV8856_DRVP_lineLengthPclk) / sysClk;
      LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d, OV8856_DRVP_lineLengthPclk = %d, sys_clk = %d, context = %d\n",params->exposureTime,numExpLines,OV8856_DRVP_lineLengthPclk, sysClk,params->context);
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32 gainVal;

   if (params->analogGain >= 16.0)
   {
      gainVal = (UINT32)(15.94 * 128.0);
   }
   else
   {
      gainVal = ((UINT32)(params->analogGain * 16.0)) << 3;
   }

#ifndef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   //deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x01;// | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   //LOGG_PRINT(LOG_INFO_E, NULL, "analogGain %f calc gain 0x%x\n", params->analogGain, gainVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_AEC_GAIN_1_ADDRESS, gainVal , OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#ifndef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x11;// | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xE1;// | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   //LOGG_PRINT(LOG_INFO_E,NULL,"analogGain %f calc gain 0x%x\n",params->analogGain,gainVal);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
   UINT32 analogGain;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_AEC_GAIN_1_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   analogGain = config.accessRegParams.data & 0x1FFF;
   analogGain = analogGain >> 3;
   params->analogGain = (float)analogGain / 16.0; //change analogGain to be float in case of accurecy of 0.5

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
*  Function Name: OV8856_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static INT32 OV8856_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(OV8856_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(OV8856_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV8856_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   FIX_UNUSED_PARAM_WARN(deviceDescriptorP);
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(usec);
#ifdef GIY
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum, sysClk = 0;
   sysClk = OV8856_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (OV8856_DRVP_lineLengthPclk - 1)) / (OV8856_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_PWM_CTRL_27_ADDRESS, (strobeUpTicksNum & 0xFFFF), OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#ifdef OV8856_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 strobeStartPoint;
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = OV8856_DRVP_vts - strobeUpTicksNum - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_PWM_CTRL_29_ADDRESS, (strobeStartPoint & 0xFFFF), OV8856_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   LOGG_PRINT(LOG_DEBUG_E,NULL,"usec = %d, OV8856_DRVP_lineLengthPclk = %d, sysClk = %d, strobeUpTicksNum = 0x%x\n", usec, OV8856_DRVP_lineLengthPclk, sysClk, strobeUpTicksNum);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV8856_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(handle);
#ifdef GIY
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
#ifndef OV8856_DRVP_GROUP_MODE_EXP_CHANGE

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_PWM_CTRL_2F_ADDRESS, 0x40, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_PWM_CTRL_24_ADDRESS, 0x0, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_STROBE_MASK_ADDRESS, 0xFF, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_STROBE_EN_ADDRESS, 0x08, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#else
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3006, 0x0c, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3210, 0x04, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3007, 0x02, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV8856_DRVP_SC_CLKRST6_ADDRESS, 0xf2, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3025, 0x02, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382c 07  ; hts global tx msb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382c, 0x07, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382d 10 ; hts global tx lsb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382d, 0x10, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3920 ff ; strobe pattern
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3920, 0xff, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3923 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3923, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3924 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3924, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3925 00 ; width frame span effective width
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3925, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3926, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3927, 0x01, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3928, 0x80, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3929, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392a, 0x14, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392b, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392c, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392d, 0x04, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392e, 0x00, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392f, 0xcb, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x38b3, 0x07, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3885, 0x07, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382b, 0x5a, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3670, 0x68, OV8856_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#endif
#endif
   return retVal;
}

static ERRG_codeE OV8856_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
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
*  Function Name: OV8856_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;
   return retVal;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
   return retCode;
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   OV8856_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   OV8856_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: OV8856_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
static ERRG_codeE OV8856_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: OV8856_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV8856 sensor driver
*
****************************************************************************/
ERRG_codeE OV8856_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(OV8856_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {

         deviceDescriptorP = &OV8856_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_OV_8856_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      OV8856_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &OV8856_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &OV8856_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &OV8856_DRVP_open;
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

