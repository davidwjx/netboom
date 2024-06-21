/******************************************************************************\
|* Copyright 2010, Dream Chip Technologies GmbH. used with permission by      *|
|* VeriSilicon.                                                               *|
|* Copyright (c) <2020> by VeriSilicon Holdings Co., Ltd. ("VeriSilicon")     *|
|* All Rights Reserved.                                                       *|
|*                                                                            *|
|* The material in this file is confidential and contains trade secrets of    *|
|* of VeriSilicon.  This is proprietary information owned or licensed by      *|
|* VeriSilicon.  No part of this work may be disclosed, reproduced, copied,   *|
|* transmitted, or used in any way for any purpose, without the express       *|
|* written permission of VeriSilicon.                                         *|
|*                                                                            *|
\******************************************************************************/
#ifndef _ISPIC_DEV_H_
#define _ISPIC_DEV_H_

#ifndef __KERNEL__
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define copy_from_user(a, b, c) isp_copy_data(a, b, c)
#define copy_to_user(a, b, c) isp_copy_data(a, b, c)

#include <hal/hal_api.h>

void isp_ic_set_hal(HalHandle_t hal);
#endif

#include "isp_version.h"
#include "isp_ioctl.h"

#define REG_ADDR(x)  ((uint32_t)(uintptr_t)&all_regs->x)

#ifdef ISP_MIV1
#define ISP_BUF_GAP             (1024)
#elif defined ISP_MIV2
#define ISP_BUF_GAP             (0)
#endif

#ifdef ISP_MI_BP
# define MI_PATH_NUM            (3)
#else
# define MI_PATH_NUM            (2)
#endif

struct ic_window {
	u16 x;
	u16 y;
	u16 width;
	u16 height;
};

struct isp_context {
	u32 mode;
	u32 sample_edge;
	bool hSyncLowPolarity, vSyncLowPolarity;
	u32 bayer_pattern;
	u32 sub_sampling;
	u32 seq_ccir;
	u32 field_selection;
	u32 input_selection;
	u32 latency_fifo;
	struct ic_window acqWindow;  /**< acquisition window */
	struct ic_window ofWindow;   /**< output formatter window */
	struct ic_window isWindow;   /**< image stabilization output window */
	u32 bypass_mode;
	u8 demosaic_threshold;
	u32 stitching_mode;
};

typedef struct isp_wdr_context
{
    bool      enabled;
    bool   changed;     //the wdr ctrl && reb shift does not have shandow
                        //register,need to change after frame end irq.
    u16    LumOffset;
    u16    RgbOffset;
    u16    Ym[33];
    u8     dY[33];

} isp_wdr_context_t;

struct isp_digital_gain_cxt {
	/* data */
	bool enable;
	u16 gain_r;
	u16 gain_b;
	u16 gain_gr;
	u16 gain_gb;
	bool changed;
};

struct isp_dummy_hblank_cxt {
	u8 bp, fp, w, in_hsize;
};

enum MIV2_PATH_ID {
	ISP_MI_PATH_MP = 0,
	ISP_MI_PATH_SP,
	ISP_MI_PATH_SP2_BP,
	ISP_MI_MCM_WR0,
	ISP_MI_MCM_WR1,
	ISP_MI_MCM_WR2,
	ISP_MI_MCM_WR3,

#ifdef ISP_MI_PP_WRITE
	ISP_MI_PATH_PP,
#endif

#ifdef ISP_MI_HDR
	ISP_MI_HDR_L,
	ISP_MI_HDR_S,
	ISP_MI_HDR_VS,
#ifdef ISP_HDR_2_SENSORS
	ISP_MI_HDR1_L,
	ISP_MI_HDR1_S,
	ISP_MI_HDR1_VS,
#endif
#endif
	ISP_MI_PATH_ID_MAX
};

struct isp_mi_context {
	struct isp_mi_data_path_context path[ISP_MI_PATH_ID_MAX];
	u32 burst_len;
	bool tileMode;              //indicate whether if mi need to work in tile mode.

};

enum DMA_ID {
	ISP_MI_DMA_ID_MCM_PP = 0,
	ISP_MI_DMA_ID_HDR_L,
	ISP_MI_DMA_ID_HDR_S,
	ISP_MI_DMA_ID_HDR_VS,
#ifdef ISP_HDR_2_SENSORS
	ISP_MI_DMA_ID_HDR1_L,
	ISP_MI_DMA_ID_HDR1_S,
	ISP_MI_DMA_ID_HDR1_VS,
#endif
	ISP_MI_DMA_ID_MAX
};
typedef enum start_dma_path_e{
	ISP_MI_DMA_PATH_MCM_PP = 0,
	ISP_MI_DMA_PATH_HDR,
	ISP_MI_DMA_PATH_MAX,
}start_dma_path_t;

struct isp_bls_context {
	bool enabled;
	u32 mode;
	u16 a, b, c, d;
};

struct isp_tpg_userdefine_mode {
	u16 total, fp, sync, bp, act;
};

struct isp_tpg_context {
	bool enabled;
	u32 image_type;
	u32 frame_num;
	u32 bayer_pattern;
	u32 color_depth;
	u32 resolution;
	u16 pixleGap;
	u16 lineGap;
	u16 gapStandard;
	u32 randomSeed;
	struct isp_tpg_userdefine_mode user_mode_h, user_mode_v;
};

typedef enum mcm_fmt_e{
	MCM_FMT_RAW8 = 0,
	MCM_FMT_RAW10,
	MCM_FMT_RAW12,
	MCM_FMT_RAW14,
	MCM_FMT_RAW16,
	MCM_FMT_RAW20,
	MCM_FMT_MAX,
}mcm_fmt_t;

typedef enum mcm_wr_index_e{
	MCM_INDEX_WR0 = 0,		//write channel 0
	MCM_INDEX_WR1,			//write channel 1	
	MCM_INDEX_G2_WR0,			//MCM_G2 writec channel 0
	MCM_INDEX_G2_WR1,			//MCM_G2 writec channel 1
	MCM_INDEX_WR_MAX,
}mcm_wr_fmt_t;

