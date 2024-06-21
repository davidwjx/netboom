#pragma once

#include "inu2.h"
#include "err_defs.h"
#include "inu_metadata.h"
#include "inu_sensorsync.h"
typedef void (frameCallbackFunc)(inu_refH ref, inu_data__hdr_t *hdrP, char *dataP, int size, void *cookie, INU_Metadata_T *metadata, inu_deviceH* device);
typedef void (startCallbackFunc)(inu_refH ref, inu_data__hdr_t *hdrP, void *cookie);
typedef void (stopCallbackFunc) (inu_refH ref, void *cookie);
typedef void (failedFrameCallBackFunc)(inu_refH ref,void *cookie, inu_deviceH* device);

#define SANDBOX_NUM_HW_CHANNELS             (27) /*Increased by WIlliam Bright, this is too low when you consider we have ISP channels at indexes 23->27*/
#define SANDBOX_NUM_CHANNELS_SRC_SENSORS    (2)
#define SANDBOX_MAX_DPE_PRESET_FILE         (5)
#define SANDBOX_MAX_LEN_FILE_NAME           (255)
#define SANDBOX_MAX_FILES_TO_SEND           (16)
#define SANDBOX_NUM_SENSORS                 (6)
#define BUFFER_SIZE                         (16 * 1024)

enum FWLib_sensorMode
{
    FWLIB_SENSOR_MODE_BIN_E = 0,
    FWLIB_SENSOR_MODE_FULL_E,
    FWLIB_SENSOR_MODE_USER_DEFINE_E,
    FWLIB_SENSOR_MODE_VERTICAL_BINNING_E,
    FWLIB_SENSOR_MODE_UXGA_E,
    FWLIB_SENSOR_MODE_UNKNOWN
};

typedef enum
{
    FWLIB_PD_WAIT_FOR_PD_E = 0,
    FWLIB_PD_WAIT_FOR_WU_E,
} FWLib_pdStateE;

typedef struct FWLib_res
{
   UINT32 width;
   UINT32 height;
   UINT32 bpp;
   UINT32 x;
   UINT32 y;
   UINT32 stride;
   UINT32 bufferHeight;
} FWLib_resT;


typedef struct FWLib_channelSrcSensorsList
{
   UINT32 sensorList[SANDBOX_NUM_CHANNELS_SRC_SENSORS];
   UINT32 numOfSensors;
} FWLib_channelSrcSensorsListT;

typedef struct
{
   bool is_active;
   char bin_name[SANDBOX_MAX_LEN_FILE_NAME];
   inu_load_network_cdnnIdE networkId;
   CDNNG_channelSwapE channelSwap;
   UINT32 pipeDepth;
   UINT32 ProcesssingFlags;
}inu_load_network_paramsT;

typedef struct
{
    char bin_name[SANDBOX_MAX_LEN_FILE_NAME];
    int vocabularyId;
}inu_load_vocabulary_paramsT;

typedef struct
{
    char bin_name[SANDBOX_MAX_LEN_FILE_NAME];
    int backgroundId;
}inu_load_background_paramsT;

typedef struct SANDBOX_initParams
{
   int disablePowerOptimization;
   int allowPowerDown;
   int standalone;
   int watchdogTimeout;
   char configFilesPath[SANDBOX_MAX_LEN_FILE_NAME];
   char calibPath[256];
   char filesSendList[SANDBOX_MAX_FILES_TO_SEND][SANDBOX_MAX_LEN_FILE_NAME];
   char filesDstNameSendList[SANDBOX_MAX_FILES_TO_SEND][SANDBOX_MAX_LEN_FILE_NAME];
   int numberOfFilesToSend;
   int bootid;
   bool hwXmlDebugMode;
   int alternate;
   int loadNetworkEnabled;
   inu_load_network_paramsT inu_load_network_params[INU_LOAD_NETWORK__IAE_TOTAL_CDNN_E];
   int loadVocabularyEnabled;
   inu_load_vocabulary_paramsT inu_load_vocabulary_params;
   int loadBackgroundEnabled;
   inu_load_background_paramsT inu_load_background_params;
   int useUartSync;
   char uartPortName[MAX_UART_PORT_NAME_LEN];
   int interfaceSelect;
   inu_device_uart_setup_t uartIfParams;
   inu_device_cdc_setup_t cdcIfParams;
   inu_device_tcp_setup_t tcpIfParams;
   char tuningServer;
} FWLib_initParamsT;

