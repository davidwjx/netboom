/****************************************************************************
 * 
 *   FileName: svcfg.c
 *
 *   Description: Services Configuration Implementation 
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"

#include "svc_cfg.h"
#include "inu_common.h"

#include "internal_cmd.h"
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

#include "sys_defs.h"
#include "os_lyr.h"
#include "mem_pool.h"
#include "svc_com.h"
#include "conn_lyr.h"
#include "marshal.h"

#include "service.h"

#include "usb_ctrl_com.h"
#include "uart_com.h"
#include "ipc_com.h"

#if DEFSG_IS_GP
#include "version.h"
#include <string.h>
#include "hw_mngr.h"
#include "wd.h"
#endif 

#if DEFSG_IS_OS_WINDOWS
#define STRTOK_R  strtok_s
#else
#define STRTOK_R  strtok_r
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define SVCFG_LOG_NONE   (0)
#define SVCFG_LOG_LOW    (1)
#define SVCFG_LOG_MED    (2)
#define SVCFG_LOG_HIGH   (3)

static int svcfgLogLevel = SVCFG_LOG_LOW;

#define log_svcfg(level, ...)\
   do{\
      if(level <= svcfgLogLevel)\
      {\
         LOGG_PRINT(LOG_INFO_E, 0, __VA_ARGS__);\
      }\
   }while(0)

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#if (DEFSG_IS_HOST_LINUX) 
#define SVCFG_SVC_INQUE_SIZE_DEFAULT  (10)
#define SVCFG_LOGGER_INQUE_SIZE       (SVCFG_SVC_INQUE_SIZE_DEFAULT)
#define SVCFG_SYSTEM_INQUE_SIZE       (SVCFG_SVC_INQUE_SIZE_DEFAULT)
#define SVCFG_INJECT_INQUE_SIZE       (SVCFG_SVC_INQUE_SIZE_DEFAULT)
#define SVCFG_LOGGER_WAKEUP_MSEC      (SVCFG_THREAD_WAKEUP_MSEC_DEFAULT)
#else
#define SVCFG_SVC_INQUE_SIZE_DEFAULT   (16)
#define SVCFG_SYSTEM_INQUE_SIZE        (80)
#define SVCFG_LOGGER_INQUE_SIZE        (96)
#define SVCFG_INJECT_INQUE_SIZE        (100)
#define SVCFG_LOGGER_WAKEUP_MSEC       (50)
#endif
#define SVCFG_CONN_QUE_SIZE_DEFAULT      (1024)
#define SVCFG_THREAD_WAKEUP_MSEC_DEFAULT (200)

#define USB_INTERFACE_NUM  (7)
#define SVCFG_UART_PORT   (31)
#define SVCFG_UART_RATE   (921600)

#define SVCFG_USB_IF0_STR     "usb_if_0"
#define SVCFG_USB_IF1_STR     "usb_if_1"
#define SVCFG_UART_STR        "uart"
 
#define SVCFG_MNGR_STR           "svc_mngr"
#define SVCFG_SYSTEM_STR         "svc_system"
#define SVCFG_LOGGER_STR         "svc_logger"
#define SVCFG_VIDEO_STREAM_STR   "svc_video"
#define SVCFG_DEPTH_STREAM_STR   "svc_depth"
#define SVCFG_WEBCAM_STREAM_STR  "svc_webcam"
#define SVCFG_INJECT_STREAM_STR  "svc_inject"
#define SVCFG_ALG_STREAM_STR     "svc_alg"
#define SVCFG_CLIENT_STREAM_STR  "svc_client"
#define SVCFG_POS_SENSORS_STR    "svc_position"
#define SVCFG_AUDIO_STR          "svc_audio"


/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/
typedef struct
{
   unsigned int numBufs;
   unsigned int size;
} SVCFG_memT;

typedef enum
{
   SVCFG_TYPE_DATA_CONN_E = 0,
   SVCFG_TYPE_HOST_MEM_E,
   SVCFG_NUM_TOKENS_E,
    
} SVCFG_tokeTypeE;

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
#ifdef OLD_API
static USB_CTRL_COMG_paramsT usbIfParams[USB_INTERFACE_NUM];
static UART_COMG_interfaceParamsT uartIfParams;
#endif
static SVCFG_memConfigT memPoolConfigTbl[SVCFG_MEMPOOL_NUM_POOLS_E]; //Memory Pool Configuration Table
static SVCFG_conConfigT connConfigTbl[SVCFG_NUM_CONNECTIONS_E]; //Connection Configuration Table
static SVCFG_chanConfigT chanConfigTbl[SVCFG_NUM_CHANNELS_E]; //Channel Configuration Table
static SVCFG_svcConfigT svcConfigTbl[SVCFG_NUM_SERVICES_E]; //Service Configuration Table

static char *memStrTbl[SVCFG_MEMPOOL_NUM_POOLS_E];
static char *connStrTbl[SVCFG_NUM_CONNECTIONS_E];
static char *svcStrTbl[SVCFG_NUM_SERVICES_E];

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
/*
 count trailing zeros. returns number of zeros preceding least significant 1 bit.
*/
static unsigned int ctz32(UINT32 x)
{
   int n;
   if(x == 0)
      return 32;
   n = 0;
   if ((x & 0x0000FFFF) == 0) { n += 16; x >>= 16; };
   if ((x & 0x000000FF) == 0) { n += 8;  x >>= 8; };
   if ((x & 0x0000000F) == 0) { n += 4; x >>= 4; };
   if ((x & 0x00000003) == 0) { n += 2; x >>= 2; };
   if ((x & 0x00000001) == 0) { n += 1; };
   return n;
}