struct isp_mcm_context {
	bool sensor_mem_bypass;			//0: use sensor latency memory in MCM
	mcm_fmt_t  wr_fmt[MCM_INDEX_WR_MAX];
	u8  bypass_switch;                  //Bypass switch 0000: sensor0 bypass  0001:sensor1 bypass .. 1111:sensor15 bypass if any
	u16 height[MCM_INDEX_WR_MAX];
	u16 width[MCM_INDEX_WR_MAX];
    mcm_fmt_t  rd_fmt;
	u32 hsync_rpeample_ext;

	bool bypass_enable;
	u32 wr_vsync_blank;
	u32 wr_vsync_duration;
	u32 wr_hsync_blank;
	u32 wr_hsync_preample;

	u32 rd_vsync_blank;
	u32 rd_vsync_duration;
	u32 rd_hsync_blank;
	u32 rd_hsync_preample;
	u16 path;
	u32 prev_path_enable;
};

struct isp_mux_context {
	u32 mp_mux;             /**< main path muxer (vi_mp_mux) */
	u32 sp_mux;             /**< self path muxer (vi_dma_spmux) */
	u32 chan_mode;          /**< 1-mp, 2-sp, 4-sp2 */
	u32 ie_mux;             /**< image effects muxer (vi_dma_iemux) */
	u32 dma_read_switch;    /**< dma read switch (vi_dma_switch) */
	u32 if_select;          /**< interface selector (if_select) */
};

struct isp_awb_context {
	bool enable;
	u8 mode;
	u16 gain_r, gain_gr, gain_gb, gain_b;
	struct ic_window window;
	u16 refcb_max_b;
	u16 refcr_max_r;
	u16 max_y;
	u16 max_c_sum;
	u16 min_y_max_g;
	u16 min_c;
};

struct isp_awb_mean {
	u32 r, g, b;
	u32 no_white_count;
};

struct isp_cnr_context {
	bool enable;
	u32 line_width;
	u32 threshold_1;
	u32 threshold_2;
};

#ifndef ISP_CNRV21
#define CNR2_WEIGHT_TABLE_SIZE 128
#define MAX_CNR2_WEIGHT_TABLE 1023
#define CNR
#else
#define CNR2_WEIGHT_TABLE_SIZE 64
#define MAX_CNR2_WEIGHT_TABLE 255
#endif

struct isp_cnr2_context {
	// bool enable;
	bool ynr_enable;
	bool cnr_enable;
	bool ynr_weight_table_change;
	bool cnr_weight_table_change;
	bool neighbor_pix_consider;
	// float c_sigma_layer[4];
	// float c_sigma_merge[3];
	// float y_sigma;
	u32 cnr_weight_table_filter[4][CNR2_WEIGHT_TABLE_SIZE+1];
#ifndef ISP_CNRV21
	u32 cnr_weight_table_merge[3][CNR2_WEIGHT_TABLE_SIZE+1];
#endif
	u32 ynr_weight_table_filter[CNR2_WEIGHT_TABLE_SIZE+1];
	u8 bit;
	u8 cnr_strength1;
	u8 cnr_strength2;
	u8 ynr_strength1;
	u8 ynr_strength2;
	u8 cnr_filter_shift;
	u32 cnr_filter_mask0;
	u32 cnr_filter_mask1;
};

struct isp_cc_context {
	u32 lCoeff[9];
	bool update_curve;
	bool conv_range_y_full, conv_range_c_full;
};

struct isp_xtalk_context {
	u32 lCoeff[9];
	u32 r, g, b;
};


#define CAEMRIC_GRAD_TBL_SIZE 8
#define CAMERIC_DATA_TBL_SIZE 289
#define CAMERIC_MAX_LSC_SECTORS 16
#define CA_CURVE_DATA_TABLE_LEN 65

struct isp_lsc_context {
	bool lsc_enable;
	bool tbl_size_changed;
	/**< correction values of R color part */
	u16 r[CAMERIC_DATA_TBL_SIZE];
	/**< correction values of G (red lines) color part */
	u16 gr[CAMERIC_DATA_TBL_SIZE];
	/**< correction values of G (blue lines) color part */
	u16 gb[CAMERIC_DATA_TBL_SIZE];
	/**< correction values of B color part */
	u16 b[CAMERIC_DATA_TBL_SIZE];
	/**< multiplication factors of x direction */
	u16 x_grad[CAEMRIC_GRAD_TBL_SIZE];
	/**< multiplication factors of y direction */
	u16 y_grad[CAEMRIC_GRAD_TBL_SIZE];
	/**< sector sizes of x direction */
	u16 x_size[CAEMRIC_GRAD_TBL_SIZE];
	/**< sector sizes of y direction */
	u16 y_size[CAEMRIC_GRAD_TBL_SIZE];
};

#define CAEMRIC_GRAD_TBL_SIZE_UP 32
#define CAMERIC_DATA_TBL_SIZE_UP 1089
#define CAMERIC_MAX_LSC_SECTORS_UP 32
#define CA_CURVE_DATA_TABLE_LEN_UP 65

