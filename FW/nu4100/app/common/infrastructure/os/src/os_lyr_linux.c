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


#if DEFSG_IS_HOST_LINUX_DESKTOP


#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define TIMERSIGNALID SIGUSR1
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


static OS_LYRG_event OS_LYRP_createEvent();
static int OS_LYRP_waitEvent(OS_LYRG_event event, UINT32 timeoutMsec);
//static int OS_LYRP_waitMultipleEvents(UINT8 numOfEvents, OS_LYRG_event _events[], UINT32 timeoutMsec);
static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event event);
static int OS_LYRP_releaseEvent(OS_LYRG_event event);

static void OS_LYRP_sleep(int timeuSec);
static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP);
static int OS_LYRP_setTime(INT32 sec, INT32 usec);

static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents);
static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP, UINT32 timeouMsec);
static ERRG_codeE OS_LYRP_setEventMsgQueue();



/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/



/*******************************************__linux__**********************************/



static int OS_LYRP_init(void)
{
   return SUCCESS_E;
}
#if 0
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
#endif
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
   int ret;
//   struct sched_param scParams;
   assert(sizeof(OS_LYRG_threadHandle) >= sizeof(pthread_t));
   do
   {
      OS_LYRP_pthreadArgsT *thrdArgs = malloc(sizeof(OS_LYRP_pthreadArgsT));

      thrdArgs->arg      = paramsP->param;
      thrdArgs->func     = (PTHREAD_ROUTINE)paramsP->func;
      thrdArgs->name     = OS_LYRP_pthreadParamsTbl[paramsP->id].name;
      thrdArgs->priority = OS_LYRP_pthreadParamsTbl[paramsP->id].priority;

      //Create the thread 
      ret = pthread_create((pthread_t *)&thrd, NULL, (PTHREAD_ROUTINE)OS_LYRP_threadWrapper, thrdArgs);
      if(ret) 
      {
         LOGG_PRINT(LOG_ERROR_E,0,"pthread_create() failed\n");
         break;
      }
   }while(0);

   if(ret)
      LOGG_PRINT(LOG_ERROR_E,0, "error %d:%s\n",ret, strerror(ret));
   //Attributes should be destroyed after use
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
   pthread_mutex_lock(mutexP);
   return SUCCESS_E;
}

static int OS_LYRP_unlockMutex(OS_LYRG_mutexT *mutexP)
{
   pthread_mutex_unlock(mutexP);
   return SUCCESS_E;
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

struct event_flag
{
    OS_LYRG_mutexT  mutex;
};

static OS_LYRG_event OS_LYRP_createEvent()
{
   struct event_flag* ev;
   ev = (struct event_flag*) malloc(sizeof(struct event_flag));

   OS_LYRP_aquireMutex(&ev->mutex);
   OS_LYRP_lockMutex(&ev->mutex);
    return (OS_LYRG_event)ev;
}


/*
 * 0 - sucess- event received
 * 1 - fail - timeout without event
 */
static int OS_LYRP_waitEvent(OS_LYRG_event event, UINT32 timeoutMsec)
{
   int      ret = SUCCESS_E;
   struct   timespec ts; //for mutex_timedwait
   struct   timeval tv; //for gettimeoday
   struct event_flag* ev = (struct event_flag*) event;

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

      ret = pthread_mutex_timedlock(&ev->mutex, &ts);

      if(ret)
      {
         if(ret != ETIMEDOUT)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL,  "pthread_mutex_timedlock failed %d\n", ret);
         }
         ret = FAIL_E;//timeout or error
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "gettimeofday failed\n");
      ret = FAIL_E;
   }

   return ret;
}

static ERRG_codeE OS_LYRP_setEvent(OS_LYRG_event event)
{
   struct event_flag* ev = (struct event_flag*) event;

   int ret;
   ret = OS_LYRP_unlockMutex(&ev->mutex);
   if(ret < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "OS_LYRG_setEvent: pthread_mutex_unlock failed %s (ret = %d)\n", strerror(errno), ret);
   }
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_releaseEvent(OS_LYRG_event event)
{
   struct event_flag* ev = (struct event_flag*) event;
   pthread_mutex_destroy(&ev->mutex);
   free(event);
   return SUCCESS_E;
}

