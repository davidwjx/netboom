/****************************************************************************
 *
 *   FileName: iae_drv.h
 *
 *   Author: Danny B.
 *
 *   Date: 
 *
 *   Description: Inuitive IAE Driver
 *   
 ****************************************************************************/
#ifndef IAE_DRV_H
#define IAE_DRV_H

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#include "io_pal.h"
#include "err_defs.h"
#include "xml_db.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/
typedef enum
{
   IAE_DRVG_IAE_OUT0_E  = 0x0,
   IAE_DRVG_IAE_OUT1_E  = 0x1,
   IAE_DRVG_IAE_OUT2_E  = 0x2,
   IAE_DRVG_IAE_OUT3_E  = 0x3,
   IAE_DRVG_IAE_ALNCTRL_IGNORE_E= 0xf,
} IAE_DRVG_alnControlEnE;

typedef enum
{
   IAE_DRVG_ALNCTRL0_IAU0_E               = 0x0,
   IAE_DRVG_ALNCTRL0_Inj0_E      = 0x1,
   IAE_DRVG_ALNCTRL0_Inj1_E      = 0x2,
   IAE_DRVG_ALNCTRL0_Inj2_E      = 0x3,
   IAE_DRVG_ALNCTRL0_Inj3_E      = 0x4,
   IAE_DRVG_ALNCTRL0_Inj4_E      = 0x5,
   IAE_DRVG_ALNCTRL0_Inj5_E      = 0x6,
   IAE_DRVG_IAE_ALNCTRL0_IGNORE_E= 0x8,
} IAE_DRVG_alnControlOut0SelE;

typedef enum
{
   IAE_DRVG_ALNCTRL1_IAU1_E               = 0x0,
   IAE_DRVG_ALNCTRL1_Inj0_E      = 0x1,
   IAE_DRVG_ALNCTRL1_Inj1_E      = 0x2,
   IAE_DRVG_ALNCTRL1_Inj2_E      = 0x3,
   IAE_DRVG_ALNCTRL1_Inj3_E      = 0x4,
   IAE_DRVG_ALNCTRL1_Inj4_E      = 0x5,
   IAE_DRVG_ALNCTRL1_Inj5_E      = 0x6,
   IAE_DRVG_IAE_ALNCTRL1_IGNORE_E= 0x8,
} IAE_DRVG_alnControlOut1SelE;

typedef enum
{
   IAE_DRVG_ALNCTRL2_IAU2_E      = 0x0,
   IAE_DRVG_ALNCTRL2_Inj0_E      = 0x1,
   IAE_DRVG_ALNCTRL2_Inj1_E      = 0x2,
   IAE_DRVG_ALNCTRL2_Inj2_E      = 0x3,
   IAE_DRVG_ALNCTRL2_Inj3_E      = 0x4,
   IAE_DRVG_ALNCTRL2_Inj4_E      = 0x5,
   IAE_DRVG_ALNCTRL2_Inj5_E      = 0x6,
   IAE_DRVG_IAE_ALNCTRL2_IGNORE_E= 0x8,
} IAE_DRVG_alnControlOut2SelE;

typedef enum
{
   IAE_DRVG_ALNCTRL3_IAU3_E      = 0x0,
   IAE_DRVG_ALNCTRL3_Inj0_E      = 0x1,
   IAE_DRVG_ALNCTRL3_Inj1_E      = 0x2,
   IAE_DRVG_ALNCTRL3_Inj2_E      = 0x3,
   IAE_DRVG_ALNCTRL3_Inj3_E      = 0x4,
   IAE_DRVG_ALNCTRL3_Inj4_E      = 0x5,
   IAE_DRVG_ALNCTRL3_Inj5_E      = 0x6,
   IAE_DRVG_IAE_ALNCTRL3_IGNORE_E= 0x8,
} IAE_DRVG_alnControlOut3SelE;

typedef enum
{
   IAE_DRVG_IAE_BYPASS_REG_0_E   = 0,
   IAE_DRVG_IAE_BYPASS_REG_1_E   = 1
} IAE_DRVG_bypassRegistersE;

typedef enum
{
   IAE_DRVG_SLU0_EN_E               = 0,
   IAE_DRVG_SLU1_EN_E               = 1,
   IAE_DRVG_SLU2_EN_E               = 2,
   IAE_DRVG_SLU3_EN_E               = 3,
   IAE_DRVG_SLU4_EN_E               = 4,
   IAE_DRVG_SLU5_EN_E               = 5,
   IAE_DRVG_GEN0_EN_E               = 6,
   IAE_DRVG_GEN1_EN_E               = 7,
   IAE_DRVG_GEN2_EN_E               = 8,
   IAE_DRVG_GEN3_EN_E               = 9,
   IAE_DRVG_IAU0_EN_E               = 10,
   IAE_DRVG_IAU1_EN_E               = 11,
   IAE_DRVG_IAU2_EN_E               = 12,
   IAE_DRVG_IAU3_EN_E               = 13,
   IAE_DRVG_SNS_EN_E                = 14,
   IAE_DRVG_IAU_DISABLE_ALL_BLOCKS_E= 31,
   IAE_DRVG_IAU_ENABLE_ALL_BLOCKS_E = 32
} IAE_DRVG_enableModuleE;

typedef enum
{
   IAE_DRVG_IAU_BPC0_BYPASS_E       = 0,
   IAE_DRVG_IAU_YUV0_BYPASS_E       = 1,
   IAE_DRVG_IAU_IB0_BYPASS_E        = 2,
   IAE_DRVG_IAU_WB0_BYPASS_E        = 3,
   IAE_DRVG_IAU_DMS0_BYPASS_E       = 4,
   IAE_DRVG_IAU_GCR0_BYPASS_E       = 5,
   IAE_DRVG_IAU_MED0_BYPASS_E       = 6,
   IAE_DRVG_IAU_CSC0_BYPASS_E       = 7,
   IAE_DRVG_IAU_DSR0_BYPASS_E       = 8,
   IAE_DRVG_IAU_BPC1_BYPASS_E       = 12,
   IAE_DRVG_IAU_YUV1_BYPASS_E       = 13,
   IAE_DRVG_IAU_IB1_BYPASS_E        = 14,
   IAE_DRVG_IAU_WB1_BYPASS_E        = 15,
   IAE_DRVG_IAU_DMS1_BYPASS_E       = 16,
   IAE_DRVG_IAU_GCR1_BYPASS_E       = 17,
   IAE_DRVG_IAU_MED1_BYPASS_E       = 18,
   IAE_DRVG_IAU_CSC1_BYPASS_E       = 19,
   IAE_DRVG_IAU_DSR1_BYPASS_E       = 20,
   IAE_DRVG_IAU_YUV2_BYPASS_E       = 24,
   IAE_DRVG_IAU_CSC2_BYPASS_E       = 25,
   IAE_DRVG_IAU_DSR2_BYPASS_E       = 26,
   IAE_DRVG_IAU_YUV3_BYPASS_E       = 28,
   IAE_DRVG_IAU_CSC3_BYPASS_E       = 29,
   IAE_DRVG_IAU_DSR3_BYPASS_E       = 30,
   IAE_DRVG_IAU_BYPASS_ALL_BLOCKS_E = 32
} IAE_DRVG_bypassModuleE;

