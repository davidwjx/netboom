
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include "gme_drv.h"
#include "nu4100_gme_regs.h"
#include "assert.h"

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
//TODO: move these defines to common header file

#define GME_DRVG_MAX_SAVE_AND_RESTORE (7)

#define GME_DRVP_POWER_MODE_ALL_SUSPEND (0x0)
#define GME_DRVP_POWER_MODE_ACTIVE  (0x1)
#define GME_DRVP_POWER_MODE_ALL_ACTIVE  (0x1f)
#define GME_DRVP_POWER_MODE_OFF     (0x0)

#define GME_DRVP_START_POWER_CHANGE  (0x1)
#define GME_DRVP_STOP_POWER_CHANGE   (0x0)

#define GME_DRVP_SAVE_RESTORE_RESUME_REG_NUM            (0)
#define GME_DRVP_SAVE_RESTORE_GME_CODE_RELOC_REG_NUM    (1)

#define GME_DRVP_INTERNAL_MEM_SIZE_BYTES  (2048)

#define GME_DRVP_SUSPEND_TIMER_FREQ_MHZ   (24)

#define GPIO_DRVP_NUM_OF_PIN_MUX_REGS     (9)

#define GME_DRVP_OSC_CLK_HZ (24000000U)

#define GME_DRVP_SLEEP_TIME   (30)  // 30us sleep
#define GME_DRV_DRVP_MAX_ITERATIONS (10000)


#define GME_DRVP_WHILE_LOOP_LIMITED(COND, MAX_ITERATIONS)   {  \
                                                               UINT32   iteration = 0; \
                                                               while((COND) && (iteration++ < MAX_ITERATIONS)) \
                                                               { \
                                                                  OS_LYRG_usleep(GME_DRVP_SLEEP_TIME); \
                                                               }\
                                                               if(iteration >= MAX_ITERATIONS) \
                                                               {\
                                                                  return GME__ERR_GME_NOT_READY;\
                                                               }\
                                                             };




/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/



typedef ERRG_codeE (*GME_DRVP_ioctlFuncListT)(void *argP);

typedef struct
{
   GME_DRVP_moduleStatusE     moduleStatus;
   UINT32                     deviceBaseAddress;
} GME_DRVP_deviceDescT;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static GME_DRVP_deviceDescT   GME_DRVP_deviceDesc;
static UINT32                 GME_DRVP_pinMuxRegs[GPIO_DRVP_NUM_OF_PIN_MUX_REGS];

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static ERRG_codeE GME_DRVP_setPowerActive(UINT32 powerModeActiveObj);
void GME_DRVG_setIoMux(GME_DRVG_hwUnitE unit);
static void GME_DRVP_dataBaseReset(void);
static ERRG_codeE GME_DRVG_readPllStatusReg(GME_DRVG_PLLTypesE pllType,GME_DRVG_pllStatusT *pllControl);
UINT32     IAE_DRVG_readSpare();

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

static void GME_DRVP_dataBaseReset(void)
{
   memset(&GME_DRVP_deviceDesc, 0, sizeof(GME_DRVP_deviceDesc));
}

ERRG_codeE GME_DRVG_ioControl(GME_DRVP_ioControlParamsT *params)
{
   int ret = GME__RET_SUCCESS;

   switch(params->ioSel)
   {
      case    GME_DRVG_LEDS_OE_E:
      {
        GME_IO_CONTROL_LEDS_RG_OE_W(params->val & 0x1ff);
         break;
      }
      case   GME_DRVG_STRAP_OE_E:
      {
         GME_IO_CONTROL_STRAP_OE_W(params->val & 0x1);
         break;
      }
      default:
      {
         return GME__ERR_INVALID_IO_PINS;
      }

   }

   return ret;
}


static ERRG_codeE GME_DRVP_setPowerActive(UINT32 powerModeActiveObj)
{
   int ret = GME__RET_SUCCESS;


   if ((GME_POWER_MODE_STATUS_VAL&powerModeActiveObj) == powerModeActiveObj)
      return ret;
   //Set the start power change bit
   GME_CONTROL_START_POWER_CHANGE_W(GME_DRVP_START_POWER_CHANGE);

   //Set power mode register active
   *GME_POWER_MODE_REG |= powerModeActiveObj;

   //Wait for power mode to change

   GME_DRVP_WHILE_LOOP_LIMITED((GME_POWER_MODE_STATUS_VAL&powerModeActiveObj) != powerModeActiveObj, GME_DRV_DRVP_MAX_ITERATIONS);

   return ret;

}

#if 0
static void GME_DRVP_initBaseAddress(UINT32 deviceVirtualAddress)
{
   GME_DRVP_deviceDesc.deviceBaseAddress = deviceVirtualAddress;
}
#endif

BOOL GME_DRVG_isPowerOnReset(void)
{
   BOOL ret;
   ret = (GME_GENERAL_STATUS_0_POR_OR_SUSPEND_FLAG_R == 0) ? TRUE : FALSE;
   return ret;
}

BOOL GME_DRVG_isWatchdogReset(void)
{
   BOOL ret;
   ret = (GME_GENERAL_STATUS_0_WDT_RESET_OCCURRED_R == 1) ? TRUE : FALSE;
   return ret;
}

ERRG_codeE GME_DRVG_setPowerMode(GME_DRVG_powerModeE powerMode)
{
   int ret = GME__RET_SUCCESS;

   switch(powerMode)
   {
      case(GME_DRVG_POWER_ALL_E):
      {
         ret = GME_DRVP_setPowerActive(GME_DRVG_POWER_ALL_E);
      }
      break;
      case(GME_DRVG_POWER_PSS_E):
      case(GME_DRVG_POWER_IAE_E):
      case(GME_DRVG_POWER_DPE_E):
      case(GME_DRVG_POWER_PPE_E):
      case(GME_DRVG_POWER_DSP_E):
      case(GME_DRVG_POWER_EVP_E):
     {
        ret = GME_DRVP_setPowerActive(1<<powerMode);
      }
      break;

      default:
         ret = GME__ERR_INVALID_POWER_MODE;
         break;
   }

   return ret;
}

ERRG_codeE GME_DRVG_getHwPowerMode(void)
{
   UINT32 hwMode = GME_POWER_MODE_STATUS_VAL;
   return hwMode;
}


static ERRG_codeE GME_DRVP_readPllControlReg(GME_DRVG_PLLTypesE pllType,GME_DRVG_pllControlT *pllControl)
{
   UINT32 reg;
   GME_DRVG_pllControlT *regP =(GME_DRVG_pllControlT *)&reg;
   switch (pllType)
   {
      case GME_DRVG_PLL_AUDIO_E:
         reg = GME_AUDIO_PLL_CONTROL_VAL;
         break;
      case GME_DRVG_PLL_CPU_E:
         reg = GME_CPU_PLL_CONTROL_VAL;
         break;
      case GME_DRVG_PLL_DDR_E:
         reg = GME_DDR_PLL_CONTROL_VAL;
         break;
      case GME_DRVG_PLL_DSP_E:
         reg = GME_DSP_PLL_CONTROL_VAL;
         break;
      case GME_DRVG_PLL_SYS_E:
         reg = GME_SYS_PLL_CONTROL_VAL;
         break;
      default:
         return GME__ERR_OPEN_FAIL_NULL_PARAMS;
         break;
   }
   *pllControl = *regP;
   return GME__RET_SUCCESS;
}

ERRG_codeE GME_DRVG_readPllStatusReg(GME_DRVG_PLLTypesE pllType,GME_DRVG_pllStatusT *pllStatus)
{
   UINT32 reg;
   GME_DRVG_pllStatusT *regP =(GME_DRVG_pllStatusT *)&reg;
   switch (pllType)
   {
      case GME_DRVG_PLL_AUDIO_E:
         reg = GME_AUDIO_PLL_STATUS_VAL;
         break;
      case GME_DRVG_PLL_CPU_E:
         reg = GME_CPU_PLL_STATUS_VAL;
         break;
      case GME_DRVG_PLL_DDR_E:
         reg = GME_DDR_PLL_STATUS_VAL;
         break;
      case GME_DRVG_PLL_DSP_E:
         reg = GME_DSP_PLL_STATUS_VAL;
         break;
      case GME_DRVG_PLL_SYS_E:
         reg = GME_SYS_PLL_STATUS_VAL;
         break;
      default:
         return GME__ERR_OPEN_FAIL_NULL_PARAMS;
         break;
   }
   *pllStatus = *regP;
   return GME__RET_SUCCESS;
}

/*
static ERRG_codeE GME_DRVP_writePllConfigReg(GME_DRVG_PLLTypesE pllType,GME_DRVG_pllControlT pllConfig)
{
   UINT32 reg = (UINT32 )pllConfig;
   switch (pllType)
   {
      case GME_DRVG_PLL_AUDIO_E:
         GME_AUDIO_PLL_CONTROL_VAL = reg;
         break;
      case GME_DRVG_PLL_CPU_E:
         GME_CPU_PLL_CONTROL_VAL = reg;
         break;
      case GME_DRVG_PLL_DDR_E:
         GME_DDR_PLL_CONTROL_VAL = reg;
         break;
      case GME_DRVG_PLL_DSP_E:
         GME_DSP_PLL_CONTROL_VAL = reg;
         break;
      case GME_DRVG_PLL_SYS_E:
         GME_SYS_PLL_CONTROL_VAL = reg;
         break;
      default:
         return GME__ERR_OPEN_FAIL_NULL_PARAMS;
         break;
   }
   *pllConfig = *((GME_DRVG_pllControlT *)&reg);
   return GME__RET_SUCCESS;
}*/