#ifdef ISP_LSC_V3
struct isp_lsc_context_up {
	bool lsc_enable;
	bool tbl_size_changed;
	/**< correction values of R color part */
	u16 r[CAMERIC_DATA_TBL_SIZE_UP];
	/**< correction values of G (red lines) color part */
	u16 gr[CAMERIC_DATA_TBL_SIZE_UP];
	/**< correction values of G (blue lines) color part */
	u16 gb[CAMERIC_DATA_TBL_SIZE_UP];
	/**< correction values of B color part */
	u16 b[CAMERIC_DATA_TBL_SIZE_UP];
	/**< multiplication factors of x direction */
	u16 x_grad[CAEMRIC_GRAD_TBL_SIZE_UP];
	/**< multiplication factors of y direction */
	u16 y_grad[CAEMRIC_GRAD_TBL_SIZE_UP/2];
	/**< sector sizes of x direction */
	u16 x_size[CAEMRIC_GRAD_TBL_SIZE_UP];
	/**< sector sizes of y direction */
	u16 y_size[CAEMRIC_GRAD_TBL_SIZE_UP/2];
};
#elif defined ISP_LSC_V4
struct isp_lsc_context_up {
	bool lsc_enable;
	bool tbl_size_changed;
	/**< correction values of R color part */
	u16 r[CAMERIC_DATA_TBL_SIZE_UP];
	/**< correction values of G (red lines) color part */
	u16 gr[CAMERIC_DATA_TBL_SIZE_UP];
	/**< correction values of G (blue lines) color part */
	u16 gb[CAMERIC_DATA_TBL_SIZE_UP];
	/**< correction values of B color part */
	u16 b[CAMERIC_DATA_TBL_SIZE_UP];
	/**< multiplication factors of x direction */
	u16 x_grad[CAEMRIC_GRAD_TBL_SIZE_UP];
	/**< multiplication factors of y direction */
	u16 y_grad[CAEMRIC_GRAD_TBL_SIZE_UP];
	/**< sector sizes of x direction */
	u16 x_size[CAEMRIC_GRAD_TBL_SIZE_UP];
	/**< sector sizes of y direction */
	u16 y_size[CAEMRIC_GRAD_TBL_SIZE_UP];
};
#endif

struct isp_dmoi_context {
	bool enable;
	uint8_t demoire_area_thr;
	uint8_t demoire_sat_shrink;
	uint16_t demoire_r2;
	uint16_t demoire_r1;
	uint8_t demoire_t2_shift;
	uint8_t demoire_t1;
	uint16_t demoire_edge_r2;
	uint16_t demoire_edge_r1;
	uint8_t demoire_edge_t2_shift;
	uint16_t demoire_edge_t1;
};

struct isp_shap_context {
	bool enable;
	uint16_t sharpen_factor_black;
	uint16_t sharpen_factor_white;
	uint16_t sharpen_clip_black;
	uint16_t sharpen_clip_white;
	uint16_t sharpen_t4_shift;
	uint16_t sharpen_t3;
	uint8_t sharpen_t2_shift;
	uint16_t sharpen_t1;
	uint16_t sharpen_r3;
	uint16_t sharpen_r2;
	uint16_t sharpen_r1;
};

struct isp_shap_line_context {
	bool enable;
	uint8_t sharpen_line_shift2;
	uint8_t sharpen_line_shift1;
	uint16_t sharpen_line_t1;
	uint16_t sharpen_line_strength;
	uint16_t sharpen_line_r2;
	uint16_t sharpen_line_r1;
};

struct isp_gfilter_context {
	uint8_t sharpen_size;
	uint8_t hf_filt_00;
	uint8_t hf_filt_01;
	uint8_t hf_filt_02;
	uint8_t hf_filt_10;
	uint8_t hf_filt_11;
	uint8_t hf_filt_12;
	uint8_t hf_filt_20;
	uint8_t hf_filt_21;
	uint8_t hf_filt_22;
};

struct isp_skin_context {
	bool enable;
	uint16_t cb_thr_max_2047;
	uint16_t cb_thr_min_2047;
	uint16_t cr_thr_max_2047;
	uint16_t cr_thr_min_2047;
	uint16_t y_thr_max_2047;
	uint16_t y_thr_min_2047;
};

struct isp_depurple_context {
	bool enable;
	bool red_sat, blue_sat;
	uint8_t depurple_sat_shrink;
	uint8_t depurple_thr;
};

struct isp_intp_thr_cxt {
	u16 intp_dir_thr_min;
	u16 intp_dir_thr_max;
};

struct isp_dmsc_context {
	bool enable;
	uint8_t demosaic_thr;
	uint8_t denoise_stren;
	struct isp_intp_thr_cxt intp;
	struct isp_dmoi_context demoire;
	struct isp_shap_context sharpen;
	struct isp_gfilter_context gFilter;
	struct isp_shap_line_context sharpenLine;
	struct isp_skin_context skin;
	struct isp_depurple_context depurple;
};

struct isp_ge_context {
	bool enable;
	u16 threshold;
	u16 h_dummy;
};

struct isp_ca_context {
	bool enable;
	u8 mode;
	u16 lut_x[CA_CURVE_DATA_TABLE_LEN];
	u16 lut_luma[CA_CURVE_DATA_TABLE_LEN];
	u16 lut_chroma[CA_CURVE_DATA_TABLE_LEN];
	u16 lut_shift[CA_CURVE_DATA_TABLE_LEN];
};

struct isp_bp_buffer_context {
	u32 addr_r;
	u32 addr_gr;
	u32 addr_gb;
	u32 addr_b;
};

struct isp_dma_context {
	u32 type;
	u32 base;
	u32 width;
	u32 height;
	u32 burst_y;
	u32 burst_c;
	u8  align;
	u8 id;
#ifdef ISP_MI_HDR
	u8 rd_wr_str;
#endif
	bool tileMode;
	u32 tileWidth;
};

struct isp_dpf_context {
	bool enable;
	u32 filter_type;
	u32 gain_usage;
	u32 strength_r;
	u32 strength_g;
	u32 strength_b;
	u8 weight_g[6];
	u8 weight_rb[6];
	u16 denoise_talbe[17];
	u32 x_scale;
	u32 nf_gain_r;
	u32 nf_gain_gr;
	u32 nf_gain_gb;
	u32 nf_gain_b;
	bool filter_r_off;
	bool filter_gr_off;
	bool filter_gb_off;
	bool filter_b_off;
};

struct isp_is_context {
	bool enable;
	bool update;
	struct ic_window window;
	u32 recenter;
	u32 max_dx, max_dy;
	u32 displace_x, displace_y;
};

struct isp_ee_context {
	bool enable;
	u8 src_strength;
	u8 strength;
	u8 input_sel;
	u32 y_gain, uv_gain, edge_gain;
};

