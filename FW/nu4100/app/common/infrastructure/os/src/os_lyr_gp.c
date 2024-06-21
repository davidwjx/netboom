/****************************************************************************
 *
 *   FileName: os_lyr.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description:  OS Abstraction Layer API
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "os_lyr.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "cmem.h"
#include "mem_map.h"

#ifdef __cplusplus
extern "C" {
#endif


#if DEFSG_IS_GP

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
/* inline asm (no optimization) */
#define Asm __asm__ volatile

#define TIMERSIGNALID SIGUSR1
#define OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES   (128*1024)//temp:increase memory for all threads, later increase for isp only
#define OS_LYRP_DEFAULT_PTHREAD_GUARD_SIZE_BYTES   (1024)
#define OS_LYRP_DEFAULT_PTHREAD_PRIORITY(policy)   ((sched_get_priority_max(policy) -sched_get_priority_min(policy))/2)
#define OS_LYRP_DEFAULT_PTHREAD_SCHED_POLICY       (SCHED_RR) //SCHED_OTHER
#define OS_LYRP_DEFAULT_PTHREAD_DETACHED_STATE     (PTHREAD_CREATE_JOINABLE)
#define OS_LYRP_DEFAULT_PTHREAD_INHERITED_SCHED    (PTHREAD_EXPLICIT_SCHED)
#define OS_LYRP_DEFAULT_PTHREAD_SCOPE              (PTHREAD_SCOPE_SYSTEM)
#ifdef _GNU_SOURCE
#define OS_LYRP_PTHREAD_MUTEX_PROTOCOL             (PTHREAD_PRIO_INHERIT)
#endif

//Thread priorities for linux under GP.
//NOTE: Linux version 3.13 creates kernel irq thread handlers with a hard coded priority 50.
//We set all the user-space priorities here to be below this value.
#define OS_LYRP_PTHREAD_PRIORITY_WD                   (1)
#define OS_LYRP_PTHREAD_PRIORITY_FW_UPDATE            (2)
#define OS_LYRP_PTHREAD_PRIORITY_SVC_LOGGER           (5)
#define OS_LYRP_PTHREAD_PRIORITY_POWER_MONITOR        (5)
#define OS_LYRP_PTHREAD_PRIORITY_CHARGER              (5)
#define OS_LYRP_PTHREAD_PRIORITY_TCP_SVR              (10)
#define OS_LYRP_PTHREAD_PRIORITY_TRIGGER              (10)
#define OS_LYRP_PTHREAD_PRIORITY_SENSORS_CTRL_ALG     (15)
#define OS_LYRP_PTHREAD_PRIORITY_POS_SENSOR_LISTENER  (21)
#define OS_LYRP_PTHREAD_PRIORITY_ICC_CMD              (44)
#define OS_LYRP_PTHREAD_PRIORITY_ICC_MSG              (44)
#define OS_LYRP_PTHREAD_PRIORITY_TIMER                (50)
#define OS_LYRP_PTHREAD_PRIORITY_TIME_LISTENER        (51)
#define OS_LYRP_PTHREAD_PRIORITY_WD                   (1)
#define OS_LYRP_PTHREAD_PRIORITY_ATL_SENS             (45)
#define OS_LYRP_PTHREAD_PRIORITY_WORK_1               (20)
#define OS_LYRP_PTHREAD_PRIORITY_POS_SENSOR_LISTENER  (21)
#define OS_LYRP_PTHREAD_PRIORITY_WORK_2               (22)
#define OS_LYRP_PTHREAD_PRIORITY_WORK_3               (45)
#define OS_LYRP_PTHREAD_PRIORITY_SVC_MNGR             (25)
#define OS_LYRP_PTHREAD_PRIORITY_COMM                 (45)
#define OS_LYRP_PTHREAD_PRIORITY_MEM_REUSE            (29)
#define OS_LYRP_PTHREAD_PRIORITY_CLS_TX               (30)
#define OS_LYRP_PTHREAD_PRIORITY_CLS_RX               (30)
#define OS_LYRP_PTHREAD_PRIORITY_GRAPH_RUNTIME        (31)
#define OS_LYRP_PTHREAD_PRIORITY_ISP                  (45)
#define OS_LYRP_PTHREAD_PRIORITY_PWR_MNGR             (40)
#define OS_LYRP_PTHREAD_PRIORITY_ICC_CMD              (44)
#define OS_LYRP_PTHREAD_PRIORITY_ICC_MSG              (44)
#define OS_LYRP_PTHREAD_PRIORITY_ATL_SENS             (45)
#define OS_LYRP_PTHREAD_PRIORITY_INT_CTRL             (50)
#define OS_LYRP_PTHREAD_PRIORITY_TIMER                (50)
#define OS_LYRP_PTHREAD_PRIORITY_UVC_SHTR             (50)
#define OS_LYRP_PTHREAD_PRIORITY_TIME_LISTENER        (51)
#define OS_LYRP_PTHREAD_PRIORITY_SENSOR_UPDATER       (45)
#define OS_LYRP_PTHREAD_PRIORITY_SENSOR_SYNC          (10)
#define OS_LYRP_PTHREAD_PRIORITY_HELSINKI_ISP_SYNC    (10)
#define OS_LYRP_PTHREAD_PRIORITY_BG_TASK              (45)
#define OS_LYRP_PTHREAD_PRIORITY_RECV_TASK            (10)

#define OS_LYRP_TOTAL_LOCKERS      (20)
#define OS_LYRP_MEMLOCK_ADDR_LOW   (0x00000000)
#define OS_LYRP_MEMLOCK_ADDR_HIGH  (0xFFFFFFFF)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
#define OS_LYRP_DEFAULT_PTHREAD_DETACHED_STATE     (PTHREAD_CREATE_JOINABLE)
#define OS_LYRP_DEFAULT_PTHREAD_GUARD_SIZE_BYTES   (1024)
#define OS_LYRP_DEFAULT_PTHREAD_INHERITED_SCHED    (PTHREAD_EXPLICIT_SCHED)
#define OS_LYRP_DEFAULT_PTHREAD_SCOPE              (PTHREAD_SCOPE_SYSTEM)


typedef void *(*PTHREAD_ROUTINE) (void *);

typedef enum
{
   OS_LYRP_CPMUTEX_FREE_E     = 0,
   OS_LYRP_CPMUTEX_OCCUPY_E   = 1,
   OS_LYRP_CPMUTEX_LAST_E
} OS_LYRP_cpMutexStatusE;

#define OS_LYRP_VIRT_MEM_MASK      (0xFFF)
#define OS_LYRP_VIRT_MEM_DUMMY_FD  (-1)

typedef struct
{
   int schedulePolicy;
   int priority;
   int stackSize;
   const char *name;
   PTHREAD_ROUTINE func;
   void *arg;
   pid_t tid;
} OS_LYRP_pthreadParamsT;

typedef struct
{
   int priority;
   const char *name;
   PTHREAD_ROUTINE func;
   void *arg;
   pid_t tid;
} OS_LYRP_pthreadArgsT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

static OS_LYRP_pthreadParamsT OS_LYRP_pthreadParamsTbl[OS_LYRG_NUM_THREAD_IDS_E];




