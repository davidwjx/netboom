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
//#include "storage.h"
#include "sim.h"

/* Synopsis drivers */
#include "usb_drv.h"
#include "uart.h"
/* Protocol drivers */
#include "xmodem_boot.h"

/* Syscalls for Zorro USB3 driver */
#include "sys_calls.h"
#include "os_defs.h"

#include "inu_storage_layout.h"
#include "inu_boot_common.h"
#include "storage.h"
#include "version.h"
#include "verify.h"
#include "i2c_drv.h"

#include "spi_flash.h"
InuBootfixHeaderT bootfixHeader;

/**********************************************
 * local defines
***********************************************/

//#define FPGA_BOARD
#ifndef CONFIG_BSS_INIT_OFF
/* clean .bss region */
#define BSS_INIT                        \
do{                                     \
	extern unsigned int __bss_start__;  \
	extern unsigned int __bss_end__;    \
	memset((void *)&__bss_start__, 0 , (unsigned int)&__bss_end__ - (unsigned int)&__bss_start__); \
	rel_log("bootfix: bss done\n");\
}while(0)
#else
#define BSS_INIT
#endif

#define BR_STRINGIFY2(x) #x
#define BR_STRINGIFY(x) BR_STRINGIFY2(x)

#define SB_HEADER_SIZE 264

/**********************************************
 * local typedefs
***********************************************/
typedef void (*void_func)(void);

/**********************************************
 * local data
***********************************************/
SECTION_PBSS static strap_info_t strap_info;
SECTION_PBSS dw_device_t *uart_boot_dev;

InuSectionHeaderT inuSectionHeaderT;
InuBootfixHeaderT bootfixMetadata;

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
void bootfix_usb_init(void)
{
	board_usb_init();
	board_usb_phy_init();
}

void SetGpioByVersion(void)
{

	 volatile unsigned int *gpioRegAddr,*gpioDdrRegAddr/*,*pinMuxReg*/;
	 unsigned int vers,count;
	 GME_IO_MUX_CTRL_4_IO_MUX_CTRL_6_W(1);
	 GME_IO_MUX_CTRL_4_IO_MUX_CTRL_7_W(1);

	for(count=0;count<1000;count++)
	{
		/* prevent compiler optimization */
		if(count > 800)
			count++;
	}


	gpioRegAddr = 0x80b8050;
	gpioDdrRegAddr = 0x80b8004;

	*gpioDdrRegAddr &= 0xfffffff9; // gpio 33 34
	vers = (*gpioRegAddr&0x6)>>1;
	if (vers != 1)
	{
		GME_IO_MUX_CTRL_3_IO_MUX_CTRL_3_W(1);
		GME_IO_MUX_CTRL_3_IO_MUX_CTRL_4_W(1);

		gpioRegAddr = 0x80b0000;
		gpioDdrRegAddr = 0x80b0004;

		*gpioDdrRegAddr &= 0xff3fffff;// gpio 22,23
		*gpioDdrRegAddr |= 0xc00000;
		*gpioRegAddr &= 0xff3fffff;
		*gpioRegAddr |= 0xc00000;
	 }
}

void bootfix_spi_init(void)
{
	SetGpioByVersion();
	board_spi_init();
	storage_init();
}



/**
 * Routine: board_basic_init
 *   Initialize clocks and update board_info structure.
 *
 */
void bootfix_basic_init(void)
{
	board_basic_init();

	// Select PLL mode according to boot straps
	board_set_mode(BOARD_CLK_PLL);
	// Set sys/cpu/lram frequency, according to the selected mode
#ifndef FPGA_BOARD
	board_clks_set();
#endif
 }
 /** Routine: bootfix_bg_usb
 *
 * Main background loop for USB
 *
 */

#define BOOTSPL_MODE_BOOTFIX 		(0x87654321)
#define BOOTSPL_CMD_SPL_INIT 		(0x100)
void bootfix_bg_usb(void)
{
	while (1)
	{

		nu3000_bg_process();
		asm volatile ("wfi" : : : "memory");
	}
}

void bootfix_usb_resume(void)
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

	bootfix_bg_usb();
}

unsigned char validateSecurityNumber(UINT32 sectionAddress, UINT32 sectionDataSize)
{
	unsigned char securityVersion = getEfuseSecurityNumber();
	unsigned char ret = 0;
	InuSecurityFooterT inuSecurityFooter = {0};

	memcpy(&inuSecurityFooter, (unsigned int*)(sectionAddress + sectionDataSize - SB_HEADER_SIZE - sizeof(InuSecurityFooterT)), sizeof(InuSecurityFooterT));

	if (securityVersion > inuSecurityFooter.securityVersion)
	{
		rel_log("bootfix: ERROR! Security Version in eFuse [%d] is larger than BootSPL's Security Version.\n", securityVersion, inuSecurityFooter.securityVersion);
		ret = 1;
	}
	rel_log("bootfix: fwVersion [%x] securityVersion = %d eFuse.securityVersion = %d.\n", inuSecurityFooter.fwVersion, inuSecurityFooter.securityVersion, securityVersion);

	return ret;
}


