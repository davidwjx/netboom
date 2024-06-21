/****************************************************************************
 *
 *   FileName: inu_mem_def.h
 *
 *   Author: Arnon C
 *   *
 *   Description: Common memory mapping
 *
 *****************************************************************************/
#ifndef INU_MEM_DEF_H
#define INU_MEM_DEF_H

#define LRAM_SIZE                         (384*1024)
#define MEM_MAPG_CEVA_BOOT_PARAMS_SIZE    (8)
#define MEM_MAPG_CEVA_BOOT_PARAMS_ADDRESS (LRAM_BASE + LRAM_SIZE - MEM_MAPG_CEVA_BOOT_PARAMS_SIZE)
#define MEM_MAPG_CRAM_CP_MUTEX_SIZE       (0x100)
#define MEM_MAPG_CP_MUTEX_ADDRESS         (MEM_MAPG_CEVA_BOOT_PARAMS_ADDRESS - MEM_MAPG_CRAM_CP_MUTEX_SIZE)
#define MEM_MAPG_CEVA_LOG_SIZE            (0x3700)
#define MEM_MAPG_CEVA_LOG_ADDRESS         (MEM_MAPG_CP_MUTEX_ADDRESS - MEM_MAPG_CEVA_LOG_SIZE)
#define MEM_MAPG_EV61_LOG_SIZE            (0x3700)
#define MEM_MAPG_EV61_LOG_ADDRESS         (MEM_MAPG_CEVA_LOG_ADDRESS - MEM_MAPG_EV61_LOG_SIZE)

#endif

