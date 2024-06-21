#include "pm.h"
#include "asm/system.h"
#include "board.h"
#include "gme.h"
#include "inu_storage_layout.h"
#include "inu_storage.h"


#ifdef BOOT_0
#define GPIO_EXT_PORTA            0x080B0050 //GPIO0
#define GPIO_BIT                  (1 << 26) // GPIO 26 (pin 26 in GPIO0)
#define GPIO_PD_DETECTED()        ((*((unsigned*)GPIO_EXT_PORTA) & GPIO_BIT) == 0) // when GPIO is low
#define IO_MUX_SELECT_GPIO()      GME_IO_MUX_CTRL_3_IO_MUX_CTRL_7_W(1)
#elif BOOT_87
#define GPIO_EXT_PORTA            0x080B8050 //GPIO1
#define GPIO_BIT                  (1 << 0)  // GPIO 32 (pin 0 in GPIO1)
#define IO_MUX_SELECT_GPIO()      GME_IO_MUX_CTRL_4_IO_MUX_CTRL_5_W(1)
#define GPIO_PD_DETECTED()        ((*((unsigned*)GPIO_EXT_PORTA) & GPIO_BIT) != 0)  // when GPIO is high
#else
#define GPIO_PD_DETECTED()        (FALSE) // no pd support
#define IO_MUX_SELECT_GPIO()
#endif

extern void spl_boot_cnn(void);
extern void invalidate_icache_all(void);
/* Globals */

/* Statics */

static UINT32 *pm_get_config_addr(void)
{
	return (UINT32 *) &__pm_config_address__;
}

static struct pm_configuration *pm_get_config(void)
{
	return (struct pm_configuration *) &__pm_config__;
}

static void pm_attach_config(void)
{
	UINT32 *config_address = pm_get_config_addr();

	/* Store vector */
	*config_address = (UINT32) pm_get_config();
}

static void cleanup_before_jump(void)
{
   /*
    * this function is called just before we call linux
    * it prepares the processor for linux
    *
    * we turn off caches etc ...
    */
   disable_interrupts();

   /*
    * Turn off I-cache and invalidate it
    */
   invalidate_icache_all();

   /*
    * Note: BootROM  runs without D-cache so no need to invalidate it here
    */
}

static int pd_detection(void)
{
	IO_MUX_SELECT_GPIO();


	if (GPIO_PD_DETECTED()) {
		pm_log("POWER_DOWN ON\n");
		return PM_POWERD_BOOT;
	}
	pm_log("POWER_DOWN OFF\n");
	return PM_NORMAL_BOOT;
}

static UINT32 get_ddr_pll_frequency(void)
{
	UINT32 freq = 1200;
#ifdef DDR_FULL_BUS_WIDTH_FREQ334_MODE
   freq = 334;
#elif DDR_FULL_BUS_WIDTH_FREQ533_MODE
   freq = 533;
#elif DDR_FULL_BUS_WIDTH_FREQ800_MODE
    freq = 800;
#elif DDR_FULL_BUS_WIDTH_FREQ1000_MODE
    freq = 1000;
#elif DDR_FULL_BUS_WIDTH_FREQ1067_MODE
    freq = 1067;
#elif DDR_FULL_BUS_WIDTH_FREQ1200_MODE
    freq = 1200;
#elif DDR_FULL_BUS_WIDTH_FREQ1334_MODE
    freq = 1334;
#elif DDR_FULL_BUS_WIDTH_FREQ1400_MODE
    freq = 1400;
#elif DDR_FULL_BUS_WIDTH_FREQ1600_MODE
    freq = 1600;
#endif
	return freq;
}

static void clks_restore(void)
{

	if (GME_SYS_PLL_STATUS_PLL_ON_R)
		GMEG_changeUnitFreq(GMEG_HW_UNIT_SYS_E);

	if (GME_CPU_PLL_STATUS_PLL_ON_R)
		GMEG_changeUnitFreq(GMEG_HW_UNIT_GPP_E);

	if (GME_DSP_PLL_STATUS_PLL_ON_R)
		GMEG_changeUnitFreq(GMEG_HW_UNIT_LRAM_E);

	if (GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 0) {
		/* Clock disabled  - turn on */
		GME_PERIPH_CLOCK_ENABLE_UART1_CLK_EN_W(1);
		/* Wait for uart status to show it is on */
		while (GME_PERIPH_CLOCK_ENABLE_STATUS_UART1_CLK_EN_R == 0);
	}

	GME_PERIPH_CLOCK_CONFIG_1_UART_1_CLK_DIV_W(GMEG_UART_CLK_SRC_OSC);
	GMEG_changeUnitFreq(GMEG_HW_UNIT_UART1_E);
	/* USB SYS clock enable */
	GME_CLOCK_ENABLE_USB_SYS_CLK_EN_W(1);
	while (GME_CLOCK_ENABLE_STATUS_USB_SYS_CLK_EN_R == 0);

	/* SPI  */
	GME_CLOCK_ENABLE_FCU_0_CLK_EN_W(1);
	while (GME_CLOCK_ENABLE_STATUS_FCU_0_CLK_EN_R == 0)	;

	/* GMEG_setFcuClockDiv(2); */
	GMEG_changeUnitFreq(GMEG_HW_UNIT_SPI_E);
}

