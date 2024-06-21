#ifndef _INU_DEFS_H_
#define _INU_DEFS_H_

#include "inu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

//#define USE_TPP

#define INU_DEFSG_SYS_CLK_A0_MHZ    660
#define INU_DEFSG_SYS_CLK_B0_MHZ    696
#define INU_DEFSG_USB_3_CLK_MHZ     500
#define INU_DEFSG_USB_2_CLK_MHZ     480
// find DSP clock value
#define INU_DEFSG_DSP_CLK_MHZ       240

#define INU_DEFSG_NANO_SEC_TICKS_IN_SEC   1000000000

#define VA_PACK(fmt, argsString)                   \
{                                                  \
      va_list  args;                               \
      va_start(args,fmt);                          \
      vsprintf(argsString ,fmt, args);             \
      va_end(args);                                \
}

typedef UINT32 RET_CODE;
typedef void* IO_HANDLE;

typedef enum
{
   INUG_SERVICE_LINK_CONNECT_E    = 0,
   INUG_SERVICE_LINK_DISCONNECT_E = 1,
} INUG_serviceLinkEventE;

typedef enum{
   INU_DEFSG_YUV16_12BIT_TO_Y8,   //input format is (packed 16bit) 12bit luma; output format is luma 8bit
   INU_DEFSG_YUV16_10BIT_TO_Y8,   //input format is (packed 16bit) 10bit luma; output format is luma 8bit
   INU_DEFSG_RGB565_TO_Y8,        //input format is (packed 16bit) RGB:5,6,5 output format is luma 8bit
   INU_DEFSG_VYUY_TO_Y8,          //input format is (packed 32bit) is 8bit V, 8bit Y1, 8bit U and 8bit Y2. output format is only luma (Y1Y2)

   INU_DEFSG_INPUT16_OUTPUT8_MARKER = INU_DEFSG_VYUY_TO_Y8, //marker

   INU_DEFSG_Y8_TO_YUV16_12BIT,   //input format is Luma 8bit; output format is grayscale (packed 16bit) 12bit luma
   INU_DEFSG_Y8_TO_YUV16_10BIT,   //input format is Luma 8bit; output format is grayscale (packed 16bit) 10bit luma

   INU_DEFSG_VYUY16_TO_RGB888,   //input format is (packed 32bit per 2 pixels) 8bit V, 8bit Y1, 8bit U and 8bit Y2. output format is RGB 8,8,8
   INU_DEFSG_IR_TO_Y8_SCALE_DOWN_4,

   INU_DEFSG_NUM_OF_FORMAT_TYPES
} INU_DEFSG_formatConvertE;

typedef enum
{
   SUCCESS_E = 0,
   FAIL_E    = 1
} INU_DEFSG_passFailE;

typedef enum
{
    WRITE_ACCESS_E = 0,
    READ_ACCESS_E = 1
} INU_DEFSG_accessTypeE;


// SYSTEM CONFIGURATION
//------------------------
typedef enum
{
   INU_DEFSG_BOOT0_E    = 0,
   INU_DEFSG_BOOT10_E   = 10,
   INU_DEFSG_BOOT20_E   = 20,
   INU_DEFSG_BOOT21_E   = 21,
   INU_DEFSG_BOOT22_E   = 22,
   INU_DEFSG_BOOT23_E   = 23,
   INU_DEFSG_BOOT24_E   = 24,
   INU_DEFSG_BOOT30_E   = 30,
   INU_DEFSG_BOOT40_E   = 40,
   INU_DEFSG_BOOT41_E   = 41,
   INU_DEFSG_BOOT50_E   = 50,
   INU_DEFSG_BOOT51_E   = 51,
   INU_DEFSG_BOOT52_E   = 52,
   INU_DEFSG_BOOT53_E   = 53,
   INU_DEFSG_BOOT55_E   = 55,
   INU_DEFSG_BOOT60_E   = 60,
   INU_DEFSG_BOOT65_E   = 65,
   INU_DEFSG_BOOT66_E   = 66,
   INU_DEFSG_BOOT70_E   = 70,
   INU_DEFSG_BOOT80_E   = 80,
   INU_DEFSG_BOOT82_E   = 82,
   INU_DEFSG_BOOT83_E   = 83,
   INU_DEFSG_BOOT84_E   = 84,
   INU_DEFSG_BOOT85_E   = 85,
   INU_DEFSG_BOOT86_E   = 86,
   INU_DEFSG_BOOT87_E   = 87,
   INU_DEFSG_BOOT90_E   = 90,
   INU_DEFSG_BOOT200_E  = 200,
   INU_DEFSG_BOOT310_E  = 310,
   INU_DEFSG_BOOT311_E  = 311,
   INU_DEFSG_BOOT330_E  = 330,
   INU_DEFSG_BOOT340_E  = 340,
   INU_DEFSG_BOOT360_E  = 360,
   INU_DEFSG_BOOT400_E  = 400,
   INU_DEFSG_BOOT401_E  = 401,
   INU_DEFSG_BOOT500_E  = 500,
   INU_DEFSG_BOOT501_E  = 501,
   INU_DEFSG_BOOT510_E  = 510,
   INU_DEFSG_BOOT520_E  = 520,
   INU_DEFSG_BOOT600_E  = 600,
   INU_DEFSG_BOOT700_E	= 700,
   INU_DEFSG_BOOT701_E	= 701,
   INU_DEFSG_BOOT702_E	= 702,
   INU_DEFSG_BOOT711_E	= 711,
   INU_DEFSG_BOOT800_E	= 800,
   INU_DEFSG_BOOT900_E	= 900,
   INU_DEFSG_NUM_OF_PROJECTS_E
} INU_DEFSG_moduleTypeE;

