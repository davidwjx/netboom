#include "common.h"
#include "board.h"
#include "spl.h"
#include "nu4000_c0_ddrp_regs.h"
#include "nu4000_c0_gme_regs.h"
#include "nu4000_c0_ddrc_mp_regs.h"
#include "nu4000_c0_ddrc_regs.h"

#ifndef NU4100_BOOTSPL_SRC_PM_H_
#define NU4100_BOOTSPL_SRC_PM_H_

#ifdef PM_DEBUG
#define pm_log(fmt, ...)	\
	do { \
           debug_printf(fmt, ##__VA_ARGS__); \
    }while(0)
#else
	#define pm_log(fmt, ...)
#endif

/* For accurate delay */
#define MACRO_CMB( A , B)           A##B
#define M_RPT(__N, __macro)         MACRO_CMB(M_RPT, __N)(__macro)

#define M_RPT0(__macro)
#define M_RPT1(__macro)             M_RPT0(__macro)   __macro(0)
#define M_RPT2(__macro)             M_RPT1(__macro)   __macro(1)
#define M_RPT3(__macro)             M_RPT2(__macro)   __macro(2)
#define M_RPT4(__macro)             M_RPT3(__macro)   __macro(3)
#define M_RPT5(__macro)             M_RPT4(__macro)   __macro(4)
#define M_RPT6(__macro)             M_RPT5(__macro)   __macro(5)
#define M_RPT7(__macro)             M_RPT6(__macro)   __macro(6)
#define M_RPT8(__macro)             M_RPT7(__macro)   __macro(7)
#define M_RPT9(__macro)             M_RPT8(__macro)   __macro(8)
#define M_RPT10(__macro)            M_RPT9(__macro)  __macro(9)
#define M_RPT11(__macro)            M_RPT10(__macro)   __macro(10)
#define M_RPT12(__macro)            M_RPT11(__macro)   __macro(11)
#define M_RPT13(__macro)            M_RPT12(__macro)   __macro(12)
#define M_RPT14(__macro)            M_RPT13(__macro)   __macro(13)
#define M_RPT15(__macro)            M_RPT14(__macro)   __macro(14)
#define M_RPT16(__macro)            M_RPT15(__macro)   __macro(15)
#define M_RPT17(__macro)            M_RPT16(__macro)   __macro(16)
#define M_RPT18(__macro)            M_RPT17(__macro)   __macro(17)
#define M_RPT19(__macro)            M_RPT18(__macro)   __macro(18)
#define M_RPT20(__macro)            M_RPT19(__macro)   __macro(19)
#define M_RPT21(__macro)            M_RPT20(__macro)   __macro(20)
#define M_RPT22(__macro)            M_RPT21(__macro)   __macro(21)
#define M_RPT23(__macro)            M_RPT22(__macro)   __macro(22)
#define M_RPT24(__macro)            M_RPT23(__macro)   __macro(23)
#define M_RPT25(__macro)            M_RPT24(__macro)   __macro(24)
#define M_RPT26(__macro)            M_RPT25(__macro)   __macro(25)
#define M_RPT27(__macro)            M_RPT26(__macro)   __macro(26)
#define M_RPT28(__macro)            M_RPT27(__macro)   __macro(27)
#define M_RPT29(__macro)            M_RPT28(__macro)   __macro(28)
#define M_RPT30(__macro)            M_RPT29(__macro)   __macro(29)
#define M_RPT31(__macro)            M_RPT30(__macro)   __macro(30)
#define M_RPT32(__macro)            M_RPT31(__macro)   __macro(31)
#define M_RPT33(__macro)            M_RPT32(__macro)   __macro(32)
#define M_RPT34(__macro)            M_RPT33(__macro)   __macro(33)
#define M_RPT35(__macro)            M_RPT34(__macro)   __macro(34)
#define M_RPT36(__macro)            M_RPT35(__macro)   __macro(35)
#define M_RPT37(__macro)            M_RPT36(__macro)   __macro(36)
#define M_RPT38(__macro)            M_RPT37(__macro)   __macro(37)
#define M_RPT39(__macro)            M_RPT38(__macro)   __macro(38)
#define M_RPT40(__macro)            M_RPT39(__macro)   __macro(39)
#define M_RPT41(__macro)            M_RPT40(__macro)   __macro(40)
#define M_RPT42(__macro)            M_RPT41(__macro)   __macro(41)
#define M_RPT43(__macro)            M_RPT42(__macro)   __macro(42)
#define M_RPT44(__macro)            M_RPT43(__macro)   __macro(43)
#define M_RPT45(__macro)            M_RPT44(__macro)   __macro(44)
#define M_RPT46(__macro)            M_RPT45(__macro)   __macro(45)
#define M_RPT47(__macro)            M_RPT46(__macro)   __macro(46)
#define M_RPT48(__macro)            M_RPT47(__macro)   __macro(47)
#define M_RPT49(__macro)            M_RPT48(__macro)   __macro(48)
#define M_RPT50(__macro)            M_RPT49(__macro)   __macro(49)
#define M_RPT51(__macro)            M_RPT50(__macro)   __macro(50)
#define M_RPT52(__macro)            M_RPT51(__macro)   __macro(51)
#define M_RPT53(__macro)            M_RPT52(__macro)   __macro(52)
#define M_RPT54(__macro)            M_RPT53(__macro)   __macro(53)
#define M_RPT55(__macro)            M_RPT54(__macro)   __macro(54)
#define M_RPT56(__macro)            M_RPT55(__macro)   __macro(55)
#define M_RPT57(__macro)            M_RPT56(__macro)   __macro(56)
#define M_RPT58(__macro)            M_RPT57(__macro)   __macro(57)
#define M_RPT59(__macro)            M_RPT58(__macro)   __macro(58)
#define M_RPT60(__macro)            M_RPT59(__macro)   __macro(59)

#define NOP(__N)                 	__asm ("nop");


typedef void (*kernel_cpu_resume_arg_t)(void) __attribute__ ((noreturn));

#define PM_MODE0					0

#define PM_NORMAL_BOOT				99
#define PM_POWERD_BOOT				98
#define PM_INITIALISED				0xCAFECAFE

#define PM_DDR_RESUME_IN_KERNEL		(0x64000000 - 4)

#define BLOCKSIZE					0x10000
#define LRAM_OFFSET_FOR_TRAINING	(SECTION_PM_DATA_START_BLOCK * BLOCKSIZE)
#define DEVICE_STATUS_REGISTER 		0x900C70C
#define I2C_4_BASE_ADDR				(0x08084000)

#define DDR_TRAINING_BACKUP(training_data_ptr, reg)		\
		*training_data_ptr = *(reg); \
		training_data_ptr++;

#define DDR_TRAINING_START			0xcafe
#define PM_DDR_TRAIN_MAGIC_OFF		0
#define PM_DDR_TRAIN_FREQ_OFF		PM_DDR_TRAIN_MAGIC_OFF + 1
#define PM_DDR_TRAIN_PAYLOAD_OFF	PM_DDR_TRAIN_MAGIC_OFF + 2


#define SPI_PAGE_SIZE 256

struct pm_configuration {
	void *tear_down;
	void *bootrom_resume;
	int wakeup;
	int status;		/* Should be 0 otherwise PM is not allowed */
	int res;
};

struct pm_data_header {
	UINT16 pm_magic;
	UINT16 ddr_freq;
};


/* EXTERNS */

extern UINT16 *__pm_training_start__;
extern UINT32 *__pm_config__;
extern UINT32 *__pm_config_address__;

extern void pm_init(void);
extern int pm_is_pd_boot(void);
extern void spl_pm(void);
extern void ddr_data_training_collection(UINT16 *training_data_ptr, UINT16 *ddr_freq);
extern int spl_training(UINT16 *training_data_ptr, UINT16 current_freq);
extern void ddr_data_training_restore(void);
extern void ddr_power_down_resume(void);
extern int pm_boot_from_pd(void);

extern void phyinit_F_loadDMEM (unsigned int freq);
extern void phyinit_C_initPhyConfig (unsigned int freq);
extern void ddrm_phy_init_srpd_seq(int freq) ;
extern void ddrm_umctl_init_srpd_seq(int freq);

#endif /* NU4100_BOOTSPL_SRC_PM_H_ */
