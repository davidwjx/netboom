#ifndef __NU4K_DEFS_H__
#define __NU4K_DEFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#define NU4K_NUM_GENS                   (4)
#define NU4K_NUM_SENSORS                (12)
#define NU4K_NUM_MEDIATORS              (6)
#define NU4K_NUM_MIPI_RX                (6)
#define NU4K_NUM_PAR_RX                 (2)
#define NU4K_NUM_WRITERS                (6)
#define NU4K_NUM_SLUS_PARALLEL          (2)
#define NU4K_NUM_SLUS                   (4)
#define NU4K_NUM_ISPS                   (2)
#define NU4K_NUM_IAUS                   (2)
#define NU4K_NUM_IAUS_COLOR             (2)
#define NU4K_NUM_HIST                   (2)
#define NU4K_NUM_PPUS_NOSCL             (4)
#define NU4K_NUM_PPUS_SCL               (2)
#define NU4K_NUM_PPUS_HYBSCL            (2)
#define NU4K_NUM_PPUS_HYB               (3)
#define NU4K_NUM_AXI_READERS            (12)
#define NU4K_NUM_CVA_READERS            (7)
#define NU4K_NUM_AXI_INTERLEAVERS       (12)
#define NU4K_NUM_HISTOGRAM_READER       (2)
#define NU4K_NUM_MIPI_STREAMS           (12)
#define NU4K_NUM_MIPI_INTERLEAVERS      (3)
#define NU4K_NUM_PARALLEL_STREAMS       (2)
#define NU4K_NUM_PARALLEL_INTERLEAVERS  (1)
#define NU4K_NUM_DPES                   (2)
#define NU4K_NUM_DPES_HYB               (1)
#define NU4K_NUM_DPPS                   (2)
#define NU4K_NUM_CVAS                   (1)
#define NU4K_NUM_DPHY_TX                (2)
#define NU4K_NUM_VSC_CSI_TX             (4)
#define NU4K_NUM_ISP_READERS            (6)



#define NU4K_NUM_INPUTS                 (NU4K_NUM_SENSORS) // + NU4K_NUM_WRITERS)
#define NU4K_NUM_OUTPUTS                (NU4K_NUM_AXI_INTERLEAVERS + NU4K_NUM_HISTOGRAM_READER + NU4K_NUM_CVA_READERS + NU4K_NUM_DPHY_TX + NU4K_NUM_ISP_READERS)// + NU4K_NUM_MIPI_INTERLEAVERS + NU4K_NUM_PARALLEL_INTERLEAVERS)

#define NU4K_INTER_NUM_INPUTS   (4)
#define NU4K_PPU_NUM_INPUTS     (1)
#define NU4K_PPU_HYB_NUM_INPUTS (2)
#define NU4K_PPU_NUM_OUTPUTS    (4)
#define NU4K_DPE_NUM_INPUTS     (2) //L/R
#define NU4K_DPE_HYB_NUM_INPUTS (2)

#define NU4K_MAX_INTERLEAVED             (4)
#define NU4K_INTERLEAVER_GROUP(n)        (n/NU4K_MAX_INTERLEAVED)
#define NU4K_INTERLEAVER_BASE(n)         ((n/NU4K_MAX_INTERLEAVED)*NU4K_MAX_INTERLEAVED)

#define NU4K_HW_PATH_EMPTY_ENTRY         (0xFF)
#define NU4K_MAX_DB_META_PATHS           (20)
//IIM input selection fields
#define NU4K_IAU_SELECT_MIN_SLU_PARALLEL (0)
#define NU4K_IAU_SELECT_MAX_SLU_PARALLEL (1)
#define NU4K_IAU_SELECT_MIN_SLU (2)
#define NU4K_IAU_SELECT_MAX_SLU (5)
#define NU4K_IAU_SELECT_MIN_INJ (6)
#define NU4K_IAU_SELECT_MAX_INJ (11)
#define NU4K_IAU_SELECT_MIN_GEN (12)
#define NU4K_IAU_SELECT_MAX_GEN (15)

