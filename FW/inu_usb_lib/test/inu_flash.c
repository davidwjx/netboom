#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>

#include "inu_usb.h"
#include "inu_storage.h"
#include "inu_storage_layout.h"
#if (defined _WIN32) || (defined _WIN64)
  #include <windows.h>
  #include <conio.h>
  #include "getopt.h"
#elif defined(__linux__)
  #include <unistd.h>
#endif

#define VERSION_STR   "1.0"
#define MAX_LINE_SIZE 512
#define MAX_READ_SIZE 0x20000
#define MAX_FILE_SIZE 0x600000 
InuUsbParamsT inuParams;
InuProductionDataT prodData;
InuStorageLayoutT inuStorageLayout;

InuSectionDataT inuSectionDataT;
InuSectionDataT *pInuSectionDataT=&inuSectionDataT;

unsigned char buffer[MAX_READ_SIZE];
unsigned char tmpBuff[MAX_FILE_SIZE];

int millisleep(unsigned ms)
{
#if (defined _WIN32) || (defined _WIN64)
  SetLastError(0);
  Sleep(ms);
  return GetLastError() ?-1 :0;
#elif defined(__linux__)
  usleep(1000 * ms);
  return 0;
#else
#error ("no milli sleep available for platform")
  return -1;
#endif
}

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
int copyFileToBuff(char *filePath,unsigned char *dataPtr,unsigned int *readSize)
{
	FILE *fd;
	int rc=0;

	fd = fopen(filePath,"rb");
	if (fd == NULL)
	{
		printf("Can't open file %s\n",filePath);
		return -1;
	}
	fseek(fd, 0, SEEK_END);
    *readSize=ftell(fd);
    fseek(fd, 0, SEEK_SET);
	printf("size %d\n",*readSize);

	*readSize = (unsigned int)fread(dataPtr,1,*readSize,fd);
	fclose(fd);


	return rc;
}

#ifdef _DEBUG
void printFlashData(unsigned int offset,unsigned int size)
{
	unsigned int ind,transfered,ret;
	
	ret =  inu_storage_read_raw(0, offset,buffer, size,&transfered);
	if (ret != INU_STORAGE_ERR_SUCCESS)
		return;
	for (ind=0;ind<size/4+1;ind++)
	{
		if (ind%8 == 0)
			printf("\n0x%6.6x:",offset+ind*4);
		printf("0x%8.8x ",((unsigned long *)buffer)[ind]);

	}
}
#endif

