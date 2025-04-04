/****************************************************************************
 *
 *   FileName: ev_init.h
 *
 *   Author:
 *
 *   Date:
 *
 *   Description:
 *
 ****************************************************************************/
#ifndef EV_INIT_H
#define EV_INIT_H

#ifdef __cplusplus
      extern "C" {
#endif




/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ***************************************************************************/
//#define EV_LATENCY_TEST
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
****************************************************************************/
//	typedef void (*CEVA_INITG_initCbT)();

/****************************************************************************
 ***************      E X T E R N A L       D A T A           ***************
****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
****************************************************************************/
void EV_INITG_init(void);
UINT32 EV_INITG_getEvFreqMhz(void);

#ifdef EV_LATENCY_TEST
void EV_INITG_coreLatencyTest();
void EV_INITG_dmaLatencyTest();
#endif	//	EV_LATENCY_TEST




#ifdef __cplusplus
}
#endif

#endif //	EV_INIT_H
