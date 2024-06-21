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
#include "flash_info.h"
#include "nu4000_c0_gme_regs.h"
#include "storage.h"
#include "spl.h"
#include "inu_boot_common.h"
#include "system.h"

#define SPI_PAGE_SIZE            (256)
#define BLOCK_SIZE               (0x10000)

#define BOOTSPL_ACTIVE_PARTITION_START_BLOCK (SECTION_BOOTSPL_START_BLOCK)
#define BOOTSPL_BACKUP_PARTITION_START_BLOCK (flashInfo.numOfBlocks/2)
#define PARTITION_SIZE_IN_BLOCKS             (flashInfo.numOfBlocks/2 - SECTION_BOOTSPL_START_BLOCK)

/**********************************************
 * local data
***********************************************/

InuSectionHeaderT inuSectionHeader;
InuBootfixHeaderT bootfixMetadata;
char metaSectionBuf[sizeof(InuSectionHeaderT) + sizeof(InuBootfixHeaderT)];
FlashInfoT flashInfo = {0x10000, 128, 0};
unsigned int flashFlags;

unsigned int storageMagicNumber[INU_STORAGE_SECTION_APP + 1] = {
	SECTION_BOOTFIX_MAGIC_NUMBER,
	SECTION_BOOTFIX_META_MAGIC_NUMBER,
	SECTION_PRODUCTION_MAGIC_NUMBER,
	SECTION_CALIBRATION_MAGIC_NUMBER,
	SECTION_BOOTSPL_MAGIC_NUMBER,
	SECTION_DTB_MAGIC_NUMBER,
	SECTION_KERNEL_MAGIC_NUMBER,
	SECTION_ROOTFS_MAGIC_NUMBER,
	SECTION_CEVA_MAGIC_NUMBER,
	SECTION_CNN_LD_MAGIC_NUMBER,
	SECTION_CNN_MAGIC_NUMBER,
	SECTION_APP_MAGIC_NUMBER
};

char *storageSectionStr[INU_STORAGE_SECTION_APP + 1] = {

	"BOOTFIX ------",
	"BOOTFIX_META -",
	"PRODUCTION ---",
	"CALIBRATION --",
	"BOOTSPL ------",
	"DTB ----------",
	"KERNEL -------",
	"ROOTFS -------",
	"CEVA ---------",
	"CNN_LD -------",
	"CNN ----------",
	"APP ----------"
};

unsigned int storageMustHaveComponent[INU_STORAGE_SECTION_APP + 1] = {
	1, // INU_STORAGE_SECTION_BOOTFIX          = 0,
	1, // INU_STORAGE_SECTION_BOOTFIX_METADATA = 1,
	1, // INU_STORAGE_SECTION_PRODUCTION_DATA  = 2,
	1, // INU_STORAGE_SECTION_CALIBRATION_DATA = 3,
	1, // INU_STORAGE_SECTION_BOOTSPL          = 4,
	1, // INU_STORAGE_SECTION_DTB              = 5,
	1, // INU_STORAGE_SECTION_KERNEL           = 6,
	1, // INU_STORAGE_SECTION_ROOTFS           = 7,
	0, // INU_STORAGE_SECTION_CEVA             = 8,
	0, // INU_STORAGE_SECTION_CNN_LD           = 9,
	0, // INU_STORAGE_SECTION_CNN              = 10,
	0  // INU_STORAGE_SECTION_APP              = 11
};

extern UINT32 spi_nor_ids_arr_size;
extern const struct flash_info spi_nor_ids[];