/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static int OS_LYRP_init(void);
static OS_LYRG_threadHandle OS_LYRP_createThread(OS_LYRG_threadParamsP params);
static ERRG_codeE OS_LYRP_waitForThread(OS_LYRG_threadHandle thrd);
static ERRG_codeE OS_LYRP_exitThread();
static int OS_LYRP_closeThread(OS_LYRG_threadHandle thrd);
static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_mutexInitCp();
static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum);
static int OS_LYRP_releaseMutexCp(OS_LYRG_mutexCpT *cpMutexP);
static int OS_LYRP_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
static int OS_LYRP_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
static OS_LYRG_event OS_LYRP_createEvent(UINT32 id);
static int OS_LYRP_waitEvent(OS_LYRG_event event, UINT32 timeoutMsec);
static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event event);
static int OS_LYRP_releaseEvent(OS_LYRG_event event, UINT32 id);
static void OS_LYRP_sleep(int timeuSec);
static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP);
static int OS_LYRP_setTime(INT32 sec, INT32 usec);
static void OS_LYRP_getNsTime(UINT32 *secP, UINT32 *nsecP);
static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeouMsec);
static void OS_LYRP_initPthreadTbl(void);

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/


/*******************************************__linux__**********************************/

static void OS_LYRP_initPthreadTbl(void)
{
   unsigned int i;
   OS_LYRP_pthreadParamsT *tblP = &OS_LYRP_pthreadParamsTbl [0];

   tblP[OS_LYRG_DEFAULT_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_DEFAULT_THREAD_ID_E].schedulePolicy= OS_LYRP_DEFAULT_PTHREAD_SCHED_POLICY;
   tblP[OS_LYRG_DEFAULT_THREAD_ID_E].priority = OS_LYRP_DEFAULT_PTHREAD_PRIORITY(OS_LYRP_DEFAULT_PTHREAD_SCHED_POLICY);
   tblP[OS_LYRG_DEFAULT_THREAD_ID_E].name = "default";

   tblP[OS_LYRG_SERVICE_LOGGER_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_SERVICE_LOGGER_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_SERVICE_LOGGER_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_SVC_LOGGER;
   tblP[OS_LYRG_SERVICE_LOGGER_THREAD_ID_E].name = "svc_logger";

   tblP[OS_LYRG_CLS_TX_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_CLS_TX_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_CLS_TX_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_CLS_TX;
   tblP[OS_LYRG_CLS_TX_THREAD_ID_E].name = "cls_tx";

   tblP[OS_LYRG_CLS_RX_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_CLS_RX_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_CLS_RX_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_CLS_RX;
   tblP[OS_LYRG_CLS_RX_THREAD_ID_E].name = "cls_rx";

   tblP[OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_TCP_SVR;
   tblP[OS_LYRG_CLS_TCP_SERVER_THREAD_ID_E].name = "tcp_svr";

   tblP[OS_LYRG_INT_CTRL_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_INT_CTRL_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_INT_CTRL_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_INT_CTRL;
   tblP[OS_LYRG_INT_CTRL_THREAD_ID_E].name = "int_ctrl";

   tblP[OS_LYRG_POS_LISTENER_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_POS_LISTENER_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_POS_LISTENER_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_POS_SENSOR_LISTENER;
   tblP[OS_LYRG_POS_LISTENER_THREAD_ID_E].name = "pos_listener";

   tblP[OS_LYRG_ICC_CMD_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_ICC_CMD_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_ICC_CMD_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_ICC_CMD;
   tblP[OS_LYRG_ICC_CMD_THREAD_ID_E].name = "icc_cmd";

   tblP[OS_LYRG_ICC_MSG_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_ICC_MSG_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_ICC_MSG_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_ICC_MSG;
   tblP[OS_LYRG_ICC_MSG_THREAD_ID_E].name = "icc_msg";

   tblP[OS_LYRG_REUSE_MEMORY_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_REUSE_MEMORY_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_REUSE_MEMORY_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_MEM_REUSE;
   tblP[OS_LYRG_REUSE_MEMORY_ID_E].name = "mem_reuse_msg";

   tblP[OS_LYRG_TIME_LISTENER_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_TIME_LISTENER_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_TIME_LISTENER_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_TIME_LISTENER;
   tblP[OS_LYRG_TIME_LISTENER_THREAD_ID_E].name = "spi_listener";

   tblP[OS_LYRG_WD_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_WD_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_WD_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_WD;
   tblP[OS_LYRG_WD_THREAD_ID_E].name = "watchdog";

   tblP[OS_LYRG_GRAPH_RUNTIME_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_GRAPH_RUNTIME_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_GRAPH_RUNTIME_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_GRAPH_RUNTIME;
   tblP[OS_LYRG_GRAPH_RUNTIME_THREAD_ID_E].name = "graph_runtime";

   tblP[OS_LYRG_WORK_1_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_WORK_1_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_WORK_1_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_WORK_1;
   tblP[OS_LYRG_WORK_1_THREAD_ID_E].name = "work_1";

   tblP[OS_LYRG_WORK_2_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_WORK_2_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_WORK_2_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_WORK_2;
   tblP[OS_LYRG_WORK_2_THREAD_ID_E].name = "work_2";

   tblP[OS_LYRG_WORK_3_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_WORK_3_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_WORK_3_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_WORK_3;
   tblP[OS_LYRG_WORK_3_THREAD_ID_E].name = "work_3";

   tblP[OS_LYRG_COMM_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_COMM_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_COMM_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_COMM;
   tblP[OS_LYRG_COMM_THREAD_ID_E].name = "inu_comm";

   tblP[OS_LYRG_TRIGGER_MNGR_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_TRIGGER_MNGR_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_TRIGGER_MNGR_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_TRIGGER;
   tblP[OS_LYRG_TRIGGER_MNGR_THREAD_ID_E].name = "trigger";

   tblP[OS_LYRG_POWER_STATS_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_POWER_STATS_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_POWER_STATS_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_POWER_MONITOR;
   tblP[OS_LYRG_POWER_STATS_THREAD_ID_E].name = "monitor";

   tblP[OS_LYRG_CHARGER_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_CHARGER_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_CHARGER_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_TRIGGER;
   tblP[OS_LYRG_CHARGER_THREAD_ID_E].name = "charger";

   tblP[OS_LYRG_PWR_MNGR_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_PWR_MNGR_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_PWR_MNGR_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_PWR_MNGR;
   tblP[OS_LYRG_PWR_MNGR_THREAD_ID_E].name = "power_manager";

   tblP[OS_LYRG_ISP_MONITOR_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_ISP_MONITOR_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYRG_ISP_MONITOR_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_ISP;
   tblP[OS_LYRG_ISP_MONITOR_THREAD_ID_E].name = "isp";

   tblP[OS_LYRG_CDC_MNGR_THREAD_ID_E].stackSize = OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_CDC_MNGR_THREAD_ID_E].schedulePolicy = SCHED_RR;
   tblP[OS_LYRG_CDC_MNGR_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_TRIGGER;
   tblP[OS_LYRG_CDC_MNGR_THREAD_ID_E].name = "cdc_mngr";

   tblP[OS_LYRG_UVC1_THREAD_ID_E].stackSize = OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_UVC1_THREAD_ID_E].schedulePolicy = SCHED_RR;
   tblP[OS_LYRG_UVC1_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_CLS_TX;
   tblP[OS_LYRG_UVC1_THREAD_ID_E].name = "uvc1_conn";

   tblP[OS_LYRG_UVC_OPR_THREAD_ID_E].stackSize = OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_UVC_OPR_THREAD_ID_E].schedulePolicy = SCHED_RR;
   tblP[OS_LYRG_UVC_OPR_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_UVC_SHTR;
   tblP[OS_LYRG_UVC_OPR_THREAD_ID_E].name = "uvc_shtr_opr";

   tblP[OS_LYRG_FW_UPDATE_THREAD_ID_E].stackSize      = OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYRG_FW_UPDATE_THREAD_ID_E].schedulePolicy = SCHED_RR;
   tblP[OS_LYRG_FW_UPDATE_THREAD_ID_E].priority       = OS_LYRP_PTHREAD_PRIORITY_FW_UPDATE;
   tblP[OS_LYRG_FW_UPDATE_THREAD_ID_E].name           = "fw_update";

   tblP[OS_LYR_SENSOR_UPDATER_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYR_SENSOR_UPDATER_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYR_SENSOR_UPDATER_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_SENSOR_UPDATER;
   tblP[OS_LYR_SENSOR_UPDATER_THREAD_ID_E].name = "snsrup";

   tblP[OS_LYR_SENSOR_SYNC_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYR_SENSOR_SYNC_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYR_SENSOR_SYNC_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_SENSOR_SYNC;
   tblP[OS_LYR_SENSOR_SYNC_THREAD_ID_E].name = "snsrsync";

   tblP[OS_LYR_HELSINKI_ISP_SYNC_THREAD_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYR_HELSINKI_ISP_SYNC_THREAD_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYR_HELSINKI_ISP_SYNC_THREAD_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_HELSINKI_ISP_SYNC;
   tblP[OS_LYR_HELSINKI_ISP_SYNC_THREAD_ID_E].name = "ispsync";

   tblP[OS_LYR_NUSERIAL_BG_TASK_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYR_NUSERIAL_BG_TASK_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYR_NUSERIAL_BG_TASK_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_BG_TASK;
   tblP[OS_LYR_NUSERIAL_BG_TASK_ID_E].name = "nuserialbg";

   tblP[OS_LYR_NUSERIAL_RECV_TASK_ID_E].stackSize= OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES;
   tblP[OS_LYR_NUSERIAL_RECV_TASK_ID_E].schedulePolicy= SCHED_RR;
   tblP[OS_LYR_NUSERIAL_RECV_TASK_ID_E].priority = OS_LYRP_PTHREAD_PRIORITY_RECV_TASK;
   tblP[OS_LYR_NUSERIAL_RECV_TASK_ID_E].name = "nuserialrcv";


   LOGG_PRINT(LOG_DEBUG_E, 0, "pthread Config Table:\n");
   for(i = 0; i < sizeof(OS_LYRP_pthreadParamsTbl)/sizeof(OS_LYRP_pthreadParamsTbl[0]); i++)
   {
      LOGG_PRINT(LOG_DEBUG_E, 0, "[%d] %s: policy=%d priority=%d stackSize=%d\n", i, tblP[i].name, tblP[i].schedulePolicy,tblP[i].priority, tblP[i].stackSize);
   }
   /*
   {
   struct timespec tp;
   ret = sched_rr_get_interval(0, &tp);
   tp.tv_sec = 0;
   tp.tv_nsec = 0;
   if(ret != 0)
      LOGG_PRINT(LOG_ERROR_E, 0, "sched_rr_get_interval error :%s\n",strerror(ret));
   else
      LOGG_PRINT(LOG_DEBUG_E, 0, "RR interval %d sec %d usec\n",tp.tv_sec, tp.tv_nsec/1000);
   }
   */

}





static int OS_LYRP_init(void)
{
   OS_LYRP_initPthreadTbl();
   return SUCCESS_E;
}

static void OS_LYRP_displayThreadAttr(pthread_attr_t *attr, const char *prefix)
{
   int s, i;
   size_t v;
   //void *stkaddr;
   struct sched_param sp;


   s = pthread_attr_getdetachstate(attr, &i);
   if (s != 0)
      LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getdetachstate error :%s\n",strerror(s));
   else
      LOGG_PRINT(LOG_DEBUG_E, 0, "%s Detach state        = %s\n", prefix,
               (i == PTHREAD_CREATE_DETACHED) ? "PTHREAD_CREATE_DETACHED" :
               (i == PTHREAD_CREATE_JOINABLE) ? "PTHREAD_CREATE_JOINABLE" :
               "???");

   s = pthread_attr_getscope(attr, &i);
   if (s != 0)
      LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getscope error :%s\n",strerror(s));
   else
      LOGG_PRINT(LOG_DEBUG_E,0, "%s Scope               = %s\n", prefix,
            (i == PTHREAD_SCOPE_SYSTEM)  ? "PTHREAD_SCOPE_SYSTEM" :
            (i == PTHREAD_SCOPE_PROCESS) ? "PTHREAD_SCOPE_PROCESS" :
            "???");

   s = pthread_attr_getinheritsched(attr, &i);
   if (s != 0)
      LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getinheritsched error :%s\n",strerror(s));
   else
      LOGG_PRINT(LOG_DEBUG_E,0,"%s Inherit scheduler   = %s\n", prefix,
            (i == PTHREAD_INHERIT_SCHED)  ? "PTHREAD_INHERIT_SCHED" :
            (i == PTHREAD_EXPLICIT_SCHED) ? "PTHREAD_EXPLICIT_SCHED" :
            "???");

   s = pthread_attr_getschedpolicy(attr, &i);
   if (s != 0)
      LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getschedpolicy error :%s\n",strerror(s));
   else
      LOGG_PRINT(LOG_DEBUG_E,0,"%s Scheduling policy   = %s\n", prefix,
            (i == SCHED_OTHER) ? "SCHED_OTHER" :
            (i == SCHED_FIFO)  ? "SCHED_FIFO" :
            (i == SCHED_RR)    ? "SCHED_RR" :
            "???");

  s = pthread_attr_getschedparam(attr, &sp);
  if (s != 0)
     LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getschedparam error :%s\n",strerror(s));
  else
     LOGG_PRINT(LOG_DEBUG_E,0,"%s Scheduling priority = %d\n", prefix, sp.sched_priority);

   s = pthread_attr_getguardsize(attr, &v);
   if (s != 0)
      LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getguardsize error :%s\n",strerror(s));
   else
      LOGG_PRINT(LOG_DEBUG_E,0,"%s Guard size          = %d bytes\n", prefix, v);

   s = pthread_attr_getstacksize(attr, &v);
   if (s != 0)
      LOGG_PRINT(LOG_DEBUG_E, 0, "pthread_attr_getstacksize error :%s\n",strerror(s));
   else
      LOGG_PRINT(LOG_DEBUG_E,0,"%s Stack size          = 0x%x bytes\n", prefix, v);

}

static void * OS_LYRP_threadWrapper(void *arg)
{
   OS_LYRP_pthreadArgsT *paramsP = (OS_LYRP_pthreadArgsT *)arg;

#ifdef _GNU_SOURCE
   paramsP->tid = syscall(SYS_gettid);
   LOGG_PRINT(LOG_DEBUG_E,0, "thread %s priority %d(linux %d) tid=%d\n",   paramsP->name, paramsP->priority, 100-(paramsP->priority+1),  paramsP->tid);
#endif
   //call user function
   paramsP->func(paramsP->arg);
   free(arg);

   return NULL;
}

static OS_LYRG_threadHandle OS_LYRP_createThread(OS_LYRG_threadParams *paramsP)
{
   OS_LYRG_threadHandle thrd = NULL;
   char thread_name[OS_LYR_GP_MAX_THREAD_NAME_LEN];

   pthread_attr_t attrs;
   int ret;
   struct sched_param scParams;
   assert(sizeof(OS_LYRG_threadHandle) >= sizeof(pthread_t));

   do
   {
      OS_LYRP_pthreadArgsT *thrdArgs = malloc(sizeof(OS_LYRP_pthreadArgsT));

      // Initialize the thread attributes
      ret = pthread_attr_init(&attrs);
      if(ret) break;

      //Set default attributes which are not in table
      ret = pthread_attr_setdetachstate(&attrs, OS_LYRP_DEFAULT_PTHREAD_DETACHED_STATE);
      if(ret) break;

      ret = pthread_attr_setguardsize(&attrs, OS_LYRP_DEFAULT_PTHREAD_GUARD_SIZE_BYTES);
      if(ret) break;

      ret = pthread_attr_setinheritsched(&attrs, OS_LYRP_DEFAULT_PTHREAD_INHERITED_SCHED);
      if(ret) break;

      ret = pthread_attr_setscope(&attrs, OS_LYRP_DEFAULT_PTHREAD_SCOPE);
      if(ret) break;

      //Set attributes from table according to ID
      ret = pthread_attr_setstacksize(&attrs, OS_LYRP_pthreadParamsTbl[paramsP->id].stackSize);
      if(ret) break;

      ret = pthread_attr_setschedpolicy(&attrs, OS_LYRP_pthreadParamsTbl[paramsP->id].schedulePolicy);
      if(ret) break;

      scParams.sched_priority = OS_LYRP_pthreadParamsTbl[paramsP->id].priority;
      ret = pthread_attr_setschedparam(&attrs, &scParams);
      if(ret) break;

      //Create the thread
      thrdArgs->arg      = paramsP->param;
      thrdArgs->func     = (PTHREAD_ROUTINE)paramsP->func;
      thrdArgs->name     = OS_LYRP_pthreadParamsTbl[paramsP->id].name;
      thrdArgs->priority = OS_LYRP_pthreadParamsTbl[paramsP->id].priority;
      ret = pthread_create((pthread_t *)&thrd, &attrs, (PTHREAD_ROUTINE)OS_LYRP_threadWrapper, thrdArgs);
      
      if(ret)
      {
         LOGG_PRINT(LOG_ERROR_E,0,"pthread_create() failed\n");
         break;
      }
      // set thread display name
      strncpy(thread_name, OS_LYRP_pthreadParamsTbl[paramsP->id].name, OS_LYR_GP_MAX_THREAD_NAME_LEN);
      pthread_setname_np((pthread_t)thrd, thread_name);
      OS_LYRP_displayThreadAttr(&attrs, OS_LYRP_pthreadParamsTbl[paramsP->id].name);
   }while(0);

   if(ret)
      LOGG_PRINT(LOG_ERROR_E,0, "error %d:%s\n",ret, strerror(ret));

   //Attributes should be destroyed after use

   ret = pthread_attr_destroy(&attrs);

   if(ret)
   {
      LOGG_PRINT(LOG_ERROR_E,0, "pthread_attr_destroy failed: %s\n",strerror(ret));
   }

   return thrd;
}

static ERRG_codeE OS_LYRP_waitForThread(OS_LYRG_threadHandle thrd)
{
   UINT32      retval;
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;

   retval = pthread_join((pthread_t)thrd, NULL);
   if(retval != SUCCESS_E)
   {
      ret = OS_LYR__ERR_WAIT_THREAD_FAIL; //Timeout or internal error
   }
   return ret;
}

static ERRG_codeE OS_LYRP_exitThread()
{
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;

   pthread_exit((void*)0);
   return ret;
}

static int OS_LYRP_closeThread(OS_LYRG_threadHandle thrd)
{
   pthread_cancel((pthread_t)thrd);
   pthread_join((pthread_t)thrd, NULL);

   return 0;
}

void OS_LYRP_displayPthreadMutexAttr(pthread_mutexattr_t *attrP)
{
   int ret;
   int val;

   ret = pthread_mutexattr_gettype(attrP, &val);
   if(!ret)
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_gettype %d\n  ", val);
      if(val ==PTHREAD_MUTEX_NORMAL)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_MUTEX_NORMAL\n");
      else if (val == PTHREAD_MUTEX_ERRORCHECK)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_MUTEX_ERRORCHECK\n");
      else if (val == PTHREAD_MUTEX_RECURSIVE)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_MUTEX_RECURSIVE\n");
      else if (val == PTHREAD_MUTEX_DEFAULT)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_MUTEX_DEFAULT\n");
      else
         LOGG_PRINT(LOG_ERROR_E, 0, "unknown\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_gettype error :%s\n",strerror(ret));
   }

   ret = pthread_mutexattr_getpshared(attrP, &val);
   if(!ret)
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_getpshared %d\n  ", val);
      if(val ==PTHREAD_PROCESS_SHARED)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_PROCESS_SHARED\n");
      else if (val == PTHREAD_PROCESS_PRIVATE)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_PROCESS_PRIVATE\n");
      else
         LOGG_PRINT(LOG_ERROR_E, 0, "unknown\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_getpshared error :%s\n",strerror(ret));
   }

   ret = pthread_mutexattr_getprotocol(attrP, &val);
   if(!ret)
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_getprotocol %d\n", val);
      if(val ==PTHREAD_PRIO_NONE)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_PRIO_NONE\n");
      else if (val == PTHREAD_PRIO_INHERIT)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_PRIO_INHERIT\n");
      else if (val == PTHREAD_PRIO_PROTECT)
         LOGG_PRINT(LOG_ERROR_E, 0, "PTHREAD_PRIO_PROTECT\n");
      else
         LOGG_PRINT(LOG_ERROR_E, 0, "unknown\n");

   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_getprotocol error :%s\n",strerror(ret));
   }

   ret = pthread_mutexattr_getprioceiling(attrP, &val);
   if(!ret)
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_getprioceiling %d\n", val);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "pthread_mutexattr_getprioceiling error :%s\n",strerror(ret));
   }

}

static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP)
{
   int ret;
   pthread_mutexattr_t attr;

   do
   {
      //init attribute
      ret = pthread_mutexattr_init(&attr);
      if(ret) break;
#ifdef _GNU_SOURCE
      //set priority inheritance protocol in attributes
      ret = pthread_mutexattr_setprotocol(&attr, OS_LYRP_PTHREAD_MUTEX_PROTOCOL);
      if(ret) break;
#endif
      //init mutex with attributes
      ret = pthread_mutex_init(mutexP, &attr);
      if(ret) break;

      //OS_LYRP_displayPthreadMutexAttr(&attr);
      ret = SUCCESS_E;

   }while(0);

   if(ret)
   {
      LOGG_PRINT(LOG_ERROR_E,0, "pthread_mutex initialization failure: %s\n",strerror(ret));
      ret = FAIL_E;
   }

   //need to destroy the attributes
   ret = pthread_mutexattr_destroy(&attr);
   if(ret)
   {
      LOGG_PRINT(LOG_ERROR_E,0, "pthread_mutexattr_destroy failed: %s\n",strerror(ret));
     ret = FAIL_E;
   }

   return ret;
}

static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP)
{
   int ret = (pthread_mutex_lock(mutexP) == 0) ? SUCCESS_E : FAIL_E;
   return ret;
}

static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP)
{
   int ret = (pthread_mutex_unlock(mutexP) == 0) ?  SUCCESS_E : FAIL_E;
   return ret;
}

static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP)
{
   int ret = SUCCESS_E;

   if(pthread_mutex_destroy(mutexP) != 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "pthread_mutex_destroy failed\n");
      ret = FAIL_E;
   }

   return ret;
}

static int OS_LYRP_mutexInitCp()
{
   MEM_MAPG_addrT cpMutexVirtBaseP;
   UINT32 cpMutexAddrSize;

   MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CP_MUTEX_E, (MEM_MAPG_addrT *)(&cpMutexVirtBaseP));
   MEM_MAPG_getSize(MEM_MAPG_CRAM_CP_MUTEX_E, &cpMutexAddrSize);

   /* reset shared memory mutex */
   memset( cpMutexVirtBaseP, 0x00, cpMutexAddrSize );

   return SUCCESS_E;
}

static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum)
{
   OS_LYRG_mutexCpT* cpMutexP;
   OS_LYRG_mutexCpT* cpMutexPhytBaseP;
   OS_LYRG_mutexCpT* cpMutexVirtBaseP;

   /* check mutex num legality */
   if ( ( (INT32)mutexNum < 0 ) || ( (UINT32)mutexNum >= OS_LYRG_NUM_CP_MUTEX_E) )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "MUTEX Index error: mutexNum = %d \n", (UINT32)mutexNum);
      return FAIL_E;
   }
   MEM_MAPG_getPhyAddr(MEM_MAPG_CRAM_CP_MUTEX_E, (MEM_MAPG_addrT *)(&cpMutexPhytBaseP));
   MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CP_MUTEX_E, (MEM_MAPG_addrT *)(&cpMutexVirtBaseP));
   cpMutexP = ( cpMutexPhytBaseP + mutexNum );
   *cpMutexhandleP = cpMutexP;
   cpMutexP = ( cpMutexVirtBaseP + mutexNum );

   if ( cpMutexP->indx == 0 )
      cpMutexP->indx = mutexNum;

   return SUCCESS_E;
}

static int OS_LYRP_releaseMutexCp( OS_LYRG_mutexCpT *cpMutexP)
{
   if ( cpMutexP->indx != 0 )
      cpMutexP->indx = 0;

   return SUCCESS_E;
}

static int OS_LYRP_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{
    OS_LYRG_mutexCpT cpMutexVirttBaseP;
    volatile UINT32* tmpP;
    UINT32           res;
    ERRG_codeE       retCode = SUCCESS_E;

    retCode = MEM_MAPG_convertPhysicalToVirtual((UINT32)&(cpMutexHandle->mutex),&(cpMutexVirttBaseP.mutex));
    if(ERRG_SUCCEEDED(retCode))
    {
        tmpP = (UINT32*)(cpMutexVirttBaseP.mutex);

        do
        {
            Asm("             mov      r1, #1         \n"
                "             ldrex    %0, [%1]       \n"
                : "=&r" (res) : "r" (tmpP)
                );

            if (res)
            {
                OS_LYRP_sleep(10);
                continue;
            }

            Asm("             strex    %0, r1, [%1]   \n"
                "             dmb                     \n"
                : "=&r" (res) : "r" (tmpP)
                );
        }
        while(res);
    }
    return retCode;
}

static int OS_LYRP_unlockMutexCp( OS_LYRG_mutexCpT *cpMutexHandle)
{
   OS_LYRG_mutexCpT cpMutexVirttBaseP;
   volatile UINT32* tmpP;

   MEM_MAPG_convertPhysicalToVirtual((UINT32)&(cpMutexHandle->mutex),&(cpMutexVirttBaseP.mutex));
   tmpP = (UINT32*)(cpMutexVirttBaseP.mutex);

   Asm("dmb \n"
       "mov      r1, #0\n" /*Immediate values need a # before the value for newer versions of GCC*/
       "str      r1, [%0]\n"
      : : "r" (tmpP));


   return SUCCESS_E;
}


static OS_LYRG_event OS_LYRP_createEvent(UINT32 id)
{
   OS_LYRG_event event;
   char name[64];
   memset(name,0,64);
   sprintf(name,"/OS_LYRG_event_%x_%u",getpid(),id);

   assert(sizeof(OS_LYRG_event) >= sizeof(sem_t *));

   sem_unlink(name);//In case it exists
   event = (OS_LYRG_event)sem_open(name, O_CREAT, S_IRUSR|S_IWUSR| S_IRGRP | S_IROTH  ,0);
   if(!event)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Event create: %s\n", strerror(errno));
      return NULL;
   }
   LOGG_PRINT(LOG_DEBUG_E, NULL, "%s %p created\n", name, event);

   return event;
}

/*
 * 0 - sucess- event received
 * 1 - fail - timeout without event
 */
static int OS_LYRP_waitEvent(OS_LYRG_event event, UINT32 timeoutMsec)
{
   int      ret = SUCCESS_E;
   struct   timespec ts; //for sem_timedwait
   struct   timeval tv; //for gettimeoday

   //if(clock_gettime(CLOCK_REALTIME, &ts) == 0)
   if(gettimeofday(&tv,NULL) == 0)
   {
      //Calculate the future time in usec
      UINT64 timeoutUsec = timeoutMsec * 1000;
      UINT32 test;

      //Convert to nano-second - handle wrap-around for usecs
      ts.tv_sec = tv.tv_sec + (timeoutUsec/1000000);
      test = tv.tv_usec + (timeoutUsec % 1000000);
      if(test < 1000000)
      {
         ts.tv_nsec = test * 1000;
      }
      else
      {
         //Overflow on usec - increment sec
         ts.tv_nsec = (test - 1000000)*1000;
         ts.tv_sec++;
      }

      if(sem_timedwait((sem_t*)event, &ts) == -1)
      {
         ret = FAIL_E;//timeout or error
         if(errno != ETIMEDOUT)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL,  "sem_timedwait failed %s\n", strerror(errno));
         }
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "gettimeofday failed\n");
      ret = FAIL_E;
   }

   return ret;
}

#if 0
static int OS_LYRP_waitMultipleEvents(UINT8 numOfEvents, OS_LYRG_event _events[], UINT32 timeoutMsec)
{
   return  SUCCESS_E;
}
#endif

static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event event)
{
   int ret;
   ret = sem_post((sem_t*)event);
   if(ret < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "OS_LYRG_setEvent: senm_post failed %s\n", strerror(errno));
      assert(0);
   }
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_releaseEvent(OS_LYRG_event event, UINT32 id)
{
   char name[64];
   memset(name,0,64);
   sprintf(name,"/OS_LYRG_event_%x_%u",getpid(),id);

   sem_close((sem_t*)event);
   sem_unlink(name);

   return SUCCESS_E;
}

static void OS_LYRP_sleep(int timeuSec)
{
   usleep(timeuSec);
}

static void OS_LYRP_getNsTime(UINT32 *secP, UINT32 *nsecP)
{
   struct timespec ts;
   int ret;

   ret = clock_gettime(CLOCK_REALTIME, &ts);
   if(ret >= 0)
   {
      *secP = ts.tv_sec;
      *nsecP = ts.tv_nsec;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "clock_gettime() failed %s\n", strerror(errno));
   }
}

static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP)
{
   struct timeval tv;

   if (gettimeofday(&tv, NULL) == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "gettimeofday failed\n");
   }
   else
   {
      *secP = tv.tv_sec;
      *msecP = tv.tv_usec/1000;
   }
   return;
}


static int OS_LYRP_setTime(INT32 sec, INT32 usec)
{
   struct timespec tv;

   tv.tv_sec = (time_t)sec;
   tv.tv_nsec = (long)(usec * 1000);
   return clock_settime(CLOCK_REALTIME, &tv);
}

static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   struct mq_attr queueAttr;
   int ret;
   int retVal = SUCCESS_E;

   ret = mq_unlink(msgQueP->name);
   if(ret < 0)
   {
      if((errno == EACCES) || (errno == ENAMETOOLONG))
         LOGG_PRINT(LOG_ERROR_E, NULL, "mq_unlink %s failed(ret=%d) %s\n", msgQueP->name, ret, strerror(errno));
   }

   // Message queue attributes //
   queueAttr.mq_flags=0;
   queueAttr.mq_maxmsg = msgQueP->maxMsgs;
   queueAttr.mq_msgsize = msgQueP->msgSize;

   msgQueP->mqHandle =  mq_open(msgQueP->name,O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IROTH | S_IWOTH, &queueAttr);
   if(msgQueP->mqHandle == (mqd_t)-1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error mq_open name=%s failed (attr %ld %ld %ld) %s\n",
      msgQueP->name, queueAttr.mq_flags, queueAttr.mq_maxmsg, queueAttr.mq_msgsize, strerror(errno));
      retVal = FAIL_E;
   }

   return retVal;
}

static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   int ret;
   int retVal = SUCCESS_E;

   //Close/remove local message queue
   ret = mq_close(msgQueP->mqHandle);
   if(ret == 0)
   {
      ret = mq_unlink(msgQueP->name);
      if(ret < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "warning mq_unlink failed(ret=%d)\n", ret);
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error mq_close failed\n");
      retVal = FAIL_E;
   }

   return retVal;
}

