#include "inu2.h"
#include "inu_streamer.h"
#include "inu2_types.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "err_defs.h"
#include "mem_pool.h"
#include "internal_cmd.h"
#include "inu_graph.h"
#include "log.h"
#include "uvc_com.h"

#include "sys_defs.h" //temp - until sizes taken from input
#include "os_lyr.h"
#include "assert.h"
#include "lz4.h"
#include "inu_data.h"

#include "marshal.h"
#include "inu_comm.h"
#include "helsinki.h"
#ifdef __cplusplus
extern "C" {
#endif

#define INU_STREAMER__READ_TIMEOUT_MSEC (1000)
#define INU_STREAMER__WRITE_STOP_TIMEOUT_USEC (1000000)
#define INU_STREAMER__WRITE_STOP_PERIOD_SLEEP_USEC (INU_STREAMER__WRITE_STOP_TIMEOUT_USEC / 10)
#define ALIGN_DWORD(len)        ((len + 3)/4)*4
#define MAX_BUFF_DESC_NUM         10
#define CALC_RATE(curr,prev,msec) (((float)(((curr)-(prev))*1000U))/((float)msec))
#define MAX_NUM_CHANKS (1)


#define INU_STREAMER__HOST_MSGQUE_DEPTH (20)
static void inu_streamer__enque(inu_streamer *me, inu_data *data);
static ERRG_codeE inu_streamer__deque(inu_streamer *me, inu_data **data, UINT32 timeout_msec);
static void inu_streamer__deleteQue(inu_streamer *me);
static void inu_streamer__createQue(inu_streamer *me, unsigned int maxMsg, unsigned int msgSize);
UINT32 inu_comm__isUvcType(inu_ref *ref);
void CONN_LYRG_txBuf(void *entry, void *buf, INT32 chanNum, UINT32 txid);


static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_STREAMER";

typedef struct
{
   UINT32 inputCnt;
   UINT32 sentCnt;
   UINT64 sentByteCnt;
   UINT32 droppedCnt;
   UINT32 skippedCnt;
} inu_streamer__srcStats_t;

typedef struct
{
   UINT32 receivedCnt;
   UINT64 receivedByteCnt;
   UINT32 droppedCnt;
   UINT32 dequedCnt;
   UINT32 enqueCnt;
} inu_streamer__sinkStats_t;


typedef struct
{
   inu_streamer__srcStats_t  srcStats;
   inu_streamer__sinkStats_t sinkStats;
}inu_streamer__stats_t;


typedef struct
{
   inu_data                            *dataStreaming;
   UINT32                              numBuffers;
   OS_LYRG_msgQueT                      dataQue;
   inu_data                            *data;
   MEM_POOLG_handleT                   inMemPool;
   inu_streamer__direction_e           streamDirection;
   inu_streamer__buffWriteCompleteCB   *p_buffWriteCompleteCB;
   MEM_POOLG_bufDescT                  *bufDescVec[MAX_BUFF_DESC_NUM];
   void                                *cookieTable[MAX_BUFF_DESC_NUM];
   UINT32                              writeInQue;
   UINT32                              streamInIndex;
   inu_streamer__stats_t               streamStats;
   inu_streamer__stats_t               prevStreamStats;
   UINT32                              prevStatsTimeSec;
   UINT16                              prevStatsTimeMs;
   MEM_POOLG_handleT                   poolH;
   UINT32                              stopFlag;
   inu_streamer__type_e                StreamType;
   UINT8                               activeUvcStream;
   UINT8                               shutterActive;
}inu_streamer__privData;

static INT32 findEmptyBufDescVec(inu_streamer__privData *privP)
{
   UINT32 i=0;
   for (i=0;i<MAX_BUFF_DESC_NUM;++i)
   {
      if (privP->bufDescVec[i]==0)
      {
         return i;
      }
   }
   return -1;
}

#if DEFSG_IS_HOST
static INT32 findBuffDesc(inu_streamer__privData *privP, MEM_POOLG_bufDescT *buffDescP)
{
   UINT32 i=0;
   for (i=0;i<MAX_BUFF_DESC_NUM;++i)
   {
      if (buffDescP && privP->bufDescVec[i]==buffDescP)
      {
         return i;
      }
   }
   return -1;
}
#endif

static const char* inu_streamer__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_streamer__dtor(inu_ref *me)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_dtor(me);
#if DEFSG_IS_HOST
   if ((privP->streamDirection == INU_STREAMER__IN) && (privP->inMemPool))
   {
      MEM_POOLG_closePool(privP->inMemPool);
      privP->inMemPool = NULL;
   }
#else
#ifdef ENABLE_UVC
   if (privP->StreamType != INU_STREAMER__REGULAR)
   {
      UVC_MAING_SetStreamTable(NULL, privP->StreamType, INU_FACTORY__CONN_UVC1_E);
   }
#endif
#endif
   free(privP);
}

