/****************************************************************************
 *
 *
 *   FileName: conn_lyr.c
 *
 *   Author: Danny Bibi. Ram B.
 *
 *   Date:
 *
 *   Description: Source file the CLS conncection layer
 *
 ****************************************************************************/


/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"

#include <assert.h>
#include "mem_pool.h"
#include "sys_defs.h"
#include "os_lyr.h"
#include "conn_lyr.h"
#include "queue_lyr.h"
#include "marshal.h"
#include "uvc_com.h"
#include "inu_factory.h"
#include "utils.h"
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************       L O C A L       D E F I N I T O I N S  ***************
 ****************************************************************************/
#define CONN_LYRP_RX_POLL_USEC  (30*1000)
#define CONN_LYRP_TX_POLL_MSEC  (30)

#define CONN_LYRP_ALLOC_SLEEP_MSEC  (10)
#define CONN_LYRP_ALLOC_TIME_MSEC   (0)
#define CONN_LYRP_MAX_CONNECTIONS     (20)
#define CONN_LYRP_MEM_POOL_WAIT_USEC  (1000)

#define CONN_LYRP_SYNC_LOW (0)
#define CONN_LYRP_SYNC_HIGH (1)
#define CONN_LYRP_TX_SYNC (0)
#define CONN_LYRP_RX_SYNC (1)

#define CLS_RX        (0)
#define CLS_TX        (1)
#define CLS_NUM_DIRS  (2)

#define CONN_LYRP_IS_OPEN(entry)        (entry->id < CONN_LYRP_MAX_CONNECTIONS)
#define CONN_LYRP_NELEMENTS(array)     (sizeof(array)/sizeof(array[0]))

#if (DEFSG_IS_HOST)
#define CONN_LYRP_DRAIN_BUF_SIZE       (512*1024)
#else
#define CONN_LYRP_DRAIN_BUF_SIZE       (128*1024)
#endif

#define CONN_LYRP_CLS_HDR_LENGTH_NUM_BITS     (56)
#define CONN_LYRP_CLS_HDR_CHANNEL_ID_NUM_BITS (8)

//length
#define CONN_LYRP_CLS_HDR_LENGTH_OFFSET            (0)
#define CONN_LYRP_CLS_HDR_LENGTH_MASK              ((1ULL << CONN_LYRP_CLS_HDR_LENGTH_NUM_BITS) - 1)
#define CONN_LYRP_CLS_HDR_LENGTH_GET(x)            ((x >> CONN_LYRP_CLS_HDR_LENGTH_OFFSET) & CONN_LYRP_CLS_HDR_LENGTH_MASK)
#define CONN_LYRP_CLS_HDR_LENGTH_SET(x,id)         (x = ((x & ~(CONN_LYRP_CLS_HDR_LENGTH_MASK << CONN_LYRP_CLS_HDR_LENGTH_OFFSET)) | ((id & CONN_LYRP_CLS_HDR_LENGTH_MASK) << CONN_LYRP_CLS_HDR_LENGTH_OFFSET)))
//channelId
#define CONN_LYRP_CLS_HDR_CHANNEL_ID_OFFSET        (CONN_LYRP_CLS_HDR_LENGTH_NUM_BITS)
#define CONN_LYRP_CLS_HDR_CHANNEL_ID_MASK          ((1ULL << CONN_LYRP_CLS_HDR_CHANNEL_ID_NUM_BITS) - 1)
#define CONN_LYRP_CLS_HDR_CHANNEL_ID_GET(x)        ((x >> CONN_LYRP_CLS_HDR_CHANNEL_ID_OFFSET) & CONN_LYRP_CLS_HDR_CHANNEL_ID_MASK)
#define CONN_LYRP_CLS_HDR_CHANNEL_ID_SET(x,id)     (x = ((x & ~(CONN_LYRP_CLS_HDR_CHANNEL_ID_MASK << CONN_LYRP_CLS_HDR_CHANNEL_ID_OFFSET)) | ((id & CONN_LYRP_CLS_HDR_CHANNEL_ID_MASK) << CONN_LYRP_CLS_HDR_CHANNEL_ID_OFFSET)))

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef enum
{
   CONN_LYRP_DOWN_E = 0,
   CONN_LYRP_UP_E,
   CONN_LYRP_PAUSE_E
} CONN_LYRP_statusE;

typedef struct
{
   UINT32 txByteCnt;
   UINT32 txPktCnt;
   UINT32 rxByteCnt;
   UINT32 rxPktCnt;
   UINT32 txQueOverflowCnt;
   UINT32 rxQueOverflowCnt;
   UINT32 outOfMem;
   UINT32 outOfMemHdr;
   UINT32 txPktErrCnt;
} CONN_LYRP_statsT;

typedef struct
{
   void *cb[CLS_NUM_DIRS];
   void *arg[CLS_NUM_DIRS];
 } CONN_LYRP_cbFuncT;

typedef struct
{
   UINT32                              id;

   //Mutex for serializing api calls
   OS_LYRG_mutexT                      serialMutex;
   BOOL                                serialMutexFlag;
   //Mutex for callback registration
   OS_LYRG_mutexT                      cbMutex[CLS_NUM_DIRS];
   BOOL                                cbMutexFlag[CLS_NUM_DIRS];
   //Status flag and mutex
   CONN_LYRP_statusE                   status;
   UINT16                              rxsync;
   UINT16                              txsync;
   OS_LYRG_mutexT                      statusMutex;
   BOOL                                statusMutexFlag;
   //Buffer for draining on rx
   UINT8                               *drainBufP;
   //Receive memory pool (for packet mode)
   MEM_POOLG_handleT                   fixedRxPool;
   //Queue layer
   QUEUE_LYRG_queInfoT                 queue;
   //Callback functions for send/receive notification
   CONN_LYRP_cbFuncT                  *cbFuncTblP;
   //Interface-specific configuration and functions
   //connection interface is up
   void                                *ifHandle;
   UINT16                              isIfUp;
   CLS_COMG_ifOperations               ifOps;
   CLS_COMG_capsT                      ifCaps;
   //Connection configurations
   OS_LYRG_threadParams                threadTxParams;
   OS_LYRG_threadParams                threadRxParams;
   //Stats
   CONN_LYRP_statsT                    stats;
   //ChainHdr
   UINT16                              chainHdr;

   //API2:

   //bind to commP
   void                                *commP;
   CONN_LYRG_timedAllocBufCbT          timedAllocBufCb;
} CONN_LYRP_infoT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

CONN_LYRP_infoT  CONN_LYRP_connTbl[CONN_LYRP_MAX_CONNECTIONS];
static CONN_LYRP_cbFuncT CONN_LYRP_cbFuncTbl[CONN_LYRP_MAX_CONNECTIONS];
static UINT32 CONN_LYRP_hdrMarshalSize = 0;
static UINT8 CONN_LYRP_drainBuf[CONN_LYRP_DRAIN_BUF_SIZE];