static void printMap(char *buf, unsigned int size, UINT32 map)
{
   unsigned int j = 0;
   while (map)
   {
      j += snprintf(buf + j, size - j, "%d ", ctz32(map));
      map &= (map - 1);
   }
}

void bottom_up_show()
{
   //Walk through service table and print configuration - top down or do 
   unsigned int i;
   log_svcfg(SVCFG_LOG_LOW, "svc configuration bootom up\n");
   char chstr[(SVCFG_NUM_CHANNELS_E * 4)+1];

   log_svcfg(SVCFG_LOG_LOW, "---------------------------------\n");
   log_svcfg(SVCFG_LOG_LOW, "enabled pools:\n");
   for (i = 0; i < SVCFG_MEMPOOL_NUM_POOLS_E; i++)
   {
      if (!memPoolConfigTbl[i].enabled)
         continue;
      printMap(chstr, sizeof(chstr), memPoolConfigTbl[i].chanMap);
      log_svcfg(SVCFG_LOG_LOW, "  [%d]%s\n", i, memStrTbl[i]);
      log_svcfg(SVCFG_LOG_LOW, "    channels: %s\n", chstr);
   }

   log_svcfg(SVCFG_LOG_LOW, "---------------------------------\n");
   log_svcfg(SVCFG_LOG_LOW, "enabled connections:\n");
   for (i = 0; i < SVCFG_NUM_CONNECTIONS_E; i++)
   {
      if (!connConfigTbl[i].enabled)
         continue;

      printMap(chstr, sizeof(chstr), connConfigTbl[i].chanMap);
      log_svcfg(SVCFG_LOG_LOW, "  [%d]%s\n", i, connStrTbl[i]);
      log_svcfg(SVCFG_LOG_LOW, "    channels: %s\n", chstr);

   }

   log_svcfg(SVCFG_LOG_LOW, "---------------------------------\n");
   log_svcfg(SVCFG_LOG_LOW, "enabled channels:\n");
   for (i = 0; i < SVCFG_NUM_CHANNELS_E; i++)
   {
      if (!chanConfigTbl[i].enabled)
         continue;

      log_svcfg(SVCFG_LOG_LOW, "  [%d]chan: %s\n", i, svcStrTbl[chanConfigTbl[i].svcId]);
   }

   log_svcfg(SVCFG_LOG_LOW, "---------------------------------\n");
   log_svcfg(SVCFG_LOG_LOW, "enabled services:\n");
   for (i = 0; i < SVCFG_NUM_SERVICES_E; i++)
   {
      if (!svcConfigTbl[i].enabled)
         continue;
      log_svcfg(SVCFG_LOG_LOW, "  [%d]%s\n", i, svcStrTbl[i]);
   }
}

