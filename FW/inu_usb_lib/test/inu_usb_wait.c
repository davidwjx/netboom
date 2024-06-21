#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "inu_usb.h"
#include "inu_host_utils.h"

#define VERSION_STR   "1.0"

#define WAIT_MODE_RAW	1
#define WAIT_MODE_BOOT	2
#define	DETECTION_TIMEOUT 5*60 //5 min

int testMode=0;
int deviceDetected=0;

static void print_usage(char *my_name)
{
   printf("%s <timeout in s> <path_to_boot_directory>\n", my_name);
}


int hotplug_callback(InuUsbStatusE status)
{
	if (status == INU_USB_EVT_INSERTION) {
			printf("inu_usb_test: hotplug callback: NU3000 raw insertion event\n");
			if (testMode == WAIT_MODE_RAW)
				deviceDetected = 1;
	}		
	else if (status == INU_USB_EVT_BOOT_COMPLETE) {
		printf("inu_usb_test: hotplug callback: NU3000 boot complete event\n");
		if (testMode == WAIT_MODE_BOOT)
				deviceDetected = 1;
	}else if (status == INU_USB_EVT_REMOVAL)
		printf("inu_usb_test: hotplug callback: NU3000 removal event\n");
	else
		printf("inu_usb_test: hotplug callback: NU3000 unknown event\n");
	return 0;
}
InuUsbParamsT inuParams;

int main(int argc, char *argv[])
{   	   
   unsigned char *bufP = NULL;
   int i, timeout_in_sec;
   
   printf("----- inu_usb_wait %s -----\n----------------------------- \nbuild %s @ %s\n-----------------------------\n", VERSION_STR, __DATE__, __TIME__); 
   testMode = WAIT_MODE_BOOT;
   if (argc  < 2) {	   
	   print_usage(argv[0]);   
	   exit(10);
   }

   timeout_in_sec = atoi(argv[1]);
   memset(&inuParams, sizeof(inuParams), 0);
   if (argc > 2)
	   strcpy(inuParams.boot_path,(const char *)argv[2]); 
   inuParams.hp_callback = hotplug_callback;   
   inuParams.logLevel = INU_USB_LOG_INFO;

   if (inu_usb_init(&inuParams) != INU_USB_ERR_SUCCESS) {
	   printf("Can't start boot service of inu_usb_lib\n");   
	   return 1;	
   }

	printf("Waiting for device insertion(%d sec tmeout)!\n",timeout_in_sec);	
	for(i=0;!deviceDetected && i < timeout_in_sec*2; i++)
	{			
		host_millisleep(500);
	}
	inu_usb_deinit();
	if (i == timeout_in_sec*2)
		return 1;

	return 0;
}
