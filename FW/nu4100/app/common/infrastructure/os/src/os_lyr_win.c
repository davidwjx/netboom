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



#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/

#if DEFSG_IS_HOST_WINDOWS


#define OS_LYRP_TOTAL_LOCKERS      (20)
#define OS_LYRP_MEMLOCK_ADDR_LOW   (0x00000000)
#define OS_LYRP_MEMLOCK_ADDR_HIGH  (0xFFFFFFFF)

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
static LARGE_INTEGER frequency={0};

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static int OS_LYRP_init(void);
static OS_LYRG_threadHandle OS_LYRP_createThread(OS_LYRG_threadParamsP params);
static ERRG_codeE OS_LYRP_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec);
static ERRG_codeE OS_LYRP_exitThread();
static int OS_LYRP_closeThread(OS_LYRG_threadHandle thrd);

static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP);
static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP);

static OS_LYRG_event OS_LYRP_createEvent();
static int OS_LYRP_waitEvent(OS_LYRG_event event, UINT32 timeoutMsec);
static int OS_LYRP_waitMultipleEvents(UINT8 numOfEvents, OS_LYRG_event _events[], UINT32 timeoutMsec);
static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event event);
static int OS_LYRP_releaseEvent(OS_LYRG_event event, UINT32 id);

static void OS_LYRP_sleep(int timeuSec);
static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP);

static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents);
static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeouMsec);

static VOID CALLBACK timer_sig_handler(PVOID lpParameter, BOOLEAN TimerOrWaitFired);

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/*******************************************_UNIQUE_OS_********************************/




/*******************************************WIN32*************************************/


#include <windows.h>

static int OS_LYRP_init(void)
{
   return SUCCESS_E;
}

static OS_LYRG_threadHandle OS_LYRP_createThread(OS_LYRG_threadParamsP params)
{
   OS_LYRG_threadHandle thrd;
   DWORD threadID;

   thrd = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)((OS_LYRG_threadParamsP)params)->func, params->param, 0, &threadID);

   return thrd;
}

static ERRG_codeE OS_LYRP_waitForThread(OS_LYRG_threadHandle thrd, UINT32 timeoutMsec)
{
   UINT32      retval;
   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;

   retval = WaitForSingleObject(thrd, timeoutMsec);
   if(retval != WAIT_OBJECT_0)
   {
      ret = OS_LYR__ERR_WAIT_THREAD_FAIL; //Timeout or internal error
   }
   return ret;
}

static ERRG_codeE OS_LYRP_exitThread()
{
//   ERRG_codeE  ret = OS_LYR__RET_SUCCESS;

   ExitThread(0);

//   return ret;
}

static int OS_LYRP_closeThread(OS_LYRG_threadHandle thrd)
{
   return CloseHandle(thrd);
}

static int OS_LYRP_aquireMutex(OS_LYRG_mutexT *mutexP)
{

   int ret = SUCCESS_E;
   *mutexP = CreateMutex(NULL, FALSE, NULL);
   if (!*mutexP)
   {
      ret = FAIL_E;
   }
   return ret;
}

static int OS_LYRP_lockMutex(OS_LYRG_mutexT *mutexP)
{
   return  (WaitForSingleObject(*mutexP, INFINITE) == WAIT_OBJECT_0) ? SUCCESS_E : FAIL_E;
}

static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP)
{
   int retVal = ReleaseMutex(*mutexP);
   if (retVal)
   {
      return SUCCESS_E;
   }
   return FAIL_E;
}

static int OS_LYRP_releaseMutex(OS_LYRG_mutexT *mutexP)
{
   int retVal = CloseHandle(*mutexP);
   if (retVal)
   {
      return SUCCESS_E;
   }
   return FAIL_E;
}

static OS_LYRG_event OS_LYRP_createEvent()
{
   OS_LYRG_event _event;
   _event = CreateEvent(NULL, FALSE,FALSE, NULL);
   return _event;
}

static int OS_LYRP_releaseEvent(OS_LYRG_event _event, UINT32 id)
{
   int retVal = CloseHandle(_event);
   FIX_UNUSED_PARAM_WARN(id);

   if (retVal)
   {
      return SUCCESS_E;
   }
   return FAIL_E;
}

static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event _event)
{
   int retVal = SetEvent(_event);
   if (retVal)
   {
      return OS_LYR__RET_SUCCESS;
   }
   return OS_LYR__ERR_UNEXPECTED;
}

static int OS_LYRP_waitEvent(OS_LYRG_event _event, UINT32 timeoutMsec)
{
   return  (WaitForSingleObject(_event, timeoutMsec) == WAIT_OBJECT_0) ? SUCCESS_E : FAIL_E; 
}

