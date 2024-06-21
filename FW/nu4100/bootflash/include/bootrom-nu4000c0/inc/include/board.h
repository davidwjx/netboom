/*
 * board.h
 *
 */
#ifndef _BOARD_H_
#define _BOARD_H_

#define BOARD_CLK_PLL           (0)
#define BOARD_CLK_NOPLL         (1)

#define BOARD_BOOT_MODE_USB      (0)
#define BOARD_BOOT_MODE_UART     (1)
#define BOARD_BOOT_MODE_FLASH    (2)
#define BOARD_BOOT_MODE_JTAG     (3)

typedef struct
{
   GMEG_pllConfigT cpu_pll_cfg;
   GMEG_pllConfigT sys_pll_cfg;
   GMEG_pllConfigT dsp_pll_cfg;
   unsigned int cpu_pll_freq_hz;
   unsigned int sys_pll_freq_hz;
   unsigned int dsp_pll_freq_hz;
   unsigned int sys_clk_div;
   unsigned int sys_clk_freq_hz;
   unsigned int cpu_clk_div;
   unsigned int cpu_freq_hz;
   unsigned int apb_timer_freq_hz;
   unsigned int spi_clk_div;
   unsigned int spi_freq_hz;
   unsigned int uart_dbg_div;
   unsigned int uart_dbg_freq_hz;
   unsigned int uart_dbg_baud_rate;
   unsigned int uart_boot_div;
   unsigned int uart_boot_freq_hz;
   unsigned int uart_boot_baud_rate;
   unsigned int usb_sys_div;
   unsigned int usb_sys_freq_hz;
} bd_info_t;

void board_dbg_uart_init();
void board_boot_uart_init();
void board_spi_init(void);
void board_timer_init(void);
void board_usb_init();
void board_usb_typec_orientation(int invert);
void board_clks_set();
void board_gpp_clk_set();
void board_sys_clk_set();
void board_lram_clk_set();
void board_clks_restore();
void board_bootstrap_get(strap_info_t *strap_info);
void board_basic_init(void);
void board_clk_pll_basic_init(void);
void board_usb_phy_init(void);
void board_usb_phy_dump_regs(void);
void board_bdinfo_init(void);
void board_set_mode(unsigned int sys_clk_mode);
const bd_info_t *get_board_info(void);
void board_pll_status_show();

#endif /* _BOARD_H_ */
