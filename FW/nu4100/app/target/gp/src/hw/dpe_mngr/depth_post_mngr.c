/****************************************************************************
 *
 *   FileName: dpe_post_mngr.c
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#include <assert.h>

#include "xml_db.h"
#include "depth_post_mngr.h"
#include "hcg_mngr.h"
#include "xml_dev.h"
#include "nu4100_ppe_regs.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

static UINT32 ppeBaseAddress;
static UINT32 depthPostInstStat=0;

typedef enum
{
   DEPTH_POST_HOST_E,                   // 0 
   DEPTH_POST_CVA_E
}DEPTH_POST_dppInstT;


/****************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

static ERRG_codeE DEPTH_POST_enable( XMLDB_dbH dbh )
{
   XMLDB_writeFieldToReg(dbh, NU4100_PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_E,1);
   XMLDB_writeFieldToReg(dbh, NU4100_PPE_DEPTH_POST_CFG_DEPTH_POST_EN_E,1);
   return (DPE_MNGR__RET_SUCCESS);
}

static ERRG_codeE DEPTH_POST_disable( XMLDB_dbH dbh )
{
   XMLDB_writeFieldToReg(dbh, NU4100_PPE_DEPTH_POST_CFG_DEPTH_POST_EN_E,0);
   return (DPE_MNGR__RET_SUCCESS);
}

ERRG_codeE DEPTH_POST_setEnableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg, void *voterHandle )
{
   (void)block; (void)inst;(void)arg;
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_PPE);
   depthPostInstStat |= 1<<*inst;
   if ( depthPostInstStat)
      return DEPTH_POST_enable(dbh);
   return (DPE_MNGR__RET_SUCCESS);
}

ERRG_codeE DEPTH_POST_setDisableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg )
{
   (void)block;(void)inst; (void)arg;
   

   depthPostInstStat &= ~(1<<*inst);
   if ( depthPostInstStat == 0)
   {
       return DEPTH_POST_disable(dbh);
   }
   return (DPE_MNGR__RET_SUCCESS);
}

ERRG_codeE DEPTH_POSTG_init(UINT32 ppe_base)
{
   ppeBaseAddress = ppe_base; //for direct access to registers
   depthPostInstStat=0;
   return PPE_MNGR__RET_SUCCESS;
}

ERRG_codeE DEPTH_POSTG_updateDPPCrop( DEPTH_POSTG_cropParamT *crop, UINT8 dppNum )
{
   UINT16 x_s = crop->xStart;
   UINT16 y_s = crop->yStart;
   UINT16 w,h;
   
   if (dppNum == DEPTH_POST_HOST_E)//depth post host
   {
      w = PPE_DEPTH_POST_MISC_HOST_CROP_END_X_R - PPE_DEPTH_POST_MISC_HOST_CROP_START_X_R;
      h = PPE_DEPTH_POST_MISC_HOST_CROP_END_Y_R - PPE_DEPTH_POST_MISC_HOST_CROP_START_Y_R;
      PPE_DEPTH_POST_MISC_HOST_CROP_START_X_W(x_s);
      PPE_DEPTH_POST_MISC_HOST_CROP_START_Y_W(y_s);
      PPE_DEPTH_POST_MISC_HOST_CROP_END_X_W((x_s + w));
      PPE_DEPTH_POST_MISC_HOST_CROP_END_Y_W((y_s + h));
   }
   else if (dppNum == DEPTH_POST_CVA_E)//depth post cva
   {
      w = PPE_DEPTH_POST_MISC_CVA_CROP_END_X_R - PPE_DEPTH_POST_MISC_CVA_CROP_START_X_R;
      h = PPE_DEPTH_POST_MISC_CVA_CROP_END_Y_R - PPE_DEPTH_POST_MISC_CVA_CROP_START_Y_R;
      PPE_DEPTH_POST_MISC_CVA_CROP_START_X_W(x_s);
      PPE_DEPTH_POST_MISC_CVA_CROP_START_Y_W(y_s);
      PPE_DEPTH_POST_MISC_CVA_CROP_END_X_W((x_s + w));
      PPE_DEPTH_POST_MISC_CVA_CROP_END_Y_W((y_s + h));
   }
   return (DPE_MNGR__RET_SUCCESS);
}