static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len)
{
   enum {PRIORITY = 0};
   int retVal = SUCCESS_E;
   int ret;
   struct timespec ts;

   //for non-blocking on send set these to 0. send will attempt to send and return immediately if queue is full
   ts.tv_sec = 0;
   ts.tv_nsec = 0;
   ret = mq_timedsend(msgQueP->mqHandle,(char *)msgP,len, PRIORITY,&ts);
   if(ret < 0)
   {
      if(errno != ETIMEDOUT)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "mq_send failed - mqHandle = %d, error = %s\n",msgQueP->mqHandle,strerror(errno));
      }
      retVal = FAIL_E;
   }

   return retVal;
}


static int OS_LYRP_waitRecvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP)
{
   enum { PRIORITY = 0};
   int ret = SUCCESS_E;
   int msgSize;

   //if(clock_gettime(CLOCK_REALTIME, &ts) == 0) //Not supported
      //Calculate the future time in usec

   msgSize = mq_receive(msgQueP->mqHandle, (char *)msgP, *lenP, PRIORITY);
   if(msgSize < 0)
   {
         //error (not a normal timeout)
         LOGG_PRINT(LOG_ERROR_E, NULL, "mq_timedreceive failed ret=%d len=%d %s \n", msgSize, *lenP, strerror(errno));
         ret = FAIL_E;
    }
   else
   {
      //On success the msg size should equal to the que message size (overwrite the lenP argument).
      *lenP = msgSize;
   }


   return ret;
}



