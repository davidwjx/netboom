#ifdef ENABLE_UVC
#include "inu_types.h"
#include "err_defs.h"
#include "inu2.h"
#include "version.h"
#include "inu2_internal.h"
#include "inu_factory.h"
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "svc_cfg.h"
#include "mem_pool.h"
#include "marshal.h"
#include "inu_comm.h"
#include <unistd.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include "libguvc.h"
#include "ve_mngr.h"
#include "os_lyr.h"
#include "mem_pool.h"
#include "gpio_drv.h"
#include "iae_drv.h"
#include "os_lyr.h"
#include "gme_mngr.h"
#include "privacy_shutter_arrays.h"
#include "uvc_main.h"
#include "uvc_com.h"

#define DEVICE_NAME_0       "/dev/video0"
#define DEVICE_NAME_1       "/dev/video1"

#define UVC_CONTROL_EP      0
#define UVC_STREAMING_EP    1

/* Definitions for debug messages */
#define UVC_EXTENDED_CONTROLS {"config probe", "config_commit", "rate control mode",\
    "temporal scale mode", "spatial scale mode", "snr scale mode", "ltr buffer size",\
    "ltr picture", "picture type", "version", "encoder reset", "framerate",\
    "video advance", "bitrate layers", "qp steps layers", "unused", "light source", NULL}
#define UVC_REQUESTS {"current", "minimum", "maximum", "definition", "res",\
    "len", "info"}

typedef struct _AppConfig AppConfig;
typedef struct _uvc_supported_h264_ctrl uvc_supported_h264_ctrl;

static const char* UVC_STANDARD_CONTROLS[STANDARD_CTRL_NUM] = {
  [BRIGHTNESS] = "brightness",
  [CONTRAST] = "contrast",
  [HUE] = "hue",
  [SATURATION] = "saturation",
  [SHARPNESS] = "sharpness",
  [WHITE_BALANCE_TEMPERATURE] = "white_balance_temperature",
  [WHITE_BALANCE_COMPONENT] = "white_balance_component",
  [GAIN] = "gain",
  [POWER_LINES_FREQUENCY] = "power_lines_frequency",
  [HUE_AUTO] = "hue_auto",
  [WHITE_BALANCE_TEMPERATURE_AUTO] = "white_balance_temperature_auto",
  [WHITE_BALANCE_COMPONENT_AUTO] = "white_balance_component_auto",
  [DIGITAL_MULTIPLIER] = "digital_multiplier",
  [DIGITAL_MULTIPLIER_LIMIT] = "digital_multiplier_limit",
  [SCANNING_MODE] = "scanning_mode",
  [AUTOEXPOSURE_MODE] = "autoexposure_mode",
  [AUTOEXPOSURE_PRIORITY] = "autoexposure_priority",
  [EXPOSURE_TIME_ABSOLUTE] = "exposure_time_absolute",
  [EXPOSURE_TIME_RELATIVE] = "exposure_time_relative",
  [FOCUS_ABSOLUTE] = "focus_absolute",
  [FOCUS_RELATIVE] = "focus_relative",
  [IRIS_ABSOLUTE] = "iris_absolute",
  [IRIS_RELATIVE] = "iris_relative",
  [ZOOM_ABSOLUTE] = "zoom_absolute",
  [ZOOM_RELATIVE] = "zoom_relative",
  [PRIVACY] = "privacy",
  [FOCUS_AUTO] = "focus_auto",
  [VIDEO_POWER_MODE] = "video_power_mode",
  [VIDEO_CONTROL_ERROR_CODE_REQ] = "video_control_error_code_req",
  [VIDEO_STREAMING_ERROR_CODE_REQ] = "video_streaming_error_code_req",
};

static const char* deviceNames[] = {DEVICE_NAME_0, DEVICE_NAME_1};

static AppConfig           app_configs[UVC_NUM_OF_DEVICES] = {0};
static uvc_device*         myUvcDev[UVC_NUM_OF_DEVICES] = {0};
static MEM_POOLG_bufDescT  *shutterBufDescP = NULL;
static unsigned int        UVC_MAIN_fps = 10;
unsigned int      startCapturing[UVC_NUM_OF_DEVICES] = {0};
static void *uvcShtterthrd = NULL;
static void *uvcShtterOpthrd = NULL;
static int closeThreads = 0;
static OS_LYRG_msgQueT thrMsgQue;

static bool           g_is_app_started[UVC_NUM_OF_DEVICES] = {0};
static bool           g_is_uvc_started[UVC_NUM_OF_DEVICES] = {0};

static UINT32         phyErrorFinished=1;
static UINT32         lastShutterState=0xffff;
static OS_LYRG_timerT phyIntTimer;

unsigned int forcePic[UVC_NUM_OF_DEVICES];
UINT32 UVC_MAIN_sendCount[UVC_NUM_OF_DEVICES] = {0};
UINT32 UVC_MAIN_shutterBuff = NULL;

inu_streamerH uvcStreamtable[UVC_NUM_OF_DEVICES][INU_STREAMER__UVC_MAX]={0};
UINT32 UVC_MAIN_freeCount[UVC_NUM_OF_DEVICES] = {0};

int cur_ctrl_val = GET_DEF_VAL;

int currDevice = 0;

