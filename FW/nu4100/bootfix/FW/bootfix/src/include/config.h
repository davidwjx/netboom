/*
 * config.h
 *
 * Common defines for bootrom
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*
 * ************************************************************************
 * Define build configurations per Environment:
 * 			 RELEASE	- Inuitive RELEASE mode - READY for PRODUCTION
 * 			 VCS 		- Inuitive NU4000 VCS Simulation (extension of Inomize simulation)
 *          NU3K_TEST - Use nu3000b0 chip to test nu4000 bootrom
 * NOTE: This build  configuration are defined in shell environment before make execution
 *       env VCS=1 make / VCS=1 make (for tcshl and bash respectively)
 ************************************************************************
 */   
 
/* #undef USB debug defines */
#undef CONFIG_BUFFER_DEBUG_MSG
/* Disable debug messages (dbg_log/err_log/debug_code)*/
#define BOOTROM_DEBUG_LEVEL 0
/* Disable DCACHE and MMU setup */
//#define CONFIG_SYS_DCACHE_OFF
/* Disable I-cache in release */
//#define CONFIG_SYS_ICACHE_OFF

#endif /* _CONIFG_H_ */
