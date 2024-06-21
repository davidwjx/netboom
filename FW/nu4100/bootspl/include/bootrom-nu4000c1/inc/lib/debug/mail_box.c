#ifdef VCS
#include "nu_regs.h"
#include "sim.h"

#define GPIO_DIRECTION           GPIO_GPIO_SWPORTA_DDR_VAL
#define GPIO_VALUE               GPIO_GPIO_SWPORTA_DR_VAL

/////////////////////////////////////////////////////////////////
// 
//             set_reg(unsigned short, char [])
//
/////////////////////////////////////////////////////////////////
//
// Fuction to set registers bit by bit 
//
unsigned long set_reg(unsigned long reg, char val[33]){
   int i;
   unsigned long gpio;
   gpio = reg;
   for (i=0;i<32;i++){
      if (val[i] == '1') gpio |= (1 << i);
      else if (val[i] == '0') gpio &= ~(1 << i);
  }
  return gpio;
}


/////////////////////////////////////////////////////////////////
//
//                  pass2sv_gpio(char)
//
/////////////////////////////////////////////////////////////////

// Set the gpio [15:8] to the wait 8 bit flag and gpio [7] to 0.
// Task nu3000_soc_arm_base_test::receive_sw_cmd() sample the gpio @ bit 7 == 0 and save gpio [15:8] flag in `NU3000_SOC_TB.sw2sv_cmd
// sv tests wait for known `NU3000_SOC_TB.sw2sv_cmd valeu
//
void pass2sv_gpio(char sw2sv_cmd)
{
   GPIO_DIRECTION |= ((1 << 15) | (1 << 14) | (1 << 13) | (1 << 12) | (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 7)); // set bits 15 downto 7 to output direction.
   GPIO_VALUE |= ((sw2sv_cmd << 8) | (0 << 7)); // Set gpio to the ascii value of the flag and bit 7 to 0. 
   GPIO_VALUE &= ( (~(1 << 15)) & ~(1 << 14) & ~(1 << 13) & ~(1 << 12) & ~(1 << 11) & ~(1 << 10) & ~(1 << 9) & ~(1 << 8) & ~(1 << 7)); // // Set data back to zero
 }
/////////////////////////////////////////////////////////////////
// 
//                  get_sv_gpio(char)
//
/////////////////////////////////////////////////////////////////
//
// Delay function that wait for GPIO [15:8] to be equal to the function input
// GPIO [15:8] is set by the sv test
// Since the GPIO direction (GPIO_DIRECTION) can onley be set by the C progrem at the begining of the function the direction is set to input
 
int get_sv_gpio(char sv2sw_cmd){
   GPIO_DIRECTION &= ( (~(1 << 15)) & ~(1 << 14) & ~(1 << 13) & ~(1 << 12) & ~(1 << 11) & ~(1 << 10) & ~(1 << 9) & ~(1 << 8) & ~(1 << 7)); // set direction to input (=0)
   if (sv2sw_cmd == 64) { // small patch to get random bit from SV 
      while(!((GPIO_GPIO_EXT_PORTA_VAL >> 8) == 64 || GPIO_GPIO_EXT_PORTA_VAL >> 8) == 63){}
      return ((GPIO_GPIO_EXT_PORTA_VAL >> 8) - 63);
   }
   while((GPIO_GPIO_EXT_PORTA_VAL >> 8) != sv2sw_cmd){}
   return sv2sw_cmd;
}

/////////////////////////////////////////////////////////////////
// 
//                     sync2sv(char)
//
/////////////////////////////////////////////////////////////////
//
// Sync between sw and sv
// Send a "go" signal to the sv and wait for handshake
// The go flag should be inside [a..z]
 
void sync2sv(char sw2sv_cmd){
   if ( sw2sv_cmd < 96 || sw2sv_cmd >122) string_to_gpio ("ERROR sw2sv_cmd is not inside [a..z]");
   pass2sv_gpio(sw2sv_cmd);
   get_sv_gpio(sw2sv_cmd-32);
}

/////////////////////////////////////////////////////////////////
// 
//                   print_gpio(char[])
//
/////////////////////////////////////////////////////////////////
//
// Function to print input string char by char to bits 8..15 of the GPIO
// bit 7 is "data valid"

void print_gpio (char str[])
{ 
  int i = 0;
  char val_111111111[33] = {'x','x','x','x','x','x','x','1','1','1','1','1','1','1','1','1','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x'};
  char val_000000000[33] = {'x','x','x','x','x','x','x','0','0','0','0','0','0','0','0','0','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x'};
  char val_dollar[33] ={'x','x','x','x','x','x','x','1','0','0','1','0','0','1','0','0','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x','x'};
  GPIO_DIRECTION = set_reg(GPIO_DIRECTION, val_111111111); // set bits [15..7] to output (=1)
  if (str[0] == '\0'){
    print_gpio("ERROR! empty string was received, input string may be too long.");
    return;
  } // print warning for empty string
  while ( str[i] != '\0'){
      GPIO_VALUE |= ((str[i] << 8) | (1 << 7)); // set gpio to ascii value of the character and bit 7 to 1.
      GPIO_VALUE = set_reg(GPIO_VALUE, val_000000000); // set bits [15..8] and bit 7 to zero after every char
      i++;
  }

  GPIO_VALUE = set_reg(GPIO_VALUE, val_dollar); // Send $ to indicate end of string
  GPIO_VALUE = set_reg(GPIO_VALUE, val_000000000);  // set bits [15..8] and bit 7 to zero.
}
#endif //VCS
