/****************************************************************************
 *
 *   FileName: ppe_mngr.c
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description: ppe control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/

#ifdef __cplusplus
   extern "C" {
#endif

#include "nu4k_defs.h"
#include "inu_common.h"
#include "nu4100_ppe_regs.h"
#include "ppe_mngr.h"
#include "ppe_drv.h"
#include "xml_dev.h"

#include "hcg_mngr.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
#define PPU_NOSCL_NUM_XML_FIELDS   (117)
#define PPU_SCL_NUM_XML_FIELDS     (131)
#define PPU_HYBSCL_NUM_XML_FIELDS  (179)
#define PPU_HYB_NUM_XML_FIELDS     (151)

static UINT32 ppeBaseAddress;

static void setPpuEnable(NUFLD_blkE block, UINT8* instanceList, int on)
{
   unsigned int numInst;
   unsigned int i;
   UINT32 en = 0;
   UINT32 current = 0;
   
   if(block == NUFLD_PPU_NOSCL_E) numInst = NU4K_NUM_PPUS_NOSCL;
   else if(block == NUFLD_PPU_SCL_E) numInst = NU4K_NUM_PPUS_SCL;
   else if(block == NUFLD_PPU_HYBSCL_E) numInst = NU4K_NUM_PPUS_HYBSCL;
   else if(block == NUFLD_PPU_HYB_E) numInst = NU4K_NUM_PPUS_HYB;
   else
      return;

   if(block == NUFLD_PPU_NOSCL_E) 
   {
      current = PPE_PPE_MISC_PPE_CTRL_PPU_WITHOUT_SCL_EN_R;
   }
   else if(block == NUFLD_PPU_SCL_E) 
   {
      current = PPE_PPE_MISC_PPE_CTRL_PPU_WITH_SCL_EN_R;
   }
   else if(block == NUFLD_PPU_HYBSCL_E) 
   {
      current = PPE_PPE_MISC_PPE_CTRL_PPU_WITH_HYBSCL_EN_R;
   }
   else if(block == NUFLD_PPU_HYB_E) 
   {
      current = PPE_PPE_MISC_PPE_CTRL_PPU_WITH_HYB_EN_R;
   }


   for(i = 0; i < numInst; i++)
   {
      if (instanceList[i] != 0xFF)
      {
         en |= (1 << instanceList[i]);
      }
   }

   if (on)
   {
      en = en | current;
   }
   else
   {
      en = current & (~en);
   }

   if(block == NUFLD_PPU_NOSCL_E) 
   {
      PPE_PPE_MISC_PPE_CTRL_PPU_WITHOUT_SCL_EN_W(en);
   }
   else if(block == NUFLD_PPU_SCL_E) 
   {
      PPE_PPE_MISC_PPE_CTRL_PPU_WITH_SCL_EN_W(en);
   }
   else if(block == NUFLD_PPU_HYBSCL_E) 
   {
      PPE_PPE_MISC_PPE_CTRL_PPU_WITH_HYBSCL_EN_W(en);
   }
   else if(block == NUFLD_PPU_HYB_E) 
   {
      PPE_PPE_MISC_PPE_CTRL_PPU_WITH_HYB_EN_W(en);
   }
   
   return;
}

ERRG_codeE PPE_MNGRG_init(UINT32 ppe_base)
{
   int ret = 0;
   ppeBaseAddress = ppe_base; //for direct access to registers

   PPE_DRVG_init(ppeBaseAddress);// for allowing access to PPE registers

   return (ret == 0) ? PPE_MNGR__RET_SUCCESS: PPE_MNGR__ERR_OUT_OF_RSRCS;
}


ERRG_codeE PPE_MNGRG_setConfigPpu(XMLDB_dbH dbh, NUFLD_blkE block, UINT32 inst, void *arg)
{
   (void)arg;(void)dbh;(void)block;(void)inst;
   ERRG_codeE ret = PPE_MNGR__RET_SUCCESS;

   return ret;
}

ERRG_codeE PPE_MNGRG_setEnablePpu(XMLDB_dbH db, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   (void)arg; (void)db;
   
   //set ready
   PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_W(1);

   //set enables for the block type
   setPpuEnable(block,instanceList,1);

   //simple implementation for now, use cde isr as down vote
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_PPE);
  
   return (PPE_MNGR__RET_SUCCESS);
}


ERRG_codeE PPE_MNGRG_setDisablePpu(XMLDB_dbH db, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   (void)arg; (void)db;

   //set disables the for block type
   setPpuEnable(block,instanceList,0);

   return (PPE_MNGR__RET_SUCCESS);
}

#ifdef __UART_ON_FPGA__
static void writeReg(UINT32 inst, UINT32 singleEnd, UINT32 beatLast)
{
   switch(inst)
   {
   case(0):
       PPE_AXI_READ0_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ0_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(1):
       PPE_AXI_READ1_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ1_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(2):
       PPE_AXI_READ2_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ2_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(3):
       PPE_AXI_READ3_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ3_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(4):
       PPE_AXI_READ4_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ4_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(5):
       PPE_AXI_READ5_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ5_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(8):
       PPE_AXI_READ8_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ8_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   
   case(9):
       PPE_AXI_READ9_AXI_CFG_SINGLE_END_W(singleEnd);
       PPE_AXI_READ9_AXI_BEAT_LAST_VAL_W(beatLast);
   break;
   }
}
#endif

void PPE_MNGRG_setReaderBurstLen( UINT8 inst, UINT32 len )
{
   LOGG_PRINT(LOG_INFO_E, NULL, "Set burst len %d to reader %d\n", len, inst);

   switch(inst)
   {
      case 0:
         PPE_AXI_READ0_AXI_CFG_BURST_LEN_W(len);
      break;
      case 1:
         PPE_AXI_READ1_AXI_CFG_BURST_LEN_W(len);
      break;
      case 2:
         PPE_AXI_READ2_AXI_CFG_BURST_LEN_W(len);
      break;
      case 3:
         PPE_AXI_READ3_AXI_CFG_BURST_LEN_W(len);
      break;
      case 4:
         PPE_AXI_READ4_AXI_CFG_BURST_LEN_W(len);
      break;
      case 5:
         PPE_AXI_READ5_AXI_CFG_BURST_LEN_W(len);
      break;
      case 8:
         PPE_AXI_READ8_AXI_CFG_BURST_LEN_W(len);
      break;
      case 9:
         PPE_AXI_READ9_AXI_CFG_BURST_LEN_W(len);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;         
   }

}

void PPE_MNGRG_setReaderSingleEnd( UINT8 inst, UINT32 val )
{
   LOGG_PRINT(LOG_INFO_E, NULL, "Set single end %d to reader %d\n", val, inst);

   switch(inst)
   {
      case 0:
         PPE_AXI_READ0_AXI_CFG_SINGLE_END_W(val);
      break;
      case 1:
         PPE_AXI_READ1_AXI_CFG_SINGLE_END_W(val);
      break;
      case 2:
         PPE_AXI_READ2_AXI_CFG_SINGLE_END_W(val);
      break;
      case 3:
         PPE_AXI_READ3_AXI_CFG_SINGLE_END_W(val);
      break;
      case 4:
         PPE_AXI_READ4_AXI_CFG_SINGLE_END_W(val);
      break;
      case 5:
         PPE_AXI_READ5_AXI_CFG_SINGLE_END_W(val);
      break;
      case 8:
         PPE_AXI_READ8_AXI_CFG_SINGLE_END_W(val);
      break;
      case 9:
         PPE_AXI_READ9_AXI_CFG_SINGLE_END_W(val);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;
   }
}

void PPE_MNGRG_setWriterBurstLen( UINT8 inst, UINT32 len )
{
   LOGG_PRINT(LOG_DEBUG_E, NULL, "Set burst len %d to writer %d\n", len, inst);

   switch(inst)
   {
      case 0:
         PPE_AXI_WRITE0_AXI_CFG_BURST_LEN_W(len);
      break;
      case 1:
         PPE_AXI_WRITE1_AXI_CFG_BURST_LEN_W(len);
      break;
      case 2:
         PPE_AXI_WRITE2_AXI_CFG_BURST_LEN_W(len);
      break;
      case 3:
         PPE_AXI_WRITE3_AXI_CFG_BURST_LEN_W(len);
      break;
      case 4:
         PPE_AXI_WRITE4_AXI_CFG_BURST_LEN_W(len);
      break;
      case 5:
         PPE_AXI_WRITE5_AXI_CFG_BURST_LEN_W(len);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;         
   }

}


static void fixAxiRdCfg(XMLDB_dbH dbh, UINT32 inst)
{
   ERRG_codeE ret = PPE_MNGR__RET_SUCCESS;
   //Overwrite axi configuration 
   UINT32 height = 0, width = 0, bpp = 0, beatLast = 0, singleEnd = 0 , xStart, yStart, stride, bufferHeght, numLinesPerChunk;

   //Get params from xmldb
   XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_WIDTH_E), &width);
   XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_HEIGHT_E), &height);
   XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_BPP_E), &bpp);
   XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_X_START_E), &xStart);
   XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_Y_START_E), &yStart);
   XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_AXIRD_E, inst, NU4100_PPE_AXI_READ0_AXI_BEAT_LAST_VAL_E), &beatLast);
   ret = XMLDB_getValue( dbh, NUFLD_calcPath( NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_STRIDE_E),            &stride);
   if(ERRG_FAILED(ret))    {stride = 0;}
   ret = XMLDB_getValue( dbh, NUFLD_calcPath( NUFLD_META_READERS_E, inst, META_READERS_RD_0_OUT_RES_BUFFERHEIGHT_E),      &bufferHeght);
   if(ERRG_FAILED(ret))    {bufferHeght = 0;}
   ret = XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_META_READERS_E, inst, META_READERS_RD_0_NUMLINESPERCHUNK_E), &numLinesPerChunk);
   if(ERRG_FAILED(ret))    {numLinesPerChunk = bufferHeght;}

   //XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_AXIRD_E, inst, NU4100_PPE_AXI_AXI_READ_0_AXI_CFG_SINGLE_END_E), &singleEnd);
   //Forcing single_end field - periphral is master of this DMA transaction, not DMA engine.   

   singleEnd = ((stride > width)) ? (0) : (1);  // stride implementation requires no singles at the end
   beatLast = ( (((width * numLinesPerChunk * bpp )) / 128) - 1); 

#ifdef __UART_ON_FPGA__
    writeReg(inst,singleEnd,beatLast);
#else
   //write to device
   XMLDB_writeFieldToReg(dbh, NUFLD_calcPath(NUFLD_AXIRD_E, inst, NU4100_PPE_AXI_READ0_AXI_CFG_SINGLE_END_E), singleEnd);
   XMLDB_writeFieldToReg(dbh,  NUFLD_calcPath(NUFLD_AXIRD_E, inst, NU4100_PPE_AXI_READ0_AXI_BEAT_LAST_VAL_E), beatLast);
#endif

   //LOGG_PRINT(LOG_INFO_E, NULL, "ppe_mngr: axi rd %d config width = %d, height = %d, bpp = %d, axiBeatLast  = %d \n", inst, width, height, bpp, beatLast );
   
}

ERRG_codeE PPE_MNGRG_setRegistersReady(void)
{
   PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_W(1);

   return PPE_MNGR__RET_SUCCESS;
}

ERRG_codeE PPE_MNGRG_updatePPUCrop(PPE_MNGRG_cropParamT *crop, UINT8 ppuMun)
{
   UINT16 x_s = crop->xStart;
   UINT16 y_s = crop->yStart;
   UINT16 w,h;

   switch(ppuMun)
   {
      case 0:
         w = PPE_PPU0_MISC_CROP_END_X_R - PPE_PPU0_MISC_CROP_START_X_R;
         h = PPE_PPU0_MISC_CROP_END_Y_R - PPE_PPU0_MISC_CROP_START_Y_R;
         PPE_PPU0_MISC_CROP_START_X_W(x_s);
         PPE_PPU0_MISC_CROP_START_Y_W(y_s);
         PPE_PPU0_MISC_CROP_END_X_W((x_s + w));
         PPE_PPU0_MISC_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 0: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU0_MISC_CROP_START_X_R, PPE_PPU0_MISC_CROP_START_Y_R, PPE_PPU0_MISC_CROP_END_X_R, PPE_PPU0_MISC_CROP_END_Y_R);
      break;
      case 1:
         w = PPE_PPU1_MISC_CROP_END_X_R - PPE_PPU1_MISC_CROP_START_X_R;
         h = PPE_PPU1_MISC_CROP_END_Y_R - PPE_PPU1_MISC_CROP_START_Y_R;
         PPE_PPU1_MISC_CROP_START_X_W(x_s);
         PPE_PPU1_MISC_CROP_START_Y_W(y_s);
         PPE_PPU1_MISC_CROP_END_X_W((x_s + w));
         PPE_PPU1_MISC_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 1: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU1_MISC_CROP_START_X_R, PPE_PPU1_MISC_CROP_START_Y_R, PPE_PPU1_MISC_CROP_END_X_R, PPE_PPU1_MISC_CROP_END_Y_R);
      break;
      case 2:
         w = PPE_PPU2_MISC_CROP_END_X_R - PPE_PPU2_MISC_CROP_START_X_R;
         h = PPE_PPU2_MISC_CROP_END_Y_R - PPE_PPU2_MISC_CROP_START_Y_R;
         PPE_PPU2_MISC_CROP_START_X_W(x_s);
         PPE_PPU2_MISC_CROP_START_Y_W(y_s);
         PPE_PPU2_MISC_CROP_END_X_W((x_s + w));
         PPE_PPU2_MISC_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 2: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU2_MISC_CROP_START_X_R, PPE_PPU2_MISC_CROP_START_Y_R, PPE_PPU2_MISC_CROP_END_X_R, PPE_PPU2_MISC_CROP_END_Y_R);
      break;
      case 3:
         w = PPE_PPU3_MISC_CROP_END_X_R - PPE_PPU3_MISC_CROP_START_X_R;
         h = PPE_PPU3_MISC_CROP_END_Y_R - PPE_PPU3_MISC_CROP_START_Y_R;
         PPE_PPU3_MISC_CROP_START_X_W(x_s);
         PPE_PPU3_MISC_CROP_START_Y_W(y_s);
         PPE_PPU3_MISC_CROP_END_X_W((x_s + w));
         PPE_PPU3_MISC_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 3: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU3_MISC_CROP_START_X_R, PPE_PPU3_MISC_CROP_START_Y_R, PPE_PPU3_MISC_CROP_END_X_R, PPE_PPU3_MISC_CROP_END_Y_R);
      break;
      case 4:
         w = PPE_PPU4_MISC_CROP_END_X_R - PPE_PPU4_MISC_CROP_START_X_R;
         h = PPE_PPU4_MISC_CROP_END_Y_R - PPE_PPU4_MISC_CROP_START_Y_R;
         PPE_PPU4_MISC_CROP_START_X_W(x_s);
         PPE_PPU4_MISC_CROP_START_Y_W(y_s);
         PPE_PPU4_MISC_CROP_END_X_W((x_s + w));
         PPE_PPU4_MISC_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 4: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU4_MISC_CROP_START_X_R, PPE_PPU4_MISC_CROP_START_Y_R, PPE_PPU4_MISC_CROP_END_X_R, PPE_PPU4_MISC_CROP_END_Y_R);
      break;
      case 5:
         w = PPE_PPU5_MISC_CROP_END_X_R - PPE_PPU5_MISC_CROP_START_X_R;
         h = PPE_PPU5_MISC_CROP_END_Y_R - PPE_PPU5_MISC_CROP_START_Y_R;
         PPE_PPU5_MISC_CROP_START_X_W(x_s);
         PPE_PPU5_MISC_CROP_START_Y_W(y_s);
         PPE_PPU5_MISC_CROP_END_X_W((x_s + w));
         PPE_PPU5_MISC_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 5: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU5_MISC_CROP_START_X_R, PPE_PPU5_MISC_CROP_START_Y_R, PPE_PPU5_MISC_CROP_END_X_R, PPE_PPU5_MISC_CROP_END_Y_R);
      break;
      case 7:
         w = PPE_PPU7_MISC0_CROP_END_X_R - PPE_PPU7_MISC0_CROP_START_X_R;
         h = PPE_PPU7_MISC0_CROP_END_Y_R - PPE_PPU7_MISC0_CROP_START_Y_R;
         PPE_PPU7_MISC0_CROP_START_X_W(x_s);
         PPE_PPU7_MISC0_CROP_START_Y_W(y_s);
         PPE_PPU7_MISC0_CROP_END_X_W((x_s + w));
         PPE_PPU7_MISC0_CROP_END_Y_W((y_s + h));
         PPE_PPU7_MISC1_CROP_START_X_W(x_s);
         PPE_PPU7_MISC1_CROP_START_Y_W(y_s);
         PPE_PPU7_MISC1_CROP_END_X_W((x_s + w));
         PPE_PPU7_MISC1_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 7: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU7_MISC0_CROP_START_X_R, PPE_PPU7_MISC0_CROP_START_Y_R, PPE_PPU7_MISC0_CROP_END_X_R, PPE_PPU7_MISC0_CROP_END_Y_R);
      break;
      case 8:
         w = PPE_PPU8_MISC0_CROP_END_X_R - PPE_PPU8_MISC0_CROP_START_X_R;
         h = PPE_PPU8_MISC0_CROP_END_Y_R - PPE_PPU8_MISC0_CROP_START_Y_R;
         PPE_PPU8_MISC0_CROP_START_X_W(x_s);
         PPE_PPU8_MISC0_CROP_START_Y_W(y_s);
         PPE_PPU8_MISC0_CROP_END_X_W((x_s + w));
         PPE_PPU8_MISC0_CROP_END_Y_W((y_s + h));
         PPE_PPU8_MISC1_CROP_START_X_W(x_s);
         PPE_PPU8_MISC1_CROP_START_Y_W(y_s);
         PPE_PPU8_MISC1_CROP_END_X_W((x_s + w));
         PPE_PPU8_MISC1_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 8: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU8_MISC0_CROP_START_X_R, PPE_PPU8_MISC0_CROP_START_Y_R, PPE_PPU8_MISC0_CROP_END_X_R, PPE_PPU8_MISC0_CROP_END_Y_R);
      break;
      case 9:
         w = PPE_PPU9_MISC0_CROP_END_X_R - PPE_PPU9_MISC0_CROP_START_X_R;
         h = PPE_PPU9_MISC0_CROP_END_Y_R - PPE_PPU9_MISC0_CROP_START_Y_R;
         PPE_PPU9_MISC0_CROP_START_X_W(x_s);
         PPE_PPU9_MISC0_CROP_START_Y_W(y_s);
         PPE_PPU9_MISC0_CROP_END_X_W((x_s + w));
         PPE_PPU9_MISC0_CROP_END_Y_W((y_s + h));
         PPE_PPU9_MISC1_CROP_START_X_W(x_s);
         PPE_PPU9_MISC1_CROP_START_Y_W(y_s);
         PPE_PPU9_MISC1_CROP_END_X_W((x_s + w));
         PPE_PPU9_MISC1_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 9: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU9_MISC0_CROP_START_X_R, PPE_PPU9_MISC0_CROP_START_Y_R, PPE_PPU9_MISC0_CROP_END_X_R, PPE_PPU9_MISC0_CROP_END_Y_R);
      break;
      case 10:
         w = PPE_PPU10_MISC0_CROP_END_X_R - PPE_PPU10_MISC0_CROP_START_X_R;
         h = PPE_PPU10_MISC0_CROP_END_Y_R - PPE_PPU10_MISC0_CROP_START_Y_R;
         PPE_PPU10_MISC0_CROP_START_X_W(x_s);
         PPE_PPU10_MISC0_CROP_START_Y_W(y_s);
         PPE_PPU10_MISC0_CROP_END_X_W((x_s + w));
         PPE_PPU10_MISC0_CROP_END_Y_W((y_s + h));
         PPE_PPU10_MISC1_CROP_START_X_W(x_s);
         PPE_PPU10_MISC1_CROP_START_Y_W(y_s);
         PPE_PPU10_MISC1_CROP_END_X_W((x_s + w));
         PPE_PPU10_MISC1_CROP_END_Y_W((y_s + h));
//       LOGG_PRINT(LOG_DEBUG_E, NULL, "ppe_mngr 10: xs %d ys %d xe %d ye %d \n", 
//          PPE_PPU10_MISC0_CROP_START_X_R, PPE_PPU10_MISC0_CROP_START_Y_R, PPE_PPU10_MISC0_CROP_END_X_R, PPE_PPU10_MISC0_CROP_END_Y_R);
      break;
      default:
      break;   
   }


   //LOGG_PRINT(LOG_INFO_E, NULL, "ppe_mngr: xs %d ys %d xe %d ye %d \n", PPE_PPU2_MISC_CROP_START_X_R, PPE_PPU2_MISC_CROP_START_Y_R, PPE_PPU2_MISC_CROP_END_X_R, PPE_PPU2_MISC_CROP_END_Y_R);


   return PPE_MNGR__RET_SUCCESS;
}

ERRG_codeE PPE_MNGRG_setConfigAxiRd( XMLDB_dbH dbh, NUFLD_blkE block, UINT32 inst, void *arg)
{
   (void)arg;(void)block;
   ERRG_codeE ret = PPE_MNGR__RET_SUCCESS;
   //UINT32 singleEnd;

   fixAxiRdCfg(dbh, inst);

   return ret;
}

ERRG_codeE PPE_MNGRG_setEnableAxiRd( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   UINT32 intStrmEn;
   (void)arg;(void)block;(void)instanceList;(void)dbh;
   int i = 0;
   /*
   while(instanceList[i] != 0xFF)
   {   
       XMLDB_getValue( dbh, NUFLD_calcPath(NUFLD_AXIRD_E, instanceList[i], NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E), &intStrmEn);
       //printf("PPE_MNGRG_setEnableAxiRd: instanceList[%d] = %d, intStrmEn = 0x%x\n",i,instanceList[i],intStrmEn);      
       XMLDB_writeFieldToReg(dbh, NUFLD_calcPath(NUFLD_AXIRD_E, instanceList[i], NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E), intStrmEn);
       i++;
   }
   */
   //set ready
   PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_W(1);

   //simple implementation for now, use cde isr as down vote
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_PPE);

   return PPE_MNGR__RET_SUCCESS;
}

