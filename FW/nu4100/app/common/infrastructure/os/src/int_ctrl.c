/****************************************************************************
 * 
 *   FileName: int_ctrl.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: nu3000 interrupt controller for linux user-space 
 *
 *****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"

#if DEFSG_IS_GP
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
#include <errno.h>
#include <string.h>
#include "log.h"
#include "os_lyr.h"
#include <sys/epoll.h> 
#include <assert.h>
/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define INT_CTRLP_UIO_DEV_CDE_1       "/dev/uio0"
#define INT_CTRLP_UIO_DEV_CDE_2       "/dev/uio1"
#define INT_CTRLP_UIO_DEV_CDE_3       "/dev/uio2"
#define INT_CTRLP_UIO_DEV_CDE_4       "/dev/uio3"
#define INT_CTRLP_UIO_DEV_CDE_5       "/dev/uio4"
#define INT_CTRLP_UIO_DEV_CDE_6       "/dev/uio5"
#define INT_CTRLP_UIO_DEV_CDE_ABORT_0 "/dev/uio6"
#define INT_CTRLP_UIO_DEV_CDE_ABORT_1 "/dev/uio7"
#define INT_CTRLP_UIO_DEV_CDE_ABORT_2 "/dev/uio8"
#define INT_CTRLP_UIO_DEV_DPE         "/dev/uio9"
#define INT_CTRLP_UIO_DEV_IAE         "/dev/uio10"
#define INT_CTRLP_UIO_DEV_PPE         "/dev/uio11"
#define INT_CTRLP_UIO_DEV_DSPA         "/dev/uio12"
#define INT_CTRLP_UIO_DEV_DSPB         "/dev/uio13"
#define INT_CTRLP_UIO_DEV_STROBE1     "/dev/uio14"
#define INT_CTRLP_UIO_DEV_STROBE2     "/dev/uio15"
#define INT_CTRLP_UIO_DEV_STROBE3     "/dev/uio16"
#define INT_CTRLP_UIO_DEV_STROBE4     "/dev/uio17"
#define INT_CTRLP_UIO_DEV_STROBE5     "/dev/uio18"
#define INT_CTRLP_UIO_DEV_STROBE6     "/dev/uio19"
#define INT_CTRLP_UIO_DEV_PR_SHUTTER  "/dev/uio20"

//HW interrupt numbers
#define INT_CTRLP_IAE_HW_NUM            (20)
#define INT_CTRLP_DPE_HW_NUM            (21)
#define INT_CTRLP_PPE_HW_NUM            (22)
#define INT_CTRLP_CDE_HW_NUM            (9)
#define INT_CTRLP_DSPA_HW_NUM           (0)
#define INT_CTRLP_DSPB_HW_NUM           (1)
#define INT_CTRLP_GPIO_HW_NUM           (14)
#define INT_CTRLP_I2S_HW_NUM            (15)
#define INT_CTRLP_I2C0_HW_NUM           (16)
#define INT_CTRLP_I2C1_HW_NUM           (17)
#define INT_CTRLP_I2C2_HW_NUM           (18)
#define INT_CTRLP_I2C3_HW_NUM           (19)

//#define INT_CTRLP_LATENCY_MEASURE
#ifdef INT_CTRLP_LATENCY_MEASURE
#define INT_CTRLP_WINDOW_SIZE  (600)
#define INT_CTRLP_LATENCY_TOTAL_ENTRY (0)
#define INT_CTRLP_LATENCY_WINDOW_ENTRY (1)
#define INT_CTRLP_LATENCY_NUM_ENTRIES  (2)
#endif
#define MAX_EVENTS OS_LYRG_NUM_INTS

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/ 
#ifdef INT_CTRLP_LATENCY_MEASURE
typedef struct
{
   UINT64 maxNs;
   UINT64 minNs;
   UINT32 winCnt;
   UINT32 winSize;
} INT_CTRLP_latencyStatT;
#endif
 
typedef struct
{
   int uioFd;
   const char *uioName;
   unsigned int hwNum;
   OS_LYRG_intHandlerT handler;

   UINT32 cnt;
#ifdef INT_CTRLP_LATENCY_MEASURE
   INT_CTRLP_latencyStatT latencyTbl[INT_CTRLP_LATENCY_NUM_ENTRIES];
#endif
   void *argP;
} INT_CTRLP_intEntryT;
/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
#ifdef INT_CTRLP_LATENCY_MEASURE
static void INT_CTRLP_initLatencies(INT_CTRLP_latencyStatT *tblP);
static inline void INT_CTRLP_updateLatencies(INT_CTRLP_latencyStatT *tblP, UINT64 *interruptTsP);
#endif

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static INT_CTRLP_intEntryT INT_CTRLP_interruptTbl[OS_LYRG_NUM_INTS];
static OS_LYRG_threadHandle INT_CTRLP_thrdH = NULL; 
static int INT_CTRLP_maxFd = 0;
static int epoll_fd = 0;
/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
 static void INIT_CTRLP_initEntry(INT_CTRLP_intEntryT *entryP, const char *uioName, unsigned int hwNum)
 {
   entryP->uioName = uioName;
   entryP->hwNum = hwNum;
   entryP->handler = NULL;
   entryP->uioFd = (-1);
   entryP->cnt = 0;
#ifdef INT_CTRLP_LATENCY_MEASURE
   INT_CTRLP_initLatencies(entryP->latencyTbl);
#endif
   
}
static void INT_CTRLP_fillTbl(void)
{
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_IAE_E],           INT_CTRLP_UIO_DEV_IAE,           INT_CTRLP_IAE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_DPE_E],           INT_CTRLP_UIO_DEV_DPE,           INT_CTRLP_DPE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_PPE_E],           INT_CTRLP_UIO_DEV_PPE,           INT_CTRLP_PPE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_1_E],         INT_CTRLP_UIO_DEV_CDE_1,         INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_2_E],         INT_CTRLP_UIO_DEV_CDE_2,         INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_3_E],         INT_CTRLP_UIO_DEV_CDE_3,         INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_4_E],         INT_CTRLP_UIO_DEV_CDE_4,         INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_5_E],         INT_CTRLP_UIO_DEV_CDE_5,         INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_6_E],         INT_CTRLP_UIO_DEV_CDE_6,         INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_ABORT_0_E],   INT_CTRLP_UIO_DEV_CDE_ABORT_0,   INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_ABORT_1_E],   INT_CTRLP_UIO_DEV_CDE_ABORT_1,   INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_CDE_ABORT_2_E],   INT_CTRLP_UIO_DEV_CDE_ABORT_2,   INT_CTRLP_CDE_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_STROBE1_E],       INT_CTRLP_UIO_DEV_STROBE1,       INT_CTRLP_GPIO_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_STROBE2_E],       INT_CTRLP_UIO_DEV_STROBE2,       INT_CTRLP_GPIO_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_STROBE3_E],       INT_CTRLP_UIO_DEV_STROBE3,       INT_CTRLP_GPIO_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_STROBE4_E],       INT_CTRLP_UIO_DEV_STROBE4,       INT_CTRLP_GPIO_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_STROBE5_E],       INT_CTRLP_UIO_DEV_STROBE5,       INT_CTRLP_GPIO_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_STROBE6_E],       INT_CTRLP_UIO_DEV_STROBE6,       INT_CTRLP_GPIO_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_DSPA_E],          INT_CTRLP_UIO_DEV_DSPA,          INT_CTRLP_DSPA_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_DSPB_E],          INT_CTRLP_UIO_DEV_DSPB,          INT_CTRLP_DSPB_HW_NUM);
   INIT_CTRLP_initEntry(&INT_CTRLP_interruptTbl[OS_LYRG_INT_PR_SHUTTER_E],    INT_CTRLP_UIO_DEV_PR_SHUTTER,    INT_CTRLP_GPIO_HW_NUM);
}

static void INT_CTRLP_showTbl()
{
   int i;
   INT_CTRLP_intEntryT *entryP;
   
   for(i = 0; i < OS_LYRG_NUM_INTS; i++)
   {
      entryP = &INT_CTRLP_interruptTbl[i];
      LOGG_PRINT(LOG_DEBUG_E, NULL, "int_ctrl config: cnt=%d name=%s num=%d fd=%d handler=%p\n", entryP->cnt, entryP->uioName, entryP->hwNum, entryP->uioFd, entryP->handler);
   }
   LOGG_PRINT(LOG_DEBUG_E, NULL, "max fd = %d\n", INT_CTRLP_maxFd);
}

static int INT_CTRP_OpenEpoll(int size)
{
   int fd =   epoll_create(size);
   if(fd == -1)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to create Epoll File descriptor \n");
   }
   return fd;
}
/**
 * @brief Add a file descriptor to an Epoll handle
 * 
 *
 * @param epoll_fd Epoll file descriptor
 * @param event Event handle
 * @return Returns an error code
 */
