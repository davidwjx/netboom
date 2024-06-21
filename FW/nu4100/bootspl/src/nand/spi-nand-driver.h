
#ifndef __SPI_NAND_DRIVER_H__
#define __SPI_NAND_DRIVER_H__

#include <string.h>
#include "spi-nand.h"
#include "errno.h"
#include "external.h"
#include "types.h"


#if 0
typedef struct
{
	u8* buf;
	u32 offset;
	u32 length;
	u32 *ret_len;
}spinand_io_param_t;

#else
typedef struct
{
	u8* buf;
	loff_t offset;
	size_t length;
	size_t ret_len;
}spinand_io_param_t;
#endif

int spinand_init(void);

int spinand_get_block_size();

int spinand_read(spinand_io_param_t *io);

int spinand_write(spinand_io_param_t *io);

int spinand_erase(spinand_io_param_t *io);



#endif	//__SPI_NAND_DRIVER_H__

