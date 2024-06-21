#ifndef _ICTL_H_
#define _ICTL_H_

typedef void (*ICTL_isrT)(void);

typedef enum
{
   ICTLG_PRIO_0_E = 0,
   ICTLG_PRIO_1_E,
   ICTLG_PRIO_2_E,

   ICTLG_MAX_PRIO_E = ICTLG_PRIO_2_E,
   ICTLGP_NUM_PRIO_E
} ICTLG_priorityE;

void ICTLG_init(void);
void ICTLG_registerIsr(UINT16 intNum, ICTL_isrT isr, UINT16 priority);
void ICTLG_deRegisterIsr(UINT16 intNum);
void ICTLG_maskInt(UINT16 intNum);
void ICTLG_unMaskInt(UINT16 intNum);
void ICTLG_generateForceInt(UINT16 intNum);
void ICTLG_clearForceInt(UINT16 intNum);
void ICTLG_setPriorityFilter(UINT16 priority);

void ICTLG_generateForceInt(UINT16 intNum);
void ICTLG_clearForceInt(UINT16 intNum);

void ICTLG_irqHandler(void);

#endif //_ICTL_H_

