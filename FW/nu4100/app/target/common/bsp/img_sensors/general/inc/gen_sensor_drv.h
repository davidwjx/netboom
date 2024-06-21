/****************************************************************************
 *
 *   FileName: gen_sensor_drv.h
 *
 *   Author: Danny B.
 *
 *   Date:
 *
 *   Description: Generic sensor layer
 *
 ****************************************************************************/
#ifndef GEN_SENSOR_DRV_H
#define GEN_SENSOR_DRV_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "err_defs.h"
#include "io_pal.h"
#include "i2c_hl_drv.h"
#include "i2c_drv.h"
#include "gme_drv.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define GEN_SENSOR_DRVG_ERROR_TEST(X) \
   if (ERRG_FAILED(X))                \
   {                                  \
      return X;                       \
   }
#define GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE8 (1)
#define GEN_SENSOR_DRVG_REGISTER_ACCESS_SIZE (2)
#define GEN_SENSOR_DRVG_SENSOR_TABLE_DELAY (5 * 1000) // 5 msec delay

#define GEN_SENSOR_DRVG_WRITE_SENSOR_REG(paramsStruct, handle, Address, dataVal, accessSizeVal, retVal) \
   {                                                                                                    \
      paramsStruct.accessRegParams.rdWr = WRITE_ACCESS_E;                                               \
      paramsStruct.accessRegParams.regAdd = Address;                                                    \
      paramsStruct.accessRegParams.data = dataVal;                                                      \
      paramsStruct.accessRegParams.accessSize = accessSizeVal;                                          \
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &paramsStruct);                                  \
   }

#define GEN_SENSOR_DRVG_READ_SENSOR_REG(paramsStruct, handle, Address, accessSizeVal, retVal) \
   {                                                                                          \
      paramsStruct.accessRegParams.rdWr = READ_ACCESS_E;                                      \
      paramsStruct.accessRegParams.regAdd = Address;                                          \
      paramsStruct.accessRegParams.accessSize = accessSizeVal;                                \
      retVal = GEN_SENSOR_DRVG_accessSensorReg(handle, &paramsStruct);                        \
   }

#define GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(paramsStruct, handle, Address, dataVal, accessSizeVal, retVal) \
   {                                                                                                          \
      paramsStruct.accessRegParams.rdWr = WRITE_ACCESS_E;                                                     \
      paramsStruct.accessRegParams.regAdd = Address;                                                          \
      paramsStruct.accessRegParams.data = dataVal;                                                            \
      paramsStruct.accessRegParams.accessSize = accessSizeVal;                                                \
      retVal = GEN_SENSOR_DRVG_accessSensorReg8(handle, &paramsStruct);                                       \
   }

#define GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(paramsStruct, handle, Address, accessSizeVal, retVal) \
   {                                                                                                \
      paramsStruct.accessRegParams.rdWr = READ_ACCESS_E;                                            \
      paramsStruct.accessRegParams.regAdd = Address;                                                \
      paramsStruct.accessRegParams.accessSize = accessSizeVal;                                      \
      retVal = GEN_SENSOR_DRVG_accessSensorReg8(handle, &paramsStruct);                             \
   }

