/****************************************************************************
 *
 *   FileName: log.h
 *
 *   Author:  Yaron A
 *
 *   Date:
 *
 *   Description: Inuitive camera channel Service API
 *
 ****************************************************************************/
#ifndef LOG_H
#define LOG_H
#include "inu_types.h"
#include "inu_defs.h"
#include "err_defs.h"
#include <string.h>
#include "mem_pool.h"
#include "os_lyr.h"


#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
// Consider to move to make file
#define DEBUG_LOG

#if DEFSG_IS_OS_WINDOWS
#define _FILE_ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
#define _FILE_ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define LOGP_MSG_HDR_LEN   sizeof(LOGP_dataHdrT)
#if DEFSG_IS_TARGET

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RESET   "\x1b[0m"

#define RED(_X_)           (COLOR_RED      _X_  COLOR_RESET)
#define GREEN(_X_)         (COLOR_GREEN    _X_  COLOR_RESET)
#define BLUE(_X_)          (COLOR_BLUE     _X_  COLOR_RESET)
#define YELLOW(_X_)        (COLOR_YELLOW   _X_  COLOR_RESET)
#define CYAN(_X_)          (COLOR_CYAN     _X_  COLOR_RESET)
#define MAGENTA(_X_)       (COLOR_MAGENTA  _X_  COLOR_RESET)

#endif

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   UINT32 pid;
   UINT32 level;
   UINT32 msgSize;
} LOGP_dataHdrT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
#if !(DEFSG_IS_CEVA)
BOOL LOGG_open();
void  LOGG_close();
void  LOGG_start(void *loggerH,OS_LYRG_msgQueT *msgFifoH);
void  LOGG_stop();
void  LOGG_openLogFile();
void  LOGG_closeLogFile();
void  LOGG_clearLogFile();
void  LOGG_sendLog(const char *log);
void  LOGG_sendLogCdc(const char *log);
void  LOGG_getDbgLevel(INU_DEFSG_logLevelE *levelP);
void  LOGG_setDbgLevel(INU_DEFSG_logLevelE level);
void  LOGG_setSendLogFlag(BOOLEAN sendLogFlag);
void  LOGG_setSendLogCdcFlag(BOOLEAN sendLogCdcFlag);
void  LOGG_setLoggerMainFlag(BOOLEAN loggerMainFlag);
void  LOGG_setPrintLogFlag(BOOLEAN printLogFlag);
void  LOGG_setDbgLevel(INU_DEFSG_logLevelE level);
#if DEFSG_IS_TARGET
void  LOGG_initializeCpLog(UINT32 logBufferAddress, OS_LYRG_cpMutexIdE mutexNum);
#endif
void  LOGG_cevaThread(void *arg);
ERRG_codeE  LOGG_outputCpStrLog();

#if DEFSG_IS_GP
BOOL LOGG_sendFileToLogger();
#endif
void LOGG_printLogUpdateFlags();
#endif	//#if !(DEFSG_IS_CEVA)

#ifdef DEBUG_LOG
   #define SET_DBG_LEVEL(level) LOGG_setDbgLevel(level)
   #if DEFSG_IS_CEVA || DEFSG_IS_EV72
      void LOGG_outputStrLog(INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char * fmt, ...);
      #define LOGG_PRINT(level, errCode, args...) LOGG_outputStrLog(level, errCode, args)
   #else
#if SCNN_SIM
    #define LOGG_PRINT(level, errCode, ...)
#else
	  void LOGG_outputStrLog(INU_DEFSG_pidE pid, INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char * fmt, ...);
      #define LOGG_PRINT(level, errCode, ...) LOGG_outputStrLog(CURR_PID, level, (ERRG_codeE)errCode, _FILE_, __func__, __LINE__, __VA_ARGS__)
      #define LOGG_PRINT_PRINTF( level, ...) LOGG_outputStrLog(CURR_PID, level, (ERRG_codeE)0, "", "", 0, __VA_ARGS__)
#endif
   #endif
#else
   #if DEFSG_IS_CEVA || DEFSG_IS_EV72
      #define LOGG_PRINT(level, args...) ((void)0)
   #else
      #define LOGG_PRINT(level, errCode, args...) ((void)0)
   #endif
#endif


#ifdef __cplusplus
}
#endif
#endif // LOG_H
