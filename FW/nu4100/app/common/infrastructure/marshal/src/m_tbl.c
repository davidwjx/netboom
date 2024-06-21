/****************************************************************************
***************       I N C L U D E    F I L E S             ***************
****************************************************************************/
#include "inu_common.h"
#include "inu2.h"
#include "internal_cmd.h"
#include "inu2_internal.h"
#include "marshal.h"
#include "m_priv.h"
#include "utils.h"
#include "xml_db.h"
#include "inu_sensorsync.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
***************       L O C A L    D E F N I T I O N S       ***************
****************************************************************************/
#define _B BUILTIN
#define _BP BPTR
#define INUG_GENERAL_SID (INUG_SERVICE_MNGR_E)

#define _sizeof(type,m) sizeof(((type *)0)->m)
#define _xm(type,m,t,n) {M_ ## t,n,_sizeof(type,m),offsetof(type,m)}
#define _m(type,m,t,n) _xm(type,m,t,n)
#ifdef M_STATIC_MEMBERS
#define _e(mid,sz,msz,nm,...) {mid,sz,msz,nm,{__VA_ARGS__}}
#define INIT_M_ENTRY(type,nm,...) \
   {\
   marshal_entry_t entry =_e(M_ ## type, sizeof(type), 0, nm, __VA_ARGS__);\
   memcpy(&marshal_tbl[M_ ## type], &entry, sizeof(entry));\
   }
#else
#define INIT_M_ENTRY(type,nm,...)\
{\
   marshal_memb_t members[MAX_MEMBERS] = {__VA_ARGS__};\
   marshal_memb_t *mpt= (marshal_memb_t *)calloc(nm,sizeof(marshal_memb_t));\
   memcpy(mpt, members, nm*sizeof(marshal_memb_t));\
   marshal_tbl[M_ ## type].members = mpt;\
   marshal_tbl[M_ ## type].m_id = M_ ## type;\
   marshal_tbl[M_ ## type].num_members = nm;\
   marshal_tbl[M_ ## type].size_of = sizeof(type);\
}
#endif //M_STATIC_MEMBERS

/****************************************************************************
***************       L O C A L    T Y P E D E F S           ***************
****************************************************************************/

/****************************************************************************
***************     P R E    D E F I N I T I O N     OF      ***************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/

/****************************************************************************
***************       L O C A L         D A T A              ***************
****************************************************************************/
static marshal_entry_t marshal_tbl[M_NUM_COMPOUND_TYPES_E];
static marshal_id_e marshal_id_lut[INUG_NUM_SERVICES_E][MAX_IOCTL_PER_SERVICE];
static UINT32 marshal_tbl_memb_cnt = 0;
static UINT32 mtbl_hash;

/****************************************************************************
***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static void set_buildin_id(marshal_memb_t *memb)
{
   unsigned int size = memb->size_of / memb->num_elements;

   if (size == 1)
      memb->m_id = M_INT8;
   else if (size == 2)
      memb->m_id = M_INT16;
   else if (size == 4)
      memb->m_id = M_INT32;
   else if (size == 8)
      memb->m_id = M_INT64;
   else
      memb->m_id = M_INVALID_ENTRY;
}

void set_tbl_builtin_ids(void)
{
   unsigned int i, j;
   for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
   {
      for (j = 0; j < marshal_tbl[i].num_members; j++)
      {
         if (marshal_tbl[i].members[j].m_id == M_BUILTIN)
            set_buildin_id(&marshal_tbl[i].members[j]);
      }
      marshal_tbl_memb_cnt += marshal_tbl[i].num_members;
   }
}

void set_tbl_m_sizes(void)
{
   unsigned int i;
   for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
   {
      marshal_tbl[i].m_size = marshal_msg(marshal_tbl[i].m_id, NULL, NULL);
   }
}

static void clear_marsahl_tbl(void)
{
   unsigned int i;
   for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
   {
      marshal_tbl[i].m_id = M_INVALID_ENTRY;
   }
}

static void clear_marshal_id_lut()
{
   unsigned int i, j;
   for (i = 0; i < INUG_NUM_SERVICES_E; i++)
   {
      for (j = 0; j < MAX_IOCTL_PER_SERVICE; j++)
      {
         marshal_id_lut[i][j] = M_INVALID_ENTRY;
      }
   }
}

static void init_lut_entry(INUG_serviceIdE sid, UINT32 cmd, marshal_id_e mid)
{
   marshal_id_lut[sid][INUG_SERVICE_IOCTL_INDEX(cmd) - 1] = mid;
}

void sanity_check_tbl()
{
   unsigned int i,j;
   for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
   {
      //Uninitialized entry
      if(marshal_tbl[i].m_id == M_INVALID_ENTRY)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "marshal table assertion failed: entry %d is invalid\n", i);
         goto sanity_check_err;
      }

      //No members
      if(marshal_tbl[i].num_members == 0)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "marshal table assertion failed: entry %d(id=%d) has 0 members\n", i, marshal_tbl[i].m_id);
         goto sanity_check_err;
      }
      //uninitialized members
      for (j = 0; j < marshal_tbl[i].num_members; j++)
      {
         if (marshal_tbl[i].members[j].num_elements == 0)
         {
            LOGG_PRINT(LOG_INFO_E, NULL, "marshal table assertion failed: entry %d(id=%d) member %d invalid\n", i,marshal_tbl[i].m_id,j);
            goto sanity_check_err;
         }
      }
   }
   return;

sanity_check_err:
      assert_func(0,"marshal table invalid - aborting\n");
}

static void hash_mtbl()
{
   enum {M_HASH_WORDS = 3};

#ifdef M_STATIC_MEMBERS
   return;
#else
   unsigned int i,j;
   INT16 dir = MARSHALG_DO;

   UINT8 crc_tbl[UTILSG_CRC_TABLE_SIZE];
   UINT16 *tmp_m_tbl = (UINT16 *)calloc(sizeof(UINT16)*M_HASH_WORDS, M_NUM_COMPOUND_TYPES_E);
   UINT16 *tmp_lut = (UINT16 *)calloc(sizeof(UINT16), INUG_NUM_SERVICES_E * MAX_IOCTL_PER_SERVICE);

   if (!tmp_m_tbl || !tmp_lut)
   {
      goto done_hash_mtbl;
   }

   //Copy,convert to LE to temporary tables values that are platfrom independent
   //Using only UINT16 values for better error detection.
   for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
   {
      marshal_u16(dir, (UINT16 *)&marshal_tbl[i].m_id, (UINT8 *)&tmp_m_tbl[i*M_HASH_WORDS]);
      marshal_u16(dir, (UINT16 *)&marshal_tbl[i].m_size, (UINT8 *)&tmp_m_tbl[i*M_HASH_WORDS +1]);
      marshal_u16(dir, (UINT16 *)&marshal_tbl[i].num_members, (UINT8 *)&tmp_m_tbl[i*M_HASH_WORDS +2]);
   }

   for (i = 0; i < INUG_NUM_SERVICES_E; i++)
   {
      for (j = 0; j < MAX_IOCTL_PER_SERVICE; j++)
      {
         marshal_u16(dir, (UINT16 *)&marshal_id_lut[i][j], (UINT8 *)&tmp_lut[i*j]);
      }
   }

   //calculate the crc
   memset(crc_tbl,0,sizeof(crc_tbl));
   UTILSG_crcInit(crc_tbl);
   mtbl_hash = (UINT32)UTILSG_calcCrc((const UINT8 *)tmp_m_tbl, sizeof(UINT16)*M_HASH_WORDS* M_NUM_COMPOUND_TYPES_E, crc_tbl);
   mtbl_hash |= (UINT32)(UTILSG_calcCrc((const UINT8 *)tmp_lut,sizeof(UINT16)*INUG_NUM_SERVICES_E * MAX_IOCTL_PER_SERVICE, crc_tbl) << 8);
   mtbl_hash |= (M_NUM_COMPOUND_TYPES_E << 16);

   //UTILSG_dump_hex((UINT8 *)tmp_m_tbl,sizeof(UINT16)*M_HASH_WORDS* M_NUM_COMPOUND_TYPES_E);
   //UTILSG_dump_hex((UINT8 *)tmp_lut,sizeof(UINT16)*INUG_NUM_SERVICES_E * MAX_IOCTL_PER_SERVICE);
done_hash_mtbl:
   if(tmp_m_tbl)
      free(tmp_m_tbl);
   if(tmp_lut)
      free(tmp_lut);
#endif
}


/****************************************************************************
***************     G L O B A L         F U N C T I O N S    ***************
****************************************************************************/
/*
   init_marshal_id_lut: Init marshal id look-up table. Maps ioctls numbers to marshal id numbers per service
*/
void init_marshal_id_lut(void)
{
   clear_marshal_id_lut();
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_GET_BUF_E, M_INTERNAL_CMDG_getBufT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_BUF_E, M_INTERNAL_CMDG_sendBufT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_WAIT_BUF_ACK_E, M_INTERNAL_CMDG_waitBufAckT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_LUT_HDR_E, M_INTERNAL_CMDG_lutChunkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_E, M_INTERNAL_CMDG_CdnnLoadNetworkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_REQUEST_FILE_FROM_GP_E, M_INTERNAL_CMDG_SendGpFileHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_CDNN_RELEASE_ALL_E, M_INTERNAL_CMDG_CdnnReleaseAllHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_CDNN_LOAD_NETWORK_ALLOC_E, M_INTERNAL_CMDG_CdnnAllocNetworkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_E, M_INTERNAL_CMDG_VisionProcLoadBackgroundHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_BACKGROUND_RELEASE_E, M_INTERNAL_CMDG_BackgroundReleaseHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_VISION_PROC_LOAD_BACKGROUND_ALLOC_E, M_INTERNAL_CMDG_VisionProcAllocBackgroundHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_LOAD_CEVA_HDR_E, M_INTERNAL_CMDG_cevaAppChunkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_WRITE_BUF_TRANSFER_E, M_inu_device__readWriteBuf);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_BURN_DYN_CALIB_E, M_inu_device__readWriteBuf);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_READ_BUF_HDR_E, M_INTERNAL_CMDG_readBufChunkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_GET_READ_FUNC_E, M_INTERNAL_CMDG_getReadFuncT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_WRITE_FLASH_HDR_E, M_INTERNAL_CMDG_flashChunkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_READ_FLASH_HDR_E, M_INTERNAL_CMDG_flashChunkHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_EEPROM_READ_E, M_inu_device__eepromAccessT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_EEPROM_WRITE_E, M_inu_device__eepromAccessT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_PING_E, M_INTERNAL_CMDG_pingT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_PING_RESPOND_E, M_INTERNAL_CMDG_pingT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_CONFIG_WD_E, M_inu_device__watchdog_timeout_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_GET_WRITE_FUNC_E, M_INTERNAL_CMDG_getWriteFuncT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_DELETE_REF_E, M_inu_ref__DtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_GRAPH_E, M_inu_graph__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_LOGGER_E, M_inu_logger__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_DATA_E, M_inu_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_IMAGE_E, M_inu_image__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_CVA_DATA_E, M_inu_cva_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SOC_CH_E, M_inu_soc_channel__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_CVA_CH_E, M_inu_cva_channel__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_ISP_CH_E, M_inu_isp_channel__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_STREAMER_E, M_inu_streamer__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SENSOR_E, M_inu_sensor__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SENSORS_GROUP_E, M_inu_sensors_group__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_FDK_E, M_inu_fdk__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_BIND_NODES_E, M_inu_ref__container);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_FUNCTION_START_E, M_inu_function__startParamsT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_GET_IMU_TEMPERATURE_E, M_inu_imu__temperatureParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_FUNCTION_STOP_E, M_inu_function__stopParamsT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_DATA_SEND_E, M_inu_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_IMAGE_SEND_E, M_inu_image__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_CVA_DATA_SEND_E, M_inu_cva_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_FAST_ORB_SEND_E, M_inu_fast_orb_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_IMU_E, M_inu_imu__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_IMU_DATA_E, M_inu_imu_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_IMU_DATA_SEND_E, M_inu_imu_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_TEMPERATURE_E, M_inu_temperature__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_TEMPERATURE_DATA_E, M_inu_temperature_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_TEMPERATURE_DATA_SEND_E, M_inu_temperature_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SOC_WRITER_E, M_inu_soc_writer__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_HISTOGRAM_E, M_inu_histogram__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_HISTOGRAM_DATA_E, M_inu_histogram_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_HISTOGRAM_DATA_SEND_E, M_inu_histogram_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_HISTOGRAM_ROI_CFG_E, M_inu_histogram__roiCfgT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_CHANGE_CROP_WINDOW_E, M_inu_soc_channel__cropParamsT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_UPDATE_CHANNEL_USER_PARAM_E, M_inu_soc_channel__userParamsT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SENSOR_CONTROL_E, M_inu_sensor_control__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_CONFIG_DB_E, M_inu_graph__send_config_db_params);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_GET_DEVICE_REF_LIST_E, M_inu_device__refList);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_FAST_ORB_E, M_inu_fast_orb__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_FAST_ORB_DATA_E, M_inu_fast_orb_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_CDNN_E, M_inu_cdnn__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_CDNN_DATA_E, M_inu_cdnn_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_CDNN_NETWORK_ID_SET_E, M_inu_cdnn__networkIdSetT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_VISION_PROC_E, M_inu_vision_proc__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SLAM_E, M_inu_slam__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_SLAM_DATA_E, M_inu_slam_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_LOAD_NETWORK_E, M_inu_load_network__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_LOAD_BACKGROUND_E, M_inu_load_background__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_CDNN_DATA_SEND_E, M_inu_cdnn_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SLAM_DATA_SEND_E, M_inu_slam_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_TSNR_DATA_SEND_E, M_inu_tsnr_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_POINT_CLOUD_DATA_SEND_E, M_inu_point_cloud_data__hdr_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_TSNR_CALC_E, M_inu_tsnr_calc__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_TSNR_DATA_E, M_inu_tsnr_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_POINT_CLOUD_DATA_E, M_inu_point_cloud_data__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_COMPRESS_E, M_inu_compress__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_DPE_PP_E, M_inu_dpe_pp__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_PP_E, M_inu_pp__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_MIPI_CH_E, M_inu_mipi_channel__CtorParams);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_ALLOC_GP_BUF_E, M_INTERNAL_CMDG_AllocGpBufHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_FREE_GP_BUF_E, M_INTERNAL_CMDG_FreeGpBufHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_FILE_TO_GP_BUF_E, M_INTERNAL_CMDG_SendGpFileHdrT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_UPDATE_RAW_NAND_SECTION_E, M_INTERNAL_CMDG_UpdateRawNandSecT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SENSOR_CONTROL_UPDATE_E, M_inu_sensor_control__parameter_list_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SENSOR_CONTROL_GET_PARAMS_E, M_inu_sensor_control__parameter_list_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_PP_UPDATE_E, M_inu_pp__parameter_list_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_VISION_PROC_UPDATE_E, M_inu_vision_proc__parameter_list_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SLAM_UPDATE_E, M_inu_slam__parameter_list_t);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_SEND_ISP_COMMAND_E, M_inu_isp_channel__commandT);
   init_lut_entry(INUG_GENERAL_SID, INTERNAL_CMDG_NEW_METADATA_INJECTOR_E, M_inu_metadata_injector__CtorParams);

   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_GET_VERSION_E, M_inu_device__version);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_RD_REG_E, M_inu_device__readRegT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_WR_REG_E, M_inu_device__writeRegT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_DB_CONTROL_E, M_INUG_ioctlDbControlT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_POWER_MODE_E, M_inu_device__powerModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SET_TIME_E, M_inu_device__set_time_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_GET_TIME_E, M_inu_device__get_time_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_GET_RTC_LOCK_E, M_inu_device__get_rtc_lock_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_GET_CFG, M_inu_sensor__runtimeCfg_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SENSOR_RUNTIME_SET_CFG, M_inu_sensor__runtimeCfg_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SENSOR_FOCUS_GET_PARAMS, M_inu_sensor__getFocusParams_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SENSOR_FOCUS_SET_PARAMS, M_inu_sensor__setFocusParams_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SENSOR_CROP_SET_PARAMS, M_inu_sensor__setCropParams_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_CALIBRATION_RD_E, M_INUG_ioctlCalibrationRdT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_BUFFER_RD_E, M_INUG_ioctlBufferRdT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_ALTERNATE_SET_CONFIG_E, M_inu_device__alternateProjCfgT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_CALIBRATION_HDR_RD_E, M_INUG_ioctlCalibrationHdrRdT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_ERASE_DYN_CALIBRATION_E, M_INUG_ioctlEraseDynCalibrationT);
   
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_BOOT_SET_MODE_E, M_INUG_ioctlBootSetModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_PROJECTOR_SET_MODE_E, M_INUG_ioctlProjectorSetModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_PROJECTOR_GET_MODE_E, M_INUG_ioctlProjectorGetModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_GET_MODE_E, M_INUG_ioctlSysGetModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SET_MODE_E, M_INUG_ioctlSysSetModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_UART2LOOPBACK_PROBE_E, M_INUG_ioctlSysUart2LbProbeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_CALIB_PATH_E, M_INUG_ioctlCalibrationPathInfoT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_CHAIN_HDR_E, M_INUG_ioctlChainHdrT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_READ_BUFFER_E, M_inu_device__readWriteBuf);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_CONFIG_PWM_E, M_inu_device__pwmConfigT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_UPDATE_CALIBRATION_E, M_inu_device__calibUpdateT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_EEPROM_READ_E, M_inu_device__eepromAccessT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_POWER_DOWN_E, M_inu_device__powerModeT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SENSOR_SYNC_E, M_inu_device__sensorSync);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SENSORUPDATE,M_inu_device__sensorUpdateT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_TUNING_SERVER_E, M_inu_device__tuningParamsT);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SELECT_TEMP_SENSOR_E, M_inu_device__selectTempSensor_t);
   init_lut_entry(INUG_SERVICE_SYSTEM_E, INUG_IOCTL_SYSTEM_SET_FLASH_SECTION_E, M_inu_device__setflashSectionHdr);
   

   init_lut_entry(INUG_SERVICE_LOGGER_E, INUG_IOCTL_LOGGER_SET_PARAMS_E, M_INUG_ioctlLoggerSetParamsT);
   init_lut_entry(INUG_SERVICE_LOGGER_E, INUG_IOCTL_LOGGER_GET_PARAMS_E, M_INUG_ioctlLoggerSetParamsT);
   init_lut_entry(INUG_SERVICE_LOGGER_E, INTERNAL_CMDG_LOG_DATA_HDR_E, M_INTERNAL_CMDG_logDataHdrT);
}