//AXI reader and ppu source selection fields
#define NU4K_PPU_SRC_SEL_MIN_SLU_PARALLEL (0)
#define NU4K_PPU_SRC_SEL_MAX_SLU_PARALLEL (1)
#define NU4K_PPU_SRC_SEL_MIN_SLU (2)
#define NU4K_PPU_SRC_SEL_MAX_SLU (5)
#define NU4K_PPU_SRC_SEL_MIN_IAU (6)
#define NU4K_PPU_SRC_SEL_MAX_IAU (7)
#define NU4K_PPU_SRC_SEL_MIN_IAU_COLOR (8)
#define NU4K_PPU_SRC_SEL_MAX_IAU_COLOR (9)
#define NU4K_PPU_SRC_SEL_MIN_INJ (10)
#define NU4K_PPU_SRC_SEL_MAX_INJ (15)
#define NU4K_PPU_SRC_SEL_MIN_CVJ (16)
#define NU4K_PPU_SRC_SEL_MAX_CVJ (20)
#define NU4K_PPU_SRC_SEL_DPE (21)
#define NU4K_PPU_SRC_SEL_WEBREG (22)
#define NU4K_PPU_SRC_SEL_DPP_HOST (23)
#define NU4K_PPU_SRC_SEL_DPP_CVA (24)
#define NU4K_PPU_SRC_SEL_MIN_PPU_NOSCL (32)
#define NU4K_PPU_SRC_SEL_MAX_PPU_NOSCL (47)
#define NU4K_PPU_SRC_SEL_MIN_PPU_SCL (48)
#define NU4K_PPU_SRC_SEL_MAX_PPU_SCL (55)
#define NU4K_PPU_SRC_SEL_MIN_PPU_HYBSCL (64)
#define NU4K_PPU_SRC_SEL_MAX_PPU_HYBSCL (71)
#define NU4K_PPU_SRC_SEL_MIN_PPU_HYB  (80)
#define NU4K_PPU_SRC_SEL_MAX_PPU_HYB  (91)

#define NU4K_INTERLEAVE_MODE_PIXEL (1)
#define NU4K_INTERLEAVE_MODE_LINE  (0)

#define NU4K_PPU_SCL_MODE_BYPASS (0)
#define NU4K_PPU_SCL_MODE_HORZ_ONLY (1)
#define NU4K_PPU_SCL_MODE_VERT_ONLY (2)
#define NU4K_PPU_SCL_MODE_HORZ_N_VERT (3)
#define NU4K_PPU_SCL_MODE_VERT_N_HORZ (4)

#define NU4K_PPU_STITCH_BYPASS (1)

//DPE
#define NU4K_DPE_HYBRID_DISABLE    (0)
#define NU4K_DPE_HYBRID_ENABLE    (1)

// DEPTH POST cropping src selection
#define NU4K_DPP_SRCSEL_REG_PRIM_DEPTH  (0)
#define NU4K_DPP_SRCSEL_PRIM_DISP       (1)
#define NU4K_DPP_SRCSEL_PRIM_DEPTH      (2)
#define NU4K_DPP_SRCSEL_SEC_DISP        (3)
#define NU4K_DPP_SRCSEL_SEC_DEPTH       (4)

#define NU4K_DPP_SCL_MODE_BYPASS    (0)
#define NU4K_DPP_SCL_MODE_HORZ_ONLY (1)
#define NU4K_DPP_SCL_MODE_VERT_ONLY (2)
#define NU4K_DPP_SCL_MODE_HORZ_VERT (3)

//IAUs bypass bits
#define NU4K_IAU_BYPASS_BPC0_BIT (0)
#define NU4K_IAU_BYPASS_YUV0_BIT (1)
#define NU4K_IAU_BYPASS_IB0_BIT  (2)
#define NU4K_IAU_BYPASS_WB0_BIT  (3)
#define NU4K_IAU_BYPASS_DMS0_BIT (4)
#define NU4K_IAU_BYPASS_GCR0_BIT (5)
#define NU4K_IAU_BYPASS_MED0_BIT (6)
#define NU4K_IAU_BYPASS_CSC0_BIT (7)
#define NU4K_IAU_BYPASS_DSR0_BIT (8)

