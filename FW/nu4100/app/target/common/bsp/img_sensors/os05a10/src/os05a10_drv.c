/****************************************************************************
 *
 *   FileName: OS05A10_drv.c
 *
 *   Author:  Gyiora A.
 *
 *   Date:
 *
 *   Description: OS05A10 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "os05a10_drv.h"
#include "os05a10_regs.h"
#include "os05a10_cfg_tbl.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include "iae_drv.h"
#include <unistd.h>
#include <errno.h>
#include "os_lyr.h"
#define XSHUTDOWN_SHARED
#define OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE OS05A10_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_configSensors(IO_HANDLE handle, void *params); //todo: implement it
static ERRG_codeE OS05A10_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_getGain(IO_HANDLE handle, void *pParams);

static ERRG_codeE OS05A10_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE OS05A10_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE OS05A10_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE OS05A10_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE OS05A10_DRVP_close(IO_HANDLE handle);
static ERRG_codeE OS05A10_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      OS05A10_DRVP_findFreeSlot();
static ERRG_codeE OS05A10_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state);

static ERRG_codeE OS05A10_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE OS05A10_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE OS05A10_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_powerup(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_getChipID(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_activateTestPattern(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_setBayerPattern(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_setBLC(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_setWB(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_changeResolution(IO_HANDLE handle, void *pParams);
static ERRG_codeE OS05A10_DRVP_exposureCtrl(IO_HANDLE handle, void *pParams);

///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////
/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  OS05A10_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   OS05A10_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        OS05A10_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      OS05A10_DRVP_resetSensors,            \
                                                                                                      OS05A10_DRVP_initSensor,              \
                                                                                                      OS05A10_DRVP_configSensors,           \
                                                                                                      OS05A10_DRVP_startSensors,            \
                                                                                                      OS05A10_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      OS05A10_DRVP_getDeviceId,             \
                                                                                                      OS05A10_DRVP_loadPresetTable,         \
                                                                                                      OS05A10_DRVP_setFrameRate,            \
                                                                                                      OS05A10_DRVP_setExposureTime,         \
                                                                                                      OS05A10_DRVP_getExposureTime,         \
                                                                                                      OS05A10_DRVP_setExposureMode,         \
                                                                                                      OS05A10_DRVP_setImgOffsets,           \
                                                                                                      OS05A10_DRVP_getAvgBrighness,         \
                                                                                                      OS05A10_DRVP_setOutFormat,            \
                                                                                                      OS05A10_DRVP_setPowerFreq,            \
                                                                                                      OS05A10_DRVP_setStrobe,               \
                                                                                                      OS05A10_DRVP_getSensorClks,           \
                                                                                                      OS05A10_DRVP_setGain,                 \
                                                                                                      OS05A10_DRVP_getGain,                 \
                                                                                                      OS05A10_DRVP_trigger,                 \
                                                                                                      OS05A10_DRVP_mirrorFlip,              \
                                                                                                      OS05A10_DRVP_powerup ,                \
                                                                                                      OS05A10_DRVP_powerdown ,              \
                                                                                                      OS05A10_DRVP_changeSensorAddress,     \
                                                                                                      OS05A10_DRVP_getChipID,               \
                                                                                                      OS05A10_DRVP_activateTestPattern,     \
                                                                                                      OS05A10_DRVP_setBayerPattern,         \
                                                                                                      OS05A10_DRVP_setBLC,                  \
                                                                                                      OS05A10_DRVP_setWB,                   \
                                                                                                      OS05A10_DRVP_changeResolution,        \
                                                                                                      OS05A10_DRVP_exposureCtrl,            \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub,                 \
                                                                                                      GEN_SENSOR_DRVG_stub};



static UINT16 OS05A10_DRVP_lineLengthPclk;
static UINT16 OS05A10_DRVP_x_output_size;
static UINT16 OS05A10_DRVP_vts;
#ifdef XSHUTDOWN_SHARED
/*
 * Note:
 *        Both of the VST FF sensors share the same
 *        XSHUTDOWN pin, so we need a mask to check the active sensors
 *        The mutex is not used now since the SESNSORS_MNGR
 *        will not bring up the sensors simulataneously.
 *        However this is not safe,
 */