struct isp_exp_context {
	bool enable;
	u32 mode;
	struct ic_window window;
};

struct isp_hist_context {
	bool enable;
	u32 mode;
	u32 step_size;
	struct ic_window window;
	u8 weight[25];
};

struct isp_hist64_context {
	bool enable;
	u8 channel;
	u32 mode;
	u8 vStepSize;
	u32 hStepInc;
	u8 r_coeff, g_coeff, b_coeff;
	u8 sample_shift;
	u16  sample_offset;
	u16 forced_upd_start_line;
	bool forced_upd;
	struct ic_window window;
	u8 weight[25];
};

struct isp_gtm_weight {
    u8 lightness_weight;
    u8 weight0;
    u8 weight1;
    u8 weight2;
};

struct isp_gtm_sta {
	u16 luma[128];
	u32	min;
	u32  max;
};

struct isp_gtm_context {
	bool enable;
	bool bm_cor_enable;
	u8 hstepSize;
	u8 vstepSize;
	u8 r_coeff, g_coeff, b_coeff;
	u32 base;
	u32 slope;
	struct isp_gtm_weight weight;
	u32 pa;
	u32 curve[129];
};

 enum isp_dpcc_replacement {
	ISP_DPCC_REP_2x2 = 0,
	ISP_DPCC_REP_2x2_PLUS_CENTER ,
	ISP_DPCC_REP_3x3 ,
	ISP_DPCC_REP_MAX,
} ;

typedef struct bad_pixel_pos_s{
	u16 v_addr;     //bad pixel vertical address (pixel position)
	u16 h_addr;     //bad pixel horizontal address (pixel position)
} bad_pixel_pos_t;

#define MAX_BAD_PIXEL_CNT 10 // sensor have a relatively small amount of defect pixels.
struct isp_dpcc_bpt {
	// u16 addr;	    //start address if a certain part of the table shall be write or read.
	bad_pixel_pos_t bad_pixel_tbl[MAX_BAD_PIXEL_CNT]; //pointer to the bad pixel table
	u16 bp_number;  //number of the bad pixels contained in the list
	enum isp_dpcc_replacement replace;  //replacement method
	bool enabled;
};

struct isp_dpcc_params {
	u32 line_thresh;
	u32 line_mad_fac;
	u32 pg_fac;
	u32 rnd_thresh;
	u32 rg_fac;
};

struct isp_dpcc_context {
	bool enable;
	u32 mode;
	u32 outmode;
	u32 set_use;
	u32 methods_set[3];
	struct isp_dpcc_params params[3];
	u32 ro_limits;
	u32 rnd_offs;
};

struct isp_flt_context {
	bool enable;
	bool changed;
	u32 denoise;
	u32 sharpen;
	u32 chrV;
	u32 chrH;
};

struct isp_cac_context {
	bool enable;
	u32 hmode, vmode;
	u32 ab, ar, bb, br, cb, cr;
	u32 xns, xnf, yns, ynf;
	u32 hstart, vstart;
	u32 width, height;
    u32 hoffset, voffset;
};

/* degamma */
struct isp_deg_context {
	bool enable;
	u8 segment[16];
	u16 r[17];
	u16 g[17];
	u16 b[17];
};

struct isp_ie_context {
	bool enable;
	u32 mode;
	u32 color_sel;
	u32 color_thresh;
	u32 sharpen_factor;
	u32 sharpen_thresh;
	int32_t m[9];
	u32 tint_cr;
	u32 tint_cb;
	bool full_range;
};

struct isp_afm_result {
	u32 sum_a, sum_b, sum_c;
	u32 lum_a, lum_b, lum_c;
};

struct isp_afm_context {
	bool enable;
	u32 thresh;
	struct ic_window window[3];
	u32 pixCnt[3];
	bool enableWinId[3];
	u32 lum_shift;
	u32 afm_shift;
	u32 max_pix_cnt;
};

struct isp_vsm_result {
	u32 x, y;
};

struct isp_vsm_context {
	bool enable;
	struct ic_window window;
	u32 h_seg, v_seg;
};

#ifndef WDR3_BIN
#define WDR3_BIN 14
#endif
struct isp_wdr3_context {
	bool enable;
	bool changed;
	bool inited;
	u32 strength;
	u32 max_gain;
	u32 global_strength;
	u32 histogram[WDR3_BIN];
	u32 shift[WDR3_BIN];
	u32 invert_linear[WDR3_BIN];
	u32 invert_curve[WDR3_BIN];
	u32 gamma_pre[WDR3_BIN];
	u32 gamma_up[WDR3_BIN];
	u32 gamma_down[WDR3_BIN];
	u32 entropy[WDR3_BIN];
	u32 distance_weight[WDR3_BIN];
	u32 difference_weight[WDR3_BIN];
};

#ifdef ISP_WDR_V4
#ifndef WDR4_BIN
#define WDR4_BIN 20
#endif

struct isp_wdr4_context {
	bool enable;
	bool changed;
	bool inited;
	u8 ratioLSVS, ratioLS;
	u8 drc_bayer_ratio, drc_bayer_ratioLSVS;
	u32 strength;
	u32 high_strength;
	u32 low_strength;
	u32 low_gain;
	u32 global_strength;
	int contrast;
	u32 flat_strength;
	u32 flat_threshold;
	u32 histogram[WDR4_BIN];
	u32 shift[WDR4_BIN];
	u32 shift0[WDR4_BIN];
	u32 invert_linear[WDR4_BIN];
	u32 invert_curve[WDR4_BIN];
	u32 gamma_pre[WDR4_BIN];
	u32 gamma_up[WDR4_BIN];
	u32 gamma_down[WDR4_BIN];
	u32 entropy[WDR4_BIN];
	u32 distance_weight[WDR4_BIN];
	u32 difference_weight[WDR4_BIN];
    u32 smooth_invert[WDR4_BIN];
};
#endif