static void OS_LYRP_sleep(int timeuSec)
{
   usleep(timeuSec);
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
   struct timeval tv;

   tv.tv_sec  = sec;
   tv.tv_usec = usec;

   return settimeofday(&tv, NULL);
}

static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents)
{
   int retval = SUCCESS_E;
   int   i            = 0;
   msgQueP->head         = (Node *)malloc(sizeof(Node));
   msgQueP->head->val    = (UINT8 *)malloc(msgQueP->msgSize);
   msgQueP->curr         = msgQueP->head;
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
               inu_init_queue(&msgQueP->queue);
               //creating a linked list of free addresses in the length of maxMsgs
               for(i=0;i<msgQueP->maxMsgs;i++)
               {
                  addNode(msgQueP->curr, msgQueP->msgSize);
                  msgQueP->curr=msgQueP->curr->next;
               }
               msgQueP->curr->next=msgQueP->head;
               msgQueP->curr=msgQueP->head;
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
   void *tmpVal;
   UINT32 i=0;

   if (msgQueP)
   {
      OS_LYRP_lockMutex(&(msgQueP->queueMutex));
      while (!inu_is_empty(&msgQueP->queue))
      {
         tmpVal = inu_dequeue(&msgQueP->queue);
         free(tmpVal);
      };

      //deleting a linked list in the length of maxMsgs
      for(i=0;i<msgQueP->maxMsgs;i++)
      {
         deleteNode(msgQueP->head);
      }
      OS_LYRP_releaseEvent(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]);
      msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]=NULL;
      OS_LYRP_releaseEvent(msgQueP->queueEvent[OS_LYRG_EVENT_USER_E]);
      msgQueP->queueEvent[OS_LYRG_EVENT_USER_E]=NULL;
      OS_LYRP_unlockMutex(&(msgQueP->queueMutex));
      OS_LYRP_releaseMutex(&(msgQueP->queueMutex));
      //msgQueP->queueMutex = NULL;

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
   if (msgQueP)
   {
      if(len <= msgQueP->msgSize)
      {
         OS_LYRP_lockMutex(&(msgQueP->queueMutex));
   
         //discard if exceeded max messages
         if(msgQueP->queue.count >= msgQueP->maxMsgs)
         {
            retval = FAIL_E; //overflow
         }
         else
        {  //copy the message into the node, inserting the message into the queue, pointing the "current" node to the next node
            memcpy(msgQueP->curr->val, msgP, len);
            //Insert into queue and signal event needs to be atomic so
            //the receiver not dequeue before the signal and then receive an "empty" signal            
            inu_enqueue(&msgQueP->queue,msgQueP->curr->val);
            //Pointing into the next empty node
            msgQueP->curr=msgQueP->curr->next;
            OS_LYRP_setEvent(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E]);
         }
         OS_LYRP_unlockMutex(&(msgQueP->queueMutex));
      }
      else
      {
         retval = FAIL_E; //message too long
      }
   }
   else
   {
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
      ret = OS_LYRG_waitEvent(msgQueP->queueEvent[OS_LYRG_EVENT_MSG_RCV_E], timeouMsec);
      if(ret == SUCCESS_E)
      {
         //Recevied event
         OS_LYRP_lockMutex(&(msgQueP->queueMutex));
         
         if(!inu_is_empty(&msgQueP->queue))
         {
            if(*lenP >= msgQueP->msgSize)
            {               
               //Copy from queue to user - copying full message
               head = (UINT8 *) inu_dequeue(&msgQueP->queue);
               memcpy(msgP, head, msgQueP->msgSize);
            }
            else
            {
               retval = FAIL_E; //user's buffer to small
            }

            //Handle bursts - set ourselves an event if there are still items in the queue
            if (!inu_is_empty(&msgQueP->queue))
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

         //Receiving message - Pointing head to the next node
         if(head)
         {
            msgQueP->head=msgQueP->head->next;
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
      retval = FAIL_E;
   }

   return retval;
}

static ERRG_codeE OS_LYRP_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId)
{
   return OS_LYRP_setEvent(msgQueP->queueEvent[eventId]);
}

static unsigned int OS_LYRP_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP)
{
   int ret;
   OS_LYRP_lockMutex(&(msgQueP->queueMutex));
   ret = msgQueP->queue.count;
   OS_LYRP_unlockMutex(&(msgQueP->queueMutex));

   return ret;
}


