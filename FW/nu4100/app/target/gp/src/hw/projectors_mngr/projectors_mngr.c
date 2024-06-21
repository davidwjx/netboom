/****************************************************************************
 *
 *   FileName: Projectors_mngr.c
 *
 *   Author:  Elad R.
 *
 *   Date:
 *
 *   Description: Projectors control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif
//#include "nu4k_defs.h"
#include "inu2_internal.h"
#include "projectors_mngr.h"
#include "gen_projector_drv.h"
#include "cgss130_drv.h"
#include "gen_model_drv.h"
#include "gme_mngr.h"
#include "hcg_mngr.h"
#include "gpio_drv.h"
#include "trigger_mngr.h"
#include "xml_db.h"
#include "sequence_mngr.h"
#include "inu_projector.h"
#include <unistd.h>
#include "inu_alt.h"
#include "mipi_mngr.h"
#include "assert.h"
/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   PROJECTORS_MNGRG_projectorInfoT PROJECTORS_MNGRP_projectorsInfo[INU_DEFSG_NUM_OF_PROJECTORS];
static UINT32 projectorIndexTbl[]={0,0,0,0,0,0,0,0,0};


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************i
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static PROJECTOR_init   PROJECTORS_MNGRP_getProjectorInitFunc(INU_DEFSG_projectorModelE projectorModel);
static void          PROJECTORS_MNGRP_showProjectorCfg(INU_DEFSG_logLevelE level ,INU_DEFSG_projSelectE projectorSelectIdx);

/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/
/*
   return first projector instance in db to match the requested functionality.
*/
//TODO ---------------------------------
static int PROJECTORS_MNGRP_findProjectorInst(XMLDB_dbH dbh, INU_DEFSG_senFunctionalityE func)
{

   return 0;
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRP_getProjectorInitFunc
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/
static PROJECTOR_init PROJECTORS_MNGRP_getProjectorInitFunc(INU_DEFSG_projectorModelE projectorModel)
{
   PROJECTOR_init retInitFunc;

   switch(projectorModel)
   {
      case INU_DEFSG_POJECTOR_MODEL_INUITIVE_E:
      {
         retInitFunc = PROJ_DRVG_init;//TODO:separate proj_drv to each projector
      }
      break;

      case INU_DEFSG_POJECTOR_MODEL_AMS_E:
      {
         retInitFunc = PROJ_DRVG_init;//TODO:separate proj_drv to each projector
      }
      break;

      default:
      {
         retInitFunc = NULL;
      }
      break;
   }

   return(retInitFunc);
}

/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRP_showProjectorCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/
void PROJECTORS_MNGRP_showProjectorCfg(INU_DEFSG_logLevelE level ,INU_DEFSG_projSelectE projectorSelectIdx)
{
   PROJECTORS_MNGRG_projectorInfoT  *projectorInfoP = PROJECTORS_MNGRG_getProjectorInfo(projectorSelectIdx);
   if (!projectorInfoP)
      return;

   LOGG_PRINT(level, NULL, "projector%d Configuration:\n",  projectorSelectIdx);
   LOGG_PRINT(level, NULL, "projectorModel=%d\n",           projectorInfoP->projectorCfg.model);
   LOGG_PRINT(level, NULL, "projectorState=%d\n",           projectorInfoP->projectorCfg.state);
   LOGG_PRINT(level, NULL, "projectorType=%d\n",            projectorInfoP->projectorCfg.type);
}



/***************************************************************************
***************     G L O B A L         F U N C T I O N S     **************
****************************************************************************/

/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_getProjectorInfo
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/
PROJECTORS_MNGRG_projectorInfoT *PROJECTORS_MNGRG_getProjectorInfo(INU_DEFSG_projSelectE projectorSelect)
{
   int i;
   for (i = 0; i < INU_DEFSG_NUM_OF_PROJECTORS; i++)
   {
      if ((PROJECTORS_MNGRP_projectorsInfo[i].projectorHandle) && (PROJECTORS_MNGRP_projectorsInfo[i].projectorCfg.projectorSelect == projectorSelect))
      {
         return &PROJECTORS_MNGRP_projectorsInfo[i];
      }
   }
   return NULL;
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/

void PROJECTORS_MNGRG_init()
{
   // reset projectors manager module info structure
   UINT32 projectorSelectIdx =0;
   memset(&PROJECTORS_MNGRP_projectorsInfo, 0, sizeof(PROJECTORS_MNGRP_projectorsInfo));

   for(projectorSelectIdx = 0; projectorSelectIdx < INU_DEFSG_NUM_OF_PROJECTORS; projectorSelectIdx++)
   {
      // init projector configuration
      PROJECTORS_MNGRP_projectorsInfo[projectorSelectIdx].projectorCfg.state = INU_DEFSG_POJECTOR_MODE_DISABLE_E;
//      TODO:
   }
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_showAllprojectorsCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Projector manager
*
****************************************************************************/
void PROJECTORS_MNGRG_showAllprojectorsCfg(INU_DEFSG_logLevelE level)
{
   PROJECTORS_MNGRP_showProjectorCfg(level, INU_DEFSG_PROJ_0_E);
   PROJECTORS_MNGRP_showProjectorCfg(level, INU_DEFSG_PROJ_1_E);
   PROJECTORS_MNGRP_showProjectorCfg(level, INU_DEFSG_PROJ_2_E);
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRP_powerdown
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Projector manager
*
****************************************************************************/
static ERRG_codeE PROJECTORS_MNGRP_powerdown(PROJECTORS_MNGRG_projectorInfoT     *projectorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRP_powerup
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Projector manager
*
****************************************************************************/
static ERRG_codeE PROJECTORS_MNGRP_powerup(PROJECTORS_MNGRG_projectorInfoT     *projectorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_initSensor
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: Projector manager
*
****************************************************************************/
static ERRG_codeE PROJECTORS_MNGRG_initProjector(PROJECTORS_MNGRG_projectorInfoT     *projectorInfoP)
{
   ERRG_codeE                 ret            = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_resetProjector
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: Projector manager

****************************************************************************/
static ERRG_codeE PROJECTORS_MNGRG_resetProjector(PROJECTORS_MNGRG_projectorInfoT     *projectorInfoP )
{
   ERRG_codeE                    ret            = SENSORS_MNGR__RET_SUCCESS;

   return(ret);
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRP_configProjector
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/
static ERRG_codeE PROJECTORS_MNGRP_configProjector(PROJECTORS_MNGRG_projectorInfoT  *projectorInfoP)
{
   ERRG_codeE                 retCode        = SENSORS_MNGR__RET_SUCCESS;
   IO_HANDLE                  projectorHandle   = projectorInfoP->projectorHandle;

   LOGG_PRINT(LOG_INFO_E, NULL, "Projector %d\n",projectorInfoP->projectorCfg.projectorSelect);

   if(projectorHandle)
   {
      retCode = PROJECTORS_MNGRG_initProjector(projectorInfoP);
   }
   return(retCode);
}


static INT32 PROJECTORS_MNGRP_findFreeTableSlot()
{
   UINT32 i=0;
   for (i=0;i<INU_DEFSG_NUM_OF_PROJECTORS;++i)  
   {
      if ((PROJECTORS_MNGRP_projectorsInfo[i].projectorHandle == 0) && (PROJECTORS_MNGRP_projectorsInfo[i].projectorCfg.projectorSelect==0))
      {
         return i;
      }
   }
   return -1;
}

static INT32 PROJECTORS_MNGRP_findFreeProjectorHandle()
{
   UINT32 i=0;
   for (i=0;i<sizeof(projectorIndexTbl)/sizeof(UINT32);++i)
   {
      if (projectorIndexTbl[i]==0)
      {
         projectorIndexTbl[i]=1;
         return i;
      }
   }
   return -1;
}

static void PROJECTORS_MNGRP_freeProjectorHandle(UINT32 index)
{
   projectorIndexTbl[index]=0;
}



/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_startProjector
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/
ERRG_codeE PROJECTORS_MNGRG_startProjector(IO_HANDLE ioHandle)
{
   ERRG_codeE                          ret = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}


/****************************************************************************
*
*  Function Name: PROJECTORS_MNGRG_stopProjector
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: projectors manager
*
****************************************************************************/
ERRG_codeE PROJECTORS_MNGRG_stopProjector(IO_HANDLE ioHandle)
{
   ERRG_codeE                 ret = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}

ERRG_codeE PROJECTORS_MNGRG_setConfigProjector( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}


ERRG_codeE PROJECTORS_MNGRG_setEnableProjector( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE              ret           = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}


ERRG_codeE PROJECTORS_MNGRG_setDisableProjector( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE              ret           = SENSORS_MNGR__RET_SUCCESS;

   return ret;
}



ERRG_codeE PROJECTORS_MNGRG_open(IO_HANDLE *ioHandleP,INUG_ioctlProjectorConfigT *projectorConfig)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   GEN_PROJECTOR_DRVG_openParametersT  projectorParams;
   IO_HANDLE                        ioHandle;
   INT32 tableHandleIndex=0;
   INT32 tableSlotIndex = PROJECTORS_MNGRP_findFreeTableSlot();
   if (tableSlotIndex==-1)
   {
       return SENSORS_MNGR__ERR_OUT_OF_RSRCS;
   }
   PROJECTORS_MNGRG_projectorInfoT*  projectorInfoP = &(PROJECTORS_MNGRP_projectorsInfo[tableSlotIndex]); 
   projectorInfoP->projectorCfg = *projectorConfig;

   //TODO: get virtual
   //MEM_MAPG_getVirtAddr( (MEM_MAPG_REG_ISPI_E +  projectorConfig->i2cNum), (&projectorParams.i2cControllerAddress));
//TODO:fill projectorParams

   tableHandleIndex = PROJECTORS_MNGRP_findFreeProjectorHandle();
   if (tableHandleIndex !=-1)
   {
//      projectorInfoP->sensorTableHandleIndex = (UINT32)tableHandleIndex;
      ret = IO_PALG_open(&ioHandle, IO_PROJ_0_E + tableHandleIndex, &projectorParams);
      projectorInfoP->projectorHandle = ioHandle;
      if (ERRG_SUCCEEDED(ret))
      {
         *ioHandleP = projectorInfoP;
         PROJECTORS_MNGRP_showProjectorCfg(LOG_DEBUG_E,tableSlotIndex);
      }
    }
    else
    {
       ret= SENSORS_MNGR__ERR_OUT_OF_RSRCS;
    }
   return ret; 
}

ERRG_codeE PROJECTORS_MNGRG_close(IO_HANDLE ioHandle)
{
   ERRG_codeE ret = SENSORS_MNGR__RET_SUCCESS;
   PROJECTORS_MNGRG_projectorInfoT  *projectorInfoP = (PROJECTORS_MNGRG_projectorInfoT  *)ioHandle;

 
   //PROJECTORS_MNGRP_freeProjectorHandle(projectorInfoP->projectorTableHandleIndex);
   ret = IO_PALG_close(projectorInfoP->projectorHandle);
   //projectorInfoP->projectorHandle=0;
   //projectorInfoP->projectorCfg.projectorSelect=0;
   //projectorInfoP->projectorCfg sensorActivity = FALSE;
   
   return ret;
}


#ifdef __cplusplus
   }
#endif