typedef enum
{
   IAE_DRVG_SLU0_SLU1_E             = 0,
   IAE_DRVG_SLU0_SLU2_E             = 1,
   IAE_DRVG_SLU0_SLU3_E             = 2,
   IAE_DRVG_SLU0_SLU4_E             = 3,
   IAE_DRVG_SLU0_SLU5_E             = 4,
   IAE_DRVG_SLU0_INJ01_E            = 5,
   IAE_DRVG_SLU0_INJ23_E            = 6,
   IAE_DRVG_SLU0_INJ45_E            = 7,
   IAE_DRVG_SLU1_SLU2_E             = 8,
   IAE_DRVG_SLU1_SLU3_E             = 9,
   IAE_DRVG_SLU1_SLU4_E             = 10,
   IAE_DRVG_SLU1_SLU5_E             = 11,
   IAE_DRVG_SLU1_INJ01_E            = 12,
   IAE_DRVG_SLU1_INJ23_E            = 13,
   IAE_DRVG_SLU1_INJ45_E            = 14,
   IAE_DRVG_SLU2_SLU3_E             = 15,
   IAE_DRVG_SLU2_SLU4_E             = 16,
   IAE_DRVG_SLU2_SLU5_E             = 17,
   IAE_DRVG_SLU2_INJ01_E            = 18,
   IAE_DRVG_SLU2_INJ23_E            = 19,
   IAE_DRVG_SLU3_SLU4_E             = 20,
   IAE_DRVG_SLU3_SLU5_E             = 21,
   IAE_DRVG_SLU3_INJ01_E            = 22,
   IAE_DRVG_SLU3_INJ23_E            = 23,
   IAE_DRVG_SLU4_SLU5_E             = 24,
   IAE_DRVG_SLU4_INJ01_E            = 25,
   IAE_DRVG_SLU4_INJ23_E            = 26,
   IAE_DRVG_SLU4_INJ45_E            = 27,
   IAE_DRVG_SLU5_INJ01_E            = 28,
   IAE_DRVG_SLU5_INJ23_E            = 29,
   IAE_DRVG_INJ01_INJ23_E           = 30,
   IAE_DRVG_INJ01_INJ45_E           = 31,
   IAE_DRVG_IIM_ALIGN_ALL_BLOCKS_E  = 32
}IAE_DRVG_iimAlignmentE;

typedef enum
{
   IAE_DRVG_MATRIX_SELECT_SLU0_E         = 0,
   IAE_DRVG_MATRIX_SELECT_SLU1_E         = 1,
   IAE_DRVG_MATRIX_SELECT_SLU2_E         = 2,
   IAE_DRVG_MATRIX_SELECT_SLU3_E         = 3,
   IAE_DRVG_MATRIX_SELECT_SLU4_E         = 4,
   IAE_DRVG_MATRIX_SELECT_SLU5_E         = 5,
   IAE_DRVG_MATRIX_SELECT_INJ0_E         = 6,
   IAE_DRVG_MATRIX_SELECT_INJ1_E         = 7,
   IAE_DRVG_MATRIX_SELECT_INJ2_E         = 8,
   IAE_DRVG_MATRIX_SELECT_INJ3_E         = 9,
   IAE_DRVG_MATRIX_SELECT_INJ4_E         = 10,
   IAE_DRVG_MATRIX_SELECT_INJ5_E         = 11,
   IAE_DRVG_MATRIX_SELECT_GEN_0_E        = 12,
   IAE_DRVG_MATRIX_SELECT_GEN_1_E        = 13,
   IAE_DRVG_MATRIX_SELECT_GEN_2_E        = 14,
   IAE_DRVG_MATRIX_SELECT_GEN_3_E        = 15,
   IAE_DRVG_NUM_MATRIX_SELECT_OPTIONS
} IAE_DRVG_iaeMatrixSelectionE;

typedef enum
{
   IAE_DRVG_MIPI_MUX0_SEL_INACTIVE_E = 0,
   IAE_DRVG_MIPI_MUX0_SEL_PHY1_E     = 1,
   IAE_DRVG_MIPI_MUX0_SEL_PHY5_E     = 2,
   IAE_DRVG_NUM_MIPI_MUX0_SEL_OPTIONS
} IAE_DRVG_iaeMipiMuxSlu0SelectionE;

typedef enum
{
   IAE_DRVG_MIPI_MUX1_SEL_INACTIVE_E = 0,
   IAE_DRVG_MIPI_MUX1_SEL_PHY4_E     = 1,
   IAE_DRVG_NUM_MIPI_MUX1_SEL_OPTIONS
} IAE_DRVG_iaeMipiMuxSlu1SelectionE;

typedef enum
{
   IAE_DRVG_MIPI_MUX2_SEL_INACTIVE_E = 0,
   IAE_DRVG_MIPI_MUX2_SEL_PHY3_E     = 1,
   IAE_DRVG_NUM_MIPI_MUX2_SEL_OPTIONS
} IAE_DRVG_iaeMipiMuxSlu2SelectionE;

typedef enum
{
   IAE_DRVG_MIPI_MUX3_SEL_INACTIVE_E = 0,
   IAE_DRVG_MIPI_MUX3_SEL_PHY2_E     = 1,
   IAE_DRVG_MIPI_MUX3_SEL_PHY3_E     = 2,
   IAE_DRVG_NUM_MIPI_MUX3_SEL_OPTIONS
} IAE_DRVG_iaeMipiMuxSlu3SelectionE;

typedef enum
{
   IAE_DRVG_MIPI_MUX4_SEL_INACTIVE_E = 0,
   IAE_DRVG_MIPI_MUX4_SEL_PHY4_E     = 1,
   IAE_DRVG_MIPI_MUX4_SEL_PHY1_E     = 2,
   IAE_DRVG_NUM_MIPI_MUX4_SEL_OPTIONS
} IAE_DRVG_iaeMipiMuxSlu4SelectionE;

typedef enum
{
   IAE_DRVG_MIPI_MUX5_SEL_INACTIVE_E = 0,
   IAE_DRVG_MIPI_MUX5_SEL_PHY5_E     = 1,
   IAE_DRVG_MIPI_MUX5_SEL_PHY0_E     = 2,
   IAE_DRVG_NUM_MIPI_MUX5_SEL_OPTIONS
} IAE_DRVG_iaeMipiMuxSlu5SelectionE;

typedef enum
{
   IAE_DRVG_GEN_SEL0_E,
   IAE_DRVG_GEN_SEL1_E,
   IAE_DRVG_GEN_SEL2_E,
   IAE_DRVG_GEN_SEL3_E,
   IAE_DRVG_NUM_OF_GEN_SEL_E
}IAE_DRVG_genSelE;

typedef enum
{
   IAE_DRVG_GEN_MODE_X_PLUS_Y_E = 0,
   IAE_DRVG_GEN_MODE_X_E        = 1,
   IAE_DRVG_GEN_MODE_Y_E        = 2,
   IAE_DRVG_GEN_MODE_X_Y_E      = 3
} IAE_DRVG_genModeE;

typedef enum
{
   IAE_DRVG_INJECT_MODE_SINGLE_SENSOR_0_E   = 0,
   IAE_DRVG_INJECT_MODE_SINGLE_SENSOR_1_E   = 1,
   IAE_DRVG_INJECT_MODE_SINGLE_SENSOR_2_E   = 2,
   IAE_DRVG_INJECT_MODE_STEREO_SENSORS_E    = 3,
   IAE_DRVG_INJECT_MODE_MIPI_E              = 4,
   IAE_DRVG_INJECT_MODE_DDR_E               = 5,
   IAE_DRVG_NUM_OF_INJECT_MODES_E           = 6
} IAE_DRVG_injectModeE;

typedef enum
{
   IAE_DRVG_IAU_0_E,
   IAE_DRVG_IAU_1_E,
   IAE_DRVG_IAU_2_E,
   IAE_DRVG_IAU_3_E,
   IAE_DRVG_NUM_OF_IAU_UNITS
} IAE_DRVG_iauSelectE;

typedef enum
{
   IAE_DRVG_OUT0_IAU0_E,
   IAE_DRVG_OUT0_NLM_E,
}IAE_DRVG_out0SelectE;

typedef enum
{
   IAE_DRVG_OUT1_IAU1_E,
   IAE_DRVG_OUT1_NLM_E,
}IAE_DRVG_out1SelectE;

typedef enum
{
   IAE_DRVG_OUT2_IAU2_E,
   IAE_DRVG_OUT2_NLM_E,
}IAE_DRVG_out2SelectE;

