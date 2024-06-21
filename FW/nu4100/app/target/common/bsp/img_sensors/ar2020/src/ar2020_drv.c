/****************************************************************************
 *
 *   FileName: AR2020_drv.c
 *
 *   Author:  Giyora A.
 *
 *   Date:
 *
 *   Description: AR2020 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "ar2020_drv.h"
#include "ar2020_cfg_tbl.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include "isp_mngr.hpp"
#include "helsinki.h"
#include <unistd.h>
#include <errno.h>

#define HELSINKI_MULTI_AR2020 1

/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
    {
        AR2020_XSHUTDOWN_R_E,
        AR2020_XSHUTDOWN_L_E,
        AR2020_FSIN_CV_E,
        AR2020_XSHUTDOWN_W_E,
        AR2020_FSIN_CLR_E,
        AR2020_MAX_NUM_E
    }AR2020_gpioNumEntryE;
int gpiosAR2020[AR2020_MAX_NUM_E] = {-1,-1,-1,-1,-1};

#define CHECK_GPIO(AR2020_gpioNumEntryE) if (gpiosAR2020[AR2020_gpioNumEntryE] != -1)

static ERRG_codeE AR2020_DRVG_PD50LE_getLensDac(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVG_PD50LE_setLensDac(IO_HANDLE handle, void *pParams);

/***************************************************************************
 ***************      L O C A L      D E C L A R A T I O N S    *************
 ****************************************************************************/

static ERRG_codeE AR2020_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE AR2020_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE AR2020_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE AR2020_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE AR2020_DRVP_close(IO_HANDLE handle);
static ERRG_codeE AR2020_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      AR2020_DRVP_findFreeSlot();
static INT32      AR2021_DRVP_findFreeSlot();
static ERRG_codeE AR2020_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_configGpio(AR2020_gpioNumEntryE gpioNumEntry,GPIO_DRVG_gpioStateE state);
static ERRG_codeE AR2020_DRVP_getTemperature(IO_HANDLE handle, void *pParams);

static ERRG_codeE AR2020_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE AR2020_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE AR2020_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_powerup(IO_HANDLE handle, void *pParams);
static UINT16     AR2020_DRVP_getSysClk(IO_HANDLE handle);

static ERRG_codeE AR2020_DRVG_PD50LE_init(IO_HANDLE handle);
static ERRG_codeE AR2020_DRVP_getIspInitParams(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_setCropWindow(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVP_getCropWindow(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVG_PD50LE_getLensDac(IO_HANDLE handle, void *pParams);
static ERRG_codeE AR2020_DRVG_PD50LE_setLensDac(IO_HANDLE handle, void *pParams);


/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define AR2020_DRVP_RESET_SLEEP_TIME                                  (3*1000)   // 3ms
#define AR2020_DRVP_CHANGE_ADDR_SLEEP_TIME                            (1*1000)   // 1ms
#define AR2020_DRVP_SENSOR_ACCESS_1_BYTE                              (1)
#define AR2020_DRVP_SENSOR_ACCESS_2_BYTE                              (2)
#define AR2020_DRVP_MAX_GAIN                                          (AR2020_DRVP_gainLut[(GAIN_LUT_SIZE - 1)])
///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define AR2020_DRVP_SC_CHIP_ID_LOW_ADDRESS                            (0x0016) //(0x3000)

#define AR2020_DRVP_SOFTWARE_RESET_ADDRESS                            (0x0103) //(0x3208)
#define AR2020_DRVP_GROUP_ACCESS_ADDRESS                              (0x0104) //(0x3208)

#define AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS                   (0x0202)

#define AR2020_DRVP_TIMING_X_OUTPUT_SIZES_LOW_ADDRESS                 (0x034c) //(0x3808)
#define AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS                           (0x0342) // (0x300C)      //[Dima] - new (0x380C)
#define AR2020_DRVP_FRAME_LENGTH_LINES_ADDRESS                        (0x0340) //   (0x380D)

#define AR2020_DRVP_IMAGE_ORIENTATION_ADDRESS                         (0x0101)

#define AR2020_DRVP_STROBE_EN_ADDRESS                                 (0x44dc) //(0x3005)

#define X_OUTPUT_OFFSET2                                              (0x3C6A)
#define X_OUTPUT_SIZE2                                                (0x3C6C)
#define Y_OUTPUT_OFFSET2                                              (0x3C70)
#define Y_OUTPUT_SIZE2                                                (0x3C72)

#define X_ADDR_START                                                  (0x0344)
#define X_ADDR_END                                                    (0x0348)
#define Y_ADDR_START                                                  (0x0346)
#define Y_ADDR_END                                                    (0x034A)
///////////////////////////////////////////////////////////
// REGISTERS ADDRESSES
///////////////////////////////////////////////////////////
#define AR2020_DRVP_COARSE_INTEGRATION_TIME_LO_ADDRESS                 (0x3d28) //(0x3012)
#define AR2020_DRVP_COARSE_INTEGRATION_TIME_HI_ADDRESS                 (0x3d26) //(0x3012)

#define AR2020_DRVP_RESET_AND_MISC_CONTROL_ADDRESS                    (0x301A)
#define AR2020_DRVP_TRIGGER_MODE_ADDRESS                              (0x3140)
#define AR2020_DRVP_VT_PIX_CLK_DIVIDER_ADDRESS                        (0x0300) //(0x302A)
#define AR2020_DRVP_VT_SYS_CLK_DIVIDER_ADDRESS                        (0x0302) //(0x302C)
#define AR2020_DRVP_OP_PIX_CLK_DIVIDER_ADDRESS                        (0x0308) //(0x3036)
#define AR2020_DRVP_OP_SYS_CLK_DIVIDER_ADDRESS                        (0x030a) //(0x3038)
#define AR2020_DRVP_PRE_PLL_DIVIDER_ADDRESS                           (0x0304) //(0x302E)
#define AR2020_DRVP_PLL_MULTIPLIER_ADDRESS                            (0x0306) //(0x3030)

#define AR2020_DRVP_GLOBAL_GAIN_ADDRESS                               (0x3062) //(0x305E)
#define AR2020_DRVP_ANALOG_GAIN_ADDRESS                               (0x3410) //(0x3060)

/* Temperature sensor control */
#define AR2020_DRVP_TPM_SENSOR_ADC_OUTPUT_ADDRESS                     (0x33C8)
#define AR2020_DRVP_TPM_SENSOR_CALIB_ADC1_ADDRESS                     (0x33D2)

#define AR2020_DRVP_ANALOG_GAIN_MASK                                   (0x3)
#define AR2020_DRVP_ANALOG_GAIN_START_BIT                              (2)


///////////////////////////////////////////////////////////
// REGISTERS VALUES
///////////////////////////////////////////////////////////
#define AR2020_DRVP_SC_CHIP_ID                                        (0x0653)
#define AR2020_DRVP_I2C_DEFAULT_ADDRESS                               (0x6c)
#define AR2020_DRVP_I2C_MASTER_ADDRESS                                (AR2020_DRVP_I2C_DEFAULT_ADDRESS)
#define AR2020_DRVP_I2C_SLAVE_ADDRESS                                 (0x6c)
#define AR2020_DRVP_I2C_GLOBAL_ADDRESS                                (0x6c)

#define AR2020_DRVP_I2C_PD50LE_ADDRESS                                (0x18)


///////////////////////////////////////////////////////////
// MACROS
///////////////////////////////////////////////////////////


/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  AR2020_DRVP_isDrvInitialized = FALSE;
static BOOL                                  AR2021_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   AR2020_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_specificDeviceDescT   AR2021_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        AR2020_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                    AR2020_DRVP_resetSensors,            \
                                                                                                    AR2020_DRVP_initSensor,              \
                                                                                                    AR2020_DRVP_configSensors,           \
                                                                                                    AR2020_DRVP_startSensors,            \
                                                                                                    AR2020_DRVP_stopSensors,             \
                                                                                                    GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                    AR2020_DRVP_getDeviceId,             \
                                                                                                    AR2020_DRVP_loadPresetTable,         \
                                                                                                    AR2020_DRVP_setFrameRate,            \
                                                                                                    AR2020_DRVP_setExposureTime,         \
                                                                                                    AR2020_DRVP_getExposureTime,         \
                                                                                                    AR2020_DRVP_setExposureMode,         \
                                                                                                    AR2020_DRVP_setImgOffsets,           \
                                                                                                    AR2020_DRVP_getAvgBrighness,         \
                                                                                                    AR2020_DRVP_setOutFormat,            \
                                                                                                    AR2020_DRVP_setPowerFreq,            \
                                                                                                    AR2020_DRVP_setStrobe,               \
                                                                                                    AR2020_DRVP_getSensorClks,           \
                                                                                                    AR2020_DRVP_setGain,                 \
                                                                                                    AR2020_DRVP_getGain,                 \
                                                                                                    AR2020_DRVP_trigger,                 \
                                                                                                    AR2020_DRVP_mirrorFlip,              \
                                                                                                    AR2020_DRVP_powerup ,                \
                                                                                                    AR2020_DRVP_powerdown ,              \
                                                                                                    AR2020_DRVP_changeSensorAddress,     \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    GEN_SENSOR_DRVG_stub,                \
                                                                                                    AR2020_DRVP_getIspInitParams,        \
                                                                                                    AR2020_DRVG_PD50LE_getLensDac,       \
                                                                                                    AR2020_DRVG_PD50LE_setLensDac,       \
                                                                                                    AR2020_DRVP_setCropWindow,			 \
                                                                                                    AR2020_DRVP_getCropWindow,			 \
                                                                                                    AR2020_DRVP_getTemperature};

static UINT16 AR2020_DRVP_lineLengthPclk;
static UINT16 AR2020_DRVP_x_output_size;
static UINT16 AR2020_DRVP_vts;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
static UINT16 AR2020_DRVP_getPixClk(IO_HANDLE handle)
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
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_PLL_MULTIPLIER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        pll_mult = ( config.accessRegParams.data ) & 0x3FF;

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_PRE_PLL_DIVIDER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        pll_prediv = ( config.accessRegParams.data ) & 0x3F;

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_VT_SYS_CLK_DIVIDER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        vt_sys_clk_div = ( config.accessRegParams.data ) & 0x1F;

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_VT_PIX_CLK_DIVIDER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        vt_pix_clk_div = ( config.accessRegParams.data ) & 0x1F;

        if (ERRG_SUCCEEDED(retVal))
        {
            if (( pll_prediv != 0 ) && ( vt_sys_clk_div != 0) &&  (vt_pix_clk_div != 0))
            {
                deviceDescriptorP->pixClk = (ref_clk * pll_mult) / (pll_prediv * vt_sys_clk_div * vt_pix_clk_div);
                //deviceDescriptorP->pixClk = deviceDescriptorP->pixClk/2; // /2=(numLanes/4)
            }
            else
            {
                deviceDescriptorP->pixClk = 0;
                LOGG_PRINT(LOG_ERROR_E, NULL, "pll_mult = %d, pll_prediv = %d, vt_sys_clk_div = %d, vt_pix_clk_div = %d, ref_clk = %d\n", pll_mult, pll_prediv, vt_sys_clk_div, vt_pix_clk_div, ref_clk);
                return ERR_UNEXPECTED;
            }
        }
    }
    //deviceDescriptorP->pixClk = 172 / 2;
    return deviceDescriptorP->pixClk;
}


