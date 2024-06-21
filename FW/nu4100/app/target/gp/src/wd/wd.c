/****************************************************************************
 *
 *   FileName: wd.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: watchdog implementation
 *
 ****************************************************************************/
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_types.h"
#include "err_defs.h"

#if DEFSG_IS_GP
#include "os_lyr.h"
#include "log.h"

#include <unistd.h>
#include "stdops.h"
#include <linux/input.h>
#include <linux/watchdog.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "wd.h"

extern int inu_imu__deInitSensor();

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define                               WD_FD    "/dev/watchdog"
#define                               WD_THRD_SLEEP_MSEC (500)

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static OS_LYRG_threadHandle           WDP_thrdH = NULL;
static UINT32                         active = 0;
static UINT32                         error = 0;
static int                            fd;
static int                            wdTimeout;
static int							  resetRequest = 0;

static void sig_handler(int signo);


/****************************************************************************
*
*  Function Name: WDP_stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static ERRG_codeE WDP_start( UINT32 wdTimeout )
{
   int status;

   if ((wdTimeout * 1000) <= WD_THRD_SLEEP_MSEC)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error: watchdog timeout is less then thread sleep time\n");
      return ERR_UNEXPECTED;
   }

   if (signal(SIGINT, sig_handler) == SIG_ERR)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "can't catch SIGUSR1\n");
      return ERR_UNEXPECTED;
   }

   fd = open(WD_FD, O_WRONLY);

   if (fd < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "Error: fail to open wd device\n");
      return ERR_UNEXPECTED;
   }

   status = ioctl(fd, WDIOC_SETTIMEOUT, &wdTimeout);

   if (status < 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL,"error doing ioctl WDIOC_SETTIMEOUT %d \n",wdTimeout);
      return ERR_UNEXPECTED;
   }

   LOGG_PRINT(LOG_INFO_E, NULL,"WD thread active, wdTimeout = %d\n",wdTimeout);

   return RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: WDP_stop
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void WDP_stop( void )
{
   UINT8 buf[1] = {"V"};

   write(fd,buf,1);
   close(fd);
}


/****************************************************************************
*
*  Function Name: sig_handler
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        WDP_stop();
        exit(0);
    }
}


/****************************************************************************
*
*  Function Name: WDG_thread
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
static int WDP_thread(void *argP)
{
   int status = 0;

   FIX_UNUSED_PARAM_WARN(argP);

   WDP_start(wdTimeout);

   while (active)
   {
      if (!error)
         status = ioctl(fd, WDIOC_KEEPALIVE, 0);

      if (status < 0)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL,"error doing ioctl WDIOC_KEEPALIVE\n");
         return ERR_UNEXPECTED;
      }

	  if(resetRequest)
	  {
	     OS_LYRG_usleep(10);
         close(fd);
         system("/sbin/reboot -f");
	  }
      OS_LYRG_usleep(WD_THRD_SLEEP_MSEC * 1000);

   }

   WDP_stop();
   LOGG_PRINT(LOG_INFO_E, NULL,"WD thread inactive\n");

   return RET_SUCCESS;
}


/****************************************************************************
*
*  Function Name: WDG_deinit
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE WDG_deinit( void )
{
   ERRG_codeE               ret = ICC__RET_SUCCESS;

   if (active)
   {
      active = 0;
   }
   else
   {
      /* wd was not activated, but it may be active in the kernel */
      fd = open(WD_FD, O_WRONLY);

      if (fd >= 0)
      {
         WDP_stop();
      }
   }
   if (WDP_thrdH) {
      OS_LYRG_waitForThread(WDP_thrdH, OS_LYRG_INFINITE);
      WDP_thrdH = NULL;
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: WDG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE WDG_init( int wdTimeoutL )
{
   ERRG_codeE               ret = ICC__RET_SUCCESS;
   OS_LYRG_threadParams     thrParams;

   if (active)
      return ret;

   active = 1;
   wdTimeout = wdTimeoutL;

   if(ERRG_SUCCEEDED(ret))
   {
      //Create thread
      thrParams.func = WDP_thread;
      thrParams.id = OS_LYRG_WD_THREAD_ID_E;
      thrParams.event = NULL;
      thrParams.param = NULL;
      WDP_thrdH = OS_LYRG_createThread(&thrParams);
      if(!WDP_thrdH)
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Error: fail to create thread\n");
         ret = ERR_UNEXPECTED;
      }
   }

   return ret;
}


/****************************************************************************
*
*  Function Name: WDG_error
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void WDG_error( void )
{
   if (active)
   {
      error = 1;
      WDG_resetChip( );
   }
}


/****************************************************************************
*
*  Function Name: WDG_resetChip
*
*  Description: uses the WD HW to perform reset for the chip
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
void WDG_resetChip( void )
{
   LOGG_PRINT(LOG_ERROR_E, NULL, "Reset chip active = %d\t&%$^#%$#^#&^#$^%#@$%@#%@#$@$@\n",active);
   inu_imu__deInitSensor();
   if (active)
      {
	     close(fd);
	     system("/sbin/reboot -f");
      }
   else
      WDP_start(1);
}


/****************************************************************************
*
*  Function Name: WDG_cmd
*
*  Description: main function to start/stop/reset with the watchdog. options:
*
*                    1. Valid timeout with value > 0. Will open the watchdog in the kernel (or continue
*                        if already opened) and set the timeout in the kernel driver to the given value.
*                        Max is 15 seconds. A new thread will open in the app and will kick the dog
*                        every 500ms.
*
*                    2. Valid timeout with value = 0. Will stop the watchdog both in the userspace and
*                        in the kernel.
*
*                    3. Invalid timeout value (0xFFFFFFFF). Will use the watchdog to reset the chip.
*                        This is used as an API to recover the sw.
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context:
*
****************************************************************************/
ERRG_codeE WDG_cmd( UINT32 wdTimeoutL )
{
   ERRG_codeE  ret = SVC_MNGR__RET_SUCCESS;
#ifndef __ZEBU__// If Zebu --> 0

   if (wdTimeoutL == 0xFFFFFFFF)
   {
	  resetRequest = 1;
     system("/sbin/reboot -f");
   }
   else if (wdTimeoutL)
   {
      ret = WDG_init(wdTimeoutL);
   }
   else
   {
      ret = WDG_deinit();
   }
#else
   ret = WDG_deinit();
   //printf("WDG_deinit in Zebu mode\n");

#endif
   return ret;
}


#endif