typedef enum
{
   IAE_DRVG_OUT3_IAU3_E,
   IAE_DRVG_OUT3_NLM_E,
}IAE_DRVG_out3SelectE;


typedef enum
{
   IAE_DRVG_RAM_FLAVOUR_STEREO160_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO128_COLOR64_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO96_COLOR128_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO64_COLOR192_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO32_COLOR256_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO16_COLOR288_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO128_Stereo32_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO96_Stereo64_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO64_Stereo96_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO32_Stereo128_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO16_Stereo144_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO288_E,
   IAE_DRVG_RAM_FLAVOUR_STEREO144_E,
}IAE_DRVG_ramFlavourE;

typedef enum
{
   IAE_DRVG_SLU0_E,
   IAE_DRVG_SLU1_E,
   IAE_DRVG_SLU2_E,
   IAE_DRVG_SLU3_E,
   IAE_DRVG_SLU4_E,
   IAE_DRVG_SLU5_E,
   IAE_DRVG_NUM_OF_INPUT_SENSORS
} IAE_DRVG_sluSelE;

typedef enum
{
   IAE_DRVG_SLU_PARALLEL_INTERFACE_BT601_E  = 0,
   IAE_DRVG_SLU_CSI2_INTERFACE_E            = 1,
   IAE_DRVG_SLU_PARALLEL_INTERFACE_BT656_E  = 2,
   IAE_DRVG_SLU_DSI2_INTERFACE_E            = 3,
} IAE_DRVG_sluInterfaceTypeE;

typedef enum
{
   IAE_DRVG_RGRG_PATTERN_E   = 0,
   IAE_DRVG_GRGR_PATTERN_E   = 1,
   IAE_DRVG_GBGB_PATTERN_E   = 2,
   IAE_DRVG_BGBG_PATTERN_E   = 3
} IAE_DRVG_rgbPatternE;

typedef enum
{
   IAE_DRVG_CSC_DSR_E,
   IAE_DRVG_DSR_CSC_E
} IAE_DRVG_cscDsrOrderE;

typedef enum
{
   IAE_DRVG_NO_LED_FRAME_E       = 0,
   IAE_DRVG_FLOOD_LED_FRAME_E    = 1,
   IAE_DRVG_PATTERN1_LED_FRAME_E = 2,
   IAE_DRVG_PATTERN2_LED_FRAME_E = 3,
   IAE_DRVG_PATTERN_LED_FRAME_E
} IAE_DRVG_FrameTypeE;

typedef enum
{
   IAE_DRVG_GREY_E     = 0,
   IAE_DRVG_BAYER_E    = 1,
   IAE_DRVG_RGB_E      = 2,
   IAE_DRVG_YUV422_E   = 3,
   IAE_DRVG_YUV420L_E  = 4,
   IAE_DRVG_YUV420_E   = 5,
} IAE_DRVG_dataFormatE;

typedef enum
{
   IAE_DRVG_MSB_E  = 0,
   IAE_DRVG_MSB1_E = 1,
   IAE_DRVG_MSB2_E = 2,
   IAE_DRVG_NUM_MSB_OPTIONS
} IAE_DRVG_pixelAlignmentE;

typedef enum
{
   IAE_DRVG_FRAMEID_SOF_E     = 0,
   IAE_DRVG_Frame_Start_Code_E= 1,
   IAE_DRVG_NUM_FRAMEID_MODES
} IAE_DRVG_frameIdModeE;

typedef enum
{
   IAE_DRVG_FRAMETS_SOF_E              = 0,
   IAE_DRVG_Generic_Short_Packet_Code_E= 1,
   IAE_DRVG_NUM_FRAMETS_MODES
} IAE_DRVG_frameTsModeE;

typedef enum
{
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RGB888_E        = 0,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RGB666_E        = 1,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RGB565_E        = 2,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RGB555_E        = 3,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RGB444_E        = 4,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_YUV422_8_BITS_E = 5,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_YUV422_10_BITS_E= 6,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_YUV420L_8_BITS_E= 7,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_YUV420_8_BITS_E = 8,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_YUV420_10_BITS_E= 9,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RAW6_E          = 10,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RAW7_E          = 11,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RAW8_E          = 12,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RAW10_E         = 13,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RAW12_E         = 14,
   IAE_DRVG_MIPI_RX_DATA_FORMAT_RAW14_E         = 15,
   IAE_DRVG_MIPI_RX_DATA_FORMATS
} IAE_DRVG_mipiRxDataFormatE;

typedef enum
{
   IAE_DRVG_UYVY_E   = 0,
   IAE_DRVG_VYUY_E   = 1,
   IAE_DRVG_YUYV_E   = 2,
   IAE_DRVG_YVYU_E   = 3,
   IAE_DRVG_YUV_FORMATS
} IAE_DRVG_YUVFormatsE;

typedef enum
{
   IAE_DRVG_RGB888_BGR_E   = 0,
   IAE_DRVG_RGB888_GBR_E   = 1,
   IAE_DRVG_RGB888_GRB_E   = 2,
   IAE_DRVG_RGB888_RGB_E   = 3,
   IAE_DRVG_RGB888_BRG_E   = 4,
   IAE_DRVG_RGB888_RBG_E   = 5,
   IAE_DRVG_RGB888_ORDERS
} IAE_DRVG_RGB888OrderE;

typedef enum
{
   IAE_DRVG_NLM_MODE_11_E,
   IAE_DRVG_NLM_MODE_21_E,
   IAE_DRVG_NLM_MODES
} IAE_DRVG_NLMModesE;
















typedef enum
{
   SLU_CVR_UNIT_ID      = 0,
   SLU_CVL_UNIT_ID      = 1,
   SLU_CS_UNIT_ID       = 2,
   IAU_CVR_UNIT_ID      = 3,
   IAU_CVL_UNIT_ID      = 4,
   IAM_CH0_UNIT_ID      = 5,
   IAM_CH1_UNIT_ID      = 6,
   IAM_CH2_UNIT_ID      = 7,
   IAM_CH3_UNIT_ID      = 8,
   IAM_CH4_UNIT_ID      = 9,
   IAM_CH5_UNIT_ID      = 10,
   IAM_CH6_UNIT_ID      = 11,
   IAM_CH7_UNIT_ID      = 12,
   IAM_CH8_UNIT_ID      = 13,
   IAM_CH9_UNIT_ID      = 14,
   IAM_CH10_UNIT_ID     = 15,
   IAM_CH11_UNIT_ID     = 16,
   IAM_CH12_UNIT_ID     = 17,
   IAM_CH13_UNIT_ID     = 18,
   IAM_DS_0_CVL_UNIT_ID = 19,
   IAM_DS_1_CVL_UNIT_ID = 20,
   IAM_DS_2_CVL_UNIT_ID = 21,
   IAM_DS_3_CVL_UNIT_ID = 22,
   IAM_DS_4_CVL_UNIT_ID = 23,
   IAM_DS_5_CVL_UNIT_ID = 24,
   IAM_DS_0_CVR_UNIT_ID = 25,
   IAM_DS_1_CVR_UNIT_ID = 26,
   IAM_DS_2_CVR_UNIT_ID = 27,
   IAM_DS_3_CVR_UNIT_ID = 28,
   IAM_DS_4_CVR_UNIT_ID = 29,
   IAM_DS_5_CVR_UNIT_ID = 30,
   PWM_UNIT_ID          = 31,
   IAE_ALL_UNITS
} IAE_DRVG_iaeUnitIdE;

typedef enum
{
   IAE_DRVG_CAMERA_L_E     = 0,
   IAE_DRVG_CAMERA_R_E     = 1,
   IAE_DRVG_CAMERA_COLOR_E = 2,
   IAE_DRVG_NUM_OF_CAMERAS
} IAE_DRVG_cameraSelectionE;

