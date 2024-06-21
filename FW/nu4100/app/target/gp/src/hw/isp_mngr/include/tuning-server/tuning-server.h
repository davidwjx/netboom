/*header for libtuning-server*/

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_DRIVER_NAME_LEN  64
#define SENSOR_CALIBRATION_FILE_PATH_LEN  128

typedef struct _isp_tuning_args_
{
    int isp_id;
    char sensor_driver_name[SENSOR_DRIVER_NAME_LEN];
    char sensor_calibration_file[SENSOR_CALIBRATION_FILE_PATH_LEN]; //full path
} isp_tuning_args;

int tuning_server_main(isp_tuning_args *, void (*tuning_cb)(void *), void *);

#ifdef __cplusplus
}
#endif