static void tear_down(void)
{
#ifdef PM_DEBUG
	volatile unsigned *ptr = (volatile unsigned *) 0x2029014;
#endif
	unsigned gme_base_addr = GME_BASE;
	volatile unsigned *gme_control;
	int lram_wake_up_mode = (int) pm_get_config()->wakeup;
	unsigned USB_state;
	unsigned int count = 0;
#ifdef PM_DEBUG
	*ptr = 0x27;
#endif

	// Write 0 to PCTRL_n.port_en. Blocks AXI port(s) from taking more transactions.
	DDRC_MP_PCTRL_0_PORT_EN_W(0);
	DDRC_MP_PCTRL_1_PORT_EN_W(0);
	DDRC_MP_PCTRL_2_PORT_EN_W(0);
	DDRC_MP_PCTRL_3_PORT_EN_W(0);
	DDRC_MP_PCTRL_4_PORT_EN_W(0);
	DDRC_MP_PCTRL_5_PORT_EN_W(0);
	DDRC_MP_PCTRL_6_PORT_EN_W(0);
	DDRC_MP_PCTRL_7_PORT_EN_W(0);

	// Wait until all AXI ports are idle
	// a. Poll PSTAT.rd_port_busy_n=0
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_0_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_1_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_2_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_3_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_4_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_5_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_6_R)		;
	while (DDRC_MP_PSTAT_RD_PORT_BUSY_7_R)		;

	// b. Poll PSTAT.wr_port_busy_n=0.
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_0_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_1_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_2_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_3_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_4_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_5_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_6_R)		;
	while (DDRC_MP_PSTAT_WR_PORT_BUSY_7_R)		;

	// Write 1 to PWRCTL.selfref_sw. Causes system to move to self-refresh state
	M_RPT(2, NOP);

	DDRP_MASTER_DFIPHYUPD_DFIPHYUPDCNT_W(0x0); // disable generation of PHY-initiated update requests (dfi_phyupd_req) prior to remove DDRC core clock to avoids the possibility of missing dfi_phyupd_req assertions while clock has been removed
	DDRC_PWRCTL_SELFREF_SW_W(0x1);

	// Poll STAT.selfref_type= 2'b10. Wait until self-refresh state entered
	while (DDRC_STAT_SELFREF_TYPE_R != 0x2)		;

	M_RPT(10, NOP);

	GME_DDRM_CONTROL_HOLD_DDRP_RESET_W(1); // Register: ddrm_control, Field: hold_ddrp_reset: "release reset when this bit is set to 0"
	GME_DDRM_CONTROL_HOLD_DDRP_APB_RESET_W(1); // Register: ddrm_control, Field: hold_ddrp_apb_reset: "release reset when this bit is set to 0"
	GME_DDRM_CONTROL_HOLD_DDRC_RESET_W(0x1);        // assert ddrc reset
	GME_DDRM_CONTROL_HOLD_DDRC_APB_RESET_W(0x1);     // assert ddrc apb reset

	// Disable DDR clock
	GME_CLOCK_ENABLE_DDR_CLK_EN_W(0x0);
	while (GME_CLOCK_ENABLE_STATUS_DDR_CLK_EN_R);

	/*  gme_power_down_prep  */

	GME_DDRM_CONTROL_MASK_RESET_SUSPEND_ENTRY_W(1);
	gme_control = GME_CONTROL_REG;

	if (lram_wake_up_mode == PM_MODE0)
	{
		/* Wake up with timer */
		GME_SUSPEND_WAKEUP_TIMER_VAL = 0x06ffffff;
		GME_SUSPEND_WAKEUP_TIMER_VAL = 0x86ffffff;

	} else
	{
		GME_SUSPEND_WAKEUP_TIMER_VAL = 0x0fffffff;
	}
#ifdef PM_DEBUG
	*ptr = 0x28;
