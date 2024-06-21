/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "common.h"

#include "gme.h" 
#include "nu_regs.h"

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
//TODO: move these defines to common header file
#define GMEP_SUCCESS  (0)
#define GMEP_FAIL     (1)

#define GMEP_POWER_MODE_SUSPEND (0x0)
#define GMEP_POWER_MODE_PSS (0x1)


#define GMEP_START_POWER_CHANGE  (0x1)
#define GMEP_STOP_POWER_CHANGE   (0x0)

#define GMEP_SUSPEND_TIMER_FREQ_MHZ   (24)

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
void GMEP_setPowerSuspend(void)
{
   // disable DDR reset before entering suspend (keeps ddr dlls in power down mode)
   //This bit should be set when power mode register is configured to trigger the new required power mode change
   GME_DDRM_CONTROL_MASK_RESET_SUSPEND_ENTRY_W(1);

   //Enable the start power change bit (can be done before or after the power change register)
   GME_CONTROL_START_POWER_CHANGE_W(GMEP_STOP_POWER_CHANGE);
   GME_CONTROL_START_POWER_CHANGE_W(GMEP_START_POWER_CHANGE);
   
   //Set power mode register 
   GME_POWER_MODE_VAL = GMEP_POWER_MODE_PSS;
   GME_POWER_MODE_VAL = GMEP_POWER_MODE_SUSPEND;
   
   //disable all interrupts in ICTL before doing WFI
   interrupt_init();
   //disable irq and fiq interrupts
   disable_interrupts();
 
   //disable USB system clock so no events will be received before sysem is ready on resume
   GMEG_disableClk(GMEG_HW_UNIT_USB_SYS_E);
 
   asm volatile ("isb" : : : "memory");
   while(1)
   {   
      asm volatile ("wfi" : : : "memory");
   }
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************/
int GMEG_isPowerOnReset(void)
{
   return (GME_GENERAL_STATUS_0_POR_OR_SUSPEND_FLAG_R == 0);
}

void GMEG_setSuspendMode(int mode)
{  
   if(mode == SYSTEM_POWER_SUSPEND_USB2)
   {
      GME_CONTROL_USB_MODE_W(0);
      GMEP_setPowerSuspend();
   }
   else
   {
      GME_CONTROL_USB_MODE_W(1);
      GMEP_setPowerSuspend();
   }
}

void GMEG_resetPowerState(void)
{
   //Disable the start power change bit (just after resume)
   GME_CONTROL_START_POWER_CHANGE_W(GMEP_STOP_POWER_CHANGE);

   // clear masking ddr reset in suspend mode (for ddr dlls power down)
   GME_DDRM_CONTROL_MASK_RESET_SUSPEND_ENTRY_W(0);
}

void GMEG_configSysPll(const GMEG_pllConfigT *pllP)
{
   //PLL calculation:
   // fvcoout = (fref/refdiv) * (fbdiv + (frac/2^24)))
   // foutpostdiv = fvcoout/(postdiv1*postdiv2)

   //Turn off/on
   if(pllP->on)
   {
     GME_SYS_PLL_CONTROL_BYPASS_W(pllP->bypass);
     GME_SYS_PLL_CONTROL_REFDIV_W(pllP->refdiv);
     GME_SYS_PLL_CONTROL_FBDIV_W(pllP->fbdiv);
     GME_SYS_PLL_CONTROL_POSTDIV1_W(pllP->postdiv1);
     GME_SYS_PLL_CONTROL_POSTDIV2_W(pllP->postdiv2);
     GME_SYS_PLL_CONTROL_DACPD_W(pllP->dacpd);
     GME_SYS_PLL_CONTROL_DSMPD_W(pllP->dsmpd);

     GME_SYS_PLL_CONTROL_PLL_ON_W(1);

   }
   else
   {
      //Turn pll off - leave configuration
      GME_SYS_PLL_CONTROL_PLL_ON_W(0);
   }
}

void GMEG_lockSysPll(void)
{
   while(!(GME_SYS_PLL_STATUS_LOCK_R));
}

void GMEG_getSysPllStatus(GMEG_pllStatusT *pllP)
{
   pllP->on = GME_SYS_PLL_STATUS_PLL_ON_R;
   pllP->bypass = GME_SYS_PLL_STATUS_BYPASS_R;
   pllP->refdiv = GME_SYS_PLL_STATUS_REFDIV_R;
   pllP->fbdiv = GME_SYS_PLL_STATUS_FBDIV_R;
   pllP->postdiv1 = GME_SYS_PLL_STATUS_POSTDIV1_R;
   pllP->postdiv2 = GME_SYS_PLL_STATUS_POSTDIV2_R;
   pllP->lock = GME_SYS_PLL_STATUS_LOCK_R;
}

void GMEG_configCpuPll(const GMEG_pllConfigT *pllP)
{
   //Turn off/on
   if(pllP->on)
   {
     GME_CPU_PLL_CONTROL_BYPASS_W(pllP->bypass);
     GME_CPU_PLL_CONTROL_REFDIV_W(pllP->refdiv);
     GME_CPU_PLL_CONTROL_FBDIV_W(pllP->fbdiv);
     GME_CPU_PLL_CONTROL_POSTDIV1_W(pllP->postdiv1);
     GME_CPU_PLL_CONTROL_POSTDIV2_W(pllP->postdiv2);
     GME_CPU_PLL_CONTROL_DACPD_W(pllP->dacpd);
     GME_CPU_PLL_CONTROL_DSMPD_W(pllP->dsmpd);

     GME_CPU_PLL_CONTROL_PLL_ON_W(1);
   }
   else
   {
      //Turn pll off - leave configuration
      GME_CPU_PLL_CONTROL_PLL_ON_W(0);
   }
}

void GMEG_lockCpuPll(void)
{
   while(!(GME_CPU_PLL_STATUS_LOCK_R));
}

void GMEG_getCpuPllStatus(GMEG_pllStatusT *pllP)
{
   pllP->on = GME_CPU_PLL_STATUS_PLL_ON_R;
   pllP->bypass = GME_CPU_PLL_STATUS_BYPASS_R;
   pllP->refdiv = GME_CPU_PLL_STATUS_REFDIV_R;
   pllP->fbdiv = GME_CPU_PLL_STATUS_FBDIV_R;
   pllP->postdiv1 = GME_CPU_PLL_STATUS_POSTDIV1_R;
   pllP->postdiv2 = GME_CPU_PLL_STATUS_POSTDIV2_R;
   pllP->lock = GME_CPU_PLL_STATUS_LOCK_R;
}

void GMEG_configDspPll(const GMEG_pllConfigT *pllP)
{
   //Turn off/on
   if(pllP->on)
   {
     GME_DSP_PLL_CONTROL_BYPASS_W(pllP->bypass);
     GME_DSP_PLL_CONTROL_REFDIV_W(pllP->refdiv);
     GME_DSP_PLL_CONTROL_FBDIV_W(pllP->fbdiv);
     GME_DSP_PLL_CONTROL_POSTDIV1_W(pllP->postdiv1);
     GME_DSP_PLL_CONTROL_POSTDIV2_W(pllP->postdiv2);
     GME_DSP_PLL_CONTROL_DACPD_W(pllP->dacpd);
     GME_DSP_PLL_CONTROL_DSMPD_W(pllP->dsmpd);

     GME_DSP_PLL_CONTROL_PLL_ON_W(1);
   }
   else
   {
      //Turn pll off - leave configuration
      GME_DSP_PLL_CONTROL_PLL_ON_W(0);
   }
}

void GMEG_lockDspPll(void)
{
   while(!(GME_DSP_PLL_STATUS_LOCK_R));
}

void GMEG_getDspPllStatus(GMEG_pllStatusT *pllP)
{
   pllP->on = GME_DSP_PLL_STATUS_PLL_ON_R;
   pllP->bypass = GME_DSP_PLL_STATUS_BYPASS_R;
   pllP->refdiv = GME_DSP_PLL_STATUS_REFDIV_R;
   pllP->fbdiv = GME_DSP_PLL_STATUS_FBDIV_R;
   pllP->postdiv1 = GME_DSP_PLL_STATUS_POSTDIV1_R;
   pllP->postdiv2 = GME_DSP_PLL_STATUS_POSTDIV2_R;
   pllP->lock = GME_DSP_PLL_STATUS_LOCK_R;
}

void GMEG_configPllLockCnt(unsigned int select, unsigned int lockTime)
{
   //select=1 selects the counter, 0 selects the PLL output
   //lockCnt = in 1/24MHz units (if enabled)
   if(select)
   {
      GME_PLL_LOCK_COUNT_LOCK_TIME_W(lockTime);
   }
   GME_PLL_LOCK_COUNT_LOCK_SELECT_W(select);
}

int GMEG_isSysPllEnabled()
{
   return GME_SYS_PLL_STATUS_PLL_ON_R;
}

int GMEG_isCpuPllEnabled()
{
   return GME_CPU_PLL_STATUS_PLL_ON_R;
}

int GMEG_isDspPllEnabled()
{
   return GME_DSP_PLL_STATUS_PLL_ON_R;
}

int GMEG_isHieEnabled()
{
   return GME_CLOCK_ENABLE_USB_SYS_CLK_EN_R;
}

void GMEG_setGppClockSrc(UINT16 clkSrc)
{
   if(clkSrc == GMEG_CPU_CLK_SRC_OSC)
   {
      GME_GPP_CLOCK_CONFIG_CLK_SRC_W(0);
   }
   else
   {
      GME_GPP_CLOCK_CONFIG_CLK_SRC_W(1);
   }
}

void GMEG_setGppClockDiv(UINT16 div)
{
   GME_GPP_CLOCK_CONFIG_GPP_CLK_DIV_W(div);
}

void GMEG_setSysClockSrc(UINT16 clkSrc)
{
   if(clkSrc == GMEG_SYS_CLK_SRC_OSC)
   {
      GME_SYS_CLOCK_CONFIG_CLK_SRC_W(0);
   }
   else
   {
      GME_SYS_CLOCK_CONFIG_CLK_SRC_W(1);
   }
}

void GMEG_setSysClockDiv(UINT16 div)
{
   GME_SYS_CLOCK_CONFIG_SYS_CLK_DIV_W(div);
}

void GMEG_setLramClockSrc(UINT16 clkSrc)
{
   if(clkSrc == GMEG_LRAM_CLK_SRC_OSC)
   {
      GME_DSP_CLOCK_CONFIG_CLK_SRC_W(0);
   }
   else
   {
      GME_DSP_CLOCK_CONFIG_CLK_SRC_W(1);
   }
}

void GMEG_setUsbClockDiv(UINT16 div)
{
   GME_SYS_CLOCK_CONFIG_USB_SYS_CLK_DIV_W(div)
}

void GMEG_setUartClockDiv(GMEG_hwUnitE uart, UINT16 div)
{
   if(uart == GMEG_HW_UNIT_UART0_E)
   {
      GME_PERIPH_CLOCK_CONFIG_UART_0_CLK_DIV_W(div);
   }
   else if(uart == GMEG_HW_UNIT_UART1_E)
   {
      GME_PERIPH_CLOCK_CONFIG_1_UART_1_CLK_DIV_W(div);
   }
}

void GMEG_setUartClockSrc(GMEG_hwUnitE uart, UINT16 src)
{
   if(uart == GMEG_HW_UNIT_UART0_E)
   {
      GME_PERIPH_CLOCK_CONFIG_UART_0_CLK_SRC_W(src);
   }
   else if(uart == GMEG_HW_UNIT_UART1_E)
   {
      GME_PERIPH_CLOCK_CONFIG_1_UART_1_CLK_DIV_W(src);
   }
}

void GMEG_setFcuClockDiv(UINT16 div)
{
   // 4 bit field. 
   div <<= 1; //Note: 3 Msbits are integer. Lsbit is fraction 0/0.5.
   GME_SYS_CLOCK_CONFIG_FCU_CLK_DIV_W(div);
}

void GMEG_changeUnitFreq(GMEG_hwUnitE unit)
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
         hwBit = (1<<GME_FREQ_CHANGE_GPP_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_GPP_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_CLEAR_GPP_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_SYS_E):
      {
         //This function does a fast-freq change - without relocking the sys PLL.
         //Assumption: SYS PLL is locked, GPP clock config is set, SYS clock config is set      
         hwBit = (1<<GME_FREQ_CHANGE_SYS_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_SYS_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_SYS_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_LRAM_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_DSP_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_DSP_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_DSP_FREQ_CHG_DONE_POS);
      }
      break;

      case(GMEG_HW_UNIT_SPI_E):
      {
         hwBit = (1<<GME_FREQ_CHANGE_FCU_GO_BIT_POS);
         hwStatusBit = (1<<GME_FRQ_CHG_STATUS_FCU_FREQ_CHG_DONE_POS);
         hwClearStatusBit = (1<<GME_FRQ_CHG_STATUS_FCU_FREQ_CHG_DONE_POS);
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

int GMEG_disableClk(GMEG_hwUnitE unit)
{
   int ret = GMEP_SUCCESS;
   
   switch(unit)
   {
      case(GMEG_HW_UNIT_TIMERS_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_TIMERS_CLK_EN_R == 1)
         {
            //Clock enabled- turn off
            GME_PERIPH_CLOCK_ENABLE_TIMERS_CLK_EN_W(0);
            //Wait for timer status to show it is off
            while(GME_PERIPH_CLOCK_ENABLE_STATUS_TIMERS_CLK_EN_R == 1);
         }
         break;
      }
      case(GMEG_HW_UNIT_UART0_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_PERIPH_CLOCK_ENABLE_UART0_CLK_EN_W(0);
            //Wait for uart status to show it is off
            while(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 1);
         }
         break;
      }
      case(GMEG_HW_UNIT_UART1_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 1)
         {
           //Clock enabled- turn off
            GME_PERIPH_CLOCK_ENABLE_UART1_CLK_EN_W(0);
            //Wait for uart status to show it is off
            while(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 1);
         }
         break;
      }
     
      case(GMEG_HW_UNIT_USB_E):
      {
         //controller (hie) clock disable
         if(GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 1)
         {
            GME_CLOCK_ENABLE_USB_SYS_CLK_EN_W(0);
            while(GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 1);
         }

         //tca apb clock disable
         GME_PERIPH_CLOCK_ENABLE_USBP_APB_CLK_EN_W(0);
         while(GME_PERIPH_CLOCK_ENABLE_STATUS_USBP_APB_CLK_EN_R == 1);

         //phy clock disable
         GME_PERIPH_CLOCK_ENABLE_OSC_USB_PHY_CLK_EN_W(0);
         while(GME_PERIPH_CLOCK_ENABLE_STATUS_OSC_USB_PHY_CLK_EN_R == 1);

         break;
      }

      case(GMEG_HW_UNIT_USB_SYS_E):
      {
         GME_CLOCK_ENABLE_USB_SYS_CLK_EN_W(0);
         while(GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 1);
         break;
      }
      
      default:
      {
         ret = GMEP_FAIL;
      }
      break;
   }
   return ret; 
}

