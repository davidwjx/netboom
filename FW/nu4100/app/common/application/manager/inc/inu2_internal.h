#ifndef _INU2_INTERNAL_H_
#define _INU2_INTERNAL_H_

#include "inu_types.h"
#if DEFSG_IS_NOT_CEVA
#include "nucfg_priv.h"
#endif

#if DEFSG_IS_NOT_HOST
#pragma pack(push, 4)
#endif

#include "inu_factory.h"
#include "inu_comm.h"
#include "inu_device.h"
#include "inu_graph.h"
#include "inu_logger.h"
#include "inu_image.h"
#include "inu_soc_channel.h"
#include "inu_isp_channel.h"
#include "inu_histogram.h"
#include "inu_histogram_data.h"
#include "inu_soc_writer.h"
#include "inu_streamer.h"
#include "inu_sensor.h"
#include "inu_sensors_group.h"
#include "inu_sensor_control.h"
#include "inu_projector.h"
#include "inu_imu.h"
#include "inu_compress.h"
#include "inu_imu_data.h"
#include "inu_temperature.h"
#include "inu_temperature_data.h"
#include "inu_fdk.h"
#include "inu_soc_writer.h"
#include "inu_fast_orb.h"
#include "inu_fast_data.h"
#include "inu_cva_channel.h"
#include "inu_cva_data.h"
#include "inu_cdnn.h"
#include "inu_cdnn_data.h"
#include "inu_vision_proc.h"
#include "inu_load_network.h"
#include "inu_load_background.h"
#include "inu_slam.h"
#include "inu_slam_data.h"
#include "inu_tsnr_calc.h"
#include "inu_tsnr_data.h"
#include "inu_point_cloud_data.h"
#include "inu_dpe_pp.h"
#include "inu_pp.h"
#include "inu_mipi_channel.h"
#include "inu_metadata_injector.h"

#if DEFSG_IS_NOT_HOST
#pragma pack(pop)
#endif

#if (defined _WIN32) || (defined _WIN64)
#define PATH_SEPARATOR  "\\"
#elif defined(__linux__)
#define PATH_SEPARATOR  "/"
#endif

#define SYSTEMP_XML_PATH_STR_LEN                         1024
#define SYSTEM_XML_FILE_NAME                           "nu4k"
#define SYSTEM_XML_MOD_FILE_NAME                       "nu4k_out_mod"
#define SYSTEM_XML_FILE_EXTENSION                      ".xml"
#define SYSTEM_GP_XML_FILE_NAME                          "config/nu4k.xml"
#define SYSTEM_GP_XML_FILE_NAME_OUT_ORIG                 "config/nu4k_out_orig.xml"
#define SYSTEM_GP_XML_FILE_NAME_OUT_MOD                 "config/nu4k_out_mod.xml"


#endif