typedef struct FWLib_configParams
{
   char *name;
   char *hwXmlFile;
   char *swXmlFile;
   int fps[SANDBOX_NUM_SENSORS];
   int mode[SANDBOX_NUM_SENSORS];
   int iaeBypass[SANDBOX_NUM_HW_CHANNELS];
   int interleave[SANDBOX_NUM_HW_CHANNELS];
   int chunkMode[SANDBOX_NUM_HW_CHANNELS];
   FWLib_resT crop[SANDBOX_NUM_HW_CHANNELS];
   FWLib_resT scale[SANDBOX_NUM_HW_CHANNELS];
   FWLib_resT injectRes[SANDBOX_NUM_HW_CHANNELS];
   int cscMode[SANDBOX_NUM_HW_CHANNELS];
   FWLib_channelSrcSensorsListT injectSensorInfo[SANDBOX_NUM_HW_CHANNELS];
   char dpeConfigFiles[SANDBOX_MAX_DPE_PRESET_FILE][SANDBOX_MAX_LEN_FILE_NAME];
   int dpeConfigNumFiles;
   void *configH;
   int injectionMode;
   int cfgId;
   int forceXML_Type; /* 0 = Don't force an XML type, 1 = Force master, 2 = Force Slave, 3 = Auto*/
   unsigned int extInterleaveIdx;
   unsigned int extInterleaveChannels[NUCFG_MAX_DB_META_CHANNELS];
} FWLib_configParamsT;


typedef struct FWLib_cfgStreamCbs
{
   frameCallbackFunc *frameCallback;
   startCallbackFunc *startCallback;
   stopCallbackFunc *stopCallback;
   failedFrameCallBackFunc *failCallback;
   failedFrameCallBackFunc *recursiveStartDone;
   void *cookie;
}FWLib_cfgChannelCbsT;

typedef enum
{
   FW_LIB__REG_SOC_E = 0,
   FW_LIB__REG_I2C_E = 1,
   FW_LIB__REG_ATT_E = 2,
   FW_LIB__REG_WAIT_E = 3,
   FW_LIB__REG_DPHY_E = 4,
   FW_LIB__REG_TEMP_E = 5,
} FWLib_regTypeE;

typedef struct
{
   UINT32 addr; //physical address
   UINT32 val;  //value to read or write
} FWLib_socRegT;

typedef struct
{
   UINT32               i2cNum;
   UINT32              i2cData;
   UINT32              regWidth;
   UINT32              regAddress;
   UINT8               tarAddress;
   UINT8               accessSize;
} FWLib_i2cRegT;

typedef struct
{
   UINT8 addr;
   UINT8 val;
} FWLib_attRegT;

typedef struct
{
   UINT32 dphyNum;
   UINT32 addr;
   UINT32 val;
} FWLib_dphyRegT;

typedef struct
{
    UINT32 addr;
    UINT32 vali;
    float  valf;
} FWLib_tempRegT;

typedef struct FWLib_readRegister
{
   FWLib_regTypeE   regType;
   FWLib_socRegT    socReg;
   FWLib_i2cRegT    i2cReg;
   FWLib_attRegT    attReg;
   FWLib_dphyRegT   dphyReg;
   FWLib_tempRegT   tempReg;
}FWLib_accessRegisterT;


typedef void* FWLib_chH;

/*
   FwLib_lib_init(int bootId)
   Init library with FWLib_initParamsT
*/
extern ERRG_codeE FwLib_lib_init(SANDBOX_initParams *paramsP);

