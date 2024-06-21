/****************************************************************************
 *
 *   FileName: sys_defs.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: system-wide internal definitions (resources and such)
 *   
 ****************************************************************************/
#ifndef SYS_DEFS_H
#define SYS_DEFS_H

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   SYS_DEFSG_CHANNEL_0_E = 0,
   SYS_DEFSG_CHANNEL_1_E,
   SYS_DEFSG_CHANNEL_2_E,
   SYS_DEFSG_CHANNEL_3_E,
   SYS_DEFSG_CHANNEL_4_E,
   SYS_DEFSG_CHANNEL_5_E,
   SYS_DEFSG_CHANNEL_6_E,
   SYS_DEFSG_CHANNEL_7_E,
   SYS_DEFSG_CHANNEL_8_E,
   SYS_DEFSG_CHANNEL_9_E,
   SYS_DEFSG_CHANNEL_10_E,
   SYS_DEFSG_CHANNEL_11_E,
   SYS_DEFSG_CHANNEL_12_E,
   SYS_DEFSG_CHANNEL_13_E,
   SYS_DEFSG_CHANNEL_14_E,
   SYS_DEFSG_CHANNEL_15_E,
   SYS_DEFSG_CHANNEL_16_E,
   SYS_DEFSG_CHANNEL_17_E,
   SYS_DEFSG_CHANNEL_18_E,
	SYS_DEFSG_CHANNEL_19_E,
	SYS_DEFSG_CHANNEL_20_E,
   SYS_DEFSG_CHANNEL_21_E,
   SYS_DEFSG_MAX_NUM_CHANNELS_E
} SYS_DEFSG_channelNumE;

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
//Static memory pool definitions
//Each channel has at most one memory pool

//Host-GP memory pools - configuration
#if DEFSG_GP_HOST
#define SYS_DEFSG_MAX_FRAME_SIZE                      (640*480*2) // HD frame/4 x VGA frame
#define SYS_DEFSG_MAX_DEPTH_SIZE                      (640*480*2) // TODO - max depth frame is smaller than HD
#define SYS_DEFSG_MAX_WEBCAM_SIZE                     (640*480*2)
#else
#define SYS_DEFSG_MAX_FRAME_SIZE                      (1280*960*2) // HD frame/4 x VGA frame
#define SYS_DEFSG_MAX_DEPTH_SIZE                      (1280*960*2) // TODO - max depth frame is smaller than HD
#define SYS_DEFSG_MAX_WEBCAM_SIZE                     (1280*964*2) //960 + 4 lines for embedded data from sensor
#endif

#define SYS_DEFSG_BASIC_DATA_FRAG_SIZE                (16*1024)

#define SYS_DEFSG_MEMPOOL_GP_HOST_GENERAL_NUM_BUFS    (128)
#define SYS_DEFSG_MEMPOOL_GP_HOST_GENERAL_SIZE_BYTES  (1100)

#define SYS_DEFSG_MEMPOOL_SVC_MNGR_DATA_NUM_BUFS      (5)
#define SYS_DEFSG_MEMPOOL_SVC_MNGR_DATA_SIZE_BYTES    (1024)

#define SYS_DEFSG_MEMPOOL_GP_HOST_SYSTEM_NUM_BUFS     (80)
#define SYS_DEFSG_MEMPOOL_GP_HOST_SYSTEM_SIZE_BYTES   (SYS_DEFSG_BASIC_DATA_FRAG_SIZE)

#define SYS_DEFSG_MEMPOOL_GP_DEPTH_VGA_NUM_BUFS       (INU_DEFSG_TRIPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_DEPTH_HD_NUM_BUFS        (INU_DEFSG_TRIPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_DEPTH_POOL_NUM_BUFS      (2*SYS_DEFSG_MEMPOOL_GP_DEPTH_VGA_NUM_BUFS)
#define SYS_DEFSG_MEMPOOL_GP_DEPTH_SIZE_BYTES         (16)
#define SYS_DEFSG_MEMPOOL_HOST_DEPTH_NUM_BUFS         (INU_DEFSG_TRIPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_HOST_DEPTH_SIZE_BYTES       ((INU_DEFSG_NUM_IMGS_NONE_INTERLEAVE_E * SYS_DEFSG_MAX_DEPTH_SIZE))

