/****************************************************************************
 *
 *   FileName: sched.c
 *
 *   Author: 
 *
 *   Date: 
 *
 *   Description: scheduler - task manager
 *
 ****************************************************************************/

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_common.h"
#include "os_lyr.h"

#include "data_base.h"
#include "ictl_drv.h"
#include "icc.h"
#include "que.h"
#include "sched.h"
#include "inu_ref.h"
#include "inu_image.h"
#include "client_api.h"
#include "evthreads.h"
#include "inu_cdnn.h"
#include "inu_vision_proc.h"
#include "inu_pp.h"
// #include "inu_cnn_pipe_api.h"

// #include "inu2_internal.h"
 
/****************************************************************************
 ***************      L O C A L       D E F N I T I O N S     ***************
 ****************************************************************************/
//#define EV_SCHED_DEBUG

/****************************************************************************
 ***************      L O C A L       T Y P E D E F S         ***************
 ****************************************************************************/
   
/****************************************************************************
 ***************      L O C A L      D A T A                  ***************
****************************************************************************/
//static STACKG_stackT newJobsPool;
static QUEG_queT     pendingJobsQue;
static QUEG_queT     finishedJobsQue;
static EvMutexType   finishedJobQueMutex;
static UINT32        schedDspSource;

//   static UINT16        activeJobDescriptor;
//   static UINT16        pendingMsgJobDescriptor;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

void testFunc(void *jobDescriptorP, UINT16 jobDescriptor)
{
   DATA_BASEG_iccJobsDescriptorDataBaseT  jobsDb;
   DATA_BASEG_readDataBase((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS, 0);

   inu_ref *ref = (inu_ref*)jobsDb.jobDescriptorPool[jobDescriptor].inputList.inu_dataPhy[0];
   LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "testFunc: num = %d, phy = 0x%x, gpVirt = 0x%x\n", 
                                 jobsDb.jobDescriptorPool[jobDescriptor].inputList.databaseListNum,
                                 jobsDb.jobDescriptorPool[jobDescriptor].inputList.inu_dataPhy[0],
                                 jobsDb.jobDescriptorPool[jobDescriptor].inputList.inu_dataVirt[0]);
   LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "testFunc: refType = %d, refId = %d\n", ref->refType,ref->id);  
   inu_image *image = (inu_image*)ref;
   LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "%llu, height = %d, width = %d, data = 0x%x\n", image->imageHdr.dataHdr.timestamp, image->imageHdr.imgDescriptor.height, image->imageHdr.imgDescriptor.width,image->data.dataPhyAddr);

}


void handleGeneralMessageFromGp(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);
#if (ICC_TEST == 1)
void shortDebugFunc(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);
void longDebugFunc(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP);

static SCHEDG_algEntryPointT algsTblEntryPoint[DATA_BASEG_TARGET_ALG_NUM] =
                                                   {
                                                      handleGeneralMessageFromGp,
                                                      shortDebugFunc,
                                                      longDebugFunc,
                                                      NULL,
                                                   };
#else
static SCHEDG_algEntryPointT algsTblEntryPoint[DATA_BASEG_TARGET_ALG_NUM] =
                                                   {
                                                      handleGeneralMessageFromGp,
                                                      NULL,
                                                      NULL,
                                                      NULL,
                                                      NULL,
                                                      CDNN_EVG_app,
                                                      VISION_PROC_EVG_app,
                                                      NULL,
                                                      NULL,
                                                      PP_EVG_app,
                                                   };
#endif

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L     D A T A                 ***************
 ****************************************************************************/
EvIntCondType   SCHEDG_Condition;


/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

