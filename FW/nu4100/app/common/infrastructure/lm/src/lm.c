#include "assert.h"

#include "inu_common.h"
#include "internal_cmd.h"
#include "inu_lm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_ref.h"


#define LINK_MONITORP_LINK_MAX_NUM_RETRY (2)

typedef enum
{
   LINK_MONITORP_LINK_STATE_INIT_E = 0,
   LINK_MONITORP_LINK_STATE_DISCONNECTED_E,
   LINK_MONITORP_LINK_STATE_CONNECTED_E,
   LINK_MONITORP_LINK_STATE_DEAD_LINK_E,
} LINK_MONITORP_linkStateE;

typedef enum
{
   LINK_MONITORP_LINK_EVENT_START_E = 0,
   LINK_MONITORP_LINK_EVENT_TIMER_EXPIRED_E,
   LINK_MONITORP_LINK_EVENT_RX_E,
   LINK_MONITORP_LINK_EVENT_STOP_E,
} LINK_MONITORP_linkEventE;

typedef struct
{
   OS_LYRG_mutexT             linkStateMutex;
   LINK_MONITORP_linkStateE   linkState;
   OS_LYRG_timerT             linkTimer;
   BOOL                       linkMonitor;
   int(*linkEventCallBack)(int, int, void*);
   void                      *linkEventCBparam;
   UINT32                     linkRetry;
   inu_ref                   *ref;
} LINK_MONITORP_linkT;

static ERRG_codeE LINK_MONITORP_invoke(LINK_MONITORP_linkT *paramsP, LINK_MONITORP_linkEventE event);
static ERRG_codeE LINK_MONITORP_linkTimerCreate(LINK_MONITORP_linkT *paramsP);
void LINK_MONITORP_linkTimerStart(LINK_MONITORP_linkT *paramsP);
void LINK_MONITORP_linkTimerStop(LINK_MONITORP_linkT *paramsP);

char *LINK_MONITORP_getStateName(LINK_MONITORP_linkStateE state)
{
   switch (state)
   {
   case(LINK_MONITORP_LINK_STATE_INIT_E):
   {
      return "INIT";
   }
   case(LINK_MONITORP_LINK_STATE_DISCONNECTED_E):
   {
      return "DISCONNECT";
   }
   case(LINK_MONITORP_LINK_STATE_CONNECTED_E):
   {
      return "CONNECT";
   }
   case(LINK_MONITORP_LINK_STATE_DEAD_LINK_E):
   {
      return "DEAD_LINK";
   }
   }

   return "";
}

char *LINK_MONITORP_getEventName(LINK_MONITORP_linkEventE event)
{
   switch (event)
   {
   case(LINK_MONITORP_LINK_EVENT_START_E):
   {
      return "START";
   }
   case(LINK_MONITORP_LINK_EVENT_TIMER_EXPIRED_E):
   {
      return "TIMER EXPIRED";
   }
   case(LINK_MONITORP_LINK_EVENT_RX_E):
   {
      return "RX";
   }
   case(LINK_MONITORP_LINK_EVENT_STOP_E):
   {
      return "STOP";
   }
   }

   return "";
}

static void LINK_MONITORP_sendPing2Peer(LINK_MONITORP_linkT *paramsP)
{
   INTERNAL_CMDG_pingT ping;
   OS_LYRG_getUsecTime(&ping.timestamp);
#ifndef __UART_ON_FPGA__   
   inu_ref__copyAndSendDataAsync(paramsP->ref, INTERNAL_CMDG_PING_E, &ping, NULL, 0);
#endif
}

