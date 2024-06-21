/****************************************************************************
 *
 *   FileName: dpe_mngr.c
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#include <assert.h>

#include "xml_db.h"
#include "dpe_mngr.h"
#include "xml_dev.h"
#include "nu4100_dpe_regs.h"
#include "mem_map.h"
#include "hcg_mngr.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
//legacy
static DPE_MNGRG_dpeInfoT DPE_MNGRP_dpeInfo;

/****************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

static ERRG_codeE DPE_MNGRG_enable( XMLDB_dbH dbh )
{
   LOGG_PRINT(LOG_DEBUG_E,0,"dpe_mngr: enable\n");
   XMLDB_writeFieldToReg(dbh, NU4100_DPE_CFG_EN_E, 1);
   return (DPE_MNGR__RET_SUCCESS);
}

static ERRG_codeE DPE_MNGRG_disable( XMLDB_dbH dbh )
{
   LOGG_PRINT(LOG_DEBUG_E,0,"dpe_mngr: disable\n");
   XMLDB_writeFieldToReg(dbh, NU4100_DPE_CFG_EN_E, 0);
   return (DPE_MNGR__RET_SUCCESS);
}

ERRG_codeE DPE_MNGRG_dumpRegs()
{
   UINT32 regsOffset[] = {dpe_offset_tbl_values};
   UINT32 regsResetVal[] = {dpe_regs_reset_val};
   UINT32 reg;
   UINT32 dpeBaseVirtAddr, dpeBasePhysAddr;
   UINT8  buf[1024];

/*
   //to dump all the registers
   LOGG_PRINT(LOG_INFO_E, NULL, "DPE registers (number of regs = %d):\n",sizeof(regsOffset));
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg+=4)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x\n"
                                    , regsOffset[reg],   *(volatile UINT32 *)(dpeDevH.virtbase + regsOffset[reg])
                                    , regsOffset[reg+1], *(volatile UINT32 *)(dpeDevH.virtbase + regsOffset[reg+1])
                                    , regsOffset[reg+2], *(volatile UINT32 *)(dpeDevH.virtbase + regsOffset[reg+2])
                                    , regsOffset[reg+3], *(volatile UINT32 *)(dpeDevH.virtbase + regsOffset[reg+3]));
   }
*/
   MEM_MAPG_getPhyAddr(MEM_MAPG_REG_DPE_E, (MEM_MAPG_addrT*)&dpeBasePhysAddr);
   MEM_MAPG_convertPhysicalToVirtual2(dpeBasePhysAddr, &dpeBaseVirtAddr, MEM_MAPG_REG_DPE_E);

   memset(buf,0,sizeof(buf));
   sprintf(buf + strlen(buf), "Modified DPE registers:\n");
   //LOGG_PRINT(LOG_INFO_E, NULL, "Modified DPE registers:\n");
   //compare against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(dpeBaseVirtAddr + regsOffset[reg]) != regsResetVal[reg])
      {
         //LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(dpeBaseVirtAddr + regsOffset[reg]));
         if (strlen(buf) > 900)
         {
            printf("%s",buf);
            memset(buf,0,sizeof(buf));
         }
         sprintf(buf + strlen(buf), "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(dpeBaseVirtAddr + regsOffset[reg]));
      }
   }
   printf("%s",buf);

   return DPE__RET_SUCCESS;
}




ERRG_codeE DPE_MNGRG_setEnableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg, void *voterHandle )
{
   (void)block; (void)inst;(void)arg;
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_DPE);
   return DPE_MNGRG_enable( dbh );
}

ERRG_codeE DPE_MNGRG_setDisableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg )
{
   (void)block;(void)inst; (void)arg;
   
   return DPE_MNGRG_disable( dbh );
}


//legacy
DPE_MNGRG_dpeInfoT *DPE_MNGRG_getDpeInfo()
{
   return(&DPE_MNGRP_dpeInfo);
}

ERRG_codeE DPE_MNGRG_closeDpeDrv()
{
   return DPE_MNGR__RET_SUCCESS;
}
ERRG_codeE DPE_MNGRG_disableDpeInterrupts()
{
   return DPE_MNGR__RET_SUCCESS;
}
ERRG_codeE DPE_MNGRG_clearDpeInterrupts()
{
   return DPE_MNGR__RET_SUCCESS;
}
ERRG_codeE DPE_MNGRG_setDisparityRange( UINT32 minDisp, UINT32 maxDisp )
{
   (void)minDisp;
   (void)maxDisp;
   return DPE_MNGR__RET_SUCCESS;
}
void DPE_MNGRG_showDpeCfg(INU_DEFSG_logLevelE level)
{
   (void)level;
}