#define INU_DEFSG_BOOT_FACTOR (10000)

typedef enum
{
   INU_DEFSG_BASE_VERSION_1_E = 0,
   INU_DEFSG_BASE_VERSION_2_E = 1,
   INU_DEFSG_BASE_VERSION_3_E = 2,
   INU_DEFSG_NUM_OF_BASE_VERSIONS_E
} INU_DEFSG_baseVersionE;

#ifdef __UART_ON_FPGA__
// Compile for USB gadget
#define DEFSG_IS_UART_MIPI
#else
//#define DEFSG_IS_USB_BULK
#define DEFSG_IS_MULTI_USB
#endif
// In case we want to use UART platform we need to comment this line below
//#define DEFSG_IS_UART_USB
//Enables experimental lowlevel feature for sleep of iae/dpe during blanking in cde driver
//#define DEFSG_EXPERIMENTAL_BLANKING_SLEEP
// Enables activation of CEVA by ICC

#define INU_DEFSG_REGULAR_PIXEL_SIZE      1
#define INU_DEFSG_YUV_PIXEL_SIZE          2

typedef enum
{
   INU_DEFSG_STREAM_METHOD_USER_E = 0,
   INU_DEFSG_STREAM_METHOD_AUTO_E = 1,
   INU_DEFSG_NUM_OF_STREAM_METHODS_E
} INU_DEFSG_streamMethodE;

typedef enum {
   INU_DEFSG_SINGLE_BUFFER = 1,
   INU_DEFSG_DOUBLE_BUFFER = 2,
   INU_DEFSG_TRIPLE_BUFFER = 3,
   INU_DEFSG_QUAD_BUFFER = 4,
   INU_DEFSG_QUINTUPLE_BUFFER = 5,
   INU_DEFSG_SEXTUPLE_BUFFER = 6,
   INU_DEFSG_SEPTUPLE_BUFFER = 7,
   INU_DEFSG_OCTUPLE_BUFFER = 8,
   INU_DEFSG_NONUPLE_BUFFER = 9,
   INU_DEFSG_DEC_BUFFER = 10,
   INU_DEFSG_RESERVED = 11,
   INU_DEFSG_RESERVED2 = 12,
   INU_DEFSG_THIRTEEN_BUFFER = 13,
   INU_DEFSG_MAX_NUM_CYC_BUFFERS = INU_DEFSG_THIRTEEN_BUFFER
} INU_DEFSG_cyclicBuffersE;

#define INU_DEFSG_NUM_EYES                2

#define INU_DEFSG_IOCTL_PROJECTOR_ID_LENGTH        (4)

#define INU_DEFSG_IOCTL_SENSOR_ID_LENGTH        (4)
#define INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH   (16)
#define INU_DEFSG_IOCTL_EXT_PRODUCTION_STRING_LENGTH   (40)
#define INU_DEFSG_IOCTL_BOOTFIX_STRING_LENGTH       (32)

// VGA resolution
#define INU_DEFSG_VGA_WIDTH               (648)
#define INU_DEFSG_VGA_HEIGHT              (488)

// SVGA resolution
#define INU_DEFSG_SVGA_WIDTH                (808)
#define INU_DEFSG_SVGA_HEIGHT               (608)

// USIF resolution
#define INU_DEFSG_USIF_WIDTH              (1064)
#define INU_DEFSG_USIF_HEIGHT             (728)

// HD resolution
#define INU_DEFSG_HD_WIDTH                (1288)
#define INU_DEFSG_HD_HEIGHT               (968)

// FULL HD resolution
#define INU_DEFSG_FULL_HD_WIDTH                (1608)
#define INU_DEFSG_FULL_HD_HEIGHT               (1208)

// Definition for download IAE LUT table
#define INU_DEFSG_IAE_LUT_TABLE_HDR_LEN   (128)

// Definition for flash ioctl command
#define INU_DEFSG_MAX_FLASH_IOCTL_BUF_SIZE (1024)
#define INU_DEFSG_FLASH_SIZE                (128) *(65536)

typedef enum
{
   INU_DEFSG_USB_EVT_REMOVAL     = 0,
   INU_DEFSG_USB_EVT_INSERTION       = 1,
   INU_DEFSG_USB_EVT_BOOT_COMPLETE   = 2,
   INU_DEFSG_USB_EVT_BOOT_FAILED     = 3
} INU_DEFSG_usbStatusE;

typedef enum
{
   INU_DEFSG_USB_EVT_RAW_REMOVAL     = 0,
   INU_DEFSG_USB_EVT_RAW_INSERTION    = 1,
} INU_DEFSG_usbRawStatusE;

typedef enum
{
   INVALID_PID    = 0,
   HOST_ID        = 1,
   GP_ID          = 2,
   CEVA_ID        = 3,
   EV61_ID        = 4,
   IAF_ID         = 5,
   CLIENT_GP_ID   = 6,
   CLIENT_CEVA_ID = 7,
   NUM_OF_PROCESSORS    = IAF_ID
} INU_DEFSG_pidE;

typedef enum {
   LOG_ERROR_E = 0,
   LOG_WARN_E  = 1,
   LOG_INFO_E  = 2,
   LOG_DEBUG_E = 3,
   LOG_LAST_E
} INU_DEFSG_logLevelE;

