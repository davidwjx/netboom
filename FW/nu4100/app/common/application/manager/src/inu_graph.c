#include "inu_graph.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if (DEFSG_IS_OS_LINUX && (!DEFSG_IS_HOST_ANDROID))
#include <unistd.h>
#endif

#include "err_defs.h"
#include "internal_cmd.h"

#include "inu2.h"
#include "inu2_internal.h"
#include "nucfg.h"
#include "calibration.h"

#include "os_lyr.h"
#include "log.h"
#include "assert.h"
#include "inu_graph_creator.h"
#if DEFSG_IS_GP
#include <unistd.h>
#include "sequence_mngr.h"
#include "xml_db.h"
#include "hcg_mngr.h"
#include "gme_mngr.h"
#include "lut_mngr.h"
#include "helsinki.h"
#endif

//#define MSG_DEBUG
#ifdef DATA_DEBUG
UINT32 inu_debug_data_map = INU_HISTOGRAM_DATA_REF_TYPE;
#endif

#define GRAPH_NODES_AS_LIST

#ifndef GRAPH_NODES_AS_LIST
#define INU_GRAPH_MAX_NUM_NODES    (80)
#endif
#define INU_GRAPH__NEW_LINE_STRING "\\--- "
#define INU_GRAPH__CONNECT_STRING  " <---- "
#define INU_GRAPH__MSGQ_NAME       "/graph_q"
#define MIN(A,B) ((A)<(B) ? (A):(B))


#define INU_GRAPH__MEM_POOL_BUFF_SIZE_BYTES  (40000)
#define INU_GRAPH__MEM_POOL_CONFIG_DB_BUFF_SIZE_BYTES  (262144)
#define INU_GRAPH__MEM_POOL_BUFF_NUM         (2)

#define INU_GRAPH__SYNC_TIMEOUT_MSEC         (15000)
#define INU_GRAPH__MAX_ID_NUM                (32)

typedef struct inu_node__link
{
   struct inu_node *node;
   struct inu_node__link *next;
}inu_node__link;

typedef struct inu_graph__privData
{
#ifdef GRAPH_NODES_AS_LIST
   inu_node__link              *nodesListHead;
#else
   inu_node                    *nodesList[INU_GRAPH_MAX_NUM_NODES];
#endif
   inu_graph__CtorParams       ctorParams;
   inu_graph__calibrationPathT calibPaths;
   UINT32                      id;
   char                        socxml_path[SYSTEMP_XML_PATH_STR_LEN];
   char                        socxml_mod_path[SYSTEMP_XML_PATH_STR_LEN];
   CALIB_sectionDataT          *calibSectionsData;
   int                         numOfSections;
   UINT32                      waitAck;
   char                        graphXMLpath[SYSTEMP_XML_PATH_STR_LEN];
   bool                        saveXmlMod;
   MEM_POOLG_handleT           lutPoolH;
   MEM_POOLG_handleT           dbPoolH;
   inu_nucfgH                  cfgH;
#if DEFSG_IS_GP
   SEQ_MNGRG_handleT           seqDbH;
   XMLDB_dbH                   gpDbh;
#endif
}inu_graph__privData;

typedef enum
{
   DATA_READY,
   FUNCTION_START,
   FUNCTION_STOP,
   FUNCTION_COMPLETE,
}inu_graph__runtimeMsgTypes;

typedef struct
{
   inu_data       *data;
   inu_function *function;
}inu_graph__dataReadyMsg;

typedef struct
{
   inu_function               *function;
   inu_function__startParamsT startParams;
}inu_graph__functionStartMsg;

typedef struct
{
   inu_function              *function;
   inu_function__stopParamsT stopParams;
}inu_graph__functionStopMsg;

typedef struct
{
   inu_function                *function;
}inu_graph__functionCompleteMsg;


typedef union
{
   inu_graph__dataReadyMsg        dataReadyMsg;
   inu_graph__functionStartMsg    functionStartMsg;
   inu_graph__functionStopMsg     functionStopMsg;
   inu_graph__functionCompleteMsg functionCompleteMsg;
}inu_graph__runtimeMsgU;

typedef struct
{
   inu_graph__runtimeMsgU     msgParam;
   inu_graph__runtimeMsgTypes msgType;
}inu_graph__runtimeMsg;

UINT32 inu_graph__getId(inu_function *me);
#if DEFSG_IS_GP
extern void inu_function__processComplete(inu_function *me);
extern void inu_function__startOperate(inu_function *me);
static void inu_graph__handleRuntimeMessage(inu_graph__runtimeMsg *msgP);
static void inu_graph__initRuntimeThread( void );
static ERRG_codeE inu_graph__gpHandleConfigDbMsg(UINT8 *dataP, UINT32 dataLen, inu_graph *me, char *name);
#else
void inu_graph__createXmlPaths(inu_graph *me);
#endif
ERRG_codeE inu_graph__addNodesFromXml(inu_graph *me, inu_nucfgH nucfgH);
ERRG_codeE inu_graph__bindIspOutputToWriter(inu_graphH meH, inu_nucfgH nucfgH);

static inu_ref__VTable _vtable;
static bool _bool_vtable_initialized = 0;
// changed to avoid overwriting by local 'name' by david @ 2022.09.24
static const char* ggname = "INU_GRAPH";

#if DEFSG_IS_GP
static OS_LYRG_threadHandle        thrGraphHandle = NULL;
static OS_LYRG_msgQueT             thrGraphMsgQue = { 0 };
static OS_LYRG_event               thrStartEvent;
static OS_LYRG_event               thrStopEvent;
static volatile int                runTimeThreadActive;
static volatile int                thrActive;
static OS_LYRG_mutexT               handlingMutex;
#endif
static INT8                        ids[INU_GRAPH__MAX_ID_NUM];
static UINT32 extInterleaveChannels = 0;
bool inu_graph__isExtInterleaveChannel(UINT32 chId)
{
    if (chId > NUCFG_MAX_DB_META_CHANNELS)
        return false;

    if (extInterleaveChannels & (1<<chId))
       return true;
    return false;
}

void inu_graph__markExtInterleaveChannel(UINT32 chId)
{
    if (chId > NUCFG_MAX_DB_META_CHANNELS)
        return;
    extInterleaveChannels |= (1 << chId);
}

static const char* inu_graph__name(inu_ref *me)
{
   FIX_UNUSED_PARAM_WARN(me);
   return ggname;
}

static void inu_graph__initIds()
{
   UINT8 i;
   for (i = 0; i < INU_GRAPH__MAX_ID_NUM; i++)
   {
      ids[i] = -1;
   }
}

static UINT8 inu_graph__acquireId( void )
{
   UINT8 i;
   for (i = 0; i < INU_GRAPH__MAX_ID_NUM; i++)
   {
      if (ids[i] < 0)
      {
         ids[i] = i;
         return i;
      }
   }
   //assert(0);
   return 0;
}

static void inu_graph__releaseId(UINT32 id)
{
   ids[id] = -1;
}


#if DEFSG_IS_GP
static ERRG_codeE inu_graph__sendMessage(inu_graph__runtimeMsg *msgP)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
#ifndef NO_THREAD
   if (OS_LYRG_sendMsg(&thrGraphMsgQue, (UINT8*)msgP, sizeof(inu_graph__runtimeMsg)))
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Graph Queue full !! \n");
      ret = INU_GRAPH__ERR_OUT_OF_RSRCS;
   }
#else
   inu_graph__handleRuntimeMessage(msgP);
#endif
   return ret;
}
static ERRG_codeE inu_graph__sendMessageWithImmediateHandling(inu_graph__runtimeMsg *msgP)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   inu_graph__handleRuntimeMessage(msgP);
   return ret;
}

/* blocked operation, wait until start ends */
ERRG_codeE inu_graph__set_function_start_msg(inu_function *function, inu_function__startParamsT *startParamP)
{
   ERRG_codeE ret;
#ifdef MSG_DEBUG
   const char *userNameTemp = inu_ref__getUserName((inu_ref*)function);
   printf("send msg from %s: [%s]\n", __FUNCTION__, userNameTemp);
#endif
   inu_graph__runtimeMsg msg;
   msg.msgParam.functionStartMsg.function = function;
   msg.msgParam.functionStartMsg.startParams = *startParamP;
   msg.msgType = FUNCTION_START;
   ret = inu_graph__sendMessage(&msg);
   if (ERRG_SUCCEEDED(ret))
   {
      OS_LYRG_waitEvent( thrStartEvent,  INU_GRAPH__SYNC_TIMEOUT_MSEC);
   }
   return ret;
}

/* blocked operation, wait until stop ends */
ERRG_codeE inu_graph__set_function_stop_msg(inu_function *function, inu_function__stopParamsT *stopParamP)
{
   ERRG_codeE ret;
#ifdef MSG_DEBUG
   const char *userNameTemp = inu_ref__getUserName((inu_ref*)function);
   printf("send msg from %s: [%s]\n", __FUNCTION__, userNameTemp);
#endif
   inu_graph__runtimeMsg msg;
   msg.msgParam.functionStopMsg.function = function;
   msg.msgParam.functionStopMsg.stopParams = *stopParamP;
   msg.msgType = FUNCTION_STOP;
   ret = inu_graph__sendMessage(&msg);
   if (ERRG_SUCCEEDED(ret))
   {
      OS_LYRG_waitEvent( thrStopEvent, INU_GRAPH__SYNC_TIMEOUT_MSEC);
   }
   return ret;
}
/* when a function operation is complete */
ERRG_codeE inu_graph__function_complete_msg_immedate(inu_function *me)
{
#ifdef MSG_DEBUG
   const char *userNameTemp = inu_ref__getUserName((inu_ref*)me);
   printf("send msg from %s: [%s]\n", __FUNCTION__, userNameTemp);
#endif
   inu_graph__runtimeMsg msg;
   msg.msgParam.functionCompleteMsg.function = me;
   msg.msgType = FUNCTION_COMPLETE;
   return inu_graph__sendMessageWithImmediateHandling(&msg);
}

