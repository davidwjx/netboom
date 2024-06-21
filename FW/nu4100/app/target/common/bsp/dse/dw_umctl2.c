#include "inu_common.h"
#if 0

#include "dw_umctl2.h"
#include "nu4100_ddrc_regs.h"
#include "nu4100_ddrc_mp_regs.h"


static UINT32 DW_MCTLP_virtBase = 0;
static UINT32 DW_MCTLP_mp_virtBase = 0;

static void writeReg(UINT32 addr, UINT32 wdata)
{
   volatile UINT32 *regAddr = (volatile UINT32 *)addr;
   //printf("dw_umctl2 write reg 0x%08x=0x%08x\n", addr,wdata);
   *regAddr = wdata;
}

static UINT32 readReg(UINT32 addr)
{
   volatile UINT32 *regAddr = (volatile UINT32 *)addr;
   return (*regAddr);
}

static int regAddr(UINT32 regOffset)
{
    return (DDRC_BASE + regOffset);
}

ERRG_codeE DW_MCTLG_init(void)
{
   ERRG_codeE           ret = MEM_MAP__RET_SUCCESS;

   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_DSE_E,    (MEM_MAPG_addrT *)&DW_MCTLP_virtBase);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d failed !!!\n");
      return (ret);
   }

   ret = MEM_MAPG_getVirtAddr(MEM_MAPG_REG_DSE_MP_E, (MEM_MAPG_addrT *)&DW_MCTLP_mp_virtBase);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "map memory region %d failed !!!\n");
      return (ret);
   }

   return ret;
}

UINT32 DW_MCTLG_getVirtBase(void)
{
   return DW_MCTLP_virtBase;
}

UINT32 DW_MCTLG_rdReg(UINT32 reg)
{
   return readReg(regAddr(reg));
}

void DW_MCTLG_wrReg(UINT32 reg, UINT32 val)
{
   writeReg(regAddr(reg),val);
}

void DW_MCTLG_setSched(int preferWr, UINT32 rdWrIdleGap, UINT32 lprNumEntries)
{
   DDRC_SCHED_PREFER_WRITE_W(preferWr);
   DDRC_SCHED_RDWR_IDLE_GAP_W(rdWrIdleGap);
   DDRC_SCHED_LPR_NUM_ENTRIES_W(lprNumEntries);
}

void DW_MCTLG_getSched(int *preferWrP, UINT32 *rdWrIdleGapP, UINT32 *lprNumEntriesP)
{
   if(preferWrP)
      *preferWrP = SCHED_PREFER_WRITE_R;
   if(rdWrIdleGapP)
      *rdWrIdleGapP = SCHED_RDWR_IDLE_GAP_R;
   if(lprNumEntriesP)
      *lprNumEntriesP = SCHED_LPR_NUM_ENTRIES_R;
}

void DW_MCTLG_setStorePerf(int storeSel, UINT32 minNonCritical, UINT32 runLen, UINT32 maxStarve)
{
   if(storeSel == DW_MCTLG_HPR)
   {
      PERFHPR0_HPR_MIN_NON_CRITICAL_W(minNonCritical);
      DDRC_PERFHPR1_HPR_XACT_RUN_LENGTH_W(runLen);
      DDRC_PERFHPR1_HPR_MAX_STARVE_W(maxStarve);
   }
   else if(storeSel == DW_MCTLG_LPR)
   {
      PERFLPR0_LPR_MIN_NON_CRITICAL_W(minNonCritical);
      DDRC_PERFLPR1_LPR_XACT_RUN_LENGTH_W(runLen);
      DDRC_PERFLPR1_LPR_MAX_STARVE_W(maxStarve);
   }
   else if(storeSel == DW_MCTLG_WR)
   {
      PERFWR0_W_MIN_NON_CRITICAL_W(minNonCritical);
      DDRC_PERFWR1_W_XACT_RUN_LENGTH_W(runLen);
      DDRC_PERFWR1_W_MAX_STARVE_W(maxStarve);
   }
}

