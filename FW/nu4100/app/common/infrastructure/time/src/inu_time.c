/****************************************************************************
 *
 *   FileName: time.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: INU time synchronization module.
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include "inu_common.h"
#include "inu2.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "serial.h"
#if DEFSG_IS_GP
#include <unistd.h>
#include "gpio_drv.h"
#include "ispi.h"
#include "rtc.h"
#include <errno.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/timex.h>
#include <math.h>
#endif


/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define READ_MODE    0x80
#define WRITE_MODE   0x7F

#define SET_READ_MODE(_buf) (_buf |= READ_MODE)
#define SET_WRITE_MODE(_buf) (_buf &= READ_MODE)

#define TIMEP_SYNCED_NOTIFY

#define ADDR_CHIP_ID  (0x00)
#define ADDR_TS_63_56 (0x01)
#define ADDR_TS_55_48 (0x02)
#define ADDR_TS_47_40 (0x03)
#define ADDR_TS_39_32 (0x04)
#define ADDR_TS_31_24 (0x05)
#define ADDR_TS_23_16 (0x06)
#define ADDR_TS_15_8  (0x07)
#define ADDR_TS_7_0   (0x08)
#ifdef TIMEP_SYNCED_NOTIFY
#define ADDR_SYNCED   (0x09)
#define TIMEP_CHECK_SYNC_INTERVAL   (1000000) // 1 sec in [uSec]
#define TIMEP_SYNCED_SLEEP_TIME     (1000000) // 1 sec in [uSec]
#define TIMEP_UNSYNCED_SLEEP_TIME   (5000)    // 5 mSec
#define TIMEP_HOST_TARGET_SYNC_DIFF (5000)    // 5 mSec

#define TIMEP_ACHIEVED_SYNC (1)
#define TIMEP_OUT_OF_SYNC   (0xFF)
#define TIMEP_INIT_SYNC     (0xAC)
#endif

#define NU_CHIP_ID    (0xCA)
#define START_PACKET  (ADDR_TS_63_56)
#define PACKET_SIZE   (16)
#define TIMEP_CHIP_ID_READ_MAX_TRY (10)
#ifndef MIN
    #define MIN(A,B) ((A)<(B) ? (A):(B))
#endif


#define USEC_PER_SEC                   (1000000)
#define MSEC_PER_SEC                   (1000)
#define TIMEP_OFFSET_AVE_PERIOD        (2000)
#define TIMEP_TICK_THRESHOLD_UPDATE    (0.0001)

#define TIMEP_ABS_DIFF_THRESHOLD_FIX   (4000)
#define TIMEP_FINE_TUNING_THRESHOLD    (10)

//CPOL = 1, CPHA = 1
#define TIMEP_SPI_SLAVE_MODE  (0x7)
#define TIMEP_SPI_MASTER_MODE (0x0)
#define TIMEP_SPI_SPEED       (1000000)

#define TIMEP_MOVING_AVERAGE_PERIOD (10)

//#define STD_DEV_CALC

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef enum
{
   TIMEP_STATE_NOT_ACTIVE,
   TIMEP_STATE_ACTIVE
} TIMEP_stateT;

typedef struct
{
   inu_device__set_time_t     setTimeParams;
   OS_LYRG_threadHandle       thrH;
   TIMEP_stateT               state;
   BOOL                       threadActive;
   PUART                      uartH;

   //process timestamp params
   UINT64                     lastTimestamp;          //previous host timestamp
   uint64_t                   last_usec;              //previous local timestamp
   long double                machine_uSecDiff_sum;   //local deltas sum
   long double                remote_uSecDiff_sum;    //host deltas sum
   unsigned int               samples;
   unsigned int               ave_period;
   int                        first_time;             //used for initializing
   long                       tick;                   //current ticks per usec
   //moving average params
   long double                tickHistory[TIMEP_MOVING_AVERAGE_PERIOD];    //moving average for tick calculation
   int                        len;                    //moving average array length
   int                        pos;                    //position in moving average array
   long double                newAvg;                 //average result
   long double                sum;                    //sum of all moving average elements
   int                        tickMovingAveValid;     //used for indicating when moving average is valid
#ifdef TIMEP_SYNCED_NOTIFY
   UINT32                     syncedNotify;           //indicate that the target is synced to the host time
#endif
   //stats
   BOOL                       enableStats;
   inu_device__getUsecTimeFuncT* getUsecTimeFunc;
#ifdef STD_DEV_CALC
   uint64_t                   localDeltaSamples[TIMEP_OFFSET_AVE_PERIOD];
   uint64_t                   remoteDeltaSamples[TIMEP_OFFSET_AVE_PERIOD];
   uint64_t                   deltaSamples[TIMEP_OFFSET_AVE_PERIOD];
   uint64_t                   localSamplesSum;
   uint64_t                   remoteSamplesSum;
   uint64_t                   deltaSamplesSum;
#endif
} TIMEP_infoT;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static TIMEP_infoT TIMEP_params;

 /****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: TIMEP_uartModeStart
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static ERRG_codeE TIMEP_uartModeStart( TIMEP_infoT *paramsP )
{
#if DEFSG_IS_GP
   sprintf(paramsP->setTimeParams.uartPortName,"%d", 1);
#endif
   return SERIALG_open(&paramsP->uartH, paramsP->setTimeParams.uartPortName, paramsP->setTimeParams.uartBaudRate, SERIALG_TYPE_UART);
}

/****************************************************************************
*
*  Function Name: TIMEP_uartModeStop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static ERRG_codeE TIMEP_uartModeStop( TIMEP_infoT *paramsP )
{
   return SERIALG_close(&paramsP->uartH);
}


#if DEFSG_IS_GP
#if DEFSG_IS_GP_LINUX
/****************************************************************************
*
*  Function Name: TIMEP_change_tick
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static int TIMEP_change_tick(long tick)
{
   static struct timex buf = {0};

   buf.tick   = tick;
   buf.modes  = ADJ_TICK;

   return adjtimex(&buf);
}


/****************************************************************************
*
*  Function Name: TIMEP_change_pll_offset
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static int TIMEP_change_pll_offset(long offset)
{
   static struct timex buf = {0};

   buf.offset = offset;

   buf.modes  = ADJ_OFFSET | ADJ_STATUS;
   buf.status = STA_PLL | STA_FLL;

   return adjtimex(&buf);
}


/****************************************************************************
*
*  Function Name: TIMEP_movingAvg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
long double TIMEP_movingAvg(long double *ptrArrNumbers, long double *ptrSum, int pos, int len, long double nextNum)
{
  //Subtract the oldest number from the prev sum, add the new number
  *ptrSum = *ptrSum - ptrArrNumbers[pos] + nextNum;
  //Assign the nextNum to the position in the array
  ptrArrNumbers[pos] = nextNum;
  //return the average
  return ((long double)(*ptrSum)) / ((long double)len);
}


#ifdef STD_DEV_CALC
/****************************************************************************
*
*  Function Name: TIMEP_squareDistanceToMean
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static long double TIMEP_squareDistanceToMean(uint64_t sample, long double mean)
{
   long double distance;
   distance = ABS((long double)sample - mean);

   return distance*distance;
}
#endif


/****************************************************************************
*
*  Function Name: TIMEP_processNewTimeSample
*
*  Description: main function for calculating freq and phase, and performing the correction
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static void TIMEP_processNewTimeSample(TIMEP_infoT *paramsP, UINT64 timestamp)
{
   UINT64 current_usec, remoteTimestamp_usec, lastRemoteTimestamp_usec;
   long double rateDifferenceAverage;
   struct timespec tv;

   clock_gettime(CLOCK_REALTIME, &tv);
   current_usec = (((UINT64)tv.tv_sec) * 1000000) + (((uint64_t)tv.tv_nsec) / 1000);

   remoteTimestamp_usec = timestamp * TIMEP_params.setTimeParams.secondHostResolution;
   lastRemoteTimestamp_usec = paramsP->lastTimestamp * TIMEP_params.setTimeParams.secondHostResolution;

   if (paramsP->first_time)
   {
      TIMEP_change_tick(paramsP->tick);

      tv.tv_sec = (long)(remoteTimestamp_usec / USEC_PER_SEC);
      tv.tv_nsec = (remoteTimestamp_usec % USEC_PER_SEC) * 1000;
      if (clock_settime(CLOCK_REALTIME, &tv) < 0)
      {
         printf("clock_settime failed!\n");
      }
      else
      {
         clock_gettime(CLOCK_REALTIME, &tv);
         current_usec = ((uint64_t)tv.tv_sec * 1000000) + ((uint64_t)tv.tv_nsec / 1000);
      }
      paramsP->first_time = 0;
      goto lastTimestamp_notValid;
   }

   //apply freq correction using kernel adjtimex
   TIMEP_change_pll_offset((long)(remoteTimestamp_usec - current_usec));

#ifdef STD_DEV_CALC
   if (current_usec > paramsP->last_usec)
      paramsP->localDeltaSamples[paramsP->samples] =  current_usec - paramsP->last_usec;
   else
      paramsP->localDeltaSamples[paramsP->samples] =  paramsP->last_usec - current_usec;
   paramsP->localSamplesSum = paramsP->localSamplesSum + paramsP->localDeltaSamples[paramsP->samples];


   if (remoteTimestamp_usec > lastRemoteTimestamp_usec)
      paramsP->remoteDeltaSamples[paramsP->samples] =  remoteTimestamp_usec - lastRemoteTimestamp_usec;
   else
      paramsP->remoteDeltaSamples[paramsP->samples] =  lastRemoteTimestamp_usec - remoteTimestamp_usec;
   paramsP->remoteSamplesSum = paramsP->remoteSamplesSum + paramsP->remoteDeltaSamples[paramsP->samples];


   if (remoteTimestamp_usec > current_usec)
      paramsP->deltaSamples[paramsP->samples] =  remoteTimestamp_usec - current_usec;
   else
      paramsP->deltaSamples[paramsP->samples] =  current_usec - remoteTimestamp_usec;
   paramsP->deltaSamplesSum = paramsP->deltaSamplesSum + paramsP->deltaSamples[paramsP->samples];
#endif

   paramsP->machine_uSecDiff_sum += (current_usec - paramsP->last_usec);
   paramsP->remote_uSecDiff_sum  += (remoteTimestamp_usec - lastRemoteTimestamp_usec);
   paramsP->samples++;
   if (paramsP->samples == paramsP->ave_period)
   {
      int64_t momentaryDiff = ((int64_t)current_usec) - ((int64_t)remoteTimestamp_usec);
      long newTick;

      rateDifferenceAverage = paramsP->machine_uSecDiff_sum / paramsP->remote_uSecDiff_sum;

#ifdef UPDATE_TICK
      paramsP->newAvg = TIMEP_movingAvg(paramsP->tickHistory, &paramsP->sum, paramsP->pos, paramsP->len, (paramsP->tick / rateDifferenceAverage));
      paramsP->pos++;
      if (paramsP->pos >= paramsP->len)
      {
         paramsP->pos = 0;
         paramsP->tickMovingAveValid = 1;
      }

      newTick = (long)round(paramsP->newAvg);
#endif

#ifdef STD_DEV_CALC
      float p2pdiff_ave = (paramsP->machine_uSecDiff_sum - paramsP->remote_uSecDiff_sum) / paramsP->samples;

      unsigned int i = 0;
      long double localSquareDis = 0, remoteSquareDis = 0, p2pSquareDis = 0;
      long double localSamplesMean,remoteSamplesMean, deltaSamplesMean;
      localSamplesMean = paramsP->localSamplesSum / paramsP->samples;
      remoteSamplesMean = paramsP->remoteSamplesSum / paramsP->samples;
      deltaSamplesMean = paramsP->deltaSamplesSum / paramsP->samples;


      for (i = 0; i < paramsP->ave_period; i++)
      {
         localSquareDis = localSquareDis + TIMEP_squareDistanceToMean(paramsP->localDeltaSamples[i], localSamplesMean);
         remoteSquareDis = remoteSquareDis + TIMEP_squareDistanceToMean(paramsP->remoteDeltaSamples[i], remoteSamplesMean);
         p2pSquareDis = p2pSquareDis + TIMEP_squareDistanceToMean(paramsP->deltaSamples[i], deltaSamplesMean);
      }

      localSquareDis = localSquareDis / paramsP->samples;
      remoteSquareDis = remoteSquareDis / paramsP->samples;
      p2pSquareDis = p2pSquareDis / paramsP->samples;
      paramsP->localSamplesSum = 0;
      paramsP->remoteSamplesSum = 0;
      paramsP->deltaSamplesSum = 0;

      printf("%llu : remote time: %lld (p2pdiff_ave = %lf momentaryDiff = %lld ). rateDifferenceAverage = %Lf ( %Lf %Lf ) current tick = %ld tickSample = %Lf newAvg = %Lf tick to set = %lu, local stdv = %lf, remote stdv = %lf, p2p stdv = %lf \n",
               current_usec, remoteTimestamp_usec, p2pdiff_ave,momentaryDiff,rateDifferenceAverage,paramsP->machine_uSecDiff_sum,paramsP->remote_uSecDiff_sum,paramsP->tick,(paramsP->tick / rateDifferenceAverage),paramsP->newAvg,newTick,sqrt(localSquareDis),sqrt(remoteSquareDis),sqrt(p2pSquareDis));
#else
      if (paramsP->enableStats)
      {
         printf("%llu : remote time: %lld (momentaryDiff = %lld ). rateDifferenceAverage = %Lf ( %Lf %Lf )\n",
                  current_usec, remoteTimestamp_usec, momentaryDiff,rateDifferenceAverage,paramsP->machine_uSecDiff_sum,paramsP->remote_uSecDiff_sum);
      }
#endif

      if ((rateDifferenceAverage > (1 + TIMEP_TICK_THRESHOLD_UPDATE)) || (rateDifferenceAverage < (1 - TIMEP_TICK_THRESHOLD_UPDATE)))
      {
#ifdef UPDATE_TICK
         if ((paramsP->tickMovingAveValid) && (newTick != paramsP->tick))
         {
            if (ABS(newTick - paramsP->tick) < TIMEP_FINE_TUNING_THRESHOLD)
            {
               if (newTick > paramsP->tick)
                  newTick = paramsP->tick+1;
               else
                  newTick = paramsP->tick-1;
            }

            if (TIMEP_change_tick(newTick) < 0)
            {
               printf("%llu: adjtimex failed1. newTick = %ld %s\n", current_usec,newTick,strerror(errno));
            }
            else
            {
               paramsP->tick = newTick;
            }
         }
#endif
#ifdef TIMEP_SYNCED_NOTIFY
         paramsP->syncedNotify = TIMEP_OUT_OF_SYNC;
#endif
      }
      else
      {
#ifdef TIMEP_SYNCED_NOTIFY
         if ((momentaryDiff > -TIMEP_HOST_TARGET_SYNC_DIFF) && (momentaryDiff < TIMEP_HOST_TARGET_SYNC_DIFF))
         {
            paramsP->syncedNotify = TIMEP_ACHIEVED_SYNC;
         }
         else
         {
            paramsP->syncedNotify = TIMEP_OUT_OF_SYNC;
         }
#endif
         if ((momentaryDiff > TIMEP_ABS_DIFF_THRESHOLD_FIX) || (momentaryDiff < -TIMEP_ABS_DIFF_THRESHOLD_FIX))
         {

            tv.tv_sec = (long)(remoteTimestamp_usec / USEC_PER_SEC);
            tv.tv_nsec = (remoteTimestamp_usec % USEC_PER_SEC) * 1000;
            if (clock_settime(CLOCK_REALTIME, &tv) < 0)
            {
               printf("clock_settime failed!\n");
            }
            else
            {
               current_usec = remoteTimestamp_usec;
            }
         }
      }

      paramsP->samples = 0;
      paramsP->machine_uSecDiff_sum = 0;
      paramsP->remote_uSecDiff_sum = 0;
   }
   lastTimestamp_notValid:
   paramsP->last_usec = current_usec;
   paramsP->lastTimestamp = timestamp;
}
#endif


/****************************************************************************
*
*  Function Name: TIMEP_spiModeStart
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static ERRG_codeE TIMEP_spiModeStart( )
{
   ERRG_codeE ret;
   GPIO_DRVG_gpioSetValParamsT    valParams;

   /* SPI in slave mode receives the chip select on line 0. This line is also connected to the flash.
       gpio 6 is used to toggle external switch, which will toggle between the flash and slave lines.*/
   valParams.val   = GPIO_DRVG_GPIO_STATE_CLEAR_E;
   valParams.gpioNum = GPIO_DRVG_GPIO_6_E;
   ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &valParams);

   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "set gpio failed\n");
      return ret;
   }

   /* change SPI to slave mode */
   ret = SPI_DRVG_setupMode (TIMEP_SPI_SLAVE_MODE);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "set slave mode\n");
      return ret;
   }

   ret = SPI_DRVG_setupSpeed(TIMEP_SPI_SPEED);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "set speed failed\n");
      return ret;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: TIMEP_spiModeStop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*  Success or specific error code.
