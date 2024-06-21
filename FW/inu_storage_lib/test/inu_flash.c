#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

#include "inu_usb.h"
#include "inu_storage.h"
#include "inu_storage_layout.h"
#include "inu_host_utils.h"
#if (defined _WIN32) || (defined _WIN64)
#include <windows.h>
#include <conio.h>
#include "getopt.h"
#elif defined(__linux__)
#include <unistd.h>
#endif
//#define _DEBUG
#define VERSION_STR   "1.0"
#define MAX_LINE_SIZE 512
#define MAX_READ_SIZE 0x20000
#define MAX_FILE_SIZE 0x4000000 // set max file size to 64M
#define DEFAULT_SPI_SPEED 60000000
//#define DEFAULT_SPI_SPEED 80000
#define NUM_CONFIG_FILE_SECTIONS 4

#define BOOTSPL_OFFSET_ACTIVE 0
#define BOOTSPL_OFFSET_BACKUP 1

InuUsbParamsT inuParams;
InuProductionDataT prodData;
InuStorageLayoutT inuStorageLayout;

InuSectionDataT inuSectionDataT;
InuSectionDataT *pInuSectionDataT = &inuSectionDataT;

unsigned char buffer[MAX_READ_SIZE];
unsigned char tmpBuff[MAX_FILE_SIZE];


unsigned int config_file_sections[NUM_CONFIG_FILE_SECTIONS] = {
										INU_STORAGE_SECTION_BOOTSPL,
										INU_STORAGE_SECTION_DTB,
										INU_STORAGE_SECTION_KERNEL,
										INU_STORAGE_SECTION_APP };

unsigned char config_file_section_name[NUM_CONFIG_FILE_SECTIONS][256];
static const unsigned long long epoch = ((unsigned long long)116444736000000000ULL);

static void print_usage(char *my_name)
{
	printf("%s <path_to_boot_directory>\n", my_name);
}


int hotplug_callback(InuUsbStatusE status)
{
	if (status == INU_USB_EVT_INSERTION)
		printf("inu_usb_test: hotplug callback: NU3000 raw insertion event\n");
	else if (status == INU_USB_EVT_BOOT_COMPLETE)
		printf("inu_usb_test: hotplug callback: NU3000 boot complete event\n");
	else if (status == INU_USB_EVT_REMOVAL)
		printf("inu_usb_test: hotplug callback: NU3000 removal event\n");
	else
		printf("inu_usb_test: hotplug callback: NU3000 unknown event\n");
	return 0;
}

int copyBuffToFile(char *filePath, unsigned char *dataPtr, unsigned int writeSize)
{
	FILE *fd;
	int rc = 0;

	fd = fopen(filePath, "wb+");
	if (fd == NULL)
	{
		printf("Can't open file %s\n", filePath);
		return -1;
	}
	rc = (int)fwrite(dataPtr,  1, writeSize,fd);
	fclose(fd);


	return rc;
}

int copyFileToBuff(char *filePath, unsigned char *dataPtr, unsigned int *readSize)
{
	FILE *fd;
	int rc = 0;

	fd = fopen(filePath, "rb");
	if (fd == NULL)
	{
		printf("Can't open file %s %d %s\n", filePath, errno, strerror(errno));
		return -1;
	}
	fseek(fd, 0, SEEK_END);
	*readSize = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	printf("size %d\n", *readSize);
	if (*readSize <= MAX_FILE_SIZE)
		*readSize = (unsigned int)fread(dataPtr, 1, *readSize, fd);
	else printf("***file size  0x%x bigger than max file size (0x%x)\n", *readSize,MAX_FILE_SIZE);
	fclose(fd);


	return rc;
}

int copyFileToDynamicBuff(char *filePath, unsigned char **dataPtr, unsigned int *readSize)
{
	FILE *fd;
	int rc = 0;
	char *tmp = NULL;

	fd = fopen(filePath, "rb");
	if (fd == NULL)
	{
		printf("Can't open file %s\n", filePath);
		return -1;
	}
	fseek(fd, 0, SEEK_END);
	*readSize = ftell(fd);

	tmp = (char*)malloc(*readSize);
	if(tmp == NULL)
	{
		printf("Failed to allocate buffer\n");
		fclose(fd);
		return -1;
	}
	fseek(fd, 0, SEEK_SET);
	printf("size %d\n", *readSize);
	*readSize = (unsigned int)fread(tmp, 1, *readSize, fd);
	fclose(fd);

	*dataPtr = tmp;

	return rc;
}

#ifdef _DEBUG
void printFlashData(unsigned int offset, unsigned int size)
{
	unsigned int ind, transfered, ret;

	ret = inu_storage_read_raw(0, offset, buffer, size, &transfered);
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return;
	for (ind = 0; ind < size / 4 + 1; ind++)
	{
		if (ind % 8 == 0)
			printf("\n0x%6.6x:", offset + ind * 4);
		printf("0x%8.8x ", ((unsigned long *)buffer)[ind]);

	}
}

void copyFlashDataToFile(unsigned int offset, unsigned int size)
{
	unsigned int transfered, ret;
	unsigned char fileName[32];

	ret = inu_storage_read_raw(0, offset, tmpBuff, size, &transfered);
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return;
	sprintf(fileName,"FlashBin_0x%x_0x%x",offset,size);
	if (size <= MAX_FILE_SIZE)
		copyBuffToFile(fileName, tmpBuff, size);
	else printf("size is too big\n");
}


