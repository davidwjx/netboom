/*
 *
 * board.c
 *
 * c-implementation of chip initializations
 */

#include "common.h"
/* Inuitive drivers */
#include "gme.h"
#include "nu_regs.h"
#include "dft.h"
#include "flash_boot.h"
#include "storage.h"
#include "sim.h"
#include "verify.h"

/* Synopsis drivers */
#include "usb_drv.h"
#include "uart.h"
/* Protocol drivers */
#include "xmodem_boot.h"

/* Syscalls for Zorro USB3 driver */
#include "sys_calls.h"
#include "os_defs.h"

/**********************************************
 * local defines
***********************************************/


#ifndef CONFIG_BSS_INIT_OFF
/* clean .bss region */
#define BSS_INIT                        \
do{                                     \
   extern unsigned int __bss_start__;   \
   extern unsigned int __bss_end__;     \
   memset((void *)&__bss_start__, 0 , (unsigned int)&__bss_end__ - (unsigned int)&__bss_start__); \
   rel_log(">bss\n");\
}while(0)
#else
#define BSS_INIT
#endif

#define BR_STRINGIFY2(x) #x
#define BR_STRINGIFY(x) BR_STRINGIFY2(x)

/**********************************************
 * local typedefs
***********************************************/
typedef void (*void_func)(void);

/**********************************************
 * local data
***********************************************/
SECTION_PBSS static strap_info_t strap_info;
SECTION_PBSS dw_device_t *uart_boot_dev;

/**********************************************
 * pre-definition local functions
***********************************************/


/**********************************************
 * functions
***********************************************/

void bootrom_hw_verif_flags_set(void)
{   
#ifdef BUILD_FOR_HW_VERIF
   //For VCS HW simulation define parameters required by USB driver
   extern int build_for_hw_verif_scale_down;
   build_for_hw_verif_scale_down = 1;
#endif   
}
   
void bootrom_hie_regs_show(void)
{
   //For VCS HW simulation define parameters required by USB driver
#ifndef BUILD_FOR_HW_VERIF   
   #define HIE_BASE_ADDR                 (0x09000000)
   #define HIE_GCTL_REG_ADDR             (volatile UINT32 *)(HIE_BASE_ADDR+0xc110)
   #define HIE_GCTL_SCALE_DOWN_BITS      (0x00000030)
   #define HIE_GCTL_SCALE_DOWN_SHIFT     (4)

   //Print out the register values in release mode for verification in release mode 
   rel_log(">gctl=%d\n", (*(HIE_GCTL_REG_ADDR) & HIE_GCTL_SCALE_DOWN_BITS) >> HIE_GCTL_SCALE_DOWN_SHIFT);
#endif
}

void bootrom_usb_phy_init(void)
{
   /* write default values, overwrite if needed below */
   board_usb_phy_init();
#ifdef VCS
   board_usb_phy_dump_regs();
#endif
}

/* 
* This function is not supposed to be reached.
*/
static void bootrom_end()
{
   sim_init();
   board_end(0);
   while(1);
}

/**
 * Routine: board_basic_init
 *   Initialize clocks and update board_info structure.
 *
 */
void bootrom_basic_init(void)
{
   board_basic_init();

   // Select PLL mode according to boot straps
   if((strap_info.bootMode == BOARD_BOOT_MODE_USB) || (strap_info.bootMode == BOARD_BOOT_MODE_FLASH))
   {
      board_set_mode(BOARD_CLK_PLL);
      // Enable USB controller and init HIE divider before enabling PLL
      board_usb_init();
      // Set sys/cpu/lram frequency, according to the selected mode
      board_clks_set();
      rel_log(">plls\n");
   }
   else if((getEfuseSecureControlData()!=NOT_SECURED_IMAGE) && (strap_info.bootMode == BOARD_BOOT_MODE_UART))//when working in secure mode and boot from uart sys_clk & cpu_clk should be increased
   {
      board_set_mode(BOARD_CLK_PLL);
      // Set sys/cpu/lram frequency, according to the selected mode
      board_clks_set();
      rel_log(">plls\n");
   }
}

/**
 * Routine: bootrom_bg_usb
 *
 * Main background loop for USB
 *
 */
