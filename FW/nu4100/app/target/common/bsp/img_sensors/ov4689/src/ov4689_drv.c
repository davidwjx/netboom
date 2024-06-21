/****************************************************************************
 *
 *   FileName: OV4689_drv.c
 *
 *   Author:  Gyiora A.
 *
 *   Date:
 *
 *   Description: OV4689 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ov4689_drv.h"
#include "ov4689_cfg_tbl.h"
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
   OV_4689P_XSHUTDOWN_R_E,
   OV_4689P_XSHUTDOWN_L_E,
   OV_4689P_FSIN_CV_E,
   OV_4689P_XSHUTDOWN_W_E,
   OV_4689P_FSIN_CLR_E,
   OV_4689P_MAX_NUM_E
}OV_4689P_gpioNumEntryE;
int gpiosOv4689[OV_4689P_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(OV_4689P_gpioNumEntryE) if (gpiosOv4689[OV_4689P_gpioNumEntryE] != -1)


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE OV4689_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE OV4689_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE OV4689_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE OV4689_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE OV4689_DRVP_close(IO_HANDLE handle);
static ERRG_codeE OV4689_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32 OV4689_DRVP_findFreeSlot();
static ERRG_codeE OV4689_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_configGpio(OV_4689P_gpioNumEntryE gpioNumEntry,GPIO_DRVG_gpioStateE state);

static ERRG_codeE OV4689_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE OV4689_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE OV4689_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE OV4689_DRVP_powerup(IO_HANDLE handle, void *pParams);





/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define OV4689_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define OV4689_DRVP_CHANGE_ADDRESS_SLEEP_TIME                         (1*1000)   // 1ms
#define OV4689_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define OV4689_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define OV4689_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define OV4689_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define OV4689_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define OV4689_DRVP_16_BIT_ACCESS                                     (16)
#define OV4689_DRVP_8_BIT_ACCESS                                      (8)
#define OV4689_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)
//#define OV4689_DRVP_GROUP_MODE_EXP_CHANGE

///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define OV4689_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define OV4689_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)
#define OV4689_DRVP_SC_SCCB_ID1_ADDRESS                               (0x0109)
#define OV4689_DRVP_SC_ID1_ADDRESS                                    (0x3004) // SC_SCCB_ID1
#define OV4689_DRVP_SC_ID2_ADDRESS                                    (0x3012) // SC_SCCB_ID2
#define OV4689_DRVP_SC_CTRL_SID_ADDRESS                               (0x3037)
#define OV4689_DRVP_SC_CTRL_ID_ADDRESS                                (0x303f)


#define OV4689_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x300A)
#define OV4689_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x300B)
#define OV4689_DRVP_SC_REG5_ADDRESS                                   (0x3027) //


#define OV4689_DRVP_AEC_EXPO_1_ADDRESS                                (0x3500)
#define OV4689_DRVP_AEC_EXPO_2_ADDRESS                                (0x3501)
#define OV4689_DRVP_AEC_EXPO_3_ADDRESS                                (0x3502)

#define OV4689_DRVP_AEC_GAIN_1_ADDRESS                                (0x3509) //0-3 fraction  bits 4- 7 gain

#define OV4689_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS                      (0x3808)
#define OV4689_DRVP_TIMING_Y_OUTPUT_SIZE_ADDRESS                      (0x380A)
#define OV4689_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define OV4689_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define OV4689_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x380E)
#define OV4689_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x380F)

#define OV4689_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define OV4689_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define OV4689_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)

#define OV4689_DRVP_PLL1_PRE_DIV0_ADDRESS                             (0X030A)
#define OV4689_DRVP_PLL1_PRE_DIV_ADDRESS                              (0x0300)
#define OV4689_DRVP_PLL1_LOOP_DIV_H_ADDRESS                           (0x0301)
#define OV4689_DRVP_PLL1_LOOP_DIV_L_ADDRESS                           (0x0302)
#define OV4689_DRVP_PLL1_SYS_PRE_DIV_ADDRESS                          (0x0305)
#define OV4689_DRVP_PLL1_SYS_DIV_ADDRESS                              (0x0306)
#define OV4689_DRVP_PLL1_M_DIV_ADDRESS                                (0x0303)
#define OV4689_DRVP_PLL1_MIPI_DIV_ADDRESS                             (0x0304)
#define OV4689_DRVP_PLL2_PRE_DIV0_ADDRESS                             (0x0314)
#define OV4689_DRVP_PLL2_PRE_DIV_ADDRESS                              (0x030B)
#define OV4689_DRVP_PLL2_LOOP_DIV_H_ADDRESS                           (0x030C)
#define OV4689_DRVP_PLL2_LOOP_DIV_L_ADDRESS                           (0x030D)
#define OV4689_DRVP_PLL2_SYS_PRE_DIV_ADDRESS                          (0x030F)
#define OV4689_DRVP_PLL2_SYS_DIV_ADDRESS                              (0x030E)
#define OV4689_DRVP_PLL2_SYS_PRE_DIVP_ADDRESS                         (0x0311)
#define OV4689_DRVP_PLL2_SA1_DIV_ADDRESS                              (0x0312)
#define OV4689_DRVP_PLL2_DAC_DIV_ADDRESS                              (0x0313)


#define OV4689_DRVP_SC_CLKRST6_ADDRESS                                (0x301C)
#define OV4689_DRVP_GROUP_ACCESS_ADDRESS                              (0x3208)
#define OV4689_DRVP_GROUP0_PERIOD_ADDRESS                             (0x3209)
#define OV4689_DRVP_GROUP1_PERIOD_ADDRESS                             (0x320A)
#define OV4689_DRVP_GRP_SW_CTRL_ADDRESS                               (0x320B)

#define OV4689_DRVP_STROBE_EN_ADDRESS                                 (0x3006)
#define OV4689_DRVP_STROBE_CNTRL_ADDRESS                              (0x3B00)
#define OV4689_DRVP_STROBE_DURATION_ADDRESS                           (0x3B05)


#define OV4689_DRVP_ANA_CORE_6_ADDRESS                                (0x3666)

///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define OV4689_DRVP_SC_CHIP_ID                                        (0x4688)
#define OV4689_DRVP_I2C_DEFAULT_ADDRESS                               (0x6C)
#define OV4689_DRVP_I2C_MASTER_ADDRESS                                (OV4689_DRVP_I2C_DEFAULT_ADDRESS)
#define OV4689_DRVP_I2C_SLAVE_ADDRESS                                 (0x20)
#define OV4689_DRVP_I2C_GLOBAL_ADDRESS                                (0xC0)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  OV4689_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   OV4689_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        OV4689_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      OV4689_DRVP_resetSensors,            \
                                                                                                      OV4689_DRVP_initSensor,              \
                                                                                                      OV4689_DRVP_configSensors,           \
                                                                                                      OV4689_DRVP_startSensors,            \
                                                                                                      OV4689_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      OV4689_DRVP_getDeviceId,             \
                                                                                                      OV4689_DRVP_loadPresetTable,         \
                                                                                                      OV4689_DRVP_setFrameRate,            \
                                                                                                      OV4689_DRVP_setExposureTime,         \
                                                                                                      OV4689_DRVP_getExposureTime,         \
                                                                                                      OV4689_DRVP_setExposureMode,         \
                                                                                                      OV4689_DRVP_setImgOffsets,           \
                                                                                                      OV4689_DRVP_getAvgBrighness,         \
                                                                                                      OV4689_DRVP_setOutFormat,            \
                                                                                                      OV4689_DRVP_setPowerFreq,            \
                                                                                                      OV4689_DRVP_setStrobe,               \
                                                                                                      OV4689_DRVP_getSensorClks,           \
                                                                                                      OV4689_DRVP_setGain,                 \
                                                                                                      OV4689_DRVP_getGain,                 \
                                                                                                      OV4689_DRVP_trigger,                 \
                                                                                                      OV4689_DRVP_mirrorFlip,              \
                                                                                                      OV4689_DRVP_powerup ,                \
                                                                                                      OV4689_DRVP_powerdown ,              \
                                                                                                      OV4689_DRVP_changeSensorAddress,     \
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

static UINT16 OV4689_DRVP_lineLengthPclk;
static UINT16 OV4689_DRVP_x_output_size;
static UINT16 OV4689_DRVP_vts;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: OV4689_DRVP_getPixClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_getPixClk(IO_HANDLE handle)
{
  ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

  UINT16 pll1_prediv,pll1_predivp,pll1_mult,pll1_m_div, pll1_mipi_div, ref_clk;
  UINT32 pxl_clk;
  UINT8 predivVal[8]={2,3,4,5,6,8,12,16};
  UINT8 mipiDiv[4]   ={4,5,6,8};
  GEN_SENSOR_DRVG_sensorParametersT   config;
  GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->pixClk)
  {
     config.accessRegParams.data = 0;

     GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV4689_DRVP_PLL1_PRE_DIV0_ADDRESS , OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
     retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
     GEN_SENSOR_DRVG_ERROR_TEST(retVal);
     pxl_clk = ref_clk;
     pll1_predivp = (( config.accessRegParams.data ) & 0x1) + 1;

     GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_PLL1_PRE_DIV_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
     GEN_SENSOR_DRVG_ERROR_TEST(retVal);

     pll1_prediv = ( config.accessRegParams.data ) & 0x7;

     GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV4689_DRVP_PLL1_LOOP_DIV_H_ADDRESS , OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
     GEN_SENSOR_DRVG_ERROR_TEST(retVal);
     pll1_mult = config.accessRegParams.data&0x3ff;


     GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_PLL1_SYS_PRE_DIV_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
     GEN_SENSOR_DRVG_ERROR_TEST(retVal);

     pll1_m_div = ( config.accessRegParams.data ) & 0xF;

     GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_PLL1_SYS_DIV_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
     GEN_SENSOR_DRVG_ERROR_TEST(retVal);

     pll1_mipi_div = ( config.accessRegParams.data ) & 0x3;


     pxl_clk = pxl_clk*pll1_mult*pll1_predivp;
     pxl_clk = (pxl_clk*2) / predivVal[pll1_prediv];
     pxl_clk = pxl_clk/(pll1_m_div + 1);

     pxl_clk = (pxl_clk)/mipiDiv[pll1_mipi_div];
     pxl_clk = (pxl_clk * OV4689_DRVP_x_output_size) / (OV4689_DRVP_lineLengthPclk);
     deviceDescriptorP->pixClk = pxl_clk*2; // Doesn't work without mul2 need to remove
     LOGG_PRINT(LOG_INFO_E,NULL,"pll1_predivp = %d pll1_prediv = %d, pll1_m_div = %d, pll1_mipi_div = %d, pll1_mult %d pxl_clk = %d %d, ref_clk = %d xSize-%d line %d\n",
                                      pll1_predivp, pll1_prediv, pll1_m_div,pll1_mipi_div,pll1_mult,pxl_clk,deviceDescriptorP->pixClk,ref_clk,OV4689_DRVP_x_output_size,OV4689_DRVP_lineLengthPclk);
  }

  return deviceDescriptorP->pixClk;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_getSysClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static UINT16 OV4689_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   UINT16 pll2_prediv,pll2_predivp,pll2_mult,pll2_sys_pre_div, pll2_sys_div, sys_clk, ref_clk;
   UINT8 predivVal[8]={2,3,4,5,6,8,12,16};
   UINT8 sysDiv[8]   ={2,3,4,5,6,7,8,10};
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->sysClk)
   {
      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV4689_DRVP_PLL2_SYS_PRE_DIVP_ADDRESS , OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      sys_clk = ref_clk;
      pll2_predivp = (( config.accessRegParams.data ) & 0x1) + 1;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_PLL2_PRE_DIV_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OV4689_DRVP_PLL2_LOOP_DIV_H_ADDRESS , OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_mult = config.accessRegParams.data&0x3ff;


      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_PLL2_SYS_PRE_DIV_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_pre_div = ( config.accessRegParams.data ) & 0xF;

       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_PLL2_SYS_DIV_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_div = ( config.accessRegParams.data ) & 0x7;


      sys_clk = sys_clk*pll2_mult*pll2_predivp;
      sys_clk = (sys_clk*2) / predivVal[pll2_prediv];
      sys_clk = sys_clk/(pll2_sys_pre_div + 1);

      sys_clk = (sys_clk*2)/sysDiv[pll2_sys_div];
      deviceDescriptorP->sysClk = sys_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll2_predivp = %d pll2_prediv = %d, pll2_sys_pre_div = %d, pll2_sys_div = %d, pll2_mult %d sys_clk = %d, ref_clk = %d\n",
                                       pll2_predivp, pll2_prediv, pll2_sys_pre_div, pll2_sys_div,pll2_mult,sys_clk,ref_clk);
   }

    return deviceDescriptorP->sysClk;
}

static ERRG_codeE OV4689_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = OV4689_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = OV4689_DRVP_getPixClk(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = OV4689_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &OV4689_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     if ((pOpenParams->sensorSelect%2) == 0)
         deviceDescriptorP->sensorAddress   = OV4689_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
     else deviceDescriptorP->sensorAddress   = OV4689_DRVP_I2C_SLAVE_ADDRESS;
     deviceDescriptorP->ioctlFuncList   = OV4689_DRVP_ioctlFuncList;
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
*  Function Name: OV4689_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
***************************************************************************/
static ERRG_codeE OV4689_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn off the sensor
   retVal = OV4689_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
