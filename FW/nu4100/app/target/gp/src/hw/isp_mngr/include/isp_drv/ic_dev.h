/****************************************************************************
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 VeriSilicon Holdings Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************
 *
 * The GPL License (GPL)
 *
 * Copyright (c) 2020 VeriSilicon Holdings Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program;
 *
 *****************************************************************************
 *
 * Note: This software is released under dual MIT and GPL licenses. A
 * recipient may use this file under the terms of either the MIT license or
 * GPL License. If you wish to use only one license not the other, you can
 * indicate your decision by deleting one of the above license notices in your
 * version of this file.
 *
 *****************************************************************************/
#ifndef _IC_DEV_H_
#define _IC_DEV_H_

#ifndef __KERNEL__
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

#define copy_from_user(a, b, c) isp_copy_data(a, b, c)
#define copy_to_user(a, b, c) isp_copy_data(a, b, c)

#if defined(HAL_CMODEL) || defined(HAL_ALTERA)
#include <hal/hal_api.h>
void ic_set_hal(HalHandle_t hal);
#endif
#endif

#include "isp_irq_queue.h"
#include "vvdefs.h"

#define IRQADDR(x) (pirqaddr->x##_offset)
#define UPDATESHADOWS 0

#ifdef ENABLE_IRQ
extern u32 ISP_BUF_GAP;
#define ISP_MI_PATH_ID_MAX 9
#endif

// #ifndef ISP_LSC_V2_TILE_MODE
// #define ISP_LSC_V2_TILE_MODE
// #endif

#define ISP_FE_FULL_BUFFER_BYTE_MAX     (0x7200) /* non-ctrl + ctrl + special registers */
#define ISP_FE_REG_OFFSET_BYTE_MAX		(0x7050)
#define ISP_FE_REG_SIZE 				(4) /* Byte */
#define ISP_FE_FULL_BUFFER_NUM  		(ISP_FE_FULL_BUFFER_BYTE_MAX/ISP_FE_REG_SIZE)
#define ISP_FE_REG_NUM					(ISP_FE_REG_OFFSET_BYTE_MAX/ISP_FE_REG_SIZE)
#define ISP_FE_REG_PART_REFRESH_NUM 	(0x200)
#define ISP_FE_FIXED_OFFSET_READ_NUM	(256)
#define ISP_FE_STATUS_REGS_MAX			(12)
#define ISP_FE_TBL_REG_MAX				(17)
#define ISP_FE_VIRT_MAXCNT				(4)
#define ISP_FE_SPECIAL_RGE_NUM			(2)
#define ISP_FE_SPECIAL_RGE_INDEX		(ISP_FE_FULL_BUFFER_NUM - ISP_FE_SPECIAL_RGE_NUM)

struct isp_reg_t {
	u32 offset;
	u32 val;
};

struct isp_reg_mem_info {
	u64 phy_base;
	u32 reg_size;
};

struct isp_misirq_regaddr {
	u32 isp_ctrl_offset;
	u32 isp_mis_offset;
	u32 isp_icr_offset;
	u32 isp_imsc_offset;

	u32 miv_mis_offset;
	u32 miv_icr_offset;
	u32 miv2_mis1_offset;
	u32 miv2_icr1_offset;
	u32 miv2_mis2_offset;
	u32 miv2_icr2_offset;
	u32 miv2_mis3_offset;
	u32 miv2_icr3_offset;
	u32 mi_mis_hdr1_offset;
	u32 mi_icr_hdr1_offset;
#ifdef ENABLE_IRQ
	u32 mi_status_offset;
#endif
};

struct isp_mi_data_path_context {
	bool enable;
	u32 out_mode;	   /**< output format */
	u32 in_mode;		/**< input format */
	u32 data_layout;	/**< layout of data */
	u32 data_alignMode;	/**< align mode of data */
	u32 in_width;
	u32 in_height;
	u32 out_width;
	u32 out_height;
	bool hscale;
	bool vscale;
	int pixelformat;
	u32 yuv_bit;
	u32 tilemode_crop_width;	//using for calculate llength,tilemode_crop_width = isp0-crop-outwidth + isp1-crop-outwidth, isp0 and isp1 are same.
	int mi_remainder;
};

#ifdef ENABLE_IRQ
struct isp_vvcam_mi_context {
	struct isp_mi_data_path_context path[ISP_MI_PATH_ID_MAX];
	u32 burst_len;
	u32 mi_path_num;
	bool tileMode;		//indicate whether if mi need to work in tile mode.
};