static int find_block_address(InuStorageSectionTypeE sectionType, unsigned int *pBlockAddress)
{
	int ret = -1;
	unsigned int ind;
	InuSectionHeaderT inuSectionHeader;
	unsigned int lastBlockAddress=0;
	unsigned int sectionBootsplStartBlock = BOOTSPL_ACTIVE_PARTITION_START_BLOCK;
	unsigned int lastBlockInPartition     = (flashInfo.numOfBlocks/2 - 1);

	if (bootfixMetadata.flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP)
	{
		sectionBootsplStartBlock = BOOTSPL_BACKUP_PARTITION_START_BLOCK;
		//
		lastBlockInPartition    += PARTITION_SIZE_IN_BLOCKS;
	}

	switch (sectionType)
	{

	case INU_STORAGE_SECTION_BOOTFIX_METADATA:
		*pBlockAddress = SECTION_BOOTFIX_META_START_BLOCK*flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_BOOTSPL:
		*pBlockAddress = sectionBootsplStartBlock * flashInfo.blockSize;
		break;
	case INU_STORAGE_SECTION_DTB:
	case INU_STORAGE_SECTION_KERNEL:
	case INU_STORAGE_SECTION_ROOTFS:
	case INU_STORAGE_SECTION_CEVA:
	case INU_STORAGE_SECTION_CNN_LD:
	case INU_STORAGE_SECTION_CNN:
	case INU_STORAGE_SECTION_APP:

		*pBlockAddress = sectionBootsplStartBlock * flashInfo.blockSize;
		for(ind = INU_STORAGE_SECTION_BOOTSPL;ind < sectionType;ind++)
		{
			if (lastBlockAddress != *pBlockAddress) // avoid reading again when skip CEVA/CNN
				ret = SPI_FLASHG_Read(*pBlockAddress, sizeof(inuSectionHeader),(unsigned char *)&inuSectionHeader);
			lastBlockAddress = *pBlockAddress;
			if (ret != INU_STORAGE_ERR_SUCCESS)
				return INU_STORAGE_ERR_INIT_FAILED;

			 if (storageMagicNumber[ind] != inuSectionHeader.magicNumber)
			{
				// this sections can be ignored
				if ((storageMustHaveComponent[ind] == 0)&&(sectionType != ind))
				{
					continue;
				}
				return INU_STORAGE_ERR_INIT_FAILED;
			}
			else
			{
				*pBlockAddress += inuSectionHeader.sectionSize*flashInfo.blockSize;
			}
		}

	default:
		ret = -1;
		break;

	}
	if ((*pBlockAddress/ flashInfo.blockSize) > lastBlockInPartition) {
		spl_log("flash (spl): error, section exceeds flash size\n");
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
				spl_log("flash (spl): production data valid, bootId=%d\n",((InuProductionHeaderT *)(prod_data_address+sizeof(UINT32)))->bootId);
			}
		}
	}

	GME_SAVE_AND_RESTORE_1_VAL = BOOTDETECT_STATUS_FINISH;
	return;
}

void read_bootfix_meta_data()
{
	uint32 ret=0;
	unsigned int flash_offset = 0;
	InuSectionHeaderT  sectoinHeader;
	InuBootfixHeaderT inuBootfixHeader;

	flash_offset = SECTION_BOOTFIX_META_START_BLOCK * flashInfo.blockSize;
	ret = SPI_FLASHG_Read(flash_offset, sizeof(InuSectionHeaderT), (unsigned char *) &sectoinHeader);
	if (ret==0)
	{
		if (sectoinHeader.magicNumber == SECTION_BOOTFIX_META_MAGIC_NUMBER)
		{
			ret = SPI_FLASHG_Read(flash_offset + sizeof(InuSectionHeaderT), sizeof(InuBootfixHeaderT), (unsigned char *)&bootfixMetadata);
			if (ret)
			{
				abort_log("spi:can't read from offset = %x, size = %x",flash_offset + sizeof(InuSectionHeaderT), sizeof(InuBootfixHeaderT));
				return -1;
			}
			spl_log("flash (spl): %s(): bootfixMetadata.flashFlags = %x\n", __func__, bootfixMetadata.flashFlags);
		}
	}

	return;
}

/**********************************************
 * global functions
***********************************************/

void storage_init()
{
	UINT32 retVal, tmp, ind=0;
	UINT8 id[SPI_NOR_MAX_ID_LEN]={0};
	const struct flash_info *info;
	static UINT32 storageIsInit = 0;

	if (storageIsInit)
	{
		// storage already initialized.
		return;
	}

	flashInfo.blockSize = 64 * 1024;
	flashInfo.numOfBlocks = 128;
	flashInfo.spi_flags = 0;

	retVal = SPI_FLASHG_read_reg(SPINOR_OP_RDID, id, SPI_NOR_MAX_ID_LEN);

	if (!retVal)
	{
		retVal = 1;
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

					spl_log(
							"flash (spl): found model %s, blockSize=%x, spi_flags = %x, numOfBlocks=%d\n",
							info->name, flashInfo.blockSize, flashInfo.spi_flags, flashInfo.numOfBlocks);
					retVal = 0;
				}

			}
		}
	}
	if (retVal)
		spl_log("flash (spl): flash model unknown, using defaults , blockSize=0x%x, numOfBlocks=%d\n", flashInfo.blockSize, flashInfo.numOfBlocks);
	spl_log("ind %d id %x %x %x %x %x %x\n",ind, id[0],id[1],id[2],id[3],id[4],id[5]);
	// read production data into CRAM, will be used for bootdetect purpose
	read_production_data();
	read_bootfix_meta_data();

	storageIsInit = 1;
	return;
}

