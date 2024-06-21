
#include <math.h>
#include "inu_common.h"
#include "nucfg_priv.h"
#include "nu4k.h"
#include "nu4k_defs.h"
#include "nufld.h"
#include "xml_db_modes.h"

#include <assert.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define DEBUG_DPE_ALG
#define MIN_DELTA_INIT_VAL       (100000)
#define NU4K_MAX_BLK_STRLEN      (16)
#define SIGNED_N_BIT_TO_U32(x, n)  (x & ((1 << n) - 1))
#if 1
#define SET_DPE_AGG_OFFSETS(SIDE,LANE,diff_a_shift, diff_b_shift1, diff_b_shift2, diff_shift, b_sum_diff, h_mask_left, h_mask_right, vmask_down, vmask_up, dyn_mask_en, edge_inf_mode, diff_clip_high, diff_clip_low, cencus_diff_shift1, cencus_diff_shift2, diff_hd_sum) \
   {\
   diff_a_shift =       NU4100_DPE_AGG_##SIDE##_P##LANE##_PXL_DIFF_A_SHIFT_E;\
   diff_b_shift1 =      NU4100_DPE_AGG_##SIDE##_P##LANE##_PXL_DIFF_B_SHIFT1_E;\
   diff_b_shift2 =      NU4100_DPE_AGG_##SIDE##_P##LANE##_PXL_DIFF_B_SHIFT2_E;\
   diff_shift =         NU4100_DPE_AGG_##SIDE##_P##LANE##_PXL_DIFF_DIFF_SHIFT_E;\
   b_sum_diff =         NU4100_DPE_AGG_##SIDE##_P##LANE##_PXL_DIFF_B_SUM_DIFF_E;\
   h_mask_left =        NU4100_DPE_AGG_##SIDE##_P##LANE##_MASK_HMASK_LEFT_E;\
   h_mask_right =       NU4100_DPE_AGG_##SIDE##_P##LANE##_MASK_HMASK_RIGHT_E;\
   vmask_down =         NU4100_DPE_AGG_##SIDE##_P##LANE##_MASK_VMASK_DOWN_E;\
   vmask_up =           NU4100_DPE_AGG_##SIDE##_P##LANE##_MASK_VMASK_UP_E;\
   dyn_mask_en =        NU4100_DPE_AGG_##SIDE##_P##LANE##_MASK_DYN_MSK_EN_E;\
   edge_inf_mode =      NU4100_DPE_AGG_##SIDE##_P##LANE##_CFG_EDGE_INFOMAP_MODE_E;\
   diff_clip_high =     NU4100_DPE_AGG_##SIDE##_P##LANE##_ABS_DIFF_CLIP_HIGH_E;\
   diff_clip_low =      NU4100_DPE_AGG_##SIDE##_P##LANE##_ABS_DIFF_CLIP_LOW_E;\
   cencus_diff_shift1 = NU4100_DPE_AGG_##SIDE##_P##LANE##_CENSUS_DIFF_HD_SHIFT1_E;\
   cencus_diff_shift2 = NU4100_DPE_AGG_##SIDE##_P##LANE##_CENSUS_DIFF_HD_SHIFT2_E;\
   diff_hd_sum =        NU4100_DPE_AGG_##SIDE##_P##LANE##_CENSUS_DIFF_HD_SUM_DIFF_E;\
   }
#define SET_DPP_SCL_OFFSETS(SEL,sclm,sclh,sclv,sclxoff,sclyoff,sclxscl,sclyscl) \
   {\
   sclm = NU4100_PPE_DEPTH_POST_SCL_##SEL##_CFG_SCALE_MODE_E;\
   sclh = NU4100_PPE_DEPTH_POST_SCL_##SEL##_OUT_SIZE_HSIZE_E;\
   sclv = NU4100_PPE_DEPTH_POST_SCL_##SEL##_OUT_SIZE_VSIZE_E;\
   sclxoff = NU4100_PPE_DEPTH_POST_SCL_##SEL##_XOFFSET_OFFSET_E;\
   sclyoff = NU4100_PPE_DEPTH_POST_SCL_##SEL##_YOFFSET_OFFSET_E;\
   sclxscl = NU4100_PPE_DEPTH_POST_SCL_##SEL##_XSCALE_SCALE_E;\
   sclyscl = NU4100_PPE_DEPTH_POST_SCL_##SEL##_YSCALE_SCALE_E;\
   }

#define SET_DPP_CROP_OFFSETS(SEL,crop_xs,crop_xe,crop_ys,crop_ye) \
   {\
   crop_xs = NU4100_PPE_DEPTH_POST_MISC_##SEL##_CROP_START_X_E;\
   crop_xe = NU4100_PPE_DEPTH_POST_MISC_##SEL##_CROP_END_X_E;\
   crop_ys = NU4100_PPE_DEPTH_POST_MISC_##SEL##_CROP_START_Y_E;\
   crop_ye = NU4100_PPE_DEPTH_POST_MISC_##SEL##_CROP_END_Y_E;\
   }

#define SET_DSR_UPDATE_OFFSETS(IAU_TYPE,dsrVertLutOff,dsrHorzLutOff,dsrOutVert,dsrOutHorz,\
   dsrVertBlock, dsrHorzBlock, dsrLutHorzSize, dsrHorzDelta, dsrHorzDeltaFractBits, dsrVertDelta,dsrVertDeltaFractBits) \
   {\
   dsrVertLutOff = NU4100_IAE_##IAU_TYPE##_DSR_LUT_OFFSET_VERTICAL_SIZE_E;\
   dsrHorzLutOff = NU4100_IAE_##IAU_TYPE##_DSR_LUT_OFFSET_HORIZONTAL_SIZE_E;\
   dsrOutVert = NU4100_IAE_##IAU_TYPE##_DSR_OUT_SIZE_VERT_E;\
   dsrOutHorz = NU4100_IAE_##IAU_TYPE##_DSR_OUT_SIZE_HORZ_E;\
   dsrVertBlock = NU4100_IAE_##IAU_TYPE##_DSR_CONTROL_LUT_L_E;\
   dsrHorzBlock = NU4100_IAE_##IAU_TYPE##_DSR_CONTROL_LUT_K_E;\
   dsrLutHorzSize = NU4100_IAE_##IAU_TYPE##_DSR_CONTROL_LUT_H_SIZE_E;\
   dsrHorzDelta = NU4100_IAE_##IAU_TYPE##_DSR_LUT_PREC_DX_SIZE_E;\
   dsrHorzDeltaFractBits = NU4100_IAE_##IAU_TYPE##_DSR_LUT_PREC_DX_PREC_E;\
   dsrVertDelta = NU4100_IAE_##IAU_TYPE##_DSR_LUT_PREC_DY_SIZE_E;\
   dsrVertDeltaFractBits = NU4100_IAE_##IAU_TYPE##_DSR_LUT_PREC_DY_PREC_E;\
   }


#define SET_IB_UPDATE_OFFSETS(IAU_TYPE,ibVertBlock,ibHorzBlock,ibLutHorzSize,ibFixedPointFractBits,\
   ibHorizontalLutOffset, ibVerticalLutOffset, ibOffset, ibOffset0, ibOffset1, ibOffset2, ibOffset3) \
   {\
   ibVertBlock = NU4100_IAE_##IAU_TYPE##_IB_CONTROL_LUT_L_E;\
   ibHorzBlock = NU4100_IAE_##IAU_TYPE##_IB_CONTROL_LUT_K_E;\
   ibLutHorzSize = NU4100_IAE_##IAU_TYPE##_IB_CONTROL_LUT_H_SIZE_E;\
   ibFixedPointFractBits = NU4100_IAE_##IAU_TYPE##_IB_CONTROL_B_PREC_E;\
   ibHorizontalLutOffset = NU4100_IAE_##IAU_TYPE##_IB_LUT_OFFSET_HORIZONTAL_SIZE_E;\
   ibVerticalLutOffset = NU4100_IAE_##IAU_TYPE##_IB_LUT_OFFSET_VERTICAL_SIZE_E;\
   ibOffset = NU4100_IAE_##IAU_TYPE##_IB_OFFSET0_OFFSET_E;\
   ibOffset0 = NU4100_IAE_##IAU_TYPE##_IB_OFFSET0_OFFSET_E;\
   ibOffset1 = NU4100_IAE_##IAU_TYPE##_IB_OFFSET1_OFFSET_E;\
   ibOffset2 = NU4100_IAE_##IAU_TYPE##_IB_OFFSET2_OFFSET_E;\
   ibOffset3 = NU4100_IAE_##IAU_TYPE##_IB_OFFSET3_OFFSET_E;\
   }

#define IS_PX_MODE_ENABLED(lane, p0_p1_mode) ((p0_p1_mode == lane) || (p0_p1_mode == BOTH_P0_P1_ENABLED))
#ifdef DEBUG_DPE_ALG
#define PRINT_SEPARATOR_DPE_ALG printf("--------------------------------\n");
#endif
#else
#define SET_DPP_SCL_OFFSETS(SEL,sclm,sclh,sclv,sclxoff,sclyoff,sclxscl,sclyscl)
#define SET_DPP_CROP_OFFSETS(SEL,crop_xs,crop_xe,crop_ys,crop_ye)
#define SET_DSR_UPDATE_OFFSETS(IAU_TYPE,dsrVertLutOff,dsrHorzLutOff,dsrOutVert,dsrOutHorz)

#endif

#define NU4K_DPP_SRCSEL_IS_PRIMARY(sel) ((sel == NU4K_DPP_SRCSEL_PRIM_DISP) || (sel == NU4K_DPP_SRCSEL_PRIM_DEPTH) || (sel == NU4K_DPP_SRCSEL_REG_PRIM_DEPTH))
#define NU4K_DPP_SRCSEL_IS_SECONDARY(sel) ((sel == NU4K_DPP_SRCSEL_SEC_DISP) || (sel == NU4K_DPP_SRCSEL_SEC_DEPTH))
#define NU4K_PPE_INTERLEAVER_EXISTS(inst) ((inst <= 5) || ((inst >= 8) && (inst <=9)))
#define NU4K_IS_SAME_INSTANCE_AND_TYPE(blk1, blk2) ((blk1->ti.type == blk2->ti.type) && (blk1->ti.inst == blk2->ti.inst))
#define NU4K_IS_PPU_BLK(blk) (((blk == NUCFG_BLK_PPU_NOSCL_E) || (blk == NUCFG_BLK_PPU_SCL_E) || (blk == NUCFG_BLK_PPU_HYBSCL_E) ||(blk == NUCFG_BLK_PPU_HYB_E)) ? 1: 0)
#define NU4K_IS_HYB_PPU_BLK(blk) (((blk == NUCFG_BLK_PPU_HYBSCL_E) ||(blk == NUCFG_BLK_PPU_HYB_E)) ? 1: 0)
#define NU4K_IS_PPU_24BIT_OUTPUT(blkp) ((((blkp->ti.type == NUCFG_BLK_PPU_HYB_E) && ((blkp->ti.inst == 1) || (blkp->ti.inst == 2)))) ? 1: 0)

#define WIN_SIZE   (17)
#define WIN_CENTER (WIN_SIZE / 2)
#define MASK_64_LSB(x) (x & ((1ULL << 32) - 1))
#define MASK_64_MSB(x) ((x >> 32) & ((1ULL << 32) - 1))
#define PI_FOLAT         3.141592653589793     /* pi */
#define MAX_RES_IN_SPARE 3000
#define SUBPIXEL_FACTOR  64
#define MAX_INTERNAL_ROW 1280

typedef enum
{
   NU4K_ANG_22    = 0,
   NU4K_ANG_45    = 1,
   NU4K_ANG_67    = 2,
   // Angles 22,45,67 are the only ones that needed to be rotated, so th
   NU4K_ANG_MAX   = 3,
   NU4K_ANG_0     = 3,
   NU4K_ANG_BOX   = 4
} nu4kAngIdxE;

static UINT8 getFreakInstance[6]={3,5,1,1,5,3}; //according to CVA reader matrix

typedef struct _nuselectT nuselectT;


typedef struct nublkT
{
   NUCFG_hwId hwId;
   struct { nuBlkTypeE type; unsigned int inst;} ti;
   unsigned int numInputs;
   nuselectT *inputs;
   unsigned int numOutputs;
   //nuselectT *outputs;
} nu4kblkT;

typedef struct _nuselectT
{
   nu4kblkT *blkp;
   UINT8 sel;
} nuselectT;

typedef struct
{
   nu4kblkT blk;
   int intMode;
} nuInterT;

typedef struct
{
   nu4kblkT blk;
} nuReaderT;

typedef struct
{
   nu4kblkT blk;
} nuCvaReaderT;

typedef struct
{
   nu4kblkT blk;
   NUCFG_resT userCrop;
} nuPpuT;

typedef struct
{
   nu4kblkT blk;
   NUCFG_resT userCrop;
} nuSluT;

typedef struct
{
   nu4kblkT blk;
} nuIspT;

typedef struct
{
   nu4kblkT blk;
} nuIauT;

typedef struct
{
   nu4kblkT blk;
} nuCvaT;

typedef struct
{
   nu4kblkT blk;
} nuGenT;

typedef struct
{
   nu4kblkT blk;
} nuSensorT;

typedef struct
{
    nu4kblkT blk;
} nuMediatorT;

typedef struct
{
   nu4kblkT blk;
} nuMipiRxT;

typedef struct
{
   nu4kblkT blk;
}nuParRxT;

typedef struct
{
   nu4kblkT blk;
   int deintMode;
} nuWriterT;

typedef struct
{
   nu4kblkT blk;
} nuDpeT;

typedef struct
{
   nu4kblkT blk;
} nuDpeHybT;

typedef struct
{
   nu4kblkT blk;
   NUCFG_resT userCrop;
} nuDppT;

typedef struct
{
   nu4kblkT blk;
} nuDphyTxT;

typedef struct
{
   nu4kblkT blk;
} nuVscCsiTxT;

typedef struct
{
   nu4kblkT blk;
} nuIspReadersT;


enum ppuSelectE
{
   PPU_SEL_NOSCL = 0,
   PPU_SEL_SCL,
   PPU_SEL_HYB_P0,
   PPU_SEL_HYB_P1,
   PPU_SEL_HYBSCL_P0,
   PPU_SEL_HYBSCL_P1,

   PPU_SEL_NUM
};

enum ppuCropFieldE
{
   CROP_START_X= 0,
   CROP_END_X,
   CROP_START_Y,
   CROP_END_Y,
   PPU_CROP_NUM_FIELDS,
};
enum ppuSclFieldE
{
   SCL_MODE= 0,
   SCL_HSIZE,
   SCL_VSIZE,
   SCL_PAD_L,
   SCL_PAD_R,
   SCL_H_IN_VSIZE,
   SCL_XSCALE,
   SCL_YSCALE,
   PPU_SCL_NUM_FIELDS,
};

enum ppuCscFieldE
{
   CSC_BYPASS = 0,
   CSC_MODE,
   CSC_A11,
   CSC_A12,
   CSC_A13,
   CSC_B1,
   CSC_A21,
   CSC_A22,
   CSC_A23,
   CSC_B2,
   CSC_A31,
   CSC_A32,
   CSC_A33,
   CSC_B3,
   PPU_CSC_NUM_FIELDS,
};

enum ppuStitchFieldE
{
   STITCH_BYPASS = 0,
   STITCH_P0_END,
   STITCH_P1_END,
   STITCH_PAD_V_0,
   STITCH_PAD_V_1,
   STITCH_HYB0_START,
   STITCH_HYB0_STOP,
   STITCH_HYB1_START,
   STITCH_HYB1_STOP,
   PPU_STITCH_NUM_FIELDS,
};
enum ppuRepackFieldE
{
   REPACK_BYPASS = 0,
   REPACK_HSIZE,
   REPACK_VSIZE,
   REPACK_OUT_HORZ,
   REPACK0_OUT_BPP,
   REPACK0_OUT_FMT,
   RPK0_CTRL0_STREAM_VECTOR,
   RPK0_CTRL0_SHIFT,
   RPK0_CTRL0_MODE,
   RPK0_MRG_REP_RATE,
   RPK0_MRG_MSK,
   RPK0_MRG_EN,
   RPK0_SHIFT_1_0,
   RPK0_CTRL1_MSK,
   RPK1_CTRL0_STREAM_VECTOR,
   RPK1_CTRL0_SHIFT,
   RPK1_CTRL0_MODE,
   RPK1_CTRL1_MASK,
   REPACK1_OUT_BPP,
   REPACK1_OUT_FMT,
   PPU_REPACK_NUM_FIELDS
};

//Main soc structure
typedef struct _nu4kT
{
   XMLDB_dbH db;
   nuGenT gens[NU4K_NUM_GENS];
   nuSensorT sensors[NU4K_NUM_SENSORS];
   nuMediatorT mediators[NU4K_NUM_MEDIATORS];
   nuMipiRxT mipiRx[NU4K_NUM_MIPI_RX];
   nuParRxT parRx[NU4K_NUM_PAR_RX];
   nuWriterT writers[NU4K_NUM_WRITERS];
   nuSluT slusParallel[NU4K_NUM_SLUS_PARALLEL];
   nuSluT slus[NU4K_NUM_SLUS];
   nuIspT isps[NU4K_NUM_ISPS];
   nuIauT iaus[NU4K_NUM_IAUS];
   nuIauT iausColor[NU4K_NUM_IAUS_COLOR];
   nuIauT hist[NU4K_NUM_HIST];
   nuPpuT ppusNoScl[NU4K_NUM_PPUS_NOSCL];
   nuPpuT ppusScl[NU4K_NUM_PPUS_SCL];
   nuPpuT ppusHybScl[NU4K_NUM_PPUS_HYBSCL];
   nuPpuT ppusHyb[NU4K_NUM_PPUS_HYB];
   nuReaderT readers[NU4K_NUM_AXI_READERS];
   nuCvaReaderT cvaReaders[NU4K_NUM_CVA_READERS];
   nuInterT inters[NU4K_NUM_AXI_INTERLEAVERS];
   nuDpeT dpe[NU4K_NUM_DPES];
   nuDpeT dpeHyb[NU4K_NUM_DPES_HYB];
   nuDppT dpp[NU4K_NUM_DPPS];
   nuCvaT cva[NU4K_NUM_CVAS];
   nuDphyTxT dphyTx[NU4K_NUM_DPHY_TX];
   nuVscCsiTxT vscCsiTx[NU4K_NUM_VSC_CSI_TX];
   nuIspReadersT ispReadersT[NU4K_NUM_ISP_READERS];
   XMLModesG_dpeFuncT dpeFunc;
   char blkStrings[NUCFG_NUM_BLKS_E][NU4K_MAX_BLK_STRLEN];
} nu4kT;

typedef struct
{
   nu4kblkT* prevBlk;
   int       pathIsGoingThroughStitch;
   int       stitcherIsEmpty;
} nu4k_interleaveSetPathT;


static char* blkName[NUCFG_NUM_BLKS_E];

//forward declarations local functions
static unsigned int getOutputBpp(nu4kT *nu4k, nu4kblkT *blk, unsigned int index);
static void getOutputRes(nu4kT *nu4k, nu4kblkT *blk,unsigned int index, unsigned int *horzP, unsigned int *vertP);
static NUCFG_formatE getOutputFormat(nu4kT *nu4k, nu4kblkT *blk,unsigned int index);
static NUCFG_formatDiscriptorU getOutputFormatDiscriptor(nu4kT *nu4k, nu4kblkT *blk,unsigned int index, NUCFG_formatDiscriptorU *fmtDisc);
static NUCFG_formatE getPpuOutFormat(nu4kT *nu4k, nu4kblkT *blk, unsigned int index);
static void getPpuRepackOut(nu4kT *nu4k, nu4kblkT *blk, int p, unsigned int pkrNum,
   unsigned int *horzp, unsigned int *bpp, NUCFG_formatE *fmt);
static void getInput(nu4kT *nu4k, nu4kblkT *blk0, unsigned int blk0_index, nu4kblkT **blk1, unsigned int *blk1_index);

static NUCFG_formatE slu2nucfgFmt[SLU_NUM_FORMATS_E];
static NUCFG_formatE iau2nucfgFmt[IAU_NUM_FORMATS_E];
static NUCFG_formatE writer2nucfgFmt[WRITER_NUM_FORMATS_E];
static NUCFG_formatE ppu2nucfgFmt[PPU_NUM_FORMATS_E];
static nu4kPpuOutFormatE nucfgFmt2ppu[NUCFG_FORMAT_NUM_FORMATS_E];
static nu4kCsiOutFormatE nucfgFmt2csi[NUCFG_FORMAT_NUM_FORMATS_E];

static NUCFG_formatE dpe2nucfgFmt[NU4K_DPE_NUM_FORMATS_E];
static nu4k_interleaveSetPathT interleaveSetPath;

static XMLDB_pathE ppuCropTbls[PPU_SEL_NUM][PPU_CROP_NUM_FIELDS];
static XMLDB_pathE ppuSclTbls[PPU_SEL_NUM][PPU_SCL_NUM_FIELDS];
static XMLDB_pathE ppuStitchTbls[PPU_SEL_NUM][PPU_STITCH_NUM_FIELDS];
static XMLDB_pathE ppuRepackTbls[PPU_SEL_NUM][PPU_REPACK_NUM_FIELDS];
static XMLDB_pathE ppuCscTbls[PPU_SEL_NUM][PPU_CSC_NUM_FIELDS];
static XMLDB_pathE nucfgMetaStartPathTbl[NUCFG_NUM_BLKS_E];


static void initDpeFormatTbl(void)
{
   int i;
   for(i = 0; i < NU4K_DPE_NUM_FORMATS_E; i++)
      dpe2nucfgFmt[i] = NUCFG_FORMAT_DISPARITY_DEBUG_E;

   dpe2nucfgFmt[NU4K_DPE_OUTSEL_MAIN_E] = NUCFG_FORMAT_DISPARITY_E;
}

static void initMetaStartPathTbl(void)
{
   nucfgMetaStartPathTbl[NUCFG_BLK_GEN_E] = META_PATHS_PATH_0_GEN_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_SEN_GROUP_E] = META_PATHS_PATH_0_SENS_GROUP_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_SEN_E] = META_PATHS_PATH_0_SENS_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_MEDIATOR_E] = META_PATHS_PATH_0_MEDIATOR_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_MIPI_RX_E] = META_PATHS_PATH_0_MIPI_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_PAR_RX_E] = META_PATHS_PATH_0_PARALLEL_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_SLU_PARALLEL_E] = META_PATHS_PATH_0_SLU_PARALLEL_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_SLU_E] = META_PATHS_PATH_0_SLU_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_ISP_E] = META_PATHS_PATH_0_ISP_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_IAU_E] = META_PATHS_PATH_0_IAU_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_IAU_COLOR_E] = META_PATHS_PATH_0_IAU_COLOR_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_HIST_E] = META_PATHS_PATH_0_HIST_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_PPU_NOSCL_E] = META_PATHS_PATH_0_PPU_NOSCL_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_PPU_SCL_E] = META_PATHS_PATH_0_PPU_SCL_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_PPU_HYBSCL_E] = META_PATHS_PATH_0_PPU_HYBSCL_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_PPU_HYB_E] = META_PATHS_PATH_0_PPU_HYB_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_AXIRD_E] = META_PATHS_PATH_0_RD_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_DPE_E] = META_PATHS_PATH_0_DPE_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_DPE_HYB_E] = XMLDB_NUM_PATHS_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_DPP_E] = META_PATHS_PATH_0_DEPTH_POST_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_INTERLEAVER_E] = META_PATHS_PATH_0_RDOUT_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_CVA_E] = META_PATHS_PATH_0_CVA_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_AXIWR_E] = META_PATHS_PATH_0_INJECTION_E;
   // TODO: what to do with META cva rdout
   nucfgMetaStartPathTbl[NUCFG_BLK_CVARD_E] = META_PATHS_PATH_0_CVA_RDOUT_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_VSC_CSI_TX_E] = META_PATHS_PATH_0_VSC_CSI_TX_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_DPHY_TX_E] = META_PATHS_PATH_0_DPHY_TX_E;
   nucfgMetaStartPathTbl[NUCFG_BLK_ISPRD_E] = META_PATHS_PATH_0_ISP_RDOUT_E;
}


static void initPpuFormatTbl(void)
{
   ppu2nucfgFmt[PPU_FORMAT_YUV422_E ] = NUCFG_FORMAT_YUV422_10BIT_E;
   ppu2nucfgFmt[PPU_FORMAT_RGB_E] = NUCFG_FORMAT_RGB888_E;
   ppu2nucfgFmt[PPU_FORMAT_GREY_E] = NUCFG_FORMAT_GREY_16_E;
   ppu2nucfgFmt[PPU_FORMAT_NA_E] = NUCFG_FORMAT_NA_E;

   nucfgFmt2ppu[NUCFG_FORMAT_GREY_16_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_BAYER_16_E] = PPU_FORMAT_RGB_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RGB888_E] = PPU_FORMAT_RGB_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RGB666_E] = PPU_FORMAT_RGB_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RGB565_E] = PPU_FORMAT_RGB_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RGB555_E] = PPU_FORMAT_RGB_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RGB444_E] = PPU_FORMAT_RGB_E;
   nucfgFmt2ppu[NUCFG_FORMAT_YUV420_8BIT_E] = PPU_FORMAT_YUV422_E;
   nucfgFmt2ppu[NUCFG_FORMAT_YUV420_8BIT_LEGACY_E] = PPU_FORMAT_YUV422_E;
   nucfgFmt2ppu[NUCFG_FORMAT_YUV420_10BIT_E] = PPU_FORMAT_YUV422_E;
   nucfgFmt2ppu[NUCFG_FORMAT_YUV422_8BIT_E] = PPU_FORMAT_YUV422_E;
   nucfgFmt2ppu[NUCFG_FORMAT_YUV422_10BIT_E] = PPU_FORMAT_YUV422_E;
   nucfgFmt2ppu[NUCFG_FORMAT_YUV420_SEMI_PLANAR_E] = PPU_FORMAT_YUV422_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RAW6_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RAW7_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RAW8_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RAW10_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RAW12_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_RAW14_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_8_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_12_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_16_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_24_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_32_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_64_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_96_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_GEN_672_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_DEPTH_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_DISPARITY_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_DISPARITY_DEBUG_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_HISTOGRAM_E] = PPU_FORMAT_GREY_E;
   nucfgFmt2ppu[NUCFG_FORMAT_NA_E] = PPU_FORMAT_GREY_E;
}

static void initCsiFormatTbl(void)
{
   nucfgFmt2csi[NUCFG_FORMAT_GREY_16_E] = RGB565;
   nucfgFmt2csi[NUCFG_FORMAT_BAYER_16_E] = RGB565;
   nucfgFmt2csi[NUCFG_FORMAT_RGB888_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_RGB666_E] = RGB666;
   nucfgFmt2csi[NUCFG_FORMAT_RGB565_E] = RGB565;
   nucfgFmt2csi[NUCFG_FORMAT_RGB555_E] = RGB555;
   nucfgFmt2csi[NUCFG_FORMAT_RGB444_E] = RGB444;
   nucfgFmt2csi[NUCFG_FORMAT_YUV420_SEMI_PLANAR_E] = YUV420_8;
   nucfgFmt2csi[NUCFG_FORMAT_YUV420_8BIT_E] = YUV420_8;
   nucfgFmt2csi[NUCFG_FORMAT_YUV420_8BIT_LEGACY_E] = YUV420_8_LEGACY;
   nucfgFmt2csi[NUCFG_FORMAT_YUV420_10BIT_E] = YUV420_10;
   nucfgFmt2csi[NUCFG_FORMAT_YUV422_8BIT_E] = YUV422_8;
   nucfgFmt2csi[NUCFG_FORMAT_YUV422_10BIT_E] = YUV422_10;
   nucfgFmt2csi[NUCFG_FORMAT_RAW6_E] = RAW6;
   nucfgFmt2csi[NUCFG_FORMAT_RAW7_E] = RAW7;
   nucfgFmt2csi[NUCFG_FORMAT_RAW8_E] = RAW8;
   nucfgFmt2csi[NUCFG_FORMAT_RAW10_E] = RAW10;
   nucfgFmt2csi[NUCFG_FORMAT_RAW12_E] = RAW12;
   nucfgFmt2csi[NUCFG_FORMAT_RAW14_E] = RAW14;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_8_E] = RAW8;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_12_E] = RAW12;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_16_E] = RGB565;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_24_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_32_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_64_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_96_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_GEN_672_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_DEPTH_E] = RAW12;
   nucfgFmt2csi[NUCFG_FORMAT_DISPARITY_E] = RAW12;
   nucfgFmt2csi[NUCFG_FORMAT_DISPARITY_DEBUG_E] = RAW12;
   nucfgFmt2csi[NUCFG_FORMAT_HISTOGRAM_E] = RGB888;
   nucfgFmt2csi[NUCFG_FORMAT_NA_E] = RGB888;
}


static void initWriterFormatTbl(void)
{
   int i;
   for(i = 0; i < WRITER_NUM_FORMATS_E; i++)  writer2nucfgFmt[i] = NUCFG_FORMAT_NA_E;

   writer2nucfgFmt[WRITER_FORMAT_RGB888_E] = NUCFG_FORMAT_RGB888_E;
   writer2nucfgFmt[WRITER_FORMAT_RGB565_E] = NUCFG_FORMAT_RGB565_E;
   writer2nucfgFmt[WRITER_FORMAT_RGB666_E] = NUCFG_FORMAT_RGB666_E;
   writer2nucfgFmt[WRITER_FORMAT_RGB555_E] = NUCFG_FORMAT_RGB555_E;
   writer2nucfgFmt[WRITER_FORMAT_RGB444_E] = NUCFG_FORMAT_RGB444_E;
   writer2nucfgFmt[WRITER_FORMAT_YUV420_SEMI_PLANAR_E] = NUCFG_FORMAT_YUV420_SEMI_PLANAR_E;
   writer2nucfgFmt[WRITER_FORMAT_YUV422_8B_E] = NUCFG_FORMAT_YUV422_8BIT_E;
   writer2nucfgFmt[WRITER_FORMAT_YUV422_10B_E] = NUCFG_FORMAT_YUV422_10BIT_E;
   writer2nucfgFmt[WRITER_FORMAT_RAW8_E] = NUCFG_FORMAT_RAW8_E;
   writer2nucfgFmt[WRITER_FORMAT_RAW6_E] = NUCFG_FORMAT_RAW6_E;
   writer2nucfgFmt[WRITER_FORMAT_RAW7_E] = NUCFG_FORMAT_RAW7_E;
   writer2nucfgFmt[WRITER_FORMAT_RAW10_E] = NUCFG_FORMAT_RAW10_E;
   writer2nucfgFmt[WRITER_FORMAT_RAW12_E] = NUCFG_FORMAT_RAW12_E;
   writer2nucfgFmt[WRITER_FORMAT_RAW14_E] = NUCFG_FORMAT_RAW14_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN8_E] = NUCFG_FORMAT_GEN_8_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN12_E] = NUCFG_FORMAT_GEN_12_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN16_E] = NUCFG_FORMAT_GEN_16_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN24_E] = NUCFG_FORMAT_GEN_24_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN32_E] = NUCFG_FORMAT_GEN_32_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN64_E] = NUCFG_FORMAT_GEN_64_E;
   writer2nucfgFmt[WRITER_FORMAT_GEN96_E] = NUCFG_FORMAT_GEN_96_E;
}

static void initIauFormatTbl(void)
{
   int i;
   for(i = 0; i < IAU_NUM_FORMATS_E; i++) iau2nucfgFmt[i] = NUCFG_FORMAT_NA_E;

   iau2nucfgFmt[IAU_FORMAT_GREY_E] = NUCFG_FORMAT_GREY_16_E;
   iau2nucfgFmt[IAU_FORMAT_BAYER_E] = NUCFG_FORMAT_BAYER_16_E;
   iau2nucfgFmt[IAU_FORMAT_RGB_E] = NUCFG_FORMAT_RGB888_E;
   iau2nucfgFmt[IAU_FORMAT_YUV422_E] = NUCFG_FORMAT_YUV422_10BIT_E;
   iau2nucfgFmt[IAU_FORMAT_YUV420L_E] = NUCFG_FORMAT_YUV420_8BIT_LEGACY_E;
   iau2nucfgFmt[IAU_FORMAT_YUV420_E] = NUCFG_FORMAT_YUV420_8BIT_E;
}

static void initSluFormatTbl(void)
{
   int i;
   for(i = 0; i < SLU_NUM_FORMATS_E; i++) slu2nucfgFmt[i] = NUCFG_FORMAT_NA_E;

   slu2nucfgFmt[SLU_FORMAT_RGB888_E] = NUCFG_FORMAT_RGB888_E;
   slu2nucfgFmt[SLU_FORMAT_RGB666_E] = NUCFG_FORMAT_RGB666_E;
   slu2nucfgFmt[SLU_FORMAT_RGB565_E] = NUCFG_FORMAT_RGB565_E;
   slu2nucfgFmt[SLU_FORMAT_RGB555_E] = NUCFG_FORMAT_RGB555_E;
   slu2nucfgFmt[SLU_FORMAT_RGB444_E] = NUCFG_FORMAT_RGB444_E;
   slu2nucfgFmt[SLU_FORMAT_YUV422_8B_E] = NUCFG_FORMAT_YUV422_8BIT_E;
   slu2nucfgFmt[SLU_FORMAT_YUV422_10B_E] = NUCFG_FORMAT_YUV422_10BIT_E;
   slu2nucfgFmt[SLU_FORMAT_YUV420_8B_LEGACY_E] = NUCFG_FORMAT_YUV420_8BIT_LEGACY_E;
   slu2nucfgFmt[SLU_FORMAT_YUV420_8B_E] = NUCFG_FORMAT_YUV420_8BIT_E;
   slu2nucfgFmt[SLU_FORMAT_YUV420_10B_E] = NUCFG_FORMAT_YUV420_10BIT_E;
   slu2nucfgFmt[SLU_FORMAT_RAW6_E] = NUCFG_FORMAT_RAW6_E;
   slu2nucfgFmt[SLU_FORMAT_RAW7_E] = NUCFG_FORMAT_RAW7_E;
   slu2nucfgFmt[SLU_FORMAT_RAW8_E] = NUCFG_FORMAT_RAW8_E;;
   slu2nucfgFmt[SLU_FORMAT_RAW10_E] = NUCFG_FORMAT_RAW10_E;
   slu2nucfgFmt[SLU_FORMAT_RAW12_E] = NUCFG_FORMAT_RAW12_E;
   slu2nucfgFmt[SLU_FORMAT_RAW14_E] = NUCFG_FORMAT_RAW14_E;
}

static inline NUCFG_formatE nu4k_slu2nucfgFmt(nu4kSluFormatE fmt)
{
   return slu2nucfgFmt[fmt];
}
static inline NUCFG_formatE nu4k_iau2nucfgFmt(nu4kIauFormatE fmt)
{
   return iau2nucfgFmt[fmt];
}
static inline NUCFG_formatE nu4k_writer2nucfgFmt(nu4kWriterFormatE fmt)
{
   return writer2nucfgFmt[fmt];
}
static NUCFG_formatE nu4k_mipi2nucfgFmt(nu4kCsiOutFormatE fmt)
{
    switch (fmt)
    {
    case YUV420_8_LEGACY:
        return NUCFG_FORMAT_YUV420_8BIT_LEGACY_E;
        break;
    case YUV420_8:
        return NUCFG_FORMAT_YUV420_8BIT_E;
        break;
    case YUV420_10:
        return NUCFG_FORMAT_YUV420_10BIT_E;
        break;
    case RGB444:
        return NUCFG_FORMAT_RGB444_E;
        break;
    case RGB555:
        return NUCFG_FORMAT_RGB555_E;
        break;
    case RGB565:
        return NUCFG_FORMAT_RGB565_E;
        break;
    case RGB666:
        return NUCFG_FORMAT_RGB666_E;
        break;
    case RGB888:
        return NUCFG_FORMAT_RGB888_E;
        break;
    case RAW6:
        return NUCFG_FORMAT_RAW6_E;
        break;
    case RAW7:
        return NUCFG_FORMAT_RAW7_E;
        break;
    case YUV422_8:
        return NUCFG_FORMAT_YUV422_8BIT_E;
        break;
    case YUV422_10:
        return NUCFG_FORMAT_YUV422_10BIT_E;
        break;
    case RAW8:
        return NUCFG_FORMAT_RAW8_E;
        break;
    case RAW10:
        return NUCFG_FORMAT_RAW10_E;
        break;
    case RAW12:
        return NUCFG_FORMAT_RAW12_E;
        break;
    case RAW14:
        return NUCFG_FORMAT_RAW14_E;
        break;
    default:
        return NUCFG_FORMAT_NA_E;
        break;
    }
}
#if 0
static inline NUCFG_formatE nu4k_ppu2nucfgFmt(nu4kPpuOutFormatE fmt)
{
   return ppu2nucfgFmt[fmt];
}
#endif
static inline NUCFG_formatE nu4k_dpe2nucfgFmt(nu4kDpeOutFormatE fmt)
{
   return dpe2nucfgFmt[fmt];
}

static ERRG_codeE getDbField(nu4kT *nu4k, NUFLD_blkE blk,unsigned int blk_num,XMLDB_pathE field, UINT32 *valP)
{
   return XMLDB_getValue(nu4k->db, NUFLD_calcPath(blk, blk_num, field), valP);
}
static void setDbField(nu4kT *nu4k, NUFLD_blkE blk,unsigned int blk_num,XMLDB_pathE field, UINT32 val)
{
   XMLDB_setValue(nu4k->db, NUFLD_calcPath(blk, blk_num, field), val);
}

static enum ppuSelectE ppuType2Sel(nuBlkTypeE type, int p)
{
   enum ppuSelectE sel = PPU_SEL_NOSCL;
   if(type == NUCFG_BLK_PPU_NOSCL_E) sel = PPU_SEL_NOSCL;
   else if(type == NUCFG_BLK_PPU_SCL_E) sel = PPU_SEL_SCL;
   else if((type == NUCFG_BLK_PPU_HYBSCL_E) && (p == 0)) sel = PPU_SEL_HYBSCL_P0;
   else if((type == NUCFG_BLK_PPU_HYBSCL_E) && (p == 1)) sel = PPU_SEL_HYBSCL_P1;
   else if((type == NUCFG_BLK_PPU_HYB_E) && (p == 0)) sel = PPU_SEL_HYB_P0;
   else if((type == NUCFG_BLK_PPU_HYB_E) && (p == 1)) sel = PPU_SEL_HYB_P1;
   return sel;
}

static void ppuCropTblSet(enum ppuSelectE sel, XMLDB_pathE start_x, XMLDB_pathE end_x, XMLDB_pathE start_y, XMLDB_pathE end_y)
{
   ppuCropTbls[sel][CROP_START_X] = start_x;
   ppuCropTbls[sel][CROP_END_X]   = end_x;
   ppuCropTbls[sel][CROP_START_Y] = start_y;
   ppuCropTbls[sel][CROP_END_Y]   = end_y;
}
static void ppuCropTblsInit(void)
{
   ppuCropTblSet(PPU_SEL_NOSCL,     NU4100_PPE_PPU0_MISC_CROP_START_X_E, NU4100_PPE_PPU0_MISC_CROP_END_X_E,
                                    NU4100_PPE_PPU0_MISC_CROP_START_Y_E, NU4100_PPE_PPU0_MISC_CROP_END_Y_E);
   ppuCropTblSet(PPU_SEL_SCL,       NU4100_PPE_PPU4_MISC_CROP_START_X_E, NU4100_PPE_PPU4_MISC_CROP_END_X_E,
                                    NU4100_PPE_PPU4_MISC_CROP_START_Y_E, NU4100_PPE_PPU4_MISC_CROP_END_Y_E);
   ppuCropTblSet(PPU_SEL_HYB_P0,    NU4100_PPE_PPU8_MISC0_CROP_START_X_E, NU4100_PPE_PPU8_MISC0_CROP_END_X_E,
                                    NU4100_PPE_PPU8_MISC0_CROP_START_Y_E, NU4100_PPE_PPU8_MISC0_CROP_END_Y_E);
   ppuCropTblSet(PPU_SEL_HYB_P1,    NU4100_PPE_PPU8_MISC1_CROP_START_X_E, NU4100_PPE_PPU8_MISC1_CROP_END_X_E,
                                    NU4100_PPE_PPU8_MISC1_CROP_START_Y_E, NU4100_PPE_PPU8_MISC1_CROP_END_Y_E);
   ppuCropTblSet(PPU_SEL_HYBSCL_P0, NU4100_PPE_PPU7_MISC0_CROP_START_X_E, NU4100_PPE_PPU7_MISC0_CROP_END_X_E,
                                    NU4100_PPE_PPU7_MISC0_CROP_START_Y_E, NU4100_PPE_PPU7_MISC0_CROP_END_Y_E);
   ppuCropTblSet(PPU_SEL_HYBSCL_P1, NU4100_PPE_PPU7_MISC1_CROP_START_X_E, NU4100_PPE_PPU7_MISC1_CROP_END_X_E,
                                    NU4100_PPE_PPU7_MISC1_CROP_START_Y_E, NU4100_PPE_PPU7_MISC1_CROP_END_Y_E);
}
static XMLDB_pathE *ppuCropTblSelect(nuBlkTypeE type, int p)
{
   //if asked ppu with scaler but not hybrid, p=1 not relevent - return null
   if ((type == NUCFG_BLK_PPU_SCL_E) && (p == 1))
      return NULL;

   return ppuCropTbls[ppuType2Sel(type,p)];
}

static void ppuSclTblSet(enum ppuSelectE sel, XMLDB_pathE scl_mode, XMLDB_pathE hsize, XMLDB_pathE vsize,
      XMLDB_pathE padl, XMLDB_pathE padr, XMLDB_pathE h_in_vsize, XMLDB_pathE xscale, XMLDB_pathE yscale)
{
   ppuSclTbls[sel][SCL_MODE]  = scl_mode;
   ppuSclTbls[sel][SCL_HSIZE] = hsize;
   ppuSclTbls[sel][SCL_VSIZE] = vsize;
   ppuSclTbls[sel][SCL_PAD_L] = padl;
   ppuSclTbls[sel][SCL_PAD_R] = padr;
   ppuSclTbls[sel][SCL_H_IN_VSIZE] = h_in_vsize;
   ppuSclTbls[sel][SCL_XSCALE] = xscale;
   ppuSclTbls[sel][SCL_YSCALE] = yscale;
}
static void ppuSclTblsInit(void)
{
   ppuSclTblSet(PPU_SEL_SCL, NU4100_PPE_PPU4_SCL_CFG_SCALE_MODE_E,
       NU4100_PPE_PPU4_SCL_OUT_SIZE_HSIZE_E,
       NU4100_PPE_PPU4_SCL_OUT_SIZE_VSIZE_E,
       NU4100_PPE_PPU4_SCL_PAD_CFG_PAD_LEFT_E,
       NU4100_PPE_PPU4_SCL_PAD_CFG_PAD_RIGHT_E,
       NU4100_PPE_PPU4_SCL_CFG_H_IN_VSIZE_E,
      NU4100_PPE_PPU4_SCL_XSCALE_SCALE_E,
      NU4100_PPE_PPU4_SCL_YSCALE_SCALE_E);
   ppuSclTblSet(PPU_SEL_HYBSCL_P0, NU4100_PPE_PPU7_SCL0_CFG_SCALE_MODE_E,
       NU4100_PPE_PPU7_SCL0_OUT_SIZE_HSIZE_E,
       NU4100_PPE_PPU7_SCL0_OUT_SIZE_VSIZE_E,
       NU4100_PPE_PPU7_SCL0_PAD_CFG_PAD_LEFT_E,
       NU4100_PPE_PPU7_SCL0_PAD_CFG_PAD_RIGHT_E,
       NU4100_PPE_PPU7_SCL0_CFG_H_IN_VSIZE_E,
      NU4100_PPE_PPU7_SCL0_XSCALE_SCALE_E,
      NU4100_PPE_PPU7_SCL0_YSCALE_SCALE_E);
   ppuSclTblSet(PPU_SEL_HYBSCL_P1, NU4100_PPE_PPU7_SCL1_CFG_SCALE_MODE_E,
       NU4100_PPE_PPU7_SCL1_OUT_SIZE_HSIZE_E,
       NU4100_PPE_PPU7_SCL1_OUT_SIZE_VSIZE_E,
       NU4100_PPE_PPU7_SCL1_PAD_CFG_PAD_LEFT_E,
       NU4100_PPE_PPU7_SCL1_PAD_CFG_PAD_RIGHT_E,
       NU4100_PPE_PPU7_SCL1_CFG_H_IN_VSIZE_E,
      NU4100_PPE_PPU7_SCL1_XSCALE_SCALE_E,
      NU4100_PPE_PPU7_SCL1_YSCALE_SCALE_E);
}

XMLDB_pathE *ppuCscTblSelect(nuBlkTypeE type, int p)
{
   //if asked ppu with scaler but not hybrid, p=1 not relevent - return null
   if (((type == NUCFG_BLK_PPU_NOSCL_E) && (p == 1))|| ((type == NUCFG_BLK_PPU_SCL_E) && (p == 1)))
      return NULL;

   return ppuCscTbls[ppuType2Sel(type, p)];
}
static void ppuCscTblSet(enum ppuSelectE sel, XMLDB_pathE csc_bypass, XMLDB_pathE csc_mode, XMLDB_pathE a11, XMLDB_pathE a12,
   XMLDB_pathE a13, XMLDB_pathE b1, XMLDB_pathE a21, XMLDB_pathE a22, XMLDB_pathE a23, XMLDB_pathE b2,
   XMLDB_pathE a31, XMLDB_pathE a32, XMLDB_pathE a33, XMLDB_pathE b3)
{
   ppuCscTbls[sel][CSC_BYPASS] = csc_bypass;
   ppuCscTbls[sel][CSC_MODE] = csc_mode;
   ppuCscTbls[sel][CSC_A11] = a11;
   ppuCscTbls[sel][CSC_A12] = a12;
   ppuCscTbls[sel][CSC_A13] = a13;
   ppuCscTbls[sel][CSC_B1] = b1;
   ppuCscTbls[sel][CSC_A21] = a21;
   ppuCscTbls[sel][CSC_A22] = a22;
   ppuCscTbls[sel][CSC_A23] = a23;
   ppuCscTbls[sel][CSC_B2] = b2;
   ppuCscTbls[sel][CSC_A31] = a31;
   ppuCscTbls[sel][CSC_A32] = a32;
   ppuCscTbls[sel][CSC_A33] = a33;
   ppuCscTbls[sel][CSC_B3] = b3;
}
static void ppuCscTblsInit(void)
{
   ppuCscTblSet(PPU_SEL_NOSCL, NU4100_PPE_PPU0_MISC_PPU_CTRL_CSC_BYPASS_E,
      NU4100_PPE_PPU0_MISC_PPU_CTRL_CSC_MODE_E,
      NU4100_PPE_PPU0_MISC_CSC1_A11_E,
      NU4100_PPE_PPU0_MISC_CSC1_A12_E,
      NU4100_PPE_PPU0_MISC_CSC2_A13_E,
      NU4100_PPE_PPU0_MISC_CSC2_B1_E,
      NU4100_PPE_PPU0_MISC_CSC3_A21_E,
      NU4100_PPE_PPU0_MISC_CSC3_A22_E,
      NU4100_PPE_PPU0_MISC_CSC4_A23_E,
      NU4100_PPE_PPU0_MISC_CSC4_B2_E,
      NU4100_PPE_PPU0_MISC_CSC5_A31_E,
      NU4100_PPE_PPU0_MISC_CSC5_A32_E,
      NU4100_PPE_PPU0_MISC_CSC6_A33_E,
      NU4100_PPE_PPU0_MISC_CSC6_B3_E);
   ppuCscTblSet(PPU_SEL_SCL, NU4100_PPE_PPU4_MISC_PPU_CTRL_CSC_BYPASS_E,
      NU4100_PPE_PPU4_MISC_PPU_CTRL_CSC_MODE_E,
      NU4100_PPE_PPU4_MISC_CSC1_A11_E,
      NU4100_PPE_PPU4_MISC_CSC1_A12_E,
      NU4100_PPE_PPU4_MISC_CSC2_A13_E,
      NU4100_PPE_PPU4_MISC_CSC2_B1_E,
      NU4100_PPE_PPU4_MISC_CSC3_A21_E,
      NU4100_PPE_PPU4_MISC_CSC3_A22_E,
      NU4100_PPE_PPU4_MISC_CSC4_A23_E,
      NU4100_PPE_PPU4_MISC_CSC4_B2_E,
      NU4100_PPE_PPU4_MISC_CSC5_A31_E,
      NU4100_PPE_PPU4_MISC_CSC5_A32_E,
      NU4100_PPE_PPU4_MISC_CSC6_A33_E,
      NU4100_PPE_PPU4_MISC_CSC6_B3_E);
   ppuCscTblSet(PPU_SEL_HYBSCL_P0, NU4100_PPE_PPU7_MISC0_PPU_CTRL_CSC_BYPASS_E,
      NU4100_PPE_PPU7_MISC0_PPU_CTRL_CSC_MODE_E,
      NU4100_PPE_PPU7_MISC0_CSC1_A11_E,
      NU4100_PPE_PPU7_MISC0_CSC1_A12_E,
      NU4100_PPE_PPU7_MISC0_CSC2_A13_E,
      NU4100_PPE_PPU7_MISC0_CSC2_B1_E,
      NU4100_PPE_PPU7_MISC0_CSC3_A21_E,
      NU4100_PPE_PPU7_MISC0_CSC3_A22_E,
      NU4100_PPE_PPU7_MISC0_CSC4_A23_E,
      NU4100_PPE_PPU7_MISC0_CSC4_B2_E,
      NU4100_PPE_PPU7_MISC0_CSC5_A31_E,
      NU4100_PPE_PPU7_MISC0_CSC5_A32_E,
      NU4100_PPE_PPU7_MISC0_CSC6_A33_E,
      NU4100_PPE_PPU7_MISC0_CSC6_B3_E);
   ppuCscTblSet(PPU_SEL_HYBSCL_P1, NU4100_PPE_PPU7_MISC1_PPU_CTRL_CSC_BYPASS_E,
      NU4100_PPE_PPU7_MISC1_PPU_CTRL_CSC_MODE_E,
      NU4100_PPE_PPU7_MISC1_CSC1_A11_E,
      NU4100_PPE_PPU7_MISC1_CSC1_A12_E,
      NU4100_PPE_PPU7_MISC1_CSC2_A13_E,
      NU4100_PPE_PPU7_MISC1_CSC2_B1_E,
      NU4100_PPE_PPU7_MISC1_CSC3_A21_E,
      NU4100_PPE_PPU7_MISC1_CSC3_A22_E,
      NU4100_PPE_PPU7_MISC1_CSC4_A23_E,
      NU4100_PPE_PPU7_MISC1_CSC4_B2_E,
      NU4100_PPE_PPU7_MISC1_CSC5_A31_E,
      NU4100_PPE_PPU7_MISC1_CSC5_A32_E,
      NU4100_PPE_PPU7_MISC1_CSC6_A33_E,
      NU4100_PPE_PPU7_MISC1_CSC6_B3_E);
   ppuCscTblSet(PPU_SEL_HYB_P0, NU4100_PPE_PPU8_MISC0_PPU_CTRL_CSC_BYPASS_E,
      NU4100_PPE_PPU8_MISC0_PPU_CTRL_CSC_MODE_E,
      NU4100_PPE_PPU8_MISC0_CSC1_A11_E,
      NU4100_PPE_PPU8_MISC0_CSC1_A12_E,
      NU4100_PPE_PPU8_MISC0_CSC2_A13_E,
      NU4100_PPE_PPU8_MISC0_CSC2_B1_E,
      NU4100_PPE_PPU8_MISC0_CSC3_A21_E,
      NU4100_PPE_PPU8_MISC0_CSC3_A22_E,
      NU4100_PPE_PPU8_MISC0_CSC4_A23_E,
      NU4100_PPE_PPU8_MISC0_CSC4_B2_E,
      NU4100_PPE_PPU8_MISC0_CSC5_A31_E,
      NU4100_PPE_PPU8_MISC0_CSC5_A32_E,
      NU4100_PPE_PPU8_MISC0_CSC6_A33_E,
      NU4100_PPE_PPU8_MISC0_CSC6_B3_E);
   ppuCscTblSet(PPU_SEL_HYB_P1, NU4100_PPE_PPU8_MISC1_PPU_CTRL_CSC_BYPASS_E,
      NU4100_PPE_PPU8_MISC1_PPU_CTRL_CSC_MODE_E,
      NU4100_PPE_PPU8_MISC1_CSC1_A11_E,
      NU4100_PPE_PPU8_MISC1_CSC1_A12_E,
      NU4100_PPE_PPU8_MISC1_CSC2_A13_E,
      NU4100_PPE_PPU8_MISC1_CSC2_B1_E,
      NU4100_PPE_PPU8_MISC1_CSC3_A21_E,
      NU4100_PPE_PPU8_MISC1_CSC3_A22_E,
      NU4100_PPE_PPU8_MISC1_CSC4_A23_E,
      NU4100_PPE_PPU8_MISC1_CSC4_B2_E,
      NU4100_PPE_PPU8_MISC1_CSC5_A31_E,
      NU4100_PPE_PPU8_MISC1_CSC5_A32_E,
      NU4100_PPE_PPU8_MISC1_CSC6_A33_E,
      NU4100_PPE_PPU8_MISC1_CSC6_B3_E);
}

static XMLDB_pathE *ppuSclTblSelect(nuBlkTypeE type, int p)
{
   //if asked ppu without scaler - return null
   if((type == NUCFG_BLK_PPU_NOSCL_E) || (type == NUCFG_BLK_PPU_HYB_E))
      return NULL;
   //if asked ppu with scaler but not hybrid, p=1 not relevent - return null
   else if ((type == NUCFG_BLK_PPU_SCL_E) && (p == 1))
      return NULL;

   return ppuSclTbls[ppuType2Sel(type,p)];
}

static void ppuStitchTblSet(enum ppuSelectE sel, XMLDB_pathE bypass, XMLDB_pathE p0_end, XMLDB_pathE p1_end,
      XMLDB_pathE pad_v_0, XMLDB_pathE pad_v_1, XMLDB_pathE hyb0_start, XMLDB_pathE hyb0_stop, XMLDB_pathE hyb1_start, XMLDB_pathE hyb1_stop )
{
   ppuStitchTbls[sel][STITCH_BYPASS] = bypass;
   ppuStitchTbls[sel][STITCH_P0_END] = p0_end;
   ppuStitchTbls[sel][STITCH_P1_END] = p1_end;
   ppuStitchTbls[sel][STITCH_PAD_V_0] = pad_v_0;
   ppuStitchTbls[sel][STITCH_PAD_V_1] = pad_v_1;
   ppuStitchTbls[sel][STITCH_HYB0_START] = hyb0_start;
   ppuStitchTbls[sel][STITCH_HYB0_STOP] = hyb0_stop;
   ppuStitchTbls[sel][STITCH_HYB1_START] = hyb1_start;
   ppuStitchTbls[sel][STITCH_HYB1_STOP] = hyb1_stop;
}
static void ppuStitchTblsInit(void)
{
   ppuStitchTblSet(PPU_SEL_HYB_P0, NU4100_PPE_PPU8_STITCH_CFG_BYPASS_E,
       NU4100_PPE_PPU8_STITCH_CFG_P0_END_E,
       NU4100_PPE_PPU8_STITCH_CFG_P1_END_E,
       NU4100_PPE_PPU8_STITCH_PAD_CFG_PAD_VAL_E,
       NU4100_PPE_PPU8_STITCH_PAD_CFG_PAD_VAL_E,
      NU4100_PPE_PPU8_STITCH_HYB0_START_E,
      NU4100_PPE_PPU8_STITCH_HYB0_STOP_E,
      NU4100_PPE_PPU8_STITCH_HYB1_START_E,
      NU4100_PPE_PPU8_STITCH_HYB1_STOP_E);
   ppuStitchTblSet(PPU_SEL_HYBSCL_P0, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E,
       NU4100_PPE_PPU7_STITCH_CFG_P0_END_E,
       NU4100_PPE_PPU7_STITCH_CFG_P1_END_E,
       NU4100_PPE_PPU7_STITCH_PAD_CFG_PAD_VAL_E,
       NU4100_PPE_PPU7_STITCH_PAD_CFG_PAD_VAL_E,
       NU4100_PPE_PPU7_STITCH_HYB0_START_E,
       NU4100_PPE_PPU7_STITCH_HYB0_STOP_E,
       NU4100_PPE_PPU7_STITCH_HYB1_START_E,
       NU4100_PPE_PPU7_STITCH_HYB1_STOP_E);
}
XMLDB_pathE *ppuStitchTblSelect(nuBlkTypeE type)
{
   if((type == NUCFG_BLK_PPU_SCL_E) || (type == NUCFG_BLK_PPU_NOSCL_E))
      return NULL;

   return ppuStitchTbls[ppuType2Sel(type,0)];
}

static void ppuRepackTblSet(enum ppuSelectE sel, XMLDB_pathE bypass, XMLDB_pathE hsize, XMLDB_pathE vsize,XMLDB_pathE horz, XMLDB_pathE rpk0_bpp, XMLDB_pathE rpk0_fmt,
   XMLDB_pathE rpk0_ctrl0_stream_vector, XMLDB_pathE rpk0_ctrl0_shift, XMLDB_pathE rpk0_ctrl0_mode, XMLDB_pathE rpk0_mrg_rep_rate, XMLDB_pathE rpk0_mrg_msk,
   XMLDB_pathE rpk0_mrg_en, XMLDB_pathE rpk0_shift_1_0, XMLDB_pathE rpk0_ctrl1_msk,XMLDB_pathE rpk1_ctrl0_stream_vector, XMLDB_pathE rpk1_ctrl0_shift,
   XMLDB_pathE rpk1_ctrl0_mode, XMLDB_pathE rpk1_ctrl1_mask, XMLDB_pathE rpk1_bpp, XMLDB_pathE rpk1_fmt)
{
   ppuRepackTbls[sel][REPACK_BYPASS] = bypass;
   ppuRepackTbls[sel][REPACK_HSIZE] = hsize;
   ppuRepackTbls[sel][REPACK_VSIZE] = vsize;
   ppuRepackTbls[sel][REPACK_OUT_HORZ] = horz;
   ppuRepackTbls[sel][REPACK0_OUT_BPP] = rpk0_bpp;
   ppuRepackTbls[sel][REPACK0_OUT_FMT] = rpk0_fmt;
   ppuRepackTbls[sel][RPK0_CTRL0_STREAM_VECTOR] = rpk0_ctrl0_stream_vector;
   ppuRepackTbls[sel][RPK0_CTRL0_SHIFT] = rpk0_ctrl0_shift;
   ppuRepackTbls[sel][RPK0_CTRL0_MODE] = rpk0_ctrl0_mode;
   ppuRepackTbls[sel][RPK0_MRG_REP_RATE] = rpk0_mrg_rep_rate;
   ppuRepackTbls[sel][RPK0_MRG_MSK] = rpk0_mrg_msk;
   ppuRepackTbls[sel][RPK0_MRG_EN] = rpk0_mrg_en;
   ppuRepackTbls[sel][RPK0_SHIFT_1_0] = rpk0_shift_1_0;
   ppuRepackTbls[sel][RPK0_CTRL1_MSK] = rpk0_ctrl1_msk;
   ppuRepackTbls[sel][RPK1_CTRL0_STREAM_VECTOR] = rpk1_ctrl0_stream_vector;
   ppuRepackTbls[sel][RPK1_CTRL0_SHIFT] = rpk1_ctrl0_shift;
   ppuRepackTbls[sel][RPK1_CTRL0_MODE] = rpk1_ctrl0_mode;
   ppuRepackTbls[sel][RPK1_CTRL1_MASK] = rpk1_ctrl1_mask;
   ppuRepackTbls[sel][REPACK1_OUT_BPP] = rpk1_bpp;
   ppuRepackTbls[sel][REPACK1_OUT_FMT] = rpk1_fmt;
}

static void ppuRepackTblsInit(void)
{
   ppuRepackTblSet(PPU_SEL_NOSCL, NU4100_PPE_PPU0_MISC_PPU_CTRL_REPACKER_BYPASS_E,
       NU4100_PPE_PPU0_REPACK0_PKR_HSIZE_P0_HSIZE_E,
       NU4100_PPE_PPU0_REPACK0_PKR_VSIZE_VSIZE_E,
       NU4100_PPE_PPU0_REPACK0_PKR_OUT_HORZ_P0_E,
       NU4100_PPE_PPU0_REPACK0_PKR_OUT_BPP_E,
       NU4100_PPE_PPU0_REPACK0_PKR_OUT_FMT_E,
       NU4100_PPE_PPU0_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E,
       NU4100_PPE_PPU0_REPACK0_SPLIT_CTRL0_SHIFT_E,
       NU4100_PPE_PPU0_REPACK0_SPLIT_CTRL0_MODE_E,
       NU4100_PPE_PPU0_REPACK0_PKR_CTRL_MRG_REP_RATE_E,
       NU4100_PPE_PPU0_REPACK0_PKR_CTRL_MRG_MSK_E,
       NU4100_PPE_PPU0_REPACK0_PKR_CTRL_MRG_EN_E,
       NU4100_PPE_PPU0_REPACK0_PKR_SHIFT0_SHIFT_1_0_E,
       NU4100_PPE_PPU0_REPACK0_SPLIT_CTRL1_MASK_E,
       NU4100_PPE_PPU0_REPACK1_SPLIT_CTRL0_STREAM_VECTOR_E,
       NU4100_PPE_PPU0_REPACK1_SPLIT_CTRL0_SHIFT_E,
       NU4100_PPE_PPU0_REPACK1_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU0_REPACK1_SPLIT_CTRL1_MASK_E,
       NU4100_PPE_PPU0_REPACK1_PKR_OUT_BPP_E,
       NU4100_PPE_PPU0_REPACK1_PKR_OUT_FMT_E
      );
   ppuRepackTblSet(PPU_SEL_SCL, NU4100_PPE_PPU4_MISC_PPU_CTRL_REPACKER_BYPASS_E,
      NU4100_PPE_PPU4_REPACK0_PKR_HSIZE_P0_HSIZE_E,
      NU4100_PPE_PPU4_REPACK0_PKR_VSIZE_VSIZE_E,
       NU4100_PPE_PPU4_REPACK0_PKR_OUT_HORZ_P0_E,
       NU4100_PPE_PPU4_REPACK0_PKR_OUT_BPP_E,
       NU4100_PPE_PPU4_REPACK0_PKR_OUT_FMT_E,
      NU4100_PPE_PPU4_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU4_REPACK0_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU4_REPACK0_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU4_REPACK0_PKR_CTRL_MRG_REP_RATE_E,
      NU4100_PPE_PPU4_REPACK0_PKR_CTRL_MRG_MSK_E,
      NU4100_PPE_PPU4_REPACK0_PKR_CTRL_MRG_EN_E,
      NU4100_PPE_PPU4_REPACK0_PKR_SHIFT0_SHIFT_1_0_E,
      NU4100_PPE_PPU4_REPACK0_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU4_REPACK1_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU4_REPACK1_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU4_REPACK1_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU4_REPACK1_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU4_REPACK1_PKR_OUT_BPP_E,
      NU4100_PPE_PPU4_REPACK1_PKR_OUT_FMT_E);
   ppuRepackTblSet(PPU_SEL_HYB_P0, NU4100_PPE_PPU8_MISC0_PPU_CTRL_REPACKER_BYPASS_E,
      NU4100_PPE_PPU8_REPACK0_PKR_HSIZE_P0_HSIZE_E,
      NU4100_PPE_PPU8_REPACK0_PKR_VSIZE_VSIZE_E,
       NU4100_PPE_PPU8_REPACK0_PKR_OUT_HORZ_P0_E,
       NU4100_PPE_PPU8_REPACK0_PKR_OUT_BPP_E,
       NU4100_PPE_PPU8_REPACK0_PKR_OUT_FMT_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU8_REPACK0_PKR_CTRL_MRG_REP_RATE_E,
      NU4100_PPE_PPU8_REPACK0_PKR_CTRL_MRG_MSK_E,
      NU4100_PPE_PPU8_REPACK0_PKR_CTRL_MRG_EN_E,
      NU4100_PPE_PPU8_REPACK0_PKR_SHIFT0_SHIFT_1_0_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU8_REPACK1_PKR_OUT_BPP_E,
      NU4100_PPE_PPU8_REPACK1_PKR_OUT_FMT_E);
   ppuRepackTblSet(PPU_SEL_HYB_P1, NU4100_PPE_PPU8_MISC1_PPU_CTRL_REPACKER_BYPASS_E,
      NU4100_PPE_PPU8_REPACK0_PKR_HSIZE_P1_HSIZE_E,
      NU4100_PPE_PPU8_REPACK0_PKR_VSIZE_VSIZE_E,
       NU4100_PPE_PPU8_REPACK0_PKR_OUT_HORZ_P1_E,
       NU4100_PPE_PPU8_REPACK0_PKR_OUT_BPP_E,
       NU4100_PPE_PPU8_REPACK0_PKR_OUT_FMT_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU8_REPACK0_PKR_CTRL_MRG_REP_RATE_E,
      NU4100_PPE_PPU8_REPACK0_PKR_CTRL_MRG_MSK_E,
      NU4100_PPE_PPU8_REPACK0_PKR_CTRL_MRG_EN_E,
      NU4100_PPE_PPU8_REPACK0_PKR_SHIFT0_SHIFT_1_0_E,
      NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU8_REPACK1_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU8_REPACK1_PKR_OUT_BPP_E,
      NU4100_PPE_PPU8_REPACK1_PKR_OUT_FMT_E);
   ppuRepackTblSet(PPU_SEL_HYBSCL_P0, NU4100_PPE_PPU7_MISC0_PPU_CTRL_REPACKER_BYPASS_E,
       NU4100_PPE_PPU7_REPACK0_PKR_HSIZE_P0_HSIZE_E,
       NU4100_PPE_PPU7_REPACK0_PKR_VSIZE_VSIZE_E,
       NU4100_PPE_PPU7_REPACK0_PKR_OUT_HORZ_P0_E,
       NU4100_PPE_PPU7_REPACK0_PKR_OUT_BPP_E,
       NU4100_PPE_PPU7_REPACK0_PKR_OUT_FMT_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU7_REPACK0_PKR_CTRL_MRG_REP_RATE_E,
      NU4100_PPE_PPU7_REPACK0_PKR_CTRL_MRG_MSK_E,
      NU4100_PPE_PPU7_REPACK0_PKR_CTRL_MRG_EN_E,
      NU4100_PPE_PPU7_REPACK0_PKR_SHIFT0_SHIFT_1_0_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU7_REPACK1_PKR_OUT_BPP_E,
      NU4100_PPE_PPU7_REPACK1_PKR_OUT_FMT_E);
   ppuRepackTblSet(PPU_SEL_HYBSCL_P1, NU4100_PPE_PPU7_MISC1_PPU_CTRL_REPACKER_BYPASS_E,
       NU4100_PPE_PPU7_REPACK0_PKR_HSIZE_P1_HSIZE_E,
       NU4100_PPE_PPU7_REPACK0_PKR_VSIZE_VSIZE_E,
       NU4100_PPE_PPU7_REPACK0_PKR_OUT_HORZ_P1_E,
       NU4100_PPE_PPU7_REPACK0_PKR_OUT_BPP_E,
       NU4100_PPE_PPU7_REPACK0_PKR_OUT_FMT_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU7_REPACK0_PKR_CTRL_MRG_REP_RATE_E,
      NU4100_PPE_PPU7_REPACK0_PKR_CTRL_MRG_MSK_E,
      NU4100_PPE_PPU7_REPACK0_PKR_CTRL_MRG_EN_E,
      NU4100_PPE_PPU7_REPACK0_PKR_SHIFT0_SHIFT_1_0_E,
      NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL0_STREAM_VECTOR_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL0_SHIFT_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL0_MODE_E,
      NU4100_PPE_PPU7_REPACK1_SPLIT_CTRL1_MASK_E,
      NU4100_PPE_PPU7_REPACK1_PKR_OUT_BPP_E,
      NU4100_PPE_PPU7_REPACK1_PKR_OUT_FMT_E);
}
XMLDB_pathE *ppuRepackTblSelect(nuBlkTypeE type,int p)
{
   //if asked ppu with scaler but not hybrid, p=1 not relevent - return null
   if ((type == NUCFG_BLK_PPU_SCL_E) && (p == 1))
      return NULL;
   return ppuRepackTbls[ppuType2Sel(type,p)];
}

static unsigned int getGeneratorOutBpp(void)
{
   return NU4K_IAE_GENERATOR_BPP;
}

static NUCFG_formatE getGeneratorFormat(void)
{
   return NUCFG_FORMAT_GEN_16_E;
}

static NUCFG_formatE getSensorFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   (void)nu4k, (void)blk;
   return NUCFG_FORMAT_NA_E;
}
static unsigned int getSensorOutBpp(nu4kT *nu4k, nu4kblkT *blk)
{
   return NUCFG_format2Bpp((NUCFG_formatE)getSensorFormat(nu4k,blk));
}

static NUCFG_formatE getWriterFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   UINT32 fmt = WRITER_FORMAT_RGB888_E;
   getDbField(nu4k, NUFLD_AXIWR_E,blk->ti.inst, NU4100_PPE_AXI_WRITE0_CTRL_DATA_FORMAT_E,&fmt);

   if(fmt >= WRITER_NUM_FORMATS_E)
      fmt = WRITER_FORMAT_RGB888_E;

   return nu4k_writer2nucfgFmt((nu4kWriterFormatE)fmt);
}

static unsigned int getWriterOutBpp(nu4kT *nu4k, nu4kblkT *blk)
{
   return NUCFG_format2Bpp(getWriterFormat(nu4k,blk));
}

static NUCFG_formatE getSluFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   UINT32 fmt = SLU_FORMAT_RGB888_E;
   XMLDB_pathE xmlPath = NU4100_IAE_SLU0_CONTROL_DATA_FORMAT_E;

   if (blk->ti.type == NUCFG_BLK_SLU_PARALLEL_E)
   {
       xmlPath = NU4100_IAE_SLU0_CONTROL_DATA_FORMAT_E;
   }
   else if(blk->ti.type == NUCFG_BLK_SLU_E)
   {
       xmlPath = NU4100_IAE_SLU2_CONTROL_DATA_FORMAT_E;
   }
   else
   {
      assert(0);
   }

   getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, xmlPath, &fmt);
   if(fmt >= SLU_NUM_FORMATS_E)
      fmt = SLU_FORMAT_RGB888_E;

   return nu4k_slu2nucfgFmt((nu4kSluFormatE)fmt);
}

static unsigned int getSluOutBpp(nu4kT *nu4k, nu4kblkT *blk)
{
   return NUCFG_format2Bpp(getSluFormat(nu4k,blk));
}
/*
static nu4kIauFormatE getIauInputFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   UINT32 fmt = IAU_FORMAT_GREY_E;

   if(blk->ti.type == NUCFG_BLK_IAU_E)
      getDbField(nu4k, NUFLD_IAU_E,blk->ti.inst, NU4100_IAE_IAU0_DATA_FORMAT_VAL_E,&fmt);
   else
      getDbField(nu4k, NUFLD_IAU_E,blk->ti.inst, NU4100_IAE_IAU2_DATA_FORMAT_VAL_E,&fmt);

   if(fmt < IAU_NUM_FORMATS_E)
      return (nu4kIauFormatE)fmt;

   return IAU_FORMAT_GREY_E;
}
*/
static NUCFG_formatE getIauOutputFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   NUCFG_formatE fmt;
   UINT32 bypassDms = 1;
   UINT32 bypassCsc = 1;
   UINT32 dataFormat = 0;

   if (blk->ti.type == NUCFG_BLK_IAU_E)
   {
      (blk->ti.inst == 0) ? XMLDB_getValue(nu4k->db, NU4100_IAE_BYPASS_CSC0_E, &bypassCsc) : XMLDB_getValue(nu4k->db, NU4100_IAE_BYPASS_CSC1_E, &bypassCsc);
      (blk->ti.inst == 0) ? XMLDB_getValue(nu4k->db, NU4100_IAE_BYPASS_DMS0_E, &bypassDms) : XMLDB_getValue(nu4k->db, NU4100_IAE_BYPASS_DMS1_E, &bypassDms);
      (blk->ti.inst == 0) ? XMLDB_getValue(nu4k->db, NU4100_IAE_IAU0_DATA_FORMAT_VAL_E, &dataFormat) : XMLDB_getValue(nu4k->db, NU4100_IAE_IAU1_DATA_FORMAT_VAL_E, &dataFormat);
   }
   else
   {
      (blk->ti.inst == 0) ? XMLDB_getValue(nu4k->db, NU4100_IAE_BYPASS_CSC2_E, &bypassCsc) : XMLDB_getValue(nu4k->db, NU4100_IAE_BYPASS_CSC3_E, &bypassCsc);
      (blk->ti.inst == 0) ? XMLDB_getValue(nu4k->db, NU4100_IAE_IAU2_DATA_FORMAT_VAL_E, &dataFormat) : XMLDB_getValue(nu4k->db, NU4100_IAE_IAU3_DATA_FORMAT_VAL_E, &dataFormat);
   }

   if ((bypassDms) && (bypassCsc))
   {
      if (dataFormat == 0)//Grey
      {
         //DSR block - adding bits to pixel
         fmt = NUCFG_FORMAT_RAW8_E;
      }
      else if (dataFormat == 1)//Bayer
      {
         fmt = NUCFG_FORMAT_BAYER_16_E;
         //call here to 'nu4k_setChannelFormat'?
      }
      else if (dataFormat == 3)//YUV422
      {
         fmt = NUCFG_FORMAT_YUV422_8BIT_E;
      }
      else
      {
         fmt = NUCFG_FORMAT_NA_E;//not supported yet
      }
   }
   else
   {
      fmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);

      //Demosaic block - switches Bayer to RGB 888 (only for iau 0/1)
      if (blk->ti.type == NUCFG_BLK_IAU_E)
      {
         if (!bypassDms)
         {
            fmt = NUCFG_FORMAT_RGB888_E;
         }
      }

      //csc  block - switches between YUV422 10 to RGB888
      if ((!bypassCsc) && ((fmt >= NUCFG_FORMAT_RGB888_E) && (fmt <= NUCFG_FORMAT_YUV422_10BIT_E)))
      {
         fmt = ((fmt >= NUCFG_FORMAT_RGB888_E) && (fmt <= NUCFG_FORMAT_RGB444_E)) ? NUCFG_FORMAT_YUV422_10BIT_E : NUCFG_FORMAT_RGB888_E;
      }
   }

   return fmt;
}

static unsigned int getIauOutBpp(nu4kT *nu4k, nu4kblkT *blk)
{
   return NUCFG_format2Bpp(getIauOutputFormat(nu4k,blk));
}

/*
*/
static NUCFG_formatE getPpuOutFormat(nu4kT *nu4k, nu4kblkT *blk, unsigned int index)
{
   (void)index;
   NUCFG_formatE inFmt;
   NUCFG_formatE outFmt = NUCFG_FORMAT_NA_E;
   UINT32 cscMode = 0, cscBypass= 0, a11 = 0, a12 = 0;;
   XMLDB_pathE cscBypassField = NU4100_PPE_PPU0_MISC_PPU_CTRL_CSC_BYPASS_E, cscModeField = NU4100_PPE_PPU0_MISC_PPU_CTRL_CSC_MODE_E;
   XMLDB_pathE a11Field = NU4100_PPE_PPU0_MISC_CSC1_A11_E, a12Field = NU4100_PPE_PPU0_MISC_CSC1_A12_E;

   if(!blk->inputs[0].blkp)
      return outFmt;

   //if repacker is not bypassed, get the format from the repacker
   getPpuRepackOut(nu4k, blk, 0, index, NULL, NULL, (NUCFG_formatE *)&outFmt);
   if(outFmt != NUCFG_FORMAT_NA_E)
      return outFmt;

   //Repacker is bypassed (or fmt is empty) - format needs to be checked at the source
   //    if input is grey output is grey
   //    else if csc is enabled input is converted between RGB888 and YUV422
   inFmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);

   if (blk->ti.type == NUCFG_BLK_PPU_NOSCL_E)
   {
       cscBypassField = NU4100_PPE_PPU0_MISC_PPU_CTRL_CSC_BYPASS_E;
       cscModeField = NU4100_PPE_PPU0_MISC_PPU_CTRL_CSC_MODE_E;
   }
   else if (blk->ti.type == NUCFG_BLK_PPU_SCL_E)
   {
       cscBypassField = NU4100_PPE_PPU4_MISC_PPU_CTRL_CSC_BYPASS_E;
       cscModeField = NU4100_PPE_PPU4_MISC_PPU_CTRL_CSC_MODE_E;
   }
   else if (blk->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
   {
       cscBypassField = NU4100_PPE_PPU7_MISC0_PPU_CTRL_CSC_BYPASS_E;
       cscModeField = NU4100_PPE_PPU7_MISC0_PPU_CTRL_CSC_MODE_E;
   }
   else if (blk->ti.type == NUCFG_BLK_PPU_HYB_E)
   {
       cscBypassField = NU4100_PPE_PPU8_MISC0_PPU_CTRL_CSC_BYPASS_E;
       cscModeField = NU4100_PPE_PPU8_MISC0_PPU_CTRL_CSC_MODE_E;
   }

   getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, cscBypassField, &cscBypass);

   if ((!cscBypass) && ((inFmt >= NUCFG_FORMAT_RGB888_E) && (inFmt <= NUCFG_FORMAT_YUV422_10BIT_E)))
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, cscModeField, &cscMode);
      outFmt = (cscMode == 0) ? NUCFG_FORMAT_YUV422_10BIT_E : NUCFG_FORMAT_RGB888_E;
   }
   else if ((!cscBypass) && (inFmt == NUCFG_FORMAT_GREY_16_E))
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, a11Field, &a11);
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, a12Field, &a12);

      if ((a11 == 4) && (a12 == 0x400))//we don't have really this mode on the HW. in this way we check if this is the case of right shift by 6
      {
         outFmt = NUCFG_FORMAT_RAW10_E;
      }
      else if ((a11 == 0) && (a12 == 0x100))//we don't have really this mode on the HW. in this way we check if this is the case of right shift by 8
      {
         outFmt = NUCFG_FORMAT_RAW8_E;
      }
   }
   else
   {
      outFmt = inFmt;
   }


   return outFmt;
}

static unsigned int getPpuOutBpp(nu4kT *nu4k, nu4kblkT *blk, unsigned int index)
{
   (void)index;
   NUCFG_formatE fmt;
   unsigned int bpp = 0;

   //if repacker is not bypassed, get the bpp from the repacker
   getPpuRepackOut(nu4k, blk, 0, index, NULL, (UINT32 *)&bpp, NULL);
   if(bpp != 0)
      return bpp;

   //if repacker is bypassed or bpp is not set, get bpp based on ppu output format
   fmt = getPpuOutFormat(nu4k,blk, index);
   if((fmt == NUCFG_FORMAT_YUV422_10BIT_E) || (fmt == NUCFG_FORMAT_RGB888_E))
   {
      bpp = 24;
   }
   else if(fmt == NUCFG_FORMAT_NA_E)
   {
      //Need to look at source (single) of ppu to determine bpp
      if(blk->inputs[0].blkp)
      {
         bpp = getOutputBpp(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);
      }
   }

   return bpp;
}

static void getPpuCropRes(nu4kT *nu4k, nu4kblkT *blk, int p, unsigned int *horzp, unsigned int *vertp)
{
   UINT32 x_start = 0,y_start = 0,x_end = 0,y_end = 0;
   XMLDB_pathE *tbl;

   tbl = ppuCropTblSelect(blk->ti.type,p);
   if (tbl)
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[CROP_START_X], &x_start);
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[CROP_END_X], &x_end);
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[CROP_START_Y], &y_start);
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[CROP_END_Y], &y_end);
   }
   *vertp = (UINT16)(y_end-y_start+1);
   *horzp = (UINT16)(x_end-x_start+1);
}

static void getPpuSclRes(nu4kT *nu4k, nu4kblkT *blk, int p, unsigned int *horzp, unsigned int *vertp)
{
   UINT32 sclmode = NU4K_PPU_SCL_MODE_BYPASS;
   UINT32 pad = 0, horz = 0, vert = 0;
   XMLDB_pathE *tbl;

   tbl = ppuSclTblSelect(blk->ti.type,p);
   if(tbl)
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_MODE], &sclmode);
      if(sclmode != NU4K_PPU_SCL_MODE_BYPASS)
      {
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_HSIZE], &horz);
         horz++;
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_PAD_L], &pad);
         horz += pad;
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_PAD_R], &pad);
         horz += pad;
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_VSIZE], &vert);
         vert++;

         *horzp = horz;
         *vertp = vert;
      }
   }
}

static void getPpuStitchRes(nu4kT *nu4k, nu4kblkT *blk, int p, unsigned int *horzp, unsigned int *vertp)
{
   UINT32 bypass = 0, vpad = 0, horz = 0,vert = 0;
   XMLDB_pathE *tbl;

   tbl = ppuStitchTblSelect(blk->ti.type);

   if(tbl)
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_BYPASS], &bypass);
      if((bypass) || (p == 0))
      {
         //assuming in bypass mode p0_end and v_0 padding take effect
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_P0_END], &horz);
         horz++;
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_PAD_V_0], &vpad);
         vert = *vertp + vpad;
      }
      else
      {
         //not bypassed and p is 1.
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_P1_END], &horz);
         horz++;
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_PAD_V_1], &vpad);
         vert = *vertp + vpad;
      }
      *horzp = horz;
      *vertp = vert;
   }
}

/*
   get ppu repacker_n (for p0/p1 )output parameters.
   The repacker out fields are virtual, not part of HW. They are used to tell the code
   what the repacker is outputing.

   The p0/p1 paramter affects only hybrid capable ppus with respect to the horizontal parameter only.
   The other paramters are the same for both p0/p1 in both hybrid and non-hybrid modes.
   if repacker is bypassed - output is equal to 16 Lsbits of input
*/
static void getPpuRepackOut(nu4kT *nu4k, nu4kblkT *blk, int p, unsigned int pkrNum,
      unsigned int *horzp, unsigned int *bpp, NUCFG_formatE *fmt)
{
   UINT32 hybEn = 0, horz = 0, bypass= 0, _fmt = 0, _bpp = 0;
   XMLDB_pathE hybEnOff,tmp;
   XMLDB_pathE *tbl;

   //for hybrid ppus - if hybrid is disabled use the p0 values even if p1 is requested.
   if((blk->ti.type == NUCFG_BLK_PPU_HYB_E) || (blk->ti.type == NUCFG_BLK_PPU_HYBSCL_E))
   {
      if (blk->ti.type == NUCFG_BLK_PPU_HYB_E)
         hybEnOff = NU4100_PPE_PPU8_REPACK0_PKR_CTRL_HYB_EN_E;
      else
         hybEnOff = NU4100_PPE_PPU7_REPACK0_PKR_CTRL_HYB_EN_E;

      tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, hybEnOff);
      XMLDB_getValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), &hybEn);
      p = (!hybEn) ? 0: p; //force to p0 if hybrid is disabled
   }
   tbl = ppuRepackTblSelect(blk->ti.type, p);
   if (tbl)
   {
      //get repacker bypass for p0/p1 - This value is common to all repackers on the ppu.
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK_BYPASS], &bypass);
      if (!bypass)
      {
         tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK_OUT_HORZ]);
         XMLDB_getValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), &horz);
         tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK0_OUT_BPP]);
         XMLDB_getValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), &_bpp);
         tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK0_OUT_FMT]);
         XMLDB_getValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), &_fmt);
         if (bpp) *bpp = _bpp;
         if (fmt) *fmt = (NUCFG_formatE)_fmt;
         if (horzp) *horzp = horz + 1;
      }
      else
      {
         if (bpp) *bpp = 16;//as defined in spec, 16 LSBits of input
      }
   }
}


/*
   For all PPU the data path is :
      cropping ==> scaling ==> hybrid stiching (2 inputs) => repacking

   Scaler, Hybrid and Repacking can be bypassed or they do not exist for some PPUs.
*/
static void getPpuOutRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int index, unsigned int *horzp, unsigned int *vertp)
{
   UINT32 p0_h = 0,p1_h = 0;
   UINT32 p0_v = 0,p1_v = 0;

   //for p0/p1 get horz/vert for cropping
   getPpuCropRes(nu4k,blk, 0, &p0_h,&p0_v);
   getPpuCropRes(nu4k,blk, 1, &p1_h,&p1_v);
   //for p0/p1 get horz/vert after scaler
   getPpuSclRes(nu4k,blk, 0, &p0_h,&p0_v);
   getPpuSclRes(nu4k,blk, 1, &p1_h,&p1_v);
   //for p0/p1 get horz/vert after sticher
   getPpuStitchRes(nu4k,blk, 0, &p0_h,&p0_v);
   getPpuStitchRes(nu4k,blk, 1, &p1_h,&p1_v);
   //for p0/p1 get horz/vert with repacker index
   getPpuRepackOut(nu4k,blk, 0, index, &p0_h, NULL,NULL);
   getPpuRepackOut(nu4k,blk, 1, index, &p1_h, NULL,NULL);

   if(vertp) *vertp = p0_v;//assuming vertical output is always the same for p0/p1
   if(horzp) *horzp = (p0_h < p1_h) ? p1_h : p0_h;//if p1 disabled it will be equal to p0
}

static unsigned int getInterOutBpp(nu4kT *nu4k, nu4kblkT *blk)
{
   UINT32 val = 4;
   // should not use pxl_data_width? // by david @ 2022.11.30
   getDbField(nu4k, NUFLD_AXIRD_E, blk->ti.inst, NU4100_PPE_AXI_READ0_CTRL_PXL_INT_WIDTH_E, &val);
   //getDbField(nu4k, NUFLD_AXIRD_E, blk->ti.inst, NU4100_PPE_AXI_READ0_CTRL_PXL_DATA_WIDTH_E, &val);

   if(val == 0)  return 8;
   if (val == 1) return 12;
   if (val == 2) return 16;
   if (val == 3) return 24;
   return 0;
}

static void getInterOutputRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horzp, unsigned int *vertp)
{
   unsigned int i;
   unsigned int ihorz = 0,ivert = 0;
   unsigned int horz = 0, vert = 0;
   //output width is sum of all axi inputs
   //output height is assumed the same for all inputs
   for(i = 0; i < blk->numInputs; i++)
   {
      if(!blk->inputs[i].blkp)
         continue;

      getOutputRes(nu4k, blk->inputs[i].blkp, blk->inputs[i].sel,&ihorz, &ivert);

      horz += ihorz;
      vert = ivert;
   }

   if(horzp) *horzp = horz;
   if(vertp) *vertp = vert;
}

static void getDphyTxOutputRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horzp, unsigned int *vertp)
{
   unsigned int i;
   unsigned int ihorz = 0,ivert = 0;
   unsigned int horz = 0, vert = 0;
   //output width is sum of all VSC CSI inputs
   //output height is assumed the same for all inputs
   for(i = 0; i < blk->numInputs; i++)
   {
      if(!blk->inputs[i].blkp)
         continue;

      getOutputRes(nu4k, blk->inputs[i].blkp, blk->inputs[i].sel,&ihorz, &ivert);

      horz += ihorz;
      vert = ivert;
   }

   if(horzp) *horzp = horz;
   if(vertp) *vertp = vert;
}

static void getIspRdOutputRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horzp, unsigned int *vertp)
{
   unsigned int horz = 0, vert = 0;
   XMLDB_pathE isp0_rd0_feild = ISPS_ISP0_OUTPUTS_MP_ENABLE_E;
   XMLDB_pathE isp1_rd0_feild = ISPS_ISP1_OUTPUTS_MP_ENABLE_E;
   XMLDB_pathE rd1_feild = ISPS_ISP0_OUTPUTS_SP1_ENABLE_E, rd0_feild;
   UINT32 rdSize = ISPS_ISP0_OUTPUTS_SP1_ENABLE_E - ISPS_ISP0_OUTPUTS_MP_ENABLE_E;
   UINT32 scl_en, crop_en;
   ERRG_codeE ret_scl,ret_crop;
   UINT32 isp0_isp1_rd_offset = (blk->ti.inst < 3) ? 0 : (ISPS_ISP1_OUTPUTS_MP_ENABLE_E - ISPS_ISP1_NUM_FRAMES_TO_SKIP_E);//jump ISP1 functionality entries


   //check if user configured scl or crop and take the sizes if yes.
   //else, take the sizes from sensor
   //TODO: check for another functionality of ISP to change sizes internaly, besides crop and scale

   ret_scl = XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_SCL_ENABLE_E + (blk->ti.inst * rdSize) + isp0_isp1_rd_offset), &scl_en);
   ret_crop = XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_CROP_ENABLE_E + (blk->ti.inst * rdSize) + isp0_isp1_rd_offset), &crop_en);
   //check scl
   if (ERRG_SUCCEEDED(ret_scl) && (scl_en))
   {
       XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_SCL_WIDTH_E + (blk->ti.inst * rdSize) + isp0_isp1_rd_offset), &horz);
       XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_SCL_HEIGHT_E + (blk->ti.inst * rdSize) + isp0_isp1_rd_offset), &vert);
   }
   //check crop
   else if (ERRG_SUCCEEDED(ret_crop) && (crop_en))
   {
       XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_CROP_WIDTH_E + (blk->ti.inst * rdSize)), &horz);
       XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_CROP_HEIGHT_E + (blk->ti.inst * rdSize)), &vert);
   }
   else//take sizes from input if exist
   {
       if (blk->inputs[0].blkp)
       {
           getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
       }
   }
    if(horzp) *horzp = horz;
    if(vertp) *vertp = vert;
}

static NUCFG_formatE getDpeOutFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   (void)blk;
   UINT32 outSel = NU4K_DPE_OUTSEL_MAIN_E;
   getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DEBUG_CFG_OUT_SEL_E, &outSel);

   return nu4k_dpe2nucfgFmt((nu4kDpeOutFormatE)outSel);
}

static unsigned int getDpeOutBpp(nu4kT *nu4k, nu4kblkT *blk)
{
   FIX_UNUSED_PARAM_WARN(blk);
   unsigned int bpp;
   UINT32 outSel = NU4K_DPE_OUTSEL_MAIN_E;
   //DPE output depends on debug output select field
   getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DEBUG_CFG_OUT_SEL_E, &outSel);

   //Assuming either 16 or 24 bit output (rounding up)
   if((outSel == NU4K_DPE_OUTSEL_FINAL_SMALL_DISPARITY_E) ||
      (outSel == NU4K_DPE_OUTSEL_EDGE_DISTANCE_LEFT_E) ||
      (outSel == NU4K_DPE_OUTSEL_EDGE_DISTANCE_RIGHT_E) ||
      (outSel == NU4K_DPE_OUTSEL_UNITE_E) ||
      (outSel == NU4K_DPE_OUTSEL_INPUT_0_E) || (outSel == NU4K_DPE_OUTSEL_INPUT_1_E) ||
      (outSel == NU4K_DPE_OUTSEL_INPUT_2_E) || (outSel == NU4K_DPE_OUTSEL_INPUT_3_E))
   {
      bpp = 16;
   }
   else
      bpp = 24;

   return bpp;
}

static void getDpeOutRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horzP, unsigned int *vertP)
{
   UINT32 dpeDebugMux = 0, horz = 0, vert = 0;
   getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DEBUG_CFG_OUT_SEL_E, &dpeDebugMux);

   if (dpeDebugMux == NU4K_DPE_OUTSEL_MAIN_E)
   {
#if 1
      UINT32 merge_size = 0, scaleMode = 0;
      //UINT32 vpad = 0;
      FIX_UNUSED_PARAM_WARN(blk);
      //Disparity offset modification -
      //a.  The output size of the DPE should always be taken from: DPE -> cfg -> merge_size
      //  <FIELD NAME="merge_size" RESET_VAL="0x0" START_BIT_OFFSET="16" WIDTH="11" VALUE="1247" USER_ID="9981" />
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MERGE_SIZE_E, &merge_size);
      if (merge_size)merge_size++;
      horz = merge_size;
#else
      UINT32 hyben = 0, merge_dis = 0, merge_size = 0, p0_horz = 0, p1_horz = 0, horz = 0, vert = 0, scaleMode = 0, vpad = 0;
      //Horizontal:
      //for instance 0 - use p0
      //for instance 1 - use p1
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_P0_SIZE_E, &p0_horz);
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_P1_SIZE_E, &p1_horz);
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_EN_E, &hyben);
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MERGE_SIZE_E, &merge_size);
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_HYB_MRG_BYPASS_E, &merge_dis);

      if (p0_horz)p0_horz++;
      if (p1_horz)p1_horz++;
      if (merge_size)merge_size++;

      if (hyben)
      {
         if (blk->ti.inst == 0)
         {//primery
            if (!merge_dis)      //merge enabled
               horz = merge_size;//when merged there is only primary
            else
               horz = p0_horz;
         }
         else
         {//secondary
            if (!merge_dis)      //merge enabled
               horz = 0;         //when merged there is no secondary
            else
               horz = p1_horz;
         }
      }
      else
         horz = p0_horz;//if hyb not enabled should not get instance 1 - so ignore it
#endif

   //Vertical:
   //Assumption here is all outputs from DIF need to have the same vertical size.
   //So only need to check the DIF P0 L/R input vsize, and if scaling is enabled the
   //scaler DIF scaler output size.
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CFG_SCALE_MODE_E, &scaleMode);
      if (scaleMode < NU4K_DIF_SCL_MODE_VERT_ONLY)
         getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CFG_IN_VSIZE_E, &vert);
      else
         getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG1_VSIZE_E, &vert);
      if (vert) vert++;

      //vert padding
      //getDbField(nu4k, NUFLD_DPE_E, 0, (XMLDB_pathE)0/*NU4100_DPE_DIF_LEFT_P0_VERT_PAD_PAD_SIZE_E*/, &vpad);
      //if(vpad) vpad++;
           //vert += vpad;
      if (vertP)
         *vertP = vert;
      if (horz)
         *horzP = horz;
   }
   else//output debug mux as is
   {
      UINT32 bypass = NU4K_PPU_SCL_MODE_BYPASS;
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CFG_SCALE_MODE_E, &bypass);
      if (bypass == NU4K_PPU_SCL_MODE_BYPASS)
      {
         //Output the DIF Left and Right Pixels. sizes are the sum the input size (pixel by pixel)
         if (blk->inputs[0].blkp)
            getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
         if (vertP)
            *vertP = vert;
         if (horz)
            *horzP = horz;
         if (blk->inputs[1].blkp)
            getOutputRes(nu4k, blk->inputs[1].blkp, blk->inputs[1].sel, &horz, &vert);
        // if (vert != *vertP)
        //    assert(0);
      }
      else
      {
         //Take sizes from scaler
         getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_HSIZE_E, &horz);
         if (horz)
            *horzP = (horz + 1);
         getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG1_HSIZE_E, &horz);
         if (horz)
            *horzP += (horz + 1);
         getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG1_VSIZE_E, &vert);
         if (vertP)
            *vertP = (vert + 1);
         getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG1_VSIZE_E, &vert);
         if ((vert + 1) != *vertP)
            assert(0);
      }
   }
}

static void getDpeHybOutRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horzP, unsigned int *vertP)
{
   UINT32 horz_0 = 0, horz_1 = 0, vert = 0;

   //horz0 is p0 horizontal
   //horz1 is p1-p0 in hybrid mode. or 0 in non hyb mode.
   if(blk->inputs[0].blkp)
      getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz_0, &vert);
   if(blk->inputs[1].blkp)
      getOutputRes(nu4k, blk->inputs[1].blkp, blk->inputs[1].sel, &horz_1, NULL);

   if(horzP)
      *horzP = horz_0 + horz_1;
   if(vertP)
      *vertP = vert;
}

static NUCFG_formatE getIspOutputFormat(nu4kT* nu4k, nu4kblkT* blk)
{
    NUCFG_formatE fmt = NUCFG_FORMAT_NA_E;
    UINT32 rdSize = ISPS_ISP0_OUTPUTS_SP1_ENABLE_E - ISPS_ISP0_OUTPUTS_MP_ENABLE_E;
    UINT32 isp0_isp1_rd_offset = (blk->ti.inst < 3) ? 0 : (ISPS_ISP1_OUTPUTS_MP_ENABLE_E - ISPS_ISP1_NUM_FRAMES_TO_SKIP_E);//jump ISP1 functionality entries
    UINT32 rd_format;
    ERRG_codeE ret;

    ret = XMLDB_getValue(nu4k->db, (XMLDB_pathE)(ISPS_ISP0_OUTPUTS_MP_FORMAT_E + (blk->ti.inst * rdSize + isp0_isp1_rd_offset)), &rd_format);
    if (ERRG_SUCCEEDED(ret))//not empty value
    {
        fmt = (NUCFG_formatE)rd_format;
    }
    else
    {
        fmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel); // assuming same format for all VSC CSI's
    }

    return fmt;
}

//dpp instance 0/1 is the host/cva respectively
static NUCFG_formatE getDppOutFormat(nu4kT *nu4k, nu4kblkT *blk)
{
   NUCFG_formatE fmt = NUCFG_FORMAT_DISPARITY_E;
   UINT32 cvaSelect= 0, hostSelect = 0, depthPos= 0;

   //Format could be disparity/depth/registered depth
   XMLDB_getValue(nu4k->db, NU4100_PPE_DEPTH_POST_MISC_CVA_CTRL_SRC_SEL_E, &cvaSelect);
   XMLDB_getValue(nu4k->db, NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_SRC_SEL_E, &hostSelect);
   XMLDB_getValue(nu4k->db, NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_DEPTH_POS_E, &depthPos);

   if( ((blk->ti.inst == 0) && (hostSelect == NU4K_DPP_SRCSEL_PRIM_DISP)) ||
       ((blk->ti.inst == 1) && (cvaSelect == NU4K_DPP_SRCSEL_PRIM_DISP)) )
   {
      fmt = NUCFG_FORMAT_DISPARITY_E;
   }
   else if( ((blk->ti.inst == 0) && ((hostSelect == NU4K_DPP_SRCSEL_PRIM_DEPTH) || (hostSelect == NU4K_DPP_SRCSEL_REG_PRIM_DEPTH))) ||
            ((blk->ti.inst == 1) && ((cvaSelect == NU4K_DPP_SRCSEL_PRIM_DEPTH) || (cvaSelect == NU4K_DPP_SRCSEL_REG_PRIM_DEPTH))) )
   {
         fmt = NUCFG_FORMAT_DEPTH_E;
   }
   //TODO:  there are different bit orderings based on positioning of confidence and px
   return fmt;
}

static void getDppOutRes(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horzP, unsigned int *vertP)
{
   UINT32 horz = 0,vert = 0;
   XMLDB_pathE scl_horz_off,scl_vert_off,na;
   //dpp instance 0/1 is the host/cva respectively
   if(blk->ti.inst == 0)
   {
      SET_DPP_SCL_OFFSETS(HOST,na, scl_horz_off, scl_vert_off,na,na,na,na);
   }
   else
   {
      SET_DPP_SCL_OFFSETS(CVA,na, scl_horz_off, scl_vert_off,na,na,na,na);
   }
   (void)na;
   //There is no real bypass of scaler - the horz/vert values of scaler always determine output resolution
   XMLDB_getValue(nu4k->db, scl_horz_off, &horz);
   if(horz) horz++;
   XMLDB_getValue(nu4k->db, scl_vert_off, &vert);
   if(vert) vert++;

   if(horzP) *horzP = horz;
   if(vertP) *vertP = vert;
}

static NUCFG_formatE geVscFormat(nu4kT* nu4k, nu4kblkT* blk)
{
    UINT32 fmt = 0; // this will translate to NUCFG_FORMAT_NA_E when returning

    getDbField(nu4k, NUFLD_VSC_CSI_TX_E, blk->ti.inst, NU4100_PPE_VSC_CSI2_CSI_CTRL_VIDEO_FORMAT_E, &fmt);

    if (fmt == 0) {
        // Get inputs video format
        if (blk->inputs[0].blkp)
        {
            NUCFG_formatE _fmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel); // assuming same format for all VSC CSI's
            return _fmt;
        }
    }

    return nu4k_mipi2nucfgFmt((nu4kCsiOutFormatE)fmt);
}

static void parseDataSel(nu4kT *nu4k, UINT8 dataSel, nu4kblkT **selblk, unsigned int *selIndex)
{

   if (dataSel <= NU4K_PPU_SRC_SEL_MAX_SLU_PARALLEL)
   {
       *selblk = &nu4k->slusParallel[dataSel - NU4K_PPU_SRC_SEL_MIN_SLU_PARALLEL].blk;
       *selIndex = 0;
   }
   else if ((dataSel >= NU4K_PPU_SRC_SEL_MIN_SLU) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_SLU))
   {
       *selblk = &nu4k->slus[dataSel - NU4K_PPU_SRC_SEL_MIN_SLU].blk;
       *selIndex = 0;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_IAU) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_IAU))
   {
      *selblk = &nu4k->iaus[dataSel-NU4K_PPU_SRC_SEL_MIN_IAU].blk;
      *selIndex = 0;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_IAU_COLOR) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_IAU_COLOR))
   {
      *selblk = &nu4k->iausColor[dataSel-NU4K_PPU_SRC_SEL_MIN_IAU_COLOR].blk;
      *selIndex = 0;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_INJ) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_CVJ))
   {
      *selblk = &nu4k->writers[((dataSel-NU4K_PPU_SRC_SEL_MIN_INJ) >> 1)].blk;
      *selIndex = 0;
   }
   else if(dataSel == NU4K_PPU_SRC_SEL_DPE)
   {
      //There are 2 instances of DPE, so we use the virtual dpe_hyb block as input.
      *selblk = &nu4k->dpeHyb[0].blk;
      *selIndex = 0;
   }
   else if(dataSel == NU4K_PPU_SRC_SEL_DPP_HOST)
   {
      *selblk = &nu4k->dpp[0].blk; //host is dpp 0, cva is 1
      selIndex = 0;
   }
   else if(dataSel == NU4K_PPU_SRC_SEL_DPP_CVA)
   {
      *selblk = &nu4k->dpp[1].blk; //host is dpp 0, cva is 1
      selIndex = 0;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_PPU_NOSCL) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_PPU_NOSCL)) {
      int stream = dataSel-NU4K_PPU_SRC_SEL_MIN_PPU_NOSCL;
      *selblk = &nu4k->ppusNoScl[stream/4].blk;
      *selIndex = stream % 4;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_PPU_SCL) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_PPU_SCL))
   {
      int stream = dataSel-NU4K_PPU_SRC_SEL_MIN_PPU_SCL;
      *selblk = &nu4k->ppusScl[stream/4].blk;
      *selIndex = stream % 4;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_PPU_HYBSCL) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_PPU_HYBSCL))
   {
      int stream = dataSel-NU4K_PPU_SRC_SEL_MIN_PPU_HYBSCL;
      *selblk = &nu4k->ppusHybScl[stream/4].blk;
      *selIndex = stream % 4;
   }
   else if((dataSel >= NU4K_PPU_SRC_SEL_MIN_PPU_HYB) && (dataSel <= NU4K_PPU_SRC_SEL_MAX_PPU_HYB))
   {
      int stream = dataSel-NU4K_PPU_SRC_SEL_MIN_PPU_HYB;
      *selblk = &nu4k->ppusHyb[stream/4].blk;
      *selIndex = stream % 4;
   }
}

static void parseIauSel(nu4kT *nu4k, UINT32 iau_sel, nu4kblkT **selblk, unsigned int *selIndex)
{
   //if((iau_sel >= NU4K_IAU_SELECT_MIN_SLU) && (iau_sel <= NU4K_IAU_SELECT_MAX_SLU))
   if(iau_sel <= NU4K_IAU_SELECT_MAX_SLU_PARALLEL)
   {
      *selblk = &nu4k->slusParallel[iau_sel - NU4K_IAU_SELECT_MIN_SLU_PARALLEL].blk;
      *selIndex = 0;
   }
   else if ((iau_sel >= NU4K_IAU_SELECT_MIN_SLU) && (iau_sel <= NU4K_IAU_SELECT_MAX_SLU))
   {
       *selblk = &nu4k->slus[iau_sel - NU4K_IAU_SELECT_MIN_SLU].blk;
       *selIndex = 0;
   }
   else if((iau_sel >= NU4K_IAU_SELECT_MIN_INJ) && (iau_sel <= NU4K_IAU_SELECT_MAX_INJ))
   {
      //inj0,1 -> writer0. inj2,3 ->writer 1, inj4,5 ->writer2
      static int writerSelMap[6] = {0,0,1,1,2,2};
      *selblk  = &nu4k->writers[writerSelMap[iau_sel-NU4K_IAU_SELECT_MIN_INJ]].blk;
      *selIndex = 0;
   }
   else if((iau_sel >= NU4K_IAU_SELECT_MIN_GEN) && (iau_sel <= NU4K_IAU_SELECT_MAX_GEN))
   {
      *selblk = &nu4k->gens[iau_sel-NU4K_IAU_SELECT_MIN_GEN].blk;
      *selIndex = 0;
   }
}

static void parseHistSel(nu4kT *nu4k, UINT32 hist_sel, UINT32 hist_inst,nu4kblkT **selblk, unsigned int *selIndex)
{
   (void)hist_sel;
   *selblk  = &nu4k->iaus[hist_inst].blk;
   *selIndex = 0;
}

static void parseMipiCsiSel(nu4kT *nu4k, UINT32 src_sel, nu4kblkT **selblk)
{
    //src_sel 0-11 are sensors 0-11. src_sel 12-17 are mediators 0-5
    if (src_sel < NU4K_NUM_SENSORS)
    {
        *selblk = &nu4k->sensors[src_sel].blk;
    }
    else if (src_sel < NU4K_NUM_SENSORS + NU4K_NUM_MEDIATORS)
    {
        *selblk = &nu4k->mediators[src_sel - NU4K_NUM_SENSORS].blk;
    }
    else
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "src sel %d is illegal\n", src_sel);
        assert(0);
    }
}

static void parseMipiRxSel(nu4kT *nu4k, UINT32 mipi_rx_inst, nu4kblkT **selblk, unsigned int *selIndex)
{
    UINT32 src_sel = 0;

    if (mipi_rx_inst == INU_DEFSG_MIPI_INST_0_E)
    {
        getDbField(nu4k, NUFLD_MIPI_RX_E, 0, BUS_MIPI_CSI_RX0_SRC_SEL_E, &src_sel);
        parseMipiCsiSel(nu4k, src_sel, selblk);
        *selIndex = 0;
    }
    else if (mipi_rx_inst == INU_DEFSG_MIPI_INST_1_E)
    {
        getDbField(nu4k, NUFLD_MIPI_RX_E, 0, BUS_MIPI_CSI_RX1_SRC_SEL_E, &src_sel);
        parseMipiCsiSel(nu4k, src_sel, selblk);
        *selIndex = 0;
    }
    else if (mipi_rx_inst == INU_DEFSG_MIPI_INST_2_E)
    {
        getDbField(nu4k, NUFLD_MIPI_RX_E, 0, BUS_MIPI_CSI_RX2_SRC_SEL_E, &src_sel);
        parseMipiCsiSel(nu4k, src_sel, selblk);
        *selIndex = 0;
    }
    else if (mipi_rx_inst == INU_DEFSG_MIPI_INST_3_E)
    {
        getDbField(nu4k, NUFLD_MIPI_RX_E, 0, BUS_MIPI_CSI_RX3_SRC_SEL_E, &src_sel);
        parseMipiCsiSel(nu4k, src_sel, selblk);
        *selIndex = 0;
    }
    else if (mipi_rx_inst == INU_DEFSG_MIPI_INST_4_E)
    {
        getDbField(nu4k, NUFLD_MIPI_RX_E, 0, BUS_MIPI_CSI_RX4_SRC_SEL_E, &src_sel);
        parseMipiCsiSel(nu4k, src_sel, selblk);
        *selIndex = 0;
    }
    else if (mipi_rx_inst == INU_DEFSG_MIPI_INST_5_E)
    {
        getDbField(nu4k, NUFLD_MIPI_RX_E, 0, BUS_MIPI_CSI_RX5_SRC_SEL_E, &src_sel);
        parseMipiCsiSel(nu4k, src_sel, selblk);
        *selIndex = 0;
    }
}

static void getPpuInput(nu4kT *nu4k, nu4kblkT *ppu, unsigned int p, nu4kblkT **input, unsigned int *in_index)
{
   XMLDB_pathE srcSelfield;
   UINT32 data_sel = 0, bypass = 0;
   nu4kblkT *_input = NULL;
   unsigned int _in_index = 0;
   XMLDB_pathE *tbl;
   tbl = ppuStitchTblSelect(ppu->ti.type);
   if (tbl)
   {
      getDbField(nu4k, nusoc_convert2Fld(ppu->ti.type), ppu->ti.inst, tbl[STITCH_BYPASS], &bypass);
   }

   if(ppu->ti.type == NUCFG_BLK_PPU_NOSCL_E)
      srcSelfield = NU4100_PPE_PPU0_MISC_PPU_CTRL_SRC_SEL_E;
   else if (ppu->ti.type == NUCFG_BLK_PPU_SCL_E)
      srcSelfield = NU4100_PPE_PPU4_MISC_PPU_CTRL_SRC_SEL_E;
   else if (ppu->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
   {
      srcSelfield = (p == 0) ? NU4100_PPE_PPU7_MISC0_PPU_CTRL_SRC_SEL_E : NU4100_PPE_PPU7_MISC1_PPU_CTRL_SRC_SEL_E;
        if ((p == 1) && bypass)
            return;
   }
   else
   {
       srcSelfield = (p == 0) ? NU4100_PPE_PPU8_MISC0_PPU_CTRL_SRC_SEL_E : NU4100_PPE_PPU8_MISC1_PPU_CTRL_SRC_SEL_E;
       if ((p == 1) && bypass)
           return;
   }

   getDbField(nu4k, nusoc_convert2Fld(ppu->ti.type), ppu->ti.inst, srcSelfield, &data_sel);
   parseDataSel(nu4k,(UINT8)(data_sel & 0xFF), &_input , &_in_index);

   //ignore case where the input is equal to ppu block - this is a loop.
   if((!_input) && (_input->ti.type == ppu->ti.type) && (_input->ti.inst == ppu->ti.inst))
      return;

   *input = _input;
   *in_index = _in_index;
}

static void getDpeInput(nu4kT *nu4k, nu4kblkT *dpe, unsigned int index, nu4kblkT **iau, unsigned int *iau_index)
{
   XMLDB_pathE chSelField = NU4100_DPE_DIF_LEFT_P0_CFG_CHANNEL_SEL_E;
   UINT32 chanSelect = 0;

   //dpe instance inputs (p0 L/R and p1 L/R) are from iau [0,3] based on channel selection
   //instance 0/1 are p0/p1 and index 0/1 are L/R, respectively
   if((dpe->ti.inst == 0) && (index == 0))
      chSelField = NU4100_DPE_DIF_LEFT_P0_CFG_CHANNEL_SEL_E;
   else if ((dpe->ti.inst == 0) && (index == 1))
      chSelField = NU4100_DPE_DIF_RIGHT_P0_CFG_CHANNEL_SEL_E;
   else if ((dpe->ti.inst == 1) && (index == 0))
      chSelField = NU4100_DPE_DIF_LEFT_P1_CFG_CHANNEL_SEL_E;
   else if((dpe->ti.inst == 1) && (index == 1))
      chSelField = NU4100_DPE_DIF_RIGHT_P1_CFG_CHANNEL_SEL_E;

   XMLDB_getValue(nu4k->db, chSelField, &chanSelect);
   if(chanSelect < NU4K_NUM_IAUS)
      *iau = &nu4k->iaus[chanSelect].blk;
   else
      *iau = &nu4k->iausColor[chanSelect-NU4K_NUM_IAUS].blk;
   *iau_index = 0; //single iau output
}

static void getDpeHybInput(nu4kT *nu4k, nu4kblkT *dpeHyb, unsigned int index, nu4kblkT **dpe, unsigned int *dpe_index)
{
   (void)dpeHyb;
   //virtual DPE hybrid bloc - used for separating the depth paths
   //Input index 0 is p0 from dpe0 and input index 1 is p1 from dpe1.
   //In non-hybrid second input is disabled

   UINT32 hyben = 0;
   if(index == 0)
   {
      *dpe = &nu4k->dpe[0].blk;
      *dpe_index = 0;//single dpe output
   }
   else if(index == 1)
   {
#ifdef NU4K_A0_DPE_HYBRID_FIX
      hyben = 0;
#else
            getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_EN_E, &hyben);
#endif
      if(hyben)
      {
         *dpe = &nu4k->dpe[1].blk;
         *dpe_index = 0;//single dpe output
      }
   }
}

static void getDppInput(nu4kT *nu4k, nu4kblkT *dpp, unsigned int index, nu4kblkT **dpe, unsigned int *dpe_index)
{
   (void)index; //single input per dpp instance
   UINT32 srcSel = 0;

   //DPP has 2 virtual instances: DPP_0/1 are host/cva,respectively.
   //DPE has 2 virtual instances: DPE_0/1 are p0(primary)/p1(secondary), respectively.
   //We use src select of the depth_post scaler/cropper to determine input to depth post.
   if(dpp->ti.inst == 0) //host
      XMLDB_getValue(nu4k->db, NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_SRC_SEL_E, &srcSel);
   else //cva
      XMLDB_getValue(nu4k->db, NU4100_PPE_DEPTH_POST_MISC_CVA_CTRL_SRC_SEL_E, &srcSel);

   if(NU4K_DPP_SRCSEL_IS_PRIMARY(srcSel))
   {
      *dpe = &nu4k->dpe[0].blk; //primary
   }
   else
   {
      *dpe = &nu4k->dpe[1].blk;//secondary
   }
   *dpe_index = 0;//single dpe output
}
/*
typedef enum
{
   INU_SENSORS__OP_MODE_BINNING_E = 1,
   INU_SENSORS__OP_MODE_VERTICAL_BINNING_E = 2,
   INU_SENSORS__OP_MODE_FULL_E = 3,
   INU_SENSORS__OP_MODE_FULL_HD_E = 4,
   INU_SENSORS__OP_MODE_USER_DEF_E = 5,
   INU_SENSORS__OP_MODE_UNKNOWN_E = 6,
   INU_SENSORS__OP_MODE_NUM_MODES = 7,
}inu_sensor__op_mode_e;
*/

void convertCalModeToInuMode(UINT32 *calMode, UINT32 *inuMode)
{
   switch (*calMode)
   {
   case(CALIB_MODE_BIN_E):
      *inuMode = 1;
      return;
   case(CALIB_MODE_FULL_E):
      *inuMode = 3;
      return;
   case(CALIB_MODE_USER_DEFINE_E):
      *inuMode = 5;
      return;
   case(CALIB_MODE_VERTICAL_BINNING_E):
      *inuMode = 2;
      return;
   case(CALIB_MODE_UXGA_E):
      *inuMode = 4;
      return;
   default:
      assert(0);
   }
}

static void updateSen(nu4kT *nu4k, nu4kblkT *blk, UINT32 *mode, UINT32 *vert_offset, INT32 *gain_offset)
{
   if(mode)
   {
      UINT32 inuMode;
      convertCalModeToInuMode(mode,&inuMode);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, SENSORS_SENS_0_OP_MODE_E, inuMode);
   }

   if(vert_offset)
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, SENSORS_SENS_0_VERT_OFFSET_E, *vert_offset);
}

static void updateWriter(nu4kT *nu4k, nu4kblkT *blk, UINT32 horizontalSize, UINT32 verticalSize)
{
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_AXI_WRITE0_SIZE_HSIZE_E, horizontalSize);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_AXI_WRITE0_SIZE_VSIZE_E, verticalSize);
}


static void updateHist(nu4kT *nu4k, nu4kblkT *blk,  UINT32 inputWidth, UINT32 inputHeight)
{
    XMLDB_pathE x0,y0,x1,y1;
    x0 = NU4100_IAE_IAU0_HIST_ROI0_NW_X0_E;
    y0 = NU4100_IAE_IAU0_HIST_ROI0_NW_Y0_E;
    x1 = NU4100_IAE_IAU0_HIST_ROI0_SE_X1_E;
    y1 = NU4100_IAE_IAU0_HIST_ROI0_SE_Y1_E;

    setDbField(nu4k, NUFLD_IAU_E, blk->ti.inst, x0, 50);
    setDbField(nu4k, NUFLD_IAU_E, blk->ti.inst, y0, 50);
    setDbField(nu4k, NUFLD_IAU_E, blk->ti.inst, x1, inputWidth - 50);
    setDbField(nu4k, NUFLD_IAU_E, blk->ti.inst, y1, inputHeight - 50);
}

static void updateSlu(nu4kT *nu4k, nu4kblkT *blk, unsigned int *horz, unsigned int *horz_offset, unsigned int *vert, unsigned int *vert_offset, UINT32 interFactor)
{
    XMLDB_pathE sluHorz, sluVert, sluCropHorz, sluCropVert;
    UINT32 sluInd =0, spareTmp=0;

    if (blk->ti.type == NUCFG_BLK_SLU_PARALLEL_E)
    {
        sluHorz = NU4100_IAE_SLU0_FRAME_SIZE_HORZ_E;
        sluVert = NU4100_IAE_SLU0_FRAME_SIZE_VERT_E;
        sluCropHorz = NU4100_IAE_SLU0_CROP_OFFSET_HORZ_E;
        sluCropVert = NU4100_IAE_SLU0_CROP_OFFSET_VERT_E;
    }
    else
    {
        sluHorz = NU4100_IAE_SLU2_FRAME_SIZE_HORZ_E;
        sluVert = NU4100_IAE_SLU2_FRAME_SIZE_VERT_E;
        sluCropHorz = NU4100_IAE_SLU2_CROP_OFFSET_HORZ_E;
        sluCropVert = NU4100_IAE_SLU2_CROP_OFFSET_VERT_E;
    }

   if(horz)
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, sluHorz, ((*horz) * interFactor) -1);
   if(vert)
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, sluVert, *vert-1);

   //set offsets if they are given
   if(horz_offset)
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, sluCropHorz, *horz_offset);
   if(vert_offset)
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, sluCropVert, *vert_offset);
}
/**
 * @brief Updates the SLU cropping using a user crop window
 * 
 * @param nu4k Nu4k Handle
 * @param blk Block handle
 * @param blockType  Block type (NUCFG_BLK_SLU_E or NUCFG_BLK_SLU_PARALLEL_E  )
 * @return Returns true if successfully used the user crop
 */
static bool updateSluUserCrop(nu4kT *nu4k, nu4kblkT *blk, nuBlkTypeE blockType)
{

   UINT32 instance = 0;
   nuSluT *slu = &nu4k->slus[blk->ti.inst];
   if(blockType == NUCFG_BLK_SLU_E)
   {
      slu = &nu4k->slus[blk->ti.inst];
   }
   else
   {
      slu = &nu4k->slusParallel[blk->ti.inst];
   }
   NUCFG_resT * userCrop = &slu->userCrop;


   if ((userCrop != NULL) && (userCrop->width != 0) && (userCrop->height != 0))
   {
      updateSlu(nu4k, blk, &userCrop->width,&userCrop->x,&userCrop->height,&userCrop->y, 1);
      printf("Found user crop with %lu w, %lu for SLU  \n",userCrop->width,userCrop->height );
      return true;
   }
   return false;
}
UINT32 nu4k_getIbOffset(nu4kT *nu4k,UINT32 oldIbOffset, nu4kblkT *blk)
{

   UINT32 dataFormat = 0xff, pixelAlignment = 0xff;
   UINT32 ibOffset = oldIbOffset;
   UINT32 actualBpp = 0;
   INT32 ibShift;
   unsigned int inputBlkInst;
   nu4kblkT *inputBlk;
    XMLDB_dbH dbh= nu4k->db;
    UINT32 sluInd=0xff;
    UINT16 sluFormat2ActualBpp[16]={ 0,0,0,0,0,0,0,0,0,0,6,7,8,10,12,14 };
    UINT16 writerFormat2ActualBpp[17] = { 0,0,0,0,0,0,0,6,7,8,10,12,14,8,12,16,24 };

    XMLDB_pathE sluTablePixelInd[NU4K_NUM_SLUS+NU4K_NUM_SLUS_PARALLEL]={ NU4100_IAE_SLU0_CONTROL_PIXEL_ALIGNMENT_E,
                            NU4100_IAE_SLU1_CONTROL_PIXEL_ALIGNMENT_E,
                            NU4100_IAE_SLU2_CONTROL_PIXEL_ALIGNMENT_E,
                            NU4100_IAE_SLU3_CONTROL_PIXEL_ALIGNMENT_E,
                            NU4100_IAE_SLU4_CONTROL_PIXEL_ALIGNMENT_E,
                            NU4100_IAE_SLU5_CONTROL_PIXEL_ALIGNMENT_E };
    XMLDB_pathE sluTableDataFmtInd[NU4K_NUM_SLUS+ NU4K_NUM_SLUS_PARALLEL]={ NU4100_IAE_SLU0_CONTROL_DATA_FORMAT_E,
                                        NU4100_IAE_SLU1_CONTROL_DATA_FORMAT_E,
                                        NU4100_IAE_SLU2_CONTROL_DATA_FORMAT_E,
                                        NU4100_IAE_SLU3_CONTROL_DATA_FORMAT_E,
                                        NU4100_IAE_SLU4_CONTROL_DATA_FORMAT_E,
                                        NU4100_IAE_SLU5_CONTROL_DATA_FORMAT_E };

    getInput(nu4k, blk, blk->ti.inst, &inputBlk, &inputBlkInst);
    sluInd = inputBlk->ti.inst;
    if ((inputBlk->ti.type == NUCFG_BLK_SLU_PARALLEL_E) || (inputBlk->ti.type == NUCFG_BLK_SLU_E))
    {
       if (inputBlk->ti.type == NUCFG_BLK_SLU_E)
          sluInd += 2;

       XMLDB_getValue(dbh, sluTablePixelInd[sluInd], &pixelAlignment);
       XMLDB_getValue(dbh, sluTableDataFmtInd[sluInd], &dataFormat);
       if (dataFormat < 16)
       {
          actualBpp = sluFormat2ActualBpp[dataFormat];
       }
    }
    else if (inputBlk->ti.type == NUCFG_BLK_AXIWR_E)
    {
       XMLDB_getValue(dbh, NUFLD_calcPath(NUFLD_AXIWR_E, inputBlk->ti.inst, NU4100_PPE_AXI_WRITE0_CTRL_DATA_FORMAT_E), &dataFormat);
       pixelAlignment = 0;
       if (dataFormat < 17)
       {
          actualBpp = writerFormat2ActualBpp[dataFormat];
       }
    }
    else
    {
       assert(0);
    }

    if (pixelAlignment != 0xff)
    {
       ibShift = 16 - actualBpp - pixelAlignment;
       if (ibShift > 0)
       {
          ibOffset = oldIbOffset << ibShift;
       }
       else
       {
          ibOffset = oldIbOffset >> abs(ibShift);
       }
    }

   return ibOffset;
}

static UINT32 mappingDsrHorzDeltaSizeValues(UINT32 val)
{
   UINT32 retVal = 0;
   switch (val)
   {
   case (11):
      retVal = 0;
      break;
   case (12):
      retVal = 1;
      break;
   case (13):
      retVal = 2;
      break;
   case (14):
      retVal = 3;
      break;
   case (15):
      retVal = 4;
      break;
   case (16):
      retVal = 5;
      break;
   default:
      assert(0);
   }
   return retVal;
}

static UINT32 mappingDsrVertDeltaSizeValues(UINT32 val)
{
   UINT32 retVal = 0;
   switch (val)
   {
   case (11):
      retVal = 5;
      break;
   case (12):
      retVal = 4;
      break;
   case (13):
      retVal = 3;
      break;
   case (14):
      retVal = 2;
      break;
   case (15):
      retVal = 1;
      break;
   case (16):
      retVal = 0;
      break;
   default:
      assert(0);
   }
   return retVal;
}

static UINT32 mappingDsrBlockSizeValues(UINT32 val)
{
   UINT32 retVal = 0;
   switch (val)
   {
   case (2):
      retVal = 0; //log2(val)-1
      break;
   case (4):
      retVal = 1;//log2(val)-1
      break;
   case (8):
      retVal = 2;//log2(val)-1
      break;
   case (16):
      retVal = 3;//log2(val)-1
      break;
   case (32):
      retVal = 4;//log2(val)-1
      break;
   default:
      assert(0);
   }
   return retVal;
}

static UINT32 mappingIbBlockSizeValues(UINT32 val)
{
   UINT32 retVal = 0;
   switch (val)
   {
   case (2):
      retVal = 0; //log2(val)-1
      break;
   case (4):
      retVal = 1;//log2(val)-1
      break;
   case (8):
      retVal = 2;//log2(val)-1
      break;
   case (16):
      retVal = 3;//log2(val)-1
      break;
   case (32):
      /* This is illegal value, but since it was programmed to the flash in many HP units,
         the assertion was replaced with warning printing. However, this wrong value is
         not used in practice. */
      retVal = 0;
      LOGG_PRINT(LOG_WARN_E, NULL, "wrong IB block size value (%d)\n", val);
      break;
   default:
      assert(0);
   }
   return retVal;
}

static void updateIau(nu4kT *nu4k, nu4kblkT *blk,  UINT32 *ibVerticalLutOffset,
   UINT32 *ibHorizontalLutOffset, UINT32 *ibOffset, UINT32* ibOffsetR, UINT32* ibOffsetGR, UINT32* ibOffsetGB, UINT32* ibOffsetB,
    UINT32 *dsrVerticalLutOffset, UINT32 *dsrHorizontalLutOffset, UINT32 *dsrOutputVerticalSize, UINT32 *dsrOutputHorizontalSize,

   UINT32 *dsrVerticalBlockSize, UINT32 *dsrHorizontalBlockSize, UINT32 *dsrLutHorizontalSize,
   UINT32 *dsrHorizontalDeltaSize, UINT32 *dsrHorizontalDeltaFractionalBits, UINT32 *dsrVerticalDeltaSize,
   UINT32 *dsrVerticalDeltaFractionalBits, UINT32 *ibVerticalBlockSize, UINT32 *ibHorizontalBlockSize,
   UINT32 *ibLutHorizontalSize, UINT32 *ibFixedPointFractionalBits)
{
   NUFLD_blkE type = nusoc_convert2Fld(blk->ti.type);
   unsigned int inst = blk->ti.inst;
   XMLDB_pathE dsrVertLutOff, dsrHorzLutOff, dsrOutVert, dsrOutHorz,
      dsrVertBlock, dsrHorzBlock, dsrLutHorzSize, dsrHorzDelta, dsrHorzDeltaFractBits, dsrVertDelta,
      dsrVertDeltaFractBits, ibVertBlock, ibHorzBlock, ibLutHorzSize, ibFixedPointFractBits, ibHorizontalLutOffsetField,
      ibVerticalLutOffsetField, ibOffsetField, ibOffset0Field, ibOffset1Field, ibOffset2Field, ibOffset3Field;
   UINT32 newIbOffset,tmpField;

   if (blk->ti.type == NUCFG_BLK_IAU_E)
   {
      SET_DSR_UPDATE_OFFSETS(IAU0, dsrVertLutOff, dsrHorzLutOff, dsrOutVert, dsrOutHorz,
         dsrVertBlock, dsrHorzBlock, dsrLutHorzSize, dsrHorzDelta, dsrHorzDeltaFractBits, dsrVertDelta, dsrVertDeltaFractBits);
      SET_IB_UPDATE_OFFSETS(IAU0,ibVertBlock,ibHorzBlock,ibLutHorzSize,ibFixedPointFractBits,
         ibHorizontalLutOffsetField, ibVerticalLutOffsetField, ibOffsetField, ibOffset0Field, ibOffset1Field, ibOffset2Field, ibOffset3Field);
   }
   else
   {
      SET_DSR_UPDATE_OFFSETS(IAU2, dsrVertLutOff, dsrHorzLutOff, dsrOutVert, dsrOutHorz,
         dsrVertBlock, dsrHorzBlock, dsrLutHorzSize, dsrHorzDelta, dsrHorzDeltaFractBits, dsrVertDelta, dsrVertDeltaFractBits);
      SET_IB_UPDATE_OFFSETS(IAU2,ibVertBlock,ibHorzBlock,ibLutHorzSize,ibFixedPointFractBits,
         ibHorizontalLutOffsetField, ibVerticalLutOffsetField, ibOffsetField, ibOffset0Field, ibOffset1Field, ibOffset2Field, ibOffset3Field);

   }

   if(dsrVerticalLutOffset)
      setDbField(nu4k, type, inst, dsrVertLutOff, *dsrVerticalLutOffset);
   if(dsrHorizontalLutOffset)
      setDbField(nu4k, type, inst, dsrHorzLutOff, *dsrHorizontalLutOffset);
   if(dsrOutputVerticalSize)
      setDbField(nu4k, type, inst, dsrOutVert, *dsrOutputVerticalSize-1);
   if(dsrOutputHorizontalSize)
      setDbField(nu4k, type, inst, dsrOutHorz, *dsrOutputHorizontalSize-1);

   if (dsrVerticalBlockSize)
      setDbField(nu4k, type, inst, dsrVertBlock, mappingDsrBlockSizeValues(*dsrVerticalBlockSize));
   if (dsrHorizontalBlockSize)
      setDbField(nu4k, type, inst, dsrHorzBlock, mappingDsrBlockSizeValues(*dsrHorizontalBlockSize));
   if (dsrLutHorizontalSize)
      setDbField(nu4k, type, inst, dsrLutHorzSize, *dsrLutHorizontalSize);
   if (dsrHorizontalDeltaSize)
      setDbField(nu4k, type, inst, dsrHorzDelta, mappingDsrHorzDeltaSizeValues(*dsrHorizontalDeltaSize));
   if (dsrHorizontalDeltaFractionalBits)
      setDbField(nu4k, type, inst, dsrHorzDeltaFractBits, *dsrHorizontalDeltaFractionalBits);
   if (dsrVerticalDeltaSize)
      setDbField(nu4k, type, inst, dsrVertDelta, mappingDsrVertDeltaSizeValues(*dsrVerticalDeltaSize));
   if (dsrVerticalDeltaFractionalBits)
      setDbField(nu4k, type, inst, dsrVertDeltaFractBits, *dsrVerticalDeltaFractionalBits);
   if (ibVerticalBlockSize)
      setDbField(nu4k, type, inst, ibVertBlock, mappingIbBlockSizeValues(*ibVerticalBlockSize));
   if (ibHorizontalBlockSize)
      setDbField(nu4k, type, inst, ibHorzBlock, mappingIbBlockSizeValues(*ibHorizontalBlockSize));
   if (ibLutHorizontalSize)
      setDbField(nu4k, type, inst, ibLutHorzSize, *ibLutHorizontalSize);
   if (ibFixedPointFractionalBits)
      setDbField(nu4k, type, inst, ibFixedPointFractBits, *ibFixedPointFractionalBits);
   if(ibHorizontalLutOffset)
      setDbField(nu4k, type, inst, ibHorizontalLutOffsetField, *ibHorizontalLutOffset);
   if(ibVerticalLutOffset)
      setDbField(nu4k, type, inst, ibVerticalLutOffsetField, *ibVerticalLutOffset);
   if(ibOffset)
   {
      newIbOffset = nu4k_getIbOffset(nu4k,*ibOffset,blk);
      setDbField(nu4k, type, inst, ibOffsetField, newIbOffset);
   }
   if (ibOffsetR)
   {
       newIbOffset = nu4k_getIbOffset(nu4k, *ibOffsetR, blk);
       setDbField(nu4k, type, inst, ibOffset0Field, newIbOffset);
   }
   if (ibOffsetGR)
   {
       newIbOffset = nu4k_getIbOffset(nu4k, *ibOffsetGR, blk);
       setDbField(nu4k, type, inst, ibOffset1Field, newIbOffset);
   }
   if (ibOffsetGB)
   {
       newIbOffset = nu4k_getIbOffset(nu4k, *ibOffsetGB, blk);
       setDbField(nu4k, type, inst, ibOffset2Field, newIbOffset);
   }
   if (ibOffsetB)
   {
       newIbOffset = nu4k_getIbOffset(nu4k, *ibOffsetB, blk);
       setDbField(nu4k, type, inst, ibOffset3Field, newIbOffset);
   }

   if (inst < 2)
   {
      getDbField(nu4k, NUFLD_IAU_E, inst, NU4100_IAE_IAU0_HIST_CONTROL_CIIF_SELECT_E, &tmpField);
      if (tmpField != 0)
      {
         //not IAU input, set ROI based on the output size of the IAU
         updateHist(nu4k, blk, *dsrOutputHorizontalSize-1, *dsrOutputVerticalSize-1);
      }
      else
      {
         //IAU input (writer or SLU), set ROI based on the input side of the IAU
         UINT32 vert, horz;
         getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
         updateHist(nu4k, blk, horz - 1, vert - 1);
      }
   }
}


static void updateZeroCropping(UINT32 *st, UINT32 *ed, UINT32 range)
{
   *st = 0;
   *ed= range-1;
}

#if 0
static void updateCopyCropping(UINT32 *st, UINT32 *ed, UINT32 range)
{
   unsigned int shift;

   if(*ed > range-1)
   {
      shift = *ed - (range-1);
      *ed -= shift;
      *st = (*st < shift) ? 0 : (*st-shift);
   }
}
#endif
/*
* update the ppu's cropping block for p0 or p1, based on the input resolution
*/
static void updatePpuCrop(nu4kT *nu4k, nu4kblkT *blk, unsigned int p, UINT32 *in_horz, UINT32 *in_vert)
{
   XMLDB_pathE *tbl;
   UINT32 crop_x_st = 0, crop_x_ed, crop_y_st = 0, crop_y_ed = 0;
   UINT32 _in_horz = 0, _in_vert = 0, bypass = 0;
   NUCFG_resT *userCrop = NULL;

   tbl = ppuStitchTblSelect(blk->ti.type);
   if (tbl)
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_BYPASS], &bypass);
   }

   if ((p >= blk->numInputs) || ((bypass) && (p == 1)))
   {
      *in_horz = 0;
      *in_vert = 0;
      return;
   }


   tbl = ppuCropTblSelect(blk->ti.type, p);

   //get input resolution to ppu
   getOutputRes(nu4k, blk->inputs[p].blkp, blk->inputs[p].sel, &_in_horz, &_in_vert);

   switch(blk->ti.type)
   {
      case NUCFG_BLK_PPU_NOSCL_E:
        userCrop = &(nu4k->ppusNoScl[blk->ti.inst].userCrop);
     break;
      case NUCFG_BLK_PPU_SCL_E:
           userCrop = &(nu4k->ppusScl[blk->ti.inst].userCrop);
     break;
      case NUCFG_BLK_PPU_HYB_E:
        userCrop = &(nu4k->ppusHyb[blk->ti.inst].userCrop);
     break;
      case NUCFG_BLK_PPU_HYBSCL_E:
        userCrop = &(nu4k->ppusHybScl[blk->ti.inst].userCrop);
     break;
     default:
        LOGG_PRINT(LOG_ERROR_E,NULL,"not a ppu type. Block type %d\n", blk->ti.type);
        return;
   }

   if( (userCrop != NULL) && (userCrop->width != 0) && (userCrop->height != 0) )
   {
      crop_x_st = userCrop->x;
      crop_x_ed = userCrop->x + userCrop->width - 1;
      crop_y_st = userCrop->y;
      crop_y_ed = userCrop->y + userCrop->height - 1;
      if( (_in_vert < crop_y_ed) || (_in_horz < crop_x_ed) ||
         (_in_vert < crop_y_st) || (_in_horz < crop_x_st) )
      {
         LOGG_PRINT(LOG_ERROR_E,NULL,"Requested crop size is out of boundries\n");
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E,NULL,"User crop applied\n");
      }
   }
   else
   {
      updateZeroCropping(&crop_x_st,&crop_x_ed, _in_horz);
      updateZeroCropping(&crop_y_st,&crop_y_ed, _in_vert);

   }

   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, tbl[CROP_START_X],crop_x_st);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, tbl[CROP_END_X], crop_x_ed);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, tbl[CROP_START_Y],crop_y_st);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, tbl[CROP_END_Y],crop_y_ed);
   *in_horz = crop_x_ed-crop_x_st+1;
   *in_vert = crop_y_ed-crop_y_st+1;
}

/*
* update the ppu's scaler block for p0 or p1, based on the input resolution
*/
static void updatePpuScl(nu4kT *nu4k, nu4kblkT *blk, int p, UINT32 *in_horz, UINT32 *in_vert)
{
   XMLDB_pathE *tbl;
   UINT32 hsizeScl, vsizeScl, sclmode = NU4K_PPU_SCL_MODE_BYPASS;
   float xscale, yscale;
   (void)in_horz;

   tbl = ppuSclTblSelect(blk->ti.type, p);
   if (tbl)
   {
      //update h_in_vsize parameter in scaler
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_H_IN_VSIZE], *in_vert-1);

      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_MODE], &sclmode);

      if ((((int)sclmode) < NU4K_PPU_SCL_MODE_BYPASS) || (sclmode > NU4K_PPU_SCL_MODE_VERT_N_HORZ))//in case 'sclmode' value is empty on XML
         sclmode = NU4K_PPU_SCL_MODE_BYPASS;

      if (sclmode != NU4K_PPU_SCL_MODE_BYPASS)
      {
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_HSIZE], &hsizeScl);
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_VSIZE], &vsizeScl);

         //update xscale&yscale
         //xscale
         xscale = (((float)*in_horz)) / (float)(hsizeScl+1);
         xscale = xscale * 65536; // like ((float) << 16), but xscale is float and there is no option to shift float type. we need the floating point accuracy
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_XSCALE], (UINT32)xscale);
         //yscale
         yscale = (((float)*in_vert)) / (float)(vsizeScl + 1);
         yscale = yscale * 65536; // like ((float) << 16), but xscale is float and there is no option to shift float type. we need the floating point accuracy
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[SCL_YSCALE], (UINT32)yscale);

         *in_horz = hsizeScl + 1;
         *in_vert = vsizeScl + 1;
      }
   }
}

static void updatePpuStitch(nu4kT *nu4k, nu4kblkT *blk, UINT32 *in_horz_0, UINT32 *in_vert_0, UINT32 *in_horz_1, UINT32 *in_vert_1)
{
   XMLDB_pathE *tbl;
   UINT32 bypassStitch;
   (void)in_vert_0;
   (void)in_vert_1;
   //Stitcher has 2 inputs p0/p1.
   //Vertical - Although sticher sholuld use padding to output the same vertical resolution - this is left to application.
   //Horizontal - Set only p0_end/p1_end according to input. Other values are left to application.
   //We need for hybrid mode data for P0&P1, therefore the function
   tbl = ppuStitchTblSelect(blk->ti.type);
   if(tbl)
   {
      //p=0
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_P0_END], (*in_horz_0) - 1);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, tbl[STITCH_HYB0_START], 0);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst, tbl[STITCH_HYB0_STOP], (*in_horz_0)-1);

     getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_BYPASS], &bypassStitch);
     if (bypassStitch == NU4K_PPU_STITCH_BYPASS)
     {
        setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_P1_END], 0);
        setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_HYB1_START], 0);
        setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_HYB1_STOP], 0);
     }
     else // bypassStitch == 0 or empty
     {
        setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_P1_END], (((*in_horz_0) + (*in_horz_1)) - 1));
        setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_HYB1_START], (*in_horz_0));
        setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[STITCH_HYB1_STOP], (((*in_horz_0) + (*in_horz_1)) - 1));
     }
   }
}

static void updatePpuRepackOut(nu4kT *nu4k, nu4kblkT *blk, int p, unsigned int pkrNum, UINT32 *in_horz, UINT32 *in_vertp)
{
   XMLDB_pathE *tbl;
   XMLDB_pathE tmp, hybEnOff;
   UINT32 hybEn = 0;
   UINT32 in_horz_0;
   ERRG_codeE ret = NUCFG__RET_SUCCESS;;


   //Configuration remains the same - only change the input horz/vert resolution for the repacker
   tbl = ppuRepackTblSelect(blk->ti.type, p);
   if (tbl)
   {
      tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK_HSIZE]);
      XMLDB_setValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), *in_horz - 1);

   if ((blk->ti.type == NUCFG_BLK_PPU_HYB_E) || (blk->ti.type == NUCFG_BLK_PPU_HYBSCL_E))
   {
      if (blk->ti.type == NUCFG_BLK_PPU_HYB_E)
         hybEnOff = NU4100_PPE_PPU8_REPACK0_PKR_CTRL_HYB_EN_E;
      else
         hybEnOff = NU4100_PPE_PPU7_REPACK0_PKR_CTRL_HYB_EN_E;

      tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, hybEnOff);
      ret = XMLDB_getValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), &hybEn);
  }

      if ((hybEn == 1) && (ret != XMLDB__EMPTY_ENTRY) && (p == 1))
      {
         tbl = ppuRepackTblSelect(blk->ti.type, 0);

         //TMP: update repacker out size for support binning resolution. Todo: Analyze and update repacker out size from repacker data values
         tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK_OUT_HORZ]);
         XMLDB_getValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), &in_horz_0);

         *in_horz += in_horz_0 + 1;
         tbl = ppuRepackTblSelect(blk->ti.type, p);
      }

      tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK_VSIZE]);
      XMLDB_setValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), *in_vertp - 1);

      //TMP: update repacker out size for support binning resolution. Todo: Analyze and update repacker out size from repacker data values
      tmp = NUFLD_calcPath(nusoc_convert2Fld(blk->ti.type), blk->ti.inst, tbl[REPACK_OUT_HORZ]);
      XMLDB_setValue(nu4k->db, NUFLD_calcPath(NUFLD_PPU_REPACK_E, pkrNum, tmp), *in_horz - 1);
      /////////////////////////////////////////////////////////////////////////////////////////////
   }
}

static void updatePpuOut(nu4kT *nu4k, nu4kblkT *blk, unsigned int index)
{
   UINT32 in_horz_0 = 0, in_horz_1 = 0, in_vert_0 = 0, in_vert_1 = 0;

   updatePpuCrop(nu4k,blk,0, &in_horz_0, &in_vert_0);
   updatePpuCrop(nu4k,blk,1, &in_horz_1, &in_vert_1);

   updatePpuScl(nu4k,blk,0, &in_horz_0, &in_vert_0);
   updatePpuScl(nu4k,blk,1, &in_horz_1, &in_vert_1);

   updatePpuStitch(nu4k,blk, &in_horz_0, &in_vert_0, &in_horz_1, &in_vert_1);

   in_vert_1 = in_vert_0; //force vertical to be equal on entry to
   updatePpuRepackOut(nu4k,blk,0,index, &in_horz_0, &in_vert_0);
   updatePpuRepackOut(nu4k,blk,1,index, &in_horz_1, &in_vert_1);
}

/*
   updates nu4k data base hybridFuncParams
*/
void nu4k_setDpeFuncParam(nu4kH h, unsigned int inst,XMLModesG_dpeFuncT *dpeFuncParamsP)
{
   nu4kT *nu4k = (nu4kT *)h;
   (void)inst;
   memcpy(&(nu4k->dpeFunc),dpeFuncParamsP,sizeof(XMLModesG_dpeFuncT));

   if (nu4k->dpeFunc.hybridFuncParams.enable==1)
   {
      setDbField(nu4k, NUFLD_DPE_E,inst, NU4100_DPE_HYBRID_CFG0_EN_E, 1); //enable hybrid
      setDbField(nu4k, NUFLD_DPE_E,inst, NU4100_DPE_MRG_CFG_MODE_E, 4); // enable merge
   }
}

static void hybridCfg(nu4kT *nu4k, unsigned int left_horz, unsigned int left_vert, unsigned int iau_right, unsigned int iau_left)
{
   //TODO: config hybrid mode sizes by system instructions
   UINT32 P0_internal_width, P0_output_width;
   UINT32 LEFT_EDGE= 12, CHIP_BUF_WIDTH=1280;
   XMLModesG_hybridFuncParamsT *hybridFuncParamsP=&(nu4k->dpeFunc.hybridFuncParams);

   if(hybridFuncParamsP->p1DownSampleFactor==1)
   {
      LOGG_PRINT(LOG_ERROR_E,NULL,"p1DownSampleFactor cannot equal to 1\n");
      return;
   }

   int P1_internal_width      =  (int)floor(left_horz / hybridFuncParamsP->p1DownSampleFactor);
   if (P1_internal_width%2)
      P1_internal_width--;
   P0_internal_width          =  CHIP_BUF_WIDTH-P1_internal_width;
   P0_output_width            =  P0_internal_width - hybridFuncParamsP->rightEdge - LEFT_EDGE;

   int scale0 = 0;
   int scale1 = 1;

   // --- DIF ---
   //Vertical dpe Input size
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CFG_IN_VSIZE_E,left_vert-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_CFG_IN_VSIZE_E,left_vert-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CFG_IN_VSIZE_E,left_vert-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_CFG_IN_VSIZE_E,left_vert-1);

   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CFG_CHANNEL_SEL_E,iau_left);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_CFG_CHANNEL_SEL_E,iau_left);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CFG_CHANNEL_SEL_E,iau_right);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_CFG_CHANNEL_SEL_E,iau_right);

   //p0 last pixel in hybrid line
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_HYBRID_CFG0_P0_SIZE_E, P0_internal_width-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_HYBRID_CFG1_AGG_P0_SIZE_E, P0_internal_width-1);
   //p1 last pixel in hybrid line
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_HYBRID_CFG0_P1_SIZE_E, CHIP_BUF_WIDTH-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_HYBRID_CFG1_AGG_P1_SIZE_E, CHIP_BUF_WIDTH-1);

   //range of p0 in hybrid row
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HYB_STOP_E, P0_internal_width-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HYB_STOP_E, P0_internal_width-1);
   //range of p1 in hybrid row
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_HYB_START_E, P0_internal_width);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_HYB_STOP_E, CHIP_BUF_WIDTH-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_HYB_START_E, P0_internal_width);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_HYB_STOP_E, CHIP_BUF_WIDTH-1);

   // L0 - Crop
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CROP_ENABLE_E,1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CROP_START_E,(left_horz/2) - (P0_output_width / 2) - LEFT_EDGE);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CROP_STOP_E,left_horz-1);

   // L0 - Scale
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CFG_SCALE_MODE_E, scale0);// Some setups require the Vertical's buffer. 0: Bypass, 2: Vertical Only
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG0_SCALE_E,65536);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_HSIZE_E,P0_internal_width-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG0_SCALE_E,65536);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG1_VSIZE_E,left_vert-1);

   // L1 - Crop
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_CROP_ENABLE_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_CROP_START_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_CROP_STOP_E,left_horz-1);

   // L1 - Scale
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_CFG_SCALE_MODE_E, scale1);// Some setups require the Vertical's buffer. 1: Horizontal, 4: Vert-->Horz
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG0_SCALE_E,(UINT32)(65536 * hybridFuncParamsP->p1DownSampleFactor));
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG1_HSIZE_E,P1_internal_width-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_VSCALE_CFG0_SCALE_E,65536);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_VSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P1_VSCALE_CFG1_VSIZE_E,left_vert-1);

   // R0 - Crop
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CROP_ENABLE_E,1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CROP_START_E,(left_horz/2) - (P0_output_width / 2) - LEFT_EDGE);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CROP_STOP_E,left_horz-1);

   // R0 - Scale
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CFG_SCALE_MODE_E, scale0);// Some setups require the Vertical's buffer. 0: Bypass, 2: Vertical Only
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG0_SCALE_E,65536);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG1_HSIZE_E,P0_internal_width-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG0_SCALE_E,65536);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG1_VSIZE_E,left_vert-1);

   // R1 - Crop
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_CROP_ENABLE_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_CROP_START_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_CROP_STOP_E,left_horz-1);

   // R1 - Scale
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_CFG_SCALE_MODE_E, scale1);// Some setups require the Vertical's buffer. 1: Horizontal, 4: Vert-->Horz
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG0_SCALE_E,(UINT32)(65536 * hybridFuncParamsP->p1DownSampleFactor));
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG1_HSIZE_E,P1_internal_width-1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_VSCALE_CFG0_SCALE_E,65536);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_VSCALE_CFG0_GAIN_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P1_VSCALE_CFG1_VSIZE_E,left_vert-1);

   // --- Merge ---
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_CFG_MRG_DISP_SCL_BYPASS_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_CFG_MRG_IMG_SCL_BYPASS_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_CFG_MRG_HYB_MRG_BYPASS_E, 0);

   //no restructure on p0
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_OFFSET_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_FACTOR_E, 1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_SHIFT_E, 0);

   //restructure on p1
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_RESCALE_OFFSET_E, 0);

   //calculate shift and factor for p1
   UINT32   shift = 0;
   double   over_ds_factor;
   over_ds_factor= (double)(hybridFuncParamsP->p1DownSampleFactor);

   double   factor=0;
   while(1)
   {
      factor = over_ds_factor * (1 << shift);
      if (factor>255)
      {
         shift--;
         factor = (int)(over_ds_factor * (1 << shift));
         break;
      }
      shift++;
   }
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_RESCALE_FACTOR_E, (int)factor);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_RESCALE_SHIFT_E, shift);

   //restructure (p0, p1): possible disparity range after restructure
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MAX_E, 143);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_OUT_CLIP_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_OUT_CLIP_MAX_E, (UINT32)ceil(143 * hybridFuncParamsP->p1DownSampleFactor));

   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_MRG_HDR_LIMITS_D0_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_MRG_HDR_LIMITS_D0_MAX_E, 143);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_MRG_HDR_LIMITS_D1_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_MRG_HDR_LIMITS_D1_MAX_E, (UINT32)ceil(143 * hybridFuncParamsP->p1DownSampleFactor));

   //rescale (p0, p1): location of rescale output in full output
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_START_E, (left_horz/2) - (P0_output_width / 2));
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_STOP_E, (left_horz/2) + (P0_output_width / 2) - 1);

   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_OUT_LOCATION_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_OUT_LOCATION_STOP_E, left_horz-1);

   //rescale (p0, p1): start of two parts before rescaling (depend on translator)
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_INT_PART_START_E, LEFT_EDGE);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_INT_PART_START_E, P0_internal_width);

   //rescale (p0, p1): rescaling factor (depend on translator)
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_SCALE_SCALE_E, 65536);

   if (hybridFuncParamsP->p1DownSampleFactor!=0)
   {
      setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_SCALE_SCALE_E, (UINT32)(65536/hybridFuncParamsP->p1DownSampleFactor));
   }
   else
   {
      LOGG_PRINT(LOG_ERROR_E,NULL," hybridFuncParams.p1DownSampleFactor cannot equal to 0\n");
      return;
   }
   //rescale (p0, p1): actual size of output of scaling
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_CFG_MERGE_SIZE_E, left_horz-1);
}


static void minDipsarityCfg(nu4kT *nu4k, unsigned int left_horz)
{
   unsigned int actual_frame_size;
   XMLModesG_minDispFuncParamsT  *minDispFuncParamsP=&(nu4k->dpeFunc.minDispFuncParams);

   actual_frame_size=left_horz-minDispFuncParamsP->minDisp;
   //crop the left from min_disp
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CROP_ENABLE_E,1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CROP_START_E,minDispFuncParamsP->minDisp);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_CROP_STOP_E,left_horz);

   //crop the right from 0
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CROP_ENABLE_E,1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CROP_START_E,0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_CROP_STOP_E,actual_frame_size);

   //p0 last pixel in hybrid line
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_HYBRID_CFG0_P0_SIZE_E, left_horz);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_HYBRID_CFG1_AGG_P0_SIZE_E, actual_frame_size);

   //range of p0 in hybrid row
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_LEFT_P0_HYB_STOP_E, actual_frame_size);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DIF_RIGHT_P0_HYB_STOP_E, actual_frame_size);

   // --- Merge ---
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_CFG_MRG_DISP_SCL_BYPASS_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_SCALE_SCALE_E, 0xFFFF);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_STOP_E, actual_frame_size);

   //no restructure on p0
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_OFFSET_E, minDispFuncParamsP->minDisp * 64);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_FACTOR_E, 1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_SHIFT_E, 0);

   //possible disparity range
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MAX_E, 0xFFFF);
}

static int census_factor_from_registers(UINT32 hd_shift1, UINT32 hd_shift2, UINT32 hd_sum_diff)
{
   if (hd_sum_diff == 0)
      return ((1 << hd_shift1) + (1 << hd_shift2));
   else
      return ((1 << hd_shift1) - (1 << hd_shift2));
}

static void aggregationSetupCfg(nu4kT *nu4k,int inst)
{
   XMLModesG_aggregationSetupParamsT *aggregationFuncParamsP = &(nu4k->dpeFunc.aggregationSetup[inst]);
   int a_shift, b_shift1, b_shift2, b_sum_diff, dyn_msk_en, edge_infomap_mode, hds1, hds2, sd, hd_shift1=0, hd_shift2=0, hd_sum_diff=0;
   int f_a_shift=0, f_b_shift1=0, f_b_shift2=0, f_b_sum_diff=0, diff_shift, f_diff_shift=0;
   float sad_weight, delta, max_value, mindelta;
   UINT32 side, lane;
   XMLDB_pathE diff_a_shift_e = XMLDB_NUM_PATHS_E, diff_b_shift1_e = XMLDB_NUM_PATHS_E, diff_b_shift2_e = XMLDB_NUM_PATHS_E, diff_shift_e = XMLDB_NUM_PATHS_E, b_sum_diff_e = XMLDB_NUM_PATHS_E,
      h_mask_left_e = XMLDB_NUM_PATHS_E, h_mask_right_e = XMLDB_NUM_PATHS_E, vmask_down_e = XMLDB_NUM_PATHS_E, vmask_up_e = XMLDB_NUM_PATHS_E,
      dyn_mask_en_e = XMLDB_NUM_PATHS_E, edge_inf_mode_e = XMLDB_NUM_PATHS_E, diff_clip_high_e = XMLDB_NUM_PATHS_E, diff_clip_low_e = XMLDB_NUM_PATHS_E,
      cencus_diff_shift1_e = XMLDB_NUM_PATHS_E, cencus_diff_shift2_e = XMLDB_NUM_PATHS_E, diff_hd_sum_e = XMLDB_NUM_PATHS_E;//initialzied for warnings

   mindelta = MIN_DELTA_INIT_VAL;//big number
   if ((aggregationFuncParamsP->SAD_census_ratio > 1) || (aggregationFuncParamsP->SAD_census_ratio < 0))
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "SAD_census_ratio must be 0~1'\n");
      assert(0);
   }

   for (a_shift = 0; a_shift < 8; a_shift++)
   {
      for (b_shift1 = 0; b_shift1 < 8; b_shift1++)
      {
         for (b_shift2 = 0; b_shift2 < b_shift1 + 1; b_shift2++)
         {
            for (b_sum_diff = 0; b_sum_diff < 2; b_sum_diff++)
            {
               if (b_sum_diff == 0)
               {
                  sad_weight = (float)(1 << a_shift) / ((1 << a_shift) + (1 << b_shift1) + (1 << b_shift2));
               }
               else if (((1 << a_shift) + (1 << b_shift1) - (1 << b_shift2)) > 0)
               {
                  sad_weight = (float)((1 << a_shift) / ((1 << a_shift) + (1 << b_shift1) - (1 << b_shift2)));
               }
               else
               {
                  sad_weight = -1;
               }
               if (sad_weight >= 0)
               {
                  delta = abs(aggregationFuncParamsP->SAD_census_ratio - sad_weight);
                  if ((mindelta == MIN_DELTA_INIT_VAL) || (delta < mindelta))
                  {
                     if (b_sum_diff == 0)
                     {
                        max_value = (float)((1023 << a_shift) + (1023 << b_shift1) + (1023 << b_shift2));
                     }
                     else
                     {
                        max_value = (float)((1023 << a_shift));
                     }
                     diff_shift = (int)((ceil(log2(max_value))) - 12);
                     if (diff_shift >= 0)
                     {
                        mindelta = delta;
                        f_a_shift = a_shift;
                        f_b_shift1 = b_shift1;
                        f_b_shift2 = b_shift2;
                        f_b_sum_diff = b_sum_diff;
                        f_diff_shift = diff_shift;
                     }
                  }
               }
            }
         }
      }
   }

   dyn_msk_en = 0;
   edge_infomap_mode = 0;
   if (aggregationFuncParamsP->agg_edge_mode == 1)
   {
      dyn_msk_en = 1;
      //edge_infomap_mode = 0
   }
   else if (aggregationFuncParamsP->agg_edge_mode == 2)
   {
      dyn_msk_en = 1;
      edge_infomap_mode = 1;
   }

   mindelta = MIN_DELTA_INIT_VAL;
   for (hds1 = 0; hds1 < 8; hds1++)
   {
      for (hds2 = 0; hds2 < hds1 + 1; hds2++)
      {
         for (sd = 0; sd < 2; sd++)
         {
            if (abs(census_factor_from_registers(hds1, hds2, sd) - aggregationFuncParamsP->census_factor) < mindelta)
            {
               mindelta = (float)(abs(census_factor_from_registers(hds1, hds2, sd) - aggregationFuncParamsP->census_factor));
               hd_shift1 = hds1;
               hd_shift2 = hds2;
               hd_sum_diff = sd;
            }
         }
      }
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
   for (lane = 0; lane < 2; lane++)// P0/P1
   {
      if (!IS_PX_MODE_ENABLED(lane, aggregationFuncParamsP->p0_p1_mode))
      {
         continue;
      }

      for (side = 0; side < 2; side++)// LEFT/RIGHT
      {
         if ((side == 0) && (lane == 0))
         {
            SET_DPE_AGG_OFFSETS(LEFT, 0, diff_a_shift_e, diff_b_shift1_e, diff_b_shift2_e, diff_shift_e, b_sum_diff_e, h_mask_left_e, h_mask_right_e, vmask_down_e, vmask_up_e, dyn_mask_en_e, edge_inf_mode_e, diff_clip_high_e, diff_clip_low_e, cencus_diff_shift1_e, cencus_diff_shift2_e, diff_hd_sum_e)
         }
         else if ((side == 1) && (lane == 0))
         {
            SET_DPE_AGG_OFFSETS(RIGHT, 0, diff_a_shift_e, diff_b_shift1_e, diff_b_shift2_e, diff_shift_e, b_sum_diff_e, h_mask_left_e, h_mask_right_e, vmask_down_e, vmask_up_e, dyn_mask_en_e, edge_inf_mode_e, diff_clip_high_e, diff_clip_low_e, cencus_diff_shift1_e, cencus_diff_shift2_e, diff_hd_sum_e)
         }
         else if ((side == 0) && (lane == 1))
         {
            SET_DPE_AGG_OFFSETS(LEFT, 1, diff_a_shift_e, diff_b_shift1_e, diff_b_shift2_e, diff_shift_e, b_sum_diff_e, h_mask_left_e, h_mask_right_e, vmask_down_e, vmask_up_e, dyn_mask_en_e, edge_inf_mode_e, diff_clip_high_e, diff_clip_low_e, cencus_diff_shift1_e, cencus_diff_shift2_e, diff_hd_sum_e)
         }
         else if ((side == 1) && (lane == 1))
         {
            SET_DPE_AGG_OFFSETS(RIGHT, 1, diff_a_shift_e, diff_b_shift1_e, diff_b_shift2_e, diff_shift_e, b_sum_diff_e, h_mask_left_e, h_mask_right_e, vmask_down_e, vmask_up_e, dyn_mask_en_e, edge_inf_mode_e, diff_clip_high_e, diff_clip_low_e, cencus_diff_shift1_e, cencus_diff_shift2_e, diff_hd_sum_e)
         }

         setDbField(nu4k, NUFLD_DPE_E, 0, diff_a_shift_e, f_a_shift);
         setDbField(nu4k, NUFLD_DPE_E, 0, diff_b_shift1_e, f_b_shift1);
         setDbField(nu4k, NUFLD_DPE_E, 0, diff_b_shift2_e, f_b_shift2);
         setDbField(nu4k, NUFLD_DPE_E, 0, diff_shift_e, f_diff_shift);
         setDbField(nu4k, NUFLD_DPE_E, 0, b_sum_diff_e, f_b_sum_diff);
         setDbField(nu4k, NUFLD_DPE_E, 0, h_mask_left_e, aggregationFuncParamsP->agg_win_horizontal / 2);
         setDbField(nu4k, NUFLD_DPE_E, 0, h_mask_right_e, aggregationFuncParamsP->agg_win_horizontal / 2);
         setDbField(nu4k, NUFLD_DPE_E, 0, vmask_down_e, aggregationFuncParamsP->agg_win_vertical / 2);
         setDbField(nu4k, NUFLD_DPE_E, 0, vmask_up_e, aggregationFuncParamsP->agg_win_vertical / 2);
         setDbField(nu4k, NUFLD_DPE_E, 0, dyn_mask_en_e, dyn_msk_en);
         setDbField(nu4k, NUFLD_DPE_E, 0, edge_inf_mode_e, edge_infomap_mode);
         setDbField(nu4k, NUFLD_DPE_E, 0, diff_clip_high_e, aggregationFuncParamsP->cost_clip_high);
         setDbField(nu4k, NUFLD_DPE_E, 0, diff_clip_low_e, aggregationFuncParamsP->cost_clip_low);
         setDbField(nu4k, NUFLD_DPE_E, 0, cencus_diff_shift1_e, hd_shift1);
         setDbField(nu4k, NUFLD_DPE_E, 0, cencus_diff_shift2_e, hd_shift2);
         setDbField(nu4k, NUFLD_DPE_E, 0, diff_hd_sum_e, hd_sum_diff);
#ifdef DEBUG_DPE_ALG
         const char *sides[] = {"left", "right"};
         const int lanes[] = {0, 1};
         printf("dpe_agg_%s_p%d_pxl_diff\na_shift = %d\nb_shift1 = %d\nb_shift2 = %d\ndiff_shift = %d\nb_sum_diff = %d\n", sides[side], lanes[lane], f_a_shift, f_b_shift1, f_b_shift2, (int)f_diff_shift, f_b_sum_diff);
         printf("dpe_agg_%s_p%d_mask\nhmask_left = %d\nhmask_right = %d\nvmask_down = %d\nvmask_up = %d\ndyn_msk_en = %d\n", sides[side], lanes[lane], aggregationFuncParamsP->agg_win_horizontal / 2, aggregationFuncParamsP->agg_win_horizontal / 2, aggregationFuncParamsP->agg_win_vertical / 2, aggregationFuncParamsP->agg_win_vertical / 2, dyn_msk_en);
         printf("dpe_agg_%s_p%d_cfg\nedge_infomap_mode = %d\n", sides[side], lanes[lane], edge_infomap_mode);
         printf("dpe_agg_%s_p%d_abs_diff_clip\nhigh = %d\nlow = %d\n", sides[side], lanes[lane], aggregationFuncParamsP->cost_clip_high, aggregationFuncParamsP->cost_clip_low);
         printf("dpe_agg_%s_p%d_census_diff\nhd_shift1 = %d\nhd_shift2 = %d\nhd_sum_diff = %d\n", sides[side], lanes[lane], hd_shift1, hd_shift2, hd_sum_diff);
#endif
      }
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void optimizationSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_optimizationSetupParamsT *optimizationFuncParamsP = &(nu4k->dpeFunc.optimizationSetup[inst]);
   UINT32 en_flag = 0;
   int p1_small = optimizationFuncParamsP->p1;
   int p2_small = optimizationFuncParamsP->p2;
   int p3_small = optimizationFuncParamsP->p3;

   if (optimizationFuncParamsP->opt_en)
      en_flag = 3;

   if (optimizationFuncParamsP->opt_edge_mask_en)
   {
      p1_small = (int)floor(p1_small / 2);
      p2_small = (int)floor(p2_small / 2);
      p3_small = (int)floor(p3_small / 2);
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
   if (IS_PX_MODE_ENABLED(0, optimizationFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_CTRL_OPT_EN_E, en_flag);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_PUNISH_P1_LARGE_VAL_E, optimizationFuncParamsP->p1);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_PUNISH_P1_SMALL_VAL_E, p1_small);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_PUNISH_P2_LARGE_VAL_E, optimizationFuncParamsP->p2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_PUNISH_P2_SMALL_VAL_E, p2_small);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_PUNISH_P3_LARGE_VAL_E, optimizationFuncParamsP->p3);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_ROPT_PUNISH_P3_SMALL_VAL_E, p3_small);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_ropt_ctrl\nopt_en = %d\n", en_flag);
      printf("dpe_dpe_p0_ropt_punish_p1\nlarge_val = %d\nsmall_val = %d\n", optimizationFuncParamsP->p1, p1_small);
      printf("dpe_dpe_p0_ropt_punish_p2\nlarge_val = %d\nsmall_val = %d\n", optimizationFuncParamsP->p2, p2_small);
      printf("dpe_dpe_p0_ropt_punish_p3\nlarge_val = %d\nsmall_val = %d\n", optimizationFuncParamsP->p3, p3_small);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, optimizationFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_CTRL_OPT_EN_E, en_flag);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_PUNISH_P1_LARGE_VAL_E, optimizationFuncParamsP->p1);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_PUNISH_P1_SMALL_VAL_E, p1_small);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_PUNISH_P2_LARGE_VAL_E, optimizationFuncParamsP->p2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_PUNISH_P2_SMALL_VAL_E, p2_small);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_PUNISH_P3_LARGE_VAL_E, optimizationFuncParamsP->p3);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_ROPT_PUNISH_P3_SMALL_VAL_E, p3_small);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_ropt_ctrl\nopt_en = %d\n", en_flag);
      printf("dpe_dpe_p1_ropt_punish_p1\nlarge_val = %d\nsmall_val = %d\n", optimizationFuncParamsP->p1, p1_small);
      printf("dpe_dpe_p1_ropt_punish_p2\nlarge_val = %d\nsmall_val = %d\n", optimizationFuncParamsP->p2, p2_small);
      printf("dpe_dpe_p1_ropt_punish_p3\nlarge_val = %d\nsmall_val = %d\n", optimizationFuncParamsP->p3, p3_small);
#endif
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void uniteSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_uniteSetupParamsT *uniteFuncParamsP = &(nu4k->dpeFunc.uniteSetup[inst]);
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, uniteFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_REG_UNITE_CFG_UNITE_THR_E, uniteFuncParamsP->unite_threshold);
#ifdef DEBUG_DPE_ALG
      printf("dpe_p0_reg_unite_cfg\nunite_thr = %d\n", uniteFuncParamsP->unite_threshold);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, uniteFuncParamsP->p0_p1_mode))
   {
       //P1
       setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_REG_UNITE_CFG_UNITE_THR_E, uniteFuncParamsP->unite_threshold);
#ifdef DEBUG_DPE_ALG
       printf("dpe_p1_reg_unite_cfg\nunite_thr = %d\n", uniteFuncParamsP->unite_threshold);
#endif
   }

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_UNITE_SIDE_E, uniteFuncParamsP->unite_side_left);
#ifdef DEBUG_DPE_ALG
   printf("dpe_cfg\nunite_side = %d\n", uniteFuncParamsP->unite_side_left);
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void smallFilterSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_smallFilterSetupParamsT *smallFilterFuncParamsP = &(nu4k->dpeFunc.smallFilterSetup[inst]);
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, smallFilterFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CFG_SMALLFILTER_EN_E, smallFilterFuncParamsP->small_filter_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG_UNITE_THR_E, smallFilterFuncParamsP->unite_threshold);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_DIR_EDGE_DISP_THR_E, smallFilterFuncParamsP->disp_threshold);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_MIN_EDGES_COUNT_E, smallFilterFuncParamsP->min_edges_count);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_MAX_EDGES_COUNT_E, smallFilterFuncParamsP->max_edges_count);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_MAX_WIN_SIZE_E, smallFilterFuncParamsP->max_win_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_DIL_WIN_SIZE_E, smallFilterFuncParamsP->dil_win_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_CLEANING_DISP_THR_E, smallFilterFuncParamsP->cleaning_disp_threshold);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_CLEAN_FLT_SIZE_E, smallFilterFuncParamsP->clean_flt_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SMALL_DISP_CFG1_CLEAN_FLT_EN_E, smallFilterFuncParamsP->clean_flt_en);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_cfg\nsmallfilter_en = %d\n", smallFilterFuncParamsP->small_filter_en);
      printf("dpe_dpe_p0_small_disp_cfg\nunite_thr = %d\n", smallFilterFuncParamsP->unite_threshold);
      printf("dpe_dpe_p0_small_disp_cfg1\n");
      printf("dir_edge_disp_thr = %d\n", smallFilterFuncParamsP->disp_threshold);
      printf("min_edges_count = %d\n", smallFilterFuncParamsP->min_edges_count);
      printf("max_edges_count = %d\n", smallFilterFuncParamsP->max_edges_count);
      printf("max_win_size = %d\n", smallFilterFuncParamsP->max_win_size);
      printf("dil_win_size = %d\n", smallFilterFuncParamsP->dil_win_size);
      printf("cleaning_disp_thr = %d\n", smallFilterFuncParamsP->cleaning_disp_threshold);
      printf("clean_flt_size = %d\n", smallFilterFuncParamsP->clean_flt_size);
      printf("clean_flt_en = %d\n", smallFilterFuncParamsP->clean_flt_en);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, smallFilterFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CFG_SMALLFILTER_EN_E, smallFilterFuncParamsP->small_filter_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG_UNITE_THR_E, smallFilterFuncParamsP->unite_threshold);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_DIR_EDGE_DISP_THR_E, smallFilterFuncParamsP->disp_threshold);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_MIN_EDGES_COUNT_E, smallFilterFuncParamsP->min_edges_count);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_MAX_EDGES_COUNT_E, smallFilterFuncParamsP->max_edges_count);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_MAX_WIN_SIZE_E, smallFilterFuncParamsP->max_win_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_DIL_WIN_SIZE_E, smallFilterFuncParamsP->dil_win_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_CLEANING_DISP_THR_E, smallFilterFuncParamsP->cleaning_disp_threshold);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_CLEAN_FLT_SIZE_E, smallFilterFuncParamsP->clean_flt_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SMALL_DISP_CFG1_CLEAN_FLT_EN_E, smallFilterFuncParamsP->clean_flt_en);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_cfg\nsmallfilter_en = %d\n", smallFilterFuncParamsP->small_filter_en);
      printf("dpe_dpe_p1_small_disp_cfg\nunite_thr = %d\n", smallFilterFuncParamsP->unite_threshold);
      printf("dpe_dpe_p1_small_disp_cfg1\n");
      printf("dir_edge_disp_thr = %d\n", smallFilterFuncParamsP->disp_threshold);
      printf("min_edges_count = %d\n", smallFilterFuncParamsP->min_edges_count);
      printf("max_edges_count = %d\n", smallFilterFuncParamsP->max_edges_count);
      printf("max_win_size = %d\n", smallFilterFuncParamsP->max_win_size);
      printf("dil_win_size = %d\n", smallFilterFuncParamsP->dil_win_size);
      printf("cleaning_disp_thr = %d\n", smallFilterFuncParamsP->cleaning_disp_threshold);
      printf("clean_flt_size = %d\n", smallFilterFuncParamsP->clean_flt_size);
      printf("clean_flt_en = %d\n", smallFilterFuncParamsP->clean_flt_en);
#endif
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void blobSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_blobSetupParamsT *blobFuncParamsP = &(nu4k->dpeFunc.blobSetup[inst]);
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, blobFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_BLOB0_CFG0_ENABLE_E, blobFuncParamsP->blob_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_BLOB0_CFG0_MAX_SIZE_E, blobFuncParamsP->blob_max_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_BLOB0_CFG0_DISP_DIFF_THR_E, blobFuncParamsP->blob_disp_diff);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_BLOB0_CFG1_LIN_DIST_E, (blobFuncParamsP->blob_buffer_height - 1));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_BLOB0_CFG1_MAX_LIST_SIZE_E, blobFuncParamsP->blob_buffer_height);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_blob0_cfg0\nenable = %d\n", blobFuncParamsP->blob_en);
      printf("max_size = %d\n", blobFuncParamsP->blob_max_size);
      printf("disp_diff_thr = %d\n", blobFuncParamsP->blob_disp_diff);
      printf("dpe_dpe_p0_blob0_cfg1\nlin_dist = %d\n", (blobFuncParamsP->blob_buffer_height - 1));
      printf("max_list_size = %d\n", blobFuncParamsP->blob_buffer_height);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, blobFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_BLOB0_CFG0_ENABLE_E, blobFuncParamsP->blob_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_BLOB0_CFG0_MAX_SIZE_E, blobFuncParamsP->blob_max_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_BLOB0_CFG0_DISP_DIFF_THR_E, blobFuncParamsP->blob_disp_diff);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_BLOB0_CFG1_LIN_DIST_E, (blobFuncParamsP->blob_buffer_height - 1));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_BLOB0_CFG1_MAX_LIST_SIZE_E, blobFuncParamsP->blob_buffer_height);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_blob0_cfg0\nenable = %d\n", blobFuncParamsP->blob_en);
      printf("max_size = %d\n", blobFuncParamsP->blob_max_size);
      printf("disp_diff_thr = %d\n", blobFuncParamsP->blob_disp_diff);
      printf("dpe_dpe_p1_blob0_cfg1\nlin_dist = %d\n", (blobFuncParamsP->blob_buffer_height - 1));
      printf("max_list_size = %d\n", blobFuncParamsP->blob_buffer_height);
#endif
   }

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_BLB_CFG_START_LINE_CIIF_0_E, blobFuncParamsP->blob_buffer_height);

#ifdef DEBUG_DPE_ALG
   printf("dpe_blb_cfg\nstart_line_ciif_0 = %d\n", blobFuncParamsP->blob_buffer_height);

   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void edgeDetectSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_edgeDetectSetupParamsT *edgeDetectFuncParamsP = &(nu4k->dpeFunc.edgeDetectSetup[inst]);
   // internal number of bit shifts
   int canny_gf_shift = 16;
   // this sum should be 256 for 16 bits shift
   int filter_sum = 1 << (canny_gf_shift / 2);
   int index_range[7] = {-3, -2, -1, 0, 1, 2, 3};
   int i = 0;
   float index_range_normal[7] = {};
   float index_range_normal_normalized = 0;
   int filter[7] = {};
   float index_range_normal_sum = 0;
   float inv_gauss_filt_sigma_times_sqrt_2_pi = (1 / (edgeDetectFuncParamsP->gauss_filt_sigma * sqrt(2 * PI_FOLAT)));
   float gauss_filt_sigma_pow_2_times_2 = (2 * edgeDetectFuncParamsP->gauss_filt_sigma * edgeDetectFuncParamsP->gauss_filt_sigma);

#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
   // matlab formula:
   // Filter = round(filter_sum*((1/(gauss_filt_sigma*sqrt(2*pi))) * exp(-0.5*([-3:3].^2/(gauss_filt_sigma^2)))) ./ sum((1/(gauss_filt_sigma*sqrt(2*pi))) * exp(-0.5*([-3:3].^2/(gauss_filt_sigma^2)))))
   for (i = 0; i < 7; i++)
   {
      // Python: index_range_normal = (1 / (gauss_filt_sigma * np.sqrt(2 * np.pi))) * np.exp(-np.power(index_range, 2.0) / (2 * gauss_filt_sigma * gauss_filt_sigma))
      index_range_normal[i] = inv_gauss_filt_sigma_times_sqrt_2_pi * exp(-pow(index_range[i], 2.0) / gauss_filt_sigma_pow_2_times_2);
      // Python: np.sum(index_range_normal)
      index_range_normal_sum += index_range_normal[i];
   }

   for (i = 0; i < 7; i++)
   {
      // Python: index_range_normal_normalized = index_range_normal / np.sum(index_range_normal)
      index_range_normal_normalized = index_range_normal[i] / index_range_normal_sum;
      // Python: filter = np.round(filter_sum * index_range_normal_normalized).astype(int)
      // (UINT32)(x + 0.5) is needed for round.
      filter[i] = round(index_range_normal_normalized * filter_sum);
   }

   if (IS_PX_MODE_ENABLED(0, edgeDetectFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_CFG_CANNY_GF_SHIFT_E, canny_gf_shift);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_HOR_COEF0_VAL_E, filter[3]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_HOR_COEF1_VAL_E, filter[2]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_HOR_COEF2_VAL_E, filter[1]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_HOR_COEF3_VAL_E, filter[0]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_VER_COEF0_VAL_E, filter[3]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_VER_COEF1_VAL_E, filter[2]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_VER_COEF2_VAL_E, filter[1]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_GAUS_VER_COEF3_VAL_E, filter[0]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_CFG_CENSUS_GF_EN_E, edgeDetectFuncParamsP->filt_before_census_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_THRS_STRONG_EDGE_THR_E, edgeDetectFuncParamsP->canny_thr_high);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_EDGE_CANNY_THRS_WEAK_EDGE_THR_E, edgeDetectFuncParamsP->canny_thr_low);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_edge_canny_cfg\ncanny_gf_shift = %d\n", canny_gf_shift);
      printf("census_gf_en = %d\n", edgeDetectFuncParamsP->filt_before_census_en);
      printf("dpe_dpe_p0_edge_canny_gaus_hor_coef0\nval = %d\n", filter[3]);
      printf("dpe_dpe_p0_edge_canny_gaus_hor_coef1\nval = %d\n", filter[2]);
      printf("dpe_dpe_p0_edge_canny_gaus_hor_coef2\nval = %d\n", filter[1]);
      printf("dpe_dpe_p0_edge_canny_gaus_hor_coef3\nval = %d\n", filter[0]);
      printf("dpe_dpe_p0_edge_canny_gaus_ver_coef0\nval = %d\n", filter[3]);
      printf("dpe_dpe_p0_edge_canny_gaus_ver_coef1\nval = %d\n", filter[2]);
      printf("dpe_dpe_p0_edge_canny_gaus_ver_coef2\nval = %d\n", filter[1]);
      printf("dpe_dpe_p0_edge_canny_gaus_ver_coef3\nval = %d\n", filter[0]);
      printf("dpe_dpe_p0_edge_canny_thrs\nstrong_edge_thr = %d\n", edgeDetectFuncParamsP->canny_thr_high);
      printf("weak_edge_thr = %d\n", edgeDetectFuncParamsP->canny_thr_low);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, edgeDetectFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_CFG_CANNY_GF_SHIFT_E, canny_gf_shift);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_HOR_COEF0_VAL_E, filter[3]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_HOR_COEF1_VAL_E, filter[2]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_HOR_COEF2_VAL_E, filter[1]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_HOR_COEF3_VAL_E, filter[0]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_VER_COEF0_VAL_E, filter[3]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_VER_COEF1_VAL_E, filter[2]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_VER_COEF2_VAL_E, filter[1]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_GAUS_VER_COEF3_VAL_E, filter[0]);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_CFG_CENSUS_GF_EN_E, edgeDetectFuncParamsP->filt_before_census_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_THRS_STRONG_EDGE_THR_E, edgeDetectFuncParamsP->canny_thr_high);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_EDGE_CANNY_THRS_WEAK_EDGE_THR_E, edgeDetectFuncParamsP->canny_thr_low);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_edge_canny_cfg\ncanny_gf_shift = %d\n", canny_gf_shift);
      printf("census_gf_en = %d\n", edgeDetectFuncParamsP->filt_before_census_en);
      printf("dpe_dpe_p1_edge_canny_gaus_hor_coef0\nval = %d\n", filter[3]);
      printf("dpe_dpe_p1_edge_canny_gaus_hor_coef1\nval = %d\n", filter[2]);
      printf("dpe_dpe_p1_edge_canny_gaus_hor_coef2\nval = %d\n", filter[1]);
      printf("dpe_dpe_p1_edge_canny_gaus_hor_coef3\nval = %d\n", filter[0]);
      printf("dpe_dpe_p1_edge_canny_gaus_ver_coef0\nval = %d\n", filter[3]);
      printf("dpe_dpe_p1_edge_canny_gaus_ver_coef1\nval = %d\n", filter[2]);
      printf("dpe_dpe_p1_edge_canny_gaus_ver_coef2\nval = %d\n", filter[1]);
      printf("dpe_dpe_p1_edge_canny_gaus_ver_coef3\nval = %d\n", filter[0]);
      printf("dpe_dpe_p1_edge_canny_thrs\nstrong_edge_thr = %d\n", edgeDetectFuncParamsP->canny_thr_high);
      printf("weak_edge_thr = %d\n", edgeDetectFuncParamsP->canny_thr_low);
#endif
   }

#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}



static UINT64 mask2d_to_reg_parse(UINT8 mask2d[WIN_SIZE][WIN_SIZE], INT8 enumerateds[WIN_SIZE][WIN_SIZE])
{
   UINT8 i, j;
   UINT64 reg = 0;

   for (i = 0; i < WIN_SIZE; i++)
   {
      for (j = 0; j < WIN_SIZE; j++)
      {
         if(mask2d[i][j] && (enumerateds[i][j] != -1))
         {
            reg |= (1ULL << enumerateds[i][j]);
         }
      }
   }

   return reg;
}

static UINT64 mask2d_to_reg_ang_box(UINT8 mask2d[WIN_SIZE][WIN_SIZE])
{
   INT8 enumerateds[WIN_SIZE][WIN_SIZE] =   {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, 60, 59, 58, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 57, 56, 55, 54, 53, 52, 51, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 50, 49, 48, 47, 46, 45, 44, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, 43, 42, 41, 40, 39, 38, 37, 36, 35, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, 34, 33, 32, 31, 30, 29, 28, 27, 26, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, 25, 24, 23, 22, 21, 20, 19, 18, 17, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 16, 15, 14, 13, 12, 11, 10, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1,  9,  8,  7,  6,  5,  4,  3, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1,  2,  1,  0, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   return mask2d_to_reg_parse(mask2d, enumerateds);
}

static UINT64 mask2d_to_reg_ang_0(UINT8 mask2d[WIN_SIZE][WIN_SIZE])
{
   INT8 enumerateds[WIN_SIZE][WIN_SIZE] =   {{-1, -1, -1, -1, -1, 62, 61, 60, 59, 58, 57, 56, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 55, 54, 53, 52, 51, 50, 49, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 48, 47, 46, 45, 44, 43, 42, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 41, 40, 39, 38, 37, 36, 35, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 34, 33, 32, 31, 30, 29, 28, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 27, 26, 25, 24, 23, 22, 21, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 20, 19, 18, 17, 16, 15, 14, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, 13, 12, 11, 10,  9,  8,  7, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1,  6,  5,  4,  3,  2,  1,  0, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   return mask2d_to_reg_parse(mask2d, enumerateds);
}

static UINT64 mask2d_to_reg_ang_22(UINT8 mask2d[WIN_SIZE][WIN_SIZE])
{
   INT8 enumerateds[WIN_SIZE][WIN_SIZE] =   {{-1, -1, -1, -1, -1, -1, -1, -1, 59, 58, 62, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, 46, 52, 51, 57, 56, 61, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, 39, 45, 44, 50, 49, 55, 54, 60, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, 32, 38, 37, 43, 42, 48, 47, 53, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, 26, 25, 31, 30, 36, 35, 41, 40, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, 19, 18, 24, 23, 29, 28, 34, 33, -1, -1, -1},
                                             {-1, -1, -1, -1, -1,  6, 12, 11, 17, 16, 22, 21, 27, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1,  5,  4, 10,  9, 15, 14, 20, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1,  3,  2,  8,  7, 13, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  0, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   return mask2d_to_reg_parse(mask2d, enumerateds);
}

static UINT64 mask2d_to_reg_ang_45(UINT8 mask2d[WIN_SIZE][WIN_SIZE])
{
   INT8 enumerateds[WIN_SIZE][WIN_SIZE] =   {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 54, 59, 63, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, 43, 48, 53, 58, 62, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, 32, 37, 42, 47, 52, 57, 61, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, 21, 26, 31, 36, 41, 46, 51, 56, 60, -1},
                                             {-1, -1, -1, -1, -1, -1, 10, 15, 20, 25, 30, 35, 40, 45, 50, 55, -1},
                                             {-1, -1, -1, -1, -1, -1,  4,  9, 14, 19, 24, 29, 34, 39, 44, 49, -1},
                                             {-1, -1, -1, -1, -1, -1, -1,  3,  8, 13, 18, 23, 28, 33, 38, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1,  2,  7, 12, 17, 22, 27, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  6, 11, 16, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0,  5, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   return mask2d_to_reg_parse(mask2d, enumerateds);
}

static UINT64 mask2d_to_reg_ang_67(UINT8 mask2d[WIN_SIZE][WIN_SIZE])
{
   INT8 enumerateds[WIN_SIZE][WIN_SIZE] =   {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 59, 62, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 39, 46, 52, 58, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, 19, 26, 32, 38, 45, 51, 57, 61, -1},
                                             {-1, -1, -1, -1, -1, -1, -1,  6, 12, 18, 25, 31, 37, 44, 50, 56, -1},
                                             {-1, -1, -1, -1, -1, -1, -1,  5, 11, 17, 24, 30, 36, 43, 49, 55, 60},
                                             {-1, -1, -1, -1, -1, -1, -1, -1,  4, 10, 16, 23, 29, 35, 42, 48, 54},
                                             {-1, -1, -1, -1, -1, -1, -1, -1,  3,  9, 15, 22, 28, 34, 41, 47, 53},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1,  2,  8, 14, 21, 27, 33, 40, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1,  1,  7, 13, 20, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,  0, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
                                             {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

   return mask2d_to_reg_parse(mask2d, enumerateds);
}

static UINT64 mask2d_to_reg(UINT8 mask2d[WIN_SIZE][WIN_SIZE], nu4kAngIdxE ang)
{
   UINT64 reg = 0;

   switch (ang)
   {
   case NU4K_ANG_BOX:
      reg = mask2d_to_reg_ang_box(mask2d);
      break;
   case NU4K_ANG_0:
      reg = mask2d_to_reg_ang_0(mask2d);
      break;
   case NU4K_ANG_22:
      reg = mask2d_to_reg_ang_22(mask2d);
      break;
   case NU4K_ANG_45:
      reg = mask2d_to_reg_ang_45(mask2d);
      break;
   case NU4K_ANG_67:
      reg = mask2d_to_reg_ang_67(mask2d);
      break;
   default:
      reg = 0;
      break;
   }

   return reg;
}

static void rotate_win(UINT8 mask2d[WIN_SIZE][WIN_SIZE], nu4kAngIdxE ang)
{
   double cosine[NU4K_ANG_MAX] = {0.9238795325112867, 0.7071067811865476, 0.38268343236508984};
   double sine[NU4K_ANG_MAX] = {-0.3826834323650898, -0.7071067811865476, -0.9238795325112867};
   UINT8 c_i = WIN_CENTER;
   UINT8 c_j = WIN_CENTER;
   UINT8 i, j;
   INT8 i2, j2;
   double v_i, v_j;
   double v_x, v_y;
   double v_x2, v_y2;
   UINT8 mask2d_temp[WIN_SIZE][WIN_SIZE];
   double cs = cosine[ang];
   double sn = sine[ang];

   memset(mask2d_temp, 0, (WIN_SIZE * WIN_SIZE));


   for (i = 0; i < WIN_SIZE; i++)
   {
      for (j = 0; j < WIN_SIZE; j++)
      {
            v_i = i - c_i;
            v_j = j - c_j;

            v_x =  v_j;
            v_y = -v_i;

            v_x2 =  cs * v_x + sn * v_y;
            v_y2 = -sn * v_x + cs * v_y;

            // use nearest neighbor for simplicity
            i2 = (UINT8)round(c_i - v_y2);
            j2 = (UINT8)round(c_j + v_x2);

            if ((i2 >= 0) && (i2 < WIN_SIZE) && (j2 >= 0) && (j2 < WIN_SIZE))
            {
               mask2d_temp[i][j] = mask2d[i2][j2];
            }
      }
   }

   memcpy(mask2d, mask2d_temp, (WIN_SIZE * WIN_SIZE));
}

static UINT64 build_reg(nu4kAngIdxE ang, UINT32 length, UINT32 width)
{
   UINT32 i, j;
   UINT32 i_start, i_end;
   UINT32 j_start, j_end;
   UINT8 mask2d[WIN_SIZE][WIN_SIZE];

   memset(mask2d, 0, (WIN_SIZE * WIN_SIZE));

   if (ang == NU4K_ANG_BOX)
   {
      i_start  = WIN_CENTER - (length / 2);
      i_end    = WIN_CENTER + (length / 2) + 1;
   }
   else
   {
      i_start  = WIN_CENTER - length + 1;
      i_end    = WIN_CENTER + 1;
   }

   j_start  = WIN_CENTER - (width / 2);
   j_end    = WIN_CENTER + (width / 2) + 1;

   for (i = i_start; i < i_end; i++)
   {
      for (j = j_start; j < j_end; j++)
      {
         mask2d[i][j] = 1;
      }
   }

   if (ang < NU4K_ANG_MAX)
   {
      rotate_win(mask2d, ang);
   }

   return mask2d_to_reg(mask2d, ang);

}

static double bit_count64(UINT64 reg)
{
   double cnt = 0;

   while (reg != 0)
   {
      cnt += (reg & 0x1);
      reg = reg >> 1;
   }

   return cnt;
}

static UINT32 calc_normal_factor(UINT64 reg)
{
    double n = 0;
    double var_normal_bits = (double)(1 << 12);

    n = bit_count64(reg);

    if (n)
    {
        return round(var_normal_bits / n);
    }
    else
    {
        return 0;
    }
}

static UINT32 calc_normal_factor2(UINT64 reg)
{
    double n = 0;
    double var_normal_bits2 = (double)(1 << 19);

    n = bit_count64(reg);

    if (n)
    {
        return round(var_normal_bits2 / (n * n));
    }
    else
    {
        return 0;
    }
}

static void infoMapSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_infoMapParamsT *infoMapFuncParamsP = &(nu4k->dpeFunc.infoMapSetup[inst]);
   UINT32 cond_precision = 7;
   UINT32 eps_o_precision = 4;
   UINT64 mask_box, mask_0, mask_22, mask_45, mask_67;
   UINT32 nf_box, nf_0, nf_22, nf_45, nf_67;
   UINT32 nf2_box, nf2_0, nf2_22, nf2_45, nf2_67;
   UINT32 im_t2, eps_t, eps_o, eps_a, min_ratio, min_vc, max_ratio, max_vc;
   float im_t = infoMapFuncParamsP->im_t;
   float float_im_t2 = im_t * im_t;
   float float_eps_t = infoMapFuncParamsP->eps_v * im_t;
   UINT32 box_size = infoMapFuncParamsP->box_size;
   UINT32 length = infoMapFuncParamsP->length;
   UINT32 width = infoMapFuncParamsP->width;

   mask_box = build_reg(NU4K_ANG_BOX, box_size, box_size);
   mask_0   = build_reg(NU4K_ANG_0, length, width);
   mask_22  = build_reg(NU4K_ANG_22, length, width);
   mask_45  = build_reg(NU4K_ANG_45, length, width);
   mask_67  = build_reg(NU4K_ANG_67, length, width);
   nf_box   = calc_normal_factor(mask_box);
   nf_0     = calc_normal_factor(mask_0);
   nf_22    = calc_normal_factor(mask_22);
   nf_45    = calc_normal_factor(mask_45);
   nf_67    = calc_normal_factor(mask_67);
   nf2_box  = calc_normal_factor2(mask_box);
   nf2_0    = calc_normal_factor2(mask_0);
   nf2_22   = calc_normal_factor2(mask_22);
   nf2_45   = calc_normal_factor2(mask_45);
   nf2_67   = calc_normal_factor2(mask_67);

   im_t2 = round(float_im_t2 * (1 << cond_precision));
   eps_t = round(float_eps_t * (1 << cond_precision));
   eps_o = round(infoMapFuncParamsP->eps_o * (1 << eps_o_precision));
   eps_a = round(infoMapFuncParamsP->eps_a * (1 << cond_precision));
   min_ratio = round(infoMapFuncParamsP->var_min_ratio * (1 << cond_precision));
   min_vc = round(infoMapFuncParamsP->var_min_offset * (1 << cond_precision));
   max_ratio = round(infoMapFuncParamsP->var_max_ratio * (1 << cond_precision));
   max_vc = round(infoMapFuncParamsP->var_max_offset * (1 << cond_precision));

#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, infoMapFuncParamsP->p0_p1_mode))
   {
      //P0
      //LEFT
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_AGG_LEFT_P0_CFG_INFOMAP_INV_EN_E, infoMapFuncParamsP->infomap_inv_en);

      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK0_LSB_MSK_E, MASK_64_LSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK0_MSB_MSK_E, MASK_64_MSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_0_Y2_E, nf_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_0_Y_E, nf2_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK1_LSB_MSK_E, MASK_64_LSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK1_MSB_MSK_E, MASK_64_MSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_1_Y2_E, nf_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_1_Y_E, nf2_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK2_LSB_MSK_E, MASK_64_LSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK2_MSB_MSK_E, MASK_64_MSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_2_Y2_E, nf_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_2_Y_E, nf2_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK3_LSB_MSK_E, MASK_64_LSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK3_MSB_MSK_E, MASK_64_MSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_3_Y2_E, nf_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_3_Y_E, nf2_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK4_LSB_MSK_E, MASK_64_LSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_EN_MSK4_MSB_MSK_E, MASK_64_MSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_4_Y2_E, nf_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_NORM_4_Y_E, nf2_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_CFG_UIM_LOW_HIGH_VAR_SEL_E, infoMapFuncParamsP->low_high_var_sel);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_CFG_IM_CLOSE_EN_E, infoMapFuncParamsP->close_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_EPS_A_O_A_E, eps_a);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_EPS_A_O_O_E, eps_o);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_EPS_T_VAL_E, eps_t);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_T2_VAL_E, im_t2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_MAX_CFG_OFFSET_E, max_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_MAX_CFG_RATIO_E, max_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_MIN_CFG_OFFSET_E, min_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_VAR_MIN_CFG_RATIO_E, min_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_BLOB_CFG_ENABLE_E, infoMapFuncParamsP->blob_enable);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_BLOB_CFG_MAX_LIST_SIZE_E, infoMapFuncParamsP->max_list_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_BLOB_CFG_LIN_DIST_E, infoMapFuncParamsP->lin_dist);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P0_UIM_BLOB_CFG_MAX_SIZE_E, infoMapFuncParamsP->max_size);

      //RIGHT
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_AGG_RIGHT_P0_CFG_INFOMAP_INV_EN_E, infoMapFuncParamsP->infomap_inv_en);

      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK0_LSB_MSK_E, MASK_64_LSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK0_MSB_MSK_E, MASK_64_MSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_0_Y2_E, nf_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_0_Y_E, nf2_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK1_LSB_MSK_E, MASK_64_LSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK1_MSB_MSK_E, MASK_64_MSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_1_Y2_E, nf_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_1_Y_E, nf2_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK2_LSB_MSK_E, MASK_64_LSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK2_MSB_MSK_E, MASK_64_MSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_2_Y2_E, nf_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_2_Y_E, nf2_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK3_LSB_MSK_E, MASK_64_LSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK3_MSB_MSK_E, MASK_64_MSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_3_Y2_E, nf_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_3_Y_E, nf2_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK4_LSB_MSK_E, MASK_64_LSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_EN_MSK4_MSB_MSK_E, MASK_64_MSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_4_Y2_E, nf_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_NORM_4_Y_E, nf2_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_CFG_UIM_LOW_HIGH_VAR_SEL_E, infoMapFuncParamsP->low_high_var_sel);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_CFG_IM_CLOSE_EN_E, infoMapFuncParamsP->close_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_EPS_A_O_A_E, eps_a);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_EPS_A_O_O_E, eps_o);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_EPS_T_VAL_E, eps_t);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_T2_VAL_E, im_t2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_MAX_CFG_OFFSET_E, max_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_MAX_CFG_RATIO_E, max_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_MIN_CFG_OFFSET_E, min_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_VAR_MIN_CFG_RATIO_E, min_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_BLOB_CFG_ENABLE_E, infoMapFuncParamsP->blob_enable);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_BLOB_CFG_MAX_LIST_SIZE_E, infoMapFuncParamsP->max_list_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_BLOB_CFG_LIN_DIST_E, infoMapFuncParamsP->lin_dist);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P0_UIM_BLOB_CFG_MAX_SIZE_E, infoMapFuncParamsP->max_size);

#ifdef DEBUG_DPE_ALG
      const char *sides[] = {"left", "right"};
      int side = 0;
      for (side = 0; side < 2; side++)// LEFT/RIGHT
      {
         printf("dpe_agg_%s_p0_cfg\ninfomap_inv_en = %d\n", sides[side], infoMapFuncParamsP->infomap_inv_en);
         printf("dpe_uim_%s_p0_uim_var_en_msk0_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_0));
         printf("dpe_uim_%s_p0_uim_var_en_msk0_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_0));
         printf("dpe_uim_%s_p0_uim_var_norm_0\ny2 = %d\ny = %d\n", sides[side], nf_0, nf2_0);
         printf("dpe_uim_%s_p0_uim_var_en_msk1_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_22));
         printf("dpe_uim_%s_p0_uim_var_en_msk1_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_22));
         printf("dpe_uim_%s_p0_uim_var_norm_1\ny2 = %d\ny = %d\n", sides[side], nf_22, nf2_22);
         printf("dpe_uim_%s_p0_uim_var_en_msk2_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_45));
         printf("dpe_uim_%s_p0_uim_var_en_msk2_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_45));
         printf("dpe_uim_%s_p0_uim_var_norm_2\ny2 = %d\ny = %d\n", sides[side], nf_45, nf2_45);
         printf("dpe_uim_%s_p0_uim_var_en_msk3_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_67));
         printf("dpe_uim_%s_p0_uim_var_en_msk3_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_67));
         printf("dpe_uim_%s_p0_uim_var_norm_3\ny2 = %d\ny = %d\n", sides[side], nf_67, nf2_67);
         printf("dpe_uim_%s_p0_uim_var_en_msk4_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_box));
         printf("dpe_uim_%s_p0_uim_var_en_msk4_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_box));
         printf("dpe_uim_%s_p0_uim_var_norm_4\ny2 = %d\ny = %d\n", sides[side], nf_box, nf2_box);
         printf("dpe_uim_%s_p0_uim_cfg\nuim_low_high_var_sel = %d\nim_close_en = %d\n",
                sides[side], infoMapFuncParamsP->low_high_var_sel, infoMapFuncParamsP->close_en);
         printf("dpe_uim_%s_p0_uim_eps_a_o\na = %d\no = %d\n", sides[side], eps_a, eps_o);
         printf("dpe_uim_%s_p0_uim_eps_t\nval = %d\n", sides[side], eps_t);
         printf("dpe_uim_%s_p0_uim_t2\nval = %d\n", sides[side], im_t2);
         printf("dpe_uim_%s_p0_uim_var_max_cfg\noffset = %d\nratio = %d\n", sides[side], max_vc, max_ratio);
         printf("dpe_uim_%s_p0_uim_var_min_cfg\noffset = %d\nratio = %d\n", sides[side], min_vc, min_ratio);
         printf("dpe_uim_%s_p0_uim_blob_cfg\nenable = %d\nmax_list_size = %d\nlin_dist = %d\nmax_size = %d\n",
                sides[side], infoMapFuncParamsP->blob_enable, infoMapFuncParamsP->max_list_size, infoMapFuncParamsP->lin_dist, infoMapFuncParamsP->max_size);
      }
#endif//DEBUG_DPE_ALG
   }

   if (IS_PX_MODE_ENABLED(1, infoMapFuncParamsP->p0_p1_mode))
   {
      //P1
      //LEFT
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_AGG_LEFT_P1_CFG_INFOMAP_INV_EN_E, infoMapFuncParamsP->infomap_inv_en);

      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK0_LSB_MSK_E, MASK_64_LSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK0_MSB_MSK_E, MASK_64_MSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_0_Y2_E, nf_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_0_Y_E, nf2_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK1_LSB_MSK_E, MASK_64_LSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK1_MSB_MSK_E, MASK_64_MSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_1_Y2_E, nf_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_1_Y_E, nf2_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK2_LSB_MSK_E, MASK_64_LSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK2_MSB_MSK_E, MASK_64_MSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_2_Y2_E, nf_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_2_Y_E, nf2_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK3_LSB_MSK_E, MASK_64_LSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK3_MSB_MSK_E, MASK_64_MSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_3_Y2_E, nf_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_3_Y_E, nf2_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK4_LSB_MSK_E, MASK_64_LSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_EN_MSK4_MSB_MSK_E, MASK_64_MSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_4_Y2_E, nf_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_NORM_4_Y_E, nf2_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_CFG_UIM_LOW_HIGH_VAR_SEL_E, infoMapFuncParamsP->low_high_var_sel);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_CFG_IM_CLOSE_EN_E, infoMapFuncParamsP->close_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_EPS_A_O_A_E, eps_a);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_EPS_A_O_O_E, eps_o);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_EPS_T_VAL_E, eps_t);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_T2_VAL_E, im_t2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_MAX_CFG_OFFSET_E, max_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_MAX_CFG_RATIO_E, max_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_MIN_CFG_OFFSET_E, min_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_VAR_MIN_CFG_RATIO_E, min_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_BLOB_CFG_ENABLE_E, infoMapFuncParamsP->blob_enable);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_BLOB_CFG_MAX_LIST_SIZE_E, infoMapFuncParamsP->max_list_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_BLOB_CFG_LIN_DIST_E, infoMapFuncParamsP->lin_dist);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_LEFT_P1_UIM_BLOB_CFG_MAX_SIZE_E, infoMapFuncParamsP->max_size);

      //RIGHT
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_AGG_RIGHT_P1_CFG_INFOMAP_INV_EN_E, infoMapFuncParamsP->infomap_inv_en);

      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK0_LSB_MSK_E, MASK_64_LSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK0_MSB_MSK_E, MASK_64_MSB(mask_0));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_0_Y2_E, nf_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_0_Y_E, nf2_0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK1_LSB_MSK_E, MASK_64_LSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK1_MSB_MSK_E, MASK_64_MSB(mask_22));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_1_Y2_E, nf_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_1_Y_E, nf2_22);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK2_LSB_MSK_E, MASK_64_LSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK2_MSB_MSK_E, MASK_64_MSB(mask_45));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_2_Y2_E, nf_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_2_Y_E, nf2_45);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK3_LSB_MSK_E, MASK_64_LSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK3_MSB_MSK_E, MASK_64_MSB(mask_67));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_3_Y2_E, nf_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_3_Y_E, nf2_67);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK4_LSB_MSK_E, MASK_64_LSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_EN_MSK4_MSB_MSK_E, MASK_64_MSB(mask_box));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_4_Y2_E, nf_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_NORM_4_Y_E, nf2_box);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_CFG_UIM_LOW_HIGH_VAR_SEL_E, infoMapFuncParamsP->low_high_var_sel);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_CFG_IM_CLOSE_EN_E, infoMapFuncParamsP->close_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_EPS_A_O_A_E, eps_a);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_EPS_A_O_O_E, eps_o);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_EPS_T_VAL_E, eps_t);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_T2_VAL_E, im_t2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_MAX_CFG_OFFSET_E, max_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_MAX_CFG_RATIO_E, max_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_MIN_CFG_OFFSET_E, min_vc);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_VAR_MIN_CFG_RATIO_E, min_ratio);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_BLOB_CFG_ENABLE_E, infoMapFuncParamsP->blob_enable);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_BLOB_CFG_MAX_LIST_SIZE_E, infoMapFuncParamsP->max_list_size);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_BLOB_CFG_LIN_DIST_E, infoMapFuncParamsP->lin_dist);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_UIM_RIGHT_P1_UIM_BLOB_CFG_MAX_SIZE_E, infoMapFuncParamsP->max_size);
#ifdef DEBUG_DPE_ALG
      const char* sides[] = { "left", "right" };
      int side = 0;
      for (side = 0; side < 2; side++)// LEFT/RIGHT
      {
          printf("dpe_agg_%s_p1_cfg\ninfomap_inv_en = %d\n", sides[side], infoMapFuncParamsP->infomap_inv_en);
          printf("dpe_uim_%s_p1_uim_var_en_msk0_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_0));
          printf("dpe_uim_%s_p1_uim_var_en_msk0_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_0));
          printf("dpe_uim_%s_p1_uim_var_norm_0\ny2 = %d\ny = %d\n", sides[side], nf_0, nf2_0);
          printf("dpe_uim_%s_p1_uim_var_en_msk1_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_22));
          printf("dpe_uim_%s_p1_uim_var_en_msk1_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_22));
          printf("dpe_uim_%s_p1_uim_var_norm_1\ny2 = %d\ny = %d\n", sides[side], nf_22, nf2_22);
          printf("dpe_uim_%s_p1_uim_var_en_msk2_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_45));
          printf("dpe_uim_%s_p1_uim_var_en_msk2_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_45));
          printf("dpe_uim_%s_p1_uim_var_norm_2\ny2 = %d\ny = %d\n", sides[side], nf_45, nf2_45);
          printf("dpe_uim_%s_p1_uim_var_en_msk3_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_67));
          printf("dpe_uim_%s_p1_uim_var_en_msk3_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_67));
          printf("dpe_uim_%s_p1_uim_var_norm_3\ny2 = %d\ny = %d\n", sides[side], nf_67, nf2_67);
          printf("dpe_uim_%s_p1_uim_var_en_msk4_lsb\nmsk = %u\n", sides[side], MASK_64_LSB(mask_box));
          printf("dpe_uim_%s_p1_uim_var_en_msk4_msb\nmsk = %u\n", sides[side], MASK_64_MSB(mask_box));
          printf("dpe_uim_%s_p1_uim_var_norm_4\ny2 = %d\ny = %d\n", sides[side], nf_box, nf2_box);
          printf("dpe_uim_%s_p1_uim_cfg\nuim_low_high_var_sel = %d\nim_close_en = %d\n",
              sides[side], infoMapFuncParamsP->low_high_var_sel, infoMapFuncParamsP->close_en);
          printf("dpe_uim_%s_p1_uim_eps_a_o\na = %d\no = %d\n", sides[side], eps_a, eps_o);
          printf("dpe_uim_%s_p1_uim_eps_t\nval = %d\n", sides[side], eps_t);
          printf("dpe_uim_%s_p1_uim_t2\nval = %d\n", sides[side], im_t2);
          printf("dpe_uim_%s_p1_uim_var_max_cfg\noffset = %d\nratio = %d\n", sides[side], max_vc, max_ratio);
          printf("dpe_uim_%s_p1_uim_var_min_cfg\noffset = %d\nratio = %d\n", sides[side], min_vc, min_ratio);
          printf("dpe_uim_%s_p1_uim_blob_cfg\nenable = %d\nmax_list_size = %d\nlin_dist = %d\nmax_size = %d\n",
              sides[side], infoMapFuncParamsP->blob_enable, infoMapFuncParamsP->max_list_size, infoMapFuncParamsP->lin_dist, infoMapFuncParamsP->max_size);
      }
#endif//DEBUG_DPE_ALG
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void subpixelSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_subpixelSetupParamsT *subpixelFuncParamsP = &(nu4k->dpeFunc.subpixelSetup[inst]);
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, subpixelFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CENSUS_CFG_PXL_COR_E, subpixelFuncParamsP->pxl_cor);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CENSUS_CFG_HMSK_E, subpixelFuncParamsP->hmsk);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CENSUS_CFG_VMSK_E, subpixelFuncParamsP->vmsk);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CENSUS_CFG_MSK_DISP_THR_E, subpixelFuncParamsP->census_cfg_msk_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_AGGR_CFG_MSK_DISP_THR_E, subpixelFuncParamsP->aggr_cfg_msk_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_AGGR_CFG_WEIGHT1_E, subpixelFuncParamsP->weight1);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_AGGR_CFG_WEIGHT2_E, subpixelFuncParamsP->weight2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_AGGR_CFG_WEIGHT3_E, subpixelFuncParamsP->weight3);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_AGGR_CFG_WEIGHT4_E, subpixelFuncParamsP->weight4);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CFG_AGGR_MSK_DISP_MODE_E, subpixelFuncParamsP->aggr_msk_disp_mode);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CFG_CENSUS_MSK_DISP_MODE_E, subpixelFuncParamsP->census_msk_disp_mode);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CFG_CENSUS_FLT_EN_E, subpixelFuncParamsP->census_flt_en);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_spx_census_cfg\n");
      printf("pxl_cor = %d\n", subpixelFuncParamsP->pxl_cor);
      printf("hmsk = %d\n", subpixelFuncParamsP->hmsk);
      printf("vmsk = %d\n", subpixelFuncParamsP->vmsk);
      printf("msk_disp_thr = %d\n", subpixelFuncParamsP->census_cfg_msk_disp_thr);
      printf("dpe_dpe_p0_spx_aggr_cfg\n");
      printf("msk_disp_thr = %d\n", subpixelFuncParamsP->aggr_cfg_msk_disp_thr);
      printf("weight1 = %d\n", subpixelFuncParamsP->weight1);
      printf("weight2 = %d\n", subpixelFuncParamsP->weight2);
      printf("weight3 = %d\n", subpixelFuncParamsP->weight3);
      printf("weight4 = %d\n", subpixelFuncParamsP->weight4);
      printf("dpe_dpe_p0_spx_cfg\n");
      printf("aggr_msk_disp_mode = %d\n", subpixelFuncParamsP->aggr_msk_disp_mode);
      printf("census_msk_disp_mode = %d\n", subpixelFuncParamsP->census_msk_disp_mode);
      printf("census_flt_en = %d\n", subpixelFuncParamsP->census_flt_en);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, subpixelFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CENSUS_CFG_PXL_COR_E, subpixelFuncParamsP->pxl_cor);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CENSUS_CFG_HMSK_E, subpixelFuncParamsP->hmsk);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CENSUS_CFG_VMSK_E, subpixelFuncParamsP->vmsk);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CENSUS_CFG_MSK_DISP_THR_E, subpixelFuncParamsP->census_cfg_msk_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_AGGR_CFG_MSK_DISP_THR_E, subpixelFuncParamsP->aggr_cfg_msk_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_AGGR_CFG_WEIGHT1_E, subpixelFuncParamsP->weight1);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_AGGR_CFG_WEIGHT2_E, subpixelFuncParamsP->weight2);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_AGGR_CFG_WEIGHT3_E, subpixelFuncParamsP->weight3);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_AGGR_CFG_WEIGHT4_E, subpixelFuncParamsP->weight4);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CFG_AGGR_MSK_DISP_MODE_E, subpixelFuncParamsP->aggr_msk_disp_mode);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CFG_CENSUS_MSK_DISP_MODE_E, subpixelFuncParamsP->census_msk_disp_mode);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CFG_CENSUS_FLT_EN_E, subpixelFuncParamsP->census_flt_en);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_spx_census_cfg\n");
      printf("pxl_cor = %d\n", subpixelFuncParamsP->pxl_cor);
      printf("hmsk = %d\n", subpixelFuncParamsP->hmsk);
      printf("vmsk = %d\n", subpixelFuncParamsP->vmsk);
      printf("msk_disp_thr = %d\n", subpixelFuncParamsP->census_cfg_msk_disp_thr);
      printf("dpe_dpe_p1_spx_aggr_cfg\n");
      printf("msk_disp_thr = %d\n", subpixelFuncParamsP->aggr_cfg_msk_disp_thr);
      printf("weight1 = %d\n", subpixelFuncParamsP->weight1);
      printf("weight2 = %d\n", subpixelFuncParamsP->weight2);
      printf("weight3 = %d\n", subpixelFuncParamsP->weight3);
      printf("weight4 = %d\n", subpixelFuncParamsP->weight4);
      printf("dpe_dpe_p1_spx_cfg\n");
      printf("aggr_msk_disp_mode = %d\n", subpixelFuncParamsP->aggr_msk_disp_mode);
      printf("census_msk_disp_mode = %d\n", subpixelFuncParamsP->census_msk_disp_mode);
      printf("census_flt_en = %d\n", subpixelFuncParamsP->census_flt_en);
#endif
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}


static void planeFitSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_planeFitSetupParamsT *plateFitFuncParamsP = &(nu4k->dpeFunc.planeFitSetup[inst]);
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, plateFitFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P0_PLANE_FIT_CFG_EN_E, plateFitFuncParamsP->plane_fit_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P0_PLANE_FIT_CFG_PIXEL_SMOOTH_MODE_E, plateFitFuncParamsP->pixel_smooth_mode);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P0_PLANE_FIT_GRAD_CFG_DISP_THR_E, plateFitFuncParamsP->grad_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P0_PLANE_FIT_GRAD_CFG_MIN_PIXELS_E, plateFitFuncParamsP->grad_min_pixels);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P0_PLANE_FIT_SMOOTH_CFG_DISP_THR_E, plateFitFuncParamsP->smooth_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P0_PLANE_FIT_SMOOTH_CFG_MIN_PIXELS_E, plateFitFuncParamsP->smooth_min_pixels);

#ifdef DEBUG_DPE_ALG
      printf("ppe_depth_post_p0_plane_fit_cfg\n");
      printf("en = %d\n", plateFitFuncParamsP->plane_fit_en);
      printf("pixel_smooth_mode = %d\n", plateFitFuncParamsP->pixel_smooth_mode);
      printf("ppe_depth_post_p0_plane_fit_grad_cfg\n");
      printf("disp_thr = %d\n", plateFitFuncParamsP->grad_disp_thr);
      printf("min_pixels = %d\n", plateFitFuncParamsP->grad_min_pixels);
      printf("ppe_depth_post_p0_plane_fit_smooth_cfg\n");
      printf("disp_thr = %d\n", plateFitFuncParamsP->smooth_disp_thr);
      printf("min_pixels = %d\n", plateFitFuncParamsP->smooth_min_pixels);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, plateFitFuncParamsP->p0_p1_mode))
   {
      //P1
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P1_PLANE_FIT_CFG_EN_E, plateFitFuncParamsP->plane_fit_en);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P1_PLANE_FIT_CFG_PIXEL_SMOOTH_MODE_E, plateFitFuncParamsP->pixel_smooth_mode);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P1_PLANE_FIT_GRAD_CFG_DISP_THR_E, plateFitFuncParamsP->grad_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P1_PLANE_FIT_GRAD_CFG_MIN_PIXELS_E, plateFitFuncParamsP->grad_min_pixels);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P1_PLANE_FIT_SMOOTH_CFG_DISP_THR_E, plateFitFuncParamsP->smooth_disp_thr);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_P1_PLANE_FIT_SMOOTH_CFG_MIN_PIXELS_E, plateFitFuncParamsP->smooth_min_pixels);

#ifdef DEBUG_DPE_ALG
      printf("ppe_depth_post_p1_plane_fit_cfg\n");
      printf("en = %d\n", plateFitFuncParamsP->plane_fit_en);
      printf("pixel_smooth_mode = %d\n", plateFitFuncParamsP->pixel_smooth_mode);
      printf("ppe_depth_post_p1_plane_fit_grad_cfg\n");
      printf("disp_thr = %d\n", plateFitFuncParamsP->grad_disp_thr);
      printf("min_pixels = %d\n", plateFitFuncParamsP->grad_min_pixels);
      printf("ppe_depth_post_p1_plane_fit_smooth_cfg\n");
      printf("disp_thr = %d\n", plateFitFuncParamsP->smooth_disp_thr);
      printf("min_pixels = %d\n", plateFitFuncParamsP->smooth_min_pixels);
#endif
   }

   if (plateFitFuncParamsP->plane_fit_en)
   {
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_PPE_DEPTH_POST_CFG_DEPTH_POST_EN_E, 1);
#ifdef DEBUG_DPE_ALG
      printf("ppe_depth_post_cfg\n");
      printf("depth_post_en = %d\n", 1);
#endif
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}
static void confidenceSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_confidenceSetupParamsT *confidenceFuncParamsP = &(nu4k->dpeFunc.confidenceSetup[inst]);
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   if (IS_PX_MODE_ENABLED(0, confidenceFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_C_CK_E, confidenceFuncParamsP->ck);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_C_CA_E, confidenceFuncParamsP->ca);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_C_CB_E, confidenceFuncParamsP->cb);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_R_RK_E, confidenceFuncParamsP->rk);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_R_RA_E, confidenceFuncParamsP->ra);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_R_RB_E, confidenceFuncParamsP->rb);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_WC0_WCR_WC0_E, confidenceFuncParamsP->wc0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_WR0_WCD_WR0_E, confidenceFuncParamsP->wr0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_CONF_WN_WN_E, confidenceFuncParamsP->wn);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_SCORE_W_C02_VAL_E, SIGNED_N_BIT_TO_U32(confidenceFuncParamsP->w_c02, 14));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_SCORE_W_C1_VAL_E, SIGNED_N_BIT_TO_U32(confidenceFuncParamsP->w_c1, 14));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_SCORE_W_UNITE_VAL_E, SIGNED_N_BIT_TO_U32(confidenceFuncParamsP->w_unite, 14));
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_conf_c\n");
      printf("ck = %d\n", confidenceFuncParamsP->ck);
      printf("ca = %d\n", confidenceFuncParamsP->ca);
      printf("cb = %d\n", confidenceFuncParamsP->cb);
      printf("dpe_dpe_p0_conf_r\n");
      printf("rk = %d\n", confidenceFuncParamsP->rk);
      printf("ra = %d\n", confidenceFuncParamsP->ra);
      printf("rb = %d\n", confidenceFuncParamsP->rb);
      printf("dpe_dpe_p0_conf_wc0_wcr\n");
      printf("wc0 = %d\n", confidenceFuncParamsP->wc0);
      printf("dpe_dpe_p0_conf_wr0_wcd\n");
      printf("wr0 = %d\n", confidenceFuncParamsP->wr0);
      printf("dpe_dpe_p0_conf_wn\n");
      printf("wn = %d\n", confidenceFuncParamsP->wn);
      printf("dpe_dpe_p0_spx_score_w_c02\n");
      printf("val = %d\n", confidenceFuncParamsP->w_c02);
      printf("dpe_dpe_p0_spx_score_w_c1\n");
      printf("val = %d\n", confidenceFuncParamsP->w_c1);
      printf("dpe_dpe_p0_spx_score_w_unite\n");
      printf("val = %d\n", confidenceFuncParamsP->w_unite);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, confidenceFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_C_CK_E, confidenceFuncParamsP->ck);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_C_CA_E, confidenceFuncParamsP->ca);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_C_CB_E, confidenceFuncParamsP->cb);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_R_RK_E, confidenceFuncParamsP->rk);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_R_RA_E, confidenceFuncParamsP->ra);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_R_RB_E, confidenceFuncParamsP->rb);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_WC0_WCR_WC0_E, confidenceFuncParamsP->wc0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_WR0_WCD_WR0_E, confidenceFuncParamsP->wr0);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_CONF_WN_WN_E, confidenceFuncParamsP->wn);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_SCORE_W_C02_VAL_E, SIGNED_N_BIT_TO_U32(confidenceFuncParamsP->w_c02, 14));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_SCORE_W_C1_VAL_E, SIGNED_N_BIT_TO_U32(confidenceFuncParamsP->w_c1, 14));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_SCORE_W_UNITE_VAL_E, SIGNED_N_BIT_TO_U32(confidenceFuncParamsP->w_unite, 14));
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_conf_c\n");
      printf("ck = %d\n", confidenceFuncParamsP->ck);
      printf("ca = %d\n", confidenceFuncParamsP->ca);
      printf("cb = %d\n", confidenceFuncParamsP->cb);
      printf("dpe_dpe_p1_conf_r\n");
      printf("rk = %d\n", confidenceFuncParamsP->rk);
      printf("ra = %d\n", confidenceFuncParamsP->ra);
      printf("rb = %d\n", confidenceFuncParamsP->rb);
      printf("dpe_dpe_p1_conf_wc0_wcr\n");
      printf("wc0 = %d\n", confidenceFuncParamsP->wc0);
      printf("dpe_dpe_p1_conf_wr0_wcd\n");
      printf("wr0 = %d\n", confidenceFuncParamsP->wr0);
      printf("dpe_dpe_p1_conf_wn\n");
      printf("wn = %d\n", confidenceFuncParamsP->wn);
      printf("dpe_dpe_p1_spx_score_w_c02\n");
      printf("val = %d\n", confidenceFuncParamsP->w_c02);
      printf("dpe_dpe_p1_spx_score_w_c1\n");
      printf("val = %d\n", confidenceFuncParamsP->w_c1);
      printf("dpe_dpe_p1_spx_score_w_unite\n");
      printf("val = %d\n", confidenceFuncParamsP->w_unite);
#endif
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void confidenceBinsSetupCfg(nu4kT *nu4k, int inst)
{
   XMLModesG_confidenceBinsSetupParamsT *confidenceBinsFuncParamsP = &(nu4k->dpeFunc.confidenceBinsSetup[inst]);
   UINT32 conf_threshold = 0;

#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   // the next two parameters have effect only in C0,
   // but can be written also to the B0 configuration and will do nothing
   // they are written to a single register with 5 bits
   // bit 0
   if (confidenceBinsFuncParamsP->spx_conf_thr_en)
   {
      conf_threshold = 1;
   }
   // bits 1..4
   conf_threshold += (confidenceBinsFuncParamsP->spx_conf_thr % 16) * 2;

   if (IS_PX_MODE_ENABLED(0, confidenceBinsFuncParamsP->p0_p1_mode))
   {
      //P0
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_1_2_BIN_1_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_1, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_1_2_BIN_2_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_2, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_3_4_BIN_3_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_3, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_3_4_BIN_4_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_4, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_5_6_BIN_5_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_5, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_5_6_BIN_6_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_6, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_7_8_BIN_7_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_7, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_7_8_BIN_8_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_8, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_9_10_BIN_9_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_9, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_9_10_BIN_10_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_10, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_11_12_BIN_11_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_11, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_11_12_BIN_12_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_12, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_13_14_BIN_13_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_13, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_13_14_BIN_14_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_14, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_BIN_THR_15_BIN_15_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_15, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_SPX_CONF_PENALTY_PENALTY_CONF_VAL3_E, conf_threshold);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p0_spx_conf_bin_thr_1_2\n");
      printf("bin_1 = %d\n", confidenceBinsFuncParamsP->bin_1);
      printf("bin_2 = %d\n", confidenceBinsFuncParamsP->bin_2);
      printf("dpe_dpe_p0_spx_conf_bin_thr_3_4\n");
      printf("bin_3 = %d\n", confidenceBinsFuncParamsP->bin_3);
      printf("bin_4 = %d\n", confidenceBinsFuncParamsP->bin_4);
      printf("dpe_dpe_p0_spx_conf_bin_thr_5_6\n");
      printf("bin_5 = %d\n", confidenceBinsFuncParamsP->bin_5);
      printf("bin_6 = %d\n", confidenceBinsFuncParamsP->bin_6);
      printf("dpe_dpe_p0_spx_conf_bin_thr_7_8\n");
      printf("bin_7 = %d\n", confidenceBinsFuncParamsP->bin_7);
      printf("bin_8 = %d\n", confidenceBinsFuncParamsP->bin_8);
      printf("dpe_dpe_p0_spx_conf_bin_thr_9_10\n");
      printf("bin_9 = %d\n", confidenceBinsFuncParamsP->bin_9);
      printf("bin_10 = %d\n", confidenceBinsFuncParamsP->bin_10);
      printf("dpe_dpe_p0_spx_conf_bin_thr_11_12\n");
      printf("bin_11 = %d\n", confidenceBinsFuncParamsP->bin_11);
      printf("bin_12 = %d\n", confidenceBinsFuncParamsP->bin_12);
      printf("dpe_dpe_p0_spx_conf_bin_thr_13_14\n");
      printf("bin_13 = %d\n", confidenceBinsFuncParamsP->bin_13);
      printf("bin_14 = %d\n", confidenceBinsFuncParamsP->bin_14);
      printf("dpe_dpe_p0_spx_conf_bin_thr_15\n");
      printf("bin_15 = %d\n", confidenceBinsFuncParamsP->bin_15);
      printf("dpe_dpe_p0_spx_conf_penalty\n");
      printf("penalty_conf_val3 = %d\n", conf_threshold);
#endif
   }

   if (IS_PX_MODE_ENABLED(1, confidenceBinsFuncParamsP->p0_p1_mode))
   {
      //P1
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_1_2_BIN_1_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_1, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_1_2_BIN_2_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_2, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_3_4_BIN_3_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_3, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_3_4_BIN_4_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_4, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_5_6_BIN_5_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_5, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_5_6_BIN_6_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_6, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_7_8_BIN_7_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_7, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_7_8_BIN_8_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_8, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_9_10_BIN_9_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_9, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_9_10_BIN_10_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_10, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_11_12_BIN_11_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_11, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_11_12_BIN_12_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_12, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_13_14_BIN_13_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_13, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_13_14_BIN_14_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_14, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_BIN_THR_15_BIN_15_E, SIGNED_N_BIT_TO_U32(confidenceBinsFuncParamsP->bin_15, 16));
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_SPX_CONF_PENALTY_PENALTY_CONF_VAL3_E, conf_threshold);
#ifdef DEBUG_DPE_ALG
      printf("dpe_dpe_p1_spx_conf_bin_thr_1_2\n");
      printf("bin_1 = %d\n", confidenceBinsFuncParamsP->bin_1);
      printf("bin_2 = %d\n", confidenceBinsFuncParamsP->bin_2);
      printf("dpe_dpe_p1_spx_conf_bin_thr_3_4\n");
      printf("bin_3 = %d\n", confidenceBinsFuncParamsP->bin_3);
      printf("bin_4 = %d\n", confidenceBinsFuncParamsP->bin_4);
      printf("dpe_dpe_p1_spx_conf_bin_thr_5_6\n");
      printf("bin_5 = %d\n", confidenceBinsFuncParamsP->bin_5);
      printf("bin_6 = %d\n", confidenceBinsFuncParamsP->bin_6);
      printf("dpe_dpe_p1_spx_conf_bin_thr_7_8\n");
      printf("bin_7 = %d\n", confidenceBinsFuncParamsP->bin_7);
      printf("bin_8 = %d\n", confidenceBinsFuncParamsP->bin_8);
      printf("dpe_dpe_p1_spx_conf_bin_thr_9_10\n");
      printf("bin_9 = %d\n", confidenceBinsFuncParamsP->bin_9);
      printf("bin_10 = %d\n", confidenceBinsFuncParamsP->bin_10);
      printf("dpe_dpe_p1_spx_conf_bin_thr_11_12\n");
      printf("bin_11 = %d\n", confidenceBinsFuncParamsP->bin_11);
      printf("bin_12 = %d\n", confidenceBinsFuncParamsP->bin_12);
      printf("dpe_dpe_p1_spx_conf_bin_thr_13_14\n");
      printf("bin_13 = %d\n", confidenceBinsFuncParamsP->bin_13);
      printf("bin_14 = %d\n", confidenceBinsFuncParamsP->bin_14);
      printf("dpe_dpe_p1_spx_conf_bin_thr_15\n");
      printf("bin_15 = %d\n", confidenceBinsFuncParamsP->bin_15);
      printf("dpe_dpe_p1_spx_conf_penalty\n");
      printf("penalty_conf_val3 = %d\n", conf_threshold);
#endif
   }
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void get_register_conf(nu4kT* nu4k,
                              UINT32 effective_input_width,
                              UINT32 input_width,
                              UINT32 input_height,
                              UINT32 p0_diff_start_crop,
                              UINT32 min_disp,
                              UINT32 p0_diff_scale_mode,
                              float p0_down_sample_factor,
                              UINT32 p0_internal_width,
                              float p1_down_sample_factor,
                              UINT32 p1_internal_width,
                              UINT32 p0_merge_target_start_out,
                              UINT32 p0_merge_target_stop_out,
                              UINT32 p0_merge_source_start_out)
{
    UINT32 dpe_hybrid_cfg0_en;
    UINT32 dpe_p0_mrg_disp_rescale_offset, dpe_p0_mrg_disp_rescale_factor, dpe_p0_mrg_disp_rescale_shift;
    UINT32 dpe_p1_mrg_disp_rescale_offset, dpe_p1_mrg_disp_rescale_factor, dpe_p1_mrg_disp_rescale_shift;
    UINT32 p0_hscale_cfg0_scale = round(65536 * p0_down_sample_factor);
    UINT32 p1_hscale_cfg0_scale = round(65536 * p1_down_sample_factor);
    UINT32 dpe_p0_mrg_scale = 0;
    UINT32 dpe_p1_mrg_scale = 0;
    UINT32 dpe_cfg_mrg_hyb_mrg_bypass;

    if (p0_down_sample_factor != 0)
    {
        dpe_p0_mrg_scale = round(65536 / p0_down_sample_factor);
    }

    if (p1_down_sample_factor != 0)
    {
        dpe_p1_mrg_scale = round(65536 / p1_down_sample_factor);
    }

   /***********************************************
    * diff
    ***********************************************/

   // p0 cropping
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CROP_ENABLE_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CROP_START_E, min_disp + p0_diff_start_crop);
   // supposed to be: min_disp+effecive_center + p0_output_width_half + RIGHT_EDGE
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CROP_STOP_E, min_disp + effective_input_width - 1);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CROP_ENABLE_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CROP_START_E, p0_diff_start_crop);
   // supposed  to be: min_disp+effecive_center + p0_output_width_half + RIGHT_EDGE
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CROP_STOP_E, effective_input_width - 1);

   // p1 cropping
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_CROP_ENABLE_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_CROP_START_E, min_disp);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_CROP_STOP_E, input_width - 1);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_CROP_ENABLE_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_CROP_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_CROP_STOP_E, effective_input_width - 1);

   // p0  scaling
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CFG_SCALE_MODE_E, p0_diff_scale_mode);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG0_GAIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG0_SCALE_E, p0_hscale_cfg0_scale);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG0_SCALE_E, (1 << 16));
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_HSIZE_E, p0_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG1_VSIZE_E, input_height - 1);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CFG_SCALE_MODE_E, p0_diff_scale_mode);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG0_GAIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG0_SCALE_E, p0_hscale_cfg0_scale);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG0_SCALE_E, (1 << 16));
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG1_HSIZE_E, p0_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG1_VSIZE_E, input_height - 1);

   // p1  scaling
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_CFG_SCALE_MODE_E, p0_diff_scale_mode);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG0_GAIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG0_SCALE_E, p1_hscale_cfg0_scale);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_VSCALE_CFG0_SCALE_E, (1 << 16));
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_HSCALE_CFG1_HSIZE_E, p1_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_VSCALE_CFG1_VSIZE_E, input_height - 1);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_CFG_SCALE_MODE_E, p0_diff_scale_mode);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG0_GAIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG0_SCALE_E, p1_hscale_cfg0_scale);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_VSCALE_CFG0_SCALE_E, (1 << 16));
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG1_SCALE_OFFSET_E,0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_HSCALE_CFG1_HSIZE_E, p1_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_VSCALE_CFG1_VSIZE_E, input_height - 1);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG1_PAD_VAL_E, 0);

   if (p1_down_sample_factor > 0)
   {
      dpe_hybrid_cfg0_en = 1;
   }
   else
   {
       dpe_hybrid_cfg0_en = 0;
   }
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_EN_E, dpe_hybrid_cfg0_en);

   // p0 last pixel in hybrid line
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_P0_SIZE_E, p0_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG1_AGG_P0_SIZE_E, p0_internal_width - 1);
   // p1 last pixel in hybrid line
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_P1_SIZE_E, p0_internal_width + p1_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG1_AGG_P1_SIZE_E, p0_internal_width + p1_internal_width - 1);

   // range of p0 in hybrid row
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HYB_STOP_E, p0_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HYB_STOP_E, p0_internal_width - 1);
   // range of p1 in hybrid row
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_HYB_START_E, p0_internal_width);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P1_HYB_STOP_E, p0_internal_width + p1_internal_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_HYB_START_E, p0_internal_width);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P1_HYB_STOP_E, p0_internal_width + p1_internal_width - 1);

   /***********************************************
    * merge
    ***********************************************/

   // restructure on p0
   dpe_p0_mrg_disp_rescale_offset = min_disp * SUBPIXEL_FACTOR;

   if (p0_down_sample_factor == 1)
   {
      dpe_p0_mrg_disp_rescale_factor = 1;
      dpe_p0_mrg_disp_rescale_shift = 0;
   }
   else
   {
      // calculate shift and factor for p0
      UINT32 shift0 = 0;
      UINT32 over_ds_factor0 = p0_down_sample_factor;
      UINT32 factor0 = 0;

      while (TRUE)
      {
         factor0 = over_ds_factor0 * (1 << shift0);
         if (factor0 > 255)
         {
            shift0 = shift0 - 1;
            factor0 = round(over_ds_factor0 * (1 << shift0));
            break;
         }
         shift0 = shift0 + 1;
      }
      dpe_p0_mrg_disp_rescale_factor = factor0;
      dpe_p0_mrg_disp_rescale_shift = shift0;

      if (min_disp > 0)
      {
          dpe_p0_mrg_disp_rescale_offset = (min_disp * SUBPIXEL_FACTOR) * (1 << shift0) / factor0;
      }

   }
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_OFFSET_E, dpe_p0_mrg_disp_rescale_offset);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_FACTOR_E, dpe_p0_mrg_disp_rescale_factor);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_SHIFT_E, dpe_p0_mrg_disp_rescale_shift);

   // restructure on p1
   dpe_p1_mrg_disp_rescale_offset = min_disp * SUBPIXEL_FACTOR;

   if (p1_down_sample_factor == 1)
   {
       dpe_p1_mrg_disp_rescale_factor = 1;
       dpe_p1_mrg_disp_rescale_shift = 0;
   }
   else
   {
      // calculate shift and factor for p1
      UINT32 shift1 = 0;
      UINT32 over_ds_factor1 = p1_down_sample_factor;
      UINT32 factor1 = 0;

      while (TRUE)
      {
         factor1 = over_ds_factor1 * (1 << shift1);
         if (factor1 > 255)
         {
            shift1 = shift1 - 1;
            factor1 = round(over_ds_factor1 * (1 << shift1));
            break;
         }
         shift1 = shift1 + 1;
      }
       dpe_p1_mrg_disp_rescale_factor = factor1;
       dpe_p1_mrg_disp_rescale_shift = shift1;

      if (min_disp > 0)
      {
         dpe_p1_mrg_disp_rescale_offset = (min_disp * SUBPIXEL_FACTOR) * (1 << shift1) / factor1;
      }

   }
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_MRG_DISP_RESCALE_OFFSET_E, dpe_p1_mrg_disp_rescale_offset);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_MRG_DISP_RESCALE_FACTOR_E, dpe_p1_mrg_disp_rescale_factor);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P1_MRG_DISP_RESCALE_SHIFT_E, dpe_p1_mrg_disp_rescale_shift);


   //  restructure (p0, p1): possible disparity range after restructure
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MAX_E, min_disp + ceil(143 * p0_down_sample_factor));

   if (p1_down_sample_factor > 0)
   {
      setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_OUT_CLIP_MIN_E, 0);
      setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_DISP_OUT_CLIP_MAX_E, min_disp + ceil(143 * p1_down_sample_factor));
   }

   // rescale (p0, p1): location of rescale output in full output
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_START_E, p0_merge_target_start_out);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_STOP_E, p0_merge_target_stop_out - 1);

   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_OUT_LOCATION_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_OUT_LOCATION_STOP_E, effective_input_width - 1);

   // rescale (p0, p1): start of two parts before rescaling (depend on translator)
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_INT_PART_START_E, p0_merge_source_start_out);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_INT_PART_START_E, p0_internal_width);

   // rescale (p0, p1): rescaling factor (depend on translator)
   if (p0_down_sample_factor > 0)
   {
      setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_SCALE_SCALE_E, dpe_p0_mrg_scale);
   }
   if (p1_down_sample_factor > 0)
   {
      setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P1_MRG_SCALE_SCALE_E, dpe_p1_mrg_scale);
   }

   // rescale (p0, p1): actual size of output of scaling. note if min disparity is bigger than 0
   // you will get 0 at the last min disparity pixels
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MERGE_SIZE_E, input_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_DISP_SCL_BYPASS_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_IMG_SCL_BYPASS_E, 0);

   if (p1_down_sample_factor > 0)
   {
      dpe_cfg_mrg_hyb_mrg_bypass = 0;
   }
   else
   {
      dpe_cfg_mrg_hyb_mrg_bypass = 1;
   }
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_HYB_MRG_BYPASS_E, dpe_cfg_mrg_hyb_mrg_bypass);

   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_MRG_CONF_CFG_CONF_MODE_E, 0);
#ifdef DEBUG_DPE_ALG
   printf("dpe_dif_left_p0_crop\nenable = 1\nstart = %d\nstop = %d\n", min_disp + p0_diff_start_crop, min_disp + effective_input_width - 1);
   printf("dpe_dif_right_p0_crop\nenable = 1\nstart = %d\nstop = %d\n", p0_diff_start_crop, effective_input_width - 1);

   printf("dpe_dif_left_p1_crop\nenable = 1\nstart = %d\nstop = %d\n", min_disp, input_width - 1);
   printf("dpe_dif_right_p1_crop\nenable = 1\nstart = %d\nstop = %d\n", 0, effective_input_width - 1);

   printf("dpe_dif_left_p0_cfg\nscale_mode = %d\n", p0_diff_scale_mode);
   printf("dpe_dif_left_p0_vscale_cfg0\nscale = %d\n", 65536);
   printf("dpe_dif_left_p0_vscale_cfg1\nvsize = %d\n", input_height - 1);
   printf("dpe_dif_left_p0_hscale_cfg0\ngain = %d\nscale = %d\n", 0, p0_hscale_cfg0_scale);
   printf("dpe_dif_left_p0_hscale_cfg1\nscale_offset = %d\nhsize = %d\n", 0, p0_internal_width - 1);

   printf("dpe_dif_right_p0_cfg\nscale_mode = %d\n", p0_diff_scale_mode);
   printf("dpe_dif_right_p0_vscale_cfg0\nscale = %d\n", 65536);
   printf("dpe_dif_right_p0_vscale_cfg1\nvsize = %d\n", input_height - 1);
   printf("dpe_dif_right_p0_hscale_cfg0\ngain = %d\nscale = %d\n", 0, p0_hscale_cfg0_scale);
   printf("dpe_dif_right_p0_hscale_cfg1\nscale_offset = %d\nhsize = %d\n", 0, p0_internal_width - 1);

   printf("dpe_dif_left_p1_cfg\nscale_mode = %d\n", p0_diff_scale_mode);
   printf("dpe_dif_left_p1_vscale_cfg0\nscale = %d\n", 65536);
   printf("dpe_dif_left_p1_vscale_cfg1\nvsize = %d\n", input_height - 1);
   printf("dpe_dif_left_p1_hscale_cfg0\ngain = %d\nscale = %d\n", 0, p1_hscale_cfg0_scale);
   printf("dpe_dif_left_p1_hscale_cfg1\nscale_offset = %d\nhsize = %d\n", 0, p1_internal_width - 1);

   printf("dpe_dif_right_p1_cfg\nscale_mode = %d\n", p0_diff_scale_mode);
   printf("dpe_dif_right_p1_vscale_cfg0\nscale = %d\n", 65536);
   printf("dpe_dif_right_p1_vscale_cfg1\nvsize = %d\n", input_height - 1);
   printf("dpe_dif_right_p1_hscale_cfg0\ngain = %d\nscale = %d\n", 0, p1_hscale_cfg0_scale);
   printf("dpe_dif_right_p1_hscale_cfg1\nscale_offset = %d\nhsize = %d\n", 0, p1_internal_width - 1);

   printf("dpe_hybrid_cfg0\nen = %d\np0_size = %d\np1_size = %d\n", dpe_hybrid_cfg0_en, p0_internal_width - 1, p0_internal_width + p1_internal_width - 1);
   printf("dpe_hybrid_cfg1\npad_val = %d\nagg_p0_size = %d\nagg_p1_size = %d\n", 0, p0_internal_width - 1, p0_internal_width + p1_internal_width - 1);

   printf("dpe_dif_left_p0_hyb\nstart = %d\nstop = %d\n", 0, p0_internal_width - 1);
   printf("dpe_dif_right_p0_hyb\nstart = %d\nstop = %d\n", 0, p0_internal_width - 1);

   printf("dpe_dif_left_p1_hyb\nstart = %d\nstop = %d\n", p0_internal_width, p0_internal_width + p1_internal_width - 1);
   printf("dpe_dif_right_p1_hyb\nstart = %d\nstop = %d\n", p0_internal_width, p0_internal_width + p1_internal_width - 1);

   printf("dpe_dpe_p0_mrg_disp_rescale\noffset = %d\nfactor = %d\nshift = %d\n", dpe_p0_mrg_disp_rescale_offset, dpe_p0_mrg_disp_rescale_factor, dpe_p0_mrg_disp_rescale_shift);
   printf("dpe_dpe_p1_mrg_disp_rescale\noffset = %d\nfactor = %d\nshift = %d\n", dpe_p1_mrg_disp_rescale_offset, dpe_p1_mrg_disp_rescale_factor, dpe_p1_mrg_disp_rescale_shift);

   printf("dpe_dpe_p0_mrg_disp_out_clip\nmin = %d\nmax = %d\n", 0, min_disp + (UINT32)ceil(143 * p0_down_sample_factor));
   if (p1_down_sample_factor > 0)
   {
       printf("dpe_dpe_p1_mrg_disp_out_clip\nmin = %d\nmax = %d\n", 0, min_disp + (UINT32)ceil(143 * p1_down_sample_factor));
   }

   printf("dpe_dpe_p0_mrg_out_location\nstart = %d\nstop = %d\n", p0_merge_target_start_out, p0_merge_target_stop_out - 1);
   printf("dpe_dpe_p1_mrg_out_location\nstart = %d\nstop = %d\n", 0, effective_input_width - 1);

   printf("dpe_dpe_p0_mrg_int\npart_start = %d\n", p0_merge_source_start_out);
   printf("dpe_dpe_p1_mrg_int\npart_start = %d\n", p0_internal_width);

   printf("dpe_dpe_p0_mrg_scale\nscale = %d\n", dpe_p0_mrg_scale);
   printf("dpe_dpe_p1_mrg_scale\nscale = %d\n", dpe_p1_mrg_scale);

   printf("dpe_cfg\nmerge_size = %d\nmrg_disp_scl_bypass = %d\nmrg_img_scl_bypass = %d\nmrg_hyb_mrg_bypass = %d\n", input_width - 1, 0, dpe_cfg_mrg_hyb_mrg_bypass, 0);

   printf("dpe_mrg_conf_cfg\nconf_mode = %d\n", 0);
#endif
}

#define HYBRID_FULL_MODE_P0_SELECT      0
#define HYBRID_FULL_MODE_P1_SELECT      1
#define HYBRID_FULL_MODE_EXTENDED_RANGE 2
#define HYBRID_FULL_MODE_HOLE_FILLING   3
#define HYBRID_FULL_MODE_CONF_SELECT    4

static void multiResFullImageSetupCfg(nu4kT *nu4k, int inst, unsigned int input_width, unsigned int input_height)
{
   XMLModesG_multiResFullImageSetupParamsT *multiResFullImageFuncParamsP = &(nu4k->dpeFunc.multiResFullImageSetup[inst]);
   UINT32 min_disp = multiResFullImageFuncParamsP->min_disp;
   float p0_down_sample_factor = multiResFullImageFuncParamsP->p0_down_sample_factor;
   float p1_down_sample_factor = multiResFullImageFuncParamsP->p1_down_sample_factor;
   UINT32 effective_input_width = input_width - min_disp;
   UINT32 p0_internal_width = 0;
   UINT32 p1_internal_width = 0;
   // diff crop start in input row
   UINT32 p0_diff_start_crop = 0;
   // p0_diff_scale_mode should have been set to NU4K_DIF_SCL_MODE_HORZ_ONLY,
   // but due to NB-2197, NB-2227 we need to set it to NU4K_DIF_SCL_MODE_V_THEN_H
   // in order to bypass the bug.
   UINT32 p0_diff_scale_mode = NU4K_DIF_SCL_MODE_V_THEN_H;
   // merge source after rescale
   UINT32 p0_merge_source_start_out = 0;
   // target in merge row
   UINT32 p0_merge_target_start_out = 0;
   UINT32 p0_merge_target_stop_out = effective_input_width;

   if (p1_down_sample_factor > 0)
   {
      p1_internal_width = (UINT32)ceil((double)effective_input_width / ((double)p1_down_sample_factor));
   }
   else
   {
       p1_internal_width = 0;
   }

   if (p0_down_sample_factor > 0)
   {
      p0_internal_width = (UINT32)ceil((double)effective_input_width / ((double)p0_down_sample_factor));
   }

   if (p0_internal_width + p1_internal_width > MAX_INTERNAL_ROW)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "full mode can't be support\n");
      LOGG_PRINT(LOG_ERROR_E, NULL, "require internal size: p0+P1=%d+%d=%d\n", p0_internal_width, p1_internal_width, p0_internal_width + p1_internal_width);
      return;
   }

#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   get_register_conf(nu4k,
                     effective_input_width,
                     input_width,
                     input_height,
                     p0_diff_start_crop,
                     min_disp,
                     p0_diff_scale_mode,
                     p0_down_sample_factor,
                     p0_internal_width,
                     p1_down_sample_factor,
                     p1_internal_width,
                     p0_merge_target_start_out,
                     p0_merge_target_stop_out,
                     p0_merge_source_start_out);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_CFG_MODE_E, multiResFullImageFuncParamsP->hybrid_full_mode);
   if (multiResFullImageFuncParamsP->hybrid_full_mode == HYBRID_FULL_MODE_EXTENDED_RANGE)
   {
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_RNG_CONF_THR_F_NV_E, multiResFullImageFuncParamsP->ext_rng_thr_far_nv);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_RNG_CONF_THR_F_C_E, multiResFullImageFuncParamsP->ext_rng_thr_far_close);

   }
   else if (multiResFullImageFuncParamsP->hybrid_full_mode == HYBRID_FULL_MODE_CONF_SELECT)
   {
       setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_CONF_THRS_CONF_THR_D0_D1_E, 0);
       setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_CONF_THRS_CONF_THR_D0_INV_E, 0);
       setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_CONF_THRS_CONF_THR_INV_D1_E, 0);
   }

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_LIMITS_D0_CONF_THR_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_LIMITS_D0_MAX_E, (UINT32)(143 * p0_down_sample_factor));
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_LIMITS_D0_MIN_E, 0);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_LIMITS_D1_CONF_THR_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_LIMITS_D1_MAX_E, (UINT32)(143 * p1_down_sample_factor));
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_MRG_HDR_LIMITS_D1_MIN_E, 0);


#ifdef DEBUG_DPE_ALG
   printf("dpe_mrg_cfg\nmode = %d\n", multiResFullImageFuncParamsP->hybrid_full_mode);
   if (multiResFullImageFuncParamsP->hybrid_full_mode == HYBRID_FULL_MODE_EXTENDED_RANGE)
   {
       printf("dpe_mrg_rng_conf_thr\nf_nv = %d\nf_c = %d\n", multiResFullImageFuncParamsP->ext_rng_thr_far_nv, multiResFullImageFuncParamsP->ext_rng_thr_far_close);
   }
   else if (multiResFullImageFuncParamsP->hybrid_full_mode == HYBRID_FULL_MODE_CONF_SELECT)
   {
       printf("dpe_mrg_hdr_conf_thrs\nconf_thr_d0_d1 = %d\nconf_thr_d0_inv = %d\nconf_thr_inv_d1 = %d\n", 0, 0, 0);
   }

   printf("dpe_mrg_hdr_limits_d0\nconf_thr = %d\nmax = %d\nmin = %d\n", 0, (UINT32)(143 * p0_down_sample_factor), 0);
   printf("dpe_mrg_hdr_limits_d1\nconf_thr = %d\nmax = %d\nmin = %d\n", 0, (UINT32)(143 * p1_down_sample_factor), 0);

   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void minDisparitySetupCfg(nu4kT *nu4k, int inst, unsigned int input_width)
{
   XMLModesG_minDisparitySetupParamsT *minDisparityFuncParamsP = &(nu4k->dpeFunc.minDisparitySetup[inst]);
   UINT32 min_disp = minDisparityFuncParamsP->min_disp;
   UINT32 effective_input_width = input_width - min_disp;
#ifdef DEBUG_DPE_ALG
   PRINT_SEPARATOR_DPE_ALG
#endif

   /***********************************************
    * diff
    ***********************************************/

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CROP_ENABLE_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CROP_START_E, min_disp);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_CROP_STOP_E, input_width - 1);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CROP_ENABLE_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CROP_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_CROP_STOP_E, effective_input_width - 1);

   // p0 last pixel in hybrid line
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG0_P0_SIZE_E, input_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_HYBRID_CFG1_AGG_P0_SIZE_E, effective_input_width - 1);

   // range of p0 in hybrid row
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HYB_STOP_E, effective_input_width - 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HYB_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HYB_STOP_E, effective_input_width - 1);

   /***********************************************
    * merge
    ***********************************************/
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_DISP_SCL_BYPASS_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_IMG_SCL_BYPASS_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_CFG_MRG_HYB_MRG_BYPASS_E, 1);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_SCALE_SCALE_E, 0xFFFF);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_START_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_OUT_LOCATION_STOP_E, effective_input_width - 1);

   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_OFFSET_E, min_disp * SUBPIXEL_FACTOR);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_FACTOR_E, 1);
   setDbField(nu4k, NUFLD_DPE_E,0, NU4100_DPE_DPE_P0_MRG_DISP_RESCALE_SHIFT_E, 0);

   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MIN_E, 0);
   setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DPE_P0_MRG_DISP_OUT_CLIP_MAX_E, 0xFFFF);
#ifdef DEBUG_DPE_ALG
   printf("dpe_dif_left_p0_crop\nenable = 1\nstart = %d\nstop = %d\n", min_disp, input_width - 1);
   printf("dpe_dif_right_p0_crop\nenable = 1\nstart = %d\nstop = %d\n", 0, effective_input_width - 1);

   printf("dpe_hybrid_cfg0\np0_size = %d\n", effective_input_width - 1);
   printf("dpe_hybrid_cfg1\nagg_p0_size = %d\n",effective_input_width - 1);

   printf("dpe_dif_left_p0_hyb\nstart = %d\nstop = %d\n", 0, effective_input_width - 1);
   printf("dpe_dif_right_p0_hyb\nstart = %d\nstop = %d\n", 0, effective_input_width - 1);

   printf("dpe_cfg\nmrg_disp_scl_bypass = %d\nmrg_img_scl_bypass = %d\nmrg_hyb_mrg_bypass = %d\n", 0, 1, 1);

   printf("dpe_dpe_p0_mrg_disp_rescale\noffset = %d\nfactor = %d\nshift = %d\n", min_disp * SUBPIXEL_FACTOR, 1, 0);

   printf("dpe_dpe_p0_mrg_disp_out_clip\nmin = %d\nmax = %d\n", min_disp, min_disp + 143);

   PRINT_SEPARATOR_DPE_ALG
#endif
}

static void updateDpeOut(nu4kT *nu4k, nu4kblkT *blk, unsigned int index)
{
   (void)index;

   UINT32 i;
   unsigned int iau_left=0, iau_right=1;
   unsigned int left_horz = 0, left_vert = 0;
   unsigned int right_horz = 0, right_vert = 0, hybrid_cfg0;

   if((!blk->inputs[0].blkp) || (!blk->inputs[1].blkp))
      return;

   //get input resolution from left IAU (input 0/1 are L/R, respectively)
   getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &left_horz, &left_vert);
   getOutputRes(nu4k, blk->inputs[1].blkp, blk->inputs[1].sel, &right_horz, &right_vert);

   if(blk->inputs[0].blkp->ti.type== NUCFG_BLK_IAU_E)
   {
      iau_left=0;
      iau_right=1;
   }
   else  if(blk->inputs[0].blkp->ti.type== NUCFG_BLK_IAU_COLOR_E)
   {
      iau_left=2;
      iau_right=3;
   }

   getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_DPE_HYBRID_CFG0_EN_E, &hybrid_cfg0);
   if ((hybrid_cfg0 == NU4K_DPE_HYBRID_DISABLE) || (hybrid_cfg0 > NU4K_DPE_HYBRID_ENABLE)) //in case that value = 0 or empty
   {
      if(left_horz) left_horz--;
      if(left_vert) left_vert--;
      if(right_horz) right_horz--;
      if(right_vert) right_vert--;

      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_HYBRID_CFG0_P0_SIZE_E, left_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_HYBRID_CFG0_P1_SIZE_E, left_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_HYBRID_CFG1_AGG_P0_SIZE_E, left_horz);

      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_LEFT_P0_CFG_IN_VSIZE_E, left_vert);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_LEFT_P1_CFG_IN_VSIZE_E, left_vert);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_RIGHT_P0_CFG_IN_VSIZE_E, right_vert);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_RIGHT_P1_CFG_IN_VSIZE_E, right_vert);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_LEFT_P0_CROP_STOP_E, left_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_LEFT_P1_CROP_STOP_E, left_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_RIGHT_P0_CROP_STOP_E, right_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_RIGHT_P1_CROP_STOP_E, right_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_LEFT_P0_HYB_STOP_E, left_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_LEFT_P1_HYB_STOP_E, left_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_RIGHT_P0_HYB_STOP_E, right_horz);
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_DIF_RIGHT_P1_HYB_STOP_E, right_horz);

      float xscale, yscale;
      UINT32 hsizeScl, vsizeScl;

      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_HSIZE_E, &hsizeScl);
      getDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG1_VSIZE_E, &vsizeScl);

      xscale = (((float)(left_horz + 1))) / (float)(hsizeScl + 1);
      xscale = xscale * 65536; // like ((float) << 16), but xscale is float and there is no option to shift float type. we need the floating point accuracy
      yscale = (((float)(left_vert + 1))) / (float)(vsizeScl + 1);
      yscale = yscale * 65536; // like ((float) << 16), but xscale is float and there is no option to shift float type. we need the floating point accuracy

      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG0_SCALE_E, (UINT32)yscale);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG0_SCALE_E, (UINT32)xscale);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG0_SCALE_E, (UINT32)yscale);
      setDbField(nu4k, NUFLD_DPE_E, 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG0_SCALE_E, (UINT32)xscale);

      //Disparity offset modification - b.   The "merge_size" value should be always set in all DPE modes (Regular/Minimum Disparity/Hybrid)
      setDbField(nu4k, NUFLD_DPE_E, blk->ti.inst, NU4100_DPE_CFG_MERGE_SIZE_E, left_horz);
   }
   else
   {
      if (nu4k->dpeFunc.hybridFuncParams.enable==1)
         hybridCfg(nu4k,left_horz, left_vert, iau_right, iau_left);
   }

   if (nu4k->dpeFunc.minDispFuncParams.enable==1)
      minDipsarityCfg(nu4k, left_horz);

   for (i = 1; i < nu4k->dpeFunc.numOfAggFunc;i++)//[0] is default. don't care for func invocation
   {
      if (nu4k->dpeFunc.aggregationSetup[i].enable == 1)
         aggregationSetupCfg(nu4k,i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfOptimizationFunc; i++)
   {
      if (nu4k->dpeFunc.optimizationSetup[i].enable == 1)
         optimizationSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfUniteFunc; i++)
   {
      if (nu4k->dpeFunc.uniteSetup[i].enable == 1)
         uniteSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfSmallFilterFunc; i++)
   {
      if (nu4k->dpeFunc.smallFilterSetup[i].enable == 1)
         smallFilterSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfBlobFunc; i++)
   {
      if (nu4k->dpeFunc.blobSetup[i].enable == 1)
         blobSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfEdgeDetectFunc; i++)
   {
      if (nu4k->dpeFunc.edgeDetectSetup[i].enable == 1)
         edgeDetectSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfInfoMapFunc; i++)
   {
      if (nu4k->dpeFunc.infoMapSetup[i].enable == 1)
         infoMapSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfSubpixelFunc; i++)
   {
      if (nu4k->dpeFunc.subpixelSetup[i].enable == 1)
         subpixelSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfPlaneFitFunc; i++)
   {
      if (nu4k->dpeFunc.planeFitSetup[i].enable == 1)
         planeFitSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfConfidenceFunc; i++)
   {
      if (nu4k->dpeFunc.confidenceSetup[i].enable == 1)
         confidenceSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfConfidenceBinsFunc; i++)
   {
      if (nu4k->dpeFunc.confidenceBinsSetup[i].enable == 1)
         confidenceBinsSetupCfg(nu4k, i);
   }

   for (i = 1; i < nu4k->dpeFunc.numOfMultiResFullImageFunc; i++)
   {
      if (nu4k->dpeFunc.multiResFullImageSetup[i].enable == 1)
      {
         unsigned int input_width = left_horz + 1;
         unsigned int input_height = left_vert + 1;
         if (hybrid_cfg0 == NU4K_DPE_HYBRID_ENABLE)
         {
            input_width = left_horz;
            input_height = left_vert;
         }
         multiResFullImageSetupCfg(nu4k, i, input_width, input_height);
      }
   }

   for (i = 1; i < nu4k->dpeFunc.numOfminDisparityFunc; i++)
   {
      if (nu4k->dpeFunc.minDisparitySetup[i].enable == 1)
      {
         unsigned int input_width = left_horz + 1;
         if (hybrid_cfg0 == NU4K_DPE_HYBRID_ENABLE)
         {
            input_width = left_horz;
         }
         minDisparitySetupCfg(nu4k, i, input_width);
      }
   }
}



static void updateDppScl(nu4kT *nu4k, nu4kblkT *blk, XMLDB_pathE sclModeE, XMLDB_pathE scl_horz_sizeE, XMLDB_pathE scl_vert_sizeE, XMLDB_pathE scl_xoffE, XMLDB_pathE scl_yoffE, XMLDB_pathE scl_xsclE, XMLDB_pathE scl_ysclE, UINT32 *in_horz, UINT32 *in_vert)
{
   UINT32 hsizeScl, vsizeScl, sclmode;
   float xscale, yscale;

   getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, sclModeE, &sclmode);

   if ((((int)sclmode) < NU4K_DPP_SCL_MODE_BYPASS) || (sclmode > NU4K_DPP_SCL_MODE_HORZ_VERT))//in case 'sclmode' value is empty on XML
      sclmode = NU4K_DPP_SCL_MODE_BYPASS;

   if (sclmode != NU4K_DPP_SCL_MODE_BYPASS)
   {
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_horz_sizeE, &hsizeScl);
      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_vert_sizeE, &vsizeScl);

      //update xscale&yscale
      //xscale
      xscale = (((float)*in_horz) + 1) / (float)(hsizeScl + 1);
      xscale = xscale * 65536; // like ((float) << 16), but xscale is float and there is no option to shift float type. we need the floating point accuracy
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_xsclE, (UINT32)xscale);
      //yscale
      yscale = (((float)*in_vert) + 1) / (float)(vsizeScl + 1);
      yscale = yscale * 65536; // like ((float) << 16), but xscale is float and there is no option to shift float type. we need the floating point accuracy
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_ysclE, (UINT32)yscale);

      *in_horz = hsizeScl + 1;
      *in_vert = vsizeScl + 1;
   }
   else
   {
      //0-scaling : use full input res - scaler must be configured even in bypass mode in A0.
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, sclModeE, NU4K_DPP_SCL_MODE_BYPASS);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_horz_sizeE, *in_horz);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_vert_sizeE, *in_vert);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_xoffE, 0);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_yoffE, 0);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_xsclE, 0x10000);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, scl_ysclE, 0x10000);
   }
}

/*
* update the dpp cropping block, based on the input resolution
*/
static void updateDppCrop(nu4kT *nu4k, nu4kblkT *blk, XMLDB_pathE crop_xs, XMLDB_pathE crop_xe, XMLDB_pathE crop_ys, XMLDB_pathE crop_ye, UINT32 *in_horz, UINT32 *in_vert)
{
   UINT32 crop_x_st = 0, crop_x_ed, crop_y_st = 0, crop_y_ed = 0;
   UINT32 _in_horz = 0, _in_vert = 0;
   NUCFG_resT *userCrop = NULL;

   //get input resolution to dpp
   getOutputRes(nu4k, blk->inputs->blkp, blk->inputs->sel, &_in_horz, &_in_vert);

   userCrop = &(nu4k->dpp[blk->ti.inst].userCrop);

   if ((userCrop != NULL) && (userCrop->width != 0) && (userCrop->height != 0))
   {
      crop_x_st = userCrop->x;
      crop_x_ed = userCrop->x + userCrop->width - 1;
      crop_y_st = userCrop->y;
      crop_y_ed = userCrop->y + userCrop->height - 1;
      if ((_in_vert < crop_y_ed) || (_in_horz < crop_x_ed) ||
         (_in_vert < crop_y_st) || (_in_horz < crop_x_st))
      {
         LOGG_PRINT(LOG_ERROR_E, NULL, "Requested crop size is out of boundries\n");
      }
      else
      {
         LOGG_PRINT(LOG_DEBUG_E, NULL, "User crop applied\n");
      }
   }
   else
   {
      updateZeroCropping(&crop_x_st, &crop_x_ed, _in_horz);
      updateZeroCropping(&crop_y_st, &crop_y_ed, _in_vert);

   }

   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, crop_xs, crop_x_st);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, crop_xe, crop_x_ed);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, crop_ys, crop_y_st);
   setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, crop_ye, crop_y_ed);
   *in_horz = crop_x_ed - crop_x_st;
   *in_vert = crop_y_ed - crop_y_st;
}

static void updateDppOut(nu4kT *nu4k, nu4kblkT *blk, unsigned int index,nusoc_updateDispT *update)
{
   (void)index;
   unsigned int horz = 0, vert = 0;
   UINT32       depthScaleFactor, hybSize, d2dFactor32bit;
   XMLDB_pathE  scl_mode, scl_horz_size, scl_vert_size, scl_xoff, scl_yoff, scl_xscl, scl_yscl;
   XMLDB_pathE  crop_xs, crop_xe, crop_ys, crop_ye;

   if(!blk->inputs[0].blkp)
      return;

   //get input resolution dpp_0 is host.
   getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
   if(horz) horz--;
   if(vert) vert--;

   //dpp instance 0/1 is the host/cva respectively
   if(blk->ti.inst == 0)
   {
      SET_DPP_CROP_OFFSETS(HOST,crop_xs, crop_xe, crop_ys, crop_ye);
      SET_DPP_SCL_OFFSETS(HOST,scl_mode,scl_horz_size,scl_vert_size,scl_xoff, scl_yoff,scl_xscl,scl_yscl);
      depthScaleFactor = nu4k->dpeFunc.depthScaleFactor.depthScaleFactorHost;
   }
   else
   {
      SET_DPP_CROP_OFFSETS(CVA,crop_xs, crop_xe, crop_ys, crop_ye);
      SET_DPP_SCL_OFFSETS(CVA,scl_mode,scl_horz_size,scl_vert_size,scl_xoff, scl_yoff,scl_xscl,scl_yscl);
      depthScaleFactor = nu4k->dpeFunc.depthScaleFactor.depthScaleFactorCva;
   }

   //Set the vertical size input (both instances host/cva are the same)
   setDbField(nu4k, NUFLD_DPP_E, 0, NU4100_PPE_DEPTH_POST_CFG_V_SIZE_E, vert);
   if (update != NULL)
   {
      d2dFactor32bit = (UINT32)(update->d2dFactor * update->fc * 64 * depthScaleFactor);
      setDbField(nu4k, NUFLD_DPP_E, 0, NU4100_PPE_DEPTH_POST_P0_D2D_CFG1_D2D_FACTOR_E, d2dFactor32bit);

      getDbField(nu4k, NUFLD_DPP_E, 0, NU4100_PPE_DEPTH_POST_CFG_HYB_SIZE_E, &hybSize);
      if(hybSize > 0)//hyb_size>0 it means hybrid enable
         setDbField(nu4k, NUFLD_DPP_E, 0, NU4100_PPE_DEPTH_POST_P1_D2D_CFG1_D2D_FACTOR_E, d2dFactor32bit);
   }


   updateDppCrop(nu4k, blk, crop_xs, crop_xe, crop_ys, crop_ye, &horz, &vert);
   updateDppScl(nu4k, blk, scl_mode, scl_horz_size, scl_vert_size, scl_xoff, scl_yoff, scl_xscl, scl_yscl, &horz, &vert);

}

static void parseMipiMuxToMipiPhy(unsigned int mipiInst, unsigned int muxSel, unsigned int blk0_index, int *mipiPhy)
{
    *mipiPhy = -1;

    if (!blk0_index)//lanes01
    {
        switch (mipiInst)
        {
        case(0):
            *mipiPhy = 0;
            break;
        case(1):
            if (muxSel == MIPI_MUX1_LANES01_SEL_PHY1_E)
                *mipiPhy = 1;
            else if (muxSel == MIPI_MUX1_LANES01_SEL_PHY5_E)
                *mipiPhy = 5;
            break;
        case(2):
            *mipiPhy = 2;
            break;
        case(3):
            if (muxSel == MIPI_MUX3_LANES01_SEL_PHY1_E)
                *mipiPhy = 1;
            else if (muxSel == MIPI_MUX3_LANES01_SEL_PHY2_E)
                *mipiPhy = 2;
            else if (muxSel == MIPI_MUX3_LANES01_SEL_PHY3_E)
                *mipiPhy = 3;
            break;
        case(4):
            if (muxSel == MIPI_MUX4_LANES01_SEL_PHY1_E)
                *mipiPhy = 1;
            else if (muxSel == MIPI_MUX4_LANES01_SEL_PHY4_E)
                *mipiPhy = 4;
            else if (muxSel == MIPI_MUX4_LANES01_SEL_PHY5_E)
                *mipiPhy = 5;
            break;
        case(5):
            if (muxSel == MIPI_MUX5_LANES01_SEL_PHY0_E)
                *mipiPhy = 0;
            else if (muxSel == MIPI_MUX5_LANES01_SEL_PHY5_E)
                *mipiPhy = 5;
            break;
        }
    }
    else//lanes23
    {
        switch (mipiInst)
        {
        case(0):
            if (muxSel == MIPI_MUX0_SEL_BLOCKED_E)
                *mipiPhy = -2;//mark as blocked
            else if (muxSel == MIPI_MUX0_SEL_PHY5_E)
                *mipiPhy = 5;
            break;
        case(1):
            if (muxSel == MIPI_MUX1_LANES23_SEL_BLOCKED_E)
                *mipiPhy = -2;//mark as blocked
            else if (muxSel == MIPI_MUX1_LANES23_SEL_PHY4_E)
                *mipiPhy = 4;
            break;
        case(2):
            if (muxSel == MIPI_MUX2_SEL_BLOCKED_E)
                *mipiPhy = -2;//mark as blocked
            else if (muxSel == MIPI_MUX2_SEL_PHY3_E)
                *mipiPhy = 3;
            break;
        case(3):
            if (muxSel == MIPI_MUX3_LANES23_SEL_BLOCKED_E)
                *mipiPhy = -2;//mark as blocked
            else if (muxSel == MIPI_MUX3_LANES23_SEL_PHY3_E)
                *mipiPhy = 3;
            else if (muxSel == MIPI_MUX3_LANES23_SEL_PHY4_E)
                *mipiPhy = 4;
            break;
        case(4):
            if (muxSel == MIPI_MUX4_LANES23_SEL_BLOCKED_E)
                *mipiPhy = -2;//mark as blocked
            else if (muxSel == MIPI_MUX4_LANES23_SEL_PHY4_E)
                *mipiPhy = 4;
            break;
        case(5):
            if (muxSel == MIPI_MUX5_LANES23_SEL_BLOCKED_E)
                *mipiPhy = -2;//mark as blocked
            else if (muxSel == MIPI_MUX5_LANES23_SEL_PHY5_E)
                *mipiPhy = 5;
            break;
        }
    }
    return;
}


static void updateVscCsiTxOut(nu4kT *nu4k, nu4kblkT *blk, unsigned int index)
{
   unsigned int horz = 0, vert = 0, packet_length, output_bpp;
   NUCFG_formatE video_format;

   getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
   output_bpp = getOutputBpp(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);
   video_format = getOutputFormat(nu4k, blk, blk->ti.inst);

   setDbField(nu4k, NUFLD_VSC_CSI_TX_E, blk->ti.inst, NU4100_PPE_VSC_CSI2_VSC_FRAME_SIZE_VERTICAL_LENGTH_E,   vert - 1);
   setDbField(nu4k, NUFLD_VSC_CSI_TX_E, blk->ti.inst, NU4100_PPE_VSC_CSI2_VSC_FRAME_SIZE_HORIZONTAL_LENGTH_E, horz - 1);

   if (((horz * output_bpp) % 8) != 0)
   {
      LOGG_PRINT(LOG_ERROR_E, NULL, "unsupported horizontal size for mipi Tx%d ((%d*%d) % 8) = %d\n",blk->ti.inst, horz,output_bpp, (horz * output_bpp % 8));
      assert(0);
   }
   /* Virtual channels - configure the VSCs also for the output */
   packet_length = ((horz * output_bpp) / 8);
   setDbField(nu4k, NUFLD_VSC_CSI_TX_E, blk->ti.inst, NU4100_PPE_VSC_CSI2_CSI_CTRL_PACKET_LENGTH_E, packet_length);
   setDbField(nu4k, NUFLD_VSC_CSI_TX_E, blk->ti.inst, NU4100_PPE_VSC_CSI2_CSI_CTRL_VIDEO_FORMAT_E, nucfgFmt2csi[video_format]);
}


static void getMipiMuxLanesSel(nu4kT *nu4k, unsigned int inst, unsigned int *mux_sel, unsigned int blk0_index)
{
   if (!blk0_index)
   {
      switch (inst)
      {
      case(0):
          *mux_sel = 0;
          break;
      case(1):
          XMLDB_getValue(nu4k->db, NU4100_IAE_MIPI_MUX_SLU1_LANES01_SEL_E, mux_sel);
          break;
      case(2):
          *mux_sel = 2;
          break;
      default:
          getDbField(nu4k, NUFLD_MIPI_MUX_E, inst - 3, NU4100_IAE_MIPI_MUX_SLU3_LANES01_SEL_E, mux_sel);
          break;
      }
      return;
   }
   else
   {
      switch (inst)
      {
      case(0):
          XMLDB_getValue(nu4k->db, NU4100_IAE_MIPI_MUX_SLU0_LANES23_SEL_E, mux_sel);
          break;
      case(1):
          XMLDB_getValue(nu4k->db, NU4100_IAE_MIPI_MUX_SLU1_LANES23_SEL_E, mux_sel);
          break;
      default:
          getDbField(nu4k, NUFLD_MIPI_MUX_E, inst - 2, NU4100_IAE_MIPI_MUX_SLU2_LANES23_SEL_E, mux_sel);
          break;
      }
      return;
   }
}

    /* [blk1] ====> [blk0]
  blk0_index is the input index of blk0. (in case of multiple inputs)
  blk1_index is the output index of blk1 (in case of multiple outputs)
*/
static void getInput(nu4kT *nu4k, nu4kblkT *blk0, unsigned int blk0_index, nu4kblkT **blk1, unsigned int *blk1_index)
{
    ERRG_codeE ret = XMLDB__RET_SUCCESS;
   switch (blk0->ti.type)
   {
      case(NUCFG_BLK_GEN_E):
      case(NUCFG_BLK_AXIWR_E):
      {

      }
      break;
      case(NUCFG_BLK_MIPI_RX_E):
      {
          parseMipiRxSel(nu4k, blk0->ti.inst, blk1, blk1_index);
      }
      break;
      case(NUCFG_BLK_PAR_RX_E):
      {
         UINT32 sen_inst = 0;
         getDbField(nu4k, NUFLD_PAR_RX_E, blk0->ti.inst, BUS_PARALLEL_SLU_0_SRC_SEL_E, &sen_inst);
         *blk1       = &nu4k->sensors[sen_inst].blk;
         *blk1_index = 0;
      }
         break;
      case(NUCFG_BLK_MEDIATOR_E):
      {
          UINT32 srcInput = 0;

          // max 2 input
          if (blk0_index == 0)
             ret = getDbField(nu4k, NUFLD_MEDIATOR_E, blk0->ti.inst, MEDIATORS_MED_0_SRC_SEL0_E, &srcInput);
          else if (blk0_index == 1)
            ret = getDbField(nu4k, NUFLD_MEDIATOR_E, blk0->ti.inst, MEDIATORS_MED_0_SRC_SEL1_E, &srcInput);

          if (ERRG_FAILED(ret))
          {
              LOGG_PRINT(LOG_ERROR_E, NULL, "illegal src sel%d for mediator %d \n", blk0_index, blk0->ti.inst);
              assert(0);
          }

          *blk1 = &nu4k->sensors[srcInput].blk;
          *blk1_index = 0;
      }
          break;
      case(NUCFG_BLK_SEN_E):
         break;
      case(NUCFG_BLK_SLU_PARALLEL_E):
      case(NUCFG_BLK_SLU_E):
      {
          NUFLD_blkE nufldBlk;
          XMLDB_pathE xmlPath;
          unsigned int sluInst = blk0->ti.inst;
          unsigned int muxSel;
          int mipiPhy = 0;
          if (blk0->ti.type == NUCFG_BLK_SLU_PARALLEL_E)
          {
              nufldBlk = NUFLD_SLU_PARALLEL_E;
              xmlPath = NU4100_IAE_SLU0_CONTROL_INTERFACE_TYPE_E;
          }
          else
          {
              nufldBlk = NUFLD_SLU_E;
              xmlPath = NU4100_IAE_SLU2_CONTROL_INTERFACE_TYPE_E;
          }

         nu4kSluInterfaceTypeE if_type = SLU_PARALLEL_INTERFACE_BT601_E;
         if (blk0->ti.type == NUCFG_BLK_SLU_PARALLEL_E)
         {
             getDbField(nu4k, nufldBlk, blk0->ti.inst, xmlPath, (UINT32*)&if_type);
         }
         else
         {
             if_type = SLU_CSI2_INTERFACE_E;
             sluInst += NU4K_PPU_SRC_SEL_MIN_SLU;//instance matching.  0,1->slu parallel  2,3,4,5->slu  mipi 0-6
         }
         if (if_type==SLU_CSI2_INTERFACE_E)
         {
             getMipiMuxLanesSel(nu4k, sluInst, &muxSel, blk0_index);
             parseMipiMuxToMipiPhy(sluInst, muxSel, blk0_index, &mipiPhy);
             if (mipiPhy == -1)//not found
             {
                assert(0);
             }
             else if (mipiPhy == -2)//blocked
             {
                *blk1 = NULL;
             }
             else
             {
                *blk1 = &nu4k->mipiRx[mipiPhy].blk;
                *blk1_index = 0;
             }
         }
         else if (if_type==SLU_PARALLEL_INTERFACE_BT601_E)
         {
            *blk1       = &nu4k->parRx[blk0->ti.inst].blk;
            *blk1_index = 0;
         }
      }
      break;
      case(NUCFG_BLK_IAU_E):
      case(NUCFG_BLK_IAU_COLOR_E):
      {
         XMLDB_pathE selField;
         UINT32 iau_sel = 0, spareTmp = 0, sluInd=0;
         if(blk0->ti.type  == NUCFG_BLK_IAU_E)
            selField = NU4100_IAE_IIM_CONTROL_IAU0_SEL_E;
         else
            selField = NU4100_IAE_IIM_CONTROL_IAU2_SEL_E;
         XMLDB_getValue(nu4k->db, (XMLDB_pathE)((UINT32)selField + blk0->ti.inst), &iau_sel);
         parseIauSel(nu4k,iau_sel, blk1, blk1_index);

         // we set all slu at the begining to be 1:1 in our db. in case which iau is conected to slu than set the slu to  ratio of 2:1
         //check if input block is slu, and if spare register is configured (1:1 rate) cancel it.
         //we need always use the worst case, if some path requires 2:1, then all should.
         if (((*blk1)->ti.type == NUCFG_BLK_SLU_PARALLEL_E) || ((*blk1)->ti.type == NUCFG_BLK_SLU_E))
         {
             if ((*blk1)->ti.type == NUCFG_BLK_SLU_E)
             {
                 sluInd = 2;
             }
             XMLDB_getValue(nu4k->db, (XMLDB_pathE)NU4100_IAE_SPARE_RW0_SPARE_E, &spareTmp);
             if ((spareTmp >> (sluInd + (*blk1)->ti.inst)) & 0x1)//check if right slu configured to 1:1
             {
                 spareTmp &= ~((1 << (sluInd + (*blk1)->ti.inst)));
                 XMLDB_setValue(nu4k->db, (XMLDB_pathE)NU4100_IAE_SPARE_RW0_SPARE_E, spareTmp);
             }
         }
      }
      break;
      case(NUCFG_BLK_HIST_E):
      {
         UINT32 hist_src=0,en=0;
         getDbField(nu4k, NUFLD_IAU_E, blk0->ti.inst, NU4100_IAE_IAU0_HIST_CONTROL_EN_E, &en);
         if (en)
         {
            getDbField(nu4k, NUFLD_IAU_E, blk0->ti.inst, NU4100_IAE_IAU0_HIST_CONTROL_CIIF_SELECT_E, &hist_src);
            parseHistSel(nu4k, hist_src, blk0->ti.inst, blk1, blk1_index);
         }
      }
      break;
      case(NUCFG_BLK_AXIRD_E):
      {
         UINT32 data_sel = 0;
         getDbField(nu4k, NUFLD_AXIRD_E, blk0->ti.inst, NU4100_PPE_AXI_READ0_CTRL_DATA_SEL_E, &data_sel);
         parseDataSel(nu4k,(UINT8)(data_sel & 0xFF),blk1, blk1_index);
      }
      break;

      case(NUCFG_BLK_PPU_NOSCL_E):
      case(NUCFG_BLK_PPU_SCL_E):
      case(NUCFG_BLK_PPU_HYBSCL_E):
      case(NUCFG_BLK_PPU_HYB_E):
      {
         getPpuInput(nu4k, blk0, blk0_index, blk1, blk1_index);
      }
      break;

      case(NUCFG_BLK_INTERLEAVER_E):
      {
         UINT32 strmEn = 0;
         if (NU4K_PPE_INTERLEAVER_EXISTS(blk0->ti.inst))
         {
            getDbField(nu4k, NUFLD_AXIRD_E, blk0->ti.inst, NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E, &strmEn);
            if (strmEn & (1 << blk0_index))
            {
               *blk1 = &nu4k->readers[NU4K_INTERLEAVER_BASE(blk0->ti.inst) + blk0_index].blk;
               *blk1_index = 0;
            }
         }
      }
      break;
      case(NUCFG_BLK_DPE_E):
      {
         //Treat DPE as 2 instances. DPE_0/1 corresponding to DPE P0/P1
         //Channel selection - Inputs are 0-3 from IAU0-3, and they are selected //to P0L/R and P1L/R.
         getDpeInput(nu4k, blk0, blk0_index, blk1, blk1_index);
      }
      break;
      case(NUCFG_BLK_DPE_HYB_E):
      {
         getDpeHybInput(nu4k, blk0, blk0_index, blk1, blk1_index);
      }
      break;
      case(NUCFG_BLK_DPP_E):
      {
         //DPP has 2 virtual instances. DPP_0/1 are host/cva,respectively.
         getDppInput(nu4k, blk0, blk0_index, blk1, blk1_index);
      }
      break;
      case(NUCFG_BLK_CVA_E):
      {
            // TODO: implement
        *blk1 = &nu4k->ppusHybScl[1].blk;
        *blk1_index = 0;
      }
      break;

      case(NUCFG_BLK_CVARD_E):
      {
          UINT32 frk_en = 0,mtrx1 = 0;
          XMLDB_getValue(nu4k->db, NU4100_CVA_FREAK_ENABLE_ENABLE_E, &frk_en);
          XMLDB_getValue(nu4k->db, NU4100_CVA_TOP_CIIF_MATRIX_1_CIIF_MATRIX_1_E, &mtrx1);
          if ( (blk0->ti.inst == getFreakInstance[mtrx1]) && (frk_en == 1) )  // Temp. Assume that freak reader is 3. (top matrix 0/1 == 0)
          {
              *blk1 = &nu4k->cva[0].blk;
              *blk1_index = 0;
          }
      }
      break;

      case(NUCFG_BLK_DPHY_TX_E):
      {
         UINT32 strmEn = 0, base, dbValue;
         XMLDB_pathE path;
         bool dsiPath = false;
         switch (blk0->ti.inst)
         {
         case(0):
            XMLDB_getValue(nu4k->db, NU4100_PPE_PPE_MISC_PPE_CTRL_MIPI_TX_DPHY0_SEL_E, &dbValue);
            if (dbValue == 0)
            {
                  path = NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_E;
            }
            else if (dbValue == 1)
            {
                  dsiPath = true;
                  path = NU4100_PPE_VSC_DSI0_VSC_CTRL_DATA_SEL_E;
            }
            else
            {
                  assert(0);
            }
            break;
         case(1):
            XMLDB_getValue(nu4k->db, NU4100_PPE_PPE_MISC_PPE_CTRL_MIPI_TX_DPHY1_SEL_E, &dbValue);
            if (dbValue == 0)
            {
                  path = NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_E;
            }
            else if (dbValue == 1)
            {
                  dsiPath = true;
                  path = NU4100_PPE_VSC_DSI0_VSC_CTRL_DATA_SEL_E;
            }
            else if (dbValue == 2)
            {
                  dsiPath = true;
                  path = NU4100_PPE_VSC_DSI1_VSC_CTRL_DATA_SEL_E;
            }
            else
            {
                  assert(0);
            }
            break;
         case(2):
            XMLDB_getValue(nu4k->db, NU4100_PPE_PPE_MISC_PPE_CTRL_MIPI_TX_DPHY2_SEL_E, &dbValue);
            if (dbValue == 0)
            {
                  path = NU4100_PPE_PPE_MISC_MIPI_VI1_CONTROL_STRM_SEL_E;
            }
            else if (dbValue == 1)
            {
                  dsiPath = true;
                  path = NU4100_PPE_VSC_DSI1_VSC_CTRL_DATA_SEL_E;
            }
            else
            {
                  assert(0);
            }
            break;
         case(3):
            XMLDB_getValue(nu4k->db, NU4100_PPE_PPE_MISC_PPE_CTRL_MIPI_TX_DPHY3_SEL_E, &dbValue);
            if (dbValue == 0)
            {
                  path = NU4100_PPE_PPE_MISC_MIPI_VI1_CONTROL_STRM_SEL_E;
            }
            else if (dbValue == 1)
            {
                  // CSI2 doesn't exist
                  assert(0);
            }
            else if (dbValue == 2)
            {
                  dsiPath = true;
                  path = NU4100_PPE_VSC_DSI1_VSC_CTRL_DATA_SEL_E;
            }
            else
            {
                  assert(0);
            }
            break;
         default:
            assert(0);
            break;
         }

         if (dsiPath)
         {
            XMLDB_getValue(nu4k->db, path, &dbValue);
            parseDataSel(nu4k, (UINT8)(dbValue & 0xFF), blk1, blk1_index);
            break;
         }

         XMLDB_getValue(nu4k->db, path, &strmEn);

         //C0 - 
         //Only two VSC CSI can be interleaved into the dphyTx, but the base is the same as if there were 4.
         //Since we define only 2 inputs, then update the base number
         base = blk0_index;
         if (path == NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_E)
         {
            base += 2;
         }
         else if (path == NU4100_PPE_PPE_MISC_MIPI_VI1_CONTROL_STRM_SEL_E)
         {
            base += 1;
         }
         else
         {
            assert(0);
         }

         if (strmEn & (1 << base))
         {
            //The vscCsiTx[] has 4 entries. The VSC are numbered 2,3,5,6.
            //vscCsiTx[0] -> vsc2
            //vscCsiTx[1] -> vsc3
            //vscCsiTx[2] -> vsc5
            //vscCsiTx[3] -> vsc6
            if (path == NU4100_PPE_PPE_MISC_MIPI_VI0_CONTROL_STRM_SEL_E)
            {
                  if (base == 2)
                  {
                     *blk1 = &nu4k->vscCsiTx[0].blk;
                  }
                  else if (base == 3)
                  {
                     *blk1 = &nu4k->vscCsiTx[1].blk;
                  }
                  else
                  {
                     assert(0);
                  }
            }
            else if (path == NU4100_PPE_PPE_MISC_MIPI_VI1_CONTROL_STRM_SEL_E)
            {
                  if (base == 1)
                  {
                     *blk1 = &nu4k->vscCsiTx[2].blk;
                  }
                  else if (base == 2)
                  {
                     *blk1 = &nu4k->vscCsiTx[3].blk;
                  }
                  else
                  {
                     assert(0);
                  }
            }
            else
            {
                  assert(0);
            }

            *blk1_index = 0;
         }
      }
      break;

      case(NUCFG_BLK_VSC_CSI_TX_E):
      {
         UINT32 data_sel = 0;
         getDbField(nu4k, NUFLD_VSC_CSI_TX_E, blk0->ti.inst, NU4100_PPE_VSC_CSI2_VSC_CTRL_DATA_SEL_E, &data_sel);
         parseDataSel(nu4k,(UINT8)(data_sel & 0xFF),blk1, blk1_index);
      }
      break;

      case(NUCFG_BLK_ISP_E):
      {
         UINT32 isp_mode = 0, exp_sel = 0;
         UINT32 exp_sel_offset_isps = NU4100_IPE_IPE_PREPROC_CTRL0_ISP1_CH0_EXP0_SEL_E - NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_EXP0_SEL_E;
         UINT32 exp_sel_offset_channels = NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH1_EXP0_SEL_E - NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_EXP0_SEL_E;
         UINT32 ch_mode_offset_isps = NU4100_IPE_IPE_PREPROC_CTRL0_ISP1_CH0_MODE_E - NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_MODE_E;

         ret = XMLDB_getValue(nu4k->db, (XMLDB_pathE)(NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_MODE_E + blk0_index + (blk0->ti.inst * ch_mode_offset_isps)), &isp_mode);
         if (ret == XMLDB__EMPTY_ENTRY)
         {
             ///assert(0);?
         }
         else
         {
             if (isp_mode == 0)//single exposure
             {
                 ret = XMLDB_getValue(nu4k->db, (XMLDB_pathE)(NU4100_IPE_IPE_PREPROC_CTRL0_ISP0_CH0_EXP0_SEL_E + (exp_sel_offset_channels * blk0_index) + (blk0->ti.inst * exp_sel_offset_isps)), &exp_sel);
                 if (ret == XMLDB__EMPTY_ENTRY)
                 {
                     ///do nothing
                 }
                 else
                 {
                     if (exp_sel == 0)
                     {
                         *blk1 = &nu4k->slusParallel[0].blk;//SLU0
                         *blk1_index = 0;
                     }
                     else if (exp_sel == 1)
                     {
                         *blk1 = &nu4k->slusParallel[1].blk;//SLU1
                         *blk1_index = 0;
                     }
                     else if (exp_sel == 2)
                     {
                         *blk1 = &nu4k->slus[2].blk;//SLU4
                         *blk1_index = 0;
                     }
                     else if (exp_sel == 3)
                     {
                         *blk1 = &nu4k->slus[3].blk;//SLU5
                         *blk1_index = 0;
                     }
                 }
             }
             else if (isp_mode == 1)//dual exposure
             {
                 //TODO: implement dual exposure
             }
         }
      }
      break;

      case(NUCFG_BLK_ISPRD_E):
      {
         UINT32 isp_rd_en = 0, isp_en = 0, blk_offset;
         XMLDB_pathE isp0_rd0_feild = ISPS_ISP0_OUTPUTS_MP_ENABLE_E;
         XMLDB_pathE isp1_rd0_feild = ISPS_ISP1_OUTPUTS_MP_ENABLE_E;
         XMLDB_pathE rd1_feild = ISPS_ISP0_OUTPUTS_SP1_ENABLE_E, rd0_feild;
         UINT32 rdSize = ISPS_ISP0_OUTPUTS_SP1_ENABLE_E - ISPS_ISP0_OUTPUTS_MP_ENABLE_E;
         UINT32 isp_inst, rd_inst = blk0->ti.inst;

         //isp0_mp  = 0,
         //isp0_sp1 = 1,
         //isp0_sp2 = 2,
         //isp1_mp  = 3,
         //isp1_sp1 = 4,
         //isp1_sp2 = 5.
         if (rd_inst < 3)
         {
             rd0_feild = ISPS_ISP0_OUTPUTS_MP_ENABLE_E;
             blk_offset = 0;
             isp_inst = 0;
         }
         else
         {
             rd0_feild = ISPS_ISP1_OUTPUTS_MP_ENABLE_E;
             blk_offset = 3;
             isp_inst = 1;
         }

         XMLDB_getValue(nu4k->db, NU4100_IPE_ENABLE_ISP_EN_E, &isp_en);//enable on IPE block (from nu4100 regs)
         ret = XMLDB_getValue(nu4k->db, (XMLDB_pathE)(rd0_feild + ((rd_inst - blk_offset) * rdSize)) , &isp_rd_en);//enable on external ISP tags (not from nu4100 regs)
         if (ret == XMLDB__EMPTY_ENTRY)
         {
             int isp_rd_en = -1;
         }

#if 1
         if (((isp_en >> isp_inst) & 1) && (isp_rd_en))
         {
             *blk1 = &nu4k->isps[isp_inst].blk;
             *blk1_index = 0;
         }
#else
         else if ((isp_en & 1) && (isp_rd_en))
         {
             *blk1 = &nu4k->isps[01].blk;
             *blk1_index = 0;
         }
         else if((isp_en & 2) && (isp_rd_en))
         {
            *blk1 = &nu4k->isps[1].blk;
             *blk1_index = 0;
         }
#endif
      }
      break;

      default:
         break;
   }
}

static void getOutputRes(nu4kT *nu4k, nu4kblkT *blk,unsigned int index, unsigned int *horzP, unsigned int *vertP)
{
   UINT32 horz = 0,vert = 0;
   ERRG_codeE ret;

   switch(blk->ti.type)
   {
      case(NUCFG_BLK_GEN_E):
      {
         getDbField(nu4k, NUFLD_GEN_E,blk->ti.inst, NU4100_IAE_FGEN0_FRAME_SIZE_HORZ_E,&horz);
         getDbField(nu4k, NUFLD_GEN_E,blk->ti.inst, NU4100_IAE_FGEN0_FRAME_SIZE_VERT_E,&vert);
         if(horz) horz++;
         if(vert) vert++;
      }
      break;
      case(NUCFG_BLK_SEN_E):
      case(NUCFG_BLK_MIPI_RX_E):
      case(NUCFG_BLK_PAR_RX_E):
      {
         //resolution not-available at sensor/mipi/par. ignore
         break;
      }
      break;
      case(NUCFG_BLK_AXIWR_E):
      {
         getDbField(nu4k, NUFLD_AXIWR_E,blk->ti.inst, NU4100_PPE_AXI_WRITE0_SIZE_HSIZE_E,&horz);
         getDbField(nu4k, NUFLD_AXIWR_E,blk->ti.inst, NU4100_PPE_AXI_WRITE0_SIZE_VSIZE_E,&vert);
         if(horz) horz++;
         if(vert) vert++;
      }
      break;
      case(NUCFG_BLK_MEDIATOR_E):
      {
          UINT32 medType = 0;
          ret = getDbField(nu4k, NUFLD_MEDIATOR_E, blk->ti.inst, MEDIATORS_MED_0_TYPE_E, &medType);
          if (ERRG_SUCCEEDED(ret))
          {
              getInterOutputRes(nu4k, blk, &horz, &vert);
          }
          else
          {
              LOGG_PRINT(LOG_ERROR_E, NULL, "mediator inst %d type is illegal\n", blk->ti.inst);
              assert(0);
          }
      }
      break;
      case(NUCFG_BLK_SLU_PARALLEL_E):
      case(NUCFG_BLK_SLU_E):
      {
          XMLDB_pathE sluHorz, sluVert;
          if (blk->ti.type == NUCFG_BLK_SLU_PARALLEL_E)
          {
              sluHorz = NU4100_IAE_SLU0_FRAME_SIZE_HORZ_E;
              sluVert = NU4100_IAE_SLU0_FRAME_SIZE_VERT_E;
          }

          else
          {
              sluHorz = NU4100_IAE_SLU2_FRAME_SIZE_HORZ_E;
              sluVert = NU4100_IAE_SLU2_FRAME_SIZE_VERT_E;
          }
          getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, sluHorz, &horz);
          getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, sluVert, &vert);
         if(horz) horz++;
         if(vert) vert++;
      }
      break;
      case(NUCFG_BLK_IAU_E):
      case(NUCFG_BLK_IAU_COLOR_E):
      {
         XMLDB_pathE dsrHorz,dsrVert;
         if(blk->ti.type == NUCFG_BLK_IAU_E)
         {
            dsrHorz = NU4100_IAE_IAU0_DSR_OUT_SIZE_HORZ_E;
            dsrVert= NU4100_IAE_IAU0_DSR_OUT_SIZE_VERT_E;
         }
         else
         {
            dsrHorz = NU4100_IAE_IAU2_DSR_OUT_SIZE_HORZ_E;
            dsrVert= NU4100_IAE_IAU2_DSR_OUT_SIZE_VERT_E;
         }
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst,dsrHorz,&horz);
         getDbField(nu4k, nusoc_convert2Fld(blk->ti.type),blk->ti.inst,dsrVert,&vert);
         if(horz) horz++;
         if(vert) vert++;
      }
      break;
      case(NUCFG_BLK_HIST_E):
      {
         horz = 1;
         vert = HIST_TOTAL_SIZE;
      }
      break;
      case(NUCFG_BLK_PPU_NOSCL_E):
      case(NUCFG_BLK_PPU_SCL_E):
      case(NUCFG_BLK_PPU_HYBSCL_E):
      case(NUCFG_BLK_PPU_HYB_E):
      {
         getPpuOutRes(nu4k,blk, index, &horz, &vert);
      }
      break;
      case(NUCFG_BLK_AXIRD_E):
      case(NUCFG_BLK_VSC_CSI_TX_E):
      {
         //get input resolution (single input) by getting reader's input block and select
         if(blk->inputs[0].blkp)
         {
            getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
         }
      }
      break;
      case(NUCFG_BLK_INTERLEAVER_E):
      {
         getInterOutputRes(nu4k,blk, &horz, &vert);
      }
      break;
      case(NUCFG_BLK_DPE_E):
      {
         //output resolution of instance 0 or 1
         getDpeOutRes(nu4k, blk,&horz, &vert);
      }
      break;
      case(NUCFG_BLK_DPE_HYB_E):
      {
         //hybrid output resolution
         getDpeHybOutRes(nu4k,blk, &horz, &vert);
         break;
      }
      break;

      case(NUCFG_BLK_DPP_E):
      {
         getDppOutRes(nu4k,blk,&horz,&vert);
      }
     break;
      case(NUCFG_BLK_CVA_E):
      {
            UINT32 frk_en, stitch_bp = 0xFFFFFFFF;
         UINT32 horz1, horz2;
         ERRG_codeE ret;
            // TODO: implement
             //printf("found CVA FRK res\n");

         ret = XMLDB_getValue(nu4k->db, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E, &stitch_bp);
            getDbField(nu4k, NUFLD_CVA_E,blk->ti.inst, NU4100_CVA_FREAK_ENABLE_ENABLE_E,&frk_en);
            if(frk_en == 1)
            {
                getDbField(nu4k, NUFLD_CVA_E,blk->ti.inst, NU4100_CVA_FREAK_CONFIG2_IMG1_IMAGE_HEIGHT_E,&vert);
            getDbField(nu4k, NUFLD_CVA_E,blk->ti.inst, NU4100_CVA_FREAK_CONFIG2_IMG1_IMAGE_WIDTH_E,&horz1);
            getDbField(nu4k, NUFLD_CVA_E,blk->ti.inst, NU4100_CVA_FREAK_CONFIG2_IMG2_IMAGE_WIDTH_E,&horz2);
            if( (ret == XMLDB__EMPTY_ENTRY) || (stitch_bp == 0))
            {
               horz = horz1 + horz2;
            }
            else if(stitch_bp == 1)
            {
               horz = horz1;
            }
                //printf("\t\t\t NUCFG_BLK_CVA_E found CVA FRK res: H-%d, V-%d, BP %x\n", horz,vert, stitch_bp);
            }
      }
      break;

      case(NUCFG_BLK_CVARD_E):
      {
         UINT32 frk_en, stitch_bp = 0xFFFFFFFF;
         UINT32 horz1, horz2, mtrx1 = 0;
         ERRG_codeE ret;
         ret = XMLDB_getValue(nu4k->db, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E, &stitch_bp);
         getDbField(nu4k, NUFLD_CVA_E,0,NU4100_CVA_FREAK_ENABLE_ENABLE_E,&frk_en);

         XMLDB_getValue(nu4k->db, NU4100_CVA_TOP_CIIF_MATRIX_1_CIIF_MATRIX_1_E, &mtrx1);
         if((frk_en == 1) && (blk->ti.inst == getFreakInstance[mtrx1]))
         {     // instead of h/v num of kp and kp width
//            getDbField(nu4k, NUFLD_CVA_E,blk->ti.inst, NU4100_CVA_DOG_BUCKET_KEYPOINTS_OUT_NUM_E,&vert);
//            getDbField(nu4k, NUFLD_CVA_E,blk->ti.inst, NU4100_CVA_AXI_IF3_CTRL_DATA_WIDTH_E,&horz);
              getDbField(nu4k, NUFLD_CVA_E, 0, NU4100_CVA_FREAK_CONFIG2_IMG1_IMAGE_HEIGHT_E,&vert);//we have 1 CVA block. inside we have 6 CVA axi rd
              getDbField(nu4k, NUFLD_CVA_E, 0, NU4100_CVA_FREAK_CONFIG2_IMG1_IMAGE_WIDTH_E,&horz1);
              getDbField(nu4k, NUFLD_CVA_E, 0, NU4100_CVA_FREAK_CONFIG2_IMG2_IMAGE_WIDTH_E,&horz2);
           if( (ret == XMLDB__EMPTY_ENTRY) || (stitch_bp == 0))
           {
              horz = horz1 + horz2;
           }
           else if(stitch_bp == 1)
           {
              horz = horz1;
           }
           //printf("\t\t\t NUCFG_BLK_CVA_E found CVA FRK res: H-%d, V-%d, BP %x\n", horz,vert, stitch_bp);
         }
      }
      break;

      case(NUCFG_BLK_DPHY_TX_E):
      {
         getDphyTxOutputRes(nu4k,blk, &horz, &vert);
      }
      break;

      case(NUCFG_BLK_ISP_E):
      {
          getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &horz, &vert);
      }

      case(NUCFG_BLK_ISPRD_E):
      {
         getIspRdOutputRes(nu4k,blk, &horz, &vert);
      }
      break;

      default:
      break;
   }

   if(horzP)
      *horzP = horz;
   if(vertP)
      *vertP = vert;
}

static unsigned int getOutputBpp(nu4kT *nu4k, nu4kblkT *blk, unsigned int index)
{
   unsigned int bpp = 0;

   switch(blk->ti.type)
   {
      case(NUCFG_BLK_GEN_E):
      {
         bpp = getGeneratorOutBpp();
      }
      break;
      case(NUCFG_BLK_SEN_E):
      {
         bpp = getSensorOutBpp(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_AXIWR_E):
      {
         bpp = getWriterOutBpp(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_SLU_PARALLEL_E):
      case(NUCFG_BLK_SLU_E):
      {
         bpp = getSluOutBpp(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_IAU_E):
      case(NUCFG_BLK_IAU_COLOR_E):
      {
         bpp = getIauOutBpp(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_PPU_NOSCL_E):
      case(NUCFG_BLK_PPU_SCL_E):
      case(NUCFG_BLK_PPU_HYBSCL_E):
      case(NUCFG_BLK_PPU_HYB_E):
      {
         bpp = getPpuOutBpp(nu4k,blk,index);
      }
      break;
      case(NUCFG_BLK_AXIRD_E):
      {
         //get input bpp (single input) by getting reader's input block and select
         if(blk->inputs[0].blkp)
         {
            bpp = getOutputBpp(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);
         }
      }
      break;
      case(NUCFG_BLK_INTERLEAVER_E):
      {
         bpp = getInterOutBpp(nu4k,blk);
      }
      break;

      case(NUCFG_BLK_DPE_E):
      case(NUCFG_BLK_DPE_HYB_E):
      {
         bpp = getDpeOutBpp(nu4k,blk);
      }
      break;
      case(NUCFG_BLK_DPP_E):
      {
         bpp = 16;//TODO
      }
      break;
      case(NUCFG_BLK_CVA_E):
      {
         UINT32 mtrx1 = 0;
         XMLDB_getValue(nu4k->db, NU4100_CVA_TOP_CIIF_MATRIX_1_CIIF_MATRIX_1_E, &mtrx1);
         if(blk->ti.inst == getFreakInstance[mtrx1])
         bpp = 672;//TODO
      }
      break;

      case(NUCFG_BLK_CVARD_E):
      {
         UINT32   mtrx1 = 0;
         XMLDB_getValue(nu4k->db, NU4100_CVA_TOP_CIIF_MATRIX_1_CIIF_MATRIX_1_E, &mtrx1);
          if(blk->ti.inst == getFreakInstance[mtrx1])
          bpp = 672;//TODO
      }
      break;

      case(NUCFG_BLK_DPHY_TX_E):
      case(NUCFG_BLK_VSC_CSI_TX_E):
      {
         if (blk->inputs[0].blkp)
         {
            bpp = getOutputBpp(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel); //arnon TBD
         }
      }
      break;

      case(NUCFG_BLK_ISP_E):
      {
          if (blk->inputs[0].blkp)
          {
              bpp = getOutputBpp(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel); //TODO: check if isp should change bpp
          }
      }
      break;
      case(NUCFG_BLK_ISPRD_E):
      {
          if (blk->inputs[0].blkp)
          {
              bpp = NUCFG_format2Bpp(getIspOutputFormat(nu4k, blk));
          }
      }
      break;
      default:
      break;
   }
   return bpp;
}

static UINT32 countBits(UINT32 num)
{
   UINT32 cnt = 0;
   while (num != 0)
   {
      if (num & 0x1)
      {
         cnt++;
      }
      num = num >> 1;
   }
   return cnt;
}

static int findPosFromMask(UINT32 mask)
{
   int cnt = 0;

   if (!mask)
      return cnt;

   while (!(mask & 0x1))
   {
      cnt++;
      mask = mask >> 1;
   }
   return cnt;
}
static void describeNumBits(nu4kT *nu4k, UINT32 fullFormatDisp, UINT32 fullFormatSubPixel, unsigned int inst, UINT32* depthPosOut, UINT32 *numBitsDisp, UINT32 *numBitsSubPixle)
{
   UINT32 mask = 0xFFFF, depthPos, depthShift, tmp;
   XMLDB_pathE maskPath, depthPosPath, depthShiftPath;
   if (inst == 0)
   {
      maskPath = NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_DEPTH_MASK_E;
      depthPosPath = NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_DEPTH_POS_E;
      depthShiftPath = NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_DEPTH_SHIFT_E;
   }
   else
   {
      maskPath = NU4100_PPE_DEPTH_POST_MISC_CVA_CTRL_DEPTH_MASK_E;
      depthPosPath = NU4100_PPE_DEPTH_POST_MISC_CVA_CTRL_DEPTH_POS_E;
      depthShiftPath = NU4100_PPE_DEPTH_POST_MISC_CVA_CTRL_DEPTH_SHIFT_E;
   }

   getDbField(nu4k, NUFLD_DPP_E, 0, maskPath, &mask);
   getDbField(nu4k, NUFLD_DPP_E, 0, depthPosPath, &depthPos);
   getDbField(nu4k, NUFLD_DPP_E, 0, depthShiftPath, &depthShift);

   *depthPosOut = depthPos + findPosFromMask(mask);//add diff from mask

   //for disparity or depth
   tmp = (fullFormatDisp & mask) >> depthShift;
   tmp = tmp << depthPos;
   *numBitsDisp = countBits(tmp);

   //for sub pixel
   tmp = (fullFormatSubPixel & mask) >> depthShift;
   tmp = tmp << depthPos;
   *numBitsSubPixle = countBits(tmp);
}

static NUCFG_formatDiscriptorU getOutputFormatDiscriptor(nu4kT *nu4k, nu4kblkT *blk,unsigned int index, NUCFG_formatDiscriptorU *formatDiscriptor)
{
   (void)index;
   NUCFG_formatDiscriptorU fmtDisc;
   memset(&fmtDisc,0,sizeof(NUCFG_formatDiscriptorU));

   switch(blk->ti.type)
   {
      case(NUCFG_BLK_GEN_E):
      case(NUCFG_BLK_SEN_E):
      case(NUCFG_BLK_MEDIATOR_E):
      case(NUCFG_BLK_MIPI_RX_E):
      case(NUCFG_BLK_PAR_RX_E):
      case(NUCFG_BLK_AXIWR_E):
      case(NUCFG_BLK_SLU_PARALLEL_E):
      case(NUCFG_BLK_SLU_E):
      case(NUCFG_BLK_IAU_E):
      case(NUCFG_BLK_IAU_COLOR_E):
      case(NUCFG_BLK_HIST_E):
      case(NUCFG_BLK_PPU_NOSCL_E):
      case(NUCFG_BLK_PPU_SCL_E):
      case(NUCFG_BLK_PPU_HYBSCL_E):
      case(NUCFG_BLK_PPU_HYB_E):
      case(NUCFG_BLK_AXIRD_E):
      case(NUCFG_BLK_INTERLEAVER_E):
      {
         //Get the format of the first interleaved stream (first non-null input)
         //Note that it's possible to interleave streams of the same pixel width but different formats,
         //  but we can only output one format.
         unsigned int i;
         for(i = 0; i < blk->numInputs; i++)
         {
            if(blk->inputs[i].blkp)
            {
               fmtDisc=getOutputFormatDiscriptor(nu4k,blk->inputs[i].blkp, blk->inputs[i].sel, formatDiscriptor);
               break;
            }
         }
      }
      break;
      case(NUCFG_BLK_DPE_E):
      case(NUCFG_BLK_DPE_HYB_E):
      {
         formatDiscriptor->depthFields.minDisp           = nu4k->dpeFunc.minDispFuncParams.minDisp;

         unsigned int i;
         for (i = 0; i < blk->numInputs; i++)
         {
            if (blk->inputs[i].blkp)
            {
               getOutputFormatDiscriptor(nu4k, blk->inputs[i].blkp, blk->inputs[i].sel, formatDiscriptor);
            }
         }
      }
      break;

      case(NUCFG_BLK_DPP_E):
      {
         UINT32 srcSelect = 0, disparityOrDepthNumBits, disparitySubPixleNumBits, depthPos = 0;
         if (blk->ti.inst == 0)//depth_post_host
         {
            getDbField(nu4k, NUFLD_DPP_E, 0, NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_SRC_SEL_E, &srcSelect);
            formatDiscriptor->depthFields.depthScaleFactor = nu4k->dpeFunc.depthScaleFactor.depthScaleFactorHost;
         }
         else if (blk->ti.inst == 1)//depth_post_cva
         {
            getDbField(nu4k, NUFLD_DPP_E, 0, NU4100_PPE_DEPTH_POST_MISC_CVA_CTRL_SRC_SEL_E, &srcSelect);
            formatDiscriptor->depthFields.depthScaleFactor = nu4k->dpeFunc.depthScaleFactor.depthScaleFactorCva;
         }
         else
         {
            assert(0);
         }

         if (srcSelect == NU4K_DPP_SRCSEL_PRIM_DISP)
         {
            //getDbField(nu4k, NUFLD_DPP_E, blk->ti.inst, NU4100_PPE_DEPTH_POST_MISC_HOST_CTRL_SRC_SEL_E, &srcSelect);
            describeNumBits(nu4k, 0xFFC0, 0x3F, blk->ti.inst, &depthPos, &disparityOrDepthNumBits, &disparitySubPixleNumBits);
            formatDiscriptor->depthFields.disparityOrDepthNumBits = disparityOrDepthNumBits;
            formatDiscriptor->depthFields.disparitySubPixleNumBits = disparitySubPixleNumBits;
         }
         else if((srcSelect == NU4K_DPP_SRCSEL_REG_PRIM_DEPTH) || (srcSelect == NU4K_DPP_SRCSEL_PRIM_DEPTH))
         {
            describeNumBits(nu4k, 0xFFFF, 0x0, blk->ti.inst, &depthPos, &disparityOrDepthNumBits, &disparitySubPixleNumBits);
            formatDiscriptor->depthFields.disparityOrDepthNumBits = disparityOrDepthNumBits;
            formatDiscriptor->depthFields.disparitySubPixleNumBits = 0;
         }
         else
         {
            LOGG_PRINT(LOG_ERROR_E, NULL, "unsupported case for DPP src select %d\n", srcSelect);
            assert(0);
         }
         formatDiscriptor->depthFields.depthPos = depthPos;

         unsigned int i;
         for (i = 0; i < blk->numInputs; i++)
         {
            if (blk->inputs[i].blkp)
            {
               getOutputFormatDiscriptor(nu4k, blk->inputs[i].blkp, blk->inputs[i].sel, formatDiscriptor);
            }
         }
     }
     break;

      default:
      break;
   }
   return fmtDisc;
}

static NUCFG_formatE getOutputFormat(nu4kT *nu4k, nu4kblkT *blk,unsigned int index)
{
   NUCFG_formatE fmt = NUCFG_FORMAT_GREY_16_E;

   switch(blk->ti.type)
   {
      case(NUCFG_BLK_GEN_E):
      {
         fmt = getGeneratorFormat();
      }
      break;
      case(NUCFG_BLK_SEN_E):
      case(NUCFG_BLK_MEDIATOR_E):
      case(NUCFG_BLK_MIPI_RX_E):
      case(NUCFG_BLK_PAR_RX_E):
      {
         fmt = getSensorFormat(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_AXIWR_E):
      {
         fmt = getWriterFormat(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_SLU_PARALLEL_E):
      case(NUCFG_BLK_SLU_E):
      {
         fmt = getSluFormat(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_IAU_E):
      case(NUCFG_BLK_IAU_COLOR_E):
      {
         fmt = getIauOutputFormat(nu4k, blk);
      }
      break;
      case(NUCFG_BLK_HIST_E):
      {
         fmt = NUCFG_FORMAT_HISTOGRAM_E;
      }
      break;
      case(NUCFG_BLK_PPU_NOSCL_E):
      case(NUCFG_BLK_PPU_SCL_E):
      case(NUCFG_BLK_PPU_HYBSCL_E):
      case(NUCFG_BLK_PPU_HYB_E):
      {
         fmt = getPpuOutFormat(nu4k,blk,index);
      }
      break;
      case(NUCFG_BLK_AXIRD_E):
      {
         //get input format (single input) by getting reader's input block and select
         if(blk->inputs[0].blkp)
         {
            fmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);
         }
      }
      break;
      case(NUCFG_BLK_INTERLEAVER_E):
      {
         //Get the format of the first interleaved stream (first non-null input)
         //Note that it's possible to interleave streams of the same pixel width but different formats,
         //  but we can only output one format.
         unsigned int i;
         for(i = 0; i < blk->numInputs; i++)
         {
            if(blk->inputs[i].blkp)
            {
               fmt = getOutputFormat(nu4k,blk->inputs[i].blkp, blk->inputs[i].sel);
               break;
            }
         }
      }
      break;
      case(NUCFG_BLK_DPE_E):
      case(NUCFG_BLK_DPE_HYB_E):
      {
         fmt = getDpeOutFormat(nu4k,blk);
      }
      break;

      case(NUCFG_BLK_DPP_E):
      {
         fmt = getDppOutFormat(nu4k,blk);
      }
      break;
      case(NUCFG_BLK_CVA_E):
      {
            // TODO: implement
            fmt = NUCFG_FORMAT_GEN_672_E;
      }
      break;

      case(NUCFG_BLK_CVARD_E):
      {
         UINT32   mtrx1 = 0;
         XMLDB_getValue(nu4k->db, NU4100_CVA_TOP_CIIF_MATRIX_1_CIIF_MATRIX_1_E, &mtrx1);
         if(blk->ti.inst == getFreakInstance[mtrx1])
            fmt = NUCFG_FORMAT_GEN_672_E;
      }
      break;

      case(NUCFG_BLK_DPHY_TX_E):
      {
          if (blk->inputs[0].blkp)
          {
              fmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel); // assuming same format for all VSC CSI's
          }
      }
      break;

      case(NUCFG_BLK_ISP_E):
      {
          if (blk->inputs[0].blkp)
          {
              fmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);
          }
      }
      break;
      case(NUCFG_BLK_ISPRD_E):
      {
          if (blk->inputs[0].blkp)
          {
              fmt = getIspOutputFormat(nu4k, blk);
          }
      }
      break;

      case(NUCFG_BLK_VSC_CSI_TX_E):
      {
          fmt = geVscFormat(nu4k, blk);
      }
      break;

      default:
      break;
   }

   return fmt;
}

static int isExtInterleaverExist(nu4kT *nu4k, nu4kblkT *blk, nu4kblkT** interBlk)
{
    int exist = 0;
    UINT32 medType;
    ERRG_codeE ret;

    while (blk->inputs)
    {
        if (blk->inputs[0].blkp->ti.type == NUCFG_BLK_MEDIATOR_E)
        {
            ret = getDbField(nu4k, NUFLD_MEDIATOR_E, blk->inputs[0].blkp->ti.inst, MEDIATORS_MED_0_TYPE_E, &medType);
            if (ERRG_SUCCEEDED(ret))
            {
                if (medType == 0)//interleaver
                {
                    *interBlk = blk->inputs[0].blkp;
                    return 1;
                }
            }
        }
        blk = blk->inputs[0].blkp;
    }
    return exist;
}

static void updateOutput(nu4kT *nu4k, nu4kblkT *blk, unsigned int index, nusoc_updateT *params)
{
   switch(blk->ti.type)
   {
      case(NUCFG_BLK_GEN_E):
      {
         //TODO: implement
      }
      break;
      case(NUCFG_BLK_SEN_E):
      {
         nusoc_updateSensT *p = (nusoc_updateSensT *)params;
         if (p)
         {
         updateSen(nu4k, blk, (p->sensorOperatingModeValid) ? &p->sensorOperatingMode : NULL,
                              (p->sensorVerticalOffsetValid) ? &p->sensorVerticalOffset : NULL,
                              (p->sensorGainOffsetValid) ? &p->sensorGainOffset : NULL);
         }
      }
      break;
      case(NUCFG_BLK_AXIWR_E):
      {
         nusoc_updateWriterT *p = (nusoc_updateWriterT *)params;
         if (p)
         {
             updateWriter(nu4k, blk, (p->horizontalSizeValid) ? p->horizontalSize - 1 : 0,
                                 (p->verticalSizeValid) ? p->verticalSize - 1: 0);
         }
      }
      break;

      case(NUCFG_BLK_MIPI_RX_E):
      case(NUCFG_BLK_PAR_RX_E):
      case(NUCFG_BLK_MEDIATOR_E)://since we take full sizes from sensors
      break; //no update

      case(NUCFG_BLK_SLU_PARALLEL_E):
      case(NUCFG_BLK_SLU_E):
      {
//for backward compatibility
#define NU4k_DEFUALT_SLU_OUTPUT_HORIZONTAL_SIZE (1280)
#define NU4k_DEFUALT_SLU_OUTPUT_HORIZONTAL_CROP_OFFSET (0)
#define NU4k_DEFUALT_SLU_OUTPUT_VERTICAL_SIZE (800)
#define NU4k_DEFUALT_SLU_OUTPUT_VERTICAL_CROP_OFFSET (0)
          UINT32 verticalCropOffset = NU4k_DEFUALT_SLU_OUTPUT_VERTICAL_CROP_OFFSET;
          UINT32 horizontalCropOffset = NU4k_DEFUALT_SLU_OUTPUT_HORIZONTAL_CROP_OFFSET;
          UINT32 sluOutputVerticalSize = NU4k_DEFUALT_SLU_OUTPUT_VERTICAL_SIZE;
          UINT32 sluOutputHorizontalSize = NU4k_DEFUALT_SLU_OUTPUT_HORIZONTAL_SIZE;
          nu4kblkT *interBlk = NULL;
          UINT32 interFactor = 1;

          //check if mediator of type 'external interleaver' exist on path so slu will get horz multiplied by 2
          if (isExtInterleaverExist(nu4k, blk, &interBlk))
              interFactor = 2;

          nusoc_updateSluT *p = (nusoc_updateSluT *)params;
          bool success = updateSluUserCrop(nu4k,blk,blk->ti.type);
          if(!success) /*Modified by William.Bright to add support for user cropping.
                        If user cropping doesn't exist for this SLU then normal SLU cropping values are used */
          {
            if (p)
            {
            
               updateSlu(nu4k, blk, (p->sluOutputHorizontalSizeValid) ? &p->sluOutputHorizontalSize : &sluOutputHorizontalSize,
                  (p->horizontalCropOffsetValid) ? &p->horizontalCropOffset : &horizontalCropOffset,
                  (p->sluOutputVerticalSizeValid) ? &p->sluOutputVerticalSize : &sluOutputVerticalSize,
                  (p->verticalCropOffsetValid) ? &p->verticalCropOffset : &verticalCropOffset, interFactor);

            }
          }
      }
      break;
      case(NUCFG_BLK_IAU_E):
      case(NUCFG_BLK_IAU_COLOR_E):
      {
//for backward compatibility
#define NU4k_DEFUALT_IAU_IB_VERTICAL_LUT_OFFSET (0)
#define NU4k_DEFUALT_IAU_IB_HORIZONTAL_LUT_OFFSET (0)
#define NU4k_DEFUALT_IAU_IB_OFFSET (0x2b)
#define NU4k_DEFUALT_IAU_IB_OFFSET_COLOR (0)
#define NU4k_DEFUALT_IAU_DSR_VERTICAL_LUT_OFFSET (0)
#define NU4k_DEFUALT_IAU_DSR_HORIZONTAL_LUT_OFFSET (0)
#define NU4k_DEFUALT_IAU_DSR_VERTICAL_OUTPUT_SIZE (0x0000031f)
#define NU4k_DEFUALT_IAU_DSR_HORIZONTAL_OUTPUT_SIZE (0x000004ff)
#define NU4k_DEFUALT_IAU_DSR_VERTICAL_BLOCK_SIZE (16)
#define NU4k_DEFUALT_IAU_DSR_HORIZONTAL_BLOCK_SIZE (8)
#define NU4k_DEFUALT_IAU_DSR_LUT_HORIZONTAL_SIZE (162)
#define NU4k_DEFUALT_IAU_DSR_HORIZONTAL_DELTA_SIZE (14)
#define NU4k_DEFUALT_IAU_DSR_HORIZONTAL_DELTA_FRAC_BITS (4)
#define NU4k_DEFUALT_IAU_DSR_VERTICAL_DELTA_SIZE (13)
#define NU4k_DEFUALT_IAU_DSR_VERTICAL_DELTA_FRAC_BITS (4)
#define NU4k_DEFUALT_IAU_IB_VERTICAL_BLOCK_SIZE (16)
#define NU4k_DEFUALT_IAU_IB_HORIZONTAL_BLOCK_SIZE (8)
#define NU4k_DEFUALT_IAU_IB_LUT_HORIZONTAL_SIZE (0x000000a2)
#define NU4k_DEFUALT_IAU_IB_FIXED_POINT_FRAC_BITS (0x0000000b)
      UINT32 ibVerticalLutOffset = NU4k_DEFUALT_IAU_IB_VERTICAL_LUT_OFFSET;
      UINT32 ibHorizontalLutOffset = NU4k_DEFUALT_IAU_IB_HORIZONTAL_LUT_OFFSET;
      UINT32 ibOffset = NU4k_DEFUALT_IAU_IB_OFFSET;
      UINT32 ibOffsetColor = NU4k_DEFUALT_IAU_IB_OFFSET_COLOR;
      UINT32 dsrVerticalLutOffset = NU4k_DEFUALT_IAU_DSR_VERTICAL_LUT_OFFSET;
      UINT32 dsrHorizontalLutOffset = NU4k_DEFUALT_IAU_DSR_HORIZONTAL_LUT_OFFSET;
      UINT32 dsrOutputVerticalSize = NU4k_DEFUALT_IAU_DSR_VERTICAL_OUTPUT_SIZE;
      UINT32 dsrOutputHorizontalSize = NU4k_DEFUALT_IAU_DSR_HORIZONTAL_OUTPUT_SIZE;
      UINT32 dsrVerticalBlockSize = NU4k_DEFUALT_IAU_DSR_VERTICAL_BLOCK_SIZE;
      UINT32 dsrHorizontalBlockSize = NU4k_DEFUALT_IAU_DSR_HORIZONTAL_BLOCK_SIZE;
      UINT32 dsrLutHorizontalSize = NU4k_DEFUALT_IAU_DSR_LUT_HORIZONTAL_SIZE;
      UINT32 dsrHorizontalDeltaSize = NU4k_DEFUALT_IAU_DSR_HORIZONTAL_DELTA_SIZE;
      UINT32 dsrHorizontalDeltaFractionalBits = NU4k_DEFUALT_IAU_DSR_HORIZONTAL_DELTA_FRAC_BITS;
      UINT32 dsrVerticalDeltaSize = NU4k_DEFUALT_IAU_DSR_VERTICAL_DELTA_SIZE;
      UINT32 dsrVerticalDeltaFractionalBits = NU4k_DEFUALT_IAU_DSR_VERTICAL_DELTA_FRAC_BITS;
      UINT32 ibVerticalBlockSize = NU4k_DEFUALT_IAU_IB_VERTICAL_BLOCK_SIZE;
      UINT32 ibHorizontalBlockSize = NU4k_DEFUALT_IAU_IB_HORIZONTAL_BLOCK_SIZE;
      UINT32 ibLutHorizontalSize = NU4k_DEFUALT_IAU_IB_LUT_HORIZONTAL_SIZE;
      UINT32 ibFixedPointFractionalBits = NU4k_DEFUALT_IAU_IB_FIXED_POINT_FRAC_BITS;

      getOutputRes(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel, &dsrOutputHorizontalSize, &dsrOutputVerticalSize);

         nusoc_updateIauT *p = (nusoc_updateIauT *)params;
         if (p)
         {
             updateIau(nu4k, blk, (p->ibVerticalLutOffsetValid) ? &p->ibVerticalLutOffset : &ibVerticalLutOffset,
                 (p->ibHorizontalLutOffsetValid) ? &p->ibHorizontalLutOffset : &ibHorizontalLutOffset,
                 (p->ibOffsetValid) ? &p->ibOffset : &ibOffset,
                 (p->ibOffsetRValid) ? &p->ibOffsetR : &ibOffsetColor,
                 (p->ibOffsetGRValid) ? &p->ibOffsetGR : &ibOffsetColor,
                 (p->ibOffsetGBValid) ? &p->ibOffsetGB : &ibOffsetColor,
                 (p->ibOffsetBValid) ? &p->ibOffsetB : &ibOffsetColor,
                 (p->dsrVerticalLutOffsetValid) ? &p->dsrVerticalLutOffset : &dsrVerticalLutOffset,
                 (p->dsrHorizontalLutOffsetValid) ? &p->dsrHorizontalLutOffset : &dsrHorizontalLutOffset,
                 (p->dsrOutputVerticalSizeValid) ? &p->dsrOutputVerticalSize : &dsrOutputVerticalSize,
                 (p->dsrOutputHorizontalSizeValid) ? &p->dsrOutputHorizontalSize : &dsrOutputHorizontalSize,

                 (p->dsrVerticalBlockSizeValid) ? &p->dsrVerticalBlockSize : &dsrVerticalBlockSize,
                 (p->dsrHorizontalBlockSizeValid) ? &p->dsrHorizontalBlockSize : &dsrHorizontalBlockSize,
                 (p->dsrLutHorizontalSizeValid) ? &p->dsrLutHorizontalSize : &dsrLutHorizontalSize,
                 (p->dsrHorizontalDeltaSizeValid) ? &p->dsrHorizontalDeltaSize : &dsrHorizontalDeltaSize,
                 (p->dsrHorizontalDeltaFractionalBitsValid) ? &p->dsrHorizontalDeltaFractionalBits : &dsrHorizontalDeltaFractionalBits,
                 (p->dsrVerticalDeltaSizeValid) ? &p->dsrVerticalDeltaSize : &dsrVerticalDeltaSize,
                 (p->dsrVerticalDeltaFractionalBitsValid) ? &p->dsrVerticalDeltaFractionalBits : &dsrVerticalDeltaFractionalBits,
                 (p->ibVerticalBlockSizeValid) ? &p->ibVerticalBlockSize : &ibVerticalBlockSize,
                 (p->ibHorizontalBlockSizeValid) ? &p->ibHorizontalBlockSize : &ibHorizontalBlockSize,
                 (p->ibLutHorizontalSizeValid) ? &p->ibLutHorizontalSize : &ibLutHorizontalSize,
                 (p->ibFixedPointFractionalBitsValid) ? &p->ibFixedPointFractionalBits : &ibFixedPointFractionalBits);
         }
      }
      break;
      case(NUCFG_BLK_PPU_NOSCL_E):
      case(NUCFG_BLK_PPU_SCL_E):
      case(NUCFG_BLK_PPU_HYBSCL_E):
      case(NUCFG_BLK_PPU_HYB_E):
      {
         updatePpuOut(nu4k,blk,index);
      }
      break;

      case(NUCFG_BLK_AXIRD_E):
      case(NUCFG_BLK_INTERLEAVER_E):
      case(NUCFG_BLK_ISPRD_E):
      break;//no update

      case(NUCFG_BLK_DPE_E):
      {
         updateDpeOut(nu4k,blk,index);
      }
      break;
      case(NUCFG_BLK_DPE_HYB_E):
      break; //no update
      case(NUCFG_BLK_DPP_E):
      {
         nusoc_updateDispT *update = (nusoc_updateDispT *)params;
      //if (update) -- Not needed here cause update is checked inside the function
         {
             updateDppOut(nu4k, blk, index, update);
         }
      }
      break;
      case(NUCFG_BLK_CVA_E):
      {
            // TODO: implement
      }
      break;

      case(NUCFG_BLK_CVARD_E):
      {
          // TODO: implement
      }
      break;

      case(NUCFG_BLK_VSC_CSI_TX_E):
      {
         updateVscCsiTxOut(nu4k,blk,index);
      }
      break;

      default:
         break;
   }
}

static char* getBlkName(nuBlkTypeE type)
{
    return blkName[type];
}
static void initBlk(nu4kblkT *blk, nuBlkTypeE type, unsigned int inst, unsigned int numInputs,unsigned int numOutputs)
{
   unsigned int i;
   if (nusoc_convert2Fld(type) != NUFLD_INVALID_E)
      blk->hwId = getBlkName(type);//we save just the name of the blk. in the past there was a static table (xml_path.c) which contained the strings
                                   //It used for 'shoeChannelInfo' printing. there we print the blkName+instance and don't save it
   else
      blk->hwId = NULL;
   blk->ti.type = type;
   blk->ti.inst = inst;
   blk->numInputs = numInputs;
   blk->numOutputs = numOutputs;
   blk->inputs =/* blk->outputs* = */ NULL;
   if(numInputs > 0)
   {
      blk->inputs = (nuselectT *)malloc(numInputs * sizeof(nuselectT));
      for(i=0;blk->inputs && (i < numInputs);i++)
         blk->inputs[i].blkp = NULL;
   }
}

static void deInitBlk(nu4kblkT *blk)
{
   blk->numInputs = 0;
   blk->numOutputs = 0;
   if(blk->inputs)
   {
      free(blk->inputs);
      blk->inputs = NULL;
   }
   /*if(blk->inputs)
   {
      free(blk->outputs);
      blk->outputs = NULL;
   }*/
}

#define _initTbl(tbl,size,type, numInputs,numOutputs)\
{\
   unsigned int k;\
   for(k = 0; k  < size; k++)\
   {\
       initBlk(&(tbl[k].blk),type, k, numInputs,numOutputs);\
   }\
}

#define _deInitTbl(tbl,size)\
{\
   unsigned int k;\
   for(k = 0; k  < size; k++)\
   {\
      deInitBlk(&(tbl[k].blk));\
   }\
}


static void initGenTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->gens, NU4K_NUM_GENS, NUCFG_BLK_GEN_E, 0, 1);
}

static void deInitGenTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->gens, NU4K_NUM_GENS);
}

static void initSensorTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->sensors, NU4K_NUM_SENSORS, NUCFG_BLK_SEN_E, 0, 1);
}

static void deInitSensorTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->sensors, NU4K_NUM_SENSORS);
}

static void initMediatorTbl(nu4kT *nu4k)
{
    _initTbl(nu4k->mediators, NU4K_NUM_MEDIATORS, NUCFG_BLK_MEDIATOR_E, 2, 1);
}

static void deInitMediatorTbl(nu4kT *nu4k)
{
    _deInitTbl(nu4k->mediators, NU4K_NUM_MEDIATORS);
}

static void initWriterTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->writers, NU4K_NUM_WRITERS, NUCFG_BLK_AXIWR_E, 0, 2);
}

static void deInitWriterTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->writers, NU4K_NUM_WRITERS);
}

static void initMipiRxTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->mipiRx, NU4K_NUM_MIPI_RX, NUCFG_BLK_MIPI_RX_E, 1, 1);
}

static void deInitMipiRxTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->mipiRx, NU4K_NUM_MIPI_RX);
}

static void initParRxTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->parRx, NU4K_NUM_PAR_RX, NUCFG_BLK_PAR_RX_E, 1, 1);
}

static void deInitParRxTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->parRx, NU4K_NUM_PAR_RX);
}

static void initReaderTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->readers, NU4K_NUM_AXI_READERS, NUCFG_BLK_AXIRD_E,  1, 1);
}

static void deInitReaderTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->readers, NU4K_NUM_AXI_READERS);
}

static void initCvaReaderTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->cvaReaders, NU4K_NUM_CVA_READERS, NUCFG_BLK_CVARD_E,  1,/*1*/ 0);
}

static void deInitCvaReaderTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->cvaReaders, NU4K_NUM_CVA_READERS);
}

static void initCvaTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->cva,NU4K_NUM_CVAS, NUCFG_BLK_CVA_E, 1, 1);
}

static void deInitCvaTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->cva,NU4K_NUM_CVAS);
}

static void initInterleaverTbl(nu4kT *nu4k)
{
   unsigned int i;
   _initTbl(nu4k->inters, NU4K_NUM_AXI_INTERLEAVERS, NUCFG_BLK_INTERLEAVER_E, NU4K_INTER_NUM_INPUTS, 1);

   for(i = 0 ; i < NU4K_NUM_AXI_INTERLEAVERS ;i++)
   {
      nu4k->inters[i].intMode = 0;
      getDbField(nu4k,NUFLD_AXIRD_E, i, NU4100_PPE_AXI_READ0_CTRL_INT_MODE_E, (UINT32 *)&nu4k->inters[i].intMode);
   }
}

static void deInitInterleaverTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->inters, NU4K_NUM_AXI_INTERLEAVERS);
}

static void initSluTbl(nu4kT *nu4k)
{
    _initTbl(nu4k->slusParallel, NU4K_NUM_SLUS_PARALLEL, NUCFG_BLK_SLU_PARALLEL_E, 1, 1);
    _initTbl(nu4k->slus, NU4K_NUM_SLUS, NUCFG_BLK_SLU_E, 2, 1);

    //set as default SLU ratio to 1:1. when IAU taking from SLU, we will reset SLU bit on spare register for ratio of 1:2
    //we need always use the worst case, if some path requires 2:1, then all should
    XMLDB_setValue(nu4k->db, (XMLDB_pathE)NU4100_IAE_SPARE_RW0_SPARE_E, 0x3F);
}

static void deInitSluTbl(nu4kT *nu4k)
{
    _deInitTbl(nu4k->slusParallel, NU4K_NUM_SLUS_PARALLEL);
    _deInitTbl(nu4k->slus, NU4K_NUM_SLUS);
}

static void initIspTbls(nu4kT *nu4k)
{
   _initTbl(nu4k->isps, NU4K_NUM_ISPS, NUCFG_BLK_ISP_E, 2, 3);//input should be 4, exp merge. next step
}

static void deInitIspTbls(nu4kT *nu4k)
{
   _deInitTbl(nu4k->isps, NU4K_NUM_ISPS);
}

static void initIauTbls(nu4kT *nu4k)
{
   _initTbl(nu4k->iaus, NU4K_NUM_IAUS, NUCFG_BLK_IAU_E, 1, 1);
   _initTbl(nu4k->iausColor, NU4K_NUM_IAUS_COLOR, NUCFG_BLK_IAU_COLOR_E, 1, 1);
}

static void deInitIauTbls(nu4kT *nu4k)
{
   _deInitTbl(nu4k->iaus, NU4K_NUM_IAUS);
   _deInitTbl(nu4k->iausColor, NU4K_NUM_IAUS_COLOR);
}

static void initHistTbls(nu4kT *nu4k)
{
   _initTbl(nu4k->hist, NU4K_NUM_HIST, NUCFG_BLK_HIST_E, 1, 1);
}

static void deInitHistTbls(nu4kT *nu4k)
{
   _deInitTbl(nu4k->hist,NU4K_NUM_HIST);
}

static void initPpuTbls(nu4kT *nu4k)
{
   _initTbl(nu4k->ppusNoScl, NU4K_NUM_PPUS_NOSCL, NUCFG_BLK_PPU_NOSCL_E, NU4K_PPU_NUM_INPUTS, NU4K_PPU_NUM_OUTPUTS);
   _initTbl(nu4k->ppusScl, NU4K_NUM_PPUS_SCL, NUCFG_BLK_PPU_SCL_E, NU4K_PPU_NUM_INPUTS, NU4K_PPU_NUM_OUTPUTS);
   _initTbl(nu4k->ppusHybScl, NU4K_NUM_PPUS_HYBSCL, NUCFG_BLK_PPU_HYBSCL_E, NU4K_PPU_HYB_NUM_INPUTS, NU4K_PPU_NUM_OUTPUTS);
   _initTbl(nu4k->ppusHyb, NU4K_NUM_PPUS_HYB, NUCFG_BLK_PPU_HYB_E, NU4K_PPU_HYB_NUM_INPUTS, NU4K_PPU_NUM_OUTPUTS);
}

static void deInitPpuTbls(nu4kT *nu4k)
{
   _deInitTbl(nu4k->ppusNoScl, NU4K_NUM_PPUS_NOSCL);
   _deInitTbl(nu4k->ppusScl, NU4K_NUM_PPUS_SCL);
   _deInitTbl(nu4k->ppusHybScl, NU4K_NUM_PPUS_HYBSCL);
   _deInitTbl(nu4k->ppusHyb, NU4K_NUM_PPUS_HYB);
}

static void initDpeTbl(nu4kT *nu4k)
{
   //Note: we treat each DPE as a single dpe blocks 2 inputs L/R
   _initTbl(nu4k->dpe, NU4K_NUM_DPES, NUCFG_BLK_DPE_E, NU4K_DPE_NUM_INPUTS, 1);
}

static void deInitDpeTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->dpe, NU4K_NUM_DPES);
}

static void initDpeHybTbl(nu4kT *nu4k)
{
   //Note: the hybrid dpe block takes 2 inputs
   _initTbl(nu4k->dpeHyb, NU4K_NUM_DPES_HYB, NUCFG_BLK_DPE_HYB_E, NU4K_DPE_HYB_NUM_INPUTS, 1);
}

static void deInitDpeHybTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->dpeHyb, NU4K_NUM_DPES_HYB);
}

static void initDppTbl(nu4kT *nu4k)
{
   //Note: we treat single dpp as 2 blocks each with 1 input and 1 output
   _initTbl(nu4k->dpp, NU4K_NUM_DPPS, NUCFG_BLK_DPP_E, 1, 1);
}


static void deInitDppTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->dpp, NU4K_NUM_DPPS);
}

static void initDphyTxTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->dphyTx, NU4K_NUM_DPHY_TX, NUCFG_BLK_DPHY_TX_E, 2, 1);
}


static void deInitDphyTxTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->dphyTx, NU4K_NUM_DPHY_TX);
}

static void initVscCsiTxTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->vscCsiTx, NU4K_NUM_VSC_CSI_TX, NUCFG_BLK_VSC_CSI_TX_E, 1, 1);
}

static void deInitVscCsiTxTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->vscCsiTx, NU4K_NUM_VSC_CSI_TX);
}

static void initIspReaderTbl(nu4kT *nu4k)
{
   _initTbl(nu4k->ispReadersT, NU4K_NUM_ISP_READERS, NUCFG_BLK_ISPRD_E, 1, 1);
}

static void deInitIspReaderTbl(nu4kT *nu4k)
{
   _deInitTbl(nu4k->ispReadersT, NU4K_NUM_ISP_READERS);
}

static void setBlkInOut(nu4kT *nu4k, nu4kblkT *blk)
{
   unsigned int i,j;
   nu4kblkT *src;

   for(i = 0; i < blk->numInputs; i++)
   {
      src = NULL; j = 0;

      getInput(nu4k, blk,i,&src,&j);
      blk->inputs[i].sel   = (UINT8)j;
      blk->inputs[i].blkp  = src;
      if(src)
      {
         assert(src->numOutputs > j);//must have output
         setBlkInOut(nu4k, src);
      }
   }
}

static void initInOut(nu4kT *nu4k)
{
   unsigned int i;
   for(i = 0; i < NU4K_NUM_AXI_INTERLEAVERS;i++)
   {
      nu4kblkT *blk = &nu4k->inters[i].blk;
      setBlkInOut(nu4k, blk);
   }

   for(i = 0; i < NU4K_NUM_HISTOGRAM_READER;i++)
   {
      nu4kblkT *blk = &nu4k->hist[i].blk;
      setBlkInOut(nu4k, blk);
   }

   for (i = 0; i < NU4K_NUM_CVA_READERS; i++)
   {
       nu4kblkT *blk = &nu4k->cvaReaders[i].blk;
       setBlkInOut(nu4k, blk);
   }

   for (i = 0; i < NU4K_NUM_DPHY_TX; i++)
   {
      nu4kblkT *blk = &nu4k->dphyTx[i].blk;
      setBlkInOut(nu4k, blk);
   }

   for (i = 0; i < NU4K_NUM_ISP_READERS; i++)
   {
      nu4kblkT *blk = &nu4k->ispReadersT[i].blk;
      setBlkInOut(nu4k, blk);
   }
}

static void initBlkString(nu4kT *nu4k, nuBlkTypeE type, const char *str)
{
    memcpy(&nu4k->blkStrings[type],str, MIN(NU4K_MAX_BLK_STRLEN, strlen(str)+1));
}

static void initBlkStrings(nu4kT *nu4k)
{
   initBlkString(nu4k, NUCFG_BLK_GEN_E, "gen");
   initBlkString(nu4k, NUCFG_BLK_SEN_E, "sensor");
   initBlkString(nu4k, NUCFG_BLK_MEDIATOR_E, "mediator");
   initBlkString(nu4k, NUCFG_BLK_MIPI_RX_E, "mipi_rx");
   initBlkString(nu4k, NUCFG_BLK_PAR_RX_E, "parallel_rx");
   initBlkString(nu4k, NUCFG_BLK_SLU_PARALLEL_E, "slu_parallel");
   initBlkString(nu4k, NUCFG_BLK_SLU_E, "slu");
   initBlkString(nu4k, NUCFG_BLK_IAU_E, "iau");
   initBlkString(nu4k, NUCFG_BLK_IAU_COLOR_E, "iau_color");
   initBlkString(nu4k, NUCFG_BLK_HIST_E, "hist");
   initBlkString(nu4k, NUCFG_BLK_PPU_NOSCL_E, "ppu_noscl");
   initBlkString(nu4k, NUCFG_BLK_PPU_SCL_E, "ppu_scl");
   initBlkString(nu4k, NUCFG_BLK_PPU_HYBSCL_E, "ppu_hybscl");
   initBlkString(nu4k, NUCFG_BLK_PPU_HYB_E, "ppu_hyb");
   initBlkString(nu4k, NUCFG_BLK_AXIRD_E, "axird");
   initBlkString(nu4k, NUCFG_BLK_CVARD_E, "cvard");
   initBlkString(nu4k, NUCFG_BLK_AXIWR_E, "axiwr");
   initBlkString(nu4k, NUCFG_BLK_DPE_E, "dpe");
   initBlkString(nu4k, NUCFG_BLK_CVA_E, "cva");
   initBlkString(nu4k, NUCFG_BLK_DPE_HYB_E, "dpe_hyb");
   initBlkString(nu4k, NUCFG_BLK_DPP_E, "dpp");
   initBlkString(nu4k, NUCFG_BLK_INTERLEAVER_E, "axi_inter");
   initBlkString(nu4k, NUCFG_BLK_DPHY_TX_E, "dphy_tx");
   initBlkString(nu4k, NUCFG_BLK_VSC_CSI_TX_E, "vsc_csi_tx");
   initBlkString(nu4k, NUCFG_BLK_ISPRD_E, "isp_rd");
}

static void initFormatTbls()
{
   initWriterFormatTbl();
   initPpuFormatTbl();
   initCsiFormatTbl();
   initIauFormatTbl();
   initSluFormatTbl();
   initDpeFormatTbl();
}

static void initPpuOffsetTbls()
{
   ppuCropTblsInit();
   ppuSclTblsInit();
   ppuStitchTblsInit();
   ppuRepackTblsInit();
   ppuCscTblsInit();
}

static void initBlkName()
{
   blkName[NUCFG_BLK_GEN_E] = "NU4100_iae_gen";
   blkName[NUCFG_BLK_SEN_E] = "NU4100_sensor";
   blkName[NUCFG_BLK_MEDIATOR_E] = "NU4100_mediator";
   blkName[NUCFG_BLK_MIPI_RX_E] = "NU4100_bus_mipi";
   blkName[NUCFG_BLK_PAR_RX_E] = "NU4100_bus_parallel";
   blkName[NUCFG_BLK_SLU_PARALLEL_E] = "NU4100_iae_slu";
   blkName[NUCFG_BLK_SLU_E] = "NU4100_iae_slu";
   blkName[NUCFG_BLK_IAU_E] = "NU4100_iae";
   blkName[NUCFG_BLK_IAU_COLOR_E] = "NU4100_iae";
   blkName[NUCFG_BLK_HIST_E] = "NU4100_iae_hist";
   blkName[NUCFG_BLK_PPU_NOSCL_E] = "NU4100_ppu";
   blkName[NUCFG_BLK_PPU_SCL_E] = "NU4100_ppu";
   blkName[NUCFG_BLK_PPU_HYBSCL_E] = "NU4100_ppu";
   blkName[NUCFG_BLK_PPU_HYB_E] = "NU4100_puu";
   blkName[NUCFG_BLK_AXIRD_E] = "NU4100_ppe_axi_reader";
   blkName[NUCFG_BLK_CVARD_E] = "NU4100_ppe_cva_reader";
   blkName[NUCFG_BLK_INTERLEAVER_E] = "NU4100_ppe_axi_reader";
   blkName[NUCFG_BLK_AXIWR_E] = "NU4100_ppe_axi_writer";
   blkName[NUCFG_BLK_DPE_E] = "NU4100_dpe";
   blkName[NUCFG_BLK_CVA_E] = "NU4100_cva";
   blkName[NUCFG_BLK_DPE_HYB_E] = "NU4100_dpe";
   blkName[NUCFG_BLK_DPP_E] = "NU4100_ppe_depth_post";
   blkName[NUCFG_BLK_DPHY_TX_E] = "NU4100_dphy_tx";
   blkName[NUCFG_BLK_VSC_CSI_TX_E] = "NU4100_vsc_csi_tx";
   blkName[NUCFG_BLK_ISP_E] = "NU4100_isp";
   blkName[NUCFG_BLK_ISPRD_E] = "NU4100_isp_reader";
}

ERRG_codeE nu4k_init(XMLDB_dbH db, nu4kH *nu4kH)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   //Init block functions
   nu4kT *nu4k = (nu4kT *)malloc(sizeof(nu4kT));
   memset((void *)nu4k, 0x00, sizeof(nu4kT));
   if(!nu4k)
   {
      *nu4kH = NULL;
      return NUCFG__ERR_OUT_OF_RSRCS;
   }
   nu4k->db = db;

   //Init tables
   initFormatTbls();
   initPpuOffsetTbls();
   initBlkName();
   initMetaStartPathTbl();

   initGenTbl(nu4k);
   initSensorTbl(nu4k);
   initMediatorTbl(nu4k);
   initWriterTbl(nu4k);
   initMipiRxTbl(nu4k);
   initParRxTbl(nu4k);
   initReaderTbl(nu4k);
   initCvaReaderTbl(nu4k);
   initInterleaverTbl(nu4k);
   initSluTbl(nu4k);
   initIspTbls(nu4k);
   initIauTbls(nu4k);
   initHistTbls(nu4k);
   initPpuTbls(nu4k);
   initDpeTbl(nu4k);
   initDpeHybTbl(nu4k);
   initDppTbl(nu4k);
   initCvaTbl(nu4k);
   initVscCsiTxTbl(nu4k);
   initDphyTxTbl(nu4k);
   initIspReaderTbl(nu4k);

   //Init input/outputs
   initInOut(nu4k);
   initBlkStrings(nu4k);

   *nu4kH = nu4k;
   return ret;
}

void nu4k_deInit(nu4kH h)
{
   nu4kT *nu4k = (nu4kT *)h;

   //free up allocated tables memory
   deInitGenTbl(nu4k);
   deInitSensorTbl(nu4k);
   deInitMediatorTbl(nu4k);
   deInitWriterTbl(nu4k);
   deInitMipiRxTbl(nu4k);
   deInitParRxTbl(nu4k);
   deInitReaderTbl(nu4k);
   deInitCvaReaderTbl(nu4k);
   deInitInterleaverTbl(nu4k);
   deInitSluTbl(nu4k);
   deInitIspTbls(nu4k);
   deInitIauTbls(nu4k);
   deInitHistTbls(nu4k);
   deInitPpuTbls(nu4k);
   deInitDpeTbl(nu4k);
   deInitDpeHybTbl(nu4k);
   deInitDppTbl(nu4k);
   deInitCvaTbl(nu4k);
   deInitVscCsiTxTbl(nu4k);
   deInitDphyTxTbl(nu4k);
   deInitIspReaderTbl(nu4k);

   //free nu4k
   free(nu4k);
}

unsigned int nu4k_getNumOutputs(nu4kH h)
{
   (void)h;
   return NU4K_NUM_OUTPUTS;
}

nublkH nu4k_getOutput(nu4kH h,unsigned int inst)
{
   nu4kT *nu4k = (nu4kT *)h;
   if(inst < NU4K_NUM_AXI_INTERLEAVERS)
      return &nu4k->inters[inst].blk;

   if(inst < (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HIST))
      return &nu4k->hist[inst - NU4K_NUM_AXI_INTERLEAVERS].blk;

   if(inst < (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HIST + NU4K_NUM_CVA_READERS))
      return &nu4k->cvaReaders[inst - (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HIST)].blk;

   if (inst < NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HIST + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX)
      return &nu4k->dphyTx[inst - (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HIST + NU4K_NUM_CVA_READERS)].blk;

   if (inst < NU4K_NUM_OUTPUTS)
       return &nu4k->ispReadersT[inst - (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HIST + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX)].blk;

   return NULL;
}

unsigned int nu4k_getNumInputs(nu4kH h)
{
   (void)h;
   return NU4K_NUM_INPUTS;
}

nublkH nu4k_getInput(nu4kH h, unsigned int inst)
{
   nu4kT *nu4k = (nu4kT *)h;
   if(inst < NU4K_NUM_SENSORS)
      return &nu4k->sensors[inst].blk;
   if((inst > NU4K_NUM_SENSORS) && (inst < NU4K_NUM_INPUTS))
      return &nu4k->writers[inst].blk;

   return NULL;
}

void nu4k_getBlkTypeInst(nu4kH h,nublkH blk, nuBlkTypeE *type, unsigned int *inst)
{
   (void)h;
   if(type)
      *type = ((nu4kblkT *)blk)->ti.type;
   if(inst)
      *inst = ((nu4kblkT *)blk)->ti.inst;
}

void nu4k_getChHwId(nu4kH h, nublkH blk, char *buf)
{
    (void)h;
    nuBlkTypeE blkType;
    unsigned int blkInst;

    nu4k_getBlkTypeInst(h, blk, &blkType, &blkInst);
    snprintf(buf, NUCFG_MAX_HW_ID_SIZE , "%s%u", blkName[blkType], blkInst);
}

NUCFG_hwId nu4k_getBlkHwName(nu4kH h,nublkH blk)
{
   (void)h;
   return ((nu4kblkT *)blk)->hwId;
}

unsigned int nu4k_getBlkNumInputs(nu4kH h,nublkH blk)
{
   (void)h;
   return ((nu4kblkT *)blk)->numInputs;
}
void nu4k_getBlkInput(nu4kH h,nublkH blk0, unsigned int blk0_index, nublkH *blk1, unsigned int *blk1_index)
{
   (void)h;
   nu4kblkT *blk0_p = (nu4kblkT *)blk0;
   *blk1 = blk0_p->inputs[blk0_index].blkp;
   *blk1_index = blk0_p->inputs[blk0_index].sel;
}
unsigned int nu4k_getBlkNumOutputs(nu4kH h,nublkH blk)
{
   (void)h;
   return ((nu4kblkT *)blk)->numOutputs;
}
void nu4k_getBlkOutputRes(nu4kH h, nublkH blk, unsigned int outNum, NUCFG_resT *outres)
{
   nu4kT *nu4k = (nu4kT *)h;
   unsigned int horz = 0,vert = 0, bpp = 0;
   getOutputRes(nu4k,(nu4kblkT *)blk, outNum, &horz, &vert);
   bpp = getOutputBpp(nu4k,(nu4kblkT *)blk, outNum);
   outres->height = (UINT16)vert;
   outres->width = (UINT16)horz;
   outres->bpp = (UINT16)bpp;
}

NUCFG_formatE nu4k_getBlkOutputFormat(nu4kH h, nublkH blk, unsigned int outNum)
{
   nu4kT *nu4k = (nu4kT *)h;
   return getOutputFormat(nu4k,(nu4kblkT *)blk, outNum);
}

NUCFG_formatDiscriptorU nu4k_getBlkOutputFormatDiscriptor(nu4kH h, nublkH blk, unsigned int outNum, NUCFG_formatDiscriptorU *fmtDisc)
{
   nu4kT *nu4k = (nu4kT *)h;
   return getOutputFormatDiscriptor(nu4k,(nu4kblkT *)blk, outNum, fmtDisc);
}

int nu4k_isInputBlk(nu4kH h, nublkH blk)
{
   (void)h;
   nu4kblkT *blkp = (nu4kblkT *)blk;
   return ((blkp->ti.type == NUCFG_BLK_GEN_E) || (blkp->ti.type == NUCFG_BLK_SEN_E) || (blkp->ti.type == NUCFG_BLK_AXIWR_E));
}

void nu4k_updateBlkOutput(nu4kH h,nublkH blk, unsigned int outNum, nusoc_updateT *params)
{
   updateOutput((nu4kT *)h,(nu4kblkT *)blk, outNum, params);
}

unsigned int nu4k_getBlkStr(nu4kH h, nublkH blk, char *str)
{
   nu4kT *nu4k = (nu4kT *)h;
   nuBlkTypeE type = ((nu4kblkT *)blk)->ti.type;
   unsigned int inst =  ((nu4kblkT *)blk)->ti.inst;

   unsigned int len = (unsigned int)strlen(nu4k->blkStrings[type])+4;
   if(str)
   {
      snprintf(str, len, "%s%d", nu4k->blkStrings[type],inst);
   }
   return len;
}

/*
recursive function that search if therer is a ppu with stitch on path.
if exist and stitch is not on bypass, increase the counter of num interleaved.
*/
static void addInterFromStitch(nu4kH h, nu4kblkT *blkP, UINT32 *countP)
{
   nu4kT *nu4k = (nu4kT *)h;
   UINT32 i, bypass = 2;

   if (blkP->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
   {
      getDbField(nu4k, nusoc_convert2Fld(blkP->ti.type), blkP->ti.inst, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E, &bypass);
   }
   else if (blkP->ti.type == NUCFG_BLK_PPU_HYB_E)
   {
      getDbField(nu4k, nusoc_convert2Fld(blkP->ti.type), blkP->ti.inst, NU4100_PPE_PPU8_STITCH_CFG_BYPASS_E, &bypass);
   }
   if ((bypass == 0) || (bypass == 0xFFFFFFFF))//0xFFFFFFFF empty value on XML --> reset value is 0 not bypass
   {
      (*countP)++;
   }
   if (nu4k_isInputBlk(h, blkP))
   {
      return;
   }
   for (i = 0; i < blkP->numInputs; i++)
   {
      if (!blkP->inputs[i].blkp)
         continue;
      addInterFromStitch(h, blkP->inputs[i].blkp, countP);
   }
}

void nu4k_getInterleaveInfoFromDb(nu4kH h, nublkH blkh, UINT32 *numIntP, NUCFG_interModeE *intMode)
{
   nu4kT *nu4k = (nu4kT *)h;
   nu4kblkT* blk = (nu4kblkT*)blkh;
   UINT32 intStrEn=0, count=0;
   *intMode = NUCFG_INTER_MODE_UNKNOWN_E;

   switch(blk->ti.type)
   {
      case(NUCFG_BLK_INTERLEAVER_E):
      {
         if (blk->ti.inst < NU4K_NUM_AXI_INTERLEAVERS) //interleaved relevant for axi rd only
         {
            getDbField(nu4k, NUFLD_AXIRD_E, blk->ti.inst, NU4100_PPE_AXI_READ0_CTRL_INT_MODE_E, (UINT32*)intMode);
            getDbField(nu4k, NUFLD_AXIRD_E, blk->ti.inst, NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E, &intStrEn);
            while (intStrEn != 0)
            {
             if ((intStrEn & 1) == 1)
             {
               count++;
             }
               intStrEn = intStrEn >> 1;
            }
           addInterFromStitch(h, blk, &count);
         }
         break;
      }

      case(NUCFG_BLK_HIST_E):
      case(NUCFG_BLK_DPHY_TX_E):
      case(NUCFG_BLK_CVARD_E):
      {
         count = 1;
         break;
      }

      case(NUCFG_BLK_ISPRD_E):
      {
         count = 1; // TODO:  Need to implement
         break;
      }

      default:
      {
         assert(0);
      }
   }

   *numIntP = count;
}

static int getHybPath(nu4kT *nu4k, nu4kblkT* blk, nu4kblkT* prevBlk)
{
   UINT32 srcSelect = 0;
   int ret = -1; //init to catch issues with the code
   nu4kblkT *_input = NULL;
   unsigned int _in_index = 0;

   //compare src select of misc0 to input block
   if (blk->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
   {
      getDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, NU4100_PPE_PPU7_MISC0_PPU_CTRL_SRC_SEL_E, &srcSelect);
      parseDataSel(nu4k, (UINT8)(srcSelect & 0xFF), &_input, &_in_index);
      if (NU4K_IS_SAME_INSTANCE_AND_TYPE(_input, prevBlk))
      {
         ret = 0;
      }
      else
      {
         getDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, NU4100_PPE_PPU7_MISC1_PPU_CTRL_SRC_SEL_E, &srcSelect);
         parseDataSel(nu4k, (UINT8)(srcSelect & 0xFF), &_input, &_in_index);
         if (NU4K_IS_SAME_INSTANCE_AND_TYPE(_input, prevBlk))
         {
            ret = 1;
         }
      }
   }

   if (blk->ti.type == NUCFG_BLK_PPU_HYB_E)
   {
      getDbField(nu4k, NUFLD_PPU_HYB_E, blk->ti.inst, NU4100_PPE_PPU8_MISC0_PPU_CTRL_SRC_SEL_E, &srcSelect);
      parseDataSel(nu4k, (UINT8)(srcSelect & 0xFF), &_input, &_in_index);
      if (NU4K_IS_SAME_INSTANCE_AND_TYPE(_input, prevBlk))
      {
         ret = 0;
      }
      else
      {
         getDbField(nu4k, NUFLD_PPU_HYB_E, blk->ti.inst, NU4100_PPE_PPU8_MISC1_PPU_CTRL_SRC_SEL_E, &srcSelect);
         parseDataSel(nu4k, (UINT8)(srcSelect & 0xFF), &_input, &_in_index);
         if (NU4K_IS_SAME_INSTANCE_AND_TYPE(_input, prevBlk))
         {
            ret = 1;
         }
      }
   }

   if ((ret != 0) && (ret != 1))
      assert(0);

   return ret;
}

/*
*   setPathInterleave:
*   The function is activated on all the blocks in a specific path.
*   This path is to be removed/added into the channel (by interleaving)
*   there are two levels of interleave in the HW - PPU stitchers, and
*   AXI interleavers.
*   Therefor, the logic is to remove the path from the PPU stitcher if
*   interleaving happens there. If there is no interleaving in the stitcher,
*   then this PPU can be removed from the AXI interleaver.
*   if the path to be removed is misc0 of a stitcher, then bypassing is not
*   enough. all the configuration from misc1 needs to overwrite the misc0.
*
*/
void nu4k_setPathInterleave(nu4kH h, const nublkH blkh, void **valP, int enable)
{
    nu4kT *nu4k = (nu4kT *)h;
    UINT32 streamEn = 0, stitcherBypass = 0;
    nu4kblkT* blk = (nu4kblkT*)blkh;

    if (*valP == NULL)
    {
       //first entry for this path, reset the flags
       memset(&interleaveSetPath,0,sizeof(interleaveSetPath));
       *valP = &interleaveSetPath;
    }

    //The function is activated on the path with this input.
    //we first check stitchers, and bypass if needed.
    //If all paths that goes through a stitcher are disabled, we need to remove from the axi interleaver
    if (blk->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
    {
       int hybPath;
       hybPath = getHybPath(nu4k, blk, interleaveSetPath.prevBlk);
       getDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E, &stitcherBypass);
       interleaveSetPath.pathIsGoingThroughStitch = true;

       if ((hybPath == 1) && (stitcherBypass))
       {
          assert(0);
       }

       if (!stitcherBypass)
       {
          //If this path goes through misc0, then we need to copy misc1 into misc0
          if (!enable)
          {
             if (hybPath == 0)
             {
                int pathIndex;
                UINT32 val;
                for (pathIndex = NU4100_PPE_PPU7_MISC1_CROP_START_X_E; pathIndex < NU4100_PPE_PPU7_SCL0_CFG_SCALE_MODE_E; pathIndex++)
                {
                   getDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, (XMLDB_pathE)pathIndex, &val);
                   setDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, (XMLDB_pathE)(pathIndex - (NU4100_PPE_PPU7_MISC1_CROP_START_X_E - NU4100_PPE_PPU7_MISC0_CROP_START_X_E)), val);
                }
                for (pathIndex = NU4100_PPE_PPU7_SCL1_CFG_SCALE_MODE_E; pathIndex < NU4100_PPE_PPU7_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E; pathIndex++)
                {
                   getDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, (XMLDB_pathE)pathIndex, &val);
                   setDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, (XMLDB_pathE)(pathIndex - (NU4100_PPE_PPU7_SCL1_CFG_SCALE_MODE_E - NU4100_PPE_PPU7_SCL0_CFG_SCALE_MODE_E)), val);
                }
             }
             setDbField(nu4k, NUFLD_PPU_HYBSCL_E, blk->ti.inst, NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E, !enable);
          }
       }
       else
       {
          interleaveSetPath.stitcherIsEmpty = true;
       }
    }

    if (blk->ti.type == NUCFG_BLK_PPU_HYB_E)
    {
       int hybPath;
       hybPath = getHybPath(nu4k, blk, interleaveSetPath.prevBlk);
       getDbField(nu4k, NUFLD_PPU_HYB_E, blk->ti.inst, NU4100_PPE_PPU8_STITCH_CFG_BYPASS_E, &stitcherBypass);
       interleaveSetPath.pathIsGoingThroughStitch = true;

       if ((hybPath == 1) && (stitcherBypass))
       {
          assert(0);
       }

       if (!stitcherBypass)
       {
          //If this path goes through misc0, then we need to copy misc1 into misc0
          if (!enable)
          {
             if (hybPath == 0)
             {
                int pathIndex;
                UINT32 val;
                for (pathIndex = NU4100_PPE_PPU8_MISC1_CROP_START_X_E; pathIndex < NU4100_PPE_PPU8_REPACK0_SPLIT_CTRL0_STREAM_VECTOR_E; pathIndex++)
                {
                   getDbField(nu4k, NUFLD_PPU_HYB_E, blk->ti.inst, (XMLDB_pathE)pathIndex, &val);
                   setDbField(nu4k, NUFLD_PPU_HYB_E, blk->ti.inst, (XMLDB_pathE)(pathIndex - (NU4100_PPE_PPU8_MISC1_CROP_START_X_E - NU4100_PPE_PPU8_MISC0_CROP_START_X_E)), val);
                }
             }
             setDbField(nu4k, NUFLD_PPU_HYB_E, blk->ti.inst, NU4100_PPE_PPU8_STITCH_CFG_BYPASS_E, !enable);
          }
       }
       else
       {
          interleaveSetPath.stitcherIsEmpty = true;
       }
    }

    if (blk->ti.type == NUCFG_BLK_INTERLEAVER_E)
    {
        if ((!interleaveSetPath.pathIsGoingThroughStitch) || (interleaveSetPath.stitcherIsEmpty))
        {
           getDbField(nu4k, NUFLD_AXIRD_E, blk->ti.inst, NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E, &streamEn);
           streamEn = (enable) ? streamEn | (1 << (interleaveSetPath.prevBlk->ti.inst % 4)) : streamEn & ~(1 << (interleaveSetPath.prevBlk->ti.inst % 4));
           setDbField(nu4k, NUFLD_AXIRD_E, blk->ti.inst, NU4100_PPE_AXI_READ0_CTRL_INT_STRM_EN_E, streamEn);
        }
    }
    interleaveSetPath.prevBlk = blk;
}


XMLDB_pathE nu4k_getXmlDbPath(nu4kH h, nuBlkTypeE type)
{
   (void)h;
   return nucfgMetaStartPathTbl[type];
}

/*
   Recursive function, search for blk with scaler on all pathes in the channle.
   if exist, set scale sizes on xml db
*/
void nu4k_updateBlkSclSize(nu4kH h, nublkH blkh, UINT32 hSize, UINT32 vSize, UINT32 *numOfPpuSclOnPath)
{
   unsigned int i;
   nu4kblkT* blk = (nu4kblkT*)blkh;
   nu4kT *nu4k = (nu4kT *)h;
   NUCFG_formatE inFmt;
   UINT32 isCscBypass, cscMode;

   if (blk->ti.type == NUCFG_BLK_PPU_SCL_E)
   {
      inFmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU4_SCL_CFG_SCALE_MODE_E, 3); //tbd from API
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU4_SCL_OUT_SIZE_HSIZE_E, hSize);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU4_SCL_OUT_SIZE_VSIZE_E, vSize);

      getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU4_MISC_PPU_CTRL_CSC_BYPASS_E, &isCscBypass);
      if (!isCscBypass)
      {
          getDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU4_MISC_PPU_CTRL_CSC_MODE_E, &cscMode);
          if (cscMode == 0)//rgb to yuv422
          {
              inFmt = NUCFG_FORMAT_YUV420_10BIT_E;
          }
          else if (cscMode == 1)//yuv422 to rgb
          {
              inFmt = NUCFG_FORMAT_RGB888_E;
          }

      }
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU4_SCL_CFG_DATA_FORMAT_E, nucfgFmt2ppu[inFmt]);
      (*(numOfPpuSclOnPath))++;
     LOGG_PRINT(LOG_INFO_E, NULL, "scaler config on blk type %s instance %d\n", getBlkName(blk->ti.type), blk->ti.inst);
   }
   else if (blk->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
   {
        /*
      //todo: we do not support hybrid scale (p0 only is ok, but using p0 and p1 is not supported). In the future, we will
      //want to extand the API to allow user to set hybrid scaler.
      inFmt = getOutputFormat(nu4k, blk->inputs[0].blkp, blk->inputs[0].sel); //todo hybrid scale
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL0_CFG_SCALE_MODE_E, 3); //tbd from API
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL0_CFG_DATA_FORMAT_E, nucfgFmt2ppu[inFmt]);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL0_OUT_SIZE_HSIZE_E, hSize);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL0_OUT_SIZE_VSIZE_E, vSize);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL1_CFG_SCALE_MODE_E, 3); //tbd from API
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL1_CFG_DATA_FORMAT_E, nucfgFmt2ppu[inFmt]);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL1_OUT_SIZE_HSIZE_E, hSize);
      setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), blk->ti.inst, NU4100_PPE_PPU7_SCL1_OUT_SIZE_VSIZE_E, vSize);
      (*(numOfPpuSclOnPath))++;
     LOGG_PRINT(LOG_INFO_E, NULL, "scaler config on blk type %s instance %d\n", getBlkName(blk->ti.type), blk->ti.inst);
      */
        LOGG_PRINT(LOG_INFO_E, NULL, "SKIP scaler config on PPU7 (HW issue)\n");
   }
   else if (blk->ti.type == NUCFG_BLK_DPP_E)
   {
      if (blk->ti.inst == 0)
      {
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_PPE_DEPTH_POST_SCL_HOST_CFG_SCALE_MODE_E, NU4K_DPP_SCL_MODE_HORZ_VERT);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_PPE_DEPTH_POST_SCL_HOST_OUT_SIZE_HSIZE_E, hSize);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_PPE_DEPTH_POST_SCL_HOST_OUT_SIZE_VSIZE_E, vSize);
      }
      else
      {
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_PPE_DEPTH_POST_SCL_CVA_CFG_SCALE_MODE_E, NU4K_DPP_SCL_MODE_HORZ_VERT);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_PPE_DEPTH_POST_SCL_CVA_OUT_SIZE_HSIZE_E, hSize);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_PPE_DEPTH_POST_SCL_CVA_OUT_SIZE_VSIZE_E, vSize);
      }
      (*(numOfPpuSclOnPath))++;
      LOGG_PRINT(LOG_INFO_E, NULL, "scaler config on blk type %s instance %d\n", getBlkName(blk->ti.type), blk->ti.inst);
   }
   else if (blk->ti.type == NUCFG_BLK_DPE_E)
   {
      //dont do scaling in DPE if already done on DPP
      if (!(*numOfPpuSclOnPath))
      {
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_DPE_DIF_LEFT_P0_CFG_SCALE_MODE_E, NU4K_DPP_SCL_MODE_HORZ_VERT);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_DPE_DIF_RIGHT_P0_CFG_SCALE_MODE_E, NU4K_DPP_SCL_MODE_HORZ_VERT);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_DPE_DIF_LEFT_P0_HSCALE_CFG1_HSIZE_E, hSize);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_DPE_DIF_RIGHT_P0_HSCALE_CFG1_HSIZE_E, hSize);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_DPE_DIF_LEFT_P0_VSCALE_CFG1_VSIZE_E, vSize);
         setDbField(nu4k, nusoc_convert2Fld(blk->ti.type), 0, NU4100_DPE_DIF_RIGHT_P0_VSCALE_CFG1_VSIZE_E, vSize);

         (*(numOfPpuSclOnPath))++;
         LOGG_PRINT(LOG_INFO_E, NULL, "scaler config on blk type %s instance %d\n", getBlkName(blk->ti.type), blk->ti.inst);
      }
   }
   if (nu4k_isInputBlk(h, blk))
   {
      return;
   }
   for (i = 0; i < blk->numInputs; i++)
   {
      if (!blk->inputs[i].blkp)
         continue;
      nu4k_updateBlkSclSize(h, blk->inputs[i].blkp, hSize, vSize, numOfPpuSclOnPath);
   }
}

ERRG_codeE nu4k_setBlkCropSize(nu4kH h, nublkH blk, unsigned int outNum, NUCFG_resT *res)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   unsigned int i, blkFound = 0;
   nu4kblkT* blkp = (nu4kblkT*)blk;
   nu4kT *nu4k = (nu4kT *)h;

   if (blkp->ti.type == NUCFG_BLK_PPU_NOSCL_E)
   {
      memcpy((void *)&nu4k->ppusNoScl[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
      blkFound = 1;
   }
   else if (blkp->ti.type == NUCFG_BLK_PPU_SCL_E)
   {
      memcpy((void *)&nu4k->ppusScl[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
      blkFound = 1;
   }
    else if (blkp->ti.type == NUCFG_BLK_SLU_PARALLEL_E)
   {
      memcpy((void *)&nu4k->slusParallel[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
      blkFound = 1;
   }
       else if (blkp->ti.type == NUCFG_BLK_SLU_E)
   {
      memcpy((void *)&nu4k->slus[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
      blkFound = 1;
   }
   else if (blkp->ti.type == NUCFG_BLK_PPU_HYB_E)
   {
      if (blkp->inputs[0].blkp->ti.type == NUCFG_BLK_PPU_SCL_E)
      {
         //ppu45 -- > ppu 8 9 10. scl+crop in 4/5
      }
      else
      {
         memcpy((void *)&nu4k->ppusHyb[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
         blkFound = 1;
      }
   }

   else if (blkp->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
   {
      memcpy((void *)&nu4k->ppusHybScl[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
      blkFound = 1;
   }

   if (blkp->ti.type == NUCFG_BLK_DPP_E)
   {
      memcpy((void *)&nu4k->dpp[blkp->ti.inst].userCrop, (void *)res, sizeof(NUCFG_resT));
      blkFound = 1;
   }


   if (nu4k_isInputBlk(h, blk) || (blkFound == 1))
   {
      return ret;
   }
   for (i = 0; i < blkp->numInputs; i++)
   {
      if (!blkp->inputs[i].blkp)
        continue;
      nu4k_setBlkCropSize(h, blkp->inputs[i].blkp, outNum, res);
   }

   if(!blkFound)
   {
      ret = NUCFG__ERR_NOT_SUPPORTED;
   }

   return ret;
}

ERRG_codeE nu4k_setChannelFormat(nu4kH h, nublkH blk, UINT32 channel, NUCFG_changeFromatE reqFormat)
{

   ERRG_codeE ret = NUCFG__RET_SUCCESS;
   unsigned int i;
   nu4kT *nu4k = (nu4kT *)h;
   nu4kblkT* blkp = (nu4kblkT*)blk;
   XMLDB_pathE *tbl, stitchBypass = NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E, *rpk_tbl;
   UINT32 stitchBypassVal, cscBypass, numSet = 1;

   if (NU4K_IS_PPU_BLK(blkp->ti.type))
   {
      tbl = ppuCscTblSelect(blkp->ti.type, 0);
      if (tbl)
      {
         //check if p1 is enabled, for setting csc both, p0&P1
         if (NU4K_IS_HYB_PPU_BLK(blkp->ti.type))
         {
            if (blkp->ti.type == NUCFG_BLK_PPU_HYBSCL_E)
               stitchBypass = NU4100_PPE_PPU7_STITCH_CFG_BYPASS_E;
            else if (blkp->ti.type == NUCFG_BLK_PPU_HYB_E)
               stitchBypass = NU4100_PPE_PPU8_STITCH_CFG_BYPASS_E;
            else
               assert(0);

            ret = getDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, stitchBypass, &stitchBypassVal);
            if (!stitchBypassVal)
               numSet = 2;
         }
         for (i = 0; i < numSet; i++)
         {
            //HW option is convert YUV<-->RGB. in addition, we are using CSC to shift bits to the LSB
            //for this case, we will set CSC to be in non bypass and config unique matrix coefficients (a11=4, a12=0x400, others 0's).
            if (reqFormat == NUCFG_CHANGE_FORMAT_TO_RAW10_E)//the current format is GRAY16, we have verified it before in the calling function
            {
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_MODE], 0);

               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A11], 4);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A12], 0x400);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A13], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_B1], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A21], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A22], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A23], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_B2], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A31], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A32], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A33], 0);
               setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_B3], 0);

            }
            else if (reqFormat == NUCFG_CHANGE_FORMAT_TO_GRAY16_E)//the current format is RAW10, we have verified it before in the calling function
            {
               getDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], &cscBypass);
               if (!cscBypass)
               {
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], 1);
               }
               else
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "Illegal format convert from RAW10 to GRAY16, CSC is not set\n");
               }
            }
            else if (reqFormat == NUCFG_CHANGE_FORMAT_TO_YUV_E)//we protect that the current format is RGB, we have verified it before in the calling function
            {
              //in this case we can just to put csc in bypass if it enabaled
              //TODO: allow config convert from pure RGB which arrive from camera by csc in mode 0(YUV to RGB)
               getDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], &cscBypass);
               if (!cscBypass)
               {
                  //set CSC to bypass
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], 1);

                  //config axi reader to 16 bpp
                  setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_AXIRD_E), channel, NU4100_PPE_AXI_READ0_CTRL_PXL_INT_WIDTH_E, 2);
                  setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_AXIRD_E),channel, NU4100_PPE_AXI_READ0_CTRL_PXL_DATA_WIDTH_E, 2);

                  //set repacker to build YUV of 16bit instead of 24bit. 8y and 8u/v
                  //it cames from 4000B0, to reduce USB traffic
                  //TODO: remove when back to regular YUV
                  rpk_tbl = ppuRepackTblSelect(blkp->ti.type,i);

                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK_BYPASS], 0);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL0_STREAM_VECTOR], 1);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL0_SHIFT], 2);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL0_MODE], 3);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_MRG_REP_RATE], 0);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_MRG_MSK], 3);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_MRG_EN], 0x11);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_SHIFT_1_0], 8);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL1_MSK], 0x3fc);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK0_OUT_BPP], 2);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK0_OUT_FMT], 10);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL0_STREAM_VECTOR], 1);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL0_SHIFT], 14);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL0_MODE], 3);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL1_MASK], 0x3fc000);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK1_OUT_BPP], 2);
                  setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK1_OUT_FMT], 10);

               }
               else
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "Unsupported format convert from RGB to YUV\n");//TODO:activate HW convert from RGB to YUV
               }

            }
            else if (reqFormat == NUCFG_CHANGE_FORMAT_TO_RGB_E)//the current format is YUV, we have verified it before in the calling function
            {
               if (NU4K_IS_PPU_24BIT_OUTPUT(blkp))
               {
                  getDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], &cscBypass);
                  if (cscBypass)
                  {
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_BYPASS], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_MODE], 1);

                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A11], 0x40);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A12], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A13], 0x5A);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_B1], 0xE98B);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A21], 0x40);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A22], 0xFFEA);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A23], 0xFFD3);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_B2], 0x110F);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A31], 0x40);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A32], 0x71);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_A33], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, tbl[CSC_B3], 0xE39A);

                     //config axi reader to 24 bpp
                     setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_AXIRD_E), channel, NU4100_PPE_AXI_READ0_CTRL_PXL_INT_WIDTH_E, 3);
                     setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_AXIRD_E), channel, NU4100_PPE_AXI_READ0_CTRL_PXL_DATA_WIDTH_E, 3);

                     //repacker bypass (which configured for YUV in 16bit)
                     rpk_tbl = ppuRepackTblSelect(blkp->ti.type, i);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK_BYPASS], 1);

                     /*setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL0_STREAM_VECTOR], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL0_SHIFT], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL0_MODE], 3);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_MRG_REP_RATE], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_MRG_MSK], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_MRG_EN], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_SHIFT_1_0], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK0_CTRL1_MSK], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK0_OUT_BPP], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK0_OUT_FMT], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL0_STREAM_VECTOR], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL0_SHIFT], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL0_MODE], 3);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[RPK1_CTRL1_MASK], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK1_OUT_BPP], 0);
                     setDbField(nu4k, nusoc_convert2Fld(blkp->ti.type), blkp->ti.inst, rpk_tbl[REPACK1_OUT_FMT], 0);*/

                  }
                  else
                  {
                     ///unused case
                  }
               }
               else
               {
                  LOGG_PRINT(LOG_ERROR_E, NULL, "Illegal format convert. PPU type %d inst %d on path unsupport 24 bit, required for RGB output\n", blkp->ti.type, blkp->ti.inst);
                  return NUCFG__ERR_PPU_UNSUPPORT_24BIT_OUTPUT;
               }
            }

            tbl = ppuCscTblSelect(blkp->ti.type, 1);//if numSet > 1 it means that we have to config p0&p1, change tbl and run the second loop
         }
      }
   }

   if (nu4k_isInputBlk(h, blk))
   {
      return ret;
   }
   for (i = 0; i < blkp->numInputs; i++)
   {
      if (!blkp->inputs[i].blkp)
        continue;
      ret = nu4k_setChannelFormat(h, blkp->inputs[i].blkp, channel, reqFormat);
   }
   return ret;
}

ERRG_codeE nu4k_setIauBypass(nu4kH h, nublkH blk, UINT32 *numIaus)
{
   ERRG_codeE ret = NUCFG__RET_SUCCESS;

   unsigned int i;
   nu4kblkT* blkp = (nu4kblkT*)blk;
   nu4kT *nu4k = (nu4kT *)h;

   if (blkp->ti.type == NUCFG_BLK_IAU_E)
   {
      if (blkp->ti.inst == 0)
      {
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_YUV0_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_IB0_E,  1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_WB0_E,  1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_DMS0_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_GCR0_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_CSC0_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_DSR0_E, 1);
      }
      else
      {
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_YUV1_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_IB1_E,  1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_WB1_E,  1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_DMS1_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_GCR1_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_CSC1_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_DSR1_E, 1);
      }
      (*numIaus)++;
   }
   else if (blkp->ti.type == NUCFG_BLK_IAU_COLOR_E)
   {
      if (blkp->ti.inst == 0)
      {
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_YUV2_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_IB2_E,  1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_CSC2_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_DSR2_E, 1);
      }
      else
      {
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_YUV3_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_IB3_E,  1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_CSC3_E, 1);
         setDbField(nu4k, nusoc_convert2Fld(NUCFG_BLK_IAU_E), 0, NU4100_IAE_BYPASS_DSR3_E, 1);
      }
      (*numIaus)++;
   }

   if (nu4k_isInputBlk(h, blk))
   {
      return ret;
   }
   for (i = 0; i < blkp->numInputs; i++)
   {
      if (!blkp->inputs[i].blkp)
        continue;
      nu4k_setIauBypass(h, blkp->inputs[i].blkp, numIaus);
   }
   return ret;
}

ERRG_codeE nu4k_getIspLinkToWriterData(nu4kH h, UINT32 writerId, UINT32* width, UINT32* height, UINT32* sensorId, UINT32* ispRdNum)
{
    ERRG_codeE ret = NUCFG__RET_SUCCESS;

    //XMLDB_pathE path;
    UINT32 val, i;
    //nu4kblkT* mySlu = NULL;
    nu4kT* nu4k = (nu4kT*)h;
    XMLDB_pathE pathLink[6] = { ISPS_ISP0_OUTPUTS_MP_LINK_TO_WRITER_E ,
                                ISPS_ISP0_OUTPUTS_SP1_LINK_TO_WRITER_E ,
                                ISPS_ISP0_OUTPUTS_SP2_LINK_TO_WRITER_E,
                                ISPS_ISP1_OUTPUTS_MP_LINK_TO_WRITER_E,
                                ISPS_ISP1_OUTPUTS_SP1_LINK_TO_WRITER_E ,
                                ISPS_ISP1_OUTPUTS_SP2_LINK_TO_WRITER_E };
    XMLDB_pathE pathWidth[6] = { META_ISP_READERS_ISP_RD_0_OUT_RES_WIDTH_E ,
                                META_ISP_READERS_ISP_RD_1_OUT_RES_WIDTH_E ,
                                META_ISP_READERS_ISP_RD_2_OUT_RES_WIDTH_E,
                                META_ISP_READERS_ISP_RD_3_OUT_RES_WIDTH_E,
                                META_ISP_READERS_ISP_RD_4_OUT_RES_WIDTH_E ,
                                META_ISP_READERS_ISP_RD_5_OUT_RES_WIDTH_E };
    XMLDB_pathE pathHeight[6] = { META_ISP_READERS_ISP_RD_0_OUT_RES_HEIGHT_E ,
                                META_ISP_READERS_ISP_RD_1_OUT_RES_HEIGHT_E ,
                                META_ISP_READERS_ISP_RD_2_OUT_RES_HEIGHT_E,
                                META_ISP_READERS_ISP_RD_3_OUT_RES_HEIGHT_E,
                                META_ISP_READERS_ISP_RD_4_OUT_RES_HEIGHT_E ,
                                META_ISP_READERS_ISP_RD_5_OUT_RES_HEIGHT_E };

    for ( i = 0; i < NUCFG_MAX_DB_ISP_META_CHANNELS; i++)
    {
        ret = getDbField(nu4k, NUFLD_ISP_E, 0, pathLink[i], &val);
        if ((val == writerId) && (ret == XMLDB__RET_SUCCESS))
        {
            getDbField(nu4k, NUFLD_ISP_E, 0, pathWidth[i], width);
            getDbField(nu4k, NUFLD_ISP_E, 0, pathHeight[i], height);

            //find the sensor input of ISP channel
            nu4kblkT* mySenBlk;
            mySenBlk = &nu4k->ispReadersT[i].blk;
            while (mySenBlk->inputs)
            {
                mySenBlk = mySenBlk->inputs[0].blkp;
                /*if ((mySenBlk->ti.type == NUCFG_BLK_SLU_PARALLEL_E) || (mySenBlk->ti.type == NUCFG_BLK_SLU_E))
                {
                    mySlu = mySenBlk;
                }*/
            }
            *sensorId = mySenBlk->ti.inst;
            *ispRdNum = i;

            //if (mySlu)
            //{
            //    //updateSlu(nu4k, mySlu, width, 0, height, 0, 1);
            //}

            return NUCFG__RET_SUCCESS;
        }
    }

    return NUCFG__ERR_NOT_SUPPORTED;
}

#ifdef __cplusplus
}
#endif