#ifdef ISP_WDR_V5
struct isp_wdr5_context{
	bool inited;
	u16 dummy_blk;
	bool enable;
	u16 block_height;
	u16 block_width;
	u32 block_area_inverse;
	u8 value_weight_3;
	u8 value_weight_2;
	u8 value_weight_1;
	u8 value_weight_0;
	u8 strength;
	u16 low_strength;
	u8 high_strength;
	u16 drc_bayer_ratio;
	u16 contrast;
	u16 pixel_base_entropy;
	u16 pixel_slope_entropy;
	u32 biliteral_height_sigma;
	u32 biliteral_width_sigma;
	u32 biliteral_value_sigma;
	u16 block_row_flag;
	u16 block_col_flag;
	u8 pixel_base_adjust;
	u8 pixel_slope_adjust;
	bool local_flat_mode;
	u8 local_flat_level;
	bool global_curve_mode;
	u8 global_flat_level;
	u16 global_fixed_weight;
	u32 frame_average;
	u32 frame_std;

	u8 lut_histogram_addr;
	u32 lut_histogram_write_data[65];
	u8 lut_shift_addr;
	u32 lut_shift_write_data[65];
	u8 lut_shift0_addr;
	u32 lut_shift0_write_data[65];
	u8 lut_gammapre_addr;
	u32 lut_gammapre_write_data[65];
	u8 lut_gammaup_addr;
	u32 lut_gammaup_write_data[65]	;
	u8 lut_gammadown_addr;
	u32 lut_gammadown_write_data[65];
	u8 lut_entropy_addr;
	u32 lut_entropy_write_data[65];
	u8 lut_distance_weight_addr;
	u32 lut_distance_weight_write_data[65];
	u8 lut_difference_weight_addr;
	u32 lut_difference_weight_write_data[65];
	u8 lut_weight_raw_addr;
	u32 lut_weight_raw_write_data[65];
	u8 lut_maxgain_addr;
	u32 lut_maxgain_write_data[65]	;
	u8 lut_mingain_addr;
	u32 lut_mingain_write_data[65];
	u8 lut_flat_factor_addr;
	u32 lut_flat_factor_write_data[272];
	u8 lut_flat_level_addr;
	u32 lut_flat_level_write_data[68];
	u8 lut_sat_x_addr;
	u32 lut_sat_x_write_data[65];
	u8 lut_sat_shift_addr;
	u32 lut_sat_shift_write_data[65];
	u8 lut_sat_addr;
	u32 lut_sat_write_data[65];
	u8 lut_degamma_addr;
	u32 lut_degamma_write_data[65]	;

	u8 ratioLSVS;
	u8 ratioLS;
	u32 buffer_log[65];
};
#endif

struct isp_exp2_context {
	bool enable;
	struct ic_window window;
	/* weight; */
	u8 r, gr, gb, b;
	u8 input_select;   //00: degamma output, 01: awb_gain output  10: WDR3 output
	/* write 4096/8192 EXPV2 mean value to dma by MI MP-JDP path. */
	/* physical address, alloacte by user */
	u64 pa;
};

#define ISP_2DNR_SIGMA_BIN 60
struct isp_2dnr_context {
	bool enable;
	u32 pre_gamma;
	u32 strength;
	u16 sigma[ISP_2DNR_SIGMA_BIN];
#if defined(ISP_2DNR_V2) || defined(ISP_2DNR_V4) || defined(ISP_2DNR_V5)
	u32 sigma_sqr;
	u32 weight;
#endif
#if defined(ISP_2DNR_V4) || defined(ISP_2DNR_V5)
	u16 str_off;
	u16 str_max;
	u8 str_slope;
#ifdef ISP_2DNR_V51
	u16 sigma_luma_px[12];
	u16 sigma_luma_y[12];
#endif
#endif
};

struct isp_3dnr_compress_context {
	u8 weight_up_y[2];
	u8 weight_down[4];
	u8 weight_up[8];
};
struct isp_3dnr_context {
	bool enable;
	bool update_bin;
	bool enable_h, enable_v;
	bool enable_temperal;
	bool enable_dilate;
	bool init;
	u32 spacial_curve[17];
	u32 temperal_curve[17];
	u32 strength;
	u32 motion_factor;
	u16 delta_factor;
	/* write full denoise3d reference raw image to dma by MI SP2. */
	/* physical address, alloacte by user */
	u64 pa;
	u32 size;
	struct isp_3dnr_compress_context compress;
};

struct isp_3dnr_update {
	u32 thr_edge_h_inv;
	u32 thr_edge_v_inv;
	u32 thr_motion_inv;
	u32 thr_range_s_inv;
	u32 range_t_h;
	u32 range_t_v;
	u32 range_d;
	u32 thr_range_t_inv;
	u32 thr_delta_h_inv;
	u32 thr_delta_v_inv;
	u32 thr_delta_t_inv;
};

#ifdef ISP_3DNR_V3
#define DENOISE3D_GAMMA_BIN 33

struct isp_tdnr_stats {
	u32 bg_sum;
	u32 motion_sum;
	u32 bg_pixel_cnt;
	u32 motion_pixel_cnt;
	u32 frame_avg;
};

#ifdef ISP_3DNR_V32
#define NOISE_CURVE_MAX 1048575
#define NOISE_CURVE_INPUT_BIT 20
#define NOISE_CURVE_SAMPLE_NUM 129

struct isp_tdnr_curve {
	u32 curve_compress[NOISE_CURVE_SAMPLE_NUM];
	u32 curve_expand[NOISE_CURVE_SAMPLE_NUM];
};
#else

struct isp_tdnr_curve {
	u16 preGamma_y[DENOISE3D_GAMMA_BIN];
	u16 invGamma_y[DENOISE3D_GAMMA_BIN];
};
#endif
struct isp_tdnr_buffer {
    /* write full denoise3d reference raw image to dma by MI SP2. */
	/* physical address, alloacte by user */
	u64 pa_refer;
	u32 size_refer;
    u64 pa_motion;
    u32 size_motion;
};

struct isp_3dnrV3_context {
	bool enable;