int GMEG_enableClk(GMEG_hwUnitE unit)
{
   int ret = GMEP_SUCCESS;
   
   switch(unit)
   {
      case(GMEG_HW_UNIT_TIMERS_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_TIMERS_CLK_EN_R == 0)
         {
            //Clock disabled  - turn on
            GME_PERIPH_CLOCK_ENABLE_TIMERS_CLK_EN_W(1);
            //Wait for timer status to show it is on
            while(GME_PERIPH_CLOCK_ENABLE_STATUS_TIMERS_CLK_EN_R == 0);
         }
         break;
      }
      case(GMEG_HW_UNIT_UART0_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 0)
         {
            //Clock disabled  - turn on
            GME_PERIPH_CLOCK_ENABLE_UART0_CLK_EN_W(1);
            //Wait for uart status to show it is on
            while(GME_PERIPH_CLOCK_ENABLE_STATUS_UART0_CLK_EN_R == 0);
         }
         break;
      }
      
      case(GMEG_HW_UNIT_UART1_E):
      {
         if(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 0)
         {
            //Clock disabled  - turn on
            GME_PERIPH_CLOCK_ENABLE_UART1_CLK_EN_W(1);
            //Wait for uart status to show it is on
            while(GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 0);
         }
         break;
      }
     
      case(GMEG_HW_UNIT_USB_E):
      {
         //phy clock enable
         GME_PERIPH_CLOCK_ENABLE_OSC_USB_PHY_CLK_EN_W(1);
         while(GME_PERIPH_CLOCK_ENABLE_STATUS_OSC_USB_PHY_CLK_EN_R == 0);

         //tca apb clock enable
         GME_PERIPH_CLOCK_ENABLE_USBP_APB_CLK_EN_W(1);
         while(GME_PERIPH_CLOCK_ENABLE_STATUS_USBP_APB_CLK_EN_R == 0);

         //controller (hie) clock enable
         if(GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 0)
         {
            GME_CLOCK_ENABLE_USB_SYS_CLK_EN_W(1);
            while(GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 0);
         }
         break;
      }

      case(GMEG_HW_UNIT_USB_SYS_E):
      {
         GME_CLOCK_ENABLE_USB_SYS_CLK_EN_W(1);
         while(GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 0);
         break;
      }

      case(GMEG_HW_UNIT_SPI_E):
      {
         GME_CLOCK_ENABLE_FCU_0_CLK_EN_W(1);
         while(GME_CLOCK_ENABLE_STATUS_FCU_0_CLK_EN_R == 0);
         break;
      }
      default:
      {
         ret = GMEP_FAIL;
      }
      break;
   }
   return ret;
}