/* This is just for testing the VC and VS controls */
int cur_ctrl_vals_test[STANDARD_CTRL_NUM] = {
  [VIDEO_POWER_MODE] = 0x12345678,
  [VIDEO_CONTROL_ERROR_CODE_REQ] = 0x12345678,
  [VIDEO_STREAMING_ERROR_CODE_REQ] = 0x12345678,
};

/* Functions */
static int UVC_MAIN_startUVC();
static void UVC_MAIN_releaseBuffer (void **buffer_pointer, void *custom_data);
static void UVC_MAIN_setControl (uvc_ctrl_params * ctrl_params,
    void *gst_image_adjust_element);
static void UVC_MAIN_getControl (uvc_ctrl_params * ctrl_params, void *custom_data);
static void UVC_MAIN_setExtendedControl (uvc_ctrl_params * ctrl_params,
    void *custom_data);
static void UVC_MAIN_getExtendedControl (uvc_ctrl_params * ctrl_params,
    void *custom_data);
static void execute_get_request (uvc_ctrl_params * ctrl_params);
static void execute_h264_get_request (uvc_ctrl_params * ctrl_params,
    uvc_supported_h264_ctrl *h264_ctl);
static void extended_control_fill_info (uvc_ext_ctrl_params *
    extended_ctrl_params);
static void register_video_formats (uvc_device * device);


void UVC_MAING_SetStreamTable(void *streamH,inu_streamer__type_e streamType, unsigned int connId)
{
    unsigned int uvcInd;

    uvcInd = connId - INU_FACTORY__CONN_UVC1_E;
    if(uvcInd >= UVC_NUM_OF_DEVICES)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid UVC conn ID. UVC currently supports up to %d devices\n", UVC_NUM_OF_DEVICES);
        return;
    }

    LOGG_PRINT(LOG_INFO_E, NULL, "Added %s to uvc connection #%d at index %d\n", inu_ref__getUserName(streamH), uvcInd, streamType);

    uvcStreamtable[uvcInd][streamType] = streamH;
    if (NULL != streamH && streamType == 0 && !g_is_uvc_started[uvcInd])
    {
        UVC_MAIN_startUVC(&app_configs[uvcInd]);
    }
}

inu_streamerH *UVC_MAING_GetStreamTable(unsigned int connId)
{
    if ((connId- INU_FACTORY__CONN_UVC1_E) < INU_STREAMER__UVC_MAX)
    {
        return uvcStreamtable[connId - INU_FACTORY__CONN_UVC1_E];
    }
    else
    {
        printf("conn ID error %d\n",connId);
    }
}

void *UVC_MAING_getUvcDevice(int uvcDevice)
{
   return myUvcDev[uvcDevice];
}

void UVC_MAING_callPushDataToUvc (void *uvcDev,
      unsigned long *buffer_data, int *buf_size,
      int *bytes_used, void **buffer_pointer)
{
    dbg("+++++++++++++++++UVC_MAING_callPushDataToUvc %x %x size %d\n",*buffer_data,*buffer_pointer,*buf_size);

    uvc_push_data_to_uvc(uvcDev, buffer_data, buf_size, bytes_used, buffer_pointer);
}

/* Release Gstreamer buffer */
static void UVC_MAIN_releaseBuffer (void **buffer_pointer, void *custom_data)
{
    AppConfig *app_config = (AppConfig*)custom_data;
    unsigned int *buf = (unsigned int*)buffer_pointer;

    // printf("---------------------release buffer %x %X\n",*buf,custom_data);
    UVC_MAING_freeStream((void *)*buf, app_config->app_id);
}

/* Set standard controls requests */
static void UVC_MAIN_setControl (uvc_ctrl_params * ctrl_params, void *gst_image_adjust_element)
{
  char *value = NULL;
  const char **control_names = UVC_STANDARD_CONTROLS;
  int idx = 0;

  value = ctrl_params->data;

  /* Get each byte from the ctrl_params data */
  cur_ctrl_val = 0;
  for (idx = 0; idx < ctrl_params->length; idx++)
  {
    cur_ctrl_val = cur_ctrl_val | (value[idx] << (idx * 8));
  }

  /* Check for control selected */
  LOGG_PRINT(LOG_DEBUG_E, NULL, "Call routine to set %s (%d). Set value %d\n",
      control_names[ctrl_params->control_id], ctrl_params->control_id, cur_ctrl_val);

  switch (ctrl_params->control_id) {
    case BRIGHTNESS:
#ifdef   TEST_UVC_TWO_CAM
        if ((ctrl_params->control_id) == BRIGHTNESS)
        {
            if (cur_ctrl_val > 50)
            {
               inu_streamer__setResetUvcActive(uvcStreamtable[0][1], 0);
               inu_streamer__setResetUvcActive(uvcStreamtable[0][0], 1);
               VE_MNGRG_setBrightness(uvcStreamtable[0][0],(UINT32)cur_ctrl_val);

            }
            else
            {
               inu_streamer__setResetUvcActive(uvcStreamtable[0][0], 0);
               inu_streamer__setResetUvcActive(uvcStreamtable[0][1], 1);
                VE_MNGRG_setBrightness(uvcStreamtable[0][1],(UINT32)(cur_ctrl_val + 50));
            }
        }
#else
      VE_MNGRG_setBrightness(uvcStreamtable[0][0], (UINT32)cur_ctrl_val);
#endif
      break;
    case CONTRAST:
    case HUE:
    case SATURATION:
    case SHARPNESS:
    case WHITE_BALANCE_TEMPERATURE:
    case WHITE_BALANCE_COMPONENT:
      break;
    case GAIN:
      VE_MNGRG_setGain(uvcStreamtable[0][0], (UINT32)cur_ctrl_val);
      break;
    case POWER_LINES_FREQUENCY:
      VE_MNGRG_setPowerFreq(uvcStreamtable[0][0], (UINT32)cur_ctrl_val);
      break;
    case HUE_AUTO:
    case WHITE_BALANCE_TEMPERATURE_AUTO:
    case WHITE_BALANCE_COMPONENT_AUTO:
    case DIGITAL_MULTIPLIER:
    case DIGITAL_MULTIPLIER_LIMIT:
    case SCANNING_MODE:
      break;
    case AUTOEXPOSURE_MODE:
      VE_MNGRG_setAutoExposure(uvcStreamtable[0][0], (UINT8)cur_ctrl_val);
      break;
    case AUTOEXPOSURE_PRIORITY:
      break;
    case EXPOSURE_TIME_ABSOLUTE:
      VE_MNGRG_setExposure(uvcStreamtable[0][0], (UINT32)cur_ctrl_val);
      break;
    case EXPOSURE_TIME_RELATIVE:
    case FOCUS_ABSOLUTE:
    case FOCUS_RELATIVE:
    case IRIS_ABSOLUTE:
    case IRIS_RELATIVE:
    case ZOOM_ABSOLUTE:
    case ZOOM_RELATIVE:
    case FOCUS_AUTO:
      break;
    case VIDEO_POWER_MODE:
    case VIDEO_CONTROL_ERROR_CODE_REQ:
    case VIDEO_STREAMING_ERROR_CODE_REQ:
      cur_ctrl_vals_test[ctrl_params->control_id] = cur_ctrl_val;
    default:
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Control selected not available\n");
      return;
  }
}