static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeoutMsec)
{
   enum {MILLION = 1000000, PRIORITY = 0};
   struct timespec ts; //for sem_timedwait
   struct timeval tv; //for gettimeoday
   int ret = SUCCESS_E;
   int msgSize;

   //if(clock_gettime(CLOCK_REALTIME, &ts) == 0) //Not supported
   if(gettimeofday(&tv,NULL) == 0)
   {
      //Calculate the future time in usec
      UINT64 timeoutUsec = timeoutMsec * 1000;
      UINT32 test;

      //Convert to nano-second - handle wrap-around for usecs
      ts.tv_sec = tv.tv_sec + (timeoutUsec/MILLION);
      test = tv.tv_usec + (timeoutUsec % MILLION);
      if(test < MILLION)
      {
         ts.tv_nsec = test * 1000;
      }
      else
      {
         //Overflow on usec - increment sec
         ts.tv_nsec = (test - MILLION)*1000;
         ts.tv_sec++;
      }

      msgSize = mq_timedreceive(msgQueP->mqHandle, (char *)msgP, *lenP, PRIORITY, &ts);
      if(msgSize < 0)
      {
         //normal timeout or error
         if(errno == ETIMEDOUT)
         {
            //Normal timeout - set len to 0
            *lenP = 0;
         }
         else
         {
            //error (not a normal timeout)
            LOGG_PRINT(LOG_ERROR_E, NULL, "mq_timedreceive failed ret=%d len=%d %s \n", msgSize, *lenP, strerror(errno));
            ret = FAIL_E;
         }
      }
      else
      {
         //On success the msg size should equal to the que message size (overwrite the lenP argument).
         *lenP = msgSize;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "gettimeofday failed\n");
      ret = FAIL_E;
   }

   return ret;
}

