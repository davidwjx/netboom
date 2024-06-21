#include "common.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_pss_regs.h"
unsigned int getAudioFreq();
unsigned int reg_080F0004_orig = 0;

void clocksInit ()
{
   //WD safety
   reg_080F0004_orig = *((unsigned int*)(0x080F0000+0x4));
   *((unsigned int*)(0x080F0000+0x4)) = 0xFF;
   *((unsigned int*)(0x080F0000+0xC)) = 0x76;


   // --------------------------------------- //
   // Configure CPU PLL parameters to 1200Mhz //
   // --------------------------------------- //
//   GME_CPU_PLL_CONTROL_VAL = 0x9003205;

   // -------------------------------------- //
   // Configure SYS PLL parameters to 600Mhz //
   // -------------------------------------- //
//   GME_SYS_PLL_CONTROL_VAL = 0xA003205;
   //GME_SYS_PLL_CONTROL_VAL = 0xC003205;        // 300 MHz

   // --------------------------------------- //
   // Configure DSP PLL parameters to 804Mhz //
   // --------------------------------------- //
//   GME_DSP_PLL_CONTROL_VAL = 0xA008609;

   // --------------------------------------- //
   // Configure EVP PLL parameters to 804Mhz //
   // --------------------------------------- //
  // GME_AUDIO_PLL_CONTROL_VAL = 0x12009605;
   //BOARD_set_freq(0,getAudioFreq(), 0);

   // --------------------------------- //
   // wait for pll_locks
   //    GME_CPU_PLL_STATUS_ADD[PLL_LOCK] == 1
   //    GME_SYS_PLL_STATUS_ADD[PLL_LOCK] == 1
   //    GME_DSP_PLL_STATUS_ADD[PLL_LOCK] == 1
   //    GME_AUDIO_PLL_STATUS_ADD[PLL_LOCK] == 1
   // --------------------------------- //
//   while(!(GME_CPU_PLL_STATUS_LOCK_R));
//   while(!(GME_SYS_PLL_STATUS_LOCK_R));
//   while(!(GME_DSP_PLL_STATUS_LOCK_R));
//   while(!(GME_AUDIO_PLL_STATUS_LOCK_R));

   // --------------------------------- //
   //   Configure clock select :
   //   	GME_GPP_CLOCK_CONFIG_ADD[CLK_SRC] == 1
   //   	GME_SYS_CLOCK_CONFIG_ADD[CLK_SRC] == 1
   //   	GME_DSP_CLOCK_CONFIG_ADD[CLK_SRC] == 1
   //   	GME_AUDIO_CLOCK_CONFIG_ADD[CLK_SRC] == 1
   // --------------------------------- //
//   GME_GPP_CLOCK_CONFIG_CLK_SRC_W(1);
//   GME_SYS_CLOCK_CONFIG_CLK_SRC_W(1);
//   GME_DSP_CLOCK_CONFIG_CLK_SRC_W(1);
// GME_AUDIO_CLOCK_CONFIG_CLK_SRC_W(0);
   //GME_AUDIO_CLOCK_CONFIG_CLK_SRC_W(1);

   // IAE Clock source from DSP, divided by 2

   GME_CIIF_CONTROL_CLK_SRC_W(2);
   GME_CIIF_CONTROL_CIIF_CLK_DIV_W(4);
// GME_AUDIO_CLOCK_CONFIG_CLK_DIV_W(4);

   // GME_CLOCK_ENABLE_AUDIO_CLK_EN_W(1);
   // while(!GME_CLOCK_ENABLE_STATUS_AUDIO_CLK_EN_R);

//  GME_PERIPH_CLOCK_ENABLE_AUDIO_EXT_CLK_EN_W(1);
//  GME_IO_CONTROL_STRAP_OE_W(1);

   // --------------------------------- //
   // Frequency change GO bit for both cpu/sys clocks:
   //    GME_FREQ_CHANGE_ADD[AUDIO_GO_BIT, DSP_GO_BIT, SYS_GO_BIT, CIIF_GO_BIT, GPP_GO_BIT] == [1,1,1,1]
   // Bit[10],Bit[8],Bit[3],Bit[2],Bit[0]
   // --------------------------------- //
   GME_FREQ_CHANGE_VAL = 0x0000050D;

   // --------------------------------- //
   // Pool the frequency change status register to read the configured value
   //        GME_FRQ_CHG_STATUS_GPP_FREQ_CHG_DONE_R
   //        GME_FRQ_CHG_STATUS_SYS_FREQ_CHG_DONE_R
   //        GME_FRQ_CHG_STATUS_DSP_FREQ_CHG_DONE_R
   //        GME_FRQ_CHG_STATUS_AUDIO_FREQ_CHG_DONE_R
   //        GME_FRQ_CHG_STATUS_CIIF_FREQ_CHG_DONE_R
   // --------------------------------- //
//   while(!(GME_FRQ_CHG_STATUS_GPP_FREQ_CHG_DONE_R));
//   while(!(GME_FRQ_CHG_STATUS_SYS_FREQ_CHG_DONE_R));
//   while(!(GME_FRQ_CHG_STATUS_DSP_FREQ_CHG_DONE_R));
   // while(!(GME_FRQ_CHG_STATUS_AUDIO_FREQ_CHG_DONE_R));
   while(!(GME_FRQ_CHG_STATUS_CIIF_FREQ_CHG_DONE_R));

   // --------------------------------- //
   // Once frequency change is done, clear the Frequency change register
   //    GME_FRQ_CHG_STATUS_CLEAR_ADD
   // --------------------------------- //
   GME_FREQ_CHANGE_VAL = 0x0000000;
   GME_FRQ_CHG_STATUS_CLEAR_VAL = 0x0000050D;

   // Power up IAE, EVP, DSP

   GME_POWER_MODE_IAE_POWER_UP_W(1);
   GME_POWER_MODE_DSP_POWER_UP_W(1);

   GME_CONTROL_START_POWER_CHANGE_W(1);

   while(!GME_POWER_MODE_STATUS_IAE_POWER_UP_R);
   while(!GME_POWER_MODE_STATUS_DSP_POWER_UP_R);

   GME_CONTROL_START_POWER_CHANGE_W(0);

   BOARD_setIaeClk(1);

#ifdef DSPA_CLOCK_EN
   GME_CLOCK_ENABLE_DSPA_CLK_EN_W(1);    // The DSP CLK EN is 1 by default (for the lram) / DSPA clock is for the processor/memories
   while(!GME_CLOCK_ENABLE_STATUS_DSPA_CLK_EN_R);
#endif

}

void clocksDeinit ()
{
   BOARD_setIaeClk(0);

   #ifdef BOOT_500 //dosent succeed
   //BOARD_setEVPClk(0);
   #endif
   
   GME_POWER_MODE_IAE_POWER_UP_W(0);
   GME_POWER_MODE_DSP_POWER_UP_W(0);

   GME_CONTROL_START_POWER_CHANGE_W(1);

   while(GME_POWER_MODE_STATUS_IAE_POWER_UP_R);
   while(GME_POWER_MODE_STATUS_DSP_POWER_UP_R);

   GME_CONTROL_START_POWER_CHANGE_W(0);

   //set WD back to normal values
   *((unsigned int*)(0x080F0000+0x4)) = reg_080F0004_orig;
   *((unsigned int*)(0x080F0000+0xC)) = 0x76;
}