typedef enum
{
   INVALID_MODULE_ID = 0x00000000,
   SVC_MNGR          = 0x00000001,
   SVC_COM           = 0x00000002,
   IAE               = 0x00000003,
   DPE               = 0x00000004,
   CDE               = 0x00000005,
   CONN_LYR          = 0x00000006,
   CONFIG            = 0x00000007,
   INU_FDK           = 0x00000008,
   INU_COMM          = 0x00000009,
   INU_LM            = 0x0000000A,
   INU_DATA          = 0x0000000B,
   SENSOR            = 0x0000000C,
   HW_MNGR           = 0x0000000D,
   PAL               = 0x0000000E,
   INIT              = 0x0000000F,
   INU_DEVICE        = 0x00000010,
   CIREG             = 0x00000011,
   INU_FUNCTION      = 0x00000012,
   INU_GRAPH         = 0x00000013,
   MEM_POOL          = 0x00000014,
   INU_LOGGER        = 0x00000015,
   I2C               = 0x00000016,
   DATA_BASE         = 0x00000017,
   MEM_MAP           = 0x00000018,
   OS_LYR            = 0x00000019,
   HW_REGS           = 0x0000001A,
   MSG_BOX           = 0x0000001B,
   SERIAL            = 0x0000001C,
   INU_REF           = 0x0000001D,
   QUEUE_LYR         = 0x0000001E,
   TCP_COM           = 0x0000001F,
   UDP_COM           = 0x00000020,
   UART_COM          = 0x00000021,
   MSGBOX_COM        = 0x00000022,
   GME               = 0x00000023,
   CEVA_BOOT         = 0x00000024,
   INU_SENSORS_GROUP = 0x00000025,
   USB_CTRL_COM      = 0x00000026,
   SYSTEM            = 0x00000027,
   GPIO              = 0x00000028,
   INU_STREAMER      = 0x00000029,
   SPI               = 0x0000002A,
   SPI_FLASH         = 0x0000002B,
   INU_NODE          = 0x0000002C,
   IPC_CTRL_COM      = 0x0000002D,
   INU_FACTORY       = 0x0000002E,
   INU_IMU_DATA      = 0x0000002F,
   INU_IMU           = 0x00000030,
   INU_TEMPERATURE_DATA= 0x00000031,
   INU_TEMPERATURE   = 0x00000032,
   HCG_MNGR          = 0x00000033,
   ICC               = 0x00000034,
   SENSORS_MNGR      = 0x00000035,
   IAE_MNGR          = 0x00000036,
   DPE_MNGR          = 0x00000037,
   CDE_MNGR          = 0x00000038,
   GME_MNGR          = 0x00000039,
   INU_MIPI_CHANNEL  = 0x0000003a,
   MIPI              = 0x0000003b,
   MIPI_COM          = 0x0000003c,
   ALT               = 0x0000003d,
   MEM_MNGR          = 0x0000003e,
   INU_PP            = 0x0000003f,
   PROJ              = 0x00000040,
   XMLDB             = 0x00000041,
   NUCFG             = 0x00000042,
   PPE               = 0x00000043,
   PPE_MNGR          = 0x00000044,
   CLIENT_API        = 0x00000045,
   CALIBRATION       = 0x00000046,
   CVA_MNGR          = 0x00000047,
   PSS               = 0x00000048,
   EV_CNN_BOOT       = 0x00000049,
   INU_SOC_CHANNEL   = 0x0000004a,
   INU_IMAGE         = 0x0000004b,
   INU_SOC_WRITER    = 0x0000004c,
   INU_HISTOGRAM     = 0x0000004d,
   INU_SENSOR_CONTROL= 0x0000004e,
   INU_SENSOR        = 0x0000004f,
   INU_FAST_ORB      = 0x00000050,
   INU_CVA_CHANNEL   = 0x00000051,
   INU_CVA_DATA      = 0x00000052,
   INU_CDNN          = 0x00000053,
   INU_LOAD_NETWORK  = 0x00000054,
   INU_SLAM          = 0x00000055,
   INU_TSNR_CALC     = 0x00000056,
   TRIGGER_MNGR      = 0x00000057,
   INU_FDK_DSO       = 0x00000058,
   INU_RAW_DATA      = 0x00000059,
   INU_FAST_ORB_DATA = 0x0000005a,
   INU_CDNN_DATA     = 0x0000005b,
   INU_SLAM_DATA     = 0x0000005c,
   INU_TSNR_DATA     = 0x0000005d,
   INU_HISTOGRAM_DATA= 0x0000005e,
   PVT               = 0x0000006f,
   INU_COMPRESS      = 0x00000060,
   INU_DPE_PP        = 0x00000061,
   SOCKET_MOD        = 0x00000062,
   NAND              = 0x00000063,
   INU_POINT_CLOUD_DATA = 0x00000064,
   CDC_COM           = 0x00000066,
   INU_LOAD_BACKGROUND = 0x00000067,
   INU_VISION_PROC   = 0x00000068,
   ISP_MNGR          = 0x00000069,
   PWR_MNGR          = 0x0000006a,
   INU_ISP_CHANNEL   = 0x0000006b,
   CDC_MNGR          = 0x0000006c,
   VE_MNGR           = 0x0000006d,
   UVC_COM           = 0x0000006e,
   INU_FW_UPDATE     = 0x0000006f,
   INU_METADATA      = 0x00000070,
   INU_SENSORSYNC     = 0x00000071,
   INU_METADATA_UART = 0x00000072,
   INU_METADATA_INJECT = 0x00000073,

  /* Add new modules here */
   MODULE_ID__MAX_E = INU_FW_UPDATE
} INU_DEFSG_moduleIdE;

typedef enum
{
   INU_DEFSG_SENSOR_RESET_E      = 0,
   INU_DEFSG_SENSOR_INIT_E       = 1,
   INU_DEFSG_SENSOR_ACTIVATE_E   = 2,
   INU_DEFSG_SENSOR_CONFIG_E     = 3
} INU_DEFSG_sensorCmdE;