static UINT16 AR2020_DRVP_getSysClk(IO_HANDLE handle)
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
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_PRE_PLL_DIVIDER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        pll_prediv = ( config.accessRegParams.data ) & 0x3F;

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_PLL_MULTIPLIER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        pll_mult = ( config.accessRegParams.data ) & 0x3FF;

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_OP_SYS_CLK_DIVIDER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        sysClk_div = ( config.accessRegParams.data ) & 0x1F;

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_OP_PIX_CLK_DIVIDER_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        pxlClk_div = ( config.accessRegParams.data ) & 0x1F;

        deviceDescriptorP->sysClk = ref_clk*pll_mult;
        deviceDescriptorP->sysClk = (deviceDescriptorP->sysClk/(pll_prediv*pxlClk_div*sysClk_div))*2;
        //deviceDescriptorP->sysClk = deviceDescriptorP->sysClk/2;// /2=(numLanes/4)
    }
    //deviceDescriptorP->sysClk = 172;

    return deviceDescriptorP->sysClk;
    /// return system clock to the sys
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_getPixelClk
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

    params->sensorClocksParams.sysClkMhz   = AR2020_DRVP_getSysClk(handle);
    params->sensorClocksParams.pixelClkMhz = AR2020_DRVP_getPixClk(handle);
    return retVal;
}