#define GEN_SENSOR_DRVG_ANALOG_GAIN_Q (4)
#define GEN_SENSOR_DRVG_DIGITAL_GAIN_Q (4)
#define GEN_SENSOR_DRVG_DelayOffset (4)

   /****************************************************************************
    ***************      G L O B A L         T Y P E D E F S     ***************
    ****************************************************************************/

   typedef struct
   {
      UINT16 regAdd;
      UINT16 accessSizeAndMsecDelay;
      UINT32 data;
   } GEN_SENSOR_DRVG_regTblParamsT;

   typedef enum
   {
      GEN_SENSOR_DRVG_INSTANCE_STATUS_DUMMY = 0,
      GEN_SENSOR_DRVG_INSTANCE_STATUS_CLOSE_E = 1,
      GEN_SENSOR_DRVG_INSTANCE_STATUS_OPEN = 2
   } GEN_SENSOR_DRVG_instanceStatusE;

   typedef enum
   {
      GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_SELF_E = 0,
      GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_SLAVE_E = 1,
      GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_BOTH_E = 2,
      GEN_SENSOR_DRVG_SENSOR_CMD_CTRL_NUM_CMDS_E
   } GEN_SENSOR_DRVG_sensorCmdCtrlE;

   typedef enum
   {
      SENSOR_OUTPUT_YUV_E = 0,
      SENSOR_OUTPUT_RGB_E = 1,
      SENSOR_OUTPUT_BAYER_E = 2
   } GEN_SENSOR_DRVG_sensorOutputFormatE;

   typedef enum
   {
      BAYER_RAW_10_E = 0,
      BAYER_PRE_LSC_8_2_E = 1,
      BAYER_POST_LSC_8_2_E = 2,
      BAYER_PROCESSED_8_E = 3
   } GEN_SENSOR_DRVG_sensorOutputBayerFormatE;

   typedef enum
   {
      RGB_565_E = 0,
      RGB_555_E = 1,
      XRGB_444_E = 2,
      RGBX_444_E = 3
   } GEN_SENSOR_DRVG_sensorOutputRgbFormatE;

   // connection IOCTL commands
   typedef enum
   {
      GEN_SENSOR_DRVG_RESET_SENSORS_IOCTL_E = 0,
      GEN_SENSOR_DRVG_INIT_SENSOER_IOCTL_E = 1,
      GEN_SENSOR_DRVG_CONFIG_SENSOR_IOCTL_E = 2,
      GEN_SENSOR_DRVG_START_SENSOR_IOCTL_E = 3,
      GEN_SENSOR_DRVG_STOP_SENSOR_IOCTL_E = 4,
      GEN_SENSOR_DRVG_ACCESS_SENSOR_REG_IOCTL_E = 5,
      GEN_SENSOR_DRVG_GET_DEVICE_ID_IOCTL_E = 6,
      GEN_SENSOR_DRVG_LOAD_PRESET_TABLE_IOCTL_E = 7,
      GEN_SENSOR_DRVG_SET_FRAME_RATE_IOCTL_E = 8,
      GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_IOCTL_E = 9,
      GEN_SENSOR_DRVG_GET_EXPOSURE_TIME_IOCTL_E = 10,
      GEN_SENSOR_DRVG_SET_EXPOSURE_MODE_IOCTL_E = 11,
      GEN_SENSOR_DRVG_SET_IMG_OFFSETS_IOCTL_E = 12,
      GEN_SENSOR_DRVG_GET_AVG_BRIGHNESS_IOCTL_E = 13,
      GEN_SENSOR_DRVG_SET_OUTPUT_FORAMT_IOCTL_E = 14,
      GEN_SENSOR_DRVG_SET_POWER_FREQUENCY_IOCTL_E = 15,
      GEN_SENSOR_DRVG_SET_STROBE_IOCTL_E = 16,
      GEN_SENSOR_DRVG_GET_PIXEL_CLOCK_IOCTL_E = 17,
      GEN_SENSOR_DRVG_SET_GAIN_IOCTL_E = 18,
      GEN_SENSOR_DRVG_GET_GAIN_IOCTL_E = 19,
      GEN_SENSOR_DRVG_TRIGGER_SENSOR_IOCTL_E = 20,
      GEN_SENSOR_DRVG_ORIENTATION_IOCTL_E = 21,
      GEN_SENSOR_DRVG_POWER_UP_SENSOR_IOCTL_E = 22,
      GEN_SENSOR_DRVG_POWER_DOWN_SENSOR_IOCTL_E = 23,
      GEN_SENSOR_DRVG_CHANGE_ADDRES_IOCTL_E = 24,
      GEN_SENSOR_DRVG_GET_CHIP_ID_IOCTL_E = 25,
      GEN_SENSOR_DRVG_TEST_PATTERN_IOCTL_E = 26,
      GEN_SENSOR_DRVG_SET_BAYER_PATTERN_IOCTL_E = 27,
      GEN_SENSOR_DRVG_SET_BLC_IOCTL_E = 28,
      GEN_SENSOR_DRVG_SET_WB_IOCTL_E = 29,
      GEN_SENSOR_DRVG_CHANGE_RESOLUTION_IOCTL_E = 30,
      GEN_SENSOR_DRVG_EXPOSURE_CTRL_IOCTL_E = 31,
      GEN_SENSOR_DRVG_GET_INIT_PARAMS_IOCTL_E = 32,
      GEN_SENSOR_DRVG_GET_FOCUS_PARAMS_IOCTL_E = 33,
      GEN_SENSOR_DRVG_SET_FOCUS_PARAMS_IOCTL_E = 34,
      GEN_SENSOR_DRVG_SET_CROP_WINDOW_IOCTL_E  = 35, /*Specific for AR2020, adjusts the crop window*/
      GEN_SENSOR_DRVG_GET_CROP_WINDOW_IOCTL_E  = 36, /*Specific for AR2020, gets the crop window*/
      GEN_SENSOR_DRVG_GET_TEMPERATURE_IOCTL_E  = 37,
      GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE_IOCTL_E = 38,
      GEN_SENSOR_DRVG_NUM_OF_IOCTLS_E
   } GEN_SENSOR_DRVG_ioctlListE;

   typedef enum
   {
      GEN_SENSOR_DRVG_RESET_SENSORS_E = GEN_SENSOR_DRVG_RESET_SENSORS_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SENSOR_INIT_E = GEN_SENSOR_DRVG_INIT_SENSOER_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_CONFIG_SENSOR_E = GEN_SENSOR_DRVG_CONFIG_SENSOR_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_START_SENSOR_E = GEN_SENSOR_DRVG_START_SENSOR_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_STOP_SENSOR_E = GEN_SENSOR_DRVG_STOP_SENSOR_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_ACCESS_SENSOR_REG_E = GEN_SENSOR_DRVG_ACCESS_SENSOR_REG_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_DEVICE_ID_E = GEN_SENSOR_DRVG_GET_DEVICE_ID_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_LOAD_PRESET_TABLE_E = GEN_SENSOR_DRVG_LOAD_PRESET_TABLE_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_FRAME_RATE_E = GEN_SENSOR_DRVG_SET_FRAME_RATE_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E = GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_EXPOSURE_TIME_E = GEN_SENSOR_DRVG_GET_EXPOSURE_TIME_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_EXPOSURE_MODE_E = GEN_SENSOR_DRVG_SET_EXPOSURE_MODE_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_IMG_OFFSETS_E = GEN_SENSOR_DRVG_SET_IMG_OFFSETS_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_AVG_BRIGHTNESS_E = GEN_SENSOR_DRVG_GET_AVG_BRIGHNESS_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_OUTPUT_FORAMT_E = GEN_SENSOR_DRVG_SET_OUTPUT_FORAMT_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_POWER_FREQUENCY_E = GEN_SENSOR_DRVG_SET_POWER_FREQUENCY_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_STROBE_TIME_E = GEN_SENSOR_DRVG_SET_STROBE_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_PIXEL_CLOCK_E = GEN_SENSOR_DRVG_GET_PIXEL_CLOCK_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_GAIN_E = GEN_SENSOR_DRVG_SET_GAIN_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_GAIN_E = GEN_SENSOR_DRVG_GET_GAIN_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_TRIGGER_SENSOR_E = GEN_SENSOR_DRVG_TRIGGER_SENSOR_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_ORIENTATION_E = GEN_SENSOR_DRVG_ORIENTATION_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_POWER_UP_SENSOR_E = GEN_SENSOR_DRVG_POWER_UP_SENSOR_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_POWER_DOWN_SENSOR_E = GEN_SENSOR_DRVG_POWER_DOWN_SENSOR_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_CHANGE_ADDRES_E = GEN_SENSOR_DRVG_CHANGE_ADDRES_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_CHIP_ID_E = GEN_SENSOR_DRVG_GET_CHIP_ID_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_TEST_PATTERN_E = GEN_SENSOR_DRVG_TEST_PATTERN_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_BAYER_PATTERN_E = GEN_SENSOR_DRVG_SET_BAYER_PATTERN_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_BLC_E = GEN_SENSOR_DRVG_SET_BLC_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_WB_E = GEN_SENSOR_DRVG_SET_WB_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_CHANGE_RESOLUTION_E = GEN_SENSOR_DRVG_CHANGE_RESOLUTION_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_EXPOSURE_CTRL_E = GEN_SENSOR_DRVG_EXPOSURE_CTRL_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_INIT_PARAMS_E = GEN_SENSOR_DRVG_GET_INIT_PARAMS_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_FOCUS_PARAMS_E = GEN_SENSOR_DRVG_GET_FOCUS_PARAMS_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_FOCUS_PARAMS_E = GEN_SENSOR_DRVG_SET_FOCUS_PARAMS_IOCTL_E | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_CROP_WINDOW_CTRL_E    = GEN_SENSOR_DRVG_SET_CROP_WINDOW_IOCTL_E       | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_CROP_WINDOW_CTRL_E    = GEN_SENSOR_DRVG_GET_CROP_WINDOW_IOCTL_E       | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_GET_TEMPERATURE_E         = GEN_SENSOR_DRVG_GET_TEMPERATURE_IOCTL_E       | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE         = GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE_IOCTL_E       | IO_CMD_MASK(IO_SENSOR_CAM0_E),
      GEN_SENSOR_DRVG_DUMMY_E
   } GEN_SENSOR_DRVG_CmdE;

   typedef enum
   {
      GEN_SENSOR_DRVG_BAYER_RGGB = 0,
      GEN_SENSOR_DRVG_BAYER_GRBG = 1,
      GEN_SENSOR_DRVG_BAYER_GBRG = 2,
      GEN_SENSOR_DRVG_BAYER_BGGR = 3,
   } GEN_SENSOR_DRVG_BAYER_FMT;

   typedef struct
   {
      UINT32 data;
      UINT16 regAdd;
      UINT8 rdWr;
      UINT8 accessSize;
      GEN_SENSOR_DRVG_sensorCmdCtrlE cmdCtrl;
   } GEN_SENSOR_DRVG_accessRegParamsT;

   typedef struct
   {
      UINT16 sensorId[INU_DEFSG_IOCTL_SENSOR_ID_LENGTH];
      INT8 sensorIdStr[17];
      GEN_SENSOR_DRVG_sensorCmdCtrlE cmdCtrl;
   } GEN_SENSOR_DRVG_getDeviceIdParamsT;

   typedef struct
   {
      INU_DEFSG_resolutionIdE sensorResolution;
      UINT32 frameRate; // should be removed. but since aptina driver does not support fps change (only by table) keeping it.
      GEN_SENSOR_DRVG_sensorOutputFormatE outputFormat;
   } GEN_SENSOR_DRVG_loadTableParamsT;

   typedef struct
   {
      UINT32 frameRate;
      INU_DEFSG_senSelectE sensorSelect;
   } GEN_SENSOR_DRVG_setFrameRateParamsT;

   typedef struct
   {
      INT32 dac;
      INT32 minDac;
      INT32 maxDac;
      UINT32 chipId;
      INU_DEFSG_sensorContextE context;
   } GEN_SENSOR_DRVG_focusParamsT;

   typedef struct
   {
      UINT32 exposureTime; // in us
      INU_DEFSG_sensorContextE context;
   } GEN_SENSOR_DRVG_exposureTimeCfgT;

   typedef struct
   {
      INU_DEFSG_sensorExpModeE expMode;
   } GEN_SENSOR_DRVG_setExposureModeParamsT;

   typedef struct
   {
      UINT16 roiXstart;
      UINT16 roiXend;
      UINT16 roiYstart;
      UINT16 roiYend;
   } GEN_SENSOR_DRVG_setImageOffsetsParamsT;

   typedef struct
   {
      UINT8 avgBrighness;
   } GEN_SENSOR_DRVG_getBrightnessParamsT;

   typedef struct
   {
      GEN_SENSOR_DRVG_sensorOutputFormatE outputFormat;
      UINT8 outputFormatSpec;
   } GEN_SENSOR_DRVG_setOutFormatParamsT;

   typedef struct
   {
      INU_DEFSG_senPowerFreqHzE powerFreq;
   } GEN_SENSOR_DRVG_setPowerFreqParamsT;

   typedef struct
   {
      INU_DEFSG_sensorTriggerSrcE triggerSrc;
      UINT32 triggerDelay;
      UINT64 gpioBitmap;
      UINT32 pulseTime;
      UINT32 isTriggerSupported;
   } GEN_SENSOR_DRVG_triggerModeParamsT;

   typedef struct
   {
      INU_DEFSG_sensorOrientationE orientation;
   } GEN_SENSOR_DRVG_orientationParamsT;

   typedef struct
   {
      float analogGain;  // Q^4
      float digitalGain; // Q^4
      INU_DEFSG_sensorContextE context;
   } GEN_SENSOR_DRVG_gainCfgT;
   /*Added for Helsinki project*/
   typedef struct
   {
      UINT32 exposureTime; // in us
      INU_DEFSG_sensorContextE context;
      GEN_SENSOR_DRVG_gainCfgT gain;
   } GEN_SENSOR_DRVG_exposureTimeCAndGainfgT;

   typedef struct
   {
      UINT16 sysClkMhz;
      UINT16 pixelClkMhz;
   } GEN_SENSOR_DRVG_sensorClocksParamsT;