typedef enum
{
   INU_DEFSG_SENSOR_MODEL_NONE_E          = 0,
   INU_DEFSG_SENSOR_MODEL_AMS_CGSS130_E   = 130,
   INU_DEFSG_SENSOR_MODEL_AR_134_E        = 134,
   INU_DEFSG_SENSOR_MODEL_AR_135_E        = 135,
   INU_DEFSG_SENSOR_MODEL_AR_135X_E       = 136,
   INU_DEFSG_SENSOR_MODEL_AR_430_E        = 430,
   INU_DEFSG_SENSOR_MODEL_AR_0234_E       = 234,
   INU_DEFSG_SENSOR_MODEL_AR_2020_E       = 2020,
   INU_DEFSG_SENSOR_MODEL_AR_2021_E       = 2021, //virtual sensor , dual resolution for sensor 2020
   INU_DEFSG_SENSOR_MODEL_APTINA_1040_E   = 1040,
   INU_DEFSG_SENSOR_MODEL_OV_7251_E       = 7251,
   INU_DEFSG_SENSOR_MODEL_OV_2685_E       = 2685,
   INU_DEFSG_SENSOR_MODEL_GC_2145_E       = 2145,
   INU_DEFSG_SENSOR_MODEL_GC_2053_E       = 2053,
   INU_DEFSG_SENSOR_MODEL_XC_9160_E       = 9160,
   INU_DEFSG_SENSOR_MODEL_OV_9282_E       = 9282,
   INU_DEFSG_SENSOR_MODEL_OV_5675_E       = 5675,
   INU_DEFSG_SENSOR_MODEL_OV_8856_E       = 8856,
   INU_DEFSG_SENSOR_MODEL_OV_4689_E       = 4689,
   INU_DEFSG_SENSOR_MODEL_CGS_132_E       = 132,
   INU_DEFSG_SENSOR_MODEL_OV_9782_E       = 9782,
   INU_DEFSG_SENSOR_MODEL_CGS_031_E       = 031,
   INU_DEFSG_SENSOR_MODEL_VD56G3_E        = 563,
   INU_DEFSG_SENSOR_MODEL_VD55G0_E        = 550,
   INU_DEFSG_SENSOR_MODEL_OS05A10_E       = 510,
   INU_DEFSG_SENSOR_MODEL_GENERIC_E       = 9998,
   INU_DEFSG_SENSOR_MODEL_AUTO_DETECT_E   = 9999
} INU_DEFSG_sensorModelE;

typedef enum
{
   INU_DEFSG_CAMERA_MODE_DUMMY_E    = 0,
   INU_DEFSG_CAMERA_MODE_SENSOR_E   = 1,
   INU_DEFSG_CAMERA_MODE_INJECT_E   = 2,
   INU_DEFSG_CAMERA_MODE_GENERATE_E = 3
} INU_DEFSG_cameraModeE;

typedef enum
{
   INU_DEFSG_RES_DUMMY_E            = 0,
   INU_DEFSG_RES_VGA_E              = 1,
   INU_DEFSG_RES_VERTICAL_BINNING_E = 2,
   INU_DEFSG_RES_HD_E               = 3,
   INU_DEFSG_RES_FULL_HD_E          = 4,
   INU_DEFSG_RES_USER_DEF_E         = 5,
   INU_DEFSG_RES_SVGA_E             = 6
} INU_DEFSG_resolutionIdE;

typedef enum
{
   INU_DEFSG_ROLE_LEFT_E = 0,
   INU_DEFSG_ROLE_RIGHT_E = 1,
   INU_DEFSG_ROLE_COLOR_E = 2
} INU_DEFSG_sensorRoleE;

typedef enum
{
   INU_DEFSG_STEREO_E = 0,
   INU_DEFSG_MONO_E = 1,
   INU_DEFSG_STEREO_2_DIFF_SENSORS_E = 2,
} INU_DEFSG_tableTypeE;

typedef enum
{
   INU_DEFSG_INJECT_MODE_SINGLE_SENSOR_0_E   = 0,
   INU_DEFSG_INJECT_MODE_SINGLE_SENSOR_1_E   = 1,
   INU_DEFSG_INJECT_MODE_SINGLE_SENSOR_2_E   = 2,
   INU_DEFSG_INJECT_MODE_STEREO_SENSORS_E    = 3,
   INU_DEFSG_INJECT_MODE_MIPI_E              = 4,
   INU_DEFSG_INJECT_MODE_DDR_E               = 5,
   INU_DEFSG_NUM_OF_INJECT_MODES_E           = 6
} INU_DEFSG_injecModeE;

typedef enum
{
   INU_DEFSG_FR_DUMMY_E = 0,
   INU_DEFSG_FR_1_E     = 1,
   INU_DEFSG_FR_2_E     = 2,
   INU_DEFSG_FR_3_E     = 3,
   INU_DEFSG_FR_4_E     = 4,
   INU_DEFSG_FR_5_E     = 5,
   INU_DEFSG_FR_10_E    = 10,
   INU_DEFSG_FR_15_E    = 15,
   INU_DEFSG_FR_20_E    = 20,
   INU_DEFSG_FR_25_E    = 25,
   INU_DEFSG_FR_30_E    = 30,
   INU_DEFSG_FR_32_E    = 32,
   INU_DEFSG_FR_35_E    = 35,
   INU_DEFSG_FR_40_E    = 40,
   INU_DEFSG_FR_45_E    = 45,
   INU_DEFSG_FR_50_E    = 50,
   INU_DEFSG_FR_55_E    = 55,
   INU_DEFSG_FR_60_E    = 60
} INU_DEFSG_frameRateIdE;

