#include "inu_common.h"
#include "model.h"
#include "gme_mngr.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "gpio_drv.h"
#define MAX_MODEL_NAME_SIZE (128)
#define MAX_SENSOR_NAME_SIZE  (128)

//   #define DECLARE_SET_MODEL_ENTRY(model) extern void MODELG_setEntry_ ## model (void) __attribute__((weak))
#define DECLARE_SET_MODEL_ENTRY(model) extern void MODELG_setEntry_ ## model (void)

#define SET_MODEL_ENTRY(model) MODELG_setEntry_ ## model
//Set the model entry point if code was linked in
#define TRY_EXEC_MODEL_ENTRY(model)\
      do{\
         SET_MODEL_ENTRY(model)();\
      }while(0)


//Weak declarations of model entry points in case model-specific code is not linked in
DECLARE_SET_MODEL_ENTRY(INU_DEFSG_BOOT0_E);
DECLARE_SET_MODEL_ENTRY(INU_DEFSG_BOOT200_E);
DECLARE_SET_MODEL_ENTRY(INU_DEFSG_BOOT400_E);
DECLARE_SET_MODEL_ENTRY(INU_DEFSG_BOOT600_E);



/**
* Read the model from the dtb
*/
ERRG_codeE MODELG_readModel(INU_DEFSG_moduleTypeE *model)
{
   ERRG_codeE  retCode = INIT__RET_SUCCESS;
   int         fd,size;
   char        buf[MAX_MODEL_NAME_SIZE];

   fd = open( "/proc/device-tree/id", O_RDONLY );
   if (fd < 0)
   {
      retCode = INIT__ERR_UNEXPECTED;
      LOGG_PRINT(LOG_ERROR_E, retCode, "error opening /proc/device-tree/id! errno = %s\n",strerror(errno));
   }
   else
   {
      memset(buf,0,sizeof(buf));
      size = read(fd, buf, sizeof(buf));
      if (size < 0)
      {
         retCode = INIT__ERR_UNEXPECTED;
         LOGG_PRINT(LOG_ERROR_E, retCode, "error reading /proc/device-tree/id! errno = %s\n",strerror(errno));
      }
      else
      {
         if(strnlen(buf,sizeof(buf)) < (unsigned int)sizeof(buf))
         {

            LOGG_PRINT(LOG_INFO_E, NULL, "detected model \"%s\"\n",buf);
            *model = (INU_DEFSG_moduleTypeE)atoi(buf);

            if ((*model % INU_DEFSG_BOOT_FACTOR) >= INU_DEFSG_NUM_OF_PROJECTS_E)
            {
               retCode = INIT__ERR_UNEXPECTED;
               LOGG_PRINT(LOG_ERROR_E, retCode, "unknown model\n");
            }
         }
         else
         {
            retCode = INIT__ERR_UNEXPECTED;
            LOGG_PRINT(LOG_ERROR_E, retCode, "device tree id invalid string\n");
         }
      }
      if (close(fd) < 0)
      {
         retCode = INIT__ERR_UNEXPECTED;
         LOGG_PRINT(LOG_ERROR_E, retCode, "closing /proc/device-tree/id! errno = %s\n",strerror(errno));
      }
   }
   return(retCode);
}


/*
 Initilaize the model type
*/
ERRG_codeE MODELG_initializeModel(void)
{
   ERRG_codeE ret;
   INU_DEFSG_moduleTypeE model = INU_DEFSG_BOOT0_E;
   //Get the model type a
   ret = MODELG_readModel(&model);
   GME_MNGRG_setModelType(model);
   GME_MNGRG_setBaseVersion(model/INU_DEFSG_BOOT_FACTOR);
   
   return ret;
}

/*
* Run the model specific entry code, according to link time
*/
void MODELG_doModelEntry(void)
{
   INU_DEFSG_moduleTypeE model = GME_MNGRG_getModelType();

   
   LOGG_PRINT(LOG_INFO_E, NULL, "MODELG_doModelEntry: detected model %d\n", model);
   switch (model)
   {
      case INU_DEFSG_BOOT0_E:
          TRY_EXEC_MODEL_ENTRY(INU_DEFSG_BOOT0_E);
         break;
      case INU_DEFSG_BOOT50_E:
      case INU_DEFSG_BOOT70_E:
      case INU_DEFSG_BOOT500_E:
      case INU_DEFSG_BOOT510_E:       
      case INU_DEFSG_BOOT520_E:
        //TRY_EXEC_MODEL_ENTRY(INU_DEFSG_BOOT0_E);
         break;
      case INU_DEFSG_BOOT200_E:
         TRY_EXEC_MODEL_ENTRY(INU_DEFSG_BOOT200_E); 
         break;
      case INU_DEFSG_BOOT400_E:
      case INU_DEFSG_BOOT401_E:
         TRY_EXEC_MODEL_ENTRY(INU_DEFSG_BOOT400_E);
         break;
      case INU_DEFSG_BOOT600_E:
         TRY_EXEC_MODEL_ENTRY(INU_DEFSG_BOOT600_E);
         break;
      default:
         //TRY_EXEC_MODEL_ENTRY(INU_DEFSG_BOOT0_E);
         break;
   }
}