	bool enable_motion_erosion;
	bool enable_motion_converage;
	bool enable_motion_dilation;
	bool enable_tnr;
	bool enable_2dnr;
	u8  strength;
	u16 noise_level;
	u16 noise_mean;
	u16 noise_threshold;
	u16 motion_mean;
	u8  range_h;
	u8  range_v;
	u8  dilate_range_h;
	u8  dilate_range_v;
	u32 motion_inv_factor;
	u16 update_factor;
	u16 motion_update_factor;
	u16 pre_motion_weight;
	u16 motion_slope;
	u16 motion_converage_shift;
	u16 motion_converage_max;
	u16 tnr_val_shift_bit;
	u16 tnr_diff_shift_bit;
	u16 sad_weight;
	u16 luma_pixel_val_shift;
	u16 luma_pixel_slope;
	u16 luma_pixel_slope_shift;
	u16 luma_pixel_slope_min;
	u16 dmy_hblank;
	u32 fn;
	struct isp_tdnr_curve curve;
    struct isp_tdnr_buffer buf;
 
	char frames;  //skip the first frame before  clear reset.
				//keep the 3dnr enable in register to avoid scrolling screen when repeat enable
#ifdef ISP_3DNR_V32
	bool febeEn;
   u32 filterLen;      //[0,1024] 20
   u32 filterLen2;     //[0,1024] 10
      u8 dilateH;         //[3,7] 3
   u8 noisemodelTransStart;    //0
   u8 fixCurveStart;       //0
   u8 lumaCompEn;          
	u8 gammaBePreBlsAFixed;
    u8 gammaBePreBlsBFixed;
    u8 gammaBePreBlsCFixed;
    u8 gammaBePreBlsDFixed;

#else
	bool enable_inv_gamma;
	bool enable_pre_gamma;
#endif
};

#endif

#ifdef ISP_3D_LUT

#define LUT3D_TBL_NUM 17*17*17
struct isp_lut3d_context{
	bool enable;
	bool lut3d_r_lut_access_vld;
	bool lut3d_g_lut_access_vld;
	bool lut3d_b_lut_access_vld;
	u32 lut3d_r[LUT3D_TBL_NUM];
	u32 lut3d_g[LUT3D_TBL_NUM];
	u32 lut3d_b[LUT3D_TBL_NUM];
};

#endif


struct isp_hdr_context {
	bool enable;
	/* hdr bls */
	u16 width;
	u16 height;
	u8 l_bit_dep, s_bit_dep, vs_bit_dep, ls_bit_dep;
	u8 weight0, weight1, weight2;
	u16 start_linear, norm_factor_mul_linear;
	u16 start_nonlinear, norm_factor_mul_nonlinear;
	u16 dummy_hblank, out_hblank;
	u16 out_vblank;
	u16 long_exp, short_exp, very_short_exp;
	u16 bls[4];
	u16 digal_gain[4];
	u32 reg;
	/* hdr awb */
	u32 r, gr, gb, b;
	/* hdr exp */
	u32 compress_lut[15];
	/* long short, very short */
	u32 ls0, ls1, vs0, vs1;
	u32 ext_bit;
	u32 valid_thresh;
	u32 offset_val;
	u32 sat_thresh;
	u32 combine_weight;
};

struct isp_simp_context {
	bool enable;
	u32 x, y;
	u32 r, g, b;
	u32 transparency_mode;
	u32 ref_image;
};

struct isp_compand_curve_context {
	bool     enable; 
	bool     update_curve;
	uint8_t  in_bit;
	uint8_t  out_bit;
	uint32_t px[64];
	uint32_t x_data[63];
	uint32_t y_data[64];
};

struct isp_compand_bls_context {
	uint8_t  bit_width;
	bool     enable;
	uint32_t a;
	uint32_t b;
	uint32_t c;
	uint32_t d;
};

/* COMPAND */
struct isp_comp_context {
	bool enable;
	struct isp_compand_curve_context expand;
	struct isp_compand_bls_context   bls;
	struct isp_compand_curve_context compress;
};

struct isp_cproc_context {
	bool enable;
    bool changed;
	u32 contrast;
	u32 brightness;
	u32 saturation;
	u32 hue;
	bool y_out_full;
	bool c_out_full;
	bool y_in_full;
};

struct elawb_ellipse_info {
	u32 x, y;		/* ellipse center */
	u32 a1, a2, a3, a4;	/* ellipse axis */
	u32 r_max_sqr;
};

struct isp_elawb_context {
	bool enable;
	u32 id;			/* ellipse id,  1-8,  0 means update all. */
	struct elawb_ellipse_info info[8];
	struct ic_window window;
	u32 r, gr, gb, b;	/* gain */
};

struct isp_gcmono_data {
	u8 basePara[1024];
	u32 px[64];
	u32 dataX[63];
	u32 dataY[64];
};

struct isp_gcmono_context {
	u32 enable;
	u32 mode;
};

struct isp_rgbgamma_data {
	u32 rgbgc_r_px[64];
	u32 rgbgc_r_datax[63];
	u32 rgbgc_r_datay[64];
	u32 rgbgc_g_px[64];
	u32 rgbgc_g_datax[63];
	u32 rgbgc_g_datay[64];
	u32 rgbgc_b_px[64];
	u32 rgbgc_b_datax[63];
	u32 rgbgc_b_datay[64];
};

struct isp_rgbgamma_context {
	bool enable,changed,rgbgamma_data_changed;
	struct isp_rgbgamma_data rgbgamma_data;
};

struct isp_rgbir_bls_context {
	u16 a;
	u16 b;
	u16 c;
	u16 d;
};

struct isp_rgbir_rgb_gain_context {
	u16 r;
	u16 g;
	u16 b;
};

struct isp_rgbir_dpcc_context {
	u16 median_thr[4];
	u16 avg_thr[4];
};

struct isp_rgbir_cc_context {
	int16_t mtx[3][4];
};