static void top_down_show()
{
   unsigned int i;
   log_svcfg(LOG_INFO_E, "svc configuration top down\n");

   for (i = 0; i < SVCFG_NUM_SERVICES_E; i++)
   {
      if (!svcConfigTbl[i].enabled)
         continue;

      log_svcfg(SVCFG_LOG_LOW, "svc [%d]%s ch=%d,%d\n", i, svcStrTbl[i], SVCFG_CTRL_CH(svcConfigTbl[i].chanIdBase), SVCFG_DATA_CH(svcConfigTbl[i].chanIdBase));
   }
   for (i = 0; i < SVCFG_NUM_CHANNELS_E; i++)
   {
      if (!chanConfigTbl[i].enabled)
         continue;

      log_svcfg(SVCFG_LOG_LOW, "chan [%d] pool=%s(%d) conn=%s(%d)\n",
         i, memStrTbl[chanConfigTbl[i].memId], chanConfigTbl[i].memId, connStrTbl[chanConfigTbl[i].connId], chanConfigTbl[i].connId);
   }

   for (i = 0; i < SVCFG_MEMPOOL_NUM_POOLS_E; i++)
   {
      if (!memPoolConfigTbl[i].enabled)
         continue;
      log_svcfg(SVCFG_LOG_LOW, "pool [%d]%s\n", i, memStrTbl[i]);
   }
   for (i = 0; i < SVCFG_NUM_CONNECTIONS_E; i++)
   {
      if (!connConfigTbl[i].enabled)
         continue;
      log_svcfg(SVCFG_LOG_LOW, "conn [%d]%s\n", i, connStrTbl[i]);
   }
}

#ifdef OLD_API
static void initMemPoolTblEntry(SVCFG_memIdE id, char *str, MEM_POOLG_typeE type, unsigned int sizeBytes, unsigned int numBufs, int rstptr)
{
   memPoolConfigTbl[id].id = id;
   memPoolConfigTbl[id].type = type;
   memPoolConfigTbl[id].bufferSize = sizeBytes;
   memPoolConfigTbl[id].numBuffers = numBufs;
   memPoolConfigTbl[id].rstptr = rstptr;
   memStrTbl[id] = str;
}

static void initConnConfigEntry(SVCFG_connIdE id, char *str, void *ifCfg, CLS_COMG_getIfOps ifOps, UINT16 queSize,INT32 rxPoolId)
{
   connConfigTbl[id].id = id;
   connConfigTbl[id].connConfig.ifCfg = ifCfg;
   connConfigTbl[id].connConfig.queueSize = queSize;
   connConfigTbl[id].connConfig.getIfOps = ifOps;
   connConfigTbl[id].connConfig.rxPoolId = rxPoolId;
   connStrTbl[id] = str;
}

static void initChConfigEntry(SVCFG_channelIdE id, SVCFG_memIdE memPool, SVCFG_connIdE conn)
{
   chanConfigTbl[id].id = id;
   chanConfigTbl[id].connId = conn;
   chanConfigTbl[id].memId = memPool;
}


static void initSvcEntry(SVCFG_svcIdE id, char *str, INUG_serviceIdE svcId, const SERVICEG_methodsT *methodsP, SVCFG_channelIdE chanIdBase, OS_LYRG_threadIdE threadId, unsigned int inQueSize, int threadWakeup, BOOL linkMonitor, SERVICEG_channelE linkChan)
{
   svcConfigTbl[id].svcId = svcId;
   svcConfigTbl[id].chanIdBase = chanIdBase;
   svcConfigTbl[id].inQueSize = inQueSize;
   svcConfigTbl[id].methodsP = methodsP;
   svcConfigTbl[id].threadId = threadId;
   svcConfigTbl[id].threadWakeup = threadWakeup;
   svcConfigTbl[id].linkMonitor = linkMonitor;
   svcConfigTbl[id].linkChan = linkChan;
   svcStrTbl[id] = str;
}

