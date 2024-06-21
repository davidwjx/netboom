/****************************************************************************
 *
 *   FileName: trigger_mngr.c
 *
 *   Author:  Elad R.
 *
 *   Date:
 *
 *   Description: trigger mngr
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "inu2_internal.h"
#include "sensors_mngr.h"
#include "trigger_mngr.h"
#include "cde_mngr.h"
#include "gme_mngr.h"
#include "gpio_drv.h"
#include "iae_drv.h"
#include "assert.h"
#include "sequence_mngr.h"
#include "inu_alt.h"
#include "sensorsync_updater.h"
#include "gme_drv.h"
#include <unistd.h>
#include "helsinki.h" 
#include "rtc.h"
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define TRIGGER_MNGRP_SUPPORTED_TRIGGER   (4)
#define TRIGGER_MNGRP_NUM_OF_SAMPLING     (4)
#define TRIGGER_MNGRP_SAMPLING_BUF_SIZE   (100)
#define TRIGGER_MNGRP_INDEX_TO_CALC_DELAY (2)
#define TRIGGER_MNGRP_NUM_OF_SLU          (6)
#define TRIGGER_MNGRP_IS_TIMER_SRC(src)   ((src == INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER1_E) || (src ==INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER2_E) || (src ==INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E))?1:0
//#define TRIGGER_MNGRP_DEBUG_PRINT

#define TRIGGER_MNGRP_MSGQ_NAME      "/TriggerMngrMsgQue"
#define TRIGGER_MNGRP_MSGQ_SIZE      (sizeof(TRIGGER_MNGRP_msgQParamsT))
#define TRIGGER_MNGRP_MSGQ_MAX_MSG   (20)
#define TRIGGER_MNGRP_MSGQ_TIMEOUT   (100)

#define DOUBLE_SEQ_DB
#define DYNAMIC_EXPOSURE_DELAY

#define TRIGGER_MNGRP_NUM_SEQ_DB     (2)
#define FSG_BASED_TRIGGERING_MODE    /* Uses the FSG for generating sync pulses*/
/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
   TRIGGER_MNGR_DONE_COLLECT_TS_E,
}TRIGGER_MNGRP_eventT;

typedef struct TRIGGER_MNGRP_seqInfo
{
   UINT64                   gpioBitMap;
   UINT32                   delay;
   UINT32                   seqPulseDelay;
   UINT32                   ratioFromTrigger;
   UINT32                   triggerCounter;
   INU_DEFSG_senSelectE     sensorSelect;
}TRIGGER_MNGRP_seqInfo;

typedef struct TRIGGER_MNGRP_seqNode
{
   struct TRIGGER_MNGRP_seqInfo   data;
   struct TRIGGER_MNGRP_seqNode  *next;
}TRIGGER_MNGRP_seqNode;

typedef struct TRIGGER_MNGRP_anchorInfo
{
   INU_DEFSG_sensorTriggerSrcE   source;
   SENSORS_MNGRG_sensorInfoT     *anchorSensorInfo;
}TRIGGER_MNGRP_anchorInfo;

typedef struct TRIGGER_MNGRP_sequenceInfo
{
   TRIGGER_MNGRP_seqNode         *seqCurrent;
   OS_LYRG_timerT                sequenceTimer;
   void                          *triggerInfoP;
}TRIGGER_MNGRP_sequenceInfo;

typedef struct TRIGGER_MNGRP_triggerInfo
{
   //General trigger params
   TRIGGER_MNGRP_anchorInfo      anchorInfo;
   OS_LYRG_mutexT                mutex;
   SENSORS_MNGRG_sensorInfoT     *sensorsInfo[INU_DEFSG_NUM_OF_INPUT_SENSORS];
   void                          *isrEntry;
   UINT32                        lcmForTimer;

   //Sequence params
#ifdef DOUBLE_SEQ_DB
   UINT32                        currentSeqDb;
   TRIGGER_MNGRP_sequenceInfo    seqDb[TRIGGER_MNGRP_NUM_SEQ_DB];
#else
   TRIGGER_MNGRP_seqNode         *seqCurrent;
   OS_LYRG_timerT                sequenceTimer;
#endif
   TRIGGER_MNGRP_seqNode         *seq;
   UINT32                        stopSeq;
   UINT32                        pauseTrigger;
}TRIGGER_MNGRP_triggerInfo;

typedef struct TRIGGER_MNGRP_autoModeParams
{
   UINT16         sensorsParticipant;
   UINT16         numParticipants;
   UINT16         sensorsNotParticipant;
   UINT16         numNotParticipants;
   INT32          score;
   UINT32         anchorSensorSelect;
}TRIGGER_MNGRP_autoModeParams;

typedef struct
{
   OS_LYRG_threadHandle    thrdH;
   OS_LYRG_msgQueT         msgQue;
   UINT32                  threadActive;
   UINT32                  runThread;
}TRIGGER_MNGRP_infoT;

typedef struct
{
   TRIGGER_MNGRP_eventT    msgCode;
}TRIGGER_MNGRP_msgQParamsT;

typedef struct
{
       unsigned int usecWidth;
       unsigned int fps;
}TRIGGER_MNGR_pwmT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static OS_LYRG_timerT            TRIGGER_MNGRP_Timer1;
static OS_LYRG_timerT            TRIGGER_MNGRP_Timer2;
static OS_LYRG_timerT            TRIGGER_MNGRP_Timer3;
static TRIGGER_MNGRP_triggerInfo TRIGGER_MNGRP_triggerInfoTbl[TRIGGER_MNGRP_SUPPORTED_TRIGGER];
static TRIGGER_MNGR_pwmT         TRIGGER_MNGR_pwmDb[INU_DEVICE_NUM_PWM_E];

UINT64                           sluTsTable[TRIGGER_MNGRP_NUM_OF_SLU][TRIGGER_MNGRP_SAMPLING_BUF_SIZE];
UINT32                           sluTsCnt[TRIGGER_MNGRP_NUM_OF_SLU];
UINT8                            sluDoneSampling = 0;
UINT8                            sluRegCbBitmap = 0;
TRIGGER_MNGRP_autoModeParams     bestTriggerOrg;
SENSORS_MNGRG_sensorInfoT       *sensorAutoList[INU_DEFSG_NUM_OF_INPUT_SENSORS] = {0};
TRIGGER_MNGRP_infoT              TRIGGER_MNGRP_info;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

static inline void TRIGGER_MNGRP_initTriggerInfo( TRIGGER_MNGRP_triggerInfo *triggerInfoP );
static inline UINT32 TRIGGER_MNGRP_calcGcd( UINT32 fps1, UINT32 fps2 );
static inline UINT32 TRIGGER_MNGRP_calcLcm( UINT32 fps1, UINT32 fps2 );
static void TRIGGER_MNGRP_handleMsg(TRIGGER_MNGRP_msgQParamsT *msgQParamsP);
static INT32 TRIGGER_MNGRP_thread(void* argP);
static inline ERRG_codeE TRIGGER_MNGRP_toggleGpio( UINT64 gpioBitmap, UINT32 seqPulseDelay, UINT32 paused);
static ERRG_codeE TRIGGER_MNGRP_stopAllAutoTriggers(void);
static ERRG_codeE TRIGGER_MNGRP_startAllTriggers(void);
static ERRG_codeE TRIGGER_MNGRP_setTriggerByScoring( TRIGGER_MNGRP_autoModeParams *autoTriggerParamsP);
static ERRG_codeE TRIGGER_MNGRP_autoReorganization(TRIGGER_MNGRP_autoModeParams *max);
static void TRIGGER_MNGRP_sequenceStep(void *argP);
static void TRIGGER_MNGRP_sequenceStart(void *argP);
static void TRIGGER_MNGRP_sluInterCb(UINT64 ts, UINT32 slu, void *argP);
static ERRG_codeE TRIGGER_MNGRP_unregisterTuningDelayCb (SENSORS_MNGRG_sensorInfoT *sensorInfoP);
static ERRG_codeE TRIGGER_MNGRP_unregisterTuningDelayAllCb ();
static ERRG_codeE TRIGGER_MNGRP_matchIndexes(UINT64 *anchorTimeList,UINT64 *timeList,UINT32 trigIndx, UINT32 *matchInd);
static ERRG_codeE TRIGGER_MNGRP_calcDelay (TRIGGER_MNGRP_autoModeParams *triggerMapping);
static void TRIGGER_MNGRP_sluTuningInterCb(UINT64 ts, UINT32 slu, void *argP);
static void TRIGGER_MNGRP_sortedInsert(TRIGGER_MNGRP_seqNode** head_ref, TRIGGER_MNGRP_seqNode* new_node);
void TRIGGER_MNGRP_print_list(TRIGGER_MNGRP_seqNode * head);
static ERRG_codeE TRIGGER_MNGRP_findTriggerSrcIndex( INU_DEFSG_sensorTriggerSrcE src, UINT32 *ind);
static void TRIGGER_MNGRP_addTriggerToSeq  (TRIGGER_MNGRP_triggerInfo *triggerInfoP, INU_DEFSG_senSelectE sensorSel);
static void TRIGGER_MNGRP_deleteTriggerFromSeq  (TRIGGER_MNGRP_triggerInfo *triggerInfoP, INU_DEFSG_senSelectE sensorSel);
static void TRIGGER_MNGRP_resetTriggerCounter  (TRIGGER_MNGRP_triggerInfo *triggerInfoP);
static ERRG_codeE TRIGGER_MNGRP_registerCbTuningDelay ();
static ERRG_codeE TRIGGER_MNGRP_startTimer(OS_LYRG_timerT *timer, UINT32 fps, void *argP);
static void TRIGGER_MNGRP_resetAllTriggerDelay();
static void TRIGGER_MNGRG_stopPwm(unsigned int pwmTrigNum);