int bootfix_spi_boot()
{
	unsigned int sbOffset = 0;
	unsigned int boot_from_flash_status = storage_check_boot_from_flash();
	unsigned int i2c_pmic_val = 1;

#ifdef BOOT_87
	i2c_pmic_val = i2c_read_pmic();
	rel_log("bootfix:i2c_pmic_val %x storage_check_boot_from_flash: %x\n", i2c_pmic_val, boot_from_flash_status);
#endif
	if (boot_from_flash_status && i2c_pmic_val)
	{
		unsigned int sectionDataSize = 0;
		rel_log("bootfix: boot from flash is detected, booting...%x\n",BOOTSPL_LOAD_ADDRESS);
		/* basic init from bootspl */
		if (getEfuseSecureControlData() != NOT_SECURED_IMAGE)
		{
			sbOffset = SB_CHUNK_SIZE;
		}

		sectionDataSize = storage_get_section_data(INU_STORAGE_SECTION_BOOTSPL, (unsigned char *)(BOOTSPL_LOAD_ADDRESS + sbOffset));
		if (getEfuseSecureControlData() != NOT_SECURED_IMAGE)
		{
			SB_Result_t result = SB_SUCCESS;
			unsigned int ret   = 0;

			*GME_SAVE_AND_RESTORE_6_REG = BOOTSPL_LOAD_ADDRESS + sbOffset;
			result = secureBoot(BOOTSPL_LOAD_ADDRESS);
			ret = validateSecurityNumber(BOOTSPL_LOAD_ADDRESS, sectionDataSize);
			if ((result != SB_SUCCESS) || (ret == 1))
			{
				storage_update_flash_flags();
				system_hard_reset();
			}
		}
		system_jump_to_address((unsigned int)BOOTSPL_LOAD_ADDRESS);
	}
	return 0;
}


void bootfix_usb_boot(void)
{
	rel_log("bootfix: usb mode\n");


	/* set up and enable interrupts */
	interrupt_init();
	enable_interrupts();

	/* Setup resume callback */
	GMEG_setResumeAddr((UINT32)bootfix_usb_resume);

	dwc_usb3_driver_init(CONTROLLER_BASE_ADDR, CONTROLLER_INTERRUPT); //  Init USB driver
	rel_log("bootfix: usb drv init\n");

	rel_log("bootfix: int%d enabled\n", CONTROLLER_INTERRUPT);
	irq_unmask(CONTROLLER_INTERRUPT); //Unmask Interrupt processing for USB interrupt
// Work around need to be removed
	bootfix_spi_boot();
	bootfix_bg_usb();
}

/**
 * Routine: board_init
 * Description:
 *	 Caled by board_por(). Initializes according to boot mode -
 *	 USB/UART and waits in background loop if needed.
 *
 **/
void bootfix_init(void)
{

#ifndef FPGA_BOARD
	// Enable system timer
	board_timer_init();
#endif

	// boot from UART, if requested
	if(strap_info.bootMode == BOARD_BOOT_MODE_UART)
	{
		rel_log("bootfix: uart mode\n");
		// Enable BOOT UART controller
		board_boot_uart_init();
		// UART boot mode
		uart_boot_dev = uart_boot_init();
		rel_log("bootfix: uart init done\n");

	}

	// enable spi and read production data from spi flash
	bootfix_spi_init();

#ifndef FPGA_BOARD
	// enables dwc3 and phy clocks, required for Linux kernel driver
	bootfix_usb_init();
#endif
	// check for SPI boot , checks UART1_SOUT push button and bootfix_strap override in flash
	// Work around need to be added instead last call to bootfix_spi_boot
	// bootfix_spi_boot();
#ifndef FPGA_BOARD
	// if not UART and not SPI, continue to USB mode
	bootfix_usb_boot();
#endif
	// This function should not be reached
	abort_log("Warning: board_end reached\n");
}


/******************************************************************************
 * Routine: bootfix_por
 * Description: Called by start.s after ARM initializations for power on reset.
 *
 *****************************************************************************/
void bootfix_por(void)
{

	// Read HW bootstraps
	board_bootstrap_get(&strap_info);

	if (strap_info.bootMode == BOARD_BOOT_MODE_JTAG)
	{
		while (1);
	}

	board_set_mode(BOARD_CLK_NOPLL);
	if (strap_info.isUartDbg)
	{
		board_dbg_uart_init();
		debug_init(1);
	}
	else
	{
		/* Init debug functions for RELEASE mode */
		//debug_init(0);
	}
	board_boot_uart_init();
	uart_boot_dev = uart_boot_init();
    fillSBEfuseDB();

	/* Basic board initializations or resume from suspend */
	/* Suspend/Resume check is done in BootROM no need to do it here */
	rel_log("\nFW BOOTFIX(4100) version: %s.%s.%s.%s. Date: %s. Time: %s.\n", MAJOR_VERSION_STR, MINOR_VERSION_STR, BUILD_VERSION_STR, SUB_BUILD_VERSION_STR, __DATE__, __TIME__);
	bootfix_basic_init();

	/* Zero out BSS
	 WARNING: Any static memory used before this point will be zeroed out unless
	 included in the .pbss section in the linker script.
	 */
	BSS_INIT;
	/* continue with main board_init phase */
	bootfix_init();
}

/**
 * Routine: get_board_info
 *
 * Get pointer to board_info structure
 *
 */
const strap_info_t *get_strap_info(void)
{
	return &strap_info;
}


