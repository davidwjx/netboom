/****************************************************************************
 * 
 *   FileName: usb_ctrl_com.c
 *
 *   Author:
 *
 *   Date: 
 *
 *   Description: USB control com interface 
 *
 ****************************************************************************/
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#define INUUSB_EXPORTS


#include "inu_usb.h"
#include "inu_boot.h"
#include "libusbk_defs.h"
#include "libusbk.h"

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
// The name below should correspond to .inf file 
#define INU_CONTROL_INTERFACE_NAME	"INU_control"
#define INU_CONTROL_INTERFACE_NAME1	"INU_stream"

#define CTL_OUT_EP	0x1
#define CTL_IN_EP	0x81
#define USB_READ_RETRY_COUNT  5
#define USB_INTERFCAE_CTRL_IDX  0
#define USB_INTERFCAE_NUM  7

typedef struct {
	KUSB_HANDLE usbHandle;
	KUSB_DRIVER_API Usb;
	KLST_HANDLE deviceList;
	KLST_DEVINFO_HANDLE deviceInfo;
	KHOT_HANDLE hotHandle;
	InuUsbParamsT inuUsbParams;
	InuHotplugCallback hpCallback;
	char bootPath[1024];
	InuLog	log;
} UsbDeviceInfoT;

/****************************************************************************
 ***************       G L O B A L        D A T A              ***************
 ****************************************************************************/

int g_isUsbInserted=0;
UsbDeviceInfoT g_inuUsDeviceInfo[USB_INTERFCAE_NUM] ;

/****************************************************************************
 ***************       L O C A L         F U N C              ***************
 ****************************************************************************/

static BOOL  GetDeviceDescriptor(UsbDeviceInfoT *usbDevice, USB_DEVICE_DESCRIPTOR *deviceDescriptor)
{
	DWORD errorCode = ERROR_SUCCESS;
	BOOL success;
	KUSB_SETUP_PACKET setupPacket;

	/*
	Use a standard control transfer to get the device descriptor. (DeviceToHost)
	*/

	// Setup packets are always 8 bytes (64 bits)
	*((__int64*)&setupPacket) = 0;

	// Fill the setup packet.
	setupPacket.BmRequest.Dir = BMREQUEST_DIR_DEVICE_TO_HOST;
	setupPacket.BmRequest.Type = BMREQUEST_TYPE_STANDARD;
	setupPacket.BmRequest.Recipient = BMREQUEST_RECIPIENT_DEVICE;
	setupPacket.Value = USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(USB_DESCRIPTOR_TYPE_DEVICE, 0);
	setupPacket.Request = USB_REQUEST_GET_DESCRIPTOR;
	setupPacket.Length = sizeof(deviceDescriptor);
	success = usbDevice->Usb.ControlTransfer(usbDevice->usbHandle, *((WINUSB_SETUP_PACKET*)&setupPacket), (PUCHAR)deviceDescriptor, sizeof(USB_DEVICE_DESCRIPTOR), NULL, NULL);
	if (!success)
	{
		errorCode = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "Can't get device descriptor. Win32Error=%u (0x%08X)\n", errorCode, errorCode);
		return success;
	}
	return success;
}

static int MatchMultiDevice(UsbDeviceInfoT *usbDevice)
{
	DWORD errorCode = ERROR_SUCCESS;
	BOOL success = FALSE;
	USB_DEVICE_DESCRIPTOR deviceDescriptor;

	if (usbDevice->usbHandle)
		success = GetDeviceDescriptor(usbDevice, &deviceDescriptor);

	if (success && deviceDescriptor.bDeviceSubClass == usbDevice->inuUsbParams.usbDeviceAddress) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_lib: multi device match detected\n");
		return 1;
	}

	return 0;
}
static int isKernelReEnumeration(UsbDeviceInfoT *usbDevice)
{
	DWORD errorCode = ERROR_SUCCESS;
	BOOL success = FALSE;
	USB_DEVICE_DESCRIPTOR deviceDescriptor;

	if (usbDevice->usbHandle)
		success = GetDeviceDescriptor(usbDevice, &deviceDescriptor);

	if (success && ((deviceDescriptor.bcdDevice & KERNEL_BCD_DEVICE_MASK) == 0x0)) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_lib: kernel reenumertion detected\n");
		return 1;
	}
	return 0;
}

