/****************************************************************************
 * 
 *   FileName: inu_profiler.c
 *
 *   Author: Konstnaitn Sinyuk
 *
 *   Date: 09/2015
 *
 *   Description: INU profiler  API implementation 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "os_lyr.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/


/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct {
   UINT64 timeStamp;
   const char *functionName; 
   UINT32  lineNumber;
} INU_PROFILER_timeStampT;
 
/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

#ifdef  INU_PROFILER
INU_PROFILER_timeStampT traceTimestamps[INU_PROFILER_MAX_TRACES ][INU_PROFILER_MAX_TRACE_LENGTH];
UINT32 histogramArray[HISTOGRAM_MAX_STEPS];
INT32 traceId=0, traceIndex=0, profilerAllowTrace=0;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

INUPROFILER_API void INUG_profiler_get_timestamp_ex(const char * functionName, int lineNumber)
{
   if (profilerAllowTrace )
   {
      //sanity check
      if (traceIndex ==  INU_PROFILER_MAX_TRACE_LENGTH) 
         traceIndex=INU_PROFILER_MAX_TRACE_LENGTH-1;
      if (traceId >= INU_PROFILER_MAX_TRACES)
         traceId = INU_PROFILER_MAX_TRACES-1;
      
      OS_LYRG_getUsecTime(&traceTimestamps[traceId][traceIndex].timeStamp);      
      traceTimestamps[traceId][traceIndex].functionName = functionName;
      traceTimestamps[traceId][traceIndex].lineNumber = lineNumber;
      traceIndex++;
      //printf ("get time stamp (id=%d, trace_id=%d), function =%s\n",traceIndex,traceId,functionName);
   }
}


INUPROFILER_API void INUG_profiler_start_trace()
{
   traceIndex =0;
   profilerAllowTrace =1;
}

INUPROFILER_API void INUG_profiler_end_trace()
{
   profilerAllowTrace=0;
   traceId++;
   if (traceId >= INU_PROFILER_MAX_TRACES)
      traceId = INU_PROFILER_MAX_TRACES-1;
}


INUPROFILER_API void INUG_profiler_init()
{
   int i,j;
   traceId = 0;
   traceIndex = 0;
   memset(histogramArray,0,sizeof(histogramArray));
   for (i=0;i<INU_PROFILER_MAX_TRACES;i++)
      for(j=0;j<INU_PROFILER_MAX_TRACE_LENGTH;j++)         
         memset(&traceTimestamps[i][j],0,sizeof(INU_PROFILER_timeStampT));
   LOGG_PRINT(LOG_INFO_E, NULL,  "init profiler\n");
}


static int cmp_by_timestamp(const void *a, const void *b)
{   
   INU_PROFILER_timeStampT *timestampA = (INU_PROFILER_timeStampT *)a;
   INU_PROFILER_timeStampT *timestampB = (INU_PROFILER_timeStampT *)b;
   return (int)(timestampA->timeStamp - timestampB->timeStamp);
}

INUPROFILER_API void INUG_profiler_print_statistics(int tresholdInMicroS)
{
   int i,j,k;
   UINT64 start_trace, end_trace, curr_trace,prev_trace, maxTrace=0, minTrace=0;
   UINT64 sum,diff;
   UINT32 histIndex;

   LOGG_PRINT_PRINTF(LOG_INFO_E,  "analysing %d traces\n",traceId);
   sum=0;
   for(i=100; i< traceId; i++)
   {                            
      for(j=0;j<INU_PROFILER_MAX_TRACE_LENGTH;j++)
         if (traceTimestamps[i][j].timeStamp == 0)
            break;
      // sort timestamps to compenstate for thread switching
      qsort(&traceTimestamps[i][0], j, sizeof(INU_PROFILER_timeStampT), cmp_by_timestamp); 
      start_trace = traceTimestamps[i][0].timeStamp;
      if (j==0)
         end_trace = traceTimestamps[i][0].timeStamp;
      else
         end_trace = traceTimestamps[i][j-1].timeStamp;      
      diff = (end_trace -start_trace);
      sum+= diff;

      prev_trace = start_trace;
      if (diff > (UINT64)tresholdInMicroS)
      {   
         LOGG_PRINT_PRINTF(LOG_INFO_E, "trace id=%d, trace length=%d,  duration=%lld uS, profiler stack:\n",i,j,diff);
         for(k=0;k<j;k++)
         {
            curr_trace = traceTimestamps[i][k].timeStamp;
            if (k>0)
               prev_trace = traceTimestamps[i][k-1].timeStamp;
            LOGG_PRINT_PRINTF(LOG_INFO_E, "\t%s(%d): diff=%lld, delta=%lld, timestamp=%lld\n",traceTimestamps[i][k].functionName,traceTimestamps[i][k].lineNumber,
               (curr_trace - start_trace),(curr_trace - prev_trace), (unsigned long)curr_trace);
         }
            
      }

      histIndex = (UINT32)diff/HISTOGRAM_RESOLUTION_MICROS;
      if (histIndex >= HISTOGRAM_MAX_STEPS)
         histIndex = HISTOGRAM_MAX_STEPS-1;
      histogramArray[histIndex]++;
      
      if (diff > maxTrace)
         maxTrace=diff;
      
      if (minTrace == 0)
         minTrace = diff;
      if (diff < minTrace)
         minTrace=diff;
      
   }      
   LOGG_PRINT_PRINTF(LOG_INFO_E, "total samples = %d,  average RTT = %lld, max RTT = %lld, min RTT = %lld\n", traceId,  sum/traceId, maxTrace, minTrace);
   LOGG_PRINT_PRINTF(LOG_INFO_E, "Full histogram: resolution %d us, max steps %d\n",HISTOGRAM_RESOLUTION_MICROS, HISTOGRAM_MAX_STEPS);
   for (i=0;i<HISTOGRAM_MAX_STEPS;i++)
      LOGG_PRINT_PRINTF(LOG_INFO_E, "\t%d uS: %d\n",i*HISTOGRAM_RESOLUTION_MICROS,histogramArray[i]);
}
#endif

#ifdef __cplusplus
}
#endif