/* Constructor */
ERRG_codeE inu_streamer__ctor(inu_streamer *me, inu_streamer__CtorParams *streamerCtorParamsP)
{
   ERRG_codeE ret;
   inu_streamer__privData *privP;
   inu_factory__connIdE connId;
   ret = inu_function__ctor(&me->function, &streamerCtorParamsP->functionCtorParams);

#if DEFSG_IS_HOST && DEFSG_IS_OS_WINDOWS
   //internal implementation of msg que in windows is in c++, and requires a constructor for it
   privP = new inu_streamer__privData;
   memset(&privP->streamStats,0,sizeof(inu_streamer__stats_t));
   memset(&privP->prevStreamStats,0,sizeof(inu_streamer__stats_t));
   privP->data = NULL; //datas come before input streamer binding in second round launch, leading crash
#else
   privP = (inu_streamer__privData*)malloc(sizeof(inu_streamer__privData));
   memset(privP,0,sizeof(inu_streamer__privData));
#endif
   memset(privP->bufDescVec, 0, sizeof(privP->bufDescVec));
   privP->streamInIndex   = 0;


   privP->activeUvcStream = 0;
   privP->inMemPool       = NULL;
   privP->streamDirection = streamerCtorParamsP->streamDirection;
   privP->numBuffers      = streamerCtorParamsP->numBuffers;
   me->privP              = privP;
   connId = (inu_factory__connIdE)streamerCtorParamsP->connId;
   if ((connId >= INU_FACTORY__CONN_UVC1_E ) && (connId<=INU_FACTORY__CONN_UVC_MAX_E))
   {
      privP->StreamType = INU_STREAMER__UVC_REG;
#if DEFSG_IS_GP
      if (strcmp("Sout_vision_bokeh", inu_ref__getUserName(me)) == 0 || strcmp("Sout_vision_bokeh_1", inu_ref__getUserName(me)) == 0)
      {
         privP->StreamType = INU_STREAMER__UVC_BOKEH;
      }
#ifdef ENABLE_UVC
      UVC_MAING_SetStreamTable(me,privP->StreamType, connId);
#endif
#endif
   }
   else
   {
      privP->StreamType = INU_STREAMER__REGULAR;
   }

   if (connId < INU_FACTORY__NUM_CONNECTIONS_E)
   {
      inu_factory__changeDataComm((inu_ref*)me, connId);
   }
   else
   {
      ret = INU_STREAMER__ERR_UNEXPECTED;
   }
   return ret;
}

static int inu_streamer__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   ERRG_codeE ret;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_rxAsyncData(me, msgCode, msgP, dataP, dataLen, bufDescP);
   inu_data *data;
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   inu_streamer__sinkStats_t *streamerSinkStatsP = &privP->streamStats.sinkStats;

   streamerSinkStatsP->receivedCnt++;
   streamerSinkStatsP->receivedByteCnt += dataLen;

   if (bufDescP == NULL)
   {
      streamerSinkStatsP->droppedCnt++;
      INU_REF__LOGG_PRINT(me, LOG_WARN_E,NULL,"received data with zero length\n");
      return 0;
   }

#if DEFSG_IS_HOST
   ret = inu_data__allocInternal(privP->data,&data);

   if (ERRG_SUCCEEDED(ret))
   {
      ((inu_data__VTable*)data->node.ref.p_vtable)->p_dataHdrSet(data, msgP);
#ifdef ISP_BUFFER_ALIGN_SIZE
      if (strstr(inu_ref__getUserName(me), "isp")) {
          inu_data__metadata_sizeSet(data, METADATA_SIZE_ALIGN(inu_data__getMetadataSize(data)));
      }
#endif
      inu_data__bufDescPtrSet(data, (MEM_POOLG_bufDescT*)*bufDescP);
      inu_streamer__enque((inu_streamer*)me, data);
   }
   else
   {
      streamerSinkStatsP->droppedCnt++;
      MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
   }
#else
   if (privP->streamDirection == INU_STREAMER__IN)
   {
      inu_function__stateParam stateParam;
      inu_function__getState(me,&stateParam);
      //do the same as normal operation check
      if(stateParam.state == ENABLE)
      {
         inu_data *clone;
         data = inu_node__getNextOutputNode(me,NULL);
         ret = inu_function__newData((inu_function*)me, data, *bufDescP, msgP, &clone);
         if (ERRG_SUCCEEDED(ret))
         {
            inu_function__doneData((inu_function*)me,clone);
         }
      }
      else
      {
         printf("stream in rx, but already closed\n");
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
      }
   }
   else
   {
      ret = inu_data__allocInternal(privP->data,&data);

      if (ERRG_SUCCEEDED(ret))
      {
         ((inu_data__VTable*)data->node.ref.p_vtable)->p_dataHdrSet(data, msgP);
         inu_data__bufDescPtrSet(data, (MEM_POOLG_bufDescT*)*bufDescP);
         inu_streamer__enque((inu_streamer*)me, data);
      }
      else
      {
         streamerSinkStatsP->droppedCnt++;
         MEM_POOLG_free((MEM_POOLG_bufDescT*)*bufDescP);
      }
   }
