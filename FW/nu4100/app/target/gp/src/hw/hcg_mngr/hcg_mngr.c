/****************************************************************************
 *
 *   FileName: hcd_mngr.c
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: Hardware Clock Gating manager.
 *                     Todos:
 *                     1. Voters DB: change array to link list
 *                     2. add map between event to voters using it
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "assert.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "linked_list.h"
#include "hcg_mngr.h"
#include "gme_mngr.h"
#include "gme_drv.h"
#include "iae_drv.h"
#include "inu2.h"
#include "log.h"
#include "helsinki.h"
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define UNIFY_TIMER
//#define TIMER_STATS
#ifdef UNIFY_TIMER
#define HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS (6)
#endif
#define HCG_MNGRP_MAX_NUM_HW_VOTERS          (10)
#define HCG_MNGRP_MAX_VOTER_NAME_LEN         (30)
#define HCG_MNGRP_MAX_UNIT_NAME_LEN          (30)
//voter debug shows more info of the manager, but can cause break in the system if used with high fps
//#define VOTERS_DEBUG
#define VOTERS_STATISTICS
//#define PROCESS_EVENT_STATS
#define HCG_MNGRP_GUARD_TIMER_NS             (2000*1000) //timer jitter, and unexcpected readout time changing from sensors
#define HCG_MNGRP_LEARNING_NUM_SAMPLES       (30)
#define HCG_MNGRP_INVALID_SENSOR_INDEX       (0xFFFFFFFF)
#define HCG_MNGRP_POLL_WAIT_TIME_PRIME       (500)
#define HCG_MNGRP_MAX_WAIT_TIME_PRIME        (4000 * HCG_MNGRP_POLL_WAIT_TIME_PRIME)
/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef struct
{
   UINT32 index;       //table index
   UINT32 voteUnits;   //which hw units are used by this voter. bitmap according to HCG_MNGRG_hwUnitE enum
   UINT32 upEvent;     //which hw event used to enable the hw units
#ifdef MULTIPILE_DOWN_EVENTS
   Node   *downEvents; //which hw events used to disable the hw units
   UINT32 downEventsNum;
   UINT32 totalDownEventsNum;
#else
   UINT32 downEvent;   //which hw event used to disable the hw units
#endif
   UINT32 votedUnits;  //which hw units is now voted
   UINT32 enDynVoting; //enable dynamic voting. If false, than we vote when registering units and only release on unregister -> unit is on always
   UINT32 used;        //mark if entry is used

   OS_LYRG_mutexT voterMutex;
#ifndef UNIFY_TIMER
   OS_LYRG_timerT timer; //timer for pre-readout event
#else
   UINT32 sensorIndex;
#endif

   //channel learning statistics
   UINT64 currentStartTimeNsec;
   UINT64 currentEndTimeNsec;
   UINT64 minFrameDurationNsec;     //minimum frame period
   UINT64 maxHwProcessDurationNsec; //maximum hw process
   UINT64 activationCounter;      //number of activations
   UINT32 statsValid;             //when stats are valid, we can start down vote the hw units for this voter
   HCG_MNGRG_hwEventE startEvent; //start of hw process
   HCG_MNGRG_hwEventE endEvent;   //end of hw process

   UINT8  voterDownEventPerFrame[16];
   UINT8  downEventPerFrameCnt;
   UINT8  maxFRRVector;

   char   name[HCG_MNGRP_MAX_VOTER_NAME_LEN];
} HCG_MNGRP_hwVoterParamT;


typedef struct
{
   OS_LYRG_mutexT    hwUnitMutex;
   UINT32            refCount;      //how many are using this unit
   UINT32            enableClkGate; //if this unit is allowed to be clock gated
   GME_DRVG_hwUnitE  hwUnit;        //GME enum

#ifdef VOTERS_STATISTICS
   //statistics
   UINT64         totalTimeOnNsec;
   UINT64         totalTimeOffNsec;
   UINT64         timeTurnedOff;
   UINT64         timeTurnedOn;
#endif

   char   name[HCG_MNGRP_MAX_UNIT_NAME_LEN];
} HCG_MNGRP_hwUnitsParamT;

typedef struct
{
   Node           *voters;
   OS_LYRG_mutexT listMutex;
   char           name[2 * HCG_MNGRP_MAX_UNIT_NAME_LEN];
} HCG_MNGRP_eventTableT;

typedef struct
{
   HCG_MNGRP_hwVoterParamT votersDb[HCG_MNGRP_MAX_NUM_HW_VOTERS];
   HCG_MNGRP_hwUnitsParamT hwUnitsDb[HCG_MNGRG_MAX_NUM_HW_UNITS];

   //prime period: when channels are added, or stopped, or fps change - restart the statstics of the channels, stop voting until stable
   OS_LYRG_timerT          primeTimer;
   UINT32                  primePeriod;

   //table of events and their voters. Ease the access in critical code (events)
   HCG_MNGRP_eventTableT upEventsTable[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
   HCG_MNGRP_eventTableT downEventsTable[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
   HCG_MNGRP_eventTableT startLearnEventsTable[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
#ifndef MULTIPILE_DOWN_EVENTS
   HCG_MNGRP_eventTableT endLearnEventsTable[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
#endif

#ifdef UNIFY_TIMER
   OS_LYRG_timerT timer[HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS];         //timers for pre-readout event
   UINT32         timerIndex[HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS];   
   UINT32         timerUsed[HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS];
#ifdef TIMER_STATS
   UINT64         designatedTimeNsec[HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS];
   UINT64         actualTimeExpiredNsec[HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS];
#endif
#endif


#ifdef PROCESS_EVENT_STATS
   UINT64 maxEventTimeNsec[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
   UINT64 minEventTimeNsec[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
   UINT64 aveEventTimeNsec[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
   UINT64 sumEventTimeNsec[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
   UINT64 eventCounter[HCG_MNGRG_HW_EVENT_MAX_NUM_E];
#endif   
} HCG_MNGRP_paramsT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static HCG_MNGRP_paramsT      votingParams;


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void HCG_MNGRG_snsrEventWrapper(UINT64 timestamp, UINT32 slu, void *argP);

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: HCG_MNGRP_hwEventToString
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
char *HCG_MNGRP_hwEventToString( UINT32 hwEvent )
{
   switch(hwEvent)
   {
   case(HCG_MNGRG_HW_EVENT_SENSOR_TIMER_1_E):
      return "TIMER_1";

   case(HCG_MNGRG_HW_EVENT_SENSOR_TIMER_2_E):
      return "TIMER_2";

   case(HCG_MNGRG_HW_EVENT_SENSOR_TIMER_3_E):
      return "TIMER_3";

   case(HCG_MNGRG_HW_EVENT_SENSOR_TIMER_4_E):
      return "TIMER_4";

   case(HCG_MNGRG_HW_EVENT_SENSOR_TIMER_5_E):
      return "TIMER_5";

   case(HCG_MNGRG_HW_EVENT_SENSOR_TIMER_6_E):
      return "TIMER_6";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_START_ISR_E):
      return "SLU0_FRAME_START";
      
   case(HCG_MNGRG_HW_EVENT_IAE_SLU1_FRAME_START_ISR_E):
      return "SLU1_FRAME_START";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU2_FRAME_START_ISR_E):
      return "SLU2_FRAME_START";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU3_FRAME_START_ISR_E):
      return "SLU3_FRAME_START";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU4_FRAME_START_ISR_E):
      return "SLU4_FRAME_START";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU5_FRAME_START_ISR_E):
      return "SLU5_FRAME_START";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E):
      return "SLU0_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + 1):
      return "SLU1_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + 2):
      return "SLU2_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + 3):
      return "SLU3_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + 4):
      return "SLU4_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + 5):
      return "SLU5_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E):
      return "IAU0_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E + 1):
      return "IAU1_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E + 2):
      return "IAU2_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E + 3):
      return "IAU3_FRAME_END";

   case(HCG_MNGRG_HW_EVENT_WRITER_0_E):
      return "WRITER_0";

   case(HCG_MNGRG_HW_EVENT_WRITER_1_E):
      return "WRITER_1";

   case(HCG_MNGRG_HW_EVENT_WRITER_2_E):
      return "WRITER_2";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH0_E):
      return "DMA_CORE0_CH0";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH1_E):
      return "DMA_CORE0_CH1";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH2_E):
      return "DMA_CORE0_CH2";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH3_E):
      return "DMA_CORE0_CH3";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH4_E):
      return "DMA_CORE0_CH4";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH5_E):
      return "DMA_CORE0_CH5";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH6_E):
      return "DMA_CORE0_CH6";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE0_CH7_E):
      return "DMA_CORE0_CH7";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH0_E):
      return "DMA_CORE1_CH0";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH1_E):
      return "DMA_CORE1_CH1";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH2_E):
      return "DMA_CORE1_CH2";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH3_E):
      return "DMA_CORE1_CH3";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH4_E):
      return "DMA_CORE1_CH4";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH5_E):
      return "DMA_CORE1_CH5";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH6_E):
      return "DMA_CORE1_CH6";
   
   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE1_CH7_E):
      return "DMA_CORE1_CH7";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH0_E):
      return "DMA_CORE2_CH0";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH1_E):
      return "DMA_CORE2_CH1";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH2_E):
      return "DMA_CORE2_CH2";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH3_E):
      return "DMA_CORE2_CH3";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH4_E):
      return "DMA_CORE2_CH4";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH5_E):
      return "DMA_CORE2_CH5";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH6_E):
      return "DMA_CORE2_CH6";

   case(HCG_MNGRG_HW_EVENT_CDE_ISR_CORE2_CH7_E):
      return "DMA_CORE2_CH7";         

   case(HCG_MNGRG_HW_EVENT_IAE_IAU_HIST0_RDY_ISR_E):
      return "HIST0_RDY";

   case(HCG_MNGRG_HW_EVENT_IAE_IAU_HIST1_RDY_ISR_E):
      return "HIST1_RDY";

   default:
      return "unknown hw event";
   }
}


/****************************************************************************
*
*  Function Name: HCG_MNGRP_initHwUnit
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
static void HCG_MNGRP_initHwUnit(char *name, HCG_MNGRP_hwUnitsParamT *hwUnitP, UINT32 enableClkGate, GME_DRVG_hwUnitE hwUnit, HCG_MNGRG_hwUnitE unit)
{
   memcpy(hwUnitP->name, name, strlen(name));
   hwUnitP->hwUnit = hwUnit;
   OS_LYRG_aquireMutex(&hwUnitP->hwUnitMutex);

   if (!enableClkGate)
   {
      //temp set as enabled, to allow one time vote
      hwUnitP->enableClkGate = 1;
      HCG_MNGRG_voteUnit(unit);
   }
   hwUnitP->enableClkGate = enableClkGate;   

#ifdef VOTERS_STATISTICS
   if (hwUnitP->enableClkGate)
   {
      OS_LYRG_getTimeNsec(&hwUnitP->timeTurnedOff);
   }
#endif
}


/****************************************************************************
*
*  Function Name: HCG_MNGRP_timerCb
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
static void HCG_MNGRP_timerCb(void *argP)
{
   HCG_MNGRG_hwEventE event;
#ifdef UNIFY_TIMER
   UINT32 timerIndex = *(UINT32*)argP;
   event = timerIndex + HCG_MNGRG_HW_EVENT_SENSOR_TIMER_1_E;
   votingParams.timerUsed[timerIndex] = 0;
#else
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)argP;
   event = hwVoterP->upEvent;
#endif
   UINT64                  nsecStart;  
   OS_LYRG_getTimeNsec(&nsecStart);

#ifdef TIMER_STATS
   votingParams.actualTimeExpiredNsec[timerIndex] = nsecStart;
#endif
   HCG_MNGRG_processEvent(event, nsecStart, 1);
}

#ifndef UNIFY_TIMER
/****************************************************************************
*
*  Function Name: HCG_MNGRP_timerCreate
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
static void HCG_MNGRP_timerCreate(HCG_MNGRP_hwVoterParamT *hwVoterP)
{
   ERRG_codeE ret;

   hwVoterP->timer.funcCb = HCG_MNGRP_timerCb;
   hwVoterP->timer.argP   = hwVoterP;

   ret = OS_LYRG_createTimer(&hwVoterP->timer);
   if (ERRG_FAILED(ret))
      assert(0);
}
#endif

/****************************************************************************
*
*  Function Name: HCG_MNGRP_invokeTimer
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
static inline ERRG_codeE HCG_MNGRP_invokeTimer(HCG_MNGRP_hwVoterParamT *hwVoterP, UINT64 eventTime)
{
   UINT64 nsec,currentNsec,timePassed;

#ifdef UNIFY_TIMER
   if (votingParams.timerUsed[hwVoterP->sensorIndex])
      return HCG_MNGR__RET_SUCCESS;
#endif

   if (hwVoterP->sensorIndex == HCG_MNGRP_INVALID_SENSOR_INDEX)
      return HCG_MNGR__RET_SUCCESS;

   //get current time
   OS_LYRG_getTimeNsec(&currentNsec);

   //calc how much time has passed from the event (the statistics are based on the time of the real events)
   timePassed = currentNsec - eventTime;

   //check if enough time for invoking timer
   if ((hwVoterP->maxHwProcessDurationNsec + HCG_MNGRP_GUARD_TIMER_NS + timePassed) >= hwVoterP->minFrameDurationNsec)
   {
      return HCG_MNGR__ERR_UNEXPECTED;
   }   
   nsec = hwVoterP->minFrameDurationNsec - hwVoterP->maxHwProcessDurationNsec - timePassed - HCG_MNGRP_GUARD_TIMER_NS; //TODO - exposure changes

#ifdef VOTERS_DEBUG
   //printf("%llu invoke timer %s of %s to %llu (event %llu, passed %llu))\n",currentNsec,HCG_MNGRP_hwEventToString(hwVoterP->upEvent),hwVoterP->name,nsec,eventTime,timePassed);
#endif
#ifndef UNIFY_TIMER
   hwVoterP->timer.firstExpiryNsec    = nsec;
   hwVoterP->timer.firstExpirySec     = 0;
   hwVoterP->timer.intervalExpirySec  = 0;
   hwVoterP->timer.intervalExpiryNsec = 0;
   return OS_LYRG_setTimer(&hwVoterP->timer);    
#else
#ifdef TIMER_STATS
   votingParams.designatedTimeNsec[hwVoterP->sensorIndex] = nsec + currentNsec;
#endif
   votingParams.timer[hwVoterP->sensorIndex].firstExpiryNsec    = nsec;
   votingParams.timer[hwVoterP->sensorIndex].firstExpirySec     = 0;
   votingParams.timer[hwVoterP->sensorIndex].intervalExpirySec  = 0;
   votingParams.timer[hwVoterP->sensorIndex].intervalExpiryNsec = 0;//hwVoterP->minFrameDurationNsec;
   votingParams.timerUsed[hwVoterP->sensorIndex] = 1;
   return OS_LYRG_setTimer(&votingParams.timer[hwVoterP->sensorIndex]); 
#endif  
}


/****************************************************************************
*
*  Function Name: HCG_MNGRP_resetStatistics
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
inline static void HCG_MNGRP_resetStatistics(HCG_MNGRP_hwVoterParamT *voterP)
{
   voterP->statsValid               = 0;
   voterP->maxHwProcessDurationNsec = 0;
   voterP->minFrameDurationNsec     = ~0ULL;
   voterP->activationCounter        = 0; 
   voterP->currentStartTimeNsec     = 0;
}

/****************************************************************************
*
*  Function Name: HCG_MNGRP_resetVoterEvents
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
inline static void HCG_MNGRP_resetVoterEvents(HCG_MNGRP_hwVoterParamT *voterP)
{
#ifdef MULTIPILE_DOWN_EVENTS
   voterP->downEvents = NULL;
   voterP->totalDownEventsNum = 0;
#else
   voterP->downEvent  = HCG_MNGRG_HW_EVENT_INVALID_E;
#endif
   voterP->upEvent    = HCG_MNGRG_HW_EVENT_INVALID_E;
   voterP->startEvent = HCG_MNGRG_HW_EVENT_INVALID_E;
   voterP->endEvent   = HCG_MNGRG_HW_EVENT_INVALID_E;
}

/****************************************************************************
*
*  Function Name: HCG_MNGRP_initVoter
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
inline static void HCG_MNGRP_initVoter(HCG_MNGRP_hwVoterParamT *voterP)
{
   voterP->votedUnits = 0;
   voterP->voteUnits  = 0;
   HCG_MNGRP_resetVoterEvents(voterP);
   HCG_MNGRP_resetStatistics(voterP);
   voterP->maxFRRVector=1; // vector of 1 in regular case
   memset(voterP->voterDownEventPerFrame,0,sizeof(voterP->voterDownEventPerFrame));
   voterP->downEventPerFrameCnt=0;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRP_primeTimerCb
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
static void HCG_MNGRP_primeTimerCb(void *argP)
{
   HCG_MNGRP_paramsT      *paramsP = (HCG_MNGRP_paramsT*)argP;
#ifdef VOTERS_DEBUG   
   printf("Prime time over!!! start gather stats for all voters!!!\n");
#endif
   paramsP->primePeriod = 0;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRP_invokePrimingTimer
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
static ERRG_codeE HCG_MNGRP_invokePrimingTimer(HCG_MNGRP_paramsT *paramsP)
{
   paramsP->primeTimer.firstExpiryNsec = 0;
   paramsP->primeTimer.firstExpirySec = 4;
   paramsP->primeTimer.intervalExpirySec = 0;
   paramsP->primeTimer.intervalExpiryNsec = 0;
   return OS_LYRG_setTimer(&paramsP->primeTimer);   
}

static void HCG_MNGRP_initEventTable(HCG_MNGRP_eventTableT *table, char *name)
{
   UINT32 i;
   for (i = 0; i < HCG_MNGRG_HW_EVENT_MAX_NUM_E; i++)
   {
      OS_LYRG_aquireMutex(&table->listMutex);
      strncpy(table->name, name, sizeof(table->name));
      strcat(table->name, "_");
      strcat(table->name, HCG_MNGRP_hwEventToString(i));
#ifdef VOTERS_DEBUG
      printf("init table: %s\n",table->name);
#endif
      table++;
   }
}

/****************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/
/****************************************************************************
*
*  Function Name: HCG_MNGRG_initVoteSystem
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
ERRG_codeE HCG_MNGRG_initVoteSystem( void )
{
   ERRG_codeE ret = HCG_MNGR__RET_SUCCESS;
   int i;
   
   memset(&votingParams,0,sizeof(HCG_MNGRP_paramsT));
   votingParams.primeTimer.funcCb = HCG_MNGRP_primeTimerCb;
   votingParams.primeTimer.argP   = &votingParams;

#ifdef PROCESS_EVENT_STATS
   memset(votingParams.minEventTimeNsec,0xFF,sizeof(UINT64)*HCG_MNGRG_HW_EVENT_MAX_NUM_E);
#endif

   ret = OS_LYRG_createTimer(&votingParams.primeTimer);
   if (ERRG_FAILED(ret))
      assert(0);

   HCG_MNGRP_initEventTable(votingParams.downEventsTable, "DownEventTbl");
   HCG_MNGRP_initEventTable(votingParams.upEventsTable  , "UpEventTbl");
   HCG_MNGRP_initEventTable(votingParams.startLearnEventsTable, "StartLearnEventTbl");
#ifndef MULTIPILE_DOWN_EVENTS
   HCG_MNGRP_initEventTable(votingParams.endLearnEventsTable, "EndLearnEventTbl");
#endif

   //Init HW units
   HCG_MNGRP_initHwUnit("DMA0",&votingParams.hwUnitsDb[HCG_MNGRG_DMA0],1, GME_DRVG_HW_UNIT_DMA_0_E, HCG_MNGRG_DMA0);
   HCG_MNGRP_initHwUnit("DMA1",&votingParams.hwUnitsDb[HCG_MNGRG_DMA1],1, GME_DRVG_HW_UNIT_DMA_1_E, HCG_MNGRG_DMA1);
   HCG_MNGRP_initHwUnit("DMA2",&votingParams.hwUnitsDb[HCG_MNGRG_DMA2],1, GME_DRVG_HW_UNIT_DMA_2_E, HCG_MNGRG_DMA2);
   #ifdef HELSINKI_USING_FSG_FOR_FTRIG_OUTPUTS
   /*Disable clock gating for the IAE for the Helsinki project 
      due to the FSG being used which has external triggers that could happen at any time
   */
   LOGG_PRINT(LOG_INFO_E,NULL,"[Helsinki] Clock gating disabled for the IAE \n");
   HCG_MNGRP_initHwUnit("IAE", &votingParams.hwUnitsDb[HCG_MNGRG_IAE], 0, GME_DRVG_HW_UNIT_IAE_E,   HCG_MNGRG_IAE);
   #else
   HCG_MNGRP_initHwUnit("IAE", &votingParams.hwUnitsDb[HCG_MNGRG_IAE], 1, GME_DRVG_HW_UNIT_IAE_E,   HCG_MNGRG_IAE);
   #endif
   HCG_MNGRP_initHwUnit("DPE", &votingParams.hwUnitsDb[HCG_MNGRG_DPE], 1, GME_DRVG_HW_UNIT_DPE_E,   HCG_MNGRG_DPE);
   HCG_MNGRP_initHwUnit("PPE", &votingParams.hwUnitsDb[HCG_MNGRG_PPE], 1, GME_DRVG_HW_UNIT_PPE_E,   HCG_MNGRG_PPE);
   HCG_MNGRP_initHwUnit("CVA", &votingParams.hwUnitsDb[HCG_MNGRG_CVA], 1, GME_DRVG_HW_UNIT_CVA_E,   HCG_MNGRG_CVA);

