#ifndef _STATS_
#define _STATS_

typedef struct
{
   UINT32 mean;
   UINT64 svariance;
   UINT32 max;
   UINT32 min;
   UINT32 numSamples;
   int overflow;
   
   //internal
   UINT64 sum;
   UINT64 sumSq;
} STATG_onlineStatT;

void STATG_begin(STATG_onlineStatT *statP);
void STATG_update(STATG_onlineStatT *statP, UINT32 sample);
void STATG_finish(STATG_onlineStatT *statP);

#endif //_ONLINE_STATS_