/*
xml database module
*/
#ifndef __XML_DB_MODES_H__
#define __XML_DB_MODES_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "err_defs.h"

typedef struct
{
   UINT32   enable;
   UINT32   minDisp;
}XMLModesG_minDispFuncParamsT;

typedef struct
{
   UINT32   enable;
   double   p1DownSampleFactor;
   UINT32   rightEdge;
}XMLModesG_hybridFuncParamsT;

typedef struct
{
   UINT32   enable;
   UINT32   numOfOutputBits;
   UINT32   confLut0;
   UINT32   confLut1;
}XMLModesG_outputBitsFuncParamsT;

typedef struct
{
   UINT32   depthScaleFactorHost;
   UINT32   depthScaleFactorCva;
}XMLModesG_depthParamsT;

typedef struct
{
   UINT32 enable;
   float  SAD_census_ratio;
   int    agg_edge_mode;
   int    agg_win_horizontal;
   int    agg_win_vertical;
   int    cost_clip_high;
   int    cost_clip_low;
   int    census_factor;
   UINT32 p0_p1_mode;
}XMLModesG_aggregationSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 opt_en;
   UINT32 opt_edge_mask_en;
   UINT32 p1;
   UINT32 p2;
   UINT32 p3;
   UINT32 p0_p1_mode;
}XMLModesG_optimizationSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 unite_threshold;
   UINT32 unite_side_left;
   UINT32 p0_p1_mode;
}XMLModesG_uniteSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 small_filter_en;
   UINT32 unite_threshold;
   UINT32 disp_threshold;
   UINT32 min_edges_count;
   UINT32 max_edges_count;
   UINT32 max_win_size;
   UINT32 dil_win_size;
   UINT32 cleaning_disp_threshold;
   UINT32 clean_flt_size;
   UINT32 clean_flt_en;
   UINT32 p0_p1_mode;
}XMLModesG_smallFilterSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 blob_en;
   UINT32 blob_max_size;
   UINT32 blob_disp_diff;
   UINT32 blob_buffer_height;
   UINT32 p0_p1_mode;
}XMLModesG_blobSetupParamsT;

typedef struct
{
   UINT32 enable;
   float gauss_filt_sigma;
   UINT32 filt_before_census_en;
   UINT32 canny_thr_high;
   UINT32 canny_thr_low;
   UINT32 p0_p1_mode;
}XMLModesG_edgeDetectSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 infomap_inv_en;
   float im_t;
   UINT32 box_size;
   UINT32 length;
   UINT32 width;
   UINT32 low_high_var_sel;
   float var_min_offset;
   float var_min_ratio;
   float var_max_offset;
   float var_max_ratio;
   float eps_v;
   float eps_o;
   float eps_a;
   UINT32 close_en;
   UINT32 blob_enable;
   UINT32 max_list_size;
   UINT32 lin_dist;
   UINT32 max_size;
   UINT32 p0_p1_mode;
}XMLModesG_infoMapParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 pxl_cor;
   UINT32 census_msk_disp_mode;
   UINT32 census_cfg_msk_disp_thr;
   UINT32 hmsk;
   UINT32 vmsk;
   UINT32 aggr_msk_disp_mode;
   UINT32 aggr_cfg_msk_disp_thr;
   UINT32 census_flt_en;
   UINT32 weight1;
   UINT32 weight2;
   UINT32 weight3;
   UINT32 weight4;
   UINT32 p0_p1_mode;
}XMLModesG_subpixelSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 plane_fit_en;
   UINT32 pixel_smooth_mode;
   UINT32 grad_disp_thr;
   UINT32 grad_min_pixels;
   UINT32 smooth_disp_thr;
   UINT32 smooth_min_pixels;
   UINT32 p0_p1_mode;
}XMLModesG_planeFitSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 ck;
   UINT32 ca;
   UINT32 cb;
   UINT32 rk;
   UINT32 ra;
   UINT32 rb;
   UINT32 wc0;
   UINT32 wr0;
   UINT32 wn;
   INT32  w_c02;
   INT32  w_c1;
   INT32  w_unite;
   UINT32 p0_p1_mode;
}XMLModesG_confidenceSetupParamsT;

