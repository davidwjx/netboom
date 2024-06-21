/****************************************************************************
 *
 *   FileName: pwr_mngr.h
 *
 *   Author: Oshri A
 *
 *   Date:
 *
 *   Description: Power Manager
 *
 ****************************************************************************/
#ifndef _PWR_MNGR_H_
#define _PWR_MNGR_H_

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "inu_defs.h"
#include "inu2.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define PWR_MNGRG_ENABLE_PD

typedef enum
{
   PWR_MNGRG_INTERNAL_SUSPEND_E  = 0,
   PWR_MNGRG_USB_U3_SUSPEND_E    = 1,
   PWR_MNGRG_POWER_DOWN_E        = 2,
} PWR_MNGRG_SLEEP_MODE_E;

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE PWR_MNGRG_prepareToPowerDown(inu_deviceH deviceH, PWR_MNGRG_SLEEP_MODE_E mode);
ERRG_codeE PWR_MNGRG_wakeUpRecovery(inu_deviceH deviceH);
ERRG_codeE PWR_MNGRG_waitForEvent(inu_deviceH deviceH);


#ifdef __cplusplus
}
#endif

#endif //   _PWR_MNGR_H_