/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
static inline void TRIGGER_MNGRP_initTriggerInfo( TRIGGER_MNGRP_triggerInfo *triggerInfoP )
{
   UINT32 i;
   OS_LYRG_lockMutex(&(triggerInfoP->mutex));
   triggerInfoP->anchorInfo.source = INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E;
   triggerInfoP->anchorInfo.anchorSensorInfo = NULL;
   triggerInfoP->stopSeq = 0;
   triggerInfoP->pauseTrigger = 0;
#ifdef DOUBLE_SEQ_DB
   triggerInfoP->currentSeqDb = 0;
   for (i = 0;i < TRIGGER_MNGRP_NUM_SEQ_DB;i++)
   {
      triggerInfoP->seqDb[i].seqCurrent = NULL;
   }
#else
   triggerInfoP->seqCurrent = NULL;
#endif
   triggerInfoP->seq = NULL;
   triggerInfoP->isrEntry = NULL;
   memset(triggerInfoP->sensorsInfo,0,sizeof(SENSORS_MNGRG_sensorInfoT*)*INU_DEFSG_NUM_OF_INPUT_SENSORS);
   OS_LYRG_unlockMutex(&(triggerInfoP->mutex));
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_calcGcd
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static inline UINT32 TRIGGER_MNGRP_calcGcd( UINT32 fps1, UINT32 fps2 )
{
   if (fps2 == 0)
   {
      return fps1;
   }
   return TRIGGER_MNGRP_calcGcd(fps2, fps1 % fps2);
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_calcLcm
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static inline UINT32 TRIGGER_MNGRP_calcLcm( UINT32 fps1, UINT32 fps2 )
{
   return ((fps1*fps2) / (TRIGGER_MNGRP_calcGcd(fps1, fps2)));
}

static void TRIGGER_MNGRP_handleMsg(TRIGGER_MNGRP_msgQParamsT *msgQParamsP)
{
   ERRG_codeE                          retCode;

   switch(msgQParamsP->msgCode)
   {
      case TRIGGER_MNGR_DONE_COLLECT_TS_E:
      {
         retCode = TRIGGER_MNGRP_unregisterTuningDelayAllCb();
         if (ERRG_FAILED(retCode))
         {
            return;
         }
         retCode = TRIGGER_MNGRP_calcDelay(&bestTriggerOrg);
         if (ERRG_FAILED(retCode))
         {
            return;
         }

      }
      //default
   }
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_thread
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: thread to recive messaged and
*
****************************************************************************/
static INT32 TRIGGER_MNGRP_thread(void* argP)
{
   TRIGGER_MNGRP_msgQParamsT   msg;
   INT32                   status   = 0;
   UINT32                  msgSize  = 0;
   TRIGGER_MNGRP_infoT     *mngrInfoP = (TRIGGER_MNGRP_infoT*)argP;

   LOGG_PRINT(LOG_INFO_E, NULL, "TRIGGER_MNGR thread started\n");

   mngrInfoP->threadActive = 1;
   while(mngrInfoP->runThread)
   {
      msgSize  = sizeof(msg);
      status   = OS_LYRG_recvMsg(&mngrInfoP->msgQue, (UINT8*)&msg, &msgSize, TRIGGER_MNGRP_MSGQ_TIMEOUT);
      if((status == SUCCESS_E) && (msgSize == sizeof(msg)) /*&& (TRIGGER_MNGRP_info.threadIsActive)*/)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL,"TRIGGER_MNGRP_thread msg recieved\n");
         TRIGGER_MNGRP_handleMsg(&msg);
      }
   }
   mngrInfoP->threadActive = 0;

   LOGG_PRINT(LOG_INFO_E, NULL, "TRIGGER_MNGR thread exited\n");

   OS_LYRG_exitThread();

   return status;
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_toggleGpio
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static inline ERRG_codeE TRIGGER_MNGRP_toggleGpio( UINT64 gpioBitmap, UINT32 seqPulseDelay, UINT32 paused)
{
   GPIO_DRVG_gpioSetPortValParamsT     portParams;
   ERRG_codeE                          retCode;

   if (paused) {
      gpioBitmap = 0;
   }
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Toggling GPIO Bitmap 0x%x, delay=%d \n",gpioBitmap ,seqPulseDelay);
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_PORT_VAL_CMD_E, &portParams);
   portParams.portVal |= gpioBitmap;
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_PORT_VAL_CMD_E, &portParams);

   OS_LYRG_usleep(seqPulseDelay);

   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_PORT_VAL_CMD_E, &portParams);
   portParams.portVal &= ~(gpioBitmap);
   retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_PORT_VAL_CMD_E, &portParams);

   return retCode;
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_autoTrigger
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
inline UINT32 getNextSelect(UINT32 *bitmap)
{
   int j = 0 ;
   UINT32 copyBitmap = *bitmap;
   while(((copyBitmap & 0x1) != 0x1) && (j < 32))
   {
      copyBitmap = copyBitmap >> 1;
      j++;
   }

   if (j != 32)
   {
      *bitmap &= ~(1 << j);
      return j;
   }
   assert(0);
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_stopAllAutoTriggers
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_stopAllAutoTriggers(void)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 i;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP;

   for (i=0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      sensorInfoP = sensorAutoList[i];
      if (sensorInfoP)
      {
         if ((sensorInfoP->sensorCfg.isTriggerSupported) && (sensorInfoP->triggerH))
         {
            retCode = TRIGGER_MNGRG_stopTrigger(sensorInfoP);
            if (ERRG_FAILED(retCode))
            {
               LOGG_PRINT(LOG_ERROR_E, NULL,"failed to stop trigger sensor %d\n",sensorInfoP->sensorCfg.sensorSelect);
               return retCode;
            }
            LOGG_PRINT(LOG_DEBUG_E, NULL,"stop trigger to sensor %d trigger source %d\n",sensorInfoP->sensorCfg.sensorSelect, sensorInfoP->sensorCfg.triggerSrc);
         }
      }
   }
   return retCode;
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_startAllTriggers
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_startAllTriggers(void)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 i;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP;

   for (i=0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      sensorInfoP = sensorAutoList[i];
      if (sensorInfoP)
      {
         if (sensorInfoP->sensorCfg.isTriggerSupported)
         {
            retCode = TRIGGER_MNGRG_startTrigger((void*)sensorInfoP,&sensorInfoP->triggerH);
            if (ERRG_FAILED(retCode))
            {
               LOGG_PRINT(LOG_ERROR_E, NULL,"failed to start trigger sensor %d\n",sensorInfoP->sensorCfg.sensorSelect);
               return retCode;
            }
         }
      }
   }
   return retCode;
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_setTriggerByScoring
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_setTriggerByScoring( TRIGGER_MNGRP_autoModeParams *autoTriggerParamsP)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT *anchorSensorInfo;
   UINT32                 tempBitmap, sensorSelect, sluInst;

   anchorSensorInfo = sensorAutoList[autoTriggerParamsP->anchorSensorSelect];

   if (anchorSensorInfo->sensorCfg.isTriggerSupported)
   {
      tempBitmap = autoTriggerParamsP->sensorsParticipant;
      while(tempBitmap)
      {
         sensorSelect = getNextSelect(&tempBitmap);
         if (sensorAutoList[sensorSelect]->sensorCfg.isTriggerSupported)//checking anchor twice
         {
            sensorAutoList[sensorSelect]->sensorCfg.triggerSrc = INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E; //simple implementation for now, assume timer3 is for auto purpose only
            LOGG_PRINT(LOG_INFO_E, NULL,"sensor %d (model %d) marked to triggered by timer #%d\n",sensorSelect,sensorAutoList[sensorSelect]->sensorCfg.sensorModel,INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E);
         }
         else
         {
            //trap assert - there is no option for sensors participating which does not support trigger when the anchor is supporting trigger
            LOGG_PRINT(LOG_INFO_E, NULL,"ERROR! sensor %d (model %d) does not supports trigger but is not in participating group with anchor who does support\n",sensorSelect,sensorAutoList[sensorSelect]->sensorCfg.sensorModel);
            assert(0);
         }
      }
   }
   else//run anchor as free running, all other participants to triggered by his SLU
   {
      anchorSensorInfo->sensorCfg.triggerSrc = 0;
      LOGG_PRINT(LOG_INFO_E, NULL,"sensor %d (model %d) marked to be anchor and free running mode\n",autoTriggerParamsP->anchorSensorSelect,sensorAutoList[autoTriggerParamsP->anchorSensorSelect]->sensorCfg.sensorModel);
      //bring SLU instance  of anchor
      retCode = SEQ_MNGRG_getSluInstBySenInst(autoTriggerParamsP->anchorSensorSelect, &sluInst);
      if (ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "failed to get anchor (instane %d) SLU instance\n",autoTriggerParamsP->anchorSensorSelect);
         return retCode;
      }
      tempBitmap = autoTriggerParamsP->sensorsParticipant;
      while(tempBitmap)
      {
         sensorSelect = getNextSelect(&tempBitmap);
         if ((sensorAutoList[sensorSelect]->sensorCfg.isTriggerSupported) && (sensorSelect != autoTriggerParamsP->anchorSensorSelect))
         {
            sensorAutoList[sensorSelect]->sensorCfg.triggerSrc = sluInst + INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU0_E;
            LOGG_PRINT(LOG_INFO_E, NULL,"sensor %d (model %d) marked to triggered by SLU #%d\n",sensorSelect,sensorAutoList[sensorSelect]->sensorCfg.sensorModel,sluInst);
         }
         else if ((!sensorAutoList[sensorSelect]->sensorCfg.isTriggerSupported) && (sensorSelect != autoTriggerParamsP->anchorSensorSelect))
         {
            //trap assert - there is no option for sensors participating which does not support trigger when the anchor does not supporting trigger
            LOGG_PRINT(LOG_INFO_E, NULL,"ERROR! sensor %d (model %d) does not supports trigger and is in participating group with anchor who does not support\n",sensorSelect,sensorAutoList[sensorSelect]->sensorCfg.sensorModel);
            assert(0);
         }
      }
   }

   //not paticipants
   tempBitmap = autoTriggerParamsP->sensorsNotParticipant;
   while(tempBitmap)
   {
      sensorSelect = getNextSelect(&tempBitmap);
      if (!sensorAutoList[sensorSelect]->sensorCfg.isTriggerSupported)//?
      {
         sensorAutoList[sensorSelect]->sensorCfg.triggerSrc = INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E;
         LOGG_PRINT(LOG_INFO_E, NULL,"sensor %d (model %d) marked to free running (not participant on gruop)\n",sensorSelect,sensorAutoList[sensorSelect]->sensorCfg.sensorModel);
      }
      else
      {
         //This should not collide if there are other sensors which were put on TIMER1 - the LCM will just change to support this added sensor
         sensorAutoList[sensorSelect]->sensorCfg.triggerSrc = INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER1_E;
         LOGG_PRINT(LOG_INFO_E, NULL,"found a sensor (%d,%d) that supports trigger which is not participants\n",sensorSelect,sensorAutoList[sensorSelect]->sensorCfg.sensorModel);
      }
      //if there was a way to optimize, it was take a part on the GCD gruop
   }

   return retCode;

}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_autoReorganization
*
*  Description: not fully optimized, can also check for sensors out of group.
*                     score - the more we are able to unify frames of different
*                     sensors, the higher the score. frames we are not able to unify, reduces score
*
*                     scoring functin: (sum FPS participants) - (sum GCD(x1,x2) all combination without reapetition) + DCD(all, if num participants >2)+ (sum fps not participants)
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_autoReorganization(TRIGGER_MNGRP_autoModeParams *max)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 i, j, gcdAll, sensorSelect, tempBitmap;
   TRIGGER_MNGRP_autoModeParams temp;;
   SENSORS_MNGRG_sensorInfoT  *tmpSensorInfoP1,*tmpSensorInfoP2;
   UINT32 partFpsTbl[INU_DEFSG_NUM_OF_INPUT_SENSORS]={0};

   max->score = 100000; //todo  set max positive
   max->sensorsParticipant = 0;
   max->sensorsNotParticipant = 0;
   max->numParticipants = 0;
   max->anchorSensorSelect = 0;
   max->numNotParticipants = 0;

   for (i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      tmpSensorInfoP1 = sensorAutoList[i];
      if (tmpSensorInfoP1)
      {
         temp.score = 0;
         temp.sensorsParticipant = 0;
         temp.sensorsNotParticipant = 0;
         temp.numParticipants = 0;
         temp.numNotParticipants= 0;

         temp.sensorsParticipant |= 1 << tmpSensorInfoP1->sensorCfg.sensorSelect;
         temp.numParticipants++;

         //first loop is to collect the sensor that can be trigger group
         for (j = 0; j < INU_DEFSG_NUM_OF_INPUT_SENSORS; j++)
         {
            //skip checking against same sensor
            if (i == j)
            {
               continue;
            }

            tmpSensorInfoP2 = sensorAutoList[j];
            if (!tmpSensorInfoP2)
            {
               continue;
            }

            //if the other sensor does not support trigger, then we skip it and let the loop calculate the score
            //while he is the anchor
            if (!tmpSensorInfoP2->sensorCfg.isTriggerSupported)
            {
               LOGG_PRINT(LOG_DEBUG_E, NULL,"opposite sensor (%d,%d) does not support trigger, continue\n",
                        tmpSensorInfoP1->sensorCfg.sensorSelect,tmpSensorInfoP2->sensorCfg.sensorSelect);
               temp.sensorsNotParticipant |= (1 << tmpSensorInfoP2->sensorCfg.sensorSelect);
               temp.numNotParticipants++;
               continue;
            }

            //if the anchor sensor does not support trigger, but the other is, then to be able to trigger it from
            //from the anchor sensor, then it must be multiplie of the fps
            if ((!tmpSensorInfoP1->sensorCfg.isTriggerSupported) &&
               (tmpSensorInfoP1->sensorCfg.fps % tmpSensorInfoP2->sensorCfg.fps) != 0)
            {
               LOGG_PRINT(LOG_DEBUG_E, NULL,"anchor does not support trigger, and opposite sensor is not multiplie (%d,%d), continue\n",
                        tmpSensorInfoP1->sensorCfg.sensorSelect,tmpSensorInfoP2->sensorCfg.sensorSelect);
               temp.sensorsNotParticipant |= (1 << tmpSensorInfoP2->sensorCfg.sensorSelect);
               continue;
            }

            temp.sensorsParticipant |= 1 << tmpSensorInfoP2->sensorCfg.sensorSelect;
            temp.numParticipants++;
         }

         //save anchor sensor
         temp.anchorSensorSelect = tmpSensorInfoP1->sensorCfg.sensorSelect;

         LOGG_PRINT(LOG_DEBUG_E, NULL,"group to test: num %d, participants 0x%x\n",temp.numParticipants,temp.sensorsParticipant);

         //score parrticipant
         tempBitmap = temp.sensorsParticipant;
         for (j = 0; j < temp.numParticipants; j++)
         {
            sensorSelect = getNextSelect(&tempBitmap);
            partFpsTbl[j] = sensorAutoList[sensorSelect]->sensorCfg.fps;
         }
         if (temp.numParticipants == 1)
         {
            temp.score = partFpsTbl[0];
         }
         else if (temp.numParticipants == 2)
         {
            temp.score = partFpsTbl[0] + partFpsTbl[1] - TRIGGER_MNGRP_calcGcd(partFpsTbl[0], partFpsTbl[1]);
         }
         else if (temp.numParticipants == 3)
         {
            gcdAll = TRIGGER_MNGRP_calcGcd(partFpsTbl[0],partFpsTbl[1]);
            gcdAll = TRIGGER_MNGRP_calcGcd(partFpsTbl[2],gcdAll);

            temp.score = partFpsTbl[0] + partFpsTbl[1] + partFpsTbl[2];//sum fps
            temp.score = temp.score - (TRIGGER_MNGRP_calcGcd(partFpsTbl[0], partFpsTbl[1])+TRIGGER_MNGRP_calcGcd(partFpsTbl[0], partFpsTbl[2])+TRIGGER_MNGRP_calcGcd(partFpsTbl[1], partFpsTbl[2]));
            temp.score = temp.score + gcdAll;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL,"Supporting for maximum 3 sensors participant in auto mode\n");
            assert(0);
         }
         //score not parrticipant
         //for sensors not participating, add their frames
         tempBitmap = temp.sensorsNotParticipant;
         while(tempBitmap)
         {
            sensorSelect = getNextSelect(&tempBitmap);
            temp.score = temp.score + sensorAutoList[sensorSelect]->sensorCfg.fps;
         }
         LOGG_PRINT(LOG_DEBUG_E, NULL,"group result: score = %d, numParticipants = %d, anchor = %d, participants = 0x%x, not participants = 0x%x\n",
            temp.score,temp.numParticipants,temp.anchorSensorSelect,temp.sensorsParticipant,temp.sensorsNotParticipant);
         if (temp.score < max->score)
         {
            *max = temp; //copy all struct
         }
         //else condition neccesary in cases that we got same score but we want the anchor will be the one with highest fps.
         //example: FE 10fps IR 5fps. same score(10) but we need FE to be anchor for the delay calculation process.
         else if ((temp.score == max->score) && (sensorAutoList[temp.anchorSensorSelect]->sensorCfg.fps > sensorAutoList[max->anchorSensorSelect]->sensorCfg.fps))
         {
            *max = temp; //copy all struct
         }
      }
   }

   LOGG_PRINT(LOG_INFO_E, NULL,"result: score = %d, numPart = %d, anchor = %d, part = 0x%x, not part = 0x%x\n",
            max->score,max->numParticipants,max->anchorSensorSelect,max->sensorsParticipant,max->sensorsNotParticipant);

   //now we need to create triggers according to the group:
   //if all participants support trigger, create sw timer and set them to that
   //if anchor does not support trigger, (and others must support), bind them to it's slu
   //for not pariticipants, each one try to optimize - if it support trigger, bind to a new timer, if not, free running
   if ((max->numNotParticipants) || (max->numParticipants)) //TODO check if can be removed
   {
      retCode = TRIGGER_MNGRP_setTriggerByScoring(max);
   }

   //if (max->numNotParticipants > 1)
   //{
   //   TRIGGER_MNGRP_autoReorganization... //future optimization - recursive on the rest
   //}

   return retCode;
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_sequenceStep
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static void TRIGGER_MNGRP_sequenceStep(void *argP)
{
   ERRG_codeE                 retCode      = SENSORS_MNGR__RET_SUCCESS;
#ifndef DOUBLE_SEQ_DB
   TRIGGER_MNGRP_triggerInfo *triggerInfoP = (TRIGGER_MNGRP_triggerInfo*)argP;
#else
   TRIGGER_MNGRP_sequenceInfo *seqInfoP = (TRIGGER_MNGRP_sequenceInfo*)argP;
   TRIGGER_MNGRP_triggerInfo *triggerInfoP = (TRIGGER_MNGRP_triggerInfo*)seqInfoP->triggerInfoP;
#endif
   UINT32                     triggerCountCalc, currentDelay, pulseDelay=0, pulseDelayCompensate=0;
   UINT64                     gpioBitmap = 0;
#ifdef DOUBLE_SEQ_DB
    if (triggerInfoP->stopSeq)
    {
       LOGG_PRINT(LOG_INFO_E, NULL,"stop sequence %p, setting seqCurrent to null\n",seqInfoP);
       seqInfoP->seqCurrent = NULL;
       return;
    }
    OS_LYRG_lockMutex(&(triggerInfoP->mutex));
   if(seqInfoP->seqCurrent==NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"seqInfoP->seqCurrent==NULL \n",seqInfoP);
      OS_LYRG_unlockMutex(&(triggerInfoP->mutex));
      return;
   }
    currentDelay = seqInfoP->seqCurrent->data.delay;
    seqInfoP->seqCurrent->data.triggerCounter++;
    triggerCountCalc = triggerInfoP->lcmForTimer / triggerInfoP->sensorsInfo[seqInfoP->seqCurrent->data.sensorSelect]->sensorCfg.fps;
    if (triggerCountCalc == seqInfoP->seqCurrent->data.triggerCounter)//check the ratio
    {
       gpioBitmap |= seqInfoP->seqCurrent->data.gpioBitMap;
       pulseDelay = MAX(pulseDelay, seqInfoP->seqCurrent->data.seqPulseDelay);
       seqInfoP->seqCurrent->data.triggerCounter = 0;
    }

    seqInfoP->seqCurrent = seqInfoP->seqCurrent->next;

    while((seqInfoP->seqCurrent) && (seqInfoP->seqCurrent->data.delay < (currentDelay + pulseDelay)))
    {
       seqInfoP->seqCurrent->data.triggerCounter++;
       triggerCountCalc = triggerInfoP->lcmForTimer / triggerInfoP->sensorsInfo[seqInfoP->seqCurrent->data.sensorSelect]->sensorCfg.fps;
       if (triggerCountCalc == seqInfoP->seqCurrent->data.triggerCounter)
       {
          gpioBitmap |= seqInfoP->seqCurrent->data.gpioBitMap;
          seqInfoP->seqCurrent->data.triggerCounter = 0;
       }
       seqInfoP->seqCurrent = seqInfoP->seqCurrent->next;
    }

    if (gpioBitmap)
    {
       retCode = TRIGGER_MNGRP_toggleGpio(gpioBitmap, pulseDelay, triggerInfoP->pauseTrigger);
       pulseDelayCompensate = pulseDelay;
    }
    if (seqInfoP->seqCurrent)
    {
       //start the timer
       seqInfoP->sequenceTimer.firstExpiryNsec = 1000 * (seqInfoP->seqCurrent->data.delay - currentDelay - pulseDelayCompensate);
       seqInfoP->sequenceTimer.firstExpirySec = 0;
       seqInfoP->sequenceTimer.intervalExpiryNsec = 0;
       seqInfoP->sequenceTimer.intervalExpirySec = 0;
       retCode = OS_LYRG_setTimer(&seqInfoP->sequenceTimer);
    }