typedef struct
{
   UINT32 r;
   UINT32 gr;
   UINT32 gb;
   UINT32 b;
} GEN_SENSOR_DRVG_rgbGainParamsT;

typedef struct
{
   UINT32 width;
   UINT32 height;
} GEN_SENSOR_DRVG_resolutionParamsT;
typedef enum
{
   GEN_SENSOR_DRVG_cropWindowNotCalculated = 0, /*Crop window limits haven't been read from the sensor*/
   GEN_SENSOR_DRVG_cropWindowCalculated = 1     /*Crop window limits have been read from the sensor */
} GEN_SENSOR_DRVG_cropWindowLimitsReadStatus;

   typedef struct
   {
      float integrationTime;        /* Integration time to be congifured*/
      float gain;                   /* Gain to be configured*/
      UINT8 numOfFramesToSkip;      /* The number of frames after which the new params are valid */
      float gainHwSet;              /* Actual gain set by sensor */
      float VSGainHwSet;            /* Actual gain(very short exp gain) set by sensor */
      float longGainHwSet;          /* Actual gain(long exp gain) set by sensor */
      float integrationTimeSet;     /* Actual integration time set by sensor */
      float VSIntegrationTimeSet;   /* Actaul integration time(very short exp time) set by sensor*/
      float longIntegrationTimeSet; /* Actaul integration time(long exp time) set by sensor*/
      float hdr_ratio;              /* Used as input&output hdr ratio */
   } GEN_SENSOR_DRVG_exposureParamsT;
   /*Added for Helsinki Project for the AR2020 driver*/
   typedef struct 
   {
      UINT32 startX;                /*Start Position (x) */
      UINT32 startY;                /*Start Position (y) */
   } GEN_SENSOR_DRVG_cropWindow;

   typedef struct
   {
      INT32 temperature;
   } GEN_SENSOR_DRVG_temperatureParamsT;
   typedef struct						   //Initial ISP parameters
   {
       GEN_SENSOR_DRVG_BAYER_FMT  bayerFormat;
       UINT32                  hts;
       UINT32                  sclk;
       UINT32                  frmLengthLines;
       UINT32                  minFps;
       UINT32                  maxFps;
       UINT32                  minIntegrationLine;
       UINT32                  maxIntegrationLine;
       float                   minGain;
       float                   maxGain;
       float                   aecMaxIntegrationTime;
   }GEN_SENSOR_DRVG_ispInitParamsT;

   typedef union
   {
      GEN_SENSOR_DRVG_accessRegParamsT accessRegParams;
      GEN_SENSOR_DRVG_getDeviceIdParamsT getDeviceIdParams;
      GEN_SENSOR_DRVG_loadTableParamsT loadTableParams;
      GEN_SENSOR_DRVG_setFrameRateParamsT setFrameRateParams;
      GEN_SENSOR_DRVG_setExposureModeParamsT setExposureModeParams;
      GEN_SENSOR_DRVG_setImageOffsetsParamsT setImageOffsetsParams;
      GEN_SENSOR_DRVG_getBrightnessParamsT getBrightnessParams;
      GEN_SENSOR_DRVG_setOutFormatParamsT setOutFormatParams;
      GEN_SENSOR_DRVG_setPowerFreqParamsT setPowerFreqParams;
      GEN_SENSOR_DRVG_triggerModeParamsT triggerModeParams;
      GEN_SENSOR_DRVG_orientationParamsT orientationParams;
      GEN_SENSOR_DRVG_sensorClocksParamsT sensorClocksParams;
      GEN_SENSOR_DRVG_rgbGainParamsT rgbGainParams;
      GEN_SENSOR_DRVG_resolutionParamsT resolutionParams;
      GEN_SENSOR_DRVG_exposureParamsT exposureCtrlParams;
      GEN_SENSOR_DRVG_cropWindow             cropWindow;
      GEN_SENSOR_DRVG_temperatureParamsT     temperatureParams;
      GEN_SENSOR_DRVG_ispInitParamsT ispInitParams;
   } GEN_SENSOR_DRVG_sensorParametersT;

   typedef ERRG_codeE (*SENSOR_init)(IO_PALG_apiCommandT *palP);

   typedef struct
   {
      void *i2cControllerAddress;
      I2C_HL_DRVG_instanceIdE i2cInstanceId;
      SENSOR_init sensorInitFunc;
      I2C_HL_DRVG_iscSpeedE i2cSpeed;
      INU_DEFSG_sensorModelE sensorModel;
      INU_DEFSG_sensorTypeE sensorType;
      INU_DEFSG_tableTypeE tableType;
      UINT32 powerGpioMaster;
      UINT32 powerGpioSlave;
      UINT32 fsinGpio;
      UINT32 sensorClk;
      UINT32 sensorClkDiv;
      UINT16 sensorId;
      UINT16 groupId;
      int isSensorGroup;
      UINT16 sensorSelect;
   } GEN_SENSOR_DRVG_openParametersT;

   typedef ERRG_codeE (*GEN_SENSOR_DRVG_ioctlFuncListT)(IO_HANDLE handle, void *argP);
