/****************************************************************************
 * 
 *   FileName: pmm_mngr.c
 *
 *   Author: Eli G.
 *
 *   Date: 
 *
 *   Description: DSP Power Manager
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "assert.h"

#include "inu_types.h"
#include "err_defs.h"

#include "icc_common.h"
#include "icc.h"
#include "data_base.h"
#include "inu2.h"
#include "log.h"

/************************ DSP sleep ******************************/

typedef ERRG_codeE (*pmm_mngr__dspSleepCbT)( void );
typedef ERRG_codeE (*pmm_mngr__dspWakeupCbT)( void );

static int                      pmm_mngr__dspUsage[ICCG_CMD_TARGET_MAX_NUM] = { 0, 0 };
//static pmm_mngr__dspSleepCbT  pmm_mngr__dspSleepCbTbl[ICCG_CMD_TARGET_MAX_NUM] = { CEVA_BOOTG_sleep,  NULL };ELAD4100
//static pmm_mngr__dspWakeupCbT pmm_mngr__dspWakeupCbTbl[ICCG_CMD_TARGET_MAX_NUM] = { CEVA_BOOTG_wakeup, NULL };ELAD4100

static OS_LYRG_mutexT dspUsageMutex[ICCG_CMD_TARGET_MAX_NUM];
static int dspMutexIsAquired[ICCG_CMD_TARGET_MAX_NUM] = { 0 , 0 };


ERRG_codeE pmm_mngr__dspPmmActivate(UINT32 dspTarget)
{
   ERRG_codeE ret = INU_FUNCTION__RET_SUCCESS;

   if(!dspMutexIsAquired[dspTarget])
   {
      OS_LYRG_aquireMutex(&dspUsageMutex[dspTarget]);
      dspMutexIsAquired[dspTarget] = TRUE;
   }

   OS_LYRG_lockMutex(&dspUsageMutex[dspTarget]);
   if (pmm_mngr__dspUsage[dspTarget] == 0)
   {
      //if (pmm_mngr__dspWakeupCbTbl[dspTarget] != NULL)ELAD4100
      {
         //ret = pmm_mngr__dspWakeupCbTbl[dspTarget]();ELAD4100
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "failed to wake up dsp %d \n",dspTarget);
         }
      }
   }      
   pmm_mngr__dspUsage[dspTarget]++;
   OS_LYRG_unlockMutex(&dspUsageMutex[dspTarget]);
   return ret;
}

ERRG_codeE pmm_mngr__dspPmmDeactivate(UINT32 dspTarget)
{
   ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;

   if(!dspMutexIsAquired[dspTarget])
   {
      assert(0);
   }
   

   OS_LYRG_lockMutex(&dspUsageMutex[dspTarget]);   
   pmm_mngr__dspUsage[dspTarget]--;
   if (pmm_mngr__dspUsage[dspTarget] == 0)
   {
      //if (pmm_mngr__dspSleepCbTbl[dspTarget] != NULL)ELAD4100
      {
         //ret = pmm_mngr__dspSleepCbTbl[dspTarget]();ELAD4100
         if (ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "failed to put dsp %d to sleep\n",dspTarget);
         }
      }
   }
   OS_LYRG_unlockMutex(&dspUsageMutex[dspTarget]);
   return ret;
}
