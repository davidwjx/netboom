#ifndef __INU_DEVICE_API_H__
#define __INU_DEVICE_API_H__

#include "inu2.h"

#define INU_DEVICE__MAX_NUM_OF_REFS (100)
#define VERSION_STRING_SIZE (32)
typedef enum
{
   INU_DEVICE__CHIP_REVISION_NU3000_A_E = 0x0000,
   INU_DEVICE__CHIP_REVISION_NU3000_B_E = 0x3000,
   INU_DEVICE__CHIP_REVISION_NU4000_E   = 0x4000,
   INU_DEVICE__CHIP_REVISION_NU4100_E   = 0x4100,
   INU_DEVICE__NUM_OF_CHIP_REVISIONS_E
} inu_device__chipRevisionIdE;

typedef enum
{
   INU_DEVICE__CHIP_VERSION_A_E = 0,
   INU_DEVICE__CHIP_VERSION_B_E = 1,
   INU_DEVICE__CHIP_VERSION_C_E = 2,
   INU_DEVICE__CHIP_VERSION_C1_E = 3,
   INU_DEVICE__NUM_OF_CHIP_VERSIONS_E
} inu_device__chipVersionIdIdE;

typedef struct
{
   inu_device__chipVersionIdIdE versionId : 16;
   inu_device__chipRevisionIdE  revisionId : 16;
} inu_device__hwVersionIdT;

typedef union
{
   UINT32                   val;
   inu_device__hwVersionIdT fields;
} inu_device__hwVersionIdU;

typedef struct
{
   UINT32 major:     4;  // 0-15
   UINT32 minor:     6;  // 0-63
   UINT32 build:     8;  // 0-255
   UINT32 subBuild:  14; // 0-16383
} inu_device__fwVersionIdT;

typedef union
{
   UINT32                     val;
   inu_device__fwVersionIdT   fields;
} inu_device__fwVersionIdU;

typedef enum
{
   INU_DEFSG_USB_NONE_E    = 0,
   INU_DEFSG_USB_1_E       = 1,
   INU_DEFSG_USB_2_E       = 2,
   INU_DEFSG_USB_3_E       = 3
} inu_device__usbSpeedE;

typedef enum inu_device_interface_0_e
{
   INU_DEVICE_INTERFACE_0_USB = 0,
   INU_DEVICE_INTERFACE_0_UART,
   INU_DEVICE_INTERFACE_0_TCP,
   INU_DEVICE_INTERFACE_0_CDC,
   INU_DEVICE_INTERFACE_0_STUB
}inu_device_interface_0_e;

typedef enum
{
   INU_DEVICE__NORMAL_MODE = 0,
   INU_DEVICE__STANDALONE_MODE
 }inu_device_standalone_mode_e;

typedef enum
{
   INU_DEVICE__DISCONNECT = 0,
   INU_DEVICE__CONNECT,
}inu_device_connect_state_e;

typedef void(*inu_event_callback)(inu_deviceH deviceH, inu_device_connect_state_e);