#endif
   return 0;
}

static int inu_streamer__txDone(inu_ref *me, void *bufP)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_txDone(me,bufP);


#if DEFSG_IS_GP
   //stream out buffer sent to host
   inu_data *data;
   MEM_POOLG_bufDescT *bufDescP = ((MEM_POOLG_bufDescT*)bufP)->nextP;
   if (bufDescP)
   {
      data = privP->dataStreaming;
      privP->dataStreaming = NULL;

      if(data)
      {
         #ifdef ENABLE_HANDLE_IMMEDIATE
         inu_function__doneData_handleImmediately((inu_function*)me, data);
         #else
         inu_function__doneData((inu_function*)me, data);
         #endif
         inu_function__complete((inu_function*)me);
      }
   }
#else
   INT32 buffIndex;
   //stream in buffer sent to target
   buffIndex = findBuffDesc(privP,((MEM_POOLG_bufDescT*)bufP)->nextP);
   if (privP->p_buffWriteCompleteCB && buffIndex>-1)
   {
       privP->p_buffWriteCompleteCB(me,((MEM_POOLG_bufDescT*)bufP)->nextP->dataP, privP->cookieTable[buffIndex]);
   }
   if (buffIndex > -1)
   {
      privP->bufDescVec[buffIndex] = 0;
      if (privP->writeInQue == 0)
      {
         INU_REF__LOGG_PRINT(me,LOG_ERROR_E, NULL, "write in que counter is zero, and received cb for buffer %p (%d)\n",((MEM_POOLG_bufDescT*)bufP)->nextP,buffIndex);
         assert(0);
      }
      privP->writeInQue--;
   }
#endif

   return 0;
}


/****************************************************************************
*
*  Function Name: inu_streamer_showSrc
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: streamer
*
****************************************************************************/
static void inu_streamer__showSrc(inu_streamer *me, UINT32 periodMsec)
{
   inu_streamer__privData   *privP = (inu_streamer__privData*)me->privP;
   inu_streamer__srcStats_t stat;
   inu_streamer__srcStats_t *prevP = &privP->prevStreamStats.srcStats;

   memcpy(&stat, &privP->streamStats.srcStats, sizeof(stat)); //copy current stats locally

   LOGG_PRINT(LOG_INFO_E, NULL, "  totals: in %u sent %u(%uMB) dropped: %u skipped %u\n",
      stat.inputCnt,
      stat.sentCnt,
      (UINT32)(stat.sentByteCnt >> 20),
      stat.droppedCnt,
      stat.skippedCnt);
   LOGG_PRINT(LOG_INFO_E, NULL, "  current(%umsec): in_fps %.2f sent_fps:%.2f(%.2fMBps) sent %u(%uMB) dropped %u skipped %u\n",
      periodMsec,
      (periodMsec) ? CALC_RATE(stat.inputCnt, prevP->inputCnt, periodMsec): (~0U),
      (periodMsec) ? CALC_RATE(stat.sentCnt, prevP->sentCnt, periodMsec) : (~0U),
      (periodMsec) ? (CALC_RATE(stat.sentByteCnt, prevP->sentByteCnt, periodMsec))/(1<<20): (~0U),
      stat.sentCnt - prevP->sentCnt,
      (UINT32)((stat.sentByteCnt - prevP->sentByteCnt) >> 20),
      stat.droppedCnt - prevP->droppedCnt,
      stat.skippedCnt - prevP->skippedCnt);

   memcpy(prevP, &stat, sizeof(stat)); //save previous

}

/****************************************************************************
*
*  Function Name: inu_streamer_showSink
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: streamer
*
****************************************************************************/
static void inu_streamer__showSink(inu_streamer *me, UINT32 periodMsec)
{
   inu_streamer__privData    *privP = (inu_streamer__privData*)me->privP;
   inu_streamer__sinkStats_t stat;
   inu_streamer__sinkStats_t *prevP = &privP->prevStreamStats.sinkStats;

   memcpy(&stat, &privP->streamStats.sinkStats, sizeof(stat)); //copy current stats locally

   LOGG_PRINT(LOG_INFO_E, NULL, "  totals: received %u(%uMB) dropped: %u dequed %u\n",
      stat.receivedCnt,
      (UINT32)(stat.receivedByteCnt >> 20),
      stat.droppedCnt,
      stat.dequedCnt);
   LOGG_PRINT(LOG_INFO_E, NULL, "  current(%umsec): fps=%.2f(%.2fMBps) received %u(%uMB) dropped %u dequed %u enqued %u\n",
      periodMsec,
      (periodMsec) ? CALC_RATE(stat.receivedCnt, prevP->receivedCnt, periodMsec): (~0U),
      (periodMsec) ? (CALC_RATE(stat.receivedByteCnt, prevP->receivedByteCnt, periodMsec))/(1<<20): (~0U),
      stat.receivedCnt - prevP->receivedCnt,
      (UINT32)((stat.receivedByteCnt - prevP->receivedByteCnt) >> 20),
      stat.droppedCnt - prevP->droppedCnt,
      stat.dequedCnt - prevP->dequedCnt,
      stat.enqueCnt - prevP->enqueCnt);

   memcpy(prevP, &stat, sizeof(stat)); //save previous
}


