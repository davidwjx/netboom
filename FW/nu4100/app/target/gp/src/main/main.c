/****************************************************************************
 *
 *   FileName: main.c
 *
 *   Author:
 *
 *   Date:
 *
 *   Description: Main for GP appliction
 *
 ****************************************************************************/

/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#include "inu_common.h"
#include <unistd.h>
#include "app_init.h"
#include <sys/syscall.h>

/****************************************************************************
 ***************       L O C A L    D E F N I T I O N S       ***************
 ****************************************************************************/
#define MAINP_SLEEP_FOR_LAST_LOGS  (1000000)
#define MAINP_SCHED_INIT_PRIORITY   (50)
#define MAIN_USE_RR_PRIORITY_FOR_INIT

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L    T Y P E D E F S           ***************
 ****************************************************************************/


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
int main(int argc, char *argv[])
{
   int ret = SUCCESS_E;
   int wdTimeout = 0;
   int opt = 0;
   ERRG_codeE  retCode;
   inu_deviceH device;
   int if_select = 0;
   int port = 0;
   extern char *optarg;
#ifdef MAIN_USE_RR_PRIORITY_FOR_INIT
   struct sched_param scParams;
   int originalPolicy;
#endif //MAIN_USE_RR_PRIORITY_FOR_INIT
   while ((opt = getopt(argc, argv, "w:t:p:h")) != -1)
   {
      switch(opt)
      {
         case 'w':
            wdTimeout = strtol(optarg, NULL, 10);
         break;

         case 't':
            if_select = strtol(optarg, NULL, 10);
         break;

         case 'p':
            port = strtol(optarg, NULL, 10);
         break;

         case 'h':

            printf("help\n");
            printf("-w WD timeout\n");
            printf("-t Interface select (0-usb, 1-uart, 2-tcp 3-stub)\n");
            printf("-p TCP port\n");
            return 0;
         break;
      }
   }

   LOGG_PRINT(LOG_DEBUG_E, NULL, "start INU GP application main\n");
#ifdef MAIN_USE_RR_PRIORITY_FOR_INIT
   // Save current sched policy for restoration after executing APP_INITG_init()
   originalPolicy = sched_getscheduler(0);

   // To ensure that all default threads that are created by APP_INITG_init() will, at least,
   // inherit a higher priority with SCHED_RR policy.
   // This code was added to ensure that the Helper Thread, that timer_create() creates,
   // will have a high priority and strong policy so it will not be blocked by lesser threads.
   // The code for timer_create() can be found in here:
   // https://codebrowser.dev/glibc/glibc/sysdeps/unix/sysv/linux/timer_create.c.html
   scParams.__sched_priority = MAINP_SCHED_INIT_PRIORITY;
   sched_setscheduler(0, SCHED_RR, &scParams);
#endif //MAIN_USE_RR_PRIORITY_FOR_INIT
   retCode = APP_INITG_init(wdTimeout, 0, NULL, NULL,INU_DEVICE__NORMAL_MODE,&device, if_select, port);
   if(ERRG_FAILED(retCode))
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "GP application init failed !!! exit program\n");
      OS_LYRG_usleep(MAINP_SLEEP_FOR_LAST_LOGS);
      return (FAIL_E);
   }
#ifdef MAIN_USE_RR_PRIORITY_FOR_INIT
   // Once APP_INITG_init() was executed, return the sched policy to its original values:
   sched_setscheduler(0, originalPolicy, NULL);
#endif //MAIN_USE_RR_PRIORITY_FOR_INIT
   LOGG_PRINT(LOG_INFO_E, NULL, "INU GP application init done. Ready for operation.\n\n");

   //Enter a background thread in this context
   while(TRUE)
   {
      while ( LOGG_sendFileToLogger()== FALSE)
      {
         OS_LYRG_usleep(100000);
      }
      OS_LYRG_usleep(3000000);
   }

   return ret;
}

