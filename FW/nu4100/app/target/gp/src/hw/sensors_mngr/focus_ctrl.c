/****************************************************************************
 *
 *   FileName: focus_ctrl.c
 *
 *   Author:  Oshri A.
 *
 *   Date:
 *
 *   Description: focus control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "inu2_internal.h"
#include "gen_sensor_drv.h"
#include "gen_model_drv.h"
#include "sensors_mngr.h"
#include "eeprom_ctrl.h"
#include "inu_sensor.h"
#include <unistd.h>
#include "assert.h"
#include "io_pal.h"
#define ALTERNATIVE_AR2020_PRODUCT_ID 0x7 /*For the new AR2020 sensors, the product ID is 7 decimal*/
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

// ar2020 eeprom info
#define FOCUS_CTRLP_EEPROM_AR2020_PRODUCT_ID_OFFSET      0
#define FOCUS_CTRLP_EEPROM_AR2020_INFINITY_OFFSET        7012
#define FOCUS_CTRLP_EEPROM_AR2020_MACRO_OFFSET           7014

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************i
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
static ERRG_codeE FOCUS_CTRLP_getFocusParams_AR2020(SENSORS_MNGRG_sensorInfoT* sensorInfoP, inu_sensor__getFocusParams_t *paramsP)
{
   ERRG_codeE                    ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_focusParamsT  params;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_FOCUS_PARAMS_E, &params);
   if (ERRG_SUCCEEDED(ret))
   {
       paramsP->dac = params.dac;
       paramsP->min = params.minDac;
       paramsP->max = params.maxDac;
       paramsP->chipId = params.chipId;
       LOGG_PRINT(LOG_INFO_E, NULL, "get focus params: current %d, min %d, max %d\n", paramsP->dac, paramsP->min, paramsP->max);
   }
   return ret;
}

static ERRG_codeE FOCUS_CTRLP_setFocusParams_AR2020(SENSORS_MNGRG_sensorInfoT* sensorInfoP, inu_sensor__setFocusParams_t *paramsP)
{
   ERRG_codeE                    ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_focusParamsT  params;

   params.dac = 0;
   if (paramsP->mode == INU_SENSOR__FOCUS_MODE_MANUAL)
   {
      params.dac = paramsP->dac;
   }
   else
   {
      INT16 eepromDac = 0;

      // get the first byte EEPROM data from I2C
      if (paramsP->mode == INU_SENSOR__FOCUS_MODE_MACRO)
      {
         ret = EEPROM_CTRLG_readFromEeprom(sensorInfoP->sensorCfg.sensorId, FOCUS_CTRLP_EEPROM_AR2020_MACRO_OFFSET, sizeof(INT16), (UINT8*)&eepromDac);
      }
      else if (paramsP->mode == INU_SENSOR__FOCUS_MODE_INFINITY)
      {
         ret = EEPROM_CTRLG_readFromEeprom(sensorInfoP->sensorCfg.sensorId, FOCUS_CTRLP_EEPROM_AR2020_INFINITY_OFFSET, sizeof(INT16), (UINT8*)&eepromDac);
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "illegal focus mode (%d)\n", paramsP->mode);
         return SENSORS_MNGR__ERR_ILLEGAL_STATE;
      }

      params.dac = (INT32)eepromDac;
   }

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_FOCUS_PARAMS_E, &params);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "set focus params to %d (mode %d)\n", params.dac, paramsP->mode);

   return ret;
}

/****************************************************************************
*  Function Name: FOCUS_CTRLG_isFocusCtlSupported
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
ERRG_codeE FOCUS_CTRLG_isFocusCtlSupported(SENSORS_MNGRG_sensorInfoT* sensorInfoP)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;

   if (sensorInfoP->focusCtrlSupport)
   {
      return SENSORS_MNGR__RET_SUCCESS; // support already checked and confirmed
   }

   switch(sensorInfoP->sensorCfg.sensorModel)
   {
      case INU_DEFSG_SENSOR_MODEL_AR_2020_E:
      {
         inu_device__eepromInfoT eepromInfo;
         UINT8                   productId;

         ret = EEPROM_CTRLG_readFromEeprom(sensorInfoP->sensorCfg.sensorId, FOCUS_CTRLP_EEPROM_AR2020_PRODUCT_ID_OFFSET, 1, &productId);
         if (ERRG_SUCCEEDED(ret))
         {
            if ((productId != 0x01) && (productId != ALTERNATIVE_AR2020_PRODUCT_ID))   // if not AF device
            {
               LOGG_PRINT(LOG_WARN_E, NULL, "not auto focus device (productId = %d)\n", productId);
               ret = SENSORS_MNGR__ERR_NOT_SUPPORTED;
            }
         }
      }
      break;

      default:
      {
         ret = ERR_INVALID_ARGS;
      }
      break;
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: FOCUS_CTRLG_getFocusParams
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: focus control
*
****************************************************************************/
ERRG_codeE FOCUS_CTRLG_getFocusParams(IO_HANDLE sensorHandle, inu_sensor__getFocusParams_t *paramsP)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;

   if (sensorHandle)
   {
      SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

      ret = FOCUS_CTRLG_isFocusCtlSupported(sensorInfoP);
      if (ERRG_SUCCEEDED(ret))
      {
         switch (sensorInfoP->sensorCfg.sensorModel)
         {
            case INU_DEFSG_SENSOR_MODEL_AR_2020_E:
            {
               return FOCUS_CTRLP_getFocusParams_AR2020(sensorInfoP, paramsP);
            }
            break;

            default:
            {
               // not supported
            }
            break;
         }
      }
      ret = SENSORS_MNGR__ERR_NOT_SUPPORTED;
      LOGG_PRINT(LOG_INFO_E, NULL, "focus control is not supported in this sensor\n");
   }
   else
   {
      ret = SENSORS_MNGR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_INFO_E, NULL, "focus control is not supported in this sensor\n");
   }
   return ret;
}

/****************************************************************************
*
*  Function Name: FOCUS_CTRLG_setFocusParams
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: sensors manager
*
****************************************************************************/
ERRG_codeE FOCUS_CTRLG_setFocusParams(IO_HANDLE sensorHandle, inu_sensor__setFocusParams_t *paramsP)
{
   ERRG_codeE retCode = SENSORS_MNGR__RET_SUCCESS;

   if (sensorHandle)
   {
      SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

      retCode = FOCUS_CTRLG_isFocusCtlSupported(sensorInfoP);
      if (ERRG_SUCCEEDED(retCode))
      {
         switch (sensorInfoP->sensorCfg.sensorModel)
         {
            case INU_DEFSG_SENSOR_MODEL_AR_2020_E:
            {
               return FOCUS_CTRLP_setFocusParams_AR2020(sensorInfoP, paramsP);
            }
            break;

            default:
            {
               // not supported
            }
            break;
         }
      }
      retCode = SENSORS_MNGR__ERR_NOT_SUPPORTED;
      LOGG_PRINT(LOG_INFO_E, NULL, "focus control is not supported in this sensor\n");
   }
   else
   {
      retCode = SENSORS_MNGR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_INFO_E, NULL, "focus control is not supported in this sensor\n");
   }
   return(retCode);
}


#ifdef __cplusplus
   }
#endif

