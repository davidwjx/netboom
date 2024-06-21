#ifndef _APB_TIMER_H_
#define _APB_TIMER_H_

#include "defs.h"
#include "config.h"

#ifndef TMR_TICKS_PER_MSEC
	#define TMR_TICKS_PER_MSEC 24*1000
#endif
#define SYSTMR_TICKS_PER_SEC TMR_TICKS_PER_MSEC *1000

typedef enum
{
   APB_TIMERG_INST_0_E = 0,
   APB_TIMERG_INST_1_E,
   APB_TIMERG_INST_2_E,
   APB_TIMERG_INST_3_E
   
} APB_TIMEG_instE;

void APB_TIMERG_init(APB_TIMEG_instE inst);
void APB_TIMERG_start(void);
BOOL APB_TIMERG_isStarted(void);
UINT32 APB_TIMERG_read(void);
void APB_TIMERG_stop(void);
void APB_TIMERG_setLoadCnt(UINT32 loadCnt);
UINT32 APB_TIMERG_getLoadCnt(void);

void APB_TIMERG_wdtReset(void);
void APB_TIMERG_wdtDisable(void);


#endif //_APB_TIMER_H_

