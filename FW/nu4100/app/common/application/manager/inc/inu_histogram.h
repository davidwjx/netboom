#ifndef _INU_HISTOGRAM_H_
#define _INU_HISTOGRAM_H_

#include "inu2.h"
#include "inu_function.h"

typedef enum
{
   INU_HISTOGRAM__IAU_HW        = 0,
   INU_HISTOGRAM__XM4           ,
} inu_histogram__engine_e;

typedef struct
{
   inu_function                 function;

   void                        *privP;
}inu_histogram;

typedef struct
{
   inu_function__CtorParams functionCtorParams;
   unsigned int nuCfgChId;
   char         configName[128];   
   inu_histogram__engine_e engine;
}inu_histogram__CtorParams;

void inu_histogram__vtable_init(void);
const inu_function__VTable *inu_histogram__vtable_get(void);
ERRG_codeE inu__reuseHistMemcpyMsg(void *me , UINT64 timestamp, UINT32 hist, MEM_POOLG_bufDescT *bufDesc);
ERRG_codeE inu__reuseFreakStartMsg(UINT32 systemFrameCtr, UINT32 currentPhyAddress);
ERRG_codeE inu__reuseMemInit(void);
#endif