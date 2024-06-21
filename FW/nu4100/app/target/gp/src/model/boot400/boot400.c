#include "inu_common.h"

#include "model.h"
#include "app_init.h"
#include "gpio_drv.h"


void boot400_init(void *arg)
{
   (void)arg;
   
}

void boot400_entry(void)
{
   ERRG_codeE ret;

   //register system initialization callback
   ret = APP_INITG_registerInitCb(boot400_init, NULL);

   if(ERRG_SUCCEEDED(ret))
      LOGG_PRINT(LOG_INFO_E, NULL, "boot400 entry completed\n");
   else
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in boot400 entry 0x%x\n", ret);
}

DEFINE_SET_MODEL_ENTRY(boot400_entry, INU_DEFSG_BOOT400_E)

