#include "inu2_types.h"
#include "inu2_internal.h"
#include "err_defs.h"
#include "internal_cmd.h"
#include "inu2.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "os_lyr.h" //operate printf
#include "assert.h"

#include "inu_graph.h"
#include "log.h"
#if DEFSG_IS_GP
#include "inu_alt.h"
#include "data_base.h"
#include "icc.h"
#include "cmem.h"
#include "sequence_mngr.h"
#include "pmm_mngr.h"
#include "helsinki.h"
#endif

//#define DEBUG_INPUT_QUE

#define INU_FUNCTION__MAX_WORK_STAT_TIME (0xFFFFFFFF)
#define INU_FUNCTION__DSP_SYNC_TIMEOUT_MSEC (1000)

typedef struct element_t {
    void *val;
    struct element_t *next;
} element_t;

typedef struct
{
   struct element_t *head;
   struct element_t *freeListHead;
   OS_LYRG_mutexT   qMutex;
   int freeCntr;
}queT;

typedef struct
{
   UINT32                     inputDataQueTotalEnqueCtr;
   UINT32                     inputDataQueFullCtr;
}dataQueStatsT;

typedef struct dataQueT
{
   queT *qP;
   inu_data *data;
   struct dataQueT *next;
   dataQueStatsT stats;
}dataQueT;

typedef struct
{
   UINT64                     aveWorkTimeUsec;
   UINT64                     maxWorkTimeUsec;
   UINT64                     minWorkTimeUsec;
   UINT64                     totalWorkTimeUsec;
   UINT64                     startWorkTimeUsec;
}inu_function__workStatsT;

typedef struct
{
   UINT64                     operatedCtr;
   inu_function__workStatsT   gpWorkStats;
   inu_function__workStatsT   dspWorkStats;

   /* average time until function received done from dsp */
   UINT64                     aveDspDoneTimeUsec;

   /* data usage statistics */
   UINT32                     dataDropped;

   UINT32                     newDataCtr;
   UINT32                     newDataTotalDroppedCtr;
   UINT32                     newDataOomCtr;

   UINT32                     dataForwardFailCtr;

   UINT32                     invokeOperateFailedCtr;

   UINT32                     doneDataCtr;
   UINT32                     doneDataTotalDroppedCtr;

   UINT32                     gpToDspCtr;
   UINT32                     gpToDspTotalDroppedCtr;
   UINT32                     gpToDspIccFailCtr;
   UINT32                     gpToDspJobDescOomCtr;
}inu_function__statsT;

#if DEFSG_IS_GP
typedef struct inu_function__thrdParams
{
   int                  id;
   OS_LYRG_threadHandle thrHandle;
   OS_LYRG_msgQueT      thrWorkMsgQue;
   volatile int         active;
   volatile int         thrActive;
}inu_function__thrdParams;
#endif //DEFSG_IS_GP

typedef struct
{
   inu_function__stateParam       stateParam;
   UINT32                         operatePipeMaxSize;
   UINT32                         operatePipeCount;
   UINT32                         operateModes;
   UINT32                         syncedFunc;
   inu_function__statsT           functionStats;

#if DEFSG_IS_GP
   dataQueT                       *queListHeadP;
   UINT32                         inputQueDepth;
   UINT32                         minInptsToOprt;
   inu_function__thrdParams       *workThrdParams;
   inu_function__workThrdPriority workPriority;
#endif
}inu_function__privData;

#define INU_FUNCTION__ACCESS_P(me)          ((inu_function*)me)
#define INU_FUNCTION__PRIVDATA_ACCESS_P(me) ((inu_function__privData*)((INU_FUNCTION__ACCESS_P(me))->privP))

static inu_function__VTable _vtable;
static bool _bool_vtable_initialized = 0;
static const char* name = "INU_FUNCTION";

#if DEFSG_IS_GP
typedef enum
{
   OPERATE,
   DSP_ACK
}inu_function__workMsgType;

typedef struct
{
   inu_function__operateParamsT operateParams;
}inu_function__functionOperateMsg;

typedef struct
{
   inu_function__operateParamsT operateParams;
   UINT32                       dspSource;
}inu_function__functionDspAckMsg;

typedef union
{
   inu_function__functionOperateMsg functionOperateMsg;
   inu_function__functionDspAckMsg  dspAckMsg;
}inu_function__workMsgParamU;

typedef struct
{
   inu_function                *function;
   inu_function__workMsgParamU msgParam;
   inu_function__workMsgType   msgType;
}inu_function__workMsg;

#define INU_FUNCTION__WORK_THREAD_NUM (3)
static const char* inuthrWorkMsgQueSuffixName[INU_FUNCTION__WORK_THREAD_NUM] = {"work_q1", "work_q2", "work_q3"};

static dataQueT *inu_function__getInputDataQue(inu_function *me, inu_data *data);
static void inu_function__emptyInputQue(dataQueT *dataQueP);
static void inu_function__removeInputQue(inu_function *me, dataQueT *dataQueP);

/***************************************************************/

static int queFull(queT *qP)
{
   //printf("queFull? free list = %p, free cntr = %d\n",qP->freeListHead,qP->freeCntr);
   return (qP->freeListHead) ? 0 : 1;
}

static void enqueue(queT *qP, void *val)
{
   element_t *new_element;

   OS_LYRG_lockMutex(&qP->qMutex);
   if (qP->freeListHead == NULL)
   {
      printf("que is full!\n");
      OS_LYRG_unlockMutex(&qP->qMutex);
      return;
   }

   new_element = qP->freeListHead;
   qP->freeListHead = qP->freeListHead->next;
   qP->freeCntr--;

   new_element->val = val;
   new_element->next = qP->head;

   qP->head = new_element;
   OS_LYRG_unlockMutex(&qP->qMutex);
}

//todo: optimize the deque, can hold pointer to one before last
static void *dequeue(queT *qP)
{
   element_t *current, *prev = NULL;
   void *retval = NULL;

   OS_LYRG_lockMutex(&qP->qMutex);

   if (qP->head == NULL)
   {
      OS_LYRG_unlockMutex(&qP->qMutex);
      return NULL;
   }

   current = qP->head;
   while (current->next != NULL)
   {
     prev = current;
     current = current->next;
   }

   retval = current->val;
   current->next = qP->freeListHead;
   qP->freeListHead = current;
   qP->freeCntr++;

   if (prev)
   {
     prev->next = NULL;
   }
   else
   {
     qP->head = NULL;
   }

   OS_LYRG_unlockMutex(&qP->qMutex);
   return retval;
}

static void *peek(queT *qP)
{
   return qP->head;
}

