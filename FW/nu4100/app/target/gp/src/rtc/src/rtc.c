/*
 * Copyright 2022 IMDT
 * All rights reserved.
 *
 */
#include "rtc.h"
#include "inu_common.h"
#include "nu4100_pss_regs.h"
#include "io_pal.h"
#include "gme_drv.h"
#include "gpio_drv.h"
#include <unistd.h>
#include "inu_common.h"
#include "os_lyr.h"
//#define DEBUG_RTC_PHASE_ERROR  # Uncomment to debug the RTC phase error
#define RTC_TRACKING_ENABLED
#define ENABLE_MASK              0x01
#define LOCK_AQUIRED             0x01
#define FNATIVE_CLIPPING_MAX     24010
#define FNATIVE_CLIPPING_MIN     23990
#define NS_IN_SECOND             (1000000000UL)
#define MAX_RTC_FRAC_SECONDS  (0xFFFFFFFFUL)
static UINT64 PPS_IN_TS = 0;
static UINT64 PPS_OUT_TS = 0;


/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/

typedef struct
{
   UINT32                     deviceBaseAddress;
} PSS_DRVP_deviceDescT;

typedef struct RTC_Configuration
{
    UINT8 rtcAlignmentMode;
    UINT8 rtcTrackingMode;
    UINT8 lockThreshold; // Number of PPS sequences below low threshold to achieve lock
    UINT8 unlockThreshold;
    UINT32 ppsAbsenceTime; // in milliseconds
    UINT32  pavgHighThreshold; // Average phase error high threashold
    UINT32  pavgLowThreshold; // Average phase error low threashold
} RTC_DRVP_configT;
/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static PSS_DRVP_deviceDescT   PSS_DRVP_deviceDesc;
static RTC_DRVP_configT rtcConfig;
static bool timeSyncStarted = false;

/****************************************************************************
*
*  Function Name: RTC_DRVG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC control
*
****************************************************************************/
ERRG_codeE RTC_DRVG_init(UINT32 memVirtAddr)
{
   PSS_DRVP_deviceDesc.deviceBaseAddress = memVirtAddr;

    // Default configuration.
    // Immediate RTC alignment mode.
    rtcConfig.rtcAlignmentMode = 0;

    // Tracking Mode, Needs to be synchronised with Master and host clock
    rtcConfig.rtcTrackingMode = 0;

    // 0 is 16 sequencial cycles of pps before lock
    rtcConfig.lockThreshold = 5;

    // 0 is 16 sequencial cycles of pps before lock
    rtcConfig.unlockThreshold = 5;

    // 5 seconds PPS absence time
    rtcConfig.ppsAbsenceTime = 5000;

    // 10 bits for close to millisecond accuracy.
    rtcConfig.pavgHighThreshold = 10;
    rtcConfig.pavgLowThreshold = 10;

   return PSS__RET_SUCCESS;
}


static void RTC_DRVP_clearInterrupts()
{
   PSS_ELU_INT_CLEAR_FIFO_OVERFLOW_W(1);
   PSS_ELU_INT_CLEAR_ONEPPS_IN_W(1);
   PSS_ELU_INT_CLEAR_ONEPPS_OUT_W(1);
   PSS_ELU_INT_CLEAR_LOCK_W(1);
   PSS_ELU_INT_CLEAR_UNLOCK_W(1);
}