/* when a function operation is complete */
ERRG_codeE inu_graph__function_complete_msg(inu_function *me)
{
#ifdef MSG_DEBUG
   const char *userNameTemp = inu_ref__getUserName((inu_ref*)me);
   printf("send msg from %s: [%s]\n", __FUNCTION__, userNameTemp);
#endif
   inu_graph__runtimeMsg msg;
   msg.msgParam.functionCompleteMsg.function = me;
   msg.msgType = FUNCTION_COMPLETE;
   return inu_graph__sendMessage(&msg);
}


/* When a function is done using data */
ERRG_codeE inu_graph__forward_data_msg(inu_function *me, inu_data *data)
{
#ifdef MSG_DEBUG
   const char *userNameTemp = inu_ref__getUserName((inu_ref*)me);
   printf("send msg from %s: [%s]\n", __FUNCTION__, userNameTemp);
#endif
   inu_graph__runtimeMsg msg;
   msg.msgParam.dataReadyMsg.data = data;
   msg.msgParam.dataReadyMsg.function = me;
   msg.msgType = DATA_READY;
   return inu_graph__sendMessage(&msg);
}
ERRG_codeE inu_graph__forward_data_msg_HandleImmediately(inu_function *me, inu_data *data)
{
#ifdef MSG_DEBUG
   const char *userNameTemp = inu_ref__getUserName((inu_ref*)me);
   printf("send msg from %s: [%s]\n", __FUNCTION__, userNameTemp);
#endif
   inu_graph__runtimeMsg msg;
   msg.msgParam.dataReadyMsg.data = data;
   msg.msgParam.dataReadyMsg.function = me;
   msg.msgType = DATA_READY;
   return inu_graph__sendMessageWithImmediateHandling(&msg);
}



/* called by graph when data needs to be dispatched.
check connected functions, and send data to them */
void inu_graph__dataReady(inu_data *clone, inu_function *sourceFunc)
{
   int found = FALSE, functionOperated = FALSE;
   inu_nodeH nodeH = NULL;
   inu_data   *realData;
   inu_function *destFunc;
   inu_function__stateParam stateParams;
   UINT32 operateModes;
   UINT32 stoppedSyncFunc = 0;

#ifdef DATA_DEBUG
   UINT64 usec;
   OS_LYRG_getUsecTime(&usec);
   if((clone->node.ref.refType) == inu_debug_data_map)
   {
      printf("%llu: _dataReady %s (%d,%p) from function %s (%d)\n",
               usec, clone->node.ref.p_vtable->p_name((inu_ref*)clone),
               clone->node.ref.id,clone,
               sourceFunc->node.ref.p_vtable->p_name((inu_ref*)sourceFunc),
               sourceFunc->node.ref.id);
   }
#endif

   //find the output node using composite
   while (!found)
   {
      nodeH = inu_node__getNextOutputNode(sourceFunc, nodeH);
      if (!nodeH)
      {
         //function not connected to any data or did not find the realData, move to next check
         break;
      }

      if (inu_data__isFromComposite((inu_data*)nodeH, clone))
      {
         //printf("found the real data output of sourceFunc. using composite\n");
         found = TRUE;
         realData = (inu_data*)nodeH;
      }
   }

   nodeH = NULL;
   //find the output node from sourceFunc which has the same source of the cloned data
   while (!found)
   {
      nodeH = inu_node__getNextOutputNode(sourceFunc, nodeH);
      if (!nodeH)
      {
         //function not connected to any data or did not find the realData, free the clone
         //printf("did not find using composite and source, release\n");
         inu_data__freeInternal(clone);
         return;
      }

      //verify its data type, as "virtual" connection like writer->soc ch, or sensor->soc ch are valid
      if ((INU_REF__IS_DATA_TYPE(inu_ref__getRefType((inu_refH)nodeH))) && (inu_data__isSameSource((inu_data*)nodeH, clone)))
      {
         //printf("found the real data output of sourceFunc. using source node\n");
         found = TRUE;
         realData = (inu_data*)nodeH;
      }
   }

   destFunc = (inu_function*)inu_node__getNextOutputNode(realData, NULL);

   //CHECK #2
   if (!destFunc)
   {
      //no functions using this clone, free it
      inu_data__freeInternal(clone);
      return;
   }

   //check if all synced functions are enabled
   //synced function relates to the same graph
   while (destFunc)
   {
      inu_function__getState(destFunc, &stateParams);
      if ((stateParams.state == DISABLE) && (inu_function__isSyncedFunc(destFunc)))
      {
         //found a synced function which is in disable state in this graph
         stoppedSyncFunc |= (1 << inu_graph__getId(destFunc));
      }
      //move to next function
      destFunc = (inu_function*)inu_node__getNextOutputNode(realData, destFunc);
   }

   destFunc = (inu_function*)inu_node__getNextOutputNode(realData, NULL);
   while (destFunc)
   {
      inu_function__getState(destFunc, &stateParams);
      operateModes = inu_function__getOperateModes(destFunc);
      if ((stateParams.state == ENABLE) && (operateModes & (1 << inu_data__getMode(clone))) &&
         ((!inu_function__isSyncedFunc(destFunc)) || ((!(stoppedSyncFunc & (1 << inu_graph__getId(destFunc)))) && (inu_function__isSyncedFunc(destFunc)))))
      {
#ifdef DATA_DEBUG
         if((clone->node.ref.refType) == inu_debug_data_map)
         {
            printf("send data %s (mode %d) to function %s (%d) que\n",clone->node.ref.p_vtable->p_name((inu_ref*)clone),inu_data__getMode(clone),destFunc->node.ref.p_vtable->p_name((inu_ref*)destFunc),destFunc->node.ref.id);
         }
#endif
         inu_function__enqueData(destFunc, realData, clone); //enque in function que
         inu_function__startOperate(destFunc);

         //first function we do not duplicate - just pass
         if (functionOperated)
         {
            inu_data__duplicateInternal(clone);
         }
         functionOperated = TRUE;
      }
      else
      {
#ifdef DATA_DEBUG
         if((clone->node.ref.refType) == inu_debug_data_map)
         {
            if (stateParams.state == DISABLE)
            {
               printf("function %s is disabled\n",destFunc->node.ref.p_vtable->p_name((inu_ref*)destFunc));
            }
            else
            {
               printf("function %s does not operate on this mode (0x%x, %d)\n",
                        destFunc->node.ref.p_vtable->p_name((inu_ref*)destFunc), operateModes, inu_data__getMode(clone));
            }
         }
#endif
      }

      //move to next function
      destFunc = (inu_function*)inu_node__getNextOutputNode(realData, destFunc);
   }

   //CHECK #4 - if all the functions were disabled, free the data
   if (!functionOperated)
   {
#ifdef DATA_DEBUG
      if((clone->node.ref.refType) == inu_debug_data_map)
      {
         printf("no function operated, free data %s\n",clone->node.ref.p_vtable->p_name((inu_ref*)clone));
      }
#endif
      inu_data__freeInternal(clone);
   }
}


/****************************************************************************
*
*  Function Name: inu_graph__handleRuntimeMessage
*
*  Description:
*               1. FUNCTION_OPERATE - this message is sent to a function, with inu_data for the
*                                     function to operate on. First we check if the state of the
*                                     function. If its running, then we que the data (allow handling
*                                     of function jitter). If the que is full, then the data replaces
*                                     the oldest in the que, and frees it.
*                                                       Incase the function is not active, then if all inputs are valid
*                                                       it can be activated
*                     2. DATA_READY - When a function produces and finish working on data, it sends this
*                                              message with the relevant data. The data is dispatched to the next
*                                              functions.
*                3. SET_STATE -
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
static void inu_graph__handleRuntimeMessage(inu_graph__runtimeMsg *msgP)
{
   /*Lock the Graph mutex whilst handling a runtime message*/
   #if DEFSG_IS_GP
   OS_LYRG_lockMutex(&handlingMutex);
   #endif
   switch (msgP->msgType)
   {
   case(DATA_READY):
   {
      inu_graph__dataReady(msgP->msgParam.dataReadyMsg.data, msgP->msgParam.dataReadyMsg.function);
      break;
   }

   case(FUNCTION_START):
   {
      inu_function *function = msgP->msgParam.functionStartMsg.function;
      ((inu_function__VTable*)function->node.ref.p_vtable)->p_start(function, &msgP->msgParam.functionStartMsg.startParams);
      OS_LYRG_setEvent(thrStartEvent);
      break;
   }

   case(FUNCTION_STOP):
   {
      inu_function *function = msgP->msgParam.functionStopMsg.function;
      ((inu_function__VTable*)function->node.ref.p_vtable)->p_stop(function, &msgP->msgParam.functionStopMsg.stopParams);
      OS_LYRG_setEvent(thrStopEvent);
      break;
   }

   case(FUNCTION_COMPLETE):
   {
      inu_function__processComplete(msgP->msgParam.functionCompleteMsg.function);
      break;
   }
   }
   #if DEFSG_IS_GP
   OS_LYRG_unlockMutex(&handlingMutex);
   #endif
}