typedef enum
{
   INU_DEVICE__IAE_LUT_IB_0_0_E = 0,
   INU_DEVICE__IAE_LUT_IB_0_1_E = 1,
   INU_DEVICE__IAE_LUT_IB_0_2_E = 2,
   INU_DEVICE__IAE_LUT_IB_0_3_E = 3,
   INU_DEVICE__IAE_LUT_IB_1_0_E = 4,
   INU_DEVICE__IAE_LUT_IB_1_1_E = 5,
   INU_DEVICE__IAE_LUT_IB_1_2_E = 6,
   INU_DEVICE__IAE_LUT_IB_1_3_E = 7,
   INU_DEVICE__IAE_LUT_IB_2_0_E = 8,
   INU_DEVICE__IAE_LUT_IB_2_1_E = 9,
   INU_DEVICE__IAE_LUT_IB_2_2_E = 10,
   INU_DEVICE__IAE_LUT_IB_2_3_E = 11,
   INU_DEVICE__IAE_LUT_IB_3_0_E = 12,
   INU_DEVICE__IAE_LUT_IB_3_1_E = 13,
   INU_DEVICE__IAE_LUT_IB_3_2_E = 14,
   INU_DEVICE__IAE_LUT_IB_3_3_E = 15,
   INU_DEVICE__IAE_LUT_GCR_0_E = 16,
   INU_DEVICE__IAE_LUT_GCR_1_E = 17,
   INU_DEVICE__IAE_LUT_DSR_0_0_E = 18,
   INU_DEVICE__IAE_LUT_DSR_0_1_E = 19,
   INU_DEVICE__IAE_LUT_DSR_0_2_E = 20,
   INU_DEVICE__IAE_LUT_DSR_0_3_E = 21,
   INU_DEVICE__IAE_LUT_DSR_1_0_E = 22,
   INU_DEVICE__IAE_LUT_DSR_1_1_E = 23,
   INU_DEVICE__IAE_LUT_DSR_1_2_E = 24,
   INU_DEVICE__IAE_LUT_DSR_1_3_E = 25,
   INU_DEVICE__IAE_LUT_DSR_2_0_E = 26,
   INU_DEVICE__IAE_LUT_DSR_2_1_E = 27,
   INU_DEVICE__IAE_LUT_DSR_2_2_E = 28,
   INU_DEVICE__IAE_LUT_DSR_2_3_E = 29,
   INU_DEVICE__IAE_LUT_DSR_3_0_E = 30,
   INU_DEVICE__IAE_LUT_DSR_3_1_E = 31,
   INU_DEVICE__IAE_LUT_DSR_3_2_E = 32,
   INU_DEVICE__IAE_LUT_DSR_3_3_E = 33,
   INU_DEVICE__IAE_LUT_LAST_E,
   INU_DEVICE__IAE_TOTAL_LUTS_E = INU_DEVICE__IAE_LUT_LAST_E
} inu_device_iaeLutIdE;

typedef enum
{
   INU_DEVICE__SET_TIME_ONE_SHOT_E          = 0,
   INU_DEVICE__SET_TIME_PLL_LOCK_USB_E      = 1,
   INU_DEVICE__SET_TIME_PLL_LOCK_UART_E     = 2,
   INU_DEVICE__SET_TIME_PLL_LOCK_SPI_E      = 3,
   INU_DEVICE__SET_TIME_PLL_LOCK_DISABLE_E  = 4,
   INU_DEVICE__SET_TIME_PLL_LOCK_RTC_E      = 5,
   INU_DEVICE__SET_TIME_PLL_LOCK_RTC_PPS_Mode = 6,       /*In this mode, the RTC will wait for a PPS before updating the coarse time*/
   INU_DEVICE__NUM_OF_SET_TIME_METHODS_E
} inu_device_setTimeMethodE;

typedef enum
{
   INU_DEVICE__RTC_INACTIVE_E                  = 0,
   INU_DEVICE__RTC_LOCK_IN_PROGRESS_E          = 1,
   INU_DEVICE__RTC_LOCK_DONE_E                 = 2,
} inu_device_rtcLockStatusE;


typedef struct
{
   UINT32 calib_data_size;
   INT32 calib_format;
   UINT64 calib_timestamp;
} inu_device_calibration_header_t;

typedef struct
{
   char *data;
   UINT32 len;
} inu_device_calibration_data_t;

typedef struct
{
   UINT32 rsvd;
} inu_device_usb_setup_t;

typedef struct
{
   UINT32   portNumber;
   UINT32   baudrate;
} inu_device_uart_setup_t;

typedef struct
{
   UINT32   portNumber;
   UINT32   baudrate;
} inu_device_cdc_setup_t;

typedef struct
{
   char    targetIp[20];
   char    hostIp[20];
   UINT32  port;
} inu_device_tcp_setup_t;

typedef union interfaceUnion
{
   inu_device_usb_setup_t usb;
   inu_device_uart_setup_t uart;
   inu_device_tcp_setup_t tcp;
   inu_device_cdc_setup_t cdc;
} interfaceUnion;