typedef struct
{
   UINT32 enable;
   INT32  bin_1;
   INT32  bin_2;
   INT32  bin_3;
   INT32  bin_4;
   INT32  bin_5;
   INT32  bin_6;
   INT32  bin_7;
   INT32  bin_8;
   INT32  bin_9;
   INT32  bin_10;
   INT32  bin_11;
   INT32  bin_12;
   INT32  bin_13;
   INT32  bin_14;
   INT32  bin_15;
   UINT32 spx_conf_thr_en;
   UINT32 spx_conf_thr;
   UINT32 p0_p1_mode;
}XMLModesG_confidenceBinsSetupParamsT;

typedef struct
{
   UINT32 enable;
   float p0_down_sample_factor;
   float p1_down_sample_factor;
   UINT32 hybrid_full_mode;
   UINT32 min_disp;
   UINT32 ext_rng_thr_far_nv;
   UINT32 ext_rng_thr_far_close;
}XMLModesG_multiResFullImageSetupParamsT;

typedef struct
{
   UINT32 enable;
   UINT32 min_disp;
}XMLModesG_minDisparitySetupParamsT;

// Maximum DPE function instances: 0 default, 1-2 possiable overload function
#define MAX_DPE_FUNCT_INST 3
#define MAX_DPE_FUNCT_HYBRID_INST 2
#define BOTH_P0_P1_ENABLED 2

typedef struct
{
   XMLModesG_hybridFuncParamsT        hybridFuncParams;
   XMLModesG_minDispFuncParamsT       minDispFuncParams;
   XMLModesG_depthParamsT             depthScaleFactor;
   UINT32 numOfAggFunc;
   UINT32 numOfOptimizationFunc;
   UINT32 numOfUniteFunc;
   UINT32 numOfSmallFilterFunc;
   UINT32 numOfBlobFunc;
   UINT32 numOfEdgeDetectFunc;
   UINT32 numOfInfoMapFunc;
   UINT32 numOfSubpixelFunc;
   UINT32 numOfPlaneFitFunc;
   UINT32 numOfConfidenceFunc;
   UINT32 numOfConfidenceBinsFunc;
   UINT32 numOfMultiResFullImageFunc;
   UINT32 numOfminDisparityFunc;
   XMLModesG_aggregationSetupParamsT         aggregationSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_optimizationSetupParamsT        optimizationSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_uniteSetupParamsT               uniteSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_smallFilterSetupParamsT         smallFilterSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_blobSetupParamsT                blobSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_edgeDetectSetupParamsT          edgeDetectSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_infoMapParamsT                  infoMapSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_subpixelSetupParamsT            subpixelSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_planeFitSetupParamsT            planeFitSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_confidenceSetupParamsT          confidenceSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_confidenceBinsSetupParamsT      confidenceBinsSetup[MAX_DPE_FUNCT_INST];
   XMLModesG_multiResFullImageSetupParamsT   multiResFullImageSetup[MAX_DPE_FUNCT_HYBRID_INST];
   XMLModesG_minDisparitySetupParamsT        minDisparitySetup[MAX_DPE_FUNCT_HYBRID_INST];
} XMLModesG_dpeFuncT;

typedef struct _XMLModesDB_dbT *XMLDBModes_dbH;


ERRG_codeE XMLModesG_getFuncData(XMLModesG_dpeFuncT *dpeFunc, const char *buf);
ERRG_codeE XMLModesG_loadFromBuf(XMLDBModes_dbH db, const char *buf);
void * XMLModesG_getParsedXML(const char *socxml_path);


#ifdef __cplusplus
}
#endif
#endif //__XML_DB_MODES_H__