//Version and ID read
UINT32 GMEG_getVersion(void)
{
   return GME_NU4000_VERSION_VAL;
}

UINT32 GMEG_getResumeAddr(void)
{
   return GME_SAVE_AND_RESTORE_0_VAL;
}

void GMEG_setResumeAddr(UINT32 addr)
{
   GME_SAVE_AND_RESTORE_0_VAL = addr;
}

int GMEG_getBootMode()
{
   return GME_STRAP_PIN_STATUS_BOOT_R;
}
int GMEG_isUartDebug(void)
{
   return (GME_STRAP_PIN_STATUS_DISABLE_UART1_LOG_R == 0);
}
int GMEG_isTypecInvert(void)
{
#ifdef B0_TEST
   return (GME_STRAP_PIN_STATUS_RESERVED_1_R);
#else
   return (GME_STRAP_PIN_STATUS_TYPE_C_SELECT_R);
#endif
}

int GMEG_isTestMode(void)
{
   return (GME_TEST_STATUS_TEST_EN_R == 1)? TRUE: FALSE;
}

//IO Mux
void GMEG_setUartMux(GMEG_hwUnitE unit)
{
}


//General-purpose read/write GME register
void GMEG_writeReg(UINT32 addrOffset, UINT32 val)
{
   *(((volatile UINT32 *)(GME_BASE + addrOffset))) = val;
}