typedef enum
{
   INU_DEFSG_FRAME_MODE_DUMMY_E           = 0,
   INU_DEFSG_FRAME_MODE_NONE_INTERLEAVE_E = 1,
   INU_DEFSG_FRAME_MODE_INTERLEAVE_E      = 2
} INU_DEFSG_frameModeIdE;

typedef enum
{
   INU_DEFSG_NUM_IMGS_NONE_INTERLEAVE_E   = 1,
   INU_DEFSG_NUM_IMGS_INTERLEAVE_E        = 2
} INU_DEFSG_imagesInFrameE;

typedef enum
{
   INU_DEFSG_SENSOR_FORMAT_DUMMY_E       = 0,
   INU_DEFSG_SENSOR_BAYER_RAW_10_E       = 1,
   INU_DEFSG_SENSOR_BAYER_RAW_8_2_POST_E = 2,
   INU_DEFSG_SENSOR_BAYER_RAW_8_2_PRE_E  = 3,
   INU_DEFSG_SENSOR_RGB_565_E            = 4
} INU_DEFSG_sensorOutFormatE;

typedef enum
{
   INU_DEFSG_POWER_FREQ_DUMMY_E     = 0,
   INU_DEFSG_SEN_POWER_FREQ_50_HZ_E = 50,
   INU_DEFSG_SEN_POWER_FREQ_60_HZ_E = 60
} INU_DEFSG_senPowerFreqHzE;

typedef enum
{
   INU_DEFSG_SENSOR_SYS_STATE_ENTER_CONFIG_CHANGE   = 0x28,
   INU_DEFSG_SENSOR_SYS_STATE_STREAMING             = 0x31,
   INU_DEFSG_SENSOR_SYS_STATE_ENTER_STREAMING       = 0x34,
   INU_DEFSG_SENSOR_SYS_STATE_ENTER_SUSPEND         = 0x40,
   INU_DEFSG_SENSOR_SYS_STATE_SUSPENDED             = 0x41,
   INU_DEFSG_SENSOR_SYS_STATE_ENTER_STANDBY         = 0x50,
   INU_DEFSG_SENSOR_SYS_STATE_STANDBY               = 0x52,
   INU_DEFSG_SENSOR_SYS_STATE_LEAVE_STANDBY         = 0x54
} INU_DEFSG_sensorStateE;

typedef enum
{
   INU_DEFSG_AE_MANUAL_E = 0,
   INU_DEFSG_AE_AUTO_E
} INU_DEFSG_sensorExpModeE;

typedef enum
{
   INU_DEFSG_SEN_MASTER_E  = 0,
   INU_DEFSG_SEN_SLAVE_E   = 1,
   INU_DEFSG_SEN_SINGLE_E  = 2,
   INU_DEFSG_SENS_NUM_FUNCTIONS_E
} INU_DEFSG_senFunctionalityE;

typedef enum
{
   INU_DEFSG_SENSOR_TYPE_SINGLE_E   = 0,
   INU_DEFSG_SENSOR_TYPE_STEREO_E   = 1
} INU_DEFSG_sensorTypeE;

typedef enum
{
   INU_DEFSG_SENSOR_TRIGGER_DISABLE_E = 0,
   INU_DEFSG_SENSOR_TRIGGER_ENABLE_E = 1
} INU_DEFSG_sensorTriggerModeE;

typedef enum
{
   INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E = 0,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER1_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER2_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_HW_TIMER_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM1_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM2_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM3_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU0_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU1_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU2_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU3_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU4_E,
   INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU5_E,
   INU_DEFSG_SENSOR_TRIGGER_AUTO_E,
   /*FSG channel to FSG timer mapping:
      FSG 0 -> FSG counter 0
      FSG 1 -> FSG counter 1
      FSG 2 -> FSG counter 2 
      FSG 3 -> There's no mapping used to the trigger manager code will need to be modified to support this trigger mode
      FSG 4 -> There's no mapping used to the trigger manager code will need to be modified to support this trigger mode
      FSG 5 -> There's no mapping used to the trigger manager code will need to be modified to support this trigger mode 
   */
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_0,
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_1,
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_2,
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_3,
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_4,
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_5,
   /*FSG triggering but enabled by another sensor/stream. 
   The use-case for this is when we have two sensors that use the same trigger
   but we want the trigger to only start when both sensors are ready so that we end up with the same 
   Frame IDs
   
   Another use case is if you want to have a test tool or sandbox control when the triggers starts, which is useful if you have two NU4Ks*/
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_0, 
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_1,        
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_2, 
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_3,          
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_4,     
   INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_5,                            
} INU_DEFSG_sensorTriggerSrcE;


typedef enum
{
   INU_DEFSG_POJECTOR_MODE_DISABLE_E = 0,
   INU_DEFSG_POJECTOR_MODE_LOW_E     = 1,
   INU_DEFSG_POJECTOR_MODE_HIGH_E    = 2
} INU_DEFSG_projectorModeE;

typedef enum
{
   INU_DEFSG_POJECTOR_TYPE_PATTERN_E = 0,
   INU_DEFSG_POJECTOR_TYPE_FLOOD_E     = 1,
   INU_DEFSG_POJECTOR_TYPE_PATTERN_FLOOD_E    = 2
} INU_DEFSG_projectorTypeE;

typedef enum
{
   INU_DEFSG_POJECTOR_MODEL_INUITIVE_E = 0,
   INU_DEFSG_POJECTOR_MODEL_AMS_E     = 1,
} INU_DEFSG_projectorModelE;


