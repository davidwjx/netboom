/****************************************************************************
 *
 *   FileName: os_lyr_gp.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: OS abstraction layer API 
 *   
 ****************************************************************************/
#ifndef _OS_LYR_LINUX_H_
#define _OS_LYR_LINUX_H_

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include <mqueue.h>
#include <pthread.h>
#include <signal.h>
#include "inu_queue.h"
#include "linked_list.h"
#if (DEFSG_IS_GP || DEFSG_GP_HOST)
#include "mem_map.h"
#endif

typedef inu_queue OS_LYRG_queueT;
typedef pthread_mutex_t OS_LYRG_mutexT;

#define OS_LYRG_INFINITE   (0xFFFFFFFF)
typedef struct timer
{
   timer_t                  timerid;
   /* Callback id */
   UINT32                   callbackId;
   OS_LYRG_timerCallbackFunctionT funcCb;
   void                     *argP;
	
   /* Time until the first expiry of the timer */
   UINT16                   firstExpirySec;
   UINT32                   firstExpiryNsec;
   
   /* Interval time expiry  */
   UINT16                   intervalExpirySec;
   UINT32                   intervalExpiryNsec;
} OS_LYRG_timerT;
typedef struct
{
   UINT16 maxMsgs;
   UINT32 msgSize;
   char name[OS_LYRG_MAX_MQUE_NAME_LEN];
   OS_LYRG_queueT queue;
   OS_LYRG_mutexT queueMutex;
   OS_LYRG_event  queueEvent[OS_LYRG_MULTUPLE_EVENTS_NUM_E];
   Node           *head, *curr;
} OS_LYRG_msgQueT;
#define TIMERSIGNALID SIGUSR1


#endif //_OS_LYR_LINUX_H_