void storage_print_section_hdr(InuStorageSectionTypeE sectionType,InuSectionHeaderT *pInuSectionHeader)
{
	unsigned int *timestamp=(unsigned int *)&pInuSectionHeader->timestamp;

	pInuSectionHeader->versionString[VERSION_STRING_SIZE - 1] =0;
	spl_log("flash (spl): section %s> version = %s, timestamp %x\n", storageSectionStr[sectionType], pInuSectionHeader->versionString, timestamp[0]);
}

int storage_get_section_data(InuStorageSectionTypeE sectionType, unsigned char *buffer)
{

	unsigned int flash_offset = 0;
	int ret, origSize;
	if (find_block_address(sectionType,&flash_offset) != 0)
	{
		abort_log("spi:can't find_block_address %x ",sectionType);
		return -1;
	}
	ret = SPI_FLASHG_Read(flash_offset, sizeof(InuSectionHeaderT), (uint8 *) &inuSectionHeader);
	if (ret)
	{
		abort_log("spi:can't read from offset=%x ",flash_offset);
		return -1;
	}
	flash_offset += sizeof(InuSectionHeaderT);
	if (inuSectionHeader.magicNumber == storageMagicNumber[sectionType])
	{
		storage_print_section_hdr(sectionType, &inuSectionHeader);
		//round up the size to div by 4, to ease the driver work
		origSize = inuSectionHeader.sectionDataSize;
		if ((inuSectionHeader.sectionDataSize%4) != 0)
			inuSectionHeader.sectionDataSize += (4 - (inuSectionHeader.sectionDataSize%4));
	  if(sectionType == INU_STORAGE_SECTION_BOOTFIX_METADATA)
	  {
		  inuSectionHeader.sectionDataSize = ((sizeof(InuBootfixHeaderT) % 4) == 0) ?
		  	(sizeof(InuBootfixHeaderT)) : ((sizeof(InuBootfixHeaderT) & 0xFFFFFFFC) + 4);
		 origSize = inuSectionHeader.sectionDataSize;

	  }
		ret = SPI_FLASHG_Read(flash_offset, inuSectionHeader.sectionDataSize, buffer);
		if (ret)
		{
			abort_log("spi:can't read from offset=%x ",flash_offset);
			return -1;
		}
	}
	else
	{
		return -1;
	}
	return origSize;
}



int storage_update_bootfix_header(InuBootfixHeaderT *buffer)
{

	unsigned int offset = 0;
	int ret;
	InuSectionHeaderT *hdr;
	InuBootfixHeaderT *bootfixMeta;


	if (find_block_address(INU_STORAGE_SECTION_BOOTFIX_METADATA,&offset) != 0)
	{
		spl_log("spi:can't find_block_address  %x ",INU_STORAGE_SECTION_BOOTFIX_METADATA);
		return -1;
	}
	ret = SPI_FLASHG_Read(offset, (sizeof(InuSectionHeaderT) + sizeof(InuBootfixHeaderT)), (uint8 *) &metaSectionBuf);
	if (ret)
	{
		abort_log("spi:can't read from offset=%x ",offset);
		return -1;
	}

	hdr = (InuSectionHeaderT *) &metaSectionBuf[0];
	bootfixMeta = (InuBootfixHeaderT *) &metaSectionBuf[sizeof(InuSectionHeaderT)];

	if (hdr->magicNumber == storageMagicNumber[INU_STORAGE_SECTION_BOOTFIX_METADATA] )
	{
		ret = SPI_FLASHG_blockErase(offset);
		if (ret)
		{
			abort_log("spi:can't erase from offset=%x ",offset);
			return -1;
		}

		memcpy(bootfixMeta, buffer, sizeof(InuBootfixHeaderT));
		spl_log("flash (spl): %s(): Call SPI_FLASHG_program(). Start offset target = %x, size = %x\n",__func__, offset, sizeof(InuBootfixHeaderT));
		ret = SPI_FLASHG_program(offset, (uint8 *) &metaSectionBuf, (sizeof(InuSectionHeaderT) + sizeof(InuBootfixHeaderT)));
		if (ret)
		{
			abort_log("spi:can't read from offset=%x ",offset);
			return -1;
		}
	}
	else
	{
		abort_log("spi:wrong magic number %x ", inuSectionHeader.magicNumber);
		return -1;
	}

	 return 0;
}