VOID KUSB_API OnHotPlug(
	KHOT_HANDLE Handle,
	KLST_DEVINFO_HANDLE DeviceInfo,
	KLST_SYNC_FLAG NotificationType)
{
	int i = USB_INTERFCAE_CTRL_IDX;
	UNREFERENCED_PARAMETER(Handle);


	// Detect only INU_control interface and skip others		
	i = 0;
	if (strstr(DeviceInfo->DeviceDesc, INU_CONTROL_INTERFACE_NAME) != NULL)
	{
		i = USB_INTERFCAE_CTRL_IDX;
	}
	else if (strstr(DeviceInfo->DeviceDesc, INU_CONTROL_INTERFACE_NAME1) != NULL)
	{
		i = 1;
	}
	else
	{
		return;
	}
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[i]);
	if (NotificationType == KLST_SYNC_FLAG_ADDED) {
		// open USB instance on each insertion
		usbDevice->deviceInfo = DeviceInfo;
		if (i == USB_INTERFCAE_CTRL_IDX)
		{
			inu_usb_open(USB_INTERFCAE_CTRL_IDX);
			g_isUsbInserted = 1;
			// check for mutlti device match
			if (!MatchMultiDevice(usbDevice))
				return;

			// check if it's bootrom or kernel enumeration
			if (isKernelReEnumeration(usbDevice))
				inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_OS, usbDevice->bootPath, usbDevice->hpCallback);
			else
				inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_RAW, usbDevice->bootPath, usbDevice->hpCallback);
		}
	}
	else {
		if (i == USB_INTERFCAE_CTRL_IDX)
		{
			g_isUsbInserted = 0;
			inu_usb_boot_hotplug_handler(INU_USB_MODE_UNPLUGGED, usbDevice->bootPath, usbDevice->hpCallback);
			inu_usb_close(USB_INTERFCAE_CTRL_IDX);
			usbDevice->deviceInfo = NULL;
		}
	}

}
/****************************************************************************
 ***************       G L O B A L       F U N C              ***************
 ****************************************************************************/
INUUSB_API
InuUsbParamsT *inu_usb_get_contex()
{
   return &g_inuUsDeviceInfo[USB_INTERFCAE_CTRL_IDX].inuUsbParams;
}

InuUsbErrorE inu_usb_set_mode(unsigned char usbInterfaceIdx, InuUsbModeE mode)
{
	BYTE polShortPacketTerminate;
    BYTE polIgnoreShortPacket;
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[usbInterfaceIdx]);	

	usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"inu_usb_set_mode: mode=%d\n",mode);
	if (mode == INU_USB_MODE_UNPLUGGED)
	{
		usbDevice->Usb.AbortPipe(usbDevice->usbHandle, CTL_IN_EP+usbInterfaceIdx);
		usbDevice->Usb.FlushPipe(usbDevice->usbHandle, CTL_IN_EP+usbInterfaceIdx);
		usbDevice->Usb.AbortPipe(usbDevice->usbHandle, CTL_OUT_EP+usbInterfaceIdx);
		usbDevice->Usb.FlushPipe(usbDevice->usbHandle, CTL_OUT_EP+usbInterfaceIdx);
		return INU_USB_ERR_SUCCESS;
	}

	if(!usbDevice->Usb.ResetPipe(usbDevice->usbHandle,(CTL_OUT_EP+usbInterfaceIdx)))
	{
		int ret=0;
		ret = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "usbDevice->Usb.ResetPipe ret=%d \n", ret);
		return INU_USB_ERR_IO_FAILED;
	}

	if(!usbDevice->Usb.ResetPipe(usbDevice->usbHandle,(CTL_IN_EP+usbInterfaceIdx)))
	{
		int ret=0;
		ret = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "usbDevice->Usb.ResetPipe ret=%d \n", ret);
		return INU_USB_ERR_IO_FAILED;
	}

	if (mode == INU_USB_MODE_PLUGGED_OS) {		
		polShortPacketTerminate  = 1;
		polIgnoreShortPacket     = 1;
	} else  {
		polShortPacketTerminate  = 0;	
		polIgnoreShortPacket     = 0;
	}

	if(!usbDevice->Usb.SetPipePolicy(usbDevice->usbHandle, CTL_OUT_EP+usbInterfaceIdx, SHORT_PACKET_TERMINATE, 1, &polShortPacketTerminate))
	{
		int ret=0;
		ret = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "usbDevice->Usb.SetPipePolicy ret=%d \n", ret);
		return INU_USB_ERR_IO_FAILED;     
	}
	if(!usbDevice->Usb.SetPipePolicy(usbDevice->usbHandle, CTL_IN_EP+usbInterfaceIdx, IGNORE_SHORT_PACKETS, 1, &polIgnoreShortPacket))
	{
		int ret=0;
		ret = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "usbDevice->Usb.SetPipePolicy ret=%d \n", ret);
		return INU_USB_ERR_IO_FAILED;
	}

	return INU_USB_ERR_SUCCESS;
}