/****************************************************************************
*
*  Function Name: inu_graph__runTimeThread
*
*  Description:   runTime thread main function. The thread purpose is to manage the execution of the
*                     graph's nodes.
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
static void inu_graph__runTimeThread(void *arg)
{
   inu_graph__runtimeMsg msg;
   UINT32 len;
   int ret;
   (void)arg;

   thrActive = 1;
   while (runTimeThreadActive)
   {
      len = sizeof(inu_graph__runtimeMsg);
      ret = OS_LYRG_recvMsg(&thrGraphMsgQue, (UINT8*)&msg, &len, 300);
      if ((ret == SUCCESS_E) && (len == sizeof(inu_graph__runtimeMsg)))
      {
         inu_graph__handleRuntimeMessage(&msg);
      }
      else
      {
         //if not timeout, send debug msg
         if (len != 0)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "inu_graph__runTimeThread mq receive fail len = %d\n", len);
         }
      }
   }
   thrActive = 0;
}

/****************************************************************************
*
*  Function Name: inu_graph__initRuntimeThread
*
*  Description:   init the graph runtime thread and it's que.
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
static void inu_graph__initRuntimeThread( void )
{
   int ret;
   OS_LYRG_threadParams thrdParams;
   //Event for synchronizing Start/Stop calls
   thrStartEvent = OS_LYRG_createEvent(0);
   if (thrStartEvent == NULL)
      assert(0);
   thrStopEvent = OS_LYRG_createEvent(0);
   if (thrStopEvent == NULL)
      assert(0);


   memset(&thrGraphMsgQue, 0, sizeof(thrGraphMsgQue));
   memcpy(thrGraphMsgQue.name, INU_GRAPH__MSGQ_NAME, MIN(sizeof(thrGraphMsgQue.name), strlen(INU_GRAPH__MSGQ_NAME) + 1));
   thrGraphMsgQue.maxMsgs = 400;
   thrGraphMsgQue.msgSize = sizeof(inu_graph__runtimeMsg);

   ret = OS_LYRG_aquireMutex(&handlingMutex);

   if (ret)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create Graph Mutex\n");
      return;
   }

   ret = OS_LYRG_createMsgQue(&thrGraphMsgQue, 0);

   if (ret)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create msgQ\n");
      return;
   }

   thrdParams.func = (OS_LYRG_threadFunction)inu_graph__runTimeThread;
   thrdParams.id = OS_LYRG_GRAPH_RUNTIME_THREAD_ID_E;
   thrdParams.event = NULL;
   runTimeThreadActive = 1;
   thrActive = 0;

   thrGraphHandle = OS_LYRG_createThread(&thrdParams);

   if (thrGraphHandle == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create thread\n");
      return;
   }
}

/****************************************************************************
*
*  Function Name: inu_graph__deinitRuntimeThread
*
*  Description:   init the graph runtime thread and it's que.
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
static void inu_graph__deinitRuntimeThread()
{
   int ret,totalSleep;

   runTimeThreadActive = 0;
   totalSleep = 0;
   while((thrActive) && (totalSleep < (1000 * 500)))
   {
      totalSleep+=1000;
      OS_LYRG_usleep(1000);
   }
   if (thrActive)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Falied to deinit graph runtime thread \n");
   }
   OS_LYRG_closeThread(thrGraphHandle);

   OS_LYRG_releaseEvent(thrStartEvent,0);
   OS_LYRG_releaseEvent(thrStopEvent,0);
   ret = OS_LYRG_deleteMsgQue(&thrGraphMsgQue);
   if (ret)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to delete msgQ\n");
   }
}

void inu_graph__initEngine( void )
{
   inu_graph__initRuntimeThread();
}

void inu_graph__deinitEngine( void )
{
   inu_graph__deinitRuntimeThread();
}


static ERRG_codeE inu_graph__gpHandleConfigDbMsg(UINT8 *dataP, UINT32 dataLen, inu_graph *me, char *name)
{
   ERRG_codeE ret = SYSTEM__RET_SUCCESS;
   inu_graph__send_config_db_params msgRet;
   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   UINT32 isMainGraph = 0;

   if (strcmp(name,"main") == 0)
   {
      isMainGraph = 1;
   }

   ret = XMLDB_importTbl((const char*)dataP, dataLen, privP->gpDbh);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = SEQ_MNGRG_initDb(&privP->seqDbH, privP->gpDbh, name);
   }
   if (ERRG_SUCCEEDED(ret))
   {
      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
      XMLDB_writeDbToRegs(privP->gpDbh, isMainGraph);
      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      SEQ_MNGRG_disableAllIdveBlocks(privP->seqDbH);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      msgRet.ret = (UINT32)ret;
      inu_ref__copyAndSendDataAsync((inu_ref*)me,INTERNAL_CMDG_SEND_CONFIG_DB_E,&msgRet,NULL,0);
   }

   return (ret);

}


void *inu_graph__getXMLDB( inu_graph *me )
{
   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   return privP->gpDbh;
}

void *inu_graph__getSeqDB( inu_graph *me )
{
   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   return privP->seqDbH;
}

ERRG_codeE inu_graph_reconfigDb(inu_graphH meH)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;

   XMLDB_writeDbToRegs(privP->gpDbh,1);

   ret = SEQ_MNGRG_disableAllIdveBlocks(privP->seqDbH);

   return ret;
}

static ERRG_codeE inu_graph__setSysConfig(inu_graph *me, inu_nucfgH nucfgH)
{
   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   ERRG_codeE ret;
   const char *xmlPath;
   const char *name;
   privP->gpDbh = NUCFG_getCurrDbH(nucfgH);
   UINT32 isMainGraph = 0;

   NUCFG_getXmlPathAndName(nucfgH,&xmlPath,&name);
   ret = SEQ_MNGRG_initDb(&privP->seqDbH, NUCFG_getCurrDbH(nucfgH), name);
   if (ERRG_SUCCEEDED(ret))
   {
      #ifdef HELSINKI_MAIN_GRAPH_NAME_1
      if (strcmp(name, HELSINKI_MAIN_GRAPH_NAME_1) == 0 || strcmp(name, HELSINKI_MAIN_GRAPH_NAME_2) == 0 || strcmp(name, HELSINKI_MAIN_GRAPH_NAME_3) == 0  )
      {
          isMainGraph = 1;
      }
      #else
      if (strcmp(name, "main") == 0 )
      {
          isMainGraph = 1;
      }
      #endif

      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
      XMLDB_writeDbToRegs(privP->gpDbh, isMainGraph);
      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
      ret = SEQ_MNGRG_disableAllIdveBlocks(privP->seqDbH);
   }
   return ret;
}

#else
static ERRG_codeE inu_graph__setSysConfig(inu_graph *me, inu_nucfgH nucfgH)
{
   const char *buf;
   UINT32 size;
   inu_graph__send_config_db_params sendConfigDBparam;
   UINT32 *ackP = NULL;
   UINT32 timeoutMsec = 4000;
   ERRG_codeE          ret = SYSTEM__RET_SUCCESS;
   UINT32  startSec, deltaMsec = 0;
   UINT16  startMsec;
   const char *xmlpath;
   const char *name;
   enum { SLEEP_USEC_E = (15 * 1000) };

   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   memset(&sendConfigDBparam,0,sizeof(sendConfigDBparam));
   NUCFG_getXmlPathAndName(nucfgH, &xmlpath, &name);
   strcpy((char*)sendConfigDBparam.name, name);
   XMLDB_exportTblP(NUCFG_getCurrDbH(nucfgH), &buf, &size);

   ret = inu_ref__copyAndSendDataAsync((inu_ref*)me, INTERNAL_CMDG_SEND_CONFIG_DB_E, &sendConfigDBparam, (UINT8*)buf, size);
   //printf("ret = %d size = %d\n", ret, size);// open this print to know the size of data, and resize the mem pool accordingly at inu_graph c'tor
   if (ERRG_FAILED(ret))
   {
      //LOGG_PRINT(LOG_ERROR_E, NULL, "Falied to send config DB");
      return ret;
   }
   ackP = &privP->waitAck;
   if (ackP)
   {
      //wait for ACK
      OS_LYRG_getTime(&startSec, &startMsec);
      do
      {
         if (*ackP != 0)
         {
//            LOGG_PRINT(LOG_DEBUG_E, NULL, "Ack recieved from host - db config proccess done on target");
            ret = (ERRG_codeE)*ackP;
            *ackP = 0;
            break;
         }
         OS_LYRG_usleep(SLEEP_USEC_E);
         deltaMsec = OS_LYRG_deltaMsec(startSec, startMsec);
//         LOGG_PRINT(LOG_DEBUG_E, NULL, "deltaMsec %d timeoutMsec %d\n", deltaMsec, timeoutMsec);
      } while (deltaMsec <= timeoutMsec);
      if (deltaMsec > timeoutMsec)
         ret = SYSTEM__ERR_TIMEOUT;
   }

   return ret;
}
#endif

static void inu_graph__dtor(inu_ref *me)
{
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)me)->privP;
   BOOLEAN standAloneMode = inu_ref__standAloneMode(me);
   inu_ref__vtable_get()->p_dtor(me);
   inu_graph__releaseId(privP->id);
#ifdef GRAPH_NODES_AS_LIST
#if 0
   inu_node__link      *runner = privP->nodesListHead;
   while(runner)
   {
      inu_factory__delete((inu_ref*)runner->node,0);
      privP->nodesListHead = privP->nodesListHead->next;
      free(runner);
      runner = privP->nodesListHead;
   }
   #else // revert order to delet nodes for dependence issue( mem buffer/pool issue)
   inu_node__link      *head = privP->nodesListHead;
   inu_node__link      *last = head;
   inu_node__link      *last_pre = head;

   while (head) {
      while (last->next) {
         last_pre = last;
         last = last->next;
      }
      inu_factory__delete((inu_ref*)last->node,0);
      free(last);
      
      if (last == head) {
         head = NULL;
         break;
      }
      last_pre->next = NULL;
      last = head;
   }
   privP->nodesListHead = NULL;
   #endif
#else
   UINT32 i;
   for (i = 0; i < INU_GRAPH_MAX_NUM_NODES; i++)
   {
      if (privP->nodesList[i])
      {
         inu_factory__delete((inu_ref*)privP->nodesList[i],0);
         privP->nodesList[i] = NULL;
      }
   }
#endif
#if DEFSG_IS_GP
#ifndef NO_THREAD
   GME_MNGRG_resetHw(false);
#endif
   SEQ_MNGRG_deinitDb(privP->seqDbH);
   if (!standAloneMode)
   {
      XMLDB_close(privP->gpDbh);
   }
#endif
   free(privP);
}

/* Constructor */
ERRG_codeE inu_graph__ctor(inu_graph *me, inu_graph__CtorParams *ctorParamsP)
{
   ERRG_codeE ret;
   inu_graph__privData *privP;

   sprintf(ctorParamsP->ref_params.userName, "GRAPH");
   ret = inu_ref__ctor(&me->ref, &ctorParamsP->ref_params);
   if (ERRG_FAILED(ret))
   {
      return ret;
   }
   privP = (inu_graph__privData*)malloc(sizeof(inu_graph__privData));
   if (!privP)
   {
      return INU_GRAPH__ERR_OUT_OF_MEM;
   }
   memset(privP, 0, sizeof(inu_graph__privData));
   me->privP = privP;
   memcpy(&privP->ctorParams, ctorParamsP, sizeof(inu_graph__CtorParams));
   privP->id = inu_graph__acquireId();

   ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_HEAP_E, INU_GRAPH__MEM_POOL_BUFF_SIZE_BYTES, INU_GRAPH__MEM_POOL_BUFF_NUM, NULL, 0, &privP->lutPoolH);
   if(ERRG_FAILED(ret))
   {
      return ret;
   }
   ret = inu_ref__addMemPool((inu_ref*)me, MEM_POOLG_TYPE_ALLOC_HEAP_E, INU_GRAPH__MEM_POOL_CONFIG_DB_BUFF_SIZE_BYTES, INU_GRAPH__MEM_POOL_BUFF_NUM, NULL, 0, &privP->dbPoolH);
   if(ERRG_FAILED(ret))
   {
      return ret;
   }