struct isp_rgbir_des_context {
	u32 ir_px1, ir_py[16];
	u32 ir_pd[16], l_pd[16];
	u32 l_px1, l_py[16];
};

struct isp_rgbir_sharpen_context {
	u8 sharpen_lvl, middle;
	u16 thresh_bl0, thresh_bl1;
	u16 thresh_sh0, thresh_sh1;
	u8 lum_weight_min, lum_weight_kink, lum_weight_gain;
};

struct isp_rgbir_ir_dnr_context {
	u16 ir_sigmaS;
	u8 winweight[6];
	u16 width, height;
};
struct isp_rgbir_context{
	bool prefilt_enable, green_filt_mode, green_filt_enable;
	u8 rgbirPattern, out_rgb_pattern, green_filt_stage1_select;
	u8 demosaic_threshold;
	bool part1_enable, part2_enable;
    bool enable_ir_raw_out;
	bool rccc_enable;
	uint32_t ir_threshold, l_threshold;
	u16 width, height;

	struct isp_rgbir_bls_context bls;
	struct isp_rgbir_rgb_gain_context rgb_gain;
	struct isp_rgbir_dpcc_context dpcc;
	struct isp_rgbir_cc_context cc;
	struct isp_rgbir_des_context des;
	struct isp_rgbir_sharpen_context sharpen;
	struct isp_rgbir_ir_dnr_context ir_dnr;
};

struct isp_crop_context {
	bool enabled;
	struct ic_window window;
};

struct isp_pdaf_context {
	bool enable;
	bool correct_en;
	u8 bayer_pattern;
	u8 sensor_type;
	u32 image_h, image_v;
	u8 pix_00_shift_hw[2];
	u8 correct_threshold;
	struct ic_window wcorrect_rect;
	u16 pd_area[4];
	u8 num_per_area[2];
	u8 shift_mark[32];
	u8 shift_lr[2];
	struct ic_window roi;
	u8 correct_r_channel, correct_l_channel;
	u8 correct_r_limit_base, correct_l_limit_base;
	u8 correct_r_change_down, correct_l_change_down;
	u8 correct_r_change_up, correct_l_change_up;
	/* physical address, alloacte by user */
	u32 pa;
};

struct isp_pp_path_context {
	u32 mi_pp_ctrl;
	u32 mi_pp_fmt;
	u32 mi_pp_data_swap;
	u32 mi_pp_y_base_ad_init;
	u32 mi_pp_y_size_init;
	u32 mi_pp_y_offs_cnt_init;
	u32 mi_pp_y_llength;
	u32 mi_pp_y_pic_width;    
	u32 mi_pp_y_pic_height;
	u32 mi_pp_y_pic_size;
	u32 mi_pp_y_lval_bytes;
};

#ifdef ISP_HDR_STITCH2
struct isp_hdrv2_context {
    bool                       bypass;                            /**< measuring enabled */
    u32                        bypass_select;
    u8                         sensor_type;
    u8                         stitching_mode;
    u8                         base_frame;
    u32                        ratio_integer[2];
    u32                        ratio_fractional[2];
    u32                        trans_range[4][2];
    u32                        extend_bit[2];
    u32                        color_weight[3];
    u32                        bls_long[4];
    u32                        bls_short[4];
    u32                        bls_very_short[4];
    u32                        bls_out[4];
	u32						   dgain[3][4];
	u32						   vs_valid_thresh;
	u32						   vs_offset_val;
    u32		                   long_sat_thresh;
    u32		                   long_sat_pix_weight;
    u32		                   compress_lut_start_idx;
    u32		                   compress_lut[15];
	u8                         hdr_id;
};

struct isp_hdrv2_dpf_context {
    bool                       dpf_enable;
    u32                        invR;
    u32                        invG;
    u32                        invB;
    u32                        weight_g_1234;
    u32                        weight_g_56;
    u32                        weight_rb_1234;
    u32                        weight_rb_56;
    u32                        noise_curve[17];
};

struct isp_hdrv2_deghost_context {
    bool                       motion_enable[2];
    u32                        motion_weight[2];
    u32                        motion_weight_shorter;
    u32                        motion_sat_threshold;
    u32                        motion_weight_update_threshold;
    u32                        motion_lower_threshold_ls[4];
    u32                        motion_factor_threshold_ls[4];
    u32                        motion_lower_threshold_lsvs[4];
    u32                        motion_factor_threshold_lsvs[4];
    u32                        dark_lower_threshold_ls[4];
    u32                        dark_factor_threshold_ls[4];
    u32                        dark_lower_threshold_lsvs[4];
    u32                        dark_factor_threshold_lsvs[4];
};
#endif


#ifdef ISP_MI_PP_READ
typedef struct pp_dma_line_entry_s {
	u16 entry_line_num;
	u16 buf_line_num;

} pp_dma_line_entry_t;

#endif

struct isp_mp_ctrl_context {
	bool enabled;		//	"mp ctrl enable:0: bypass, output direct through input 1: eable the mp ctrl function"

	bool vsync_pol;		//	"Vertical sync polarity 0 : high active;1 : low active "
	bool hsync_pol;		//"Horizontal sync polarity 0 : high active; 1: low active"
	bool out_pixel_num;	//"At output port 1'b0: one pixel in one cycle 1'b1: two pixel in one cycle"
	bool in_pixel_num;	//"At input port 1'b0: one pixel in one cycle 1'b1: two pixel in one cycle"
	u16 crop_size; 		//	crop size for one line
	u16 crop_offset; 	//	crop offset for one line
	bool buf_full; 		//	buf full indicator  for line SRAM
	// bool crop_en;		//	"soft reset at input side clock,High active "
	// bool soft_rst; 		//"mp ctrl enable: 0 : bypass,output direct through input 1 : eable the mp ctrl function "
	// bool mp_mode; 		//"Vertical sync polarity 0 : high active; 1 : low active "
	// u16 crop_h_start; 	// crop hstart
	// u16 crop_hsize; 	//crop hsize
	// u16 crop_v_start; 	//crop vstart
	// u16 crop_vsize; 	//crop vsize
	// u16 final_hsize; 	//final hsize
	// u16 final_vsize; 	//final vsize
};

