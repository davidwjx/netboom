#ifndef INU_DEF_PARAMS
#define INU_DEF_PARAMS


#define INU_DEFS_PARAMS_FILE_NAME      "FwInuDefs.txt"
#define INU_DEFS_PARAMS_MAX_INJECT_SIZE_STR         "maximum_inject_size"
#define INU_DEFS_PARAMS_MAX_VIDEO_SIZE_STR          "maximum_video_size"
#define INU_DEFS_PARAMS_MAX_DEPTH_SIZE_STR          "maximum_depth_size"
#define INU_DEFS_PARAMS_MAX_WEBCAM_SIZE_STR         "maximum_webcam_size"

#define INU_DEF_PARAMS_GP_HOST_GENERAL_SIZE_STR 	"gp_host_general_size"
#define INU_DEF_PARAMS_GP_HOST_ALG_CTRL_SIZE_STR	"gp_host_alg_ctrl_size"
#define INU_DEF_PARAMS_GP_HOST_CLIENT_CTRL_SIZE_STR	"gp_host_client_ctrl_size"
#define INU_DEF_PARAMS_SVC_MNGR_DATA_SIZE_STR		"svc_mngr_data_size"
#define INU_DEF_PARAMS_GP_HOST_SYSTEM_SIZE_STR		"gp_host_system_size"
#define INU_DEF_PARAMS_GP_HOST_ALG_DATA_SIZE_STR	"gp_host_alg_data_size"
#define INU_DEF_PARAMS_GP_HOST_LOGGER_SIZE_STR		"gp_host_logger_size"
#define INU_DEF_PARAMS_GP_HOST_POS_SENSORS_SIZE_STR	"gp_host_pos_sensors_size"
#define INU_DEF_PARAMS_GP_HOST_CLIENT_DATA_SIZE_STR	"gp_host_client_data"


#define INU_DEF_INJECT_VGA_NUM_BUFS_STR    "inject_vga_num_bufs"
#define INU_DEF_INJECT_HD_NUM_BUFS_STR     "inject_hd_num_bufs"
#define INU_DEF_VIDEO_VGA_NUM_BUFS_STR     "video_vga_num_bufs"
#define INU_DEF_VIDEO_HD_NUM_BUFS_STR      "video_hd_num_bufs"
#define INU_DEF_DEPTH_VGA_NUM_BUFS_STR     "depth_vga_num_bufs"
#define INU_DEF_DEPTH_HD_NUM_BUFS_STR      "depth_hd_num_bufs"
#define INU_DEF_WEBCAM_VGA_NUM_BUFS_STR    "webcam_vga_num_bufs"
#define INU_DEF_WEBCAM_HD_NUM_BUFS_STR     "webcam_hd_num_bufs"
#define INU_DEF_CLIENT_CTRL_NUM_BUFS_STR     "client_ctrl_num_bufs"
#define INU_DEF_CLIENT_DATA_NUM_BUFS_STR     "client_data_num_bufs"



typedef struct
{
	char *str;
	char paramFound;
	int paramVal;
}INU_DEFS_PARAMS_configTblT;

typedef enum
{
	INU_DEF_MAX_INJECT_IND_E,
	INU_DEF_MAX_VIDEO_IND_E,
	INU_DEF_MAX_DEPTH_IND_E,
	INU_DEF_MAX_WEBCAM_IND_E,
	INU_DEF_GP_HOST_GENERAL_IND_E, 
	INU_DEF_GP_HOST_ALG_CTRL_IND_E, 
	INU_DEF_GP_HOST_CLIENT_CTRL_IND_E, 
	INU_DEF_SVC_MNGR_DATA_IND_E, 
	INU_DEF_GP_HOST_SYSTEM_IND_E, 
	INU_DEF_GP_HOST_ALG_DATA_IND_E, 
	INU_DEF_GP_HOST_LOGGER_IND_E, 
	INU_DEF_GP_HOST_POS_SENSORS_IND_E, 
	INU_DEF_GP_HOST_CLIENT_DATA_IND_E,
	INU_DEF_GP_HOST_MAX_MEM_POOL_IND_E = INU_DEF_GP_HOST_CLIENT_DATA_IND_E,

	INU_DEF_INJECT_VGA_NUM_BUFS_IND_E,
	INU_DEF_INJECT_HD_NUM_BUFS_IND_E,
	INU_DEF_VIDEO_VGA_NUM_BUFS_IND_E,
	INU_DEF_VIDEO_HD_NUM_BUFS_IND_E,
	INU_DEF_DEPTH_VGA_NUM_BUFS_IND_E,
	INU_DEF_DEPTH_HD_NUM_BUFS_IND_E,
	INU_DEF_WEBCAM_VGA_NUM_BUFS_IND_E,
	INU_DEF_WEBCAM_HD_NUM_BUFS_IND_E,


	INU_DEF_CLIENT_CTRL_NUM_BUFS_IND_E,
	INU_DEF_CLIENT_DATA_NUM_BUFS_IND_E,



	INU_DEF_NUM_PARAMS
}INU_DEFS_PARAMS_IndE;












BOOL INU_DEFS_PARAMS_overwriteValue(INU_DEFS_PARAMS_IndE inuDefVal, unsigned int *val);

#endif
