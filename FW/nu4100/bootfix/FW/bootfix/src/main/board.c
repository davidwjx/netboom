/*
 *
 * board.c
 *
 * c-implementation of chip initializations
 */

#include "common.h"
#include "nu_regs.h"
/* Inuitive drivers */
#include "gme.h"
#include "apb_timer.h"

/**********************************************
 * local defines
***********************************************/

#define BOARD_UART_BAUD_RATE_DEBUG     (115200)
#define BOARD_UART_BAUD_RATE_BOOT      (115200)

#define BOARD_BOOT_UART           (GMEG_HW_UNIT_UART0_E)
#define BOARD_DBG_UART            (GMEG_HW_UNIT_UART1_E)
#define BOARD_SPI                 (GMEG_HW_UNIT_SPI_E)

#ifdef NU3K_TEST
#define APB_CLK_DIV              (4)
#else
#define APB_CLK_DIV              (2)
#endif
#define SPI_FIXED_DIV            (2)

#define PLL_CLK_300M_HZ          (300000000)
#define PLL_CLK_396M_HZ          (396000000)
#define PLL_CLK_540M_HZ          (540000000)
#define PLL_CLK_600M_HZ          (600000000)
#define PLL_CLK_800M_HZ          (800000000)
#define OSC_CLK_HZ               (24000000)

#define PLL_LOCK_CNT_TIME        (0x960)//100 usec in 1/24MHz periods (41.67nsec) = 2400
#define PLL_LOCK_CNT_SELECT      (1) //enable lock count

#define USB_TCA_GCFG_ADDR          ((volatile UINT32 *)(GME_BASE + GME_USBP_TCA_GCFG_OFFSET))
#define USB_TCA_SYSMODE_CFG_ADDR   ((volatile UINT32 *)(GME_BASE + GME_USBP_TCA_SYSMODE_CFG_OFFSET))
#define USB_TCA_PSTATE_ADDR        ((volatile UINT32 *)(GME_BASE + GME_USBP_TCA_PSTATE_OFFSET))
#define USB_TCA_VBUS_CTRL_ADDR     ((volatile UINT32 *)(GME_BASE + GME_USBP_TCA_VBUS_CTRL_OFFSET))

#define USB_TCA_SET_BITS(reg,pos,len,value)  SetGroupBits32(reg,pos,len,value)
#define USB_TCA_GET_BITS(reg,pos,len)        GetGroupBits32(reg,pos,len)

//TCA GCFG
#define USB_TCA_GCFG_W(val)                        setTcaField(USB_TCA_GCFG_ADDR, 0, 32, val)
#define USB_TCA_GCFG_R                             getTcaField(USB_TCA_GCFG_ADDR,0,32)
#define USB_TCA_GCFG_OPMODE_W(val)                 setTcaField(USB_TCA_GCFG_ADDR, GME_USBP_TCA_GCFG_OP_MODE_POS, GME_USBP_TCA_GCFG_OP_MODE_LEN,val)
#define USB_TCA_GCFG_ROLE_HSTDEV_W(val)            setTcaField(USB_TCA_GCFG_ADDR, GME_USBP_TCA_GCFG_ROLE_HSTDEV_POS, GME_USBP_TCA_GCFG_ROLE_HSTDEV_LEN,val)
#define USB_TCA_GCFG_OPMODE_SET(reg, val)          USB_TCA_SET_BITS(reg,GME_USBP_TCA_GCFG_OP_MODE_POS,GME_USBP_TCA_GCFG_OP_MODE_LEN,val)
#define USB_TCA_GCFG_ROLE_HSTDEV_SET(reg, val)     USB_TCA_SET_BITS(reg,GME_USBP_TCA_GCFG_ROLE_HSTDEV_POS,GME_USBP_TCA_GCFG_ROLE_HSTDEV_LEN,val)

