#ifndef __DEPTH_POST_MNGR_H__
#define __DEPTH_POST_MNGR_H__

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
}DEPTH_POSTG_cropParamT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/

ERRG_codeE DEPTH_POST_setEnableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg, void *voterHandle );
ERRG_codeE DEPTH_POST_setDisableFunc( XMLDB_dbH dbh, NUFLD_blkE block, UINT8* inst, void *arg );
ERRG_codeE DEPTH_POSTG_updateDPPCrop(DEPTH_POSTG_cropParamT *crop, UINT8 dppMun);
ERRG_codeE DEPTH_POSTG_init(UINT32 ppe_base);


#endif //__DEPTH_POST_MNGR_H__

