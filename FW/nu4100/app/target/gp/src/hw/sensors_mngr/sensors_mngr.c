/****************************************************************************
 *
 *   FileName: sensors_mngr.c
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: sensors control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif
//#include "nu4k_defs.h"
#include "inu2_internal.h"
#include "sensors_mngr.h"
#include "eeprom_ctrl.h"
#include "focus_ctrl.h"
#include "gen_sensor_drv.h"
#include "ar0234_drv.h"
#include "ar2020_drv.h"
#include "ar2020_drv.h"
#include "ov7251_drv.h"
#include "ov9282_drv.h"
#include "ov5675_drv.h"
#include "ov8856_drv.h"
#include "ov4689_drv.h"
#include "cgs132_drv.h"
#include "gc2145_drv.h"
#include "ar430_drv.h"
#include "ov2685_drv.h"
#include "xc9160_drv.h"
#include "cgss130_drv.h"
#include "ov9782_drv.h"
#include "cgs031_drv.h"
#include "os05a10_drv.h"
#include "vd55g0_drv.h"
#include "vd56g3_drv.h"
#include "gen_model_drv.h"
#include "gme_mngr.h"
#include "hcg_mngr.h"
#include "gpio_drv.h"
#include "trigger_mngr.h"
#include "xml_db.h"
#include "sequence_mngr.h"
#include "inu_sensor.h"
#include <unistd.h>
#include "inu_alt.h"
#include "mipi_mngr.h"
#include "assert.h"
#include "io_pal.h"
#include "metadata_updater.h"
#include "helsinki.h"
//#define STORE_HANDLE_IN_SENSOR_ENABLE /*Not needed for Helsinki */
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   SENSORS_MNGRG_sensorInfoT SENSORS_MNGRP_sensorsInfo[INU_DEFSG_NUM_OF_INPUT_SENSORS];
static UINT32 sensorIndexTbl[]={0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0,0,0};
#define HP_CASE

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
static SENSOR_init   SENSORS_MNGRP_getSensorInitFunc(INU_DEFSG_sensorModelE sensorModel);
static void          SENSORS_MNGRP_showSensorCfg(INU_DEFSG_logLevelE level ,SENSORS_MNGRG_sensorInfoT  *sensorInfoP);
static ERRG_codeE    SENSORS_MNGRP_changeExpMode(IO_HANDLE sensorHandle, INU_DEFSG_sensorExpModeE expMode);
static ERRG_codeE    SENSORS_MNGRP_setRowOffset(IO_HANDLE sensorHandle, INU_DEFSG_sensorOffsetsT *offsetsP);
static ERRG_codeE    SENSORS_MNGRP_setPowerFrequency(IO_HANDLE sensorHandle, INU_DEFSG_senPowerFreqHzE powerFreqHz);

#define OV5675_OV8856_WORKAROUND

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
/*
   return first sensor instance in db to match the requested functionality.
*/
//TODO ---------------------------------
static int SENSORS_MNGRP_findSensorInst(XMLDB_dbH dbh, INU_DEFSG_senFunctionalityE func)
{
   enum {NUM_SENSORS_E = 6};
   UINT32 funcVal = -1;
   int i;
   for(i = 0 ; i < NUM_SENSORS_E; i++)
   {
      funcVal = -1;
      XMLDB_getValue(dbh, NUFLD_calcPath(NUFLD_SENSOR_E, i, SENSORS_SENS_0_FUNCTION_E),&funcVal);

      if(funcVal == (UINT32)func)
         break;

   }
   return (i == NUM_SENSORS_E) ? -1: i;
}

/*
   try to override the vertical offset in the sensor configuraiton structure from the nucfg db
*/
   //TODO --------------------
static int SENSORS_MNGRP_getImgOffsets(SENSORS_MNGRG_sensorInfoT *sensorInfoP)
{
   int ret = 0;
/*
   XMLDB_dbH dbh;
   int inst = 0;
   UINT32 val = 0;
   ERRG_codeE err = XMLDB__RET_SUCCESS;

   dbh = inu_graph__getXMLDB();
   if(dbh)
   {
      inst = SENSORS_MNGRP_findSensorInst(dbh, sensorInfoP->sensorCfg.sensorFunc);
      if(inst >= 0)
      {
         err = XMLDB_getValue(dbh, NUFLD_calcPath(NUFLD_SENSOR_E, inst, SENSORS_SENS_0_VERT_OFFSET_E),&val);
         if(ERRG_SUCCEEDED(err))
         {
            sensorInfoP->sensorCfg.offsets.roiXstart = (UINT16)0xFFFF; //ignored
            sensorInfoP->sensorCfg.offsets.roiXend = (UINT16)0xFFFF; //ignored
            sensorInfoP->sensorCfg.offsets.roiYstart = (UINT16)val; //[rb] TODO - verify the structure of this field
            sensorInfoP->sensorCfg.offsets.roiYend = (UINT16)(val >> 16);//[rb] TODO - verify the structure of this field
            ret = 1;
         }
      }
   }

   if(ret)
      LOGG_PRINT(LOG_DEBUG_E,0,"sensor mngr: got vertical offset from db: func=%d inst=%d ystart %d yend=%d\n",
         sensorInfoP->sensorCfg.sensorFunc, inst, sensorInfoP->sensorCfg.offsets.roiYstart ,sensorInfoP->sensorCfg.offsets.roiYend);
   else
      LOGG_PRINT(LOG_DEBUG_E,0,"sensor mngr: could not get vertical offset from db. dbh=%p func=%d inst=%d err=%x \n",
         dbh,sensorInfoP->sensorCfg.sensorFunc, inst,err);
*/
   return ret;

}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_stereoStrobeIsr
*
*  Description: Strobe handler:
*                   Apply exposure and projector mode for next frame.
*                   Save timestamp with 1/2 exposure time componsation
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
static void SENSORS_MNGRP_strobeIsr(UINT64 timestamp, UINT64 count, void *argP)
{
   ERRG_codeE                          ret          = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_exposureTimeCfgT    setExposure;
   SENSORS_MNGRG_sensorInfoT          *sensorInfoP  = (SENSORS_MNGRG_sensorInfoT*)argP;
   IO_HANDLE                           sensorHandle = sensorInfoP->sensorHandle;
   PROJ_DRVG_projSetValParamsT         projSetValParams;
   PROJ_DRVG_projGetValParamsT         projGetValParams;
   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;

// UINT64                              currentUsec = 0;
// OS_LYRG_getUsecTime(&currentUsec);
// printf("%llu strobe isr. (currentUsec %llu)\n",timestamp/1000,currentUsec);

   //for AS3648 projector in SERES module, we need to set control every strobe.'out_on' and 'mode_setting' AS3648's fields are automatically cleared after a flash pulse.
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_AMS_CGSS130_E)
   {
      system("echo 1 > /sys/bus/i2c/drivers/as3648/0-0030/arm_select");
   }

   OS_LYRG_lockMutex(&sensorInfoP->strobeTsMutex);
   /* Strobe isr is set to falling edge, reduce the exposure time to give the starting time */
   sensorInfoP->strobeTs  =  timestamp - ((sensorInfoP->exposure[ALTG_getThisFrameMode(sensorInfoP->altH)]/2) * 1000);
   sensorInfoP->strobeCtr += count;
   OS_LYRG_unlockMutex(&sensorInfoP->strobeTsMutex);

   ret = ALTG_strobeHandle(sensorInfoP->altH, sensorInfoP);
   if (ERRG_FAILED(ret))
   {
      if (sensorInfoP->pendingChange)
      {
         LOGG_PRINT(LOG_INFO_E,NULL,"Pending change found \n");
         setExposure.exposureTime = sensorInfoP->pendingExposure[sensorInfoP->pendingContext];
         setExposure.context      = sensorInfoP->pendingContext;
         ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
         if(ERRG_SUCCEEDED(ret))
         {
            ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_STROBE_TIME_E, &setExposure);
            if(ERRG_SUCCEEDED(ret))
            {
               SENSORS_MNGRG_updateSensorExpTime((IO_HANDLE)argP,setExposure.exposureTime,setExposure.context);
            }
         }
         specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);

         if ((specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_CGS_132_E)) ||
                  (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OV_4689_E) || (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E))
         {
             unsigned int savedAddr = specificDev->sensorAddress;
            if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_CGS_132_E)
                 specificDev->sensorAddress = 0x60;
             else if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E)
                 specificDev->sensorAddress = 0x30;
             else specificDev->sensorAddress = 0x6c;
             ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
             if(ERRG_SUCCEEDED(ret))
             {
                  ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_STROBE_TIME_E, &setExposure);
                  if(ERRG_SUCCEEDED(ret))
                  {
                      SENSORS_MNGRG_updateSensorExpTime((IO_HANDLE)argP,setExposure.exposureTime,setExposure.context);
                  }
             }
             specificDev->sensorAddress = savedAddr;
         }
      }
      sensorInfoP->pendingChange = 0;

      //update the projectorState in the sensor according to the current projector state. it was at this state when the strobe was active high
      //to the projector
      projGetValParams.projNum = PROJ_DRVG_PROJ_0_E; //TODO - bind through HW xml the projector to the sensor
      ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_PROJ_0_E), PROJ_DRVG_GET_PROJ_VAL_CMD_E, &projGetValParams);
      if (ERRG_SUCCEEDED(ret))
      {
         sensorInfoP->projectorState = projGetValParams.projState;
      }

      //check for a pending projector change
      if (sensorInfoP->pendingProjectorChange)
      {
         projSetValParams.projNum = PROJ_DRVG_PROJ_0_E; //TODO - bind through HW xml the projector to the sensor
         projSetValParams.projState = sensorInfoP->pendingProjectorState;
         ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_PROJ_0_E), PROJ_DRVG_SET_PROJ_VAL_CMD_E, &projSetValParams);
         sensorInfoP->pendingProjectorChange = 0;
      }
   }

   //GME_MNGRG_processEvent(GME_DRVG_HW_EVENT_STROBE_RISE_ISR_1_E + sensorInfoP->sensorCfg.strobeNum);
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_setStrobeIsr
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
static ERRG_codeE SENSORS_MNGRP_setStrobeIsr(OS_LYRG_intHandlerT cb, INU_DEFSG_strobesListE strobeNum, void *argP)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   OS_LYRG_intNumE intNum = strobeNum + OS_LYRG_INT_STROBE1_E;
   ret = OS_LYRG_intCtrlRegister(intNum, cb, argP);
   if (ERRG_SUCCEEDED(ret))
   {
      ret = OS_LYRG_intCtrlEnable(intNum);
   }
   return ret;
}