typedef struct
{
   enum inu_device_interface_0_e select;
   interfaceUnion interfaceParams;
} inu_device_interface_0_t;

typedef struct
{
   char               *bufP;
   UINT32              bufsize;
   char               *path;
} inu_device_memory_t;

//Structure Definition for download IAE LUT table
typedef struct
{
   UINT32  lutId;
   UINT32  sensorInd;
   UINT32  dblBuffId;
   UINT8   buf[128 - 3*(sizeof(UINT32))]; //todo - remove
} inu_device_lut_hdr_t;

typedef struct
{
   inu_device_setTimeMethodE method;
   INT32 seconds;
   INT32 microSeconds;
   UINT32 secondHostResolution;
   UINT32 uartBaudRate;
   UINT32 startTick;
   char uartPortName[MAX_UART_PORT_NAME_LEN];
   UINT32 debugEnable;
} inu_device__set_time_t;

typedef void inu_device__getUsecTimeFuncT(UINT64* usecP);

typedef struct
{
   UINT64 usec;
} inu_device__get_time_t;

typedef struct 
{
   UINT8 lockStatus;
} inu_device__get_rtc_lock_t;

typedef struct
{
   UINT32 sec;
} inu_device__watchdog_timeout_t;

typedef struct
{
   char *buf;
   UINT32 len;
   UINT32 address;
} inu_device__buff_info_t;

typedef enum
{
   INU_DEVICE__REG_SOC_E     = 0,
   INU_DEVICE__REG_I2C_E     = 1,
   INU_DEVICE__REG_ATT_E     = 2,
   INU_DEVICE__REG_WAIT_E    = 3,
   INU_DEVICE__REG_DPHY_E    = 4,
   INU_DEVICE__REG_TEMP_E     = 5,
} inu_device__regTypeE;

typedef enum
{
   INU_DEVICE__DB_PHASE_0_E  = 0,
   INU_DEVICE__DB_PHASE_1_E  = 1,
   INU_DEVICE__DB_PHASE_2_E  = 2,
   INU_DEVICE__DB_PHASE_A_E  = 0xA,
   INU_DEVICE__DB_PHASE_B_E  = 0xB,
   INU_DEVICE__DB_PHASE_C_E  = 0xC,
   INU_DEVICE__DB_PHASE_D_E  = 0xD,
   INU_DEVICE__DB_PHASE_E_E  = 0xE,
   INU_DEVICE__DB_PHASE_F_E  = 0xF
} inu_device__dbPhaseE;

typedef enum
{
   INU_DEVICE__I2C_0_E    = 0,
   INU_DEVICE__I2C_1_E    = 1,
   INU_DEVICE__I2C_2_E    = 2,
   INU_DEVICE__I2C_3_E    = 3,
   INU_DEVICE__I2C_4_E    = 4,
   INU_DEVICE__I2C_5_E    = 5,
   INU_DEVICE__I2C_NUM_E
} inu_device__i2cNumE;


typedef enum
{
   INU_DEVICE_POJECTOR_DISABLE_E      = 0,
   INU_DEVICE_POJECTOR_PATTERN_LOW_E  = 1,
   INU_DEVICE_POJECTOR_PATTERN_HIGH_E = 2,
   INU_DEVICE_POJECTOR_FLOOD_LOW_E    = 3,
   INU_DEVICE_POJECTOR_FLOOD_HIGH_E   = 4,
} inu_device__projectorModeE;

typedef enum
{
   INU_DEVICE_PROJ_0_E = 0,
   INU_DEVICE_PROJ_1_E = 1,
   INU_DEVICE_PROJ_2_E = 2,
   INU_DEVICE_NUM_OF_INPUT_PROJECTORS
} inu_device__projSelectE;

typedef struct
{
   inu_device__projSelectE     projSelect;
   inu_device__projectorModeE  projSetMode;
} inu_device__projectorSetModeT;

typedef struct
{
   inu_device__projSelectE      projSelect;
   inu_device__projectorModeE   projGetMode;
} inu_device__projectorGetModeT;