static unsigned int OS_LYRP_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP)
{
   struct mq_attr attr;
   int ret;
   unsigned int curr = 0;

   ret = mq_getattr(msgQueP->mqHandle, &attr);
   if(ret >= 0)
   {
      curr = attr.mq_curmsgs;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "mq_getattr failed - mqHandle = %d, error = %s\n",msgQueP->mqHandle,strerror(errno));
   }
   return curr;
}


static void OS_LYRP_sigExpiryHandler (union sigval val)
{
   OS_LYRG_timerT *timerP;

   if (val.sival_ptr)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL , "received signal, val.sival_ptr = %p\n",val.sival_ptr);
      timerP = (OS_LYRG_timerT*)val.sival_ptr;
      timerP->funcCb(timerP->argP);
   }
   else
   {
      LOGG_PRINT(LOG_WARN_E, NULL , "WARNING: received signal but CB is null!, val.sival_ptr = %d\n",val.sival_ptr);
   }
}


static ERRG_codeE OS_LYRP_createTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   pthread_attr_t attr;
   pthread_attr_init( &attr );

   //Set default attributes which are not in table
   ret = pthread_attr_setdetachstate(&attr, OS_LYRP_DEFAULT_PTHREAD_DETACHED_STATE);
   if(ret) return FAIL_E;

   ret = pthread_attr_setguardsize(&attr, OS_LYRP_DEFAULT_PTHREAD_GUARD_SIZE_BYTES);
   if(ret) return FAIL_E;

   ret = pthread_attr_setinheritsched(&attr, OS_LYRP_DEFAULT_PTHREAD_INHERITED_SCHED);
   if(ret) return FAIL_E;

   ret = pthread_attr_setscope(&attr, OS_LYRP_DEFAULT_PTHREAD_SCOPE);
   if(ret) return FAIL_E;

   //Set attributes from table according to ID
   ret = pthread_attr_setstacksize(&attr, OS_LYRP_DEFAULT_PTHREAD_STACK_SIZE_BYTES);
   if(ret) return FAIL_E;

   ret = pthread_attr_setschedpolicy(&attr, SCHED_RR);
   if(ret) return FAIL_E;

   struct sched_param parm;
   parm.sched_priority = OS_LYRP_PTHREAD_PRIORITY_TIMER;
   pthread_attr_setschedparam(&attr, &parm);

   struct sigevent sig;
   sig.sigev_notify = SIGEV_THREAD;
   sig.sigev_notify_function = OS_LYRP_sigExpiryHandler;
   sig.sigev_value.sival_ptr = timerP;
   sig.sigev_notify_attributes = &attr;

   //create a new timer.
   if (timer_create(CLOCK_REALTIME , &sig, &(timerP->timerid)) != 0)
   {
     LOGG_PRINT(LOG_ERROR_E, NULL, "timer_create failed callbackId=%d errno = %s\n",timerP->callbackId, strerror(errno));
     ret = FAIL_E;
   }
   else
   {
     LOGG_PRINT(LOG_DEBUG_E, NULL, "timer_create success: callbackId=%d, timerId=0x%x\n",timerP->callbackId,timerP->timerid);
     ret = OS_LYR__RET_SUCCESS;
   }

   pthread_attr_destroy(&attr);

   return ret;
}