void bootrom_bg_usb(void)
{
   while (1)
   {
      nu3000_bg_process();
      asm volatile ("wfi" : : : "memory");
   }
}

/******************************************************************************
 * Routine: bootrom_usb_resume
 * Description: Called by start.s after ARM initializations for usb resume.
 *
 *****************************************************************************/
void bootrom_usb_resume(void) 
{
   GMEG_resetPowerState();

   board_clks_restore();

   if(strap_info.isUartDbg)
   {
      board_dbg_uart_init();
   }
   debug_init(strap_info.isUartDbg);

   rel_log("rs\n");

   //read SB data from EFUSE
   fillSBEfuseDB();

   board_timer_init();

   // Notify USB driver about resume event
   // This action required due to cancellation of USB WAKEP event processing in driver
   // USB_WAKEP events can be written to LRAM before AXI is ready
   dwc_usb3_common_wakeup();

   interrupt_init();
   enable_interrupts(); 

   nu3000_usb_irq_register(CONTROLLER_INTERRUPT);
   irq_unmask(CONTROLLER_INTERRUPT); 

   bootrom_bg_usb();

}


/**
 * Routine: board_init
 * Description:
 *    Caled by board_por(). Initializes according to boot mode -
 *    USB/UART and waits in background loop if needed.
 *
 **/
void bootrom_init(void)
{
   // Enable system timer
   board_timer_init();

   /* UART or USB decision */
   if(strap_info.bootMode == BOARD_BOOT_MODE_UART)
   {
      rel_log(">uart mode\n");
      // Enable BOOT UART controller
      board_boot_uart_init();
      // UART boot mode
      uart_boot_dev = uart_boot_init();

      rel_log(">rdy\n");
      xmodem_boot_init();
   }
   else if((strap_info.bootMode == BOARD_BOOT_MODE_USB) || (strap_info.bootMode == BOARD_BOOT_MODE_FLASH))
   {
      rel_log(">usb\n");

      /* set up and enable interrupts */
      interrupt_init();      
      enable_interrupts();

      /* Init usb phy */
      bootrom_usb_phy_init();

      /* Setup resume callback */
      GMEG_setResumeAddr((UINT32)bootrom_usb_resume);

      /* Setup global variables expected by Zoro */
      bootrom_hw_verif_flags_set();
      
      dwc_usb3_driver_init(CONTROLLER_BASE_ADDR, CONTROLLER_INTERRUPT); //  Init USB driver
      bootrom_hie_regs_show();
      irq_unmask(CONTROLLER_INTERRUPT); //Unmask Interrupt processing for USB interrupt
      rel_log(">rdy\n");
      bootrom_bg_usb();
   }

   // This function should not be reached
   rel_log("error: bootrom_end reached\n");
   bootrom_end(); //not supposed to get here
}


/******************************************************************************
 * Routine: bootrom_por
 * Description: Called by start.s after ARM initializations for power on reset.
 *
 *****************************************************************************/
 void bootrom_por(void)
 {
   if(GMEG_isTestMode())
   {
      run_test_mode();
   }

   board_bootstrap_get(&strap_info);
   
   if(strap_info.bootMode == BOARD_BOOT_MODE_JTAG)
   {
      while(1);
   }

   board_set_mode(BOARD_CLK_NOPLL);
   if(strap_info.isUartDbg)
   {
      board_dbg_uart_init();
   }
   debug_init(strap_info.isUartDbg);

   rel_log (">%s.%s.%s\n",BR_STRINGIFY(MAJOR_VERSION) ,BR_STRINGIFY(MINOR_VERSION), BR_STRINGIFY(BUILD_VERSION));

   //read SB data from EFUSE
   fillSBEfuseDB();

   storage_boot(&strap_info);

   // Run basic init before BSS
   bootrom_basic_init();

   /* Zero out BSS
    WARNING: Any static memory used before this point will be zeroed out unless
    included in the .pbss section in the linker script.
    */
   BSS_INIT;

   /* continue with main board_init phase */
   bootrom_init();
}

/**
 * Routine: get_board_info
 *    
 *    Get pointer to board_info structure
 
 *
 */
const strap_info_t *get_strap_info(void)
{
   return &strap_info;
}


