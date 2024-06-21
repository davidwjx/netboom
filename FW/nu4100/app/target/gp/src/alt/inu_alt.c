/****************************************************************************
 * 
 *   FileName: inu_alt.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: alternate module
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/

#include "inu_common.h"
#include "inu2.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "inu_alt.h"
#include "sensors_mngr.h"
#include "gen_sensor_drv.h"
#include "inu2.h"
#include "assert.h"

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#if 0
typedef struct
{
   UINT64               timestamp;
} CHARGERP_msgQParamsT;


#define CHARGERP_MSGQ_NAME      "/ChargerMsgQue"
#define CHARGERP_MSGQ_SIZE      (sizeof(CHARGERP_msgQParamsT))
#define CHARGERP_MSGQ_MAX_MSG   (20)
#define CHARGERP_MSGQ_TIMEOUT   (5000)
#endif

//#define ALT_DEBUG
#define ALTERNATEP_MAX_NUM_INSTANCES (3)
/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct
{
   BOOL                        active;
   OS_LYRG_threadHandle        thrdH;
   BOOL                        threadIsActive;
   OS_LYRG_msgQueT             msgQue;
   UINT8                       version;
   UINT64                      strobeTick;
   UINT32                      currentMode;
   UINT32                      thisFrameMode;
   UINT32                      pendingModeChange;
   UINT32                      ticksLeftInMode;
   UINT32                      numTicksInMode[ALTG_MAX_NUM_MODES];
   UINT32                      numModes;
} ALTERNATEP_infoT;

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static ALTERNATEP_infoT ALTP_params[ALTERNATEP_MAX_NUM_INSTANCES];

 /****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
#if 0
static void CHARGERP_handleMsg(CHARGERP_infoT *paramsP, CHARGERP_msgQParamsT *msgQParamsP)
{
   ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;
   UINT8      status;
   
   ret = CHARGERP_readReg(paramsP, INTERRUPT_STATUS_REGISTER_ADDR, &status);
   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "isr status = 0x%x, (%llu)\n",status,msgQParamsP->timestamp);
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read isr status\n");
   }   
}


static int CHARGERP_thread(void *argP)
{
   CHARGERP_msgQParamsT    msg;
   INT32                   status   = 0;
   UINT32                  msgSize  = 0;
   CHARGERP_infoT          *paramsP = (CHARGERP_infoT*)argP;

   LOGG_PRINT(LOG_INFO_E, NULL, "thread started\n");
   paramsP->threadIsActive = 1;
   while(paramsP->threadIsActive)
   {
      msgSize  = sizeof(msg);
      status   = OS_LYRG_recvMsg(&paramsP->msgQue, (UINT8*)&msg, &msgSize, CHARGERP_MSGQ_TIMEOUT);
      if((status == SUCCESS_E) && (msgSize == sizeof(msg)))
      {
         CHARGERP_handleMsg(paramsP, &msg);
      }
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "thread exited\n");

   OS_LYRG_exitThread();

   return status;
}


static ERRG_codeE CHARGERP_createThread( CHARGERP_infoT *paramsP )
{
   //Early initializations on GP before service open . Assumed called once 
   ERRG_codeE           retCode = (ERRG_codeE)RET_SUCCESS;
   OS_LYRG_threadParams thrParams;

   memcpy(paramsP->msgQue.name, CHARGERP_MSGQ_NAME, sizeof(CHARGERP_MSGQ_NAME));
   paramsP->msgQue.maxMsgs = CHARGERP_MSGQ_MAX_MSG;
   paramsP->msgQue.msgSize = CHARGERP_MSGQ_SIZE;

   //Create msg queue
   if(OS_LYRG_createMsgQue(&paramsP->msgQue, OS_LYRG_MULTUPLE_EVENTS_NUM_E) != SUCCESS_E)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "msg queue create failed\n");
      return ERR_UNEXPECTED;
   }

   //Invoke thread
   thrParams.func    = CHARGERP_thread;
   thrParams.id      = OS_LYRG_CHARGER_THREAD_ID_E;
   thrParams.event   = NULL;
   thrParams.param   = paramsP;
   paramsP->thrdH    = OS_LYRG_createThread(&thrParams);
   if(!paramsP->thrdH)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "thread create failed\n");
      retCode = HW_MNGR__ERR_CREATE_THREAD_FAIL;
   }
   return retCode;
}
#endif
void ALTP_updateCounters( ALTERNATEP_infoT *paramsP )
{
   if (!paramsP->ticksLeftInMode)
      assert(0);

   if (paramsP->pendingModeChange)
   {
      paramsP->thisFrameMode = paramsP->currentMode;
      paramsP->pendingModeChange = 0;
   }
   
   paramsP->ticksLeftInMode--;
   if (paramsP->ticksLeftInMode)
   {
      //printf("ticks left in mode %d - %d\n",paramsP->currentMode,paramsP->ticksLeftInMode);
   }
   else
   {
      paramsP->currentMode++;
      paramsP->currentMode %= paramsP->numModes;
      paramsP->ticksLeftInMode = paramsP->numTicksInMode[paramsP->currentMode];
      paramsP->pendingModeChange = 1;
#ifdef ALT_DEBUG
      printf("new mode %d, num ticks %d\n",paramsP->currentMode,paramsP->ticksLeftInMode);
#endif
   }
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
int ALTG_getMode( inu_altH altH )
{
   ALTERNATEP_infoT        *paramsP = (ALTERNATEP_infoT*)altH;
   int mode;
   if ((paramsP) &&( paramsP->active))
   {
      mode = paramsP->currentMode;
   }
   else
   {
      mode = 0;
   }
   return mode;
}

/*
   This function cannot be called before ALTP_updateCounters() occured in frame period
*/
int ALTG_getThisFrameMode( inu_altH altH )
{
   ALTERNATEP_infoT        *paramsP = (ALTERNATEP_infoT*)altH;
   int                      prevMode;

   if ((paramsP) &&(paramsP->active))
   {
      prevMode = paramsP->thisFrameMode;
   }
   else
   {
      prevMode = 0;
   }
   return prevMode;
}


