/*
/*
  inu2.c
*/
#include "inu_types.h"
#include "err_defs.h"
#include "inu2.h"
#include "version.h"
#include "inu2_internal.h"
#include "inu_factory.h"
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "svc_cfg.h"
#include "mem_pool.h"
#include "marshal.h"
#include "inu_comm.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INU_INIT_INSTEAD_APP_INIT

#ifdef INU_INIT_INSTEAD_APP_INIT
#if DEFSG_IS_GP
#include "app_init.h"
#include "inu_charger.h"
#include "inu_alt.h"
#include "model.h"
#include "hw_mngr.h"
#include "cnn_boot.h"
#include "icc.h"
#include "trigger_mngr.h"
#include "wd.h"
#include "gme_mngr.h"
#include "cdc_mngr.h"
#include "isp_mngr.hpp"
#include "metadata_updater.h"
#include "sensorsync_updater.h"
#include "helsinki_isp_synchronizer.h"
#endif
#endif

// argument 'resume' specifies whether this is not initial inu_init but just resuming from
// suspend state where only parital initialization is required.
ERRG_codeE inu_init(BOOLEAN resume)
{
   ERRG_codeE            ret = SVC_MNGR__RET_SUCCESS;
#ifdef INU_INIT_INSTEAD_APP_INIT
   VERSION_PRINT;

#if DEFSG_IS_GP
   INU_DEFSG_moduleTypeE modelType;
   INU_DEFSG_baseVersionE baseVersion;

   if (!resume)
   {
#ifdef ENABLE_UVC
      UVC_MAING_init(1);
      UVC_MAING_init(0);// workaround run lib twice
#endif
      //printf("uvc_main_init FINISHED 2\n");
      ret = MODELG_initializeModel();
      if(ERRG_SUCCEEDED(ret))
      {
         // init mmap module
         LOGG_PRINT(LOG_INFO_E, NULL, "init memory map module\n");
         ret = MEM_MAPG_open();
         if(ERRG_FAILED(ret))
         {
            LOGG_PRINT(LOG_ERROR_E, ret, "open memory map module failed\n");
         }
         else
         {
            UINT32 *virtP;
            MEM_MAPG_getVirtAddr(MEM_MAPG_CRAM_STATS_E,(MEM_MAPG_addrT*)&virtP);
            *virtP = 0;
         }
      }
   }

   if (ERRG_SUCCEEDED(ret))
   {
      ret = HW_MNGRG_earlyInit();
   }

   if (ERRG_SUCCEEDED(ret))
   {
      ret = ALTG_init();
   }

   MODELG_doModelEntry();
#endif
#endif

   if (!resume)
   {
      ret = OS_LYRG_init();
      if (ERRG_SUCCEEDED(ret))
      {
         LOGG_open();
         MEM_POOLG_init();
         MARSHALG_init();
         inu_factory__init();
         //Initialize static connection/channel/service configuration
         SVCFG_init();
         //Initialize CLS
         ret = CONN_LYRG_init();
      }

      if (ERRG_SUCCEEDED(ret))
      {
         //Initialize Service Com Layer
         ret = SVC_COMG_init();
      }

      if (ERRG_SUCCEEDED(ret))
      {
         SVCFG_updateMemSizes();
      }

      if (ERRG_SUCCEEDED(ret))
      {
         NUFLD_init();
      }
   }

#ifdef INU_INIT_INSTEAD_APP_INIT
#if DEFSG_IS_GP
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "initialize Data Base\n");
      ret = DATA_BASEG_initDataBase();
      if(ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "Data base module initialization failed\n");
      }
   }

   if (ERRG_SUCCEEDED(ret))
   {
      if (!resume)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "init interrupt ctrl module\n");
         ret = OS_LYRG_intCtrlInit();
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "init model CB\n");
      //APP_INITP_initalizeCbTblInit();
   }


   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "init HW manager\n");
      ret = HW_MNGRG_init();
   }

   if(ERRG_SUCCEEDED(ret))
   {
      //ret = APP_INITG_dispatch_init();
   }

   if (ERRG_SUCCEEDED(ret))
   {
      if (!resume)
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "init ICC\n");
         ret = ICCG_iccInit();
      }
   }

   if (ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "init trigger mngr\n");
      ret = TRIGGER_MNGRG_init();
   }

   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "Start CNN BOOT\n");
      ret = CNN_BOOTG_start();
   }

