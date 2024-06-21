/*
 *
 * board.c
 *
 * c-implementation of chip initializations
 */

#include "common.h"
#include "sys_calls.h"
#include "os_defs.h"
#include "spi_flash.h"
#include "inu_storage_layout.h"
#include "inu_storage.h"
#include "flash_info.h"
#include "gme.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_gpio_regs.h"
#include "storage.h"
#include "inu_boot_common.h"


/**********************************************
 * global data
***********************************************/
InuProductionHeaderT productionHdrG;

/**********************************************
 * local data
***********************************************/

InuSectionHeaderT inuSectionHeader;
InuBootfixHeaderT bootfixMetadata;
FlashInfoT flashInfo;

extern UINT32 spi_nor_ids_arr_size;
extern const struct flash_info spi_nor_ids[];

unsigned int flashFlags = 0;

static int find_block_address(InuStorageSectionTypeE sectionType, unsigned int *pBlockaddress)
{
	int ret = -1;
	InuSectionHeaderT inuSectionHeader;
	unsigned int blockAddress;
	unsigned int sectionBootsplStartBlock = SECTION_BOOTSPL_START_BLOCK;

	if (flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP)
	{
		sectionBootsplStartBlock = flashInfo.numOfBlocks / 2;
		rel_log("flash (bootfix): !!!!!! %s(): Boot from BACKUP PARTITION !!!!!!\n", __func__);
	}
	else
	{
		rel_log("flash (bootfix): !!!!!! %s(): Boot from ACTIVE PARTITION !!!!!!\n", __func__);
	}
	rel_log("flash (bootfix): %s(): flashFlags = %x, sectionBootsplStartBlock = %x\n", __func__, flashFlags, sectionBootsplStartBlock);
	switch (sectionType)
	{

	case INU_STORAGE_SECTION_BOOTSPL:
		*pBlockaddress = sectionBootsplStartBlock * flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_DTB:
		blockAddress = sectionBootsplStartBlock * flashInfo.blockSize;
		ret = SPI_FLASHG_Read(blockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
		if (ret)
			return -1;
		*pBlockaddress = blockAddress + inuSectionHeader.sectionSize*flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_KERNEL:
		blockAddress = sectionBootsplStartBlock * flashInfo.blockSize;
		ret = SPI_FLASHG_Read(blockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
		if (ret)
			return -1;
		blockAddress += inuSectionHeader.sectionSize * flashInfo.blockSize;
		ret = SPI_FLASHG_Read(blockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
		if (ret)
			return -1;
		*pBlockaddress = blockAddress + inuSectionHeader.sectionSize*flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_APP:
		blockAddress = sectionBootsplStartBlock * flashInfo.blockSize;
		ret = SPI_FLASHG_Read(blockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
		if (ret)
			return -1;
		blockAddress += inuSectionHeader.sectionSize * flashInfo.blockSize;
		ret = SPI_FLASHG_Read(blockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
		if (ret)
			return -1;
		blockAddress += inuSectionHeader.sectionSize * flashInfo.blockSize;
		ret = SPI_FLASHG_Read(blockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
		if (ret)
			return -1;
		*pBlockaddress = blockAddress + inuSectionHeader.sectionSize * flashInfo.blockSize;
		break;
	default:
		ret = -1;
		break;

	}
	if ((*pBlockaddress/ flashInfo.blockSize) > (flashInfo.numOfBlocks - 1)) {
		rel_log("flash (bootfix): error, section exceeds flash size\n");
		return -1;
	}

	return 0;
}

void read_production_data()
{
	uint32 ret=0;
	unsigned char *prod_data_address = (unsigned char *)(BOOTDETECT_PRODUCTION_DATA_ADDRESS);
	unsigned int flash_offset = 0;
	InuSectionHeaderT  sectoinHeader;
	// use spiflash_read to the production data and store to @BOOTDETECT_PRODUCTION_DATA_ADDRESS + 4, keep valid magic at @BOOTDETECT_PRODUCTION_DATA_ADDRESS
	// mark layout invalid by default
	*((uint32 *)prod_data_address)=0;

	flash_offset = SECTION_PRODUCTION_START_BLOCK*flashInfo.blockSize;
	ret = SPI_FLASHG_Read(flash_offset, sizeof(InuSectionHeaderT), (unsigned char *) &sectoinHeader);
	if (ret==0)
	{
		if (sectoinHeader.magicNumber == SECTION_PRODUCTION_MAGIC_NUMBER)
		{
			ret = SPI_FLASHG_Read(flash_offset+sizeof(InuSectionHeaderT), sizeof(InuProductionHeaderT), prod_data_address+sizeof(UINT32));
			if (ret==0)
			{
				*((uint32 *)prod_data_address)=BOOTROM_STORAGE_NU3000_MAGIC_NUMBER;
				rel_log("flash (bootfix): production data valid, bootId=%d\n",((InuProductionHeaderT *)(prod_data_address+sizeof(UINT32)))->bootId);
			}
		}
		else ret = 1;
	}

	if (ret == 0)
		memcpy(&productionHdrG,prod_data_address+sizeof(UINT32),sizeof(InuProductionHeaderT));
	else memset(&productionHdrG,0,sizeof(productionHdrG));
	GME_SAVE_AND_RESTORE_1_VAL = BOOTDETECT_STATUS_FINISH;
	return;
}

/**********************************************
 * global functions
***********************************************/

void storage_init()
{
	UINT32 retVal, tmp, ind, ret;
	UINT8 id[SPI_NOR_MAX_ID_LEN];
	const struct flash_info *info;

	flashInfo.blockSize = 64*1024;
	flashInfo.numOfBlocks = 128;
	flashInfo.spi_flags = 0;

	ret = SPI_FLASHG_init(0, 0, (get_board_info()->sys_clk_freq_hz/get_board_info()->spi_clk_div)/2);
	if (ret) {
		rel_log("flash (bootfix): can't init flash\n");
		return;
	}

	retVal = SPI_FLASHG_read_reg(SPINOR_OP_RDID, id, SPI_NOR_MAX_ID_LEN);

	if (!retVal)
	{
		retVal=1;
		for (tmp = 0; tmp < spi_nor_ids_arr_size - 1; tmp++)
		{
			info = &spi_nor_ids[tmp];
			if (info->id_len)
			{
				for (ind = 0; ind < info->id_len; ind++)
				{
					if (info->id[ind] != id[ind])
						break;

				}
				if (ind == info->id_len)
				{
					flashInfo.blockSize = info->sector_size;
					flashInfo.numOfBlocks = info->n_sectors;
					flashInfo.spi_flags = info->flags;
					flashInfo.spi_flags &= ~SPI_NOR_QUAD_READ;//will be update from flash metadata
					flashInfo.spi_flags &= ~SPI_NOR_DUAL_READ;//will be update from flash metadata

					rel_log("flash (bootfix): found model %s, blockSize = %x, spi_flags = %x, numOfBlocks = %d\n",
							info->name, flashInfo.blockSize, flashInfo.spi_flags, flashInfo.numOfBlocks);
					retVal=0;
				}

			}
		}
	}
	if (retVal)
		rel_log("flash (bootfix): flash model unknown, using defaults , blockSize=0x%x, numOfBlocks=%d\n",flashInfo.blockSize, flashInfo.numOfBlocks);

	// read production data into CRAM, will be used for bootdetect purpose
	read_production_data();
	 return;
}


int storage_get_section_data(InuStorageSectionTypeE sectionType, unsigned char *buffer)
{

	unsigned int flash_offset = 0;
	int ret,origSize;

	if (flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_FATAL)
	{
		abort_log("flash (bootfix): FATAL flash flag is present. Aborting.");
		return -1;
	}

	if (find_block_address(sectionType,&flash_offset) != 0)
		return -1;
	ret = SPI_FLASHG_Read(flash_offset, sizeof(InuSectionHeaderT), (uint8 *) &inuSectionHeader);
	if (ret)
	{
		abort_log("flsh (bootfix): can't read from offset=%x\n",flash_offset);
		return -1;
	}
	rel_log("flash (bootfix): %s(): flash_offset = %x, sectionDataSize = %x\n", __func__, flash_offset, inuSectionHeader.sectionDataSize);
	flash_offset += sizeof(InuSectionHeaderT);
	origSize = inuSectionHeader.sectionDataSize;
	if ((inuSectionHeader.sectionDataSize%4) != 0)
		inuSectionHeader.sectionDataSize += (4 - (inuSectionHeader.sectionDataSize%4));
	ret = SPI_FLASHG_Read(flash_offset, inuSectionHeader.sectionDataSize, buffer);
	if (ret)
	{
		abort_log("flash (bootfix): can't read from offset=%x ",flash_offset);
		return -1;
	}
	return origSize;
}

int storage_check_boot_from_flash(void)
{
	int ret;
	unsigned int mux_ctrl_0_5, push_button_override = 1, gpio_ddr, gpio_ddr_reset;
	unsigned int startOffset;
	int isBootfromFlash = 0, tim;

	startOffset = SECTION_BOOTFIX_META_START_BLOCK * flashInfo.blockSize;
	ret = SPI_FLASHG_Read(startOffset, sizeof(InuSectionHeaderT), (uint8 *) &inuSectionHeader);
	//rel_log("flash (bootfix): %s(): Read InuSectionHeaderT startOffset = %x, ret = %x\n", __func__, startOffset, ret);
	if (!ret)
	{
		//rel_log("flash (bootfix): %s(): magicNumber = %x, SECTION_BOOTFIX_META_MAGIC_NUMBER = %x\n", __func__, inuSectionHeader.magicNumber, SECTION_BOOTFIX_META_MAGIC_NUMBER);
		if (inuSectionHeader.magicNumber == SECTION_BOOTFIX_META_MAGIC_NUMBER)
		{
			//rel_log("flash (bootfix): %s(): Read InuBootfixHeaderT ret = %x\n", __func__, ret);
			ret = SPI_FLASHG_Read(startOffset + sizeof(InuSectionHeaderT),sizeof(InuBootfixHeaderT), (uint8 *) &bootfixMetadata);
			if (!ret)
			{
				isBootfromFlash = bootfixMetadata.isBootfromFlash;
				flashFlags      = bootfixMetadata.flashFlags;
				rel_log("flash (bootfix): %s(): isBootfromFlash = %x, flashFlags = %x\n", __func__, isBootfromFlash, flashFlags);
			}
		}
	}

#define OVERRIDE_BUTTON
#ifdef OVERRIDE_BUTTON
	/*
	 We will use I2C0_SD to check if we boot from Flash or ROM.
	 There is always a pullup on the main board for this ball.
	 When there is no external MANO board connected you will read "1"
	 When there is MANO attached, the I2C0_SD can be set to "0", which will allow to prevent boot from flash
	 The same ball is routed to the GPIO0
	 */
	gpio_ddr_reset = GPIO_GPIO_SWPORTA_DDR_GPIO_SWPORTA_DDR_R;
	gpio_ddr = (gpio_ddr_reset & 0xFFFFFFFE); //make sure bit 0 (gpio0) is set to 0, to make it as input
	GPIO_GPIO_SWPORTA_DDR_GPIO_SWPORTA_DDR_W(gpio_ddr);

	mux_ctrl_0_5=GME_IO_MUX_CTRL_0_IO_MUX_CTRL_5_R;
	GME_IO_MUX_CTRL_0_IO_MUX_CTRL_5_W(1);
	/* add small delay to allow the GPIO HW to detect the change and set the register */
	for(tim=0;tim<1000;tim++)
	{
		/* prevent compiler optimization */
		if(tim > 800)
			tim++;
	}

	/* now sample the value in the input */
	push_button_override = GPIO_GPIO_EXT_PORTA_GPIO_EXT_PORTA_R;
	push_button_override = ((push_button_override >> 0) & 0x1);

	/* return mux to previous value */
	GME_IO_MUX_CTRL_0_IO_MUX_CTRL_5_W(mux_ctrl_0_5);
	/* return to reset value */
	GPIO_GPIO_SWPORTA_DDR_GPIO_SWPORTA_DDR_W(gpio_ddr_reset);

	if (push_button_override == 0)
	{
		if (isBootfromFlash == 1)
		{
			rel_log("flash (bootfix): push button override detected, skipping boot from flash\n");
			isBootfromFlash = 0;
		}
	}
#endif

	*(GME_SAVE_AND_RESTORE_0_REG + OFFSET_LOAD_FROM_FLASH) = isBootfromFlash;
	return isBootfromFlash;
}

void storage_update_flash_flags()
{
	int ret;
	unsigned int startOffset;

	startOffset = SECTION_BOOTFIX_META_START_BLOCK * flashInfo.blockSize;
	ret = SPI_FLASHG_Read(startOffset, sizeof(InuSectionHeaderT), (uint8 *)&inuSectionHeader);
	if (!ret)
	{
		if (inuSectionHeader.magicNumber == SECTION_BOOTFIX_META_MAGIC_NUMBER)
		{
			startOffset += sizeof(InuSectionHeaderT);
			ret = SPI_FLASHG_Read(startOffset, sizeof(InuBootfixHeaderT), (uint8 *)&bootfixMetadata);
			if (!ret)
			{
				if (bootfixMetadata.flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP)
				{
					bootfixMetadata.flashFlags |= INU_STORAGE_LAYOUT__FLASH_FLAGS_FATAL;
				}
				else
				{
					// Upon
					bootfixMetadata.flashFlags |= INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP;
					bootfixMetadata.flashFlags &= ~INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS;
				}

				SPI_FLASHG_program(startOffset, (uint8 *)&bootfixMetadata, sizeof(InuBootfixHeaderT));
			}
		}
	}

}
