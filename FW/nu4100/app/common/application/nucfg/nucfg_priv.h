#ifndef __NUCFG_PRIV_H__
#define __NUCFG_PRIV_H__

#include "xml_db.h"
#include "nucfg.h"
#include "nufld.h"
#include "graph.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
   NUCFG_BLK_GEN_E,              // 0
   NUCFG_BLK_SEN_GROUP_E,        // 1
   NUCFG_BLK_SEN_E,              // 2
   NUCFG_BLK_MEDIATOR_E,         // 3
   NUCFG_BLK_MIPI_RX_E,          // 4
   NUCFG_BLK_PAR_RX_E,           // 5
   NUCFG_BLK_MIPI_MUX_E,         // 6
   NUCFG_BLK_SLU_PARALLEL_E,     // 7
   NUCFG_BLK_SLU_E,              // 8
   NUCFG_BLK_ISP_E,              // 9
   NUCFG_BLK_IAU_E,              // 10
   NUCFG_BLK_IAU_COLOR_E,        // 11
   NUCFG_BLK_HIST_E,             // 12
   NUCFG_BLK_PPU_NOSCL_E,        // 13
   NUCFG_BLK_PPU_SCL_E,          // 14
   NUCFG_BLK_PPU_HYBSCL_E,       // 15
   NUCFG_BLK_PPU_HYB_E,          // 16
   NUCFG_BLK_AXIRD_E,            // 17
   NUCFG_BLK_INTERLEAVER_E,      // 18
   NUCFG_BLK_AXIWR_E,            // 19
   NUCFG_BLK_DPE_E,              // 20
   NUCFG_BLK_DPE_HYB_E,          // 21
   NUCFG_BLK_DPP_E,              // 22
   NUCFG_BLK_CVA_E,              // 23
   NUCFG_BLK_CVARD_E,            // 24
   NUCFG_BLK_DPHY_TX_E,          // 25
   NUCFG_BLK_VSC_CSI_TX_E,       // 26
   NUCFG_BLK_ISPRD_E,            // 27

   NUCFG_NUM_BLKS_E,
} nuBlkTypeE;

////////////////////////////////
typedef struct _NUCFG_socT * NUCFG_socH;
typedef void *nublkH;
//The ID is inteneded to connect this API to the xml tags
typedef const char *NUCFG_hwId;

ERRG_codeE nusoc_init(XMLDB_dbH db, NUCFG_socH *h);
void nusoc_deInit(NUCFG_socH);
unsigned int nusoc_getNumOutputs(NUCFG_socH);
nublkH nusoc_getOutput(NUCFG_socH, unsigned int index);
unsigned int nusoc_getNumInputs(NUCFG_socH);
nublkH nusoc_getInput(NUCFG_socH, unsigned int index);

void nusoc_getBlkTypeInst(NUCFG_socH, nublkH, nuBlkTypeE *type, unsigned int *inst);
NUCFG_hwId nusoc_getBlkHwId(NUCFG_socH, nublkH);
unsigned int nusoc_getBlkNumInputs(NUCFG_socH, nublkH);
void nusoc_getBlkInput(NUCFG_socH, nublkH, unsigned int, nublkH *, unsigned int *);
unsigned int nusoc_getBlkNumOutputs(NUCFG_socH, nublkH);
void nusoc_setDpeFuncParam(NUCFG_socH h, unsigned int inst, XMLModesG_dpeFuncT *dpeFuncParams);
//void nusoc_getBlkOutput(NUCFG_socH, nublkH, unsigned int, nublkH *, unsigned int *);
void nusoc_getBlkOutputRes(NUCFG_socH,nublkH, unsigned int, NUCFG_resT *res);
NUCFG_formatE nusoc_getBlkOutputFormat(NUCFG_socH,nublkH, unsigned int);
NUCFG_formatDiscriptorU nusoc_getBlkOutputFormatDiscriptor(NUCFG_socH,nublkH, unsigned int, NUCFG_formatDiscriptorU *fmtDisc);
unsigned int nusoc_getBlkStr(NUCFG_socH,nublkH, char *str);
int nusoc_isInputBlk(NUCFG_socH, nublkH);
int nusoc_isSameBlk(NUCFG_socH, nublkH, nublkH);
int nusoc_getOutSelect(NUCFG_socH h, nublkH blk_from, nublkH blk_to);
void nusoc_getInterleaveInfoFromDb(NUCFG_socH , nublkH blk, UINT32 *numIntP, NUCFG_interModeE *intMode);
void nusoc_setPathInterleave(NUCFG_socH h, const nublkH blkh, void **arg, int enable);
XMLDB_pathE nusoc_getXmlDbPath(NUCFG_socH h, nuBlkTypeE type);
void nusoc_getChHwId(NUCFG_socH h, nublkH blk, char *buf);
void nusoc_updateBlkSclSize(NUCFG_socH h, nublkH blk, UINT32 hSize, UINT32 vSize, UINT32 *numOfPpuSclOnPath);
ERRG_codeE nusoc_setBlkCropSize(NUCFG_socH h, nublkH blk, unsigned int i, NUCFG_resT *res);
ERRG_codeE nusoc_setIauBypass(NUCFG_socH h, nublkH blk, UINT32 *numIaus);
ERRG_codeE nusoc_setChannelFormat(NUCFG_socH h, nublkH blk, UINT32 channel, NUCFG_changeFromatE format);
ERRG_codeE nusoc_getIspLinkToWriterData(NUCFG_socH h, UINT32 writerId, UINT32* width, UINT32* height, UINT32* ispRdId, UINT32* ispRdNum);

