/****************************************************************************
*
*   FileName: inu_storage_layout.h
*
*   Author:  Konstantin Sinyuk
*
*   Date: 15/03/2016
*
*   Description: Defines layout of Inuitive flash storage.
*				  Includes definition for Bootrom override region, production info,
calibration data and boot data
*
****************************************************************************/
#ifndef INU_STORAGE_LAYOUT_H
#define INU_STORAGE_LAYOUT_H

#include <time.h>
#include <inu_storage.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
	This file defines the layout of data on flash.
	Currently each section is flash block aligned for data write safety.
	Each section will start from the header (InuSectionHeaderT), sectionMetaData(if exists) and then section binary data.
	The only exception is BOOTFIX section that will have BootROM header, than binary data, then section header and then section MetaData  (at separate block)

	NOTE:
		The BOOTFIX section version is the version number for the whole flash layout
*/

// Legacy defines
#define BOOTROM_STORAGE_NU3000_MAGIC_NUMBER     (0xDA)
#define LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER   (0x87654321)

#define SECTION_START_BLOCK_DYNAMIC             (0xfffffffa)
#define SECTION_START_BLOCK_LAST                (0xfffffffb)
#define SECTION_START_BLOCK_BEFORE_LAST         (0xfffffffc)
#define SECTION_SIZE_DYNAMIC                    (0xfffffffd)

#define BOOTFIX_DATA_SIZE                       (127*1024)
// INU_STORAGE_SECTION_BOOTFIX
#define SECTION_BOOTFIX_MAGIC_NUMBER            (0x87654321)
#define SECTION_BOOTFIX_VERSION_NUMBER          (0x100)
#define SECTION_BOOTFIX_START_BLOCK             (0)
#define SECTION_BOOTFIX_SIZE_IN_BLOCKS          (2)
#define SECTION_BOOTFIX_AUTO_SIZE_INCREASE      (0)

// INU_STORAGE_SECTION_BOOTFIX_META
#define SECTION_BOOTFIX_META_MAGIC_NUMBER       (0x87654322)
#define SECTION_BOOTFIX_META_VERSION_NUMBER     (0x100)
#define SECTION_BOOTFIX_META_START_BLOCK        (3)
#define SECTION_BOOTFIX_META_SIZE_IN_BLOCKS     (1)
#define SECTION_BOOTFIX_META_AUTO_SIZE_INCREASE (0)

//INU_STORAGE_SECTION_PRODUCTION_DATA
#define SECTION_PRODUCTION_MAGIC_NUMBER         (0x87654323)
#define SECTION_PRODUCTION_VERSION_NUMBER       (0x100)
#define SECTION_PRODUCTION_START_BLOCK          (2)
#define SECTION_PRODUCTION_SIZE_IN_BLOCKS       (1)
#define SECTION_PRODUCTION_AUTO_SIZE_INCREASE   (0)

//INU_STORAGE_SECTION_CALIBRATION_DATA
#define SECTION_CALIBRATION_MAGIC_NUMBER        (0x87654324)
#define SECTION_CALIBRATION_VERSION_NUMBER      (0x100)
#define SECTION_CALIBRATION_START_BLOCK         (SECTION_START_BLOCK_LAST)
#define SECTION_CALIBRATION_SIZE_IN_BLOCKS      (SECTION_SIZE_DYNAMIC)
#define SECTION_CALIBRATION_AUTO_SIZE_INCREASE  (30)

//INU_STORAGE_SECTION_PM_DATA
#define SECTION_PM_DATA_MAGIC_NUMBER            (0x8765432D) // PM_DATA has a special MAGIC_NUMBER because it's a protectecd section for PM_CODE access only.
#define SECTION_PM_DATA_VERSION_NUMBER          (0x100)
#define SECTION_PM_DATA_START_BLOCK             (4)          // Section 4 shouldn't be availablr from all other apps but PM_CODE to write or read.
#define SECTION_PM_DATA_SIZE_IN_BLOCKS          (1)
#define SECTION_PM_DATA_AUTO_SIZE_INCREASE      (0)

//INU_STORAGE_SECTION_BOOTSPL_DATA
#define SECTION_BOOTSPL_MAGIC_NUMBER            (0x87654325)
#define SECTION_BOOTSPL_VERSION_NUMBER          (0x100)
#define SECTION_BOOTSPL_START_BLOCK             (7)
#define SECTION_BOOTSPL_SIZE_IN_BLOCKS          (SECTION_SIZE_DYNAMIC)
#define SECTION_BOOTSPL_AUTO_SIZE_INCREASE      (30)

//INU_STORAGE_SECTION_DTB
#define SECTION_DTB_MAGIC_NUMBER                (0x87654326)
#define SECTION_DTB_VERSION_NUMBER              (0x100)
#define SECTION_DTB_START_BLOCK                 (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_DTB_SIZE_IN_BLOCKS              (1)
#define SECTION_DTB_AUTO_SIZE_INCREASE          (0)

