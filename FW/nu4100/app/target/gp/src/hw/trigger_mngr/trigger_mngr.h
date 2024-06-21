/****************************************************************************
 *
 *   FileName: 
 *
 *   Author:  Arnon C.
 *
 *   Date:
 *
 *   Description:
 *
 ****************************************************************************/
#ifndef TRIGGER_MNGR_H
#define TRIGGER_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "sensors_mngr.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define  TRIGGER_MNGR__NUM_SUPPORTED_SENSOR 5
/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void* trigger_mngrH;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE TRIGGER_MNGRG_init();
ERRG_codeE TRIGGER_MNGRG_deinit();
ERRG_codeE TRIGGER_MNGRG_startTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo, trigger_mngrH *triggerH);
ERRG_codeE TRIGGER_MNGRG_stopTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo);
ERRG_codeE TRIGGER_MNGRG_manageStartTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo, trigger_mngrH *triggerH);
ERRG_codeE TRIGGER_MNGRG_manageStopTrigger(SENSORS_MNGRG_sensorInfoT *sensorInfo);
void       TRIGGER_MNGRG_configPwm(unsigned int pwmTrigNum,unsigned int pwmFps,unsigned int pwmUsecWidth, unsigned int pwmCmd);
ERRG_codeE TRIGGER_MNGRG_changeFps(trigger_mngrH triggerH, UINT32 fps);

#ifdef __cplusplus
}
#endif

#endif //   TRIGGER_MNGR_H