typedef enum
{
   IAE_DRVG_LAN_0_E  = 0,
   IAE_DRVG_LAN_1_E  = 1,
   IAE_DRVG_LAN_2_E  = 2,
   IAE_DRVG_NUM_OF_LANS
} IAE_DRVG_lanSelectionE;



typedef enum
{
   IAE_DRVG_FRAME_START_SELECT_MASK_CVLS_E   = 0,
   IAE_DRVG_FRAME_START_SELECT_MASK_CVRS_E   = 1,
   IAE_DRVG_FRAME_START_SELECT_MASK_CS_E     = 2,
   IAE_DRVG_FRAME_START_SELECT_MASK_GEN_E    = 3,
   IAE_DRVG_FRAME_START_SELECT_MASK_LAST_E   = 4
} IAE_DRVG_iaeFrameStartSelectMaskE;

typedef enum
{
   IAE_DRVG_CVR_SLU_OUT_AND_CVL_SLU_OUT_E = 0,
   IAE_DRVG_CVR_SLU_OUT_AND_CVR_IAU_OUT_E = 1,
   IAE_DRVG_CVL_SLU_OUT_AND_CVL_IAU_OUT_E = 2,
   IAE_DRVG_COLOR_SLU_OUT_E               = 3,
   IAE_DRVG_CVR_IAU_OUT_E                 = 4,
   IAE_DRVG_CVL_IAU_OUT_E
} IAE_DRVG_iamMuxSelectE;

typedef enum
{
   IAE_DRVG_IAM_INTERLEAVER_0_7_E   = 0,
   IAE_DRVG_IAM_INTERLEAVER_1_8_E   = 1,
   IAE_DRVG_IAM_INTERLEAVER_2_9_E   = 2,
   IAE_DRVG_IAM_INTERLEAVER_3_10_E  = 3,
   IAE_DRVG_IAM_INTERLEAVER_4_11_E  = 4,
   IAE_DRVG_IAM_INTERLEAVER_5_12_E  = 5,
   IAE_DRVG_IAM_INTERLEAVER_6_13_E  = 6
} IAE_DRVG_iamInterleaveChannelE;

typedef enum
{
   IAE_DRVG_IAM_CH0_E         = 0,
   IAE_DRVG_IAM_CH1_E         = 1,
   IAE_DRVG_IAM_CH2_E         = 2,
   IAE_DRVG_IAM_CH3_E         = 3,
   IAE_DRVG_IAM_CH4_E         = 4,
   IAE_DRVG_IAM_CH5_E         = 5,
   IAE_DRVG_IAM_CH6_E         = 6,
   IAE_DRVG_IAM_CH7_E         = 7,
   IAE_DRVG_IAM_CH8_E         = 8,
   IAE_DRVG_IAM_CH9_E         = 9,
   IAE_DRVG_IAM_CH10_E        = 10,
   IAE_DRVG_IAM_CH11_E        = 11,
   IAE_DRVG_IAM_CH12_E        = 12,
   IAE_DRVG_IAM_CH13_E        = 13,
   IAE_DRVG_IAM_NUM_CHANNELS  = 14
}IAE_DRVG_iamChannelSelectionE;


// IAU interupts
typedef enum
{
   IAE_DRVG_INT_IAU_IN_FRAME_START_E  = 0,
   IAE_DRVG_INT_IAU_OUT_FRAME_END_E,
   IAE_DRVG_INT_DSR_IN_FRAME_START_E,
   IAE_DRVG_INT_DSR_OUT_FRAME_END_E,
   IAE_DRVG_INT_DSR_FRAME_OVERFLOW_E,
	IAE_DRVG_INT_DSR_LINE_BUFFER_OVERFLOW_E,
	IAE_DRVG_INT_DSR_OUT_OF_BOUNDRY_E,
	IAE_DRVG_INT_DSR_PPM_UPDATE_E,
   IAE_DRVG_NUM_OF_INT_IAU_E
}  IAE_DRVG_iauInterruptsE;

// IAU histogram interupts
typedef enum
{
   IAE_DRVG_INT_HIST_RDY_E   = 0,
	IAE_DRVG_INT_HIST_MISSED_E,	
	IAE_DRVG_INT_EHE_RAM_INIT_DONE_E,
	IAE_DRVG_INT_EHE_FRAME_END_E,
	IAE_DRVG_INT_AXI_FIFO_OVERFLOW_E,
   IAE_DRVG_NUM_OF_INT_HIST_E
}  IAE_DRVG_histInterruptsE;


typedef enum
{
   IAE_DRVG_SLU0_INTERRUPT_E = 0,
   IAE_DRVG_SLU1_INTERRUPT_E,
   IAE_DRVG_SLU2_INTERRUPT_E,
   IAE_DRVG_SLU3_INTERRUPT_E,
   IAE_DRVG_SLU4_INTERRUPT_E,
   IAE_DRVG_SLU5_INTERRUPT_E,

	IAE_DRVG_CSI_RX0_INTERRUPT_E = 8,
	IAE_DRVG_CSI_RX1_INTERRUPT_E,
	IAE_DRVG_CSI_RX2_INTERRUPT_E,
	IAE_DRVG_CSI_RX3_INTERRUPT_E,
	IAE_DRVG_CSI_RX4_INTERRUPT_E,
	IAE_DRVG_CSI_RX5_INTERRUPT_E,

	IAE_DRVG_DSI_RX0_INTERRUPT_E = 16,
	IAE_DRVG_DSI_RX1_INTERRUPT_E,

   IAE_DRVG_IAU0_INTERRUPT_E = 24,
   IAE_DRVG_IAU1_INTERRUPT_E,
   IAE_DRVG_IAU2_INTERRUPT_E,
   IAE_DRVG_IAU3_INTERRUPT_E,

   IAE_DRVG_SNSR_CTRL_INTERRUPT_E = 28,

   IAE_DRVG_TOP_INTERRUPT_E = 31,
   IAE_DRVG_ALL_INTERRUPTS_E,
} IAE_DRVG_iaeIntE;

typedef enum
{
   IAE_DRVG_DPHY0_ERRSYNC_E = 4,
   IAE_DRVG_DPHY0_ERRCTL_E,
   IAE_DRVG_DPHY0_ERRCONTEN_E,
   IAE_DRVG_DPHY1_ERRSYNC_E,
   IAE_DRVG_DPHY1_ERRCTL_E,
   IAE_DRVG_DPHY1_ERRCONTEN_E,
   IAE_DRVG_DPHY2_ERRSYNC_E,
   IAE_DRVG_DPHY2_ERRCTL_E,
   IAE_DRVG_DPHY2_ERRCONTEN_E,
   IAE_DRVG_DPHY3_ERRSYNC_E,
   IAE_DRVG_DPHY3_ERRCTL_E,
   IAE_DRVG_DPHY3_ERRCONTEN_E,
   IAE_DRVG_DPHY4_ERRSYNC_E,
   IAE_DRVG_DPHY4_ERRCTL_E,
   IAE_DRVG_DPHY4_ERRCONTEN_E,
   IAE_DRVG_DPHY5_ERRSYNC_E,
   IAE_DRVG_DPHY5_ERRCTL_E,
   IAE_DRVG_DPHY5_ERRCONTEN_E,
   IAE_DRVG_ALLPHY5_INT_E,
} IAE_DRVG_iaePhyIntE;

#define IAE_IAU_IRQ_BITS_MASK ((1 << IAE_DRVG_IAU0_INTERRUPT_E) | (1 << IAE_DRVG_IAU1_INTERRUPT_E) |(1 << IAE_DRVG_IAU2_INTERRUPT_E) |(1 << IAE_DRVG_IAU3_INTERRUPT_E))

typedef enum
{
   IAE_DRVG_DIM      = 0,
   IAE_DRVG_DIM_AND  = 1
} IAE_DRVG_DIM_MODE_E;

typedef enum
{
   IAE_DRVG_pwm_ACTIVE_HIGH_E = 0,
   IAE_DRVG_pwm_ACTIVE_LOW_E  = 1
} IAE_DRVG_pwmPolarityE;


