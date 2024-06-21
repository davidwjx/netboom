/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 * Aneesh V <aneesh@ti.com>
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
#include <asm/armv7.h>

#define ARMV7_DCACHE_INVAL_ALL		1
#define ARMV7_DCACHE_CLEAN_INVAL_ALL	2
#define ARMV7_DCACHE_INVAL_RANGE	3
#define ARMV7_DCACHE_CLEAN_INVAL_RANGE	4

static signed int log_2_n_round_up(unsigned int n)
{
	signed int log2n = -1;
	unsigned int temp = n;

	while (temp) {
		log2n++;
		temp >>= 1;
	}

	if (n & (n - 1))
		return log2n + 1; /* not power of 2 - round up */
	else
		return log2n; /* power of 2 */
}

/*
 * Write the level and type you want to Cache Size Selection Register(CSSELR)
 * to get size details from Current Cache Size ID Register(CCSIDR)
 */
static void set_csselr(unsigned int level, unsigned int type)
{	unsigned int csselr = level << 1 | type;

	/* Write to Cache Size Selection Register(CSSELR) */
	asm volatile ("mcr p15, 2, %0, c0, c0, 0" : : "r" (csselr));
}

static unsigned int get_ccsidr(void)
{
	unsigned int ccsidr;

	/* Read current CP15 Cache Size ID Register */
	asm volatile ("mrc p15, 1, %0, c0, c0, 0" : "=r" (ccsidr));
	return ccsidr;
}

static unsigned int get_clidr(void)
{
	unsigned int clidr;

	/* Read current CP15 Cache Level ID Register */
	asm volatile ("mrc p15,1,%0,c0,c0,1" : "=r" (clidr));
	return clidr;
}

static void v7_inval_dcache_level_setway(unsigned int level, unsigned int num_sets,
					 unsigned int num_ways, unsigned int way_shift,
					 unsigned int log2_line_len)
{
	int way, set, setway;

	/*
	 * For optimal assembly code:
	 *	a. count down
	 *	b. have bigger loop inside
	 */
	for (way = num_ways - 1; way >= 0 ; way--) {
		for (set = num_sets - 1; set >= 0; set--) {
			setway = (level << 1) | (set << log2_line_len) |
				 (way << way_shift);
			/* Invalidate data/unified cache line by set/way */
			asm volatile ("	mcr p15, 0, %0, c7, c6, 2"
					: : "r" (setway));
		}
	}
	/* DSB to make sure the operation is complete */
	CP15DSB;
}

static void v7_clean_inval_dcache_level_setway(unsigned int level, unsigned int num_sets,
					       unsigned int num_ways, unsigned int way_shift,
					       unsigned int log2_line_len)
{
	int way, set, setway;

	/*
	 * For optimal assembly code:
	 *	a. count down
	 *	b. have bigger loop inside
	 */
	for (way = num_ways - 1; way >= 0 ; way--) {
		for (set = num_sets - 1; set >= 0; set--) {
			setway = (level << 1) | (set << log2_line_len) |
				 (way << way_shift);
			/*
			 * Clean & Invalidate data/unified
			 * cache line by set/way
			 */
			asm volatile ("	mcr p15, 0, %0, c7, c14, 2"
					: : "r" (setway));
		}
	}
	/* DSB to make sure the operation is complete */
	CP15DSB;
}

static void v7_maint_dcache_level_setway(unsigned int level, unsigned int operation)
{
	unsigned int ccsidr;
	unsigned int num_sets, num_ways, log2_line_len, log2_num_ways;
	unsigned int way_shift;

	set_csselr(level, ARMV7_CSSELR_IND_DATA_UNIFIED);

	ccsidr = get_ccsidr();

	log2_line_len = ((ccsidr & CCSIDR_LINE_SIZE_MASK) >>
				CCSIDR_LINE_SIZE_OFFSET) + 2;
	/* Converting from words to bytes */
	log2_line_len += 2;

	num_ways  = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK) >>
			CCSIDR_ASSOCIATIVITY_OFFSET) + 1;
	num_sets  = ((ccsidr & CCSIDR_NUM_SETS_MASK) >>
			CCSIDR_NUM_SETS_OFFSET) + 1;
	/*
	 * According to ARMv7 ARM number of sets and number of ways need
	 * not be a power of 2
	 */
	log2_num_ways = log_2_n_round_up(num_ways);

	way_shift = (32 - log2_num_ways);
	if (operation == ARMV7_DCACHE_INVAL_ALL) {
		v7_inval_dcache_level_setway(level, num_sets, num_ways,
				      way_shift, log2_line_len);
	} else if (operation == ARMV7_DCACHE_CLEAN_INVAL_ALL) {
		v7_clean_inval_dcache_level_setway(level, num_sets, num_ways,
						   way_shift, log2_line_len);
	}
}

static void v7_maint_dcache_all(unsigned int operation)
{
	unsigned int level, cache_type, level_start_bit = 0;

	unsigned int clidr = get_clidr();

	for (level = 0; level < 7; level++) {
		cache_type = (clidr >> level_start_bit) & 0x7;
		if ((cache_type == ARMV7_CLIDR_CTYPE_DATA_ONLY) ||
		    (cache_type == ARMV7_CLIDR_CTYPE_INSTRUCTION_DATA) ||
		    (cache_type == ARMV7_CLIDR_CTYPE_UNIFIED))
			v7_maint_dcache_level_setway(level, operation);
		level_start_bit += 3;
	}
}


void invalidate_dcache_all(void)
{
	v7_maint_dcache_all(ARMV7_DCACHE_INVAL_ALL);

	v7_outer_cache_inval_all();
}


/*
 * Flush range from all levels of d-cache/unified-cache used:
 * Affects the range [start, start + size - 1]
 */
/*
 * Stub implementations for outer cache operations
 */
void __v7_outer_cache_inval_all(void)
{
}
void v7_outer_cache_inval_all(void)
	__attribute__((weak, alias("__v7_outer_cache_inval_all")));

