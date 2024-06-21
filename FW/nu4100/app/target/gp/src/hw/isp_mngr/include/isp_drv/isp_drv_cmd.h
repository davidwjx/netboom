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

/* VeriSilicon 2020 */

#ifndef _ISP_DRV_CMD_H_
#define _ISP_DRV_CMD_H_
#include "isp_ic_dev.h"

enum {
	ISPDRV_RESET				= 0x00,
	ISPDRV_WRITE_REG			= 0x01,
	ISPDRV_READ_REG 			= 0x02,
	ISPDRV_S_INPUT				= 0x03,
	ISPDRV_ENABLE				= 0x04,
	ISPDRV_DISABLE				= 0x05,
	ISPDRV_ISP_STATUS			= 0x06,
	ISPDRV_ISP_STOP 			= 0x07,
	ISPDRV_START_CAPTURE		= 0x08,
	ISPDRV_DISABLE_ISP_OFF		= 0x09,
	ISPDRV_SET_BUFFER			= 0x0A,
	ISPDRV_SET_BP_BUFFER		= 0x0B,
	ISPDRV_START_DMA_READ		= 0x0C,
	ISPDRV_MI_START 			= 0x0D,
	ISPDRV_MI_STOP				= 0x0E,
	ISPDRV_ENABLE_TPG			= 0x0F,
	ISPDRV_DISABLE_TPG			= 0x10,
	ISPDRV_S_TPG				= 0x11,
	ISPDRV_S_MCM_WR				= 0x12,
	ISPDRV_ENABLE_BLS			= 0x13,
	ISPDRV_DISABLE_BLS			= 0x14,
	ISPDRV_S_BLS				= 0x15,
	ISPDRV_S_MUX				= 0x16,
	ISPDRV_ENABLE_AWB			= 0x17,
	ISPDRV_DISABLE_AWB			= 0x18,
	ISPDRV_S_AWB				= 0x19,
	ISPDRV_G_AWBMEAN			= 0x1A,
	ISPDRV_S_IS 				= 0x1B,
	ISPDRV_S_RAW_IS 			= 0x1C,
	ISPDRV_S_CNR				= 0x1D,
	ISPDRV_S_CC 				= 0x1E,
	ISPDRV_S_XTALK				= 0x1F,
	ISPDRV_S_GAMMA_OUT			= 0x20,
	ISPDRV_ENABLE_LSC			= 0x21,
	ISPDRV_DISABLE_LSC			= 0x22,
	ISPDRV_S_LSC_TBL			= 0x23,
	ISPDRV_S_LSC_SEC			= 0x24,
	ISPDRV_S_DPF				= 0x25,
	ISPDRV_S_EE 				= 0x26,
	ISPDRV_S_EXP				= 0x27,
	ISPDRV_S_HDREXP				= 0x28,
	ISPDRV_G_EXPMEAN			= 0x29,
	ISPDRV_G_HDREXPMEAN			= 0x2A,
	ISPDRV_S_HIST				= 0x2B,
	ISPDRV_G_HISTMEAN			= 0x2C,
	ISPDRV_S_HDRHIST			= 0x2D,
	ISPDRV_G_HDRHISTMEAN        = 0x2E,
	ISPDRV_S_HIST64             = 0x2F,
	ISPDRV_G_HIST64MEAN         = 0x30,
	ISPDRV_G_HIST64VSTART_STATUS= 0x31,
	ISPDRV_U_HIST64 			= 0x32,
	ISPDRV_S_DPCC				= 0x33,
	ISPDRV_S_FLT				= 0x34,
	ISPDRV_S_CAC				= 0x35,
	ISPDRV_S_DEG				= 0x36,
	ISPDRV_S_AFM				= 0x37,
	ISPDRV_G_AFM				= 0x38,
	ISPDRV_S_VSM				= 0x39,
	ISPDRV_G_VSM				= 0x3A,
	ISPDRV_S_IE 				= 0x3B,
	ISPDRV_ENABLE_WDR3			= 0x3C,
	ISPDRV_DISABLE_WDR3 		= 0x3D,
	ISPDRV_U_WDR3				= 0x3E,
	ISPDRV_S_WDR3				= 0x3F,
	ISPDRV_S_EXP2               = 0x40,
	ISPDRV_S_EXP2_INPUTSEL      = 0x41,
	ISPDRV_S_EXP2_SIZERATIO     = 0x42,
	ISPDRV_S_2DNR				= 0x43,
	ISPDRV_S_3DNR				= 0x44,
	ISPDRV_G_3DNR				= 0x45, /* get last avg */
	ISPDRV_U_3DNR				= 0x46, /* update */
	ISPDRV_R_3DNR				= 0x47, /* read back 3dnr reference image. */
	ISPDRV_S_3DNR_CMP			= 0x48, /*config 3dnr compress */
	ISPDRV_U_3DNR_STRENGTH 		= 0x49,
	ISPDRV_S_3DNR_MOT			= 0x4A,  /*config 3dnr motion*/
	ISPDRV_S_3DNR_DLT  			= 0x4B,/*config 3dnr delta*/
	ISPDRV_S_HDR				= 0x4C,
	ISPDRV_S_COMP				= 0x4D,
	ISPDRV_S_CPROC				= 0x4E,
	ISPDRV_S_SIMP				= 0x4F,
	ISPDRV_S_ELAWB				= 0x50,
	ISPDRV_S_HDR_WB 			= 0x51,
	ISPDRV_S_HDR_BLS			= 0x52,
	ISPDRV_S_HDR_DIGITAL_GAIN	= 0x53,
	ISPDRV_ENABLE_WB			= 0x54,
	ISPDRV_DISABLE_WB			= 0x55,
	ISPDRV_DISABLE_HDR			= 0x56,
	ISPDRV_ENABLE_HDR			= 0x57,
	ISPDRV_ENABLE_GAMMA_OUT 	= 0x58,
	ISPDRV_DISABLE_GAMMA_OUT	= 0x59,
	ISPDRV_G_STATUS 			= 0x5A,
	ISPDRV_G_FEATURE			= 0x5B,
	ISPDRV_G_FEATURE_VERSION	= 0x5C,
	ISPDRV_ENABLE_GCMONO		= 0x5D,
	ISPDRV_DISABLE_GCMONO		= 0x5E,
	ISPDRV_S_GCMONO 			= 0x5F,
	ISPDRV_ENABLE_RGBGAMMA		= 0x60,
	ISPDRV_DISABLE_RGBGAMMA 	= 0x61,
	ISPDRV_S_RGBGAMMA			= 0x62,
	ISPDRV_S_DEMOSAIC			= 0x63,
	ISPDRV_S_DMSC_INTP			= 0x64,
	ISPDRV_S_DMSC_DMOI			= 0x65,
	ISPDRV_S_DMSC_SKIN			= 0x66,
	ISPDRV_S_DMSC_CAC			= 0x67,
	ISPDRV_S_DMSC_SHAP			= 0x68,
	ISPDRV_S_DMSC_SHAP_LINE		= 0x69,
	ISPDRV_S_DMSC_DEPURPLE      = 0x6A,
	ISPDRV_S_DMSC_GFILTER       = 0x6B,
	ISPDRV_S_DMSC				= 0x6C,
	ISPDRV_S_GREENEQUILIBRATE	= 0x6D,
	ISPDRV_S_COLOR_ADJUST		= 0x6E,
	ISPDRV_S_DIGITAL_GAIN		= 0x6F,
	ISPDRV_ENABLE_DIGITAL_GAIN	= 0x70,

#ifdef ISP_WDR_V4
	ISPDRV_ENABLE_WDR4          = 0x71,
	ISPDRV_DISABLE_WDR4         = 0x72,
	ISPDRV_U_WDR4               = 0x73,
	ISPDRV_S_WDR4               = 0x74,
#endif
	ISPDRV_WDR_CONFIG	    	= 0x75,
	ISPDRV_S_WDR_CURVE	    	= 0x76,
	ISPDRV_ENABLE_RGBIR	    	= 0x77,
	ISPDRV_S_RGBIR		    	= 0x78,
	ISPDRV_RGBIR_HW_INIT		= 0x79,
	ISPDRV_RGBIR_S_IR_DNR		= 0x7A,
	ISPDRV_RGBIR_S_SHARPEN		= 0x7B,
	ISPDRV_RGBIR_S_DES	    	= 0x7C,
	ISPDRV_RGBIR_S_CC_MATRIX	= 0x7D,
	ISPDRV_RGBIR_S_DPCC	    	= 0x7E,
	ISPDRV_RGBIR_S_GAIN	    	= 0x7F,
	ISPDRV_RGBIR_S_BLS	    	= 0x80,
	ISPDRV_RGBIR_S_IR_RAW_OUT	= 0x81,
	ISPDRV_S_TDNR               = 0x82,
	ISPDRV_TDNR_ENABLE  		= 0x83,
	ISPDRV_TDNR_DISABLE         = 0x84,
	ISPDRV_TDNR_ENABLE_TDNR     = 0x85,
	ISPDRV_TDNR_DISABLE_TDNR    = 0x86,
	ISPDRV_TDNR_ENABLE_2DNR     = 0x87,
	ISPDRV_TDNR_DISABLE_2DNR    = 0x88,
	ISPDRV_S_TDNR_CURVE         = 0x89,
	ISPDRV_G_TDNR               = 0x8A,
	ISPDRV_S_TDNR_STRENGTH      = 0x8B,
	ISPDRV_U_TDNR_NOISE         = 0x8C,
	ISPDRV_U_TDNR_THR           = 0x8D,
    ISPDRV_S_TDNR_BUF           = 0x8E,  // refer and motion
    ISPDRV_R_TDNR_REFER         = 0x8F,
    ISPDRV_R_TDNR_MOTION        = 0x90,
	ISPDRV_GET_MIS				= 0x91,
	ISPDRV_CFG_DMA				= 0x92,
	ISPDRV_BYPASS_MCM			= 0x93,
	ISPDRV_SET_PPW_LINE_NUM     = 0x94,
	ISPDRV_GET_PPW_LINE_CNT     = 0x95,
	ISPDRV_CFG_DMA_LINE_ENTRY   = 0x96,
	ISPDRV_S_CROP   			= 0x97,
	ISPDRV_G_MIS_IRQADDR        = 0x98,
	ISPDRV_S_REG_SIZE           = 0x99,
	ISPDRV_S_CROP_BACKEND 		= 0x9a,
	ISPDRV_MI_CFG 				= 0x9b,
	ISPDRV_C_DPCC_BPT 			= 0x9c,
	ISPDRV_ENABLE_PDAF   		= 0x9d,
	ISPDRV_DISABLE_PDAF   		= 0x9e,
	ISPDRV_S_PDAF   			= 0x9f,
	ISPDRV_S_PP_PATH   			= 0xa0,
#ifdef ISP_CNR2
	ISPDRV_CNR2_ENABLE          = 0xa2,
	ISPDRV_CNR2_DISABLE         = 0xa3,
	ISPDRV_YNR_ENABLE           = 0xa4,
	ISPDRV_YNR_DISABLE          = 0xa5,
	ISPDRV_S_CNR2_STRENGTH      = 0xa6,
	ISPDRV_S_CNR2_FILTER        = 0xa7,
	ISPDRV_S_CNR2_TBL           = 0xa8,	
#endif
#ifdef ISP_WDR_V5
	ISPDRV_ENABLE_WDR5          = 0xa9,
	ISPDRV_DISABLE_WDR5         = 0xaa,
	ISPDRV_U_WDR5_CURVE         = 0xab,
	ISPDRV_S_WDR5               = 0xac,
#endif
    ISPDRV_S_HDRV2                  = 0xad,
    ISPDRV_ENABLE_HDRV2             = 0xae,
    ISPDRV_DISABLE_HDRV2            = 0xaf,
    ISPDRV_S_HDRV2_DPF              = 0xb0,
    ISPDRV_ENABLE_HDRV2_DPF         = 0xb1,
    ISPDRV_DISABLE_HDRV2_DPF        = 0xb2,
    ISPDRV_S_HDRV2_DEGHOST          = 0xb3,
    ISPDRV_ENABLE_HDRV2_DEGHOST 	= 0xb4,
    ISPDRV_DISABLE_HDRV2_DEGHOST	= 0xb5,
	ISPDRV_CFG_2DNR_LUMA_CURVE      = 0xb6,
	ISPDRV_CFG_MP_CTRL      		= 0xb7,
	ISPDRV_GET_MP_CTRL_STATUS		= 0xb8,
	ISPDRV_S_TDNR_RST               = 0xb9,
	ISPDRV_S_STA                    = 0xba,
	ISPDRV_ENABLE_RCCC              = 0xbb,
	ISPDRV_G_AFM_NEED               = 0xbc, //only for tilemode afm parameter pass
#ifdef ISP_3D_LUT
	ISPDRV_LUT3D_ENABLE				= 0xbd,
	ISPDRV_S_LUT3D_TAL				= 0xbe,
#endif
	ISPDRV_S_GTM					= 0xbf,
	ISPDRV_G_GTMDATA				= 0xc0,
	ISPDRV_CONFIG_SBI_MI            = 0xc1,
};

