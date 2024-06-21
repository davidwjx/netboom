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

#ifdef __cplusplus
extern "C" {
#endif

#define BOOTROM_STORAGE_NU3000_MAGIC_NUMBER     (0xDA)
#define BOOTFIX_HEADER_MAGIC_NUMBER			    (0x87654321)
#define LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER	(0x87654321)
#define PRODUCTION_HEADER_MAGIC_NUMBER		    (0x87654322)
#define CALIBRATION_HEADER_MAGIC_NUMBER		    (0x87654323)
#define EXTENDED_HEADER_MAGIC_NUMBER		    (0x87654324)
#define BOOTFIX_HEADER_VERSION_NUMBER		    (0x100)
#define PRODUCTION_HEADER_VERSION_NUMBER	    (0x100)
#define CALIBRATION_HEADER_VERSION_NUMBER	    (0x100)
#define EXTENDED_HEADER_VERSION_NUMBER		    (0x100)
#define PRODUCTION_STRING_SIZE				    (16)

// Layout sections definition
#define BOOTFIX_DATA_SIZE					(127*1024)
#define BOOTFIX_HEADER_SIZE					(1*1024)
#define PRODUCTION_HEADER_START				(BOOTFIX_DATA_SIZE + BOOTFIX_HEADER_SIZE)
#define PRODUCTION_HEADER_SIZE				(512)
#define CALIBRATION_HEADER_START			(PRODUCTION_HEADER_START + PRODUCTION_HEADER_SIZE)
#define CALIBRATION_HEADER_SIZE				(512)
#define EXTENDED_HEADER_START				(CALIBRATION_HEADER_START + CALIBRATION_HEADER_SIZE)
#define EXTENDED_HEADER_SIZE				(2*1024)
#define CALIBRATION_SIZE                    (1024*1024)
#define BOOTSPL_SIZE                        (24*1024)
#define DTB_SIZE                            (16*1024)

typedef enum {
	BOOTROM_STORAGE_TYPE_USB_E = 0,
	BOOTROM_STORAGE_TYPE_BOOT_E
} BootromStorageTypeE;

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

typedef struct 
{
	unsigned int magicNumber;
    // version number of Header
    unsigned int versionNumber;	
	// allows to change bootfix strap value
	unsigned int strapValue;	
    // location of boot data
	unsigned int bootDataOffset;	
	unsigned int bootDataSize;	
} InuBootfixHeaderT;

typedef struct 
{   
	unsigned int magicNumber;
    // version number of Header
    unsigned int versionNumber;
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
	unsigned int magicNumber;
    // version number of Header
    unsigned int versionNumber;
    // location of calibration data
	unsigned int calibrationDataOffset;
	// length of calibration data
    unsigned int calibrationDataSize;
	// calibration data format
	unsigned int calibrationDataFormat;
	// burned revision - a counter that is increased when data is burned on flash.
	unsigned int  swRevision;	
} InuCalibrationHeaderT;

// use this extended header for extended data (e.g sensor control configuration, other parameters)
typedef struct 
{   
	unsigned int magicNumber;
    // version number of Header
    unsigned int versionNumber;
    // location of calibration data
	unsigned int extendedDataOffset;
	// length of calibration data
    unsigned int extendedDataSize;
	// calibration data format
	unsigned int extendedDataFormat;
	// Burned revision - a counter that is increased when data is burned on flash.
	unsigned int  swRevision;	
} InuExtendedHeaderT;


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
	BootromStorageHeaderT bootromHdr;
    unsigned char bootfixData[BOOTFIX_DATA_SIZE - sizeof(BootromStorageHeaderT)];	
	InuBootfixHeaderT bootfixHeader;
	unsigned char	reservedBootfix[BOOTFIX_HEADER_SIZE - sizeof(InuBootfixHeaderT)];
	InuProductionHeaderT  productionHeader;
	unsigned char	reservedProductionHeader[PRODUCTION_HEADER_SIZE - sizeof(InuProductionHeaderT)];
	InuCalibrationHeaderT  calibrationHeader;
	unsigned char	reservedCalibrationHeader[CALIBRATION_HEADER_SIZE - sizeof(InuCalibrationHeaderT)];	
	InuExtendedHeaderT  extendedHeader;
	unsigned char	extendedCalibrationHeader[EXTENDED_HEADER_SIZE - sizeof(InuExtendedHeaderT)];		
#if (defined _WIN32) ||(defined _WIN64)
}InuStorageLayoutT;
#pragma pack(pop)
#else
} __attribute__((__packed__)) InuStorageLayoutT;
#endif




// defines a union for legacy and new layout for the flash storage
typedef union 
{
	InuStorageLayoutT storagelayout;
	InuStorageLegacyLayoutT storageLegacyLayout;
} InuStorageU;

#ifdef __cplusplus
}
#endif

#endif // INU_STORAGE_LAYOUT_H 