static ERRG_codeE SENSORS_MNGRP_releaseStrobeIsr(INU_DEFSG_strobesListE strobeNum)
{
   OS_LYRG_intNumE intNum = strobeNum + OS_LYRG_INT_STROBE1_E;
   return OS_LYRG_intCtrlUnregister(intNum);
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_getSensorInitFunc
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
static SENSOR_init SENSORS_MNGRP_getSensorInitFunc(INU_DEFSG_sensorModelE sensorModel)
{
   SENSOR_init retInitFunc;

   switch(sensorModel)
   {
      case INU_DEFSG_SENSOR_MODEL_AMS_CGSS130_E:
      {
         retInitFunc = CGSS130_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OV_7251_E:
      {
         retInitFunc = OV7251_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OV_2685_E:
      {
         retInitFunc = OV2685_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OV_9282_E:
      {
         retInitFunc = OV9282_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_GC_2145_E://corsair
      {
         retInitFunc = GC2145_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_GC_2053_E:
      {
         retInitFunc = GC2145_DRVG_init;  // TODO: change to 2053 when the driver is ready
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OV_5675_E:
      {
         retInitFunc = OV5675_DRVG_init;
      }
      break;
      case INU_DEFSG_SENSOR_MODEL_OV_8856_E:
      {
         retInitFunc = OV8856_DRVG_init;
      }
      break;
      case INU_DEFSG_SENSOR_MODEL_OV_4689_E:
      {
         retInitFunc = OV4689_DRVG_init;
      }
      break;
      case INU_DEFSG_SENSOR_MODEL_CGS_132_E:
      {
         retInitFunc = CGS132_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_AR_0234_E:
      {
          retInitFunc = AR0234_DRVG_init;
      }
      break;
      case INU_DEFSG_SENSOR_MODEL_AR_2020_E:
      {
          retInitFunc = AR2020_DRVG_init;
      }
      break;
      case INU_DEFSG_SENSOR_MODEL_AR_2021_E:
      {
          retInitFunc = AR2021_DRVG_init;
      }
      break;
      case INU_DEFSG_SENSOR_MODEL_AR_430_E:
      {
         retInitFunc = AR430_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OV_9782_E:
      {
         retInitFunc = OV9782_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_CGS_031_E:
      {
         retInitFunc = CGS031_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_XC_9160_E:
      {
          retInitFunc = XC9160_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OS05A10_E:
      {
          retInitFunc = OS05A10_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_VD55G0_E:
      {
          retInitFunc = VD55G0_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_VD56G3_E:
      {
          retInitFunc = VD56G3_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_GENERIC_E:
      {
         retInitFunc = GENMODEL_DRVG_init;
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_NONE_E:
      {
         retInitFunc = NULL;
      }
      break;

      default:
      {
         retInitFunc = NULL;
      }
      break;
   }

   return(retInitFunc);
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_showSensorCfg
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
void SENSORS_MNGRP_showSensorCfg(INU_DEFSG_logLevelE level ,SENSORS_MNGRG_sensorInfoT  *sensorInfoP)
{
   LOGG_PRINT(level, NULL, "Sensor (%p) Configuration:\n",  sensorInfoP);
   LOGG_PRINT(level, NULL, "sensorSelect=%d\n",          sensorInfoP->sensorCfg.sensorSelect);
   LOGG_PRINT(level, NULL, "sensorCmd=%d\n",             sensorInfoP->sensorCfg.sensorCmd);
   if (sensorInfoP->sensorCfg.sensorModel==INU_DEFSG_SENSOR_MODEL_AR_135X_E)
      LOGG_PRINT(level, NULL, "sensorModel=135X\n");
   else
      LOGG_PRINT(level, NULL, "sensorModel=%d\n",           sensorInfoP->sensorCfg.sensorModel);
   LOGG_PRINT(level, NULL, "sensorFunc=%d\n",            sensorInfoP->sensorCfg.sensorFunc);
   LOGG_PRINT(level, NULL, "resolutionId=%d\n",          sensorInfoP->sensorCfg.resolutionId);
   LOGG_PRINT(level, NULL, "fps=%d\n",                   sensorInfoP->sensorCfg.fps);
   LOGG_PRINT(level, NULL, "format=%d\n",                sensorInfoP->sensorCfg.format);
   LOGG_PRINT(level, NULL, "powerFreqHz=%d\n",           sensorInfoP->sensorCfg.powerFreqHz);
   LOGG_PRINT(level, NULL, "expMode=%d\n",               sensorInfoP->sensorCfg.expMode);
   LOGG_PRINT(level, NULL, "offsets: x start=%d, y start=%d, x end=%d, y end=%d\n",
                                                         sensorInfoP->sensorCfg.offsets.roiXstart,
                                                         sensorInfoP->sensorCfg.offsets.roiYstart,
                                                         sensorInfoP->sensorCfg.offsets.roiXend,
                                                         sensorInfoP->sensorCfg.offsets.roiYend);
   LOGG_PRINT(level, NULL, "defaultExp=%d\n",            sensorInfoP->sensorCfg.defaultExp);
   LOGG_PRINT(level, NULL, "totalExpTimePerSec=%d\n",    sensorInfoP->sensorCfg.totalExpTimePerSec);
   LOGG_PRINT(level, NULL, "expTimePerDutyCycle=%d\n",   sensorInfoP->sensorCfg.expTimePerDutyCycle);
   LOGG_PRINT(level, NULL, "trigger src=%d\n",           sensorInfoP->sensorCfg.triggerSrc);
   LOGG_PRINT(level, NULL, "trigger delay=%d\n",         sensorInfoP->sensorCfg.triggerDelay);
   LOGG_PRINT(level, NULL, "orientation=%d\n",           sensorInfoP->sensorCfg.orientation);
   LOGG_PRINT(level, NULL, "extGpio=%d\n",               sensorInfoP->sensorCfg.extGpio);
   LOGG_PRINT(level, NULL, "extGpioDelay=%d\n",          sensorInfoP->sensorCfg.extGpioDelay);
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_changeExpMode
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
static ERRG_codeE SENSORS_MNGRP_changeExpMode(IO_HANDLE sensorHandle, INU_DEFSG_sensorExpModeE expMode)
{
   ERRG_codeE                          ret         = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;

   sensorParams.setExposureModeParams.expMode = expMode;
   ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_MODE_E, &sensorParams);

   return ret;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_setRowOffset
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
static ERRG_codeE SENSORS_MNGRP_setRowOffset(IO_HANDLE sensorHandle, INU_DEFSG_sensorOffsetsT *offsetsP)
{
   ERRG_codeE                          ret         = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;

   sensorParams.setImageOffsetsParams.roiXstart = offsetsP->roiXstart;
   sensorParams.setImageOffsetsParams.roiXend   = offsetsP->roiXend;
   sensorParams.setImageOffsetsParams.roiYstart = offsetsP->roiYstart;
   sensorParams.setImageOffsetsParams.roiYend   = offsetsP->roiYend;
   ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_IMG_OFFSETS_E, &sensorParams);

   return ret;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_setPowerFrequency
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
static ERRG_codeE SENSORS_MNGRP_setPowerFrequency(IO_HANDLE sensorHandle, INU_DEFSG_senPowerFreqHzE powerFreqHz)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;

   sensorParams.setPowerFreqParams.powerFreq = powerFreqHz;
   ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_POWER_FREQUENCY_E, &sensorParams);

   return ret;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_setMirrorFlipMode
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
static ERRG_codeE SENSORS_MNGRP_setOrientation(IO_HANDLE sensorHandle, INU_DEFSG_sensorOrientationE orientation)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;

   sensorParams.orientationParams.orientation = orientation;
   ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_ORIENTATION_E, &sensorParams);

   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_setInitExposureGainVals
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
static ERRG_codeE SENSORS_MNGRP_setInitExposureGainVals( SENSORS_MNGRG_sensorInfoT *sensorInfoP )
{
   ERRG_codeE                          ret           = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_exposureTimeCfgT    getExposure;
   GEN_SENSOR_DRVG_gainCfgT            getGain;
   INU_DEFSG_sensorContextE            context;

   for (context = INU_DEFSG_SENSOR_CONTEX_A; context < INU_DEFSG_SENSOR_NUM_CONTEXTS; context++)
   {
      getExposure.context     = context;
      getGain.context         = context;
      getGain.analogGain      = 0;
      getGain.digitalGain     = 0;

      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_EXPOSURE_TIME_E, &getExposure);
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_GAIN_E, &getGain);

      if (sensorInfoP->sensorCfg.defaultExp != INU_DEFSG_INVALID)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Setting default exposure=%lu, initial exposure=%lu \n",sensorInfoP->sensorCfg.defaultExp, getExposure.exposureTime);
         getExposure.exposureTime=sensorInfoP->sensorCfg.defaultExp;
      }
      if (sensorInfoP->sensorCfg.defaultGain > 0)
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Setting default gain=%f(analog), initial again=%f dgain=%f model %d\n",sensorInfoP->sensorCfg.defaultGain, getGain.analogGain, getGain.digitalGain, sensorInfoP->sensorCfg.sensorModel);
         getGain.analogGain = sensorInfoP->sensorCfg.defaultGain;
         ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_E, &getGain);
      }

      if (!sensorInfoP->sensorCfg.fps)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Sensor %d (%p) model %d fps is 0! set to 30\n",
                                        sensorInfoP->sensorCfg.sensorSelect, sensorInfoP, sensorInfoP->sensorCfg.sensorModel);
         sensorInfoP->sensorCfg.fps = 30;
      }

      /* special case - OV7251 in trigger mode cannot set exposure to overlap the readout (~13.5ms) */
      if (((getExposure.exposureTime) >= ((1000000 / sensorInfoP->sensorCfg.fps) - 13500))
           && (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_7251_E) && (sensorInfoP->sensorCfg.isTriggerSupported))
      {
         getExposure.exposureTime = ((1000000 / sensorInfoP->sensorCfg.fps) - 13500);
      }

      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &getExposure);
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_STROBE_TIME_E, &getExposure);
      sensorInfoP->exposure[context]    = getExposure.exposureTime;
      sensorInfoP->analogGain[context]  = getGain.analogGain;
      sensorInfoP->digitalGain[context] = getGain.digitalGain;
      //the pendingExposure is the "shadow" value, at init make sure its the same as the real exposure.
      sensorInfoP->pendingExposure[context] = getExposure.exposureTime;
   }

   return ret;
}




/***************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorInfo
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Sensors manager
*
****************************************************************************/
SENSORS_MNGRG_sensorInfoT *SENSORS_MNGRG_getSensorInfo(INU_DEFSG_senSelectE sensorSelect)
{
   int i;
   for (i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      if ((SENSORS_MNGRP_sensorsInfo[i].sensorHandle) && (SENSORS_MNGRP_sensorsInfo[i].sensorCfg.sensorSelect == sensorSelect))
      {
         return &SENSORS_MNGRP_sensorsInfo[i];
      }
   }
   return NULL;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorInitParams
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Sensors manager
*
****************************************************************************/
ERRG_codeE SENSORS_MNGRG_getIspInitParams(IO_HANDLE sensorHandle)
{
    ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
	SENSORS_MNGRG_sensorInfoT		 *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
	GEN_SENSOR_DRVG_sensorParametersT p;

	ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_INIT_PARAMS_E, &p);
    if(ERRG_SUCCEEDED(ret))
	{
        sensorInfoP->isp.bayerFormat        = p.ispInitParams.bayerFormat;
	    sensorInfoP->isp.hts                = p.ispInitParams.hts;
	    sensorInfoP->isp.sclk               = p.ispInitParams.sclk;

	    sensorInfoP->isp.frmLengthLines     = p.ispInitParams.frmLengthLines;

		sensorInfoP->isp.minIntegrationLine = p.ispInitParams.minIntegrationLine;
	    sensorInfoP->isp.maxIntegrationLine = p.ispInitParams.maxIntegrationLine;
	    sensorInfoP->isp.minGain            = p.ispInitParams.minGain;
	    sensorInfoP->isp.maxGain            = p.ispInitParams.maxGain;
	    sensorInfoP->isp.minFps             = p.ispInitParams.minFps;
	    sensorInfoP->isp.maxFps             = p.ispInitParams.maxFps;
		sensorInfoP->isp.aecMaxIntegrationTime = p.ispInitParams.aecMaxIntegrationTime;
	}
	else
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "Getting initial sensors params for ISP failed\n");
	}
	return ret;

}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorInfoByModel
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Sensors manager
*
****************************************************************************/
//////
// this function will return the first instance of the given sensor model
// it will problematic when there are more then one instance of same model in paths
//////
SENSORS_MNGRG_sensorInfoT *SENSORS_MNGRG_getSensorInfoByModel(INU_DEFSG_sensorModelE sensorModel)
{
   int i;
   for (i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      if ((SENSORS_MNGRP_sensorsInfo[i].sensorHandle) && (SENSORS_MNGRP_sensorsInfo[i].sensorCfg.sensorModel == ((INU_DEFSG_sensorModelE)(sensorModel))))
      {
         return &SENSORS_MNGRP_sensorsInfo[i];
      }
   }
   return NULL;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGR_getStrobeData
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Sensors manager
*
****************************************************************************/
ERRG_codeE SENSORS_MNGRG_getStrobeData(IO_HANDLE ioHandle, UINT64 *timestampP, UINT64 *ctrP, UINT32 *projMode)
{
   UINT64 timestamp,ctr;
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   UINT32 sluInst;
   SENSORS_MNGRG_sensorInfoT    *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   OS_LYRG_lockMutex(&sensorInfoP->strobeTsMutex);
   timestamp = sensorInfoP->strobeTs;
   ctr = sensorInfoP->strobeCtr;
   OS_LYRG_unlockMutex(&sensorInfoP->strobeTsMutex);
   //printf("SENSORS_MNGR_getStrobeData (%p): ts = %llu, ctr = %llu\n",sensorInfoP,timestamp,ctr );
   if (!ctr || !timestamp)
   {
      *timestampP = 0;
      *ctrP = 0;
      *projMode = 0;
      return SENSORS_MNGR__ERR_UNEXPECTED;
   }
   *timestampP = timestamp;
   *ctrP       = ctr;
   *projMode   = sensorInfoP->projectorState;
   return ret;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Sensors manager
*
****************************************************************************/
void SENSORS_MNGRG_init()
{
   // reset sensors manager module info structure
   UINT32 sensorSelectIdx =0;
   memset(&SENSORS_MNGRP_sensorsInfo, 0, sizeof(SENSORS_MNGRP_sensorsInfo));

   for(sensorSelectIdx = 0; sensorSelectIdx < INU_DEFSG_NUM_OF_INPUT_SENSORS; sensorSelectIdx++)
   {
      // init sensor configuration
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorActivity               = FALSE;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.sensorCmd          = INU_DEFSG_SENSOR_RESET_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.resolutionId       = INU_DEFSG_RES_VGA_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.fps                = INU_DEFSG_FR_20_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.powerFreqHz        = INU_DEFSG_SEN_POWER_FREQ_50_HZ_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.expMode            = INU_DEFSG_AE_MANUAL_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.offsets.roiXend    = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.offsets.roiXstart  = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.offsets.roiYstart  = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.offsets.roiYend    = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.defaultExp         = -1;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.defaultGain        = -1;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.totalExpTimePerSec = -1;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.expTimePerDutyCycle= -1;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.triggerSrc         = INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.triggerDelay       = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.orientation        = INU_DEFSG_SENSOR_ORIGINAL_E;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.extGpio            = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.extGpioDelay       = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorCfg.sensorSelect       = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].sensorHandle                 = 0;
      SENSORS_MNGRP_sensorsInfo[sensorSelectIdx].focusCtrlSupport             = 0;
   }
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_showAllSensorsCfg
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
void SENSORS_MNGRG_showAllSensorsCfg()
{
   UINT32 i;
   for ( i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      if (SENSORS_MNGRP_sensorsInfo[i].sensorHandle)
      {
         SENSORS_MNGRP_showSensorCfg(LOG_INFO_E, &SENSORS_MNGRP_sensorsInfo[i]);
      }
   }
}



/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_updateSensorExpTime
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
void SENSORS_MNGRG_updateSensorExpTime(IO_HANDLE ioHandle, UINT32 expTime, INU_DEFSG_sensorContextE context)
{
   SENSORS_MNGRG_sensorInfoT     *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   sensorInfoP->exposure[context] = expTime;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_updateSensorFPS
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
void SENSORS_MNGRG_updateSensorFPS(IO_HANDLE ioHandle, UINT32 FPS)
{
   SENSORS_MNGRG_sensorInfoT     *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   sensorInfoP->sensorCfg.fps = FPS;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_updateSensorGain
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
void SENSORS_MNGRG_updateSensorGain(IO_HANDLE ioHandle, float analogGain, float digitalGain, inu_sensor__sensorContext_e context)
{

   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT           *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;

   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;
   GEN_SENSOR_DRVG_gainCfgT            getGain;

   if (sensorInfoP->sensorActivity)
   {
	  specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);

	  if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_2020_E || specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_2021_E)
	  {
		   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_GAIN_E, &getGain);
		   if(ERRG_SUCCEEDED(ret))
		   {
			  sensorInfoP->analogGain[context] = getGain.analogGain;
			  sensorInfoP->digitalGain[context] = getGain.digitalGain;

		      //printf("!!! %s !!! Analog %f, digital %f\n", __func__, getGain.analogGain, getGain.digitalGain);
		   }
	  }
      else
      {
		  sensorInfoP->analogGain[context] = analogGain;
		  sensorInfoP->digitalGain[context] = digitalGain;
      }
   }

}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_setSensorExpTime
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
ERRG_codeE SENSORS_MNGRG_setSensorExpTime(IO_HANDLE ioHandle, UINT32 *expTimeP, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   UINT32                              totalExpTimePerSec;
   UINT32                              expTimePerDutyCycle;
   SENSORS_MNGRG_sensorInfoT           *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   UINT32                              fps;
   GEN_SENSOR_DRVG_exposureTimeCfgT    setExposure;
   INU_DEFSG_moduleTypeE               modelType = GME_MNGRG_getModelType();

   fps = sensorInfoP->sensorCfg.fps;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "fps = %d, exp = %d, totalExpTimePerSec=%x, expTimePerDutyCycle=%x, defaultExp=%x\n" , fps, *expTimeP, sensorInfoP->sensorCfg.totalExpTimePerSec,sensorInfoP->sensorCfg.expTimePerDutyCycle,sensorInfoP->sensorCfg.defaultExp);
   /* Stereo sensors exposure in some cases control the projector. We protect the system by limiting the exposure */
   if((sensorInfoP->sensorCfg.sensorFunc == INU_DEFSG_SEN_MASTER_E) || (sensorInfoP->sensorCfg.sensorFunc == INU_DEFSG_SEN_SLAVE_E))
   {
     if (sensorInfoP->sensorCfg.totalExpTimePerSec!=INU_DEFSG_INVALID)
     {
        totalExpTimePerSec = sensorInfoP->sensorCfg.totalExpTimePerSec;
        if ((totalExpTimePerSec * 1000) < (fps * (*expTimeP)))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Exceeding max, clipping,totalExposureTimePerSecond=%d[ms/s],requested=%d[us],fps=%d,allowedPerFrame=%d[us])\n", totalExpTimePerSec,*expTimeP,fps, (totalExpTimePerSec * 1000) / fps );
             *expTimeP = (totalExpTimePerSec * 1000) / fps;
        }
     }

     if (sensorInfoP->sensorCfg.expTimePerDutyCycle!=INU_DEFSG_INVALID)
     {
        expTimePerDutyCycle = sensorInfoP->sensorCfg.expTimePerDutyCycle;
        if ((expTimePerDutyCycle * 1000) < (*expTimeP))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Exceeding max, clipping,expTimePerDutyCycle=%d[ms/s],requested=%d[us],fps=%d,allowedPerFrame=%d[us])\n", expTimePerDutyCycle,*expTimeP);
             *expTimeP = (expTimePerDutyCycle * 1000);
        }
     }
   }

   /* limit the exposure from reaching the frame time (90%) */
   if ((*expTimeP) && (fps > 1) && ((*expTimeP) >= (1000000 / (fps + 1))))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "set exposure higher then fps allows, clip (fps = %d, exp = %d)\n" , fps, *expTimeP);
       *expTimeP = 1000000 / fps;
       *expTimeP -= (*expTimeP / 10);
   }

   /* special case - OV7251 in trigger mode cannot set exposure to overlap the readout (~13.5ms) */
   if (((*expTimeP) >= ((1000000 / fps) - 13500))
        && (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_7251_E) && (sensorInfoP->sensorCfg.isTriggerSupported))
   {
      *expTimeP = ((1000000 / fps) - 13500);
   }
   //LOGG_PRINT(LOG_INFO_E, NULL, "set exposure time without probe, MODEL = %d exp  = %d\n" , sensorInfoP->sensorCfg.sensorModel, *expTimeP);
   /* check if sensor without probe (TBD- move to XML parameter)*/
   if ( (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_GC_2145_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_2685_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_9782_E) || //since in boot 10311 there is no strobe signal
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_5675_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_AR_2020_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_AR_2021_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_8856_E) )
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "set exposure time without probe, MODEL = %d exp  = %d\n" , sensorInfoP->sensorCfg.sensorModel, *expTimeP);
      setExposure.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
   }
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_VD56G3_E)
   {
      setExposure.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
   }
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_VD55G0_E)
   {
      setExposure.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
   }