#if DEFSG_IS_GP
   if (inu_ref__standAloneMode(me))
#endif
   {
      if (ctorParamsP->graphXMLpath)
      {
          memcpy(privP->graphXMLpath, ctorParamsP->graphXMLpath, strlen(ctorParamsP->graphXMLpath) + 1);
      }
      privP->saveXmlMod = ctorParamsP->saveXmlMod;
   }

#if DEFSG_IS_HOST
   if (ctorParamsP->socxml_path)
   {
      // copy specific soc xml path
      memcpy(privP->socxml_path, ctorParamsP->socxml_path, strlen(ctorParamsP->socxml_path) + 1);
   }
   else
   {
      //auto create xml paths according to bootid and bootpath
      inu_graph__createXmlPaths(me);
   }

   // copy calibration
   if (ctorParamsP->calibPathsP)
   {
      ret = CALIBG_getCalibData(ctorParamsP->calibPathsP->calibrationPath, &privP->calibSectionsData, &privP->numOfSections);
      inu_device__saveCalibPath((inu_device*)inu_ref__getDevice((inu_ref*)me),ctorParamsP->calibPathsP->calibrationPath, privP->numOfSections > 0);
   }

   privP->waitAck = 0;
#else
   if (inu_ref__standAloneMode(me))
   {
      if (ctorParamsP->socxml_path)
      {
         // copy specific soc xml path
         memcpy(privP->socxml_path, ctorParamsP->socxml_path, strlen(ctorParamsP->socxml_path) + 1);
      }
      if (ctorParamsP->calibPathsP)
      {
         ret = CALIBG_getCalibData(ctorParamsP->calibPathsP->calibrationPath, &privP->calibSectionsData, &privP->numOfSections);
      }
      inu_device__saveCalibPath((inu_device*)inu_ref__getDevice((inu_ref*)me), ctorParamsP->calibPathsP->calibrationPath, privP->numOfSections > 0);
   }
   else
   {
      ret = XMLDB_open(&privP->gpDbh);
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
   }
#endif
   return ret;
}


static int inu_graph__rxData(inu_ref *me, UINT32 msgCode, UINT8 *msgP, UINT8 *dataP, UINT32 dataLen, void **bufDescP)
{
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)me)->privP;
   int ret;
   ret = inu_ref__vtable_get()->p_rxAsyncData(me, msgCode, msgP, dataP, dataLen,bufDescP);
   switch (msgCode)
   {
      case(INTERNAL_CMDG_SEND_CONFIG_DB_E):
      {
         inu_graph__send_config_db_params *msgParamsP = (inu_graph__send_config_db_params*)msgP;
#if DEFSG_IS_GP
         FIX_UNUSED_PARAM_WARN(privP);
         ret = inu_graph__gpHandleConfigDbMsg(dataP, dataLen, (inu_graph*)me, msgParamsP->name);
#else
         privP->waitAck = msgParamsP->ret;
         assert(msgParamsP->ret != 0);
#endif
      }
      default:
      break;
   }
   return ret;
}


void inu_graph__vtable_init(void)
{
   if (!_bool_vtable_initialized)
   {
      inu_ref__vtableInitDefaults(&_vtable);
      _vtable.p_name = inu_graph__name;
      _vtable.p_ctor = (inu_ref__Ctor*)inu_graph__ctor;
      _vtable.p_dtor = inu_graph__dtor;
      _vtable.p_rxAsyncData = inu_graph__rxData;
      inu_graph__initIds();
      _bool_vtable_initialized = true;
   }
}

const inu_ref__VTable *inu_graph__vtable_get(void)
{
   inu_graph__vtable_init();
   return (const inu_ref__VTable*)&_vtable;
}

UINT32 inu_graph__getId(inu_function *me)
{
   inu_graph *graph = (inu_graph*)inu_node__getGraph(me);
   inu_graph__privData *privP = (inu_graph__privData*)graph->privP;
   return privP->id;
}


#if DEFSG_IS_HOST
static void inu_graph__hostCreateXmlPath(char *out_xmlPath, const char *bootPath, int bootId)
{
   char str_boot_id[15];
   char str_path_boot_id[15];

   sprintf(str_boot_id, "boot%d", bootId);
   sprintf(str_path_boot_id, "boot%d", (bootId / 100) * 100);

   strcpy(out_xmlPath, bootPath);
   strcat(out_xmlPath, PATH_SEPARATOR);
   strcat(out_xmlPath, "NU4000");
   strcat(out_xmlPath, PATH_SEPARATOR);
   strcat(out_xmlPath, str_path_boot_id);
   strcat(out_xmlPath, PATH_SEPARATOR);
   strcat(out_xmlPath, SYSTEM_XML_FILE_NAME);
   strcat(out_xmlPath, "_");
   strcat(out_xmlPath, str_boot_id);
   strcat(out_xmlPath, SYSTEM_XML_FILE_EXTENSION);
}

void inu_graph__createXmlPaths(inu_graph *me)
{
   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   //if no specific file is given, go to default xml location by bootid and bootpath
   if (!privP->socxml_path)
   {
      inu_graph__hostCreateXmlPath(privP->socxml_path, inu_device__getBootPathFromRef((inu_ref*)me), inu_device__getBootIdFromRef((inu_ref*)me));
      printf("XML from: %s, path: %s result: %d\n", privP->socxml_path, inu_device__getBootPathFromRef((inu_ref*)me), inu_device__getBootIdFromRef((inu_ref*)me));
   }
   printf("XML modified: %s (path: %s)\n", privP->socxml_mod_path, inu_device__getBootPathFromRef((inu_ref*)me));
}
#endif

ERRG_codeE inu_graph__delete(inu_graphH graphH)
{
   if (!graphH)
      return INU_GRAPH__ERR_NULL_PTR;
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)graphH)->privP;
   NUCFG_deInit(privP->cfgH);
   return inu_factory__delete((inu_ref*)graphH, 0);
}