#define NU4K_IAU_BYPASS_BPC1_BIT (12)
#define NU4K_IAU_BYPASS_YUV1_BIT (13)
#define NU4K_IAU_BYPASS_IB1_BIT  (14)
#define NU4K_IAU_BYPASS_WB1_BIT  (15)
#define NU4K_IAU_BYPASS_DMS1_BIT (16)
#define NU4K_IAU_BYPASS_GCR1_BIT (17)
#define NU4K_IAU_BYPASS_MED1_BIT (18)
#define NU4K_IAU_BYPASS_CSC1_BIT (19)
#define NU4K_IAU_BYPASS_DSR1_BIT (20)

#define NU4K_IAU_BYPASS_YUV2_BIT (24)
#define NU4K_IAU_BYPASS_CSC2_BIT (25)
#define NU4K_IAU_BYPASS_DSR2_BIT (26)

#define NU4K_IAU_BYPASS_YUV3_BIT (28)
#define NU4K_IAU_BYPASS_CSC3_BIT (29)
#define NU4K_IAU_BYPASS_DSR3_BIT (30)

#define NU4K_IAE_GENERATOR_BPP (16)

#define NU4K_DIF_SCL_MODE_BYPASS    (0)
#define NU4K_DIF_SCL_MODE_HORZ_ONLY (1)
#define NU4K_DIF_SCL_MODE_VERT_ONLY (2)
#define NU4K_DIF_SCL_MODE_H_THEN_V  (3)
#define NU4K_DIF_SCL_MODE_V_THEN_H  (4)

typedef enum {
   //DPE debug output select
   NU4K_DPE_OUTSEL_MAIN_E = 0,  //Main dpe output 20   4 conf, 10 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_DIF_LR_E = 2,  //DIF Left & Right output   20   Y left, Y right
   NU4K_DPE_OUTSEL_EDGE_DET_LEFT_E = 3,//Edge detector left output  19   1 edge, 8 census, 10 Y {edge, Census, Y}
   NU4K_DPE_OUTSEL_EDGE_DET_RIGHT_E = 18,//Edge detector right output   19   1 edge, 8 census, 10 Y {edge, Census, Y}
   NU4K_DPE_OUTSEL_AGGREGATION_LEFT_E = 4,//Aggregation output left   24   cost of DISP_SEL (0�143, and ANGLE_SEL (0, 45, 90, 135 and regular, {cost left}
   NU4K_DPE_OUTSEL_AGGREGATION_RIGHT_E = 19,   //Aggregation output right   24   cost of DISP_SEL (0�143, and ANGLE_SEL (0, 45, 90, 135 and regular, {cost right}
   NU4K_DPE_OUTSEL_REGULAR_DISPARITY_MAP_SORT_LEFT_E = 5,   //20   12 cost + 8 disparity index for DISP_SEL (0�5,
   NU4K_DPE_OUTSEL_REGULAR_DISPARITY_MAP_SORT_RIGHT_E = 20,//20   12 cost + 8 disparity index for DISP_SEL (0�5,
   NU4K_DPE_OUTSEL_SMALL_DISPARITY_MAP_SELECT_LEFT_E = 6,//20   12 cost + 8 disparity index for DISP_SEL (0�1, and ANGLE_SEL (0�3,
   NU4K_DPE_OUTSEL_SMALL_DISPARITY_MAP_SELECT_RIGHT_E = 21,   //20   12 cost + 8 disparity index for DISP_SEL (0�1, and ANGLE_SEL (0�3,
   NU4K_DPE_OUTSEL_SMALL_DISPARITY_MAP_UNITE_E = 7, //24 4 conf + 12 cost + 8 disparity index for ANGLE_SEL (0�3,
   NU4K_DPE_OUTSEL_SMALL_DISP_MAP_REG_DISP_SELECT_E = 8, //20   12 cost + 8 bits disparity index
   NU4K_DPE_OUTSEL_DISPARITY_MAP_SELECTOR_OUTPUT_E = 9, //23   3 selector + 12 cost + 8 disparity (using to map selector,
   NU4K_DPE_OUTSEL_FINAL_SMALL_DISPARITY_E = 10, //13   1 map select + 4 bits confidence + 8 bits disparity
   NU4K_DPE_OUTSEL_EDGE_DISTANCE_LEFT_E = 11, //13   1 edge + 12 edge distance mask
   NU4K_DPE_OUTSEL_EDGE_DISTANCE_RIGHT_E = 1, //13   1 edge + 12 edge distance mask
   NU4K_DPE_OUTSEL_OPTIMIZATION_OUTPUT_LEFT_E = 12, //20   12 cost + 8 disparity index for DISP_SEL (0�2,
   NU4K_DPE_OUTSEL_OPTIMIZATION_OUTPUT_RIGHT_E = 22, //20   12 cost + 8 disparity index for DISP_SEL (0�2,
   NU4K_DPE_OUTSEL_UNITE_E = 13, // 1 edge + 4 bits confidence + 8 bits disparity
   NU4K_DPE_OUTSEL_SAD_CENSUS_SUB_PIXEL_E = 14,// 8 SAD subpixel, 8 census subpixel S1.6
   NU4K_DPE_OUTSEL_SUB_PIXEL_E = 15,//19   1 edge, 4 conf, 8 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_BLOB_0_OUTPUT_E = 16,//20   Blob0 invalid, 1 edge, 4 conf, 8 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_BLOB_1_OUTPUT_E = 17,//21   Blob1 fill, blob1 invalid, 1 edge, 4 conf, 8 disp, 6 subpixel
   NU4K_DPE_OUTSEL_BLOB_OUTPUT_E = 23, //19   1 edge, 4 conf, 8 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_DISPARITY_RESTRUCTURE_OUTPUT_E = 24,  //21   1 edge, 4 conf, 10 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_PART_0_SCALED_IMAGE_OUTPU_TE = 25,  //20   4 conf, 10 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_PART_1_SCALED_IMAGE_OUTPUT_E = 26,  //4 conf, 10 disparity, 6 subpixel
   NU4K_DPE_OUTSEL_INPUT_0_E = 28, //10   Y
   NU4K_DPE_OUTSEL_INPUT_1_E = 29, //10   Y
   NU4K_DPE_OUTSEL_INPUT_2_E = 30, //10   Y
   NU4K_DPE_OUTSEL_INPUT_3_E = 31, //10   Y

   NU4K_DPE_NUM_FORMATS_E = 32,
} nu4kDpeOutFormatE;