ERRG_codeE PPE_MNGRG_setDisableAxiRd( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   int i = 0;
   (void)arg;(void)block;(void)instanceList;(void)dbh;
   while(instanceList[i] != 0xFF)
   {
       //printf("PPE_MNGRG_setDisableAxiRd: instanceList[%d] = %d\n",i,instanceList[i]);
       XMLDB_writeFieldToReg(dbh, NUFLD_calcPath(NUFLD_AXIRD_E, instanceList[i], NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E), 0);
       i++;
   }
   return PPE_MNGR__RET_SUCCESS;
}


ERRG_codeE PPE_MNGRG_setEnableAxiWr( XMLDB_dbH dbh, NUFLD_blkE block, UINT8 instance, void *arg, void *voterHandle )
{
   (void)arg;(void)block;(void)dbh;
   //set ready
   PPE_PPE_MISC_CTRL_REGISTERS_READY_DONE_W(1);

   if (instance==0)
   {
      PPE_AXI_WRITE0_CTRL_EN_W(1);
   }
   else if (instance==1)
   {
      PPE_AXI_WRITE1_CTRL_EN_W(1);
   }
   else if (instance==2)
   {
      PPE_AXI_WRITE2_CTRL_EN_W(1);
   }
   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_PPE);

   return PPE_MNGR__RET_SUCCESS;
}

