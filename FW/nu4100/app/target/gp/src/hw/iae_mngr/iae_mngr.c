/****************************************************************************
 *
 *   FileName: iae_mngr.c
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: iae control
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"

#ifdef __cplusplus
   extern "C" {
#endif

#include "nu4k_defs.h"
#include "iae_mngr.h"
#include "iae_drv.h"
#include "cde_mngr_new.h"
#include "hcg_mngr.h"
#include "xml_db.h"

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define IAE_MNGRP_DEFAULT_IMG_PORCH_SIZE  (8)

// IAE interrupts for sensor Recovery
#define IAE_MNGRP_SLU_SENSOR_RECOVERY_INTERRUPT (0x0)
#define IAE_MNGRP_IAU_SENSOR_RECOVERY_INTERRUPT (0x0)
#define IAE_MNGRP_TOP_SENSOR_RECOVERY_INTERRUPT (0x1)


// IAE interrupts for Recovery
#define IAE_MNGRP_SLU_RECOVERY_INTERRUPT     (0x1F)
#define IAE_MNGRP_IAU_RECOVERY_INTERRUPT     (0x70)
#define IAE_MNGRP_TOP_RECOVERY_INTERRUPT     (0x3FFFFF)


// IAE interrupts  ready for histogram
#define IAE_MNGRP_SLU_HIST_READY_INTERRUPT   (0x0)
#define IAE_MNGRP_IAU_HIST_READY_INTERRUPT   (0x1)
#define IAE_MNGRP_TOP_HIST_READY_INTERRUPT   (0x0)

// IAE interrupts missed for histogram
#define IAE_MNGRP_SLU_HIST_MISSED_INTERRUPT  (0x0)
#define IAE_MNGRP_IAU_HIST_MISSED_INTERRUPT  (0x2)
#define IAE_MNGRP_TOP_HIST_MISSED_INTERRUPT  (0x0)

// IAE interrupts missed for histogram
#define IAE_MNGRP_SLU_MIPI_ERR_INTERRUPT     (0x0)
#define IAE_MNGRP_IAU_MIPI_ERR_INTERRUPT     (0x0)
#define IAE_MNGRP_TOP_MIPI_ERR_INTERRUPT     (0xFFFFFE)

#define IAE_MNGRP_STATUS_SLU_ALL_INTERRUPTS (IAE_MNGRP_SLU_SENSOR_RECOVERY_INTERRUPT | IAE_MNGRP_SLU_HIST_MISSED_INTERRUPT | IAE_MNGRP_SLU_MIPI_ERR_INTERRUPT)
#define IAE_MNGRP_STATUS_IAU_ALL_INTERRUPTS (IAE_MNGRP_IAU_SENSOR_RECOVERY_INTERRUPT | IAE_MNGRP_IAU_HIST_MISSED_INTERRUPT | IAE_MNGRP_IAU_MIPI_ERR_INTERRUPT)
#define IAE_MNGRP_STATUS_TOP_ALL_INTERRUPTS (IAE_MNGRP_TOP_SENSOR_RECOVERY_INTERRUPT | IAE_MNGRP_TOP_HIST_MISSED_INTERRUPT | IAE_MNGRP_TOP_MIPI_ERR_INTERRUPT)
#define IAE_MNGRP_STATUS_HIST_ALL_INTERRUPTS (IAE_MNGRP_IAU_RECOVERY_INTERRUPT | IAE_MNGRP_IAU_HIST_READY_INTERRUPT)


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static   IAE_MNGRG_iaeInfoT IAE_MNGRP_iaeInfo;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/

/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void IAE_MNGRP_getIaeBypassReg(XMLDB_dbH hwDb, UINT32 *bypass, UINT32 blkInstance);


/****************************************************************************
 ***************      L O C A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: IAE_MNGRP_enableCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE Config context
*
****************************************************************************/
static ERRG_codeE IAE_MNGRP_enableCfg(UINT32 *enableCfg)
{
   ERRG_codeE              retCode        = IAE_MNGR__RET_SUCCESS;
   IAE_DRVG_enableModuleE   enableBlock;
   int isBitSet;
   UINT32 startEnableValue = IAE_DRBVG_getCurrentSLUEnableValue();
   for (enableBlock=0; enableBlock<15; enableBlock++)
   {
      isBitSet = *enableCfg & (1 << enableBlock);
      if (isBitSet)
         IAE_DRVG_getSLUWriteValueEnable(&enableBlock,&startEnableValue);
   }
   IAE_DRVG_writeSLUEnable(startEnableValue);
   return(retCode);
}

/****************************************************************************
*
*  Function Name: IAE_MNGRP_enableCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE Config context
*
****************************************************************************/
static ERRG_codeE IAE_MNGRP_disableCfg(UINT32 *enableCfg)
{
   ERRG_codeE              retCode        = IAE_MNGR__RET_SUCCESS;
   IAE_DRVG_enableModuleE   enableBlock;
   int isBitSet;
   UINT32 startDisableValue  = IAE_DRBVG_getCurrentSLUEnableValue();
   for (enableBlock=0; enableBlock<15; enableBlock++)
   {
      isBitSet = *enableCfg & (1 << enableBlock);
      if (isBitSet)
         IAE_DRVG_getSLUWriteValueDisable(&enableBlock,&startDisableValue);
   }
   IAE_DRVG_writeSLUEnable(startDisableValue);
   return(retCode);
}