static ERRG_codeE OS_LYRP_setTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   struct itimerspec  newValue;

   newValue.it_value.tv_sec     = timerP->firstExpirySec;
   newValue.it_value.tv_nsec    = timerP->firstExpiryNsec;
   newValue.it_interval.tv_sec  = timerP->intervalExpirySec;
   newValue.it_interval.tv_nsec = timerP->intervalExpiryNsec;

   if (timer_settime(timerP->timerid, 0, &newValue, NULL ) != 0)
   {
       LOGG_PRINT(LOG_ERROR_E, NULL, "timer_settime failed: timerid=0x%x, sec = %d, nsec = %d, int_sec = %d, int_nsec = %d, errno = %s \n",
                  timerP->timerid,
                  newValue.it_value.tv_sec,
                  newValue.it_value.tv_nsec,
                  newValue.it_interval.tv_sec,
                  newValue.it_interval.tv_nsec,
                  strerror(errno));

       ret = FAIL_E;
   }
   else
   {
       LOGG_PRINT(LOG_DEBUG_E, NULL, "timer_settime success: timerid=0x%x, sec = %d, nsec = %d, int_sec = %d, int_nsec = %d\n",
                  timerP->timerid,
                  newValue.it_value.tv_sec,
                  newValue.it_value.tv_nsec,
                  newValue.it_interval.tv_sec,
                  newValue.it_interval.tv_nsec);
   }

   return ret;

}