ERRG_codeE PPE_MNGRG_setDisableAxiWr( XMLDB_dbH dbh, NUFLD_blkE block, UINT8 instance, void *arg )
{
   (void)arg;(void)block;(void)dbh;

   if (instance==0)
   {
      PPE_AXI_WRITE0_CTRL_EN_W(0);
   }
   else if (instance==1)
   {
      PPE_AXI_WRITE1_CTRL_EN_W(0);
   }
   else if (instance==2)
   {
      PPE_AXI_WRITE2_CTRL_EN_W(0);
   }
//   PPE_AXI_WRITE3_CTRL_EN_W(instanceList[3]?0:1);
//   PPE_AXI_WRITE4_CTRL_EN_W(instanceList[4]?0:1);
//   PPE_AXI_WRITE5_CTRL_EN_W(instanceList[5]?0:1);
   return PPE_MNGR__RET_SUCCESS;
}

void PPE_MNGRG_getReaderIntStrmEn(UINT8 inst, UINT32 *intStrmEnVal)
{
   switch (inst)
   {
      case 0:
         *intStrmEnVal = PPE_AXI_READ0_CTRL_INT_STRM_EN_R;
      break;
      case 1:
         *intStrmEnVal = PPE_AXI_READ1_CTRL_INT_STRM_EN_R;
      break;
      case 2:
         *intStrmEnVal = PPE_AXI_READ2_CTRL_INT_STRM_EN_R;
      break;
      case 3:
         *intStrmEnVal = PPE_AXI_READ3_CTRL_INT_STRM_EN_R;
      break;
      case 4:
         *intStrmEnVal = PPE_AXI_READ4_CTRL_INT_STRM_EN_R;
      break;
      case 5:
         *intStrmEnVal = PPE_AXI_READ5_CTRL_INT_STRM_EN_R;
      break;
      case 8:
         *intStrmEnVal = PPE_AXI_READ8_CTRL_INT_STRM_EN_R;
      break;
      case 9:
         *intStrmEnVal = PPE_AXI_READ9_CTRL_INT_STRM_EN_R;
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;
   }
}