/****************************************************************************
*
*  Function Name: IAE_MNGRP_setIaeBypassReg
*
*  Description: this function build the relevant iau bypass values
*
*  Inputs:
*
*  Outputs: 
*
*  Returns:
*
*  Context: IAE Config context
*
****************************************************************************/
static void IAE_MNGRP_getIaeBypassReg(XMLDB_dbH hwDb, UINT32 *bypass, UINT32 blkInstance)
{
   UINT32 value = 0;
   ERRG_codeE ret;
   (void)blkInstance;

   *bypass = 0;
    
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_YUV0_E, &value );
   *bypass = *bypass | (value << 0);
   //LOGG_PRINT(LOG_INFO_E, NULL, "YUV0 reg %d bypassVal %x\n", value,*bypass,ret);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_IB0_E, &value );
   *bypass = *bypass | (value << 2);
   //LOGG_PRINT(LOG_INFO_E, NULL, "IB0 reg %d bypassVal %x\n", value,*bypass,ret);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_WB0_E, &value );
   *bypass = *bypass | (value << 3);
   //LOGG_PRINT(LOG_INFO_E, NULL, "WB0 reg %x bypassVal %x\n", value,*bypass,ret);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_DMS0_E, &value );
   *bypass = *bypass | (value << 4);
   //LOGG_PRINT(LOG_INFO_E, NULL, "DMS0 reg %x bypassVal %x\n", value,*bypass,ret);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_GCR0_E, &value );
   *bypass = *bypass | (value << 5);
   //LOGG_PRINT(LOG_INFO_E, NULL, "GCR0 reg %x bypassVal %x\n", value,*bypass,ret);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_CSC0_E, &value );
   *bypass = *bypass | (value << 6);
   //LOGG_PRINT(LOG_INFO_E, NULL, "CSC0 reg %x bypassVal %x\n", value,*bypass,ret);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_DSR0_E, &value );
   *bypass = *bypass | (value << 7);
   //LOGG_PRINT(LOG_INFO_E, NULL, "DSR0 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_YUV1_E, &value );
   *bypass = *bypass | (value << 8);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "YUV1 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_IB1_E, &value );
   *bypass = *bypass | (value << 10);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "IB1 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_WB1_E, &value );
   *bypass = *bypass | (value << 11);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "WB1 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_DMS1_E, &value );
   *bypass = *bypass | (value << 12);
   //LOGG_PRINT(LOG_INFO_E, NULL, "DMS1 reg %x bypassVal %x\n", value,*bypass);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_GCR1_E, &value );
   *bypass = *bypass | (value << 13);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "GCR1 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_CSC1_E, &value );
   *bypass = *bypass | (value << 14);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "CSC1 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_DSR1_E, &value );
   *bypass = *bypass | (value << 15);
   //LOGG_PRINT(LOG_INFO_E, NULL, "DSR1 reg %x bypassVal %x\n", value,*bypass);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_YUV2_E, &value );
   *bypass = *bypass | (value << 16);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "YUV2 reg %x bypassVal %x\n", value,*bypass);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_RPJ2_E, &value );
   *bypass = *bypass | (value << 17);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "RPJ2 reg %x bypassVal %x\n", value,*bypass);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_IB2_E, &value );
   *bypass = *bypass | (value << 18);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "IB2 reg %x bypassVal %x\n", value,*bypass);

   
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_CSC2_E, &value );
   *bypass = *bypass | (value << 22);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "CSC2 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_DSR2_E, &value );
   *bypass = *bypass | (value << 23);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "DSR2 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_YUV3_E, &value );
   *bypass = *bypass | (value << 24);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "YUV3 reg %x bypassVal %x\n", value,*bypass);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_RPJ3_E, &value );
   *bypass = *bypass | (value << 25);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "RPJ3 reg %x bypassVal %x\n", value,*bypass);

   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_IB3_E, &value );
   *bypass = *bypass | (value << 26);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "IB3 reg %x bypassVal %x\n", value,*bypass);
   
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_CSC3_E, &value );
   *bypass = *bypass | (value << 30);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "CSC3 reg %x bypassVal %x\n", value,*bypass);
     
   XMLDB_getValue( hwDb,  NU4100_IAE_BYPASS_DSR3_E, &value );
   *bypass = *bypass | (value << 31);
   //  LOGG_PRINT(LOG_INFO_E, NULL, "DSR3 reg %x bypassVal %x\n", value,*bypass);
}


/****************************************************************************
 ***************      G L O B A L       F U N C T I O N S       ***************
 ****************************************************************************/

/****************************************************************************
*
*  Function Name: IAE_MNGRG_getIaeInfo
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: IAE manager
*
****************************************************************************/
IAE_MNGRG_iaeInfoT *IAE_MNGRG_getIaeInfo()
{
   return(&IAE_MNGRP_iaeInfo);
}


/****************************************************************************
*
*  Function Name: IAE_MNGRP_firstPixelCb
*
*  Description: 
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: Interrupt thread
*
****************************************************************************/
static void IAE_MNGRP_firstPixelCb(UINT64 ts, UINT32 slu, void *argP)
{
   IAE_MNGRG_iaeInfoT    *iaeInfoP = (IAE_MNGRG_iaeInfoT*)argP;
   iaeInfoP->sluFirstPixelTs[slu] = ts;
}