#else
   if (triggerInfoP->stopSeq)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"stop sequence, setting seqCurrent to null\n");
      triggerInfoP->seqCurrent = NULL;
      return;
   }
   OS_LYRG_lockMutex(&(triggerInfoP->mutex));
   currentDelay = triggerInfoP->seqCurrent->data.delay;
   triggerInfoP->seqCurrent->data.triggerCounter++;
   triggerCountCalc = triggerInfoP->lcmForTimer / triggerInfoP->sensorsInfo[triggerInfoP->seqCurrent->data.sensorSelect]->sensorCfg.fps;
   if (triggerCountCalc == triggerInfoP->seqCurrent->data.triggerCounter)//check the ratio
   {
      gpioBitmap |= triggerInfoP->seqCurrent->data.gpioBitMap;
      pulseDelay = MAX(pulseDelay, triggerInfoP->seqCurrent->data.seqPulseDelay);
      triggerInfoP->seqCurrent->data.triggerCounter = 0;
   }

   triggerInfoP->seqCurrent = triggerInfoP->seqCurrent->next;
   while((triggerInfoP->seqCurrent) && (triggerInfoP->seqCurrent->data.delay < (currentDelay + pulseDelay/*pulseDelayCompensate*/)))
   {
      triggerInfoP->seqCurrent->data.triggerCounter++;
      triggerCountCalc = triggerInfoP->lcmForTimer / triggerInfoP->sensorsInfo[triggerInfoP->seqCurrent->data.sensorSelect]->sensorCfg.fps;
      if (triggerCountCalc == triggerInfoP->seqCurrent->data.triggerCounter)
      {
         gpioBitmap |= triggerInfoP->seqCurrent->data.gpioBitMap;
         triggerInfoP->seqCurrent->data.triggerCounter = 0;
      }
      triggerInfoP->seqCurrent = triggerInfoP->seqCurrent->next;
   }
   if (gpioBitmap)
   {
      retCode = TRIGGER_MNGRP_toggleGpio(gpioBitmap, pulseDelay, triggerInfoP->pauseTrigger);
      pulseDelayCompensate = pulseDelay;
   }
   if (triggerInfoP->seqCurrent)
   {
      //start the timer
      triggerInfoP->sequenceTimer.firstExpiryNsec = 1000 * (triggerInfoP->seqCurrent->data.delay - currentDelay - pulseDelayCompensate);
      triggerInfoP->sequenceTimer.firstExpirySec = 0;
      triggerInfoP->sequenceTimer.intervalExpiryNsec = 0;
      triggerInfoP->sequenceTimer.intervalExpirySec = 0;
      retCode = OS_LYRG_setTimer(&triggerInfoP->sequenceTimer);
   }