#ifdef UNIFY_TIMER
   for (i = 0; i < HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS; i++)
   {
      votingParams.timer[i].funcCb = HCG_MNGRP_timerCb;
      votingParams.timerIndex[i]   = i;
      votingParams.timer[i].argP   = &votingParams.timerIndex[i];

      ret = OS_LYRG_createTimer(&votingParams.timer[i]);
      if (ERRG_FAILED(ret))
         assert(0);
   }
#endif
   for (i = 0; i < HCG_MNGRP_MAX_NUM_HW_VOTERS; i++)
   {
      HCG_MNGRP_initVoter(&votingParams.votersDb[i]);
#ifndef UNIFY_TIMER
      HCG_MNGRP_timerCreate(&votingParams.votersDb[i]);
#endif
      OS_LYRG_aquireMutex(&votingParams.votersDb[i].voterMutex);      
      votingParams.votersDb[i].index = i;
   }

   //register events
   ret = IAE_DRVG_registerSnsrIsrCb(HCG_MNGRG_snsrEventWrapper, &votingParams, (void*)&i); 
   return ret;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_startPrimePeriod
*
*  Description: Prime is a period of time in which we let the system enter a stable state.
*                    Its activated after add/remove of a channel, or fps change of a channel.
*                    Reset all channel statistics, and prevent learning during this period.
*                    When period the over, then learning can start again.
*                    The function will block until all the channels has entered prime state
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
void HCG_MNGRG_startPrimePeriod( void )
{
   HCG_MNGRP_paramsT *paramsP = &votingParams;
   UINT32 i;
   UINT32 votersNotDone = 1, totalTime = 0;

   if (!paramsP->primePeriod)
   {
      //new channel, set priming time - arm a timer, disable voting, reset stats of all
      for ( i = 0; i < HCG_MNGRP_MAX_NUM_HW_VOTERS; i++)
      {
         OS_LYRG_lockMutex(&paramsP->votersDb[i].voterMutex);      
         if (paramsP->votersDb[i].used)
         {
            HCG_MNGRP_resetStatistics(&paramsP->votersDb[i]);
         }
         OS_LYRG_unlockMutex(&paramsP->votersDb[i].voterMutex);         
      }

      //start the prime period here
      paramsP->primePeriod = 1;

      //wait until all the voters entered priming state
      while ((votersNotDone) && (totalTime < HCG_MNGRP_MAX_WAIT_TIME_PRIME))
      {
         votersNotDone = 0;
         for ( i = 0; i < HCG_MNGRP_MAX_NUM_HW_VOTERS; i++)
         {
            OS_LYRG_lockMutex(&paramsP->votersDb[i].voterMutex);
            if (paramsP->votersDb[i].used)
            {
               if (paramsP->votersDb[i].votedUnits != paramsP->votersDb[i].voteUnits)
               {
                  votersNotDone = 1;
               }
            }
            OS_LYRG_unlockMutex(&paramsP->votersDb[i].voterMutex);   
            if (votersNotDone)
               break;
         }
         if (votersNotDone)
         {
            OS_LYRG_usleep(HCG_MNGRP_POLL_WAIT_TIME_PRIME);
            totalTime+= HCG_MNGRP_POLL_WAIT_TIME_PRIME;
         }
      }

      if (totalTime >= HCG_MNGRP_MAX_WAIT_TIME_PRIME)
      {
         LOGG_PRINT(LOG_WARN_E, NULL, "Not all voters are in prime state!\n");
      }
      
      HCG_MNGRP_invokePrimingTimer(paramsP);   
   }
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_addVoterToEventTable
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
inline static void HCG_MNGRG_addVoterToEventTable( HCG_MNGRG_voterHandle handle, HCG_MNGRP_eventTableT *eventTableP )
{
   Node *temp;
   //add this voter to the event table. check first if already exists
   OS_LYRG_lockMutex(&eventTableP->listMutex);
   temp = eventTableP->voters;
   while(temp)
   {
      if (temp->val == handle)
      {
         //voter is already registered to this event, exit
         OS_LYRG_unlockMutex(&eventTableP->listMutex);
         return;
      }
      temp = temp->next;
   }

   //if we reached this point, then we need to add the voter
   Node *newVoterMapEntry = (Node*)malloc(sizeof(Node));
   memset(newVoterMapEntry,0,sizeof(Node));
   newVoterMapEntry->val = handle;
   if (eventTableP->voters)
   {
      newVoterMapEntry->next = eventTableP->voters;
      eventTableP->voters = newVoterMapEntry;
   }
   else
   {
      eventTableP->voters = newVoterMapEntry;
   }
#ifdef VOTERS_DEBUG
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
   printf("Added voter %s to table %s\n",hwVoterP->name, eventTableP->name);
#endif
   OS_LYRG_unlockMutex(&eventTableP->listMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_removeVoterFromEventTable
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
inline static void HCG_MNGRG_removeVoterFromEventTable( HCG_MNGRG_voterHandle handle, HCG_MNGRP_eventTableT *eventTableP )
{
   Node *temp = eventTableP->voters;
   Node *voterNode;
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle; 
#ifdef VOTERS_DEBUG
   printf("remove voter %s from table %s\n",hwVoterP->name,eventTableP->name);
#endif
   if(!temp)
      return;

   OS_LYRG_lockMutex(&eventTableP->listMutex);
   //check if its the head
   if (temp->val == handle)
   {
      voterNode = temp;
      eventTableP->voters = eventTableP->voters->next;
      free(voterNode);
      OS_LYRG_unlockMutex(&eventTableP->listMutex);
      return;
   }
   else
   {
      while(temp)
      {
         //if node is not the head, then the list must have more then 2 nodes, so its safe to access temp->next
         if (temp->next->val == handle)
         {
            voterNode = temp->next;
            temp->next = temp->next->next;
            free(voterNode);
            OS_LYRG_unlockMutex(&eventTableP->listMutex);
            return;
         }
         temp = temp->next;
      }
   }

   printf("error! voter %s was not found in table %p\n",hwVoterP->name,eventTableP);
   OS_LYRG_unlockMutex(&eventTableP->listMutex);
   assert(0);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_allocVoter
*
*  Description: search for an empty entry in DB, init the entry.
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
ERRG_codeE HCG_MNGRG_allocVoter( HCG_MNGRG_voterHandle *handleP, char *name, UINT32 enDynVoting )
{
   ERRG_codeE ret = HCG_MNGR__RET_SUCCESS;
   HCG_MNGRP_paramsT *paramsP = &votingParams;
   UINT32 i;
   
   for ( i = 0; i < HCG_MNGRP_MAX_NUM_HW_VOTERS; i++)
   {
      OS_LYRG_lockMutex(&paramsP->votersDb[i].voterMutex);
      if (!paramsP->votersDb[i].used)
      {
         paramsP->votersDb[i].used = 1;
         memset(paramsP->votersDb[i].name, 0, HCG_MNGRP_MAX_VOTER_NAME_LEN);
         memcpy(paramsP->votersDb[i].name, name, strlen(name));
         *handleP = &paramsP->votersDb[i];
         paramsP->votersDb[i].enDynVoting = enDynVoting;
         HCG_MNGRP_initVoter(*handleP);   
#ifdef VOTERS_DEBUG         
         printf("allocated voter %s -> %p success. voting enabled? %d\n",name, *handleP, enDynVoting);
#endif
         OS_LYRG_unlockMutex(&paramsP->votersDb[i].voterMutex);         
         break;
      }
      OS_LYRG_unlockMutex(&paramsP->votersDb[i].voterMutex);
   }

   if (i == HCG_MNGRP_MAX_NUM_HW_VOTERS)
   {
      ret = HCG_MNGR__ERR_OUT_OF_RSRCS;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_deallocVoter
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
void HCG_MNGRG_deallocVoter( HCG_MNGRG_voterHandle handle )
{
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
#ifdef MULTIPILE_DOWN_EVENTS
   Node *downEvent;
#endif

   OS_LYRG_lockMutex(&hwVoterP->voterMutex);
#ifdef MULTIPILE_DOWN_EVENTS
   while(hwVoterP->downEvents)
   {
      HCG_MNGRG_removeVoterFromEventTable(handle, &votingParams.downEventsTable[(UINT32)(hwVoterP->downEvents->val)]);
      downEvent = hwVoterP->downEvents;
      hwVoterP->downEvents = hwVoterP->downEvents->next;
      free(downEvent);
   }
#else
   HCG_MNGRG_removeVoterFromEventTable(handle, &votingParams.downEventsTable[hwVoterP->downEvent]);
#endif
   HCG_MNGRG_removeVoterFromEventTable(handle, &votingParams.upEventsTable[hwVoterP->upEvent]);
   HCG_MNGRG_removeVoterFromEventTable(handle, &votingParams.startLearnEventsTable[hwVoterP->startEvent]);
#ifndef MULTIPILE_DOWN_EVENTS
   HCG_MNGRG_removeVoterFromEventTable(handle, &votingParams.endLearnEventsTable[hwVoterP->endEvent]);
#endif

#ifdef VOTERS_DEBUG   
   printf("dealloc voter %s success. voteUnits 0x%x, votedUnits 0x%x\n",hwVoterP->name,hwVoterP->voteUnits,hwVoterP->votedUnits);
#endif
   hwVoterP->used = 0; //events will not process for this voter
   
   //incase channel is now active, free all voted units
   HCG_MNGRG_devoteUnits(hwVoterP->votedUnits);
   hwVoterP->votedUnits = 0;
   
   //clear db
   HCG_MNGRP_resetVoterEvents(hwVoterP);
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_registerHwUnitUpEvent
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
void HCG_MNGRG_registerHwUnitUpEvent( HCG_MNGRG_voterHandle handle, UINT32 event )
{
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
   OS_LYRG_lockMutex(&hwVoterP->voterMutex);
   //allow only one event
   if (hwVoterP->upEvent == HCG_MNGRG_HW_EVENT_INVALID_E)
   {
      hwVoterP->upEvent = event;
      HCG_MNGRG_addVoterToEventTable(handle, &votingParams.upEventsTable[event]);
#ifdef VOTERS_DEBUG   
      printf("new up event. event %s to voter %s\n",HCG_MNGRP_hwEventToString(event), hwVoterP->name);
#endif
   }
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_registerHwUnitDownEvent
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
void HCG_MNGRG_registerHwUnitDownEvent( HCG_MNGRG_voterHandle handle, UINT32 event, UINT32 numOccurrences ,UINT32 frrVecSize)
{
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;

   if (!hwVoterP)
      return;

   OS_LYRG_lockMutex(&hwVoterP->voterMutex); 
#ifdef MULTIPILE_DOWN_EVENTS
   Node *newDownEventP  = (Node*)malloc(sizeof(Node));
   newDownEventP->val   = (void*)event;
   newDownEventP->next  = hwVoterP->downEvents;
   hwVoterP->downEvents = newDownEventP;
   if (frrVecSize != 0)
       hwVoterP->maxFRRVector = frrVecSize;
   HCG_MNGRG_addVoterToEventTable(handle, &votingParams.downEventsTable[event]);
   hwVoterP->totalDownEventsNum += numOccurrences;
#ifdef VOTERS_DEBUG
   printf("new down event. event %s to voter %s total down events %d\n",HCG_MNGRP_hwEventToString(event), hwVoterP->name, hwVoterP->totalDownEventsNum);
#endif
#else
   //allow only one event
   if (hwVoterP->downEvent == HCG_MNGRG_HW_EVENT_INVALID_E)
   {
      hwVoterP->downEvent = event;
      HCG_MNGRG_addVoterToEventTable(handle, &votingParams.downEventsTable[event]);
#ifdef VOTERS_DEBUG   
      printf("new down event. event %s to voter %s\n",HCG_MNGRP_hwEventToString(event), hwVoterP->name);
#endif
   }
#endif
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_registerHwUnitVote
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
ERRG_codeE HCG_MNGRG_registerHwUnitVote( HCG_MNGRG_voterHandle handle, HCG_MNGRG_hwUnitE hwUnit )
{
   ERRG_codeE ret = HCG_MNGR__RET_SUCCESS;
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
#ifdef VOTERS_DEBUG
   HCG_MNGRP_hwUnitsParamT *hwUnitP = &votingParams.hwUnitsDb[hwUnit];
#endif
   if (!hwVoterP)
      return HCG_MNGR__ERR_NULL_PTR;

   OS_LYRG_lockMutex(&hwVoterP->voterMutex);
   //check if unit is already registered
   if ((hwVoterP->voteUnits & (1 << hwUnit)) == 0)
   {
#ifdef VOTERS_DEBUG
      printf("register hwunit %s to voter %s\n",hwUnitP->name, hwVoterP->name);
#endif
      hwVoterP->voteUnits |= (1 << hwUnit);

      //if dynamic voting is not enabled, then once the voters registers a HW, we set it on
      if (!hwVoterP->enDynVoting)
      {
         HCG_MNGRG_voteUnit(hwUnit);
      }
   }
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);   
   return ret;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_voteUnit
*
*  Description: vote for a HW unit. check reference count and call GME to enable
*                    the clock.
*
*  Inputs:
*
*  Outputs: none
*
*  Returns: 1 if the hw unit was turned on in this function call
*
*  Context: 
*
****************************************************************************/
UINT32 HCG_MNGRG_voteUnit( HCG_MNGRG_hwUnitE unit )
{
   UINT32 unitOn = 0;

   if (unit < HCG_MNGRG_MAX_NUM_HW_UNITS)
   {
      HCG_MNGRP_hwUnitsParamT *hwUnitP = &votingParams.hwUnitsDb[unit];
      if (hwUnitP->enableClkGate)
      {
         //if first vote
         OS_LYRG_lockMutex(&hwUnitP->hwUnitMutex);
         if (hwUnitP->refCount == 0)
         {
#ifdef VOTERS_STATISTICS
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
            hwUnitP->totalTimeOffNsec += (time - hwUnitP->timeTurnedOff);
            hwUnitP->timeTurnedOn = time;
#endif

#ifdef VOTERS_DEBUG
            printf("%s on ",hwUnitP->name);
#endif
            GME_DRVG_enableClk(hwUnitP->hwUnit);
            unitOn = 1;
         }
         hwUnitP->refCount++;
         OS_LYRG_unlockMutex(&hwUnitP->hwUnitMutex);
      }
   }
   return unitOn;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_devoteUnit
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
UINT32 HCG_MNGRG_devoteUnit( HCG_MNGRG_hwUnitE unit )
{
   UINT32 unitOff = 0;

   if (unit < HCG_MNGRG_MAX_NUM_HW_UNITS)
   {
      HCG_MNGRP_hwUnitsParamT *hwUnitP = &votingParams.hwUnitsDb[unit];
      if (hwUnitP->enableClkGate)
      {
         OS_LYRG_lockMutex(&hwUnitP->hwUnitMutex);
         if (!hwUnitP->refCount)
         {
            OS_LYRG_unlockMutex(&hwUnitP->hwUnitMutex);
            return unitOff;
         }
         
         //if last vote
         if (hwUnitP->refCount == 1)
         {
#ifdef VOTERS_STATISTICS
            UINT64 time;
            OS_LYRG_getTimeNsec(&time);
            hwUnitP->totalTimeOnNsec += (time - hwUnitP->timeTurnedOn);
            hwUnitP->timeTurnedOff = time;
#endif

#ifdef VOTERS_DEBUG
            printf("%s off ",hwUnitP->name);
#endif
            GME_DRVG_disableClk(hwUnitP->hwUnit);
            unitOff = 1;
         }
         hwUnitP->refCount--;
         OS_LYRG_unlockMutex(&hwUnitP->hwUnitMutex);         
      }
   }
   return unitOff;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_voteUnits
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
UINT32 HCG_MNGRG_voteUnits( UINT32 units )
{
   UINT32 unitsOn = 0;
   int j = 0;

   while(units)
   {
      if ((units & 0x1) == 0x1)
      {
         unitsOn |= (HCG_MNGRG_voteUnit(j) << j);
      }
      units = units >> 1;
      j++;
   }
#ifdef VOTERS_DEBUG
   printf("\n");
#endif
   return unitsOn;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_devoteUnits
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
UINT32 HCG_MNGRG_devoteUnits( UINT32 units )
{
   UINT32 unitsOff = 0;
   int j = 31;

   while(units)
   {
      if ((units & 0x80000000) == 0x80000000)
      {
         unitsOff |= (HCG_MNGRG_devoteUnit(j) << j);;
      }
      units = units << 1;
      j--;
   }
#ifdef VOTERS_DEBUG
   printf("\n");
#endif
   return unitsOff;
}


/****************************************************************************
*
*  Function Name: HCG_MNGRP_processLearnEvent
*
*  Description: Gather channel statistics - process time & fps. Dont use the sensor fps because
*                     its the chip time maybe a little different. After enough samples are gathered,
*                     mark the voter stats as valid, which will allow voting down for it's hw units.
*                     Continue to gather statistics, to encounter small drifts due to temperature.
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
static void HCG_MNGRP_processLearnEvent( HCG_MNGRG_hwEventE event, UINT64 timeNsec )
{
   HCG_MNGRP_paramsT      *paramsP = &votingParams;
   HCG_MNGRP_eventTableT   *tableP;
   Node                    *voterNodeP;
   UINT64                  period;
   UINT32                  numsamples;
   if (paramsP->primePeriod)
      return;

   tableP = &paramsP->startLearnEventsTable[event];
   OS_LYRG_lockMutex(&tableP->listMutex);
   voterNodeP = tableP->voters;
   while(voterNodeP)
   {
      HCG_MNGRP_hwVoterParamT *voterP = (HCG_MNGRP_hwVoterParamT*)voterNodeP->val;
      OS_LYRG_lockMutex(&voterP->voterMutex);
#ifdef MULTIPILE_DOWN_EVENTS
      //set the counter for the required down events
      voterP->downEventsNum = voterP->totalDownEventsNum;
#endif
      if (voterP->activationCounter)
      {
         period = (timeNsec - voterP->currentStartTimeNsec);
         voterP->minFrameDurationNsec = (voterP->minFrameDurationNsec < period) ? voterP->minFrameDurationNsec : period;
      }
      if ((!voterP->statsValid)&&voterP->currentStartTimeNsec)
          voterP->downEventPerFrameCnt = (voterP->downEventPerFrameCnt+1)%voterP->maxFRRVector;
      voterP->currentStartTimeNsec = timeNsec;
      OS_LYRG_unlockMutex(&voterP->voterMutex);
      voterNodeP = voterNodeP->next;
   }
   OS_LYRG_unlockMutex(&tableP->listMutex);

#ifdef MULTIPILE_DOWN_EVENTS
   tableP = &paramsP->downEventsTable[event];
#else
   tableP = &paramsP->endLearnEventsTable[event];
#endif
   OS_LYRG_lockMutex(&tableP->listMutex);
   voterNodeP = tableP->voters;
   while(voterNodeP)
   {
      HCG_MNGRP_hwVoterParamT *voterP = (HCG_MNGRP_hwVoterParamT*)voterNodeP->val;
      OS_LYRG_lockMutex(&voterP->voterMutex);
#ifndef MULTIPILE_DOWN_EVENTS
      if (voterP->currentStartTimeNsec) //check if start has a value, since we can have end event without a start
#else

      if ((!voterP->statsValid)&& (voterP->currentStartTimeNsec))
          voterP->voterDownEventPerFrame[voterP->downEventPerFrameCnt]=voterP->downEventsNum;


      if ((!voterP->downEventsNum) && (voterP->currentStartTimeNsec)) //after all HW's are done, check if start has a value, since we can have end event without a start
#endif
      {
         voterP->currentEndTimeNsec = timeNsec;
         period = voterP->currentEndTimeNsec - voterP->currentStartTimeNsec;
         voterP->activationCounter++;
         voterP->maxHwProcessDurationNsec = (voterP->maxHwProcessDurationNsec > period) ? voterP->maxHwProcessDurationNsec : period;

#ifndef MULTIPILE_DOWN_EVENTS
         if (voterP->activationCounter == HCG_MNGRP_LEARNING_NUM_SAMPLES)
#else
         if (voterP->maxFRRVector > 1)
             numsamples = HCG_MNGRP_LEARNING_NUM_SAMPLES*2;
         else numsamples=HCG_MNGRP_LEARNING_NUM_SAMPLES;
         if ((!voterP->statsValid) && (voterP->activationCounter == numsamples))
#endif
         {
#ifdef VOTERS_DEBUG            
            printf("voter %s stats are now valid. Process time[ns]: max %llu. Fps time[ns]: min %llu\n",
                   voterP->name,voterP->maxHwProcessDurationNsec,voterP->minFrameDurationNsec);
#endif
            voterP->statsValid = 1;
            voterP->downEventPerFrameCnt = (voterP->downEventPerFrameCnt+1)%voterP->maxFRRVector;

         }
#ifdef MULTIPILE_DOWN_EVENTS
         else
         {
            //voter is not valid yet, there is no up event running
            voterP->downEventsNum = voterP->totalDownEventsNum;
         }
#endif
      }
      OS_LYRG_unlockMutex(&voterP->voterMutex);
      voterNodeP = voterNodeP->next;
   } 
   OS_LYRG_unlockMutex(&tableP->listMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_registerStartEvent
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
void HCG_MNGRG_registerStartEvent(HCG_MNGRG_voterHandle handle, HCG_MNGRG_hwEventE event )
{
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
   OS_LYRG_lockMutex(&hwVoterP->voterMutex);
   //allow only one event
   if (hwVoterP->startEvent == HCG_MNGRG_HW_EVENT_INVALID_E)
   {   
      hwVoterP->startEvent = event;
      HCG_MNGRG_addVoterToEventTable(handle, &votingParams.startLearnEventsTable[event]);
#ifdef VOTERS_DEBUG   
      printf("voter %s start event %s\n",hwVoterP->name,HCG_MNGRP_hwEventToString(event));
#endif
   }
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_registerEndEvent
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
void HCG_MNGRG_registerEndEvent(HCG_MNGRG_voterHandle handle, HCG_MNGRG_hwEventE event )
{
#ifndef MULTIPILE_DOWN_EVENTS
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;

   if (!hwVoterP)
      return;

   OS_LYRG_lockMutex(&hwVoterP->voterMutex);
   //allow only one event
   if (hwVoterP->endEvent == HCG_MNGRG_HW_EVENT_INVALID_E)
   {    
      hwVoterP->endEvent = event;
      HCG_MNGRG_addVoterToEventTable(handle, &votingParams.endLearnEventsTable[event]);
#ifdef VOTERS_DEBUG
      printf("voter %s end event %s\n",hwVoterP->name,HCG_MNGRP_hwEventToString(event));
#endif  
   }
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);
#else
   (void)handle;
   (void)event;
#endif
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_snsrEventWrapper
*
*  Description: registered cb in IAE drive, call process event with [us]
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
static void HCG_MNGRG_snsrEventWrapper(UINT64 timestamp, UINT32 slu, void *argP)
{
   (void)argP;
#ifdef TIMER_STATS
   printf("SLU%d: actual diff: %llu, designated diff: %llu\n",slu,timestamp - votingParams.actualTimeExpiredNsec[0],timestamp - votingParams.designatedTimeNsec[0]);
#endif
   HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_START_ISR_E + slu, timestamp, 1);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_processEvent
*
*  Description: Send to learn event function, and then traverse over the voters, and 
*                    check which has this event as up/down. Act if needed.
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
void HCG_MNGRG_processEvent( HCG_MNGRG_hwEventE event, UINT64 timeNsec, UINT32 numEvents )
{
   HCG_MNGRP_paramsT      *paramsP = &votingParams;
   HCG_MNGRP_eventTableT   *tableP;
#if 0 
   Node                    *voterNodeP;
   ERRG_codeE              ret = HCG_MNGR__RET_SUCCESS;
#ifdef PROCESS_EVENT_STATS
   UINT64 nsecStart;
   UINT64 nsecEnd;
   UINT64 period;
   OS_LYRG_getTimeNsec(&nsecStart);
#endif

#ifdef VOTERS_DEBUG
   UINT64 nsecStart,nsecEnd;
   OS_LYRG_getTimeNsec(&nsecStart);
#endif

#ifndef MULTIPILE_DOWN_EVENTS
   HCG_MNGRP_processLearnEvent(event,timeNsec);
#endif

   //for up events, we do not care if statistics are valid, since prime might set as invalid
   tableP = &paramsP->upEventsTable[event];
   OS_LYRG_lockMutex(&tableP->listMutex);   
   voterNodeP = tableP->voters;
   while(voterNodeP)
   {
     HCG_MNGRP_hwVoterParamT *voterP = (HCG_MNGRP_hwVoterParamT*)voterNodeP->val;
      OS_LYRG_lockMutex(&voterP->voterMutex);
      if (!voterP->votedUnits)
      {
#ifdef VOTERS_DEBUG
         printf("%s: ",voterP->name);
#endif
         HCG_MNGRG_voteUnits(voterP->voteUnits);
         voterP->votedUnits = voterP->voteUnits;
      }
      OS_LYRG_unlockMutex(&voterP->voterMutex);
      voterNodeP = voterNodeP->next;
   }
   OS_LYRG_unlockMutex(&tableP->listMutex);

   tableP = &paramsP->downEventsTable[event];
   OS_LYRG_lockMutex(&tableP->listMutex);
   voterNodeP = tableP->voters;
   while(voterNodeP)
   {
      HCG_MNGRP_hwVoterParamT *voterP = (HCG_MNGRP_hwVoterParamT*)voterNodeP->val;
      OS_LYRG_lockMutex(&voterP->voterMutex);

#ifdef MULTIPILE_DOWN_EVENTS
      //decrease the number of down events, if zero then we can close the HW's
      voterP->downEventsNum -= numEvents;
      //if this is the last down end event, and stats are valid, we can invoke timer
      if ((voterP->statsValid) && (voterP->downEventsNum == voterP->voterDownEventPerFrame[voterP->downEventPerFrameCnt]))
#else
      //if this is down end event, and stats are valid, we can invoke timer (or channel without timer) and devote HW units
      if ((voterP->statsValid) || (voterP->sensorIndex == HCG_MNGRP_INVALID_SENSOR_INDEX))
#endif
      {
         voterP->downEventPerFrameCnt = (voterP->downEventPerFrameCnt+1)%voterP->maxFRRVector;
         ret = HCG_MNGRP_invokeTimer(voterP,timeNsec);
         //if timer invoked, close HW units. 
         if(ERRG_SUCCEEDED(ret) && (voterP->votedUnits))
         {
   #ifdef VOTERS_DEBUG
            printf("%s: ",voterP->name);
   #endif
            OS_LYRG_usleep(500); //TODO - need to investigate with VLSI team why interleaved channels require delay before closer
            HCG_MNGRG_devoteUnits(voterP->voteUnits);
            voterP->votedUnits = 0;
         }
      }
      OS_LYRG_unlockMutex(&voterP->voterMutex);
      voterNodeP = voterNodeP->next;
   }   
   OS_LYRG_unlockMutex(&tableP->listMutex);   

#ifdef MULTIPILE_DOWN_EVENTS
   HCG_MNGRP_processLearnEvent(event,timeNsec);
#endif

#ifdef VOTERS_DEBUG
   OS_LYRG_getTimeNsec(&nsecEnd);
   if ((event > HCG_MNGRG_HW_EVENT_IAE_SLU5_FRAME_START_ISR_E) || (event < HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_START_ISR_E)) //ignore start events, to reduce log size
      printf("%llu: %s at %llu, ( %llu )\n",nsecEnd,HCG_MNGRP_hwEventToString(event),timeNsec,nsecEnd-nsecStart);
#endif

#ifdef PROCESS_EVENT_STATS
   OS_LYRG_getTimeNsec(&nsecEnd);
   period = nsecEnd - nsecStart;
   paramsP->sumEventTimeNsec[event]+= period;
   paramsP->eventCounter[event]++;
   paramsP->aveEventTimeNsec[event] = paramsP->sumEventTimeNsec[event] / paramsP->eventCounter[event];
   if (period > paramsP->maxEventTimeNsec[event])
      paramsP->maxEventTimeNsec[event] = period;
   if (period < paramsP->minEventTimeNsec[event])
      paramsP->minEventTimeNsec[event] = period;
#endif
#endif
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_startVoterWithTimer
*
*  Description: start a voter. The clock for the HW units registered to this voter will 
*                    be active until the learning period is over. The HCG will create internal
*                    timer for this voter, which will make the up event
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
void HCG_MNGRG_startVoterWithTimer( HCG_MNGRG_voterHandle handle, UINT32 sensorIndex )
{
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
#ifndef UNIFY_TIMER
   (void)sensorIndex;
   HCG_MNGRG_registerHwUnitUpEvent(hwVoterP, HCG_MNGRG_HW_EVENT_SENSOR_TIMER_1_E + hwVoterP->index);
#else
   if (sensorIndex > HCG_MNGRP_MAX_NUM_PRE_READOUT_TIMERS)
   {
      assert(0);
   }

   //find the timer for the sensor source of this voter, and register to it
   HCG_MNGRG_registerHwUnitUpEvent(hwVoterP, HCG_MNGRG_HW_EVENT_SENSOR_TIMER_1_E + sensorIndex);
   hwVoterP->sensorIndex = sensorIndex;
#ifdef VOTERS_DEBUG
   printf("Start voter %p %s, sensorIndex: %d, units 0x%x\n",handle,hwVoterP->name,sensorIndex,hwVoterP->voteUnits);
#endif
#endif

   //one time vote - until stats are learned.
#ifdef MULTIPILE_DOWN_EVENTS
   hwVoterP->downEventsNum = hwVoterP->totalDownEventsNum;
#endif  
   HCG_MNGRG_voteUnits(hwVoterP->voteUnits);
   OS_LYRG_lockMutex(&hwVoterP->voterMutex);   
   hwVoterP->votedUnits = hwVoterP->voteUnits;
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);   
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_startVoter
*
*  Description: start a voter. The clock for the HW units registered to this voter will 
*                    be active until the learning period is over. 
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
void HCG_MNGRG_startVoter( HCG_MNGRG_voterHandle handle )
{
   HCG_MNGRP_hwVoterParamT *hwVoterP = (HCG_MNGRP_hwVoterParamT*)handle;
   hwVoterP->sensorIndex = HCG_MNGRP_INVALID_SENSOR_INDEX;
#ifdef VOTERS_DEBUG
   printf("Start voter %p %s, units 0x%x\n",handle,hwVoterP->name,hwVoterP->voteUnits);
#endif

   //one time vote - until stats are learned.
#ifdef MULTIPILE_DOWN_EVENTS
   hwVoterP->downEventsNum = hwVoterP->totalDownEventsNum;
#endif  
   HCG_MNGRG_voteUnits(hwVoterP->voteUnits);
   OS_LYRG_lockMutex(&hwVoterP->voterMutex);
   hwVoterP->votedUnits = hwVoterP->voteUnits;
   OS_LYRG_unlockMutex(&hwVoterP->voterMutex);
}


/****************************************************************************
*
*  Function Name: HCG_MNGRG_showStats
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
void HCG_MNGRG_showStats()
{
#ifdef VOTERS_STATISTICS
   HCG_MNGRP_paramsT      *paramsP = &votingParams;
   UINT32 i;
   UINT64 nsec;
   OS_LYRG_getTimeNsec(&nsec);

   printf("\n%llu: HW units stats:\n",nsec);
   for ( i = 0; i < HCG_MNGRG_MAX_NUM_HW_UNITS; i++)
   {
      OS_LYRG_lockMutex(&paramsP->hwUnitsDb[i].hwUnitMutex);
      OS_LYRG_getTimeNsec(&nsec);

      //first, fill data to this moment
      if (paramsP->hwUnitsDb[i].refCount)
      {    
         //unit is on
         paramsP->hwUnitsDb[i].totalTimeOnNsec += nsec - paramsP->hwUnitsDb[i].timeTurnedOn;
      }
      else
      {
         //unit is off
         paramsP->hwUnitsDb[i].totalTimeOffNsec += nsec - paramsP->hwUnitsDb[i].timeTurnedOff;      
      }
      
      printf("Unit %s: total time on %llu, total time off %llu, precent on %f\n",
               paramsP->hwUnitsDb[i].name,paramsP->hwUnitsDb[i].totalTimeOnNsec,paramsP->hwUnitsDb[i].totalTimeOffNsec, 
               100 * (double)paramsP->hwUnitsDb[i].totalTimeOnNsec / (double)(paramsP->hwUnitsDb[i].totalTimeOnNsec + paramsP->hwUnitsDb[i].totalTimeOffNsec)  );

      paramsP->hwUnitsDb[i].totalTimeOnNsec = 0;
      paramsP->hwUnitsDb[i].totalTimeOffNsec = 0;
      
      OS_LYRG_unlockMutex(&paramsP->hwUnitsDb[i].hwUnitMutex);      
   }

   printf("Voters stats:\n");
   for( i = 0; i < HCG_MNGRP_MAX_NUM_HW_VOTERS; i++)
   {
      OS_LYRG_lockMutex(&paramsP->votersDb[i].voterMutex);
      if (paramsP->votersDb[i].used)
      {
         printf("Voter %s: valid %d(%lld/%d), Process time[ns]: max %llu. Fps time[ns]: min %llu\n",
                paramsP->votersDb[i].name,
                paramsP->votersDb[i].statsValid,
                paramsP->votersDb[i].activationCounter,
                HCG_MNGRP_LEARNING_NUM_SAMPLES,
                paramsP->votersDb[i].maxHwProcessDurationNsec,
                paramsP->votersDb[i].minFrameDurationNsec);
      }
      OS_LYRG_unlockMutex(&paramsP->votersDb[i].voterMutex);      
   }
#ifdef PROCESS_EVENT_STATS
   for (i = 0; i < HCG_MNGRG_HW_EVENT_MAX_NUM_E; i++)
   {
      if (paramsP->eventCounter[i])
      {
         printf("%s (%llu): ave %llu, max %llu, min %llu\n",HCG_MNGRP_hwEventToString(i),paramsP->eventCounter[i],paramsP->aveEventTimeNsec[i],paramsP->maxEventTimeNsec[i],paramsP->minEventTimeNsec[i]);
      }
   }
#endif
#endif
}

#ifdef __cplusplus
   }
#endif