typedef enum
{
   IAE_DRVG_SLU_FIRST_BITS_9_2_FOLLOWED_BY_BITS_1_0_E    = 0,
   IAE_DRVG_SLU_FIRST_BITS_11_4_FOLLOWED_BY_BITS_3_0_E   = 1,
   IAE_DRVG_SLU_FIRST_BITS_15_8_FOLLOWED_BY_BITS_7_0_E   = 2,
   IAE_DRVG_SLU_FIRST_BITS_11_2_FOLLOWED_BY_BITS_1_0_E   = 3,
   IAE_DRVG_SLU_FIRST_BITS_15_6_FOLLOWED_BY_BITS_5_0_E   = 4,
   IAE_DRVG_SLU_FIRST_BITS_15_4_FOLLOWED_BY_BITS_3_0_E   = 5,
   IAE_DRVG_SLU_FORBIDDEN                                = 6,
   IAE_DRVG_SLU_ALL_DATA_ARRIVES_AT_THE_SAME_CYCLE_E     = 7
} IAE_DRVG_sluPixelDivisionE;

typedef enum
{
   IAE_DRVG_SLU_UPPER_BITS_FIRST_E  = 0,
   IAE_DRVG_SLU_LOWER_BITS_FIRST_E  = 1
} IAE_DRVG_sluPixelInorderE;

typedef enum
{
   IAE_DRVG_SLU_FRAME_VALID_ACTIVE_HIGH_E = 0,
   IAE_DRVG_SLU_FRAME_VALID_ACTIVE_LOW_E  = 1
} IAE_DRVG_frameValidPolarityE;

typedef enum
{
   IAE_DRVG_SLU_LINE_VALID_ACTIVE_HIGH_E  = 0,
   IAE_DRVG_SLU_LINE_VALID_ACTIVE_LOW_E   = 1
} IAE_DRVG_lineValidPolarityE;


typedef enum
{
   IAE_DRVG_TRIGGER_POSITIVE_POLARITY_E   = 0,
   IAE_DRVG_TRIGGER_NEGATIVE_POLARITY_E   = 1
} IAE_DRVG_triggerPolarityE;


typedef enum
{
   IAE_DRVG_DISABLE_E  = 0,
   IAE_DRVG_ENABLE_E   = 1,
} IAE_DRVG_enableE;

typedef enum
{
   IAE_DRVG_ACTIVE_HIGH_E  = 0,
   IAE_DRVG_ACTIVE_LOW_E   = 1,
} IAE_DRVG_polarityE;

typedef enum
{
   IAE_DRVG_SHIFT_LEFT_E   = 0,
   IAE_DRVG_SHIFT_RIGHT_E  = 1,
} IAE_DRVG_shiftDirE;

typedef enum
{
   IAE_DRVG_PWM_0_E         = 0,
   IAE_DRVG_PWM_1_E         = 1,
   IAE_DRVG_PWM_2_E         = 2,
   IAE_DRVG_NUM_PWM_E       = 3,
}IAE_DRVG__pwmNumE;
typedef enum
{
   IAE_DRVG_FSG_CNT_0_E       = 0,
   IAE_DRVG_FSG_CNT_1_E       = 1,
   IAE_DRVG_FSG_CNT_2_E       = 2,
   IAE_DRVG_FSG_CNT_3_E       = 3,
   IAE_DRVG_FSG_CNT_4_E       = 4,
   IAE_DRVG_FSG_CNT_5_E       = 5,
}IAE_DRVG_fsgCounterNumE;
typedef enum 
{
   IAE_DRVG_FSG_CNTMODE_MANUAL  = 0,
   IAE_DRVG_FSG_CNTMODE_SWMODE = 1,
   IAE_DRVG_FSG_CNTMODE_HWMODE = 2,
   IAE_DRVG_FSG_CNTMODE_HWSYNC = 3
} IAE_DRVG_fsgCntMode;
typedef enum
{
   IAE_DRVG_FSG_CNTSRC_0_E       = 0,
   IAE_DRVG_FSG_CNTSRC_1_E       = 1,
   IAE_DRVG_FSG_CNTSRC_2_E       = 2,
}IAE_DRVG_fsgCounterSrc;
typedef enum
{
   IAE_DRVG_FSG_PULSE_ACTIVE_HIGH       = 0,
   IAE_DRVG_FSG_PULSE_ACTIVE_LOW       =  1,
}IAE_DRVG_fsgPulsePolarityE;

typedef struct
{
   UINT16               enable;
   UINT16               width;
   UINT16               height;
   UINT16               fps;
   IAE_DRVG_genModeE   mode;
} IAE_DRVG_genCfgT;

typedef struct
{
   UINT16                  enable;
   IAE_DRVG_injectModeE    mode;
   UINT16                  width;
   UINT16                  height;
   UINT16                  fps;
} IAE_DRVG_injectCfgT;

typedef struct
{
//   IAE_DRVG_genCfgT    gen[IAE_DRVG_NUM_OF_IAE_GENERATORS_E];
   IAE_DRVG_injectCfgT injector;
} IAE_DRVG_iimCfgT;

typedef struct
{
   UINT32   active;
   UINT16   width;
   UINT16   height;
} IAE_DRVG_bpcCfgT;

typedef struct
{
   UINT32   active;
   UINT16   ibOffset;
} IAE_DRVG_ibCfgT;

typedef struct
{
   UINT32   active;
   UINT16   a_bu;
   UINT16   b_u;
   UINT16   a_bv;
   UINT16   b_v;
   UINT16   a_by;
   UINT16   b_y;
   UINT16   a_ru;
   UINT16   a_gu;
   UINT16   a_rv;
   UINT16   a_gv;
   UINT16   a_ry;
   UINT16   a_gy;
} IAE_DRVG_rgb2yuvCfgT;

typedef struct
{
   UINT32   x0;
   UINT32   x1;
   UINT32   y0;
   UINT32   y1;
} IAE_DRVG_histRoiT;

typedef struct
{
   UINT32   en;
   UINT32   grey10_en;
   UINT32   grey12_en;
   UINT32   ciif_select;
} IAE_DRVG_histEnT;

typedef struct
{
   UINT32   active;
} IAE_DRVG_dmsCfgT;

typedef struct
{
   UINT32   active;
} IAE_DRVG_gcrCfgT;

typedef struct
{
   UINT32   active;
} IAE_DRVG_firCfgT;

typedef struct
{
   UINT32   active;
} IAE_DRVG_yuvCfgT;

typedef enum
{
   IAE_DRVG_IAE_HIST_ROI_0_E = 0,
   IAE_DRVG_IAE_HIST_ROI_1_E = 1,
   IAE_DRVG_IAE_HIST_ROI_2_E = 2,
   IAE_DRVG_IAE_HIST_ROI_TOTAL_NUM_E
} IAE_DRVG_iaeHistRoiNumE;

typedef struct
{
   IAE_DRVG_alnControlEnE        alnControlEn0;
   IAE_DRVG_alnControlEnE        alnControlEn1;
   IAE_DRVG_alnControlOut0SelE   out0Sel;
   IAE_DRVG_alnControlOut1SelE   out1Sel;
   IAE_DRVG_alnControlOut2SelE   out2Sel;
   IAE_DRVG_alnControlOut3SelE   out3Sel;
   UINT16                        aFull;
} IAE_DRVG_alnCtrlCfgT;

typedef struct
{
   IAE_DRVG_bypassModuleE  bypassBlock;
} IAE_DRVG_BypassCfgT;

typedef struct
{
   IAE_DRVG_enableModuleE  enableBlock;
} IAE_DRVG_EnableCfgT;

typedef struct
{
   IAE_DRVG_iimAlignmentE  iimAlignment;
} IAE_DRVG_iaeIimAlignmentCfgT;