void handleGeneralMessageFromGp(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP)
{
   DATA_BASEG_iccJobsDescriptorDataBaseT  jobsDb;

   FIX_UNUSED_PARAM_WARN(jobDescriptorP);

   DATA_BASEG_readDataBase((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS, 0);

//   LOGG_PRINT(LOG_DEBUG_E, NULL, "handleGeneralMessageFromGp :jobDescriptor=%d\n", jobDescriptor);

   CLIENT_APIG_dspFromGpMsgRcv(DATA_BASEG_GP_TO_EV62_FDK_MSG, jobsDb.jobDescriptorPool[jobDescriptor].arg, paramsP, jobDescriptor);}
#if (ICC_TEST == 1)

void shortDebugFunc(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP)
{
    
//      DATA_BASEG_jobDescriptorT *jobDb = (DATA_BASEG_jobDescriptorT*)jobDescriptorP;
   FIX_UNUSED_PARAM_WARN(jobDescriptor);



//      LOGG_PRINT(LOG_DEBUG_E, NULL, "sdf:d=%x,cy=%u\n",jobDb->data,timer.ticks); 
}

void longDebugFunc(void *jobDescriptorP, UINT16 jobDescriptor, inu_function__operateParamsT *paramsP)
{
#define EV_CYCLES_COUNT_DELAY_10MS (8070000)

//      DATA_BASEG_jobDescriptorT *jobDb = (DATA_BASEG_jobDescriptorT*)jobDescriptorP;
   unsigned long long lastTimer,currentTimer; 
   UINT32 totalTime = 0;
   FIX_UNUSED_PARAM_WARN(jobDescriptor);
   lastTimer = getCycles();


   while(1)
   {
      currentTimer = getCycles();
      if (currentTimer < lastTimer)
      {
         break; //just ignore this trigger
      }
      else
      {
         totalTime += (int)(currentTimer - lastTimer);
      }

      if (totalTime > EV_CYCLES_COUNT_DELAY_10MS)
         break;

      lastTimer = currentTimer;      
   }

//      LOGG_PRINT(LOG_DEBUG_E, NULL, "ldf:d=%x,t=%u,cy=%u\n",jobDb->data,totalTime,currentTimer); 
}
#endif

/****************************************************************************
*
*  Function Name: SCHEDP_createInputParams
*
*  Description: 
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: scheduler
*
****************************************************************************/
void SCHEDP_fillInputParams( DATA_BASEG_inputListT *databaseInput, inu_function__operateParamsT *params )
{
   UINT16 i;
   inu_ref *refP;
   memset(params, 0 , sizeof(inu_function__operateParamsT));
   //fill input params
   for (i = 0; i < databaseInput->databaseListNum; i++)
   {
      params->dataInputs[i] = databaseInput->inu_dataPhy[i];
      refP = (inu_ref*)params->dataInputs[i];
      evDataCacheFlushInvalidateLines(refP, refP + sizeof(inu_ref));
      evDataCacheFlushInvalidateLines(refP + sizeof(inu_ref), refP + refP->refSize);
   }
   params->dataInputsNum = databaseInput->databaseListNum;  
}




/****************************************************************************
*
*  Function Name: SCHEDP_isJobsPending
*
*  Description: 
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: scheduler
*
****************************************************************************/
void SCHEDP_addNewJobs( void )
{
   //if we want to have priority, then we need to sort the new job in a priority queue.

   //UINT32 newJobDescriptor = 0;
   
   //while(STACKG_pop(&newJobsPool,(STACKG_elemT*)&newJobDescriptor) == SUCCESS_E)
   //{      
      //SCHEDP_sortJob(newJobDescriptor);      

   //}
}

/****************************************************************************
*
*  Function Name: SCHEDP_isJobsPending
*
*  Description: 
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: scheduler
*
****************************************************************************/
INT32 SCHEDP_isJobsPending( void )
{
   INT32 numOfJobsPending;
   //if we want to have priority, then we need to check the priority queue.

   ICTL_DRVP_maskInt(1);
   numOfJobsPending = QUEG_numElements(&pendingJobsQue);   
   ICTL_DRVP_unMaskInt(1);

   return ((numOfJobsPending > 0) ? 1 : 0);   
}


/****************************************************************************
 ***************      G L O B A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: SCHEDG_pushNewJob
*
*  Description: 
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: scheduler, called from ISR
*
****************************************************************************/
void SCHEDG_pushNewJob( UINT32 jobDescriptor )
{
//   INT32 numOfJobsPending;
   LOGG_PRINT(LOG_DEBUG_E, 0, "PUSH ISR %d	NumElm=%d Max cnt=%d\n", jobDescriptor, QUEG_numElements(&pendingJobsQue),pendingJobsQue.maxCnt);

   //STACKG_push(&newJobsPool,(UINT32)jobDescriptor);
   if (QUEG_enque(&pendingJobsQue,(QUEG_elemT)jobDescriptor) < 0)
   {
      LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "e q is full\n");
   }
//   numOfJobsPending = QUEG_numElements(&pendingJobsQue);
//   LOGG_PRINT(LOG_DEBUG_E, NULL, "SCHEDG_pushNewJob: numOfJobsPending=%d\n", numOfJobsPending);
}