#endif
   OS_LYRG_unlockMutex(&(triggerInfoP->mutex));
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "fail in trigger src %d\n",triggerInfoP->anchorInfo.source);
   }
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_sequenceStart
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
#ifdef DYNAMIC_EXPOSURE_DELAY
/* function to swap data of two nodes a and b*/
void TRIGGER_MNGRP_swapList(TRIGGER_MNGRP_seqNode *a, TRIGGER_MNGRP_seqNode *b)
{
    TRIGGER_MNGRP_seqInfo temp = a->data;
    a->data = b->data;
    b->data = temp;
}

static void TRIGGER_MNGRP_sortList(TRIGGER_MNGRP_seqNode *headP)
{
   TRIGGER_MNGRP_seqNode *ptr1;
   TRIGGER_MNGRP_seqNode *lptr = NULL;
   int swapped;

   /* Checking for empty list */
   if (headP == NULL)
       return;

   do
   {
       swapped = 0;
       ptr1 = headP;

       while (ptr1->next != lptr)
       {
           if (ptr1->data.delay > ptr1->next->data.delay)
           {

               TRIGGER_MNGRP_swapList(ptr1, ptr1->next);
               swapped = 1;
           }
           ptr1 = ptr1->next;
       }
       lptr = ptr1;
   }
   while (swapped);

}
#endif

static void    TRIGGER_MNGRP_sequenceStart(void *argP)
{
   ERRG_codeE                 retCode      = SENSORS_MNGR__RET_SUCCESS;
   TRIGGER_MNGRP_triggerInfo *triggerInfoP = (TRIGGER_MNGRP_triggerInfo*)argP;
   UINT64                     gpioBitmap = 0;
   UINT32                     triggerCountCalc, pulseDelay=0, pulseDelayCompensate=0;
#ifdef DYNAMIC_EXPOSURE_DELAY
   UINT32 maxExp = 0;
   TRIGGER_MNGRP_seqNode *tempP;
#endif

   if (triggerInfoP->stopSeq)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"stop sequence, setting seqCurrent to null\n");
#ifndef DOUBLE_SEQ_DB
      triggerInfoP->seqCurrent = NULL; //should be NULL at this point, but just to make sure
#endif
      return;
   }

   OS_LYRG_lockMutex(&(triggerInfoP->mutex));

#ifdef DOUBLE_SEQ_DB
   //generate sequence db, and pass this db to step
   TRIGGER_MNGRP_sequenceInfo *seqInfoP = &triggerInfoP->seqDb[triggerInfoP->currentSeqDb];
   triggerInfoP->currentSeqDb ^= 1; //toggle between 0&1 for next start invocation

   //sequence start, init the step
   seqInfoP->seqCurrent = triggerInfoP->seq;
#ifdef DYNAMIC_EXPOSURE_DELAY
   //sensor control changes the exposure, which modifies the readout location.
   //we use the trigger mode to keep the readout at the same spot, by adjusting the
   //delay according to the exposure
   //first find the max exposure. delay = origdelay + maxexposure - sensorexposure
   //every activation, modify the delays in the list, and resort it
   tempP = seqInfoP->seqCurrent;
   while(tempP)
   {
      if (triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->sensorCfg.isTriggerSupported)
      {
         maxExp = MAX(maxExp, triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->exposure[ALTG_getThisFrameMode(triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->altH)]);
      }
      tempP = tempP->next;
   }
   tempP = seqInfoP->seqCurrent;
   while(tempP)
   {
      if (triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->sensorCfg.isTriggerSupported)
      {
         tempP->data.delay = triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->sensorCfg.triggerDelay + (maxExp - triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->exposure[ALTG_getThisFrameMode(triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->altH)]);
         LOGG_PRINT(LOG_DEBUG_E, NULL, "%p delay = %d, maxExp %d, exp %d\n",tempP,tempP->data.delay,maxExp,triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->exposure[ALTG_getThisFrameMode(triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->altH)]);
      }
      tempP = tempP->next;
   }
   TRIGGER_MNGRP_sortList(seqInfoP->seqCurrent);
#endif
   while ((seqInfoP->seqCurrent) && (!seqInfoP->seqCurrent->data.delay))//toggle all delay 0, but check their counter ratio
   {
      seqInfoP->seqCurrent->data.triggerCounter++;

      triggerCountCalc = triggerInfoP->lcmForTimer / triggerInfoP->sensorsInfo[seqInfoP->seqCurrent->data.sensorSelect]->sensorCfg.fps;
      if (triggerCountCalc == seqInfoP->seqCurrent->data.triggerCounter)
      {
         gpioBitmap |= seqInfoP->seqCurrent->data.gpioBitMap;
         pulseDelay = MAX(pulseDelay, seqInfoP->seqCurrent->data.seqPulseDelay);
         seqInfoP->seqCurrent->data.triggerCounter = 0;
      }
      seqInfoP->seqCurrent = seqInfoP->seqCurrent->next;
   }
   if (gpioBitmap)
   {
      retCode = TRIGGER_MNGRP_toggleGpio(gpioBitmap, pulseDelay, triggerInfoP->pauseTrigger);
      pulseDelayCompensate = pulseDelay;
   }

   if (seqInfoP->seqCurrent)
   {
      //start the timer
      seqInfoP->sequenceTimer.firstExpiryNsec = 1000 * (seqInfoP->seqCurrent->data.delay - pulseDelayCompensate);
      seqInfoP->sequenceTimer.firstExpirySec = 0;
      seqInfoP->sequenceTimer.intervalExpiryNsec = 0;
      seqInfoP->sequenceTimer.intervalExpirySec = 0;
      retCode = OS_LYRG_setTimer(&seqInfoP->sequenceTimer);
   }
#else
   //sequence start, init the step
   triggerInfoP->seqCurrent = triggerInfoP->seq;

   while ((triggerInfoP->seqCurrent) && (!triggerInfoP->seqCurrent->data.delay))//toggle all delay 0, but check their counter ratio
   {
      triggerInfoP->seqCurrent->data.triggerCounter++;

      triggerCountCalc = triggerInfoP->lcmForTimer / triggerInfoP->sensorsInfo[triggerInfoP->seqCurrent->data.sensorSelect]->sensorCfg.fps;
      if (triggerCountCalc == triggerInfoP->seqCurrent->data.triggerCounter)
      {
         gpioBitmap |= triggerInfoP->seqCurrent->data.gpioBitMap;
         pulseDelay = MAX(pulseDelay, triggerInfoP->seqCurrent->data.seqPulseDelay);
         triggerInfoP->seqCurrent->data.triggerCounter = 0;
      }
      triggerInfoP->seqCurrent = triggerInfoP->seqCurrent->next;
   }
   if (gpioBitmap)
   {
      retCode = TRIGGER_MNGRP_toggleGpio(gpioBitmap, pulseDelay, triggerInfoP->pauseTrigger);
      pulseDelayCompensate = pulseDelay;
   }
    
   if (triggerInfoP->seqCurrent)
   {
      //start the timer
      triggerInfoP->sequenceTimer.firstExpiryNsec = 1000 * (triggerInfoP->seqCurrent->data.delay - pulseDelayCompensate);
      triggerInfoP->sequenceTimer.firstExpirySec = 0;
      triggerInfoP->sequenceTimer.intervalExpiryNsec = 0;
      triggerInfoP->sequenceTimer.intervalExpirySec = 0;
      retCode = OS_LYRG_setTimer(&triggerInfoP->sequenceTimer);
   }
#endif
   OS_LYRG_unlockMutex(&(triggerInfoP->mutex));

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "fail in trigger src %d\n",triggerInfoP->anchorInfo.source);
   }
}


static void TRIGGER_MNGRP_sluInterCb(UINT64 ts, UINT32 slu, void *argP)
{
   TRIGGER_MNGRP_triggerInfo *triggerInfoP = (TRIGGER_MNGRP_triggerInfo*)argP;
   (void)ts;
   if ((slu + INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU0_E) == triggerInfoP->anchorInfo.source)
   {
      TRIGGER_MNGRP_sequenceStart(argP);
   }
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_unregisterTuningDelayCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_unregisterTuningDelayCb (SENSORS_MNGRG_sensorInfoT *sensorInfoP)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 sluInst;

   retCode = IAE_DRVG_unregisterSnsrIsrCb(sensorInfoP->isrForTuningDelay);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"failed to unregister\n");
      return retCode;
   }
   SEQ_MNGRG_getSluInstBySenInst(sensorInfoP->sensorCfg.sensorSelect, &sluInst);
   sensorInfoP->isrForTuningDelay = NULL;
   sluRegCbBitmap &= ~(1 << sluInst);

   LOGG_PRINT(LOG_DEBUG_E, NULL, "unregister tuning delay cb sensor %d slu %d\n",sensorInfoP->sensorCfg.sensorModel, sluInst);

   return retCode;
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_unregisterTuningDelayAllCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_unregisterTuningDelayAllCb ()
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 i;

   for (i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS;i++)
   {
      if ((sensorAutoList[i]) && (sensorAutoList[i]->isrForTuningDelay != NULL))
      {
         retCode = TRIGGER_MNGRP_unregisterTuningDelayCb(sensorAutoList[i]);
         if (ERRG_FAILED(retCode))
         {
            return retCode;
         }
      }
   }
   return retCode;
}

static ERRG_codeE TRIGGER_MNGRP_matchIndexes(UINT64 *anchorTimeList,UINT64 *timeList,UINT32 trigIndx, UINT32 *matchInd)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32 tmpInd = 0;

   if ((anchorTimeList[0] == 0) || (timeList[0] == 0))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "time stamp tables are wrong (zero)\n");
      return TRIGGER_MNGR__TS_TABLE_ZERO;
   }

   while (timeList[trigIndx] > anchorTimeList[tmpInd])
   {
      tmpInd++;
      if (tmpInd == TRIGGER_MNGRP_SAMPLING_BUF_SIZE)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "time stamp tables are wrong, end of table, can't find match\n");
         assert(0);
         return TRIGGER_MNGR__ERR_UNEXPECTED;
      }
   }
   if (tmpInd == 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "time stamp tables are wrong\n");
      assert(0);
      return TRIGGER_MNGR__ERR_UNEXPECTED;
   }
   *matchInd = tmpInd - 1;
   return retCode;
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_calcdelay
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_calcDelay (TRIGGER_MNGRP_autoModeParams *triggerMapping)
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 tmpSensorParticipant, sluInst, senInst;
   UINT32                 matchIndex = TRIGGER_MNGRP_INDEX_TO_CALC_DELAY;
   UINT64                 *anchorTimeList, *timeList;

#ifdef TRIGGER_MNGRP_DEBUG_PRINT
   UINT32                 i, j;
   LOGG_PRINT(LOG_INFO_E, NULL,"time stamps tables:\n");
   for(j=0;j<TRIGGER_MNGRP_NUM_OF_SLU;j++)
   {
      if (sluTsTable[j][0] == 0)
         continue;
      for (i=0; i<20;i++)
      {
         LOGG_PRINT(LOG_INFO_E, NULL,"slu%d %llu\n",j,sluTsTable[j][i]);
      }
   }