static ERRG_codeE OS_LYRP_deleteTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;

   if (timerP->timerid)
   {
      if (timer_delete(timerP->timerid) != 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "timer_delete failed: timerid=0x%x, errno = %s \n", timerP->timerid, strerror(errno));
         ret = FAIL_E;
      }
      timerP->timerid = NULL;
   }

   return ret;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE OS_LYRG_init()
{
   ERRG_codeE ret;

   ret = (ERRG_codeE)OS_LYRP_init();
   return ret;
}

OS_LYRG_threadHandle OS_LYRG_createThread(OS_LYRG_threadParams *params)
{
   OS_LYRG_threadHandle thrd = 0;

   thrd = OS_LYRP_createThread(params);
   return thrd;
}

ERRG_codeE OS_LYRG_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec)
{
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(timeoutMsec);
   ret = OS_LYRP_waitForThread(thrd);
   return(ret);
}

int OS_LYRG_closeThread(OS_LYRG_threadHandle thrd)
{
   return OS_LYRP_closeThread(thrd);
}

ERRG_codeE OS_LYRG_exitThread()
{
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;
   ret = OS_LYRP_exitThread();
   return(ret);
}

int OS_LYRG_aquireMutex(OS_LYRG_mutexT *mutexP)
{
   return OS_LYRP_aquireMutex(mutexP);
}

int OS_LYRG_lockMutex(OS_LYRG_mutexT *mutexP)
{
   return OS_LYRP_lockMutex(mutexP);
}

int OS_LYRG_unlockMutex(OS_LYRG_mutexT *mutexP)
{
   return OS_LYRP_unlockMutex(mutexP);
}

int OS_LYRG_releaseMutex(OS_LYRG_mutexT *mutexP)
{
   return OS_LYRP_releaseMutex(mutexP);
}

int OS_LYRG_mutexInitCp()
{
   return OS_LYRP_mutexInitCp();
}
int OS_LYRG_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum)
{
   return OS_LYRP_aquireMutexCp(cpMutexhandleP, mutexNum);
}

int OS_LYRG_releaseMutexCp(OS_LYRG_mutexCpT *cpMutexP)
{
   return OS_LYRP_releaseMutexCp(cpMutexP);
}

int OS_LYRG_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{
   return OS_LYRP_unlockMutexCp(cpMutexHandle);
}

int OS_LYRG_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{
   return OS_LYRP_lockMutexCp(cpMutexHandle);
}

OS_LYRG_event OS_LYRG_createEvent(UINT32 id)
{
   return OS_LYRP_createEvent(id);
}

int OS_LYRG_releaseEvent(OS_LYRG_event event, UINT32 id)
{
   return OS_LYRP_releaseEvent(event, id);
}

int OS_LYRG_setEvent(OS_LYRG_event event)
{
   return OS_LYRP_setEvent(event);
}

int OS_LYRG_waitEvent(OS_LYRG_event event,UINT32 timeout)
{
   return OS_LYRP_waitEvent(event,timeout);
}

void OS_LYRG_usleep(int timeuSec)
{
   OS_LYRP_sleep(timeuSec);
}

void OS_LYRG_getTime(UINT32 *secP, UINT16 *msecP)
{
   UINT32 nsec = 0;
   OS_LYRP_getNsTime(secP, &nsec);
   *msecP = nsec/1000000UL;

}

ERRG_codeE OS_LYRG_setTime(INT32 sec, INT32 usec)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;

   if (OS_LYRP_setTime(sec, usec) == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "settimeofday failed, sec = %ld, usec = %ld\n",sec,usec);
      //ret = ERR_UNEXPECTED; //TEST
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "settimeofday success, sec = %ld, usec = %ld\n",sec,usec);
   }

   return ret;
}

void OS_LYRG_getTimeNsec(UINT64 *nsecP)
{
   UINT32 sec=0,nsec=0;
   OS_LYRP_getNsTime(&sec, &nsec);
   *nsecP = (((UINT64)sec) * NSEC_PER_SEC) + nsec;
}

void OS_LYRG_getMsecTime(UINT32 *msecP)
{
   UINT32 currSec = 0;
   UINT16 currMsec = 0;
   OS_LYRP_getTime(&currSec, &currMsec);
   *msecP = (currSec *1000) + currMsec;
}

void OS_LYRG_getUsecTime(UINT64 *usecP)
{
   OS_LYRG_getTimeNsec(usecP);
   *usecP = *usecP / 1000; //convert ns to us
}

UINT32 OS_LYRG_deltaMsec(UINT32 prevSec, UINT16 prevMsec)
{
   UINT32 deltaSec = 0;
   UINT16 deltaMsec = 0;
   UINT32 currSec = 0;
   UINT16 currMsec = 0;

   OS_LYRG_getTime(&currSec, &currMsec);
   if(currSec > prevSec)
   {
      deltaSec = currSec - prevSec - 1;
      deltaMsec = (1000 - prevMsec) + currMsec;
   }
   else if(currSec == prevSec)
   {
      deltaSec = 0;
      deltaMsec = currMsec - prevMsec;
   }

   return ((deltaSec * 1000) + deltaMsec);
}

int OS_LYRG_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents)
{
   FIX_UNUSED_PARAM_WARN(numOfEvents);
   return OS_LYRP_createMsgQue(msgQueP);
}

int OS_LYRG_deleteMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   return OS_LYRP_deleteMsgQue(msgQueP);
}

int OS_LYRG_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len)
{
   return OS_LYRP_sendMsg(msgQueP, msgP, len);
}

int OS_LYRG_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeoutMsec)
{
   return OS_LYRP_recvMsg(msgQueP, msgP, lenP, timeoutMsec);
}