/****************************************************************************
*
*  Function Name: IAE_MNGRG_deinit
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: IAE manager
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_deinit( )
{

   ERRG_codeE            retCode  = IAE_MNGR__RET_SUCCESS;
   IAE_MNGRG_iaeInfoT    *iaeInfoP   = IAE_MNGRG_getIaeInfo();

   IAE_DRVG_deinit( );

   retCode = IAE_DRVG_unregisterSnsrIsrCb(iaeInfoP->sluFirstPixelIsrHandle);
   if(ERRG_FAILED(retCode))
   {
      return retCode;
   }

   return retCode;
}



/****************************************************************************
*
*  Function Name: IAE_MNGRG_init
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: IAE manager
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_init( UINT32 memVirtAddr )
{
   ERRG_codeE            retCode  = IAE_MNGR__RET_SUCCESS;
   IAE_MNGRG_iaeInfoT    *iaeInfoP   = IAE_MNGRG_getIaeInfo();

   // reset IAE manager module info structure
   memset(iaeInfoP, 0, sizeof(IAE_MNGRG_iaeInfoT));

   retCode = IAE_DRVG_init( memVirtAddr );
   if(ERRG_FAILED(retCode))
   {
      return retCode;
   }

   retCode = IAE_DRVG_registerSnsrIsrCb(IAE_MNGRP_firstPixelCb, iaeInfoP, &iaeInfoP->sluFirstPixelIsrHandle);
   if(ERRG_FAILED(retCode))
   {
      return retCode;
   }


//   retCode = IAE_MNGRP_histInit();

   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_openIaeDrv
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE manager
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_openIaeDrv()
{
   ERRG_codeE           retCode = IAE_MNGR__RET_SUCCESS;
/*   IAE_DRVG_openParamsT iaeDrvOpenParams;
   MEM_MAPG_addrT       memVirtAddr;
   IO_HANDLE            ioHandle;

   MEM_MAPG_getVirtAddr(MEM_MAPG_REG_IAE_E, (&memVirtAddr));
   iaeDrvOpenParams.iaeRegistersVirtualAddr = (UINT32)(memVirtAddr);

   iaeDrvOpenParams.cdeHandle = IO_PALG_getHandle(IO_CDE_E);
   retCode = IO_PALG_open(&ioHandle, IO_IAE_E, &iaeDrvOpenParams);
   if(ERRG_SUCCEEDED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "open IAE driver\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "open IAE driver fail.\n");
   }*/

   return(retCode);
}

/****************************************************************************
*
*  Function Name: IAE_MNGRG_closeIaeDrv
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE manager
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_closeIaeDrv()
{
   ERRG_codeE  retCode  = IAE_MNGR__RET_SUCCESS;

/*   retCode = IO_PALG_close(IO_PALG_getHandle(IO_IAE_E));
   if(ERRG_SUCCEEDED(retCode))
   {
      LOGG_PRINT(LOG_INFO_E, NULL, "close IAE\n");
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E, retCode, "close IAE fail.\n");
   }*/

   return(retCode);
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_showIaeCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
void IAE_MNGRG_showIaeCfg(INU_DEFSG_logLevelE level)
{
   (void)level;
   IAE_DRVG_dumpRegs();
/*   INUG_ioctlIaeConfigT *iaeCfgP = &IAE_MNGRP_iaeInfo.iaeCfg;
   unsigned int i;
   LOGG_PRINT(level, NULL, "IAE configuration:\n");
   LOGG_PRINT(level, NULL, "IIM injector: enable=%d fps=%d h=%d w=%d mode=%d \n\n", iaeCfgP->iim.injector.enable, iaeCfgP->iim.injector.fps, 
      iaeCfgP->iim.injector.height, iaeCfgP->iim.injector.width, iaeCfgP->iim.injector.mode);

   for(i = 0; i < INU_DEFSG_NUM_OF_INPUT_SENSORS; i++)
   {
      LOGG_PRINT(level, NULL,"SLU%d:\n",i);
      LOGG_PRINT(level, NULL,"interfaceType=%d\n",iaeCfgP->slu[i].interfaceType);
      LOGG_PRINT(level, NULL,"frameType=%d\n", iaeCfgP->slu[i].frameType);
      LOGG_PRINT(level, NULL,"rgbPattern=%d\n", iaeCfgP->slu[i].rgbPattern);
      LOGG_PRINT(level, NULL,"porch: topRows=%d botRows=%d leftCol=%d rightCol=%d\n\n", iaeCfgP->slu[i].porch.topRows,iaeCfgP->slu[i].porch.botRows,
         iaeCfgP->slu[i].porch.leftCols, iaeCfgP->slu[i].porch.rightCols);
   }

   for(i = 0; i < INU_DEFSG_NUM_OF_IAU_UNITS; i++)
   {   
      LOGG_PRINT(level, NULL,"IAU%d:\n",i);
      LOGG_PRINT(level, NULL,"bpc active=%d h=%d w=%d\n",iaeCfgP->iau[i].bpc.active, iaeCfgP->iau[i].bpc.height, iaeCfgP->iau[i].bpc.width);
      LOGG_PRINT(level, NULL,"ib active=%d\n", iaeCfgP->iau[i].ib.active);
      LOGG_PRINT(level, NULL,"wb active=%d CB=%d CR=%d CGB=%d CGR=%d, WB=%d WR=%d WGB=%d WGR=%d\n",
         iaeCfgP->iau[i].wb.active, iaeCfgP->iau[i].wb.colorBlue, iaeCfgP->iau[i].wb.colorRed, iaeCfgP->iau[i].wb.colorGreenBlue, 
         iaeCfgP->iau[i].wb.colorGreenRed, iaeCfgP->iau[i].wb.whiteBlue, iaeCfgP->iau[i].wb.whiteRed, iaeCfgP->iau[i].wb.whiteGreenBlue, 
         iaeCfgP->iau[i].wb.whiteGreenRed);
      LOGG_PRINT(level, NULL,"dms active=%d\n", iaeCfgP->iau[i].dms.active);
      LOGG_PRINT(level, NULL,"gcr active=%d\n", iaeCfgP->iau[i].gcr.active);
      LOGG_PRINT(level, NULL,"fir active=%d h=%d w=%d\n", iaeCfgP->iau[i].fir.active);
      LOGG_PRINT(level, NULL,"dsr active=%d h=%d w=%d\n", iaeCfgP->iau[i].dsr.active, iaeCfgP->iau[i].dsr.height, iaeCfgP->iau[i].dsr.width);
      LOGG_PRINT(level, NULL,"yuv active=%d\n\n", iaeCfgP->iau[i].yuv.active);
   }

   for(i = 0; i < INU_DEFSG_IAE_NUM_OF_CHANNELS_E; i++)
   {
      LOGG_PRINT(level, NULL,"outChan%d:\n",i);
      LOGG_PRINT(level, NULL,"active=%d\n", iaeCfgP->outChan[i].active);
      LOGG_PRINT(level, NULL,"compress active=%d\n", iaeCfgP->outChan[i].compress.active);
      LOGG_PRINT(level, NULL,"crop active=%d x0=%d x1=%d y0=%d y1=%d\n", iaeCfgP->outChan[i].crop.active, iaeCfgP->outChan[i].crop.x0, 
         iaeCfgP->outChan[i].crop.x1, iaeCfgP->outChan[i].crop.y0, iaeCfgP->outChan[i].crop.y1);
      LOGG_PRINT(level, NULL,"dsamp active=%d scaleRatio=%d\n", iaeCfgP->outChan[i].dsamp.active, iaeCfgP->outChan[i].dsamp.scaleRatio);
      LOGG_PRINT(level, NULL,"outChanSelect=%d H=%d W=%d h=%d w=%d x=%d y=%d\n\n", iaeCfgP->outChan[i].outChanSelect, iaeCfgP->outChan[i].bufferHeight, iaeCfgP->outChan[i].bufferWidth,iaeCfgP->outChan[i].outHeight, 
         iaeCfgP->outChan[i].outWidth,iaeCfgP->outChan[i].frameStartX,iaeCfgP->outChan[i].frameStartY);
   }*/
}

