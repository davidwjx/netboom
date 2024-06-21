#include "inu_common.h"

#include "model.h"
#include "app_init.h"
#include "gpio_drv.h"


void boot600_init(void *arg)
{
   (void)arg;
   
}

void boot600_entry(void)
{
   ERRG_codeE ret;

   //register system initialization callback
   ret = APP_INITG_registerInitCb(boot600_init, NULL);

   if(ERRG_SUCCEEDED(ret))
      LOGG_PRINT(LOG_INFO_E, NULL, "boot600 entry completed\n");
   else
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in boot600 entry 0x%x\n", ret);
}

DEFINE_SET_MODEL_ENTRY(boot600_entry, INU_DEFSG_BOOT600_E)