/*
*							------- roi 0 -------
*							0x000 - 0x3ff red
*							0x400 - 0x7ff green
*							0x800 - 0xbff blue
*
*							------- roi 1 -------
*							0xc00 - 0xfff red
*							0x1000 - 0x13ff green
*							0x1400 - 0x17ff blue
*
*							------- roi 0 -------
*							0x1800 - 0x1bff red
*							0x1c00 - 0x1fff green
*							0x2000 - 0x23ff blue
*
*							------- roi 0 ------
*							0x2400 - 0x2403 accumulator red
*							0x2404 - 0x2407 accumulator green
*							0x2408 - 0x240b accumulator blue
*
*							------- roi 1 ------
*							0x240c - 0x240f accumulator red
*							0x2410 - 0x2413 accumulator green
*							0x2414 - 0x2417 accumulator blue
*
*							------- roi 2 ------
*							0x2418 - 0x241b accumulator red
*							0x241c - 0x241f accumulator green
*							0x2420 - 0x2424 accumulator blue
*
*							------- frame counter -------
*							0x2424 - 0x2428
*/
#define HIST_TOTAL_SIZE (0x2424)

typedef enum
{
   HIST_CIIF_INPUT_SLU_E  = 0,
   HIST_CIIF_INPUT_IB_E   = 1,
   HIST_CIIF_INPUT_WB_E   = 2,
   HIST_CIIF_INPUT_GCR_E  = 3,
   HIST_CIIF_INPUT_MED_E  = 4,
   HIST_CIIF_INPUT_CSC_E  = 5,
   HIST_CIIF_INPUT_DSR_E  = 6,
   HIST_CIIF_INPUT_NUMS_E = 7,
} nu4kHistCiifInputE;


typedef enum
{
   SLU_PARALLEL_INTERFACE_BT601_E  = 0,
   SLU_CSI2_INTERFACE_E            = 1,
   SLU_PARALLEL_INTERFACE_BT656_E  = 2,
   SLU_DSI2_INTERFACE_E            = 3,
} nu4kSluInterfaceTypeE;

