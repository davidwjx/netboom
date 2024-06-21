#include "usb_drv.h"
#include "spi_flash.h"
#include "flash_info.h"
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

	/* Macronix */
	{ "mx25l512e",   INFO(0xc22010, 0, 64 * 1024,   1, SECT_4K) },
	{ "mx25l2005a",  INFO(0xc22012, 0, 64 * 1024,   4, SECT_4K) },
	{ "mx25l4005a",  INFO(0xc22013, 0, 64 * 1024,   8, SECT_4K) },
	{ "mx25l8005",   INFO(0xc22014, 0, 64 * 1024,  16, 0) },
	{ "mx25l1606e",  INFO(0xc22015, 0, 64 * 1024,  32, SECT_4K) },
	{ "mx25l3205d",  INFO(0xc22016, 0, 64 * 1024,  64, 0) },
	{ "mx25l3255e",  INFO(0xc29e16, 0, 64 * 1024,  64, SECT_4K) },
	{ "mx25l6405d",  INFO(0xc22017, 0, 64 * 1024, 128, 0) },
	{ "mx25u6435f",  INFO(0xc22537, 0, 64 * 1024, 128, SECT_4K) },
	{ "mx25u12873f", INFO(0xc22538, 0, 64 * 1024, 512, SECT_4K) },
	{ "mx25l12805d", INFO(0xc22018, 0, 64 * 1024, 256, 0) },
	{ "mx25l12855e", INFO(0xc22618, 0, 64 * 1024, 256, 0) },
	{ "mx25l25635e", INFO(0xc22019, 0, 64 * 1024, 512, 0) },
	{ "mx25l25655e", INFO(0xc22619, 0, 64 * 1024, 512, 0) },
	{ "mx66l51235l", INFO(0xc2201a, 0, 64 * 1024, 1024, SPI_NOR_QUAD_READ) },
	{ "mx66u51235f", INFO(0xc2253a, 0, 64 * 1024, 1024, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES) },
	{ "mx66l1g45g",  INFO(0xc2201b, 0, 64 * 1024, 2048, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
	{ "mx66l1g55g",  INFO(0xc2261b, 0, 64 * 1024, 2048, SPI_NOR_QUAD_READ) },

   { "w25q16dw", INFO(0xef6015, 0, 64 * 1024,  32, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ | SPI_NOR_HAS_LOCK | SPI_NOR_HAS_TB)},
   { "w25q32dw", INFO(0xef6016, 0, 64 * 1024,  64, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
   { "w25q64dw", INFO(0xef6017, 0, 64 * 1024, 128, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
   { "w25q128fw", INFO(0xef6018, 0, 64 * 1024, 256, SECT_4K | SPI_NOR_DUAL_READ | SPI_NOR_QUAD_READ) },
   { "w25q256jw", INFO(0xef6019, 0, 64 * 1024, 512, SECT_4K | SPI_NOR_DUAL_READ |SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES) },
   { "w25q512nw", INFO(0xef6020, 0, 64 * 1024, 1024, SECT_4K | SPI_NOR_DUAL_READ |SPI_NOR_QUAD_READ | SPI_NOR_4B_OPCODES) },
   { "gd25lq16", INFO(0xc86015, 0, 64 * 1024, 32, SECT_4K | SPI_NOR_DUAL_READ |  SPI_NOR_QUAD_READ) },
   { },
};

UINT32 spi_nor_ids_arr_size=ARRAY_SIZE(spi_nor_ids);