static void UVC_MAIN_resetGetControllData(uvc_ctrl_params * ctrl_params, int *data)
{
   *data = 0;

   switch (ctrl_params->get_req_type) {
     case UVC_GET_CUR_REQ:
       *data = GET_CUR_VAL;
       break;
     case UVC_GET_MIN_REQ:
       *data = GET_MIN_VAL;
       break;
     case UVC_GET_MAX_REQ:
       *data = GET_MAX_VAL;
       break;
     case UVC_GET_DEF_REQ:
       *data = GET_DEF_VAL;
       break;
     case UVC_GET_RES_REQ:
       *data = GET_RES_VAL;
       break;
     case UVC_GET_LEN_REQ:
       ctrl_params->data[0] = ctrl_params->length;
       return;
     case UVC_GET_INFO_REQ:
       *data = GET_INFO_VAL;
       break;
     default:
       dbg ("Invalid get request type\n");
   }
   return;
}

/* Get standard controls requests */
static void UVC_MAIN_getControl (uvc_ctrl_params * ctrl_params, void *custom_data)
{
  const char **control_names = UVC_STANDARD_CONTROLS;
  const char *request_names[] = UVC_REQUESTS;
  int idx;

  VE_MNGRG_uvcRequestE req = ctrl_params->get_req_type == UVC_GET_CUR_REQ ? VE_MNGRG_UVC_REQ_GET_CURR_E :
                             ctrl_params->get_req_type == UVC_GET_MIN_REQ ? VE_MNGRG_UVC_REQ_GET_MIN_E :
                             ctrl_params->get_req_type == UVC_GET_MAX_REQ ? VE_MNGRG_UVC_REQ_GET_MAX_E :
                             ctrl_params->get_req_type == UVC_GET_RES_REQ ? VE_MNGRG_UVC_REQ_GET_RES_E :
                             ctrl_params->get_req_type == UVC_GET_DEF_REQ ? VE_MNGRG_UVC_REQ_GET_DEFAULT_E :
                             ctrl_params->get_req_type == UVC_GET_INFO_REQ ? VE_MNGRG_UVC_REQ_GET_INFO_E : VE_MNGRG_NUM_OF_UVC_REQUESTS_E;

  UVC_MAIN_resetGetControllData(ctrl_params, &cur_ctrl_val);

  /* LEN request was already handled by the UVC_MAIN_resetGetControllData function */
  if (ctrl_params->get_req_type == UVC_GET_LEN_REQ) {
    return;
  }

  switch (ctrl_params->control_id) {
    case BRIGHTNESS:
      VE_MNGRG_getBrightness(uvcStreamtable[0][0], req, &cur_ctrl_val);
      break;
    case CONTRAST:
    case HUE:
    case SATURATION:
    case SHARPNESS:
    case WHITE_BALANCE_TEMPERATURE:
    case WHITE_BALANCE_COMPONENT:
      break;
    case GAIN:
      VE_MNGRG_getGain(uvcStreamtable[0][0], req, &cur_ctrl_val);
      break;
    case POWER_LINES_FREQUENCY:
      VE_MNGRG_getPowerFreq(uvcStreamtable[0][0], req, &cur_ctrl_val);
      break;
    case HUE_AUTO:
    case WHITE_BALANCE_TEMPERATURE_AUTO:
    case WHITE_BALANCE_COMPONENT_AUTO:
    case DIGITAL_MULTIPLIER:
    case DIGITAL_MULTIPLIER_LIMIT:
    case SCANNING_MODE:
      break;
    case AUTOEXPOSURE_MODE:
      VE_MNGRG_getAutoExposure(uvcStreamtable[0][0], req, (UINT8*)&cur_ctrl_val);
      break;
    case AUTOEXPOSURE_PRIORITY:
      break;
    case EXPOSURE_TIME_ABSOLUTE:
      VE_MNGRG_getExposure(uvcStreamtable[0][0], req, &cur_ctrl_val);
      break;
    case EXPOSURE_TIME_RELATIVE:
    case FOCUS_ABSOLUTE:
    case FOCUS_RELATIVE:
    case IRIS_ABSOLUTE:
    case IRIS_RELATIVE:
    case ZOOM_ABSOLUTE:
    case ZOOM_RELATIVE:
      break;
    case PRIVACY:
      VE_MNGRG_getPrivacy(uvcStreamtable[0][0], req, (UINT8*)&cur_ctrl_val);
      break;
    case FOCUS_AUTO:
      break;
    case VIDEO_POWER_MODE:
    case VIDEO_CONTROL_ERROR_CODE_REQ:
    case VIDEO_STREAMING_ERROR_CODE_REQ:
      /* TODO: Here we should fill the values for each control with useful information using req as well */
      cur_ctrl_val = cur_ctrl_vals_test[ctrl_params->control_id];
      break;
    default:
      LOGG_PRINT(LOG_WARN_E, NULL, "Control selected (%d %s) not available\n",ctrl_params->control_id,control_names[ctrl_params->control_id]);
      return;
  }

  LOGG_PRINT(LOG_DEBUG_E, NULL, "GET %s (%d) %s (%d), value = %d\n", request_names[ctrl_params->get_req_type], ctrl_params->get_req_type,
                                            control_names[ctrl_params->control_id], ctrl_params->control_id, cur_ctrl_val);

  /* This writes the data as little endian, first byte of data has LSB of cur_ctrl_val */
  for (idx = 0; idx < ctrl_params->length; ++idx)
  {
    ctrl_params->data[idx] = cur_ctrl_val & 0xFF;
    cur_ctrl_val = cur_ctrl_val >> 8;
  }
}

