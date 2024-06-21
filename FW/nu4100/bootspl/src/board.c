#include "common.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_spi0_regs.h"
#include "board.h"
#include "gme.h"
#include "spl.h"


#define BOARD_BUSY_NUM_LOOPS		(0xFFFF)  
#define BOARD_MAX_ITERATIONS 		(100)
#define BOARD_WHILE_LOOP_LIMITED(COND, MAX_ITERATIONS)   {  \
                                                               UINT32   iteration = 0; \
                                                               UINT32   busy_loop = 0; \
                                                               while((COND) && (iteration++ < MAX_ITERATIONS)) \
                                                               { \
                                                                  busy_loop = BOARD_BUSY_NUM_LOOPS; \
                                                                  while(busy_loop--); \                                                                  
                                                               }\
                                                               if(iteration >= MAX_ITERATIONS) \
                                                               {\
                                                                  return -1;\
                                                               }\
                                                             };


static int BOARD_readPllControlReg(BOARD_PLLTypesE pllType,UINT32 *pllControl)
{
	switch (pllType)
	{
		case BOARD_PLL_AUDIO_E:
			*pllControl = GME_AUDIO_PLL_CONTROL_VAL;
			break;
		case BOARD_PLL_CPU_E:
			*pllControl = GME_CPU_PLL_CONTROL_VAL;
			break;
		case BOARD_PLL_DDR_E:
			*pllControl = GME_DDR_PLL_CONTROL_VAL;
			break;
		case BOARD_PLL_DSP_E:
			*pllControl = GME_DSP_PLL_CONTROL_VAL;
			break;
		case BOARD_PLL_SYS_E:
			*pllControl = GME_SYS_PLL_CONTROL_VAL;
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

static int BOARD_readPllStatusReg(BOARD_PLLTypesE pllType,UINT32 *pllControl)
{
	switch (pllType)
	{
		case BOARD_PLL_AUDIO_E:
			*pllControl = GME_AUDIO_PLL_STATUS_VAL;
			break;
		case BOARD_PLL_CPU_E:
			*pllControl = GME_CPU_PLL_STATUS_VAL;
			break;
		case BOARD_PLL_DDR_E:
			*pllControl = GME_DDR_PLL_STATUS_VAL;
			break;
		case BOARD_PLL_DSP_E:
			*pllControl = GME_DSP_PLL_STATUS_VAL;
			break;
		case BOARD_PLL_SYS_E:
			*pllControl = GME_SYS_PLL_STATUS_VAL;
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

static unsigned int BOARD_calc_sys_pll(BOARD_PLLTypesE pllType)
{

   unsigned int freq;

   BOARD_pllControlT pllControl;

   BOARD_readPllStatusReg(pllType,(UINT32 *)&pllControl);

   // no supprt for fraction mode
   freq = ((((UINT32 )(BOARD_OSC_CLK_HZ/pllControl.refdiv))*(pllControl.fbdiv))/pllControl.postdiv1)/pllControl.postdiv2;

   return freq;
}

static int BOARD_writePllConfigReg(BOARD_PLLTypesE pllType,UINT32 *pllConfig)
{
	switch (pllType)
	{
		case BOARD_PLL_AUDIO_E:
			GME_AUDIO_PLL_CONTROL_VAL = *pllConfig;
			break;
		case BOARD_PLL_CPU_E:
			GME_CPU_PLL_CONTROL_VAL = *pllConfig;
			break;
		case BOARD_PLL_DDR_E:
			GME_DDR_PLL_CONTROL_VAL = *pllConfig;
			break;
		case BOARD_PLL_DSP_E:
			GME_DSP_PLL_CONTROL_VAL = *pllConfig;
			break;
		case BOARD_PLL_SYS_E:
			GME_SYS_PLL_CONTROL_VAL = *pllConfig;
			break;
		default:
			return -1;
			break;
	}
	return 0;
}

static UINT32 BOARD_readPllParamReg(BOARD_PLLTypesE pllType,UINT32 *freq,UINT32 *fraq)
{
	UINT32 param;
	unsigned long long val;
	BOARD_pllControlT pllControl;

	BOARD_readPllStatusReg(pllType,(UINT32 *)&pllControl);
	if (pllControl.dsm == 1)
	{
		*freq = 0;
		*fraq = 0;
		return 0;
	}
	switch (pllType)
	{
		case BOARD_PLL_AUDIO_E:
			param = GME_AUDIO_PLL_PARAM_VAL;
			break;
		case BOARD_PLL_CPU_E:
			param = GME_CPU_PLL_PARAM_VAL;
			break;
		case BOARD_PLL_DDR_E:
			param = GME_DDR_PLL_PARAM_VAL;
			break;
		case BOARD_PLL_DSP_E:
			param = GME_DSP_PLL_PARAM_VAL;
			break;
		case BOARD_PLL_SYS_E:
			param = GME_SYS_PLL_PARAM_VAL;
			break;
		default:
			return 0;
			break;
	}
	val = ((((unsigned long long )(BOARD_OSC_CLK_HZ/pllControl.refdiv))*param)/pllControl.postdiv1)/pllControl.postdiv2;
	val = val/1000000; // in MEGA
	*freq = val /0x1000000ULL;
	*fraq = ((val % 0x1000000ULL)*10000)/0x1000000ULL;
	return 0;
}

void BOARD_print_freq(void)
{
	BOARD_PLLTypesE ind;
	UINT32 extraFreq,fraq;
	char *board_ppl_strs[]={"AUD",
						    "CPU",
	                        "DDR",
	                        "DSP",
	                        "SYS"};

	for (ind=BOARD_PLL_AUDIO_E;ind<=BOARD_PLL_SYS_E;ind++)
	{
		BOARD_readPllParamReg(ind,&extraFreq,&fraq);
		spl_log("%s frequency : %d.%dM\n",board_ppl_strs[ind],BOARD_calc_sys_pll(ind)/1000000 + extraFreq, fraq);
	}
}

static int BOARD_writePllParamReg(BOARD_PLLTypesE pllType,UINT32 fraq)
{

	unsigned long long val;
	val = 0x1000000ULL * fraq ;
	fraq = val/10000;
	switch (pllType)
	{
		case BOARD_PLL_AUDIO_E:
			GME_AUDIO_PLL_PARAM_VAL = fraq;
			break;
		case BOARD_PLL_CPU_E:
			GME_CPU_PLL_PARAM_VAL = fraq;
			break;
		case BOARD_PLL_DDR_E:
			GME_DDR_PLL_PARAM_VAL = fraq;
			break;
		case BOARD_PLL_DSP_E:
			GME_DSP_PLL_PARAM_VAL = fraq;
			break;
		case BOARD_PLL_SYS_E:
			GME_SYS_PLL_PARAM_VAL = fraq;
			break;
		default:
			return -1;
			break;
	}
	return 0;
}


void BOARD_set_freq(BOARD_PLLTypesE pllType, unsigned int numInMega,unsigned int fraq)
{

	   BOARD_pllControlT pllControl;

	   BOARD_readPllControlReg(pllType,(UINT32 *)&pllControl);
      pllControl.pll_on = 1;
      pllControl.bypass = 0;

	   pllControl.refdiv   =  12;
	   pllControl.postdiv1 = 2;
	   pllControl.postdiv2 = 1;
	   if (fraq != 0)
	   {
		   pllControl.dsm   =  0;
		   BOARD_writePllParamReg(pllType,fraq);
	   }
	   else pllControl.dsm   =  1;
	   pllControl.fbdiv = numInMega&0xfff;
      pllControl.dac = 0;

	   BOARD_writePllConfigReg(pllType,(UINT32 *)&pllControl);
}

void board_spi_init(void)
{
   GMEG_enableClk(GMEG_HW_UNIT_SPI_E);
   SPI0_ISPI_CONFIGURATION_RX_PHASE_W(1);
   SPI0_ISPI_CONFIGURATION_CLK_DIV_W(0);

   GMEG_setFcuClockDiv(2);
   GMEG_changeUnitFreq(GMEG_HW_UNIT_SPI_E);
}

void board_changeUnitFreq(GMEG_hwUnitE unit)
{
   UINT32 hwBit = 0;
   UINT32 hwStatusBit = 0;
   UINT32 hwClearStatusBit = 0;

   switch(unit)
   {
      case(GMEG_HW_UNIT_UART0_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_UART0_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_UART0_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_CLEAR_UART0_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_UART1_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_UART1_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_UART1_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_CLEAR_UART1_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_USB_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_USB_SYS_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_USB_SYS_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_CLEAR_USB_SYS_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_GPP_E):
      {
         //This function does a fast-freq change - without relocking the sys PLL.
         //Assumption: SYS PLL is locked, GPP clock config is set, SYS clock config is set
         hwBit = (1<<GME_FREQ_CHANGE_GPP_FULL_CHG_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_GPP_FULL_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_CLEAR_GPP_FULL_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_SYS_E):
      {
         //This function does a fast-freq change - without relocking the sys PLL.
         //Assumption: SYS PLL is locked, GPP clock config is set, SYS clock config is set
         hwBit = (1<<GME_FREQ_CHANGE_SYS_FULL_CHG_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_SYS_FULL_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_CLEAR_SYS_FULL_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_LRAM_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_DSP_FULL_CHG_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_DSP_FULL_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_DSP_FULL_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_SPI_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_FCU_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_FCU_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_FCU_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_USB_SYS_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_USB_SYS_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_USB_SYS_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_USB_SYS_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_AUDIO_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_AUDIO_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_AUDIO_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_AUDIO_FREQ_CHG_DONE_POS);
      }
      break;

      default:
      return;
   }

   //Stop change if active (for safety)
   GME_FREQ_CHANGE_VAL &= ~(hwBit);
   //clear status bits first (HW does not clear these)
   GME_FRQ_CHG_STATUS_CLEAR_VAL = hwClearStatusBit;
   //Do change
   GME_FREQ_CHANGE_VAL |= hwBit;

   //Wait for all change to complete
   while((GME_FRQ_CHG_STATUS_VAL & hwStatusBit) == 0);

   //Clear status bit and freq change
   GME_FRQ_CHG_STATUS_CLEAR_VAL = hwClearStatusBit;
   GME_FREQ_CHANGE_VAL &= ~hwBit;

}


void BOARD_setPowerActive(UINT32 powerModeActiveObj)
{
   if ((GME_POWER_MODE_STATUS_VAL&powerModeActiveObj) == powerModeActiveObj)
      return;

   //Set the start power change bit
   GME_CONTROL_START_POWER_CHANGE_W(0x1);

   //Set power mode register active
   *GME_POWER_MODE_REG |= powerModeActiveObj;

   //Wait for power mode to change
   while((GME_POWER_MODE_STATUS_VAL&powerModeActiveObj) != powerModeActiveObj);
}


void BOARD_setPowerMode(BOARD_powerModeE powerMode)
{  
   switch(powerMode)
   {
      case(BOARD_POWER_ALL_E):
      {         
         BOARD_setPowerActive(BOARD_POWER_ALL_E);
      }
      break;
      case(BOARD_POWER_PSS_E):
      case(BOARD_POWER_IAE_E):
      case(BOARD_POWER_DPE_E):
      case(BOARD_POWER_PPE_E):
      case(BOARD_POWER_DSP_E):
      case(BOARD_POWER_EVP_E):
     {
        BOARD_setPowerActive(1<<powerMode);
      }
      break;

      default:
         break;
   }
}


void BOARD_setIaeClk(UINT8 enable)
{  
	if (enable == 1)
	{
		if(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 0)
		{
		   GME_CLOCK_ENABLE_IAE_CLK_EN_W(1);
		   BOARD_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 0, BOARD_MAX_ITERATIONS);
		   
		   spl_log("IAE clock enabled - %d\n",GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);
		}	
		else
		{
			spl_log("IAE clock enable failed, already enabled - %d\n",GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);
		}
	}
	else
	{
		if(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 1)
		{
		   GME_CLOCK_ENABLE_IAE_CLK_EN_W(0);
		   BOARD_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 1, BOARD_MAX_ITERATIONS);
		   spl_log("IAE clock disabled - %d\n",GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);
	    }   
	    else
	    {
	   	   spl_log("IAE clock disable failed, already disabled - %d\n",GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R);
	    }
	}

}

void BOARD_setEVPClk(UINT8 enable)
{  
	spl_log("EVP clock start - %d current status %d\n",enable,GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R);
	if (enable == 1)
	{
		if(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 0)
		{
		   GME_CLOCK_ENABLE_EVP_CLK_EN_W(1);
		   BOARD_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 0, BOARD_MAX_ITERATIONS);
		   
		   spl_log("EVP clock enabled - %d\n",GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R);
		}	
		else
		{
			spl_log("EVP clock enable failed, already enabled - %d\n",GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R);
		}
	}
	else
	{
		if(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 1)
		{
		   GME_CLOCK_ENABLE_EVP_CLK_EN_W(0);
		   BOARD_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 1, BOARD_MAX_ITERATIONS);
		   spl_log("EVP clock disabled - %d\n",GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R);
	    }   
	    else
	    {
	   	   spl_log("EVP clock disable failed, already disabled - %d\n",GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R);
	    }
	}

}