/* same as inu_graph__delete, but without destroying the nucfg */
ERRG_codeE inu_graph__close(inu_graphH graphH)
{
   if (!graphH)
      return INU_GRAPH__ERR_NULL_PTR;
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)graphH)->privP;
   return inu_factory__delete((inu_ref*)graphH, 0);
}

static ERRG_codeE inu_graph__createXmlMod(const char *xmlPath, char *xmlModPath, CALIB_sectionDataT* calibSectionsData, int NumOfSections, int debugMode, const char *name, inu_nucfgH *nucfg)
{
   // create xml'
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   char *xmlbuf = NULL;
   unsigned int xmlsize;
   FILE *file;
   NUCFG_calibT calib;
   if (calibSectionsData)
   {
      calib.numSections = NumOfSections;
      calib.tbl = calibSectionsData;
   }
   else
   {
      calib.numSections = 0;
      calib.tbl = NULL;
   }

   XMLDB_setDebugMode(debugMode);
   if (XMLDB_loadNuSocxml(&xmlbuf, &xmlsize, xmlPath) < 0)
   {
      //LOGG_PRINT(LOG_INFO_E, NULL, "Failed to load xml from path: %s:\n", xmlPath);
      printf("Failed to load xml from path: %s:\n", xmlPath);
      if ((file = fopen(xmlModPath, "r")) != NULL)
      {
         fclose(file);
         unlink(xmlModPath);
      }

      if (xmlbuf)
      {
         free(xmlbuf);
      }
      return SYSTEM__ERR_UNEXPECTED;
   }

   ret = NUCFG_init(&calib, xmlbuf, xmlPath, name, nucfg);

   if (xmlbuf)
   {
       free(xmlbuf);
   }
   return ret;
}

static void inu_graph__createXmlModPath(char *out_xmlModPath, const char *path, const char *name)
{
   strcpy(out_xmlModPath, path);
   strcat(out_xmlModPath, PATH_SEPARATOR);
   strcat(out_xmlModPath, SYSTEM_XML_MOD_FILE_NAME);
   strcat(out_xmlModPath, "_");
   strcat(out_xmlModPath, name);
   strcat(out_xmlModPath, SYSTEM_XML_FILE_EXTENSION);
}

static ERRG_codeE inu_graph__xmlParse(inu_graph *me, int debugMode, const char *name, inu_nucfgH *nucfg)
{
   ERRG_codeE ret;

   inu_graph__privData *privP = (inu_graph__privData*)me->privP;
   ret = inu_graph__createXmlMod(privP->socxml_path, privP->socxml_mod_path, privP->calibSectionsData, privP->numOfSections, debugMode, name, nucfg);
   return ret;
}

static ERRG_codeE inu_graph__saveXmlMod(inu_graphH meH, const char *xmlPath, inu_nucfgH nucfgH)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   char *xmlbuf = NULL;
   const char *name;
   char xmlModPath[1024] = {0};
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
   unsigned int xmlsize;

   NUCFG_getXmlPathAndName(nucfgH, &xmlPath, &name);
   inu_graph__createXmlModPath(xmlModPath, inu_device__getBootPathFromRef((inu_ref*)meH), name);
   if (XMLDB_loadNuSocxml(&xmlbuf, &xmlsize, xmlPath) == 0)
   {
      ret = NUCFG_saveModDbHAndWriteXml(privP->cfgH, xmlbuf, xmlModPath);
   }

   if(xmlbuf)
   {
      free(xmlbuf);
   }
   return ret;
}

ERRG_codeE inu_graph__finalize(inu_graphH meH, inu_nucfgH nucfg)
{
   ERRG_codeE ret;
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
   unsigned int groupCtr;
   outChannelT *chTbl;

   ret = NUCFG_applyIspLinkToWriterData(nucfg);

   //apply user api's
   ret = NUCFG_applyDpeFunc(nucfg);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "failed to apply DPE config, exit\n");
      return ret;
   }
   ret = NUCFG_applyChannelDimentions(nucfg);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "failed to apply channel dimentions, exit\n");
      return ret;
   }
   ret = NUCFG_applyGroupsMode(nucfg);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "failed to apply group mode, exit\n");
      return ret;
   }

   //view\query channels and input table
   //chTbl = (outChannelT*)malloc(sizeof(outChannelT) * NUCFG_getNumChannels(nucfg));
   //ret = NUCFG_getChannelTable(nucfg, chTbl);
   //if (ERRG_SUCCEEDED(ret))
   //{
   //   for (groupCtr = 0; groupCtr < NUCFG_getNumGroups(nucfg); groupCtr++)
   //   {
   //      NUCFG_showChannelGroup(nucfg, LOG_INFO_E, groupCtr);
   //   }
   //}
   //free(chTbl);
#if !DEFSG_IS_GP
   extInterleaveChannels = 0;
   ret = inu_graph__setupExtIntInfo((inu_graph*)meH, privP->graphXMLpath);
   if (!ERRG_SUCCEEDED(ret))
   {
       printf("Error setup extended DMA Interleaving info, no stream will use DMA Interleaving mode.\n");
   }
#endif
   ret = inu_graph__addNodesFromXml((inu_graph*)meH, nucfg);
   if (ERRG_SUCCEEDED(ret))
   {
      if (privP->saveXmlMod)
         ret = inu_graph__saveXmlMod(meH, privP->socxml_path, nucfg);
   }
   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_graph__setSysConfig((inu_graph*)meH, nucfg);
   }
   ret = inu_graph__createPostProcessNodes((inu_graph*)meH, privP->graphXMLpath);
#if DEFSG_IS_HOST
   if (ERRG_SUCCEEDED(ret))
   {
       //show the graph
       if (privP->saveXmlMod)
       {
           INU_DEFSG_logLevelE loglevel;
           LOGG_getDbgLevel(&loglevel);
           if (loglevel >= LOG_DEBUG_E)
               inu_graph__showGraph(meH);
       }
   }
#endif
   //
   inu_graph__bindIspOutputToWriter((inu_graph*)meH, nucfg);

   return ret;
}


ERRG_codeE inu_graph__new(inu_graphH *meH, inu_graph__initParams *initParamsP, inu_nucfgH *nucfg)
{
   ERRG_codeE ret;
   inu_graph__CtorParams ctorParams;
   inu_graph *me;

   ctorParams.socxml_path = initParamsP->socxml_path;
   ctorParams.ref_params.device = initParamsP->deviceH;
   ctorParams.calibPathsP = initParamsP->calibPathsP;
   ctorParams.graphXMLpath = initParamsP->graphXMLpath;
   ctorParams.saveXmlMod = initParamsP->saveXmlMod;

   ret = inu_factory__new((inu_ref**)&me, &ctorParams, INU_GRAPH_REF_TYPE);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_graph__xmlParse((inu_graph*)me, initParamsP->debugMode, initParamsP->name, nucfg);
      if (ERRG_SUCCEEDED(ret))
      {
         if (initParamsP->calibPathsP != NULL)
         {
            inu_device__readLutFiles(initParamsP->deviceH,(char *)initParamsP->calibPathsP->calibrationPath[0], 0);
         }

         inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)me)->privP;
         privP->cfgH = *nucfg;
         *meH = (inu_graphH*)me;
      }
   }

   return ret;
}

/* subset of required steps from inu_graph__new and inu_graph__finalize in order to create a graph with existing nucfg */
ERRG_codeE inu_graph__renew(inu_graphH *meH, inu_graph__initParams *initParamsP, inu_nucfgH *nucfg)
{
   ERRG_codeE ret;
   inu_graph__CtorParams ctorParams;
   inu_graph *me;

   if (!nucfg || !*nucfg)
   {
      return INU_GRAPH__ERR_INVALID_ARGS;
   }

   // taken from inu_graph__new:

   ctorParams.socxml_path = initParamsP->socxml_path;
   ctorParams.ref_params.device = initParamsP->deviceH;
   ctorParams.calibPathsP = initParamsP->calibPathsP;
   ctorParams.graphXMLpath = initParamsP->graphXMLpath;
   ctorParams.saveXmlMod = initParamsP->saveXmlMod;

   ret = inu_factory__new((inu_ref**)&me, &ctorParams, INU_GRAPH_REF_TYPE);
   if (ERRG_SUCCEEDED(ret))
   {
      if (initParamsP->calibPathsP != NULL)
      {
         inu_device__readLutFiles(initParamsP->deviceH,(char *)initParamsP->calibPathsP->calibrationPath[0], 0);
      }

      inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)me)->privP;
      privP->cfgH = *nucfg;
      *meH = (inu_graphH*)me;
   }

   // taken from inu_graph__finalize:

   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_graph__addNodesFromXml((inu_graph*)me, *nucfg);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      ret = inu_graph__setSysConfig(me, *nucfg);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      inu_graph__privData *privP = (inu_graph__privData*)me->privP;

      ret = inu_graph__createPostProcessNodes(me, privP->graphXMLpath);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      inu_graph__bindIspOutputToWriter(me, *nucfg);
   }
   return ret;
}