/* Set extended controls requests */
static void
UVC_MAIN_setExtendedControl (uvc_ctrl_params * ctrl_params, void *custom_data)
{
  const char *control_names[] = UVC_EXTENDED_CONTROLS;

  /* This switch contains only the h264 controls that were implemented
     in this demo application, but you can implement any of the h264 controls
     of the UVC Specification 1.1. The library supports all of them. */
  dbg ("Call routine to set %s. Set value %f.\n",
      control_names[ctrl_params->extended_control_id], *ctrl_params->data);

  switch (ctrl_params->extended_control_id) {
    case VIDEO_CONFIG_PROBE:
    case RATE_CONTROL_MODE:
    case PICTURE_TYPE_CONTROL:
    case ENCODER_RESET:
    case XU_EXT_GENERIC_CONTROL:
      break;
    default:
      dbg ("Extended control selected not available\n");
      return;
  }
  printf ("Setting %s extended control\n", control_names[ctrl_params->extended_control_id]);
}

/* Get extended controls requests */
static void UVC_MAIN_getExtendedControl (uvc_ctrl_params * ctrl_params, void *custom_data)
{
  uvc_supported_h264_ctrl *data = NULL;
  uvc_supported_h264_ctrl *h264_ctl = NULL;
  int idx = 0;
  int is_h264 = 0;
  const char *control_names[] = UVC_EXTENDED_CONTROLS;

  is_h264 = (custom_data) ? 0 : 1;
  if (is_h264) {
    h264_ctl = (uvc_supported_h264_ctrl *) custom_data;
  }

  /* This switch contains only the h264 controls that were implemented
     in this demo application, but you can implement any of the h264 controls
     of the UVC Specification 1.1. The library supports all of them. */
  dbg ("Call routine to get %s.",
      control_names[ctrl_params->extended_control_id]);

  switch (ctrl_params->extended_control_id) {
    case VIDEO_CONFIG_PROBE:
      data = &h264_ctl[0];
      break;
    case RATE_CONTROL_MODE:
      data = &h264_ctl[1];
      break;
    case PICTURE_TYPE_CONTROL:
      data = &h264_ctl[2];
      break;
    case ENCODER_RESET:
      data = &h264_ctl[3];
      break;
    case XU_EXT_GENERIC_CONTROL:
      break;
    default:
      dbg ("Extended control selected not available\n");
      return;
  }
  printf ("Getting %s extended control\n", control_names[ctrl_params->extended_control_id]);

  if (is_h264) {
    execute_h264_get_request (ctrl_params, data);
  } else {
    execute_get_request (ctrl_params);
  }
}

/* Execute normal GET requests */
static void
execute_get_request (uvc_ctrl_params * ctrl_params)
{
  int idx = 0;
  int data = 0;
  const char *request_names[] = UVC_REQUESTS;

  /* Check the get request type */
  dbg ("Received %s get request\n", request_names[ctrl_params->get_req_type]);

  switch (ctrl_params->get_req_type) {
    case UVC_GET_CUR_REQ:
      data = cur_ctrl_val;
      break;
    case UVC_GET_MIN_REQ:
      data = GET_MIN_VAL;
      break;
    case UVC_GET_MAX_REQ:
      data = GET_MAX_VAL;
      break;
    case UVC_GET_DEF_REQ:
      data = GET_DEF_VAL;
      break;
    case UVC_GET_RES_REQ:
      data = GET_RES_VAL;
      break;
    case UVC_GET_LEN_REQ:
      ctrl_params->data[0] = ctrl_params->length;
      return;
    case UVC_GET_INFO_REQ:
      data = GET_INFO_VAL;
      break;
    default:
      dbg ("Invalid get request type\n");
      return;
  }
  printf ("%s value %d\n", request_names[ctrl_params->get_req_type], data);
  for (idx = 0; idx < ctrl_params->length; ++idx) {
    ctrl_params->data[idx] = data & 0xFF;
    data = data >> 8;
  }
}