/****************************************************************************
 ***************       E X P O R T E D       F U N C          ***************
 ****************************************************************************/

INUUSB_API
InuUsbErrorE inu_usb_init(InuUsbParamsT *inuUsbParams)
{
	KHOT_PARAMS hotParams;		
	int i=0;

	for (i=0;i<USB_INTERFCAE_NUM;++i)
	{
		UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[i]);

		if (!usbDevice->inuUsbParams.log)		
			usbDevice->inuUsbParams.log = inu_console_log;
		
		if (i==USB_INTERFCAE_CTRL_IDX)
		{
			if (usbDevice->hpCallback)
				return INU_USB_ERR_INIT_CALLBACK;
			memcpy(&usbDevice->inuUsbParams, inuUsbParams, sizeof(InuUsbParamsT));	

            if (!usbDevice->inuUsbParams.log) 		
    			usbDevice->inuUsbParams.log = inu_console_log;

            usbDevice->hpCallback = inuUsbParams->hp_callback;
			if (inuUsbParams->boot_path) {
				strcpy(usbDevice->bootPath, inuUsbParams->boot_path);
			} else {
				usbDevice->bootPath[0]=0;
			}

			memset(&hotParams, 0, sizeof(hotParams));
			hotParams.OnHotPlug = OnHotPlug;
			hotParams.Flags = KHOT_FLAG_PLUG_ALL_ON_INIT;
			// A "real world" application should set a specific device interface guid if possible.
			// strcpy(hotParams.PatternMatch.DeviceInterfaceGUID, "{F676DCF6-FDFE-E0A9-FC12-8057DBE8E4B8}");
			strcpy(hotParams.PatternMatch.DeviceInterfaceGUID, "*");	
			sprintf(hotParams.PatternMatch.DeviceID,"USB\\VID_%x",INU_VENDOR_ID);	

			usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"Initialize a HotK device notification event monitor..\n");
			usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"Looking for 'DeviceInterfaceGUID's matching the pattern '%s'..\n", hotParams.PatternMatch.DeviceInterfaceGUID);

			// Initializes a new HotK handle.
			if (!HotK_Init(&usbDevice->hotHandle, &hotParams))	{		
				usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"HotK_Init failed. ErrorCode: %08Xh\n", GetLastError());
				return INU_USB_ERR_INIT_FAILED;
			}	
		}
	}
	return INU_USB_ERR_SUCCESS;
}