#define  ISP_GEN_CFG_UPDATE(dev)	{                                \
	uint32_t isp_ctrl = isp_read_reg(dev, REG_ADDR(isp_ctrl));       \
	REG_SET_SLICE(isp_ctrl, MRV_ISP_ISP_GEN_CFG_UPD, 1);			\
	isp_write_reg(dev, REG_ADDR(isp_ctrl), isp_ctrl);               \
	}

USE_TRACER(ISP_DRV_INFO);
USE_TRACER(ISP_DRV_WARN);
USE_TRACER(ISP_DRV_ERROR);

#define ISP_REG_GAP 4
int isp_drv_priv_cmd(struct isp_ic_dev *dev, unsigned int cmd, void *args);
int isp_drv_fpga_cmd(HalHandle_t HalHandle,unsigned int cmd, void *args);
long isp_copy_data(void *dst, void *src, int size);

#ifdef ISP_3D_LUT
int isp_lut3d_enable(struct isp_ic_dev *dev);
// int isp_lut3d_tbl_sel(struct isp_ic_dev *dev);
int isp_s_lut3d_tbl(struct isp_ic_dev *dev);
#endif

/* internal functions, can called by v4l2 video device and ioctl */
#ifdef ISP_CNR2
int isp_cnr2_enable(struct isp_ic_dev *dev);
int isp_cnr2_disable(struct isp_ic_dev *dev);
int isp_ynr_enable(struct isp_ic_dev *dev);
int isp_ynr_disable(struct isp_ic_dev *dev);
int isp_s_cnr2_strength(struct isp_ic_dev *dev);
int isp_s_cnr2_filter(struct isp_ic_dev *dev);
int isp_s_cnr2_tbl(struct isp_ic_dev *dev);
#endif
int isp_reset(struct isp_ic_dev *dev);
int isp_enable_tpg(struct isp_ic_dev *dev);
int isp_disable_tpg(struct isp_ic_dev *dev);
int isp_enable_bls(struct isp_ic_dev *dev);
int isp_disable_bls(struct isp_ic_dev *dev);
int isp_enable(struct isp_ic_dev *dev);
int isp_disable(struct isp_ic_dev *dev);
bool is_isp_enable(struct isp_ic_dev *dev);
int isp_enable_lsc(struct isp_ic_dev *dev);
int isp_disable_lsc(struct isp_ic_dev *dev);
int isp_enable_digital_gain(struct isp_ic_dev *dev);
int isp_s_digital_gain(struct isp_ic_dev *dev);
int isp_s_input(struct isp_ic_dev *dev);
int isp_s_demosaic(struct isp_ic_dev *dev);
int isp_s_tpg(struct isp_ic_dev *dev);
int isp_s_mcm_wr(struct isp_ic_dev *dev);
int isp_bypass_mcm(struct isp_ic_dev *dev);
int isp_s_mux(struct isp_ic_dev *dev);
int isp_s_bls(struct isp_ic_dev *dev);
int isp_enable_awb(struct isp_ic_dev *dev);
int isp_disable_awb(struct isp_ic_dev *dev);
int isp_s_awb(struct isp_ic_dev *dev);
int isp_g_awbmean(struct isp_ic_dev *dev, struct isp_awb_mean *mean);
int isp_s_is(struct isp_ic_dev *dev);
int isp_s_raw_is(struct isp_ic_dev *dev);
int isp_s_cnr(struct isp_ic_dev *dev);
int isp_start_stream(struct isp_ic_dev *dev, u32 framenum);
int isp_stop_stream(struct isp_ic_dev *dev);
int isp_s_cc(struct isp_ic_dev *dev);
int isp_s_xtalk(struct isp_ic_dev *dev);
int isp_enable_wb(struct isp_ic_dev *dev, bool bEnable);
int isp_enable_gamma_out(struct isp_ic_dev *dev, bool bEnable);
int isp_s_gamma_out(struct isp_ic_dev *dev);
int isp_ioc_disable_isp_off(struct isp_ic_dev *dev, void *args);
int isp_s_lsc_sec(struct isp_ic_dev *dev);
int isp_s_lsc_tbl(struct isp_ic_dev *dev);
int isp_s_dpf(struct isp_ic_dev *dev);
int isp_s_ee(struct isp_ic_dev *dev);
int isp_s_exp(struct isp_ic_dev *dev);
int isp_s_hdrexp(struct isp_ic_dev *dev);
int isp_g_expmean(struct isp_ic_dev *dev, u8 * mean);
int isp_g_hdrexpmean(struct isp_ic_dev *dev, u8 * mean);
int isp_s_hist(struct isp_ic_dev *dev);
int isp_g_histmean(struct isp_ic_dev *dev, u32 * mean);
int isp_s_hdrhist(struct isp_ic_dev *dev);
int isp_g_hdrhistmean(struct isp_ic_dev *dev, u32 * mean);
#ifdef ISP_HIST64
int isp_s_hist64(struct isp_ic_dev *dev);
int isp_g_hist64mean(struct isp_ic_dev *dev, u32 *mean);
int isp_g_hist64_vstart(struct isp_ic_dev *dev, u32 start_line);
int isp_update_hist64(struct isp_ic_dev *dev);
int isp_g_hist64_vstart_status(struct isp_ic_dev *dev, u32 *status);
#endif
int isp_s_gtm(struct isp_ic_dev *dev);
int isp_g_gtmdata(struct isp_ic_dev *dev, struct isp_gtm_sta *sta);
int isp_s_dpcc(struct isp_ic_dev *dev);
int isp_cfg_dpcc_bpt(struct isp_ic_dev *dev);
int isp_s_flt(struct isp_ic_dev *dev);
int isp_s_cac(struct isp_ic_dev *dev);
int isp_s_deg(struct isp_ic_dev *dev);
int isp_s_ie(struct isp_ic_dev *dev);
int isp_s_vsm(struct isp_ic_dev *dev);
int isp_g_vsm(struct isp_ic_dev *dev, struct isp_vsm_result *vsm);
int isp_s_afm(struct isp_ic_dev *dev);
int isp_g_afm(struct isp_ic_dev *dev, struct isp_afm_result *afm);
int isp_enable_wdr3(struct isp_ic_dev *dev);
int isp_disable_wdr3(struct isp_ic_dev *dev);
int isp_u_wdr3(struct isp_ic_dev *dev);
int isp_s_wdr3(struct isp_ic_dev *dev);
#ifdef ISP_WDR_V4
int isp_enable_wdr4(struct isp_ic_dev *dev);
int isp_disable_wdr4(struct isp_ic_dev *dev);
int isp_u_wdr4(struct isp_ic_dev *dev);
int isp_s_wdr4(struct isp_ic_dev *dev);
#endif
#ifdef ISP_WDR_V5
int isp_enable_wdr5(struct isp_ic_dev *dev);
int isp_disable_wdr5(struct isp_ic_dev *dev);
int isp_u_wdr5_curve(struct isp_ic_dev *dev);
int isp_s_wdr5(struct isp_ic_dev *dev);
#endif
int isp_s_sta(struct isp_ic_dev *dev);
int isp_s_exp2(struct isp_ic_dev *dev);
int isp_s_exp2_inputsel(struct isp_ic_dev *dev);
int isp_s_exp2_sizeratio(struct isp_ic_dev *dev, u32 ratio);
int isp_s_hdr(struct isp_ic_dev *dev);
int isp_s_hdr_wb(struct isp_ic_dev *dev);
int isp_s_hdr_bls(struct isp_ic_dev *dev);
//int isp_s_hdr_digal_gain(struct isp_ic_dev *dev);
int isp_enable_hdr(struct isp_ic_dev *dev);
int isp_disable_hdr(struct isp_ic_dev *dev);
#ifdef ISP_2DNR_V5
int isp_tdnr_s_2dnr(struct isp_ic_dev *dev);	
#else
int isp_s_2dnr(struct isp_ic_dev *dev);
#endif
#if defined(ISP_3DNR) || defined(ISP_3DNR_V2)
int isp_s_3dnr(struct isp_ic_dev *dev);
int isp_g_3dnr(struct isp_ic_dev *dev, u32 * avg);
int isp_u_3dnr(struct isp_ic_dev *dev, struct isp_3dnr_update *dnr3_update);
int isp_s_3dnr_motion(struct isp_ic_dev *dev);
int isp_s_3dnr_delta(struct isp_ic_dev *dev);
int isp_denoise3dnV32_update(struct isp_ic_dev *dev);