static UINT32 OS05A10_DRVP_active_mask;
#endif
/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static inline UINT16 OS05A10_DRVP_preDivClk(UINT16 clk, UINT16 div_reg_val)
{
    switch (div_reg_val) {
        case 0:
            return clk;
        case 1:
            /* pre_div is 1.5(3/2)*/
            return  clk * 2 / 3;
        case 2:
            return clk >> 1;
        case 3:
            /* pre_div is 2.5(5/2)*/
            return clk * 2 / 5;
        case 4:
            return clk / 3;
        case 5:
            return clk >> 2;
        case 6:
            return clk / div_reg_val;
        case 7:
            return clk >> 3;
        default:
            return clk;
    }
}

static inline UINT16 OS05A10_DRVP_tcDivClk(UINT16 clk, UINT16 div_reg_val)
{
    switch (div_reg_val) {
        case 0:
            return clk;
        case 1:
            /* pre_div is 1.5(3/2)*/
            return  clk * 2 / 3;
        case 2:
            return clk >> 1;
        case 3:
            /* pre_div is 2.5(5/2)*/
            return clk * 2 / 5;
        case 4:
            return clk / 3;
        case 5:
            /* pre_div is 3.5(7/2)*/
            return clk * 2 / 7;
        case 6:
            return clk >> 2;
        case 7:
            return clk / 5;
        default:
            return clk;
    }
}

static UINT16 OS05A10_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   if (!deviceDescriptorP->pixClk)
   {
      UINT16 pll1_pre_divp, pll1_pre_div, pll1_loop, pll1_mipi_pre_div, pll1_mipi_div, dig_pclk_div;
      UINT16 pix_clk;
      UINT16 ref_clk;
      UINT8  mipiDiv[4]   = {4,5,6,8};
      
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pix_clk = ref_clk;
   
      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL1_PRE_DIVP_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_pre_divp = ( config.accessRegParams.data ) & 0x1;
      if (pll1_pre_divp) {
        pix_clk >>= 1;
      }

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL1_PRE_DIV_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_pre_div = ( config.accessRegParams.data ) & 0x7;
      pix_clk = OS05A10_DRVP_preDivClk(pix_clk, pll1_pre_div);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL1_LOOP_HIGH_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_loop = (( config.accessRegParams.data ) & 0x3) << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL1_LOOP_LOW_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_loop |= ( config.accessRegParams.data ) & 0xFF;
      pix_clk *= pll1_loop;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OS05A10_DRVP_MIPI_PRE_DIVIDER_ADDRESS , OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mipi_pre_div = ( config.accessRegParams.data ) & 0x0F;
      pix_clk /= (pll1_mipi_pre_div + 1);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_MIPI_DIVIDER_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll1_mipi_div = ( config.accessRegParams.data ) & 0x03;
      pix_clk /= mipiDiv[pll1_mipi_div];

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_DIG_PCLK_DIV_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      dig_pclk_div = config.accessRegParams.data & 0x8;
      if (dig_pclk_div) {
        pix_clk >>= 1;
      }

      deviceDescriptorP->pixClk = pix_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll1_pre_divp = %d, pll1_pre_div = %d, pll1_loop = %d, pll1_mipi_pre_div %d pll1_mipi_div %d pixClk = %d, ref_clk = %d\n", 
                                  pll1_pre_divp, pll1_pre_div, pll1_loop,pll1_mipi_pre_div,pll1_mipi_div,deviceDescriptorP->pixClk,ref_clk);
   }
   return deviceDescriptorP->pixClk;

}

