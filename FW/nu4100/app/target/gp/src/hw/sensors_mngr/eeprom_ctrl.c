/****************************************************************************
 *
 *    FileName: eeprom_ctrl.c
 *
 *    Author:  Oshri A.
 *
 *    Date:
 *
 *    Description: eeprom control
 *
 ****************************************************************************/

/****************************************************************************
 ***************                    I N C L U D E    F I L E S          *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
    extern "C" {
#endif

#include "inu2_internal.h"
#include "sensors_mngr.h"
#include "eeprom_ctrl.h"
#include "hw_regs.h"
#include <unistd.h>
#include "assert.h"
#include "helsinki.h"
/****************************************************************************
 ***************            L O C A L         D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************                L O C A L     T Y P E D E F S        ***************
 ****************************************************************************/

/****************************************************************************
 ***************         L O C A L            D A T A                  ***************
 ****************************************************************************/

/****************************************************************************
 ***************         G L O B A L         D A T A                  ***************
 ****************************************************************************/

/****************************************************************************i
 ***************        E X T E R N A L    F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************      P R E     D E F I N I T I O N      OF        ***************
 ***************      L O C A L            F U N C T I O N S        ***************
 ****************************************************************************/


/****************************************************************************
*
*  Function Name: EEPROM_CTRLG_getEepromInfo
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: eeprom control
*
****************************************************************************/
ERRG_codeE EEPROM_CTRLG_getEepromInfo(INU_DEFSG_senSelectE sensorId, EEPROM_CTRLG_eepromInfoT *eepromInfo)
{
    ERRG_codeE                      ret = SENSORS_MNGR__RET_SUCCESS;
    SENSORS_MNGRG_sensorInfoT  *sensorInfo;

    sensorInfo = SENSORS_MNGRG_getSensorInfo(sensorId);
    if (sensorInfo)
    {
        switch(sensorInfo->sensorCfg.sensorModel)
        {
            case INU_DEFSG_SENSOR_MODEL_OV_5675_E:
            {
                eepromInfo->i2cNum = INU_DEVICE__I2C_2_E;
                eepromInfo->regAddress = 0;
                eepromInfo->tarAddress = 0xA8;
                eepromInfo->accessSize = 2;

                eepromInfo->eepromSize = 8192; // EEPROM M24C64X
            }
            break;

            case INU_DEFSG_SENSOR_MODEL_OV_8856_E:
            {
                eepromInfo->i2cNum = INU_DEVICE__I2C_2_E;
                eepromInfo->regAddress = 0;
                eepromInfo->tarAddress = 0xA0;
                eepromInfo->accessSize = 2;

                eepromInfo->eepromSize = 8192; // EEPROM M24C64X
            }
            break;

            case INU_DEFSG_SENSOR_MODEL_AR_2020_E:
            {
                if(HELSINKI_getMaster()) 
                /*The master has the AR2020 on i2c 2, the slave has the AR2020 on i2c 1*/
                {
                eepromInfo->i2cNum = INU_DEVICE__I2C_2_E;
                }
                else
                {
                eepromInfo->i2cNum = INU_DEVICE__I2C_1_E;
                }
                eepromInfo->regAddress = 0;
                eepromInfo->tarAddress = 0xA0;
                eepromInfo->accessSize = 2;

                eepromInfo->eepromSize = 8192; // EEPROM P24C128E
            }
            break;

            default:
            {
                LOGG_PRINT(LOG_ERROR_E, ret, "eeprom not found for sensor %d\n", sensorId);
                ret = SENSORS_MNGR__ERR_NOT_SUPPORTED;
            }
            break;
        }
    }
    else
    {
        ret = SENSORS_MNGR__ERR_INVALID_ARGS;
        LOGG_PRINT(LOG_ERROR_E, ret, "Failed to get sensor %d info\n", sensorId);
    }
    return ret;
}

/****************************************************************************
*
*  Function Name: EEPROM_CTRLG_readFromEeprom
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: eeprom control
*
****************************************************************************/
ERRG_codeE EEPROM_CTRLG_readFromEeprom(INU_DEFSG_senSelectE sensorId, UINT32 offset, UINT32 size, UINT8 *data)
{
    EEPROM_CTRLG_eepromInfoT    eepromInfo;
    ERRG_codeE                      retCode = SENSORS_MNGR__RET_SUCCESS;
    UINT32                            i;

    retCode = EEPROM_CTRLG_getEepromInfo(sensorId, &eepromInfo);
    if (ERRG_FAILED(retCode))
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "Failed to get eeprom info of sensor %d\n", sensorId);
        return retCode;
    }

    if((offset + size) > eepromInfo.eepromSize)
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "offset (%d) and size (%d) exceeding eeprom size (%d)\n", offset, size, eepromInfo.eepromSize);
        return SENSORS_MNGR__ERR_INVALID_ARGS;
    }

    LOGG_PRINT(LOG_INFO_E, NULL, "read EEPROM data of sensor ID %d (i2c %d)\n", sensorId, eepromInfo.i2cNum);

    // get EEPROM data from I2C
    for (i = 0; i < size; i++)
    {
        data[i] = HW_REGSG_readI2cReg(eepromInfo.i2cNum, eepromInfo.regAddress + offset + i, eepromInfo.tarAddress, eepromInfo.accessSize, 1);
    }

    LOGG_PRINT(LOG_INFO_E, NULL, "read EEPROM data from offset %d (%d bytes) done\n", offset, size);

    return retCode;
}

/****************************************************************************
*
*  Function Name: EEPROM_CTRLG_writeToEeprom
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: eeprom control
*
****************************************************************************/
ERRG_codeE EEPROM_CTRLG_writeToEeprom(INU_DEFSG_senSelectE sensorId, UINT32 offset, UINT32 size, UINT8* data)
{
    EEPROM_CTRLG_eepromInfoT    eepromInfo;
    ERRG_codeE                      retCode = SENSORS_MNGR__RET_SUCCESS;
    UINT32                            i;

    retCode = EEPROM_CTRLG_getEepromInfo(sensorId, &eepromInfo);
    if (ERRG_FAILED(retCode))
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "Failed to get eeprom info of sensor %d\n", sensorId);
        return retCode;
    }

    if((offset + size) > eepromInfo.eepromSize)
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "offset (%d) and size (%d) exceeding eeprom size (%d)\n", offset, size, eepromInfo.eepromSize);
        return SENSORS_MNGR__ERR_INVALID_ARGS;
    }

    LOGG_PRINT(LOG_INFO_E, NULL, "write EEPROM data of sensor ID %d (i2c %d)\n", sensorId, eepromInfo.i2cNum);

    // write data into EEPROM
    for (i = 0; i < size; i++)
    {
        HW_REGSG_writeI2cReg(eepromInfo.i2cNum, eepromInfo.regAddress + offset + i, eepromInfo.tarAddress, eepromInfo.accessSize, 1, data[i]);

        OS_LYRG_usleep(5000);    // byte writes takes 5ms, according to the M24C64X EEprom datasheet
    }

    LOGG_PRINT(LOG_INFO_E, NULL, "write EEPROM data from offset %d (%d bytes) done\n", offset, size);

    return retCode;
}

#ifdef __cplusplus
    }
#endif

