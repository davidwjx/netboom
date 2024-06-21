
/****************************************************************************
 *
 *   FileName: Nand_drv.h
 *
 *   Author: Dima S.
 *
 *   Date: 
 *
 *   Description: Raw NAND wrapper on Linux MTD device
 *   
 ****************************************************************************/
#ifndef __NAND_RAW_STORAGE_H__
#define __NAND_RAW_STORAGE_H__

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#ifdef __cplusplus
      extern "C" {
#endif

#include "err_defs.h"
#include "inu_types.h"
#include "inu_storage_layout.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
	UINT8* 						buf;
	UINT32 						length;
	InuStorageNandSectionTypE 	type;
	//	UINT32 offset;
}NAND_STRGG_ioParamT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
	 ERRG_codeE NAND_STRGG_init(void);
	 ERRG_codeE NAND_STRGG_readSection(NAND_STRGG_ioParamT *io);
	 ERRG_codeE NAND_STRGG_writeSection(NAND_STRGG_ioParamT *io);
	 ERRG_codeE NAND_STRGG_eraseSection(InuStorageNandSectionTypE type);
	 ERRG_codeE NAND_STRGG_readHeader(InuStorageNandHeaderT* header);
	 ERRG_codeE NAND_STRGG_getSectionOffsetSize(InuStorageNandSectionTypE type, UINT32 *offset, UINT32 *size);
	 ERRG_codeE NAND_STRGG_updateSection(NAND_STRGG_ioParamT *io, InuStorageNandImgVerT* imgVersion);
	 bool NAND_STRGG_isDevPresent(void);


#ifdef __cplusplus
}
#endif


#endif	//__NAND_RAW_STORAGE_H__