#ifdef HP_CASE
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_5675_E)
   {
      //printf("SENSORS_MNGRG_setSensorExpTime MODEL 5675\n");
      setExposure.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
   }

   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_8856_E)
   {
      //printf("SENSORS_MNGRG_setSensorExpTime MODEL 8856\n");
      setExposure.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
   }
#endif
   /* check if value is different from current */
   else if (*expTimeP != sensorInfoP->exposure[context])
   {
      /* save the pending change to the next strobe */
      sensorInfoP->pendingExposure[context] = *expTimeP;
      sensorInfoP->pendingContext = context;
      sensorInfoP->pendingChange = 1;
   }

   return(ret);
}

ERRG_codeE SENSORS_MNGRG_setSensorExpTimeAndGain(IO_HANDLE ioHandle, UINT32 *expTimeP,float *analogGainP, float *digitalGainP, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   UINT32                              totalExpTimePerSec;
   UINT32                              expTimePerDutyCycle;
   SENSORS_MNGRG_sensorInfoT           *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   UINT32                              fps;
   GEN_SENSOR_DRVG_exposureTimeCAndGainfgT    setExposureAndGain;
   INU_DEFSG_moduleTypeE               modelType = GME_MNGRG_getModelType();

   setExposureAndGain.gain.analogGain   = *analogGainP;
   setExposureAndGain.gain.digitalGain  = *digitalGainP;
   setExposureAndGain.gain.context      = context;

   fps = sensorInfoP->sensorCfg.fps;
   LOGG_PRINT(LOG_DEBUG_E, NULL, "fps = %d, exp = %d, totalExpTimePerSec=%x, expTimePerDutyCycle=%x, defaultExp=%x\n" , fps, *expTimeP, sensorInfoP->sensorCfg.totalExpTimePerSec,sensorInfoP->sensorCfg.expTimePerDutyCycle,sensorInfoP->sensorCfg.defaultExp);
   /* Stereo sensors exposure in some cases control the projector. We protect the system by limiting the exposure */
   if((sensorInfoP->sensorCfg.sensorFunc == INU_DEFSG_SEN_MASTER_E) || (sensorInfoP->sensorCfg.sensorFunc == INU_DEFSG_SEN_SLAVE_E))
   {
     if (sensorInfoP->sensorCfg.totalExpTimePerSec!=INU_DEFSG_INVALID)
     {
        totalExpTimePerSec = sensorInfoP->sensorCfg.totalExpTimePerSec;
        if ((totalExpTimePerSec * 1000) < (fps * (*expTimeP)))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Exceeding max, clipping,totalExposureTimePerSecond=%d[ms/s],requested=%d[us],fps=%d,allowedPerFrame=%d[us])\n", totalExpTimePerSec,*expTimeP,fps, (totalExpTimePerSec * 1000) / fps );
             *expTimeP = (totalExpTimePerSec * 1000) / fps;
        }
     }

     if (sensorInfoP->sensorCfg.expTimePerDutyCycle!=INU_DEFSG_INVALID)
     {
        expTimePerDutyCycle = sensorInfoP->sensorCfg.expTimePerDutyCycle;
        if ((expTimePerDutyCycle * 1000) < (*expTimeP))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Exceeding max, clipping,expTimePerDutyCycle=%d[ms/s],requested=%d[us],fps=%d,allowedPerFrame=%d[us])\n", expTimePerDutyCycle,*expTimeP);
             *expTimeP = (expTimePerDutyCycle * 1000);
        }
     }
   }

   /* limit the exposure from reaching the frame time (90%) */
   if ((*expTimeP) && (fps > 1) && ((*expTimeP) >= (1000000 / (fps + 1))))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "set exposure higher then fps allows, clip (fps = %d, exp = %d)\n" , fps, *expTimeP);
       *expTimeP = 1000000 / fps;
       *expTimeP -= (*expTimeP / 10);
   }

   /* special case - OV7251 in trigger mode cannot set exposure to overlap the readout (~13.5ms) */
   if (((*expTimeP) >= ((1000000 / fps) - 13500))
        && (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_7251_E) && (sensorInfoP->sensorCfg.isTriggerSupported))
   {
      *expTimeP = ((1000000 / fps) - 13500);
   }
   //LOGG_PRINT(LOG_INFO_E, NULL, "set exposure time without probe, MODEL = %d exp  = %d\n" , sensorInfoP->sensorCfg.sensorModel, *expTimeP);
   /* check if sensor without probe (TBD- move to XML parameter)*/
   if ( (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_GC_2145_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_2685_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_9782_E) || //since in boot 10311 there is no strobe signal
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_5675_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_AR_2020_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_AR_2021_E) ||
      (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_8856_E) )
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "set exposure time without probe, MODEL = %d exp  = %d\n" , sensorInfoP->sensorCfg.sensorModel, *expTimeP);
      setExposureAndGain.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE, &setExposureAndGain);
   }
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_VD56G3_E)
   {
      setExposureAndGain.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE, &setExposureAndGain);
   }
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_VD55G0_E)
   {
      setExposureAndGain.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE, &setExposureAndGain);
   }

