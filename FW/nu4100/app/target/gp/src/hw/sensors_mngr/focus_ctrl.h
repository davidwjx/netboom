/****************************************************************************
 *
 *   FileName: focus_ctrl.h
 *
 *   Author:  Oshri A.
 *
 *   Date:
 *
 *   Description: focus control
 *
 ****************************************************************************/

#ifndef FOCUS_CTRL_H
#define FOCUS_CTRL_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE FOCUS_CTRLG_isFocusCtlSupported(SENSORS_MNGRG_sensorInfoT* sensorInfo);
ERRG_codeE FOCUS_CTRLG_getFocusParams(IO_HANDLE sensorHandle, inu_sensor__getFocusParams_t *paramsP);
ERRG_codeE FOCUS_CTRLG_setFocusParams(IO_HANDLE sensorHandle, inu_sensor__setFocusParams_t *paramsP);


#ifdef __cplusplus
}
#endif

#endif //   FOCUS_CTRL_H