#endif

void inu_display_file_data(InuSectionDataT *pInuSectionDataT)
{
	unsigned int ind;
	printf("\n-------------section data-------------------\n");
	printf("%-20.20s      %d\n", "DataFormat", pInuSectionDataT->sectionDataFormat);
	printf("%-20.20s      %d\n", "DataSize", pInuSectionDataT->sectionDataSize);
	printf("%-20.20s      %llx\n", "timestamp", pInuSectionDataT->timestamp);
	if (pInuSectionDataT->sectionDataSize < MAX_FILE_SIZE)
	{
		for (ind = 0; ind < pInuSectionDataT->sectionDataSize; ind++)
		{
			if (ind % 16 == 0)
				printf("\n%3.3x)", ind);
			printf("%2.2x ", pInuSectionDataT->sectionDataBuffer[ind]);
		}
	}
	printf("\n------------------------------------------------------\n");

}

void inu_display_prd(InuProductionDataT *prodData)
{
	char copyStr[PRODUCTION_EXT_STRING_SIZE + 1];
	memset(copyStr, 0, sizeof(copyStr));
	printf("\n-------------production data-------------------\n");
	strncpy(copyStr, prodData->serialNumber, PRODUCTION_EXT_STRING_SIZE);
	printf("%-20.20s     %s\n", "serialNumber", copyStr);
	strncpy(copyStr, prodData->modelNumber, PRODUCTION_STRING_SIZE);
	printf("%-20.20s     %s\n", "modelNumber", copyStr);
	strncpy(copyStr, prodData->partNumber, PRODUCTION_STRING_SIZE);
	printf("%-20.20s     %s\n", "partNumber", copyStr);
	strncpy(copyStr, prodData->sensorRevision, PRODUCTION_STRING_SIZE);
	printf("%-20.20s     %s\n", "sensorRevision", copyStr);
	printf("%-20.20s     %d\n", "bootid", prodData->bootId);
	printf("%-20.20s     0x%x\n", "chipid", prodData->chipHardwareVersion);
	printf("------------------------------------------------------\n");
}
void inu_display_hdr(InuSectionDataT *pInuSectionDataT)
{
	char versionString[VERSION_STRING_SIZE] = { 0 };

	strncpy(versionString, pInuSectionDataT->versionString, VERSION_STRING_SIZE - 1);
	printf("format 0x%x\n", pInuSectionDataT->sectionDataFormat);
	printf("datasize 0x%x\n", pInuSectionDataT->sectionDataSize);
	printf("timestamp  0x%llx %s\n", pInuSectionDataT->timestamp,ctime(&pInuSectionDataT->timestamp));
	printf("versionString  %s\n", versionString);

}


/*
* timezone information is stored outside the kernel so tzp isn't used anymore.
*
* Note: this function is not for Win32 high precision timing purpose. See
* elapsed_time().
*/
int winGettimeofday(long *tv_sec)
{
#if (defined _WIN32) || (defined _WIN64)
	FILETIME    file_time;
	SYSTEMTIME  system_time;
	ULARGE_INTEGER ularge;
	typedef struct stimeval
	{
		long tv_sec;
		long tv_usec;
	} timeval;
	struct timeval tp;


	GetSystemTime(&system_time);
	SystemTimeToFileTime(&system_time, &file_time);
	ularge.LowPart = file_time.dwLowDateTime;
	ularge.HighPart = file_time.dwHighDateTime;

	tp.tv_sec = (long)((ularge.QuadPart - epoch) / 10000000L);
	tp.tv_usec = (long)(system_time.wMilliseconds * 1000);
	*tv_sec = tp.tv_sec;
#else
	*tv_sec = 0;
#endif
	return 0;
}

int inu_flash_read_config(char *config_file)
{
	FILE *fd;
	int rc = 0, ind = 0;
	char buff[256];
	char *string=buff;

	fd = fopen(config_file, "rb");
	if (fd == NULL)
	{
		printf("Can't open file %s\n", config_file);
		return -1;
	}
	while (fgets(string, sizeof(buff), fd) != NULL)
	{   
		if (strlen(string) > 2)
		{
			if (string[strlen(string) - 1] == '\n')
				string[strlen(string) - 2] = 0;
			while (*string == ' ')
				string++;
			strcpy(config_file_section_name[ind],string);
			if (ind == NUM_CONFIG_FILE_SECTIONS)
			{
				printf("too many files\n");
				return -1;
			}
			if (fopen(config_file_section_name[ind], "rb") != NULL)
				ind++;
			else
			{
				printf("can't open file %s\n",config_file_section_name[ind]);
				return -1;
			}
		}
	}
	if (ind != NUM_CONFIG_FILE_SECTIONS)
	{
		return -1;
	}
	return 0;
}


