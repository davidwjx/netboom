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
InuUsbParamsT inuParams;
int main(int argc, char *argv[])
{   	   
   unsigned char *bufP = NULL;      
   
   printf("----- inu_usb_test %s -----\n----------------------------- \nbuild %s @ %s\n-----------------------------\n", VERSION_STR, __DATE__, __TIME__); 
   if (argc == 1 || argc >3) {
	    print_usage(argv[0]);
		exit(10);
   }

   memset(&inuParams, sizeof(inuParams), 0);
   inuParams.hp_callback = hotplug_callback;
   strcpy(inuParams.boot_path,(const char *)argv[1]);   
   inuParams.logLevel = INU_USB_LOG_INFO;
   inuParams.bootId=-1;
   if (inu_usb_init(&inuParams) != INU_USB_ERR_SUCCESS) {
	   printf("Can't start boot service of inu_usb_lib\n");   
	   return 1;	
   }

	printf("HotK monitor initialized successfully!\n");
	printf("Press 'CTL^C' to exit...\n\n");

	for(;;)
	{	
		host_millisleep(500);	
	}

	inu_usb_deinit();	
}