/*
 slu0_control_data_format
 0 RGB888 | 1 RGB666| 2 RGB565| 3 RGB555| 4 RGB444 | 5 YUV422 8 bit| 6 YUV422, 10 bit| 7 YUV420, 8bit legacy|
 8 YUV420 8 bit | 9 YUV420, 10 bit| 10 RAW6 | 11 RAW7 | 12 RAW8 | 13 RAW10 | 14 RAW12| 15 RAW14
 */
typedef enum
{
   SLU_FORMAT_RGB888_E = 0,
   SLU_FORMAT_RGB666_E,
   SLU_FORMAT_RGB565_E,
   SLU_FORMAT_RGB555_E,
   SLU_FORMAT_RGB444_E,
   SLU_FORMAT_YUV422_8B_E,
   SLU_FORMAT_YUV422_10B_E,
   SLU_FORMAT_YUV420_8B_LEGACY_E,
   SLU_FORMAT_YUV420_8B_E,
   SLU_FORMAT_YUV420_10B_E,
   SLU_FORMAT_RAW6_E,
   SLU_FORMAT_RAW7_E,
   SLU_FORMAT_RAW8_E,
   SLU_FORMAT_RAW10_E,
   SLU_FORMAT_RAW12_E,
   SLU_FORMAT_RAW14_E,

   SLU_NUM_FORMATS_E,
} nu4kSluFormatE;

typedef enum
{
   MIPI_MUX0_SEL_BLOCKED_E = 0,
   MIPI_MUX0_SEL_PHY5_E    = 1,
   NUM_MIPI_MUX0_SEL_OPTIONS
} nu4kMipiMuxSlu0SelectionE;

typedef enum
{
   MIPI_MUX1_LANES01_SEL_PHY1_E    = 0,
   MIPI_MUX1_LANES01_SEL_PHY5_E    = 1,
   NUM_MIPI_MUX1_LANES01_SEL_OPTIONS
} nu4kMipiMuxSlu1Lanes01SelectionE;

typedef enum
{
	MIPI_MUX1_LANES23_SEL_BLOCKED_E = 0,
	MIPI_MUX1_LANES23_SEL_PHY4_E    = 1,
	NUM_MIPI_MUX1_LANES23_SEL_OPTIONS
} nu4kMipiMuxSlu1Lanes23SelectionE;

typedef enum
{
   MIPI_MUX2_SEL_BLOCKED_E = 0,
   MIPI_MUX2_SEL_PHY3_E    = 1,
   NUM_MIPI_MUX2_SEL_OPTIONS
} nu4kMipiMuxSlu2SelectionE;

typedef enum
{
   MIPI_MUX3_LANES01_SEL_PHY1_E     = 0,
   MIPI_MUX3_LANES01_SEL_PHY2_E     = 1,
   MIPI_MUX3_LANES01_SEL_PHY3_E     = 2,
   MIPI_MUX3_LANES01_SEL_NA_E       = 3,
   NUM_MIPI_MUX3_LANES01_SEL_OPTIONS
} nu4kMipiMuxSlu3Lanes01SelectionE;

typedef enum
{
	MIPI_MUX3_LANES23_SEL_BLOCKED_E  = 0,
	MIPI_MUX3_LANES23_SEL_PHY3_E     = 1,
	MIPI_MUX3_LANES23_SEL_PHY4_E     = 2,
	MIPI_MUX3_LANES23_SEL_NA_E       = 3,
	NUM_MIPI_MUX3_LANES23_SEL_OPTIONS
} nu4kMipiMuxSlu3Lanes23SelectionE;

typedef enum
{
   MIPI_MUX4_LANES01_SEL_PHY1_E     = 0,
   MIPI_MUX4_LANES01_SEL_PHY4_E     = 1,
   MIPI_MUX4_LANES01_SEL_PHY5_E     = 2,
   NUM_MIPI_MUX4_LANES01_SEL_OPTIONS
} nu4kMipiMuxSlu4Lanes01SelectionE;