static void initUsbIfParams(void)
{
   enum { USB_IF0 = 0, USB_IF1 = 1};

   usbIfParams[USB_IF0].usbInterfaceIdx = 0;
   usbIfParams[USB_IF0].fixedBuffSize=0;
   usbIfParams[USB_IF0].fixedSizeCap=FALSE;
   usbIfParams[USB_IF0].txCap=TRUE;
   usbIfParams[USB_IF0].rxCap=TRUE;

   usbIfParams[USB_IF1].usbInterfaceIdx = 1;
   usbIfParams[USB_IF1].fixedBuffSize=1024;
   usbIfParams[USB_IF1].fixedSizeCap=TRUE;
   usbIfParams[USB_IF1].txCap= (DEFSG_IS_HOST) ? FALSE:TRUE;
   usbIfParams[USB_IF1].rxCap=(DEFSG_IS_HOST) ? TRUE:FALSE;
}

static void initUartIfParams(void)
{
   uartIfParams.portNumber = SVCFG_UART_PORT;
   uartIfParams.baudrate = SVCFG_UART_RATE;
}

static void initConnConfigTbl(void)
{
   enum { USB_IF0 = 0, USB_IF1 = 1};
   initUsbIfParams();
   initUartIfParams();

   initConnConfigEntry(SVCFG_CONN_USB_IF0_E, SVCFG_USB_IF0_STR,&(usbIfParams[USB_IF0]),USB_CTRL_COMG_getOps, SVCFG_CONN_QUE_SIZE_DEFAULT, -1);
   initConnConfigEntry(SVCFG_CONN_USB_IF1_E, SVCFG_USB_IF1_STR,&(usbIfParams[USB_IF1]),USB_CTRL_COMG_getOps, SVCFG_CONN_QUE_SIZE_DEFAULT, SVCFG_MEMPOOL_USBIF_FIXED_E);
   initConnConfigEntry(SVCFG_CONN_UART_E, SVCFG_UART_STR,&uartIfParams,UART_COMG_getOps,SVCFG_CONN_QUE_SIZE_DEFAULT,-1);
}

