/****************************************************************************
 *   
 *   FileName: queue_lyr.c
 *
 *   Author: Danny Bibi Ram B.
 *  
 *   Date:
 *
 *   Description: Source file the CLS Queue 
 *   
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"

#include "os_lyr.h"
#include "queue_lyr.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L       D E F I N I T O I N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef enum
{
   QUEUE_LYRP_STATE_FREE_E,
   QUEUE_LYRP_STATE_SEND_E,
   QUEUE_LYRP_STATE_RECV_E
} QUEUE_LYRP_pktStateE;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L     F U N C T I O N          ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: QUEUE_LYRG_init
*
*  Description: Initialize CLS queue
*   
*  Inputs: connParametersP - pointers to CLS conneciton structure
*
*  Outputs: void
*
*  Returns: void
*
****************************************************************************/
void QUEUE_LYRG_init(QUEUE_LYRG_queInfoT *queP)
{
   UINT16 i;

   OS_LYRG_lockMutex(&queP->queueMutexTx);
   for (i = 0; i < queP->numTxInQue; i++)
   {
      queP->queueTbl[i].pktState = QUEUE_LYRP_STATE_FREE_E;
      queP->queueTbl[i].pktLen = 0;
      queP->queueTbl[i].pktBuffP = NULL;
   }
   queP->sendTailIdx = 0;
   queP->sendHeadIdx = 0;
   OS_LYRG_unlockMutex(&queP->queueMutexTx);

   OS_LYRG_lockMutex(&queP->queueMutexRx);
   for (i = queP->numTxInQue; i < queP->queueSize; i++)
   {
      queP->queueTbl[i].pktState = QUEUE_LYRP_STATE_FREE_E;
      queP->queueTbl[i].pktLen = 0;
      queP->queueTbl[i].pktBuffP = NULL;
   }
   queP->recvTailIdx = queP->numTxInQue;
   queP->recvHeadIdx = queP->numTxInQue;
   OS_LYRG_unlockMutex(&queP->queueMutexRx);
}

/****************************************************************************
*
*  Function Name: QUEUE_LYRG_add
*
*  Description: Insert buffer into queue (tail)
*   
*  Inputs: queP - pointers to CLS queue 
*          bufferP,len - pointer to buffer and its lengh
*          type - pkt type - TX or RX.
*
*  Outputs: void
*
*  Returns: ERRG_codeE - success if inserted. error on overflow.
*  
*  Contexts: Multiple contexts.
*
****************************************************************************/
ERRG_codeE QUEUE_LYRG_add(QUEUE_LYRG_queInfoT *queP, void *bufferP, UINT32 len, INT32 channel, UINT32 arg, QUEUE_LYRG_pktTypeE type)
{
   UINT16 *indexP = 0;
   UINT16 botIndex, size;
   INT32 busyState;
   OS_LYRG_mutexT *queMutexP;
   ERRG_codeE retVal = QUEUE_LYR__RET_SUCCESS;

   queMutexP = (type == QUEUE_LYRG_TYPE_TX_E) ? &queP->queueMutexTx : &queP->queueMutexRx;

   OS_LYRG_lockMutex(queMutexP);

   if(type == QUEUE_LYRG_TYPE_TX_E)
   {
      indexP = &queP->sendTailIdx;
      botIndex = 0;
      size = queP->numTxInQue;
      busyState = QUEUE_LYRP_STATE_SEND_E;
   }
   else
   {
      indexP = &queP->recvTailIdx;
      botIndex = queP->numTxInQue;
      size = queP->queueSize;
      busyState = QUEUE_LYRP_STATE_RECV_E;
   }

   if(queP->queueTbl[*indexP].pktState == QUEUE_LYRP_STATE_FREE_E)
   {
      //Next is free - insert
      queP->queueTbl[*indexP].pktState = busyState;
      queP->queueTbl[*indexP].pktBuffP = bufferP;
      queP->queueTbl[*indexP].pktLen = len;
      queP->queueTbl[*indexP].chanId = (UINT16)channel;
      queP->queueTbl[*indexP].arg = arg;
   }
   else
   {
      //Next not free - queue overflow - return error
      retVal = QUEUE_LYR__ERR_GENERAL_ERR;
   }   
   
   //Increment head with wrap around
   if(ERRG_SUCCEEDED(retVal))
   {
      *indexP = ((*indexP + 1) < size) ? (*indexP) + 1: botIndex;
   }

   OS_LYRG_unlockMutex(queMutexP);

   return retVal;
}

