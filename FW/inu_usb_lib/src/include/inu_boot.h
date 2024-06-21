/****************************************************************************
 *
 *   FileName: inu_usb.h
 *
 *   Author:  Konstantin Sinyuk
 *
 *   Date: 
 *
 *   Description: Inuitive user space abstraction for USB boot module
 *   
 ****************************************************************************/
#ifndef INU_BOOT_H
#define INU_BOOT_H

#include "inu_usb.h"

typedef enum
{	
	INU_USB_MODE_PLUGGED_RAW 	= 0,
	INU_USB_MODE_PLUGGED_OS		= 1,
	INU_USB_MODE_UNPLUGGED		= 2,
	INU_USB_MODE_DETECT_CHIP_VERSION = 3,
} InuUsbModeE; 

// Important: KERNEL_BCD_DEVICE_MASK allows to check MSB value of bcdDevice
//			  The MSB value 0 is for kernel,  1 for bootrom
#define KERNEL_BCD_DEVICE_MASK		0x8000

InuUsbErrorE inu_usb_boot_hotplug_handler(InuUsbModeE mode, char *boot_path, InuHotplugCallback hp_callback);
void inu_console_log(InuUsbLogLevelE level, const char * fmt, ...);

#ifdef __cplusplus
extern "C" {
#endif
//InuUsbErrorE inu_usb_monitor_read(unsigned int address, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
//InuUsbErrorE inu_usb_monitor_write(unsigned int address, unsigned char *buffer, unsigned int size);
//InuUsbErrorE inu_usb_monitor_jump(unsigned int address);
//InuUsbParamsT *inu_usb_get_contex();
InuUsbErrorE inu_usb_set_mode(unsigned char usbInterfaceIdx, InuUsbModeE mode);
#ifdef __cplusplus
}
#endif

#endif // INU_BOOT_H 