static queT *createQ(int size)
{
   int i;
   queT *qP;
   element_t *new_element;

   if (!size)
   {
      return NULL;
   }

   qP = (queT*)malloc(sizeof(queT));
   if (!qP)
   {
      return NULL;
   }
   memset(qP,0,sizeof(queT));

   for (i = 0; i < size; i++)
   {
      new_element = (element_t*)malloc(sizeof(element_t));
      new_element->next = qP->freeListHead;
      qP->freeListHead = new_element;
      qP->freeCntr++;
   }

   OS_LYRG_aquireMutex(&qP->qMutex);

   return qP;
}

static void freeQ(queT *qP)
{
   element_t *temp;
   OS_LYRG_lockMutex(&qP->qMutex);

   while (qP->head)
   {
      temp = qP->head;
      qP->head = qP->head->next;
      free(temp);
   }

   while (qP->freeListHead)
   {
      temp = qP->freeListHead;
      qP->freeListHead = qP->freeListHead->next;
      free(temp);
   }
   OS_LYRG_unlockMutex(&qP->qMutex);
   OS_LYRG_releaseMutex(&qP->qMutex);
   free(qP);
}

/****************************************************************************
*
*  Function Name: inu_function__handleWorkMessage
*
*  Description:
*               1.
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
static void inu_function__handleWorkMessage(inu_function__workMsg *msgP)
{
   inu_function *function = msgP->function;

   switch (msgP->msgType)
   {
      case(OPERATE):
      {
         ((inu_function__VTable*)(function->node.ref.p_vtable))->p_operate(function, &msgP->msgParam.functionOperateMsg.operateParams);
         break;
      }

      case(DSP_ACK):
      {
         ((inu_function__VTable*)(function->node.ref.p_vtable))->p_dspAck(function, &msgP->msgParam.dspAckMsg.operateParams, msgP->msgParam.dspAckMsg.dspSource);
         break;
      }
   }
}

/****************************************************************************
*
*  Function Name: inu_function__workThread
*
*  Description:   The thread purpose is to execute the operate of a function
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
static void inu_function__workThread(void *arg)
{
   inu_function__workMsg msg;
   int ret;
   inu_function *me = (inu_function*)arg;
   inu_function__thrdParams *workThrParamsP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workThrdParams;
   UINT32 len;

   workThrParamsP->thrActive = 1;

   while (workThrParamsP->active)
   {
      len = sizeof(inu_function__workMsg);
      ret = OS_LYRG_recvMsg(&workThrParamsP->thrWorkMsgQue, (UINT8*)&msg, &len, 300);
      if ((ret == SUCCESS_E) && (len == sizeof(inu_function__workMsg)))
      {
         inu_function__handleWorkMessage(&msg);
      }
      else
      {
         //if not timeout, send debug msg
         if (len != 0)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "inu_function__workThread mq receive fail len = %d\n", len);
         }
      }
   }

   workThrParamsP->thrActive = 0;
}

/****************************************************************************
*
*  Function Name: inu_function__initWorkThread
*
*  Description:   init the function work thread and its queue.
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
static ERRG_codeE inu_function__initWorkThread(inu_function *me, unsigned int thrdPriority)
{
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;
   OS_LYRG_threadParams thrdParams;
   char temp[256];
   inu_function__thrdParams *workThrdParamsP;

   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workThrdParams = (inu_function__thrdParams *)malloc(sizeof(inu_function__thrdParams));
   workThrdParamsP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workThrdParams;
   if (!workThrdParamsP)
   {
      ret = INU_FUNCTION__ERR_OUT_OF_MEM;
   }
   else
   {
      memset(&workThrdParamsP->thrWorkMsgQue, 0, sizeof(workThrdParamsP->thrWorkMsgQue));
      snprintf(temp, 256, "/%s__%s", inu_ref__getUserName(me), inuthrWorkMsgQueSuffixName[thrdPriority]);
      memcpy(workThrdParamsP->thrWorkMsgQue.name, temp, MIN(OS_LYRG_MAX_MQUE_NAME_LEN, strlen(temp) + 1));
      workThrdParamsP->thrWorkMsgQue.maxMsgs = 100;
      workThrdParamsP->thrWorkMsgQue.msgSize = sizeof(inu_function__workMsg);

      LOGG_PRINT(LOG_INFO_E, NULL, "Creating work thread: [%s]\n", workThrdParamsP->thrWorkMsgQue.name);

      ret = (ERRG_codeE)OS_LYRG_createMsgQue(&workThrdParamsP->thrWorkMsgQue, 0);
      if (ret)
      {
         printf("failed to create msgQ\n");
         ret = INU_FUNCTION__ERR_UNEXPECTED;
      }
      else
      {
         workThrdParamsP->id = thrdPriority;
         workThrdParamsP->active = 1;
         workThrdParamsP->thrActive = 0;
         thrdParams.func = (OS_LYRG_threadFunction)inu_function__workThread;
         thrdParams.param = me;
         thrdParams.id = OS_LYRG_WORK_1_THREAD_ID_E + thrdPriority;
         thrdParams.event = NULL;

         workThrdParamsP->thrHandle = OS_LYRG_createThread(&thrdParams);
         if (workThrdParamsP->thrHandle == NULL)
         {
            printf("failed to create thread\n");
            ret = INU_FUNCTION__ERR_UNEXPECTED;
         }
      }
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_function__deinitWorkThread
*
*  Description:   deinit the work thread and its queue
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
static void inu_function__deinitWorkThread(inu_function__thrdParams *workThrdParamsP)
{
   int ret,totalSleep;
   workThrdParamsP->active = 0;
   totalSleep = 0;

   while((workThrdParamsP->thrActive) && (totalSleep < (1000 * 500)))
   {
      totalSleep+=1000;
      OS_LYRG_usleep(1000);
   }

   if (workThrdParamsP->thrActive)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Falied to deinit work thread %s\n", workThrdParamsP->thrWorkMsgQue.name);
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "Closing work thread: [%s]\n", workThrdParamsP->thrWorkMsgQue.name);

   OS_LYRG_closeThread(workThrdParamsP->thrHandle);
   ret = OS_LYRG_deleteMsgQue(&workThrdParamsP->thrWorkMsgQue);
   if (ret)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to delete msgQ\n");
   }

   if (workThrdParamsP)
   {
      free(workThrdParamsP);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "deinitWorkThread() was called, but workThrdParamsP is NULL\n");
   }
}

static ERRG_codeE inu_function__sendOperateMsgWorkThr(struct inu_function *function, inu_function__operateParamsT *inputParamsP)
{
   inu_function__workMsg msg;
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;
   struct inu_function__thrdParams *workThrdParamsP = INU_FUNCTION__PRIVDATA_ACCESS_P(function)->workThrdParams;

   msg.msgType = OPERATE;
   msg.function = function;
   msg.msgParam.functionOperateMsg.operateParams = *inputParamsP; //copy struct

   if (OS_LYRG_sendMsg(&workThrdParamsP->thrWorkMsgQue, (UINT8*)&msg, sizeof(inu_function__workMsg)))
   {
      ret = INU_FUNCTION__ERR_OUT_OF_RSRCS;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: inu_function__sendDspAckMsgWorkThr
*
*  Description:   The thread purpose is to execute the operate of a function
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
ERRG_codeE inu_function__sendDspAckMsgWorkThr(inu_function *function, inu_function__operateParamsT *operateParamsP, UINT32 dspSource)
{
   inu_function__workMsg msg;
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;
   inu_function__thrdParams *workThrdParamsP = INU_FUNCTION__PRIVDATA_ACCESS_P(function)->workThrdParams;

   msg.msgType = DSP_ACK;
   msg.function = function;
   msg.msgParam.dspAckMsg.operateParams = *operateParamsP; //copy struct
   msg.msgParam.dspAckMsg.dspSource     = dspSource;
   if (OS_LYRG_sendMsg(&workThrdParamsP->thrWorkMsgQue, (UINT8*)&msg, sizeof(inu_function__workMsg)))
   {
      ret = INU_FUNCTION__ERR_OUT_OF_RSRCS;
   }
   return ret;
}
#endif // DEFSG_IS_GP
static const char* inu_function__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return name;
}

static void inu_function__dtor(inu_ref *me)
{
   dataQueT *dqP;
   inu_node__vtable_get()->ref_vtable.p_dtor(me);
#if DEFSG_IS_GP
   if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workPriority < INU_FUNCTION__WORK_THR_PRIORITY_NO_THR)
   {
      inu_function__deinitWorkThread(INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workThrdParams);
   }

   //if function state is not stopped, we will stop it here
   if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount > 0)
   {
      inu_function__stopParamsT dummy;
      ((inu_function__VTable*)me->p_vtable)->p_stop((inu_function*)me, &dummy);
      INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount = 0;
      //inu_graph__set_function_stop_msg(INU_FUNCTION__ACCESS_P(me), &dummy);
   }

   //remove all input data ques
   while(INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP)
   {
      dqP                                               = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
      INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP->next;
      inu_function__removeInputQue(INU_FUNCTION__ACCESS_P(me), dqP);
   }
#else
   FIX_UNUSED_PARAM_WARN(dqP);
#endif
   free(INU_FUNCTION__PRIVDATA_ACCESS_P(me));
}

/* Constructor */
ERRG_codeE inu_function__ctor(inu_function *me, inu_function__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_function__privData *privP;
   ret = inu_node__ctor(&me->node, &ctorParamsP->nodeCtorParams);

   if (ERRG_SUCCEEDED(ret))
   {
      privP = (inu_function__privData*)malloc(sizeof(inu_function__privData));
      if (!privP)
      {
         ret = INU_FUNCTION__ERR_OUT_OF_MEM;
      }
      else
      {
         memset(privP,0,sizeof(inu_function__privData));
         me->privP = privP;
         INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operateModes = ctorParamsP->mode;
         INU_FUNCTION__PRIVDATA_ACCESS_P(me)->syncedFunc   = ctorParamsP->syncedFunc;
#if DEFSG_IS_GP
         if (ctorParamsP->pipeMaxSize < 1)
         {
           ret = INU_FUNCTION__ERR_PIPE_FAIL;
         }
         else
         {
            INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeMaxSize = ctorParamsP->pipeMaxSize;
            INU_FUNCTION__PRIVDATA_ACCESS_P(me)->inputQueDepth = ctorParamsP->inputQueDepth;
            INU_FUNCTION__PRIVDATA_ACCESS_P(me)->minInptsToOprt = ctorParamsP->minInptsToOprt;
            inu_function__setWorkThrdPriority(me,ctorParamsP->workPriority);
            if (ctorParamsP->workPriority < INU_FUNCTION__WORK_THR_PRIORITY_NO_THR)
            {
               inu_function__initWorkThread(me, ctorParamsP->workPriority);
            }
         }
#endif
      }
   }
   return ret;
}

