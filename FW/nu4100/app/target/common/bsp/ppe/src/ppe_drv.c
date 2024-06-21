/****************************************************************************
 *
 *   FileName: ppe_drv.c
 *
 *   Author:   Dima S.
 *
 *   Date:
 *
 *   Description: Inuitive PPE Driver
 *
 ****************************************************************************/



/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#ifdef __cplusplus
      extern "C" {
#endif

#include "ppe_drv.h"
#include "nu4100_regs.h"
#include "os_lyr.h"
#include "mem_map.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define CONIFGURE_CSC_PARAMS(n, ppucfg) {\
      PPE_PPU## n ##_MISC_CSC1_A11_W(ppuCfg->csc.a11);\
      PPE_PPU## n ##_MISC_CSC1_A12_W(ppuCfg->csc.a12);\
      PPE_PPU## n ##_MISC_CSC2_A13_W(ppuCfg->csc.a13);\
      PPE_PPU## n ##_MISC_CSC2_B1_W(ppuCfg->csc.b1);\
      PPE_PPU## n ##_MISC_CSC3_A21_W(ppuCfg->csc.a21);\
      PPE_PPU## n ##_MISC_CSC3_A22_W(ppuCfg->csc.a22);\
      PPE_PPU## n ##_MISC_CSC4_A23_W(ppuCfg->csc.a23);\
      PPE_PPU## n ##_MISC_CSC4_B2_W(ppuCfg->csc.b2);\
      PPE_PPU## n ##_MISC_CSC5_A31_W(ppuCfg->csc.a31);\
      PPE_PPU## n ##_MISC_CSC5_A32_W(ppuCfg->csc.a32);\
      PPE_PPU## n ##_MISC_CSC6_A33_W(ppuCfg->csc.a33);\
      PPE_PPU## n ##_MISC_CSC6_B3_W(ppuCfg->csc.b3);\
}


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
#define PPE_DRVP_MAX_NUM_ISR_TYPE_REGISTERS (5)

typedef struct
{
   PPE_DRVG_isrCbT cb;
   void            *argP;
   UINT32           blkNum;
}PPE_DRVP_isrHandleParamsT;

typedef struct
{
   PPE_DRVP_isrHandleParamsT isrHandleTbl[PPE_DRVG_ISR_MAX_TYPE_NUM][PPE_DRVP_MAX_NUM_ISR_TYPE_REGISTERS];
}PPE_DRVP_paramsT;


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
PPE_DRVP_paramsT PPE_DRVP_params;



/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/
static UINT32 ppeBaseAddress;