/****************************************************************************
*
*  Function Name: IAE_MNGRG_receiveHistogram
*
*  Description: Histogram in memory (one side):
*
*                    ------- roi 0 -------
*                    0x000 - 0x3ff red
*                    0x400 - 0x7ff green
*                    0x800 - 0xbff blue
*
*                    ------- roi 1 -------
*                    0xc00 - 0xfff red
*                    0x1000 - 0x13ff green      
*                    0x1400 - 0x17ff blue
*
*                    ------- roi 2 -------                    
*                    0x1800 - 0x1bff red
*                    0x1c00 - 0x1fff green      
*                    0x2000 - 0x23ff blue
*
*                    ------- roi 0 ------
*                    0x2400 - 0x2403 accumulator red
*                    0x2404 - 0x2407 accumulator green
*                    0x2408 - 0x240b accumulator blue
*
*                    ------- roi 1 ------
*                    0x240c - 0x240f accumulator red
*                    0x2410 - 0x2413 accumulator green
*                    0x2414 - 0x2417 accumulator blue
*
*                    ------- roi 2 ------
*                    0x2418 - 0x241b accumulator red
*                    0x241c - 0x241f accumulator green
*                    0x2420 - 0x2424 accumulator blue
*
*                    ------- frame counter -------
*                    0x2424 - 0x2428
*
*
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE control
*
****************************************************************************/
/*void IAE_MNGRG_printHist(void *ptr)
{
   int i,j;
   UINT64 *temp = (UINT64*)ptr;
   UINT32 *temp_32 = (UINT32*)ptr;

   printf("printHist:\n");
   for(j=0;j<1;j++)
   {
      printf("histogram %d:\n",j);
      for (i = 0; i < (0x100); i++)
      {
         printf("%d ",temp_32[i+(j*0x100)]);
      }
      printf("\n");
   }
   printf("\n");
   for(j=0;j<3;j++)
   {
      printf("accumulator %d: %d\n",j,temp_32[(0x2400/4)+j]);
   }
   printf("frame counter = %d\n\n",temp_32[0x2424/4]);
}*/


/****************************************************************************
*
*  Function Name: IAE_MNGRG_showIaeCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setConfigGen( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE retCode = IAE_MNGR__RET_SUCCESS;
   (void)block;
   (void)blkInstance;
   (void)arg;
   (void)hwDb;

#if 0
   IAE_DRVG_iaeFGenCfgT    genCfg;  
   UINT32 val = 0;


   genCfg.genSelect = blkInstance;
   
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_GEN_E, blkInstance, NU4100_IAE_IIM_GENS_FGEN_0_CONTROL_MODE_E) , &val );          
   genCfg.mode = val;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_GEN_E, blkInstance, NU4100_IAE_IIM_GENS_FGEN_0_CONTROL_FRAME_NUM_E) , &val );           
   genCfg.frameNum = val;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_GEN_E, blkInstance, NU4100_IAE_IIM_GENS_FGEN_0_FRAME_SIZE_HORZ_E) , &val );          
   genCfg.frameSizeHorz = val;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_GEN_E, blkInstance, NU4100_IAE_IIM_GENS_FGEN_0_FRAME_SIZE_VERT_E) , &val );          
   genCfg.frameSizeVert = val;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_GEN_E, blkInstance, NU4100_IAE_IIM_GENS_FGEN_0_BLANK_FRAME_E) , &val );           
   genCfg.frameBlank = val;
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_GEN_E, blkInstance, NU4100_IAE_IIM_GENS_FGEN_0_BLANK_LINE_E) , &val );            
   genCfg.lineBlank = val;

   
   retCode =  IAE_DRVG_FGenCfg( &genCfg );
#endif
   LOGG_PRINT(LOG_DEBUG_E, NULL, "IAE manager IAE_MNGRG_setConfigGen done\n");

   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_setConfigSlu
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setConfigSlu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   UINT32             mipiMuxCfg;
   ERRG_codeE       retCode = IAE_MNGR__RET_SUCCESS;

   (void)block;
   (void)arg;
/*
   //retCode = IAE_DRVG_configSlu(hwDb);
   XMLDB_getValue( hwDb, NUFLD_calcPath( NUFLD_MIPI_MUX_E, blkInstance, NU4100_IAE_MIPI_MUX_SLU0_LANES23_SEL_E), &mipiMuxCfg);

   if(ERRG_SUCCEEDED(retCode))
      retCode = IAE_DRVG_mipiMuxCfg(blkInstance, mipiMuxCfg);
*/
   return retCode;
}