static void inu_graph__fillSensorParamsFromXml(unsigned int inst, inu_sensor__parameter_list_t *paramsListP, inu_nucfgH nucfg)
{
   ERRG_codeE ret;
   UINT32 analogGain;

   paramsListP->id                  = (inu_sensor__id_e)inst;
   paramsListP->defaultExp          = 5000;
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_FUNCTION_E), (UINT32*)&paramsListP->function);
   ret = XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_FPS_E), &paramsListP->fps);
   if (ERRG_FAILED(ret))
   {
      paramsListP->fps = NUCFG_DEFAULT_FPS; //default fps
   }
   //XMLDB_getValue(NUCFG_getCurrDbH(), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_VERT_OFFSET_E), &sensorInitParams.paramsList.exposure.exposure);todo
   //XMLDB_getValue(NUCFG_getCurrDbH(), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_EXP_MODE_E), (UINT32*)&paramsListP->exp_mode);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_DEFAULT_GAIN_E), &analogGain);
   paramsListP->gain.analog = (float)analogGain;
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_DEFAULT_EXPOSURE_E),&paramsListP->defaultExp);
   ret = XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_TOTAL_EXP_TIME_PER_SEC_E), &paramsListP->totalExpTimePerSec);
   if (ERRG_FAILED(ret))
   {
      paramsListP->totalExpTimePerSec = INU_DEFSG_INVALID;
   }
   ret = XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_EXP_TIME_PER_DUTY_CYCLE_E), &paramsListP->expTimePerDutyCycle);
   if (ERRG_FAILED(ret))
   {
      paramsListP->expTimePerDutyCycle = INU_DEFSG_INVALID;
   }
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_OP_MODE_E), (UINT32*)&paramsListP->op_mode);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_ORIENTATION_E), (UINT32*)&paramsListP->orientation);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_POWER_HZ_E), (UINT32*)&paramsListP->power_hz);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_MODEL_E), (UINT32*)&paramsListP->model);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_LENS_TYPE_E), (UINT32*)&paramsListP->lenType);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_ROLE_E), (UINT32*)&paramsListP->role);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_TABLE_TYPE_E), (UINT32*)&paramsListP->tableType);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_VERT_OFFSET_E), &paramsListP->vertical_offset);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_I2C_NUM_E), &paramsListP->i2cNum);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_POWER_GPIO_MASTER_E), &paramsListP->power_gpio_master);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_POWER_GPIO_SLAVE_E), &paramsListP->power_gpio_slave);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_FSIN_GPIO_E), &paramsListP->fsin_gpio);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_EXP_MODE_E), &paramsListP->exp_mode);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_CLK_SRC_E), &paramsListP->sensor_clk_src);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_DIVIDER_E), &paramsListP->sensor_clk_divider);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_STROBE_NUM_E), &paramsListP->strobeNum);
   XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_PROJECTOR_INST_E), (UINT32*)&paramsListP->projector_inst);

   ret = XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_TRIGGER_SRC_E), (UINT32*)&paramsListP->trigger_src);
   if (ERRG_FAILED(ret))
      paramsListP->trigger_src = INU_SENSOR__TRIGGER_SRC_DISABLE_E;
   ret = XMLDB_getValue(NUCFG_getCurrDbH(nucfg), NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_TRIGGER_DELAY_E), (UINT32*)&paramsListP->trigger_delay);
   if (ERRG_FAILED(ret))
      paramsListP->trigger_delay = 0;

   NUCFG_getSensorSizes(paramsListP->model, paramsListP->op_mode, &paramsListP->sensor_width, &paramsListP->sensor_height);
}

