#ifndef __NUFLD_H__
#define __NUFLD_H__

#include "xml_db.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum //__attribute__ ((__packed__))
{
   NUFLD_SLU_PARALLEL_E = 0,
   NUFLD_SLU_E          = 1,
   NUFLD_IAU_E          = 2,
   NUFLD_IAU_COLOR_E    = 3,
   NUFLD_HIST_E         = 4,
   NUFLD_SENSOR_E       = 5,
   NUFLD_SENSOR_GROUP_E = 6,
   NUFLD_MEDIATOR_E     = 7,
   NUFLD_PROJECTOR_E    = 8,
   NUFLD_GEN_E          = 9,
   NUFLD_DPE_E          = 10,
   NUFLD_DPP_E          = 11,
   NUFLD_PPU_NOSCL_E    = 12,
   NUFLD_PPU_SCL_E      = 13,
   NUFLD_PPU_HYBSCL_E   = 14,
   NUFLD_PPU_HYB_E      = 15,
   NUFLD_PPU_REPACK_E   = 16,
   NUFLD_AXIRD_E        = 17,
   NUFLD_AXIWR_E        = 18,
   NUFLD_CVARD_E        = 19,
   NUFLD_CVA_E          = 20,
   NUFLD_MIPI_RX_E      = 21,
   NUFLD_MIPI_TX_E      = 22,
   NUFLD_PAR_RX_E       = 23,
   NUFLD_MIPI_MUX_E     = 24,
   NUFLD_PAR_TX_E       = 25,
   NUFLD_DPHY_TX_E      = 26,
   NUFLD_VSC_CSI_TX_E   = 27,
   NUFLD_ISP_E          = 28,
   NUFLD_META_READERS_E = 29,
   NUFLD_META_WRITERS_E = 30,
   NUFLD_META_PATHS_E   = 31,
   NUFLD_META_HIST_E    = 32,
   NUFLD_META_CVA_RD_E  = 33,
   NUFLD_META_MIPI_TX_E = 34,
   NUFLD_META_ISP_RD_E  = 35,
   NUFLD_NUM_BLKS_E     = 36,
   NUFLD_INVALID_E,
}NUFLD_blkE;

UINT32 NUFLD_getNumBlks(NUFLD_blkE blk);
UINT32 NUFLD_getBlkSize(NUFLD_blkE blk, unsigned int blkNum);
XMLDB_pathE NUFLD_startBlockPath(NUFLD_blkE blk, unsigned int blkNum);
XMLDB_pathE NUFLD_calcPath(NUFLD_blkE blk, unsigned int blkNum, XMLDB_pathE blk0field);
void NUFLD_init();

#ifdef __cplusplus
}
#endif
#endif //__NUFLD_H__