//TCA VBUS_CTRL
#define USB_TCA_VBUS_CTRL_W(val)                            setTcaField(USB_TCA_VBUS_CTRL_ADDR, 0,32,val)
#define USB_TCA_VBUS_CTRL_R                                 getTcaField(USB_TCA_VBUS_CTRL_ADDR,0,32)
#define USB_TCA_VBUS_CTRL_VBUSVALID_OVERRD_W(val)           setTcaField(USB_TCA_VBUS_CTRL_ADDR, GME_USBP_TCA_VBUS_CTRL_VBUSVALID_OVERRD_POS, GME_USBP_TCA_VBUS_CTRL_VBUSVALID_OVERRD_LEN,val)
#define USB_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_W(val)        setTcaField(USB_TCA_VBUS_CTRL_ADDR, GME_USBP_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_POS, GME_USBP_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_LEN,val)
#define USB_TCA_VBUS_CTRL_VBUSVALID_OVERRD_SET(reg,val)     USB_TCA_SET_BITS(reg, GME_USBP_TCA_VBUS_CTRL_VBUSVALID_OVERRD_POS, GME_USBP_TCA_VBUS_CTRL_VBUSVALID_OVERRD_LEN, val)
#define USB_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_SET(reg,val)  USB_TCA_SET_BITS(reg, GME_USBP_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_POS, GME_USBP_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_LEN, val)

//TCA PSTATE
#define USB_TCA_PSTATE_R                          getTcaField(USB_TCA_PSTATE_ADDR,0,32)
#define USB_TCA_PSTATE_PIPE0_POWERDOWN_R          getTcaField(USB_TCA_PSTATE_ADDR, GME_USBP_TCA_PSTATE_PIPE0_POWERDOWN_POS, GME_USBP_TCA_PSTATE_PIPE0_POWERDOWN_LEN)
#define USB_TCA_PSTATE_RX_PLL_STATE_R             getTcaField(USB_TCA_PSTATE_ADDR, GME_USBP_TCA_PSTATE_RX_PLL_STATE_POS, GME_USBP_TCA_PSTATE_RX_PLL_STATE_LEN)
#define USB_TCA_PSTATE_TX_STATE_R                 getTcaField(USB_TCA_PSTATE_ADDR, GME_USBP_TCA_PSTATE_TX_STATE_POS, GME_USBP_TCA_PSTATE_TX_STATE_LEN)
#define USB_TCA_PSTATE_TX_CM_STATE_R              getTcaField(USB_TCA_PSTATE_ADDR, GME_USBP_TCA_PSTATE_TX_CM_STATE_POS, GME_USBP_TCA_PSTATE_TX_CM_STATE_LEN)
#define USB_TCA_PSTATE_PIPE0_POWERDOWN_GET(reg)   USB_TCA_GET_BITS(reg, GME_USBP_TCA_PSTATE_PIPE0_POWERDOWN_POS, GME_USBP_TCA_PSTATE_PIPE0_POWERDOWN_LEN)
#define USB_TCA_PSTATE_RX_PLL_STATE_GET(reg)      USB_TCA_GET_BITS(reg, GME_USBP_TCA_PSTATE_RX_PLL_STATE_POS, GME_USBP_TCA_PSTATE_RX_PLL_STATE_LEN)
#define USB_TCA_PSTATE_TX_STATE_GET(reg)          USB_TCA_GET_BITS(reg, GME_USBP_TCA_PSTATE_TX_STATE_POS, GME_USBP_TCA_PSTATE_TX_STATE_LEN)
#define USB_TCA_PSTATE_TX_CM_STATE_GET(reg)       USB_TCA_GET_BITS(reg, GME_USBP_TCA_PSTATE_TX_CM_STATE_POS, GME_USBP_TCA_PSTATE_TX_CM_STATE_LEN)

