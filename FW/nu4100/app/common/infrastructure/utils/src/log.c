/****************************************************************************
 *
 *   FileName: log.c
 *
 *   Author: Benny V.
 *
 *   Date:
 *
 *   Description: INU log prints
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include <stdio.h>
#include "inu_common.h"
#include "cdc_mngr.h"
#if DEFSG_IS_GP || DEFSG_IS_HOST
#include "inu_ref.h"
#include "inu_str.h"
#endif

#if DEFSG_IS_GP
#include "cnn_boot.h"
#endif

#if DEFSG_IS_HOST
#include "inu2.h"
#endif

#if DEFSG_IS_HOST_LINUX
#include "syslog.h"
#include <stdarg.h>
#endif

#if DEFSG_IS_TARGET
#include "mem_map.h"
#endif


#if DEFSG_IS_CEVA || DEFSG_IS_EV72

 #if DEFSG_IS_CEVA
 #define LOG_ADDRESS (MEM_MAPG_CEVA_LOG_ADDRESS)
 #define UNCACHED_QUALIFIER
 #else
 #include <stdarg.h>
 #define LOG_ADDRESS (MEM_MAPG_EV61_LOG_ADDRESS)
 #define UNCACHED_QUALIFIER _Uncached
#include "evthreads.h"
 #endif

#ifdef __cplusplus
      extern "C" {
 #endif

 /****************************************************************************
 *
 *  Function Name: LOGG_outputStrLog
 *
 *  Description:
 *
 *  Inputs:
 *
 *  Outputs:
 *
 *  Returns:
 *
 *  Context: logger
 *
 ****************************************************************************/
void LOGG_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fmt, ...)
{
   char *strP;
   LOGG_cevaLogbufferT *cevaLogbufferP;
   volatile UINT32 *dbgLevelP;
   long flags;
   static int lostLogCounter = 0;
   va_list args;

   FIX_UNUSED_PARAM_WARN(errCode);

   if(level < LOG_DEBUG_E)//do not print DEBUG level
   {
      va_start(args,fmt);

      cevaLogbufferP = (LOGG_cevaLogbufferT*)LOG_ADDRESS;

      OS_LYRG_lockMutexCp(cevaLogbufferP->cpMutex);
      flags = OS_LYRG_disableInterrupts();
      if((cevaLogbufferP->wrBufferIndex != cevaLogbufferP->rdBufferIndex) || ((cevaLogbufferP->state == LOGG_CEVA_LOG_STATE_EMPTY) && (cevaLogbufferP->wrBufferIndex == cevaLogbufferP->rdBufferIndex)))
      {
         lostLogCounter = 0;
         cevaLogbufferP->state = LOGG_CEVA_LOG_STATE_EMPTY;
         strP = &(cevaLogbufferP->logBuffer[cevaLogbufferP->wrBufferIndex][0]);
         dbgLevelP = (UINT32*)strP;
         #if DEFSG_IS_CEVA
         *dbgLevelP = CEVA_ID;
         #else
         *dbgLevelP = (evGetCpu()&0xf);
         #endif
         *dbgLevelP++ |= (level << 4);

         vsprintf((char*)dbgLevelP, fmt, args);
         cevaLogbufferP->wrBufferIndex++;
         cevaLogbufferP->wrBufferIndex &= (LOGP_NUM_OF_CEVA_LOG_BUFS-1);
         cevaLogbufferP->state = (cevaLogbufferP->wrBufferIndex == cevaLogbufferP->rdBufferIndex) ? LOGG_CEVA_LOG_STATE_FULL : LOGG_CEVA_LOG_STATE_EMPTY;
      }
      else
      {
         UINT32 lastLine;

         lostLogCounter++;

         cevaLogbufferP->state = LOGG_CEVA_LOG_STATE_EMPTY;

         if (cevaLogbufferP->wrBufferIndex == 0)
         {
            lastLine = (LOGP_NUM_OF_CEVA_LOG_BUFS-1);
         }
         else
         {
            lastLine = (cevaLogbufferP->wrBufferIndex-1);
         }

         strP = &(cevaLogbufferP->logBuffer[lastLine][0]);
         dbgLevelP = (UINT32*)strP;
         #if DEFSG_IS_CEVA
         *dbgLevelP = CEVA_ID;
         #else
         *dbgLevelP = (evGetCpu()&0xf);
         #endif
         *dbgLevelP++ |= (level << 4);

         sprintf((char*)dbgLevelP, "LOG BUF IS FULL! lost count = %d\n", lostLogCounter);
         cevaLogbufferP->state = LOGG_CEVA_LOG_STATE_FULL;
      }
      OS_LYRG_enableInterrupts(flags);
      OS_LYRG_unlockMutexCp(cevaLogbufferP->cpMutex);

      va_end(args);
   }


}

