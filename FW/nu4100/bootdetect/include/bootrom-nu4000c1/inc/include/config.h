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
 * NOTE: This build  configuration are defined in shell environment before make execution
 *       env VCS=1 make / VCS=1 make (for tcshl and bash respectively)
 ************************************************************************
 */

#ifdef RELEASE

   /* sanity check */
   #undef VCS
   /* #undef USB debug defines */
   #undef CONFIG_BUFFER_DEBUG_MSG
   /* Disable debug messages (dbg_log/err_log/debug_code)*/
   #define BOOTROM_DEBUG_LEVEL 0
   /* Disable DCACHE and MMU setup */
   //#define CONFIG_SYS_DCACHE_OFF
   /* Disable I-cache in release */
//   #define CONFIG_SYS_ICACHE_OFF

#elif defined(VCS)

   /* Enable ALL debug messages */
   #define BOOTROM_DEBUG_LEVEL 0xFFFF
   /* Disable init of BSS*/
   //#define CONFIG_BSS_INIT_OFF
   /* Disable DCACHE and MMU setup */
   //#define CONFIG_SYS_DCACHE_OFF
   /* I-CACHE disable */
   //#define CONFIG_SYS_ICACHE_OFF

#elif defined(B0_TEST)
   /* Enable ALL debug messages */
   #define BOOTROM_DEBUG_LEVEL 0x4
   /* Disable init of BSS*/
   //#define CONFIG_BSS_INIT_OFF
   /* Disable DCACHE and MMU setup */
   //#define CONFIG_SYS_DCACHE_OFF
   /* I-CACHE disable */
   //#define CONFIG_SYS_ICACHE_OFF
   
#else
#error "No environment variable defined"
#endif


#endif /* _CONIFG_H_ */
