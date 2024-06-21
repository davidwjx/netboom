#include "inu_common.h"

#include "model.h"
#include "app_init.h"
#include "gpio_drv.h"


void boot200_init(void *arg)
{
   (void)arg;
   
}

void boot200_entry(void)
{
   ERRG_codeE ret;

   //register system initialization callback
   ret = APP_INITG_registerInitCb(boot200_init, NULL);

   if(ERRG_SUCCEEDED(ret))
      LOGG_PRINT(LOG_INFO_E, NULL, "boot200 entry completed\n");
   else
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in boot200 entry 0x%x\n", ret);
}

DEFINE_SET_MODEL_ENTRY(boot200_entry, INU_DEFSG_BOOT200_E)

