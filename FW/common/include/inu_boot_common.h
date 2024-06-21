
#ifndef _INU_BOOT_COMMON_H_
#define _INU_BOOT_COMMON_H_

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

typedef enum
{
	INU_BOOT_PARAM_CPUFREQ,
	INU_BOOT_PARAM_CPUFRAQ,
	INU_BOOT_PARAM_SYSFREQ,
	INU_BOOT_PARAM_DDRFREQ,
	INU_BOOT_PARAM_DSPFREQ,
	INU_BOOT_PARAM_AUDIOFREQ,
	INU_BOOT_MAX_PARAMS
}InuBootCommonParamsE;

typedef struct
{
    unsigned int magicNum;
    unsigned int param[INU_BOOT_MAX_PARAMS];
}INU_BOOT_boardConfigT;

#define BOOTSPLB0_C0_PARAM_MAGIC    (0xabcdef12)
#define INU_BOOT_CPUMHZFREQ_STR     "cpuMhzFreq"
#define INU_BOOT_CPUFRAQ_STR        "cpuFraq"
#define INU_BOOT_SYSMHZFREQ_STR     "sysMhzFreq"
#define INU_BOOT_DDRHZFREQ_STR      "ddrMhzFreq"
#define INU_BOOT_DSPMHZFREQ_STR     "dspMhzFreq"
#define INU_BOOT_AUDIOMHZFREQ_STR   "audioMhzFreq"
#define BOOTSPLB0_C0_PARAM_MAGIC    (0xabcdef12)


#define CRAM_START (0x1000000)
#define LRAM_START (0x2000000)
#define GME_SAVE_AND_RESTORE0_ADDR (0x8020104)
#define GME_SAVE_AND_RESTORE1_ADDR (0x8020108)
#define GME_SAVE_AND_RESTORE2_ADDR (0x802010C)
#define GME_SAVE_AND_RESTORE3_ADDR (0x8020110)
#define GME_SAVE_AND_RESTORE4_ADDR (0x8020114)
#define GME_SAVE_AND_RESTORE5_ADDR (0x8020118)
#define GME_SAVE_AND_RESTORE6_ADDR (0x802011C)
#define GME_SAVE_AND_RESTORE7_ADDR (0x8020120)

#define GME_CHIP_VERSION		   (0x80200B4)


#define LRAM_EXTRA_CODE_OFFSET			(0x40000)
// asuming extra code is less than 64K
// production data must be in LRAM  in order to survive suspend resume cycle in bootfix
#define LRAM_PRODUCTION_DATA_OFFSET		(0x48000)


#define BOOTFLASH_STATUS_READY				(0)
#define BOOTFLASH_STATUS_ACTIVE				(1)
#define BOOTFLASH_STATUS_FINISH				(2)
#define BOOTFLASH_STATUS_UNKNOWN			(0xFF)
#define BOOTFLASH_COMMAND_REG				(GME_SAVE_AND_RESTORE1_ADDR)

#define BOOTDETECT_COMMAND_REG				(GME_SAVE_AND_RESTORE1_ADDR)
#define BOOTDETECT_STATUS_FINISH			(2)
#define BOOTDETECT_LOAD_ADDRESS 			(LRAM_START + LRAM_EXTRA_CODE_OFFSET)
#define BOOTDETECT_PRODUCTION_DATA_ADDRESS 	(LRAM_START + LRAM_PRODUCTION_DATA_OFFSET)

#define BOOTSPL_MODE_BOOTFIX 		(0x87654321)
#define BOOTSPL_CMD_SPL_INIT 		(0x100)
#define BOOTSPL_CMD_SPL_INIT_DONE 	(0x101)


#define BOOTSPL_CMD_REG				(GME_SAVE_AND_RESTORE5_ADDR)
#define BOOTSPL_ADDRESS_REG			(GME_SAVE_AND_RESTORE3_ADDR)
#define BOOTSPL_MODE_REG			(GME_SAVE_AND_RESTORE4_ADDR)
#define BOOTSPL_LOAD_ADDRESS		(CRAM_START)
#define BOOTSPLB0_C0_LOAD_ADDRESS	(LRAM_START+0x30000)
#define BOOTSPLB0_C0_PARAM_ADDRESS	(LRAM_START+0x30000 - 0x1000)


#define	NU4000_A0_CHIP_VERSION		(0x40000000)
#define	NU4000_B0_CHIP_VERSION		(0x40000001)
#define	NU4000_C0_CHIP_VERSION		(0x40000002)
#define	NU4000_C1_CHIP_VERSION		(0x40000003)
#define	NU4100_CHIP_VERSION		   (0x41000000)
#define	NU3000_B0_CHIP_VERSION		(0x30000001)
#define	NU3000_A0_CHIP_VERSION		(0)

#endif
