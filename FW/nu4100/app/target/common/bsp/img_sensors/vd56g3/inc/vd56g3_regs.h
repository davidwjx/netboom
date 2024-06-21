#ifndef __VD56G3_REGS_H__
#define __VD56G3_REGS_H__

/* ------Sensor Registers------ */
/* Status Registers */
#define ST_REG_DEVICE_MODEL_ID              0x0
#define ST_REG_FWPATCH_REVISION             0x001e
#define ST_REG_VTPATCH_ID                   0x0020
#define ST_REG_SYSTEM_FSM                   0x0028
#define ST_REG_SYSTEM_PLL_CLK               0x0038
#define ST_REG_PIXEL_CLK                    0x003c
#define ST_REG_TEMPERATURE                  0x004c
#define ST_REG_FRAME_RATE                   0x004e
#define ST_REG_APPLIED_COARSE_EXPOSURE      0x0064
#define ST_REG_APPLIED_ANALOG_GAIN          0x0068
#define ST_REG_APPLIED_DIGITAL_GAIN         0x006a
#define ST_REG_AE_MODE                      0x0072
#define ST_REG_LINE_LENGTH                  0x0076
#define ST_REG_FRAME_LENGTH                 0x0078
#define ST_REG_Y_START                      0x007e
#define ST_REG_Y_END                        0x0080
#define ST_REG_READOUT_CTRL                 0x0086
/* Command Registers */
#define CMD_REG_BOOT                        0x0200
#define CMD_REG_SW_STBY                     0x0201
#define CMD_REG_STREAMING                   0x0202
#define CMD_REG_VTPATCHING                  0x0203
/* Sensor Settings Registers */
#define SS_REG_EXT_CLOCK                    0x0220
#define SS_REG_DEVICE_I2C_CTRL              0x0230
/* Static Registers */
#define STATIC_REG_LINE_LENGTH              0x0300
#define STATIC_REG_ORIENTATION              0x0302
#define STATIC_REG_VT_CTRL                  0x0309
/* Dynamic Registers */
#define DYN_REG_PATGEN_CTRL                 0x0400
#define DYN_REG_READ_OFFSET                 0x0444
#define DYN_REG_GROUP_PARAM_HOLD            0x0448
/* Context Registers */
#define CONTEXT_0_REG_EXP_MODE              0x044c
#define CONTEXT_0_REG_MANUAL_ANALOG_GAIN    0x044d
#define CONTEXT_0_REG_MANUAL_COARSE_EXP     0x044e
#define CONTEXT_0_REG_MANUAL_DIGITAL_GAIN   0x0450
#define CONTEXT_0_REG_FRAME_LENGTH          0x0458
#define CONTEXT_0_REG_OUT_ROI_X_START       0x045e
#define CONTEXT_0_REG_OUT_ROI_X_END         0x0460
#define CONTEXT_0_REG_OUT_ROI_Y_START       0x0462
#define CONTEXT_0_REG_OUT_ROI_Y_END         0x0464
#define CONTEXT_0_REG_GPIO_0_CTRL           0x0467
#define CONTEXT_0_REG_GPIO_1_CTRL           0x0468
/* NVM Registers */
/* Miscellaneous Registers */
#define MISC_REG_EXP_COARSE_INTG_MARGIN     0x0946
#define MISC_REG_DPHYTX_CTRL                0x0956
#define MISC_REG_MAX_DG                     0x095e
#define MISC_REG_MAX_AG_CODED               0x0960
#define MISC_REG_MIN_DG                     0x097c
#define MISC_REG_MIN_AG_CODED               0x097e
/* FW Patch Registers */
#define FW_PATCH_REG_BUF                    0x2000

/* ------Sensor register fields------ */
typedef enum {
        SYSTEM_FSM_HW_STBY = 0,
        SYSTEM_FSM_READY_TO_BOOT = 1,
        SYSTEM_FSM_SW_STBY = 2,
        SYSTEM_FSM_STREAMING = 3,
        SYSTEM_FSM_ERROR   = 0xFF
}System_FSM_e;



typedef enum {
        NORMAL_STREAMING = 0,
        DIGITAL_BINNING_2 = 1,
        DIGITAL_BINNING_4 = 2,
        SUBSAMPLING_2 = 3,
        SUBSAMPLING_4 = 4,
}Readout_ctrl_e;

#define COMMAND_BOOT                    0x1
#define COMMAND_PATCH_SETUP             0x2

#define COMMAND_START_STREAM            0x1
#define COMMAND_NVM_READ                0x2
#define COMMAND_NVM_PROG                0x3
#define COMMAND_THSENS_READ             0x4
#define COMMAND_I2C_ADDR_UPDATE         0x5

#define COMMAND_STOP_STREAM             0x1
#define COMMAND_VT_FSYNC_IN_I2C         0x2

#define COMMAND_START_VTRAM_UPDATE      0x1
#define COMMAND_END_VTRAM_UPDATE        0x2

#define IMAGE_NO_FLIP                   0x0
#define IMAGE_X_FLIP                    0x1
#define IMAGE_Y_FLIP                    0x2
#define IMAGE_XY_FLIP                   0x3