static void inu_streamer__showStats(inu_ref *me)
{
   inu_streamer *streamer = (inu_streamer*)me;
   inu_streamer__privData *privP = (inu_streamer__privData*)streamer->privP;
   inu_function__vtable_get()->node_vtable.ref_vtable.p_showStats(me);
   UINT32         curTimeDurationMs = 0;

   curTimeDurationMs = OS_LYRG_deltaMsec(privP->prevStatsTimeSec,  privP->prevStatsTimeMs);
   OS_LYRG_getTime(&privP->prevStatsTimeSec, &privP->prevStatsTimeMs);  //Remember current time for next call

   if (privP->streamDirection == INU_STREAMER__IN)
   {
#if DEFSG_IS_HOST
      inu_streamer__showSrc(streamer, curTimeDurationMs);
#else
      inu_streamer__showSink(streamer, curTimeDurationMs);
#endif
   }
   else
   {
#if DEFSG_IS_HOST
      inu_streamer__showSink(streamer, curTimeDurationMs);
#else
      inu_streamer__showSrc(streamer, curTimeDurationMs);
#endif
   }
}

static void inu_streamer__newInput(inu_node *me, inu_node *input)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   UINT32 memSize = 0;
#if DEFSG_IS_HOST
   MEM_POOLG_typeE memType = MEM_POOLG_TYPE_ALLOC_HEAP_E;
#else
   MEM_POOLG_bufDescT *bufDescP;
#endif

   inu_function__vtable_get()->node_vtable.p_newInput(me,input);
   privP->data = (inu_data*)input;
#ifdef ISP_BUFFER_ALIGN_SIZE
   if (strstr(inu_ref__getUserName(me), "isp")) {
       inu_data__metadata_sizeSet(privP->data, METADATA_SIZE_ALIGN(inu_data__getMetadataSize(privP->data)));
   }
#endif
#if DEFSG_IS_HOST
   //UINT32 metadataSize = inu_data__getMetadataSize(privP->data);
   memSize = ALIGN_DWORD(inu_data__chunkSizeGet(privP->data) + inu_data__getMetadataSize(privP->data) + MARSHALG_getMarshalSize((INUG_ioctlCmdE)((inu_data__VTable*)(privP->data->node.ref.p_vtable))->p_hdrMsgCodeGet(privP->data)) + inu_comm__getFullHdrMarshalSize()+HELSINKI_USB3_EXTRA_BUFFER_PADDING_SIZE);
   if (inu_device__useShareMem(inu_ref__getDevice(&me->ref)))
      memType=MEM_POOLG_TYPE_ALLOC_SHARED_E;


   //printf("memsize: %d, metadataSize: %d, memType: %d\n", memSize, metadataSize, memType);
   inu_ref__addMemPool((inu_ref*)me, memType, memSize, privP->numBuffers * inu_data__chunkTotalNumGet(privP->data),NULL, 0, &privP->poolH); //todo: allow user to provide memory for pool
   //printf("numBuffers: %p, chunkNum: %d, stream->privP->poolH: %p\n", privP->numBuffers, inu_data__chunkTotalNumGet(privP->data), privP->poolH);
   inu_streamer__createQue((inu_streamer*)me,privP->numBuffers * inu_data__chunkTotalNumGet(privP->data),sizeof(inu_data*));
   if (privP->streamDirection == INU_STREAMER__OUT)
   {
      inu_data__allocInternalDuplicatePool(privP->data, privP->numBuffers * inu_data__chunkTotalNumGet(privP->data) * MAX_NUM_CHANKS);
   }
#else
   ERRG_codeE ret;
   if (privP->streamDirection == INU_STREAMER__IN)
   {
      memSize = ALIGN_DWORD(MARSHALG_getMarshalSize((INUG_ioctlCmdE)((inu_data__VTable*)(privP->data->node.ref.p_vtable))->p_hdrMsgCodeGet(privP->data)) + inu_comm__getFullHdrMarshalSize());

      //check if inu_ref's pool is big enough to hold this channel's header
      ret = inu_ref__allocBuf((inu_ref*)me, memSize, &bufDescP);
      if (ERRG_FAILED(ret))
      {
         //printf("failed to get memory! memSize needed = %d\n",memSize);
         ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_HEAP_E, memSize, privP->numBuffers,NULL, 0, &privP->poolH);
         if (ERRG_FAILED(ret))
         {
            assert(0);
         }
      }
      else
      {
         MEM_POOLG_free(bufDescP);
      }
   }
   else
   {
      inu_streamer__createQue((inu_streamer*)me,privP->numBuffers * inu_data__chunkTotalNumGet(privP->data),sizeof(inu_data*));
   }
