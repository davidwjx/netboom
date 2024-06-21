/****************************************************************************
 *
 *   FileName: iae_drv.c
 *
 *   Author:  Danny B.
 *
 *   Date:
 *
 *   Description: Inuitive IAE Driver
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#ifdef __cplusplus
      extern "C" {
#endif

#include "nu4k_defs.h"
#include "iae_drv.h"
#include "nu4100_iae_regs.h"
#include "xml_dev.h"
#include "os_lyr.h"
#include "mem_map.h"
#include "iae_mngr.h"
#include "hcg_mngr.h"
#include "gme_drv.h"
#include "helsinki.h"
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
   IAE_DRVP_MODULE_STATUS_DUMMY_E = 0,
   IAE_DRVP_MODULE_STATUS_CLOSE_E = 1,
   IAE_DRVP_MODULE_STATUS_OPEN_E  = 2,
   IAE_DRVP_MODULE_STATUS_LAST_E
} IAE_DRVP_moduleStatusE;

#define IAE_DRVP_MAX_NUM_SNSR_CB (12)

typedef struct
{
   IAE_DRVG_snsrInterruptCbT isrCb;
   void                     *argP;
   UINT8                    used;
}IAE_DRVP_snsrIsrCbT;

typedef struct
{
   IAE_DRVP_moduleStatusE   moduleStatus;
   IAE_DRVG_interruptCbT    isrCb[IAE_DRVG_ALL_INTERRUPTS_E];
   IAE_DRVG_phyInterruptCbT isrPhyCb[IAE_DRVG_ALLPHY5_INT_E];
   IAE_DRVP_snsrIsrCbT      snsrIsrCb[IAE_DRVP_MAX_NUM_SNSR_CB];
   IAE_DRVP_snsrIsrCbT     eofIsrCb[IAE_DRVP_MAX_NUM_SNSR_CB];
   UINT32                   deviceBaseAddress;
} IAE_DRVP_deviceDescT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static IAE_DRVP_deviceDescT   IAE_DRVP_deviceDesc;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
// local functions
static void IAE_DRVP_dataBaseReset(void);



//static ERRG_codeE IAE_DRVP_initIaeDevice();
//static ERRG_codeE IAE_DRVP_open(IAE_DRVG_openParamsT *params);
//static ERRG_codeE IAE_DRVP_close();

void IAE_DRVG_FSG_sensDisable()
{
   IAE_ENABLE_SNS_EN_W(0);
}
void IAE_DRVG_FSG_sensEnable()
{
   IAE_ENABLE_SNS_EN_W(1);
}
void IAE_DRVG_FSG_swTrigger0()
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_0_W(1);
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
}
void IAE_DRVG_FSG_swTrigger1()
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_1_W(1);
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
}
void IAE_DRVG_FSG_swTrigger2()
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_2_W(1);
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
}
void IAE_DRVG_FSG_swTrigger(IAE_DRVG_fsgCounterSrc fsgTrigger)
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   LOGG_PRINT(LOG_INFO_E,NULL,"Writing SW Trigger for counter:%lu\n", fsgTrigger);
   switch(fsgTrigger)
   {
      case IAE_DRVG_FSG_CNTSRC_0_E:
      /*0 = FSG FTRIG0 functionality and GIO 38  (FTRIG0) is CTRL5.CTRL3*/
      IAE_DRVG_FSG_swTrigger0();
      break;
      case IAE_DRVG_FSG_CNTSRC_1_E:
      IAE_DRVG_FSG_swTrigger1();
      break;
      case IAE_DRVG_FSG_CNTSRC_2_E:
      IAE_DRVG_FSG_swTrigger2();
      break;
   }
   /*Unsure if IAE_DRVG_FSG_swTrigger0,1,2 should be called, I don't think it should when disabling*/
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
}
void  IAE_DRVG_disableFTRIGOutput(IAE_DRVG_fsgCounterNumE fsgTrigger)
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   LOGG_PRINT(LOG_INFO_E,NULL,"Disabling FTRIG:%lu\n", fsgTrigger);
   switch(fsgTrigger)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      /*0 = FSG FTRIG0 functionality and GIO 38  (FTRIG0) is CTRL5.CTRL3*/
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_CTRL_EN_W(0);
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_CTRL_EN_W(0);
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_CTRL_EN_W(0);
      break;
      case IAE_DRVG_FSG_CNT_3_E:
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_CTRL_EN_W(0);
      break;
      case IAE_DRVG_FSG_CNT_4_E:
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_CTRL_EN_W(0);
      break;
      case IAE_DRVG_FSG_CNT_5_E:
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_CTRL_EN_W(0);
      break;
   }
   /*Unsure if IAE_DRVG_FSG_swTrigger0,1,2 should be called, I don't think it should when disabling*/
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
 
}
void  IAE_DRVG_enableFTRIGOutput(IAE_DRVG_fsgCounterNumE fsgTrigger)
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   LOGG_PRINT(LOG_INFO_E,NULL,"Enabling FTRIG:%lu\n", fsgTrigger);
   switch(fsgTrigger)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      /*0 = FSG FTRIG0 functionality and GIO 38  (FTRIG0) is CTRL5.CTRL3*/
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_CTRL_EN_W(1);
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_CTRL_EN_W(1);
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_CTRL_EN_W(1);
      break;
      case IAE_DRVG_FSG_CNT_3_E:
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_CTRL_EN_W(1);
      break;
      case IAE_DRVG_FSG_CNT_4_E:
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_CTRL_EN_W(1);
      break;
      case IAE_DRVG_FSG_CNT_5_E:
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_CTRL_EN_W(1);
      break;
   }
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   IAE_DRVG_FSG_swTrigger0();
   IAE_DRVG_FSG_swTrigger1();
   IAE_DRVG_FSG_swTrigger2();
}

/****************************************************************************
 ***********************      G L O B A L     F U N C T I O N S     *********************
 ****************************************************************************/
void IAE_DRVG_enablePwmDim(unsigned int pwmNum,unsigned int fps,unsigned int widthInUsec)
{
  unsigned int width,delay,currFreq;
  GME_DRVG_getCiifClk(&currFreq);

  delay = (currFreq / 2)/fps;
  width = currFreq*widthInUsec;
  LOGG_PRINT(LOG_INFO_E, NULL,"enable pwmNum %d fps %d delay %d width usec %d width %d\n",
        pwmNum,fps,delay,widthInUsec,width);
  switch (pwmNum)
  {
  case IAE_DRVG_PWM_0_E:
     IAE_SNSR_CTRL_PWM_CNT0_CFG_FS_SRC_W(12);
     IAE_SNSR_CTRL_PWM_CNT0_CFG_FRAME_SYNC_DELAY_W(delay);
     IAE_SNSR_CTRL_PWM_CNT0_CFG_EN_W(1);


     IAE_SNSR_CTRL_PWM_DIM0_CTRL_POLARITY_W(0);
     IAE_SNSR_CTRL_PWM_DIM0_CTRL_DIM_MODE_W(0);
     IAE_SNSR_CTRL_PWM_DIM0_CTRL_SYNC_SRC_W(0);
     IAE_SNSR_CTRL_PWM_DIM0_CTRL_DIM_CHANGE_W(1);
     IAE_SNSR_CTRL_PWM_DIM0_FREQ_VAL_W(0);
     IAE_SNSR_CTRL_PWM_DIM0_WIDTH_VAL_W(width);


     IAE_SNSR_CTRL_PWM_DIM0_CTRL_EN_W(1);
     IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_0_W(1);
    break;
  case IAE_DRVG_PWM_1_E:
     IAE_SNSR_CTRL_PWM_CNT1_CFG_FS_SRC_W(13);
     IAE_SNSR_CTRL_PWM_CNT1_CFG_FRAME_SYNC_DELAY_W(delay);
     IAE_SNSR_CTRL_PWM_CNT1_CFG_EN_W(1);


     IAE_SNSR_CTRL_PWM_DIM1_CTRL_POLARITY_W(0);
     IAE_SNSR_CTRL_PWM_DIM1_CTRL_DIM_MODE_W(0);
     IAE_SNSR_CTRL_PWM_DIM1_CTRL_SYNC_SRC_W(1);
     IAE_SNSR_CTRL_PWM_DIM1_CTRL_DIM_CHANGE_W(1);
     IAE_SNSR_CTRL_PWM_DIM1_FREQ_VAL_W(0);
     IAE_SNSR_CTRL_PWM_DIM1_WIDTH_VAL_W(widthInUsec);


     IAE_SNSR_CTRL_PWM_DIM1_CTRL_EN_W(1);
     IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_1_W(1);
     break;
  case IAE_DRVG_PWM_2_E:
     IAE_SNSR_CTRL_PWM_CNT2_CFG_FS_SRC_W(14);
     IAE_SNSR_CTRL_PWM_CNT2_CFG_FRAME_SYNC_DELAY_W(delay);
     IAE_SNSR_CTRL_PWM_CNT2_CFG_EN_W(1);


     IAE_SNSR_CTRL_PWM_DIM2_CTRL_POLARITY_W(0);
     IAE_SNSR_CTRL_PWM_DIM2_CTRL_DIM_MODE_W(0);
     IAE_SNSR_CTRL_PWM_DIM2_CTRL_SYNC_SRC_W(2);
     IAE_SNSR_CTRL_PWM_DIM2_CTRL_DIM_CHANGE_W(1);
     IAE_SNSR_CTRL_PWM_DIM2_FREQ_VAL_W(0);
     IAE_SNSR_CTRL_PWM_DIM2_WIDTH_VAL_W(widthInUsec);


     IAE_SNSR_CTRL_PWM_DIM2_CTRL_EN_W(1);
     IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_2_W(1);
     break;
  default:
     break;
  }

}

void IAE_DRVG_disablePwmDim(unsigned int pwmNum)
{
  switch (pwmNum)
  {
  case IAE_DRVG_PWM_0_E:
     IAE_SNSR_CTRL_PWM_CNT0_CFG_EN_W(0);
     IAE_SNSR_CTRL_PWM_DIM0_CTRL_EN_W(0);
     IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_0_W(0);
    break;
  case IAE_DRVG_PWM_1_E:
     IAE_SNSR_CTRL_PWM_CNT1_CFG_EN_W(0);
     IAE_SNSR_CTRL_PWM_DIM1_CTRL_EN_W(0);
     IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_1_W(0);
     break;
  case IAE_DRVG_PWM_2_E:
     IAE_SNSR_CTRL_PWM_CNT2_CFG_EN_W(0);
     IAE_SNSR_CTRL_PWM_DIM2_CTRL_EN_W(0);
     IAE_SNSR_CTRL_CTRL_FSG_SW_TRIGGER_2_W(0);
     break;
  default:
     break;
  }

}

ERRG_codeE IAE_DRVG_setBaseDir0(int inst, int val)
{
   ERRG_codeE ret = MIPI__RET_SUCCESS;
   
   switch(inst)
   {
      case 0:
        IAE_RX_DPHY_CTRL_DPHY0_BASEDIR_0_W(val);
        break;
      case 1:
        IAE_RX_DPHY_CTRL_DPHY1_BASEDIR_0_W(val);
        break;
      case 2:
        IAE_RX_DPHY_CTRL_DPHY2_BASEDIR_0_W(val);
        break;
      case 3:
        IAE_RX_DPHY_CTRL_DPHY3_BASEDIR_0_W(val);
        break;
      case 4:
        IAE_RX_DPHY_CTRL_DPHY4_BASEDIR_0_W(val);
        break;
      case 5:
        IAE_RX_DPHY_CTRL_DPHY5_BASEDIR_0_W(val);
        break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL,"invalid instance=%d\n",inst);
         ret = IAE__ERR_INVALID_ARGS;
      break;
   }
   return ret;
}

ERRG_codeE IAE_DRVG_setForceXMode(int inst, int val)
{
   ERRG_codeE ret = MIPI__RET_SUCCESS;

   switch(inst)
   {
      case 0:
        IAE_RX_DPHY_CTRL_DPHY0_FORCERXMODE_N_W(val);
        break;
      case 1:
        IAE_RX_DPHY_CTRL_DPHY1_FORCERXMODE_N_W(val);
        break;
      case 2:
        IAE_RX_DPHY_CTRL_DPHY2_FORCERXMODE_N_W(val);
        break;
      case 3:
        IAE_RX_DPHY_CTRL_DPHY3_FORCERXMODE_N_W(val);
        break;
      case 4:
        IAE_RX_DPHY_CTRL_DPHY4_FORCERXMODE_N_W(val);
        break;
      case 5:
        IAE_RX_DPHY_CTRL_DPHY5_FORCERXMODE_N_W(val);
        break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL,"invalid instance=%d\n",inst);
         ret = IAE__ERR_INVALID_ARGS;
      break;
   }
   return ret;
}

void IAE_DRVG_configDefaultPhySelect()
{
    IAE_MIPI_MUX_PHY0_SEL_W(1);
    IAE_MIPI_MUX_PHY1_SEL_W(1);
    IAE_MIPI_MUX_PHY2_SEL_W(1);
    IAE_MIPI_MUX_PHY3_SEL_W(2);
    IAE_MIPI_MUX_PHY4_SEL_W(3);
    IAE_MIPI_MUX_PHY5_SEL_W(4);
}