#ifndef __UART_ON_FPGA__
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_DEBUG_E, NULL, "init Watchdog %d\n",3);
      ret = WDG_cmd(3);
   }
#endif

   if (ERRG_SUCCEEDED(ret))
   {
      if (!resume)
      {
         ret = inu__reuseMemInit();
      }
   }

   if(ERRG_SUCCEEDED(ret))
   {
//      ret = MEM_MAPG_showMappings();
   }

   modelType = GME_MNGRG_getModelType();
   baseVersion = GME_MNGRG_getBaseVersion();

   if ((modelType == INU_DEFSG_BOOT51_E) || (modelType == INU_DEFSG_BOOT501_E) || (modelType == INU_DEFSG_BOOT53_E) || (baseVersion >= INU_DEFSG_BASE_VERSION_2_E))
   {
      if (!resume)
      {
         // UART2 needs to be activated for Attiny
         inu_charger__init_charger_t initChargerParams;
         initChargerParams.debugEnable = 0;
         initChargerParams.chargerModule = INU_CHARGER__AT828_E;
         initChargerParams.uartBaudRate = 19200;
         initChargerParams.uartPortNum = 2;
         CHARGERG_start(&initChargerParams);
      }
   }

   if ((modelType == INU_DEFSG_BOOT51_E) || (modelType == INU_DEFSG_BOOT501_E))
   {
      // in case of fw update for NAND once we booted, set boot success flag.
      inu_device__gpCheckFwUpdateBootSuccess();
   }

   if(ERRG_SUCCEEDED(ret))
   {
      inu_graph__initEngine();
   }
   if(ERRG_SUCCEEDED(ret))
   {
      METADATA_UPDATER_init();
   }
   if(ERRG_SUCCEEDED(ret))
   {
      SENSORSYNC_UPDATER_init();
   }
    if(ERRG_SUCCEEDED(ret))
   {
      HELSINKI_ISP_Sync_Init();
   }
   if(ERRG_SUCCEEDED(ret))
   {
      CDC_MNGRG_handle handle;

      if (!resume)
      {
         CDC_MNGRG_init();
         CDC_MNGRG_open(&handle, 0, CDC_MNGRG_DEFAULT_UART_BAUDRATE);
      }
   }

#endif
#endif

   return ret;
}

// argument 'suspend' specifies whether this is not final de-initalization but just
// entering to suspend state where only parital de-initialization is required.
void inu_deinit(BOOLEAN suspend)
{
#ifdef INU_INIT_INSTEAD_APP_INIT
#if DEFSG_IS_GP
   ERRG_codeE ret;

   if (!suspend)
   {
      CDC_MNGRG_close(NULL);
      CDC_MNGRG_deinit();
   }

   inu_graph__deinitEngine();
   ret = HW_MNGRG_close();
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "close hw mngr failed\n");
   }

   WDG_cmd(0);

   if (!suspend)
   {
      MARSHALG_deInit();
      LOGG_PRINT(LOG_INFO_E, NULL, "close memory map module\n");
      ret = MEM_MAPG_close();
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "close memory map module failed\n");
      }

      // ALTG_deInit();

      ret = ICCG_iccClose();
      if (ERRG_FAILED(ret))
      {
         LOGG_PRINT(LOG_ERROR_E, ret, "close ICC module failed\n");
      }
   }

   ret = TRIGGER_MNGRG_deinit();
   if(ERRG_FAILED(ret))
   {
      LOGG_PRINT(LOG_ERROR_E, ret, "close trigger mngr module failed\n");
   }
   #ifdef ENABLE_UVC
   UVC_MAING_closeThreads();
   #endif

   LOGG_PRINT(LOG_INFO_E, NULL, "deinit complete\n");
#endif
#endif

   if (!suspend)
   {
      CONN_LYRG_deinit();
   }

#if (DEFSG_IS_HOST)
   MARSHALG_deInit();
#endif
}

void inu_useLongerTimeout(bool useLongerTimeout)
{
    inu_ref__useLongerTimeout(useLongerTimeout);
}

#ifdef __cplusplus
}
#endif
