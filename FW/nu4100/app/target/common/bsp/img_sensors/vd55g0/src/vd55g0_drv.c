/****************************************************************************
 *
 *   FileName: VD55G0_drv.c
 *
 *   Author:  Leon Lew
 *
 *   Date:
 *
 *   Description: VD55G0 sensor driver
 *
 ****************************************************************************/

#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "io_pal.h"
#include "gen_sensor_drv.h"
#include "vd55g0_drv.h"
#include "vd55g0_cfg_tbl.h"
#include "vd55g0_regs.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "gme_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include <unistd.h>
#include <errno.h>
#include "vd55g0_fw_patch.h"
#include "vd55g0_vt_patch.h"
#include "mipi_drv.h"
/****************************************************************************
 ***************       L O C A L       D E F I N I T I O N S  ***************
 ****************************************************************************/
#define VD55G0_DRVP_SENSOR_ACCESS_1_BYTE        (1)
#define VD55G0_DRVP_SENSOR_ACCESS_2_BYTE        (2)
#define VD55G0_DRVP_SENSOR_ACCESS_3_BYTE        (3)
#define VD55G0_DRVP_SENSOR_ACCESS_4_BYTE        (4)

#define ___constant_swab16(x) ((UINT16)(                         \
        (((UINT16)(x) & (UINT16)0x00ffU) << 8) |                  \
        (((UINT16)(x) & (UINT16)0xff00U) >> 8)))

#define ___constant_swab32(x) ((UINT32)(                         \
        (((UINT32)(x) & (UINT32)0x000000ffUL) << 24) |            \
        (((UINT32)(x) & (UINT32)0x0000ff00UL) <<  8) |            \
        (((UINT32)(x) & (UINT32)0x00ff0000UL) >>  8) |            \
        (((UINT32)(x) & (UINT32)0xff000000UL) >> 24)))

#define BYTE_SWAP32(x)  ___constant_swab32(x)
#define BYTE_SWAP16(x)  ___constant_swab16(x)

#define VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
/****************************************************************************
 ***************      L O C A L         T Y P E D E F S     ***************
 ****************************************************************************/

/***************************************************************************
***************      L O C A L      D E C L A R A T I O N S    *************
****************************************************************************/