static void initMemPoolConfigTbl(void)
{
#if 0
   initMemPoolTblEntry(SVCFG_MEMPOOL_MNGR_CTRL_E, "mngr_ctrl_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_GENERAL_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_GENERAL_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_ALG_CTRL_E, "alg_ctrl_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_ALG_CTRL_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_ALG_CTRL_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_CLIENT_CTRL_E, "client_ctrl_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_CTRL_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_CTRL_NUM_BUFS, 0);

   initMemPoolTblEntry(SVCFG_MEMPOOL_MNGR_DATA_E, "mngr_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_SVC_MNGR_DATA_SIZE_BYTES, SYS_DEFSG_MEMPOOL_SVC_MNGR_DATA_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_SYSTEM_DATA_E, "system_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_SYSTEM_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_SYSTEM_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_ALG_DATA_E, "alg_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_ALG_DATA_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_ALG_DATA_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_LOGGER_DATA_E, "logger_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_LOGGER_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_LOGGER_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_USBIF_FIXED_E, "usbif_fixed_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_POS_SENSORS_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_POS_SENSORS_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_CLIENT_DATA_E, "client_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_DATA_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_HOST_CLIENT_DATA_NUM_BUFS, 0);
#if (DEFSG_IS_GP)
   initMemPoolTblEntry(SVCFG_MEMPOOL_INJECT_DATA_E, "inject_data_pool", MEM_POOLG_TYPE_USER_E, SYS_DEFSG_MEMPOOL_GP_HOST_INJECT_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_INJECT_VGA_NUM_BUFS, 1);
   initMemPoolTblEntry(SVCFG_MEMPOOL_VIDEO_DATA_E, "video_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_VIDEO_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_VIDEO_POOL_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_DEPTH_DATA_E, "depth_data_pool" , MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_DEPTH_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_DEPTH_POOL_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_WEBCAM_DATA_E, "webcam_data_pool",MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_GP_WEBCAM_SIZE_BYTES, SYS_DEFSG_MEMPOOL_GP_WEBCAM_POOL_NUM_BUFS, 0);
#else
   initMemPoolTblEntry(SVCFG_MEMPOOL_INJECT_DATA_E, "inject_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_HOST_INJECT_SIZE_BYTES, SYS_DEFSG_MEMPOOL_HOST_INJECT_NUM_BUFS, 1);
   initMemPoolTblEntry(SVCFG_MEMPOOL_VIDEO_DATA_E, "video_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_HOST_VIDEO_SIZE_BYTES, SYS_DEFSG_MEMPOOL_HOST_VIDEO_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_DEPTH_DATA_E, "depth_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_HOST_DEPTH_SIZE_BYTES, SYS_DEFSG_MEMPOOL_HOST_DEPTH_NUM_BUFS, 0);
   initMemPoolTblEntry(SVCFG_MEMPOOL_WEBCAM_DATA_E, "webcam_data_pool", MEM_POOLG_TYPE_INTERNAL_E, SYS_DEFSG_MEMPOOL_HOST_WEBCAM_SIZE_BYTES, SYS_DEFSG_MEMPOOL_HOST_WEBCAM_NUM_BUFS, 0);
#endif
#endif
}


static void initChanConfigTbl(void)
{
#if defined (DEFSG_IS_USB_BULK)
   SVCFG_connIdE ctrlConn= SVCFG_CONN_USB_IF0_E;
   SVCFG_connIdE streamConn= SVCFG_CONN_USB_IF0_E;
   SVCFG_connIdE fixedCon =SVCFG_CONN_USB_IF0_E;
#elif defined(DEFSG_IS_MULTI_USB)
   SVCFG_connIdE ctrlConn= SVCFG_CONN_USB_IF0_E;
   SVCFG_connIdE streamConn= SVCFG_CONN_USB_IF0_E;
   SVCFG_connIdE fixedCon =SVCFG_CONN_USB_IF1_E;
#elif defined(DEFSG_IS_UART_USB)
   SVCFG_connIdE ctrlConn= SVCFG_CONN_UART_E;
   SVCFG_connIdE streamConn= SVCFG_CONN_USB_IF0_E;
   SVCFG_connIdE fixedCon =SVCFG_CONN_UART_E;
#elif defined(DEFSG_IS_UART_MIPI)
   SVCFG_connIdE ctrlConn= SVCFG_CONN_UART_E;
   SVCFG_connIdE streamConn= SVCFG_CONN_UART_E;
   SVCFG_connIdE fixedCon =SVCFG_CONN_UART_E;
#endif
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_MNGR_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_SYSTEM_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_INJECT_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_VIDEO_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_DEPTH_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_WEBCAM_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_ALG_E), SVCFG_MEMPOOL_ALG_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_LOGGER_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_CLIENT_E), SVCFG_MEMPOOL_CLIENT_CTRL_E, ctrlConn);
   initChConfigEntry(SVCFG_CTRL_CH(SVCFG_CHAN_BASE_POS_SENSORS_E), SVCFG_MEMPOOL_MNGR_CTRL_E, ctrlConn);
   
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_MNGR_E), SVCFG_MEMPOOL_MNGR_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_SYSTEM_E), SVCFG_MEMPOOL_SYSTEM_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_INJECT_E), SVCFG_MEMPOOL_INJECT_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_VIDEO_E), SVCFG_MEMPOOL_VIDEO_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_DEPTH_E), SVCFG_MEMPOOL_DEPTH_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_WEBCAM_E), SVCFG_MEMPOOL_WEBCAM_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_ALG_E), SVCFG_MEMPOOL_ALG_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_LOGGER_E), SVCFG_MEMPOOL_LOGGER_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_CLIENT_E), SVCFG_MEMPOOL_CLIENT_DATA_E, streamConn);
   initChConfigEntry(SVCFG_DATA_CH(SVCFG_CHAN_BASE_POS_SENSORS_E), SVCFG_MEMPOOL_USBIF_FIXED_E, fixedCon);

};
#endif
#ifdef OLD_API
static void initServiceConfigTbl()
{
   //initSvcEntry(SVCFG_SVC_MNGR_E,          SVCFG_MNGR_STR,       INUG_SERVICE_MNGR_E, SVC_MNGRP_getMethods(),         SVCFG_CHAN_BASE_MNGR_E,       OS_LYRG_SERVICE_MNGR_THREAD_ID_E ,     SVCFG_SVC_INQUE_SIZE_DEFAULT,         SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, TRUE,    SERVICEG_CTRL_CHAN_E  );
   //initSvcEntry(SVCFG_SVC_SYSTEM_E,        SVCFG_SYSTEM_STR,     INUG_SERVICE_SYSTEM_E, SYSTEMG_getMethods(),         SVCFG_CHAN_BASE_SYSTEM_E,     OS_LYRG_SERVICE_SYSTEM_THREAD_ID_E ,   SVCFG_SYSTEM_INQUE_SIZE,              SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  );
   //initSvcEntry(SVCFG_SVC_LOGGER_E,        SVCFG_LOGGER_STR,     INUG_SERVICE_LOGGER_E, LOGGERG_getMethods(),         SVCFG_CHAN_BASE_LOGGER_E,     OS_LYRG_SERVICE_LOGGER_THREAD_ID_E,    SVCFG_LOGGER_INQUE_SIZE,              SVCFG_LOGGER_WAKEUP_MSEC ,         FALSE,  SERVICEG_CTRL_CHAN_E  );
//   initSvcEntry(SVCFG_SVC_VIDEO_STREAM_E,  SVCFG_VIDEO_STREAM_STR,  INUG_SERVICE_VIDEO_STREAM_E, VIDEOG_getMethods(),   SVCFG_CHAN_BASE_VIDEO_E,      OS_LYRG_SERVICE_VIDEO_THREAD_ID_E,     SVCFG_SVC_INQUE_SIZE_DEFAULT,         SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  );
//   initSvcEntry(SVCFG_SVC_DEPTH_STREAM_E,  SVCFG_DEPTH_STREAM_STR, INUG_SERVICE_DEPTH_STREAM_E, DEPTHG_getMethods(),  SVCFG_CHAN_BASE_DEPTH_E,      OS_LYRG_SERVICE_DEPTH_THREAD_ID_E,     SVCFG_SVC_INQUE_SIZE_DEFAULT,         SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  ),
//   initSvcEntry(SVCFG_SVC_WEBCAM_STREAM_E, SVCFG_WEBCAM_STREAM_STR, INUG_SERVICE_WEBCAM_STREAM_E, WEBCAMG_getMethods(), SVCFG_CHAN_BASE_WEBCAM_E,   OS_LYRG_SERVICE_WEBCAM_THREAD_ID_E,    SVCFG_SVC_INQUE_SIZE_DEFAULT,         SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  ),
//   initSvcEntry(SVCFG_SVC_INJECT_STREAM_E,  SVCFG_INJECT_STREAM_STR,   INUG_SERVICE_INJECT_STREAM_E, INJECTG_getMethods(),   SVCFG_CHAN_BASE_INJECT_E,     OS_LYRG_SERVICE_INJECTION_THREAD_ID_E,  SVCFG_INJECT_INQUE_SIZE,             SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  ),
//   initSvcEntry(SVCFG_SVC_ALG_STREAM_E,    SVCFG_ALG_STREAM_STR,       INUG_SERVICE_ALG_STREAM_E, ALGG_getMethods(),         SVCFG_CHAN_BASE_ALG_E,        OS_LYRG_SERVICE_ALG_THREAD_ID_E,        SVCFG_SVC_INQUE_SIZE_DEFAULT,        SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  ),
   //initSvcEntry(SVCFG_SVC_CLIENT_STREAM_E,  SVCFG_CLIENT_STREAM_STR,   INUG_SERVICE_CLIENT_STREAM_E, CLIENTG_getMethods(),   SVCFG_CHAN_BASE_CLIENT_E,     OS_LYRG_SERVICE_CLIENT_THREAD_ID_E,     SVCFG_SVC_INQUE_SIZE_DEFAULT,        SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  );
//   initSvcEntry(SVCFG_SVC_POS_SENSORS_E,   SVCFG_POS_SENSORS_STR,   INUG_SERVICE_POS_SENSORS_E, POS_SENSORSG_getMethods(),  SVCFG_CHAN_BASE_POS_SENSORS_E, OS_LYRG_SERVICE_POS_SENSORS_THREAD_ID_E,  SVCFG_SVC_INQUE_SIZE_DEFAULT,   SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  );
//   initSvcEntry(SVCFG_SVC_AUDIO_E,         SVCFG_AUDIO_STR,     INUG_SERVICE_AUDIO_STREAM_E, AUDIOG_getMethods(),     SVCFG_CHAN_BASE_AUDIO_E,      OS_LYRG_SERVICE_AUDIO_THREAD_ID_E,      SVCFG_SVC_INQUE_SIZE_DEFAULT,        SVCFG_THREAD_WAKEUP_MSEC_DEFAULT, FALSE,   SERVICEG_CTRL_CHAN_E  );
}
#endif
static void clearMaps()
{
   unsigned int i;
   for (i = 0; i < SVCFG_MEMPOOL_NUM_POOLS_E; i++)
      memPoolConfigTbl[i].chanMap = 0;
   for (i = 0; i < SVCFG_NUM_CONNECTIONS_E; i++)
      connConfigTbl[i].chanMap = 0;
   for (i = 0; i < SVCFG_NUM_CHANNELS_E; i++)
      chanConfigTbl[i].svcId = (SVCFG_svcIdE)~0;
}

