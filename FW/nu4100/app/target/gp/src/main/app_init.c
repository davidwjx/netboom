/****************************************************************************
 *
 *   FileName: app_init.c
 *
 *   Author: Benny Vaturi
 *
 *   Date: 21/01/13
 *
 *   Description: CEVA application initialization process
 *
 ****************************************************************************/

/****************************************************************************
 ***************      I N C L U D E   F I L E S                **************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
      extern "C" {
#endif

#include "data_base.h"
#include "app_init.h"
#include "version.h"
#include "os_lyr.h"
#include "hw_mngr.h"
#include "hcg_mngr.h"
#include "hca_mngr.h"
#include "gme_mngr.h"
#include "pwr_mngr.h"
#include "inu2_internal.h"
#include "icc.h"
#include "trigger_mngr.h"
#include "inu_charger.h"
#include "charger.h"
#include "model.h"
#include "marshal.h"
#include "mem_pool.h"
#include "wd.h"

#include "inu2.h"
#ifdef ENABLE_TRACING
#include "trace-marker.h"
#include <unistd.h>
#include <sys/syscall.h>
#endif
//#define TEST_PERF_APP
#ifdef TEST_PERF_APP
#include "usb_ctrl_com.h"
#include "mem_map.h"
#include <sys/time.h>
#include "cmem.h"
#endif

/****************************************************************************
 ***************      L O C A L       D E F N I T I O N S     ***************
****************************************************************************/
#define APP_INITP_NUM_INIT_CB       (2)  //Number of initialization callbacks supported
#define APP_INITP_NUM_INTERRUPT_CB  (2)  // Number of interrupt callbacks supported for each interrupt source

/****************************************************************************
 ***************      L O C A L       T Y P E D E F S         ***************
****************************************************************************/
typedef struct
{
   APP_INITG_initCbT cb;
   void              *arg;
} APP_INITP_initCbInfoT;

typedef void (*APP_INITP_initCbT)(void *arg);

typedef struct
{
    APP_INITP_initCbInfoT   initCbTbl[APP_INITP_NUM_INIT_CB];
    UINT32                  initCbTblIndex;
    OS_LYRG_threadHandle    monitorThreadH;
} APP_INITP_infoT;



/****************************************************************************
 ***************      L O C A L       D A T A                 ***************
****************************************************************************/
static APP_INITP_infoT  APP_INITP_info;

/****************************************************************************
 ***************      G L O B A L     D A T A                 ***************
****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
****************************************************************************/
extern ERRG_codeE CNN_BOOTG_start( void );
/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
****************************************************************************/
static void APP_INITP_initializeCbDispatch(void);
#if 0
static void APP_INITP_initalizeCbTblInit(void);
#endif
static void APP_INITP_setInitCbEntry(APP_INITP_initCbInfoT *entryP, APP_INITP_initCbT cb, void *arg);

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
****************************************************************************/

#ifdef TEST_PERF_APP
#define TEST_PERF_BUFLEN (1024 * 1024 * 4)
#define TEST_PERF_BUFS 4
#define TEST_PERF_HDR1SZ (4)
#define TEST_PERF_HDR2SZ (12)
#define TEST_PERF_HDRSZ (TEST_PERF_HDR1SZ + TEST_PERF_HDR2SZ)

#define RECV_CDE_BUFFER
#ifndef RECV_CDE_BUFFER
UINT8 static_recv_buf[TEST_PERF_BUFLEN+ TEST_PERF_HDRSZ];
#endif

CLS_COMG_ifOperations test_ops;
USB_CTRL_COMG_paramsT params;
void *test_h;