void OS_LYRP_sigExpiryHandler (int sig, siginfo_t *siginfo, void *context)
{
   OS_LYRG_timerT *timerP;

   FIX_UNUSED_PARAM_WARN(context);

   if (siginfo->si_value.sival_ptr)
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL , "received signal, info->si_value = %d, sigNum = %d\n",siginfo->si_value.sival_int,sig);
      timerP = (OS_LYRG_timerT*)siginfo->si_value.sival_ptr;
      timerP->funcCb(timerP->argP);
   }
   else
   {
      LOGG_PRINT(LOG_WARN_E, NULL , "WARNING: received signal but CB is null!, info->si_value = %d, sigNum = %d\n",siginfo->si_value.sival_int,sig);
   }   
}


static ERRG_codeE OS_LYRP_createTimer( OS_LYRG_timerT *timerP )
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   struct sigevent sevp;         
   struct sigaction sa;
   sigset_t mask;

   /* Establish handler for timer signal */
   sa.sa_flags = SA_SIGINFO;
   sa.sa_sigaction = OS_LYRP_sigExpiryHandler;
   sigemptyset(&sa.sa_mask);
   if (sigaction(TIMERSIGNALID, &sa, NULL) == -1)
      return OS_LYR__ERR_COMMAND_FAIL;
   
   /* Block timer signal temporarily */   
   sigemptyset(&mask);
   sigaddset(&mask, TIMERSIGNALID);
   if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
      return OS_LYR__ERR_COMMAND_FAIL;

   if(ERRG_SUCCEEDED(ret))
   {
      sevp.sigev_notify          = SIGEV_SIGNAL;
      sevp.sigev_signo           = TIMERSIGNALID;
      sevp.sigev_value.sival_ptr = timerP;

      if (timer_create(CLOCK_REALTIME , &sevp, &(timerP->timerid)) != 0)
      {
        LOGG_PRINT(LOG_ERROR_E, NULL, "timer_create failed callbackId=%d errno = %s\n",timerP->callbackId, strerror(errno));
        ret = OS_LYR__ERR_COMMAND_FAIL;
      }
      else
      {
        LOGG_PRINT(LOG_DEBUG_E, NULL, "timer_create success: callbackId=%d, timerId=%d\n",timerP->callbackId,timerP->timerid);   
      }   
   }

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
       LOGG_PRINT(LOG_ERROR_E, NULL, "timer_settime failed: timerid=%d, sec = %d, nsec = %d, int_sec = %d, int_nsec = %d, errno = %s \n",
                  timerP->timerid, 
                  newValue.it_value.tv_sec, 
                  newValue.it_value.tv_nsec, 
                  newValue.it_interval.tv_sec, 
                  newValue.it_interval.tv_nsec, 
                  strerror(errno));
       
       ret = OS_LYR__ERR_COMMAND_FAIL;
   }
   else
   {
       LOGG_PRINT(LOG_DEBUG_E, NULL, "timer_settime success: timerid=%d, sec = %d, nsec = %d, int_sec = %d, int_nsec = %d\n",
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

   if (timer_delete(timerP->timerid) != 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "timer_delete failed: timerid=%d, errno = %s \n", timerP->timerid, strerror(errno));   
      ret = OS_LYR__ERR_COMMAND_FAIL;
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


OS_LYRG_event OS_LYRG_createEvent(UINT32 id)
{
   return OS_LYRP_createEvent();
}

int OS_LYRG_releaseEvent(OS_LYRG_event event, UINT32 id)
{
   return OS_LYRP_releaseEvent(event);
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
   OS_LYRP_getTime(secP, msecP);
}

ERRG_codeE OS_LYRG_setTime(INT32 sec, INT32 usec)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;

   if (OS_LYRP_setTime(sec, usec) == -1)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "settimeofday failed, sec = %ld, usec = %ld\n",sec,usec);
      ret = OS_LYR__ERR_COMMAND_FAIL;
   }   

   return ret;
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
   struct timeval time;
    if (gettimeofday(&time, NULL) == -1)
      {
            LOGG_PRINT(LOG_ERROR_E, NULL,  "gettimeofday failed\n");
      }   
   *usecP = (time.tv_sec*1000000) + time.tv_usec;
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
   return OS_LYRP_getCurrNumMsg(msgQueP);
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
#endif

#ifdef __cplusplus
}
#endif
