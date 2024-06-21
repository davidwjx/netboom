/****************************************************************************
 *
 *   FileName: os_lyr.h
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: OS abstraction layer API
 *
 ****************************************************************************/
#ifndef _OS_LYR_H_
#define _OS_LYR_H_

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include "inu_types.h"
#include "err_defs.h"

#define OS_LYRG_MAX_MQUE_NAME_LEN       (64)
#define OS_LYRG_WHILE while(1)
#define OS_LYRG_BREAK break
typedef void* OS_LYRG_event;
typedef void (*OS_LYRG_timerCallbackFunctionT)(void *argP);
typedef enum {
   OS_LYRG_EVENT_MSG_RCV_E = 0,
   OS_LYRG_EVENT_USER_E,
   OS_LYRG_MULTUPLE_EVENTS_NUM_E
} OS_LYRG_multipleEventsE;
#if DEFSG_IS_GP
#include "os_lyr_gp.h"
#elif DEFSG_IS_OS_LINUX_GLIBC
#include "os_lyr_linux.h"
#elif DEFSG_IS_OS_LINUX_BIONIC
#include "os_lyr_android.h"
#elif DEFSG_IS_OS_WINDOWS
#include "os_lyr_win.h"
#elif DEFSG_IS_OS_CEVA
#include "os_lyr_ceva.h"
#elif DEFSG_IS_OS_EV72
#include "os_lyr_ev72.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define NSEC_PER_SEC (1000000000UL)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   OS_LYRG_DEFAULT_THREAD_ID_E = 0,
   OS_LYRG_SERVICE_LOGGER_THREAD_ID_E,
   OS_LYRG_CLS_TX_THREAD_ID_E ,
   OS_LYRG_CLS_RX_THREAD_ID_E ,
   OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E,
   OS_LYRG_INT_CTRL_THREAD_ID_E,
   OS_LYRG_ICC_CMD_THREAD_ID_E,
   OS_LYRG_ICC_MSG_THREAD_ID_E,
   OS_LYRG_POS_LISTENER_THREAD_ID_E,
   OS_LYRG_TIME_LISTENER_THREAD_ID_E,
   OS_LYRG_TIME_INJECT_THREAD_ID_E,
   OS_LYRG_WD_THREAD_ID_E,
   OS_LYRG_GRAPH_RUNTIME_THREAD_ID_E,
   OS_LYRG_WORK_1_THREAD_ID_E,
   OS_LYRG_WORK_2_THREAD_ID_E,
   OS_LYRG_WORK_3_THREAD_ID_E,
   OS_LYRG_COMM_THREAD_ID_E,
   OS_LYRG_REUSE_MEMORY_ID_E,
   OS_LYRG_TRIGGER_MNGR_THREAD_ID_E,
   OS_LYRG_POWER_STATS_THREAD_ID_E,
   OS_LYRG_CHARGER_THREAD_ID_E,
   OS_LYRG_PWR_MNGR_THREAD_ID_E,
   OS_LYRG_CDC_MNGR_THREAD_ID_E,
   OS_LYRG_UVC1_THREAD_ID_E,
   OS_LYRG_UVC_OPR_THREAD_ID_E,
   OS_LYR_SENSOR_UPDATER_THREAD_ID_E,
   OS_LYR_SENSOR_SYNC_THREAD_ID_E,
   OS_LYR_HELSINKI_ISP_SYNC_THREAD_ID_E,
   OS_LYR_NUSERIAL_BG_TASK_ID_E,
   OS_LYR_NUSERIAL_RECV_TASK_ID_E,
#if SCNN_SIM
   OS_LYRG_SCNN_THREAD_ID_E,
   OS_LYRG_SCNN_FUNCTION_THREAD_ID_E,
#endif
   OS_LYRG_ISP_MONITOR_THREAD_ID_E,
   OS_LYRG_FW_UPDATE_THREAD_ID_E,

   OS_LYRG_NUM_THREAD_IDS_E
} OS_LYRG_threadIdE;

typedef void* OS_LYRG_threadHandle;
typedef int (*OS_LYRG_threadFunction)(void *);
typedef struct OS_LYRG_thread_Params
{
   OS_LYRG_threadHandle    threadHandle;
   OS_LYRG_threadFunction  func;
   OS_LYRG_event           event;
   void*                   param;
   OS_LYRG_threadIdE       id;
} OS_LYRG_threadParams, *OS_LYRG_threadParamsP;

