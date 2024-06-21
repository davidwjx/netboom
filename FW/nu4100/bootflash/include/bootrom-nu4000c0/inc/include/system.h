/*
 * system.h
 *
 */
#ifndef _SYSTEM_H_
#define _SYSTEM_H_

#define SECTION_PBSS __attribute__((__section__(".pbss")))
#define PACKED       __attribute__ ((__packed__))

typedef struct
{
   //boot strap values
   int bootMode;   //boot mode
   int isUartDbg;  //Enable UART debug output or not
   int typecInvert; // 0 not inverted,1-inverted
} strap_info_t;


#define  SYSTEM_POWER_SUSPEND_USB3   (1)
#define  SYSTEM_POWER_SUSPEND_USB2   (2)

int system_tick_delay(unsigned int delay_in_ticks,  int (*do_function) (void));
unsigned int system_ticks_per_msec(void);
void system_getSwVersion(unsigned int *major, unsigned int *minor, unsigned int *build);
unsigned int system_getHwVersion();
void system_jump_to_address(unsigned int address);

/**
  Returns strap info for the bootrom
 */
const strap_info_t *get_strap_info(void);


/**
  sleep for the given amount of microseconds
 */
void system_udelay(int us);

/**
  sleep for the given amount of milliseconds
 */
void system_mdelay(int ms);

/**
  reset the chip (and then start running ROM from the beginning)
 */
void system_hard_reset(void);

/**
  return 1 if we have a self power source, 0 if bus powered only.
 */
int system_is_self_powered(void);

/**
  move the system to a given power mode
  the following modes are defined:
  0 - full power.
  1 - suspend mode.
 */
void system_power_mode(int mode);
void system_resume(void);
/**
   Do the usb phy typec orientation sequence.
   Call from usb controller driver after usb phy reset and before connecting on the bus
*/
void system_usb_typec_orientation(void);

int interrupt_init(void);
void enable_interrupts(void);
void enable_async_abort(void);
int disable_interrupts (void);
/* Enable D-cache */
void enable_caches();
void disable_caches();

/**
  register an ISR for a given IRQ. enable that IRQ.
 */
void irq_register(int intNum, void (*isr)(void));
/**
  acknowledge an IRQ to the interrupt controller, to clear it.
 */
void irq_clear(int intNum);
/**
  disable an IRQ (or at least - don't call the previously registered ISR anymore.
 */
void irq_deregister(int irq);
void irq_mask(int irq);
void irq_unmask(int irq);

#endif /* _SYSTEM_H_ */
