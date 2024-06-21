/****************************************************************************
 *
 *   FileName: CGS132_drv.c
 *
 *   Author:  Giyora A.
 *
 *   Date:
 *
 *   Description: CGS132 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "cgs132_drv.h"
#include "cgs132_cfg_tbl.h"
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
   CGS_132_XSHUTDOWN_R_E,
   CGS_132_XSHUTDOWN_L_E,
   CGS_132_FSIN_CV_E,
   CGS_132_XSHUTDOWN_W_E,
   CGS_132_FSIN_CLR_E,
   CGS_132_MAX_NUM_E
}CGS_132_gpioNumEntryE;
int gpiosCgs132[CGS_132_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(CGS_132_gpioNumEntryE) if (gpiosCgs132[CGS_132_gpioNumEntryE] != -1)


/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE CGS132_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE CGS132_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE CGS132_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE CGS132_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE CGS132_DRVP_close(IO_HANDLE handle);
static ERRG_codeE CGS132_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      CGS132_DRVP_findFreeSlot();
static ERRG_codeE CGS132_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_configGpio(CGS_132_gpioNumEntryE gpioNumEntry,GPIO_DRVG_gpioStateE state);

static ERRG_codeE CGS132_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE CGS132_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE CGS132_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE CGS132_DRVP_powerup(IO_HANDLE handle, void *pParams);





/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define CGS132_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define CGS132_DRVP_CHANGE_ADDRESS_SLEEP_TIME                         (1*1000)   // 1ms
#define CGS132_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define CGS132_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define CGS132_DRVP_SENSOR_ACCESS_3_BYTE                              (3)
#define CGS132_DRVP_SENSOR_ACCESS_4_BYTE                              (4)
#define CGS132_DRVP_REGISTER_ACCESS_SIZE                              (2)
#define CGS132_DRVP_16_BIT_ACCESS                                     (16)
#define CGS132_DRVP_8_BIT_ACCESS                                      (8)
#define CGS132_DRVP_MAX_NUMBER_SENSOR_INSTANCE                        (6)
//#define CGS132_DRVP_GROUP_MODE_EXP_CHANGE

///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define CGS132_DRVP_SC_MODE_SELECT_ADDRESS                            (0x0100)
#define CGS132_DRVP_SC_SOFTWARE_RESET_ADDRESS                         (0x0103)
#define CGS132_DRVP_SC_SCCB_ID1_ADDRESS                               (0x0109)
#define CGS132_DRVP_SC_CTRL_2B_ADDRESS                                (0x302B) // SC_SCCB_ID2

#define CGS132_DRVP_LINE_LENGTH_HIGH_ADDRESS                          (0x320C)
#define CGS132_BLANK_ROW_HIGH                                         (0x3228)

#define CGS132_DRVP_SC_CHIP_ID_HIGH_ADDRESS                           (0x3107)
#define CGS132_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x3108)
#define CGS132_DRVP_SC_REG5_ADDRESS                                   (0x3027) //


#define CGS132_DRVP_AEC_EXPO_1_ADDRESS                                (0x3e00)
#define CGS132_DRVP_AEC_EXPO_2_ADDRESS                                (0x3e01)
#define CGS132_DRVP_AEC_EXPO_3_ADDRESS                                (0x3e02)
//#define CGS132_DRVP_GAIN_METHOD_ADDRESS                               (0x3e03)
#define CGS132_DRVP_DIGL_GAIN_1_ADDRESS                               (0x3e06) //0-3 fraction  bits 4- 7 gain
#define CGS132_DRVP_ANALOG_GAIN_1_ADDRESS                             (0x3e08) //0-3 fraction  bits 4- 7 gain

#define CGS132_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS                      (0x3808)
#define CGS132_DRVP_TIMING_HTS_LOW_ADDRESS                            (0x380C)
#define CGS132_DRVP_TIMING_HTS_HIGH_ADDRESS                           (0x380D)
#define CGS132_DRVP_TIMING_VTS_LOW_ADDRESS                            (0x320E)
#define CGS132_DRVP_TIMING_VTS_HIGH_ADDRESS                           (0x320F)

#define CGS132_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS                (0x3820)
#define CGS132_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS              (0x3821)

#define CGS132_DRVP_TIMING_REG_2B_ADDRESS                             (0x382B)





#define CGS132_DRVP_STROBE_EN_ADDRESS                                 (0x3361)
#define CGS132_DRVP_STROBE_MASK_ADDRESS                               (0x3920)




///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define CGS132_DRVP_SC_CHIP_ID                                        (0x132)
#define CGS132_DRVP_I2C_DEFAULT_ADDRESS                               (0x60)
#define CGS132_DRVP_I2C_MASTER_ADDRESS                                (CGS132_DRVP_I2C_DEFAULT_ADDRESS)
#define CGS132_DRVP_I2C_SLAVE_ADDRESS                                 (0x60)
#define CGS132_DRVP_I2C_GLOBAL_ADDRESS                                (0x60)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  CGS132_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   CGS132_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        CGS132_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      CGS132_DRVP_resetSensors,            \
                                                                                                      CGS132_DRVP_initSensor,              \
                                                                                                      CGS132_DRVP_configSensors,           \
                                                                                                      CGS132_DRVP_startSensors,            \
                                                                                                      CGS132_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      CGS132_DRVP_getDeviceId,             \
                                                                                                      CGS132_DRVP_loadPresetTable,         \
                                                                                                      CGS132_DRVP_setFrameRate,            \
                                                                                                      CGS132_DRVP_setExposureTime,         \
                                                                                                      CGS132_DRVP_getExposureTime,         \
                                                                                                      CGS132_DRVP_setExposureMode,         \
                                                                                                      CGS132_DRVP_setImgOffsets,           \
                                                                                                      CGS132_DRVP_getAvgBrighness,         \
                                                                                                      CGS132_DRVP_setOutFormat,            \
                                                                                                      CGS132_DRVP_setPowerFreq,            \
                                                                                                      CGS132_DRVP_setStrobe,               \
                                                                                                      CGS132_DRVP_getSensorClks,           \
                                                                                                      CGS132_DRVP_setGain,                 \
                                                                                                      CGS132_DRVP_getGain,                 \
                                                                                                      CGS132_DRVP_trigger,                 \
                                                                                                      CGS132_DRVP_mirrorFlip,              \
                                                                                                      CGS132_DRVP_powerup ,                \
                                                                                                      CGS132_DRVP_powerdown ,              \
                                                                                                      CGS132_DRVP_changeSensorAddress,     \
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

static UINT16 CGS132_DRVP_lineLengthPclk;
static UINT16 CGS132_DRVP_x_output_size;
static UINT16 CGS132_DRVP_vts;
//static UINT8 CGS132_DRVPGainMethod;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static UINT16 CGS132_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   if (!deviceDescriptorP->pixClk)
   {
      UINT16 pll1_prediv0,pll1_prediv,pll1_mult,pll1_m_div, pll1_mipi_div, pix_clk, ref_clk;
      UINT8  predivVal[8] = {2,3,4,5,6,8,12,16};
      UINT8  mipiDiv[4]   = {4,5,6,8};
      deviceDescriptorP->pixClk = 108; // ?

 /*     retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pix_clk = ref_clk;

      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL1_PRE_DIV0_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_prediv0 = ( config.accessRegParams.data ) & 0x1;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL1_PRE_DIV_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL1_MULT_H_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mult = (( config.accessRegParams.data ) & 0x3) << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL1_MULT_L_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mult |= ( config.accessRegParams.data ) & 0xFF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,CGS132_DRVP_PLL1_M_DIV_ADDRESS , CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_m_div = ( config.accessRegParams.data ) & 0x0F;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL1_MIPI_DIV_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mipi_div = ( config.accessRegParams.data ) & 0x03;

      pix_clk = pix_clk / (pll1_prediv0 + 1);
      pix_clk = (pix_clk*2) / predivVal[pll1_prediv];
      pix_clk = pix_clk * pll1_mult;
      pix_clk = pix_clk / (pll1_m_div + 1);
      pix_clk = pix_clk / mipiDiv[pll1_mipi_div];

      pix_clk = (pix_clk * CGS132_DRVP_x_output_size) / (CGS132_DRVP_lineLengthPclk);

      deviceDescriptorP->pixClk = pix_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll1_prediv0 = %d, pll1_prediv = %d, pll1_mult = %d, pll1_m_div %d pll1_mipi_div %d pixClk = %d, ref_clk = %d\n",
                                  pll1_prediv0, pll1_prediv, pll1_mult,pll1_m_div,pll1_mipi_div,deviceDescriptorP->pixClk,ref_clk);
 */  }
   return deviceDescriptorP->pixClk;

}