static int OS_LYRP_waitMultipleEvents(UINT8 numOfEvents, OS_LYRG_event _events[], UINT32 timeoutMsec)
{
   return  (WaitForMultipleObjects(numOfEvents, _events, FALSE, timeoutMsec) < numOfEvents) ? SUCCESS_E : FAIL_E; 
}

static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents)
{
   int retval = SUCCESS_E;
   if (msgQueP)
   {
      //assert(msgQueP->queue.empty());

      retval = OS_LYRP_aquireMutex(&(msgQueP->queueMutex));
      if (retval == SUCCESS_E)
      {
         msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E] = OS_LYRP_createEvent();
         if(numOfEvents > 1)
         {
            msgQueP->queueEvent[OS_LYRG_EVENT_USER_E] = OS_LYRP_createEvent();
            if(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E] && msgQueP->queueEvent[OS_LYRG_EVENT_USER_E]) 
            {
               retval = SUCCESS_E;
            }
            else
            {
               retval = FAIL_E;
            }
         }
         else
         {
            if(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]) 
            {
               retval = SUCCESS_E;
            }
            else
            {
               retval = FAIL_E;
            }
         }

      }
    }
   else
   {
   retval = FAIL_E;
   }
    return retval;
}

static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   int retval = SUCCESS_E;

   if (msgQueP)
   {
      OS_LYRP_lockMutex(&(msgQueP->queueMutex));
      while (!msgQueP->queue.empty())
      {
         delete msgQueP->queue.front();
         msgQueP->queue.pop();

      };

      OS_LYRP_releaseEvent(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E], 0);
      msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]=NULL;
      OS_LYRP_releaseEvent(msgQueP->queueEvent[OS_LYRG_EVENT_USER_E], 0);
      msgQueP->queueEvent[OS_LYRG_EVENT_USER_E]=NULL;
      OS_LYRP_unlockMutex(&(msgQueP->queueMutex));
      OS_LYRP_releaseMutex(&(msgQueP->queueMutex));
      msgQueP->queueMutex = NULL;

      //assert(msgQueP->queue.empty());
   }
   else
   {
      retval = FAIL_E;
   }
   return retval;
}

static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len)
{
   int retval = SUCCESS_E;
   UINT8 * tempBuff;  
   if (msgQueP)
   {
      if(len <= msgQueP->msgSize)
      {
         OS_LYRP_lockMutex(&(msgQueP->queueMutex));
   
         //discard if exceeded max messages
         if(msgQueP->queue.size() >= msgQueP->maxMsgs)
         {
            LOGG_PRINT(LOG_DEBUG_E, NULL, "error: overflow\n");
            retval = FAIL_E; //overflow
         }
         else
         {
            tempBuff = new UINT8[msgQueP->msgSize]; 
            memcpy(tempBuff, msgP, len);

            //Insert into queuue and signal event needs to be atomic so
            //the receiver not dequeue before the signal and then receive an "empty" signal
            msgQueP->queue.push(tempBuff);
            OS_LYRP_setEvent(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]);
         }
         OS_LYRP_unlockMutex(&(msgQueP->queueMutex));
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "error: message too long\n");
         retval = FAIL_E; //message too long
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error: msgQueP is NULL\n");
      retval = FAIL_E;
   }
   return retval;
}

static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeouMsec)
{
   int retval = SUCCESS_E;
   int ret;
   UINT8 *head=NULL;

   if (msgQueP)
   {
      ret = OS_LYRP_waitMultipleEvents(OS_LYRG_MULTUPLE_EVENTS_NUM_E, msgQueP->queueEvent, timeouMsec);
      if(ret == SUCCESS_E)
      {
         //Recevied event
         OS_LYRP_lockMutex(&(msgQueP->queueMutex));
         
         if(!msgQueP->queue.empty())
         {
            if(*lenP >= msgQueP->msgSize)
            {               
               //Copy from queue to user - copying full message
               *lenP = msgQueP->msgSize;
               head = msgQueP->queue.front();
               memcpy(msgP, head, msgQueP->msgSize);
               msgQueP->queue.pop();
            }
            else
            {
               retval = FAIL_E; //user's buffer to small
            }

            //Handle bursts - set ourselves an event if there are still items in the queue
            if (!msgQueP->queue.empty())
            {
               OS_LYRP_setEvent(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]);
            }
         }
         else
         {
            //Received event with empty queue - should not happen.
            retval = FAIL_E;
            //assert(0);
         }

         //Delete queue item allocated in send
         if(head)
         {
            delete[] (head);
         }

         OS_LYRP_unlockMutex(&(msgQueP->queueMutex));
      }
      else
      {
         //Event timeout(or error)
         retval = FAIL_E;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "receive msg queue fail: NULL message queue\n");
      retval = FAIL_E;
   }

   return retval;
}