/****************************************************************************
*
*  Function Name: AR2020_DRVP_getIspInitParams
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
static ERRG_codeE AR2020_DRVP_getIspInitParams(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    params->ispInitParams.hts                = 832;
    params->ispInitParams.sclk               = 45000000;

    params->ispInitParams.frmLengthLines     = 1120;

    params->ispInitParams.minIntegrationLine = 4;
    params->ispInitParams.maxIntegrationLine = 0;	// will be calculated in ISP side using current fps;
    params->ispInitParams.minGain            = 1.0;
    params->ispInitParams.maxGain            = AR2020_DRVP_MAX_GAIN;
    params->ispInitParams.minFps             = 3;
    params->ispInitParams.maxFps             = 90;
    params->ispInitParams.bayerFormat        = GEN_SENSOR_DRVG_BAYER_GRBG;
	params->ispInitParams.aecMaxIntegrationTime  = 0;
    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_open
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
    ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
    INT32                              sensorInstanceId;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GPIO_DRVG_gpioSetValParamsT         gpioParams;
    static unsigned int master=0;

    FIX_UNUSED_PARAM_WARN(deviceId);
    sensorInstanceId = AR2020_DRVP_findFreeSlot();

    if (sensorInstanceId > -1)///////////TBD check if open sensor was init before using , 2020 is opened twice , once per each path (4 lane issue)
    {
        deviceDescriptorP = &AR2020_DRVP_deviceDesc[sensorInstanceId];
        //LOGG_PRINT(LOG_INFO_E,NULL,"open sensor 2020 deviceDescriptorP=%p\n",deviceDescriptorP);
        deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
        if ((pOpenParams->sensorSelect%2) == 0)
            deviceDescriptorP->sensorAddress   = AR2020_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
        else deviceDescriptorP->sensorAddress   = AR2020_DRVP_I2C_SLAVE_ADDRESS;
        deviceDescriptorP->ioctlFuncList   = AR2020_DRVP_ioctlFuncList;
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

static ERRG_codeE AR2021_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
    ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
    INT32                              sensorInstanceId;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GPIO_DRVG_gpioSetValParamsT         gpioParams;
    static unsigned int master=0;

    FIX_UNUSED_PARAM_WARN(deviceId);
    sensorInstanceId = AR2021_DRVP_findFreeSlot();

    if (sensorInstanceId > -1)
    {
        deviceDescriptorP = &AR2021_DRVP_deviceDesc[sensorInstanceId];
        //LOGG_PRINT(LOG_INFO_E,NULL,"open sensor 2021 deviceDescriptorP=%p\n",deviceDescriptorP);
        deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
        if ((pOpenParams->sensorSelect%2) == 0)
            deviceDescriptorP->sensorAddress   = AR2020_DRVP_I2C_MASTER_ADDRESS; //Device address at reset. will be changed later on
        else deviceDescriptorP->sensorAddress   = AR2020_DRVP_I2C_SLAVE_ADDRESS;
        deviceDescriptorP->ioctlFuncList   = AR2020_DRVP_ioctlFuncList;
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
        //temp setting 2021 crop values:
        deviceDescriptorP->max_crop_x=2815;
        deviceDescriptorP->max_crop_y=2111;
        deviceDescriptorP->cropWindowLimitStatus=1;
        printf("setting to 2021 : deviceDescriptorP->cropWindowLimitStatus=1 \n");
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
 *  Function Name: AR2020_DRVP_resetSensors
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ***************************************************************************/
static ERRG_codeE AR2020_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    FIX_UNUSED_PARAM_WARN(pParams);
    FIX_UNUSED_PARAM_WARN(handle);
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  AR2020_DRVP_SOFTWARE_RESET_ADDRESS, 0x1, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);

    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_power_down
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

    //Turn off the sensor
    retVal = AR2020_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_power_up
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_powerup(IO_HANDLE handle, void *pParams)
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
    OS_LYRG_usleep(AR2020_DRVP_RESET_SLEEP_TIME);

    //read chipID
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR2020_DRVP_SC_CHIP_ID_LOW_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    if (config.accessRegParams.data != AR2020_DRVP_SC_CHIP_ID)
        return SENSOR__ERR_UNEXPECTED;

    LOGG_PRINT(LOG_INFO_E,NULL,("power up (type = %d, address = 0x%x, handle = 0x%x),gpio %d, CHIP ID 0x%x\n"),
            deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle, gpioParams.gpioNum, config.accessRegParams.data);

    return retVal;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_changeSensorAddress
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT8 address = *(UINT8*)pParams;

    FIX_UNUSED_PARAM_WARN(pParams);
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    LOGG_PRINT(LOG_INFO_E,NULL,"change address sensor (type = %d, address = 0x%x -> 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,address,handle);

    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_initSensor
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    FIX_UNUSED_PARAM_WARN(handle);
    LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
    return retVal;
}
/**
 * @brief Finds a register value within a sensor preset table
 * 
 *
 * @param sensorTablePtr Pointer to a register table
 * @param regCount Number of registers in the register table 
 * @param registerAddress Register address to look for
 * @param value Pointer which is set to the value of the register when a match is found
 * @return Returns an error code
 */
static ERRG_codeE AR2020_get16BitRegValueFromSensorTable( const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr,UINT16 regCount,UINT32 registerAddress, UINT16 *value)
{
    for(int i =0;i < regCount; i++)
    {
        if(sensorTablePtr[i].regAdd == registerAddress)
        {
            *value = (sensorTablePtr[i].data & 0xFFFF);
            return INU_SENSOR__RET_SUCCESS;
        }
        
    }
    /*We cannot find this register address within sensorTablePtr*/
	LOGG_PRINT(LOG_INFO_E,NULL,"We cannot find this register address within sensorTablePtr\n");
    return INU_SENSOR__ERR_UNEXPECTED;
}
/**
 * @brief Gets the Crop window limits to use based off the register table being used
 * 
 *
 * @param deviceDescriptorP GEN_SENSOR_DRVG_specificDeviceDescT pointer
 * @param sensorTablePtr LUT which contains register addresses and values to use
 * @param regCount Number of registers in the sensorTablePtr
 * @return 
 */
