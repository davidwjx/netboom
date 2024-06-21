/******************************************************************************\
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

/**
 * @file hal_common.h
 *
 * @brief
 *
 *
 * @note
 *
 *****************************************************************************/

#ifndef __HAL_COMMON_H__
#define __HAL_COMMON_H__

#include <ebase/trace.h>
#include <ebase/dct_assert.h>
#include <oslayer/oslayer.h>
#include <cam_device/cam_device_buf_defs.h>
#include <common/return_codes.h>

#ifdef __cplusplus
extern "C"
{
#endif


/** Interrupt handling mode switch.
  * If set to zero (0), the interrupt facilities built into the kernel driver
  * will not be used. Instead, the user-space application will poll for
  * interrupts by reading the masked interrupt status register repeatedly.
  * If set to one (1), the kernel driver will handle the interrupt requests by
  * the hardware. This is much more efficient, but requires the IRQ logic in the
  * hardware to work correctly. */
#define FPGA_USE_KERNEL_IRQ_HANDLING 0

#define HAL_FE_STATUS_REGS_MAX       (12)
#define HAL_FE_TBL_REG_MAX           (17)
/******************************************************************************
 * local macro definitions
 *****************************************************************************/
typedef enum HalIsrSrc_e
{
    eHalIsrSrcIspIrq         = 0x0000,
    eHalIsrSrcJpeStatusIrq,
    eHalIsrSrcJpeErrorIrq,
    eHalIsrSrcCamIcMiIrq,
    eHalIsrSrcCamIcMi1Irq,
    eHalIsrSrcCamIcMi2Irq,
    eHalIsrSrcCamIcMi3Irq,
    eHalIsrSrcCamIcMipiIrq,
    eHalIsrSrcCamIcIspStitchingIrq,
    eHalIsrSrcCamIcHdr1Irq,
    eHalIsrMax,
} HalIsrSrc_t;


typedef enum HalChId_s
{
    HAL_CH_ID0 = 0, //!<isp channel id 0.
    HAL_CH_ID1 = 1, //!<isp channel id 1.
    HAL_CH_ID2 = 2, //!<isp channel id 2.
    HAL_CH_ID3 = 3, //!<isp channel id 3.
    HAL_CH_NUM      //!<isp channel number.
}HalChId_e;

typedef enum HalIspPipeId_s
{
    HAL_ISP_HW_PIPE_ID0 = 0, //!<isp HW pipeline id 0.
    HAL_ISP_HW_PIPE_ID1 = 1, //!<isp HW pipeline id 0.
    HAL_ISP_HW_PIPE_NUM      //!<isp HW pipeline total number.
}HalIspPipeId_e;


/******************************************************************************
 * local type definitions
 *****************************************************************************/
#if defined ( HAL_ALTERA )

/* IRQ handle type. */
typedef struct _fpga_irq_handle {
#if FPGA_USE_KERNEL_IRQ_HANDLING
        int __dummy;
#else
        uint32_t mis_addr;
        uint32_t cis_addr;
        uint32_t timeout;
        volatile int cancel;
        osMutex poll_mutex;
#endif
} fpga_irq_handle_t;


#endif

/*****************************************************************************/
/**
 * @brief   handle to hal instance
 *****************************************************************************/
typedef void * HalHandle_t;

/*****************************************************************************/
/**
 * @brief   hal irq context
 *****************************************************************************/
struct HalIrqCtx_s                                  // note: a forward declaration was given in this file before!
{
    HalHandle_t         HalHandle;                  /**< hal handle this context belongs to; must be set by callee prior connection of irq! */
    uint32_t            misRegAddress;              /**< address of the masked interrupt status register (MIS); must be set by callee prior connection of irq! */
    uint32_t            icrRegAddress;              /**< address of the interrupt clear register (ICR); must be set by callee prior connection of irq! */

    osInterrupt         OsIrq;                      /**< os layer abstraction for the interrupt */
    uint32_t            misValue;                   /**< value of the MIS-Register */

#if defined ( HAL_ALTERA )
    fpga_irq_handle_t   AlteraIrqHandle;            /**< handle for multiple interrupt handler */
#endif

    HalIsrSrc_t   irq_src;
    void*         hBinder;                      /**< handle for binder context*/

};

/*****************************************************************************/
/**
 * @brief   hal irq context
 *****************************************************************************/
typedef struct HalIrqCtx_s HalIrqCtx_t; // implicit forward declaration of struct HalIrqCtx_s

//FE header

//Define for FE handle
#define FE_OFFSET 0x8000

typedef void* HalFEHandle_t;


typedef struct FEReg_s
{
	uint32_t isp_ctrl; /*register offset*/
	uint32_t isp_tpg_ctrl;/*register offset*/
	uint32_t isp_mis;/*register offset*/
	uint32_t isp_icr;/*register offset*/

	uint32_t mi_ctrl;/*register offset*/
	uint32_t mi_mis;/*register offset*/
	uint32_t mi_mis1;/*register offset*/
	uint32_t mi_mis2;/*register offset*/
	uint32_t mi_mis3;/*register offset*/
	uint32_t mi_icr;/*register offset*/
	uint32_t mi_icr1;/*register offset*/
	uint32_t mi_icr2;/*register offset*/
	uint32_t mi_icr3;/*register offset*/
	uint32_t mi_imsc3;/*register offset*/
	uint32_t mi_mcm_bus_id;/*register offset*/
	uint32_t mi_init;/*register offset*/
	uint32_t mi_hdr_ctrl;/*register offset*/

	uint32_t isp_hist;/*register offset*/
	uint32_t isp_stitching_ctrl;/*register offset*/
	uint32_t isp_stitching_mis;/*register offset*/
	uint32_t isp_stitching_icr;/*register offset*/
	uint32_t isp_stitching_ctrl1;/*register offset*/
	uint32_t isp_stitching_ctrl2;/*register offset*/
	uint32_t isp_stitching_ctrl3;/*register offset*/
	uint32_t mipi_mis;/*register offset*/
	uint32_t mipi_icr;/*register offset*/

	uint32_t denoise3d_ctrl[3];

	uint32_t fe_ctrl;/*register offset*/
	uint32_t fe_dma_start;/*register offset*/
	uint32_t fe_dma_ad;/*register offset*/
	uint32_t fe_imsc;/*register offset*/
	uint32_t fe_mis;/*register offset*/
	uint32_t fe_icr;/*register offset*/

	uint32_t mi_init_mask;
	uint32_t mi_ctrl_mask;
	uint32_t mi_imsc3_mask;
	uint32_t isp_ctrl_mask[4];/*register mask bit*/
	uint32_t stitching_ctrl_mask[3];
	uint32_t isp_tpg_ctrl_mask;/*register mask bit*/
	uint32_t mi_mcm_bus_id_mask[4];
	uint32_t mi_mcm_bus_id_shift[3];
	uint32_t mi_hdr_ctrl_mask[2];
	uint32_t denoise3d_ctrl_mask[4];
	uint32_t isp_wo_bits_mask[4];
}FEReg_t;

typedef struct FETbl_s {
	uint32_t tbl_reg_offset;
	uint16_t params_num;
}FETbl_t;

typedef struct FEStatusReg_s {
	uint32_t reg_base;
	uint32_t reg_num;
	uint32_t reg_step;//4/8/12/...
}FEStatusReg_t;

typedef struct FERegParams_s {
	FEReg_t general_ctrl;

	uint8_t tbl_reg_addr_num; /*number of register address for LUT*/
	uint16_t tbl_total_params_num; /*the total number of params for LUT*/
	FETbl_t tbl[HAL_FE_TBL_REG_MAX];

	uint8_t status_regs_num;
	FEStatusReg_t status_regs[HAL_FE_STATUS_REGS_MAX];
}FERegParams_t;

typedef struct FEStatus_s
{
    uint8_t enable;
    uint32_t status;
    uint32_t curr_channel;
}FEStatus_t;

typedef struct FEVdInfo_s
{
    uint32_t work_mode;
	uint32_t vdid;
}FEVdInfo_t;

typedef struct FECtrl_s
{
    FEVdInfo_t curr_vd;
	FEVdInfo_t next_vd;
}FECtrl_t;

typedef struct FEIniCfg_s
{
    const char *fe_name;
    uint32_t max_vd_caps;
    FERegParams_t fe_params;
}FEIniCfg_t;

typedef struct FEFusaCmdBuff_s
{
    uint32_t cmd_buff_num;      /*number of command parameters*/
    uint32_t cmd_buff_phy_base; /*physical linear continue base address*/
                                /*of command buffer*/
}FEFusaCmdBuff_t;

typedef enum FUSA_VIDEO_TYPE_s
{
    FE_FUSA_VIDEO_TYPE_STL = 0,
    FE_FUSA_VIDEO_TYPE_DP  = 1
} FusaVideoType_e;

typedef struct FEFusaCtrl_s
{
    uint32_t curr_vdid;
    uint32_t next_vdid;
    FusaVideoType_e fusa_video_type;        /*STL(0x01)/DP(0x02)*/
    FEFusaCmdBuff_t fusa_cmd_buff;
}FEFusaCtrl_t;

typedef struct HalFEContext_s
{
    osMutex     modMutex;   //!< common short term mutex; e.g. for read-modify-write accesses
    uint32_t    refCount;   //!< internal ref count
    uint32_t    hpId;//!< hw pipeline id==feId
    int32_t     feFd;
    uint32_t    max_vd_caps;
} HalFEContext_t;

typedef struct __HalBuffPoolCtx__
{
    BufIdentity bufPoolMp[BUFF_POOL_MAX_OUTPUT_BUF_NUMBER];
    BufIdentity bufPoolSP1[BUFF_POOL_MAX_OUTPUT_BUF_NUMBER];
    BufIdentity bufPoolSP2[BUFF_POOL_MAX_OUTPUT_BUF_NUMBER];
    BufIdentity bufPoolMCMWR[BUFF_POOL_MAX_OUTPUT_BUF_NUMBER];
#ifdef ISP_MI_HDR
    BufIdentity bufPoolHDR[BUFF_POOL_MAX_OUTPUT_BUF_NUMBER];
#endif
    BufIdentity bufPoolWriteMax[BUFF_POOL_MAX_OUTPUT_BUF_NUMBER];
    BufIdentity bufPoolRead[BUFF_POOL_MAX_INPUT_BUF_NUMBER];
    uint32_t    bufPoolMpNum;
    uint32_t    bufPoolSP1Num;
    uint32_t    bufPoolSP2Num;
    uint32_t    bufPoolMCMWRNum;
#ifdef ISP_MI_HDR
    uint32_t    bufPoolHDRNum;
#endif
    uint32_t    bufPoolWriteMaxNum;
    uint32_t    bufPoolReadNum;
}HalBuffPoolCtx;

RESULT HalUserReadMemory( HalHandle_t HalHandle, uint32_t phy_address, uint8_t* p_read_buffer, uint32_t byte_size );
RESULT HalUserWriteMemory( HalHandle_t HalHandle, uint32_t phy_address, uint8_t* p_write_buffer, uint32_t byte_size );

RESULT HalIplMap(HalHandle_t HalHandle , BufIdentity * bufIdentify);
RESULT HalIplUnMap(HalHandle_t HalHandle , BufIdentity * bufIdentify);

BufIdentity* HalGetPoolList(HalHandle_t HalHandle, ISPCORE_BUFIO_ID chain);
uint32_t* HalGetPoolNumHandle(HalHandle_t HalHandle, ISPCORE_BUFIO_ID chain);
uint32_t HalGetPoolBufferSize(HalHandle_t HalHandle, ISPCORE_BUFIO_ID chain);

RESULT HalSetBuffMode(HalHandle_t HalHandle, BUFF_MODE buf_work_mode);
BUFF_MODE HalGetBuffMode(HalHandle_t HalHandle);

HalFEHandle_t HalFEOpen( int hpId );
RESULT HalFEClose( HalFEHandle_t feHandle );
RESULT HalFEInit(HalFEHandle_t HalHandle, FEIniCfg_t  *ctx);
RESULT HalFEDeinit(HalFEHandle_t HalHandle);
HalFEHandle_t HalFEGetHandle( int hpId );
RESULT HalFEGetStatus(HalFEHandle_t HalHandle, FEStatus_t *fe_status);
RESULT HalFESwtich(HalFEHandle_t HalHandle, FECtrl_t *fe_ctrl);
RESULT HalFEInsertFusaFrm(HalFEHandle_t HalHandle, FEFusaCtrl_t *fusa_ctrl);
RESULT HalFEReadRegister(HalFEHandle_t HalHandle, uint32_t vdId, uint32_t address, uint32_t *data);
RESULT HalFEWriteRegister(HalFEHandle_t HalHandle, uint32_t vdId, uint32_t address, uint32_t data);
RESULT HalFEWriteTable(HalFEHandle_t HalHandle, uint32_t vdId, uint32_t address, uint32_t data);

#ifdef __cplusplus
}
#endif
#endif /* __HAL_COMMON_H__ */
