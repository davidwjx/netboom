
#ifndef _SPL_H_
#define _SPL_H_

#include "ddr_app.h"

/*****************************************/
/*   Memory layout for boot              */
/*****************************************/

#define DDR_START_ADDRESS               (0x40000000) //0x80000000
#define DDR_KERNEL_START_ADDRESS        (DDR_START_ADDRESS + TEXT_OFFSET)
#define MAX_KERNEL_SIZE                 (0x400000) // for cache table
#define DDR_DTB_START_ADDRESS           (DDR_START_ADDRESS + DTB_OFFSET)
#define DDR_APP_START_ADDRESS           (DDR_START_ADDRESS + APP_OFFSET)
#define DDR_CEVA_START_ADDRESS          (DDR_START_ADDRESS + CEVA_OFFSET - 380)
//Assume that initramfs is less than 16Mb
#define DDR_INITRAMFS_START_ADDRESS     (DDR_START_ADDRESS+ INITRAMFS_OFFSET)
#define MAX_INITRAMFS_SIZE              (0x2000000) // for cache table
//static - depends on the CNN linker script configuration
#define DDR_CNN_LRAM_BCK_START_ADDRESS  (0x47b00000) // in mem_map MEM_MAPG_DDR_LRAM_BACKUP_E
#define DDR_CNN_BCK_START_ADDRESS       (0x47b10000) // in mem_map MEM_MAPG_DDR_EV62_BACKUP_CODE_E
#define DDR_CNN_START_ADDRESS           (0x48000000) // in mem_map MEM_MAPG_DDR_EV62_CODE_E

#define LRAM_START_ADDRESS              (0x02000000)
#define LRAM_TOTAL_SIZE                 (0x60000)

#define DDR_CNN_LRAM_BCK_SIZE           (0x10000)    // in size of MEM_MAPG_DDR_LRAM_BACKUP_E

#define INUITIVE_MACH_TYPE              (0x1000)

/*****************************************/
/*   BootSPL interface                   */
/*****************************************/
void spl_wd_init();
void spl_board_lowlevel_init();
void spl_board_init();
void spl_spi_board_init();
void spl_ddr_init();
void spl_usb_boot();
void spl_boot_linux();
int uart0_printf(const char *fmt_str, ...);
#define SPL_PRINT
#ifdef SPL_PRINT
/* Use this function to print release messages */
#if defined(BOOT_86) || defined(BOOT_87)
#define spl_log(fmt, ...)  \
   do { \
       uart0_printf(fmt, ##__VA_ARGS__); \
       debug_printf(fmt, ##__VA_ARGS__); \
   }while(0)

#else
#define spl_log(fmt, ...)  \
   do { \
       debug_printf(fmt, ##__VA_ARGS__); \
   }while(0)
#endif // BOOT_86 BOOT_87
#else
#define spl_log(fmt, ...)
#endif
#endif