void PPE_MNGRG_setReaderIntStrmEn(UINT8 inst, UINT32 val)
{
   switch (inst)
   {
      case 0:
         PPE_AXI_READ0_CTRL_INT_STRM_EN_W(val);
      break;
      case 1:
         PPE_AXI_READ1_CTRL_INT_STRM_EN_W(val);
      break;
      case 2:
         PPE_AXI_READ2_CTRL_INT_STRM_EN_W(val);
      break;
      case 3:
         PPE_AXI_READ3_CTRL_INT_STRM_EN_W(val);
      break;
      case 4:
         PPE_AXI_READ4_CTRL_INT_STRM_EN_W(val);
      break;
      case 5:
         PPE_AXI_READ5_CTRL_INT_STRM_EN_W(val);
      break;
      case 8:
         PPE_AXI_READ8_CTRL_INT_STRM_EN_W(val);
      break;
      case 9:
         PPE_AXI_READ9_CTRL_INT_STRM_EN_W(val);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;
   }
}

void PPE_MNGRG_getReaderBeatLast(UINT8 inst, UINT32 *beatLast)
{
   switch(inst)
   {
      case 0:
         *beatLast = PPE_AXI_READ0_AXI_BEAT_LAST_VAL_R;
      break;
      case 1:
         *beatLast = PPE_AXI_READ1_AXI_BEAT_LAST_VAL_R;
      break;
      case 2:
         *beatLast = PPE_AXI_READ2_AXI_BEAT_LAST_VAL_R;
      break;
      case 3:
         *beatLast = PPE_AXI_READ3_AXI_BEAT_LAST_VAL_R;
      break;
      case 4:
         *beatLast = PPE_AXI_READ4_AXI_BEAT_LAST_VAL_R;
      break;
      case 5:
         *beatLast = PPE_AXI_READ5_AXI_BEAT_LAST_VAL_R;
      break;
      case 8:
         *beatLast = PPE_AXI_READ8_AXI_BEAT_LAST_VAL_R;
      break;
      case 9:
         *beatLast = PPE_AXI_READ9_AXI_BEAT_LAST_VAL_R;
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;
   }
}