typedef struct
{
   IAE_DRVG_iaeMatrixSelectionE        iau0Select;
   IAE_DRVG_iaeMatrixSelectionE        iau1Select;
   IAE_DRVG_iaeMatrixSelectionE        iau2Select;
   IAE_DRVG_iaeMatrixSelectionE        iau3Select;
} IAE_DRVG_iaeIimCtrlCfgT;

typedef struct
{
   IAE_DRVG_iaeMipiMuxSlu0SelectionE   iaeMipiMuxSlu0;
   IAE_DRVG_iaeMipiMuxSlu1SelectionE   iaeMipiMuxSlu1;
   IAE_DRVG_iaeMipiMuxSlu2SelectionE   iaeMipiMuxSlu2;
   IAE_DRVG_iaeMipiMuxSlu3SelectionE   iaeMipiMuxSlu3;
   IAE_DRVG_iaeMipiMuxSlu4SelectionE   iaeMipiMuxSlu4;
   IAE_DRVG_iaeMipiMuxSlu5SelectionE   iaeMipiMuxSlu5;
} IAE_DRVG_iaeMipiMuxCfgT;

typedef struct
{
   IAE_DRVG_bypassRegistersE  nlmBypass;
   IAE_DRVG_NLMModesE         nlmMode;
   IAE_DRVG_iauSelectE        nlmSrc;
   IAE_DRVG_out0SelectE       out0;
   IAE_DRVG_out1SelectE       out1;
   IAE_DRVG_out2SelectE       out2;
   IAE_DRVG_out3SelectE       out3;
   IAE_DRVG_ramFlavourE       ramFlavour;
   UINT16                     oneOverH;
   UINT16                     csc0A;
   UINT16                     csc0B;
   UINT16                     csc1C;
   UINT16                     csc1D;
}IAE_DRVG_iaeNlmCfgT;

typedef struct
{
   IAE_DRVG_genSelE     genSelect;
   UINT16               frameSizeHorz;
   UINT16               frameSizeVert;
   UINT16               frameBlank;
   UINT16               lineBlank;
   IAE_DRVG_genModeE    mode;
   UINT16               frameNum;
} IAE_DRVG_iaeFGenCfgT;

typedef struct
{
   IAE_DRVG_histRoiT    roi[IAE_DRVG_IAE_HIST_ROI_TOTAL_NUM_E];
} IAE_DRVG_histRoiCfgT;

typedef struct
{
   IAE_DRVG_iauSelectE  iauSelect;
   UINT32               lutK;
   UINT32               lutL;
   UINT32               lutHSize;
   UINT32               lutMode;
   UINT32               bPrec;
   UINT32               horizontalOffset;
   UINT32               verticalOffset;
   UINT32               ibOffset0;
   UINT32               ibOffset1;
   UINT32               ibOffset2;
   UINT32               ibOffset3;
} IAE_DRVG_IbCfgT;

typedef struct
{
   IAE_DRVG_iauSelectE  iauSelect;
   UINT16               wbBlueW_blue;
   UINT16               wbBlueC_blue;
   UINT16               wbControlW_prec;
   UINT16               wbGbW_gb;
   UINT16               wbGbC_gb;
   UINT16               wbGrW_gr;
   UINT16               wbGrC_gr;
   UINT16               wbRedW_red;
   UINT16               wbRedC_red;
} IAE_DRVG_wbCfgT;

typedef struct
{
   IAE_DRVG_rgbPatternE bayerPattern;
   IAE_DRVG_FrameTypeE  frameType;
}IAE_DRVG_iauCtrlCfgT;

typedef struct
{
   UINT16               cscBv0A;
   UINT16               cscBv0B;
   UINT16               cscBv1C;
   UINT16               cscBv1D;
   UINT16               cscGu0A;
   UINT16               cscGu0B;
   UINT16               cscGu1C;
   UINT16               cscGu1D;
   UINT16               cscRy0A;
   UINT16               cscRy0B;
   UINT16               cscRy1C;
   UINT16               cscRy1D;
}IAE_DRVG_CSCCfgT;

typedef struct
{
   UINT32                  blankLine;
   UINT32                  blankFrame;
   UINT32                  buffInitFull;
   UINT32                  buffMaxAddr;
   IAE_DRVG_cscDsrOrderE   cscDsrOrder;
   UINT32                  lutMode;
   UINT32                  lutK;
   UINT32                  lutL;
   UINT32                  lutHSize;
   UINT32                  lutOffsetHorSize;
   UINT32                  lutOffsetVerSize;
   UINT32                  lutPrecDxPrec;
   UINT32                  lutPrecDyPrec;
   UINT32                  lutPrecDxSize;
   UINT32                  lutPrecDySize;
   UINT32                  outSizeHor;
   UINT32                  outSizeVer;
} IAE_DRVG_dsrCfgT;

typedef struct
{
   UINT16                  vecEn;
   UINT16                  vecSize;
   UINT32                  go;
}IAE_DRVG_FrrCfgT;

typedef struct
{
   IAE_DRVG_iauSelectE     iauSelect;
   IAE_DRVG_iauCtrlCfgT    iauCtrlCfg;
//   IAE_DRVG_histCfgT       histCfg;
   IAE_DRVG_IbCfgT         IbCfg;
   IAE_DRVG_wbCfgT         wbCfg;
   IAE_DRVG_CSCCfgT        cscCfg;
   IAE_DRVG_dataFormatE    dataFormat;
   IAE_DRVG_dsrCfgT        dsrCfg;
   IAE_DRVG_FrrCfgT        frrCfg;
}IAE_DRVG_iauCfgT;

typedef struct
{
   IAE_DRVG_sluSelE              sluSelect;
   IAE_DRVG_sluInterfaceTypeE    interfaceType;
   IAE_DRVG_pixelAlignmentE      pixelAlignment;
   IAE_DRVG_frameIdModeE         frameIdMode;
   IAE_DRVG_frameTsModeE         frameTsMode;
   UINT16                        mipiRxVc;
   IAE_DRVG_mipiRxDataFormatE    mipiRxDataFormat;
   IAE_DRVG_YUVFormatsE          yuvFormat;
   IAE_DRVG_RGB888OrderE         rgb888Order;
   UINT16                        TsDt;
} IAE_DRVG_SluCtrlT;

typedef struct
{
   IAE_DRVG_enableE     twoCycleEn;
   IAE_DRVG_polarityE   frameValidPolarity;
   IAE_DRVG_polarityE   lineValidPolarity;
   UINT8                bt656_mode;
}IAE_DRVG_ParCtrlT;

typedef struct 
{
   UINT8             fsg2_trig_src; /* FSG counter trigger source:
                                       0...5 - SLU SOF
                                       6...11 - SLU EOF
                                       12..14 - SW sync
                                       15 - Reserved
                                       16...31 - Ext sync   */
   UINT8                fsg1_trig_src; /* FSG counter trigger source:
                                       0...5 - SLU SOF
                                       6...11 - SLU EOF
                                       12..14 - SW sync
                                       15 - Reserved
                                       16...31 - Ext sync   */
   UINT8                fsg0_trig_src; /* FSG counter trigger source:
                                       0...5 - SLU SOF
                                       6...11 - SLU EOF
                                       12..14 - SW sync
                                       15 - Reserved
                                       16...31 - Ext sync   */
   UINT8                ts_cnt_range;   /*time stamp = TS_CNT >> TS_CNT_RANGE (0->16) */
   BOOL                 ts_src;         /*0 = Internal counter, 1 = External Counter*/
   UINT8                fsg0_cnt_mode;  /*0 = Manual
                                       1 =  Automatic SW mode
                                       2 = Automatic HW mode
                                       3 = Automatic HW sync mode
                                       */
   UINT8                fsg1_cnt_mode;  /*0 = Manual
                                       1 =  Automatic SW mode
                                       2 = Automatic HW mode
                                       3 = Automatic HW sync mode
                                       */
   UINT8                fsg2_cnt_mode;  /*0 = Manual
                                       1 =  Automatic SW mode
                                       2 = Automatic HW mode
                                       3 = Automatic HW sync mode
                                       */
   BOOL                 ts_cnt_en;     /*Free running counter enable*/
   UINT8                fsg_cnt_en;    /*Bitmap, 0 = fsg0, 1= fsg1, 2=fsg2*/
}  IAE_DRVG_snsrCtrlFsgCntrCtrl; 

