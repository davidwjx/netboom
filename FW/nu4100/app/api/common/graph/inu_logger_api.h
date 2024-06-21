#ifndef _INU_LOGGER_API_H_
#define _INU_LOGGER_API_H_

#include "inu2.h"

typedef void* inu_logH;

typedef struct
{
   void     *freeBufPtr;
   char     *dataP;
   UINT32   dataLen;
} logDataT;

typedef struct
{
   INU_DEFSG_logLevelE  log_levels[NUM_OF_PROCESSORS];//log level per unit
   int                  quiet;                        //While this mode is enabled the library will not output anything to stderr, but will continue to write to the file if one is set.
   FILE                 *fp;                          //A file pointer where the log should be written
   BOOLEAN              hostPrintLogs;                //"host fw" prints "host fw logs"
   BOOLEAN              hostSendLogs;                 //"host fw" sends "host fw logs" to msg_queue so the host will be able to print the log.
   BOOLEAN              hostPrintGpLogs;              //"host fw" prints "gp logs"
   BOOLEAN              hostSendGpLogs;               //"host fw" sends "gp logs" to msg_queue so the host will be able to print the log.
   BOOLEAN              gpPrintLogs;                  //prints "gp prints" in gp
   BOOLEAN              gpSendLogs;                   //sends "gp prints" to host
   BOOLEAN              gpSendCdcLogs;                //sends "gp prints" to host via cdc
}inu_log__configT;

typedef struct
{
   inu_deviceH       deviceH;
   inu_log__configT  configParams;
}inu_log__initParams;

ERRG_codeE inu_logger__new(inu_logH *meH, inu_log__initParams *initParamsP);
void inu_logger__delete(inu_logH meH);

void inu_logger__start(inu_logH meH);
void inu_logger__stop();
ERRG_codeE inu_logger__read(inu_logH meH, logDataT *logData);
ERRG_codeE inu_logger__free(logDataT *logData);
#endif