typedef enum
{
   INU_DEVICE_PWM_0_E         = 0,
   INU_DEVICE_PWM_1_E         = 1,
   INU_DEVICE_PWM_2_E         = 2,
   INU_DEVICE_NUM_PWM_E       = 3,
}inu_device__pwmNumE;

typedef enum
{
   INU_DEVICE_PWM_CONFIG_E         = 0,
   INU_DEVICE_PWM_CONFIG_START_E   = 1,
   INU_DEVICE_PWM_STOP_E           = 2,
}inu_device__pwmCmdE;

typedef struct
{
   inu_device__pwmNumE pwmNum;
   UINT32              fps;
   UINT32              widthInUsec;
   inu_device__pwmCmdE pwmCmd;
}inu_device__pwmConfigT;

typedef struct
{
   UINT32   dblBuffId;
}inu_device__calibUpdateT;

typedef struct
{
   UINT32 disableUnits;
} inu_device__powerModeT;

typedef struct
{
  UINT32 addr; //physical address
  UINT32 val;  //value to read or write
} inu_device__socRegT;

typedef struct
{
   inu_device__i2cNumE i2cNum;
   UINT32              i2cData;
   UINT32              regWidth;
   UINT32              regAddress;
   UINT8               tarAddress;
   UINT8               accessSize;
} inu_device__i2cRegT;

typedef enum
{
   INU_DEVICE_EEPROM_READ_E        = 0,
   INU_DEVICE_EEPROM_WRITE_E       = 1,
}inu_device__eepromOperationE;

typedef struct
{
   UINT32                         sensorId;
   inu_device__eepromOperationE   operation;
   UINT32                         size;
   UINT8                          *data;
}inu_device__eepromAccessT;

typedef struct
{
   inu_device__i2cRegT            i2cInfo;
   UINT32                         eepromSize;
}inu_device__eepromInfoT;

typedef enum
{
   INU_DEVICE_POWER_DOWN_FIRST_BOOT_E       = 0,
   INU_DEVICE_POWER_DOWN_WAIT_FOR_EVENT_E   = 1,
   INU_DEVICE_POWER_DOWN_PREPARE_E          = 2,
   INU_DEVICE_POWER_DOWN_ENTER_E            = 3,
   INU_DEVICE_POWER_DOWN_EXIT_E             = 4,
} inu_device__powerDownCmdE;

typedef struct
{
   inu_device__powerDownCmdE     cmd;
} inu_device__powerDownT;

typedef struct
{
   UINT8     sensorNum;
} inu_device__tuningParamsT;

typedef struct
{
   UINT32                      tempSensorNum; //[Us]
}inu_device__selectTempSensor_t;


typedef struct
{
   UINT8 addr;
   UINT8 val;
} inu_device__attRegT;

typedef struct
{
   UINT32               usecWait;
} inu_device__waitRegT;

typedef struct
{
    UINT32 dphyNum;
    UINT32 addr;
    UINT32 val;
}inu_device__dphyRegT;

typedef struct
{
    UINT32 addr;
    UINT32 vali;
    float  valf;
}inu_device__tempRegT;

typedef struct
{
   inu_device__regTypeE   regType;
   inu_device__socRegT    socReg;
   inu_device__i2cRegT    i2cReg;
   inu_device__attRegT    attReg;
   inu_device__waitRegT   waitReg;
   inu_device__dphyRegT   dphyReg;
   inu_device__tempRegT   tempReg;
} inu_device__readRegT;

typedef struct
{
   inu_device__regTypeE   regType;
   inu_device__dbPhaseE   phase;
   UINT32                 calRegsCntr;
   inu_device__socRegT    socReg;
   inu_device__i2cRegT    i2cReg;
   inu_device__attRegT    attReg;
   inu_device__waitRegT   waitReg;
} inu_device__writeRegT;

typedef void *inu_device__sharedMallocFuncT(unsigned int size,void *inu_node);
typedef void inu_device__sharedFreeFuncT(void *ptr,void *inu_node);