/*
FwLib_activate_channel(...)
Activate HW channel. User callbacks invoked when start,new data, stop events occur.
If the FWlib runs from host processor, the FWLib will stream out from the device the data and invoke the callbacks on it.

inu_deviceH          deviceH          - handle to the device
FWLib_configParams   *paramsP         - configuration from which to open the channel
char                 *stream_out_name - the name of the reqyested channel
FWLib_cfgChannelCbsT *chCfgCbsP       - user callbacks for this channel
FWLib_chH            *chH             - handle to the channel
*/
extern ERRG_codeE FwLib_activate_channel(inu_deviceH deviceH, FWLib_configParams *paramsP, char *stream_out_name, FWLib_cfgChannelCbsT *chCfgCbsP, FWLib_chH *chH);

/*
   FwLib_stop_channel(...)
   Stop HW channel.

   inu_deviceH          deviceH     - handle to the device
   FWLib_chH            chH         - handle to the channel
*/
extern ERRG_codeE FwLib_stop_channel(inu_deviceH deviceH, FWLib_chH chH);

extern ERRG_codeE FwLib_activate_hw(inu_deviceH deviceH, FWLib_configParams *paramsP, char *function_name, FWLib_chH *chH);

extern ERRG_codeE FwLib_stop_hw(inu_deviceH deviceH, FWLib_chH chH);

extern ERRG_codeE FwLib_offline_get_config(const char *socxml_path, inu_nucfgH *cfgH);

/*
   FwLib_init(...)
   Init a device. Returns a handle to the device and to the device version info

   inu_deviceH          *deviceH_ptr       - handle to the device
   inu_device__version  *deviceVersionP    - version struct
*/
extern ERRG_codeE FwLib_init(inu_deviceH *deviceH_ptr, const inu_device__version **deviceVersionP);

/*
   FwLib_waitForPdState(...)
   return when suspend & resume sequence has been completed

   inu_deviceH          *deviceH_ptr       - handle to the device
   FWLib_pdStateE       pdState            - state to wait
*/
extern ERRG_codeE FwLib_waitForPdState(FWLib_pdStateE pdState);

/*
   FwLib_config(...)
   Creates a configuration from the given params.
   The handle to the configuration is kept inside the FWLib_configParams

   inu_deviceH          deviceH     - handle to the device
   FWLib_configParams   *paramsP    - configuration from which to open the channel (void *configH)
*/
extern ERRG_codeE FwLib_config(inu_deviceH deviceH, FWLib_configParams *paramsP);

/*
   FwLib_close_config(...)
   Closes a configuration.

   inu_deviceH          deviceH     - handle to the device
   FWLib_configParams   *paramsP    - the configuration to close
*/
extern ERRG_codeE FwLib_close_config(inu_deviceH deviceH, FWLib_configParams *paramsP);

/*
   FwLib_bind_nodes_configs(...)
   Bind nodes between two different configurations, which will instruct the graph scheduler to
   pass data between the two.

   FWLib_configParams   *params1P    - configuration from which to to connect from
   char                 *name1       - name of the node to connect from
   FWLib_configParams   *params2P    - configuration from which to to connect to
   char                 *name2       - name of the node to connect to

*/
extern ERRG_codeE FwLib_bind_nodes_configs(FWLib_configParams *params1P, char *name1, FWLib_configParams *params2P, char *name2);

/*
   FwLib_deinit(...)
   De-init a device. Gets a handle to the device and de-init the device.

   inu_deviceH          *deviceH_ptr       - handle to the device
*/
extern ERRG_codeE FwLib_deinit(inu_deviceH deviceH);

/*
   FwLib_boot(int bootId)
   Boot device with USB interface.
   Bootid - overwrite the module Id, to load the corresponding configuration files. Use -1 for auto detect from device flash
*/
extern ERRG_codeE FwLib_boot(int bootId);

/*
   FwLib_boot_deinit()
   Deinit the boot process.
*/
extern ERRG_codeE FwLib_boot_deinit();