#endif
}

static void inu_streamer__newOutput(inu_node *me, inu_node *output)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   inu_function__vtable_get()->node_vtable.p_newOutput(me,output);
   inu_data *data = (inu_data*)output;

#if DEFSG_IS_HOST
   if (privP->streamDirection == INU_STREAMER__IN)
   {
      MEM_POOLG_cfgT poolCfg;
      privP->data = data;
      poolCfg.bufferSize = inu_data__sizeGet(data);
      poolCfg.numBuffers = privP->numBuffers;
      poolCfg.resetBufPtrInAlloc = 0;
      poolCfg.freeCb = NULL;
      poolCfg.memP = NULL;
      poolCfg.type = (MEM_POOLG_typeE)MEM_POOLG_TYPE_ALLOC_USER_E; //always use user memory, zero memcpy
      MEM_POOLG_initPool(&(privP->inMemPool), &poolCfg);
      inu_data__allocInternalDuplicatePool(privP->data, privP->numBuffers);
   }
#else
   MEM_POOLG_handleT tempH;
   UINT32 memSize = 0;
   privP->data = data;

   //if streamer is in, and we have new output, then we create the pool
   if (privP->streamDirection == INU_STREAMER__IN )
   {
      memSize = ALIGN_DWORD(inu_data__chunkSizeGet(data) + MARSHALG_getMarshalSize(((inu_data__VTable*)(data->node.ref.p_vtable))->p_hdrMsgCodeGet(data)) + inu_comm__getFullHdrMarshalSize());
      //printf("adding mempool: memSize = %d, data = %d, marshal = %d, privP->numBuffers = %d\n",memSize,inu_data__sizeGet(data),MARSHALG_getMarshalSize(((inu_data__VTable*)(data->node.ref.p_vtable))->p_hdrMsgCodeGet(data)),privP->numBuffers);
      inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_CMEM_E, memSize, privP->numBuffers * inu_data__chunkTotalNumGet(privP->data), NULL, 0, &tempH);
   }
#endif
}

static void inu_streamer__removeInput(inu_node *me, inu_node *input)
{
   inu_function__vtable_get()->node_vtable.p_removeInput(me,input);
   inu_streamer__deleteQue((inu_streamer*)me);
}

#if DEFSG_IS_GP


// inu_function table
static void inu_streamer__operate(inu_function *me, inu_function__operateParamsT *paramsP)
{
   inu_data__hdr_t        *hdrP;
   inu_data               *input = (inu_data*)paramsP->dataInputs[0];
   MEM_POOLG_bufDescT     *bufP;
   ERRG_codeE             ret=INU_STREAMER__RET_SUCCESS;
   UINT32                 msgCode;
   TuvcRlsParams          uvcParams;

   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   inu_function__vtable_get()->p_operate(me, paramsP);

   //streamer supports only one input for now
   if (paramsP->dataInputsNum != 1)
      assert(0);

   if (privP->streamDirection == INU_STREAMER__OUT )
   {
      inu_streamer__srcStats_t *streamerSrcStatsP = &privP->streamStats.srcStats;
      streamerSrcStatsP->inputCnt++;

      msgCode = ((inu_data__VTable*)input->node.ref.p_vtable)->p_hdrMsgCodeGet(input);
      hdrP = ((inu_data__VTable*)input->node.ref.p_vtable)->p_dataHdrGet(input);
      inu_data__bufDescPtrGet(input,&bufP);

      if (bufP == NULL)
      {
         printf("error!, bufP is null at streamer operate\n");
         assert(0);
      }

      //In host mode, then this should not happened - as the conn_lyr callback is used for
      //marking complete on the function. But, on standalone mode, when the data is enqued
      //for the user to read, then the function is comeplete. So this case should not cause an
      //assert
      //if (privP->dataStreaming)
      //{
      //   printf("privP->dataStreaming not null!\n");
      //   assert(0);
      //}

      privP->dataStreaming = input;
      //printf("%s operation! bufP = %p, hdrP = %p, index %llu, len = %d\n", inu_ref__getUserName(me), bufP, hdrP,hdrP->dataIndex,bufP->dataLen);
      if (inu_ref__standAloneMode(me))
      {

          if (inu_comm__isUvcType((inu_ref *)me) )
          {
             if ((inu_streamer__isUvcActive(me)) &&
                (inu_streamer__isShtterActive(me) == 0))
             {
             unsigned int buffer_data,buf_size=inu_data__chunkSizeGet(input);
             void *buffer_pointer = NULL;

             buffer_data=(unsigned int)inu_data__getMemPtr(input);
             buffer_pointer = (void *) buffer_data;

             uvcParams.stream = me;
             uvcParams.data = input;
             uvcParams.buffer_pointer = buffer_pointer;
             uvcParams.bufSize = buf_size;
             CONN_LYRG_txBuf(((inu_comm *)inu_ref__getDataComm((inu_ref *)me))->conn,
                  (void *)&uvcParams,me->node.ref.id,0);
             }
             else
             {
                inu_function__complete((inu_function*)me);
                inu_streamer__free(me, input);
             }
          }
          else
          {

             //In standalone mode, when the data is available for the user, enque and the function is complete
             inu_streamer__enque((inu_streamer*)me, input);
             ret = inu_function__complete((inu_function*)me);
          }
      }
      else
      {
         //duplicate because conn_lyr will free
         MEM_POOLG_duplicate(bufP);
         ret = inu_ref__sendDataAsync((inu_ref*)me, msgCode, hdrP, bufP);
      }
      if (ERRG_FAILED(ret))
      {
         streamerSrcStatsP->droppedCnt++;
      }
      else
      {
         streamerSrcStatsP->sentCnt++;
         streamerSrcStatsP->sentByteCnt += MEM_POOLG_getChainLen(bufP);
      }
   }
}
#endif