struct isp_mrv_mi_irq_mask_context {
	u32 FrameEndMask;
	u32 ErrorMask;
	u32 FifoFullMask;
};
#endif

struct isp_gamma_out_context {
	bool enableWB, enableGamma;
	bool changed;
	u32 mode;
	u32 curve[17];
};

struct isp_extmem_info {
	u64 addr;
	u64 size;
	u64 user_rmem_size;
};

struct isp_buffer_context {
	u32 type;
	u32 path;
	u32 addr_y, addr_cb, addr_cr;
	u32 size_y, size_cb, size_cr;
};

struct isp_miv_info {
	bool miv2_open;
	bool mis2_open;
};

struct isp_irq_data {
	uint32_t addr;
	uint32_t val;
	uint32_t nop[14];
};

enum isp_fe_state {
	ISP_FE_STATE_INIT,
	ISP_FE_STATE_READY,
	ISP_FE_STATE_RUNNING,
	ISP_FE_STATE_WAITING,
	ISP_FE_STATE_EXIT
};

enum fe_work_mode_e {
	ISP_FE_WORK_MODE_BYPASS = 0,
	ISP_FE_WORK_MODE_MCM = 1,
	ISP_FE_WORK_MODE_TEST = 2,
	ISP_FE_WORK_MODE_MAX
};

enum isp_fe_cmd_type_e {
	ISP_FE_CMD_REG_READ = 0, /**< unused */
	ISP_FE_CMD_REG_WRITE = 1,/**< Write immediate values into SW registers of IP */
	ISP_FE_CMD_END = 2,	/**< The last command in this command buffer */
	ISP_FE_CMD_NOP = 3	/**< Add an empty command to */
};

union cmd_wreg_u {
	struct cmd_wreg_t {
		uint32_t start_reg_addr:16;	/**< [15:0] Specify the address of
						   the first SW register to be writen*/
		uint32_t length:10;		/**< [25:16] Specify how many SW
						   registers (32 bits for each) should
						   be writen; 0:1024, 1:1, ?? , 1023:1023;
						   length is fixed to 1 in current driver*/
		uint32_t rsv:1;			/**< [26] Reserved for future */
		uint32_t op_code:5;		/**< [31:27] opcode = 5??h01 */
	} cmd;
	uint32_t all;
};

struct isp_fe_cmd_wreg_t {
	union cmd_wreg_u fe_cmd;
	uint32_t w_data;
};

union cmd_end_u {
	struct cmd_end_t {
		uint32_t event_id:5;			/**< [4:0] ID indicator when this cmd is
								executed, set as 5'h1A*/
		uint32_t rsv0:3;			/**< [7:5] Reserved for future*/
		uint32_t event_enable:1;		/**< [8] Event enable when this cmd is
								 executed, set as 1'b1 */
		uint32_t rsv1:18;			/**< [26:9] Reserved for future*/
		uint32_t op_code:5;			/**< [31:27]opcode = 5??h02 */
	} cmd;
	uint32_t all;
};

struct isp_fe_cmd_end_t {
	union cmd_end_u fe_cmd;
	uint32_t rsv2;					/**< Reserved for future */
};

union cmd_nop_u {
	struct cmd_nop_t {
		uint32_t rsv0:27;			/**< [26:0] Reserved for future*/
		uint32_t op_code:5;				/**< [31:27]opcode = 5??h03 */
	} cmd;
	uint32_t all;
};

struct isp_fe_cmd_nop_t {
	union cmd_nop_u fe_cmd;
	uint32_t rsv1;					/**< Reserved for future */
};

union isp_fe_cmd_u {
	struct isp_fe_cmd_wreg_t cmd_wreg;
	struct isp_fe_cmd_end_t cmd_end;
	struct isp_fe_cmd_nop_t cmd_nop;
};

struct isp_fe_cmd_buffer_t {
	//struct list_head		cmd_buffer_entry;
	uint32_t curr_cmd_num;
	uint32_t cmd_num_max;
	union isp_fe_cmd_u *cmd_buffer;
#ifdef __KERNEL__
	dma_addr_t cmd_dma_addr;
#endif
};

struct isp_fe_status_regs_t {
	uint32_t reg_base;
	uint32_t reg_num;
	uint32_t reg_step;	//4/8/12/...
};