UINT32 GMEG_readReg(UINT32 addrOffset)
{
   return *(((volatile UINT32 *)(GME_BASE + addrOffset)));
}


//SaveAndRestore read/write GME register
void GMEG_writeSaveAndRestoreReg(UINT32 id, UINT32 val)
{
   *(((volatile UINT32 *)(GME_BASE + GME_SAVE_AND_RESTORE_0_OFFSET + 4*id))) = val;
}

UINT32 GMEG_readSaveAndRestoreReg(UINT32 id)
{
   return *(((volatile UINT32 *)(GME_BASE + GME_SAVE_AND_RESTORE_0_OFFSET + 4*id)));
}

void GMEG_disableSuspendTimer(void)
{
   //Disable suspend timer 
   GME_SUSPEND_WAKEUP_TIMER_COUNT_ENABLE_W(0);
}


void GMEG_setDoSrcChgOnSuspend(BOOL doChange)
{
   int val = (doChange == TRUE) ? 1 : 0;
   GME_CONTROL_GPP_SRC_CHG_ON_SUSPEND_W(val)
}

void GMEG_phyBridgeWaitForAckFall( void )
{
   while( GME_PHY_BRIDGE_1_CR_ACK_R == 1 ) {}
}

void GMEG_phyBridgeWaitForAckRise( void )
{
   while (GME_PHY_BRIDGE_1_CR_ACK_R == 0) {}
}