static ERRG_codeE inu_streamer__start(inu_function *me, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   ret = inu_function__vtable_get()->p_start(me, startParamP);
   privP->stopFlag = 0;
   privP->writeInQue = 0;
   OS_LYRG_getTime(&privP->prevStatsTimeSec, &privP->prevStatsTimeMs);  //Take time for statistics
   return ret;
}

static ERRG_codeE inu_streamer__stop(inu_function *me, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)me)->privP;
   int safetyTimeout = INU_STREAMER__WRITE_STOP_TIMEOUT_USEC;

   //set stopping flag, and wait until all Tx in que has completed
   if (privP->streamDirection == INU_STREAMER__IN)
   {
      privP->stopFlag = 1;
   }

   while((privP->writeInQue) && (safetyTimeout >= 0))
   {
      INU_REF__LOGG_PRINT(me,LOG_WARN_E, NULL, "Tx (%d) in que, waiting. safetyTimeout = %d\n",privP->writeInQue,safetyTimeout);
      OS_LYRG_usleep(INU_STREAMER__WRITE_STOP_PERIOD_SLEEP_USEC);
      safetyTimeout -= INU_STREAMER__WRITE_STOP_PERIOD_SLEEP_USEC;
   }

   if (safetyTimeout <= 0)
   {
      INU_REF__LOGG_PRINT(me,LOG_ERROR_E, NULL, "Tx (%d) in que, waiting failed!\n",privP->writeInQue,safetyTimeout);
   }

   ret = inu_function__vtable_get()->p_stop(me, stopParamP);

   return ret;
}

void inu_streamer__vtable_init(void)
{
   if (!_bool_vtable_initialized) {
      inu_function__vtableInitDefaults(&_vtable);
      _vtable.node_vtable.ref_vtable.p_name = inu_streamer__name;
      _vtable.node_vtable.ref_vtable.p_dtor = inu_streamer__dtor;
      _vtable.node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_streamer__ctor;
      _vtable.node_vtable.ref_vtable.p_rxAsyncData = inu_streamer__rxData;
      _vtable.node_vtable.ref_vtable.p_txDone = inu_streamer__txDone;
      _vtable.node_vtable.ref_vtable.p_showStats = inu_streamer__showStats;

      _vtable.node_vtable.p_newInput = inu_streamer__newInput;
      _vtable.node_vtable.p_newOutput = inu_streamer__newOutput;
      _vtable.node_vtable.p_removeInput = inu_streamer__removeInput;

#if DEFSG_IS_GP
      _vtable.p_operate = inu_streamer__operate;
#endif
      _vtable.p_start = inu_streamer__start;
      _vtable.p_stop = inu_streamer__stop;

      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_streamer__vtable_get(void)
{
   inu_streamer__vtable_init();
   return (const inu_function__VTable*)&_vtable;
}


static void inu_streamer__createQue(inu_streamer *me, unsigned int maxMsg, unsigned int msgSize)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)me->privP;
   int status;

   char queName[32];
   char stId[8];

   memset(queName,0,sizeof(queName));
   memset(stId, 0, sizeof(stId));
   strcpy(queName,"/");
   strcat(queName, me->function.node.ref.p_vtable->p_name((inu_ref*)me));
   sprintf(stId,"%x", me->function.node.ref.id);
   strncat(queName, stId, sizeof(queName)-strlen(me->function.node.ref.p_vtable->p_name((inu_ref*)me))-1);
   //strcat(privP->dataQue.name, queName);
   memcpy(privP->dataQue.name, queName, strlen(queName)+1);
   privP->dataQue.maxMsgs = maxMsg;
   privP->dataQue.msgSize = msgSize;
   status = OS_LYRG_createMsgQue(&privP->dataQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E);
   if(status != SUCCESS_E)
   {
      printf("que create failed: que %s maxMsg=%d msgSize=%d\n", privP->dataQue.name, maxMsg, msgSize);
   }
}