struct isp_fe_virt_reg_t {
	uint8_t vdid;
	uint32_t offset;
	uint32_t val;
};

/*
 *   CMD Buffer
 * +------------+
 * |            |
 * |            |
 * |  Register  |
 * |    Buffer  |
 * |            |
 * +------------+          +------------------+
 * |            |<---------+   tbl_cmd_buffer0|
 * |            |          +------------------+
 * |    LUT     |<---------+   tbl_cmd_buffer1|
 * |    Buffer  |          +------------------+
 * |            |<---------+       ...        |
 * |            |          +------------------+
 * |            |<---------+   tbl_cmd_bufferx|
 * +------------+          +------------------+
 */

struct isp_fe_tbl_buffer_t {
	uint16_t params_num;
	uint32_t tbl_reg_offset;
	uint16_t curr_index;
	struct isp_fe_cmd_wreg_t *tbl_cmd_buffer;
};

struct isp_fe_reg_t {
	uint32_t isp_ctrl;	/*register offset */
	uint32_t isp_tpg_ctrl;	/*register offset */
	uint32_t isp_mis;	/*register offset */
	uint32_t isp_icr;	/*register offset */

	uint32_t mi_ctrl;	/*register offset */
	uint32_t mi_mis;	/*register offset */
	uint32_t mi_mis1;	/*register offset */
	uint32_t mi_mis2;	/*register offset */
	uint32_t mi_mis3;	/*register offset */
	uint32_t mi_icr;	/*register offset */
	uint32_t mi_icr1;	/*register offset */
	uint32_t mi_icr2;	/*register offset */
	uint32_t mi_icr3;	/*register offset */
	uint32_t mi_imsc3;	/*register offset */
	uint32_t mi_mcm_bus_id;	/*register offset */
	uint32_t mi_init;	/*register offset */
	uint32_t mi_hdr_ctrl;	/*register offset */

	uint32_t isp_hist;	/*register offset */
	uint32_t isp_stitching_ctrl;	/*register offset */
	uint32_t isp_stitching_mis;	/*register offset */
	uint32_t isp_stitching_icr;	/*register offset */
	uint32_t isp_stitching_ctrl1;	/*register offset */
	uint32_t isp_stitching_ctrl2;	/*register offset */
	uint32_t isp_stitching_ctrl3;	/*register offset */
	uint32_t mipi_mis;	/*register offset */
	uint32_t mipi_icr;	/*register offset */

	uint32_t denoise3d_ctrl[3];

	uint32_t fe_ctrl;	/*register offset */
	uint32_t fe_dma_start;	/*register offset */
	uint32_t fe_dma_ad;	/*register offset */
	uint32_t fe_imsc;	/*register offset */
	uint32_t fe_mis;	/*register offset */
	uint32_t fe_icr;	/*register offset */

	uint32_t mi_init_mask;
	uint32_t mi_ctrl_mask;
	uint32_t mi_imsc3_mask;
	uint32_t isp_ctrl_mask[4];	/*register mask bit */
	uint32_t stitching_ctrl_mask[3];
	uint32_t isp_tpg_ctrl_mask;	/*register mask bit */
	uint32_t mi_mcm_bus_id_mask[4];
	uint32_t mi_mcm_bus_id_shift[3];
	uint32_t mi_hdr_ctrl_mask[2];
	uint32_t denoise3d_ctrl_mask[4];
	uint32_t isp_wo_bits_mask[4];
};

struct isp_fe_tbl_t {
	uint32_t tbl_reg_offset;
	uint16_t params_num;
};

struct isp_fe_params_t {
	uint32_t max_vd_caps;
	struct isp_fe_reg_t general_ctrl;

	uint8_t tbl_reg_addr_num;	/*number of register address for LUT */
	uint16_t tbl_total_params_num;	/*the total number of params for LUT */
	struct isp_fe_tbl_t tbl[ISP_FE_TBL_REG_MAX];

	uint8_t status_regs_num;
	struct isp_fe_status_regs_t status_regs[ISP_FE_STATUS_REGS_MAX];

};

struct isp_fe_status_t {
	uint8_t enable;
	uint32_t status;
	uint32_t curr_channel;
};

struct isp_fe_switch_t {
	uint32_t vd_mode;
	uint32_t next_vdid[2];
};

