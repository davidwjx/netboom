/****************************************************************************
 *
 *   FileName: ppe_mngr.h
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description: ppe control
 *
 ****************************************************************************/

#ifndef __PPE_MNGR_H__
#define __PPE_MNGR_H__

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "xml_db.h"
#include "nufld.h"
/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
	UINT16 xStart;
	UINT16 yStart;
	UINT16 width;
	UINT16 height;
}PPE_MNGRG_cropParamT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE PPE_MNGRG_init( UINT32 memVirtAddr );
void PPE_MNGRG_deinit(void);

ERRG_codeE PPE_MNGRG_setConfigPpu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE PPE_MNGRG_setConfigAxiRd( XMLDB_dbH dbh, NUFLD_blkE block, UINT32 inst, void *arg);
ERRG_codeE PPE_MNGRG_setConfigAxiWr( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );

ERRG_codeE PPE_MNGRG_setEnableAxiWr( XMLDB_dbH dbh, NUFLD_blkE block, UINT8 instance, void *arg, void *voterHandle  );
ERRG_codeE PPE_MNGRG_setEnablePpu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE PPE_MNGRG_setEnableAxiRd( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE PPE_MNGRG_setDisableAxiWr( XMLDB_dbH dbh, NUFLD_blkE block, UINT8 instance, void *arg );
ERRG_codeE PPE_MNGRG_setDisablePpu( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
ERRG_codeE PPE_MNGRG_setDisableAxiRd( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
void PPE_MNGRG_setReaderBurstLen( UINT8 inst, UINT32 len );
void PPE_MNGRG_setWriterBurstLen( UINT8 inst, UINT32 len );


void PPE_MNGRG_dumpRegs(void);



ERRG_codeE PPE_MNGRG_updatePPUCrop(PPE_MNGRG_cropParamT *crop, UINT8 ppuMun);
ERRG_codeE PPE_MNGRG_setRegistersReady(void);

void PPE_MNGRG_getReaderBeatLast(UINT8 inst, UINT32 *beatLast);
void PPE_MNGRG_setReaderBeatLast(UINT8 inst, UINT32 beatLast);
void PPE_MNGRG_getReaderIntStrmEn(UINT8 inst, UINT32 *intStrmEnVal);
void PPE_MNGRG_setReaderIntStrmEn(UINT8 inst, UINT32 val);
void PPE_MNGRG_setReaderSingleEnd(UINT8 inst, UINT32 val);

#ifdef __cplusplus
}
#endif

#endif //   __PPE_MNGR_H__