#endif
   //get anchor time list
   retCode = SEQ_MNGRG_getSluInstBySenInst(triggerMapping->anchorSensorSelect, &sluInst);
   if (ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to get anchor (instance %d) SLU instance\n",triggerMapping->anchorSensorSelect);
      return retCode;
   }
   anchorTimeList = (UINT64*)&sluTsTable[sluInst];

   //get participants time list
   tmpSensorParticipant = triggerMapping->sensorsParticipant;
   while(tmpSensorParticipant)
   {
      senInst = getNextSelect(&tmpSensorParticipant);
      if (senInst == triggerMapping->anchorSensorSelect)
         continue;
      retCode = SEQ_MNGRG_getSluInstBySenInst(senInst, &sluInst);
      if (ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "failed to get participant (instance %d) SLU instance\n",tmpSensorParticipant);
         return retCode;
      }
      timeList = (UINT64*)&sluTsTable[sluInst];
      retCode = TRIGGER_MNGRP_matchIndexes(anchorTimeList, timeList, TRIGGER_MNGRP_INDEX_TO_CALC_DELAY, &matchIndex);
      if (retCode == TRIGGER_MNGR__TS_TABLE_ZERO)
      {
         sensorAutoList[senInst]->sensorCfg.triggerDelay = 0;
         LOGG_PRINT(LOG_ERROR_E, NULL, "failed to calculate delay for sensor %d, continue with delay 0\n",senInst);
         continue;// if did not collect TS for this SLU, mark delay 0 and continue
      }
      if (ERRG_FAILED(retCode))
      {
         return retCode;
      }
      LOGG_PRINT(LOG_DEBUG_E, NULL,"dif for delay anchor with sen %d is %llu(ind %d) - %llu(ind %d) = %llu\n",senInst,timeList[TRIGGER_MNGRP_INDEX_TO_CALC_DELAY],TRIGGER_MNGRP_INDEX_TO_CALC_DELAY, anchorTimeList[matchIndex],matchIndex, timeList[TRIGGER_MNGRP_INDEX_TO_CALC_DELAY]-anchorTimeList[matchIndex]);
      LOGG_PRINT(LOG_INFO_E, NULL, "calculated delay for sensor select %d is %d usec\n",senInst,(abs(timeList[TRIGGER_MNGRP_INDEX_TO_CALC_DELAY] - anchorTimeList[matchIndex + 1]))/1000);

      //fill dealy to sensor
      sensorAutoList[senInst]->sensorCfg.triggerDelay = (abs(timeList[TRIGGER_MNGRP_INDEX_TO_CALC_DELAY] - anchorTimeList[matchIndex + 1]))/1000;
   }

   retCode = TRIGGER_MNGRP_stopAllAutoTriggers();
   if (ERRG_FAILED(retCode))
   {
      return retCode;
   }

   retCode = TRIGGER_MNGRP_startAllTriggers();
   if (ERRG_FAILED(retCode))
   {
      return retCode;
   }

   //TODO - check if needed
   memset(&sluTsTable,0,sizeof(UINT64) * TRIGGER_MNGRP_SAMPLING_BUF_SIZE * TRIGGER_MNGRP_NUM_OF_SLU);
   memset(&sluTsCnt,0,sizeof(UINT32) * TRIGGER_MNGRP_NUM_OF_SLU);

   return retCode;

}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_sluTuningInterCb
*
*  Description: gather statistics about this sensor, no need for mutex protection, function
*                    called from interrupt thread
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: Interrupt thread
*
****************************************************************************/
static void TRIGGER_MNGRP_sluTuningInterCb(UINT64 ts, UINT32 slu, void *argP)
{
   int                             ret;
   UINT32                          sluInst;
   SENSORS_MNGRG_sensorInfoT       *sensorInfo = (SENSORS_MNGRG_sensorInfoT *)argP;
   TRIGGER_MNGRP_msgQParamsT       msg;

   SEQ_MNGRG_getSluInstBySenInst(sensorInfo->sensorCfg.sensorSelect, &sluInst);
   if(slu == sluInst)
   {
      sluTsTable[slu][sluTsCnt[slu]] = ts;
      sluTsCnt[slu]++;
      if (sluTsCnt[slu] == TRIGGER_MNGRP_NUM_OF_SAMPLING)// >= ?
      {
         sluDoneSampling |= (1 << slu);
      }
      if (sluTsCnt[slu] >= TRIGGER_MNGRP_SAMPLING_BUF_SIZE)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "error while collect timestamps\n");

         msg.msgCode = TRIGGER_MNGR_DONE_COLLECT_TS_E;
         ret = OS_LYRG_sendMsg(&TRIGGER_MNGRP_info.msgQue, (UINT8*)&msg, sizeof(TRIGGER_MNGRP_msgQParamsT));
         if(ret != SUCCESS_E)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to send msg to Trigger manager thread!\n");
         }
         //assert(0);
      }

      if (sluDoneSampling == sluRegCbBitmap)//if all done
      {
#if 1
         sluDoneSampling = 0;

         msg.msgCode = TRIGGER_MNGR_DONE_COLLECT_TS_E;
         ret = OS_LYRG_sendMsg(&TRIGGER_MNGRP_info.msgQue, (UINT8*)&msg, sizeof(TRIGGER_MNGRP_msgQParamsT));
         if(ret != SUCCESS_E)
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "failed to send msg to Trigger manager thread!\n");
         }

#else
         //unrgister all registered
         sluDoneSampling = 0;
         retCode = TRIGGER_MNGRP_unregisterTuningDelayAllCb();
         if (ERRG_FAILED(retCode))
         {
            return;
         }
         retCode = TRIGGER_MNGRP_calcDelay(&bestTriggerOrg);
         if (ERRG_FAILED(retCode))
         {
            return;
         }
#endif
      }
   }
   return;
}

static void TRIGGER_MNGRP_sortedInsert(TRIGGER_MNGRP_seqNode** head_ref, TRIGGER_MNGRP_seqNode* new_node)
{
    TRIGGER_MNGRP_seqNode* current;
    /* Special case for the head end */
    if (((*head_ref)->data).delay >= new_node->data.delay)
    {
        new_node->next = *head_ref;
        *head_ref = new_node;
    }
    else
    {
        /* Locate the node before the point of insertion */
        current = *head_ref;
        while ((current->next != NULL) &&
            (current->next->data.delay < new_node->data.delay))
        {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}

void TRIGGER_MNGRP_print_list(TRIGGER_MNGRP_seqNode * head)
{
    TRIGGER_MNGRP_seqNode * current = head;

   while (current != NULL)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"linked list %p delay %d gpio %llx ratio %d next %p\n",current, current->data.delay,current->data.gpioBitMap, current->data.ratioFromTrigger,current->next);
      current = current->next;
   }
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_findTriggerSrcIndex
*
*  Description: run on TRIGGER_MNGRP_triggerInfoTbl and search if the source is exist in the table.
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_findTriggerSrcIndex( INU_DEFSG_sensorTriggerSrcE src, UINT32 *ind)
{
   UINT32 i;

   for (i=0;i<TRIGGER_MNGRP_SUPPORTED_TRIGGER;i++)
   {
      if (TRIGGER_MNGRP_triggerInfoTbl[i].anchorInfo.source == src)
      {
         *ind = i;
         return TRIGGER_MNGR__RET_SUCCESS;
      }
   }
   return TRIGGER_MNGR__TRIGGER_SRC_NOT_FOUND;
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_addTriggerToSeq
*
*  Description:  TODO future optimization - unify nodes with the same delay and ratio
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static void TRIGGER_MNGRP_addTriggerToSeq  (TRIGGER_MNGRP_triggerInfo *triggerInfoP, INU_DEFSG_senSelectE sensorSel)
{
   struct TRIGGER_MNGRP_seqNode *tmpNode = NULL;
   SENSORS_MNGRG_sensorInfoT    *sensorInfoP = triggerInfoP->sensorsInfo[sensorSel];
   UINT32                       ratioFromAnchor;

   ratioFromAnchor = triggerInfoP->anchorInfo.anchorSensorInfo->sensorCfg.fps / sensorInfoP->sensorCfg.fps;
   LOGG_PRINT(LOG_INFO_E, NULL, "sensor %d, ratio from anchor 1 : %d, anchor fps %d sen fps %d\n",sensorInfoP->sensorCfg.sensorSelect,ratioFromAnchor,triggerInfoP->anchorInfo.anchorSensorInfo->sensorCfg.fps,sensorInfoP->sensorCfg.fps);

   if (triggerInfoP->seq == NULL)//first node
   {
      triggerInfoP->seq = malloc(sizeof(TRIGGER_MNGRP_seqNode));
      memset(triggerInfoP->seq,0,sizeof(TRIGGER_MNGRP_seqNode));
      triggerInfoP->seq->data.delay = sensorInfoP->sensorCfg.triggerDelay;
      triggerInfoP->seq->data.gpioBitMap |= (((UINT64)1) << sensorInfoP->sensorCfg.fsinGpio);
      triggerInfoP->seq->data.seqPulseDelay = sensorInfoP->pulseTime;
      triggerInfoP->seq->data.sensorSelect = sensorSel;
      if (triggerInfoP->anchorInfo.anchorSensorInfo->sensorCfg.fps < sensorInfoP->sensorCfg.fps)
      {
         assert(0);
      }
      triggerInfoP->seq->data.ratioFromTrigger = ratioFromAnchor;
      triggerInfoP->seq->data.triggerCounter = 0;
      triggerInfoP->seq->next = NULL;
   }
   else//run on linked list and search the position by delay
   {
      //delay does not exist, add new node to the list
      tmpNode = malloc(sizeof(TRIGGER_MNGRP_seqNode));
      memset(tmpNode,0,sizeof(TRIGGER_MNGRP_seqNode));
      tmpNode->data.delay = sensorInfoP->sensorCfg.triggerDelay;
      tmpNode->data.gpioBitMap |= (((UINT64)1) << sensorInfoP->sensorCfg.fsinGpio);
      tmpNode->data.seqPulseDelay = sensorInfoP->pulseTime;
      tmpNode->data.sensorSelect = sensorSel;
      tmpNode->data.triggerCounter = 0;
      tmpNode->data.ratioFromTrigger = ratioFromAnchor;
      tmpNode->next = triggerInfoP->seq;
      TRIGGER_MNGRP_sortedInsert(&triggerInfoP->seq,tmpNode);
   }
   TRIGGER_MNGRP_print_list(triggerInfoP->seq);
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_deleteTriggerFromSeq
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static void TRIGGER_MNGRP_deleteTriggerFromSeq  (TRIGGER_MNGRP_triggerInfo *triggerInfoP, INU_DEFSG_senSelectE sensorSel)
{
   TRIGGER_MNGRP_seqNode     *currentNode;
   TRIGGER_MNGRP_seqNode     *previousNode = NULL;
   TRIGGER_MNGRP_seqNode     *next_node    = NULL;

   currentNode = triggerInfoP->seq;

   OS_LYRG_lockMutex(&(triggerInfoP->mutex));
   while(currentNode != NULL)
   {
      if (currentNode->data.sensorSelect == sensorSel)
      {
         //remove node from seq
         if (previousNode == NULL)//first node. remove head
         {
            next_node = currentNode->next;
            free(currentNode);
            //update new head
            triggerInfoP->seq = next_node;
         }
         else
         {
            previousNode->next = currentNode->next;
            free(currentNode);
         }
         break;
      }
      previousNode = currentNode;
      currentNode = currentNode->next;
   }
   OS_LYRG_unlockMutex(&(triggerInfoP->mutex));
   //TRIGGER_MNGRP_print_list(triggerInfoP->seq);
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_resetTriggerCounter
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static void TRIGGER_MNGRP_resetTriggerCounter  (TRIGGER_MNGRP_triggerInfo *triggerInfoP)
{
   struct TRIGGER_MNGRP_seqNode *tmpNode = NULL;
   tmpNode = triggerInfoP->seq;

   while(tmpNode)
   {
      tmpNode->data.triggerCounter = 0;
      tmpNode = tmpNode->next;
   }
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_resetTriggerCounter
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_registerCbTuningDelay ()
{
   ERRG_codeE             retCode      = SENSORS_MNGR__RET_SUCCESS;
   UINT32                 i, sluInst;

   memset(&sluTsTable,0,sizeof(UINT64) * TRIGGER_MNGRP_SAMPLING_BUF_SIZE * TRIGGER_MNGRP_NUM_OF_SLU);
   memset(&sluTsCnt,0,sizeof(UINT32) * TRIGGER_MNGRP_NUM_OF_SLU);

   for (i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS;i++)
   {
      if (sensorAutoList[i])
      {
         SEQ_MNGRG_getSluInstBySenInst(sensorAutoList[i]->sensorCfg.sensorSelect, &sluInst);
         sluRegCbBitmap |= (1 << sluInst);
         //TODO - register only one calback
         retCode = IAE_DRVG_registerSnsrIsrCb(TRIGGER_MNGRP_sluTuningInterCb, sensorAutoList[i], &sensorAutoList[i]->isrForTuningDelay);
         if(ERRG_FAILED(retCode))
         {
            return retCode;
         }
         LOGG_PRINT(LOG_DEBUG_E, NULL, "sensor model %d register to tuing delay cb\n",sensorAutoList[i]->sensorCfg.sensorModel);
      }
   }

   return retCode;
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_startTimer
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static ERRG_codeE TRIGGER_MNGRP_startTimer(OS_LYRG_timerT *timer, UINT32 fps, void *argP)
{
   ERRG_codeE                 retCode        = SENSORS_MNGR__RET_SUCCESS;
   UINT32                     sec,nsec;

   if (fps != 0)
   {
      //create the trigger timer
      timer->funcCb = TRIGGER_MNGRP_sequenceStart;
      timer->argP   = argP;
      retCode = OS_LYRG_createTimer(timer);
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create trigger timer\n");
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "create trigger timer.\n");
      }

      if (fps == 1)
      {
         sec = 1;
         nsec = 0;
      }
      else
      {
         sec = 0;
         nsec = 1000000000 / fps;
      }

      //start the timer
      timer->firstExpiryNsec = 500000000;
      timer->firstExpirySec = 0;
      timer->intervalExpiryNsec = nsec;
      timer->intervalExpirySec = sec;
      retCode = OS_LYRG_setTimer(timer);
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "failed to start trigger timer\n");
      }
      else
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "start trigger timer %d fps\n", fps);
      }
   }

   return retCode;
}