*
*  Context:
*
****************************************************************************/
static ERRG_codeE TIMEP_spiModeStop( )
{
   ERRG_codeE ret;
   GPIO_DRVG_gpioSetValParamsT     valParams;

   /* change SPI to master mode */
   SPI_DRVG_setupMode(TIMEP_SPI_MASTER_MODE);
   valParams.val     = GPIO_DRVG_GPIO_STATE_SET_E;
   valParams.gpioNum = GPIO_DRVG_GPIO_6_E;
   ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &valParams);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "set gpio failed\n");
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: TIMEP_spiThread
*
*  Description: in the b600 project, timestamps are received from the SPI (nu3000 is set as slave).
*                    timestamps struct:
*
*                    address:   val:
*                    0x01        timestamp[31:24]
*                    0x02        timestamp[23:16]
*                    0x03        timestamp[15:8]
*                    0x04        timestamp[7:0]
*
*                    The spi is configured to slave mode, and we use a thread to listen for new timestamps.
*                    The timestamps are saved, and will then be encoded into the video injected files, so it can
*                    be decoded from the disparity output.

*
*  Inputs:

*  Outputs:
*
*  Returns:
*
*  Context: generic services procedures
*
****************************************************************************/
static int TIMEP_spiThread(void *argP)
{
   TIMEP_infoT *paramsP = argP;
   ERRG_codeE ret;

   UINT8 rxBuf[20] = {0};
   UINT32 timestampL;
   int i,cnt, lostSync = 0, chipId = 0;

   ret = TIMEP_spiModeStart();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "spi start failed, exit\n");
      return -1;
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "spi listener thread active\n");
   paramsP->threadActive = TRUE;


   while(paramsP->state == TIMEP_STATE_ACTIVE)
   {
      i = 0;

      /* read until we receive 8 bytes of data */
      while((i < 8) && (paramsP->state == TIMEP_STATE_ACTIVE))
      {
         cnt = 8 - i;
         SPI_DRVG_read((BYTE*)&rxBuf[i],(UINT32*)&cnt);
         i += cnt;
      }

      timestampL = 0;

      if (rxBuf[0] == READ_MODE)
      {
         LOGG_PRINT(LOG_INFO_E, NULL,"received chipid on spi\n");
         chipId = 1;
      }

      for(i = 0; (i < 4) && (paramsP->state == TIMEP_STATE_ACTIVE); i++)
      {
         /* check the address fields. we use them to verify the spi transfer */
         if (rxBuf[i * 2] != (i+0x01))
         {
            /* ts not synced! , keep reading from spi until we resync. mark this event (lostSync) so we will keep the previous timestamp */
            //printf("TS NOT SYNCED! rxBuf[%d] = 0x%x, cnt = %d\n",i * 2,rxBuf[i * 2],cnt);
            do
            {
               cnt = 2;
               SPI_DRVG_read((BYTE*)rxBuf,(UINT32*)&cnt);
               lostSync = 1;
            }while((rxBuf[0] != 0x4) && (cnt == 2) && (paramsP->state == TIMEP_STATE_ACTIVE));
         }
         timestampL |= (rxBuf[(i*2) + 1] << (8*(3-i)));
      }

      if (!lostSync)
      {
#if DEFSG_IS_GP_LINUX
         TIMEP_processNewTimeSample(paramsP, (UINT64)timestampL);
#else
         spiTimestamp = timestampL;
#endif
   //         if ((spiTimestamp & 0xFF) == 0)
   //            printf("%d\n",spiTimestamp);
      }
      else
      {
         if (!chipId)
         {
            LOGG_PRINT(LOG_WARN_E, NULL, "TS NOT SYNCED!\n");
         }
      }
      lostSync = 0;
      chipId = 0;
   }

   ret = TIMEP_spiModeStop();
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "spi stop failed\n");
      return -1;
   }

   paramsP->threadActive = 0;
   LOGG_PRINT(LOG_INFO_E, NULL, "spi listener thread deactive\n");
   return 0;
}