void GMEG_phyBridgeCapAddr(UINT16 phyRegAddr)
{
   //phy reg address
   GME_PHY_BRIDGE_0_CR_DATA_IN_W(phyRegAddr);

   //cap address
   GME_PHY_BRIDGE_0_CR_CAP_ADDR_W(0x1);
 
   //wait for ACK
   GMEG_phyBridgeWaitForAckRise();     

   //reset cap address
   GME_PHY_BRIDGE_0_CR_CAP_ADDR_W(0);
   
   //wait for ACK to reset
   GMEG_phyBridgeWaitForAckFall();
}

void GMEG_setUsbPhyReg( UINT16 regAddr, UINT16 value )
{
   GME_PHY_BRIDGE_0_VAL = 0;
   GME_PHY_BRIDGE_1_VAL = 0;

   GMEG_phyBridgeCapAddr(regAddr);
   
   //data to write
   GME_PHY_BRIDGE_0_CR_DATA_IN_W(value);

   //capture data
   GME_PHY_BRIDGE_0_CR_CAP_DATA_W(0x1);

   //wait for ACK
   GMEG_phyBridgeWaitForAckRise();

   //reset capture data
   GME_PHY_BRIDGE_0_CR_CAP_DATA_W(0);

   //wait for ACK to reset
   GMEG_phyBridgeWaitForAckFall();

   //write captured data to reg
   GME_PHY_BRIDGE_0_CR_WRITE_W(0x1);

   //wait for ACK
   GMEG_phyBridgeWaitForAckRise();

   //reset write captured data
   GME_PHY_BRIDGE_0_CR_WRITE_W(0);

   //wait for ACK to reset
   GMEG_phyBridgeWaitForAckFall();

}

void GMEG_initUsbPhyParam(void)
{
   GME_PHY_PARAM_1_RETENABLEN_W(1);  
   GME_PHY_PARAM_1_REF_SSP_EN_W(1);
}

