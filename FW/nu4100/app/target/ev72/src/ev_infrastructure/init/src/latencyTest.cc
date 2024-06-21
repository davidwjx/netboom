/****************************************************************************
 *
 *   FileName: ev_init.c
 *
 *   Author: Noap Porat
 *
 *   Date: 10/18
 *
 *   Description: EV6X application initialization process
 *   
 ****************************************************************************/

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/


#include <inu_app_pipe.h>
#include "inu_common.h"
#include "ev_init.h"
#include "log.h"
#include "ictl_drv.h"
#include <evthreads.h>
#include <evdev.h>




 

/****************************************************************************
 ***************      L O C A L       D E F N I T I O N S     ***************
****************************************************************************/

/****************************************************************************
 ***************      L O C A L       T Y P E D E F S         ***************
****************************************************************************/

/****************************************************************************
 ***************      L O C A L      D A T A                    ***************
****************************************************************************/

/****************************************************************************
 ***************      G L O B A L     D A T A                 ***************
****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/

#ifdef EV_LATENCY_TEST

void EV_INITG_coreLatencyTest()
{
	volatile UINT32 *ddrP, *ddrPstart;
	volatile UINT32 *intP, *intPstart;
	volatile UINT32 *csmP, *csmPstart;
	volatile UINT32 *sysP, *sysPstart;
	volatile UINT32 *lramP, *lramPstart = (UINT32 *)0x02000000;
	volatile _Uncached UINT32 *uncachedDdrP;

	UINT32 idx;
	UINT32 time2; 
	UINT32 time1;
	UINT32 totErrors;

	ddrPstart = (UINT32 *)evMemAlloc(256*128, EV_MR_USE_CNDC,-1);	// DDR
	uncachedDdrP = (_Uncached UINT32 *)evMemAlloc(256*128, EV_MR_USE_CNDC,-1);	// DDR
	intPstart = (UINT32 *)evMemAlloc(128*4+16, EV_MR_USE_VCCM,0);	//VCCM
	sysPstart = (UINT32 *)evSystemAddr((char*)intPstart, 0);
	csmPstart = (UINT32 *)evMemAlloc(256*128+8, EV_MR_USE_CSM,0); //CSM

	ddrP = ddrPstart;
	intP = intPstart;
	sysP = sysPstart;
	csmP = csmPstart;
	lramP = lramPstart;

	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "CORE LATENCY TEST : ddrP	= %x uncachedDDR = %x, intP = %x sysP  = %x csmP  = %x\n\n", ddrP, uncachedDdrP, intP, sysP, csmP);

	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;		
		*intP;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) read from intP (core) = %d cycles\n", idx, (time2-time1)/16);
	}
	
	
	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		*intP = 1;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) write to intP (core) = %d cycles\n", idx, (time2-time1)/16);
	}
	


	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*csmP ;		
		*csmP ;		
		*csmP ;		
		*csmP ;		
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		*csmP ;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) read from csm (core) = %d cycles\n", idx, (time2-time1)/16);
	}

 

	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*csmP = 1;		
		*csmP = 1;		
		*csmP = 1;		
		*csmP = 1;		
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		*csmP = 1;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) write to csm (core) = %d cycles\n", idx, (time2-time1)/16);
	}
	
	

	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;		
		*ddrP;
		time2 = _lr(AUX_RTC_LOW);

		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) read from ddr (core) = %d cycles\n", idx, (time2-time1)/16);
	}

	

	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;		
		*ddrP = 1;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) write to ddr (core) = %d cycles\n", idx, (time2-time1)/16);
	}


	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;		
		*uncachedDdrP;
		time2 = _lr(AUX_RTC_LOW);

		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) read from uncached ddr (core) = %d cycles\n", idx, (time2-time1)/16);
	}


	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;		
		*uncachedDdrP = 1;
		time2 = _lr(AUX_RTC_LOW);
		
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) write to uncached ddr (core) = %d cycles\n", idx, (time2-time1)/16);
	}


	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP; 	
		*lramP;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) read from lram (core) = %d cycles\n", idx, (time2-time1)/16);
	}

	
	for(idx = 0 ; idx < 2; idx++)
	{
		time1 = _lr(AUX_RTC_LOW);
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;		
		*lramP = 1;
		time2 = _lr(AUX_RTC_LOW);
		LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(iteration #%d) write to lram (core) = %d cycles\n", idx, (time2-time1)/16);
	}
}



void EV_INITG_dmaLatencyTest()
{
	ev_ocl_event_t  dmaRet;
	UINT32 totDmaTime, time32Stop, time32Start;
	UINT32 idx, totErrors;
	volatile UINT32 *ddrP;
	volatile UINT32 *intP;
	volatile UINT32 *sysP;
	volatile _Uncached UINT32 *uncachedDdrP;

	ddrP = (UINT32 *)evMemAlloc(256*128, EV_MR_USE_CNDC,-1);	// DDR
	uncachedDdrP = (_Uncached UINT32 *)evMemAlloc(256*128, EV_MR_USE_CNDC,-1);	// DDR
	intP = (UINT32 *)evMemAlloc(128*4+16, EV_MR_USE_VCCM,0);	//VCCM
	sysP = (UINT32 *)evSystemAddr((char*)intP, 0);	//	accessing the VCCM via DMA


	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "DMA LATENCY TEST : ddrP	= %x uncachedDDR = %x, intP = %x sysP  = %x\n\n", ddrP, uncachedDdrP, intP, sysP);

	// empty DMA fifo
	dmaRet = evAsyncCopy2D((char*)ddrP, (char*)sysP, 4, 256, 0, 4*128); 	
	evWaitEvent(dmaRet);


	/////////////////	VCCM -> DDR (uncahed)	/////////////////////////////////
	//initialize source and destination buffers
	for(idx = 0 ; idx < 128; idx++)
	{
			intP[idx] = idx;
			uncachedDdrP[64*idx] = 0;
	}

	// DDR <-> internal
	time32Start = _lr(AUX_RTC_LOW);
	{		
		dmaRet = evAsyncCopy2D((char*)uncachedDdrP, (char*)sysP, 4, 256, 4, 4*128);	
	}
	evWaitEvent(dmaRet);
	time32Stop = _lr(AUX_RTC_LOW);
	totDmaTime = (time32Stop - time32Start);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "int -> ddr(uncached) single(DMA)  = %d cycles\n\n", totDmaTime/(128));
	totErrors = 0;
	//check for errors
	for(idx = 0 ; idx < 128; idx++)
	{	
		if(uncachedDdrP[64*idx] !=  idx)
			totErrors++;
	}
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "VCCM->DDR(uncached) tot errors = %d\n\n", totErrors);


	///////////////////////////	DDR -> VCCM (uncached)
	//initialize source and destination buffers
	for(idx = 0 ; idx < 128; idx++)
	{
			intP[idx] = 0;
			uncachedDdrP[64*idx] = idx;
	}				
	time32Start = _lr(AUX_RTC_LOW);

	{		
		dmaRet = evAsyncCopy2D((char*)sysP, (char*)uncachedDdrP, 4, 4, 256, 4*128);
	}
	evWaitEvent(dmaRet);
	time32Stop = _lr(AUX_RTC_LOW);
	totDmaTime = (time32Stop - time32Start);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "(uncached) ddr -> int = %d cycles\n\n", totDmaTime/(128));
	totErrors = 0 ;
	//check for errors
	for(idx = 0 ; idx < 128; idx++)
	{	
		if(intP[idx] !=  idx)
			totErrors++;
	}
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "DDR->VCCM(uncached) tot errors = %d\n\n", totErrors);


	///////////////////////////////////////	VCCD -> DDR	/////////////////////////////
	for(idx = 0 ; idx < 128; idx++)
	{
			intP[idx] = idx;
			ddrP[64*idx] = 0;
	}

	// DDR <-> internal
	time32Start = _lr(AUX_RTC_LOW);
	for(idx = 0 ; idx < 16; idx++)
	{		
		dmaRet = evAsyncCopy2D((char*)ddrP, (char*)sysP, 4, 256, 4, 4*128); 	
	}
	evWaitEvent(dmaRet);
	time32Stop = _lr(AUX_RTC_LOW);
	totDmaTime = (time32Stop - time32Start);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "int -> ddr (dma) = %d %d cycles\n\n", totDmaTime/(16*128), totDmaTime);


	/////////////////////////////////////	DDR -> VCCM	//////////////////////		
	for(idx = 0 ; idx < 128; idx++)
	{
			intP[idx] = 0;
			ddrP[64*idx] = idx;
	}				
	time32Start = _lr(AUX_RTC_LOW);
	for(idx = 0 ; idx < 16; idx++)
	{		
		dmaRet = evAsyncCopy2D((char*)sysP, (char*)ddrP, 4, 4, 256, 4*128);
	}
	evWaitEvent(dmaRet);
	time32Stop = _lr(AUX_RTC_LOW);
	totDmaTime = (time32Stop - time32Start);
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "ddr -> int = %d cycles\n\n", totDmaTime/(16*128));
	LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "%d %d %d %d %d %d %d %d\n\n", intP[1], intP[2], intP[3], intP[4], intP[5], intP[6], intP[7], intP[8]);
}
#endif //	EV_LATENCY_TEST
