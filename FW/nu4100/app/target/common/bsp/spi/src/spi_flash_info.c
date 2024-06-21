#include "spi_flash_info.h"
/* NOTE: double check command sets and memory organization when you add
 * more nor chips.  This current list focusses on newer chips, which
 * have been converging on command sets which including JEDEC ID.
 *
 * All newly added entries should describe *hardware* and should use SECT_4K
 * (or SECT_4K_PMC) if hardware supports erasing 4 KiB sectors. For usage
 * scenarios excluding small sectors there is config option that can be
 * disabled: CONFIG_MTD_SPI_NOR_USE_4K_SECTORS.
 * For historical (and compatibility) reasons (before we got above config) some
 * old entries may be missing 4K flag.
 */
const struct flash_info spi_nor_ids[] = {
    /* Atmel -- some are (confusingly) marketed as "DataFlash" */
    {"at25fs010",  INFO(0x1f6601, 0, 32 * 1024,   4, SECT_4K)},
    {"at25fs040",  INFO(0x1f6604, 0, 64 * 1024,   8, SECT_4K)},

    {"at25df041a", INFO(0x1f4401, 0, 64 * 1024,   8, SECT_4K)},
    {"at25df321a", INFO(0x1f4701, 0, 64 * 1024,  64, SECT_4K)},
    {"at25df641",  INFO(0x1f4800, 0, 64 * 1024, 128, SECT_4K)},

    {"at26f004",   INFO(0x1f0400, 0, 64 * 1024,  8, SECT_4K)},
    {"at26df081a", INFO(0x1f4501, 0, 64 * 1024, 16, SECT_4K)},
    {"at26df161a", INFO(0x1f4601, 0, 64 * 1024, 32, SECT_4K)},
    {"at26df321",  INFO(0x1f4700, 0, 64 * 1024, 64, SECT_4K)},

    {"at45db081d", INFO(0x1f2500, 0, 64 * 1024, 16, SECT_4K)},

    /* EON -- en25xxx */
    {"en25f32",    INFO(0x1c3116, 0, 64 * 1024,   64, SECT_4K)},
    {"en25p32",    INFO(0x1c2016, 0, 64 * 1024,   64, 0)},
    {"en25q32b",   INFO(0x1c3016, 0, 64 * 1024,   64, 0)},
    {"en25p64",    INFO(0x1c2017, 0, 64 * 1024,  128, 0)},
    {"en25q64",    INFO(0x1c3017, 0, 64 * 1024,  128, SECT_4K)},
    {"en25qh128",  INFO(0x1c7018, 0, 64 * 1024,  256, 0)},
    {"en25qh256",  INFO(0x1c7019, 0, 64 * 1024,  512, 0)},
    {"en25s64",   INFO(0x1c3817, 0, 64 * 1024,  128, SECT_4K)},

    /* ESMT */
    {"f25l32pa", INFO(0x8c2016, 0, 64 * 1024, 64, SECT_4K)},

    /* Everspin */
    {"mr25h256", CAT25_INFO( 32 * 1024, 1, 256, 2, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},
    {"mr25h10",  CAT25_INFO(128 * 1024, 1, 256, 3, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},

    /* Fujitsu */
    {"mb85rs1mt", INFO(0x047f27, 0, 128 * 1024, 1, SPI_NOR_NO_ERASE)},

    /* GigaDevice */
    {"gd25q32", INFO(0xc84016, 0, 64 * 1024,  64, SECT_4K)},
    {"gd25q64", INFO(0xc84017, 0, 64 * 1024, 128, SECT_4K)},
    {"gd25q128", INFO(0xc84018, 0, 64 * 1024, 256, SECT_4K)},

    /* Intel/Numonyx -- xxxs33b */
    {"160s33b",  INFO(0x898911, 0, 64 * 1024,  32, 0)},
    {"320s33b",  INFO(0x898912, 0, 64 * 1024,  64, 0)},
    {"640s33b",  INFO(0x898913, 0, 64 * 1024, 128, 0)},

    /* ISSI */
    {"is25cd512", INFO(0x7f9d20, 0, 32 * 1024,   2, SECT_4K)},

    /* Macronix */
    {"mx25l512e",   INFO(0xc22010, 0, 64 * 1024,   1, SECT_4K)},
    {"mx25l2005a",  INFO(0xc22012, 0, 64 * 1024,   4, SECT_4K)},
    {"mx25l4005a",  INFO(0xc22013, 0, 64 * 1024,   8, SECT_4K)},
    {"mx25l8005",   INFO(0xc22014, 0, 64 * 1024,  16, 0)},
    {"mx25l1606e",  INFO(0xc22015, 0, 64 * 1024,  32, SECT_4K)},
    {"mx25l3205d",  INFO(0xc22016, 0, 64 * 1024,  64, 0)},
    {"mx25l3255e",  INFO(0xc29e16, 0, 64 * 1024,  64, SECT_4K)},
    {"mx25l6405d",  INFO(0xc22017, 0, 64 * 1024, 128, 0)},
    {"mx25u6435f",  INFO(0xc22537, 0, 64 * 1024, 128, SECT_4K)},
     {"mx25u12873f", INFO(0xc22538, 0, 64 * 1024, 512, SECT_4K)},
    {"mx25l12805d", INFO(0xc22018, 0, 64 * 1024, 256, 0)},
    {"mx25l12855e", INFO(0xc22618, 0, 64 * 1024, 256, 0)},
    {"mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, 0)},
    {"mx25l25655e", INFO(0xc22619, 0, 64 * 1024, 512, 0)},
    {"mx66l51235l", INFO(0xc2201a, 0, 64 * 1024, 1024, SPI_NOR_QUAD_READ)},
	{ "mx66u51235f", INFO(0xc2253a, 0, 64 * 1024, 1024, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES)},
	{ "mx66l1g45g",  INFO(0xc2201b, 0, 64 * 1024, 2048, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"mx66l1g55g",  INFO(0xc2261b, 0, 64 * 1024, 2048, SPI_NOR_QUAD_READ)},

    /* Micron */
    {"n25q032",    INFO(0x20ba16, 0, 64 * 1024,   64, SPI_NOR_QUAD_READ)},
    {"n25q032a",    INFO(0x20bb16, 0, 64 * 1024,   64, SPI_NOR_QUAD_READ)},
    {"n25q064",     INFO(0x20ba17, 0, 64 * 1024,  128, SECT_4K | SPI_NOR_QUAD_READ)},
    {"n25q064a",    INFO(0x20bb17, 0, 64 * 1024,  128, SECT_4K | SPI_NOR_QUAD_READ)},
    {"n25q128a11",  INFO(0x20bb18, 0, 64 * 1024,  256, SPI_NOR_QUAD_READ)},
    {"n25q128a13",  INFO(0x20ba18, 0, 64 * 1024,  256, SPI_NOR_QUAD_READ)},
    {"n25q256a",    INFO(0x20ba19, 0, 64 * 1024,  512, SECT_4K | SPI_NOR_QUAD_READ)},
    {"n25q512a",    INFO(0x20bb20, 0, 64 * 1024, 1024, SECT_4K | USE_FSR | SPI_NOR_QUAD_READ)},
    {"n25q512ax3",  INFO(0x20ba20, 0, 64 * 1024, 1024, SECT_4K | USE_FSR | SPI_NOR_QUAD_READ)},
    {"n25q00",      INFO(0x20ba21, 0, 64 * 1024, 2048, SECT_4K | USE_FSR | SPI_NOR_QUAD_READ)},

    /* PMC */
    {"pm25lv512",   INFO(0,        0, 32 * 1024,    2, SECT_4K_PMC)},
    {"pm25lv010",   INFO(0,        0, 32 * 1024,    4, SECT_4K_PMC)},
    {"pm25lq032",   INFO(0x7f9d46, 0, 64 * 1024,   64, SECT_4K)},

    /* Spansion -- single (large) sector size only, at least
     * for the chips listed here (without boot sectors).
     */
    {"s25sl032p",  INFO(0x010215, 0x4d00,  64 * 1024,  64, SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25sl064p",  INFO(0x010216, 0x4d00,  64 * 1024, 128, SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25fl256s0", INFO(0x010219, 0x4d00, 256 * 1024, 128, 0)},
    {"s25fl256s1", INFO(0x010219, 0x4d01,  64 * 1024, 512, SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25fl512s",  INFO(0x010220, 0x4d00, 256 * 1024, 256, SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s70fl01gs",  INFO(0x010221, 0x4d00, 256 * 1024, 256, 0)},
    {"s25sl12800", INFO(0x012018, 0x0300, 256 * 1024,  64, 0)},
    {"s25sl12801", INFO(0x012018, 0x0301,  64 * 1024, 256, 0)},
    {"s25fl128s",   INFO6(0x012018, 0x4d0180, 64 * 1024, 256, SECT_4K | SPI_NOR_QUAD_READ)},
    {"s25fl129p0", INFO(0x012018, 0x4d00, 256 * 1024,  64, SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"S25fl128p",   INFO6(0x012018, 0x4d0181,   64 * 1024, 256, 0)},
    {"s25fl129p1", INFO(0x012018, 0x4d01,  64 * 1024, 256, SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25sl004a",  INFO(0x010212,      0,  64 * 1024,   8, 0)},
    {"s25sl008a",  INFO(0x010213,      0,  64 * 1024,  16, 0)},
    {"s25sl016a",  INFO(0x010214,      0,  64 * 1024,  32, 0)},
    {"s25sl032a",  INFO(0x010215,      0,  64 * 1024,  64, 0)},
    {"s25sl064a",  INFO(0x010216,      0,  64 * 1024, 128, 0)},
    {"s25fl004k",  INFO(0xef4013,      0,  64 * 1024,   8, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25fl008k",  INFO(0xef4014,      0,  64 * 1024,  16, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25fl016k",  INFO(0xef4015,      0,  64 * 1024,  32, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"s25fl064k",  INFO(0xef4017,      0,  64 * 1024, 128, SECT_4K)},
    {"s25fl132k",  INFO(0x014016,      0,  64 * 1024,  64, SECT_4K)},
    {"s25fl164k",  INFO(0x014017,      0,  64 * 1024, 128, SECT_4K)},
    {"s25fl204k",  INFO(0x014013,      0,  64 * 1024,   8, SECT_4K | SPI_NOR_DUAL_READ)},

    /* SST -- large erase sizes are "overlays", "sectors" are 4K */
    {"sst25vf040b", INFO(0xbf258d, 0, 64 * 1024,  8, SECT_4K | SST_WRITE)},
    {"sst25vf080b", INFO(0xbf258e, 0, 64 * 1024, 16, SECT_4K | SST_WRITE)},
    {"sst25vf016b", INFO(0xbf2541, 0, 64 * 1024, 32, SECT_4K | SST_WRITE)},
    {"sst25vf032b", INFO(0xbf254a, 0, 64 * 1024, 64, SECT_4K | SST_WRITE)},
    {"sst25vf064c", INFO(0xbf254b, 0, 64 * 1024, 128, SECT_4K)},
    {"sst25wf512",  INFO(0xbf2501, 0, 64 * 1024,  1, SECT_4K | SST_WRITE)},
    {"sst25wf010",  INFO(0xbf2502, 0, 64 * 1024,  2, SECT_4K | SST_WRITE)},
    {"sst25wf020",  INFO(0xbf2503, 0, 64 * 1024,  4, SECT_4K | SST_WRITE)},
    {"sst25wf020a", INFO(0x621612, 0, 64 * 1024,  4, SECT_4K)},
    {"sst25wf040b", INFO(0x621613, 0, 64 * 1024,  8, SECT_4K)},
    {"sst25wf040",  INFO(0xbf2504, 0, 64 * 1024,  8, SECT_4K | SST_WRITE)},
    {"sst25wf080",  INFO(0xbf2505, 0, 64 * 1024, 16, SECT_4K | SST_WRITE)},

    /* ST Microelectronics -- newer production may have feature updates */
    {"m25p05",  INFO(0x202010,  0,  32 * 1024,   2, 0)},
    {"m25p10",  INFO(0x202011,  0,  32 * 1024,   4, 0)},
    {"m25p20",  INFO(0x202012,  0,  64 * 1024,   4, 0)},
    {"m25p40",  INFO(0x202013,  0,  64 * 1024,   8, 0)},
    {"m25p80",  INFO(0x202014,  0,  64 * 1024,  16, 0)},
    {"m25p16",  INFO(0x202015,  0,  64 * 1024,  32, 0)},
    {"m25p32",  INFO(0x202016,  0,  64 * 1024,  64, 0)},
    {"m25p64",  INFO(0x202017,  0,  64 * 1024, 128, 0)},
    {"m25p128", INFO(0x202018,  0, 256 * 1024,  64, 0)},

    {"m25p05-nonjedec",  INFO(0, 0,  32 * 1024,   2, 0)},
    {"m25p10-nonjedec",  INFO(0, 0,  32 * 1024,   4, 0)},
    {"m25p20-nonjedec",  INFO(0, 0,  64 * 1024,   4, 0)},
    {"m25p40-nonjedec",  INFO(0, 0,  64 * 1024,   8, 0)},
    {"m25p80-nonjedec",  INFO(0, 0,  64 * 1024,  16, 0)},
    {"m25p16-nonjedec",  INFO(0, 0,  64 * 1024,  32, 0)},
    {"m25p32-nonjedec",  INFO(0, 0,  64 * 1024,  64, 0)},
    {"m25p64-nonjedec",  INFO(0, 0,  64 * 1024, 128, 0)},
    {"m25p128-nonjedec", INFO(0, 0, 256 * 1024,  64, 0)},

    {"m45pe10", INFO(0x204011,  0, 64 * 1024,    2, 0)},
    {"m45pe80", INFO(0x204014,  0, 64 * 1024,   16, 0)},
    {"m45pe16", INFO(0x204015,  0, 64 * 1024,   32, 0)},

    {"m25pe20", INFO(0x208012,  0, 64 * 1024,  4,       0)},
    {"m25pe80", INFO(0x208014,  0, 64 * 1024, 16,       0)},
    {"m25pe16", INFO(0x208015,  0, 64 * 1024, 32, SECT_4K)},

    {"m25px16",    INFO(0x207115,  0, 64 * 1024, 32, SECT_4K)},
    {"m25px32",    INFO(0x207116,  0, 64 * 1024, 64, SECT_4K)},
    {"m25px32-s0", INFO(0x207316,  0, 64 * 1024, 64, SECT_4K)},
    {"m25px32-s1", INFO(0x206316,  0, 64 * 1024, 64, SECT_4K)},
    {"m25px64",    INFO(0x207117,  0, 64 * 1024, 128, 0)},
    {"m25px80",    INFO(0x207114,  0, 64 * 1024, 16, 0)},

    /* Winbond -- w25x "blocks" are 64K, "sectors" are 4KiB */
    {"w25x05", INFO(0xef3010, 0, 64 * 1024,  1,  SECT_4K)},
    {"w25x10", INFO(0xef3011, 0, 64 * 1024,  2,  SECT_4K)},
    {"w25x20", INFO(0xef3012, 0, 64 * 1024,  4,  SECT_4K)},
    {"w25x40", INFO(0xef3013, 0, 64 * 1024,  8,  SECT_4K)},
    {"w25x80", INFO(0xef3014, 0, 64 * 1024,  16, SECT_4K)},
    {"w25x16", INFO(0xef3015, 0, 64 * 1024,  32, SECT_4K)},
    {"w25x32", INFO(0xef3016, 0, 64 * 1024,  64, SECT_4K)},
    {"w25q32", INFO(0xef4016, 0, 64 * 1024,  64, SECT_4K)},
    {"w25x64", INFO(0xef3017, 0, 64 * 1024, 128, SECT_4K)},
    {"w25q64", INFO(0xef4017, 0, 64 * 1024, 128, SECT_4K)},
    {"w25q16dw", INFO(0xef6015, 0, 64 * 1024,  32, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB)},
    {"w25q32dw", INFO(0xef6016, 0, 64 * 1024,  64, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"w25q64dw", INFO(0xef6017, 0, 64 * 1024, 128, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"w25q128fw", INFO(0xef6018, 0, 64 * 1024, 256, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ)},
    {"w25q256jw", INFO(0xef6019, 0, 64 * 1024, 512, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES)},
    {"w25q512nw", INFO(0xef6020, 0, 64 * 1024, 1024, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES)},
    {"w25q80", INFO(0xef5014, 0, 64 * 1024,  16, SECT_4K)},
    {"w25q80bl", INFO(0xef4014, 0, 64 * 1024,  16, SECT_4K)},
    {"w25q128", INFO(0xef4018, 0, 64 * 1024, 256, SECT_4K)},
    {"w25q256", INFO(0xef4019, 0, 64 * 1024, 512, SECT_4K)},

    /* Catalyst / On Semiconductor -- non-JEDEC */
    {"cat25c11", CAT25_INFO(  16, 8, 16, 1, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},
    {"cat25c03", CAT25_INFO(  32, 8, 16, 2, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},
    {"cat25c09", CAT25_INFO( 128, 8, 32, 2, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},
    {"cat25c17", CAT25_INFO( 256, 8, 32, 2, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},
    {"cat25128", CAT25_INFO(2048, 8, 64, 2, SPI_NOR_NO_ERASE | SPI_NOR_NO_FR)},
    {"",INFO(0,0,0,0,0)}
};

UINT32 spi_nor_ids_arr_size=sizeof(spi_nor_ids)/sizeof(struct flash_info);




