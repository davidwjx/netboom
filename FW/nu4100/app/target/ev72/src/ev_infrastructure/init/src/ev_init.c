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

#ifdef __cplusplus 
      extern "C" {
#endif

#include "inu_common.h"
#include "ev_init.h"
#include "log.h"
//	#include "ceva_tests.h"
//	#include "nu4100_ictl2_regs.h"
#include "ictl_drv.h"
//	#include "vp_int.h"
#include "pss_drv.h"



#include "data_base.h"
#include "icc.h"
#include "sched.h"

 

/****************************************************************************
 ***************      L O C A L       D E F N I T I O N S     ***************
****************************************************************************/

/****************************************************************************
 ***************      L O C A L       T Y P E D E F S         ***************
****************************************************************************/
typedef struct
{
	UINT32 pll_on	 :1,
		  bypass  :1,
		  refdiv   :6,
		  fbdiv	  :12,
		  unused   :4,
		  postdiv1 :3,
		  postdiv2 :3,
		  dac 	  :1,
		  dsm 	  :1;
} EV_INITP_pllT;

	

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

#if 0
#define  NOAM_DEBUG3                   0x1003D30

_Interrupt void ictl_handler()
{
//	    uint32_t status;
    static volatile uint32_t *debug3  = (volatile uint32_t *)NOAM_DEBUG3;
//	    static volatile uint32_t *forceint  = (volatile uint32_t *)ICTL_INTFORCE;
//	//		static uint32_t *status1  = ( uint32_t *)NOAM_DEBUG3_1;
//	
    *debug3  = 0xcafec0fe;
//	
//	    status = *((uint32_t*)0x08130020);
//		*status1  = status;
//		int_cntr++;
    ICTL_DRVG_clearForceInt(0);

//		*forceint = 0;
	__asm__ __volatile__ ("dsync " : : : "memory"); //dsync to make sure we cleared the int
//		evIntCondSignal(&waitCondition);
    
    LOGG_PRINT(LOG_INFO_E, (ERRG_codeE)0, "EV Interrupt!\n");
}
#endif


static void EV_INITP_initializeInterrupts() 
{  
	int intmask;
	//disable ints
	intmask = evIntsOff();
	// set interrupt handler for ICTL message
    _setvecti(17, ICTL_DRVG_irqHandler);
	_mask_vecti(17, 1);
	evIntsRestore(intmask);
    ICC_DRVG_init();
}

UINT32 EV_INITG_getEvFreqMhz() 
{  
	UINT32 *pllRegPtr;
	UINT32 foutvco,freq;
	EV_INITP_pllT pllReg;
	   
	pllRegPtr = (unsigned int *)&pllReg;

   *pllRegPtr = *(volatile UINT32 *)0x0802041c;//GME_DSP_PLL_STATUS_VAL
   //Reading the status register for printing
   foutvco = (24*pllReg.fbdiv)/pllReg.refdiv;
   freq = (foutvco/pllReg.postdiv1)/pllReg.postdiv2;
//	   LOGG_PRINT(LOG_INFO_E, NULL, "EVP frequency = %d MHz, pll status = 0x%x\n", freq, *pllRegPtr);
	return freq;
}


/****************************************************************************
 ***************      G L O B A L     F U N C T I O N S       ***************
****************************************************************************/

void EV_INITG_init(void)
{
	UINT32 evFreq;
   PSS_DRVG_init(0x080D0000);

   //Initialize DMA Manager

   #ifdef MUTEX_TEST
      MUTEX_TESTG_test();  
   #endif
   
   DATA_BASEG_initDataBase();
   SCHEDG_init();
   EV_INITP_initializeInterrupts();

	evXdmaInit();

#ifdef EV_LATENCY_TEST
	EV_INITG_coreLatencyTest();
	EV_INITG_dmaLatencyTest();
#endif //#ifdef EV_LATENCY_TEST
	evFreq = EV_INITG_getEvFreqMhz();
   LOGG_PRINT(LOG_INFO_E, NULL, "EVP frequency = %d MHz\n", evFreq);
   SCHEDG_entryPoint(ICCG_CMD_TARGET_DSPB); 

}

#ifdef __cplusplus
}
#endif