static ERRG_codeE AR2020_getCropWindowLimits( GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP, 
        const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr,    UINT16 regCount)
{
    UINT16 crop_x_start, crop_x_end, max_crop_x, crop_x_len, crop_y_start, crop_y_end, max_crop_y, crop_y_len;
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

    retVal = AR2020_get16BitRegValueFromSensorTable(sensorTablePtr,regCount,X_ADDR_START,&crop_x_start);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    retVal = AR2020_get16BitRegValueFromSensorTable(sensorTablePtr,regCount,X_ADDR_END,&crop_x_end);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    retVal = AR2020_get16BitRegValueFromSensorTable(sensorTablePtr,regCount,X_OUTPUT_SIZE2,&crop_x_len);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    max_crop_x = crop_x_end - crop_x_start - crop_x_len;
    retVal = AR2020_get16BitRegValueFromSensorTable(sensorTablePtr,regCount,Y_ADDR_START,&crop_y_start);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    retVal = AR2020_get16BitRegValueFromSensorTable(sensorTablePtr,regCount,Y_ADDR_END,&crop_y_end);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    retVal = AR2020_get16BitRegValueFromSensorTable(sensorTablePtr,regCount,Y_OUTPUT_SIZE2,&crop_y_len);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    max_crop_y = crop_y_end - crop_y_start - crop_y_len;
    GEN_SENSOR_DRVG_specificDeviceDescT *sensorHandle = (GEN_SENSOR_DRVG_specificDeviceDescT *) deviceDescriptorP->sensorHandle;
    /*Max_Crop_X is 1 less than expected according to the requirements*/
    deviceDescriptorP->max_crop_x = max_crop_x;
    deviceDescriptorP->max_crop_y = max_crop_y;
    deviceDescriptorP->cropWindowLimitStatus = GEN_SENSOR_DRVG_cropWindowCalculated;
    LOGG_PRINT(LOG_INFO_E,NULL,"Handle:%px Crop window limits found to be %lu,%lu \n",deviceDescriptorP, deviceDescriptorP->max_crop_x , deviceDescriptorP->max_crop_y);//make debug
    return retVal;
}
/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_loadPresetTable
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
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
        sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_4LANE_1024x1024_90_table;
        regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_4LANE_1024x1024_90_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
    }
    else
    {
        if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
        {
            sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_4LANE_1120x840_544x408_table;//AR2020_CFG_TBLG_MIPI_24M_4LANE_1024x1024_1024x948_table;
            regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_4LANE_1120x840_544x408_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
#ifdef HELSINKI_MULTI_AR2020
            ISP_MNGRG_setImgType(0);
#endif
        }
        else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
        {
            sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_4LANE_832x608_832x608_table;//AR2020_CFG_TBLG_MIPI_24M_4LANE_1120x840_544x408_table;
            regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_4LANE_832x608_832x608_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
#ifdef HELSINKI_MULTI_AR2020
            ISP_MNGRG_setImgType(3);
            HELSINKI_changeVSTAFFsgCnt(75);
            params->loadTableParams.frameRate = 75;
#endif
        }
        else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_USER_DEF_E)
        {
            sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_4LANE_608x448_608x448_table;
            regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_4LANE_608x448_608x448_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
#ifdef HELSINKI_MULTI_AR2020
            ISP_MNGRG_setImgType(2);
#endif
        }
        else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_FULL_HD_E)
        {
            sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_4LANE_800x736_672x608_table;
            regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_4LANE_800x736_672x608_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
#ifdef HELSINKI_MULTI_AR2020
            HELSINKI_changeVSTAFFsgCnt(75);
            params->loadTableParams.frameRate = 75;
            ISP_MNGRG_setImgType(1);
#endif
        }
        else
        {
            sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_4LANE_1920x2700_30_table;
            regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_4LANE_1920x2700_30_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
        }
    }


    if (handleP)
    {
        retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
        if (ERRG_SUCCEEDED(retVal))
        {
            GEN_SENSOR_DRVG_sensorParametersT   config;
            config.accessRegParams.data = 0;
            GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
            GEN_SENSOR_DRVG_ERROR_TEST(retVal);
            AR2020_DRVP_lineLengthPclk = config.accessRegParams.data & 0xFFFF;

            GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_TIMING_X_OUTPUT_SIZES_LOW_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
            GEN_SENSOR_DRVG_ERROR_TEST(retVal);
            AR2020_DRVP_x_output_size = config.accessRegParams.data & 0xFFFF;

            expParams.context = INU_DEFSG_SENSOR_CONTEX_A;
            AR2020_DRVP_getExposureTime(handle,&expParams);
            //AR2020_DRVP_setStrobeDuration(deviceDescriptorP,expParams.exposureTime);
            AR2020_DRVP_enStrobe(deviceDescriptorP);
            AR2020_getCropWindowLimits(deviceDescriptorP,sensorTablePtr,regNum);
        }
        #if 0
        regNum = sizeof(AR2020_GRR_TABLE)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
        retVal = GEN_SENSOR_DRVG_regTableLoad(handleP,(GEN_SENSOR_DRVG_regTblParamsT *)AR2020_GRR_TABLE,regNum);
        #endif
    }
    else
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Error loading present table\n");
    }

    return retVal;
}



/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setExposureMode
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);
    return SENSOR__RET_SUCCESS;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setImgOffsets
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);
    return SENSOR__RET_SUCCESS;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_getAvgBrighness
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);
    return SENSOR__RET_SUCCESS;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setOutFormat
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);
    return SENSOR__RET_SUCCESS;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setPowerFreq
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);
    return SENSOR__RET_SUCCESS;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setStrobe
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
    GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

    retVal = AR2020_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_syncSensors
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_configSensors(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    UINT16                              sysClk;
    (void)pParams;(void)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    if (params->triggerModeParams.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
    {
        IO_HANDLE *handleP;
        const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr;
        UINT16 regNum;

        handleP = (IO_HANDLE*) deviceDescriptorP;

        params->triggerModeParams.isTriggerSupported = 1;
        //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  AR2020_DRVP_TRIGGER_MODE_ADDRESS, 0x3, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        sensorTablePtr = AR2020_CFG_TBLG_MIPI_24M_trigger_table;
        regNum = sizeof(AR2020_CFG_TBLG_MIPI_24M_trigger_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);

        GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
        usleep(10000);

        sensorTablePtr = AR2020_DigitalTestDelayed;
        regNum = sizeof(AR2020_DigitalTestDelayed)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);

        GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);

        //GME_DRVG_modifyFTRIGMode(IAE_DRVG_FSG_CNT_1_E,TRIGGER_MANAGER );
        //GME_DRVG_modifyFTRIGMode(IAE_DRVG_FSG_CNT_2_E,TRIGGER_MANAGER);
    }
    else
    {
        params->triggerModeParams.isTriggerSupported = 0;
    }
    LOGG_PRINT(LOG_DEBUG_E, NULL,"address %x params->triggerModeParams.isTriggerSupported %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);
    params->triggerModeParams.gpioBitmap |= (((UINT64)1) << deviceDescriptorP->fsinGpio);
    params->triggerModeParams.pulseTime = 300;

    return retVal;

}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_stopSensors
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE AR2020_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    FIX_UNUSED_PARAM_WARN(pParams);
    //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,  AR2020_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x400, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_startSensors
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ***************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE AR2020_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    LOGG_PRINT(LOG_DEBUG_E, NULL,"address %x params->triggerModeParams.isTriggerSupported %x\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);
#if 0
    //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR2020_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0x902, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle,   AR2020_DRVP_RESET_AND_MISC_CONTROL_ADDRESS, 0xd0a, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    //GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, handle,   0x0100, 0x1, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
    //In trigger mode start and stop should be implemented with fsin gpio
    //AR2020_DRVP_configGpio(deviceDescriptorP->fsinGpio, GPIO_DRVG_GPIO_STATE_SET_E);
    AR2020_DRVG_PD50LE_init(handle);

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
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT16                              pix_clk;
    UINT16                              sysClk;
    UINT16                              frameLengthLine;
    UINT16                              numExpLines;

    config.accessRegParams.data = 0;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR2020_DRVP_FRAME_LENGTH_LINES_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    frameLengthLine = config.accessRegParams.data & 0xFFFF;

    config.accessRegParams.data = 0;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    numExpLines = config.accessRegParams.data & 0xFFFF;

    pix_clk = AR2020_DRVP_getPixClk(handle);

    sysClk = AR2020_DRVP_getSysClk(handle);
    if (ERRG_SUCCEEDED(retVal) && frameLengthLine * params->setFrameRateParams.frameRate)
    {
        if(frameLengthLine > numExpLines)
            AR2020_DRVP_lineLengthPclk = (int) pix_clk * 8 * 1000000 / ((int)frameLengthLine * params->setFrameRateParams.frameRate);
        else
            AR2020_DRVP_lineLengthPclk = (int) pix_clk * 8 * 1000000 / ((int)numExpLines * params->setFrameRateParams.frameRate);

        LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. AR2020_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,AR2020_DRVP_lineLengthPclk,frameLengthLine,sysClk);
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS, AR2020_DRVP_lineLengthPclk & 0xffff, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    }
#if 0
    sysClk = AR2020_DRVP_getSysClk(handle);
    if (ERRG_SUCCEEDED(retVal) && AR2020_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate)
    {
        //frameLengthLine = (1000000 * sysClk) / (AR2020_DRVP_lineLengthPclk * params->setFrameRateParams.frameRate);

        frameLengthLine = ((0x600*25)/params->setFrameRateParams.frameRate);
        //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, AR2020_DRVP_FRAME_LENGTH_LINES_ADDRESS, frameLengthLine, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        AR2020_DRVP_vts = frameLengthLine;
        LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. AR2020_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, sysClk = %d\n",params->setFrameRateParams.frameRate,AR2020_DRVP_lineLengthPclk,frameLengthLine,sysClk);

    }