//TCA SYSMODE
#define USB_TCA_SYSMODE_CFG_TYPEC_FLIP_W(val)     setTcaField(USB_TCA_SYSMODE_CFG_ADDR, GME_USBP_TCA_SYSMODE_CFG_TYPEC_FLIP_POS, GME_USBP_TCA_SYSMODE_CFG_TYPEC_FLIP_LEN,val)
#define USB_TCA_SYSMODE_CFG_TYPEC_DISABLE_W(val)  setTcaField(USB_TCA_SYSMODE_CFG_ADDR, GME_USBP_TCA_SYSMODE_CFG_TYPEC_DISABLE_POS, GME_USBP_TCA_SYSMODE_CFG_TYPEC_DISABLE_LEN,val)

/* This should be at least 4 clocks of the 24MHZ or 4*42 ns = 166 ns. */
#define TCA_ACCESS_DELAY_USEC (2)
#define TCA_CFGG_OPMODE_MANUAL        (0)
#define TCA_CFGG_ROLE_HSTDEV_DEVICE   (1)
#define TCA_VBUSVALID_OVERRD_FOLLOW_SYS_VBUSVALID (0x2)
#define TCA_VBUSVALID_OVERRD_DRIVE_0 (0x0)
#define TCA_POWERPRESENT_OVERRD_DRIVE_0  (0x0)
/**********************************************
 * local typedefs
***********************************************/

/**********************************************
 * local data
***********************************************/
SECTION_PBSS static int board_mode=0;

const bd_info_t board_modes[]=
{
   {
      .cpu_pll_freq_hz =PLL_CLK_800M_HZ,
      .cpu_pll_cfg.refdiv = 12,
      .cpu_pll_cfg.fbdiv = 800,
      .cpu_pll_cfg.postdiv1 = 2,
      .cpu_pll_cfg.postdiv2 = 1,
      .cpu_pll_cfg.dacpd = 0,
      .cpu_pll_cfg.dsmpd = 1,
      .cpu_pll_cfg.frac = 0,
      .cpu_pll_cfg.bypass = 0,
      .cpu_pll_cfg.on = 1,

      .sys_pll_freq_hz =PLL_CLK_300M_HZ,
      .sys_pll_cfg.refdiv = 12,
      .sys_pll_cfg.fbdiv = 300,
      .sys_pll_cfg.postdiv1 = 2,
      .sys_pll_cfg.postdiv2 = 1,
      .sys_pll_cfg.dacpd = 0,
      .sys_pll_cfg.dsmpd = 1,
      .sys_pll_cfg.frac = 0,
      .sys_pll_cfg.bypass = 0,
      .sys_pll_cfg.on = 1,

      .dsp_pll_freq_hz =PLL_CLK_600M_HZ,
      .dsp_pll_cfg.refdiv = 2,
      .dsp_pll_cfg.fbdiv = 100,
      .dsp_pll_cfg.postdiv1 = 2,
      .dsp_pll_cfg.postdiv2 = 1,
      .dsp_pll_cfg.dacpd = 0,
      .dsp_pll_cfg.dsmpd = 1,
      .dsp_pll_cfg.frac = 0,
      .dsp_pll_cfg.bypass = 0,
      .dsp_pll_cfg.on = 1,

      .sys_clk_div = 1,
      .sys_clk_freq_hz = PLL_CLK_540M_HZ,
      .cpu_clk_div = 1,
      .cpu_freq_hz = PLL_CLK_800M_HZ,
      .spi_clk_div = 2, //fractional divider
      .spi_freq_hz = PLL_CLK_540M_HZ / SPI_FIXED_DIV / 8,
      .apb_timer_freq_hz = OSC_CLK_HZ,
      .usb_sys_div = 4,

      .usb_sys_freq_hz = PLL_CLK_540M_HZ,

      .uart_dbg_freq_hz = OSC_CLK_HZ,
      .uart_boot_freq_hz = OSC_CLK_HZ,
      .uart_dbg_div = 1,
      .uart_boot_div = 1,
      .uart_dbg_baud_rate = BOARD_UART_BAUD_RATE_DEBUG,
      .uart_boot_baud_rate = BOARD_UART_BAUD_RATE_BOOT,
   },
   {
      .cpu_pll_freq_hz =0,
      .cpu_pll_cfg.on = 0,

      .sys_pll_freq_hz =0,
      .sys_pll_cfg.on = 0,

      .dsp_pll_freq_hz =0,
      .dsp_pll_cfg.on = 0,

      .sys_clk_div = 1,
      .sys_clk_freq_hz = OSC_CLK_HZ,
      .cpu_clk_div = 1,
      .cpu_freq_hz = OSC_CLK_HZ,
#ifdef NU3K_TEST
      .spi_clk_div = 1,
      .spi_freq_hz = OSC_CLK_HZ / APB_CLK_DIV / SPI_FIXED_DIV,
      .apb_timer_freq_hz = OSC_CLK_HZ / APB_CLK_DIV,
#else
      .spi_clk_div = 2,
      .spi_freq_hz = OSC_CLK_HZ / SPI_FIXED_DIV / 1,
      .apb_timer_freq_hz = OSC_CLK_HZ,
#endif
      .usb_sys_div = 1,
      .usb_sys_freq_hz = OSC_CLK_HZ,
      .uart_dbg_freq_hz = OSC_CLK_HZ,
      .uart_boot_freq_hz = OSC_CLK_HZ,
      .uart_dbg_div = 1,
      .uart_boot_div = 1,
      .uart_dbg_baud_rate = BOARD_UART_BAUD_RATE_DEBUG,
      .uart_boot_baud_rate = BOARD_UART_BAUD_RATE_BOOT,
   },
};