static int INT_CTRP_AddToEpoll(int epoll_fd, struct epoll_event *event, int fd)
{
   int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, event);
   if(ret)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to add file descriptor to the Epoll handle %s \n", strerror(errno));
   }
   return ret;
}

static ERRG_codeE INT_CTRLP_openAll(void)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   int i = 0;
   INT_CTRLP_intEntryT *entryP;
   UINT32 irqOff = 0;

   INT_CTRLP_maxFd = 0;

   epoll_fd = INT_CTRP_OpenEpoll(OS_LYRG_NUM_INTS);
   if(epoll_fd == -1)
   {
      return OS_LYR__ERR_UNEXPECTED;
      
   }
   for(i = 0; i < OS_LYRG_NUM_INTS; i++)
   {
      entryP = &INT_CTRLP_interruptTbl[i];
      entryP->cnt = 0;
      
      entryP->uioFd = open(entryP->uioName, O_RDWR);
      if(entryP->uioFd >= 0)
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"Opening %s for index %lu \n",entryP->uioName, i  );
         struct epoll_event event;
         event.events = EPOLLIN;
         event.data.ptr = (void *) entryP;  /*Pointer in the interrupt table*/
         INT_CTRP_AddToEpoll(epoll_fd,&event, entryP->uioFd);
         INT_CTRLP_maxFd   = (entryP->uioFd > INT_CTRLP_maxFd) ? entryP->uioFd : INT_CTRLP_maxFd ;
         //The kernel sets interrupts to enabled/unmasked (during probe- before the open) so we disable here.
         write(entryP->uioFd, &irqOff, sizeof(irqOff));
      }
