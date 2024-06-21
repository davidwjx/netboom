/****************************************************************************
 *
 *   FileName: service.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: General service API 
 *   
 ****************************************************************************/
#ifndef SERVICE_H
#define SERVICE_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "err_defs.h"
#include "os_lyr.h"
#include "mem_pool.h"
#include "sys_defs.h"
#include "svc_com.h"
#include "inu_cmd.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
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

typedef ERRG_codeE (*SERVICEG_mngCbT)(SERVICEG_svcMngCbParams *svcMngCbParamsP);
typedef void (*SERVICEG_preIoctlCbT)(SERVICEG_handleT handle, INT32 *ioctlNumP,  void **argP);
typedef void (*SERVICEG_postIoctlCbT)(SERVICEG_handleT handle, INT32 *ioctlNumP, void **argP);
typedef ERRG_codeE (*SERVICEG_msgCbT)(UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, MEM_POOLG_bufDescT **bufDescP);
typedef ERRG_codeE (*SERVICEG_periodicCbT)();
typedef int (*SERVICEG_userMsgCbT)(UINT8 *dataP, UINT32 dataLen, SERVICEG_handleT userMsgParams, SERVICEG_handleT userCbParams);
typedef ERRG_codeE (*SERVICEG_registerMsgCbT)(SERVICEG_userMsgCbT userMsgCb);
typedef SVC_COMG_sendCbT SERVICEG_sendMsgCbT;

typedef struct
{
   //hooks/callbacks for service-specific functions
   char *(*getName)(void);
// manage call back
   SERVICEG_mngCbT   open;
   SERVICEG_mngCbT   close;
   SERVICEG_mngCbT   ioctl;
// message call back
   SERVICEG_msgCbT   handleMsg;
// periodic call back
   SERVICEG_periodicCbT    periodicTask;
// chann hooks
   SERVICEG_msgCbT chanHook[SERVICEG_NUM_CHANNELS_E];
// ioctl pre/post call back
   SERVICEG_preIoctlCbT preIoctl;
   SERVICEG_postIoctlCbT postIoctl;
} SERVICEG_methodsT;

typedef struct
{
   SVC_COMG_handleT chanTbl[SERVICEG_NUM_CHANNELS_E];
   MEM_POOLG_handleT localPoolH;
} SERVICEG_svcRescT;

typedef struct
{
   INT32                            serviceId;
   OS_LYRG_threadIdE                threadId;
   UINT32                           inQueSize;
   SERVICEG_svcRescT                *rescP;
   const SERVICEG_methodsT          *methodsP;
   int                              threadWakeup;
   BOOL                             linkMonitor;
   INUG_hostTargetConnectionE       hostTargetConnection;
   SERVICEG_channelE                linkChan;
   int (*linkEventCallBack)(int, int, void*);
   void                             *linkEventCallBackArgP;
} SERVICEG_initParamsT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
// general functions
const char *      SERVICEG_getName(SERVICEG_handleT handle);
SERVICEG_handleT  SERVICEG_getHandle(INT32 serviceId);
INT32             SERVICEG_getId(SERVICEG_handleT handle);
void*             SERVICEG_getDataMemPool(SERVICEG_handleT handle);
void*             SERVICEG_getCtrlMemPool(SERVICEG_handleT handle);
void*             SERVICEG_getMinPoolH(SERVICEG_handleT handle, UINT32 size);
void              SERVICEG_showStats(SERVICEG_handleT handle);
BOOL              SERVICEG_isServiceOpen(SERVICEG_handleT handle);

//Initialize/Release the service (locally) called once on startup
ERRG_codeE SERVICEG_init(SERVICEG_handleT *handleP, SERVICEG_initParamsT *paramsP);
void SERVICEG_deInit(SERVICEG_handleT handle);
// Open and close the service (locally)
ERRG_codeE SERVICEG_open(SERVICEG_handleT handle, SERVICEG_ioctlSyncE ioctlDir);
ERRG_codeE SERVICEG_close(SERVICEG_handleT handle, SERVICEG_ioctlSyncE ioctlDir);
// IOCTLs - local and forward request (synchronous)
ERRG_codeE SERVICEG_ioctl(SERVICEG_handleT handle, INT32 ioctlNum, void *argP);
ERRG_codeE SERVICEG_killOutstandingIoclts(SERVICEG_handleT  handle);
ERRG_codeE SERVICEG_regiserMsgCb(SERVICEG_handleT  handle, SERVICEG_userMsgCbT cb, SERVICEG_handleT cbContext);
ERRG_codeE SERVICEG_getMsgCb(SERVICEG_handleT  handle, SERVICEG_userMsgCbT *cb, SERVICEG_handleT *cbContext);

ERRG_codeE SERVICEG_send(SERVICEG_handleT handle, int local, SERVICEG_channelE chan, UINT32 msgCode, void *msg, MEM_POOLG_bufDescT *mbuf, UINT32 txid);
ERRG_codeE SERVICEG_copyAndSend(SERVICEG_handleT handle, int local, SERVICEG_channelE chan,UINT32 msgCode, void *msg, UINT8 *buf, UINT32 bufLen);

#define SERVICEG_sendDataCh(handle,msgCode,msg, mbuf, txid)  SERVICEG_send(handle,0,SERVICEG_DATA_CHAN_E,msgCode,msg,mbuf,txid)
#define SERVICEG_sendCtrlCh(handle,msgCode,msg, mbuf, txid)  SERVICEG_send(handle,0,SERVICEG_CTRL_CHAN_E,msgCode,msg,mbuf,txid)
#define SERVICEG_copyAndSendDataCh(handle,msgCode, msg, buf,buflen)  SERVICEG_copyAndSend(handle,0,SERVICEG_DATA_CHAN_E,msgCode,msg,buf,buflen)
#define SERVICEG_copyAndSendCtrlCh(handle,msgCode, msg, buf,buflen) SERVICEG_copyAndSend(handle,0,SERVICEG_CTRL_CHAN_E,msgCode,msg,buf,buflen)

#define SERVICEG_sendDataChLocal(handle,msgCode, msg, mbuf)  SERVICEG_send(handle,1,SERVICEG_DATA_CHAN_E,msgCode,msg,mbuf,SVC_COMG_TXID_IGNORE)
#define SERVICEG_sendCtrlChLocal(handle,msgCode, msg, mbuf)  SERVICEG_send(handle,1,SERVICEG_CTRL_CHAN_E,msgCode,msg,mbuf,SVC_COMG_TXID_IGNORE)
#define SERVICEG_copyAndSendDataChLocal(handle,msgCode, msg, buf,buflen)  SERVICEG_copyAndSend(handle,1,SERVICEG_DATA_CHAN_E,msgCode,msg,buf,buflen)
#define SERVICEG_copyAndSendCtrlChLocal(handle,msgCode, msg, buf,buflen) SERVICEG_copyAndSend(handle,1,SERVICEG_CTRL_CHAN_E,msgCode,msg,buf,buflen)


void SERVICEG_setSendMsgCb(SERVICEG_handleT handle, SERVICEG_channelE chan, SERVICEG_sendMsgCbT cb);

#ifdef __cplusplus
}
#endif 

#endif //SERVICE_H
