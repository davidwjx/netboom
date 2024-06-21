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
#if DEFSG_IS_EV72


#include "err_defs.h"
#include "log.h"
#include "que.h"
#include "evthreads.h"
#include "os_lyr.h"
#include <stdlib.h>
#include <stdio.h>
#include "mem_map.h"
//	#include <arc/arc_reg.h>
#include <arc/arc_intrinsics.h>


#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define Asm   __asm__ volatile      /* inline asm (no optimization) */ 

/* status32 Reg bits related to Interrupt Handling */
#define STATUS_E1_BIT      1   /* Int 1 enable */
#define STATUS_E2_BIT      2   /* Int 2 enable */
#define STATUS_E1_MASK      (1<<STATUS_E1_BIT)
#define STATUS_E2_MASK      (1<<STATUS_E2_BIT)

#define EVTH_DSYNC              __asm__ __volatile__ ("dsync " : : : "memory")
#define EVTH_WRITE_MEMORY_FENCE __asm__ __volatile__ ("dmb 2 " : : : "memory")
#define EVTH_MEMORY_FENCE       __asm__ __volatile__ ("dmb 3 " : : : "memory")

#define EVTH_SMP_FENCE EVTH_MEMORY_FENCE

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/


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
#endif

static int OS_LYRP_mutexInitCp();
static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum);
static int OS_LYRP_releaseMutexCp(OS_LYRG_mutexCpT *cpMutexP);
static int OS_LYRP_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
static int OS_LYRP_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle);
static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP);
static int OS_LYRP_sendMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 len);
static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP);


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


unsigned int OS_LYRP_getCurrNumMsg(OS_LYRG_msgQueT *msgQueP)
{
   return 0;
}


static ERRG_codeE OS_LYRP_setEventMsgQueue(OS_LYRG_msgQueT *msgQueP, OS_LYRG_multipleEventsE eventId)
{
   return OS_LYR__RET_SUCCESS;
}

#endif
static int OS_LYRP_createMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   int retval = SUCCESS_E;
   int   i            = 0;
   msgQueP->head         = (Node *)malloc(sizeof(Node));
   msgQueP->head->val    = (UINT8 *)malloc(msgQueP->msgSize);
   msgQueP->curr         = msgQueP->head;
	msgQueP->counter      = 0;
   if (msgQueP)
   {
      retval = evMutexInit(&(msgQueP->queueMutex));    
      if (retval == EVTH_ERROR_OK)
      {
         evCondInit(&(msgQueP->queueCondition));
         msgQueP->queue.maxCnt = QUEG_MAX_ELEMENTS;
         QUEG_init(&(msgQueP->queue));
         //creating a linked list of free addresses in the length of maxMsgs
         for(i=0;i<msgQueP->maxMsgs;i++)
         {
            addNode(msgQueP->curr, msgQueP->msgSize);
            msgQueP->curr=msgQueP->curr->next;
         }
         msgQueP->curr->next=msgQueP->head;
         msgQueP->curr=msgQueP->head;
         retval = EVTH_ERROR_OK;
      }
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, 0, "Failed to create msgQ \n");
        retval = FAIL_E;
    }
    return retval;
}