/****************************************************************************
*
*  Function Name: TRIGGER_MNGRP_resetTriggerCounter
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
static void TRIGGER_MNGRP_resetAllTriggerDelay()
{
   UINT32     i;

   for (i=0;i<INU_DEFSG_NUM_OF_INPUT_SENSORS;i++)
   {
      if(sensorAutoList[i])
      {
         sensorAutoList[i]->sensorCfg.triggerDelay = 0;
      }
   }

   return;
}

/***************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/
/****************************************************************************
*
*  Function Name: TRIGGER_MNGRG_startTrigger
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
ERRG_codeE TRIGGER_MNGRG_manageStartTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo, trigger_mngrH *triggerH)
{
   ERRG_codeE                          ret = TRIGGER_MNGR__RET_SUCCESS;

   if (sensorInfo->sensorCfg.triggerSrc == INU_DEFSG_SENSOR_TRIGGER_AUTO_E)
   {
      ret = TRIGGER_MNGRP_stopAllAutoTriggers();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }

      sensorAutoList[sensorInfo->sensorCfg.sensorSelect] = sensorInfo;
      ret = TRIGGER_MNGRP_autoReorganization(&bestTriggerOrg);
      if (ERRG_FAILED(ret))
      {
         return ret;
      }

      //tuning delay
      ret = TRIGGER_MNGRP_unregisterTuningDelayAllCb();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
      ret = TRIGGER_MNGRP_registerCbTuningDelay();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
      TRIGGER_MNGRP_resetAllTriggerDelay();

      ret = TRIGGER_MNGRP_startAllTriggers();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
   }
   else
   {
      TRIGGER_MNGRG_startTrigger(sensorInfo,triggerH);
   }

   return ret;
}

/****************************************************************************
   *
   *  Function Name: TRIGGER_MNGRG_stopTrigger
   *
   *  Description:
   *
   *  Inputs:
   *
   *  Outputs: none
   *
   *  Returns:
   *
   *  Context: sensors manager
   *
   ****************************************************************************/
ERRG_codeE TRIGGER_MNGRG_manageStopTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo)
{
   ERRG_codeE                          ret = TRIGGER_MNGR__RET_SUCCESS;

   if (sensorInfo->sensorCfg.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
   {
      ret = TRIGGER_MNGRG_stopTrigger(sensorInfo);
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
   }

   if (sensorAutoList[sensorInfo->sensorCfg.sensorSelect] == sensorInfo)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "sensor model %d back to auto src\n",sensorInfo->sensorCfg.sensorModel);
      sensorInfo->sensorCfg.triggerSrc = INU_DEFSG_SENSOR_TRIGGER_AUTO_E;
      sensorInfo->sensorCfg.triggerDelay = 0;
      sensorAutoList[sensorInfo->sensorCfg.sensorSelect] = NULL;

      ret = TRIGGER_MNGRP_stopAllAutoTriggers();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
      ret = TRIGGER_MNGRP_autoReorganization(&bestTriggerOrg);
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
      ret = TRIGGER_MNGRP_unregisterTuningDelayAllCb();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
      ret = TRIGGER_MNGRP_registerCbTuningDelay();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
      TRIGGER_MNGRP_resetAllTriggerDelay();

      ret = TRIGGER_MNGRP_startAllTriggers();
      if (ERRG_FAILED(ret))
      {
         return ret;
      }
   }

   return ret;
}

/*
 * Function Name: TRIGGER_MNGRG_changeFps
 * Description:
 *        Changing the fps for the trigger source.
 *        The 1st edition is simple, not considering the trigger source(assume sw timer is used)
 *        , and only changes the sensorCfg.fps to let the timer callback function to calculate
 *        the skip counters.
 *        The 1st edition also assume that same trigger source will use same sensors.
 *        This interface is reserved for sensor synchronization design
 *        !!!The 1st edition is only to test changing FPS dynamically!!!
 *        So the fps arg must be a number by which lcmForTimer can be divisible
 *        WARNING: This interface has many constrains now,
 *                 And will be modified after sensors' synchronization
 *                 code is ready.
 * Input: triggerH - TRIGGER_MNGRP_triggerInfo handle
 *        fps      - targetFps to change
 * Returns:  TRIGGER_MNGR__RET_SUCCESS if no error
 * Context: sensors manager
 */
ERRG_codeE TRIGGER_MNGRG_changeFps(trigger_mngrH triggerH, UINT32 fps)
{
   ERRG_codeE ret = TRIGGER_MNGR__RET_SUCCESS;
   TRIGGER_MNGRP_triggerInfo *triggerInfoP = (TRIGGER_MNGRP_triggerInfo *)triggerH;
   TRIGGER_MNGRP_seqNode *tempP;

   OS_LYRG_lockMutex(&(triggerInfoP->mutex));
   triggerInfoP->pauseTrigger = 1;
   tempP = triggerInfoP->seq;
   while (tempP)
   {
      triggerInfoP->sensorsInfo[tempP->data.sensorSelect]->sensorCfg.fps = fps;
      tempP = tempP->next;
   }

   TRIGGER_MNGRP_resetTriggerCounter(triggerInfoP);
   triggerInfoP->pauseTrigger = 0;
   OS_LYRG_unlockMutex(&(triggerInfoP->mutex));
   return ret;
}
#define PREVENT_FSG_TRIG_SLEEP_US (100)
/*! Warning !
 The FSG must be configured by the HW XML , there is also an assumption that FSG counter 0 connects to FSG trigger output 0, FSG counter 1 connects to FSG trigger output 1 etc.
 This might not be valid depending on your project!*/
