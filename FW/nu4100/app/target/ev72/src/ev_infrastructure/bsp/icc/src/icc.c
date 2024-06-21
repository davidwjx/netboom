/****************************************************************************
 *
 *   FileName: icc.c
 *
 *   Author:  Noam P. & Arnon C.
 *
 *   Date: 
 *
 *   Description: 
 *   
 ****************************************************************************/
#include "inu_common.h"
#include "data_base.h"
#include "ictl_drv.h"
#include "icc.h"
#include "sched.h"

/****************************************************************************
***************     L O C A L        D E F I N I T I O N S    ***************
****************************************************************************/
#define ICC_DEBUG             (0)

/****************************************************************************
***************     L O C A L          D A T A         ***************
****************************************************************************/
static const UINT32           ICCP_mapTblGpToEvBoxDbName[ICCG_CMD_TARGET_MAX_NUM] = {DATA_BASEG_ICC_BOX_GP_DSPA, DATA_BASEG_ICC_BOX_GP_DSPB};
const UINT32                  ICCP_mapTblEvToGpBoxDbName[ICCG_CMD_TARGET_MAX_NUM] = {DATA_BASEG_ICC_BOX_DSPA_GP, DATA_BASEG_ICC_BOX_DSPB_GP};

static const UINT32           gpIctlForceAddress  = 0x08170010;
static const UINT32           gpIctlStatusAddress = 0x08170020;

static       ICCP_msgStateT   ICCP_msgState = ICCP_IDLE; 
static       OS_LYRG_mutexCpT *ICCP_mutexP[3];

/****************************************************************************
***************     G L O B A L          D A T A         ***************
****************************************************************************/


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
 
#define  NOAM_DEBUG3                   0x1003D30
static void ICC_DRVP_handleIsr( int dspTarget );
void  ICC_DRVP_evIsr(void);
void  ICC_DRVP_dbgPrint(void);


/****************************************************************************
***************     L O C A L          F U N C T I O N S        ***************
****************************************************************************/
static void ICC_DRVP_handleIsr( int dspTarget )
{
   DATA_BASEG_iccBoxDataBaseT  iccBoxGpToEv;
   volatile INT32  pollingCounter = -1;
#if (ICC_DEBUG == 1)
   DATA_BASEG_iccJobsDescriptorDataBaseT jobsDb;
   TIMERSG_timeT timer;
   TIMERSG_getTicks(&timer);
#endif

   DATA_BASEG_readDataBaseNoMutex((UINT8*)(&iccBoxGpToEv), ICCP_mapTblGpToEvBoxDbName[dspTarget]);

   if ( (iccBoxGpToEv.box & ICC_COMMONG_iccCmdReqAckMask) == ICC_COMMONG_iccCmdReqAckMask )
   {
      //FIXME : this should be done inside the ICTL driver, but there is an issue when moving it to the ICTL file which causes tests to fail 
      while(((*(volatile UINT32 *)gpIctlStatusAddress & (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM)) == (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM)) && (pollingCounter < MAX_NUM_POLL_CYCLES))
      {
         //wait for GP to finish handling previous ISR if there is
         pollingCounter++;
      }

      //here we push the job descriptor into the queue for the scheduler
      SCHEDG_pushNewJob((UINT32)(iccBoxGpToEv.box & ICC_COMMONG_JOB_DESCRIPTOR_MASK));
      
      //send ACK for CMD     
      iccBoxGpToEv.box &= (~ICC_COMMONG_iccCmdReqAckMask);            
      DATA_BASEG_writeDataBaseNoMutex((UINT8*)(&iccBoxGpToEv), ICCP_mapTblEvToGpBoxDbName[dspTarget]);      

#if (ICC_DEBUG == 1)
      DATA_BASEG_readDataBaseNoMutex((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS); 
      LOGG_PRINT(LOG_DEBUG_E, NULL, "c i:d=%d,b=%x,d=%d,p=%d,cy=%u\n",dspTarget,iccBoxGpToEv.box,jobsDb.jobDescriptorPool[iccBoxGpToEv.box & 0xFFFF].data,pollingCounter,timer.ticks);   
#endif

      //Interrupts the GP
      OS_LYRG_lockMutexCp(ICCP_mutexP[0]);
      *(volatile UINT32 *)gpIctlForceAddress |= (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM);
      OS_LYRG_unlockMutexCp(ICCP_mutexP[0]);
            
#if (ICC_DEBUG == 1)
      //read result, if not set correctly print error!
      if ((*(volatile UINT32 *)gpIctlStatusAddress & (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM)) != (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM))
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "c i e:i=%x\n",*(volatile UINT32 *)gpIctlStatusAddress);   
      }
#endif      
    //Singal the scheduler
      evIntCondSignal(&SCHEDG_Condition);
      evPreempt(); 
   }
   else
   {
      //Received ACK for MSG.
      //TODO: here the scheduler needs to enable the sending of remaining messages
      ICCP_msgState = ICCP_IDLE;
#if (ICC_DEBUG == 1)
      DATA_BASEG_readDataBaseNoMutex((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS); 
      LOGG_PRINT(LOG_DEBUG_E, NULL, "m i:d=%d,j=%d,d=%d,cy=%u\n",dspTarget,iccBoxGpToEv.box,jobsDb.jobDescriptorPool[iccBoxGpToEv.box & 0xFFFF].data,timer.ticks);
#endif      
   }
}