unsigned int GME_DRVG_isSysPllEnabled(GME_DRVG_PLLTypesE pllType)
{

   GME_DRVG_pllControlT pllControl;

   pllControl.pll_on = 0;
   GME_DRVP_readPllControlReg(pllType,&pllControl);
   return pllControl.pll_on;
}


void GME_DRVG_getSysPllConfig(GME_DRVG_pllObjConfigT *pllObjConfig)
{
  GME_DRVP_readPllControlReg(pllObjConfig->pllType,&pllObjConfig->pllControl);

  return;
}

UINT32 GME_DRVG_readPllFraq(GME_DRVG_PLLTypesE pllType,UINT32 *additionalfreq,UINT32 *fraq)
{
    UINT32 param;
    unsigned long long val;
    GME_DRVG_pllStatusT pllControl;
    memset(&pllControl,0,sizeof(pllControl));

    GME_DRVG_readPllStatusReg(pllType,&pllControl);
    if (pllControl.dsm == 1)
    {
        *additionalfreq = 0;
        *fraq = 0;
        return 0;
    }
    switch (pllType)
    {
        case GME_DRVG_PLL_AUDIO_E:
            param = GME_AUDIO_PLL_PARAM_VAL;
            break;
        case GME_DRVG_PLL_CPU_E:
            param = GME_CPU_PLL_PARAM_VAL;
            break;
        case GME_DRVG_PLL_DDR_E:
            param = GME_DDR_PLL_PARAM_VAL;
            break;
        case GME_DRVG_PLL_DSP_E:
            param = GME_DSP_PLL_PARAM_VAL;
            break;
        case GME_DRVG_PLL_SYS_E:
            param = GME_SYS_PLL_PARAM_VAL;
            break;
        default:
            return 0;
            break;
    }
    val = ((((unsigned long long )(GME_DRVP_OSC_CLK_HZ/pllControl.refdiv))*param)/pllControl.postdiv1)/pllControl.postdiv2;
   // val = val/1000000; // in MEGA
    *additionalfreq = val /0x1000000ULL;
    *fraq = ((val % 0x1000000ULL)*10000)/0x1000000ULL;
    return 0;
}

unsigned int GME_DRVG_calc_pll(GME_DRVG_PLLTypesE pllType)
{
   unsigned int freq,extrafreq,fraq;
   GME_DRVG_pllStatusT pllControl;
   if (pllType < GME_DRVG_PLL_USB2_E)
   {
      memset(&pllControl,0,sizeof(pllControl));
      GME_DRVG_readPllStatusReg(pllType,&pllControl);
      if (pllControl.pll_on == TRUE)
      {
         GME_DRVG_readPllFraq(pllType,&extrafreq,&fraq);

         freq = (GME_DRVP_OSC_CLK_HZ/pllControl.refdiv)*(pllControl.fbdiv)/pllControl.postdiv1/pllControl.postdiv2;
         freq += extrafreq;
      }
      else
         freq = 0;
   }
   else
   {
      switch(pllType)
      {
         case(GME_DRVG_PLL_USB2_E):
         freq = INU_DEFSG_USB_2_CLK_MHZ * 1000000;
         break;
         case(GME_DRVG_PLL_USB3_E):
         freq = INU_DEFSG_USB_3_CLK_MHZ * 1000000;
         break;
         default:
         freq = 0;
         assert(0);
         break;
      }
   }

   return freq;
}


void GME_DRVG_configAudioPll(GME_DRVG_pllConfigT *pllP)
{
   //Turn off/on
   if(pllP->on)
   {
     GME_AUDIO_PLL_CONTROL_BYPASS_W(pllP->bypass);
     GME_AUDIO_PLL_CONTROL_REFDIV_W(pllP->refdiv);
     GME_AUDIO_PLL_CONTROL_FBDIV_W(pllP->fbdiv);
     GME_AUDIO_PLL_CONTROL_POSTDIV1_W(pllP->postdiv1);
     GME_AUDIO_PLL_CONTROL_POSTDIV2_W(pllP->postdiv2);
     GME_AUDIO_PLL_CONTROL_DACPD_W(pllP->dacpd);
     GME_AUDIO_PLL_CONTROL_DSMPD_W(pllP->dsmpd);

     GME_AUDIO_PLL_CONTROL_PLL_ON_W(1);
   }
   else
   {
      //Turn pll off - leave configuration
      GME_AUDIO_PLL_CONTROL_PLL_ON_W(0);
   }


   return;

}


void GME_DRVG_setGppClockSrc(int clkSrc)
{
   //These settings only take effect when the go bit is the freq change register is set. So order does not matter.

   if(clkSrc == GME_DRVG_GPP_CLK_SRC_OSC)
   {
      GME_GPP_CLOCK_CONFIG_CLK_SRC_W(0);
   }
   else
   {
      GME_GPP_CLOCK_CONFIG_CLK_SRC_W(1);
   }
}

int GME_DRVG_getGppClkSrc(void)
{
   int src;
   src = (GME_GPP_CLOCK_CONFIG_CLK_SRC_R == 0) ? GME_DRVG_GPP_CLK_SRC_OSC : GME_DRVG_GPP_CLK_SRC_PLL;
   return src;
}

void GME_DRVG_setCiifClkSrc(GME_DRVG_ciifClkSrcE src)
{
   GME_CIIF_CONTROL_CLK_SRC_W(src);
}


GME_DRVG_ciifClkSrcE GME_DRVG_getCiifClkSrc()
{
   return GME_CIIF_CONTROL_CLK_SRC_R;
}


void GME_DRVG_getCiifClk(UINT32 *ciifClkHzP)
{
   UINT32 div,fraq;
   GME_DRVG_ciifClkSrcE ciifClkSrc;

   ciifClkSrc  = GME_CIIF_CONTROL_CLK_SRC_R;
   div = (GME_CIIF_CONTROL_CIIF_CLK_DIV_R>>1);
   fraq = GME_CIIF_CONTROL_CIIF_CLK_DIV_R&1;

   switch(ciifClkSrc)
   {
      case GME_DRVG_CIIF_SRC_USB3PLL_E:
      {
        *ciifClkHzP = (GME_DRVG_calc_pll(GME_DRVG_PLL_USB3_E)*2)/(2*div+fraq);
      }

      break;

      case GME_DRVG_CIIF_SRC_AUDIOPLL_E:
      {
          *ciifClkHzP = (GME_DRVG_calc_pll(GME_DRVG_PLL_AUDIO_E)*2)/(2*div+fraq);
      }
      break;

      case GME_DRVG_CIIF_SRC_DSP_E:
      {
          *ciifClkHzP = (GME_DRVG_calc_pll(GME_DRVG_PLL_DSP_E)*2)/(2*div+fraq);
      }
      break;

   }

   LOGG_PRINT(LOG_DEBUG_E, NULL, "gme calc ciif clock %dHz(src=%d div=%d)\n",*ciifClkHzP, ciifClkSrc, div);
}


ERRG_codeE GME_DRVG_setUartClkSrc(int clkSrc)
{
   ERRG_codeE ret = GME__RET_SUCCESS;

   GME_PERIPH_CLOCK_CONFIG_UART_0_CLK_SRC_W(clkSrc)

   return ret;
}


ERRG_codeE GME_DRVG_setUnitClockDiv(GME_DRVG_unitClkDivT *params)
{
   switch (params->unit)
   {
      case (GME_DRVG_HW_UNIT_FCU_E):
      {
         GME_SYS_CLOCK_CONFIG_FCU_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_UNIT_UART0_E):
      {
         GME_PERIPH_CLOCK_CONFIG_UART_0_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_UNIT_UART1_E):
      {
         GME_PERIPH_CLOCK_CONFIG_1_UART_1_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_CV0_REF_CLK_EN_E):
      {
         GME_PERIPH_CLOCK_CONFIG_CV_0_REF_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_CV1_REF_CLK_EN_E):
      {
         GME_PERIPH_CLOCK_CONFIG_CV_1_REF_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_CV2_REF_CLK_EN_E):
      {
         GME_PERIPH_CLOCK_CONFIG_CV_2_REF_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_CV3_REF_CLK_EN_E):
      {
         GME_PERIPH_CLOCK_CONFIG_CV_3_REF_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_I2S_CLK_EN_E):
      {
         GME_PERIPH_CLOCK_CONFIG_I2S_M_CLK_DIV_W(params->div);
      }
      break;

      case (GME_DRVG_HW_UNIT_CIIF_E):
      {
         GME_CIIF_CONTROL_CIIF_CLK_DIV_W(params->div);
      }
      break;
      case (GME_DRVG_HW_UNIT_CIIF_PAR_0_E):
      {
         GME_CIIF_CONTROL_PAR_0_CLK_DIV_W(params->div);
      }
      break;
      case (GME_DRVG_HW_UNIT_CIIF_PAR_1_E):
      {
         GME_CIIF_CONTROL_PAR_1_CLK_DIV_W(params->div);
      }
      break;
      case (GME_DRVG_HW_AUDIO_CLK_EN_E):
      {
         GME_AUDIO_CLOCK_CONFIG_CLK_DIV_W(params->div);
         GME_AUDIO_CLOCK_CONFIG_CLK_SRC_W(1);
      }
      break;
      default:
         return GME__ERR_INVALID_CLK_MODULE;
      break;
   }

   return GME__RET_SUCCESS;

}

