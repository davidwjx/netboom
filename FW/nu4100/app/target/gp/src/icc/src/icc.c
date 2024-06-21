/****************************************************************************
 * 
 *   FileName: icc.c
 *
 *   Author: Arnon C. & Noam P
 *
 *   Date: 
 *
 *   Description: Inter core communication module file 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"

#if DEFSG_IS_GP
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "log.h"
#include "os_lyr.h"
#include "que.h"
#include "data_base.h"
#include "inu_stack.h"
#include "icc_common.h"
#include "icc.h"
#include "stdops.h"

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define ICCP_THREAD_WAKEUP_MSEC  (50)
#define ICCP_DATA_MSGQ_MAX       (10)
#define ICCP_CMD_DATA_MSGQ_NAME       "/IccCmdDataQue"
#define ICCP_MSG_DATA_MSGQ_NAME       "/IccMsgDataQue"
#define ICCP_CMD_QUEUE_MAX_SIZE  (20)
#define ICCP_MSG_THREAD_IDX      (0)
#define ICCP_CMD_THREAD_IDX      (1)

#define MAX_NUM_POLL_CYCLES (5)

#define WITH_POLLING
//#define ICC_DEBUG_PRINTS
// #define MOVE_ACK_INSIDE_ISR

typedef struct
{
   UINT32 active;
   UINT32 runThr;
}ICCP_threadParams;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static OS_LYRG_threadHandle           ICCP_cmdThrdH = NULL;
static OS_LYRG_threadHandle           ICCP_msgThrdH = NULL;
static OS_LYRG_msgQueT                ICCP_cmdMsgQue;
static OS_LYRG_msgQueT                ICCP_msgMsgQue;
static ICCP_cmdStateT                 ICCP_cmdState[ICCG_CMD_TARGET_MAX_NUM] = { ICCP_IDLE, ICCP_IDLE };
static QUEG_queT                      ICCP_cmdQueue[ICCG_CMD_TARGET_MAX_NUM];
static STACKG_stackT                  ICCP_jobDescriptorsStack;
static OS_LYRG_mutexT                 ICCP_jobDescriptorStackMutex;
static const UINT32                   ICCP_mapTblGpToCevaBoxDbName[ICCG_CMD_TARGET_MAX_NUM] = {DATA_BASEG_ICC_BOX_GP_DSPA,DATA_BASEG_ICC_BOX_GP_DSPB};
static const UINT32                   ICCP_mapTblCevaToGpBoxDbName[ICCG_CMD_TARGET_MAX_NUM] = {DATA_BASEG_ICC_BOX_DSPA_GP,DATA_BASEG_ICC_BOX_DSPB_GP};
static UINT32                         ICCP_virtAddrGpIctlForce;
static UINT32                         ICCP_virtAddrDspIctlForce[ICCG_CMD_TARGET_MAX_NUM];
static UINT32                         ICCP_virtAddrGpIctlStatus;
static UINT32                         ICCP_virtAddrDspIctlStatus[ICCG_CMD_TARGET_MAX_NUM];
static OS_LYRG_mutexCpT              *ICCP_mutexP[3];
static ICCP_threadParams              ICCP_thrParams[2];


/****************************************************************************
***************    L O C A L         F U N C T I O N S        ***************
****************************************************************************/
static void ICCP_invokeCb(UINT32 descriptorHandle, ERRG_codeE retVal, ICC_COMMONG_dspTargetT dspTarget)
{
   DATA_BASEG_iccJobsDescriptorDataBaseT *jobsDbP;

   DATA_BASEG_accessDataBaseNoMutex((UINT8**)(&jobsDbP), DATA_BASEG_JOB_DESCRIPTORS);
   if ( ((descriptorHandle & ICC_COMMONG_iccCmdMsgDescMask) < ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE) &&
        (jobsDbP->jobDescriptorPool[descriptorHandle & ICC_COMMONG_iccCmdMsgDescMask].cb != NULL    ))
   {
      jobsDbP->jobDescriptorPool[descriptorHandle & ICC_COMMONG_iccCmdMsgDescMask].cb(descriptorHandle, retVal, dspTarget);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid jobDescriptor! 0x%x\n",descriptorHandle);   
   }
}

static ERRG_codeE ICCP_poll( ICC_COMMONG_dspTargetT dspTarget )
{
   UINT32               regVal;
   INT32                pollingCounter = -1;   
   ERRG_codeE           ret = RET_SUCCESS;   

   regVal = *(volatile UINT32 *)ICCP_virtAddrDspIctlStatus[dspTarget];

   while(((regVal & 0x2) == 0x2) && (pollingCounter < MAX_NUM_POLL_CYCLES))
   {
      //wait for CEVA to finish handling previous ISR if there is
      pollingCounter++;
      OS_LYRG_usleep(1);
      regVal = *(volatile UINT32 *)ICCP_virtAddrDspIctlStatus[dspTarget];
   } 

   if (pollingCounter == MAX_NUM_POLL_CYCLES)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"polling timeout! dsp %d, regVal = %x\n",dspTarget,regVal);
      ret = ICC__ERR_POLLING_TIMEOUT_FAIL;
   }

   return ret;
}