/****************************************************************************
*
*  Function Name: IAE_MNGRG_showIaeCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setConfigIau( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg )
{
   ERRG_codeE retCode = IAE_MNGR__RET_SUCCESS;
   UINT32                     bypass = 0;
   UINT32                     src = 0;
   XMLDB_pathE                srcPath;

   (void)arg;
   //IAE_DRVG_configIae(hwDb);

   srcPath = NUFLD_calcPath( block, blkInstance, NU4100_IAE_IIM_CONTROL_IAU0_SEL_E);
   XMLDB_getValue( hwDb,  srcPath, &src );            
   IAE_DRVG_iaeIimSrcCfg( blkInstance, src );   

    IAE_MNGRP_getIaeBypassReg(hwDb, &bypass, blkInstance);
    LOGG_PRINT(LOG_DEBUG_E, NULL, "IAE bypass reg %x\n", bypass);
   
   bypass = (block == NUFLD_IAU_E) ? ((bypass >> (blkInstance * 12)) & 0x1FF) : ((bypass >> ((blkInstance * 4) + 24)) & 0x07);
   IAE_DRVG_bypassCfg( blkInstance, bypass );
   LOGG_PRINT(LOG_DEBUG_E, NULL, "IAE_MNGRG_setConfigIau - source %d, bypass = 0x%x\n", src, bypass);

   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_setEnableHist
*
*  Description:
*
*  Inputs:
*
*  Outputs: none
*
*  Returns:
*
*  Context: IAE manager
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setEnableHist( UINT8* instanceList, void *cb, void *argP )
{
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;
   IAE_MNGRG_iaeInfoT    *iaeInfoP   = IAE_MNGRG_getIaeInfo();

   iaeInfoP->histogramCb[instanceList[0]] = cb;
   iaeInfoP->histogramCbArg[instanceList[0]] = argP;

   retCode = IAE_DRVG_setHistInterrupt(instanceList[0], 1);
   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_showIaeCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setEnableGen( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;
   UINT32 enable;
   UINT8 i = 0;
   (void)hwDb;
   (void)block;
   (void)arg;

   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_IAE);

   for( i = 0; i < NU4K_NUM_GENS; i++ )
   {
      enable = *(instanceList + i);

      if( enable != 0xFF )
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "Enabling generator num %d\n", enable);
         enable += IAE_DRVG_GEN0_EN_E;
         IAE_DRVG_enableCfg(&enable);
      }
   }

   return retCode;
}

/****************************************************************************
*
*  Function Name: IAE_MNGRG_showIaeCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
UINT32 enableSLUcount = 0;
extern UINT32 gpStreamNUM;
extern bool IS_CVA_INIT;
ERRG_codeE IAE_MNGRG_setEnableSlu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;
   UINT8    i = 0;
   UINT32   enabledSlus = 0;
   (void)hwDb;
   (void)block;
   UINT8* parallelInstanceList = (UINT8*) arg;
   (void)voterHandle;

#if 0
   enableSLUcount++;
   if(enableSLUcount < gpStreamNUM*2)
      return retCode;
#endif
   LOGG_PRINT(LOG_INFO_E, 0, "set IAE ready enableSLUcount %d\n",enableSLUcount);
   IAE_DRVG_iaeReady();

#if 0
   enabledSlus = 0x21;
   IAE_MNGRP_enableCfg( &enabledSlus );
   OS_LYRG_usleep(1000*1000);
   
   enabledSlus |= 0xC;
   IAE_MNGRP_enableCfg( &enabledSlus );
   OS_LYRG_usleep(1000*1000);
   
   enabledSlus |= 0x12;
   IAE_MNGRP_enableCfg( &enabledSlus );
   LOGG_PRINT(LOG_INFO_E, 0, "Enabled Slus - 0x%x enableSLUcount %d\n", enabledSlus,enableSLUcount);
#endif   
#if 1
   for( i = 0; i < (NU4K_MAX_DB_META_PATHS); i++  )
   {
      if (instanceList[i] != 0xFF)
      {
         enabledSlus |= (1 << (instanceList[i] +IAE_DRVG_SLU2_EN_E) );
      }
      /*When an SLU and a parallel SLU share a path, the original code non-atomically enabled both SLUs and this can cause an SLU overflow*/
      if(parallelInstanceList)
      {
         if (parallelInstanceList[i] != 0xFF)
         {
            enabledSlus |= (1 << (parallelInstanceList[i] + IAE_DRVG_SLU0_EN_E));
         }
      }
   }
   if (enabledSlus)
   {
      /*workaround for CVA startup, we will enable SLUs after both Tracking stereo and CVA are started,avoid SLUs overflow*/
      if(IS_CVA_INIT == false && enabledSlus == 0xc)
         return retCode;
   
      IAE_MNGRP_enableCfg( &enabledSlus );
      LOGG_PRINT(LOG_INFO_E, 0, "Enabled Slus - 0x%x\n", enabledSlus);
   }