#ifdef __cplusplus
}
#endif

#else

#include "mem_pool.h"
#include "internal_cmd.h"

#include <stdarg.h>
#include <string.h>


#if DEFSG_IS_GP
#include "mem_map.h"
#endif


#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define LOGP_LEVEL_STRING_LEN          (8)
#define LOGP_TIME_STAMP_STRING_LEN     (15)
#define LOGP_FILE_STRING_LEN           (25)
#define LOGP_LINE_STRING_LEN           (10)
#define LOGP_PID_STRING_LEN            (10)
#define LOGP_FUNC_STRING_LEN           (40)
#define LOGP_FMT_STRING_LEN            (300)
#define LOGP_ERR_STRING_LEN            (150)
#define LOGP_MAIN_STRING_LEN           ( (LOGP_TIME_STAMP_STRING_LEN) + (LOGP_LEVEL_STRING_LEN) + (LOGP_PID_STRING_LEN) + (LOGP_FILE_STRING_LEN) + (LOGP_FUNC_STRING_LEN) + (LOGP_FMT_STRING_LEN))


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef void (*LOGP_outputStrLogT)(const char *log);


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/


#define LOGP_FILE_NAME                 "/tmp/nu3000.log"
static void LOGP_getProcessorString(INU_DEFSG_pidE pid, char *stringP);
static void LOGP_getLevelString(INU_DEFSG_logLevelE level, char *string,size_t size);
static void LOGP_insertSpaces(char *string, UINT32 size);
static void LOGP_outputStrLogPrintf(const char *log);
#if DEFSG_IS_GP
   static void LOGP_outputWriteToFile(const char *log);
   static void LOGP_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, INU_DEFSG_pidE pid, char * fmt);
   static void LOGP_outputCevaStrLog(LOGG_cevaLogbufferT *cevaLogbufferP, const char* procId);
#endif
static void LOGP_outputStrLogMethods(char *logStr);

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

static INU_DEFSG_logLevelE LOGP_dbgLevel           = LOG_INFO_E;
static BOOLEAN             LOGG_isLoggerOn         = TRUE;
static BOOLEAN             LOGG_printLogFlag       = TRUE;
static BOOLEAN             LOGG_sendLogFlag        = TRUE;
static BOOLEAN             LOGG_sendLogCdcFlag     = FALSE;
static void                *LOGP_loggerH           = NULL;
static OS_LYRG_msgQueT     *LOGP_msgFifoH;
#if DEFSG_IS_GP
static FILE                *LOGP_logFileP          = NULL;
static BOOLEAN             LOGG_printLogFileFlag   = TRUE;
#endif
static OS_LYRG_mutexT      LOGP_logMutex;
static BOOLEAN             LOGP_thrExit            = TRUE;

//char levelStr[LOGP_LEVEL_STRING_LEN];
//char fileStr [LOGP_FILE_STRING_LEN];
//char lineStr [LOGP_LINE_STRING_LEN];
//char pidStr  [LOGP_PID_STRING_LEN];
//char funcStr [LOGP_FUNC_STRING_LEN];
//char errStr  [LOGP_ERR_STRING_LEN];
//char mainStr [LOGP_MAIN_STRING_LEN];
//char timeStr [LOGP_TIME_STAMP_STRING_LEN];


#if DEFSG_IS_HOST
      LOGP_outputStrLogT  LOGP_outputStrLogTbl[] = {LOGP_outputStrLogPrintf,  LOGG_sendLog};
#elif DEFSG_IS_GP
      BOOLEAN  LOGG_loggerConnected    = FALSE;
      LOGP_outputStrLogT  LOGP_outputStrLogTbl[] = {LOGP_outputStrLogPrintf,  LOGG_sendLog, LOGP_outputWriteToFile, LOGG_sendLogCdc};