static ERRG_codeE OS_LYRP_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId)
{
   return OS_LYRP_setEvent(msgQueP->queueEvent[eventId]);
}

static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP)
{
   UINT32 msec = GetTickCount();
   *secP = msec/1000;     
   *msecP = (UINT16)(msec % 1000);

   return;
}

static void OS_LYRP_sleep(int timeuSec)
{
   if (timeuSec<1000)
   {
      timeuSec=1000;
   }
   Sleep(timeuSec/1000);
}

static VOID CALLBACK timer_sig_handler(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
{
   OS_LYRG_timerT *timerP = (OS_LYRG_timerT*)lpParameter;
   FIX_UNUSED_PARAM_WARN(TimerOrWaitFired);

   if (timerP->funcCb)
      timerP->funcCb(timerP->argP);
}

static ERRG_codeE OS_LYRP_createTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   UINT32 firstExpirymSec, intervalExpiryMsec;

   firstExpirymSec = 0xFFFFFFFF;
   intervalExpiryMsec = 0;

   if(CreateTimerQueueTimer(&timerP->win_timer, NULL, (WAITORTIMERCALLBACK)timer_sig_handler, timerP, firstExpirymSec, intervalExpiryMsec, WT_EXECUTEDEFAULT) == 0)
   {
     LOGG_PRINT(LOG_ERROR_E, NULL, "CreateTimerQueueTimer() error\n");
     ret = OS_LYR__ERR_UNEXPECTED;
   }

   LOGG_PRINT(LOG_DEBUG_E, NULL, "create timer: callBackId = %d, firstExpirymSec = %d, intervalExpiryMsec = %d argP=%p\n",timerP->callbackId,firstExpirymSec,intervalExpiryMsec,timerP->argP);
    return ret;
}

static ERRG_codeE OS_LYRP_setTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   UINT32 firstExpirymSec, intervalExpiryMsec;

   firstExpirymSec = (timerP->firstExpirySec * 1000) + (timerP->firstExpiryNsec / 1000000);
   intervalExpiryMsec = (timerP->intervalExpirySec * 1000) + (timerP->intervalExpiryNsec / 1000000);

   if(ChangeTimerQueueTimer(NULL, timerP->win_timer, firstExpirymSec, intervalExpiryMsec) == 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "ChangeTimerQueueTimer() error\n"); 
      ret = OS_LYR__ERR_UNEXPECTED;
   }

   LOGG_PRINT(LOG_DEBUG_E, NULL, "change timer: callBackId = %d, firstExpirymSec = %d, intervalExpiryMsec = %d argP=%p\n",timerP->callbackId,firstExpirymSec,intervalExpiryMsec,timerP->argP);

   return ret;
}


static ERRG_codeE OS_LYRP_deleteTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;

   DeleteTimerQueueTimer(NULL, timerP->win_timer, INVALID_HANDLE_VALUE);

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
   FIX_UNUSED_PARAM_WARN(id);
   return OS_LYRP_createEvent();
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
int OS_LYRG_waitMultipleEvents(UINT8 numOfEvents, OS_LYRG_event events[],UINT32 timeout)
{
   return OS_LYRP_waitMultipleEvents(numOfEvents, events,timeout);
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

void OS_LYRG_getUsecTime(UINT64 *usecP)
{   
   LARGE_INTEGER time; 
        if (!frequency.QuadPart)
         QueryPerformanceFrequency(&frequency);
   if (!QueryPerformanceCounter(&time))
      LOGG_PRINT(LOG_ERROR_E, NULL,  "QueryPerformanceFrequency failed\n");   
   *usecP = (UINT64)(time.QuadPart /(frequency.QuadPart/1000000));
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
   FIX_UNUSED_PARAM_WARN(msgQueP);
   return(0);
}

ERRG_codeE OS_LYRG_createTimer(OS_LYRG_timerT *timerP)
{   
   return OS_LYRP_createTimer(timerP);
}

ERRG_codeE OS_LYRG_setTimer(OS_LYRG_timerT *timerP)
{
   return OS_LYRP_setTimer(timerP);
}


ERRG_codeE OS_LYRG_deleteTimer(OS_LYRG_timerT *timerP)
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
#endif /* windows */


#ifdef __cplusplus
}
#endif