int OS_LYRG_waitRecvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP)
{
   return OS_LYRP_waitRecvMsg(msgQueP, msgP, lenP);
}

ERRG_codeE OS_LYRG_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId)
{
   FIX_UNUSED_PARAM_WARN(msgQueP);
   FIX_UNUSED_PARAM_WARN(eventId);
   return OS_LYR__RET_SUCCESS;
}

unsigned int OS_LYRG_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP)
{
   return OS_LYRP_getCurrNumMsg(msgQueP);
}

/****************************************************************************
*
*  Function Name: OS_LYRG_openMemFd
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/

ERRG_codeE OS_LYRG_openMemFd(INT32 *memFdP)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   INT32 status;

   LOGG_PRINT(LOG_INFO_E, NULL, "memory module open\n");

   /* First initialize the CMEM module */
   status =(INT32) CMEM_init();
   if(status == OS_LYRP_VIRT_MEM_DUMMY_FD)
   {
      ret = OS_LYR__ERR_MEM_FD_OPEN_FAIL;
      LOGG_PRINT(LOG_ERROR_E, ret, "memory module open fail !!!\n");
   }

   *memFdP = open("/dev/mem", O_RDWR | O_SYNC);
   if(*memFdP == OS_LYRP_VIRT_MEM_DUMMY_FD)
   {
      ret = OS_LYR__ERR_MEM_FD_OPEN_FAIL;
      LOGG_PRINT(LOG_ERROR_E, ret, "/dev/mem open fail !!!\n");
   }

   return ret;
}

ERRG_codeE OS_LYRG_openBuffMemFd(INT32 *memFdP)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   unsigned int data,rc;

   *memFdP = open("/dev/inu_mem", O_RDWR | O_SYNC);
   return ret;
}

ERRG_codeE OS_LYRG_closeBuffMemFd(INT32 memFd)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   close(memFd);
   return ret;
}

/****************************************************************************
*
*  Function Name: OS_LYRG_closeMemFd
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE OS_LYRG_closeMemFd(INT32 memFd)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   CMEM_exit();
   close(memFd);
   return ret;
}

/****************************************************************************
*
*  Function Name: OS_LYRG_mapRegion
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE OS_LYRG_mapRegion(INT32 memFd, UINT32 *phyAddrP, UINT32 regionSize, BYTE isCached, UINT32 *virtAddrP, INT32 regionId, INT32 *dmaBufFD)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   UINT32 devPhyBaseAddr = *phyAddrP;
   void           *mapped_base;
   off_t          dev_base=0;

   // Map via mmap
   if (regionId == -1)
   {
      dev_base = (UINT32)(devPhyBaseAddr);

      mapped_base = mmap(0, regionSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, (off_t)devPhyBaseAddr & ~OS_LYRP_VIRT_MEM_MASK);
      if(mapped_base == (void *)OS_LYRP_VIRT_MEM_DUMMY_FD)
      {
         ret = OS_LYR__ERR_REGION_MAP_FAIL;
         LOGG_PRINT(LOG_ERROR_E, ret, "OS_LYR__ERR_REGION_MAP_FAIL using mmap\n");
      }
      else
      {
         *virtAddrP = (UINT32)mapped_base;
         LOGG_PRINT(LOG_DEBUG_E, NULL, "cmem alloc. dev_base = 0x%X  \n",dev_base);
      }
   }
   else if (regionId == 2)
   {
      dev_base = (UINT32)(devPhyBaseAddr);

      mapped_base = mmap(0, regionSize, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0);
      if(mapped_base == (void *)OS_LYRP_VIRT_MEM_DUMMY_FD)
      {
         ret = OS_LYR__ERR_REGION_MAP_FAIL;
         LOGG_PRINT(LOG_ERROR_E, ret, "OS_LYR__ERR_REGION_MAP_FAIL using mmap\n");
      }
      else
      {
         *virtAddrP = (UINT32)mapped_base;
         LOGG_PRINT(LOG_DEBUG_E, NULL, "cmem alloc. dev_base = 0x%X  \n",dev_base);
     }
   }
   // Map via CMEM
   else
   {
      CMEM_AllocParams cmemParams= {
                                    CMEM_HEAP,        /* type */
                                    CMEM_NONCACHED,   /* flags */
                                    4                 /* alignment */
                                    };
      if (isCached)
      {
         cmemParams.flags = CMEM_CACHED;
      }
      else
      {
         cmemParams.flags = CMEM_NONCACHED | CMEM_PHYS | CMEM_WB | CMEM_INV;
      }

      mapped_base = CMEM_alloc2(regionId, regionSize, &cmemParams);
      *phyAddrP = (UINT32) CMEM_getPhys(mapped_base);
      dev_base = *phyAddrP;
      LOGG_PRINT(LOG_DEBUG_E, NULL, "cmem alloc. dev_base = 0x%X  \n",dev_base);

      if(mapped_base == NULL)
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "OS_LYR__ERR_REGION_MAP_FAIL using cmem: id = %d, size = %d\n",regionId, regionSize);

         ret = OS_LYR__ERR_REGION_MAP_FAIL;
      }
      else
      {
         /*Store the DMA Buf file descriptor which is useful for converting a buffer into a scatter gather table within the kernel.
         They're also quite useful in general and seem to be the de-facto standard for passing around buffers that can be used with DMA controllers
         Please see https://docs.kernel.org/driver-api/dma-buf.html for more information
         */
         if(dmaBufFD)
         {
            *dmaBufFD= CMEM_export_dmabuf(mapped_base);
            LOGG_PRINT(LOG_DEBUG_E, NULL, "CMEM File Buffer=0x%p \n",*dmaBufFD);
         }
         *virtAddrP = (UINT32)mapped_base;
      }
  }
  return ret;
}

/****************************************************************************
*
*  Function Name: OS_LYRG_unmapRegion
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Memory
*
****************************************************************************/
ERRG_codeE OS_LYRG_unmapRegion(void *baseAddr, UINT32 regionSize, INT32 regionId)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   INT32 status;
   if (regionId == -1)
   {
      status = munmap(baseAddr , regionSize);
      if(status == OS_LYRP_VIRT_MEM_DUMMY_FD)
      {
         ret = OS_LYR__ERR_REGION_UNMAP_FAIL;
         LOGG_PRINT(LOG_ERROR_E, ret, "UnMap memory region fail !!!\n");
      }
   }
   else
   {
      if (CMEM_free(baseAddr, NULL) < 0)
      {
         ret = OS_LYR__ERR_REGION_UNMAP_FAIL;
         LOGG_PRINT(LOG_ERROR_E, ret, "UnMap memory region fail !!!\n");
      }
   }
   return ret;
}

ERRG_codeE OS_LYRG_createTimer( OS_LYRG_timerT *timerP )
{
   return OS_LYRP_createTimer(timerP);
}

ERRG_codeE OS_LYRG_setTimer( OS_LYRG_timerT *timerP )
{
   return OS_LYRP_setTimer(timerP);
}

ERRG_codeE OS_LYRG_deleteTimer( OS_LYRG_timerT *timerP )
{
   return OS_LYRP_deleteTimer(timerP);
}

ERRG_codeE OS_LYRG_createTmpFile( FILE **file )
{
   FILE *tmpFile;
   tmpFile = tmpfile();
   if (!tmpFile)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed on tmpfile()\n");
      return OS_LYR__ERR_FILE_OPEN_FAIL;
   }
   *file = tmpFile;
   return OS_LYR__RET_SUCCESS;
}
#endif


#ifdef __cplusplus
}
#endif