/****************************************************************************
***************     G L O B A L        F U N C T I O N S        ***************
****************************************************************************/
INT32 ICC_DRVG_sendMsg( ICC_COMMONG_dspTargetT dspTarget , UINT16 jobDescriptor )
{
   DATA_BASEG_iccBoxDataBaseT  iccBoxEvToGp;
   volatile INT32  pollingCounter = -1;
#if (ICC_DEBUG == 1)
   DATA_BASEG_iccJobsDescriptorDataBaseT jobsDb;
   TIMERSG_timeT timer;    
#endif
   INT32 ret = 0;
  

   //FIXME : this should be done inside the ICTL driver, but there is an issue when moving it to the ICTL file which causes tests to fail
   while(((*(volatile UINT32 *)gpIctlStatusAddress & (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM)) == (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM)) && (pollingCounter < MAX_NUM_POLL_CYCLES))
   {
      //wait for GP to finish handling previous ISR if there is
      pollingCounter++;
   }
   ret = pollingCounter;

   //prepare the message, copy the descriptor and add the message bit
   iccBoxEvToGp.box = (UINT32)jobDescriptor | (ICC_COMMONG_iccCmdReqAckMask);

   DATA_BASEG_writeDataBaseNoMutex((UINT8*)(&iccBoxEvToGp), ICCP_mapTblEvToGpBoxDbName[dspTarget]);  

#if (ICC_DEBUG == 1)    
   DATA_BASEG_readDataBaseNoMutex((UINT8*)(&jobsDb), DATA_BASEG_JOB_DESCRIPTORS); 
   TIMERSG_getTicks(&timer); 
   LOGG_PRINT(LOG_DEBUG_E, NULL, "m:d=%d,j=%d,p=%d,d=%d,a=%x,cy=%u\n",dspTarget,(iccBoxEvToGp.box&0xFFFF),pollingCounter,jobsDb.jobDescriptorPool[iccBoxEvToGp.box & 0xFFFF].data,jobsDb.jobDescriptorPool[iccBoxEvToGp.box & 0xFFFF].arg,timer.ticks); 
#endif

   ICCP_msgState = ICCP_WAIT_FOR_ACK;

   //Interrupt the GP      
   OS_LYRG_lockMutexCp(ICCP_mutexP[0]);
   *(volatile UINT32 *)gpIctlForceAddress |= (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM);
   OS_LYRG_unlockMutexCp(ICCP_mutexP[0]); 

#if (ICC_DEBUG == 1) 
   //read result, if not set correctly print error!
   if (*(volatile UINT32 *)gpIctlStatusAddress & (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM) != (1 << ICC_COMMONG_ICTL0_DSPB_INT_NUM))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "m e:i=%x\n",*(volatile UINT32 *)gpIctlStatusAddress);   
   }   
#endif

   return ret;
}




void ICC_DRVP_evIsr(void)
{
#if (ICC_DEBUG == 1)
   UINT32 regVal;
   TIMERSG_timeT timer;

   TIMERSG_getTicks(&timer);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "i s:cy=%u\n",timer.ticks);
#endif

   ICC_DRVP_handleIsr(ICCG_CMD_TARGET_DSPB);

   OS_LYRG_lockMutexCp(ICCP_mutexP[2]);
#if (ICC_DEBUG == 1)
   regVal = ICTL_DRVG_clearForceInt(1);
#else
   ICTL_DRVG_clearForceInt(1);
#endif
   OS_LYRG_unlockMutexCp(ICCP_mutexP[2]);

#if (ICC_DEBUG == 1)
   TIMERSG_getTicks(&timer);
   LOGG_PRINT(LOG_DEBUG_E, NULL, "i e:%x, cy=%u\n",regVal,timer.ticks);
#endif    
}


void ICC_DRVP_dbgPrint(void)
{
   static int counter = 0;
       volatile uint32_t *debug3  = (volatile uint32_t *)NOAM_DEBUG3;
       counter++;
       *debug3++  = 0xcafec0fe;
       *debug3  = counter;
       
       LOGG_PRINT(LOG_INFO_E, NULL, "DSP alive!\n");
   ICTL_DRVG_clearForceInt(2);
}

INT32 ICC_DRVG_allowedToSendMsg( void )
{
   return (ICCP_msgState == ICCP_IDLE ? 1 : 0);
}

void ICC_DRVG_init()
{
   ICTL_DRVG_isrT    registerParams;   
   int retVal,i;
   
   //init ictl mutex's
   for (i = 0; i < 3; i++)
   {
      retVal = OS_LYRG_aquireMutexCp(&ICCP_mutexP[i], OS_LYRG_CP_MUTEX_ICC_ICTL_GP_E + i);  

      if ( retVal == FAIL_E )
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,  "OS_LYRG_aquireMutexCp for ICC %d failed !!!\n", OS_LYRG_CP_MUTEX_ICC_ICTL_GP_E + i);
      }
   } 

   //Configure DSP ICTL to enable SW interrupt
   ICTL_DRVG_initIctl();
   registerParams.intNum = 1;
   registerParams.priority = ICTLG_PRIO_0_E;
   registerParams.isr = ICC_DRVP_evIsr;
   ICTL_DRVG_registerIsr(&registerParams);

   //Register the timer line for debug
   registerParams.intNum   = 2;
   registerParams.priority = ICTLG_PRIO_1_E;

   registerParams.isr = ICC_DRVP_dbgPrint;
   ICTL_DRVG_registerIsr(&registerParams);
   
}