#ifdef HP_CASE
   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_5675_E)
   {
      //printf("SENSORS_MNGRG_setSensorExpTime MODEL 5675\n");
      setExposureAndGain.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE, &setExposureAndGain);
   }

   if (sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_8856_E)
   {
      //printf("SENSORS_MNGRG_setSensorExpTime MODEL 8856\n");
      setExposureAndGain.exposureTime = *expTimeP;
      sensorInfoP->exposure[context] = *expTimeP;
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_AND_EXPOSURE, &setExposureAndGain);
   }
#endif
   /* check if value is different from current */
   else if (*expTimeP != sensorInfoP->exposure[context])
   {
      /* save the pending change to the next strobe */
      sensorInfoP->pendingExposure[context] = *expTimeP;
      sensorInfoP->pendingContext = context;
      sensorInfoP->pendingChange = 1;
   }
   
   GEN_SENSOR_DRVG_gainCfgT         setGain;
   setGain.analogGain   = *analogGainP;
   setGain.digitalGain  = *digitalGainP;
   setGain.context      = context;
   SENSORS_MNGRG_updateSensorGain(ioHandle,setGain.analogGain,setGain.digitalGain,context);

   return(ret);
}
/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorExpTime
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
ERRG_codeE SENSORS_MNGRG_getSensorExpTime(IO_HANDLE ioHandle, UINT32 *expTimeP, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret            = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   if (sensorInfoP->sensorActivity)
   {
      *expTimeP = sensorInfoP->exposure[context];
   }
   else
   {
      ret = SENSORS_MNGR__ERR_SENSOR_INACTIVE;
   }
   return(ret);
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorGain
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
ERRG_codeE SENSORS_MNGRG_getSensorGain(IO_HANDLE sensorHandle, float *analogGainP, float *digitalGainP, inu_sensor__sensorContext_e context)
{
   ERRG_codeE  ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT          *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

   if (sensorInfoP->sensorActivity)
   {
     *analogGainP  = sensorInfoP->analogGain[context];
     *digitalGainP = sensorInfoP->digitalGain[context];
   }
   else
   {
      ret = SENSORS_MNGR__ERR_SENSOR_INACTIVE;
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_setSensorGain
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
ERRG_codeE SENSORS_MNGRG_setSensorGain(IO_HANDLE sensorHandle, float *analogGainP, float *digitalGainP, inu_sensor__sensorContext_e context)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_gainCfgT         setGain;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;

   // Note: The analog gain is recieved as float. Later in the driver, each sensor works in its own implementation (float or casting to integer)
   if ((*analogGainP != sensorInfoP->analogGain[context]) || (*digitalGainP != sensorInfoP->digitalGain[context]))
   {
      setGain.analogGain   = *analogGainP;
      setGain.digitalGain  = *digitalGainP;
      setGain.context      = context;
      LOGG_PRINT(LOG_DEBUG_E,NULL,"Setting the Analog gain to %.3f for handle %p \n",setGain.analogGain,sensorInfoP );
      ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_E, &setGain);
      if(ERRG_SUCCEEDED(ret))
      {
         SENSORS_MNGRG_updateSensorGain(sensorHandle,setGain.analogGain,setGain.digitalGain,context);
      }

      specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);

      if ((specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E))
      {
         unsigned int savedAddr = specificDev->sensorAddress;
         if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E)
            specificDev->sensorAddress = 0x30;

         ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_GAIN_E, &setGain);
         if(ERRG_SUCCEEDED(ret))
         {
            SENSORS_MNGRG_updateSensorGain(sensorHandle,setGain.analogGain,setGain.digitalGain,context);
         }
         specificDev->sensorAddress = savedAddr;
      }
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorFps
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
ERRG_codeE SENSORS_MNGRP_getSensorFps(IO_HANDLE sensorHandle, UINT32 *fpsP, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT           *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   (void)context;
   if (sensorHandle)
   {
      *fpsP = sensorInfoP->sensorCfg.fps;
   }
   else
   {
      ret =  SENSORS_MNGR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_ERROR_E, ret, "invalid sensor handle\n");
   }

   return(ret);
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_getSensorTemperature
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
ERRG_codeE SENSORS_MNGRG_getSensorTemperature(IO_HANDLE sensorHandle, INT32 *temperatureP)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT           *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_temperatureParamsT  tempParams;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_TEMPERATURE_E, &tempParams);
   *temperatureP = tempParams.temperature;

   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_setSensorFps
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
ERRG_codeE SENSORS_MNGRP_setSensorFps(SENSORS_MNGRG_sensorInfoT *sensorInfoP, UINT32 *fpsP, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                           sensorHandle   = sensorInfoP->sensorHandle;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;
   (void)context;

   if (sensorHandle)
   {
      if (*fpsP == 0)
      {
         return SENSORS_MNGR__RET_SUCCESS;
      }

      HCG_MNGRG_startPrimePeriod();
      sensorParams.setFrameRateParams.frameRate = *fpsP;
      sensorParams.setFrameRateParams.sensorSelect = sensorInfoP->sensorCfg.sensorSelect;
      ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_FRAME_RATE_E, &sensorParams);

      if(ERRG_SUCCEEDED(ret))
      {
         sensorInfoP->sensorCfg.fps = sensorParams.setFrameRateParams.frameRate;
         *fpsP = sensorParams.setFrameRateParams.frameRate;
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "failed to set new fps \n");
         }
      }
   }
   else
   {
      ret =  SENSORS_MNGR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_ERROR_E, ret, "invalid sensor handle\n");
   }

   return(ret);
}
/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_getEepromInfo
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
ERRG_codeE SENSORS_MNGRP_getEepromInfo(SENSORS_MNGRG_sensorInfoT* sensorInfo, inu_device__eepromInfoT *eepromInfo)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;

   switch(sensorInfo->sensorCfg.sensorModel)
   {
      case INU_DEFSG_SENSOR_MODEL_OV_5675_E:
      {
         eepromInfo->i2cInfo.i2cNum = INU_DEVICE__I2C_2_E;
         eepromInfo->i2cInfo.regAddress = 0;
         eepromInfo->i2cInfo.tarAddress = 0xA8;
         eepromInfo->i2cInfo.accessSize = 2;

         eepromInfo->eepromSize = 8192; // EEPROM M24C64X
      }
      break;

      case INU_DEFSG_SENSOR_MODEL_OV_8856_E:
      {
         eepromInfo->i2cInfo.i2cNum = INU_DEVICE__I2C_2_E;
         eepromInfo->i2cInfo.regAddress = 0;
         eepromInfo->i2cInfo.tarAddress = 0xA0;
         eepromInfo->i2cInfo.accessSize = 2;

         eepromInfo->eepromSize = 8192; // EEPROM M24C64X
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


/**
 * @brief Updates the cropping window for a sensor (Specifically target at AR2020)
 *
 * @param sensorInfoP Sensor Info
 * @param startX Start X position
 * @param startY Start Y position
 * @param context  Unused
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE SENSORS_MNGRP_setCropWindow(SENSORS_MNGRG_sensorInfoT * sensorInfoP, UINT32 startX, UINT32 startY, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;
   (void)context;

   if (sensorInfoP && sensorInfoP->sensorHandle )
   {
      if(((sensorInfoP->sensorCfg.sensorModel==2020)||(sensorInfoP->sensorCfg.sensorModel==2021)))
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Handle: %p, Setting new crop window X:%lu, Y:%lu \n", sensorInfoP,startX,startY);
         sensorParams.cropWindow.startX = startX;
         sensorParams.cropWindow.startY = startY;
         ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_CROP_WINDOW_CTRL_E, &sensorParams);
         if(ERRG_SUCCEEDED(ret))
         {
            sensorInfoP->sensorCfg.cropStartX = startX;
            sensorInfoP->sensorCfg.cropStartY = startY;
            if(ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, ret, "failed to set new Crop window \n");
            }
         }
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "Crop window updates are only supported for the AR2020/AR2021 as crop \n");
      }
   }
   else
   {
      ret =  SENSORS_MNGR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_ERROR_E, ret, "Invalid arguments for crop window update\n");
   }
   return(ret);
}
ERRG_codeE SENSORS_MNGRP_setCropParams(IO_HANDLE sensorHandle, inu_sensor__setCropParams_t *paramsP)
{
   ERRG_codeE retCode = SENSORS_MNGR__RET_SUCCESS;

   if (sensorHandle)
   {
      SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

      switch (sensorInfoP->sensorCfg.sensorModel)
      {
         case INU_DEFSG_SENSOR_MODEL_AR_2020_E:
         case INU_DEFSG_SENSOR_MODEL_AR_2021_E:
         {
            return SENSORS_MNGRP_setCropWindow(sensorInfoP, paramsP->startX, paramsP->startY,0);
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
   LOGG_PRINT(LOG_INFO_E, NULL, "crop control is not supported in this sensor\n");

   return(retCode);
}
/**
 * @brief Gets the cropping window for a sensor (Specifically target at AR2020)
 *
 * @param sensorInfoP Sensor Info
 * @param startX Start X position pointer
 * @param startY Start Y position pointer
 * @param context  Unused
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE SENSORS_MNGRP_getCropWindow(SENSORS_MNGRG_sensorInfoT * sensorInfoP, UINT32 *startX, UINT32 *startY, INU_DEFSG_sensorContextE context)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;
   (void)context;

   if (sensorInfoP && sensorInfoP->sensorHandle )
   {
      if(((sensorInfoP->sensorCfg.sensorModel==2020)||(sensorInfoP->sensorCfg.sensorModel==2021)))
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Handle: %p, Getting new crop window \n", sensorInfoP);
         ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_CROP_WINDOW_CTRL_E, &sensorParams);
         if(ERRG_SUCCEEDED(ret))
         {
            *startX = sensorParams.cropWindow.startX;
            *startY = sensorParams.cropWindow.startY;
            sensorInfoP->sensorCfg.cropStartX = *startX;
            sensorInfoP->sensorCfg.cropStartY = *startY;
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Handle: %p,Crop window X:%lu, Y:%lu \n", sensorInfoP,*startX,*startY);
            if(ERRG_FAILED(ret))
            {
               LOGG_PRINT(LOG_ERROR_E, ret, "failed to get new Crop window \n");
            }
         }
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "Crop window updates are only supported for the AR2020 \n");
      }
   }
   else
   {
      ret =  SENSORS_MNGR__ERR_INVALID_ARGS;
      LOGG_PRINT(LOG_ERROR_E, ret, "Invalid arguments for crop window update\n");
   }
   return(ret);
}
/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_modeToString
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
static char *SENSORS_MNGRP_modeToString(INU_DEFSG_resolutionIdE mode)
{
   switch(mode)
   {
      case(INU_DEFSG_RES_DUMMY_E):
         return "Dummy";
      case(INU_DEFSG_RES_VGA_E):
         return "Binning";
      case(INU_DEFSG_RES_VERTICAL_BINNING_E):
         return "Vertical Binning";
      case(INU_DEFSG_RES_HD_E):
         return "Full";
      case(INU_DEFSG_RES_FULL_HD_E):
         return "Full HD";
      case(INU_DEFSG_RES_USER_DEF_E):
         return "User def";
      default:
         return "Unknown";
   }
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_loadCfgTbl
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
static ERRG_codeE SENSORS_MNGRP_loadCfgTbl(SENSORS_MNGRG_sensorInfoT     *sensorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                     sensorHandle   = sensorInfoP->sensorHandle;
   GEN_SENSOR_DRVG_sensorParametersT   sensorParams;

   if (sensorHandle==NULL)
   {
      return SENSORS_MNGR__ERR_INVALID_ARGS;
   }

   sensorParams.loadTableParams.sensorResolution   = sensorInfoP->sensorCfg.resolutionId;
   sensorParams.loadTableParams.frameRate          = sensorInfoP->sensorCfg.fps;
   sensorParams.loadTableParams.outputFormat       = sensorInfoP->sensorCfg.format;

   ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_LOAD_PRESET_TABLE_E, &sensorParams); // Sensor table specifiec configuration & change config 1st
   sensorInfoP->sensorCfg.fps                      = sensorParams.loadTableParams.frameRate;

   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "load sensor config table (mode=%s, frame rate=%dfps)\n", SENSORS_MNGRP_modeToString(sensorInfoP->sensorCfg.resolutionId), sensorInfoP->sensorCfg.fps);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "load sensor config table fail\n");
   }

   return ret;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_initSensor
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
ERRG_codeE SENSORS_MNGRP_powerdown(SENSORS_MNGRG_sensorInfoT     *sensorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                  sensorHandle   = sensorInfoP->sensorHandle;
   if(sensorHandle)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "power down\n" );
      ret = IO_PALG_ioctl(sensorHandle   , GEN_SENSOR_DRVG_POWER_DOWN_SENSOR_E, sensorInfoP);
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_initSensor
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
ERRG_codeE SENSORS_MNGRP_powerup(SENSORS_MNGRG_sensorInfoT     *sensorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                  sensorHandle   = sensorInfoP->sensorHandle;
   if(sensorHandle)
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "power up %d\n",sensorInfoP->sensorCfg.sensorSelect);
      ret = IO_PALG_ioctl(sensorHandle   , GEN_SENSOR_DRVG_POWER_UP_SENSOR_E, sensorInfoP);
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_initSensor
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
static ERRG_codeE SENSORS_MNGRP_changeAddress(SENSORS_MNGRG_sensorInfoT     *sensorInfoP, UINT8 address)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                  sensorHandle   = sensorInfoP->sensorHandle;
   if(sensorHandle)
   {
      ret = IO_PALG_ioctl(sensorHandle   , GEN_SENSOR_DRVG_CHANGE_ADDRES_E, &address);
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_initSensor
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
static ERRG_codeE SENSORS_MNGRG_initSensor(SENSORS_MNGRG_sensorInfoT     *sensorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                     sensorHandle   = sensorInfoP->sensorHandle;
   if(sensorHandle)
   {
      ret = IO_PALG_ioctl(sensorHandle   , GEN_SENSOR_DRVG_SENSOR_INIT_E, sensorInfoP);
   }
   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_resetSensor
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
ERRG_codeE SENSORS_MNGRG_resetSensor(SENSORS_MNGRG_sensorInfoT     *sensorInfoP )
{
   ERRG_codeE                    ret            = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                     sensorHandle  = sensorInfoP->sensorHandle;
   if(sensorHandle)
   {
      ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_RESET_SENSORS_E, NULL);
   }
   return(ret);
}

ERRG_codeE SENSORS_MNGRG_changeExpMode(IO_HANDLE ioHandle, INU_DEFSG_sensorExpModeE expMode)
{
   ERRG_codeE retCode      = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE  sensorHandle = ((SENSORS_MNGRG_sensorInfoT *)ioHandle)->sensorHandle;

   retCode = SENSORS_MNGRP_changeExpMode(sensorHandle, expMode);

   return retCode;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_configSensor
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
static ERRG_codeE SENSORS_MNGRP_configSensor(SENSORS_MNGRG_sensorInfoT  *sensorInfoP)
{
   ERRG_codeE                 retCode        = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                  sensorHandle   = sensorInfoP->sensorHandle;

   LOGG_PRINT(LOG_INFO_E, NULL, "Sensor %d\n",sensorInfoP->sensorCfg.sensorSelect);

   if(sensorHandle)
   {
      SENSORS_MNGRP_getImgOffsets(sensorInfoP);
      retCode = SENSORS_MNGRG_resetSensor(sensorInfoP);
      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRG_initSensor(sensorInfoP);
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRP_setPowerFrequency(sensorHandle, sensorInfoP->sensorCfg.powerFreqHz);
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "Set sensor offsets to (%d,%d,%d,%d)\n",sensorInfoP->sensorCfg.offsets.roiXstart, sensorInfoP->sensorCfg.offsets.roiYstart,  sensorInfoP->sensorCfg.offsets.roiXend,  sensorInfoP->sensorCfg.offsets.roiYend);
         retCode = SENSORS_MNGRP_setRowOffset(sensorHandle, &sensorInfoP->sensorCfg.offsets);
      }

      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRP_loadCfgTbl(sensorInfoP);
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRP_changeExpMode(sensorHandle, sensorInfoP->sensorCfg.expMode);
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         //set initial FPS from sensor config
         if (!sensorInfoP->sensorCfg.fps)
         {
            LOGG_PRINT(LOG_INFO_E, NULL, "WARNING WARNING! FPS WAS SET TO ZERO (sensor %d), SET TO 30\n",sensorInfoP->sensorCfg.sensorSelect);
            sensorInfoP->sensorCfg.fps = 30;
         }
         retCode = SENSORS_MNGRP_setSensorFps(sensorInfoP, &sensorInfoP->sensorCfg.fps, INU_DEFSG_SENSOR_CONTEX_A);
      }
      if(ERRG_FAILED(retCode))
      {
         LOGG_PRINT(LOG_ERROR_E, retCode, "FAILED TO CONFIGURE SENSOR (resolution=%s, frame rate=%dfps)\n", SENSORS_MNGRP_modeToString(sensorInfoP->sensorCfg.resolutionId), sensorInfoP->sensorCfg.fps);
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         GEN_SENSOR_DRVG_sensorParametersT params;
         params.triggerModeParams.triggerSrc   = sensorInfoP->sensorCfg.triggerSrc;
         params.triggerModeParams.triggerDelay = sensorInfoP->sensorCfg.triggerDelay;
         params.triggerModeParams.gpioBitmap    = 0;

         //OV9282 current configuration does not allow trigger mode in higher fps then 25. Each activation generates two frames inside the sensor.
         //first frame is corrupted, and we mask it. With large exposure time, the two frames period is larger then 1/30.
         //TODO - get better config from OV FAE
         if (((sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_9782_E)||(sensorInfoP->sensorCfg.sensorModel == INU_DEFSG_SENSOR_MODEL_OV_9282_E)) && (sensorInfoP->sensorCfg.fps > 25))
         {
            params.triggerModeParams.triggerSrc = INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E;
            LOGG_PRINT(LOG_WARN_E, NULL,"Sensor %d OV9282 activated in trigger mode with fps above 25, force free running mode\n",sensorInfoP->sensorCfg.sensorSelect);
         }
         /*
            Disable trigger mode when alternate is active. The trigger mode adds complexity on top of alternate which
            we want to avoid
         */
         if (ALTG_isActive(sensorInfoP->altH))
         {
            LOGG_PRINT(LOG_INFO_E, NULL,"Alternate mode is active, with trigger mode. Disable trigger mode\n");
            params.triggerModeParams.triggerSrc = INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E;
         }
         retCode = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_CONFIG_SENSOR_E, &params);
         sensorInfoP->gpioBitmap = params.triggerModeParams.gpioBitmap;
         sensorInfoP->pulseTime = params.triggerModeParams.pulseTime;
         sensorInfoP->sensorCfg.isTriggerSupported = params.triggerModeParams.isTriggerSupported;
      }
      if(ERRG_SUCCEEDED(retCode))
      {
         sensorInfoP->sensorActivity = TRUE;
      }
   }
   return(retCode);
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_runtimeGetConfigSensor
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
ERRG_codeE SENSORS_MNGRP_runtimeGetConfigSensor(IO_HANDLE sensorHandle, inu_sensor__runtimeCfg_t *cfgP)
{
   ERRG_codeE                 retCode        = SENSORS_MNGR__RET_SUCCESS;

   if(sensorHandle)
   {
      retCode = SENSORS_MNGRG_getSensorTemperature(sensorHandle, &cfgP->temperature);
      retCode = SENSORS_MNGRG_getSensorExpTime(sensorHandle, &cfgP->exposure, cfgP->context);
      if (ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRG_getSensorGain(sensorHandle, &cfgP->gain.analog, &cfgP->gain.digital, cfgP->context);
      }
      if (ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRP_getSensorFps(sensorHandle, &cfgP->fps, cfgP->context);
      }
   }
   return(retCode);
}