#endif

#if defined(ISP_3DNR) || defined(ISP_3DNR_V2_V1)
int isp_r_3dnr(struct isp_ic_dev *dev);
#endif
#ifdef ISP_3DNR_V2
int isp_s_3dnr_cmp(struct isp_ic_dev *dev);
#endif



int isp_u_3dnr_strength(struct isp_ic_dev *dev);
int isp_s_comp(struct isp_ic_dev *dev);
int isp_s_simp(struct isp_ic_dev *dev);
int isp_s_cproc(struct isp_ic_dev *dev);
int isp_s_elawb(struct isp_ic_dev *dev);
int isp_ioc_qcap(struct isp_ic_dev *dev, void *args);
int isp_ioc_g_status(struct isp_ic_dev *dev, void *args);

int isp_enable_gcmono(struct isp_ic_dev *dev);
int isp_disable_gcmono(struct isp_ic_dev *dev);
int isp_s_gcmono(struct isp_ic_dev *dev, struct isp_gcmono_data *data);	/* set curve */
int isp_enable_rgbgamma(struct isp_ic_dev *dev);
int isp_disable_rgbgamma(struct isp_ic_dev *dev);
int isp_s_rgbgamma(struct isp_ic_dev *dev, struct isp_rgbgamma_data *data);

u32 isp_read_mi_irq(struct isp_ic_dev *dev);
void isp_reset_mi_irq(struct isp_ic_dev *dev, u32 icr);