static ERRG_codeE ICCP_prepareIccBox( ICC_COMMONG_dspTargetT dspTarget , UINT32 descriptorHandle )
{
   DATA_BASEG_iccBoxDataBaseT  *iccBoxGpToCevaP;
   ERRG_codeE                  ret = RET_SUCCESS;   

#ifdef WITH_POLLING   
   ret = ICCP_poll(dspTarget);
#endif

   if(ERRG_SUCCEEDED(ret))
   {
      //wait a bit for CEVA to exit irq mode
      OS_LYRG_usleep(1);

      DATA_BASEG_accessDataBase((UINT8**)(&iccBoxGpToCevaP), ICCP_mapTblGpToCevaBoxDbName[dspTarget]);
      iccBoxGpToCevaP->box = (ICC_COMMONG_iccCmdReqAckMask | descriptorHandle); 
      DATA_BASEG_accessDataBaseRelease(ICCP_mapTblGpToCevaBoxDbName[dspTarget]);
   }
   else
   {
      ICCP_invokeCb(descriptorHandle,ret,dspTarget);
   }

   return ret;
}


static void ICCP_kickDsp( ICC_COMMONG_dspTargetT dspTarget )
{
   UINT32 regVal;

   OS_LYRG_lockMutexCp(ICCP_mutexP[1+dspTarget]);
   regVal = *(volatile UINT32 *)ICCP_virtAddrDspIctlForce[dspTarget];
   regVal |= 0x2;
   //Trigger DSP interrupt
   *(volatile UINT32 *)ICCP_virtAddrDspIctlForce[dspTarget] = regVal;
   OS_LYRG_unlockMutexCp(ICCP_mutexP[1+dspTarget]);
}


int ICCP_handleIccBox( ICC_COMMONG_dspTargetT dspTarget )
{
   DATA_BASEG_iccBoxDataBaseT  iccBoxDspToGp;

   //prepare message for database     
   DATA_BASEG_readDataBase((UINT8*)(&iccBoxDspToGp), ICCP_mapTblCevaToGpBoxDbName[dspTarget], 0);

   if (iccBoxDspToGp.box & ICC_COMMONG_iccCmdReqAckMask)
   {
      /* Recevied MSG from CEVA */
      ICCG_msgT msg;

      msg.msgType          = ICCG_MSG_REQ;
      msg.dspSource        = dspTarget;
      msg.descriptorHandle = iccBoxDspToGp.box & ICC_COMMONG_iccCmdMsgDescMask;

      ICCG_sendIccMsg(&msg);
   
      return msg.descriptorHandle;
   }
   else
   {
      /* Recevied ACK from CEVA for CMD */
      ICCG_cmdT cmd;   

      cmd.cmdType          = ICCG_CMD_ACK;
      cmd.dspTarget        = dspTarget;
      cmd.descriptorHandle = iccBoxDspToGp.box;
      ICCG_sendIccCmd(&cmd);

      return -1;
   }   
}


void ICCP_dspaIsr(UINT64 timestamp, UINT64 count, void *argP)
{
   UINT32 regVal;
#ifdef MOVE_ACK_INSIDE_ISR
   INT32 ret;
#endif

   FIX_UNUSED_PARAM_WARN(timestamp);
   FIX_UNUSED_PARAM_WARN(count);
   FIX_UNUSED_PARAM_WARN(argP);

#ifdef MOVE_ACK_INSIDE_ISR
   ret = ICCP_handleIccBox(ICCG_CMD_TARGET_DSPA);
#else
   ICCP_handleIccBox(ICCG_CMD_TARGET_DSPA);
#endif

   //Clear force interrupt bit
   OS_LYRG_lockMutexCp(ICCP_mutexP[0]);
   regVal = *(UINT32 *)ICCP_virtAddrGpIctlForce;
   regVal &= ~((UINT32)(1 << ICC_COMMONG_ICTL0_DSPA_INT_NUM));
   *(UINT32 *)ICCP_virtAddrGpIctlForce = regVal;
   OS_LYRG_unlockMutexCp(ICCP_mutexP[0]);  

#ifdef MOVE_ACK_INSIDE_ISR
   if (ret != -1)
   {
      DATA_BASEG_iccBoxDataBaseT    iccBoxGpToCeva;
      ICCG_msgT msg;

      msg.descriptorHandle = ret;
      msg.dspSource = ICCG_CMD_TARGET_DSPA;
      
      //ACK the ceva
      iccBoxGpToCeva.box = msg.descriptorHandle & (~ICC_COMMONG_iccCmdReqAckMask);
      DATA_BASEG_writeDataBase((UINT8*)(&iccBoxGpToCeva), ICCP_mapTblGpToCevaBoxDbName[msg.dspSource], 0);
   
#ifdef WITH_POLLING   
      ret = ICCP_poll(msg.dspSource);
#endif  

      //TODO: how to overcome an issue which CEVA will not receive our msg ACK...
      ICCP_kickDsp(msg.dspSource);
   }
#endif

}

