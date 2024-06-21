/****************************************************************************
 *
 *   FileName: inu_usb.h
 *
 *   Author:  Konstantin Sinyuk
 *
 *   Date: 
 *
 *   Description: Inuitive user space abstraction for USB access
 *   
 ****************************************************************************/
#ifndef INU_USB_H
#define INU_USB_H

#ifdef INUUSB_EXPORTS
    #ifdef __GNUC__
              #ifdef __linux__
                     #define INUUSB_API __attribute__((visibility("default")))
              #else
                     #define INUUSB_API __attribute__ ((visibility ("default")))
              #endif
    #else
        #define INUUSB_API __declspec(dllexport)
    #endif
    #define INUUSB_API_TEMPLATE
#else
    #ifdef __GNUC__
              #ifdef __linux__
                     #define INUUSB_API
              #else
                     #define INUUSB_API __attribute__ ((visibility ("default")))
              #endif
    #else
        #define INUUSB_API __declspec(dllimport)
    #endif
    #define INUUSB_API_TEMPLATE extern
#endif

#define _FILE_ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)

typedef enum
{
   INU_USB_EVT_REMOVAL        = 0,
   INU_USB_EVT_INSERTION      = 1,
   INU_USB_EVT_BOOT_COMPLETE  = 2,
   INU_USB_EVT_BOOT_FAILED    = 3,
   INU_USB_EVT_FILTERID_UPDATED = 4,
} InuUsbStatusE; 

typedef enum
{
   INU_USB_EVT_RAW_REMOVAL       = 0,
   INU_USB_EVT_RAW_INSERTION	 = 1,
} InuUsbRawStatusE; 

typedef enum
{
   INU_USB_ERR_SUCCESS        = 0,
   INU_USB_ERR_INIT_FAILED    = 1,
   INU_USB_ERR_LIB_NO_MATCH   = 2,
   INU_USB_ERR_IO_FAILED      = 3,
   INU_USB_ERR_DISCONNECTED   = 4,
   INU_USB_ERR_INIT_CALLBACK  = 5,
   INU_USB_ERR_BOOT_FAILURE   = 6,
   INU_USB_ERR_FILE_ACCESS    = 7,
   INU_USB_ERR_OVERFLOW       = 8,
   INU_USB_ERR_NO_DEVICE      = 9,
   INU_USB_ERR_CLOSE_TIMEOUT  = 10,
   INU_USB_ERR_ALLOC_FAILURE  = 11,
   INU_USB_ERR_TRANSFER_FAILURE = 12,
} InuUsbErrorE;

typedef enum
{
   INU_USB_LOG_ERROR    = 0,
   INU_USB_LOG_WARN     = 1,
   INU_USB_LOG_INFO     = 2,
   INU_USB_LOG_DEBUG    = 3,
} InuUsbLogLevelE;

#define INU_VENDOR_ID               (0x2959)
#define INU_CONTROL_INTERFACE_NAME  "INU_control"
#define CTL_OUT_EP                  0x1
#define CTL_IN_EP                   0x81
#define INT_OUT_EP                  0x2
#define INT_IN_EP                   0x82


#ifdef __cplusplus
extern "C" {
#endif

typedef int (*InuHotplugCallback)(InuUsbStatusE status, unsigned int val);
typedef int (*InuIntCallback)(unsigned char *buf, int actual_length);
typedef void (*InuLog)(InuUsbLogLevelE level, const char * fmt, ...);

typedef struct 
{
   char                 boot_path[1024];
   InuHotplugCallback   hp_callback;
   InuLog               log;
   InuUsbLogLevelE      logLevel;   
   int                  filterId;
   int                  bootId;
   int                  version;
   int                  usbDeviceAddress;
   int                  usbFd;
   int                  usbUseExternalHpCallback;
   int                  thrSleepTimeUs;
} InuUsbParamsT;


INUUSB_API
InuUsbErrorE inu_usb_init(InuUsbParamsT *params);
INUUSB_API
InuUsbErrorE inu_usb_deinit();
INUUSB_API
InuUsbErrorE inu_usb_change_params(InuUsbParamsT *params);
INUUSB_API
InuUsbErrorE inu_usb_open(unsigned char usbInterfaceIdx);
INUUSB_API
InuUsbErrorE inu_usb_open_non_ctl_enp();
INUUSB_API
InuUsbErrorE inu_usb_close_non_ctl_enp();
INUUSB_API
InuUsbErrorE inu_usb_close(unsigned char usbInterfaceIdx);
INUUSB_API
InuUsbErrorE inu_usb_hotplug_callback_external(InuUsbRawStatusE usbEvent, int usbFd);
INUUSB_API
InuUsbErrorE inu_usb_write_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size,  unsigned int *transfered_size);
INUUSB_API 
InuUsbErrorE inu_usb_read_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
INUUSB_API 
InuUsbErrorE inu_usb_buffered_read_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
INUUSB_API
InuUsbErrorE inu_usb_write_interrupt(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size,  unsigned int *transfered_size);
INUUSB_API 
InuUsbErrorE inu_usb_read_interrupt(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
INUUSB_API 
InuUsbErrorE inu_usb_init_interrupt_async(unsigned char interfaceIdx, unsigned int transfer_size, unsigned int num_of_transfers, InuIntCallback intCallback);
INUUSB_API 
InuUsbErrorE inu_usb_deinit_interrupt_async(unsigned char interfaceIdx);
INUUSB_API
InuUsbErrorE inu_usb_monitor_write(unsigned int address, unsigned char *buffer, unsigned int size);
INUUSB_API
InuUsbErrorE inu_usb_monitor_read(unsigned int address, unsigned char *buffer, unsigned int size, unsigned int *transfered_size);
INUUSB_API
InuUsbErrorE inu_usb_monitor_jump(unsigned int address);
INUUSB_API
InuUsbParamsT *inu_usb_get_contex();
INUUSB_API
int inu_usb_get_descriptor_bootId();
INUUSB_API
InuUsbErrorE inu_usb_get_boot_path(char *bootPath);
INUUSB_API
int inu_usb_boot_is_reset_in_progress();
INUUSB_API
void inu_usb_boot_set_reset_in_progress(int reset_flag);

#ifdef __cplusplus
}
#endif

#endif // INU_USB_H 