/******************************************************************************
 ***************     P R E    D E F I N I T I O N     OF        ***************
 ***************     L O C A L         F U N C T I O N S        ***************
 ******************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE CONN_LYRP_initEntry(CONN_LYRP_infoT *entryP)
{
   ERRG_codeE retval=CONN_LYR__RET_SUCCESS;
   int dir;
   
   memset(entryP, 0, sizeof(CONN_LYRP_infoT));
   entryP->id = CONN_LYRP_MAX_CONNECTIONS;
   entryP->drainBufP = CONN_LYRP_drainBuf;

   if(ERRG_SUCCEEDED(retval))
   {
      if((OS_LYRG_aquireMutex(&entryP->serialMutex)) == SUCCESS_E)
         entryP->serialMutexFlag = TRUE;
      else
         retval = CONN_LYR__ERR_OUT_OF_RSRCS;
   }
   
   if(ERRG_SUCCEEDED(retval))
   {  
      dir = CLS_TX;
      if((OS_LYRG_aquireMutex(&entryP->cbMutex[dir])) == SUCCESS_E)
         entryP->cbMutexFlag[dir] = TRUE;
      else
         retval = CONN_LYR__ERR_OUT_OF_RSRCS;
   }

   if(ERRG_SUCCEEDED(retval))
   {
      dir = CLS_RX;
      if((OS_LYRG_aquireMutex(&entryP->cbMutex[dir])) == SUCCESS_E)
         entryP->cbMutexFlag[dir] = TRUE;
      else
         retval = CONN_LYR__ERR_OUT_OF_RSRCS;
   }
   
   return retval;
}

static void CONN_LYRP_deinitEntry(CONN_LYRP_infoT *entryP)
{
   int dir;
   if(entryP->serialMutexFlag)
   {
      OS_LYRG_releaseMutex(&entryP->serialMutex);
      entryP->serialMutexFlag = FALSE;
   }

   dir = CLS_TX;
   if(entryP->cbMutexFlag[dir])
   {
      OS_LYRG_releaseMutex(&entryP->cbMutex[dir]);
      entryP->cbMutexFlag[dir] = FALSE;
   }
   dir = CLS_RX;
   if(entryP->cbMutexFlag[dir])
   {
      OS_LYRG_releaseMutex(&entryP->cbMutex[dir]);
      entryP->cbMutexFlag[dir] = FALSE;
   }

   memset(entryP, 0, sizeof(CONN_LYRP_infoT));
}


static void CONN_LYRP_parseHeader(UINT8 *hdrP, UINT32 *lenP, INT32 *channelIdP)
{
   UINT16 idx = 0;
   UINT64 val;

   MARSHALG_u64(MARSHALG_UNDO, &val, &hdrP[idx]);
   idx += sizeof(UINT64);
   *lenP = CONN_LYRP_CLS_HDR_LENGTH_GET(val);
   *channelIdP = CONN_LYRP_CLS_HDR_CHANNEL_ID_GET(val);

   LOGG_PRINT(LOG_DEBUG_E, NULL, "0x%x %d %d\n", *((UINT32 *)hdrP),*channelIdP, *lenP);
   
   return;
}

static UINT32 CONN_LYRP_writeHeader(UINT8 *hdrP, UINT32 len, INT32 channelId)
{
   UINT32 idx = 0;
   UINT64 val = 0;

   //Write header using marshal API to handle endianness and packing
   if(hdrP)
   {
      CONN_LYRP_CLS_HDR_LENGTH_SET(val,len);
      CONN_LYRP_CLS_HDR_CHANNEL_ID_SET(val,channelId);      
      MARSHALG_u64(MARSHALG_DO, &val, &hdrP[idx]);    
   }

   idx += sizeof(UINT64);
  
   return idx;
}

/*
*  For fixed mode - set dataLen of chain to match the fixed size using last buffer in the chain.
*  The data which is after the real data will ignored.
*/
static ERRG_codeE CONN_LYRP_fixChainLen(CONN_LYRP_infoT * entryP, MEM_POOLG_bufDescT *descP)
{
   ERRG_codeE ret = CONN_LYR__RET_SUCCESS;
   MEM_POOLG_bufDescT *last;
   UINT32 padlen, chainlen;

   last = MEM_POOLG_getLast(descP);
   chainlen = MEM_POOLG_getChainLen(descP);
   if(entryP->ifCaps.fixedSize >= chainlen)
   {
      //fix the last buffer's length
      padlen = entryP->ifCaps.fixedSize - chainlen;
      if(padlen + last->dataLen <= last->size)
      {
          last->dataLen += padlen;
      }
      else
      {
        ret = CONN_LYR__ERR_INVALID_ARGS;//not enough room in last
      }
      //last->dataLen = entryP->ifCaps.fixedSize -(chainLen -last->dataLen);
   }
   else
   {
      ret = CONN_LYR__ERR_INVALID_ARGS; //exceeded fixed size
   }
   return ret;
}

static ERRG_codeE CONN_LYRP_addHdr(CONN_LYRP_infoT *entryP, INT32 refId, MEM_POOLG_bufDescT **descP, UINT16 isFixed, UINT16 chainHdr)
{
   ERRG_codeE retVal = CONN_LYR__RET_SUCCESS;

   if (chainHdr)
   {
      if (!isFixed)
      {
         MEM_POOLG_bufDescT *bufferHeaderP = NULL;
         retVal = entryP->timedAllocBufCb(entryP->commP, refId, CONN_LYRP_hdrMarshalSize, 0, (void**)&bufferHeaderP);
         if(ERRG_SUCCEEDED(retVal))
         {
            CONN_LYRP_writeHeader(bufferHeaderP->dataP, MEM_POOLG_getChainLen(*descP), refId);
            bufferHeaderP->dataLen = CONN_LYRP_hdrMarshalSize;
            
            MEM_POOLG_chain(bufferHeaderP, *descP);
            *descP = bufferHeaderP; //swap
         }
         else
         {
            entryP->stats.outOfMemHdr++;
         }
      }
      else
      {
         //write header to start of buffer
         (*descP)->dataP -= CONN_LYRP_hdrMarshalSize;
         CONN_LYRP_writeHeader((*descP)->dataP, MEM_POOLG_getChainLen(*descP), refId);
         (*descP)->dataLen += CONN_LYRP_hdrMarshalSize;
      }
   }
   else
   {
      FIX_UNUSED_PARAM_WARN(isFixed);

      //write header to start of buffer
      (*descP)->dataP -= CONN_LYRP_hdrMarshalSize;
      CONN_LYRP_writeHeader((*descP)->dataP, MEM_POOLG_getChainLen(*descP), refId);
      (*descP)->dataLen += CONN_LYRP_hdrMarshalSize;
   }

   return retVal;
}

static void CONN_LYRP_setStatus(CONN_LYRP_infoT *entryP, CONN_LYRP_statusE status)
{
   OS_LYRG_lockMutex(&entryP->statusMutex);
   entryP->status = status;
   OS_LYRG_unlockMutex(&entryP->statusMutex);
}

static CONN_LYRP_statusE CONN_LYRP_getStatus(CONN_LYRP_infoT *entryP)
{
   CONN_LYRP_statusE st;
   OS_LYRG_lockMutex(&entryP->statusMutex);
   st = entryP->status;;
   OS_LYRG_unlockMutex(&entryP->statusMutex);
   return st;
}

static CONN_LYRP_statusE CONN_LYRP_waitForStatus(CONN_LYRP_infoT *entryP, CONN_LYRP_statusE status)
{
   CONN_LYRP_statusE curr;

   curr = CONN_LYRP_getStatus(entryP);
   while((curr != status) && (curr != CONN_LYRP_DOWN_E))
   {  
      OS_LYRG_usleep(1000*10);
      curr = CONN_LYRP_getStatus(entryP);
   }
   return curr;
}

static void CONN_LYRP_sync(CONN_LYRP_infoT *entryP, int isRx)
{
   int sync;

   OS_LYRG_lockMutex(&entryP->statusMutex);
   sync = (isRx) ? entryP->rxsync : entryP->txsync;
   OS_LYRG_unlockMutex(&entryP->statusMutex);

   while(sync != CONN_LYRP_SYNC_HIGH)
   {
      OS_LYRG_usleep(1000*10);

      OS_LYRG_lockMutex(&entryP->statusMutex);
      sync = (isRx) ? entryP->rxsync : entryP->txsync;
      OS_LYRG_unlockMutex(&entryP->statusMutex);      
   }
}

static void CONN_LYRP_setSync(CONN_LYRP_infoT *entryP, int isRx)
{
   OS_LYRG_lockMutex(&entryP->statusMutex);
   if(isRx)
      entryP->rxsync = CONN_LYRP_SYNC_HIGH;
   else
      entryP->txsync = CONN_LYRP_SYNC_HIGH;
   OS_LYRG_unlockMutex(&entryP->statusMutex);
}