typedef enum
{
   INU_DEFSG_PROJ_0_E = 0,
   INU_DEFSG_PROJ_1_E = 1,
   INU_DEFSG_PROJ_2_E = 2,
   INU_DEFSG_NUM_OF_PROJECTORS
} INU_DEFSG_projSelectE;

typedef enum
{
   INU_DEFSG_SENSOR_ORIGINAL_E = 0,
   INU_DEFSG_SENSOR_MIRROR_E = 1,
   INU_DEFSG_SENSOR_FLIP_E = 2,
   INU_DEFSG_SENSOR_MIRROR_FLIP_E = 3,
} INU_DEFSG_sensorOrientationE;

typedef enum
{
   INU_DEFSG_SENSOR_0_E = 0,
   INU_DEFSG_SENSOR_1_E = 1,
   INU_DEFSG_SENSOR_2_E = 2,
   INU_DEFSG_SENSOR_3_E = 3,
   INU_DEFSG_SENSOR_4_E = 4,
   INU_DEFSG_SENSOR_5_E = 5,
   INU_DEFSG_SENSOR_6_E = 6,
   INU_DEFSG_SENSOR_7_E = 7,
   INU_DEFSG_SENSOR_8_E = 8,
   INU_DEFSG_SENSOR_9_E = 9,
   INU_DEFSG_SENSOR_10_E = 10,
   INU_DEFSG_SENSOR_11_E = 11,
   INU_DEFSG_NUM_OF_INPUT_SENSORS
} INU_DEFSG_senSelectE;

typedef enum
{
   INU_DEFSG_STROBE1 = 0,
   INU_DEFSG_STROBE2,
   INU_DEFSG_STROBE3,
   INU_DEFSG_STROBE4,
   INU_DEFSG_STROBE5,
   INU_DEFSG_STROBE6,
   INU_DEFSG_STROBES_NUM,
   INU_DEFSG_STROBES_INVALID
}INU_DEFSG_strobesListE;

typedef enum
{
   INU_DEFSG_SENSOR_CONTEX_A        = 0,
   INU_DEFSG_SENSOR_CONTEX_B           ,
   INU_DEFSG_SENSOR_NUM_CONTEXTS
} INU_DEFSG_sensorContextE;

typedef enum
{
   INU_DEFSG_IAU_0_E = 0,
   INU_DEFSG_IAU_1_E = 1,
   INU_DEFSG_NUM_OF_IAU_UNITS
} INU_DEFSG_iauSelectE;

typedef enum
{
   INU_DEFSG_IAM_0_E = 0,
   INU_DEFSG_IAM_1_E = 1,
   INU_DEFSG_NUM_OF_IAM_UNITS
} INU_DEFSG_iamSideE;

typedef enum
{
   INU_DEFSG_LANE_0_E = 0,
   INU_DEFSG_LANE_1_E = 1,
   INU_DEFSG_LANE_2_E = 2,
   INU_DEFSG_NUM_OF_LANES
} INU_DEFSG_laneNumE;

typedef enum
{
   INU_DEFSG_MIPI_INST_0_E= 0,
   INU_DEFSG_MIPI_INST_1_E,
   INU_DEFSG_MIPI_INST_2_E,
   INU_DEFSG_MIPI_INST_3_E,
   INU_DEFSG_MIPI_INST_4_E,
   INU_DEFSG_MIPI_INST_5_E,
   INU_DEFSG_MIPI_NUM_INSTANCES_E,
} INU_DEFSG_mipiInstE;

typedef enum
{
   INU_DEFSG_VSC_0_E = 0,
   INU_DEFSG_VSC_1_E = 1,
   INU_DEFSG_VSC_2_E = 2,
   INU_DEFSG_VSC_3_E = 3,
   INU_DEFSG_VSC_4_E = 4,

   INU_DEFSG_NUM_VSC
} INU_DEFSG_vscE;

typedef enum
{
   INU_DEFSG_MIPI_TX_SRC_VSC_0_E = 0,
   INU_DEFSG_MIPI_TX_SRC_VSC_1_E = 1,
   INU_DEFSG_MIPI_TX_SRC_VSC_2_E = 2,
   INU_DEFSG_MIPI_TX_SRC_INTERLEAVER_E = 3,

   INU_DEFSG_MIPI_TX_NUM_SOURCES
} INU_DEFSG_mipiTxSourceE;

typedef enum
{
   INU_DEFSG_MIPI_VI_INPUT_VSC_1_E = 0,
   INU_DEFSG_MIPI_VI_INPUT_VSC_2_E = 1,
   INU_DEFSG_MIPI_VI_INPUT_VSC_3_E = 2,
   INU_DEFSG_MIPI_VI_INPUT_VSC_4_E = 3,

   INU_DEFSG_MIPI_VI_NUM_INPUTS
} INU_DEFSG_mipiViInputE;

typedef enum
{
   INU_DEFSG_VIRT_CHAN_0_E = 0,
   INU_DEFSG_VIRT_CHAN_1_E,
   INU_DEFSG_VIRT_CHAN_2_E,
   INU_DEFSG_VIRT_CHAN_3_E,

} INU_DEFSG_mipiVirtChanE;

typedef enum
{
   INU_DEFSG_DB_CONTROL_ACTIVATE_E,
   INU_DEFSG_DB_CONTROL_CLEAR_E
} INU_DEFSG_dbControlE;

