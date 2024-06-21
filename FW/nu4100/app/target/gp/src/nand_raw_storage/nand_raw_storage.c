/****************************************************************************
 *
 *   FileName: Nand_raw_storage.c
 *
 *   Author: Dima S.
 *
 *   Date: 
 *
 *   Description: Raw NAND sections accesss
 *   
 ****************************************************************************/


/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#include "os_lyr.h"
#include "inu_storage_layout.h"

#include "nand_raw_storage.h"
#include "nand_drv.h"

#ifdef __cplusplus
      extern "C" {
#endif






/****************************************************************************
 ***************        L O C A L      D E F N I T I O N S  ***************
 ****************************************************************************/
//#define NAND_STRG_DEBUG

 /****************************************************************************
  ***************		   L O C A L	 T Y P E D E F S	  ***************
  ****************************************************************************/

 /****************************************************************************
  ***************	   L O C A L		D A T A 		   ***************
  ****************************************************************************/
static UINT8 storageInitialized = 0;
 
 /****************************************************************************
  ***************	   G L O B A L		D A T A 		   ***************
  ****************************************************************************/

 /****************************************************************************
  ***************	   E X T E R N A L	  F U N C T I O N S 	 ***************
  ****************************************************************************/
 
 /****************************************************************************
  ***************	  P R E    D E F I N I T I O N		OF		***************
  ***************	  L O C A L 	  F U N C T I O N S 	 ***************
  ****************************************************************************/
#if 0
static ERRG_codeE NAND_STRGG_scanForBadBlocks(void);
static ERRG_codeE NAND_STRGG_buildBadBlocksMap(void);
#endif

  
  /****************************************************************************
   ***************	   L O C A L	   F U N C T I O N S	***************
   ****************************************************************************/
  
 static ERRG_codeE NAND_STRGG_initHeader(InuStorageNandHeaderT* header)
 {
 	 UINT8 type = 0;
	 UINT32 sectOffset, sectLen; 

	 //memset(header, NAND_EMPTY_UINT8, sizeof(InuStorageNandHeaderT));

	 header->magicNumber = NAND_HEADER_MAGIC_NUMBER;
	 header->headerVersion = NAND_HEADER_HEADER_VERSION;
	 

	for( type = NAND_SECTION_TYPE_HEADER; type <= NAND_SECTION_TYPE_APP; type++ )
   	{
   		NAND_STRGG_getSectionOffsetSize(type, &sectOffset, &sectLen);
		
		//LOGG_PRINT(LOG_INFO_E, NULL, "Section type %d, offset 0x%x size 0x%x \n", type, sectOffset, sectLen);
		header->section[type].type				= type;
		header->section[type].startAddress 		= sectOffset;
		header->section[type].sizeBytes 		= sectLen;
		header->section[type].imgStartAddress 	= NAND_EMPTY_UINT32;
		header->section[type].imgSizeBytes 		= NAND_EMPTY_UINT32;
		header->section[type].imgVersion.major	= NAND_EMPTY_UINT32;
		header->section[type].imgVersion.minor	= NAND_EMPTY_UINT32;
		header->section[type].imgVersion.build	= NAND_EMPTY_UINT32;
   	}
   
	 return NAND__RET_SUCCESS;
 }

 /****************************************************************************
  ***************	  G L O B A L		 F U N C T I O N S	  **************
  ****************************************************************************/

 ERRG_codeE NAND_STRGG_init(void)
{
	ERRG_codeE ret = NAND__RET_SUCCESS;
	InuStorageNandHeaderT header;

#ifdef NAND_STRG_DEBUG
	UINT32 i = 0;
#endif

	ret = NAND_DRVG_open();

	if(ERRG_FAILED(ret))
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "NAND raw device not found\n");
		return ret;
	}
	else
	{
		NAND_STRGG_readHeader(&header);

		if(header.magicNumber != NAND_HEADER_MAGIC_NUMBER)	//Header not exists - Fill sections locations into header
		{
			NAND_STRGG_initHeader(&header);
		}
		
		//NAND_STRGG_buildBadBlocksMap();
		storageInitialized = 1;

#ifdef NAND_STRG_DEBUG	
		LOGG_PRINT(LOG_INFO_E, NULL, "NAND raw device initialized.\n");

		printf("Magic Number : 0x%x\n", header.magicNumber);
		printf("Header Version : 0x%x\n", header.headerVersion);
		printf("Header Version : 0x%x\n", header.headerVersion);
		printf("Sections list:\n");

		for(i = 0; i < NAND_SECTION_MAX_SECTIONS; i++)
		{
			printf("Type : 0x%x\n", header.section[i].type);
			printf("SectionAddr : 0x%x\n", header.section[i].startAddress);
			printf("SectionSize : 0x%x\n", header.section[i].sizeBytes);
			printf("Image Offset : 0x%x\n", header.section[i].imgStartAddress);
			printf("Image Size : 0x%x\n", header.section[i].imgSizeBytes);
			printf("Ver Maj : 0x%x\n", header.section[i].imgVersion.major);
			printf("Ver Min : 0x%x\n", header.section[i].imgVersion.minor);
			printf("Ver build : 0x%x\n", header.section[i].imgVersion.build);
			
			printf("\n");
		}
#endif
	}


	return ret;
}