void storage_set_flash_quad()
{
	flashInfo.spi_flags |= SPI_NOR_QUAD_READ;
}

void storage_update_flash_flags(FwUpdateE updateState)
{
	int ret;
	unsigned int startOffset;
	int isBootfromFlash = 0;
	unsigned int sectionHeaderSize = sizeof(InuSectionHeaderT);

	startOffset = SECTION_BOOTFIX_META_START_BLOCK * flashInfo.blockSize;
	ret = SPI_FLASHG_Read(startOffset, sectionHeaderSize, (uint8 *)&inuSectionHeader);
	spl_log("flash (spl): %s(): Call SPI_FLASHG_Read() of inuSectionHeader. ret = %x\n",__func__, ret);
	if (!ret)
	{
		spl_log("flash (spl): %s(): Call SPI_FLASHG_Read(). magicNumber = %x, SECTION_BOOTFIX_META_MAGIC_NUMBER = %x\n",__func__, ret, inuSectionHeader.magicNumber, SECTION_BOOTFIX_META_MAGIC_NUMBER);
		if (inuSectionHeader.magicNumber == SECTION_BOOTFIX_META_MAGIC_NUMBER)
		{
			ret = SPI_FLASHG_Read(startOffset + sectionHeaderSize, sizeof(InuBootfixHeaderT), (uint8 *)&bootfixMetadata);
			spl_log("flash (spl): %s(): Call SPI_FLASHG_Read() of bootfixMetadata. ret = %x\n",__func__, ret);
			if (!ret)
			{
				if (updateState == FW_UPDATE_SUCCESS)
				{
					bootfixMetadata.flashFlags &= ~INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS;
					GME_SAVE_AND_RESTORE_7_VAL = FW_UPDATE_FROM_ACTIVE_PARTITION;
				}
				else if (updateState == FW_UPDATE_BTBP_VALID)
				{
					bootfixMetadata.flashFlags &= ~INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP;
					GME_SAVE_AND_RESTORE_7_VAL = FW_UPDATE_FROM_BACKUP_PARTITION;
				}
				else //FW_UPDATE_ERROR
				{
					if (bootfixMetadata.flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP)
					{
						// Upon Partition validatyion failure of Backup Partition need to rais FATAL flag and clear BTBP flag
						bootfixMetadata.flashFlags &= ~INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP;
						bootfixMetadata.flashFlags |= INU_STORAGE_LAYOUT__FLASH_FLAGS_FATAL;
					}
					else
					{
						// Upon Partition validatyion failure of Active Partition, need to raise BTBP flag, and clear FW_UPDATE flag
						bootfixMetadata.flashFlags |= INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP;
						bootfixMetadata.flashFlags &= ~INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS;
					}
				}

				ret = SPI_FLASHG_blockErase(startOffset);
				if (ret)
				{
					abort_log("spi:can't erase from offset=%x ",startOffset);
					return -1;
				}
				spl_log("flash (spl): %s(): Call SPI_FLASHG_program(). Start offset target = %x, size = %x\n",__func__, startOffset, sizeof(InuBootfixHeaderT));
				SPI_FLASHG_program(startOffset, (uint8 *)&inuSectionHeader, sectionHeaderSize);
				SPI_FLASHG_program(startOffset + sectionHeaderSize, (uint8 *)&bootfixMetadata, sizeof(InuBootfixHeaderT));
			}
		}
	}
}