int isp_ioc_cfg_dma(struct isp_ic_dev *dev, void *args);
int isp_ioc_start_dma_read(struct isp_ic_dev *dev, void *args);
int isp_mi_cfg(struct isp_ic_dev *dev);
int isp_mi_start(struct isp_ic_dev *dev);
int isp_mi_stop(struct isp_ic_dev *dev);
int isp_set_buffer(struct isp_ic_dev *dev, struct isp_buffer_context *buf);
int isp_set_bp_buffer(struct isp_ic_dev *dev,
		      struct isp_bp_buffer_context *buf);

int isp_enable_dmsc(struct isp_ic_dev *dev);
int isp_disable_dmsc(struct isp_ic_dev *dev);
int isp_set_dmsc_intp(struct isp_ic_dev *dev);
int isp_set_dmsc_skin(struct isp_ic_dev *dev);
int isp_set_dmsc_gfilter(struct isp_ic_dev *dev);
int isp_set_dmsc_depurple(struct isp_ic_dev *dev);
int isp_set_dmsc_cac(struct isp_ic_dev *dev);
int isp_set_dmsc_sharpen(struct isp_ic_dev *dev);
int isp_set_dmsc_sharpen_line(struct isp_ic_dev *dev);
int isp_set_dmsc_dmoi(struct isp_ic_dev *dev);
int isp_s_dmsc(struct isp_ic_dev *dev);
int isp_s_ge(struct isp_ic_dev *dev);
int isp_s_ca(struct isp_ic_dev *dev);
int isp_set_crop_backend(struct isp_ic_dev *dev);
int isp_s_color_adjust(struct isp_ic_dev *dev);
int isp_config_dummy_hblank(struct isp_ic_dev *dev);