//      else
//      {
//        ret = OS_LYR__ERR_UNEXPECTED;
//      }
   }

   INT_CTRLP_showTbl();

   return ret;
}


static void INT_CTRLP_setFds(fd_set *rfdsP)
{
   int i;
   INT_CTRLP_intEntryT *entryP;
   
   FD_ZERO(rfdsP);
   for(i = 0; i < OS_LYRG_NUM_INTS; i++)
   {
      entryP = &INT_CTRLP_interruptTbl[i];
      if (entryP->uioFd >= 0)
      {
         FD_SET(entryP->uioFd, rfdsP);
      }
   }
}

static int INT_CTRLP_thread(void *argP)
{
   enum {WAKE_SEC_E = 1};
   UINT32               irqOn = 1;
   INT_CTRLP_intEntryT  *entryP;
   fd_set               rfds;
   struct timespec      ts;
   int                  numSet;
   int                  i;
   int                  ret = OS_LYR__ERR_UNEXPECTED;

   FIX_UNUSED_PARAM_WARN(argP);
   struct epoll_event  events[MAX_EVENTS];

   while(1)
   {
      ts.tv_sec = WAKE_SEC_E ;
      ts.tv_nsec = 0;

      numSet = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
      if(numSet < 0)
      {
         LOGG_PRINT(LOG_ERROR_E,0, "Interrupt controller: select error: %s\n",strerror(errno));
         break;
      }
      else if (numSet > 0)
      {
         for(i = 0; i < numSet; i++)
         {
            entryP = (INT_CTRLP_intEntryT  *)events[i].data.ptr;
            if ((events[i].events & EPOLLIN))
            {
               LOGG_PRINT(LOG_DEBUG_E,NULL,"Event index %lu, Event file descriptor %d, Interrupt table FD %d \n",index,entryP->uioFd);
               char buf[16];
               UINT64 timeStamp, counter;

               //Note: Need to clear out fd with read of 4 bytes at least
               ret = read(entryP->uioFd,buf, sizeof(timeStamp)*2);
               memcpy(&timeStamp,&buf[0],sizeof(timeStamp));
               memcpy(&counter,&buf[8],sizeof(counter));
#ifdef INT_CTRLP_LATENCY_MEASURE
               if(ret >= 0)
                  INT_CTRLP_updateLatencies(entryP->latencyTbl, &timeStamp);
#endif
               if(entryP->handler)
                  entryP->handler(timeStamp,counter,entryP->argP);
               entryP->cnt++;

               //IRQ disabled by kernel so need to reenable it after handler is called.
               write(entryP->uioFd, &irqOn, sizeof(irqOn));

            }
         }
      }
   }

   return ret;
}