void ICCP_dspbIsr(UINT64 timestamp, UINT64 count, void *argP)
{
   UINT32 regVal;
#ifdef MOVE_ACK_INSIDE_ISR
   INT32 ret;
#endif
   FIX_UNUSED_PARAM_WARN(timestamp);
   FIX_UNUSED_PARAM_WARN(count);
   FIX_UNUSED_PARAM_WARN(argP);

#ifdef MOVE_ACK_INSIDE_ISR
   ret = ICCP_handleIccBox(ICCG_CMD_TARGET_DSPB);
#else
   ICCP_handleIccBox(ICCG_CMD_TARGET_DSPB);
#endif

   //Clear force interrupt bit
   OS_LYRG_lockMutexCp(ICCP_mutexP[0]);
   regVal = *(UINT32 *)ICCP_virtAddrGpIctlForce;
   regVal &= ~((UINT32)(1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM));
   *(UINT32 *)ICCP_virtAddrGpIctlForce = regVal;
   OS_LYRG_unlockMutexCp(ICCP_mutexP[0]); 

#ifdef MOVE_ACK_INSIDE_ISR
   if (ret != -1)
   {
      DATA_BASEG_iccBoxDataBaseT    iccBoxGpToEv;
      ICCG_msgT msg;

      msg.descriptorHandle = ret;
      msg.dspSource = ICCG_CMD_TARGET_DSPB;
      
      //ACK the ceva
      iccBoxGpToEv.box = msg.descriptorHandle & (~ICC_COMMONG_iccCmdReqAckMask);
      DATA_BASEG_writeDataBase((UINT8*)(&iccBoxGpToEv), ICCP_mapTblGpToCevaBoxDbName[msg.dspSource], 0);

#ifdef WITH_POLLING   
      ret = ICCP_poll(msg.dspSource);
#endif  

      //TODO: how to overcome an issue which CEVA will not receive our msg ACK...
      ICCP_kickDsp(msg.dspSource);
   }
#endif
}

static void ICCP_iccHandleCmd( ICCG_cmdT *cmdP )
{
   ERRG_codeE                  ret = RET_SUCCESS;

#ifdef ICC_DEBUG_PRINTS
   LOGG_PRINT(LOG_INFO_E, NULL, "Cmd: dsp = %d, state = %d, type = %d\n",cmdP->dspTarget,ICCP_cmdState[cmdP->dspTarget],cmdP->cmdType);
#endif
   switch (ICCP_cmdState[cmdP->dspTarget])
   {
      case(ICCP_IDLE):
      {
         if (cmdP->cmdType == ICCG_CMD_REQ)
         {
            ret = ICCP_prepareIccBox( cmdP->dspTarget, cmdP->descriptorHandle );

            if(ERRG_SUCCEEDED(ret))
            {
               ICCP_kickDsp( cmdP->dspTarget );
               ICCP_cmdState[cmdP->dspTarget] = ICCP_WAIT_FOR_ACK;            
            }
         }
         else
         {
            ICCP_invokeCb(cmdP->descriptorHandle, ICC__ERR_CMD_ACK_WRONG_STATE_FAIL,cmdP->dspTarget);
            LOGG_PRINT(LOG_ERROR_E, NULL, "Cmd received with ACK while in wrong state!\n");
         }
      }break;

      case(ICCP_WAIT_FOR_ACK):
      {
         if (cmdP->cmdType == ICCG_CMD_REQ)
         {
            UINT32 queueElm;

            queueElm = (cmdP->dspTarget << 16 )| cmdP->descriptorHandle; 
            //Insert cmd request to queue
            if (QUEG_enque(&ICCP_cmdQueue[cmdP->dspTarget], (QUEG_elemT)queueElm) < 0)
            {
               LOGG_PRINT(LOG_WARN_E, NULL, "Cmd queue is full!\n");
               ICCP_invokeCb(cmdP->descriptorHandle, ICC__ERR_CMD_Q_FULL_FAIL,cmdP->dspTarget);
            }
            else
            {
#ifdef ICC_DEBUG_PRINTS            
               LOGG_PRINT(LOG_INFO_E, NULL, "queued cmd success. dsp =%d, j = %d\n",cmdP->dspTarget,cmdP->descriptorHandle);
#endif               
            }
         }
         else
         {
            QUEG_elemT elem;
            //Consider sanity check on the ACK
            //Pop next cmd from queue and if queue is empty move to idle state.
            if (QUEG_deque(&ICCP_cmdQueue[cmdP->dspTarget], &elem) < 0)
            {
               ICCP_cmdState[cmdP->dspTarget] = ICCP_IDLE;
#ifdef ICC_DEBUG_PRINTS
               LOGG_PRINT(LOG_INFO_E, NULL, "Cmd queue is empty, return to state ICCP_IDLE\n");
#endif               
            }
            else
            {
               cmdP->cmdType = ICCG_CMD_REQ;
               cmdP->descriptorHandle = (UINT32)elem & 0xFFFF;
               cmdP->dspTarget = (((UINT32)elem) >> 16) & 0xFFFF;

#ifdef ICC_DEBUG_PRINTS
               LOGG_PRINT(LOG_INFO_E, NULL, "poped elem = 0x%x from queue, dsp = %d, j = %d\n",elem,cmdP->dspTarget,cmdP->descriptorHandle );
#endif               
               ret = ICCP_prepareIccBox( cmdP->dspTarget, cmdP->descriptorHandle );

               if(ERRG_SUCCEEDED(ret))
               {
                  ICCP_kickDsp( cmdP->dspTarget );               
               }
            }
         }
      }break;

      default:
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "iligal ICC Cmd state!\n");
      }break;
   }
}   



