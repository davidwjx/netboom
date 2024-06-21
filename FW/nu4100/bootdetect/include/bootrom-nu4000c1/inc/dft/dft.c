#include "common.h"
#include "board.h"
#include "storage.h"
#include "flash_boot.h"
#include "gme.h"

#define LOCK_CNT_TIME        (0x1c22)//300 usec in 1/24MHz periods (41.67nsec) = 7202
#define LOCK_CNT_SELECT      (1) //enable lock count

void run_test_mode(void)
{
   debug_init(0); //disable all printfs
   GMEG_configPllLockCnt(LOCK_CNT_SELECT, LOCK_CNT_TIME);
   board_set_mode(BOARD_CLK_PLL);
   board_clks_set();
   board_spi_init();
   storage_flash_init(get_board_info()->spi_freq_hz);

   //check for boot
   if(flash_is_valid() && flash_is_bootable())
   {
      flash_boot(); //flash boot does not return
   }
}