void DW_MCTLG_getStorePerf(int storeSel, UINT32 *minNonCriticalP, UINT32 *runLenP, UINT32 *maxStarveP)
{
   UINT32 minNonCritical,runLen,maxStarve;
   minNonCritical = runLen = maxStarve = 0;

   if(storeSel == DW_MCTLG_HPR)
   {
      minNonCritical = PERFHPR0_HPR_MIN_NON_CRITICAL_R;
      runLen       = PERFHPR1_HPR_XACT_RUN_LENGTH_R;
      maxStarve    = PERFHPR1_HPR_MAX_STARVE_R;
   }
   else if(storeSel == DW_MCTLG_LPR)
   {
      minNonCritical = PERFLPR0_LPR_MIN_NON_CRITICAL_R;
      runLen       = PERFLPR1_LPR_XACT_RUN_LENGTH_R;
      maxStarve    = PERFLPR1_LPR_MAX_STARVE_R;
   }
   else if(storeSel == DW_MCTLG_WR)
   {
      minNonCritical = PERFWR0_W_MIN_NON_CRITICAL_R;
      runLen       = PERFWR1_W_XACT_RUN_LENGTH_R;
      maxStarve    = PERFWR1_W_MAX_STARVE_R;
   }
   
   if(maxStarveP) 
      *maxStarveP = maxStarve;
   if(runLenP) 
      *runLenP= runLen;
   if(minNonCriticalP) 
      *minNonCriticalP= minNonCritical;

}

void DW_MCTLG_setPortCommonCfg(int flags)
{
   int limit = (flags & DW_MCTLG_PORT_PM_LIMIT_EN) >> DW_MCTLG_PORT_PM_LIMIT_POS;
   DDRC_MP_PCCFG_PAGEMATCH_LIMIT_W(limit);
}

void DW_MCTLG_getPortCommonCfg(int *flagsP)
{
   int limit= PCCFG_PAGEMATCH_LIMIT_R;

   if(flagsP)
      *flagsP = (limit << DW_MCTLG_PORT_PM_LIMIT_POS);
}

void DW_MCTLG_setPortRdCfg(int port, UINT32 flags, UINT32 priority)
{
   int hprEn = (flags & DW_MCTLG_PORT_HPR_EN) >> DW_MCTLG_PORT_HPR_POS;
   int pageMatchEn = (flags & DW_MCTLG_PORT_PAGE_MATCH_EN) >> DW_MCTLG_PORT_PAGE_MATCH_POS;
   int agingEn = (flags & DW_MCTLG_PORT_AGING_EN) >> DW_MCTLG_PORT_AGING_POS;
   
   if(port == 0)
   {
      PCFGR_0_RD_PORT_HPR_EN_W(hprEn);
      DDRC_MP_PCFGR_0_RD_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGR_0_RD_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGR_0_RD_PORT_PRIORITY_W(priority);
   }
   else if(port == 1)
   {
      PCFGR_1_RD_PORT_HPR_EN_W(hprEn);
      DDRC_MP_PCFGR_1_RD_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGR_1_RD_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGR_1_RD_PORT_PRIORITY_W(priority);
   }
   else if(port == 2)
   {
      PCFGR_2_RD_PORT_HPR_EN_W(hprEn);
      DDRC_MP_PCFGR_2_RD_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGR_2_RD_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGR_2_RD_PORT_PRIORITY_W(priority);
   }
   else if(port == 3)
   {
      PCFGR_3_RD_PORT_HPR_EN_W(hprEn);
      DDRC_MP_PCFGR_3_RD_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGR_3_RD_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGR_3_RD_PORT_PRIORITY_W(priority);
   }
   else if(port == 4)
   {
      PCFGR_4_RD_PORT_HPR_EN_W(hprEn);
      DDRC_MP_PCFGR_4_RD_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGR_4_RD_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGR_4_RD_PORT_PRIORITY_W(priority);
   }   
   else if(port == 5)
   {
      PCFGR_5_RD_PORT_HPR_EN_W(hprEn);
      DDRC_MP_PCFGR_5_RD_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGR_5_RD_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGR_5_RD_PORT_PRIORITY_W(priority);
   }
}