static void inu_streamer__deleteQue(inu_streamer *me)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)me->privP;
   inu_data *clone;
   INT32 status;
   //empty the que and free the inu_datas
   while (inu_streamer__deque((inu_streamer*)me,&clone,0) == 0)
   {
      inu_data__freeInternal(clone);
   }
   status = OS_LYRG_deleteMsgQue(&privP->dataQue);
   if(status != SUCCESS_E )
      INU_REF__LOGG_PRINT(me,LOG_WARN_E, NULL, "warning OS_LYRG_deleteMsgQue failed\n");
}


static void inu_streamer__enque(inu_streamer *me, inu_data *data)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)me->privP;
   inu_streamer__sinkStats_t *streamerSinkStatsP = &privP->streamStats.sinkStats;
   int status;
   //printf("enque %p\n",data);
   status = OS_LYRG_sendMsg(&privP->dataQue, (UINT8*)&data, sizeof(data));
   if (status == FAIL_E)
   {
      streamerSinkStatsP->droppedCnt++;
      ((inu_data__VTable*)data->node.ref.p_vtable)->p_freeData(data);
   }
   else
   {
      streamerSinkStatsP->enqueCnt++;
   }
}

static ERRG_codeE inu_streamer__deque(inu_streamer *me, inu_data **data, UINT32 timeout_msec)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)me->privP;
   inu_streamer__sinkStats_t *streamerSinkStatsP = &privP->streamStats.sinkStats;
   ERRG_codeE ret = INU_STREAMER__RET_SUCCESS;
   int status;
   UINT32 len;

   len = sizeof(data);
   status = OS_LYRG_recvMsg(&privP->dataQue,(UINT8*)data,&len,timeout_msec);
   if ((status == SUCCESS_E)&& (len > 0))
   {
      streamerSinkStatsP->dequedCnt++;
      //decompress if needed
      if (inu_data__isCompressed(*data))
      {
         MEM_POOLG_bufDescT *newBufP,*origBufP;
         inu_data__bufDescPtrGet(*data,&origBufP);
         ret = inu_ref__allocBuf((inu_ref*)me,origBufP->size,&newBufP);
         if (ERRG_SUCCEEDED(ret))
         {
            newBufP->dataLen = LZ4_decompress_safe((const char*)origBufP->dataP,(char*)newBufP->dataP,origBufP->dataLen,newBufP->size);
            if (newBufP->dataLen)
            {
               inu_data__bufDescPtrSet(*data,newBufP);
               MEM_POOLG_free(origBufP);
            }
         }
      }
   }
   else
   {
      ret = INU_STREAMER__ERR_TIMEOUT;
   }

   return ret;
}


void inu_streamer__setBufWriteCB(inu_streamerH meH, inu_streamer__buffWriteCompleteCB writeBufCB)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;
   privP->p_buffWriteCompleteCB = writeBufCB;
}

ERRG_codeE inu_streamer__read(inu_streamerH meH, inu_dataH *dataH_p)
{
   ERRG_codeE ret;
   ret = inu_streamer__deque((inu_streamer*)meH, (inu_data**)dataH_p, INU_STREAMER__READ_TIMEOUT_MSEC);
   //printf("%s (%d) read: %p\n", ref->p_vtable->p_name(ref), ref->id,*dataH_p);
   return ret;
}

ERRG_codeE inu_streamer__write(inu_streamerH meH, char *buf, int len, inu_data__hdr_t *hdrP, void *cookie)
{
   ERRG_codeE               ret;
   UINT32                   msgCode;
   inu_streamer             *streamer = (inu_streamer*)meH;
   MEM_POOLG_bufDescT       *bufP;
   inu_streamer__privData   *privP = (inu_streamer__privData*)streamer->privP;
   inu_streamer__srcStats_t *streamerSrcStatsP = &privP->streamStats.srcStats;
   MEM_POOLG_handleT        memPool;
   INT32                    buffDescIndex=0;

#if DEFSG_IS_HOST
   if (privP->stopFlag)
   {
      return INU_STREAMER__ERR_ILLEGAL_STATE;
   }
#endif
   streamerSrcStatsP->inputCnt++;

   buffDescIndex = findEmptyBufDescVec(privP);
   if (buffDescIndex == -1)
   {
      streamerSrcStatsP->skippedCnt++;
      INU_REF__LOGG_PRINT(streamer,LOG_ERROR_E, NULL, "failed to find free buffer desc place holder\n");
      return INU_STREAMER__ERR_OUT_OF_RSRCS;
   }
   privP->cookieTable[buffDescIndex] = cookie;
   msgCode = ((inu_data__VTable*)privP->data->node.ref.p_vtable)->p_hdrMsgCodeGet(privP->data);
   if (!hdrP)
   {
      hdrP = (inu_data__hdr_t*)((inu_data__VTable*)privP->data->node.ref.p_vtable)->p_dataHdrGet(privP->data);
   }
   memPool = privP->inMemPool;

   MEM_POOLG_alloc(memPool,len,&bufP);
   if (bufP == NULL)
   {
      streamerSrcStatsP->droppedCnt++;
      INU_REF__LOGG_PRINT(streamer,LOG_ERROR_E, NULL, "failed to allocate buffer. len = %d, (%d,%d,%d)\n",
                           len, MEM_POOLG_getBufSize(memPool), MEM_POOLG_getNumBufs(memPool), MEM_POOLG_getNumFree(memPool));
      return INU_STREAMER__ERR_OUT_OF_RSRCS;
   }

   privP->bufDescVec[buffDescIndex] = bufP;
   hdrP->dataIndex = privP->streamInIndex++;
   OS_LYRG_getUsecTime(&hdrP->timestamp);

   bufP->dataP = (UINT8*)buf;
   bufP->dataLen = len;

   ret = inu_ref__sendDataAsync((inu_ref*)meH, msgCode, hdrP, bufP);
   if (ERRG_FAILED(ret))
   {
      streamerSrcStatsP->droppedCnt++;
      privP->bufDescVec[buffDescIndex] = 0;
      MEM_POOLG_free(bufP);
   }
   else
   {
      privP->writeInQue++;
      streamerSrcStatsP->sentCnt++;
      streamerSrcStatsP->sentByteCnt += len;
   }

   return ret;
}