static int my_recv_func(void *bufr)
{
   unsigned char *buf = bufr;
   unsigned char mult = buf[0];
   unsigned int bsent;
   UINT32 frame_cnt = 0;
   UINT32 frame_cnt2 = (UINT32)~0;
   while(1)
   {
      UINT32 cnt = 0;
#if 0
      ////////////////////////////////////
      //single receive
      test_ops.recv(test_h, buf, (1 << mult) * 1024 + TEST_PERF_HDRSZ, &bsent);
      #ifdef RECV_CDE_BUFFER
      CMEM_cacheInv((void *)buf, sizeof(UINT32));
      #endif
      memcpy(&cnt, (void *)buf,4);
      if(cnt != frame_cnt)
      {
         printf("frame cnt error got 0x%08x expected 0x%08x\n",cnt,frame_cnt);
         frame_cnt = cnt;
      }
      frame_cnt++;
#elif 1
      ////////////////////////////////////
      //recv header first then data
      static unsigned char hdr[TEST_PERF_HDRSZ];

      test_ops.recv(test_h, hdr, TEST_PERF_HDR1SZ, &bsent);
      memcpy(buf,hdr, TEST_PERF_HDR1SZ);
      test_ops.recv(test_h, buf+TEST_PERF_HDR1SZ, (1 << mult) * 1024+TEST_PERF_HDR2SZ, &bsent);
#ifdef RECV_CDE_BUFFER
      CMEM_cacheInv((void *)buf+TEST_PERF_HDR1SZ, sizeof(UINT32));
#endif
      memcpy(&cnt, (void *)buf,4);
      if(cnt != frame_cnt)
      {
         printf("frame cnt error got 0x%08x expected 0x%08x\n",cnt,frame_cnt);
         frame_cnt = cnt;
      }
      frame_cnt++;

      memcpy(&cnt, (void *)buf+TEST_PERF_HDR1SZ,4);
      if(cnt != frame_cnt2)
      {
         printf("frame cnt2 error got 0x%08x expected 0x%08x\n",cnt,frame_cnt2);
         frame_cnt2 = cnt;
      }
      frame_cnt2--;
#endif

   }
   return 0;
}

static void run_test_perf(void)
{
   unsigned char *buf;
   unsigned char *bufr;
   static unsigned char hdr[TEST_PERF_BUFS * TEST_PERF_HDR1SZ];
   static unsigned char hdr1[TEST_PERF_BUFS * TEST_PERF_HDR2SZ];
   unsigned char idx = 0;
   unsigned int bsent;
   unsigned char mult = 1;
   UINT32 frame_cnt = 0;
   OS_LYRG_threadParams tp = {
      .func = my_recv_func,
   };
   OS_LYRG_threadHandle th;

   MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_CDE_BUFFERS_E, (void **)&buf);
   USB_CTRL_COMG_getOps(&test_ops);
   for(idx = 0; idx < TEST_PERF_BUFS; idx++)
   {
      memset(buf + TEST_PERF_BUFLEN * idx, idx, TEST_PERF_BUFLEN);
      memset(hdr + TEST_PERF_HDR1SZ * idx, idx, TEST_PERF_HDR1SZ);
      memset(hdr1 + TEST_PERF_HDR2SZ * idx, idx, TEST_PERF_HDR2SZ);
   }
#ifdef RECV_CDE_BUFFER
   bufr = buf + TEST_PERF_BUFLEN * idx;
#else
   printf("using static receive buffers\n");
   bufr = static_recv_buf;