INUUSB_API
InuUsbErrorE inu_usb_change_params(InuUsbParamsT *inuUsbParams)
{         
   UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[USB_INTERFCAE_CTRL_IDX]);
   
   if (inuUsbParams->boot_path) {
      strcpy(usbDevice->bootPath, inuUsbParams->boot_path);
   }
   
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_deinit()
{
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[USB_INTERFCAE_CTRL_IDX]);
	DWORD errorCode = ERROR_SUCCESS;
	// Free the HotK handle.
	g_isUsbInserted = 0;
	inu_usb_close(USB_INTERFCAE_CTRL_IDX);
	usbDevice->deviceInfo = NULL;
	if (!HotK_Free(usbDevice->hotHandle))
	{		
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"HotK_Free failed. ErrorCode: %08Xh\n",  GetLastError());
		return INU_USB_ERR_INIT_FAILED;
	}
	usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"HotK monitor closed successfully!\n");
	memset(g_inuUsDeviceInfo,0,sizeof(g_inuUsDeviceInfo));

	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_open_non_ctl_enp()
{
   int i=1;
   InuUsbErrorE errorCode = INU_USB_ERR_SUCCESS;
	for (i=1;i<USB_INTERFCAE_NUM;++i)
	{
		UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[i]);
		if (usbDevice->usbHandle != NULL)
		{
			errorCode = inu_usb_open(i);
			if (errorCode!=INU_USB_ERR_SUCCESS)
			{
				return errorCode;
			}
		}
	}
	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_close_non_ctl_enp()
{
	int i=1;
	InuUsbErrorE errorCode = INU_USB_ERR_SUCCESS;
	for (i=1;i<USB_INTERFCAE_NUM;++i)
	{
		UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[i]);
		if (usbDevice->usbHandle != NULL)
		{
			errorCode = inu_usb_close(i);
			if (errorCode!=INU_USB_ERR_SUCCESS)
			{
				return errorCode;
			}
		}
	}
	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_open(UINT8 usbInterfaceIdx)
{
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[usbInterfaceIdx]); 
	if (usbDevice->deviceInfo == NULL) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"No valid device info, call inu_usb_init before...\n");
		return INU_USB_ERR_INIT_FAILED;
	}
	// Do not reopen USB driver if usb handle is valid
	if (usbDevice->usbHandle != NULL)
		return INU_USB_ERR_SUCCESS;

	LibK_LoadDriverAPI(&usbDevice->Usb, usbDevice->deviceInfo->DriverID);
	if (!usbDevice->Usb.Init(&usbDevice->usbHandle, usbDevice->deviceInfo))
	{		
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Init device failed. ErrorCode: %08Xh\n", GetLastError());
		usbDevice->usbHandle = NULL;
		return INU_USB_ERR_INIT_FAILED;
	}
    UCHAR pipeIndex = 0;
    WINUSB_PIPE_INFORMATION pipeInfo;
    while (usbDevice->Usb.QueryPipe(usbDevice->usbHandle, 0, pipeIndex++, &pipeInfo))
    {
        printf("  PipeId=0x%02X PipeType=0x%02X Interval=%u MaximumPacketSize=%u\n",
               pipeInfo.PipeId, pipeInfo.PipeType, pipeInfo.Interval, pipeInfo.MaximumPacketSize);
    }
	if (usbInterfaceIdx)
		inu_usb_set_mode(usbInterfaceIdx, INU_USB_MODE_PLUGGED_OS);	
	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_close(UINT8 usbInterfaceIdx)
{
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[usbInterfaceIdx]);
	if (usbDevice->usbHandle)
	{
		inu_usb_set_mode(usbInterfaceIdx, INU_USB_MODE_UNPLUGGED);		
		usbDevice->Usb.Free(usbDevice->usbHandle);
		usbDevice->usbHandle = NULL;
	}
	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_write_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size,  unsigned int *transfered_size)
{
	BOOL success;
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[interfaceIdx]);
	
	if (!g_isUsbInserted)
		return INU_USB_ERR_DISCONNECTED;

	success = usbDevice->Usb.WritePipe(usbDevice->usbHandle, CTL_OUT_EP+interfaceIdx, (PUCHAR)buffer, size, transfered_size, NULL);	
	if (success == TRUE && *transfered_size == size)
	{
		return INU_USB_ERR_SUCCESS;	
	}
	else
	{
		int ret=0;
		ret = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "inu_usb_write_bulk ret=%d \n", ret);
	}
	return INU_USB_ERR_IO_FAILED;
}

INUUSB_API 
InuUsbErrorE inu_usb_read_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
	BOOL success;
	UsbDeviceInfoT *usbDevice = &(g_inuUsDeviceInfo[interfaceIdx]);;
	 
	if (!g_isUsbInserted)
		return INU_USB_ERR_DISCONNECTED;

	success = usbDevice->Usb.ReadPipe(usbDevice->usbHandle, CTL_IN_EP+interfaceIdx, (PUCHAR)buffer, size, transfered_size, NULL);	

	if (success == TRUE) 
	{
		return INU_USB_ERR_SUCCESS;
	}
	else
	{
		int ret=0;
		ret = GetLastError();
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "inu_usb_read_bulk ret=%d \n", ret);
	}
	
	return INU_USB_ERR_IO_FAILED;
}

INUUSB_API
InuUsbErrorE inu_usb_buffered_read_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
	return inu_usb_read_bulk(interfaceIdx, buffer, size, transfered_size);
}


INUUSB_API
InuUsbErrorE inu_usb_hotplug_callback_external(InuUsbRawStatusE usbEvent, int usbFd)
{
	return INU_USB_ERR_SUCCESS;
}

#ifdef __cplusplus
}
#endif