#endif


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/


/****************************************************************************
*
*  Function Name: LOGP_outputStrLogPrintf
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
#if DEFSG_IS_HOST_ANDROID
   #include <android/log.h>
#endif
static void LOGP_outputStrLogPrintf(const char *log)
{
   if(LOGG_printLogFlag == TRUE)
   {
      #if DEFSG_IS_HOST_ANDROID
         __android_log_print(ANDROID_LOG_INFO, "InuDev", log);
      #else
         printf("%s",log);
#if DEFSG_IS_HOST_WINDOWS
         OutputDebugString(log);
#elif DEFSG_IS_HOST_LINUX
         syslog(LOG_INFO, "%s", log);
#endif

      #endif
   }
}
/**
 * @brief Clears all of our temporary print buffers
 * 
 */
static void LOGP_clearBuffers()
{
   //memset(fileStr,0,sizeof(fileStr));
   //memset(lineStr,0,sizeof(lineStr));
   //memset(levelStr,0,sizeof(levelStr));
   //memset(fileStr,0,sizeof(fileStr));
   //memset(pidStr,0,sizeof(pidStr));
   //memset(funcStr,0,sizeof(funcStr));
   //memset(mainStr,0,sizeof(mainStr));
   //memset(timeStr,0,sizeof(timeStr));
}
/****************************************************************************
*
*  Function Name: LOGP_outputStrLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static void LOGP_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, INU_DEFSG_pidE pid, char *fmtStr)
{
   if( (level <= LOGP_dbgLevel) && (LOGG_isLoggerOn == TRUE) )
   {
      UINT32   sec;
      UINT16   msec;
      char levelStr[LOGP_LEVEL_STRING_LEN] = { 0 };
      char fileStr[LOGP_FILE_STRING_LEN] = { 0 };
      char lineStr[LOGP_LINE_STRING_LEN] = { 0 };
      char pidStr[LOGP_PID_STRING_LEN] = { 0 };
      char funcStr[LOGP_FUNC_STRING_LEN] = { 0 };
      char errStr[LOGP_ERR_STRING_LEN] = { 0 };
      char mainStr[LOGP_MAIN_STRING_LEN] = { 0 };
      char timeStr[LOGP_TIME_STAMP_STRING_LEN] = { 0 };

      // unnecessary to keep synchronization here. removed to improve the reponse speed once deleting logger due to target crashes
      // and move such the synchronization to LOGG_sendLog
      // by david @ 2023.11.01
      //OS_LYRG_lockMutex(&LOGP_logMutex);
      OS_LYRG_getTime(&sec, &msec); 
      /*Clear all of the print buffers*/
      //LOGP_clearBuffers();
      // create  seperated strings
      // processor ID string
      LOGP_getProcessorString(pid, pidStr);
      LOGP_insertSpaces(pidStr, LOGP_PID_STRING_LEN);
      // level string
      LOGP_getLevelString(level,levelStr,sizeof(levelStr));
      LOGP_insertSpaces(levelStr, LOGP_LEVEL_STRING_LEN);
      // Time String
      snprintf (timeStr,sizeof(timeStr), "%u.%03u sec ", (UINT16)sec, msec);
      LOGP_insertSpaces(timeStr, LOGP_TIME_STAMP_STRING_LEN);
      // file string
      snprintf (fileStr,LOGP_FILE_STRING_LEN- LOGP_LINE_STRING_LEN - 1, "%s ", fileP);
      fileStr[LOGP_FILE_STRING_LEN - LOGP_LINE_STRING_LEN - 1] = 0;
      snprintf (lineStr,LOGP_LINE_STRING_LEN - 1, "(%d),", lineNum);
      strncat(fileStr, lineStr, LOGP_LINE_STRING_LEN);
      LOGP_insertSpaces(fileStr, LOGP_FILE_STRING_LEN);
      // func string
      snprintf(funcStr, LOGP_FUNC_STRING_LEN - 1, "%s(), ", funcP);
      snprintf (funcStr,sizeof(funcStr), "%s(), ", funcP);
      LOGP_insertSpaces(funcStr, LOGP_FUNC_STRING_LEN);
      // concatenate all strings together
      /*Example output:
      FW 4100 LOG (GP):INFO,   9.727 sec      model.c(59)              MODELG_readModel(),                     detected model "10065" */
      snprintf (mainStr,sizeof(mainStr),"FW 4100 LOG (%s):%s%s%s%s%s",pidStr,levelStr,timeStr,fileStr,funcStr,fmtStr);
      if (strlen(fileP) == 0)
         LOGP_outputStrLogMethods(fmtStr);
      else
         // print final string to log
         LOGP_outputStrLogMethods(mainStr);

      if(level == LOG_ERROR_E)
      {
         if(errCode != (ERRG_codeE)NULL)
         {
            INU_STRG_getErrString(errCode, errStr);
            strcat (errStr, "\n");
            LOGP_outputStrLogMethods(errStr);
         }
      }
   }
}

