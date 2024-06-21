/****************************************************************************
 *
 *    FileName: eeprom_ctl.c
 *
 *    Author:  Oshri A.
 *
 *    Date:
 *
 *    Description: eeprom control
 *
 ****************************************************************************/

#ifndef EEPROM_CTRL_H
#define EEPROM_CTRL_H

#ifdef __cplusplus
        extern "C" {
#endif

/****************************************************************************
 ***************                    I N C L U D E    F I L E S          *************
 ****************************************************************************/
#include "inu_common.h"

/****************************************************************************
 ***************      G L O B A L          D E F N I T I O N S     **************
 ****************************************************************************/
typedef struct
{
    UINT32 i2cNum;
    UINT32 i2cData;
    UINT32 regWidth;
    UINT32 regAddress;
    UINT32 tarAddress;
    UINT32 accessSize;
    UINT32 eepromSize;
} EEPROM_CTRLG_eepromInfoT;

/****************************************************************************
 ***************        G L O B A L            T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************         G L O B A L         D A T A                  ***************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L            F U N C T I O N S     ***************
 *************************************************************************/
ERRG_codeE EEPROM_CTRLG_getEepromInfo(INU_DEFSG_senSelectE sensorId, EEPROM_CTRLG_eepromInfoT *eepromInfo);
ERRG_codeE EEPROM_CTRLG_readFromEeprom(INU_DEFSG_senSelectE sensorId, UINT32 offset, UINT32 size, UINT8 *data);
ERRG_codeE EEPROM_CTRLG_writeToEeprom(INU_DEFSG_senSelectE sensorId, UINT32 offset, UINT32 size, UINT8* data);

#ifdef __cplusplus
}
#endif

#endif //    EEPROM_CTRL_H


