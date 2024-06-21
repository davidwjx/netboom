#ifndef BOARD_H
#define BOARD_H

#include "gme.h"
//avoiding modifing the bootrom header files
#define GMEG_HW_UNIT_AUDIO_E (GMEG_HW_UNIT_LRAM_E + 1)

void BOARD_print_freq();
typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
		  refdiv   :6,
		  fbdiv    :12,
		  unused   :4,
		  postdiv1 :3,
		  postdiv2 :3,
		  dac      :1,
		  dsm      :1;
} BOARD_pllControlT;

typedef struct
{
   UINT32 pll_on   :1,
          bypass   :1,
		  refdiv   :6,
		  fbdiv    :12,
		  unused   :4,
		  postdiv1 :3,
		  postdiv2 :3,
		  lock     :1,
		  unused1  :1;
} BOARD_pllStatusT;

typedef enum {
   BOARD_PLL_AUDIO_E = 0,
   BOARD_PLL_CPU_E,
   BOARD_PLL_DDR_E,
   BOARD_PLL_DSP_E,
   BOARD_PLL_SYS_E
} BOARD_PLLTypesE;

typedef enum {
   BOARD_POWER_PSS_E = 0,
   BOARD_POWER_IAE_E,
   BOARD_POWER_DPE_E,
   BOARD_POWER_PPE_E,
   BOARD_POWER_DSP_E,
   BOARD_POWER_EVP_E,
   BOARD_POWER_ALL_E = 0x3f
} BOARD_powerModeE;


#define BOARD_OSC_CLK_HZ (24000000U)
#define BOARD_AUDIO_CLK_M_HZ (864) //850)
#define BOARD_CPU_CLK_M_HZ (973)
#define BOARD_CPU_CLK_FRAQ (2096)
#define BOARD_DSP_CLK_M_HZ (800) //(944)
#define BOARD_SYS_CLK_M_HZ (600)

void BOARD_set_freq(BOARD_PLLTypesE pllType, unsigned int numInMega,unsigned int fraq);
void BOARD_setPowerMode(BOARD_powerModeE powerMode);
void board_changeUnitFreq(GMEG_hwUnitE unit);
void BOARD_setIaeClk(UINT8 enable);
void BOARD_setEVPClk(UINT8 enable);

#endif