int ALTG_getNextFrameMode( inu_altH altH )
{
   ALTERNATEP_infoT        *paramsP = (ALTERNATEP_infoT*)altH;
   int mode;

   if (paramsP->ticksLeftInMode - 1)
   {
      //printf("ticks left in mode %d - %d\n",paramsP->currentMode,paramsP->ticksLeftInMode);
      mode = paramsP->currentMode;
   }
   else
   {
      mode = paramsP->currentMode + 1;
      mode %= paramsP->numModes;
#ifdef ALT_DEBUG
      printf("next mode %d, num ticks %d\n",mode,paramsP->ticksLeftInMode);
#endif
   }

   return mode;
}


ERRG_codeE ALTG_strobeHandle( inu_altH altH, void *sensorInfo1P)
{
   ERRG_codeE                 ret = INU_DEVICE__RET_SUCCESS;
   ALTERNATEP_infoT          *paramsP = (ALTERNATEP_infoT*)altH;
   SENSORS_MNGRG_sensorInfoT *sensorInfoP = (SENSORS_MNGRG_sensorInfoT*)sensorInfo1P;
   IO_HANDLE                  sensorHandle = sensorInfoP->sensorHandle;   

   if ((paramsP) &&(paramsP->active))
   {
#ifdef ALT_DEBUG
      UINT32 startMode = ALTG_getMode(altH);
#endif
      //1. Update counters
      ALTP_updateCounters(paramsP);

   
      //2. Handle projector toggle
      PROJ_DRVG_projSetValParamsT projSetValParams;
      projSetValParams.projNum = 0;
      if (ALTG_getMode(altH) == 1)
      {
         projSetValParams.projState = PROJ_DRVG_PROJ_STATE_CLEAR_E;
      }
      else
      {
         projSetValParams.projState = PROJ_DRVG_PROJ_STATE_PATTERN_HIGH_E;
      }
      
      ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_PROJ_0_E), PROJ_DRVG_SET_PROJ_VAL_CMD_E, &projSetValParams); 

#ifdef ALT_DEBUG
      //check for a pending exposure change
      UINT64 usec;
      UINT8 exp[3];
      OS_LYRG_getUsecTime(&usec);
      I2C_DRVG_read(I2C_DRVG_I2C_INSTANCE_1_E,0x3500,2,1,&exp[0],0xc0,0);
      I2C_DRVG_read(I2C_DRVG_I2C_INSTANCE_1_E,0x3501,2,1,&exp[1],0xc0,0);
      I2C_DRVG_read(I2C_DRVG_I2C_INSTANCE_1_E,0x3502,2,1,&exp[2],0xc0,0);   