static void CONN_LYRP_resetSync(CONN_LYRP_infoT *entryP, int isRx)
{
   OS_LYRG_lockMutex(&entryP->statusMutex);
   if(isRx)
      entryP->rxsync = CONN_LYRP_SYNC_LOW;
   else
      entryP->txsync = CONN_LYRP_SYNC_LOW;
   OS_LYRG_unlockMutex(&entryP->statusMutex);
}


static void CONN_LYRP_allocError(CONN_LYRP_infoT *entryP, ERRG_codeE err)
{
   if(err == MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL)
   {
      entryP->stats.outOfMem++;
      LOGG_PRINT(LOG_DEBUG_E, NULL, "alloc error no buffers cnt=%d\n", entryP->stats.outOfMem);
   }
   else
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "alloc for unexpected size\n");
   }
}

static ERRG_codeE CONN_LYRP_timedAlloc(MEM_POOLG_handleT poolH, UINT32 timeoutMsec, UINT32 size, MEM_POOLG_bufDescT **outBufP)
{
   ERRG_codeE  ret = CONN_LYR__RET_SUCCESS;
   INT32       leftMsec = (INT32)timeoutMsec;
   UINT32      sleepMsec;
   MEM_POOLG_bufDescT *bufP = NULL;

   do
   {
      ret = MEM_POOLG_alloc(poolH, size, &bufP);
      if(ERRG_FAILED(ret))
      {
         if(ret == MEM_POOL__ERR_ALLOC_BUF_FAIL_NO_BUF_IN_POOL)
         {
            sleepMsec= (leftMsec > CONN_LYRP_ALLOC_SLEEP_MSEC) ? CONN_LYRP_ALLOC_SLEEP_MSEC : leftMsec;
            LOGG_PRINT(LOG_DEBUG_E, NULL, "warn alloc timeout left=%d %d\n", leftMsec,sleepMsec);
            if(sleepMsec > 0)
            {
               OS_LYRG_usleep(sleepMsec*1000);
               leftMsec -=sleepMsec;
            }
         }
         else
            break; //non-timeout error
       }
   }while((!bufP) && (leftMsec > 0));
   
   *outBufP = bufP;
   
   return ret;
}

static ERRG_codeE CONN_LYRP_drain(CONN_LYRP_infoT *entryP, UINT32 len, UINT32 timeout_usec)
{
   ERRG_codeE retVal=CONN_LYR__RET_SUCCESS;
   INT32 left;
   UINT32 readLen = 0;
   UINT32 toRead;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "Draining begin %d\n", len);

   left = len;

   while(left>0)
   {
      toRead = MIN((UINT32)left, sizeof(CONN_LYRP_drainBuf));
      if((entryP->ifCaps.flags & CLS_COMG_CAP_TIMED_RECV))
         retVal = entryP->ifOps.timed_recv(entryP->ifHandle, entryP->drainBufP, toRead, timeout_usec, &readLen);
      else
         retVal = entryP->ifOps.recv(entryP->ifHandle, entryP->drainBufP, toRead, &readLen);

      LOGG_PRINT(LOG_DEBUG_E, NULL, "Draining... total=%d, toRead=%d readLen=%d left=%d\n", len, toRead, readLen, left-readLen);
      //TODO handle timeout correctly 
      if(ERRG_FAILED(retVal))
      {
         break;
      }
      left -= readLen;
      readLen = 0;
   }
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Draining done. left=%d\n", left);

   return retVal;
}

static ERRG_codeE CONN_LYRP_recvUnBufferedRead(CONN_LYRP_infoT *entryP,UINT32 len, UINT8 *bufPhyP, UINT8 *bufP, UINT32 timeoutUsec )
{
   ERRG_codeE retVal;
   UINT32 readLen;

#ifdef INU_PROFILER_CONN_LYR
   INUG_profiler_get_timestamp();
#endif

   if(bufP)
   {
      if(entryP->ifOps.recv_unbuffered)
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Starting UnBuffered read \n");
         retVal = entryP->ifOps.recv_unbuffered(entryP->ifHandle, bufP, len, &readLen);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Interface doesn't support unbuffered read \n"); 
         return CONN_LYR__ERR_INVALID_ARGS;
      }
      
      if(ERRG_SUCCEEDED(retVal))
      {
         //sanitfy check - verify read len is equal to requested len on success
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Read %lu bytes  \n",readLen );
         if(len != readLen)
         {         
            retVal = CONN_LYR__ERR_RECV_ERROR;
         }
         entryP->stats.rxByteCnt += readLen;
      }
   }
   else
   {
      retVal = CONN_LYRP_drain(entryP, len, timeoutUsec);
   }
   return retVal;
}
static ERRG_codeE CONN_LYRP_recv(CONN_LYRP_infoT *entryP,UINT32 len, UINT8 *bufPhyP, UINT8 *bufP, UINT32 timeoutUsec )
{
   ERRG_codeE retVal;
   UINT32 readLen;

#ifdef INU_PROFILER_CONN_LYR
   INUG_profiler_get_timestamp();
#endif

   if(bufP)
   {
      if((entryP->ifCaps.flags & CLS_COMG_CAP_TIMED_RECV))
         retVal = entryP->ifOps.timed_recv(entryP->ifHandle, bufP, len, timeoutUsec, &readLen);
      if ((entryP->ifCaps.flags & CLS_COMG_CAP_RX_CONT_RECV) && bufPhyP)
      {
         retVal = entryP->ifOps.recvContBuf(entryP->ifHandle, bufPhyP, bufP, len, &readLen);
      }
      else
         retVal = entryP->ifOps.recv(entryP->ifHandle, bufP, len, &readLen);
      
      if(ERRG_SUCCEEDED(retVal))
      {
         //sanitfy check - verify read len is equal to requested len on success
         if(len != readLen)
         {         
            retVal = CONN_LYR__ERR_RECV_ERROR;
         }
         entryP->stats.rxByteCnt += readLen;
      }
   }
   else
   {
      retVal = CONN_LYRP_drain(entryP, len, timeoutUsec);
   }
   return retVal;
}

static ERRG_codeE CONN_LYRP_peek(CONN_LYRP_infoT *entryP,UINT32 len, UINT8 *bufP, UINT32 timeoutUsec)
{
   ERRG_codeE retVal;
   UINT32 readLen;
   (void)timeoutUsec; //TODO: peek should be removed, or we should support timeout 

   retVal = entryP->ifOps.peek(entryP->ifHandle, bufP, len, &readLen);
   
   if(ERRG_SUCCEEDED(retVal))
   {
      //sanity check - verify read len is equal to requested len on success
      if(len != readLen)
      {         
         retVal = CONN_LYR__ERR_RECV_ERROR;
      }
   }
   return retVal;
}

static ERRG_codeE CONN_LYRP_recvParseHdr(CONN_LYRP_infoT *entryP, UINT32 *lenP, INT32 *channelIdP, UINT8 *bufP, UINT32 timeoutUsec)
{
   ERRG_codeE retVal;
   memset(bufP, 0, CONN_LYRP_hdrMarshalSize);

   if(entryP->ifCaps.flags & CLS_COMG_CAP_PEEK)
      retVal = CONN_LYRP_peek(entryP, CONN_LYRP_hdrMarshalSize, bufP, timeoutUsec);
   else
      retVal = CONN_LYRP_recv(entryP, CONN_LYRP_hdrMarshalSize, NULL, bufP, timeoutUsec);

   if(ERRG_SUCCEEDED(retVal))
   {
      CONN_LYRP_parseHeader(bufP, lenP, channelIdP);
   }
   return retVal;
}