void DW_MCTLG_getPortRdCfg(int port, UINT32 *flagsP, UINT32 *priorityP)
{
   int hprEn, pageMatchEn,agingEn;
   UINT32 priority = 0;
   hprEn = pageMatchEn = agingEn = 0;
   
   if(port == 0)
   {
      hprEn       = PCFGR_0_RD_PORT_HPR_EN_R;
      pageMatchEn = PCFGR_0_RD_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGR_0_RD_PORT_AGING_EN_R;
      priority    = PCFGR_0_RD_PORT_PRIORITY_R;
   }
   else if(port == 1)
   {
      hprEn       = PCFGR_1_RD_PORT_HPR_EN_R;
      pageMatchEn = PCFGR_1_RD_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGR_1_RD_PORT_AGING_EN_R;
      priority    = PCFGR_1_RD_PORT_PRIORITY_R;
   }
   else if(port == 2)
   {
      hprEn       = PCFGR_2_RD_PORT_HPR_EN_R;
      pageMatchEn = PCFGR_2_RD_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGR_2_RD_PORT_AGING_EN_R;
      priority    = PCFGR_2_RD_PORT_PRIORITY_R;
   }
   else if(port == 3)
   {
      hprEn       = PCFGR_3_RD_PORT_HPR_EN_R;
      pageMatchEn = PCFGR_3_RD_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGR_3_RD_PORT_AGING_EN_R;
      priority    = PCFGR_3_RD_PORT_PRIORITY_R;
   }
   else if(port == 4)
   {
      hprEn       = PCFGR_4_RD_PORT_HPR_EN_R;
      pageMatchEn = PCFGR_4_RD_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGR_4_RD_PORT_AGING_EN_R;
      priority    = PCFGR_4_RD_PORT_PRIORITY_R;
   }   
   else if(port == 5)
   {
      hprEn       = PCFGR_5_RD_PORT_HPR_EN_R;
      pageMatchEn = PCFGR_5_RD_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGR_5_RD_PORT_AGING_EN_R;
      priority    = PCFGR_5_RD_PORT_PRIORITY_R;
   }

   if(flagsP)
      *flagsP = ((hprEn << DW_MCTLG_PORT_HPR_POS)| (pageMatchEn << DW_MCTLG_PORT_PAGE_MATCH_POS) | (agingEn << DW_MCTLG_PORT_AGING_POS));
   if(priorityP)
      *priorityP = priority;
}

void DW_MCTLG_setPortWrCfg(int port, UINT32 flags, UINT32 priority)
{
   int pageMatchEn = (flags & DW_MCTLG_PORT_PAGE_MATCH_EN) >> DW_MCTLG_PORT_PAGE_MATCH_POS;
   int agingEn = (flags & DW_MCTLG_PORT_AGING_EN) >> DW_MCTLG_PORT_AGING_POS;
   
   if(port == 0)
   {
      DDRC_MP_PCFGW_0_WR_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGW_0_WR_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGW_0_WR_PORT_PRIORITY_W(priority);
   }
   else if(port == 1)
   {
      DDRC_MP_PCFGW_1_WR_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGW_1_WR_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGW_1_WR_PORT_PRIORITY_W(priority);
   }
   else if(port == 2)
   {
      DDRC_MP_PCFGW_2_WR_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGW_2_WR_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGW_2_WR_PORT_PRIORITY_W(priority);
   }
   else if(port == 3)
   {
      DDRC_MP_PCFGW_3_WR_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGW_3_WR_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGW_3_WR_PORT_PRIORITY_W(priority);
   }
   else if(port == 4)
   {
      DDRC_MP_PCFGW_4_WR_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGW_4_WR_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGW_4_WR_PORT_PRIORITY_W(priority);
   }   
   else if(port == 5)
   {
      DDRC_MP_PCFGW_5_WR_PORT_PAGEMATCH_EN_W(pageMatchEn);
      DDRC_MP_PCFGW_5_WR_PORT_AGING_EN_W(agingEn);
      DDRC_MP_PCFGW_5_WR_PORT_PRIORITY_W(priority);
   }
}