#endif
   idx = 0;

   while(test_ops.open((void **)&test_h,&params) != USB_CTRL_COM__RET_SUCCESS)
      OS_LYRG_usleep(1000000);

   LOGG_PRINT(LOG_INFO_E, ret, "usb open success ...\n");
   test_ops.recv(test_h, &mult, 1, &bsent);
   printf("mult = %d\n", mult);
   tp.param = bufr;
   bufr[0] = mult;
   th = OS_LYRG_createThread(&tp);
   while(1)
   {
//      memset(buf, idx, TEST_PERF_BUFLEN);
//      int i;
//      for(i = 0; i < TEST_PERF_BUFLEN / 4; i++)
//         ((unsigned int *)buf)[i] = (idx << 24) + (idx << 16) + (idx << 8) + idx;
      //printf("%d - %x\n", idx, buf[TEST_PERF_BUFLEN * idx]);
      memcpy(hdr + TEST_PERF_HDR1SZ * idx,&frame_cnt,4);
      test_ops.send(test_h, hdr + TEST_PERF_HDR1SZ * idx, TEST_PERF_HDR1SZ, &bsent);
      test_ops.send(test_h, hdr1 + TEST_PERF_HDR2SZ * idx, TEST_PERF_HDR2SZ, &bsent);
      test_ops.send(test_h, buf + TEST_PERF_BUFLEN * idx, (1 << mult) * 1024, &bsent);
      idx = (idx + 1) % TEST_PERF_BUFS;
      frame_cnt++;
   }

   OS_LYRG_closeThread(th);
   test_ops.close(test_h);
}
#endif

/****************************************************************************
 ***************      G L O B A L     F U N C T I O N S       ***************
****************************************************************************/

/****************************************************************************
*
*  Function Name: APP_INITG_dispatch_init
*
*  Description: calls APP_INITP_initializeCbDispatch
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: pheripherals control
*
****************************************************************************/
ERRG_codeE APP_INITG_dispatch_init()
{
   ERRG_codeE           retCode = HW_MNGR__RET_SUCCESS;
   APP_INITP_initializeCbDispatch();

   return(retCode);
}

ERRG_codeE APP_INITP_monitorThread(void *argP)
{
   inu_deviceH deviceH = (inu_deviceH)argP;

   while(TRUE)
   {
      OS_LYRG_usleep(3000000);
      APP_INITG_statsMonitor(deviceH);
   }
}

ERRG_codeE APP_INITP_monitor(inu_deviceH deviceH)
{
   ERRG_codeE           retCode = HW_MNGR__RET_SUCCESS;
   OS_LYRG_threadParams threadParams;

   //create thread
   threadParams.func = (OS_LYRG_threadFunction)APP_INITP_monitorThread;
   threadParams.id = OS_LYRG_POWER_STATS_THREAD_ID_E;
   threadParams.event = NULL;
   threadParams.param = deviceH;

   APP_INITP_info.monitorThreadH = OS_LYRG_createThread(&threadParams);
   if (APP_INITP_info.monitorThreadH == NULL)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "failed to create powerMonitor thread\n");
      return HW_MNGR__ERR_UNEXPECTED;
   }

   return retCode;

}

/****************************************************************************
*
*  Function Name: APP_INITP_initializeCbDispatch
*
*  Description: initialize CB functions
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: interrupt manager
*
****************************************************************************/
static void APP_INITP_initializeCbDispatch(void)
{
   UINT32 i;
   APP_INITP_initCbInfoT *tblP = APP_INITP_info.initCbTbl;

   for(i = 0; i < APP_INITP_info.initCbTblIndex; i++)
   {
      APP_INITP_initCbInfoT *cbEntryP = &tblP[i];
      if(cbEntryP->cb)
         cbEntryP->cb(cbEntryP->arg);
   }
}

/****************************************************************************
*
*  Function Name: APP_INITP_setInitCbEntry
*
*  Description: set CB functions
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: interrupt manager
*
****************************************************************************/
static void APP_INITP_setInitCbEntry(APP_INITP_initCbInfoT *entryP, APP_INITP_initCbT cb, void *arg)
{
   entryP->cb  = cb;
   entryP->arg = arg;
}

#if 0
/****************************************************************************
*
*  Function Name: APP_INITP_initalizeCbTblInit
*
*  Description: set CB functions
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: interrupt manager
*
****************************************************************************/
static void APP_INITP_initalizeCbTblInit(void)
{
   unsigned int i;
   for(i = 0; i < APP_INITP_NUM_INIT_CB; i++)
   {
      APP_INITP_setInitCbEntry(&APP_INITP_info.initCbTbl[i], NULL, NULL);
   }
   APP_INITP_info.initCbTblIndex = 0;
}
#endif