void IAE_DRVG_configPhySelect(int inst, int val)
{
    switch(inst)
   {
      case 0:
        IAE_MIPI_MUX_PHY0_SEL_W(val);
        break;
      case 1:
        IAE_MIPI_MUX_PHY1_SEL_W(val);
        break;
      case 2:
        IAE_MIPI_MUX_PHY2_SEL_W(val);
        break;
      case 3:
        IAE_MIPI_MUX_PHY3_SEL_W(val);
        break;
      case 4:
        IAE_MIPI_MUX_PHY4_SEL_W(val);
        break;
      case 5:
        IAE_MIPI_MUX_PHY5_SEL_W(val);
        break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL,"invalid instance=%d\n",inst);
         return;
      break;
   }
   return;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_bypassCfg
*
*  Description: Bypass IAU Blocks. Not in bypass in default.
*
*  Inputs: An IAU block to bypass
*
*  Outputs: ERRG_codeE
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
#if 1
ERRG_codeE IAE_DRVG_bypassCfg(UINT8 iaeNum, UINT32 bypass)
{
   ERRG_codeE                 ret = IAE__RET_SUCCESS;
   UINT32                     tmp;

   //read bypass register value
   tmp = IAE_BYPASS_VAL;
   //printf("IAE_DRVG_bypassCfg current bypass val 0x%x\n");

   //modified the relevant iau bypass values
   switch( iaeNum )
   {
      case 0:
      {
         tmp            &= 0xFFFFFF00;
         tmp |= ( bypass & 0xFF );
         break;
      }
      case 1:
      {
         tmp            &= 0xFFFF00FF;
         tmp |= ( bypass & 0xFF00 );
         break;
      }
      case 2:
      {
         tmp            &= 0xFF00FFFF;
         tmp |= ( bypass & 0xFF0000 );
         break;
      }
      case 3:
      {
         tmp            &= 0x00FFFFFF;
         tmp |= ( bypass & 0xFF000000 );
         break;
      }
      //default:
      //{

      //}
      
   }

   //write the updated values to register
   IAE_BYPASS_VAL = tmp;
   
   return ret;
}
#else
ERRG_codeE IAE_DRVG_bypassCfg(const IAE_DRVG_bypassModuleE  *bypassBlock)
{
   ERRG_codeE                 ret = IAE__RET_SUCCESS;
   switch(*bypassBlock)
   {
      case IAE_DRVG_IAU_BYPASS_ALL_BLOCKS_E:
      {
         IAE_BYPASS_VAL=0xFFFFFFFF;
         break;
      }
      case IAE_DRVG_IAU_BPC0_BYPASS_E:
      {
         IAE_BYPASS_BPC0_W(1);
         break;
      }
      case IAE_DRVG_IAU_YUV0_BYPASS_E:
      {
         IAE_BYPASS_YUV0_W(1);
         break;
      }
      case IAE_DRVG_IAU_IB0_BYPASS_E:
      {
         IAE_BYPASS_IB0_W(1);
         break;
      }
      case IAE_DRVG_IAU_WB0_BYPASS_E:
      {
         IAE_BYPASS_WB0_W(1);
         break;
      }
      case IAE_DRVG_IAU_DMS0_BYPASS_E:
      {
         IAE_BYPASS_DMS0_W(1);
         break;
      }
      case IAE_DRVG_IAU_GCR0_BYPASS_E:
      {
         IAE_BYPASS_GCR0_W(1);
         break;
      }
      case IAE_DRVG_IAU_MED0_BYPASS_E:
      {
         IAE_BYPASS_MED0_W(1);
         break;
      }
      case IAE_DRVG_IAU_CSC0_BYPASS_E:
      {
         IAE_BYPASS_CSC0_W(1);
         break;
      }
      case IAE_DRVG_IAU_DSR0_BYPASS_E:
      {
         IAE_BYPASS_DSR0_W(1);
         break;
      }
      case IAE_DRVG_IAU_BPC1_BYPASS_E:
      {
         IAE_BYPASS_BPC1_W(1);
         break;
      }
      case IAE_DRVG_IAU_YUV1_BYPASS_E:
      {
         IAE_BYPASS_YUV1_W(1);
         break;
      }
      case IAE_DRVG_IAU_IB1_BYPASS_E:
      {
         IAE_BYPASS_IB1_W(1);
         break;
      }
      case IAE_DRVG_IAU_WB1_BYPASS_E:
      {
         IAE_BYPASS_WB1_W(1);
         break;
      }
      case IAE_DRVG_IAU_DMS1_BYPASS_E:
      {
         IAE_BYPASS_DMS1_W(1);
         break;
      }
      case IAE_DRVG_IAU_GCR1_BYPASS_E:
      {
         IAE_BYPASS_GCR1_W(1);
         break;
      }
      case IAE_DRVG_IAU_MED1_BYPASS_E:
      {
         IAE_BYPASS_MED1_W(1);
         break;
      }
      case IAE_DRVG_IAU_CSC1_BYPASS_E:
      {
         IAE_BYPASS_CSC1_W(1);
         break;
      }
      case IAE_DRVG_IAU_DSR1_BYPASS_E:
      {
         IAE_BYPASS_DSR1_W(1);
         break;
      }
      case IAE_DRVG_IAU_YUV2_BYPASS_E:
      {
         IAE_BYPASS_YUV2_W(1);
         break;
      }
      case IAE_DRVG_IAU_CSC2_BYPASS_E:
      {
         IAE_BYPASS_CSC2_W(1);
         break;
      }
      case IAE_DRVG_IAU_DSR2_BYPASS_E:
      {
         IAE_BYPASS_DSR2_W(1);
         break;
      }
      case IAE_DRVG_IAU_YUV3_BYPASS_E:
      {
         IAE_BYPASS_YUV3_W(1);
         break;
      }
      case IAE_DRVG_IAU_CSC3_BYPASS_E:
      {
         IAE_BYPASS_CSC3_W(1);
         break;
      }
      case IAE_DRVG_IAU_DSR3_BYPASS_E:
      {
         IAE_BYPASS_DSR3_W(1);
         break;
      }
      default:
      {
         ret = IAE__ERR_BYPASS_FAIL_WRONG_MODULE;
         break;
      }
   }
   return ret;
}

#endif
ERRG_codeE IAE_DRVG_setFSGCntPeriod(const IAE_DRVG_fsgCounterNumE counter,const UINT32 period)
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);

   ERRG_codeE ret = IAE__RET_SUCCESS; 
   switch(counter)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      IAE_SNSR_CTRL_FSG_CNTR0_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      IAE_SNSR_CTRL_FSG_CNTR1_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      IAE_SNSR_CTRL_FSG_CNTR2_PERIOD_PERIOD_W(period);
      break;
      default:
      ret =  IAE__ERR_INVALID_ARGS;
   }

   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return ret;
}
ERRG_codeE IAE_DRVG_setFSGCntMode(const IAE_DRVG_fsgCounterSrc counter,const IAE_DRVG_fsgCntMode cntMode )
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);   /*Vote that the IAE is enabled*/
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Writing cnt mode for counter %lu to be %lu \n", counter,cntMode);
   const UINT8 cntMode_u = (UINT8)cntMode;


   switch(counter)
   {
      case IAE_DRVG_FSG_CNTSRC_0_E:
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG0_CNT_MODE_W(cntMode);
      break;
      case IAE_DRVG_FSG_CNTSRC_1_E:
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG1_CNT_MODE_W(cntMode);
      break;
      case IAE_DRVG_FSG_CNTSRC_2_E:
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG2_CNT_MODE_W(cntMode);
      break;
   }
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return IAE__RET_SUCCESS;
}

ERRG_codeE IAE_DRVG_setFSGTrigSrc(const IAE_DRVG_fsgCounterSrc counter,const UINT8 trigSrc )
{
   if(trigSrc>31)
   {
      return IAE__ERR_INVALID_ARGS;
   }
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);   /*Vote that the IAE is enabled*/
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Trigger source for counter:%lu is being set to %lu \n",counter,trigSrc);
   switch(counter)
   {
      case IAE_DRVG_FSG_CNTSRC_0_E:
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG0_TRIG_SRC_W(trigSrc);
      break;
      case IAE_DRVG_FSG_CNTSRC_1_E:
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG1_TRIG_SRC_W(trigSrc);
      break;
      case IAE_DRVG_FSG_CNTSRC_2_E:
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG2_TRIG_SRC_W(trigSrc);
      break;
   }
   IAE_DRVG_FSG_sensEnable();
    HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
    return IAE__RET_SUCCESS;
}

ERRG_codeE IAE_DRVG_FSGCntRegisterCfg(const IAE_DRVG_snsrCtrlFsgCntrCtrl *config)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;
   if(config==NULL)
   {
      return IAE__ERR_INVALID_ARGS; 
   }
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);   /*Vote that the IAE is enabled*/
   GME_DRVG_iaeCoreReset();
   //XMLDB_restoreIAERegisters();
   if(config->fsg2_trig_src <=31)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG2_TRIG_SRC_W(config->fsg2_trig_src);
   }
   else
   {
      ret =  IAE__ERR_INVALID_ARGS;
   }
   if(config->fsg1_trig_src <=31)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG1_TRIG_SRC_W(config->fsg1_trig_src);
   }
   else
   {
      ret =  IAE__ERR_INVALID_ARGS;
   }
   if(config->fsg0_trig_src <=31)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG0_TRIG_SRC_W(config->fsg0_trig_src);
   }
   else
   {
      ret =  IAE__ERR_INVALID_ARGS;
   }
   if(config->ts_cnt_range <=15)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_TS_CNT_RANGE_W(config->ts_cnt_range);
   }
   else
   {
      ret = IAE__ERR_INVALID_ARGS;
   }
   IAE_SNSR_CTRL_FSG_CNTR_CTRL_TS_CNT_EN_W(config->ts_cnt_en==true);
   if(config->fsg0_cnt_mode <=3)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG0_CNT_MODE_W(config->fsg0_cnt_mode);
   }
   else
   {
      ret = IAE__ERR_INVALID_ARGS;
   }
   if(config->fsg1_cnt_mode <= 3)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG1_CNT_MODE_W(config->fsg1_cnt_mode);
   }
   else
   {
      ret =  IAE__ERR_INVALID_ARGS;
   }
   if(config->fsg2_cnt_mode <= 3)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG2_CNT_MODE_W(config->fsg2_cnt_mode);
   }
   else
   {
      ret =  IAE__ERR_INVALID_ARGS;
   }
   IAE_SNSR_CTRL_FSG_CNTR_CTRL_TS_SRC_W(config->ts_src==true);
   if(config->fsg_cnt_en <= 2)
   {
      IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG_CNT_EN_W(config->fsg_cnt_en);
   }
   else
   {
      ret =  IAE__ERR_INVALID_ARGS;
   }

 
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return ret;
}
ERRG_codeE IAE_DRVG_FSGPulseCtrlCfg(const IAE_DRVG_fsgCounterNumE counter, const IAE_DRVG_snsrCtrlFSGFtrigPulseCtrl *config)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;
   /*Sanity check all the arguments before doing any writes*/
   if(config==NULL)
   {
      return IAE__ERR_INVALID_ARGS; 
   }
   if(config->counterSrc > 2)
   {
      return IAE__ERR_INVALID_ARGS;
   }   
   if(config->pulsePolarity> 1)
   {
      return IAE__ERR_INVALID_ARGS;
   }
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);   /*Vote that the IAE is enabled*/
   switch(counter)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_CTRL_WIDTH_W(config->width);
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_CTRL_FSG_CNT_SRC_W(config->counterSrc);
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_CTRL_POL_W(config->pulsePolarity);
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_CTRL_EN_W(config->pulseEnable);
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_CTRL_WIDTH_W(config->width);
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_CTRL_FSG_CNT_SRC_W(config->counterSrc);
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_CTRL_POL_W(config->pulsePolarity);
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_CTRL_EN_W(config->pulseEnable);
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_CTRL_WIDTH_W(config->width);
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_CTRL_FSG_CNT_SRC_W(config->counterSrc);
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_CTRL_POL_W(config->pulsePolarity);
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_CTRL_EN_W(config->pulseEnable);
      break;
      case IAE_DRVG_FSG_CNT_3_E:
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_CTRL_WIDTH_W(config->width);
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_CTRL_FSG_CNT_SRC_W(config->counterSrc);
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_CTRL_POL_W(config->pulsePolarity);
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_CTRL_EN_W(config->pulseEnable);
      break;
      case IAE_DRVG_FSG_CNT_4_E:
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_CTRL_WIDTH_W(config->width);
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_CTRL_FSG_CNT_SRC_W(config->counterSrc);
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_CTRL_POL_W(config->pulsePolarity);
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_CTRL_EN_W(config->pulseEnable);
      break;
      case IAE_DRVG_FSG_CNT_5_E:
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_CTRL_WIDTH_W(config->width);
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_CTRL_FSG_CNT_SRC_W(config->counterSrc);
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_CTRL_POL_W(config->pulsePolarity);
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_CTRL_EN_W(config->pulseEnable);
      break;
      default:
      ret = IAE__ERR_INVALID_ARGS;
      break;
   }

   return ret;
}
ERRG_codeE IAE_DRVG_setPulseOffset(const IAE_DRVG_fsgCounterNumE counter, const UINT32 offset)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;
   /*Sanity check all the arguments before doing any writes*/
   if(counter > IAE_DRVG_FSG_CNT_5_E)
   {
      return IAE__ERR_INVALID_ARGS; 
   }
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);   /*Vote that the IAE is enabled*/
   switch(counter)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_OFFSET_OFFSET_W(offset);
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_OFFSET_OFFSET_W(offset);
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_OFFSET_OFFSET_W(offset);
      break;
      case IAE_DRVG_FSG_CNT_3_E:
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_OFFSET_OFFSET_W(offset);
      break;
      case IAE_DRVG_FSG_CNT_4_E:
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_OFFSET_OFFSET_W(offset);
      break;
      case IAE_DRVG_FSG_CNT_5_E:
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_OFFSET_OFFSET_W(offset);
      break;
      default:
      ret = IAE__ERR_INVALID_ARGS;
      break;
   }

   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return ret;
}
ERRG_codeE IAE_DRVG_setFSGPulseRepeatPeriod(const IAE_DRVG_fsgCounterNumE counter, const UINT32 period)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;

   /*Sanity check all the arguments before doing any writes*/
   if(counter > IAE_DRVG_FSG_CNT_5_E)
   {
      return IAE__ERR_INVALID_ARGS; 
   }
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);   /*Vote that the IAE is enabled*/
   switch(counter)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      IAE_SNSR_CTRL_FSG_FTRIG0_PULSE_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      IAE_SNSR_CTRL_FSG_FTRIG1_PULSE_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      IAE_SNSR_CTRL_FSG_FTRIG2_PULSE_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_3_E:
      IAE_SNSR_CTRL_FSG_FTRIG3_PULSE_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_4_E:
      IAE_SNSR_CTRL_FSG_FTRIG4_PULSE_PERIOD_PERIOD_W(period);
      break;
      case IAE_DRVG_FSG_CNT_5_E:
      IAE_SNSR_CTRL_FSG_FTRIG5_PULSE_PERIOD_PERIOD_W(period);
      break;
      default:
      ret = IAE__ERR_INVALID_ARGS;
      break;
   }
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return ret;
}
/**
 * @brief Calculates the required period for the FSG counters to achieve the FPS defined in FPS
 * 
 *
 * @param FPS FPS in Hz
 * @return Returns the FSG counter period to use to achieve the FPS defined in FPS
 */