static int inu_function__rxIoctl(inu_ref *me, void *msgP, int msgCode)
{
   int ret;

   ret = inu_node__vtable_get()->ref_vtable.p_rxSyncCtrl(me, msgP, msgCode);
#if DEFSG_IS_GP
   switch (msgCode)
   {
      case(INTERNAL_CMDG_FUNCTION_START_E):
      {
         //inu_graph__set_function_start_msg(INU_FUNCTION__ACCESS_P(me), (inu_function__startParamsT*)msgP);
         ((inu_function__VTable*)me->p_vtable)->p_start((inu_function*)me, msgP);

         break;
      }

      case(INTERNAL_CMDG_FUNCTION_STOP_E):
      {
         UINT32 totalSleep = 0;

         //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"stop start\n");
         ((inu_function__VTable*)me->p_vtable)->p_stop((inu_function*)me, msgP);
         while((INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount > 0) && (totalSleep < 500000))
         {
            OS_LYRG_usleep(500);
            totalSleep += 500;
         }
         if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount > 0)
         {
            INU_REF__LOGG_PRINT(me, LOG_WARN_E, NULL,"function did not complete when trying to stop\n");
         }
         //inu_graph__set_function_stop_msg(INU_FUNCTION__ACCESS_P(me), (inu_function__stopParamsT*)msgP);
         //INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"stop complete\n");
         break;
      }

      default:
      break;
   }
#endif
   return ret;
}

static inline void inu_function__showWorkStats(char *str, inu_function__workStatsT *workStatsP)
{
   if(workStatsP->aveWorkTimeUsec)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "%s average work time %llu usec. max %llu usec. min %llu usec\n",str, workStatsP->aveWorkTimeUsec,workStatsP->maxWorkTimeUsec,workStatsP->minWorkTimeUsec);
   }
}