static ERRG_codeE INT_CTRLP_setOn(OS_LYRG_intNumE intNum, UINT32 irqOn)
{
   ssize_t retval;
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;

   retval = write(INT_CTRLP_interruptTbl[intNum].uioFd, &irqOn, sizeof(irqOn));
   if((retval < 0) || retval != sizeof(irqOn))
   {
      ret = OS_LYR__ERR_UNEXPECTED;
   }

   return ret;
}

#ifdef INT_CTRLP_LATENCY_MEASURE
static void INT_CTRLP_initLatencyStat(INT_CTRLP_latencyStatT *latencyP, INT32 winSize)
{
   latencyP->maxNs = 0;
   latencyP->minNs = ~0U;
   latencyP->winCnt = 0;
   latencyP->winSize = winSize;
}

static void INT_CTRLP_initLatencies(INT_CTRLP_latencyStatT *tblP)
{
   INT_CTRLP_initLatencyStat(&tblP[INT_CTRLP_LATENCY_TOTAL_ENTRY], -1);
   INT_CTRLP_initLatencyStat(&tblP[INT_CTRLP_LATENCY_WINDOW_ENTRY], INT_CTRLP_WINDOW_SIZE);
}

static inline void INT_CTRLP_updateLatency(INT_CTRLP_latencyStatT *latencyP, UINT64 diffNsec)
{
   latencyP->winCnt++;

   if((latencyP->winSize >= 0) && (latencyP->winCnt >= (UINT32)latencyP->winSize))
      INT_CTRLP_initLatencyStat(latencyP, latencyP->winSize);

   if(diffNsec > latencyP->maxNs) 
   {
      //LOGG_PRINT(LOG_DEBUG_E, 0, "interrupt ctrl: max latency (win=%d)%llu->%llu\n", latencyP->winSize, latencyP->maxNs, diffNsec);
      latencyP->maxNs = diffNsec;
   }
   if(diffNsec < latencyP->minNs)
   {
      //LOGG_PRINT(LOG_DEBUG_E, 0, "interrupt ctrl: min latency (win=%d)%llu->%llu\n", latencyP->winSize,latencyP->minNs, diffNsec);
      latencyP->minNs = diffNsec;
   }
}