#define TEST_PAT_TYPE_NOPAT             0x0
#define TEST_PAT_TYPE_SOLID             0x1
#define TEST_PAT_TYPE_VBAR              0x2
#define TEST_PAT_TYPE_GRADBAR           0x3
#define TEST_PAT_TYPE_HGREY             0x10
#define TEST_PAT_TYPE_VGREY             0x11
#define TEST_PAT_TYPE_DGREY             0x12
#define TEST_PAT_TYPE_PN28              0x13
#define TEST_PAT_TYPE_SMEAR             0x14

#define VT_MODE_SLAVE                   0x1

#define GPIO_CTRL_MODE_RESERVED         0x0
#define GPIO_CTRL_MODE_GPIIO_IN         0x1
#define GPIO_CTRL_MODE_STROBE           0x2
#define GPIO_CTRL_MODE_PWM_STROBE       0x3
#define GPIO_CTRL_MODE_PWM              0x4
#define GPIO_CTRL_MODE_GPIO_OUT         0x5
#define GPIO_CTRL_MODE_VSYNC_OUT_0      0x6
#define GPIO_CTRL_MODE_VSYNC_OUT_1      0x7
#define GPIO_CTRL_MODE_VSYNC_OUT_2      0x8
#define GPIO_CTRL_MODE_VT_SLAVE_MODE    0xa


#define CONTEXT_REG_ADDR_SIZE           0x48

#define I2C_MODE_FAST                   0x0
#define I2C_MODE_FAST_PLUS              0x1

#define DPHYTX_CTRL_DISABLE_CONT_MODE   0xc
#define DPHYTX_CTRL_ENABLE_CONT_MODE    0x1c

#define FWBUF_SIZE                      (64 * 1024)

/* ------Sensor Error Codes------ */
/* FW Patch Errors */
#define CODE_TOO_LARGE              	0x400
#define TOO_MANY_PATCHES            	0x401
#define TOO_MANY_HOOKS              	0x402
/* Exception Errors */
#define PROTECT                        	0x500
#define OPCODE                       	0x501
#define GPRSIZE                        	0x502
#define PMISALIGN                      	0x503
#define POUTOFMEM                   	0x504
#define PEXECUTE                    	0x505
#define DMISALIGN                   	0x506
#define DOUTOFMEM                   	0x507
#define DREAD                       	0x508
#define DWRITE                      	0x509
#define PSYSERR                     	0x50A
#define OVERFLOW                    	0x50B
#define UNKNOWN                     	0x50C
/* VTiming Erros */
#define LONG_COARSE_MAX_ERROR       	0xa00
#define LONG_COARSE_MIN_ERROR       	0xa01
#define BAD_FRAME_LENGTH_ERROR      	0xa02
#define ISB_LONG_PIPE_OVERFLOW      	0xa03
#define Y_SIZE_SS_ERROR             	0xa04
#define X_SIZE_SS_ERROR             	0xa05
#define BGISON_LOW                  	0xa06
#define BGISON_HIGH                 	0xa07
#define TOKEN_NOT_FOUND_ERROR       	0xa08
/* ISP Errors */
#define SDR_FIFO_FULL_ERROR             0xb00
#define OF_FIFO_FULL_ERROR              0xb01
#define ISLGEN_INVALID_CFG_ERROR        0xb02
#define ISLGEN_MEMORY_LOCKED_ERROR      0xb03
#define ISLGEN_MISSED_TRIGGER_ERROR     0xb04
#define ISLGEN_TOO_MANY_ENTRIES_ERROR   0xb05
#define MUTICROP_NO_ROI_ERROR           0xb06
#define ISB2IDP_LINEBLANKING_ERROR      0xb07
/* OIF Errors */
#define CSI_LANE_DESYNC_ERROR           0xc00
#define CSI_PKT_TOO_LONG_ERROR          0xc01
#define CSI_PKT_TOO_SHORT_ERROR         0xc02
#define CSI_UNDERFLOW_ERROR             0xc03
#define MERGER_EXT_SYNC_MISSED_ERROR    0xc04

/* ------Misc Macros------ */
#define VD56G3_DRVP_I2C_DEFAULT_ADDRESS     0x20
#define VD56G3_DRVP_I2C_MASTER_ADDRESS      0x30
#define VD56G3_DRVP_I2C_SLAVE_ADDRESS       0x40
#define VD56G3_DRVP_SLAVE_ADDR_ACTIVE       0x1
#define VD56G3_DRVP_MASTER_ADDR_ACTIVE      0x2
#define VD56G3_DRVP_BOTH_ADDR_ACTIVE        (VD56G3_DRVP_SLAVE_ADDR_ACTIVE | VD56G3_DRVP_MASTER_ADDR_ACTIVE)
#define VD56G3_DRVP_PIXEL_CLOCK_HZ          (160800000)

#define VD56G3_MAX_AGAIN                31
#define VD56G3_MAX_RETRY                0x1000
#define VD56G3_CHIP_ID                  0x5603
#endif/* __VD56G3_REGS_H__  */
