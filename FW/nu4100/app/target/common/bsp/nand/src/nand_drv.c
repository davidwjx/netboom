/****************************************************************************
 *
 *   FileName: Nand_drv.c
 *
 *   Author: Dima S.
 *
 *   Date: 
 *
 *   Description: Raw NAND wrapper on Linux MTD device
 *   
 ****************************************************************************/


/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "inu_common.h"
#ifdef __cplusplus
      extern "C" {
#endif
#include <unistd.h>
#include <sys/types.h>

#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

#include "os_lyr.h"
#include "nand_drv.h"





/****************************************************************************
 ***************        L O C A L      D E F N I T I O N S  ***************
 ****************************************************************************/


/****************************************************************************
***************		   L O C A L	 T Y P E D E F S	  ***************
****************************************************************************/
typedef struct
{
   int    mtdFileDescr;
   UINT32 partitionSize;
   UINT32 eraseBlockSize;
   UINT32 writePageSize;
   UINT32 oobSize;

}NAND_DRVP_deviceDescT;

typedef NAND_DRVP_deviceDescT* NAND_DRVP_deviceH;

//mtd_info_t

// struct mtd_info_user {
//	__u8 type;
//	__u32 flags;
//	__u32 size;	/* Total size of the MTD */
//	__u32 erasesize;
//	__u32 writesize;
//	__u32 oobsize;	/* Amount of OOB data per block (e.g. 16) */
 /****************************************************************************
  ***************	   L O C A L		D A T A 		   ***************
  ****************************************************************************/
static NAND_DRVP_deviceDescT		nandDevice;
static NAND_DRVP_deviceH			nandH;


 
 /****************************************************************************
  ***************	   G L O B A L		D A T A 		   ***************
  ****************************************************************************/
#define NAND_DRVP_RAW_MTD_DEVICE			("/dev/mtd1")
//#define SLEEP_US__			(80000)
 /****************************************************************************
  ***************	   E X T E R N A L	  F U N C T I O N S 	 ***************
  ****************************************************************************/
 
 /****************************************************************************
  ***************	  P R E    D E F I N I T I O N		OF		***************
  ***************	  L O C A L 	  F U N C T I O N S 	 ***************
  ****************************************************************************/

  
  /****************************************************************************
   ***************	   L O C A L	   F U N C T I O N S	***************
   ****************************************************************************/
  
 /****************************************************************************
  ***************	  G L O B A L		 F U N C T I O N S	  **************
  ****************************************************************************/

ERRG_codeE NAND_DRVG_open(void)
{
	ERRG_codeE ret = NAND__RET_SUCCESS;
	mtd_info_t mtd_info;

	nandH = &nandDevice;

	memset((void*)nandH, 0x00, sizeof(NAND_DRVP_deviceDescT));
	
	nandH->mtdFileDescr = open(NAND_DRVP_RAW_MTD_DEVICE, (O_RDWR|O_SYNC|O_RSYNC));

	if(nandH->mtdFileDescr == -1)
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "Can't open NAND raw device %s\n", NAND_DRVP_RAW_MTD_DEVICE);
		return NAND__ERR_NOT_SUPPORTED;
	}

	ioctl(nandH->mtdFileDescr, MEMGETINFO, &mtd_info);
	
	if(nandH->mtdFileDescr == -1)
	{
		LOGG_PRINT(LOG_ERROR_E, NULL, "Can't send NAND MEMGETINFO ioctl. device %s\n", NAND_DRVP_RAW_MTD_DEVICE);
		return NAND__ERR_NOT_SUPPORTED;
	}
	nandH->partitionSize = mtd_info.size;
	nandH->eraseBlockSize = mtd_info.erasesize;
	nandH->writePageSize = mtd_info.writesize;
	nandH->oobSize = mtd_info.oobsize;

	printf("MTD type: %u\n", mtd_info.type);
	printf("MTD total size : %u Mbytes\n", mtd_info.size/1024/1024);
	printf("MTD erase size : %u Kbytes\n", mtd_info.erasesize/1024);
	printf("MTD OOB size :	 %u  bytes\n", mtd_info.oobsize);
	printf("MTD writesize :  %u  bytes\n", mtd_info.writesize);

	return ret;
}
 ERRG_codeE NAND_DRVG_read(NAND_DRVG_ioParamT *io)
 {
	 if(nandH->mtdFileDescr == 0)
	 {
		 return NAND__ERR_NOT_SUPPORTED;
	 }
	 
	 //LOGG_PRINT(LOG_INFO_E, NULL,"NAND_DRVG_read: Reading from offset %x, length %x\n", io->offset, io->length);
	 lseek(nandH->mtdFileDescr, io->offset, SEEK_SET);
	 read(nandH->mtdFileDescr, io->buf, io->length);
	 return NAND__RET_SUCCESS;
 }
 ERRG_codeE NAND_DRVG_write(NAND_DRVG_ioParamT *io)
 {
	 if(nandH->mtdFileDescr == 0)
	 {
		 return NAND__ERR_NOT_SUPPORTED;
	 }
	 //LOGG_PRINT(LOG_INFO_E, NULL,"NAND_DRVG_write: Writing to offset %x, length %x\n", io->offset, io->length);
	 lseek(nandH->mtdFileDescr, io->offset, SEEK_SET);
	 write(nandH->mtdFileDescr, io->buf, io->length);
	 return NAND__RET_SUCCESS;
 }
 ERRG_codeE NAND_DRVG_erase(NAND_DRVG_ioParamT *io)
 {
 	erase_info_t ei;
	
	if(nandH->mtdFileDescr == 0)
	{
		return NAND__ERR_NOT_SUPPORTED;
	}
	
	// check that alligned to blocksize
	io->length = (((io->length % nandH->eraseBlockSize) == 0) ?
		(io->length):
		((io->length / nandH->eraseBlockSize + 1) * nandH->eraseBlockSize));
	ei.length = io->length;	
	ei.start = io->offset;
	
	//LOGG_PRINT(LOG_INFO_E, NULL,"NAND_DRVG_erase: Erasing offset %x, length %x\n", io->offset, io->length);
	ioctl(nandH->mtdFileDescr, MEMUNLOCK, &ei);
	ioctl(nandH->mtdFileDescr, MEMERASE, &ei);
	return NAND__RET_SUCCESS;
 }

 
 UINT32 NAND_DRVG_getPageSize()
 {
 	if(nandH->mtdFileDescr != 0)
		return nandH->writePageSize;
	else
		return 0;
 }
 
 UINT32 NAND_DRVG_getBlockSize()
 {
 	if(nandH->mtdFileDescr != 0)
		return nandH->eraseBlockSize;
	else
		return 0;
 }