static UINT16 OS05A10_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   if (!deviceDescriptorP->sysClk)
   {
      UINT16 pll2_pre_divp,pll2_pre_div,pll2_div_loop, pll2_tc_pre_divider,pll2_tc_divider, dig_salclk_div;
      UINT16 ref_clk;
      UINT16 sys_clk;
      
      retVal = GEN_SENSOR_DRVG_getRefClk(&ref_clk, deviceDescriptorP->sensorClk);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      sys_clk = ref_clk;
   
      config.accessRegParams.data = 0;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL2_PRE_DIVP_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_pre_divp = config.accessRegParams.data & 0x1;
      if (pll2_pre_divp) {
         sys_clk = (sys_clk >> 1);
      }

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL2_PRE_DIV_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      pll2_pre_div = config.accessRegParams.data & 0x7;
      sys_clk = OS05A10_DRVP_preDivClk(sys_clk, pll2_pre_div);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,OS05A10_DRVP_PLL2_LOOP_HIGH_ADDRESS , OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_div_loop = (config.accessRegParams.data & 0x3) << 8;
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL2_LOOP_LOW_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_div_loop |= config.accessRegParams.data & 0xff;

      sys_clk *= pll2_div_loop;

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL2_TC_PRE_DIVIDER_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_tc_pre_divider = config.accessRegParams.data & 0xF;
      sys_clk = sys_clk / (pll2_tc_pre_divider + 1);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_PLL2_TC_DIVIDER_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      pll2_tc_divider = config.accessRegParams.data & 0x7;
      sys_clk = OS05A10_DRVP_tcDivClk(sys_clk, pll2_tc_divider);
      
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_DIG_SALCLK_DIV_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      dig_salclk_div = config.accessRegParams.data & 0x1;
      if (dig_salclk_div) {
        sys_clk >>= 1;
      }

      deviceDescriptorP->sysClk = sys_clk;
      LOGG_PRINT(LOG_INFO_E,NULL,"pll2_pre_divp = %d, pll2_pre_div = %d, pll2_div_loop = %d, pll2_tc_pre_div = %d pll2_tc_div %d sysClk = %d, ref_clk = %d\n",
                     pll2_pre_divp, pll2_pre_div, pll2_div_loop, pll2_tc_pre_divider, pll2_tc_divider, sys_clk, ref_clk);

   }

   return deviceDescriptorP->sysClk;
}

#ifdef XSHUTDOWN_SHARED
static void OS05A10_DRVP_changeActiveMask(UINT32 i2cNum,BOOL active)
{
    if (active) {
        OS05A10_DRVP_active_mask |= (1 << i2cNum);
    } else {
        OS05A10_DRVP_active_mask &= ~(1 << i2cNum);
    }
}
#endif

