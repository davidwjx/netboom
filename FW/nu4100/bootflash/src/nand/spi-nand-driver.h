
#ifndef __SPI_NAND_DRIVER_H__
#define __SPI_NAND_DRIVER_H__

#include <string.h>
#include "spi-nand.h"
#include "errno.h"
#include "external.h"
#include "types.h"


typedef struct
{
	u8* buf;
	loff_t offset;
	size_t length;
	size_t *ret_len;
}spinand_io_param_t;


int spinand_init(void);

int spinand_read(spinand_io_param_t *io);

int spinand_write(spinand_io_param_t *io);

int spinand_erase(spinand_io_param_t *io);



#endif	//__SPI_NAND_DRIVER_H__