int inu_flash_run_config(char *config_file)
{

	int rc = 0, ind = 0, sectionType;
	unsigned int size;
	long tv_sec;


	winGettimeofday(&tv_sec);
//	printf("time %s %x %x %x\n", ctime(&mytime),sizeof(tp),sizeof(pInuSectionDataT->timestamp), sizeof(tp.tv_sec));
//	printf("SystemTime 0x%x\n",sizeof(SystemTime));
	if (inu_flash_read_config(config_file) != -1)
	{
		for (ind = 0; ind < NUM_CONFIG_FILE_SECTIONS; ind++)
		{
			memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
			pInuSectionDataT->sectionDataBuffer = tmpBuff;
			pInuSectionDataT->timestamp = tv_sec;
			strncpy(pInuSectionDataT->versionString, "1.01", sizeof(pInuSectionDataT->versionString));
			sectionType = config_file_sections[ind];
			if (sectionType == INU_STORAGE_SECTION_APP)
				pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
			else pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
			rc = copyFileToBuff(config_file_section_name[ind], pInuSectionDataT->sectionDataBuffer, &size);
			if ( rc != -1)
			{
				pInuSectionDataT->sectionDataSize = size;
				rc = inu_storage_write_section_data(sectionType, pInuSectionDataT);
				if (rc != INU_STORAGE_ERR_SUCCESS)
				{
					printf("Fail writing section %d",sectionType);
					return rc;
				}
			}
			else return rc;
		}
	}
	return rc;
}