/****************************************************************************
*
*  Function Name: TIMEP_rtcThread
*
*  Description: Timestamps are received from the RTC.
*                    timestamps struct: timespec
*
*                    address:   val:
*                    tv_sec     holds the Seconds part
*                    tv_nsec    holds the fraction seconds part
*
*  Inputs:

*  Outputs:
*
*  Returns:
*
*  Context: generic services procedures
*
****************************************************************************/
static int TIMEP_rtcThread(void *argP)
{
   TIMEP_infoT *paramsP = argP;
   ERRG_codeE ret;

   int i,cnt, lostSync = 0;
   RTC_DRVG_RtcTimeT rtcTimeStamp;
   UINT64 timestampL;

   LOGG_PRINT(LOG_INFO_E, NULL, "RTC listener thread active\n");
   paramsP->threadActive = TRUE;

   while(paramsP->state == TIMEP_STATE_ACTIVE)
   {
      // Get current RTC time
      RTC_DRVG_getTime(&rtcTimeStamp);
      timestampL = ((UINT64)(rtcTimeStamp.seconds + RTC_DRVG_coarseTimeGet()) * USEC_PER_SEC) 
                     + (((UINT64)rtcTimeStamp.fracSeconds) / MSEC_PER_SEC);

      TIMEP_processNewTimeSample(paramsP, timestampL);

      if(paramsP->syncedNotify){
         // Time has synced, Read RTC in 10 second interval when synced
         sleep(10);
      }
      else{
         // not synced, read RTc every 100ms interval
         usleep(100000);
      }
   }

   paramsP->threadActive = 0;
   LOGG_PRINT(LOG_INFO_E, NULL, "RTC listener thread deactive\n");
   return 0;
}