#endif
    return retVal;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setExposureTime
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT32                              numExpLines,maxExpLines, pixel_line;
    UINT16                              sysClk = 0, pix_clk = 0;

    if(deviceDescriptorP)
    {
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GROUP_ACCESS_ADDRESS, 1, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
#if 1
        pix_clk = AR2020_DRVP_getPixClk(handle);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        /*An optimization to reduce I2C*/
        if(deviceDescriptorP->exposureState.lineLength == 0)
        {
            
            GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
            GEN_SENSOR_DRVG_ERROR_TEST(retVal);
            pixel_line = config.accessRegParams.data & 0xFFFF;
            deviceDescriptorP->exposureState.lineLength = pixel_line;

        }
        else
        {
            pixel_line = deviceDescriptorP->exposureState.lineLength;
        }

        if ((pixel_line != 0) && (pix_clk != 0))
        {
            numExpLines = params->exposureTime * pix_clk * 8 / pixel_line ; /// 3000 * 176 * 8 / 12
            if(numExpLines > 0)
            {
                LOGG_PRINT(LOG_DEBUG_E,NULL,"sen exposure. numLines = %d, exposureTime = %d, pixel_line = %d,pix_clk = %d, context=%d\n", numExpLines, params->exposureTime, pixel_line, pix_clk, params->context);
                GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS, numExpLines, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"0 exposure trying to be written \n");
            }
        }
        else
        {
            numExpLines = 0;
            LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR set exposure. numLines = %d, exposureTime = %d, pixel_line = %d,pix_clk = %d, context=%d\n", numExpLines, params->exposureTime, pixel_line, pix_clk, params->context);
            return ERR_UNEXPECTED;
        }
#else
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS, params->exposureTime, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
#endif
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GROUP_ACCESS_ADDRESS, 0, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"set exposure with bad handle!\n");
        return ERR_UNEXPECTED;
    }

    return SENSOR__RET_SUCCESS;

}
static ERRG_codeE AR2020_DRVP_setExposureTimeAndGain(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_exposureTimeCAndGainfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCAndGainfgT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT32                              numExpLines,maxExpLines, pixel_line;
    UINT16                              sysClk = 0, pix_clk = 0;

    if(deviceDescriptorP)
    {

        UINT32                              analogGainIndex  = 0;
        UINT32                              i;
        if (params->gain.analogGain > AR2020_DRVP_MAX_GAIN)
        {
            LOGG_PRINT(LOG_ERROR_E,NULL,"set gain with bad values. Gain:%f\n", params->gain.analogGain);
            return ERR_UNEXPECTED;
        }

        for (i = 0; i < (sizeof(AR2020_DRVP_gainLut)/sizeof(float)); i++)
        {
            if (AR2020_DRVP_gainLut[i] >= params->gain.analogGain)
            {
                analogGainIndex = i;
                break;
            }
        }

  
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GROUP_ACCESS_ADDRESS, 1, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
#if 1
        pix_clk = AR2020_DRVP_getPixClk(handle);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        pixel_line = config.accessRegParams.data & 0xFFFF;

        if ((pixel_line != 0) && (pix_clk != 0))
        {
            numExpLines = params->exposureTime * pix_clk * 8 / pixel_line ; /// 3000 * 176 * 8 / 12
            if(numExpLines > 0)
            {
                LOGG_PRINT(LOG_DEBUG_E,NULL,"sen exposure. numLines = %d, exposureTime = %d, pixel_line = %d,pix_clk = %d, context=%d\n", numExpLines, params->exposureTime, pixel_line, pix_clk, params->context);
                GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS, numExpLines, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
                /*Write gain whilst we have the group hold*/
                GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GLOBAL_GAIN_ADDRESS, analogGainIndex | (analogGainIndex << 8), AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
                GEN_SENSOR_DRVG_ERROR_TEST(retVal);

                //  This use−case will allow the user to implement a group of
                // registers at the end of an uninterrupted frame. If the time
                // delay between writing the group of registers and the end of
                // the frame is unacceptable, the restart_ctrl (R0x301A[1])
                // function can be used to interrupt the current frame.
                // Using restart_ctrl with grouped_parameter_hold, the
                // restart_ctrl function will end the current frame and begin a
                // new frame. After the new frame has begun, the restart_ctrl
                // bit will reset from “1” to “0”. The restart_ctrl can be used
                // with the grouped_parameter_hold feature to hold the
                // implementation of a number of registers when interrupting
                // a frame. The recommended syntax for using the
                // GROUPED_PARAMETER_HOLD and restart_ctrl
                // functions together is shown below:
                // GROUPED_PARAMETER_HOLD 1
                // RESTART_CTRL 1
                // GROUPED_PARAMETER_HOLD 0
                #if 0
                const uint32_t restart_ctrl_val = AR2020_RESET_CONTROL_VALUE | (1<<1); /*Enable RESTART_CTRL*/
                GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_RESET_AND_MISC_CONTROL_ADDRESS,restart_ctrl_val , AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
                #endif
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"0 exposure trying to be written \n");
            }
        }
        else
        {
            numExpLines = 0;
            LOGG_PRINT(LOG_ERROR_E,NULL,"ERROR set exposure. numLines = %d, exposureTime = %d, pixel_line = %d,pix_clk = %d, context=%d\n", numExpLines, params->exposureTime, pixel_line, pix_clk, params->context);
            return ERR_UNEXPECTED;
        }
#else
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS, params->exposureTime, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
#endif
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GROUP_ACCESS_ADDRESS, 0, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"set exposure with bad handle!\n");
        return ERR_UNEXPECTED;
    }

    return SENSOR__RET_SUCCESS;

}