/****************************************************************************
*
*  Function Name: LOGG_outputStrLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_outputStrLog(INU_DEFSG_pidE pid, INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...)
{
   char fmtStr  [LOGP_FMT_STRING_LEN];
   if( (level <= LOGP_dbgLevel) && (LOGG_isLoggerOn == TRUE) )
   {
      VA_PACK(fmt, fmtStr);
      LOGP_outputStrLog(level, errCode, fileP, funcP, lineNum, pid, fmtStr);
   }

}

/****************************************************************************
*
*  Function Name: LOGP_outputStrLogMethods
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGP_outputStrLogMethods(char *logStr)
{
   UINT8 i=0;

   for (i=0; i<sizeof(LOGP_outputStrLogTbl)/sizeof(LOGP_outputStrLogT); ++i)
   {
      LOGP_outputStrLogTbl[i](logStr);
   }

}

/****************************************************************************
*
*  Function Name: LOGP_getLevelString
*
*  Description: convert log level ienum to string
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static void LOGP_getLevelString(INU_DEFSG_logLevelE level, char *string,size_t size)
{
   switch(level)
   {
      case LOG_ERROR_E:
         strncpy(string, "ERROR, ",size);
         break;
      case LOG_WARN_E:
         strncpy(string, "WARN,  ",size);
         break;
      case LOG_INFO_E:
         strncpy(string, "INFO,  ",size);
         break;
      case LOG_DEBUG_E:
         strncpy(string, "DEBUG, ",size);
         break;
      default:
         strncpy(string, "       ",size);
         break;
   }
}

/****************************************************************************
*
*  Function Name: ERRP_getProcessorString
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static void LOGP_getProcessorString(INU_DEFSG_pidE pid, char *stringP)
{
   switch(pid)
   {
      case HOST_ID:
         strcpy(stringP, "(HOST):");
         break;
      case GP_ID:
         strcpy(stringP, "(GP):");
         break;
      case CEVA_ID:
         strcpy(stringP, "(CEVA):");
         break;
      case EV61_ID:
         strcpy(stringP, "(CNN):");
         break;
      case IAF_ID:
         strcpy(stringP, "(IAF):");
         break;
      case CLIENT_GP_ID:
         strcpy(stringP, "(FDK_GP):");
         break;
      case CLIENT_CEVA_ID:
         strcpy(stringP, "(FDK_CE):");
         break;
      default:
         strcpy(stringP, "(NA):");
         break;
   }
}

/****************************************************************************
*
*  Function Name: LOGP_insertSpaces
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static void LOGP_insertSpaces(char *string, UINT32 size)
{
   UINT32 len = (UINT32)strnlen(string,size);
   const int numberSpacesNeeded = size - len - 1; /*Last character must be 0 so make the length size-1*/
   if(numberSpacesNeeded > 0)
   {
      for(int i = 0; i < numberSpacesNeeded; i++) /* Add numberSpacesNeeded of spaces*/
      {
         strcat(string, " ");
      }
   }
}