#endif
   return retCode;
}

/****************************************************************************
*
*  Function Name: IAE_MNGRG_showIaeCfg
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setEnableIau( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle )
{
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;
   UINT8    i = 0;
   UINT32   enabledIaus = 0;
   UINT32   iimControlField, byPassReg, iaeInst;
   (void)hwDb;
   (void)arg;

   HCG_MNGRG_registerHwUnitVote(voterHandle, HCG_MNGRG_IAE);

   LOGG_PRINT(LOG_DEBUG_E, 0, "set IAE ready\n");
   IAE_DRVG_iaeReady();
   
   for( i = 0; i < NU4K_MAX_DB_META_PATHS; i++  )
   {
      if (instanceList[i] <= 2 )
      {
         iimControlField = ( (block == NUFLD_IAU_E) ? (NU4100_IAE_IIM_CONTROL_IAU0_SEL_E + instanceList[i]):(NU4100_IAE_IIM_CONTROL_IAU2_SEL_E + instanceList[i]) );
         //printf("write to %d (%d)\n",iimControlField , block);
         XMLDB_writeFieldToRegFromDb(hwDb, iimControlField);
         enabledIaus |= (1 << instanceList[i]);

         IAE_MNGRP_getIaeBypassReg(hwDb, &byPassReg,instanceList[i]);
         //printf("IAE_MNGRG_setEnableIau bypass reg 0x%x\n",byPassReg);
         iaeInst = (block == NUFLD_IAU_E) ? (instanceList[i]) : (instanceList[i] + 2);
         IAE_DRVG_bypassCfg(iaeInst, byPassReg);
      }
   }

   enabledIaus = enabledIaus << ( (block == NUFLD_IAU_E) ? (IAE_DRVG_IAU0_EN_E):(IAE_DRVG_IAU2_EN_E) );
   IAE_MNGRP_enableCfg( &enabledIaus );

   LOGG_PRINT(LOG_DEBUG_E, 0, "Enabled Iaus - 0x%x\n", enabledIaus);
   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_setDisableGen
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setDisableGen( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;
   UINT32 enable;
   UINT8 i = 0;
   (void)hwDb;
   (void)block;
   (void)arg;
    UINT8* disableAll = (UINT8*)arg;
    
    if (disableAll)
    {
        enable = IAE_DRVG_IAU_DISABLE_ALL_BLOCKS_E;
        IAE_DRVG_disableCfg(&enable);
    }
    else
    {
         for( i = 0; i < NU4K_NUM_GENS; i++ )
         {
            enable = *(instanceList + i);

            LOGG_PRINT(LOG_DEBUG_E, NULL, "IAE_MNGRG_setEnableGen Enabling generator num %d\n", enable);

            if( enable != 0xFF )
            {
               enable += IAE_DRVG_GEN0_EN_E;
               IAE_DRVG_disableCfg(&enable);
            }
         }
    }
   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_setDisableSlu
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setDisableSlu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;
   UINT8    i = 0;
   UINT32   enabledSlus = 0;
   (void)hwDb;
   (void)block;
   (void)arg;
   UINT8* parallelInstanceList = (UINT8*) arg;
   for( i = 0; i < (NU4K_MAX_DB_META_PATHS); i++  )
   {
      if (instanceList[i] != 0xFF)
         enabledSlus |= (1 << (instanceList[i] + IAE_DRVG_SLU2_EN_E));
      /*When an SLU and a parallel SLU share a path, the original code non-atomically enabled both SLUs and this can cause an SLU overflow*/
      if(parallelInstanceList)
      {
         if (parallelInstanceList[i] != 0xFF)
         {
            enabledSlus |= (1 << (parallelInstanceList[i] + IAE_DRVG_SLU0_EN_E));
         }
      }  
   }


   if (enabledSlus)
   {
      IAE_MNGRP_disableCfg( &enabledSlus );
      LOGG_PRINT(LOG_INFO_E, 0, "Disable Slus - 0x%x\n", enabledSlus);
   }
   return retCode;
}

