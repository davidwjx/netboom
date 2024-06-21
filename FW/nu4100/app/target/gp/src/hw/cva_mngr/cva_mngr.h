/****************************************************************************
 *
 *   FileName: cva_mngr.h
 *
 *   Author:  Dima S.
 *
 *   Date:
 *
 *   Description:
 *
 ****************************************************************************/
#ifndef __CVA_MNGR_H__
#define __CVA_MNGR_H__

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "xml_db.h"
#include "nufld.h"
#include "cde_mngr_new.h"
#include "nu4100_cva_regs.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef struct
{
	UINT32 frameCnt;
	UINT32 descrNum;
}CVA_MNGRG_freakFrameParamsT;


typedef enum
{
	CVA_MNGRG_YLINE_OPEN_IIC_READY		= 0x000,
	CVA_MNGRG_YLINE_OPEN_IIC_NOT_READY	= 0x00A,	// 10 is a default value
	CVA_MNGRG_YLINE_CLOSE				= 0x7FF,
}CVA_MNGRG_yLineE;


/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE CVA_MNGRG_init( UINT32 memVirtAddr );
ERRG_codeE CVA_MNGRG_setConfigCva( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE CVA_MNGRG_setConfigDoGFreak( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );

ERRG_codeE CVA_MNGRG_updateIICReady( UINT32 frameNum, UINT32 address );
UINT8 CVA_MNGRG_getIicReady();
void CVA_MNGRG_cvaIicCb(CDE_MNGRG_userCbParamsT *userParams, void *arg);
void CVA_MNGRG_controlFrameDoneCb(CDE_MNGRG_userCbParamsT *userParams, void *arg);
void CVA_MNGRG_dumpRegs(void);

ERRG_codeE CVA_MNGRG_getFreakFrameParams(CVA_MNGRG_freakFrameParamsT* frkParams);
void CVA_MNGRG_setFreakGo(void);
void CVA_MNGRG_setControlReader(void *reader);
void CVA_MNGRG_setEnableCva();
void CVA_MNGRG_setDisableCva();
UINT32 CVA_MNGRG_calcIICCompressedSize(UINT32 width, UINT32 height, UINT8 hybrid);
void CVA_MNGRG_setLeakyOpen(UINT32 go);
UINT8 CVA_MNGRG_isLeakyModeConfigured(void);
UINT8 CVA_MNGRG_isDdrlessConfigured(void);
void CVA_MNGRG_getDdrlessInfo(UINT32 num, UINT32 *addressP, UINT32 *sizeP);

#ifdef __cplusplus
}
#endif

#endif //   __CVA_MNGR_H__