ERRG_codeE GME_DRVG_getUnitClockDiv(GME_DRVG_unitClkDivT *paramsP)
{
   switch (paramsP->unit)
   {

   case (GME_DRVG_HW_UNIT_UART0_E):
   {
      paramsP->div = GME_PERIPH_CLOCK_CONFIG_UART_0_CLK_DIV_R;
   }
   break;

   case (GME_DRVG_HW_UNIT_UART1_E):
   {
      paramsP->div = GME_PERIPH_CLOCK_CONFIG_1_UART_1_CLK_DIV_R;
   }
   break;

   case (GME_DRVG_HW_CV0_REF_CLK_EN_E):
   {
      paramsP->div = GME_PERIPH_CLOCK_CONFIG_CV_0_REF_CLK_DIV_R;
   }
   break;

   case (GME_DRVG_HW_CV1_REF_CLK_EN_E):
   {
      paramsP->div = GME_PERIPH_CLOCK_CONFIG_CV_1_REF_CLK_DIV_R;
   }
   break;

   case (GME_DRVG_HW_CV2_REF_CLK_EN_E):
   {
      paramsP->div = GME_PERIPH_CLOCK_CONFIG_CV_2_REF_CLK_DIV_R;
   }
   break;

   case (GME_DRVG_HW_CV3_REF_CLK_EN_E):
   {
      paramsP->div = GME_PERIPH_CLOCK_CONFIG_CV_3_REF_CLK_DIV_R;
   }
   break;

   case (GME_DRVG_HW_UNIT_CIIF_E):
   {
      paramsP->div = GME_CIIF_CONTROL_CIIF_CLK_DIV_R;
   }
   break;
   case (GME_DRVG_HW_UNIT_CIIF_PAR_0_E):
   {
      paramsP->div = GME_CIIF_CONTROL_PAR_0_CLK_DIV_R;
   }
   break;
   case (GME_DRVG_HW_UNIT_CIIF_PAR_1_E):
   {
      paramsP->div = GME_CIIF_CONTROL_PAR_1_CLK_DIV_R;
   }
   break;
   case (GME_DRVG_HW_AUDIO_CLK_EN_E):
   {
      paramsP->div = GME_AUDIO_CLOCK_CONFIG_CLK_DIV_R;
   }
   break;


   default:
      return GME__ERR_INVALID_CLK_MODULE;
   break;

   }
   return GME__RET_SUCCESS;

}