typedef struct
{
   inu_event_callback            eventCb;
   inu_device_interface_0_t      interface_0;
   const char                    *bootPath;
   inu_device_standalone_mode_e  standAloneMode;
   unsigned int                  useSharedMemory;
   inu_device__sharedMallocFuncT *sharedMallocFunc;
   inu_device__sharedFreeFuncT   *sharedFreeFunc;
} inu_device__initParams;

typedef struct
{
   INT32 deviceId;
} inu_device__params;

#define INU_DEFSG_IOCTL_SENSOR_ID_LENGTH        (4)
#define INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH   (16)
#define INU_DEFSG_IOCTL_EXT_PRODUCTION_STRING_LENGTH   (40)
#define INU_DEFSG_IOCTL_BOOTFIX_STRING_LENGTH       (32)

typedef enum
{
   INU_DEVICE__PLL_AUDIO_E,
   INU_DEVICE__PLL_CPU_E,
   INU_DEVICE__PLL_DDR_E,
   INU_DEVICE__PLL_DSP_E,
   INU_DEVICE__PLL_SYS_E,
   INU_DEVICE__PLL_USB2_E,
   INU_DEVICE__PLL_USB3_E,
   INU_DEVICE__PLL_NUM_PLLS_E
} inu_device__PLLTypesE;

typedef struct
{
    inu_device__hwVersionIdU hwVerId;
    inu_device__fwVersionIdU fwVerId;
    UINT16                   masterSensorId[INU_DEFSG_IOCTL_SENSOR_ID_LENGTH];
    UINT16                   slaveSensorId[INU_DEFSG_IOCTL_SENSOR_ID_LENGTH];
    UINT16                   ispVersion[INU_DEFSG_IOCTL_SENSOR_ID_LENGTH];
    UINT8                    isMaster;
    // General production data
    INT8                     serialNumber[INU_DEFSG_IOCTL_EXT_PRODUCTION_STRING_LENGTH];
    INT8                     modelNumber[INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH];
    INT8                     partNumber[INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH];
    INT8                     sensorRevision[INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH];
    // boot folder id - see Boot_Description.xlsx for decoding
    UINT32                   bootId;
    UINT32                   baseVersion;
    UINT64                   bootfixTimestamp;
    INT8                     bootfixVersion[INU_DEFSG_IOCTL_BOOTFIX_STRING_LENGTH];
    // interface info
    inu_device__usbSpeedE    usbSpeed;
    UINT32                   pllFreq[INU_DEVICE__PLL_NUM_PLLS_E];
} inu_device__version;

typedef struct
{
   UINT32       id;
   UINT32       refType;
}inu_device__refListEntry;

typedef struct
{
   UINT32                   numberOfRefs;
   inu_device__refListEntry refMember[INU_DEVICE__MAX_NUM_OF_REFS];
}inu_device__refList;

typedef struct
{
    UINT32                address;
    UINT32                bufLen;
    UINT32                numOfChunks;
    UINT32                chunkLen;
    UINT16                chunkId;
    char                  *buf;
}inu_device__readWriteBuf;

typedef struct
{
	// version number of Header
	unsigned int versionNumber;
	// real size of data in section
	unsigned int sectionDataSize;
	// section data format
	unsigned int sectionFormat;
	// Current date and time
	unsigned long long timestamp;
	// Version string for the current section (if supported)
	char versionString[VERSION_STRING_SIZE];
} inu_device__setflashSectionHdr;

typedef enum
{
   INU_DEVICE_PRODUCT_TYPE_NONE_E         = 0,
   INU_DEVICE_PRODUCT_TYPE_BOOTSPL_E      = 1,
   INU_DEVICE_PRODUCT_TYPE_LXKERNEL_E     = 2,
   INU_DEVICE_PRODUCT_TYPE_LXDTB_E     = 3,
   INU_DEVICE_PRODUCT_TYPE_CEVADSP_E      = 4,
   INU_DEVICE_PRODUCT_TYPE_EV62BOOT_E     = 5,
   INU_DEVICE_PRODUCT_TYPE_EV62APP_E      = 6,
   INU_DEVICE_PRODUCT_TYPE_INITRAMFS_E     = 7,
   INU_DEVICE_PRODUCT_TYPE_APPLICATION_E   = 8,
   INU_DEVICE_PRODUCT_TYPE_CNNNETWORK_E      = 9,
   INU_DEVICE_PRODUCT_TYPE_HWXML_E        = 10,
   INU_DEVICE_PRODUCT_TYPE_SWXML_E        = 11,

   INU_DEVICE_PRODUCT_TYPE_MAXIMUM_E
} inu_device__fwUpdtProdTypeE;

