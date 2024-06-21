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
#ifndef _OS_LYR_WIN_H_
#define _OS_LYR_WIN_H_

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include <queue>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#define OS_LYRG_INFINITE   (INFINITE)
using namespace std ;
typedef queue < UINT8* > OS_LYRG_queueT;
typedef HANDLE OS_LYRG_mutexT;

typedef struct timer
{
   /* Callback id */
   UINT32                   callbackId;
   OS_LYRG_timerCallbackFunctionT funcCb;
   void                     *argP;  
   HANDLE win_timer;

   /* Time until the first expiry of the timer */
   UINT16                   firstExpirySec;
   UINT32                   firstExpiryNsec;
   
   /* Interval time expiry  */
   UINT16                   intervalExpirySec;
   UINT32                   intervalExpiryNsec;
} OS_LYRG_timerT;   

typedef struct
{
   UINT32 maxMsgs;
   UINT32 msgSize;
   char name[OS_LYRG_MAX_MQUE_NAME_LEN];
   OS_LYRG_queueT queue;
   OS_LYRG_mutexT queueMutex;
   OS_LYRG_event  queueEvent[OS_LYRG_MULTUPLE_EVENTS_NUM_E];   
} OS_LYRG_msgQueT; 


#endif //_OS_LYR_WIN_H_


