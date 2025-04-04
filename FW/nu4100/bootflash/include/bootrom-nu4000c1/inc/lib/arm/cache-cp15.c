/*
 * (C) Copyright 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <config.h>
#include <asm/system.h>
#include "nu_regs.h"

//TODO: move this to header file
extern void flush_dcache_all(void);

#define ALIGN(x,a)              __ALIGN_MASK((x),(typeof(x))(a)-1)
#define __ALIGN_MASK(x,mask)    (((x)+(mask))&~(mask))

#if !(defined(CONFIG_SYS_ICACHE_OFF) && defined(CONFIG_SYS_DCACHE_OFF))

extern unsigned int __tlb_start__;
unsigned int tlb_addr = (unsigned int)&__tlb_start__;

static void cp_delay (void)
{
	volatile int i;

	/* copro seems to need some delay between reading and writing */
	for (i = 0; i < 100; i++)
		nop();
	asm volatile("" : : : "memory");
}

void set_section_dcache(int section, enum dcache_option option)
{
	unsigned int *page_table = (unsigned int *)tlb_addr;
	unsigned int value;

	value = (section << MMU_SECTION_SHIFT) | (3 << 10);
	value |= option;
	page_table[section] = value;
}

void __mmu_page_table_flush(unsigned long start, unsigned long stop)
{
	//debug("%s: Warning: not implemented\n", __func__);
}

void mmu_page_table_flush(unsigned long start, unsigned long stop)
	__attribute__((weak, alias("__mmu_page_table_flush")));

void mmu_set_region_dcache_behaviour(unsigned int start, int size,
				     enum dcache_option option)
{
	unsigned int *page_table = (unsigned int *)tlb_addr;
	unsigned int upto, end;

	end = ALIGN(start + size, MMU_SECTION_SIZE) >> MMU_SECTION_SHIFT;
	start = start >> MMU_SECTION_SHIFT;
	/*debug("%s: start=%x, size=%x, option=%d\n", __func__, start, size,
	      option);*/
	for (upto = start; upto < end; upto++)
		set_section_dcache(upto, option);
	mmu_page_table_flush((unsigned int)&page_table[start], (unsigned int)&page_table[end]);
}

void bank_mmu_setup(int address)
{
   int   i;
   i = address >> 20;

#if defined(CONFIG_SYS_ARM_CACHE_WRITETHROUGH)
      set_section_dcache(i, DCACHE_WRITETHROUGH);
#else
      set_section_dcache(i, DCACHE_WRITEBACK);
#endif
}

/* to activate the MMU we need to set up virtual memory: use 1M areas */
static inline void mmu_setup(void)
{
   int i;
   unsigned int reg;

   /* Set up an identity-mapping for all 4GB, rw for everyone */
   for (i = 0; i < 4096; i++)
      set_section_dcache(i, DCACHE_OFF);

   //Enable ROM section
   bank_mmu_setup(0x0);
   //Enable LRAM section
   bank_mmu_setup(LRAM_BASE);

   /* Copy the page table address to cp15 */
   asm volatile("mcr p15, 0, %0, c2, c0, 0"
           : : "r" (tlb_addr) : "memory");
   /* Set the access control to all-supervisor */
   asm volatile("mcr p15, 0, %0, c3, c0, 0"
           : : "r" (~0));

   /* and enable the mmu */
   reg = get_cr();   /* get control reg. */
   cp_delay();
   set_cr(reg | CR_M);
}

static int mmu_enabled(void)
{
	return get_cr() & CR_M;
}

#ifdef B0_TEST
#include "debug.h"
static unsigned int get_clidr(void)
{
   unsigned int clidr;

   /* Read current CP15 Cache Level ID Register */
   asm volatile ("mrc p15,1,%0,c0,c0,1" : "=r" (clidr));
   return clidr;
}

void getCPUstatus()
{
   unsigned int reg;
   unsigned int clidr = get_clidr();
   reg = get_cr();   /* get control reg. */
   rel_log("lvl: %x, cntrl: %x \n",clidr,reg);
}
#endif

/* cache_bit must be either CR_I or CR_C */
static void cache_enable(unsigned int cache_bit)
{
	unsigned int reg;

	/* The data cache is not active unless the mmu is enabled too */
	if ((cache_bit == CR_C) && !mmu_enabled())
		mmu_setup();
	reg = get_cr();	/* get control reg. */
	cp_delay();
	set_cr(reg | cache_bit);
}

/* cache_bit must be either CR_I or CR_C */
static void cache_disable(unsigned int cache_bit)
{
	unsigned int reg;

	reg = get_cr();
	cp_delay();

	if (cache_bit == CR_C) {
		/* if cache isn;t enabled no need to disable */
		if ((reg & CR_C) != CR_C)
			return;
		/* if disabling data cache, disable mmu too */
		cache_bit |= CR_M;
	}
	reg = get_cr();
	cp_delay();
	if (cache_bit == (CR_C | CR_M))
		flush_dcache_all();
	set_cr(reg & ~cache_bit);
}
#endif

#ifdef CONFIG_SYS_ICACHE_OFF
void icache_enable (void)
{
	return;
}

void icache_disable (void)
{
	return;
}

int icache_status (void)
{
	return 0;					/* always off */
}
#else
void icache_enable(void)
{
	cache_enable(CR_I);
}

void icache_disable(void)
{
	cache_disable(CR_I);
}

int icache_status(void)
{
	return (get_cr() & CR_I) != 0;
}
#endif

#ifdef CONFIG_SYS_DCACHE_OFF
void dcache_enable (void)
{
	return;
}

void dcache_disable (void)
{
	return;
}

int dcache_status (void)
{
	return 0;					/* always off */
}
#else
void dcache_enable(void)
{
	cache_enable(CR_C);
}

void dcache_disable(void)
{
	cache_disable(CR_C);
}

int dcache_status(void)
{
	return (get_cr() & CR_C) != 0;
}
#endif