static void inu_function__showStats(inu_ref *me)
{
   inu_function__statsT   *statsInfoP   = &INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats;
#if DEFSG_IS_GP
   dataQueT               *queListHeadP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
#endif
   inu_node__vtable_get()->ref_vtable.p_showStats(me);

   inu_function__showWorkStats("gp", &statsInfoP->gpWorkStats);
   inu_function__showWorkStats("dsp",&statsInfoP->dspWorkStats );
#if DEFSG_IS_GP
   if(statsInfoP->newDataCtr)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "New data: total %u, total dropped %u (oom %u, forward fail %u)\n",statsInfoP->newDataCtr,statsInfoP->newDataTotalDroppedCtr,statsInfoP->newDataOomCtr,statsInfoP->dataForwardFailCtr);
   }
   if(statsInfoP->doneDataCtr)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Done data: total %u, total dropped %u\n",statsInfoP->doneDataCtr,statsInfoP->doneDataTotalDroppedCtr);
   }
   while (queListHeadP)
   {
      if (queListHeadP->stats.inputDataQueTotalEnqueCtr)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Input data que (%s): total enque %u, dropped when que was full %u\n",queListHeadP->data->node.ref.p_vtable->p_name((inu_ref*)queListHeadP->data),queListHeadP->stats.inputDataQueTotalEnqueCtr,queListHeadP->stats.inputDataQueFullCtr);
      }
      queListHeadP = queListHeadP->next;
   }
   if (statsInfoP->gpToDspCtr)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Gp-Dsp data: total %u, total dropped %u (icc fail %u, job descriptor fail %u)\n",statsInfoP->gpToDspCtr,statsInfoP->gpToDspTotalDroppedCtr,statsInfoP->gpToDspIccFailCtr,statsInfoP->gpToDspJobDescOomCtr);
   }
#endif
}



/*
   For each input, a function has an input que. The input que holds the incoming data into the function.
   The input que purpose is:
   1. function proccess jitter
   2. sync several inputs with time proximity
   3. account for async inputs
 */
static void inu_function__newInput(inu_node *me, inu_node *input)
{
   dataQueT  *dataQueP;
   inu_node__vtable_get()->p_newInput(me,input);
#if DEFSG_IS_GP
   if (INU_REF__IS_DATA_TYPE(inu_ref__getRefType(input)))
   {
      int chunkFactor;
      /* create an input data que, and insert it to the list */
      dataQueP = (dataQueT*)malloc(sizeof(dataQueT));
      memset(dataQueP,0,sizeof(dataQueT));
      /* when using chunk mode, the data burst is high, increase the depth by factor of 10 to prevent loss of data if the que gets full */
      chunkFactor = (inu_data__chunkTotalNumGet((inu_data*)input) > 1) ? inu_data__chunkTotalNumGet((inu_data*)input) * 10 : 1;
      dataQueP->qP = createQ(INU_FUNCTION__PRIVDATA_ACCESS_P(me)->inputQueDepth * chunkFactor);
      dataQueP->data = (inu_data*)input;

      /* add to the end of the list, keep order of creation */
      if (!INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP)
      {
         INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP = dataQueP;
      }
      else
      {
         dataQueT *tempP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
         while(tempP->next)
         {
            tempP = tempP->next;
         }
         tempP->next = dataQueP;
      }
   }
#else
   FIX_UNUSED_PARAM_WARN(dataQueP);
#endif
}

static void inu_function__removeInput(inu_node *me, inu_node *input)
{
   inu_node__vtable_get()->p_removeInput(me,input);
#if DEFSG_IS_GP
   if (INU_REF__IS_DATA_TYPE(inu_ref__getRefType(input)))
   {
      dataQueT *dataQueP;
      //find the data que for this input, empty it and remove the que
      dataQueP = inu_function__getInputDataQue(INU_FUNCTION__ACCESS_P(me),(inu_data*)input);
      if (dataQueP)
      {
         inu_function__emptyInputQue(dataQueP);
         inu_function__removeInputQue(INU_FUNCTION__ACCESS_P(me), dataQueP);
      }
   }
#endif
}


// inu_function table
static void inu_function__operate(inu_function *me, inu_function__operateParamsT *paramsP)
{
   INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL,"inputs num = %d\n",paramsP->dataInputsNum);
}

/* graph context only */
static ERRG_codeE inu_function__startCb(inu_function *me, inu_function__startParamsT *startParamsP)
{
   FIX_UNUSED_PARAM_WARN(startParamsP);
#if DEFSG_IS_GP
   INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"start %s of %s\n",inu_ref__getUserName(me),SEQ_MNGRG_getName(inu_graph__getSeqDB(inu_node__getGraph(me))));
#else
   INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"start %s\n",inu_ref__getUserName(me));
#endif

   //reset working stats for new record session
   memset(&INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats,0,sizeof(INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats));
   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpWorkStats.minWorkTimeUsec = INU_FUNCTION__MAX_WORK_STAT_TIME;
   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.dspWorkStats.minWorkTimeUsec = INU_FUNCTION__MAX_WORK_STAT_TIME;
   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.state = ENABLE;
   return INU_FUNCTION__RET_SUCCESS;
}

/* graph context only */
static ERRG_codeE inu_function__stopCb(inu_function *me, inu_function__stopParamsT *stopParamsP)
{
   FIX_UNUSED_PARAM_WARN(stopParamsP);
   INU_REF__LOGG_PRINT(me, LOG_INFO_E, NULL,"stop\n");

   //need to clear ques if disabled
   //if function is active, then we let it finish and clear the que from finish
   if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount > 0)
   {
      INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL,"function is active, wait for it to end\n");
   }
#if DEFSG_IS_GP
   else
   {
      //function is not active, can clear the ques now
      inu_function__emptyAllInputQues(me);
   }
#endif
   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.state = DISABLE;
   return INU_FUNCTION__RET_SUCCESS;
}

static void inu_function__dspAck(inu_function *me, inu_function__operateParamsT *paramsP, inu_function__coreE dspSource)
{
   FIX_UNUSED_PARAM_WARN(me);
   FIX_UNUSED_PARAM_WARN(paramsP);
   FIX_UNUSED_PARAM_WARN(dspSource);
}

void inu_function__vtableInitDefaults(inu_function__VTable *vtableP)
{
   inu_node__vtableInitDefaults(&vtableP->node_vtable);

   vtableP->node_vtable.ref_vtable.p_name = inu_function__name;
   vtableP->node_vtable.ref_vtable.p_dtor = inu_function__dtor;
   vtableP->node_vtable.ref_vtable.p_ctor = (inu_ref__Ctor*)inu_function__ctor;
   vtableP->node_vtable.ref_vtable.p_rxSyncCtrl = inu_function__rxIoctl;
   vtableP->node_vtable.ref_vtable.p_showStats = inu_function__showStats;


   vtableP->node_vtable.p_newInput    = inu_function__newInput;
   vtableP->node_vtable.p_removeInput = inu_function__removeInput;

   vtableP->p_operate = inu_function__operate;
   vtableP->p_start   = inu_function__startCb;
   vtableP->p_stop    = inu_function__stopCb;
   vtableP->p_dspAck  = inu_function__dspAck;
}