/* Execute H264 GET requests */
static void
execute_h264_get_request (uvc_ctrl_params * ctrl_params,
    uvc_supported_h264_ctrl *h264_ctl)
{
  int idx = 0;
  uint *data = NULL;
  const char *request_names[] = UVC_REQUESTS;

  /* Check the get request type */
  dbg ("Received %s get request\n", request_names[ctrl_params->get_req_type]);

  switch (ctrl_params->get_req_type) {
    case UVC_GET_CUR_REQ:
      data = &h264_ctl->get_cur_req[0];
      break;
    case UVC_GET_MIN_REQ:
      data = &h264_ctl->get_min_req[0];
      break;
    case UVC_GET_MAX_REQ:
      data = &h264_ctl->get_max_req[0];
      break;
    case UVC_GET_DEF_REQ:
      data = &h264_ctl->get_max_req[0];
      break;
    case UVC_GET_RES_REQ:
      data = &h264_ctl->get_cur_req[0];
      break;
    case UVC_GET_LEN_REQ:
      ctrl_params->data[0] = ctrl_params->length;
      return;
    case UVC_GET_INFO_REQ:
      ctrl_params->data[0] = 0xFF;
      return;
    default:
      dbg ("Invalid get request type \n");
      return;
  }
  /* Note
   * For controls that have more than 1 byte of length, this test writes the
   * same value in each byte required by the control
   */
  for (idx = 0; idx < ctrl_params->length; ++idx) {
    ctrl_params->data[idx] = data[idx];
  }
}

/* Fill the parameters of each extended control to register */
static void
extended_control_fill_info (uvc_ext_ctrl_params * extended_ctrl_params)
{
  extended_ctrl_params[0].xu_identifier = XU_EXT_GENERIC_CONTROL;
  extended_ctrl_params[0].ext_ctrl_length = 1;
}

/* Register available video formats */
static void
register_video_formats (uvc_device * device)
{
  int num_formats = 1;

/* You can add any resolution you want according with your device,
   but you have to update the driver drivers/usb/gadget/webcam.c */

/* Resolutions and framerates supported by the YUYV color format */
  struct uvc_frame_info uvc_frames_yuyv[] = {
    {1920, 1080, {166666, 333333, 666666, 1000000, 0},},
    {0, 0, {0,},},
  };

/* Formats supported */
  struct uvc_format_info uvc_formats[] = {
    {1, V4L2_PIX_FMT_YUYV, uvc_frames_yuyv},
  };

  uvc_register_video_formats (device, &uvc_formats[0], num_formats);
}

#ifdef TEST_UVC_TWO_CAM
static ERRG_codeE VE_MNGRP_recursiveStartFunctions(inu_functionH functionH)
{
   inu_function__startParamsT startParams;
   inu_nodeH inputNode;
   ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

   if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(functionH)))
   {
      ret = inu_function__start(functionH, &startParams);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "failed starting %s\n", inu_ref__getUserName(functionH));
      }
   }

   inputNode = inu_node__getNextInputNode(functionH, NULL);

   while (inputNode)
   {
      ret = VE_MNGRP_recursiveStartFunctions(inputNode);
      inputNode = inu_node__getNextInputNode(functionH, inputNode);
   }
   return ret;
}

static ERRG_codeE VE_MNGRP_recursiveStopFunctions(inu_functionH functionH)
{
   inu_function__stopParamsT stopParams;
   inu_nodeH inputNode;
   ERRG_codeE ret = VE_MNGR__RET_SUCCESS;

   if (INU_REF__IS_FUNCTION_TYPE(inu_ref__getRefType(functionH)))
   {
      ret = inu_function__stop(functionH, &stopParams);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "failed stopping %s\n", inu_ref__getUserName(functionH));
      }
   }

   inputNode = inu_node__getNextInputNode(functionH, NULL);
   while (inputNode)
   {
      ret = VE_MNGRP_recursiveStopFunctions(inputNode);
      inputNode = inu_node__getNextInputNode(functionH, inputNode);
   }
   return ret;
}
#endif

static void prShutterCb(UINT64 timestamp, UINT64 count, void *argP)
{
   UINT8 msg[4];
   UINT32 len=4;
   int ret;

   if (thrMsgQue.mqHandle != -1) // queue was created
   {
       ret = OS_LYRG_sendMsg(&thrMsgQue, msg, 4);
   }
}