#endif


      //3. Set strobe time, for the time of the next frame mode
      GEN_SENSOR_DRVG_exposureTimeCfgT    setStrobe;
      sensorInfoP->pendingContext = ALTG_getMode(altH);
      setStrobe.exposureTime = sensorInfoP->pendingExposure[sensorInfoP->pendingContext];
      setStrobe.context      = sensorInfoP->pendingContext;
      ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_STROBE_TIME_E, &setStrobe);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "strobe: failed to set strobe duration!!!\n");      
      }


      //4. Handle exposure toggle. The exposure we set now, is according to what the after next frame will be
      GEN_SENSOR_DRVG_exposureTimeCfgT    setExposure;
      sensorInfoP->pendingContext = ALTG_getNextFrameMode(altH);
      setExposure.exposureTime = sensorInfoP->pendingExposure[sensorInfoP->pendingContext];
      setExposure.context      = sensorInfoP->pendingContext;
      //LOGG_PRINT(LOG_INFO_E, NULL, "strobe: mode %d, nextFrame mode %d, thisFrameMode %d exp %d\n",ALTG_getMode(altH),ALTG_getNextFrameMode(altH),ALTG_getThisFrameMode(altH),setExposure.exposureTime); 
      ret = IO_PALG_ioctl(sensorHandle, GEN_SENSOR_DRVG_SET_EXPOSURE_TIME_E, &setExposure);
      if(ERRG_SUCCEEDED(ret))
      {
         SENSORS_MNGRG_updateSensorExpTime((IO_HANDLE)sensorInfoP,setExposure.exposureTime,setExposure.context);
      }
      else
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "strobe: failed to set exposure!!!\n");
      }

#ifdef ALT_DEBUG
      printf("%llu: applied mode %d (mode when entered isr %d) (%d,%d), ( current sensor exp 0x%x%x%x )\n",
                    usec,ALTG_getMode(altH),startMode,sensorInfoP->pendingExposure[0],
                    sensorInfoP->pendingExposure[1],exp[0],exp[1],exp[2]);
#endif
   }
   else
   {
      ret = INU_DEVICE__ERR_ILLEGAL_STATE;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: ALTG_start
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/ 
ERRG_codeE ALTG_start( inu_altH *altH, UINT32 numFramesPattern, UINT32 numFramesFlood )
{
   ERRG_codeE              ret = INU_DEVICE__ERR_OUT_OF_RSRCS;
   UINT32                  i;
   ALTERNATEP_infoT        *paramsP;

   for (i = 0; i < ALTERNATEP_MAX_NUM_INSTANCES; i++)
   {
      if (!ALTP_params[i].active)
      {
         paramsP = &ALTP_params[i];
         paramsP->numTicksInMode[0] = numFramesPattern;
         paramsP->numTicksInMode[1] = numFramesFlood;
         paramsP->numModes = 2;
         paramsP->currentMode = 0; //start with first mode
         paramsP->ticksLeftInMode = paramsP->numTicksInMode[paramsP->currentMode];
         paramsP->active = 1;
         
         *altH = paramsP;
         return INU_DEVICE__RET_SUCCESS;
      }
   }
   //bind alternate handle to the channel
   //for now - only on IR
   
#if 0
   //create thread for handling the interrupts
   ret = CHARGERP_createThread(&CHARGERP_params);

   if(ERRG_SUCCEEDED(ret))
   {
      CHARGERP_params.initialized = TRUE;
      if(ERRG_SUCCEEDED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"CHARGER init success. Battery level = %d\n",batteryLevel);
      }
   }
#endif
   return ret;
}



/****************************************************************************
*
*  Function Name: ALTG_stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE ALTG_stop( inu_altH altH )
{
   ERRG_codeE              ret = INU_DEVICE__RET_SUCCESS;
   ALTERNATEP_infoT        *paramsP = (ALTERNATEP_infoT*)altH;
   paramsP->active = 0;
   return ret;
}



/****************************************************************************
*
*  Function Name: ALTG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
ERRG_codeE ALTG_init( )
{
   ERRG_codeE              ret = INU_DEVICE__RET_SUCCESS;
   memset(ALTP_params,0,sizeof(ALTP_params));
   return ret;
}

/****************************************************************************
*
*  Function Name: ALTG_isActive
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*    Success or specific error code.
*
*  Context:
*
****************************************************************************/
BOOL ALTG_isActive( inu_altH altH )
{
   ALTERNATEP_infoT        *paramsP;
   BOOL                     isActive = FALSE;

   if (altH)
   {
      paramsP = (ALTERNATEP_infoT*)altH;
      isActive = paramsP->active;
   }

   return isActive;
}