/****************************************************************************
*
*  Function Name: LOGG_start
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void  LOGG_start(void *loggerH, OS_LYRG_msgQueT *msgFifoH)
{
   OS_LYRG_lockMutex(&LOGP_logMutex);

   LOGP_loggerH = loggerH;

   if(LOGP_loggerH)
   {
      LOGP_msgFifoH     = msgFifoH;
   }

#if (DEFSG_IS_GP)
   LOGG_printLogUpdateFlags();
#endif

   OS_LYRG_unlockMutex(&LOGP_logMutex);
}

/****************************************************************************
*
*  Function Name: LOGG_stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_stop()
{
   OS_LYRG_lockMutex(&LOGP_logMutex);
   LOGP_loggerH = NULL;
   OS_LYRG_unlockMutex(&LOGP_logMutex);
}

/****************************************************************************
*
*  Function Name: LOGG_setDbgLevel
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_getDbgLevel(INU_DEFSG_logLevelE *levelP)
{
   *levelP = LOGP_dbgLevel;
}

/****************************************************************************
*
*  Function Name: LOGG_setDbgLevel
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_setDbgLevel(INU_DEFSG_logLevelE level)
{
   LOGP_dbgLevel = level;
}

/****************************************************************************
*
*  Function Name: LOGG_setSendLogFlag
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_setSendLogFlag(BOOLEAN sendLogFlag)
{
   LOGG_sendLogFlag = sendLogFlag;
}

/****************************************************************************
*
*  Function Name: LOGG_setSendLogCdcFlag
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_setSendLogCdcFlag(BOOLEAN sendLogCdcFlag)
{
   LOGG_sendLogCdcFlag = sendLogCdcFlag;
}

/****************************************************************************
*
*  Function Name: LOGG_setLoggerMainFlag
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_setLoggerMainFlag(BOOLEAN loggerMainFlag)
{
   LOGG_isLoggerOn = loggerMainFlag;
}

/****************************************************************************
*
*  Function Name: LOGG_setPrintLogFlag
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_setPrintLogFlag(BOOLEAN printLogFlag)
{
   LOGG_printLogFlag = printLogFlag;
}

/****************************************************************************
*
*  Function Name: LOGG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
BOOL LOGG_open()
{
#if DEFSG_IS_TARGET
   if(sizeof(LOGG_cevaLogbufferT) > MEM_MAPG_CEVA_LOG_SIZE)
   {
      return FALSE;
   }
#endif

   OS_LYRG_aquireMutex(&LOGP_logMutex);
#if DEFSG_IS_GP
   LOGP_thrExit=FALSE;
//   LOGG_initializeCpLog(MEM_MAPG_CEVA_LOG_ADDRESS,OS_LYRG_CP_MUTEX_CEVA_LOG_E);
//   LOGG_initializeCpLog(MEM_MAPG_EV61_LOG_ADDRESS,OS_LYRG_CP_MUTEX_EV61_LOG_E);
#endif
   return TRUE;
}

/****************************************************************************
*
*  Function Name: LOGG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_close()
{
   LOGP_thrExit=TRUE;
   LOGG_setPrintLogFlag(FALSE);
   OS_LYRG_releaseMutex(&LOGP_logMutex);
}

#if DEFSG_IS_HOST

/****************************************************************************
*
*  Function Name: LOGG_sendLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_sendLog(const char *log)
{
#ifndef CDC_HOST_LIB
   ERRG_codeE  ret;
   UINT32      msgSize;

   if(LOGG_sendLogFlag == TRUE)
   {

      INT32                status;
      MEM_POOLG_bufDescT   *buffDescP = NULL;
      logDataT             logData;

      OS_LYRG_lockMutex(&LOGP_logMutex);
      if(LOGP_loggerH != NULL)
      {
         msgSize = (UINT32)strlen(log) - 1;  // calc message size without '\n'
         if(msgSize <= LOGP_MAIN_STRING_LEN)
         {
            ret = inu_ref__allocBuf((inu_ref *)LOGP_loggerH, msgSize, &buffDescP);
            if(ERRG_SUCCEEDED(ret))
            {
               logData.freeBufPtr         = buffDescP;
               logData.dataP              = (char *)buffDescP->dataP;
               memcpy(logData.dataP, log, msgSize);
               logData.dataP[msgSize]    = '\0';
               logData.dataLen           = msgSize;
               status = OS_LYRG_sendMsg(LOGP_msgFifoH, (UINT8 *)&(logData), sizeof(logDataT));
               if(status != SUCCESS_E)
               {
                  MEM_POOLG_free(buffDescP);
               }
            }
         }
      }
      OS_LYRG_unlockMutex(&LOGP_logMutex);
   }
#else
   FIX_UNUSED_PARAM_WARN(log);
#endif
}

#elif DEFSG_IS_GP

/****************************************************************************
*
*  Function Name: LOGG_initializeCpLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Logger service
*
****************************************************************************/
void LOGG_initializeCpLog(UINT32 logBufferAddress, OS_LYRG_cpMutexIdE mutexNum)
{
   LOGG_cevaLogbufferT *cevaLogbufferP;
   UINT32 tmp;

   MEM_MAPG_convertPhysicalToVirtual(logBufferAddress, &tmp);
//   MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_CEVA_LOG_E,&tmp);
   cevaLogbufferP = (LOGG_cevaLogbufferT*)tmp;
   memset(cevaLogbufferP->logBuffer,0,sizeof(cevaLogbufferP->logBuffer));

   OS_LYRG_aquireMutexCp(&(cevaLogbufferP->cpMutex),mutexNum);
   cevaLogbufferP->rdBufferIndex = 0;
   cevaLogbufferP->wrBufferIndex = 0;
   cevaLogbufferP->state = LOGG_CEVA_LOG_STATE_EMPTY;
}


