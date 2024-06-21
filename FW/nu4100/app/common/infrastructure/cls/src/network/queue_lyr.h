/****************************************************************************
 *
 *   FileName: queue_lyr.h
 *
 *   Author: Danny Bibi. Ram B.
 *
 *   Date: 
 *
 *   Description: CLS (COM Layer Stack) Queue API 
 *   
 ****************************************************************************/
#ifndef _QUEUE_LYR_H_
#define _QUEUE_LYR_H_

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define QUEUE_LYRG_QUE_MAX_SIZE    (1024)

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef enum
{
   QUEUE_LYRG_TYPE_TX_E = 1,
   QUEUE_LYRG_TYPE_RX_E
} QUEUE_LYRG_pktTypeE;

typedef struct
{
   INT32    pktState;
   UINT32   pktLen;
   void     *pktBuffP;
   INT32    chanId;
   UINT32   arg;
} QUEUE_LYRG_queuePktT;

typedef struct
{
   QUEUE_LYRG_queuePktT    queueTbl[QUEUE_LYRG_QUE_MAX_SIZE];
   OS_LYRG_mutexT          queueMutexRx;
   BOOL                    mutexRxFlag;
   OS_LYRG_mutexT          queueMutexTx;
   BOOL                    mutexTxFlag;
   UINT16                  queueSize;
   UINT16                  numTxInQue;
   UINT16                  sendHeadIdx;
   UINT16                  sendTailIdx;
   UINT16                  recvHeadIdx;
   UINT16                  recvTailIdx;
} QUEUE_LYRG_queInfoT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

void QUEUE_LYRG_init(QUEUE_LYRG_queInfoT *queP);
ERRG_codeE QUEUE_LYRG_add(QUEUE_LYRG_queInfoT *queP, void *bufferP, UINT32 len, INT32 chanNum, UINT32 arg, QUEUE_LYRG_pktTypeE type);
void QUEUE_LYRG_rem(QUEUE_LYRG_queInfoT *queP, void **bufferP, UINT32 *lenP, INT32 *chanNumP, UINT32 *arg, QUEUE_LYRG_pktTypeE type);
int QUEUE_LYRG_isFull(QUEUE_LYRG_queInfoT *queP, QUEUE_LYRG_pktTypeE type);

#ifdef __cplusplus
}
#endif

#endif /* _QUEUE_LYR_H_ */

