/****************************************************************************
 *
 *   FileName: dpe_mngr.h
 *
 *
 *   Description: dpe control
 *
 ****************************************************************************/

#ifndef DPE_MNGR_H
#define DPE_MNGR_H

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

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE DPE_MNGRG_setEnableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg, void *voterHandle);
ERRG_codeE DPE_MNGRG_setDisableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg);
ERRG_codeE DPE_MNGRG_dumpRegs();

////////////////////////////////////////////
//legacy

// DPE interupts
typedef enum
{
   DPE_MNGRG_INT_GROUP_RECOVERY_E   = 0,
   DPE_MNGRG_NUM_OF_INT_GROUPS_E
}  DPE_MNGRG_interruptGroupsE;

typedef struct
{
   UINT32   groupMask;
   BOOL     groupStatus;
} DPE_MNGRG_intParams;

// DPE data base
typedef struct
{
   UINT32                  dpeActivity;
   UINT16                  numActiveChannels;
   UINT16                  dpeInWidth;
   UINT16                  dpeInHeight;
   DPE_MNGRG_intParams     dpeIntParams[DPE_MNGRG_NUM_OF_INT_GROUPS_E];
} DPE_MNGRG_dpeInfoT;


DPE_MNGRG_dpeInfoT *DPE_MNGRG_getDpeInfo();
ERRG_codeE DPE_MNGRG_openDpeDrv();
ERRG_codeE DPE_MNGRG_closeDpeDrv();
ERRG_codeE DPE_MNGRG_setDisparityRange( UINT32 minDisp, UINT32 maxDisp );
ERRG_codeE DPE_MNGRG_disableDpeInterrupts();
ERRG_codeE DPE_MNGRG_clearDpeInterrupts();
void       DPE_MNGRG_showDpeCfg(INU_DEFSG_logLevelE level);

#ifdef __cplusplus
}
#endif

#endif //   DPE_MNGR_H