static int OS_LYRP_deleteMsgQue(OS_LYRG_msgQueT *msgQueP)
{
   int retval   = SUCCESS_E;
   void *tmpVal = NULL;
   UINT32 i     = 0;

   if (msgQueP)
   {
    
      evMutexLock(&(msgQueP->queueMutex));
      while (!QUEG_numElements(&msgQueP->queue))
      {
          
        QUEG_deque(&msgQueP->queue,(QUEG_elemT*)tmpVal);
        free(tmpVal);
      }

      //deleting a linked list in the length of maxMsgs
      for(i=0;i<msgQueP->maxMsgs;i++)
      {
         deleteNode(msgQueP->head);
      }
      evCondDestroy(&(msgQueP->queueCondition));
      evMutexUnLock(&(msgQueP->queueMutex));
      evMutexDestroy(&(msgQueP->queueMutex));

   }
   else
   {
    
      LOGG_PRINT(LOG_ERROR_E, 0, "Failed to delete msgQ \n");
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
          evMutexLock(&(msgQueP->queueMutex));
         //discard if exceeded max messages
         if(QUEG_numElements(&msgQueP->queue) >= msgQueP->maxMsgs)
         {
            LOGG_PRINT(LOG_ERROR_E, 0, "Failed to send msg, num of Q elements too high \n");
            retval = FAIL_E; //overflow
         }
         else
         {  //copy the message into the node, inserting the message into the queue, pointing the "current" node to the next node
            memcpy(msgQueP->curr->val, msgP, len);
            //Insert into queue and signal event needs to be atomic so
            //the receiver not dequeue before the signal and then receive an "empty" signal            
            QUEG_enque(&msgQueP->queue,(QUEG_elemT)msgQueP->curr->val);
            //Pointing into the next empty node
            msgQueP->curr   =   msgQueP->curr->next;
            
            //Trigger the processing thread
            msgQueP->counter++;
            evCondSignal(&(msgQueP->queueCondition));    
          }
          evMutexUnLock(&(msgQueP->queueMutex));
          
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



static int OS_LYRP_recvMsg(OS_LYRG_msgQueT *msgQueP, UINT8 *msgP, UINT32 *lenP)
{
   int retval   = SUCCESS_E;
   UINT32 head = 0;

   if (msgQueP)
   {
		 LOGG_PRINT(LOG_DEBUG_E, 0, "rec msg counter=%d queElem=%d RTC=%llu \n", msgQueP->counter, QUEG_numElements(&msgQueP->queue), getRTC());
      //Wait for signaling
      evMutexLock(&(msgQueP->queueMutex));
      while (msgQueP->counter == 0)
      {
          evCondWait(&(msgQueP->queueCondition), &(msgQueP->queueMutex));
      }
      msgQueP->counter--;

      //Recevied event
      if(QUEG_numElements(&msgQueP->queue))
      {
        
//	        LOGG_PRINT(LOG_INFO_E, 0, "QUEG_numElements=%d  \n", QUEG_numElements(&msgQueP->queue));
         if(*lenP >= msgQueP->msgSize)
         {               
            //Copy from queue to user - copying full message
            QUEG_deque(&msgQueP->queue,(QUEG_elemT*)&head);
            memcpy(msgP, (void *)head, msgQueP->msgSize);
            //Receiving message - Pointing head to the next node
            if(head)
            {
               msgQueP->head  =   msgQueP->head->next;
            }
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, 0, "Wrong msg len=%d  \n", *lenP);
            retval = FAIL_E; //user's buffer to small
         }
      }
      else
      {
         //Received event with empty queue - should not happen.
         LOGG_PRINT(LOG_ERROR_E, 0, "Recieved msg with empry que  \n");
         retval = FAIL_E;
      }
      evMutexUnLock(&(msgQueP->queueMutex));
   }
   else
   {
      retval = FAIL_E;
   }
//	   LOGG_PRINT(LOG_INFO_E, 0, "end OS_LYRP_recvMsg  \n");

   return retval;
}




static int OS_LYRP_mutexInitCp()
{
   return OS_LYR__RET_SUCCESS;
}

static int OS_LYRP_aquireMutexCp(OS_LYRG_mutexCpT **cpMutexhandleP, OS_LYRG_cpMutexIdE mutexNum)
{
   OS_LYRG_mutexCpT* cpMutexP;
   OS_LYRG_mutexCpT* cpMutexPhyBaseP;

   /* check mutex num legality */
   if ((UINT32)mutexNum >= OS_LYRG_NUM_CP_MUTEX_E) 
   {
      LOGG_PRINT(LOG_ERROR_E, 0, "MUTEX Index error: mutexNum = %d \n", (UINT32)mutexNum);
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

/**
* Lock the address; make sure to use evEnterCriticalSection!
* \ingroup group_snps_evthreads_internal
* \param[in] addr address to lock
*/
static inline int evLoadLocked(void *ptr) 
{
    return _llock((int *) ptr);
}

/**
* Store if memory at address not changed; make sure to use evLeaveCriticalSection!
* \ingroup group_snps_evthreads_internal
* \param[in] val value to store
* \param[in] addr address to store to
*/
static inline int evStoreCond(int val, void *ptr) 
{
    return _scond(val, (int *) ptr);
}

/**
* Use this function for a short-lived lock.
*/
//static inline void lockSetArch(volatile int &lock) {
static inline int OS_LYRP_lockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{
    // - Memory barrier is not required as "_llock" ensures exclusive access
    // - Don't need to protect critical section, as this function is already
   //   called with interrupts disabled

   do 
	{
        int val = evLoadLocked((void *)&(cpMutexHandle->mutex));

        if (val == 0)
		  {
            if (evStoreCond(1, ((void *)&(cpMutexHandle->mutex))) == 0) 
				{
                break;
            }
        } 
    } while (1);

    // make sure out-of-order writes are done, but more importantly:
    // ensure that we don't access the shared resource before acquiring
    // the lock.
    EVTH_SMP_FENCE;
	 
	 return SUCCESS_E;
}

/**
* Clear the short-lived lock.
*/
//static inline void lockClrArch(volatile int &lock) {
static inline int OS_LYRP_unlockMutexCp(OS_LYRG_mutexCpT *cpMutexHandle)
{

    UINT32 *lock;
    // Ensure accesses to protected resource have completed
    EVTH_SMP_FENCE;

    lock = (UINT32 *)&(cpMutexHandle->mutex);
    //EV_ASSERT(lock == 1);
    *lock = 0;
	 
	 return SUCCESS_E;
}


static void OS_LYRP_sleep(int timeuSec)
{

}

static void OS_LYRP_getTime(UINT32 *secP, UINT16 *msecP)
{

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
#if 0
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
#endif
void OS_LYRG_enableInterrupts(unsigned long flags)
{
/*   Asm(   "   flag %0         \n"   
         :   
         : "r"(flags)   
         : "memory"); */

//	   _seti(1<<4 | 0xf);
    evIntsRestore(flags);

}

long OS_LYRG_disableInterrupts()
{
 //  unsigned long temp, flags;   
/*   Asm(   "   lr  %1, [status32]   \n"   
         "   bic %0, %1, %2      \n"   
         "   and.f 0, %1, %2   \n"   
         "   flag.nz %0      \n"   
         : "=r"(temp), "=r"(flags)   
         : "n"((STATUS_E1_MASK | STATUS_E2_MASK))   
         : "memory", "cc");
*/
//	   _seti(0);


//	   return flags;
   return evIntsOff();
}
int OS_LYRG_createMsgQue(OS_LYRG_msgQueT *msgQueP, UINT32 numOfEvents)
{
   (void)(numOfEvents); //avoidng warning
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
   (void)(timeoutMsec); //avoidng warning
   return  OS_LYRP_recvMsg(msgQueP, msgP, lenP);
}



#ifdef __cplusplus
}
#endif


#endif