ERRG_codeE inu_graph__addNodesFromXml(inu_graph *me, inu_nucfgH nucfgH)
{
   ERRG_codeE ret;
   unsigned int i, j, z, x, master, chnls;
   outChannelT                *chTbl = (outChannelT*)malloc(sizeof(outChannelT) * NUCFG_getNumChannels(nucfgH));
   NUCFG_inputT               *input_tbl = (NUCFG_inputT*)malloc(sizeof(NUCFG_inputT) * NUCFG_getNumInputs(nucfgH));
   inu_nodeH                  *inputsH = (inu_nodeH*)malloc(sizeof(inu_nodeH) * NUCFG_getNumInputs(nucfgH));
   inu_sensors_groupH         *sensors_groupsH = (inu_sensors_groupH*)malloc(sizeof(inu_sensors_groupH) * NUCFG_getNumGroups(nucfgH));
   unsigned int                inst;
   NUCFG_inputTypeE            type;
   inu_nodeH                   channel = NULL;
   inu_nodeH                   data = NULL;
   const char                 *configName, *xmlPath;

   NUCFG_getXmlPathAndName(nucfgH,&xmlPath,&configName);
   memset(inputsH, 0, sizeof(inu_nodeH*) * NUCFG_getNumInputs(nucfgH));
   memset(sensors_groupsH, 0, sizeof(inu_sensors_groupH*) * NUCFG_getNumGroups(nucfgH));

   NUCFG_getInputTable(nucfgH, input_tbl);
   ret = NUCFG_getChannelTable(nucfgH, chTbl);
   if (ERRG_FAILED(ret))
   {
       return ret;
   }
   //create sensor groups
   chnls = NUCFG_getNumChannels(nucfgH);
   for (i = 0; i < chnls; i++)
   {
      for (j = 0; j < chTbl[i].numInputs; j++)
      {
         inst = chTbl[i].inputs[j].inst;
         type = chTbl[i].inputs[j].type;
         if(type == NUCFG_INPUT_TYPE_SENSOR_E_E)
         {
            //create sources group or use if already exists
             // to avoid accessing out of bound of array.  david @ 2022.09.24
             if (chTbl[i].modeGroup == NUCFG_CHAN_MODE_NA)
                 continue;

            if (!sensors_groupsH[chTbl[i].modeGroup])
            {
               //find the master sensor, we will need it because its parameters define the group handle
               //to do this, we find other channels with the same group, and check their inputs until we
               //find the master
               master = 0xFF;
               for (z = 0; (z < NUCFG_getNumChannels(nucfgH)) && (master == 0xFF); z++)
               {
                  if (chTbl[z].modeGroup == chTbl[i].modeGroup)
                  {
                     for (x = 0; (x < chTbl[z].numInputs) && (master == 0xFF); x++)
                     {
                        if (input_tbl[chTbl[z].inputs[x].inst].params.sensorParams.function == 0) //todo use enum
                        {
                           //found the master, can stop searching
                           master = chTbl[z].inputs[x].inst;
                        }
                     }
                  }
               }
               if (master == 0xFF)
                  assert(0);

               inu_sensor__parameter_list_t masterParamsList;
               inu_graph__fillSensorParamsFromXml(master, &masterParamsList, nucfgH);
               ret = inu_graph__createSensorGroup(me, &sensors_groupsH[chTbl[i].modeGroup], &masterParamsList, chTbl[i].modeGroup);
               if (ERRG_FAILED(ret))
               {
                  assert(0);
               }
               inu_graph__insertNode(me, sensors_groupsH[chTbl[i].modeGroup]);
            }
         }
      }
   }
   //create inputs:
   //traverse over the channel table, find all channels with inputs.
   //each input, check type and create the inu_ref accordingly (if the input was not already created)
   for (i = 0; i < NUCFG_getNumChannels(nucfgH); i++)
   {
      for (j = 0; j < chTbl[i].numInputs; j++)
      {
         inst = chTbl[i].inputs[j].inst;
         if (!inputsH[inst])
         {
            if (chTbl[i].inputs[j].type == NUCFG_INPUT_TYPE_INJECT_E_E)
            {
               inu_image__descriptor imageDescriptor;
               imageDescriptor.bitsPerPixel = chTbl[i].resolution.bpp;
               imageDescriptor.format = chTbl[i].format;
               imageDescriptor.height = chTbl[i].resolution.height;
               imageDescriptor.width = chTbl[i].resolution.width / chTbl[i].numInterleaved;
               imageDescriptor.x = 0; //todo 2d
               imageDescriptor.y = 0; //todo 2d
               imageDescriptor.stride = chTbl[i].resolution.width; //todo 2d
               imageDescriptor.bufferHeight = 0; //todo 2d
               imageDescriptor.realBitsMask = 0; //todo repacker
               imageDescriptor.numInterleaveImages = chTbl[i].numInterleaved;
               ret = inu_graph__createWriter(me, &inputsH[inst], &imageDescriptor, inst, configName);
               if (ERRG_FAILED(ret))
                  return ret;
            }
            else if (chTbl[i].inputs[j].type == NUCFG_INPUT_TYPE_SENSOR_E_E)
            {
                // to avoid accessing out of bound of array.  david @ 2022.09.24
                if (chTbl[i].modeGroup == NUCFG_CHAN_MODE_NA)
                    continue;

               inu_sensor__parameter_list_t paramsList;
               inu_graph__fillSensorParamsFromXml(inst, &paramsList, nucfgH);
               chTbl[i].inputs[j].params.sensorParams.model = paramsList.model; //only need this line for the ov2685 workaround
               paramsList.function = INU_SENSOR__FUNCTION_SLAVE; //set sensors as slaves
               paramsList.strobeNum = INU_DEFSG_STROBES_INVALID; //only groups has a strobe
               paramsList.groupId = chTbl[i].modeGroup;
               ret = inu_graph__createSensor(me, &inputsH[inst], &paramsList, inst);
               if (ERRG_FAILED(ret))
               {
                  assert(0);
               }

               //bind sources to sources group
               inu_node__bindNodes(sensors_groupsH[chTbl[i].modeGroup], inputsH[inst]);
            }
            else if (chTbl[i].inputs[j].type == NUCFG_INPUT_TYPE_GEN_E_E)
            {

                continue;
            }
            else
            {
               assert(0);
            }
            inu_graph__insertNode(me, inputsH[inst]);
         }
      }
   }
   //create HW channels
   for (i = 0; i < NUCFG_getNumChannels(nucfgH); i++)
   {
      if (chTbl[i].numInputs > 0)
      {
         if (chTbl[i].outputType == NUCFG_OUTPUT_TYPE_AXI_RD_E_E)
         {
            //create the channel and its output data
            if (chTbl[i].format == NUCFG_FORMAT_HISTOGRAM_E)
            {
               ret = inu_graph__createHistogram(me, &channel, (chTbl[i].id - 12), configName);
               if (ERRG_FAILED(ret))
                  return ret;
               ret = inu_graph__createHistData(me, &data, channel, (chTbl[i].id - 12));
               if (ERRG_FAILED(ret))
                  return ret;
            }
            else if (chTbl[i].format == NUCFG_FORMAT_GEN_672_E)
            {
               inu_cva_data__descriptor_t cvaDescriptor;
               inu_image__descriptor      imageDescr;
               inu_nodeH                  iicData;
               ret = inu_graph__createCvaChannel(me, &channel, chTbl[i].id, configName);
               if (ERRG_FAILED(ret))
                  return ret;
               cvaDescriptor.bpp = chTbl[i].resolution.bpp;
               cvaDescriptor.format = chTbl[i].format;
               cvaDescriptor.height = chTbl[i].resolution.height;
               cvaDescriptor.width = chTbl[i].resolution.width;
               cvaDescriptor.kpNum = 0;
               cvaDescriptor.type = INU_CVA_DATA_DOG_FREAK;
               ret = inu_graph__createCvaData(me, &data, channel, &cvaDescriptor, chTbl[i].id);
               if (ERRG_FAILED(ret))
                  return ret;

               imageDescr.bitsPerPixel = 32;
               imageDescr.height = chTbl[i].resolution.height;
               imageDescr.width = chTbl[i].resolution.width;
               imageDescr.bufferHeight = imageDescr.height;
               imageDescr.stride = imageDescr.width;
               imageDescr.format = chTbl[i].format;
               if (imageDescr.height > (1 << 16))
                  assert(0);

               ret = inu_graph__createImage(me, &iicData, channel, &imageDescr, chTbl[i].id, (UINT16)imageDescr.height);
               if (ERRG_FAILED(ret))
                  return ret;

               inu_graph__insertNode(me, channel);
               inu_graph__insertNode(me, data);
               inu_graph__insertNode(me, iicData);
               inu_node__bindNodes(channel, data);
               inu_node__bindNodes(channel, iicData);


            }
            else if (chTbl[i].format < NUCFG_FORMAT_HISTOGRAM_E)
            {
               ret = inu_graph__createSocChannel(me, &channel, chTbl[i].id, configName);
               if (ERRG_FAILED(ret))
               {
                  return ret;
               }
               inu_image__descriptor   imageDescriptor;
               imageDescriptor.bitsPerPixel = chTbl[i].resolution.bpp;
               imageDescriptor.format = chTbl[i].format;
               imageDescriptor.height = (chTbl[i].resolution.height);
               // modified to avoid deviding by 0; david @ 2022.09.25
               if (chTbl[i].numInterleaved > 0)
                   imageDescriptor.width = chTbl[i].resolution.width / chTbl[i].numInterleaved;
               else
                   imageDescriptor.width = chTbl[i].resolution.width;

               imageDescriptor.x = chTbl[i].resolution.x;
               imageDescriptor.y = chTbl[i].resolution.y;
               imageDescriptor.stride = (chTbl[i].resolution.stride);
               imageDescriptor.bufferHeight = chTbl[i].resolution.bufferHeight;
               imageDescriptor.realBitsMask = 0; //todo repacker
               imageDescriptor.numInterleaveImages = chTbl[i].numInterleaved;
               imageDescriptor.interMode = chTbl[i].intreleavedMode;
               imageDescriptor.formatDiscriptor = chTbl[i].formatDiscriptor;
               if (chTbl[i].numLinesPerChunk > (1 << 16))
                  assert(0);
               // modified to avoid deviding by 0 in createImage; david @ 2022.09.25
               if (chTbl[i].numLinesPerChunk == 0)
                   continue;
               ret = inu_graph__createImage(me, &data, channel, &imageDescriptor, chTbl[i].id, (UINT16)chTbl[i].numLinesPerChunk);
               if (ERRG_FAILED(ret))
                  return ret;
               if (chTbl[i].inputs[0].type == NUCFG_INPUT_TYPE_GEN_E_E)
               {
                  inu_graph__insertNode(me, channel);
                  inu_graph__insertNode(me, data);
                  inu_node__bindNodes(channel, data);
                  continue;
               }
            }

            if (chTbl[i].format != NUCFG_FORMAT_GEN_672_E)
            {
               if ((data == NULL) || (channel == NULL))
                  assert(0);
               //insert the channel and data, and bind them
               inu_graph__insertNode(me, channel);
               inu_graph__insertNode(me, data);
               inu_node__bindNodes(channel, data);
            }

         }
         else if (chTbl[i].outputType == NUCFG_OUTPUT_TYPE_MIPI_TX_E)
         {
            ret = inu_graph__createMipiChannel(me, &channel, chTbl[i].id - 21, configName);
            if (ERRG_FAILED(ret))
               return ret;
            //insert the channel
            inu_graph__insertNode(me, channel);
         }
         else if (chTbl[i].outputType == NUCFG_OUTPUT_TYPE_ISP_RD_E)
         {

             ret = inu_graph__createIspChannel(me, &channel, chTbl[i].id, configName);
             if (ERRG_FAILED(ret))
             {
                 return ret;
             }
             inu_image__descriptor   imageDescriptor;
             imageDescriptor.bitsPerPixel = chTbl[i].resolution.bpp;
             imageDescriptor.format = chTbl[i].format;
             imageDescriptor.height = (chTbl[i].resolution.height);
             imageDescriptor.width = chTbl[i].resolution.width / chTbl[i].numInterleaved;
             imageDescriptor.x = chTbl[i].resolution.x;
             imageDescriptor.y = chTbl[i].resolution.y;
             imageDescriptor.stride = (chTbl[i].resolution.stride);
             imageDescriptor.bufferHeight = chTbl[i].resolution.bufferHeight;
             imageDescriptor.realBitsMask = 0; //todo repacker
             imageDescriptor.numInterleaveImages = chTbl[i].numInterleaved;
             imageDescriptor.interMode = chTbl[i].intreleavedMode;
             imageDescriptor.formatDiscriptor = chTbl[i].formatDiscriptor;
             if (chTbl[i].numLinesPerChunk > (1 << 16))
                 assert(0);
             ret = inu_graph__createImage(me, &data, channel, &imageDescriptor, chTbl[i].id, (UINT16)chTbl[i].numLinesPerChunk);
             if (ERRG_FAILED(ret))
                 return ret;

             if ((data == NULL) || (channel == NULL))
                 assert(0);
             //insert the channel and data, and bind them
             inu_graph__insertNode(me, channel);
             inu_graph__insertNode(me, data);
             inu_node__bindNodes(channel, data);
         }

         //bind the channel to it's source
         for (j = 0; j < chTbl[i].numInputs; j++)
         {
            //ignore inject and generator
            if (inputsH[chTbl[i].inputs[j].inst])
            {
               //bind channel to input
               inu_node__bindNodes(inputsH[chTbl[i].inputs[j].inst], channel);
            }
         }
      }
   }

   free(inputsH);
   free(sensors_groupsH);
   free(input_tbl);
   free(chTbl);

   return ret;
}

ERRG_codeE inu_graph__bindIspOutputToWriter(inu_graphH meH, inu_nucfgH nucfgH)
{
    //bind isp output channels to writers if it needed. according to xml feild in isp section
    ERRG_codeE ret = NUCFG__RET_SUCCESS;
    UINT32 val, i;
    XMLDB_pathE pathLink[6] = { ISPS_ISP0_OUTPUTS_MP_LINK_TO_WRITER_E ,
                             ISPS_ISP0_OUTPUTS_SP1_LINK_TO_WRITER_E ,
                             ISPS_ISP0_OUTPUTS_SP2_LINK_TO_WRITER_E,
                             ISPS_ISP1_OUTPUTS_MP_LINK_TO_WRITER_E,
                             ISPS_ISP1_OUTPUTS_SP1_LINK_TO_WRITER_E ,
                             ISPS_ISP1_OUTPUTS_SP2_LINK_TO_WRITER_E };

    for (i = 0; i < NUCFG_MAX_DB_ISP_META_CHANNELS; i++)
    {
        ret = XMLDB_getValue(NUCFG_getCurrDbH(nucfgH), pathLink[i], &val);
        if (ERRG_SUCCEEDED(ret))
        {
            inu_nodeH node1H;
            inu_nodeH node2H;
            char str1[20];
            char str2[20];

            sprintf(str1, "WRITER_%d", val);
            ret = inu_graph__findNode(meH, str1, &node1H);
            if (ERRG_SUCCEEDED(ret))
            {
                sprintf(str2, "IMAGE_%d", i + 23);
                ret = inu_graph__findNode(meH, str2, &node2H);
                if (ERRG_SUCCEEDED(ret))
                {
                    ret = inu_node__bindNodes(node2H, node1H);
                    if (ERRG_SUCCEEDED(ret))
                    {
                        printf("bind nodes succeeded between %s and %s\n", str1, str2);
                    }
                }
            }
        }
    }
    return ret;
}