/****************************************************************************
*
*  Function Name: LINK_MONITORP_invoke
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
static ERRG_codeE LINK_MONITORP_invoke(LINK_MONITORP_linkT *paramsP, LINK_MONITORP_linkEventE event)
{
   ERRG_codeE  ret = INU_LM__RET_SUCCESS;
   LINK_MONITORP_linkStateE oldState = paramsP->linkState;

   OS_LYRG_lockMutex(&paramsP->linkStateMutex);

   switch (paramsP->linkState)
   {
   case(LINK_MONITORP_LINK_STATE_INIT_E):
   {
      switch (event)
      {
      case(LINK_MONITORP_LINK_EVENT_START_E):
      {
         LINK_MONITORP_linkTimerStart(paramsP);
         LINK_MONITORP_sendPing2Peer(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_DISCONNECTED_E;
      }break;

      default:
      {
         //LOGG_PRINT(LOG_ERROR_E, NULL, "Link state machine: service %s event %d, state %d\n",privDataP->methodsP->getName(),event,paramsP->linkState);
         ret = INU_LM__ERR_ILLEGAL_STATE;
      }break;
      }
   }break;

   case(LINK_MONITORP_LINK_STATE_DISCONNECTED_E):
   {
      switch (event)
      {
      case(LINK_MONITORP_LINK_EVENT_TIMER_EXPIRED_E):
      {
         LINK_MONITORP_linkTimerStart(paramsP);
         LINK_MONITORP_sendPing2Peer(paramsP);
      }break;
      case(LINK_MONITORP_LINK_EVENT_RX_E):
      {
         LINK_MONITORP_linkTimerStart(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_CONNECTED_E;
         LOGG_PRINT(LOG_INFO_E, NULL, "Device Connected!\n");
         if (paramsP->linkEventCallBack)
            paramsP->linkEventCallBack(0, INUG_SERVICE_LINK_CONNECT_E, paramsP->linkEventCBparam);
      }break;
      case(LINK_MONITORP_LINK_EVENT_STOP_E):
      {
         LINK_MONITORP_linkTimerStop(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_INIT_E;
      }
      break;
      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Link state machine: event %s, oldState %s, state %s\n", LINK_MONITORP_getEventName(event), LINK_MONITORP_getStateName(oldState) ,LINK_MONITORP_getStateName(paramsP->linkState));
         ret = INU_LM__ERR_ILLEGAL_STATE;
      }break;
      }
   }break;

   case(LINK_MONITORP_LINK_STATE_CONNECTED_E):
   {
      switch (event)
      {
      case(LINK_MONITORP_LINK_EVENT_TIMER_EXPIRED_E):
      {
         LINK_MONITORP_linkTimerStart(paramsP);
         LINK_MONITORP_sendPing2Peer(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_DEAD_LINK_E;
      }break;
      case(LINK_MONITORP_LINK_EVENT_RX_E):
      {
         LINK_MONITORP_linkTimerStart(paramsP);
      }break;
      case(LINK_MONITORP_LINK_EVENT_STOP_E):
      {
         LINK_MONITORP_linkTimerStop(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_INIT_E;
         LOGG_PRINT(LOG_INFO_E, NULL, "device disconnected\n");
         if (paramsP->linkEventCallBack)
            paramsP->linkEventCallBack(0/*privDataP->sid*/, INUG_SERVICE_LINK_DISCONNECT_E, paramsP->linkEventCBparam);
      }
      break;
      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Link state machine: event %s, state %s\n", LINK_MONITORP_getEventName(event), LINK_MONITORP_getStateName(paramsP->linkState));
         ret = INU_LM__ERR_ILLEGAL_STATE;
      }break;
      }
   }break;

   case(LINK_MONITORP_LINK_STATE_DEAD_LINK_E):
   {
      switch (event)
      {
      case(LINK_MONITORP_LINK_EVENT_TIMER_EXPIRED_E):
      {
         if (paramsP->linkRetry < LINK_MONITORP_LINK_MAX_NUM_RETRY)
         {
            LINK_MONITORP_linkTimerStart(paramsP);
            LINK_MONITORP_sendPing2Peer(paramsP);
            paramsP->linkRetry++;
         }
         else
         {
            paramsP->linkRetry = 0;
            paramsP->linkState = LINK_MONITORP_LINK_STATE_DISCONNECTED_E;
            LOGG_PRINT(LOG_INFO_E, NULL, "device disconnected\n");
            if (paramsP->linkEventCallBack)
               paramsP->linkEventCallBack(0 /*privDataP->sid*/, INUG_SERVICE_LINK_DISCONNECT_E, paramsP->linkEventCBparam);
         }
      }break;
      case(LINK_MONITORP_LINK_EVENT_RX_E):
      {
         paramsP->linkRetry = 0;
         LINK_MONITORP_linkTimerStart(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_CONNECTED_E;
      }break;
      case(LINK_MONITORP_LINK_EVENT_STOP_E):
      {
         LINK_MONITORP_linkTimerStop(paramsP);
         paramsP->linkState = LINK_MONITORP_LINK_STATE_INIT_E;
         paramsP->linkRetry = 0;
         LOGG_PRINT(LOG_INFO_E, NULL, "Disconnected!\n");
         if (paramsP->linkEventCallBack)
            paramsP->linkEventCallBack(0/*privDataP->sid*/, INUG_SERVICE_LINK_DISCONNECT_E, paramsP->linkEventCBparam);
      }
      break;
      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Link state machine: event %s, state %s\n", LINK_MONITORP_getEventName(event), LINK_MONITORP_getStateName(paramsP->linkState));
         ret = INU_LM__ERR_ILLEGAL_STATE;
      }break;
      }
   }break;

   default:
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "unknown state! service %s state %s\n", LINK_MONITORP_getEventName(event), LINK_MONITORP_getStateName(paramsP->linkState));
      ret = INU_LM__ERR_ILLEGAL_STATE;
   }
   }

   if (ERRG_SUCCEEDED(ret))
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "service %s,current state = %s, event = %s, old state = %s\n", LINK_MONITORP_getStateName(paramsP->linkState), LINK_MONITORP_getEventName(event), LINK_MONITORP_getStateName(oldState));
   }
   else
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "current state = %s, event = %s, old state = %s\n", LINK_MONITORP_getStateName(paramsP->linkState), LINK_MONITORP_getEventName(event), LINK_MONITORP_getStateName(oldState));
   }

   OS_LYRG_unlockMutex(&paramsP->linkStateMutex);

   return ret;
}


