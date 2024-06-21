/****************************************************************************
 *
 *   FileName: inu_common_cp.h
 *
 *   Author: Arnon C
 *   *
 *   Description: Common cp definitions
 *
 *****************************************************************************/
#ifndef INU_COMMON_CP_H
#define INU_COMMON_CP_H
#if DEFSG_IS_TARGET
typedef enum
{
   OS_LYRG_CP_MUTEX_ICC_BOX_GP_DSPA_E = 0,
   OS_LYRG_CP_MUTEX_ICC_BOX_GP_DSPB_E,
   OS_LYRG_CP_MUTEX_ICC_BOX_DSPA_GP_E,
   OS_LYRG_CP_MUTEX_ICC_BOX_DSPB_GP_E,
   OS_LYRG_CP_MUTEX_JOB_DESCRIPTORS_E,
   OS_LYRG_CP_MUTEX_FAST_E,
   OS_LYRG_CP_MUTEX_CDNN_E,
   OS_LYRG_CP_MUTEX_CDNN_EV_E,
   OS_LYRG_CP_MUTEX_VISION_PROC_E,
   OS_LYRG_CP_MUTEX_SLAM_E,
   OS_LYRG_CP_MUTEX_SLAM_EV_E,
   OS_LYRG_CP_MUTEX_DPE_PP_E,
   OS_LYRG_CP_MUTEX_PP_E,
   OS_LYRG_CP_MUTEX_GP_TO_XM4_E,
   OS_LYRG_CP_MUTEX_GP_TO_EV_E,
   OS_LYRG_CP_MUTEX_DPE_E,
   OS_LYRG_CP_MUTEX_HISTOGRAM_E,
   OS_LYRG_CP_MUTEX_CEVA_LOG_E,
   OS_LYRG_CP_MUTEX_EV61_LOG_E,
   OS_LYRG_CP_MUTEX_CLIENT_E,
   OS_LYRG_CP_MUTEX_ICC_ICTL_GP_E,
   OS_LYRG_CP_MUTEX_ICC_ICTL_DSP_A_E,
   OS_LYRG_CP_MUTEX_ICC_ICTL_DSP_B_E,
   OS_LYRG_CP_MUTEX_TSNR_E,
   OS_LYRG_NUM_CP_MUTEX_E
} OS_LYRG_cpMutexIdE;

typedef enum
{
   LOGG_CEVA_LOG_STATE_EMPTY = 0,
   LOGG_CEVA_LOG_STATE_FULL,
   LOGG_CEVA_LOG_MAX_NUM_STATE = 0xFFFFFFFF
} LOGG_cevaLogStateE;

typedef struct cpMutex
{
   unsigned int indx;
   unsigned int mutex;
} OS_LYRG_mutexCpT;

//	#define LOGG_CEVA_ID_A_STR          "CEVA A  "
#define LOGG_CEVA_ID_STR          "CEVA"
//	#define LOGG_CEVA_ID_B_STR          "CEVA B  "
#define LOGG_EV72_0_ID_STR          "EV_0"
#define LOGG_EV72_1_ID_STR          "EV_1"

//	#define LOGG_CEVA_ID_DEADER_SIZE    sizeof(LOGG_CEVA_ID_STR)
#define LOGG_DSP_ID_HEADER_SIZE    sizeof(LOGG_EV72_0_ID_STR)

#define LOGP_CEVA_PRINTF_SIZE       (100)
#define LOGP_CEVA_BUF_LOG_SIZE      ((LOGP_CEVA_PRINTF_SIZE + LOGG_DSP_ID_HEADER_SIZE) + (4 - ((LOGP_CEVA_PRINTF_SIZE + LOGG_DSP_ID_HEADER_SIZE) % 4)))
#define LOGP_NUM_OF_CEVA_LOG_BUFS   (128)


typedef struct __attribute__((packed, aligned(4)))
{
   OS_LYRG_mutexCpT     *cpMutex;
   UINT32               rdBufferIndex;
   UINT32               wrBufferIndex;
   LOGG_cevaLogStateE   state;
   char                 logBuffer[LOGP_NUM_OF_CEVA_LOG_BUFS][LOGP_CEVA_BUF_LOG_SIZE];
   char                 safetyBuffer[100]; //incase print of last index is larger then allowed
} LOGG_cevaLogbufferT;

#endif

#endif