struct isp_fe_buff_t {
#ifdef __KERNEL__
	spinlock_t cmd_buffer_lock;
#endif
	uint8_t prev_index; /* Speed up the search for LUT buffer index */
	uint8_t tbl_reg_addr_num;	/*number of register address for LUT */
	uint16_t tbl_total_params_num;	/*the total number of params for LUT */
	struct isp_fe_tbl_buffer_t tbl_buffer[ISP_FE_TBL_REG_MAX];

	struct isp_fe_cmd_buffer_t refresh_full_regs;
	struct isp_fe_cmd_buffer_t refresh_part_regs;
	struct isp_fe_cmd_buffer_t refresh_fusa_regs;
	uint32_t rd_index;
	uint32_t fixed_reg_rd_num;	/*statics register which has the fixed offset */
	uint32_t *fixed_reg_buffer; /* only store the value of statics register
						which has the fixed offset */

	uint8_t isp_ctrl_enable;
	uint8_t isp_mi_enable;
};

struct isp_fe_context {
	bool enable;
	enum fe_work_mode_e work_mode;
#ifdef __KERNEL__
	struct completion fe_completion;
	spinlock_t full_buff_lock;
#endif
	enum isp_fe_state state;
	bool fst_wr_flag;	//first write
	struct isp_fe_reg_t general_ctrl;

	uint32_t *reg_buffer;	/* Store the default value of isp registers.
							It can use the cached buffer */
	uint8_t running_num;	/* number of isp virtual channels
							which are running*/
	bool virt_running[ISP_FE_VIRT_MAXCNT];

	uint8_t vdid_num;		/* the total number of virtual number */
	uint8_t prev_vdid;
	uint8_t curr_vdid;
	struct isp_fe_buff_t *fe_buff;

	uint16_t *hash_map_tbl; /*index table for mapping register offset
							to the full command buffer index*/

	uint8_t status_regs_num;
	struct isp_fe_status_regs_t status_regs[ISP_FE_STATUS_REGS_MAX];

	u64 last_t_ns;
};

struct vvcam_fe_dev {
	void __iomem *base;
	void __iomem *reset;
	u64 phybase;
	struct device *dev;

	u32 isp_mis;
	int id;

	struct isp_fe_context fe;
	int (*fe_get_vdid) (struct vvcam_fe_dev *dev, uint8_t *vd_id);
	int (*fe_dma_irq) (struct vvcam_fe_dev *dev);
	void (*fe_isp_irq_work) (struct vvcam_fe_dev *dev);
};


struct vvcam_ic_dev {
	void __iomem *base;
	void __iomem *reset;
	void *rmem;
	u64 user_rmem_size;
	u64 phybase;
	struct device *dev;
#ifdef ISP8000NANO_V1802
	struct regmap *mix_gpr;
#endif
#if defined(__KERNEL__) && defined(ENABLE_IRQ)
	struct vvbuf_ctx *bctx;
	struct vb2_dc_buf *mi_buf[ISP_MI_PATH_ID_MAX];
	struct vb2_dc_buf *mi_buf_shd[ISP_MI_PATH_ID_MAX];
	int (*alloc) (struct vvcam_ic_dev *dev,
			  struct isp_buffer_context *buf);
	int (*free) (struct vvcam_ic_dev *dev, struct vb2_dc_buf *buf);
	int *state;
#endif
	void (*post_event) (struct vvcam_ic_dev *dev, void *data, size_t size);
#if UPDATESHADOWS
	struct isp_gamma_out_context gamma_out;
	bool update_gamma_en;
#endif
	struct isp_misirq_regaddr irqaddr;
	u32 isp_mis;
	int id;
#ifdef ENABLE_IRQ
	struct isp_vvcam_mi_context mi;
	struct isp_mrv_mi_irq_mask_context irq_mask;
#endif
	struct isp_miv_info miv_info;

	isp_mis_list_t circle_list;	//The irq circle list
	int set_mivirqaddr;

	struct vvcam_fe_dev *fe;//registered by fe driver
	int refcount;
};

void ic_write_reg(struct vvcam_ic_dev *dev, u32 offset, u32 val);
u32 ic_read_reg(struct vvcam_ic_dev *dev, u32 offset);
void ic_write_tbl(struct vvcam_ic_dev *dev, u32 offset, u32 val);

#endif /* _IC_DEV_H_ */