typedef struct
{
   UINT8                      width;               /*Pulse width (counts)*/
   IAE_DRVG_fsgCounterSrc    counterSrc;          /*Counter source      */
   IAE_DRVG_fsgPulsePolarityE pulsePolarity;       /*Pulse polarity      */
   BOOL                       pulseEnable;         /*True = Enable pulse, False = Disable pulse*/
} IAE_DRVG_snsrCtrlFSGFtrigPulseCtrl;
typedef struct
{
   UINT16               mask;
   UINT8                shift;
   IAE_DRVG_shiftDirE   shiftDir;
   IAE_DRVG_enableE     swapEn;
}IAE_DRVG_twoCycleCtrlT;

typedef struct
{
   UINT16   horz;
   UINT16   vert;
}IAE_DRVG_CropOffsetT;

typedef struct
{
   UINT16   horz;
   UINT16   vert;
}IAE_DRVG_FrameSizeT;

typedef struct
{
   IAE_DRVG_CropOffsetT    cropOffset;
   UINT16                  frameId;
   UINT32                  frameTs;
   IAE_DRVG_FrameSizeT     frameSize;
   IAE_DRVG_SluCtrlT       sluCtrl;
   IAE_DRVG_ParCtrlT       parCtrl;
   IAE_DRVG_twoCycleCtrlT  twoCycCtrl0;
   IAE_DRVG_twoCycleCtrlT  twoCycCtrl1;
}IAE_DRVG_sluCfgT;

typedef struct
{
	UINT32 status;
   UINT64 timestamp;
}IAE_DRVG_intCfgT;

typedef void (*IAE_DRVG_interruptCbT) (IAE_DRVG_intCfgT intCtrl, IAE_DRVG_iaeIntE isrSrc);
typedef void (*IAE_DRVG_snsrInterruptCbT) (UINT64 timestamp, UINT32 slu, void *argP);
typedef void (*IAE_DRVG_phyInterruptCbT) (UINT32 srcInt);

typedef struct
{
	IAE_DRVG_iaeIntE      isrSrc;
   IAE_DRVG_interruptCbT cb;
} IAE_DRVG_setInterruptCbT;

typedef struct
{
   UINT32            iaeRegisterAddress;
   UINT32            value;
   INU_DEFSG_accessTypeE readWrite;
} IAE_DRVG_accessIaeRegisterT;

typedef union
{
   IAE_DRVG_alnCtrlCfgT             alnCtrlCfg;
   IAE_DRVG_BypassCfgT              bypassCfg;
   IAE_DRVG_EnableCfgT              enableCfg;
   IAE_DRVG_FrrCfgT                 frrCfg;
   IAE_DRVG_iaeIimAlignmentCfgT     iimAlnCfg;
   IAE_DRVG_iaeIimCtrlCfgT          iimCtrlCfg;
   IAE_DRVG_iaeMipiMuxCfgT          mipiMuxCfg;
   IAE_DRVG_iaeNlmCfgT              nlmCfg;
   IAE_DRVG_iaeFGenCfgT             fGenerateCfg;
   IAE_DRVG_iauCfgT                 iauCfg;
   IAE_DRVG_sluCfgT                 sluCfg;
   IAE_DRVG_intCfgT                 intCfg;
   IAE_DRVG_setInterruptCbT         setInterruptCb;
   IAE_DRVG_accessIaeRegisterT      accessIaeRegister;
} IAE_DRVG_iaeParametersT;


typedef struct
{
   INU_DEFSG_iaeLutIdE  lutId;
   UINT16               chunkLen;
   UINT32               iaeLutOffset;
   UINT32               ddrLutBasePhysAddr;
} IAE_DRVG_downloadLutT;

typedef struct
{
   UINT32               chunkLen;
   UINT32               ddrHistBasePhysAddr;
} IAE_DRVG_loadHistT;

// module open function params
typedef struct
{
   UINT32      iaeRegistersVirtualAddr;
} IAE_DRVG_openParamsT;

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
#if 1
ERRG_codeE IAE_DRVG_init( UINT32 memVirtAddr );
void       IAE_DRVG_deinit(void);
#else
ERRG_codeE IAE_DRVG_init(IO_PALG_apiCommandT *palP);
#endif
#if 1
ERRG_codeE IAE_DRVG_bypassCfg(UINT8 iaeNum, UINT32 bypass);
#else
ERRG_codeE IAE_DRVG_bypassCfg(const IAE_DRVG_bypassModuleE  *bypassBlock);
#endif
ERRG_codeE IAE_DRVG_enableCfg(const IAE_DRVG_enableModuleE  *enableBlock);
ERRG_codeE IAE_DRVG_disableCfg(const IAE_DRVG_enableModuleE  *enableBlock);
ERRG_codeE IAE_DRVG_alnCtrlCfg(const IAE_DRVG_alnCtrlCfgT *alnCtrlCfg);
ERRG_codeE IAE_DRVG_FrrCfg(const IAE_DRVG_FrrCfgT *frrCfg);
ERRG_codeE IAE_DRVG_iimAlnCfg(const IAE_DRVG_iimAlignmentE  *iimAlignment);
#if 1
void       IAE_DRVG_iaeIimSrcCfg(UINT8 iaeNum, IAE_DRVG_iaeMatrixSelectionE  srcSelcet);
#else
ERRG_codeE IAE_DRVG_iimCtrlCfg(const IAE_DRVG_iaeIimCtrlCfgT  *iimCtrlCfg);
#endif
ERRG_codeE IAE_DRVG_rxClkEnCfg(INU_DEFSG_mipiInstE inst);
ERRG_codeE IAE_DRVG_nlmCtrlCfg(const IAE_DRVG_iaeNlmCfgT   *nlmCfg);
ERRG_codeE IAE_DRVG_nlmCscCfg(const IAE_DRVG_iaeNlmCfgT   *nlmCfg);
ERRG_codeE IAE_DRVG_mipiMuxCfg(IAE_DRVG_sluSelE inst, UINT32 mipiMuxCfg);
ERRG_codeE IAE_DRVG_FGenCfg(const IAE_DRVG_iaeFGenCfgT  *fGenerateCfg);
ERRG_codeE IAE_DRVG_iauCtrlCfg(const IAE_DRVG_iauCfgT   *iauCfg);
ERRG_codeE IAE_DRVG_histGetRoiCfg(IAE_DRVG_iauSelectE iauSelectIdx, IAE_DRVG_histRoiCfgT *cfgP);
ERRG_codeE IAE_DRVG_histRoiCfg(IAE_DRVG_iauSelectE iauSelectIdx, IAE_DRVG_histRoiCfgT *cfgP);
ERRG_codeE IAE_DRVG_ibCfg(const IAE_DRVG_iauCfgT  *iauCfg);
ERRG_codeE IAE_DRVG_wbCfg(const IAE_DRVG_iauCfgT  *iauCfg);
ERRG_codeE IAE_DRVG_cscCfg(const IAE_DRVG_iauCfgT *iauCfg);
ERRG_codeE IAE_DRVG_dataFormatCfg(const IAE_DRVG_iauCfgT   *iauCfg);
ERRG_codeE IAE_DRVG_dsrCfg(const IAE_DRVG_iauCfgT   *iauCfg);
ERRG_codeE IAE_DRVG_iauFrrCfg(const IAE_DRVG_iauCfgT   *iauCfg);
ERRG_codeE IAE_DRVG_sluCtrlCfg(const IAE_DRVG_sluCfgT   *sluCfg);
ERRG_codeE IAE_DRVG_sluCropCfg(const IAE_DRVG_sluCfgT   *sluCfg);
ERRG_codeE IAE_DRVG_sluParControl(const IAE_DRVG_sluCfgT   *sluCfg);
ERRG_codeE IAE_DRVG_twoCycleCtrl0(const IAE_DRVG_sluCfgT   *sluCfg);
ERRG_codeE IAE_DRVG_twoCycleCtrl1(const IAE_DRVG_sluCfgT   *sluCfg);
ERRG_codeE IAE_DRVG_sluFrameSize(const IAE_DRVG_sluCfgT *sluCfg);
ERRG_codeE IAE_DRVG_sluFrameIdRead(IAE_DRVG_sluCfgT  *sluCfg);
ERRG_codeE IAE_DRVG_sluFrameTsRead(IAE_DRVG_sluCfgT  *sluCfg);
ERRG_codeE IAE_DRVG_setSluInterruptEnable(UINT32 mask, UINT32 slu);
ERRG_codeE IAE_DRVG_iaeReady();
ERRG_codeE IAE_DRVG_setInterruptCb(IAE_DRVG_setInterruptCbT *setInterruptCb);
ERRG_codeE IAE_DRVG_setHistInterrupt( IAE_DRVG_iauSelectE histSelect, UINT32 on );
ERRG_codeE IAE_DRVG_accessIaeRegister(IAE_DRVG_iaeParametersT *params);
ERRG_codeE IAE_DRVG_dumpRegs(void);
ERRG_codeE IAE_DRVG_setBaseDir0(int inst, int val);
ERRG_codeE IAE_DRVG_setForceXMode(int inst, int val);
void       IAE_DRVG_configDefaultPhySelect();
void       IAE_DRVG_configPhySelect(int inst, int val);
ERRG_codeE IAE_DRVG_registerSnsrIsrCb(IAE_DRVG_snsrInterruptCbT cb, void *argP, void **entryP);
ERRG_codeE IAE_DRVG_unregisterSnsrIsrCb(void *entryP);
ERRG_codeE IAE_DRVG_registerPhyCb(IAE_DRVG_phyInterruptCbT cb, unsigned int ind);
void       IAE_DRVG_enablePwmDim(unsigned int pwmNum,unsigned int fps,unsigned int widthInUsec);
void       IAE_DRVG_disablePwmDim(unsigned int pwmNum);
UINT32     IAE_DRVG_readSpare();
void IAE_DRVG_FSG_swTrigger0();
void IAE_DRVG_FSG_swTrigger1();
void IAE_DRVG_FSG_swTrigger2();