/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
ERRG_codeE PPE_DRVP_setupPpu0( PPE_DRVG_ppuCfgT   *ppuCfg);
ERRG_codeE PPE_DRVP_setupPpu1( PPE_DRVG_ppuCfgT   *ppuCfg);
ERRG_codeE PPE_DRVP_setupPpu2( PPE_DRVG_ppuCfgT   *ppuCfg);
ERRG_codeE PPE_DRVP_setupPpu3( PPE_DRVG_ppuCfgT   *ppuCfg);
ERRG_codeE PPE_DRVP_setupAxiReader0( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader1( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader2( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader3( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader4( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader5( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader8( PPE_DRVG_axiReaderCfgT   *readerCfg );
ERRG_codeE PPE_DRVP_setupAxiReader9( PPE_DRVG_axiReaderCfgT   *readerCfg );


/****************************************************************************
***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpu0
*
*  Description:   
*
*  Inputs:            ppuCfg
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/


ERRG_codeE PPE_DRVP_setupPpu0( PPE_DRVG_ppuCfgT   *ppuCfg)
{
   // Setup PPU control
   PPE_PPU0_MISC_PPU_CTRL_SRC_SEL_W(ppuCfg->control.srcSel);
   PPE_PPU0_MISC_PPU_CTRL_SPLIT_REP_RATE_W(ppuCfg->control.splitRepRate);

   if( ppuCfg->control.cscBypass )
   {
      PPE_PPU0_MISC_PPU_CTRL_CSC_BYPASS_W(ppuCfg->control.cscBypass);
   }
   else
   {
      PPE_PPU0_MISC_PPU_CTRL_CSC_MODE_W(ppuCfg->control.cscMode) ;
      CONIFGURE_CSC_PARAMS(0,ppuCfg);
   }
   if( ppuCfg->control.repackerBypass)
   {
      PPE_PPU0_MISC_PPU_CTRL_REPACKER_BYPASS_W(ppuCfg->control.repackerBypass);
   }
   else
   {
      // TODO: Configure Repacker
      LOGG_PRINT(LOG_ERROR_E, NULL, "Configuring Repacker\n");
   }

   // Setup PPU crop
   PPE_PPU0_MISC_CROP_START_X_W( ppuCfg->crop.startX );                  
   PPE_PPU0_MISC_CROP_START_Y_W( ppuCfg->crop.startY );                  
   PPE_PPU0_MISC_CROP_END_X_W( ppuCfg->crop.endX );                     // NOTE: Should be val-1 . Handled in NU config    
   PPE_PPU0_MISC_CROP_END_Y_W( ppuCfg->crop.endY );                     // NOTE: Should be val-1 . Handled in NU config

   return PPE__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpu0
*
*  Description:   
*
*  Inputs:            ppuCfg
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/


ERRG_codeE PPE_DRVP_setupPpu1( PPE_DRVG_ppuCfgT   *ppuCfg)
{
   // Setup PPU control
   PPE_PPU1_MISC_PPU_CTRL_SRC_SEL_W(ppuCfg->control.srcSel);
   PPE_PPU1_MISC_PPU_CTRL_SPLIT_REP_RATE_W(ppuCfg->control.splitRepRate);

   if( ppuCfg->control.cscBypass )
   {
      PPE_PPU1_MISC_PPU_CTRL_CSC_BYPASS_W(ppuCfg->control.cscBypass);
   }
   else
   {
      PPE_PPU1_MISC_PPU_CTRL_CSC_MODE_W(ppuCfg->control.cscMode);
      CONIFGURE_CSC_PARAMS(1,ppuCfg);
   }
   if( ppuCfg->control.repackerBypass)
   {
      PPE_PPU1_MISC_PPU_CTRL_REPACKER_BYPASS_W(ppuCfg->control.repackerBypass);
   }
   else
   {
      // TODO: Configure Repacker
      LOGG_PRINT(LOG_ERROR_E, NULL, "Configuring Repacker\n");
   }

   // Setup PPU crop
   PPE_PPU1_MISC_CROP_START_X_W( ppuCfg->crop.startX );                  
   PPE_PPU1_MISC_CROP_START_Y_W( ppuCfg->crop.startY );                  
   PPE_PPU1_MISC_CROP_END_X_W( ppuCfg->crop.endX );                     // NOTE: Should be val-1 . Handled in NU config    
   PPE_PPU1_MISC_CROP_END_Y_W( ppuCfg->crop.endY );                     // NOTE: Should be val-1 . Handled in NU config

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpu0
*
*  Description:   
*
*  Inputs:            ppuCfg
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/


ERRG_codeE PPE_DRVP_setupPpu2( PPE_DRVG_ppuCfgT   *ppuCfg)
{
   // Setup PPU control
   PPE_PPU2_MISC_PPU_CTRL_SRC_SEL_W(ppuCfg->control.srcSel);
   PPE_PPU2_MISC_PPU_CTRL_SPLIT_REP_RATE_W(ppuCfg->control.splitRepRate);

   if( ppuCfg->control.cscBypass )
   {
      PPE_PPU2_MISC_PPU_CTRL_CSC_BYPASS_W(ppuCfg->control.cscBypass);
   }
   else
   {
      PPE_PPU2_MISC_PPU_CTRL_CSC_MODE_W(ppuCfg->control.cscMode);
      CONIFGURE_CSC_PARAMS(2,ppuCfg);
   }
   if( ppuCfg->control.repackerBypass)
   {
      PPE_PPU2_MISC_PPU_CTRL_REPACKER_BYPASS_W(ppuCfg->control.repackerBypass);
   }
   else
   {
      // TODO: Configure Repacker
      LOGG_PRINT(LOG_ERROR_E, NULL, "Configuring Repacker\n");
   }

   // Setup PPU crop
   PPE_PPU2_MISC_CROP_START_X_W( ppuCfg->crop.startX );                  
   PPE_PPU2_MISC_CROP_START_Y_W( ppuCfg->crop.startY );                  
   PPE_PPU2_MISC_CROP_END_X_W( ppuCfg->crop.endX );                     // NOTE: Should be val-1 . Handled in NU config    
   PPE_PPU2_MISC_CROP_END_Y_W( ppuCfg->crop.endY );                     // NOTE: Should be val-1 . Handled in NU config

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpu0
*
*  Description:   
*
*  Inputs:            ppuCfg
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/


ERRG_codeE PPE_DRVP_setupPpu3( PPE_DRVG_ppuCfgT   *ppuCfg)
{
   // Setup PPU control
   PPE_PPU3_MISC_PPU_CTRL_SRC_SEL_W(ppuCfg->control.srcSel);
   PPE_PPU3_MISC_PPU_CTRL_SPLIT_REP_RATE_W(ppuCfg->control.splitRepRate);

   if( ppuCfg->control.cscBypass )
   {
      PPE_PPU3_MISC_PPU_CTRL_CSC_BYPASS_W(ppuCfg->control.cscBypass);
   }
   else
   {
      PPE_PPU3_MISC_PPU_CTRL_CSC_MODE_W(ppuCfg->control.cscMode);
      CONIFGURE_CSC_PARAMS(3,ppuCfg);
   }
   if( ppuCfg->control.repackerBypass)
   {
      PPE_PPU3_MISC_PPU_CTRL_REPACKER_BYPASS_W(ppuCfg->control.repackerBypass);
   }
   else
   {
      // TODO: Configure Repacker
      LOGG_PRINT(LOG_ERROR_E, NULL, "Configuring Repacker\n");
   }

   // Setup PPU crop
   PPE_PPU3_MISC_CROP_START_X_W( ppuCfg->crop.startX );                  
   PPE_PPU3_MISC_CROP_START_Y_W( ppuCfg->crop.startY );                  
   PPE_PPU3_MISC_CROP_END_X_W( ppuCfg->crop.endX );                     // NOTE: Should be val-1 . Handled in NU config    
   PPE_PPU3_MISC_CROP_END_Y_W( ppuCfg->crop.endY );                     // NOTE: Should be val-1 . Handled in NU config

   return PPE__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader0
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader0( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ0_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ0_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ0_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ0_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ0_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ0_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ0_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ0_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ0_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ0_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ0_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ0_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 
   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader0
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader1( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ1_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ1_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ1_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ1_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ1_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ1_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ1_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ1_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ1_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ1_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ1_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ1_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader0
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader2( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ2_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ2_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ2_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ2_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ2_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ2_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ2_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ2_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ2_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ2_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ2_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ2_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader0
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader3( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ3_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ3_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ3_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ3_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ3_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ3_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ3_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ3_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ3_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ3_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ3_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ3_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader0
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader4( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ4_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ4_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ4_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ4_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ4_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ4_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ4_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ4_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ4_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ4_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ4_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ4_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader0
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader5( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ5_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ5_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ5_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ5_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ5_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ5_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ5_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ5_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ5_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ5_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ5_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ5_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader8
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVP_setupAxiReader8( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ8_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ8_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ8_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ8_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ8_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ8_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ8_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ8_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ8_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ8_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ8_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ8_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: PPE_DRVP_setupAxiReader9
*
*  Description:   Setup AXI read buffer
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE PPE_DRVP_setupAxiReader9( PPE_DRVG_axiReaderCfgT   *readerCfg )
{
   PPE_AXI_READ9_CTRL_DATA_SEL_W( readerCfg->control.srcSel );               
   PPE_AXI_READ9_CTRL_INT_STRM_EN_W( readerCfg->control.interleaveEnable );         // en
   PPE_AXI_READ9_CTRL_PXL_DATA_WIDTH_W( readerCfg->control.pixelDataWidth );          // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit
   PPE_AXI_READ9_CTRL_PXL_INT_WIDTH_W( readerCfg->control.pixelInterleaveWidth );      // 00 - 8bit 01 - 12bit, 10 - 16bit, 11 - 24bit   
   PPE_AXI_READ9_CTRL_MUTUAL_STRMS_W( readerCfg->control.mutualStrms );            // Need to configure mutual streams for recovery. 
   PPE_AXI_READ9_CTRL_ERROR_RECOVERY_EN_W( readerCfg->control.errorRecoveryEn );      // enable recovery mechanism
   PPE_AXI_READ9_AXI_CFG_BURST_LEN_W( readerCfg->burstLen );       
   PPE_AXI_READ9_AXI_CFG_SINGLE_END_W( readerCfg->singleEnd );                     // 0/1 DMA/PPE control the axi transaction

   PPE_AXI_READ9_INT_ENABLE_FRAME_START_W( readerCfg->interrupt.frame_start );
   PPE_AXI_READ9_INT_ENABLE_FRAME_END_W( readerCfg->interrupt.frame_end );
   PPE_AXI_READ9_INT_ENABLE_LINE_BUFFER_OVERFLOW_W( readerCfg->interrupt.line_buffer_overflow ); 
   
   PPE_AXI_READ9_AXI_BEAT_LAST_VAL_W( readerCfg->axiBeatLast );                  // NOTE: Should be val-1 . Handled in NU config 

   return PPE__RET_SUCCESS;
}


static void PPE_DRVP_handleAxiReadIsr(PPE_DRVP_paramsT *PPE_DRVP_paramsP, UINT64 timestamp, UINT64 count, void *argP, UINT32 num)
{
   UINT32 reader0IsrStatusAddress = (ppeBaseAddress + 0x9010 + (num * 0x100));//ease access 0x100 offset between reader isr registers
   UINT32 reader0IsrClearAddress = (ppeBaseAddress + 0x9018 + (num * 0x100));  
   volatile UINT32 isrStatus = *(volatile UINT32*)(reader0IsrStatusAddress);
   UINT32 i;

   if (isrStatus & 0x6)
   {
      for (i = 0; i < PPE_DRVP_MAX_NUM_ISR_TYPE_REGISTERS; i++)
      {
         if ((PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_AXI_READER][i].cb) &&
             (PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_AXI_READER][i].blkNum == num))
         {
            PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_AXI_READER][i].cb(timestamp,num,PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_AXI_READER][i].argP);
         }
      }
   }
   *(volatile UINT32*)reader0IsrClearAddress = 0x6;
}


static void PPE_DRVP_handlePpeMiscIsr(PPE_DRVP_paramsT *PPE_DRVP_paramsP, UINT64 timestamp, UINT64 count, void *argP)
{
   volatile UINT32 isrMistStatus;
   isrMistStatus = PPE_PPE_MISC_INT_VAL;
   //printf("%llu: isr 0x%x\n",timestamp,isrMistStatus);
   *PPE_PPE_MISC_INT_CLEAR_REG = 0xFF;
}

static void PPE_DRVP_handlePpeVscMipiIsr(PPE_DRVP_paramsT *PPE_DRVP_paramsP, UINT64 timestamp, UINT64 count, void *argP, UINT32 num)
{
   UINT32 vscIsrStatusAddress = (ppeBaseAddress + 0x7020 + (num * 0x80));//ease access 0x80 offset between vsc mipi blocks
   UINT32 vscIsrClearAddress  = (ppeBaseAddress + 0x7028 + (num * 0x80)); 
   volatile UINT32 isrStatus = *(volatile UINT32*)(vscIsrStatusAddress);
   UINT32 i;
   //printf("%llu: vsc mipi %d isr 0x%x\n",timestamp,num,isrStatus);

   if (isrStatus & 0x1)
   {
      printf("%llu: vsc mipi %d isr 0x%x OVERFLOW!\n",timestamp,num,isrStatus);
   }

   //HW bug workaround - the end of frame ISR will pop with the frame start (even if it is not enabled) when using virtual channels.
   //if (isrStatus != 0x6)
   {
      //if need to register on more events, add more handles and activate according to bit in status reg
      for (i = 0; i < PPE_DRVP_MAX_NUM_ISR_TYPE_REGISTERS; i++)
      {
         if ((PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_MIPI_VSC_FRAME_END][i].cb) &&
             (PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_MIPI_VSC_FRAME_END][i].blkNum == num))
         {
            PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_MIPI_VSC_FRAME_END][i].cb(timestamp,num,PPE_DRVP_paramsP->isrHandleTbl[PPE_DRVG_ISR_MIPI_VSC_FRAME_END][i].argP);
         }
      }
   }
   *(volatile UINT32*)vscIsrClearAddress = isrStatus;
}



static void PPE_DRVP_isr(UINT64 timestamp, UINT64 count, void *argP)
{
   PPE_DRVP_paramsT *PPE_DRVP_paramsP = (PPE_DRVP_paramsT*)argP;

   (void)count;
   UINT32 l1Isr = PPE_PPE_MISC_INTERRUPT_STATUS_VAL;
   UINT32 num = 0;
   //printf("%llu: ppe isr: L1 0x%x\n",timestamp,l1Isr);
   
   while(l1Isr)
   {
      if (l1Isr & 0x1)
      {
         switch(num)
         {
            //0..11 axi read
            case(0):
            case(1):
            case(2):
            case(3):
            case(4):
            case(5):
            case(6):
            case(7):
            case(8):
            case(9):
            case(10):
            case(11):
            {
               PPE_DRVP_handleAxiReadIsr(PPE_DRVP_paramsP, timestamp, count, argP, num);
               break;
            }
            case(15):
            {
               PPE_DRVP_handlePpeMiscIsr(PPE_DRVP_paramsP, timestamp, count, argP);
               break;              
            }
            case(16):
            case(17):
            case(18):
            case(19):
            case(20):
            case(21):
            case(22):
            case(23):
            case(24):
            case(25):
            case(26):
            case(27):
            {
               PPE_DRVP_handlePpeVscMipiIsr(PPE_DRVP_paramsP, timestamp, count, argP, num - 16);
               break;              
            }
            default:
               LOGG_PRINT(LOG_ERROR_E, NULL, "ignore handle of ppe isr %d\n",num);
               break;
         }
      }
      
      l1Isr >>= 1;
      num++;
   }
}

/****************************************************************************
 ***********************      G L O B A L     F U N C T I O N S     *********************
 ****************************************************************************/
/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpuWithoutScl
*
*  Description:   Setup PPU without scaler (ppus 0 - 3)
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/
ERRG_codeE PPE_DRVG_init( UINT32 memVirtAddr )
{
   ERRG_codeE         retCode  = PPE__RET_SUCCESS;
   
   //MEM_MAPG_getVirtAddr(MEM_MAPG_REG_PPE_E, (&memVirtAddr));
   ppeBaseAddress = (UINT32)memVirtAddr;

   memset(&PPE_DRVP_params,0,sizeof(PPE_DRVP_params));

   retCode = OS_LYRG_intCtrlRegister(OS_LYRG_INT_PPE_E, PPE_DRVP_isr, &PPE_DRVP_params);

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "failed to register iae isr\n");
   }
   else
   {
      retCode = OS_LYRG_intCtrlEnable(OS_LYRG_INT_PPE_E);
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "failed to enable iae isr\n");
      }
   }

   

   return retCode;
}

/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpuWithoutScl
*
*  Description:   Setup PPU without scaler (ppus 0 - 3)
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVG_setupPpuWithoutScl( UINT8 ppuNum, PPE_DRVG_ppuCfgT   *ppuCfg)
{
   ERRG_codeE retCode = PPE__RET_SUCCESS;

   switch( ppuNum )
   {
      case 0:
         PPE_DRVP_setupPpu0( ppuCfg );
      break;
      case 1:
         PPE_DRVP_setupPpu1( ppuCfg );
      break;
      case 2:
         PPE_DRVP_setupPpu2( ppuCfg );
      break;
      case 3:
         PPE_DRVP_setupPpu3( ppuCfg );
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Nothing to do for default (%d)\n",ppuNum);
      break;
   }
   return retCode;
}

/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpuWithoutScl
*
*  Description:   Setup PPU without scaler (ppus 0 - 3)
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVG_setupAxiReader(  UINT8 readerNum, PPE_DRVG_axiReaderCfgT   *readerCfg)
{
   ERRG_codeE retCode = PPE__RET_SUCCESS;

   switch( readerNum )
   {
      case 0:
         PPE_DRVP_setupAxiReader0( readerCfg );
      break;
      case 1:
         PPE_DRVP_setupAxiReader1( readerCfg );
      break;
      case 2:
         PPE_DRVP_setupAxiReader2( readerCfg );
      break;
      case 3:
         PPE_DRVP_setupAxiReader3( readerCfg );
      break;
      case 4:
         PPE_DRVP_setupAxiReader4( readerCfg );
      break;
      case 5:
         PPE_DRVP_setupAxiReader5( readerCfg );
      break;
      case 8:
         PPE_DRVP_setupAxiReader8( readerCfg );
      break;
      case 9:
         PPE_DRVP_setupAxiReader9( readerCfg );
      break;

      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Nothing to do for default (%d)\n",readerNum);
      break;
   }
 
   LOGG_PRINT(LOG_INFO_E, 0, "set ppe ready\n"); 
   PPE_DRVG_ppeReady();
   return retCode;

}

UINT8 PPE_DRVG_readOverflow(UINT8 readerNum)
{
   UINT8 retCode=0;
   switch (readerNum)
   {
      case 0:
         retCode = PPE_AXI_READ0_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 1:
         retCode = PPE_AXI_READ1_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 2:
         retCode = PPE_AXI_READ2_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 3:
         retCode = PPE_AXI_READ3_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 4:
         retCode = PPE_AXI_READ4_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 5:
         retCode = PPE_AXI_READ5_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 8:
         retCode = PPE_AXI_READ8_INT_LINE_BUFFER_OVERFLOW_R;
      break;
      case 9:
         retCode = PPE_AXI_READ9_INT_LINE_BUFFER_OVERFLOW_R;
      break;
   }
    return retCode;
}


void PPE_DRVG_showStats()
{
   unsigned int ind;
   LOGG_PRINT(LOG_INFO_E, NULL, "----------Axi Overflows statistics:\n");
   for (ind=0;ind<PPE_MAX_AXI_READER;ind++)
   {
      if (PPE_DRVG_readOverflow(ind))
         LOGG_PRINT(LOG_INFO_E, NULL, "Overflows on AXI reader %d\n",ind);
   }
   LOGG_PRINT(LOG_INFO_E, NULL, "-----Finish Axi Overflows statistics----\n");
}
/****************************************************************************
*
*  Function Name: PPE_DRVG_setupPpuWithoutScl
*
*  Description:   Setup PPU without scaler (ppus 0 - 3)
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: 
*
****************************************************************************/

ERRG_codeE PPE_DRVG_enablePpu(   PPE_DRVG_ppuEnableU enable )
{
   PPE_PPE_MISC_PPE_CTRL_VAL |= enable.word;
   
   return PPE__RET_SUCCESS;
}


void PPE_DRVG_updateWriterFrameId ( UINT8 writerNum, UINT32 frameId )
{
   switch (writerNum)
   {
      case 0:
         PPE_AXI_WRITE0_FRAME_ID0_ID_W(frameId);
      break;
      case 1:
         PPE_AXI_WRITE1_FRAME_ID0_ID_W(frameId);
      break;
      case 2:
         PPE_AXI_WRITE2_FRAME_ID0_ID_W(frameId);
      break;
      case 3:
         PPE_AXI_WRITE3_FRAME_ID0_ID_W(frameId);
      break;
      case 4:
         PPE_AXI_WRITE4_FRAME_ID0_ID_W(frameId);
      break;
      case 5:
         PPE_AXI_WRITE5_FRAME_ID0_ID_W(frameId);
      break;
      default:
         break;
   }
}
void PPE_DRVG_updateWriterTimestamp ( UINT8 writerNum, UINT64 timestamp )
{
   UINT32 LSW = timestamp & 0xFFFFFFFF;
   UINT32 MSW = (timestamp & 0xFFFFFFFF00000000) >> 32 ;
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Updating timestamp for %lu writerNum to be  %x, %x \n",writerNum,LSW,MSW );
   switch (writerNum)
   {
      case 0:
         PPE_AXI_WRITE0_FRAME_TS0_LSB_TS_W(LSW);
         PPE_AXI_WRITE0_FRAME_TS0_MSB_TS_W(MSW);
      break;
      case 1:
         PPE_AXI_WRITE1_FRAME_TS0_LSB_TS_W(LSW);
         PPE_AXI_WRITE1_FRAME_TS0_MSB_TS_W(MSW);
      break;
      case 2:
         PPE_AXI_WRITE2_FRAME_TS0_LSB_TS_W(LSW);
         PPE_AXI_WRITE2_FRAME_TS0_MSB_TS_W(MSW);
      break;
      case 3:
         PPE_AXI_WRITE3_FRAME_TS0_LSB_TS_W(LSW);
         PPE_AXI_WRITE3_FRAME_TS0_MSB_TS_W(MSW);
      break;
      case 4:
         PPE_AXI_WRITE4_FRAME_TS0_LSB_TS_W(LSW);
         PPE_AXI_WRITE4_FRAME_TS0_MSB_TS_W(MSW);
      break;
      case 5:
         PPE_AXI_WRITE5_FRAME_TS0_LSB_TS_W(LSW);
         PPE_AXI_WRITE5_FRAME_TS0_MSB_TS_W(MSW);
      break;
      default:
         break;
   }
}

void PPE_DRVG_csiControllerTxCfg(UINT32 tx_num, UINT32 pktSize, UINT32 videoFormat, UINT32 minDelayBetweenPckts)
{
   // configure CSI tx controller
   switch(tx_num)
   {
     case 0: 
       PPE_PPE_MISC_MIPI_CSI_TX0_CONTROL_PACKET_LENGTH_W(pktSize);                           // # pixels * bit-per-pixel / 8
       PPE_PPE_MISC_MIPI_CSI_TX0_CONTROL_VIDEO_FORMAT_W(videoFormat);
       PPE_PPE_MISC_MIPI_CSI_TX0_CONTROL_USE_EXTERNAL_PARAM_W(1);                            // use external parameters
       PPE_PPE_MISC_MIPI_CSI_TX0_CONTROL1_MIN_DELAY_BETWEEN_PACKETS_W(minDelayBetweenPckts); // min delay between packets
       //PPE_VSC_CSI2_VSC_CTRL2_WEIGHT_STEP_W(1);
       break;
     case 2: 
       PPE_PPE_MISC_MIPI_CSI_TX1_CONTROL_PACKET_LENGTH_W(pktSize);                           // # pixels * bit-per-pixel / 8
       PPE_PPE_MISC_MIPI_CSI_TX1_CONTROL_VIDEO_FORMAT_W(videoFormat);
       PPE_PPE_MISC_MIPI_CSI_TX1_CONTROL_USE_EXTERNAL_PARAM_W(1);                            // use external parameters
       PPE_PPE_MISC_MIPI_CSI_TX1_CONTROL1_MIN_DELAY_BETWEEN_PACKETS_W(minDelayBetweenPckts); // min delay between packets
       //PPE_VSC_CSI4_VSC_CTRL2_WEIGHT_STEP_W(1);
       break;
     case 3: 
       PPE_PPE_MISC_MIPI_CSI_TX2_CONTROL_PACKET_LENGTH_W(pktSize);                           // # pixels * bit-per-pixel / 8
       PPE_PPE_MISC_MIPI_CSI_TX2_CONTROL_VIDEO_FORMAT_W(videoFormat);
       PPE_PPE_MISC_MIPI_CSI_TX2_CONTROL_USE_EXTERNAL_PARAM_W(1);                            // use external parameters
       PPE_PPE_MISC_MIPI_CSI_TX2_CONTROL1_MIN_DELAY_BETWEEN_PACKETS_W(minDelayBetweenPckts); // min delay between packets
       //PPE_VSC_CSI8_VSC_CTRL2_WEIGHT_STEP_W(1);
       break;
   }
}



void PPE_DRVG_mipiViEn(INT32 ViNum, UINT32 intMode)
{
   switch(ViNum)
   {
      case(0):
         // interleaving VSC2, and VSC6
         if(PPE_PPE_MISC_MIPI_VI0_CONTROL_INT_MODE_R != intMode)
         {
            PPE_PPE_MISC_MIPI_VI0_CONTROL_INT_MODE_W(intMode);
         }
      break;
      case(1):
         // interleaving VSC5, and VSC6
         if(PPE_PPE_MISC_MIPI_VI1_CONTROL_INT_MODE_R != intMode)
         {
            PPE_PPE_MISC_MIPI_VI1_CONTROL_INT_MODE_W(intMode);
         }
      break;
      default:
         break;

   }
}


void PPE_DRVG_mipiViDis(INT32 ViNum)
{
   switch(ViNum)
   {
      case(0):
         // interleaving VSC2, and VSC6
         PPE_PPE_MISC_MIPI_VI0_CONTROL_INT_MODE_W(0);
         PPE_VSC_CSI2_VSC_CTRL_ENABLE_W(0);
         PPE_VSC_CSI3_VSC_CTRL_ENABLE_W(0);
      break;
      case(1):
         // interleaving VSC5, and VSC6
         PPE_PPE_MISC_MIPI_VI1_CONTROL_INT_MODE_W(0);
      break;
      default:
         break;

   }
}


void PPE_DRVG_mipiVscCsiTxEn( UINT32 vscNum, PPE_DRVG_isrCbT cb, void *argP, void **handle )
{
   UINT32 vscHwNum;
   switch(vscNum)
   {
      case 0: 
       PPE_VSC_CSI2_VSC_CTRL_ENABLE_W(1);
       vscHwNum = 2;
       break;
      case 1: 
       PPE_VSC_CSI3_VSC_CTRL_ENABLE_W(1);
       vscHwNum = 3;
       break;
      case 2: 
       PPE_VSC_CSI5_VSC_CTRL_ENABLE_W(1);
       vscHwNum = 5;
       break;
      case 3: 
       PPE_VSC_CSI6_VSC_CTRL_ENABLE_W(1);
       vscHwNum = 6;
       break;
      default:
       LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to enable VSC CSI (%d)\n",vscNum);
       return;
      break;
   }

   
   PPE_DRVG_registerIsr(PPE_DRVG_ISR_MIPI_VSC_FRAME_END, vscHwNum, cb, argP, handle);
}


void PPE_DRVG_mipiVscCsiTxDis( UINT32 vscNum, void *handle )
{
#if 0
   switch(vscNum)
   {
     case 0: 
       PPE_VSC_CSI2_VSC_CTRL_ENABLE_W(0);
       break;
     case 1: 
       PPE_VSC_CSI3_VSC_CTRL_ENABLE_W(0);
       break;
     case 2: 
       PPE_VSC_CSI5_VSC_CTRL_ENABLE_W(0);
       break;
     case 3: 
       PPE_VSC_CSI6_VSC_CTRL_ENABLE_W(0);
       break;
     default:
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to disable VSC CSI (%d)\n",vscNum);
        return;
     break;
  }
#endif
  PPE_DRVG_unregisterIsr(handle);
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_iaeReady
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
ERRG_codeE PPE_DRVG_ppeReady( void )
{
   PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_W(1);
   return PPE__RET_SUCCESS;
}

ERRG_codeE PPE_DRVG_registerIsr(PPE_DRVG_isrTypeE isrType, UINT32 blkNum, PPE_DRVG_isrCbT cb, void* argP, void **handle)
{
   UINT32 i;
   ERRG_codeE ret = PPE__RET_SUCCESS;

   for (i = 0; i < PPE_DRVP_MAX_NUM_ISR_TYPE_REGISTERS; i++)
   {
      if (!PPE_DRVP_params.isrHandleTbl[isrType][i].cb)
      {
         PPE_DRVP_params.isrHandleTbl[isrType][i].cb     = cb;
         PPE_DRVP_params.isrHandleTbl[isrType][i].argP   = argP;
         PPE_DRVP_params.isrHandleTbl[isrType][i].blkNum = blkNum;
         *handle = &PPE_DRVP_params.isrHandleTbl[isrType][i];
         //printf("registerd isr (%p): type %d, argP %p, blkNum %d\n",*handle,isrType,argP,blkNum);
         return PPE__RET_SUCCESS;
      }
   }
   return PPE__ERR_OUT_OF_RSRCS;
}


void PPE_DRVG_unregisterIsr(void *handle)
{
   //protect against user miss behaviour
   if (((UINT32)handle == 0xffffffff) || (handle == NULL))
      return;

   PPE_DRVP_isrHandleParamsT *isrHandleEntry = (PPE_DRVP_isrHandleParamsT*)handle;
   //printf("unregister %d\n",isrHandleEntry->blkNum);
   memset(isrHandleEntry,0,sizeof(PPE_DRVP_isrHandleParamsT));
}

#if 0
#define FRAME_WIDTH  1248
#define FRAME_HEIGHT 776
//#define FRAME_WIDTH  16
//#define FRAME_HEIGHT 4
#define FRAME_NUM 1
#define SOF_BLANK 1100
#define EOF_BLANK 1100
#define LINE_BLANK 640
#define RAW12 0x2c
void hackVsc()
{
     /* TX Path */
       
     // vcs 0 control
     PPE_VSC_CSI2_VSC_CTRL_DATA_SEL_W(6);                                       // src sel - iau 0
     PPE_VSC_CSI2_VSC_CTRL_ENABLE_W(1);                                         // en  // Disable if DPHY is not active!
     PPE_VSC_CSI2_VSC_FRAME_SIZE_HORIZONTAL_LENGTH_W(FRAME_WIDTH-1);            // frame width
     PPE_VSC_CSI2_VSC_FRAME_SIZE_VERTICAL_LENGTH_W(FRAME_HEIGHT-1);             // frame height
     PPE_VSC_CSI2_VSC_BLANKS_FRAME_BLANK_W(FRAME_HEIGHT/2);                     // frame blank
     PPE_VSC_CSI2_VSC_BLANKS_LINE_BLANK_W(LINE_BLANK);                          // line blank
     PPE_VSC_CSI2_VSC_BLANKS2_SOF2LINE0_BLANK_W(SOF_BLANK)                      // SOF blank
     PPE_VSC_CSI2_VSC_BLANKS2_LASTLINE2EOF_BLANK_W(EOF_BLANK)                   // EOF blank
     PPE_VSC_CSI2_CSI_CTRL_PACKET_LENGTH_W(FRAME_WIDTH*12/8);   
     PPE_VSC_CSI2_CSI_CTRL_VIDEO_FORMAT_W(RAW12);   
     PPE_VSC_CSI2_CSI_CTRL_VIRTUAL_CHANNEL_W(0);   
   
     // vcs 3 control
     PPE_VSC_CSI3_VSC_CTRL_DATA_SEL_W(7);                                       // src sel - iau 1
     PPE_VSC_CSI3_VSC_CTRL_ENABLE_W(1);                                         // en  // Disable if DPHY is not active!
     PPE_VSC_CSI3_VSC_FRAME_SIZE_HORIZONTAL_LENGTH_W(FRAME_WIDTH-1);            // frame width
     PPE_VSC_CSI3_VSC_FRAME_SIZE_VERTICAL_LENGTH_W(FRAME_HEIGHT-1);             // frame height
     PPE_VSC_CSI3_VSC_BLANKS_FRAME_BLANK_W(FRAME_HEIGHT/2);                     // frame blank
     PPE_VSC_CSI3_VSC_BLANKS_LINE_BLANK_W(LINE_BLANK);                          // line blank
     PPE_VSC_CSI3_VSC_BLANKS2_SOF2LINE0_BLANK_W(SOF_BLANK)                      // SOF blank
     PPE_VSC_CSI3_VSC_BLANKS2_LASTLINE2EOF_BLANK_W(EOF_BLANK)                   // EOF blank
     PPE_VSC_CSI3_CSI_CTRL_PACKET_LENGTH_W(FRAME_WIDTH*12/8);   
     PPE_VSC_CSI3_CSI_CTRL_VIDEO_FORMAT_W(RAW12);   
     PPE_VSC_CSI3_CSI_CTRL_VIRTUAL_CHANNEL_W(1);   

printf("change from %x\n",PPE_PPE_MISC_MIPI_VI0_CONTROL_VAL);
   
       // interleaver control
     PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_W(0xC);                              // interleaving VSC2, and VC3
   
     // bypass vsc interleaving bug - configure interleaving mode to (3) virtual channels
     PPE_PPE_MISC_MIPI_VI0_CONTROL_INT_MODE_W(3);                                // virtual channel interleave
   
     // dphy tx input sel
     PPE_PPE_MISC_PPE_CTRL_MIPI_TX_DPHY0_SEL_W(0);                               // Tx DPHY0 src sel -> CSI0
}
#endif
#ifdef __cplusplus
 }
#endif