void DW_MCTLG_getPortWrCfg(int port, UINT32 *flagsP, UINT32 *priorityP)
{
   int pageMatchEn,agingEn;
   UINT32 priority = 0;
   pageMatchEn = agingEn = 0;
   
   if(port == 0)
   {
      pageMatchEn = PCFGW_0_WR_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGW_0_WR_PORT_AGING_EN_R;
      priority   = PCFGW_0_WR_PORT_PRIORITY_R;
   }
   else if(port == 1)
   {
       pageMatchEn = PCFGW_1_WR_PORT_PAGEMATCH_EN_R;
       agingEn     = PCFGW_1_WR_PORT_AGING_EN_R;
       priority   = PCFGW_1_WR_PORT_PRIORITY_R;
   }
   else if(port == 2)
   {
       pageMatchEn = PCFGW_2_WR_PORT_PAGEMATCH_EN_R;
       agingEn     = PCFGW_2_WR_PORT_AGING_EN_R;
       priority   = PCFGW_2_WR_PORT_PRIORITY_R;
   }
   else if(port == 3)
   {
      pageMatchEn = PCFGW_3_WR_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGW_3_WR_PORT_AGING_EN_R;
      priority   = PCFGW_3_WR_PORT_PRIORITY_R;
   }
   else if(port == 4)
   {
      pageMatchEn = PCFGW_4_WR_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGW_4_WR_PORT_AGING_EN_R;
      priority   = PCFGW_4_WR_PORT_PRIORITY_R;
   }   
   else if(port == 5)
   {
      pageMatchEn = PCFGW_5_WR_PORT_PAGEMATCH_EN_R;
      agingEn     = PCFGW_5_WR_PORT_AGING_EN_R;
      priority   = PCFGW_5_WR_PORT_PRIORITY_R;
   }

   if(flagsP)
      *flagsP = ((pageMatchEn << DW_MCTLG_PORT_PAGE_MATCH_POS) | (agingEn << DW_MCTLG_PORT_AGING_POS));
   if(priorityP)
      *priorityP = priority;
}

void DW_MCTLG_showRegs(void)
{
   LOGG_PRINT(LOG_INFO_E, NULL,"DSE DW_UMCTL2 registers\n");
   LOGG_PRINT(LOG_INFO_E, NULL, "SCHED 0x%08x = 0x%08x\n", SCHED_REG, SCHED_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PERFHPR0 0x%08x = 0x%08x\n", PERFHPR0_REG, PERFHPR0_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PERFHPR1 0x%08x = 0x%08x\n", PERFHPR1_REG, PERFHPR1_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PERFLPR0 0x%08x = 0x%08x\n", PERFLPR0_REG, PERFLPR0_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PERFLPR1 0x%08x = 0x%08x\n", PERFLPR1_REG, PERFLPR1_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PERFWR0 0x%08x = 0x%08x\n", PERFWR0_REG, PERFWR0_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PERFWR1 0x%08x = 0x%08x\n", PERFWR1_REG, PERFWR1_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCCFG    0x%08x = 0x%08x\n", PCCFG_REG, PCCFG_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGR0   0x%08x = 0x%08x\n", PCFGR_0_REG, PCFGR_0_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGW0   0x%08x = 0x%08x\n", PCFGW_0_REG, PCFGW_0_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGR1   0x%08x = 0x%08x\n", PCFGR_1_REG, PCFGR_1_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGW1   0x%08x = 0x%08x\n", PCFGW_1_REG, PCFGW_1_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGR2   0x%08x = 0x%08x\n", PCFGR_2_REG, PCFGR_2_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGW2   0x%08x = 0x%08x\n", PCFGW_2_REG, PCFGW_2_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGR3   0x%08x = 0x%08x\n", PCFGR_3_REG, PCFGR_3_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGW3   0x%08x = 0x%08x\n", PCFGW_3_REG, PCFGW_3_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGR4   0x%08x = 0x%08x\n", PCFGR_4_REG, PCFGR_4_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGW4   0x%08x = 0x%08x\n", PCFGW_4_REG, PCFGW_4_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGR5   0x%08x = 0x%08x\n", PCFGR_5_REG, PCFGR_5_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL, "PCFGW5   0x%08x = 0x%08x\n", PCFGW_5_REG, PCFGW_5_VAL);
}
#else
ERRG_codeE DW_MCTLG_init(void)
{
   return MEM_MAP__RET_SUCCESS;
}

void DW_MCTLG_setSched(int preferWr, UINT32 rdWrIdleGap, UINT32 lprNumEntries)
{
(void)preferWr;(void)rdWrIdleGap;(void)lprNumEntries;
}
#endif