static UINT16 CGS132_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   deviceDescriptorP->sysClk = 108;
   if (!deviceDescriptorP->sysClk)
   {
     UINT16 pll2_prediv,pll2_mult,pll2_sys_pre_div, pll2_sys_div, sys_clk, ref_clk;
      UINT8  predivVal[8]={2,3,4,5,6,8,12,16};
      UINT8  sysDiv[8]   ={2,3,4,5,6,7,8,10};

 /*      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      sys_clk = ref_clk;

      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL2_PRE_DIV_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_prediv = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL2_SYS_PRE_DIV_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_pre_div = ( config.accessRegParams.data ) & 0xF;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL2_SYS_DIV_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_sys_div = ( config.accessRegParams.data ) & 0x7;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,CGS132_DRVP_PLL2_LOOP_DIV_H_ADDRESS , CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_mult = config.accessRegParams.data&0x3 << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_PLL2_LOOP_DIV_L_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_mult |= config.accessRegParams.data&0xff;

      sys_clk = sys_clk*pll2_mult;
      sys_clk = (sys_clk*2) / predivVal[pll2_prediv];
      sys_clk = sys_clk/(pll2_sys_pre_div + 1);

      sys_clk = (sys_clk*2)/sysDiv[pll2_sys_div];
      deviceDescriptorP->sysClk = sys_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll2_prediv = %d, pll2_sys_pre_div = %d, pll2_sys_div = %d, pll2_mult %d sysClk = %d, ref_clk = %d\n", pll2_prediv, pll2_sys_pre_div, pll2_sys_div,pll2_mult,deviceDescriptorP->sysClk,ref_clk);
*/
   }
   return deviceDescriptorP->sysClk;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = CGS132_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = CGS132_DRVP_getPixClk(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   static unsigned int master=0;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = CGS132_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &CGS132_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     if ((pOpenParams->sensorSelect%2) == 0)
         deviceDescriptorP->sensorAddress   = CGS132_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
     else {
        deviceDescriptorP->sensorAddress   = CGS132_DRVP_I2C_SLAVE_ADDRESS;
        //Turn ON GIO54 to SET
        ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
        retVal = CGS132_DRVP_configGpio(54, GPIO_DRVG_GPIO_STATE_CLEAR_E);     //GPIO_DRVG_GPIO_STATE_CLEAR_E
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
     }
     deviceDescriptorP->ioctlFuncList   = CGS132_DRVP_ioctlFuncList;
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
*  Function Name: CGS132_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
***************************************************************************/
static ERRG_codeE CGS132_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn off the sensor
   retVal = CGS132_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
//   deviceDescriptorP->sensorAddress = CGS132_DRVP_I2C_MASTER_ADDRESS;
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   LOGG_PRINT(LOG_INFO_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x)\n",
           deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);
    //Turn on the sensor
   retVal = CGS132_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_SET_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(CGS132_DRVP_RESET_SLEEP_TIME);

   // reset sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID


   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, CGS132_DRVP_SC_CHIP_ID_HIGH_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E,NULL,"CHIPID 0x%x\n",config.accessRegParams.data);


   if (config.accessRegParams.data != CGS132_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 address = *(UINT8*)pParams;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
 /*  GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_SC_CTRL_2B_ADDRESS, address, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   deviceDescriptorP->sensorAddress = address;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_SC_CTRL_2B_ADDRESS, address, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(CGS132_DRVP_CHANGE_ADDRESS_SLEEP_TIME);

   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, CGS132_DRVP_SC_CTRL_2B_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Function Name: CGS132_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
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
      sensorTablePtr = CGS132_CFG_TBLG_single_configTable;
      regNum = sizeof(CGS132_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = CGS132_CFG_TBLG_stereo_vga_configTable;
         regNum = sizeof(CGS132_CFG_TBLG_stereo_vga_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         //we are using vertical res for hybrid. actualy we have to load for sensor full configuration.in addition will be more manipulation for hybrid, lut, etc..
         sensorTablePtr = CGS132_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(CGS132_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {

         sensorTablePtr = CGS132_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(CGS132_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {
         GEN_SENSOR_DRVG_sensorParametersT   config;
#if 0
         config.accessRegParams.data = 0;
         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GAIN_METHOD_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         CGS132_DRVPGainMethod = config.accessRegParams.data & 0xFF;
         //LOGG_PRINT(LOG_INFO_E,NULL,"gain method = %x\n", CGS132_DRVPGainMethod);
#endif
         config.accessRegParams.data = 0;
         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_LINE_LENGTH_HIGH_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         CGS132_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;

         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);
         CGS132_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

         expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
         CGS132_DRVP_getExposureTime(handle,&expParams);
         CGS132_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
         CGS132_DRVP_enStrobe(deviceDescriptorP);

#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3017, 0xf2, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_SC_CLKRST6_ADDRESS, 0x04, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3200, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x3201, 0x04, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);

         // enable group mode
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GRP_SW_CTRL_ADDRESS, 0x5, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Function Name: CGS132_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;


   retVal = CGS132_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_configSensors(IO_HANDLE handle, void *pParams)
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
       // Single Frame Trigger Mode from FAE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3222, 0x02, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x02
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3223, 0x48, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x48
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3226, 0x10, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x10
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3227, 0x10, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x10
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3217, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x00
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3218, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x00
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x322b, 0x0b, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x0b
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x320e, 0x3f, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x3f
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x320f, 0xff, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0xff
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3225, 0x04, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x04
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x300a, 0x62, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal); // 0x62


 /*     GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x320c, 0x8F, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302c, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302d, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302e, 0x50, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x302f, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x303f, 0x02, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x4242, 0x01, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3030, 0x80, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   0x3030, 0x84, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   CGS132_DRVP_STROBE_MASK_ADDRESS, 0x55, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      //trial and error - if value is higher, then frame is curropted in full mode. when value is lower, can't reach high  then
      //the calculation was taken from the CGS132_DRVP_setFrameRate function.
      sysClk = CGS132_DRVP_getSysClk(handle);
      CGS132_DRVP_vts = (1000000 * sysClk) / (CGS132_DRVP_lineLengthPclk * 23);

      CGS132_DRVP_vts = CGS132_DRVP_vts/3; //TODO: verify strobe duration on scope
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CGS132_DRVP_TIMING_VTS_LOW_ADDRESS, CGS132_DRVP_vts, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
 */
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);


     /*  IO_HANDLE *handleP;
       GEN_SENSOR_DRVG_exposureTimeCfgT expParams;
       UINT16 regNum;
       GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
       FIX_UNUSED_PARAM_WARN(pParams);
       handleP = (IO_HANDLE*) deviceDescriptorP;

    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)CGS132_CFG_trig_configTable, sizeof(CGS132_CFG_trig_configTable)/sizeof(CGS132_CFG_trig_configTable));


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
*  Function Name: CGS132_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE CGS132_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  CGS132_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE CGS132_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;\
   const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr;
   IO_HANDLE *handleP;
   UINT16 regNum;
   handleP = (IO_HANDLE*) deviceDescriptorP;

   LOGG_PRINT(LOG_INFO_E, NULL,"address %x params->triggerModeParams.isTriggerSupported %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      sensorTablePtr = CGS132_CFG_TBLG_single_configTable;
      regNum = sizeof(CGS132_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = CGS132_CFG_TBLG_stereo_vga_configTable;
         regNum = sizeof(CGS132_CFG_TBLG_stereo_vga_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         //we are using vertical res for hybrid. actualy we have to load for sensor full configuration.in addition will be more manipulation for hybrid, lut, etc..
         sensorTablePtr = CGS132_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(CGS132_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {
         sensorTablePtr = CGS132_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(CGS132_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableRead(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   CGS132_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              sysClk;
   UINT16                              frameLengthLine;

   sysClk = CGS132_DRVP_getSysClk(handle);
   if (ERRG_SUCCEEDED(retVal) && CGS132_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
        //frameLengthLine = (1000000 * sysClk) / (CGS132_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
        GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
        UINT32 groupHold;
        //change to next group
        deviceDescriptorP->group = !deviceDescriptorP->group;

        // group hold start
        groupHold = 0x00 | deviceDescriptorP->group;
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


      frameLengthLine = ((0x600*20)/params->setFrameRateParams.frameRate);
      //frameLengthLine = ((0x1194*30)/params->setFrameRateParams.frameRate);
      //frameLengthLine = ((0x600*50)/params->setFrameRateParams.frameRate);


      // frameLengthLine = frameLengthLine / 4;

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CGS132_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      CGS132_DRVP_vts = frameLengthLine;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CGS132_BLANK_ROW_HIGH, frameLengthLine - 6, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. CGS132_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,CGS132_DRVP_lineLengthPclk,frameLengthLine,sysClk);



#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
      //group hold end
      groupHold = 0x10 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      // group hold launch
      groupHold = 0xA0 | deviceDescriptorP->group;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif


   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines,maxExpLines;
   UINT16                              sysClk = 0;



   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   sysClk = CGS132_DRVP_getSysClk(handle);

   numExpLines = (params->exposureTime * sysClk * 16 * 2) / CGS132_DRVP_lineLengthPclk;
   numExpLines =  numExpLines&0xfffff;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_TIMING_VTS_LOW_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   maxExpLines = (config.accessRegParams.data -8)*16;
   if (numExpLines > maxExpLines)
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"numExpLines limits from 0x%x to 0x%x\n",numExpLines,maxExpLines);
      numExpLines = maxExpLines;
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_AEC_EXPO_1_ADDRESS, numExpLines, CGS132_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"sys_clk = %d, CGS132_DRVP_lineLengthPclk = %d, numExpLines = 0x%x maxExpLines 0x%x, time = %d, address = 0x%x, context = %d\n",sysClk,CGS132_DRVP_lineLengthPclk,numExpLines,maxExpLines,params->exposureTime,deviceDescriptorP->sensorAddress,params->context);


#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              sysClk;

//   if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
//      return SENSOR__RET_SUCCESS;

   params->exposureTime = 0;

   if (ERRG_SUCCEEDED(retVal))
   {
      LOGG_PRINT(LOG_DEBUG_E,NULL,"get exposure (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_AEC_EXPO_1_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines = config.accessRegParams.data & 0xfffff;


      sysClk = CGS132_DRVP_getSysClk(handle);
      params->exposureTime = (numExpLines * CGS132_DRVP_lineLengthPclk) / sysClk;
      params->exposureTime = params->exposureTime/16;
      LOGG_PRINT(LOG_INFO_E,NULL,"exposure time = %d, numExpLines = %d 0x%x, CGS132_DRVP_lineLengthPclk = %d, sys_clk = %d, context = %d\n",params->exposureTime,numExpLines,numExpLines,CGS132_DRVP_lineLengthPclk, sysClk,params->context);
   }

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT8 analogGain;
   UINT8 analogFineGain;
   UINT8 analogFineGainMinLevel = 0x20;
   UINT8 analogFineGainMaxLevel;
#if 0
   UINT8 digitalGain;
   UINT8 digitalFineGain;
#endif
   UINT16 tempGain;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 groupHold;
   //change to next group
   deviceDescriptorP->group = !deviceDescriptorP->group;

   // group hold start
   groupHold = 0x00 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   //check gain value to be set valid
   tempGain = (UINT32)params->analogGain;
   analogFineGain = (UINT8) (tempGain &0xff);
   analogGain = (UINT8) ((tempGain>>8) &0xff);

   if (analogGain == 0x03)
   {
      analogFineGainMaxLevel = 0x39;
   }
   else if ((analogGain == 0x23) || (analogGain == 0x27) || (analogGain == 0x2F) || (analogGain == 0x3F))
   {
      analogFineGainMaxLevel = 0x3F;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Unsuppoerted Analog Gain word: 0x%x\n", params->analogGain);
      return retVal;
   }

   if (analogFineGain > analogFineGainMaxLevel)
   {
      analogFineGain = analogFineGainMaxLevel;
   }

   if (analogFineGain < analogFineGainMinLevel)
   {
      analogFineGain = analogFineGainMinLevel;
   }

   tempGain = ((analogGain & 0x3F) << 8) | (analogFineGain & 0x3F);
   LOGG_PRINT(LOG_INFO_E,NULL,"Set Analog Gain word: 0x%x\n", tempGain);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_ANALOG_GAIN_1_ADDRESS, tempGain , CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#if 0
   tempGain = params->digitalGain;
   digitalFineGain = (UINT8) (tempGain &0xff);
   digitalGain = (UINT8) ((tempGain>>8) &0xff);

   if((digitalGain == 0x00) || (digitalGain == 0x01) || (digitalGain == 0x03) || (digitalGain == 0x07) ||(digitalGain == 0x0F))
   {
      if((digitalFineGain > 0xFC) || (digitalFineGain < 0x80))
      {
         return retVal;
      }
      else if(digitalFineGain%4)
      {
         return retVal;
      }
   }
   else
   {
      return retVal;
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_DIGL_GAIN_1_ADDRESS, params->digitalGain , CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
   //group hold end
   groupHold = 0x10 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // group hold launch
   groupHold = 0xA0 | deviceDescriptorP->group;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_GROUP_ACCESS_ADDRESS, groupHold, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
   //UINT8 dgtlGain[16]={1,2,0,4,1,1,1,8,
   //                    1,1,1,1,1,1,1,16};

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_DIGL_GAIN_1_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //params->digitalGain = dgtlGain[config.accessRegParams.data & 0xF];
   params->digitalGain = config.accessRegParams.data & 0xFFF;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_ANALOG_GAIN_1_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   params->analogGain = config.accessRegParams.data & 0x3FFF;
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
*  Function Name: CGS132_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/

static INT32 CGS132_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(CGS132_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(CGS132_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS,config.accessRegParams.data, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      config.accessRegParams.data |= 0x1 << 2;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS,config.accessRegParams.data, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum, sysClk = 0;

#ifdef GGGG
   sysClk = CGS132_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (CGS132_DRVP_lineLengthPclk - 1)) / (CGS132_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_PWM_CTRL_27_ADDRESS, (strobeUpTicksNum & 0xFFFF), CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#ifdef CGS132_DRVP_GROUP_MODE_EXP_CHANGE
   UINT32 strobeStartPoint;
   //VTS - Texposure - 7 - Tnegative
   strobeStartPoint = CGS132_DRVP_vts - strobeUpTicksNum - 7 - 0;
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_PWM_CTRL_29_ADDRESS, (strobeStartPoint & 0xFFFF), CGS132_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   LOGG_PRINT(LOG_DEBUG_E,NULL,"usec = %d, CGS132_DRVP_lineLengthPclk = %d, sysClk = %d, strobeUpTicksNum = 0x%x\n", usec, CGS132_DRVP_lineLengthPclk, sysClk, strobeUpTicksNum);
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: CGS132_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CGS132_DRVP_STROBE_EN_ADDRESS, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E,NULL,"strobe reg = 0x%x)\n",config.accessRegParams.data);

#ifdef GGGG
#ifndef CGS132_DRVP_GROUP_MODE_EXP_CHANGE

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_PWM_CTRL_2F_ADDRESS, 0x40, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_PWM_CTRL_24_ADDRESS, 0x0, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_STROBE_MASK_ADDRESS, 0xFF, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_STROBE_EN_ADDRESS, 0x08, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#else
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3006, 0x0c, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3210, 0x04, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3007, 0x02, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   CGS132_DRVP_SC_CLKRST6_ADDRESS, 0xf2, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3020, 0x20, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3025, 0x02, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382c 07  ; hts global tx msb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382c, 0x07, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 382d 10 ; hts global tx lsb
   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382d, 0x10, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3920 ff ; strobe pattern
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3920, 0xff, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3923 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3923, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3924 00 ; delay frame shift
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3924, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   //c0 3925 00 ; width frame span effective width
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3925, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3926, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3927, 0x01, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3928, 0x80, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3929, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392a, 0x14, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392b, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392c, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392d, 0x04, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392e, 0x00, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x392f, 0xcb, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x38b3, 0x07, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3885, 0x07, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x382b, 0x5a, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   0x3670, 0x68, CGS132_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#endif
#endif

   return retVal;
}

static ERRG_codeE CGS132_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
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
*  Function Name: CGS132_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;
   return retVal;
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
   return retCode;
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   CGS132_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   CGS132_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: CGS132_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
static ERRG_codeE CGS132_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
     return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: CGS132_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: CGS132 sensor driver
*
****************************************************************************/
ERRG_codeE CGS132_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(CGS132_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {

         deviceDescriptorP = &CGS132_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_CGS_132_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      CGS132_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &CGS132_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &CGS132_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &CGS132_DRVP_open;
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

