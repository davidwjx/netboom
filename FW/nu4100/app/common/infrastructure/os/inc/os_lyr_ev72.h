/****************************************************************************
 *
 *   FileName: os_lyr_ev61.h
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: OS abstraction layer API 
 *   
 ****************************************************************************/
#ifndef _OS_LYR_EV61_H_
#define _OS_LYR_EV61_H_

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "que.h"
#include "evthreads.h"
#include "linked_list.h"


typedef void * OS_LYRG_mutexT;
typedef struct timer
{
   /* Time until the first expiry of the timer */
   UINT16                   firstExpirySec;
   UINT32                   firstExpiryNsec;
   
   /* Interval time expiry  */
   UINT16                   intervalExpirySec;
   UINT32                   intervalExpiryNsec;
} OS_LYRG_timerT;

typedef struct
{
   UINT16       maxMsgs;
   UINT32       msgSize;
   QUEG_queT    queue;
   EvMutexType  queueMutex;
   EvCondType   queueCondition;
   Node         *head, *curr;
   UINT32       counter;
} OS_LYRG_msgQueT;


long OS_LYRG_disableInterrupts();
void OS_LYRG_enableInterrupts(unsigned long flags);


#endif //_OS_LYR_EV61_H_