/****************************************************************************
*
*  Function Name: QUEUE_LYRG_rem
*
*  Description: Removes buffer from queue (head)
*   
*  Inputs: conParamsP - pointers to CLS conneciton structure
*          type - pkt type - TX or RX.
*
*  Outputs: bufferP,lenP - On return will removed buffer pointer and its length.
*
*  Returns:
*  
*  Contexts: Multiple contexts.
*
****************************************************************************/
void QUEUE_LYRG_rem(QUEUE_LYRG_queInfoT *queP, void **bufferP, UINT32 *lenP, INT32 *chanIdP, UINT32 *argP, QUEUE_LYRG_pktTypeE type)
{
   UINT16 *indexP = 0;
   UINT16 botIndex, size;
   INT32 readyState;
   OS_LYRG_mutexT *queMutexP;
   UINT16 found = 1;

   queMutexP = (type == QUEUE_LYRG_TYPE_TX_E) ? &queP->queueMutexTx : &queP->queueMutexRx;
   OS_LYRG_lockMutex(queMutexP);

   if(type == QUEUE_LYRG_TYPE_TX_E)
   {
      indexP      = &queP->sendHeadIdx;
      botIndex    = 0;
      size        = queP->numTxInQue;
      readyState  = QUEUE_LYRP_STATE_SEND_E;
   }
   else
   {
      indexP      = &queP->recvHeadIdx;
      botIndex    = queP->numTxInQue;
      size        = queP->queueSize;
      readyState  = QUEUE_LYRP_STATE_RECV_E;
   }

   //Attempt to remove head from queue - if not in receive/sending state ignore
   if(queP->queueTbl[*indexP].pktState == readyState)
   {
      //Set Outputs
      *lenP = queP->queueTbl[*indexP].pktLen;
      *bufferP = queP->queueTbl[*indexP].pktBuffP;
      *chanIdP = queP->queueTbl[*indexP].chanId;
      *argP = queP->queueTbl[*indexP].arg;

      //Remove from queue
      queP->queueTbl[*indexP].pktState = QUEUE_LYRP_STATE_FREE_E;
      queP->queueTbl[*indexP].pktLen   = 0;
      queP->queueTbl[*indexP].pktBuffP = NULL;
   }   
   else
   {
      *lenP = 0; //empty
      found = 0;
   }

   if(found)
   {
      //Increment head with wrap around
      *indexP = ((*indexP) + 1 < size) ? (*indexP) + 1: botIndex;
   }

   OS_LYRG_unlockMutex(queMutexP);

}

int QUEUE_LYRG_isFull(QUEUE_LYRG_queInfoT *queP, QUEUE_LYRG_pktTypeE type)
{
   UINT16 *indexP = 0;
   int isFull;
   OS_LYRG_mutexT *queMutexP;

   queMutexP = (type == QUEUE_LYRG_TYPE_TX_E) ? &queP->queueMutexTx : &queP->queueMutexRx;

   OS_LYRG_lockMutex(queMutexP);

   if(type == QUEUE_LYRG_TYPE_TX_E)
      indexP = &queP->sendTailIdx;
   else
      indexP = &queP->recvTailIdx;

   if(queP->queueTbl[*indexP].pktState == QUEUE_LYRP_STATE_FREE_E)
      isFull = 0;
   else
      isFull = 1;

   OS_LYRG_unlockMutex(queMutexP);

   return isFull;
}

#ifdef __cplusplus
}
#endif
