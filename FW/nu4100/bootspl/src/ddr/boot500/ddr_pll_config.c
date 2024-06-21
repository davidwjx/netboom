#include "nu4000_c0_gme_regs.h"
#include "phyinit.h"

// ==========================================================================================
// Configure DDR PLL
// Spec of DDRM clock scheme is in:
//   http://svn/repos/inuitive/db/NU4000B0/vlsi/gme/trunk/doc/GME_clock_tree_NU4000_B0.pdf
// 
// PLL configuration is calculated by:
//   http://svn/repos/inuitive/db/NU4000B0/vlsi/gme/trunk/doc/PLL_frequency_calculator.xlsx
// to get an output frequency of 804MHz
//
void ddr_pll_config (unsigned int freq) {

  volatile unsigned int data_val;
  volatile unsigned int delay_cnt;
  
  GME_SPARE_0_SPARE_BITS_W(0xa01a0001); // write
  switch (freq) {
  case 334:
    GME_DDR_PLL_PARAM_FRAC_W(0x000000); // field: frac = PLL Fraction divider value in fraction mode
  
    GME_DDR_PLL_CONTROL_REFDIV_W(0x03);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x07D);  // PLL feedback divider value: Integer mode: 16-3800. Fractional mode: 20- 380. ==> get 498MHz.
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x6); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(1);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;
  case 350:
    GME_DDR_PLL_PARAM_FRAC_W(0x6ACFC0); // field: frac = PLL Fraction divider value in fraction mode
  
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x03a);  // PLL feedback divider value: Integer mode: 16-3800. Fractional mode: 20- 380. ==> get 498MHz.
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x4); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x2); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(0);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;
  case 1000:
  // set pll to fraction mode: DDR freq = ~533MHz(1066mb/s); PLLVCOOUT = ~1000MHz; FOUTPOSTDIV = ~500MHz(474MHz)
  // Register: ddr_pll_param
  GME_DDR_PLL_PARAM_FRAC_W(0x000000); // field: frac = PLL Fraction divider value in fraction mode
  
  // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x03);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x07D);  // PLL feedback divider value: Integer mode: 16-3800. Fractional mode: 20- 380. ==> get 498MHz.
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
  GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
  GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
  GME_DDR_PLL_CONTROL_DSMPD_W(1);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
  GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
  break;

  case 1400:
    GME_DDR_PLL_PARAM_FRAC_W(0x6ACFC0);  // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x03a);  // PLL feedback divider value
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(0);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;

  case 1334:
    GME_DDR_PLL_PARAM_FRAC_W(0x989680);  // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x037);  // PLL feedback divider value
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(0);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;

  case 1200:
    GME_DDR_PLL_PARAM_FRAC_W(0x000000); // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x032);  // PLL feedback divider value
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(1);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;

  case 1067:
    GME_DDR_PLL_PARAM_FRAC_W(0x6ACFC0);  // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x02C);  // PLL feedback divider value
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(0);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;

  case 533:
    // set pll to fraction mode: DDR freq = ~533MHz(1066mb/s); PLLVCOOUT = ~1066MHz; FOUTPOSTDIV = ~266.5MHz(132.894MHz)
    // Register: ddr_pll_param
    GME_DDR_PLL_PARAM_FRAC_W(0x4C4B40); // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x02C);  // PLL feedback divider value: Integer mode: 16-3800. Fractional mode: 20- 380. ==> get 266.5MHz.
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x2); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(0);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;

  case 800:
    // set pll to fraction mode: DDR freq = ~800MHz(1600mb/s); PLLVCOOUT = ~1600MHz; FOUTPOSTDIV = ~800MHz(132.894MHz)
    // Register: ddr_pll_param
    GME_DDR_PLL_PARAM_FRAC_W(0x000000); // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x085);  // PLL feedback divider value: Integer mode: 16-3800. Fractional mode: 20- 380. ==> get 399MHz.
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x4); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x2); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(1);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
    break;

  default: // 1600
    // DDR freq = 1600MHz(3200mb/s); PLLVCOOUT = 1600MHz; FOUTPOSTDIV = 800MHz Frequency is 792MHz
    // Register: ddr_pll_param
    GME_DDR_PLL_PARAM_FRAC_W(0x000000); // field: frac = PLL Fraction divider value in fraction mode
  
    // Register: ddr_pll_control (field description is from SoC On-Line)
    GME_DDR_PLL_CONTROL_REFDIV_W(0x01);  // Reference clock divide value
    GME_DDR_PLL_CONTROL_FBDIV_W(0x042);  // PLL feedback divider value: Integer mode: 16-3800. Fractional mode: 20- 380. ==> get 792MHz.
    GME_DDR_PLL_CONTROL_POSTDIV1_W(0x2); // PLL post divider 1
    GME_DDR_PLL_CONTROL_POSTDIV2_W(0x1); // PLL post divider 2
  
    GME_DDR_PLL_CONTROL_BYPASS_W(0);     // Reference clock is bypassed to FOUTPOSTDIV: 0-normal (reset value), 1-bypass
    GME_DDR_PLL_CONTROL_DACPD_W(0);      // Power down noise canceling DAC in FRAC mode: 0-DAC is active (default mode, reset value), 1-DAC in not active (test mode only).
    GME_DDR_PLL_CONTROL_DSMPD_W(1);      // Power down delta-sigma modulator: 0-DSM is active (fractional mode), 1-DSM is powered down (integer mode, reset value)
  
    GME_DDR_PLL_CONTROL_PLL_ON_W(1);     // Turn PLL on/off: 0-OFF 1-ON
  }
  
  // wait for pll lock
  data_val = 0;
  while (data_val == 0) {
    data_val = GME_DDR_PLL_STATUS_LOCK_R; // Register: ddr_pll_status, Field: lock
    delay_cnt=0;
    while (delay_cnt<10) delay_cnt++; // polling delay
  }
  GME_SPARE_0_SPARE_BITS_W(0xa01a0002); // write
  
  // Enable DDR_CLK
  GME_CLOCK_ENABLE_DDR_CLK_EN_W(1); // Register: clock_enable, Field: ddr_clk_en: "DDR PLL output clock gater enable"
  
  // and make sure DDR clock is enabled:
  // ("Sw can read this register to see which system clocks are enabled/disabled")
  data_val = 0;
  while (data_val == 0) {
    data_val = GME_CLOCK_ENABLE_STATUS_DDR_CLK_EN_R; // Register: clock_enable_status, Field: ddr_clk_en: "DDR PLL output clock gater enable status"
    delay_cnt=0;
    while (delay_cnt<5) delay_cnt++; // polling delay
  }
  
  GME_SPARE_0_SPARE_BITS_W(0xa01a0003); // write
  
}