typedef struct
{
   UINT32 major;
   UINT32 minor;
   UINT32 build;
}inu_device__fwUpdtProdVerT;

typedef struct{
    BOOLEAN isBootfromFlash;
    BOOLEAN bootFromFactoryDefault;
    inu_device__fwUpdtProdVerT version[INU_DEVICE_PRODUCT_TYPE_MAXIMUM_E];
}inu_device__componenetsVersionT;

#define FW_UPDATE_MAX_PATH_LENGTH         (128)

struct fwUpdateElement
{
   char* fileName;
   inu_device__fwUpdtProdTypeE   type;
   inu_device__fwUpdtProdVerT version; // TODO: change to inuStorageLayoutType
};

typedef struct
{
   struct fwUpdateElement element[INU_DEVICE_PRODUCT_TYPE_MAXIMUM_E];
   UINT32 numProductsToUpdate;
}inu_device__fwUpdateHdrT;

typedef struct _inu_device__alternateProjCfgT
{
   UINT32 alternateModeEnable;
   UINT32 numFramesPattern;
   UINT32 numFramesFlood;
}inu_device__alternateProjCfgT;

#include "inu_metadata.h"
#include "inu_sensorsync.h"
typedef struct
{
   char                         buffer[INU_METADATA_MAX_SIZE];
} inu_device__sensorUpdateT;


//API functions
ERRG_codeE inu_device__usb_monitor(int(*hotplugCallbackFunc)(INU_DEFSG_usbStatusE, inu_device__hwVersionIdU), int usbId, int externalHp, int filterId, int thrdSleepUs);

ERRG_codeE inu_device__usb_monitor_deinit();
ERRG_codeE inu_device__boot_change_params(const char *bootDirPath);
ERRG_codeE inu_device__boot_hotplug_callback_external(INU_DEFSG_usbRawStatusE usbEvent, int usbFd);
ERRG_codeE inu_device__usb_boot(int(*hotplugCallbackFunc)(INU_DEFSG_usbStatusE, inu_device__hwVersionIdU), const char *bootPath, int usbId, int externalHp, int bootid_override, int filterId, int thrdSleepUs);
ERRG_codeE inu_device__boot_deinit();
ERRG_codeE inu_device__new(inu_deviceH *deviceH, inu_device__initParams *deviceInitParamsP);
void       inu_device__delete(inu_deviceH meH);
void       inu_device__getDeviceRefList(void *me);
ERRG_codeE inu_device__getDeviceInfo(inu_deviceH meH, const inu_device__version **versionP);
ERRG_codeE inu_device__getCalibrationHeader(inu_deviceH meH, inu_device_calibration_header_t *calib_header);
ERRG_codeE inu_device__getCalibrationData(inu_deviceH meH, inu_device_calibration_data_t *calib_data);
ERRG_codeE inu_device__accessEepromData(inu_deviceH meH, inu_device__eepromAccessT *eepromAccess);
ERRG_codeE inu_device__loadLut(inu_deviceH meH, inu_device_memory_t *memP, inu_device_lut_hdr_t *lutHdrP);
ERRG_codeE inu_device__loadLuts(inu_deviceH meH, UINT32 sensorMode, inu_device_memory_t *memP, inu_device_lut_hdr_t *lutHdrP);
ERRG_codeE inu_device__setTime(inu_deviceH meH, inu_device__set_time_t *time_data, inu_device__getUsecTimeFuncT* getUsecTimeFunc);
ERRG_codeE inu_device__getTime(inu_deviceH meH, inu_device__get_time_t *time_data);
ERRG_codeE inu_device__getRtcLock(inu_deviceH meH, inu_device__get_rtc_lock_t *rtc_Lock);
ERRG_codeE inu_device__setWatchdog(inu_deviceH meH, inu_device__watchdog_timeout_t *watchdog_timeoutP);
ERRG_codeE inu_device__writeRegister(inu_deviceH meH, inu_device__writeRegT *writeRegisterParamsP);
ERRG_codeE inu_device__readRegister(inu_deviceH meH, inu_device__readRegT *readRegisterParamsP);
void       inu_device__showStats(inu_deviceH meH);
ERRG_codeE inu_device__readCalib(inu_deviceH device,CONFG_modeE configMode);
ERRG_codeE inu_device__getProjectorMode(inu_deviceH meH, inu_device__projectorGetModeT *projGetModeP);
ERRG_codeE inu_device__setProjectorMode(inu_deviceH meH, inu_device__projectorSetModeT *projSetModeP);
ERRG_codeE inu_device__readBuff(inu_deviceH meH, inu_device__buff_info_t *bufP);
ERRG_codeE inu_device__writeBuff(inu_deviceH meH, inu_device__buff_info_t *bufP);
ERRG_codeE inu_device__writeCalib(inu_deviceH meH, inu_device__setflashSectionHdr *flashHdr,char *dynCalibFullName);


