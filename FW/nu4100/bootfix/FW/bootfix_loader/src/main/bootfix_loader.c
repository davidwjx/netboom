/* This code performs board_end() and prints some debug prints */
#include "common.h"
#include "usb_drv.h"
#include "spi_flash.h"
#include "gme.h"
#include "nu4000_c0_gme_regs.h"
#include "bootfix_loader.h"
#include "storage.h"
#include "verify.h"

#ifndef LRAM_BASE
#define LRAM_BASE 0x2000000
#endif
#define BOOTFIX_LRAM_OFFSET 0x8000
storage_boot_header_t storage_boot_header;
extern void dcache_disable(void);
static void *uart_boot_dev= NULL;
int uart0_printf(const char *fmt_str, ...);

#define loader_log(fmt, ...)  \
   do { \
      debug_printf(fmt, ##__VA_ARGS__); \
      uart0_printf(fmt, ##__VA_ARGS__); \
   }while(0)

void system_jump_to_address(unsigned int address)
{
	// Ensure to invalidate the i-cache before jump to CRAM/DDR
   GME_SW_RESET_0_SW_UART1_RESET_N_W(0);
   GME_SW_RESET_0_SW_UART1_RESET_N_W(1);
   invalidate_icache_all();
	((void_func)address)();
}

/* load bootfix code from flash to ram and jump to bootfix*/
void load_bootfix_code()
{
  uint32 ret,size,load_addr=LRAM_BASE + BOOTFIX_LRAM_OFFSET,sbOffset=0;
  uint8 *flash_addr;

  ret = SPI_FLASHG_Read(sizeof(storage_header_t), sizeof(storage_boot_header_t), (uint8 *) &storage_boot_header);
  if (ret == 0)
  {
    flash_addr = (uint8 *)(sizeof(storage_header_t)+sizeof(storage_boot_header_t));

    flash_addr += storage_boot_header.bootcode_size; // bootfix loader size
    ret = SPI_FLASHG_Read((unsigned int)flash_addr, sizeof(size), (uint8 *) &size);
    if (ret == 0)
    {
      flash_addr += sizeof(size);


      /* read bootfix from flash and copy it to ram*/
       if (size%4)
        size += (4-size%4);
      if (getEfuseSecureControlData() != NOT_SECURED_IMAGE)
          sbOffset = 0x4000;
      ret = SPI_FLASHG_Read((UINT32)flash_addr, size,(BYTE *)(load_addr+sbOffset));
      if (ret == 0)
      {
          if (getEfuseSecureControlData() != NOT_SECURED_IMAGE)
             startSecureBoot(load_addr+sbOffset,sbOffset);
          loader_log ("bootfix_loader: %s %d Jump to bootfix\n",__FILE__,__LINE__);
          system_jump_to_address(load_addr);
      }
    }
  }
  loader_log ("bootfix_loader: Fail read FLASH %x\n",ret);
}

/* Byte write primitive for all supported environments */
void uart0_writeb(unsigned char byte)
{
/*******************************************/
/*********  For BootROM release  ***********/
/*******************************************/
   // Enable UART debug according to strap pin
   if(uart_boot_dev)
   {
      uart_write(uart_boot_dev, byte);
   }
}

static inline void fill_string(char *p)
{
   while (*p != 0) {
       uart0_writeb(*p++);
   }
}

static inline void fill_int(unsigned int data)
{
   if ((data/10) > 0) {
      fill_int(data/10);
   }

   uart0_writeb((data%10) + '0');
}

static inline void fill_hex_int(unsigned int data)
{
   if ((data >> 4) > 0) {
      fill_hex_int(data >> 4);
   }

   if ((data & 0xF) < 10)
       uart0_writeb((data & 0xF) + '0');
   else
       uart0_writeb((data & 0xF) - 10 + 'a');
}

int uart0_printf(const char *fmt_str, ...)
{
   va_list ap;

   if (uart_boot_dev == NULL)
      return 0;

   va_start(ap, fmt_str);

   while (*fmt_str != 0) {

#ifdef CONFIG_BUFFER_DEBUG_MSG
      if(debug_buffer_overflow())
         break;
#endif

      if (*fmt_str == '%') {
         if (*(fmt_str + 1) == '%') {
             uart0_writeb('%');
            fmt_str += 2;
         }
         else {
            fmt_str++;   /* skip % */
            while(*fmt_str != 0) {

               switch (*fmt_str) {
               case 'd':
               case 'i':
                  fill_int(va_arg(ap, unsigned int));
                  break;
               case 'u':
               case 'x':
                  uart0_writeb('0');
                  uart0_writeb('x');
                  fill_hex_int(va_arg(ap, unsigned int));
                  break;
               case 's':
                  fill_string(va_arg(ap, char *));
                  break;
               case 'c':
                  uart0_writeb ((unsigned char)va_arg(ap, signed long));
                  break;
               default:
               {
                  fmt_str++;
                  continue; //skip over unsupported stuff
               }

               }

               fmt_str++;
               break;
            }
         }
      }
      else if(*fmt_str == '\n') {
         uart0_writeb('\r');
         uart0_writeb(*fmt_str++);
      }
      else {
          uart0_writeb(*fmt_str++);
      }
   }
   va_end(ap);

   return 0;
}

__attribute__((__section__(".boot")))
void bootflash_init()
{
  dcache_disable();
  fillSBEfuseDB();
  board_set_mode(BOARD_CLK_PLL);
  if (getEfuseSecureControlData() == NOT_SECURED_IMAGE)
      board_lram_clk_set();

  board_boot_uart_init();
  uart_boot_dev = uart_boot_init();
  board_sys_clk_set();
  board_gpp_clk_set();
  loader_log ("bootfix_loader: version=%d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, BUILD_VERSION);
  board_spi_init();

  load_bootfix_code();

   // When this function returns it enters process_bg_process loop in ROM
  return;
}