ERRG_codeE inu_streamer__free(inu_streamerH meH, inu_dataH dataH)
{
#if DEFSG_IS_GP
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer*)meH)->privP;
//   MEM_POOLG_bufDescT *bufP;

   //In standalone mode, when the user release the buffer, then the data can continue
   privP->dataStreaming = NULL;
   //since we duplicate the buffer incase its host mode to handle the conn_lyr, we must do it also here
   //inu_data__bufDescPtrGet(dataH,&bufP);
   //MEM_POOLG_free(bufP);
   inu_function__doneData((inu_function*)meH, dataH);
#else
   FIX_UNUSED_PARAM_WARN(meH);
   inu_data__freeInternal((inu_data*)dataH);
#endif
   //inu_data__freeInternal((inu_data*)dataH);
   return INU_STREAMER__RET_SUCCESS;
}

inu_streamer__type_e inu_streamer__getStreamType(inu_streamerH meH)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;
   return privP->StreamType;
}

UINT8 inu_streamer__isUvcActive(inu_streamerH meH)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;
   return privP->activeUvcStream;
}

#if DEFSG_IS_GP
UINT8 inu_streamer__isShtterActive(inu_streamerH meH)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;
   return privP->shutterActive;
}
#endif

void inu_streamer__setResetUvcActive(inu_streamerH meH,unsigned int activeFlag)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;
   privP->activeUvcStream = activeFlag;
}

void inu_streamer__setShtterActive(inu_streamerH meH,unsigned int activeFlag)
{
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;
   privP->shutterActive = activeFlag;
}

ERRG_codeE inu_streamer__getPoolCfg(inu_streamerH meH,inu_streamer_poolCfg *poolCfg)
{
   ERRG_codeE ret=INU_STREAMER__RET_SUCCESS;
   inu_streamer__privData *privP = (inu_streamer__privData*)((inu_streamer *)meH)->privP;

   poolCfg->refId      = ((inu_ref *)meH)->id;
   poolCfg->numBuffers = privP->numBuffers * inu_data__chunkTotalNumGet(privP->data) * MAX_NUM_CHANKS;
   poolCfg->bufferSize = ALIGN_DWORD(inu_data__chunkSizeGet(privP->data) + MARSHALG_getMarshalSize((INUG_ioctlCmdE)((inu_data__VTable*)(privP->data->node.ref.p_vtable))->p_hdrMsgCodeGet(privP->data)) + inu_comm__getFullHdrMarshalSize());
   poolCfg->bufferSize += MEM_POOLG_getNumReservedBytes();
   // modified by david @ 2022.11.08 to make the buffer include metadata content.
#if defined(METADATA)
#ifdef ISP_BUFFER_ALIGN_SIZE 
   if (strstr(inu_ref__getUserName(meH), "isp")) {
       poolCfg->bufferSize += METADATA_SIZE_ALIGN(inu_data__getMetadataSize(privP->data));
   } else {
       poolCfg->bufferSize += inu_data__getMetadataSize(privP->data);
   }
#else
   poolCfg->bufferSize += inu_data__getMetadataSize(privP->data);
#endif
   
#endif
   poolCfg->hdrSize = MEM_POOLG_getNumReservedBytes() + MARSHALG_getMarshalSize((INUG_ioctlCmdE)((inu_data__VTable*)(privP->data->node.ref.p_vtable))->p_hdrMsgCodeGet(privP->data)) + inu_comm__getFullHdrMarshalSize();

   return ret;
}

#ifdef __cplusplus
}
#endif
