#include "inu_common.h"
#include "model.h"
#include "app_init.h"

void boot0_init(void *arg)
{
   (void)arg;
}

void boot0_entry(void)
{
   ERRG_codeE ret;

   //register system initialization callback
   ret = APP_INITG_registerInitCb(boot0_init, NULL);
   
   if(ERRG_SUCCEEDED(ret))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "boot0 entry completed\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "error in boot0 entry 0x%x\n", ret);
   }
}

DEFINE_SET_MODEL_ENTRY(boot0_entry, INU_DEFSG_BOOT0_E)