void CONN_LYRG_txCallback(void *entry, INT32 channelId, MEM_POOLG_bufDescT *bufDescP, UINT32 txid)
{
   void *cbFunc;
   int dir = CLS_TX;
   CONN_LYRP_infoT *entryP=(CONN_LYRP_infoT*)entry;
  
   OS_LYRG_lockMutex(&entryP->cbMutex[dir]);

   (void)txid;
   cbFunc = entryP->cbFuncTblP->cb[dir];
   if(cbFunc)
      ((CONN_LYRG_txCbT)entryP->cbFuncTblP->cb[dir])(entryP->cbFuncTblP->arg[dir], channelId, bufDescP);

   OS_LYRG_unlockMutex(&entryP->cbMutex[dir]);

}

static void CONN_LYRP_rxCallback(CONN_LYRP_infoT *entryP, INT32 channelId, MEM_POOLG_bufDescT *bufDescP)
{
   void *cbFunc;
   int dir = CLS_RX;
  
   if (channelId == -1 && bufDescP == NULL)
   {
       cbFunc = entryP->cbFuncTblP->cb[dir];
       if (cbFunc)
           ((CONN_LYRG_rxCbT)entryP->cbFuncTblP->cb[dir])(entryP->cbFuncTblP->arg[dir], channelId, bufDescP);
       return;
   }

   bufDescP->dataP += CONN_LYRP_hdrMarshalSize;
   bufDescP->dataLen -= CONN_LYRP_hdrMarshalSize;

   OS_LYRG_lockMutex(&entryP->cbMutex[dir]);

   cbFunc = entryP->cbFuncTblP->cb[dir];
   if (cbFunc)
      ((CONN_LYRG_rxCbT)entryP->cbFuncTblP->cb[dir])(entryP->cbFuncTblP->arg[dir], channelId, bufDescP);

   OS_LYRG_unlockMutex(&entryP->cbMutex[dir]);

   if(!cbFunc)
      MEM_POOLG_free(bufDescP);
}

static void CONN_LYRP_register(CONN_LYRP_infoT *entryP, int dir, INT32 channelNum, void *func, void *arg)
{
   assert(channelNum < SYS_DEFSG_MAX_NUM_CHANNELS_E);

   OS_LYRG_lockMutex(&entryP->cbMutex[dir]);

   (void)channelNum;
   entryP->cbFuncTblP->cb[dir] = func;
   entryP->cbFuncTblP->arg[dir]= arg;

   OS_LYRG_unlockMutex(&entryP->cbMutex[dir]);
}

static void CONN_LYRP_recvError(CONN_LYRP_infoT *entryP, MEM_POOLG_bufDescT *bufDescP, ERRG_codeE retVal)
{
   if(bufDescP)
      MEM_POOLG_free(bufDescP);

   if(retVal == CONN_LYR__ERR_RECV_TIMEOUT)
   {
      LOGG_PRINT(LOG_DEBUG_E, 0, "rx timeout\n");//timeout - ignore 
   }
   else
   {
      if(retVal == CONN_LYR__ERR_CONNECTION_IS_DOWN)
         LOGG_PRINT(LOG_INFO_E, retVal, "rx detected connection down\n");
      else
         LOGG_PRINT(LOG_ERROR_E, retVal, "rx failed - set connection down\n");

      CONN_LYRP_setStatus(entryP, CONN_LYRP_DOWN_E);
   }
}

static void CONN_LYRP_rxStream(CONN_LYRP_infoT *entryP, UINT32 timeoutUsec)
{
   ERRG_codeE retVal;
   UINT32 len = 0;
   MEM_POOLG_bufDescT *bufDescP = NULL;
   INT32 refId = 0;
#if (DEFSG_PROCESSOR == DEFSG_GP)
   UINT32 bufPhyAddr;
#endif
   UINT8 hdr[sizeof(CONN_LYRG_clsHdrT)]; //sizeof is >= marshalled size
   LOGG_PRINT(LOG_DEBUG_E, NULL, "begin rx stream\n");

   retVal = CONN_LYRP_recvParseHdr(entryP, &len, &refId, hdr, timeoutUsec);
   if(ERRG_SUCCEEDED(retVal))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Allocating buffer of size %d \n", len+ CONN_LYRP_hdrMarshalSize);
      //Allocate buffer from ref's memory pool based on ref Id.
      retVal = entryP->timedAllocBufCb(entryP->commP, refId, len + CONN_LYRP_hdrMarshalSize, CONN_LYRP_ALLOC_TIME_MSEC, (void**)&bufDescP);
      if(ERRG_SUCCEEDED(retVal) && (bufDescP->dataP))
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "allocated buffer %d %x d=%x\n", len+ CONN_LYRP_hdrMarshalSize,bufDescP,bufDescP->dataP);
#if (DEFSG_PROCESSOR == DEFSG_GP)
         //Check if memory is contiguous, so we can call optimized function in the if
         if (((entryP->ifCaps.flags & CLS_COMG_CAP_RX_CONT_RECV) == CLS_COMG_CAP_RX_CONT_RECV) && 
             (ERRG_SUCCEEDED(MEM_POOLG_getDataPhyAddr(bufDescP, &bufPhyAddr))))
         {
            UINT8 *tempBufP = (UINT8*)bufPhyAddr;
            retVal = CONN_LYRP_recv(entryP, len, &tempBufP[CONN_LYRP_hdrMarshalSize], &bufDescP->dataP[CONN_LYRP_hdrMarshalSize], timeoutUsec);
         }
         else
         {
#endif
            //Read into buffer
            if(entryP->ifCaps.flags & CLS_COMG_CAP_PEEK)
               retVal = CONN_LYRP_recv(entryP, len+CONN_LYRP_hdrMarshalSize, NULL, bufDescP->dataP, timeoutUsec);
            else
               retVal = CONN_LYRP_recv(entryP, len, NULL, &bufDescP->dataP[CONN_LYRP_hdrMarshalSize], timeoutUsec);
#if (DEFSG_PROCESSOR == DEFSG_GP)
         }
#endif

         if(ERRG_SUCCEEDED(retVal))
         {
            if(!(entryP->ifCaps.flags & CLS_COMG_CAP_PEEK))
               memcpy(bufDescP->dataP, hdr, CONN_LYRP_hdrMarshalSize); //Copy back the CLS header to the buffer TEST[rb]
            bufDescP->dataLen = len+CONN_LYRP_hdrMarshalSize;

            entryP->stats.rxPktCnt++;
            CONN_LYRP_rxCallback(entryP, refId, bufDescP);
            
         }
      }
      else
      {
         CONN_LYRP_allocError(entryP, retVal);
         //drain into local buffer - required so we don't lose sync with transmitter on the stream
         retVal = CONN_LYRP_recv(entryP, len, NULL, NULL, timeoutUsec);
      }
   }   

   if(ERRG_FAILED(retVal))
   {
      CONN_LYRP_recvError(entryP, bufDescP, retVal);
   }

#ifdef INU_PROFILER_CONN_LYR
   INUG_profiler_get_timestamp();
#endif    
}