#ifdef ISP_STA

#define AEV2_DMA_OFFSET 0
#define AEV2_DMA_SIZE (4096*2)

#define HDR0_DMA_SIZE 168
#define HDR0_DMA_OFFSET (4096*2)

#define HDR1_DMA_SIZE 168
#define HDR1_DMA_OFFSET (4096*2+168)

#define HDR2_DMA_SIZE 168
#define HDR2_DMA_OFFSET (4096*2+168+168)

#define AF_DMA_SIZE 24
#define AF_DMA_OFFSET (4096*2+168+168+168) 

#define HIST256_DMA_SIZE 1024
#define HIST256_DMA_OFFSET (4096*2+168+168+168+24)

#define HIST64_DMA_SIZE 128
#define HIST64_DMA_OFFSET (4096*2+168+168+168+24+1024)

#define BASE_F_CNT_SIZE 4
#define BASE_F_CNT_OFFSET (4096*2+168+168+168+24+1024+128) 

#ifdef ISP_GTM
#define GTM_DMA_SIZE 128*2
#define GTM_DMA_OFFSET (4096*2+168+168+168+24+1024+128+4)

#define ISP_STA_DMA_TOTAL_SIZE 10132
#define ISP_STA_DMA_SIZE_MALLOC 10144//18208 //(ISP_STA_DMA_TOTAL_SIZE+15)/16*16
#else
#define ISP_STA_DMA_TOTAL_SIZE 9876
#define ISP_STA_DMA_SIZE_MALLOC 9888 //(ISP_STA_DMA_TOTAL_SIZE+15)/16*16
#endif  //ISP_GTM

#else

#ifndef ISP_AEV2_V2
#define AEV2_DMA_SIZE 4096
#else
#define AEV2_DMA_SIZE (4096 *2)
#endif

#endif

struct isp_sta_context {
	bool isp_sta_enable;
	bool stitch_sta_enable;
	u64 pa;
};

struct isp_ic_dev {
	int id;
	HalHandle_t HalHandle;
	struct isp_context ctx;
	struct isp_digital_gain_cxt dgain;
	struct isp_bls_context bls;
	struct isp_tpg_context tpg;
	struct isp_mcm_context mcm;
	struct isp_mux_context mux;
	struct isp_awb_context awb;
#if defined(ISP_LSC_V3)||defined(ISP_LSC_V4)
	struct isp_lsc_context_up lsc_up;
#endif
	struct isp_lsc_context lsc;
	struct isp_gamma_out_context gamma_out;
	struct isp_xtalk_context xtalk;
	struct isp_cc_context cc;
	struct isp_cnr_context cnr;
	struct isp_cnr2_context cnr2;
	struct isp_is_context is;
	struct isp_is_context rawis;
	struct isp_mi_context mi;
	struct isp_dpf_context dpf;
	struct isp_ee_context ee;
	struct isp_exp_context exp;
	struct isp_hist_context hist;
#ifdef ISP_HIST64
	struct isp_hist64_context hist64;
#endif
	struct isp_dpcc_context dpcc;
	struct isp_flt_context flt;
	struct isp_cac_context cac;
	struct isp_deg_context deg;
	struct isp_ie_context ie;
	struct isp_vsm_context vsm;
	struct isp_afm_context afm;
	struct isp_wdr3_context wdr3;
	struct isp_exp2_context exp2;
	struct isp_hdr_context hdr;
	struct isp_exp_context hdrexp;
	struct isp_hist_context hdrhist;
	struct isp_2dnr_context dnr2;
	struct isp_3dnr_context dnr3;

	struct isp_comp_context comp;
	struct isp_simp_context simp;
	struct isp_cproc_context cproc;
	struct isp_elawb_context elawb;
	struct isp_gcmono_context gcmono;
	struct isp_rgbgamma_context rgbgamma;
	struct isp_dmsc_context demosaic;
	struct isp_ge_context ge;
	struct isp_ca_context ca;
	struct isp_dummy_hblank_cxt hblank;
	struct isp_misirq_regaddr irqaddr;
    isp_wdr_context_t wdr;
	bool streaming;
	bool update_gamma_en;
#ifdef ISP_WDR_V4
	struct isp_wdr4_context wdr4;
#endif
#ifdef ISP_3DNR_V3
	struct isp_3dnrV3_context tdnr;
#endif
#ifdef ISP_3D_LUT
	struct isp_lut3d_context lut3d;
#endif

	struct isp_rgbir_context rgbir;
	u32   isp_mis;
#ifdef ISP_MI_PP_READ
	pp_dma_line_entry_t pp_dma_line_entry;
#endif
	struct isp_miv_info miv_info;
	struct isp_crop_context  crop[ISP_MI_PATH_SP2_BP + 1], cropBackEnd[ISP_MI_PATH_SP2_BP + 1];
	struct isp_dpcc_bpt dpcc_bpt; // dpcc bad pixel table
	struct isp_pdaf_context pdaf; 
	struct isp_pp_path_context pp_path;

#ifdef ISP_WDR_V5
	struct isp_wdr5_context wdr5;

#endif 

#ifdef ISP_HDR_STITCH2
    struct isp_hdrv2_context hdrv2;
    struct isp_hdrv2_dpf_context hdrv2_dpf[3];
    struct isp_hdrv2_deghost_context hdrv2_deghost;
#endif
    struct isp_mp_ctrl_context mp_ctrl;
//#ifdef ISP_STA
	struct isp_sta_context isp_sta;
//#endif
	struct isp_gtm_context gtm;
};


void isp_write_reg(struct isp_ic_dev *dev, u32 offset, u32 val);
u32 isp_read_reg(struct isp_ic_dev *dev, u32 offset);
void isp_write_tbl(struct isp_ic_dev *dev, u32 offset, u32 val);

#endif /* _ISPIC_DEV_H_ */