/****************************************************************************
*
*  Function Name: LOGP_outputWriteToFile
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGP_outputWriteToFile(const char *log)
{
   if(LOGG_printLogFileFlag == TRUE)
   {
      LOGG_openLogFile();
      fprintf(LOGP_logFileP, log);
   }
}

void LOGG_printLogUpdateFlags()
{
    LOGG_loggerConnected = TRUE;
   LOGG_printLogFileFlag = FALSE;
}

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

void LOGG_clearLogFile()
{
   if(LOGG_printLogFileFlag == TRUE)
   {
      LOGP_logFileP = fopen (LOGP_FILE_NAME,"w");
      if (!LOGP_logFileP)
      {
         printf("Can't open log file %s\n", LOGP_FILE_NAME);
         perror("Error: ");
      }

      if (LOGP_logFileP)
         fclose (LOGP_logFileP);
      else
         printf("The file %s is not opened.\n", LOGP_FILE_NAME);
   }
}

void LOGG_openLogFile()
{
   if (!LOGP_logFileP)
   {
      LOGP_logFileP = fopen (LOGP_FILE_NAME,"w+");
      if (!LOGP_logFileP)
      {
         printf("Can't open log file %s\n", LOGP_FILE_NAME);
       }
   }
}

void LOGG_closeLogFile()
{
   if (LOGP_logFileP)
   {
      if (fclose (LOGP_logFileP) == 0)
         LOGP_logFileP = NULL;

   }
   else
      printf("The file %s is not opened.\n", LOGP_FILE_NAME);
}

BOOL LOGG_sendFileToLogger()
{
   char prString[LOGP_MAIN_STRING_LEN];
   BOOL retVal = FALSE;

   if (LOGG_loggerConnected == TRUE)
   {
      retVal = TRUE;
      if ((LOGP_logFileP))
      {
         fseek(LOGP_logFileP, 0, SEEK_SET);

         while (fgets(prString, LOGP_MAIN_STRING_LEN, LOGP_logFileP) != NULL)
         {
            prString[LOGP_MAIN_STRING_LEN - 1] = 0;
            LOGG_sendLog(prString);
         }
         LOGG_closeLogFile();
         remove(LOGP_FILE_NAME);
      }
   }

   return retVal;
}

/****************************************************************************
*
*  Function Name: LOGG_sendLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_sendLog(const char *log)
{
   UINT32               msgSize;
   char                 *logP = (char *)log;
   if(LOGG_sendLogFlag == TRUE)
   {
      LOGP_dataHdrT        hdr;
      if(LOGP_loggerH != NULL)
      {
         msgSize = (UINT32)strlen(log);
         *(logP + msgSize) = '\0';
         if(msgSize <= LOGP_MAIN_STRING_LEN)
         {

            hdr.pid     = CURR_PID;
            hdr.level   = LOG_INFO_E;
            hdr.msgSize = msgSize;
            inu_ref__copyAndSendDataAsync((inu_ref*)LOGP_loggerH,INTERNAL_CMDG_LOG_DATA_HDR_E,&hdr,(void*)log,LOGP_MAIN_STRING_LEN);
         }
      }
   }
}

/****************************************************************************
*
*  Function Name: LOGG_sendLogCdc
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_sendLogCdc(const char *log)
{
   if(LOGG_sendLogCdcFlag == TRUE)
   {
      UINT32  msgSize;
      char    *logP = (char *)log;

      msgSize = (UINT32)strlen(log);
      *(logP + msgSize) = '\0';
      if(msgSize <= LOGP_MAIN_STRING_LEN)
      {
         CDC_MNGRG_log(logP);
      }
   }
}

/****************************************************************************
*
*  Function Name: LOGP_outputCevaStrLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
static void LOGP_outputCevaStrLog(LOGG_cevaLogbufferT *cevaLogbufferP, const char* procId)
{
   char prefix[125];
   char cevaLevelStr[16];
   char cevaIdStr[LOGG_DSP_ID_HEADER_SIZE];
   UINT32 *cevaIdP;
   UINT32 cevaId;
   INU_DEFSG_logLevelE level;

   do
   {
      OS_LYRG_lockMutexCp(cevaLogbufferP->cpMutex);
      if((cevaLogbufferP->rdBufferIndex != cevaLogbufferP->wrBufferIndex) || ((cevaLogbufferP->state == LOGG_CEVA_LOG_STATE_FULL) && (cevaLogbufferP->wrBufferIndex == cevaLogbufferP->rdBufferIndex)))
      {
         cevaIdP = (UINT32*)&cevaLogbufferP->logBuffer[cevaLogbufferP->rdBufferIndex][0];
         cevaId = *cevaIdP++;
         strcpy(cevaIdStr, procId);
         if ((cevaId & 0xF) == 1)
         {
            strcpy(cevaIdStr, LOGG_EV72_1_ID_STR);
         }
         level = cevaId >> 4;
         if (level <= LOGP_dbgLevel)
         {
            memset(cevaLevelStr,0,sizeof(cevaLevelStr));
            LOGP_getLevelString(level, cevaLevelStr,sizeof(cevaLevelStr));          
            snprintf(prefix,125, "FW 4KC0 LOG (%s): %s%s", cevaIdStr,cevaLevelStr, (char*)cevaIdP);
            LOGP_outputStrLogMethods(&prefix[0]);
         }
         cevaLogbufferP->rdBufferIndex++;
         cevaLogbufferP->rdBufferIndex &= (LOGP_NUM_OF_CEVA_LOG_BUFS-1);
         cevaLogbufferP->state = (cevaLogbufferP->wrBufferIndex == cevaLogbufferP->rdBufferIndex) ? LOGG_CEVA_LOG_STATE_EMPTY : LOGG_CEVA_LOG_STATE_FULL;
      }
      OS_LYRG_unlockMutexCp(cevaLogbufferP->cpMutex);
   } while(cevaLogbufferP->state == LOGG_CEVA_LOG_STATE_FULL);
}


/****************************************************************************
*
*  Function Name: LOGG_cevaThread
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
void LOGG_cevaThread(void *arg)
{
   (void)arg;
   while(!LOGP_thrExit)
   {
      OS_LYRG_usleep(100*1000);
      LOGG_outputCpStrLog();
   }
}

/****************************************************************************
*
*  Function Name: LOGG_outputCpStrLog
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: logger
*
****************************************************************************/
extern BOOL CNN_BOOTG_isActive( void );
ERRG_codeE LOGG_outputCpStrLog()
{
   ERRG_codeE ret = RET_SUCCESS;
   LOGG_cevaLogbufferT *cevaLogbufferP;
   UINT32 tmp;

   if (CNN_BOOTG_isActive())
   {
      ret = MEM_MAPG_convertPhysicalToVirtual(MEM_MAPG_EV61_LOG_ADDRESS, &tmp);
      if(ERRG_FAILED(ret))
      {
         return ret;
      }
      cevaLogbufferP = (LOGG_cevaLogbufferT*)tmp;
      LOGP_outputCevaStrLog(cevaLogbufferP,LOGG_EV72_0_ID_STR);
   }

   return ret;
}

#endif  //if host else GP

#ifdef __cplusplus
}
#endif

#endif   //#else not ceva
