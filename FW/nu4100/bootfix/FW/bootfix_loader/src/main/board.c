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
#define PLL_CLK_973M_HZ          (973000000)
#define OSC_CLK_HZ               (24000000)

#define PLL_LOCK_CNT_TIME        (0x960)//100 usec in 1/24MHz periods (41.67nsec) = 2400
#define PLL_LOCK_CNT_SELECT      (1) //enable lock count

/**********************************************
 * local typedefs
***********************************************/

/**********************************************
 * local data
***********************************************/
typedef enum
{
   PLL_CPU = 0,
   PLL_SYS = 1,
   PLL_MAX_NUM = 2
} pplConfigE;



const GMEG_pllConfigT gme_configs[PLL_MAX_NUM]=
{
      {//CPU
      .refdiv = 12,
      .fbdiv = 600,
      .postdiv1 = 2,
      .postdiv2 = 1,
      .dacpd = 0,
      .dsmpd = 1,
      .frac = 0,
      .bypass = 0,
      .on = 1,
      },
     {//SYS
       .refdiv = 12,
      .fbdiv = 300,
      .postdiv1 = 2,
      .postdiv2 = 1,
      .dacpd = 0,
      .dsmpd = 1,
      .frac = 0,
      .bypass = 0,
      .on = 1,
      },
   };

void board_spi_init(void)
{

  SPI0_ISPI_CONFIGURATION_RX_PHASE_W(1);

   GMEG_enableClk(BOARD_SPI);
   GMEG_setFcuClockDiv(2);
   GMEG_changeUnitFreq(BOARD_SPI);
   SPI0_ISPI_CONFIGURATION_CLK_DIV_W(4);
}



/**
 * Routine: board_sys_clk_set
 * Sets sys clock and dividers according board info
 */
void board_sys_clk_set()
{

  GMEG_configSysPll(&gme_configs[PLL_SYS]);
  GMEG_setSysClockSrc(GMEG_SYS_CLK_SRC_PLL);
  GMEG_setSysClockDiv(1);
  GMEG_changeUnitFreq(GMEG_HW_UNIT_SYS_E);
  GMEG_lockSysPll();
}


void board_gpp_clk_set()
{
   unsigned long long val;
   unsigned int frac;
   if (gme_configs[PLL_CPU].frac)
   {
      val = 0x1000000ULL * gme_configs[PLL_CPU].frac;
      frac = val/10000;
      GME_CPU_PLL_PARAM_VAL = frac;
   }
   GMEG_configCpuPll(&gme_configs[PLL_CPU]);
   GMEG_setGppClockSrc(GMEG_CPU_CLK_SRC_PLL);
   GMEG_setGppClockDiv(1);
   GMEG_changeUnitFreq(GMEG_HW_UNIT_GPP_E);
   GMEG_lockCpuPll();
}