int isp_s_rgbir(struct isp_ic_dev *dev);
int isp_enable_rgbir(struct isp_ic_dev *dev);
int isp_rgbir_hw_init(struct isp_ic_dev *dev);
int isp_rgbir_s_ir_dnr(struct isp_ic_dev *dev);
int isp_rgbir_s_sharpen(struct isp_ic_dev *dev);
int isp_rgbir_s_des(struct isp_ic_dev *dev);
int isp_rgbir_s_cc_matrix(struct isp_ic_dev *dev);
int isp_rgbir_s_dpcc(struct isp_ic_dev *dev);
int isp_rgbir_s_gain(struct isp_ic_dev *dev);
int isp_rgbir_s_bls(struct isp_ic_dev *dev);
int isp_rgbir_out_ir_raw(struct isp_ic_dev *dev);

int isp_enable_rccc(struct isp_ic_dev *dev);

#ifdef ISP_3DNR_V3
int isp_s_tdnr(struct isp_ic_dev *dev);
int isp_tdnr_set_strength(struct isp_ic_dev *dev);
int isp_tdnr_set_motion(struct isp_ic_dev *dev);
int isp_tdnr_enable(struct isp_ic_dev *dev);
int isp_tdnr_disable(struct isp_ic_dev *dev);
int isp_tdnr_enable_tdnr(struct isp_ic_dev *dev);
int isp_tdnr_disable_tdnr(struct isp_ic_dev *dev);
int isp_tdnr_enable_2dnr(struct isp_ic_dev *dev);
int isp_tdnr_disable_2dnr(struct isp_ic_dev *dev);
int isp_tdnr_g_stats(struct isp_ic_dev *dev, struct isp_tdnr_stats *avg);
int isp_tdnr_u_noise(struct isp_ic_dev *dev);
int isp_r_tdnr_refer(struct isp_ic_dev *dev);
int isp_r_tdnr_motion(struct isp_ic_dev *dev);
int isp_tdnr_u_thr(struct isp_ic_dev *dev);
int isp_tdnr_s_buf(struct isp_ic_dev *dev);
int isp_tdnr_s_rst(struct isp_ic_dev *dev, bool reset);
#ifdef ISP_3DNR_V32
int isp_s_gamma_febe(struct isp_ic_dev *dev);
#else
int isp_tdnr_cfg_gamma(struct isp_ic_dev *dev);
#endif
#endif