static void updateMaps(void)
{
   unsigned int i;

   clearMaps();
   for (i = 0; i < SVCFG_NUM_SERVICES_E; i++)
   {
      if (!svcConfigTbl[i].enabled)
         continue;
      //set the service id in the service's channels
      chanConfigTbl[SVCFG_CTRL_CH(svcConfigTbl[i].chanIdBase)].svcId = (SVCFG_svcIdE)i;
      chanConfigTbl[SVCFG_DATA_CH(svcConfigTbl[i].chanIdBase)].svcId = (SVCFG_svcIdE)i;
   }
   for (i = 0; i < SVCFG_NUM_CHANNELS_E; i++)
   {
      if (!chanConfigTbl[i].enabled)
         continue;

      //set the channel bit in the channel's memory pool channel map and connection channel map.
      memPoolConfigTbl[chanConfigTbl[i].memId].chanMap |= (1 << i);
      connConfigTbl[chanConfigTbl[i].connId].chanMap |= (1 << i);
   }
}

static void initDefaultConfiguration(void)
{
#ifdef OLD_API
   initConnConfigTbl();
   initMemPoolConfigTbl();
   initChanConfigTbl();
   initServiceConfigTbl();
#endif
}

/*
* enable preconfigured svc components
*/
static void enableSvc(SVCFG_svcIdE svcId)
{
   SVCFG_svcConfigT *svc = &svcConfigTbl[svcId];
   SVCFG_chanConfigT *ctrlCh = &chanConfigTbl[SVCFG_CTRL_CH(svc->chanIdBase)];
   SVCFG_chanConfigT *dataCh = &chanConfigTbl[SVCFG_DATA_CH(svc->chanIdBase)];

   ctrlCh->enabled = 1;
   memPoolConfigTbl[ctrlCh->memId].enabled = 1;
   connConfigTbl[ctrlCh->connId].enabled = 1;
   dataCh->enabled = 1;
   memPoolConfigTbl[dataCh->memId].enabled = 1;
   connConfigTbl[dataCh->connId].enabled = 1;
   svc->enabled = 1;
}