static void inu_function__vtable_init()
{
   if (!_bool_vtable_initialized)
   {
      inu_function__vtableInitDefaults(&_vtable);
      _bool_vtable_initialized = true;
   }
}

const inu_function__VTable *inu_function__vtable_get(void)
{
   inu_function__vtable_init();
   return &_vtable;
}

ERRG_codeE inu_function__start(inu_functionH meH, inu_function__startParamsT *startParamP)
{
   inu_function *me = (inu_function*)meH;
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;

   if (!INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount)
   {
      ret = inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_FUNCTION_START_E, startParamP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
      if (ERRG_SUCCEEDED(ret))
      {
         ret = ((inu_function__VTable*)me->node.ref.p_vtable)->p_start(me, startParamP);
      }
   }
   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount++;
   return ret;
}

ERRG_codeE inu_function__stop(inu_functionH meH, inu_function__stopParamsT *stopParamP)
{
   inu_function *me = (inu_function*)meH;
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;

   if (!INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Stop command was called for [%s], but this function wasn't started\n",inu_ref__getUserName(me));
      return INU_FUNCTION__ERR_UNEXPECTED;
   }

   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount--;
   if (!INU_FUNCTION__PRIVDATA_ACCESS_P(me)->stateParam.activationRefCount)
   {
      ret = inu_ref__sendCtrlSync((inu_ref*)meH, INTERNAL_CMDG_FUNCTION_STOP_E, stopParamP, INU_REF__SYNC_DFLT_TIMEOUT_MSEC);
      if (ERRG_SUCCEEDED(ret))
      {
         ret = ((inu_function__VTable*)me->node.ref.p_vtable)->p_stop(me, stopParamP);
      }
   }
   return ret;
}

void inu_function__getState(inu_functionH meH, inu_function__stateParam *stateParamP)
{
   memcpy(stateParamP, &INU_FUNCTION__PRIVDATA_ACCESS_P(meH)->stateParam, sizeof(inu_function__stateParam));
}

UINT32 inu_function__getOperateModes(inu_functionH meH)
{
   return INU_FUNCTION__PRIVDATA_ACCESS_P(meH)->operateModes;
}

UINT32 inu_function__isSyncedFunc(inu_functionH meH)
{
   return INU_FUNCTION__PRIVDATA_ACCESS_P(meH)->syncedFunc;
}

static inline void inu_function__processCompleteStats(inu_function__workStatsT *workStatsP, UINT64 ctr)
{
   UINT64 usec, period;

   if(ctr)
   {
      OS_LYRG_getUsecTime(&usec);
      period = (usec - workStatsP->startWorkTimeUsec);
      workStatsP->totalWorkTimeUsec += period;
      workStatsP->aveWorkTimeUsec = workStatsP->totalWorkTimeUsec / ctr;
      if (workStatsP->maxWorkTimeUsec < period)
         workStatsP->maxWorkTimeUsec = period;
      if (workStatsP->minWorkTimeUsec > period)
         workStatsP->minWorkTimeUsec = period;
   }
}

#if DEFSG_IS_GP
void inu_function__setWorkThrdPriority(inu_function *me, inu_function__workThrdPriority workPriority)
{
#ifndef NO_THREAD
   if (workPriority >= INU_FUNCTION__WORK_THR_PRIORITY_NUM)
   {
      workPriority = INU_FUNCTION__WORK_THR_PRIORITY_LOW;
   }
#else
   workPriority = INU_FUNCTION__WORK_THR_PRIORITY_NO_THR;
#endif
   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workPriority = workPriority;
}


//todo: optimize how ques are stored, currently it costs o(n) of number of ques each time to enque data
ERRG_codeE inu_function__enqueData(inu_function *me, inu_data *data, inu_data *clone)
{
   //find the data's que
   dataQueT  *runningP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
   while (runningP)
   {
      if (runningP->data == data)
      {
         break;
      }
      runningP = runningP->next;
   }

   //did not find the que
   if (!runningP)
   {
      assert(0);
      return INU_FUNCTION__ERR_OUT_OF_RSRCS;
   }

   //if que is full, then deque (will return the oldest), free it, and then enque
   if (queFull(runningP->qP))
   {
#ifdef DEBUG_INPUT_QUE
      UINT64 usec;
      OS_LYRG_getUsecTime(&usec);
      printf("%llu: %s input data que %s is full, free oldest\n",usec,me->node.ref.p_vtable->p_name((inu_ref*)me),runningP->data->node.ref.p_vtable->p_name((inu_ref*)runningP->data));
#endif
      inu_data__freeInternal((inu_data*)dequeue(runningP->qP));
      runningP->stats.inputDataQueFullCtr++;
   }

   runningP->stats.inputDataQueTotalEnqueCtr++;
#ifdef MEM_POOLG_BUFF_TRACE
   MEM_POOLG_bufDescT *bufDesP;
   inu_data__bufDescPtrGet(clone, &bufDesP);
   MEM_POOLG_buffTraceUpdateState(inu_ref__getUserName(me), bufDesP, __func__);
#endif
   enqueue(runningP->qP,clone);
   return INU_FUNCTION__RET_SUCCESS;
}

