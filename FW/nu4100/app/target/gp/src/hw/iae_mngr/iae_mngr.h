/****************************************************************************
 *
 *   FileName: iae_mngr.h
 *
 *   Author:  Benny V.
 *
 *   Date:
 *
 *   Description: iae control
 *
 ****************************************************************************/

#ifndef IAE_MNGR_H
#define IAE_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "iae_drv.h"
#include "xml_db.h"
#include "nufld.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

// IAE interupts
typedef enum
{
   IAE_MNGRG_INT_GROUP_RECOVERY_E      = 0,
   IAE_MNGRG_INT_GROUP_SEN_RECOVERY_E  = 1,
   IAE_MNGRG_INT_GROUP_HIST_READY_E    = 2,
   IAE_MNGRG_INT_GROUP_HIST_MISSED_E   = 3,
   IAE_MNGRG_INT_GROUP_MIPI_ERR_E      = 4,
   IAE_MNGRG_NUM_OF_INT_GROUPS_E
}  IAE_MNGRG_interruptGroupsE;


typedef struct
{
   UINT32 slu;
   UINT32 iau;
   UINT32 top;
} IAE_MNGRG_intGroupMaskT;

typedef struct
{
   IAE_MNGRG_intGroupMaskT groupMask;
   BOOL                    groupStatus;
} IAE_MNGRG_intParams;

typedef void (*IAE_MNGRG_histogramCb)(void *argP, UINT64 timestamp, UINT32 hist);
typedef void (*IAE_MNGRG_sluCb)(void *argP, UINT64 timestamp, UINT32 slu);

// IAE data base
typedef struct
{
   //IAE_DRVG_alnCtrlCfgT             alnCtrlCfg;
   //UINT32                           bypassCfg;
   //UINT32                           enableCfg;
   //IAE_DRVG_FrrCfgT                 frrCfg;
   //UINT32                           iimAlnCfg;
   //IAE_DRVG_iaeIimCtrlCfgT          iimCtrlCfg;
   //IAE_DRVG_iaeMipiMuxCfgT          mipiMuxCfg;
   //IAE_DRVG_iaeNlmCfgT              nlmCfg;
   //IAE_DRVG_iaeFGenCfgT             fGenerateCfg[IAE_DRVG_NUM_OF_GEN_SEL_E];
   //IAE_DRVG_iauCfgT                 iauCfg[IAE_DRVG_NUM_OF_IAU_UNITS];
   //IAE_DRVG_sluCfgT                 sluCfg[IAE_DRVG_NUM_OF_INPUT_SENSORS];
   //IAE_DRVG_intCfgT                 intCfg;
   //IAE_MNGRG_intParams              iaeIntParams[IAE_MNGRG_NUM_OF_INT_GROUPS_E];
   IAE_DRVG_setInterruptCbT         setInterruptCb;
   IAE_DRVG_accessIaeRegisterT      accessIaeRegister;
   IAE_MNGRG_histogramCb            histogramCb[2];
   void                             *histogramCbArg[2];
   UINT64                           sluFirstPixelTs[6];
   void                             *sluFirstPixelIsrHandle;
} IAE_MNGRG_iaeInfoT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/


ERRG_codeE IAE_MNGRG_cfg(IAE_MNGRG_iaeInfoT *iaeInfo);
IAE_MNGRG_iaeInfoT *IAE_MNGRG_getIaeInfo();
ERRG_codeE IAE_MNGRG_init( UINT32 memVirtAddr );
ERRG_codeE IAE_MNGRG_deinit( );
ERRG_codeE IAE_MNGRG_openIaeDrv();
ERRG_codeE IAE_MNGRG_closeIaeDrv();
ERRG_codeE IAE_MNGRG_startIae(void);

ERRG_codeE IAE_MNGRG_stopIae();
void       IAE_MNGRG_showIaeCfg(INU_DEFSG_logLevelE level);
ERRG_codeE IAE_MNGRG_setMipiInterrupts(void);
UINT32     IAE_MNGRG_getCiifClk();


ERRG_codeE IAE_MNGRG_setConfigSlu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE IAE_MNGRG_setConfigIau( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE IAE_MNGRG_setConfigGen( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE IAE_MNGRG_setEnableHist( UINT8* instanceList, void *cb, void *argP );



ERRG_codeE IAE_MNGRG_setEnableGen( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE IAE_MNGRG_setEnableSlu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE IAE_MNGRG_setEnableIau( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );

ERRG_codeE IAE_MNGRG_setDisableGen( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
ERRG_codeE IAE_MNGRG_setDisableSlu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
ERRG_codeE IAE_MNGRG_setDisableIau( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
ERRG_codeE IAE_MNGRG_setDisableHist( UINT8* instanceList );



#ifdef __cplusplus
}
#endif

#endif //   IAE_MNGR_H