#define SYS_DEFSG_MEMPOOL_GP_VIDEO_VGA_NUM_BUFS       (INU_DEFSG_DOUBLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_VIDEO_HD_NUM_BUFS        (INU_DEFSG_DOUBLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_VIDEO_POOL_NUM_BUFS      (2*SYS_DEFSG_MEMPOOL_GP_VIDEO_VGA_NUM_BUFS)
#define SYS_DEFSG_MEMPOOL_GP_VIDEO_SIZE_BYTES         (16)
#define SYS_DEFSG_MEMPOOL_HOST_VIDEO_NUM_BUFS         (INU_DEFSG_TRIPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_HOST_VIDEO_SIZE_BYTES       ((INU_DEFSG_NUM_IMGS_INTERLEAVE_E * SYS_DEFSG_MAX_FRAME_SIZE))

#define SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_CTRL_NUM_BUFS      (8)
#define SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_CTRL_SIZE_BYTES    (32*1024)
#define SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_DATA_NUM_BUFS      (2)
#define SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_DATA_SIZE_BYTES    (640*480*2)

#define SYS_DEFSG_MEMPOOL_GP_HOST_ALG_CTRL_NUM_BUFS      (4)
#define SYS_DEFSG_MEMPOOL_GP_HOST_ALG_CTRL_SIZE_BYTES    (4096)
#define SYS_DEFSG_MEMPOOL_GP_HOST_ALG_DATA_NUM_BUFS      (4)
#define SYS_DEFSG_MEMPOOL_GP_HOST_ALG_DATA_SIZE_BYTES    (163712)

#define SYS_DEFSG_MEMPOOL_GP_INJECT_VGA_NUM_BUFS      (INU_DEFSG_QUAD_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_INJECT_HD_NUM_BUFS       (INU_DEFSG_QUAD_BUFFER)
#define SYS_DEFSG_MEMPOOL_HOST_INJECT_NUM_BUFS        (16)
#define SYS_DEFSG_MEMPOOL_GP_HOST_INJECT_SIZE_BYTES   ((INU_DEFSG_NUM_IMGS_INTERLEAVE_E * SYS_DEFSG_MAX_FRAME_SIZE))
#define SYS_DEFSG_MEMPOOL_HOST_INJECT_SIZE_BYTES      (16)

#define SYS_DEFSG_MEMPOOL_GP_HOST_LOGGER_NUM_BUFS     (1024)
#define SYS_DEFSG_MEMPOOL_GP_HOST_LOGGER_SIZE_BYTES   ((400) + 64)

#define SYS_DEFSG_MEMPOOL_GP_WEBCAM_VGA_NUM_BUFS      (INU_DEFSG_DOUBLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_WEBCAM_HD_NUM_BUFS       (INU_DEFSG_DOUBLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_WEBCAM_POOL_NUM_BUFS     (2*SYS_DEFSG_MEMPOOL_GP_WEBCAM_VGA_NUM_BUFS)
#define SYS_DEFSG_MEMPOOL_GP_WEBCAM_SIZE_BYTES        (16)
#define SYS_DEFSG_MEMPOOL_HOST_WEBCAM_NUM_BUFS        (INU_DEFSG_TRIPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_HOST_WEBCAM_SIZE_BYTES      ((INU_DEFSG_NUM_IMGS_INTERLEAVE_E * SYS_DEFSG_MAX_WEBCAM_SIZE))

#define SYS_DEFSG_MEMPOOL_GP_HOST_POS_SENSORS_NUM_BUFS          (128)
#define SYS_DEFSG_MEMPOOL_GP_HOST_POS_SENSORS_SIZE_BYTES        (1024)

#define SYS_DEFSG_MEMPOOL_AUDIO_TRANSFER_SIZE_BYTES   (512)
#define SYS_DEFSG_MEMPOOL_GP_AUDIO_NUM_BUFS           (INU_DEFSG_OCTUPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_GP_AUDIO_SIZE_BYTES         (16)
#define SYS_DEFSG_MEMPOOL_HOST_AUDIO_NUM_BUFS         (INU_DEFSG_TRIPLE_BUFFER)
#define SYS_DEFSG_MEMPOOL_HOST_AUDIO_SIZE_BYTES       (SYS_DEFSG_MEMPOOL_AUDIO_TRANSFER_SIZE_BYTES)

// TCP interface static set-up
#define SYS_DEFSG_CLS_TCP_DATA_PORT                   (50000)


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
#ifdef __cplusplus
}
#endif

#endif //SYS_DEFS_H