#endif

	/* Device Status Registe - CONNECTSPD */
	USB_state = (*(volatile unsigned *) (DEVICE_STATUS_REGISTER)) & 0x7;
	if (USB_state == 0x4) {
		*gme_control = 0x1004a6; // GME_CONTROL_START_POWER_CHANGE_W(0); //USB3.0
		*gme_control = 0x1004a7; // GME_CONTROL_START_POWER_CHANGE_W(1);
	} else {
		*gme_control = 0x100026; // GME_CONTROL_START_POWER_CHANGE_W(0); //USB2.0 high-speed
		*gme_control = 0x100027; // GME_CONTROL_START_POWER_CHANGE_W(1);
	}

	//GME_CLOCK_ENABLE_USB_SYS_CLK_EN_W(0);
	GME_CLOCK_ENABLE_CVA_CLK_EN_W(0);
	GME_CLOCK_ENABLE_EVP_CLK_EN_W(0); //BOARD_setEVPClk(0) uses spl log which can't be called during tearing down
	GME_CLOCK_ENABLE_GPIO_DB_CLK_EN_W(0);
	GME_CLOCK_ENABLE_FCU_1_CLK_EN_W(0);
	GME_CLOCK_ENABLE_I2C_4_CLK_EN_W(1);
	GME_CLOCK_ENABLE_DSPA_CLK_EN_W(0);    // reset EVP
	while (GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R);

	////////////// lower the core to 0.72V ////////////////
	(*(((volatile UINT32*) (gme_base_addr + 0xc8)))) = 1;  //power mode=1
	(*(((volatile UINT32*) (gme_base_addr + 0xc8)))) = 0;  //power mode=0

	count=0; while (count < 500) count++;
	*gme_control = 0x100427; //enable wakeup from USB
	count=0; while (count < 500) count++;

	asm volatile ("isb" : : : "memory");
	asm volatile ("dsb");
	while (1) {
		__asm__ __volatile__ ("wfi" : : : "memory");
	}
}

static void gme_power_up_cleanup(void)
{
#ifdef PM_DEBUG
	volatile unsigned *ptr = (volatile unsigned *) 0x2029020;
	*ptr = 0xDD000001;
#endif

	UINT32 hwBit = 0;
	UINT32 hwClearStatusBit = 0;
#ifdef PM_DEBUG
	*ptr = 0xDD000002;
#endif

	GME_PERIPH_CLOCK_ENABLE_UART0_CLK_EN_W(1);
	GME_PERIPH_CLOCK_ENABLE_UART1_CLK_EN_W(1);
#ifdef PM_DEBUG
	*ptr = 0xDD000003;
#endif

	// start UART1 again
	(*((volatile UINT32*) (0x81D0088))) = 0x6;  //sw reset
	M_RPT(50, NOP);M_RPT(50, NOP);
	(*((volatile UINT32*) (0x81D0010))) = 0xB;  //MCR
	M_RPT(50, NOP);M_RPT(50, NOP);
	(*((volatile UINT32*) (0x81D000c))) = 0x93; //LCR
	M_RPT(50, NOP);M_RPT(50, NOP);
	(*((volatile UINT32*) (0x81D0000))) = 0xd; //DLL
	M_RPT(50, NOP);M_RPT(50, NOP);
	(*((volatile UINT32*) (0x81D0004))) = 0x0; //DLH
	M_RPT(50, NOP);M_RPT(50, NOP);
	(*((volatile UINT32*) (0x81D000c))) = 0x13; //LCR
	M_RPT(50, NOP);M_RPT(50, NOP);
#ifdef PM_DEBUG
	*ptr = 0xDD000004;
#endif

	GME_CLOCK_ENABLE_CVA_CLK_EN_W(1);
	GME_CLOCK_ENABLE_GPIO_DB_CLK_EN_W(1);
	GME_CLOCK_ENABLE_FCU_1_CLK_EN_W(1);
	GME_CLOCK_ENABLE_I2C_0_CLK_EN_W(1);
	GME_CLOCK_ENABLE_I2C_1_CLK_EN_W(1);
	GME_CLOCK_ENABLE_I2C_2_CLK_EN_W(1);
	GME_CLOCK_ENABLE_I2C_3_CLK_EN_W(1);
	GME_CLOCK_ENABLE_I2C_4_CLK_EN_W(1);

	while (!GME_CLOCK_ENABLE_STATUS_I2C_4_CLK_EN_R)	;

	GME_FRQ_CHG_STATUS_CLEAR_VAL = hwClearStatusBit; //Clear status bit and freq change
	GME_FREQ_CHANGE_VAL &= ~hwBit;
	GME_CONTROL_START_POWER_CHANGE_W(1);  //Set the start power change bit

	spl_boot_cnn();

	/* Start */
	(*((volatile UINT32 *)((UINT32)(0x080F0000) + (0x0)))) = 0;
}