//   deviceDescriptorP->sensorAddress = CGS132_DRVP_I2C_MASTER_ADDRESS;
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_INFO_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x)\n",
           deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);
   //Turn on the sensor
   retVal = OV4689_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_SET_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(OV4689_DRVP_RESET_SLEEP_TIME);

   // reset sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OV4689_DRVP_SC_CHIP_ID_HIGH_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E,NULL,"CHIPID 0x%x\n",config.accessRegParams.data);


   if (config.accessRegParams.data != OV4689_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 shareAddr = *(UINT8*)pParams;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   if (shareAddr < 2)
   {
       LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, shareAddr = %d -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,shareAddr,handle);
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV4689_DRVP_SC_CTRL_SID_ADDRESS, shareAddr, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
   IO_HANDLE *handleP;
   GEN_SENSOR_DRVG_exposureTimeCfgT expParams;
   UINT16 regNum;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   FIX_UNUSED_PARAM_WARN(pParams);
   handleP = (IO_HANDLE*) deviceDescriptorP;

   LOGG_PRINT(LOG_INFO_E, NULL, "tableType: %d, res %d\n",deviceDescriptorP->tableType,params->loadTableParams.sensorResolution);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      sensorTablePtr = OV4689_CFG_TBLG_single_configTable;
      regNum = sizeof(OV4689_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = OV4689_CFG_TBLG_stereo_vga_configTable;
         regNum = sizeof(OV4689_CFG_TBLG_stereo_vga_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = OV4689_CFG_TBLG_stereo_vertical_binning_configTable;
         regNum = sizeof(OV4689_CFG_TBLG_stereo_vertical_binning_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {

         sensorTablePtr = OV4689_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(OV4689_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {
         GEN_SENSOR_DRVG_sensorParametersT   config;
         config.accessRegParams.data = 0;
         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_TIMING_HTS_LOW_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         OV4689_DRVP_lineLengthPclk = config.accessRegParams.data & 0xEFFF;

         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         OV4689_DRVP_x_output_size = config.accessRegParams.data & 0x1FFF;

         expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
         OV4689_DRVP_getExposureTime(handle,&expParams);
         OV4689_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
         OV4689_DRVP_enStrobe(deviceDescriptorP);

#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3017, 0xf2, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_SC_CLKRST6_ADDRESS, 0x04, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3200, 0x00, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3201, 0x04, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         // enable group mode
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GRP_SW_CTRL_ADDRESS, 0x5, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
      }
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Error loading present table\n");
   }

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_TIMING_VTS_LOW_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   OV4689_DRVP_vts = config.accessRegParams.data;

   return retVal;
}



/****************************************************************************
*
*  Function Name: OV4689_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   retVal = OV4689_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_setAutoExposureAvgLuma
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setAutoExposureAvgLuma(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}



/****************************************************************************
*
*  Function Name: OV4689_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16                              sysClk;
   (void)pParams;(void)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3002, 0x00, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3000, 0x0, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3824, 0x00, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3825, 0x20, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3826, OV4689_DRVP_vts -2, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3827, OV4689_DRVP_vts -2, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
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
*  Function Name: OV4689_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE OV4689_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OV4689_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE OV4689_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   LOGG_PRINT(LOG_INFO_E, NULL,"address %x params->triggerModeParams.isTriggerSupported %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OV4689_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

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
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              sysClk;
   UINT32                              frameLengthLine;

   sysClk = OV4689_DRVP_getSysClk(handle);
   if (ERRG_SUCCEEDED(retVal) && OV4689_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
        frameLengthLine = (1000000 * sysClk) / (OV4689_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);
#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
        GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
        UINT32 groupHold;
        //change to next group
        deviceDescriptorP->group = !deviceDescriptorP->group;

        // group hold start
        groupHold = 0x00 | deviceDescriptorP->group;
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

        frameLengthLine &=0xefff;
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, OV4689_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        OV4689_DRVP_vts = frameLengthLine;
        LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. OV4689_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,OV4689_DRVP_lineLengthPclk,frameLengthLine,sysClk);



#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
      //group hold end
      groupHold = 0x10 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // group hold launch
      groupHold = 0xA0 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif



   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
   UINT16                              sysClk = 0;
   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   if (OV4689_DRVP_lineLengthPclk != 0)
   {
      sysClk = OV4689_DRVP_getSysClk(handle);
      numExpLines = ((params->exposureTime * sysClk) + (OV4689_DRVP_lineLengthPclk - 1)) / OV4689_DRVP_lineLengthPclk; /* setting same exp to both sensor using stereo handle */
      numExpLines = numExpLines << 4;
      numExpLines = numExpLines & 0xFFFF0;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_AEC_EXPO_1_ADDRESS, numExpLines, OV4689_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      LOGG_PRINT(LOG_INFO_E,NULL,"sys_clk = %d, OV4689_DRVP_lineLengthPclk = %d, numExpLines = %d 0x%x, time = %d, address = 0x%x, context = %d\n",sysClk,OV4689_DRVP_lineLengthPclk,numExpLines,numExpLines,params->exposureTime,deviceDescriptorP->sensorAddress,params->context);
   }

#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              sysClk=0;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

   params->exposureTime = 0;

   if (ERRG_SUCCEEDED(retVal))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"get exposure (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_AEC_EXPO_1_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xfffff) >> 4);

      sysClk = OV4689_DRVP_getSysClk(handle);
      params->exposureTime = (numExpLines * OV4689_DRVP_lineLengthPclk) / sysClk;
      LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d, OV4689_DRVP_lineLengthPclk = %d, sys_clk = %d, context = %d\n",params->exposureTime,numExpLines,OV4689_DRVP_lineLengthPclk, sysClk,params->context);
   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
   if (params->analogGain > 0xFF)
   {
      params->analogGain = 0xFF;
   }

#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_AEC_GAIN_1_ADDRESS, params->analogGain , OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);


#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

   params->analogGain = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_AEC_GAIN_1_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   params->analogGain = config.accessRegParams.data & 0xFF;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
*  Function Name: OV4689_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/

static INT32 OV4689_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(OV4689_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(OV4689_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum, sysClk = 0;
   sysClk = OV4689_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (OV4689_DRVP_lineLengthPclk - 1)) / (OV4689_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

 //  GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OV4689_DRVP_STROBE_DURATION_ADDRESS  , (strobeUpTicksNum & 0xFFFF), OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
 //  GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#ifdef OV4689_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 strobeStartPoint;
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = OV4689_DRVP_vts - strobeUpTicksNum - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV4689_DRVP_STROBE_DURATION_ADDRESS, (strobeStartPoint & 0xFFFF), OV4689_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   LOGG_PRINT(LOG_DEBUG_E,NULL,"usec = %d, OV4689_DRVP_lineLengthPclk = %d, sysClk = %d, strobeUpTicksNum = 0x%x\n", usec, OV4689_DRVP_lineLengthPclk, sysClk, strobeUpTicksNum);
   return retVal;
}


/****************************************************************************
*
*  Function Name: OV4689_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;



#ifndef OV4689_DRVP_GROUP_MODE_EXP_CHANGE

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV4689_DRVP_STROBE_CNTRL_ADDRESS, 0x81, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);


#else

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OV4689_DRVP_STROBE_CNTRL_ADDRESS, 0x81, OV4689_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#endif

   return retVal;
}

static ERRG_codeE OV4689_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
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
*  Function Name: OV4689_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;
   return retVal;
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
   return retCode;
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   OV4689_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   OV4689_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: OV4689_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
static ERRG_codeE OV4689_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: OV4689_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OV4689 sensor driver
*
****************************************************************************/
ERRG_codeE OV4689_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(OV4689_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {

         deviceDescriptorP = &OV4689_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_OV_4689_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      OV4689_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &OV4689_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &OV4689_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &OV4689_DRVP_open;
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