//INU_STORAGE_SECTION_KERNEL
#define SECTION_KERNEL_MAGIC_NUMBER             (0x87654327)
#define SECTION_KERNEL_VERSION_NUMBER           (0x100)
#define SECTION_KERNEL_START_BLOCK              (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_KERNEL_SIZE_IN_BLOCKS           (SECTION_SIZE_DYNAMIC)
#define SECTION_KERNEL_AUTO_SIZE_INCREASE       (10)

//INU_STORAGE_SECTION_APP
#define SECTION_APP_MAGIC_NUMBER                (0x87654328)
#define SECTION_APP_VERSION_NUMBER              (0x100)
#define SECTION_APP_START_BLOCK                 (SECTION_START_BLOCK_DYNAMIC)
#define SECTION_APP_SIZE_IN_BLOCKS              (SECTION_SIZE_DYNAMIC)
#define SECTION_APP_AUTO_SIZE_INCREASE          (0)

#define OLD_CALIB_BLOCK_OFFSET                  (3)

#define INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS    (0x00000001)
#define INU_STORAGE_LAYOUT__FLASH_FLAGS_BTBP                (0x00000002)
#define INU_STORAGE_LAYOUT__FLASH_FLAGS_FATAL               (0x00000004)

typedef struct
{
	unsigned int magicNumber;
	// version number of Header
	unsigned int versionNumber;
	// real size of data in section
	unsigned int sectionDataSize;
	// full section size
	unsigned int sectionSize;
	// section data format
	InuSectionFormatE sectionFormat;
	// Current date and time
	unsigned long long timestamp;
	// Version string for the current section (if supported)
	char versionString[VERSION_STRING_SIZE];
} InuSectionHeaderT;

typedef struct
{
	// when true, the bootspl will proceed to boot kernel and nu3000.zip from flash
	// should be overridable by push button from MANO board
	unsigned int isBootfromFlash;
	// allows to change chip default strap value
	unsigned int strapOverrideMask;
	unsigned int strapOverrideValue;
	unsigned int isQuadFlash;
	// Flash Flags are needed for FW Update procedure
	unsigned int flashFlags;
} InuBootfixHeaderT;

typedef struct
{
	// Unique ID of each sensor
	char serialNumber[PRODUCTION_STRING_SIZE];
	// General operational data
	char modelNumber[PRODUCTION_STRING_SIZE];
	char partNumber[PRODUCTION_STRING_SIZE];
	char sensorRevision[PRODUCTION_STRING_SIZE];
	// boot folder id - obtained from BootModelCross.csv production table
	unsigned int  bootId;
} InuProductionHeaderT;

typedef struct
{
	// FW Version ID
	unsigned int fwVersion;
	// Security Version ID
	unsigned char securityVersion;
	// Size of InuSecurityHeaderT must be 16 Bytes
	unsigned char unUsedChars[3];
} InuSecurityFooterT;

#define PRODUCTION_STRING_SIZE_LEGACY		(16)
typedef struct
{
	// Arbitrary ID that is used in order to differentiate sensors with old data
	unsigned int ID;
	// Version number of Header
	unsigned int VersionNumber;
	// Unique ID of each sensor
	char SerialNumber[PRODUCTION_STRING_SIZE_LEGACY];
	// Size of calibration data
	unsigned int CalibrationDataSize;
	// !!! Must keep all the above members in this order for backward compatibility !!!
	// Size of this header
	unsigned int HeaderSize;
	// General operational data
	char ModelNumber[PRODUCTION_STRING_SIZE_LEGACY];
	char PartNumber[PRODUCTION_STRING_SIZE_LEGACY];
	char SensorRevision[PRODUCTION_STRING_SIZE_LEGACY];
	// Burned revision - a counter that is increased when data is burned on flash.
	unsigned int SWRevision;
}InuStorageLegacyLayoutT;

#if (defined _WIN32) || (defined _WIN64)
#pragma pack(push,1)
#endif
typedef struct {
	unsigned char   magic;
	unsigned char   storage_size_in_kb;
	unsigned short  storage_page_size;
	unsigned char   storage_mode;
	unsigned char   storage_type;
	unsigned int  bootcode_load_address;
	unsigned int  bootcode_size;
#if (defined _WIN32) ||(defined _WIN64)
}BootromStorageHeaderT;
#pragma pack(pop)
#else
} __attribute__((__packed__)) BootromStorageHeaderT;
#endif

#if (defined _WIN32) || (defined _WIN64)
#pragma pack(push,1)
#endif
typedef struct {
	BootromStorageHeaderT bootromHdr;
	unsigned char bootfixData[BOOTFIX_DATA_SIZE - sizeof(BootromStorageHeaderT)];
	InuSectionHeaderT bootfixSectionHeader;
#if (defined _WIN32) ||(defined _WIN64)
}InuStorageLayoutT;
#pragma pack(pop)
#else
} __attribute__((__packed__)) InuStorageLayoutT;
#endif


// defines a union for legacy and new layout for the first block of flash storage
typedef union
{
	InuStorageLayoutT storagelayout;
	InuStorageLegacyLayoutT storageLegacyLayout;
} InuStorageU;

#ifdef __cplusplus
}
#endif

#endif // INU_STORAGE_LAYOUT_H