/*
init_marshal_tbl - Initialize the marshal table.

Example: Add a new marshal type for fooT with ioctl num FOO_E:
   1. Create M_footT:
   Add a new enum value in marshal_id_e called M_footT.
   2. Map FOO_E -> M_fooT:
   Add new init_lut_entry() call in init_marshal_id_lut().
   3. Add fooT to marshal table
   Add an INIT_M_ENTRY macro invocation in init_marshal_tbl()
*/
void init_marshal_tbl(void)
{
   clear_marsahl_tbl();

   INIT_M_ENTRY(INUG_ioctlLoggerSetParamsT, 8, \
      _m(INUG_ioctlLoggerSetParamsT, hostLogSeverity, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, hostPrintLogs, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, hostSendLogs, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, hostPrintGpLogs, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, hostSendGpLogs, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, gpLogSeverity, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, gpPrintLogs, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, gpSendLogs, _B, 1), \
      _m(INUG_ioctlLoggerSetParamsT, gpSendCdcLogs, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_logDataHdrT, 3, \
      _m(INTERNAL_CMDG_logDataHdrT, pid, _B, 1), \
      _m(INTERNAL_CMDG_logDataHdrT, level, _B, 1), \
      _m(INTERNAL_CMDG_logDataHdrT, msgSize, _B, 1));
   INIT_M_ENTRY(INUG_ioctlCalibrationRdT, 2, \
      _m(INUG_ioctlCalibrationRdT, calibrationRdSize, _B, 1), \
      _m(INUG_ioctlCalibrationRdT, calibrationRdBufP, _BP, 1));
   INIT_M_ENTRY(INUG_ioctlProjectorSetModeT, 2, \
      _m(INUG_ioctlProjectorSetModeT, projSelect, _B, 1),    \
      _m(INUG_ioctlProjectorSetModeT, projSetMode, _B, 1));
   INIT_M_ENTRY(INUG_ioctlProjectorGetModeT, 2, \
      _m(INUG_ioctlProjectorGetModeT, projSelect, _B, 1),    \
      _m(INUG_ioctlProjectorGetModeT, projGetMode, _B, 1));
   INIT_M_ENTRY(INUG_ioctlSysSetModeT, 1, \
      _m(INUG_ioctlSysSetModeT, configMode, _B, 1));
   INIT_M_ENTRY(INUG_configMetaT, 15, \
      _m(INUG_configMetaT, video_width, _B, 1), \
      _m(INUG_configMetaT, video_height, _B, 1), \
      _m(INUG_configMetaT, depth_width, _B, 1), \
      _m(INUG_configMetaT, depth_height, _B, 1), \
      _m(INUG_configMetaT, webcam_width, _B, 1), \
      _m(INUG_configMetaT, webcam_height, _B, 1), \
      _m(INUG_configMetaT, fc_left, _B, 1), \
      _m(INUG_configMetaT, cc_left_x, _B, 1), \
      _m(INUG_configMetaT, cc_left_y, _B, 1), \
      _m(INUG_configMetaT, fc_right, _B, 1), \
      _m(INUG_configMetaT, cc_right_x, _B, 1), \
      _m(INUG_configMetaT, cc_right_y, _B, 1), \
      _m(INUG_configMetaT, T, _B, 1), \
      _m(INUG_configMetaT, Nx, _B, 1), \
      _m(INUG_configMetaT, Ny, _B, 1));
   INIT_M_ENTRY(INUG_ioctlSysGetModeT, 1, \
      _m(INUG_ioctlSysGetModeT, metaData, INUG_configMetaT, 1));
   INIT_M_ENTRY(INUG_ioctlSysUart2LbProbeT, 1, \
      _m(INUG_ioctlSysUart2LbProbeT, loopback, _B, 1));
   INIT_M_ENTRY(INUG_ioctlCalibrationHdrRdT, 3, \
      _m(INUG_ioctlCalibrationHdrRdT, calibrationDataSize, _B, 1), \
      _m(INUG_ioctlCalibrationHdrRdT, calibrationDataFormat, _B, 1), \
      _m(INUG_ioctlCalibrationHdrRdT, calibrationTimestamp, _B, 1));
   INIT_M_ENTRY(INUG_ioctlEraseDynCalibrationT, 1, \
      _m(INUG_ioctlEraseDynCalibrationT, calibType, _B, 1));
   INIT_M_ENTRY(INUG_ioctlChainHdrT, 1, \
      _m(INUG_ioctlChainHdrT, chainHdr, _B, 1));
   INIT_M_ENTRY(INUG_ioctlBootSetModeT, 1, \
      _m(INUG_ioctlBootSetModeT, bootSetMode, _B, 1));
   INIT_M_ENTRY(INUG_ioctlBufferRdT, 3, \
      _m(INUG_ioctlBufferRdT, startAddress, _B, 1), \
      _m(INUG_ioctlBufferRdT, size, _B, 1), \
      _m(INUG_ioctlBufferRdT, buffP, _BP, 1));
   INIT_M_ENTRY(inu_device__alternateProjCfgT, 3, \
      _m(inu_device__alternateProjCfgT, alternateModeEnable, _B, 1), \
      _m(inu_device__alternateProjCfgT, numFramesPattern, _B, 1), \
      _m(inu_device__alternateProjCfgT, numFramesFlood, _B, 1));
   INIT_M_ENTRY(inu_device__set_time_t, 8, \
      _m(inu_device__set_time_t, method, _B, 1), \
      _m(inu_device__set_time_t, seconds, _B, 1), \
      _m(inu_device__set_time_t, microSeconds, _B, 1), \
      _m(inu_device__set_time_t, secondHostResolution, _B, 1), \
      _m(inu_device__set_time_t, uartBaudRate, _B, 1), \
      _m(inu_device__set_time_t, startTick, _B, 1), \
      _m(inu_device__set_time_t, uartPortName, _B, MAX_UART_PORT_NAME_LEN), \
      _m(inu_device__set_time_t, debugEnable, _B, 1));
   INIT_M_ENTRY(inu_device__get_time_t, 1, \
      _m(inu_device__get_time_t, usec, _B, 1));
   INIT_M_ENTRY(inu_device__get_rtc_lock_t, 1, \
      _m(inu_device__get_rtc_lock_t, lockStatus, _B, 1));
   INIT_M_ENTRY(inu_sensor__gain_t, 2, \
      _m(inu_sensor__gain_t, analog, _B, 1), \
      _m(inu_sensor__gain_t, digital, _B, 1));
   INIT_M_ENTRY(inu_sensor__runtimeCfg_t, 5, \
      _m(inu_sensor__runtimeCfg_t, exposure, _B, 1), \
      _m(inu_sensor__runtimeCfg_t, gain, inu_sensor__gain_t, 1), \
      _m(inu_sensor__runtimeCfg_t, fps, _B, 1), \
      _m(inu_sensor__runtimeCfg_t, context, _B, 1),\
      _m(inu_sensor__runtimeCfg_t, temperature, _B, 1));
   INIT_M_ENTRY(inu_sensor__getFocusParams_t, 5, \
      _m(inu_sensor__getFocusParams_t, dac, _B, 1), \
      _m(inu_sensor__getFocusParams_t, min, _B, 1), \
      _m(inu_sensor__getFocusParams_t, max, _B, 1), \
      _m(inu_sensor__getFocusParams_t, chipId, _B, 1), \
      _m(inu_sensor__getFocusParams_t, context, _B, 1));
   INIT_M_ENTRY(inu_sensor__setFocusParams_t, 3, \
      _m(inu_sensor__setFocusParams_t, mode, _B, 1), \
      _m(inu_sensor__setFocusParams_t, dac, _B, 1), \
      _m(inu_sensor__setFocusParams_t, context, _B, 1));
   INIT_M_ENTRY(inu_sensor__setCropParams_t, 2, \
      _m(inu_sensor__setCropParams_t, startX, _B, 1), \
      _m(inu_sensor__setCropParams_t, startY, _B, 1)); 
   INIT_M_ENTRY(INUG_ioctlDbControlT, 1, _m(INUG_ioctlDbControlT, dbControl, _B, 1));
   INIT_M_ENTRY(inu_device__powerModeT, 1, _m(inu_device__powerModeT, disableUnits, _B, 1));
   INIT_M_ENTRY(inu_device__tuningParamsT, 1, \
     _m(inu_device__tuningParamsT, sensorNum, _B, 1));
   INIT_M_ENTRY(inu_device__readRegT, 7, \
      _m(inu_device__readRegT, regType, _B, 1), \
      _m(inu_device__readRegT, socReg, inu_device__socRegT, 1), \
      _m(inu_device__readRegT, i2cReg, inu_device__i2cRegT, 1), \
      _m(inu_device__readRegT, attReg, inu_device__attRegT, 1), \
      _m(inu_device__readRegT, attReg, inu_device__waitRegT, 1),\
      _m(inu_device__readRegT, dphyReg, inu_device__dphyRegT, 1),\
      _m(inu_device__readRegT, tempReg, inu_device__tempRegT, 1));
   INIT_M_ENTRY(inu_device__waitRegT, 1, \
      _m(inu_device__waitRegT, usecWait, _B, 1));
   INIT_M_ENTRY(inu_device__i2cRegT, 6, \
      _m(inu_device__i2cRegT, i2cNum, _B, 1), \
      _m(inu_device__i2cRegT, i2cData, _B, 1), \
      _m(inu_device__i2cRegT, regWidth, _B, 1), \
      _m(inu_device__i2cRegT, regAddress, _B, 1), \
      _m(inu_device__i2cRegT, tarAddress, _B, 1), \
      _m(inu_device__i2cRegT, accessSize, _B, 1));
   INIT_M_ENTRY(inu_device__socRegT, 2, \
      _m(inu_device__socRegT, addr, _B, 1), \
      _m(inu_device__socRegT, val, _B, 1));
   INIT_M_ENTRY(inu_device__attRegT, 2, \
       _m(inu_device__attRegT, addr, _B, 1), \
       _m(inu_device__attRegT, val, _B, 1));
   INIT_M_ENTRY(inu_device__writeRegT, 7, \
      _m(inu_device__writeRegT, regType, _B, 1), \
      _m(inu_device__writeRegT, phase, _B, 1), \
      _m(inu_device__writeRegT, calRegsCntr, _B, 1), \
      _m(inu_device__writeRegT, socReg, inu_device__socRegT, 1), \
      _m(inu_device__writeRegT, i2cReg, inu_device__i2cRegT, 1), \
      _m(inu_device__writeRegT, attReg, inu_device__attRegT, 1), \
      _m(inu_device__writeRegT, waitReg, inu_device__waitRegT, 1));
   INIT_M_ENTRY(inu_device__version, 16, \
      _m(inu_device__version, hwVerId.val, _B, 1), \
      _m(inu_device__version, fwVerId, _B, 1), \
      _m(inu_device__version, masterSensorId, _B, INU_DEFSG_IOCTL_SENSOR_ID_LENGTH), \
      _m(inu_device__version, slaveSensorId, _B, INU_DEFSG_IOCTL_SENSOR_ID_LENGTH), \
      _m(inu_device__version, ispVersion, _B, INU_DEFSG_IOCTL_SENSOR_ID_LENGTH), \
      _m(inu_device__version, isMaster, _B, 1), \
      _m(inu_device__version, serialNumber, _B, INU_DEFSG_IOCTL_EXT_PRODUCTION_STRING_LENGTH), \
      _m(inu_device__version, modelNumber, _B, INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH), \
      _m(inu_device__version, partNumber, _B, INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH), \
      _m(inu_device__version, sensorRevision, _B, INU_DEFSG_IOCTL_PRODUCTION_STRING_LENGTH), \
      _m(inu_device__version, bootId, _B, 1), \
      _m(inu_device__version, baseVersion, _B, 1), \
      _m(inu_device__version, bootfixTimestamp, _B, 1), \
      _m(inu_device__version, bootfixVersion, _B, INU_DEFSG_IOCTL_BOOTFIX_STRING_LENGTH), \
      _m(inu_device__version, usbSpeed, _B, 1), \
      _m(inu_device__version, pllFreq, _B, INU_DEVICE__PLL_NUM_PLLS_E));
   INIT_M_ENTRY(INTERNAL_CMDG_pingT, 1, _m(INTERNAL_CMDG_pingT, timestamp, _B, 1));
   INIT_M_ENTRY(inu_device__watchdog_timeout_t, 1, _m(inu_device__watchdog_timeout_t, sec, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_flashChunkHdrT, 5, \
      _m(INTERNAL_CMDG_flashChunkHdrT, flashAck, _B, 1), \
      _m(INTERNAL_CMDG_flashChunkHdrT, chunkId, _B, 1), \
      _m(INTERNAL_CMDG_flashChunkHdrT, chunkLen, _B, 1), \
      _m(INTERNAL_CMDG_flashChunkHdrT, numOfChunks, _B, 1), \
      _m(INTERNAL_CMDG_flashChunkHdrT, flashBufSize, _B, 1));
   INIT_M_ENTRY(inu_device__eepromAccessT, 4, \
      _m(inu_device__eepromAccessT, sensorId, _B, 1), \
      _m(inu_device__eepromAccessT, operation, _B, 1), \
      _m(inu_device__eepromAccessT, size, _B, 1), \
      _m(inu_device__eepromAccessT, data, _BP, 1));
   INIT_M_ENTRY(inu_device__powerDownT, 1, \
      _m(inu_device__powerDownT, cmd, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_getReadFuncT, 1, _m(INTERNAL_CMDG_getReadFuncT, read, _BP, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_readBufChunkHdrT, 4, \
      _m(INTERNAL_CMDG_readBufChunkHdrT, chunkId, _B, 1), \
      _m(INTERNAL_CMDG_readBufChunkHdrT, chunkLen, _B, 1), \
      _m(INTERNAL_CMDG_readBufChunkHdrT, numOfChunks, _B, 1), \
      _m(INTERNAL_CMDG_readBufChunkHdrT, buffSize, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_sendBufChunkHdrT, 4, \
      _m(INTERNAL_CMDG_sendBufChunkHdrT, destAddress, _B, 1), \
      _m(INTERNAL_CMDG_sendBufChunkHdrT, chunkId, _B, 1), \
      _m(INTERNAL_CMDG_sendBufChunkHdrT, chunkLen, _B, 1), \
      _m(INTERNAL_CMDG_sendBufChunkHdrT, numOfChunks, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_cevaAppChunkHdrT, 4, \
      _m(INTERNAL_CMDG_cevaAppChunkHdrT, cevaId, _B, 1), \
      _m(INTERNAL_CMDG_cevaAppChunkHdrT, chunkId, _B, 1), \
      _m(INTERNAL_CMDG_cevaAppChunkHdrT, chunkLen, _B, 1), \
      _m(INTERNAL_CMDG_cevaAppChunkHdrT, numOfChunks, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_waitBufAckT, 1, _m(INTERNAL_CMDG_waitBufAckT, dataType, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_lutChunkHdrT, 9, \
      _m(INTERNAL_CMDG_lutChunkHdrT, calibMode, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, lutId, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, sensorInd, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, dblBuffId, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, mode, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, chunkId, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, chunkLen, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, numOfChunks, _B, 1), \
      _m(INTERNAL_CMDG_lutChunkHdrT, lutSize, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_CdnnLoadNetworkHdrT, 16, \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, networkId, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, engineType, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, mode, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, chunkId, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, chunkLen, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, numOfChunks, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, networkSize, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, rawScale, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, channelSwap, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, resultsFormat, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, isYoloNetwork, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, bitAccuracy, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, pipeDepth, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, ProcesssingFlags, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, checksumall, _B, 1), \
      _m(INTERNAL_CMDG_CdnnLoadNetworkHdrT, pad, _B, 946));
   INIT_M_ENTRY(INTERNAL_CMDG_CdnnAllocNetworkHdrT, 2, \
      _m(INTERNAL_CMDG_CdnnAllocNetworkHdrT, networkId, _B, 1), \
      _m(INTERNAL_CMDG_CdnnAllocNetworkHdrT, networkSize, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_VisionProcLoadBackgroundHdrT, 1, _m(INTERNAL_CMDG_VisionProcLoadBackgroundHdrT, backgroundId, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_VisionProcAllocBackgroundHdrT, 2, \
       _m(INTERNAL_CMDG_VisionProcAllocBackgroundHdrT, backgroundId, _B, 1), \
       _m(INTERNAL_CMDG_VisionProcAllocBackgroundHdrT, backgroundSize, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_AllocGpBufHdrT, 2, \
      _m(INTERNAL_CMDG_AllocGpBufHdrT, bufId, _B, 1), \
      _m(INTERNAL_CMDG_AllocGpBufHdrT, bufSize, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_FreeGpBufHdrT, 2, \
      _m(INTERNAL_CMDG_FreeGpBufHdrT, bufId, _B, 1), \
      _m(INTERNAL_CMDG_FreeGpBufHdrT, bufDescrP, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_UpdateRawNandSecT, 6, \
      _m(INTERNAL_CMDG_UpdateRawNandSecT, type, _B, 1), \
      _m(INTERNAL_CMDG_UpdateRawNandSecT, filename, _B, MAX_FILE_NAME), \
      _m(INTERNAL_CMDG_UpdateRawNandSecT, verMajor, _B, 1), \
      _m(INTERNAL_CMDG_UpdateRawNandSecT, verMinor, _B, 1), \
      _m(INTERNAL_CMDG_UpdateRawNandSecT, verBuild, _B, 1), \
      _m(INTERNAL_CMDG_UpdateRawNandSecT, updatedSectionsFlags, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_SendGpFileHdrT, 1, _m(INTERNAL_CMDG_SendGpFileHdrT, filename, _B, MAX_FILE_NAME));
   INIT_M_ENTRY(INTERNAL_CMDG_CdnnReleaseAllHdrT, 1, _m(INTERNAL_CMDG_CdnnReleaseAllHdrT, message, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_BackgroundReleaseHdrT, 1, _m(INTERNAL_CMDG_BackgroundReleaseHdrT, message, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_sendBufT, 3, \
      _m(INTERNAL_CMDG_sendBufT, dataType, _B, 1), \
      _m(INTERNAL_CMDG_sendBufT, numBytes, _B, 1), \
      _m(INTERNAL_CMDG_sendBufT, bufP, _BP, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_getBufT, 6, \
      _m(INTERNAL_CMDG_getBufT, bufDescP, _BP, 1), \
      _m(INTERNAL_CMDG_getBufT, mode, _B, 1), \
      _m(INTERNAL_CMDG_getBufT, bufCntr, _B, 1), \
      _m(INTERNAL_CMDG_getBufT, gpTimeNs, _B, 1), \
      _m(INTERNAL_CMDG_getBufT, hostTimeSec, _B, 1), \
      _m(INTERNAL_CMDG_getBufT, hostTimeMs, _B, 1));
   INIT_M_ENTRY(INU_DEFSG_sensorOffsetsT, 4, \
      _m(INU_DEFSG_sensorOffsetsT, roiXstart, _B, 1), \
      _m(INU_DEFSG_sensorOffsetsT, roiYstart, _B, 1), \
      _m(INU_DEFSG_sensorOffsetsT, roiXend, _B, 1), \
      _m(INU_DEFSG_sensorOffsetsT, roiYend, _B, 1));
   INIT_M_ENTRY(INUG_ioctlCalibrationPathInfoT, 2, \
      _m(INUG_ioctlCalibrationPathInfoT, calibrationPath, _BP, 1), \
      _m(INUG_ioctlCalibrationPathInfoT, numOfPaths, _B, 1));
   INIT_M_ENTRY(INU_CMDG_posTemperatureT, 1, _m(INU_CMDG_posTemperatureT, temperature, _B, 1));
   INIT_M_ENTRY(INTERNAL_CMDG_getWriteFuncT, 1, _m(INTERNAL_CMDG_getWriteFuncT, write, _BP, 1));
   INIT_M_ENTRY(inu_ref__container, 1, _m(inu_ref__container, id, _B, 1));
   INIT_M_ENTRY(inu_ref__DtorParams, 1, _m(inu_ref__DtorParams, ref, inu_ref__container, 1));
   INIT_M_ENTRY(inu_ref__CtorParams, 7, \
      _m(inu_ref__CtorParams, device, _BP, 1), \
      _m(inu_ref__CtorParams, ctrlCommH, _BP, 1), \
      _m(inu_ref__CtorParams, dataCommH, _BP, 1), \
      _m(inu_ref__CtorParams, id, _B, 1), \
      _m(inu_ref__CtorParams, statsEnable, _B, 1), \
      _m(inu_ref__CtorParams, dataChanDirectFlag, _B, 1),
      _m(inu_ref__CtorParams, userName, _B, MAX_NAME_LEN));
   INIT_M_ENTRY(inu_graph__CtorParams, 3, \
      _m(inu_graph__CtorParams, ref_params, inu_ref__CtorParams, 1), \
      _m(inu_graph__CtorParams, socxml_path, _BP, 1), \
      _m(inu_graph__CtorParams, calibPathsP, _BP, 1));
   INIT_M_ENTRY(inu_log__configT, 9, \
      _m(inu_log__configT, log_levels, _B, NUM_OF_PROCESSORS), \
      _m(inu_log__configT, quiet, _B, 1), \
      _m(inu_log__configT, fp, _BP, 1), \
      _m(inu_log__configT, hostPrintLogs, _B, 1), \
      _m(inu_log__configT, hostSendLogs, _B, 1), \
      _m(inu_log__configT, hostPrintGpLogs, _B, 1), \
      _m(inu_log__configT, hostSendGpLogs, _B, 1), \
      _m(inu_log__configT, gpPrintLogs, _B, 1), \
      _m(inu_log__configT, gpSendLogs, _B, 1));
   INIT_M_ENTRY(inu_logger__CtorParams, 2, \
      _m(inu_logger__CtorParams, ref_params, inu_ref__CtorParams, 1), \
      _m(inu_logger__CtorParams, logConfig, inu_log__configT, 1));
   INIT_M_ENTRY(inu_node__CtorParams,        2, \
      _m(inu_node__CtorParams, ref_params, inu_ref__CtorParams, 1), \
      _m(inu_node__CtorParams, graph_ref_id, _B, 1));
   INIT_M_ENTRY(inu_function__CtorParams,    7, \
      _m(inu_function__CtorParams, nodeCtorParams, inu_node__CtorParams, 1), \
      _m(inu_function__CtorParams, workPriority, _B, 1),\
      _m(inu_function__CtorParams, pipeMaxSize, _B, 1),\
      _m(inu_function__CtorParams, inputQueDepth, _B, 1),\
      _m(inu_function__CtorParams, minInptsToOprt, _B, 1),\
      _m(inu_function__CtorParams, mode, _B, 1),\
      _m(inu_function__CtorParams, syncedFunc, _B, 1));
   INIT_M_ENTRY(inu_data__CtorParams,        2, \
      _m(inu_data__CtorParams, nodeCtorParams, inu_node__CtorParams, 1), \
      _m(inu_data__CtorParams, sourceNode, inu_ref__container, 1));
   INIT_M_ENTRY(inu_image__descriptor,        12, \
      _m(inu_image__descriptor, width, _B, 1), \
      _m(inu_image__descriptor, height, _B, 1), \
      _m(inu_image__descriptor, format, _B, 1), \
      _m(inu_image__descriptor, formatDiscriptor, _B, 1), \
      _m(inu_image__descriptor, x, _B, 1), \
      _m(inu_image__descriptor, y, _B, 1), \
      _m(inu_image__descriptor, stride, _B, 1), \
      _m(inu_image__descriptor, bufferHeight, _B, 1), \
      _m(inu_image__descriptor, bitsPerPixel, _B, 1), \
      _m(inu_image__descriptor, realBitsMask, _B, 1), \
      _m(inu_image__descriptor, numInterleaveImages, _B, 1),
      _m(inu_image__descriptor, interMode, _B, 1));
   INIT_M_ENTRY(inu_image__userInfoT,        2, \
      _m(inu_image__userInfoT, param1, _B, 1), \
      _m(inu_image__userInfoT, param2, _B, 1));
   INIT_M_ENTRY(inu_image__CtorParams,       3, \
      _m(inu_image__CtorParams, dataCtorParams, inu_data__CtorParams, 1), \
      _m(inu_image__CtorParams, imageDescriptor, inu_image__descriptor, 1),
      _m(inu_image__CtorParams, numLinesPerChunk, _B, 1));
   INIT_M_ENTRY(inu_cva_data__descriptor_t,        6, \
      _m(inu_cva_data__descriptor_t, width, _B, 1), \
      _m(inu_cva_data__descriptor_t, height, _B, 1), \
      _m(inu_cva_data__descriptor_t, format, _B, 1), \
      _m(inu_cva_data__descriptor_t, kpNum, _B, 1), \
      _m(inu_cva_data__descriptor_t, bpp, _B, 1), \
      _m(inu_cva_data__descriptor_t, type, _B, 1));
   INIT_M_ENTRY(inu_cva_data__CtorParams,       2, \
      _m(inu_cva_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1), \
      _m(inu_cva_data__CtorParams, cvaDescriptor, inu_cva_data__descriptor_t, 1));
   INIT_M_ENTRY(inu_soc_channel__CtorParams, 3, \
      _m(inu_soc_channel__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_soc_channel__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_soc_channel__CtorParams, configName, _B, 128));
   INIT_M_ENTRY(inu_mipi_channel__CtorParams, 3, \
      _m(inu_mipi_channel__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_mipi_channel__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_mipi_channel__CtorParams, configName, _B, 128));
   INIT_M_ENTRY(inu_cva_channel__CtorParams, 3, \
      _m(inu_cva_channel__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_cva_channel__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_cva_channel__CtorParams, configName, _B, 128));
   INIT_M_ENTRY(inu_isp_channel__CtorParams, 3, \
      _m(inu_isp_channel__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_isp_channel__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_isp_channel__CtorParams, configName, _B, 128));
   INIT_M_ENTRY(inu_streamer__CtorParams, 4, \
      _m(inu_streamer__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_streamer__CtorParams, streamDirection, _B, 1), \
      _m(inu_streamer__CtorParams, connId, _B, 1), \
      _m(inu_streamer__CtorParams, numBuffers, _B, 1));
   INIT_M_ENTRY(inu_compress__CtorParams, 1, _m(inu_compress__CtorParams, functionCtorParams, inu_function__CtorParams, 1));
   INIT_M_ENTRY(inu_sensor__resolution_t, 2, \
      _m(inu_sensor__resolution_t, width, _B, 1), \
      _m(inu_sensor__resolution_t, height, _B, 1));
   INIT_M_ENTRY(inu_sensor__parameter_list_t, 30, \
      _m(inu_sensor__parameter_list_t, id, _B, 1), \
      _m(inu_sensor__parameter_list_t, role, _B, 1), \
      _m(inu_sensor__parameter_list_t, op_mode, _B, 1), \
      _m(inu_sensor__parameter_list_t, tableType, _B, 1), \
      _m(inu_sensor__parameter_list_t, function, _B, 1), \
      _m(inu_sensor__parameter_list_t, fps, _B, 1), \
      _m(inu_sensor__parameter_list_t, power_hz, _B, 1), \
      _m(inu_sensor__parameter_list_t, defaultExp, _B, 1), \
      _m(inu_sensor__parameter_list_t, totalExpTimePerSec, _B, 1), \
      _m(inu_sensor__parameter_list_t, expTimePerDutyCycle, _B, 1), \
      _m(inu_sensor__parameter_list_t, trigger_src, _B, 1), \
      _m(inu_sensor__parameter_list_t, trigger_delay, _B, 1), \
      _m(inu_sensor__parameter_list_t, orientation, _B, 1), \
      _m(inu_sensor__parameter_list_t, triggerExtGpio, _B, 1), \
      _m(inu_sensor__parameter_list_t, vertical_offset, _B, 1), \
      _m(inu_sensor__parameter_list_t, gain, inu_sensor__gain_t, 1), \
      _m(inu_sensor__parameter_list_t, i2cNum, _B, 1), \
      _m(inu_sensor__parameter_list_t, power_gpio_master, _B, 1), \
      _m(inu_sensor__parameter_list_t, power_gpio_slave, _B, 1), \
      _m(inu_sensor__parameter_list_t, sensor_clk_src, _B, 1), \
      _m(inu_sensor__parameter_list_t, sensor_clk_divider, _B, 1), \
      _m(inu_sensor__parameter_list_t, fsin_gpio, _B, 1), \
      _m(inu_sensor__parameter_list_t, exp_mode, _B, 1), \
      _m(inu_sensor__parameter_list_t, strobeNum, _B, 1), \
      _m(inu_sensor__parameter_list_t, groupId, _B, 1), \
      _m(inu_sensor__parameter_list_t, sensor_width, _B, 1), \
      _m(inu_sensor__parameter_list_t, sensor_height, _B, 1), \
      _m(inu_sensor__parameter_list_t, model, _B, 1), \
      _m(inu_sensor__parameter_list_t, lenType, _B, 1), \
      _m(inu_sensor__parameter_list_t, projector_inst, _B, 1));
   INIT_M_ENTRY(inu_sensor__CtorParams, 2, \
      _m(inu_sensor__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_sensor__CtorParams, params, inu_sensor__parameter_list_t, 1));
   INIT_M_ENTRY(inu_sensors_group__CtorParams, 3, \
      _m(inu_sensors_group__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_sensors_group__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_sensors_group__CtorParams, masterParamsList, inu_sensor__parameter_list_t, 1));
   INIT_M_ENTRY(inu_fdk__CtorParams, 4, \
      _m(inu_fdk__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_fdk__CtorParams, nodeId, _B, 1), \
      _m(inu_fdk__CtorParams, numBuffs, _B, 1), \
      _m(inu_fdk__CtorParams, buffSize, _B, 1));
   INIT_M_ENTRY(inu_imu_data__chDescriptorT, 11, \
      _m(inu_imu_data__chDescriptorT, scale, _B, 1), \
      _m(inu_imu_data__chDescriptorT, offset, _B, 1), \
      _m(inu_imu_data__chDescriptorT, bytes, _B, 1), \
      _m(inu_imu_data__chDescriptorT, bits_used, _B, 1), \
      _m(inu_imu_data__chDescriptorT, maskL, _B, 1), \
      _m(inu_imu_data__chDescriptorT, maskH, _B, 1), \
      _m(inu_imu_data__chDescriptorT, is_signed, _B, 1), \
      _m(inu_imu_data__chDescriptorT, location, _B, 1), \
      _m(inu_imu_data__chDescriptorT, be, _B, 1), \
      _m(inu_imu_data__chDescriptorT, shift, _B, 1), \
      _m(inu_imu_data__chDescriptorT, channelType, _B, 1));
   INIT_M_ENTRY(inu_imu_data__descriptorT, 3, \
      _m(inu_imu_data__descriptorT, chDescriptors, inu_imu_data__chDescriptorT,INU_IMU_DATA__SENSOR_NUM_TYPES_E * INU_IMU_DATA__SENSOR_CHANNEL_NUM_TYPES_E), \
      _m(inu_imu_data__descriptorT, num_channels, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E),
      _m(inu_imu_data__descriptorT, shiftTs, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E));
   INIT_M_ENTRY(inu_imu__CtorParams, 8, \
      _m(inu_imu__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_imu__CtorParams, scale, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E), \
      _m(inu_imu__CtorParams, odr, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E), \
      _m(inu_imu__CtorParams, accelBw, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E), \
      _m(inu_imu__CtorParams, gyroBw, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E), \
      _m(inu_imu__CtorParams, batchSize, _B, INU_IMU_DATA__SENSOR_NUM_TYPES_E), \
       _m(inu_imu__CtorParams, descriptor, inu_imu_data__descriptorT, 1), \
      _m(inu_imu__CtorParams, IMU_paired_batching_batchSize, _B, 1));
   INIT_M_ENTRY(inu_imu_data__CtorParams, 2, \
      _m(inu_imu_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1), \
      _m(inu_imu_data__CtorParams, descriptor, inu_imu_data__descriptorT, 1));
   INIT_M_ENTRY(inu_imu_data__hdr_t, 5, \
      _m(inu_imu_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_imu_data__hdr_t, sensorType, _B, 1), \
      _m(inu_imu_data__hdr_t, batchCount, _B, 1), \
      _m(inu_imu_data__hdr_t, batchSizeBytes, _B, 1), \
      _m(inu_imu_data__hdr_t, metadata, _B, sizeof(inu_imu_data__metadataT)));
   INIT_M_ENTRY(inu_temperature__CtorParams, 2, \
      _m(inu_temperature__CtorParams, functionCtorParams,  inu_function__CtorParams, 1), \
      _m(inu_temperature__CtorParams, fps,          _B, 1));
   INIT_M_ENTRY(inu_temperature_data__CtorParams, 1, _m(inu_temperature_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
   INIT_M_ENTRY(inu_temperature_data__hdr_t, 7, \
      _m(inu_temperature_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_temperature_data__hdr_t, imuTemp, _B, 1), \
      _m(inu_temperature_data__hdr_t, sensorTempCvr, _B, 1), \
      _m(inu_temperature_data__hdr_t, sensorTempCvl, _B, 1), \
      _m(inu_temperature_data__hdr_t, chipTemp, _B, 1), \
      _m(inu_temperature_data__hdr_t, chipVoltage, _B, 1), \
      _m(inu_temperature_data__hdr_t, chipProcess, _B, PD_SLAVES));
   INIT_M_ENTRY(inu_function__startParamsT, 1, _m(inu_function__startParamsT, dummy, _B, 1));
   INIT_M_ENTRY(inu_function__stopParamsT, 1, _m(inu_function__stopParamsT, dummy, _B, 1));
   INIT_M_ENTRY(inu_data__hdr_t, 4, \
      _m(inu_data__hdr_t, timestamp, _B, 1), \
      _m(inu_data__hdr_t, dataIndex, _B, 1), \
      _m(inu_data__hdr_t, chunkNum, _B, 1), \
      _m(inu_data__hdr_t, flags, _B, 1));
   INIT_M_ENTRY(inu_image__sensorInfoT, 4, \
      _m(inu_image__sensorInfoT, exposure, _B, 1), \
      _m(inu_image__sensorInfoT, analogGain, _B, 1), \
      _m(inu_image__sensorInfoT, digitalGain, _B, 1), \
      _m(inu_image__sensorInfoT, nucfgId, _B, 1));
   INIT_M_ENTRY(inu_image__hdr_t, 6, \
      _m(inu_image__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_image__hdr_t, imgDescriptor, inu_image__descriptor, 1), \
      _m(inu_image__hdr_t, exposureGain, inu_image__sensorInfoT, NUCFG_MAX_CHAN_INPUTS), \
      _m(inu_image__hdr_t, userInfo, inu_image__userInfoT, 1), \
      _m(inu_image__hdr_t, projMode, _B, 1), \
      _m(inu_image__hdr_t, sensorGroup, _B, 1), \
      _m(inu_image__hdr_t, readoutTs, _B, 1));
   INIT_M_ENTRY(inu_cva_data__hdr_t, 3, \
      _m(inu_cva_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_cva_data__hdr_t, cvaDescriptor, inu_cva_data__descriptor_t, 1), \
      _m(inu_cva_data__hdr_t, readoutTs, _B, 1));
   INIT_M_ENTRY(inu_point_cloud_data__hdr_t, 2, \
      _m(inu_point_cloud_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_point_cloud_data__hdr_t, pointCloudListSize, _B, 1));
   INIT_M_ENTRY(inu_fast_orb_data__hdr_t, 5, \
      _m(inu_fast_orb_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_fast_orb_data__hdr_t, numKeyPoints, _B, 1), \
      _m(inu_fast_orb_data__hdr_t, yOffset, _B, 1), \
      _m(inu_fast_orb_data__hdr_t, scoreOffset, _B, 1), \
      _m(inu_fast_orb_data__hdr_t, descOffset, _B, 1));
   INIT_M_ENTRY(inu_cdnn_data__tailCevaFieldsT, 1, \
      _m(inu_cdnn_data__tailCevaFieldsT, fracBits, _B, 1));
   INIT_M_ENTRY(inu_cdnn_data__tailSynopsysFieldsT, 5, \
      _m(inu_cdnn_data__tailSynopsysFieldsT, scale, _B, 1), \
      _m(inu_cdnn_data__tailSynopsysFieldsT, allocCount, _B, 1), \
      _m(inu_cdnn_data__tailSynopsysFieldsT, validCount, _B, 1), \
      _m(inu_cdnn_data__tailSynopsysFieldsT, bboxScale, _B, 1), \
      _m(inu_cdnn_data__tailSynopsysFieldsT, confidenceScale, _B, 1));
   INIT_M_ENTRY(inu_cdnn_data__tailFieldsT, 2, \
      _m(inu_cdnn_data__tailFieldsT, tailCevaFields, inu_cdnn_data__tailCevaFieldsT, 1), \
      _m(inu_cdnn_data__tailFieldsT, tailSynopsysFields, inu_cdnn_data__tailSynopsysFieldsT, 1));
   INIT_M_ENTRY(inu_cdnn_data__tailHeaderT, 6, \
      _m(inu_cdnn_data__tailHeaderT, elementSize, _B, 1), \
      _m(inu_cdnn_data__tailHeaderT, elementCount, _B, 1), \
      _m(inu_cdnn_data__tailHeaderT, width, _B, 1), \
      _m(inu_cdnn_data__tailHeaderT, height, _B, 1), \
      _m(inu_cdnn_data__tailHeaderT, inputs, _B, 1),  \
      _m(inu_cdnn_data__tailHeaderT, cnnFields, inu_cdnn_data__tailFieldsT, 1));
   INIT_M_ENTRY(inu_cdnn_data__hdr_t, 8, \
      _m(inu_cdnn_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_cdnn_data__hdr_t, totOutputSize, _B, 1), \
      _m(inu_cdnn_data__hdr_t, frameId, _B, 1), \
      _m(inu_cdnn_data__hdr_t, numOfTails, _B, 1), \
      _m(inu_cdnn_data__hdr_t, network_id, _B, 1), \
      _m(inu_cdnn_data__hdr_t, engineType, _B, 1), \
      _m(inu_cdnn_data__hdr_t, tailHeader, inu_cdnn_data__tailHeaderT, CDNNG_MAX_TAILS), \
      _m(inu_cdnn_data__hdr_t, offsetToBlob, _B, CDNNG_MAX_TAILS));
   INIT_M_ENTRY(inu_slam_data__hdr_t, 5, \
      _m(inu_slam_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_slam_data__hdr_t, ddrP, _B, 1), \
      _m(inu_slam_data__hdr_t, totOutputSize, _B, 1), \
      _m(inu_slam_data__hdr_t, slamImuDataSampleNum, _B, 1), \
      _m(inu_slam_data__hdr_t, frameId, _B, 1));
   INIT_M_ENTRY(inu_soc_writer__CtorParams, 4, \
      _m(inu_soc_writer__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_soc_writer__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_soc_writer__CtorParams, configName, _B, 128), \
      _m(inu_soc_writer__CtorParams, imageDescriptor, inu_image__descriptor, 1));
   INIT_M_ENTRY(inu_histogram__CtorParams, 4, \
      _m(inu_histogram__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_histogram__CtorParams, nuCfgChId, _B, 1), \
      _m(inu_histogram__CtorParams, configName, _B, 128), \
      _m(inu_histogram__CtorParams, engine, _B, 1));
   INIT_M_ENTRY(inu_histogram_data__CtorParams, 1, \
      _m(inu_histogram_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
   INIT_M_ENTRY(inu_histogram__roiT, 4, \
      _m(inu_histogram__roiT, x0, _B, 1), \
      _m(inu_histogram__roiT, x1, _B, 1), \
      _m(inu_histogram__roiT, y0, _B, 1), \
      _m(inu_histogram__roiT, y1, _B, 1));
   INIT_M_ENTRY(inu_histogram_data__hdr_t, 4, \
      _m(inu_histogram_data__hdr_t, dataHdr, inu_data__hdr_t, 1), \
      _m(inu_histogram_data__hdr_t, roi, inu_histogram__roiT, INU_HISTOGRAM__ROI_NUM), \
      _m(inu_histogram_data__hdr_t, accumulator, _B, INU_HISTOGRAM__ROI_NUM), \
      _m(inu_histogram_data__hdr_t, hwFrameCounter, _B, 1));
   INIT_M_ENTRY(inu_histogram__roiCfgT, 1, \
      _m(inu_histogram__roiCfgT, roi, inu_histogram__roiT, INU_HISTOGRAM__ROI_NUM));
   INIT_M_ENTRY(inu_soc_channel__cropParamsT, 2, \
      _m(inu_soc_channel__cropParamsT, xStart, _B, 1), \
      _m(inu_soc_channel__cropParamsT, yStart, _B, 1));
   INIT_M_ENTRY(inu_soc_channel__userParamsT, 2, \
      _m(inu_soc_channel__userParamsT, param1, _B, 1), \
      _m(inu_soc_channel__userParamsT, param2, _B, 1));
   INIT_M_ENTRY(inu_sensor_control__parameter_list_t, 15, \
      _m(inu_sensor_control__parameter_list_t, operationMode, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, nSatMax, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, glSat, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, mdMax, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, mdMin, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, exposureStepResolution, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, deltaSatMax, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, aggressiveStep, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, noActivationPeriod, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, exposureMax, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, exposureMin, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, debug, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, snrTarget, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, slopeWeight, _B, 1), \
      _m(inu_sensor_control__parameter_list_t, algVersion, _B, 1));
   INIT_M_ENTRY(inu_sensor_control__CtorParams, 2, \
      _m(inu_sensor_control__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_sensor_control__CtorParams, params, inu_sensor_control__parameter_list_t, 1));
   INIT_M_ENTRY(inu_graph__send_config_db_params, 2, \
      _m(inu_graph__send_config_db_params, ret, _B, 1), \
      _m(inu_graph__send_config_db_params, name, _B, 128));
   INIT_M_ENTRY(inu_device__refListEntry, 2, \
      _m(inu_device__refListEntry, id, _B, 1), \
      _m(inu_device__refListEntry, refType, _B, 1));
   INIT_M_ENTRY(inu_device__refList, 2, \
      _m(inu_device__refList, numberOfRefs, _B, 1), \
      _m(inu_device__refList, refMember, inu_device__refListEntry, INU_DEVICE__MAX_NUM_OF_REFS));
   INIT_M_ENTRY(inu_fast_orb__CtorParams, 1, \
      _m(inu_fast_orb__CtorParams, functionCtorParams, inu_function__CtorParams, 1));
   INIT_M_ENTRY(inu_fast_orb_data__CtorParams, 1, \
      _m(inu_fast_orb_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
    INIT_M_ENTRY(inu_imu__temperatureParams, 1, \
      _m(inu_imu__temperatureParams, temperature, _B, 1));
   INIT_M_ENTRY(inu_cdnn__CtorParams, 1, \
      _m(inu_cdnn__CtorParams, functionCtorParams, inu_function__CtorParams, 1));
   INIT_M_ENTRY(inu_cdnn_data__CtorParams, 1, \
      _m(inu_cdnn_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
   INIT_M_ENTRY(inu_cdnn__networkIdSetT, 1, \
      _m(inu_cdnn__networkIdSetT, network_id, _B, 1));
   INIT_M_ENTRY(inu_vision_proc__CtorParams, 1, \
       _m(inu_vision_proc__CtorParams, functionCtorParams, inu_function__CtorParams, 1));
   INIT_M_ENTRY(VISION_PROCG_bokehParamsT, 8, \
      _m(VISION_PROCG_bokehParamsT, visionProcResolution, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, visionProcCamera, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, blurLevel, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, depthOutput, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, faceBoxDraw, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, faceSmoothFactor, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, noFaceFramesToKeepBlur, _B, 1), \
      _m(VISION_PROCG_bokehParamsT, multiFaceMode, _B, 1));
   INIT_M_ENTRY(VISION_PROCG_backgroundRemovalParamsT, 2, \
       _m(VISION_PROCG_backgroundRemovalParamsT, visionProcResolution, _B, 1), \
       _m(VISION_PROCG_backgroundRemovalParamsT, backgroundImage, _B, 1));
   INIT_M_ENTRY(VISION_PROCG_lowLightIspParamsT, 2, \
      _m(VISION_PROCG_lowLightIspParamsT, visionProcResolution, _B, 1), \
      _m(VISION_PROCG_lowLightIspParamsT, stride, _B, 1));
   INIT_M_ENTRY(VISION_PROCG_autoFramingParamsT, 4, \
      _m(VISION_PROCG_autoFramingParamsT, visionProcResolution, _B, 1), \
      _m(VISION_PROCG_autoFramingParamsT, maxZoom, _B, 1), \
      _m(VISION_PROCG_autoFramingParamsT, trackingSpeed, _B, 1), \
      _m(VISION_PROCG_autoFramingParamsT, multiplePersonDetection, _B, 1));
   INIT_M_ENTRY(inu_vision_proc__parameter_list_t, 5, \
      _m(inu_vision_proc__parameter_list_t, algType, _B, 1), \
      _m(inu_vision_proc__parameter_list_t, bokehParams, VISION_PROCG_bokehParamsT, 1), \
      _m(inu_vision_proc__parameter_list_t, backgroundRemovalParams, VISION_PROCG_backgroundRemovalParamsT, 1), \
      _m(inu_vision_proc__parameter_list_t, lowLightIspParams, VISION_PROCG_lowLightIspParamsT, 1), \
      _m(inu_vision_proc__parameter_list_t, autoFramingParams, VISION_PROCG_autoFramingParamsT, 1));
   INIT_M_ENTRY(StereoMatcherT, 4, \
      _m(StereoMatcherT, secondBestTh_100, _B, 1), \
      _m(StereoMatcherT, radiusThreshold, _B, 1), \
      _m(StereoMatcherT, HammingDistanceThreshold, _B, 1), \
      _m(StereoMatcherT, scaleDifferenceThreshold, _B, 1));
   INIT_M_ENTRY(FrameMatcherT, 4, \
      _m(FrameMatcherT, ratioThreshold_100, _B, 1), \
      _m(FrameMatcherT, radiusThreshold, _B, 1), \
      _m(FrameMatcherT, HammingDistanceThreshold, _B, 1), \
      _m(FrameMatcherT, scaleDifferenceThreshold, _B, 1));
   INIT_M_ENTRY(KeyframesT, 5, \
      _m(KeyframesT, UseKeyframes, _B, 1), \
      _m(KeyframesT, numberOfKeyframesForMatching, _B, 1), \
      _m(KeyframesT, connectedKeyframesThreshold, _B, 1), \
      _m(KeyframesT, numTrackedClosePointsThreshold, _B, 1), \
      _m(KeyframesT, numNonTrackedClosePointsThreshold, _B, 1));
   INIT_M_ENTRY(MapPointCullingT, 4, \
      _m(MapPointCullingT, mapPointCullRatioThr, _B, 1), \
      _m(MapPointCullingT, observingKeyframesThr, _B, 1), \
      _m(MapPointCullingT, keyframeIndexDiffThr, _B, 1), \
      _m(MapPointCullingT, keyframeIndexDiffThr2, _B, 1));
   INIT_M_ENTRY(KeyframeCullingT, 2, \
      _m(KeyframeCullingT, observationThreshold, _B, 1), \
      _m(KeyframeCullingT, redundantMapPointThreshold_100, _B, 1));
   INIT_M_ENTRY(TrackingT, 11, \
      _m(TrackingT, MaximumDepthThreshold, _B, 1), \
      _m(TrackingT, minimumDepthThreshold, _B, 1), \
      _m(TrackingT, ClosePointsDepthThreshold, _B, 1), \
      _m(TrackingT, rhoThresholdMono, _B, 1), \
      _m(TrackingT, rhoThresholdStereo, _B, 1), \
      _m(TrackingT, StereoMatcherSt, StereoMatcherT, 1), \
      _m(TrackingT, FrameMatcherSt, FrameMatcherT, 1), \
      _m(TrackingT, KeyframesSt, KeyframesT, 1), \
      _m(TrackingT, KeyframeMatcherSt, FrameMatcherT, 1), \
      _m(TrackingT, MapPointCullingSt, MapPointCullingT, 1), \
      _m(TrackingT, KeyframeCullingSt, KeyframeCullingT, 1));
   INIT_M_ENTRY(LocalBundleAdjustmentT, 2, \
      _m(LocalBundleAdjustmentT, RunLocalBundleAdjustment, _B, 1), \
      _m(LocalBundleAdjustmentT, numberOfKeyframesForLBA, _B, 1));
   INIT_M_ENTRY(LocalizationT, 1, \
      _m(LocalizationT, RunRelocalization, _B, 1));
   INIT_M_ENTRY(inu_slam__parameter_list_t, 14, \
      _m(inu_slam__parameter_list_t, baseline, _B, 1), \
      _m(inu_slam__parameter_list_t, fx, _B, 1), \
      _m(inu_slam__parameter_list_t, fy, _B, 1), \
      _m(inu_slam__parameter_list_t, cx, _B, 1), \
      _m(inu_slam__parameter_list_t, cy, _B, 1), \
      _m(inu_slam__parameter_list_t, IMU_freq, _B, 1), \
      _m(inu_slam__parameter_list_t, NoiseGyro, _B, 1), \
      _m(inu_slam__parameter_list_t, NoiseAcc, _B, 1), \
      _m(inu_slam__parameter_list_t, GyroWalk, _B, 1), \
      _m(inu_slam__parameter_list_t, AccWalk, _B, 1), \
      _m(inu_slam__parameter_list_t, TrackingSt, TrackingT, 1), \
      _m(inu_slam__parameter_list_t, LocalBundleAdjustmentSt, LocalBundleAdjustmentT, 1), \
      _m(inu_slam__parameter_list_t, LocalizationSt, LocalizationT, 1), \
      _m(inu_slam__parameter_list_t, fps, _B, 1));
   INIT_M_ENTRY(inu_slam__CtorParams, 2, \
      _m(inu_slam__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_slam__CtorParams, params, inu_slam__parameter_list_t, 1));
   INIT_M_ENTRY(inu_slam_data__CtorParams, 1, \
      _m(inu_slam_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
   INIT_M_ENTRY(inu_load_network__CtorParams, 2, \
      _m(inu_load_network__CtorParams, ref_params, inu_ref__CtorParams, 1), \
      _m(inu_load_network__CtorParams, initWorkSpace,  _B, 1));
   INIT_M_ENTRY(inu_load_background__CtorParams, 1, \
      _m(inu_load_background__CtorParams, ref_params, inu_ref__CtorParams, 1));
    INIT_M_ENTRY(inu_device__readWriteBuf, 6, \
      _m(inu_device__readWriteBuf, address, _B, 1), \
      _m(inu_device__readWriteBuf, bufLen, _B, 1), \
      _m(inu_device__readWriteBuf, numOfChunks, _B, 1), \
      _m(inu_device__readWriteBuf, chunkLen, _B, 1), \
      _m(inu_device__readWriteBuf, chunkId,  _B, 1),  \
      _m(inu_device__readWriteBuf, buf, _BP, 1));
   INIT_M_ENTRY(inu_device__pwmConfigT, 4, \
      _m(inu_device__pwmConfigT, pwmNum , _B, 1), \
      _m(inu_device__pwmConfigT, fps , _B, 1), \
      _m(inu_device__pwmConfigT, widthInUsec , _B, 1), \
      _m(inu_device__pwmConfigT, pwmCmd , _B, 1));
   INIT_M_ENTRY(inu_device__calibUpdateT, 1, \
      _m(inu_device__calibUpdateT, dblBuffId, _B, 1));
   INIT_M_ENTRY(inu_tsnr_calc__parameter_list_t, 5, \
      _m(inu_tsnr_calc__parameter_list_t, saturatedGrayLevelValue, _B, 1), \
      _m(inu_tsnr_calc__parameter_list_t, saturatedThreshold, _B, 1), \
      _m(inu_tsnr_calc__parameter_list_t, allowedBadSNRBlocksThreshold, _B, 1), \
      _m(inu_tsnr_calc__parameter_list_t, saturationThresholdInImage, _B, 1), \
      _m(inu_tsnr_calc__parameter_list_t, sideToCalc, _B, 1));
   INIT_M_ENTRY(inu_tsnr_data__hdr_t, 1, \
      _m(inu_tsnr_data__hdr_t, paramsList, inu_tsnr_calc__parameter_list_t, 1));
   INIT_M_ENTRY(inu_tsnr_calc__CtorParams, 2, \
      _m(inu_tsnr_calc__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_tsnr_calc__CtorParams, params, inu_tsnr_calc__parameter_list_t, 1));
   INIT_M_ENTRY(inu_tsnr_data__CtorParams, 1, \
      _m(inu_tsnr_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
   INIT_M_ENTRY(inu_dpe_pp__CtorParams, 1, \
      _m(inu_dpe_pp__CtorParams, functionCtorParams, inu_function__CtorParams, 1));
   INIT_M_ENTRY(inu_isp_channel__commandT, 2, \
      _m(inu_isp_channel__commandT, cmd, _B, 1), \
      _m(inu_isp_channel__commandT, cmdParams, _B, 32));
   INIT_M_ENTRY(inu_pp__parameter_list_t, 16, \
      _m(inu_pp__parameter_list_t, sensorGroup, _B, 1), \
      _m(inu_pp__parameter_list_t, baseline, _B, 1), \
      _m(inu_pp__parameter_list_t, fx, _B, 1), \
      _m(inu_pp__parameter_list_t, fy, _B, 1), \
      _m(inu_pp__parameter_list_t, cxL, _B, 1), \
      _m(inu_pp__parameter_list_t, cxR, _B, 1), \
      _m(inu_pp__parameter_list_t, cy, _B, 1), \
      _m(inu_pp__parameter_list_t, maxDepthMm, _B, 1), \
      _m(inu_pp__parameter_list_t, voxel_leaf_x_size, _B, 1), \
      _m(inu_pp__parameter_list_t, voxel_leaf_y_size, _B, 1), \
      _m(inu_pp__parameter_list_t, voxel_leaf_z_size, _B, 1), \
      _m(inu_pp__parameter_list_t, vgf_flag, _B, 1), \
      _m(inu_pp__parameter_list_t, flip_x, _B, 1), \
      _m(inu_pp__parameter_list_t, flip_y, _B, 1), \
      _m(inu_pp__parameter_list_t, flip_z, _B, 1), \
      _m(inu_pp__parameter_list_t, model, _B, 1));
   INIT_M_ENTRY(inu_device__selectTempSensor_t, 1, \
      _m(inu_device__selectTempSensor_t, tempSensorNum, _B, 1));
   INIT_M_ENTRY(inu_pp__CtorParams, 3, \
      _m(inu_pp__CtorParams, functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_pp__CtorParams, actionMap, _B, 1), \
      _m(inu_pp__CtorParams, params, inu_pp__parameter_list_t, 1));
   INIT_M_ENTRY(inu_device__setflashSectionHdr, 5, \
      _m(inu_device__setflashSectionHdr, versionNumber, _B, 1), \
      _m(inu_device__setflashSectionHdr, sectionDataSize, _B, 1), \
      _m(inu_device__setflashSectionHdr, sectionFormat, _B, 1), \
      _m(inu_device__setflashSectionHdr, timestamp, _B, 1), \
      _m(inu_device__setflashSectionHdr, versionString, _B, 32));


   INIT_M_ENTRY(inu_point_cloud_data__CtorParams, 1, \
      _m(inu_point_cloud_data__CtorParams, dataCtorParams, inu_data__CtorParams, 1));
   INIT_M_ENTRY(inu_device__sensorUpdateT, 1, _m(inu_device__sensorUpdateT, buffer, _B, sizeof(inu_device__sensorUpdateT)));   /*INU_metadata_TopLevel_size + 64 bytes for spare fixed data*/
   INIT_M_ENTRY(inu_device__sensorSync, 1, _m(inu_device__sensorSync, buffer, _B, sizeof(inu_device__sensorSync)));   /*128 Bytes */
   INIT_M_ENTRY(inu_device__dphyRegT, 3, \
       _m(inu_device__dphyRegT, dphyNum, _B, 1), \
       _m(inu_device__dphyRegT, addr, _B, 1), \
       _m(inu_device__dphyRegT, val, _B, 1));
   INIT_M_ENTRY(inu_device__tempRegT, 3, \
       _m(inu_device__tempRegT, addr, _B, 1), \
       _m(inu_device__tempRegT, vali, _B, 1), \
       _m(inu_device__tempRegT, valf, _B, 1));
   INIT_M_ENTRY(inu_sensor__getFocusParams_t, 5, \
      _m(inu_sensor__getFocusParams_t, dac, _B, 1), \
      _m(inu_sensor__getFocusParams_t, min, _B, 1), \
      _m(inu_sensor__getFocusParams_t, max, _B, 1), \
      _m(inu_sensor__getFocusParams_t, chipId, _B, 1), \
      _m(inu_sensor__getFocusParams_t, context, _B, 1));
   INIT_M_ENTRY(inu_sensor__setFocusParams_t, 3, \
      _m(inu_sensor__setFocusParams_t, mode, _B, 1), \
      _m(inu_sensor__setFocusParams_t, dac, _B, 1), \
      _m(inu_sensor__setFocusParams_t, context, _B, 1));
   INIT_M_ENTRY(inu_metadata_injector__CtorParams, 2, \
      _m(inu_metadata_injector__CtorParams,functionCtorParams, inu_function__CtorParams, 1), \
      _m(inu_metadata_injector__CtorParams, readerID, _B, 1));
   sanity_check_tbl();

   set_tbl_builtin_ids();
   set_tbl_m_sizes();

   init_marshal_id_lut();

   hash_mtbl();
}

void deinit_marshal_tbl()
{
#ifndef M_STATIC_MEMBERS
   unsigned int i;
   for (i = 0; i < M_NUM_COMPOUND_TYPES_E; i++)
   {
      if (marshal_tbl[i].members)
      {
         free(marshal_tbl[i].members);
         marshal_tbl[i].members = NULL;
      }
   }
#endif
}

marshal_entry_t *marshal_tbl_entry(unsigned int index)
{
   return &marshal_tbl[index];
}

marshal_id_e  marshal_id_lut_entry(int sid, unsigned int ioctl)
{
   if (ioctl >= sizeof(marshal_id_lut[sid])/sizeof(marshal_id_lut[sid][0]))
      return M_INVALID_ENTRY;

   return marshal_id_lut[sid][ioctl];
}

void show_marshal_tbl_stats()
{
   LOGG_PRINT(LOG_INFO_E, NULL, "tbl_size = %d num_entries= %d(size=%d) num_members = %d(size=%d)\nlut_size=%d\n",
      sizeof(marshal_tbl) + marshal_tbl_memb_cnt*sizeof(marshal_memb_t), M_NUM_COMPOUND_TYPES_E, sizeof(marshal_entry_t),
      marshal_tbl_memb_cnt, sizeof(marshal_memb_t), sizeof(marshal_id_lut));
}

UINT32 get_marshal_tbl_hash(void)
{
   return mtbl_hash;
}

#ifdef __cplusplus
}
#endif