/****************************************************************************
*
*  Function Name: IAE_MNGRG_setDisableIau
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setDisableIau( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg )
{
   UINT8         i = 0;
   UINT32        enabledIaus = 0;
   ERRG_codeE              retCode     = IAE_MNGR__RET_SUCCESS;

   (void)hwDb;
   (void)arg;

   for( i = 0; i < NU4K_MAX_DB_META_PATHS; i++  )
   {
      enabledIaus |= (1 << instanceList[i]);
   }
   
   enabledIaus = enabledIaus << ( (block == NUFLD_IAU_E) ? (IAE_DRVG_IAU0_EN_E):(IAE_DRVG_IAU2_EN_E) );
   IAE_MNGRP_disableCfg( &enabledIaus );
   
   LOGG_PRINT(LOG_DEBUG_E, 0, "Disabled Iaus - 0x%x\n", enabledIaus);

   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGRG_setDisableHist
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE IAE_MNGRG_setDisableHist( UINT8* instanceList )
{
   ERRG_codeE                retCode     = IAE_MNGR__RET_SUCCESS;
   LOGG_PRINT(LOG_INFO_E, NULL, "Disable histogram %d\n",instanceList[0]);
   retCode = IAE_DRVG_setHistInterrupt(instanceList[0], 0);
   return retCode;
}


/****************************************************************************
*
*  Function Name: IAE_MNGR_lut_test_buildmask
*
*  Description: build the mask according to the lut
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
UINT32 IAE_MNGR_lut_test_buildmask(UINT32 lutNum, UINT32 addr)
{
   switch(lutNum)
   {
      case(MEM_MAPG_IAE_LUT_IB_0_0_E):
      case(MEM_MAPG_IAE_LUT_IB_0_1_E):
      case(MEM_MAPG_IAE_LUT_IB_0_2_E):
      case(MEM_MAPG_IAE_LUT_IB_0_3_E):
      case(MEM_MAPG_IAE_LUT_IB_1_0_E):
      case(MEM_MAPG_IAE_LUT_IB_1_1_E):
      case(MEM_MAPG_IAE_LUT_IB_1_2_E):
      case(MEM_MAPG_IAE_LUT_IB_1_3_E):
      case(MEM_MAPG_IAE_LUT_IB_2_0_E):
      case(MEM_MAPG_IAE_LUT_IB_2_1_E):
      case(MEM_MAPG_IAE_LUT_IB_2_2_E):
      case(MEM_MAPG_IAE_LUT_IB_2_3_E):
      case(MEM_MAPG_IAE_LUT_IB_3_0_E):
      case(MEM_MAPG_IAE_LUT_IB_3_1_E):
      case(MEM_MAPG_IAE_LUT_IB_3_2_E):
      case(MEM_MAPG_IAE_LUT_IB_3_3_E):
      {
         if (addr & 0x1)
            return 0xF;
         else
            return 0xFFFFFFFF;
      }
      break;
      case(MEM_MAPG_IAE_LUT_GCR_0_E):
      case(MEM_MAPG_IAE_LUT_GCR_1_E):
         return 0x3FF;
      break;
      case(MEM_MAPG_IAE_LUT_DSR_0_0_E):
      case(MEM_MAPG_IAE_LUT_DSR_0_1_E):
      case(MEM_MAPG_IAE_LUT_DSR_0_2_E):
      case(MEM_MAPG_IAE_LUT_DSR_0_3_E):
      case(MEM_MAPG_IAE_LUT_DSR_1_0_E):
      case(MEM_MAPG_IAE_LUT_DSR_1_1_E):
      case(MEM_MAPG_IAE_LUT_DSR_1_2_E):
      case(MEM_MAPG_IAE_LUT_DSR_1_3_E):
      case(MEM_MAPG_IAE_LUT_DSR_2_0_E):
      case(MEM_MAPG_IAE_LUT_DSR_2_1_E):
      case(MEM_MAPG_IAE_LUT_DSR_2_2_E):
      case(MEM_MAPG_IAE_LUT_DSR_2_3_E):
      case(MEM_MAPG_IAE_LUT_DSR_3_0_E):
      case(MEM_MAPG_IAE_LUT_DSR_3_1_E):
      case(MEM_MAPG_IAE_LUT_DSR_3_2_E):
      case(MEM_MAPG_IAE_LUT_DSR_3_3_E):
      {
         if ((addr & 0x3) == 0x3)
            return 0xFFF;
         else
            return 0xFFFFFFFF;
      }
      break;
   }

   return 0xFFFFFFFF;
}


/****************************************************************************
*
*  Function Name: IAE_MNGR_lut_test
*
*  Description: test for accessing lut using the cpu. write and read simple counter
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static void func_DoneCb(CDE_MNGRG_userCbParamsT *cbParam, void *arg)
{
   printf("DMA MEMCPY DONE %p %p\n",cbParam,arg);
}

#if 0
void IAE_MNGR_lut_test()
{
   MEM_MAPG_addrT    memVirtAddr;
   MEM_MAPG_addrT    memPhyAddr;
   MEM_MAPG_addrT    ddrLutVirtAddr, ddrLutPhyAddr;
   ERRG_codeE        retCode = HW_MNGR__RET_SUCCESS;
   CDE_MNGRG_channnelInfoT *channelInfo;

   LOGG_PRINT(LOG_INFO_E, NULL, "START TEST\n");

   if(ERRG_SUCCEEDED(retCode))
   {
      UINT32 size, reg, lutNum, i;
      UINT32 mask;
     

      for (lutNum = INU_DEFSG_IAE_LUT_IB_0_0_E; lutNum <= INU_DEFSG_IAE_LUT_DSR_3_3_E; lutNum++)
      {
          MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_LUT_SENSOR_IB_0_0_A_E + lutNum, &ddrLutVirtAddr);
          MEM_MAPG_getPhyAddr(MEM_MAPG_DDR_LUT_SENSOR_IB_0_0_A_E + lutNum, &ddrLutPhyAddr);
        //LOGG_PRINT(LOG_INFO_E, NULL, "ddr_pys 0x%x ddr_virt 0x%x lut num %d\n",(UINT32)ddrLutPhyAddr, (UINT32)ddrLutVirtAddr, lutNum);
          MEM_MAPG_getVirtAddr(MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum, &memVirtAddr);
          MEM_MAPG_getPhyAddr(MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum,  &memPhyAddr);
          MEM_MAPG_getSize(MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum,&size);

         //reg = (UINT32)memVirtAddr;
          reg = (UINT32)ddrLutVirtAddr;
          for (i = 0; i < (size/4); i++)
          {
             mask = IAE_MNGR_lut_test_buildmask(MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum,i);
 
             //LOGG_PRINT(LOG_INFO_E, NULL, "write to reg 0x%x (virt) 0x%x val 0x%x ddr_pys 0x%x ddr_virt 0x%x\n",memPhyAddr + (i * 4),memVirtAddr + (i * 4),i,ddrLutPhyAddr, ddrLutVirtAddr);
             *(UINT32*)(reg + (i * 4)) = (i & (mask));
          }
        retCode = CDE_MNGRG_memcpyOpenChan(&channelInfo, size, func_DoneCb, NULL);
        if(ERRG_SUCCEEDED(retCode))
        {
           retCode = CDE_MNGRG_memcpyBlock(channelInfo,(UINT32)memPhyAddr,(UINT32)ddrLutPhyAddr);
           if (ERRG_FAILED(retCode))
           {
              LOGG_PRINT(LOG_ERROR_E, NULL, "DMA memcpy failed src 0x%x dst 0x%x val 0x%x\n",memPhyAddr + (i * 4),i);
           }
           CDE_MNGRG_memcpyCloseChan(channelInfo);
        }
        //LOGG_PRINT(LOG_INFO_E, NULL, "DMA memcpy src 0x%x dst 0x%x val 0x%x\n",memPhyAddr + (i * 4),i);
         }
     
      for (lutNum = INU_DEFSG_IAE_LUT_IB_0_0_E; lutNum <= INU_DEFSG_IAE_LUT_DSR_3_3_E; lutNum++)
      {
         MEM_MAPG_getVirtAddr(MEM_MAPG_DDR_LUT_SENSOR_IB_0_0_A_E + lutNum, &ddrLutVirtAddr);
         MEM_MAPG_getVirtAddr(MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum, &memVirtAddr);
         MEM_MAPG_getSize(MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum,&size);
         for (i = 0; i < (size/4); i++)
         {
            if ((*(UINT32*)(ddrLutVirtAddr + (i*4))) != (*(UINT32*)(memVirtAddr + (i*4))))
            {
                 LOGG_PRINT(LOG_INFO_E, NULL, "lut = %d, lutNum=%d error at reg = 0x%x val = 0x%x (should be 0x%x)\n",MEM_MAPG_IAE_LUT_IB_0_0_E+lutNum,lutNum,memVirtAddr + (i*4),*(UINT32*)(memVirtAddr + (i*4)),*(UINT32*)(ddrLutVirtAddr + (i*4)));
            }
           else
           {
             //LOGG_PRINT(LOG_INFO_E, NULL, "SUCCESS 0x%x 0x%x\n",*(UINT8*)(ddrLutVirtAddr+k),*(UINT8*)(memVirtAddr+k));
           }
         }
       }
      }
    LOGG_PRINT(LOG_INFO_E, NULL, "END TEST\n");
}
#else
void IAE_MNGR_lut_test()
{
   MEM_MAPG_addrT    memVirtAddr;
   MEM_MAPG_addrT    memPhyAddr;
   ERRG_codeE        retCode = HW_MNGR__RET_SUCCESS;

   LOGG_PRINT(LOG_INFO_E, NULL, "START TEST\n");

   if(ERRG_SUCCEEDED(retCode))
   {
      UINT32 size, reg, lutNum, i;
      UINT32 mask;


      for (lutNum = MEM_MAPG_IAE_LUT_IB_0_0_E; lutNum < MEM_MAPG_IAE_LUT_DSR_3_3_E; lutNum++)
      {
         MEM_MAPG_getVirtAddr(lutNum, &memVirtAddr);
         MEM_MAPG_getPhyAddr(lutNum,  &memPhyAddr);
         MEM_MAPG_getSize(lutNum,&size);

         reg = (UINT32)memVirtAddr;
         for (i = 0; i < (size/4); i++)
         {
            mask = IAE_MNGR_lut_test_buildmask(lutNum,i);

            //LOGG_PRINT(LOG_INFO_E, NULL, "write to reg 0x%x val 0x%x\n",memPhyAddr + (i * 4),i);
            *(UINT32*)(reg + (i * 4)) = (i & (mask));
         }

         reg = (UINT32)memVirtAddr;
         for (i = 0; i < (size/4); i++)
         {
            mask = IAE_MNGR_lut_test_buildmask(lutNum,i);

            //LOGG_PRINT(LOG_INFO_E, NULL, "read from reg 0x%x\n",memPhyAddr + (i * 4));
            if (*(UINT32*)(reg + (i * 4)) != (i & mask))
                LOGG_PRINT(LOG_INFO_E, NULL, "lut = %d, error at reg = 0x%x val = 0x%x (should be 0x%x)\n",lutNum,memPhyAddr + (i * 4),*(UINT32*)(reg + (i * 4)),i);
         }
      }
   }

   LOGG_PRINT(LOG_INFO_E, NULL, "END TEST\n");
}
#endif

/****************************************************************************
*
*  Function Name: IAE_MNGR_lut_test_buildmask
*
*  Description: build the mask according to the lut
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static UINT32 ramManagerMode;
void IAE_MNGR_setMemReuse(void)
{
   ramManagerMode = IAE_DRVG_getRamManagerMode(); 
   IAE_DRVG_setRamManagerMode(0);
}

void IAE_MNGR_clearMemReuse(void)
{
   IAE_DRVG_setRamManagerMode(ramManagerMode);
}

#ifdef __cplusplus
   }
#endif