/****************************************************************************
*
*  Function Name: SENSORS_MNGRP_runtimeSetConfigSensor
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
ERRG_codeE SENSORS_MNGRP_runtimeSetConfigSensor(IO_HANDLE sensorHandle, inu_sensor__runtimeCfg_t *cfgP)
{
   ERRG_codeE                 retCode        = SENSORS_MNGR__RET_SUCCESS;

   if(sensorHandle)
   {
      retCode = SENSORS_MNGRG_setSensorExpTime(sensorHandle, &cfgP->exposure, cfgP->context);
      if (ERRG_SUCCEEDED(retCode))
      {
         retCode = SENSORS_MNGRG_setSensorGain(sensorHandle, &cfgP->gain.analog, &cfgP->gain.digital, cfgP->context);
      }

      //SDK removed the FPS configuration in prevous versions, for compatibility consideration
      //Only support changing FPS dynamically for VD56G3,VD55G0 & OS05A10
      if (ERRG_SUCCEEDED(retCode))
      {
         SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
         GEN_SENSOR_DRVG_specificDeviceDescT *specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);

         if (specificDev->sensorModel != INU_DEFSG_SENSOR_MODEL_VD56G3_E
             && specificDev->sensorModel != INU_DEFSG_SENSOR_MODEL_VD55G0_E
             && specificDev->sensorModel != INU_DEFSG_SENSOR_MODEL_OS05A10_E) {
            return retCode;
         }

         if (cfgP->fps == 0) {
            return retCode;
         }

         if (sensorInfoP->triggerH) {
            //Only change the sensorCfg.fps to notify the trigger manager
            //Not changing the settings of sensor itself(user must ensure the XML has maximum FPS configuration)
            retCode = TRIGGER_MNGRG_changeFps(sensorInfoP->triggerH, cfgP->fps);
         } else {
            //Not triggered by external source, config sensor hw
            retCode = SENSORS_MNGRP_setSensorFps(sensorHandle, &cfgP->fps, cfgP->context);
         }
      }
   }
   return(retCode);
}

static INT32 SENSORS_MNGRP_findFreeTableSlot()
{
   UINT32 i=0;
   for (i=0;i<INU_DEFSG_NUM_OF_INPUT_SENSORS;++i)
   {
      if ((SENSORS_MNGRP_sensorsInfo[i].sensorHandle == 0) && (SENSORS_MNGRP_sensorsInfo[i].sensorCfg.sensorSelect==0))
      {
         return i;
      }
   }
   return -1;
}

static INT32 SENSORS_MNGRP_findFreeSensorHandle()
{
   UINT32 i=0;
   for (i=0;i<sizeof(sensorIndexTbl)/sizeof(UINT32);++i)
   {
      if (sensorIndexTbl[i]==0)
      {
         sensorIndexTbl[i]=1;
         return i;
      }
   }
   return -1;
}

static void SENSORS_MNGRP_freeSensorHandle(UINT32 index)
{
   sensorIndexTbl[index]=0;
}

void *SENSORS_MNGRG_findSensorInfo(INU_DEFSG_sensorModelE model)
{
    UINT32 i;
    SENSORS_MNGRG_sensorInfoT *SensorInfo;

    for (i = 0; i < sizeof(sensorIndexTbl)/sizeof(int); i++) {
            if (sensorIndexTbl[i]) {
                    SensorInfo = &SENSORS_MNGRP_sensorsInfo[i];
                    if (SensorInfo->sensorHandle && SensorInfo->sensorCfg.sensorModel == model) {
                            return (void *)SensorInfo;
                    }
            }
    }

    return NULL;
}

/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_startSensor
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
ERRG_codeE SENSORS_MNGRG_startSensor(IO_HANDLE ioHandle)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   IO_HANDLE                  sensorHandle   = sensorInfoP->sensorHandle;

   if (sensorInfoP->sensorActivity == TRUE)
   {
      if(sensorInfoP->sensorCfg.sensorFunc == INU_DEFSG_SEN_MASTER_E)
      {
         if(sensorHandle)
         {
            GEN_SENSOR_DRVG_sensorParametersT params;
            params.triggerModeParams.isTriggerSupported = sensorInfoP->sensorCfg.isTriggerSupported;
            sensorInfoP->strobeCtr = 0;
            sensorInfoP->strobeTs = 0;
            const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
            #ifdef STORE_HANDLE_IN_SENSOR_ENABLE
            LOGG_PRINT(LOG_INFO_E,NULL,"Storing sensor handle (for metadata updates) with sensor ID:%lu, Handle:0x%x  \n",sensorInfoP->sensorCfg.sensorSelect,sensorInfoP);
            METADATA_UPDATER_storeSensorHandle(modelType,(sensorInfoP),sensorInfoP->sensorCfg.sensorSelect);
            #endif
            LOGG_PRINT(LOG_INFO_E, NULL,"activate sensor(s) (sensor id - %d, trigger mode, handle:0x%x %s)\n",
                       sensorInfoP->sensorCfg.sensorSelect,sensorInfoP, !params.triggerModeParams.isTriggerSupported ? "disabled" : "enabled");
            if (sensorInfoP->sensorCfg.triggerSrc != INU_DEFSG_SENSOR_TRIGGER_SRC_DISABLE_E)
            {
               ret = TRIGGER_MNGRG_manageStartTrigger(sensorInfoP, &sensorInfoP->triggerH);
               if (ERRG_FAILED(ret))
                  return ret;
            }

            ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_START_SENSOR_E, &params);
         }
      }
      else
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"activate sensor fail. undefined sensor functionality (%d)\n", sensorInfoP->sensorCfg.sensorFunc);
         ret = SENSORS_MNGR__ERR_START_FAIL;
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: SENSORS_MNGRG_stopSensor
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
ERRG_codeE SENSORS_MNGRG_stopSensor(IO_HANDLE ioHandle)
{
   ERRG_codeE                 ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)ioHandle;
   IO_HANDLE                  sensorHandle   = sensorInfoP->sensorHandle;

   if (sensorInfoP->sensorActivity && sensorHandle)
   {
      LOGG_PRINT(LOG_INFO_E, NULL,"stop stereo sensors (stereo sensor id - %d)\n",sensorInfoP->sensorCfg.sensorSelect);
      ret = TRIGGER_MNGRG_manageStopTrigger(sensorInfoP);
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret,"failed to stop trigger on sensor %d\n",sensorInfoP->sensorCfg.sensorSelect);
         return ret;
      }
      ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_STOP_SENSOR_E, NULL);
   }
   return ret;
}

ERRG_codeE SENSORS_MNGRG_setConfigSensor( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   inu_nodeH sensorGroupP,sensor;
   IO_HANDLE sensorHandle,sensorGroupHandle;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP;
   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;

   (void)hwDb;(void)block;(void)blkInstance;
   ret = inu_graph__getOrigSensGroup(arg, &sensorGroupP);
   if (ERRG_SUCCEEDED(ret))
   {
      //check if group is configured
      ret = inu_sensors_group__getSensorHandle(sensorGroupP,&sensorGroupHandle);
      if (ERRG_SUCCEEDED(ret))
      {
         sensorInfoP = sensorGroupHandle;
         if(sensorInfoP->sensorActivity == FALSE)
         {
            //power down all sensors
            sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
            while(sensor)
            {
               inu_sensor__getSensorHandle(sensor,&sensorHandle);
               SENSORS_MNGRP_powerdown(sensorHandle);
               sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
            }

            //change address one sensor at a time
            sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
            int ctr = 0xB0;
            while(sensor)
            {
               inu_sensor__getSensorHandle(sensor,&sensorHandle);
               SENSORS_MNGRP_powerup(sensorHandle);
               SENSORS_MNGRP_changeAddress(sensorHandle, ctr);
               ctr++;
               ctr++;
               sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
            }
            specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(((SENSORS_MNGRG_sensorInfoT  *)sensorGroupHandle)->sensorHandle);
            if (specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E))
            {
            // addresses need to be taken from XML
               specificDev->sensorAddress = 0x30;
               ret = SENSORS_MNGRP_configSensor(sensorGroupHandle);
               specificDev->sensorAddress = 0x20;
            }

            if (specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_CGS_132_E))
            {
            // addresses need to be taken from XML
               specificDev->sensorAddress = 0x60;
               ret = SENSORS_MNGRP_configSensor(sensorGroupHandle);
               specificDev->sensorAddress = 0x60;
            }
            if (specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OV_4689_E))
            {
            // addresses need to be taken from XML
               specificDev->sensorAddress = 0x6c;
               ret = SENSORS_MNGRP_configSensor(sensorGroupHandle);
               specificDev->sensorAddress = 0x20;
            }
#ifdef HP_CASE
            if (specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OV_5675_E))
            {
               specificDev->sensorAddress = 0x50;//sould be not hard coded. but in sensor_mngr_open(), if function=master-->stereo. and stereo-->global address
               //we change here the sensor group address. in ov5675&ov8856 we only one sensor at the sensor group, we want to config them seperate and start on common address
            }
            if (specificDev && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OV_8856_E))
            {
               specificDev->sensorAddress = 0x80;//sould be not hard coded. but in sensor_mngr_open(), if function=master-->stereo. and stereo-->global address
               //we change here the sensor group address. in ov5675&ov8856 we only one sensor at the sensor group, we want to config them seperate and start on common address
            }
#endif
            if (specificDev
                    && (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_VD55G0_E
                        || specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_VD56G3_E)) {
                GEN_SENSOR_DRVG_specificDeviceDescT *sensorMember;
                //VD55G0&VD56G3 does not have a broadcast address, we change the sensor group address
                //to one of the sensors
                sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
                while(sensor)
                {
                    inu_sensor__getSensorHandle(sensor,&sensorHandle);
                    sensorMember = GEN_SENSOR_DRVG_getSensorSpecificHandle(((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorHandle);
                    specificDev->sensorAddress = sensorMember->sensorAddress;
                    ret = SENSORS_MNGRP_configSensor(sensorGroupHandle);
                    sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
                }
            }
            else if( specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OS05A10_E) 
            {
                GEN_SENSOR_DRVG_specificDeviceDescT *sensorMember;
                /* OS05A10 sensors[VST_FF] are on differnt i2c buses */
                sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
                while(sensor)
                {
                    inu_sensor__getSensorHandle(sensor,&sensorHandle);
                    sensorMember = GEN_SENSOR_DRVG_getSensorSpecificHandle(((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorHandle);
                    specificDev->sensorAddress = sensorMember->sensorAddress;
                    /*I2C instance ID has to copied because the VST FF sensors are on different buses*/
                    specificDev->i2cInstanceId = sensorMember->i2cInstanceId;
                    ret = SENSORS_MNGRP_configSensor(sensorGroupHandle);
                    sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
                }
            }
            else
            //in case of stereo from 2 different sensors, we have to config each sensors from the sensor gruop separate, each with uniqe address
            if (sensorInfoP->sensorCfg.tableType == INU_DEFSG_STEREO_2_DIFF_SENSORS_E)
            {
               sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
               while(sensor)
               {
                  inu_sensor__getSensorHandle(sensor,&sensorHandle);
                  ret = SENSORS_MNGRP_configSensor(sensorHandle);


                  sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
               }

               sensorInfoP->sensorActivity = TRUE;
            }
            else
            {
               ret = SENSORS_MNGRP_configSensor(sensorGroupHandle);
            }

            ret = inu_graph__getOrigSensGroup(arg, &sensorGroupP);
            sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
            while(sensor)
            {
               //Run separate configuration for each sensor in group
               inu_sensor__getSensorHandle(sensor,&sensorHandle);
               ret = SENSORS_MNGRP_setOrientation(((SENSORS_MNGRG_sensorInfoT *)sensorHandle)->sensorHandle, ((SENSORS_MNGRG_sensorInfoT *)sensorHandle)->sensorCfg.orientation);
               sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
            }
         }
      }
   }
   return ret;
}

