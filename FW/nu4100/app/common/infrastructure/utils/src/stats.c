#include "inu_common.h"

#include "stats.h"
/*
psuedo-code for variance calculation

   sum := 0
   sumsq := 0

   for k = 1 to n do
     sum := sum + x(k)
     sumsq := sumsq + x(k)^2
   end for
   
   v :=sumsq - (1/n)*sum^2
   if we want the standard variance then
     return v/n
   else . we want the sample variance
     return v/(n - 1)
   end if
*/
void STATG_begin(STATG_onlineStatT *statP)
{
   memset(statP, 0, sizeof(STATG_onlineStatT));
   statP->min = UINT32_MAX;
}

void STATG_update(STATG_onlineStatT *statP, UINT32 sample)
{
   UINT64 sq;

   //mean-variance
   if(!statP->overflow)
   {
      if(statP->sum + sample < UINT32_MAX)
      {
         statP->sum += sample;
      
         sq = (UINT64)sample*(UINT64)sample;
         if(UINT64_MAX - statP->sumSq > sq)
            statP->sumSq += sq;
         else
            statP->overflow = 1;//overflow on sum of squared

         statP->numSamples++;
         
         LOGG_PRINT(LOG_DEBUG_E,NULL, "%u\n", sample); 
      }
      else
      {
         statP->overflow = 1;//squared sum will overflow on completion
      }
   }

   //min-max
   if(sample < statP->min) 
      statP->min = sample;
   else if (sample > statP->max)
      statP->max = sample;

}

void STATG_finish(STATG_onlineStatT *statP)
{
   UINT64 diff;
   UINT64 sqSum;

   statP->mean = statP->sum/statP->numSamples;

   if(statP->numSamples < 2)
      return;

   if(statP->sum >= UINT32_MAX)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL, "variance calc: undetected overflow in sum\n"); 
      return;
   }
   
   sqSum = statP->sum*statP->sum;
   if(statP->sumSq >= (sqSum/statP->numSamples))
   {
      diff = statP->sumSq - (sqSum/statP->numSamples);
      statP->svariance = (diff/(statP->numSamples-1)); //unbiased sample variance

      LOGG_PRINT(LOG_DEBUG_E,NULL, "variance calc sum=%llu sqSum=%llu sqSum/n=%llu sumSq=%llu diff=%llu n=%u overflow=%d\n", 
         statP->sum, sqSum, sqSum/statP->numSamples, statP->sumSq, diff, statP->numSamples, statP->overflow);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL, "variance calc: invalid sum of squars < squared sum/n\n"); 
   }

}