typedef enum
{
   INU_DEFSG_IAE_CH_VID_0_E   = 0,
   INU_DEFSG_IAE_CH_VID_1_E   = 1,
   INU_DEFSG_IAE_CH_VID_2_E   = 2,
   INU_DEFSG_IAE_CH_VID_3_E   = 3,
   INU_DEFSG_IAE_CH_VID_4_E   = 4,
   INU_DEFSG_IAE_CH_VID_5_E   = 5,
   INU_DEFSG_IAE_CH_VID_6_E   = 6,
   INU_DEFSG_IAE_CH_WEBCAM_E  = 7,
   INU_DEFSG_IAE_CH_INJECT_E  = 8,
   INU_DEFSG_IAE_NUM_OF_CHANNELS_E
} INU_DEFSG_iaeChanIdE;

typedef enum
{
   INU_DEFSG_DPE_CH_DEPTH_E   = 0,
   INU_DEFSG_DPE_CH_STATS_E   = 1,
   INU_DEFSG_DPE_NUM_OF_CHANNELS_E
} INU_DEFSG_dpeChanIdE;

typedef enum
{
   CONFG_BINNNG       = 0,
   CONFG_FULL         = 1,
   CONFG_ALTERNATING  = 2,
   CONFG_VERT_BINNING = 3,
   CONFG_USER         = 4,
} CONFG_modeE;

#define INU_DEFSG_FIRST_IAE_CHANNEL_E  INU_DEFSG_IAE_CH_VID_0_E
#define INU_DEFSG_FIRST_DPE_CHANNEL_E  INU_DEFSG_IAE_NUM_OF_CHANNELS_E

typedef  enum
{
   INU_DEFSG_SYS_CH_0_VIDEO_ID_E     = INU_DEFSG_FIRST_IAE_CHANNEL_E,
   INU_DEFSG_SYS_CH_1_VIDEO_ID_E,
   INU_DEFSG_SYS_CH_2_VIDEO_ID_E,
   INU_DEFSG_SYS_CH_3_VIDEO_ID_E,
   INU_DEFSG_SYS_CH_4_VIDEO_ID_E,
   INU_DEFSG_SYS_CH_5_VIDEO_ID_E,
   INU_DEFSG_SYS_CH_6_VIDEO_ID_E,
   INU_DEFSG_SYS_CH_7_WEBCAM_ID_E,
   INU_DEFSG_SYS_CH_8_INJECT_ID_E,
   INU_DEFSG_SYS_CH_9_DEPTH_ID_E       = INU_DEFSG_FIRST_DPE_CHANNEL_E,
   INU_DEFSG_SYS_CH_10_DPE_STATS_ID_E,
   INU_DEFSG_NUM_OF_HW_CHANNELS_E,
   INU_DEFSG_SYS_CH_11_LUT_ID_E        = INU_DEFSG_NUM_OF_HW_CHANNELS_E,
   INU_DEFSG_SYS_CH_12_HISTO_ID_E,
   INU_DEFSG_SYS_CH_13_MIPI_ID_E,
   INU_DEFSG_SYS_CH_14_CLIENT_ID_E,
   INU_DEFSG_SYS_CH_15_I2S_ID_E,
   INU_DEFSG_NUM_OF_SYS_CHANNELS_E
} INU_DEFSG_sysChannelIdE;

#define setLutModeAB(lutId,mode) (lutId | mode<<31);
#define getLutModeAB(lutId)      (lutId>>31 & 0x1);
#define getLutIdAB(lutId)        (lutId & ~(1 << 31));
#define INU_DEFSG_INVALID        (0xFFFFFFFF)

typedef enum
{
   INU_DEFSG_IAE_LUT_IB_0_0_E       = 0,
   INU_DEFSG_IAE_LUT_IB_0_1_E       = 1,
   INU_DEFSG_IAE_LUT_IB_0_2_E       = 2,
   INU_DEFSG_IAE_LUT_IB_0_3_E       = 3,
   INU_DEFSG_IAE_LUT_IB_1_0_E       = 4,
   INU_DEFSG_IAE_LUT_IB_1_1_E       = 5,
   INU_DEFSG_IAE_LUT_IB_1_2_E       = 6,
   INU_DEFSG_IAE_LUT_IB_1_3_E       = 7,
   INU_DEFSG_IAE_LUT_IB_2_0_E       = 8,
   INU_DEFSG_IAE_LUT_IB_2_1_E       = 9,
   INU_DEFSG_IAE_LUT_IB_2_2_E       = 10,
   INU_DEFSG_IAE_LUT_IB_2_3_E       = 11,
   INU_DEFSG_IAE_LUT_IB_3_0_E       = 12,
   INU_DEFSG_IAE_LUT_IB_3_1_E       = 13,
   INU_DEFSG_IAE_LUT_IB_3_2_E       = 14,
   INU_DEFSG_IAE_LUT_IB_3_3_E       = 15,
   INU_DEFSG_IAE_LUT_DSR_0_0_E      = 16,
   INU_DEFSG_IAE_LUT_DSR_0_1_E      = 17,
   INU_DEFSG_IAE_LUT_DSR_0_2_E      = 18,
   INU_DEFSG_IAE_LUT_DSR_0_3_E      = 19,
   INU_DEFSG_IAE_LUT_DSR_1_0_E      = 20,
   INU_DEFSG_IAE_LUT_DSR_1_1_E      = 21,
   INU_DEFSG_IAE_LUT_DSR_1_2_E      = 22,
   INU_DEFSG_IAE_LUT_DSR_1_3_E      = 23,
   INU_DEFSG_IAE_LUT_DSR_2_0_E      = 24,
   INU_DEFSG_IAE_LUT_DSR_2_1_E      = 25,
   INU_DEFSG_IAE_LUT_DSR_2_2_E      = 26,
   INU_DEFSG_IAE_LUT_DSR_2_3_E      = 27,
   INU_DEFSG_IAE_LUT_DSR_3_0_E      = 28,
   INU_DEFSG_IAE_LUT_DSR_3_1_E      = 29,
   INU_DEFSG_IAE_LUT_DSR_3_2_E      = 30,
   INU_DEFSG_IAE_LUT_DSR_3_3_E      = 31,
   INU_DEFSG_IAE_LUT_GCR_0_E        = 32,
   INU_DEFSG_IAE_LUT_GCR_1_E        = 33,
   INU_DEFSG_IAE_LUT_LAST_E,
   INU_DEFSG_IAE_TOTAL_LUTS_E = INU_DEFSG_IAE_LUT_LAST_E
} INU_DEFSG_iaeLutIdE;