static UINT32 IAE_DRVG_convertFPSToCounterPeriod(const double FPS)
{
  UINT32 counterFrequency = 0;
  GME_DRVG_getCiifClk(&counterFrequency);
  const double counterPeriod_F = ((double)counterFrequency)/FPS; /*Calculate in floating point first*/
  const UINT32 counterPeriod = (UINT32) counterPeriod_F; /*Convert to UINT32 after*/
  return (UINT32) counterPeriod;
}
void IAE_DRVG_writeFSGEnable(IAE_DRVG_fsgCounterSrc counter, bool enable)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);                        /*Vote that the IAE is enabled*/
   UINT32 original_value = IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG_CNT_EN_R;

   switch(counter)
   {
      case IAE_DRVG_FSG_CNTSRC_0_E:
         if(enable)
            original_value |= (1<< 0);
         else
            original_value &= ~(1<<(0));
         break;
      case IAE_DRVG_FSG_CNTSRC_1_E:
         if(enable)
            original_value |= (1<< 1);
         else
            original_value &= ~(1<<(1));
         break;
         break;
      case IAE_DRVG_FSG_CNTSRC_2_E:
         if(enable)
            original_value |= (1<< 2);
         else
            original_value &= ~(1<<(2));
         break;
         break;
   }
   IAE_SNSR_CTRL_FSG_CNTR_CTRL_FSG_CNT_EN_W(original_value);
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
}
UINT32 IAE_DRBG_getFSGCounterPeriod(IAE_DRVG_fsgCounterSrc counter)
{
   UINT32 value = 0;
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);                        /*Vote that the IAE is enabled*/
   switch(counter)
   {
      case IAE_DRVG_FSG_CNTSRC_0_E:
         value = IAE_SNSR_CTRL_FSG_CNTR0_PERIOD_PERIOD_R;
         break;
      case IAE_DRVG_FSG_CNTSRC_1_E:
         value = IAE_SNSR_CTRL_FSG_CNTR1_PERIOD_PERIOD_R;
         break;
      case IAE_DRVG_FSG_CNTSRC_2_E:
         value = IAE_SNSR_CTRL_FSG_CNTR2_PERIOD_PERIOD_R;
         break;
   }
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return value;
}
ERRG_codeE IAE_DRVG_setFSGCounterPeriod(IAE_DRVG_fsgCounterSrc counter, double FPS)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);                        /*Vote that the IAE is enabled*/
   const UINT32 cntPeriod =  IAE_DRVG_convertFPSToCounterPeriod(FPS);   /*Period will depend on the clock rate of the counters*/
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Using counter period %lu to achieve %.3f FPS \n",cntPeriod,FPS);
   switch(counter)
   {
      case IAE_DRVG_FSG_CNTSRC_0_E:
         IAE_SNSR_CTRL_FSG_CNTR0_PERIOD_PERIOD_W(cntPeriod);
         break;
      case IAE_DRVG_FSG_CNTSRC_1_E:
         IAE_SNSR_CTRL_FSG_CNTR1_PERIOD_PERIOD_W(cntPeriod);
         break;
      case IAE_DRVG_FSG_CNTSRC_2_E:
         IAE_SNSR_CTRL_FSG_CNTR2_PERIOD_PERIOD_W(cntPeriod);
         break;
   }
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   return ret;
}

UINT32 enabledSlu = 0;
void IAE_DRVG_disableAllSlu()
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   enabledSlu = IAE_ENABLE_VAL;
   IAE_ENABLE_VAL &= 0xFFFFFFC0;
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   //20ms should be enough for longest readout (RGB full resolution)
   OS_LYRG_usleep(30000);
}

