/****************************************************************************
 *
 *   FileName: AR2020_cfg_tbl.h
 *
 *   Author: Benny V.
 *
 *   Date:
 *
 *   Description: AR2020 layer
 *
 ****************************************************************************/
#ifndef AR2020_CFG_TBL_H
#define AR2020_CFG_TBL_H

#include "inu_common.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GAIN_LUT_SIZE (128)
#define AR2020_RESET_CONTROL_VALUE                                    (0x708) /*Enable dropping bad frames*/
/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/
float AR2020_DRVP_gainLut[GAIN_LUT_SIZE] =
{
    1.000,   // 0
    1.044,   // 1
    1.09,    // 2
    1.138,   // 3
    1.189,   // 4
    1.241,   // 5
    1.296,   // 6
    1.353,   // 7
    1.413,   // 8
    1.475,   // 9
    1.54,    // 10
    1.608,   // 11
    1.679,   // 12
    1.753,   // 13
    1.83,    // 14
    1.911,   // 15
    1.995,   // 16
    2.083,   // 17
    2.175,   // 18
    2.271,   // 19
    2.371,   // 20
    2.476,   // 21
    2.585,   // 22
    2.699,   // 23
    2.818,   // 24
    2.943,   // 25
    3.073,   // 26
    3.208,   // 27
    3.35,    // 28
    3.497,   // 29
    3.652,   // 30
    3.813,   // 31
    3.981,   // 32
    4.157,   // 33
    4.34,    // 34
    4.532,   // 35
    4.732,   // 36
    4.94,    // 37
    5.158,   // 38
    5.386,   // 39
    5.623,   // 40
    5.872,   // 41
    6.131,   // 42
    6.401,   // 43
    6.683,   // 44
    6.978,   // 45
    7.286,   // 46
    7.608,   // 47
    7.943,   // 48
    8.294,   // 49
    8.66,    // 50
    9.042,   // 51
    9.441,   // 52
    9.857,   // 53
    10.292,  // 54
    10.746,  // 55
    11.22,   // 56
    11.715,  // 57
    12.232,  // 58
    12.772,  // 59
    13.335,  // 60
    13.924,  // 61
    14.538,  // 62
    15.179,  // 63
    15.849,  // 64
    16.548,  // 65
    17.278,  // 66
    18.041,  // 67
    18.836,  // 68
    19.668,  // 69
    20.535,  // 70
    21.441,  // 71
    22.387,  // 72
    23.375,  // 73
    24.406,  // 74
    25.483,  // 75
    26.607,  // 76
    27.781,  // 77
    29.007,  // 78
    30.287,  // 79
    31.623,  // 80
    33.018,  // 81
    34.475,  // 82
    35.996,  // 83
    37.584,  // 84
    39.242,  // 85
    40.973,  // 86
    42.781,  // 87
    44.668,  // 88
    46.639,  // 89
    48.697,  // 90
    50.845,  // 91
    53.088,  // 92
    55.431,  // 93
    57.876,  // 94
    60.43,   // 95
    63.096,  // 96
    65.879,  // 97
    68.786,  // 98
    71.821,  // 99
    74.989,  // 100
    78.298,  // 101
    81.752,  // 102
    85.359,  // 103
    89.125,  // 104
    93.057,  // 105
    97.163,  // 106
    101.45,  // 107
    105.925, // 108
    110.599, // 109
    115.478, // 110
    120.573, // 111
    125.893, // 112
    131.447, // 113
    137.246, // 114
    143.301, // 115
    149.624, // 116
    156.225, // 117
    163.117, // 118
    170.314, // 119
    177.828, // 120
    185.674, // 121
    193.865, // 122
    202.418, // 123
    211.349, // 124
    220.673, // 125
    230.409, // 126
    240.575, // 127
};



typedef enum {
    AR2020_CFG_TBLG_SINGLE_TABLE_E        = 0,
    AR2020_CFG_TBLG_STEREO_TABLE_E,
    AR2020_CFG_TBLG_NUM_OF_TABLES_E
} AR2020_CFG_TBLG_sensorTablesE;
/**
 * Continuous GRR Mode Example Settings:
REG = 0x0104, 0x0001
//GROUPED_PARAMETER_HOLD
REG = 0x3294, 0x7009 //GLOBAL_SEQ_TRIGGER
REG = 0x333A, 0x6448 //GRR_DONE_CTRL
REG = 0x3296, 0x00EC //GLOBAL_RST_END
REG = 0x3298, 0x00A0 //GLOBAL_SHUTTER_START
REG = 0x329C, 0x00A1 //GLOBAL_READ_START
REG = 0x0104, 0x0000
//GROUPED_PARAMETER_HOLD
*/
const GEN_SENSOR_DRVG_regTblParamsT AR2020_GRR_TABLE[] = 
{
{0x104,0x1,1},
{0x3294,0x2,0x7009},
{0x333A,0x2,0x6448},
{0x3296,0x2,0x00EC},
{0x3298,0x2,0x00A0},
{0x329C,0x2,0x00A1},
{0x0104,0x1,0}
};

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_trigger_table[] =
{
    // {0x0100, 1, 0x00},
    // {0x301A, 2, 0x0108},
    // {0x30C0, 2, 0xfdf3},//{0x30C0, 2, 0xfd73},//{0x30C0, 2, 0xfdf3},
    // //{0x0202, 2, 1500},
    // {0x3140, 2, 0x0001},
    // {0x0100, 1, 1}, // MODE_SELECT
        /*
        REG= 0x44D6, 0xF206 // DAC_LD_32_33
REG= 0x0100, 0x00 // MODE_SELECT
REG= 0x301A, 0x0108 // RESET_REGISTER   // Enable GPI
REG= 0x30C0, 0xFDF3 // GPI_STATUS       // GPI_TRIGGER0_PIN_SEL choose GPIO3
REG= 0x3140, 0x0001 // TRIGGER_MODE 1
REG= 0x340E, 0x6188 // ANALOG_CTRL9
REG= 0x3F82, 0x0800 // DIGITAL_TEST
REG= 0x0100, 0x01 // MODE_SELECT
REG= 0x44D6, 0xB206 // DAC_LD_32_33
DELAY = 10
REG= 0x3F82, 0x0000 // DIGITAL_TEST*/

    { 0x44D6,2, 0xF206}, // DAC_LD_32_33
    {0x0100,1, 0x00}, // MODE_SELECT
    {0x301A,2, 0x0508}, // RESET_REGISTER   // Enable GPI
    {0x30C0,2, 0xFDF3}, // GPI_STATUS       // GPI_TRIGGER0_PIN_SEL choose GPIO3
    {0x3140,2, 0x0001}, // TRIGGER_MODE 1
    {0x340E,2, 0x6188}, // ANALOG_CTRL9
    {0x3F82,2, 0x0800}, // DIGITAL_TEST
    {0x0100,1, 0x01}, // MODE_SELECT
    {0x44D6,2, 0xB206}, // DAC_LD_32_33
//DELAY = 10
//REG= 0x3F82, 0x0000 // DIGITAL_TEST

};
const GEN_SENSOR_DRVG_regTblParamsT AR2020_DigitalTestDelayed[] = 
{
    {0x3F82,2, 0x0000} // DIGITAL_TEST
};
const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_800x736_672x608_table[] =
{
    {0x0103, 0x3201, 0x01}, // SOFTWARE_RESET

    { 0x0304, 0x2,  0x2 }, // PRE_PLL_CLK_DIV1=2
    { 0x0306, 0x2,  0x56 }, // PLL_MULTIPLIER1=86
    { 0x0300, 0x2, 0x6 }, // VT_PIX_CLK_DIV=6
    { 0x0302, 0x2, 0x1 }, // VT_SYS_CLK_DIV=1
    { 0x030C, 0x2,  0x7 }, // PRE_PLL_CLK_DIV2=7
    { 0x030E, 2,   0x247 }, // { 0x030E, 0x2,  0x103 }, // PLL_MULTIPLIER2=259
    { 0x0308, 2,  0x5 }, // { 0x0308, 0x2, 0xA }, // OP_PIX_CLK_DIV=10
    { 0x030A, 2,  0x2 }, // { 0x030A, 0x2, 0x1 }, // OP_SYS_CLK_DIV=1
    {0x0344, 0x2, 0x3C8 }, // X_ADDR_START 968
    {0x0348, 0x2, 0x1047 }, // X_ADDR_END 4167
    {0x0346, 0x2, 0x1C8 }, // Y_ADDR_START 456
    {0x034A, 0x2, 0xD47 }, // Y_ADDR_END 3399
    {0x034C, 0x2, 0xC80 }, // X_OUTPUT_SIZE 3200
    {0x034E, 0x2, 0xB80 }, // Y_OUTPUT_SIZE 2944
    { 0x0380, 0x2,  0x1 }, // X_EVEN_INC
    { 0x0382, 0x2,  0x1 }, // X_ODD_INC
    { 0x0384, 0x2,  0x1 }, // Y_EVEN_INC
    { 0x0386, 0x2,  0x1 }, // Y_ODD_INC

    { 0x0900, 0x1,  0x0}, // binning_mode
    { 0x0901, 0x1,  0x11}, // binning_type

    {0x0342, 0x2, 0x17C0 }, // LINE_LENGTH_PCK
    {0x0340, 0x2, 0xB9E }, // FRAME_LENGTH_LINES
    {0x0202, 0x2, 0x41A }, // COARSE_INTEGRATION_TIME
    {0x0112, 0x2,  0xA0A}, // CSI_DATA_FORMAT
    {0x0114, 0x1,  0x3}, // CSI_LANE_MODE

#if 1
    {0x0800, 1,  0xD}, // TCLK_POST
    {0x0801, 1,  0x7}, // THS_PREPARE 
    {0x0802, 1,  0xC}, // THS_ZERO_MIN
    {0x0803, 1,  0x9}, //  THS_TRAIL 
    {0x0804, 1,  0xB}, // TCLK_TRAIL_MIN
    {0x0805, 1,  0x6}, // TCLK_PREPARE 
    {0x0806, 1,  0x24}, // TCLK_ZERO 
    {0x0807, 1,  0x7}, // TLPX 
    {0x082A, 1,  0xF}, // TWAKEUP
    {0x082B, 1,  0xC}, // TNINT 
    {0x082C, 1,  0xD}, // THS_EXIT
    {0x3F06, 2,  0xC0}, // MIPI_TIMING_2 C0
    {0x3F0A, 2,  0x2000}, //{0x3F0A, 2,  0xA000}, // MIPI_TIMING_4 A000
    {0x3F0C, 2,  0xA}, // MIPI_TIMING_5 A
    { 0x3F20, 2,  0x8080 }, //  MIPI_PHY_TRIM_MSB
    { 0x3F1E, 2,  0x0004 }, //  MIPI_PHY_TRIM_LSB
#else
    {0x0800, 0x1, 0xB}, // TCLK_POST
    {0x0801, 0x1, 0x5}, // THS_PREPARE
    {0x0802, 0x1, 0x9}, // THS_ZERO_MIN
    {0x0803, 0x1, 0x7}, //  THS_TRAIL
    {0x0804, 0x1, 0x8}, // TCLK_TRAIL_MIN
    {0x0805, 0x1, 0x5}, // TCLK_PREPARE
    {0x0806, 0x1, 0x1B}, // TCLK_ZERO
    {0x0807, 0x1, 0x6}, // TLPX
    {0x082A, 0x1, 0xB}, // TWAKEUP
    {0x082B, 0x1, 0x9}, // TNINT
    {0x082C, 0x1, 0xA}, // THS_EXIT
    {0x3F06, 0x2, 0xC0}, // MIPI_TIMING_2 C0
    {0x3F0A, 0x2, 0x2000}, //{0x3F0A, 0x2, 0xA000}, // MIPI_TIMING_4 A000
    {0x3F0C, 0x2, 0x7}, // MIPI_TIMING_5 7
    {0x3F1E, 0x2, 0x0}, // MIPI_PHY_TRIM[2](en_boost) 0
    {0x3F20, 0x2, 0x8008}, //  MIPI_PHY_TRIM[17]=0
    { 0x3f20, 0x2, 0x0808}, //
#endif
    { 0x4000, 0x2, 0x0114}, //
    { 0x4002, 0x2, 0x1a25}, //
    { 0x4004, 0x2, 0x3dff}, //
    { 0x4006, 0x2, 0xffff}, //
    { 0x4008, 0x2, 0x0a35}, //
    { 0x400a, 0x2, 0x10ef}, //
    { 0x400c, 0x2, 0x3003}, //
    { 0x400e, 0x2, 0x30d8}, //
    { 0x4010, 0x2, 0xf003}, //
    { 0x4012, 0x2, 0xb5f0}, //
    { 0x4014, 0x2, 0x0085}, //
    { 0x4016, 0x2, 0xf004}, //
    { 0x4018, 0x2, 0x9a89}, //
    { 0x401a, 0x2, 0xf000}, //
    { 0x401c, 0x2, 0x9997}, //
    { 0x401e, 0x2, 0xf000}, //
    { 0x4020, 0x2, 0x30c0}, //
    { 0x4022, 0x2, 0xf000}, //
    { 0x4024, 0x2, 0x82f0}, //
    { 0x4026, 0x2, 0x0030}, //
    { 0x4028, 0x2, 0x18f0}, //
    { 0x402a, 0x2, 0x0320}, //
    { 0x402c, 0x2, 0x58f0}, //
    { 0x402e, 0x2, 0x089c}, //
    { 0x4030, 0x2, 0xf010}, //
    { 0x4032, 0x2, 0x99b6}, //
    { 0x4034, 0x2, 0xf003}, //
    { 0x4036, 0x2, 0xb498}, //
    { 0x4038, 0x2, 0xa096}, //
    { 0x403a, 0x2, 0xf000}, //
    { 0x403c, 0x2, 0xa2f0}, //
    { 0x403e, 0x2, 0x00a2}, //
    { 0x4040, 0x2, 0xf008}, //
    { 0x4042, 0x2, 0x9df0}, //
    { 0x4044, 0x2, 0x209d}, //
    { 0x4046, 0x2, 0x8c08}, //
    { 0x4048, 0x2, 0x08f0}, //
    { 0x404a, 0x2, 0x0036}, //
    { 0x404c, 0x2, 0x008f}, //
    { 0x404e, 0x2, 0xf000}, //
    { 0x4050, 0x2, 0x88f0}, //
    { 0x4052, 0x2, 0x0488}, //
    { 0x4054, 0x2, 0xf000}, //
    { 0x4056, 0x2, 0x3600}, //
    { 0x4058, 0x2, 0xf000}, //
    { 0x405a, 0x2, 0x83f0}, //
    { 0x405c, 0x2, 0x0290}, //
    { 0x405e, 0x2, 0xf000}, //
    { 0x4060, 0x2, 0x8bf0}, //
    { 0x4062, 0x2, 0x2ea3}, //
    { 0x4064, 0x2, 0xf000}, //
    { 0x4066, 0x2, 0xa3f0}, //
    { 0x4068, 0x2, 0x089d}, //
    { 0x406a, 0x2, 0xf075}, //
    { 0x406c, 0x2, 0x3003}, //
    { 0x406e, 0x2, 0x4070}, //
    { 0x4070, 0x2, 0x216d}, //
    { 0x4072, 0x2, 0x1cf6}, //
    { 0x4074, 0x2, 0x8b00}, //
    { 0x4076, 0x2, 0x5186}, //
    { 0x4078, 0x2, 0x1300}, //
    { 0x407a, 0x2, 0x0205}, //
    { 0x407c, 0x2, 0x36d8}, //
    { 0x407e, 0x2, 0xf002}, //
    { 0x4080, 0x2, 0x8387}, //
    { 0x4082, 0x2, 0xf006}, //
    { 0x4084, 0x2, 0x8702}, //
    { 0x4086, 0x2, 0x0d02}, //
    { 0x4088, 0x2, 0x05f0}, //
    { 0x408a, 0x2, 0x0383}, //
    { 0x408c, 0x2, 0xf001}, //
    { 0x408e, 0x2, 0x87f0}, //
    { 0x4090, 0x2, 0x0213}, //
    { 0x4092, 0x2, 0x0036}, //
    { 0x4094, 0x2, 0xd887}, //
    { 0x4096, 0x2, 0x020d}, //
    { 0x4098, 0x2, 0xe0e0}, //
    { 0x409a, 0x2, 0xe0e0}, //
    { 0x409c, 0x2, 0xe0e0}, //
    { 0x409e, 0x2, 0xe0e0}, //
    { 0x40a0, 0x2, 0xf000}, //
    { 0x40a2, 0x2, 0x0401}, //
    { 0x40a4, 0x2, 0xf008}, //
    { 0x40a6, 0x2, 0x82f0}, //
    { 0x40a8, 0x2, 0x0883}, //
    { 0x40aa, 0x2, 0xf009}, //
    { 0x40ac, 0x2, 0x85f0}, //
    { 0x40ae, 0x2, 0x2985}, //
    { 0x40b0, 0x2, 0x87f0}, //
    { 0x40b2, 0x2, 0x2a87}, //
    { 0x40b4, 0x2, 0xf63e}, //
    { 0x40b6, 0x2, 0x88f0}, //
    { 0x40b8, 0x2, 0x0801}, //
    { 0x40ba, 0x2, 0x40f0}, //
    { 0x40bc, 0x2, 0x0800}, //
    { 0x40be, 0x2, 0x48f0}, //
    { 0x40c0, 0x2, 0x0882}, //
    { 0x40c2, 0x2, 0xf008}, //
    { 0x40c4, 0x2, 0x0401}, //
    { 0x40c6, 0x2, 0xf008}, //
    { 0x40c8, 0x2, 0xe0e0}, //
    { 0x40ca, 0x2, 0xe0e0}, //
    { 0x40cc, 0x2, 0xe0e0}, //
    { 0x40ce, 0x2, 0xe0e0}, //
    { 0x40d0, 0x2, 0xf000}, //
    { 0x40d2, 0x2, 0x0401}, //
    { 0x40d4, 0x2, 0xf015}, //
    { 0x40d6, 0x2, 0x003c}, //
    { 0x40d8, 0x2, 0xf00e}, //
    { 0x40da, 0x2, 0x85f0}, //
    { 0x40dc, 0x2, 0x0687}, //
    { 0x40de, 0x2, 0xf002}, //
    { 0x40e0, 0x2, 0x87f0}, //
    { 0x40e2, 0x2, 0x61e8}, //
    { 0x40e4, 0x2, 0x3904}, //
    { 0x40e6, 0x2, 0xf005}, //
    { 0x40e8, 0x2, 0x3482}, //
    { 0x40ea, 0x2, 0xf000}, //
    { 0x40ec, 0x2, 0x3241}, //
    { 0x40ee, 0x2, 0xf000}, //
    { 0x40f0, 0x2, 0x3904}, //
    { 0x40f2, 0x2, 0xf00e}, //
    { 0x40f4, 0x2, 0x3904}, //
    { 0x40f6, 0x2, 0xf000}, //
    { 0x40f8, 0x2, 0x3241}, //
    { 0x40fa, 0x2, 0xf000}, //
    { 0x40fc, 0x2, 0x3482}, //
    { 0x40fe, 0x2, 0xf005}, //
    { 0x4100, 0x2, 0xc0e6}, //
    { 0x4102, 0x2, 0xf004}, //
    { 0x4104, 0x2, 0x3904}, //
    { 0x4106, 0x2, 0xf003}, //
    { 0x4108, 0x2, 0xb0f0}, //
    { 0x410a, 0x2, 0x0000}, //
    { 0x410c, 0x2, 0x18f0}, //
    { 0x410e, 0x2, 0x0086}, //
    { 0x4110, 0x2, 0xf000}, //
    { 0x4112, 0x2, 0x86f0}, //
    { 0x4114, 0x2, 0x89b0}, //
    { 0x4116, 0x2, 0xf000}, //
    { 0x4118, 0x2, 0xe9f0}, //
    { 0x411a, 0x2, 0x008a}, //
    { 0x411c, 0x2, 0xf000}, //
    { 0x411e, 0x2, 0x0005}, //
    { 0x4120, 0x2, 0xf000}, //
    { 0x4122, 0x2, 0xe0e0}, //
    { 0x4124, 0x2, 0xe0e0}, //
    { 0x4126, 0x2, 0xe0e0}, //
    { 0x4128, 0x2, 0x0a35}, //
    { 0x412a, 0x2, 0x10ef}, //
    { 0x412c, 0x2, 0x3003}, //
    { 0x412e, 0x2, 0x30d8}, //
    { 0x4130, 0x2, 0xf005}, //
    { 0x4132, 0x2, 0x85f0}, //
    { 0x4134, 0x2, 0x049a}, //
    { 0x4136, 0x2, 0x89f0}, //
    { 0x4138, 0x2, 0x0099}, //
    { 0x413a, 0x2, 0x97f0}, //
    { 0x413c, 0x2, 0x0030}, //
    { 0x413e, 0x2, 0xc0f0}, //
    { 0x4140, 0x2, 0x0082}, //
    { 0x4142, 0x2, 0xf000}, //
    { 0x4144, 0x2, 0x3018}, //
    { 0x4146, 0x2, 0xf002}, //
    { 0x4148, 0x2, 0xb520}, //
    { 0x414a, 0x2, 0x58f0}, //
    { 0x414c, 0x2, 0x089c}, //
    { 0x414e, 0x2, 0xf010}, //
    { 0x4150, 0x2, 0x99b6}, //
    { 0x4152, 0x2, 0xf003}, //
    { 0x4154, 0x2, 0xb498}, //
    { 0x4156, 0x2, 0xa096}, //
    { 0x4158, 0x2, 0xf000}, //
    { 0x415a, 0x2, 0xa2f0}, //
    { 0x415c, 0x2, 0x00a2}, //
    { 0x415e, 0x2, 0xf008}, //
    { 0x4160, 0x2, 0x9df0}, //
    { 0x4162, 0x2, 0x209d}, //
    { 0x4164, 0x2, 0x8c08}, //
    { 0x4166, 0x2, 0x08f0}, //
    { 0x4168, 0x2, 0x0036}, //
    { 0x416a, 0x2, 0x008f}, //
    { 0x416c, 0x2, 0x88f0}, //
    { 0x416e, 0x2, 0x0188}, //
    { 0x4170, 0x2, 0x3600}, //
    { 0x4172, 0x2, 0xf000}, //
    { 0x4174, 0x2, 0x83f0}, //
    { 0x4176, 0x2, 0x0290}, //
    { 0x4178, 0x2, 0xf001}, //
    { 0x417a, 0x2, 0x8bf0}, //
    { 0x417c, 0x2, 0x2da3}, //
    { 0x417e, 0x2, 0xf000}, //
    { 0x4180, 0x2, 0xa3f0}, //
    { 0x4182, 0x2, 0x089d}, //
    { 0x4184, 0x2, 0xf06d}, //
    { 0x4186, 0x2, 0x4070}, //
    { 0x4188, 0x2, 0x3003}, //
    { 0x418a, 0x2, 0x214d}, //
    { 0x418c, 0x2, 0x1ff6}, //
    { 0x418e, 0x2, 0x0851}, //
    { 0x4190, 0x2, 0x0245}, //
    { 0x4192, 0x2, 0x9d36}, //
    { 0x4194, 0x2, 0xd8f0}, //
    { 0x4196, 0x2, 0x0083}, //
    { 0x4198, 0x2, 0xf000}, //
    { 0x419a, 0x2, 0x87f0}, //
    { 0x419c, 0x2, 0x0087}, //
    { 0x419e, 0x2, 0xf000}, //
    { 0x41a0, 0x2, 0x36d8}, //
    { 0x41a2, 0x2, 0x020d}, //
    { 0x41a4, 0x2, 0x0205}, //
    { 0x41a6, 0x2, 0xf000}, //
    { 0x41a8, 0x2, 0x36d8}, //
    { 0x41aa, 0x2, 0xf000}, //
    { 0x41ac, 0x2, 0x83f0}, //
    { 0x41ae, 0x2, 0x0087}, //
    { 0x41b0, 0x2, 0xf000}, //
    { 0x41b2, 0x2, 0x87f0}, //
    { 0x41b4, 0x2, 0x0036}, //
    { 0x41b6, 0x2, 0xd802}, //
    { 0x41b8, 0x2, 0x0d02}, //
    { 0x41ba, 0x2, 0x05f0}, //
    { 0x41bc, 0x2, 0x0036}, //
    { 0x41be, 0x2, 0xd8f0}, //
    { 0x41c0, 0x2, 0x0083}, //
    { 0x41c2, 0x2, 0xf000}, //
    { 0x41c4, 0x2, 0x87f0}, //
    { 0x41c6, 0x2, 0x0087}, //
    { 0x41c8, 0x2, 0xf000}, //
    { 0x41ca, 0x2, 0x36d8}, //
    { 0x41cc, 0x2, 0x020d}, //
    { 0x41ce, 0x2, 0x0205}, //
    { 0x41d0, 0x2, 0xf000}, //
    { 0x41d2, 0x2, 0x36d8}, //
    { 0x41d4, 0x2, 0xf000}, //
    { 0x41d6, 0x2, 0x83f0}, //
    { 0x41d8, 0x2, 0x0087}, //
    { 0x41da, 0x2, 0xf000}, //
    { 0x41dc, 0x2, 0x8713}, //
    { 0x41de, 0x2, 0x0036}, //
    { 0x41e0, 0x2, 0xd802}, //
    { 0x41e2, 0x2, 0x0de0}, //
    { 0x41e4, 0x2, 0xe0e0}, //
    { 0x41e6, 0x2, 0xe0e0}, //
    { 0x41e8, 0x2, 0x9f13}, //
    { 0x41ea, 0x2, 0x0041}, //
    { 0x41ec, 0x2, 0x80f3}, //
    { 0x41ee, 0x2, 0xf213}, //
    { 0x41f0, 0x2, 0x00f0}, //
    { 0x41f2, 0x2, 0x13b8}, //
    { 0x41f4, 0x2, 0xf04c}, //
    { 0x41f6, 0x2, 0x9ff0}, //
    { 0x41f8, 0x2, 0x00b7}, //
    { 0x41fa, 0x2, 0xf006}, //
    { 0x41fc, 0x2, 0x0035}, //
    { 0x41fe, 0x2, 0x10af}, //
    { 0x4200, 0x2, 0x3003}, //
    { 0x4202, 0x2, 0x30c0}, //
    { 0x4204, 0x2, 0xb2f0}, //
    { 0x4206, 0x2, 0x01b5}, //
    { 0x4208, 0x2, 0xf001}, //
    { 0x420a, 0x2, 0x85f0}, //
    { 0x420c, 0x2, 0x0292}, //
    { 0x420e, 0x2, 0xf000}, //
    { 0x4210, 0x2, 0x9a8b}, //
    { 0x4212, 0x2, 0xf000}, //
    { 0x4214, 0x2, 0x9997}, //
    { 0x4216, 0x2, 0xf007}, //
    { 0x4218, 0x2, 0xb6f0}, //
    { 0x421a, 0x2, 0x0020}, //
    { 0x421c, 0x2, 0x5830}, //
    { 0x421e, 0x2, 0xc040}, //
    { 0x4220, 0x2, 0x1282}, //
    { 0x4222, 0x2, 0xf005}, //
    { 0x4224, 0x2, 0x9cf0}, //
    { 0x4226, 0x2, 0x01b2}, //
    { 0x4228, 0x2, 0xf008}, //
    { 0x422a, 0x2, 0xb8f0}, //
    { 0x422c, 0x2, 0x0799}, //
    { 0x422e, 0x2, 0xf005}, //
    { 0x4230, 0x2, 0x98f0}, //
    { 0x4232, 0x2, 0x0296}, //
    { 0x4234, 0x2, 0xa2f0}, //
    { 0x4236, 0x2, 0x00a2}, //
    { 0x4238, 0x2, 0xf008}, //
    { 0x423a, 0x2, 0x9df0}, //
    { 0x423c, 0x2, 0x02a1}, //
    { 0x423e, 0x2, 0xf01f}, //
    { 0x4240, 0x2, 0x1009}, //
    { 0x4242, 0x2, 0x2220}, //
    { 0x4244, 0x2, 0x0808}, //
    { 0x4246, 0x2, 0xf000}, //
    { 0x4248, 0x2, 0x3600}, //
    { 0x424a, 0x2, 0xf000}, //
    { 0x424c, 0x2, 0x88f0}, //
    { 0x424e, 0x2, 0x0788}, //
    { 0x4250, 0x2, 0x3600}, //
    { 0x4252, 0x2, 0xf000}, //
    { 0x4254, 0x2, 0x83f0}, //
    { 0x4256, 0x2, 0x0290}, //
    { 0x4258, 0x2, 0xf016}, //
    { 0x425a, 0x2, 0x8bf0}, //
    { 0x425c, 0x2, 0x11a3}, //
    { 0x425e, 0x2, 0xf000}, //
    { 0x4260, 0x2, 0xa3f0}, //
    { 0x4262, 0x2, 0x089d}, //
    { 0x4264, 0x2, 0xf002}, //
    { 0x4266, 0x2, 0xa1f0}, //
    { 0x4268, 0x2, 0x20a1}, //
    { 0x426a, 0x2, 0xf006}, //
    { 0x426c, 0x2, 0x4300}, //
    { 0x426e, 0x2, 0xf049}, //
    { 0x4270, 0x2, 0x4014}, //
    { 0x4272, 0x2, 0x8b8e}, //
    { 0x4274, 0x2, 0x9df0}, //
    { 0x4276, 0x2, 0x0802}, //
    { 0x4278, 0x2, 0x02f0}, //
    { 0x427a, 0x2, 0x00a6}, //
    { 0x427c, 0x2, 0xf013}, //
    { 0x427e, 0x2, 0xb283}, //
    { 0x4280, 0x2, 0x9c36}, //
    { 0x4282, 0x2, 0x00f0}, //
    { 0x4284, 0x2, 0x0636}, //
    { 0x4286, 0x2, 0x009c}, //
    { 0x4288, 0x2, 0xf008}, //
    { 0x428a, 0x2, 0x8bf0}, //
    { 0x428c, 0x2, 0x0083}, //
    { 0x428e, 0x2, 0xa0f0}, //
    { 0x4290, 0x2, 0x0630}, //
    { 0x4292, 0x2, 0x18f0}, //
    { 0x4294, 0x2, 0x02a3}, //
    { 0x4296, 0x2, 0xf000}, //
    { 0x4298, 0x2, 0xa3f0}, //
    { 0x429a, 0x2, 0x0243}, //
    { 0x429c, 0x2, 0x00f0}, //
    { 0x429e, 0x2, 0x049d}, //
    { 0x42a0, 0x2, 0xf078}, //
    { 0x42a2, 0x2, 0x3018}, //
    { 0x42a4, 0x2, 0xf000}, //
    { 0x42a6, 0x2, 0x9d82}, //
    { 0x42a8, 0x2, 0xf000}, //
    { 0x42aa, 0x2, 0x9030}, //
    { 0x42ac, 0x2, 0xc0f0}, //
    { 0x42ae, 0x2, 0x1130}, //
    { 0x42b0, 0x2, 0xc0f0}, //
    { 0x42b2, 0x2, 0x0082}, //
    { 0x42b4, 0x2, 0xf001}, //
    { 0x42b6, 0x2, 0x1009}, //
    { 0x42b8, 0x2, 0xf02a}, //
    { 0x42ba, 0x2, 0xa2f0}, //
    { 0x42bc, 0x2, 0x00a2}, //
    { 0x42be, 0x2, 0x3018}, //
    { 0x42c0, 0x2, 0xf007}, //
    { 0x42c2, 0x2, 0x9df0}, //
    { 0x42c4, 0x2, 0x1c8c}, //
    { 0x42c6, 0x2, 0xf005}, //
    { 0x42c8, 0x2, 0x301f}, //
    { 0x42ca, 0x2, 0x216d}, //
    { 0x42cc, 0x2, 0x0a51}, //
    { 0x42ce, 0x2, 0x1fea}, //
    { 0x42d0, 0x2, 0x8640}, //
    { 0x42d2, 0x2, 0xe29f}, //
    { 0x42d4, 0x2, 0xf009}, //
    { 0x42d6, 0x2, 0x0005}, //
    { 0x42d8, 0x2, 0xf000}, //
    { 0x42da, 0x2, 0x30c0}, //
    { 0x42dc, 0x2, 0xf001}, //
    { 0x42de, 0x2, 0x83f0}, //
    { 0x42e0, 0x2, 0x0036}, //
    { 0x42e2, 0x2, 0x00f0}, //
    { 0x42e4, 0x2, 0x0087}, //
    { 0x42e6, 0x2, 0xf007}, //
    { 0x42e8, 0x2, 0x87f0}, //
    { 0x42ea, 0x2, 0x0036}, //
    { 0x42ec, 0x2, 0xc0f0}, //
    { 0x42ee, 0x2, 0x0000}, //
    { 0x42f0, 0x2, 0x0df0}, //
    { 0x42f2, 0x2, 0x0000}, //
    { 0x42f4, 0x2, 0x05f0}, //
    { 0x42f6, 0x2, 0x0030}, //
    { 0x42f8, 0x2, 0xc0f0}, //
    { 0x42fa, 0x2, 0x0183}, //
    { 0x42fc, 0x2, 0xf000}, //
    { 0x42fe, 0x2, 0x3600}, //
    { 0x4300, 0x2, 0xf000}, //
    { 0x4302, 0x2, 0x87f0}, //
    { 0x4304, 0x2, 0x0787}, //
    { 0x4306, 0x2, 0xf000}, //
    { 0x4308, 0x2, 0x36c0}, //
    { 0x430a, 0x2, 0xf000}, //
    { 0x430c, 0x2, 0x000f}, //
    { 0x430e, 0x2, 0xf42a}, //
    { 0x4310, 0x2, 0x4180}, //
    { 0x4312, 0x2, 0x1300}, //
    { 0x4314, 0x2, 0x9ff0}, //
    { 0x4316, 0x2, 0x00e0}, //
    { 0x4318, 0x2, 0xe0e0}, //
    { 0x431a, 0x2, 0xe0e0}, //
    { 0x431c, 0x2, 0xe0e0}, //
    { 0x431e, 0x2, 0xe0e0}, //
    { 0x4320, 0x2, 0xe0e0}, //
    { 0x4322, 0x2, 0xe0e0}, //
    { 0x4324, 0x2, 0xe0e0}, //
    { 0x4326, 0x2, 0xe0e0}, //
    { 0x4328, 0x2, 0xe0e0}, //
    { 0x432a, 0x2, 0xe0e0}, //
    { 0x432c, 0x2, 0xe0e0}, //
    { 0x432e, 0x2, 0xe0e0}, //
    { 0x4330, 0x2, 0xe0e0}, //
    { 0x4332, 0x2, 0xe0e0}, //
    { 0x4334, 0x2, 0xe0e0}, //
    { 0x4336, 0x2, 0xe0e0}, //
    { 0x4338, 0x2, 0xe0e0}, //
    { 0x433a, 0x2, 0xe0e0}, //
    { 0x433c, 0x2, 0xe0e0}, //
    { 0x433e, 0x2, 0xe0e0}, //
    { 0x4340, 0x2, 0xe0e0}, //
    { 0x4342, 0x2, 0xe0e0}, //
    { 0x4344, 0x2, 0xe0e0}, //
    { 0x4346, 0x2, 0xe0e0}, //
    { 0x4348, 0x2, 0xe0e0}, //
    { 0x434a, 0x2, 0xe0e0}, //
    { 0x434c, 0x2, 0xe0e0}, //
    { 0x434e, 0x2, 0xe0e0}, //
    { 0x4350, 0x2, 0xe0e0}, //
    { 0x4352, 0x2, 0xe0e0}, //
    { 0x4354, 0x2, 0xe0e0}, //
    { 0x4356, 0x2, 0xe0e0}, //
    { 0x4358, 0x2, 0xe0e0}, //
    { 0x435a, 0x2, 0xe0e0}, //
    { 0x435c, 0x2, 0xe0e0}, //
    { 0x435e, 0x2, 0xe0e0}, //
    { 0x4360, 0x2, 0xe0e0}, //
    { 0x4362, 0x2, 0xe0e0}, //
    { 0x4364, 0x2, 0xe0e0}, //
    { 0x4366, 0x2, 0xe0e0}, //
    { 0x4368, 0x2, 0xe0e0}, //
    { 0x436a, 0x2, 0xe0e0}, //
    { 0x436c, 0x2, 0xe0e0}, //
    { 0x436e, 0x2, 0xe0e0}, //
    { 0x4370, 0x2, 0xe0e0}, //
    { 0x4372, 0x2, 0xe0e0}, //
    { 0x4374, 0x2, 0xe0e0}, //
    { 0x4376, 0x2, 0xe0e0}, //
    { 0x4378, 0x2, 0xe0e0}, //
    { 0x437a, 0x2, 0xe0e0}, //
    { 0x437c, 0x2, 0xe0e0}, //
    { 0x437e, 0x2, 0xe0e0}, //
    { 0x4380, 0x2, 0xe0e0}, //
    { 0x4382, 0x2, 0xe0e0}, //
    { 0x4384, 0x2, 0xe0e0}, //
    { 0x4386, 0x2, 0xe0e0}, //
    { 0x4388, 0x2, 0xe0e0}, //
    { 0x438a, 0x2, 0xe0e0}, //
    { 0x438c, 0x2, 0xe0e0}, //
    { 0x438e, 0x2, 0xe0e0}, //
    { 0x4390, 0x2, 0xe0e0}, //
    { 0x4392, 0x2, 0xe0e0}, //
    { 0x4394, 0x2, 0xe0e0}, //
    { 0x4396, 0x2, 0xe0e0}, //
    { 0x4398, 0x2, 0xe0e0}, //
    { 0x439a, 0x2, 0xe0e0}, //
    { 0x439c, 0x2, 0xe0e0}, //
    { 0x439e, 0x2, 0xe0e0}, //
    { 0x43a0, 0x2, 0xe0e0}, //
    { 0x43a2, 0x2, 0xe0e0}, //
    { 0x43a4, 0x2, 0xe0e0}, //
    { 0x43a6, 0x2, 0xe0e0}, //
    { 0x43a8, 0x2, 0xe0e0}, //
    { 0x43aa, 0x2, 0xe0e0}, //
    { 0x43ac, 0x2, 0xe0e0}, //
    { 0x43ae, 0x2, 0xe0e0}, //
    { 0x43b0, 0x2, 0xe0e0}, //
    { 0x43b2, 0x2, 0xe0e0}, //
    { 0x43b4, 0x2, 0xe0e0}, //
    { 0x43b6, 0x2, 0xe0e0}, //
    { 0x43b8, 0x2, 0xe0e0}, //
    { 0x43ba, 0x2, 0xe0e0}, //
    { 0x43bc, 0x2, 0xe0e0}, //
    { 0x43be, 0x2, 0xe0e0}, //
    { 0x43c0, 0x2, 0xe0e0}, //
    { 0x43c2, 0x2, 0xe0e0}, //
    { 0x43c4, 0x2, 0xe0e0}, //
    { 0x43c6, 0x2, 0xe0e0}, //
    { 0x43c8, 0x2, 0xe0e0}, //
    { 0x43ca, 0x2, 0xe0e0}, //
    { 0x43cc, 0x2, 0xe0e0}, //
    { 0x43ce, 0x2, 0xe0e0}, //
    { 0x43d0, 0x2, 0xe0e0}, //
    { 0x43d2, 0x2, 0xe0e0}, //
    { 0x43d4, 0x2, 0xe0e0}, //
    { 0x43d6, 0x2, 0xe0e0}, //
    { 0x43d8, 0x2, 0xe0e0}, //
    { 0x43da, 0x2, 0xe0e0}, //
    { 0x43dc, 0x2, 0xe0e0}, //
    { 0x43de, 0x2, 0xe0e0}, //
    { 0x43e0, 0x2, 0xe0e0}, //
    { 0x43e2, 0x2, 0xe0e0}, //
    { 0x43e4, 0x2, 0xe0e0}, //
    { 0x43e6, 0x2, 0xe0e0}, //
    { 0x43e8, 0x2, 0xe0e0}, //
    { 0x43ea, 0x2, 0xe0e0}, //
    { 0x43ec, 0x2, 0xe0e0}, //
    { 0x43ee, 0x2, 0xe0e0}, //
    { 0x43f0, 0x2, 0xe0e0}, //
    { 0x43f2, 0x2, 0xe0e0}, //
    { 0x43f4, 0x2, 0xe0e0}, //
    { 0x43f6, 0x2, 0xe0e0}, //
    { 0x43f8, 0x2, 0xe0e0}, //
    { 0x43fa, 0x2, 0xe0e0}, //
    { 0x43fc, 0x2, 0xe0e0}, //
    { 0x43fe, 0x2, 0xe0e0}, //
    { 0x4400, 0x2, 0xe0e0}, //
    { 0x4402, 0x2, 0xe0e0}, //
    { 0x4404, 0x2, 0xe0e0}, //
    { 0x4406, 0x2, 0xe0e0}, //
    { 0x4408, 0x2, 0xe0e0}, //
    { 0x440a, 0x2, 0xe0e0}, //
    { 0x440c, 0x2, 0xe0e0}, //
    { 0x440e, 0x2, 0xe0e0}, //
    { 0x4410, 0x2, 0xe0e0}, //
    { 0x4412, 0x2, 0xe0e0}, //
    { 0x4414, 0x2, 0xe0e0}, //
    { 0x4416, 0x2, 0xe0e0}, //
    { 0x4418, 0x2, 0xe0e0}, //
    { 0x441a, 0x2, 0xe0e0}, //
    { 0x441c, 0x2, 0xe0e0}, //
    { 0x441e, 0x2, 0xe0e0}, //
    { 0x4420, 0x2, 0xe0e0}, //
    { 0x4422, 0x2, 0xe0e0}, //
    { 0x4424, 0x2, 0xe0e0}, //
    { 0x4426, 0x2, 0xe0e0}, //
    { 0x4428, 0x2, 0xe0e0}, //
    { 0x442a, 0x2, 0xe0e0}, //
    { 0x442c, 0x2, 0xe0e0}, //
    { 0x442e, 0x2, 0xe0e0}, //
    { 0x4430, 0x2, 0xe0e0}, //
    { 0x4432, 0x2, 0xe0e0}, //
    { 0x4434, 0x2, 0xe0e0}, //
    { 0x4436, 0x2, 0xe0e0}, //
    { 0x4438, 0x2, 0xe0e0}, //
    { 0x443a, 0x2, 0xe0e0}, //
    { 0x443c, 0x2, 0xe0e0}, //
    { 0x443e, 0x2, 0xe0e0}, //
    { 0x4440, 0x2, 0xe0e0}, //
    { 0x4442, 0x2, 0xe0e0}, //
    { 0x4444, 0x2, 0xe0e0}, //
    { 0x4446, 0x2, 0xe0e0}, //
    { 0x4448, 0x2, 0xe0e0}, //
    { 0x444a, 0x2, 0xe0e0}, //
    { 0x444c, 0x2, 0xe0e0}, //
    { 0x444e, 0x2, 0xe0e0}, //
    { 0x4450, 0x2, 0xe0e0}, //
    { 0x4452, 0x2, 0xe0e0}, //
    { 0x4454, 0x2, 0xe0e0}, //
    { 0x4456, 0x2, 0xe0e0}, //
    { 0x4458, 0x2, 0xe0e0}, //
    { 0x445a, 0x2, 0xe0e0}, //
    { 0x445c, 0x2, 0xe0e0}, //
    { 0x445e, 0x2, 0xe0e0}, //
    { 0x4460, 0x2, 0xe0e0}, //
    { 0x4462, 0x2, 0xe0e0}, //
    { 0x4464, 0x2, 0xe0e0}, //
    { 0x4466, 0x2, 0xe0e0}, //
    { 0x4468, 0x2, 0xe0e0}, //
    { 0x446a, 0x2, 0xe0e0}, //
    { 0x446c, 0x2, 0xe0e0}, //
    { 0x446e, 0x2, 0xe0e0}, //
    { 0x4470, 0x2, 0xe0e0}, //
    { 0x4472, 0x2, 0xe0e0}, //
    { 0x4474, 0x2, 0xe0e0}, //
    { 0x4476, 0x2, 0xe0e0}, //
    { 0x4478, 0x2, 0xe0e0}, //
    { 0x447a, 0x2, 0xe0e0}, //
    { 0x447c, 0x2, 0xe0e0}, //
    { 0x447e, 0x2, 0xe0e0}, //
    { 0x4480, 0x2, 0xe0e0}, //
    { 0x4482, 0x2, 0xe0e0}, //
    { 0x4484, 0x2, 0xe0e0}, //
    { 0x4486, 0x2, 0xe0e0}, //
    { 0x4488, 0x2, 0xe0e0}, //
    { 0x448a, 0x2, 0xe0e0}, //
    { 0x448c, 0x2, 0xe0e0}, //
    { 0x448e, 0x2, 0xe0e0}, //
    { 0x4490, 0x2, 0xe0e0}, //
    { 0x4492, 0x2, 0xe0e0}, //
    { 0x4494, 0x2, 0xe0e0}, //
    { 0x4496, 0x2, 0xe0e0}, //
    { 0x4498, 0x2, 0xe0e0}, //
    { 0x449a, 0x2, 0xe0e0}, //
    { 0x449c, 0x2, 0xe0e0}, //
    { 0x449e, 0x2, 0xe0e0}, //
    { 0x44a0, 0x2, 0xe0e0}, //
    { 0x44a2, 0x2, 0xe0e0}, //
    { 0x44a4, 0x2, 0xe0e0}, //
    { 0x44a6, 0x2, 0xe0e0}, //
    { 0x44a8, 0x2, 0xe0e0}, //
    { 0x44aa, 0x2, 0xe0e0}, //
    { 0x44ac, 0x2, 0xe0e0}, //
    { 0x44ae, 0x2, 0xe0e0}, //
    { 0x44b0, 0x2, 0xe0e0}, //
    { 0x44b2, 0x2, 0xe0e0}, //
    { 0x44b4, 0x2, 0xe0e0}, //
    { 0x5500, 0x2, 0x0000}, //
    { 0x5502, 0x2, 0x0002}, //
    { 0x5504, 0x2, 0x0006}, //
    { 0x5506, 0x2, 0x0009}, //
    { 0x5508, 0x2, 0x000f}, //
    { 0x550a, 0x2, 0x0010}, //
    { 0x550c, 0x2, 0x0011}, //
    { 0x550e, 0x2, 0x0012}, //
    { 0x5510, 0x2, 0x0019}, //
    { 0x5512, 0x2, 0x0020}, //
    { 0x5514, 0x2, 0x0021}, //
    { 0x5516, 0x2, 0x0023}, //
    { 0x5518, 0x2, 0x0026}, //
    { 0x551a, 0x2, 0x002b}, //
    { 0x551c, 0x2, 0x002f}, //
    { 0x551e, 0x2, 0x0030}, //
    { 0x5400, 0x2, 0x0100}, //
    { 0x5402, 0x2, 0x2106}, //
    { 0x5404, 0x2, 0x1101}, //
    { 0x5406, 0x2, 0x3106}, //
    { 0x5408, 0x2, 0x7100}, //
    { 0x540a, 0x2, 0x8107}, //
    { 0x540c, 0x2, 0xb101}, //
    { 0x540e, 0x2, 0xd101}, //
    { 0x5410, 0x2, 0xf12e}, //
    { 0x5412, 0x2, 0xf112}, //
    { 0x5414, 0x2, 0xf184}, //
    { 0x5416, 0x2, 0xf224}, //
    { 0x5418, 0x2, 0xf306}, //
    { 0x541a, 0x2, 0xf446}, //
    { 0x541c, 0x2, 0xf609}, //
    { 0x541e, 0x2, 0xf887}, //
    { 0x5420, 0x2, 0xfc0b}, //
    { 0x5422, 0x2, 0xfc0b}, //
    { 0x5424, 0x2, 0xfffa}, //
    { 0x5426, 0x2, 0x5557}, //
    { 0x5428, 0x2, 0x0005}, //
    { 0x542a, 0x2, 0xa550}, //
    { 0x542c, 0x2, 0xaaaa}, //
    { 0x542e, 0x2, 0x000a}, //
    { 0x5460, 0x2, 0x2269}, //
    { 0x5462, 0x2, 0x0b87}, //
    { 0x5464, 0x2, 0x0b87}, //
    { 0x5466, 0x2, 0x0983}, //
    { 0x5498, 0x2, 0x225e}, //
    { 0x549a, 0x2, 0xbcaa}, //
    { 0x549c, 0x2, 0xbcaa}, //
    { 0x549e, 0x2, 0xbdaa}, //
    { 0x3060, 0x2, 0xff01}, //
    { 0x44ba, 0x2, 0x0050}, //
    { 0x44bc, 0x2, 0xbcaa}, //
    { 0x44c0, 0x2, 0x4070}, //
    { 0x44c4, 0x2, 0x04d0}, //
    { 0x44c6, 0x2, 0x17e2}, //
    { 0x44c8, 0x2, 0xea43}, //
    { 0x44ca, 0x2, 0x000e}, //
    { 0x44cc, 0x2, 0x7777}, //
    { 0x44ce, 0x2, 0x8ba4}, //
    { 0x44d0, 0x2, 0x1735}, //
    { 0x44d2, 0x2, 0x0b87}, //
    { 0x44d4, 0x2, 0x8000}, //
    { 0x44d6, 0x2, 0xf206}, //
    { 0x44d8, 0x2, 0xaafa}, //
    { 0x44da, 0x2, 0xe001}, //
    { 0x44de, 0x2, 0x9bbc}, //
    { 0x44e0, 0x2, 0x283c}, //
    { 0x44e2, 0x2, 0x2821}, //
    { 0x44e4, 0x2, 0x8000}, //
    { 0x44e6, 0x2, 0x503f}, //
    { 0x32a4, 0x2, 0x0000}, //
    { 0x333c, 0x2, 0x0001}, //
    { 0x301a, 0x2, 0x0000}, //
    { 0x3600, 0x2, 0x94df}, //
    { 0x3700, 0x2, 0x0001}, //
    { 0x3980, 0x2, 0x0003}, //
    { 0x36c0, 0x2, 0x0001}, //
    { 0x36de, 0x2, 0x002a}, //
    { 0x301a, 0x2, 0x0008}, //
    { 0x3060, 0x2, 0x0000}, //
    { 0x3060, 0x2, 0xff01}, //
    { 0x3980, 0x2, 0x0001}, //
    { 0x3340, 0x2, 0x0c00}, //
    { 0x3340, 0x2, 0x1c00}, //
    { 0x36c0, 0x2, 0x0001}, //
    { 0x400, 0x2,  0x3 }, //  X
    { 0x404, 0x2,  0xFFA }, //  Scale_M
    { 0x0406, 0x2,  0x400 }, //  Scale_N
    { 0x3C58, 0x2,  0x8 }, //  Slice overlap
    { 0x3C40, 0x2,  0x2 }, //  Scaling Mode
    { 0x3C42, 0x2,  0xDBA }, //  SCALE_SECOND_RESIDUAL_EVEN=3514
    { 0x3C44, 0x2,  0x3BD }, //  SCALE_SECOND_RESIDUAL_ODD=957
    { 0x3C46, 0x2,  0x405 }, //  scale_second_crop_e=4scale_second_crop_o=5
    { 0x3C48, 0x2,  0xB62 }, //  SCALE_THIRD_RESIDUAL_EVEN=2914
    { 0x3C4A, 0x2,  0x165 }, //  SCALE_THIRD_RESIDUAL_ODD=357
    { 0x3C4C, 0x2,  0x405 }, //  scale_third_crop_e=4scale_third_crop_o=5
    { 0x3C4E, 0x2,  0x90A }, //  SCALE_FOURTH_RESIDUAL_EVEN=2314
    { 0x3C50, 0x2,  0xF07 }, //  SCALE_FOURTH_RESIDUAL_ODD=3847
    { 0x3C52, 0x2,  0x808 }, //  scale_fourth_crop_e=8scale_fourth_crop_o=8
    { 0x3C60, 0x2,  0xC8 }, //  SCALE_FIRST_PXL_NUM=200
    { 0x3C62, 0x2,  0xC8 }, //  SCALE_SECOND_PXL_NUM=200
    { 0x3C64, 0x2,  0xD0 }, //  SCALE_THIRD_PXL_NUM=208
    { 0x3C66, 0x2,  0xC0 }, //  SCALE_FOURTH_PXL_NUM=192
    { 0x3EC8, 0x2,  0x0 }, //  X_OUTPU_OFFSET:0
    { 0x3ECA, 0x2,  0x0 }, //  Y_OUTPU_OFFSET:0
    { 0x034C, 0x2,  0x320 }, //  X_OUTPU_SIZE:800
    { 0x034E, 0x2,  0x2E0 }, //  Y_OUTPU_SIZE:736
    { 0x3C6A, 0x2,  0x0 }, //  X_OUTPU_OFFSET2:0
    { 0x3C70, 0x2,  0x0 }, //  Y_OUTPU_OFFSET2:0
    { 0x3C6C, 0x2,  0x2A0 }, //  X_OUTPU_SIZE2:672
    { 0x3C72, 0x2,  0x260 }, //  Y_OUTPU_SIZE2:608
    { 0x3C74, 0x2,  0x3 }, //  Sync_regen
    { 0x3C76, 0x2,  0x20 }, //  Sync_regen_blank
    { 0x3C7A, 0x2,  0x0 }, //  reorder_ctrl
    { 0x3F18, 0x2, 0x7B70 }, //  MIPI_JPEG_PN9_DATA_TYPE
    { 0x3F1A, 0x2, 0x102B }, //  T2 data type = RAW 10

    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE

    { 0x0100, 0x1, 0x01  }, // MODE_SELECT
    { 0x44D6, 0x2, 0xB206 }, // dcg_vaa_en=0
};

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_832x608_832x608_table[] =
{
    { 0x0103, 0x3201,  0x01 }, //  SOFTWARE_RESET

    { 0x0304, 2,   0x2 }, // PRE_PLL_CLK_DIV1=2
    { 0x0306, 2,   0x56 }, // PLL_MULTIPLIER1=86
    { 0x0300, 2,  0x6 }, // VT_PIX_CLK_DIV=6
    { 0x0302, 2,  0x1 }, // VT_SYS_CLK_DIV=1
    { 0x030C, 2,   0x7 }, // PRE_PLL_CLK_DIV2=7
    { 0x030E, 2,   0x247 }, // PLL_MULTIPLIER2=350
    { 0x0308, 2,  0x5 }, // OP_PIX_CLK_DIV=5
    { 0x030A, 2,  0x2 }, // OP_SYS_CLK_DIV=2
    {0x0344, 2,  0x378 }, // X_ADDR_START 888
    {0x0348, 2,  0x1097 }, // X_ADDR_END 4247
    {0x0346, 2,  0x2A8 }, // Y_ADDR_START 680
    {0x034A, 2,  0xC67 }, // Y_ADDR_END 3175
    {0x034C, 2,  0xD20 }, // X_OUTPUT_SIZE 3360
    {0x034E, 2,  0x9C0 }, // Y_OUTPUT_SIZE 2496
    { 0x0380, 2,   0x1 }, // X_EVEN_INC
    { 0x0382, 2,   0x1 }, // X_ODD_INC
    { 0x0384, 2,   0x1 }, // Y_EVEN_INC
    { 0x0386, 2,   0x1 }, // Y_ODD_INC

    { 0x0900, 1,   0x0}, // binning_mode
    { 0x0901, 1,   0x11}, // binning_type

    {0x0342, 2,  0x1780 }, // LINE_LENGTH_PCK
    {0x0340, 2,  0x9CE }, // FRAME_LENGTH_LINES
    {0x0202, 2,  0x900 }, // COARSE_INTEGRATION_TIME
    { 0x0112, 2,   0xA0A}, // CSI_DATA_FORMAT
    { 0x0114, 1,   0x3}, // CSI_LANE_MODE

    {0x0800, 1,  0xD}, // TCLK_POST
    {0x0801, 1,  0x7}, // THS_PREPARE 
    {0x0802, 1,  0xC}, // THS_ZERO_MIN
    {0x0803, 1,  0x9}, //  THS_TRAIL 
    {0x0804, 1,  0xB}, // TCLK_TRAIL_MIN
    {0x0805, 1,  0x6}, // TCLK_PREPARE 
    {0x0806, 1,  0x24}, // TCLK_ZERO 
    {0x0807, 1,  0x7}, // TLPX 
    {0x082A, 1,  0xF}, // TWAKEUP
    {0x082B, 1,  0xC}, // TNINT 
    {0x082C, 1,  0xD}, // THS_EXIT
    {0x3F06, 2,  0xC0}, // MIPI_TIMING_2 C0
    {0x3F0A, 2,  0x2000}, //{0x3F0A, 2,  0xA000}, // MIPI_TIMING_4 A000
    {0x3F0C, 2,  0xA}, // MIPI_TIMING_5 A
    { 0x3F20, 2,  0x8080 }, //  MIPI_PHY_TRIM_MSB
    { 0x3F1E, 2,  0x0004 }, //  MIPI_PHY_TRIM_LSB

    { 0x4000, 2,  0x0114 }, //  DYNAMIC_SEQRAM_00
    { 0x4002, 2,  0x1A25 }, //  DYNAMIC_SEQRAM_02
    { 0x4004, 2,  0x3DFF }, //  DYNAMIC_SEQRAM_04
    { 0x4006, 2,  0xFFFF }, //  DYNAMIC_SEQRAM_06
    { 0x4008, 2,  0x0A35 }, //  DYNAMIC_SEQRAM_08
    { 0x400A, 2,  0x10EF }, //  DYNAMIC_SEQRAM_0A
    { 0x400C, 2,  0x3003 }, //  DYNAMIC_SEQRAM_0C
    { 0x400E, 2,  0x30D8 }, //  DYNAMIC_SEQRAM_0E
    { 0x4010, 2,  0xF003 }, //  DYNAMIC_SEQRAM_10
    { 0x4012, 2,  0xB5F0 }, //  DYNAMIC_SEQRAM_12
    { 0x4014, 2,  0x0085 }, //  DYNAMIC_SEQRAM_14
    { 0x4016, 2,  0xF004 }, //  DYNAMIC_SEQRAM_16
    { 0x4018, 2,  0x9A89 }, //  DYNAMIC_SEQRAM_18
    { 0x401A, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1A
    { 0x401C, 2,  0x9997 }, //  DYNAMIC_SEQRAM_1C
    { 0x401E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1E
    { 0x4020, 2,  0x30C0 }, //  DYNAMIC_SEQRAM_20
    { 0x4022, 2,  0xF000 }, //  DYNAMIC_SEQRAM_22
    { 0x4024, 2,  0x82F0 }, //  DYNAMIC_SEQRAM_24
    { 0x4026, 2,  0x0030 }, //  DYNAMIC_SEQRAM_26
    { 0x4028, 2,  0x18F0 }, //  DYNAMIC_SEQRAM_28
    { 0x402A, 2,  0x0320 }, //  DYNAMIC_SEQRAM_2A
    { 0x402C, 2,  0x58F0 }, //  DYNAMIC_SEQRAM_2C
    { 0x402E, 2,  0x089C }, //  DYNAMIC_SEQRAM_2E
    { 0x4030, 2,  0xF010 }, //  DYNAMIC_SEQRAM_30
    { 0x4032, 2,  0x99B6 }, //  DYNAMIC_SEQRAM_32
    { 0x4034, 2,  0xF003 }, //  DYNAMIC_SEQRAM_34
    { 0x4036, 2,  0xB498 }, //  DYNAMIC_SEQRAM_36
    { 0x4038, 2,  0xA096 }, //  DYNAMIC_SEQRAM_38
    { 0x403A, 2,  0xF000 }, //  DYNAMIC_SEQRAM_3A
    { 0x403C, 2,  0xA2F0 }, //  DYNAMIC_SEQRAM_3C
    { 0x403E, 2,  0x00A2 }, //  DYNAMIC_SEQRAM_3E
    { 0x4040, 2,  0xF008 }, //  DYNAMIC_SEQRAM_40
    { 0x4042, 2,  0x9DF0 }, //  DYNAMIC_SEQRAM_42
    { 0x4044, 2,  0x209D }, //  DYNAMIC_SEQRAM_44
    { 0x4046, 2,  0x8C08 }, //  DYNAMIC_SEQRAM_46
    { 0x4048, 2,  0x08F0 }, //  DYNAMIC_SEQRAM_48
    { 0x404A, 2,  0x0036 }, //  DYNAMIC_SEQRAM_4A
    { 0x404C, 2,  0x008F }, //  DYNAMIC_SEQRAM_4C
    { 0x404E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_4E
    { 0x4050, 2,  0x88F0 }, //  DYNAMIC_SEQRAM_50
    { 0x4052, 2,  0x0488 }, //  DYNAMIC_SEQRAM_52
    { 0x4054, 2,  0xF000 }, //  DYNAMIC_SEQRAM_54
    { 0x4056, 2,  0x3600 }, //  DYNAMIC_SEQRAM_56
    { 0x4058, 2,  0xF000 }, //  DYNAMIC_SEQRAM_58
    { 0x405A, 2,  0x83F0 }, //  DYNAMIC_SEQRAM_5A
    { 0x405C, 2,  0x0290 }, //  DYNAMIC_SEQRAM_5C
    { 0x405E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_5E
    { 0x4060, 2,  0x8BF0 }, //  DYNAMIC_SEQRAM_60
    { 0x4062, 2,  0x2EA3 }, //  DYNAMIC_SEQRAM_62
    { 0x4064, 2,  0xF000 }, //  DYNAMIC_SEQRAM_64
    { 0x4066, 2,  0xA3F0 }, //  DYNAMIC_SEQRAM_66
    { 0x4068, 2,  0x089D }, //  DYNAMIC_SEQRAM_68
    { 0x406A, 2,  0xF075 }, //  DYNAMIC_SEQRAM_6A
    { 0x406C, 2,  0x3003 }, //  DYNAMIC_SEQRAM_6C
    { 0x406E, 2,  0x4070 }, //  DYNAMIC_SEQRAM_6E
    { 0x4070, 2,  0x216D }, //  DYNAMIC_SEQRAM_70
    { 0x4072, 2,  0x1CF6 }, //  DYNAMIC_SEQRAM_72
    { 0x4074, 2,  0x8B00 }, //  DYNAMIC_SEQRAM_74
    { 0x4076, 2,  0x5186 }, //  DYNAMIC_SEQRAM_76
    { 0x4078, 2,  0x1300 }, //  DYNAMIC_SEQRAM_78
    { 0x407A, 2,  0x0205 }, //  DYNAMIC_SEQRAM_7A
    { 0x407C, 2,  0x36D8 }, //  DYNAMIC_SEQRAM_7C
    { 0x407E, 2,  0xF002 }, //  DYNAMIC_SEQRAM_7E
    { 0x4080, 2,  0x8387 }, //  DYNAMIC_SEQRAM_80
    { 0x4082, 2,  0xF006 }, //  DYNAMIC_SEQRAM_82
    { 0x4084, 2,  0x8702 }, //  DYNAMIC_SEQRAM_84
    { 0x4086, 2,  0x0D02 }, //  DYNAMIC_SEQRAM_86
    { 0x4088, 2,  0x05F0 }, //  DYNAMIC_SEQRAM_88
    { 0x408A, 2,  0x0383 }, //  DYNAMIC_SEQRAM_8A
    { 0x408C, 2,  0xF001 }, //  DYNAMIC_SEQRAM_8C
    { 0x408E, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_8E
    { 0x4090, 2,  0x0213 }, //  DYNAMIC_SEQRAM_90
    { 0x4092, 2,  0x0036 }, //  DYNAMIC_SEQRAM_92
    { 0x4094, 2,  0xD887 }, //  DYNAMIC_SEQRAM_94
    { 0x4096, 2,  0x020D }, //  DYNAMIC_SEQRAM_96
    { 0x4098, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_98
    { 0x409A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_9A
    { 0x409C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_9C
    { 0x409E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_9E
    { 0x40A0, 2,  0xF000 }, //  DYNAMIC_SEQRAM_A0
    { 0x40A2, 2,  0x0401 }, //  DYNAMIC_SEQRAM_A2
    { 0x40A4, 2,  0xF008 }, //  DYNAMIC_SEQRAM_A4
    { 0x40A6, 2,  0x82F0 }, //  DYNAMIC_SEQRAM_A6
    { 0x40A8, 2,  0x0883 }, //  DYNAMIC_SEQRAM_A8
    { 0x40AA, 2,  0xF009 }, //  DYNAMIC_SEQRAM_AA
    { 0x40AC, 2,  0x85F0 }, //  DYNAMIC_SEQRAM_AC
    { 0x40AE, 2,  0x2985 }, //  DYNAMIC_SEQRAM_AE
    { 0x40B0, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_B0
    { 0x40B2, 2,  0x2A87 }, //  DYNAMIC_SEQRAM_B2
    { 0x40B4, 2,  0xF63E }, //  DYNAMIC_SEQRAM_B4
    { 0x40B6, 2,  0x88F0 }, //  DYNAMIC_SEQRAM_B6
    { 0x40B8, 2,  0x0801 }, //  DYNAMIC_SEQRAM_B8
    { 0x40BA, 2,  0x40F0 }, //  DYNAMIC_SEQRAM_BA
    { 0x40BC, 2,  0x0800 }, //  DYNAMIC_SEQRAM_BC
    { 0x40BE, 2,  0x48F0 }, //  DYNAMIC_SEQRAM_BE
    { 0x40C0, 2,  0x0882 }, //  DYNAMIC_SEQRAM_C0
    { 0x40C2, 2,  0xF008 }, //  DYNAMIC_SEQRAM_C2
    { 0x40C4, 2,  0x0401 }, //  DYNAMIC_SEQRAM_C4
    { 0x40C6, 2,  0xF008 }, //  DYNAMIC_SEQRAM_C6
    { 0x40C8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_C8
    { 0x40CA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_CA
    { 0x40CC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_CC
    { 0x40CE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_CE
    { 0x40D0, 2,  0xF000 }, //  DYNAMIC_SEQRAM_D0
    { 0x40D2, 2,  0x0401 }, //  DYNAMIC_SEQRAM_D2
    { 0x40D4, 2,  0xF015 }, //  DYNAMIC_SEQRAM_D4
    { 0x40D6, 2,  0x002C }, //  DYNAMIC_SEQRAM_D6
    { 0x40D8, 2,  0xF00E }, //  DYNAMIC_SEQRAM_D8
    { 0x40DA, 2,  0x85F0 }, //  DYNAMIC_SEQRAM_DA
    { 0x40DC, 2,  0x0687 }, //  DYNAMIC_SEQRAM_DC
    { 0x40DE, 2,  0xF002 }, //  DYNAMIC_SEQRAM_DE
    { 0x40E0, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_E0
    { 0x40E2, 2,  0x61E8 }, //  DYNAMIC_SEQRAM_E2
    { 0x40E4, 2,  0x3900 }, //  DYNAMIC_SEQRAM_E4
    { 0x40E6, 2,  0xF005 }, //  DYNAMIC_SEQRAM_E6
    { 0x40E8, 2,  0x3480 }, //  DYNAMIC_SEQRAM_E8
    { 0x40EA, 2,  0xF000 }, //  DYNAMIC_SEQRAM_EA
    { 0x40EC, 2,  0x3240 }, //  DYNAMIC_SEQRAM_EC
    { 0x40EE, 2,  0xF000 }, //  DYNAMIC_SEQRAM_EE
    { 0x40F0, 2,  0x3900 }, //  DYNAMIC_SEQRAM_F0
    { 0x40F2, 2,  0xF00E }, //  DYNAMIC_SEQRAM_F2
    { 0x40F4, 2,  0x3900 }, //  DYNAMIC_SEQRAM_F4
    { 0x40F6, 2,  0xF000 }, //  DYNAMIC_SEQRAM_F6
    { 0x40F8, 2,  0x3240 }, //  DYNAMIC_SEQRAM_F8
    { 0x40FA, 2,  0xF000 }, //  DYNAMIC_SEQRAM_FA
    { 0x40FC, 2,  0x3480 }, //  DYNAMIC_SEQRAM_FC
    { 0x40FE, 2,  0xF005 }, //  DYNAMIC_SEQRAM_FE
    { 0x4100, 2,  0xC0E6 }, //  DYNAMIC_SEQRAM_100
    { 0x4102, 2,  0xF004 }, //  DYNAMIC_SEQRAM_102
    { 0x4104, 2,  0x3900 }, //  DYNAMIC_SEQRAM_104
    { 0x4106, 2,  0xF003 }, //  DYNAMIC_SEQRAM_106
    { 0x4108, 2,  0xB0F0 }, //  DYNAMIC_SEQRAM_108
    { 0x410A, 2,  0x0083 }, //  DYNAMIC_SEQRAM_10A
    { 0x410C, 2,  0xF000 }, //  DYNAMIC_SEQRAM_10C
    { 0x410E, 2,  0x86F0 }, //  DYNAMIC_SEQRAM_10E
    { 0x4110, 2,  0x0086 }, //  DYNAMIC_SEQRAM_110
    { 0x4112, 2,  0xF089 }, //  DYNAMIC_SEQRAM_112
    { 0x4114, 2,  0xB0F0 }, //  DYNAMIC_SEQRAM_114
    { 0x4116, 2,  0x00E9 }, //  DYNAMIC_SEQRAM_116
    { 0x4118, 2,  0xF000 }, //  DYNAMIC_SEQRAM_118
    { 0x411A, 2,  0x8AF0 }, //  DYNAMIC_SEQRAM_11A
    { 0x411C, 2,  0x0000 }, //  DYNAMIC_SEQRAM_11C
    { 0x411E, 2,  0x05F0 }, //  DYNAMIC_SEQRAM_11E
    { 0x4120, 2,  0x00E0 }, //  DYNAMIC_SEQRAM_120
    { 0x4122, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_122
    { 0x4124, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_124
    { 0x4126, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_126
    { 0x4128, 2,  0x0A35 }, //  DYNAMIC_SEQRAM_128
    { 0x412A, 2,  0x10EF }, //  DYNAMIC_SEQRAM_12A
    { 0x412C, 2,  0x3003 }, //  DYNAMIC_SEQRAM_12C
    { 0x412E, 2,  0x30D8 }, //  DYNAMIC_SEQRAM_12E
    { 0x4130, 2,  0xF005 }, //  DYNAMIC_SEQRAM_130
    { 0x4132, 2,  0x85F0 }, //  DYNAMIC_SEQRAM_132
    { 0x4134, 2,  0x049A }, //  DYNAMIC_SEQRAM_134
    { 0x4136, 2,  0x89F0 }, //  DYNAMIC_SEQRAM_136
    { 0x4138, 2,  0x0099 }, //  DYNAMIC_SEQRAM_138
    { 0x413A, 2,  0x97F0 }, //  DYNAMIC_SEQRAM_13A
    { 0x413C, 2,  0x0030 }, //  DYNAMIC_SEQRAM_13C
    { 0x413E, 2,  0xC0F0 }, //  DYNAMIC_SEQRAM_13E
    { 0x4140, 2,  0x0082 }, //  DYNAMIC_SEQRAM_140
    { 0x4142, 2,  0xF000 }, //  DYNAMIC_SEQRAM_142
    { 0x4144, 2,  0x3018 }, //  DYNAMIC_SEQRAM_144
    { 0x4146, 2,  0xF002 }, //  DYNAMIC_SEQRAM_146
    { 0x4148, 2,  0xB520 }, //  DYNAMIC_SEQRAM_148
    { 0x414A, 2,  0x58F0 }, //  DYNAMIC_SEQRAM_14A
    { 0x414C, 2,  0x089C }, //  DYNAMIC_SEQRAM_14C
    { 0x414E, 2,  0xF010 }, //  DYNAMIC_SEQRAM_14E
    { 0x4150, 2,  0x99B6 }, //  DYNAMIC_SEQRAM_150
    { 0x4152, 2,  0xF003 }, //  DYNAMIC_SEQRAM_152
    { 0x4154, 2,  0xB498 }, //  DYNAMIC_SEQRAM_154
    { 0x4156, 2,  0xA096 }, //  DYNAMIC_SEQRAM_156
    { 0x4158, 2,  0xF000 }, //  DYNAMIC_SEQRAM_158
    { 0x415A, 2,  0xA2F0 }, //  DYNAMIC_SEQRAM_15A
    { 0x415C, 2,  0x00A2 }, //  DYNAMIC_SEQRAM_15C
    { 0x415E, 2,  0xF008 }, //  DYNAMIC_SEQRAM_15E
    { 0x4160, 2,  0x9DF0 }, //  DYNAMIC_SEQRAM_160
    { 0x4162, 2,  0x209D }, //  DYNAMIC_SEQRAM_162
    { 0x4164, 2,  0x8C08 }, //  DYNAMIC_SEQRAM_164
    { 0x4166, 2,  0x08F0 }, //  DYNAMIC_SEQRAM_166
    { 0x4168, 2,  0x0036 }, //  DYNAMIC_SEQRAM_168
    { 0x416A, 2,  0x008F }, //  DYNAMIC_SEQRAM_16A
    { 0x416C, 2,  0x88F0 }, //  DYNAMIC_SEQRAM_16C
    { 0x416E, 2,  0x0188 }, //  DYNAMIC_SEQRAM_16E
    { 0x4170, 2,  0x3600 }, //  DYNAMIC_SEQRAM_170
    { 0x4172, 2,  0xF000 }, //  DYNAMIC_SEQRAM_172
    { 0x4174, 2,  0x83F0 }, //  DYNAMIC_SEQRAM_174
    { 0x4176, 2,  0x0290 }, //  DYNAMIC_SEQRAM_176
    { 0x4178, 2,  0xF001 }, //  DYNAMIC_SEQRAM_178
    { 0x417A, 2,  0x8BF0 }, //  DYNAMIC_SEQRAM_17A
    { 0x417C, 2,  0x2DA3 }, //  DYNAMIC_SEQRAM_17C
    { 0x417E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_17E
    { 0x4180, 2,  0xA3F0 }, //  DYNAMIC_SEQRAM_180
    { 0x4182, 2,  0x089D }, //  DYNAMIC_SEQRAM_182
    { 0x4184, 2,  0xF06D }, //  DYNAMIC_SEQRAM_184
    { 0x4186, 2,  0x4070 }, //  DYNAMIC_SEQRAM_186
    { 0x4188, 2,  0x3003 }, //  DYNAMIC_SEQRAM_188
    { 0x418A, 2,  0x214D }, //  DYNAMIC_SEQRAM_18A
    { 0x418C, 2,  0x1FF6 }, //  DYNAMIC_SEQRAM_18C
    { 0x418E, 2,  0x0851 }, //  DYNAMIC_SEQRAM_18E
    { 0x4190, 2,  0x0245 }, //  DYNAMIC_SEQRAM_190
    { 0x4192, 2,  0x9D36 }, //  DYNAMIC_SEQRAM_192
    { 0x4194, 2,  0xD8F0 }, //  DYNAMIC_SEQRAM_194
    { 0x4196, 2,  0x0083 }, //  DYNAMIC_SEQRAM_196
    { 0x4198, 2,  0xF000 }, //  DYNAMIC_SEQRAM_198
    { 0x419A, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_19A
    { 0x419C, 2,  0x0087 }, //  DYNAMIC_SEQRAM_19C
    { 0x419E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_19E
    { 0x41A0, 2,  0x36D8 }, //  DYNAMIC_SEQRAM_1A0
    { 0x41A2, 2,  0x020D }, //  DYNAMIC_SEQRAM_1A2
    { 0x41A4, 2,  0x0205 }, //  DYNAMIC_SEQRAM_1A4
    { 0x41A6, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1A6
    { 0x41A8, 2,  0x36D8 }, //  DYNAMIC_SEQRAM_1A8
    { 0x41AA, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1AA
    { 0x41AC, 2,  0x83F0 }, //  DYNAMIC_SEQRAM_1AC
    { 0x41AE, 2,  0x0087 }, //  DYNAMIC_SEQRAM_1AE
    { 0x41B0, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1B0
    { 0x41B2, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_1B2
    { 0x41B4, 2,  0x0036 }, //  DYNAMIC_SEQRAM_1B4
    { 0x41B6, 2,  0xD802 }, //  DYNAMIC_SEQRAM_1B6
    { 0x41B8, 2,  0x0D02 }, //  DYNAMIC_SEQRAM_1B8
    { 0x41BA, 2,  0x05F0 }, //  DYNAMIC_SEQRAM_1BA
    { 0x41BC, 2,  0x0036 }, //  DYNAMIC_SEQRAM_1BC
    { 0x41BE, 2,  0xD8F0 }, //  DYNAMIC_SEQRAM_1BE
    { 0x41C0, 2,  0x0083 }, //  DYNAMIC_SEQRAM_1C0
    { 0x41C2, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1C2
    { 0x41C4, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_1C4
    { 0x41C6, 2,  0x0087 }, //  DYNAMIC_SEQRAM_1C6
    { 0x41C8, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1C8
    { 0x41CA, 2,  0x36D8 }, //  DYNAMIC_SEQRAM_1CA
    { 0x41CC, 2,  0x020D }, //  DYNAMIC_SEQRAM_1CC
    { 0x41CE, 2,  0x0205 }, //  DYNAMIC_SEQRAM_1CE
    { 0x41D0, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1D0
    { 0x41D2, 2,  0x36D8 }, //  DYNAMIC_SEQRAM_1D2
    { 0x41D4, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1D4
    { 0x41D6, 2,  0x83F0 }, //  DYNAMIC_SEQRAM_1D6
    { 0x41D8, 2,  0x0087 }, //  DYNAMIC_SEQRAM_1D8
    { 0x41DA, 2,  0xF000 }, //  DYNAMIC_SEQRAM_1DA
    { 0x41DC, 2,  0x8713 }, //  DYNAMIC_SEQRAM_1DC
    { 0x41DE, 2,  0x0036 }, //  DYNAMIC_SEQRAM_1DE
    { 0x41E0, 2,  0xD802 }, //  DYNAMIC_SEQRAM_1E0
    { 0x41E2, 2,  0x0DE0 }, //  DYNAMIC_SEQRAM_1E2
    { 0x41E4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_1E4
    { 0x41E6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_1E6
    { 0x41E8, 2,  0x9F13 }, //  DYNAMIC_SEQRAM_1E8
    { 0x41EA, 2,  0x0041 }, //  DYNAMIC_SEQRAM_1EA
    { 0x41EC, 2,  0x80F3 }, //  DYNAMIC_SEQRAM_1EC
    { 0x41EE, 2,  0xF213 }, //  DYNAMIC_SEQRAM_1EE
    { 0x41F0, 2,  0x00F0 }, //  DYNAMIC_SEQRAM_1F0
    { 0x41F2, 2,  0x13B8 }, //  DYNAMIC_SEQRAM_1F2
    { 0x41F4, 2,  0xF04C }, //  DYNAMIC_SEQRAM_1F4
    { 0x41F6, 2,  0x9FF0 }, //  DYNAMIC_SEQRAM_1F6
    { 0x41F8, 2,  0x00B7 }, //  DYNAMIC_SEQRAM_1F8
    { 0x41FA, 2,  0xF006 }, //  DYNAMIC_SEQRAM_1FA
    { 0x41FC, 2,  0x0035 }, //  DYNAMIC_SEQRAM_1FC
    { 0x41FE, 2,  0x10AF }, //  DYNAMIC_SEQRAM_1FE
    { 0x4200, 2,  0x3003 }, //  DYNAMIC_SEQRAM_200
    { 0x4202, 2,  0x30C0 }, //  DYNAMIC_SEQRAM_202
    { 0x4204, 2,  0xB2F0 }, //  DYNAMIC_SEQRAM_204
    { 0x4206, 2,  0x01B5 }, //  DYNAMIC_SEQRAM_206
    { 0x4208, 2,  0xF001 }, //  DYNAMIC_SEQRAM_208
    { 0x420A, 2,  0x85F0 }, //  DYNAMIC_SEQRAM_20A
    { 0x420C, 2,  0x0292 }, //  DYNAMIC_SEQRAM_20C
    { 0x420E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_20E
    { 0x4210, 2,  0x9A8B }, //  DYNAMIC_SEQRAM_210
    { 0x4212, 2,  0xF000 }, //  DYNAMIC_SEQRAM_212
    { 0x4214, 2,  0x9997 }, //  DYNAMIC_SEQRAM_214
    { 0x4216, 2,  0xF007 }, //  DYNAMIC_SEQRAM_216
    { 0x4218, 2,  0xB6F0 }, //  DYNAMIC_SEQRAM_218
    { 0x421A, 2,  0x0020 }, //  DYNAMIC_SEQRAM_21A
    { 0x421C, 2,  0x5830 }, //  DYNAMIC_SEQRAM_21C
    { 0x421E, 2,  0xC040 }, //  DYNAMIC_SEQRAM_21E
    { 0x4220, 2,  0x1282 }, //  DYNAMIC_SEQRAM_220
    { 0x4222, 2,  0xF005 }, //  DYNAMIC_SEQRAM_222
    { 0x4224, 2,  0x9CF0 }, //  DYNAMIC_SEQRAM_224
    { 0x4226, 2,  0x01B2 }, //  DYNAMIC_SEQRAM_226
    { 0x4228, 2,  0xF008 }, //  DYNAMIC_SEQRAM_228
    { 0x422A, 2,  0xB8F0 }, //  DYNAMIC_SEQRAM_22A
    { 0x422C, 2,  0x0799 }, //  DYNAMIC_SEQRAM_22C
    { 0x422E, 2,  0xF005 }, //  DYNAMIC_SEQRAM_22E
    { 0x4230, 2,  0x98F0 }, //  DYNAMIC_SEQRAM_230
    { 0x4232, 2,  0x0296 }, //  DYNAMIC_SEQRAM_232
    { 0x4234, 2,  0xA2F0 }, //  DYNAMIC_SEQRAM_234
    { 0x4236, 2,  0x00A2 }, //  DYNAMIC_SEQRAM_236
    { 0x4238, 2,  0xF008 }, //  DYNAMIC_SEQRAM_238
    { 0x423A, 2,  0x9DF0 }, //  DYNAMIC_SEQRAM_23A
    { 0x423C, 2,  0x02A1 }, //  DYNAMIC_SEQRAM_23C
    { 0x423E, 2,  0xF01F }, //  DYNAMIC_SEQRAM_23E
    { 0x4240, 2,  0x1009 }, //  DYNAMIC_SEQRAM_240
    { 0x4242, 2,  0x2220 }, //  DYNAMIC_SEQRAM_242
    { 0x4244, 2,  0x0808 }, //  DYNAMIC_SEQRAM_244
    { 0x4246, 2,  0xF000 }, //  DYNAMIC_SEQRAM_246
    { 0x4248, 2,  0x3600 }, //  DYNAMIC_SEQRAM_248
    { 0x424A, 2,  0xF000 }, //  DYNAMIC_SEQRAM_24A
    { 0x424C, 2,  0x88F0 }, //  DYNAMIC_SEQRAM_24C
    { 0x424E, 2,  0x0788 }, //  DYNAMIC_SEQRAM_24E
    { 0x4250, 2,  0x3600 }, //  DYNAMIC_SEQRAM_250
    { 0x4252, 2,  0xF000 }, //  DYNAMIC_SEQRAM_252
    { 0x4254, 2,  0x83F0 }, //  DYNAMIC_SEQRAM_254
    { 0x4256, 2,  0x0290 }, //  DYNAMIC_SEQRAM_256
    { 0x4258, 2,  0xF016 }, //  DYNAMIC_SEQRAM_258
    { 0x425A, 2,  0x8BF0 }, //  DYNAMIC_SEQRAM_25A
    { 0x425C, 2,  0x11A3 }, //  DYNAMIC_SEQRAM_25C
    { 0x425E, 2,  0xF000 }, //  DYNAMIC_SEQRAM_25E
    { 0x4260, 2,  0xA3F0 }, //  DYNAMIC_SEQRAM_260
    { 0x4262, 2,  0x089D }, //  DYNAMIC_SEQRAM_262
    { 0x4264, 2,  0xF002 }, //  DYNAMIC_SEQRAM_264
    { 0x4266, 2,  0xA1F0 }, //  DYNAMIC_SEQRAM_266
    { 0x4268, 2,  0x20A1 }, //  DYNAMIC_SEQRAM_268
    { 0x426A, 2,  0xF006 }, //  DYNAMIC_SEQRAM_26A
    { 0x426C, 2,  0x4300 }, //  DYNAMIC_SEQRAM_26C
    { 0x426E, 2,  0xF049 }, //  DYNAMIC_SEQRAM_26E
    { 0x4270, 2,  0x4014 }, //  DYNAMIC_SEQRAM_270
    { 0x4272, 2,  0x8B8E }, //  DYNAMIC_SEQRAM_272
    { 0x4274, 2,  0x9DF0 }, //  DYNAMIC_SEQRAM_274
    { 0x4276, 2,  0x0802 }, //  DYNAMIC_SEQRAM_276
    { 0x4278, 2,  0x02F0 }, //  DYNAMIC_SEQRAM_278
    { 0x427A, 2,  0x00A6 }, //  DYNAMIC_SEQRAM_27A
    { 0x427C, 2,  0xF013 }, //  DYNAMIC_SEQRAM_27C
    { 0x427E, 2,  0xB283 }, //  DYNAMIC_SEQRAM_27E
    { 0x4280, 2,  0x9C36 }, //  DYNAMIC_SEQRAM_280
    { 0x4282, 2,  0x00F0 }, //  DYNAMIC_SEQRAM_282
    { 0x4284, 2,  0x0636 }, //  DYNAMIC_SEQRAM_284
    { 0x4286, 2,  0x009C }, //  DYNAMIC_SEQRAM_286
    { 0x4288, 2,  0xF008 }, //  DYNAMIC_SEQRAM_288
    { 0x428A, 2,  0x8BF0 }, //  DYNAMIC_SEQRAM_28A
    { 0x428C, 2,  0x0083 }, //  DYNAMIC_SEQRAM_28C
    { 0x428E, 2,  0xA0F0 }, //  DYNAMIC_SEQRAM_28E
    { 0x4290, 2,  0x0630 }, //  DYNAMIC_SEQRAM_290
    { 0x4292, 2,  0x18F0 }, //  DYNAMIC_SEQRAM_292
    { 0x4294, 2,  0x02A3 }, //  DYNAMIC_SEQRAM_294
    { 0x4296, 2,  0xF000 }, //  DYNAMIC_SEQRAM_296
    { 0x4298, 2,  0xA3F0 }, //  DYNAMIC_SEQRAM_298
    { 0x429A, 2,  0x0243 }, //  DYNAMIC_SEQRAM_29A
    { 0x429C, 2,  0x00F0 }, //  DYNAMIC_SEQRAM_29C
    { 0x429E, 2,  0x049D }, //  DYNAMIC_SEQRAM_29E
    { 0x42A0, 2,  0xF078 }, //  DYNAMIC_SEQRAM_2A0
    { 0x42A2, 2,  0x3018 }, //  DYNAMIC_SEQRAM_2A2
    { 0x42A4, 2,  0xF000 }, //  DYNAMIC_SEQRAM_2A4
    { 0x42A6, 2,  0x9D82 }, //  DYNAMIC_SEQRAM_2A6
    { 0x42A8, 2,  0xF000 }, //  DYNAMIC_SEQRAM_2A8
    { 0x42AA, 2,  0x9030 }, //  DYNAMIC_SEQRAM_2AA
    { 0x42AC, 2,  0xC0F0 }, //  DYNAMIC_SEQRAM_2AC
    { 0x42AE, 2,  0x1130 }, //  DYNAMIC_SEQRAM_2AE
    { 0x42B0, 2,  0xC0F0 }, //  DYNAMIC_SEQRAM_2B0
    { 0x42B2, 2,  0x0082 }, //  DYNAMIC_SEQRAM_2B2
    { 0x42B4, 2,  0xF001 }, //  DYNAMIC_SEQRAM_2B4
    { 0x42B6, 2,  0x1009 }, //  DYNAMIC_SEQRAM_2B6
    { 0x42B8, 2,  0xF02A }, //  DYNAMIC_SEQRAM_2B8
    { 0x42BA, 2,  0xA2F0 }, //  DYNAMIC_SEQRAM_2BA
    { 0x42BC, 2,  0x00A2 }, //  DYNAMIC_SEQRAM_2BC
    { 0x42BE, 2,  0x3018 }, //  DYNAMIC_SEQRAM_2BE
    { 0x42C0, 2,  0xF007 }, //  DYNAMIC_SEQRAM_2C0
    { 0x42C2, 2,  0x9DF0 }, //  DYNAMIC_SEQRAM_2C2
    { 0x42C4, 2,  0x1C8C }, //  DYNAMIC_SEQRAM_2C4
    { 0x42C6, 2,  0xF005 }, //  DYNAMIC_SEQRAM_2C6
    { 0x42C8, 2,  0x301F }, //  DYNAMIC_SEQRAM_2C8
    { 0x42CA, 2,  0x216D }, //  DYNAMIC_SEQRAM_2CA
    { 0x42CC, 2,  0x0A51 }, //  DYNAMIC_SEQRAM_2CC
    { 0x42CE, 2,  0x1FEA }, //  DYNAMIC_SEQRAM_2CE
    { 0x42D0, 2,  0x8640 }, //  DYNAMIC_SEQRAM_2D0
    { 0x42D2, 2,  0xE29F }, //  DYNAMIC_SEQRAM_2D2
    { 0x42D4, 2,  0xF009 }, //  DYNAMIC_SEQRAM_2D4
    { 0x42D6, 2,  0x0005 }, //  DYNAMIC_SEQRAM_2D6
    { 0x42D8, 2,  0xF000 }, //  DYNAMIC_SEQRAM_2D8
    { 0x42DA, 2,  0x30C0 }, //  DYNAMIC_SEQRAM_2DA
    { 0x42DC, 2,  0xF001 }, //  DYNAMIC_SEQRAM_2DC
    { 0x42DE, 2,  0x83F0 }, //  DYNAMIC_SEQRAM_2DE
    { 0x42E0, 2,  0x0036 }, //  DYNAMIC_SEQRAM_2E0
    { 0x42E2, 2,  0x00F0 }, //  DYNAMIC_SEQRAM_2E2
    { 0x42E4, 2,  0x0087 }, //  DYNAMIC_SEQRAM_2E4
    { 0x42E6, 2,  0xF007 }, //  DYNAMIC_SEQRAM_2E6
    { 0x42E8, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_2E8
    { 0x42EA, 2,  0x0036 }, //  DYNAMIC_SEQRAM_2EA
    { 0x42EC, 2,  0xC0F0 }, //  DYNAMIC_SEQRAM_2EC
    { 0x42EE, 2,  0x0000 }, //  DYNAMIC_SEQRAM_2EE
    { 0x42F0, 2,  0x0DF0 }, //  DYNAMIC_SEQRAM_2F0
    { 0x42F2, 2,  0x0000 }, //  DYNAMIC_SEQRAM_2F2
    { 0x42F4, 2,  0x05F0 }, //  DYNAMIC_SEQRAM_2F4
    { 0x42F6, 2,  0x0030 }, //  DYNAMIC_SEQRAM_2F6
    { 0x42F8, 2,  0xC0F0 }, //  DYNAMIC_SEQRAM_2F8
    { 0x42FA, 2,  0x0183 }, //  DYNAMIC_SEQRAM_2FA
    { 0x42FC, 2,  0xF000 }, //  DYNAMIC_SEQRAM_2FC
    { 0x42FE, 2,  0x3600 }, //  DYNAMIC_SEQRAM_2FE
    { 0x4300, 2,  0xF000 }, //  DYNAMIC_SEQRAM_300
    { 0x4302, 2,  0x87F0 }, //  DYNAMIC_SEQRAM_302
    { 0x4304, 2,  0x0787 }, //  DYNAMIC_SEQRAM_304
    { 0x4306, 2,  0xF000 }, //  DYNAMIC_SEQRAM_306
    { 0x4308, 2,  0x36C0 }, //  DYNAMIC_SEQRAM_308
    { 0x430A, 2,  0xF000 }, //  DYNAMIC_SEQRAM_30A
    { 0x430C, 2,  0x000F }, //  DYNAMIC_SEQRAM_30C
    { 0x430E, 2,  0xF42A }, //  DYNAMIC_SEQRAM_30E
    { 0x4310, 2,  0x4180 }, //  DYNAMIC_SEQRAM_310
    { 0x4312, 2,  0x1300 }, //  DYNAMIC_SEQRAM_312
    { 0x4314, 2,  0x9FF0 }, //  DYNAMIC_SEQRAM_314
    { 0x4316, 2,  0x00E0 }, //  DYNAMIC_SEQRAM_316
    { 0x4318, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_318
    { 0x431A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_31A
    { 0x431C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_31C
    { 0x431E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_31E
    { 0x4320, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_320
    { 0x4322, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_322
    { 0x4324, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_324
    { 0x4326, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_326
    { 0x4328, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_328
    { 0x432A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_32A
    { 0x432C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_32C
    { 0x432E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_32E
    { 0x4330, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_330
    { 0x4332, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_332
    { 0x4334, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_334
    { 0x4336, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_336
    { 0x4338, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_338
    { 0x433A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_33A
    { 0x433C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_33C
    { 0x433E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_33E
    { 0x4340, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_340
    { 0x4342, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_342
    { 0x4344, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_344
    { 0x4346, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_346
    { 0x4348, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_348
    { 0x434A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_34A
    { 0x434C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_34C
    { 0x434E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_34E
    { 0x4350, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_350
    { 0x4352, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_352
    { 0x4354, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_354
    { 0x4356, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_356
    { 0x4358, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_358
    { 0x435A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_35A
    { 0x435C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_35C
    { 0x435E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_35E
    { 0x4360, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_360
    { 0x4362, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_362
    { 0x4364, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_364
    { 0x4366, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_366
    { 0x4368, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_368
    { 0x436A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_36A
    { 0x436C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_36C
    { 0x436E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_36E
    { 0x4370, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_370
    { 0x4372, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_372
    { 0x4374, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_374
    { 0x4376, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_376
    { 0x4378, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_378
    { 0x437A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_37A
    { 0x437C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_37C
    { 0x437E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_37E
    { 0x4380, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_380
    { 0x4382, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_382
    { 0x4384, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_384
    { 0x4386, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_386
    { 0x4388, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_388
    { 0x438A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_38A
    { 0x438C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_38C
    { 0x438E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_38E
    { 0x4390, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_390
    { 0x4392, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_392
    { 0x4394, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_394
    { 0x4396, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_396
    { 0x4398, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_398
    { 0x439A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_39A
    { 0x439C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_39C
    { 0x439E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_39E
    { 0x43A0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3A0
    { 0x43A2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3A2
    { 0x43A4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3A4
    { 0x43A6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3A6
    { 0x43A8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3A8
    { 0x43AA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3AA
    { 0x43AC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3AC
    { 0x43AE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3AE
    { 0x43B0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3B0
    { 0x43B2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3B2
    { 0x43B4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3B4
    { 0x43B6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3B6
    { 0x43B8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3B8
    { 0x43BA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3BA
    { 0x43BC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3BC
    { 0x43BE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3BE
    { 0x43C0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3C0
    { 0x43C2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3C2
    { 0x43C4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3C4
    { 0x43C6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3C6
    { 0x43C8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3C8
    { 0x43CA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3CA
    { 0x43CC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3CC
    { 0x43CE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3CE
    { 0x43D0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3D0
    { 0x43D2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3D2
    { 0x43D4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3D4
    { 0x43D6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3D6
    { 0x43D8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3D8
    { 0x43DA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3DA
    { 0x43DC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3DC
    { 0x43DE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3DE
    { 0x43E0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3E0
    { 0x43E2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3E2
    { 0x43E4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3E4
    { 0x43E6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3E6
    { 0x43E8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3E8
    { 0x43EA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3EA
    { 0x43EC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3EC
    { 0x43EE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3EE
    { 0x43F0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3F0
    { 0x43F2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3F2
    { 0x43F4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3F4
    { 0x43F6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3F6
    { 0x43F8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3F8
    { 0x43FA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3FA
    { 0x43FC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3FC
    { 0x43FE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_3FE
    { 0x4400, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_400
    { 0x4402, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_402
    { 0x4404, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_404
    { 0x4406, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_406
    { 0x4408, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_408
    { 0x440A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_40A
    { 0x440C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_40C
    { 0x440E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_40E
    { 0x4410, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_410
    { 0x4412, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_412
    { 0x4414, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_414
    { 0x4416, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_416
    { 0x4418, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_418
    { 0x441A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_41A
    { 0x441C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_41C
    { 0x441E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_41E
    { 0x4420, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_420
    { 0x4422, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_422
    { 0x4424, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_424
    { 0x4426, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_426
    { 0x4428, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_428
    { 0x442A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_42A
    { 0x442C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_42C
    { 0x442E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_42E
    { 0x4430, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_430
    { 0x4432, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_432
    { 0x4434, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_434
    { 0x4436, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_436
    { 0x4438, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_438
    { 0x443A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_43A
    { 0x443C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_43C
    { 0x443E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_43E
    { 0x4440, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_440
    { 0x4442, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_442
    { 0x4444, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_444
    { 0x4446, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_446
    { 0x4448, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_448
    { 0x444A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_44A
    { 0x444C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_44C
    { 0x444E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_44E
    { 0x4450, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_450
    { 0x4452, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_452
    { 0x4454, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_454
    { 0x4456, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_456
    { 0x4458, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_458
    { 0x445A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_45A
    { 0x445C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_45C
    { 0x445E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_45E
    { 0x4460, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_460
    { 0x4462, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_462
    { 0x4464, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_464
    { 0x4466, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_466
    { 0x4468, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_468
    { 0x446A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_46A
    { 0x446C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_46C
    { 0x446E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_46E
    { 0x4470, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_470
    { 0x4472, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_472
    { 0x4474, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_474
    { 0x4476, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_476
    { 0x4478, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_478
    { 0x447A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_47A
    { 0x447C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_47C
    { 0x447E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_47E
    { 0x4480, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_480
    { 0x4482, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_482
    { 0x4484, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_484
    { 0x4486, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_486
    { 0x4488, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_488
    { 0x448A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_48A
    { 0x448C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_48C
    { 0x448E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_48E
    { 0x4490, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_490
    { 0x4492, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_492
    { 0x4494, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_494
    { 0x4496, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_496
    { 0x4498, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_498
    { 0x449A, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_49A
    { 0x449C, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_49C
    { 0x449E, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_49E
    { 0x44A0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4A0
    { 0x44A2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4A2
    { 0x44A4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4A4
    { 0x44A6, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4A6
    { 0x44A8, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4A8
    { 0x44AA, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4AA
    { 0x44AC, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4AC
    { 0x44AE, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4AE
    { 0x44B0, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4B0
    { 0x44B2, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4B2
    { 0x44B4, 2,  0xE0E0 }, //  DYNAMIC_SEQRAM_4B4
    { 0x5500, 2,  0x0000 }, //  AGAIN_LUT0
    { 0x5502, 2,  0x0002 }, //  AGAIN_LUT1
    { 0x5504, 2,  0x0006 }, //  AGAIN_LUT2
    { 0x5506, 2,  0x0009 }, //  AGAIN_LUT3
    { 0x5508, 2,  0x000F }, //  AGAIN_LUT4
    { 0x550A, 2,  0x0010 }, //  AGAIN_LUT5
    { 0x550C, 2,  0x0011 }, //  AGAIN_LUT6
    { 0x550E, 2,  0x0012 }, //  AGAIN_LUT7
    { 0x5510, 2,  0x0019 }, //  AGAIN_LUT8
    { 0x5512, 2,  0x0020 }, //  AGAIN_LUT9
    { 0x5514, 2,  0x0021 }, //  AGAIN_LUT10
    { 0x5516, 2,  0x0023 }, //  AGAIN_LUT11
    { 0x5518, 2,  0x0026 }, //  AGAIN_LUT12
    { 0x551A, 2,  0x002B }, //  AGAIN_LUT13
    { 0x551C, 2,  0x002F }, //  AGAIN_LUT14
    { 0x551E, 2,  0x0030 }, //  AGAIN_LUT15
    { 0x5400, 2,  0x0100 }, //  GT1_COARSE0
    { 0x5402, 2,  0x2106 }, //  GT1_COARSE1
    { 0x5404, 2,  0x1101 }, //  GT1_COARSE2
    { 0x5406, 2,  0x3106 }, //  GT1_COARSE3
    { 0x5408, 2,  0x7100 }, //  GT1_COARSE4
    { 0x540A, 2,  0x8107 }, //  GT1_COARSE5
    { 0x540C, 2,  0xB101 }, //  GT1_COARSE6
    { 0x540E, 2,  0xD101 }, //  GT1_COARSE7
    { 0x5410, 2,  0xF12E }, //  GT1_COARSE8
    { 0x5412, 2,  0xF112 }, //  GT1_COARSE9
    { 0x5414, 2,  0xF184 }, //  GT1_COARSE10
    { 0x5416, 2,  0xF224 }, //  GT1_COARSE11
    { 0x5418, 2,  0xF306 }, //  GT1_COARSE12
    { 0x541A, 2,  0xF446 }, //  GT1_COARSE13
    { 0x541C, 2,  0xF609 }, //  GT1_COARSE14
    { 0x541E, 2,  0xF887 }, //  GT1_COARSE15
    { 0x5420, 2,  0xFC0B }, //  GT1_COARSE16
    { 0x5422, 2,  0xFC0B }, //  GT1_COARSE17
    { 0x5424, 2,  0xFFFA }, //  GT1_DCG_ATTN_SET0
    { 0x5426, 2,  0x5557 }, //  GT1_DCG_ATTN_SET1
    { 0x5428, 2,  0x0005 }, //  GT1_DCG_ATTN_SET2
    { 0x542A, 2,  0xA550 }, //  GT1_ZONE_SET0
    { 0x542C, 2,  0xAAAA }, //  GT1_ZONE_SET1
    { 0x542E, 2,  0x000A }, //  GT1_ZONE_SET2
    { 0x5460, 2,  0x2269 }, //  ZT1_REG0_ADDR
    { 0x5462, 2,  0x0B87 }, //  ZT1_REG0_VALUE0
    { 0x5464, 2,  0x0B87 }, //  ZT1_REG0_VALUE1
    { 0x5466, 2,  0x0983 }, //  ZT1_REG0_VALUE2
    { 0x5498, 2,  0x225E }, //  ZT1_REG7_ADDR
    { 0x549A, 2,  0xBCAA }, //  ZT1_REG7_VALUE0
    { 0x549C, 2,  0xBCAA }, //  ZT1_REG7_VALUE1
    { 0x549E, 2,  0xBDAA }, //  ZT1_REG7_VALUE2
    { 0x3060, 2,  0xFF01 }, //  GAIN_TABLE_CTRL
    { 0x44BA, 2,  0x0050 }, //  DAC_LD_4_5
    { 0x44BC, 2,  0xBCAA }, //  DAC_LD_6_7
    { 0x44C0, 2,  0x4070 }, //  DAC_LD_10_11
    { 0x44C4, 2,  0x04D0 }, //  DAC_LD_14_15
    { 0x44C6, 2,  0x17E2 }, //  DAC_LD_16_17
    { 0x44C8, 2,  0xEA43 }, //  DAC_LD_18_19
    { 0x44CA, 2,  0x000E }, //  DAC_LD_20_21
    { 0x44CC, 2,  0x7777 }, //  DAC_LD_22_23
    { 0x44CE, 2,  0x8BA4 }, //  DAC_LD_24_25
    { 0x44D0, 2,  0x1735 }, //  DAC_LD_26_27
    { 0x44D2, 2,  0x0B87 }, //  DAC_LD_28_29
    { 0x44D4, 2,  0x0000 }, //  DAC_LD_30_31
    { 0x44D6, 2,  0xF206 }, //  DAC_LD_32_33
    { 0x44D8, 2,  0xAAFA }, //  DAC_LD_34_35
    { 0x44DA, 2,  0xE001 }, //  DAC_LD_36_37
    { 0x44DE, 2,  0x9BBC }, //  DAC_LD_40_41
    { 0x44E0, 2,  0x283C }, //  DAC_LD_42_43
    { 0x44E2, 2,  0x2821 }, //  DAC_LD_44_45
    { 0x44E4, 2,  0x8000 }, //  DAC_LD_46_47
    { 0x44E6, 2,  0x503F }, //  DAC_LD_48_49
    { 0x32A4, 2,  0x0000 }, //  CRM_CTRL
    { 0x328E, 2,  0x0004 }, //  ADDR_CTRL
    { 0x333C, 2,  0x0001 }, //  DYNAMIC_CTRL
    { 0x301A, 2,  0x0000 }, //  RESET_REGISTER
    { 0x3600, 2,  0x94DF }, //  FDOC_CTRL
    { 0x3616, 2,  0x0000 }, //  FDOC_CTRL2
    { 0x3700, 2,  0x0001 }, //  PIX_DEF_ID
    { 0x3980, 2,  0x0003 }, //  PIX_DEF_CORR
    { 0x36C0, 2,  0x0001 }, //  DIGITAL_GAIN_CTRL
    { 0x36DE, 2,  0x002A }, //  DATA_PEDESTAL1
    { 0x301A, 2,  0x0008 }, //  RESET_REGISTER
    { 0x3060, 2,  0x0000 }, //  GAIN_TABLE_CTRL
    { 0x3982, 2,  0xAC70 }, //  PDC_DYN_EDGE_THRES
    { 0x3984, 2,  0xFA98 }, //  PDC_DYN_LO_DEFECT_THRES
    { 0x3986, 2,  0xFC3F }, //  PDC_DYN_HI_DEFECT_THRES
    { 0x3988, 2,  0xAC70 }, //  PDC_DYN_EDGE_THRES_T2
    { 0x398A, 2,  0xFA98 }, //  PDC_DYN_LO_DEFECT_THRES_T2
    { 0x398C, 2,  0xFC3F }, //  PDC_DYN_HI_DEFECT_THRES_T2
    { 0x3980, 2,  0x0003 }, //  PIX_DEF_CORR
    { 0x3060, 2,  0xFF01 }, //  GAIN_TABLE_CTRL
    { 0x3340, 2,  0x1C60 }, //  OTPM_CTRL
    { 0x400, 2,   0x3 }, //  X
    { 0x404, 2,   0xFF9 }, //  Scale_M
    { 0x0406, 2,   0x400 }, //  Scale_N
    { 0x3C58, 2,   0x8 }, //  Slice overlap
    { 0x3C40, 2,   0x2 }, //  Scaling Mode
    { 0x3C42, 2,   0xD36 }, //  SCALE_SECOND_RESIDUAL_EVEN=3382
    { 0x3C44, 2,   0x339 }, //  SCALE_SECOND_RESIDUAL_ODD=825
    { 0x3C46, 2,   0x708 }, //  scale_second_crop_e=7scale_second_crop_o=8
    { 0x3C48, 2,   0xA57 }, //  SCALE_THIRD_RESIDUAL_EVEN=2647
    { 0x3C4A, 2,   0x5A }, //  SCALE_THIRD_RESIDUAL_ODD=90
    { 0x3C4C, 2,   0x607 }, //  scale_third_crop_e=6scale_third_crop_o=7
    { 0x3C4E, 2,   0x778 }, //  SCALE_FOURTH_RESIDUAL_EVEN=1912
    { 0x3C50, 2,   0xD74 }, //  SCALE_FOURTH_RESIDUAL_ODD=3444
    { 0x3C52, 2,   0x505 }, //  scale_fourth_crop_e=5scale_fourth_crop_o=5
    { 0x3C60, 2,   0xD8 }, //  SCALE_FIRST_PXL_NUM=216
    { 0x3C62, 2,   0xD0 }, //  SCALE_SECOND_PXL_NUM=208
    { 0x3C64, 2,   0xD0 }, //  SCALE_THIRD_PXL_NUM=208
    { 0x3C66, 2,   0xD0 }, //  SCALE_FOURTH_PXL_NUM=208
    { 0x3EC8, 2,   0x0 }, //  X_OUTPU_OFFSET:0
    { 0x3ECA, 2,   0x0 }, //  Y_OUTPU_OFFSET:0
    { 0x034C, 2,   0x340 }, //  X_OUTPU_SIZE:840
    { 0x034E, 2,   0x260 }, //  Y_OUTPU_SIZE:624
    { 0x3C6A, 2,   0x0 }, //  X_OUTPU_OFFSET2:0
    { 0x3C70, 2,   0x0 }, //  Y_OUTPU_OFFSET2:0
    { 0x3C6C, 2,   0x340 }, //  X_OUTPU_SIZE2:840
    { 0x3C72, 2,   0x260 }, //  Y_OUTPU_SIZE2:624
    { 0x3C7A, 2,   0x0 }, //  reorder_ctrl
    { 0x3C74, 2,   0x3 }, //  Sync_regen
    { 0x3C76, 2,   0x20 }, //  Sync_regen_blank
    { 0x3F18, 2,  0x7B70 }, //  MIPI_JPEG_PN9_DATA_TYPE

    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE
    {0x0100, 1,  0x01 }, //  MODE_SELECT

    {0x44D6, 2,  0xB206 }, //  DAC_LD_32_33
};

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_608x448_608x448_table[] =
{
    {0x0103, 0x3201, 0x01}, // SOFTWARE_RESET

    { 0x0304, 0x2,  0x2 }, // PRE_PLL_CLK_DIV1=2
    { 0x0306, 0x2,  0x56 }, // PLL_MULTIPLIER1=86
    { 0x0300, 0x2, 0x6 }, // VT_PIX_CLK_DIV=6
    { 0x0302, 0x2, 0x1 }, // VT_SYS_CLK_DIV=1
    { 0x030C, 0x2,  0x7 }, // PRE_PLL_CLK_DIV2=7
    { 0x030E, 0x2,  0x247 }, //{ 0x030E, 0x2,  0x15E }, // PLL_MULTIPLIER2=350
    { 0x0308, 0x2, 0x5 }, // OP_PIX_CLK_DIV=5
    { 0x030A, 0x2, 0x2 }, // OP_SYS_CLK_DIV=2
    {0x0344, 0x2, 0x378 }, // X_ADDR_START 888
    {0x0348, 0x2, 0x1097 }, // X_ADDR_END 4247
    {0x0346, 0x2, 0x2A8 }, // Y_ADDR_START 680
    {0x034A, 0x2, 0xC67 }, // Y_ADDR_END 3175
    {0x034C, 0x2, 0xD20 }, // X_OUTPUT_SIZE 3360
    {0x034E, 0x2, 0x9C0 }, // Y_OUTPUT_SIZE 2496
    { 0x0380, 0x2,  0x1 }, // X_EVEN_INC
    { 0x0382, 0x2,  0x1 }, // X_ODD_INC
    { 0x0384, 0x2,  0x1 }, // Y_EVEN_INC
    { 0x0386, 0x2,  0x1 }, // Y_ODD_INC

    { 0x0900, 0x1,  0x0}, // binning_mode
    { 0x0901, 0x1,  0x11}, // binning_type

    {0x0342, 0x2, 0x1780 }, // LINE_LENGTH_PCK
    {0x0340, 0x2, 0x9EE }, // FRAME_LENGTH_LINES 9ce= 91.1fps
    {0x0202, 0x2, 0x900 }, // COARSE_INTEGRATION_TIME
    { 0x0112, 0x2,  0xA0A}, // CSI_DATA_FORMAT
    { 0x0114, 0x1,  0x3}, // CSI_LANE_MODE

    {0x0800, 0x1, 0xD}, // TCLK_POST
    {0x0801, 0x1, 0x7}, // THS_PREPARE
    {0x0802, 0x1, 0xC}, // THS_ZERO_MIN
    {0x0803, 0x1, 0x9}, //  THS_TRAIL
    {0x0804, 0x1, 0xB}, // TCLK_TRAIL_MIN
    {0x0805, 0x1, 0x6}, // TCLK_PREPARE
    {0x0806, 0x1, 0x24}, // TCLK_ZERO
    {0x0807, 0x1, 0x7}, // TLPX
    {0x082A, 0x1, 0xF}, // TWAKEUP
    {0x082B, 0x1, 0xC}, // TNINT
    {0x082C, 0x1, 0xD}, // THS_EXIT
    {0x3F06, 0x2, 0xC0}, // MIPI_TIMING_2 C0
    {0x3F0A, 0x2, 0x2000}, //{0x3F0A, 0x2, 0xA000}, // MIPI_TIMING_4 A000
    {0x3F0C, 0x2, 0xA}, // MIPI_TIMING_5 A
    { 0x3F20, 0x2, 0x8080 }, //  MIPI_PHY_TRIM_MSB
    { 0x3F1E, 0x2, 0x0004 }, //  MIPI_PHY_TRIM_LSB

    { 0x4000, 0x2, 0x0114 }, //  DYNAMIC_SEQRAM_00
    { 0x4002, 0x2, 0x1A25 }, //  DYNAMIC_SEQRAM_02
    { 0x4004, 0x2, 0x3DFF }, //  DYNAMIC_SEQRAM_04
    { 0x4006, 0x2, 0xFFFF }, //  DYNAMIC_SEQRAM_06
    { 0x4008, 0x2, 0x0A35 }, //  DYNAMIC_SEQRAM_08
    { 0x400A, 0x2, 0x10EF }, //  DYNAMIC_SEQRAM_0A
    { 0x400C, 0x2, 0x3003 }, //  DYNAMIC_SEQRAM_0C
    { 0x400E, 0x2, 0x30D8 }, //  DYNAMIC_SEQRAM_0E
    { 0x4010, 0x2, 0xF003 }, //  DYNAMIC_SEQRAM_10
    { 0x4012, 0x2, 0xB5F0 }, //  DYNAMIC_SEQRAM_12
    { 0x4014, 0x2, 0x0085 }, //  DYNAMIC_SEQRAM_14
    { 0x4016, 0x2, 0xF004 }, //  DYNAMIC_SEQRAM_16
    { 0x4018, 0x2, 0x9A89 }, //  DYNAMIC_SEQRAM_18
    { 0x401A, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1A
    { 0x401C, 0x2, 0x9997 }, //  DYNAMIC_SEQRAM_1C
    { 0x401E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1E
    { 0x4020, 0x2, 0x30C0 }, //  DYNAMIC_SEQRAM_20
    { 0x4022, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_22
    { 0x4024, 0x2, 0x82F0 }, //  DYNAMIC_SEQRAM_24
    { 0x4026, 0x2, 0x0030 }, //  DYNAMIC_SEQRAM_26
    { 0x4028, 0x2, 0x18F0 }, //  DYNAMIC_SEQRAM_28
    { 0x402A, 0x2, 0x0320 }, //  DYNAMIC_SEQRAM_2A
    { 0x402C, 0x2, 0x58F0 }, //  DYNAMIC_SEQRAM_2C
    { 0x402E, 0x2, 0x089C }, //  DYNAMIC_SEQRAM_2E
    { 0x4030, 0x2, 0xF010 }, //  DYNAMIC_SEQRAM_30
    { 0x4032, 0x2, 0x99B6 }, //  DYNAMIC_SEQRAM_32
    { 0x4034, 0x2, 0xF003 }, //  DYNAMIC_SEQRAM_34
    { 0x4036, 0x2, 0xB498 }, //  DYNAMIC_SEQRAM_36
    { 0x4038, 0x2, 0xA096 }, //  DYNAMIC_SEQRAM_38
    { 0x403A, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_3A
    { 0x403C, 0x2, 0xA2F0 }, //  DYNAMIC_SEQRAM_3C
    { 0x403E, 0x2, 0x00A2 }, //  DYNAMIC_SEQRAM_3E
    { 0x4040, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_40
    { 0x4042, 0x2, 0x9DF0 }, //  DYNAMIC_SEQRAM_42
    { 0x4044, 0x2, 0x209D }, //  DYNAMIC_SEQRAM_44
    { 0x4046, 0x2, 0x8C08 }, //  DYNAMIC_SEQRAM_46
    { 0x4048, 0x2, 0x08F0 }, //  DYNAMIC_SEQRAM_48
    { 0x404A, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_4A
    { 0x404C, 0x2, 0x008F }, //  DYNAMIC_SEQRAM_4C
    { 0x404E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_4E
    { 0x4050, 0x2, 0x88F0 }, //  DYNAMIC_SEQRAM_50
    { 0x4052, 0x2, 0x0488 }, //  DYNAMIC_SEQRAM_52
    { 0x4054, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_54
    { 0x4056, 0x2, 0x3600 }, //  DYNAMIC_SEQRAM_56
    { 0x4058, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_58
    { 0x405A, 0x2, 0x83F0 }, //  DYNAMIC_SEQRAM_5A
    { 0x405C, 0x2, 0x0290 }, //  DYNAMIC_SEQRAM_5C
    { 0x405E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_5E
    { 0x4060, 0x2, 0x8BF0 }, //  DYNAMIC_SEQRAM_60
    { 0x4062, 0x2, 0x2EA3 }, //  DYNAMIC_SEQRAM_62
    { 0x4064, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_64
    { 0x4066, 0x2, 0xA3F0 }, //  DYNAMIC_SEQRAM_66
    { 0x4068, 0x2, 0x089D }, //  DYNAMIC_SEQRAM_68
    { 0x406A, 0x2, 0xF075 }, //  DYNAMIC_SEQRAM_6A
    { 0x406C, 0x2, 0x3003 }, //  DYNAMIC_SEQRAM_6C
    { 0x406E, 0x2, 0x4070 }, //  DYNAMIC_SEQRAM_6E
    { 0x4070, 0x2, 0x216D }, //  DYNAMIC_SEQRAM_70
    { 0x4072, 0x2, 0x1CF6 }, //  DYNAMIC_SEQRAM_72
    { 0x4074, 0x2, 0x8B00 }, //  DYNAMIC_SEQRAM_74
    { 0x4076, 0x2, 0x5186 }, //  DYNAMIC_SEQRAM_76
    { 0x4078, 0x2, 0x1300 }, //  DYNAMIC_SEQRAM_78
    { 0x407A, 0x2, 0x0205 }, //  DYNAMIC_SEQRAM_7A
    { 0x407C, 0x2, 0x36D8 }, //  DYNAMIC_SEQRAM_7C
    { 0x407E, 0x2, 0xF002 }, //  DYNAMIC_SEQRAM_7E
    { 0x4080, 0x2, 0x8387 }, //  DYNAMIC_SEQRAM_80
    { 0x4082, 0x2, 0xF006 }, //  DYNAMIC_SEQRAM_82
    { 0x4084, 0x2, 0x8702 }, //  DYNAMIC_SEQRAM_84
    { 0x4086, 0x2, 0x0D02 }, //  DYNAMIC_SEQRAM_86
    { 0x4088, 0x2, 0x05F0 }, //  DYNAMIC_SEQRAM_88
    { 0x408A, 0x2, 0x0383 }, //  DYNAMIC_SEQRAM_8A
    { 0x408C, 0x2, 0xF001 }, //  DYNAMIC_SEQRAM_8C
    { 0x408E, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_8E
    { 0x4090, 0x2, 0x0213 }, //  DYNAMIC_SEQRAM_90
    { 0x4092, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_92
    { 0x4094, 0x2, 0xD887 }, //  DYNAMIC_SEQRAM_94
    { 0x4096, 0x2, 0x020D }, //  DYNAMIC_SEQRAM_96
    { 0x4098, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_98
    { 0x409A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_9A
    { 0x409C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_9C
    { 0x409E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_9E
    { 0x40A0, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_A0
    { 0x40A2, 0x2, 0x0401 }, //  DYNAMIC_SEQRAM_A2
    { 0x40A4, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_A4
    { 0x40A6, 0x2, 0x82F0 }, //  DYNAMIC_SEQRAM_A6
    { 0x40A8, 0x2, 0x0883 }, //  DYNAMIC_SEQRAM_A8
    { 0x40AA, 0x2, 0xF009 }, //  DYNAMIC_SEQRAM_AA
    { 0x40AC, 0x2, 0x85F0 }, //  DYNAMIC_SEQRAM_AC
    { 0x40AE, 0x2, 0x2985 }, //  DYNAMIC_SEQRAM_AE
    { 0x40B0, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_B0
    { 0x40B2, 0x2, 0x2A87 }, //  DYNAMIC_SEQRAM_B2
    { 0x40B4, 0x2, 0xF63E }, //  DYNAMIC_SEQRAM_B4
    { 0x40B6, 0x2, 0x88F0 }, //  DYNAMIC_SEQRAM_B6
    { 0x40B8, 0x2, 0x0801 }, //  DYNAMIC_SEQRAM_B8
    { 0x40BA, 0x2, 0x40F0 }, //  DYNAMIC_SEQRAM_BA
    { 0x40BC, 0x2, 0x0800 }, //  DYNAMIC_SEQRAM_BC
    { 0x40BE, 0x2, 0x48F0 }, //  DYNAMIC_SEQRAM_BE
    { 0x40C0, 0x2, 0x0882 }, //  DYNAMIC_SEQRAM_C0
    { 0x40C2, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_C2
    { 0x40C4, 0x2, 0x0401 }, //  DYNAMIC_SEQRAM_C4
    { 0x40C6, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_C6
    { 0x40C8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_C8
    { 0x40CA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_CA
    { 0x40CC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_CC
    { 0x40CE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_CE
    { 0x40D0, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_D0
    { 0x40D2, 0x2, 0x0401 }, //  DYNAMIC_SEQRAM_D2
    { 0x40D4, 0x2, 0xF015 }, //  DYNAMIC_SEQRAM_D4
    { 0x40D6, 0x2, 0x002C }, //  DYNAMIC_SEQRAM_D6
    { 0x40D8, 0x2, 0xF00E }, //  DYNAMIC_SEQRAM_D8
    { 0x40DA, 0x2, 0x85F0 }, //  DYNAMIC_SEQRAM_DA
    { 0x40DC, 0x2, 0x0687 }, //  DYNAMIC_SEQRAM_DC
    { 0x40DE, 0x2, 0xF002 }, //  DYNAMIC_SEQRAM_DE
    { 0x40E0, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_E0
    { 0x40E2, 0x2, 0x61E8 }, //  DYNAMIC_SEQRAM_E2
    { 0x40E4, 0x2, 0x3900 }, //  DYNAMIC_SEQRAM_E4
    { 0x40E6, 0x2, 0xF005 }, //  DYNAMIC_SEQRAM_E6
    { 0x40E8, 0x2, 0x3480 }, //  DYNAMIC_SEQRAM_E8
    { 0x40EA, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_EA
    { 0x40EC, 0x2, 0x3240 }, //  DYNAMIC_SEQRAM_EC
    { 0x40EE, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_EE
    { 0x40F0, 0x2, 0x3900 }, //  DYNAMIC_SEQRAM_F0
    { 0x40F2, 0x2, 0xF00E }, //  DYNAMIC_SEQRAM_F2
    { 0x40F4, 0x2, 0x3900 }, //  DYNAMIC_SEQRAM_F4
    { 0x40F6, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_F6
    { 0x40F8, 0x2, 0x3240 }, //  DYNAMIC_SEQRAM_F8
    { 0x40FA, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_FA
    { 0x40FC, 0x2, 0x3480 }, //  DYNAMIC_SEQRAM_FC
    { 0x40FE, 0x2, 0xF005 }, //  DYNAMIC_SEQRAM_FE
    { 0x4100, 0x2, 0xC0E6 }, //  DYNAMIC_SEQRAM_100
    { 0x4102, 0x2, 0xF004 }, //  DYNAMIC_SEQRAM_102
    { 0x4104, 0x2, 0x3900 }, //  DYNAMIC_SEQRAM_104
    { 0x4106, 0x2, 0xF003 }, //  DYNAMIC_SEQRAM_106
    { 0x4108, 0x2, 0xB0F0 }, //  DYNAMIC_SEQRAM_108
    { 0x410A, 0x2, 0x0083 }, //  DYNAMIC_SEQRAM_10A
    { 0x410C, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_10C
    { 0x410E, 0x2, 0x86F0 }, //  DYNAMIC_SEQRAM_10E
    { 0x4110, 0x2, 0x0086 }, //  DYNAMIC_SEQRAM_110
    { 0x4112, 0x2, 0xF089 }, //  DYNAMIC_SEQRAM_112
    { 0x4114, 0x2, 0xB0F0 }, //  DYNAMIC_SEQRAM_114
    { 0x4116, 0x2, 0x00E9 }, //  DYNAMIC_SEQRAM_116
    { 0x4118, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_118
    { 0x411A, 0x2, 0x8AF0 }, //  DYNAMIC_SEQRAM_11A
    { 0x411C, 0x2, 0x0000 }, //  DYNAMIC_SEQRAM_11C
    { 0x411E, 0x2, 0x05F0 }, //  DYNAMIC_SEQRAM_11E
    { 0x4120, 0x2, 0x00E0 }, //  DYNAMIC_SEQRAM_120
    { 0x4122, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_122
    { 0x4124, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_124
    { 0x4126, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_126
    { 0x4128, 0x2, 0x0A35 }, //  DYNAMIC_SEQRAM_128
    { 0x412A, 0x2, 0x10EF }, //  DYNAMIC_SEQRAM_12A
    { 0x412C, 0x2, 0x3003 }, //  DYNAMIC_SEQRAM_12C
    { 0x412E, 0x2, 0x30D8 }, //  DYNAMIC_SEQRAM_12E
    { 0x4130, 0x2, 0xF005 }, //  DYNAMIC_SEQRAM_130
    { 0x4132, 0x2, 0x85F0 }, //  DYNAMIC_SEQRAM_132
    { 0x4134, 0x2, 0x049A }, //  DYNAMIC_SEQRAM_134
    { 0x4136, 0x2, 0x89F0 }, //  DYNAMIC_SEQRAM_136
    { 0x4138, 0x2, 0x0099 }, //  DYNAMIC_SEQRAM_138
    { 0x413A, 0x2, 0x97F0 }, //  DYNAMIC_SEQRAM_13A
    { 0x413C, 0x2, 0x0030 }, //  DYNAMIC_SEQRAM_13C
    { 0x413E, 0x2, 0xC0F0 }, //  DYNAMIC_SEQRAM_13E
    { 0x4140, 0x2, 0x0082 }, //  DYNAMIC_SEQRAM_140
    { 0x4142, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_142
    { 0x4144, 0x2, 0x3018 }, //  DYNAMIC_SEQRAM_144
    { 0x4146, 0x2, 0xF002 }, //  DYNAMIC_SEQRAM_146
    { 0x4148, 0x2, 0xB520 }, //  DYNAMIC_SEQRAM_148
    { 0x414A, 0x2, 0x58F0 }, //  DYNAMIC_SEQRAM_14A
    { 0x414C, 0x2, 0x089C }, //  DYNAMIC_SEQRAM_14C
    { 0x414E, 0x2, 0xF010 }, //  DYNAMIC_SEQRAM_14E
    { 0x4150, 0x2, 0x99B6 }, //  DYNAMIC_SEQRAM_150
    { 0x4152, 0x2, 0xF003 }, //  DYNAMIC_SEQRAM_152
    { 0x4154, 0x2, 0xB498 }, //  DYNAMIC_SEQRAM_154
    { 0x4156, 0x2, 0xA096 }, //  DYNAMIC_SEQRAM_156
    { 0x4158, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_158
    { 0x415A, 0x2, 0xA2F0 }, //  DYNAMIC_SEQRAM_15A
    { 0x415C, 0x2, 0x00A2 }, //  DYNAMIC_SEQRAM_15C
    { 0x415E, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_15E
    { 0x4160, 0x2, 0x9DF0 }, //  DYNAMIC_SEQRAM_160
    { 0x4162, 0x2, 0x209D }, //  DYNAMIC_SEQRAM_162
    { 0x4164, 0x2, 0x8C08 }, //  DYNAMIC_SEQRAM_164
    { 0x4166, 0x2, 0x08F0 }, //  DYNAMIC_SEQRAM_166
    { 0x4168, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_168
    { 0x416A, 0x2, 0x008F }, //  DYNAMIC_SEQRAM_16A
    { 0x416C, 0x2, 0x88F0 }, //  DYNAMIC_SEQRAM_16C
    { 0x416E, 0x2, 0x0188 }, //  DYNAMIC_SEQRAM_16E
    { 0x4170, 0x2, 0x3600 }, //  DYNAMIC_SEQRAM_170
    { 0x4172, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_172
    { 0x4174, 0x2, 0x83F0 }, //  DYNAMIC_SEQRAM_174
    { 0x4176, 0x2, 0x0290 }, //  DYNAMIC_SEQRAM_176
    { 0x4178, 0x2, 0xF001 }, //  DYNAMIC_SEQRAM_178
    { 0x417A, 0x2, 0x8BF0 }, //  DYNAMIC_SEQRAM_17A
    { 0x417C, 0x2, 0x2DA3 }, //  DYNAMIC_SEQRAM_17C
    { 0x417E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_17E
    { 0x4180, 0x2, 0xA3F0 }, //  DYNAMIC_SEQRAM_180
    { 0x4182, 0x2, 0x089D }, //  DYNAMIC_SEQRAM_182
    { 0x4184, 0x2, 0xF06D }, //  DYNAMIC_SEQRAM_184
    { 0x4186, 0x2, 0x4070 }, //  DYNAMIC_SEQRAM_186
    { 0x4188, 0x2, 0x3003 }, //  DYNAMIC_SEQRAM_188
    { 0x418A, 0x2, 0x214D }, //  DYNAMIC_SEQRAM_18A
    { 0x418C, 0x2, 0x1FF6 }, //  DYNAMIC_SEQRAM_18C
    { 0x418E, 0x2, 0x0851 }, //  DYNAMIC_SEQRAM_18E
    { 0x4190, 0x2, 0x0245 }, //  DYNAMIC_SEQRAM_190
    { 0x4192, 0x2, 0x9D36 }, //  DYNAMIC_SEQRAM_192
    { 0x4194, 0x2, 0xD8F0 }, //  DYNAMIC_SEQRAM_194
    { 0x4196, 0x2, 0x0083 }, //  DYNAMIC_SEQRAM_196
    { 0x4198, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_198
    { 0x419A, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_19A
    { 0x419C, 0x2, 0x0087 }, //  DYNAMIC_SEQRAM_19C
    { 0x419E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_19E
    { 0x41A0, 0x2, 0x36D8 }, //  DYNAMIC_SEQRAM_1A0
    { 0x41A2, 0x2, 0x020D }, //  DYNAMIC_SEQRAM_1A2
    { 0x41A4, 0x2, 0x0205 }, //  DYNAMIC_SEQRAM_1A4
    { 0x41A6, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1A6
    { 0x41A8, 0x2, 0x36D8 }, //  DYNAMIC_SEQRAM_1A8
    { 0x41AA, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1AA
    { 0x41AC, 0x2, 0x83F0 }, //  DYNAMIC_SEQRAM_1AC
    { 0x41AE, 0x2, 0x0087 }, //  DYNAMIC_SEQRAM_1AE
    { 0x41B0, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1B0
    { 0x41B2, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_1B2
    { 0x41B4, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_1B4
    { 0x41B6, 0x2, 0xD802 }, //  DYNAMIC_SEQRAM_1B6
    { 0x41B8, 0x2, 0x0D02 }, //  DYNAMIC_SEQRAM_1B8
    { 0x41BA, 0x2, 0x05F0 }, //  DYNAMIC_SEQRAM_1BA
    { 0x41BC, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_1BC
    { 0x41BE, 0x2, 0xD8F0 }, //  DYNAMIC_SEQRAM_1BE
    { 0x41C0, 0x2, 0x0083 }, //  DYNAMIC_SEQRAM_1C0
    { 0x41C2, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1C2
    { 0x41C4, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_1C4
    { 0x41C6, 0x2, 0x0087 }, //  DYNAMIC_SEQRAM_1C6
    { 0x41C8, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1C8
    { 0x41CA, 0x2, 0x36D8 }, //  DYNAMIC_SEQRAM_1CA
    { 0x41CC, 0x2, 0x020D }, //  DYNAMIC_SEQRAM_1CC
    { 0x41CE, 0x2, 0x0205 }, //  DYNAMIC_SEQRAM_1CE
    { 0x41D0, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1D0
    { 0x41D2, 0x2, 0x36D8 }, //  DYNAMIC_SEQRAM_1D2
    { 0x41D4, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1D4
    { 0x41D6, 0x2, 0x83F0 }, //  DYNAMIC_SEQRAM_1D6
    { 0x41D8, 0x2, 0x0087 }, //  DYNAMIC_SEQRAM_1D8
    { 0x41DA, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_1DA
    { 0x41DC, 0x2, 0x8713 }, //  DYNAMIC_SEQRAM_1DC
    { 0x41DE, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_1DE
    { 0x41E0, 0x2, 0xD802 }, //  DYNAMIC_SEQRAM_1E0
    { 0x41E2, 0x2, 0x0DE0 }, //  DYNAMIC_SEQRAM_1E2
    { 0x41E4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_1E4
    { 0x41E6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_1E6
    { 0x41E8, 0x2, 0x9F13 }, //  DYNAMIC_SEQRAM_1E8
    { 0x41EA, 0x2, 0x0041 }, //  DYNAMIC_SEQRAM_1EA
    { 0x41EC, 0x2, 0x80F3 }, //  DYNAMIC_SEQRAM_1EC
    { 0x41EE, 0x2, 0xF213 }, //  DYNAMIC_SEQRAM_1EE
    { 0x41F0, 0x2, 0x00F0 }, //  DYNAMIC_SEQRAM_1F0
    { 0x41F2, 0x2, 0x13B8 }, //  DYNAMIC_SEQRAM_1F2
    { 0x41F4, 0x2, 0xF04C }, //  DYNAMIC_SEQRAM_1F4
    { 0x41F6, 0x2, 0x9FF0 }, //  DYNAMIC_SEQRAM_1F6
    { 0x41F8, 0x2, 0x00B7 }, //  DYNAMIC_SEQRAM_1F8
    { 0x41FA, 0x2, 0xF006 }, //  DYNAMIC_SEQRAM_1FA
    { 0x41FC, 0x2, 0x0035 }, //  DYNAMIC_SEQRAM_1FC
    { 0x41FE, 0x2, 0x10AF }, //  DYNAMIC_SEQRAM_1FE
    { 0x4200, 0x2, 0x3003 }, //  DYNAMIC_SEQRAM_200
    { 0x4202, 0x2, 0x30C0 }, //  DYNAMIC_SEQRAM_202
    { 0x4204, 0x2, 0xB2F0 }, //  DYNAMIC_SEQRAM_204
    { 0x4206, 0x2, 0x01B5 }, //  DYNAMIC_SEQRAM_206
    { 0x4208, 0x2, 0xF001 }, //  DYNAMIC_SEQRAM_208
    { 0x420A, 0x2, 0x85F0 }, //  DYNAMIC_SEQRAM_20A
    { 0x420C, 0x2, 0x0292 }, //  DYNAMIC_SEQRAM_20C
    { 0x420E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_20E
    { 0x4210, 0x2, 0x9A8B }, //  DYNAMIC_SEQRAM_210
    { 0x4212, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_212
    { 0x4214, 0x2, 0x9997 }, //  DYNAMIC_SEQRAM_214
    { 0x4216, 0x2, 0xF007 }, //  DYNAMIC_SEQRAM_216
    { 0x4218, 0x2, 0xB6F0 }, //  DYNAMIC_SEQRAM_218
    { 0x421A, 0x2, 0x0020 }, //  DYNAMIC_SEQRAM_21A
    { 0x421C, 0x2, 0x5830 }, //  DYNAMIC_SEQRAM_21C
    { 0x421E, 0x2, 0xC040 }, //  DYNAMIC_SEQRAM_21E
    { 0x4220, 0x2, 0x1282 }, //  DYNAMIC_SEQRAM_220
    { 0x4222, 0x2, 0xF005 }, //  DYNAMIC_SEQRAM_222
    { 0x4224, 0x2, 0x9CF0 }, //  DYNAMIC_SEQRAM_224
    { 0x4226, 0x2, 0x01B2 }, //  DYNAMIC_SEQRAM_226
    { 0x4228, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_228
    { 0x422A, 0x2, 0xB8F0 }, //  DYNAMIC_SEQRAM_22A
    { 0x422C, 0x2, 0x0799 }, //  DYNAMIC_SEQRAM_22C
    { 0x422E, 0x2, 0xF005 }, //  DYNAMIC_SEQRAM_22E
    { 0x4230, 0x2, 0x98F0 }, //  DYNAMIC_SEQRAM_230
    { 0x4232, 0x2, 0x0296 }, //  DYNAMIC_SEQRAM_232
    { 0x4234, 0x2, 0xA2F0 }, //  DYNAMIC_SEQRAM_234
    { 0x4236, 0x2, 0x00A2 }, //  DYNAMIC_SEQRAM_236
    { 0x4238, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_238
    { 0x423A, 0x2, 0x9DF0 }, //  DYNAMIC_SEQRAM_23A
    { 0x423C, 0x2, 0x02A1 }, //  DYNAMIC_SEQRAM_23C
    { 0x423E, 0x2, 0xF01F }, //  DYNAMIC_SEQRAM_23E
    { 0x4240, 0x2, 0x1009 }, //  DYNAMIC_SEQRAM_240
    { 0x4242, 0x2, 0x2220 }, //  DYNAMIC_SEQRAM_242
    { 0x4244, 0x2, 0x0808 }, //  DYNAMIC_SEQRAM_244
    { 0x4246, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_246
    { 0x4248, 0x2, 0x3600 }, //  DYNAMIC_SEQRAM_248
    { 0x424A, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_24A
    { 0x424C, 0x2, 0x88F0 }, //  DYNAMIC_SEQRAM_24C
    { 0x424E, 0x2, 0x0788 }, //  DYNAMIC_SEQRAM_24E
    { 0x4250, 0x2, 0x3600 }, //  DYNAMIC_SEQRAM_250
    { 0x4252, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_252
    { 0x4254, 0x2, 0x83F0 }, //  DYNAMIC_SEQRAM_254
    { 0x4256, 0x2, 0x0290 }, //  DYNAMIC_SEQRAM_256
    { 0x4258, 0x2, 0xF016 }, //  DYNAMIC_SEQRAM_258
    { 0x425A, 0x2, 0x8BF0 }, //  DYNAMIC_SEQRAM_25A
    { 0x425C, 0x2, 0x11A3 }, //  DYNAMIC_SEQRAM_25C
    { 0x425E, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_25E
    { 0x4260, 0x2, 0xA3F0 }, //  DYNAMIC_SEQRAM_260
    { 0x4262, 0x2, 0x089D }, //  DYNAMIC_SEQRAM_262
    { 0x4264, 0x2, 0xF002 }, //  DYNAMIC_SEQRAM_264
    { 0x4266, 0x2, 0xA1F0 }, //  DYNAMIC_SEQRAM_266
    { 0x4268, 0x2, 0x20A1 }, //  DYNAMIC_SEQRAM_268
    { 0x426A, 0x2, 0xF006 }, //  DYNAMIC_SEQRAM_26A
    { 0x426C, 0x2, 0x4300 }, //  DYNAMIC_SEQRAM_26C
    { 0x426E, 0x2, 0xF049 }, //  DYNAMIC_SEQRAM_26E
    { 0x4270, 0x2, 0x4014 }, //  DYNAMIC_SEQRAM_270
    { 0x4272, 0x2, 0x8B8E }, //  DYNAMIC_SEQRAM_272
    { 0x4274, 0x2, 0x9DF0 }, //  DYNAMIC_SEQRAM_274
    { 0x4276, 0x2, 0x0802 }, //  DYNAMIC_SEQRAM_276
    { 0x4278, 0x2, 0x02F0 }, //  DYNAMIC_SEQRAM_278
    { 0x427A, 0x2, 0x00A6 }, //  DYNAMIC_SEQRAM_27A
    { 0x427C, 0x2, 0xF013 }, //  DYNAMIC_SEQRAM_27C
    { 0x427E, 0x2, 0xB283 }, //  DYNAMIC_SEQRAM_27E
    { 0x4280, 0x2, 0x9C36 }, //  DYNAMIC_SEQRAM_280
    { 0x4282, 0x2, 0x00F0 }, //  DYNAMIC_SEQRAM_282
    { 0x4284, 0x2, 0x0636 }, //  DYNAMIC_SEQRAM_284
    { 0x4286, 0x2, 0x009C }, //  DYNAMIC_SEQRAM_286
    { 0x4288, 0x2, 0xF008 }, //  DYNAMIC_SEQRAM_288
    { 0x428A, 0x2, 0x8BF0 }, //  DYNAMIC_SEQRAM_28A
    { 0x428C, 0x2, 0x0083 }, //  DYNAMIC_SEQRAM_28C
    { 0x428E, 0x2, 0xA0F0 }, //  DYNAMIC_SEQRAM_28E
    { 0x4290, 0x2, 0x0630 }, //  DYNAMIC_SEQRAM_290
    { 0x4292, 0x2, 0x18F0 }, //  DYNAMIC_SEQRAM_292
    { 0x4294, 0x2, 0x02A3 }, //  DYNAMIC_SEQRAM_294
    { 0x4296, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_296
    { 0x4298, 0x2, 0xA3F0 }, //  DYNAMIC_SEQRAM_298
    { 0x429A, 0x2, 0x0243 }, //  DYNAMIC_SEQRAM_29A
    { 0x429C, 0x2, 0x00F0 }, //  DYNAMIC_SEQRAM_29C
    { 0x429E, 0x2, 0x049D }, //  DYNAMIC_SEQRAM_29E
    { 0x42A0, 0x2, 0xF078 }, //  DYNAMIC_SEQRAM_2A0
    { 0x42A2, 0x2, 0x3018 }, //  DYNAMIC_SEQRAM_2A2
    { 0x42A4, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_2A4
    { 0x42A6, 0x2, 0x9D82 }, //  DYNAMIC_SEQRAM_2A6
    { 0x42A8, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_2A8
    { 0x42AA, 0x2, 0x9030 }, //  DYNAMIC_SEQRAM_2AA
    { 0x42AC, 0x2, 0xC0F0 }, //  DYNAMIC_SEQRAM_2AC
    { 0x42AE, 0x2, 0x1130 }, //  DYNAMIC_SEQRAM_2AE
    { 0x42B0, 0x2, 0xC0F0 }, //  DYNAMIC_SEQRAM_2B0
    { 0x42B2, 0x2, 0x0082 }, //  DYNAMIC_SEQRAM_2B2
    { 0x42B4, 0x2, 0xF001 }, //  DYNAMIC_SEQRAM_2B4
    { 0x42B6, 0x2, 0x1009 }, //  DYNAMIC_SEQRAM_2B6
    { 0x42B8, 0x2, 0xF02A }, //  DYNAMIC_SEQRAM_2B8
    { 0x42BA, 0x2, 0xA2F0 }, //  DYNAMIC_SEQRAM_2BA
    { 0x42BC, 0x2, 0x00A2 }, //  DYNAMIC_SEQRAM_2BC
    { 0x42BE, 0x2, 0x3018 }, //  DYNAMIC_SEQRAM_2BE
    { 0x42C0, 0x2, 0xF007 }, //  DYNAMIC_SEQRAM_2C0
    { 0x42C2, 0x2, 0x9DF0 }, //  DYNAMIC_SEQRAM_2C2
    { 0x42C4, 0x2, 0x1C8C }, //  DYNAMIC_SEQRAM_2C4
    { 0x42C6, 0x2, 0xF005 }, //  DYNAMIC_SEQRAM_2C6
    { 0x42C8, 0x2, 0x301F }, //  DYNAMIC_SEQRAM_2C8
    { 0x42CA, 0x2, 0x216D }, //  DYNAMIC_SEQRAM_2CA
    { 0x42CC, 0x2, 0x0A51 }, //  DYNAMIC_SEQRAM_2CC
    { 0x42CE, 0x2, 0x1FEA }, //  DYNAMIC_SEQRAM_2CE
    { 0x42D0, 0x2, 0x8640 }, //  DYNAMIC_SEQRAM_2D0
    { 0x42D2, 0x2, 0xE29F }, //  DYNAMIC_SEQRAM_2D2
    { 0x42D4, 0x2, 0xF009 }, //  DYNAMIC_SEQRAM_2D4
    { 0x42D6, 0x2, 0x0005 }, //  DYNAMIC_SEQRAM_2D6
    { 0x42D8, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_2D8
    { 0x42DA, 0x2, 0x30C0 }, //  DYNAMIC_SEQRAM_2DA
    { 0x42DC, 0x2, 0xF001 }, //  DYNAMIC_SEQRAM_2DC
    { 0x42DE, 0x2, 0x83F0 }, //  DYNAMIC_SEQRAM_2DE
    { 0x42E0, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_2E0
    { 0x42E2, 0x2, 0x00F0 }, //  DYNAMIC_SEQRAM_2E2
    { 0x42E4, 0x2, 0x0087 }, //  DYNAMIC_SEQRAM_2E4
    { 0x42E6, 0x2, 0xF007 }, //  DYNAMIC_SEQRAM_2E6
    { 0x42E8, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_2E8
    { 0x42EA, 0x2, 0x0036 }, //  DYNAMIC_SEQRAM_2EA
    { 0x42EC, 0x2, 0xC0F0 }, //  DYNAMIC_SEQRAM_2EC
    { 0x42EE, 0x2, 0x0000 }, //  DYNAMIC_SEQRAM_2EE
    { 0x42F0, 0x2, 0x0DF0 }, //  DYNAMIC_SEQRAM_2F0
    { 0x42F2, 0x2, 0x0000 }, //  DYNAMIC_SEQRAM_2F2
    { 0x42F4, 0x2, 0x05F0 }, //  DYNAMIC_SEQRAM_2F4
    { 0x42F6, 0x2, 0x0030 }, //  DYNAMIC_SEQRAM_2F6
    { 0x42F8, 0x2, 0xC0F0 }, //  DYNAMIC_SEQRAM_2F8
    { 0x42FA, 0x2, 0x0183 }, //  DYNAMIC_SEQRAM_2FA
    { 0x42FC, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_2FC
    { 0x42FE, 0x2, 0x3600 }, //  DYNAMIC_SEQRAM_2FE
    { 0x4300, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_300
    { 0x4302, 0x2, 0x87F0 }, //  DYNAMIC_SEQRAM_302
    { 0x4304, 0x2, 0x0787 }, //  DYNAMIC_SEQRAM_304
    { 0x4306, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_306
    { 0x4308, 0x2, 0x36C0 }, //  DYNAMIC_SEQRAM_308
    { 0x430A, 0x2, 0xF000 }, //  DYNAMIC_SEQRAM_30A
    { 0x430C, 0x2, 0x000F }, //  DYNAMIC_SEQRAM_30C
    { 0x430E, 0x2, 0xF42A }, //  DYNAMIC_SEQRAM_30E
    { 0x4310, 0x2, 0x4180 }, //  DYNAMIC_SEQRAM_310
    { 0x4312, 0x2, 0x1300 }, //  DYNAMIC_SEQRAM_312
    { 0x4314, 0x2, 0x9FF0 }, //  DYNAMIC_SEQRAM_314
    { 0x4316, 0x2, 0x00E0 }, //  DYNAMIC_SEQRAM_316
    { 0x4318, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_318
    { 0x431A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_31A
    { 0x431C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_31C
    { 0x431E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_31E
    { 0x4320, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_320
    { 0x4322, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_322
    { 0x4324, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_324
    { 0x4326, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_326
    { 0x4328, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_328
    { 0x432A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_32A
    { 0x432C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_32C
    { 0x432E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_32E
    { 0x4330, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_330
    { 0x4332, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_332
    { 0x4334, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_334
    { 0x4336, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_336
    { 0x4338, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_338
    { 0x433A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_33A
    { 0x433C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_33C
    { 0x433E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_33E
    { 0x4340, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_340
    { 0x4342, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_342
    { 0x4344, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_344
    { 0x4346, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_346
    { 0x4348, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_348
    { 0x434A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_34A
    { 0x434C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_34C
    { 0x434E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_34E
    { 0x4350, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_350
    { 0x4352, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_352
    { 0x4354, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_354
    { 0x4356, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_356
    { 0x4358, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_358
    { 0x435A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_35A
    { 0x435C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_35C
    { 0x435E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_35E
    { 0x4360, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_360
    { 0x4362, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_362
    { 0x4364, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_364
    { 0x4366, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_366
    { 0x4368, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_368
    { 0x436A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_36A
    { 0x436C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_36C
    { 0x436E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_36E
    { 0x4370, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_370
    { 0x4372, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_372
    { 0x4374, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_374
    { 0x4376, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_376
    { 0x4378, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_378
    { 0x437A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_37A
    { 0x437C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_37C
    { 0x437E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_37E
    { 0x4380, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_380
    { 0x4382, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_382
    { 0x4384, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_384
    { 0x4386, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_386
    { 0x4388, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_388
    { 0x438A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_38A
    { 0x438C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_38C
    { 0x438E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_38E
    { 0x4390, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_390
    { 0x4392, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_392
    { 0x4394, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_394
    { 0x4396, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_396
    { 0x4398, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_398
    { 0x439A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_39A
    { 0x439C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_39C
    { 0x439E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_39E
    { 0x43A0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3A0
    { 0x43A2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3A2
    { 0x43A4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3A4
    { 0x43A6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3A6
    { 0x43A8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3A8
    { 0x43AA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3AA
    { 0x43AC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3AC
    { 0x43AE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3AE
    { 0x43B0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3B0
    { 0x43B2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3B2
    { 0x43B4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3B4
    { 0x43B6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3B6
    { 0x43B8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3B8
    { 0x43BA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3BA
    { 0x43BC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3BC
    { 0x43BE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3BE
    { 0x43C0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3C0
    { 0x43C2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3C2
    { 0x43C4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3C4
    { 0x43C6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3C6
    { 0x43C8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3C8
    { 0x43CA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3CA
    { 0x43CC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3CC
    { 0x43CE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3CE
    { 0x43D0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3D0
    { 0x43D2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3D2
    { 0x43D4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3D4
    { 0x43D6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3D6
    { 0x43D8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3D8
    { 0x43DA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3DA
    { 0x43DC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3DC
    { 0x43DE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3DE
    { 0x43E0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3E0
    { 0x43E2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3E2
    { 0x43E4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3E4
    { 0x43E6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3E6
    { 0x43E8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3E8
    { 0x43EA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3EA
    { 0x43EC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3EC
    { 0x43EE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3EE
    { 0x43F0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3F0
    { 0x43F2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3F2
    { 0x43F4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3F4
    { 0x43F6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3F6
    { 0x43F8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3F8
    { 0x43FA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3FA
    { 0x43FC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3FC
    { 0x43FE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_3FE
    { 0x4400, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_400
    { 0x4402, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_402
    { 0x4404, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_404
    { 0x4406, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_406
    { 0x4408, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_408
    { 0x440A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_40A
    { 0x440C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_40C
    { 0x440E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_40E
    { 0x4410, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_410
    { 0x4412, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_412
    { 0x4414, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_414
    { 0x4416, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_416
    { 0x4418, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_418
    { 0x441A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_41A
    { 0x441C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_41C
    { 0x441E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_41E
    { 0x4420, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_420
    { 0x4422, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_422
    { 0x4424, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_424
    { 0x4426, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_426
    { 0x4428, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_428
    { 0x442A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_42A
    { 0x442C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_42C
    { 0x442E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_42E
    { 0x4430, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_430
    { 0x4432, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_432
    { 0x4434, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_434
    { 0x4436, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_436
    { 0x4438, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_438
    { 0x443A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_43A
    { 0x443C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_43C
    { 0x443E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_43E
    { 0x4440, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_440
    { 0x4442, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_442
    { 0x4444, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_444
    { 0x4446, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_446
    { 0x4448, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_448
    { 0x444A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_44A
    { 0x444C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_44C
    { 0x444E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_44E
    { 0x4450, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_450
    { 0x4452, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_452
    { 0x4454, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_454
    { 0x4456, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_456
    { 0x4458, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_458
    { 0x445A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_45A
    { 0x445C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_45C
    { 0x445E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_45E
    { 0x4460, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_460
    { 0x4462, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_462
    { 0x4464, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_464
    { 0x4466, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_466
    { 0x4468, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_468
    { 0x446A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_46A
    { 0x446C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_46C
    { 0x446E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_46E
    { 0x4470, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_470
    { 0x4472, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_472
    { 0x4474, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_474
    { 0x4476, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_476
    { 0x4478, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_478
    { 0x447A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_47A
    { 0x447C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_47C
    { 0x447E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_47E
    { 0x4480, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_480
    { 0x4482, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_482
    { 0x4484, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_484
    { 0x4486, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_486
    { 0x4488, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_488
    { 0x448A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_48A
    { 0x448C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_48C
    { 0x448E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_48E
    { 0x4490, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_490
    { 0x4492, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_492
    { 0x4494, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_494
    { 0x4496, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_496
    { 0x4498, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_498
    { 0x449A, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_49A
    { 0x449C, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_49C
    { 0x449E, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_49E
    { 0x44A0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4A0
    { 0x44A2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4A2
    { 0x44A4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4A4
    { 0x44A6, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4A6
    { 0x44A8, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4A8
    { 0x44AA, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4AA
    { 0x44AC, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4AC
    { 0x44AE, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4AE
    { 0x44B0, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4B0
    { 0x44B2, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4B2
    { 0x44B4, 0x2, 0xE0E0 }, //  DYNAMIC_SEQRAM_4B4
    { 0x5500, 0x2, 0x0000 }, //  AGAIN_LUT0
    { 0x5502, 0x2, 0x0002 }, //  AGAIN_LUT1
    { 0x5504, 0x2, 0x0006 }, //  AGAIN_LUT2
    { 0x5506, 0x2, 0x0009 }, //  AGAIN_LUT3
    { 0x5508, 0x2, 0x000F }, //  AGAIN_LUT4
    { 0x550A, 0x2, 0x0010 }, //  AGAIN_LUT5
    { 0x550C, 0x2, 0x0011 }, //  AGAIN_LUT6
    { 0x550E, 0x2, 0x0012 }, //  AGAIN_LUT7
    { 0x5510, 0x2, 0x0019 }, //  AGAIN_LUT8
    { 0x5512, 0x2, 0x0020 }, //  AGAIN_LUT9
    { 0x5514, 0x2, 0x0021 }, //  AGAIN_LUT10
    { 0x5516, 0x2, 0x0023 }, //  AGAIN_LUT11
    { 0x5518, 0x2, 0x0026 }, //  AGAIN_LUT12
    { 0x551A, 0x2, 0x002B }, //  AGAIN_LUT13
    { 0x551C, 0x2, 0x002F }, //  AGAIN_LUT14
    { 0x551E, 0x2, 0x0030 }, //  AGAIN_LUT15
    { 0x5400, 0x2, 0x0100 }, //  GT1_COARSE0
    { 0x5402, 0x2, 0x2106 }, //  GT1_COARSE1
    { 0x5404, 0x2, 0x1101 }, //  GT1_COARSE2
    { 0x5406, 0x2, 0x3106 }, //  GT1_COARSE3
    { 0x5408, 0x2, 0x7100 }, //  GT1_COARSE4
    { 0x540A, 0x2, 0x8107 }, //  GT1_COARSE5
    { 0x540C, 0x2, 0xB101 }, //  GT1_COARSE6
    { 0x540E, 0x2, 0xD101 }, //  GT1_COARSE7
    { 0x5410, 0x2, 0xF12E }, //  GT1_COARSE8
    { 0x5412, 0x2, 0xF112 }, //  GT1_COARSE9
    { 0x5414, 0x2, 0xF184 }, //  GT1_COARSE10
    { 0x5416, 0x2, 0xF224 }, //  GT1_COARSE11
    { 0x5418, 0x2, 0xF306 }, //  GT1_COARSE12
    { 0x541A, 0x2, 0xF446 }, //  GT1_COARSE13
    { 0x541C, 0x2, 0xF609 }, //  GT1_COARSE14
    { 0x541E, 0x2, 0xF887 }, //  GT1_COARSE15
    { 0x5420, 0x2, 0xFC0B }, //  GT1_COARSE16
    { 0x5422, 0x2, 0xFC0B }, //  GT1_COARSE17
    { 0x5424, 0x2, 0xFFFA }, //  GT1_DCG_ATTN_SET0
    { 0x5426, 0x2, 0x5557 }, //  GT1_DCG_ATTN_SET1
    { 0x5428, 0x2, 0x0005 }, //  GT1_DCG_ATTN_SET2
    { 0x542A, 0x2, 0xA550 }, //  GT1_ZONE_SET0
    { 0x542C, 0x2, 0xAAAA }, //  GT1_ZONE_SET1
    { 0x542E, 0x2, 0x000A }, //  GT1_ZONE_SET2
    { 0x5460, 0x2, 0x2269 }, //  ZT1_REG0_ADDR
    { 0x5462, 0x2, 0x0B87 }, //  ZT1_REG0_VALUE0
    { 0x5464, 0x2, 0x0B87 }, //  ZT1_REG0_VALUE1
    { 0x5466, 0x2, 0x0983 }, //  ZT1_REG0_VALUE2
    { 0x5498, 0x2, 0x225E }, //  ZT1_REG7_ADDR
    { 0x549A, 0x2, 0xBCAA }, //  ZT1_REG7_VALUE0
    { 0x549C, 0x2, 0xBCAA }, //  ZT1_REG7_VALUE1
    { 0x549E, 0x2, 0xBDAA }, //  ZT1_REG7_VALUE2
    { 0x3060, 0x2, 0xFF01 }, //  GAIN_TABLE_CTRL
    { 0x44BA, 0x2, 0x0050 }, //  DAC_LD_4_5
    { 0x44BC, 0x2, 0xBCAA }, //  DAC_LD_6_7
    { 0x44C0, 0x2, 0x4070 }, //  DAC_LD_10_11
    { 0x44C4, 0x2, 0x04D0 }, //  DAC_LD_14_15
    { 0x44C6, 0x2, 0x17E2 }, //  DAC_LD_16_17
    { 0x44C8, 0x2, 0xEA43 }, //  DAC_LD_18_19
    { 0x44CA, 0x2, 0x000E }, //  DAC_LD_20_21
    { 0x44CC, 0x2, 0x7777 }, //  DAC_LD_22_23
    { 0x44CE, 0x2, 0x8BA4 }, //  DAC_LD_24_25
    { 0x44D0, 0x2, 0x1735 }, //  DAC_LD_26_27
    { 0x44D2, 0x2, 0x0B87 }, //  DAC_LD_28_29
    { 0x44D4, 0x2, 0x0000 }, //  DAC_LD_30_31
    { 0x44D6, 0x2, 0xF206 }, //  DAC_LD_32_33
    { 0x44D8, 0x2, 0xAAFA }, //  DAC_LD_34_35
    { 0x44DA, 0x2, 0xE001 }, //  DAC_LD_36_37
    { 0x44DE, 0x2, 0x9BBC }, //  DAC_LD_40_41
    { 0x44E0, 0x2, 0x283C }, //  DAC_LD_42_43
    { 0x44E2, 0x2, 0x2821 }, //  DAC_LD_44_45
    { 0x44E4, 0x2, 0x8000 }, //  DAC_LD_46_47
    { 0x44E6, 0x2, 0x503F }, //  DAC_LD_48_49
    { 0x32A4, 0x2, 0x0000 }, //  CRM_CTRL
    { 0x328E, 0x2, 0x0004 }, //  ADDR_CTRL
    { 0x333C, 0x2, 0x0001 }, //  DYNAMIC_CTRL
    { 0x301A, 0x2, 0x0000 }, //  RESET_REGISTER
    { 0x3600, 0x2, 0x94DF }, //  FDOC_CTRL
    { 0x3616, 0x2, 0x0000 }, //  FDOC_CTRL2
    { 0x3700, 0x2, 0x0001 }, //  PIX_DEF_ID
    { 0x3980, 0x2, 0x0003 }, //  PIX_DEF_CORR
    { 0x36C0, 0x2, 0x0001 }, //  DIGITAL_GAIN_CTRL
    { 0x36DE, 0x2, 0x002A }, //  DATA_PEDESTAL1
    { 0x301A, 0x2, 0x0008 }, //  RESET_REGISTER
    { 0x3060, 0x2, 0x0000 }, //  GAIN_TABLE_CTRL
    { 0x3982, 0x2, 0xAC70 }, //  PDC_DYN_EDGE_THRES
    { 0x3984, 0x2, 0xFA98 }, //  PDC_DYN_LO_DEFECT_THRES
    { 0x3986, 0x2, 0xFC3F }, //  PDC_DYN_HI_DEFECT_THRES
    { 0x3988, 0x2, 0xAC70 }, //  PDC_DYN_EDGE_THRES_T2
    { 0x398A, 0x2, 0xFA98 }, //  PDC_DYN_LO_DEFECT_THRES_T2
    { 0x398C, 0x2, 0xFC3F }, //  PDC_DYN_HI_DEFECT_THRES_T2
    { 0x3980, 0x2, 0x0003 }, //  PIX_DEF_CORR
    { 0x3060, 0x2, 0xFF01 }, //  GAIN_TABLE_CTRL
    { 0x3340, 0x2, 0x1C60 }, //  OTPM_CTRL
    { 0x400, 0x2,  0x3 }, //  X,Y scaling
    { 0x404, 0x2,  0xB09 }, //  Scale_M
    { 0x0406, 0x2,  0x200 }, //  Scale_N
    { 0x3C58, 0x2,  0x10 }, //  Slice overlap
    { 0x3C40, 0x2,  0x2 }, //  Scaling Mode
    { 0x3C42, 0x2,  0x77F }, //  SCALE_SECOND_RESIDUAL_EVEN=1919
    { 0x3C44, 0x2,  0xFA }, //  SCALE_SECOND_RESIDUAL_ODD=250
    { 0x3C46, 0x2,  0xA0B }, //  scale_second_crop_e=10scale_second_crop_o=11
    { 0x3C48, 0x2,  0x62B }, //  SCALE_THIRD_RESIDUAL_EVEN=1579
    { 0x3C4A, 0x2,  0xAAF }, //  SCALE_THIRD_RESIDUAL_ODD=2735
    { 0x3C4C, 0x2,  0xA0A }, //  scale_third_crop_e=10scale_third_crop_o=10
    { 0x3C4E, 0x2,  0x4D7 }, //  SCALE_FOURTH_RESIDUAL_EVEN=1239
    { 0x3C50, 0x2,  0x95B }, //  SCALE_FOURTH_RESIDUAL_ODD=2395
    { 0x3C52, 0x2,  0xA0A }, //  scale_fourth_crop_e=10scale_fourth_crop_o=10
    { 0x3C60, 0x2,  0xA0 }, //  SCALE_FIRST_PXL_NUM=160
    { 0x3C62, 0x2,  0x98 }, //  SCALE_SECOND_PXL_NUM=152
    { 0x3C64, 0x2,  0x98 }, //  SCALE_THIRD_PXL_NUM=152
    { 0x3C66, 0x2,  0x90 }, //  SCALE_FOURTH_PXL_NUM=144
    { 0x3EC8, 0x2,  0x0 }, //  X_OUTPU_OFFSET:0
    { 0x3ECA, 0x2,  0x0 }, //  Y_OUTPU_OFFSET:0
    { 0x034C, 0x2,  0x260 }, //  X_OUTPU_SIZE:608
    { 0x034E, 0x2,  0x1C0 }, //  Y_OUTPU_SIZE:448
    { 0x3C6A, 0x2,  0x520 }, //  X_OUTPU_OFFSET2:1312
    { 0x3C70, 0x2,  0x3D0 }, //  Y_OUTPU_OFFSET2:976
    { 0x3C6C, 0x2,  0x260 }, //  X_OUTPU_SIZE2:608
    { 0x3C72, 0x2,  0x1C0 }, //  Y_OUTPU_SIZE2:448
    { 0x3C7A, 0x2,  0x0 }, //  reorder_ctrl
    { 0x3C74, 0x2,  0x3 }, //  Sync_regen
    { 0x3C76, 0x2,  0x20 }, //  Sync_regen_blank
    { 0x3F18, 0x2, 0x7B70 }, //  MIPI_JPEG_PN9_DATA_TYPE

    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE

    { 0x0100, 0x1,  0x01 }, //  MODE_SELECT
    { 0x44D6, 0x2,  0xB206 }, //  DAC_LD_32_33
};


#if 1
const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_1120x840_544x408_table[] =
{
    {0x0103, 0x3201, 0x01}, // SOFTWARE_RESET

    {0x0304, 2, 0x0002},  // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 0x0056},  // VT_PLL_MULTIPLIER
    {0x0300, 2, 0x0006},  // VT_PIX_CLK_DIV
    {0x0302, 2, 0x0001},  // VT_SYS_CLK_DIV
    {0x030C, 2, 0x0007},  // OP_PRE_PLL_CLK_DIV
    {0x030E, 2, 0x01a0}, // {0x030E, 2, 0x0247},  // OP_PLL_MULTIPLIER
    {0x0308, 2, 0x0005},  // OP_PIX_CLK_DIV
    {0x030A, 2, 0x0002},  // OP_SYS_CLK_DIV
    {0x0344, 2, 0x0378},  // X_ADDR_START
    {0x0348, 2, 0x1097},  // X_ADDR_END
    {0x0346, 2, 0x02A8},  // Y_ADDR_START
    {0x034A, 2, 0x0C67},  // Y_ADDR_END
    {0x034C, 2, 0x0D20},  // X_OUTPUT_SIZE
    {0x034E, 2, 0x09C0},  // Y_OUTPUT_SIZE
    {0x0380, 2, 0x0001},  // X_EVEN_INC
    {0x0382, 2, 0x0001},  // X_ODD_INC
    {0x0384, 2, 0x0001},  // Y_EVEN_INC
    {0x0386, 2, 0x0001},  // Y_ODD_INC

    {0x0900, 1, 0x00},  // BINNING_MODE
    {0x0901, 1, 0x11},  // BINNING_TYPE

    {0x0342, 2, 0x1710},  // LINE_LENGTH_PCK
    {0x0340, 2, 0x09CE},  // FRAME_LENGTH_LINES
    {0x0202, 2, 0x0870},  // COARSE_INTEGRATION_TIME
    {0x0112, 2, 0x0A0A},  // CSI_DATA_FORMAT
    {0x0114, 1, 0x03},  // CSI_LANE_MODE
#if 1
    {0x0800, 1, 0x0C}, // TCLK_POST
    {0x0801, 1, 0x06}, // THS_PREPARE
    {0x0802, 1, 0x0B}, // THS_ZERO_MIN
    {0x0803, 1, 0x08}, // THS_TRAIL
    {0x0804, 1, 0x0A}, // TCLK_TRAIL_MIN
    {0x0805, 1, 0x07}, // TCLK_PREPARE
    {0x0806, 1, 0x21}, // TCLK_ZERO
    {0x0807, 1, 0x07}, // TLPX
    {0x082A, 1, 0x0E}, // TWAKEUP
    {0x082B, 1, 0x0B}, // TINIT
    {0x082C, 1, 0x0C}, // THS_EXIT
    {0x3F06, 2, 0x00C0},  // MIPI_TIMING_2
    {0x3F0A, 2, 0x2000}, //{0x3F0A, 2, 0xA000},  // MIPI_TIMING_4
    {0x3F0C, 2, 0x0010},  // MIPI_TIMING_5
    {0x3F20, 2, 0x8080},  // MIPI_PHY_TRIM_MSB
    {0x3F1E, 2, 0x0004},  // MIPI_PHY_TRIM_LSB
#else
    {0x0800, 1, 0x12},  // TCLK_POST
    {0x0801, 1, 0x0A},  // THS_PREPARE
    {0x0802, 1, 0x14},  // THS_ZERO_MIN
    {0x0803, 1, 0x0E},  // THS_TRAIL
    {0x0804, 1, 0x11},  // TCLK_TRAIL_MIN
    {0x0805, 1, 0x09},  // TCLK_PREPARE
    {0x0806, 1, 0x3C},  // TCLK_ZERO
    {0x0807, 1, 0x0B},  // TLPX
    {0x082A, 1, 0x19},  // TWAKEUP
    {0x082B, 1, 0x14},  // TINIT
    {0x082C, 1, 0x15},  // THS_EXIT
    {0x3F06, 2, 0x00C0},  // MIPI_TIMING_2
    {0x3F0A, 2, 0x0000}, //{0x3F0A, 2, 0xA000},  // MIPI_TIMING_4
    {0x3F0C, 2, 0x0010},  // MIPI_TIMING_5
    {0x3F20, 2, 0x8080},  // MIPI_PHY_TRIM_MSB
    {0x3F1E, 2, 0x0004},  // MIPI_PHY_TRIM_LSB
#endif
    {0x4000, 2, 0x0114},  // DYNAMIC_SEQRAM_00
    {0x4002, 2, 0x1A25},  // DYNAMIC_SEQRAM_02
    {0x4004, 2, 0x3DFF},  // DYNAMIC_SEQRAM_04
    {0x4006, 2, 0xFFFF},  // DYNAMIC_SEQRAM_06
    {0x4008, 2, 0x0A35},  // DYNAMIC_SEQRAM_08
    {0x400A, 2, 0x10EF},  // DYNAMIC_SEQRAM_0A
    {0x400C, 2, 0x3003},  // DYNAMIC_SEQRAM_0C
    {0x400E, 2, 0x30D8},  // DYNAMIC_SEQRAM_0E
    {0x4010, 2, 0xF003},  // DYNAMIC_SEQRAM_10
    {0x4012, 2, 0xB5F0},  // DYNAMIC_SEQRAM_12
    {0x4014, 2, 0x0085},  // DYNAMIC_SEQRAM_14
    {0x4016, 2, 0xF004},  // DYNAMIC_SEQRAM_16
    {0x4018, 2, 0x9A89},  // DYNAMIC_SEQRAM_18
    {0x401A, 2, 0xF000},  // DYNAMIC_SEQRAM_1A
    {0x401C, 2, 0x9997},  // DYNAMIC_SEQRAM_1C
    {0x401E, 2, 0xF000},  // DYNAMIC_SEQRAM_1E
    {0x4020, 2, 0x30C0},  // DYNAMIC_SEQRAM_20
    {0x4022, 2, 0xF000},  // DYNAMIC_SEQRAM_22
    {0x4024, 2, 0x82F0},  // DYNAMIC_SEQRAM_24
    {0x4026, 2, 0x0030},  // DYNAMIC_SEQRAM_26
    {0x4028, 2, 0x18F0},  // DYNAMIC_SEQRAM_28
    {0x402A, 2, 0x0320},  // DYNAMIC_SEQRAM_2A
    {0x402C, 2, 0x58F0},  // DYNAMIC_SEQRAM_2C
    {0x402E, 2, 0x089C},  // DYNAMIC_SEQRAM_2E
    {0x4030, 2, 0xF010},  // DYNAMIC_SEQRAM_30
    {0x4032, 2, 0x99B6},  // DYNAMIC_SEQRAM_32
    {0x4034, 2, 0xF003},  // DYNAMIC_SEQRAM_34
    {0x4036, 2, 0xB498},  // DYNAMIC_SEQRAM_36
    {0x4038, 2, 0xA096},  // DYNAMIC_SEQRAM_38
    {0x403A, 2, 0xF000},  // DYNAMIC_SEQRAM_3A
    {0x403C, 2, 0xA2F0},  // DYNAMIC_SEQRAM_3C
    {0x403E, 2, 0x00A2},  // DYNAMIC_SEQRAM_3E
    {0x4040, 2, 0xF008},  // DYNAMIC_SEQRAM_40
    {0x4042, 2, 0x9DF0},  // DYNAMIC_SEQRAM_42
    {0x4044, 2, 0x209D},  // DYNAMIC_SEQRAM_44
    {0x4046, 2, 0x8C08},  // DYNAMIC_SEQRAM_46
    {0x4048, 2, 0x08F0},  // DYNAMIC_SEQRAM_48
    {0x404A, 2, 0x0036},  // DYNAMIC_SEQRAM_4A
    {0x404C, 2, 0x008F},  // DYNAMIC_SEQRAM_4C
    {0x404E, 2, 0xF000},  // DYNAMIC_SEQRAM_4E
    {0x4050, 2, 0x88F0},  // DYNAMIC_SEQRAM_50
    {0x4052, 2, 0x0488},  // DYNAMIC_SEQRAM_52
    {0x4054, 2, 0xF000},  // DYNAMIC_SEQRAM_54
    {0x4056, 2, 0x3600},  // DYNAMIC_SEQRAM_56
    {0x4058, 2, 0xF000},  // DYNAMIC_SEQRAM_58
    {0x405A, 2, 0x83F0},  // DYNAMIC_SEQRAM_5A
    {0x405C, 2, 0x0290},  // DYNAMIC_SEQRAM_5C
    {0x405E, 2, 0xF000},  // DYNAMIC_SEQRAM_5E
    {0x4060, 2, 0x8BF0},  // DYNAMIC_SEQRAM_60
    {0x4062, 2, 0x2EA3},  // DYNAMIC_SEQRAM_62
    {0x4064, 2, 0xF000},  // DYNAMIC_SEQRAM_64
    {0x4066, 2, 0xA3F0},  // DYNAMIC_SEQRAM_66
    {0x4068, 2, 0x089D},  // DYNAMIC_SEQRAM_68
    {0x406A, 2, 0xF075},  // DYNAMIC_SEQRAM_6A
    {0x406C, 2, 0x3003},  // DYNAMIC_SEQRAM_6C
    {0x406E, 2, 0x4070},  // DYNAMIC_SEQRAM_6E
    {0x4070, 2, 0x216D},  // DYNAMIC_SEQRAM_70
    {0x4072, 2, 0x1CF6},  // DYNAMIC_SEQRAM_72
    {0x4074, 2, 0x8B00},  // DYNAMIC_SEQRAM_74
    {0x4076, 2, 0x5186},  // DYNAMIC_SEQRAM_76
    {0x4078, 2, 0x1300},  // DYNAMIC_SEQRAM_78
    {0x407A, 2, 0x0205},  // DYNAMIC_SEQRAM_7A
    {0x407C, 2, 0x36D8},  // DYNAMIC_SEQRAM_7C
    {0x407E, 2, 0xF002},  // DYNAMIC_SEQRAM_7E
    {0x4080, 2, 0x8387},  // DYNAMIC_SEQRAM_80
    {0x4082, 2, 0xF006},  // DYNAMIC_SEQRAM_82
    {0x4084, 2, 0x8702},  // DYNAMIC_SEQRAM_84
    {0x4086, 2, 0x0D02},  // DYNAMIC_SEQRAM_86
    {0x4088, 2, 0x05F0},  // DYNAMIC_SEQRAM_88
    {0x408A, 2, 0x0383},  // DYNAMIC_SEQRAM_8A
    {0x408C, 2, 0xF001},  // DYNAMIC_SEQRAM_8C
    {0x408E, 2, 0x87F0},  // DYNAMIC_SEQRAM_8E
    {0x4090, 2, 0x0213},  // DYNAMIC_SEQRAM_90
    {0x4092, 2, 0x0036},  // DYNAMIC_SEQRAM_92
    {0x4094, 2, 0xD887},  // DYNAMIC_SEQRAM_94
    {0x4096, 2, 0x020D},  // DYNAMIC_SEQRAM_96
    {0x4098, 2, 0xE0E0},  // DYNAMIC_SEQRAM_98
    {0x409A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_9A
    {0x409C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_9C
    {0x409E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_9E
    {0x40A0, 2, 0xF000},  // DYNAMIC_SEQRAM_A0
    {0x40A2, 2, 0x0401},  // DYNAMIC_SEQRAM_A2
    {0x40A4, 2, 0xF008},  // DYNAMIC_SEQRAM_A4
    {0x40A6, 2, 0x82F0},  // DYNAMIC_SEQRAM_A6
    {0x40A8, 2, 0x0883},  // DYNAMIC_SEQRAM_A8
    {0x40AA, 2, 0xF009},  // DYNAMIC_SEQRAM_AA
    {0x40AC, 2, 0x85F0},  // DYNAMIC_SEQRAM_AC
    {0x40AE, 2, 0x2985},  // DYNAMIC_SEQRAM_AE
    {0x40B0, 2, 0x87F0},  // DYNAMIC_SEQRAM_B0
    {0x40B2, 2, 0x2A87},  // DYNAMIC_SEQRAM_B2
    {0x40B4, 2, 0xF63E},  // DYNAMIC_SEQRAM_B4
    {0x40B6, 2, 0x88F0},  // DYNAMIC_SEQRAM_B6
    {0x40B8, 2, 0x0801},  // DYNAMIC_SEQRAM_B8
    {0x40BA, 2, 0x40F0},  // DYNAMIC_SEQRAM_BA
    {0x40BC, 2, 0x0800},  // DYNAMIC_SEQRAM_BC
    {0x40BE, 2, 0x48F0},  // DYNAMIC_SEQRAM_BE
    {0x40C0, 2, 0x0882},  // DYNAMIC_SEQRAM_C0
    {0x40C2, 2, 0xF008},  // DYNAMIC_SEQRAM_C2
    {0x40C4, 2, 0x0401},  // DYNAMIC_SEQRAM_C4
    {0x40C6, 2, 0xF008},  // DYNAMIC_SEQRAM_C6
    {0x40C8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_C8
    {0x40CA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_CA
    {0x40CC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_CC
    {0x40CE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_CE
    {0x40D0, 2, 0xF000},  // DYNAMIC_SEQRAM_D0
    {0x40D2, 2, 0x0401},  // DYNAMIC_SEQRAM_D2
    {0x40D4, 2, 0xF015},  // DYNAMIC_SEQRAM_D4
    {0x40D6, 2, 0x002C},  // DYNAMIC_SEQRAM_D6
    {0x40D8, 2, 0xF00E},  // DYNAMIC_SEQRAM_D8
    {0x40DA, 2, 0x85F0},  // DYNAMIC_SEQRAM_DA
    {0x40DC, 2, 0x0687},  // DYNAMIC_SEQRAM_DC
    {0x40DE, 2, 0xF002},  // DYNAMIC_SEQRAM_DE
    {0x40E0, 2, 0x87F0},  // DYNAMIC_SEQRAM_E0
    {0x40E2, 2, 0x61E8},  // DYNAMIC_SEQRAM_E2
    {0x40E4, 2, 0x3900},  // DYNAMIC_SEQRAM_E4
    {0x40E6, 2, 0xF005},  // DYNAMIC_SEQRAM_E6
    {0x40E8, 2, 0x3480},  // DYNAMIC_SEQRAM_E8
    {0x40EA, 2, 0xF000},  // DYNAMIC_SEQRAM_EA
    {0x40EC, 2, 0x3240},  // DYNAMIC_SEQRAM_EC
    {0x40EE, 2, 0xF000},  // DYNAMIC_SEQRAM_EE
    {0x40F0, 2, 0x3900},  // DYNAMIC_SEQRAM_F0
    {0x40F2, 2, 0xF00E},  // DYNAMIC_SEQRAM_F2
    {0x40F4, 2, 0x3900},  // DYNAMIC_SEQRAM_F4
    {0x40F6, 2, 0xF000},  // DYNAMIC_SEQRAM_F6
    {0x40F8, 2, 0x3240},  // DYNAMIC_SEQRAM_F8
    {0x40FA, 2, 0xF000},  // DYNAMIC_SEQRAM_FA
    {0x40FC, 2, 0x3480},  // DYNAMIC_SEQRAM_FC
    {0x40FE, 2, 0xF005},  // DYNAMIC_SEQRAM_FE
    {0x4100, 2, 0xC0E6},  // DYNAMIC_SEQRAM_100
    {0x4102, 2, 0xF004},  // DYNAMIC_SEQRAM_102
    {0x4104, 2, 0x3900},  // DYNAMIC_SEQRAM_104
    {0x4106, 2, 0xF003},  // DYNAMIC_SEQRAM_106
    {0x4108, 2, 0xB0F0},  // DYNAMIC_SEQRAM_108
    {0x410A, 2, 0x0083},  // DYNAMIC_SEQRAM_10A
    {0x410C, 2, 0xF000},  // DYNAMIC_SEQRAM_10C
    {0x410E, 2, 0x86F0},  // DYNAMIC_SEQRAM_10E
    {0x4110, 2, 0x0086},  // DYNAMIC_SEQRAM_110
    {0x4112, 2, 0xF089},  // DYNAMIC_SEQRAM_112
    {0x4114, 2, 0xB0F0},  // DYNAMIC_SEQRAM_114
    {0x4116, 2, 0x00E9},  // DYNAMIC_SEQRAM_116
    {0x4118, 2, 0xF000},  // DYNAMIC_SEQRAM_118
    {0x411A, 2, 0x8AF0},  // DYNAMIC_SEQRAM_11A
    {0x411C, 2, 0x0000},  // DYNAMIC_SEQRAM_11C
    {0x411E, 2, 0x05F0},  // DYNAMIC_SEQRAM_11E
    {0x4120, 2, 0x00E0},  // DYNAMIC_SEQRAM_120
    {0x4122, 2, 0xE0E0},  // DYNAMIC_SEQRAM_122
    {0x4124, 2, 0xE0E0},  // DYNAMIC_SEQRAM_124
    {0x4126, 2, 0xE0E0},  // DYNAMIC_SEQRAM_126
    {0x4128, 2, 0x0A35},  // DYNAMIC_SEQRAM_128
    {0x412A, 2, 0x10EF},  // DYNAMIC_SEQRAM_12A
    {0x412C, 2, 0x3003},  // DYNAMIC_SEQRAM_12C
    {0x412E, 2, 0x30D8},  // DYNAMIC_SEQRAM_12E
    {0x4130, 2, 0xF005},  // DYNAMIC_SEQRAM_130
    {0x4132, 2, 0x85F0},  // DYNAMIC_SEQRAM_132
    {0x4134, 2, 0x049A},  // DYNAMIC_SEQRAM_134
    {0x4136, 2, 0x89F0},  // DYNAMIC_SEQRAM_136
    {0x4138, 2, 0x0099},  // DYNAMIC_SEQRAM_138
    {0x413A, 2, 0x97F0},  // DYNAMIC_SEQRAM_13A
    {0x413C, 2, 0x0030},  // DYNAMIC_SEQRAM_13C
    {0x413E, 2, 0xC0F0},  // DYNAMIC_SEQRAM_13E
    {0x4140, 2, 0x0082},  // DYNAMIC_SEQRAM_140
    {0x4142, 2, 0xF000},  // DYNAMIC_SEQRAM_142
    {0x4144, 2, 0x3018},  // DYNAMIC_SEQRAM_144
    {0x4146, 2, 0xF002},  // DYNAMIC_SEQRAM_146
    {0x4148, 2, 0xB520},  // DYNAMIC_SEQRAM_148
    {0x414A, 2, 0x58F0},  // DYNAMIC_SEQRAM_14A
    {0x414C, 2, 0x089C},  // DYNAMIC_SEQRAM_14C
    {0x414E, 2, 0xF010},  // DYNAMIC_SEQRAM_14E
    {0x4150, 2, 0x99B6},  // DYNAMIC_SEQRAM_150
    {0x4152, 2, 0xF003},  // DYNAMIC_SEQRAM_152
    {0x4154, 2, 0xB498},  // DYNAMIC_SEQRAM_154
    {0x4156, 2, 0xA096},  // DYNAMIC_SEQRAM_156
    {0x4158, 2, 0xF000},  // DYNAMIC_SEQRAM_158
    {0x415A, 2, 0xA2F0},  // DYNAMIC_SEQRAM_15A
    {0x415C, 2, 0x00A2},  // DYNAMIC_SEQRAM_15C
    {0x415E, 2, 0xF008},  // DYNAMIC_SEQRAM_15E
    {0x4160, 2, 0x9DF0},  // DYNAMIC_SEQRAM_160
    {0x4162, 2, 0x209D},  // DYNAMIC_SEQRAM_162
    {0x4164, 2, 0x8C08},  // DYNAMIC_SEQRAM_164
    {0x4166, 2, 0x08F0},  // DYNAMIC_SEQRAM_166
    {0x4168, 2, 0x0036},  // DYNAMIC_SEQRAM_168
    {0x416A, 2, 0x008F},  // DYNAMIC_SEQRAM_16A
    {0x416C, 2, 0x88F0},  // DYNAMIC_SEQRAM_16C
    {0x416E, 2, 0x0188},  // DYNAMIC_SEQRAM_16E
    {0x4170, 2, 0x3600},  // DYNAMIC_SEQRAM_170
    {0x4172, 2, 0xF000},  // DYNAMIC_SEQRAM_172
    {0x4174, 2, 0x83F0},  // DYNAMIC_SEQRAM_174
    {0x4176, 2, 0x0290},  // DYNAMIC_SEQRAM_176
    {0x4178, 2, 0xF001},  // DYNAMIC_SEQRAM_178
    {0x417A, 2, 0x8BF0},  // DYNAMIC_SEQRAM_17A
    {0x417C, 2, 0x2DA3},  // DYNAMIC_SEQRAM_17C
    {0x417E, 2, 0xF000},  // DYNAMIC_SEQRAM_17E
    {0x4180, 2, 0xA3F0},  // DYNAMIC_SEQRAM_180
    {0x4182, 2, 0x089D},  // DYNAMIC_SEQRAM_182
    {0x4184, 2, 0xF06D},  // DYNAMIC_SEQRAM_184
    {0x4186, 2, 0x4070},  // DYNAMIC_SEQRAM_186
    {0x4188, 2, 0x3003},  // DYNAMIC_SEQRAM_188
    {0x418A, 2, 0x214D},  // DYNAMIC_SEQRAM_18A
    {0x418C, 2, 0x1FF6},  // DYNAMIC_SEQRAM_18C
    {0x418E, 2, 0x0851},  // DYNAMIC_SEQRAM_18E
    {0x4190, 2, 0x0245},  // DYNAMIC_SEQRAM_190
    {0x4192, 2, 0x9D36},  // DYNAMIC_SEQRAM_192
    {0x4194, 2, 0xD8F0},  // DYNAMIC_SEQRAM_194
    {0x4196, 2, 0x0083},  // DYNAMIC_SEQRAM_196
    {0x4198, 2, 0xF000},  // DYNAMIC_SEQRAM_198
    {0x419A, 2, 0x87F0},  // DYNAMIC_SEQRAM_19A
    {0x419C, 2, 0x0087},  // DYNAMIC_SEQRAM_19C
    {0x419E, 2, 0xF000},  // DYNAMIC_SEQRAM_19E
    {0x41A0, 2, 0x36D8},  // DYNAMIC_SEQRAM_1A0
    {0x41A2, 2, 0x020D},  // DYNAMIC_SEQRAM_1A2
    {0x41A4, 2, 0x0205},  // DYNAMIC_SEQRAM_1A4
    {0x41A6, 2, 0xF000},  // DYNAMIC_SEQRAM_1A6
    {0x41A8, 2, 0x36D8},  // DYNAMIC_SEQRAM_1A8
    {0x41AA, 2, 0xF000},  // DYNAMIC_SEQRAM_1AA
    {0x41AC, 2, 0x83F0},  // DYNAMIC_SEQRAM_1AC
    {0x41AE, 2, 0x0087},  // DYNAMIC_SEQRAM_1AE
    {0x41B0, 2, 0xF000},  // DYNAMIC_SEQRAM_1B0
    {0x41B2, 2, 0x87F0},  // DYNAMIC_SEQRAM_1B2
    {0x41B4, 2, 0x0036},  // DYNAMIC_SEQRAM_1B4
    {0x41B6, 2, 0xD802},  // DYNAMIC_SEQRAM_1B6
    {0x41B8, 2, 0x0D02},  // DYNAMIC_SEQRAM_1B8
    {0x41BA, 2, 0x05F0},  // DYNAMIC_SEQRAM_1BA
    {0x41BC, 2, 0x0036},  // DYNAMIC_SEQRAM_1BC
    {0x41BE, 2, 0xD8F0},  // DYNAMIC_SEQRAM_1BE
    {0x41C0, 2, 0x0083},  // DYNAMIC_SEQRAM_1C0
    {0x41C2, 2, 0xF000},  // DYNAMIC_SEQRAM_1C2
    {0x41C4, 2, 0x87F0},  // DYNAMIC_SEQRAM_1C4
    {0x41C6, 2, 0x0087},  // DYNAMIC_SEQRAM_1C6
    {0x41C8, 2, 0xF000},  // DYNAMIC_SEQRAM_1C8
    {0x41CA, 2, 0x36D8},  // DYNAMIC_SEQRAM_1CA
    {0x41CC, 2, 0x020D},  // DYNAMIC_SEQRAM_1CC
    {0x41CE, 2, 0x0205},  // DYNAMIC_SEQRAM_1CE
    {0x41D0, 2, 0xF000},  // DYNAMIC_SEQRAM_1D0
    {0x41D2, 2, 0x36D8},  // DYNAMIC_SEQRAM_1D2
    {0x41D4, 2, 0xF000},  // DYNAMIC_SEQRAM_1D4
    {0x41D6, 2, 0x83F0},  // DYNAMIC_SEQRAM_1D6
    {0x41D8, 2, 0x0087},  // DYNAMIC_SEQRAM_1D8
    {0x41DA, 2, 0xF000},  // DYNAMIC_SEQRAM_1DA
    {0x41DC, 2, 0x8713},  // DYNAMIC_SEQRAM_1DC
    {0x41DE, 2, 0x0036},  // DYNAMIC_SEQRAM_1DE
    {0x41E0, 2, 0xD802},  // DYNAMIC_SEQRAM_1E0
    {0x41E2, 2, 0x0DE0},  // DYNAMIC_SEQRAM_1E2
    {0x41E4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_1E4
    {0x41E6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_1E6
    {0x41E8, 2, 0x0035},  // DYNAMIC_SEQRAM_1E8
    {0x41EA, 2, 0x10AF},  // DYNAMIC_SEQRAM_1EA
    {0x41EC, 2, 0x3003},  // DYNAMIC_SEQRAM_1EC
    {0x41EE, 2, 0x30C0},  // DYNAMIC_SEQRAM_1EE
    {0x41F0, 2, 0xB2F0},  // DYNAMIC_SEQRAM_1F0
    {0x41F2, 2, 0x01B5},  // DYNAMIC_SEQRAM_1F2
    {0x41F4, 2, 0xF001},  // DYNAMIC_SEQRAM_1F4
    {0x41F6, 2, 0x85F0},  // DYNAMIC_SEQRAM_1F6
    {0x41F8, 2, 0x0292},  // DYNAMIC_SEQRAM_1F8
    {0x41FA, 2, 0xF000},  // DYNAMIC_SEQRAM_1FA
    {0x41FC, 2, 0x9A8B},  // DYNAMIC_SEQRAM_1FC
    {0x41FE, 2, 0xF000},  // DYNAMIC_SEQRAM_1FE
    {0x4200, 2, 0x9997},  // DYNAMIC_SEQRAM_200
    {0x4202, 2, 0xF007},  // DYNAMIC_SEQRAM_202
    {0x4204, 2, 0xB6F0},  // DYNAMIC_SEQRAM_204
    {0x4206, 2, 0x0020},  // DYNAMIC_SEQRAM_206
    {0x4208, 2, 0x5830},  // DYNAMIC_SEQRAM_208
    {0x420A, 2, 0xC040},  // DYNAMIC_SEQRAM_20A
    {0x420C, 2, 0x1282},  // DYNAMIC_SEQRAM_20C
    {0x420E, 2, 0xF005},  // DYNAMIC_SEQRAM_20E
    {0x4210, 2, 0x9CF0},  // DYNAMIC_SEQRAM_210
    {0x4212, 2, 0x01B2},  // DYNAMIC_SEQRAM_212
    {0x4214, 2, 0xF008},  // DYNAMIC_SEQRAM_214
    {0x4216, 2, 0xB8F0},  // DYNAMIC_SEQRAM_216
    {0x4218, 2, 0x0799},  // DYNAMIC_SEQRAM_218
    {0x421A, 2, 0xF005},  // DYNAMIC_SEQRAM_21A
    {0x421C, 2, 0x98F0},  // DYNAMIC_SEQRAM_21C
    {0x421E, 2, 0x0296},  // DYNAMIC_SEQRAM_21E
    {0x4220, 2, 0xA2F0},  // DYNAMIC_SEQRAM_220
    {0x4222, 2, 0x00A2},  // DYNAMIC_SEQRAM_222
    {0x4224, 2, 0xF008},  // DYNAMIC_SEQRAM_224
    {0x4226, 2, 0x9DF0},  // DYNAMIC_SEQRAM_226
    {0x4228, 2, 0x02A1},  // DYNAMIC_SEQRAM_228
    {0x422A, 2, 0xF01F},  // DYNAMIC_SEQRAM_22A
    {0x422C, 2, 0x1009},  // DYNAMIC_SEQRAM_22C
    {0x422E, 2, 0x2220},  // DYNAMIC_SEQRAM_22E
    {0x4230, 2, 0x0808},  // DYNAMIC_SEQRAM_230
    {0x4232, 2, 0xF000},  // DYNAMIC_SEQRAM_232
    {0x4234, 2, 0x3600},  // DYNAMIC_SEQRAM_234
    {0x4236, 2, 0xF000},  // DYNAMIC_SEQRAM_236
    {0x4238, 2, 0x88F0},  // DYNAMIC_SEQRAM_238
    {0x423A, 2, 0x0788},  // DYNAMIC_SEQRAM_23A
    {0x423C, 2, 0x3600},  // DYNAMIC_SEQRAM_23C
    {0x423E, 2, 0xF000},  // DYNAMIC_SEQRAM_23E
    {0x4240, 2, 0x83F0},  // DYNAMIC_SEQRAM_240
    {0x4242, 2, 0x0290},  // DYNAMIC_SEQRAM_242
    {0x4244, 2, 0xF016},  // DYNAMIC_SEQRAM_244
    {0x4246, 2, 0x8BF0},  // DYNAMIC_SEQRAM_246
    {0x4248, 2, 0x11A3},  // DYNAMIC_SEQRAM_248
    {0x424A, 2, 0xF000},  // DYNAMIC_SEQRAM_24A
    {0x424C, 2, 0xA3F0},  // DYNAMIC_SEQRAM_24C
    {0x424E, 2, 0x089D},  // DYNAMIC_SEQRAM_24E
    {0x4250, 2, 0xF002},  // DYNAMIC_SEQRAM_250
    {0x4252, 2, 0xA1F0},  // DYNAMIC_SEQRAM_252
    {0x4254, 2, 0x20A1},  // DYNAMIC_SEQRAM_254
    {0x4256, 2, 0xF006},  // DYNAMIC_SEQRAM_256
    {0x4258, 2, 0x4300},  // DYNAMIC_SEQRAM_258
    {0x425A, 2, 0xF04A},  // DYNAMIC_SEQRAM_25A
    {0x425C, 2, 0x8B8E},  // DYNAMIC_SEQRAM_25C
    {0x425E, 2, 0x9DF0},  // DYNAMIC_SEQRAM_25E
    {0x4260, 2, 0x1640},  // DYNAMIC_SEQRAM_260
    {0x4262, 2, 0x14F0},  // DYNAMIC_SEQRAM_262
    {0x4264, 2, 0x0B02},  // DYNAMIC_SEQRAM_264
    {0x4266, 2, 0x02F0},  // DYNAMIC_SEQRAM_266
    {0x4268, 2, 0x00A6},  // DYNAMIC_SEQRAM_268
    {0x426A, 2, 0xF013},  // DYNAMIC_SEQRAM_26A
    {0x426C, 2, 0xB283},  // DYNAMIC_SEQRAM_26C
    {0x426E, 2, 0x9C36},  // DYNAMIC_SEQRAM_26E
    {0x4270, 2, 0x00F0},  // DYNAMIC_SEQRAM_270
    {0x4272, 2, 0x0636},  // DYNAMIC_SEQRAM_272
    {0x4274, 2, 0x009C},  // DYNAMIC_SEQRAM_274
    {0x4276, 2, 0xF008},  // DYNAMIC_SEQRAM_276
    {0x4278, 2, 0x8BF0},  // DYNAMIC_SEQRAM_278
    {0x427A, 2, 0x0083},  // DYNAMIC_SEQRAM_27A
    {0x427C, 2, 0xA0F0},  // DYNAMIC_SEQRAM_27C
    {0x427E, 2, 0x0630},  // DYNAMIC_SEQRAM_27E
    {0x4280, 2, 0x18F0},  // DYNAMIC_SEQRAM_280
    {0x4282, 2, 0x02A3},  // DYNAMIC_SEQRAM_282
    {0x4284, 2, 0xF000},  // DYNAMIC_SEQRAM_284
    {0x4286, 2, 0xA3F0},  // DYNAMIC_SEQRAM_286
    {0x4288, 2, 0x0243},  // DYNAMIC_SEQRAM_288
    {0x428A, 2, 0x00F0},  // DYNAMIC_SEQRAM_28A
    {0x428C, 2, 0x049D},  // DYNAMIC_SEQRAM_28C
    {0x428E, 2, 0xF078},  // DYNAMIC_SEQRAM_28E
    {0x4290, 2, 0x3018},  // DYNAMIC_SEQRAM_290
    {0x4292, 2, 0xF000},  // DYNAMIC_SEQRAM_292
    {0x4294, 2, 0x9D82},  // DYNAMIC_SEQRAM_294
    {0x4296, 2, 0xF000},  // DYNAMIC_SEQRAM_296
    {0x4298, 2, 0x9030},  // DYNAMIC_SEQRAM_298
    {0x429A, 2, 0xC0F0},  // DYNAMIC_SEQRAM_29A
    {0x429C, 2, 0x1130},  // DYNAMIC_SEQRAM_29C
    {0x429E, 2, 0xC0F0},  // DYNAMIC_SEQRAM_29E
    {0x42A0, 2, 0x0082},  // DYNAMIC_SEQRAM_2A0
    {0x42A2, 2, 0xF001},  // DYNAMIC_SEQRAM_2A2
    {0x42A4, 2, 0x1009},  // DYNAMIC_SEQRAM_2A4
    {0x42A6, 2, 0xF02A},  // DYNAMIC_SEQRAM_2A6
    {0x42A8, 2, 0xA2F0},  // DYNAMIC_SEQRAM_2A8
    {0x42AA, 2, 0x00A2},  // DYNAMIC_SEQRAM_2AA
    {0x42AC, 2, 0x3018},  // DYNAMIC_SEQRAM_2AC
    {0x42AE, 2, 0xF007},  // DYNAMIC_SEQRAM_2AE
    {0x42B0, 2, 0x9DF0},  // DYNAMIC_SEQRAM_2B0
    {0x42B2, 2, 0x1C8C},  // DYNAMIC_SEQRAM_2B2
    {0x42B4, 2, 0xF005},  // DYNAMIC_SEQRAM_2B4
    {0x42B6, 2, 0x301F},  // DYNAMIC_SEQRAM_2B6
    {0x42B8, 2, 0x216D},  // DYNAMIC_SEQRAM_2B8
    {0x42BA, 2, 0x0A51},  // DYNAMIC_SEQRAM_2BA
    {0x42BC, 2, 0x1CEA},  // DYNAMIC_SEQRAM_2BC
    {0x42BE, 2, 0x4162},  // DYNAMIC_SEQRAM_2BE
    {0x42C0, 2, 0x0045},  // DYNAMIC_SEQRAM_2C0
    {0x42C2, 2, 0xF000},  // DYNAMIC_SEQRAM_2C2
    {0x42C4, 2, 0x30C0},  // DYNAMIC_SEQRAM_2C4
    {0x42C6, 2, 0xF001},  // DYNAMIC_SEQRAM_2C6
    {0x42C8, 2, 0x83F0},  // DYNAMIC_SEQRAM_2C8
    {0x42CA, 2, 0x0036},  // DYNAMIC_SEQRAM_2CA
    {0x42CC, 2, 0x00F0},  // DYNAMIC_SEQRAM_2CC
    {0x42CE, 2, 0x0087},  // DYNAMIC_SEQRAM_2CE
    {0x42D0, 2, 0xF006},  // DYNAMIC_SEQRAM_2D0
    {0x42D2, 2, 0x87F0},  // DYNAMIC_SEQRAM_2D2
    {0x42D4, 2, 0x0036},  // DYNAMIC_SEQRAM_2D4
    {0x42D6, 2, 0xC0F0},  // DYNAMIC_SEQRAM_2D6
    {0x42D8, 2, 0x0000},  // DYNAMIC_SEQRAM_2D8
    {0x42DA, 2, 0x0D00},  // DYNAMIC_SEQRAM_2DA
    {0x42DC, 2, 0x05F0},  // DYNAMIC_SEQRAM_2DC
    {0x42DE, 2, 0x0030},  // DYNAMIC_SEQRAM_2DE
    {0x42E0, 2, 0xC0F0},  // DYNAMIC_SEQRAM_2E0
    {0x42E2, 2, 0x0183},  // DYNAMIC_SEQRAM_2E2
    {0x42E4, 2, 0xF000},  // DYNAMIC_SEQRAM_2E4
    {0x42E6, 2, 0x3600},  // DYNAMIC_SEQRAM_2E6
    {0x42E8, 2, 0xF000},  // DYNAMIC_SEQRAM_2E8
    {0x42EA, 2, 0x87F0},  // DYNAMIC_SEQRAM_2EA
    {0x42EC, 2, 0x0687},  // DYNAMIC_SEQRAM_2EC
    {0x42EE, 2, 0xF000},  // DYNAMIC_SEQRAM_2EE
    {0x42F0, 2, 0x36C0},  // DYNAMIC_SEQRAM_2F0
    {0x42F2, 2, 0xF000},  // DYNAMIC_SEQRAM_2F2
    {0x42F4, 2, 0x000F},  // DYNAMIC_SEQRAM_2F4
    {0x42F6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_2F6
    {0x42F8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_2F8
    {0x42FA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_2FA
    {0x42FC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_2FC
    {0x42FE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_2FE
    {0x4300, 2, 0xE0E0},  // DYNAMIC_SEQRAM_300
    {0x4302, 2, 0xE0E0},  // DYNAMIC_SEQRAM_302
    {0x4304, 2, 0xE0E0},  // DYNAMIC_SEQRAM_304
    {0x4306, 2, 0xE0E0},  // DYNAMIC_SEQRAM_306
    {0x4308, 2, 0xE0E0},  // DYNAMIC_SEQRAM_308
    {0x430A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_30A
    {0x430C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_30C
    {0x430E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_30E
    {0x4310, 2, 0xE0E0},  // DYNAMIC_SEQRAM_310
    {0x4312, 2, 0xE0E0},  // DYNAMIC_SEQRAM_312
    {0x4314, 2, 0xE0E0},  // DYNAMIC_SEQRAM_314
    {0x4316, 2, 0xE0E0},  // DYNAMIC_SEQRAM_316
    {0x4318, 2, 0xE0E0},  // DYNAMIC_SEQRAM_318
    {0x431A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_31A
    {0x431C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_31C
    {0x431E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_31E
    {0x4320, 2, 0xE0E0},  // DYNAMIC_SEQRAM_320
    {0x4322, 2, 0xE0E0},  // DYNAMIC_SEQRAM_322
    {0x4324, 2, 0xE0E0},  // DYNAMIC_SEQRAM_324
    {0x4326, 2, 0xE0E0},  // DYNAMIC_SEQRAM_326
    {0x4328, 2, 0xE0E0},  // DYNAMIC_SEQRAM_328
    {0x432A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_32A
    {0x432C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_32C
    {0x432E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_32E
    {0x4330, 2, 0xE0E0},  // DYNAMIC_SEQRAM_330
    {0x4332, 2, 0xE0E0},  // DYNAMIC_SEQRAM_332
    {0x4334, 2, 0xE0E0},  // DYNAMIC_SEQRAM_334
    {0x4336, 2, 0xE0E0},  // DYNAMIC_SEQRAM_336
    {0x4338, 2, 0xE0E0},  // DYNAMIC_SEQRAM_338
    {0x433A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_33A
    {0x433C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_33C
    {0x433E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_33E
    {0x4340, 2, 0xE0E0},  // DYNAMIC_SEQRAM_340
    {0x4342, 2, 0xE0E0},  // DYNAMIC_SEQRAM_342
    {0x4344, 2, 0xE0E0},  // DYNAMIC_SEQRAM_344
    {0x4346, 2, 0xE0E0},  // DYNAMIC_SEQRAM_346
    {0x4348, 2, 0xE0E0},  // DYNAMIC_SEQRAM_348
    {0x434A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_34A
    {0x434C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_34C
    {0x434E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_34E
    {0x4350, 2, 0xE0E0},  // DYNAMIC_SEQRAM_350
    {0x4352, 2, 0xE0E0},  // DYNAMIC_SEQRAM_352
    {0x4354, 2, 0xE0E0},  // DYNAMIC_SEQRAM_354
    {0x4356, 2, 0xE0E0},  // DYNAMIC_SEQRAM_356
    {0x4358, 2, 0xE0E0},  // DYNAMIC_SEQRAM_358
    {0x435A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_35A
    {0x435C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_35C
    {0x435E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_35E
    {0x4360, 2, 0xE0E0},  // DYNAMIC_SEQRAM_360
    {0x4362, 2, 0xE0E0},  // DYNAMIC_SEQRAM_362
    {0x4364, 2, 0xE0E0},  // DYNAMIC_SEQRAM_364
    {0x4366, 2, 0xE0E0},  // DYNAMIC_SEQRAM_366
    {0x4368, 2, 0xE0E0},  // DYNAMIC_SEQRAM_368
    {0x436A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_36A
    {0x436C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_36C
    {0x436E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_36E
    {0x4370, 2, 0xE0E0},  // DYNAMIC_SEQRAM_370
    {0x4372, 2, 0xE0E0},  // DYNAMIC_SEQRAM_372
    {0x4374, 2, 0xE0E0},  // DYNAMIC_SEQRAM_374
    {0x4376, 2, 0xE0E0},  // DYNAMIC_SEQRAM_376
    {0x4378, 2, 0xE0E0},  // DYNAMIC_SEQRAM_378
    {0x437A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_37A
    {0x437C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_37C
    {0x437E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_37E
    {0x4380, 2, 0xE0E0},  // DYNAMIC_SEQRAM_380
    {0x4382, 2, 0xE0E0},  // DYNAMIC_SEQRAM_382
    {0x4384, 2, 0xE0E0},  // DYNAMIC_SEQRAM_384
    {0x4386, 2, 0xE0E0},  // DYNAMIC_SEQRAM_386
    {0x4388, 2, 0xE0E0},  // DYNAMIC_SEQRAM_388
    {0x438A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_38A
    {0x438C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_38C
    {0x438E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_38E
    {0x4390, 2, 0xE0E0},  // DYNAMIC_SEQRAM_390
    {0x4392, 2, 0xE0E0},  // DYNAMIC_SEQRAM_392
    {0x4394, 2, 0xE0E0},  // DYNAMIC_SEQRAM_394
    {0x4396, 2, 0xE0E0},  // DYNAMIC_SEQRAM_396
    {0x4398, 2, 0xE0E0},  // DYNAMIC_SEQRAM_398
    {0x439A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_39A
    {0x439C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_39C
    {0x439E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_39E
    {0x43A0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3A0
    {0x43A2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3A2
    {0x43A4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3A4
    {0x43A6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3A6
    {0x43A8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3A8
    {0x43AA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3AA
    {0x43AC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3AC
    {0x43AE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3AE
    {0x43B0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3B0
    {0x43B2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3B2
    {0x43B4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3B4
    {0x43B6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3B6
    {0x43B8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3B8
    {0x43BA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3BA
    {0x43BC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3BC
    {0x43BE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3BE
    {0x43C0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3C0
    {0x43C2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3C2
    {0x43C4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3C4
    {0x43C6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3C6
    {0x43C8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3C8
    {0x43CA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3CA
    {0x43CC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3CC
    {0x43CE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3CE
    {0x43D0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3D0
    {0x43D2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3D2
    {0x43D4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3D4
    {0x43D6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3D6
    {0x43D8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3D8
    {0x43DA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3DA
    {0x43DC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3DC
    {0x43DE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3DE
    {0x43E0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3E0
    {0x43E2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3E2
    {0x43E4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3E4
    {0x43E6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3E6
    {0x43E8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3E8
    {0x43EA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3EA
    {0x43EC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3EC
    {0x43EE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3EE
    {0x43F0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3F0
    {0x43F2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3F2
    {0x43F4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3F4
    {0x43F6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3F6
    {0x43F8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3F8
    {0x43FA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3FA
    {0x43FC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3FC
    {0x43FE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_3FE
    {0x4400, 2, 0xE0E0},  // DYNAMIC_SEQRAM_400
    {0x4402, 2, 0xE0E0},  // DYNAMIC_SEQRAM_402
    {0x4404, 2, 0xE0E0},  // DYNAMIC_SEQRAM_404
    {0x4406, 2, 0xE0E0},  // DYNAMIC_SEQRAM_406
    {0x4408, 2, 0xE0E0},  // DYNAMIC_SEQRAM_408
    {0x440A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_40A
    {0x440C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_40C
    {0x440E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_40E
    {0x4410, 2, 0xE0E0},  // DYNAMIC_SEQRAM_410
    {0x4412, 2, 0xE0E0},  // DYNAMIC_SEQRAM_412
    {0x4414, 2, 0xE0E0},  // DYNAMIC_SEQRAM_414
    {0x4416, 2, 0xE0E0},  // DYNAMIC_SEQRAM_416
    {0x4418, 2, 0xE0E0},  // DYNAMIC_SEQRAM_418
    {0x441A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_41A
    {0x441C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_41C
    {0x441E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_41E
    {0x4420, 2, 0xE0E0},  // DYNAMIC_SEQRAM_420
    {0x4422, 2, 0xE0E0},  // DYNAMIC_SEQRAM_422
    {0x4424, 2, 0xE0E0},  // DYNAMIC_SEQRAM_424
    {0x4426, 2, 0xE0E0},  // DYNAMIC_SEQRAM_426
    {0x4428, 2, 0xE0E0},  // DYNAMIC_SEQRAM_428
    {0x442A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_42A
    {0x442C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_42C
    {0x442E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_42E
    {0x4430, 2, 0xE0E0},  // DYNAMIC_SEQRAM_430
    {0x4432, 2, 0xE0E0},  // DYNAMIC_SEQRAM_432
    {0x4434, 2, 0xE0E0},  // DYNAMIC_SEQRAM_434
    {0x4436, 2, 0xE0E0},  // DYNAMIC_SEQRAM_436
    {0x4438, 2, 0xE0E0},  // DYNAMIC_SEQRAM_438
    {0x443A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_43A
    {0x443C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_43C
    {0x443E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_43E
    {0x4440, 2, 0xE0E0},  // DYNAMIC_SEQRAM_440
    {0x4442, 2, 0xE0E0},  // DYNAMIC_SEQRAM_442
    {0x4444, 2, 0xE0E0},  // DYNAMIC_SEQRAM_444
    {0x4446, 2, 0xE0E0},  // DYNAMIC_SEQRAM_446
    {0x4448, 2, 0xE0E0},  // DYNAMIC_SEQRAM_448
    {0x444A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_44A
    {0x444C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_44C
    {0x444E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_44E
    {0x4450, 2, 0xE0E0},  // DYNAMIC_SEQRAM_450
    {0x4452, 2, 0xE0E0},  // DYNAMIC_SEQRAM_452
    {0x4454, 2, 0xE0E0},  // DYNAMIC_SEQRAM_454
    {0x4456, 2, 0xE0E0},  // DYNAMIC_SEQRAM_456
    {0x4458, 2, 0xE0E0},  // DYNAMIC_SEQRAM_458
    {0x445A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_45A
    {0x445C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_45C
    {0x445E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_45E
    {0x4460, 2, 0xE0E0},  // DYNAMIC_SEQRAM_460
    {0x4462, 2, 0xE0E0},  // DYNAMIC_SEQRAM_462
    {0x4464, 2, 0xE0E0},  // DYNAMIC_SEQRAM_464
    {0x4466, 2, 0xE0E0},  // DYNAMIC_SEQRAM_466
    {0x4468, 2, 0xE0E0},  // DYNAMIC_SEQRAM_468
    {0x446A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_46A
    {0x446C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_46C
    {0x446E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_46E
    {0x4470, 2, 0xE0E0},  // DYNAMIC_SEQRAM_470
    {0x4472, 2, 0xE0E0},  // DYNAMIC_SEQRAM_472
    {0x4474, 2, 0xE0E0},  // DYNAMIC_SEQRAM_474
    {0x4476, 2, 0xE0E0},  // DYNAMIC_SEQRAM_476
    {0x4478, 2, 0xE0E0},  // DYNAMIC_SEQRAM_478
    {0x447A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_47A
    {0x447C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_47C
    {0x447E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_47E
    {0x4480, 2, 0xE0E0},  // DYNAMIC_SEQRAM_480
    {0x4482, 2, 0xE0E0},  // DYNAMIC_SEQRAM_482
    {0x4484, 2, 0xE0E0},  // DYNAMIC_SEQRAM_484
    {0x4486, 2, 0xE0E0},  // DYNAMIC_SEQRAM_486
    {0x4488, 2, 0xE0E0},  // DYNAMIC_SEQRAM_488
    {0x448A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_48A
    {0x448C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_48C
    {0x448E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_48E
    {0x4490, 2, 0xE0E0},  // DYNAMIC_SEQRAM_490
    {0x4492, 2, 0xE0E0},  // DYNAMIC_SEQRAM_492
    {0x4494, 2, 0xE0E0},  // DYNAMIC_SEQRAM_494
    {0x4496, 2, 0xE0E0},  // DYNAMIC_SEQRAM_496
    {0x4498, 2, 0xE0E0},  // DYNAMIC_SEQRAM_498
    {0x449A, 2, 0xE0E0},  // DYNAMIC_SEQRAM_49A
    {0x449C, 2, 0xE0E0},  // DYNAMIC_SEQRAM_49C
    {0x449E, 2, 0xE0E0},  // DYNAMIC_SEQRAM_49E
    {0x44A0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4A0
    {0x44A2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4A2
    {0x44A4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4A4
    {0x44A6, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4A6
    {0x44A8, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4A8
    {0x44AA, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4AA
    {0x44AC, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4AC
    {0x44AE, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4AE
    {0x44B0, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4B0
    {0x44B2, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4B2
    {0x44B4, 2, 0xE0E0},  // DYNAMIC_SEQRAM_4B4
    {0x5500, 2, 0x0000},  // AGAIN_LUT0
    {0x5502, 2, 0x0002},  // AGAIN_LUT1
    {0x5504, 2, 0x0006},  // AGAIN_LUT2
    {0x5506, 2, 0x0009},  // AGAIN_LUT3
    {0x5508, 2, 0x000F},  // AGAIN_LUT4
    {0x550A, 2, 0x0010},  // AGAIN_LUT5
    {0x550C, 2, 0x0011},  // AGAIN_LUT6
    {0x550E, 2, 0x0012},  // AGAIN_LUT7
    {0x5510, 2, 0x0019},  // AGAIN_LUT8
    {0x5512, 2, 0x0020},  // AGAIN_LUT9
    {0x5514, 2, 0x0021},  // AGAIN_LUT10
    {0x5516, 2, 0x0023},  // AGAIN_LUT11
    {0x5518, 2, 0x0026},  // AGAIN_LUT12
    {0x551A, 2, 0x002B},  // AGAIN_LUT13
    {0x551C, 2, 0x002F},  // AGAIN_LUT14
    {0x551E, 2, 0x0030},  // AGAIN_LUT15
    {0x5400, 2, 0x0100},  // GT1_COARSE0
    {0x5402, 2, 0x2106},  // GT1_COARSE1
    {0x5404, 2, 0x1101},  // GT1_COARSE2
    {0x5406, 2, 0x3106},  // GT1_COARSE3
    {0x5408, 2, 0x7100},  // GT1_COARSE4
    {0x540A, 2, 0x8107},  // GT1_COARSE5
    {0x540C, 2, 0xB101},  // GT1_COARSE6
    {0x540E, 2, 0xD101},  // GT1_COARSE7
    {0x5410, 2, 0xF12E},  // GT1_COARSE8
    {0x5412, 2, 0xF112},  // GT1_COARSE9
    {0x5414, 2, 0xF184},  // GT1_COARSE10
    {0x5416, 2, 0xF224},  // GT1_COARSE11
    {0x5418, 2, 0xF306},  // GT1_COARSE12
    {0x541A, 2, 0xF446},  // GT1_COARSE13
    {0x541C, 2, 0xF609},  // GT1_COARSE14
    {0x541E, 2, 0xF887},  // GT1_COARSE15
    {0x5420, 2, 0xFC0B},  // GT1_COARSE16
    {0x5422, 2, 0xFC0B},  // GT1_COARSE17
    {0x5424, 2, 0xFFFA},  // GT1_DCG_ATTN_SET0
    {0x5426, 2, 0x5557},  // GT1_DCG_ATTN_SET1
    {0x5428, 2, 0x0005},  // GT1_DCG_ATTN_SET2
    {0x542A, 2, 0xA550},  // GT1_ZONE_SET0
    {0x542C, 2, 0xAAAA},  // GT1_ZONE_SET1
    {0x542E, 2, 0x000A},  // GT1_ZONE_SET2
    {0x5460, 2, 0x2269},  // ZT1_REG0_ADDR
    {0x5462, 2, 0x0B87},  // ZT1_REG0_VALUE0
    {0x5464, 2, 0x0B87},  // ZT1_REG0_VALUE1
    {0x5466, 2, 0x0983},  // ZT1_REG0_VALUE2
    {0x5498, 2, 0x225E},  // ZT1_REG7_ADDR
    {0x549A, 2, 0xBCAA},  // ZT1_REG7_VALUE0
    {0x549C, 2, 0xBCAA},  // ZT1_REG7_VALUE1
    {0x549E, 2, 0xBDAA},  // ZT1_REG7_VALUE2
    {0x3060, 2, 0xFF01},  // GAIN_TABLE_CTRL
    {0x44BA, 2, 0x0050},  // DAC_LD_4_5
    {0x44BC, 2, 0xBCAA},  // DAC_LD_6_7
    {0x44C0, 2, 0x4070},  // DAC_LD_10_11
    {0x44C4, 2, 0x04D0},  // DAC_LD_14_15
    {0x44C6, 2, 0x17E2},  // DAC_LD_16_17
    {0x44C8, 2, 0xEA43},  // DAC_LD_18_19
    {0x44CA, 2, 0x000E},  // DAC_LD_20_21
    {0x44CC, 2, 0x7777},  // DAC_LD_22_23
    {0x44CE, 2, 0x8BA4},  // DAC_LD_24_25
    {0x44D0, 2, 0x1735},  // DAC_LD_26_27
    {0x44D2, 2, 0x0B87},  // DAC_LD_28_29
    {0x44D4, 2, 0x0000},  // DAC_LD_30_31
    {0x44D6, 2, 0xF206},  // DAC_LD_32_33
    {0x44D8, 2, 0xAAFA},  // DAC_LD_34_35
    {0x44DA, 2, 0xE001},  // DAC_LD_36_37
    {0x44DE, 2, 0x9BBC},  // DAC_LD_40_41
    {0x44E0, 2, 0x283C},  // DAC_LD_42_43
    {0x44E2, 2, 0x2821},  // DAC_LD_44_45
    {0x44E4, 2, 0x8000},  // DAC_LD_46_47
    {0x44E6, 2, 0x503F},  // DAC_LD_48_49
    {0x32A4, 2, 0x0000},  // CRM_CTRL
    {0x328E, 2, 0x0004},  // ADDR_CTRL 
    {0x333C, 2, 0x0001},  // DYNAMIC_CTRL
    {0x301A, 2, 0x0000},  // RESET_REGISTER
    {0x3600, 2, 0x94DF},  // FDOC_CTRL
    {0x3616, 2, 0x0000},  // FDOC_CTRL2
    {0x3700, 2, 0x0001},  // PIX_DEF_ID
    {0x3980, 2, 0x0003},  // PIX_DEF_CORR
    {0x36C0, 2, 0x0001},  // DIGITAL_GAIN_CTRL
    {0x36DE, 2, 0x002A},  // DATA_PEDESTAL1
    {0x301A, 2, 0x0008},  // RESET_REGISTER
    {0x3060, 2, 0x0000},  // GAIN_TABLE_CTRL
    {0x3982, 2, 0xAC70},  // PDC_DYN_EDGE_THRES
    {0x3984, 2, 0xFA98},  // PDC_DYN_LO_DEFECT_THRES
    {0x3986, 2, 0xFC3F},  // PDC_DYN_HI_DEFECT_THRES
    {0x3988, 2, 0xAC70},  // PDC_DYN_EDGE_THRES_T2
    {0x398A, 2, 0xFA98},  // PDC_DYN_LO_DEFECT_THRES_T2
    {0x398C, 2, 0xFC3F},  // PDC_DYN_HI_DEFECT_THRES_T2
    {0x3980, 2, 0x0003},  // PIX_DEF_CORR
    {0x3060, 2, 0xFF01},  // GAIN_TABLE_CTRL
    {0x3340, 2, 0x1C60},  // OTPM_CTRL
    {0x0400, 2, 0x0003},  // SCALING_MODE
    {0x0404, 2, 0x0BFC},  // SCALE_M
    {0x0406, 2, 0x0400},  // SCALE_N
    {0x3C58, 2, 0x0008},  // SLICE_OVERLAP
    {0x3C40, 2, 0x0002},  // SCALE_CTRL
    {0x3C42, 2, 0x01E4},  // SCALE_SECOND_RESIDUAL_EVEN
    {0x3C44, 2, 0x05E2},  // SCALE_SECOND_RESIDUAL_ODD
    {0x3C46, 2, 0x0A0A},  // SCALE_SECOND_CROP
    {0x3C48, 2, 0x0BB0},  // SCALE_THIRD_RESIDUAL_EVEN
    {0x3C4A, 2, 0x03B2},  // SCALE_THIRD_RESIDUAL_ODD
    {0x3C4C, 2, 0x090A},  // SCALE_THIRD_CROP
    {0x3C4E, 2, 0x0980},  // SCALE_FOURTH_RESIDUAL_EVEN
    {0x3C50, 2, 0x0182},  // SCALE_FOURTH_RESIDUAL_ODD
    {0x3C52, 2, 0x090A},  // SCALE_FOURTH_CROP
    {0x3C60, 2, 0x0120},  // SCALE_FIRST_PXL_NUM
    {0x3C62, 2, 0x0118},  // SCALE_SECOND_PXL_NUM
    {0x3C64, 2, 0x0118},  // SCALE_THIRD_PXL_NUM
    {0x3C66, 2, 0x0110},  // SCALE_FOURTH_PXL_NUM
    {0x3EC8, 2, 0x0000},  // X_OUTPUT_OFFSET
    {0x3ECA, 2, 0x0000},  // Y_OUTPUT_OFFSET
    {0x034C, 2, 0x0460},  // X_OUTPUT_SIZE
    {0x034E, 2, 0x0340},  // Y_OUTPUT_SIZE
    {0x3C7A, 2, 0x0000},  // REORDER_CTRL
    {0x3C6A, 2, 0},  // X_OUTPUT_OFFSET2
    {0x3C70, 2, 0},  // Y_OUTPUT_OFFSET2
    {0x3C6C, 2, 0x0220},  // X_OUTPUT_SIZE2
    {0x3C72, 2, 0x0180},  // Y_OUTPUT_SIZE2
    {0x3C74, 2, 0x0003},  // SYNC_REGEN
    {0x3C76, 2, 0x0020},  // SYNC_REGEN_BLANK
    {0x3F18, 2, 0x7B70},  // MIPI_JPEG_PN9_DATA_TYPE
    {0x3F1A, 2, 0x102B},  // MIPI_DATA_TYPE_INTERLEAVE

    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE

    {0x0100, 1, 0x01},  // MODE_SELECT
    {0x44D6, 2, 0xB206},  // DAC_LD_32_33
};
#else
const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_1120x840_544x408_table[] =
{
    {0x0103, 0x3201, 0x01}, // SOFTWARE_RESET

    {0x0304, 2, 0x0002}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 0x0056}, // VT_PLL_MULTIPLIER
    {0x0300, 2, 0x0006}, // VT_PIX_CLK_DIV
    {0x0302, 2, 0x0001}, // VT_SYS_CLK_DIV
    {0x030C, 2, 0x0007}, // OP_PRE_PLL_CLK_DIV
    {0x030E, 2, 0x01a0}, // {0x030E, 2, 0x0247}, // OP_PLL_MULTIPLIER
    {0x0308, 2, 0x000A}, // OP_PIX_CLK_DIV
    {0x030A, 2, 0x0001}, // OP_SYS_CLK_DIV
    {0x0344, 2, 0x0368}, // X_ADDR_START
    {0x0348, 2, 0x10A7}, // X_ADDR_END
    {0x0346, 2, 0x0290}, // Y_ADDR_START
    {0x034A, 2, 0x0C7F}, // Y_ADDR_END
    {0x034C, 2, 0x0D40}, // X_OUTPUT_SIZE
    {0x034E, 2, 0x09F0}, // Y_OUTPUT_SIZE
    {0x0380, 2, 0x0001}, // X_EVEN_INC
    {0x0382, 2, 0x0001}, // X_ODD_INC
    {0x0384, 2, 0x0001}, // Y_EVEN_INC
    {0x0386, 2, 0x0001}, // Y_ODD_INC

    {0x0900, 1, 0x00}, // BINNING_MODE
    {0x0901, 1, 0x11}, // BINNING_TYPE

    {0x0342, 2, 0x1710}, // LINE_LENGTH_PCK
    {0x0340, 2, 0x09FE}, // FRAME_LENGTH_LINES
    {0x0202, 2, 0x091A}, // COARSE_INTEGRATION_TIME
    {0x0112, 2, 0x0A0A}, // CSI_DATA_FORMAT
    {0x0114, 1, 0x03}, // CSI_LANE_MODE
#if 1
    {0x0800, 1, 0x0C}, // TCLK_POST
    {0x0801, 1, 0x06}, // THS_PREPARE
    {0x0802, 1, 0x0B}, // THS_ZERO_MIN
    {0x0803, 1, 0x08}, // THS_TRAIL
    {0x0804, 1, 0x0A}, // TCLK_TRAIL_MIN
    {0x0805, 1, 0x07}, // TCLK_PREPARE
    {0x0806, 1, 0x21}, // TCLK_ZERO
    {0x0807, 1, 0x07}, // TLPX
    {0x082A, 1, 0x0E}, // TWAKEUP
    {0x082B, 1, 0x0B}, // TINIT
    {0x082C, 1, 0x0C}, // THS_EXIT
    {0x3F06, 2, 0x00C0}, // MIPI_TIMING_2
    {0x3F0A, 2, 0x0000}, //{0x3F0A, 2, 0x8000}, // MIPI_TIMING_4
    {0x3F0C, 2, 0x0009}, // MIPI_TIMING_5
    {0x3F20, 2, 0x0808}, // MIPI_PHY_TRIM_MSB
    {0x3F02, 2, 0x0002}, // PHY_CTRL
#else
    {0x0800, 1, 0x12}, // TCLK_POST
    {0x0801, 1, 0x0A}, // THS_PREPARE
    {0x0802, 1, 0x14}, // THS_ZERO_MIN
    {0x0803, 1, 0x0E}, // THS_TRAIL
    {0x0804, 1, 0x11}, // TCLK_TRAIL_MIN
    {0x0805, 1, 0x09}, // TCLK_PREPARE
    {0x0806, 1, 0x3C}, // TCLK_ZERO
    {0x0807, 1, 0x0B}, // TLPX
    {0x082A, 1, 0x19}, // TWAKEUP
    {0x082B, 1, 0x14}, // TINIT
    {0x082C, 1, 0x15}, // THS_EXIT
    {0x3F06, 2, 0x00C0}, // MIPI_TIMING_2
    {0x3F0A, 2, 0x2000}, //{0x3F0A, 2, 0xA000}, // MIPI_TIMING_4
    {0x3F0C, 2, 0x0010}, // MIPI_TIMING_5
    {0x3F20, 2, 0x8088}, // MIPI_PHY_TRIM_MSB
    {0x3F1E, 2, 0x0004}, // MIPI_PHY_TRIM_LSB
    {0x3F20, 2, 0x0808}, // MIPI_PHY_TRIM_MSB
#endif
    {0x4000, 2, 0x0114}, // DYNAMIC_SEQRAM_00
    {0x4002, 2, 0x1A25}, // DYNAMIC_SEQRAM_02
    {0x4004, 2, 0x3DFF}, // DYNAMIC_SEQRAM_04
    {0x4006, 2, 0xFFFF}, // DYNAMIC_SEQRAM_06
    {0x4008, 2, 0x0A35}, // DYNAMIC_SEQRAM_08
    {0x400A, 2, 0x10EF}, // DYNAMIC_SEQRAM_0A
    {0x400C, 2, 0x3003}, // DYNAMIC_SEQRAM_0C
    {0x400E, 2, 0x30D8}, // DYNAMIC_SEQRAM_0E
    {0x4010, 2, 0xF003}, // DYNAMIC_SEQRAM_10
    {0x4012, 2, 0xB5F0}, // DYNAMIC_SEQRAM_12
    {0x4014, 2, 0x0085}, // DYNAMIC_SEQRAM_14
    {0x4016, 2, 0xF004}, // DYNAMIC_SEQRAM_16
    {0x4018, 2, 0x9A89}, // DYNAMIC_SEQRAM_18
    {0x401A, 2, 0xF000}, // DYNAMIC_SEQRAM_1A
    {0x401C, 2, 0x9997}, // DYNAMIC_SEQRAM_1C
    {0x401E, 2, 0xF000}, // DYNAMIC_SEQRAM_1E
    {0x4020, 2, 0x30C0}, // DYNAMIC_SEQRAM_20
    {0x4022, 2, 0xF000}, // DYNAMIC_SEQRAM_22
    {0x4024, 2, 0x82F0}, // DYNAMIC_SEQRAM_24
    {0x4026, 2, 0x0030}, // DYNAMIC_SEQRAM_26
    {0x4028, 2, 0x18F0}, // DYNAMIC_SEQRAM_28
    {0x402A, 2, 0x0320}, // DYNAMIC_SEQRAM_2A
    {0x402C, 2, 0x58F0}, // DYNAMIC_SEQRAM_2C
    {0x402E, 2, 0x089C}, // DYNAMIC_SEQRAM_2E
    {0x4030, 2, 0xF010}, // DYNAMIC_SEQRAM_30
    {0x4032, 2, 0x99B6}, // DYNAMIC_SEQRAM_32
    {0x4034, 2, 0xF003}, // DYNAMIC_SEQRAM_34
    {0x4036, 2, 0xB498}, // DYNAMIC_SEQRAM_36
    {0x4038, 2, 0xA096}, // DYNAMIC_SEQRAM_38
    {0x403A, 2, 0xF000}, // DYNAMIC_SEQRAM_3A
    {0x403C, 2, 0xA2F0}, // DYNAMIC_SEQRAM_3C
    {0x403E, 2, 0x00A2}, // DYNAMIC_SEQRAM_3E
    {0x4040, 2, 0xF008}, // DYNAMIC_SEQRAM_40
    {0x4042, 2, 0x9DF0}, // DYNAMIC_SEQRAM_42
    {0x4044, 2, 0x209D}, // DYNAMIC_SEQRAM_44
    {0x4046, 2, 0x8C08}, // DYNAMIC_SEQRAM_46
    {0x4048, 2, 0x08F0}, // DYNAMIC_SEQRAM_48
    {0x404A, 2, 0x0036}, // DYNAMIC_SEQRAM_4A
    {0x404C, 2, 0x008F}, // DYNAMIC_SEQRAM_4C
    {0x404E, 2, 0xF000}, // DYNAMIC_SEQRAM_4E
    {0x4050, 2, 0x88F0}, // DYNAMIC_SEQRAM_50
    {0x4052, 2, 0x0488}, // DYNAMIC_SEQRAM_52
    {0x4054, 2, 0xF000}, // DYNAMIC_SEQRAM_54
    {0x4056, 2, 0x3600}, // DYNAMIC_SEQRAM_56
    {0x4058, 2, 0xF000}, // DYNAMIC_SEQRAM_58
    {0x405A, 2, 0x83F0}, // DYNAMIC_SEQRAM_5A
    {0x405C, 2, 0x0290}, // DYNAMIC_SEQRAM_5C
    {0x405E, 2, 0xF000}, // DYNAMIC_SEQRAM_5E
    {0x4060, 2, 0x8BF0}, // DYNAMIC_SEQRAM_60
    {0x4062, 2, 0x2EA3}, // DYNAMIC_SEQRAM_62
    {0x4064, 2, 0xF000}, // DYNAMIC_SEQRAM_64
    {0x4066, 2, 0xA3F0}, // DYNAMIC_SEQRAM_66
    {0x4068, 2, 0x089D}, // DYNAMIC_SEQRAM_68
    {0x406A, 2, 0xF075}, // DYNAMIC_SEQRAM_6A
    {0x406C, 2, 0x3003}, // DYNAMIC_SEQRAM_6C
    {0x406E, 2, 0x4070}, // DYNAMIC_SEQRAM_6E
    {0x4070, 2, 0x216D}, // DYNAMIC_SEQRAM_70
    {0x4072, 2, 0x1CF6}, // DYNAMIC_SEQRAM_72
    {0x4074, 2, 0x8B00}, // DYNAMIC_SEQRAM_74
    {0x4076, 2, 0x5186}, // DYNAMIC_SEQRAM_76
    {0x4078, 2, 0x1300}, // DYNAMIC_SEQRAM_78
    {0x407A, 2, 0x0205}, // DYNAMIC_SEQRAM_7A
    {0x407C, 2, 0x36D8}, // DYNAMIC_SEQRAM_7C
    {0x407E, 2, 0xF002}, // DYNAMIC_SEQRAM_7E
    {0x4080, 2, 0x8387}, // DYNAMIC_SEQRAM_80
    {0x4082, 2, 0xF006}, // DYNAMIC_SEQRAM_82
    {0x4084, 2, 0x8702}, // DYNAMIC_SEQRAM_84
    {0x4086, 2, 0x0D02}, // DYNAMIC_SEQRAM_86
    {0x4088, 2, 0x05F0}, // DYNAMIC_SEQRAM_88
    {0x408A, 2, 0x0383}, // DYNAMIC_SEQRAM_8A
    {0x408C, 2, 0xF001}, // DYNAMIC_SEQRAM_8C
    {0x408E, 2, 0x87F0}, // DYNAMIC_SEQRAM_8E
    {0x4090, 2, 0x0213}, // DYNAMIC_SEQRAM_90
    {0x4092, 2, 0x0036}, // DYNAMIC_SEQRAM_92
    {0x4094, 2, 0xD887}, // DYNAMIC_SEQRAM_94
    {0x4096, 2, 0x020D}, // DYNAMIC_SEQRAM_96
    {0x4098, 2, 0xE0E0}, // DYNAMIC_SEQRAM_98
    {0x409A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9A
    {0x409C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9C
    {0x409E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9E
    {0x40A0, 2, 0xF000}, // DYNAMIC_SEQRAM_A0
    {0x40A2, 2, 0x0401}, // DYNAMIC_SEQRAM_A2
    {0x40A4, 2, 0xF008}, // DYNAMIC_SEQRAM_A4
    {0x40A6, 2, 0x82F0}, // DYNAMIC_SEQRAM_A6
    {0x40A8, 2, 0x0883}, // DYNAMIC_SEQRAM_A8
    {0x40AA, 2, 0xF009}, // DYNAMIC_SEQRAM_AA
    {0x40AC, 2, 0x85F0}, // DYNAMIC_SEQRAM_AC
    {0x40AE, 2, 0x2985}, // DYNAMIC_SEQRAM_AE
    {0x40B0, 2, 0x87F0}, // DYNAMIC_SEQRAM_B0
    {0x40B2, 2, 0x2A87}, // DYNAMIC_SEQRAM_B2
    {0x40B4, 2, 0xF63E}, // DYNAMIC_SEQRAM_B4
    {0x40B6, 2, 0x88F0}, // DYNAMIC_SEQRAM_B6
    {0x40B8, 2, 0x0801}, // DYNAMIC_SEQRAM_B8
    {0x40BA, 2, 0x40F0}, // DYNAMIC_SEQRAM_BA
    {0x40BC, 2, 0x0800}, // DYNAMIC_SEQRAM_BC
    {0x40BE, 2, 0x48F0}, // DYNAMIC_SEQRAM_BE
    {0x40C0, 2, 0x0882}, // DYNAMIC_SEQRAM_C0
    {0x40C2, 2, 0xF008}, // DYNAMIC_SEQRAM_C2
    {0x40C4, 2, 0x0401}, // DYNAMIC_SEQRAM_C4
    {0x40C6, 2, 0xF008}, // DYNAMIC_SEQRAM_C6
    {0x40C8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_C8
    {0x40CA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CA
    {0x40CC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CC
    {0x40CE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CE
    {0x40D0, 2, 0xF000}, // DYNAMIC_SEQRAM_D0
    {0x40D2, 2, 0x0401}, // DYNAMIC_SEQRAM_D2
    {0x40D4, 2, 0xF015}, // DYNAMIC_SEQRAM_D4
    {0x40D6, 2, 0x003C}, // DYNAMIC_SEQRAM_D6
    {0x40D8, 2, 0xF00E}, // DYNAMIC_SEQRAM_D8
    {0x40DA, 2, 0x85F0}, // DYNAMIC_SEQRAM_DA
    {0x40DC, 2, 0x0687}, // DYNAMIC_SEQRAM_DC
    {0x40DE, 2, 0xF002}, // DYNAMIC_SEQRAM_DE
    {0x40E0, 2, 0x87F0}, // DYNAMIC_SEQRAM_E0
    {0x40E2, 2, 0x61E8}, // DYNAMIC_SEQRAM_E2
    {0x40E4, 2, 0x3904}, // DYNAMIC_SEQRAM_E4
    {0x40E6, 2, 0xF005}, // DYNAMIC_SEQRAM_E6
    {0x40E8, 2, 0x3482}, // DYNAMIC_SEQRAM_E8
    {0x40EA, 2, 0xF000}, // DYNAMIC_SEQRAM_EA
    {0x40EC, 2, 0x3241}, // DYNAMIC_SEQRAM_EC
    {0x40EE, 2, 0xF000}, // DYNAMIC_SEQRAM_EE
    {0x40F0, 2, 0x3904}, // DYNAMIC_SEQRAM_F0
    {0x40F2, 2, 0xF00E}, // DYNAMIC_SEQRAM_F2
    {0x40F4, 2, 0x3904}, // DYNAMIC_SEQRAM_F4
    {0x40F6, 2, 0xF000}, // DYNAMIC_SEQRAM_F6
    {0x40F8, 2, 0x3241}, // DYNAMIC_SEQRAM_F8
    {0x40FA, 2, 0xF000}, // DYNAMIC_SEQRAM_FA
    {0x40FC, 2, 0x3482}, // DYNAMIC_SEQRAM_FC
    {0x40FE, 2, 0xF005}, // DYNAMIC_SEQRAM_FE
    {0x4100, 2, 0xC0E6}, // DYNAMIC_SEQRAM_100
    {0x4102, 2, 0xF004}, // DYNAMIC_SEQRAM_102
    {0x4104, 2, 0x3904}, // DYNAMIC_SEQRAM_104
    {0x4106, 2, 0xF003}, // DYNAMIC_SEQRAM_106
    {0x4108, 2, 0xB0F0}, // DYNAMIC_SEQRAM_108
    {0x410A, 2, 0x0000}, // DYNAMIC_SEQRAM_10A
    {0x410C, 2, 0x18F0}, // DYNAMIC_SEQRAM_10C
    {0x410E, 2, 0x0086}, // DYNAMIC_SEQRAM_10E
    {0x4110, 2, 0xF000}, // DYNAMIC_SEQRAM_110
    {0x4112, 2, 0x86F0}, // DYNAMIC_SEQRAM_112
    {0x4114, 2, 0x89B0}, // DYNAMIC_SEQRAM_114
    {0x4116, 2, 0xF000}, // DYNAMIC_SEQRAM_116
    {0x4118, 2, 0xE9F0}, // DYNAMIC_SEQRAM_118
    {0x411A, 2, 0x008A}, // DYNAMIC_SEQRAM_11A
    {0x411C, 2, 0xF000}, // DYNAMIC_SEQRAM_11C
    {0x411E, 2, 0x0005}, // DYNAMIC_SEQRAM_11E
    {0x4120, 2, 0xF000}, // DYNAMIC_SEQRAM_120
    {0x4122, 2, 0xE0E0}, // DYNAMIC_SEQRAM_122
    {0x4124, 2, 0xE0E0}, // DYNAMIC_SEQRAM_124
    {0x4126, 2, 0xE0E0}, // DYNAMIC_SEQRAM_126
    {0x4128, 2, 0x0A35}, // DYNAMIC_SEQRAM_128
    {0x412A, 2, 0x10EF}, // DYNAMIC_SEQRAM_12A
    {0x412C, 2, 0x3003}, // DYNAMIC_SEQRAM_12C
    {0x412E, 2, 0x30D8}, // DYNAMIC_SEQRAM_12E
    {0x4130, 2, 0xF005}, // DYNAMIC_SEQRAM_130
    {0x4132, 2, 0x85F0}, // DYNAMIC_SEQRAM_132
    {0x4134, 2, 0x049A}, // DYNAMIC_SEQRAM_134
    {0x4136, 2, 0x89F0}, // DYNAMIC_SEQRAM_136
    {0x4138, 2, 0x0099}, // DYNAMIC_SEQRAM_138
    {0x413A, 2, 0x97F0}, // DYNAMIC_SEQRAM_13A
    {0x413C, 2, 0x0030}, // DYNAMIC_SEQRAM_13C
    {0x413E, 2, 0xC0F0}, // DYNAMIC_SEQRAM_13E
    {0x4140, 2, 0x0082}, // DYNAMIC_SEQRAM_140
    {0x4142, 2, 0xF000}, // DYNAMIC_SEQRAM_142
    {0x4144, 2, 0x3018}, // DYNAMIC_SEQRAM_144
    {0x4146, 2, 0xF002}, // DYNAMIC_SEQRAM_146
    {0x4148, 2, 0xB520}, // DYNAMIC_SEQRAM_148
    {0x414A, 2, 0x58F0}, // DYNAMIC_SEQRAM_14A
    {0x414C, 2, 0x089C}, // DYNAMIC_SEQRAM_14C
    {0x414E, 2, 0xF010}, // DYNAMIC_SEQRAM_14E
    {0x4150, 2, 0x99B6}, // DYNAMIC_SEQRAM_150
    {0x4152, 2, 0xF003}, // DYNAMIC_SEQRAM_152
    {0x4154, 2, 0xB498}, // DYNAMIC_SEQRAM_154
    {0x4156, 2, 0xA096}, // DYNAMIC_SEQRAM_156
    {0x4158, 2, 0xF000}, // DYNAMIC_SEQRAM_158
    {0x415A, 2, 0xA2F0}, // DYNAMIC_SEQRAM_15A
    {0x415C, 2, 0x00A2}, // DYNAMIC_SEQRAM_15C
    {0x415E, 2, 0xF008}, // DYNAMIC_SEQRAM_15E
    {0x4160, 2, 0x9DF0}, // DYNAMIC_SEQRAM_160
    {0x4162, 2, 0x209D}, // DYNAMIC_SEQRAM_162
    {0x4164, 2, 0x8C08}, // DYNAMIC_SEQRAM_164
    {0x4166, 2, 0x08F0}, // DYNAMIC_SEQRAM_166
    {0x4168, 2, 0x0036}, // DYNAMIC_SEQRAM_168
    {0x416A, 2, 0x008F}, // DYNAMIC_SEQRAM_16A
    {0x416C, 2, 0x88F0}, // DYNAMIC_SEQRAM_16C
    {0x416E, 2, 0x0188}, // DYNAMIC_SEQRAM_16E
    {0x4170, 2, 0x3600}, // DYNAMIC_SEQRAM_170
    {0x4172, 2, 0xF000}, // DYNAMIC_SEQRAM_172
    {0x4174, 2, 0x83F0}, // DYNAMIC_SEQRAM_174
    {0x4176, 2, 0x0290}, // DYNAMIC_SEQRAM_176
    {0x4178, 2, 0xF001}, // DYNAMIC_SEQRAM_178
    {0x417A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_17A
    {0x417C, 2, 0x2DA3}, // DYNAMIC_SEQRAM_17C
    {0x417E, 2, 0xF000}, // DYNAMIC_SEQRAM_17E
    {0x4180, 2, 0xA3F0}, // DYNAMIC_SEQRAM_180
    {0x4182, 2, 0x089D}, // DYNAMIC_SEQRAM_182
    {0x4184, 2, 0xF06D}, // DYNAMIC_SEQRAM_184
    {0x4186, 2, 0x4070}, // DYNAMIC_SEQRAM_186
    {0x4188, 2, 0x3003}, // DYNAMIC_SEQRAM_188
    {0x418A, 2, 0x214D}, // DYNAMIC_SEQRAM_18A
    {0x418C, 2, 0x1FF6}, // DYNAMIC_SEQRAM_18C
    {0x418E, 2, 0x0851}, // DYNAMIC_SEQRAM_18E
    {0x4190, 2, 0x0245}, // DYNAMIC_SEQRAM_190
    {0x4192, 2, 0x9D36}, // DYNAMIC_SEQRAM_192
    {0x4194, 2, 0xD8F0}, // DYNAMIC_SEQRAM_194
    {0x4196, 2, 0x0083}, // DYNAMIC_SEQRAM_196
    {0x4198, 2, 0xF000}, // DYNAMIC_SEQRAM_198
    {0x419A, 2, 0x87F0}, // DYNAMIC_SEQRAM_19A
    {0x419C, 2, 0x0087}, // DYNAMIC_SEQRAM_19C
    {0x419E, 2, 0xF000}, // DYNAMIC_SEQRAM_19E
    {0x41A0, 2, 0x36D8}, // DYNAMIC_SEQRAM_1A0
    {0x41A2, 2, 0x020D}, // DYNAMIC_SEQRAM_1A2
    {0x41A4, 2, 0x0205}, // DYNAMIC_SEQRAM_1A4
    {0x41A6, 2, 0xF000}, // DYNAMIC_SEQRAM_1A6
    {0x41A8, 2, 0x36D8}, // DYNAMIC_SEQRAM_1A8
    {0x41AA, 2, 0xF000}, // DYNAMIC_SEQRAM_1AA
    {0x41AC, 2, 0x83F0}, // DYNAMIC_SEQRAM_1AC
    {0x41AE, 2, 0x0087}, // DYNAMIC_SEQRAM_1AE
    {0x41B0, 2, 0xF000}, // DYNAMIC_SEQRAM_1B0
    {0x41B2, 2, 0x87F0}, // DYNAMIC_SEQRAM_1B2
    {0x41B4, 2, 0x0036}, // DYNAMIC_SEQRAM_1B4
    {0x41B6, 2, 0xD802}, // DYNAMIC_SEQRAM_1B6
    {0x41B8, 2, 0x0D02}, // DYNAMIC_SEQRAM_1B8
    {0x41BA, 2, 0x05F0}, // DYNAMIC_SEQRAM_1BA
    {0x41BC, 2, 0x0036}, // DYNAMIC_SEQRAM_1BC
    {0x41BE, 2, 0xD8F0}, // DYNAMIC_SEQRAM_1B3
    {0x41C0, 2, 0x0083}, // DYNAMIC_SEQRAM_1C0
    {0x41C2, 2, 0xF000}, // DYNAMIC_SEQRAM_1C2
    {0x41C4, 2, 0x87F0}, // DYNAMIC_SEQRAM_1C4
    {0x41C6, 2, 0x0087}, // DYNAMIC_SEQRAM_1C6
    {0x41C8, 2, 0xF000}, // DYNAMIC_SEQRAM_1C8
    {0x41CA, 2, 0x36D8}, // DYNAMIC_SEQRAM_1CA
    {0x41CC, 2, 0x020D}, // DYNAMIC_SEQRAM_1CC
    {0x41CE, 2, 0x0205}, // DYNAMIC_SEQRAM_1CE
    {0x41D0, 2, 0xF000}, // DYNAMIC_SEQRAM_1D0
    {0x41D2, 2, 0x36D8}, // DYNAMIC_SEQRAM_1D2
    {0x41D4, 2, 0xF000}, // DYNAMIC_SEQRAM_1D4
    {0x41D6, 2, 0x83F0}, // DYNAMIC_SEQRAM_1D6
    {0x41D8, 2, 0x0087}, // DYNAMIC_SEQRAM_1D8
    {0x41DA, 2, 0xF000}, // DYNAMIC_SEQRAM_1DA
    {0x41DC, 2, 0x8713}, // DYNAMIC_SEQRAM_1DC
    {0x41DE, 2, 0x0036}, // DYNAMIC_SEQRAM_1DE
    {0x41E0, 2, 0xD802}, // DYNAMIC_SEQRAM_1E0
    {0x41E2, 2, 0x0DE0}, // DYNAMIC_SEQRAM_1E2
    {0x41E4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_1E4
    {0x41E6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_1E6
    {0x41E8, 2, 0x9F13}, // DYNAMIC_SEQRAM_1E8
    {0x41EA, 2, 0x0041}, // DYNAMIC_SEQRAM_1EA
    {0x41EC, 2, 0x80F3}, // DYNAMIC_SEQRAM_1EC
    {0x41EE, 2, 0xF213}, // DYNAMIC_SEQRAM_1EE
    {0x41F0, 2, 0x00F0}, // DYNAMIC_SEQRAM_1F0
    {0x41F2, 2, 0x13B8}, // DYNAMIC_SEQRAM_1F2
    {0x41F4, 2, 0xF04C}, // DYNAMIC_SEQRAM_1F4
    {0x41F6, 2, 0x9FF0}, // DYNAMIC_SEQRAM_1F6
    {0x41F8, 2, 0x00B7}, // DYNAMIC_SEQRAM_1F8
    {0x41FA, 2, 0xF006}, // DYNAMIC_SEQRAM_1FA
    {0x41FC, 2, 0x0035}, // DYNAMIC_SEQRAM_1FC
    {0x41FE, 2, 0x10AF}, // DYNAMIC_SEQRAM_1FE
    {0x4200, 2, 0x3003}, // DYNAMIC_SEQRAM_200
    {0x4202, 2, 0x30C0}, // DYNAMIC_SEQRAM_202
    {0x4204, 2, 0xB2F0}, // DYNAMIC_SEQRAM_204
    {0x4206, 2, 0x01B5}, // DYNAMIC_SEQRAM_206
    {0x4208, 2, 0xF001}, // DYNAMIC_SEQRAM_208
    {0x420A, 2, 0x85F0}, // DYNAMIC_SEQRAM_20A
    {0x420C, 2, 0x0292}, // DYNAMIC_SEQRAM_20C
    {0x420E, 2, 0xF000}, // DYNAMIC_SEQRAM_20E
    {0x4210, 2, 0x9A8B}, // DYNAMIC_SEQRAM_210
    {0x4212, 2, 0xF000}, // DYNAMIC_SEQRAM_212
    {0x4214, 2, 0x9997}, // DYNAMIC_SEQRAM_214
    {0x4216, 2, 0xF007}, // DYNAMIC_SEQRAM_216
    {0x4218, 2, 0xB6F0}, // DYNAMIC_SEQRAM_218
    {0x421A, 2, 0x0020}, // DYNAMIC_SEQRAM_21A
    {0x421C, 2, 0x5830}, // DYNAMIC_SEQRAM_21C
    {0x421E, 2, 0xC040}, // DYNAMIC_SEQRAM_21E
    {0x4220, 2, 0x1282}, // DYNAMIC_SEQRAM_220
    {0x4222, 2, 0xF005}, // DYNAMIC_SEQRAM_222
    {0x4224, 2, 0x9CF0}, // DYNAMIC_SEQRAM_224
    {0x4226, 2, 0x01B2}, // DYNAMIC_SEQRAM_226
    {0x4228, 2, 0xF008}, // DYNAMIC_SEQRAM_228
    {0x422A, 2, 0xB8F0}, // DYNAMIC_SEQRAM_22A
    {0x422C, 2, 0x0799}, // DYNAMIC_SEQRAM_22C
    {0x422E, 2, 0xF005}, // DYNAMIC_SEQRAM_22E
    {0x4230, 2, 0x98F0}, // DYNAMIC_SEQRAM_230
    {0x4232, 2, 0x0296}, // DYNAMIC_SEQRAM_232
    {0x4234, 2, 0xA2F0}, // DYNAMIC_SEQRAM_234
    {0x4236, 2, 0x00A2}, // DYNAMIC_SEQRAM_236
    {0x4238, 2, 0xF008}, // DYNAMIC_SEQRAM_238
    {0x423A, 2, 0x9DF0}, // DYNAMIC_SEQRAM_23A
    {0x423C, 2, 0x02A1}, // DYNAMIC_SEQRAM_23C
    {0x423E, 2, 0xF01F}, // DYNAMIC_SEQRAM_23E
    {0x4240, 2, 0x1009}, // DYNAMIC_SEQRAM_240
    {0x4242, 2, 0x2220}, // DYNAMIC_SEQRAM_242
    {0x4244, 2, 0x0808}, // DYNAMIC_SEQRAM_244
    {0x4246, 2, 0xF000}, // DYNAMIC_SEQRAM_246
    {0x4248, 2, 0x3600}, // DYNAMIC_SEQRAM_248
    {0x424A, 2, 0xF000}, // DYNAMIC_SEQRAM_24A
    {0x424C, 2, 0x88F0}, // DYNAMIC_SEQRAM_24C
    {0x424E, 2, 0x0788}, // DYNAMIC_SEQRAM_24E
    {0x4250, 2, 0x3600}, // DYNAMIC_SEQRAM_250
    {0x4252, 2, 0xF000}, // DYNAMIC_SEQRAM_252
    {0x4254, 2, 0x83F0}, // DYNAMIC_SEQRAM_254
    {0x4256, 2, 0x0290}, // DYNAMIC_SEQRAM_256
    {0x4258, 2, 0xF016}, // DYNAMIC_SEQRAM_258
    {0x425A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_25A
    {0x425C, 2, 0x11A3}, // DYNAMIC_SEQRAM_25C
    {0x425E, 2, 0xF000}, // DYNAMIC_SEQRAM_25E
    {0x4260, 2, 0xA3F0}, // DYNAMIC_SEQRAM_260
    {0x4262, 2, 0x089D}, // DYNAMIC_SEQRAM_262
    {0x4264, 2, 0xF002}, // DYNAMIC_SEQRAM_264
    {0x4266, 2, 0xA1F0}, // DYNAMIC_SEQRAM_266
    {0x4268, 2, 0x20A1}, // DYNAMIC_SEQRAM_268
    {0x426A, 2, 0xF006}, // DYNAMIC_SEQRAM_26A
    {0x426C, 2, 0x4300}, // DYNAMIC_SEQRAM_26C
    {0x426E, 2, 0xF049}, // DYNAMIC_SEQRAM_26E
    {0x4270, 2, 0x4014}, // DYNAMIC_SEQRAM_270
    {0x4272, 2, 0x8B8E}, // DYNAMIC_SEQRAM_272
    {0x4274, 2, 0x9DF0}, // DYNAMIC_SEQRAM_274
    {0x4276, 2, 0x0802}, // DYNAMIC_SEQRAM_276
    {0x4278, 2, 0x02F0}, // DYNAMIC_SEQRAM_278
    {0x427A, 2, 0x00A6}, // DYNAMIC_SEQRAM_27A
    {0x427C, 2, 0xF013}, // DYNAMIC_SEQRAM_27C
    {0x427E, 2, 0xB283}, // DYNAMIC_SEQRAM_27E
    {0x4280, 2, 0x9C36}, // DYNAMIC_SEQRAM_280
    {0x4282, 2, 0x00F0}, // DYNAMIC_SEQRAM_282
    {0x4284, 2, 0x0636}, // DYNAMIC_SEQRAM_284
    {0x4286, 2, 0x009C}, // DYNAMIC_SEQRAM_286
    {0x4288, 2, 0xF008}, // DYNAMIC_SEQRAM_288
    {0x428A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_28A
    {0x428C, 2, 0x0083}, // DYNAMIC_SEQRAM_28C
    {0x428E, 2, 0xA0F0}, // DYNAMIC_SEQRAM_28E
    {0x4290, 2, 0x0630}, // DYNAMIC_SEQRAM_290
    {0x4292, 2, 0x18F0}, // DYNAMIC_SEQRAM_292
    {0x4294, 2, 0x02A3}, // DYNAMIC_SEQRAM_294
    {0x4296, 2, 0xF000}, // DYNAMIC_SEQRAM_296
    {0x4298, 2, 0xA3F0}, // DYNAMIC_SEQRAM_298
    {0x429A, 2, 0x0243}, // DYNAMIC_SEQRAM_29A
    {0x429C, 2, 0x00F0}, // DYNAMIC_SEQRAM_29C
    {0x429E, 2, 0x049D}, // DYNAMIC_SEQRAM_29E
    {0x42A0, 2, 0xF078}, // DYNAMIC_SEQRAM_2A0
    {0x42A2, 2, 0x3018}, // DYNAMIC_SEQRAM_2A2
    {0x42A4, 2, 0xF000}, // DYNAMIC_SEQRAM_2A4
    {0x42A6, 2, 0x9D82}, // DYNAMIC_SEQRAM_2A6
    {0x42A8, 2, 0xF000}, // DYNAMIC_SEQRAM_2A8
    {0x42AA, 2, 0x9030}, // DYNAMIC_SEQRAM_2AA
    {0x42AC, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2AC
    {0x42AE, 2, 0x1130}, // DYNAMIC_SEQRAM_2AE
    {0x42B0, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2B0
    {0x42B2, 2, 0x0082}, // DYNAMIC_SEQRAM_2B2
    {0x42B4, 2, 0xF001}, // DYNAMIC_SEQRAM_2B4
    {0x42B6, 2, 0x1009}, // DYNAMIC_SEQRAM_2B6
    {0x42B8, 2, 0xF02A}, // DYNAMIC_SEQRAM_2B8
    {0x42BA, 2, 0xA2F0}, // DYNAMIC_SEQRAM_2BA
    {0x42BC, 2, 0x00A2}, // DYNAMIC_SEQRAM_2BC
    {0x42BE, 2, 0x3018}, // DYNAMIC_SEQRAM_2BE
    {0x42C0, 2, 0xF007}, // DYNAMIC_SEQRAM_2C0
    {0x42C2, 2, 0x9DF0}, // DYNAMIC_SEQRAM_2C2
    {0x42C4, 2, 0x1C8C}, // DYNAMIC_SEQRAM_2C4
    {0x42C6, 2, 0xF005}, // DYNAMIC_SEQRAM_2C6
    {0x42C8, 2, 0x301F}, // DYNAMIC_SEQRAM_2C8
    {0x42CA, 2, 0x216D}, // DYNAMIC_SEQRAM_2CA
    {0x42CC, 2, 0x0A51}, // DYNAMIC_SEQRAM_2CC
    {0x42CE, 2, 0x1FEA}, // DYNAMIC_SEQRAM_2CE
    {0x42D0, 2, 0x8640}, // DYNAMIC_SEQRAM_2D0
    {0x42D2, 2, 0xE29F}, // DYNAMIC_SEQRAM_2D2
    {0x42D4, 2, 0xF009}, // DYNAMIC_SEQRAM_2D4
    {0x42D6, 2, 0x0005}, // DYNAMIC_SEQRAM_2D6
    {0x42D8, 2, 0xF000}, // DYNAMIC_SEQRAM_2D8
    {0x42DA, 2, 0x30C0}, // DYNAMIC_SEQRAM_2DA
    {0x42DC, 2, 0xF001}, // DYNAMIC_SEQRAM_2DC
    {0x42DE, 2, 0x83F0}, // DYNAMIC_SEQRAM_2DE
    {0x42E0, 2, 0x0036}, // DYNAMIC_SEQRAM_2E0
    {0x42E2, 2, 0x00F0}, // DYNAMIC_SEQRAM_2E2
    {0x42E4, 2, 0x0087}, // DYNAMIC_SEQRAM_2E4
    {0x42E6, 2, 0xF007}, // DYNAMIC_SEQRAM_2E6
    {0x42E8, 2, 0x87F0}, // DYNAMIC_SEQRAM_2E8
    {0x42EA, 2, 0x0036}, // DYNAMIC_SEQRAM_2EA
    {0x42EC, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2EC
    {0x42EE, 2, 0x0000}, // DYNAMIC_SEQRAM_2EE
    {0x42F0, 2, 0x0DF0}, // DYNAMIC_SEQRAM_2F0
    {0x42F2, 2, 0x0000}, // DYNAMIC_SEQRAM_2F2
    {0x42F4, 2, 0x05F0}, // DYNAMIC_SEQRAM_2F4
    {0x42F6, 2, 0x0030}, // DYNAMIC_SEQRAM_2F6
    {0x42F8, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2F8
    {0x42FA, 2, 0x0183}, // DYNAMIC_SEQRAM_2FA
    {0x42FC, 2, 0xF000}, // DYNAMIC_SEQRAM_2FC
    {0x42FE, 2, 0x3600}, // DYNAMIC_SEQRAM_2FE
    {0x4300, 2, 0xF000}, // DYNAMIC_SEQRAM_300
    {0x4302, 2, 0x87F0}, // DYNAMIC_SEQRAM_302
    {0x4304, 2, 0x0787}, // DYNAMIC_SEQRAM_304
    {0x4306, 2, 0xF000}, // DYNAMIC_SEQRAM_306
    {0x4308, 2, 0x36C0}, // DYNAMIC_SEQRAM_308
    {0x430A, 2, 0xF000}, // DYNAMIC_SEQRAM_30A
    {0x430C, 2, 0x000F}, // DYNAMIC_SEQRAM_30C
    {0x430E, 2, 0xF42A}, // DYNAMIC_SEQRAM_30E
    {0x4310, 2, 0x4180}, // DYNAMIC_SEQRAM_310
    {0x4312, 2, 0x1300}, // DYNAMIC_SEQRAM_312
    {0x4314, 2, 0x9FF0}, // DYNAMIC_SEQRAM_314
    {0x4316, 2, 0x00E0}, // DYNAMIC_SEQRAM_316
    {0x4318, 2, 0xE0E0}, // DYNAMIC_SEQRAM_318
    {0x431A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31A
    {0x431C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31C
    {0x431E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31E
    {0x4320, 2, 0xE0E0}, // DYNAMIC_SEQRAM_320
    {0x4322, 2, 0xE0E0}, // DYNAMIC_SEQRAM_322
    {0x4324, 2, 0xE0E0}, // DYNAMIC_SEQRAM_324
    {0x4326, 2, 0xE0E0}, // DYNAMIC_SEQRAM_326
    {0x4328, 2, 0xE0E0}, // DYNAMIC_SEQRAM_328
    {0x432A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32A
    {0x432C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32C
    {0x432E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32E
    {0x4330, 2, 0xE0E0}, // DYNAMIC_SEQRAM_330
    {0x4332, 2, 0xE0E0}, // DYNAMIC_SEQRAM_332
    {0x4334, 2, 0xE0E0}, // DYNAMIC_SEQRAM_334
    {0x4336, 2, 0xE0E0}, // DYNAMIC_SEQRAM_336
    {0x4338, 2, 0xE0E0}, // DYNAMIC_SEQRAM_338
    {0x433A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33A
    {0x433C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33C
    {0x433E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33E
    {0x4340, 2, 0xE0E0}, // DYNAMIC_SEQRAM_340
    {0x4342, 2, 0xE0E0}, // DYNAMIC_SEQRAM_342
    {0x4344, 2, 0xE0E0}, // DYNAMIC_SEQRAM_344
    {0x4346, 2, 0xE0E0}, // DYNAMIC_SEQRAM_346
    {0x4348, 2, 0xE0E0}, // DYNAMIC_SEQRAM_348
    {0x434A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34A
    {0x434C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34C
    {0x434E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34E
    {0x4350, 2, 0xE0E0}, // DYNAMIC_SEQRAM_350
    {0x4352, 2, 0xE0E0}, // DYNAMIC_SEQRAM_352
    {0x4354, 2, 0xE0E0}, // DYNAMIC_SEQRAM_354
    {0x4356, 2, 0xE0E0}, // DYNAMIC_SEQRAM_356
    {0x4358, 2, 0xE0E0}, // DYNAMIC_SEQRAM_358
    {0x435A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35A
    {0x435C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35C
    {0x435E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35E
    {0x4360, 2, 0xE0E0}, // DYNAMIC_SEQRAM_360
    {0x4362, 2, 0xE0E0}, // DYNAMIC_SEQRAM_362
    {0x4364, 2, 0xE0E0}, // DYNAMIC_SEQRAM_364
    {0x4366, 2, 0xE0E0}, // DYNAMIC_SEQRAM_366
    {0x4368, 2, 0xE0E0}, // DYNAMIC_SEQRAM_368
    {0x436A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36A
    {0x436C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36C
    {0x436E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36E
    {0x4370, 2, 0xE0E0}, // DYNAMIC_SEQRAM_370
    {0x4372, 2, 0xE0E0}, // DYNAMIC_SEQRAM_372
    {0x4374, 2, 0xE0E0}, // DYNAMIC_SEQRAM_374
    {0x4376, 2, 0xE0E0}, // DYNAMIC_SEQRAM_376
    {0x4378, 2, 0xE0E0}, // DYNAMIC_SEQRAM_378
    {0x437A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37A
    {0x437C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37C
    {0x437E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37E
    {0x4380, 2, 0xE0E0}, // DYNAMIC_SEQRAM_380
    {0x4382, 2, 0xE0E0}, // DYNAMIC_SEQRAM_382
    {0x4384, 2, 0xE0E0}, // DYNAMIC_SEQRAM_384
    {0x4386, 2, 0xE0E0}, // DYNAMIC_SEQRAM_386
    {0x4388, 2, 0xE0E0}, // DYNAMIC_SEQRAM_388
    {0x438A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38A
    {0x438C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38C
    {0x438E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38E
    {0x4390, 2, 0xE0E0}, // DYNAMIC_SEQRAM_390
    {0x4392, 2, 0xE0E0}, // DYNAMIC_SEQRAM_392
    {0x4394, 2, 0xE0E0}, // DYNAMIC_SEQRAM_394
    {0x4396, 2, 0xE0E0}, // DYNAMIC_SEQRAM_396
    {0x4398, 2, 0xE0E0}, // DYNAMIC_SEQRAM_398
    {0x439A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39A
    {0x439C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39C
    {0x439E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39E
    {0x43A0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A0
    {0x43A2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A2
    {0x43A4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A4
    {0x43A6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A6
    {0x43A8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A8
    {0x43AA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AA
    {0x43AC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AC
    {0x43AE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AE
    {0x43B0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B0
    {0x43B2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B2
    {0x43B4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B4
    {0x43B6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B6
    {0x43B8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B8
    {0x43BA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BA
    {0x43BC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BC
    {0x43BE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BE
    {0x43C0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C0
    {0x43C2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C2
    {0x43C4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C4
    {0x43C6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C6
    {0x43C8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C8
    {0x43CA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CA
    {0x43CC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CC
    {0x43CE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CE
    {0x43D0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D0
    {0x43D2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D2
    {0x43D4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D4
    {0x43D6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D6
    {0x43D8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D8
    {0x43DA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DA
    {0x43DC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DC
    {0x43DE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DE
    {0x43E0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E0
    {0x43E2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E2
    {0x43E4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E4
    {0x43E6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E6
    {0x43E8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E8
    {0x43EA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EA
    {0x43EC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EC
    {0x43EE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EE
    {0x43F0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F0
    {0x43F2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F2
    {0x43F4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F4
    {0x43F6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F6
    {0x43F8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F8
    {0x43FA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FA
    {0x43FC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FC
    {0x43FE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FE
    {0x4400, 2, 0xE0E0}, // DYNAMIC_SEQRAM_400
    {0x4402, 2, 0xE0E0}, // DYNAMIC_SEQRAM_402
    {0x4404, 2, 0xE0E0}, // DYNAMIC_SEQRAM_404
    {0x4406, 2, 0xE0E0}, // DYNAMIC_SEQRAM_406
    {0x4408, 2, 0xE0E0}, // DYNAMIC_SEQRAM_408
    {0x440A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40A
    {0x440C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40C
    {0x440E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40E
    {0x4410, 2, 0xE0E0}, // DYNAMIC_SEQRAM_410
    {0x4412, 2, 0xE0E0}, // DYNAMIC_SEQRAM_412
    {0x4414, 2, 0xE0E0}, // DYNAMIC_SEQRAM_414
    {0x4416, 2, 0xE0E0}, // DYNAMIC_SEQRAM_416
    {0x4418, 2, 0xE0E0}, // DYNAMIC_SEQRAM_418
    {0x441A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41A
    {0x441C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41C
    {0x441E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41E
    {0x4420, 2, 0xE0E0}, // DYNAMIC_SEQRAM_420
    {0x4422, 2, 0xE0E0}, // DYNAMIC_SEQRAM_422
    {0x4424, 2, 0xE0E0}, // DYNAMIC_SEQRAM_424
    {0x4426, 2, 0xE0E0}, // DYNAMIC_SEQRAM_426
    {0x4428, 2, 0xE0E0}, // DYNAMIC_SEQRAM_428
    {0x442A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42A
    {0x442C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42C
    {0x442E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42E
    {0x4430, 2, 0xE0E0}, // DYNAMIC_SEQRAM_430
    {0x4432, 2, 0xE0E0}, // DYNAMIC_SEQRAM_432
    {0x4434, 2, 0xE0E0}, // DYNAMIC_SEQRAM_434
    {0x4436, 2, 0xE0E0}, // DYNAMIC_SEQRAM_436
    {0x4438, 2, 0xE0E0}, // DYNAMIC_SEQRAM_438
    {0x443A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43A
    {0x443C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43C
    {0x443E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43E
    {0x4440, 2, 0xE0E0}, // DYNAMIC_SEQRAM_440
    {0x4442, 2, 0xE0E0}, // DYNAMIC_SEQRAM_442
    {0x4444, 2, 0xE0E0}, // DYNAMIC_SEQRAM_444
    {0x4446, 2, 0xE0E0}, // DYNAMIC_SEQRAM_446
    {0x4448, 2, 0xE0E0}, // DYNAMIC_SEQRAM_448
    {0x444A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44A
    {0x444C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44C
    {0x444E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44E
    {0x4450, 2, 0xE0E0}, // DYNAMIC_SEQRAM_450
    {0x4452, 2, 0xE0E0}, // DYNAMIC_SEQRAM_452
    {0x4454, 2, 0xE0E0}, // DYNAMIC_SEQRAM_454
    {0x4456, 2, 0xE0E0}, // DYNAMIC_SEQRAM_456
    {0x4458, 2, 0xE0E0}, // DYNAMIC_SEQRAM_458
    {0x445A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45A
    {0x445C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45C
    {0x445E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45E
    {0x4460, 2, 0xE0E0}, // DYNAMIC_SEQRAM_460
    {0x4462, 2, 0xE0E0}, // DYNAMIC_SEQRAM_462
    {0x4464, 2, 0xE0E0}, // DYNAMIC_SEQRAM_464
    {0x4466, 2, 0xE0E0}, // DYNAMIC_SEQRAM_466
    {0x4468, 2, 0xE0E0}, // DYNAMIC_SEQRAM_468
    {0x446A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46A
    {0x446C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46C
    {0x446E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46E
    {0x4470, 2, 0xE0E0}, // DYNAMIC_SEQRAM_470
    {0x4472, 2, 0xE0E0}, // DYNAMIC_SEQRAM_472
    {0x4474, 2, 0xE0E0}, // DYNAMIC_SEQRAM_474
    {0x4476, 2, 0xE0E0}, // DYNAMIC_SEQRAM_476
    {0x4478, 2, 0xE0E0}, // DYNAMIC_SEQRAM_478
    {0x447A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47A
    {0x447C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47C
    {0x447E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47E
    {0x4480, 2, 0xE0E0}, // DYNAMIC_SEQRAM_480
    {0x4482, 2, 0xE0E0}, // DYNAMIC_SEQRAM_482
    {0x4484, 2, 0xE0E0}, // DYNAMIC_SEQRAM_484
    {0x4486, 2, 0xE0E0}, // DYNAMIC_SEQRAM_486
    {0x4488, 2, 0xE0E0}, // DYNAMIC_SEQRAM_488
    {0x448A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48A
    {0x448C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48C
    {0x448E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48E
    {0x4490, 2, 0xE0E0}, // DYNAMIC_SEQRAM_490
    {0x4492, 2, 0xE0E0}, // DYNAMIC_SEQRAM_492
    {0x4494, 2, 0xE0E0}, // DYNAMIC_SEQRAM_494
    {0x4496, 2, 0xE0E0}, // DYNAMIC_SEQRAM_496
    {0x4498, 2, 0xE0E0}, // DYNAMIC_SEQRAM_498
    {0x449A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49A
    {0x449C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49C
    {0x449E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49E
    {0x44A0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A0
    {0x44A2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A2
    {0x44A4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A4
    {0x44A6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A6
    {0x44A8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A8
    {0x44AA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AA
    {0x44AC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AC
    {0x44AE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AE
    {0x44B0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B0
    {0x44B2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B2
    {0x44B4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B4
    {0x5500, 2, 0x0000}, // AGAIN_LUT0
    {0x5502, 2, 0x0002}, // AGAIN_LUT1
    {0x5504, 2, 0x0006}, // AGAIN_LUT2
    {0x5506, 2, 0x0009}, // AGAIN_LUT3
    {0x5508, 2, 0x000F}, // AGAIN_LUT4
    {0x550A, 2, 0x0010}, // AGAIN_LUT5
    {0x550C, 2, 0x0011}, // AGAIN_LUT6
    {0x550E, 2, 0x0012}, // AGAIN_LUT7
    {0x5510, 2, 0x0019}, // AGAIN_LUT8
    {0x5512, 2, 0x0020}, // AGAIN_LUT9
    {0x5514, 2, 0x0021}, // AGAIN_LUT10
    {0x5516, 2, 0x0023}, // AGAIN_LUT11
    {0x5518, 2, 0x0026}, // AGAIN_LUT12
    {0x551A, 2, 0x002B}, // AGAIN_LUT13
    {0x551C, 2, 0x002F}, // AGAIN_LUT14
    {0x551E, 2, 0x0030}, // AGAIN_LUT15
    {0x5400, 2, 0x0100}, // GT1_COARSE0
    {0x5402, 2, 0x2106}, // GT1_COARSE1
    {0x5404, 2, 0x1101}, // GT1_COARSE2
    {0x5406, 2, 0x3106}, // GT1_COARSE3
    {0x5408, 2, 0x7100}, // GT1_COARSE4
    {0x540A, 2, 0x8107}, // GT1_COARSE5
    {0x540C, 2, 0xB101}, // GT1_COARSE6
    {0x540E, 2, 0xD101}, // GT1_COARSE7
    {0x5410, 2, 0xF12E}, // GT1_COARSE8
    {0x5412, 2, 0xF112}, // GT1_COARSE9
    {0x5414, 2, 0xF184}, // GT1_COARSE10
    {0x5416, 2, 0xF224}, // GT1_COARSE11
    {0x5418, 2, 0xF306}, // GT1_COARSE12
    {0x541A, 2, 0xF446}, // GT1_COARSE13
    {0x541C, 2, 0xF609}, // GT1_COARSE14
    {0x541E, 2, 0xF887}, // GT1_COARSE15
    {0x5420, 2, 0xFC0B}, // GT1_COARSE16
    {0x5422, 2, 0xFC0B}, // GT1_COARSE17
    {0x5424, 2, 0xFFFA}, // GT1_DCG_ATTN_SET0
    {0x5426, 2, 0x5557}, // GT1_DCG_ATTN_SET1
    {0x5428, 2, 0x0005}, // GT1_DCG_ATTN_SET2
    {0x542A, 2, 0xA550}, // GT1_ZONE_SET0
    {0x542C, 2, 0xAAAA}, // GT1_ZONE_SET1
    {0x542E, 2, 0x000A}, // GT1_ZONE_SET2
    {0x5460, 2, 0x2269}, // ZT1_REG0_ADDR
    {0x5462, 2, 0x0B87}, // ZT1_REG0_VALUE0
    {0x5464, 2, 0x0B87}, // ZT1_REG0_VALUE1
    {0x5466, 2, 0x0983}, // ZT1_REG0_VALUE2
    {0x5498, 2, 0x225E}, // ZT1_REG7_ADDR
    {0x549A, 2, 0xBCAA}, // ZT1_REG7_VALUE0
    {0x549C, 2, 0xBCAA}, // ZT1_REG7_VALUE1
    {0x549E, 2, 0xBDAA}, // ZT1_REG7_VALUE2
    {0x3060, 2, 0xFF01}, // GAIN_TABLE_CTRL
    {0x44BA, 2, 0x0050}, // DAC_LD_4_5
    {0x44BC, 2, 0xBCAA}, // DAC_LD_6_7
    {0x44C0, 2, 0x4070}, // DAC_LD_10_11
    {0x44C4, 2, 0x04D0}, // DAC_LD_14_15
    {0x44C6, 2, 0x17E2}, // DAC_LD_16_17
    {0x44C8, 2, 0xEA43}, // DAC_LD_18_19
    {0x44CA, 2, 0x000E}, // DAC_LD_20_21
    {0x44CC, 2, 0x7777}, // DAC_LD_22_23
    {0x44CE, 2, 0x8BA4}, // DAC_LD_24_25
    {0x44D0, 2, 0x1735}, // DAC_LD_26_27
    {0x44D2, 2, 0x0B87}, // DAC_LD_28_29
    {0x44D4, 2, 0x8000}, // DAC_LD_30_31
    {0x44D6, 2, 0xF206}, // DAC_LD_32_33
    {0x44D8, 2, 0xAAFA}, // DAC_LD_34_35
    {0x44DA, 2, 0xE001}, // DAC_LD_36_37
    {0x44DE, 2, 0x9BBC}, // DAC_LD_40_41
    {0x44E0, 2, 0x283C}, // DAC_LD_42_43
    {0x44E2, 2, 0x2821}, // DAC_LD_44_45
    {0x44E4, 2, 0x8000}, // DAC_LD_46_47
    {0x44E6, 2, 0x503F}, // DAC_LD_48_49
    {0x32A4, 2, 0x0000}, // CRM_CTRL
    {0x333C, 2, 0x0001}, // DYNAMIC_CTRL
    {0x301A, 2, 0x0000}, // RESET_REGISTER
    {0x3600, 2, 0x94DF}, // FDOC_CTRL
    {0x3700, 2, 0x0001}, // PIX_DEF_ID
    {0x3980, 2, 0x0003}, // PIX_DEF_CORR
    {0x36C0, 2, 0x0001}, // DIGITAL_GAIN_CTRL
    {0x36DE, 2, 0x002A}, // DATA_PEDESTAL1
    {0x301A, 2, 0x0008}, // RESET_REGISTER
    {0x3060, 2, 0x0000}, // GAIN_TABLE_CTRL
    {0x3060, 2, 0xFF01}, // GAIN_TABLE_CTRL
    {0x3980, 2, 0x0001}, // PIX_DEF_CORR
    {0x3340, 2, 0x0C00}, // OTPM_CTRL
    {0x3340, 2, 0x1C00}, // OTPM_CTRL
    {0x36C0, 2, 0x0001}, // DIGITAL_GAIN_CTRL
    {0x0400, 2, 0x0003}, // SCALING_MODE
    {0x0404, 2, 0x0C10}, // SCALE_M
    {0x0406, 2, 0x0400}, // SCALE_N
    {0x3C58, 2, 0x0008}, // SLICE_OVERLAP
    {0x3C40, 2, 0x0002}, // SCALE_CTRL
    {0x3C42, 2, 0x0880}, // SCALE_SECOND_RESIDUAL_EVEN
    {0x3C44, 2, 0x0078}, // SCALE_SECOND_RESIDUAL_ODD
    {0x3C46, 2, 0x090A}, // SCALE_SECOND_CROP
    {0x3C48, 2, 0x0140}, // SCALE_THIRD_RESIDUAL_EVEN
    {0x3C4A, 2, 0x0548}, // SCALE_THIRD_RESIDUAL_ODD
    {0x3C4C, 2, 0x0909}, // SCALE_THIRD_CROP
    {0x3C4E, 2, 0x0610}, // SCALE_FOURTH_RESIDUAL_EVEN
    {0x3C50, 2, 0x0A18}, // SCALE_FOURTH_RESIDUAL_ODD
    {0x3C52, 2, 0x0808}, // SCALE_FOURTH_CROP
    {0x3C60, 2, 0x0120}, // SCALE_FIRST_PXL_NUM
    {0x3C62, 2, 0x0118}, // SCALE_SECOND_PXL_NUM
    {0x3C64, 2, 0x0118}, // SCALE_THIRD_PXL_NUM
    {0x3C66, 2, 0x0114}, // SCALE_FOURTH_PXL_NUM
    {0x3EC8, 2, 0x0000}, // X_OUTPUT_OFFSET
    {0x3ECA, 2, 0x0000}, // Y_OUTPUT_OFFSET
    {0x034C, 2, 0x0460}, // X_OUTPUT_SIZE
    {0x034E, 2, 0x0348}, // Y_OUTPUT_SIZE
    {0x3C7A, 2, 0x0000}, // REORDER_CTRL
    {0x3C6A, 2, 0x0590}, // X_OUTPUT_OFFSET2
    {0x3C70, 2, 0x0430}, // Y_OUTPUT_OFFSET2
    {0x3C6C, 2, 0x0220}, // X_OUTPUT_SIZE2
    {0x3C72, 2, 0x0198}, // Y_OUTPUT_SIZE2
    {0x3C74, 2, 0x0003}, // SYNC_REGEN
    {0x3C76, 2, 0x0020}, // SYNC_REGEN_BLANK
    {0x3F18, 2, 0x7B70}, // MIPI_JPEG_PN9_DATA_TYPE
    {0x3F1A, 2, 0x102B}, // MIPI_DATA_TYPE_INTERLEAVE

    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE
    {0x0100, 1, 0x01}, // MODE_SELECT
    {0x44D6, 2, 0xB206}, // DAC_LD_32_33

};
#endif

#if 1
const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_1024x1024_1024x948_table[] =
{
    {0x0103, 0x3201, 0x01}, // SOFTWARE_RESET

    {0x0304, 2, 0x0002}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 0x0056}, // VT_PLL_MULTIPLIER
    {0x0300, 2, 0x0006}, // VT_PIX_CLK_DIV
    {0x0302, 2, 0x0001}, // VT_SYS_CLK_DIV
    {0x030C, 2, 0x0007}, // OP_PRE_PLL_CLK_DIV
    {0x030E, 2, 0x01a0}, //{0x030E, 2, 0x020D}, // OP_PLL_MULTIPLIER
    {0x0308, 2, 0x000A}, // OP_PIX_CLK_DIV
    {0x030A, 2, 0x0001}, // OP_SYS_CLK_DIV
    {0x0344, 2, 0x04C2}, // X_ADDR_START
    {0x0348, 2, 0x0F4D}, // X_ADDR_END
    {0x0346, 2, 0x02A6}, // Y_ADDR_START
    {0x034A, 2, 0x0C69}, // Y_ADDR_END
    {0x034C, 2, 0x0A8C}, // X_OUTPUT_SIZE
    {0x034E, 2, 0x09C4}, // Y_OUTPUT_SIZE
    {0x0380, 2, 0x0001}, // X_EVEN_INC
    {0x0382, 2, 0x0001}, // X_ODD_INC
    {0x0384, 2, 0x0001}, // Y_EVEN_INC
    {0x0386, 2, 0x0001}, // Y_ODD_INC

    {0x0900, 1, 0x00}, // BINNING_MODE
    {0x0901, 1, 0x11}, // BINNING_TYPE

    {0x0342, 2, 0x16E0}, // LINE_LENGTH_PCK
    {0x0340, 2, 0x0a14}, //{0x0340, 2, 0x0a14}, //{0x0340, 2, 0x0A0E}, // FRAME_LENGTH_LINES
    {0x0202, 2, 0x03e8}, //{0x0202, 2, 0x0494}, // COARSE_INTEGRATION_TIME

    {0x0112, 2, 0x0A0A}, // CSI_DATA_FORMAT
    {0x0114, 1, 0x03}, // CSI_LANE_MODE
    {0x0800, 1, 0x0C}, // TCLK_POST
    {0x0801, 1, 0x06}, // THS_PREPARE
    {0x0802, 1, 0x0B}, // THS_ZERO_MIN
    {0x0803, 1, 0x08}, // THS_TRAIL
    {0x0804, 1, 0x0A}, // TCLK_TRAIL_MIN
    {0x0805, 1, 0x07}, // TCLK_PREPARE
    {0x0806, 1, 0x21}, // TCLK_ZERO
    {0x0807, 1, 0x07}, // TLPX
    {0x082A, 1, 0x0E}, // TWAKEUP
    {0x082B, 1, 0x0B}, // TINIT
    {0x082C, 1, 0x0C}, // THS_EXIT
    {0x3F06, 2, 0x00C0}, // MIPI_TIMING_2
    {0x3F0A, 2, 0x0000}, //{0x3F0A, 2, 0x8000}, // MIPI_TIMING_4
    {0x3F0C, 2, 0x0009}, // MIPI_TIMING_5
    {0x3F20, 2, 0x0808}, // MIPI_PHY_TRIM_MSB
    {0x3F02, 2, 0x0002}, // PHY_CTRL


    {0x4000, 2, 0x0114}, // DYNAMIC_SEQRAM_00
    {0x4002, 2, 0x1A25}, // DYNAMIC_SEQRAM_02
    {0x4004, 2, 0x3DFF}, // DYNAMIC_SEQRAM_04
    {0x4006, 2, 0xFFFF}, // DYNAMIC_SEQRAM_06
    {0x4008, 2, 0x0A35}, // DYNAMIC_SEQRAM_08
    {0x400A, 2, 0x10EF}, // DYNAMIC_SEQRAM_0A
    {0x400C, 2, 0x3003}, // DYNAMIC_SEQRAM_0C
    {0x400E, 2, 0x30D8}, // DYNAMIC_SEQRAM_0E
    {0x4010, 2, 0xF003}, // DYNAMIC_SEQRAM_10
    {0x4012, 2, 0xB5F0}, // DYNAMIC_SEQRAM_12
    {0x4014, 2, 0x0085}, // DYNAMIC_SEQRAM_14
    {0x4016, 2, 0xF004}, // DYNAMIC_SEQRAM_16
    {0x4018, 2, 0x9A89}, // DYNAMIC_SEQRAM_18
    {0x401A, 2, 0xF000}, // DYNAMIC_SEQRAM_1A
    {0x401C, 2, 0x9997}, // DYNAMIC_SEQRAM_1C
    {0x401E, 2, 0xF000}, // DYNAMIC_SEQRAM_1E
    {0x4020, 2, 0x30C0}, // DYNAMIC_SEQRAM_20
    {0x4022, 2, 0xF000}, // DYNAMIC_SEQRAM_22
    {0x4024, 2, 0x82F0}, // DYNAMIC_SEQRAM_24
    {0x4026, 2, 0x0030}, // DYNAMIC_SEQRAM_26
    {0x4028, 2, 0x18F0}, // DYNAMIC_SEQRAM_28
    {0x402A, 2, 0x0320}, // DYNAMIC_SEQRAM_2A
    {0x402C, 2, 0x58F0}, // DYNAMIC_SEQRAM_2C
    {0x402E, 2, 0x089C}, // DYNAMIC_SEQRAM_2E
    {0x4030, 2, 0xF010}, // DYNAMIC_SEQRAM_30
    {0x4032, 2, 0x99B6}, // DYNAMIC_SEQRAM_32
    {0x4034, 2, 0xF003}, // DYNAMIC_SEQRAM_34
    {0x4036, 2, 0xB498}, // DYNAMIC_SEQRAM_36
    {0x4038, 2, 0xA096}, // DYNAMIC_SEQRAM_38
    {0x403A, 2, 0xF000}, // DYNAMIC_SEQRAM_3A
    {0x403C, 2, 0xA2F0}, // DYNAMIC_SEQRAM_3C
    {0x403E, 2, 0x00A2}, // DYNAMIC_SEQRAM_3E
    {0x4040, 2, 0xF008}, // DYNAMIC_SEQRAM_40
    {0x4042, 2, 0x9DF0}, // DYNAMIC_SEQRAM_42
    {0x4044, 2, 0x209D}, // DYNAMIC_SEQRAM_44
    {0x4046, 2, 0x8C08}, // DYNAMIC_SEQRAM_46
    {0x4048, 2, 0x08F0}, // DYNAMIC_SEQRAM_48
    {0x404A, 2, 0x0036}, // DYNAMIC_SEQRAM_4A
    {0x404C, 2, 0x008F}, // DYNAMIC_SEQRAM_4C
    {0x404E, 2, 0xF000}, // DYNAMIC_SEQRAM_4E
    {0x4050, 2, 0x88F0}, // DYNAMIC_SEQRAM_50
    {0x4052, 2, 0x0488}, // DYNAMIC_SEQRAM_52
    {0x4054, 2, 0xF000}, // DYNAMIC_SEQRAM_54
    {0x4056, 2, 0x3600}, // DYNAMIC_SEQRAM_56
    {0x4058, 2, 0xF000}, // DYNAMIC_SEQRAM_58
    {0x405A, 2, 0x83F0}, // DYNAMIC_SEQRAM_5A
    {0x405C, 2, 0x0290}, // DYNAMIC_SEQRAM_5C
    {0x405E, 2, 0xF000}, // DYNAMIC_SEQRAM_5E
    {0x4060, 2, 0x8BF0}, // DYNAMIC_SEQRAM_60
    {0x4062, 2, 0x2EA3}, // DYNAMIC_SEQRAM_62
    {0x4064, 2, 0xF000}, // DYNAMIC_SEQRAM_64
    {0x4066, 2, 0xA3F0}, // DYNAMIC_SEQRAM_66
    {0x4068, 2, 0x089D}, // DYNAMIC_SEQRAM_68
    {0x406A, 2, 0xF075}, // DYNAMIC_SEQRAM_6A
    {0x406C, 2, 0x3003}, // DYNAMIC_SEQRAM_6C
    {0x406E, 2, 0x4070}, // DYNAMIC_SEQRAM_6E
    {0x4070, 2, 0x216D}, // DYNAMIC_SEQRAM_70
    {0x4072, 2, 0x1CF6}, // DYNAMIC_SEQRAM_72
    {0x4074, 2, 0x8B00}, // DYNAMIC_SEQRAM_74
    {0x4076, 2, 0x5186}, // DYNAMIC_SEQRAM_76
    {0x4078, 2, 0x1300}, // DYNAMIC_SEQRAM_78
    {0x407A, 2, 0x0205}, // DYNAMIC_SEQRAM_7A
    {0x407C, 2, 0x36D8}, // DYNAMIC_SEQRAM_7C
    {0x407E, 2, 0xF002}, // DYNAMIC_SEQRAM_7E
    {0x4080, 2, 0x8387}, // DYNAMIC_SEQRAM_80
    {0x4082, 2, 0xF006}, // DYNAMIC_SEQRAM_82
    {0x4084, 2, 0x8702}, // DYNAMIC_SEQRAM_84
    {0x4086, 2, 0x0D02}, // DYNAMIC_SEQRAM_86
    {0x4088, 2, 0x05F0}, // DYNAMIC_SEQRAM_88
    {0x408A, 2, 0x0383}, // DYNAMIC_SEQRAM_8A
    {0x408C, 2, 0xF001}, // DYNAMIC_SEQRAM_8C
    {0x408E, 2, 0x87F0}, // DYNAMIC_SEQRAM_8E
    {0x4090, 2, 0x0213}, // DYNAMIC_SEQRAM_90
    {0x4092, 2, 0x0036}, // DYNAMIC_SEQRAM_92
    {0x4094, 2, 0xD887}, // DYNAMIC_SEQRAM_94
    {0x4096, 2, 0x020D}, // DYNAMIC_SEQRAM_96
    {0x4098, 2, 0xE0E0}, // DYNAMIC_SEQRAM_98
    {0x409A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9A
    {0x409C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9C
    {0x409E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9E
    {0x40A0, 2, 0xF000}, // DYNAMIC_SEQRAM_A0
    {0x40A2, 2, 0x0401}, // DYNAMIC_SEQRAM_A2
    {0x40A4, 2, 0xF008}, // DYNAMIC_SEQRAM_A4
    {0x40A6, 2, 0x82F0}, // DYNAMIC_SEQRAM_A6
    {0x40A8, 2, 0x0883}, // DYNAMIC_SEQRAM_A8
    {0x40AA, 2, 0xF009}, // DYNAMIC_SEQRAM_AA
    {0x40AC, 2, 0x85F0}, // DYNAMIC_SEQRAM_AC
    {0x40AE, 2, 0x2985}, // DYNAMIC_SEQRAM_AE
    {0x40B0, 2, 0x87F0}, // DYNAMIC_SEQRAM_B0
    {0x40B2, 2, 0x2A87}, // DYNAMIC_SEQRAM_B2
    {0x40B4, 2, 0xF63E}, // DYNAMIC_SEQRAM_B4
    {0x40B6, 2, 0x88F0}, // DYNAMIC_SEQRAM_B6
    {0x40B8, 2, 0x0801}, // DYNAMIC_SEQRAM_B8
    {0x40BA, 2, 0x40F0}, // DYNAMIC_SEQRAM_BA
    {0x40BC, 2, 0x0800}, // DYNAMIC_SEQRAM_BC
    {0x40BE, 2, 0x48F0}, // DYNAMIC_SEQRAM_BE
    {0x40C0, 2, 0x0882}, // DYNAMIC_SEQRAM_C0
    {0x40C2, 2, 0xF008}, // DYNAMIC_SEQRAM_C2
    {0x40C4, 2, 0x0401}, // DYNAMIC_SEQRAM_C4
    {0x40C6, 2, 0xF008}, // DYNAMIC_SEQRAM_C6
    {0x40C8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_C8
    {0x40CA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CA
    {0x40CC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CC
    {0x40CE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CE
    {0x40D0, 2, 0xF000}, // DYNAMIC_SEQRAM_D0
    {0x40D2, 2, 0x0401}, // DYNAMIC_SEQRAM_D2
    {0x40D4, 2, 0xF015}, // DYNAMIC_SEQRAM_D4
    {0x40D6, 2, 0x003C}, // DYNAMIC_SEQRAM_D6
    {0x40D8, 2, 0xF00E}, // DYNAMIC_SEQRAM_D8
    {0x40DA, 2, 0x85F0}, // DYNAMIC_SEQRAM_DA
    {0x40DC, 2, 0x0687}, // DYNAMIC_SEQRAM_DC
    {0x40DE, 2, 0xF002}, // DYNAMIC_SEQRAM_DE
    {0x40E0, 2, 0x87F0}, // DYNAMIC_SEQRAM_E0
    {0x40E2, 2, 0x61E8}, // DYNAMIC_SEQRAM_E2
    {0x40E4, 2, 0x3904}, // DYNAMIC_SEQRAM_E4
    {0x40E6, 2, 0xF005}, // DYNAMIC_SEQRAM_E6
    {0x40E8, 2, 0x3482}, // DYNAMIC_SEQRAM_E8
    {0x40EA, 2, 0xF000}, // DYNAMIC_SEQRAM_EA
    {0x40EC, 2, 0x3241}, // DYNAMIC_SEQRAM_EC
    {0x40EE, 2, 0xF000}, // DYNAMIC_SEQRAM_EE
    {0x40F0, 2, 0x3904}, // DYNAMIC_SEQRAM_F0
    {0x40F2, 2, 0xF00E}, // DYNAMIC_SEQRAM_F2
    {0x40F4, 2, 0x3904}, // DYNAMIC_SEQRAM_F4
    {0x40F6, 2, 0xF000}, // DYNAMIC_SEQRAM_F6
    {0x40F8, 2, 0x3241}, // DYNAMIC_SEQRAM_F8
    {0x40FA, 2, 0xF000}, // DYNAMIC_SEQRAM_FA
    {0x40FC, 2, 0x3482}, // DYNAMIC_SEQRAM_FC
    {0x40FE, 2, 0xF005}, // DYNAMIC_SEQRAM_FE
    {0x4100, 2, 0xC0E6}, // DYNAMIC_SEQRAM_100
    {0x4102, 2, 0xF004}, // DYNAMIC_SEQRAM_102
    {0x4104, 2, 0x3904}, // DYNAMIC_SEQRAM_104
    {0x4106, 2, 0xF003}, // DYNAMIC_SEQRAM_106
    {0x4108, 2, 0xB0F0}, // DYNAMIC_SEQRAM_108
    {0x410A, 2, 0x0000}, // DYNAMIC_SEQRAM_10A
    {0x410C, 2, 0x18F0}, // DYNAMIC_SEQRAM_10C
    {0x410E, 2, 0x0086}, // DYNAMIC_SEQRAM_10E
    {0x4110, 2, 0xF000}, // DYNAMIC_SEQRAM_110
    {0x4112, 2, 0x86F0}, // DYNAMIC_SEQRAM_112
    {0x4114, 2, 0x89B0}, // DYNAMIC_SEQRAM_114
    {0x4116, 2, 0xF000}, // DYNAMIC_SEQRAM_116
    {0x4118, 2, 0xE9F0}, // DYNAMIC_SEQRAM_118
    {0x411A, 2, 0x008A}, // DYNAMIC_SEQRAM_11A
    {0x411C, 2, 0xF000}, // DYNAMIC_SEQRAM_11C
    {0x411E, 2, 0x0005}, // DYNAMIC_SEQRAM_11E
    {0x4120, 2, 0xF000}, // DYNAMIC_SEQRAM_120
    {0x4122, 2, 0xE0E0}, // DYNAMIC_SEQRAM_122
    {0x4124, 2, 0xE0E0}, // DYNAMIC_SEQRAM_124
    {0x4126, 2, 0xE0E0}, // DYNAMIC_SEQRAM_126
    {0x4128, 2, 0x0A35}, // DYNAMIC_SEQRAM_128
    {0x412A, 2, 0x10EF}, // DYNAMIC_SEQRAM_12A
    {0x412C, 2, 0x3003}, // DYNAMIC_SEQRAM_12C
    {0x412E, 2, 0x30D8}, // DYNAMIC_SEQRAM_12E
    {0x4130, 2, 0xF005}, // DYNAMIC_SEQRAM_130
    {0x4132, 2, 0x85F0}, // DYNAMIC_SEQRAM_132
    {0x4134, 2, 0x049A}, // DYNAMIC_SEQRAM_134
    {0x4136, 2, 0x89F0}, // DYNAMIC_SEQRAM_136
    {0x4138, 2, 0x0099}, // DYNAMIC_SEQRAM_138
    {0x413A, 2, 0x97F0}, // DYNAMIC_SEQRAM_13A
    {0x413C, 2, 0x0030}, // DYNAMIC_SEQRAM_13C
    {0x413E, 2, 0xC0F0}, // DYNAMIC_SEQRAM_13E
    {0x4140, 2, 0x0082}, // DYNAMIC_SEQRAM_140
    {0x4142, 2, 0xF000}, // DYNAMIC_SEQRAM_142
    {0x4144, 2, 0x3018}, // DYNAMIC_SEQRAM_144
    {0x4146, 2, 0xF002}, // DYNAMIC_SEQRAM_146
    {0x4148, 2, 0xB520}, // DYNAMIC_SEQRAM_148
    {0x414A, 2, 0x58F0}, // DYNAMIC_SEQRAM_14A
    {0x414C, 2, 0x089C}, // DYNAMIC_SEQRAM_14C
    {0x414E, 2, 0xF010}, // DYNAMIC_SEQRAM_14E
    {0x4150, 2, 0x99B6}, // DYNAMIC_SEQRAM_150
    {0x4152, 2, 0xF003}, // DYNAMIC_SEQRAM_152
    {0x4154, 2, 0xB498}, // DYNAMIC_SEQRAM_154
    {0x4156, 2, 0xA096}, // DYNAMIC_SEQRAM_156
    {0x4158, 2, 0xF000}, // DYNAMIC_SEQRAM_158
    {0x415A, 2, 0xA2F0}, // DYNAMIC_SEQRAM_15A
    {0x415C, 2, 0x00A2}, // DYNAMIC_SEQRAM_15C
    {0x415E, 2, 0xF008}, // DYNAMIC_SEQRAM_15E
    {0x4160, 2, 0x9DF0}, // DYNAMIC_SEQRAM_160
    {0x4162, 2, 0x209D}, // DYNAMIC_SEQRAM_162
    {0x4164, 2, 0x8C08}, // DYNAMIC_SEQRAM_164
    {0x4166, 2, 0x08F0}, // DYNAMIC_SEQRAM_166
    {0x4168, 2, 0x0036}, // DYNAMIC_SEQRAM_168
    {0x416A, 2, 0x008F}, // DYNAMIC_SEQRAM_16A
    {0x416C, 2, 0x88F0}, // DYNAMIC_SEQRAM_16C
    {0x416E, 2, 0x0188}, // DYNAMIC_SEQRAM_16E
    {0x4170, 2, 0x3600}, // DYNAMIC_SEQRAM_170
    {0x4172, 2, 0xF000}, // DYNAMIC_SEQRAM_172
    {0x4174, 2, 0x83F0}, // DYNAMIC_SEQRAM_174
    {0x4176, 2, 0x0290}, // DYNAMIC_SEQRAM_176
    {0x4178, 2, 0xF001}, // DYNAMIC_SEQRAM_178
    {0x417A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_17A
    {0x417C, 2, 0x2DA3}, // DYNAMIC_SEQRAM_17C
    {0x417E, 2, 0xF000}, // DYNAMIC_SEQRAM_17E
    {0x4180, 2, 0xA3F0}, // DYNAMIC_SEQRAM_180
    {0x4182, 2, 0x089D}, // DYNAMIC_SEQRAM_182
    {0x4184, 2, 0xF06D}, // DYNAMIC_SEQRAM_184
    {0x4186, 2, 0x4070}, // DYNAMIC_SEQRAM_186
    {0x4188, 2, 0x3003}, // DYNAMIC_SEQRAM_188
    {0x418A, 2, 0x214D}, // DYNAMIC_SEQRAM_18A
    {0x418C, 2, 0x1FF6}, // DYNAMIC_SEQRAM_18C
    {0x418E, 2, 0x0851}, // DYNAMIC_SEQRAM_18E
    {0x4190, 2, 0x0245}, // DYNAMIC_SEQRAM_190
    {0x4192, 2, 0x9D36}, // DYNAMIC_SEQRAM_192
    {0x4194, 2, 0xD8F0}, // DYNAMIC_SEQRAM_194
    {0x4196, 2, 0x0083}, // DYNAMIC_SEQRAM_196
    {0x4198, 2, 0xF000}, // DYNAMIC_SEQRAM_198
    {0x419A, 2, 0x87F0}, // DYNAMIC_SEQRAM_19A
    {0x419C, 2, 0x0087}, // DYNAMIC_SEQRAM_19C
    {0x419E, 2, 0xF000}, // DYNAMIC_SEQRAM_19E
    {0x41A0, 2, 0x36D8}, // DYNAMIC_SEQRAM_1A0
    {0x41A2, 2, 0x020D}, // DYNAMIC_SEQRAM_1A2
    {0x41A4, 2, 0x0205}, // DYNAMIC_SEQRAM_1A4
    {0x41A6, 2, 0xF000}, // DYNAMIC_SEQRAM_1A6
    {0x41A8, 2, 0x36D8}, // DYNAMIC_SEQRAM_1A8
    {0x41AA, 2, 0xF000}, // DYNAMIC_SEQRAM_1AA
    {0x41AC, 2, 0x83F0}, // DYNAMIC_SEQRAM_1AC
    {0x41AE, 2, 0x0087}, // DYNAMIC_SEQRAM_1AE
    {0x41B0, 2, 0xF000}, // DYNAMIC_SEQRAM_1B0
    {0x41B2, 2, 0x87F0}, // DYNAMIC_SEQRAM_1B2
    {0x41B4, 2, 0x0036}, // DYNAMIC_SEQRAM_1B4
    {0x41B6, 2, 0xD802}, // DYNAMIC_SEQRAM_1B6
    {0x41B8, 2, 0x0D02}, // DYNAMIC_SEQRAM_1B8
    {0x41BA, 2, 0x05F0}, // DYNAMIC_SEQRAM_1BA
    {0x41BC, 2, 0x0036}, // DYNAMIC_SEQRAM_1BC
    {0x41BE, 2, 0xD8F0}, // DYNAMIC_SEQRAM_1B3
    {0x41C0, 2, 0x0083}, // DYNAMIC_SEQRAM_1C0
    {0x41C2, 2, 0xF000}, // DYNAMIC_SEQRAM_1C2
    {0x41C4, 2, 0x87F0}, // DYNAMIC_SEQRAM_1C4
    {0x41C6, 2, 0x0087}, // DYNAMIC_SEQRAM_1C6
    {0x41C8, 2, 0xF000}, // DYNAMIC_SEQRAM_1C8
    {0x41CA, 2, 0x36D8}, // DYNAMIC_SEQRAM_1CA
    {0x41CC, 2, 0x020D}, // DYNAMIC_SEQRAM_1CC
    {0x41CE, 2, 0x0205}, // DYNAMIC_SEQRAM_1CE
    {0x41D0, 2, 0xF000}, // DYNAMIC_SEQRAM_1D0
    {0x41D2, 2, 0x36D8}, // DYNAMIC_SEQRAM_1D2
    {0x41D4, 2, 0xF000}, // DYNAMIC_SEQRAM_1D4
    {0x41D6, 2, 0x83F0}, // DYNAMIC_SEQRAM_1D6
    {0x41D8, 2, 0x0087}, // DYNAMIC_SEQRAM_1D8
    {0x41DA, 2, 0xF000}, // DYNAMIC_SEQRAM_1DA
    {0x41DC, 2, 0x8713}, // DYNAMIC_SEQRAM_1DC
    {0x41DE, 2, 0x0036}, // DYNAMIC_SEQRAM_1DE
    {0x41E0, 2, 0xD802}, // DYNAMIC_SEQRAM_1E0
    {0x41E2, 2, 0x0DE0}, // DYNAMIC_SEQRAM_1E2
    {0x41E4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_1E4
    {0x41E6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_1E6
    {0x41E8, 2, 0x9F13}, // DYNAMIC_SEQRAM_1E8
    {0x41EA, 2, 0x0041}, // DYNAMIC_SEQRAM_1EA
    {0x41EC, 2, 0x80F3}, // DYNAMIC_SEQRAM_1EC
    {0x41EE, 2, 0xF213}, // DYNAMIC_SEQRAM_1EE
    {0x41F0, 2, 0x00F0}, // DYNAMIC_SEQRAM_1F0
    {0x41F2, 2, 0x13B8}, // DYNAMIC_SEQRAM_1F2
    {0x41F4, 2, 0xF04C}, // DYNAMIC_SEQRAM_1F4
    {0x41F6, 2, 0x9FF0}, // DYNAMIC_SEQRAM_1F6
    {0x41F8, 2, 0x00B7}, // DYNAMIC_SEQRAM_1F8
    {0x41FA, 2, 0xF006}, // DYNAMIC_SEQRAM_1FA
    {0x41FC, 2, 0x0035}, // DYNAMIC_SEQRAM_1FC
    {0x41FE, 2, 0x10AF}, // DYNAMIC_SEQRAM_1FE
    {0x4200, 2, 0x3003}, // DYNAMIC_SEQRAM_200
    {0x4202, 2, 0x30C0}, // DYNAMIC_SEQRAM_202
    {0x4204, 2, 0xB2F0}, // DYNAMIC_SEQRAM_204
    {0x4206, 2, 0x01B5}, // DYNAMIC_SEQRAM_206
    {0x4208, 2, 0xF001}, // DYNAMIC_SEQRAM_208
    {0x420A, 2, 0x85F0}, // DYNAMIC_SEQRAM_20A
    {0x420C, 2, 0x0292}, // DYNAMIC_SEQRAM_20C
    {0x420E, 2, 0xF000}, // DYNAMIC_SEQRAM_20E
    {0x4210, 2, 0x9A8B}, // DYNAMIC_SEQRAM_210
    {0x4212, 2, 0xF000}, // DYNAMIC_SEQRAM_212
    {0x4214, 2, 0x9997}, // DYNAMIC_SEQRAM_214
    {0x4216, 2, 0xF007}, // DYNAMIC_SEQRAM_216
    {0x4218, 2, 0xB6F0}, // DYNAMIC_SEQRAM_218
    {0x421A, 2, 0x0020}, // DYNAMIC_SEQRAM_21A
    {0x421C, 2, 0x5830}, // DYNAMIC_SEQRAM_21C
    {0x421E, 2, 0xC040}, // DYNAMIC_SEQRAM_21E
    {0x4220, 2, 0x1282}, // DYNAMIC_SEQRAM_220
    {0x4222, 2, 0xF005}, // DYNAMIC_SEQRAM_222
    {0x4224, 2, 0x9CF0}, // DYNAMIC_SEQRAM_224
    {0x4226, 2, 0x01B2}, // DYNAMIC_SEQRAM_226
    {0x4228, 2, 0xF008}, // DYNAMIC_SEQRAM_228
    {0x422A, 2, 0xB8F0}, // DYNAMIC_SEQRAM_22A
    {0x422C, 2, 0x0799}, // DYNAMIC_SEQRAM_22C
    {0x422E, 2, 0xF005}, // DYNAMIC_SEQRAM_22E
    {0x4230, 2, 0x98F0}, // DYNAMIC_SEQRAM_230
    {0x4232, 2, 0x0296}, // DYNAMIC_SEQRAM_232
    {0x4234, 2, 0xA2F0}, // DYNAMIC_SEQRAM_234
    {0x4236, 2, 0x00A2}, // DYNAMIC_SEQRAM_236
    {0x4238, 2, 0xF008}, // DYNAMIC_SEQRAM_238
    {0x423A, 2, 0x9DF0}, // DYNAMIC_SEQRAM_23A
    {0x423C, 2, 0x02A1}, // DYNAMIC_SEQRAM_23C
    {0x423E, 2, 0xF01F}, // DYNAMIC_SEQRAM_23E
    {0x4240, 2, 0x1009}, // DYNAMIC_SEQRAM_240
    {0x4242, 2, 0x2220}, // DYNAMIC_SEQRAM_242
    {0x4244, 2, 0x0808}, // DYNAMIC_SEQRAM_244
    {0x4246, 2, 0xF000}, // DYNAMIC_SEQRAM_246
    {0x4248, 2, 0x3600}, // DYNAMIC_SEQRAM_248
    {0x424A, 2, 0xF000}, // DYNAMIC_SEQRAM_24A
    {0x424C, 2, 0x88F0}, // DYNAMIC_SEQRAM_24C
    {0x424E, 2, 0x0788}, // DYNAMIC_SEQRAM_24E
    {0x4250, 2, 0x3600}, // DYNAMIC_SEQRAM_250
    {0x4252, 2, 0xF000}, // DYNAMIC_SEQRAM_252
    {0x4254, 2, 0x83F0}, // DYNAMIC_SEQRAM_254
    {0x4256, 2, 0x0290}, // DYNAMIC_SEQRAM_256
    {0x4258, 2, 0xF016}, // DYNAMIC_SEQRAM_258
    {0x425A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_25A
    {0x425C, 2, 0x11A3}, // DYNAMIC_SEQRAM_25C
    {0x425E, 2, 0xF000}, // DYNAMIC_SEQRAM_25E
    {0x4260, 2, 0xA3F0}, // DYNAMIC_SEQRAM_260
    {0x4262, 2, 0x089D}, // DYNAMIC_SEQRAM_262
    {0x4264, 2, 0xF002}, // DYNAMIC_SEQRAM_264
    {0x4266, 2, 0xA1F0}, // DYNAMIC_SEQRAM_266
    {0x4268, 2, 0x20A1}, // DYNAMIC_SEQRAM_268
    {0x426A, 2, 0xF006}, // DYNAMIC_SEQRAM_26A
    {0x426C, 2, 0x4300}, // DYNAMIC_SEQRAM_26C
    {0x426E, 2, 0xF049}, // DYNAMIC_SEQRAM_26E
    {0x4270, 2, 0x4014}, // DYNAMIC_SEQRAM_270
    {0x4272, 2, 0x8B8E}, // DYNAMIC_SEQRAM_272
    {0x4274, 2, 0x9DF0}, // DYNAMIC_SEQRAM_274
    {0x4276, 2, 0x0802}, // DYNAMIC_SEQRAM_276
    {0x4278, 2, 0x02F0}, // DYNAMIC_SEQRAM_278
    {0x427A, 2, 0x00A6}, // DYNAMIC_SEQRAM_27A
    {0x427C, 2, 0xF013}, // DYNAMIC_SEQRAM_27C
    {0x427E, 2, 0xB283}, // DYNAMIC_SEQRAM_27E
    {0x4280, 2, 0x9C36}, // DYNAMIC_SEQRAM_280
    {0x4282, 2, 0x00F0}, // DYNAMIC_SEQRAM_282
    {0x4284, 2, 0x0636}, // DYNAMIC_SEQRAM_284
    {0x4286, 2, 0x009C}, // DYNAMIC_SEQRAM_286
    {0x4288, 2, 0xF008}, // DYNAMIC_SEQRAM_288
    {0x428A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_28A
    {0x428C, 2, 0x0083}, // DYNAMIC_SEQRAM_28C
    {0x428E, 2, 0xA0F0}, // DYNAMIC_SEQRAM_28E
    {0x4290, 2, 0x0630}, // DYNAMIC_SEQRAM_290
    {0x4292, 2, 0x18F0}, // DYNAMIC_SEQRAM_292
    {0x4294, 2, 0x02A3}, // DYNAMIC_SEQRAM_294
    {0x4296, 2, 0xF000}, // DYNAMIC_SEQRAM_296
    {0x4298, 2, 0xA3F0}, // DYNAMIC_SEQRAM_298
    {0x429A, 2, 0x0243}, // DYNAMIC_SEQRAM_29A
    {0x429C, 2, 0x00F0}, // DYNAMIC_SEQRAM_29C
    {0x429E, 2, 0x049D}, // DYNAMIC_SEQRAM_29E
    {0x42A0, 2, 0xF078}, // DYNAMIC_SEQRAM_2A0
    {0x42A2, 2, 0x3018}, // DYNAMIC_SEQRAM_2A2
    {0x42A4, 2, 0xF000}, // DYNAMIC_SEQRAM_2A4
    {0x42A6, 2, 0x9D82}, // DYNAMIC_SEQRAM_2A6
    {0x42A8, 2, 0xF000}, // DYNAMIC_SEQRAM_2A8
    {0x42AA, 2, 0x9030}, // DYNAMIC_SEQRAM_2AA
    {0x42AC, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2AC
    {0x42AE, 2, 0x1130}, // DYNAMIC_SEQRAM_2AE
    {0x42B0, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2B0
    {0x42B2, 2, 0x0082}, // DYNAMIC_SEQRAM_2B2
    {0x42B4, 2, 0xF001}, // DYNAMIC_SEQRAM_2B4
    {0x42B6, 2, 0x1009}, // DYNAMIC_SEQRAM_2B6
    {0x42B8, 2, 0xF02A}, // DYNAMIC_SEQRAM_2B8
    {0x42BA, 2, 0xA2F0}, // DYNAMIC_SEQRAM_2BA
    {0x42BC, 2, 0x00A2}, // DYNAMIC_SEQRAM_2BC
    {0x42BE, 2, 0x3018}, // DYNAMIC_SEQRAM_2BE
    {0x42C0, 2, 0xF007}, // DYNAMIC_SEQRAM_2C0
    {0x42C2, 2, 0x9DF0}, // DYNAMIC_SEQRAM_2C2
    {0x42C4, 2, 0x1C8C}, // DYNAMIC_SEQRAM_2C4
    {0x42C6, 2, 0xF005}, // DYNAMIC_SEQRAM_2C6
    {0x42C8, 2, 0x301F}, // DYNAMIC_SEQRAM_2C8
    {0x42CA, 2, 0x216D}, // DYNAMIC_SEQRAM_2CA
    {0x42CC, 2, 0x0A51}, // DYNAMIC_SEQRAM_2CC
    {0x42CE, 2, 0x1FEA}, // DYNAMIC_SEQRAM_2CE
    {0x42D0, 2, 0x8640}, // DYNAMIC_SEQRAM_2D0
    {0x42D2, 2, 0xE29F}, // DYNAMIC_SEQRAM_2D2
    {0x42D4, 2, 0xF009}, // DYNAMIC_SEQRAM_2D4
    {0x42D6, 2, 0x0005}, // DYNAMIC_SEQRAM_2D6
    {0x42D8, 2, 0xF000}, // DYNAMIC_SEQRAM_2D8
    {0x42DA, 2, 0x30C0}, // DYNAMIC_SEQRAM_2DA
    {0x42DC, 2, 0xF001}, // DYNAMIC_SEQRAM_2DC
    {0x42DE, 2, 0x83F0}, // DYNAMIC_SEQRAM_2DE
    {0x42E0, 2, 0x0036}, // DYNAMIC_SEQRAM_2E0
    {0x42E2, 2, 0x00F0}, // DYNAMIC_SEQRAM_2E2
    {0x42E4, 2, 0x0087}, // DYNAMIC_SEQRAM_2E4
    {0x42E6, 2, 0xF007}, // DYNAMIC_SEQRAM_2E6
    {0x42E8, 2, 0x87F0}, // DYNAMIC_SEQRAM_2E8
    {0x42EA, 2, 0x0036}, // DYNAMIC_SEQRAM_2EA
    {0x42EC, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2EC
    {0x42EE, 2, 0x0000}, // DYNAMIC_SEQRAM_2EE
    {0x42F0, 2, 0x0DF0}, // DYNAMIC_SEQRAM_2F0
    {0x42F2, 2, 0x0000}, // DYNAMIC_SEQRAM_2F2
    {0x42F4, 2, 0x05F0}, // DYNAMIC_SEQRAM_2F4
    {0x42F6, 2, 0x0030}, // DYNAMIC_SEQRAM_2F6
    {0x42F8, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2F8
    {0x42FA, 2, 0x0183}, // DYNAMIC_SEQRAM_2FA
    {0x42FC, 2, 0xF000}, // DYNAMIC_SEQRAM_2FC
    {0x42FE, 2, 0x3600}, // DYNAMIC_SEQRAM_2FE
    {0x4300, 2, 0xF000}, // DYNAMIC_SEQRAM_300
    {0x4302, 2, 0x87F0}, // DYNAMIC_SEQRAM_302
    {0x4304, 2, 0x0787}, // DYNAMIC_SEQRAM_304
    {0x4306, 2, 0xF000}, // DYNAMIC_SEQRAM_306
    {0x4308, 2, 0x36C0}, // DYNAMIC_SEQRAM_308
    {0x430A, 2, 0xF000}, // DYNAMIC_SEQRAM_30A
    {0x430C, 2, 0x000F}, // DYNAMIC_SEQRAM_30C
    {0x430E, 2, 0xF42A}, // DYNAMIC_SEQRAM_30E
    {0x4310, 2, 0x4180}, // DYNAMIC_SEQRAM_310
    {0x4312, 2, 0x1300}, // DYNAMIC_SEQRAM_312
    {0x4314, 2, 0x9FF0}, // DYNAMIC_SEQRAM_314
    {0x4316, 2, 0x00E0}, // DYNAMIC_SEQRAM_316
    {0x4318, 2, 0xE0E0}, // DYNAMIC_SEQRAM_318
    {0x431A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31A
    {0x431C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31C
    {0x431E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31E
    {0x4320, 2, 0xE0E0}, // DYNAMIC_SEQRAM_320
    {0x4322, 2, 0xE0E0}, // DYNAMIC_SEQRAM_322
    {0x4324, 2, 0xE0E0}, // DYNAMIC_SEQRAM_324
    {0x4326, 2, 0xE0E0}, // DYNAMIC_SEQRAM_326
    {0x4328, 2, 0xE0E0}, // DYNAMIC_SEQRAM_328
    {0x432A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32A
    {0x432C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32C
    {0x432E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32E
    {0x4330, 2, 0xE0E0}, // DYNAMIC_SEQRAM_330
    {0x4332, 2, 0xE0E0}, // DYNAMIC_SEQRAM_332
    {0x4334, 2, 0xE0E0}, // DYNAMIC_SEQRAM_334
    {0x4336, 2, 0xE0E0}, // DYNAMIC_SEQRAM_336
    {0x4338, 2, 0xE0E0}, // DYNAMIC_SEQRAM_338
    {0x433A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33A
    {0x433C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33C
    {0x433E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33E
    {0x4340, 2, 0xE0E0}, // DYNAMIC_SEQRAM_340
    {0x4342, 2, 0xE0E0}, // DYNAMIC_SEQRAM_342
    {0x4344, 2, 0xE0E0}, // DYNAMIC_SEQRAM_344
    {0x4346, 2, 0xE0E0}, // DYNAMIC_SEQRAM_346
    {0x4348, 2, 0xE0E0}, // DYNAMIC_SEQRAM_348
    {0x434A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34A
    {0x434C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34C
    {0x434E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34E
    {0x4350, 2, 0xE0E0}, // DYNAMIC_SEQRAM_350
    {0x4352, 2, 0xE0E0}, // DYNAMIC_SEQRAM_352
    {0x4354, 2, 0xE0E0}, // DYNAMIC_SEQRAM_354
    {0x4356, 2, 0xE0E0}, // DYNAMIC_SEQRAM_356
    {0x4358, 2, 0xE0E0}, // DYNAMIC_SEQRAM_358
    {0x435A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35A
    {0x435C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35C
    {0x435E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35E
    {0x4360, 2, 0xE0E0}, // DYNAMIC_SEQRAM_360
    {0x4362, 2, 0xE0E0}, // DYNAMIC_SEQRAM_362
    {0x4364, 2, 0xE0E0}, // DYNAMIC_SEQRAM_364
    {0x4366, 2, 0xE0E0}, // DYNAMIC_SEQRAM_366
    {0x4368, 2, 0xE0E0}, // DYNAMIC_SEQRAM_368
    {0x436A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36A
    {0x436C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36C
    {0x436E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36E
    {0x4370, 2, 0xE0E0}, // DYNAMIC_SEQRAM_370
    {0x4372, 2, 0xE0E0}, // DYNAMIC_SEQRAM_372
    {0x4374, 2, 0xE0E0}, // DYNAMIC_SEQRAM_374
    {0x4376, 2, 0xE0E0}, // DYNAMIC_SEQRAM_376
    {0x4378, 2, 0xE0E0}, // DYNAMIC_SEQRAM_378
    {0x437A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37A
    {0x437C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37C
    {0x437E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37E
    {0x4380, 2, 0xE0E0}, // DYNAMIC_SEQRAM_380
    {0x4382, 2, 0xE0E0}, // DYNAMIC_SEQRAM_382
    {0x4384, 2, 0xE0E0}, // DYNAMIC_SEQRAM_384
    {0x4386, 2, 0xE0E0}, // DYNAMIC_SEQRAM_386
    {0x4388, 2, 0xE0E0}, // DYNAMIC_SEQRAM_388
    {0x438A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38A
    {0x438C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38C
    {0x438E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38E
    {0x4390, 2, 0xE0E0}, // DYNAMIC_SEQRAM_390
    {0x4392, 2, 0xE0E0}, // DYNAMIC_SEQRAM_392
    {0x4394, 2, 0xE0E0}, // DYNAMIC_SEQRAM_394
    {0x4396, 2, 0xE0E0}, // DYNAMIC_SEQRAM_396
    {0x4398, 2, 0xE0E0}, // DYNAMIC_SEQRAM_398
    {0x439A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39A
    {0x439C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39C
    {0x439E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39E
    {0x43A0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A0
    {0x43A2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A2
    {0x43A4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A4
    {0x43A6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A6
    {0x43A8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A8
    {0x43AA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AA
    {0x43AC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AC
    {0x43AE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AE
    {0x43B0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B0
    {0x43B2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B2
    {0x43B4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B4
    {0x43B6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B6
    {0x43B8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B8
    {0x43BA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BA
    {0x43BC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BC
    {0x43BE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BE
    {0x43C0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C0
    {0x43C2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C2
    {0x43C4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C4
    {0x43C6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C6
    {0x43C8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C8
    {0x43CA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CA
    {0x43CC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CC
    {0x43CE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CE
    {0x43D0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D0
    {0x43D2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D2
    {0x43D4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D4
    {0x43D6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D6
    {0x43D8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D8
    {0x43DA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DA
    {0x43DC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DC
    {0x43DE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DE
    {0x43E0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E0
    {0x43E2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E2
    {0x43E4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E4
    {0x43E6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E6
    {0x43E8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E8
    {0x43EA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EA
    {0x43EC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EC
    {0x43EE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EE
    {0x43F0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F0
    {0x43F2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F2
    {0x43F4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F4
    {0x43F6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F6
    {0x43F8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F8
    {0x43FA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FA
    {0x43FC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FC
    {0x43FE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FE
    {0x4400, 2, 0xE0E0}, // DYNAMIC_SEQRAM_400
    {0x4402, 2, 0xE0E0}, // DYNAMIC_SEQRAM_402
    {0x4404, 2, 0xE0E0}, // DYNAMIC_SEQRAM_404
    {0x4406, 2, 0xE0E0}, // DYNAMIC_SEQRAM_406
    {0x4408, 2, 0xE0E0}, // DYNAMIC_SEQRAM_408
    {0x440A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40A
    {0x440C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40C
    {0x440E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40E
    {0x4410, 2, 0xE0E0}, // DYNAMIC_SEQRAM_410
    {0x4412, 2, 0xE0E0}, // DYNAMIC_SEQRAM_412
    {0x4414, 2, 0xE0E0}, // DYNAMIC_SEQRAM_414
    {0x4416, 2, 0xE0E0}, // DYNAMIC_SEQRAM_416
    {0x4418, 2, 0xE0E0}, // DYNAMIC_SEQRAM_418
    {0x441A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41A
    {0x441C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41C
    {0x441E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41E
    {0x4420, 2, 0xE0E0}, // DYNAMIC_SEQRAM_420
    {0x4422, 2, 0xE0E0}, // DYNAMIC_SEQRAM_422
    {0x4424, 2, 0xE0E0}, // DYNAMIC_SEQRAM_424
    {0x4426, 2, 0xE0E0}, // DYNAMIC_SEQRAM_426
    {0x4428, 2, 0xE0E0}, // DYNAMIC_SEQRAM_428
    {0x442A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42A
    {0x442C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42C
    {0x442E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42E
    {0x4430, 2, 0xE0E0}, // DYNAMIC_SEQRAM_430
    {0x4432, 2, 0xE0E0}, // DYNAMIC_SEQRAM_432
    {0x4434, 2, 0xE0E0}, // DYNAMIC_SEQRAM_434
    {0x4436, 2, 0xE0E0}, // DYNAMIC_SEQRAM_436
    {0x4438, 2, 0xE0E0}, // DYNAMIC_SEQRAM_438
    {0x443A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43A
    {0x443C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43C
    {0x443E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43E
    {0x4440, 2, 0xE0E0}, // DYNAMIC_SEQRAM_440
    {0x4442, 2, 0xE0E0}, // DYNAMIC_SEQRAM_442
    {0x4444, 2, 0xE0E0}, // DYNAMIC_SEQRAM_444
    {0x4446, 2, 0xE0E0}, // DYNAMIC_SEQRAM_446
    {0x4448, 2, 0xE0E0}, // DYNAMIC_SEQRAM_448
    {0x444A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44A
    {0x444C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44C
    {0x444E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44E
    {0x4450, 2, 0xE0E0}, // DYNAMIC_SEQRAM_450
    {0x4452, 2, 0xE0E0}, // DYNAMIC_SEQRAM_452
    {0x4454, 2, 0xE0E0}, // DYNAMIC_SEQRAM_454
    {0x4456, 2, 0xE0E0}, // DYNAMIC_SEQRAM_456
    {0x4458, 2, 0xE0E0}, // DYNAMIC_SEQRAM_458
    {0x445A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45A
    {0x445C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45C
    {0x445E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45E
    {0x4460, 2, 0xE0E0}, // DYNAMIC_SEQRAM_460
    {0x4462, 2, 0xE0E0}, // DYNAMIC_SEQRAM_462
    {0x4464, 2, 0xE0E0}, // DYNAMIC_SEQRAM_464
    {0x4466, 2, 0xE0E0}, // DYNAMIC_SEQRAM_466
    {0x4468, 2, 0xE0E0}, // DYNAMIC_SEQRAM_468
    {0x446A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46A
    {0x446C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46C
    {0x446E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46E
    {0x4470, 2, 0xE0E0}, // DYNAMIC_SEQRAM_470
    {0x4472, 2, 0xE0E0}, // DYNAMIC_SEQRAM_472
    {0x4474, 2, 0xE0E0}, // DYNAMIC_SEQRAM_474
    {0x4476, 2, 0xE0E0}, // DYNAMIC_SEQRAM_476
    {0x4478, 2, 0xE0E0}, // DYNAMIC_SEQRAM_478
    {0x447A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47A
    {0x447C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47C
    {0x447E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47E
    {0x4480, 2, 0xE0E0}, // DYNAMIC_SEQRAM_480
    {0x4482, 2, 0xE0E0}, // DYNAMIC_SEQRAM_482
    {0x4484, 2, 0xE0E0}, // DYNAMIC_SEQRAM_484
    {0x4486, 2, 0xE0E0}, // DYNAMIC_SEQRAM_486
    {0x4488, 2, 0xE0E0}, // DYNAMIC_SEQRAM_488
    {0x448A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48A
    {0x448C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48C
    {0x448E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48E
    {0x4490, 2, 0xE0E0}, // DYNAMIC_SEQRAM_490
    {0x4492, 2, 0xE0E0}, // DYNAMIC_SEQRAM_492
    {0x4494, 2, 0xE0E0}, // DYNAMIC_SEQRAM_494
    {0x4496, 2, 0xE0E0}, // DYNAMIC_SEQRAM_496
    {0x4498, 2, 0xE0E0}, // DYNAMIC_SEQRAM_498
    {0x449A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49A
    {0x449C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49C
    {0x449E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49E
    {0x44A0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A0
    {0x44A2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A2
    {0x44A4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A4
    {0x44A6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A6
    {0x44A8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A8
    {0x44AA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AA
    {0x44AC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AC
    {0x44AE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AE
    {0x44B0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B0
    {0x44B2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B2
    {0x44B4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B4
    {0x5500, 2, 0x0000}, // AGAIN_LUT0
    {0x5502, 2, 0x0002}, // AGAIN_LUT1
    {0x5504, 2, 0x0006}, // AGAIN_LUT2
    {0x5506, 2, 0x0009}, // AGAIN_LUT3
    {0x5508, 2, 0x000F}, // AGAIN_LUT4
    {0x550A, 2, 0x0010}, // AGAIN_LUT5
    {0x550C, 2, 0x0011}, // AGAIN_LUT6
    {0x550E, 2, 0x0012}, // AGAIN_LUT7
    {0x5510, 2, 0x0019}, // AGAIN_LUT8
    {0x5512, 2, 0x0020}, // AGAIN_LUT9
    {0x5514, 2, 0x0021}, // AGAIN_LUT10
    {0x5516, 2, 0x0023}, // AGAIN_LUT11
    {0x5518, 2, 0x0026}, // AGAIN_LUT12
    {0x551A, 2, 0x002B}, // AGAIN_LUT13
    {0x551C, 2, 0x002F}, // AGAIN_LUT14
    {0x551E, 2, 0x0030}, // AGAIN_LUT15
    {0x5400, 2, 0x0100}, // GT1_COARSE0
    {0x5402, 2, 0x2106}, // GT1_COARSE1
    {0x5404, 2, 0x1101}, // GT1_COARSE2
    {0x5406, 2, 0x3106}, // GT1_COARSE3
    {0x5408, 2, 0x7100}, // GT1_COARSE4
    {0x540A, 2, 0x8107}, // GT1_COARSE5
    {0x540C, 2, 0xB101}, // GT1_COARSE6
    {0x540E, 2, 0xD101}, // GT1_COARSE7
    {0x5410, 2, 0xF12E}, // GT1_COARSE8
    {0x5412, 2, 0xF112}, // GT1_COARSE9
    {0x5414, 2, 0xF184}, // GT1_COARSE10
    {0x5416, 2, 0xF224}, // GT1_COARSE11
    {0x5418, 2, 0xF306}, // GT1_COARSE12
    {0x541A, 2, 0xF446}, // GT1_COARSE13
    {0x541C, 2, 0xF609}, // GT1_COARSE14
    {0x541E, 2, 0xF887}, // GT1_COARSE15
    {0x5420, 2, 0xFC0B}, // GT1_COARSE16
    {0x5422, 2, 0xFC0B}, // GT1_COARSE17
    {0x5424, 2, 0xFFFA}, // GT1_DCG_ATTN_SET0
    {0x5426, 2, 0x5557}, // GT1_DCG_ATTN_SET1
    {0x5428, 2, 0x0005}, // GT1_DCG_ATTN_SET2
    {0x542A, 2, 0xA550}, // GT1_ZONE_SET0
    {0x542C, 2, 0xAAAA}, // GT1_ZONE_SET1
    {0x542E, 2, 0x000A}, // GT1_ZONE_SET2
    {0x5460, 2, 0x2269}, // ZT1_REG0_ADDR
    {0x5462, 2, 0x0B87}, // ZT1_REG0_VALUE0
    {0x5464, 2, 0x0B87}, // ZT1_REG0_VALUE1
    {0x5466, 2, 0x0983}, // ZT1_REG0_VALUE2
    {0x5498, 2, 0x225E}, // ZT1_REG7_ADDR
    {0x549A, 2, 0xBCAA}, // ZT1_REG7_VALUE0
    {0x549C, 2, 0xBCAA}, // ZT1_REG7_VALUE1
    {0x549E, 2, 0xBDAA}, // ZT1_REG7_VALUE2
    {0x3060, 2, 0xFF01}, // GAIN_TABLE_CTRL
    {0x44BA, 2, 0x0050}, // DAC_LD_4_5
    {0x44BC, 2, 0xBCAA}, // DAC_LD_6_7
    {0x44C0, 2, 0x4070}, // DAC_LD_10_11
    {0x44C4, 2, 0x04D0}, // DAC_LD_14_15
    {0x44C6, 2, 0x17E2}, // DAC_LD_16_17
    {0x44C8, 2, 0xEA43}, // DAC_LD_18_19
    {0x44CA, 2, 0x000E}, // DAC_LD_20_21
    {0x44CC, 2, 0x7777}, // DAC_LD_22_23
    {0x44CE, 2, 0x8BA4}, // DAC_LD_24_25
    {0x44D0, 2, 0x1735}, // DAC_LD_26_27
    {0x44D2, 2, 0x0B87}, // DAC_LD_28_29
    {0x44D4, 2, 0x8000}, // DAC_LD_30_31
    {0x44D6, 2, 0xF206}, // DAC_LD_32_33
    {0x44D8, 2, 0xAAFA}, // DAC_LD_34_35
    {0x44DA, 2, 0xE001}, // DAC_LD_36_37
    {0x44DE, 2, 0x9BBC}, // DAC_LD_40_41
    {0x44E0, 2, 0x283C}, // DAC_LD_42_43
    {0x44E2, 2, 0x2821}, // DAC_LD_44_45
    {0x44E4, 2, 0x8000}, // DAC_LD_46_47
    {0x44E6, 2, 0x503F}, // DAC_LD_48_49
    {0x32A4, 2, 0x0000}, // CRM_CTRL
    {0x333C, 2, 0x0001}, // DYNAMIC_CTRL
    {0x301A, 2, 0x0000}, // RESET_REGISTER
    {0x3600, 2, 0x94DF}, // FDOC_CTRL
    {0x3700, 2, 0x0001}, // PIX_DEF_ID
    {0x3980, 2, 0x0003}, // PIX_DEF_CORR
    {0x36C0, 2, 0x0001}, // DIGITAL_GAIN_CTRL
    {0x36DE, 2, 0x002A}, // DATA_PEDESTAL1
    {0x301A, 2, 0x0008}, // RESET_REGISTER
    {0x3060, 2, 0x0000}, // GAIN_TABLE_CTRL
    {0x3060, 2, 0xFF01}, // GAIN_TABLE_CTRL
    {0x3980, 2, 0x0001}, // PIX_DEF_CORR
    {0x3340, 2, 0x0C00}, // OTPM_CTRL
    {0x3340, 2, 0x1C00}, // OTPM_CTRL
    {0x36C0, 2, 0x0001}, // DIGITAL_GAIN_CTRL

    {0x0400, 2, 0x0003}, // SCALING_MODE
    {0x0404, 2, 0x0A84}, // SCALE_M
    {0x0406, 2, 0x0400}, // SCALE_N
    {0x3C58, 2, 0x0008}, // SLICE_OVERLAP
    {0x3C40, 2, 0x0002}, // SCALE_CTRL
    {0x3C42, 2, 0x07F0}, // SCALE_SECOND_RESIDUAL_EVEN
    {0x3C44, 2, 0x00AE}, // SCALE_SECOND_RESIDUAL_ODD
    {0x3C46, 2, 0x090A}, // SCALE_SECOND_CROP
    {0x3C48, 2, 0x0474}, // SCALE_THIRD_RESIDUAL_EVEN
    {0x3C4A, 2, 0x07B6}, // SCALE_THIRD_RESIDUAL_ODD
    {0x3C4C, 2, 0x0C0C}, // SCALE_THIRD_CROP
    {0x3C4E, 2, 0x00F8}, // SCALE_FOURTH_RESIDUAL_EVEN
    {0x3C50, 2, 0x043A}, // SCALE_FOURTH_RESIDUAL_ODD
    {0x3C52, 2, 0x0B0B}, // SCALE_FOURTH_CROP
    {0x3C60, 2, 0x0108}, // SCALE_FIRST_PXL_NUM
    {0x3C62, 2, 0x0108}, // SCALE_SECOND_PXL_NUM
    {0x3C64, 2, 0x0100}, // SCALE_THIRD_PXL_NUM
    {0x3C66, 2, 0x00F8}, // SCALE_FOURTH_PXL_NUM
    {0x3EC8, 2, 0x0000}, // X_OUTPUT_OFFSET
    {0x3ECA, 2, 0x0000}, // Y_OUTPUT_OFFSET
    {0x034C, 2, 0x0400}, // X_OUTPUT_SIZE
    {0x034E, 2, 0x03B4}, // Y_OUTPUT_SIZE
    {0x3C7A, 2, 0x0000}, // REORDER_CTRL
    {0x3C6A, 2, 0x03AC}, // X_OUTPUT_OFFSET2
    {0x3C70, 2, 0x02E0}, // Y_OUTPUT_OFFSET2
    {0x3C6C, 2, 0x0400}, // X_OUTPUT_SIZE2
    {0x3C72, 2, 0x0400}, // Y_OUTPUT_SIZE2
    {0x3C74, 2, 0x0003}, // SYNC_REGEN
    {0x3C76, 2, 0x0020}, // SYNC_REGEN_BLANK
    {0x3F18, 2, 0x7B70}, // MIPI_JPEG_PN9_DATA_TYPE
    {0x3F1A, 2, 0x102B}, // MIPI_DATA_TYPE_INTERLEAVE

    {0x3062, 2, 0x002b}, // fenghuan add
    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE

    {0x0100, 1, 0x01}, // MODE_SELECT
    {0x44D6, 2, 0xB206}, // DAC_LD_32_33
};
#else

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_1024x1024_1024x948_table[] =
{
    {0x103, 0x3201, 0x01},

    {0x0304, 2, 0x2},
    {0x0306, 2, 0x56},
    {0x0300, 2, 0x6},
    {0x0302, 2, 0x1},
    {0x030C, 2, 0x7},
    {0x030E, 2, 0x75},
    {0x0308, 2, 0xA},
    {0x030A, 2, 0x1},
    {0x0344, 2, 0x4C0},
    {0x0348, 2, 0xF4F},
    {0x0346, 2, 0x2A6},
    {0x034A, 2, 0xC69},
    {0x034C, 2, 0xA90},
    {0x034E, 2, 0x9C4},
    {0x0380, 2, 0x1},
    {0x0382, 2, 0x1},
    {0x0384, 2, 0x1},
    {0x0386, 2, 0x1},

    {0x0900, 1, 0x0},
    {0x0901, 1, 0x11},
    {0x0342, 2, 0x4720},
    {0x0340, 2, 0x9D2},
    {0x0202, 2, 0x4E9},
    {0x0112, 2, 0xA0A},
    {0x0114, 1, 0x3},

    {0x0800, 1, 0x8},
    {0x0801, 1, 0x3},
    {0x0802, 1, 0x4},
    {0x0803, 1, 0x4},
    {0x0804, 1, 0x5},
    {0x0805, 1, 0x3},
    {0x0806, 1, 0xC},
    {0x0807, 1, 0x3},
    {0x082A, 1, 0x5},
    {0x082B, 1, 0x4},
    {0x082C, 1, 0x5},
    {0x3F06, 2, 0xC0},
    {0x3F0A, 2, 0x2000}, //{0x3F0A, 2, 0xA000},
    {0x3F0C, 2, 0x4},
    {0x3F20, 2, 0x8008},
    {0x3F1E, 2, 0x0},
    {0x3f20, 2, 0x0808},

    {0x4000, 2, 0x0114},
    {0x4002, 2, 0x1a25},
    {0x4004, 2, 0x3dff},
    {0x4006, 2, 0xffff},
    {0x4008, 2, 0x0a35},
    {0x400a, 2, 0x10ef},
    {0x400c, 2, 0x3003},
    {0x400e, 2, 0x30d8},
    {0x4010, 2, 0xf003},
    {0x4012, 2, 0xb5f0},
    {0x4014, 2, 0x0085},
    {0x4016, 2, 0xf004},
    {0x4018, 2, 0x9a89},
    {0x401a, 2, 0xf000},
    {0x401c, 2, 0x9997},
    {0x401e, 2, 0xf000},
    {0x4020, 2, 0x30c0},
    {0x4022, 2, 0xf000},
    {0x4024, 2, 0x82f0},
    {0x4026, 2, 0x0030},
    {0x4028, 2, 0x18f0},
    {0x402a, 2, 0x0320},
    {0x402c, 2, 0x58f0},
    {0x402e, 2, 0x089c},
    {0x4030, 2, 0xf010},
    {0x4032, 2, 0x99b6},
    {0x4034, 2, 0xf003},
    {0x4036, 2, 0xb498},
    {0x4038, 2, 0xa096},
    {0x403a, 2, 0xf000},
    {0x403c, 2, 0xa2f0},
    {0x403e, 2, 0x00a2},
    {0x4040, 2, 0xf008},
    {0x4042, 2, 0x9df0},
    {0x4044, 2, 0x209d},
    {0x4046, 2, 0x8c08},
    {0x4048, 2, 0x08f0},
    {0x404a, 2, 0x0036},
    {0x404c, 2, 0x008f},
    {0x404e, 2, 0xf000},
    {0x4050, 2, 0x88f0},
    {0x4052, 2, 0x0488},
    {0x4054, 2, 0xf000},
    {0x4056, 2, 0x3600},
    {0x4058, 2, 0xf000},
    {0x405a, 2, 0x83f0},
    {0x405c, 2, 0x0290},
    {0x405e, 2, 0xf000},
    {0x4060, 2, 0x8bf0},
    {0x4062, 2, 0x2ea3},
    {0x4064, 2, 0xf000},
    {0x4066, 2, 0xa3f0},
    {0x4068, 2, 0x089d},
    {0x406a, 2, 0xf075},
    {0x406c, 2, 0x3003},
    {0x406e, 2, 0x4070},
    {0x4070, 2, 0x216d},
    {0x4072, 2, 0x1cf6},
    {0x4074, 2, 0x8b00},
    {0x4076, 2, 0x5186},
    {0x4078, 2, 0x1300},
    {0x407a, 2, 0x0205},
    {0x407c, 2, 0x36d8},
    {0x407e, 2, 0xf002},
    {0x4080, 2, 0x8387},
    {0x4082, 2, 0xf006},
    {0x4084, 2, 0x8702},
    {0x4086, 2, 0x0d02},
    {0x4088, 2, 0x05f0},
    {0x408a, 2, 0x0383},
    {0x408c, 2, 0xf001},
    {0x408e, 2, 0x87f0},
    {0x4090, 2, 0x0213},
    {0x4092, 2, 0x0036},
    {0x4094, 2, 0xd887},
    {0x4096, 2, 0x020d},
    {0x4098, 2, 0xe0e0},
    {0x409a, 2, 0xe0e0},
    {0x409c, 2, 0xe0e0},
    {0x409e, 2, 0xe0e0},
    {0x40a0, 2, 0xf000},
    {0x40a2, 2, 0x0401},
    {0x40a4, 2, 0xf008},
    {0x40a6, 2, 0x82f0},
    {0x40a8, 2, 0x0883},
    {0x40aa, 2, 0xf009},
    {0x40ac, 2, 0x85f0},
    {0x40ae, 2, 0x2985},
    {0x40b0, 2, 0x87f0},
    {0x40b2, 2, 0x2a87},
    {0x40b4, 2, 0xf63e},
    {0x40b6, 2, 0x88f0},
    {0x40b8, 2, 0x0801},
    {0x40ba, 2, 0x40f0},
    {0x40bc, 2, 0x0800},
    {0x40be, 2, 0x48f0},
    {0x40c0, 2, 0x0882},
    {0x40c2, 2, 0xf008},
    {0x40c4, 2, 0x0401},
    {0x40c6, 2, 0xf008},
    {0x40c8, 2, 0xe0e0},
    {0x40ca, 2, 0xe0e0},
    {0x40cc, 2, 0xe0e0},
    {0x40ce, 2, 0xe0e0},
    {0x40d0, 2, 0xf000},
    {0x40d2, 2, 0x0401},
    {0x40d4, 2, 0xf015},
    {0x40d6, 2, 0x003c},
    {0x40d8, 2, 0xf00e},
    {0x40da, 2, 0x85f0},
    {0x40dc, 2, 0x0687},
    {0x40de, 2, 0xf002},
    {0x40e0, 2, 0x87f0},
    {0x40e2, 2, 0x61e8},
    {0x40e4, 2, 0x3904},
    {0x40e6, 2, 0xf005},
    {0x40e8, 2, 0x3482},
    {0x40ea, 2, 0xf000},
    {0x40ec, 2, 0x3241},
    {0x40ee, 2, 0xf000},
    {0x40f0, 2, 0x3904},
    {0x40f2, 2, 0xf00e},
    {0x40f4, 2, 0x3904},
    {0x40f6, 2, 0xf000},
    {0x40f8, 2, 0x3241},
    {0x40fa, 2, 0xf000},
    {0x40fc, 2, 0x3482},
    {0x40fe, 2, 0xf005},
    {0x4100, 2, 0xc0e6},
    {0x4102, 2, 0xf004},
    {0x4104, 2, 0x3904},
    {0x4106, 2, 0xf003},
    {0x4108, 2, 0xb0f0},
    {0x410a, 2, 0x0000},
    {0x410c, 2, 0x18f0},
    {0x410e, 2, 0x0086},
    {0x4110, 2, 0xf000},
    {0x4112, 2, 0x86f0},
    {0x4114, 2, 0x89b0},
    {0x4116, 2, 0xf000},
    {0x4118, 2, 0xe9f0},
    {0x411a, 2, 0x008a},
    {0x411c, 2, 0xf000},
    {0x411e, 2, 0x0005},
    {0x4120, 2, 0xf000},
    {0x4122, 2, 0xe0e0},
    {0x4124, 2, 0xe0e0},
    {0x4126, 2, 0xe0e0},
    {0x4128, 2, 0x0a35},
    {0x412a, 2, 0x10ef},
    {0x412c, 2, 0x3003},
    {0x412e, 2, 0x30d8},
    {0x4130, 2, 0xf005},
    {0x4132, 2, 0x85f0},
    {0x4134, 2, 0x049a},
    {0x4136, 2, 0x89f0},
    {0x4138, 2, 0x0099},
    {0x413a, 2, 0x97f0},
    {0x413c, 2, 0x0030},
    {0x413e, 2, 0xc0f0},
    {0x4140, 2, 0x0082},
    {0x4142, 2, 0xf000},
    {0x4144, 2, 0x3018},
    {0x4146, 2, 0xf002},
    {0x4148, 2, 0xb520},
    {0x414a, 2, 0x58f0},
    {0x414c, 2, 0x089c},
    {0x414e, 2, 0xf010},
    {0x4150, 2, 0x99b6},
    {0x4152, 2, 0xf003},
    {0x4154, 2, 0xb498},
    {0x4156, 2, 0xa096},
    {0x4158, 2, 0xf000},
    {0x415a, 2, 0xa2f0},
    {0x415c, 2, 0x00a2},
    {0x415e, 2, 0xf008},
    {0x4160, 2, 0x9df0},
    {0x4162, 2, 0x209d},
    {0x4164, 2, 0x8c08},
    {0x4166, 2, 0x08f0},
    {0x4168, 2, 0x0036},
    {0x416a, 2, 0x008f},
    {0x416c, 2, 0x88f0},
    {0x416e, 2, 0x0188},
    {0x4170, 2, 0x3600},
    {0x4172, 2, 0xf000},
    {0x4174, 2, 0x83f0},
    {0x4176, 2, 0x0290},
    {0x4178, 2, 0xf001},
    {0x417a, 2, 0x8bf0},
    {0x417c, 2, 0x2da3},
    {0x417e, 2, 0xf000},
    {0x4180, 2, 0xa3f0},
    {0x4182, 2, 0x089d},
    {0x4184, 2, 0xf06d},
    {0x4186, 2, 0x4070},
    {0x4188, 2, 0x3003},
    {0x418a, 2, 0x214d},
    {0x418c, 2, 0x1ff6},
    {0x418e, 2, 0x0851},
    {0x4190, 2, 0x0245},
    {0x4192, 2, 0x9d36},
    {0x4194, 2, 0xd8f0},
    {0x4196, 2, 0x0083},
    {0x4198, 2, 0xf000},
    {0x419a, 2, 0x87f0},
    {0x419c, 2, 0x0087},
    {0x419e, 2, 0xf000},
    {0x41a0, 2, 0x36d8},
    {0x41a2, 2, 0x020d},
    {0x41a4, 2, 0x0205},
    {0x41a6, 2, 0xf000},
    {0x41a8, 2, 0x36d8},
    {0x41aa, 2, 0xf000},
    {0x41ac, 2, 0x83f0},
    {0x41ae, 2, 0x0087},
    {0x41b0, 2, 0xf000},
    {0x41b2, 2, 0x87f0},
    {0x41b4, 2, 0x0036},
    {0x41b6, 2, 0xd802},
    {0x41b8, 2, 0x0d02},
    {0x41ba, 2, 0x05f0},
    {0x41bc, 2, 0x0036},
    {0x41be, 2, 0xd8f0},
    {0x41c0, 2, 0x0083},
    {0x41c2, 2, 0xf000},
    {0x41c4, 2, 0x87f0},
    {0x41c6, 2, 0x0087},
    {0x41c8, 2, 0xf000},
    {0x41ca, 2, 0x36d8},
    {0x41cc, 2, 0x020d},
    {0x41ce, 2, 0x0205},
    {0x41d0, 2, 0xf000},
    {0x41d2, 2, 0x36d8},
    {0x41d4, 2, 0xf000},
    {0x41d6, 2, 0x83f0},
    {0x41d8, 2, 0x0087},
    {0x41da, 2, 0xf000},
    {0x41dc, 2, 0x8713},
    {0x41de, 2, 0x0036},
    {0x41e0, 2, 0xd802},
    {0x41e2, 2, 0x0de0},
    {0x41e4, 2, 0xe0e0},
    {0x41e6, 2, 0xe0e0},
    {0x41e8, 2, 0x9f13},
    {0x41ea, 2, 0x0041},
    {0x41ec, 2, 0x80f3},
    {0x41ee, 2, 0xf213},
    {0x41f0, 2, 0x00f0},
    {0x41f2, 2, 0x13b8},
    {0x41f4, 2, 0xf04c},
    {0x41f6, 2, 0x9ff0},
    {0x41f8, 2, 0x00b7},
    {0x41fa, 2, 0xf006},
    {0x41fc, 2, 0x0035},
    {0x41fe, 2, 0x10af},
    {0x4200, 2, 0x3003},
    {0x4202, 2, 0x30c0},
    {0x4204, 2, 0xb2f0},
    {0x4206, 2, 0x01b5},
    {0x4208, 2, 0xf001},
    {0x420a, 2, 0x85f0},
    {0x420c, 2, 0x0292},
    {0x420e, 2, 0xf000},
    {0x4210, 2, 0x9a8b},
    {0x4212, 2, 0xf000},
    {0x4214, 2, 0x9997},
    {0x4216, 2, 0xf007},
    {0x4218, 2, 0xb6f0},
    {0x421a, 2, 0x0020},
    {0x421c, 2, 0x5830},
    {0x421e, 2, 0xc040},
    {0x4220, 2, 0x1282},
    {0x4222, 2, 0xf005},
    {0x4224, 2, 0x9cf0},
    {0x4226, 2, 0x01b2},
    {0x4228, 2, 0xf008},
    {0x422a, 2, 0xb8f0},
    {0x422c, 2, 0x0799},
    {0x422e, 2, 0xf005},
    {0x4230, 2, 0x98f0},
    {0x4232, 2, 0x0296},
    {0x4234, 2, 0xa2f0},
    {0x4236, 2, 0x00a2},
    {0x4238, 2, 0xf008},
    {0x423a, 2, 0x9df0},
    {0x423c, 2, 0x02a1},
    {0x423e, 2, 0xf01f},
    {0x4240, 2, 0x1009},
    {0x4242, 2, 0x2220},
    {0x4244, 2, 0x0808},
    {0x4246, 2, 0xf000},
    {0x4248, 2, 0x3600},
    {0x424a, 2, 0xf000},
    {0x424c, 2, 0x88f0},
    {0x424e, 2, 0x0788},
    {0x4250, 2, 0x3600},
    {0x4252, 2, 0xf000},
    {0x4254, 2, 0x83f0},
    {0x4256, 2, 0x0290},
    {0x4258, 2, 0xf016},
    {0x425a, 2, 0x8bf0},
    {0x425c, 2, 0x11a3},
    {0x425e, 2, 0xf000},
    {0x4260, 2, 0xa3f0},
    {0x4262, 2, 0x089d},
    {0x4264, 2, 0xf002},
    {0x4266, 2, 0xa1f0},
    {0x4268, 2, 0x20a1},
    {0x426a, 2, 0xf006},
    {0x426c, 2, 0x4300},
    {0x426e, 2, 0xf049},
    {0x4270, 2, 0x4014},
    {0x4272, 2, 0x8b8e},
    {0x4274, 2, 0x9df0},
    {0x4276, 2, 0x0802},
    {0x4278, 2, 0x02f0},
    {0x427a, 2, 0x00a6},
    {0x427c, 2, 0xf013},
    {0x427e, 2, 0xb283},
    {0x4280, 2, 0x9c36},
    {0x4282, 2, 0x00f0},
    {0x4284, 2, 0x0636},
    {0x4286, 2, 0x009c},
    {0x4288, 2, 0xf008},
    {0x428a, 2, 0x8bf0},
    {0x428c, 2, 0x0083},
    {0x428e, 2, 0xa0f0},
    {0x4290, 2, 0x0630},
    {0x4292, 2, 0x18f0},
    {0x4294, 2, 0x02a3},
    {0x4296, 2, 0xf000},
    {0x4298, 2, 0xa3f0},
    {0x429a, 2, 0x0243},
    {0x429c, 2, 0x00f0},
    {0x429e, 2, 0x049d},
    {0x42a0, 2, 0xf078},
    {0x42a2, 2, 0x3018},
    {0x42a4, 2, 0xf000},
    {0x42a6, 2, 0x9d82},
    {0x42a8, 2, 0xf000},
    {0x42aa, 2, 0x9030},
    {0x42ac, 2, 0xc0f0},
    {0x42ae, 2, 0x1130},
    {0x42b0, 2, 0xc0f0},
    {0x42b2, 2, 0x0082},
    {0x42b4, 2, 0xf001},
    {0x42b6, 2, 0x1009},
    {0x42b8, 2, 0xf02a},
    {0x42ba, 2, 0xa2f0},
    {0x42bc, 2, 0x00a2},
    {0x42be, 2, 0x3018},
    {0x42c0, 2, 0xf007},
    {0x42c2, 2, 0x9df0},
    {0x42c4, 2, 0x1c8c},
    {0x42c6, 2, 0xf005},
    {0x42c8, 2, 0x301f},
    {0x42ca, 2, 0x216d},
    {0x42cc, 2, 0x0a51},
    {0x42ce, 2, 0x1fea},
    {0x42d0, 2, 0x8640},
    {0x42d2, 2, 0xe29f},
    {0x42d4, 2, 0xf009},
    {0x42d6, 2, 0x0005},
    {0x42d8, 2, 0xf000},
    {0x42da, 2, 0x30c0},
    {0x42dc, 2, 0xf001},
    {0x42de, 2, 0x83f0},
    {0x42e0, 2, 0x0036},
    {0x42e2, 2, 0x00f0},
    {0x42e4, 2, 0x0087},
    {0x42e6, 2, 0xf007},
    {0x42e8, 2, 0x87f0},
    {0x42ea, 2, 0x0036},
    {0x42ec, 2, 0xc0f0},
    {0x42ee, 2, 0x0000},
    {0x42f0, 2, 0x0df0},
    {0x42f2, 2, 0x0000},
    {0x42f4, 2, 0x05f0},
    {0x42f6, 2, 0x0030},
    {0x42f8, 2, 0xc0f0},
    {0x42fa, 2, 0x0183},
    {0x42fc, 2, 0xf000},
    {0x42fe, 2, 0x3600},
    {0x4300, 2, 0xf000},
    {0x4302, 2, 0x87f0},
    {0x4304, 2, 0x0787},
    {0x4306, 2, 0xf000},
    {0x4308, 2, 0x36c0},
    {0x430a, 2, 0xf000},
    {0x430c, 2, 0x000f},
    {0x430e, 2, 0xf42a},
    {0x4310, 2, 0x4180},
    {0x4312, 2, 0x1300},
    {0x4314, 2, 0x9ff0},
    {0x4316, 2, 0x00e0},
    {0x4318, 2, 0xe0e0},
    {0x431a, 2, 0xe0e0},
    {0x431c, 2, 0xe0e0},
    {0x431e, 2, 0xe0e0},
    {0x4320, 2, 0xe0e0},
    {0x4322, 2, 0xe0e0},
    {0x4324, 2, 0xe0e0},
    {0x4326, 2, 0xe0e0},
    {0x4328, 2, 0xe0e0},
    {0x432a, 2, 0xe0e0},
    {0x432c, 2, 0xe0e0},
    {0x432e, 2, 0xe0e0},
    {0x4330, 2, 0xe0e0},
    {0x4332, 2, 0xe0e0},
    {0x4334, 2, 0xe0e0},
    {0x4336, 2, 0xe0e0},
    {0x4338, 2, 0xe0e0},
    {0x433a, 2, 0xe0e0},
    {0x433c, 2, 0xe0e0},
    {0x433e, 2, 0xe0e0},
    {0x4340, 2, 0xe0e0},
    {0x4342, 2, 0xe0e0},
    {0x4344, 2, 0xe0e0},
    {0x4346, 2, 0xe0e0},
    {0x4348, 2, 0xe0e0},
    {0x434a, 2, 0xe0e0},
    {0x434c, 2, 0xe0e0},
    {0x434e, 2, 0xe0e0},
    {0x4350, 2, 0xe0e0},
    {0x4352, 2, 0xe0e0},
    {0x4354, 2, 0xe0e0},
    {0x4356, 2, 0xe0e0},
    {0x4358, 2, 0xe0e0},
    {0x435a, 2, 0xe0e0},
    {0x435c, 2, 0xe0e0},
    {0x435e, 2, 0xe0e0},
    {0x4360, 2, 0xe0e0},
    {0x4362, 2, 0xe0e0},
    {0x4364, 2, 0xe0e0},
    {0x4366, 2, 0xe0e0},
    {0x4368, 2, 0xe0e0},
    {0x436a, 2, 0xe0e0},
    {0x436c, 2, 0xe0e0},
    {0x436e, 2, 0xe0e0},
    {0x4370, 2, 0xe0e0},
    {0x4372, 2, 0xe0e0},
    {0x4374, 2, 0xe0e0},
    {0x4376, 2, 0xe0e0},
    {0x4378, 2, 0xe0e0},
    {0x437a, 2, 0xe0e0},
    {0x437c, 2, 0xe0e0},
    {0x437e, 2, 0xe0e0},
    {0x4380, 2, 0xe0e0},
    {0x4382, 2, 0xe0e0},
    {0x4384, 2, 0xe0e0},
    {0x4386, 2, 0xe0e0},
    {0x4388, 2, 0xe0e0},
    {0x438a, 2, 0xe0e0},
    {0x438c, 2, 0xe0e0},
    {0x438e, 2, 0xe0e0},
    {0x4390, 2, 0xe0e0},
    {0x4392, 2, 0xe0e0},
    {0x4394, 2, 0xe0e0},
    {0x4396, 2, 0xe0e0},
    {0x4398, 2, 0xe0e0},
    {0x439a, 2, 0xe0e0},
    {0x439c, 2, 0xe0e0},
    {0x439e, 2, 0xe0e0},
    {0x43a0, 2, 0xe0e0},
    {0x43a2, 2, 0xe0e0},
    {0x43a4, 2, 0xe0e0},
    {0x43a6, 2, 0xe0e0},
    {0x43a8, 2, 0xe0e0},
    {0x43aa, 2, 0xe0e0},
    {0x43ac, 2, 0xe0e0},
    {0x43ae, 2, 0xe0e0},
    {0x43b0, 2, 0xe0e0},
    {0x43b2, 2, 0xe0e0},
    {0x43b4, 2, 0xe0e0},
    {0x43b6, 2, 0xe0e0},
    {0x43b8, 2, 0xe0e0},
    {0x43ba, 2, 0xe0e0},
    {0x43bc, 2, 0xe0e0},
    {0x43be, 2, 0xe0e0},
    {0x43c0, 2, 0xe0e0},
    {0x43c2, 2, 0xe0e0},
    {0x43c4, 2, 0xe0e0},
    {0x43c6, 2, 0xe0e0},
    {0x43c8, 2, 0xe0e0},
    {0x43ca, 2, 0xe0e0},
    {0x43cc, 2, 0xe0e0},
    {0x43ce, 2, 0xe0e0},
    {0x43d0, 2, 0xe0e0},
    {0x43d2, 2, 0xe0e0},
    {0x43d4, 2, 0xe0e0},
    {0x43d6, 2, 0xe0e0},
    {0x43d8, 2, 0xe0e0},
    {0x43da, 2, 0xe0e0},
    {0x43dc, 2, 0xe0e0},
    {0x43de, 2, 0xe0e0},
    {0x43e0, 2, 0xe0e0},
    {0x43e2, 2, 0xe0e0},
    {0x43e4, 2, 0xe0e0},
    {0x43e6, 2, 0xe0e0},
    {0x43e8, 2, 0xe0e0},
    {0x43ea, 2, 0xe0e0},
    {0x43ec, 2, 0xe0e0},
    {0x43ee, 2, 0xe0e0},
    {0x43f0, 2, 0xe0e0},
    {0x43f2, 2, 0xe0e0},
    {0x43f4, 2, 0xe0e0},
    {0x43f6, 2, 0xe0e0},
    {0x43f8, 2, 0xe0e0},
    {0x43fa, 2, 0xe0e0},
    {0x43fc, 2, 0xe0e0},
    {0x43fe, 2, 0xe0e0},
    {0x4400, 2, 0xe0e0},
    {0x4402, 2, 0xe0e0},
    {0x4404, 2, 0xe0e0},
    {0x4406, 2, 0xe0e0},
    {0x4408, 2, 0xe0e0},
    {0x440a, 2, 0xe0e0},
    {0x440c, 2, 0xe0e0},
    {0x440e, 2, 0xe0e0},
    {0x4410, 2, 0xe0e0},
    {0x4412, 2, 0xe0e0},
    {0x4414, 2, 0xe0e0},
    {0x4416, 2, 0xe0e0},
    {0x4418, 2, 0xe0e0},
    {0x441a, 2, 0xe0e0},
    {0x441c, 2, 0xe0e0},
    {0x441e, 2, 0xe0e0},
    {0x4420, 2, 0xe0e0},
    {0x4422, 2, 0xe0e0},
    {0x4424, 2, 0xe0e0},
    {0x4426, 2, 0xe0e0},
    {0x4428, 2, 0xe0e0},
    {0x442a, 2, 0xe0e0},
    {0x442c, 2, 0xe0e0},
    {0x442e, 2, 0xe0e0},
    {0x4430, 2, 0xe0e0},
    {0x4432, 2, 0xe0e0},
    {0x4434, 2, 0xe0e0},
    {0x4436, 2, 0xe0e0},
    {0x4438, 2, 0xe0e0},
    {0x443a, 2, 0xe0e0},
    {0x443c, 2, 0xe0e0},
    {0x443e, 2, 0xe0e0},
    {0x4440, 2, 0xe0e0},
    {0x4442, 2, 0xe0e0},
    {0x4444, 2, 0xe0e0},
    {0x4446, 2, 0xe0e0},
    {0x4448, 2, 0xe0e0},
    {0x444a, 2, 0xe0e0},
    {0x444c, 2, 0xe0e0},
    {0x444e, 2, 0xe0e0},
    {0x4450, 2, 0xe0e0},
    {0x4452, 2, 0xe0e0},
    {0x4454, 2, 0xe0e0},
    {0x4456, 2, 0xe0e0},
    {0x4458, 2, 0xe0e0},
    {0x445a, 2, 0xe0e0},
    {0x445c, 2, 0xe0e0},
    {0x445e, 2, 0xe0e0},
    {0x4460, 2, 0xe0e0},
    {0x4462, 2, 0xe0e0},
    {0x4464, 2, 0xe0e0},
    {0x4466, 2, 0xe0e0},
    {0x4468, 2, 0xe0e0},
    {0x446a, 2, 0xe0e0},
    {0x446c, 2, 0xe0e0},
    {0x446e, 2, 0xe0e0},
    {0x4470, 2, 0xe0e0},
    {0x4472, 2, 0xe0e0},
    {0x4474, 2, 0xe0e0},
    {0x4476, 2, 0xe0e0},
    {0x4478, 2, 0xe0e0},
    {0x447a, 2, 0xe0e0},
    {0x447c, 2, 0xe0e0},
    {0x447e, 2, 0xe0e0},
    {0x4480, 2, 0xe0e0},
    {0x4482, 2, 0xe0e0},
    {0x4484, 2, 0xe0e0},
    {0x4486, 2, 0xe0e0},
    {0x4488, 2, 0xe0e0},
    {0x448a, 2, 0xe0e0},
    {0x448c, 2, 0xe0e0},
    {0x448e, 2, 0xe0e0},
    {0x4490, 2, 0xe0e0},
    {0x4492, 2, 0xe0e0},
    {0x4494, 2, 0xe0e0},
    {0x4496, 2, 0xe0e0},
    {0x4498, 2, 0xe0e0},
    {0x449a, 2, 0xe0e0},
    {0x449c, 2, 0xe0e0},
    {0x449e, 2, 0xe0e0},
    {0x44a0, 2, 0xe0e0},
    {0x44a2, 2, 0xe0e0},
    {0x44a4, 2, 0xe0e0},
    {0x44a6, 2, 0xe0e0},
    {0x44a8, 2, 0xe0e0},
    {0x44aa, 2, 0xe0e0},
    {0x44ac, 2, 0xe0e0},
    {0x44ae, 2, 0xe0e0},
    {0x44b0, 2, 0xe0e0},
    {0x44b2, 2, 0xe0e0},
    {0x44b4, 2, 0xe0e0},
    {0x5500, 2, 0x0000},
    {0x5502, 2, 0x0002},
    {0x5504, 2, 0x0006},
    {0x5506, 2, 0x0009},
    {0x5508, 2, 0x000f},
    {0x550a, 2, 0x0010},
    {0x550c, 2, 0x0011},
    {0x550e, 2, 0x0012},
    {0x5510, 2, 0x0019},
    {0x5512, 2, 0x0020},
    {0x5514, 2, 0x0021},
    {0x5516, 2, 0x0023},
    {0x5518, 2, 0x0026},
    {0x551a, 2, 0x002b},
    {0x551c, 2, 0x002f},
    {0x551e, 2, 0x0030},
    {0x5400, 2, 0x0100},
    {0x5402, 2, 0x2106},
    {0x5404, 2, 0x1101},
    {0x5406, 2, 0x3106},
    {0x5408, 2, 0x7100},
    {0x540a, 2, 0x8107},
    {0x540c, 2, 0xb101},
    {0x540e, 2, 0xd101},
    {0x5410, 2, 0xf12e},
    {0x5412, 2, 0xf112},
    {0x5414, 2, 0xf184},
    {0x5416, 2, 0xf224},
    {0x5418, 2, 0xf306},
    {0x541a, 2, 0xf446},
    {0x541c, 2, 0xf609},
    {0x541e, 2, 0xf887},
    {0x5420, 2, 0xfc0b},
    {0x5422, 2, 0xfc0b},
    {0x5424, 2, 0xfffa},
    {0x5426, 2, 0x5557},
    {0x5428, 2, 0x0005},
    {0x542a, 2, 0xa550},
    {0x542c, 2, 0xaaaa},
    {0x542e, 2, 0x000a},
    {0x5460, 2, 0x2269},
    {0x5462, 2, 0x0b87},
    {0x5464, 2, 0x0b87},
    {0x5466, 2, 0x0983},
    {0x5498, 2, 0x225e},
    {0x549a, 2, 0xbcaa},
    {0x549c, 2, 0xbcaa},
    {0x549e, 2, 0xbdaa},
    {0x3060, 2, 0xff01},
    {0x44ba, 2, 0x0050},
    {0x44bc, 2, 0xbcaa},
    {0x44c0, 2, 0x4070},
    {0x44c4, 2, 0x04d0},
    {0x44c6, 2, 0x17e2},
    {0x44c8, 2, 0xea43},
    {0x44ca, 2, 0x000e},
    {0x44cc, 2, 0x7777},
    {0x44ce, 2, 0x8ba4},
    {0x44d0, 2, 0x1735},
    {0x44d2, 2, 0x0b87},
    {0x44d4, 2, 0x8000},
    {0x44d6, 2, 0xf206},
    {0x44d8, 2, 0xaafa},
    {0x44da, 2, 0xe001},
    {0x44de, 2, 0x9bbc},
    {0x44e0, 2, 0x283c},
    {0x44e2, 2, 0x2821},
    {0x44e4, 2, 0x8000},
    {0x44e6, 2, 0x503f},
    {0x32a4, 2, 0x0000},
    {0x333c, 2, 0x0001},
    {0x301a, 2, 0x0000},
    {0x3600, 2, 0x94df},
    {0x3700, 2, 0x0001},
    {0x3980, 2, 0x0003},
    {0x36c0, 2, 0x0001},
    {0x36de, 2, 0x002a},
    {0x301a, 2, 0x0008},
    {0x3060, 2, 0x0000},
    {0x3060, 2, 0xff01},
    {0x3980, 2, 0x0001},
    {0x3340, 2, 0x0c00},
    {0x3340, 2, 0x1c00},
    {0x36c0, 2, 0x0001},
    {0x400, 2, 0x3},
    {0x404, 2, 0xA63},
    {0x0406, 2, 0x400},
    {0x3C58, 2, 0x8},
    {0x3C40, 2, 0x2},
    {0x3C42, 2, 0x257},
    {0x3C44, 2, 0x588},
    {0x3C46, 2, 0xC0C},
    {0x3C48, 2, 0x300},
    {0x3C4A, 2, 0x631},
    {0x3C4C, 2, 0x909},
    {0x3C4E, 2, 0x3A9},
    {0x3C50, 2, 0x6DA},
    {0x3C52, 2, 0xA0A},
    {0x3C60, 2, 0x110},
    {0x3C62, 2, 0x100},
    {0x3C64, 2, 0x108},
    {0x3C66, 2, 0xFC},
    {0x3EC8, 2, 0x0},
    {0x3ECA, 2, 0x0},
    {0x034C, 2, 0x400},
    {0x034E, 2, 0x3B4},
    {0x3C7A, 2, 0x0},
    {0x3C6A, 2, 0x0},
    {0x3C70, 2, 0x0},
    {0x3C6C, 2, 0x400},
    {0x3C72, 2, 0x400},
    {0x3C74, 2, 0x3},
    {0x3C76, 2, 0x20},
    {0x3F18, 2, 0x7B70},
    {0x3F1A, 2, 0x102B},

    {0x3062, 2, 0x002b}, // fenghuan add

    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE
    {0x0100, 1, 0x01},
    {0x44D6, 2, 0xB206},
};
#endif

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_2704x2512_30_table[] =
{
    {0x0103, 0x3201, 0x01},

    {0x0304, 2, 0x0002}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 0x0056}, // VT_PLL_MULTIPLIER
    {0x0300, 2, 0x0006}, // VT_PIX_CLK_DIV
    {0x0302, 2, 0x0001}, // VT_SYS_CLK_DIV
    {0x030C, 2, 0x0007}, // OP_PRE_PLL_CLK_DIV
    {0x030E, 2, 0x0140}, // OP_PLL_MULTIPLIER
    {0x0308, 2, 0x000A}, // OP_PIX_CLK_DIV
    {0x030A, 2, 0x0001}, // OP_SYS_CLK_DIV
    {0x0344, 2, 0x4b8},
    {0x0348, 2, 0xF47},
    {0x0346, 2, 0x298},
    {0x034A, 2, 0xC67},
    {0x034C, 2, 0xA90},
    {0x034E, 2, 0x9d0},
    {0x0380, 2, 0x0001}, // X_EVEN_INC
    {0x0382, 2, 0x0001}, // X_ODD_INC
    {0x0384, 2, 0x0001}, // Y_EVEN_INC
    {0x0386, 2, 0x0001}, // Y_ODD_INC
    {0x0900, 1, 0x00}, // BINNING_MODE
    {0x0901, 1, 0x11}, // BINNING_TYPE
    {0x0342, 2, 0x7312}, //{0x0342, 2, 0x2DF0}, // LINE_LENGTH_PCK
    {0x0340, 2, 0x0F1E}, //{0x0340, 2, 0x0F1E}, // FRAME_LENGTH_LINES
    {0x0202, 2, 0x05dc}, //{0x0202, 2, 0x123F}, // COARSE_INTEGRATION_TIME
    {0x0112, 2, 0x0A0A}, // CSI_DATA_FORMAT
    {0x0114, 1, 0x03}, // CSI_LANE_MODE
    {0x0800, 1, 0x0C}, // TCLK_POST
    {0x0801, 1, 0x06}, // THS_PREPARE
    {0x0802, 1, 0x0B}, // THS_ZERO_MIN
    {0x0803, 1, 0x08}, // THS_TRAIL
    {0x0804, 1, 0x0A}, // TCLK_TRAIL_MIN
    {0x0805, 1, 0x07}, // TCLK_PREPARE
    {0x0806, 1, 0x21}, // TCLK_ZERO
    {0x0807, 1, 0x07}, // TLPX
    {0x082A, 1, 0x0E}, // TWAKEUP
    {0x082B, 1, 0x0B}, // TINIT
    {0x082C, 1, 0x0C}, // THS_EXIT
    {0x3F06, 2, 0x00C0}, // MIPI_TIMING_2
    {0x3F0A, 2, 0x0000}, //{0x3F0A, 2, 0x8000}, // MIPI_TIMING_4
    {0x3F0C, 2, 0x0009}, // MIPI_TIMING_5
    {0x3F20, 2, 0x0808}, // MIPI_PHY_TRIM_MSB
    {0x3F02, 2, 0x0002}, // PHY_CTRL
    {0x4000, 2, 0x0114}, // DYNAMIC_SEQRAM_00
    {0x4002, 2, 0x1A25}, // DYNAMIC_SEQRAM_02
    {0x4004, 2, 0x3DFF}, // DYNAMIC_SEQRAM_04
    {0x4006, 2, 0xFFFF}, // DYNAMIC_SEQRAM_06
    {0x4008, 2, 0x0A35}, // DYNAMIC_SEQRAM_08
    {0x400A, 2, 0x108F}, // DYNAMIC_SEQRAM_0A
    {0x400C, 2, 0x3003}, // DYNAMIC_SEQRAM_0C
    {0x400E, 2, 0x30D8}, // DYNAMIC_SEQRAM_0E
    {0x4010, 2, 0xF002}, // DYNAMIC_SEQRAM_10
    {0x4012, 2, 0x92B5}, // DYNAMIC_SEQRAM_12
    {0x4014, 2, 0xF000}, // DYNAMIC_SEQRAM_14
    {0x4016, 2, 0x85F0}, // DYNAMIC_SEQRAM_16
    {0x4018, 2, 0x0091}, // DYNAMIC_SEQRAM_18
    {0x401A, 2, 0xF002}, // DYNAMIC_SEQRAM_1A
    {0x401C, 2, 0x9A89}, // DYNAMIC_SEQRAM_1C
    {0x401E, 2, 0xF000}, // DYNAMIC_SEQRAM_1E
    {0x4020, 2, 0x9997}, // DYNAMIC_SEQRAM_20
    {0x4022, 2, 0xF000}, // DYNAMIC_SEQRAM_22
    {0x4024, 2, 0x30C0}, // DYNAMIC_SEQRAM_24
    {0x4026, 2, 0xF000}, // DYNAMIC_SEQRAM_26
    {0x4028, 2, 0x82F0}, // DYNAMIC_SEQRAM_28
    {0x402A, 2, 0x0030}, // DYNAMIC_SEQRAM_2A
    {0x402C, 2, 0x18F0}, // DYNAMIC_SEQRAM_2C
    {0x402E, 2, 0x0320}, // DYNAMIC_SEQRAM_2E
    {0x4030, 2, 0x58F0}, // DYNAMIC_SEQRAM_30
    {0x4032, 2, 0x089C}, // DYNAMIC_SEQRAM_32
    {0x4034, 2, 0xF011}, // DYNAMIC_SEQRAM_34
    {0x4036, 2, 0xB6F0}, // DYNAMIC_SEQRAM_36
    {0x4038, 2, 0x0099}, // DYNAMIC_SEQRAM_38
    {0x403A, 2, 0xF001}, // DYNAMIC_SEQRAM_3A
    {0x403C, 2, 0xB498}, // DYNAMIC_SEQRAM_3C
    {0x403E, 2, 0xA096}, // DYNAMIC_SEQRAM_3E
    {0x4040, 2, 0xF000}, // DYNAMIC_SEQRAM_40
    {0x4042, 2, 0xA2F0}, // DYNAMIC_SEQRAM_42
    {0x4044, 2, 0x00A2}, // DYNAMIC_SEQRAM_44
    {0x4046, 2, 0xF008}, // DYNAMIC_SEQRAM_46
    {0x4048, 2, 0x9DF0}, // DYNAMIC_SEQRAM_48
    {0x404A, 2, 0x209D}, // DYNAMIC_SEQRAM_4A
    {0x404C, 2, 0x8C08}, // DYNAMIC_SEQRAM_4C
    {0x404E, 2, 0x08F0}, // DYNAMIC_SEQRAM_4E
    {0x4050, 2, 0x0036}, // DYNAMIC_SEQRAM_50
    {0x4052, 2, 0x008F}, // DYNAMIC_SEQRAM_52
    {0x4054, 2, 0xF000}, // DYNAMIC_SEQRAM_54
    {0x4056, 2, 0x88F0}, // DYNAMIC_SEQRAM_56
    {0x4058, 2, 0x0488}, // DYNAMIC_SEQRAM_58
    {0x405A, 2, 0xF000}, // DYNAMIC_SEQRAM_5A
    {0x405C, 2, 0x3600}, // DYNAMIC_SEQRAM_5C
    {0x405E, 2, 0xF000}, // DYNAMIC_SEQRAM_5E
    {0x4060, 2, 0x83F0}, // DYNAMIC_SEQRAM_60
    {0x4062, 2, 0x0290}, // DYNAMIC_SEQRAM_62
    {0x4064, 2, 0xF000}, // DYNAMIC_SEQRAM_64
    {0x4066, 2, 0x8BF0}, // DYNAMIC_SEQRAM_66
    {0x4068, 2, 0x2EA3}, // DYNAMIC_SEQRAM_68
    {0x406A, 2, 0xF000}, // DYNAMIC_SEQRAM_6A
    {0x406C, 2, 0xA3F0}, // DYNAMIC_SEQRAM_6C
    {0x406E, 2, 0x089D}, // DYNAMIC_SEQRAM_6E
    {0x4070, 2, 0xF075}, // DYNAMIC_SEQRAM_70
    {0x4072, 2, 0x3003}, // DYNAMIC_SEQRAM_72
    {0x4074, 2, 0x4070}, // DYNAMIC_SEQRAM_74
    {0x4076, 2, 0x216D}, // DYNAMIC_SEQRAM_76
    {0x4078, 2, 0x1CF6}, // DYNAMIC_SEQRAM_78
    {0x407A, 2, 0x8B00}, // DYNAMIC_SEQRAM_7A
    {0x407C, 2, 0x5186}, // DYNAMIC_SEQRAM_7C
    {0x407E, 2, 0x1300}, // DYNAMIC_SEQRAM_7E
    {0x4080, 2, 0x0205}, // DYNAMIC_SEQRAM_80
    {0x4082, 2, 0x36D8}, // DYNAMIC_SEQRAM_82
    {0x4084, 2, 0xF002}, // DYNAMIC_SEQRAM_84
    {0x4086, 2, 0x8387}, // DYNAMIC_SEQRAM_86
    {0x4088, 2, 0xF006}, // DYNAMIC_SEQRAM_88
    {0x408A, 2, 0x8702}, // DYNAMIC_SEQRAM_8A
    {0x408C, 2, 0x0D02}, // DYNAMIC_SEQRAM_8C
    {0x408E, 2, 0x05F0}, // DYNAMIC_SEQRAM_8E
    {0x4090, 2, 0x0383}, // DYNAMIC_SEQRAM_90
    {0x4092, 2, 0xF001}, // DYNAMIC_SEQRAM_92
    {0x4094, 2, 0x87F0}, // DYNAMIC_SEQRAM_94
    {0x4096, 2, 0x0213}, // DYNAMIC_SEQRAM_96
    {0x4098, 2, 0x0036}, // DYNAMIC_SEQRAM_98
    {0x409A, 2, 0xD887}, // DYNAMIC_SEQRAM_9A
    {0x409C, 2, 0x020D}, // DYNAMIC_SEQRAM_9C
    {0x409E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_9E
    {0x40A0, 2, 0xF000}, // DYNAMIC_SEQRAM_A0
    {0x40A2, 2, 0x0401}, // DYNAMIC_SEQRAM_A2
    {0x40A4, 2, 0xF008}, // DYNAMIC_SEQRAM_A4
    {0x40A6, 2, 0x82F0}, // DYNAMIC_SEQRAM_A6
    {0x40A8, 2, 0x0883}, // DYNAMIC_SEQRAM_A8
    {0x40AA, 2, 0xF009}, // DYNAMIC_SEQRAM_AA
    {0x40AC, 2, 0x85F0}, // DYNAMIC_SEQRAM_AC
    {0x40AE, 2, 0x2985}, // DYNAMIC_SEQRAM_AE
    {0x40B0, 2, 0x87F0}, // DYNAMIC_SEQRAM_B0
    {0x40B2, 2, 0x2A87}, // DYNAMIC_SEQRAM_B2
    {0x40B4, 2, 0xF63E}, // DYNAMIC_SEQRAM_B4
    {0x40B6, 2, 0x88F0}, // DYNAMIC_SEQRAM_B6
    {0x40B8, 2, 0x0801}, // DYNAMIC_SEQRAM_B8
    {0x40BA, 2, 0x40F0}, // DYNAMIC_SEQRAM_BA
    {0x40BC, 2, 0x0800}, // DYNAMIC_SEQRAM_BC
    {0x40BE, 2, 0x48F0}, // DYNAMIC_SEQRAM_BE
    {0x40C0, 2, 0x0882}, // DYNAMIC_SEQRAM_C0
    {0x40C2, 2, 0xF008}, // DYNAMIC_SEQRAM_C2
    {0x40C4, 2, 0x0401}, // DYNAMIC_SEQRAM_C4
    {0x40C6, 2, 0xF008}, // DYNAMIC_SEQRAM_C6
    {0x40C8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_C8
    {0x40CA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CA
    {0x40CC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CC
    {0x40CE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_CE
    {0x40D0, 2, 0xF000}, // DYNAMIC_SEQRAM_D0
    {0x40D2, 2, 0x0401}, // DYNAMIC_SEQRAM_D2
    {0x40D4, 2, 0xF015}, // DYNAMIC_SEQRAM_D4
    {0x40D6, 2, 0x003C}, // DYNAMIC_SEQRAM_D6
    {0x40D8, 2, 0xF00E}, // DYNAMIC_SEQRAM_D8
    {0x40DA, 2, 0x85F0}, // DYNAMIC_SEQRAM_DA
    {0x40DC, 2, 0x0687}, // DYNAMIC_SEQRAM_DC
    {0x40DE, 2, 0xF002}, // DYNAMIC_SEQRAM_DE
    {0x40E0, 2, 0x87F0}, // DYNAMIC_SEQRAM_E0
    {0x40E2, 2, 0x61E8}, // DYNAMIC_SEQRAM_E2
    {0x40E4, 2, 0x3904}, // DYNAMIC_SEQRAM_E4
    {0x40E6, 2, 0xF005}, // DYNAMIC_SEQRAM_E6
    {0x40E8, 2, 0x3482}, // DYNAMIC_SEQRAM_E8
    {0x40EA, 2, 0xF000}, // DYNAMIC_SEQRAM_EA
    {0x40EC, 2, 0x3241}, // DYNAMIC_SEQRAM_EC
    {0x40EE, 2, 0xF000}, // DYNAMIC_SEQRAM_EE
    {0x40F0, 2, 0x3904}, // DYNAMIC_SEQRAM_F0
    {0x40F2, 2, 0xF00E}, // DYNAMIC_SEQRAM_F2
    {0x40F4, 2, 0x3904}, // DYNAMIC_SEQRAM_F4
    {0x40F6, 2, 0xF000}, // DYNAMIC_SEQRAM_F6
    {0x40F8, 2, 0x3241}, // DYNAMIC_SEQRAM_F8
    {0x40FA, 2, 0xF000}, // DYNAMIC_SEQRAM_FA
    {0x40FC, 2, 0x3482}, // DYNAMIC_SEQRAM_FC
    {0x40FE, 2, 0xF005}, // DYNAMIC_SEQRAM_FE
    {0x4100, 2, 0xC0E6}, // DYNAMIC_SEQRAM_100
    {0x4102, 2, 0xF004}, // DYNAMIC_SEQRAM_102
    {0x4104, 2, 0x3904}, // DYNAMIC_SEQRAM_104
    {0x4106, 2, 0xF003}, // DYNAMIC_SEQRAM_106
    {0x4108, 2, 0xB0F0}, // DYNAMIC_SEQRAM_108
    {0x410A, 2, 0x0000}, // DYNAMIC_SEQRAM_10A
    {0x410C, 2, 0x18F0}, // DYNAMIC_SEQRAM_10C
    {0x410E, 2, 0x0086}, // DYNAMIC_SEQRAM_10E
    {0x4110, 2, 0xF000}, // DYNAMIC_SEQRAM_110
    {0x4112, 2, 0x86F0}, // DYNAMIC_SEQRAM_112
    {0x4114, 2, 0x89B0}, // DYNAMIC_SEQRAM_114
    {0x4116, 2, 0xF000}, // DYNAMIC_SEQRAM_116
    {0x4118, 2, 0xE9F0}, // DYNAMIC_SEQRAM_118
    {0x411A, 2, 0x008A}, // DYNAMIC_SEQRAM_11A
    {0x411C, 2, 0xF000}, // DYNAMIC_SEQRAM_11C
    {0x411E, 2, 0x0005}, // DYNAMIC_SEQRAM_11E
    {0x4120, 2, 0xF000}, // DYNAMIC_SEQRAM_120
    {0x4122, 2, 0xE0E0}, // DYNAMIC_SEQRAM_122
    {0x4124, 2, 0xE0E0}, // DYNAMIC_SEQRAM_124
    {0x4126, 2, 0xE0E0}, // DYNAMIC_SEQRAM_126
    {0x4128, 2, 0x0A35}, // DYNAMIC_SEQRAM_128
    {0x412A, 2, 0x108F}, // DYNAMIC_SEQRAM_12A
    {0x412C, 2, 0x3003}, // DYNAMIC_SEQRAM_12C
    {0x412E, 2, 0x30D8}, // DYNAMIC_SEQRAM_12E
    {0x4130, 2, 0xF005}, // DYNAMIC_SEQRAM_130
    {0x4132, 2, 0x8592}, // DYNAMIC_SEQRAM_132
    {0x4134, 2, 0xF002}, // DYNAMIC_SEQRAM_134
    {0x4136, 2, 0x919A}, // DYNAMIC_SEQRAM_136
    {0x4138, 2, 0x89F0}, // DYNAMIC_SEQRAM_138
    {0x413A, 2, 0x0099}, // DYNAMIC_SEQRAM_13A
    {0x413C, 2, 0x97F0}, // DYNAMIC_SEQRAM_13C
    {0x413E, 2, 0x0030}, // DYNAMIC_SEQRAM_13E
    {0x4140, 2, 0xC0F0}, // DYNAMIC_SEQRAM_140
    {0x4142, 2, 0x0082}, // DYNAMIC_SEQRAM_142
    {0x4144, 2, 0xF000}, // DYNAMIC_SEQRAM_144
    {0x4146, 2, 0x3018}, // DYNAMIC_SEQRAM_146
    {0x4148, 2, 0xF002}, // DYNAMIC_SEQRAM_148
    {0x414A, 2, 0xB520}, // DYNAMIC_SEQRAM_14A
    {0x414C, 2, 0x58F0}, // DYNAMIC_SEQRAM_14C
    {0x414E, 2, 0x089C}, // DYNAMIC_SEQRAM_14E
    {0x4150, 2, 0xF011}, // DYNAMIC_SEQRAM_150
    {0x4152, 2, 0xB6F0}, // DYNAMIC_SEQRAM_152
    {0x4154, 2, 0x0099}, // DYNAMIC_SEQRAM_154
    {0x4156, 2, 0xF001}, // DYNAMIC_SEQRAM_156
    {0x4158, 2, 0xB498}, // DYNAMIC_SEQRAM_158
    {0x415A, 2, 0xA096}, // DYNAMIC_SEQRAM_15A
    {0x415C, 2, 0xF000}, // DYNAMIC_SEQRAM_15C
    {0x415E, 2, 0xA2F0}, // DYNAMIC_SEQRAM_15E
    {0x4160, 2, 0x00A2}, // DYNAMIC_SEQRAM_160
    {0x4162, 2, 0xF008}, // DYNAMIC_SEQRAM_162
    {0x4164, 2, 0x9DF0}, // DYNAMIC_SEQRAM_164
    {0x4166, 2, 0x209D}, // DYNAMIC_SEQRAM_166
    {0x4168, 2, 0x8C08}, // DYNAMIC_SEQRAM_168
    {0x416A, 2, 0x08F0}, // DYNAMIC_SEQRAM_16A
    {0x416C, 2, 0x0036}, // DYNAMIC_SEQRAM_16C
    {0x416E, 2, 0x008F}, // DYNAMIC_SEQRAM_16E
    {0x4170, 2, 0x88F0}, // DYNAMIC_SEQRAM_170
    {0x4172, 2, 0x0188}, // DYNAMIC_SEQRAM_172
    {0x4174, 2, 0x3600}, // DYNAMIC_SEQRAM_174
    {0x4176, 2, 0xF000}, // DYNAMIC_SEQRAM_176
    {0x4178, 2, 0x83F0}, // DYNAMIC_SEQRAM_178
    {0x417A, 2, 0x0290}, // DYNAMIC_SEQRAM_17A
    {0x417C, 2, 0xF001}, // DYNAMIC_SEQRAM_17C
    {0x417E, 2, 0x8BF0}, // DYNAMIC_SEQRAM_17E
    {0x4180, 2, 0x2DA3}, // DYNAMIC_SEQRAM_180
    {0x4182, 2, 0xF000}, // DYNAMIC_SEQRAM_182
    {0x4184, 2, 0xA3F0}, // DYNAMIC_SEQRAM_184
    {0x4186, 2, 0x089D}, // DYNAMIC_SEQRAM_186
    {0x4188, 2, 0xF06D}, // DYNAMIC_SEQRAM_188
    {0x418A, 2, 0x4070}, // DYNAMIC_SEQRAM_18A
    {0x418C, 2, 0x3003}, // DYNAMIC_SEQRAM_18C
    {0x418E, 2, 0x214D}, // DYNAMIC_SEQRAM_18E
    {0x4190, 2, 0x1FF6}, // DYNAMIC_SEQRAM_190
    {0x4192, 2, 0x0851}, // DYNAMIC_SEQRAM_192
    {0x4194, 2, 0x0245}, // DYNAMIC_SEQRAM_194
    {0x4196, 2, 0x9D36}, // DYNAMIC_SEQRAM_196
    {0x4198, 2, 0xD8F0}, // DYNAMIC_SEQRAM_198
    {0x419A, 2, 0x0083}, // DYNAMIC_SEQRAM_19A
    {0x419C, 2, 0xF000}, // DYNAMIC_SEQRAM_19C
    {0x419E, 2, 0x87F0}, // DYNAMIC_SEQRAM_19E
    {0x41A0, 2, 0x0087}, // DYNAMIC_SEQRAM_1A0
    {0x41A2, 2, 0xF000}, // DYNAMIC_SEQRAM_1A2
    {0x41A4, 2, 0x36D8}, // DYNAMIC_SEQRAM_1A4
    {0x41A6, 2, 0x020D}, // DYNAMIC_SEQRAM_1A6
    {0x41A8, 2, 0x0205}, // DYNAMIC_SEQRAM_1A8
    {0x41AA, 2, 0xF000}, // DYNAMIC_SEQRAM_1AA
    {0x41AC, 2, 0x36D8}, // DYNAMIC_SEQRAM_1AC
    {0x41AE, 2, 0xF000}, // DYNAMIC_SEQRAM_1AE
    {0x41B0, 2, 0x83F0}, // DYNAMIC_SEQRAM_1B0
    {0x41B2, 2, 0x0087}, // DYNAMIC_SEQRAM_1B2
    {0x41B4, 2, 0xF000}, // DYNAMIC_SEQRAM_1B4
    {0x41B6, 2, 0x87F0}, // DYNAMIC_SEQRAM_1B6
    {0x41B8, 2, 0x0036}, // DYNAMIC_SEQRAM_1B8
    {0x41BA, 2, 0xD802}, // DYNAMIC_SEQRAM_1BA
    {0x41BC, 2, 0x0D02}, // DYNAMIC_SEQRAM_1BC
    {0x41BE, 2, 0x05F0}, // DYNAMIC_SEQRAM_1B3
    {0x41C0, 2, 0x0036}, // DYNAMIC_SEQRAM_1C0
    {0x41C2, 2, 0xD8F0}, // DYNAMIC_SEQRAM_1C2
    {0x41C4, 2, 0x0083}, // DYNAMIC_SEQRAM_1C4
    {0x41C6, 2, 0xF000}, // DYNAMIC_SEQRAM_1C6
    {0x41C8, 2, 0x87F0}, // DYNAMIC_SEQRAM_1C8
    {0x41CA, 2, 0x0087}, // DYNAMIC_SEQRAM_1CA
    {0x41CC, 2, 0xF000}, // DYNAMIC_SEQRAM_1CC
    {0x41CE, 2, 0x36D8}, // DYNAMIC_SEQRAM_1CE
    {0x41D0, 2, 0x020D}, // DYNAMIC_SEQRAM_1D0
    {0x41D2, 2, 0x0205}, // DYNAMIC_SEQRAM_1D2
    {0x41D4, 2, 0xF000}, // DYNAMIC_SEQRAM_1D4
    {0x41D6, 2, 0x36D8}, // DYNAMIC_SEQRAM_1D6
    {0x41D8, 2, 0xF000}, // DYNAMIC_SEQRAM_1D8
    {0x41DA, 2, 0x83F0}, // DYNAMIC_SEQRAM_1DA
    {0x41DC, 2, 0x0087}, // DYNAMIC_SEQRAM_1DC
    {0x41DE, 2, 0xF000}, // DYNAMIC_SEQRAM_1DE
    {0x41E0, 2, 0x8713}, // DYNAMIC_SEQRAM_1E0
    {0x41E2, 2, 0x0036}, // DYNAMIC_SEQRAM_1E2
    {0x41E4, 2, 0xD802}, // DYNAMIC_SEQRAM_1E4
    {0x41E6, 2, 0x0DE0}, // DYNAMIC_SEQRAM_1E6
    {0x41E8, 2, 0x9F13}, // DYNAMIC_SEQRAM_1E8
    {0x41EA, 2, 0x0041}, // DYNAMIC_SEQRAM_1EA
    {0x41EC, 2, 0x80F3}, // DYNAMIC_SEQRAM_1EC
    {0x41EE, 2, 0xF213}, // DYNAMIC_SEQRAM_1EE
    {0x41F0, 2, 0x00F0}, // DYNAMIC_SEQRAM_1F0
    {0x41F2, 2, 0x13B8}, // DYNAMIC_SEQRAM_1F2
    {0x41F4, 2, 0xF04C}, // DYNAMIC_SEQRAM_1F4
    {0x41F6, 2, 0x9FF0}, // DYNAMIC_SEQRAM_1F6
    {0x41F8, 2, 0x00B7}, // DYNAMIC_SEQRAM_1F8
    {0x41FA, 2, 0xF006}, // DYNAMIC_SEQRAM_1FA
    {0x41FC, 2, 0x0035}, // DYNAMIC_SEQRAM_1FC
    {0x41FE, 2, 0x10AF}, // DYNAMIC_SEQRAM_1FE
    {0x4200, 2, 0x3003}, // DYNAMIC_SEQRAM_200
    {0x4202, 2, 0x30C0}, // DYNAMIC_SEQRAM_202
    {0x4204, 2, 0xB2F0}, // DYNAMIC_SEQRAM_204
    {0x4206, 2, 0x01B5}, // DYNAMIC_SEQRAM_206
    {0x4208, 2, 0xF001}, // DYNAMIC_SEQRAM_208
    {0x420A, 2, 0x85F0}, // DYNAMIC_SEQRAM_20A
    {0x420C, 2, 0x0292}, // DYNAMIC_SEQRAM_20C
    {0x420E, 2, 0xF000}, // DYNAMIC_SEQRAM_20E
    {0x4210, 2, 0x9A8B}, // DYNAMIC_SEQRAM_210
    {0x4212, 2, 0xF000}, // DYNAMIC_SEQRAM_212
    {0x4214, 2, 0x9997}, // DYNAMIC_SEQRAM_214
    {0x4216, 2, 0xF007}, // DYNAMIC_SEQRAM_216
    {0x4218, 2, 0xB6F0}, // DYNAMIC_SEQRAM_218
    {0x421A, 2, 0x0020}, // DYNAMIC_SEQRAM_21A
    {0x421C, 2, 0x5830}, // DYNAMIC_SEQRAM_21C
    {0x421E, 2, 0xC040}, // DYNAMIC_SEQRAM_21E
    {0x4220, 2, 0x1282}, // DYNAMIC_SEQRAM_220
    {0x4222, 2, 0xF005}, // DYNAMIC_SEQRAM_222
    {0x4224, 2, 0x9CF0}, // DYNAMIC_SEQRAM_224
    {0x4226, 2, 0x01B2}, // DYNAMIC_SEQRAM_226
    {0x4228, 2, 0xF008}, // DYNAMIC_SEQRAM_228
    {0x422A, 2, 0xB8F0}, // DYNAMIC_SEQRAM_22A
    {0x422C, 2, 0x0799}, // DYNAMIC_SEQRAM_22C
    {0x422E, 2, 0xF005}, // DYNAMIC_SEQRAM_22E
    {0x4230, 2, 0x98F0}, // DYNAMIC_SEQRAM_230
    {0x4232, 2, 0x0296}, // DYNAMIC_SEQRAM_232
    {0x4234, 2, 0xA2F0}, // DYNAMIC_SEQRAM_234
    {0x4236, 2, 0x00A2}, // DYNAMIC_SEQRAM_236
    {0x4238, 2, 0xF008}, // DYNAMIC_SEQRAM_238
    {0x423A, 2, 0x9DF0}, // DYNAMIC_SEQRAM_23A
    {0x423C, 2, 0x02A1}, // DYNAMIC_SEQRAM_23C
    {0x423E, 2, 0xF01F}, // DYNAMIC_SEQRAM_23E
    {0x4240, 2, 0x1009}, // DYNAMIC_SEQRAM_240
    {0x4242, 2, 0x2220}, // DYNAMIC_SEQRAM_242
    {0x4244, 2, 0x0808}, // DYNAMIC_SEQRAM_244
    {0x4246, 2, 0xF000}, // DYNAMIC_SEQRAM_246
    {0x4248, 2, 0x3600}, // DYNAMIC_SEQRAM_248
    {0x424A, 2, 0xF000}, // DYNAMIC_SEQRAM_24A
    {0x424C, 2, 0x88F0}, // DYNAMIC_SEQRAM_24C
    {0x424E, 2, 0x0788}, // DYNAMIC_SEQRAM_24E
    {0x4250, 2, 0x3600}, // DYNAMIC_SEQRAM_250
    {0x4252, 2, 0xF000}, // DYNAMIC_SEQRAM_252
    {0x4254, 2, 0x83F0}, // DYNAMIC_SEQRAM_254
    {0x4256, 2, 0x0290}, // DYNAMIC_SEQRAM_256
    {0x4258, 2, 0xF016}, // DYNAMIC_SEQRAM_258
    {0x425A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_25A
    {0x425C, 2, 0x11A3}, // DYNAMIC_SEQRAM_25C
    {0x425E, 2, 0xF000}, // DYNAMIC_SEQRAM_25E
    {0x4260, 2, 0xA3F0}, // DYNAMIC_SEQRAM_260
    {0x4262, 2, 0x089D}, // DYNAMIC_SEQRAM_262
    {0x4264, 2, 0xF002}, // DYNAMIC_SEQRAM_264
    {0x4266, 2, 0xA1F0}, // DYNAMIC_SEQRAM_266
    {0x4268, 2, 0x20A1}, // DYNAMIC_SEQRAM_268
    {0x426A, 2, 0xF006}, // DYNAMIC_SEQRAM_26A
    {0x426C, 2, 0x4300}, // DYNAMIC_SEQRAM_26C
    {0x426E, 2, 0xF049}, // DYNAMIC_SEQRAM_26E
    {0x4270, 2, 0x4014}, // DYNAMIC_SEQRAM_270
    {0x4272, 2, 0x8B8E}, // DYNAMIC_SEQRAM_272
    {0x4274, 2, 0x9DF0}, // DYNAMIC_SEQRAM_274
    {0x4276, 2, 0x0802}, // DYNAMIC_SEQRAM_276
    {0x4278, 2, 0x02F0}, // DYNAMIC_SEQRAM_278
    {0x427A, 2, 0x00A6}, // DYNAMIC_SEQRAM_27A
    {0x427C, 2, 0xF013}, // DYNAMIC_SEQRAM_27C
    {0x427E, 2, 0xB283}, // DYNAMIC_SEQRAM_27E
    {0x4280, 2, 0x9C36}, // DYNAMIC_SEQRAM_280
    {0x4282, 2, 0x00F0}, // DYNAMIC_SEQRAM_282
    {0x4284, 2, 0x0636}, // DYNAMIC_SEQRAM_284
    {0x4286, 2, 0x009C}, // DYNAMIC_SEQRAM_286
    {0x4288, 2, 0xF008}, // DYNAMIC_SEQRAM_288
    {0x428A, 2, 0x8BF0}, // DYNAMIC_SEQRAM_28A
    {0x428C, 2, 0x0083}, // DYNAMIC_SEQRAM_28C
    {0x428E, 2, 0xA0F0}, // DYNAMIC_SEQRAM_28E
    {0x4290, 2, 0x0630}, // DYNAMIC_SEQRAM_290
    {0x4292, 2, 0x18F0}, // DYNAMIC_SEQRAM_292
    {0x4294, 2, 0x02A3}, // DYNAMIC_SEQRAM_294
    {0x4296, 2, 0xF000}, // DYNAMIC_SEQRAM_296
    {0x4298, 2, 0xA3F0}, // DYNAMIC_SEQRAM_298
    {0x429A, 2, 0x0243}, // DYNAMIC_SEQRAM_29A
    {0x429C, 2, 0x00F0}, // DYNAMIC_SEQRAM_29C
    {0x429E, 2, 0x049D}, // DYNAMIC_SEQRAM_29E
    {0x42A0, 2, 0xF078}, // DYNAMIC_SEQRAM_2A0
    {0x42A2, 2, 0x3018}, // DYNAMIC_SEQRAM_2A2
    {0x42A4, 2, 0xF000}, // DYNAMIC_SEQRAM_2A4
    {0x42A6, 2, 0x9D82}, // DYNAMIC_SEQRAM_2A6
    {0x42A8, 2, 0xF000}, // DYNAMIC_SEQRAM_2A8
    {0x42AA, 2, 0x9030}, // DYNAMIC_SEQRAM_2AA
    {0x42AC, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2AC
    {0x42AE, 2, 0x1130}, // DYNAMIC_SEQRAM_2AE
    {0x42B0, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2B0
    {0x42B2, 2, 0x0082}, // DYNAMIC_SEQRAM_2B2
    {0x42B4, 2, 0xF001}, // DYNAMIC_SEQRAM_2B4
    {0x42B6, 2, 0x1009}, // DYNAMIC_SEQRAM_2B6
    {0x42B8, 2, 0xF02A}, // DYNAMIC_SEQRAM_2B8
    {0x42BA, 2, 0xA2F0}, // DYNAMIC_SEQRAM_2BA
    {0x42BC, 2, 0x00A2}, // DYNAMIC_SEQRAM_2BC
    {0x42BE, 2, 0x3018}, // DYNAMIC_SEQRAM_2BE
    {0x42C0, 2, 0xF007}, // DYNAMIC_SEQRAM_2C0
    {0x42C2, 2, 0x9DF0}, // DYNAMIC_SEQRAM_2C2
    {0x42C4, 2, 0x1C8C}, // DYNAMIC_SEQRAM_2C4
    {0x42C6, 2, 0xF005}, // DYNAMIC_SEQRAM_2C6
    {0x42C8, 2, 0x301F}, // DYNAMIC_SEQRAM_2C8
    {0x42CA, 2, 0x216D}, // DYNAMIC_SEQRAM_2CA
    {0x42CC, 2, 0x0A51}, // DYNAMIC_SEQRAM_2CC
    {0x42CE, 2, 0x1FEA}, // DYNAMIC_SEQRAM_2CE
    {0x42D0, 2, 0x8640}, // DYNAMIC_SEQRAM_2D0
    {0x42D2, 2, 0xE29F}, // DYNAMIC_SEQRAM_2D2
    {0x42D4, 2, 0xF009}, // DYNAMIC_SEQRAM_2D4
    {0x42D6, 2, 0x0005}, // DYNAMIC_SEQRAM_2D6
    {0x42D8, 2, 0xF000}, // DYNAMIC_SEQRAM_2D8
    {0x42DA, 2, 0x30C0}, // DYNAMIC_SEQRAM_2DA
    {0x42DC, 2, 0xF001}, // DYNAMIC_SEQRAM_2DC
    {0x42DE, 2, 0x83F0}, // DYNAMIC_SEQRAM_2DE
    {0x42E0, 2, 0x0036}, // DYNAMIC_SEQRAM_2E0
    {0x42E2, 2, 0x00F0}, // DYNAMIC_SEQRAM_2E2
    {0x42E4, 2, 0x0087}, // DYNAMIC_SEQRAM_2E4
    {0x42E6, 2, 0xF007}, // DYNAMIC_SEQRAM_2E6
    {0x42E8, 2, 0x87F0}, // DYNAMIC_SEQRAM_2E8
    {0x42EA, 2, 0x0036}, // DYNAMIC_SEQRAM_2EA
    {0x42EC, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2EC
    {0x42EE, 2, 0x0000}, // DYNAMIC_SEQRAM_2EE
    {0x42F0, 2, 0x0DF0}, // DYNAMIC_SEQRAM_2F0
    {0x42F2, 2, 0x0000}, // DYNAMIC_SEQRAM_2F2
    {0x42F4, 2, 0x05F0}, // DYNAMIC_SEQRAM_2F4
    {0x42F6, 2, 0x0030}, // DYNAMIC_SEQRAM_2F6
    {0x42F8, 2, 0xC0F0}, // DYNAMIC_SEQRAM_2F8
    {0x42FA, 2, 0x0183}, // DYNAMIC_SEQRAM_2FA
    {0x42FC, 2, 0xF000}, // DYNAMIC_SEQRAM_2FC
    {0x42FE, 2, 0x3600}, // DYNAMIC_SEQRAM_2FE
    {0x4300, 2, 0xF000}, // DYNAMIC_SEQRAM_300
    {0x4302, 2, 0x87F0}, // DYNAMIC_SEQRAM_302
    {0x4304, 2, 0x0787}, // DYNAMIC_SEQRAM_304
    {0x4306, 2, 0xF000}, // DYNAMIC_SEQRAM_306
    {0x4308, 2, 0x36C0}, // DYNAMIC_SEQRAM_308
    {0x430A, 2, 0xF000}, // DYNAMIC_SEQRAM_30A
    {0x430C, 2, 0x000F}, // DYNAMIC_SEQRAM_30C
    {0x430E, 2, 0xF42A}, // DYNAMIC_SEQRAM_30E
    {0x4310, 2, 0x4180}, // DYNAMIC_SEQRAM_310
    {0x4312, 2, 0x1300}, // DYNAMIC_SEQRAM_312
    {0x4314, 2, 0x9FF0}, // DYNAMIC_SEQRAM_314
    {0x4316, 2, 0x00E0}, // DYNAMIC_SEQRAM_316
    {0x4318, 2, 0xE0E0}, // DYNAMIC_SEQRAM_318
    {0x431A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31A
    {0x431C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31C
    {0x431E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_31E
    {0x4320, 2, 0xE0E0}, // DYNAMIC_SEQRAM_320
    {0x4322, 2, 0xE0E0}, // DYNAMIC_SEQRAM_322
    {0x4324, 2, 0xE0E0}, // DYNAMIC_SEQRAM_324
    {0x4326, 2, 0xE0E0}, // DYNAMIC_SEQRAM_326
    {0x4328, 2, 0xE0E0}, // DYNAMIC_SEQRAM_328
    {0x432A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32A
    {0x432C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32C
    {0x432E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_32E
    {0x4330, 2, 0xE0E0}, // DYNAMIC_SEQRAM_330
    {0x4332, 2, 0xE0E0}, // DYNAMIC_SEQRAM_332
    {0x4334, 2, 0xE0E0}, // DYNAMIC_SEQRAM_334
    {0x4336, 2, 0xE0E0}, // DYNAMIC_SEQRAM_336
    {0x4338, 2, 0xE0E0}, // DYNAMIC_SEQRAM_338
    {0x433A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33A
    {0x433C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33C
    {0x433E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_33E
    {0x4340, 2, 0xE0E0}, // DYNAMIC_SEQRAM_340
    {0x4342, 2, 0xE0E0}, // DYNAMIC_SEQRAM_342
    {0x4344, 2, 0xE0E0}, // DYNAMIC_SEQRAM_344
    {0x4346, 2, 0xE0E0}, // DYNAMIC_SEQRAM_346
    {0x4348, 2, 0xE0E0}, // DYNAMIC_SEQRAM_348
    {0x434A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34A
    {0x434C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34C
    {0x434E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_34E
    {0x4350, 2, 0xE0E0}, // DYNAMIC_SEQRAM_350
    {0x4352, 2, 0xE0E0}, // DYNAMIC_SEQRAM_352
    {0x4354, 2, 0xE0E0}, // DYNAMIC_SEQRAM_354
    {0x4356, 2, 0xE0E0}, // DYNAMIC_SEQRAM_356
    {0x4358, 2, 0xE0E0}, // DYNAMIC_SEQRAM_358
    {0x435A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35A
    {0x435C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35C
    {0x435E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_35E
    {0x4360, 2, 0xE0E0}, // DYNAMIC_SEQRAM_360
    {0x4362, 2, 0xE0E0}, // DYNAMIC_SEQRAM_362
    {0x4364, 2, 0xE0E0}, // DYNAMIC_SEQRAM_364
    {0x4366, 2, 0xE0E0}, // DYNAMIC_SEQRAM_366
    {0x4368, 2, 0xE0E0}, // DYNAMIC_SEQRAM_368
    {0x436A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36A
    {0x436C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36C
    {0x436E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_36E
    {0x4370, 2, 0xE0E0}, // DYNAMIC_SEQRAM_370
    {0x4372, 2, 0xE0E0}, // DYNAMIC_SEQRAM_372
    {0x4374, 2, 0xE0E0}, // DYNAMIC_SEQRAM_374
    {0x4376, 2, 0xE0E0}, // DYNAMIC_SEQRAM_376
    {0x4378, 2, 0xE0E0}, // DYNAMIC_SEQRAM_378
    {0x437A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37A
    {0x437C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37C
    {0x437E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_37E
    {0x4380, 2, 0xE0E0}, // DYNAMIC_SEQRAM_380
    {0x4382, 2, 0xE0E0}, // DYNAMIC_SEQRAM_382
    {0x4384, 2, 0xE0E0}, // DYNAMIC_SEQRAM_384
    {0x4386, 2, 0xE0E0}, // DYNAMIC_SEQRAM_386
    {0x4388, 2, 0xE0E0}, // DYNAMIC_SEQRAM_388
    {0x438A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38A
    {0x438C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38C
    {0x438E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_38E
    {0x4390, 2, 0xE0E0}, // DYNAMIC_SEQRAM_390
    {0x4392, 2, 0xE0E0}, // DYNAMIC_SEQRAM_392
    {0x4394, 2, 0xE0E0}, // DYNAMIC_SEQRAM_394
    {0x4396, 2, 0xE0E0}, // DYNAMIC_SEQRAM_396
    {0x4398, 2, 0xE0E0}, // DYNAMIC_SEQRAM_398
    {0x439A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39A
    {0x439C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39C
    {0x439E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_39E
    {0x43A0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A0
    {0x43A2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A2
    {0x43A4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A4
    {0x43A6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A6
    {0x43A8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3A8
    {0x43AA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AA
    {0x43AC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AC
    {0x43AE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3AE
    {0x43B0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B0
    {0x43B2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B2
    {0x43B4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B4
    {0x43B6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B6
    {0x43B8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3B8
    {0x43BA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BA
    {0x43BC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BC
    {0x43BE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3BE
    {0x43C0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C0
    {0x43C2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C2
    {0x43C4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C4
    {0x43C6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C6
    {0x43C8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3C8
    {0x43CA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CA
    {0x43CC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CC
    {0x43CE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3CE
    {0x43D0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D0
    {0x43D2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D2
    {0x43D4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D4
    {0x43D6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D6
    {0x43D8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3D8
    {0x43DA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DA
    {0x43DC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DC
    {0x43DE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3DE
    {0x43E0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E0
    {0x43E2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E2
    {0x43E4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E4
    {0x43E6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E6
    {0x43E8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3E8
    {0x43EA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EA
    {0x43EC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EC
    {0x43EE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3EE
    {0x43F0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F0
    {0x43F2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F2
    {0x43F4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F4
    {0x43F6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F6
    {0x43F8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3F8
    {0x43FA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FA
    {0x43FC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FC
    {0x43FE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_3FE
    {0x4400, 2, 0xE0E0}, // DYNAMIC_SEQRAM_400
    {0x4402, 2, 0xE0E0}, // DYNAMIC_SEQRAM_402
    {0x4404, 2, 0xE0E0}, // DYNAMIC_SEQRAM_404
    {0x4406, 2, 0xE0E0}, // DYNAMIC_SEQRAM_406
    {0x4408, 2, 0xE0E0}, // DYNAMIC_SEQRAM_408
    {0x440A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40A
    {0x440C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40C
    {0x440E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_40E
    {0x4410, 2, 0xE0E0}, // DYNAMIC_SEQRAM_410
    {0x4412, 2, 0xE0E0}, // DYNAMIC_SEQRAM_412
    {0x4414, 2, 0xE0E0}, // DYNAMIC_SEQRAM_414
    {0x4416, 2, 0xE0E0}, // DYNAMIC_SEQRAM_416
    {0x4418, 2, 0xE0E0}, // DYNAMIC_SEQRAM_418
    {0x441A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41A
    {0x441C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41C
    {0x441E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_41E
    {0x4420, 2, 0xE0E0}, // DYNAMIC_SEQRAM_420
    {0x4422, 2, 0xE0E0}, // DYNAMIC_SEQRAM_422
    {0x4424, 2, 0xE0E0}, // DYNAMIC_SEQRAM_424
    {0x4426, 2, 0xE0E0}, // DYNAMIC_SEQRAM_426
    {0x4428, 2, 0xE0E0}, // DYNAMIC_SEQRAM_428
    {0x442A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42A
    {0x442C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42C
    {0x442E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_42E
    {0x4430, 2, 0xE0E0}, // DYNAMIC_SEQRAM_430
    {0x4432, 2, 0xE0E0}, // DYNAMIC_SEQRAM_432
    {0x4434, 2, 0xE0E0}, // DYNAMIC_SEQRAM_434
    {0x4436, 2, 0xE0E0}, // DYNAMIC_SEQRAM_436
    {0x4438, 2, 0xE0E0}, // DYNAMIC_SEQRAM_438
    {0x443A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43A
    {0x443C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43C
    {0x443E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_43E
    {0x4440, 2, 0xE0E0}, // DYNAMIC_SEQRAM_440
    {0x4442, 2, 0xE0E0}, // DYNAMIC_SEQRAM_442
    {0x4444, 2, 0xE0E0}, // DYNAMIC_SEQRAM_444
    {0x4446, 2, 0xE0E0}, // DYNAMIC_SEQRAM_446
    {0x4448, 2, 0xE0E0}, // DYNAMIC_SEQRAM_448
    {0x444A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44A
    {0x444C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44C
    {0x444E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_44E
    {0x4450, 2, 0xE0E0}, // DYNAMIC_SEQRAM_450
    {0x4452, 2, 0xE0E0}, // DYNAMIC_SEQRAM_452
    {0x4454, 2, 0xE0E0}, // DYNAMIC_SEQRAM_454
    {0x4456, 2, 0xE0E0}, // DYNAMIC_SEQRAM_456
    {0x4458, 2, 0xE0E0}, // DYNAMIC_SEQRAM_458
    {0x445A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45A
    {0x445C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45C
    {0x445E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_45E
    {0x4460, 2, 0xE0E0}, // DYNAMIC_SEQRAM_460
    {0x4462, 2, 0xE0E0}, // DYNAMIC_SEQRAM_462
    {0x4464, 2, 0xE0E0}, // DYNAMIC_SEQRAM_464
    {0x4466, 2, 0xE0E0}, // DYNAMIC_SEQRAM_466
    {0x4468, 2, 0xE0E0}, // DYNAMIC_SEQRAM_468
    {0x446A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46A
    {0x446C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46C
    {0x446E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_46E
    {0x4470, 2, 0xE0E0}, // DYNAMIC_SEQRAM_470
    {0x4472, 2, 0xE0E0}, // DYNAMIC_SEQRAM_472
    {0x4474, 2, 0xE0E0}, // DYNAMIC_SEQRAM_474
    {0x4476, 2, 0xE0E0}, // DYNAMIC_SEQRAM_476
    {0x4478, 2, 0xE0E0}, // DYNAMIC_SEQRAM_478
    {0x447A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47A
    {0x447C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47C
    {0x447E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_47E
    {0x4480, 2, 0xE0E0}, // DYNAMIC_SEQRAM_480
    {0x4482, 2, 0xE0E0}, // DYNAMIC_SEQRAM_482
    {0x4484, 2, 0xE0E0}, // DYNAMIC_SEQRAM_484
    {0x4486, 2, 0xE0E0}, // DYNAMIC_SEQRAM_486
    {0x4488, 2, 0xE0E0}, // DYNAMIC_SEQRAM_488
    {0x448A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48A
    {0x448C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48C
    {0x448E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_48E
    {0x4490, 2, 0xE0E0}, // DYNAMIC_SEQRAM_490
    {0x4492, 2, 0xE0E0}, // DYNAMIC_SEQRAM_492
    {0x4494, 2, 0xE0E0}, // DYNAMIC_SEQRAM_494
    {0x4496, 2, 0xE0E0}, // DYNAMIC_SEQRAM_496
    {0x4498, 2, 0xE0E0}, // DYNAMIC_SEQRAM_498
    {0x449A, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49A
    {0x449C, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49C
    {0x449E, 2, 0xE0E0}, // DYNAMIC_SEQRAM_49E
    {0x44A0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A0
    {0x44A2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A2
    {0x44A4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A4
    {0x44A6, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A6
    {0x44A8, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4A8
    {0x44AA, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AA
    {0x44AC, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AC
    {0x44AE, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4AE
    {0x44B0, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B0
    {0x44B2, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B2
    {0x44B4, 2, 0xE0E0}, // DYNAMIC_SEQRAM_4B4
    {0x5500, 2, 0x0000}, // AGAIN_LUT0
    {0x5502, 2, 0x0002}, // AGAIN_LUT1
    {0x5504, 2, 0x0006}, // AGAIN_LUT2
    {0x5506, 2, 0x0009}, // AGAIN_LUT3
    {0x5508, 2, 0x000F}, // AGAIN_LUT4
    {0x550A, 2, 0x0010}, // AGAIN_LUT5
    {0x550C, 2, 0x0011}, // AGAIN_LUT6
    {0x550E, 2, 0x0012}, // AGAIN_LUT7
    {0x5510, 2, 0x0019}, // AGAIN_LUT8
    {0x5512, 2, 0x0020}, // AGAIN_LUT9
    {0x5514, 2, 0x0021}, // AGAIN_LUT10
    {0x5516, 2, 0x0023}, // AGAIN_LUT11
    {0x5518, 2, 0x0026}, // AGAIN_LUT12
    {0x551A, 2, 0x002B}, // AGAIN_LUT13
    {0x551C, 2, 0x002F}, // AGAIN_LUT14
    {0x551E, 2, 0x0030}, // AGAIN_LUT15
    {0x5400, 2, 0x0100}, // GT1_COARSE0
    {0x5402, 2, 0x2106}, // GT1_COARSE1
    {0x5404, 2, 0x1101}, // GT1_COARSE2
    {0x5406, 2, 0x3106}, // GT1_COARSE3
    {0x5408, 2, 0x7100}, // GT1_COARSE4
    {0x540A, 2, 0x8107}, // GT1_COARSE5
    {0x540C, 2, 0xB101}, // GT1_COARSE6
    {0x540E, 2, 0xD101}, // GT1_COARSE7
    {0x5410, 2, 0xF12E}, // GT1_COARSE8
    {0x5412, 2, 0xF112}, // GT1_COARSE9
    {0x5414, 2, 0xF184}, // GT1_COARSE10
    {0x5416, 2, 0xF224}, // GT1_COARSE11
    {0x5418, 2, 0xF306}, // GT1_COARSE12
    {0x541A, 2, 0xF446}, // GT1_COARSE13
    {0x541C, 2, 0xF609}, // GT1_COARSE14
    {0x541E, 2, 0xF887}, // GT1_COARSE15
    {0x5420, 2, 0xFC0B}, // GT1_COARSE16
    {0x5422, 2, 0xFC0B}, // GT1_COARSE17
    {0x5424, 2, 0xFFFA}, // GT1_DCG_ATTN_SET0
    {0x5426, 2, 0x5557}, // GT1_DCG_ATTN_SET1
    {0x5428, 2, 0x0005}, // GT1_DCG_ATTN_SET2
    {0x542A, 2, 0xA550}, // GT1_ZONE_SET0
    {0x542C, 2, 0xAAAA}, // GT1_ZONE_SET1
    {0x542E, 2, 0x000A}, // GT1_ZONE_SET2
    {0x5460, 2, 0x2269}, // ZT1_REG0_ADDR
    {0x5462, 2, 0x0B87}, // ZT1_REG0_VALUE0
    {0x5464, 2, 0x0B87}, // ZT1_REG0_VALUE1
    {0x5466, 2, 0x0983}, // ZT1_REG0_VALUE2
    {0x5498, 2, 0x225E}, // ZT1_REG7_ADDR
    {0x549A, 2, 0xBCAA}, // ZT1_REG7_VALUE0
    {0x549C, 2, 0xBCAA}, // ZT1_REG7_VALUE1
    {0x549E, 2, 0xBDAA}, // ZT1_REG7_VALUE2
    {0x3060, 2, 0xFF01}, // GAIN_TABLE_CTRL
    {0x44BA, 2, 0x0048}, // DAC_LD_4_5
    {0x44BC, 2, 0xBCAA}, // DAC_LD_6_7
    {0x44C0, 2, 0x4070}, // DAC_LD_10_11
    {0x44C4, 2, 0x04D0}, // DAC_LD_14_15
    {0x44C6, 2, 0x17E2}, // DAC_LD_16_17
    {0x44C8, 2, 0xEA43}, // DAC_LD_18_19
    {0x44CA, 2, 0x000E}, // DAC_LD_20_21
    {0x44CC, 2, 0x7777}, // DAC_LD_22_23
    {0x44CE, 2, 0x8BA4}, // DAC_LD_24_25
    {0x44D0, 2, 0x1735}, // DAC_LD_26_27
    {0x44D2, 2, 0x0B87}, // DAC_LD_28_29
    {0x44D4, 2, 0x8000}, // DAC_LD_30_31
    {0x44D6, 2, 0xF206}, // DAC_LD_32_33
    {0x44D8, 2, 0xAAFA}, // DAC_LD_34_35
    {0x44DA, 2, 0xE001}, // DAC_LD_36_37
    {0x44DE, 2, 0x9BBC}, // DAC_LD_40_41
    {0x44E0, 2, 0x283C}, // DAC_LD_42_43
    {0x44E2, 2, 0x2821}, // DAC_LD_44_45
    {0x44E4, 2, 0x8000}, // DAC_LD_46_47
    {0x44E6, 2, 0x503F}, // DAC_LD_48_49
    {0x32A4, 2, 0x0000}, // CRM_CTRL
    {0x333C, 2, 0x0001}, // DYNAMIC_CTRL
    {0x0300, 2, 0x0006}, // VT_PIX_CLK_DIV
    {0x0304, 2, 0x0002}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 99}, //{0x0306, 2, 0x0067}, // VT_PLL_MULTIPLIER
    {0x3980, 2, 0x0001}, // PIX_DEF_CORR
    {0x3700, 2, 0x0001}, // PIX_DEF_ID
    {0x3340, 2, 0x1C60}, // OTPM_CTRL
    {0x3340, 2, 0x1C60}, // OTPM_CTRL
    {0x36C0, 2, 0x0001}, // DIGITAL_GAIN_CTRL
    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE
    {0x0100, 1, 0x01}, // MODE_SELECT
    {0x44D6, 2, 0xB206}, // DAC_LD_32_33
};

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_1920x2700_30_table[] =
{
    {0x0103, 0x3201, 0x01},

    {0x0304, 2, 2}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 88}, // VT_PLL_MULTIPLIER
    {0x0300, 2, 6}, // VT_PIX_CLK_DIV
    {0x0302, 2, 1}, // VT_SYS_CLK_DIV
    {0x030C, 2, 7}, // OP_PRE_PLL_CLK_DIV
    {0x030E, 2, 320}, // OP_PLL_MULTIPLIER
    {0x0308, 2, 10}, // OP_PIX_CLK_DIV
    {0x030A, 2, 1}, // OP_SYS_CLK_DIV
    {0x0344, 2, 1608}, // X_ADDR_START
    {0x0348, 2, 3527}, // X_ADDR_END
    {0x0346, 2, 578}, // Y_ADDR_START
    {0x034A, 2, 3277}, // Y_ADDR_END
    {0x034C, 2, 1920}, // X_OUTPUT_SIZE
    {0x034E, 2, 2700}, // Y_OUTPUT_SIZE
    {0x0380, 2, 1}, // X_EVEN_INC
    {0x0382, 2, 1}, // X_ODD_INC
    {0x0384, 2, 1}, // Y_EVEN_INC
    {0x0386, 2, 1}, // Y_ODD_INC
    {0x0900, 1, 0}, // BINNING_MODE
    {0x0901, 1, 17}, // BINNING_TYPE
    {0x0342, 2, 14000}, //{0x0342, 2, 0x3800}, //{0x0342, 2, 11760}, // LINE_LENGTH_PCK
    {0x0340, 2, 3200}, //{0x0340, 2, 0xf1e}, //{0x0340, 2, 3870}, //{0x0340, 2, 3870}, // FRAME_LENGTH_LINES
    {0x0202, 2, 1000}, //{0x0202, 2, 4671}, //{0x0202, 2, 4671}, // COARSE_INTEGRATION_TIME
    {0x0112, 2, 2570}, // CSI_DATA_FORMAT
    {0x0114, 1, 3}, // CSI_LANE_MODE
    {0x0800, 1, 12}, // TCLK_POST
    {0x0801, 1, 6}, // THS_PREPARE
    {0x0802, 1, 11}, // THS_ZERO_MIN
    {0x0803, 1, 8}, // THS_TRAIL
    {0x0804, 1, 10}, // TCLK_TRAIL_MIN
    {0x0805, 1, 7}, // TCLK_PREPARE
    {0x0806, 1, 33}, // TCLK_ZERO
    {0x0807, 1, 7}, // TLPX
    {0x082A, 1, 14}, // TWAKEUP
    {0x082B, 1, 11}, // TINIT
    {0x082C, 1, 12}, // THS_EXIT
    {0x3F06, 2, 192}, // MIPI_TIMING_2
    {0x3F0A, 2, 0}, //{0x3F0A, 2, 32768}, // MIPI_TIMING_4
    {0x3F0C, 2, 9}, // MIPI_TIMING_5
    {0x3F20, 2, 2056}, // MIPI_PHY_TRIM_MSB
    {0x3F02, 2, 2}, // PHY_CTRL

    {0x4000, 2, 276}, // DYNAMIC_SEQRAM_00
    {0x4002, 2, 6693}, // DYNAMIC_SEQRAM_02
    {0x4004, 2, 15871}, // DYNAMIC_SEQRAM_04
    {0x4006, 2, 65535}, // DYNAMIC_SEQRAM_06
    {0x4008, 2, 2613}, // DYNAMIC_SEQRAM_08
    {0x400A, 2, 4239}, // DYNAMIC_SEQRAM_0A
    {0x400C, 2, 12291}, // DYNAMIC_SEQRAM_0C
    {0x400E, 2, 12504}, // DYNAMIC_SEQRAM_0E
    {0x4010, 2, 61442}, // DYNAMIC_SEQRAM_10
    {0x4012, 2, 37557}, // DYNAMIC_SEQRAM_12
    {0x4014, 2, 61440}, // DYNAMIC_SEQRAM_14
    {0x4016, 2, 34288}, // DYNAMIC_SEQRAM_16
    {0x4018, 2, 145}, // DYNAMIC_SEQRAM_18
    {0x401A, 2, 61442}, // DYNAMIC_SEQRAM_1A
    {0x401C, 2, 39561}, // DYNAMIC_SEQRAM_1C
    {0x401E, 2, 61440}, // DYNAMIC_SEQRAM_1E
    {0x4020, 2, 39319}, // DYNAMIC_SEQRAM_20
    {0x4022, 2, 61440}, // DYNAMIC_SEQRAM_22
    {0x4024, 2, 12480}, // DYNAMIC_SEQRAM_24
    {0x4026, 2, 61440}, // DYNAMIC_SEQRAM_26
    {0x4028, 2, 33520}, // DYNAMIC_SEQRAM_28
    {0x402A, 2, 48}, // DYNAMIC_SEQRAM_2A
    {0x402C, 2, 6384}, // DYNAMIC_SEQRAM_2C
    {0x402E, 2, 800}, // DYNAMIC_SEQRAM_2E
    {0x4030, 2, 22768}, // DYNAMIC_SEQRAM_30
    {0x4032, 2, 2204}, // DYNAMIC_SEQRAM_32
    {0x4034, 2, 61457}, // DYNAMIC_SEQRAM_34
    {0x4036, 2, 46832}, // DYNAMIC_SEQRAM_36
    {0x4038, 2, 153}, // DYNAMIC_SEQRAM_38
    {0x403A, 2, 61441}, // DYNAMIC_SEQRAM_3A
    {0x403C, 2, 46232}, // DYNAMIC_SEQRAM_3C
    {0x403E, 2, 41110}, // DYNAMIC_SEQRAM_3E
    {0x4040, 2, 61440}, // DYNAMIC_SEQRAM_40
    {0x4042, 2, 41712}, // DYNAMIC_SEQRAM_42
    {0x4044, 2, 162}, // DYNAMIC_SEQRAM_44
    {0x4046, 2, 61448}, // DYNAMIC_SEQRAM_46
    {0x4048, 2, 40432}, // DYNAMIC_SEQRAM_48
    {0x404A, 2, 8349}, // DYNAMIC_SEQRAM_4A
    {0x404C, 2, 35848}, // DYNAMIC_SEQRAM_4C
    {0x404E, 2, 2288}, // DYNAMIC_SEQRAM_4E
    {0x4050, 2, 54}, // DYNAMIC_SEQRAM_50
    {0x4052, 2, 143}, // DYNAMIC_SEQRAM_52
    {0x4054, 2, 61440}, // DYNAMIC_SEQRAM_54
    {0x4056, 2, 35056}, // DYNAMIC_SEQRAM_56
    {0x4058, 2, 1160}, // DYNAMIC_SEQRAM_58
    {0x405A, 2, 61440}, // DYNAMIC_SEQRAM_5A
    {0x405C, 2, 13824}, // DYNAMIC_SEQRAM_5C
    {0x405E, 2, 61440}, // DYNAMIC_SEQRAM_5E
    {0x4060, 2, 33776}, // DYNAMIC_SEQRAM_60
    {0x4062, 2, 656}, // DYNAMIC_SEQRAM_62
    {0x4064, 2, 61440}, // DYNAMIC_SEQRAM_64
    {0x4066, 2, 35824}, // DYNAMIC_SEQRAM_66
    {0x4068, 2, 11939}, // DYNAMIC_SEQRAM_68
    {0x406A, 2, 61440}, // DYNAMIC_SEQRAM_6A
    {0x406C, 2, 41968}, // DYNAMIC_SEQRAM_6C
    {0x406E, 2, 2205}, // DYNAMIC_SEQRAM_6E
    {0x4070, 2, 61557}, // DYNAMIC_SEQRAM_70
    {0x4072, 2, 12291}, // DYNAMIC_SEQRAM_72
    {0x4074, 2, 16496}, // DYNAMIC_SEQRAM_74
    {0x4076, 2, 8557}, // DYNAMIC_SEQRAM_76
    {0x4078, 2, 7414}, // DYNAMIC_SEQRAM_78
    {0x407A, 2, 35584}, // DYNAMIC_SEQRAM_7A
    {0x407C, 2, 20870}, // DYNAMIC_SEQRAM_7C
    {0x407E, 2, 4864}, // DYNAMIC_SEQRAM_7E
    {0x4080, 2, 517}, // DYNAMIC_SEQRAM_80
    {0x4082, 2, 14040}, // DYNAMIC_SEQRAM_82
    {0x4084, 2, 61442}, // DYNAMIC_SEQRAM_84
    {0x4086, 2, 33671}, // DYNAMIC_SEQRAM_86
    {0x4088, 2, 61446}, // DYNAMIC_SEQRAM_88
    {0x408A, 2, 34562}, // DYNAMIC_SEQRAM_8A
    {0x408C, 2, 3330}, // DYNAMIC_SEQRAM_8C
    {0x408E, 2, 1520}, // DYNAMIC_SEQRAM_8E
    {0x4090, 2, 899}, // DYNAMIC_SEQRAM_90
    {0x4092, 2, 61441}, // DYNAMIC_SEQRAM_92
    {0x4094, 2, 34800}, // DYNAMIC_SEQRAM_94
    {0x4096, 2, 531}, // DYNAMIC_SEQRAM_96
    {0x4098, 2, 54}, // DYNAMIC_SEQRAM_98
    {0x409A, 2, 55431}, // DYNAMIC_SEQRAM_9A
    {0x409C, 2, 525}, // DYNAMIC_SEQRAM_9C
    {0x409E, 2, 57568}, // DYNAMIC_SEQRAM_9E
    {0x40A0, 2, 61440}, // DYNAMIC_SEQRAM_A0
    {0x40A2, 2, 1025}, // DYNAMIC_SEQRAM_A2
    {0x40A4, 2, 61448}, // DYNAMIC_SEQRAM_A4
    {0x40A6, 2, 33520}, // DYNAMIC_SEQRAM_A6
    {0x40A8, 2, 2179}, // DYNAMIC_SEQRAM_A8
    {0x40AA, 2, 61449}, // DYNAMIC_SEQRAM_AA
    {0x40AC, 2, 34288}, // DYNAMIC_SEQRAM_AC
    {0x40AE, 2, 10629}, // DYNAMIC_SEQRAM_AE
    {0x40B0, 2, 34800}, // DYNAMIC_SEQRAM_B0
    {0x40B2, 2, 10887}, // DYNAMIC_SEQRAM_B2
    {0x40B4, 2, 63038}, // DYNAMIC_SEQRAM_B4
    {0x40B6, 2, 35056}, // DYNAMIC_SEQRAM_B6
    {0x40B8, 2, 2049}, // DYNAMIC_SEQRAM_B8
    {0x40BA, 2, 16624}, // DYNAMIC_SEQRAM_BA
    {0x40BC, 2, 2048}, // DYNAMIC_SEQRAM_BC
    {0x40BE, 2, 18672}, // DYNAMIC_SEQRAM_BE
    {0x40C0, 2, 2178}, // DYNAMIC_SEQRAM_C0
    {0x40C2, 2, 61448}, // DYNAMIC_SEQRAM_C2
    {0x40C4, 2, 1025}, // DYNAMIC_SEQRAM_C4
    {0x40C6, 2, 61448}, // DYNAMIC_SEQRAM_C6
    {0x40C8, 2, 57568}, // DYNAMIC_SEQRAM_C8
    {0x40CA, 2, 57568}, // DYNAMIC_SEQRAM_CA
    {0x40CC, 2, 57568}, // DYNAMIC_SEQRAM_CC
    {0x40CE, 2, 57568}, // DYNAMIC_SEQRAM_CE
    {0x40D0, 2, 61440}, // DYNAMIC_SEQRAM_D0
    {0x40D2, 2, 1025}, // DYNAMIC_SEQRAM_D2
    {0x40D4, 2, 61461}, // DYNAMIC_SEQRAM_D4
    {0x40D6, 2, 60}, // DYNAMIC_SEQRAM_D6
    {0x40D8, 2, 61454}, // DYNAMIC_SEQRAM_D8
    {0x40DA, 2, 34288}, // DYNAMIC_SEQRAM_DA
    {0x40DC, 2, 1671}, // DYNAMIC_SEQRAM_DC
    {0x40DE, 2, 61442}, // DYNAMIC_SEQRAM_DE
    {0x40E0, 2, 34800}, // DYNAMIC_SEQRAM_E0
    {0x40E2, 2, 25064}, // DYNAMIC_SEQRAM_E2
    {0x40E4, 2, 14596}, // DYNAMIC_SEQRAM_E4
    {0x40E6, 2, 61445}, // DYNAMIC_SEQRAM_E6
    {0x40E8, 2, 13442}, // DYNAMIC_SEQRAM_E8
    {0x40EA, 2, 61440}, // DYNAMIC_SEQRAM_EA
    {0x40EC, 2, 12865}, // DYNAMIC_SEQRAM_EC
    {0x40EE, 2, 61440}, // DYNAMIC_SEQRAM_EE
    {0x40F0, 2, 14596}, // DYNAMIC_SEQRAM_F0
    {0x40F2, 2, 61454}, // DYNAMIC_SEQRAM_F2
    {0x40F4, 2, 14596}, // DYNAMIC_SEQRAM_F4
    {0x40F6, 2, 61440}, // DYNAMIC_SEQRAM_F6
    {0x40F8, 2, 12865}, // DYNAMIC_SEQRAM_F8
    {0x40FA, 2, 61440}, // DYNAMIC_SEQRAM_FA
    {0x40FC, 2, 13442}, // DYNAMIC_SEQRAM_FC
    {0x40FE, 2, 61445}, // DYNAMIC_SEQRAM_FE
    {0x4100, 2, 49382}, // DYNAMIC_SEQRAM_100
    {0x4102, 2, 61444}, // DYNAMIC_SEQRAM_102
    {0x4104, 2, 14596}, // DYNAMIC_SEQRAM_104
    {0x4106, 2, 61443}, // DYNAMIC_SEQRAM_106
    {0x4108, 2, 45296}, // DYNAMIC_SEQRAM_108
    {0x410A, 2, 0}, // DYNAMIC_SEQRAM_10A
    {0x410C, 2, 6384}, // DYNAMIC_SEQRAM_10C
    {0x410E, 2, 134}, // DYNAMIC_SEQRAM_10E
    {0x4110, 2, 61440}, // DYNAMIC_SEQRAM_110
    {0x4112, 2, 34544}, // DYNAMIC_SEQRAM_112
    {0x4114, 2, 35248}, // DYNAMIC_SEQRAM_114
    {0x4116, 2, 61440}, // DYNAMIC_SEQRAM_116
    {0x4118, 2, 59888}, // DYNAMIC_SEQRAM_118
    {0x411A, 2, 138}, // DYNAMIC_SEQRAM_11A
    {0x411C, 2, 61440}, // DYNAMIC_SEQRAM_11C
    {0x411E, 2, 5}, // DYNAMIC_SEQRAM_11E
    {0x4120, 2, 61440}, // DYNAMIC_SEQRAM_120
    {0x4122, 2, 57568}, // DYNAMIC_SEQRAM_122
    {0x4124, 2, 57568}, // DYNAMIC_SEQRAM_124
    {0x4126, 2, 57568}, // DYNAMIC_SEQRAM_126
    {0x4128, 2, 2613}, // DYNAMIC_SEQRAM_128
    {0x412A, 2, 4239}, // DYNAMIC_SEQRAM_12A
    {0x412C, 2, 12291}, // DYNAMIC_SEQRAM_12C
    {0x412E, 2, 12504}, // DYNAMIC_SEQRAM_12E
    {0x4130, 2, 61445}, // DYNAMIC_SEQRAM_130
    {0x4132, 2, 34194}, // DYNAMIC_SEQRAM_132
    {0x4134, 2, 61442}, // DYNAMIC_SEQRAM_134
    {0x4136, 2, 37274}, // DYNAMIC_SEQRAM_136
    {0x4138, 2, 35312}, // DYNAMIC_SEQRAM_138
    {0x413A, 2, 153}, // DYNAMIC_SEQRAM_13A
    {0x413C, 2, 38896}, // DYNAMIC_SEQRAM_13C
    {0x413E, 2, 48}, // DYNAMIC_SEQRAM_13E
    {0x4140, 2, 49392}, // DYNAMIC_SEQRAM_140
    {0x4142, 2, 130}, // DYNAMIC_SEQRAM_142
    {0x4144, 2, 61440}, // DYNAMIC_SEQRAM_144
    {0x4146, 2, 12312}, // DYNAMIC_SEQRAM_146
    {0x4148, 2, 61442}, // DYNAMIC_SEQRAM_148
    {0x414A, 2, 46368}, // DYNAMIC_SEQRAM_14A
    {0x414C, 2, 22768}, // DYNAMIC_SEQRAM_14C
    {0x414E, 2, 2204}, // DYNAMIC_SEQRAM_14E
    {0x4150, 2, 61457}, // DYNAMIC_SEQRAM_150
    {0x4152, 2, 46832}, // DYNAMIC_SEQRAM_152
    {0x4154, 2, 153}, // DYNAMIC_SEQRAM_154
    {0x4156, 2, 61441}, // DYNAMIC_SEQRAM_156
    {0x4158, 2, 46232}, // DYNAMIC_SEQRAM_158
    {0x415A, 2, 41110}, // DYNAMIC_SEQRAM_15A
    {0x415C, 2, 61440}, // DYNAMIC_SEQRAM_15C
    {0x415E, 2, 41712}, // DYNAMIC_SEQRAM_15E
    {0x4160, 2, 162}, // DYNAMIC_SEQRAM_160
    {0x4162, 2, 61448}, // DYNAMIC_SEQRAM_162
    {0x4164, 2, 40432}, // DYNAMIC_SEQRAM_164
    {0x4166, 2, 8349}, // DYNAMIC_SEQRAM_166
    {0x4168, 2, 35848}, // DYNAMIC_SEQRAM_168
    {0x416A, 2, 2288}, // DYNAMIC_SEQRAM_16A
    {0x416C, 2, 54}, // DYNAMIC_SEQRAM_16C
    {0x416E, 2, 143}, // DYNAMIC_SEQRAM_16E
    {0x4170, 2, 35056}, // DYNAMIC_SEQRAM_170
    {0x4172, 2, 392}, // DYNAMIC_SEQRAM_172
    {0x4174, 2, 13824}, // DYNAMIC_SEQRAM_174
    {0x4176, 2, 61440}, // DYNAMIC_SEQRAM_176
    {0x4178, 2, 33776}, // DYNAMIC_SEQRAM_178
    {0x417A, 2, 656}, // DYNAMIC_SEQRAM_17A
    {0x417C, 2, 61441}, // DYNAMIC_SEQRAM_17C
    {0x417E, 2, 35824}, // DYNAMIC_SEQRAM_17E
    {0x4180, 2, 11683}, // DYNAMIC_SEQRAM_180
    {0x4182, 2, 61440}, // DYNAMIC_SEQRAM_182
    {0x4184, 2, 41968}, // DYNAMIC_SEQRAM_184
    {0x4186, 2, 2205}, // DYNAMIC_SEQRAM_186
    {0x4188, 2, 61549}, // DYNAMIC_SEQRAM_188
    {0x418A, 2, 16496}, // DYNAMIC_SEQRAM_18A
    {0x418C, 2, 12291}, // DYNAMIC_SEQRAM_18C
    {0x418E, 2, 8525}, // DYNAMIC_SEQRAM_18E
    {0x4190, 2, 8182}, // DYNAMIC_SEQRAM_190
    {0x4192, 2, 2129}, // DYNAMIC_SEQRAM_192
    {0x4194, 2, 581}, // DYNAMIC_SEQRAM_194
    {0x4196, 2, 40246}, // DYNAMIC_SEQRAM_196
    {0x4198, 2, 55536}, // DYNAMIC_SEQRAM_198
    {0x419A, 2, 131}, // DYNAMIC_SEQRAM_19A
    {0x419C, 2, 61440}, // DYNAMIC_SEQRAM_19C
    {0x419E, 2, 34800}, // DYNAMIC_SEQRAM_19E
    {0x41A0, 2, 135}, // DYNAMIC_SEQRAM_1A0
    {0x41A2, 2, 61440}, // DYNAMIC_SEQRAM_1A2
    {0x41A4, 2, 14040}, // DYNAMIC_SEQRAM_1A4
    {0x41A6, 2, 525}, // DYNAMIC_SEQRAM_1A6
    {0x41A8, 2, 517}, // DYNAMIC_SEQRAM_1A8
    {0x41AA, 2, 61440}, // DYNAMIC_SEQRAM_1AA
    {0x41AC, 2, 14040}, // DYNAMIC_SEQRAM_1AC
    {0x41AE, 2, 61440}, // DYNAMIC_SEQRAM_1AE
    {0x41B0, 2, 33776}, // DYNAMIC_SEQRAM_1B0
    {0x41B2, 2, 135}, // DYNAMIC_SEQRAM_1B2
    {0x41B4, 2, 61440}, // DYNAMIC_SEQRAM_1B4
    {0x41B6, 2, 34800}, // DYNAMIC_SEQRAM_1B6
    {0x41B8, 2, 54}, // DYNAMIC_SEQRAM_1B8
    {0x41BA, 2, 55298}, // DYNAMIC_SEQRAM_1BA
    {0x41BC, 2, 3330}, // DYNAMIC_SEQRAM_1BC
    {0x41BE, 2, 1520}, // DYNAMIC_SEQRAM_1B3
    {0x41C0, 2, 54}, // DYNAMIC_SEQRAM_1C0
    {0x41C2, 2, 55536}, // DYNAMIC_SEQRAM_1C2
    {0x41C4, 2, 131}, // DYNAMIC_SEQRAM_1C4
    {0x41C6, 2, 61440}, // DYNAMIC_SEQRAM_1C6
    {0x41C8, 2, 34800}, // DYNAMIC_SEQRAM_1C8
    {0x41CA, 2, 135}, // DYNAMIC_SEQRAM_1CA
    {0x41CC, 2, 61440}, // DYNAMIC_SEQRAM_1CC
    {0x41CE, 2, 14040}, // DYNAMIC_SEQRAM_1CE
    {0x41D0, 2, 525}, // DYNAMIC_SEQRAM_1D0
    {0x41D2, 2, 517}, // DYNAMIC_SEQRAM_1D2
    {0x41D4, 2, 61440}, // DYNAMIC_SEQRAM_1D4
    {0x41D6, 2, 14040}, // DYNAMIC_SEQRAM_1D6
    {0x41D8, 2, 61440}, // DYNAMIC_SEQRAM_1D8
    {0x41DA, 2, 33776}, // DYNAMIC_SEQRAM_1DA
    {0x41DC, 2, 135}, // DYNAMIC_SEQRAM_1DC
    {0x41DE, 2, 61440}, // DYNAMIC_SEQRAM_1DE
    {0x41E0, 2, 34579}, // DYNAMIC_SEQRAM_1E0
    {0x41E2, 2, 54}, // DYNAMIC_SEQRAM_1E2
    {0x41E4, 2, 55298}, // DYNAMIC_SEQRAM_1E4
    {0x41E6, 2, 3552}, // DYNAMIC_SEQRAM_1E6
    {0x41E8, 2, 40723}, // DYNAMIC_SEQRAM_1E8
    {0x41EA, 2, 65}, // DYNAMIC_SEQRAM_1EA
    {0x41EC, 2, 33011}, // DYNAMIC_SEQRAM_1EC
    {0x41EE, 2, 61971}, // DYNAMIC_SEQRAM_1EE
    {0x41F0, 2, 240}, // DYNAMIC_SEQRAM_1F0
    {0x41F2, 2, 5048}, // DYNAMIC_SEQRAM_1F2
    {0x41F4, 2, 61516}, // DYNAMIC_SEQRAM_1F4
    {0x41F6, 2, 40944}, // DYNAMIC_SEQRAM_1F6
    {0x41F8, 2, 183}, // DYNAMIC_SEQRAM_1F8
    {0x41FA, 2, 61446}, // DYNAMIC_SEQRAM_1FA
    {0x41FC, 2, 53}, // DYNAMIC_SEQRAM_1FC
    {0x41FE, 2, 4271}, // DYNAMIC_SEQRAM_1FE
    {0x4200, 2, 12291}, // DYNAMIC_SEQRAM_200
    {0x4202, 2, 12480}, // DYNAMIC_SEQRAM_202
    {0x4204, 2, 45808}, // DYNAMIC_SEQRAM_204
    {0x4206, 2, 437}, // DYNAMIC_SEQRAM_206
    {0x4208, 2, 61441}, // DYNAMIC_SEQRAM_208
    {0x420A, 2, 34288}, // DYNAMIC_SEQRAM_20A
    {0x420C, 2, 658}, // DYNAMIC_SEQRAM_20C
    {0x420E, 2, 61440}, // DYNAMIC_SEQRAM_20E
    {0x4210, 2, 39563}, // DYNAMIC_SEQRAM_210
    {0x4212, 2, 61440}, // DYNAMIC_SEQRAM_212
    {0x4214, 2, 39319}, // DYNAMIC_SEQRAM_214
    {0x4216, 2, 61447}, // DYNAMIC_SEQRAM_216
    {0x4218, 2, 46832}, // DYNAMIC_SEQRAM_218
    {0x421A, 2, 32}, // DYNAMIC_SEQRAM_21A
    {0x421C, 2, 22576}, // DYNAMIC_SEQRAM_21C
    {0x421E, 2, 49216}, // DYNAMIC_SEQRAM_21E
    {0x4220, 2, 4738}, // DYNAMIC_SEQRAM_220
    {0x4222, 2, 61445}, // DYNAMIC_SEQRAM_222
    {0x4224, 2, 40176}, // DYNAMIC_SEQRAM_224
    {0x4226, 2, 434}, // DYNAMIC_SEQRAM_226
    {0x4228, 2, 61448}, // DYNAMIC_SEQRAM_228
    {0x422A, 2, 47344}, // DYNAMIC_SEQRAM_22A
    {0x422C, 2, 1945}, // DYNAMIC_SEQRAM_22C
    {0x422E, 2, 61445}, // DYNAMIC_SEQRAM_22E
    {0x4230, 2, 39152}, // DYNAMIC_SEQRAM_230
    {0x4232, 2, 662}, // DYNAMIC_SEQRAM_232
    {0x4234, 2, 41712}, // DYNAMIC_SEQRAM_234
    {0x4236, 2, 162}, // DYNAMIC_SEQRAM_236
    {0x4238, 2, 61448}, // DYNAMIC_SEQRAM_238
    {0x423A, 2, 40432}, // DYNAMIC_SEQRAM_23A
    {0x423C, 2, 673}, // DYNAMIC_SEQRAM_23C
    {0x423E, 2, 61471}, // DYNAMIC_SEQRAM_23E
    {0x4240, 2, 4105}, // DYNAMIC_SEQRAM_240
    {0x4242, 2, 8736}, // DYNAMIC_SEQRAM_242
    {0x4244, 2, 2056}, // DYNAMIC_SEQRAM_244
    {0x4246, 2, 61440}, // DYNAMIC_SEQRAM_246
    {0x4248, 2, 13824}, // DYNAMIC_SEQRAM_248
    {0x424A, 2, 61440}, // DYNAMIC_SEQRAM_24A
    {0x424C, 2, 35056}, // DYNAMIC_SEQRAM_24C
    {0x424E, 2, 1928}, // DYNAMIC_SEQRAM_24E
    {0x4250, 2, 13824}, // DYNAMIC_SEQRAM_250
    {0x4252, 2, 61440}, // DYNAMIC_SEQRAM_252
    {0x4254, 2, 33776}, // DYNAMIC_SEQRAM_254
    {0x4256, 2, 656}, // DYNAMIC_SEQRAM_256
    {0x4258, 2, 61462}, // DYNAMIC_SEQRAM_258
    {0x425A, 2, 35824}, // DYNAMIC_SEQRAM_25A
    {0x425C, 2, 4515}, // DYNAMIC_SEQRAM_25C
    {0x425E, 2, 61440}, // DYNAMIC_SEQRAM_25E
    {0x4260, 2, 41968}, // DYNAMIC_SEQRAM_260
    {0x4262, 2, 2205}, // DYNAMIC_SEQRAM_262
    {0x4264, 2, 61442}, // DYNAMIC_SEQRAM_264
    {0x4266, 2, 41456}, // DYNAMIC_SEQRAM_266
    {0x4268, 2, 8353}, // DYNAMIC_SEQRAM_268
    {0x426A, 2, 61446}, // DYNAMIC_SEQRAM_26A
    {0x426C, 2, 17152}, // DYNAMIC_SEQRAM_26C
    {0x426E, 2, 61513}, // DYNAMIC_SEQRAM_26E
    {0x4270, 2, 16404}, // DYNAMIC_SEQRAM_270
    {0x4272, 2, 35726}, // DYNAMIC_SEQRAM_272
    {0x4274, 2, 40432}, // DYNAMIC_SEQRAM_274
    {0x4276, 2, 2050}, // DYNAMIC_SEQRAM_276
    {0x4278, 2, 752}, // DYNAMIC_SEQRAM_278
    {0x427A, 2, 166}, // DYNAMIC_SEQRAM_27A
    {0x427C, 2, 61459}, // DYNAMIC_SEQRAM_27C
    {0x427E, 2, 45699}, // DYNAMIC_SEQRAM_27E
    {0x4280, 2, 39990}, // DYNAMIC_SEQRAM_280
    {0x4282, 2, 240}, // DYNAMIC_SEQRAM_282
    {0x4284, 2, 1590}, // DYNAMIC_SEQRAM_284
    {0x4286, 2, 156}, // DYNAMIC_SEQRAM_286
    {0x4288, 2, 61448}, // DYNAMIC_SEQRAM_288
    {0x428A, 2, 35824}, // DYNAMIC_SEQRAM_28A
    {0x428C, 2, 131}, // DYNAMIC_SEQRAM_28C
    {0x428E, 2, 41200}, // DYNAMIC_SEQRAM_28E
    {0x4290, 2, 1584}, // DYNAMIC_SEQRAM_290
    {0x4292, 2, 6384}, // DYNAMIC_SEQRAM_292
    {0x4294, 2, 675}, // DYNAMIC_SEQRAM_294
    {0x4296, 2, 61440}, // DYNAMIC_SEQRAM_296
    {0x4298, 2, 41968}, // DYNAMIC_SEQRAM_298
    {0x429A, 2, 579}, // DYNAMIC_SEQRAM_29A
    {0x429C, 2, 240}, // DYNAMIC_SEQRAM_29C
    {0x429E, 2, 1181}, // DYNAMIC_SEQRAM_29E
    {0x42A0, 2, 61560}, // DYNAMIC_SEQRAM_2A0
    {0x42A2, 2, 12312}, // DYNAMIC_SEQRAM_2A2
    {0x42A4, 2, 61440}, // DYNAMIC_SEQRAM_2A4
    {0x42A6, 2, 40322}, // DYNAMIC_SEQRAM_2A6
    {0x42A8, 2, 61440}, // DYNAMIC_SEQRAM_2A8
    {0x42AA, 2, 36912}, // DYNAMIC_SEQRAM_2AA
    {0x42AC, 2, 49392}, // DYNAMIC_SEQRAM_2AC
    {0x42AE, 2, 4400}, // DYNAMIC_SEQRAM_2AE
    {0x42B0, 2, 49392}, // DYNAMIC_SEQRAM_2B0
    {0x42B2, 2, 130}, // DYNAMIC_SEQRAM_2B2
    {0x42B4, 2, 61441}, // DYNAMIC_SEQRAM_2B4
    {0x42B6, 2, 4105}, // DYNAMIC_SEQRAM_2B6
    {0x42B8, 2, 61482}, // DYNAMIC_SEQRAM_2B8
    {0x42BA, 2, 41712}, // DYNAMIC_SEQRAM_2BA
    {0x42BC, 2, 162}, // DYNAMIC_SEQRAM_2BC
    {0x42BE, 2, 12312}, // DYNAMIC_SEQRAM_2BE
    {0x42C0, 2, 61447}, // DYNAMIC_SEQRAM_2C0
    {0x42C2, 2, 40432}, // DYNAMIC_SEQRAM_2C2
    {0x42C4, 2, 7308}, // DYNAMIC_SEQRAM_2C4
    {0x42C6, 2, 61445}, // DYNAMIC_SEQRAM_2C6
    {0x42C8, 2, 12319}, // DYNAMIC_SEQRAM_2C8
    {0x42CA, 2, 8557}, // DYNAMIC_SEQRAM_2CA
    {0x42CC, 2, 2641}, // DYNAMIC_SEQRAM_2CC
    {0x42CE, 2, 8170}, // DYNAMIC_SEQRAM_2CE
    {0x42D0, 2, 34368}, // DYNAMIC_SEQRAM_2D0
    {0x42D2, 2, 58015}, // DYNAMIC_SEQRAM_2D2
    {0x42D4, 2, 61449}, // DYNAMIC_SEQRAM_2D4
    {0x42D6, 2, 5}, // DYNAMIC_SEQRAM_2D6
    {0x42D8, 2, 61440}, // DYNAMIC_SEQRAM_2D8
    {0x42DA, 2, 12480}, // DYNAMIC_SEQRAM_2DA
    {0x42DC, 2, 61441}, // DYNAMIC_SEQRAM_2DC
    {0x42DE, 2, 33776}, // DYNAMIC_SEQRAM_2DE
    {0x42E0, 2, 54}, // DYNAMIC_SEQRAM_2E0
    {0x42E2, 2, 240}, // DYNAMIC_SEQRAM_2E2
    {0x42E4, 2, 135}, // DYNAMIC_SEQRAM_2E4
    {0x42E6, 2, 61447}, // DYNAMIC_SEQRAM_2E6
    {0x42E8, 2, 34800}, // DYNAMIC_SEQRAM_2E8
    {0x42EA, 2, 54}, // DYNAMIC_SEQRAM_2EA
    {0x42EC, 2, 49392}, // DYNAMIC_SEQRAM_2EC
    {0x42EE, 2, 0}, // DYNAMIC_SEQRAM_2EE
    {0x42F0, 2, 3568}, // DYNAMIC_SEQRAM_2F0
    {0x42F2, 2, 0}, // DYNAMIC_SEQRAM_2F2
    {0x42F4, 2, 1520}, // DYNAMIC_SEQRAM_2F4
    {0x42F6, 2, 48}, // DYNAMIC_SEQRAM_2F6
    {0x42F8, 2, 49392}, // DYNAMIC_SEQRAM_2F8
    {0x42FA, 2, 387}, // DYNAMIC_SEQRAM_2FA
    {0x42FC, 2, 61440}, // DYNAMIC_SEQRAM_2FC
    {0x42FE, 2, 13824}, // DYNAMIC_SEQRAM_2FE
    {0x4300, 2, 61440}, // DYNAMIC_SEQRAM_300
    {0x4302, 2, 34800}, // DYNAMIC_SEQRAM_302
    {0x4304, 2, 1927}, // DYNAMIC_SEQRAM_304
    {0x4306, 2, 61440}, // DYNAMIC_SEQRAM_306
    {0x4308, 2, 14016}, // DYNAMIC_SEQRAM_308
    {0x430A, 2, 61440}, // DYNAMIC_SEQRAM_30A
    {0x430C, 2, 15}, // DYNAMIC_SEQRAM_30C
    {0x430E, 2, 62506}, // DYNAMIC_SEQRAM_30E
    {0x4310, 2, 16768}, // DYNAMIC_SEQRAM_310
    {0x4312, 2, 4864}, // DYNAMIC_SEQRAM_312
    {0x4314, 2, 40944}, // DYNAMIC_SEQRAM_314
    {0x4316, 2, 224}, // DYNAMIC_SEQRAM_316
    {0x4318, 2, 57568}, // DYNAMIC_SEQRAM_318
    {0x431A, 2, 57568}, // DYNAMIC_SEQRAM_31A
    {0x431C, 2, 57568}, // DYNAMIC_SEQRAM_31C
    {0x431E, 2, 57568}, // DYNAMIC_SEQRAM_31E
    {0x4320, 2, 57568}, // DYNAMIC_SEQRAM_320
    {0x4322, 2, 57568}, // DYNAMIC_SEQRAM_322
    {0x4324, 2, 57568}, // DYNAMIC_SEQRAM_324
    {0x4326, 2, 57568}, // DYNAMIC_SEQRAM_326
    {0x4328, 2, 57568}, // DYNAMIC_SEQRAM_328
    {0x432A, 2, 57568}, // DYNAMIC_SEQRAM_32A
    {0x432C, 2, 57568}, // DYNAMIC_SEQRAM_32C
    {0x432E, 2, 57568}, // DYNAMIC_SEQRAM_32E
    {0x4330, 2, 57568}, // DYNAMIC_SEQRAM_330
    {0x4332, 2, 57568}, // DYNAMIC_SEQRAM_332
    {0x4334, 2, 57568}, // DYNAMIC_SEQRAM_334
    {0x4336, 2, 57568}, // DYNAMIC_SEQRAM_336
    {0x4338, 2, 57568}, // DYNAMIC_SEQRAM_338
    {0x433A, 2, 57568}, // DYNAMIC_SEQRAM_33A
    {0x433C, 2, 57568}, // DYNAMIC_SEQRAM_33C
    {0x433E, 2, 57568}, // DYNAMIC_SEQRAM_33E
    {0x4340, 2, 57568}, // DYNAMIC_SEQRAM_340
    {0x4342, 2, 57568}, // DYNAMIC_SEQRAM_342
    {0x4344, 2, 57568}, // DYNAMIC_SEQRAM_344
    {0x4346, 2, 57568}, // DYNAMIC_SEQRAM_346
    {0x4348, 2, 57568}, // DYNAMIC_SEQRAM_348
    {0x434A, 2, 57568}, // DYNAMIC_SEQRAM_34A
    {0x434C, 2, 57568}, // DYNAMIC_SEQRAM_34C
    {0x434E, 2, 57568}, // DYNAMIC_SEQRAM_34E
    {0x4350, 2, 57568}, // DYNAMIC_SEQRAM_350
    {0x4352, 2, 57568}, // DYNAMIC_SEQRAM_352
    {0x4354, 2, 57568}, // DYNAMIC_SEQRAM_354
    {0x4356, 2, 57568}, // DYNAMIC_SEQRAM_356
    {0x4358, 2, 57568}, // DYNAMIC_SEQRAM_358
    {0x435A, 2, 57568}, // DYNAMIC_SEQRAM_35A
    {0x435C, 2, 57568}, // DYNAMIC_SEQRAM_35C
    {0x435E, 2, 57568}, // DYNAMIC_SEQRAM_35E
    {0x4360, 2, 57568}, // DYNAMIC_SEQRAM_360
    {0x4362, 2, 57568}, // DYNAMIC_SEQRAM_362
    {0x4364, 2, 57568}, // DYNAMIC_SEQRAM_364
    {0x4366, 2, 57568}, // DYNAMIC_SEQRAM_366
    {0x4368, 2, 57568}, // DYNAMIC_SEQRAM_368
    {0x436A, 2, 57568}, // DYNAMIC_SEQRAM_36A
    {0x436C, 2, 57568}, // DYNAMIC_SEQRAM_36C
    {0x436E, 2, 57568}, // DYNAMIC_SEQRAM_36E
    {0x4370, 2, 57568}, // DYNAMIC_SEQRAM_370
    {0x4372, 2, 57568}, // DYNAMIC_SEQRAM_372
    {0x4374, 2, 57568}, // DYNAMIC_SEQRAM_374
    {0x4376, 2, 57568}, // DYNAMIC_SEQRAM_376
    {0x4378, 2, 57568}, // DYNAMIC_SEQRAM_378
    {0x437A, 2, 57568}, // DYNAMIC_SEQRAM_37A
    {0x437C, 2, 57568}, // DYNAMIC_SEQRAM_37C
    {0x437E, 2, 57568}, // DYNAMIC_SEQRAM_37E
    {0x4380, 2, 57568}, // DYNAMIC_SEQRAM_380
    {0x4382, 2, 57568}, // DYNAMIC_SEQRAM_382
    {0x4384, 2, 57568}, // DYNAMIC_SEQRAM_384
    {0x4386, 2, 57568}, // DYNAMIC_SEQRAM_386
    {0x4388, 2, 57568}, // DYNAMIC_SEQRAM_388
    {0x438A, 2, 57568}, // DYNAMIC_SEQRAM_38A
    {0x438C, 2, 57568}, // DYNAMIC_SEQRAM_38C
    {0x438E, 2, 57568}, // DYNAMIC_SEQRAM_38E
    {0x4390, 2, 57568}, // DYNAMIC_SEQRAM_390
    {0x4392, 2, 57568}, // DYNAMIC_SEQRAM_392
    {0x4394, 2, 57568}, // DYNAMIC_SEQRAM_394
    {0x4396, 2, 57568}, // DYNAMIC_SEQRAM_396
    {0x4398, 2, 57568}, // DYNAMIC_SEQRAM_398
    {0x439A, 2, 57568}, // DYNAMIC_SEQRAM_39A
    {0x439C, 2, 57568}, // DYNAMIC_SEQRAM_39C
    {0x439E, 2, 57568}, // DYNAMIC_SEQRAM_39E
    {0x43A0, 2, 57568}, // DYNAMIC_SEQRAM_3A0
    {0x43A2, 2, 57568}, // DYNAMIC_SEQRAM_3A2
    {0x43A4, 2, 57568}, // DYNAMIC_SEQRAM_3A4
    {0x43A6, 2, 57568}, // DYNAMIC_SEQRAM_3A6
    {0x43A8, 2, 57568}, // DYNAMIC_SEQRAM_3A8
    {0x43AA, 2, 57568}, // DYNAMIC_SEQRAM_3AA
    {0x43AC, 2, 57568}, // DYNAMIC_SEQRAM_3AC
    {0x43AE, 2, 57568}, // DYNAMIC_SEQRAM_3AE
    {0x43B0, 2, 57568}, // DYNAMIC_SEQRAM_3B0
    {0x43B2, 2, 57568}, // DYNAMIC_SEQRAM_3B2
    {0x43B4, 2, 57568}, // DYNAMIC_SEQRAM_3B4
    {0x43B6, 2, 57568}, // DYNAMIC_SEQRAM_3B6
    {0x43B8, 2, 57568}, // DYNAMIC_SEQRAM_3B8
    {0x43BA, 2, 57568}, // DYNAMIC_SEQRAM_3BA
    {0x43BC, 2, 57568}, // DYNAMIC_SEQRAM_3BC
    {0x43BE, 2, 57568}, // DYNAMIC_SEQRAM_3BE
    {0x43C0, 2, 57568}, // DYNAMIC_SEQRAM_3C0
    {0x43C2, 2, 57568}, // DYNAMIC_SEQRAM_3C2
    {0x43C4, 2, 57568}, // DYNAMIC_SEQRAM_3C4
    {0x43C6, 2, 57568}, // DYNAMIC_SEQRAM_3C6
    {0x43C8, 2, 57568}, // DYNAMIC_SEQRAM_3C8
    {0x43CA, 2, 57568}, // DYNAMIC_SEQRAM_3CA
    {0x43CC, 2, 57568}, // DYNAMIC_SEQRAM_3CC
    {0x43CE, 2, 57568}, // DYNAMIC_SEQRAM_3CE
    {0x43D0, 2, 57568}, // DYNAMIC_SEQRAM_3D0
    {0x43D2, 2, 57568}, // DYNAMIC_SEQRAM_3D2
    {0x43D4, 2, 57568}, // DYNAMIC_SEQRAM_3D4
    {0x43D6, 2, 57568}, // DYNAMIC_SEQRAM_3D6
    {0x43D8, 2, 57568}, // DYNAMIC_SEQRAM_3D8
    {0x43DA, 2, 57568}, // DYNAMIC_SEQRAM_3DA
    {0x43DC, 2, 57568}, // DYNAMIC_SEQRAM_3DC
    {0x43DE, 2, 57568}, // DYNAMIC_SEQRAM_3DE
    {0x43E0, 2, 57568}, // DYNAMIC_SEQRAM_3E0
    {0x43E2, 2, 57568}, // DYNAMIC_SEQRAM_3E2
    {0x43E4, 2, 57568}, // DYNAMIC_SEQRAM_3E4
    {0x43E6, 2, 57568}, // DYNAMIC_SEQRAM_3E6
    {0x43E8, 2, 57568}, // DYNAMIC_SEQRAM_3E8
    {0x43EA, 2, 57568}, // DYNAMIC_SEQRAM_3EA
    {0x43EC, 2, 57568}, // DYNAMIC_SEQRAM_3EC
    {0x43EE, 2, 57568}, // DYNAMIC_SEQRAM_3EE
    {0x43F0, 2, 57568}, // DYNAMIC_SEQRAM_3F0
    {0x43F2, 2, 57568}, // DYNAMIC_SEQRAM_3F2
    {0x43F4, 2, 57568}, // DYNAMIC_SEQRAM_3F4
    {0x43F6, 2, 57568}, // DYNAMIC_SEQRAM_3F6
    {0x43F8, 2, 57568}, // DYNAMIC_SEQRAM_3F8
    {0x43FA, 2, 57568}, // DYNAMIC_SEQRAM_3FA
    {0x43FC, 2, 57568}, // DYNAMIC_SEQRAM_3FC
    {0x43FE, 2, 57568}, // DYNAMIC_SEQRAM_3FE
    {0x4400, 2, 57568}, // DYNAMIC_SEQRAM_400
    {0x4402, 2, 57568}, // DYNAMIC_SEQRAM_402
    {0x4404, 2, 57568}, // DYNAMIC_SEQRAM_404
    {0x4406, 2, 57568}, // DYNAMIC_SEQRAM_406
    {0x4408, 2, 57568}, // DYNAMIC_SEQRAM_408
    {0x440A, 2, 57568}, // DYNAMIC_SEQRAM_40A
    {0x440C, 2, 57568}, // DYNAMIC_SEQRAM_40C
    {0x440E, 2, 57568}, // DYNAMIC_SEQRAM_40E
    {0x4410, 2, 57568}, // DYNAMIC_SEQRAM_410
    {0x4412, 2, 57568}, // DYNAMIC_SEQRAM_412
    {0x4414, 2, 57568}, // DYNAMIC_SEQRAM_414
    {0x4416, 2, 57568}, // DYNAMIC_SEQRAM_416
    {0x4418, 2, 57568}, // DYNAMIC_SEQRAM_418
    {0x441A, 2, 57568}, // DYNAMIC_SEQRAM_41A
    {0x441C, 2, 57568}, // DYNAMIC_SEQRAM_41C
    {0x441E, 2, 57568}, // DYNAMIC_SEQRAM_41E
    {0x4420, 2, 57568}, // DYNAMIC_SEQRAM_420
    {0x4422, 2, 57568}, // DYNAMIC_SEQRAM_422
    {0x4424, 2, 57568}, // DYNAMIC_SEQRAM_424
    {0x4426, 2, 57568}, // DYNAMIC_SEQRAM_426
    {0x4428, 2, 57568}, // DYNAMIC_SEQRAM_428
    {0x442A, 2, 57568}, // DYNAMIC_SEQRAM_42A
    {0x442C, 2, 57568}, // DYNAMIC_SEQRAM_42C
    {0x442E, 2, 57568}, // DYNAMIC_SEQRAM_42E
    {0x4430, 2, 57568}, // DYNAMIC_SEQRAM_430
    {0x4432, 2, 57568}, // DYNAMIC_SEQRAM_432
    {0x4434, 2, 57568}, // DYNAMIC_SEQRAM_434
    {0x4436, 2, 57568}, // DYNAMIC_SEQRAM_436
    {0x4438, 2, 57568}, // DYNAMIC_SEQRAM_438
    {0x443A, 2, 57568}, // DYNAMIC_SEQRAM_43A
    {0x443C, 2, 57568}, // DYNAMIC_SEQRAM_43C
    {0x443E, 2, 57568}, // DYNAMIC_SEQRAM_43E
    {0x4440, 2, 57568}, // DYNAMIC_SEQRAM_440
    {0x4442, 2, 57568}, // DYNAMIC_SEQRAM_442
    {0x4444, 2, 57568}, // DYNAMIC_SEQRAM_444
    {0x4446, 2, 57568}, // DYNAMIC_SEQRAM_446
    {0x4448, 2, 57568}, // DYNAMIC_SEQRAM_448
    {0x444A, 2, 57568}, // DYNAMIC_SEQRAM_44A
    {0x444C, 2, 57568}, // DYNAMIC_SEQRAM_44C
    {0x444E, 2, 57568}, // DYNAMIC_SEQRAM_44E
    {0x4450, 2, 57568}, // DYNAMIC_SEQRAM_450
    {0x4452, 2, 57568}, // DYNAMIC_SEQRAM_452
    {0x4454, 2, 57568}, // DYNAMIC_SEQRAM_454
    {0x4456, 2, 57568}, // DYNAMIC_SEQRAM_456
    {0x4458, 2, 57568}, // DYNAMIC_SEQRAM_458
    {0x445A, 2, 57568}, // DYNAMIC_SEQRAM_45A
    {0x445C, 2, 57568}, // DYNAMIC_SEQRAM_45C
    {0x445E, 2, 57568}, // DYNAMIC_SEQRAM_45E
    {0x4460, 2, 57568}, // DYNAMIC_SEQRAM_460
    {0x4462, 2, 57568}, // DYNAMIC_SEQRAM_462
    {0x4464, 2, 57568}, // DYNAMIC_SEQRAM_464
    {0x4466, 2, 57568}, // DYNAMIC_SEQRAM_466
    {0x4468, 2, 57568}, // DYNAMIC_SEQRAM_468
    {0x446A, 2, 57568}, // DYNAMIC_SEQRAM_46A
    {0x446C, 2, 57568}, // DYNAMIC_SEQRAM_46C
    {0x446E, 2, 57568}, // DYNAMIC_SEQRAM_46E
    {0x4470, 2, 57568}, // DYNAMIC_SEQRAM_470
    {0x4472, 2, 57568}, // DYNAMIC_SEQRAM_472
    {0x4474, 2, 57568}, // DYNAMIC_SEQRAM_474
    {0x4476, 2, 57568}, // DYNAMIC_SEQRAM_476
    {0x4478, 2, 57568}, // DYNAMIC_SEQRAM_478
    {0x447A, 2, 57568}, // DYNAMIC_SEQRAM_47A
    {0x447C, 2, 57568}, // DYNAMIC_SEQRAM_47C
    {0x447E, 2, 57568}, // DYNAMIC_SEQRAM_47E
    {0x4480, 2, 57568}, // DYNAMIC_SEQRAM_480
    {0x4482, 2, 57568}, // DYNAMIC_SEQRAM_482
    {0x4484, 2, 57568}, // DYNAMIC_SEQRAM_484
    {0x4486, 2, 57568}, // DYNAMIC_SEQRAM_486
    {0x4488, 2, 57568}, // DYNAMIC_SEQRAM_488
    {0x448A, 2, 57568}, // DYNAMIC_SEQRAM_48A
    {0x448C, 2, 57568}, // DYNAMIC_SEQRAM_48C
    {0x448E, 2, 57568}, // DYNAMIC_SEQRAM_48E
    {0x4490, 2, 57568}, // DYNAMIC_SEQRAM_490
    {0x4492, 2, 57568}, // DYNAMIC_SEQRAM_492
    {0x4494, 2, 57568}, // DYNAMIC_SEQRAM_494
    {0x4496, 2, 57568}, // DYNAMIC_SEQRAM_496
    {0x4498, 2, 57568}, // DYNAMIC_SEQRAM_498
    {0x449A, 2, 57568}, // DYNAMIC_SEQRAM_49A
    {0x449C, 2, 57568}, // DYNAMIC_SEQRAM_49C
    {0x449E, 2, 57568}, // DYNAMIC_SEQRAM_49E
    {0x44A0, 2, 57568}, // DYNAMIC_SEQRAM_4A0
    {0x44A2, 2, 57568}, // DYNAMIC_SEQRAM_4A2
    {0x44A4, 2, 57568}, // DYNAMIC_SEQRAM_4A4
    {0x44A6, 2, 57568}, // DYNAMIC_SEQRAM_4A6
    {0x44A8, 2, 57568}, // DYNAMIC_SEQRAM_4A8
    {0x44AA, 2, 57568}, // DYNAMIC_SEQRAM_4AA
    {0x44AC, 2, 57568}, // DYNAMIC_SEQRAM_4AC
    {0x44AE, 2, 57568}, // DYNAMIC_SEQRAM_4AE
    {0x44B0, 2, 57568}, // DYNAMIC_SEQRAM_4B0
    {0x44B2, 2, 57568}, // DYNAMIC_SEQRAM_4B2
    {0x44B4, 2, 57568}, // DYNAMIC_SEQRAM_4B4
    {0x5500, 2, 0}, // AGAIN_LUT0
    {0x5502, 2, 2}, // AGAIN_LUT1
    {0x5504, 2, 6}, // AGAIN_LUT2
    {0x5506, 2, 9}, // AGAIN_LUT3
    {0x5508, 2, 15}, // AGAIN_LUT4
    {0x550A, 2, 16}, // AGAIN_LUT5
    {0x550C, 2, 17}, // AGAIN_LUT6
    {0x550E, 2, 18}, // AGAIN_LUT7
    {0x5510, 2, 25}, // AGAIN_LUT8
    {0x5512, 2, 32}, // AGAIN_LUT9
    {0x5514, 2, 33}, // AGAIN_LUT10
    {0x5516, 2, 35}, // AGAIN_LUT11
    {0x5518, 2, 38}, // AGAIN_LUT12
    {0x551A, 2, 43}, // AGAIN_LUT13
    {0x551C, 2, 47}, // AGAIN_LUT14
    {0x551E, 2, 48}, // AGAIN_LUT15
    {0x5400, 2, 256}, // GT1_COARSE0
    {0x5402, 2, 8454}, // GT1_COARSE1
    {0x5404, 2, 4353}, // GT1_COARSE2
    {0x5406, 2, 12550}, // GT1_COARSE3
    {0x5408, 2, 28928}, // GT1_COARSE4
    {0x540A, 2, 33031}, // GT1_COARSE5
    {0x540C, 2, 45313}, // GT1_COARSE6
    {0x540E, 2, 53505}, // GT1_COARSE7
    {0x5410, 2, 61742}, // GT1_COARSE8
    {0x5412, 2, 61714}, // GT1_COARSE9
    {0x5414, 2, 61828}, // GT1_COARSE10
    {0x5416, 2, 61988}, // GT1_COARSE11
    {0x5418, 2, 62214}, // GT1_COARSE12
    {0x541A, 2, 62534}, // GT1_COARSE13
    {0x541C, 2, 62985}, // GT1_COARSE14
    {0x541E, 2, 63623}, // GT1_COARSE15
    {0x5420, 2, 64523}, // GT1_COARSE16
    {0x5422, 2, 64523}, // GT1_COARSE17
    {0x5424, 2, 65530}, // GT1_DCG_ATTN_SET0
    {0x5426, 2, 21847}, // GT1_DCG_ATTN_SET1
    {0x5428, 2, 5}, // GT1_DCG_ATTN_SET2
    {0x542A, 2, 42320}, // GT1_ZONE_SET0
    {0x542C, 2, 43690}, // GT1_ZONE_SET1
    {0x542E, 2, 10}, // GT1_ZONE_SET2
    {0x5460, 2, 8809}, // ZT1_REG0_ADDR
    {0x5462, 2, 2951}, // ZT1_REG0_VALUE0
    {0x5464, 2, 2951}, // ZT1_REG0_VALUE1
    {0x5466, 2, 2435}, // ZT1_REG0_VALUE2
    {0x5498, 2, 8798}, // ZT1_REG7_ADDR
    {0x549A, 2, 48298}, // ZT1_REG7_VALUE0
    {0x549C, 2, 48298}, // ZT1_REG7_VALUE1
    {0x549E, 2, 48554}, // ZT1_REG7_VALUE2
    {0x3060, 2, 65281}, // GAIN_TABLE_CTRL
    {0x44BA, 2, 72}, // DAC_LD_4_5
    {0x44BC, 2, 48298}, // DAC_LD_6_7
    {0x44C0, 2, 16496}, // DAC_LD_10_11
    {0x44C4, 2, 1232}, // DAC_LD_14_15
    {0x44C6, 2, 6114}, // DAC_LD_16_17
    {0x44C8, 2, 59971}, // DAC_LD_18_19
    {0x44CA, 2, 14}, // DAC_LD_20_21
    {0x44CC, 2, 30583}, // DAC_LD_22_23
    {0x44CE, 2, 35748}, // DAC_LD_24_25
    {0x44D0, 2, 5941}, // DAC_LD_26_27
    {0x44D2, 2, 2951}, // DAC_LD_28_29
    {0x44D4, 2, 32768}, // DAC_LD_30_31
    {0x44D6, 2, 61958}, // DAC_LD_32_33
    {0x44D8, 2, 43770}, // DAC_LD_34_35
    {0x44DA, 2, 57345}, // DAC_LD_36_37
    {0x44DE, 2, 39868}, // DAC_LD_40_41
    {0x44E0, 2, 10300}, // DAC_LD_42_43
    {0x44E2, 2, 10273}, // DAC_LD_44_45
    {0x44E4, 2, 32768}, // DAC_LD_46_47
    {0x44E6, 2, 20543}, // DAC_LD_48_49
    {0x32A4, 2, 0}, // CRM_CTRL
    {0x333C, 2, 1}, // DYNAMIC_CTRL
    {0x0300, 2, 6}, // VT_PIX_CLK_DIV
    {0x0304, 2, 2}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 88}, //{0x0306, 2, 103}, // VT_PLL_MULTIPLIER
    {0x3980, 2, 1}, // PIX_DEF_CORR
    {0x3700, 2, 1}, // PIX_DEF_ID
    {0x3340, 2, 7264}, // OTPM_CTRL
    {0x3340, 2, 7264}, // OTPM_CTRL
    {0x36C0, 2, 1}, // DIGITAL_GAIN_CTRL

    {0x3062, 2, 0x002b}, // fenghuan add
    {0x36c4, 2, 0x1c0},
    {0x36c6, 2, 0x1c0},
    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE
    {0x0100, 1, 1}, // MODE_SELECT
    {0x44D6, 2, 45574}, // DAC_LD_32_33
};

const GEN_SENSOR_DRVG_regTblParamsT AR2020_CFG_TBLG_MIPI_24M_4LANE_1024x1024_90_table[] =
{
    {0x0103, 0x3201, 0x01},
    {0x0304, 2, 2}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 86}, // VT_PLL_MULTIPLIER
    {0x0300, 2, 6}, // VT_PIX_CLK_DIV
    {0x0302, 2, 1}, // VT_SYS_CLK_DIV
    {0x030C, 2, 7}, // OP_PRE_PLL_CLK_DIV
    {0x030E, 2, 290}, // OP_PLL_MULTIPLIER
    {0x0308, 2, 10}, // OP_PIX_CLK_DIV
    {0x030A, 2, 1}, // OP_SYS_CLK_DIV

    {0x0344, 2, 2056}, // X_ADDR_START
    {0x0348, 2, 3079}, // X_ADDR_END
    {0x0346, 2, 1416}, // Y_ADDR_START
    {0x034A, 2, 2439}, // Y_ADDR_END
    {0x034C, 2, 1024}, // X_OUTPUT_SIZE
    {0x034E, 2, 1024}, // Y_OUTPUT_SIZE
    {0x0380, 2, 1}, // X_EVEN_INC
    {0x0382, 2, 1}, // X_ODD_INC
    {0x0384, 2, 1}, // Y_EVEN_INC
    {0x0386, 2, 1}, // Y_ODD_INC
    {0x0900, 1, 0}, // BINNING_MODE
    {0x0901, 1, 17}, // BINNING_TYPE
    {0x0342, 2, 7250}, //{0x0342, 2, 5872}, // LINE_LENGTH_PCK 0x1ce0 = 7392
    {0x0340, 2, 2108}, //{0x0340, 2, 1038}, // FRAME_LENGTH_LINES 0x9a0 = 2464
    {0x0202, 2, 2600}, //{0x0202, 2, 2603}, // COARSE_INTEGRATION_TIME
    {0x0112, 2, 2570}, // CSI_DATA_FORMAT
    {0x0114, 1, 3}, // CSI_LANE_MODE

    {0x0800, 1, 12}, // TCLK_POST
    {0x0801, 1, 6}, // THS_PREPARE
    {0x0802, 1, 10}, // THS_ZERO_MIN
    {0x0803, 1, 8}, // THS_TRAIL
    {0x0804, 1, 9}, // TCLK_TRAIL_MIN
    {0x0805, 1, 6}, // TCLK_PREPARE
    {0x0806, 1, 30}, // TCLK_ZERO
    {0x0807, 1, 6}, // TLPX
    {0x082A, 1, 13}, // TWAKEUP
    {0x082B, 1, 10}, // TINIT
    {0x082C, 1, 11}, // THS_EXIT
    {0x3F06, 2, 192}, // MIPI_TIMING_2
    {0x3F0A, 2, 0}, //{0x3F0A, 2, 32768}, // MIPI_TIMING_4
    {0x3F0C, 2, 8}, // MIPI_TIMING_5

    {0x3F20, 2, 2056}, // MIPI_PHY_TRIM_MSB
    {0x3F02, 2, 2}, // PHY_CTRL

    {0x4000, 2, 276}, // DYNAMIC_SEQRAM_00
    {0x4002, 2, 6693}, // DYNAMIC_SEQRAM_02
    {0x4004, 2, 15871}, // DYNAMIC_SEQRAM_04
    {0x4006, 2, 65535}, // DYNAMIC_SEQRAM_06
    {0x4008, 2, 2613}, // DYNAMIC_SEQRAM_08
    {0x400A, 2, 4239}, // DYNAMIC_SEQRAM_0A
    {0x400C, 2, 12291}, // DYNAMIC_SEQRAM_0C
    {0x400E, 2, 12504}, // DYNAMIC_SEQRAM_0E
    {0x4010, 2, 61442}, // DYNAMIC_SEQRAM_10
    {0x4012, 2, 37557}, // DYNAMIC_SEQRAM_12
    {0x4014, 2, 61440}, // DYNAMIC_SEQRAM_14
    {0x4016, 2, 34288}, // DYNAMIC_SEQRAM_16
    {0x4018, 2, 145}, // DYNAMIC_SEQRAM_18
    {0x401A, 2, 61442}, // DYNAMIC_SEQRAM_1A
    {0x401C, 2, 39561}, // DYNAMIC_SEQRAM_1C
    {0x401E, 2, 61440}, // DYNAMIC_SEQRAM_1E
    {0x4020, 2, 39319}, // DYNAMIC_SEQRAM_20
    {0x4022, 2, 61440}, // DYNAMIC_SEQRAM_22
    {0x4024, 2, 12480}, // DYNAMIC_SEQRAM_24
    {0x4026, 2, 61440}, // DYNAMIC_SEQRAM_26
    {0x4028, 2, 33520}, // DYNAMIC_SEQRAM_28
    {0x402A, 2, 48}, // DYNAMIC_SEQRAM_2A
    {0x402C, 2, 6384}, // DYNAMIC_SEQRAM_2C
    {0x402E, 2, 800}, // DYNAMIC_SEQRAM_2E
    {0x4030, 2, 22768}, // DYNAMIC_SEQRAM_30
    {0x4032, 2, 2204}, // DYNAMIC_SEQRAM_32
    {0x4034, 2, 61457}, // DYNAMIC_SEQRAM_34
    {0x4036, 2, 46832}, // DYNAMIC_SEQRAM_36
    {0x4038, 2, 153}, // DYNAMIC_SEQRAM_38
    {0x403A, 2, 61441}, // DYNAMIC_SEQRAM_3A
    {0x403C, 2, 46232}, // DYNAMIC_SEQRAM_3C
    {0x403E, 2, 41110}, // DYNAMIC_SEQRAM_3E
    {0x4040, 2, 61440}, // DYNAMIC_SEQRAM_40
    {0x4042, 2, 41712}, // DYNAMIC_SEQRAM_42
    {0x4044, 2, 162}, // DYNAMIC_SEQRAM_44
    {0x4046, 2, 61448}, // DYNAMIC_SEQRAM_46
    {0x4048, 2, 40432}, // DYNAMIC_SEQRAM_48
    {0x404A, 2, 8349}, // DYNAMIC_SEQRAM_4A
    {0x404C, 2, 35848}, // DYNAMIC_SEQRAM_4C
    {0x404E, 2, 2288}, // DYNAMIC_SEQRAM_4E
    {0x4050, 2, 54}, // DYNAMIC_SEQRAM_50
    {0x4052, 2, 143}, // DYNAMIC_SEQRAM_52
    {0x4054, 2, 61440}, // DYNAMIC_SEQRAM_54
    {0x4056, 2, 35056}, // DYNAMIC_SEQRAM_56
    {0x4058, 2, 1160}, // DYNAMIC_SEQRAM_58
    {0x405A, 2, 61440}, // DYNAMIC_SEQRAM_5A
    {0x405C, 2, 13824}, // DYNAMIC_SEQRAM_5C
    {0x405E, 2, 61440}, // DYNAMIC_SEQRAM_5E
    {0x4060, 2, 33776}, // DYNAMIC_SEQRAM_60
    {0x4062, 2, 656}, // DYNAMIC_SEQRAM_62
    {0x4064, 2, 61440}, // DYNAMIC_SEQRAM_64
    {0x4066, 2, 35824}, // DYNAMIC_SEQRAM_66
    {0x4068, 2, 11939}, // DYNAMIC_SEQRAM_68
    {0x406A, 2, 61440}, // DYNAMIC_SEQRAM_6A
    {0x406C, 2, 41968}, // DYNAMIC_SEQRAM_6C
    {0x406E, 2, 2205}, // DYNAMIC_SEQRAM_6E
    {0x4070, 2, 61557}, // DYNAMIC_SEQRAM_70
    {0x4072, 2, 12291}, // DYNAMIC_SEQRAM_72
    {0x4074, 2, 16496}, // DYNAMIC_SEQRAM_74
    {0x4076, 2, 8557}, // DYNAMIC_SEQRAM_76
    {0x4078, 2, 7414}, // DYNAMIC_SEQRAM_78
    {0x407A, 2, 35584}, // DYNAMIC_SEQRAM_7A
    {0x407C, 2, 20870}, // DYNAMIC_SEQRAM_7C
    {0x407E, 2, 4864}, // DYNAMIC_SEQRAM_7E
    {0x4080, 2, 517}, // DYNAMIC_SEQRAM_80
    {0x4082, 2, 14040}, // DYNAMIC_SEQRAM_82
    {0x4084, 2, 61442}, // DYNAMIC_SEQRAM_84
    {0x4086, 2, 33671}, // DYNAMIC_SEQRAM_86
    {0x4088, 2, 61446}, // DYNAMIC_SEQRAM_88
    {0x408A, 2, 34562}, // DYNAMIC_SEQRAM_8A
    {0x408C, 2, 3330}, // DYNAMIC_SEQRAM_8C
    {0x408E, 2, 1520}, // DYNAMIC_SEQRAM_8E
    {0x4090, 2, 899}, // DYNAMIC_SEQRAM_90
    {0x4092, 2, 61441}, // DYNAMIC_SEQRAM_92
    {0x4094, 2, 34800}, // DYNAMIC_SEQRAM_94
    {0x4096, 2, 531}, // DYNAMIC_SEQRAM_96
    {0x4098, 2, 54}, // DYNAMIC_SEQRAM_98
    {0x409A, 2, 55431}, // DYNAMIC_SEQRAM_9A
    {0x409C, 2, 525}, // DYNAMIC_SEQRAM_9C
    {0x409E, 2, 57568}, // DYNAMIC_SEQRAM_9E
    {0x40A0, 2, 61440}, // DYNAMIC_SEQRAM_A0
    {0x40A2, 2, 1025}, // DYNAMIC_SEQRAM_A2
    {0x40A4, 2, 61448}, // DYNAMIC_SEQRAM_A4
    {0x40A6, 2, 33520}, // DYNAMIC_SEQRAM_A6
    {0x40A8, 2, 2179}, // DYNAMIC_SEQRAM_A8
    {0x40AA, 2, 61449}, // DYNAMIC_SEQRAM_AA
    {0x40AC, 2, 34288}, // DYNAMIC_SEQRAM_AC
    {0x40AE, 2, 10629}, // DYNAMIC_SEQRAM_AE
    {0x40B0, 2, 34800}, // DYNAMIC_SEQRAM_B0
    {0x40B2, 2, 10887}, // DYNAMIC_SEQRAM_B2
    {0x40B4, 2, 63038}, // DYNAMIC_SEQRAM_B4
    {0x40B6, 2, 35056}, // DYNAMIC_SEQRAM_B6
    {0x40B8, 2, 2049}, // DYNAMIC_SEQRAM_B8
    {0x40BA, 2, 16624}, // DYNAMIC_SEQRAM_BA
    {0x40BC, 2, 2048}, // DYNAMIC_SEQRAM_BC
    {0x40BE, 2, 18672}, // DYNAMIC_SEQRAM_BE
    {0x40C0, 2, 2178}, // DYNAMIC_SEQRAM_C0
    {0x40C2, 2, 61448}, // DYNAMIC_SEQRAM_C2
    {0x40C4, 2, 1025}, // DYNAMIC_SEQRAM_C4
    {0x40C6, 2, 61448}, // DYNAMIC_SEQRAM_C6
    {0x40C8, 2, 57568}, // DYNAMIC_SEQRAM_C8
    {0x40CA, 2, 57568}, // DYNAMIC_SEQRAM_CA
    {0x40CC, 2, 57568}, // DYNAMIC_SEQRAM_CC
    {0x40CE, 2, 57568}, // DYNAMIC_SEQRAM_CE
    {0x40D0, 2, 61440}, // DYNAMIC_SEQRAM_D0
    {0x40D2, 2, 1025}, // DYNAMIC_SEQRAM_D2
    {0x40D4, 2, 61461}, // DYNAMIC_SEQRAM_D4
    {0x40D6, 2, 60}, // DYNAMIC_SEQRAM_D6
    {0x40D8, 2, 61454}, // DYNAMIC_SEQRAM_D8
    {0x40DA, 2, 34288}, // DYNAMIC_SEQRAM_DA
    {0x40DC, 2, 1671}, // DYNAMIC_SEQRAM_DC
    {0x40DE, 2, 61442}, // DYNAMIC_SEQRAM_DE
    {0x40E0, 2, 34800}, // DYNAMIC_SEQRAM_E0
    {0x40E2, 2, 25064}, // DYNAMIC_SEQRAM_E2
    {0x40E4, 2, 14596}, // DYNAMIC_SEQRAM_E4
    {0x40E6, 2, 61445}, // DYNAMIC_SEQRAM_E6
    {0x40E8, 2, 13442}, // DYNAMIC_SEQRAM_E8
    {0x40EA, 2, 61440}, // DYNAMIC_SEQRAM_EA
    {0x40EC, 2, 12865}, // DYNAMIC_SEQRAM_EC
    {0x40EE, 2, 61440}, // DYNAMIC_SEQRAM_EE
    {0x40F0, 2, 14596}, // DYNAMIC_SEQRAM_F0
    {0x40F2, 2, 61454}, // DYNAMIC_SEQRAM_F2
    {0x40F4, 2, 14596}, // DYNAMIC_SEQRAM_F4
    {0x40F6, 2, 61440}, // DYNAMIC_SEQRAM_F6
    {0x40F8, 2, 12865}, // DYNAMIC_SEQRAM_F8
    {0x40FA, 2, 61440}, // DYNAMIC_SEQRAM_FA
    {0x40FC, 2, 13442}, // DYNAMIC_SEQRAM_FC
    {0x40FE, 2, 61445}, // DYNAMIC_SEQRAM_FE
    {0x4100, 2, 49382}, // DYNAMIC_SEQRAM_100
    {0x4102, 2, 61444}, // DYNAMIC_SEQRAM_102
    {0x4104, 2, 14596}, // DYNAMIC_SEQRAM_104
    {0x4106, 2, 61443}, // DYNAMIC_SEQRAM_106
    {0x4108, 2, 45296}, // DYNAMIC_SEQRAM_108
    {0x410A, 2, 0}, // DYNAMIC_SEQRAM_10A
    {0x410C, 2, 6384}, // DYNAMIC_SEQRAM_10C
    {0x410E, 2, 134}, // DYNAMIC_SEQRAM_10E
    {0x4110, 2, 61440}, // DYNAMIC_SEQRAM_110
    {0x4112, 2, 34544}, // DYNAMIC_SEQRAM_112
    {0x4114, 2, 35248}, // DYNAMIC_SEQRAM_114
    {0x4116, 2, 61440}, // DYNAMIC_SEQRAM_116
    {0x4118, 2, 59888}, // DYNAMIC_SEQRAM_118
    {0x411A, 2, 138}, // DYNAMIC_SEQRAM_11A
    {0x411C, 2, 61440}, // DYNAMIC_SEQRAM_11C
    {0x411E, 2, 5}, // DYNAMIC_SEQRAM_11E
    {0x4120, 2, 61440}, // DYNAMIC_SEQRAM_120
    {0x4122, 2, 57568}, // DYNAMIC_SEQRAM_122
    {0x4124, 2, 57568}, // DYNAMIC_SEQRAM_124
    {0x4126, 2, 57568}, // DYNAMIC_SEQRAM_126
    {0x4128, 2, 2613}, // DYNAMIC_SEQRAM_128
    {0x412A, 2, 4239}, // DYNAMIC_SEQRAM_12A
    {0x412C, 2, 12291}, // DYNAMIC_SEQRAM_12C
    {0x412E, 2, 12504}, // DYNAMIC_SEQRAM_12E
    {0x4130, 2, 61445}, // DYNAMIC_SEQRAM_130
    {0x4132, 2, 34194}, // DYNAMIC_SEQRAM_132
    {0x4134, 2, 61442}, // DYNAMIC_SEQRAM_134
    {0x4136, 2, 37274}, // DYNAMIC_SEQRAM_136
    {0x4138, 2, 35312}, // DYNAMIC_SEQRAM_138
    {0x413A, 2, 153}, // DYNAMIC_SEQRAM_13A
    {0x413C, 2, 38896}, // DYNAMIC_SEQRAM_13C
    {0x413E, 2, 48}, // DYNAMIC_SEQRAM_13E
    {0x4140, 2, 49392}, // DYNAMIC_SEQRAM_140
    {0x4142, 2, 130}, // DYNAMIC_SEQRAM_142
    {0x4144, 2, 61440}, // DYNAMIC_SEQRAM_144
    {0x4146, 2, 12312}, // DYNAMIC_SEQRAM_146
    {0x4148, 2, 61442}, // DYNAMIC_SEQRAM_148
    {0x414A, 2, 46368}, // DYNAMIC_SEQRAM_14A
    {0x414C, 2, 22768}, // DYNAMIC_SEQRAM_14C
    {0x414E, 2, 2204}, // DYNAMIC_SEQRAM_14E
    {0x4150, 2, 61457}, // DYNAMIC_SEQRAM_150
    {0x4152, 2, 46832}, // DYNAMIC_SEQRAM_152
    {0x4154, 2, 153}, // DYNAMIC_SEQRAM_154
    {0x4156, 2, 61441}, // DYNAMIC_SEQRAM_156
    {0x4158, 2, 46232}, // DYNAMIC_SEQRAM_158
    {0x415A, 2, 41110}, // DYNAMIC_SEQRAM_15A
    {0x415C, 2, 61440}, // DYNAMIC_SEQRAM_15C
    {0x415E, 2, 41712}, // DYNAMIC_SEQRAM_15E
    {0x4160, 2, 162}, // DYNAMIC_SEQRAM_160
    {0x4162, 2, 61448}, // DYNAMIC_SEQRAM_162
    {0x4164, 2, 40432}, // DYNAMIC_SEQRAM_164
    {0x4166, 2, 8349}, // DYNAMIC_SEQRAM_166
    {0x4168, 2, 35848}, // DYNAMIC_SEQRAM_168
    {0x416A, 2, 2288}, // DYNAMIC_SEQRAM_16A
    {0x416C, 2, 54}, // DYNAMIC_SEQRAM_16C
    {0x416E, 2, 143}, // DYNAMIC_SEQRAM_16E
    {0x4170, 2, 35056}, // DYNAMIC_SEQRAM_170
    {0x4172, 2, 392}, // DYNAMIC_SEQRAM_172
    {0x4174, 2, 13824}, // DYNAMIC_SEQRAM_174
    {0x4176, 2, 61440}, // DYNAMIC_SEQRAM_176
    {0x4178, 2, 33776}, // DYNAMIC_SEQRAM_178
    {0x417A, 2, 656}, // DYNAMIC_SEQRAM_17A
    {0x417C, 2, 61441}, // DYNAMIC_SEQRAM_17C
    {0x417E, 2, 35824}, // DYNAMIC_SEQRAM_17E
    {0x4180, 2, 11683}, // DYNAMIC_SEQRAM_180
    {0x4182, 2, 61440}, // DYNAMIC_SEQRAM_182
    {0x4184, 2, 41968}, // DYNAMIC_SEQRAM_184
    {0x4186, 2, 2205}, // DYNAMIC_SEQRAM_186
    {0x4188, 2, 61549}, // DYNAMIC_SEQRAM_188
    {0x418A, 2, 16496}, // DYNAMIC_SEQRAM_18A
    {0x418C, 2, 12291}, // DYNAMIC_SEQRAM_18C
    {0x418E, 2, 8525}, // DYNAMIC_SEQRAM_18E
    {0x4190, 2, 8182}, // DYNAMIC_SEQRAM_190
    {0x4192, 2, 2129}, // DYNAMIC_SEQRAM_192
    {0x4194, 2, 581}, // DYNAMIC_SEQRAM_194
    {0x4196, 2, 40246}, // DYNAMIC_SEQRAM_196
    {0x4198, 2, 55536}, // DYNAMIC_SEQRAM_198
    {0x419A, 2, 131}, // DYNAMIC_SEQRAM_19A
    {0x419C, 2, 61440}, // DYNAMIC_SEQRAM_19C
    {0x419E, 2, 34800}, // DYNAMIC_SEQRAM_19E
    {0x41A0, 2, 135}, // DYNAMIC_SEQRAM_1A0
    {0x41A2, 2, 61440}, // DYNAMIC_SEQRAM_1A2
    {0x41A4, 2, 14040}, // DYNAMIC_SEQRAM_1A4
    {0x41A6, 2, 525}, // DYNAMIC_SEQRAM_1A6
    {0x41A8, 2, 517}, // DYNAMIC_SEQRAM_1A8
    {0x41AA, 2, 61440}, // DYNAMIC_SEQRAM_1AA
    {0x41AC, 2, 14040}, // DYNAMIC_SEQRAM_1AC
    {0x41AE, 2, 61440}, // DYNAMIC_SEQRAM_1AE
    {0x41B0, 2, 33776}, // DYNAMIC_SEQRAM_1B0
    {0x41B2, 2, 135}, // DYNAMIC_SEQRAM_1B2
    {0x41B4, 2, 61440}, // DYNAMIC_SEQRAM_1B4
    {0x41B6, 2, 34800}, // DYNAMIC_SEQRAM_1B6
    {0x41B8, 2, 54}, // DYNAMIC_SEQRAM_1B8
    {0x41BA, 2, 55298}, // DYNAMIC_SEQRAM_1BA
    {0x41BC, 2, 3330}, // DYNAMIC_SEQRAM_1BC
    {0x41BE, 2, 1520}, // DYNAMIC_SEQRAM_1B3
    {0x41C0, 2, 54}, // DYNAMIC_SEQRAM_1C0
    {0x41C2, 2, 55536}, // DYNAMIC_SEQRAM_1C2
    {0x41C4, 2, 131}, // DYNAMIC_SEQRAM_1C4
    {0x41C6, 2, 61440}, // DYNAMIC_SEQRAM_1C6
    {0x41C8, 2, 34800}, // DYNAMIC_SEQRAM_1C8
    {0x41CA, 2, 135}, // DYNAMIC_SEQRAM_1CA
    {0x41CC, 2, 61440}, // DYNAMIC_SEQRAM_1CC
    {0x41CE, 2, 14040}, // DYNAMIC_SEQRAM_1CE
    {0x41D0, 2, 525}, // DYNAMIC_SEQRAM_1D0
    {0x41D2, 2, 517}, // DYNAMIC_SEQRAM_1D2
    {0x41D4, 2, 61440}, // DYNAMIC_SEQRAM_1D4
    {0x41D6, 2, 14040}, // DYNAMIC_SEQRAM_1D6
    {0x41D8, 2, 61440}, // DYNAMIC_SEQRAM_1D8
    {0x41DA, 2, 33776}, // DYNAMIC_SEQRAM_1DA
    {0x41DC, 2, 135}, // DYNAMIC_SEQRAM_1DC
    {0x41DE, 2, 61440}, // DYNAMIC_SEQRAM_1DE
    {0x41E0, 2, 34579}, // DYNAMIC_SEQRAM_1E0
    {0x41E2, 2, 54}, // DYNAMIC_SEQRAM_1E2
    {0x41E4, 2, 55298}, // DYNAMIC_SEQRAM_1E4
    {0x41E6, 2, 3552}, // DYNAMIC_SEQRAM_1E6
    {0x41E8, 2, 40723}, // DYNAMIC_SEQRAM_1E8
    {0x41EA, 2, 65}, // DYNAMIC_SEQRAM_1EA
    {0x41EC, 2, 33011}, // DYNAMIC_SEQRAM_1EC
    {0x41EE, 2, 61971}, // DYNAMIC_SEQRAM_1EE
    {0x41F0, 2, 240}, // DYNAMIC_SEQRAM_1F0
    {0x41F2, 2, 5048}, // DYNAMIC_SEQRAM_1F2
    {0x41F4, 2, 61516}, // DYNAMIC_SEQRAM_1F4
    {0x41F6, 2, 40944}, // DYNAMIC_SEQRAM_1F6
    {0x41F8, 2, 183}, // DYNAMIC_SEQRAM_1F8
    {0x41FA, 2, 61446}, // DYNAMIC_SEQRAM_1FA
    {0x41FC, 2, 53}, // DYNAMIC_SEQRAM_1FC
    {0x41FE, 2, 4271}, // DYNAMIC_SEQRAM_1FE
    {0x4200, 2, 12291}, // DYNAMIC_SEQRAM_200
    {0x4202, 2, 12480}, // DYNAMIC_SEQRAM_202
    {0x4204, 2, 45808}, // DYNAMIC_SEQRAM_204
    {0x4206, 2, 437}, // DYNAMIC_SEQRAM_206
    {0x4208, 2, 61441}, // DYNAMIC_SEQRAM_208
    {0x420A, 2, 34288}, // DYNAMIC_SEQRAM_20A
    {0x420C, 2, 658}, // DYNAMIC_SEQRAM_20C
    {0x420E, 2, 61440}, // DYNAMIC_SEQRAM_20E
    {0x4210, 2, 39563}, // DYNAMIC_SEQRAM_210
    {0x4212, 2, 61440}, // DYNAMIC_SEQRAM_212
    {0x4214, 2, 39319}, // DYNAMIC_SEQRAM_214
    {0x4216, 2, 61447}, // DYNAMIC_SEQRAM_216
    {0x4218, 2, 46832}, // DYNAMIC_SEQRAM_218
    {0x421A, 2, 32}, // DYNAMIC_SEQRAM_21A
    {0x421C, 2, 22576}, // DYNAMIC_SEQRAM_21C
    {0x421E, 2, 49216}, // DYNAMIC_SEQRAM_21E
    {0x4220, 2, 4738}, // DYNAMIC_SEQRAM_220
    {0x4222, 2, 61445}, // DYNAMIC_SEQRAM_222
    {0x4224, 2, 40176}, // DYNAMIC_SEQRAM_224
    {0x4226, 2, 434}, // DYNAMIC_SEQRAM_226
    {0x4228, 2, 61448}, // DYNAMIC_SEQRAM_228
    {0x422A, 2, 47344}, // DYNAMIC_SEQRAM_22A
    {0x422C, 2, 1945}, // DYNAMIC_SEQRAM_22C
    {0x422E, 2, 61445}, // DYNAMIC_SEQRAM_22E
    {0x4230, 2, 39152}, // DYNAMIC_SEQRAM_230
    {0x4232, 2, 662}, // DYNAMIC_SEQRAM_232
    {0x4234, 2, 41712}, // DYNAMIC_SEQRAM_234
    {0x4236, 2, 162}, // DYNAMIC_SEQRAM_236
    {0x4238, 2, 61448}, // DYNAMIC_SEQRAM_238
    {0x423A, 2, 40432}, // DYNAMIC_SEQRAM_23A
    {0x423C, 2, 673}, // DYNAMIC_SEQRAM_23C
    {0x423E, 2, 61471}, // DYNAMIC_SEQRAM_23E
    {0x4240, 2, 4105}, // DYNAMIC_SEQRAM_240
    {0x4242, 2, 8736}, // DYNAMIC_SEQRAM_242
    {0x4244, 2, 2056}, // DYNAMIC_SEQRAM_244
    {0x4246, 2, 61440}, // DYNAMIC_SEQRAM_246
    {0x4248, 2, 13824}, // DYNAMIC_SEQRAM_248
    {0x424A, 2, 61440}, // DYNAMIC_SEQRAM_24A
    {0x424C, 2, 35056}, // DYNAMIC_SEQRAM_24C
    {0x424E, 2, 1928}, // DYNAMIC_SEQRAM_24E
    {0x4250, 2, 13824}, // DYNAMIC_SEQRAM_250
    {0x4252, 2, 61440}, // DYNAMIC_SEQRAM_252
    {0x4254, 2, 33776}, // DYNAMIC_SEQRAM_254
    {0x4256, 2, 656}, // DYNAMIC_SEQRAM_256
    {0x4258, 2, 61462}, // DYNAMIC_SEQRAM_258
    {0x425A, 2, 35824}, // DYNAMIC_SEQRAM_25A
    {0x425C, 2, 4515}, // DYNAMIC_SEQRAM_25C
    {0x425E, 2, 61440}, // DYNAMIC_SEQRAM_25E
    {0x4260, 2, 41968}, // DYNAMIC_SEQRAM_260
    {0x4262, 2, 2205}, // DYNAMIC_SEQRAM_262
    {0x4264, 2, 61442}, // DYNAMIC_SEQRAM_264
    {0x4266, 2, 41456}, // DYNAMIC_SEQRAM_266
    {0x4268, 2, 8353}, // DYNAMIC_SEQRAM_268
    {0x426A, 2, 61446}, // DYNAMIC_SEQRAM_26A
    {0x426C, 2, 17152}, // DYNAMIC_SEQRAM_26C
    {0x426E, 2, 61513}, // DYNAMIC_SEQRAM_26E
    {0x4270, 2, 16404}, // DYNAMIC_SEQRAM_270
    {0x4272, 2, 35726}, // DYNAMIC_SEQRAM_272
    {0x4274, 2, 40432}, // DYNAMIC_SEQRAM_274
    {0x4276, 2, 2050}, // DYNAMIC_SEQRAM_276
    {0x4278, 2, 752}, // DYNAMIC_SEQRAM_278
    {0x427A, 2, 166}, // DYNAMIC_SEQRAM_27A
    {0x427C, 2, 61459}, // DYNAMIC_SEQRAM_27C
    {0x427E, 2, 45699}, // DYNAMIC_SEQRAM_27E
    {0x4280, 2, 39990}, // DYNAMIC_SEQRAM_280
    {0x4282, 2, 240}, // DYNAMIC_SEQRAM_282
    {0x4284, 2, 1590}, // DYNAMIC_SEQRAM_284
    {0x4286, 2, 156}, // DYNAMIC_SEQRAM_286
    {0x4288, 2, 61448}, // DYNAMIC_SEQRAM_288
    {0x428A, 2, 35824}, // DYNAMIC_SEQRAM_28A
    {0x428C, 2, 131}, // DYNAMIC_SEQRAM_28C
    {0x428E, 2, 41200}, // DYNAMIC_SEQRAM_28E
    {0x4290, 2, 1584}, // DYNAMIC_SEQRAM_290
    {0x4292, 2, 6384}, // DYNAMIC_SEQRAM_292
    {0x4294, 2, 675}, // DYNAMIC_SEQRAM_294
    {0x4296, 2, 61440}, // DYNAMIC_SEQRAM_296
    {0x4298, 2, 41968}, // DYNAMIC_SEQRAM_298
    {0x429A, 2, 579}, // DYNAMIC_SEQRAM_29A
    {0x429C, 2, 240}, // DYNAMIC_SEQRAM_29C
    {0x429E, 2, 1181}, // DYNAMIC_SEQRAM_29E
    {0x42A0, 2, 61560}, // DYNAMIC_SEQRAM_2A0
    {0x42A2, 2, 12312}, // DYNAMIC_SEQRAM_2A2
    {0x42A4, 2, 61440}, // DYNAMIC_SEQRAM_2A4
    {0x42A6, 2, 40322}, // DYNAMIC_SEQRAM_2A6
    {0x42A8, 2, 61440}, // DYNAMIC_SEQRAM_2A8
    {0x42AA, 2, 36912}, // DYNAMIC_SEQRAM_2AA
    {0x42AC, 2, 49392}, // DYNAMIC_SEQRAM_2AC
    {0x42AE, 2, 4400}, // DYNAMIC_SEQRAM_2AE
    {0x42B0, 2, 49392}, // DYNAMIC_SEQRAM_2B0
    {0x42B2, 2, 130}, // DYNAMIC_SEQRAM_2B2
    {0x42B4, 2, 61441}, // DYNAMIC_SEQRAM_2B4
    {0x42B6, 2, 4105}, // DYNAMIC_SEQRAM_2B6
    {0x42B8, 2, 61482}, // DYNAMIC_SEQRAM_2B8
    {0x42BA, 2, 41712}, // DYNAMIC_SEQRAM_2BA
    {0x42BC, 2, 162}, // DYNAMIC_SEQRAM_2BC
    {0x42BE, 2, 12312}, // DYNAMIC_SEQRAM_2BE
    {0x42C0, 2, 61447}, // DYNAMIC_SEQRAM_2C0
    {0x42C2, 2, 40432}, // DYNAMIC_SEQRAM_2C2
    {0x42C4, 2, 7308}, // DYNAMIC_SEQRAM_2C4
    {0x42C6, 2, 61445}, // DYNAMIC_SEQRAM_2C6
    {0x42C8, 2, 12319}, // DYNAMIC_SEQRAM_2C8
    {0x42CA, 2, 8557}, // DYNAMIC_SEQRAM_2CA
    {0x42CC, 2, 2641}, // DYNAMIC_SEQRAM_2CC
    {0x42CE, 2, 8170}, // DYNAMIC_SEQRAM_2CE
    {0x42D0, 2, 34368}, // DYNAMIC_SEQRAM_2D0
    {0x42D2, 2, 58015}, // DYNAMIC_SEQRAM_2D2
    {0x42D4, 2, 61449}, // DYNAMIC_SEQRAM_2D4
    {0x42D6, 2, 5}, // DYNAMIC_SEQRAM_2D6
    {0x42D8, 2, 61440}, // DYNAMIC_SEQRAM_2D8
    {0x42DA, 2, 12480}, // DYNAMIC_SEQRAM_2DA
    {0x42DC, 2, 61441}, // DYNAMIC_SEQRAM_2DC
    {0x42DE, 2, 33776}, // DYNAMIC_SEQRAM_2DE
    {0x42E0, 2, 54}, // DYNAMIC_SEQRAM_2E0
    {0x42E2, 2, 240}, // DYNAMIC_SEQRAM_2E2
    {0x42E4, 2, 135}, // DYNAMIC_SEQRAM_2E4
    {0x42E6, 2, 61447}, // DYNAMIC_SEQRAM_2E6
    {0x42E8, 2, 34800}, // DYNAMIC_SEQRAM_2E8
    {0x42EA, 2, 54}, // DYNAMIC_SEQRAM_2EA
    {0x42EC, 2, 49392}, // DYNAMIC_SEQRAM_2EC
    {0x42EE, 2, 0}, // DYNAMIC_SEQRAM_2EE
    {0x42F0, 2, 3568}, // DYNAMIC_SEQRAM_2F0
    {0x42F2, 2, 0}, // DYNAMIC_SEQRAM_2F2
    {0x42F4, 2, 1520}, // DYNAMIC_SEQRAM_2F4
    {0x42F6, 2, 48}, // DYNAMIC_SEQRAM_2F6
    {0x42F8, 2, 49392}, // DYNAMIC_SEQRAM_2F8
    {0x42FA, 2, 387}, // DYNAMIC_SEQRAM_2FA
    {0x42FC, 2, 61440}, // DYNAMIC_SEQRAM_2FC
    {0x42FE, 2, 13824}, // DYNAMIC_SEQRAM_2FE
    {0x4300, 2, 61440}, // DYNAMIC_SEQRAM_300
    {0x4302, 2, 34800}, // DYNAMIC_SEQRAM_302
    {0x4304, 2, 1927}, // DYNAMIC_SEQRAM_304
    {0x4306, 2, 61440}, // DYNAMIC_SEQRAM_306
    {0x4308, 2, 14016}, // DYNAMIC_SEQRAM_308
    {0x430A, 2, 61440}, // DYNAMIC_SEQRAM_30A
    {0x430C, 2, 15}, // DYNAMIC_SEQRAM_30C
    {0x430E, 2, 62506}, // DYNAMIC_SEQRAM_30E
    {0x4310, 2, 16768}, // DYNAMIC_SEQRAM_310
    {0x4312, 2, 4864}, // DYNAMIC_SEQRAM_312
    {0x4314, 2, 40944}, // DYNAMIC_SEQRAM_314
    {0x4316, 2, 224}, // DYNAMIC_SEQRAM_316
    {0x4318, 2, 57568}, // DYNAMIC_SEQRAM_318
    {0x431A, 2, 57568}, // DYNAMIC_SEQRAM_31A
    {0x431C, 2, 57568}, // DYNAMIC_SEQRAM_31C
    {0x431E, 2, 57568}, // DYNAMIC_SEQRAM_31E
    {0x4320, 2, 57568}, // DYNAMIC_SEQRAM_320
    {0x4322, 2, 57568}, // DYNAMIC_SEQRAM_322
    {0x4324, 2, 57568}, // DYNAMIC_SEQRAM_324
    {0x4326, 2, 57568}, // DYNAMIC_SEQRAM_326
    {0x4328, 2, 57568}, // DYNAMIC_SEQRAM_328
    {0x432A, 2, 57568}, // DYNAMIC_SEQRAM_32A
    {0x432C, 2, 57568}, // DYNAMIC_SEQRAM_32C
    {0x432E, 2, 57568}, // DYNAMIC_SEQRAM_32E
    {0x4330, 2, 57568}, // DYNAMIC_SEQRAM_330
    {0x4332, 2, 57568}, // DYNAMIC_SEQRAM_332
    {0x4334, 2, 57568}, // DYNAMIC_SEQRAM_334
    {0x4336, 2, 57568}, // DYNAMIC_SEQRAM_336
    {0x4338, 2, 57568}, // DYNAMIC_SEQRAM_338
    {0x433A, 2, 57568}, // DYNAMIC_SEQRAM_33A
    {0x433C, 2, 57568}, // DYNAMIC_SEQRAM_33C
    {0x433E, 2, 57568}, // DYNAMIC_SEQRAM_33E
    {0x4340, 2, 57568}, // DYNAMIC_SEQRAM_340
    {0x4342, 2, 57568}, // DYNAMIC_SEQRAM_342
    {0x4344, 2, 57568}, // DYNAMIC_SEQRAM_344
    {0x4346, 2, 57568}, // DYNAMIC_SEQRAM_346
    {0x4348, 2, 57568}, // DYNAMIC_SEQRAM_348
    {0x434A, 2, 57568}, // DYNAMIC_SEQRAM_34A
    {0x434C, 2, 57568}, // DYNAMIC_SEQRAM_34C
    {0x434E, 2, 57568}, // DYNAMIC_SEQRAM_34E
    {0x4350, 2, 57568}, // DYNAMIC_SEQRAM_350
    {0x4352, 2, 57568}, // DYNAMIC_SEQRAM_352
    {0x4354, 2, 57568}, // DYNAMIC_SEQRAM_354
    {0x4356, 2, 57568}, // DYNAMIC_SEQRAM_356
    {0x4358, 2, 57568}, // DYNAMIC_SEQRAM_358
    {0x435A, 2, 57568}, // DYNAMIC_SEQRAM_35A
    {0x435C, 2, 57568}, // DYNAMIC_SEQRAM_35C
    {0x435E, 2, 57568}, // DYNAMIC_SEQRAM_35E
    {0x4360, 2, 57568}, // DYNAMIC_SEQRAM_360
    {0x4362, 2, 57568}, // DYNAMIC_SEQRAM_362
    {0x4364, 2, 57568}, // DYNAMIC_SEQRAM_364
    {0x4366, 2, 57568}, // DYNAMIC_SEQRAM_366
    {0x4368, 2, 57568}, // DYNAMIC_SEQRAM_368
    {0x436A, 2, 57568}, // DYNAMIC_SEQRAM_36A
    {0x436C, 2, 57568}, // DYNAMIC_SEQRAM_36C
    {0x436E, 2, 57568}, // DYNAMIC_SEQRAM_36E
    {0x4370, 2, 57568}, // DYNAMIC_SEQRAM_370
    {0x4372, 2, 57568}, // DYNAMIC_SEQRAM_372
    {0x4374, 2, 57568}, // DYNAMIC_SEQRAM_374
    {0x4376, 2, 57568}, // DYNAMIC_SEQRAM_376
    {0x4378, 2, 57568}, // DYNAMIC_SEQRAM_378
    {0x437A, 2, 57568}, // DYNAMIC_SEQRAM_37A
    {0x437C, 2, 57568}, // DYNAMIC_SEQRAM_37C
    {0x437E, 2, 57568}, // DYNAMIC_SEQRAM_37E
    {0x4380, 2, 57568}, // DYNAMIC_SEQRAM_380
    {0x4382, 2, 57568}, // DYNAMIC_SEQRAM_382
    {0x4384, 2, 57568}, // DYNAMIC_SEQRAM_384
    {0x4386, 2, 57568}, // DYNAMIC_SEQRAM_386
    {0x4388, 2, 57568}, // DYNAMIC_SEQRAM_388
    {0x438A, 2, 57568}, // DYNAMIC_SEQRAM_38A
    {0x438C, 2, 57568}, // DYNAMIC_SEQRAM_38C
    {0x438E, 2, 57568}, // DYNAMIC_SEQRAM_38E
    {0x4390, 2, 57568}, // DYNAMIC_SEQRAM_390
    {0x4392, 2, 57568}, // DYNAMIC_SEQRAM_392
    {0x4394, 2, 57568}, // DYNAMIC_SEQRAM_394
    {0x4396, 2, 57568}, // DYNAMIC_SEQRAM_396
    {0x4398, 2, 57568}, // DYNAMIC_SEQRAM_398
    {0x439A, 2, 57568}, // DYNAMIC_SEQRAM_39A
    {0x439C, 2, 57568}, // DYNAMIC_SEQRAM_39C
    {0x439E, 2, 57568}, // DYNAMIC_SEQRAM_39E
    {0x43A0, 2, 57568}, // DYNAMIC_SEQRAM_3A0
    {0x43A2, 2, 57568}, // DYNAMIC_SEQRAM_3A2
    {0x43A4, 2, 57568}, // DYNAMIC_SEQRAM_3A4
    {0x43A6, 2, 57568}, // DYNAMIC_SEQRAM_3A6
    {0x43A8, 2, 57568}, // DYNAMIC_SEQRAM_3A8
    {0x43AA, 2, 57568}, // DYNAMIC_SEQRAM_3AA
    {0x43AC, 2, 57568}, // DYNAMIC_SEQRAM_3AC
    {0x43AE, 2, 57568}, // DYNAMIC_SEQRAM_3AE
    {0x43B0, 2, 57568}, // DYNAMIC_SEQRAM_3B0
    {0x43B2, 2, 57568}, // DYNAMIC_SEQRAM_3B2
    {0x43B4, 2, 57568}, // DYNAMIC_SEQRAM_3B4
    {0x43B6, 2, 57568}, // DYNAMIC_SEQRAM_3B6
    {0x43B8, 2, 57568}, // DYNAMIC_SEQRAM_3B8
    {0x43BA, 2, 57568}, // DYNAMIC_SEQRAM_3BA
    {0x43BC, 2, 57568}, // DYNAMIC_SEQRAM_3BC
    {0x43BE, 2, 57568}, // DYNAMIC_SEQRAM_3BE
    {0x43C0, 2, 57568}, // DYNAMIC_SEQRAM_3C0
    {0x43C2, 2, 57568}, // DYNAMIC_SEQRAM_3C2
    {0x43C4, 2, 57568}, // DYNAMIC_SEQRAM_3C4
    {0x43C6, 2, 57568}, // DYNAMIC_SEQRAM_3C6
    {0x43C8, 2, 57568}, // DYNAMIC_SEQRAM_3C8
    {0x43CA, 2, 57568}, // DYNAMIC_SEQRAM_3CA
    {0x43CC, 2, 57568}, // DYNAMIC_SEQRAM_3CC
    {0x43CE, 2, 57568}, // DYNAMIC_SEQRAM_3CE
    {0x43D0, 2, 57568}, // DYNAMIC_SEQRAM_3D0
    {0x43D2, 2, 57568}, // DYNAMIC_SEQRAM_3D2
    {0x43D4, 2, 57568}, // DYNAMIC_SEQRAM_3D4
    {0x43D6, 2, 57568}, // DYNAMIC_SEQRAM_3D6
    {0x43D8, 2, 57568}, // DYNAMIC_SEQRAM_3D8
    {0x43DA, 2, 57568}, // DYNAMIC_SEQRAM_3DA
    {0x43DC, 2, 57568}, // DYNAMIC_SEQRAM_3DC
    {0x43DE, 2, 57568}, // DYNAMIC_SEQRAM_3DE
    {0x43E0, 2, 57568}, // DYNAMIC_SEQRAM_3E0
    {0x43E2, 2, 57568}, // DYNAMIC_SEQRAM_3E2
    {0x43E4, 2, 57568}, // DYNAMIC_SEQRAM_3E4
    {0x43E6, 2, 57568}, // DYNAMIC_SEQRAM_3E6
    {0x43E8, 2, 57568}, // DYNAMIC_SEQRAM_3E8
    {0x43EA, 2, 57568}, // DYNAMIC_SEQRAM_3EA
    {0x43EC, 2, 57568}, // DYNAMIC_SEQRAM_3EC
    {0x43EE, 2, 57568}, // DYNAMIC_SEQRAM_3EE
    {0x43F0, 2, 57568}, // DYNAMIC_SEQRAM_3F0
    {0x43F2, 2, 57568}, // DYNAMIC_SEQRAM_3F2
    {0x43F4, 2, 57568}, // DYNAMIC_SEQRAM_3F4
    {0x43F6, 2, 57568}, // DYNAMIC_SEQRAM_3F6
    {0x43F8, 2, 57568}, // DYNAMIC_SEQRAM_3F8
    {0x43FA, 2, 57568}, // DYNAMIC_SEQRAM_3FA
    {0x43FC, 2, 57568}, // DYNAMIC_SEQRAM_3FC
    {0x43FE, 2, 57568}, // DYNAMIC_SEQRAM_3FE
    {0x4400, 2, 57568}, // DYNAMIC_SEQRAM_400
    {0x4402, 2, 57568}, // DYNAMIC_SEQRAM_402
    {0x4404, 2, 57568}, // DYNAMIC_SEQRAM_404
    {0x4406, 2, 57568}, // DYNAMIC_SEQRAM_406
    {0x4408, 2, 57568}, // DYNAMIC_SEQRAM_408
    {0x440A, 2, 57568}, // DYNAMIC_SEQRAM_40A
    {0x440C, 2, 57568}, // DYNAMIC_SEQRAM_40C
    {0x440E, 2, 57568}, // DYNAMIC_SEQRAM_40E
    {0x4410, 2, 57568}, // DYNAMIC_SEQRAM_410
    {0x4412, 2, 57568}, // DYNAMIC_SEQRAM_412
    {0x4414, 2, 57568}, // DYNAMIC_SEQRAM_414
    {0x4416, 2, 57568}, // DYNAMIC_SEQRAM_416
    {0x4418, 2, 57568}, // DYNAMIC_SEQRAM_418
    {0x441A, 2, 57568}, // DYNAMIC_SEQRAM_41A
    {0x441C, 2, 57568}, // DYNAMIC_SEQRAM_41C
    {0x441E, 2, 57568}, // DYNAMIC_SEQRAM_41E
    {0x4420, 2, 57568}, // DYNAMIC_SEQRAM_420
    {0x4422, 2, 57568}, // DYNAMIC_SEQRAM_422
    {0x4424, 2, 57568}, // DYNAMIC_SEQRAM_424
    {0x4426, 2, 57568}, // DYNAMIC_SEQRAM_426
    {0x4428, 2, 57568}, // DYNAMIC_SEQRAM_428
    {0x442A, 2, 57568}, // DYNAMIC_SEQRAM_42A
    {0x442C, 2, 57568}, // DYNAMIC_SEQRAM_42C
    {0x442E, 2, 57568}, // DYNAMIC_SEQRAM_42E
    {0x4430, 2, 57568}, // DYNAMIC_SEQRAM_430
    {0x4432, 2, 57568}, // DYNAMIC_SEQRAM_432
    {0x4434, 2, 57568}, // DYNAMIC_SEQRAM_434
    {0x4436, 2, 57568}, // DYNAMIC_SEQRAM_436
    {0x4438, 2, 57568}, // DYNAMIC_SEQRAM_438
    {0x443A, 2, 57568}, // DYNAMIC_SEQRAM_43A
    {0x443C, 2, 57568}, // DYNAMIC_SEQRAM_43C
    {0x443E, 2, 57568}, // DYNAMIC_SEQRAM_43E
    {0x4440, 2, 57568}, // DYNAMIC_SEQRAM_440
    {0x4442, 2, 57568}, // DYNAMIC_SEQRAM_442
    {0x4444, 2, 57568}, // DYNAMIC_SEQRAM_444
    {0x4446, 2, 57568}, // DYNAMIC_SEQRAM_446
    {0x4448, 2, 57568}, // DYNAMIC_SEQRAM_448
    {0x444A, 2, 57568}, // DYNAMIC_SEQRAM_44A
    {0x444C, 2, 57568}, // DYNAMIC_SEQRAM_44C
    {0x444E, 2, 57568}, // DYNAMIC_SEQRAM_44E
    {0x4450, 2, 57568}, // DYNAMIC_SEQRAM_450
    {0x4452, 2, 57568}, // DYNAMIC_SEQRAM_452
    {0x4454, 2, 57568}, // DYNAMIC_SEQRAM_454
    {0x4456, 2, 57568}, // DYNAMIC_SEQRAM_456
    {0x4458, 2, 57568}, // DYNAMIC_SEQRAM_458
    {0x445A, 2, 57568}, // DYNAMIC_SEQRAM_45A
    {0x445C, 2, 57568}, // DYNAMIC_SEQRAM_45C
    {0x445E, 2, 57568}, // DYNAMIC_SEQRAM_45E
    {0x4460, 2, 57568}, // DYNAMIC_SEQRAM_460
    {0x4462, 2, 57568}, // DYNAMIC_SEQRAM_462
    {0x4464, 2, 57568}, // DYNAMIC_SEQRAM_464
    {0x4466, 2, 57568}, // DYNAMIC_SEQRAM_466
    {0x4468, 2, 57568}, // DYNAMIC_SEQRAM_468
    {0x446A, 2, 57568}, // DYNAMIC_SEQRAM_46A
    {0x446C, 2, 57568}, // DYNAMIC_SEQRAM_46C
    {0x446E, 2, 57568}, // DYNAMIC_SEQRAM_46E
    {0x4470, 2, 57568}, // DYNAMIC_SEQRAM_470
    {0x4472, 2, 57568}, // DYNAMIC_SEQRAM_472
    {0x4474, 2, 57568}, // DYNAMIC_SEQRAM_474
    {0x4476, 2, 57568}, // DYNAMIC_SEQRAM_476
    {0x4478, 2, 57568}, // DYNAMIC_SEQRAM_478
    {0x447A, 2, 57568}, // DYNAMIC_SEQRAM_47A
    {0x447C, 2, 57568}, // DYNAMIC_SEQRAM_47C
    {0x447E, 2, 57568}, // DYNAMIC_SEQRAM_47E
    {0x4480, 2, 57568}, // DYNAMIC_SEQRAM_480
    {0x4482, 2, 57568}, // DYNAMIC_SEQRAM_482
    {0x4484, 2, 57568}, // DYNAMIC_SEQRAM_484
    {0x4486, 2, 57568}, // DYNAMIC_SEQRAM_486
    {0x4488, 2, 57568}, // DYNAMIC_SEQRAM_488
    {0x448A, 2, 57568}, // DYNAMIC_SEQRAM_48A
    {0x448C, 2, 57568}, // DYNAMIC_SEQRAM_48C
    {0x448E, 2, 57568}, // DYNAMIC_SEQRAM_48E
    {0x4490, 2, 57568}, // DYNAMIC_SEQRAM_490
    {0x4492, 2, 57568}, // DYNAMIC_SEQRAM_492
    {0x4494, 2, 57568}, // DYNAMIC_SEQRAM_494
    {0x4496, 2, 57568}, // DYNAMIC_SEQRAM_496
    {0x4498, 2, 57568}, // DYNAMIC_SEQRAM_498
    {0x449A, 2, 57568}, // DYNAMIC_SEQRAM_49A
    {0x449C, 2, 57568}, // DYNAMIC_SEQRAM_49C
    {0x449E, 2, 57568}, // DYNAMIC_SEQRAM_49E
    {0x44A0, 2, 57568}, // DYNAMIC_SEQRAM_4A0
    {0x44A2, 2, 57568}, // DYNAMIC_SEQRAM_4A2
    {0x44A4, 2, 57568}, // DYNAMIC_SEQRAM_4A4
    {0x44A6, 2, 57568}, // DYNAMIC_SEQRAM_4A6
    {0x44A8, 2, 57568}, // DYNAMIC_SEQRAM_4A8
    {0x44AA, 2, 57568}, // DYNAMIC_SEQRAM_4AA
    {0x44AC, 2, 57568}, // DYNAMIC_SEQRAM_4AC
    {0x44AE, 2, 57568}, // DYNAMIC_SEQRAM_4AE
    {0x44B0, 2, 57568}, // DYNAMIC_SEQRAM_4B0
    {0x44B2, 2, 57568}, // DYNAMIC_SEQRAM_4B2
    {0x44B4, 2, 57568}, // DYNAMIC_SEQRAM_4B4

    {0x5500, 2, 0}, // AGAIN_LUT0
    {0x5502, 2, 2}, // AGAIN_LUT1
    {0x5504, 2, 6}, // AGAIN_LUT2
    {0x5506, 2, 9}, // AGAIN_LUT3
    {0x5508, 2, 15}, // AGAIN_LUT4
    {0x550A, 2, 16}, // AGAIN_LUT5
    {0x550C, 2, 17}, // AGAIN_LUT6
    {0x550E, 2, 18}, // AGAIN_LUT7
    {0x5510, 2, 25}, // AGAIN_LUT8
    {0x5512, 2, 32}, // AGAIN_LUT9
    {0x5514, 2, 33}, // AGAIN_LUT10
    {0x5516, 2, 35}, // AGAIN_LUT11
    {0x5518, 2, 38}, // AGAIN_LUT12
    {0x551A, 2, 43}, // AGAIN_LUT13
    {0x551C, 2, 47}, // AGAIN_LUT14
    {0x551E, 2, 48}, // AGAIN_LUT15
    {0x5400, 2, 256}, // GT1_COARSE0
    {0x5402, 2, 8454}, // GT1_COARSE1
    {0x5404, 2, 4353}, // GT1_COARSE2
    {0x5406, 2, 12550}, // GT1_COARSE3
    {0x5408, 2, 28928}, // GT1_COARSE4
    {0x540A, 2, 33031}, // GT1_COARSE5
    {0x540C, 2, 45313}, // GT1_COARSE6
    {0x540E, 2, 53505}, // GT1_COARSE7
    {0x5410, 2, 61742}, // GT1_COARSE8
    {0x5412, 2, 61714}, // GT1_COARSE9
    {0x5414, 2, 61828}, // GT1_COARSE10
    {0x5416, 2, 61988}, // GT1_COARSE11
    {0x5418, 2, 62214}, // GT1_COARSE12
    {0x541A, 2, 62534}, // GT1_COARSE13
    {0x541C, 2, 62985}, // GT1_COARSE14
    {0x541E, 2, 63623}, // GT1_COARSE15
    {0x5420, 2, 64523}, // GT1_COARSE16
    {0x5422, 2, 64523}, // GT1_COARSE17
    {0x5424, 2, 65530}, // GT1_DCG_ATTN_SET0
    {0x5426, 2, 21847}, // GT1_DCG_ATTN_SET1
    {0x5428, 2, 5}, // GT1_DCG_ATTN_SET2
    {0x542A, 2, 42320}, // GT1_ZONE_SET0
    {0x542C, 2, 43690}, // GT1_ZONE_SET1
    {0x542E, 2, 10}, // GT1_ZONE_SET2
    {0x5460, 2, 8809}, // ZT1_REG0_ADDR
    {0x5462, 2, 2951}, // ZT1_REG0_VALUE0
    {0x5464, 2, 2951}, // ZT1_REG0_VALUE1
    {0x5466, 2, 2435}, // ZT1_REG0_VALUE2
    {0x5498, 2, 8798}, // ZT1_REG7_ADDR
    {0x549A, 2, 48298}, // ZT1_REG7_VALUE0
    {0x549C, 2, 48298}, // ZT1_REG7_VALUE1
    {0x549E, 2, 48554}, // ZT1_REG7_VALUE2
    {0x3060, 2, 65281}, // GAIN_TABLE_CTRL

    {0x44BA, 2, 72}, // DAC_LD_4_5
    {0x44BC, 2, 48298}, // DAC_LD_6_7
    {0x44C0, 2, 16496}, // DAC_LD_10_11
    {0x44C4, 2, 1232}, // DAC_LD_14_15
    {0x44C6, 2, 6114}, // DAC_LD_16_17
    {0x44C8, 2, 59971}, // DAC_LD_18_19
    {0x44CA, 2, 14}, // DAC_LD_20_21
    {0x44CC, 2, 30583}, // DAC_LD_22_23
    {0x44CE, 2, 35748}, // DAC_LD_24_25
    {0x44D0, 2, 5941}, // DAC_LD_26_27
    {0x44D2, 2, 2951}, // DAC_LD_28_29
    {0x44D4, 2, 32768}, // DAC_LD_30_31
    {0x44D6, 2, 61958}, // DAC_LD_32_33
    {0x44D8, 2, 43770}, // DAC_LD_34_35
    {0x44DA, 2, 57345}, // DAC_LD_36_37
    {0x44DE, 2, 39868}, // DAC_LD_40_41
    {0x44E0, 2, 10300}, // DAC_LD_42_43
    {0x44E2, 2, 10273}, // DAC_LD_44_45
    {0x44E4, 2, 32768}, // DAC_LD_46_47
    {0x44E6, 2, 20543}, // DAC_LD_48_49
    {0x32A4, 2, 0}, // CRM_CTRL
    {0x333C, 2, 1}, // DYNAMIC_CTRL

    {0x0300, 2, 6}, // VT_PIX_CLK_DIV
    {0x0304, 2, 2}, // VT_PRE_PLL_CLK_DIV
    {0x0306, 2, 86}, //{0x0306, 2, 103}, // VT_PLL_MULTIPLIER

    {0x3980, 2, 1}, // PIX_DEF_CORR
    {0x3700, 2, 1}, // PIX_DEF_ID
    {0x3340, 2, 7264}, // OTPM_CTRL
    {0x3340, 2, 7264}, // OTPM_CTRL
    {0x36C0, 2, 1}, // DIGITAL_GAIN_CTRL

    {0x3062, 2, 0x002b}, // fenghuan add
    {0x36c4, 2, 0x1c0},
    {0x36c6, 2, 0x1c0},
    {0x0138, 1, 0x01}, // TEMPERATURE_ENABLE
    {0x0100, 1, 1}, // MODE_SELECT
    {0x44D6, 2, 45574}, // DAC_LD_32_33
};

#ifdef __cplusplus
}
#endif


#endif