static void ICCP_iccHandleMsg( ICCG_msgT *msgP )
{
   ERRG_codeE                    ret = RET_SUCCESS;  
      
#ifndef MOVE_ACK_INSIDE_ISR
   DATA_BASEG_iccBoxDataBaseT    iccBoxGpToCeva;


   //ACK the ceva
   iccBoxGpToCeva.box = msgP->descriptorHandle & (~ICC_COMMONG_iccCmdReqAckMask);
   DATA_BASEG_writeDataBase((UINT8*)(&iccBoxGpToCeva), ICCP_mapTblGpToCevaBoxDbName[msgP->dspSource], 0);

#ifdef WITH_POLLING   
   ret = ICCP_poll(msgP->dspSource);
#endif  

   //TODO: how to overcome an issue which CEVA will not receive our msg ACK...
   ICCP_kickDsp(msgP->dspSource);
#endif
   ICCP_invokeCb(msgP->descriptorHandle, ret, msgP->dspSource);
}   


/****************************************************************************
*
*  Function Name: ICCP_iccCmdThread
*
*  Description: Inter core communication thread. The thread is interrupting the CEVA
*                with a message (triggered by any service), and then waits
*                for an ACK from CEVA (to enable next CMD interrupt)
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
static int ICCP_iccCmdThread(void *argP)
{
   ICCG_cmdT cmd;
   INT32  status;
   UINT32 cmdSize = 0;
   ICCP_threadParams *thrdParamsP = (ICCP_threadParams*)argP;

   thrdParamsP->active = 1;
   while(thrdParamsP->runThr)
   {
      cmdSize = sizeof(ICCG_cmdT);
      status = OS_LYRG_recvMsg(&ICCP_cmdMsgQue,  (UINT8*)&cmd, &cmdSize, ICCP_THREAD_WAKEUP_MSEC);      

      if((status == SUCCESS_E) && (cmdSize > 0))
      {
         ICCP_iccHandleCmd(&cmd);
      }
   }
   thrdParamsP->active = 0;

   return ICC__RET_SUCCESS;
}



static int ICCP_iccMsgThread(void *argP)
{
   ICCG_msgT msg;
   INT32  status;
   UINT32 msgSize = 0;
   ICCP_threadParams *thrdParamsP = (ICCP_threadParams*)argP;

   thrdParamsP->active = 1;
   while(thrdParamsP->runThr)
   {
      msgSize = sizeof(ICCG_msgT);
      status = OS_LYRG_recvMsg(&ICCP_msgMsgQue,  (UINT8*)&msg, &msgSize, ICCP_THREAD_WAKEUP_MSEC);
      
      if((status == SUCCESS_E) && (msgSize > 0))
      {
         ICCP_iccHandleMsg(&msg);
      }      
   }
   thrdParamsP->active = 0;

   return ICC__RET_SUCCESS;   
}

/****************************************************************************
*
*  Function Name: ICCP_iccCmdInit
*
*  Description: Function will init the Inter Core communication thread module:
*                    1. 
*                    2.
*                    3. 
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
static ERRG_codeE ICCP_iccCmdInit( void )
{
   ERRG_codeE               ret = ICC__RET_SUCCESS;
   OS_LYRG_threadParams     thrParams;
   ICC_COMMONG_dspTargetT   dspTarget;

   memcpy(ICCP_cmdMsgQue.name, ICCP_CMD_DATA_MSGQ_NAME, MIN(sizeof(ICCP_cmdMsgQue.name), strlen(ICCP_CMD_DATA_MSGQ_NAME)+1));
   ICCP_cmdMsgQue.maxMsgs = ICCP_DATA_MSGQ_MAX;
   ICCP_cmdMsgQue.msgSize = sizeof(ICCG_cmdT);

   //Create message queue
   if(OS_LYRG_createMsgQue(&ICCP_cmdMsgQue, 0) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "icc msg queue create failed\n");
      ret = ICC__ERR_CREATE_MSG_Q_FAIL;
   }

   //Create local queues
   for (dspTarget = ICCG_CMD_TARGET_DSPA ; dspTarget < ICCG_CMD_TARGET_MAX_NUM ; dspTarget++)
   {
      ICCP_cmdQueue[dspTarget].maxCnt = ICCP_CMD_QUEUE_MAX_SIZE;
      QUEG_init(&ICCP_cmdQueue[dspTarget]);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //Create thread
      thrParams.func = ICCP_iccCmdThread;
      thrParams.id = OS_LYRG_ICC_CMD_THREAD_ID_E;
      thrParams.event = NULL;
      thrParams.param = &ICCP_thrParams[ICCP_CMD_THREAD_IDX];
      ICCP_thrParams[ICCP_CMD_THREAD_IDX].runThr = 1;
      ICCP_cmdThrdH = OS_LYRG_createThread(&thrParams);
      if(!ICCP_cmdThrdH)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Error: fail to create thread\n");
         ret = ICC__ERR_UNEXPECTED;
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: ICCP_iccMsgInit
*
*  Description: Function will init the Inter Core communication thread module:
*                    1. 
*                    2. 
*                    3. 
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
static ERRG_codeE ICCP_iccMsgInit( void )
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;
   OS_LYRG_threadParams thrParams;
 
   memcpy(ICCP_msgMsgQue.name, ICCP_MSG_DATA_MSGQ_NAME, MIN(sizeof(ICCP_msgMsgQue.name), strlen(ICCP_MSG_DATA_MSGQ_NAME)+1));
   ICCP_msgMsgQue.maxMsgs = ICCP_DATA_MSGQ_MAX;
   ICCP_msgMsgQue.msgSize = sizeof(ICCG_cmdT);
   
   //Create queue
   if(OS_LYRG_createMsgQue(&ICCP_msgMsgQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E) != SUCCESS_E)
   {
      return ICC__ERR_CREATE_MSG_Q_FAIL;
   }
   //Create thread
   thrParams.func = ICCP_iccMsgThread;
   thrParams.id = OS_LYRG_ICC_MSG_THREAD_ID_E;
   thrParams.event = NULL;
   thrParams.param = &ICCP_thrParams[ICCP_MSG_THREAD_IDX];
   ICCP_thrParams[ICCP_MSG_THREAD_IDX].runThr = 1;
   ICCP_msgThrdH = OS_LYRG_createThread(&thrParams);
   if(!ICCP_msgThrdH)
   {
      ret = ICC__ERR_UNEXPECTED;
   }

   return ret;
}


static ERRG_codeE ICCP_iccJobsManagerInit( void )
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;   
   int i;

   OS_LYRG_aquireMutex(&ICCP_jobDescriptorStackMutex);

   STACKG_init(&ICCP_jobDescriptorsStack,ICCP_JOB_DESCRIPTOR_STACK_MAXSIZE);

   for ( i = 0; i < ICCP_JOB_DESCRIPTOR_STACK_MAXSIZE; i++ )
   {
      STACKG_push(&ICCP_jobDescriptorsStack,(STACKG_elemT)i);
   }

   return ret;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE ICCG_getJobDescriptor( UINT16 *index )
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;
   STACKG_elemT         temp;

   OS_LYRG_lockMutex(&ICCP_jobDescriptorStackMutex);
   ret = STACKG_pop(&ICCP_jobDescriptorsStack,&temp);
   OS_LYRG_unlockMutex(&ICCP_jobDescriptorStackMutex);

   *index = (UINT16)((UINT16)temp & 0xFFFF);

   return(ret);   
}

ERRG_codeE ICCG_returnJobDescriptor( UINT32 index )
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;

   OS_LYRG_lockMutex(&ICCP_jobDescriptorStackMutex);
   ret = STACKG_push(&ICCP_jobDescriptorsStack,(STACKG_elemT)index);
   OS_LYRG_unlockMutex(&ICCP_jobDescriptorStackMutex);

   return ret;
}

ERRG_codeE ICCG_sendIccCmd(ICCG_cmdT *cmdP)
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;

   if(OS_LYRG_sendMsg(&ICCP_cmdMsgQue,(UINT8 *)cmdP, sizeof(ICCG_cmdT)) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "cmdP->cmdType = %d, cmdP->descriptorHandle = %d, cmdP->dspTarget = %d\n",cmdP->cmdType, cmdP->descriptorHandle, cmdP->dspTarget);   
      ret = ICC__ERR_OUT_OF_RSRCS;
   }

   return ret;
}

ERRG_codeE ICCG_sendIccMsg(ICCG_msgT *msgP)
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;

   if(OS_LYRG_sendMsg(&ICCP_msgMsgQue,(UINT8 *)msgP, sizeof(ICCG_msgT)) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "msgP->msgType = %d, msgP->descriptorHandle = %d, msgP->dspSource = %d\n",msgP->msgType, msgP->descriptorHandle, msgP->dspSource);      
      ret = ICC__ERR_OUT_OF_RSRCS;
   }

   return ret;
}

ERRG_codeE ICCG_iccInit( void )
{
   ERRG_codeE           ret = ICC__RET_SUCCESS;
   int                  mutexRet,i;

   UINT32 gpIctlForcePhyAddress    = 0x08170010;
   UINT32 dspaIctlForcePhyAddress  = 0x08120010;
   UINT32 dspbIctlForcePhyAddress  = 0x08130010;

   UINT32 gpIctlStatusPhyAddress   = 0x08170020;
   UINT32 dspaIctlStatusPhyAddress = 0x08120020;
   UINT32 dspbIctlStatusPhyAddress = 0x08130020;
  
   MEM_MAPG_convertPhysicalToVirtual3(gpIctlForcePhyAddress,   (&ICCP_virtAddrGpIctlForce));
   MEM_MAPG_convertPhysicalToVirtual3(dspaIctlForcePhyAddress, (&ICCP_virtAddrDspIctlForce[ICCG_CMD_TARGET_DSPA]));   
   MEM_MAPG_convertPhysicalToVirtual3(dspbIctlForcePhyAddress, (&ICCP_virtAddrDspIctlForce[ICCG_CMD_TARGET_DSPB]));

   MEM_MAPG_convertPhysicalToVirtual3(gpIctlStatusPhyAddress,   (&ICCP_virtAddrGpIctlStatus));
   MEM_MAPG_convertPhysicalToVirtual3(dspaIctlStatusPhyAddress, (&ICCP_virtAddrDspIctlStatus[ICCG_CMD_TARGET_DSPA]));   
   MEM_MAPG_convertPhysicalToVirtual3(dspbIctlStatusPhyAddress, (&ICCP_virtAddrDspIctlStatus[ICCG_CMD_TARGET_DSPB]));

   if(ERRG_SUCCEEDED(ret))
   {   
      ret = ICCP_iccCmdInit();
   }
   
   if(ERRG_SUCCEEDED(ret))
   {   
      ret = ICCP_iccMsgInit();
   }

   if(ERRG_SUCCEEDED(ret))
   {   
      ret = ICCP_iccJobsManagerInit();
   }  

   //Register isr and enable DSP interrupts
   OS_LYRG_intCtrlRegister(OS_LYRG_INT_DSPA_E, ICCP_dspaIsr, NULL);
   OS_LYRG_intCtrlEnable(OS_LYRG_INT_DSPA_E);
   OS_LYRG_intCtrlRegister(OS_LYRG_INT_DSPB_E, ICCP_dspbIsr, NULL);
   OS_LYRG_intCtrlEnable(OS_LYRG_INT_DSPB_E);

   //init ictl mutex's
   for (i = 0; i < 3; i++)
   {
      mutexRet = OS_LYRG_aquireMutexCp(&ICCP_mutexP[i], OS_LYRG_CP_MUTEX_ICC_ICTL_GP_E + i);  

      if ( mutexRet == FAIL_E )
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,  "OS_LYRG_aquireMutexCp for ICC %d failed !!!\n", OS_LYRG_CP_MUTEX_ICC_ICTL_GP_E + i);
      }
   }

   return ret;
}

ERRG_codeE ICCG_iccClose(void)
{
   ERRG_codeE  ret = ICC__RET_SUCCESS;
   INT32       ststus, i, saftyCntr;

   //exit threads
   for (i = 0; i < 2; i++)
   {
      saftyCntr = 0;
      ICCP_thrParams[i].runThr = 0;
      while((ICCP_thrParams[i].active) && (saftyCntr < 100))
      {
         OS_LYRG_usleep(100000);
         saftyCntr++;
      }
      if (saftyCntr == 10)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,  "thread %d fails on exit\n",i);
         ret = ICC__ERR_UNEXPECTED;
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ststus = OS_LYRG_deleteMsgQue(&ICCP_msgMsgQue);
      if(ststus == FAIL_E)
      {
         ret = ICC__ERR_DELETE_MSG_Q_FAIL;
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ststus = OS_LYRG_deleteMsgQue(&ICCP_cmdMsgQue);
      if(ststus == FAIL_E)
      {
         ret = ICC__ERR_DELETE_MSG_Q_FAIL;
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlUnregister(OS_LYRG_INT_DSPA_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlDisable(OS_LYRG_INT_DSPA_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlUnregister(OS_LYRG_INT_DSPB_E);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlDisable(OS_LYRG_INT_DSPB_E);
   }
   return(ret);
}

#ifdef ICC_TEST
#include <time.h>
#include <stdlib.h>

#define MAX_RTT_TIME_PERIOD (10000)

typedef struct
{
   UINT64 start_us[ICCP_JOB_DESCRIPTOR_STACK_MAXSIZE];
   UINT64 sum_us;
   UINT64 min_us;
   UINT64 max_us;
   UINT64 rtt_time_us[MAX_RTT_TIME_PERIOD];
   int    sample;
}ICCP_testStats;

volatile UINT32 dataCheck;
UINT32 enablePrints = 1;
ICCP_testStats stats;

void ICCP_testUpdateStart(ICCP_testStats *statsP, int descriptor)
{
   OS_LYRG_getUsecTime(&statsP->start_us[descriptor]);   
}

void ICCP_testUpdateEnd(ICCP_testStats *statsP, int descriptor)
{
   UINT64 end;
   int temp;
   OS_LYRG_getUsecTime(&end);
   temp = statsP->sample % MAX_RTT_TIME_PERIOD;
   statsP->rtt_time_us[temp] = end - statsP->start_us[descriptor];
   statsP->sum_us = statsP->sum_us + statsP->rtt_time_us[temp];
   statsP->min_us = (statsP->rtt_time_us[temp] < statsP->min_us) ? statsP->rtt_time_us[temp] : statsP->min_us;
   statsP->max_us = (statsP->rtt_time_us[temp] > statsP->max_us) ? statsP->rtt_time_us[temp] : statsP->max_us;
   statsP->sample++;
}

void ICCP_testCalcStats(ICCP_testStats *statsP)
{
   UINT64 average_us;
   average_us = statsP->sum_us / statsP->sample;
   LOGG_PRINT(LOG_INFO_E, NULL, "*********** average RTT[us] = %llu, min RTT[us] = %llu, max RTT[us] = %llu **\n",average_us,statsP->min_us,statsP->max_us);
}

void ICCP_testInitStats(ICCP_testStats *statsP)
{
   memset(statsP,0,sizeof(ICCP_testStats));
   statsP->min_us = 0xFFFFFFFF;
}

void ICCP_testMsgCb( UINT16 jobDescriptorIndex , ERRG_codeE result, ICC_COMMONG_dspTargetT dspTarget )
{
   DATA_BASEG_iccJobsDescriptorDataBaseT  jobsDb;

   ICCP_testUpdateEnd(&stats,jobDescriptorIndex);

   DATA_BASEG_readDataBase((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS, 0);
   if (enablePrints)
      LOGG_PRINT(LOG_INFO_E, NULL, "Received ICC MSG: jobDescriptorIndex = %d, data = %d, result = 0x%x, dspTarget = %d\n",jobDescriptorIndex, jobsDb.jobDescriptorPool[jobDescriptorIndex].data,result,dspTarget);

   if (ERRG_SUCCEEDED(result)) 
   {
      dataCheck++;
   }
   
   ICCG_returnJobDescriptor(jobDescriptorIndex);
}

void ICCP_testSendCmds( UINT16 dspTarget , UINT32 data , DATA_BASEG_targetAlgJobE alg )
{
   ICCG_cmdT                              cmd;
   DATA_BASEG_iccJobsDescriptorDataBaseT  jobsDb;
   UINT16                                 jobDescriptorIndex;
   ERRG_codeE                             ret = ICC__RET_SUCCESS;
   volatile UINT32                        *ictlDsp; 

   ret = ICCG_getJobDescriptor(&jobDescriptorIndex);

   if (ERRG_SUCCEEDED(ret))
   {
      DATA_BASEG_readDataBase((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS, 0);                 
      jobsDb.jobDescriptorPool[jobDescriptorIndex].alg = alg;
      jobsDb.jobDescriptorPool[jobDescriptorIndex].cb  = ICCP_testMsgCb;
      jobsDb.jobDescriptorPool[jobDescriptorIndex].data = data;
      DATA_BASEG_writeDataBase((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS, 0);
      
      cmd.cmdType  = ICCG_CMD_REQ;
      cmd.descriptorHandle = jobDescriptorIndex;
      cmd.dspTarget = dspTarget;      

      ictlDsp = (UINT32*)ICCP_virtAddrDspIctlStatus[dspTarget];

      if (enablePrints)
         LOGG_PRINT(LOG_INFO_E, NULL, "Send CMD to DSP %d : j = %d, d = %d, c_s = %d, d_i = %x\n",dspTarget, jobDescriptorIndex,data,ICCP_cmdState[dspTarget],*ictlDsp);

      ICCP_testUpdateStart(&stats,jobDescriptorIndex);

      ICCG_sendIccCmd(&cmd);
   }
   else
   {
      if (enablePrints)
         LOGG_PRINT(LOG_INFO_E, NULL, "Error sending CMD to DSP %d : data = %d (stack is empty)\n",dspTarget, data);
   }
  
}

void ICCP_testShowResults( char *testName, int numOfIccCmds, UINT32 secStart, UINT16 msecStart, UINT32 secEnd, UINT16 msecEnd )
{
   UINT32 sec ,msec, totalMsec;

   sec = secEnd-secStart;
   msec = msecEnd-msecStart;
   totalMsec = 1000*sec + msec;

   OS_LYRG_usleep(1000000);
   LOGG_PRINT(LOG_INFO_E, NULL, "*****************************************************************************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "*********** run %s test results: ********************************************\n",testName);
   LOGG_PRINT(LOG_INFO_E, NULL, "*********** dataCheck = %d, Number of cmds = %d, lost precent = %f **********\n",dataCheck, numOfIccCmds, 100 * ((float)numOfIccCmds - (float)dataCheck) / (float)numOfIccCmds ); 
   LOGG_PRINT(LOG_INFO_E, NULL, "*********** duration: %d.02%d, ICC per second : optimal = %f, actual = %f ***\n",sec ,msec,1000/((float)totalMsec)/((float)numOfIccCmds),1000/((float)totalMsec)/((float)dataCheck));
   ICCP_testCalcStats(&stats);
   LOGG_PRINT(LOG_INFO_E, NULL, "*********** Jobs descriptor top = %d ****************************************\n",ICCP_jobDescriptorsStack.top );
   STACKG_show(&ICCP_jobDescriptorsStack);
   LOGG_PRINT(LOG_INFO_E, NULL, "*****************************************************************************\n");
   OS_LYRG_usleep(5000000);   
}

void ICCG_test( void )
{
   UINT32           dspTarget, iteration;
   UINT32           secStart,  secEnd;
   UINT16           msecStart, msecEnd;
   
   LOGG_PRINT(LOG_INFO_E, NULL, "ICC test program start.\n");
   OS_LYRG_usleep(500000);
   enablePrints = 0;

   // 1. Run simple single test - send ICC's with long delay to each ceva
   for (dspTarget = 0; dspTarget < ICCG_CMD_TARGET_MAX_NUM; dspTarget++)   
   {
      ICCP_testInitStats(&stats);
      LOGG_PRINT(LOG_INFO_E, NULL, "*******************************************************\n");
      LOGG_PRINT(LOG_INFO_E, NULL, "***************** run simple test DSP %d *********************\n",dspTarget);
      LOGG_PRINT(LOG_INFO_E, NULL, "*******************************************************\n");
      dataCheck = 0;

      OS_LYRG_getTime(&secStart, &msecStart);
      
      for (iteration = 0; iteration < 100; iteration++)
      {
         ICCP_testSendCmds( dspTarget, iteration, DATA_BASEG_ALG_TEST_SHORT );
         OS_LYRG_usleep(30000);
      }

      OS_LYRG_getTime(&secEnd, &msecEnd);
      
      ICCP_testShowResults("simple single", iteration, secStart, msecStart, secEnd, msecEnd);
   }

   // 2. Run stress single test - send ICC's with short delay 
   enablePrints = 0;
   for (dspTarget = 0; dspTarget < ICCG_CMD_TARGET_MAX_NUM; dspTarget++)   
   {
      ICCP_testInitStats(&stats);
      LOGG_PRINT(LOG_INFO_E, NULL, "****************************************************\n");   
      LOGG_PRINT(LOG_INFO_E, NULL, "***************** run stress test DSP %d ******************\n",dspTarget);
      LOGG_PRINT(LOG_INFO_E, NULL, "****************************************************\n");
      dataCheck = 0;
      
      OS_LYRG_getTime(&secStart, &msecStart);      
      
      for (iteration = 0; iteration < 1000; iteration++)
      {
         ICCP_testSendCmds( dspTarget, iteration, DATA_BASEG_ALG_TEST_SHORT);
         OS_LYRG_usleep(1000);
      }
      
      OS_LYRG_getTime(&secEnd, &msecEnd);

      ICCP_testShowResults("single stress", iteration, secStart, msecStart, secEnd, msecEnd);           
   }




   //3. Run ceva scheduler test - send ICC's with vary delay and activate job with delay
   LOGG_PRINT(LOG_INFO_E, NULL, "*********************************************************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "***************** run ceva scheduler test ***************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "*********************************************************\n");
   OS_LYRG_usleep(1000000); 
   dataCheck = 0;
   ICCP_testInitStats(&stats);

   OS_LYRG_getTime(&secStart, &msecStart);      
   
   for (iteration = 0; iteration < 100; iteration++ )
   {
      ICCP_testSendCmds( ICCG_CMD_TARGET_DSPA, iteration, DATA_BASEG_ALG_TEST_LONG );
      OS_LYRG_usleep(10000);
   }      

   OS_LYRG_getTime(&secEnd, &msecEnd);

   ICCP_testShowResults("ceva scheduler", iteration , secStart, msecStart, secEnd, msecEnd);   


   //4. Run test with random delays
   LOGG_PRINT(LOG_INFO_E, NULL, "*****************************************************************************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "***************** run random delays *****************************************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "*****************************************************************************\n");
   OS_LYRG_usleep(1000000); 
   dataCheck = 0;
   ICCP_testInitStats(&stats);  

   srand(time(NULL));

   OS_LYRG_getTime(&secStart, &msecStart);      
   
   for (iteration = 0; iteration < 100; iteration++ )
   {
      ICCP_testSendCmds( ICCG_CMD_TARGET_DSPA, iteration, DATA_BASEG_ALG_TEST_LONG );
      OS_LYRG_usleep( rand() % 32768 );
   }      

   OS_LYRG_getTime(&secEnd, &msecEnd);
   ICCP_testShowResults("random delays", iteration , secStart, msecStart, secEnd, msecEnd);   
   dataCheck = 0;   


   LOGG_PRINT(LOG_INFO_E, NULL, "*****************************************************************************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "***************** ICC test program complete *********************************\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "*****************************************************************************\n");
}
#endif


#endif