/****************************************************************************
*
*  Function Name: TIMEP_uartThread
*
*  Description:
*
*  Inputs:

*  Outputs:
*
*  Returns:
*
*  Context: generic services procedures
*
****************************************************************************/
static int TIMEP_uartThread(void *argP)
{
   TIMEP_infoT *paramsP = argP;
   ERRG_codeE ret;

   UINT8 rxBuf[20] = {0};
   UINT8 txBuf[20] = {0};
   UINT64 timestamp = 0;
   UINT32 i,cnt, lostSync = 0;

   ret = TIMEP_uartModeStart(paramsP);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "uart start failed, exit\n");
      return -1;
   }

   paramsP->threadActive = TRUE;
   // Initialize syncedNotify to INIT_SYNC (0xAC)
   paramsP->syncedNotify = TIMEP_INIT_SYNC;

   while(paramsP->state == TIMEP_STATE_ACTIVE)
   {
      i = 0;
      cnt = 1;

      SERIALG_recv(&paramsP->uartH,&rxBuf[i],cnt,&cnt,5);

      if (cnt == 0)
      {
         //timeout
         continue;
      }

      //check if chipID requested
      if ((cnt >= 1) && (rxBuf[i] == READ_MODE))
      {
         printf("CHIPID request, send response\n");

         txBuf[0] = NU_CHIP_ID;
         SERIALG_send(&paramsP->uartH,(INT8*)txBuf,1,0);
         continue;
      }

#ifdef TIMEP_SYNCED_NOTIFY
      //check if sync notify requested
      if ((cnt >= 1) && (rxBuf[i] == (READ_MODE | ADDR_SYNCED)))
      {
         txBuf[0] = paramsP->syncedNotify;
         if (paramsP->syncedNotify !=  TIMEP_ACHIEVED_SYNC)
         {
            printf("sync request, send response (%X)\n",txBuf[0]);
         }
         SERIALG_send(&paramsP->uartH,(INT8*)txBuf,1,0);
         continue;
      }
#endif

      //check if first timestamp packet
      if ((cnt >= 1) && (rxBuf[i] == START_PACKET))
      {
         //receive rest of timestamp packet
         while(i < (PACKET_SIZE - 1))
         {
            cnt = (PACKET_SIZE - 1) - i;
            SERIALG_recv(&paramsP->uartH,&rxBuf[i+1],cnt,&cnt,5);
            i += cnt;
         }

         timestamp = 0;
         for(i = 0; i < (PACKET_SIZE / 2); i++)
         {
            if (rxBuf[i * 2] != (i + START_PACKET))
            {
               //ts not synced!
               printf("TS NOT SYNCED! rxBuf[%d] = 0x%x, cnt = %d\n",i * 2,rxBuf[i * 2],cnt);
               do
               {
                  cnt = 2;
                  SERIALG_recv(&paramsP->uartH,rxBuf,cnt,&cnt,5);
                  printf("re-sync: rxBuf[0] = 0x%x, cnt = %d\n",rxBuf[0],cnt);
                  lostSync = 1;
               }while((rxBuf[0] != ADDR_TS_7_0) && (cnt == 2));
            }
            timestamp |= ((UINT64)(rxBuf[(i*2) + 1]&0xFF) << (UINT64)(8*(((PACKET_SIZE / 2) - 1) -i)));
         }
      }

      if (!lostSync)
      {
         TIMEP_processNewTimeSample(paramsP,timestamp);

         //if (timestamp < lastTimestamp)
         //{
         // printf("wrong order of TS! current = %llu, old = %d\n",timestamp,lastTimestamp);
         //}
         //else if ((timestamp - lastTimestamp) > 1)
         //{
         //   printf("lose of TS! current = %d, old = %d\n",timestamp,lastTimestamp);
         //}
      }
      lostSync = 0;
   }

   ret = TIMEP_uartModeStop(paramsP);
   if (ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "uart stop failed\n");
      return -1;
   }

   paramsP->threadActive = FALSE;
   return 0;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: TIMEG_enableStats
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
void TIMEG_enableStats( void )
{
   TIMEP_params.enableStats = !TIMEP_params.enableStats;
}