static void UVC_MAIN_handleShutter()
{
    unsigned int ind,portVal;
    unsigned char closeCamera = 0;

    GPIO_DRVG_gpioGetValParamsT  portParams;
    ERRG_codeE                   retCode;
    IO_HANDLE                    handle;

    handle = IO_PALG_getHandle(IO_GPIO_E);
    if (handle)
    {
        portParams.gpioNum = PRIVAY_GPIO_NUM;
        retCode = IO_PALG_ioctl(handle, GPIO_DRVG_GET_GPIO_VAL_CMD_E, &portParams);
        closeCamera = portParams.val;
    }
    else
    {
        closeCamera = !lastShutterState ? 0 : 1;
    }

    if ((closeCamera) || (phyErrorFinished == 0) || (startCapturing[currDevice] == 0))
    {
        //call for reset for IAE&IPE
        GME_MNGRG_shutterSeq();
        for (ind = 0; ind < INU_STREAMER__UVC_MAX; ind++)
        {
            if (uvcStreamtable[currDevice][ind] != NULL)
            {
                if (closeCamera)
                {
                    inu_streamer__setShtterActive(uvcStreamtable[currDevice][ind], 1);
                }
            }
        }
        if (lastShutterState != 0)
        {
            VE_MNGRG_stopStream(uvcStreamtable[currDevice]);
            lastShutterState = 0;
        }
    }
    else if (phyErrorFinished)
    {
        if (lastShutterState != 1)
        {
            //when we back from shutter, we have to reconfig db again since we did reset for IAE&IPE
            if (uvcStreamtable[currDevice][0] != NULL && !VE_MNGRP_anyStreamIsRunning())
            {
                inu_graph_reconfigDb(inu_node__getGraph(uvcStreamtable[currDevice][0]));
            }

            forcePic[currDevice] = 1;
            VE_MNGRG_startStream(uvcStreamtable[currDevice]);
            forcePic[currDevice] = 0;
            lastShutterState = 1;
        }

        for (ind = 0; ind < INU_STREAMER__UVC_MAX; ind++)
        {
            if (uvcStreamtable[currDevice][ind] != NULL)
            {
                inu_streamer__setShtterActive(uvcStreamtable[currDevice][ind], 0);
            }
        }
    }
}

static int UVC_MAIN_shutterOprThread(void *argP)
{
   UINT8 msg[4];
   UINT32 len=4,count;
   while (!closeThreads)
   {
      count=0;
      len=4;
      OS_LYRG_waitRecvMsg(&thrMsgQue,msg,&len);
      while (!closeThreads)
      {
         len =4;
         OS_LYRG_recvMsg(&thrMsgQue,msg,&len,10);//clean queue
         if (len == 0) // no messages
            break;
      }
      UVC_MAIN_handleShutter();
   }
}

static int UVC_MAIN_shutterThread(void *argP)
{
    MEM_POOLG_cfgT cfg;
    MEM_POOLG_handleT poolH;
    ERRG_codeE ret;
    unsigned int buffer_data,buf_size=1920*1080*2,bytes_used,ind;
    void *buffer_pointer = NULL;
    static unsigned int s_count=0;
    UINT16 *ptr;

    cfg.bufferSize = 1920*1080*2;
    cfg.numBuffers = 1;
    cfg.resetBufPtrInAlloc = 0;
    cfg.freeCb = NULL;
    cfg.freeArg = NULL;
    cfg.type = MEM_POOLG_TYPE_ALLOC_CMEM_E;
    cfg.memP = NULL;

    ret = MEM_POOLG_initPool(&poolH, &cfg);
    MEM_POOLG_alloc(poolH,MEM_POOLG_getBufSize(poolH), &shutterBufDescP);
    UVC_MAIN_shutterBuff = (UINT32)shutterBufDescP->dataP;
    LOGG_PRINT(LOG_DEBUG_E, NULL, "UVC_MAIN_shutterThread -before PrivShutterBuildFile\n\r");
    if (PRIVACY_SHUTTER_ARRAYSG_BuildFullHDFile(shutterBufDescP->dataP) == 0)
    {
        ptr = (UINT16 *)shutterBufDescP->dataP;
    }
    else
    {
        for (ind=0;ind<1920*1080/2;ind++)
        {
          ((unsigned int *)shutterBufDescP->dataP)[ind] = 0x4444;
        }
        LOGG_PRINT(LOG_INFO_E, NULL, "UVC_MAIN_shutterThread - in else if\n\r");
    }

    while (!closeThreads)
    {
        if (uvcStreamtable[currDevice][0] != NULL)
        {
            if (startCapturing[currDevice])
            {
                if (inu_streamer__isShtterActive(uvcStreamtable[currDevice][0]) || forcePic[currDevice])
                {
                    bytes_used = buf_size;
                    buffer_data=(unsigned int)shutterBufDescP->dataP;
                    buffer_pointer = (void *) buffer_data;
                    if (UVC_MAIN_freeCount[currDevice] == UVC_MAIN_sendCount[currDevice])
                    {
                        pthread_mutex_lock(&app_configs[currDevice].lock);
                        UVC_MAING_callPushDataToUvc(UVC_MAING_getUvcDevice(currDevice), (unsigned long *)&buffer_data, (int *)&buf_size, (int *)&bytes_used, (void **)&buffer_pointer);
                        UVC_MAIN_sendCount[currDevice]++;
                        pthread_mutex_unlock(&app_configs[currDevice].lock);
                    }
                }
            }
        }
        usleep(1000000 / UVC_MAIN_fps);
    }
}

void UVC_MAING_setPrivShtrFps(unsigned int fps)
{
   UVC_MAIN_fps = fps;
}