bool NAND_STRGG_isDevPresent(void)
{
	ERRG_codeE ret = NAND__RET_SUCCESS;

	if(storageInitialized == 1)
	{
		return TRUE;
	}
	else
	{
		ret  = NAND_STRGG_init();
		if(ERRG_SUCCEEDED(ret))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
}

 ERRG_codeE NAND_STRGG_readSection(NAND_STRGG_ioParamT *io)
 {
	 ERRG_codeE ret = NAND__RET_SUCCESS;
	 NAND_DRVG_ioParamT readOp;
	 UINT32 sectionSize;

	 readOp.buf = io->buf;
	 readOp.length = io->length;
	 NAND_STRGG_getSectionOffsetSize(io->type, &readOp.offset, &sectionSize);
	 if(sectionSize < readOp.length)
	 {
		 return NAND__ERR_OUT_OF_MEM;
	 }

	 ret = NAND_DRVG_read(&readOp);
	 
	 return ret;
 }
 ERRG_codeE NAND_STRGG_writeSection(NAND_STRGG_ioParamT *io)
 {
	 ERRG_codeE ret = NAND__RET_SUCCESS;
	 NAND_DRVG_ioParamT writeOp;
	 UINT32 sectionSize;

	 writeOp.buf = io->buf;
	 writeOp.length = io->length;
	 NAND_STRGG_getSectionOffsetSize(io->type, &writeOp.offset, &sectionSize);
	 if(sectionSize < writeOp.length)
	 {
		 return NAND__ERR_OUT_OF_MEM;
	 }

	 ret = NAND_DRVG_write(&writeOp);
	 
	 return ret;
 }
 ERRG_codeE NAND_STRGG_eraseSection(InuStorageNandSectionTypE type)
 {
	ERRG_codeE ret = NAND__RET_SUCCESS;	
	NAND_DRVG_ioParamT eraseOp;

	NAND_STRGG_getSectionOffsetSize(type, &eraseOp.offset, &eraseOp.length);

	NAND_DRVG_erase(&eraseOp);
	

	return ret;
 }

ERRG_codeE NAND_STRGG_readHeader(InuStorageNandHeaderT* header)
{
	ERRG_codeE ret = NAND__RET_SUCCESS;
	NAND_DRVG_ioParamT readOp;
	UINT32 sectionSize;
	
	if(!storageInitialized)
	{
//		ret = NAND_STRGG_init();
		if(ERRG_FAILED(ret))
			return ret;
	}	
	
	readOp.buf = (UINT8*)header;
	readOp.length = sizeof(InuStorageNandHeaderT);
	NAND_STRGG_getSectionOffsetSize(NAND_SECTION_TYPE_HEADER, &readOp.offset, &sectionSize);
	LOGG_PRINT(LOG_INFO_E, NULL, "Reading header from block %d.\n", (readOp.offset/NAND_DRVG_getBlockSize()) );

	
	ret = NAND_DRVG_read(&readOp);
	
	return ret;
}

ERRG_codeE NAND_STRGG_getSectionOffsetSize(InuStorageNandSectionTypE type, UINT32 *offset, UINT32 *size)
{
   //The MTD device is already pointing to the start of the FW update partition
	UINT32 partitionStart = 0;//NAND_PARTITION_FW_UPDATE_START_BLOCK;
	
	switch(type)
	{
		case NAND_SECTION_TYPE_HEADER: 
			*offset = (partitionStart + NAND_SECTION_HEADER_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_HEADER_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_BOOTSPL: 
			*offset = (partitionStart + NAND_SECTION_BOOTSPL_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_BOOTSPL_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_KERNEL: 
			*offset = (partitionStart + NAND_SECTION_KERNEL_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_KERNEL_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_DTB: 
			*offset = (partitionStart + NAND_SECTION_DTB_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_DTB_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_CEVA: 
			*offset = (partitionStart + NAND_SECTION_CEVA_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_CEVA_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_EV62_BOOT: 
			*offset = (partitionStart + NAND_SECTION_EV62_BOOT_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_EV62_BOOT_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_EV62_APP: 
			*offset = (partitionStart + NAND_SECTION_EV62_APP_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_EV62_APP_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_INITRAMFS: 
			*offset = (partitionStart + NAND_SECTION_INITRAMFS_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_INITRAMFS_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;
		case NAND_SECTION_TYPE_APP: 
			*offset = (partitionStart + NAND_SECTION_APP_START_BLOCK) * NAND_DRVG_getBlockSize();
			*size = NAND_SECTION_APP_SIZE_BLOCKS * NAND_DRVG_getBlockSize();
		break;

		
		case NAND_SECTION_TYPE_NONE: 
		default:	
			*offset = NAND_EMPTY_UINT32;
			*size = NAND_EMPTY_UINT32;
		break;
	}

   return NAND__RET_SUCCESS;
}


ERRG_codeE NAND_STRGG_updateSection(NAND_STRGG_ioParamT *io, InuStorageNandImgVerT* imageVer)
{
	ERRG_codeE ret = NAND__RET_SUCCESS;
	InuStorageNandHeaderT header;
	NAND_STRGG_ioParamT write;
	UINT32 j = 0;
	UINT8 i = 0, sectionFound = 0;

	if(!storageInitialized)
	{
		ret = NAND_STRGG_init();
		if(ERRG_FAILED(ret))
			return ret;
	}

	
#if 0
	NAND_STRGG_scanForBadBlocks();
	return ret;
#endif
	if((io->buf == 0) || (io->length == 0))
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid Input params: type %d, buf %p, len %d.\n", io->type, io->buf, io->length);
		return NAND__ERR_INVALID_ARGS;
	}
	
#if 0
	LOGG_PRINT(LOG_INFO_E, NULL, "Input params: type %d, buf %p, len %d. Ver: maj %d min %d build %d\n", io->type, io->buf, io->length);
#endif

	NAND_STRGG_readHeader(&header);

	if(header.magicNumber != NAND_HEADER_MAGIC_NUMBER)	//Header not exists - Fill sections locations into header
	{
		NAND_STRGG_initHeader(&header);
	}
	
	for(i = 0; i < NAND_SECTION_MAX_SECTIONS; i++)
	{
		if(header.section[i].type == io->type )
		{
			LOGG_PRINT(LOG_INFO_E, NULL, "Found Update request for section %d, type %d.\n", i, io->type);
			sectionFound = 1;
			break;
		}
	}

	if(sectionFound == 0)
	{
	
		LOGG_PRINT(LOG_INFO_E, NULL, "Did no found Update request for section %d, type %d.\n", i, io->type);
		return NAND__ERR_INVALID_ARGS;
	}
#if 0	
	else
	{
	
		printf("Magic Number : 0x%x\n", header.magicNumber);
		printf("Header Version : 0x%x\n", header.headerVersion);
		printf("Sections list:\n");
		
		for(i = 0; i < NAND_SECTION_MAX_SECTIONS; i++)
		{
			printf("Type : 0x%x\n", header.section[i].type);
			printf("SectionAddr : 0x%x\n", header.section[i].startAddress);
			printf("SectionSize : 0x%x\n", header.section[i].sizeBytes);
			printf("Image Offset : 0x%x\n", header.section[i].imgStartAddress);
			printf("Image Size : 0x%x\n", header.section[i].imgSizeBytes);
			printf("Ver Maj : 0x%x\n", header.section[i].imgVersion.major);
			printf("Ver Min : 0x%x\n", header.section[i].imgVersion.minor);
			printf("Ver build : 0x%x\n", header.section[i].imgVersion.build);
			
			printf("\n");
		}
		LOGG_PRINT(LOG_INFO_E, NULL, "\tSTUB.\n");
		return ret;
	}
#endif	

	LOGG_PRINT(LOG_INFO_E, NULL, "Erasing section %d.\n", i);

	ret = NAND_STRGG_eraseSection(io->type);
	if(ERRG_FAILED(ret))
	{
	
		LOGG_PRINT(LOG_ERROR_E, NULL, "Erasing section %d failed.\n", i);
		return NAND__ERR_INVALID_ARGS;
	}

	header.section[i].imgStartAddress = NAND_EMPTY_UINT32;
	header.section[i].imgSizeBytes = NAND_EMPTY_UINT32;
	
	header.section[i].imgVersion.major = NAND_EMPTY_UINT32;
	header.section[i].imgVersion.minor = NAND_EMPTY_UINT32;
	header.section[i].imgVersion.build = NAND_EMPTY_UINT32;

	ret = NAND_STRGG_writeSection(io);
	if(ERRG_SUCCEEDED(ret))
	{
		NAND_DRVG_ioParamT readOp;

		readOp.buf = malloc(io->length);
		readOp.offset = header.section[i].startAddress;
		readOp.length = io->length;
		
		NAND_DRVG_read(&readOp);

		if(memcmp(readOp.buf, io->buf, io->length) == 0)
		{
			header.section[i].imgStartAddress = header.section[i].startAddress;
			header.section[i].imgSizeBytes = io->length;
			
			header.section[i].imgVersion.major =  imageVer->major;
			header.section[i].imgVersion.minor =  imageVer->minor;
			header.section[i].imgVersion.build =  imageVer->build;
			
			write.buf = (UINT8*)&header;
			write.length = sizeof(InuStorageNandHeaderT);
			write.type = NAND_SECTION_TYPE_HEADER;
			
			NAND_STRGG_eraseSection(NAND_SECTION_TYPE_HEADER);
			NAND_STRGG_writeSection(&write);
		}
		else
		{
			LOGG_PRINT(LOG_ERROR_E, NULL, "Data verification on section %d failed\n", i);

			

			for(j = 0; j < (header.section[i].imgSizeBytes); j += NAND_DRVG_getBlockSize())
			{
				if(memcmp((UINT8*)(readOp.buf + j), (UINT8*)(io->buf + j), NAND_DRVG_getBlockSize()) != 0)
				{
					LOGG_PRINT(LOG_ERROR_E, NULL, "Found bad block %d\n", 
							((header.section[i].startAddress + j) / NAND_DRVG_getBlockSize())) ;
				}
			}
			//return ret;
		}
		free(readOp.buf);
		
	}
	else
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "Writing Section %d failed\n", i);		
		return ret;
	}

	return ret;

}

/*
UINT32 NAND_DRVG_getPageSize();
UINT32 NAND_DRVG_getBlockSize();
UINT32 NAND_DRVG_getPartitionSize();
ERRG_codeE NAND_DRVG_isBadBlock(UINT32 offset, UINT8* isBad);
ERRG_codeE NAND_DRVG_setBadBlock(UINT32 offset);
*/
#if 0
ERRG_codeE NAND_STRGG_buildBadBlocksMap(void)
{
	
	ERRG_codeE ret = NAND__RET_SUCCESS;
	int retVal;
	loff_t offset;
	UINT32 bbCount = 0;
	UINT32 bbMapSize;
	UINT8 isBadBlock = 0;
	NAND_DRVG_ioParamT ioWriteBb, ioReadBb;
	InuStorageNandBbMapT *bbMap;
	

	if(storageInitialized == 0)
	{
		return NAND__ERR_NOT_SUPPORTED;
	}


	bbMapSize = sizeof(InuStorageNandBbMapT);
	bbMap = malloc(bbMapSize);		// map is uint32 per block
	
	ioReadBb.buf = (UINT8*)bbMap;
	ioReadBb.length = (bbMapSize);  
	ioReadBb.offset = (NAND_SECTION_HEADER_BB_MAP_OFFSET);
	
	ret = NAND_DRVG_read(&ioReadBb);
	if(bbMap->magicNumber == NAND_HEADER_BB_MAP_MAGIC_NUMBER)
	{
		free(bbMap);
		return NAND__RET_SUCCESS;	// Bad block map already built
	}

	memset(bbMap, 0x00, sizeof(InuStorageNandBbMapT));
	bbMap->magicNumber = NAND_HEADER_BB_MAP_MAGIC_NUMBER;

	for(offset = 0; offset < NAND_DRVG_getPartitionSize(); offset += NAND_DRVG_getBlockSize())
	{
		NAND_DRVG_isBadBlock(offset, &isBadBlock);
		if(isBadBlock > 0)
		{
			LOGG_PRINT(LOG_INFO_E, NULL, RED("Found bad block %d. Offset 0x%x.\n"), offset/NAND_DRVG_getBlockSize(), offset);
			bbCount++;
			bbMap->block[offset/NAND_DRVG_getBlockSize()] = 0x01;
		}
	}

	LOGG_PRINT(LOG_INFO_E, NULL, "Found %d bad Blocks. Saving result to the map\n", bbCount);

	//	preform read modify write operation
	NAND_STRGG_getSectionOffsetSize(NAND_SECTION_TYPE_HEADER, &ioWriteBb.offset, &ioWriteBb.length);
	ioWriteBb.buf = (UINT8*)malloc(ioWriteBb.length);

	ret = NAND_DRVG_read(&ioWriteBb);

	memcpy((ioWriteBb.buf + NAND_SECTION_HEADER_BB_MAP_OFFSET), bbMap, bbMapSize);
	
	ret = NAND_DRVG_write(&ioWriteBb);

	free(bbMap);
	
return ret;
}



static ERRG_codeE NAND_STRGG_scanForBadBlocks(void)
{
	
	ERRG_codeE ret = NAND__RET_SUCCESS;
	UINT32 offset, byteCnt = 0;
	UINT32 bbCount = 0, bbFound = 0;
	UINT8 isBadBlock = 0;
	NAND_DRVG_ioParamT erase, write, read;

	if(storageInitialized == 0)
	{
		return NAND__ERR_NOT_SUPPORTED;
	}
	
	write.buf = malloc(NAND_DRVG_getBlockSize());
	write.length = (NAND_DRVG_getBlockSize());
	read.buf  = malloc(NAND_DRVG_getBlockSize());
	read.length = (NAND_DRVG_getBlockSize());

	memset(write.buf, 0xAA, NAND_DRVG_getBlockSize());

	for(offset = 0; offset < NAND_DRVG_getPartitionSize(); offset += NAND_DRVG_getBlockSize())
	{
		
		if(((offset/NAND_DRVG_getBlockSize())%0x10) == 0)
			LOGG_PRINT(LOG_INFO_E, NULL, "Scanning block %d (offset 0x%x).\n", (offset/NAND_DRVG_getBlockSize()), offset);


		memset(read.buf, 0x00, NAND_DRVG_getBlockSize());
		write.offset = offset;
		read.offset  = offset;
		
		ret = NAND_DRVG_write(&write);
		if(ERRG_SUCCEEDED(ret))
		{
			//printf("|");
			ret = NAND_DRVG_read(&read);
			if(ERRG_SUCCEEDED(ret))
			{
				//printf(".");
				for(byteCnt = 0; byteCnt < NAND_DRVG_getBlockSize(); byteCnt++)
				{
					if(*(write.buf + byteCnt) != *(read.buf + byteCnt))
					{
						LOGG_PRINT(LOG_INFO_E, NULL, RED("Found bad block %d. Offset 0x%x.\n"), offset/NAND_DRVG_getBlockSize(), offset);
						bbFound = 1;
						bbCount++;
						
							
						NAND_DRVG_isBadBlock(offset, &isBadBlock);
						if(isBadBlock > 0)
						{
							LOGG_PRINT(LOG_INFO_E, NULL, "Bad block %d already marked.\n", offset/NAND_DRVG_getBlockSize());
						}
						else
						{
							ret = NAND_DRVG_setBadBlock(offset);
							if(ERRG_FAILED(ret))
							{
								return NAND__ERR_NOT_SUPPORTED;
							}
						}
					}
				}
			}
		}
		if(bbFound == 0)	//Current block is good
		{
			erase.offset = offset;
			erase.length = NAND_DRVG_getBlockSize();
			NAND_DRVG_erase(&erase);
		}
		else
		{
			bbFound = 0;
		}
		
	}
	
	LOGG_PRINT(LOG_INFO_E, NULL, " found %d Bad blocks \n", bbCount);
	return ret;
}
#endif


#ifdef __cplusplus
 }
#endif