ERRG_codeE SENSORS_MNGRG_setEnableSensor( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE              ret           = SENSORS_MNGR__RET_SUCCESS;
   inu_nodeH sensorGroupP,sensor;
   IO_HANDLE sensorHandle,sensorGroupHandle;

   SENSORS_MNGRG_sensorInfoT *sensorInfoP;
   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;
   UINT32 mipiId[MIPI_MNGRG_MAX_PHY_TO_SENSOR];
   UINT32 mipiCnt = 0;
   (void)hwDb;(void)block;(void)instanceList;(void)voterHandle;
   ret = inu_graph__getOrigSensGroup(arg, &sensorGroupP);
   if (ERRG_SUCCEEDED(ret))
   {
#if 1
      sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
      while(sensor)
      {
         mipiCnt = 0;
         MIPI_MNGRG_getMipiRxTable(hwDb, NUFLD_MIPI_RX_E, inu_sensor__getId(sensor), mipiId, &mipiCnt );

         MIPI_MNGRG_setConfigRx(hwDb, mipiId, mipiCnt, NULL );

         sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
      }
#endif
      sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
      while(sensor)
      {
         inu_sensor__getSensorHandle(sensor,&sensorHandle);
         ret = SENSORS_MNGRP_setInitExposureGainVals(sensorHandle);
         sensorInfoP = (SENSORS_MNGRG_sensorInfoT*)sensorHandle;
         sensorInfoP->sensorActivity = TRUE;
         sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
      }

      ret = inu_sensors_group__getSensorHandle(sensorGroupP,&sensorGroupHandle);
      if (ERRG_SUCCEEDED(ret))
      {
         const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
         sensorInfoP = (SENSORS_MNGRG_sensorInfoT*)sensorHandle;
         METADATA_UPDATER_storeGroupHandle(modelType,(sensorGroupP));
         specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(((SENSORS_MNGRG_sensorInfoT  *)sensorGroupHandle)->sensorHandle);
         if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_AR_0234_E)
         {
            //TODO: Test storing sensor handles for this type of sensor
            SENSORS_MNGRG_sensorInfoT *storedHandle = (SENSORS_MNGRG_sensorInfoT  *)sensorGroupHandle;
            const INU_DEFSG_senSelectE sensorSelect = storedHandle->sensorCfg.sensorSelect;
            METADATA_UPDATER_storeSensorExposureAndGains(modelType,storedHandle,sensorSelect);
           // addresses need to be taken from XML
            specificDev->sensorAddress = 0x30;
            ret = SENSORS_MNGRP_setInitExposureGainVals(sensorGroupHandle);
            ret = SENSORS_MNGRG_startSensor(sensorGroupHandle);
            specificDev->sensorAddress = 0x20;
         }
         if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_CGS_132_E)
         {
            //TODO: Test storing sensor handles for this type of sensor
            SENSORS_MNGRG_sensorInfoT *storedHandle = (SENSORS_MNGRG_sensorInfoT  *)sensorGroupHandle;
            const INU_DEFSG_senSelectE sensorSelect = storedHandle->sensorCfg.sensorSelect;
            METADATA_UPDATER_storeSensorExposureAndGains(modelType,storedHandle,sensorSelect);
            // addresses need to be taken from XML
            specificDev->sensorAddress = 0x60;
            ret = SENSORS_MNGRP_setInitExposureGainVals(sensorGroupHandle);
            ret = SENSORS_MNGRG_startSensor(sensorGroupHandle);
            specificDev->sensorAddress = 0x60;
         }
         if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OV_4689_E)
         {

            specificDev->sensorAddress = 0x6c;
            SENSORS_MNGRP_changeAddress(sensorGroupHandle, 1); // change both addresses to 0x20
            specificDev->sensorAddress = 0x20;
            SENSORS_MNGRG_sensorInfoT *storedHandle = (SENSORS_MNGRG_sensorInfoT  *)sensorGroupHandle;
            const INU_DEFSG_senSelectE sensorSelect = storedHandle->sensorCfg.sensorSelect;
            METADATA_UPDATER_storeSensorExposureAndGains(modelType,storedHandle,sensorSelect);
            ret = SENSORS_MNGRP_setInitExposureGainVals(sensorGroupHandle);
            ret = SENSORS_MNGRG_startSensor(sensorGroupHandle);
            SENSORS_MNGRP_changeAddress(sensorGroupHandle, 0); // seperate address
         }
         if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_VD55G0_E
                         || specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_VD56G3_E) {
                GEN_SENSOR_DRVG_specificDeviceDescT *sensorMember;
                sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
                while(sensor)
                {
                    inu_sensor__getSensorHandle(sensor,&sensorHandle);
                    sensorMember = GEN_SENSOR_DRVG_getSensorSpecificHandle(((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorHandle);
                    SENSORS_MNGRG_sensorInfoT *storedHandle = (SENSORS_MNGRG_sensorInfoT  *)(sensorGroupHandle);
                    specificDev->sensorAddress = sensorMember->sensorAddress;
                    const INU_DEFSG_senSelectE sensorSelect = ((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorCfg.sensorSelect;
                    METADATA_UPDATER_storeSensorExposureAndGains(modelType,(sensorHandle),sensorSelect);
                    ret = SENSORS_MNGRP_setInitExposureGainVals(sensorGroupHandle);
                    ret = SENSORS_MNGRG_startSensor(sensorGroupHandle);
                    sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
                }
         }
         else if(specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_OS05A10_E)
         {
            GEN_SENSOR_DRVG_specificDeviceDescT *sensorMember;
                sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
               while(sensor)
                {
                    inu_sensor__getSensorHandle(sensor,&sensorHandle);
                    sensorMember = GEN_SENSOR_DRVG_getSensorSpecificHandle(((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorHandle);
                    SENSORS_MNGRG_sensorInfoT *storedHandle = (SENSORS_MNGRG_sensorInfoT  *)(sensorGroupHandle);
                    specificDev->sensorAddress = sensorMember->sensorAddress;
                    specificDev->i2cInstanceId = sensorMember->i2cInstanceId;
                    LOGG_PRINT(LOG_INFO_E,NULL,"Starting sensor using I2C Instance ID %lu, sensorHandle:%p",specificDev->i2cInstanceId,sensorHandle );
                    const INU_DEFSG_senSelectE sensorSelect = ((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorCfg.sensorSelect;
                    METADATA_UPDATER_storeSensorExposureAndGains(modelType,(sensorHandle),sensorSelect);
                    ret = SENSORS_MNGRP_setInitExposureGainVals(sensorGroupHandle);
                    ret = SENSORS_MNGRG_startSensor(sensorGroupHandle);
                    sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
                }
         }
         else
         {
            /* We need to store the sensor handle before starting the sensors otherwise the update to the GME registers in the trigger manager won't work*/
            SENSORS_MNGRG_sensorInfoT *storedHandle = (SENSORS_MNGRG_sensorInfoT  *)sensorGroupHandle;
            const INU_DEFSG_senSelectE sensorSelect = storedHandle->sensorCfg.sensorSelect;
            METADATA_UPDATER_storeSensorExposureAndGains(modelType,storedHandle,sensorSelect);
            ret = SENSORS_MNGRP_setInitExposureGainVals(sensorGroupHandle);
            ret = SENSORS_MNGRG_startSensor(sensorGroupHandle);

            if (ERRG_SUCCEEDED(ret))
            {
               sensor = inu_node__getNextOutputNode(sensorGroupP, NULL);
               while (sensor)
               {
                  inu_sensor__getSensorHandle(sensor, &sensorHandle);
                  const INU_DEFSG_senSelectE sensorSelect = ((SENSORS_MNGRG_sensorInfoT  *)sensorHandle)->sensorCfg.sensorSelect;
                  METADATA_UPDATER_storeSensorExposureAndGains(modelType,(sensorHandle),sensorSelect);
                  if (ERRG_SUCCEEDED(FOCUS_CTRLG_isFocusCtlSupported(sensorHandle)))
                  {
                     inu_sensor__setFocusParams_t paramsP;

                     sensorInfoP = (SENSORS_MNGRG_sensorInfoT*)sensorHandle;
                     sensorInfoP->focusCtrlSupport = 1;

                     paramsP.context = INU_SENSOR__CONTEXT_A;
                     paramsP.mode = INU_SENSOR__FOCUS_MODE_INFINITY;
                     ret = FOCUS_CTRLG_setFocusParams(sensorHandle, &paramsP);
                  }
                  sensor = inu_node__getNextOutputNode(sensorGroupP, sensor);
               }
            }
         }

      }
   }
   return ret;
}


ERRG_codeE SENSORS_MNGRG_setDisableSensor( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE              ret           = SENSORS_MNGR__RET_SUCCESS;
   inu_nodeH sensorGroupP,sensor;
   IO_HANDLE sensorHandle;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP;
   UINT8 mipiInstance[NU4K_NUM_MIPI_RX];
   memset(mipiInstance, 0xFF, NU4K_NUM_MIPI_RX);
   UINT32 mipiIds[MIPI_MNGRG_MAX_PHY_TO_SENSOR];
   UINT32 mipiCnt = 0;


   (void)hwDb;(void)block;(void)instanceList;
   ret = inu_graph__getOrigSensGroup(arg, &sensorGroupP);
   if (ERRG_SUCCEEDED(ret))
   {
      sensor = inu_node__getNextOutputNode(sensorGroupP,NULL);
      while(sensor)
      {
         MIPI_MNGRG_getMipiRxTable(hwDb, NUFLD_MIPI_RX_E, inu_sensor__getId(sensor), mipiIds, &mipiCnt);
         MIPI_MNGRG_setDisableRx(hwDb, NUFLD_MIPI_RX_E, mipiIds, mipiCnt , NULL );
         inu_sensor__getSensorHandle(sensor,&sensorHandle);
         sensorInfoP = sensorHandle;
         sensorInfoP->sensorActivity = FALSE;
         sensor = inu_node__getNextOutputNode(sensorGroupP,sensor);
      }
      ret = inu_sensors_group__getSensorHandle(sensorGroupP,&sensorHandle);
      if (ERRG_SUCCEEDED(ret))
      {
         sensorInfoP = sensorHandle;
         ret = SENSORS_MNGRG_stopSensor(sensorHandle);
         sensorInfoP->sensorActivity = FALSE;
      }
   }
   return ret;
}

ERRG_codeE SENSORS_MNGRG_getChipID(IO_HANDLE sensorHandle, UINT32 *chipID)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_GET_CHIP_ID_E, chipID);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_readReg(IO_HANDLE sensorHandle, UINT16 address, UINT32 *value)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   if (!value) {
       return SENSORS_MNGR__ERR_INVALID_ARGS;
   }

   specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);

   config.accessRegParams.data = 0;
   if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_GC_2145_E)
   {
       GEN_SENSOR_DRVG_READ_SENSOR_ADDR8_REG(config, specificDev, address, 1, ret);
   }
   else
   {
       GEN_SENSOR_DRVG_READ_SENSOR_REG(config, specificDev, address, 1, ret);
   }

   if (ERRG_SUCCEEDED(ret))
   {
       *value = config.accessRegParams.data;
   }

   return ret;
}

ERRG_codeE SENSORS_MNGRG_writeReg(IO_HANDLE sensorHandle, UINT16 address, UINT32 value)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_specificDeviceDescT *specificDev;
   GEN_SENSOR_DRVG_sensorParametersT   config;

   if (!value) {
       return SENSORS_MNGR__ERR_INVALID_ARGS;
   }

   specificDev = GEN_SENSOR_DRVG_getSensorSpecificHandle(sensorInfoP->sensorHandle);

   if (specificDev->sensorModel == INU_DEFSG_SENSOR_MODEL_GC_2145_E)
   {
       GEN_SENSOR_DRVG_WRITE_SENSOR_ADDR8_REG(config, specificDev, address, value, 1, ret);
   }
   else
   {
       GEN_SENSOR_DRVG_WRITE_SENSOR_REG(config, specificDev, address, value, 1, ret);
   }

   return ret;
}