/****************************************************************************
*
*  Function Name: APP_INITG_registerInitCb
*
*  Description: register CB function
*
*  Inputs: none
*
*  Outputs: none
*
*  Returns: none
*
*  Context: interrupt manager
*
****************************************************************************/
ERRG_codeE APP_INITG_registerInitCb(APP_INITG_initCbT cb, void *arg)
{
   ERRG_codeE ret = SYSTEM__RET_SUCCESS;

   //register initalization callback to be invoked during system service open

   if(!cb)
      return SYSTEM__ERR_INVALID_ARGS;

   if(APP_INITP_info.initCbTblIndex >= APP_INITP_NUM_INIT_CB)
      return SYSTEM__ERR_OUT_OF_RSRCS;

   APP_INITP_setInitCbEntry(&APP_INITP_info.initCbTbl[APP_INITP_info.initCbTblIndex], cb, arg);
   APP_INITP_info.initCbTblIndex++;

   return ret;
}

/****************************************************************************
*
*  Function Name: APP_INITP_linkCallBack
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void APP_INITP_linkCallBack(inu_deviceH deviceH, inu_device_connect_state_e state)
{
   FIX_UNUSED_PARAM_WARN(deviceH);
   //ERRG_codeE ret = INIT__RET_SUCCESS;

   if (state == INU_DEVICE__DISCONNECT)
   {
      //printf("Graceful exit. Old APP_INITP_info.closeNotification = %d\n", APP_INITP_info.closeNotification);
      //APP_INITP_info.closeNotification = 1;
      //ret = APP_INITG_close(deviceH);
      LOGG_PRINT(LOG_ERROR_E, NULL, "FW did not receive any data from Host, trigger reset\n");
      WDG_error();
   }
}


/****************************************************************************
*
*  Function Name: APP_INITG_statsMonitor
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
#define POWER_STATS             (0x1)
#define REF_STATS               (0x2)
#define POWER_OPTIMIZATION_OFF  (0x4)
#define POWER_OPTIMIZATION_ON   (0x8)
#define BATTERY_STATUS          (0x10)
#define POWER_DOWN              (0x20)
#define MULTI_GRAPH_STAT        (0x40)


extern ERRG_codeE HCA_MNGRG_showStats();
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
extern void SEQ_MNGRG_printSchedulerDebugBuf();
#endif

void APP_INITG_statsMonitor( inu_deviceH deviceH )
{
   UINT32 *virtP;
   MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_STATS_E,(MEM_MAPG_addrT*)&virtP);
   if ( (*virtP & POWER_STATS) == POWER_STATS )
   {
      HCG_MNGRG_showStats();
      HCA_MNGRG_showStats();
   }
   if ( (*virtP & POWER_OPTIMIZATION_OFF) == POWER_OPTIMIZATION_OFF )
   {
      *virtP &= ~POWER_OPTIMIZATION_OFF;
      HCG_MNGRG_voteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   }
   if ( (*virtP & POWER_OPTIMIZATION_ON) == POWER_OPTIMIZATION_ON )
   {
      *virtP &= ~POWER_OPTIMIZATION_ON;
      HCG_MNGRG_devoteUnits(HCG_MNGRG_HW_IDVE_UNITS);
   }
   if ( (*virtP & BATTERY_STATUS) == BATTERY_STATUS )
   {
      UINT8 batteryLevel;
      CHARGERG_batteryStatus(&batteryLevel);
      printf("batteryLevel = %d\n",batteryLevel);
   }
   if ( (*virtP & REF_STATS) == REF_STATS )
   {
      inu_device__showStats(deviceH);
      *virtP &= ~REF_STATS;
   }
   if ( (*virtP & POWER_DOWN) == POWER_DOWN )
   {
      printf("power down\n");
      CHARGERG_enterPowerDown(5);
   }
   if ( (*virtP & MULTI_GRAPH_STAT) == MULTI_GRAPH_STAT )
   {
#ifdef SCHEDULER_DEBUG_PRINT_TO_BUF
      SEQ_MNGRG_printSchedulerDebugBuf();
#endif
   }
}


ERRG_codeE helsinki_ev72_node_init(void);
/****************************************************************************
*
*  Function Name: APP_INITG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE APP_INITG_init(int      wdTimeout,
                              UINT32   clientDdrBufSize,
                              UINT32   *clientDdrBufPhyAddrP,
                              UINT32   *clientDdrBufVirtAddrP,
                              inu_device_standalone_mode_e standAloneMode,
                              inu_deviceH *devicePtr,
                              UINT32 interface_select,
                              UINT32 port)
{
   ERRG_codeE ret = INIT__RET_SUCCESS;

   FIX_UNUSED_PARAM_WARN(clientDdrBufSize);
   if (clientDdrBufPhyAddrP)
      *clientDdrBufPhyAddrP = 0;
   if (clientDdrBufVirtAddrP)
      *clientDdrBufVirtAddrP = 0;

   if(ERRG_SUCCEEDED(ret))
   {
#ifdef ENABLE_TRACING
      // when tracing is enabled
      char buff[100];
      int rc;

      // initialize trace marker file
      rc = trace_marker_init();
      if (rc != 0)
	  {
          fprintf(stderr, "Failed to open trace marker file: %s\n", strerror(rc));
      }
      // set the main PID as the filter for tracing
      pid_t tid = syscall(SYS_gettid);
      sprintf(buff, "echo %d > /tracing/set_ftrace_pid", tid);
      system(buff);
      sprintf(buff, "echo %d > /tracing/set_event_pid", tid);
      system(buff);
#endif
      LOGG_PRINT(LOG_DEBUG_E, NULL, "initialize new api\n");
      
      //ret = helsinki_ev72_node_init();
      //if(ERRG_SUCCEEDED(ret))
      //{
      //   LOGG_PRINT(LOG_INFO_E, NULL, "EV72 Node init success.\n");
      //}
      //else
      //{
      //   LOGG_PRINT(LOG_INFO_E, NULL, "EV72 Node init failed.\n");
      //}

      inu_init(FALSE);
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //create device
      inu_deviceH device;
      inu_device__initParams deviceInitParams;
      //register handler on device to monitor device connection
      deviceInitParams.eventCb = APP_INITP_linkCallBack;
      deviceInitParams.standAloneMode = standAloneMode;
      //tcp parameters
      deviceInitParams.interface_0.select = (inu_device_interface_0_e)interface_select;
      deviceInitParams.interface_0.interfaceParams.tcp.port = port;
      deviceInitParams.useSharedMemory = 0;
      //strcpy(deviceInitParams.interface_0.interfaceParams.tcp.hostIp, ipHost);
      //initialize device
      if (deviceInitParams.interface_0.select == INU_DEVICE_INTERFACE_0_TCP)
      {
         strcpy(deviceInitParams.interface_0.interfaceParams.tcp.hostIp, "192.168.10.6");
         strcpy(deviceInitParams.interface_0.interfaceParams.tcp.targetIp, "192.168.10.5");
         deviceInitParams.interface_0.interfaceParams.tcp.port = port;

      }
      ret = inu_device__new(&device, &deviceInitParams);
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "init device failed\n",ret);
      }
      else
      {
          *devicePtr = device;
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      ret = APP_INITP_monitor(*devicePtr);
   }

   return(ret);
}

/****************************************************************************
*
*  Function Name: APP_INITG_close
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE APP_INITG_close(inu_deviceH deviceH)
{
   ERRG_codeE ret;

   ret = OS_LYRG_closeThread(APP_INITP_info.monitorThreadH);
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "close monitor thread failed\n");
   }

   inu_device__delete(deviceH);

   inu_deinit(FALSE);

   return(ret);
}

#ifdef __cplusplus
}
#endif

