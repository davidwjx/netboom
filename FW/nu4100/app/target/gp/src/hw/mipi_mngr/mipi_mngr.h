
/****************************************************************************
 *
 *   FileName: mipi_mngr.h
 *
 *   Author: Arnon C.
 *
 *   Date: 
 *
 *   Description: mipi manager
 *   
 ****************************************************************************/
#ifndef MIPI_MNGR_H
#define MIPI_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "mipi_drv.h"
#include "xml_db.h"
#include "nufld.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
#define MIPI_MNGRG_TX_ASYNC          (1 << 0)
#define MIPI_MNGRG_TX_PAD            (1 << 1)
#define MIPI_MNGRG_TX_HDR            (1 << 2)

#define MIPI_MNGRG_MAX_PHY_TO_SENSOR (2)


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef void (*MIPI_MNGRG_txdoneT)(void *arg);

typedef struct
{
   UINT16 hdrSize; //in lines
} MIPI_MNGRG_cfgT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE MIPI_MNGRG_init(void);
ERRG_codeE MIPI_MNGRG_setConfigRx( XMLDB_dbH hwDb, UINT32 *insList, UINT32 mipiCnt, void *arg );
ERRG_codeE MIPI_MNGRG_setEnableRx( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE MIPI_MNGRG_setDisableRx( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 *insList, UINT32 mipiCnt, void *arg );
ERRG_codeE MIPI_MNGRG_getMipiRxTable( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, UINT32 *mipiId, UINT32 *mipiCnt );

void MIPI_MNGRG_deInit(void);
ERRG_codeE MIPI_MNGRG_cfg(INU_DEFSG_mipiInstE inst, mipiInstConfigT *instCfgP);
ERRG_codeE MIPI_MNGRG_cfgVi(mipiViCfgT *cfgP);
ERRG_codeE MIPI_MNGRG_cfgVsc(INU_DEFSG_vscE vsc, mipiVscCfgT *cfgP);
mipiInstConfigT * MIPI_MNGRG_getCfg(INU_DEFSG_mipiInstE inst);
mipiViCfgT *   MIPI_MNGRG_getViCfg(void);
mipiVscCfgT *  MIPI_MNGRG_getVscCfg(INU_DEFSG_vscE vsc);

ERRG_codeE MIPI_MNGRG_tx(INU_DEFSG_mipiInstE inst, UINT8 *bufP, UINT32 len, UINT32 flags, MIPI_MNGRG_txdoneT cb, void *arg);
ERRG_codeE MIPI_MNGRG_txChain(INU_DEFSG_mipiInstE inst, MEM_POOLG_bufDescT *bufP, UINT32 flags, MIPI_MNGRG_txdoneT cb, void *arg);
ERRG_codeE MIPI_MNGRG_stream(INU_DEFSG_mipiInstE      inst, INU_DEFSG_sysChannelIdE ch);
void       MIPI_MNGRG_streamStop(INU_DEFSG_sysChannelIdE ch);
UINT32     MIPI_MNGRG_getHdrLines(void);

void MIPI_MNGRG_showInst(INU_DEFSG_mipiInstE inst, INU_DEFSG_logLevelE logLevel);
void MIPI_MNGRG_showVi(INU_DEFSG_logLevelE logLevel);
void MIPI_MNGRG_showVsc(INU_DEFSG_vscE vsc, INU_DEFSG_logLevelE logLevel);
void MIPI_MNGRG_showCfg(INU_DEFSG_logLevelE logLevel);
void MIPI_MNGRG_showRegs(void);

#ifdef __cplusplus
}
#endif

#endif //   MIPI_MNGR_H