typedef enum
{
	MIPI_MUX4_LANES23_SEL_BLOCKED_E  = 0,
	MIPI_MUX4_LANES23_SEL_PHY4_E     = 1,
	NUM_MIPI_MUX4_LANES23_SEL_OPTIONS
} nu4kMipiMuxSlu4Lanes23SelectionE;

typedef enum
{
   MIPI_MUX5_LANES01_SEL_PHY0_E     = 0,
   MIPI_MUX5_LANES01_SEL_PHY5_E     = 1,
   NUM_MIPI_MUX5_LANES01_SEL_OPTIONS
} nu4kMipiMuxSlu5Lanes01SelectionE;

typedef enum
{
	MIPI_MUX5_LANES23_SEL_BLOCKED_E  = 0,
	MIPI_MUX5_LANES23_SEL_PHY5_E     = 1,
	NUM_MIPI_MUX5_LANES23_SEL_OPTIONS
} nu4kMipiMuxSlu5Lanes23SelectionE;

/*
iau0_data_format:
 0: Grey| 1: Bayer| 2: RGB | 3: YUV422 |  4: YUV420L | 5: YUV420
*/
typedef enum
{
   IAU_FORMAT_GREY_E = 0,
   IAU_FORMAT_BAYER_E,
   IAU_FORMAT_RGB_E,
   IAU_FORMAT_YUV422_E,
   IAU_FORMAT_YUV420L_E,
   IAU_FORMAT_YUV420_E,

   IAU_NUM_FORMATS_E,
} nu4kIauFormatE;

/*
 0...4 - RGB888/666/565/555/444
 5.. 6 - YUV422 8b/10b
 7..12 - RAW6/7/8/10/12/14
 13..16 - Generic 8/12/16/24
 17..19 - Generic 32/64/96, key point
 */
typedef enum
{
   WRITER_FORMAT_RGB888_E = 0,
   WRITER_FORMAT_RGB666_E,
   WRITER_FORMAT_RGB565_E,
   WRITER_FORMAT_RGB555_E,
   WRITER_FORMAT_RGB444_E,
   WRITER_FORMAT_YUV422_8B_E,
   WRITER_FORMAT_YUV422_10B_E,
   WRITER_FORMAT_RAW6_E,
   WRITER_FORMAT_RAW7_E,
   WRITER_FORMAT_RAW8_E,
   WRITER_FORMAT_RAW10_E,
   WRITER_FORMAT_RAW12_E,
   WRITER_FORMAT_RAW14_E,
   WRITER_FORMAT_GEN8_E,
   WRITER_FORMAT_GEN12_E,
   WRITER_FORMAT_GEN16_E,
   WRITER_FORMAT_GEN24_E,
   WRITER_FORMAT_GEN32_E,
   WRITER_FORMAT_GEN64_E,
   WRITER_FORMAT_GEN96_E,
   WRITER_FORMAT_YUV420_SEMI_PLANAR_E,
   WRITER_NUM_FORMATS_E,
}nu4kWriterFormatE;

typedef enum
{
   PPU_FORMAT_RGB_E = 0,
   PPU_FORMAT_YUV422_E,
   PPU_FORMAT_GREY_E,
   PPU_FORMAT_NA_E,
   PPU_NUM_FORMATS_E,
}nu4kPpuOutFormatE;


// mipi video formats
typedef enum {
   /*  Name   Code (hex)   Bits per pixel*/
   YUV420_8_LEGACY = 0x1a,		// 8
   YUV420_8 = 0x18,		// 8
   YUV420_10 = 0x19,		// 10
   RGB444 = 0x20,		// 16
   RGB555 = 0x21,		// 16
   RGB565 = 0x22,		// 16
   RGB666 = 0x23,		// 18
   RGB888 = 0x24,		// 24
   RAW6 = 0x28,	// 6
   RAW7 = 0x29,	// 7
   YUV422_8 = 0x1e,		// 8
   YUV422_10 = 0x1f,		// 10
   RAW8 = 0x2a,	// 8
   RAW10 = 0x2b,		// 10
   RAW12 = 0x2c,	// 12
   RAW14 = 0x2d,		// 14
} nu4kCsiOutFormatE;

#ifdef __cplusplus
}
#endif

#endif //__NU4K_DEFS_H__

