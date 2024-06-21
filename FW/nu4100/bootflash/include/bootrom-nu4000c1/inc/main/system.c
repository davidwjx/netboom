/*
 *
	system.c
	Implementation of basic system calls

*/

#include "common.h"

#include "gme.h"
#include "apb_timer.h"
#include "nu_regs.h"

/**********************************************
 * local defines
***********************************************/

/**********************************************
 * local typedefs
***********************************************/
typedef void (*void_func)(void);

/**********************************************
 * local data
***********************************************/
const unsigned long max_delay_in_ticks;

/**********************************************
 * pre-definition local functions
***********************************************/

static unsigned int get_tick_count(UINT32 loadCnt);
/**********************************************
 * functions
***********************************************/

/*************************************************************************
 * Routine: get_tick_count
 *   Synopsis timer counts down.
 *   In get_tick_count we reverse the order so it counts up, emulating OS ticks.
 *   Warning: The tick wrap around issue should be treated in upper levels
 *
 *************************************************************************/
static unsigned int get_tick_count(UINT32 loadCnt)
{
   return (loadCnt - APB_TIMERG_read());
}

/*************************************************************************
 * Routine: system_tick_delay
 *   Main timer wait routine, that deals with timer wrap around also
 *
 *************************************************************************/
int system_tick_delay(unsigned int delay_in_ticks,  int (*do_function) (void))
{
   unsigned int timer_ticks;
   unsigned int prev_timer_ticks;
   unsigned int timer_start_ticks;
   int timeout_in_ticks;
   unsigned int load_cnt;

   if(!APB_TIMERG_isStarted())
   {
      debug_log(1,"error: %s called with timer off\n", __func__);
      return 0;
   }
   
   timeout_in_ticks = delay_in_ticks;

   load_cnt = APB_TIMERG_getLoadCnt();
   timer_start_ticks = get_tick_count(load_cnt);
   timer_ticks = timer_start_ticks;
   
   do
   {      
      prev_timer_ticks = timer_ticks;
      timer_ticks = get_tick_count(load_cnt);
      
      // Check for timer wrap
      if(timer_ticks < prev_timer_ticks)
      {
         timeout_in_ticks -= load_cnt - timer_start_ticks + timer_ticks;
         timer_start_ticks = timer_ticks;      
      }
      
      if (do_function != NULL && do_function())
      { 
         return 1;
      }      
      
   }while(((int)(timer_ticks - timer_start_ticks)) < timeout_in_ticks);

   return 0;
       
}

 #ifdef B0_TEST
 /*************************************************************************
  * Routine: system_get_tick_count
 *   Get current synopsis timer count (in reverse, so it counts up)
 *   Warning: Does not handle wrap-around. Use for debugging only.
 *
  *************************************************************************/
unsigned int system_get_tick_count(void)
{
   return APB_TIMERG_getLoadCnt() -APB_TIMERG_read() ;
}
#endif

void system_udelay(int us)
{
	unsigned int ticks_in_us;
	ticks_in_us = ((get_board_info()->apb_timer_freq_hz / 1000) / 1000);

	system_tick_delay(us * ticks_in_us, NULL);
}

/**
  Return the bootrom SW version.
  
 */
void system_getSwVersion(unsigned int *major, unsigned int *minor, unsigned int *build)
{
   extern unsigned int _major_version;  
   extern unsigned int _minor_version;  
   extern unsigned int _build_version;
   *major = _major_version;
   *minor = _minor_version;
   *build = _build_version;
}


/**
  Return the bootrom HW version.
  HW Version is a 32-bit integer.
*/
unsigned int system_getHwVersion()
{
   return GMEG_getVersion();
}


void system_mdelay(int ms)
{
	unsigned int i;
	for (i=0; i< ms; i++)
	{
		system_udelay(1000);
	}
}


unsigned int system_ticks_per_msec(void)
{
	return (get_board_info()->apb_timer_freq_hz/1000);
}

void system_hard_reset(void)
{
   APB_TIMERG_wdtReset();
   while(1){asm volatile("nop");};
}

void system_power_mode(int mode)
{
   rel_log("sp\n");
   GMEG_setSuspendMode(mode);
}

int system_is_self_powered(void)
{
   return 0;
}

void system_usb_typec_orientation(void)
{
   board_usb_typec_orientation(get_strap_info()->typecInvert);
}

extern void invalidate_icache_all(void);
extern void dcache_enable(void);
extern void icache_enable(void);
void system_jump_to_address(unsigned int address)
{
	// Ensure to invalidate the i-cache before jump to CRAM/DDR
	invalidate_icache_all();

	((void_func)address)();
}

void enable_caches()
{
//#if !(defined(CONFIG_SYS_DCACHE_OFF))
   dcache_enable();
   icache_enable();
//#endif
}

void disable_caches()
{
//#if !(defined(CONFIG_SYS_DCACHE_OFF))
   dcache_disable();
   icache_disable();
//#endif
}