/****************************************************************************
*
*  Function Name: LINK_MONITORP_linkTimerExpireCallback
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
void LINK_MONITORP_linkTimerExpireCallback(void *argP)
{
   LINK_MONITORP_linkT *paramsP;

   //LOGG_PRINT(LOG_DEBUG_E, NULL, "timer expired. argP = %p\n", argP);
   //printf("timer expired. argP = %p\n", argP);

   if (argP)
   {
      paramsP = (LINK_MONITORP_linkT*)argP;
      LINK_MONITORP_invoke(paramsP, LINK_MONITORP_LINK_EVENT_TIMER_EXPIRED_E);
   }
}


/****************************************************************************
*
*  Function Name: LINK_MONITORP_linkTimerStart
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
void LINK_MONITORP_linkTimerStart(   LINK_MONITORP_linkT *paramsP)
{
   ERRG_codeE ret;

   //Set timer
   paramsP->linkTimer.firstExpiryNsec = 0; /*This has had to be increased due to it not being enough time with metadata, sensorsync updates and sensor updates added*/
   paramsP->linkTimer.firstExpirySec = 15;
   paramsP->linkTimer.intervalExpirySec = 0xFFFF;
   paramsP->linkTimer.intervalExpiryNsec = 0;
   ret = OS_LYRG_setTimer(&paramsP->linkTimer);
   if (ERRG_FAILED(ret))
   {
      //LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to set link timer\n");
      printf("Failed to set link timer\n");
   }
   else
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "Set link timer %dusec\n", privDataP->linkParams.linkTimer.firstExpiryNsec / 1000);
   }
}