static ERRG_codeE UVC_MAIN_createUvcShutterThr()
{
   OS_LYRG_threadParams    thrParams;
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   OS_LYRG_threadParams      thrdParams;

   thrParams.func = UVC_MAIN_shutterThread;
   thrParams.id = OS_LYRG_UVC1_THREAD_ID_E;
   thrParams.event = NULL;
   thrParams.param = NULL;
   uvcShtterthrd = OS_LYRG_createThread(&thrParams);
   ret = OS_LYRG_intCtrlRegister(OS_LYRG_INT_PR_SHUTTER_E, prShutterCb, NULL);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlEnable(OS_LYRG_INT_PR_SHUTTER_E);
   }
   memset(&thrMsgQue,0,sizeof(thrMsgQue));
   strcpy(thrMsgQue.name,"/privShtrOpr");
   thrMsgQue.maxMsgs = 60;
   thrMsgQue.msgSize = sizeof(unsigned int);

   ret = OS_LYRG_createMsgQue(&thrMsgQue, 1);

   thrParams.func = UVC_MAIN_shutterOprThread;
   thrParams.id = OS_LYRG_UVC_OPR_THREAD_ID_E;
   thrParams.event = NULL;
   thrParams.param = NULL;
   uvcShtterOpthrd = OS_LYRG_createThread(&thrParams);
   return ret;
}

static unsigned char UVC_MAIN_readPrivacyShutter()
{
   unsigned int ind, status=0;
   ERRG_codeE                          retCode;
   GPIO_DRVG_gpioGetValParamsT      portParams;

   portParams.gpioNum = PRIVAY_GPIO_NUM;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_VAL_CMD_E, &portParams);
   status = portParams.val;

   for (ind=0; ind < INU_STREAMER__UVC_MAX; ind++)
   {
      if (uvcStreamtable[UVC_STREAM_TABLE_IND0][ind] != NULL)
      {
         inu_streamer__setShtterActive(uvcStreamtable[UVC_STREAM_TABLE_IND0][ind], status);
      }
   }

   return status;
}

void UVC_MAIN_cleanPhyIntr(void *pArg)
{
    UINT8 msg[4];
    UINT32 len=4;
    phyErrorFinished=1;
    if (thrMsgQue.mqHandle != -1) // queue was created
    {
        OS_LYRG_sendMsg(&thrMsgQue, msg, 4);
    }
}

ERRG_codeE UVC_MAIN_phyIntCreateTimer(void)
{
    ERRG_codeE ret;

    phyIntTimer.funcCb = UVC_MAIN_cleanPhyIntr;
    ret = OS_LYRG_createTimer(&phyIntTimer);
    return ret;
}

void UVC_MAIN_phyInterruptFunc (UINT32 srcInt)
{
    UINT8 msg[4];
    UINT32 len=4;
    int ret;

    phyErrorFinished = 0;
    phyIntTimer.firstExpiryNsec       = 500000000;//half a sec
    phyIntTimer.firstExpirySec        = 0;
    phyIntTimer.intervalExpiryNsec    = 0;
    phyIntTimer.intervalExpirySec     = 0;
    ret = OS_LYRG_setTimer(&phyIntTimer);
    if (thrMsgQue.mqHandle != -1) // queue was created
    {
        OS_LYRG_sendMsg(&thrMsgQue, msg, 4);
    }
}

static void UVC_MAIN_startCapturing (uvc_video_fmt * video_fmt, struct buffer **mem, void *custom_data)
{
    AppConfig *app_config = (AppConfig*)custom_data;
    UINT8 msg[4];

    printf("+++++++++++++Configuring app (#%d) to provide width %d height %d (fps %d)\n",
            app_config->app_id, video_fmt->width, video_fmt->height, video_fmt->frame_interval);

    if (uvcShtterthrd == NULL)
    {
        UVC_MAIN_createUvcShutterThr();
        UVC_MAIN_phyIntCreateTimer();
        IAE_DRVG_registerPhyCb(UVC_MAIN_phyInterruptFunc,IAE_DRVG_DPHY0_ERRCTL_E);
        IAE_DRVG_registerPhyCb(UVC_MAIN_phyInterruptFunc,IAE_DRVG_DPHY1_ERRCTL_E);
    }

    // TODO: VE_MNGRG_setResolutionMode(video_fmt->width,video_fmt->height);

    currDevice = app_config->app_id;
    startCapturing[app_config->app_id] = 1;
    forcePic[app_config->app_id] = 1;
    if (thrMsgQue.mqHandle != -1) // queue was created
    {
        lastShutterState=0xffff;
        phyErrorFinished=1;
        OS_LYRG_sendMsg(&thrMsgQue, msg, 4);
    }
}