/*
   inu_function__getValidInputs

   purpose is to check if a function input ques hold data which the
   function can now work on. The data list is returned in dataInputs. If the input que do not have
   valid data, then the function return error.

   TODO: add time proximity check, add async input ques
*/
static ERRG_codeE inu_function__getValidInputs(inu_function *me, inu_function__operateParamsT *inputParamsP)
{
   dataQueT                *runningP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
   UINT16                  i = 0,numInputs=0,allQueFull=1;
   inu_data                *data;

   //function without input ques is valid to run always
   if (!runningP)
   {
      return INU_FUNCTION__RET_SUCCESS;
   }

   //need to check all ques, and see if we have inputs in all (add peek to que function?)
   while (runningP)
   {
      //TODO: check timestamp proximity? what about async inputs?
      if (!peek(runningP->qP))
      {
         allQueFull=0;
      }
      else numInputs++;
      runningP = runningP->next;
   }

   if ( INU_FUNCTION__PRIVDATA_ACCESS_P(me)->minInptsToOprt != 0xffff)
   {
       if (numInputs<INU_FUNCTION__PRIVDATA_ACCESS_P(me)->minInptsToOprt) // continue if we reached threshold
         return INU_FUNCTION__ERR_OUT_OF_RSRCS;
   }
   else
   {
       if (allQueFull == 0) //Not all queue are full, exit with error
         return INU_FUNCTION__ERR_OUT_OF_RSRCS;
   }


   //all ques have data, or we exceed threshold
   //  fill the dataInputs and return success
   runningP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
   while (runningP)
   {
      data = dequeue(runningP->qP);
      if (data)
      {
         inputParamsP->dataInputs[i] = data;
         i++;

#ifdef HELSINKI_DISABLE_MULTI_INPUT_SUPPORT
         //printf("inu_ref__getUserName(&me->node.ref) - %s\n", inu_ref__getUserName(&me->node.ref));
         
         /*Writer don't support multiple inputs, neither does the streamer,
           but the graph infrastructure has no way of knowing this at the moment*/
         if(!inu_ref__getDisableMultiInputSuportOverride(&me->node.ref))
            break;
#endif
         // if(!strcmp(inu_ref__getUserName(&me->node.ref), "Stream_Cva_0") || !strcmp(inu_ref__getUserName(&me->node.ref), "WRITER_1"))
         // {
         //    /*Writer don't support multiple inputs, neither does the streamer,
         //    but the graph infrastructure has no way of knowing this at the moment*/
         //    break;  
         // }
      }
      runningP = runningP->next;
   }
   inputParamsP->dataInputsNum = i;

   return INU_FUNCTION__RET_SUCCESS;
}

static dataQueT *inu_function__getInputDataQue(inu_function *me, inu_data *data)
{
   dataQueT                *runningP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;

   while(runningP)
   {
      //find the input que for this data
      if (data == runningP->data)
      {
         return runningP;
      }
      runningP = runningP->next;
   }

   //did not find, return null
   return runningP;
}

static void inu_function__removeInputQue(inu_function *me, dataQueT *dataQueP)
{
   dataQueT *prevDataQueP = NULL, *currentDataQueP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;

   while (currentDataQueP)
   {
      if (currentDataQueP == dataQueP)
      {
         if (prevDataQueP)
         {
            prevDataQueP->next = currentDataQueP->next;
         }
         else
         {
            INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP = currentDataQueP->next;
         }

         freeQ(currentDataQueP->qP);
         free(currentDataQueP);
         return;
      }
      currentDataQueP = currentDataQueP->next;
   }
}

/*
   inu_function__emptyInputQue

   function is used for clearing up an input que from the function. used when an input data is removed
   from the graph or function is disabled

*/
static void inu_function__emptyInputQue(dataQueT *dataQueP)
{
   inu_data                *clone;

   clone = (inu_data*)dequeue(dataQueP->qP);
   while (clone)
   {
      inu_data__freeInternal(clone);
      clone = (inu_data*)dequeue(dataQueP->qP);
   }
}

/*
   inu_function__emptyInputQues

   function is used for clearing up an input que from the function. used when an input data is removed
   from the graph or function is disabled

*/
void inu_function__emptyAllInputQues(inu_function *me)
{
   dataQueT *runningP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;
   while(runningP)
   {
      inu_function__emptyInputQue(runningP);
      runningP = runningP->next;
   }
}

/* context - graph only */
void inu_function__processComplete(inu_function *me)
{
   inu_function__statsT *statsP = &INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats;
   ERRG_codeE ret;
   inu_function__stateParam stateParam;
   inu_function__operateParamsT inputParams;

   inu_function__processCompleteStats(&statsP->gpWorkStats, statsP->operatedCtr);

   //if function got disabled during operation, clearing ques waited until it completed.
   //after clearing the que, the function will set as not active (inu_function__getValidInputs will fail)
   inu_function__getState(me, &stateParam);
   if(stateParam.state == DISABLE)
   {
      inu_function__emptyAllInputQues(me);
   }

   ret = inu_function__getValidInputs(me, &inputParams);
   if (ERRG_SUCCEEDED(ret))
   {
#ifdef MEM_POOLG_BUFF_TRACE
      MEM_POOLG_bufDescT *bufDescP;

      inu_data__bufDescPtrGet(inputParams.dataInputs[0], &bufDescP);
      MEM_POOLG_buffTraceUpdateState(inu_ref__getUserName(me), bufDescP, __func__);
#endif
      statsP->operatedCtr++;
#ifdef DEBUG_INPUT_QUE
      printf("inu_function__complete %s (%d) and inputs valid in que, run with inputs from que\n",me->node.ref.p_vtable->p_name((inu_ref*)me), me->node.ref.id);
#endif
      OS_LYRG_getUsecTime(&statsP->gpWorkStats.startWorkTimeUsec);
      if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workPriority == INU_FUNCTION__WORK_THR_PRIORITY_NO_THR)
      {
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            char tmp2[256]="";
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
             sprintf(tmp2,"%s: (%s) operate from processComplete %llu\n",inu_ref__getUserName(me),SEQ_MNGRG_getName(inu_graph__getSeqDB(inu_node__getGraph(me))),time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp2);
#endif
         ((inu_function__VTable*)me->node.ref.p_vtable)->p_operate(me, &inputParams);
      }
      else
      {
         ret = (inu_function__sendOperateMsgWorkThr(me, &inputParams));
         if (ERRG_FAILED(ret))
         {
            statsP->invokeOperateFailedCtr++;
         }
      }
   }
   else
   {
#ifdef DEBUG_INPUT_QUE
      printf("inu_function__complete %s (%d) inputs not valid, set as not active\n",me->node.ref.p_vtable->p_name((inu_ref*)me), me->node.ref.id);
#endif

      if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount == 0)
      {
#ifdef DEBUG_INPUT_QUE
          printf("inu_function__complete %s (%d) operatePipeCount shouldn't be zero\n",me->node.ref.p_vtable->p_name((inu_ref*)me), me->node.ref.id);
#endif
      }
      else
      {
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            char tmp1[256]="";
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
             sprintf(tmp1,"%s: (%s) processComplete -- %llu\n",inu_ref__getUserName(me),SEQ_MNGRG_getName(inu_graph__getSeqDB(inu_node__getGraph(me))),time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp1);
#endif
         INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount--;
      }
   }
}