ERRG_codeE GME_DRVG_disableClk(GME_DRVG_hwUnitE unit)
{
   int ret = GME__RET_SUCCESS;

   switch(unit)
   {
      case(GME_DRVG_HW_UNIT_UART0_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_PERIPH_CLOCK_ENABLE_UART0_CLK_EN_W(0);
            //Wait for uart status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_UNIT_UART1_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_PERIPH_CLOCK_ENABLE_UART1_CLK_EN_W(0);
            //Wait for uart status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

     case(GME_DRVG_HW_UNIT_I2C0_E):
      {
         //Gate the I2C3 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_0_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_CLOCK_ENABLE_I2C_0_CLK_EN_W(0);
            //Wait for status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_0_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

     case(GME_DRVG_HW_UNIT_I2C1_E):
      {
         //Gate the I2C3 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_1_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_CLOCK_ENABLE_I2C_1_CLK_EN_W(0);
            //Wait for status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_1_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
     case(GME_DRVG_HW_UNIT_I2C2_E):
      {
         //Gate the I2C3 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_2_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_CLOCK_ENABLE_I2C_2_CLK_EN_W(0);
            //Wait for status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_2_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
     case(GME_DRVG_HW_UNIT_I2C3_E):
      {
         //Gate the I2C3 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_3_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_CLOCK_ENABLE_I2C_3_CLK_EN_W(0);
            //Wait for status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_3_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

     case(GME_DRVG_HW_UNIT_I2C4_E):
      {
         //Gate the I2C3 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_4_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_CLOCK_ENABLE_I2C_4_CLK_EN_W(0);
            //Wait for status to show it is off
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_4_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_UNIT_CVA_E):
      {
         if(GME_CLOCK_ENABLE_STATUS_CVA_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_CVA_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_CVA_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }


      case(GME_DRVG_HW_UNIT_LRAM_E):
      {
         //Enable LRAM clock
         if(GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_LRAM_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }


     case(GME_DRVG_HW_UNIT_IAE_E):
     {
         //IAE clock enable
         if(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_IAE_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

     case(GME_DRVG_HW_UNIT_DPE_E):
     {
         //DPE clock enable
         if(GME_CLOCK_ENABLE_STATUS_DPE_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_DPE_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DPE_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

     case(GME_DRVG_HW_UNIT_PPE_E):
     {
         //PPE clock enable
         if(GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_PPE_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

     case(GME_DRVG_HW_UNIT_DMA_0_E):
     {
         //DMA clock enable
         if(GME_CLOCK_ENABLE_STATUS_DMA_0_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_DMA_0_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DMA_0_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }
     case(GME_DRVG_HW_UNIT_DMA_1_E):
     {
         if(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_DMA_1_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }
     case(GME_DRVG_HW_UNIT_DMA_2_E):
     {
         if(GME_CLOCK_ENABLE_STATUS_DMA_2_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_DMA_2_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DMA_2_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
        break;
     }
     case(GME_DRVG_HW_I2S_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_I2S_CLK_EN_R== 1)
         {
            GME_PERIPH_CLOCK_ENABLE_I2S_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_I2S_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_CVLS_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_CLK_EN_R == 1)
         {
         GME_PERIPH_CLOCK_ENABLE_CV_0_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_CVRS_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_CLK_EN_R == 1)
         {
         GME_PERIPH_CLOCK_ENABLE_CV_1_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_CV0_REF_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_REF_CLK_EN_R== 1)
         {
            GME_PERIPH_CLOCK_ENABLE_CV_0_REF_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_REF_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_CV1_REF_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_REF_CLK_EN_R== 1)
         {
            GME_PERIPH_CLOCK_ENABLE_CV_1_REF_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_REF_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_CV2_REF_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_2_REF_CLK_EN_R== 1)
         {
            GME_PERIPH_CLOCK_ENABLE_CV_2_REF_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_2_REF_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_CV3_REF_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_3_REF_CLK_EN_R== 1)
         {
            GME_PERIPH_CLOCK_ENABLE_CV_3_REF_CLK_EN_W(0);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_3_REF_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }
      case(GME_DRVG_HW_UNIT_USB_E):
      {
         //controller (hie) clock disable
/*         if(GME_CLOCK_ENABLE_STATUS_HIE_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_HIE_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_HIE_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
*/
         //phy clock disable
         GME_PERIPH_CLOCK_ENABLE_OSC_USB_PHY_CLK_EN_W(0);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_OSC_USB_PHY_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);

         break;
      }

      case GME_DRVG_HW_MIPI_DPHY0_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_R== 1)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_W(0);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY1_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_R== 1)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_W(0);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY2_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_R== 1)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_W(0);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY3_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_TX_CFG_CLK_EN_R== 1)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_TX_CFG_CLK_EN_W(0);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_TX_CFG_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_UNIT_CNN_E:
      {
         if(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_EVP_CLK_EN_W(0);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      default:
      {
         ret = GME__ERR_INVALID_CLK_MODULE;
      }
      break;
   }
   return ret;
}

ERRG_codeE GME_DRVG_enableClk(GME_DRVG_hwUnitE unit)
{
   int ret = GME__RET_SUCCESS;
   switch(unit)
   {
     case(GME_DRVG_HW_UNIT_UART0_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 0)
         {
            //Clock disabled  - turn on
            GME_PERIPH_CLOCK_ENABLE_UART0_CLK_EN_W(1);
            //Wait for uart status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_AUDIO_CLK_EN_E):
      {
       if(GME_CLOCK_ENABLE_STATUS_AUDIO_CLK_EN_R == 0)
       {
          //Clock disabled  - turn on
          GME_CLOCK_ENABLE_AUDIO_CLK_EN_W(1);
          //Wait for uart status to show it is on
          GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_AUDIO_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
       }
       break;
      }

      case(GME_DRVG_HW_AUDIO_EXT_CLK_EN_E):
      {
       if(GME_PERIPH_CLOCK_ENABLE_STATUS_AUDIO_EXT_CLK_EN_R == 0)
       {
          //Clock disabled  - turn on
          GME_PERIPH_CLOCK_ENABLE_AUDIO_EXT_CLK_EN_W(1);
          //Wait for uart status to show it is on
          GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_AUDIO_EXT_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
       }
       break;
      }

      case(GME_DRVG_HW_UNIT_UART1_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 0)
         {
            //Clock disabled  - turn on
            GME_PERIPH_CLOCK_ENABLE_UART1_CLK_EN_W(1);
            //Wait for uart status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_UNIT_I2C0_E):
      {
         //Enable I2C1 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_0_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_I2C_0_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_0_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_UNIT_I2C1_E):
      {
         //Enable I2C1 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_1_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_I2C_1_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_1_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_UNIT_I2C2_E):
      {
         //Enable I2C2 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_2_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_I2C_2_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_2_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_UNIT_I2C3_E):
      {
         //Enable I2C3 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_3_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_I2C_3_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_3_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_UNIT_I2C4_E):
      {
         //Enable I2C4 clock
         if(GME_CLOCK_ENABLE_STATUS_I2C_4_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_I2C_4_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_I2C_4_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

     case(GME_DRVG_HW_UNIT_IAE_E):
     {
         //IAE clock enable
         if(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_IAE_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_IAE_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

     case(GME_DRVG_HW_UNIT_DPE_E):
     {
         //DPE clock enable
         if(GME_CLOCK_ENABLE_STATUS_DPE_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_DPE_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DPE_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

     case(GME_DRVG_HW_UNIT_PPE_E):
     {
         //PPE clock enable
         if(GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_PPE_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_PPE_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

     case(GME_DRVG_HW_UNIT_DMA_0_E):
     {
         //DMA clock enable
         if(GME_CLOCK_ENABLE_STATUS_DMA_0_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_DMA_0_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DMA_0_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }
     case(GME_DRVG_HW_UNIT_DMA_1_E):
     {
         if(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_DMA_1_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DMA_1_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }
     case(GME_DRVG_HW_UNIT_DMA_2_E):
     {
         if(GME_CLOCK_ENABLE_STATUS_DMA_2_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_DMA_2_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DMA_2_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
     }

      case(GME_DRVG_HW_UNIT_CVA_E):
      {
         //CVE clock enable
         if(GME_CLOCK_ENABLE_STATUS_CVA_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_CVA_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_CVA_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
       }

       case(GME_DRVG_HW_UNIT_LRAM_E):
      {
         //Enable LRAM clock
         if(GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_LRAM_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_LRAM_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_I2S_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_I2S_CLK_EN_R== 0)
         {
            GME_PERIPH_CLOCK_ENABLE_I2S_CLK_EN_W(1);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_I2S_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_CVLS_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_CLK_EN_R == 0)
         {
            GME_PERIPH_CLOCK_ENABLE_CV_0_CLK_EN_W(1);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_CVRS_CLK_EN_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_CLK_EN_R == 0)
         {
            GME_PERIPH_CLOCK_ENABLE_CV_1_CLK_EN_W(1);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case(GME_DRVG_HW_CV0_REF_CLK_EN_E):
      {
          if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_REF_CLK_EN_R== 0)
           {
              GME_PERIPH_CLOCK_ENABLE_CV_0_REF_CLK_EN_W(1);
              //Wait for status to show it is on
              GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_0_REF_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
           }
           break;
      }

      case(GME_DRVG_HW_CV1_REF_CLK_EN_E):
      {
          if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_REF_CLK_EN_R== 0)
           {
              GME_PERIPH_CLOCK_ENABLE_CV_1_REF_CLK_EN_W(1);
              //Wait for status to show it is on
              GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_1_REF_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
           }
           break;
      }

      case(GME_DRVG_HW_CV2_REF_CLK_EN_E):
      {
          if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_2_REF_CLK_EN_R== 0)
           {
              GME_PERIPH_CLOCK_ENABLE_CV_2_REF_CLK_EN_W(1);
              //Wait for status to show it is on
              GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_2_REF_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
           }
           break;
      }

      case(GME_DRVG_HW_CV3_REF_CLK_EN_E):
      {
          if(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_3_REF_CLK_EN_R== 0)
           {
              GME_PERIPH_CLOCK_ENABLE_CV_3_REF_CLK_EN_W(1);
              //Wait for status to show it is on
              GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_CV_3_REF_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
           }
           break;
      }

      case(GME_DRVG_HW_UNIT_GPIO_E):
      {
         if(GME_CLOCK_ENABLE_GPIO_DB_CLK_EN_R== 0)
         {
            GME_CLOCK_ENABLE_GPIO_DB_CLK_EN_W(1);
            //Wait for status to show it is on
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_GPIO_DB_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case GME_DRVG_HW_UNIT_OSC_E:
         if(GME_PERIPH_CLOCK_ENABLE_OSC_PERIPHERAL_CLK_EN_R== 0)
         {
            GME_PERIPH_CLOCK_ENABLE_OSC_PERIPHERAL_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_STATUS_OSC_PERIPHERAL_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
      break;

      case GME_DRVG_HW_MIPI_DPHY0_RX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_RX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_RX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_RX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY1_RX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_RX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_RX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_RX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY2_RX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_RX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_RX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_RX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY3_RX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_RX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_RX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_RX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY4_RX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY4_RX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY4_RX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY4_RX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY5_RX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY5_RX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY5_RX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY5_RX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY0_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY1_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY1_TX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY2_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_MIPI_DPHY3_TX_CLK_EN_E:
      if(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_TX_CFG_CLK_EN_R== 0)
      {
         GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_TX_CFG_CLK_EN_W(1);
         GME_DRVP_WHILE_LOOP_LIMITED(GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY3_TX_CFG_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
      }
      break;

      case GME_DRVG_HW_UNIT_CNN_E:
      {
         if(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_EVP_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_EVP_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      case GME_DRVG_HW_UNIT_DSP_E:
      {
         if(GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);
            GME_DRVP_WHILE_LOOP_LIMITED(GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);
         }
         break;
      }

      default:
         ret = GME__ERR_INVALID_CLK_MODULE;
      break;
   }
   return ret;
}

void GME_DRVG_enbleDbgClock(GME_DRVG_dbgClockE dbgClock, UINT32 div)
{
   UINT32 debug_clock_control_reg;
   GME_DRVG_debug_clkControlT *debug_clock_controlP=(GME_DRVG_debug_clkControlT *)&debug_clock_control_reg;
    volatile unsigned int busy_wait=2000;

   GME_IO_MUX_CTRL_0_IO_MUX_CTRL_0_W(2);

   debug_clock_controlP->div_en =1;
   debug_clock_controlP->clk_div = div;
   debug_clock_controlP->clk_src_sel = dbgClock;
    debug_clock_controlP->unused =0;
   GME_DEBUG_CLK_CONTROL_VAL = debug_clock_control_reg;

   while(busy_wait--);
}

ERRG_codeE GME_DRVG_disableDbgClock(GME_DRVG_dbgClockE dbgClock, UINT32 div)
{
   UINT32 debug_clock_control_reg;
   GME_DRVG_debug_clkControlT *debug_clock_controlP=(GME_DRVG_debug_clkControlT *)&debug_clock_control_reg;
   ERRG_codeE retCode;

    FIX_UNUSED_PARAM_WARN(div);
   debug_clock_controlP->div_en =0;
   debug_clock_controlP->clk_div = 0;
   debug_clock_controlP->clk_src_sel = dbgClock;
    debug_clock_controlP->unused =0;

   GME_DEBUG_CLK_CONTROL_VAL = debug_clock_control_reg;
   GME_DRVP_WHILE_LOOP_LIMITED(1,1); // wait

   return retCode;
}

void GME_DRVG_stopDbgClock(GME_DRVG_dbgClockE dbgClock, UINT32 div)
{
   FIX_UNUSED_PARAM_WARN(dbgClock);
   FIX_UNUSED_PARAM_WARN(div);
   GME_IO_MUX_CTRL_0_IO_MUX_CTRL_1_W(0);
}

ERRG_codeE GME_DRVG_resetUnit(GME_DRVG_hwUnitE unit)
{
   int ret = GME__RET_SUCCESS;

   switch(unit)
   {

         case(GME_DRVG_HW_UNIT_I2C0_E):
         {
              GME_SW_RESET_0_SW_I2C_0_RESET_N_W(1);
              GME_SW_RESET_0_SW_I2C_0_RESET_N_W(0);
              GME_SW_RESET_0_SW_I2C_0_RESET_N_W(1);
            break;
         }
         case(GME_DRVG_HW_UNIT_I2C1_E):
         {
              GME_SW_RESET_0_SW_I2C_1_RESET_N_W(1);
              GME_SW_RESET_0_SW_I2C_1_RESET_N_W(0);
              GME_SW_RESET_0_SW_I2C_1_RESET_N_W(1);
            break;
         }
         case(GME_DRVG_HW_UNIT_I2C2_E):
         {
              GME_SW_RESET_0_SW_I2C_2_RESET_N_W(1);
              GME_SW_RESET_0_SW_I2C_2_RESET_N_W(0);
              GME_SW_RESET_0_SW_I2C_2_RESET_N_W(1);
            break;
         }
         case(GME_DRVG_HW_UNIT_I2C3_E):
         {
              GME_SW_RESET_0_SW_I2C_3_RESET_N_W(1);
              GME_SW_RESET_0_SW_I2C_3_RESET_N_W(0);
              GME_SW_RESET_0_SW_I2C_3_RESET_N_W(1);
            break;
         }

         case(GME_DRVG_HW_UNIT_I2C4_E):
         {
              GME_SW_RESET_0_SW_I2C_4_RESET_N_W(1);
              GME_SW_RESET_0_SW_I2C_4_RESET_N_W(0);
              GME_SW_RESET_0_SW_I2C_4_RESET_N_W(1);
            break;
         }

      case(GME_DRVG_HW_UNIT_IAE_E):
      {
         GME_SW_RESET_0_SW_IAE_RESET_N_W(1);
         GME_SW_RESET_0_SW_IAE_RESET_N_W(0);
         GME_SW_RESET_0_SW_IAE_RESET_N_W(1);
         break;
      }

      case(GME_DRVG_HW_UNIT_DPE_E):
      {
         GME_SW_RESET_0_SW_DPE_RESET_N_W(1);
         GME_SW_RESET_0_SW_DPE_RESET_N_W(0);
         GME_SW_RESET_0_SW_DPE_RESET_N_W(1);
         break;
      }

      case(GME_DRVG_HW_UNIT_PPE_E):
      {
         GME_SW_RESET_0_SW_PPE_RESET_N_W(1);
         GME_SW_RESET_0_SW_PPE_RESET_N_W(0);
         GME_SW_RESET_0_SW_PPE_RESET_N_W(1);
         break;
      }

      case(GME_DRVG_HW_UNIT_DMA_0_E):
      {
         GME_SW_RESET_0_SW_DMA_0_RESET_N_W(1);
         GME_SW_RESET_0_SW_DMA_0_RESET_N_W(0);
         GME_SW_RESET_0_SW_DMA_0_RESET_N_W(1);
         break;
      }
      case(GME_DRVG_HW_UNIT_DMA_1_E):
      {
         GME_SW_RESET_0_SW_DMA_1_RESET_N_W(1);
         GME_SW_RESET_0_SW_DMA_1_RESET_N_W(0);
         GME_SW_RESET_0_SW_DMA_1_RESET_N_W(1);
         break;
      }
      case(GME_DRVG_HW_UNIT_DMA_2_E):
      {
         GME_SW_RESET_0_SW_DMA_2_RESET_N_W(1);
         GME_SW_RESET_0_SW_DMA_2_RESET_N_W(0);
         GME_SW_RESET_0_SW_DMA_2_RESET_N_W(1);
         break;
      }
      case(GME_DRVG_HW_UNIT_CVA_E):
      {
         GME_SW_RESET_1_SW_CVA_RESET_N_W(1);
         GME_SW_RESET_1_SW_CVA_RESET_N_W(0);
         GME_SW_RESET_1_SW_CVA_RESET_N_W(1);
         break;
      }

     case(GME_DRVG_HW_UNIT_DSP_E):
      {
         GME_SW_RESET_0_SW_DSPA_RESET_N_W(1);
         GME_SW_RESET_0_SW_DSPA_RESET_N_W(0);
         GME_SW_RESET_0_SW_DSPA_RESET_N_W(1);
         break;
      }

      case(GME_DRVG_HW_UNIT_ISP_E):
       {
          // power down
          GME_POWER_MODE_EVP_POWER_UP_W(0);
          while(!(GME_POWER_MODE_STATUS_EVP_POWER_UP_R==1));
         // power up
          GME_POWER_MODE_EVP_POWER_UP_W(1);
          while(!(GME_POWER_MODE_STATUS_EVP_POWER_UP_R==0));

         break;
       }


      default:
      {
         ret = GME__ERR_INVALID_CLK_MODULE;
      }
      break;
   }
   return ret;
}

//Version and ID read
ERRG_codeE GME_DRVG_getVersion(GME_DRVG_getVersionParamsT *getVersionParamsP)
{
   ERRG_codeE retCode = GME__RET_SUCCESS;

   getVersionParamsP->val = GME_NU4000_VERSION_VAL;

   return GME__RET_SUCCESS;
}

//Fuse array
UINT32 GME_DRVG_getFuse32(UINT16 offset)
{
   UINT32 val = 0;;

   val = *(GME_FUSE_SHADOW_0_REG + offset);

   return val;
}
#define GME_MUX_CTRL_REGISTER_ENABLE_PULL_DOWN_RESISTOR (1<<3)
/**
 * @brief Switches UART0_SIN to be in GPIO mode instead of UART mode 
 */
void GME_DRVG_switchUART0SinToGPIOIn()
{
   /*1 = GPIO Functionaltiy and PIN 21 (UART0SIN) is CTRL2.CTRL1.
   Set this with a pull down enabled too
   */
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_1_W(1 | GME_MUX_CTRL_REGISTER_ENABLE_PULL_DOWN_RESISTOR  )
}
/**
 * @brief Switches UART0_SOUT to be in GPIO mode instead of UART mode 
 */
void GME_DRVG_switchUART0SoutToGPIOIn()
{
   /*1 = GPIO Functionaltiy and PIN 22 (UART0SOUT) is CTRL2.CTRL2, 
   Set this with a pull down enabled too
   */
   GME_IO_MUX_CTRL_2_IO_MUX_CTRL_2_W(1 | GME_MUX_CTRL_REGISTER_ENABLE_PULL_DOWN_RESISTOR)
}

UINT8 GME_DRVG_getFuseSecurityControl()
{
   UINT8 val = 0;;

   val = GME_FUSE_SHADOW_5_SECURITY_CONTROL_R;

   return val;
}
void GME_DRVG_switchWAKEUPToGPIOIn()
{   
   GME_IO_MUX_CTRL_4_IO_MUX_CTRL_5_W(1)
}
void GME_DRVG_enableAllFTrigIOOutputEnables()
{
   GME_IO_CONTROL_FSG_FTRIG_OE_W(0x7F);
}
/**
 * @brief Switches FTRIG0 to be in FTRIG mode instead of GPIO mode 
 */
void GME_DRVG_modifyFTRIGMode(IAE_DRVG_fsgCounterNumE fsgCounter,GME_DRVG_triggerMode trigMode)

{
   UINT8 trigModeW = 0;
   if(trigMode == FSG_BLOCK)
   {
      trigModeW = 0;
   }
   else if(trigMode ==TRIGGER_MANAGER )
   {
      trigModeW = 1;
   }
   LOGG_PRINT(LOG_INFO_E,NULL,"Updating FSG register for fsgCounter:%lu, trigModeL:%lu \n", fsgCounter,trigMode);
   switch(fsgCounter)
   {
      case IAE_DRVG_FSG_CNT_0_E:
      /*0 = FSG FTRIG0 functionality and GIO 38  (FTRIG0) is CTRL5.CTRL3*/
      GME_IO_MUX_CTRL_5_IO_MUX_CTRL_3_W(trigModeW)
      break;
      case IAE_DRVG_FSG_CNT_1_E:
      GME_IO_MUX_CTRL_5_IO_MUX_CTRL_4_W(trigModeW)
      break;
      case IAE_DRVG_FSG_CNT_2_E:
      GME_IO_MUX_CTRL_5_IO_MUX_CTRL_5_W(trigModeW)
      break;
      case IAE_DRVG_FSG_CNT_3_E:
      GME_IO_MUX_CTRL_7_IO_MUX_CTRL_6_W(trigModeW)
      break;
      case IAE_DRVG_FSG_CNT_4_E:
      GME_IO_MUX_CTRL_7_IO_MUX_CTRL_7_W(trigModeW)
      break;
      case IAE_DRVG_FSG_CNT_5_E:
      GME_IO_MUX_CTRL_8_IO_MUX_CTRL_0_W(trigModeW)
      break;
   }
}
void GME_DRVG_switchToGPIOForExternalEvents(const GME_DRVG_externalEventE extEvent)
{
   switch(extEvent)
   {
         case GME_DRVG_EXTERNAL_EVENTS_0:
         GME_IO_MUX_CTRL_0_IO_MUX_CTRL_5_W(1);
         break;
         case GME_DRVG_EXTERNAL_EVENTS_1:
         GME_IO_MUX_CTRL_0_IO_MUX_CTRL_6_W(1);
         break;
         case GME_DRVG_EXTERNAL_EVENTS_2:
         GME_IO_MUX_CTRL_0_IO_MUX_CTRL_7_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_3:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_0_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_4:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_1_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_5:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_2_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_6:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_3_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_7:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_4_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_8:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_5_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_9:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_6_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_10:
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_7_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_11:
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_0_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_12:
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_1_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_13:
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_2_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_14:
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_3_W(1);
         break; 
         case GME_DRVG_EXTERNAL_EVENTS_15:
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_4_W(1);
         break; 
   }
}
//IO Mux
/* Set IO pin muxing for specific HW unit */
void GME_DRVG_setIoMux(GME_DRVG_hwUnitE unit)
{
   switch(unit)
   {
      case(GME_DRVG_HW_UNIT_UART0_E):
      {
         // 1. uart0_sin:   IOC_REGC_1   func 0: uart0_sdi    func 1: gio_rplca_2
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_1_W(0);
         // 2. uart0_sout: IOC_REGC_2   func 1: uart0_sdo
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_2_W(0);
         break;
      }

      case(GME_DRVG_HW_UNIT_UART1_E):
      {
         //1. uart1_sdi:  func 0
         GME_IO_MUX_CTRL_4_IO_MUX_CTRL_7_W(0);
            //2. uart1_sdo:  func1
         GME_IO_MUX_CTRL_5_IO_MUX_CTRL_0_W(1);
         break;
      }
     case(GME_DRVG_HW_UNIT_I2C4_E):
      {
         ///1. i2c4_sd: func 0
         GME_IO_MUX_CTRL_1_IO_MUX_CTRL_7_W(0);
         ///2. i2c4_sc:  func 0
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_0_W(0);
         break;
      }
     case(GME_DRVG_HW_I2S_CLK_EN_E):
      {
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_7_W(0xB);
         GME_IO_MUX_CTRL_3_IO_MUX_CTRL_0_W(9);
         GME_IO_MUX_CTRL_3_IO_MUX_CTRL_1_W(9);
         GME_IO_MUX_CTRL_4_IO_MUX_CTRL_1_W(8);
         break;
      }
      case(GME_DRVG_HW_AUDIO_CLK_EN_E):
      {
         GME_IO_MUX_CTRL_2_IO_MUX_CTRL_7_W(8);
         GME_IO_MUX_CTRL_3_IO_MUX_CTRL_0_W(8);
         GME_IO_MUX_CTRL_3_IO_MUX_CTRL_1_W(8);
         GME_IO_MUX_CTRL_4_IO_MUX_CTRL_1_W(8);
         break;
      }

      default:
         break;
   }
}
void GME_SetPPSHostPinMuxing(UINT8 value)
{
    GME_IO_MUX_CTRL_7_IO_MUX_CTRL_5_W(value);
}
int GME_GetPPSHostPinMuxing()
{
   return GME_IO_MUX_CTRL_7_IO_MUX_CTRL_5_R;
}

/****************************************************************************
*
*  Function Name: GME_DRVG_savePinMuxSetup
*
*  Description: save current values of the IO pin registers
**
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
void GME_DRVG_savePinMuxSetup(void)
{
   UINT32 i;

   for (i = 0; i < GPIO_DRVP_NUM_OF_PIN_MUX_REGS; i++)
   {
      GME_DRVP_pinMuxRegs[i] = GME_DRVG_readReg(GME_IO_MUX_CTRL_0_OFFSET + i * sizeof(UINT32));
   }
}

/****************************************************************************
*
*  Function Name: GME_DRVG_restorePinMuxSetup
*
*  Description: restore the IO pin register values
**
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
void GME_DRVG_restorePinMuxSetup(void)
{
   UINT32             i;
   GME_DRVG_writeRegT params;

   for (i = 0; i < GPIO_DRVP_NUM_OF_PIN_MUX_REGS; i++)
   {
      params.offsetAddress = GME_IO_MUX_CTRL_0_OFFSET + i * sizeof(UINT32);
      params.val = GME_DRVP_pinMuxRegs[i];

      GME_DRVG_writeReg(&params);
   }
}

//General-purpose read/write GME register
ERRG_codeE GME_DRVG_writeReg(GME_DRVG_writeRegT *params)
{
   *(((volatile UINT32 *)(GME_BASE + params->offsetAddress))) = params->val;

   return GME__RET_SUCCESS;
}

UINT32 GME_DRVG_readReg(UINT32 addrOffset)
{
   return *(((volatile UINT32 *)(GME_BASE + addrOffset)));
}


/****************************************************************************
*
*  Function Name: GME_DRVP_phyBridgeWaitForAckFall
*
*  Description: USB phy register writing sequence.
*                    Details on the sequence is in DWC databook section 3.12 Control Register Access.
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
static ERRG_codeE GME_DRVP_phyBridgeWaitForAckFall( void )
{
   ERRG_codeE ret = GME__RET_SUCCESS;

   GME_DRVP_WHILE_LOOP_LIMITED(GME_PHY_BRIDGE_1_CR_ACK_R == 1, GME_DRV_DRVP_MAX_ITERATIONS);

   return ret;
}


/****************************************************************************
*
*  Function Name: GME_DRVP_phyBridgeWaitForAckRise
*
*  Description: USB phy register writing sequence.
*                    Details on the sequence is in DWC databook section 3.12 Control Register Access.
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
static ERRG_codeE GME_DRVP_phyBridgeWaitForAckRise( void )
{
   ERRG_codeE ret = GME__RET_SUCCESS;

   GME_DRVP_WHILE_LOOP_LIMITED(GME_PHY_BRIDGE_1_CR_ACK_R == 0, GME_DRV_DRVP_MAX_ITERATIONS);

   return ret;
}


/****************************************************************************
*
*  Function Name: GME_DRVP_phyBridgeCapAddr
*
*  Description: USB phy register writing sequence.
*                    Details on the sequence is in DWC databook section 3.12 Control Register Access.
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
void GME_DRVP_phyBridgeCapAddr(UINT16 phyRegAddr)
{
   //phy reg address
   GME_PHY_BRIDGE_0_CR_DATA_IN_W(phyRegAddr);

   //cap address
   GME_PHY_BRIDGE_0_CR_CAP_ADDR_W(0x1);

   //wait for ACK
   GME_DRVP_phyBridgeWaitForAckRise();

   //reset cap address
   GME_PHY_BRIDGE_0_CR_CAP_ADDR_W(0);

   //wait for ACK to reset
   GME_DRVP_phyBridgeWaitForAckFall();
}


/****************************************************************************
*
*  Function Name: GME_DRVG_setUsbPhyReg
*
*  Description: USB phy register writing sequence.
*                    Details on the sequence is in DWC databook section 3.12 Control Register Access.
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
ERRG_codeE GME_DRVG_setUsbPhyReg(GME_DRVG_usbPhyRegParamsT *usbPhyRegParamsP)
{
   GME_PHY_BRIDGE_0_VAL = 0;
   GME_PHY_BRIDGE_1_VAL = 0;

   GME_DRVP_phyBridgeCapAddr(usbPhyRegParamsP->regAddr);

   //data to write
   GME_PHY_BRIDGE_0_CR_DATA_IN_W(usbPhyRegParamsP->value);

   //capture data
   GME_PHY_BRIDGE_0_CR_CAP_DATA_W(0x1);

   //wait for ACK
   GME_DRVP_phyBridgeWaitForAckRise();

   //reset capture data
   GME_PHY_BRIDGE_0_CR_CAP_DATA_W(0);

   //wait for ACK to reset
   GME_DRVP_phyBridgeWaitForAckFall();

   //write captured data to reg
   GME_PHY_BRIDGE_0_CR_WRITE_W(0x1);

   //wait for ACK
   GME_DRVP_phyBridgeWaitForAckRise();

   //reset write captured data
   GME_PHY_BRIDGE_0_CR_WRITE_W(0);

   //wait for ACK to reset
   GME_DRVP_phyBridgeWaitForAckFall();

   return GME__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: GME_DRVG_enable_usb3_pll
*
*  Description: The following function enables the usb3 pll using the usb_phy_write protocol
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
ERRG_codeE GME_DRVG_enable_usb3_pll( void )
{
   ERRG_codeE                 ret = GME_MNGR__RET_SUCCESS;
   GME_DRVG_usbPhyRegParamsT  usbPhyRegParams = {0};

   usbPhyRegParams.regAddr = 0x12;
   ret = GME_DRVG_getUsbPhyReg( &usbPhyRegParams);
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "read USB Phy register: Addr = 0x%x, Val = 0x%x\n", usbPhyRegParams.regAddr, usbPhyRegParams.value);
   }
   else
   {
      usbPhyRegParams.value = 0;
      LOGG_PRINT(LOG_ERROR_E, NULL, "read USB Phy register fail!!! Addr = 0x%x\n", usbPhyRegParams.regAddr);
   }

   if((usbPhyRegParams.value & ((0x1 << 5) | (0x1 << 4))) != ((0x1 << 5) | (0x1 << 4)))
   {
      usbPhyRegParams.value |= (0x1 << 5) | (0x1 << 4);

      ret = GME_DRVG_setUsbPhyReg( &usbPhyRegParams);
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "write USB Phy register ioctl: Addr = 0x%x, Val = 0x%x\n", usbPhyRegParams.regAddr, usbPhyRegParams.value);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "write USB Phy register fail!!! Addr = 0x%x, val=0x%X\n", usbPhyRegParams.regAddr, usbPhyRegParams.value);
      }


      usbPhyRegParams.regAddr = 0x11;
      ret = GME_DRVG_getUsbPhyReg( &usbPhyRegParams);
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "read USB Phy register: Addr = 0x%x, Val = 0x%x\n", usbPhyRegParams.regAddr, usbPhyRegParams.value);
      }
      else
      {
         usbPhyRegParams.value = 0;
         LOGG_PRINT(LOG_ERROR_E, NULL, "read USB Phy register fail!!! Addr = 0x%x\n", usbPhyRegParams.regAddr);
      }

      usbPhyRegParams.value |= (0x1 << 0) | (0x1 << 1);

      ret = GME_DRVG_setUsbPhyReg( &usbPhyRegParams);
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "write USB Phy register ioctl: Addr = 0x%x, Val = 0x%x\n", usbPhyRegParams.regAddr, usbPhyRegParams.value);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "write USB Phy register fail!!! Addr = 0x%x, val=0x%X\n", usbPhyRegParams.regAddr, usbPhyRegParams.value);
      }

      GME_PHY_PARAM_1_REF_SSP_EN_W(1);
      GME_CONTROL_KEEP_USB3_PLL_ON_W(1);
   }
//   GME_USBP_TCA_SYSMODE_CFG_TYPEC_DISABLE_W(0);

   return ret;
}


/****************************************************************************
*
*  Function Name: GME_DRVG_getUsbPhyReg
*
*  Description: USB phy register reading sequence.
*                    Details on the sequence is in DWC databook section 3.12 Control Register Access.
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
ERRG_codeE GME_DRVG_getUsbPhyReg(GME_DRVG_usbPhyRegParamsT *usbPhyRegParamsP)
{
   GME_PHY_BRIDGE_0_VAL = 0;
   GME_PHY_BRIDGE_1_VAL = 0;

   GME_DRVP_phyBridgeCapAddr(usbPhyRegParamsP->regAddr);

   //set read
   GME_PHY_BRIDGE_0_CR_READ_W(0x1);

   //wait for ACK
   GME_DRVP_phyBridgeWaitForAckRise();

   //reset read
   GME_PHY_BRIDGE_0_CR_READ_W(0);

   //wait for ACK to reset
   GME_DRVP_phyBridgeWaitForAckFall();

   //get data now
   usbPhyRegParamsP->value = GME_PHY_BRIDGE_1_CR_DATA_OUT_R;

   return GME__RET_SUCCESS;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S     **************
 ****************************************************************************/
ERRG_codeE GME_DRVG_changeUnitFreq(GME_DRVG_unitClkDivT * params)
{
   UINT32 bitOffset = 0;
   UINT32 bitsValue = 1;
   UINT32 hwBit = 0;
   ERRG_codeE ret;

   ret = GME_DRVG_setUnitClockDiv(params);

   if(ERRG_FAILED(ret))
   {
      return ret;
   }

   switch(params->unit)
   {
      case(GME_DRVG_HW_UNIT_FCU_E):
      {
         bitOffset = GME_FREQ_CHANGE_FCU_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_UNIT_UART0_E):
      {
         bitOffset = GME_FREQ_CHANGE_UART0_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_CV0_REF_CLK_EN_E):
      {
         bitOffset             = GME_FREQ_CHANGE_CV_0_REF_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_CV1_REF_CLK_EN_E):
      {
         bitOffset             = GME_FREQ_CHANGE_CV_1_REF_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_CV2_REF_CLK_EN_E):
      {
         bitOffset             = GME_FREQ_CHANGE_CV_2_REF_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_CV3_REF_CLK_EN_E):
      {
         bitOffset             = GME_FREQ_CHANGE_CV_3_REF_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_UNIT_UART1_E):
      {
         bitOffset             = GME_FREQ_CHANGE_UART1_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_UNIT_GPP_E):
      {
          //This function does a fast-freq change - without relocking the sys PLL.
          //Assumption: SYS PLL is locked, GPP clock config is set, SYS clock config is set
         bitOffset             = GME_FREQ_CHANGE_GPP_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_UNIT_CIIF_E):
      {
         bitOffset             = GME_FREQ_CHANGE_CIIF_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_I2S_CLK_EN_E):
      {
         bitOffset             = GME_FREQ_CHANGE_I2S_GO_BIT_POS;
         break;
      }

      case(GME_DRVG_HW_AUDIO_CLK_EN_E):
      {
         bitOffset             = GME_FREQ_CHANGE_AUDIO_GO_BIT_POS;
         break;
      }

      default:
         return GME__ERR_INVALID_CLK_MODULE;
      break;
   }
   hwBit = bitsValue << bitOffset;
   GME_FRQ_CHG_STATUS_ENABLE_VAL |= bitOffset;
   //Stop change if active (for safety)
   GME_FREQ_CHANGE_VAL &= ~(hwBit);
   //clear status bits first (HW does not clear these)
   GME_FRQ_CHG_STATUS_CLEAR_VAL |= hwBit;

   //Do change
   GME_FREQ_CHANGE_VAL |= hwBit;

   //Wait for all change to complete
   GME_DRVP_WHILE_LOOP_LIMITED((GME_FRQ_CHG_STATUS_VAL & hwBit) == 0, GME_DRV_DRVP_MAX_ITERATIONS);

   //Clear status bit and freq change
   GME_FRQ_CHG_STATUS_CLEAR_VAL |= hwBit;
   GME_FREQ_CHANGE_VAL &= ~hwBit;

   return GME__RET_SUCCESS;

}


/****************************************************************************
*
*  Function Name: GME_DRVG_updateCpuPll
*
*  Description: The following function updates the CPU PLL to  1209.1392
*                    It's needed for achieving higher rates for UART
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: USB
*
****************************************************************************/
ERRG_codeE GME_DRVG_updateCpuPll( void )
{
   GME_DRVG_unitClkDivT                            gmeClkConfig;
   UINT32                                          i = 0;

   GME_PLL_LOCK_COUNT_LOCK_SELECT_W(0);
   GME_CPU_PLL_PARAM_VAL   = 0x85F110;
   GME_CPU_PLL_CONTROL_VAL = 0xA00C909;
   gmeClkConfig.div = 1;
   gmeClkConfig.unit = GME_DRVG_HW_UNIT_GPP_E;
   GME_DRVG_changeUnitFreq(&gmeClkConfig);

   while(GME_CPU_PLL_STATUS_LOCK_R == 0)
   {
      i++;
      if (i > 10)
      {
         return GME_MNGR__ERR_TIMEOUT;
      }
      OS_LYRG_usleep(5000);
   }
   return GME_MNGR__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: GME_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: GME control
*
****************************************************************************/
ERRG_codeE GME_DRVG_init(UINT32 memVirtAddr)
{
   ERRG_codeE gmeInitRetCode = GME__RET_SUCCESS;

   GME_DRVP_dataBaseReset();
   GME_DRVP_deviceDesc.deviceBaseAddress = memVirtAddr;
   GME_IO_CONTROL_STRAP_OE_W(1); //ARNON - ask amit
   return gmeInitRetCode;
}


void GMEG_mipiDphyTxConfig(INU_DEFSG_mipiInstE inst, unsigned int videoFormat)
{
   switch (inst)
   {
      case INU_DEFSG_MIPI_INST_0_E:
         GME_MIPI_DPHY_CONTROL_0_DPHY0_TX_VIDEO_FORMAT_W(videoFormat);
         break;
      case INU_DEFSG_MIPI_INST_1_E:
         GME_MIPI_DPHY_CONTROL_0_DPHY1_TX_VIDEO_FORMAT_W(videoFormat);
         break;
      case INU_DEFSG_MIPI_INST_2_E:
         GME_MIPI_DPHY_CONTROL_0_DPHY2_TX_VIDEO_FORMAT_W(videoFormat);
         break;
      case INU_DEFSG_MIPI_INST_3_E:
         GME_MIPI_DPHY_CONTROL_0_DPHY3_TX_VIDEO_FORMAT_W(videoFormat);
         break;
      default:
         break;

   }
}


void GMEG_showMipiDphyConfig(INU_DEFSG_mipiInstE inst)
{
    UINT32 videoFormat = 0;
   switch (inst)
   {
      case INU_DEFSG_MIPI_INST_0_E:
         videoFormat = GME_MIPI_DPHY_CONTROL_0_DPHY0_TX_VIDEO_FORMAT_R;
         break;
      case INU_DEFSG_MIPI_INST_1_E:
         videoFormat = GME_MIPI_DPHY_CONTROL_0_DPHY1_TX_VIDEO_FORMAT_R;
         break;
      case INU_DEFSG_MIPI_INST_2_E:
         videoFormat = GME_MIPI_DPHY_CONTROL_0_DPHY2_TX_VIDEO_FORMAT_R;
         break;
      default:
         break;


   }
   LOGG_PRINT(LOG_INFO_E, NULL,"mipi %d dphy config:  video_format=%x\n",
            inst,
            videoFormat);

}


void GMEG_mipiDphyTxSet(INU_DEFSG_mipiInstE phyNum, unsigned char set)
{
   unsigned int regVal = GME_PERIPH_CLOCK_ENABLE_VAL;
   unsigned int bitOffset = phyNum + GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_TX_CFG_CLK_EN_POS;
   if (phyNum <= GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY2_TX_CFG_CLK_EN_POS)
   {
      if (set == 1)
      {
         GME_PERIPH_CLOCK_ENABLE_VAL |= 1<<bitOffset;
      }
      else
      {
         GME_PERIPH_CLOCK_ENABLE_VAL = regVal & (~(1<<bitOffset));
      }
   }
}

void GMEG_mipiDphyRxSet(INU_DEFSG_mipiInstE phyNum, unsigned char set)
{
   unsigned int regVal = GME_PERIPH_CLOCK_ENABLE_VAL;
   unsigned int bitOffset = phyNum + GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY0_RX_CFG_CLK_EN_POS;

   if (bitOffset <= GME_PERIPH_CLOCK_ENABLE_MIPI_DPHY5_RX_CFG_CLK_EN_POS)
   {
      if (set == 1)
         GME_PERIPH_CLOCK_ENABLE_VAL  |= 1<<bitOffset;
      else
         GME_PERIPH_CLOCK_ENABLE_VAL   = regVal & (~(1<<bitOffset));
   }
}

void GMEG_mipiDphyEnable(INU_DEFSG_mipiInstE inst)
{
//   GMEG_mipiDphyTxSet(inst,1);
   GMEG_mipiDphyRxSet(inst,1);
}

void GMEG_setSluRatesfromSpare(unsigned int sluNum)
{
    unsigned int spareVal = IAE_DRVG_readSpare();

    spareVal = (spareVal>>sluNum)&1;

    switch (sluNum)
    {
    case 0:
        GME_CIIF_RATE_CONTROL_IAE_SLU0_RATE_1TO1_W(spareVal);
        break;
    case 1:
        GME_CIIF_RATE_CONTROL_IAE_SLU1_RATE_1TO1_W(spareVal);
        break;
    case 2:
        GME_CIIF_RATE_CONTROL_IAE_SLU2_RATE_1TO1_W(spareVal);
        break;
    case 3:
        GME_CIIF_RATE_CONTROL_IAE_SLU3_RATE_1TO1_W(spareVal);
        break;
    case 4:
        GME_CIIF_RATE_CONTROL_IAE_SLU4_RATE_1TO1_W(spareVal);
        break;
    case 5:
        GME_CIIF_RATE_CONTROL_IAE_SLU5_RATE_1TO1_W(spareVal);
        break;
    }

}


void GMEG_mipiDphyDisable(INU_DEFSG_mipiInstE inst)
{
   GMEG_mipiDphyTxSet(inst,0);
   GMEG_mipiDphyRxSet(inst,0);
}

void GMEG_showMipiRegs(void)
{
   LOGG_PRINT(LOG_INFO_E, NULL,"GME_PERIPH_CLOCK_ENABLE 0x%08x=0x%08x\n", GME_PERIPH_CLOCK_ENABLE_REG, GME_PERIPH_CLOCK_ENABLE_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL,"GME_MIPI_DPHY_CONTROL_0_REG 0x%08x=0x%08x\n", GME_MIPI_DPHY_CONTROL_0_REG, GME_MIPI_DPHY_CONTROL_0_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL,"GME_MIPI_DPHY_CONTROL_0_REG 0x%08x=0x%08x\n", GME_MIPI_DPHY_CONTROL_1_REG, GME_MIPI_DPHY_CONTROL_1_VAL);
   LOGG_PRINT(LOG_INFO_E, NULL,"GME_MIPI_DPHY_CONTROL_0_REG 0x%08x=0x%08x\n", GME_MIPI_DPHY_CONTROL_2_REG, GME_MIPI_DPHY_CONTROL_2_VAL);
}

#ifdef DEFSG_EXPERIMENTAL_BLANKING_SLEEP
void gme_enable_iae(void)
{
   GME_CLOCK_ENABLE_IAE_CLK_EN_W(1);
}

void gme_disable_iae(void)
{
   GME_CLOCK_ENABLE_IAE_CLK_EN_W(0);
}
void gme_enable_dpe(void)
{
   GME_CLOCK_ENABLE_DPE_CLK_EN_W(1);
}
void gme_disable_dpe(void)
{
   GME_CLOCK_ENABLE_DPE_CLK_EN_W(0);
}
UINT32 gme_get_sr6(void)
{
   return GME_SAVE_AND_RESTORE_6_VAL;
}
#endif
UINT32 GME_DRVG_getSr7(void)
{
   return GME_SAVE_AND_RESTORE_7_VAL;
}

/****************************************************************************
*
*  Function Name: GME_DRVG_dumpRegs
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
ERRG_codeE GME_DRVG_dumpRegs()
{
   UINT32 regsOffset[] = {gme_offset_tbl_values};
   UINT32 regsResetVal[] = {gme_regs_reset_val};
   UINT32 reg;

/*
   //to dump all the registers
   LOGG_PRINT(LOG_INFO_E, NULL, "GME registers (number of regs = %d):\n",sizeof(regsOffset));
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg+=4)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x, 0x%04x = 0x%08x\n"
                                    , regsOffset[reg],   *(volatile UINT32 *)(GME_BASE + regsOffset[reg])
                                    , regsOffset[reg+1], *(volatile UINT32 *)(GME_BASE + regsOffset[reg+1])
                                    , regsOffset[reg+2], *(volatile UINT32 *)(GME_BASE + regsOffset[reg+2])
                                    , regsOffset[reg+3], *(volatile UINT32 *)(GME_BASE + regsOffset[reg+3]));
   }
*/

   LOGG_PRINT(LOG_INFO_E, NULL, "Modified GME registers:\n");
   //compare against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(GME_BASE + regsOffset[reg]) != regsResetVal[reg])
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(GME_BASE + regsOffset[reg]));
      }
   }


   return IAE__RET_SUCCESS;
}

void GME_DRVG_shutterSeq()
{
   GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_ISP_E);
   GME_DRVG_resetUnit(GME_DRVG_HW_UNIT_IAE_E);

      // 1. I2C off:
      //printf("GME_DRVG_shutterSeq i2c off\n");
      // 1.1. change function of io1_1&io1_2 to be 1 (gpio)
      /*GME_IO_MUX_CTRL_1_IO_MUX_CTRL_1_W(1);
      GME_IO_MUX_CTRL_1_IO_MUX_CTRL_2_W(1);
      // 1.2. chnge GPIO4&5 to be output val=0
      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 4;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);
      gpioParams.val = 0;
      gpioParams.gpioNum = 4;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

      gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
      gpioDir.gpioNum = 5;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);
      gpioParams.val = 0;
      gpioParams.gpioNum = 5;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   */
      // 2. sensors ref clk disable by strap oe, change bit 0 to be 0
#if 0
      UINT32 oe = GME_IO_CONTROL_STRAP_OE_R;
      printf("oe reg val 0x%x\n",oe);
      oe &= 1;
      printf("oe reg val after change 0x%x\n",oe);
      GME_IO_CONTROL_STRAP_OE_W(oe);
#else
      /*printf("GME_DRVG_shutterSeq densors ref clock off\n");
      UINT32 eo = GME_IO_CONTROL_STRAP_OE_R;
         printf("before oe 0x%x\n");

      GME_IO_CONTROL_STRAP_OE_W(0);
      eo = GME_IO_CONTROL_STRAP_OE_R;
      printf("after oe 0x%x\n");*/
#endif
         /*
      printf("GME_DRVG_shutterSeq enter to hutter seq\n");
      GPIO_DRVG_gpioSetValParamsT         gpioParams;
      GPIO_DRVG_gpioSetDirParamsT         gpioDir;

      // 3. xshtdown to sensors
      printf("GME_DRVG_shutterSeq shutdoewn sensors\n");
      gpioParams.val = 0;
      gpioParams.gpioNum = 33;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);

      gpioParams.val = 0;
      gpioParams.gpioNum = 34;
      IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
   */

      return;

   return;
}

#if 0
void GME_DRVG_backFromShutterSeq()
{
   //GPIO_DRVG_gpioSetValParamsT         gpioParams;
   //GPIO_DRVG_gpioSetDirParamsT         gpioDir;

   // 1. I2C o:
   //printf("GME_DRVG_backFromShutterSeq start seq\n");
   // 1.1. change function of io1_1&io1_2 to be 0 (i2c)
   //printf("GME_DRVG_backFromShutterSeq i2c \n");
   /*GME_IO_MUX_CTRL_1_IO_MUX_CTRL_1_W(0);
   GME_IO_MUX_CTRL_1_IO_MUX_CTRL_2_W(0);

   gpioDir.direction = GPIO_DRVG_IN_DIRECTION_E;
   gpioDir.gpioNum = 4;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);

   gpioDir.direction = GPIO_DRVG_IN_DIRECTION_E;
   gpioDir.gpioNum = 5;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);
*/
   // 2. sensors ref clk enable by strap oe, change bit 0 to be 1
   /*GME_IO_CONTROL_STRAP_OE_W(1);
   UINT32 eo = GME_IO_CONTROL_STRAP_OE_R;
   printf("oe 0x%x\n");*/
   //OS_LYRG_usleep(3000000);
}
#endif

void GME_DRVG_iaeCoreReset()
{
   GME_SW_RESET_0_SW_IAE_CORE_RESET_N_W(1);
   GME_SW_RESET_0_SW_IAE_CORE_RESET_N_W(0);
   GME_SW_RESET_0_SW_IAE_CORE_RESET_N_W(1);
}
