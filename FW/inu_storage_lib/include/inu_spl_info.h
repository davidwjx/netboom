
#ifndef _INU_SPL_INFO_H_
#define _INU_SPL_INFO_H_

/********************************************************/
/*   Shared interface between bootspl and inu_usb_lib   */
/********************************************************/
typedef struct
{	
	unsigned int invokeLinuxBootAddress;
	unsigned int kernelStartAddress;
	unsigned int dtbStartAddress;
	unsigned int appStartAddress;
	unsigned int initramfsStartAddress;
   unsigned int cnnStartAddress;
   unsigned int cevaStartAddress;
} InuBootsplInfoHeaderT;

#define BOOTSPL_MODE_BOOTFIX 		(0x87654321)
#define BOOTSPL_CMD_SPL_INIT 		(0x100)
#define BOOTSPL_CMD_SPL_INIT_DONE 	(0x101)

#define CRAM_START (0x1000000)
#define LRAM_START (0x2000000)
#define CRAM_FREE_OFFSET (0x10000)

#define GME_SAVE_AND_RESTORE0_ADDR (0x8020108)
#define GME_SAVE_AND_RESTORE1_ADDR (0x8020108)
#define GME_SAVE_AND_RESTORE2_ADDR (0x802010C)
#define GME_SAVE_AND_RESTORE3_ADDR (0x8020110)
#define GME_SAVE_AND_RESTORE4_ADDR (0x8020114)
#define GME_SAVE_AND_RESTORE5_ADDR (0x8020118)
#define GME_CHIP_VERSION		   (0x80200B4)

#define	NU3000_B0_CHIP_VERSION		(0x30000001)
#define	NU3000_A0_CHIP_VERSION		(0)

#define BOOTSPL_CMD_REG				(GME_SAVE_AND_RESTORE5_ADDR)
#define BOOTSPL_ADDRESS_REG			(GME_SAVE_AND_RESTORE3_ADDR)
#define BOOTSPL_MODE_REG			(GME_SAVE_AND_RESTORE4_ADDR)
#define BOOTSPL_LOAD_ADDRESS		(CRAM_START)



#endif
