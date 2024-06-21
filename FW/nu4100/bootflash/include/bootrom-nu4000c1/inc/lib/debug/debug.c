/*
 * Copyright (c) 2013, Inuitive Corporation
 * Author: Konstantin Sinyuk
 *
 */

#include "common.h"
#include "uart.h"
#include "gme.h"
#include "nu_regs.h"

#include <stdio.h>
#include <stdarg.h>

#ifdef CONFIG_BUFFER_DEBUG_MSG
extern unsigned int system_get_tick_count(void);
#define DEBUG_BUFFER_SIZE   (128)
#define DEBUG_BUFFER_FLUSH_MSEC  (5000)
SECTION_PBSS static unsigned char dbg_buf[DEBUG_BUFFER_SIZE+1];
SECTION_PBSS static volatile unsigned int dbg_buf_ptr;
SECTION_PBSS static volatile int  dbg_buf_enabled;
SECTION_PBSS static volatile int dbg_buf_flushing;
SECTION_PBSS static unsigned int last_flush_time;
#endif

#ifdef VCS
//Address location to write to in VCS environment
#define DEBUG_VCS_PRINT_BYTE_VAL *((volatile UINT32 *)(0x0205fffc))
#endif


/* Do not clean the UART device info, thus we can init it safely before BSS cleanup */
SECTION_PBSS struct dw_device *dbg_uart;


/* Pre definition */
void debug_writeb(unsigned char);


static inline void fill_string(char *p)
{
   while (*p != 0) {
      debug_writeb(*p++);
   }
}

static inline void fill_hex_int(unsigned int data)
{
   if ((data >> 4) > 0) {
      fill_hex_int(data >> 4);
   }

   if ((data & 0xF) < 10)
      debug_writeb((data & 0xF) + '0');
   else
      debug_writeb((data & 0xF) - 10 + 'a');
}

static inline void fill_int(unsigned int data)
{
   if ((data/10) > 0) {
      fill_int(data/10);
   }

   debug_writeb((data%10) + '0');
}

#ifdef CONFIG_BUFFER_DEBUG_MSG
static int debug_buffer_overflow(void)
{
   const char overflow_str[] = {'\n','\r','#','#','#','\n','\r'};
   int i;
   int ret = 0;
   //throttle on number of printfs
   if(dbg_buf_ptr >= DEBUG_BUFFER_SIZE)
   {
      ret = 1;
      if(dbg_buf_ptr == DEBUG_BUFFER_SIZE) 
      {
         if(dbg_uart)
         {
            //marker to indicate overflow
            for(i = 0;i < sizeof(overflow_str); i++)
               uart_write(dbg_uart, overflow_str[i]); 
         }
         dbg_buf_ptr++;
      }
   }
   return ret;
}
#endif

int debug_printf(const char *fmt_str, ...)
{
   va_list ap;

   if (dbg_uart == NULL)
      return 0;

#ifdef B0_TEST
      debug_writeb('b');
      debug_writeb('0');
      debug_writeb(' ');
#endif 

   va_start(ap, fmt_str);

   while (*fmt_str != 0) {

#ifdef CONFIG_BUFFER_DEBUG_MSG
      if(debug_buffer_overflow())
         break;
#endif

      if (*fmt_str == '%') {
         if (*(fmt_str + 1) == '%') {
            debug_writeb('%');
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
                  debug_writeb('0');
                  debug_writeb('x');
                  fill_hex_int(va_arg(ap, unsigned int));
                  break;
               case 's':
                  fill_string(va_arg(ap, char *));
                  break;
               case 'c':
                  debug_writeb ((unsigned char)va_arg(ap, signed long));
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
         debug_writeb('\r');
         debug_writeb(*fmt_str++);
      }
      else {
         debug_writeb(*fmt_str++);
      }
   }
   va_end(ap);
   
   return 0;
}



/* This function inits Boot ROM debug engine in all environments */
void debug_init(int isUartDbg)
{
#ifdef CONFIG_BUFFER_DEBUG_MSG
   dbg_buf_ptr = 0;
   dbg_buf_enabled = 0;
   dbg_buf_flushing = 0;
#endif

/*******************************************/
/*********  For BootROM release  ***********/
/*******************************************/
   dbg_uart = NULL;

   if (isUartDbg)
   {
      dbg_uart = uart_debug_init();
   }
}


/* Byte write primitive for all supported environments */
void debug_writeb(unsigned char byte)
{
#ifdef VCS
   // VCS simulation write to special address location
   DEBUG_VCS_PRINT_BYTE_VAL = byte;
   return;
#endif

#ifdef CONFIG_BUFFER_DEBUG_MSG
   if (dbg_buf_enabled)
   {
      if(!dbg_buf_flushing)
      {
         if (dbg_buf_ptr < DEBUG_BUFFER_SIZE)
           dbg_buf[dbg_buf_ptr++] = byte;
      }
      return;
   }
#endif
/*******************************************/
/*********  For BootROM release  ***********/
/*******************************************/
   // Enable UART debug according to strap pin
   if(dbg_uart)
   {
      uart_write(dbg_uart, byte);
   }
}

void debug_buffer_enable(int enable)
{
#ifdef CONFIG_BUFFER_DEBUG_MSG
   dbg_buf_enabled = enable;
#endif   
}

void debug_flush_buffer()
{
#ifdef CONFIG_BUFFER_DEBUG_MSG
   unsigned int i;
   if ((!dbg_buf_enabled) || (!dbg_uart))
      return;

   dbg_buf_flushing =1;
   for (i=0; i< MIN(dbg_buf_ptr,DEBUG_BUFFER_SIZE); i++)
   {
      uart_write(dbg_uart, dbg_buf[i]);
   }
   dbg_buf_ptr = 0;
   dbg_buf_flushing=0;
   
   last_flush_time = system_get_tick_count();
#endif
}

void debug_check_for_flush()
{
#ifdef CONFIG_BUFFER_DEBUG_MSG
	unsigned int curr_diff_time;
	unsigned int curr_diff_time_ms;
	unsigned int curr_time;

   if (!dbg_buf_enabled)
      return;

   curr_time = system_get_tick_count();
   if(curr_time < last_flush_time)
      last_flush_time = 0; //instead of computing wrap-around

   curr_diff_time = curr_time - last_flush_time;
   curr_diff_time_ms = curr_diff_time / system_ticks_per_msec();
   if (curr_diff_time_ms > DEBUG_BUFFER_FLUSH_MSEC)
      debug_flush_buffer();
#endif
}
/* Used for quick code debug (legacy from Inomize environment) */
void debug_code(unsigned int code)
{
#ifdef VCS
	DEBUG_VCS_PRINT_BYTE_VAL = code;
	return;
#endif

  debug_writeb(code & 0xff);
  debug_writeb((code>>8) & 0xff);
  debug_writeb((code>>16) & 0xff);
  debug_writeb((code>>24) & 0xff);
}