/**********************************************
 * pre-definition local functions
***********************************************/


/**********************************************
 * functions
***********************************************/
static void setTcaField(volatile UINT32 *addr,unsigned int pos, unsigned int width, UINT32 value)
{
   system_udelay(TCA_ACCESS_DELAY_USEC);
   if(width < 32)
   {
      SetGroupBits32(*addr,pos,width,value);
   }
   else
      *addr = value;
}

static UINT32 getTcaField(volatile UINT32 *addr,unsigned int pos, unsigned int width)
{
   system_udelay(TCA_ACCESS_DELAY_USEC);
   return ((width < 32) ? GetGroupBits32(*addr,pos,width) : *addr);
}
void board_dbg_uart_init()
{
   //const bd_info_t *bd_info = &board_modes[board_mode];
   int uart = BOARD_DBG_UART;

//   GMEG_setUartMux(uart);
   GMEG_enableClk(uart);
   GMEG_setUartClockSrc(uart, GMEG_UART_CLK_SRC_OSC);
 //  GMEG_setUartClockDiv(uart, bd_info->uart_dbg_div);
   GMEG_changeUnitFreq(uart);
}

void board_boot_uart_init()
{
   const bd_info_t *bd_info = &board_modes[board_mode];
   int uart = BOARD_BOOT_UART;

   GMEG_setUartMux(uart);
   GMEG_enableClk(uart);
   GMEG_setUartClockSrc(uart, GMEG_UART_CLK_SRC_OSC);
   GMEG_setUartClockDiv(uart, bd_info->uart_boot_div);
   GMEG_changeUnitFreq(uart);
}

void board_timer_init(void)
{
   GMEG_enableClk(GMEG_HW_UNIT_TIMERS_E);

   APB_TIMERG_init(APB_TIMERG_INST_0_E);
   APB_TIMERG_start();
}

void board_spi_init(void)
{
   const bd_info_t *bd_info = &board_modes[board_mode];

   GMEG_enableClk(BOARD_SPI);
   GMEG_setFcuClockDiv(bd_info->spi_clk_div);
   GMEG_changeUnitFreq(BOARD_SPI);
}

void board_usb_init()
{
   const bd_info_t *bd_info = &board_modes[board_mode];
   ///////////////////////////////////////////////////////
   //Note: Change HIE divider before starting the PLL
   ///////////////////////////////////////////////////////

   GMEG_enableClk(GMEG_HW_UNIT_USB_E);
   GMEG_setUsbClockDiv(bd_info->usb_sys_div);
   GMEG_changeUnitFreq(GMEG_HW_UNIT_USB_E);
}

