/****************************************************************************
 *
 *   FileName: svc_cfg.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: Service Configuration API 
 *   
 ****************************************************************************/
#ifndef SVCFG_H
#define SVCFG_H

#include "err_defs.h"
#include "os_lyr.h"
#include "mem_pool.h"
#include "sys_defs.h"
#include "inu_cmd.h"
#include "conn_lyr.h"
/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define SERVICEG_TXID_IGNORE   (SVC_COMG_TXID_IGNORE)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   SERVICEG_CTRL_CHAN_E = 0,
   SERVICEG_DATA_CHAN_E,
   SERVICEG_NUM_CHANNELS_E
} SERVICEG_channelE;

typedef enum
{
   SERVICEG_IOCTL_SYNC_START_E   = 0,
   SERVICEG_IOCTL_SYNC_DONE_E    = 1
} SERVICEG_ioctlSyncE;

typedef enum
{
   SERVICEG_STATE_INIT_E = 1,
   SERVICEG_STATE_OPENED_E,
   SERVICEG_STATE_RUNNING_E,
   SERVICEG_STATE_PAUSED_E,
   SERVICEG_NUM_STATES_E
} SERVICEG_stateE;

typedef void * SERVICEG_handleT;

typedef struct
{
   SERVICEG_handleT     handle;
   SERVICEG_ioctlSyncE  sync;
   INT32                ioctlNum;
   void                 *argP;
} SERVICEG_svcMngCbParams;



/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
*****************************************************************************/
typedef enum
{
   SVCFG_MEMPOOL_MNGR_CTRL_E = 0,
   SVCFG_MEMPOOL_MNGR_DATA_E,
   SVCFG_MEMPOOL_SYSTEM_DATA_E,
   SVCFG_MEMPOOL_INJECT_DATA_E,
   SVCFG_MEMPOOL_VIDEO_DATA_E,
   SVCFG_MEMPOOL_DEPTH_DATA_E,
   SVCFG_MEMPOOL_WEBCAM_DATA_E,
   SVCFG_MEMPOOL_CLIENT_CTRL_E,
   SVCFG_MEMPOOL_CLIENT_DATA_E,
   SVCFG_MEMPOOL_ALG_CTRL_E,
   SVCFG_MEMPOOL_ALG_DATA_E,
   SVCFG_MEMPOOL_LOGGER_DATA_E,
   SVCFG_MEMPOOL_USBIF_FIXED_E,
   SVCFG_MEMPOOL_NUM_POOLS_E
} SVCFG_memIdE;

typedef enum
{
   SVCFG_CONN_USB_IF0_E = 0,
   SVCFG_CONN_USB_IF1_E ,
   SVCFG_CONN_UART_E,
   SVCFG_NUM_CONNECTIONS_E
} SVCFG_connIdE;

typedef enum
{
   SVCFG_CHAN_BASE_MNGR_E         = 0,
   SVCFG_CHAN_BASE_SYSTEM_E       = 2,
   SVCFG_CHAN_BASE_INJECT_E       = 4,
   SVCFG_CHAN_BASE_VIDEO_E        = 6,
   SVCFG_CHAN_BASE_DEPTH_E        = 8,
   SVCFG_CHAN_BASE_WEBCAM_E       = 10,
   SVCFG_CHAN_BASE_ALG_E          = 12,
   SVCFG_CHAN_BASE_LOGGER_E       = 14,
   SVCFG_CHAN_BASE_CLIENT_E       = 16,
   SVCFG_CHAN_BASE_POS_SENSORS_E  = 18,
   SVCFG_CHAN_BASE_AUDIO_E        = 20,
   SVCFG_CHAN_LAST, 
   SVCFG_NUM_CHANNELS_E,
} SVCFG_channelIdE;

#define SVCFG_CTRL_CH(base) (base)
#define SVCFG_DATA_CH(base) ((SVCFG_channelIdE)(base+1))
#define SVCFG_CH_BASE(ch) (ch &= (~0x1))

typedef enum
{
   SVCFG_SVC_MNGR_E           = 0,
   SVCFG_SVC_SYSTEM_E         = 1,
   SVCFG_SVC_LOGGER_E         = 2,
   SVCFG_SVC_VIDEO_STREAM_E   = 3,
   SVCFG_SVC_DEPTH_STREAM_E   = 4,
   SVCFG_SVC_WEBCAM_STREAM_E  = 5,
   SVCFG_SVC_INJECT_STREAM_E  = 6,
   SVCFG_SVC_ALG_STREAM_E     = 7,
   SVCFG_SVC_CLIENT_STREAM_E  = 8,
   SVCFG_SVC_POS_SENSORS_E    = 9,
   SVCFG_SVC_AUDIO_E          = 10,
   SVCFG_NUM_SERVICES_E
} SVCFG_svcIdE;

typedef struct
{
   SVCFG_memIdE      id;
   int               enabled;
   UINT32            chanMap;
   MEM_POOLG_typeE   type;
   UINT32            bufferSize;
   UINT32            numBuffers;
   int               rstptr;
}SVCFG_memConfigT;

typedef struct
{
   SVCFG_connIdE       id;
   int                 enabled;
   UINT32              chanMap;
   CONN_LYRG_openCfgT  connConfig;
}SVCFG_conConfigT;


typedef struct
{
   SVCFG_channelIdE id;
   int              enabled;
   SVCFG_svcIdE     svcId;
   SVCFG_memIdE     memId;
   SVCFG_connIdE    connId;
} SVCFG_chanConfigT;

/*
typedef struct
{
   SVCFG_svcIdE            id;
   int                     enabled;
   INUG_serviceIdE         svcId;
   const SERVICEG_methodsT *methodsP;
   SVCFG_channelIdE        chanIdBase;
   OS_LYRG_threadIdE       threadId;
   UINT32                  inQueSize;
   int                     threadWakeup;
   BOOL                    linkMonitor;
   SERVICEG_channelE       linkChan;
}SVCFG_svcConfigT; 

static inline INUG_serviceIdE id2Sid(SVCFG_svcIdE id)
{
   return (INUG_serviceIdE)id; 
}
static inline SVCFG_svcIdE  sid2Id(INUG_serviceIdE sid)
{
   return (SVCFG_svcIdE)sid;
}
*/	



#ifdef __cplusplus
}
#endif

#endif //SVCFG_H