/****************************************************************************
*
*  Function Name: SCHEDG_pushFinishedJob
*
*  Description: Update  scheduler a job is done, by pushing its descriptor to the Finished jobs que
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: ALGs/CNN threads
*
****************************************************************************/
void SCHEDG_pushFinishedJob( UINT32 jobDescriptor )
{
    
   evMutexLock(&finishedJobQueMutex);
   if (jobDescriptor != SCHED_NOT_FINISHED_JOB)
   {
      if(QUEG_enque(&finishedJobsQue,(QUEG_elemT)jobDescriptor) < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "finishedJobsQue is full\n");
      }
   }
   evMutexUnLock(&finishedJobQueMutex);
}

/****************************************************************************
*
*  Function Name: ev_icc_thread
*
*  Description: Handle messages to/from the GP
*
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: void
*
*  Context: ALGs/CNN threads
*
****************************************************************************/
void *ev_icc_thread(void *arg)
{
   INT32 ret;
   volatile UINT32 k;
   UINT16 finishedAlgoDescriptor;
   //if we want to have priority, then we need to get the next job from the priority queue.
   DATA_BASEG_iccJobsDescriptorDataBaseT  jobsDb;
   UINT16                                 jobDescriptor;
   inu_function__operateParamsT           params;

   LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "EV scheduler start, core %d\n", evGetCpu());

   OS_LYRG_WHILE
   {
#ifdef EV_SCHED_DEBUG   
      LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "s,j=%d,p=%d,d=%d\n",finishedAlgoDescriptor,SCHEDP_isJobsPending(),schedDspSource);
#endif
   //Waiting for a signal (from the ICC for a new command, or from any thread for done job
   evIntCondWait(&SCHEDG_Condition);

   if (QUEG_numElements(&finishedJobsQue))
   {

      ICTL_DRVP_maskInt(1);
      if (ICC_DRVG_allowedToSendMsg())
      {
        evMutexLock(&finishedJobQueMutex);
         QUEG_deque(&finishedJobsQue,(QUEG_elemT*)&finishedAlgoDescriptor);
         evMutexUnLock(&finishedJobQueMutex);
         ret = ICC_DRVG_sendMsg(schedDspSource, finishedAlgoDescriptor);

         if (ret >= MAX_NUM_POLL_CYCLES)
         {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL,  "error: didn't send message - GP was in ISR\n");
         }
         ICTL_DRVP_unMaskInt(1);
      }
      else
      {
         ICTL_DRVP_unMaskInt(1);
         //Add some delay to enable interrupts before masking interrupts again
         for (k = 0; k<10000; k++)
         {
            __asm__ volatile("nop");
         }
         LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL,  "didn't send message yet, - waiting for GP ACK\n");
         evIntCondSignal(&SCHEDG_Condition);
      }

   }
   else
   {
      if (SCHEDP_isJobsPending())
      {
         ICTL_DRVP_maskInt(1);
         QUEG_deque(&pendingJobsQue,(QUEG_elemT*)&jobDescriptor);
         ICTL_DRVP_unMaskInt(1);
         DATA_BASEG_readDataBase((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS, 0);

#ifdef EV_SCHED_DEBUG
         LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)NULL, "s exec job,j=%d,alg=%d\n",jobDescriptor,jobsDb.jobDescriptorPool[jobDescriptor].alg);
#endif

         if ((jobDescriptor < ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE) && (jobsDb.jobDescriptorPool[jobDescriptor].alg < DATA_BASEG_TARGET_ALG_NUM) && (algsTblEntryPoint[jobsDb.jobDescriptorPool[jobDescriptor].alg] != NULL))
         {
            SCHEDP_fillInputParams(&jobsDb.jobDescriptorPool[jobDescriptor].inputList, &params);
            algsTblEntryPoint[jobsDb.jobDescriptorPool[jobDescriptor].alg](&jobsDb.jobDescriptorPool[jobDescriptor], jobDescriptor, &params);
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "s3 exec error,j=%d,alg=%d, EP = %x\n",jobDescriptor,jobsDb.jobDescriptorPool[jobDescriptor].alg, algsTblEntryPoint[jobsDb.jobDescriptorPool[jobDescriptor].alg]);
         }
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "sched thread was triggered but no job %d \n", finishedAlgoDescriptor);
      }
   }

   //       evYieldThread();
   }
}