void PPE_MNGRG_setReaderBeatLast(UINT8 inst, UINT32 beatLast)
{
   switch(inst)
   {
      case 0:
         PPE_AXI_READ0_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 1:
         PPE_AXI_READ1_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 2:
         PPE_AXI_READ2_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 3:
         PPE_AXI_READ3_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 4:
         PPE_AXI_READ4_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 5:
         PPE_AXI_READ5_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 8:
         PPE_AXI_READ8_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      case 9:
         PPE_AXI_READ9_AXI_BEAT_LAST_VAL_W(beatLast);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL, "Got wrong instance - %d\n", inst);
      break;
   }

}

void PPE_MNGRG_dumpRegs()
{
   UINT32 regsOffset[] = {ppe_offset_tbl_values};
   UINT32 regsResetVal[] = {ppe_regs_reset_val};
   UINT32 reg;
   UINT8  buf[1024];

/*
   //to dump all the registers
   LOGG_PRINT(LOG_INFO_E, NULL, "PPE registers (number of regs = %d):\n",sizeof(regsOffset));
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg+=4)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x\n"
                                    , regsOffset[reg],   *(volatile UINT32 *)(PPE_BASE + regsOffset[reg])
                                    , regsOffset[reg+1], *(volatile UINT32 *)(PPE_BASE + regsOffset[reg+1])
                                    , regsOffset[reg+2], *(volatile UINT32 *)(PPE_BASE + regsOffset[reg+2])
                                    , regsOffset[reg+3], *(volatile UINT32 *)(PPE_BASE + regsOffset[reg+3]));

   }
*/
   memset(buf,0,sizeof(buf));
   sprintf(buf + strlen(buf), "Modified PPE registers:\n");
//   LOGG_PRINT(LOG_INFO_E, NULL, "Modified PPE registers:\n");
   //compare against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(ppeBaseAddress + regsOffset[reg]) != regsResetVal[reg])
      {
         if (strlen(buf) > 900)
         {
            printf("%s",buf);
            memset(buf,0,sizeof(buf));
         }
         //LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(PPE_BASE + regsOffset[reg]));
         sprintf(buf + strlen(buf), "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(PPE_BASE + regsOffset[reg]));
      }
   }
   printf("%s",buf);
}


#ifdef __cplusplus
   }
#endif