void board_lram_clk_set()
{
   const bd_info_t *bd_info = &board_modes[board_mode];

   if (bd_info->dsp_pll_cfg.on)
   {
      GMEG_configDspPll(&bd_info->dsp_pll_cfg);
      GMEG_setLramClockSrc(GMEG_LRAM_CLK_SRC_PLL);
      GMEG_changeUnitFreq(GMEG_HW_UNIT_LRAM_E);
      GMEG_lockDspPll();
   }
}

/**
 * Routine: board_sys_clk_set
 * Sets sys clock and dividers according board info
 */
void board_sys_clk_set()
{
   const bd_info_t *bd_info;
   bd_info = &board_modes[board_mode];

   if (bd_info->sys_pll_cfg.on)
   {
      GMEG_configSysPll(&bd_info->sys_pll_cfg);
      GMEG_setSysClockSrc(GMEG_SYS_CLK_SRC_PLL);
      GMEG_setSysClockDiv(bd_info->sys_clk_div);
      GMEG_changeUnitFreq(GMEG_HW_UNIT_SYS_E);
      GMEG_lockSysPll();
  }

}

void board_gpp_clk_set()
{
   const bd_info_t *bd_info;
   bd_info = &board_modes[board_mode];
   if (bd_info->cpu_pll_cfg.on)
   {
      GMEG_configCpuPll(&bd_info->cpu_pll_cfg);
      GMEG_setGppClockSrc(GMEG_CPU_CLK_SRC_PLL);
      GMEG_setGppClockDiv(bd_info->cpu_clk_div);
      GMEG_changeUnitFreq(GMEG_HW_UNIT_GPP_E);
      GMEG_lockCpuPll();
   }
}

void board_clks_set()
{
  // board_gpp_clk_set();
 //  board_sys_clk_set();
 //  board_lram_clk_set();
}

void board_clks_restore()
{
   if (GMEG_isSysPllEnabled())
   {
      GMEG_lockSysPll();
      GMEG_changeUnitFreq(GMEG_HW_UNIT_SYS_E);
   }

   if (GMEG_isCpuPllEnabled())
   {
      GMEG_lockCpuPll();
      GMEG_changeUnitFreq(GMEG_HW_UNIT_GPP_E);
   }

   if (GMEG_isDspPllEnabled())
   {
      GMEG_lockDspPll();
      GMEG_changeUnitFreq(GMEG_HW_UNIT_LRAM_E);
   }

   //re-enable usb sys clock after other clocks for safety. No need for a frequency change.
   GMEG_enableClk(GMEG_HW_UNIT_USB_SYS_E);
}

/**
 * Routine: board_basic_init
 * Puts CPU in predefined default state
 */
void board_basic_init(void)
{
   APB_TIMERG_wdtDisable(); //Disable watchdog (safety)
   GMEG_disableSuspendTimer(); //Disable suspend timer (safety)
   //Set GME PLL lock indication to come from timer and not pll (for all PLLs)
   GMEG_configPllLockCnt(PLL_LOCK_CNT_SELECT, PLL_LOCK_CNT_TIME);
}

/**
 * Routine: board_usb_phy_init
 * Initializes USB PHY
 */
void board_usb_phy_init(void)
{
   GMEG_initUsbPhyParam();
   //GMEG_setUsbPhyReg(GMEG_USBPHY_LANE0_RX_OVRD_IN_HI_ADDR, GMEG_USBPHY_RX_OVRD_IN_HI_VAL);
#ifndef A0_TEST
   //GMEG_setUsbPhyReg(GMEG_USBPHY_LANE1_RX_OVRD_IN_HI_ADDR, GMEG_USBPHY_RX_OVRD_IN_HI_VAL);
#endif
}