typedef enum {
        EXP_MODE_AUTO = 0,
        EXP_MODE_FREEZE = 1,
        EXP_MODE_MANUAL = 2,
} GEN_SENSOR_DRVG_Exposure_mode_e;

   typedef struct 
{
   GEN_SENSOR_DRVG_Exposure_mode_e  exposuremode;     /*Used for VD55G0 as the exposure mode*/
   bool                             exposuremodeRead; /*True if the exposure mode has been read*/
   UINT32                              pixelClk;      /*Pixel clock*/
   UINT32                              lineLength;    /*Line length*/
   bool pixelClockAndLineLengthRead;                  /*True if the pixel clock and line length has been read*/
   UINT32                           maxExpLines;      /*Max exposure lines*/
   bool                             readMaxExpLines;  /*True if max exposure lines has been read*/
} GEN_SENSOR_DRVG_ExposureState;
typedef struct
   {
      GEN_SENSOR_DRVG_instanceStatusE deviceStatus;
      GEN_SENSOR_DRVG_ioctlFuncListT *ioctlFuncList;
      UINT16 sensorAddress;
      UINT16 pixClk;
      UINT16 sysClk;
      UINT16 tClk;
      I2C_DRVG_instanceIdE i2cInstanceId;
      I2C_HL_DRVG_iscSpeedE i2cSpeed;
      INU_DEFSG_sensorModelE sensorModel;
      INU_DEFSG_sensorTypeE sensorType;
      INU_DEFSG_tableTypeE tableType;
      void *sensorHandle;
      UINT32 powerGpioMaster;
      UINT32 fsinGpio;
      UINT32 sensorClk;
      UINT16 sensorId;
      UINT16 groupId;
      int isSensorGroup;
      UINT32 maxIntegrationTime;
      UINT32 minIntegrationTime;
      UINT32 maxAECGain;
      UINT32 minAECGain;
      UINT32 bayerPattern;
      BOOLEAN testPatternEn;
      UINT32 max_crop_x;            /*Maximum x position for cropping*/
      UINT32 max_crop_y;            /*Maximum y position for cropping*/
      GEN_SENSOR_DRVG_cropWindowLimitsReadStatus cropWindowLimitStatus;   /*Used to determine if we need to read the crop window limits or not*/
      GEN_SENSOR_DRVG_ExposureState    exposureState; /*Used as an optimization for i2c write time for exposure*/

   } GEN_SENSOR_DRVG_specificDeviceDescT;

   /****************************************************************************
    ***************     G L O B A L        D E F N I T I O N S    **************
    ****************************************************************************/
   ERRG_codeE GEN_SENSOR_DRVG_init(IO_PALG_apiCommandT *palP);
   ERRG_codeE GEN_SENSOR_DRVG_ioctlAccessReg(IO_HANDLE handle, void *pParams);
   ERRG_codeE GEN_SENSOR_DRVG_accessSensorReg8(IO_HANDLE handle, void *pParams);
   ERRG_codeE GEN_SENSOR_DRVG_accessSensorReg(IO_HANDLE handle, void *pParams);
   ERRG_codeE GEN_SENSOR_DRVG_regTableLoad8(IO_HANDLE handle, GEN_SENSOR_DRVG_regTblParamsT *tableP, UINT16 regsInTable);
   ERRG_codeE GEN_SENSOR_DRVG_regTableLoad(IO_HANDLE handle, GEN_SENSOR_DRVG_regTblParamsT *tableP, UINT16 regsInTable);
   ERRG_codeE GEN_SENSOR_DRVG_regTableRead(IO_HANDLE handle, GEN_SENSOR_DRVG_regTblParamsT *tableP, UINT16 regsInTable);
   ERRG_codeE GEN_SENSOR_DRVG_getRefClk(UINT16 *sys_clkP, GME_DRVG_hwUnitE sensorClk);
   ERRG_codeE GEN_SENSOR_DRVP_gpioInit(UINT32 gpioNum);
   ERRG_codeE GEN_SENSOR_DRVP_setSensorRefClk(INT32 clk, UINT16 div);
   UINT16 GEN_SENSOR_DRVG_getSensorPixelClk(IO_HANDLE handle);
   void *GEN_SENSOR_DRVG_getSensorSpecificHandle(IO_HANDLE sensorHandle);
   ERRG_codeE GEN_SENSOR_DRVG_stub(IO_HANDLE handle, void *pParams);

#ifdef __cplusplus
}
#endif

#endif // GEN_SENSOR_DRV_H