static void CONN_LYRP_rxUnBuffered(CONN_LYRP_infoT *entryP, UINT32 timeoutUsec)
{
   ERRG_codeE retVal;
   UINT32 fixedSize = entryP->ifCaps.unbufferedFixedSize;
   UINT32 len = 0;
   MEM_POOLG_bufDescT *bufDescP = NULL;
   INT32 channelId = 0;
   MEM_POOLG_handleT poolH;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "CONN_LYRP_rxUnBuffered: begin rx packet, fixed Size:%lu \n",fixedSize );

   poolH = entryP->fixedRxPool;
   if(!poolH)
   {
      OS_LYRG_usleep(CONN_LYRP_MEM_POOL_WAIT_USEC);
      return; //no rx pool available 
   }

   retVal = CONN_LYRP_timedAlloc(poolH, 100, fixedSize, &bufDescP);
   if(ERRG_SUCCEEDED(retVal))
   {
      retVal = CONN_LYRP_recvUnBufferedRead(entryP, fixedSize, NULL, bufDescP->dataP, timeoutUsec);
      if(ERRG_SUCCEEDED(retVal))
      {
         //Parse and check header for channel and actual len
         CONN_LYRP_parseHeader(bufDescP->dataP, &len, &channelId);
         // such channelId treated as ref-id should not be bigger than INU_COMM__MAX_NUM_REGISTERED = 100
         // once it happens, should skip it to avoid crash when accessing inu_comm->registeredRef
         // TODO: need to check further why such invalid URB packet is received 
         // by david @ 20230901
         if (channelId >= 100) // INU_COMM__MAX_NUM_REGISTERED
         {
             if (bufDescP)
                 MEM_POOLG_free(bufDescP);
             LOGG_PRINT(LOG_ERROR_E, NULL, "recvd abnormal pkt: len = %u, channelId = %d\n", len, channelId);
             return;
         }
         bufDescP->dataLen = len+CONN_LYRP_hdrMarshalSize;
         entryP->stats.rxPktCnt++;
         //MEM_POOLG_free(bufDescP);
         CONN_LYRP_rxCallback(entryP,channelId,bufDescP);
      }
      else
      {
         CONN_LYRP_recvError(entryP, bufDescP, retVal);
         CONN_LYRP_rxCallback(entryP, -1, NULL);
      }
   }
   else
   {
       CONN_LYRP_allocError(entryP, retVal);
   }
}

static void CONN_LYRP_rxFixed(CONN_LYRP_infoT *entryP, UINT32 timeoutUsec)
{
   ERRG_codeE retVal;
   UINT32 fixedSize = entryP->ifCaps.fixedSize;
   UINT32 len = 0;
   MEM_POOLG_bufDescT *bufDescP = NULL;
   INT32 channelId = 0;
   MEM_POOLG_handleT poolH;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "begin rx packet\n");

   poolH = entryP->fixedRxPool;
   if(!poolH)
   {
      OS_LYRG_usleep(CONN_LYRP_MEM_POOL_WAIT_USEC);
      return; //no rx pool available 
   }

   retVal = CONN_LYRP_timedAlloc(poolH, 100, fixedSize, &bufDescP);
   if(ERRG_SUCCEEDED(retVal))
   {
      //Read packet - entire size
      retVal = CONN_LYRP_recv(entryP, fixedSize, NULL, bufDescP->dataP, timeoutUsec);
      if(ERRG_SUCCEEDED(retVal))
      {
         //Parse and check header for channel and actual len
         CONN_LYRP_parseHeader(bufDescP->dataP, &len, &channelId);
         bufDescP->dataLen = len+CONN_LYRP_hdrMarshalSize;
         entryP->stats.rxPktCnt++;
         CONN_LYRP_rxCallback(entryP,channelId,bufDescP);
      }
      else
      {
         CONN_LYRP_recvError(entryP, bufDescP, retVal);
      }
   }
   else
   {
       CONN_LYRP_allocError(entryP, retVal);
   }
}

/****************************************************************************
*
*  Function Name: CONN_LYRP_rx
*
*  Description: Handles data received on the interface.
*     In stream mode first read the header from the interface and then based on the header 
*     allocate a buffer from the memory pool and read the rest.
*
*     In fixed mode read a fixed size packet from the interface into the internal mem pool 
*     buffer and then parse the header.
*
****************************************************************************/
static void CONN_LYRP_rx(CONN_LYRP_infoT *entryP)
{  
#ifdef INU_PROFILER_CONN_LYR   
    INUG_profiler_get_timestamp();
#endif

   UINT32 timeout = CONN_LYRP_RX_POLL_USEC;
     /*We have added this mode for Helsinki where instead of having three URB (USB Request buffers ) per frame
   We instead have one URB that includes the header and the image which is then demuxed.
   This optimizaton is needed due to the high cost of sending small URBs which have deadtime and handshaking associated with each URB.
   The target side doesn't need any changes 
   */
   if(entryP->ifCaps.flags & CLS_COMG_CAP_MODE_UNBUFFERED_RECV)
      CONN_LYRP_rxUnBuffered(entryP, timeout);
   else if(entryP->ifCaps.flags & CLS_COMG_CAP_MODE_STREAM)
      CONN_LYRP_rxStream(entryP, timeout);
   else
      CONN_LYRP_rxFixed(entryP, timeout);

#ifdef INU_PROFILER_CONN_LYR   
       INUG_profiler_get_timestamp();
#endif

}

static int CONN_LYRP_rxThread(CONN_LYRP_infoT *entryP)
{
   BOOL cond = TRUE;
   CONN_LYRP_statusE status;
   int sel = CONN_LYRP_RX_SYNC;
   
   while(cond)
   {
      CONN_LYRP_rx(entryP);

      status = CONN_LYRP_getStatus(entryP);
      if(status ==CONN_LYRP_PAUSE_E)
      {
         CONN_LYRP_setSync(entryP, sel);
         CONN_LYRP_waitForStatus(entryP, CONN_LYRP_UP_E);
         CONN_LYRP_resetSync(entryP, sel);
      }
      else if(status == CONN_LYRP_DOWN_E)
      {
         CONN_LYRP_setSync(entryP, sel);
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Detected down - exiting thread\n");
         cond = FALSE;
      }
   }

   return CONN_LYR__RET_SUCCESS;
}

void CONN_LYRG_txBuf(void *entry, void *buf, INT32 chanNum, UINT32 txid)
{            
   UINT32  sentBytes = 0;
   ERRG_codeE retVal;
   CONN_LYRP_infoT *entryP=(CONN_LYRP_infoT *)entry;
   MEM_POOLG_bufDescT *bufP=(MEM_POOLG_bufDescT *)buf;
   int numberTransfers = 0;
   unsigned int totalSize = 0;
   {
      /*Temporary pointer used to check the number of transfers and the total size*/
      MEM_POOLG_bufDescT *currPTemp = (MEM_POOLG_bufDescT *)bufP;
      while(currPTemp)
      {
         totalSize = totalSize+currPTemp->dataLen;
         currPTemp = currPTemp->nextP;
         numberTransfers++;
      }
   }
   if((entryP->ifCaps.flags & CLS_COMG_CAP_MODE_UNBUFFERED_SEND_V) )
   {
      if(totalSize==entryP->ifCaps.unbufferedFixedSize)
      {
         retVal = entryP->ifOps.sendv_unbuffered(entryP->ifHandle, bufP, &sentBytes);
      }
      else
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"Cannot transmit due to size mismatch %lu!=%lu \n",totalSize,entryP->ifCaps.unbufferedFixedSize );
         retVal = CONN_LYR__RET_SUCCESS;
      }
   }
   else if(entryP->ifCaps.flags & CLS_COMG_CAP_SENDV)
      retVal = entryP->ifOps.sendv(entryP->ifHandle, bufP, &sentBytes);
   else
      retVal = entryP->ifOps.send(entryP->ifHandle, bufP->dataP, bufP->dataLen, &sentBytes);

   if(ERRG_SUCCEEDED(retVal))
   {
      if (!(entryP->ifCaps.flags & CLS_COMG_CAP_MODE_UVC))  // UVC will cleasr its buffer
        CONN_LYRG_txCallback(entryP,chanNum, bufP, txid);
      entryP->stats.txByteCnt += sentBytes;
      entryP->stats.txPktCnt++;
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "tx error -set connection down\n");
      MEM_POOLG_free(bufP);
      CONN_LYRP_setStatus(entryP,CONN_LYRP_DOWN_E);
      entryP->stats.txPktErrCnt++;
   }   
}