ERRG_codeE inu_device__writeFileToGp(inu_deviceH meH, char* fileName, char *dstFilename);
ERRG_codeE inu_device__powerDown(inu_deviceH meH, inu_device__powerDownT* powerDownParams);
ERRG_codeE inu_device__getPowerDownStatus(inu_device__powerDownCmdE* powerDownStatus);
ERRG_codeE inu_device__updatePowerDownStatus(inu_device__powerDownCmdE powerDownStatus);
void       inu_device__readLutFiles(inu_deviceH deviceH, char *calibPath, int dblBuffId);
ERRG_codeE inu_device__setPowerMode(inu_deviceH meH, inu_device__powerModeT *powerModeP);
ERRG_codeE inu_device__fwUpdate(inu_deviceH meH, inu_device__fwUpdateHdrT *updateParams);
ERRG_codeE inu_device__pwmConfig(inu_deviceH meH, inu_device__pwmConfigT *pwmConfig);
ERRG_codeE inu_device__calibUpdate(inu_deviceH meH, inu_device__calibUpdateT *calibUpdate);
ERRG_codeE inu_device__getFlashComponentsVersion(inu_device__componenetsVersionT* componentsVersion);
ERRG_codeE inu_device__alternateProjector(inu_deviceH meH, inu_device__alternateProjCfgT *cfgP);
ERRG_codeE inu_device__findNode(inu_deviceH meH, const char *userName, inu_refH *inuRefP);
void       inu_device__reloadLutFiles(inu_deviceH deviceH, char* calibPath);
/**
 * @brief Perform a sensor Update with the sensor commands contained in metadataUpdate
 *
 * @param meH INU device to perform the sensor update on
 * @param metadataUpdate Contains the list of sensors to update and for which parameters
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE inu_device__sensorUpdate(inu_deviceH meH, INU_Metadata_T* metadataUpdate);
/**
 * @brief Configures the target to use the sensor synchronization described in sensorSyncCfg
 *
 *
 * @param me device to configure
 * @param sensorSyncCfg Sensor synchronization configuration to use
 * @return Returns an error code
 */
ERRG_codeE inu_device__sensorSyncCfg(inu_deviceH me, inu_sensorSync__updateT* sensorSyncCfg);
ERRG_codeE inu_device__enableTuningServer(inu_deviceH meH, inu_device__tuningParamsT* tsParams);
ERRG_codeE inu_device__readFileFromGp(inu_deviceH meH, char* targetfileName, char* hostFileName);
ERRG_codeE inu_device__selectTempSensor(inu_deviceH meH, inu_device__selectTempSensor_t* selectTempSensor);
void inu_device__EraseDynCalibration(inu_deviceH meH);

#endif
