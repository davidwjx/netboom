/*
inu2.h
*/
#ifndef _INU2_H_
#define _INU2_H_

#ifdef __cplusplus
extern "C" {
#endif


#include "inu2_types.h"
#include "err_defs.h"
#include "nucfg.h"
#include "inu_ref_api.h"
#include "inu_device_api.h"
#include "inu_node_api.h"
#include "inu_graph_api.h"
#include "inu_logger_api.h"
#include "inu_data_api.h"
#include "inu_function_api.h"
#include "inu_image_api.h"
#include "inu_histogram_api.h"
#include "inu_histogram_data_api.h"
#include "inu_fdk_api.h"
#include "inu_soc_channel_api.h"
#include "inu_isp_channel_api.h"
#include "inu_streamer_api.h"
#include "inu_sensor_api.h"
#include "inu_projector_api.h"
#include "inu_sensors_group_api.h"
#include "inu_sensor_control_api.h"
#include "inu_imu_data_api.h"
#include "inu_imu_api.h"
#include "inu_compress_api.h"
#include "inu_temperature_data_api.h"
#include "inu_temperature_api.h"
#include "inu_soc_writer_api.h"
#include "inu_fast_orb_api.h"
#include "inu_fast_data_api.h"
#include "inu_cva_channel_api.h"
#include "inu_cva_data_api.h"
#include "inu_cdnn_api.h"
#include "inu_vision_proc_api.h"
#include "inu_load_network_api.h"
#include "inu_load_background_api.h"
#include "inu_cdnn_data_api.h"
#include "inu_slam_api.h"
#include "inu_slam_data_api.h"
#include "inu_tsnr_calc_api.h"
#include "inu_tsnr_data_api.h"
#include "inu_point_cloud_data_api.h"
#include "inu_dpe_pp_api.h"
#include "inu_pp_api.h"
#include "inu_mipi_channel_api.h"


/* Initialization */
ERRG_codeE inu_init(BOOLEAN resume);
void UVC_MAING_init(unsigned int killInit);
void UVC_MAING_callPushDataToUvc (void *uvcDev,
      unsigned long *buffer_data, int *buf_size,
      int *bytes_used, void **buffer_pointer);
void inu_deinit(BOOLEAN suspend);
void *UVC_MAING_getUvcDevice(int uvcDevice);
void UVC_MAING_freeStream(void *ptr, int uvcDevice);
void UVC_MAING_closeThreads();
void inu_useLongerTimeout(bool useLongerTimeout);

#ifdef __cplusplus
}
#endif

#endif//_INU2_H_