/* context - graph only */
void inu_function__startOperate(inu_function *me)
{
   inu_function__statsT *statsP = &INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats;
   ERRG_codeE ret;

   if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount < INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeMaxSize)
   {
      inu_function__operateParamsT inputParams;
      ret = inu_function__getValidInputs(me, &inputParams);
      if (ERRG_SUCCEEDED(ret))
      {
#ifdef MEM_POOLG_BUFF_TRACE
         MEM_POOLG_bufDescT *bufDescP;

         inu_data__bufDescPtrGet(inputParams.dataInputs[0], &bufDescP);
         MEM_POOLG_buffTraceUpdateState(inu_ref__getUserName(me), bufDescP, __func__);
#endif
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            char tmp[256]="";
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
            sprintf(tmp,"%s: refId %d validInput %llu\n",me->node.ref.p_vtable->p_name((inu_ref*)me), me->node.ref.id,time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp);
#endif
         statsP->operatedCtr++;
         OS_LYRG_getUsecTime(&statsP->gpWorkStats.startWorkTimeUsec);
         INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount++;
         if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workPriority == INU_FUNCTION__WORK_THR_PRIORITY_NO_THR)
         {
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            char tmp3[256]="";
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
            sprintf(tmp3,"%s: (%s) operate from startOperate %llu\n",inu_ref__getUserName(me),SEQ_MNGRG_getName(inu_graph__getSeqDB(inu_node__getGraph(me))),time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp3);
#endif
            ((inu_function__VTable*)me->node.ref.p_vtable)->p_operate(me, &inputParams);
         }
         else
         {
            while(ERRG_FAILED((inu_function__sendOperateMsgWorkThr(me, &inputParams))))
            {
               statsP->invokeOperateFailedCtr++;
               OS_LYRG_usleep(1);
            }
         }
      }
   }
   else
   {
 #ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
            char tmp2[256]="";
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
            sprintf(tmp2,"%s: (%d) pipe full %llu\n",me->node.ref.p_vtable->p_name((inu_ref*)me), me->node.ref.id,time);
            SEQ_MNGRG_addSchedulerDebugPrint(tmp2);
#endif
      //printf("inu_function__startOperate fail 2 operatePipeCount %d operatePipeMaxSize %d\n",INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeCount,INU_FUNCTION__PRIVDATA_ACCESS_P(me)->operatePipeMaxSize);
   }
}

/* when a function completes its operate */
ERRG_codeE inu_function__complete(inu_function *me)
{
   return inu_graph__function_complete_msg(me);
}
ERRG_codeE inu_function__complete__handleImmediately(inu_function *me)
{
   return inu_graph__function_complete_msg_immedate(me);
}

/*
     inu_function__newData -

      When a function creates new data, we clone inu_data of the same type, and fill it with the given bufferDescriptor and Hdr.
      important note - the cloned data is not cached, to allow multiprocess accessibility.
      TODO: optimize function to perform only one access on the cloned data
*/
ERRG_codeE inu_function__newData(inu_function *me, inu_data *data, MEM_POOLG_bufDescT *bufP, void *hdrP, inu_data **newData)
{
   inu_function__statsT *statsP = &INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats;
   inu_data *clone;
   ERRG_codeE ret;

   statsP->newDataCtr++;

   //alloc a data clone from pool, assign pointer, hdr
   ret = inu_data__allocInternal(data,&clone);
   if (ERRG_SUCCEEDED(ret))
   {
      //save the buffer descriptor for this inu_data clone
      inu_data__bufDescPtrSet(clone, bufP);
      //save the physical address (if there is) of the data. required for other processors
      ret = inu_data__getMemPhyPtr(clone, &clone->dataPhyAddr);
      if (ERRG_FAILED(ret))
      {
         INU_REF__LOGG_PRINT(me, LOG_WARN_E, NULL, "failed to get phy address for new data\n");
      }
      //save hdr to inu_data
      ((inu_data__VTable*)clone->node.ref.p_vtable)->p_dataHdrSet(clone,hdrP);
      if (CMEM_cacheWb(clone,inu_factory__getRefSize(clone->node.ref.refType)))
      {
         assert(0);
      }
      *newData = clone;
#ifdef DATA_DEBUG
      if((clone->node.ref.refType) == inu_debug_data_map)
      {
         UINT64 usec;
         OS_LYRG_getUsecTime(&usec);
         printf("%llu: new data: %s (%d,%p) mode %d from function %s (%d)\n",
                     usec, clone->node.ref.p_vtable->p_name((inu_ref*)clone),
                     clone->node.ref.id,clone,inu_data__getMode(clone),
                     me->node.ref.p_vtable->p_name((inu_ref*)me),
                     me->node.ref.id);
      }
#endif
      INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL, "bufP = %p, cntr = %llu, ts = %llu, clone = %p\n",bufP,((inu_data__hdr_t*)hdrP)->dataIndex, ((inu_data__hdr_t*)hdrP)->timestamp,clone);
   }
   else
   {
      MEM_POOLG_free(bufP);
      statsP->newDataOomCtr++;
   }
   return ret;
}