#if 0
static void disableSvc(SVCFG_svcIdE svcId)
{
   SVCFG_svcConfigT *svc = &svcConfigTbl[svcId];
   SVCFG_chanConfigT *ctrlCh = &chanConfigTbl[SVCFG_CTRL_CH(svc->chanIdBase)];
   SVCFG_chanConfigT *dataCh = &chanConfigTbl[SVCFG_DATA_CH(svc->chanIdBase)];

   ctrlCh->enabled = 0;
   memPoolConfigTbl[ctrlCh->memId].enabled = 0;
   connConfigTbl[ctrlCh->connId].enabled = 0;
   dataCh->enabled = 0;
   memPoolConfigTbl[dataCh->memId].enabled = 0;
   connConfigTbl[dataCh->connId].enabled = 0;
   svc->enabled = 0;
}
#endif

static void deactivateAll(void)
{
   int i;
   for(i = 0;i <SVCFG_MEMPOOL_NUM_POOLS_E; i++)
      memPoolConfigTbl[i].enabled = 0;

   for(i = 0;i <SVCFG_NUM_CONNECTIONS_E; i++)
      connConfigTbl[i].enabled = 0;
   
   for(i = 0;i <SVCFG_NUM_CHANNELS_E; i++)
      chanConfigTbl[i].enabled = 0;
   
   for(i = 0;i <SVCFG_NUM_SERVICES_E; i++)
      svcConfigTbl[i].enabled = 0;
}


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
/*
   Initialize configuration to defaults and mark resources as enabled.
*/
void SVCFG_init(void)
{
   //Initialize configuration
   initDefaultConfiguration();

   //Top-down enabling of services and related resources.
   deactivateAll();
   enableSvc(SVCFG_SVC_MNGR_E);
   enableSvc(SVCFG_SVC_SYSTEM_E);
   enableSvc(SVCFG_SVC_LOGGER_E);
   enableSvc(SVCFG_SVC_VIDEO_STREAM_E);
   enableSvc(SVCFG_SVC_DEPTH_STREAM_E);
   enableSvc(SVCFG_SVC_WEBCAM_STREAM_E);
   enableSvc(SVCFG_SVC_INJECT_STREAM_E);
   //enableSvc(SVCFG_SVC_ALG_STREAM_E);
   //enableSvc(SVCFG_SVC_CLIENT_STREAM_E);
   //enableSvc(SVCFG_SVC_POS_SENSORS_E);
   //enableSvc(SVCFG_SVC_AUDIO_E);

   updateMaps();
   //SVCFG_show(0);
}

