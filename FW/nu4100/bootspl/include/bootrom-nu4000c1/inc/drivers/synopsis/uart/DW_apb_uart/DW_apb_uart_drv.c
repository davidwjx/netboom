/* --------------------------------------------------------------------
** 
** Synopsys DesignWare DW_apb_uart Software Driver Kit and
** documentation (hereinafter, "Software") is an Unsupported
** proprietary work of Synopsys, Inc. unless otherwise expressly
** agreed to in writing between Synopsys and you.
** 
** The Software IS NOT an item of Licensed Software or Licensed
** Product under any End User Software License Agreement or Agreement
** for Licensed Product with Synopsys or any supplement thereto. You
** are permitted to use and redistribute this Software in source and
** binary forms, with or without modification, provided that
** redistributions of source code must retain this notice. You may not
** view, use, disclose, copy or distribute this file or any information
** contained herein except pursuant to this license grant from Synopsys.
** If you do not agree with this notice, including the disclaimer
** below, then you are not authorized to use the Software.
** 
** THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS"
** BASIS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
** FOR A PARTICULAR PURPOSE ARE HEREBY DISCLAIMED. IN NO EVENT SHALL
** SYNOPSYS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
** OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
** USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
** DAMAGE.
** 
** --------------------------------------------------------------------
*/

#include <string.h>
#include <stdio.h>
#include "common.h"
#include "gme.h"
#include "uart.h"
#include "DW_common.h"
#include "DW_apb_uart_public.h"
#include "DW_apb_uart_private.h"
#include "DW_apb_uart_defs.h"

// This definition is used by the assetion macros to determine the
// current file name.  It is defined in the DW_common_dbc.h header.
DW_DEFINE_THIS_FILE;

#define RXBUFLEN        40
#define UART_SCLCK    	24000000
#define UART_BAUDRATE   115200

// -------
// Target specific PRINTF's
// -------


#define PRINTF(...)

/* Do not zero uninitialized variables for UART */
SECTION_PBSS struct dw_device uart1;
SECTION_PBSS struct dw_device uart2;
SECTION_PBSS struct dw_uart_param uart1_param;
SECTION_PBSS struct dw_uart_param uart2_param;
SECTION_PBSS struct dw_uart_instance uart1_instance;
SECTION_PBSS struct dw_uart_instance uart2_instance;


////
// function prototypes
////

static void setupDeviceStructuresUART1(void)
{   
   // UART 1 device structure setup.
   uart1.name           = "uart1";
   uart1.comp_type      = Dw_apb_uart;
   uart1.data_width     = i_uart1_CC_UART_APB_DATA_WIDTH;
   uart1.base_address   = (void *) i_uart1_DW_APB_UART_BASE;
   uart1.comp_param     = &uart1_param;
   uart1.instance       = &uart1_instance;
   uart1.os             = NULL;
      
   uart1_param.afce_mode = i_uart1_CC_UART_AFCE_MODE;         // automatic flow control
   uart1_param.dma_extra = i_uart1_CC_UART_DMA_EXTRA;         // DW_ahb_dmac compatibility signals
   uart1_param.fifo_access = i_uart1_CC_UART_FIFO_ACCESS;       // programmable FIFO access mode
   uart1_param.fifo_stat = i_uart1_CC_UART_FIFO_STAT;         // additional FIFO status registers
   uart1_param.new_feat = i_uart1_CC_UART_ADDITIONAL_FEATURES;          // new v3 features (shadow registers, etc.)
   uart1_param.shadow = i_uart1_CC_UART_SHADOW;            // include shadow registers
   uart1_param.sir_lp_mode = i_uart1_CC_UART_SIR_LP_MODE;       // low-power IrDA SIR mode
   uart1_param.sir_mode = i_uart1_CC_UART_SIR_MODE;          // serial infrared
   uart1_param.thre_mode = i_uart1_CC_UART_THRE_MODE;         // programmable thre interrupts
   uart1_param.fifo_mode = i_uart1_CC_UART_FIFO_MODE;      // FIFO depth
}