void board_usb_typec_orientation(int invert)
{
   //assuming phy powered-up
   UINT32 reg;
   int idle = 0;

   //set opmode to manual
   reg = USB_TCA_GCFG_R;
   USB_TCA_GCFG_OPMODE_SET(reg, TCA_CFGG_OPMODE_MANUAL);
   USB_TCA_GCFG_ROLE_HSTDEV_SET(reg, TCA_CFGG_ROLE_HSTDEV_DEVICE);
   USB_TCA_GCFG_W(reg);

   //block vbus detection from controller
   reg = USB_TCA_VBUS_CTRL_R;
   USB_TCA_VBUS_CTRL_VBUSVALID_OVERRD_SET(reg, TCA_VBUSVALID_OVERRD_DRIVE_0);
   USB_TCA_VBUS_CTRL_POWERPRESENT_OVERRD_SET(reg, TCA_POWERPRESENT_OVERRD_DRIVE_0);
   USB_TCA_VBUS_CTRL_W(reg);

#define WAIT_FOR_TYPECE_STATE
#ifdef WAIT_FOR_TYPECE_STATE
   do
   {
      reg = USB_TCA_PSTATE_R;
      idle = (USB_TCA_PSTATE_PIPE0_POWERDOWN_GET(reg) == 0x3)  &&
           (USB_TCA_PSTATE_RX_PLL_STATE_GET(reg) == 0) &&
           (USB_TCA_PSTATE_TX_STATE_GET(reg) == 0) &&
           (USB_TCA_PSTATE_TX_CM_STATE_GET(reg) == 0);
   }while(!idle);
#endif
   //set lowpower operation for lane switch
   USB_TCA_SYSMODE_CFG_TYPEC_DISABLE_W(1);
#ifdef USB_TYPE2
   //lane orientation
   USB_TCA_SYSMODE_CFG_TYPEC_FLIP_W((invert) ? 0 : 1);
   rel_log("**********USB_TYPE2*************\n");
#else
   //lane orientation
   USB_TCA_SYSMODE_CFG_TYPEC_FLIP_W((invert) ? 1 : 0);
#endif
   //set normal operation
   USB_TCA_SYSMODE_CFG_TYPEC_DISABLE_W(0);

   //enable vbus detection at controller
   USB_TCA_VBUS_CTRL_VBUSVALID_OVERRD_W(TCA_VBUSVALID_OVERRD_FOLLOW_SYS_VBUSVALID);
}

/**
 * Routine: board_usb_phy_dump_regs
 * Dump USB PHY registers
 */
void board_usb_phy_dump_regs(void)
{
   rel_log("PHY_PARAM_0=%x\n", GMEG_readReg(GME_PHY_PARAM_0_OFFSET));
   rel_log("PHY_PARAM_1=%x\n", GMEG_readReg(GME_PHY_PARAM_1_OFFSET));
   rel_log("PHY_PARAM_2=%x\n", GMEG_readReg(GME_PHY_PARAM_2_OFFSET));
   rel_log("PHY_CLK=%x\n", GMEG_readReg(GME_PHY_CLOCK_OFFSET));
}

/**
 * Routine: board_bootstrap_get
 *   Read bootstrap values from HW or from efuse override.
 *
 */
void board_bootstrap_get(strap_info_t *strap_info)
{

#if defined(NU3K_TEST)
   //boot mode is NU3K_TEST value
   strap_info->isUartDbg = 1;
   strap_info->bootMode = (NU3K_TEST & 0x3);
   strap_info->typecInvert = (bootstraps & 0x100);

#else
   // Use HW bootstraps
   strap_info->bootMode = GMEG_getBootMode();
   strap_info->isUartDbg = (GMEG_isUartDebug()) ? TRUE : FALSE;
   strap_info->typecInvert = GMEG_isTypecInvert() ? TRUE : FALSE;
#endif
   strap_info->isUartDbg = 1;
}

void board_set_mode(unsigned int sys_clk_mode)
{
   // set system mode
   board_mode = sys_clk_mode;
}

/**
 * Routine: get_board_info
 *
 *    Get pointer to bd_info structure that contains all clk/div settings for the system
 *
 */
const bd_info_t *get_board_info(void)
{
   return &board_modes[board_mode];
}