/****************************************************************************************************/
/*                  RUNTIME APIs                                                                    */
/****************************************************************************************************/
/*
   FwLib_read_register
*/
extern ERRG_codeE FwLib_read_register(inu_deviceH deviceH, FWLib_accessRegisterT *accessRegP);

/*
   FwLib_write_register
*/
extern ERRG_codeE FwLib_write_register(inu_deviceH deviceH, FWLib_accessRegisterT *accessRegP);

/*
   get_sensor_params
*/
ERRG_codeE FwLib_get_sensor_params(inu_deviceH deviceH, UINT32 sensorId, UINT32 *exposureUsP, float *analogGainP, float *digitalGainP, UINT32 *fps, INT32 *temperature);

/*
   get_sensorGroup_params
*/
extern ERRG_codeE FwLib_get_sensorGroup_params(inu_deviceH deviceH, UINT32 sensorGroupId, UINT32 *exposureUsP, float *analogGainP, float*digitalGainP, UINT32 *fps);

/*
   FwLib_set_sensorGroup_params
*/
extern ERRG_codeE FwLib_set_sensorGroup_params(inu_deviceH deviceH, UINT32 sensorGroupId, UINT32 exposureUs, float analogGain, float digitalGain, UINT32 fps);
/*
   FwLib_getSensorFocusParams
*/
extern ERRG_codeE FwLib_getSensorFocusParams(inu_deviceH deviceH, UINT32 sensorId, INT32* dac, INT32* min, INT32* max, UINT32* chipId);

/*
   FwLib_setSensorFocusParams
*/
extern ERRG_codeE FwLib_setSensorFocusParams(inu_deviceH deviceH, UINT32 sensorId, INT32 dac, int mode);

/*
   FwLib_setSensorCropParams
*/
extern ERRG_codeE FwLib_setSensorCropParams(inu_deviceH deviceH, UINT32 sensorId, UINT32 startX, UINT32 startY);
/*
   FwLib_sw_reset
*/
extern void FwLib_sw_reset(inu_deviceH meH);

/*
FwLib_reset_channel_frame_count(...)
API to reset the frame counter of a specific channel, to be called from
usercallback functions context

inu_refH             ref
*/
extern ERRG_codeE FwLib_reset_channel_frame_count(inu_refH ref);

/*
FwLib_set_channel_user_info(...)
API to set user data to be passed via data headers. Function should be called
from usercallback functions context

inu_refH ref
UINT32 param_1
UINT32 param_2
*/
extern ERRG_codeE FwLib_set_channel_user_info(inu_refH ref, UINT32 param_1, UINT32 param_2);

extern ERRG_codeE FwLib_set_network_to_node(inu_deviceH deviceH, void *configH, char *cdnn_func_name, UINT32 network_id);

extern void FwLib_reload_lut_files(inu_deviceH deviceH, char *calib_path);

ERRG_codeE FwLib_enableTuningServer(inu_deviceH deviceH,UINT8 sensorNum);
ERRG_codeE FwLib_ispCommand(inu_refH ref, UINT32 cmd, ...);
bool FWLib_serializeMetadata(INU_Metadata_T* metadata, char* buffer, UINT32 size);
bool FWLib_serializeMetadataNoDelimit(INU_Metadata_T* metadata, char* buffer, UINT32 size, UINT32 *outputSize);
int FWLib_GetMetadataSizeWithoutLinePadding(char* buffer, size_t size);
ERRG_codeE FWLIB_getWBGains(inu_refH ref,inu_isp_channel__ispCmdParamU *command);
ERRG_codeE FWLIB_getExpoParams(inu_refH ref, inu_isp_channel__ispCmdParamU *command);/*
   FwLib_set_manual_exposure_params
*/
extern ERRG_codeE FwLib_set_manual_exposure_params(inu_deviceH deviceH, UINT32 sensorGroupId, UINT32 exposureUs, float analogGain, float digitalGain);
