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

#include "inu_types.h"
#if DEFSG_IS_CEVA

#include "err_defs.h"
#include "log.h"
#include "os_lyr.h"
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "mss.h"
#include "mss_config.h"
//#include "VP_IO.h"
#include "mem_map.h"
#include "vp_int.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
 
#define MM4K_CORE_FREQUENCY         (600000000)
#define MM4K_NUM_OF_TICKS_IN_USEC   (MM4K_CORE_FREQUENCY/1000000)
#define OS_LYRP_TOTAL_LOCKERS         (20)
#define OS_LYRP_MEMLOCK_ADDR_LOW      (0x00000000)
#define OS_LYRP_MEMLOCK_ADDR_HIGH     (0xFFFFFFFF)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef enum
{
   OS_LYRP_CPMUTEX_FREE_E     = 0,
   OS_LYRP_CPMUTEX_OCCUPY_E   = 1,
   OS_LYRP_CPMUTEX_LAST_E
} OS_LYRP_cpMutexStatusE;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static int OS_LYRP_init(void);
#if 0
static OS_LYRG_threadHandle OS_LYRP_createThread(OS_LYRG_threadParamsP params);
static ERRG_codeE OS_LYRP_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec);
static ERRG_codeE OS_LYRP_exitThread();
static int OS_LYRP_closeThread(OS_LYRG_threadHandle thrd);

static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP);

static OS_LYRG_event OS_LYRP_createEvent(UINT32 id);
static int OS_LYRP_waitEvent(OS_LYRG_event event, UINT32 timeoutMsec);
static int OS_LYRP_waitMultipleEvents(UINT8 numOfEvents, OS_LYRG_event _events[], UINT32 timeoutMsec);
static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event event);
static int OS_LYRP_releaseEvent(OS_LYRG_event event, UINT32 id);
static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents);
static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeouMsec);
#endif

static int OS_LYRP_mutexInitCp();
static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum);
static int OS_LYRP_releaseMutexCp(OS_LYRG_mutexCpT *cpMutexP);
static int OS_LYRP_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
static int OS_LYRP_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);

static void OS_LYRP_sleep(int timeuSec);
static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP);

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

static int OS_LYRP_init(void)
{
   return OS_LYR__RET_SUCCESS;
}

#if 0
static OS_LYRG_threadHandle OS_LYRP_createThread(OS_LYRG_threadParamsP params)
{
   OS_LYRG_threadHandle thrd = 0;

   return thrd;
}


static ERRG_codeE OS_LYRP_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec)
{
   return OS_LYR__RET_SUCCESS;
}


static int OS_LYRP_closeThread(OS_LYRG_threadHandle thrd)
{
   return 0;
}


static ERRG_codeE OS_LYRP_exitThread()
{
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;

   return ret;
}

static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP)
{
   mutexP = NULL;
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP)
{
   mutexP = NULL;
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP)
{
   mutexP = NULL;
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP)
{
   mutexP = NULL;
   return OS_LYR__RET_SUCCESS;
}

static OS_LYRG_event OS_LYRP_createEvent(UINT32 id)
{
   OS_LYRG_event _event;

   return _event;
}

static int OS_LYRP_releaseEvent(OS_LYRG_event _event, UINT32 id)
{

   return OS_LYR__RET_SUCCESS;
}

static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event _event)
{
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_waitEvent(OS_LYRG_event _event, UINT32 timeoutMsec)
{
   return  OS_LYR__RET_SUCCESS; 
}

static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents)
{

   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   return OS_LYR__RET_SUCCESS;
}

unsigned int OS_LYRP_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP)
{
   return 0;
}

static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len)
{
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeouMsec)
{
   return OS_LYR__RET_SUCCESS;
}

static ERRG_codeE OS_LYRP_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId)
{
   return OS_LYR__RET_SUCCESS;
}

#endif

static int OS_LYRP_mutexInitCp()
{
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum)
{
   OS_LYRG_mutexCpT* cpMutexP;
   OS_LYRG_mutexCpT* cpMutexPhyBaseP;

   /* check mutex num legality */
   if ( ( (UINT32)mutexNum < 0 ) || ( (UINT32)mutexNum >= OS_LYRG_NUM_CP_MUTEX_E) )
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "MUTEX Index error: mutexNum = %d \n", (UINT32)mutexNum);
      return FAIL_E;
   }

   //MEM_MAPG_getPhyAddr(MEM_MAPG_CRAM_CP_MUTEX_E, (MEM_MAPG_addrT *)(&cpMutexPhyBaseP));
   cpMutexPhyBaseP = (OS_LYRG_mutexCpT*)MEM_MAPG_CP_MUTEX_ADDRESS;
   cpMutexP = ( cpMutexPhyBaseP + mutexNum );

   if ( cpMutexP->indx == 0 )
      cpMutexP->indx = mutexNum;

   *cpMutexhandleP = cpMutexP;
   
   return SUCCESS_E;

}

static int OS_LYRP_releaseMutexCp(OS_LYRG_mutexCpT *cpMutexP)
{
   if ( cpMutexP->indx != 0 )
   cpMutexP->indx = 0;

   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{
   MSS_CONFIGG_lockMutexCp(&(cpMutexHandle->mutex));

   return SUCCESS_E;
}

static int OS_LYRP_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{
   MSS_CONFIGG_unLockMutexCp(&(cpMutexHandle->mutex));

   return SUCCESS_E;
}

static void OS_LYRP_sleep(int timeuSec)
{
   volatile UINT64 t1, t2;

   t1 = mss_get_clock();

   t2 = t1 + MM4K_NUM_OF_TICKS_IN_USEC*timeuSec;
   
   while(t1 < t2)
   {
      t1 = mss_get_clock();
   }
}

static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP)
{
   UINT32 t;
   
   t = mss_get_clock();
   *secP = (t)/(MM4K_CORE_FREQUENCY);
   *msecP = (1000*t)/(MM4K_CORE_FREQUENCY);

   return;
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

#if 0
OS_LYRG_threadHandle OS_LYRG_createThread(OS_LYRG_threadParams *params)
{
   OS_LYRG_threadHandle thrd = 0;

   thrd = OS_LYRP_createThread(params);
   return thrd;
}

ERRG_codeE OS_LYRG_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec)
{
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;
   ret = OS_LYRP_waitForThread(thrd, timeoutMsec);
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

int OS_LYRG_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents)
{
   return OS_LYRP_createMsgQue(msgQueP, numOfEvents);
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

ERRG_codeE OS_LYRG_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId)
{
   return OS_LYRP_setEventMsgQueue(msgQueP, eventId);
}

unsigned int OS_LYRG_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP)
{
   return OS_LYRP_getCurrNumMsg(msgQueP);
}

#endif

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

void OS_LYRG_usleep(int timeuSec)
{
   OS_LYRP_sleep(timeuSec);
}

void OS_LYRG_getTime(UINT32 *secP, UINT16 *msecP)
{
   OS_LYRP_getTime(secP, msecP);
}

void OS_LYRG_getMsecTime(UINT32 *msecP)
{
   UINT32 currSec = 0;
   UINT16 currMsec = 0;
   OS_LYRP_getTime(&currSec, &currMsec);
   *msecP = (currSec *1000) + currMsec;
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

void OS_LYRG_enableInterrupts(unsigned long flags)
{
   VP_INTG_enableInterrupts();
}

long OS_LYRG_disableInterrupts()
{
   VP_INTG_disableInterrupts();
   return 0;
}

#ifdef __cplusplus
}
#endif


#endif
