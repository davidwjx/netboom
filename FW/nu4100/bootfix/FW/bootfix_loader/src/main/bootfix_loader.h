#ifndef BOOTFIX_LOADER_H
#define BOOTFIX_LOADER_H
/**********************************************
 * local defines
***********************************************/
#define DDR_START_ADDRESS  (0x80000000)
#define DDR_SIZE (0x04000000) //64 MB
#define SPL_DDR_INIT_DONE   (0x1)
#define EFUSE_SIZE (0x200)
#define ENABLE_M3_DEBUG 1
#define ALLIGN_BOOTFIX 0

typedef void (*void_func)(void);

extern void invalidate_icache_all(void);
#endif