int main(int argc, char* argv[])
{

	int ind, retCode;
	unsigned int size = 32;
	unsigned int firstWord, Param = 0, Param1 = 0, Param2=0, Param3=0,partition, section;
	unsigned int writeProd = 0, writeCalib = 0, argcN=argc;
	char  *write_file = NULL;
	char **argvN = argv;
	InuStorageSectionTypeE sectionType;
	InuProductionDataT *prodData = (InuProductionDataT *)tmpBuff;
	InuFlashDataT inuFlashData;
	InuNandDataT  inuNandData;
	int tv_sec;
	unsigned int transfered, chipVersion=0;
	char *workPath=NULL;
	unsigned char sectionOutputFile = 0;
	SpiParamsT spiParams;
	unsigned int buff[0x80];
	unsigned int bootsplOffset = 0;

    char *sectionOutputFileName[INU_STORAGE_SECTION_APP + 1] = {  "SECTION_BOOTFIX_file" ,
                                                                 "SECTION_METADATA_file",
                                                                 "SECTION_PRODUCTION_DATA ",
																 "SECTION_CALIBRATION_DATA",
																 "SECTION_DYN_CALIBRATION_DATA",
																 "SECTION_BOOTSPL",
                                                                 "SECTION_DTB",
                                                                 "SECTION_KERNEL",
                                                                 "SECTION_ROOTFS",
                                                                 "SECTION_CEVA",
                                                                 "SECTION_CNN_LD",
                                                                 "SECTION_CNN",
                                                                 "SECTION_APP"
	                                                           };

#ifdef USE_OLD_PUBLIC_KEY
	unsigned char secure_boot_public_key[] = {
		/* the Qx value */
		0x0e, 0xa5, 0x42, 0xfc,
		0x41, 0xfb, 0x72, 0x27,
		0x10, 0x35, 0x04, 0xba,
		0xe5, 0x4a, 0x9f, 0xf3,
		0x16, 0xbd, 0x1d, 0xf5,
		0x42, 0x37, 0x6a, 0x67,
		0xc2, 0x31, 0x9e, 0x29,
		0xeb, 0x63, 0xbd, 0x49,

		/* the Qy value */
		0x35, 0x37, 0x15, 0xad,
		0x5d, 0x43, 0x95, 0x40,
		0x97, 0xea, 0xaf, 0x99,
		0x6c, 0x76, 0x37, 0x80,
		0x45, 0xc9, 0xe4, 0xaa,
		0xaf, 0x7a, 0x35, 0x64,
		0xa0, 0x56, 0x3e, 0x8c,
		0x5a, 0x88, 0x74, 0xed
	};

UINT8 imageDecryption[] =
	{
		0xb8 ,0x2c ,0xa2 ,0x2a ,0x3f ,0xfa ,0x47 ,0xb9 ,
		0xd0 ,0xbc ,0xf6 ,0x9c ,0x6f ,0xec ,0xbb ,0xab
	};
#endif //USE_OLD_PUBLIC_KEY

//#define USE_HP_KEY


#ifdef USE_HP_KEY
unsigned char secure_boot_public_key[] = { //HP Official Public Key

// C array containing the public key.
	// the Qx value
	0x7b, 0x69, 0x91, 0x44,
	0x3c, 0x1c, 0x4d, 0xa4,
	0x8d, 0xda, 0x2d, 0x23,
	0xbe, 0xac, 0x8f, 0x27,
	0x01, 0x47, 0x5c, 0x6e,
	0x19, 0x8a, 0x94, 0xde,
	0xc1, 0x3a, 0x38, 0xfe,
	0xa1, 0xc1, 0x77, 0x88,

	// the Qy value
	0x5d, 0xc7, 0x83, 0xc7,
	0x8f, 0xea, 0xe5, 0x68,
	0xbb, 0x6e, 0x8f, 0xf7,
	0x3a, 0x29, 0xfd, 0x89,
	0xb1, 0x4b, 0x09, 0xd7,
	0xba, 0xee, 0xdd, 0x6a,
	0x2b, 0xce, 0x59, 0x94,
	0x39, 0x04, 0xd7, 0xec
};
#else //USE_HP_KEY
unsigned char secure_boot_public_key[] = { //DM - Inuitive public test key

	/* the Qx value */
	0x41, 0x47, 0xa3, 0xe1,
	0xc7, 0x7f, 0x07, 0xcf,
	0x89, 0x38, 0xe1, 0x97,
	0x15, 0x51, 0xd9, 0xbc,
	0x86, 0xc8, 0x20, 0x58,
	0xfa, 0xf2, 0x4c, 0x5a,
	0x72, 0x9b, 0x5c, 0x3e,
	0xfd, 0xea, 0x15, 0xc5,

	/* the Qy value */
	0x47, 0xdc, 0x90, 0xef,
	0x7b, 0xde, 0x72, 0xc7,
	0xa1, 0xfb, 0xac, 0x5b,
	0xfb, 0x48, 0xf0, 0x7b,
	0x15, 0x4e, 0x4b, 0x50,
	0x06, 0xf6, 0x4a, 0xc9,
	0x44, 0x84, 0x16, 0xf6,
	0x23, 0xbc, 0x8d, 0x34
};
#endif //USE_HP_KEY


	winGettimeofday(&tv_sec);
	/* C array containing the public key. */
 #ifdef _DEBUG
	unsigned int Data;
#endif
	printf("main: start flash app...\n");
	if (argc <= 1)
	{
#ifdef _DEBUG
		printf("Usage: inu_flash.exe \n"
			"\t -A production serial(string)\n"
			"\t -B production model number(string)\n"
			"\t -C production part number(string)\n"
			"\t -D production sensor revison(string)\n"
			"\t -E production boot id(decimal)\n"
			"\t -P <address> <size> read address\n"
			"\t -W <address> <data> write to address\n"
			"\t -Q <address> <size> delete sectors starting from address\n"
			"\t -E production boot id(decimal)\n"
			"\t -f burn flash using config file (with full path)\n"
			"\t -G burn calibration file (with full path)\n"
			"\t -h burn dynamic calibration file (with full path)\n"
			"\t -H write flash efuse\n"
			"\t -I <load from flash(0,1)><quad SPI(0,1)> \n"
			"\t -J boofix loader file(with full path)\n"
			"\t -K <optional Partition Numer(0,1)>[(4-SPL 5-DTB 6-KERNEl 7-rootfs 8-CEVA 9-CNN_LD 10-CNN 11-APP)] [file to burn(with full path)]\n"
			"\t -L [ATTiny file to burn(with full path)]\n"
			"\t -S show components\n"
			"\t -s save flash data to file <offset> <size>\n"
			"\t -T [freq for SPI(use -T before any command . ex -T 8000000 -Z 0)][frequeny][secondary flash 0/1][spiNum 0/1/2/3][chipSelect 0/1/2/3]\n"
			"\t -V force chip version (3000 burning 4000 flash)<NU4000_B0_CHIP_VERSION - 0x40000001 ...>\n"
			"\t -w <file> <offset> -- write file to flash offset\n"
			"\t -v <file> <offset> -- verify file written to this offset\n"
			"\t -Z [print(0 -bootfix 1-metadata  2-production 3- calibration 4- dynamic calibration 5-SPL 6-DTB 7-KERNEl 8-rootfs 9-CEVA 10-CNN_LD 11-CNN 12-APP)] [save - for saving on disk]\n"
			"\t -p burn NAND flash with UBI image file. E.g. -P ubi_for_flash.img\n"
			"\t -r read NAND flash using start address and size. Result saved into file\n"
			"\t -e erase NAND flash using start address and size.\n"
			"\t dry <directory> <sizeInMega> exe. dry C:\\temp\\ 8 -J bootfixFile\n"
			"\t id <chipId> exa. id 0x40000002 -J bootfixFile\n"
			"\t -u update NAND flash raw section. -u <partition> <section> <file>, where partition - (0 - factory, 1 - upgrade),\n"
			"\t\t  section - (1-SPL, 2-kernel, 3-dtb, 4-ceva, 5-ev62_boot, 6-ev62_app, 7-initramfs, 8-application)\n"
		);
#else 
		printf("Usage: inu_flash.exe \n"
			"\t -f burn flash using config file (with full path)\n"
			"\t -Z print(0 production  1- calibration 2-SPL 3-DTB 4-KERNEl 5-APP)\n"
			"\t -I bootfix strap(hex)\n"
			"\t dry <directory> <sizeInMega> exe. dry C:\\temp\\ 8 -J bootfixFile\n"
			);
#endif
		return 1;
	}

    if (argc > 4)
    {
      if (strcmp(argv[1],"dry")==0)
      {
         workPath = argv[2];
         sscanf(argv[3], "%d", &Param1);
         argc -= 3;
         argv = &argv[3];
         argvN=argv;
         argcN=argc;
       }
	  else if (strcmp(argv[1], "id") == 0)

	  {
         sscanf(argv[2], "%x", &Param2);
         argc -= 2;
         argv = &argv[2];
         argvN=argv;
         argcN=argc;

	  }

    }
	spiParams.spi_freq = DEFAULT_SPI_SPEED;
	spiParams.spi_cs   = 0;
	spiParams.spi_num  = 0;
	if (inu_storage_open(spiParams, 0, workPath, Param1,Param2) != INU_STORAGE_ERR_SUCCESS)
	{
		printf("Can't start boot service of inu_usb_lib\n");
		return 1;
	}
	if (inu_storage_get_chip_version(&chipVersion) != 0)
		printf("Fail read chip id\n");
	else printf("Chip id %x\n", chipVersion);

	// inu_flash_read_production_data(&prodData);
	memset(tmpBuff, 0, sizeof(tmpBuff));
	pInuSectionDataT->sectionDataBuffer = tmpBuff;
	pInuSectionDataT->sectionDataSize = 0x10000;

	inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);

	//Target configuration