void storage_copy_partition(UINT32 targetOffset, UINT32 sourceOffset)
{
	UINT32 totalSize = PARTITION_SIZE_IN_BLOCKS * BLOCK_SIZE;
	UINT8  buff[SPI_PAGE_SIZE];
	UINT32 totalSizeInPages = totalSize/SPI_PAGE_SIZE;
	UINT32 pageNum = 0, blockNum = 0;
	UINT32 startSourceOffset = sourceOffset;
	UINT32 startTargetOffset = targetOffset;
	UINT32 ret;

	spl_log("flash (spl): %s(): ENTRY. totalSizeInPages = %d, PARTITION_SIZE_IN_BLOCKS = %d\n",__func__, totalSizeInPages, PARTITION_SIZE_IN_BLOCKS);
	spl_log("flash (spl): %s(): Call SPI_FLASHG_blockErase(). Start offset = %x\n",__func__, targetOffset);
	// Erase target partition
	for (blockNum = 0; blockNum < PARTITION_SIZE_IN_BLOCKS; blockNum++)
	{
		if (!(blockNum % 100))
		{
			spl_log("flash (spl): %s(): Running SPI_FLASHG_blockErase(). Start offset = %x. blockNum = %d\n",__func__, targetOffset, blockNum);
		}
		SPI_FLASHG_blockErase(targetOffset);
		targetOffset += BLOCK_SIZE;
	}

	//spl_log("flash (spl): %s(): End SPI_FLASHG_blockErase(). Start offset = %x. blockNum = %d\n",__func__, targetOffset, blockNum);

	spl_log("flash (spl): %s(): Call SPI_FLASHG_program(). Start offset target = %x, source = %x\n",__func__, startTargetOffset, startSourceOffset);

	// Copy source partion to target partition SPI_PAGE_SIZE (256 Bytes) at a time.
	for (pageNum = 0; pageNum < totalSizeInPages; pageNum++)
	{
		// if (pageNum == 0)
		// {
		// 	startSourceOffset += SPI_PAGE_SIZE;
		// 	startTargetOffset += SPI_PAGE_SIZE;

		// 	continue;
		// }

		ret = SPI_FLASHG_Read(startSourceOffset, SPI_PAGE_SIZE, buff);
		if (ret)
		{
			abort_log("spi:can't read from offset=%x ", startSourceOffset);
			return;
		}

		if (!(pageNum % (100*256)))
		{
			spl_log("flash (spl): %s(): Running SPI_FLASHG_program(). Start offset target = %x, source = %x. pageNum = %d\n",__func__, startTargetOffset, startSourceOffset, pageNum);
			//spl_log("flash (spl): %s(): %.8x: %.8x, %.8x, %.8x, %.8x, %.8x, %.8x, %.8x, %.8x\n",__func__,  startSourceOffset, ((UINT32*)buff)[0], ((UINT32*)buff)[1], ((UINT32*)buff)[2], ((UINT32*)buff)[3], ((UINT32*)buff)[4], ((UINT32*)buff)[5], ((UINT32*)buff)[6], ((UINT32*)buff)[7]);
		}

		SPI_FLASHG_program(startTargetOffset, buff, SPI_PAGE_SIZE);

		startSourceOffset += SPI_PAGE_SIZE;
		startTargetOffset += SPI_PAGE_SIZE;
	}

	spl_log("flash (spl): %s(): End SPI_FLASHG_program(). Start offset target = %x, source = %x. pageNum = %d\n",__func__, startTargetOffset, startSourceOffset, pageNum);
}

void storage_restore_flash_info_from_backup_partition()
{
	UINT32 targetOffset = BOOTSPL_ACTIVE_PARTITION_START_BLOCK * BLOCK_SIZE;
	UINT32 sourceOffset = BOOTSPL_BACKUP_PARTITION_START_BLOCK * BLOCK_SIZE;
	storage_copy_partition(targetOffset, sourceOffset);
	storage_update_flash_flags(FW_UPDATE_BTBP_VALID);
}

void storage_copy_flash_info_from_active_to_backup_partition()
{
	UINT32 targetOffset = BOOTSPL_BACKUP_PARTITION_START_BLOCK * BLOCK_SIZE;
	UINT32 sourceOffset = BOOTSPL_ACTIVE_PARTITION_START_BLOCK * BLOCK_SIZE;
	storage_copy_partition(targetOffset, sourceOffset);
	storage_update_flash_flags(FW_UPDATE_SUCCESS);
}

int storage_copy_flash_info_to_other_partition()
{
	spl_log("flash (spl): %s(): bootfixMetadata.flashFlags = %x\n", __func__ ,bootfixMetadata.flashFlags);

	if (bootfixMetadata.flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS)
	{
		storage_copy_flash_info_from_active_to_backup_partition();
		return INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS;
	}
	else if (bootfixMetadata.flashFlags & INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP)
	{
		storage_restore_flash_info_from_backup_partition();
		return INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP;
	}
}

void storage_update_failure_flash_flags()
{
	storage_update_flash_flags(FW_UPDATE_ERROR);
}