static ERRG_codeE TRIGGER_MNGR_updateGMERegisters(INU_DEFSG_sensorTriggerSrcE triggerSrc, bool enable)
{
   GME_DRVG_triggerMode triggerMode = TRIGGER_MANAGER;
   IAE_DRVG_fsgCounterNumE GMERegister = IAE_DRVG_FSG_CNT_0_E;
   /*We should make sure we don't falsely trigger a sensor by switching to GPIO muxing before enabling a sw trigger
    Here's an explanation why:
    1. As soon as a sw trigger is sent, the FSG output will be set high
    2. This causes a trigger to go both sensors
    3. At this point in time only one of the sensors might be active due to how the sensors are configured by the graph builder
    4. Therefore one sensor will start outputting MIPI data but the other won't and therefore when both SLUs are enabled the interleaving might not work

   To prevent this from happening, I disable the trigger output and switch to GPIO muxing whilst issueing the SW trigger.
   Once this is done I switch back to FSG muxing and then enable the FSG trigger output
    */
   IAE_DRVG_FSG_sensEnable();
    switch(triggerSrc)
  {
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_0:
      {
         GMERegister = IAE_DRVG_FSG_CNT_0_E;
         if(enable)
         {
            IAE_DRVG_disableFTRIGOutput(GMERegister);
            triggerMode = FSG_BLOCK;
            /*Switch to GPIO mode whilst starting the SW trigger so that we don't get an instantanous trigger pulse*/
            GME_DRVG_modifyFTRIGMode(GMERegister,TRIGGER_MANAGER);
            usleep(PREVENT_FSG_TRIG_SLEEP_US);
            IAE_DRVG_FSG_swTrigger0();
            usleep(PREVENT_FSG_TRIG_SLEEP_US);
            IAE_DRVG_enableFTRIGOutput(GMERegister);
            HELSINKI_enableToFTrigger();
         }
         else
         {
            IAE_DRVG_disableFTRIGOutput(GMERegister);
         }
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_1:
      {
         GMERegister = IAE_DRVG_FSG_CNT_1_E;
         if(enable)
         {
            IAE_DRVG_disableFTRIGOutput(GMERegister);
            triggerMode = FSG_BLOCK;
            /*Switch to GPIO mode whilst starting the SW trigger so that we don't get an instantanous trigger pulse*/
            GME_DRVG_modifyFTRIGMode(GMERegister,TRIGGER_MANAGER);
            usleep(PREVENT_FSG_TRIG_SLEEP_US);
            IAE_DRVG_FSG_swTrigger1();
            usleep(PREVENT_FSG_TRIG_SLEEP_US);
            HELSINKI_enableToFTrigger();
            IAE_DRVG_enableFTRIGOutput(GMERegister);

         }
         else
         {
            IAE_DRVG_disableFTRIGOutput(GMERegister);
         }
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_2:
      {
         GMERegister = IAE_DRVG_FSG_CNT_2_E;
         if(enable)
         {
            IAE_DRVG_disableFTRIGOutput(GMERegister);
            triggerMode = FSG_BLOCK;
            /*Switch to GPIO mode whilst starting the SW trigger so that we don't get an instantanous trigger pulse*/
            GME_DRVG_modifyFTRIGMode(GMERegister,TRIGGER_MANAGER);
            usleep(PREVENT_FSG_TRIG_SLEEP_US);
            IAE_DRVG_FSG_swTrigger2();
            usleep(PREVENT_FSG_TRIG_SLEEP_US);
            HELSINKI_enableToFTrigger();
            IAE_DRVG_enableFTRIGOutput(GMERegister);
         }
         else
         {
            IAE_DRVG_disableFTRIGOutput(IAE_DRVG_FSG_CNT_2_E);
         }
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_3:
      {
         GMERegister = IAE_DRVG_FSG_CNT_3_E;
         triggerMode = FSG_BLOCK;
         LOGG_PRINT(LOG_ERROR_E,NULL,"Unsupported auto SW triggering for FSG counter 3 \n");
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_4:
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"Unsupported auto SW triggering for FSG counter 4 \n");
         GMERegister = IAE_DRVG_FSG_CNT_4_E;
         triggerMode = FSG_BLOCK;
      break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_5:
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"Unsupported auto SW triggering for FSG counter 5 \n");
         GMERegister = IAE_DRVG_FSG_CNT_5_E;
         triggerMode = FSG_BLOCK;
         break;
      }
      /*In these modes we can disable the trigger but we will never automatically enable the trigger
      The enabling of the FSG trigger will come from the SDK/Sandbox or from enabling another stream!*/
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_0:
      {
         GMERegister = IAE_DRVG_FSG_CNT_0_E;
         if(!enable)
            IAE_DRVG_disableFTRIGOutput(GMERegister);
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_1:
      {
         GMERegister = IAE_DRVG_FSG_CNT_1_E;
         if(!enable)
            IAE_DRVG_disableFTRIGOutput(GMERegister);
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_2:
      {
         GMERegister = IAE_DRVG_FSG_CNT_2_E;
         if(!enable)
            IAE_DRVG_disableFTRIGOutput(GMERegister);
         break;
      }
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_3:
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_4:
      case INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_5:
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"Skipping FSG initialization due to the use of  FSG slave mode \n");
         return INU_SENSORSYNC__RET_SUCCESS;
      }
      default:
      {
         triggerMode = TRIGGER_MANAGER;
         break;
      }
  }
  LOGG_PRINT(LOG_DEBUG_E,NULL,"Updating trig mode for GME register %lu to be %lu \n",GMERegister,triggerMode);
  GME_DRVG_modifyFTRIGMode(GMERegister,triggerMode);
  return INU_SENSORSYNC__RET_SUCCESS;

}
ERRG_codeE TRIGGER_MNGR_configureFSG(const INU_DEFSG_sensorTriggerSrcE triggerSrc, bool enable)
{

   ERRG_codeE ret  = TRIGGER_MNGR__RET_SUCCESS;
  
   LOGG_PRINT(LOG_INFO_E,NULL,"Updating GME registers to switch trigger source to be triggerSrc %lu and enabled=%lu) \n",triggerSrc,enable);
   /*In the future we could use the FPS value to configure the FSG registers, at the moment we are using the HW XML to configure the FSG registers
   And we just change the GME muxing plus start the SW trigger when using the FSG triggering mode
   */
   TRIGGER_MNGR_updateGMERegisters(triggerSrc,enable);
  return INU_SENSORSYNC__ERR_NOT_SUPPORTED;
}


/****************************************************************************
*
*  Function Name: TRIGGER_MNGRG_startTrigger
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
ERRG_codeE TRIGGER_MNGRG_startTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo, trigger_mngrH *triggerH)
{
   ERRG_codeE                          ret = TRIGGER_MNGR__RET_SUCCESS;
   UINT32                              i, j, triggerInd;
   TRIGGER_MNGRP_triggerInfo           *triggerInfo;
   SENSORS_MNGRG_sensorInfoT           *anchorInfo;
   UINT32                              senSel;
   UINT32 pwmNum;

   //search in the table if exist entry with same source
   if ((sensorInfo->sensorCfg.triggerSrc >= INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM1_E) &&
          (sensorInfo->sensorCfg.triggerSrc <= INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM3_E))
   {
      pwmNum = sensorInfo->sensorCfg.triggerSrc - INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM1_E;
      if (TRIGGER_MNGR_pwmDb[pwmNum].fps != sensorInfo->sensorCfg.fps)
         LOGG_PRINT(LOG_INFO_E, NULL, "PWM %d different from sensor fps %d using Sensor fps %d\n",
               TRIGGER_MNGR_pwmDb[pwmNum].fps,
               sensorInfo->sensorCfg.fps,sensorInfo->sensorCfg.fps);

      IAE_DRVG_enablePwmDim(pwmNum,
            sensorInfo->sensorCfg.fps,
            TRIGGER_MNGR_pwmDb[pwmNum].usecWidth);
      return ret;
   }
   ret = TRIGGER_MNGRP_findTriggerSrcIndex(sensorInfo->sensorCfg.triggerSrc, &triggerInd);
   if (ERRG_SUCCEEDED(ret))//found
   {
      triggerInfo = &TRIGGER_MNGRP_triggerInfoTbl[triggerInd];
      //add sensor to triggered by src
      OS_LYRG_lockMutex(&triggerInfo->mutex);
      triggerInfo->sensorsInfo[sensorInfo->sensorCfg.sensorSelect] = sensorInfo;

      if (TRIGGER_MNGRP_IS_TIMER_SRC(sensorInfo->sensorCfg.triggerSrc))
      {
         //every new channel, calc LCM, and reset triggerCounter
         triggerInfo->lcmForTimer = TRIGGER_MNGRP_calcLcm(triggerInfo->lcmForTimer, sensorInfo->sensorCfg.fps);
         TRIGGER_MNGRP_resetTriggerCounter(triggerInfo);

         //?
         if(triggerInfo->anchorInfo.anchorSensorInfo->sensorCfg.fps < sensorInfo->sensorCfg.fps)
         {
            triggerInfo->anchorInfo.anchorSensorInfo = sensorInfo;
         }
      }

      TRIGGER_MNGRP_addTriggerToSeq(triggerInfo, sensorInfo->sensorCfg.sensorSelect);
      *triggerH = triggerInfo;
      OS_LYRG_unlockMutex(&triggerInfo->mutex);

      LOGG_PRINT(LOG_INFO_E, NULL, "Sensor %d (model %d) added to triggered by source %d\n",sensorInfo->sensorCfg.sensorSelect,sensorInfo->sensorCfg.sensorModel,sensorInfo->sensorCfg.triggerSrc);

      //return TRIGGER_MNGR__RET_SUCCESS;
   }
   else//not found
   {
      //add new trigger
      for (i=0;i<TRIGGER_MNGRP_SUPPORTED_TRIGGER;i++)
      {
         triggerInfo = &TRIGGER_MNGRP_triggerInfoTbl[i];
         if (triggerInfo->anchorInfo.source == 0)
         {
            OS_LYRG_lockMutex(&(triggerInfo->mutex));
            triggerInfo->anchorInfo.source = sensorInfo->sensorCfg.triggerSrc;
#ifdef DOUBLE_SEQ_DB
            for (j = 0;j < TRIGGER_MNGRP_NUM_SEQ_DB;j++)
            {
               triggerInfo->seqDb[j].triggerInfoP = triggerInfo;
            }
#endif
            if (TRIGGER_MNGRP_IS_TIMER_SRC(sensorInfo->sensorCfg.triggerSrc))
            {
               triggerInfo->lcmForTimer = sensorInfo->sensorCfg.fps;
               triggerInfo->anchorInfo.anchorSensorInfo = sensorInfo;
            }
            /* The FSG trigger source cannot be treated like an SLU trigger as the else statement 
               below will cause a segmentation fault. 
               The Timer won't be started becuase the switch statements on line 1968 won't enter a case where the software timer will be started */
            else if(sensorInfo->sensorCfg.triggerSrc >=INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_0 &&  sensorInfo->sensorCfg.triggerSrc <=INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_5)
            {
               triggerInfo->lcmForTimer = sensorInfo->sensorCfg.fps;
               triggerInfo->anchorInfo.anchorSensorInfo = sensorInfo;
            }
            else if(sensorInfo->sensorCfg.triggerSrc >= INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_0 &&  sensorInfo->sensorCfg.triggerSrc <=INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_5)
            {
               triggerInfo->lcmForTimer = sensorInfo->sensorCfg.fps;
               triggerInfo->anchorInfo.anchorSensorInfo = sensorInfo;
            }
            else //if (slu)
            {
               SEQ_MNGRG_getSenInstBySluInst( sensorInfo->sensorCfg.triggerSrc - INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU0_E,&senSel);
               anchorInfo = SENSORS_MNGRG_getSensorInfo((INU_DEFSG_senSelectE)senSel);
               triggerInfo->anchorInfo.anchorSensorInfo = anchorInfo;
               triggerInfo->lcmForTimer = triggerInfo->anchorInfo.anchorSensorInfo->sensorCfg.fps;
            }
            triggerInfo->sensorsInfo[sensorInfo->sensorCfg.sensorSelect] = sensorInfo;

            TRIGGER_MNGRP_addTriggerToSeq(triggerInfo, sensorInfo->sensorCfg.sensorSelect);
            *triggerH = triggerInfo;
            OS_LYRG_unlockMutex(&triggerInfo->mutex);
            break;
         }
      }
   }
   /*For helsinki we have a new mode where the Trigger manager isn't used but the FSG generates the sync pulses
     In this mode we will need to modify the I/O muxing using the GME registers which is done through calling 
     TRIGGER_MNGR_configureFSG

     Please note that the FSG should be configured in the HW XML or by using the Sensor Sync API, it would be possible to have some sort of autoconfigure of the FSG registers based on the FPS
      but it wouldn't be as flexible as the HW XML
   */
   TRIGGER_MNGR_configureFSG(sensorInfo->sensorCfg.triggerSrc,true);
   switch(sensorInfo->sensorCfg.triggerSrc)
   {
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER1_E):
      {
         ret = OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer1);
         ret = TRIGGER_MNGRP_startTimer(&TRIGGER_MNGRP_Timer1,triggerInfo->lcmForTimer, *triggerH);
         break;
      }
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER2_E):
      {
         ret = OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer2);
         ret = TRIGGER_MNGRP_startTimer(&TRIGGER_MNGRP_Timer2,triggerInfo->lcmForTimer, *triggerH);
         break;
      }
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E):
      {
         ret = OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer3);
         ret = TRIGGER_MNGRP_startTimer(&TRIGGER_MNGRP_Timer3,triggerInfo->lcmForTimer, *triggerH);
         break;
      }
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_0):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_1):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_2):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_3):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_4):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_5):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_0):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_1):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_2):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_3):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_4):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_DEFAULT_DISABLED_5):
      /*TRIGGER_MNGR_configureFSG Handles configuring the FSG for us in all 7 different modes*/
         break;
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU0_E):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU1_E):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU2_E):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU3_E):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU4_E):
      case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU5_E):
      {
         if (triggerInfo->isrEntry==NULL)
         {
            ret = IAE_DRVG_registerSnsrIsrCb(TRIGGER_MNGRP_sluInterCb, triggerInfo, &triggerInfo->isrEntry);
         }
         break;
      }
      default:
         ret = TRIGGER_MNGR__TRIGGER_SRC_NOT_FOUND;
   }
   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Sensor model %d added to triggered by source %d\n",sensorInfo->sensorCfg.sensorModel,sensorInfo->sensorCfg.triggerSrc);
   }
   return ret;
}


ERRG_codeE TRIGGER_MNGRG_stopTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfoP)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   UINT32 i,cnt=0,safetyCtr = 0,totalDelay = 0;
   TRIGGER_MNGRP_triggerInfo *triggerInfoP =  (TRIGGER_MNGRP_triggerInfo*)sensorInfoP->triggerH;
   if ((sensorInfoP->sensorCfg.triggerSrc >= INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM1_E) &&
         (sensorInfoP->sensorCfg.triggerSrc <= INU_DEFSG_SENSOR_TRIGGER_SRC_SW_PWM3_E))
   {
       TRIGGER_MNGRG_stopPwm(sensorInfoP->sensorCfg.triggerSrc - sensorInfoP->sensorCfg.triggerSrc);
       return ret;
   }

   triggerInfoP->stopSeq = 1;
   totalDelay = 2000000;
   //wait if in the middle of a sequence