ERRG_codeE SENSORS_MNGRG_activateTestPattern(IO_HANDLE sensorHandle, BOOLEAN enable)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_TEST_PATTERN_E, &enable);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_setBayerPattern(IO_HANDLE sensorHandle, UINT8 pattern)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_BAYER_PATTERN_E, &pattern);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_setBLC(IO_HANDLE sensorHandle, UINT32 r, UINT32 gr, UINT32 gb, UINT32 b)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_sensorParametersT param;

   param.rgbGainParams.r = r;
   param.rgbGainParams.gr = gr;
   param.rgbGainParams.gb = gb;
   param.rgbGainParams.b = b;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_BLC_E, &param);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_setWB(IO_HANDLE sensorHandle, UINT32 r, UINT32 gr, UINT32 gb, UINT32 b)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_sensorParametersT param;

   param.rgbGainParams.r = r;
   param.rgbGainParams.gr = gr;
   param.rgbGainParams.gb = gb;
   param.rgbGainParams.b = b;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_SET_WB_E, &param);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_changeResolution(IO_HANDLE sensorHandle, UINT32 width, UINT32 height)
{
   ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;
   GEN_SENSOR_DRVG_sensorParametersT param;

   param.resolutionParams.width = width;
   param.resolutionParams.height = height;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_CHANGE_RESOLUTION_E, &param);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_exposureControl(IO_HANDLE sensorHandle, void *param)
{
    ERRG_codeE                       ret = SENSORS_MNGR__RET_SUCCESS;
    SENSORS_MNGRG_sensorInfoT        *sensorInfoP = (SENSORS_MNGRG_sensorInfoT *)sensorHandle;

   ret = IO_PALG_ioctl(sensorInfoP->sensorHandle, GEN_SENSOR_DRVG_EXPOSURE_CTRL_E, (GEN_SENSOR_DRVG_sensorParametersT *)param);

   return ret;
}