#ifdef ISP_MI_PP_WRITE
int  isp_set_ppw_line_num(struct isp_ic_dev *dev, u16 line_num);
int  isp_get_ppw_pic_cnt(struct isp_ic_dev *dev, u16* pic_cnt);
#endif

#ifdef ISP_MI_PP_READ
int  isp_cfg_pp_dma_line_entry(struct isp_ic_dev *dev);
#endif
#ifdef __KERNEL__
int clean_dma_buffer(struct isp_ic_dev *dev);
irqreturn_t isp_hw_isr(int irq, void *data);
void isp_clear_interrupts(struct isp_ic_dev *dev);
#endif
/*get irq mis value from store array*/
u32 isp_read_mis(struct isp_ic_dev *dev, u32 irq_src);
int isp_ioc_read_mis(struct isp_ic_dev *dev, void *__user args);

/*set scaler*/
int isp_set_scaling(int id, struct isp_ic_dev *dev, bool stabilization, bool crop);
int isp_set_crop(struct isp_ic_dev *dev);

int isp_ioc_g_feature(struct isp_ic_dev *dev, void *args);
int isp_ioc_g_feature_veresion(struct isp_ic_dev *dev, void *args);

#ifdef ISP_PDAF
int isp_s_pdaf(struct isp_ic_dev *dev);
int isp_enable_pdaf(struct isp_ic_dev *dev);
int isp_disable_pdaf(struct isp_ic_dev *dev);
#endif

#ifdef ISP_HDR_STITCH2
int isp_s_hdr_v2(struct isp_ic_dev *dev);
int isp_enable_hdr_v2(struct isp_ic_dev *dev);
int isp_disable_hdr_v2(struct isp_ic_dev *dev);
int isp_s_hdr_v2_dpf(struct isp_ic_dev *dev);
int isp_enable_hdr_v2_dpf(struct isp_ic_dev *dev);
int isp_disable_hdr_v2_dpf(struct isp_ic_dev *dev);
int isp_s_hdr_v2_deghost(struct isp_ic_dev *dev);
int isp_enable_hdr_v2_deghost(struct isp_ic_dev *dev);
int isp_disable_hdr_v2_deghost(struct isp_ic_dev *dev);
#endif

int isp_s_pp_path(struct isp_ic_dev *dev);

#endif /* _ISP_DRV_H_ */