#ifdef _DEBUG
	while ((firstWord = getopt(argc, argv, ("A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:Q:R:S:T:UV:W:X:Y:Z:a:b:e:f:h:p:q:r:s:t:u:v:w"))) != EOF)
#else
	while ((firstWord = getopt(argc, argv, ("f:I:UZ"))) != EOF)
#endif
	{


		switch (firstWord)
		{

		case ('L'):
			inu_storage_close();
			#if (defined _WIN32) || (defined _WIN64)
			Sleep(2);
			#else
			sleep(2);
			#endif
			if (spiParams.spi_freq == DEFAULT_SPI_SPEED )
			   spiParams.spi_freq = 80000;
			spiParams.spi_cs   = 0;
			spiParams.spi_num  = 1;
			if (inu_storage_open(spiParams, 0, workPath, Param1, Param2) != INU_STORAGE_ERR_SUCCESS)
			{
				printf("Can't start boot service of inu_usb_lib\n");
				return 1;
			}
			inu_storage_burn_avr_file(optarg);

			break;

		case ('f') :

			inu_flash_run_config(optarg);
			break;

		case ('I') :
			sscanf(optarg, "%x", &Param);
			if (argcN == 4)
			{
				sscanf(argvN[3], "%d", &Param1);
			}
			pInuSectionDataT->sectionDataBuffer = tmpBuff;
			if (inu_storage_read_section_data(INU_STORAGE_SECTION_BOOTFIX_METADATA, pInuSectionDataT) != 0)
			{
		    	strcpy(pInuSectionDataT->versionString, "bootfixHdrver");
			    pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
				pInuSectionDataT->timestamp = tv_sec;
			    memset(pInuSectionDataT->sectionDataBuffer,0,sizeof(InuBootfixHeaderT));
			}
			pInuSectionDataT->sectionDataSize = sizeof(InuBootfixHeaderT);
			((InuBootfixHeaderT	*)pInuSectionDataT->sectionDataBuffer)->isQuadFlash = Param1;
			((InuBootfixHeaderT	*)pInuSectionDataT->sectionDataBuffer)->isBootfromFlash = (InuStrapOverrideE)(Param & 1);
			inu_storage_write_section_data(INU_STORAGE_SECTION_BOOTFIX_METADATA, pInuSectionDataT);
			break;

		case ('U'):
			pInuSectionDataT->sectionDataBuffer = tmpBuff;
			if (inu_storage_read_section_data(INU_STORAGE_SECTION_BOOTFIX_METADATA, pInuSectionDataT) != 0)
			{
				strcpy(pInuSectionDataT->versionString, "bootfixHdrver");
				pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
				pInuSectionDataT->timestamp = tv_sec;
				memset(pInuSectionDataT->sectionDataBuffer, 0, sizeof(InuBootfixHeaderT));
			}
			pInuSectionDataT->sectionDataSize = sizeof(InuBootfixHeaderT);
			((InuBootfixHeaderT*)pInuSectionDataT->sectionDataBuffer)->flashFlags = INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS;
			inu_storage_write_section_data(INU_STORAGE_SECTION_BOOTFIX_METADATA, pInuSectionDataT);
			break;
#ifdef _DEBUG
      case ('A') :

         if (strlen(optarg) < PRODUCTION_STRING_SIZE)
         {
            strncpy(prodData->serialNumber, optarg, PRODUCTION_STRING_SIZE);

            writeProd = 1;
         }
         break;
      case ('F') :

         if (strlen(optarg) < PRODUCTION_EXT_STRING_SIZE)
         {
            strncpy(prodData->serialNumber, optarg, PRODUCTION_EXT_STRING_SIZE);
            prodData->extKey = EXT_KEY_NUM;
            writeProd = 1;
         }
         break;
      case ('B') :
         if (strlen(optarg) < PRODUCTION_STRING_SIZE)
         {
            strncpy(prodData->modelNumber, optarg, PRODUCTION_STRING_SIZE);
            writeProd = 1;
         }
         break;
      case ('C') :
         if (strlen(optarg) < PRODUCTION_STRING_SIZE)
         {
            strncpy(prodData->partNumber, optarg, PRODUCTION_STRING_SIZE);
            writeProd = 1;
         }
         break;
      case ('D') :
         if (strlen(optarg) < PRODUCTION_STRING_SIZE)
         {
            strncpy(prodData->sensorRevision, optarg, PRODUCTION_STRING_SIZE);
            writeProd = 1;
         }
         break;
      case ('E') :
         sscanf(optarg, "%d", &Param);
         prodData->bootId = Param;
         writeProd = 1;
         break;
		case ('G') :

			pInuSectionDataT->timestamp = tv_sec;
			strcpy(pInuSectionDataT->versionString, "1.01");
			pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
			sectionType = INU_STORAGE_SECTION_CALIBRATION_DATA;
			write_file = argvN[2];
			break;
		case ('h') :

			pInuSectionDataT->timestamp = tv_sec;
			strcpy(pInuSectionDataT->versionString, "1.02");
			pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
			sectionType = INU_STORAGE_SECTION_DYNAMIC_CALIBRATION;
			write_file = argvN[2];
			break;
		case ('K') :

			pInuSectionDataT->timestamp = tv_sec;
			strcpy(pInuSectionDataT->versionString, "1.01");
			pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
			if (argcN == 4)
			{
				inu_storage_update_bootsplOffest(BOOTSPL_OFFSET_ACTIVE);
				sscanf(argvN[2], "%d", &sectionType);
				if (((sectionType >= INU_STORAGE_SECTION_BOOTSPL) && (sectionType <= INU_STORAGE_SECTION_APP)) || (sectionType >= INU_STORAGE_SECTION_PRODUCTION_DATA))
				{
					write_file = argvN[3];
				}
			}
			else if (argcN == 5)
			{
				sscanf(argvN[2], "%d", &bootsplOffset);
				if (bootsplOffset == 0)
				{
					inu_storage_update_bootsplOffest(BOOTSPL_OFFSET_ACTIVE);
				}
				else if (bootsplOffset == 1)
				{
					inu_storage_update_bootsplOffest(BOOTSPL_OFFSET_BACKUP);
				}
				else
				{
					printf("Unsupported bootsplOffset = %d\n", bootsplOffset);
					return -1;
				}
				sscanf(argvN[3], "%d", &sectionType);
				if (((sectionType >= INU_STORAGE_SECTION_BOOTSPL) && (sectionType <= INU_STORAGE_SECTION_APP)) || (sectionType >= INU_STORAGE_SECTION_PRODUCTION_DATA))
				{
					write_file = argvN[4];
				}
			}
			else
			{
				printf("Unsupported numer of args for -K command. argcN = %d. [Allowed numer of args are 4 or 5]\n", argcN);
				return -1;
			}

			break;



		case ('J') :
			pInuSectionDataT->timestamp = 0x1234;
			strcpy(pInuSectionDataT->versionString, "bootfixver");
			pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
			if (copyFileToBuff(optarg, pInuSectionDataT->sectionDataBuffer, &size) != -1)
			{
				pInuSectionDataT->sectionDataSize = size;
				inu_storage_write_section_data(INU_STORAGE_SECTION_BOOTFIX, pInuSectionDataT);
			}
			break;
		case ('S') :
		    {
				InuComponentDataT components;
				memset(&components,0,sizeof(components));
				inu_storage_get_components(&components, "C:\\Program Files\\Inuitive\\InuDev\\bin\\");
				printf("\nlinux=%s\ndtb=%s\nspl=%s\napp=%s\n", components.linux_file_name, components.dtb_file_name, components.spl_file_name, components.app_file_name);
		    }
			break;
		case ('R') :
			sscanf(optarg, "%x", &Param);
			if (Param <= INU_STORAGE_SECTION_APP)
				inu_storage_erase_section((InuStorageSectionTypeE)Param, 0);
			break;

		case ('Q') :
			sscanf(optarg, "%x", &Param);
			if (argcN == 4)
			{
				sscanf(argvN[3], "%x", &Data);
				inu_storage_erase(0, Param, Data);
			}
			//		inu_storage_write_efuse();
			host_millisleep(500);
			/*		inu_storage_write_efuse(29, 8);
					host_millisleep(500); */
			break;
		case ('H') :

			sscanf(optarg, "%x", &Param);
#ifdef USE_OLD_PUBLIC_KEY
			if (Param == 0x1234)
			{
				memset(buff, 0, sizeof(buff));
				memcpy(&buff[6],secure_boot_public_key,sizeof(secure_boot_public_key));
				memcpy(&buff[22], imageDecryption, sizeof(imageDecryption));
				inu_storage_program_efuse(buff);
			}
			else if (Param == 0x4321)
			{
				memset(buff, 0, sizeof(buff));
				buff[5] = 0x60;
				inu_storage_program_efuse(buff);
			}
#endif
			if (Param == 0x12345)
			{
				memset(buff, 0, sizeof(buff));
				memcpy(&buff[6], secure_boot_public_key, sizeof(secure_boot_public_key));
				//memcpy(&buff[22], imageDecryption, sizeof(imageDecryption));
				inu_storage_program_efuse(buff);
			}
			else if (Param == 0x54321)
			{
				memset(buff, 0, sizeof(buff));
				// 0 - GPP JTAG disable
				// 1 - DSP JTAG disable
				// 2 - EV JTAG disable
				// 3 - TEST pin disable
				// 5 - Integrity force

				buff[5] = 0x2F;
				inu_storage_program_efuse(buff);
			}
			host_millisleep(500);
			break;

		case ('W') :
			sscanf(optarg, "%x", &Param);
			if (argcN == 4)
				sscanf(argvN[3], "%x", &Data);
			//		for (ind = 0; ind < 0x4000;ind++)
			inu_storage_write_raw(0, Param, (unsigned char *)&Data, 4);
			//		memset(tmpBuff, Data&0xff,0x10000);
				//	inu_storage_write_raw(0, Param,tmpBuff, 0x10000);
			break;

		case ('w'):
			if (copyFileToBuff(argvN[2], tmpBuff, &size) != -1)
			{
				Data = 0;
				if (argcN == 4)
				{
					sscanf(argvN[3], "%x", &Data);
				}
				inu_storage_write_raw(0, Data, tmpBuff, size);
				printf("writing file %s offset %d\n",argvN[1],Data);
				unsigned char *tstBuff = malloc(size);
				if (tstBuff != NULL)
				{
    				inu_storage_read_raw(0, Data, tstBuff, size, &transfered);
    				for (ind = 0; ind < size; ind++)
    				{
    					if (tmpBuff[ind] != tstBuff[ind])
    					{
    						printf("ind 0x%x file data 0x%x - read data - %x\n", ind, tmpBuff[ind], tstBuff[ind]);
    						break;
    					}
    				}
    				if (ind == size)
    					printf("Verify O.K\n");
    				else printf("Verify FAILED ind 0x%x\n", ind);
					free(tstBuff);

				}
			}
			break;

		case ('v'):
			if (copyFileToBuff(argvN[2], tmpBuff, &size) != -1)
			{
				Data = 0;
				if (argcN == 4)
				{
					sscanf(argvN[3], "%x", &Data);
				}
				unsigned char *tstBuff = malloc(size);
				if (tstBuff != NULL)
				{
			    	inu_storage_read_raw(0, Data, tstBuff, size, &transfered);
    				for (ind = 0; ind < size; ind++)
    				{
    					if (tmpBuff[ind] != tstBuff[ind])
    					{
    						printf("ind 0x%x file data -  0x%x read data - %x\n", ind,tmpBuff[ind], tstBuff[ind]);
    						break;
    					}
    				}
    				if (ind == size)
    					printf("Verify O.K\n");
    				else printf("Verify FAILED ind 0x%x\n",ind);
    				free(tstBuff);
    			}
			}
			break;
		case ('q'):
			if (copyFileToBuff(argvN[2], tmpBuff, &size) != -1)
			{
				Data = 0;
				if (argcN == 4)
				{
					sscanf(argvN[3], "%x", &Data);
				}
				inuFlashData.dataBuffer = tmpBuff;
				inuFlashData.dataSize = size;
				inuFlashData.dataOffset = Data;
				 inu_storage_write_data(&inuFlashData);

			}
			break;

		case ('t'):
			Data = 0;
			if (argcN  >= 4)
			{
				sscanf(argvN[2], "%x", &Param);
				sscanf(argvN[3], "%x", &Param1);
				inuFlashData.dataBuffer = tmpBuff;
				inuFlashData.dataOffset = Param;
				inuFlashData.dataSize = Param1;
				inu_storage_read_data(&inuFlashData);
				for (ind = 0; ind < inuFlashData.dataSize / 4 + 1; ind++)
				{
					if (ind % 8 == 0)
						printf("\n0x%6.6x:",ind * 4 + inuFlashData.dataOffset);
					printf("0x%8.8x ", ((unsigned long *)inuFlashData.dataBuffer)[ind]);

				}
			}


			break;

		case ('P') :
			sscanf(optarg, "%x", &Param);
			if (argcN >= 4)
				size = atoi(argvN[3]);
			/*				printf("offset %x strap offset %x calib size %x sizeof(InuStorageLayoutT) %x bootspl offset %x dtb offset %x zImage offset %x\n",
			(unsigned int)Param,
			(unsigned int)BOOTFIX_DATA_SIZE,
			(unsigned int)CALIBRATION_SIZE,
			(unsigned int)sizeof(InuStorageLayoutT),
			(unsigned int)(CALIBRATION_SIZE+sizeof(InuStorageLayoutT)),
			(unsigned int)(CALIBRATION_SIZE+sizeof(InuStorageLayoutT) + BOOTSPL_SIZE),
			(unsigned int)(CALIBRATION_SIZE+sizeof(InuStorageLayoutT) + BOOTSPL_SIZE + DTB_SIZE));*/
			printFlashData(Param, size);
			break;
		case ('s'):
			sscanf(optarg, "%x", &Param);
			if (argcN >= 4)
				sscanf(argvN[3], "%x", &size);;
			copyFlashDataToFile(Param, size);
			break;
		case ('p') :

			if (copyFileToDynamicBuff(optarg, &inuNandData.dataBuffer, &inuNandData.dataSize) != -1)
			{
				inuNandData.dataOffset = 0x00;
				printf("Performing NAND image burn. Address  0x%x size 0x%x\n", inuNandData.dataOffset, inuNandData.dataSize);
				inu_storage_write_nand_data(&inuNandData);
				free(inuNandData.dataBuffer);
			}
			break;
		case ('u'):
			sscanf(argvN[2], "%x", &partition);
			sscanf(argvN[3], "%x", &section);

			if (copyFileToDynamicBuff(argvN[4], &inuNandData.dataBuffer, &inuNandData.dataSize) == -1)
			{
				printf("unable to update\n");
				return -1;
			}
			inuNandData.dataOffset = 0x00;	// not in use here.
			printf("Performing NAND update. partition  0x%x section 0x%x\n", partition, section);

			inu_storage_update_nand_image(partition, section, &inuNandData);

			free(inuNandData.dataBuffer);

			break;

		case ('r') :
			{
				char fileName[40] = "Nand_Read_Addr_";
				char addrStr[8];
				FILE *fd;

				sscanf(argvN[2], "%x", &inuNandData.dataOffset);
				sscanf(argvN[3], "%x", &inuNandData.dataSize);
				#ifdef __linux__
				sprintf(addrStr,"%d",inuNandData.dataOffset);
				#else
				_itoa(inuNandData.dataOffset, addrStr, 16);
				#endif
				strcat(fileName, addrStr);
				
				fd = fopen(fileName, "wb");
				if (fd == NULL)
				{
					printf("Can't open file %s\n", fileName);
					return -1;
				}

				inuNandData.dataBuffer = malloc(inuNandData.dataSize);
				
				inu_storage_read_nand_data(&inuNandData);

				fwrite(inuNandData.dataBuffer, sizeof(char), inuNandData.dataSize, fd);

				fclose(fd);
				break;
			}

		case ('e'):
			
			sscanf(argvN[2], "%x", &inuNandData.dataOffset);
			sscanf(argvN[3], "%x", &inuNandData.dataSize);
			printf("Performing NAND erase. Address  0x%x size 0x%x\n", inuNandData.dataOffset, inuNandData.dataSize);

			inu_storage_erase_nand_data(&inuNandData);

			break;
#endif

		case ('T') :

			sscanf(argv[2], "%d", &Param);
			sscanf(argv[3], "%d", &Param1);
			if (argc >=6)
			{
			   sscanf(argv[4], "%d", &Param2);
			   sscanf(argv[5], "%d", &Param3);
			}
			inu_storage_close();
	#if (defined _WIN32) || (defined _WIN64)
			Sleep(2);
    #else
			sleep(2);
	#endif
			spiParams.spi_freq = Param;
			spiParams.spi_num = Param2;
			spiParams.spi_cs  = Param3;
			if (inu_storage_open(spiParams,Param1, NULL, 0,0) != INU_STORAGE_ERR_SUCCESS)
			{
				printf("Can't start boot service of inu_usb_lib\n");
				return 1;
			}
			printf("open stoarge freq %d flashNum %d spiNum %d chipSelect %d \n",Param,Param1,
			                                                 spiParams.spi_num,spiParams.spi_cs);
			inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);

			argcN = argc - 5;
			argvN = &argv[5];

			break;
		case ('V') :
#ifdef _DEBUG
			sscanf(argv[2], "%x", &Param);
			inu_storage_set_version(Param);
#endif
			break;
		case ('Z') :
         if (argcN > 3)
           if (strncmp("save", argvN[2],strlen("save"))==0);
              sectionOutputFile = 1;
         sscanf(argvN[2], "%d", &ind);
        sectionType = ind;
         if ((ind >= INU_STORAGE_SECTION_BOOTFIX) && (ind <= INU_STORAGE_SECTION_APP))
         {
           memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
           memset(tmpBuff, 0, sizeof(tmpBuff));
           if (inu_storage_read_section_data(sectionType, pInuSectionDataT) == 0)
           {
             size = pInuSectionDataT->sectionDataSize;
             if (size <= MAX_FILE_SIZE)
             {

             	memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
             	memset(tmpBuff, 0, sizeof(tmpBuff));
             	pInuSectionDataT->sectionDataBuffer = tmpBuff;
             	pInuSectionDataT->sectionDataSize = size;
             	inu_storage_read_section_data(sectionType, pInuSectionDataT);
             	if (sectionOutputFile && (sectionType <=INU_STORAGE_SECTION_APP))
             		copyBuffToFile(sectionOutputFileName[sectionType], pInuSectionDataT->sectionDataBuffer, pInuSectionDataT->sectionDataSize);

		         if (ind == INU_STORAGE_SECTION_PRODUCTION_DATA)
		         {
		            inu_display_prd((InuProductionDataT *)pInuSectionDataT->sectionDataBuffer);
		            inu_display_hdr(pInuSectionDataT);
		         }
		         else
		         {
		            inu_display_file_data(pInuSectionDataT);
		            inu_display_hdr(pInuSectionDataT);
		         }
             }
           }
           else printf("file size 0x%x is bigger than MAX(0x%x)\n", size, MAX_FILE_SIZE);


			}
			break;	


		case '?':
			printf("Unknown option character \n");
			break;
		}
		if (write_file)
		{
			if (copyFileToBuff(write_file, pInuSectionDataT->sectionDataBuffer, &size) != -1)
			{
				pInuSectionDataT->sectionDataSize = size;
				retCode = inu_storage_write_section_data(sectionType, pInuSectionDataT);
				if (retCode != 0)
					printf("***************Fail Writing file retCode=%d sectionType %s\n", retCode,sectionOutputFileName[sectionType%(INU_STORAGE_SECTION_APP + 1)]);

			}
			else printf("copy file to buffer Failed");

			write_file = NULL;
		}
	}

	if (writeProd != 0)
	{
		pInuSectionDataT->sectionDataSize = sizeof(InuProductionDataT);
		pInuSectionDataT->timestamp = 0x1234;
		strcpy(pInuSectionDataT->versionString, "versionPrd");
		pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
		retCode = inu_storage_write_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);
		if (retCode != 0)
			printf("***************Fail Writing production retCode=%d\n", retCode);
		memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
		memset(tmpBuff, 0, sizeof(tmpBuff));
		pInuSectionDataT->sectionDataBuffer = tmpBuff;
		inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);
		inu_display_prd((InuProductionDataT *)pInuSectionDataT->sectionDataBuffer);
		inu_display_hdr(pInuSectionDataT);
	}

	inu_storage_close();
	printf("Exiting flash app ...\n");
	return 0;
}