static void CONN_LYRP_tx(CONN_LYRP_infoT *entryP, UINT32 pollMsec)
{
   UINT32               len = 0;
   INT32                chanNum;
   MEM_POOLG_bufDescT   *bufP;
   INT32                ret = SUCCESS_E;
   UINT32               txid;
   
#ifdef INU_PROFILER_CONN_LYR   
   INUG_profiler_get_timestamp();
#endif

   ret = OS_LYRG_waitEvent(entryP->threadTxParams.event, pollMsec);
   if(ret == SUCCESS_E)
   {
      //Wait for single event and empty the queue so we don't miss an event
      do
      {
         len = 0;
         QUEUE_LYRG_rem(&entryP->queue, (void **)&bufP, &len, &chanNum, &txid, QUEUE_LYRG_TYPE_TX_E);
         if(len > 0)
         {
            CONN_LYRG_txBuf(entryP, (void *)bufP, chanNum, txid);
         }
      }while(len > 0);
   }
   else
   {
   //   LOGG_PRINT(LOG_DEBUG_E, NULL, "poll tx wait timeout (%d msec) or error\n", pollMsec);
   }

#ifdef INU_PROFILER_CONN_LYR   
   INUG_profiler_get_timestamp();
#endif

}


static int CONN_LYRP_txThread(CONN_LYRP_infoT *entryP)
{
   BOOL cond = TRUE;
   int sel = CONN_LYRP_TX_SYNC;
   CONN_LYRP_statusE status;

   while(cond)
   {
      CONN_LYRP_tx(entryP, CONN_LYRP_TX_POLL_MSEC);
	  if (!(entryP->ifCaps.flags & CLS_COMG_CAP_MODE_UVC)) 
	  {
        status =CONN_LYRP_getStatus(entryP);
        if(status == CONN_LYRP_PAUSE_E)
        {
           CONN_LYRP_setSync(entryP, sel);
           CONN_LYRP_waitForStatus(entryP, CONN_LYRP_UP_E);
           CONN_LYRP_resetSync(entryP, sel);
        }
        else if(status == CONN_LYRP_DOWN_E)
        {
           CONN_LYRP_setSync(entryP, sel);
           LOGG_PRINT(LOG_DEBUG_E, NULL, "Detected down - exiting thread\n");
           cond = FALSE;
        }
      }

   }
   return CONN_LYR__RET_SUCCESS;
}


static void CONN_LYRP_initCbTblEntry(CONN_LYRP_cbFuncT *tblP)
{
   tblP->cb[CLS_TX] = NULL;
   tblP->cb[CLS_RX] = NULL;
   tblP->arg[CLS_TX] = NULL;
   tblP->arg[CLS_RX] = NULL;
}


/****************************************************************************
*
*  Function Name: CONN_LYRP_cleanup
*
*  Description:
*   Closes a connection in the correct order.
*
*  Returns: void
*
****************************************************************************/
static void CONN_LYRP_cleanup(CONN_LYRP_infoT *entryP)
{
   LOGG_PRINT(LOG_DEBUG_E, NULL, "begin cleanup\n");

   if(entryP->statusMutexFlag)
   {
      CONN_LYRP_setStatus(entryP,CONN_LYRP_DOWN_E);
   }

   if(entryP->threadTxParams.threadHandle)
   {
      CONN_LYRP_sync(entryP, CONN_LYRP_TX_SYNC);
      OS_LYRG_waitForThread(entryP->threadTxParams.threadHandle, OS_LYRG_INFINITE);
      entryP->threadTxParams.threadHandle= NULL;
      LOGG_PRINT(LOG_DEBUG_E, NULL, "closed Tx thread\n");
   }

   if(entryP->threadTxParams.event)
   {
      OS_LYRG_releaseEvent(entryP->threadTxParams.event, entryP->id);
      entryP->threadTxParams.event = NULL;
   }
  
   if(entryP->isIfUp)
   {
      if(entryP->ifCaps.flags & CLS_COMG_CAP_TIMED_RECV)
      {
         if(entryP->threadRxParams.threadHandle)
         {
            CONN_LYRP_sync(entryP, CONN_LYRP_RX_SYNC);
            OS_LYRG_waitForThread(entryP->threadRxParams.threadHandle, OS_LYRG_INFINITE);
            entryP->threadRxParams.threadHandle = NULL;
            LOGG_PRINT(LOG_DEBUG_E, NULL, "closed Rx thread\n");
         }
         entryP->ifOps.close(entryP->ifHandle);
         entryP->isIfUp = 0;
         LOGG_PRINT(LOG_DEBUG_E, NULL, "closed Interface\n");
      }
      else
      {
         //no timed receive - must force close interface to release thread
         entryP->ifOps.close(entryP->ifHandle);
         entryP->isIfUp = 0;
         LOGG_PRINT(LOG_DEBUG_E, NULL, "closed Interface\n");
      
         //Close receiver thread blocking on interface
         if(entryP->threadRxParams.threadHandle)
         {
            OS_LYRG_waitForThread(entryP->threadRxParams.threadHandle, OS_LYRG_INFINITE);
            entryP->threadRxParams.threadHandle = NULL;
            LOGG_PRINT(LOG_DEBUG_E, NULL, "closed Rx thread\n");
         }
      }
   }

   //Close TX and RX que mutex
   if(entryP->queue.mutexRxFlag)
   {
      OS_LYRG_releaseMutex(&(entryP->queue.queueMutexRx));
      entryP->queue.mutexTxFlag = FALSE;
      LOGG_PRINT(LOG_DEBUG_E, NULL,  "free tx queue mutex\n");
   }

   if(entryP->queue.mutexTxFlag)
   {
      OS_LYRG_releaseMutex(&(entryP->queue.queueMutexTx));
      entryP->queue.mutexTxFlag = FALSE;
      LOGG_PRINT(LOG_DEBUG_E, NULL,  "free tx queue mutex\n");
   }
   
   if(entryP->statusMutexFlag)
   {
      OS_LYRG_releaseMutex(&entryP->statusMutex);
      entryP->statusMutexFlag = FALSE;
      LOGG_PRINT(LOG_DEBUG_E, NULL,  "free status mutex\n");
   }

   entryP->id = CONN_LYRP_MAX_CONNECTIONS;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "cleanup done\n");
}

static ERRG_codeE CONN_LYRP_configure(CONN_LYRP_infoT *entryP,CONN_LYRG_cfgT *conCfgP)
{
   ERRG_codeE retVal = CONN_LYR__RET_SUCCESS;

   if(entryP->ifCaps.flags & CLS_COMG_CAP_TIMED_RECV)
   {
      //pause the rx and tx threads
      CONN_LYRP_setStatus(entryP, CONN_LYRP_PAUSE_E);
      CONN_LYRP_sync(entryP, CONN_LYRP_TX_SYNC);
      CONN_LYRP_sync(entryP, CONN_LYRP_RX_SYNC);
   }
   retVal = entryP->ifOps.config(entryP->ifHandle, conCfgP->ifCfg);

   return retVal;
}

/****************************************************************************
 ***************       G L O B A L     F U N C T I O N        ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: CONN_LYRG_init
*
*  Description: Initialize the CLS. Should be called once on system startup.
*   
*  Returns: ERRG_codeE - success or error. Error if unable to allocate
*     resources.
*  
*  Context: Single Context.
*  
****************************************************************************/
ERRG_codeE CONN_LYRG_init(void)
{
   ERRG_codeE  retval = CONN_LYR__RET_SUCCESS;
   UINT32      i;
   CONN_LYRP_infoT *entryP;

   CONN_LYRP_hdrMarshalSize  = CONN_LYRP_writeHeader(NULL,0,0);

   for(i = 0; i < CONN_LYRP_NELEMENTS(CONN_LYRP_connTbl); i++)
   {
      entryP = &CONN_LYRP_connTbl[i];
      retval = CONN_LYRP_initEntry(entryP);
      if(ERRG_FAILED(retval))
      {
         CONN_LYRP_deinitEntry(entryP);
         break;
      }
   }

   return retval;
}