/*
Update memory pool sizes with marshalling and header sizes.
Memory pools need to include space for the marshalled msg sizes + cls header for reception of messages.
On the tx side the msg + cls header are in a seperate buffer from the data - but we don't assume this.

Instead of hard-coding the maximum msg sizes, we get the maximum size for each memory pool based
on which channels/services are configured to use it.
*/
void SVCFG_updateMemSizes(void)
{
   INT32 i, j;
   UINT32 chanMap, mmax, max;
   SVCFG_memConfigT *mentry;
   for (i = 0; i < SVCFG_MEMPOOL_NUM_POOLS_E; i++)
   {
      //For all channels using using the pool, take the max over all their service's sizes
      LOGG_PRINT(LOG_DEBUG_E, NULL, "updating sizes: pool %d\n", i);
      mentry = SVCFG_getMemConfig((SVCFG_memIdE)i);
      chanMap = mentry->chanMap;
      j = 0;
      mmax = 0;
      while (chanMap)
      {
         if (chanMap & 0x1)
         {
            //get the channel's service max size
            max = MARSHALG_getMaxSize(id2Sid(SVCFG_getChanConfig((SVCFG_channelIdE)j)->svcId));
            if (max > mmax)
            {
               mmax = max;
               LOGG_PRINT(LOG_DEBUG_E, NULL, "  active chan %d (svcId=%d) max = %d mmax=%d\n", j, (SVCFG_getChanConfig((SVCFG_channelIdE)j)->svcId), max, mmax);
            }

         }
         chanMap >>= 1;
         j++;
      }

      //update the pool size
      mentry->bufferSize += (mmax + SVC_COMG_getFullHdrMarshalSize());
      mentry->bufferSize = (mentry->bufferSize + 4) & (~0x3);
      LOGG_PRINT(LOG_DEBUG_E, NULL, "  total = %d (maxmsg %d + hdr %d)\n", mentry->bufferSize, mmax, SVC_COMG_getFullHdrMarshalSize());
   }

}

SVCFG_memConfigT *SVCFG_getMemConfig(SVCFG_memIdE id)
{
   return &memPoolConfigTbl[id];
}
SVCFG_conConfigT *SVCFG_getConnConfig(SVCFG_connIdE id)
{
   return &connConfigTbl[id];
}
SVCFG_chanConfigT *SVCFG_getChanConfig(SVCFG_channelIdE id)
{
   return &chanConfigTbl[id];
}
SVCFG_svcConfigT *SVCFG_getSvcConfig(SVCFG_svcIdE svcId)
{
   return &svcConfigTbl[svcId];
}

void SVCFG_show(int botup)
{
   //Walk through service table and print configuration - top down or bottom up 
   if (botup)
      bottom_up_show();
   else
      top_down_show();
}


#ifdef __cplusplus
}
#endif




