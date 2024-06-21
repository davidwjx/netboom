#ifdef VCS
//#include "zr_common.h"
  // Function to print input string char by char to bits 8..15 of the GPIO
  // bit 7 is "data valid"
#include "nu_regs.h"
#include "common.h"
#include "sim.h"

void string_to_gpio (char str[])
{ 
  int i = 0;
  char warning[] = "Warning! empty string was received, input string may be too long.";
  int warning_len = 66; // TODO change if warning string is changed
  unsigned long gpio;

//  GPIO_SWPORTA_DDR_VAL = ((1 << 15) | (1 << 14) | (1 << 13) | (1 << 12) | (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 7)); // set bits 15 downto 7 to output direction.
  gpio = GPIO_GPIO_SWPORTA_DDR_VAL;

  gpio |= (1 << 15);
  gpio |= (1 << 14);
  gpio |= (1 << 13);
  gpio |= (1 << 12);
  gpio |= (1 << 11);
  gpio |= (1 << 10);
  gpio |= (1 << 9);
  gpio |= (1 << 8);
  gpio |= (1 << 7);

  GPIO_GPIO_SWPORTA_DDR_VAL = gpio;
  //GPIO_SWPORTA_DDR_VAL = gpio;

  if (str[i] == '\0'){
    for (i=0;i==warning_len;i++){ 
       str[i] = warning[i];
    }
    i = 0;
  } // print warning for empty string
  while ( str[i] != '\0'){
      GPIO_GPIO_SWPORTA_DR_VAL |= ((str[i] << 8) | (1 << 7)); // set gpio to ascii value of the character and bit 7 to 1.
      //GPIO_SWPORTA_DR_VAL = (0 << 0); // set bit 7 (and all other bits) back to zero.

      gpio = GPIO_GPIO_SWPORTA_DR_VAL;

      gpio &= ~(1 << 15);
      gpio &= ~(1 << 14);
      gpio &= ~(1 << 13);
      gpio &= ~(1 << 12);
      gpio &= ~(1 << 11);
      gpio &= ~(1 << 10);
      gpio &= ~(1 << 9);
      gpio &= ~(1 << 8);
      gpio &= ~(1 << 7);

      GPIO_GPIO_SWPORTA_DR_VAL = gpio;

      //GPIO_SWPORTA_DR_VAL = (0 << 0); // set bit 7 (and all other bits) back to zero.
      i++;
  }
  //GPIO_SWPORTA_DR_VAL = (('$' << 8) | (1 << 7)); // set gpio to ascii value of $ and bit 7 to 1. $ is the sighn for end of transaction.
  //
      gpio = GPIO_GPIO_SWPORTA_DR_VAL;

  gpio &= ~(1 << 15);
  gpio &= ~(1 << 14);
  gpio |= (1 << 13);
  gpio &= ~(1 << 12);
  gpio &= ~(1 << 11);
  gpio |= (1 << 10);
  gpio &= ~(1 << 9);
  gpio &= ~(1 << 8);
  gpio |= (1 << 7);
      GPIO_GPIO_SWPORTA_DR_VAL = gpio;


  //GPIO_SWPORTA_DR_VAL = (0 << 0); // set bit 7 (and all other bits) back to zero.



            gpio = GPIO_GPIO_SWPORTA_DR_VAL;

      gpio &= ~(1 << 15);
      gpio &= ~(1 << 14);
      gpio &= ~(1 << 13);
      gpio &= ~(1 << 12);
      gpio &= ~(1 << 11);
      gpio &= ~(1 << 10);
      gpio &= ~(1 << 9);
      gpio &= ~(1 << 8);
      gpio &= ~(1 << 7);

      GPIO_GPIO_SWPORTA_DR_VAL = gpio;

/*
  GPIO_SWPORTA_DDR_VAL = ((1 << 15) | (1 << 14) | (1 << 13) | (1 << 12) | (1 << 11) | (1 << 10) | (1 << 9) | (1 << 8) | (1 << 7)); // set bits 15 downto 7 to output direction.
  if (str[i] == '\0'){
    for (i=0;i==warning_len;i++){ 
       str[i] = warning[i];
    }
    i = 0;
  } // print warning for empty string
  while ( str[i] != '\0'){
      GPIO_SWPORTA_DR_VAL = ((str[i] << 8) | (1 << 7)); // set gpio to ascii value of the character and bit 7 to 1.
      GPIO_SWPORTA_DR_VAL = (0 << 0); // set bit 7 (and all other bits) back to zero.
      i++;
  }
  GPIO_SWPORTA_DR_VAL = (('$' << 8) | (1 << 7)); // set gpio to ascii value of $ and bit 7 to 1. $ is the sighn for end of transaction.
  GPIO_SWPORTA_DR_VAL = (0 << 0); // set bit 7 (and all other bits) back to zero.
*/
}
#endif //VCS