/****************************************************************************
*
*  Function Name: CONN_LYRG_deinit
*
*  Description: Close the CLS. 
*    This will force close all open connections.
*   
*  Context: Single Context.
*  
****************************************************************************/
void CONN_LYRG_deinit(void)
{
   unsigned int i;
   CONN_LYRP_infoT *entryP;

   for(i = 0; i < CONN_LYRP_NELEMENTS(CONN_LYRP_connTbl); i++)
   {
      entryP = &CONN_LYRP_connTbl[i];
      CONN_LYRG_close(entryP);
      CONN_LYRP_deinitEntry(entryP);
   }

}

/****************************************************************************
*
*  Function Name: CONN_LYRG_open
*
*  Description: Create a new connection.
*
*  Context: multiple contexts
*
****************************************************************************/
ERRG_codeE  CONN_LYRG_open(CON_LYRG_handleT *handleP, CONN_LYRG_openCfgT *conCfgP, UINT32 connId)
{
   ERRG_codeE              retVal = CONN_LYR__RET_SUCCESS;
   CONN_LYRP_infoT   *entryP;
   UINT32 i = 0;
   int once = 0;
   //LOGG_setDbgLevel(LOG_DEBUG_E);

   //Find a free connection entry
   for(i = 0; i < CONN_LYRP_NELEMENTS(CONN_LYRP_connTbl); i++)
   {
      entryP = &CONN_LYRP_connTbl[i];
      OS_LYRG_lockMutex(&entryP->serialMutex);

      if(entryP->id == CONN_LYRP_MAX_CONNECTIONS)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "found new conn entry %d\n", i);
         entryP->id = i;

         //Reset these flags here in case of failure
         entryP->queue.mutexRxFlag    = FALSE;
         entryP->queue.mutexTxFlag    = FALSE;
         entryP->statusMutexFlag      = FALSE;

         do
         {
            if(OS_LYRG_aquireMutex(&(entryP->statusMutex)) == FAIL_E)
            {
               retVal = CONN_LYR__ERR_INIT_CLS_FAIL;
               break;
            }               
            entryP->statusMutexFlag = TRUE;
            
            //Get interface operations
            conCfgP->getIfOps(&entryP->ifOps);

            //Open interface
            retVal = entryP->ifOps.open(&entryP->ifHandle, conCfgP->ifCfg);
            if(ERRG_FAILED(retVal))
               break;
            entryP->isIfUp = 1; //Interface is up
            LOGG_PRINT(LOG_DEBUG_E, NULL, "Open interface OK\n");

            //Get interface capabilities
            entryP->ifOps.getCaps(entryP->ifHandle, &entryP->ifCaps);

            //For fixed mode set or allocate the internal memory pool
            if(!(entryP->ifCaps.flags & CLS_COMG_CAP_MODE_STREAM) && (entryP->ifCaps.flags & CLS_COMG_CAP_RX))
            {
               MEM_POOLG_cfgT cfg;
               cfg.bufferSize = entryP->ifCaps.fixedSize;
               cfg.freeArg = NULL;
               cfg.freeCb = NULL;
               cfg.numBuffers = 128; //taken from old pos_sensor definition, need to
               cfg.type = MEM_POOLG_TYPE_ALLOC_HEAP_E;
               retVal = MEM_POOLG_initPool(&entryP->fixedRxPool, &cfg);
               if (ERRG_FAILED(retVal))
                  break;
            }
            if((entryP->ifCaps.flags & CLS_COMG_CAP_MODE_UNBUFFERED_RECV) && (entryP->ifCaps.flags & CLS_COMG_CAP_RX))
            {
               MEM_POOLG_cfgT cfg;
               cfg.bufferSize = entryP->ifCaps.unbufferedFixedSize;
               cfg.freeArg = NULL;
               cfg.freeCb = NULL;
               cfg.numBuffers = 128; //taken from old pos_sensor definition, need to
               cfg.type = MEM_POOLG_TYPE_ALLOC_HEAP_E;
               retVal = MEM_POOLG_initPool(&entryP->fixedRxPool, &cfg);
               if (ERRG_FAILED(retVal))
                  break;
            }

            //Set up queue layer
            if(conCfgP->queueSize > QUEUE_LYRG_QUE_MAX_SIZE)
            {
               retVal = CONN_LYR__ERR_INIT_CLS_FAIL;
               break;
            }            
            if(OS_LYRG_aquireMutex(&(entryP->queue.queueMutexTx)) == FAIL_E)
            {
               retVal = CONN_LYR__ERR_INIT_CLS_FAIL;
               break;
            }
            entryP->queue.mutexTxFlag = TRUE;

            entryP->queue.queueSize = conCfgP->queueSize;
            entryP->queue.numTxInQue = conCfgP->queueSize; //Entire queue used for TX
            QUEUE_LYRG_init(&entryP->queue);
            entryP->timedAllocBufCb = conCfgP->timedAllocBufCb;
            entryP->commP = conCfgP->commP;

            CONN_LYRP_initCbTblEntry(&CONN_LYRP_cbFuncTbl[entryP->id]);
            entryP->cbFuncTblP = &CONN_LYRP_cbFuncTbl[entryP->id];

            LOGG_PRINT(LOG_DEBUG_E, NULL, "Queue Layer init OK (%d)\n", conCfgP->queueSize);
            //Serial mutex for serializing calls to send and other API functions.
            if ((connId >= INU_FACTORY__CONN_UVC1_E) &&
                    (connId <= INU_FACTORY__CONN_UVC_MAX_E))
                continue;
            //Initialize internal thread(s)
            entryP->threadRxParams.func = (OS_LYRG_threadFunction) CONN_LYRP_rxThread;
            entryP->threadRxParams.param = entryP;
            entryP->threadRxParams.id = OS_LYRG_CLS_RX_THREAD_ID_E;
            entryP->threadTxParams.func = (OS_LYRG_threadFunction) CONN_LYRP_txThread;
            entryP->threadTxParams.param = entryP;
            entryP->threadTxParams.id = OS_LYRG_CLS_TX_THREAD_ID_E;
            entryP->threadTxParams.event = OS_LYRG_createEvent(entryP->id);
            if(entryP->threadTxParams.event == 0)
            {
              retVal = CONN_LYR__ERR_INIT_CLS_FAIL;
              break;
            }
            LOGG_PRINT(LOG_DEBUG_E, NULL, "Event initialization OK\n");
            entryP->status = CONN_LYRP_UP_E;

            if(entryP->ifCaps.flags & CLS_COMG_CAP_RX)
            {
               LOGG_PRINT(LOG_DEBUG_E, NULL, "Launch RX\n");
               //Launch threads
               entryP->threadRxParams.threadHandle = OS_LYRG_createThread(&entryP->threadRxParams);
               if(!entryP->threadRxParams.threadHandle)
               {
                  retVal = CONN_LYR__ERR_INIT_CLS_FAIL;
                  break;
               }
               entryP->rxsync = CONN_LYRP_SYNC_LOW;
            }

            if(entryP->ifCaps.flags & CLS_COMG_CAP_TX)
            {
               LOGG_PRINT(LOG_DEBUG_E, NULL, "Launch TX\n");
               entryP->threadTxParams.threadHandle = OS_LYRG_createThread(&entryP->threadTxParams);
               if(!entryP->threadTxParams.threadHandle)
               {
                  retVal = CONN_LYR__ERR_INIT_CLS_FAIL;
                  break;
               }
               entryP->txsync = CONN_LYRP_SYNC_LOW;
            }
            entryP->chainHdr = 0;
            //LOGG_PRINT(LOG_INFO_E, NULL, "CONN_LYRP_cbFuncTbl5: CONN_LYRP_cbFuncTbl[0].cb[1] = %p, CONN_LYRP_cbFuncTbl[0].cb[1] = %p\n",CONN_LYRP_cbFuncTbl[0].cb[1],CONN_LYRP_cbFuncTbl[1].cb[1]);            
         }while(once);
         //LOGG_PRINT(LOG_INFO_E, NULL, "CONN_LYRP_cbFuncTbl6: CONN_LYRP_cbFuncTbl[0].cb[1] = %p, CONN_LYRP_cbFuncTbl[0].cb[1] = %p\n", CONN_LYRP_cbFuncTbl[0].cb[1], CONN_LYRP_cbFuncTbl[1].cb[1]);
         if(ERRG_SUCCEEDED(retVal))
         {
            *handleP = entryP;
            LOGG_PRINT(LOG_INFO_E, NULL, "Opened connection id=%d\n", entryP->id);
            OS_LYRG_unlockMutex(&entryP->serialMutex);
            break;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, retVal, "Failed open connection id=%d\n", entryP->id);
            //Failure - clean up whatever needs cleaning
            CONN_LYRP_cleanup(entryP);
         }

      }
      OS_LYRG_unlockMutex(&entryP->serialMutex);
   } //for

   if(i == CONN_LYRP_MAX_CONNECTIONS)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "no free connections\n");
      retVal = CONN_LYR__ERR_NO_FREE_CONNECTIONS; //no free connections
   }


   return retVal;
}

