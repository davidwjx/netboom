#ifndef ICTL_DRV_H
#define ICTL_DRV_H

#include "inu_types.h"
#include "evthreads.h"

typedef void (*ICTL_isrT)(void);

typedef enum
{
   ICTLG_PRIO_0_E = 0,
   ICTLG_PRIO_1_E,
   ICTLG_PRIO_2_E,

   ICTLG_MAX_PRIO_E = ICTLG_PRIO_2_E,
   ICTLGP_NUM_PRIO_E
} ICTLG_priorityE;



//////////////////////////////////////////////////////////////////
//connection IOCTL commands
/////////////////////////////////////////////////////////////////

//	typedef enum
//	{
//	   ICTL_DRVG_WDTMR_DSP_INT	=  0,
//		ICTL_DRVG_SW_INT,
//		ICTL_DRVG_TMR_DSP_INT_0,
//		ICTL_DRVG_TMR_DSP_INT_1,
//		ICTL_DRVG_TMR_DSP_INT_2,
//		ICTL_DRVG_TMR_DSP_INT_3,
//		ICTL_DRVG_IAE_INT,
//		ICTL_DRVG_DPE_INT,
//		CTL_DRVG_DSP_CPM_DBG_GEN_GVI_R,
//		CTL_DRVG_CDE_DSP_INT,
//		CTL_DRVG_DSP_MAILBOX_MSG_SW_INT,
//		CTL_DRVG_GPP_MAILBOX_MSG_SW_INT,
//		CTL_DRVG_DSPX_DMSS_MCCI_RD_INT_0,
//		CTL_DRVG_DSPX_DMSS_MCCI_RD_INT_1,
//		CTL_DRVG_DSPX_DMSS_MCCI_RD_INT_2,
//		CTL_DRVG_DSPX_DMSS_MCCI_RD_INT_3,
//		CTL_DRVG_I2C1_INT,
//		CTL_DRVG_I2C2_INT,
//		CTL_DRVG_I2C3_INT,
//		CTL_DRVG_I2C4_INT,
//		CTL_DRVG_UART0_INT,
//		CTL_DRVG_UARTX_INT, //UARTX: UART1 for DSPA and UART2 for DSPB
//		CTL_DRVG_DSP_DMSS_MCCI_WR_INT,
//		CTL_DRVG_DSP_VDMA_INT_R,
//		CTL_DRVG_DSP_PDMA_INT_R,
//		CTL_DRVG_GPIO_BUS_INT_0,
//		CTL_DRVG_GPIO_BUS_INT_1,
//		CTL_DRVG_GPIO_BUS_INT_2,
//		CTL_DRVG_GPIO_BUS_INT_3,
//		CTL_DRVG_TMR_DSPX_INT_0,
//		CTL_DRVG_TMR_DSPX_INT_1,
//		CTL_DRVG_I2S_INT
//	} ICTL_DRVG_ioctlSources;

typedef struct
{
   UINT16 intNum;
   UINT16 priority;
   ICTL_isrT isr;

} ICTL_DRVG_isrT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
UINT32 ICTL_DRVG_clearForceInt(UINT16 intNum);
_Interrupt void ICTL_DRVG_irqHandler();
void ICTL_DRVG_initIctl(void);

void ICTL_DRVP_unMaskInt(UINT16 intNum);
void ICTL_DRVP_maskInt(UINT16 intNum);
void ICTL_DRVG_registerIsr(ICTL_DRVG_isrT *registerParams);

/****************************************************************************
 ***************     EXTERN                                   ***************
 ****************************************************************************/
 extern   EvIntCondType SCHEDG_Condition;

//	extern UINT32   ICTL_DRVG_deviceBaseAddress;


#endif //ICTL_DRV_H