/****************************************************************************
*
*  Function Name: SCHEDG_entryPoint
*
*  Description: 
*   
*  Inputs:   finishedSchedulingAlgo
*
*  Outputs: none
*
*  Returns: finishedSchedulingAlgo
*
*  Context: scheduler
*
****************************************************************************/
void SCHEDG_entryPoint(UINT32 dspSource)
{

   EvThreadType t;
   void* thread_ret;

   schedDspSource = dspSource;
   t = evNewThread(ev_icc_thread, EVTH_INIT_CPU, ICC_THREAD_CPU, EVTH_INIT_ARGPTR, NULL, EVTH_INIT_PRIORITY, ICC_THREAD_PRIORITY, EVTH_INIT_PREEMPTIBLE, 1, EVTH_INIT_LAST);
   if (t == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)NULL, "Failed to open ev_icc_thread\n");
      return;
   }

   evJoinThread(t, &thread_ret);
}

/****************************************************************************
*
*  Function Name: SCHEDG_registerTaskEntryPoint
*
*  Description: register new task entry point to scheduler
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: RET_CODE
*
*  Context: scheduler
*
****************************************************************************/
// ERRG_codeE SCHEDG_registerTaskEntryPoint(UINT32 taskId, SCHEDG_algEntryPointT taskEntryPoint)
// {
//    DATA_BASEG_targetAlgJobE algJobId = (DATA_BASEG_targetAlgJobE)taskId;
//    algsTblEntryPoint[algJobId] = taskEntryPoint;
// }

/* SchedSwHandler */
_Interrupt void SchedSwHandler(void) {
   /* Clean interrupt & preempt */
   _psp_set_aux(PSP_AUX_IRQ_HINT, 0);
   evPreempt();
}

/* SCHEDG_preempt_init */
static void *SCHEDG_preempt_init(void *args)
{
   int oldInts = evIntsOff();

   _setvecti(SCHED_SW_IRQ, SchedSwHandler);
   evIntMask(SCHED_SW_IRQ, EVTH_CPU_INT_ENABLE);
   evIntsRestore(oldInts);

   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "SCHEDG_preempt_init_rpc Done on %d\n", evGetCpu());
}

/* inu_preempt */
void SCHEDG_preempt(void)
{
   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "SCHEDG_preempt\n");
   _psp_set_aux(PSP_AUX_IRQ_HINT, SCHED_SW_IRQ);
}

/****************************************************************************
*
*  Function Name: SCHEDG_init
*
*  Description: init of SCHED module.
*   
*  Inputs:   none
*
*  Outputs: none
*
*  Returns: RET_CODE
*
*  Context: scheduler
*
****************************************************************************/
void SCHEDG_init()
{
   int status;
   
   LOGG_PRINT(LOG_DEBUG_E, (ERRG_codeE)NULL, "scheduler init\n");

//   STACKG_init(&newJobsPool,ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE);
   pendingJobsQue.maxCnt  = ICC_COMMONG_JOB_DESCRIPTOR_POOL_SIZE;
   finishedJobsQue.maxCnt = QUEG_MAX_ELEMENTS;
   QUEG_init(&pendingJobsQue);
   QUEG_init(&finishedJobsQue);
   
   status = evMutexInit(&finishedJobQueMutex);
   if(status!= EVTH_ERROR_OK)
   {
       LOGG_PRINT(LOG_ERROR_E, (ERRG_codeE)0, "SCHEDG_init: Failed to create finishedJobQueMutex\n");
   }
   
   evRpc(SCHEDG_preempt_init, NULL, 0); // Init SW IRQ on CPU 0 
   evRpc(SCHEDG_preempt_init, NULL, 1); // Init SW IRQ on CPU 1
}


