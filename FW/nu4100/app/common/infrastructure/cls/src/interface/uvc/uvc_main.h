
#ifndef UVC_MAIN_H

#include "libguvc.h"


#define MAX_UVC_STREAM 2
#define UVC_STREAM_TABLE_IND0 (0)  // when more then one UVC will be handled
#define NUM_UVC_CONN (INU_FACTORY__CONN_UVC_MAX_E - INU_FACTORY__CONN_UVC1_E + 1)
#define PRIVAY_GPIO_NUM (27)
#define UVC_MAIN_MAX_ROWS_FULLHD 1920
#define UVC_MAIN_MAX_COLOUMS_FULLHD 1080
#define UVC_MAIN_MAX_ROWS_HD 1280
#define UVC_MAIN_MAX_COLOUMS_HD 720

/* Test values, these definitions are used to put in data buffer when a
 * specific get is required
 */
#define GET_CUR_VAL 50
#define GET_MIN_VAL 0
#define GET_MAX_VAL 1000
#define GET_LEN_VAL 1
#define GET_DEF_VAL 0
#define GET_RES_VAL 1
#define GET_INFO_VAL 3

#define XU_EXT_GENERIC_CONTROL 16
/* Buffer methods */
enum
{
  PUSH,
  PULL
};

/* Endpoint types */
enum
{
  ISOCHRONOUS,
  BULK
};

/* Video formats */
enum
{
  YUY2,
  MJPEG,
  H264
};

struct _AppConfig
{
  uint app_id;
  char *device_name;
  int buffer_method;
  int endpoint_type;
  int video_format;
  int frame_interval;
  uvc_device *device;
  int running;
  uint control_ep;
  uint streaming_ep;
  pthread_mutex_t lock;
};

/*H264 GET Request Struct*/
struct _uvc_supported_h264_ctrl
{
  uint *get_max_req;
  uint *get_min_req;
  uint *get_cur_req;
};

#endif