void IAE_DRVG_enableAllSlu()
{
   HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   IAE_ENABLE_VAL |= (enabledSlu & 0x3F);
   HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_getSLUWriteValueEnable
*
*  Description: Enable IAE Blocks
*
*  Inputs: An IAE block to Enable
*
*  Outputs: ERRG_codeE
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
void IAE_DRVG_getSLUWriteValueEnable(const IAE_DRVG_enableModuleE  *enableBlock, UINT32 *enableValue)
{

   switch(*enableBlock)
   {
      
      case IAE_DRVG_IAU_ENABLE_ALL_BLOCKS_E:
      {
         *enableValue = 0xFFFFFFFF;
         break;
      }
      case IAE_DRVG_IAU_DISABLE_ALL_BLOCKS_E:
      {
         *enableValue=0;
         break;
      }
      case IAE_DRVG_SLU0_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_SLU0_EN_POS;
         break;
      }
      case IAE_DRVG_SLU1_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_SLU1_EN_POS;
         break;
      }
      case IAE_DRVG_SLU2_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_SLU2_EN_POS;
         break;
      }
      case IAE_DRVG_SLU3_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_SLU3_EN_POS;
         break;
      }
      case IAE_DRVG_SLU4_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_SLU4_EN_POS;
         break;
      }
      case IAE_DRVG_SLU5_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_SLU5_EN_POS;
         break;
      }
      case IAE_DRVG_GEN0_EN_E:
      {
          *enableValue |= 1 << IAE_ENABLE_GEN0_EN_POS;
         break;
      }
      case IAE_DRVG_GEN1_EN_E:
      {
          *enableValue |= 1 << IAE_ENABLE_GEN1_EN_POS;
         break;
      }
      case IAE_DRVG_GEN2_EN_E:
      {
          *enableValue |= 1 << IAE_ENABLE_GEN2_EN_POS;
         break;
      }
      case IAE_DRVG_GEN3_EN_E:
      {
          *enableValue |= 1 << IAE_ENABLE_GEN3_EN_POS;
         break;
      }
      case IAE_DRVG_IAU0_EN_E:
      {
          *enableValue |= 1 << IAE_ENABLE_IAU0_EN_POS;
         break;
      }
      case IAE_DRVG_IAU1_EN_E:
      {
          *enableValue |= 1 << IAE_ENABLE_IAU1_EN_POS;
         break;
      }
      case IAE_DRVG_IAU2_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_IAU2_EN_POS;
         break;
      }
      case IAE_DRVG_IAU3_EN_E:
      {
         *enableValue |= 1 << IAE_ENABLE_IAU3_EN_POS;
         break;
      }
   }
   // TODO: Remove
   //HELSINKI_startFSGTimers();
}
UINT32 IAE_DRBVG_getCurrentSLUEnableValue()
{
   return IAE_ENABLE_VAL;
}
/****************************************************************************
*
*  Function Name: IAE_DRVG_enableCfg
*
*  Description: Enable IAE Blocks
*
*  Inputs: An IAE block to Enable
*
*  Outputs: ERRG_codeE
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
void IAE_DRVG_getSLUWriteValueDisable(const IAE_DRVG_enableModuleE  *enableBlock, UINT32 *disableValue)
{
   if (*enableBlock < IAE_DRVG_GEN0_EN_E)
      enabledSlu = enabledSlu & (~(1 << *enableBlock));
   switch(*enableBlock)
   {
      case IAE_DRVG_IAU_ENABLE_ALL_BLOCKS_E:
      {
         *disableValue = 0xFFFFFFFF;
         break;
      }
      case IAE_DRVG_IAU_DISABLE_ALL_BLOCKS_E:
      {
         *disableValue=0;
         break;
      }
      case IAE_DRVG_SLU0_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_SLU0_EN_POS) ;
         break;
      }
      case IAE_DRVG_SLU1_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_SLU1_EN_POS);
         break;
      }
      case IAE_DRVG_SLU2_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_SLU2_EN_POS);
         break;
      }
      case IAE_DRVG_SLU3_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_SLU3_EN_POS);
         break;
      }
      case IAE_DRVG_SLU4_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_SLU4_EN_POS);
         break;
      }
      case IAE_DRVG_SLU5_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_SLU5_EN_POS);
         break;
      }
      case IAE_DRVG_GEN0_EN_E:
      {
         *disableValue &= ~(1 <<IAE_ENABLE_GEN0_EN_POS);
         break;
      }
      case IAE_DRVG_GEN1_EN_E:
      {
         *disableValue &= ~(1 <<IAE_ENABLE_GEN1_EN_POS);
         break;
      }
      case IAE_DRVG_GEN2_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_GEN2_EN_POS);
         break;
      }
      case IAE_DRVG_GEN3_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_GEN3_EN_POS);
         break;
      }
      case IAE_DRVG_IAU0_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_IAU0_EN_POS);
         break;
      }
      case IAE_DRVG_IAU1_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_IAU1_EN_POS);
         break;
      }
      case IAE_DRVG_IAU2_EN_E:
      {
         *disableValue &= ~(1 <<  IAE_ENABLE_IAU2_EN_POS);
         break;
      }
      case IAE_DRVG_IAU3_EN_E:
      {
         *disableValue &= ~(1 << IAE_ENABLE_IAU3_EN_POS);
         break;
      }
   }
}

void IAE_DRVG_writeSLUEnable(UINT32 value)
{
   LOGG_PRINT(LOG_INFO_E,NULL,"Writing IAE ENABLE value to be %x \n", value);
   IAE_ENABLE_VAL = value;
}
ERRG_codeE IAE_DRVG_enableCfg(const IAE_DRVG_enableModuleE  *enableBlock)
{
   ERRG_codeE              ret = IAE__RET_SUCCESS;
   switch(*enableBlock)
   {
      case IAE_DRVG_IAU_ENABLE_ALL_BLOCKS_E:
      {
         IAE_ENABLE_VAL=0xFFFFFFFF;
         break;
      }
      case IAE_DRVG_IAU_DISABLE_ALL_BLOCKS_E:
      {
         IAE_ENABLE_VAL=0;
         break;
      }
      case IAE_DRVG_SLU0_EN_E:
      {
         IAE_ENABLE_SLU0_EN_W(1);
         break;
      }
      case IAE_DRVG_SLU1_EN_E:
      {
         IAE_ENABLE_SLU1_EN_W(1);
         break;
      }
      case IAE_DRVG_SLU2_EN_E:
      {
         IAE_ENABLE_SLU2_EN_W(1);
         break;
      }
      case IAE_DRVG_SLU3_EN_E:
      {
         IAE_ENABLE_SLU3_EN_W(1);
         break;
      }
      case IAE_DRVG_SLU4_EN_E:
      {
         IAE_ENABLE_SLU4_EN_W(1);
         break;
      }
      case IAE_DRVG_SLU5_EN_E:
      {
         IAE_ENABLE_SLU5_EN_W(1);
         break;
      }
      case IAE_DRVG_GEN0_EN_E:
      {
         IAE_ENABLE_GEN0_EN_W(1);
         break;
      }
      case IAE_DRVG_GEN1_EN_E:
      {
         IAE_ENABLE_GEN1_EN_W(1);
         break;
      }
      case IAE_DRVG_GEN2_EN_E:
      {
         IAE_ENABLE_GEN2_EN_W(1);
         break;
      }
      case IAE_DRVG_GEN3_EN_E:
      {
         IAE_ENABLE_GEN3_EN_W(1);
         break;
      }
      case IAE_DRVG_IAU0_EN_E:
      {
         IAE_ENABLE_IAU0_EN_W(1);
         break;
      }
      case IAE_DRVG_IAU1_EN_E:
      {
         IAE_ENABLE_IAU1_EN_W(1);
         break;
      }
      case IAE_DRVG_IAU2_EN_E:
      {
         IAE_ENABLE_IAU2_EN_W(1);
         break;
      }
      case IAE_DRVG_IAU3_EN_E:
      {
         IAE_ENABLE_IAU3_EN_W(1);
         break;
      }
      default:
      {
         ret = IAE__ERR_ENABLE_FAIL_WRONG_MODULE;
         break;
      }
   }
   // TODO: Remove
   //HELSINKI_startFSGTimers();
   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_disableCfg
*
*  Description: Enable IAE Blocks
*
*  Inputs: An IAE block to Enable
*
*  Outputs: ERRG_codeE
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
ERRG_codeE IAE_DRVG_disableCfg(const IAE_DRVG_enableModuleE  *enableBlock)
{
   ERRG_codeE              ret = IAE__RET_SUCCESS;


   switch(*enableBlock)
   {
      case IAE_DRVG_IAU_ENABLE_ALL_BLOCKS_E:
      {
         IAE_ENABLE_VAL=0xFFFFFFFF;
         break;
      }
      case IAE_DRVG_IAU_DISABLE_ALL_BLOCKS_E:
      {
         IAE_ENABLE_VAL=0;
         break;
      }
      case IAE_DRVG_SLU0_EN_E:
      {
         IAE_ENABLE_SLU0_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_SLU1_EN_E: 
      {                       
         IAE_ENABLE_SLU1_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_SLU2_EN_E: 
      {                       
         IAE_ENABLE_SLU2_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_SLU3_EN_E: 
      {                       
         IAE_ENABLE_SLU3_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_SLU4_EN_E: 
      {                       
         IAE_ENABLE_SLU4_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_SLU5_EN_E:
      {                       
         IAE_ENABLE_SLU5_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_GEN0_EN_E: 
      {                       
         IAE_ENABLE_GEN0_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_GEN1_EN_E: 
      {                       
         IAE_ENABLE_GEN1_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_GEN2_EN_E: 
      {                       
         IAE_ENABLE_GEN2_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_GEN3_EN_E: 
      {                       
         IAE_ENABLE_GEN3_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_IAU0_EN_E: 
      {                       
         IAE_ENABLE_IAU0_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_IAU1_EN_E: 
      {                       
         IAE_ENABLE_IAU1_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_IAU2_EN_E: 
      {                       
         IAE_ENABLE_IAU2_EN_W(0);
         break;               
      }                       
      case IAE_DRVG_IAU3_EN_E: 
      {                       
         IAE_ENABLE_IAU3_EN_W(0);
         break;
      }
      default:
      {
         ret = IAE__ERR_ENABLE_FAIL_WRONG_MODULE;
         break;
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_FrrCfg
*
*  Description: frame rate reduction
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
ERRG_codeE IAE_DRVG_FrrCfg(const IAE_DRVG_FrrCfgT *frrCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   IAE_FRR_CFG_VEC_EN_W(frrCfg->vecEn);
   IAE_FRR_CFG_VEC_SIZE_W(frrCfg->vecSize);
   IAE_FRR_INIT_GO_W(frrCfg->go);
   return ret;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_iimCtrlCfg
*
*  Description:  Main IIM functionalities
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

#if 1
void IAE_DRVG_iaeIimSrcCfg(UINT8 iaeNum, IAE_DRVG_iaeMatrixSelectionE  srcSelcet)
{
   UINT32 val = IAE_IIM_CONTROL_VAL & ~(0xF << (4 * iaeNum));
   IAE_IIM_CONTROL_VAL = val | (srcSelcet << (4 * iaeNum));
}
#else
ERRG_codeE IAE_DRVG_iimCtrlCfg(const IAE_DRVG_iaeIimCtrlCfgT  *iimCtrlCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   IAE_IIM_CONTROL_IAU0_SEL_W(iimCtrlCfg->iau0Select);
   IAE_IIM_CONTROL_IAU1_SEL_W(iimCtrlCfg->iau1Select);
   IAE_IIM_CONTROL_IAU2_SEL_W(iimCtrlCfg->iau2Select);
   IAE_IIM_CONTROL_IAU3_SEL_W(iimCtrlCfg->iau3Select);
   return ret;
}
#endif

/****************************************************************************
*
*  Function Name: IAE_DRVG_mipiMuxCfg
*
*  Description:   mipi mux configuration, for example, slu0 connected through lanes01 to phy0 (default)
* and through lanes 23 to phy1/5.
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
ERRG_codeE IAE_DRVG_mipiMuxCfg(IAE_DRVG_sluSelE inst, UINT32 mipiMuxCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch( inst )
   {
      case IAE_DRVG_SLU0_E:
         IAE_MIPI_MUX_SLU0_LANES23_SEL_W(mipiMuxCfg);
      break;
      case IAE_DRVG_SLU1_E:
         IAE_MIPI_MUX_SLU1_LANES23_SEL_W(mipiMuxCfg);
      break;
      case IAE_DRVG_SLU2_E:
         IAE_MIPI_MUX_SLU2_LANES23_SEL_W(mipiMuxCfg);
      break;
      case IAE_DRVG_SLU3_E:
         IAE_MIPI_MUX_SLU3_LANES01_SEL_W(mipiMuxCfg);
      break;
      case IAE_DRVG_SLU4_E:
         IAE_MIPI_MUX_SLU4_LANES01_SEL_W(mipiMuxCfg);
      break;
      case IAE_DRVG_SLU5_E:
         IAE_MIPI_MUX_SLU5_LANES01_SEL_W(mipiMuxCfg);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL,"invalid instance=%d\n",inst);
         ret = IAE__ERR_INVALID_ARGS;
      break;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_rxClkEnCfg
*
*  Description: Enable rx clk
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: IAE control
*
****************************************************************************/
ERRG_codeE  IAE_DRVG_rxClkEnCfg(INU_DEFSG_mipiInstE inst)
{
   ERRG_codeE ret = IAE__RET_SUCCESS;
   switch( inst )
   {
      case INU_DEFSG_MIPI_INST_0_E:
         IAE_SLU0_CONTROL_ENABLECLK_W(1);
      break;
      case INU_DEFSG_MIPI_INST_1_E:
         IAE_SLU1_CONTROL_ENABLECLK_W(1);
      break;
      case INU_DEFSG_MIPI_INST_2_E:
         IAE_SLU2_CONTROL_ENABLECLK_W(1);
      break;
      case INU_DEFSG_MIPI_INST_3_E:
         IAE_SLU3_CONTROL_ENABLECLK_W(1);
      break;
      case INU_DEFSG_MIPI_INST_4_E:
         IAE_SLU4_CONTROL_ENABLECLK_W(1);
      break;
      case INU_DEFSG_MIPI_INST_5_E:
         IAE_SLU5_CONTROL_ENABLECLK_W(1);
      break;
      default:
         LOGG_PRINT(LOG_ERROR_E, NULL,"invalid instance=%d\n",inst);
         ret = IAE__ERR_INVALID_ARGS;
      break;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_FGenCfg
*
*  Description: frame generator function.
*
*  Inputs: mode, num of frames, frame & line blanking, frame resolution
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
ERRG_codeE IAE_DRVG_FGenCfg(const IAE_DRVG_iaeFGenCfgT  *fGenerateCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;

   switch (fGenerateCfg->genSelect)
   {
      case IAE_DRVG_GEN_SEL0_E:
         IAE_FGEN0_BLANK_FRAME_W(fGenerateCfg->frameBlank);
         IAE_FGEN0_BLANK_LINE_W(fGenerateCfg->lineBlank);
         IAE_FGEN0_CONTROL_FRAME_NUM_W(fGenerateCfg->frameNum);
         IAE_FGEN0_CONTROL_MODE_W(fGenerateCfg->mode);
         IAE_FGEN0_FRAME_SIZE_HORZ_W(fGenerateCfg->frameSizeHorz);
         IAE_FGEN0_FRAME_SIZE_VERT_W(fGenerateCfg->frameSizeVert);
      break;

      case IAE_DRVG_GEN_SEL1_E:
         IAE_FGEN1_BLANK_FRAME_W(fGenerateCfg->frameBlank);
         IAE_FGEN1_BLANK_LINE_W(fGenerateCfg->lineBlank);
         IAE_FGEN1_CONTROL_FRAME_NUM_W(fGenerateCfg->frameNum);
         IAE_FGEN1_CONTROL_MODE_W(fGenerateCfg->mode);
         IAE_FGEN1_FRAME_SIZE_HORZ_W(fGenerateCfg->frameSizeHorz);
         IAE_FGEN1_FRAME_SIZE_VERT_W(fGenerateCfg->frameSizeVert);
      break;

      case IAE_DRVG_GEN_SEL2_E:
         IAE_FGEN2_BLANK_FRAME_W(fGenerateCfg->frameBlank);
         IAE_FGEN2_BLANK_LINE_W(fGenerateCfg->lineBlank);
         IAE_FGEN2_CONTROL_FRAME_NUM_W(fGenerateCfg->frameNum);
         IAE_FGEN2_CONTROL_MODE_W(fGenerateCfg->mode);
         IAE_FGEN2_FRAME_SIZE_HORZ_W(fGenerateCfg->frameSizeHorz);
         IAE_FGEN2_FRAME_SIZE_VERT_W(fGenerateCfg->frameSizeVert);
      break;

      case IAE_DRVG_GEN_SEL3_E:
         IAE_FGEN3_BLANK_FRAME_W(fGenerateCfg->frameBlank);
         IAE_FGEN3_BLANK_LINE_W(fGenerateCfg->lineBlank);
         IAE_FGEN3_CONTROL_FRAME_NUM_W(fGenerateCfg->frameNum);
         IAE_FGEN3_CONTROL_MODE_W(fGenerateCfg->mode);
         IAE_FGEN3_FRAME_SIZE_HORZ_W(fGenerateCfg->frameSizeHorz);
         IAE_FGEN3_FRAME_SIZE_VERT_W(fGenerateCfg->frameSizeVert);
      break;

      default:
         ret = IAE__ERR_FRAME_GEN_FAIL_WRONG_MODULE;
      break;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_iauCtrlCfg
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
ERRG_codeE IAE_DRVG_iauCtrlCfg(const IAE_DRVG_iauCfgT   *iauCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (iauCfg->iauSelect)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_CONTROL_FRAME_TYPE_W(iauCfg->iauCtrlCfg.frameType);
         IAE_IAU0_CONTROL_BAYER_PATTERN_W(iauCfg->iauCtrlCfg.bayerPattern);
      break;
      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_CONTROL_FRAME_TYPE_W(iauCfg->iauCtrlCfg.frameType);
         IAE_IAU1_CONTROL_BAYER_PATTERN_W(iauCfg->iauCtrlCfg.bayerPattern);
      break;

      default:
         ret = IAE__ERR_IAU_FAIL_WRONG_MODULE;
      break;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_histRoiCfg
*
*  Description: Configures the histogram ROI register. Note - zero value is iligal
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
ERRG_codeE IAE_DRVG_histGetRoiCfg(IAE_DRVG_iauSelectE iauSelectIdx, IAE_DRVG_histRoiCfgT *cfgP)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;

   switch (iauSelectIdx)
   {
      case IAE_DRVG_IAU_0_E:
         cfgP->roi[0].x0 = IAE_IAU0_HIST_ROI0_NW_X0_R;
         cfgP->roi[0].y0 = IAE_IAU0_HIST_ROI0_NW_Y0_R;
         cfgP->roi[0].x1 = IAE_IAU0_HIST_ROI0_SE_X1_R;
         cfgP->roi[0].y1 = IAE_IAU0_HIST_ROI0_SE_Y1_R;
         cfgP->roi[1].x0 = IAE_IAU0_HIST_ROI1_NW_X0_R;
         cfgP->roi[1].y0 = IAE_IAU0_HIST_ROI1_NW_Y0_R;
         cfgP->roi[1].x1 = IAE_IAU0_HIST_ROI1_SE_X1_R;
         cfgP->roi[1].y1 = IAE_IAU0_HIST_ROI1_SE_Y1_R;
         cfgP->roi[2].x0 = IAE_IAU0_HIST_ROI2_NW_X0_R;
         cfgP->roi[2].y0 = IAE_IAU0_HIST_ROI2_NW_Y0_R;
         cfgP->roi[2].x1 = IAE_IAU0_HIST_ROI2_SE_X1_R;
         cfgP->roi[2].y1 = IAE_IAU0_HIST_ROI2_SE_Y1_R;
      break;

      case IAE_DRVG_IAU_1_E:
         cfgP->roi[0].x0 = IAE_IAU1_HIST_ROI0_NW_X0_R;
         cfgP->roi[0].y0 = IAE_IAU1_HIST_ROI0_NW_Y0_R;
         cfgP->roi[0].x1 = IAE_IAU1_HIST_ROI0_SE_X1_R;
         cfgP->roi[0].y1 = IAE_IAU1_HIST_ROI0_SE_Y1_R;
         cfgP->roi[1].x0 = IAE_IAU1_HIST_ROI1_NW_X0_R;
         cfgP->roi[1].y0 = IAE_IAU1_HIST_ROI1_NW_Y0_R;
         cfgP->roi[1].x1 = IAE_IAU1_HIST_ROI1_SE_X1_R;
         cfgP->roi[1].y1 = IAE_IAU1_HIST_ROI1_SE_Y1_R;
         cfgP->roi[2].x0 = IAE_IAU1_HIST_ROI2_NW_X0_R;
         cfgP->roi[2].y0 = IAE_IAU1_HIST_ROI2_NW_Y0_R;
         cfgP->roi[2].x1 = IAE_IAU1_HIST_ROI2_SE_X1_R;
         cfgP->roi[2].y1 = IAE_IAU1_HIST_ROI2_SE_Y1_R;
      break;

      default:
         ret = IAE__ERR_HISTOGRAM_CFG_FAIL_WRONG_IAU;
      break;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_histRoiCfg
*
*  Description: Configures the histogram ROI register. Note - zero value is iligal
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
ERRG_codeE IAE_DRVG_histRoiCfg(IAE_DRVG_iauSelectE iauSelectIdx, IAE_DRVG_histRoiCfgT *cfgP)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;

   switch (iauSelectIdx)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_HIST_ROI0_NW_X0_W(cfgP->roi[0].x0);
         IAE_IAU0_HIST_ROI0_NW_Y0_W(cfgP->roi[0].y0);
         IAE_IAU0_HIST_ROI0_SE_X1_W(cfgP->roi[0].x1);
         IAE_IAU0_HIST_ROI0_SE_Y1_W(cfgP->roi[0].y1);
         IAE_IAU0_HIST_ROI1_NW_X0_W(cfgP->roi[1].x0);
         IAE_IAU0_HIST_ROI1_NW_Y0_W(cfgP->roi[1].y0);
         IAE_IAU0_HIST_ROI1_SE_X1_W(cfgP->roi[1].x1);
         IAE_IAU0_HIST_ROI1_SE_Y1_W(cfgP->roi[1].y1);
         IAE_IAU0_HIST_ROI2_NW_X0_W(cfgP->roi[2].x0);
         IAE_IAU0_HIST_ROI2_NW_Y0_W(cfgP->roi[2].y0);
         IAE_IAU0_HIST_ROI2_SE_X1_W(cfgP->roi[2].x1);
         IAE_IAU0_HIST_ROI2_SE_Y1_W(cfgP->roi[2].y1);
      break;

      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_HIST_ROI0_NW_X0_W(cfgP->roi[0].x0);
         IAE_IAU1_HIST_ROI0_NW_Y0_W(cfgP->roi[0].y0);
         IAE_IAU1_HIST_ROI0_SE_X1_W(cfgP->roi[0].x1);
         IAE_IAU1_HIST_ROI0_SE_Y1_W(cfgP->roi[0].y1);
         IAE_IAU1_HIST_ROI1_NW_X0_W(cfgP->roi[1].x0);
         IAE_IAU1_HIST_ROI1_NW_Y0_W(cfgP->roi[1].y0);
         IAE_IAU1_HIST_ROI1_SE_X1_W(cfgP->roi[1].x1);
         IAE_IAU1_HIST_ROI1_SE_Y1_W(cfgP->roi[1].y1);
         IAE_IAU1_HIST_ROI2_NW_X0_W(cfgP->roi[2].x0);
         IAE_IAU1_HIST_ROI2_NW_Y0_W(cfgP->roi[2].y0);
         IAE_IAU1_HIST_ROI2_SE_X1_W(cfgP->roi[2].x1);
         IAE_IAU1_HIST_ROI2_SE_Y1_W(cfgP->roi[2].y1);
      break;

      default:
         ret = IAE__ERR_HISTOGRAM_CFG_FAIL_WRONG_IAU;
      break;
   }

   if (ERRG_SUCCEEDED(ret))
   {
      ret = IAE_DRVG_iaeReady();
   }

   return ret;
}



/****************************************************************************
*
*  Function Name: IAE_DRVG_wbCfg
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
ERRG_codeE IAE_DRVG_wbCfg(const IAE_DRVG_iauCfgT  *iauCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (iauCfg->iauSelect)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_WB_BLUE_C_BLUE_W(iauCfg->wbCfg.wbBlueC_blue);
         IAE_IAU0_WB_BLUE_W_BLUE_W(iauCfg->wbCfg.wbBlueW_blue);
         IAE_IAU0_WB_CONTROL_W_PREC_W(iauCfg->wbCfg.wbControlW_prec);
         IAE_IAU0_WB_GB_W_GB_W(iauCfg->wbCfg.wbGbW_gb);
         IAE_IAU0_WB_GB_C_GB_W(iauCfg->wbCfg.wbGbC_gb);
         IAE_IAU0_WB_GR_W_GR_W(iauCfg->wbCfg.wbGrW_gr);
         IAE_IAU0_WB_GR_C_GR_W(iauCfg->wbCfg.wbGrC_gr);
         IAE_IAU0_WB_RED_C_RED_W(iauCfg->wbCfg.wbRedC_red);
         IAE_IAU0_WB_RED_W_RED_W(iauCfg->wbCfg.wbRedW_red);
      break;
      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_WB_BLUE_C_BLUE_W(iauCfg->wbCfg.wbBlueC_blue);
         IAE_IAU1_WB_BLUE_W_BLUE_W(iauCfg->wbCfg.wbBlueW_blue);
         IAE_IAU1_WB_CONTROL_W_PREC_W(iauCfg->wbCfg.wbControlW_prec);
         IAE_IAU1_WB_GB_W_GB_W(iauCfg->wbCfg.wbGbW_gb);
         IAE_IAU1_WB_GB_C_GB_W(iauCfg->wbCfg.wbGbC_gb);
         IAE_IAU1_WB_GR_W_GR_W(iauCfg->wbCfg.wbGrW_gr);
         IAE_IAU1_WB_GR_C_GR_W(iauCfg->wbCfg.wbGrC_gr);
         IAE_IAU1_WB_RED_C_RED_W(iauCfg->wbCfg.wbRedC_red);
         IAE_IAU1_WB_RED_W_RED_W(iauCfg->wbCfg.wbRedW_red);
      break;

      default:
         ret = IAE__ERR_WB_CFG_FAIL_WRONG_IAU;
      break;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_cscCfg
*
*  Description: rgb and yuv422 conversion coefficients
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
ERRG_codeE IAE_DRVG_cscCfg(const IAE_DRVG_iauCfgT *iauCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (iauCfg->iauSelect)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_CSC_BV0_A_W(iauCfg->cscCfg.cscBv0A);
         IAE_IAU0_CSC_BV0_B_W(iauCfg->cscCfg.cscBv0B);
         IAE_IAU0_CSC_BV1_C_W(iauCfg->cscCfg.cscBv1C);
         IAE_IAU0_CSC_BV1_D_W(iauCfg->cscCfg.cscBv1D);
         IAE_IAU0_CSC_GU0_A_W(iauCfg->cscCfg.cscGu0A);
         IAE_IAU0_CSC_GU0_B_W(iauCfg->cscCfg.cscGu0B);
         IAE_IAU0_CSC_GU1_C_W(iauCfg->cscCfg.cscGu1C);
         IAE_IAU0_CSC_GU1_D_W(iauCfg->cscCfg.cscGu1D);
         IAE_IAU0_CSC_RY0_A_W(iauCfg->cscCfg.cscRy0A);
         IAE_IAU0_CSC_RY0_B_W(iauCfg->cscCfg.cscRy0B);
         IAE_IAU0_CSC_RY1_C_W(iauCfg->cscCfg.cscRy1C);
         IAE_IAU0_CSC_RY1_D_W(iauCfg->cscCfg.cscRy1D);
      break;

      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_CSC_BV0_A_W(iauCfg->cscCfg.cscBv0A);
         IAE_IAU1_CSC_BV0_B_W(iauCfg->cscCfg.cscBv0B);
         IAE_IAU1_CSC_BV1_C_W(iauCfg->cscCfg.cscBv1C);
         IAE_IAU1_CSC_BV1_D_W(iauCfg->cscCfg.cscBv1D);
         IAE_IAU1_CSC_GU0_A_W(iauCfg->cscCfg.cscGu0A);
         IAE_IAU1_CSC_GU0_B_W(iauCfg->cscCfg.cscGu0B);
         IAE_IAU1_CSC_GU1_C_W(iauCfg->cscCfg.cscGu1C);
         IAE_IAU1_CSC_GU1_D_W(iauCfg->cscCfg.cscGu1D);
         IAE_IAU1_CSC_RY0_A_W(iauCfg->cscCfg.cscRy0A);
         IAE_IAU1_CSC_RY0_B_W(iauCfg->cscCfg.cscRy0B);
         IAE_IAU1_CSC_RY1_C_W(iauCfg->cscCfg.cscRy1C);
         IAE_IAU1_CSC_RY1_D_W(iauCfg->cscCfg.cscRy1D);
      break;

      case IAE_DRVG_IAU_2_E:
         IAE_IAU2_CSC_BV0_A_W(iauCfg->cscCfg.cscBv0A);
         IAE_IAU2_CSC_BV0_B_W(iauCfg->cscCfg.cscBv0B);
         IAE_IAU2_CSC_BV1_C_W(iauCfg->cscCfg.cscBv1C);
         IAE_IAU2_CSC_BV1_D_W(iauCfg->cscCfg.cscBv1D);
         IAE_IAU2_CSC_GU0_A_W(iauCfg->cscCfg.cscGu0A);
         IAE_IAU2_CSC_GU0_B_W(iauCfg->cscCfg.cscGu0B);
         IAE_IAU2_CSC_GU1_C_W(iauCfg->cscCfg.cscGu1C);
         IAE_IAU2_CSC_GU1_D_W(iauCfg->cscCfg.cscGu1D);
         IAE_IAU2_CSC_RY0_A_W(iauCfg->cscCfg.cscRy0A);
         IAE_IAU2_CSC_RY0_B_W(iauCfg->cscCfg.cscRy0B);
         IAE_IAU2_CSC_RY1_C_W(iauCfg->cscCfg.cscRy1C);
         IAE_IAU2_CSC_RY1_D_W(iauCfg->cscCfg.cscRy1D);
      break;

      case IAE_DRVG_IAU_3_E:
         IAE_IAU3_CSC_BV0_A_W(iauCfg->cscCfg.cscBv0A);
         IAE_IAU3_CSC_BV0_B_W(iauCfg->cscCfg.cscBv0B);
         IAE_IAU3_CSC_BV1_C_W(iauCfg->cscCfg.cscBv1C);
         IAE_IAU3_CSC_BV1_D_W(iauCfg->cscCfg.cscBv1D);
         IAE_IAU3_CSC_GU0_A_W(iauCfg->cscCfg.cscGu0A);
         IAE_IAU3_CSC_GU0_B_W(iauCfg->cscCfg.cscGu0B);
         IAE_IAU3_CSC_GU1_C_W(iauCfg->cscCfg.cscGu1C);
         IAE_IAU3_CSC_GU1_D_W(iauCfg->cscCfg.cscGu1D);
         IAE_IAU3_CSC_RY0_A_W(iauCfg->cscCfg.cscRy0A);
         IAE_IAU3_CSC_RY0_B_W(iauCfg->cscCfg.cscRy0B);
         IAE_IAU3_CSC_RY1_C_W(iauCfg->cscCfg.cscRy1C);
         IAE_IAU3_CSC_RY1_D_W(iauCfg->cscCfg.cscRy1D);
      break;

      default:
         ret = IAE__ERR_CSC_CFG_FAIL_WRONG_IAU;
      break;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_dataFormatCfg
*
*  Description: format of pixels entering IAU
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
ERRG_codeE IAE_DRVG_dataFormatCfg(const IAE_DRVG_iauCfgT   *iauCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (iauCfg->iauSelect)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_DATA_FORMAT_VAL_W(iauCfg->dataFormat);
      break;

      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_DATA_FORMAT_VAL_W(iauCfg->dataFormat);
      break;

      case IAE_DRVG_IAU_2_E:
         IAE_IAU2_DATA_FORMAT_VAL_W(iauCfg->dataFormat);
      break;

      case IAE_DRVG_IAU_3_E:
         IAE_IAU3_DATA_FORMAT_VAL_W(iauCfg->dataFormat);
      break;

      default:
         ret = IAE__ERR_DATA_FORMAT_FAIL_WRONG_IAU;
      break;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_dsrCfg
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
ERRG_codeE IAE_DRVG_dsrCfg(const IAE_DRVG_iauCfgT   *iauCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (iauCfg->iauSelect)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_DSR_BLANK_FRAME_W(iauCfg->dsrCfg.blankFrame);
         IAE_IAU0_DSR_BLANK_LINE_W(iauCfg->dsrCfg.blankLine);
         IAE_IAU0_DSR_BUFF_INIT_FULL_W(iauCfg->dsrCfg.buffInitFull);
         IAE_IAU0_DSR_BUFF_MAX_ADDR_W(iauCfg->dsrCfg.buffMaxAddr);
         IAE_IAU0_DSR_CONTROL_ORDER_W(iauCfg->dsrCfg.cscDsrOrder);
         IAE_IAU0_DSR_CONTROL_LUT_H_SIZE_W(iauCfg->dsrCfg.lutHSize);
         IAE_IAU0_DSR_CONTROL_LUT_K_W(iauCfg->dsrCfg.lutK);
         IAE_IAU0_DSR_CONTROL_LUT_L_W(iauCfg->dsrCfg.lutL);
         IAE_IAU0_DSR_CONTROL_LUT_MODE_W(iauCfg->dsrCfg.lutMode);
         IAE_IAU0_DSR_LUT_OFFSET_HORIZONTAL_SIZE_W(iauCfg->dsrCfg.lutOffsetHorSize);
         IAE_IAU0_DSR_LUT_OFFSET_VERTICAL_SIZE_W(iauCfg->dsrCfg.lutOffsetVerSize);
         IAE_IAU0_DSR_LUT_PREC_DX_PREC_W(iauCfg->dsrCfg.lutPrecDxPrec);
         IAE_IAU0_DSR_LUT_PREC_DY_PREC_W(iauCfg->dsrCfg.lutPrecDyPrec);
         IAE_IAU0_DSR_LUT_PREC_DX_SIZE_W(iauCfg->dsrCfg.lutPrecDxSize);
         IAE_IAU0_DSR_LUT_PREC_DY_SIZE_W(iauCfg->dsrCfg.lutPrecDySize);
         IAE_IAU0_DSR_OUT_SIZE_HORZ_W(iauCfg->dsrCfg.outSizeHor);
         IAE_IAU0_DSR_OUT_SIZE_VERT_W(iauCfg->dsrCfg.outSizeVer);
      break;

      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_DSR_BLANK_FRAME_W(iauCfg->dsrCfg.blankFrame);
         IAE_IAU1_DSR_BLANK_LINE_W(iauCfg->dsrCfg.blankLine);
         IAE_IAU1_DSR_BUFF_INIT_FULL_W(iauCfg->dsrCfg.buffInitFull);
         IAE_IAU1_DSR_BUFF_MAX_ADDR_W(iauCfg->dsrCfg.buffMaxAddr);
         IAE_IAU1_DSR_CONTROL_ORDER_W(iauCfg->dsrCfg.cscDsrOrder);
         IAE_IAU1_DSR_CONTROL_LUT_H_SIZE_W(iauCfg->dsrCfg.lutHSize);
         IAE_IAU1_DSR_CONTROL_LUT_K_W(iauCfg->dsrCfg.lutK);
         IAE_IAU1_DSR_CONTROL_LUT_L_W(iauCfg->dsrCfg.lutL);
         IAE_IAU1_DSR_CONTROL_LUT_MODE_W(iauCfg->dsrCfg.lutMode);
         IAE_IAU1_DSR_LUT_OFFSET_HORIZONTAL_SIZE_W(iauCfg->dsrCfg.lutOffsetHorSize);
         IAE_IAU1_DSR_LUT_OFFSET_VERTICAL_SIZE_W(iauCfg->dsrCfg.lutOffsetVerSize);
         IAE_IAU1_DSR_LUT_PREC_DX_PREC_W(iauCfg->dsrCfg.lutPrecDxPrec);
         IAE_IAU1_DSR_LUT_PREC_DY_PREC_W(iauCfg->dsrCfg.lutPrecDyPrec);
         IAE_IAU1_DSR_LUT_PREC_DX_SIZE_W(iauCfg->dsrCfg.lutPrecDxSize);
         IAE_IAU1_DSR_LUT_PREC_DY_SIZE_W(iauCfg->dsrCfg.lutPrecDySize);
         IAE_IAU1_DSR_OUT_SIZE_HORZ_W(iauCfg->dsrCfg.outSizeHor);
         IAE_IAU1_DSR_OUT_SIZE_VERT_W(iauCfg->dsrCfg.outSizeVer);
      break;

      default:
         ret = IAE__ERR_DSR_FAIL_WRONG_IAU;
      break;
   }
   return ret;
}



/****************************************************************************
*
*  Function Name: IAE_DRVG_iauFrrCfg
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
ERRG_codeE IAE_DRVG_iauFrrCfg(const IAE_DRVG_iauCfgT   *iauCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (iauCfg->iauSelect)
   {
      case IAE_DRVG_IAU_0_E:
         IAE_IAU0_FRR_CFG_VEC_EN_W(iauCfg->frrCfg.vecEn);
         IAE_IAU0_FRR_CFG_VEC_SIZE_W(iauCfg->frrCfg.vecSize);
         IAE_IAU0_FRR_INIT_GO_W(iauCfg->frrCfg.go);
      break;

      case IAE_DRVG_IAU_1_E:
         IAE_IAU1_FRR_CFG_VEC_EN_W(iauCfg->frrCfg.vecEn);
         IAE_IAU1_FRR_CFG_VEC_SIZE_W(iauCfg->frrCfg.vecSize);
         IAE_IAU1_FRR_INIT_GO_W(iauCfg->frrCfg.go);
      break;

      case IAE_DRVG_IAU_2_E:
         IAE_IAU2_FRR_CFG_VEC_EN_W(iauCfg->frrCfg.vecEn);
         IAE_IAU2_FRR_CFG_VEC_SIZE_W(iauCfg->frrCfg.vecSize);
         IAE_IAU2_FRR_INIT_GO_W(iauCfg->frrCfg.go);
      break;

      case IAE_DRVG_IAU_3_E:
         IAE_IAU3_FRR_CFG_VEC_EN_W(iauCfg->frrCfg.vecEn);
         IAE_IAU3_FRR_CFG_VEC_SIZE_W(iauCfg->frrCfg.vecSize);
         IAE_IAU3_FRR_INIT_GO_W(iauCfg->frrCfg.go);
      break;

      default:
         ret = IAE__ERR_FRR_FAIL_WRONG_IAU;
      break;
   }
   return ret;
}


/****************************************************************************
*  Function Name: IAE_DRVG_sluCropCfg
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
ERRG_codeE IAE_DRVG_sluCropCfg(const IAE_DRVG_sluCfgT   *sluCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (sluCfg->sluCtrl.sluSelect)
   {
      case IAE_DRVG_SLU0_E:
         IAE_SLU0_CROP_OFFSET_HORZ_W(sluCfg->cropOffset.horz);
         IAE_SLU0_CROP_OFFSET_VERT_W(sluCfg->cropOffset.vert);
      break;
      case IAE_DRVG_SLU1_E:
         IAE_SLU1_CROP_OFFSET_HORZ_W(sluCfg->cropOffset.horz);
         IAE_SLU1_CROP_OFFSET_VERT_W(sluCfg->cropOffset.vert);
      break;
      case IAE_DRVG_SLU2_E:
         IAE_SLU2_CROP_OFFSET_HORZ_W(sluCfg->cropOffset.horz);
         IAE_SLU2_CROP_OFFSET_VERT_W(sluCfg->cropOffset.vert);
      break;
      case IAE_DRVG_SLU3_E:
         IAE_SLU3_CROP_OFFSET_HORZ_W(sluCfg->cropOffset.horz);
         IAE_SLU3_CROP_OFFSET_VERT_W(sluCfg->cropOffset.vert);
      break;
      case IAE_DRVG_SLU4_E:
         IAE_SLU4_CROP_OFFSET_HORZ_W(sluCfg->cropOffset.horz);
         IAE_SLU4_CROP_OFFSET_VERT_W(sluCfg->cropOffset.vert);
      break;
      case IAE_DRVG_SLU5_E:
         IAE_SLU5_CROP_OFFSET_HORZ_W(sluCfg->cropOffset.horz);
         IAE_SLU5_CROP_OFFSET_VERT_W(sluCfg->cropOffset.vert);
      break;
      default:
         ret = IAE__ERR_SLU_CROP_FAIL;
      break;
   }
   return ret;
}


/****************************************************************************
*  Function Name: IAE_DRVG_frameIdRead
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
ERRG_codeE IAE_DRVG_sluFrameIdRead(IAE_DRVG_sluCfgT  *sluCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (sluCfg->sluCtrl.sluSelect)
   {
      case IAE_DRVG_SLU0_E:
         sluCfg->frameId=IAE_SLU0_FRAME_ID_ID_R;
      break;
      case IAE_DRVG_SLU1_E:
         sluCfg->frameId=IAE_SLU1_FRAME_ID_ID_R;
      break;
      case IAE_DRVG_SLU2_E:
         sluCfg->frameId=IAE_SLU2_FRAME_ID_ID_R;
      break;
      case IAE_DRVG_SLU3_E:
         sluCfg->frameId=IAE_SLU3_FRAME_ID_ID_R;
      break;
      case IAE_DRVG_SLU4_E:
         sluCfg->frameId=IAE_SLU4_FRAME_ID_ID_R;
      break;
      case IAE_DRVG_SLU5_E:
         sluCfg->frameId=IAE_SLU5_FRAME_ID_ID_R;
      break;
      default:
         ret = IAE__ERR_FRAME_ID_READ_FAIL;
      break;
   }
   return ret;
}



/****************************************************************************
*  Function Name: IAE_DRVG_sluParControl
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
ERRG_codeE IAE_DRVG_sluParControl(const IAE_DRVG_sluCfgT   *sluCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (sluCfg->sluCtrl.sluSelect)
   {
      case IAE_DRVG_SLU0_E:
         IAE_SLU0_PAR_CONTROL_TWO_CYCLE_EN_W(sluCfg->parCtrl.twoCycleEn);
         IAE_SLU0_PAR_CONTROL_FRAME_VALID_POLARITY_W(sluCfg->parCtrl.frameValidPolarity);
         IAE_SLU0_PAR_CONTROL_LINE_VALID_POLARITY_W(sluCfg->parCtrl.lineValidPolarity);
         IAE_SLU0_PAR_CONTROL_BT656_CODE_MODE_W(sluCfg->parCtrl.bt656_mode);
      break;
      case IAE_DRVG_SLU1_E:
         IAE_SLU1_PAR_CONTROL_TWO_CYCLE_EN_W(sluCfg->parCtrl.twoCycleEn);
         IAE_SLU1_PAR_CONTROL_FRAME_VALID_POLARITY_W(sluCfg->parCtrl.frameValidPolarity);
         IAE_SLU1_PAR_CONTROL_LINE_VALID_POLARITY_W(sluCfg->parCtrl.lineValidPolarity);
         IAE_SLU1_PAR_CONTROL_BT656_CODE_MODE_W(sluCfg->parCtrl.bt656_mode);
      break;
      case IAE_DRVG_SLU2_E:
      case IAE_DRVG_SLU3_E:
      case IAE_DRVG_SLU4_E:
      case IAE_DRVG_SLU5_E:
         //no HW blocks
      break;
      default:
         ret = IAE__ERR_SLU_PAR_CONTROL_FAIL;
      break;
   }
   return ret;
}

/****************************************************************************
*  Function Name: IAE_DRVG_twoCycleCtrl0
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
ERRG_codeE IAE_DRVG_twoCycleCtrl0(const IAE_DRVG_sluCfgT   *sluCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (sluCfg->sluCtrl.sluSelect)
   {
      case IAE_DRVG_SLU0_E:
         IAE_SLU0_TWO_CYCLE_CTRL0_SWAP_EN_W(sluCfg->twoCycCtrl0.swapEn);
         IAE_SLU0_TWO_CYCLE_CTRL0_SHIFT_W(sluCfg->twoCycCtrl0.shift);
         IAE_SLU0_TWO_CYCLE_CTRL0_SHIFT_DIR_W(sluCfg->twoCycCtrl0.shiftDir);
         IAE_SLU0_TWO_CYCLE_CTRL0_MASK_W(sluCfg->twoCycCtrl0.mask);
      break;
      case IAE_DRVG_SLU1_E:
         IAE_SLU1_TWO_CYCLE_CTRL0_SWAP_EN_W(sluCfg->twoCycCtrl0.swapEn);
         IAE_SLU1_TWO_CYCLE_CTRL0_SHIFT_W(sluCfg->twoCycCtrl0.shift);
         IAE_SLU1_TWO_CYCLE_CTRL0_SHIFT_DIR_W(sluCfg->twoCycCtrl0.shiftDir);
         IAE_SLU1_TWO_CYCLE_CTRL0_MASK_W(sluCfg->twoCycCtrl0.mask);
      break;
      case IAE_DRVG_SLU2_E:
      case IAE_DRVG_SLU3_E:
      case IAE_DRVG_SLU4_E:
      case IAE_DRVG_SLU5_E:
         //no HW blocks
      break;
      default:
         ret = IAE__ERR_SLU_PAR_CONTROL_FAIL;
      break;
   }
   return ret;
}


/****************************************************************************
*  Function Name: IAE_DRVG_twoCycleCtrl1
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
ERRG_codeE IAE_DRVG_twoCycleCtrl1(const IAE_DRVG_sluCfgT   *sluCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (sluCfg->sluCtrl.sluSelect)
   {
      case IAE_DRVG_SLU0_E:
         IAE_SLU0_TWO_CYCLE_CTRL1_SWAP_EN_W(sluCfg->twoCycCtrl1.swapEn);
         IAE_SLU0_TWO_CYCLE_CTRL1_SHIFT_W(sluCfg->twoCycCtrl1.shift);
         IAE_SLU0_TWO_CYCLE_CTRL1_SHIFT_DIR_W(sluCfg->twoCycCtrl1.shiftDir);
         IAE_SLU0_TWO_CYCLE_CTRL1_MASK_W(sluCfg->twoCycCtrl1.mask);
      break;
      case IAE_DRVG_SLU1_E:
         IAE_SLU1_TWO_CYCLE_CTRL1_SWAP_EN_W(sluCfg->twoCycCtrl1.swapEn);
         IAE_SLU1_TWO_CYCLE_CTRL1_SHIFT_W(sluCfg->twoCycCtrl1.shift);
         IAE_SLU1_TWO_CYCLE_CTRL1_SHIFT_DIR_W(sluCfg->twoCycCtrl1.shiftDir);
         IAE_SLU1_TWO_CYCLE_CTRL1_MASK_W(sluCfg->twoCycCtrl1.mask);
      break;
      case IAE_DRVG_SLU2_E:
      case IAE_DRVG_SLU3_E:
      case IAE_DRVG_SLU4_E:
      case IAE_DRVG_SLU5_E:
         //no HW blocks
      break;
      default:
         ret = IAE__ERR_SLU_PAR_CONTROL_FAIL;
      break;
   }
   return ret;
}


/****************************************************************************
*  Function Name: IAE_DRVG_sluFrameSize
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
ERRG_codeE IAE_DRVG_sluFrameSize(const IAE_DRVG_sluCfgT *sluCfg)
{
   ERRG_codeE  ret = IAE__RET_SUCCESS;
   switch (sluCfg->sluCtrl.sluSelect)
   {
      case IAE_DRVG_SLU0_E:
         IAE_SLU0_FRAME_SIZE_HORZ_W(sluCfg->frameSize.horz);
         IAE_SLU0_FRAME_SIZE_VERT_W(sluCfg->frameSize.vert);
      break;
      case IAE_DRVG_SLU1_E:
         IAE_SLU1_FRAME_SIZE_HORZ_W(sluCfg->frameSize.horz);
         IAE_SLU1_FRAME_SIZE_VERT_W(sluCfg->frameSize.vert);
      break;
      case IAE_DRVG_SLU2_E:
         IAE_SLU2_FRAME_SIZE_HORZ_W(sluCfg->frameSize.horz);
         IAE_SLU2_FRAME_SIZE_VERT_W(sluCfg->frameSize.vert);
      break;
      case IAE_DRVG_SLU3_E:
         IAE_SLU3_FRAME_SIZE_HORZ_W(sluCfg->frameSize.horz);
         IAE_SLU3_FRAME_SIZE_VERT_W(sluCfg->frameSize.vert);
      break;
      case IAE_DRVG_SLU4_E:
         IAE_SLU4_FRAME_SIZE_HORZ_W(sluCfg->frameSize.horz);
         IAE_SLU4_FRAME_SIZE_VERT_W(sluCfg->frameSize.vert);
      break;
      case IAE_DRVG_SLU5_E:
         IAE_SLU5_FRAME_SIZE_HORZ_W(sluCfg->frameSize.horz);
         IAE_SLU5_FRAME_SIZE_VERT_W(sluCfg->frameSize.vert);
      break;
      default:
         ret = IAE__ERR_SLU_CROP_FAIL;
      break;
   }
   return ret;
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
ERRG_codeE IAE_DRVG_iaeReady()
{
   IAE_REGISTERS_READY_DONE_W(1);
   return IAE__RET_SUCCESS;
}

/****************************************************************************
 ************************     L O C A L     F U N C T I O N S     **********************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: IAE_DRVG_clearInterrupts
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
ERRG_codeE IAE_DRVG_clearInterrupts(IAE_DRVG_intCfgT *intCfg)
{
   ERRG_codeE        ret = IAE__RET_SUCCESS;

   IAE_IRQ_CLEAR_VAL            =intCfg->status;

   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_setSluInterruptEnable
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
ERRG_codeE IAE_DRVG_setSluInterruptEnable(UINT32 mask, UINT32 slu)
{
   ERRG_codeE        ret = IAE__RET_SUCCESS;

   switch(slu)
   {
      case(0):
      IAE_SLU0_IRQ_ENABLE_VAL = mask;
      break;
      case(1):
      IAE_SLU1_IRQ_ENABLE_VAL = mask;
      break;
      case(2):
      IAE_SLU2_IRQ_ENABLE_VAL = mask;
      break;
      case(3):
      IAE_SLU3_IRQ_ENABLE_VAL = mask;
      break;
      case(4):
      IAE_SLU4_IRQ_ENABLE_VAL = mask;
      break;
      case(5):
      IAE_SLU5_IRQ_ENABLE_VAL = mask;
      break;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_setHistInterruptEnable
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
ERRG_codeE IAE_DRVG_setHistInterrupt( IAE_DRVG_iauSelectE histSelect, UINT32 on )
{
   ERRG_codeE        ret = IAE__RET_SUCCESS;
   UINT32            mask = (on << IAE_DRVG_INT_HIST_RDY_E);

   if (histSelect == IAE_DRVG_IAU_0_E)
   {
      IAE_IAU0_HIST_IRQ_ENABLE_VAL  = mask;
   }
   else if (histSelect == IAE_DRVG_IAU_1_E)
   {
      IAE_IAU1_HIST_IRQ_ENABLE_VAL  = mask;
   }
   else
   {
      ret = IAE__ERR_UNEXPECTED;
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: IAE_DRVP_setInterruptCb
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
ERRG_codeE IAE_DRVG_setInterruptCb(IAE_DRVG_setInterruptCbT *setInterruptCb)
{
   ERRG_codeE                 ret   = IAE__RET_SUCCESS;
   IAE_DRVP_deviceDescT       *devP = &IAE_DRVP_deviceDesc;
   IAE_DRVG_setInterruptCbT   *setP = setInterruptCb;

   LOGG_PRINT(LOG_DEBUG_E, NULL, "iae isr set %p\n", setP->cb);

   //One ISR is supported - overwrite
   devP->isrCb[setP->isrSrc] = setP->cb;

   return ret;
}


/****************************************************************************
 ***********************      L O C A L     F U N C T I O N S     **********************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: IAE_DRVP_accessIaeRegister
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
ERRG_codeE IAE_DRVG_accessIaeRegister(IAE_DRVG_iaeParametersT *params)
{
   if(params->accessIaeRegister.readWrite == READ_ACCESS_E)
   {
      params->accessIaeRegister.value = *(volatile UINT32 *)(IAE_BASE + params->accessIaeRegister.iaeRegisterAddress);
   }
   else
   {
      *(volatile UINT32 *)(IAE_BASE + params->accessIaeRegister.iaeRegisterAddress) = params->accessIaeRegister.value;
   }

   return IAE__RET_SUCCESS;
}

/****************************************************************************
*
*  Function Name: IAE_DRVP_dataBaseReset
*
*  Description: reset all IAE_DRVP_deviceDesc structure
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: IAE control
*
****************************************************************************/
static void IAE_DRVP_dataBaseReset(void)
{
   memset(&IAE_DRVP_deviceDesc, 0, sizeof(IAE_DRVP_deviceDesc));
}

void UVC_MAIN_handleMipi(unsigned int mipiCloseCamera);

/****************************************************************************
*
*  Function Name: IAE_DRVP_isr
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE isr
*
****************************************************************************/
static void IAE_DRVP_isr(UINT64 timestamp, UINT64 count, void *argP)
{
   IAE_DRVG_intCfgT intCtrl;
   UINT32 irqStatus, idx;
   (void)count;(void)argP;

   //read current interrupt status
   intCtrl.status = IAE_IRQ_STATUS_VAL;


 //   IAE_IRQ_CLEAR_VAL = 0x0;

   intCtrl.timestamp = timestamp;
   if (!intCtrl.status)
   {
      //sporious isr, return
      printf("sporious isr\n");
      return;
   }

   irqStatus = intCtrl.status;
   idx = 0;

    //invoke callbacks
   while (irqStatus)
   {
      if(( irqStatus & 0x1 ) && ( IAE_DRVP_deviceDesc.isrCb[idx] ))
      {
         IAE_DRVP_deviceDesc.isrCb[idx](intCtrl, idx);
      }
      irqStatus = irqStatus >> 1;
      idx++;
   }
}

static void IAE_DRVP_handleSluIsr(UINT64 timestamp, UINT32 slu)
{
   UINT32 status = 0;

   //disable the isrs
   switch(slu)
   {
      case(0):
      status = IAE_SLU0_IRQ_VAL & IAE_SLU0_IRQ_ENABLE_VAL;
      IAE_SLU0_IRQ_ENABLE_VAL &= ~status;
      break;
      case(1):
      status = IAE_SLU1_IRQ_VAL & IAE_SLU1_IRQ_ENABLE_VAL;
      IAE_SLU1_IRQ_ENABLE_VAL &= ~status;
      break;
      case(2):
      status = IAE_SLU2_IRQ_VAL & IAE_SLU2_IRQ_ENABLE_VAL;
      IAE_SLU2_IRQ_ENABLE_VAL &= ~status;
      break;
      case(3):
      status = IAE_SLU3_IRQ_VAL & IAE_SLU3_IRQ_ENABLE_VAL;
      IAE_SLU3_IRQ_ENABLE_VAL &= ~status;
      break;
      case(4):
      status = IAE_SLU4_IRQ_VAL & IAE_SLU4_IRQ_ENABLE_VAL;
      IAE_SLU4_IRQ_ENABLE_VAL &= ~status;
      break;
      case(5):
      status = IAE_SLU5_IRQ_VAL & IAE_SLU5_IRQ_ENABLE_VAL;
      IAE_SLU5_IRQ_ENABLE_VAL &= ~status;
      break;      
      default:
      break;
   }
      
   //re-enable and clear
   switch(slu)
   {
      case(0):
      IAE_SLU0_IRQ_ENABLE_VAL |= status;
      IAE_SLU0_IRQ_CLEAR_VAL = status;
      break;
      case(1):
      IAE_SLU1_IRQ_ENABLE_VAL |= status;
      IAE_SLU1_IRQ_CLEAR_VAL = status;
      break;
      case(2):
      IAE_SLU2_IRQ_ENABLE_VAL |= status;
      IAE_SLU2_IRQ_CLEAR_VAL = status;
      break;
      case(3):
      IAE_SLU3_IRQ_ENABLE_VAL |= status;
      IAE_SLU3_IRQ_CLEAR_VAL = status;
      break;
      case(4):
      IAE_SLU4_IRQ_ENABLE_VAL |= status;
      IAE_SLU4_IRQ_CLEAR_VAL = status;
      break;
      case(5):
      IAE_SLU5_IRQ_ENABLE_VAL |= status;
      IAE_SLU5_IRQ_CLEAR_VAL = status;
      break;      
      default:
      break;
   } 
   UINT32               temp,ctr = 0,i;
   IAE_DRVP_deviceDescT *devP = &IAE_DRVP_deviceDesc;


   for (int i = 0; i < IAE_DRVP_MAX_NUM_SNSR_CB; i++)
   {
      if (devP->eofIsrCb[i].used)
      {
         devP->eofIsrCb[i].isrCb(timestamp, slu, devP->eofIsrCb[i].argP);
      }
   }


   

   //get the isr type and handle it
   if (status)
   {
      //todo - implement cb's to iae_DRV lvl
      //assumption - only eof isr enabled
      HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_IAE_SLU0_FRAME_END_ISR_E + slu , timestamp, 1);
      //LOGG_PRINT(LOG_INFO_E, NULL, "SLU%d isr 0x%x, timestamp = %llu\n",slu,status,timestamp);
   }
}



static void IAE_DRVP_handleIauIsr(UINT64 timestamp, UINT32 iau)
{
   UINT32 status = 0;

   //disable the isrs
   switch(iau)
   {
      case(0):
      status = IAE_IAU0_IRQ_VAL & IAE_IAU0_IRQ_ENABLE_VAL;
      IAE_IAU0_IRQ_ENABLE_VAL &= ~status;
      break;
      case(1):
      status = IAE_IAU1_IRQ_VAL & IAE_IAU1_IRQ_ENABLE_VAL;
      IAE_IAU1_IRQ_ENABLE_VAL &= ~status;
      break;
      case(2):
      status = IAE_IAU2_IRQ_VAL & IAE_IAU2_IRQ_ENABLE_VAL;
      IAE_IAU2_IRQ_ENABLE_VAL &= ~status;
      break;
      case(3):
      status = IAE_IAU3_IRQ_VAL & IAE_IAU3_IRQ_ENABLE_VAL;
      IAE_IAU3_IRQ_ENABLE_VAL &= ~status;
      break;
      default:
      break;
   }
      
   //get the isr type and handle it
   if (status)
   {
      //todo - implement cb's to iae_DRV lvl
      //assumption - only eof isr enabled
      HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_IAE_IAU0_FRAME_END_ISR_E + iau , timestamp, 1);
      LOGG_PRINT(LOG_INFO_E, NULL, "IAU%d isr 0x%x, timestamp = %llu\n",iau,status,timestamp);
   }  

   //re-enable and clear
   switch(iau)
   {
      case(0):
      IAE_IAU0_IRQ_ENABLE_VAL |= status;
      IAE_IAU0_IRQ_CLEAR_VAL = status;
      break;
      case(1):
      IAE_IAU1_IRQ_ENABLE_VAL |= status;
      IAE_IAU1_IRQ_CLEAR_VAL = status;
      break;
      case(2):
      IAE_IAU2_IRQ_ENABLE_VAL |= status;
      IAE_IAU2_IRQ_CLEAR_VAL = status;
      break;
      case(3):
      IAE_IAU3_IRQ_ENABLE_VAL |= status;
      IAE_IAU3_IRQ_CLEAR_VAL = status;
      break;
      default:
      break;
   }  
}

static void IAE_DRVP_handleHistIsr(UINT64 timestamp, UINT32 hist)
{
   UINT32 status;

   //disable the isrs
   switch(hist)
   {
      case(0):
      status = IAE_IAU0_HIST_IRQ_VAL & IAE_IAU0_HIST_IRQ_ENABLE_VAL;
      IAE_IAU0_HIST_IRQ_ENABLE_VAL &= ~status;
      break;
      case(1):
      status = IAE_IAU1_HIST_IRQ_VAL & IAE_IAU1_HIST_IRQ_ENABLE_VAL;
      IAE_IAU1_HIST_IRQ_ENABLE_VAL &= ~status;
      break;
      default:
      break;
   }

   if (status)
   {
      IAE_MNGRG_iaeInfoT *iaeInfoP = IAE_MNGRG_getIaeInfo();
      iaeInfoP->histogramCb[hist](iaeInfoP->histogramCbArg[hist], timestamp, hist);
   }

   //re-enable and clear
   switch(hist)
   {
      case(0):
      IAE_IAU0_HIST_IRQ_CLEAR_VAL = status;     
      IAE_IAU0_HIST_IRQ_ENABLE_VAL |= status;
      break;
      case(1):
      IAE_IAU1_HIST_IRQ_CLEAR_VAL = status;     
      IAE_IAU1_HIST_IRQ_ENABLE_VAL |= status;
      break;
      default:
      break;
   }

   if (status)
   {
      HCG_MNGRG_processEvent(HCG_MNGRG_HW_EVENT_IAE_IAU_HIST0_RDY_ISR_E + hist, timestamp, 1);
   }
}

/****************************************************************************
*
*  Function Name: IAE_DRVP_sluInterruptCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE ISR context
*
****************************************************************************/
static void IAE_DRVP_sluInterruptCb(IAE_DRVG_intCfgT intCtrl, IAE_DRVG_iaeIntE isrSrc)
{
   IAE_DRVP_handleSluIsr(intCtrl.timestamp,isrSrc);
}


/****************************************************************************
*
*  Function Name: IAE_DRVP_iauInterruptCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE ISR context
*
****************************************************************************/
static void IAE_DRVP_iauInterruptCb(IAE_DRVG_intCfgT intCtrl, IAE_DRVG_iaeIntE isrSrc)
{
   UINT32            iau = (isrSrc - IAE_DRVG_IAU0_INTERRUPT_E);

   IAE_DRVP_handleIauIsr(intCtrl.timestamp,iau);
   if (iau < 2)
   {
    IAE_DRVP_handleHistIsr(intCtrl.timestamp,iau);
   }

}

/****************************************************************************
*
*  Function Name: IAE_DRVP_snrsCtrlInterruptCb
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE ISR context
*
****************************************************************************/
static void IAE_DRVP_snrsCtrlInterruptCb(IAE_DRVG_intCfgT intCtrl, IAE_DRVG_iaeIntE isrSrc)
{
   UINT32               status,temp,ctr = 0,i;
   IAE_DRVP_deviceDescT *devP = &IAE_DRVP_deviceDesc;
   (void)isrSrc;
   status = IAE_SNSR_CTRL_IRQ_VAL & IAE_SNSR_CTRL_IRQ_ENABLE_VAL;
   IAE_SNSR_CTRL_IRQ_ENABLE_VAL &= ~status;

   temp = (status >> 16);
   while (temp)
   {
      if ((temp & 0x1) == 0x1)
      {
         for (i = 0; i < IAE_DRVP_MAX_NUM_SNSR_CB; i++)
         {
            if (devP->snsrIsrCb[i].used)
            {
               devP->snsrIsrCb[i].isrCb(intCtrl.timestamp, ctr, devP->snsrIsrCb[i].argP);
            }
         }
      }
      temp  = temp >> 1;
      ctr++;
   }

   IAE_SNSR_CTRL_IRQ_CLEAR_VAL   = status;
   IAE_SNSR_CTRL_IRQ_ENABLE_VAL |= status;
}


/****************************************************************************
*
*  Function Name: IAE_DRVG_registerSnsrIsrCb
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
ERRG_codeE IAE_DRVG_registerSnsrIsrCb(IAE_DRVG_snsrInterruptCbT cb, void *argP, void **entryP)
{
   IAE_DRVP_deviceDescT       *devP = &IAE_DRVP_deviceDesc;
   UINT32 i;

   for (i = 0; i < IAE_DRVP_MAX_NUM_SNSR_CB; i++)
   {
      if (!devP->snsrIsrCb[i].used)
      {
         *entryP = &devP->snsrIsrCb[i];
         devP->snsrIsrCb[i].isrCb = cb;
         devP->snsrIsrCb[i].argP  = argP;
         devP->snsrIsrCb[i].used  = 1;
         return IAE__RET_SUCCESS;
      }
   }
   LOGG_PRINT(LOG_ERROR_E, NULL, "failed to register slu snsrs isr cb\n");
   return IAE__ERR_OUT_OF_RSRCS;
}
/****************************************************************************
*
*  Function Name: IAE_DRVG_registerEofISRCb
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
ERRG_codeE IAE_DRVG_registerEofISRCb(IAE_DRVG_snsrInterruptCbT cb, void *argP, void **entryP)
{
   IAE_DRVP_deviceDescT       *devP = &IAE_DRVP_deviceDesc;
   UINT32 i;

   for (i = 0; i < IAE_DRVP_MAX_NUM_SNSR_CB; i++)
   {
      if (!devP->eofIsrCb[i].used)
      {
         *entryP = &devP->eofIsrCb[i];
         devP->eofIsrCb[i].isrCb = cb;
         devP->eofIsrCb[i].argP  = argP;
         devP->eofIsrCb[i].used  = 1;
         return IAE__RET_SUCCESS;
      }
   }
   LOGG_PRINT(LOG_ERROR_E, NULL, "failed to register EOF isr cb\n");
   return IAE__ERR_OUT_OF_RSRCS;
}
/****************************************************************************
*
*  Function Name: IAE_DRVG_unregisterSnsrIsrCb
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
ERRG_codeE IAE_DRVG_unregisterEofISRCb(void *entryP)
{
   IAE_DRVP_snsrIsrCbT *cbEntryP = (IAE_DRVP_snsrIsrCbT*)entryP;
   memset(cbEntryP,0,sizeof(IAE_DRVP_snsrIsrCbT));
   return IAE__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: IAE_DRVG_unregisterSnsrIsrCb
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
ERRG_codeE IAE_DRVG_unregisterSnsrIsrCb(void *entryP)
{
   IAE_DRVP_snsrIsrCbT *cbEntryP = (IAE_DRVP_snsrIsrCbT*)entryP;
   memset(cbEntryP,0,sizeof(IAE_DRVP_snsrIsrCbT));
   return IAE__RET_SUCCESS;
}

ERRG_codeE IAE_DRVG_registerPhyCb(IAE_DRVG_phyInterruptCbT cb, unsigned int ind)
{
   IAE_DRVP_deviceDescT       *devP = &IAE_DRVP_deviceDesc;

   devP->isrPhyCb[ind] = cb;
   printf("************************set %s %d ind %d cb %x\n",__FILE__,__LINE__,ind,cb);
   return IAE__ERR_OUT_OF_RSRCS;
}

UINT32 IAE_DRVG_readSpare()
{
   return IAE_SPARE_RW0_SPARE_R;
}
/****************************************************************************
*
*  Function Name: IAE_DRVG_init
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
ERRG_codeE IAE_DRVG_init( UINT32 memVirtAddr )
{
   ERRG_codeE retCode = IAE__RET_SUCCESS;
   UINT32 i;
   IAE_DRVP_deviceDescT       *devP = &IAE_DRVP_deviceDesc;

   IAE_DRVP_dataBaseReset();
   IAE_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "failed to init iae regs db\n");
      return retCode;
   }

   //register IAU's isr
   if(ERRG_SUCCEEDED(retCode))
   {
      IAE_DRVG_setInterruptCbT cb;

      for (i = 0; i < 4; i++)
      {
         cb.isrSrc = IAE_DRVG_IAU0_INTERRUPT_E + i; 
         cb.cb=IAE_DRVP_iauInterruptCb;
         IAE_DRVG_setInterruptCb(&cb);
      }

      for (i = 0; i < 6; i++)
      {
         cb.isrSrc = IAE_DRVG_SLU0_INTERRUPT_E + i; 
         cb.cb=IAE_DRVP_sluInterruptCb;
         IAE_DRVG_setInterruptCb(&cb);
      }
      
      //snrs ctrl isr
      cb.isrSrc = IAE_DRVG_SNSR_CTRL_INTERRUPT_E; 
      cb.cb=IAE_DRVP_snrsCtrlInterruptCb;
      IAE_DRVG_setInterruptCb(&cb);
      
      memset(devP->isrPhyCb,0,sizeof(devP->isrPhyCb));
   }
   retCode = OS_LYRG_intCtrlRegister(OS_LYRG_INT_IAE_E, IAE_DRVP_isr, NULL);

   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "failed to register iae isr\n");
   }
   else
   {
      retCode = OS_LYRG_intCtrlEnable(OS_LYRG_INT_IAE_E);
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "failed to enable iae isr\n");
      }
   }

   return retCode;
}

void IAE_DRVG_enableInt(void)
{
    IAE_IRQ_ENABLE_VAL = 0x3f0;
}

void IAE_DRVG_disableInt(void)
{
    IAE_IRQ_ENABLE_VAL = 0;
}

void IAE_DRVG_deinit(void)
{
}

/****************************************************************************
*
*  Function Name: IAE_DRVG_dumpRegs
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
ERRG_codeE IAE_DRVG_dumpRegs()
{
   UINT32 regsOffset[] = {iae_offset_tbl_values};
   UINT32 regsResetVal[] = {iae_regs_reset_val};
   UINT32 reg;
   UINT8  buf[2048];

/*
   //to dump all the registers
   LOGG_PRINT(LOG_INFO_E, NULL, "IAE registers (number of regs = %d):\n",sizeof(regsOffset));
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg+=4)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x\n"
                                    , regsOffset[reg],   *(volatile UINT32 *)(IAE_BASE + regsOffset[reg])
                                    , regsOffset[reg+1], *(volatile UINT32 *)(IAE_BASE + regsOffset[reg+1])
                                    , regsOffset[reg+2], *(volatile UINT32 *)(IAE_BASE + regsOffset[reg+2])
                                    , regsOffset[reg+3], *(volatile UINT32 *)(IAE_BASE + regsOffset[reg+3]));
   }
*/

   memset(buf,0,sizeof(buf));
   sprintf(buf + strlen(buf), "Modified IAE registers:\n");
//   LOGG_PRINT(LOG_INFO_E, NULL, "Modified IAE registers:\n");
   //compare against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(IAE_BASE + regsOffset[reg]) != regsResetVal[reg])
      {
//         LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(IAE_BASE + regsOffset[reg]));
         sprintf(buf + strlen(buf), "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(IAE_BASE + regsOffset[reg]));
      }
   }
   printf("%s",buf);

   return IAE__RET_SUCCESS;
}




void IAE_DRVG_setRamManagerMode(UINT32 mode)
{
   IAE_RAM_MANAGER_MODE_W(mode);
}

UINT32 IAE_DRVG_getRamManagerMode(void)
{
   return IAE_RAM_MANAGER_MODE_R;
}
#if 0
void set_frame_generator(int genNum, int frameNum, int frameWidth, int frameHeight, int mode, int fblank, int lblank)
{
  switch (genNum) 
    {
    case 0:
      IAE_FGEN0_CONTROL_MODE_W(mode);
      IAE_FGEN0_CONTROL_FRAME_NUM_W(frameNum);
      IAE_FGEN0_FRAME_SIZE_HORZ_W(frameWidth);
      IAE_FGEN0_FRAME_SIZE_VERT_W(frameHeight);
      IAE_FGEN0_BLANK_FRAME_W(fblank);
      IAE_FGEN0_BLANK_LINE_W(lblank);
      break;

    case 1:
      IAE_FGEN1_CONTROL_MODE_W(mode);
      IAE_FGEN1_CONTROL_FRAME_NUM_W(frameNum);
      IAE_FGEN1_FRAME_SIZE_HORZ_W(frameWidth);
      IAE_FGEN1_FRAME_SIZE_VERT_W(frameHeight);
      IAE_FGEN1_BLANK_FRAME_W(fblank);
      IAE_FGEN1_BLANK_LINE_W(lblank);        
      break;

    case 2:
      IAE_FGEN2_CONTROL_MODE_W(mode);
      IAE_FGEN2_CONTROL_FRAME_NUM_W(frameNum);
      IAE_FGEN2_FRAME_SIZE_HORZ_W(frameWidth);
      IAE_FGEN2_FRAME_SIZE_VERT_W(frameHeight);
      IAE_FGEN2_BLANK_FRAME_W(fblank);
      IAE_FGEN2_BLANK_LINE_W(lblank);        
      break;

    case 3:
      IAE_FGEN3_CONTROL_MODE_W(mode);
      IAE_FGEN3_CONTROL_FRAME_NUM_W(frameNum);
      IAE_FGEN3_FRAME_SIZE_HORZ_W(frameWidth);
      IAE_FGEN3_FRAME_SIZE_VERT_W(frameHeight);
      IAE_FGEN3_BLANK_FRAME_W(fblank);
      IAE_FGEN3_BLANK_LINE_W(lblank);        
      break;
    }
}

//
// start frame generator
//
// arguments: #gen

void start_frame_generator(int genNum)
{
  switch (genNum) 
    {
    case 0: IAE_ENABLE_GEN0_EN_W(1);
      break;
    case 1: IAE_ENABLE_GEN1_EN_W(1);
      break;
    case 2: IAE_ENABLE_GEN2_EN_W(1);
      break;
    case 3: IAE_ENABLE_GEN3_EN_W(1);
      break;
    }
}

#define FRAME_WIDTH  1248
#define FRAME_HEIGHT 776
//#define FRAME_WIDTH  16
//#define FRAME_HEIGHT 4
#define FRAME_NUM 1
#define SOF_BLANK 1100
#define EOF_BLANK 1100
#define LINE_BLANK 640
extern void hackVsc();
extern void hackGme();

void hackGens()
{
   hackVsc();

   IAE_ENABLE_IAU0_EN_W(1);
   IAE_ENABLE_IAU1_EN_W(1);
   
   // IIM control (IIM <-> IAUs muxing) - select image gen 0
   
   IAE_IIM_CONTROL_IAU0_SEL_W(12);
   IAE_IIM_CONTROL_IAU1_SEL_W(12);

   *IAE_BYPASS_REG = 0xFFFFFFFF;

   
   printf("MIPI test: Cfg frm gen0\n");
   set_frame_generator(0, FRAME_NUM, FRAME_WIDTH-1, FRAME_HEIGHT-1, 0, FRAME_HEIGHT/2, LINE_BLANK*2);
   
   printf("MIPI test: Cfg frm gen1\n");
   set_frame_generator(1, FRAME_NUM, FRAME_WIDTH-1, FRAME_HEIGHT-1, 0, FRAME_HEIGHT/2, LINE_BLANK*2);
   
   //
   // start frame generator
   //
   // arguments: #gen
     
   start_frame_generator(0);
   start_frame_generator(1);

   IAE_REGISTERS_READY_DONE_W(1);
}
#endif
#ifdef __cplusplus
 }
#endif