void inu_display_file_data(InuSectionDataT *pInuSectionDataT)
{
	unsigned int ind;
	printf("\n-------------section data-------------------\n");
	printf("%-20.20s      %d\n", "DataFormat", pInuSectionDataT->sectionDataFormat);
	printf("%-20.20s      %d\n", "DataSize", pInuSectionDataT->sectionDataSize);
	printf("%-20.20s      %llx\n", "timestamp", pInuSectionDataT->timestamp);
	if (pInuSectionDataT->sectionDataSize < 0x400000)
	{
		for (ind = 0; ind<pInuSectionDataT->sectionDataSize; ind++)
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
	char copyStr[PRODUCTION_STRING_SIZE + 1];
	memset(copyStr, 0, sizeof(copyStr));
	printf("\n-------------production data-------------------\n");
	strncpy(copyStr, prodData->serialNumber, PRODUCTION_STRING_SIZE);
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
	char versionString[VERSION_STRING_SIZE] = {0};
/*	strncpy(versionString,pInuSectionHeaderT->versionString,VERSION_STRING_SIZE-1);
	printf("format 0x%x\n", pInuSectionHeaderT->sectionFormat);
	printf("magic  0x%x\n", pInuSectionHeaderT->magicNumber);
	printf("timestamp 0x%x\n", pInuSectionHeaderT->timestamp);
	printf("datasize 0x%x\n", pInuSectionHeaderT->sectionDataSize);
	printf("section size 0x%x\n", pInuSectionHeaderT->sectionSize);
	printf("version number %d\n", pInuSectionHeaderT->versionNumber);
	printf("versionString  %s\n", versionString);*/
	strncpy(versionString, pInuSectionDataT->versionString, VERSION_STRING_SIZE - 1);
	printf("format 0x%x\n", pInuSectionDataT->sectionDataFormat);
	printf("datasize 0x%x\n", pInuSectionDataT->sectionDataSize);;
	printf("timestamp 0x%llx\n", pInuSectionDataT->timestamp);
	printf("versionString  %s\n", versionString);

}

int main(int argc, char* argv[])
{

   int ind;
   unsigned int size=32,Data;
   unsigned int firstWord,Param=0,Param2=0;
   unsigned int writeProd=0,writeCalib=0, write_file=0;
   InuStorageSectionTypeE sectionType;

   InuProductionDataT *prodData = (InuProductionDataT *)tmpBuff;

   printf("main: start flash app...\n");
   if (argc <= 1) 
   {
        printf ("Usage: inu_flash.exe \n" 
			"\t -A production serial(string)\n"		
			"\t -B production model number(string)\n"		
			"\t -C production part number(string)\n"		
			"\t -D production sensor revison(string)\n"		
			"\t -E production boot id(decimal)\n"		
			"\t -F calibration file (with full path)\n"		
			"\t -H write flash efuse\n"		
			"\t -I bootfix strap(hex)\n"		
			"\t -J boofix loader file(with full path)\n"		
			"\t -M DTB file (with full path)\n"		
			"\t -N kernel file (with full path)\n"		
			"\t -O nu30000 application file (with full path)\n"		
			"\t -Z print(0 production  1- calibration)\n"		
			);
	    return 1;
   }   
   
   
   if (inu_storage_open() != INU_STORAGE_ERR_SUCCESS)
   {
		printf("Can't start boot service of inu_usb_lib\n");
		return 1;
   }

  // inu_flash_read_production_data(&prodData);
    memset(tmpBuff, 0, sizeof(tmpBuff));
    pInuSectionDataT->sectionDataBuffer = tmpBuff;
	pInuSectionDataT->sectionDataSize = 0x10000;

    inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);

    //Target configuration
    while ((firstWord= getopt(argc, argv, ("A:B:C:D:E:F:G:H:I:J:K:L:M:N:O:P:R:Q:Z:W:"))) != EOF) 
	{
        switch (firstWord) 
		{
			  case ('F'):

				    pInuSectionDataT->timestamp = 0x1234;
				    strcpy(pInuSectionDataT->versionString, "versioncalib");
				    pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
					sectionType = INU_STORAGE_SECTION_CALIBRATION_DATA;
					write_file = 1;
            break;
			  case ('M') :

				  pInuSectionDataT->timestamp = 0x5678;
				  strcpy(pInuSectionDataT->versionString, "versionDTB");
				  pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
				  sectionType = INU_STORAGE_SECTION_DTB;
				  write_file = 1;
				  break;
			  case ('N') :

				  pInuSectionDataT->timestamp = 0x6789;
				  strcpy(pInuSectionDataT->versionString, "versionKERNEL");
				  pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
				  sectionType = INU_STORAGE_SECTION_KERNEL;
				  write_file = 1;
				  break;
			  case ('O') :

				  pInuSectionDataT->timestamp = 0x789a;
				  strcpy(pInuSectionDataT->versionString, "versionAPP");
				  pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
				  sectionType = INU_STORAGE_SECTION_APP;
				  write_file = 1;
				  break;


             case ('A'):

				 if (strlen(optarg)<PRODUCTION_STRING_SIZE)
					{
						strncpy(prodData->serialNumber,optarg,PRODUCTION_STRING_SIZE);

						writeProd=1;
					}
             break;
             case ('B'):
				 if (strlen(optarg)<PRODUCTION_STRING_SIZE)
					{
						strncpy(prodData->modelNumber,optarg,PRODUCTION_STRING_SIZE);
						writeProd=1;
					}
              break;
             case ('C'):
				 if (strlen(optarg)<PRODUCTION_STRING_SIZE)
					{
						strncpy(prodData->partNumber,optarg,PRODUCTION_STRING_SIZE);
						writeProd=1;
					}
             break;
             case ('D'):
				 if (strlen(optarg)<PRODUCTION_STRING_SIZE)
					{
						strncpy(prodData->sensorRevision,optarg,PRODUCTION_STRING_SIZE);
						writeProd=1;
					}
             break;
             case ('E'):
					sscanf(optarg, "%d", &Param);
					prodData->bootId=Param;
					writeProd=1;
              break;
			 case ('J'):
				 pInuSectionDataT->timestamp = 0x1234;
				 strcpy(pInuSectionDataT->versionString, "bootfixver");
				 pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
				 if (copyFileToBuff(optarg, pInuSectionDataT->sectionDataBuffer, &size) != -1)
				 {
					 pInuSectionDataT->sectionDataSize = size;
					 inu_storage_write_section_data(INU_STORAGE_SECTION_BOOTFIX, pInuSectionDataT);
				 }
				 break;
             case ('I'):
				 sscanf(optarg, "%x", &Param);
				 memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
				 pInuSectionDataT->sectionDataBuffer = tmpBuff;
				 pInuSectionDataT->timestamp = 0x1234;
				 strcpy(pInuSectionDataT->versionString, "bootfixHdrver");
				 pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;
				 pInuSectionDataT->sectionDataSize = 8;

				 ((InuBootfixMetaDataT	*)pInuSectionDataT->sectionDataBuffer)->bootStrapOverride = (InuStrapOverrideE)(Param & 1);
				 inu_storage_write_section_data(INU_STORAGE_SECTION_BOOTFIX_METADATA, pInuSectionDataT);
				 break;
			 case ('P'):
					sscanf(optarg, "%x", &Param);
					if (argc == 4)
						size = atoi(argv[3]);
	/*				printf("offset %x strap offset %x calib size %x sizeof(InuStorageLayoutT) %x bootspl offset %x dtb offset %x zImage offset %x\n",
						(unsigned int)Param,
						(unsigned int)BOOTFIX_DATA_SIZE,
						(unsigned int)CALIBRATION_SIZE,
						(unsigned int)sizeof(InuStorageLayoutT),
						(unsigned int)(CALIBRATION_SIZE+sizeof(InuStorageLayoutT)),
						(unsigned int)(CALIBRATION_SIZE+sizeof(InuStorageLayoutT) + BOOTSPL_SIZE),
						(unsigned int)(CALIBRATION_SIZE+sizeof(InuStorageLayoutT) + BOOTSPL_SIZE + DTB_SIZE));*/
#ifdef _DEBUG
 					printFlashData(Param,size);
#endif
            break;

			case ('R'):
					sscanf(optarg, "%x", &Param);
				    if (Param <= INU_STORAGE_SECTION_APP)
						inu_storage_erase_section((InuStorageSectionTypeE)Param, 0);
				   break;

			case ('Q'):
#ifdef _DEBUG
				sscanf(optarg, "%x", &Param);
				if (argc == 4)
				{ 
					sscanf(argv[3], "%x", &Data);
					inu_storage_erase(0,Param, Data);
				}
#endif
		//		inu_storage_write_efuse();
 				millisleep(500); 
		/*		inu_storage_write_efuse(29, 8);
				millisleep(500); */
           break;
			case ('H'):
				sscanf(optarg, "%x", &Param);
				if (Param == 0x1234)
					inu_storage_program_efuse();
 				millisleep(500); 
           break;

			case ('W'):
					sscanf(optarg, "%x", &Param);
					if (argc == 4)
						sscanf(argv[3], "%x", &Data);
#ifdef _DEBUG
			//		for (ind = 0; ind < 0x4000;ind++)
					inu_storage_write_raw(0, Param,(unsigned char *)&Data, 4);
			//		memset(tmpBuff, Data&0xff,0x10000);
				//	inu_storage_write_raw(0, Param,tmpBuff, 0x10000);
#endif
				   break;

			 case ('Z'):
				    ind = atoi(optarg);
					if ((ind > 0)&&(ind<=4))
					{   
						if (ind == 1)
						{
							sectionType = INU_STORAGE_SECTION_CALIBRATION_DATA;

						}
						else if (ind == 2)
						{
							sectionType = INU_STORAGE_SECTION_DTB;

						}
						else if (ind == 3)
						{
							sectionType = INU_STORAGE_SECTION_KERNEL;
						}
						else if (ind == 4)
						{
							sectionType = INU_STORAGE_SECTION_APP;

						}


						memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
						memset(tmpBuff, 0, sizeof(tmpBuff));
						pInuSectionDataT->sectionDataBuffer = tmpBuff;
						pInuSectionDataT->sectionDataSize = MAX_FILE_SIZE;
						inu_storage_read_section_data(sectionType, pInuSectionDataT);

						inu_display_file_data(pInuSectionDataT);
				    	inu_display_hdr(pInuSectionDataT);


					}
					else if (ind==0)
					{
						memset(tmpBuff,0,sizeof(tmpBuff));
						pInuSectionDataT->sectionDataBuffer = tmpBuff;
						pInuSectionDataT->sectionDataSize = MAX_FILE_SIZE;
						inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);
						inu_display_prd((InuProductionDataT *)pInuSectionDataT->sectionDataBuffer);
						inu_display_hdr(pInuSectionDataT);
					}

	
             break;
			 

              case '?':
                printf ("Unknown option character \n");
                break;
        }
		if (write_file)
		{
			if (copyFileToBuff(optarg, pInuSectionDataT->sectionDataBuffer, &size) != -1)
			{
				pInuSectionDataT->sectionDataSize = size;
				inu_storage_write_section_data(sectionType, pInuSectionDataT);
			}

			memset(pInuSectionDataT, 0, sizeof(InuSectionDataT));
			memset(tmpBuff, 0, sizeof(tmpBuff));
			pInuSectionDataT->sectionDataSize = MAX_FILE_SIZE;
			pInuSectionDataT->sectionDataBuffer = tmpBuff;
			inu_storage_read_section_data(sectionType, pInuSectionDataT);

	//		inu_display_file_data(pInuSectionDataT);
			write_file = 0;
		}
    }  

	if (writeProd != 0)
	{
		pInuSectionDataT->sectionDataSize = sizeof(InuProductionDataT);
		pInuSectionDataT->timestamp = 0x1234;
		strcpy(pInuSectionDataT->versionString, "versionPrd");
		pInuSectionDataT->sectionDataFormat = INU_STORAGE_SECTION_FORMAT_RAW;

		inu_storage_write_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);
		memset(pInuSectionDataT,0,sizeof(InuSectionDataT));
		memset(tmpBuff,0,sizeof(tmpBuff));
		pInuSectionDataT->sectionDataBuffer = tmpBuff;
		inu_storage_read_section_data(INU_STORAGE_SECTION_PRODUCTION_DATA, pInuSectionDataT);
		inu_display_prd((InuProductionDataT *)pInuSectionDataT->sectionDataBuffer);
		inu_display_hdr(pInuSectionDataT);
	}

	inu_storage_close();
    printf("Exiting flash app ...\n");
    return 0;
}