/****************************************************************************
*
*  Function Name: TIMEP_gpStart
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEP_gpStart( inu_device__set_time_t *setTimeParamsP)
{
   ERRG_codeE              ret = RET_SUCCESS;
   OS_LYRG_threadParams    thrParams;

   switch(setTimeParamsP->method)
   {
      case(INU_DEVICE__SET_TIME_ONE_SHOT_E):
      {
         ret = OS_LYRG_setTime(setTimeParamsP->seconds,setTimeParamsP->microSeconds);
         return ret;
      }break;

      case(INU_DEVICE__SET_TIME_PLL_LOCK_USB_E):
      {
         LOGG_PRINT(LOG_ERROR_E, ERR_NOT_SUPPORTED, "INU_DEVICE__SET_TIME_PLL_LOCK_USB_E mode is not supported yet, exit\n");
         return ERR_NOT_SUPPORTED;
      }break;

      case(INU_DEVICE__SET_TIME_PLL_LOCK_UART_E):
      {
         thrParams.func = TIMEP_uartThread;
      }break;

      case(INU_DEVICE__SET_TIME_PLL_LOCK_SPI_E):
      {
         thrParams.func = TIMEP_spiThread;
      }break;

      case(INU_DEVICE__SET_TIME_PLL_LOCK_RTC_E):
      {
         // Set the HW Time
         ret = RTC_DRVG_setTime(setTimeParamsP->seconds,setTimeParamsP->microSeconds, RTC_DRVG_TrackingMode_ImmediateMode);
#ifdef SYNC_SYSTIME_WITH_RTC
         thrParams.func = TIMEP_rtcThread;

         if(TIMEP_params.state == TIMEP_STATE_ACTIVE){
            // Only set the RTC coarse time,
            // RTC Tracking and systime sync is already active.
            return ret;
         }
#else
         return ret;
#endif   
      }break;
      case(INU_DEVICE__SET_TIME_PLL_LOCK_RTC_PPS_Mode):
      {
         // Set the HW Time
         ret = RTC_DRVG_setTime(setTimeParamsP->seconds,setTimeParamsP->microSeconds, RTC_DRVG_TrackingMode_PendingMode );
#ifdef SYNC_SYSTIME_WITH_RTC
         thrParams.func = TIMEP_rtcThread;

         if(TIMEP_params.state == TIMEP_STATE_ACTIVE){
            // Only set the RTC coarse time,
            // RTC Tracking and systime sync is already active.
            return ret;
         }
#else
         return ret;
#endif   
      }break;

      default:
      {
         return ERR_UNEXPECTED;
      }
      break;
   }

   if (TIMEP_params.state == TIMEP_STATE_ACTIVE)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"TIMEP_gpStart failed, module is already active\n");
      return ERR_UNEXPECTED;
   }

   memset(&TIMEP_params,0,sizeof(TIMEP_params));
   memcpy(&TIMEP_params.setTimeParams, setTimeParamsP, sizeof(inu_device__set_time_t));
   TIMEP_params.state = TIMEP_STATE_ACTIVE;
   TIMEP_params.len = sizeof(TIMEP_params.tickHistory) / sizeof(TIMEP_params.tickHistory[0]);
   TIMEP_params.tick = setTimeParamsP->startTick;
   TIMEP_params.first_time = 1;
   TIMEP_params.ave_period = (TIMEP_params.setTimeParams.secondHostResolution == 1) ? 200 : 2000;
   LOGG_PRINT(LOG_INFO_E, NULL, "hostScale = %d, method = %d, start tick = %d, debug = %d\n",TIMEP_params.setTimeParams.secondHostResolution,setTimeParamsP->method,setTimeParamsP->startTick,TIMEP_params.setTimeParams.debugEnable);
   if (TIMEP_params.setTimeParams.debugEnable)
   {
      TIMEG_enableStats();
   }

   //Create thread
   if(ERRG_SUCCEEDED(ret))
   {
      //Launch algorithm scheduler thread
      thrParams.id            = OS_LYRG_TIME_LISTENER_THREAD_ID_E;
      thrParams.event         = NULL;
      thrParams.param         = (void*)&TIMEP_params;
      TIMEP_params.thrH       = OS_LYRG_createThread(&thrParams);
      if(!TIMEP_params.thrH)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"TIME listener thread create failed\n");
         ret = ERR_UNEXPECTED;
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: TIMEP_gpStop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEP_gpStop()
{
   TIMEP_params.state = TIMEP_STATE_NOT_ACTIVE;
   while (TIMEP_params.threadActive == TRUE)
   {
      OS_LYRG_usleep(10000);
   }

   memset(&TIMEP_params,0,sizeof(TIMEP_params));
   LOGG_PRINT(LOG_INFO_E, NULL,"TIMEP_gpStop successful\n");
   return RET_SUCCESS;
}
#else
/****************************************************************************
*
*  Function Name: TIMEG_timeInjectThread
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEG_readChipId( TIMEP_infoT *paramsP )
{
   UINT8 txBuf[20] = {0};
   UINT8 rxBuf[20] = {0};
   UINT32 cnt, numTry = 0;
   UINT8 chipID = 0;
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

   txBuf[0] = ADDR_CHIP_ID;
   SET_READ_MODE(txBuf[0]);
   while((chipID != NU_CHIP_ID) && (numTry < TIMEP_CHIP_ID_READ_MAX_TRY))
   {
      OS_LYRG_usleep(10000); //sleep for 10ms
      SERIALG_send(&paramsP->uartH,(INT8*)txBuf,1,1000);
      SERIALG_recv(&paramsP->uartH,(UINT8*)rxBuf,1,&cnt,2);
      chipID = rxBuf[0];
      numTry++;
      LOGG_PRINT(LOG_INFO_E, NULL, "chipID=0x%x\n",chipID);
   }

   if (numTry == TIMEP_CHIP_ID_READ_MAX_TRY)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read chipId, exit\n");
      ret = (ERRG_codeE)ERR_IO_ERROR;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: main
*
*  Description: main
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
int TIMEG_injectThread(void *params)
{
    TIMEP_infoT *timeParamsP = (TIMEP_infoT*)params;
    UINT64 usec;
    UINT8 i;
    char txBuf[20] = { 0 };
    int delay = TIMEP_UNSYNCED_SLEEP_TIME;
    inu_device__getUsecTimeFuncT* getUsecTimeFunc = timeParamsP->getUsecTimeFunc;

    if (getUsecTimeFunc == NULL)
    {
        getUsecTimeFunc = OS_LYRG_getUsecTime;
        LOGG_PRINT(LOG_DEBUG_E, NULL, "Using default getUsecTime function - OS_LYRG_getUsecTime\n");
    }
    else
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "Using user's getUsecTime function - [%p]\n", getUsecTimeFunc);
    }

#ifdef TIMEP_SYNCED_NOTIFY
    UINT32 cnt;
    UINT64 lastSyncTime = 0;
    char txSyncedMsgBuf[4] = { 0 };
    char rxSyncedMsgBuf[4] = { 0 };
    txSyncedMsgBuf[0] = ADDR_SYNCED;
    SET_READ_MODE(txSyncedMsgBuf[0]);
    //OS_LYRG_getUsecTime(&lastSyncTime);
    getUsecTimeFunc(&lastSyncTime);
#endif

    timeParamsP->threadActive = TRUE;
    LOGG_PRINT(LOG_INFO_E, NULL,"TIME inject thread created\n");

    while (timeParamsP->state == TIMEP_STATE_ACTIVE)
    {
        //OS_LYRG_getUsecTime(&usec);
        getUsecTimeFunc(&usec);

        for (i = 0; i < (PACKET_SIZE/2); i++)
        {
            txBuf[i * 2] = START_PACKET + i;
            txBuf[(i * 2) + 1] = (usec >> (8 * (((PACKET_SIZE / 2) - 1) - i))) & 0xFF;
        }
        SERIALG_send(&timeParamsP->uartH, (INT8*)txBuf, i * 2, 0);

#ifdef TIMEP_SYNCED_NOTIFY
        if (((usec - lastSyncTime) > TIMEP_CHECK_SYNC_INTERVAL) || (rxSyncedMsgBuf[0] == TIMEP_ACHIEVED_SYNC))
        {
            //OS_LYRG_getUsecTime(&lastSyncTime);
            getUsecTimeFunc(&lastSyncTime);
            SERIALG_send(&timeParamsP->uartH,(INT8*)txSyncedMsgBuf,1,1000);
            SERIALG_recv(&timeParamsP->uartH,(UINT8*)rxSyncedMsgBuf,1,&cnt,10);
            if ((cnt > 0) && (rxSyncedMsgBuf[0] == TIMEP_ACHIEVED_SYNC))
            {
               //target is synced, keep reading it until its not.
               OS_LYRG_usleep(TIMEP_SYNCED_SLEEP_TIME);
            }
        }
#endif

        OS_LYRG_usleep(delay); //sleep
#if 0
        printf("send TS %llu\n", usec);
#endif
    }

    TIMEP_uartModeStop(timeParamsP);
    timeParamsP->threadActive = FALSE;
    return 0;
}

/****************************************************************************
*
*  Function Name: TIMEP_hostStart
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEP_hostStart( inu_device__set_time_t *setTimeParamsP, inu_device__getUsecTimeFuncT* getUsecTimeFunc)
{
   ERRG_codeE              ret = INU_DEVICE__RET_SUCCESS;
   OS_LYRG_threadParams    thrParams;

   memset(&TIMEP_params,0,sizeof(TIMEP_params));
   memcpy(&TIMEP_params.setTimeParams, setTimeParamsP, sizeof(inu_device__set_time_t));
   TIMEP_params.state = TIMEP_STATE_ACTIVE;
   TIMEP_params.getUsecTimeFunc = getUsecTimeFunc;

   //open UART
   ret = TIMEP_uartModeStart(&TIMEP_params);

   //try to read chip ID
   if(ERRG_SUCCEEDED(ret))
   {
      ret = TIMEG_readChipId(&TIMEP_params);
   }

   //Create thread
   if(ERRG_SUCCEEDED(ret))
   {
      //Launch algorithm scheduler thread
      thrParams.id            = OS_LYRG_TIME_INJECT_THREAD_ID_E;
      thrParams.event         = NULL;
      thrParams.param         = (void*)&TIMEP_params;
      thrParams.func          = TIMEG_injectThread;
      TIMEP_params.thrH       = OS_LYRG_createThread(&thrParams);
      if(!TIMEP_params.thrH)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"TIME inject thread create failed\n");
         ret = INU_DEVICE__ERR_UNEXPECTED;
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: TIMEP_hostStop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEP_hostStop()
{
   volatile TIMEP_infoT *paramsP = (TIMEP_infoT*)&TIMEP_params;
   UINT32 retryCtr = 0;

   paramsP->state = TIMEP_STATE_NOT_ACTIVE;
   while ((paramsP->threadActive == TRUE) && (retryCtr < 15))
   {
      OS_LYRG_usleep(100000);
      retryCtr++;
   }

   if(paramsP->threadActive == TRUE)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"TIMEP_hostStop error\n");
      return INU_DEVICE__ERR_TIMEOUT;
   }

   memset((void*)paramsP,0,sizeof(TIMEP_infoT));
   LOGG_PRINT(LOG_INFO_E, NULL,"TIMEP_hostStop successful\n");
   return INU_DEVICE__RET_SUCCESS;
}
#endif

/****************************************************************************
*
*  Function Name: TIMEG_start
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEG_start(inu_device__set_time_t *setTimeParamsP, inu_device__getUsecTimeFuncT* getUsecTimeFunc)
{
    ERRG_codeE ret;
#if DEFSG_IS_GP
    ret = TIMEP_gpStart(setTimeParamsP);
#else
    ret = TIMEP_hostStart(setTimeParamsP, getUsecTimeFunc);
#endif
    return ret;
}

/****************************************************************************
*
*  Function Name: TIMEG_stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE TIMEG_stop()
{
    ERRG_codeE ret;
#if DEFSG_IS_GP
   ret = TIMEP_gpStop();
#else
   ret = TIMEP_hostStop();
#endif
   return ret;
}