typedef enum
{
   INU_DEFSG_IAE_HISTOGRAM_E      = 0,
   INU_DEFSG_IAE_HISTOGRAM_LAST_E
} INU_DEFSG_iaeHistIdE;

typedef struct
{
   UINT16   roiXstart;
   UINT16   roiYstart;
   UINT16   roiXend;
   UINT16   roiYend;
} INU_DEFSG_sensorOffsetsT;

typedef struct
{
   INU_DEFSG_sensorModelE     sensorType;
   INU_DEFSG_resolutionIdE    resolutionId;
   UINT32                     fps;
   INU_DEFSG_sensorOutFormatE format;
   INU_DEFSG_senPowerFreqHzE  powerFreqHz;
} INU_DEFSG_commonSensorParamsT;

typedef struct
{
   UINT16   video_width;
   UINT16   video_height;
   UINT16   depth_width;
   UINT16   depth_height;
   UINT16   webcam_width;
   UINT16   webcam_height;
   double   fc_left;
   double   cc_left_x;
   double   cc_left_y;
   double   fc_right;
   double   cc_right_x;
   double   cc_right_y;
   double   T;
   double   Nx;
   double   Ny;

} INUG_configMetaT;

typedef struct
{
   INT16  x;      // x-coordinate of top left corner of rectangle
   INT16  y;      // y-coordinate of top left corner of rectangle
   UINT16 width;  // Width  in the x-direction.
   UINT16 height; // Height in the y-direction.
} INU_DEFSG_rectT;

typedef struct
{
   INT16 x;      // x-coordinate of the point
   INT16 y;      // y-coordinate of the point
} INU_DEFSG_pointT;

//Structure Definition for download IAE LUT table
typedef struct
{
   UINT32  lutId;
   UINT8   buf[INU_DEFSG_IAE_LUT_TABLE_HDR_LEN - ( sizeof(UINT32) )];
} INU_DEFSG_lutHeaderT;

#define INU_DEFGS_FRAME_HDR_CNTR_OFFSET  (0)
#define INU_DEFGS_FRAME_HDR_CNTR_SIZE    (4)
#define INU_DEFGS_FRAME_HDR_TIMESTAMP_OFFSET  (INU_DEFGS_FRAME_HDR_CNTR_OFFSET + INU_DEFGS_FRAME_HDR_CNTR_SIZE)
#define INU_DEFGS_FRAME_HDR_TIMESTAMP_SIZE    (8)
#define INU_DEFGS_FRAME_HDR_SIZE    (12)

typedef enum
{
   INU_DEFSG_POS_SENSOR_ACCELAROMETER_E = 0,
   INU_DEFSG_POS_SENSOR_GYROSCOPE_E = 1,
   INU_DEFSG_POS_SENSOR_MAGNETOMETER_E = 2,
   INU_DEFSG_POS_SENSOR_NUM_TYPES_E
} INU_DEFSG_posSensorTypeE;

typedef enum
{
   INU_DEFSG_POS_SENSOR_ACCELAROMETER_X_E = 0,
   INU_DEFSG_POS_SENSOR_ACCELAROMETER_Y_E = 1,
   INU_DEFSG_POS_SENSOR_ACCELAROMETER_Z_E = 2,
   INU_DEFSG_POS_SENSOR_GYROSCOPE_X_E = 3,
   INU_DEFSG_POS_SENSOR_GYROSCOPE_Y_E = 4,
   INU_DEFSG_POS_SENSOR_GYROSCOPE_Z_E = 5,
   INU_DEFSG_POS_SENSOR_MAGNETOMETER_X_E = 6,
   INU_DEFSG_POS_SENSOR_MAGNETOMETER_Y_E = 7,
   INU_DEFSG_POS_SENSOR_MAGNETOMETER_Z_E = 8,
   INU_DEFSG_POS_SENSOR_TIMESTAMP_E = 9,
   INU_DEFSG_POS_SENSOR_CHANNEL_NUM_TYPES_E
} INU_DEFSG_posSensorChannelTypeE;

typedef enum
{
   INU_DEFSG_CEVA_ID_A_E   = 0,
   INU_DEFSG_CEVA_ID_B_E   = 1,
   INU_DEFSG_NUM_OF_CEVA_IDS_E
} INU_DEFSG_cevaIdE;


//       DOG-FREAK interface:
//---------------------------------------
#define INU_DEFSG_DOG_FREAK_OUTPUT_BUFFER_SIZE  (70528)
#define INU_DEFSG_DOG_FREAK_OUTPUT_HEADER_SIZE  (24)

typedef enum
{
   INU_DEFSG_DOG_FREAK_NUM_OF_SENSORS_E   = 0,
   INU_DEFSG_DOG_FREAK_ENABLE_FREAK_E     = 1,
   INU_DEFSG_DOG_FREAK_REPETITION_FACTOR_E = 2,
   INU_DEFSG_DOG_FREAK_NUM_OF_PARAM_E
} INU_DEFSG_dogFreakParams_E;

#ifdef __cplusplus
}
#endif

#endif   // _INU_DEFS_H_