/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setExposureTime
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT32                              numExpLines = 0, pixel_line;
    UINT16                              sysClk, pxl_clk;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    pixel_line = config.accessRegParams.data & 0xFFFF;

    params->exposureTime = 0;

    pxl_clk = AR2020_DRVP_getPixClk(handle);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_COARSE_INTEGRATION_TIME_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    numExpLines = config.accessRegParams.data & 0xFFFF;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_COARSE_INTEGRATION_TIME_HI_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    numExpLines |= (config.accessRegParams.data & 0xFFFF) << 16;

    //GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_LINE_LENGTH_PCK_ADDRESS, AR2020_DRVP_2_BYTE, retVal);
    //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    if (pxl_clk != 0)
    {
        params->exposureTime = (numExpLines * pixel_line) / (pxl_clk * 8);
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E,ERR_UNEXPECTED,"Failed to get exposure. context = %d, num_lines = %d, pixel_line = %d, pxl_clk = %d, exposure = %d\n",params->context,numExpLines,pixel_line,pxl_clk,params->exposureTime);
        return ERR_UNEXPECTED;
    }


    return retVal;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setGain
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_gainCfgT            *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT32                              analogGainIndex  = 0;
    UINT32                              i;

    if (params->analogGain > AR2020_DRVP_MAX_GAIN)
    {
        LOGG_PRINT(LOG_ERROR_E,NULL,"set gain with bad values. Gain:%f\n", params->analogGain);
        return ERR_UNEXPECTED;
    }

    for (i = 0; i < (sizeof(AR2020_DRVP_gainLut)/sizeof(float)); i++)
    {
        if (AR2020_DRVP_gainLut[i] >= params->analogGain)
        {
            analogGainIndex = i;
            break;
        }
	}

    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GLOBAL_GAIN_ADDRESS, analogGainIndex | (analogGainIndex << 8), AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#if 0
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_ANALOG_GAIN_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    config.accessRegParams.data &= ~(AR2020_DRVP_ANALOG_GAIN_MASK << AR2020_DRVP_ANALOG_GAIN_START_BIT); //reset the gain bits
    config.accessRegParams.data |= (analogGain & AR2020_DRVP_ANALOG_GAIN_MASK) << AR2020_DRVP_ANALOG_GAIN_START_BIT; //set new value
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_ANALOG_GAIN_ADDRESS, config.accessRegParams.data, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
    LOGG_PRINT(LOG_DEBUG_E,NULL,"set gain.Address 0x%x, Analog Gain = %f, digital = %d\n", deviceDescriptorP->sensorAddress, params->analogGain, analogGainIndex);

    return SENSOR__RET_SUCCESS;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_getGain
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT config;
    GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_GLOBAL_GAIN_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    params->analogGain = AR2020_DRVP_gainLut[config.accessRegParams.data & 0x7f];
    params->digitalGain = 1;
    LOGG_PRINT(LOG_DEBUG_E,NULL,"context = %d, analog = %d, digital = %d gain_data = 0x%x\n", params->context,params->analogGain,params->digitalGain,config.accessRegParams.data);

    return retVal;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_getDeviceId
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
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
 *  Function Name: AR2020_DRVP_findFreeSlot
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/