#ifdef DOUBLE_SEQ_DB
   for( i = 0; i < TRIGGER_MNGRP_NUM_SEQ_DB; i++)
   {
      safetyCtr = 0;

      while((triggerInfoP->seqDb[i].seqCurrent) && (safetyCtr < totalDelay) && (totalDelay!=0))
      {
         OS_LYRG_usleep(100);
         if ((safetyCtr + 100) < totalDelay)
            safetyCtr += 100;
         else
            safetyCtr = totalDelay;
      }

      if ((safetyCtr >= totalDelay) && (totalDelay != 0))
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "trigger sequence is stuck! sen model %d, totalDelay = %d[us], safetyCtr = %d\n",sensorInfoP->sensorCfg.sensorModel,totalDelay,safetyCtr);
         assert(0);
      }
   }
#else
   while((triggerInfoP->seqCurrent) && (safetyCtr < sensorInfoP->sensorCfg.triggerDelay) && (sensorInfoP->sensorCfg.triggerDelay!=0))
   {
      OS_LYRG_usleep(500);
      safetyCtr+=500;
   }

   if ((safetyCtr >= sensorInfoP->sensorCfg.triggerDelay) && (sensorInfoP->sensorCfg.triggerDelay != 0))
   {
      LOGG_PRINT(LOG_WARN_E, NULL, "trigger sequence is stuck! sen model %d\n",sensorInfoP->sensorCfg.sensorModel);
      assert(0);
   }
#endif
   //at this point, the sequence timer should not be running
   TRIGGER_MNGRP_deleteTriggerFromSeq(triggerInfoP, sensorInfoP->sensorCfg.sensorSelect);

   triggerInfoP->sensorsInfo[sensorInfoP->sensorCfg.sensorSelect] = NULL;
   //check if someone register to this trigger source. if not delete
   for (i=0; i<INU_DEFSG_NUM_OF_INPUT_SENSORS;i++)
   {
      if (triggerInfoP->sensorsInfo[i])
         cnt++;
   }
   if (cnt == 0)
   {
      switch(sensorInfoP->sensorCfg.triggerSrc)
      {
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER1_E):
            ret =  OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer1);
            break;
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER2_E):
            ret =  OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer2);
            break;
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E):
            ret =  OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer3);
            break;
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU0_E):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU1_E):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU2_E):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU3_E):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU4_E):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_SLU5_E):
         {
            ret = IAE_DRVG_unregisterSnsrIsrCb(triggerInfoP->isrEntry);
            triggerInfoP->isrEntry = NULL;
            break;
         }
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_0):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_1):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_2):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_3):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_4):
         case (INU_DEFSG_SENSOR_TRIGGER_SRC_FSG_5):
            ret = TRIGGER_MNGR_configureFSG(sensorInfoP->sensorCfg.triggerSrc,false);
            break;
         default:
            ret = TRIGGER_MNGR__TRIGGER_SRC_NOT_FOUND;
      }

      TRIGGER_MNGRP_initTriggerInfo(triggerInfoP);
   }
   else
   {
      if (TRIGGER_MNGRP_IS_TIMER_SRC(triggerInfoP->anchorInfo.source))
      {
         TRIGGER_MNGRP_seqNode  *tmpNode;
         //calc new LCM
         tmpNode = triggerInfoP->seq;
         triggerInfoP->lcmForTimer = triggerInfoP->sensorsInfo[tmpNode->data.sensorSelect]->sensorCfg.fps;
         tmpNode = tmpNode->next;
         while (tmpNode)
         {
            triggerInfoP->lcmForTimer = TRIGGER_MNGRP_calcLcm(triggerInfoP->lcmForTimer, triggerInfoP->sensorsInfo[tmpNode->data.sensorSelect]->sensorCfg.fps);
            tmpNode = tmpNode->next;
         }

         LOGG_PRINT(LOG_DEBUG_E, NULL, "update new lcm %d\n",triggerInfoP->lcmForTimer);
         //reset trigger counters
         TRIGGER_MNGRP_resetTriggerCounter(triggerInfoP);
         switch(sensorInfoP->sensorCfg.triggerSrc)
         {
            case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER1_E):
            {
               ret = OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer1);
               ret = TRIGGER_MNGRP_startTimer(&TRIGGER_MNGRP_Timer1,triggerInfoP->lcmForTimer, triggerInfoP);
               break;
            }
            case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER2_E):
            {
               ret = OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer2);
               ret = TRIGGER_MNGRP_startTimer(&TRIGGER_MNGRP_Timer2,triggerInfoP->lcmForTimer, triggerInfoP);
               break;
            }
            case (INU_DEFSG_SENSOR_TRIGGER_SRC_SW_TIMER3_E):
            {
               ret = OS_LYRG_deleteTimer(&TRIGGER_MNGRP_Timer3);
               ret = TRIGGER_MNGRP_startTimer(&TRIGGER_MNGRP_Timer3,triggerInfoP->lcmForTimer, triggerInfoP);
               break;
            }
            default:
               break;
         }
      }
   }

   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Stop trigger to sensor model %d trigger source %d\n",sensorInfoP->sensorCfg.sensorModel,sensorInfoP->sensorCfg.triggerSrc);
   }

   triggerInfoP->stopSeq = 0;
   return ret;
}


ERRG_codeE TRIGGER_MNGRG_init()
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   UINT32     i,j;
   TRIGGER_MNGRP_triggerInfo *triggerInfoP;
   OS_LYRG_threadParams threadParams;

   memset(&TRIGGER_MNGRP_triggerInfoTbl,0,sizeof(TRIGGER_MNGRP_triggerInfo)*TRIGGER_MNGRP_SUPPORTED_TRIGGER);

   for (i=0;i<TRIGGER_MNGRP_SUPPORTED_TRIGGER;i++)
   {
      triggerInfoP = &TRIGGER_MNGRP_triggerInfoTbl[i];
      TRIGGER_MNGRP_initTriggerInfo(triggerInfoP);
      OS_LYRG_aquireMutex(&triggerInfoP->mutex);
#ifdef DOUBLE_SEQ_DB
      for (j = 0;j < TRIGGER_MNGRP_NUM_SEQ_DB;j++)
      {
         triggerInfoP->seqDb[j].sequenceTimer.funcCb = TRIGGER_MNGRP_sequenceStep;
         triggerInfoP->seqDb[j].sequenceTimer.argP   = &triggerInfoP->seqDb[j];
         ret = OS_LYRG_createTimer(&triggerInfoP->seqDb[j].sequenceTimer);
      }
#else
      triggerInfoP->sequenceTimer.funcCb = TRIGGER_MNGRP_sequenceStep;
      triggerInfoP->sequenceTimer.argP   = triggerInfoP;
      ret = OS_LYRG_createTimer(&triggerInfoP->sequenceTimer);
#endif
   }

   memcpy(TRIGGER_MNGRP_info.msgQue.name, TRIGGER_MNGRP_MSGQ_NAME, sizeof(TRIGGER_MNGRP_MSGQ_NAME));
   TRIGGER_MNGRP_info.msgQue.maxMsgs = TRIGGER_MNGRP_MSGQ_MAX_MSG;
   TRIGGER_MNGRP_info.msgQue.msgSize = TRIGGER_MNGRP_MSGQ_SIZE;

   //Create msg queue
   if(OS_LYRG_createMsgQue(&TRIGGER_MNGRP_info.msgQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Trigger manager msg queue create failed\n");
      return ERR_UNEXPECTED;
   }
   //create thread
   TRIGGER_MNGRP_info.runThread = 1;
   threadParams.func = (OS_LYRG_threadFunction)TRIGGER_MNGRP_thread;
   threadParams.id = OS_LYRG_TRIGGER_MNGR_THREAD_ID_E;
   threadParams.event = NULL;
   threadParams.param = &TRIGGER_MNGRP_info;

   TRIGGER_MNGRP_info.thrdH = OS_LYRG_createThread(&threadParams);
   if (TRIGGER_MNGRP_info.thrdH == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create thread\n");
      return TRIGGER_MNGR__ERR_UNEXPECTED;
   }

   memset(&TRIGGER_MNGRP_Timer1,0,sizeof(TRIGGER_MNGRP_Timer1));
   memset(&TRIGGER_MNGRP_Timer2,0,sizeof(TRIGGER_MNGRP_Timer2));
   memset(&TRIGGER_MNGRP_Timer3,0,sizeof(TRIGGER_MNGRP_Timer3));

   memset(TRIGGER_MNGR_pwmDb,0,sizeof(TRIGGER_MNGR_pwmDb));

   return ret;
}

ERRG_codeE TRIGGER_MNGRG_deinit()
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   UINT32     i,j, saftyCtr = 0;
   TRIGGER_MNGRP_triggerInfo *triggerInfoP;

   TRIGGER_MNGRP_info.runThread = 0;
   while ((TRIGGER_MNGRP_info.threadActive) && (saftyCtr < 10))
   {
      OS_LYRG_usleep(100000);
      saftyCtr++;
   }
   if (saftyCtr == 10)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,  "thread fails on exit\n");
   }

   OS_LYRG_deleteMsgQue(&TRIGGER_MNGRP_info.msgQue);

   for (i=0;i<TRIGGER_MNGRP_SUPPORTED_TRIGGER;i++)
   {
      triggerInfoP = &TRIGGER_MNGRP_triggerInfoTbl[i];
#ifdef DOUBLE_SEQ_DB
      for (j = 0;j < TRIGGER_MNGRP_NUM_SEQ_DB;j++)
      {
         OS_LYRG_deleteTimer(&triggerInfoP->seqDb[j].sequenceTimer);
      }
#else
      OS_LYRG_deleteTimer(&triggerInfoP->sequenceTimer);
#endif
      OS_LYRG_releaseMutex(&triggerInfoP->mutex);
   }

   OS_LYRG_waitForThread(TRIGGER_MNGRP_info.thrdH, OS_LYRG_INFINITE);

   memset(&TRIGGER_MNGRP_info,          0, sizeof(TRIGGER_MNGRP_info));
   memset(&TRIGGER_MNGRP_triggerInfoTbl,0, sizeof(TRIGGER_MNGRP_triggerInfo)*TRIGGER_MNGRP_SUPPORTED_TRIGGER);

   return ret;
}

void TRIGGER_MNGRG_stopPwm(unsigned int pwmTrigNum)
{
   if (pwmTrigNum < INU_DEVICE_NUM_PWM_E)
   {
      TRIGGER_MNGR_pwmDb[pwmTrigNum].fps = 0;
      TRIGGER_MNGR_pwmDb[pwmTrigNum].usecWidth = 0;
      IAE_DRVG_disablePwmDim(pwmTrigNum);
   }
}

void TRIGGER_MNGRG_configPwm(unsigned int pwmTrigNum,unsigned int pwmFps,unsigned int pwmUsecWidth, unsigned int pwmCmd)
{
   LOGG_PRINT(LOG_INFO_E, NULL,"pwmNum %d FPS %d width %d command %d\n",pwmTrigNum,pwmFps,pwmUsecWidth,pwmCmd);
   if (pwmTrigNum < INU_DEVICE_NUM_PWM_E)
   {
      TRIGGER_MNGR_pwmDb[pwmTrigNum].fps = pwmFps;
      TRIGGER_MNGR_pwmDb[pwmTrigNum].usecWidth = pwmUsecWidth;
      if (pwmCmd == INU_DEVICE_PWM_CONFIG_START_E)
         IAE_DRVG_enablePwmDim(pwmTrigNum,pwmFps,pwmUsecWidth);
      else if (pwmCmd == INU_DEVICE_PWM_STOP_E)
         IAE_DRVG_disablePwmDim(pwmTrigNum);
   }
}
#ifdef __cplusplus
   }
#endif

