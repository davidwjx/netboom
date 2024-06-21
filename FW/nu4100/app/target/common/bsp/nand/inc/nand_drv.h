
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
#ifndef __NAND_DRIVER_H__
#define __NAND_DRIVER_H__

/****************************************************************************
 ***************      I N C L U D E   F I L E S                 *************
 ****************************************************************************/
#ifdef __cplusplus
      extern "C" {
#endif

#include "err_defs.h"

#include "inu_types.h"

/****************************************************************************
 ***************     G L O B A L        D E F N I T I O N S    **************
 ****************************************************************************/


/****************************************************************************
 ***************      G L O B A L         T Y P E D E F S     ***************
 ****************************************************************************/

typedef struct
{
	UINT8* buf;
	UINT32 offset;
	UINT32 length;
	UINT32 *ret_len;
} NAND_DRVG_ioParamT;

/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/
ERRG_codeE NAND_DRVG_open(void);
ERRG_codeE NAND_DRVG_read(NAND_DRVG_ioParamT *io);
ERRG_codeE NAND_DRVG_write(NAND_DRVG_ioParamT *io);
ERRG_codeE NAND_DRVG_erase(NAND_DRVG_ioParamT *io);

ERRG_codeE NAND_DRVG_isBadBlock(UINT32 offset, UINT8* isBad);
ERRG_codeE NAND_DRVG_setBadBlock(UINT32 offset);


UINT32 NAND_DRVG_getPageSize();
UINT32 NAND_DRVG_getBlockSize();
UINT32 NAND_DRVG_getPartitionSize();


#ifdef __cplusplus
}
#endif


#endif	//__NAND_DRIVER_H__





