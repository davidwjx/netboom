/****************************************************************************
 *
 *   FileName: inu_cmd.h
 *
 *   Author: Ram B.
 *
 *   Date:
 *
 *   Description: Inuitive Service API - ioctl commands and definitions
 *
 ****************************************************************************/
#ifndef INU_CMD_H
#define INU_CMD_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_types.h"
#include "inu_defs.h"

#ifdef __cplusplus
extern "C" {
#endif


/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define INUG_SERVICE_LEN               (8)  //up to 256 services
#define INUG_SERVICE_MASK(s)           ((s) << (32-INUG_SERVICE_LEN))
#define INUG_SERVICE_INDEX(s)          ((((s) >> (32-INUG_SERVICE_LEN))& 0xFF))
#define INUG_SERVICE_IOCTL_INDEX(s)    ((s)&0xFFFFFF)

#define INUG_GET_DATA_TYPE_SVC(s)      ((s) >> (32-INUG_SERVICE_LEN) )
#define INUG_GET_DATA_TYPE_NUM(s)      ((s)&0xFFFFFF)

#define __IOCTL(num, svc, name)        name = num | INUG_SERVICE_MASK(svc)
#define __DATATYPE(num, svc, name)     name = num | INUG_SERVICE_MASK(svc)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void *INUG_handleT;
typedef int (*INUGServiceLinkCallback)(int sid, int serviceLinkEvent, void *argP);
typedef void (*INUG_writeNotifyDoneCb)(void *bufferP);


//Define services here
typedef enum
{
   INUG_SERVICE_MNGR_E           = 0,  // RESERVED - DO NOT USE !!!
   INUG_SERVICE_SYSTEM_E         = 1,
   INUG_SERVICE_LOGGER_E         = 2,
   INUG_NUM_SERVICES_E
} INUG_serviceIdE;

typedef enum
{
   INUG_CALIBRATION_SECTION_FORMAT_RAW = 0,
   INUG_CALIBRATION_SECTION_FORMAT_ZIP = 1,
} INUG_calibrationDataFormatE;

typedef enum
{
   INUG_STORAGE_BOOT_FROM_USB     = 0,
   INUG_STORAGE_BOOT_FROM_FLASH   = 1,
} InuG_StrapOverrideE;

typedef enum
{
   INUG_REGULAR_MODE_E     = 0,
   INUG_STAND_ALONE_MODE_E = 1,
} INUG_hostTargetConnectionE;

//Service IOCTL commands
//8 MSbits are used for service ID mask
typedef enum
{
   //General service commands
   INU_CMDG_FREE_BUF_E           = 0x00000001,
   INU_CMDG_GEN_LAST_E,

   //////////////////////////////////////////////////
   //Specific service commands

   // System Service
   /* GENERAL HW Control */
   __IOCTL(0x1,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_GET_VERSION_E),
   __IOCTL(0x2,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_RD_REG_E),
   __IOCTL(0x3,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_WR_REG_E),
   __IOCTL(0x4,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_DB_CONTROL_E),
   __IOCTL(0x5,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_POWER_MODE_E),
   __IOCTL(0x6,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_GET_TIME_E),
   __IOCTL(0x7,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_GET_STATS_E),
   __IOCTL(0x8,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SET_TIME_E),
   //Sensor Configuration
   __IOCTL(0x9,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG),
   __IOCTL(0xA,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG),
   //Calibration read ioctl
   __IOCTL(0xB,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_CALIBRATION_RD_E),
   //Buffer read
   __IOCTL(0xC,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_BUFFER_RD_E),
   //Alternate configuration
   __IOCTL(0xD,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_ALTERNATE_SET_CONFIG_E),
   //Calibration Header read
   __IOCTL(0xF,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_CALIBRATION_HDR_RD_E),
   //Boot mode
   __IOCTL(0x10,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_BOOT_SET_MODE_E),
   //projector config mode
   __IOCTL(0x11,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E),
   //projector get mode
   __IOCTL(0x12,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_PROJECTOR_GET_MODE_E),
   //Sys get mode
   __IOCTL(0x13,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_GET_MODE_E),
   //Sys set mode mode
   __IOCTL(0x14,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SET_MODE_E),
   //uart2 loopback
   __IOCTL(0x15,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_UART2LOOPBACK_PROBE_E),
   //calibration paths
   __IOCTL(0x16,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_CALIB_PATH_E),
   //chain hdr mode
   __IOCTL(0x17,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_CHAIN_HDR_E),
   //Activate new Calibration mode
   __IOCTL(0x18,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_ACTIVATE_CALIB_MODE_E),
   //Read buffer
   __IOCTL(0x19,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_READ_BUFFER_E),
   //Config PWM
   __IOCTL(0x1A,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_CONFIG_PWM_E),
   //Update Calibration
   __IOCTL(0x1B,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_UPDATE_CALIBRATION_E),
   //EEPROM access
   __IOCTL(0x1C,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_EEPROM_READ_E),
   //Power down
   __IOCTL(0x1D,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_POWER_DOWN_E),
   //Tuning server
   __IOCTL(0x1E,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_TUNING_SERVER_E),
   //Sensor focus Configuration
   __IOCTL(0x1F,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SENSOR_FOCUS_GET_PARAMS),
   __IOCTL(0x20,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SENSOR_FOCUS_SET_PARAMS),
   __IOCTL(0x21,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SELECT_TEMP_SENSOR_E),
   __IOCTL(0x22,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_ERASE_DYN_CALIBRATION_E),
   __IOCTL(0x23,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SET_FLASH_SECTION_E),
   __IOCTL(0x24,  INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SENSOR_CROP_SET_PARAMS),
   /*Helsinki specific*/
   __IOCTL(0x40,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_GET_RTC_LOCK_E),
   __IOCTL(0x41,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SYSTEM_SENSORUPDATE),
   __IOCTL(0x42,   INUG_SERVICE_SYSTEM_E,  INUG_IOCTL_SENSOR_SYNC_E),
   INUG_IOCTL_SYSTEM_LAST_E,

   //Logger Service
   __IOCTL(0x1,   INUG_SERVICE_LOGGER_E,        INUG_IOCTL_LOGGER_SET_PARAMS_E),
   __IOCTL(0x2,   INUG_SERVICE_LOGGER_E,        INUG_IOCTL_LOGGER_GET_PARAMS_E),
   __IOCTL(0x3,   INUG_SERVICE_LOGGER_E,        INUG_IOCTL_LOGGER_START_E),
   __IOCTL(0x4,   INUG_SERVICE_LOGGER_E,        INUG_IOCTL_LOGGER_STOP_E),
   INUG_IOCTL_LOGGER_LAST_E,
} INUG_ioctlCmdE;

//Service DATA type
//8 MSbits are used for service ID mask
typedef enum
{
   __DATATYPE(0, INUG_SERVICE_SYSTEM_E,   INUG_DATA_TYPE_SYS_CEVA_A_APP_E),
   __DATATYPE(1, INUG_SERVICE_SYSTEM_E,   INUG_DATA_TYPE_SYS_CEVA_B_APP_E),
   __DATATYPE(2, INUG_SERVICE_SYSTEM_E,   INUG_DATA_TYPE_SYS_LUT_E),
   __DATATYPE(3, INUG_SERVICE_SYSTEM_E,   INUG_DATA_TYPE_SYS_FLASH_E),
   __DATATYPE(4, INUG_SERVICE_SYSTEM_E,   INUG_DATA_TYPE_SYS_WRITE_BUFF_E),

   //Logger Service
   __DATATYPE(0, INUG_SERVICE_LOGGER_E,   INUG_DATA_TYPE_LOG_E),
   INUG_DATA_TYPE_LAST_E
} INUG_dataTypeE;

#define POS_DATA_MAX_NUM_OF_CHANNELS (7)

typedef struct
{
   float imuVal;
   char  *name;
} INU_CMDG_posSensors_data;

typedef struct
{
   INU_CMDG_posSensors_data   imuData[INU_DEFSG_POS_SENSOR_CHANNEL_NUM_TYPES_E-1];
   UINT64                     timeStamp;
} INU_CMDG_posSensorsClientInfoT;

typedef struct
{
   float scale;
   float offset;
   UINT32 bytes;
   UINT32 bits_used;
   UINT64 mask;
   UINT32 is_signed;
   UINT32 location;
   UINT32 be;
   UINT32 shift;
   INU_DEFSG_posSensorChannelTypeE channelType;
} INU_CMDG_posDataHdrT;

typedef struct
{
   INU_CMDG_posDataHdrT hdr[INU_DEFSG_POS_SENSOR_NUM_TYPES_E][POS_DATA_MAX_NUM_OF_CHANNELS];
   UINT16 num_channels[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
   float odr[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
   float scale[INU_DEFSG_POS_SENSOR_NUM_TYPES_E];
} INU_CMDG_posStartT;

typedef struct
{
   UINT32   notused;
} INU_CMDG_posStopT;

typedef struct
{
   UINT32 sensorType;
   UINT32 frameCntr;
} INU_CMDG_sendPosDataT;

typedef struct
{
   float   temperature;
} INU_CMDG_posTemperatureT;

/****************************************************************************
*  IOCTL Command typedefs
 ****************************************************************************/
// INUG_IOCTL_SYSTEM_CHAIN_HDR_E
typedef struct
{
   UINT16                  chainHdr;
} INUG_ioctlChainHdrT;


// INUG_IOCTL_SYSTEM_DB_CONTROL_E
typedef struct
{
   INU_DEFSG_dbControlE dbControl;
} INUG_ioctlDbControlT;

// INUG_IOCTL_SYSTEM_SENSOR_SET_CFG
// INUG_IOCTL_SYSTEM_SENSOR_GET_CFG
typedef struct
{
   INU_DEFSG_senSelectE            sensorSelect;
   INU_DEFSG_sensorCmdE            sensorCmd;
   INU_DEFSG_sensorModelE          sensorModel;
   INU_DEFSG_senFunctionalityE     sensorFunc;
   INU_DEFSG_tableTypeE            tableType;
   INU_DEFSG_resolutionIdE         resolutionId;
   UINT32                          fps;
   INU_DEFSG_sensorOutFormatE      format;
   INU_DEFSG_senPowerFreqHzE       powerFreqHz;
   INU_DEFSG_sensorExpModeE        expMode;
   INU_DEFSG_sensorOffsetsT        offsets;
   UINT32                          defaultExp;           //[us]
   float                           defaultGain;
   UINT32                          totalExpTimePerSec;   //[ms/s]
   UINT32                          expTimePerDutyCycle;  //[ms/s]
   INU_DEFSG_sensorTriggerSrcE     triggerSrc;
   UINT32                          triggerDelay;
   UINT32                          isTriggerSupported;
   INU_DEFSG_sensorOrientationE    orientation;
   UINT64                          extGpio; //bitmap of gpios to toggle in addition during trigger mode
   UINT32                          extGpioDelay; //delay in us when to trigger the extGpio
   UINT16                          groupId;  //which group this sensor belongs
   UINT16                          sensorId; //sensor number inside the group
   UINT32                          i2cNum;
   UINT32                          powerGpioMaster;
   UINT32                          powerGpioSlave;
   UINT32                          fsinGpio;
   UINT32                          sensorClkSrc;
   UINT32                          sensorClkDiv;
   INU_DEFSG_strobesListE          strobeNum;
   UINT32                          senWidth;
   UINT32                          senHeight;
   UINT8                           isSensorGroup;
   UINT8                           projector_inst;
   UINT32                          cropStartX;
   UINT32                          cropStartY;
} INUG_ioctlSensorConfigT;

typedef struct
{
   INU_DEFSG_projSelectE               projectorSelect;
   INU_DEFSG_projectorModeE            state;
   INU_DEFSG_projectorTypeE            type;
   INU_DEFSG_projectorModelE           model;
} INUG_ioctlProjectorConfigT;


// INUG_IOCTL_SYSTEM_SENSOR_EXPOSURE_SET_CFG
// INUG_IOCTL_SYSTEM_SENSOR_EXPOSURE_GET_CFG
typedef struct
{
   INU_DEFSG_senSelectE          sensorSelect;
   UINT32                        exposureTime[INU_DEFSG_SENSOR_NUM_CONTEXTS]; //[us]
   UINT8                         stereo;
} INUG_ioctlSensorExposureTimeConfigT;

// INUG_IOCTL_SYSTEM_SENSOR_GAIN_SET_CFG
// INUG_IOCTL_SYSTEM_SENSOR_GAIN_GET_CFG
typedef struct
{
   INU_DEFSG_senSelectE          sensorSelect;
   UINT32                        analogGain[INU_DEFSG_SENSOR_NUM_CONTEXTS];
   UINT32                        digitalGain[INU_DEFSG_SENSOR_NUM_CONTEXTS];
   UINT8                         stereo;
} INUG_ioctlSensorGainConfigT;

// INUG_IOCTL_SYSTEM_SENSOR_FPS_SET_CFG
typedef struct
{
   INU_DEFSG_senSelectE          sensorSelect;
   UINT32                        fps;
} INUG_ioctlSensorFpsConfigT;

// INUG_IOCTL_SYSTEM_ALTERNATE_SET_CONFIG_E
typedef struct
{
   UINT32   alternateModeEnable;
   UINT32   modeANumFrames;
   UINT32   modeBNumFrames;
} INUG_ioctlAlternateConfigT;

// INUG_IOCTL_LOGGER_SET_PARAMS_E
// INUG_IOCTL_LOGGER_GET_PARAMS_E
typedef struct
{
   INU_DEFSG_logLevelE  hostLogSeverity;
   BOOLEAN              hostPrintLogs;
   BOOLEAN              hostSendLogs;
   BOOLEAN              hostPrintGpLogs;
   BOOLEAN              hostSendGpLogs;
   INU_DEFSG_logLevelE  gpLogSeverity;
   BOOLEAN              gpPrintLogs;
   BOOLEAN              gpSendLogs;
   BOOLEAN              gpSendCdcLogs;
} INUG_ioctlLoggerSetParamsT;

//INUG_IOCTL_SYSTEM_CALIBRATION_RD_E
typedef struct
{
   UINT32                  calibrationRdSize;
   UINT8                  *calibrationRdBufP;
} INUG_ioctlCalibrationRdT;

//INUG_IOCTL_SYSTEM_BUFFER_RD_E
typedef struct
{
   UINT32                  startAddress;//Start address for reading on target (e.g. DDR)
   UINT32                  size;
   UINT32                  *buffP; //pointer to buffer/file at host
} INUG_ioctlBufferRdT;

//INUG_IOCTL_SYSTEM_CALIBRATION_HDR_RD_E
typedef struct
{
   UINT32   calibrationDataSize;
   INUG_calibrationDataFormatE   calibrationDataFormat;
   UINT64   calibrationTimestamp;
} INUG_ioctlCalibrationHdrRdT;

typedef struct
{
   UINT8    calibType;
} INUG_ioctlEraseDynCalibrationT;

//INUG_IOCTL_SYSTEM_BOOT_SET_MODE_E
typedef struct
{
   InuG_StrapOverrideE   bootSetMode;
} INUG_ioctlBootSetModeT;

//INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E
typedef struct
{
   INU_DEFSG_projSelectE projSelect;
   INU_DEFSG_projectorModeE   projSetMode;
} INUG_ioctlProjectorSetModeT;

typedef struct
{
   INU_DEFSG_projSelectE      projSelect;
   INU_DEFSG_projectorModeE   projGetMode;
} INUG_ioctlProjectorGetModeT;

//INUG_IOCTL_SYSTEM_GET_MODE_E
typedef struct
{
   INUG_configMetaT metaData;
} INUG_ioctlSysGetModeT;

//INUG_IOCTL_SYSTEM_SET_MODE_E
typedef struct
{
   CONFG_modeE            configMode;
} INUG_ioctlSysSetModeT;

//INUG_IOCTL_SYSTEM_UART2LOOPBACK_PROBE_E
typedef struct
{
   BOOLEAN                loopback;
} INUG_ioctlSysUart2LbProbeT;

//INUG_IOCTL_SYSTEM_CALIB_PATH_E
typedef struct
{
   char                   **calibrationPath;
   UINT16                   numOfPaths;
} INUG_ioctlCalibrationPathInfoT;

//INUG_IOCTL_INJECT_SET_NOTIFY_CB_E
typedef struct
{
   INUG_writeNotifyDoneCb     notifyCb;
} INUG_ioctlInjectSetNotifyCbT;

typedef struct
{
   UINT32               hdr;
   UINT32               length;
   UINT8                *dataP;
} INUG_ioctlClientDataInT;

typedef struct
{
   INU_DEFSG_senSelectE side;
   UINT16 x;      // x-coordinate of top left corner of rectangle
   UINT16 y;      // y-coordinate of top left corner of rectangle
   UINT16 width;  // Width  in the x-direction.
   UINT16 height; // Height in the y-direction.
} INUG_ioctlSensorCtrlSetRoiT;

typedef struct
{
   UINT32               *bufP;
   UINT32               destAddress;
} INUG_bufferAccessT;

#ifdef __cplusplus
}
#endif
#endif //INU_CMD_H
