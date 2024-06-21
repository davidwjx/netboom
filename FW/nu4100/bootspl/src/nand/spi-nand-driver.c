
#include "debug.h"
//#include "inu_spl_info.h"
#include "spi-nand-driver.h"
//#include "inu_storage_layout.h"



struct spi_nand_chip *chip_ptr = NULL;
//unsigned int *TEMP = NULL;

int spinand_init(void)
{
	
	int ret = 0;
	ret = spi_nand_init(&chip_ptr);

	if(ret)
	{
		debug_printf("Nand initialization FAILED!!!!\n");
	}
	else
	{
		debug_printf("Nand initialized!!!!  PtrV %x \n", (uint32_t)chip_ptr);
	}

	return ret;
}

int spinand_get_block_size()
{
	if(chip_ptr == NULL)
		return 0;

	return chip_ptr->block_size;
}

int spinand_read(spinand_io_param_t *io)
{
	debug_printf("spinand_read:io->offset %x, io->length %x, io->buf %x\n",io->offset, io->length, io->buf);
	return spi_nand_read(chip_ptr,io->offset, io->length, &io->ret_len, io->buf);
}

int spinand_write(spinand_io_param_t *io)
{
	//debug_printf("spinand_write:io->offset %x, io->length %x\n",io->offset, io->length);
	return spi_nand_write(chip_ptr, io->offset, io->length, &io->ret_len, io->buf);
}

int spinand_erase(spinand_io_param_t *io)
{
	//debug_printf("spinand_erase:io->offset %x, io->length %x\n",io->offset, io->length);
	return spi_nand_erase(chip_ptr, io->offset, io->length);
}