/****************************************************************************
*
*  Function Name: RTC_DRVG_trackingConfigure
*
*  Description: Configure the Tracking registers
*
*  Inputs: Tracking regsiters values
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC control
*
****************************************************************************/
static ERRG_codeE RTC_DRVP_trackingConfigure(RTC_DRVP_configT *rtcConfigPtr,const RTC_DRVG_TrackingModeE trackingMode )
{
    // use the configuration if provided, otherwise defaut values.
    if(rtcConfigPtr){
        rtcConfig.rtcAlignmentMode = rtcConfigPtr->rtcAlignmentMode;
        rtcConfig.rtcTrackingMode = rtcConfigPtr->rtcTrackingMode;
        rtcConfig.lockThreshold = rtcConfigPtr->lockThreshold;
        rtcConfig.unlockThreshold = rtcConfigPtr->unlockThreshold;
        rtcConfig.ppsAbsenceTime = rtcConfigPtr->ppsAbsenceTime;
        rtcConfig.pavgHighThreshold = rtcConfigPtr->pavgHighThreshold;
        rtcConfig.pavgLowThreshold = rtcConfigPtr->pavgLowThreshold;
    }
   /*
   Register values from Evgeni's excel simulation
   register name, value, using spreadsheet value
   coarse_time_immediate, 1, yes
   coarse_time_pending, 0, yes
   rtc_track_mode, 1, yes
   rtc_coarse_time_value, 0x0, yes
   rtc_fnative_max, 0x5dca, yes
   rtc_fnative_min, 0x5db6, yes
   rtc_pavg_max,0x4000, yes
   rtc_pavg_min, 0xc000, yes
   pps_absense_time, 0x1388, yes
   rtc_alpha, 0x73, yes
   rtc_beta, 0x73, yes
   rtc_gama, 0x0d, yes
   rtc_period, 0x5dbf, yes
   rtc_fnative, 0x5dc0, yes
   rtc_f2n_factor, 0x22551000, yes
   */
   #define NATIVE_FREQUENCY 24000
    UINT16 fNativeMax = 0x5dca;
    UINT16 fNativeMin = 0x5db6;
     PSS_ELU_RTC_CONTROL_RTC_TRACK_MODE_W(0x1);
    if(trackingMode == RTC_DRVG_TrackingMode_ImmediateMode)
    {
      PSS_ELU_RTC_CONTROL_RTC_ALIGNMENT_MODE_W(0x0); /*Immediate - This means the coarse time will be updated instantly */
    }
    else
    {
      PSS_ELU_RTC_CONTROL_RTC_ALIGNMENT_MODE_W(0x1); /*Pending mode - This means the coarse time will be updated on the next PPS*/
    }
    // F_Native Min clipping value
    PSS_ELU_RTC_FNATIVE_MIN_CLIPPING_W(fNativeMin);
    PSS_ELU_RTC_FNATIVE_MAX_CLIPPING_W(fNativeMax);
    PSS_ELU_RTC_PAVG_MAX_CLIPPING_W(0x4000);
    PSS_ELU_RTC_PAVG_MIN_CLIPPING_W(0xc000);
    PSS_ELU_RTC_PPS_ABSENSE_TIME_ABSENSE_TIME_W(5000);

   PSS_ELU_RTC_COEFFICIENT_ALPHA_W(0x73);
   PSS_ELU_RTC_COEFFICIENT_BETA_W(0x73);
   PSS_ELU_RTC_COEFFICIENT_GAMA_W(0x0d);

   PSS_ELU_RTC_INIT_PARAMS_RTC_PERIOD_W(0x5dbf);
   PSS_ELU_RTC_INIT_PARAMS_FNATIVE_W(0x5dc0);
   PSS_ELU_RTC_F2N_FACTOR_VAL_W(0x22551000);

    // Absence time for PPS

    PSS_ELU_RTC_CONTROL_LOCK_THR_W(rtcConfig.lockThreshold);
    PSS_ELU_RTC_CONTROL_UNLOCK_THR_W(rtcConfig.unlockThreshold);

   PSS_ELU_RTC_PAVG_HIGH_THR_VAL_W(0x4000);
   PSS_ELU_RTC_PAVG_LOW_THR_VAL_W(0xc000);
   /*Clear the Interrupt registers so we know if we have achieved a lock or not*/
   RTC_DRVP_clearInterrupts();
    // RTC init params, f2nfactor are left to be reset values.
    // pavg_low_thr & pavg_high_thr TBD
   
   /*We must disable the PPS host pin until the RTC registers have been written otherwise the RTC will glitch and misbehave 
    I have done this by setting the PPS host pin to be in GPIO muxing until the RTC has been configured
   */
  LOGG_PRINT(LOG_INFO_E,NULL,"Written RTC registers, Alignment Mode = %lu  \n",(unsigned int)RTC_DRVG_TrackingMode_ImmediateMode );
   GME_SetPPSHostPinMuxing(0);

   return PSS__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: RTC_DRVG_status
*
*  Description: Get RTC Tracking Lock status
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC control
*
****************************************************************************/
void RTC_DRVG_status(inu_device__get_rtc_lock_t *rtcStatusPtr)
{
   if(PSS_ELU_CONTROL_ENABLE_R != ENABLE_MASK){

      // RTC is not active
      rtcStatusPtr->lockStatus = INU_DEVICE__RTC_INACTIVE_E;
   }
   else if(PSS_ELU_INT_LOCK_R == LOCK_AQUIRED){

      // RTC Tracking is locked with host clock
      rtcStatusPtr->lockStatus = INU_DEVICE__RTC_LOCK_DONE_E;
   }
   else{

      // RTC Tracking is not yet locked with host clock
      rtcStatusPtr->lockStatus = INU_DEVICE__RTC_LOCK_IN_PROGRESS_E;
   }
}

/****************************************************************************
*
*  Function Name: RTC_DRVG_coarseTimeSet
*
*  Description: Set the coarse time value
*
*  Inputs: 32 bit coarse time value
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC Control
*
****************************************************************************/
ERRG_codeE RTC_DRVG_coarseTimeSet(UINT32 val, bool toggle_ts_ld)
{
   /*TS Load needs to be toggled when loading the coarse time value*/
   if(toggle_ts_ld)
   {
      PSS_ELU_COMMAND_TS_LD_CMD_W(0);
   }
   PSS_ELU_RTC_COARSE_TIME_VALUE_COARSE_TIME_VALUE_W(val);
   if(toggle_ts_ld)
   {
      PSS_ELU_COMMAND_TS_LD_CMD_W(1);
   }
   LOGG_PRINT(LOG_DEBUG_E,NULL,"Set time to be %lu \n", val);
   return PSS__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: RTC_DRVG_coarseTimeGet
*
*  Description: Get the coarse time value
*
*  Inputs: 
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC Control
*
****************************************************************************/
UINT32 RTC_DRVG_coarseTimeGet(void)
{
   return PSS_ELU_RTC_COARSE_TIME_VALUE_COARSE_TIME_VALUE_R;
}


/****************************************************************************
*
*  Function Name: RTC_DRVG_tsHighValGet
*
*  Description: Get the current Timestamp high value. (Seconds)
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC Control
*
****************************************************************************/
static UINT32 RTC_DRVP_tsHighValGet()
{
   return PSS_ELU_RTC_TS_HIGH_RTC_TS_HIGH_VAL_R;
}


/****************************************************************************
*
*  Function Name: RTC_DRVG_tsLowValGet
*
*  Description: Get the current Timestamp low value. (Fraction seconds)
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC Control
*
****************************************************************************/
static UINT32 RTC_DRVP_tsLowValGet()
{
   return PSS_ELU_RTC_TS_LOW_RTC_TS_LOW_VAL_R;
}
#ifdef DEBUG_RTC_PHASE_ERROR
#include<sys/time.h>
long long timeInMilliseconds(void) {
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

void RTC_Reg_Print_Phase_Error_Stats()
{  
   /*Phase average is a 25 bit signed value from -0.5 to 0.5*/
   UINT32 pavg  = PSS_ELU_RTC_PAVG_STS_VAL_R;
   UINT32 pps = PSS_ELU_INT_LOCK_R;
   INT32 pavg_s = 0;
   if(pavg & ( 1<< 24))
   {
   /* Sign extend the phase average, this can be printed directly if needed*/
   int m = 1U << (25 - 1);  
      pavg_s = (pavg ^ m) - m; /*Sign extended phase average*/
   }
   else
   {
      pavg_s = (INT32) pavg;
   }
   double average_phase_error = 0.5* ((double)pavg_s / ( 1<< 23) * 1000.0);

   UINT64 time_ms = timeInMilliseconds();
   printf("Time, %llu, AVG_PHASE_ERROR_HEX, 0x%x, AVG_PHASE_ERROR_INT32, %d,  AVG_PHASE_ERROR_milliseconds %.6f \n", time_ms,pavg, pavg_s, average_phase_error);
}
#endif
void RTC_REG_dump()
{
   #ifdef DEBUG_PPS_HOST_PIN
   GME_SetPPSHostPinMuxing(1);
   GPIO_DRVG_gpioSetDirParamsT directionParams;
   directionParams.direction = GPIO_DRVG_IN_DIRECTION_E;
   directionParams.gpioNum = GPIO_DRVG_GPIO_56_E;
   IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &directionParams);
   LOGG_PRINT(LOG_INFO_E, NULL, "GPIO: Set GPIO %d direction to %d, muxing %d\n", directionParams.gpioNum, directionParams.direction, GME_GetPPSHOSTmux());

   GPIO_DRVG_gpioGetValParamsT getParams_GIO56;
   getParams_GIO56.gpioNum = GPIO_DRVG_GPIO_56_E; 
   ERRG_codeE ret_1 = IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_GET_GPIO_VAL_CMD_E, &getParams_GIO56);
   LOGG_PRINT(LOG_INFO_E, NULL, "PPS_HOST GPIO: Read GPIO HW pin %d as %d\n", getParams_GIO56.gpioNum,getParams_GIO56.val);
   GME_SetPPSHostPinMuxing(0);
   printf("Start RTC register dump, switched back to RTC Pin muxing %d\n", GME_GetPPSHOSTmux());
   #endif

   for(unsigned int i= 0x0; i <0x2E0; i = i +4 )
   {
      volatile uint32_t * address =  ((volatile UINT32 * ) (PSS_BASE + i));
      printf("%p ,0x%x \n" ,i +0x80D0000 , *address); /*Print with spaces so that it's easy to grep register values*/

   }
   #ifdef DEBUG_RTC_PHASE_ERROR
   RTC_Reg_Print_Phase_Error_Stats();
   #endif
   RTC_DRVP_clearInterrupts();

   printf("Cleared ELU interrupts \n");

}


/****************************************************************************
*
*  Function Name: RTC_DRVG_enable
*
*  Description: Enable RTC ticking
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC Control
*
****************************************************************************/
static ERRG_codeE RTC_DRVP_enable()
{
   PSS_ELU_CONTROL_ENABLE_W(ENABLE_MASK);
   return PSS__RET_SUCCESS;
}
static UINT32 coarseTime = 0;

ERRG_codeE RTC_DRVG_setTime(UINT32 seconds, UINT32 microseconds,const RTC_DRVG_TrackingModeE trackingMode )
{
   // Check if RTC is enabled
   if(PSS_ELU_CONTROL_ENABLE_R != ENABLE_MASK)
      RTC_DRVP_enable();
   
#ifdef RTC_TRACKING_ENABLED
   if(!timeSyncStarted){
      // Init the RTC tracking feature
      RTC_DRVP_trackingConfigure(&rtcConfig, trackingMode);
      timeSyncStarted = true;
   }
#endif
   coarseTime = seconds;
      // Set the RTC coarse time.
   RTC_DRVG_coarseTimeSet(seconds,true);

   //RTC_REG_dump();

   return PSS__RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: RTC_DRVG_getTime
*
*  Description: Retrieve the RTC timestamp regsiter values in
*               struct timespec object.
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC Control
*
****************************************************************************/
ERRG_codeE RTC_DRVG_getTime(RTC_DRVG_RtcTimeT *timeParam)
{ 
   timeParam->seconds = RTC_DRVP_tsHighValGet();
   timeParam->fracSeconds = RTC_DRVP_tsLowValGet();
   return PSS__RET_SUCCESS;
}
ERRG_codeE RTC_DRVG_convertRTCTimeToNs(UINT64 *result, RTC_DRVG_RtcTimeT * time)
{
   if (result == NULL || time == NULL )
   {
      return INU_IMU_DATA__ERR_INVALID_ARGS;
   }
   UINT64 seconds = time->seconds;
   UINT64 fracSeconds = time->fracSeconds;
   double nanosecond_part = (double)NS_IN_SECOND* (fracSeconds / MAX_RTC_FRAC_SECONDS);            /* Operate in double f to make calculation easier*/
   UINT32 nanosecond_part_u = (UINT32) nanosecond_part;                             /*Convert back to UINT32*/
   *result = (UINT64)( seconds * NS_IN_SECOND) + nanosecond_part_u;
   return PSS__RET_SUCCESS;
}
ERRG_codeE RTC_DRVG_getTimeNs(UINT64 *result)
{
   if (result == NULL)
   {
      return INU_IMU_DATA__ERR_INVALID_ARGS;
   }
   RTC_DRVG_RtcTimeT rtcTime = {0,0};
   RTC_DRVG_getTime(&rtcTime); 
   return RTC_DRVG_convertRTCTimeToNs(result,&rtcTime);

}
ERRG_codeE RTC_DRVG_getTimeRaw(UINT64 *result)
{
   if (result == NULL)
   {
      return INU_IMU_DATA__ERR_INVALID_ARGS;
   }
   RTC_DRVG_RtcTimeT rtcTime = {0,0};
   RTC_DRVG_getTime(&rtcTime); 
   UINT64 rawTime   = ((UINT64)rtcTime.seconds) << 32;
   rawTime |= rtcTime.fracSeconds;
   *result = rawTime;
   return PSS__RET_SUCCESS;

}
ERRG_codeE RTC_DRVG_convertNsTimeToRTCTime(UINT64 nsTime, RTC_DRVG_RtcTimeT *rtcTime)
{
   if (rtcTime == NULL)
   {
      return INU_IMU_DATA__ERR_INVALID_ARGS;
   }
   UINT32 seconds = nsTime / NS_IN_SECOND;
   UINT32 ns_remainder = nsTime % NS_IN_SECOND;
   double fracSeconds = MAX_RTC_FRAC_SECONDS * ((double)ns_remainder / NS_IN_SECOND);

   rtcTime->seconds = seconds;
   rtcTime->fracSeconds = (UINT32)fracSeconds;
   return PSS__RET_SUCCESS;
}
/****************************************************************************
*
*  Function Name: PSS_DRVG_dumpRegs
*
*  Description:
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: RTC control
*
****************************************************************************/
ERRG_codeE RTC_DRVG_dumpRegs()
{
   UINT32 regsOffset[] = {pss_offset_tbl_values};
   UINT32 regsResetVal[] = {pss_regs_reset_val};
   UINT32 reg;

   LOGG_PRINT(LOG_INFO_E, NULL, "Modified RTC registers:\n");

   //Compare RTC registers against reset val
   for (reg = 0; reg < (sizeof(regsOffset)/4); reg++)
   {
      if (*(volatile UINT32 *)(PSS_BASE + regsOffset[reg]) != regsResetVal[reg])
      {
         LOGG_PRINT(LOG_INFO_E, NULL, "0x%04x = 0x%08x\n",regsOffset[reg],*(volatile UINT32 *)(PSS_BASE + regsOffset[reg]));
      }
   }

   return PSS__RET_SUCCESS;
}
/**
 * @brief Converts RTC time into nanoseconds
 * @param metadata_time RTC based time reading
 * @return Returns time in nanoseconds
 */
 UINT64 RTC_convertRTCTimeToNS(UINT64 rtc_time)
{
	UINT64 second = (rtc_time >> 32);
	UINT64 frac_part = (rtc_time & 0xFFFFFFFF);
	double nanoseconds = (double)1E9* (double)frac_part/((double)0xFFFFFFFF);	/* Scale frac_part to be from 0->1*/
	return 1000000000*second + (UINT64) nanoseconds;
}