typedef struct
{
   UINT32 sensorOperatingMode;
   UINT32 sensorVerticalOffset;
   INT32 sensorGainOffset;
   //valids
   UINT8 sensorOperatingModeValid;
   UINT8 sensorVerticalOffsetValid;
   UINT8 sensorGainOffsetValid;
} nusoc_updateSensT;

typedef struct
{
  float          d2dFactor;
  float          fc;
}nusoc_updateDispT;

typedef struct
{
   UINT32 verticalSize;
   UINT32 horizontalSize;
   //valids
   UINT8 verticalSizeValid;
   UINT8 horizontalSizeValid;
} nusoc_updateWriterT;

typedef struct
{
   UINT32 verticalCropOffset;
   UINT32 horizontalCropOffset;
   UINT32 sluOutputVerticalSize;
   UINT32 sluOutputHorizontalSize;
   //valids
   UINT8 verticalCropOffsetValid;
   UINT8 horizontalCropOffsetValid;
   UINT8 sluOutputVerticalSizeValid;
   UINT8 sluOutputHorizontalSizeValid;
} nusoc_updateSluT;

typedef struct
{
   UINT32 ibVerticalLutOffset;
   UINT32 ibHorizontalLutOffset;
   UINT32 ibOffset;
   UINT32 ibOffsetR;
   UINT32 ibOffsetGR;
   UINT32 ibOffsetGB;
   UINT32 ibOffsetB;
   UINT32 dsrVerticalLutOffset;
   UINT32 dsrHorizontalLutOffset;
   UINT32 dsrOutputVerticalSize;
   UINT32 dsrOutputHorizontalSize;
   UINT32 dsrVerticalBlockSize;
   UINT32 dsrHorizontalBlockSize;
   UINT32 dsrLutHorizontalSize;
   UINT32 dsrHorizontalDeltaSize;
   UINT32 dsrHorizontalDeltaFractionalBits;
   UINT32 dsrVerticalDeltaSize;
   UINT32 dsrVerticalDeltaFractionalBits;
   UINT32 ibVerticalBlockSize;
   UINT32 ibHorizontalBlockSize;
   UINT32 ibLutHorizontalSize;
   UINT32 ibFixedPointFractionalBits;
   //valids
   UINT8 ibVerticalLutOffsetValid;
   UINT8 ibHorizontalLutOffsetValid;
   UINT8 ibOffsetValid;
   UINT8 ibOffsetRValid;
   UINT8 ibOffsetGRValid;
   UINT8 ibOffsetGBValid;
   UINT8 ibOffsetBValid;
   UINT8 dsrVerticalLutOffsetValid;
   UINT8 dsrHorizontalLutOffsetValid;
   UINT8 dsrOutputVerticalSizeValid;
   UINT8 dsrOutputHorizontalSizeValid;
   UINT8 dsrVerticalBlockSizeValid;
   UINT8 dsrHorizontalBlockSizeValid;
   UINT8 dsrLutHorizontalSizeValid;
   UINT8 dsrHorizontalDeltaSizeValid;
   UINT8 dsrHorizontalDeltaFractionalBitsValid;
   UINT8 dsrVerticalDeltaSizeValid;
   UINT8 dsrVerticalDeltaFractionalBitsValid;
   UINT8 ibVerticalBlockSizeValid;
   UINT8 ibHorizontalBlockSizeValid;
   UINT8 ibLutHorizontalSizeValid;
   UINT8 ibFixedPointFractionalBitsValid;
} nusoc_updateIauT;

typedef union
{
   nusoc_updateSensT sens;
   nusoc_updateSluT slu;
   nusoc_updateIauT iau;
   nusoc_updateWriterT writer;
   nusoc_updateDispT   disptoDepth;
} nusoc_updateT;

void nusoc_updateBlkOutput(NUCFG_socH, nublkH, unsigned int, nusoc_updateT *);

////////////////////////////////
ERRG_codeE nugraph_create(NUCFG_socH, unsigned int outnum, GRAPHG_handleT *gh);
void nugraph_delete(GRAPHG_handleT gh);
void nugraph_show(NUCFG_socH, GRAPHG_handleT gh);
//////////////////////////
const char *nusoc_hwId2Str(NUCFG_hwId);
NUFLD_blkE nusoc_convert2Fld(nuBlkTypeE);
nuBlkTypeE nusoc_convert2Blk(NUFLD_blkE);

XMLDB_dbH NUCFG_getDbH(inu_nucfgH nucfg);
XMLDB_dbH NUCFG_getCurrDbH(inu_nucfgH nucfg);
ERRG_codeE NUCFG_revertDb(inu_nucfgH nucfg);
#ifdef __cplusplus
}
#endif

#endif //__NUCFG_PRIV_H__