ERRG_codeE inu_function__doneData(inu_function *me, inu_data *data)
{
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;
   inu_function__statsT *statsP = &INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats;
#ifdef MEM_POOLG_BUFF_TRACE
   MEM_POOLG_bufDescT *bufDescP;

   inu_data__bufDescPtrGet(data, &bufDescP);
   MEM_POOLG_buffTraceUpdateState(inu_ref__getUserName(me), bufDescP, __func__);
#endif
   ret = inu_graph__forward_data_msg(me, data);
   if (ERRG_FAILED(ret))
   {
      inu_data__freeInternal(data);
      statsP->dataForwardFailCtr++;
      INU_REF__LOGG_PRINT(me, LOG_WARN_E, NULL, "new data failed, graph thread is blocked!\n");
   }
   else
   {
      statsP->doneDataCtr++;
   }

   return ret;
}
ERRG_codeE inu_function__doneData_handleImmediately(inu_function *me, inu_data *data)
{
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;
   inu_function__statsT *statsP = &INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats;

   ret = inu_graph__forward_data_msg_HandleImmediately(me, data);
   if (ERRG_FAILED(ret))
   {
      inu_data__freeInternal(data);
      statsP->dataForwardFailCtr++;
      INU_REF__LOGG_PRINT(me, LOG_WARN_E, NULL, "new data failed, graph thread is blocked!\n");
   }
   else
   {
      statsP->doneDataCtr++;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name:  inu_function__gpFromDspMsgAckRcv
*
*  Description: callback for sending messages to dsp.
*                   changes context to graph thread for executing function callback
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     ICC thread
*
****************************************************************************/
static void inu_function__gpFromDspMsgAckRcv(UINT16 jobDescriptorIndex, ERRG_codeE result, UINT32 dspSource)
{
   DATA_BASEG_iccJobsDescriptorDataBaseT  *jobsDbP;
   inu_function                           *me;
   inu_function__operateParamsT            operateParams;
   UINT16                                  i;

   //job descriptor is in our hands, its safe to access with no mutex
   DATA_BASEG_accessDataBaseNoMutex((UINT8**)&jobsDbP, DATA_BASEG_JOB_DESCRIPTORS);

   //extract the function from which the dsp cmd was started
   me = (inu_function*)jobsDbP->jobDescriptorPool[jobDescriptorIndex].arg;

   pmm_mngr__dspPmmDeactivate(dspSource);

   if(ERRG_FAILED(result))
   {
      INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpToDspTotalDroppedCtr++;
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, result, "receive ack for Dsp message with error\n");
   }
   else
   {
      inu_function__processCompleteStats(&INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.dspWorkStats, INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpToDspCtr);
      INU_REF__LOGG_PRINT(me, LOG_DEBUG_E, NULL, "Dsp done: jobDescriptorIndex=%d, result=0x%X\n", jobDescriptorIndex, result);
   }

   //construct the operateParams from the DSP msg
   for(i = 0; i < jobsDbP->jobDescriptorPool[jobDescriptorIndex].inputList.databaseListNum; i++)
   {
      operateParams.dataInputs[i] = jobsDbP->jobDescriptorPool[jobDescriptorIndex].inputList.inu_dataVirt[i];
   }
   operateParams.dataInputsNum = jobsDbP->jobDescriptorPool[jobDescriptorIndex].inputList.databaseListNum;

   if (INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workPriority == INU_FUNCTION__WORK_THR_PRIORITY_NO_THR)
   {
        ((inu_function__VTable*)me->node.ref.p_vtable)->p_dspAck(me, &operateParams, dspSource);
   }
    else
    {
       //process the ack from the work thread
       result = inu_function__sendDspAckMsgWorkThr(me, &operateParams, dspSource);
       if(ERRG_FAILED(result))
       {
          INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.invokeOperateFailedCtr++;
          INU_REF__LOGG_PRINT(me, LOG_ERROR_E, result, "failed to send ack to work thread (%d)\n",INU_FUNCTION__PRIVDATA_ACCESS_P(me)->workPriority);
       }
    }

   result = ICCG_returnJobDescriptor(jobDescriptorIndex);
   if(ERRG_FAILED(result))
   {
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, result, "failed to return jobDescriptor to pool\n");
   }
}

/****************************************************************************
*
*  Function Name:  inu_function__sendDspCtrlMsg
*
*  Description:
*
*  Returns:
*     Success or specific error code.
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
ERRG_codeE inu_function__sendDspMsg(inu_function *me, inu_function__operateParamsT *paramsP, UINT32 alg, UINT32 data, UINT32 command, inu_function__coreE dspTarget)
{
   ERRG_codeE                            retCode = INU_FUNCTION__RET_SUCCESS;
   UINT16                                jobDescriptorIndex, i;
   ICCG_cmdT                             cmd;
   DATA_BASEG_iccJobsDescriptorDataBaseT *jobsDbP;

   if (paramsP->dataInputsNum > DATA_BASE__MAX_NUM_LIST)
   {
      retCode = INU_FUNCTION__ERR_INVALID_ARGS;
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error sending CMD to DSP (%d > %d)\n",paramsP->dataInputsNum,DATA_BASE__MAX_NUM_LIST);
      return retCode;
   }


   INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpToDspCtr++;
   retCode = ICCG_getJobDescriptor(&jobDescriptorIndex);

   if(ERRG_SUCCEEDED(retCode))
   {
      //safe to access without mutex, we got the descriptor
      DATA_BASEG_accessDataBaseNoMutex((UINT8**)(&jobsDbP), DATA_BASEG_JOB_DESCRIPTORS);
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].alg             = alg;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].cb              = inu_function__gpFromDspMsgAckRcv;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].data            = data;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].arg             = (UINT32)me;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].inputList.databaseListNum = paramsP->dataInputsNum;
      jobsDbP->jobDescriptorPool[jobDescriptorIndex].command         = command;
      for(i = 0; i < paramsP->dataInputsNum; i++)
      {
         retCode = inu_data__getPhyAddress(paramsP->dataInputs[i], (UINT32*)&jobsDbP->jobDescriptorPool[jobDescriptorIndex].inputList.inu_dataPhy[i]);
         if(ERRG_FAILED(retCode))
         {
            assert(0);
         }
         //keep track of inu_data in icc DB so we can return it to the caller
         jobsDbP->jobDescriptorPool[jobDescriptorIndex].inputList.inu_dataVirt[i] = paramsP->dataInputs[i];
      }
      cmd.cmdType  = ICCG_CMD_REQ;
      cmd.descriptorHandle = jobDescriptorIndex;
      cmd.dspTarget = dspTarget;
      OS_LYRG_getUsecTime(&INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.dspWorkStats.startWorkTimeUsec);
      retCode = pmm_mngr__dspPmmActivate(dspTarget);
      if(ERRG_FAILED(retCode))
      {
         INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error activate dsp %d\n" );
         return retCode;
      }

      retCode = ICCG_sendIccCmd(&cmd);
      if(ERRG_FAILED(retCode))
      {
         INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpToDspIccFailCtr++;
         INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error sending CMD to DSP\n" );
      }
   }
   else
   {
      INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpToDspJobDescOomCtr++;
      INU_REF__LOGG_PRINT(me, LOG_ERROR_E, retCode, "Error sending CMD to DSP (stack is empty)\n" );
   }

   if(ERRG_FAILED(retCode))
   {
      INU_FUNCTION__PRIVDATA_ACCESS_P(me)->functionStats.gpToDspTotalDroppedCtr++;
   }

   return retCode;
}

/****************************************************************************
*
*  Function Name:  inu_function__hasMoreInput
*
*  Description: Query the input ques for the function, and return their status
*
*  Returns:
*     0 if all the input ques are empty
*
*  Context:
*     Single and blockable context.
*
****************************************************************************/
int inu_function__hasMoreInput(inu_functionH meH)
{
   inu_function            *me = (inu_function*)meH;
   dataQueT                *runningP = INU_FUNCTION__PRIVDATA_ACCESS_P(me)->queListHeadP;

   //function without input ques
   if (!runningP)
   {
      return 0;
   }

   //need to check all ques, and see if we have inputs in all (add peek to que function?)
   while (runningP)
   {
      if (peek(runningP->qP))
      {
         //que is not empty
         return 1;
      }
      runningP = runningP->next;
   }

   return 0;
}
#endif //DEFSG_IS_GP