void IAE_DRVG_setRamManagerMode(UINT32 mode);
UINT32 IAE_DRVG_getRamManagerMode(void);
void IAE_DRVG_disableAllSlu();
void IAE_DRVG_enableAllSlu();
/**
 * @brief Configures the FSG_Counter register with the configuration defined in config
 * 
 * 
 *
 * @param config Configuration for the IAE_SNSR_CTRL register
 * @return Returns an error code if invalid arguments are detected
 */
ERRG_codeE IAE_DRVG_FSGCntRegisterCfg(const IAE_DRVG_snsrCtrlFsgCntrCtrl *config);
/**
 * @brief Configures the FSG Pulse control register defined by Config
 * 
 *
 * @param counter FSG Counter(0->2)
 * @param config Pulse configuration to use
 * @return ERRG_codeE Error code returned
 */
ERRG_codeE IAE_DRVG_FSGPulseCtrlCfg(const IAE_DRVG_fsgCounterNumE counter, const IAE_DRVG_snsrCtrlFSGFtrigPulseCtrl *config);
/**
 * @brief Sets the counter period for an FSG Counter
 * 
 * @param counter FSG Counter (0->2)
 * @param period  Period to use (Raw units, 1 count = 1/250MHz = 4ns)
 * @return ERRG_codeE Error code returned
 */
ERRG_codeE IAE_DRVG_setFSGCntPeriod(const IAE_DRVG_fsgCounterNumE counter,const UINT32 period);
/**
 * @brief Sets the Pulse offset for an FSG counter
 * 
 * 
 *
 * @param counter FSG Counter (0->2)
 * @param offset Pulse offset to be set
 * @return ERRG_codeE Error code returned
 */
ERRG_codeE IAE_DRVG_setPulseOffset(const IAE_DRVG_fsgCounterNumE counter,const UINT32 offset);
/**
 * @brief Registers the eof interrupt callback
 * 
 * 
 *
 * @param cb Interrupt handler to hall
 * @param argP Arg to use
 * @param entryP This pointer will point to the entry where this CB is stored
 * @return Returns an error code
 */
ERRG_codeE IAE_DRVG_registerEofISRCb(IAE_DRVG_snsrInterruptCbT cb, void *argP, void **entryP);
/**
 * @brief Unregisters the EOF CB
 * 
 */
ERRG_codeE IAE_DRVG_unregisterEofISRCb(void *entryP);
/**
 * @brief Sets the Count mode for the FSGCount[Counter](0->2)  to cntMode
 * 
 *
 * @param counter Counter(0->2)
 * @param cntMode Count mode to use
 * @return Returns an error code
 */
ERRG_codeE IAE_DRVG_setFSGCntMode(const IAE_DRVG_fsgCounterSrc counter,const IAE_DRVG_fsgCntMode cntMode );
/**
 * @brief Sets the Trigger source for FSGCount[Counter] (0->2) to trigSrc
 * 
 * 
 *
 * @param counter FSG Counter(0->2)
 * @param trigSrc 
 * FSG counter trigger source:
 *  0...5 - SLU SOF
 *  6...11 - SLU EOF
 *  12..14 - SW sync
 *  15 - Reserved
 *  16...31 - Ext sync
 * @return Returns an error code
 */
ERRG_codeE IAE_DRVG_setFSGTrigSrc(const IAE_DRVG_fsgCounterSrc counter,const UINT8 trigSrc );
/**
 * @brief Sets the pulse repetition period for FSGCount[Counter] (0->2) to period
 * 
 *
 * @param counter FSG Counter(0->2)
 * @param period Pulse repetition period
 * @return Returns an error code
 */
ERRG_codeE IAE_DRVG_setFSGPulseRepeatPeriod(const IAE_DRVG_fsgCounterNumE counter, const UINT32 period);
/**
 * @brief Modifies the FSG Counter period to acheive a desired FPS
 * 
 *
 * @param counter Counter (0->2)
 * @param FPS     Desired FPS
 * @return Returns an error code
 */
ERRG_codeE IAE_DRVG_setFSGCounterPeriod(IAE_DRVG_fsgCounterSrc counter, double FPS);
void  IAE_DRVG_FSG_sensEnable();
void  IAE_DRVG_FSG_sensDisable();
void  IAE_DRVG_enableFTRIGOutput(IAE_DRVG_fsgCounterNumE fsgTrigger);
void IAE_DRVG_disableFTRIGOutput(IAE_DRVG_fsgCounterNumE fsgTrigger);
UINT32 IAE_DRBVG_getCurrentSLUEnableValue();
void IAE_DRVG_getSLUWriteValueEnable(const IAE_DRVG_enableModuleE  *enableBlock, UINT32 *enableValue);
void IAE_DRVG_getSLUWriteValueDisable(const IAE_DRVG_enableModuleE  *enableBlock, UINT32 *disableValue);
void IAE_DRVG_writeSLUEnable(UINT32 value);
void IAE_DRVG_writeFSGEnable(IAE_DRVG_fsgCounterSrc counter, bool enable);
UINT32 IAE_DRBG_getFSGCounterPeriod(IAE_DRVG_fsgCounterSrc counter);
void IAE_DRVG_FSG_swTrigger(IAE_DRVG_fsgCounterSrc fsgTrigger);
#endif //IAE_DRV_H