static void ddr_power_resume(void)
{
#ifdef PM_DEBUG
	volatile unsigned *ptr = (volatile unsigned *) 0x2029014;
	volatile unsigned *ptr1 = (volatile unsigned *) 0x2029024;
	*ptr1 = 0;
	*ptr = 0xAA000001;
#endif
	volatile unsigned *kernel_ptr = (volatile unsigned *) (PM_DDR_RESUME_IN_KERNEL);
	int freq;
	kernel_cpu_resume_arg_t kernel_cpu_resume;

#ifdef PM_DEBUG
	*ptr = 0xAA000002;
#endif

	/* Disable the start power change bit (just after resume) */
	GME_CONTROL_START_POWER_CHANGE_W(0);
	/* clear masking ddr reset in suspend mode (for ddr dlls power down) */
	GME_DDRM_CONTROL_MASK_RESET_SUSPEND_ENTRY_W(0);
	clks_restore();
	/* Enable DDR clock
	  assert ddrc apb reset */
	GME_DDRM_CONTROL_HOLD_DDRC_APB_RESET_W(0x1);
	/* assert ddrc reset */
	GME_DDRM_CONTROL_HOLD_DDRC_RESET_W(0x1);
	/* Register: ddrm_control, Field: hold_ddrp_reset: "release reset when this bit is set to 0" */
	GME_DDRM_CONTROL_HOLD_DDRP_RESET_W(1);
	/* Register: ddrm_control, Field: hold_ddrp_apb_reset: "release reset when this bit is set to 0" */
	GME_DDRM_CONTROL_HOLD_DDRP_APB_RESET_W(1);

	freq = get_ddr_pll_frequency();
	ddr_pll_config(freq);

	M_RPT(50, NOP);
	GME_CLOCK_ENABLE_DDR_CLK_EN_W(0x1);
	/* pool ddr clock enable status */
	while(!(GME_CLOCK_ENABLE_STATUS_DDR_CLK_EN_R));

#ifdef PM_DEBUG
	*ptr = 0xAA000003;
#endif
	ddrm_umctl_init_srpd_seq(freq);
#ifdef PM_DEBUG
	*ptr = 0xAA000007;
#endif

	GME_DDRM_CONTROL_MASK_RESET_SUSPEND_ENTRY_W(0);   // unmask ddrp reset during power-resume
	while(GME_DDRM_CONTROL_MASK_RESET_SUSPEND_ENTRY_R);
	M_RPT(50, NOP);
	gme_power_up_cleanup();
	kernel_cpu_resume = (kernel_cpu_resume_arg_t) (*kernel_ptr);
	cleanup_before_jump();
	kernel_cpu_resume();
}

void ddr_power_down_resume(void)
{
	volatile unsigned *kernel_ptr = (volatile unsigned *) (PM_DDR_RESUME_IN_KERNEL);
	int freq;
	kernel_cpu_resume_arg_t kernel_cpu_resume;

	freq = get_ddr_pll_frequency();
	ddr_pll_config(freq);

	GME_CLOCK_ENABLE_FCU_1_CLK_EN_W(1);
	while (GME_CLOCK_ENABLE_STATUS_FCU_1_CLK_EN_R == 0);

	ddrm_umctl_init_srpd_seq(freq);

	memcpy(LRAM_START_ADDRESS, DDR_CNN_LRAM_BCK_START_ADDRESS, DDR_CNN_LRAM_BCK_SIZE);
	spl_boot_cnn();

	kernel_cpu_resume = (kernel_cpu_resume_arg_t) (*kernel_ptr);
	cleanup_before_jump();
	kernel_cpu_resume();
}

int pm_boot_from_pd(void)
{
	return pm_get_config()->wakeup == PM_POWERD_BOOT;
}

void pm_init(void)
{
	struct pm_configuration *config;

	pm_attach_config();
	config = pm_get_config();

	config->tear_down = tear_down;
	config->bootrom_resume = ddr_power_resume;
	config->wakeup = (int) PM_NORMAL_BOOT;
	if (pd_detection() == PM_POWERD_BOOT)
		config->wakeup = (int) PM_POWERD_BOOT;
	config->status = PM_INITIALISED;
}

void spl_pm(void)
{
	UINT16 *training_data_ptr = (UINT16 *) &__pm_training_start__;
	UINT16 current_freq = get_ddr_pll_frequency();

	ddr_data_training_collection(training_data_ptr, &current_freq);

	/* Check if training should be flashed  */
	pm_get_config()->status =
			spl_training(training_data_ptr, current_freq);
}

