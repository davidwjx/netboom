#include "inu_common.h"
#include "inu_defs_params.h"

#define INU_DEFS_PARAMS_LINE_MAX 128
static INU_DEFS_PARAMS_configTblT INU_DEFS_PARAMS_overwriteVal[INU_DEF_NUM_PARAMS] = {
                                                     { INU_DEFS_PARAMS_MAX_INJECT_SIZE_STR,0,0} ,
                                                     { INU_DEFS_PARAMS_MAX_VIDEO_SIZE_STR,0,0 } ,
                                                     { INU_DEFS_PARAMS_MAX_DEPTH_SIZE_STR,0,0 } ,
                                                     { INU_DEFS_PARAMS_MAX_WEBCAM_SIZE_STR,0,0 } ,
                                                     { INU_DEF_PARAMS_GP_HOST_GENERAL_SIZE_STR ,0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_ALG_CTRL_SIZE_STR,0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_CLIENT_CTRL_SIZE_STR,0,0 },
                                                     { INU_DEF_PARAMS_SVC_MNGR_DATA_SIZE_STR, 0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_SYSTEM_SIZE_STR, 0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_ALG_DATA_SIZE_STR, 0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_LOGGER_SIZE_STR, 0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_POS_SENSORS_SIZE_STR, 0,0 },
                                                     { INU_DEF_PARAMS_GP_HOST_CLIENT_DATA_SIZE_STR,0,0 },
                                                     { INU_DEF_INJECT_VGA_NUM_BUFS_STR , 0,0 },
                                                     { INU_DEF_INJECT_HD_NUM_BUFS_STR , 0,0 },
                                                     { INU_DEF_VIDEO_VGA_NUM_BUFS_STR ,0,0 },
                                                     { INU_DEF_VIDEO_HD_NUM_BUFS_STR  ,0,0 },
                                                     { INU_DEF_DEPTH_VGA_NUM_BUFS_STR , 0,0 },
                                                     { INU_DEF_DEPTH_HD_NUM_BUFS_STR  ,0,0 },
                                                     { INU_DEF_WEBCAM_VGA_NUM_BUFS_STR , 0,0 },
                                                     { INU_DEF_WEBCAM_HD_NUM_BUFS_STR  ,0,0 },
                                                     { INU_DEF_CLIENT_CTRL_NUM_BUFS_STR, 0,0 },
                                                     { INU_DEF_CLIENT_DATA_NUM_BUFS_STR , 0,0 }
};



void INU_DEFS_PARAMS_readFile()
{
   char line[INU_DEFS_PARAMS_LINE_MAX];  
   char paramName[INU_DEFS_PARAMS_LINE_MAX];
   unsigned int paramVal,ind;
   FILE *file;
 
   file = fopen(INU_DEFS_PARAMS_FILE_NAME,"r");
   if ( file == NULL)
   {
       LOGG_PRINT(LOG_INFO_E, NULL, "Can't find %s\n", INU_DEFS_PARAMS_FILE_NAME);
   }
   else
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "Using %s\n", INU_DEFS_PARAMS_FILE_NAME);
      while (fgets(line,sizeof(line),file) != NULL)
      {
            if ( sscanf(line,"%s %d",paramName,&paramVal) == 2)
            {
               for (ind=0;ind<INU_DEF_NUM_PARAMS;ind++)
               {
                  if (strcmp(INU_DEFS_PARAMS_overwriteVal[ind].str, paramName) == 0)
                  {
                     LOGG_PRINT(LOG_INFO_E, NULL, "%s %d\n",paramName,paramVal);
                     INU_DEFS_PARAMS_overwriteVal[ind].paramVal = paramVal;
                     INU_DEFS_PARAMS_overwriteVal[ind].paramFound = 1;
                  }
               }
            }
        }
   }
 }



BOOL INU_DEFS_PARAMS_overwriteValue(INU_DEFS_PARAMS_IndE inuDefVal, unsigned int *val)
{
   static unsigned char read_inudef_table_done = 0;
   if (read_inudef_table_done == 0)
   {
      INU_DEFS_PARAMS_readFile();
      read_inudef_table_done = 1;
   }
   if (INU_DEFS_PARAMS_overwriteVal[inuDefVal].paramFound)
   {
      *val = INU_DEFS_PARAMS_overwriteVal[inuDefVal].paramVal;
   }

     return INU_DEFS_PARAMS_overwriteVal[inuDefVal].paramFound;
}