static INT32 AR2020_DRVP_findFreeSlot()
{
    UINT32                              sensorInstanceId;

    for (sensorInstanceId = 0; sensorInstanceId < sizeof(AR2020_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
    {
        if(AR2020_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
        {
            return sensorInstanceId;
        }
    }
    return -1;
}

static INT32 AR2021_DRVP_findFreeSlot()
{
    UINT32                              sensorInstanceId;

    for (sensorInstanceId = 0; sensorInstanceId < sizeof(AR2021_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
    {
        if(AR2021_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
        {
            return sensorInstanceId;
        }
    }
    return -1;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_mirrorFlip
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_E) || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
    {
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_IMAGE_ORIENTATION_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        config.accessRegParams.data |= 0x1 << 0x0;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_IMAGE_ORIENTATION_ADDRESS,config.accessRegParams.data, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    }
    if ((params->orientationParams.orientation == INU_DEFSG_SENSOR_FLIP_E)  || ( params->orientationParams.orientation == INU_DEFSG_SENSOR_MIRROR_FLIP_E))
    {
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_IMAGE_ORIENTATION_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        config.accessRegParams.data |= 0x1 << 0x1;

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_IMAGE_ORIENTATION_ADDRESS,config.accessRegParams.data, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    }

    //GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);

    return retVal;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_setStrobeDuration
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;

    return retVal;
}


/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_enStrobe
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_enStrobe(IO_HANDLE handle)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT16                              val;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_STROBE_EN_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    LOGG_PRINT(LOG_INFO_E,NULL,"strobe reg = 0x%x)\n",config.accessRegParams.data);

    val = config.accessRegParams.data & 0xFFFF;

    //GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP,   AR2020_DRVP_STROBE_EN_ADDRESS, val | 0x20, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    //GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    return retVal;
}

static ERRG_codeE AR2020_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
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
 *  Function Name: AR2020_DRVP_trigger
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    (void)handle;(void)pParams;
    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_close_gpio
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_close_gpio(UINT32 gpioNum)
{
    ERRG_codeE  retCode = INIT__RET_SUCCESS;
    GPIO_DRVG_gpioOpenParamsT     params;

    params.gpioNum =gpioNum;
    retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params);
    return retCode;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_close
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_close(IO_HANDLE handle)
{
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    AR2020_DRVP_powerdown(handle,NULL);
    deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
    AR2020_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
    LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
    return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: AR2020_DRVP_getTemperature
*
*  Description: Get the temperature of sensor
*
*  Inputs: N/A
*
*  Outputs: temperature - current temperature in Celcius
*
*  Returns: SENSOR__RET_SUCCESS if success
*           otherwise error code is returned
*
*  Context: AR2020 sensor driver
*
****************************************************************************/
static ERRG_codeE AR2020_DRVP_getTemperature(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_temperatureParamsT    *params=(GEN_SENSOR_DRVG_temperatureParamsT *)pParams;
    INT16 ic8, id2;

    //Read TEMPERATURE
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_TPM_SENSOR_ADC_OUTPUT_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    ic8 = (INT16)config.accessRegParams.data;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, AR2020_DRVP_TPM_SENSOR_CALIB_ADC1_ADDRESS, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    id2 = (INT16)config.accessRegParams.data;

    params->temperature = (INT32)(60 + 0.2505 * (ic8 - id2));

    return retVal;
}

/****************************************************************************
 *
 *  Function Name: AR2020_DRVP_ioctl
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
static ERRG_codeE AR2020_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
    return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}

#define PD50LE_MODE             0x0
#define PD50LE_DAC_MSB          0x2
#define PD50LE_DAC_LSB          0x3
#define PD50LE_STATUS           0x4
#define PD50LE_CHIP_ID          0x7

#define PD50LE_CHIP_ID_VAL      0xA3

#define PD50LE_MAX_DAC          (875)
#define PD50LE_MIN_DAC          (-183)

ERRG_codeE AR2020_DRVG_PD50LE_read(GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP, UINT8 reg, UINT8* val)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    deviceDescriptorP->sensorAddress = AR2020_DRVP_I2C_PD50LE_ADDRESS;

    GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, deviceDescriptorP, reg, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    *val = (config.accessRegParams.data & 0xff);

    LOGG_PRINT(LOG_DEBUG_E, NULL, "READ from register addr: 0x%x value: 0x%x\n", reg, *val);

    return retVal;
}

ERRG_codeE AR2020_DRVG_PD50LE_write(GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP, UINT8 reg, UINT8 val)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    deviceDescriptorP->sensorAddress = AR2020_DRVP_I2C_PD50LE_ADDRESS;

    GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, deviceDescriptorP, reg, val, AR2020_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    LOGG_PRINT(LOG_DEBUG_E, NULL, "WRITE to register addr: 0x%x value: 0x%x\n", reg, val);

    return retVal;
}

ERRG_codeE AR2020_DRVG_PD50LE_writeDac(GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP, INT16 val)
{
    ERRG_codeE      retVal = SENSOR__RET_SUCCESS;
    unsigned char   msb, lsb;
    UINT16          dac_val;

    if (val > PD50LE_MAX_DAC)
    {
        LOGG_PRINT(LOG_WARN_E,NULL,"value (%d) exceeds maximal allowd (%d), set maximal value\n", (INT16)val, PD50LE_MAX_DAC);
        val = PD50LE_MAX_DAC;
    }
    else if (val < PD50LE_MIN_DAC)
    {
        LOGG_PRINT(LOG_WARN_E,NULL,"value (%d) is below the minimal allowd (%d), set minimal value\n", (INT16)val, PD50LE_MIN_DAC);
        val = PD50LE_MIN_DAC;
    }

    dac_val = (UINT16)abs(val);

    if (val > 0)
    {
        dac_val |= 0x8000;
    }

    msb = (dac_val & 0xff00) >> 8;
    lsb = dac_val & 0xff;

    AR2020_DRVG_PD50LE_write(deviceDescriptorP, PD50LE_DAC_MSB, msb);
    AR2020_DRVG_PD50LE_write(deviceDescriptorP, PD50LE_DAC_LSB, lsb);

    OS_LYRG_usleep(5000);

    return retVal;
}

static ERRG_codeE AR2020_DRVG_PD50LE_getLensDac(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_focusParamsT        *params = (GEN_SENSOR_DRVG_focusParamsT*) pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;
    UINT16 dac_val;
    UINT8  lsb, msb;

    params->dac = 0;
    retVal = AR2020_DRVG_PD50LE_read(deviceDescriptorP, PD50LE_DAC_LSB, &lsb);
    if(ERRG_SUCCEEDED(retVal))
    {
        retVal = AR2020_DRVG_PD50LE_read(deviceDescriptorP, PD50LE_DAC_MSB, &msb);
        if(ERRG_SUCCEEDED(retVal))
        {
            dac_val = (UINT16)abs((UINT16)(msb & ~0x80) << 8) | ((UINT16)lsb);
            params->dac = (msb & 0x80) ? (INT16)dac_val : (INT16)(0 - dac_val);   // if bit 7 in msb is set the polarity is positive
            params->minDac = PD50LE_MIN_DAC;
            params->maxDac = PD50LE_MAX_DAC;
            params->chipId = PD50LE_CHIP_ID_VAL;
        }
    }
    return retVal;
}

static ERRG_codeE AR2020_DRVG_PD50LE_setLensDac(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_focusParamsT        *params = (GEN_SENSOR_DRVG_focusParamsT*) pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;

    retVal = AR2020_DRVG_PD50LE_writeDac(deviceDescriptorP, (INT16)params->dac);
    if (ERRG_FAILED(retVal))
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "failed to set lens dac\n");
    }
    return retVal;
}

static ERRG_codeE AR2020_DRVG_PD50LE_init(IO_HANDLE handle)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT deviceDescriptorP, *deviceDP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT8                               chipId;

    deviceDescriptorP.deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
    deviceDescriptorP.ioctlFuncList = NULL;
    deviceDescriptorP.i2cInstanceId = deviceDP->i2cInstanceId;
    deviceDescriptorP.i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
    deviceDescriptorP.sensorModel   = INU_DEFSG_SENSOR_MODEL_AR_2020_E;
    deviceDescriptorP.sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
    deviceDescriptorP.sensorHandle  = NULL;

    retVal = AR2020_DRVG_PD50LE_read(&deviceDescriptorP, PD50LE_CHIP_ID, &chipId);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    if (chipId != PD50LE_CHIP_ID_VAL)
    {
        LOGG_PRINT(LOG_WARN_E,NULL,"AR2020 sensor has no PD50LE device\n");
        return retVal;
    }

    OS_LYRG_usleep(10000);

    AR2020_DRVG_PD50LE_write(&deviceDescriptorP, PD50LE_MODE, 1); ////////////////we dont see this value? addrs 0?
    AR2020_DRVG_PD50LE_write(&deviceDescriptorP, PD50LE_STATUS, 0);

    AR2020_DRVG_PD50LE_writeDac(&deviceDescriptorP, 0);
    OS_LYRG_usleep(10000);
    AR2020_DRVG_PD50LE_writeDac(&deviceDescriptorP, 250); //733
    OS_LYRG_usleep(10000);
    AR2020_DRVG_PD50LE_writeDac(&deviceDescriptorP, 0); //733
    OS_LYRG_usleep(10000);
    AR2020_DRVG_PD50LE_writeDac(&deviceDescriptorP, 250);
    OS_LYRG_usleep(10000);
    //AR2020_DRVG_PD50LE_writeDac(&deviceDescriptorP, 0);

    return retVal;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
 *
 *  Function Name: AR2020_DRVG_init
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
ERRG_codeE AR2020_DRVG_init(IO_PALG_apiCommandT *palP)
{
    UINT32                               sensorInstanceId;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

    if(AR2020_DRVP_isDrvInitialized == FALSE)
    {
        for (sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)
        {
            deviceDescriptorP = &AR2020_DRVP_deviceDesc[sensorInstanceId];
            memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
            deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
            deviceDescriptorP->ioctlFuncList = NULL;
            deviceDescriptorP->sensorAddress = 0;
            deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
            deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
            deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_AR_2020_E;
            deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
            deviceDescriptorP->sensorHandle  = NULL;
            deviceDescriptorP->powerGpioMaster = GPIO_DRVG_GPIO_44_E;
            deviceDescriptorP->fsinGpio      = GPIO_DRVG_GPIO_39_E;
        }
        AR2020_DRVP_isDrvInitialized = TRUE;
    }

    if (palP!=NULL)
    {
        AR2020_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_GET_TEMPERATURE_IOCTL_E] = AR2020_DRVP_getTemperature;
        AR2020_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_GET_FOCUS_PARAMS_IOCTL_E] = AR2020_DRVG_PD50LE_getLensDac;
        AR2020_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_SET_FOCUS_PARAMS_IOCTL_E] = AR2020_DRVG_PD50LE_setLensDac;
        AR2020_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE_IOCTL_E] = AR2020_DRVP_setExposureTimeAndGain;
        palP->close =  (IO_PALG_closeT) &AR2020_DRVP_close;
        palP->ioctl =  (IO_PALG_ioctlT) &AR2020_DRVP_ioctl;
        palP->open  =  (IO_PALG_openT)  &AR2020_DRVP_open;
    }
    else
    {
        return SENSOR__ERR_NULL_PTR;
    }

    return SENSOR__RET_SUCCESS;
}

/****************************************************************************
 *
 *  Function Name: AR2021_DRVG_init
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: AR2020 sensor driver
 *
 ****************************************************************************/
ERRG_codeE AR2021_DRVG_init(IO_PALG_apiCommandT *palP)
{
    UINT32                               sensorInstanceId;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

    if(AR2021_DRVP_isDrvInitialized == FALSE)
    {
        for (sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)
        {
            deviceDescriptorP = &AR2021_DRVP_deviceDesc[sensorInstanceId];
            memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
            deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
            deviceDescriptorP->ioctlFuncList = NULL;
            deviceDescriptorP->sensorAddress = 0;
            deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
            deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
            deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_AR_2021_E;
            deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
            deviceDescriptorP->sensorHandle  = NULL;
        }
        /*
        //////////////AR2020_getCropWindowLimits(deviceDescriptorP,sensorTablePtr,regNum);
        deviceDescriptorP->max_crop_x=2815;
        deviceDescriptorP->max_crop_y=2111;
        deviceDescriptorP->cropWindowLimitStatus=1;
        printf("!!!!!! setting to 2021 : deviceDescriptorP->cropWindowLimitStatus=1 \n");
        printf("!!!!!! 2021 p=%0 \n",deviceDescriptorP);*/
        AR2021_DRVP_isDrvInitialized = TRUE;
    }

    if (palP!=NULL)
    {
        palP->close =  (IO_PALG_closeT) &AR2020_DRVP_close;
        palP->ioctl =  (IO_PALG_ioctlT) &AR2020_DRVP_ioctl;
        palP->open  =  (IO_PALG_openT)  &AR2021_DRVP_open;
    }
    else
    {
        return SENSOR__ERR_NULL_PTR;
    }

    return SENSOR__RET_SUCCESS;
}
static ERRG_codeE AR2020_DRVP_setCropWindow(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    if(deviceDescriptorP)
    {
        
        if(deviceDescriptorP->cropWindowLimitStatus == GEN_SENSOR_DRVG_cropWindowNotCalculated)
        {
            LOGG_PRINT(LOG_ERROR_E,NULL,"%p Crop window maximum size isn't known, cannot set the crop window \n", deviceDescriptorP);
            return SENSOR__ERR_INVALID_ARGS;
        }
        /*Check that the crop window is valid*/
        if(params->cropWindow.startX <= deviceDescriptorP->max_crop_x && params->cropWindow.startY<= deviceDescriptorP->max_crop_y )
            {
                /*Requirement from On Semi FAE, StartX % 4 == 0 and Start Y % 2 == 0*/
                if(((params->cropWindow.startX % 4) == 0) && ((params->cropWindow.startY % 2) == 0))
                {
                    const UINT32 crop_x = params->cropWindow.startX;///in my imp do I need the const?
                    const UINT32 crop_y = params->cropWindow.startY;
                    LOGG_PRINT(LOG_DEBUG_E,NULL,"Setting crop window for AR2020 to be X,%lu,Y,%lu \n",crop_x,crop_y);
                    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, X_OUTPUT_OFFSET2, crop_x, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
                    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
                    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, Y_OUTPUT_OFFSET2, crop_y, AR2020_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
                    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
                    LOGG_PRINT(LOG_DEBUG_E,NULL,"Finished setting crop window for AR2020 to be X,%lu,Y,%lu \n",crop_x,crop_y );
                }
                else
                {
                    LOGG_PRINT(LOG_ERROR_E,NULL,"Unaligned Crop window update for AR2020:  X,%lu,Y,%lu \n",params->cropWindow.startX,params->cropWindow.startY);
                    return SENSOR__ERR_INVALID_ARGS;
                }
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"Invalid crop window for AR2020: X:%lu,Y:%lu MAX_X:%lu, MAX_Y:%lu \n",params->cropWindow.startX,params->cropWindow.startY,deviceDescriptorP->max_crop_x,deviceDescriptorP->max_crop_y );
                return SENSOR__ERR_INVALID_ARGS;
            }
        return retVal;
    }
    else
    {
        return SENSOR__ERR_INVALID_ARGS;
    }
}

static ERRG_codeE AR2020_DRVP_getCropWindow(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_sensorParametersT    *params= ((GEN_SENSOR_DRVG_sensorParametersT *)pParams);
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    if(deviceDescriptorP)
    {
        #define MAXIMUM_CROP_X 3392 - 544 /*Native resolution - Crop window */
        #define MAXIMUM_CROP_Y 2544 - 408 /* Native resolution - Crop window */
            if(params)
            {
                config.accessRegParams.data = 0;
                GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, X_OUTPUT_OFFSET2,  AR2020_DRVP_SENSOR_ACCESS_2_BYTE,retVal);
                GEN_SENSOR_DRVG_ERROR_TEST(retVal);
                params->cropWindow.startX = ( config.accessRegParams.data ) & 0xFFFF;
                GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, Y_OUTPUT_OFFSET2, AR2020_DRVP_SENSOR_ACCESS_2_BYTE,retVal);
                GEN_SENSOR_DRVG_ERROR_TEST(retVal);
                params->cropWindow.startY= ( config.accessRegParams.data ) & 0xFFFF;
                LOGG_PRINT(LOG_INFO_E,NULL,"Finished getting crop window for AR2020. X,%lu,Y,%lu \n", params->cropWindow.startX, params->cropWindow.startY );//make debug
            }
            else
            {
                LOGG_PRINT(LOG_ERROR_E,NULL,"Crop window pointer invalid \n");
                return SENSOR__ERR_INVALID_ARGS;
            }
        return retVal;
    }
    else
    {
        return SENSOR__ERR_INVALID_ARGS;
    }
}
#ifdef __cplusplus
}
#endif
