/****************************************************************************
 *
 *   FileName: iae_mngr.c
 *
 *   Author:  Elad R.
 *
 *   Date:
 *
 *   Description: iae control
 *
 ****************************************************************************/

#ifndef PROJECTORS_MNGR_H
#define PROJECTORS_MNGR_H

#ifdef __cplusplus
      extern "C" {
#endif

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "nufld.h"
#include "xml_db.h"
#include "proj_drv.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/

/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
   IO_HANDLE                  projectorHandle;
   INUG_ioctlProjectorConfigT projectorCfg;
} PROJECTORS_MNGRG_projectorInfoT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 *************************************************************************/



void       PROJECTORS_MNGRG_init();

PROJECTORS_MNGRG_projectorInfoT *PROJECTORS_MNGRG_getProjectorInfo(INU_DEFSG_projSelectE projectorSelect);
void       PROJECTORS_MNGRG_showAllProjectorCfg(INU_DEFSG_logLevelE level);
ERRG_codeE PROJECTORS_MNGRG_setConfigProjector( XMLDB_dbH hwDb, NUFLD_blkE block, UINT32 blkInstance, void *arg );
ERRG_codeE PROJECTORS_MNGRG_setEnableProjector( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg, void *voterHandle );
ERRG_codeE PROJECTORS_MNGRG_setDisableProjector( XMLDB_dbH hwDb, NUFLD_blkE block, UINT8* instanceList, void *arg );
ERRG_codeE PROJECTORS_MNGRG_open(IO_HANDLE *ioHandleP,INUG_ioctlProjectorConfigT *projectorConfig);
ERRG_codeE PROJECTORS_MNGRG_close(IO_HANDLE ioHandle);
ERRG_codeE PROJECTORS_MNGRG_startProjector(IO_HANDLE ioHandle);
ERRG_codeE PROJECTORS_MNGRG_stopProjector(IO_HANDLE ioHandle);


#ifdef __cplusplus
}
#endif

#endif //   PROJECTORS_MNGR_H