static inline void INT_CTRLP_updateLatencies(INT_CTRLP_latencyStatT *tblP, UINT64 *interruptTsP)
{
   UINT64 currentTs;
   UINT64 diffNs;
   
   OS_LYRG_getTimeNsec(&currentTs);
   diffNs = currentTs - *interruptTsP;

   INT_CTRLP_updateLatency(&tblP[INT_CTRLP_LATENCY_WINDOW_ENTRY],diffNs);
   INT_CTRLP_updateLatency(&tblP[INT_CTRLP_LATENCY_TOTAL_ENTRY],diffNs);

}
#endif

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE OS_LYRG_intCtrlInit(void)
{
   ERRG_codeE ret = OS_LYR__RET_SUCCESS;
   OS_LYRG_threadParams thrParams;

   INT_CTRLP_fillTbl();

   //Open interrupts 
   ret = INT_CTRLP_openAll();

   if(ERRG_SUCCEEDED(ret))
   {
      //Launch interrupt handling thread 
      thrParams.func    = INT_CTRLP_thread;
      thrParams.id      = OS_LYRG_INT_CTRL_THREAD_ID_E;
      thrParams.event   = NULL;
      thrParams.param   = NULL;
      INT_CTRLP_thrdH   = OS_LYRG_createThread(&thrParams);
      if(!INT_CTRLP_thrdH)
      {
         ret = OS_LYR__ERR_UNEXPECTED;
      }
   }

   return ret;
}

ERRG_codeE OS_LYRG_intCtrlRegister(OS_LYRG_intNumE intNum, OS_LYRG_intHandlerT isr, void *argP)
{
   INT_CTRLP_interruptTbl[intNum].handler = isr;
   INT_CTRLP_interruptTbl[intNum].argP    = argP;
   return OS_LYR__RET_SUCCESS;
}

ERRG_codeE OS_LYRG_intCtrlUnregister(OS_LYRG_intNumE intNum)
{
   //Do this after interrupts are disabled
   INT_CTRLP_interruptTbl[intNum].handler = NULL;
   INT_CTRLP_interruptTbl[intNum].argP    = NULL;
   return OS_LYR__RET_SUCCESS;
}

ERRG_codeE OS_LYRG_intCtrlEnable(OS_LYRG_intNumE intNum)
{
   return INT_CTRLP_setOn(intNum,1);
}

ERRG_codeE OS_LYRG_intCtrlDisable(OS_LYRG_intNumE intNum)
{
   return INT_CTRLP_setOn(intNum,0);
}

void OS_LYRG_intCtrlStats(void)
{
   int i;
   INT_CTRLP_intEntryT *entryP;

   LOGG_PRINT(LOG_INFO_E, 0,"Interrupt statistics:\n");
   LOGG_PRINT(LOG_INFO_E, 0,"-------------------\n");
   for(i = 0; i < OS_LYRG_NUM_INTS; i++)
   {
      entryP = &INT_CTRLP_interruptTbl[i];
      LOGG_PRINT(LOG_INFO_E, 0, "[%d]%s fd=%d isr=%p cnt=%d\n",entryP->hwNum, entryP->uioName, entryP->uioFd, entryP->handler, entryP->cnt);
#ifdef INT_CTRLP_LATENCY_MEASURE 
      LOGG_PRINT(LOG_INFO_E, 0, "kernel-user latency: win_size %d:[%llu,%llu] total:[%llu,%llu]\n",
         entryP->latencyTbl[INT_CTRLP_LATENCY_WINDOW_ENTRY].winSize,
         entryP->latencyTbl[INT_CTRLP_LATENCY_WINDOW_ENTRY].maxNs, entryP->latencyTbl[INT_CTRLP_LATENCY_WINDOW_ENTRY].minNs, 
         entryP->latencyTbl[INT_CTRLP_LATENCY_TOTAL_ENTRY].maxNs, entryP->latencyTbl[INT_CTRLP_LATENCY_TOTAL_ENTRY].minNs);
#endif
   }
}

#endif //DEFSG_IS_GP_LINUX
