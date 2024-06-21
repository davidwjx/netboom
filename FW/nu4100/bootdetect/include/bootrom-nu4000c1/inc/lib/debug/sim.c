// VCS test support routines
#include "nu_regs.h"
#include "common.h"


/**********************************************
 * functions
***********************************************/
void end_of_test_vcs(unsigned int  errors_num) {
//Test ends when GPIO0 rise
// GPIO1: 0 = PASS. 1 = FAIL
// Data Direction Register: 0 = input, 1 = output
   GPIO_GPIO_SWPORTA_DDR_VAL = ((1 << 0) | (1 << 1)); // set bits 0 and 1: GPIO 0 and GPIO1 set to output direction
   if (errors_num > 0) {           
     GPIO_GPIO_SWPORTA_DR_VAL = ((1 << 0) | (1 << 1)); // test failed
   } else {
     GPIO_GPIO_SWPORTA_DR_VAL = (1 << 0); // test passed
   }
}

void sim_init()
{
   GME_IO_MUX_CTRL_0_VAL = 0x01100000;

   GME_CLOCK_ENABLE_GPIO_DB_CLK_EN_W(1);     
   GME_CONTROL_GPIO_DEBOUNCE_ENABLE_W(1);
}

/* Last function call in Boot ROM */
void board_end(unsigned int errors_num)
{
   end_of_test_vcs(errors_num);
   while(1);
}
