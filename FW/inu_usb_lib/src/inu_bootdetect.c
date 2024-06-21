/****************************************************************************
*
*   FileName: inu_bootdetect.
*
*   Author:  Giyora Arbel, Konstantin Sinyuk
*
*   Date:
*
*   Description: Inuitive user space abstraction for automatic bootId detection
*
****************************************************************************/
#define INUUSB_EXPORTS

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "inu_usb.h"
#include "inu_boot.h"
#include "inu_bootdetect.h"
#include "inu_storage_layout.h"
#include "inu_boot_common.h"
#include "inu_host_utils.h"
#include "bootdetect_nu3000b0.h"
#include "bootdetect_nu3000a0.h"
#include "bootdetect_nu4000a0.h"
#include "bootdetect_nu4000b0.h"
#include "bootdetect_nu4000c0.h"
#include "bootdetect_nu4000c1.h"

static InuUsbErrorE bootdetect_read_production_data(InuProductionHeaderT*productionData,unsigned int offset,unsigned int magic_number)
{
	InuStorageU inuStorage;
	unsigned int transfered_size;
	InuUsbErrorE read_err;
	int bootId = 0;	
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	if (magic_number == LEGACY_PRODUCTION_HEADER_MAGIC_NUMBER)
	{
		read_err = inu_usb_monitor_read(offset, (unsigned char *)&inuStorage.storageLegacyLayout, sizeof(InuStorageLegacyLayoutT), &transfered_size);
		if (read_err != INU_USB_ERR_SUCCESS)
		{
			return INU_USB_ERR_INIT_FAILED;
		}

		//link between model number & bootid
		if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1M(0014)"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1L(0011)"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1L(0013)"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1G(0017)"))
		{
			bootId = 100;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1GR(0030)"))
		{
			bootId = 120;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1T(0018)"))
		{
			bootId = 10;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1TM"))
		{
			bootId = 10;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2NM"))
		{
			if (!strcmp(inuStorage.storageLegacyLayout.PartNumber, "100-0600-01"))
			{
				bootId = 30;
			}
			else if (!strcmp(inuStorage.storageLegacyLayout.PartNumber, "110-0033-01"))
			{
				bootId = 30;
			}
			else			
			{
				bootId = 31;
			}			
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1B"))
		{
			bootId = 600;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1S(0018)"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2AP"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1O(0019)"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2NMF"))
		{
			if (!strcmp(inuStorage.storageLegacyLayout.PartNumber, "100-0602-01"))
			{
				bootId = 110;
			}
			else if (!strcmp(inuStorage.storageLegacyLayout.PartNumber, "110-0032-01"))
			{
				bootId = 110;
			}
			else
			{
				bootId = 111;
			}
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2NMNC"))
		{
			if (!strcmp(inuStorage.storageLegacyLayout.PartNumber, "100-0026-01"))
			{
				bootId = 20;
			}
			else if (!strcmp(inuStorage.storageLegacyLayout.PartNumber, "100-0026-02"))
			{
				bootId = 21;
			}
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2T"))
		{
			bootId = 31;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2ELG"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1V"))
		{
			bootId = 10;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2NM0"))
		{
			bootId = 20;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.1EOC"))
		{
			bootId = 0;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.2TE"))
		{
			bootId = 40;
		}
		else if (!strcmp(inuStorage.storageLegacyLayout.ModelNumber, "M3.3V"))
		{
			bootId = 60;
		}
		productionData->bootId = bootId;
		strncpy(productionData->modelNumber, inuStorage.storageLegacyLayout.ModelNumber, PRODUCTION_STRING_SIZE - 1);
		strncpy(productionData->partNumber, inuStorage.storageLegacyLayout.PartNumber, PRODUCTION_STRING_SIZE - 1);
		strncpy(productionData->sensorRevision, inuStorage.storageLegacyLayout.SensorRevision, PRODUCTION_STRING_SIZE - 1);
		strncpy(productionData->serialNumber, inuStorage.storageLegacyLayout.SerialNumber, PRODUCTION_STRING_SIZE - 1);

		inuUsbParams->log(INU_USB_LOG_INFO, "inu_flash: legacy model_number: %s part_number: %s sensor_rev: %s serial_num: %s bootId=%d\n",
			productionData->modelNumber, productionData->partNumber, productionData->sensorRevision, productionData->serialNumber, bootId);
	}
	else if (magic_number== BOOTROM_STORAGE_NU3000_MAGIC_NUMBER)
	{
		read_err = inu_usb_monitor_read(offset, (unsigned char *)productionData, sizeof(InuProductionHeaderT), &transfered_size);
		if (read_err != INU_USB_ERR_SUCCESS)
		{
			return INU_USB_ERR_INIT_FAILED;
		}
		inuUsbParams->log(INU_USB_LOG_INFO, "inu_flash: model_number: %s  part_number: %s  sensor_rev: %s  serial_num: %s  bootId=%d  chip version 0x%x\n",
				productionData->modelNumber, productionData->partNumber, productionData->sensorRevision, productionData->serialNumber, productionData->bootId, inuUsbParams->version);
	}
	else
	{
		inuUsbParams->log(INU_USB_LOG_ERROR, "inu_flash: can't read production data (assuming bootId=0) \n");
		productionData->bootId = 0;
		strcpy(productionData->modelNumber, "unknown");
		strcpy(productionData->partNumber, "unknown");
		strcpy(productionData->sensorRevision, "unknown");
		strcpy(productionData->serialNumber, "unknown");

	}
	return INU_USB_ERR_SUCCESS;
}
/****************************************************************************
*
*  Function Name: inu_storage_read_bootdetect
*
*  Description: Activates the state machine of the bootdetect in the target.
*
*  Inputs:  productionData struct to fill in.
*
*  Outputs:
*
*  Returns:
*
*  Context: read flash
*
****************************************************************************/
int inu_bootdetect_init()
{
   InuUsbErrorE err;
   int ready = BOOTFLASH_STATUS_READY;   
   unsigned int transfered_size;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
   unsigned int size;
   InuProductionDataT productionData;

   // clean production data area before invoking bootdetect on target
   memset((void *)&productionData, 0, sizeof(InuProductionDataT));
   err = inu_usb_monitor_write(BOOTDETECT_PRODUCTION_DATA_ADDRESS, (unsigned char *)&productionData, (unsigned int)sizeof(InuProductionDataT));
   if (err != INU_USB_ERR_SUCCESS)
   {
	   inuUsbParams->log(INU_USB_LOG_ERROR, "inu_usb: can't access chip for writing, check USB connection...\n");
	   if (err != INU_USB_ERR_SUCCESS)
		   return -1;
   }
   
   ready = 0;
   err = inu_usb_monitor_write(BOOTDETECT_COMMAND_REG, (unsigned char *)&ready, sizeof(unsigned int));
   if (err != INU_USB_ERR_SUCCESS)
      return -1;
   if (inuUsbParams->version == NU3000_B0_CHIP_VERSION)
	   err = inu_usb_monitor_write(BOOTDETECT_LOAD_ADDRESS, bootdetect_nu3000b0, (unsigned int)bootdetect_nu3000b0_len);
   else if (inuUsbParams->version == NU4000_A0_CHIP_VERSION)
 	   err = inu_usb_monitor_write(BOOTDETECT_LOAD_ADDRESS, bootdetect_nu4000a0, (unsigned int)bootdetect_nu4000a0_len);
   else if (inuUsbParams->version == NU4000_B0_CHIP_VERSION)
 	   err = inu_usb_monitor_write(BOOTDETECT_LOAD_ADDRESS, bootdetect_nu4000b0, (unsigned int)bootdetect_nu4000b0_len);
   else if (inuUsbParams->version == NU4000_C0_CHIP_VERSION)
 	   err = inu_usb_monitor_write(BOOTDETECT_LOAD_ADDRESS, bootdetect_nu4000c0, (unsigned int)bootdetect_nu4000c0_len);
   else if ((inuUsbParams->version == NU4000_C1_CHIP_VERSION)||  (inuUsbParams->version == NU4100_CHIP_VERSION))
 	   err = inu_usb_monitor_write(BOOTDETECT_LOAD_ADDRESS, bootdetect_nu4000c1, (unsigned int)bootdetect_nu4000c1_len);
    err = inu_usb_monitor_jump(BOOTDETECT_LOAD_ADDRESS);
   if (err != INU_USB_ERR_SUCCESS)
	   return -1;
   
   do
   {
      err = inu_usb_monitor_read(BOOTDETECT_COMMAND_REG, (unsigned char *)&ready, sizeof(unsigned int), &transfered_size);
      if (err != INU_USB_ERR_SUCCESS)
         return -1;
      host_millisleep(50);
   } while (ready != BOOTDETECT_STATUS_FINISH);

   return 0;
}

int inu_bootdetect_get_bootid()
{
	InuUsbErrorE err;
	InuProductionHeaderT productionData;
	unsigned int magic_number = 0;
	unsigned int transfered_size;
	int target_addr = BOOTDETECT_PRODUCTION_DATA_ADDRESS;
	InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

	//reads the magic number (that bootdetect write) from the cram.
	err = inu_usb_monitor_read(target_addr, (unsigned char *)&magic_number, sizeof(unsigned int), &transfered_size);
   if (err == INU_USB_ERR_SUCCESS)
   {
   	bootdetect_read_production_data(&productionData, target_addr + sizeof(unsigned int), magic_number);

   	inuUsbParams->log(INU_USB_LOG_INFO, "inu_usb_bootdetect: detected bootId=%d\n", productionData.bootId);
   	inuUsbParams->log(INU_USB_LOG_INFO, "\t\t\tmodelNumber=%s\n", productionData.modelNumber);
   	inuUsbParams->log(INU_USB_LOG_INFO, "\t\t\tserialNumber=%s\n", productionData.serialNumber);
   	inuUsbParams->log(INU_USB_LOG_INFO, "\t\t\tsensorRevision=%s\n", productionData.sensorRevision);
   	inuUsbParams->log(INU_USB_LOG_INFO, "\t\t\tpartNumber=%s\n", productionData.partNumber);
   }
   else
   {
      inuUsbParams->log(INU_USB_LOG_INFO, "inu_usb_bootdetect: failed to read production data (err=%d)\n", err);
      productionData.bootId = 0;
   }  
	return productionData.bootId;
}