UINT32 NAND_DRVG_getPartitionSize()
{
   if(nandH->mtdFileDescr != 0)
	   return nandH->partitionSize;
   else
	   return 0;
}



ERRG_codeE NAND_DRVG_isBadBlock(UINT32 offset, UINT8* isBad)
{
	ERRG_codeE ret = NAND__RET_SUCCESS;
	int retVal;
	
	if(nandH->mtdFileDescr == 0)
	{
		return NAND__ERR_NOT_SUPPORTED;
	}
		
	retVal = ioctl(nandH->mtdFileDescr, MEMGETBADBLOCK, &offset);

	*isBad = (retVal > 0) ? (1) : (0);

	return ret;
}

ERRG_codeE NAND_DRVG_setBadBlock(UINT32 offset)
{
	
	ERRG_codeE ret = NAND__RET_SUCCESS;
	int retVal;
	
	if(nandH->mtdFileDescr == 0)
	{
		return NAND__ERR_NOT_SUPPORTED;
	}
		
	retVal = ioctl(nandH->mtdFileDescr, MEMGETBADBLOCK, &offset);

	ret = (retVal >= 0) ? (NAND__RET_SUCCESS) : (NAND__ERR_NOT_SUPPORTED);
	LOGG_PRINT(LOG_INFO_E, NULL,"Set Bad Block at offset %x. ret = %x\n", offset, retVal);

	return ret;
}




#ifdef __cplusplus
 }
#endif



