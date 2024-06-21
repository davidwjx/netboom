/****************************************************************************
 *
 *   FileName: pss_drv.h
 *
 *   Author: 
 *
 *   Date:
 *
 *   Description: PSS  driver
 *
 ****************************************************************************/
#ifndef PSS_DRV_H
#define PSS_DRV_H

ERRG_codeE PSS_DRVG_arcControl(UINT32 value);
ERRG_codeE PSS_DRVG_init(UINT32 memVirtAddr);
ERRG_codeE PSS_DRVG_dumpRegs();
/**
 * @brief Enables the ELU without debouncing so that external synchronization signals can me used in the FSG
 * 
 * 
 *
 * @return Returns an error code
 */
ERRG_codeE PSS_DRVG_enableWithoutDebouncing();

#endif