/****************************************************************************
*
*  Function Name: LINK_MONITORP_linkTimerStop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
void LINK_MONITORP_linkTimerStop( LINK_MONITORP_linkT *paramsP )
{
   ERRG_codeE ret;

   //Set timer
   paramsP->linkTimer.firstExpiryNsec = 0;
   paramsP->linkTimer.firstExpirySec = 0;
   paramsP->linkTimer.intervalExpirySec = 0xFFFF;
   paramsP->linkTimer.intervalExpiryNsec = 0;
   ret = OS_LYRG_setTimer(&paramsP->linkTimer);
   if (ERRG_FAILED(ret))
   {
      //LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to set link timer\n");
      printf("Failed to set link timer\n");
   }
   else
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "Timer stop!\n");
      //printf("Timer stop!\n");
   }
}

/****************************************************************************
*
*  Function Name: LINK_MONITORP_linkTimerCreate
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Service manager
*
****************************************************************************/
static ERRG_codeE LINK_MONITORP_linkTimerCreate( LINK_MONITORP_linkT *paramsP )
{
   ERRG_codeE ret;

   paramsP->linkTimer.funcCb = LINK_MONITORP_linkTimerExpireCallback;
   paramsP->linkTimer.argP = paramsP;
   ret = OS_LYRG_createTimer(&paramsP->linkTimer);
   if (ERRG_FAILED(ret))
   {
      //LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to create link timer\n");
      printf("Failed to create link timer\n");
   }
   else
   {
      //LOGG_PRINT(LOG_DEBUG_E, NULL, "create link timer. cb = %p argP = %p\n", svcP->linkParams.linkTimer.funcCb, svcP->linkParams.linkTimer.argP);
      //printf("create link timer. cb = %p argP = %p\n", linkParamsP->linkTimer.funcCb, linkParamsP->linkTimer.argP);
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: LINK_MONITORG__rxEvent
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void LINK_MONITORG_rxEvent( LINK_MONITORG_handleT lmH )
{
   LINK_MONITORP_linkT *paramsP = (LINK_MONITORP_linkT*)lmH;

   if (paramsP)
      LINK_MONITORP_invoke(paramsP, LINK_MONITORP_LINK_EVENT_RX_E);
}

/****************************************************************************
*
*  Function Name: LINK_MONITORP__create
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
int LINK_MONITORG_create(LINK_MONITORG_handleT *lmH, const LINK_MONITORG_cfg *cfgP)
{
   ERRG_codeE ret = INU_LM__RET_SUCCESS;
#ifndef __UART_ON_FPGA__   
   LINK_MONITORP_linkT *linkParamsP = (LINK_MONITORP_linkT*)malloc(sizeof(LINK_MONITORP_linkT));
   memset(linkParamsP,0,sizeof(LINK_MONITORP_linkT));

   linkParamsP->linkEventCallBack = cfgP->linkEventCallBack;
   linkParamsP->linkEventCBparam  = cfgP->linkEventCBparam;
   linkParamsP->ref               = (inu_ref*)cfgP->ref;

   OS_LYRG_aquireMutex(&linkParamsP->linkStateMutex);
   LINK_MONITORP_linkTimerCreate(linkParamsP);

   *lmH = linkParamsP;

   LINK_MONITORP_invoke(linkParamsP, LINK_MONITORP_LINK_EVENT_START_E);
#endif
   return ret;
}


/****************************************************************************
*
*  Function Name: LINK_MONITORP__delete
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
int LINK_MONITORG_delete(LINK_MONITORG_handleT lmH)
{
   ERRG_codeE ret = INU_LM__RET_SUCCESS;
   LINK_MONITORP_linkT *linkParamsP = (LINK_MONITORP_linkT*)lmH;

   LINK_MONITORP_invoke(linkParamsP, LINK_MONITORP_LINK_EVENT_STOP_E);
   OS_LYRG_deleteTimer(&linkParamsP->linkTimer);
   free(lmH);

   return ret;
}