static ERRG_codeE VD55G0_DRVP_resetSensors(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_initSensor(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_startSensors(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_stopSensors(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_configSensors(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_getDeviceId(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_loadPresetTable(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_setExposureTime(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_getExposureTime(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setGain(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_getGain(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setExposureMode(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_setImgOffsets(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_getAvgBrighness(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_setOutFormat(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_setPowerFreq(IO_HANDLE handle, void  *setPowerFreqParamsP);
static ERRG_codeE VD55G0_DRVP_setStrobe(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_trigger(IO_HANDLE handle, void *params);
static ERRG_codeE VD55G0_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setFrameRate(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_close_gpio(INU_DEFSG_sensorTypeE   sensorType);

static ERRG_codeE VD55G0_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params);
static ERRG_codeE VD55G0_DRVP_close(IO_HANDLE handle);
static ERRG_codeE VD55G0_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP);
static INT32      VD55G0_DRVP_findFreeSlot();
static ERRG_codeE VD55G0_DRVP_getSensorClks(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_configGpio(UINT32 gpioNumEntry,GPIO_DRVG_gpioStateE state);

static ERRG_codeE VD55G0_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec);
static ERRG_codeE VD55G0_DRVP_enStrobe(IO_HANDLE handle);
static ERRG_codeE VD55G0_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_powerdown(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_powerup(IO_HANDLE handle, void *pParams);

static ERRG_codeE VD55G0_DRVP_getChipID(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_activateTestPattern(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setBayerPattern(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setBLC(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setWB(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_changeResolution(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_exposureCtrl(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_setCropWindow(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_getCropWindow(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_getTemperature(IO_HANDLE handle, void *pParams);
static ERRG_codeE VD55G0_DRVP_unimplementedFunction(IO_HANDLE handle, void *pParams);

/****************************************************************************
 ***************       L O C A L       D A T A              ***************
 ****************************************************************************/
static BOOL                                  VD55G0_DRVP_isDrvInitialized = FALSE;
static GEN_SENSOR_DRVG_specificDeviceDescT   VD55G0_DRVP_deviceDesc[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static GEN_SENSOR_DRVG_ioctlFuncListT        VD55G0_DRVP_ioctlFuncList[GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E] = {
                                                                                                      VD55G0_DRVP_resetSensors,            \
                                                                                                      VD55G0_DRVP_initSensor,              \
                                                                                                      VD55G0_DRVP_configSensors,           \
                                                                                                      VD55G0_DRVP_startSensors,            \
                                                                                                      VD55G0_DRVP_stopSensors,             \
                                                                                                      GEN_SENSOR_DRVG_ioctlAccessReg,      \
                                                                                                      VD55G0_DRVP_getDeviceId,             \
                                                                                                      VD55G0_DRVP_loadPresetTable,         \
                                                                                                      VD55G0_DRVP_setFrameRate,            \
                                                                                                      VD55G0_DRVP_setExposureTime,         \
                                                                                                      VD55G0_DRVP_getExposureTime,         \
                                                                                                      VD55G0_DRVP_setExposureMode,         \
                                                                                                      VD55G0_DRVP_setImgOffsets,           \
                                                                                                      VD55G0_DRVP_getAvgBrighness,         \
                                                                                                      VD55G0_DRVP_setOutFormat,            \
                                                                                                      VD55G0_DRVP_setPowerFreq,            \
                                                                                                      VD55G0_DRVP_setStrobe,               \
                                                                                                      VD55G0_DRVP_getSensorClks,           \
                                                                                                      VD55G0_DRVP_setGain,                 \
                                                                                                      VD55G0_DRVP_getGain,                 \
                                                                                                      VD55G0_DRVP_trigger,                 \
                                                                                                      VD55G0_DRVP_mirrorFlip,              \
                                                                                                      VD55G0_DRVP_powerup ,                \
                                                                                                      VD55G0_DRVP_powerdown ,              \
                                                                                                      VD55G0_DRVP_changeSensorAddress,     \
                                                                                                      VD55G0_DRVP_getChipID,               \
                                                                                                      VD55G0_DRVP_activateTestPattern,     \
                                                                                                      VD55G0_DRVP_setBayerPattern,         \
                                                                                                      VD55G0_DRVP_setBLC,                  \
                                                                                                      VD55G0_DRVP_setWB,                   \
                                                                                                      VD55G0_DRVP_changeResolution,        \
                                                                                                      VD55G0_DRVP_exposureCtrl,            \
                                                                                                      GEN_SENSOR_DRVG_stub,        \
                                                                                                      GEN_SENSOR_DRVG_stub,       \
                                                                                                      GEN_SENSOR_DRVG_stub,       \
                                                                                                      GEN_SENSOR_DRVG_stub,			 \
                                                                                                      GEN_SENSOR_DRVG_stub,			 \
                                                                                                      VD55G0_DRVP_getTemperature };

static UINT16 VD55G0_DRVP_lineLengthPclk;
static UINT16 VD55G0_DRVP_x_output_size;
static UINT16 VD55G0_DRVP_vts;
static UINT32 VD55G0_DRVP_slaveGpios[] = {48};
static UINT32 VD55G0_DRVP_activeInstanceMask = 0;

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S    ***************
 ****************************************************************************/
#ifdef DEBUG
static void dump_sensor_regs(IO_HANDLE handle, UINT32 startAddress, UINT32 endAddress, const char *tips)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (tips) {
        printf("%s\n", tips);
   }
   while (startAddress <= endAddress) {
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, startAddress, 1, retVal);
        printf("\tAddress[%04X]: 0x%02x\n", startAddress, config.accessRegParams.data & 0xFF);
        startAddress++;
   }
   printf("+++++++[leonlew#]Registers dump done++++++++++\n");
}

void VD55G0_setDefaultAddress(IO_HANDLE handle)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    UINT16 new_address = VD55G0_DRVP_I2C_DEFAULT_ADDRESS;

     GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, SS_REG_DEVICE_I2C_CTRL, new_address, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to change sensor address\n");
        return;
    }
    //Issue I2C_ADDR_UPDATE command
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_SW_STBY, COMMAND_I2C_ADDR_UPDATE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

    OS_LYRG_usleep(4000);
    deviceDescriptorP->sensorAddress = new_address;

    LOGG_PRINT(LOG_INFO_E, NULL, "Handle(0x%08x): Sensor address changed to 0x%x\n", (UINT32)handle, new_address);
    //Read Device ID again to see if the new address works
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_DEVICE_MODEL_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
   config.accessRegParams.data = BYTE_SWAP32(config.accessRegParams.data);
   LOGG_PRINT(LOG_INFO_E,NULL,"Read Device Model ID after change sensor's address: 0x%08x\n", config.accessRegParams.data);
}
#endif

static ERRG_codeE VD55G0_DRVP_getExposureMode(IO_HANDLE handle, GEN_SENSOR_DRVG_Exposure_mode_e *expMode, UINT8 contextIndex)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT16 ctxRegAddr = CONTEXT_0_REG_EXP_MODE + CONTEXT_REG_ADDR_SIZE * contextIndex; 

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ctxRegAddr, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

    *expMode = config.accessRegParams.data & 0x3;

    return retVal;
}

static ERRG_codeE VD55G0_DRVP_groupHoldEnable(IO_HANDLE handle, bool enable)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT8   groupHoldEn = enable ? 1 : 0;

    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, DYN_REG_GROUP_PARAM_HOLD, groupHoldEn, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);

    return retVal;
}

static System_FSM_e VD55G0_DRVP_getFSM(IO_HANDLE handle)
{
    ERRG_codeE retVal = SENSOR__ERR_CONFIGUTATION_FAILED;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    config.accessRegParams.data = 0;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_SYSTEM_FSM, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

    return (config.accessRegParams.data & 0xFF);
}

static ERRG_codeE VD55G0_DRVP_wait4FSM(IO_HANDLE handle, System_FSM_e state)
{
    ERRG_codeE retVal = SENSOR__ERR_CONFIGUTATION_FAILED;
    int retry = VD55G0_MAX_RETRY;

    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    while (retry >= 0) {
        config.accessRegParams.data = 0;
        GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_SYSTEM_FSM, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);

        if (config.accessRegParams.data == state) {
            retVal = SENSOR__RET_SUCCESS;
            break;
        }

        OS_LYRG_usleep(1000);
        retry--;
    }

    return retVal;
}

static UINT16 VD55G0_DRVP_getPixClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (!deviceDescriptorP->pixClk)
   {
       /*
        * From the databook, VD55G0's pixel clock is fixed at 150Mhz
        * Since the ST_REG_PIXEL_CLK is not ready when sensor's not streaming
        * Just return 150Mhz
        */
       deviceDescriptorP->pixClk = VD55G0_PIXEL_CLOCK_HZ / 1000000;
   }

   return deviceDescriptorP->pixClk;

}

/* Get sys clock, unit is MHZ */
static UINT16 VD55G0_DRVP_getSysClk(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   if (!deviceDescriptorP->sysClk)
   {
       /*
        * This should be MC_CLK(150Mhz)
        */
       deviceDescriptorP->sysClk = 150;
   }
   return deviceDescriptorP->sysClk;
}

static ERRG_codeE VD55G0_DRVP_getLineTiming(IO_HANDLE handle, UINT32 *pixelClk, UINT32 *lineLength)
{
     ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    *pixelClk = VD55G0_PIXEL_CLOCK_HZ;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, STATIC_REG_LINE_LENGTH, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    GEN_SENSOR_DRVG_ERROR_TEST(retVal);
    *lineLength = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);

    return retVal;
}

static ERRG_codeE VD55G0_DRVP_getMinFrameLength(IO_HANDLE handle, UINT32 *minFrameLength)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    UINT16 yStart, yEnd;
    UINT8 subSamplingFactor;
    UINT16 readOffset;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_Y_START, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    yStart = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_Y_END, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    yEnd = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);
 
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_READOUT_CTRL, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    subSamplingFactor = config.accessRegParams.data & 0x7;

    if (subSamplingFactor == READOUT_SS_X2) {
        subSamplingFactor = 2;
    } else if (subSamplingFactor == READOUT_SS_X4) {
        subSamplingFactor = 4;
    } else {
        subSamplingFactor = 1;
    }

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, MISC_REG_FULL_READ_OFFSET, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    readOffset = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);

    /*
     * We assume the min_line_lenth = current_line_length
     * TODO: consider min_line_lenth & current_line_length
     */
    *minFrameLength = (yEnd - yStart + 1)/subSamplingFactor
                        + 31 + readOffset + 20;

    return retVal;
}

static ERRG_codeE VD55G0_DRVP_getExposureGainLimits(IO_HANDLE handle, UINT32 *maxExpCoarse, UINT32 *minAgain, UINT32 *maxAgain, UINT32 *minDgain, UINT32 *maxDgain)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_MAX_EXPOSURE, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    *maxExpCoarse = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);
    *minAgain = 0;
    *maxAgain = VD55G0_MAX_AGAIN;
    *minDgain = 0;
    *maxDgain = VD55G0_MAX_DGAIN;
}

static ERRG_codeE VD55G0_DRVP_loadFWPatch(IO_HANDLE handle)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    IO_HANDLE handleP = (IO_HANDLE*) deviceDescriptorP;
    UINT16 regNum;

    //Load FW Patch
    sensorTablePtr = VD55G0_FW_PATCH_table;
    regNum = sizeof(VD55G0_FW_PATCH_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to load FW patch with error: 0x%x\n", retVal);
        return retVal;
    }

   //Issue PATCH_SETUP command
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_BOOT, COMMAND_PATCH_SETUP, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to issue PATCH_SETUP command with error: 0x%x\n", retVal);
        return retVal;
   }

   //Delay 52ms as ST suggests
   OS_LYRG_usleep(52000);

   //Check FW version
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP,ST_REG_FWPATCH_REVISION, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to read FW version with error: 0x%x\n", retVal);
        return retVal;
   }

   config.accessRegParams.data = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);
   LOGG_PRINT(LOG_INFO_E, NULL, "FW patch revision(%d,%d)\n"
                   , config.accessRegParams.data & 0xFF
                   , (config.accessRegParams.data >> 8) & 0xFF);

    return retVal;
}

static ERRG_codeE VD55G0_DRVP_loadVTPatch(IO_HANDLE handle)
{
    ERRG_codeE retVal = SENSOR__RET_SUCCESS;
    const GEN_SENSOR_DRVG_regTblParamsT *sensorTablePtr;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    IO_HANDLE handleP = (IO_HANDLE*) deviceDescriptorP;
    UINT16 regNum;

    //Issue START_VTRAM_UPDATE
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_VTPATCHING, COMMAND_START_VTRAM_UPDATE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to issue START_VTRAM_UPDATE with error: 0x%x\n", retVal);
        return retVal;
    }

    //Delay 6ms as ST suggests
    OS_LYRG_usleep(6000);

    //Load VT Patch
    sensorTablePtr = VD55G0_VT_PATCH_table;
    regNum = sizeof(VD55G0_VT_PATCH_table)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
    retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to load VT patch with error: 0x%x\n", retVal);
        return retVal;
    }

    //Issue END_VTRAM_UPDATE command
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_VTPATCHING, COMMAND_END_VTRAM_UPDATE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    if (!ERRG_SUCCEEDED(retVal)) {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to issue END_VTRAM_UPDATE command with error: 0x%x\n", retVal);
         return retVal;
    }

    OS_LYRG_usleep(6000);

    return retVal;
}

static BOOLEAN VD55G0_DRVP_useSlaveAddress(UINT32 powerGpioPin)
{
    for (UINT32 i = 0; i < (sizeof(VD55G0_DRVP_slaveGpios) / sizeof(UINT32)); i++) {
        if (powerGpioPin == VD55G0_DRVP_slaveGpios[i])
            return TRUE;
    }

    return FALSE;
}
/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getPixelClk
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getSensorClks(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   GEN_SENSOR_DRVG_sensorParametersT    *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;

   params->sensorClocksParams.sysClkMhz   = VD55G0_DRVP_getSysClk(handle);
   params->sensorClocksParams.pixelClkMhz = VD55G0_DRVP_getPixClk(handle);
   LOGG_PRINT(LOG_INFO_E, NULL, "%s: sysClk - %d(Mhz) pixelClk - %d(Mhz)\n", __func__, params->sensorClocksParams.sysClkMhz, params->sensorClocksParams.pixelClkMhz);

   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_open
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_open(IO_HANDLE *handleP, IO_PALG_deviceIdE deviceId, void *params)
{
   ERRG_codeE                          retCode           = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT     *pOpenParams      = (GEN_SENSOR_DRVG_openParametersT *)params;
   INT32                              sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;
   static unsigned int master=0;
   FIX_UNUSED_PARAM_WARN(deviceId);
   sensorInstanceId = VD55G0_DRVP_findFreeSlot();
   if (sensorInstanceId > -1)
   {
     deviceDescriptorP = &VD55G0_DRVP_deviceDesc[sensorInstanceId];
     deviceDescriptorP->deviceStatus    = GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN;
     deviceDescriptorP->sensorAddress   = VD55G0_DRVP_I2C_DEFAULT_ADDRESS; //Device address at reset. will be changed later on
     deviceDescriptorP->ioctlFuncList   = VD55G0_DRVP_ioctlFuncList;
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
*  Function Name: VD55G0_DRVP_resetSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
***************************************************************************/
static ERRG_codeE VD55G0_DRVP_resetSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   FIX_UNUSED_PARAM_WARN(handle);

   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_power_down
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_powerdown(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle; 
   LOGG_PRINT(LOG_DEBUG_E,NULL,"power down (type = %d, address = 0x%x, handle = 0x%x)\n",deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);

   //Turn off the sensor 
   retVal = VD55G0_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_CLEAR_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   if (VD55G0_DRVP_useSlaveAddress(deviceDescriptorP->powerGpioMaster)) {
       VD55G0_DRVP_activeInstanceMask &= ~(VD55G0_DRVP_SLAVE_ADDR_ACTIVE);
   } else {
       VD55G0_DRVP_activeInstanceMask &= ~(VD55G0_DRVP_MASTER_ADDR_ACTIVE);
   }
   //after powerdown, the i2c address returns to default
   deviceDescriptorP->sensorAddress = VD55G0_DRVP_I2C_DEFAULT_ADDRESS;
   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_power_up
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_powerup(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(pParams);
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle; 
   UINT32 vt_patch_id0,vt_patch_id1;
   UINT16 new_address;

   LOGG_PRINT(LOG_INFO_E,NULL,"power up (type = %d, address = 0x%x, handle = 0x%x)\n",
           deviceDescriptorP->sensorType,deviceDescriptorP->sensorAddress,handle);
    //Turn on the sensor
   retVal = VD55G0_DRVP_configGpio(deviceDescriptorP->powerGpioMaster,GPIO_DRVG_GPIO_STATE_SET_E);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   OS_LYRG_usleep(VD55G0_DRVP_RESET_SLEEP_TIME);
   retVal = VD55G0_DRVP_wait4FSM(handle, SYSTEM_FSM_READY_TO_BOOT);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_DEVICE_MODEL_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
   config.accessRegParams.data = BYTE_SWAP32(config.accessRegParams.data);
   LOGG_PRINT(LOG_INFO_E,NULL,"Device Model ID: 0x%08x\n", config.accessRegParams.data);

   if (config.accessRegParams.data != VD55G0_CHIP_ID) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Device Model ID error - 0x%x\n", config.accessRegParams.data);
        return SENSOR__ERR_OPEN_DEVICE_FAIL;
   }

   //load FW Patch
   retVal = VD55G0_DRVP_loadFWPatch(handle);
   if (!ERRG_SUCCEEDED(retVal)) {
       LOGG_PRINT(LOG_ERROR_E, NULL, "Sensor loadFWPatch failed: 0x%x\n", retVal);
   }

    /* Send boot command */
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_BOOT, COMMAND_BOOT, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    OS_LYRG_usleep(4000);
    retVal = VD55G0_DRVP_wait4FSM(handle, SYSTEM_FSM_SW_STBY);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Sensor failed to enter SW_STBY state!\n");
        return SENSOR__ERR_USR_INIT_FAILED;
    }
    OS_LYRG_usleep(4000);

#if 0 //ST FAE said that VD55G0 does not need to load VT Patch
    /* Update VT patch*/
    retVal = VD55G0_DRVP_loadVTPatch(handle);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to load VT patch: 0x%x\n", retVal);
        return SENSOR__ERR_USR_INIT_FAILED;
    }

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_VTPATCH_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
    vt_patch_id0 = BYTE_SWAP32(config.accessRegParams.data);
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_VTPATCH_ID + 4, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
    vt_patch_id1 = BYTE_SWAP32(config.accessRegParams.data);
    LOGG_PRINT(LOG_INFO_E, NULL, "VTPATCH_ID: 0x%08x 0x%08x\n", vt_patch_id0, vt_patch_id1);

    LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
#endif

    //Change sensor's address
    if (VD55G0_DRVP_useSlaveAddress(deviceDescriptorP->powerGpioMaster)) {
        new_address = VD55G0_DRVP_I2C_SLAVE_ADDRESS;
        VD55G0_DRVP_activeInstanceMask |= VD55G0_DRVP_SLAVE_ADDR_ACTIVE;
    }
    else
    {
        new_address = VD55G0_DRVP_I2C_MASTER_ADDRESS;
        VD55G0_DRVP_activeInstanceMask |= VD55G0_DRVP_MASTER_ADDR_ACTIVE;
    }
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, SS_REG_DEVICE_I2C_CTRL, new_address, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to change sensor address\n");
        return SENSOR__ERR_USR_INIT_FAILED;
    }
    //Issue I2C_ADDR_UPDATE command
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_SW_STBY, COMMAND_I2C_ADDR_UPDATE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

    OS_LYRG_usleep(4000);
    deviceDescriptorP->sensorAddress = new_address;

    LOGG_PRINT(LOG_INFO_E, NULL, "Handle(0x%08x): Sensor address changed to 0x%x\n", (UINT32)handle, new_address);
    //Read Device ID again to see if the new address works
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_DEVICE_MODEL_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
   config.accessRegParams.data = BYTE_SWAP32(config.accessRegParams.data);
   LOGG_PRINT(LOG_INFO_E,NULL,"Read Device Model ID after change sensor's address: 0x%08x\n", config.accessRegParams.data);

   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_changeSensorAddress
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_changeSensorAddress(IO_HANDLE handle, void *pParams)
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
*  Function Name: VD55G0_DRVP_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_initSensor(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal               = SENSOR__RET_SUCCESS;
#if 0
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_sensorParametersT config;
    UINT16 ref_clk;
    UINT32 vt_patch_id0,vt_patch_id1;
    UINT16 new_address;

    /* Send boot command */
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_BOOT, COMMAND_BOOT, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    OS_LYRG_usleep(4000);
    retVal = VD55G0_DRVP_wait4FSM(handle, SYSTEM_FSM_SW_STBY);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Sensor failed to enter SW_STBY state!\n");
        return SENSOR__ERR_USR_INIT_FAILED; 
    }
    OS_LYRG_usleep(4000);

#if 0 //ST FAE said that VD55G0 does not need to load VT Patch
    /* Update VT patch*/
    retVal = VD55G0_DRVP_loadVTPatch(handle);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to load VT patch: 0x%x\n", retVal);
        return SENSOR__ERR_USR_INIT_FAILED;
    }

    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_VTPATCH_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
    vt_patch_id0 = BYTE_SWAP32(config.accessRegParams.data);
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_VTPATCH_ID + 4, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
    vt_patch_id1 = BYTE_SWAP32(config.accessRegParams.data);
    LOGG_PRINT(LOG_INFO_E, NULL, "VTPATCH_ID: 0x%08x 0x%08x\n", vt_patch_id0, vt_patch_id1);

    LOGG_PRINT(LOG_DEBUG_E,NULL,"init sensor\n");
#endif

    //Change sensor's address
    if (VD55G0_DRVP_useSlaveAddress(deviceDescriptorP->powerGpioMaster))
        new_address = VD55G0_DRVP_I2C_SLAVE_ADDRESS;
    else
        new_address = VD55G0_DRVP_I2C_MASTER_ADDRESS;
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, SS_REG_DEVICE_I2C_CTRL, new_address, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
    if (!ERRG_SUCCEEDED(retVal)) {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to change sensor address\n");
        return SENSOR__ERR_USR_INIT_FAILED;
    }
    //Issue I2C_ADDR_UPDATE command
    GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CMD_REG_SW_STBY, COMMAND_I2C_ADDR_UPDATE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);


    deviceDescriptorP->sensorAddress = new_address;

    LOGG_PRINT(LOG_INFO_E, NULL, "Handle(0x%08x): Sensor address changed to 0x%x\n", (UINT32)handle, new_address);
    //Read Device ID again to see if the new address works
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_DEVICE_MODEL_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
   config.accessRegParams.data = BYTE_SWAP32(config.accessRegParams.data);
   LOGG_PRINT(LOG_INFO_E,NULL,"Read Device Model ID after change sensor's address: 0x%08x\n", config.accessRegParams.data);

#endif
   return retVal;
}

static UINT32 VD55G0_DRVP_getMaxIntegrationTime(GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP)
{
    ERRG_codeE retVal;
    UINT32                              pixelClk = 0;
    UINT32                              lineLength = 0;
    GEN_SENSOR_DRVG_sensorParametersT config;

    VD55G0_DRVP_getLineTiming(deviceDescriptorP, &pixelClk, &lineLength);
    if (pixelClk == 0) {
        return SENSOR__ERR_CONFIGUTATION_FAILED;
    }

    config.accessRegParams.data = 0;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_APPLIED_COARSE_EXPOSURE, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    /*Check: use us as integration time unit*/
    return BYTE_SWAP16(config.accessRegParams.data & 0xFFFF) * lineLength / pixelClk * 1000000;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_loadPresetTable
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_loadPresetTable(IO_HANDLE handle, void *pParams)
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
   GEN_SENSOR_DRVG_sensorParametersT config;

   LOGG_PRINT(LOG_INFO_E, NULL, "addr %x tableType:  %d, res %d\n",deviceDescriptorP->sensorAddress, deviceDescriptorP->tableType,params->loadTableParams.sensorResolution);

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_SYSTEM_FSM, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   LOGG_PRINT(LOG_INFO_E, NULL, "VD55G0 FSM is 0x%x\n", config.accessRegParams.data & 0xFF);

   if (deviceDescriptorP->tableType == INU_DEFSG_MONO_E)
   {
      sensorTablePtr = VD55G0_CFG_TBLG_single_configTable;
      regNum = sizeof(VD55G0_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
   }
   else
   {
#if 0
      if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VGA_E)
      {
         sensorTablePtr = VD55G0_CFG_TBLG_stereo_vga_configTable;
         regNum = sizeof(VD55G0_CFG_TBLG_stereo_vga_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else if (params->loadTableParams.sensorResolution == INU_DEFSG_RES_VERTICAL_BINNING_E)
      {
         sensorTablePtr = VD55G0_CFG_TBLG_stereo_vertical_binning_configTable;
         regNum = sizeof(VD55G0_CFG_TBLG_stereo_vertical_binning_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
      else
      {
      
         sensorTablePtr = VD55G0_CFG_TBLG_stereo_hd_configTable;
         regNum = sizeof(VD55G0_CFG_TBLG_stereo_hd_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
      }
#else
      sensorTablePtr = VD55G0_CFG_TBLG_single_configTable;
      regNum = sizeof(VD55G0_CFG_TBLG_single_configTable)/sizeof(GEN_SENSOR_DRVG_regTblParamsT);
#endif

   }

   if (handleP)
   {
      retVal = GEN_SENSOR_DRVG_regTableLoad(handleP, (GEN_SENSOR_DRVG_regTblParamsT *)sensorTablePtr, regNum);
      if (ERRG_SUCCEEDED(retVal))
      {
         deviceDescriptorP->maxIntegrationTime = VD55G0_DRVP_getMaxIntegrationTime(deviceDescriptorP);
         deviceDescriptorP->minIntegrationTime = 0;
         deviceDescriptorP->maxAECGain = 0xFF;
         deviceDescriptorP->minAECGain = 0;
         VD55G0_DRVP_setStrobeDuration(handle, deviceDescriptorP->maxIntegrationTime);
         VD55G0_DRVP_enStrobe(handle);
      }
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Error loading present table\n");
   }

   //Sleep 64ms as ST suggests
   OS_LYRG_usleep(64000);

   return retVal;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setExposureMode
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setExposureMode(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setImgOffsets
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setImgOffsets(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getAvgBrighness
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getAvgBrighness(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setOutFormat
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setOutFormat(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setPowerFreq
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setPowerFreq(IO_HANDLE handle, void *pParams)
{
   FIX_UNUSED_PARAM_WARN(handle);
   FIX_UNUSED_PARAM_WARN(pParams);
   return SENSOR__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setStrobe(IO_HANDLE handle, void *pParams)
{
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;


   retVal = VD55G0_DRVP_setStrobeDuration(deviceDescriptorP,params->exposureTime);

   return retVal;

}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_syncSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_configSensors(IO_HANDLE handle, void *pParams)
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
*  Function Name: VD55G0_DRVP_stopSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
//remove the gpio bridge operation check Arnon
static ERRG_codeE VD55G0_DRVP_stopSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   FIX_UNUSED_PARAM_WARN(pParams);

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CMD_REG_STREAMING, COMMAND_STOP_STREAM, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   retVal = VD55G0_DRVP_wait4FSM(handle, SYSTEM_FSM_SW_STBY);

   return retVal;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_startSensors
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
***************************************************************************/
static ERRG_codeE VD55G0_DRVP_startSensors(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT   *params     = (GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   if (VD55G0_DRVP_getFSM(handle) == SYSTEM_FSM_STREAMING) {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CMD_REG_STREAMING, COMMAND_STOP_STREAM, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
      VD55G0_DRVP_wait4FSM(handle, SYSTEM_FSM_SW_STBY);
   }

   if (deviceDescriptorP->tableType == INU_DEFSG_STEREO_E) {
        /* Config sensor as SLAVE mode triggered by GPIO_0*/
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, STATIC_REG_VT_CTRL, VT_MODE_SLAVE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CONTEXT_0_REG_GPIO_0_CTRL, GPIO_CTRL_MODE_VT_SLAVE_MODE, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   if (VD55G0_DRVP_getFSM(handle) == SYSTEM_FSM_SW_STBY) {
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CMD_REG_SW_STBY, COMMAND_START_STREAM, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   }

   OS_LYRG_usleep(16000);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   retVal = VD55G0_DRVP_wait4FSM(handle, SYSTEM_FSM_STREAMING);
   LOGG_PRINT(LOG_INFO_E, NULL,"Sensor address %x params->triggerModeParams.isTriggerSupported %x started\n",deviceDescriptorP->sensorAddress,params->triggerModeParams.isTriggerSupported);

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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setFrameRate(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   UINT32                              pixelClk;
   UINT32                              lineLength;
   UINT16                              frameLengthLine;
   UINT32                              minFrameLength;

   retVal = VD55G0_DRVP_getLineTiming(handle, &pixelClk, &lineLength);
   if (ERRG_SUCCEEDED(retVal) && lineLength * params->setFrameRateParams.frameRate)
   {
#ifdef VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
      retVal = VD55G0_DRVP_groupHoldEnable(handle, true);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
     
      retVal = VD55G0_DRVP_getMinFrameLength(handle, &minFrameLength); 
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      frameLengthLine = pixelClk / (lineLength * params->setFrameRateParams.frameRate);
      frameLengthLine = (frameLengthLine < minFrameLength) ? minFrameLength : frameLengthLine;
      GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, handle, CONTEXT_0_REG_FRAME_LENGTH, BYTE_SWAP16(frameLengthLine), VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      VD55G0_DRVP_vts = frameLengthLine;

      LOGG_PRINT(LOG_INFO_E,NULL,"sen frame rate %d. VD55G0_DRVP_lineLengthPclk = 0x%x, frameLengthLine = 0x%x, pixelClk = %d\n",params->setFrameRateParams.frameRate,VD55G0_DRVP_lineLengthPclk,frameLengthLine, pixelClk);


#ifdef VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
       retVal = VD55G0_DRVP_groupHoldEnable(handle, false);
       GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

        
   }
   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines;


   if(!deviceDescriptorP->exposureState.exposuremodeRead)
   {
      retVal = VD55G0_DRVP_getExposureMode(handle, &deviceDescriptorP->exposureState.exposuremode, 0);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);

      if (deviceDescriptorP->exposureState.exposuremode == EXP_MODE_AUTO) {
         LOGG_PRINT(LOG_INFO_E, NULL, "Using auto exposure control, no config will be applied!\n");
      }
      deviceDescriptorP->exposureState.exposuremodeRead = true;
   }

#ifdef VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
   retVal = VD55G0_DRVP_groupHoldEnable(handle, true);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif
   if(!deviceDescriptorP->exposureState.pixelClockAndLineLengthRead)
   {
      retVal = VD55G0_DRVP_getLineTiming(handle, &deviceDescriptorP->exposureState.pixelClk, &deviceDescriptorP->exposureState.lineLength);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      deviceDescriptorP->exposureState.pixelClockAndLineLengthRead = true;
   }

   /* exposureTime unit is us, so divide exposureTIme  by 1000000 */
   numExpLines = (params->exposureTime * (deviceDescriptorP->exposureState.pixelClk / 1000000) + (deviceDescriptorP->exposureState.lineLength - 1)) / deviceDescriptorP->exposureState.lineLength;
   numExpLines =  numExpLines & 0xFFFF;
   if(!deviceDescriptorP->exposureState.readMaxExpLines)
   {
      GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_MAX_EXPOSURE, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
      GEN_SENSOR_DRVG_ERROR_TEST(retVal);
      deviceDescriptorP->exposureState.maxExpLines = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);
      deviceDescriptorP->exposureState.readMaxExpLines = true;
   }
   if (numExpLines >  deviceDescriptorP->exposureState.maxExpLines )
   {
      LOGG_PRINT(LOG_INFO_E,NULL,"numExpLines limits from 0x%x to 0x%x\n",numExpLines, deviceDescriptorP->exposureState.maxExpLines );
      numExpLines =  deviceDescriptorP->exposureState.maxExpLines ;
   }

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CONTEXT_0_REG_MANUAL_COARSE_EXP, BYTE_SWAP16(numExpLines), VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   LOGG_PRINT(LOG_DEBUG_E,NULL,"pixel_clk = %d, numExpLines = 0x%x maxExpLines 0x%x, time = %d, address = 0x%x, context = %d\n",deviceDescriptorP->exposureState.pixelClk,numExpLines,deviceDescriptorP->exposureState.maxExpLines,params->exposureTime,deviceDescriptorP->sensorAddress,params->context);

#ifdef VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
   retVal = VD55G0_DRVP_groupHoldEnable(handle, false);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getExposureTime
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getExposureTime(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_exposureTimeCfgT    *params=(GEN_SENSOR_DRVG_exposureTimeCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT32                              numExpLines = 0;
   UINT16                              sysClk;

   params->exposureTime = 0;

   //printf("[leonlew#####]: %s (handle is 0x%08X, model - %d) - sensorAddress is 0x%x\n", __func__, (UINT32)handle,
     //             deviceDescriptorP->sensorModel, deviceDescriptorP->sensorAddress);
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_APPLIED_COARSE_EXPOSURE, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   numExpLines = BYTE_SWAP16(config.accessRegParams.data & 0xFFFF);

   sysClk = VD55G0_DRVP_getSysClk(handle);
   params->exposureTime = (numExpLines * VD55G0_DRVP_lineLengthPclk) / sysClk;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"exposure time = %d, numExpLines = %d 0x%x, VD55G0_DRVP_lineLengthPclk = %d, sys_clk = %d, context = %d\n",params->exposureTime,numExpLines,numExpLines,VD55G0_DRVP_lineLengthPclk, sysClk,params->context);

   return retVal;
}

/**
 * @brief Converts a float to fixed point
 *
 * @param f Float to convert
 * @return UINT16 Fixed point result
 */
static UINT16 VD55G0_Convert_Float_To_FP_5_8(const float f )
{
   const float fixedPointVlue = f*256;
   return ((UINT16)fixedPointVlue) & 0x1FFF;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_gainCfgT             *params=(GEN_SENSOR_DRVG_gainCfgT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   params->analogGain = (params->analogGain > VD55G0_MAX_AGAIN) ? VD55G0_MAX_AGAIN : params->analogGain;

#ifdef VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
   retVal = VD55G0_DRVP_groupHoldEnable(handle, true);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CONTEXT_0_REG_MANUAL_AGAIN, (UINT32)params->analogGain , VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   UINT16 digitalGainVal = VD55G0_Convert_Float_To_FP_5_8(params->digitalGain);
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CONTEXT_0_REG_MANUAL_DGAIN, BYTE_SWAP16(digitalGainVal) , VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

#ifdef VD55G0_DRVP_GROUP_MODE_EXP_CHANGE
   retVal = VD55G0_DRVP_groupHoldEnable(handle, false);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
#endif

   LOGG_PRINT(LOG_DEBUG_E,NULL,"Set again to %f, dgain to %f\n",params->analogGain, params->digitalGain);
   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getGain
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getGain(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT config;
   GEN_SENSOR_DRVG_gainCfgT *params = (GEN_SENSOR_DRVG_gainCfgT *) pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *) handle;

   //printf("[leonlew#####]: %s (handle is 0x%08X, model - %d) - sensorAddress is 0x%x\n", __func__, (UINT32)handle,
     //             deviceDescriptorP->sensorModel, deviceDescriptorP->sensorAddress);
   /* Read analog gain */
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_APPLIED_ANALOG_GAIN, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   params->analogGain = config.accessRegParams.data & 0x1F;

   /* Read digital gain */
   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_APPLIED_DIGITAL_GAIN, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);
   UINT16 digitalGainVal = BYTE_SWAP16(config.accessRegParams.data & 0x1FFF);

   LOGG_PRINT(LOG_INFO_E,NULL,"dgain from register 0x%x(%d)\n", digitalGainVal, digitalGainVal);
   /*Convert FP5.8 register value to float*/
   params->digitalGain = (float)digitalGainVal / 256;

   LOGG_PRINT(LOG_DEBUG_E,NULL,"again is %f, dgain is %f\n",params->analogGain, params->digitalGain);

   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getDeviceId
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getDeviceId(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   UINT32                              regVal[4];
   GEN_SENSOR_DRVG_getDeviceIdParamsT  *deviceIdParamsP = (GEN_SENSOR_DRVG_getDeviceIdParamsT*)pParams;

   FIX_UNUSED_PARAM_WARN(handle);

   regVal[0] = 0x30;
   regVal[1] = 0x47;
   regVal[2] = 0x35;
   regVal[3] = 0x53;

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
*  Function Name: VD55G0_DRVP_findFreeSlot
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/

static INT32 VD55G0_DRVP_findFreeSlot()
{
   UINT32                              sensorInstanceId;
   for(sensorInstanceId = 0; sensorInstanceId < sizeof(VD55G0_DRVP_deviceDesc)/sizeof(GEN_SENSOR_DRVG_specificDeviceDescT); sensorInstanceId++)
   {
      if(VD55G0_DRVP_deviceDesc[sensorInstanceId].deviceStatus == GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E)
      {
        return sensorInstanceId;
      }
   }
   return -1;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_mirrorFlip
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_mirrorFlip(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_sensorParametersT    *params=(GEN_SENSOR_DRVG_sensorParametersT *)pParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   switch (params->orientationParams.orientation) {
       case INU_DEFSG_SENSOR_MIRROR_E:
               config.accessRegParams.data = IMAGE_X_FLIP;
               break;
       case INU_DEFSG_SENSOR_FLIP_E:
               config.accessRegParams.data = IMAGE_Y_FLIP;
               break;
       case INU_DEFSG_SENSOR_MIRROR_FLIP_E:
               config.accessRegParams.data = IMAGE_XY_FLIP;
               break;
       default:
               /* Default: no flip & mirror */
               config.accessRegParams.data = IMAGE_NO_FLIP;
               break;
   }
   
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, STATIC_REG_ORIENTATION, 
                   config.accessRegParams.data, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);

   GEN_SENSOR_DRVG_ERROR_TEST(retVal);  
   LOGG_PRINT(LOG_DEBUG_E,NULL,"mirror flip mode = %d\n",params->orientationParams.orientation);
   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setStrobeDuration
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setStrobeDuration(IO_HANDLE handle, UINT32 usec)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   return retVal;
}


/****************************************************************************
*
*  Function Name: VD55G0_DRVP_enStrobe
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_enStrobe(IO_HANDLE handle)
{
   ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   config;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
   UINT8 gpio_ctrl_regval;

   GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, CONTEXT_0_REG_GPIO_1_CTRL, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   gpio_ctrl_regval = config.accessRegParams.data & 0xFF;

   /* Set strobe mode */
   gpio_ctrl_regval &= ~0xF;
   gpio_ctrl_regval |= GPIO_CTRL_MODE_STROBE;
   /* TODO: set polarity */
   GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, CONTEXT_0_REG_GPIO_1_CTRL, gpio_ctrl_regval, VD55G0_DRVP_SENSOR_ACCESS_1_BYTE, retVal);
   GEN_SENSOR_DRVG_ERROR_TEST(retVal);

   return retVal;

}

static ERRG_codeE VD55G0_DRVP_configGpio(UINT32 gpioNum,GPIO_DRVG_gpioStateE state)
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
*  Function Name: VD55G0_DRVP_trigger
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_trigger(IO_HANDLE handle, void *pParams)
{
   ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
   (void)handle;(void)pParams;   
   return retVal;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_close_gpio
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_close_gpio(UINT32 gpioNum)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   GPIO_DRVG_gpioOpenParamsT     params;
   params.gpioNum =gpioNum;     
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_CLOSE_GPIO_CMD_E, &params); 
   return retCode;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_close(IO_HANDLE handle)
{
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

   VD55G0_DRVP_powerdown(handle,NULL);
   deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
   VD55G0_DRVP_close_gpio(deviceDescriptorP->powerGpioMaster);
   LOGG_PRINT(LOG_ERROR_E, NULL, "close \n");
   return(SENSOR__RET_SUCCESS);
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getChipID
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getChipID(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE                          retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    UINT32 *chipID = pParams;

    config.accessRegParams.data = 0;
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, handle, ST_REG_DEVICE_MODEL_ID, VD55G0_DRVP_SENSOR_ACCESS_4_BYTE, retVal);
    *chipID = BYTE_SWAP32(config.accessRegParams.data);

    return retVal;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_activateTestPattern
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_activateTestPattern(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;

    BOOLEAN enable = *(BOOLEAN *)pParams;
    if (enable != deviceDescriptorP->testPatternEn) {
        UINT16 regVal = enable ? (1 << 3) : 0;
        
        if (enable) {
            /* Test pattern set to a fixed pattern, e.g: vertical colorbar */
            retVal |= (TEST_PAT_TYPE_VBAR << 2);
        }

        GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, deviceDescriptorP, DYN_REG_PATGEN_CTRL, BYTE_SWAP16(regVal), VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
        GEN_SENSOR_DRVG_ERROR_TEST(retVal);
        deviceDescriptorP->testPatternEn = enable;
    }

    return retVal;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_setBayerPattern
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setBayerPattern(IO_HANDLE handle, void *pParams)
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
*  Function Name: VD55G0_DRVP_setBLC
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setBLC(IO_HANDLE handle, void *pParams)
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
*  Function Name: VD55G0_DRVP_setWB
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_setWB(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    /*
     * No decription in VD55G0 data sheet for white balance
     * this function will do nothing for now
     */

    return retVal;
}

/****************************************************************************
*
*  Function Name: VD55G0_DRVP_changeResolution
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_changeResolution(IO_HANDLE handle, void *pParams)
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
*  Function Name: VD55G0_DRVP_exposureCtrl
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_exposureCtrl(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    /*
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
     */

    return retVal;
}
static ERRG_codeE VD55G0_DRVP_setCropWindow(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    /*
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
     */
}
/****************************************************************************
*
*  Function Name: VD55G0_DRVP_getTemperature
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
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_getTemperature(IO_HANDLE handle, void *pParams)
{
    ERRG_codeE  retVal = SENSOR__RET_SUCCESS;
    FIX_UNUSED_PARAM_WARN(pParams);
    GEN_SENSOR_DRVG_sensorParametersT   config;
    GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP   = (GEN_SENSOR_DRVG_specificDeviceDescT *)handle;
    GEN_SENSOR_DRVG_temperatureParamsT    *params=(GEN_SENSOR_DRVG_temperatureParamsT *)pParams;

    //Read TEMPERATURE status
    GEN_SENSOR_DRVG_READ_SENSOR_REG(config, deviceDescriptorP, ST_REG_TEMPERATURE, VD55G0_DRVP_SENSOR_ACCESS_2_BYTE, retVal);
    params->temperature = (INT32)BYTE_SWAP16(config.accessRegParams.data);

    return retVal;
}
static ERRG_codeE VD55G0_DRVP_getCropWindow(IO_HANDLE handle, void *pParams)
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
*  Function Name: VD55G0_DRVP_ioctl
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
static ERRG_codeE VD55G0_DRVP_ioctl(IO_HANDLE handle, UINT32 cmd, void *argP)
{
     return(((GEN_SENSOR_DRVG_specificDeviceDescT *)handle)->ioctlFuncList[cmd](handle, argP));
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: VD55G0_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: VD55G0 sensor driver
*
****************************************************************************/
ERRG_codeE VD55G0_DRVG_init(IO_PALG_apiCommandT *palP)
{
   UINT32                               sensorInstanceId;
   GEN_SENSOR_DRVG_specificDeviceDescT *deviceDescriptorP;

   if(VD55G0_DRVP_isDrvInitialized == FALSE)
   {
      for(sensorInstanceId = 0; sensorInstanceId < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorInstanceId++)      {
      
         deviceDescriptorP = &VD55G0_DRVP_deviceDesc[sensorInstanceId];
         memset(deviceDescriptorP,0,sizeof(GEN_SENSOR_DRVG_specificDeviceDescT));
         deviceDescriptorP->deviceStatus  = GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E;
         deviceDescriptorP->ioctlFuncList = NULL;
         deviceDescriptorP->sensorAddress = 0;
         deviceDescriptorP->i2cInstanceId = I2C_DRVG_I2C_INSTANCE_1_E;
         deviceDescriptorP->i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
         deviceDescriptorP->sensorModel   = INU_DEFSG_SENSOR_MODEL_VD55G0_E; //TODO: add this to INU_DEFSG_sensorModelE
         deviceDescriptorP->sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
         deviceDescriptorP->sensorHandle  = NULL;
         deviceDescriptorP->testPatternEn = FALSE;
         deviceDescriptorP->bayerPattern  = GEN_SENSOR_DRVG_BAYER_RGGB;
         deviceDescriptorP->exposureState.exposuremodeRead = false;
         deviceDescriptorP->exposureState.pixelClockAndLineLengthRead = false;
         deviceDescriptorP->exposureState.readMaxExpLines = false;
      }
      VD55G0_DRVP_isDrvInitialized = TRUE;
   }

   if (palP!=NULL)
   {
      palP->close =  (IO_PALG_closeT) &VD55G0_DRVP_close;
      palP->ioctl =  (IO_PALG_ioctlT) &VD55G0_DRVP_ioctl;
      palP->open  =  (IO_PALG_openT)  &VD55G0_DRVP_open;
   }
   else
   {
      return SENSOR__ERR_NULL_PTR;
   }

   return SENSOR__RET_SUCCESS;
}
static ERRG_codeE VD55G0_DRVP_unimplementedFunction(IO_HANDLE handle, void *pParams)
{
    /*Added to fill the gaps in the IOCTL table*/
    FIX_UNUSED_PARAM_WARN(handle);
    FIX_UNUSED_PARAM_WARN(pParams);
    LOGG_PRINT(LOG_ERROR_E,NULL,"Unimplemented ioctl called for VD55g0 \n");
    return SENSOR__ERR_NULL_PTR;
}

#ifdef __cplusplus
}
#endif