void inu_graph__insertNode(inu_graphH meH, inu_nodeH nodeH)
{
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
#ifdef GRAPH_NODES_AS_LIST
   inu_node__link *newNode, *runner;

   newNode = (inu_node__link*)malloc(sizeof(inu_node__link));
   if (!newNode)
      assert(0);

   newNode->node = (inu_node*)nodeH;
   newNode->next = NULL;
   //add inputs at the end, to maintain the same order of creation
   //check if head is empty
   if (!(privP->nodesListHead))
   {
      privP->nodesListHead = newNode;
   }
   else
   {
      runner = privP->nodesListHead;
      //find last in list
      while(runner->next)
      {
         runner = runner->next;
      }
      runner->next = newNode;
   }
#else
   int i;
   for (i = 0; i < INU_GRAPH_MAX_NUM_NODES; i++)
   {
      if (privP->nodesList[i] == NULL)
      {
         privP->nodesList[i] = (inu_node*)nodeH; //todo add check if is node type
         inu_node__insertGraph(nodeH,meH);
         break;
      }
   }
#endif
}

void inu_graph__removeNode(inu_graphH meH, inu_nodeH nodeH)
{
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
#ifdef GRAPH_NODES_AS_LIST
   inu_node__link *removeNode = privP->nodesListHead;
   inu_node__link *temp;

   if (!removeNode)
      assert(0);

   if (removeNode->node == nodeH)
   {
      free(removeNode);
      privP->nodesListHead = NULL;
      return;
   }

   while(removeNode->next)
   {
      if(removeNode->next->node == nodeH)
      {
         temp = removeNode->next;
         removeNode->next = removeNode->next->next;
         free(temp);
         return;
      }
      removeNode = removeNode->next;
   }
#else
   int i;
   for (i = 0; i < INU_GRAPH_MAX_NUM_NODES; i++)
   {
      if (privP->nodesList[i] == nodeH)
      {
         privP->nodesList[i] = NULL;
         inu_node__removeGraph(nodeH);
         break;
      }
   }
#endif
}

ERRG_codeE inu_graph__walkGraphNodes(inu_graphH meH, INT32(*node_cb)(inu_nodeH nodeH, void **arg), void **argP)
{
   INT32 ret;
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
#ifdef GRAPH_NODES_AS_LIST
   inu_node__link *runner = privP->nodesListHead;
   while(runner)
   {
      ret = node_cb(runner->node, argP);
      if (ret == INU_WALKSOC_QUIT)
         return INU_GRAPH__RET_SUCCESS;
      runner = runner->next;
   }
#else
   int i;

   for (i = 0; i < INU_GRAPH_MAX_NUM_NODES; i++)
   {
      if (privP->nodesList[i])
      {
         ret = node_cb(privP->nodesList[i], argP);
         if (ret == INU_WALKSOC_QUIT)
            return INU_GRAPH__RET_SUCCESS;
      }
   }
#endif
   return INU_GRAPH__RET_SUCCESS;
}

ERRG_codeE inu_graph__findNode(inu_graphH meH, const char *userName, inu_refH *inuRefP)
{
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
#ifdef GRAPH_NODES_AS_LIST
   inu_node__link *runner = privP->nodesListHead;
   while(runner)
   {
       inu_ref *refP =  (inu_ref*)runner->node;
       const char *userNameTemp=inu_ref__getUserName(refP);
       if (strcmp(userName,userNameTemp)==0)
       {
          *inuRefP=runner->node;
          return INU_GRAPH__RET_SUCCESS;
       }
       runner = runner->next;
   }
#else
   int i;

   for (i = 0; i < INU_GRAPH_MAX_NUM_NODES; i++)
   {
      if (privP->nodesList[i])
      {
          inu_ref *refP =  (inu_ref*)privP->nodesList[i];
          const char *userNameTemp=inu_ref__getUserName(refP);
          if (strcmp(userName,userNameTemp)==0)
          {
             *inuRefP=privP->nodesList[i];
             return INU_GRAPH__RET_SUCCESS;
          }
      }
   }
#endif
   return INU_GRAPH__ERR_UNEXPECTED;
}

ERRG_codeE inu_graph__printNodes(inu_graphH meH, inu_refH *inuRefP)
{
   inu_graph__privData *privP = (inu_graph__privData*)((inu_graph*)meH)->privP;
   inu_node__link *runner = privP->nodesListHead;
   int i = 0;

   printf("List of nodes:\n");
   while (runner)
   {
      inu_ref *refP = (inu_ref*)runner->node;
      if (INU_STREAMER_REF_TYPE == inu_ref__getRefType(refP))
      {
         const char *userNameTemp = inu_ref__getUserName(refP);
         printf("\t[%02d]: %s\n", i++, userNameTemp);
      }
      runner = runner->next;
   }
   printf("\n");
   return INU_GRAPH__ERR_UNEXPECTED;
}


ERRG_codeE inu_graph__getOrigSensGroup(inu_nodeH srcNodeH, inu_nodeH *dstNodeH)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   inu_nodeH input = inu_node__getNextInputNode(srcNodeH, NULL);
   *dstNodeH = NULL;
//   printf("%s (%d) ", ((inu_node*)srcNodeH)->ref.p_vtable->p_name((inu_ref*)srcNodeH), ((inu_node*)srcNodeH)->ref.id);

   while (input && !(*dstNodeH))
   {
      ret = inu_graph__getOrigSensGroup(input, dstNodeH);
      input = inu_node__getNextInputNode(srcNodeH, input);
   }
   if (inu_ref__instanceOf(srcNodeH, INU_SENSORS_GROUP_REF_TYPE))
   {
      *dstNodeH = srcNodeH;
//      printf("\n");
   }

   if (!*dstNodeH)
   {
      ret = INU_GRAPH__ERR_UNEXPECTED;
   }
   return ret;
}

//find writer node by given channel node
ERRG_codeE inu_graph__getOrigWriters(inu_nodeH srcNodeH, inu_nodeH *dstNodeH)
{
   ERRG_codeE ret = INU_GRAPH__RET_SUCCESS;
   inu_nodeH input = inu_node__getNextInputNode(srcNodeH, NULL);
   //inu_nodeH output;
   *dstNodeH = NULL;
   //UINT32 cnt = 0;
   //printf("%s (%d) ", ((inu_node*)srcNodeH)->ref.p_vtable->p_name((inu_ref*)srcNodeH), ((inu_node*)srcNodeH)->ref.id);

   while (input && !(*dstNodeH))
   {
      ret = inu_graph__getOrigWriters(input, dstNodeH);
      input = inu_node__getNextInputNode(srcNodeH, input);
      if (inu_ref__instanceOf(srcNodeH, INU_SOC_WRITER_REF_TYPE))
      {
         *dstNodeH = srcNodeH;
         //printf("\n");
         return ret;
      }
   }


   if (!*dstNodeH)
   {
      ret = INU_GRAPH__ERR_UNEXPECTED;
   }
   return ret;
}


static void inu_graph__printBranch(inu_nodeH node, int stringLen)
{
   unsigned int i, j;
   inu_nodeH input = inu_node__getNextInputNode(node, NULL);

   j = printf("%s (%d)", ((inu_node*)node)->ref.p_vtable->p_name((inu_ref*)node), ((inu_node*)node)->ref.id);
   stringLen += j;
   if (input)
   {
      j = printf(INU_GRAPH__CONNECT_STRING);
      stringLen += j;
   }

   while (input)
   {
      inu_graph__printBranch(input, stringLen);
      input = inu_node__getNextInputNode(node, input);
      if (input)
      {
         printf("\n");
         for (i = 0; i < (stringLen - strlen(INU_GRAPH__NEW_LINE_STRING)); i++)
            printf(" ");
         printf(INU_GRAPH__NEW_LINE_STRING);
      }
   }
   return;
}

//Need to check if calbration is also required for users
ERRG_codeE inu_graph__offlineGetConfig(const char *socxml_path, inu_nucfgH *cfgH)
{
   NUFLD_init();
   return inu_graph__createXmlMod(socxml_path, NULL, NULL, 0, 0, "temp", cfgH);
}


void inu_graph__showGraph(inu_graphH meH)
{
   inu_graph__privData *privP  = (inu_graph__privData*)((inu_graph*)meH)->privP;
#ifdef GRAPH_NODES_AS_LIST
   inu_node__link      *runner = privP->nodesListHead;
   while(runner)
   {
      //find all nodes with no outputs
      if(!inu_node__getNextOutputNode(runner->node, NULL))
      {
         inu_graph__printBranch(runner->node, 0);
         printf("\n\n");
      }
      runner = runner->next;
   }
#else
   int i;

   //find all nodes with no outputs
   for (i = 0; i < INU_GRAPH_MAX_NUM_NODES; i++)
   {
      if ((privP->nodesList[i]) && (!inu_node__getNextOutputNode(privP->nodesList[i], NULL)))
      {
         inu_graph__printBranch(privP->nodesList[i], 0);
         printf("\n\n");
      }
   }
#endif
}