static void setupDeviceStructuresUART2(void)
{
   // UART 2 device structure setup.
  uart2.name           = "uart2";
  uart2.comp_type      = Dw_apb_uart;
  uart2.data_width     = i_uart1_CC_UART_APB_DATA_WIDTH;
  uart2.base_address   = (void *) i_uart2_DW_APB_UART_BASE;
  uart2.comp_param     = &uart2_param;
  uart2.instance       = &uart2_instance;
  uart2.os             = NULL;

  uart2_param.afce_mode = i_uart1_CC_UART_AFCE_MODE;         // automatic flow control
  uart2_param.dma_extra = i_uart1_CC_UART_DMA_EXTRA;         // DW_ahb_dmac compatibility signals
  uart2_param.fifo_access = i_uart1_CC_UART_FIFO_ACCESS;       // programmable FIFO access mode
  uart2_param.fifo_stat = i_uart1_CC_UART_FIFO_STAT;         // additional FIFO status registers
  uart2_param.new_feat = i_uart1_CC_UART_ADDITIONAL_FEATURES;          // new v3 features (shadow registers, etc.)
  uart2_param.shadow = i_uart1_CC_UART_SHADOW;            // include shadow registers
  uart2_param.sir_lp_mode = i_uart1_CC_UART_SIR_LP_MODE;       // low-power IrDA SIR mode
  uart2_param.sir_mode = i_uart1_CC_UART_SIR_MODE;          // serial infrared
  uart2_param.thre_mode = i_uart1_CC_UART_THRE_MODE;         // programmable thre interrupts
  uart2_param.fifo_mode = i_uart1_CC_UART_FIFO_MODE;      // FIFO depth
}

/**********************************************************************/

static unsigned int calc_clock_divisor(unsigned int sclk, unsigned int baudrate)
{
	int divisor,divisor_reminder;
	divisor =  sclk / (16 * baudrate);
	divisor_reminder = sclk %  (16 * baudrate);

	// if reminder is grater than 10%  increase divisor
	if (divisor_reminder > (16 * baudrate)/10 )
		divisor ++;
	return divisor;
}

static void uart_apb_delay()
{
	volatile int i;
	// need to wait at least X APB clock cycles after the baud rate is set where
	//APB is SYS/2 clock. Assuming CPU  <= SYS we need at least 2*8*CPU cycles.
	for(i = 0; i < 32; i++);
}

static void uart_init_device(struct dw_device *device, unsigned int freq, unsigned int baud, int enable_fifo)
{
	int divisor;

	 // Initialize UARTs and set baud rate
    dw_uart_init(device);
	divisor = calc_clock_divisor(freq, baud);
    // Enable loopback to protect UART core from any RX events
    dw_uart_enableLoopback(device);

	dw_uart_setClockDivisor(device, divisor);
	uart_apb_delay();

	 // line settings
    dw_uart_setLineControl(device, Uart_line_8n1);
	 // enable FIFOs only for boot UART
     // no fifo is needed in debug mode to guarantee the real time performance of debug port
    if (enable_fifo)
    	dw_uart_enableFifos(device);

	 // ??? disable programmable THR empty mode
    dw_uart_disablePtime(device);

    // Disable loopback before normal operation
    dw_uart_disableLoopback(device);
    return;
}
/**********************************************************************/
/* NU3000 UART Access functions
 * UART1 - boot UART
 * UART2 - debug UART
 */
/**********************************************************************/
struct dw_device  *uart_boot_init()
{
	memset(&uart1,0,sizeof (uart1));
	memset(&uart1_instance,0,sizeof (uart1_instance));
	memset(&uart1_param,0,sizeof (uart1_param));

	setupDeviceStructuresUART1();
	uart_init_device(&uart1, get_board_info()->uart_boot_freq_hz, get_board_info()->uart_boot_baud_rate, 1);
    return &uart1;
}

struct dw_device  *uart_debug_init()
{
	memset(&uart2,0,sizeof (uart2));
	memset(&uart2_instance,0,sizeof (uart2_instance));
	memset(&uart2_param,0,sizeof (uart2_param));

	setupDeviceStructuresUART2();
	uart_init_device(&uart2, get_board_info()->uart_dbg_freq_hz, get_board_info()->uart_dbg_baud_rate, 0);
	return &uart2;
}

int uart_write(struct dw_device  *uart, unsigned char byte)
{
	struct dw_uart_portmap *portmap = uart->base_address;

	while ((UART_IN8P(portmap->lsr) & Uart_line_thre) == 0);
	UART_OUT8P(byte, portmap->rbr_thr_dll);
	return 1;
}


int uart_read(struct dw_device  *uart, unsigned char* data)
{
	// check for an error or break condition on uart1
	struct dw_uart_portmap *portmap = uart->base_address;
	unsigned char status;

	status = UART_IN8P(portmap->lsr);
	if((status & (Uart_line_oe | Uart_line_pe | Uart_line_fe |  Uart_line_bi | Uart_line_rfe)) != 0x0)
	{
		rel_log("ure:status=%x\n",status);
		return 0;
	}

	if ((status & Uart_line_dr) != 0)
	{
		*data = UART_IN8P(portmap->rbr_thr_dll);
		return 1;
	}

	return 0;
}

int uart_is_ready(struct dw_device  *uart)
{
	struct dw_uart_portmap *portmap = uart->base_address;
	unsigned char status;

	status = UART_IN8P(portmap->lsr);

	return (status & Uart_line_dr);
}

void uart_print(struct dw_device  *uart, const char *ptr)
{
	int i = 0;

	while (ptr[i] != '\0') {
		uart_write(uart, ptr[i]);
		i++;
	}
}