/****************************************************************************
*
*  Function Name: CONN_LYRG_close
*
*  Description: Close an open connection.
*   
*  Context: muliple contexts.
*  
****************************************************************************/
void CONN_LYRG_close(CON_LYRG_handleT handle)
{
   CONN_LYRP_infoT *entryP = (CONN_LYRP_infoT *)handle;

   OS_LYRG_lockMutex(&entryP->serialMutex);

   CONN_LYRP_cleanup(entryP);
      
   OS_LYRG_unlockMutex(&entryP->serialMutex);
}

ERRG_codeE CONN_LYRG_config(CON_LYRG_handleT handle, CONN_LYRG_cfgT *conCfgP)
{
   ERRG_codeE retVal = CONN_LYR__RET_SUCCESS;
   CONN_LYRP_infoT *entryP = (CONN_LYRP_infoT *)handle;

   OS_LYRG_lockMutex(&entryP->serialMutex);

   if(CONN_LYRP_IS_OPEN(entryP))
   {
      retVal = CONN_LYRP_configure(entryP, conCfgP);
   }
   OS_LYRG_unlockMutex(&entryP->serialMutex);
   return retVal;
}

/****************************************************************************
*
*  Function Name: CONN_LYRG_send
*
*  Description: Place data to send on queue 
*     
*  Outputs: void
*
*  Returns: ERRG_codeE success or error.
*
*  Context: Multiple contexts.
*
****************************************************************************/
ERRG_codeE CONN_LYRG_send(CON_LYRG_handleT handle, INT32 channelNum, void *bufferP, UINT32 txid,UINT32 msgCode)
{
   ERRG_codeE retVal = CONN_LYR__RET_SUCCESS;
   CONN_LYRP_infoT *entryP = (CONN_LYRP_infoT *)handle;
   MEM_POOLG_bufDescT *descP = (MEM_POOLG_bufDescT *)bufferP;
   UINT16 isFixed;

#ifdef INU_PROFILER_CONN_LYR
   INUG_profiler_get_timestamp();
#endif
   CONN_LYRP_statusE stat = CONN_LYRP_getStatus(entryP);
   if (CONN_LYRP_DOWN_E == stat)
   {
       return CONN_LYR__ERR_CONNECTION_IS_DOWN;
   }

   OS_LYRG_lockMutex(&entryP->serialMutex);

   if(CONN_LYRP_IS_OPEN(entryP))
   {
      //Assuming serialization of send calls so we can first check if que is full.
      if(!QUEUE_LYRG_isFull(&entryP->queue,QUEUE_LYRG_TYPE_TX_E))
      {
         isFixed = !(entryP->ifCaps.flags & CLS_COMG_CAP_MODE_STREAM);
         
         retVal = CONN_LYRP_addHdr(entryP, channelNum, &descP, isFixed, entryP->chainHdr);
         if(ERRG_SUCCEEDED(retVal))
         {
            if(isFixed)
               retVal = CONN_LYRP_fixChainLen(entryP, descP);
         }
         
         if(ERRG_SUCCEEDED(retVal))
         {
            QUEUE_LYRG_add(&entryP->queue, (void *)descP, descP->dataLen, channelNum, txid, QUEUE_LYRG_TYPE_TX_E); 
            OS_LYRG_setEvent(entryP->threadTxParams.event);
         }
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "overflow %p %d %d\n", descP, descP->dataLen,channelNum);
         entryP->stats.txQueOverflowCnt++;
         retVal = CONN_LYR__ERR_SEND_QUE_OVERFLOW;
      }
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "send packet fail. connection is down\n");
      retVal = CONN_LYR__ERR_CONNECTION_IS_DOWN;
   }

   OS_LYRG_unlockMutex(&entryP->serialMutex);

#ifdef INU_PROFILER_CONN_LYR
   INUG_profiler_get_timestamp();
#endif

   return retVal;
}

UINT32 CONN_LYRG_getHdrMarshalSize(void)
{
   return CONN_LYRP_hdrMarshalSize;
}

void CONN_LYRG_rxRegister(CON_LYRG_handleT handle, CONN_LYRG_rxCbT func, void *arg)
{
   CONN_LYRP_register((CONN_LYRP_infoT *)handle, CLS_RX, (UINT32)~0, func, arg);
}
void CONN_LYRG_txRegister(CON_LYRG_handleT handle, CONN_LYRG_txCbT func, void *arg)
{
   CONN_LYRP_register((CONN_LYRP_infoT *)handle, CLS_TX, (UINT32)~0, func, arg);
}
void CONN_LYRG_chainHdrMode(UINT16 chainHdr)
{
   CONN_LYRP_infoT *entryP;
   unsigned int i;

   for(i = 0; i < CONN_LYRP_NELEMENTS(CONN_LYRP_connTbl); i++)
   {
      entryP = &CONN_LYRP_connTbl[i];
      if (entryP->id != CONN_LYRP_MAX_CONNECTIONS)
      {
         LOGG_PRINT(LOG_INFO_E, NULL,  "Set chainhdr mode %d for entry %p\n", chainHdr,entryP); 
         entryP->chainHdr = chainHdr;
      }
   }
}

void CONN_LYRG_showStats(CON_LYRG_handleT handle, CONN_LYRG_showModeE mode)
{
   CONN_LYRP_infoT *entryP = (CONN_LYRP_infoT *)handle;

   if((mode == CONN_LYRG_RX_STATS_E) || (mode == CONN_LYRG_ALL_STATS_E))
   {
      LOGG_PRINT(LOG_INFO_E, NULL,  "Connection Rx(%d): pkt=%8u %8uB (OF=%u NOMEM=%u)\n", entryP->id, entryP->stats.rxPktCnt, entryP->stats.rxByteCnt, 
      entryP->stats.rxQueOverflowCnt, entryP->stats.outOfMem);
   }
   if((mode == CONN_LYRG_TX_STATS_E) || (mode == CONN_LYRG_ALL_STATS_E))
   {
      LOGG_PRINT(LOG_INFO_E, NULL,  "Connection Tx(%d): pkt=%8u %8uB (OF=%u ERR=%u HDRERR=%u)\n", entryP->id, entryP->stats.txPktCnt, entryP->stats.txByteCnt,
            entryP->stats.txQueOverflowCnt, entryP->stats.txPktErrCnt, entryP->stats.outOfMemHdr);
   }

   return;
}


#ifdef __cplusplus
}
#endif
