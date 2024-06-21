#ifndef _DW_UMCTL2_H_
#define _DW_UMCTL2_H_

#define DW_MCTLG_LPR  (0)
#define DW_MCTLG_HPR  (1)
#define DW_MCTLG_WR   (2)

#define DW_MCTLG_PORT_0_GPP    (0)
#define DW_MCTLG_PORT_1_DSPA   (1)
#define DW_MCTLG_PORT_2_DSPB   (2)
#define DW_MCTLG_PORT_3_HIE    (3)
#define DW_MCTLG_PORT_4_CDE0   (4)
#define DW_MCTLG_PORT_5_CDE1   (5)

#define DW_MCTLG_PORT_HPR_POS          (0)
#define DW_MCTLG_PORT_PAGE_MATCH_POS   (1)
#define DW_MCTLG_PORT_AGING_POS        (2)
#define DW_MCTLG_PORT_HPR_EN           (1<<DW_MCTLG_PORT_HPR_POS)
#define DW_MCTLG_PORT_PAGE_MATCH_EN    (1<<DW_MCTLG_PORT_PAGE_MATCH_POS)
#define DW_MCTLG_PORT_AGING_EN         (1<<DW_MCTLG_PORT_AGING_POS)

#define DW_MCTLG_PORT_PM_LIMIT_POS     (0)
#define DW_MCTLG_PORT_PM_LIMIT_EN      (1<<DW_MCTLG_PORT_PM_LIMIT_POS)

//Register offsets
#define DW_MCTLG_REG_SCHED        (0x250)
#define DW_MCTLG_REG_PERFHPPR0    (0x258)
#define DW_MCTLG_REG_PERFHPPR1    (0x25C)
#define DW_MCTLG_REG_PERFLPPR0    (0x260)
#define DW_MCTLG_REG_PERFLPPR1    (0x264)
#define DW_MCTLG_REG_PERFWR0      (0x268)
#define DW_MCTLG_REG_PERFWR1      (0x26c)
#define DW_MCTLG_REG_PCCFG        (0x400)
#define DW_MCTLG_REG_PCFGR(n)     (0x404 + (n*0xb0))
#define DW_MCTLG_REG_PCFGW(n)     (0x408 + (n*0xb0))


ERRG_codeE DW_MCTLG_init(void);
UINT32 DW_MCTLG_getVirtBase(void);

UINT32 DW_MCTLG_rdReg(UINT32 reg);
void DW_MCTLG_wrReg(UINT32 reg, UINT32 val);

void DW_MCTLG_setSched(int preferWr, UINT32 rdWrIdleGap, UINT32 lprNumEntries);
void DW_MCTLG_setStorePerf(int storeSel, UINT32 minNonCritical, UINT32 runLen, UINT32 maxStarve);
void DW_MCTLG_setPortCommonCfg(int flags);
void DW_MCTLG_setPortRdCfg(int port, UINT32 flags, UINT32 priority);
void DW_MCTLG_setPortWrCfg(int port, UINT32 flags, UINT32 priority);

void DW_MCTLG_getSched(int *preferWrP, UINT32 *rdWrIdleGapP, UINT32 *lprNumEntriesP);
void DW_MCTLG_getStorePerf(int storeSel, UINT32 *minNonCriticalP, UINT32 *runLenP, UINT32 *maxStarveP);
void DW_MCTLG_getPortRdCfg(int port, UINT32 *flagsP, UINT32 *priorityP);
void DW_MCTLG_getPortWrCfg(int port, UINT32 *flagsP, UINT32 *priorityP);


void DW_MCTLG_showRegs(void);

#endif //_DW_UMCTL2_H_