ERRG_codeE SENSORS_MNGRG_open(IO_HANDLE *ioHandleP,INUG_ioctlSensorConfigT *sensorConfig)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_openParametersT  sensorParams;
   IO_HANDLE                        ioHandle;
   INT32 tableHandleIndex=0;
   INT32 tableSlotIndex = SENSORS_MNGRP_findFreeTableSlot();
   if (tableSlotIndex==-1)
   {
       return SENSORS_MNGR__ERR_OUT_OF_RSRCS;
   }
   SENSORS_MNGRG_sensorInfoT*  sensorInfoP = &(SENSORS_MNGRP_sensorsInfo[tableSlotIndex]);
   sensorInfoP->sensorCfg = *sensorConfig;
   MEM_MAPG_getVirtAddr( (MEM_MAPG_REG_ISPI_E +  sensorConfig->i2cNum), (&sensorParams.i2cControllerAddress));
   sensorParams.sensorInitFunc = SENSORS_MNGRP_getSensorInitFunc( sensorInfoP->sensorCfg.sensorModel);
   sensorParams.i2cInstanceId = sensorConfig->i2cNum;
   sensorParams.i2cSpeed      = I2C_HL_DRVG_SPEED_STANDARD_E;
   sensorParams.sensorModel   = sensorInfoP->sensorCfg.sensorModel;
   sensorParams.sensorType    = INU_DEFSG_SENSOR_TYPE_SINGLE_E;
   sensorParams.powerGpioMaster =  sensorInfoP->sensorCfg.powerGpioMaster;
   sensorParams.powerGpioSlave =  sensorInfoP->sensorCfg.powerGpioSlave; //todo : remove
   sensorParams.fsinGpio =  sensorInfoP->sensorCfg.fsinGpio;
   sensorParams.sensorClk =  sensorInfoP->sensorCfg.sensorClkSrc;
   sensorParams.sensorClkDiv = sensorInfoP->sensorCfg.sensorClkDiv;
   sensorParams.tableType = sensorInfoP->sensorCfg.tableType;
   sensorParams.sensorId = sensorInfoP->sensorCfg.sensorId;
   sensorParams.groupId = sensorInfoP->sensorCfg.groupId;
   sensorParams.isSensorGroup = sensorInfoP->sensorCfg.isSensorGroup;
   sensorParams.sensorSelect = sensorInfoP->sensorCfg.sensorSelect;
   const INU_DEFSG_moduleTypeE modelType = GME_MNGRG_getModelType();
 //  sensorParams->sensorAddr = sensorInfoP->sensorCfg.sensorAddr
   if (sensorInfoP->sensorCfg.sensorFunc == INU_DEFSG_SEN_MASTER_E)
   {
      sensorParams.sensorType = INU_DEFSG_SENSOR_TYPE_STEREO_E; //todo: remove
   }
   tableHandleIndex = SENSORS_MNGRP_findFreeSensorHandle();
   if (tableHandleIndex !=-1)
   {
      if(sensorInfoP->sensorCfg.strobeNum != INU_DEFSG_STROBES_INVALID)
      {
         OS_LYRG_aquireMutex(&sensorInfoP->strobeTsMutex);
         SENSORS_MNGRP_setStrobeIsr(SENSORS_MNGRP_strobeIsr,sensorInfoP->sensorCfg.strobeNum,(void*)sensorInfoP);
      }
      sensorInfoP->sensorTableHandleIndex = (UINT32)tableHandleIndex;
      ret = IO_PALG_open(&ioHandle, IO_SENSOR_CAM0_E+ tableHandleIndex, &sensorParams);
      sensorInfoP->sensorHandle = ioHandle;
      if (ERRG_SUCCEEDED(ret))
      {
         *ioHandleP = sensorInfoP;
         SENSORS_MNGRP_showSensorCfg(LOG_DEBUG_E,sensorInfoP);
      }
    }
    else
    {
       ret= SENSORS_MNGR__ERR_OUT_OF_RSRCS;
    }
   return ret;
}

ERRG_codeE SENSORS_MNGRG_close(IO_HANDLE ioHandle)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   SENSORS_MNGRG_sensorInfoT  *sensorInfoP = (SENSORS_MNGRG_sensorInfoT  *)ioHandle;
   if(sensorInfoP->sensorCfg.strobeNum != INU_DEFSG_STROBES_INVALID)
   {
      ret = SENSORS_MNGRP_releaseStrobeIsr(sensorInfoP->sensorCfg.strobeNum);
      OS_LYRG_releaseMutex(&sensorInfoP->strobeTsMutex);
   }

   if (ERRG_SUCCEEDED(ret))
   {
      SENSORS_MNGRP_freeSensorHandle(sensorInfoP->sensorTableHandleIndex);
      ret = IO_PALG_close(sensorInfoP->sensorHandle);
      sensorInfoP->sensorHandle=0;
      sensorInfoP->sensorCfg.sensorSelect=0;
      sensorInfoP->sensorActivity = FALSE;
   }
   return ret;
}

UINT16 SENSORS_MNGRG_getSensorPixelClock(IO_HANDLE ioHandle)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_SENSOR_DRVG_sensorParametersT params;
   if (ioHandle)
   {
      ret = IO_PALG_ioctl(ioHandle, GEN_SENSOR_DRVG_GET_PIXEL_CLOCK_E, &params);
   }
   if (ERRG_FAILED(ret))
   {
      assert(0);
   }
   return params.sensorClocksParams.pixelClkMhz;
}


ERRG_codeE SENSORS_MNGRG_resetStrobeCnt(IO_HANDLE ioHandle)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   int mret;
   SENSORS_MNGRG_sensorInfoT  *sensorInfoP = (SENSORS_MNGRG_sensorInfoT  *)ioHandle;

   OS_LYRG_lockMutex(&sensorInfoP->strobeTsMutex);
   sensorInfoP->strobeCtr = 0;
   OS_LYRG_unlockMutex(&sensorInfoP->strobeTsMutex);

   return ret;

}


#ifdef __cplusplus
   }
#endif
