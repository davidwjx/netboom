/****************************************************************************
 *
 *   FileName: hw_regs.h
 *
 *   Author: Yaron A. & Eyal A.
 *
 *   Date:
 *
 *   Description: peripherals control
 *
 ****************************************************************************/
#ifndef HW_REGS_H
#define HW_REGS_H

#include "inu_common.h"
#include "inu2.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "io_pal.h"

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void (*HW_REGSG_customPhaseCbT)(inu_device__writeRegT *writeRegIoctlParamsP);
typedef void (*HW_REGSG_clearRegsPhaseCbT)();

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE  HW_REGSG_init();
ERRG_codeE  HW_REGSG_close();

ERRG_codeE  HW_REGSG_insertParam2Table(inu_device__writeRegT *writeRegIoctlParamsP);

ERRG_codeE  HW_REGSG_ClearSenTable();
ERRG_codeE  HW_REGSG_activateSenTable(inu_device__dbPhaseE phase);

ERRG_codeE  HW_REGSG_ClearHwTable();
ERRG_codeE  HW_REGSG_activateHwTable(inu_device__dbPhaseE phase);

ERRG_codeE  HW_REGSG_clearI2cTable();
ERRG_codeE  HW_REGSG_activateI2cTable(inu_device__dbPhaseE phase);

ERRG_codeE  HW_REGSG_clearWaitTable();
ERRG_codeE  HW_REGSG_activateWaitTable(inu_device__dbPhaseE phase);

ERRG_codeE  HW_REGSG_clearCommandTable();
ERRG_codeE  HW_REGSG_activateCommandTable(inu_device__dbPhaseE phase);

ERRG_codeE  HW_REGSG_writeSocReg(UINT32 physicalAddr, UINT32 val);
ERRG_codeE HW_REGSG_readSocReg(UINT32 physicalAddr, UINT32 *regValP);

ERRG_codeE  HW_REGSG_writeI2cReg(inu_device__i2cNumE i2cNum, UINT32 regAddr, UINT8 tarAddress, UINT8 accessSize, UINT32 regWidth, UINT32 val);
UINT32      HW_REGSG_readI2cReg(inu_device__i2cNumE i2cNum, UINT32 regAddr, UINT8 tarAddress, UINT8 accessSize, UINT32 regWidth);

ERRG_codeE  HW_REGSG_readReg(inu_device__readRegT *readRegIoctlParamsP);
ERRG_codeE  HW_REGSG_writeReg(inu_device__writeRegT *writeRegIoctlParamsP);
void        HW_REGSG_customPhaseCb(HW_REGSG_customPhaseCbT PhaseCb, HW_REGSG_clearRegsPhaseCbT clearRegsPhaseCb);
ERRG_codeE  HW_REGSG_dbControl(INUG_ioctlDbControlT *dbControlParamsP);
ERRG_codeE HW_REGSG_readSocRegVolatile(UINT32 physicalAddr,volatile UINT32 *regValP);

#ifdef __cplusplus
}
#endif

#endif //   HW_REGS_H