static void UVC_MAIN_stopCapturing (void *custom_data)
{
    AppConfig *app_config = (AppConfig*)custom_data;
    UINT8 msg[4];

    printf ("++++%s %s %d++++ (dev #%d)\n", __FILE__,__FUNCTION__,__LINE__, app_config->app_id);
#ifndef TEST_UVC_TWO_CAM
    currDevice = app_config->app_id;
    startCapturing[app_config->app_id] = 0;
    if (thrMsgQue.mqHandle != -1) // queue was created
    {
        OS_LYRG_sendMsg(&thrMsgQue, msg, 4);
    }
#else
    VE_MNGRP_recursiveStopFunctions(uvcStreamtable[0][0]);
    VE_MNGRP_recursiveStopFunctions(uvcStreamtable[0][1]);
#endif
}
static int UVC_MAIN_startUVC(AppConfig *app_config)
{
    int init_status = 0;
    uint num_ext_ctrls = 1;
    uvc_device * device = NULL;
    uvc_ext_ctrl_params extended_ctrl_params[num_ext_ctrls];

    LOGG_PRINT(LOG_INFO_E, NULL, "UVC Start\n");

    init_status = uvc_init(&device, app_config->device_name);
    if (init_status)
    {
        err ("Error: wrong initialization %i", init_status);
        return -1;
    }

    app_config->device = device;

    /* Set control and streaming endpoints */
    uvc_set_uvc_endpoints (device, app_config->control_ep, app_config->streaming_ep);

    myUvcDev[app_config->app_id] = device;

    /* Set callback handlers */
    uvc_set_release_data_handler (device, UVC_MAIN_releaseBuffer, (void*)app_config);
    uvc_set_start_capturing_signal (device, UVC_MAIN_startCapturing, (void*)app_config);
    uvc_set_stop_capturing_signal (device, UVC_MAIN_stopCapturing, (void*)app_config);
    uvc_set_control_function (device, UVC_MAIN_setControl, (void*)app_config);
    uvc_get_control_function (device, UVC_MAIN_getControl, (void*)app_config);
    uvc_set_extended_control_function (device, UVC_MAIN_setExtendedControl, (void*)app_config);
    uvc_get_extended_control_function (device, UVC_MAIN_getExtendedControl, (void*)app_config);

    if (BULK == app_config->endpoint_type)
    {
        uvc_set_usb_endpoint_type (device, USB_BULK_TYPE);
    }

    VE_MNGRG_init(uvcStreamtable[UVC_STREAM_TABLE_IND0]);

    uvc_set_buffer_method_function (device, PUSH_BUFFER_METHOD);

    uvc_set_io_method_function (device, IO_METHOD_USERPTR);

    /* Create structures for extended controls */
    extended_control_fill_info (&extended_ctrl_params[0]);
    uvc_extended_controls_register (device, &extended_ctrl_params[0], num_ext_ctrls);

    /* Call the v4l2 function that has the supported video formats and call the register video format library function */
    register_video_formats (device);

    g_is_uvc_started[app_config->app_id] = TRUE;
    uvc_start (device);

    return 0;
}

static void UVC_MAIN_stopUVC(AppConfig* app_config)
{
    LOGG_PRINT(LOG_INFO_E, NULL, "Stopping UVC #%d\n", app_config->app_id);

    app_config->running = 0;

    if (NULL != myUvcDev[app_config->app_id])
    {
        uvc_close (myUvcDev[app_config->app_id]);
        myUvcDev[app_config->app_id] = NULL;
        g_is_uvc_started[app_config->app_id] = FALSE;
    }
}

/* Start the UVC environment */
static int UVC_MAIN_startUvcApp (AppConfig* app_config, unsigned int killInit)
{
    if (!g_is_app_started[app_config->app_id] && killInit)
    {
        return 0;
    }

    if (!killInit)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Starting UVC Stream Init (#%d)\n", app_config->app_id);
        g_is_app_started[app_config->app_id] = TRUE;
    }
    else
    {
        UVC_MAIN_stopUVC(app_config);
        g_is_app_started[app_config->app_id] = FALSE;
    }

    return 0;
}

void UVC_MAING_init(unsigned int killInit)
{
    int i;

#ifdef INU_INIT_INSTEAD_APP_INIT
    VERSION_PRINT;
#endif

    for (i = 0; i < UVC_NUM_OF_DEVICES; i++)
    {
        forcePic[i] = 1;

        if (access(deviceNames[i], F_OK) == 0)
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "RUNNING LIBGUVC #%d\n", i);
        }
        else
        {
            LOGG_PRINT(LOG_INFO_E, NULL, "UVC NOT running #%d\n", i);
            return;
        }

        if (pthread_mutex_init(&app_configs[i].lock, NULL) != 0)
        {
            printf("\n mutex init has failed\n");
            return;
        }

        app_configs[i].app_id = i;
        app_configs[i].device_name = (char*)deviceNames[i];
        app_configs[i].control_ep = 2 * i;
        app_configs[i].streaming_ep = 2 * i + 1;

        app_configs[i].buffer_method = PUSH;
        app_configs[i].endpoint_type = BULK;
        app_configs[i].video_format = YUY2;
        app_configs[i].running = 1;

        UVC_MAIN_startUvcApp(&app_configs[i], killInit);

        if (killInit)
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "STOPPED RUNNING LIBGUVC #%d\n", i);
        }
    }
}

void UVC_MAING_closeThreads()
{
    UINT8 msg[4];

    closeThreads = 1;

    if (uvcShtterthrd)
    {
        OS_LYRG_waitForThread(uvcShtterthrd, OS_LYRG_INFINITE);
        uvcShtterthrd = NULL;
    }

    if (uvcShtterOpthrd)
    {
        OS_LYRG_sendMsg(&thrMsgQue, msg, 4);
        OS_LYRG_waitForThread(uvcShtterOpthrd, OS_LYRG_INFINITE);

        OS_LYRG_deleteMsgQue(&thrMsgQue);
        uvcShtterOpthrd = NULL;
    }

    closeThreads = 0;
}

#ifdef __cplusplus
}
#endif
#endif