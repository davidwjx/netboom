/****************************************************************************
 *
 *   FileName: mipi_mngr.c
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description: Mipi manager
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "mipi_mngr.h"
#include "mipi_drv.h"
#include "nu4k_defs.h"
#include "assert.h"
#include "iae_drv.h"
#include "gme_drv.h"


#define INDEX_TO_MIPI_INST(i) (i)

#define DIR_TX (0)
#define DIR_RX (1)
#define MIPI_DBG_LVL (LOG_DEBUG_E) //LOG_INFO_E 

static UINT32 parseCsiInst (UINT32 inst, UINT32 phySel)
{
   switch (inst)
   {
      case (0):
         if (phySel == 1)
            return 0;
         else if (phySel == 2)
            return 5;
         break;
      case (1):
         if (phySel == 1)
            return 1;
         else if (phySel == 2)
            return 3;
         else if (phySel == 3)
            return 4;
         break;
      case (2):
         if (phySel == 1)
            return 2;
         else if (phySel == 2)
            return 3;
         break;
      case (3):
         if (phySel == 1)
            return 2;
         else if (phySel == 2)
            return 3;
         break;
      case (4):
         if (phySel == 1)
            return 1;
         else if (phySel == 2)
            return 3;
         else if (phySel == 3)
            return 4;
         break;
      case (5):
         if (phySel == 1)
            return 0;
         else if (phySel == 2)
            return 1;
         else if (phySel == 3)
            return 4;
         else if (phySel == 4)
            return 5;
         break;
         //default:
           
   }
}

/****************************************************************************
*
*  Function Name: MIPI_MNGRG_setCfgRx
*
*  Description:   Configure mipi rx instance
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE MIPI_MNGRG_setConfigRx( XMLDB_dbH hwDb, UINT32 *insList, UINT32 mipiCnt, void *arg )
{
   ERRG_codeE                 ret = MIPI__RET_SUCCESS;
   UINT32 pllMhz;
   UINT32 format;
   UINT32 laneEnable;   //bit0 clklane, bit1 -lane0, bit2 - lane1
   UINT32 phySel, i ,is4lanes, csiInst;
   mipiInstConfigT mipiCfg;
   FIX_UNUSED_PARAM_WARN(arg);
   
   memset(&mipiCfg,0,sizeof(mipiInstConfigT));

   //by default, config  slu0-->phy0    slu1-->phy1    slu2-->phy2..
   IAE_DRVG_configDefaultPhySelect();
   
   (mipiCnt > 1) ? (is4lanes = 1) : (is4lanes = 0);
   
   for(i=0; i< mipiCnt; i++)
   {
      XMLDB_writeBlockDbToRegs(hwDb, NUFLD_MIPI_RX_E, insList[i]);
      GMEG_mipiDphyEnable(insList[i]);
      GME_DRVG_enableClk((GME_DRVG_hwUnitE)(GME_DRVG_HW_MIPI_DPHY0_RX_CLK_EN_E + insList[i]));

      ret = XMLDB_getValue( hwDb, NU4100_IAE_MIPI_MUX_PHY0_SEL_E + insList[i], (UINT32 *)&(phySel));
      if (ret == XMLDB__RET_SUCCESS)
      {
         //if there is value on phy_sel in the xml, config it(overwrite the default from 'IAE_DRVG_configDefaultPhySelect'function).
         //and save the csi instance: 1. to know which csi to config in 'MIPI_DRVG_cfgRx' functoin. 2. to compare in the next loop (if wxist another phy) if other csi was choosen-and fail in this case
         IAE_DRVG_configPhySelect(insList[i], phySel);
         csiInst = parseCsiInst(insList[i],phySel);
         LOGG_PRINT(LOG_INFO_E, NULL, "mipi phy %d, physel %d, csi is %d\n", insList[i], phySel, csiInst);
         if (i==0)
         {
            mipiCfg.mipiInst = csiInst;
         }
         else
         {
            if (mipiCfg.mipiInst != csiInst)//if we have tow PHYs which connected to the same sensor,  it is wrong state if they have different controller (csi)
            {
               LOGG_PRINT(LOG_ERROR_E, NULL, "two PHYs with different csi, first is %s second is %d\n",mipiCfg.mipiInst,csiInst);
               assert(0);
            }
         }
      }
      else
      {
         mipiCfg.mipiInst = (INU_DEFSG_mipiInstE)insList[i];
      }
      

      mipiCfg.dphy.direction  = DIR_RX;
      //assumption, both configured the same
      XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_MIPI_RX_E, insList[i], BUS_MIPI_CSI_RX0_LANES_E)  ,(UINT32 *)&(laneEnable));
      XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_MIPI_RX_E, insList[i], BUS_MIPI_CSI_RX0_FORMAT_E) ,(UINT32 *)&(format));
      XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_MIPI_RX_E, insList[i], BUS_MIPI_CSI_RX0_PLL_E)    ,(UINT32 *)&(pllMhz));
      

      mipiCfg.dphy.laneEnable =(UINT16 )laneEnable;
      mipiCfg.dphy.format     =(UINT16 )format;
      mipiCfg.dphy.pllMhz     =(UINT16 )pllMhz;

      
      LOGG_PRINT(LOG_INFO_E, NULL, "block MIPI RX, blkInstance = %d, laneEnable= %d, format= %d, pllMhz= %d\n", insList[i], mipiCfg.dphy.laneEnable, mipiCfg.dphy.format, mipiCfg.dphy.pllMhz);
   }

   ret = MIPI_DRVG_cfgRx(mipiCfg, is4lanes);
   return ret;
}

/****************************************************************************
*
*  Function Name: MIPI_MNGRG_getMipiRxId
*
*  Description:   Configure mipi rx instance
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
#define MIPI_RX_NUM_BLKS    (6)

ERRG_codeE MIPI_MNGRG_getMipiRxTable( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, UINT32 *mipiId, UINT32 *mipiCnt )
{
   ERRG_codeE       ret = MIPI__RET_SUCCESS;
   UINT8            i = 0;
   UINT32 sourceSelect = 0xFF;
   UINT32 medSourceSelect0,medSourceSelect1;

   *mipiId = 0xFF;
   *mipiCnt = 0;//to protect on some sensors on the same gruop, we will enter to here more than once, this cnt shold be initial to zero
   for(i = 0; i < MIPI_RX_NUM_BLKS; i++)
   {
      ret = XMLDB_getValue( hwDb, NUFLD_calcPath( block, i, BUS_MIPI_CSI_RX0_SRC_SEL_E)  ,(UINT32 *)&(sourceSelect));
      if (ERRG_SUCCEEDED(ret))
      {
         if (sourceSelect > 11)//mediators. so continue and bring mipi instance
         {
            //printf("MIPI_MNGRG_getMipiRxId src sel is mediator %d\n",sourceSelect);
            ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_MEDIATOR_E, sourceSelect - 12, MEDIATORS_MED_0_SRC_SEL0_E)  ,&(medSourceSelect0));
            if (ERRG_FAILED(ret))
            {
               return ret;
            }
            ret = XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_MEDIATOR_E, sourceSelect - 12, MEDIATORS_MED_0_SRC_SEL1_E)  ,&(medSourceSelect1));
            if (ERRG_SUCCEEDED(ret))
            {
               if((medSourceSelect0 == blkInstance) || (medSourceSelect1 == blkInstance))
               {
                  //printf("MIPI_MNGRG_getMipiRxId found mipi inst %d through mediator\n",i);
                  mipiId[(*mipiCnt)++] = i;
               }
            }
            else
            {
               return ret;
            }
         }
         else
         {
            if(sourceSelect == blkInstance)
            {
               if ((*mipiCnt) == MIPI_MNGRG_MAX_PHY_TO_SENSOR)
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "Max 2 phy connections for sensor id %d are supported\n",blkInstance);
                  assert(0);
               }
               mipiId[(*mipiCnt)++] = i;
               LOGG_PRINT(LOG_INFO_E, NULL, "found mipi inst %d for sensor %d\n",i,blkInstance);
            }
         }
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: MIPI_MNGRG_setEnableRx
*
*  Description:   Enable mipi rx instance
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE MIPI_MNGRG_setEnableRx( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE                 ret = MIPI__RET_SUCCESS;
   FIX_UNUSED_PARAM_WARN(arg);
   FIX_UNUSED_PARAM_WARN(hwDb);
   FIX_UNUSED_PARAM_WARN(block);
   FIX_UNUSED_PARAM_WARN(instanceList);
   FIX_UNUSED_PARAM_WARN(voterHandle);
   int i;

   for( i = 0; i < NU4K_NUM_MIPI_RX; i++ )
   {
      if( instanceList[i] != 0xFF )
      {
         //MIPI_MNGRG_setConfigRx(hwDb,block, instanceList[i], arg );
      }
   }

   
   return ret;
}

/****************************************************************************
*
*  Function Name: MIPI_MNGRG_setDisableRx
*
*  Description:   Disable mipi rx instance
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE MIPI_MNGRG_setDisableRx( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 *insList, UINT32 mipiCnt, void *arg )
{
   ERRG_codeE                 ret = MIPI__RET_SUCCESS;
   UINT8 i = 0;
   mipiInstConfigT            mipiCfg;
   memset(&mipiCfg,0,sizeof(mipiInstConfigT));
   FIX_UNUSED_PARAM_WARN(arg);
   FIX_UNUSED_PARAM_WARN(hwDb);
   FIX_UNUSED_PARAM_WARN(block);

   UINT8* disableAll = (UINT8*)arg;

   if (disableAll)
   {
      for( i = 0; i < NU4K_NUM_MIPI_RX; i++ )
      {
         mipiCfg.mipiInst = (INU_DEFSG_mipiInstE)i;
         ret = MIPI_DRVG_resetRx(mipiCfg);
      }
   }
   else
   {
      for( i = 0; i < mipiCnt; i++ )
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Disabling mipi inst %d\n", insList[i]);
         mipiCfg.mipiInst = (INU_DEFSG_mipiInstE)insList[i];
         ret = MIPI_DRVG_resetRx(mipiCfg);
         
   }
}
   return ret;
}

void MIPI_MNGRG_deInit(void)
{
/*   if(mipiDb.bufId != MEM_MNGRG_INVALID_BUFFER_IDX)
   {
      MEM_MNGRG_bufferClose(mipiDb.bufId);
      mipiDb.bufId = MEM_MNGRG_INVALID_BUFFER_IDX;
   }
   if(mipiDb.txEvent)
   {
      OS_LYRG_releaseEvent(mipiDb.txEvent, (UINT32)&mipiDb);
      mipiDb.txEvent = NULL;
   }*/
}