typedef enum
{
   OS_LYRG_INT_CDE_1_E = 0,
   OS_LYRG_INT_CDE_2_E,
   OS_LYRG_INT_CDE_3_E,
   OS_LYRG_INT_CDE_4_E,
   OS_LYRG_INT_CDE_5_E,
   OS_LYRG_INT_CDE_6_E,
   OS_LYRG_INT_CDE_ABORT_0_E,
   OS_LYRG_INT_CDE_ABORT_1_E,
   OS_LYRG_INT_CDE_ABORT_2_E,
   OS_LYRG_INT_IAE_E,
   OS_LYRG_INT_DPE_E,
   OS_LYRG_INT_PPE_E,
   OS_LYRG_INT_DSPA_E,
   OS_LYRG_INT_DSPB_E,
   OS_LYRG_INT_STROBE1_E,
   OS_LYRG_INT_STROBE2_E,
   OS_LYRG_INT_STROBE3_E,
   OS_LYRG_INT_STROBE4_E,
   OS_LYRG_INT_STROBE5_E,
   OS_LYRG_INT_STROBE6_E,
   OS_LYRG_INT_GPIO_SLAVE_E,  //  gpio
   OS_LYRG_INT_PR_SHUTTER_E,

   OS_LYRG_NUM_INTS
} OS_LYRG_intNumE;

typedef void (*OS_LYRG_intHandlerT)(UINT64 timeStamp, UINT64 count, void *argP);

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE OS_LYRG_init();


//Thread handling API
OS_LYRG_threadHandle OS_LYRG_createThread(OS_LYRG_threadParamsP params);
ERRG_codeE  OS_LYRG_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec);
int  OS_LYRG_closeThread(OS_LYRG_threadHandle thrd);
ERRG_codeE OS_LYRG_exitThread();

//Mutex API
int OS_LYRG_aquireMutex(OS_LYRG_mutexT *mutexP);
int OS_LYRG_lockMutex(OS_LYRG_mutexT *mutexP);
int OS_LYRG_unlockMutex(OS_LYRG_mutexT *mutexP);
int OS_LYRG_releaseMutex(OS_LYRG_mutexT *mutexP);

//Event API
OS_LYRG_event OS_LYRG_createEvent(UINT32 id);
int OS_LYRG_releaseEvent(OS_LYRG_event event,UINT32 id);
int OS_LYRG_setEvent(OS_LYRG_event event);
int OS_LYRG_waitEvent(OS_LYRG_event event,UINT32 timeout);

//Sleep API
void OS_LYRG_usleep(int timeuSec);

//Time API
void OS_LYRG_getTime(UINT32 *secP, UINT16 *msecP);
UINT32 OS_LYRG_deltaMsec(UINT32 prevSec, UINT16 prevMsec);
void OS_LYRG_getMsecTime(UINT32 *msecP);
void OS_LYRG_getUsecTime(UINT64 *usecP);
void OS_LYRG_getTimeNsec(UINT64 *nsecP);
ERRG_codeE OS_LYRG_setTime(INT32 sec, INT32 usec);

//Message Queue API
int OS_LYRG_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents);
int OS_LYRG_deleteMsgQue(OS_LYRG_msgQueT *msgQueP);
int OS_LYRG_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
int OS_LYRG_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeoutMsec);
unsigned int OS_LYRG_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP);
ERRG_codeE OS_LYRG_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId);
int OS_LYRG_createMsgQueNonBlock(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents);
#if DEFSG_IS_TARGET
//Cross - Platform Lockers API
int OS_LYRG_mutexInitCp();
int OS_LYRG_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum);
int OS_LYRG_releaseMutexCp(OS_LYRG_mutexCpT *cpMutexP);
int OS_LYRG_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
int OS_LYRG_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
#endif

//Timer Controller API
ERRG_codeE OS_LYRG_createTimer( OS_LYRG_timerT *timerP );
ERRG_codeE OS_LYRG_setTimer( OS_LYRG_timerT *timerP );
ERRG_codeE OS_LYRG_deleteTimer( OS_LYRG_timerT *timerP );

//File access API
ERRG_codeE OS_LYRG_createTmpFile( FILE **file );

//Virtual address region/section module API
#if DEFSG_IS_GP
int OS_LYRG_waitRecvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP);
ERRG_codeE OS_LYRG_openMemFd(INT32 *memFdP);
ERRG_codeE OS_LYRG_openBuffMemFd(INT32 *memFdP);

ERRG_codeE OS_LYRG_closeMemFd(INT32 memFd);
ERRG_codeE OS_LYRG_mapRegion(INT32 memFd, UINT32 *phyAddrP, UINT32 regionSize, BYTE isCached, UINT32 *virtAddrP, INT32 regionId, INT32 *dmaBufFD);
ERRG_codeE OS_LYRG_unmapRegion(void *baseAddr, UINT32 regionSize, INT32 regionId);

//Interrupt Controller API
ERRG_codeE OS_LYRG_intCtrlInit(void);
ERRG_codeE OS_LYRG_intCtrlRegister(OS_LYRG_intNumE intNum, OS_LYRG_intHandlerT isr, void *argp);
ERRG_codeE OS_LYRG_intCtrlUnregister(OS_LYRG_intNumE intNum);
ERRG_codeE OS_LYRG_intCtrlEnable(OS_LYRG_intNumE intNum);
ERRG_codeE OS_LYRG_intCtrlDisable(OS_LYRG_intNumE intNum);
ERRG_codeE OS_LYRG_intCtrlIoctl(OS_LYRG_intNumE intNum, int gpio);
void OS_LYRG_intCtrlStats(void);
#endif


#ifdef __cplusplus
}
#endif

#endif //_OS_LYR_H_


