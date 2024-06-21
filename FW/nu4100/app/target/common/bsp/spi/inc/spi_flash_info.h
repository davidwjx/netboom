#ifndef BOOTFLASH_INFO_H
#define BOOTFLASH_INFO_H

#include "inu_types.h"

#ifndef SPI_NOR_MAX_ID_LEN
#define SPI_NOR_MAX_ID_LEN   6
#endif
#define BIT(nr)         (1UL << (nr))
struct flash_info {
	char		*name;

	/*
	 * This array stores the ID bytes.
	 * The first three bytes are the JEDIC ID.
	 * JEDEC ID zero means "no ID" (mostly older chips).
	 */
	UINT8		id[SPI_NOR_MAX_ID_LEN];
	UINT8		id_len;

	/* The size listed here is what works with SPINOR_OP_SE, which isn't
	 * necessarily called a "sector" by the vendor.
	 */
	UINT32  	sector_size;
	UINT16		n_sectors;

	UINT16		page_size;
	UINT16		addr_width;

	UINT16		flags;
#define	SECT_4K			   0x01	/* SPINOR_OP_BE_4K works uniformly */
#define	SPI_NOR_NO_ERASE	0x02	/* No erase command needed */
#define	SST_WRITE		   0x04	/* use SST byte programming */
#define	SPI_NOR_NO_FR		0x08	/* Can't do fastread */
#define	SECT_4K_PMC		   0x10	/* SPINOR_OP_BE_4K_PMC works uniformly */
#define	SPI_NOR_DUAL_READ	0x20    /* Flash supports Dual Read */
#define	SPI_NOR_QUAD_READ	0x40    /* Flash supports Quad Read */
#define	USE_FSR			   0x80	/* use flag status register */
#define SPI_NOR_HAS_LOCK   BIT(8)   /* Flash supports lock/unlock via SR */
#define SPI_NOR_HAS_TB     BIT(9)
#define SPI_NOR_4B_OPCODES	BIT(11)
};


/* Used when the "_ext_id" is two bytes at most */
#define INFO(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		.id = {							\
			((_jedec_id) >> 16) & 0xff,			\
			((_jedec_id) >> 8) & 0xff,			\
			(_jedec_id) & 0xff,				\
			((_ext_id) >> 8) & 0xff,			\
			(_ext_id) & 0xff,				\
			},						\
		.id_len = (!(_jedec_id) ? 0 : (3 + ((_ext_id) ? 2 : 0))),	\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),

#define INFO6(_jedec_id, _ext_id, _sector_size, _n_sectors, _flags)	\
		.id = {							\
			((_jedec_id) >> 16) & 0xff,			\
			((_jedec_id) >> 8) & 0xff,			\
			(_jedec_id) & 0xff,				\
			((_ext_id) >> 16) & 0xff,			\
			((_ext_id) >> 8) & 0xff,			\
			(_ext_id) & 0xff,				\
			},						\
		.id_len = 6,						\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = 256,					\
		.flags = (_flags),

#define CAT25_INFO(_sector_size, _n_sectors, _page_size, _addr_width, _flags)	\
		.sector_size = (_sector_size),				\
		.n_sectors = (_n_sectors),				\
		.page_size = (_page_size),				\
		.addr_width = (_addr_width),				\
		.flags = (_flags),



#endif