static void OS05A10_DRVP_configAsSLAVE(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16 hts, vts;
   LOGG_PRINT(LOG_INFO_E,NULL,"Configuring OSO5A10 as slave for handle 0x%0x \n",handle);
   /* Stop streaming */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* IO_PAD_OEN: vsync out enable set to 0 */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_IO_PAD_OEN_L_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* PAD_CTRL: fsin output set to 0 */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_PAD_CTRL_ADDRESS, 0x2, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG7E: reset internal counter(daclk) */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG7E_ADDRESS, 0xa, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG97: long to short fixed distance offset */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG97_ADDRESS, 0x84, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG98: fixed long to short distance height */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG98_ADDRESS, 0x64, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* VSYNC CS: set to 0 */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_VSYNC_COL_START_H_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_VSYNC_COL_START_L_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* VSYNC R: H set to 0 L set to 1 */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_VSYNC_ROW_START_H_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_VSYNC_ROW_START_L_ADDRESS, 0x1, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG22: reset slave before vsync */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG22_ADDRESS, 0x74, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG23: frame sync enable , r counter initial manual enable */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG23_ADDRESS, 0x50, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* CS RST FSIN: column counter reset value */
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_TIMING_HTS_LOW_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   hts = config.accessRegParams.data & 0xFFFF;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_TIMING_VTS_LOW_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   vts = config.accessRegParams.data & 0xFFFF;

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_CS_RST_FSIN_H_ADDRESS, ((hts >> 1) & 0xFF00) >> 8, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_CS_RST_FSIN_L_ADDRESS, ((hts >> 1) & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_R_RST_FSIN_H_ADDRESS, ((vts - 4) & 0xFF00) >> 8, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_R_RST_FSIN_L_ADDRESS, ((vts - 4) & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG32: vsync width set to 0x02 */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG32_ADDRESS, 0x02, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG34: context en set to 0x04 */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG34_ADDRESS, 0x04, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   /* REG42: reverse long/short exposure order set to 0x0 for linear mode */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_REG42_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = OS05A10_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = OS05A10_DRVP_getPixClk(handle);

   return retVal;
}

static BOOL OS05A10_DRVP_isStreaming(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OS05A10_DRVP_SC_MODE_SELECT_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   return (config.accessRegParams.data & 0x1) ? TRUE : FALSE;
}

#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
/****************************************************************************
*
*  Function Name: OS05A10_DRVP_GroupHold

*
*  Description: Group hold start&end control
*
*  Inputs: IO_HANDLE handle - GEN_SENSOR_DRVG_specificDeviceDescT pointer
*          UINT32 group - which SRAM group to hold(0,1,2,3)
*          BOOL holdStart - TRUE: hold start FALSE: hold end
*  Outputs: NA
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static inline ERRG_codeE OS05A10_DRVP_GroupHold(IO_HANDLE handle, UINT32 group, BOOL holdStart)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 groupHold = 0;

   /* Clear for context switching */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP_SW_CTRL_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   /* Upper 4-bits to control hold start(0x0) or end(0x1) */
   groupHold = holdStart ? 0 : 0x10;
   /* Lower 4-bits to control which group to access */
   groupHold |= (group & 0xF);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

   if (holdStart) {
      /* Declare each group hold start */
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, 0x0808, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_GroupLaunch

*
*  Description: Group lanch(delay manual lauch)
*
*  Inputs: IO_HANDLE handle - GEN_SENSOR_DRVG_specificDeviceDescT pointer
*          UINT32 group - which SRAM group to hold(0,1,2,3)
*  Outputs: NA
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/

static inline ERRG_codeE OS05A10_DRVP_GroupLaunch(IO_HANDLE handle, UINT32 group)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 groupHold = 0;

#if 0
   /*Manual launch on*/
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP_SW_CTRL_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   /* delay launch group */
   groupHold = 0xA0 | (group & 0xF);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP_ACCESS_ADDRESS, groupHold, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   return retVal;
}
#endif /* OS05A10_DRVP_GROUP_MODE_EXP_CHANGE */

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = OS05A10_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &OS05A10_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     deviceDescriptorP->sensorAddress   = OS05A10_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
     deviceDescriptorP->ioctlFuncList   = OS05A10_DRVP_ioctlFuncList;
     deviceDescriptorP->i2cInstanceId   = pOpenParams->i2cInstanceId;
     deviceDescriptorP->i2cSpeed        = pOpenParams->i2cSpeed;
     deviceDescriptorP->sensorType      = pOpenParams->sensorType; //todo remove
     deviceDescriptorP->tableType       = pOpenParams->tableType;
     deviceDescriptorP->powerGpioMaster = pOpenParams->powerGpioMaster;
     deviceDescriptorP->fsinGpio        = pOpenParams->fsinGpio;
     deviceDescriptorP->sensorClk       = pOpenParams->sensorClk;
#ifdef XSHUTDOWN_SHARED
     if (OS05A10_DRVP_active_mask == 0) {
#endif
     GEN_SENSOR_DRVP_gpioInit(deviceDescriptorP->powerGpioMaster);
#ifdef XSHUTDOWN_SHARED
     }
#endif
     
     GEN_SENSOR_DRVP_setSensorRefClk(deviceDescriptorP->sensorClk, pOpenParams->sensorClkDiv);
     *handleP = (IO_HANDLE)deviceDescriptorP;
     if(pOpenParams->sensorType == INU_DEFSG_SENSOR_TYPE_STEREO_E)
     {
        //need to add another adressfor second slave maybe from XML
        deviceDescriptorP->sensorAddress = OS05A10_DRVP_I2C_GLOBAL_ADDRESS;
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
*  Function Name: OS05A10_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
***************************************************************************/
static ERRG_codeE OS05A10_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle; 
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

#ifdef XSHUTDOWN_SHARED
   OS05A10_DRVP_changeActiveMask(deviceDescriptorP->i2cInstanceId, FALSE);
   if (OS05A10_DRVP_active_mask == 0) {
#endif
   //Turn off the sensor 
   retVal = OS05A10_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
#ifdef XSHUTDOWN_SHARED
   }
#endif
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //after powerdown, the i2c address returns to default
   deviceDescriptorP->sensorAddress = OS05A10_DRVP_I2C_MASTER_ADDRESS;
   return retVal;
}

//Debug codes, TODO: delete it when release the product version, keep it for now
static void DumpSensorHandle(GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP)
{
    printf("sensorAddress: 0x%x\n", deviceDescriptorP->sensorAddress);
    printf("i2cInstanceId: %d\n", deviceDescriptorP->i2cInstanceId);
    printf("sensroType: %d\n", deviceDescriptorP->sensorType);
    printf("tableType: %d\n", deviceDescriptorP->tableType);
    printf("fsinGpio: %d\n", deviceDescriptorP->fsinGpio);
    printf("powerGpio: %d\n", deviceDescriptorP->powerGpioMaster);
    printf("sensorClk:%d\n", deviceDescriptorP->sensorClk);
    printf("sensorId:%d\n", deviceDescriptorP->sensorId);
}

extern UINT32 GME_DRVG_readReg(UINT32 addrOffset);
extern ERRG_codeE GME_DRVG_writeReg(GME_DRVG_writeRegT *params);
static void DumpClockRegs()
{
    UINT32 clk_en = GME_DRVG_readReg(0x88);
    GME_DRVG_writeRegT wr;

    clk_en |= (0x3FU << 18U);
    wr.offsetAddress = 0x84;
    wr.val = clk_en;
    GME_DRVG_writeReg(&wr);

    //Periph clock en
    clk_en = GME_DRVG_readReg(0xBC);
    clk_en |= 0xFFFF;
    wr.offsetAddress = 0xBC;
    wr.val = clk_en;
    GME_DRVG_writeReg(&wr);

    wr.offsetAddress = 0xA4;
    wr.val = (0x3 << 23) | (0x3 << 12);
    GME_DRVG_writeReg(&wr);

    OS_LYRG_usleep(1000000);

    printf("GME_CLK_ENABLE_STATUS(i2c 18-23): 0x%08X\n", GME_DRVG_readReg(0x88));
    printf("GME_PERIPH_CLOCK_CONFIG: 0x%08X\n", GME_DRVG_readReg(0xB8));
    printf("GME_PERIPH_CLOCK_EN_STATUS: 0x%08X\n", GME_DRVG_readReg(0x148));
    printf("GME_FREQ_CHG_STATUE: 0x%08x\n", GME_DRVG_readReg(0xA8));
    printf("GME_POWER_MODE_STATUS: 0x%08X\n", GME_DRVG_readReg(0xCC));
}
/****************************************************************************
*
*  Function Name: OS05A10_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle; 
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

#ifdef XSHUTDOWN_SHARED
   if (OS05A10_DRVP_active_mask == 0) {
#endif
   //Turn on the sensor
   retVal = OS05A10_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_SET_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#ifdef XSHUTDOWN_SHARED
   }
   OS05A10_DRVP_changeActiveMask(deviceDescriptorP->i2cInstanceId, TRUE);
#endif
   OS_LYRG_usleep(OS05A10_DRVP_RESET_SLEEP_TIME);

   // reset sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_SC_SOFTWARE_RESET_ADDRESS, 1, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   // set sensor(s)
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_SC_SOFTWARE_RESET_ADDRESS, 0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   //read chipID
   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OS05A10_DRVP_SC_CHIP_ID_HIGH_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_INFO_E, NULL, "Chip ID read from sensor: 0x%x\n", config.accessRegParams.data);
   if (config.accessRegParams.data != OS05A10_DRVP_SC_CHIP_ID)
      return SENSOR__ERR_UNEXPECTED;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(pParams);
   //Leave empty for now since current sensor uses a single I2C controller
#if 0
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT8 address = *(UINT8*)pParams;

   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;    

   LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   OS05A10_DRVP_SC_SCCB_ID1_ADDRESS, address, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(OS05A10_DRVP_CHANGE_ADDRESS_SLEEP_TIME);

   deviceDescriptorP->sensorAddress = address;

   config.accessRegParams.data = 0;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OS05A10_DRVP_SC_SCCB_ID1_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   if (config.accessRegParams.data != address)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Error changing address! (%x) (requested %x)\n",config.accessRegParams.data,address);
   }
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle); 
   LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
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

   if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_USER_DEF_E)
   {
      sensorTablePtr = OS05A10_CFG_TBLG_scan_configTable;
      regNum = sizeof(OS05A10_CFG_TBLG_scan_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
      sensorTablePtr = OS05A10_CFG_TBLG_single_configTable;
      regNum = sizeof(OS05A10_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {
         GEN_SENSOR_DRVG_sensorParametersT   config;
         config.accessRegParams.data = 0;  
         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_TIMING_HTS_LOW_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_2_BYTE, retVal);   
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);   
         OS05A10_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;

         GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_TIMING_X_OUTPUT_SIZE_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_2_BYTE, retVal);   
         GEN_SENSOR_DRVG_ERROR_TEST(retVal);   
         OS05A10_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

         expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
         OS05A10_DRVP_getExposureTime(handle,&expParams);
         OS05A10_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
         OS05A10_DRVP_enStrobe(deviceDescriptorP);

#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP0_PERIOD_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP1_PERIOD_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
         GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_GROUP_SW_CTRL_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
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
*  Function Name: OS05A10_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

   retVal = OS05A10_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT16                              sysClk;
   (void)pParams;(void)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
      //trial and error - if value is higher, then frame is curropted in full mode. when value is lower, can't reach high fps then 
      //the calculation was taken from the OS05A10_DRVP_setFrameRate function.
      params->triggerModeParams.isTriggerSupported = 1;
      OS05A10_DRVP_configGpio(deviceDescriptorP->fsinGpio, GPIO_DRVG_GPIO_STATE_CLEAR_E);
      OS05A10_DRVP_configAsSLAVE(handle); 
   }
   else
   {
      params->triggerModeParams.isTriggerSupported = 0;
   }

   //TODO: need to investigate the White Balance parameters, below are only demo values for my enviroment
   //      May be removed later or be configured by AWB apis
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_B_H_ADDRESS, 0x7, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_G_H_ADDRESS, 0x4, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_R_H_ADDRESS, 0x7, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

   params->triggerModeParams.gpioBitmap |= (((UINT64)1) << deviceDescriptorP->fsinGpio);
   params->triggerModeParams.pulseTime = 300;

   return retVal;

}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE OS05A10_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  OS05A10_DRVP_SC_MODE_SELECT_ADDRESS, 0x0, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE OS05A10_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   OS05A10_DRVP_SC_MODE_SELECT_ADDRESS, 0x1, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   /* From the datasheet of the OS05A10 - Entering streaming mode takes 10ms (Section 2.7)[t4] so we need to sleep for 
   10ms so that both sensors are ready for when FTRIG2 starts toggling */
   OS_LYRG_usleep(10000);
   return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setFrameRate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT16                              sysClk;
   UINT16                              frameLengthLine;
   BOOL                                isStreaming;

   sysClk = OS05A10_DRVP_getSysClk(handle);
   if (ERRG_SUCCEEDED(retVal) && OS05A10_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
   {
        frameLengthLine = ((1000000 * sysClk) + ((OS05A10_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate) - 1)) / (OS05A10_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

        isStreaming = OS05A10_DRVP_isStreaming(handle);
#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
        if (isStreaming)
        {
           retVal = OS05A10_DRVP_GroupHold(handle, 0, TRUE);
           GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        }
#endif

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, OS05A10_DRVP_TIMING_VTS_LOW_ADDRESS, frameLengthLine, OS05A10_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        OS05A10_DRVP_vts = frameLengthLine;
        LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. OS05A10_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,OS05A10_DRVP_lineLengthPclk,frameLengthLine,sysClk);      

#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
        if (isStreaming)
        {
            retVal = OS05A10_DRVP_GroupHold(handle, 0, FALSE);
            GEN_SENSOR_DRVG_ERROR_TEST(retVal);
            retVal = OS05A10_DRVP_GroupLaunch(handle, 0);
        }
#endif
   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;
   UINT16                              sysClk = 0;
   BOOL                                isStreaming;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

   isStreaming = OS05A10_DRVP_isStreaming(handle);
#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
   if (isStreaming)
   {
       retVal = OS05A10_DRVP_GroupHold(handle, 0, TRUE);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
#endif

   if (OS05A10_DRVP_lineLengthPclk != 0)
   {
      sysClk = OS05A10_DRVP_getSysClk(handle);
      numExpLines = ((params->exposureTime * sysClk) + (OS05A10_DRVP_lineLengthPclk - 1)) / OS05A10_DRVP_lineLengthPclk; /* setting same exp to both sensor using stereo handle */

      if (numExpLines < OS05A10_DRVP_MIN_EXPLINES) {
         numExpLines = OS05A10_DRVP_MIN_EXPLINES;
      } else if (numExpLines > (OS05A10_DRVP_vts - 8U)) {
         numExpLines = (OS05A10_DRVP_vts - 8U);
      }

      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_EXPO_H_ADDRESS, ((numExpLines >> 0x8) & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_EXPO_L_ADDRESS, (numExpLines & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "numExpLines - %u, sysClk - %u, min_explines - %u VTS - %u\n", numExpLines, sysClk, OS05A10_DRVP_MIN_EXPLINES, OS05A10_DRVP_vts);
   }

#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
   if (isStreaming)
   {
      retVal = OS05A10_DRVP_GroupHold(handle, 0, FALSE);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      retVal = OS05A10_DRVP_GroupLaunch(handle, 0);
   }
#endif
   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_getExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
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

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_EXPO_H_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= ((config.accessRegParams.data & 0xFF) << 8);

      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_EXPO_L_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      numExpLines |= (config.accessRegParams.data & 0xFF);

      sysClk = OS05A10_DRVP_getSysClk(handle);
      params->exposureTime = (numExpLines * OS05A10_DRVP_lineLengthPclk) / sysClk;
      LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d, OS05A10_DRVP_lineLengthPclk = %d, sys_clk = %d, context = %d\n",params->exposureTime,numExpLines,OS05A10_DRVP_lineLengthPclk, sysClk,params->context);
   }
   return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   BOOL                                isStreaming;

   /* Use real gain format */
   if ((UINT32)params->analogGain > 0x7FF)
   {
      params->analogGain = (float)0x7FF;
   }

   isStreaming = OS05A10_DRVP_isStreaming(handle);
#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
   if (isStreaming)
   {
      retVal = OS05A10_DRVP_GroupHold(handle, 0, TRUE);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   }
#endif

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_GAIN_H_ADDRESS, ((UINT32)params->analogGain >> 8) & 0x3F, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_GAIN_L_ADDRESS, ((UINT32)params->analogGain & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#ifdef OS05A10_DRVP_GROUP_MODE_EXP_CHANGE
   if (isStreaming)
   {
      retVal = OS05A10_DRVP_GroupHold(handle, 0, FALSE);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      retVal = OS05A10_DRVP_GroupLaunch(handle, 0);
   }
#endif

   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
   UINT16 analogGain;

   //if (params->context == INU_DEFSG_SENSOR_CONTEX_B)
   //   return SENSOR__RET_SUCCESS;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_GAIN_L_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   analogGain = config.accessRegParams.data & 0xFF;
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_LONG_GAIN_H_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   analogGain |= ((config.accessRegParams.data & 0x3F) << 8);
   params->analogGain = (float)analogGain;

   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT32                              regVal[4];
   GEN_SENSOR_DRVG_getDeviceIdParamsT  *deviceIdParamsP = (GEN_SENSOR_DRVG_getDeviceIdParamsT*)pParams;

   FIX_UNUSED_PARAM_WARN(handle);

   regVal[0] = 0x41;
   regVal[1] = 0x05;
   regVal[2] = 0x53;
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
*  Function Name: OS05A10_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/

static INT32 OS05A10_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(OS05A10_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(OS05A10_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT8 format1_reg_val, format2_reg_val;
   
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);    
   format1_reg_val = config.accessRegParams.data & 0xFF;
   format1_reg_val &= ~(0x1 << 2);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);    
   format2_reg_val = config.accessRegParams.data & 0xFF;
   format2_reg_val &= ~(0x1 << 2);

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
       format1_reg_val |= 0x1 << 2;
   }

   if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
   {
       format2_reg_val |= 0x1 << 2;
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_IMAGE_ORIENTATION_HORIZONTAL_ADDRESS, format1_reg_val, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_IMAGE_ORIENTATION_VERTICAL_ADDRESS, format2_reg_val, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);  
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);
   
   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT16                              strobeUpTicksNum, sysClk = 0;

   sysClk = OS05A10_DRVP_getSysClk(handle);
   strobeUpTicksNum = ((usec * sysClk) + (OS05A10_DRVP_lineLengthPclk - 1)) / (OS05A10_DRVP_lineLengthPclk); /* round up the result. we want the strobe to cover all the exposure time (will cause power waste but reduce signal noise) */

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_STROBE_H_ADDRESS, ((strobeUpTicksNum >> 8) & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_STROBE_L_ADDRESS, (strobeUpTicksNum & 0xFF), OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   LOGG_PRINT(LOG_DEBUG_E,NULL,"usec = %d, OS05A10_DRVP_lineLengthPclk = %d, sysClk = %d, strobeUpTicksNum = 0x%x\n", usec, OS05A10_DRVP_lineLengthPclk, sysClk, strobeUpTicksNum);

   return retVal;
}


/****************************************************************************
*
*  Function Name: OS05A10_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   /* strobe mode[2:0] - LED1(0x1), polarity[6] - high(0), strobe on[7] - on(1)*/
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_STROBE_CTRL0_ADDRESS, 0x81, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   /* strobe latency[1:0] - next frame(0), repeat enable[2] - en(1)*/
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_STROBE_CTRL1_ADDRESS, 0x4, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;
}

static ERRG_codeE OS05A10_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
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
*  Function Name: OS05A10_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;   
   return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;     
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params); 
   return retCode;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   OS05A10_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   OS05A10_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_getChipID
*
*  Description: read chip id of the sensor
*
*  Inputs:
*
*  Outputs: chipID - UINT32 to hold the chip id information
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_getChipID(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT32 *chipID = pParams;

    config.accessRegParams.data = 0;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, OS05A10_DRVP_SC_CHIP_ID_HIGH_ADDRESS, OS05A10_DRVP_SENSOR_ACCESS_3_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    *chipID = config.accessRegParams.data;

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_activateTestPattern
*
*  Description: activate test pattern for the sensor
*
*  Inputs: enable - TRUE: enable sensor test pattern
*                   FALSE: disable sensor test pattern
*
*  Outputs:
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_activateTestPattern(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    BOOLEAN enable = *(BOOLEAN *)pParams;
    if (enable != deviceDescriptorP->testPatternEn) {
        UINT16 regVal = enable ? (1 << 7) : 0;

        if (enable) {
            /* Test pattern set to a fixed pattern, e.g: vertical colorbar */
            retVal |= (TEST_PAT_TYPE_VBAR << 2);
        }

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_TEST_PAT_ADDRESS, regVal, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        deviceDescriptorP->testPatternEn = enable;
    }

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setBayerPattern
*
*  Description: set bayer pattern for sensor
*
*  Inputs: pattern:  0  -  BAYER RGGB,  1  -  BAYER GRBG,
*                    2  -  BYAER GBRG,  3  -  BAYER BGGR
*
*  Outputs:
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setBayerPattern(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    UINT8 pattern = *(UINT8 *)pParams;
    /*
     * this function will do nothing for now
     */

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setBLC
*
*  Description: set black level correction r,gr,gb,b offset
*
*  Inputs: arg: offsets for each channel(R,Gr,Gb,B)
*
*  Outputs:
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setBLC(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    /*
     * Using default pedestal settings,
     * this function will do nothing for now
     */

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_setWB
*
*  Description: set white balance gains
*
*  Inputs: arg : gains for each channel(R,Gr,Gb,B)
*
*  Outputs:
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_setWB(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_sensorParametersT   *WBParam = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT16 r_gain, g_gain, b_gain;

    r_gain = WBParam->rgbGainParams.r & 0xFFF;
    g_gain = WBParam->rgbGainParams.gr & 0xFFF;
    b_gain = WBParam->rgbGainParams.b & 0xFFF;

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_R_H_ADDRESS, r_gain >> 8, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_R_L_ADDRESS, r_gain & 0xFF, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_G_H_ADDRESS, g_gain >> 8, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_G_L_ADDRESS, g_gain & 0xFF, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_B_H_ADDRESS, b_gain >> 8, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, OS05A10_DRVP_AWB_GAIN_B_L_ADDRESS, b_gain & 0xFF, OS05A10_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_changeResolution
*
*  Description: change sensor's resolution
*
*  Inputs: arg: width - image width, height - image height
*
*  Outputs:
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_changeResolution(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    /*
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
     */

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_exposureCtrl
*
*  Description: change sensor's exposure & gain (considering the HDR mode)
*
*  Inputs: integrationTime        - new integrationTime to be set
*          gain                   - new gain to be set
*          pNumOfFramesToSkip     - the number of frames after which the new setting are valid
*          pGainHwSet             - the actual gain set by sensor HW
*          pIntegrationTimeSet    - the actual integration time set by sensor HW
*          pHdr_ratio             - as input: the HDR ratio for sensor;
*                                   as output: the actual HDR ratio set by sensor HW
*  Outputs:
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_exposureCtrl(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    /*
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
     */

    return retVal;
}

/****************************************************************************
*
*  Function Name: OS05A10_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
static ERRG_codeE OS05A10_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
   return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: OS05A10_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: OS05A10 sensor driver
*
****************************************************************************/
ERRG_codeE OS05A10_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(OS05A10_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {
      
         deviceDescriptorP = &OS05A10_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_OS05A10_E;
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
      }
      OS05A10_DRVP_isDrvInitialized = TRUE;
   }
#ifdef XSHUTDOWN_SHARED
   OS05A10_DRVP_active_mask = 0;
#endif
   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &OS05A10_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &OS05A10_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &OS05A10_DRVP_open;
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

