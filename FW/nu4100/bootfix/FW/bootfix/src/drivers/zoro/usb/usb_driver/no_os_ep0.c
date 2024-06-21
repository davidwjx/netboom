/**
	@brief  updates to Synopsys code

	@date December, 2012

	@author Avner Shapiro, Zoro Solutions Ltd

     <b> Copyright (c) 2010-2013 Zoro Solutions Ltd. </b>\n
    43 Hamelacha street, P.O. Box 8786, Poleg Industrial Park, Netanaya, ZIP 42505 Israel\n
    All rights reserved\n\n
    Proprietary rights of Zoro Solutions Ltd are involved in the
    subject matter of this material. All manufacturing, reproduction,
    use, and sales rights pertaining to this subject matter are governed
    by the license agreement. The recipient of this software implicitly
    accepts the terms of the license. This source code is the unpublished
    property and trade secret of Zoro Solutions Ltd.
    It is to be utilized solely under license from Zoro Solutions Ltd and it
    is to be maintained on a confidential basis for internal company use
    only. It is to be protected from disclosure to unauthorized parties,
    both within the Licensee company and outside, in a manner not less stringent
    than that utilized for Licensee's own proprietary internal information.
    No copies of the source or object code are to leave the premises of
    Licensee's business except in strict accordance with the license
    agreement signed by Licensee with Zoro Solutions Ltd.\n\n

    For more details - http://zoro-sw.com
    email: info@zoro-sw.com
*/

/** @file
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

#ifdef DWC_UTE
#include "ute_if.h"
#endif

#include "conf.h"
#include "usb_drv.h"
#include "sys_calls.h"
#include "fw_helpers.h"

/* enable writing of product serial to the usb descriptor */
#define ENABLE_SERIAL_STRING

/* taken from inu_storage definitions */
#ifdef ENABLE_SERIAL_STRING
#include "inu_storage_layout.h"
extern InuProductionHeaderT productionHdrG;
#endif

/*=======================================================================*/
/*
 * EP0 routines
 */


/* Make the following structure type definitions "packed" if using a Microsoft
 * compiler. The UPACKED attribute (defined in no_os_defs.h) embedded in the
 * structure type definitions does the same thing for GCC. Other compilers may
 * need something different.
 */
/* These standard USB descriptor types are defined in usb.h */


const static usb_device_descriptor_t device_desc = {
	USB_DEVICE_DESCRIPTOR_SIZE,		/* bLength */
	UDESC_DEVICE,				/* bDescriptorType */

	UCONSTW(0),				/* bcdUSB (filled in later) */
	UDCLASS_IN_INTERFACE,   /* bDeviceClass */
	0,					/* bDeviceSubClass */
	0,					/* bDeviceProtocol */
	0,					/* bMaxPacketSize */

	UCONSTW(DWC_VENDOR_ID),		/* idVendor */
	UCONSTW(DWC_PRODUCT_ID),	/* idProduct */
	UCONSTW(0xffff),			/* bcdDevice */

	DWC_STRING_MANUFACTURER,	/* iManufacturer */
	DWC_STRING_PRODUCT,			/* iProduct */
	DWC_STRING_SERIAL,			/* iSerialNumber */

	1,					/* bNumConfigurations */
};


const static usb_device_qualifier_t dev_qualifier  = {
	USB_DEVICE_QUALIFIER_SIZE,		/* bLength */
	UDESC_DEVICE_QUALIFIER,			/* bDescriptorType */

	UCONSTW(0),				/* bcdUSB (filled in later) */
	UDCLASS_IN_INTERFACE,		/* bDeviceClass */
	0,					/* bDeviceSubClass */
	0,					/* bDeviceProtocol */
	0,					/* bMaxPacketSize0 */
	1,					/* bNumConfigurations */
	0,					/* bReserved */
};


/* These application-specific config descriptor types are defined in
 * no_os_defs.h
 */


const fs_config_desc_t fs_config_desc = {
	/* config descriptor */
	{
		USB_CONFIG_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_CONFIG,			/* bDescriptorType */
		UCONSTW(sizeof(fs_config_desc_t)),			/* wTotalLength */
		1,				/* bNumInterface */
		1,				/* bConfigurationValue */
		0,				/* iConfiguration */
		UC_ATT_ONE,
		100 / UC_POWER_FACTOR,		/* bMaxPower (100 ma) */
	},
	/* interface descriptor */
	{
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		0,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		2,				/* bNumEndpoints */
		UICLASS_VENDOR,			/* bInterfaceClass */
		0,				/* bInterfaceSubClass */
		0,				/* bInterfaceProtocol */
		0,				/* iInterface */
	},
	/* bulk IN endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_IN,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(64),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk OUT endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_OUT,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(64),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
};

const hs_config_desc_t hs_config_desc = {
	/* config descriptor */
	{
		USB_CONFIG_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_CONFIG,			/* bDescriptorType */
		UCONSTW(0),			/* wTotalLength (filled in later) */
		1,				/* bNumInterface */
		1,				/* bConfigurationValue */
		0,				/* iConfiguration */
		UC_ATT_ONE | DWC_POWER_FLAGS_ADD,
		DWC_MAX_POWER_HS / UC_POWER_FACTOR,
	},
	/* interface descriptor */
	{
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		0,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		2,				/* bNumEndpoints */
		UICLASS_VENDOR,			/* bInterfaceClass */
		0,				/* bInterfaceSubClass */
		0,				/* bInterfaceProtocol */
		DWC_STRING_PROP_CTRL, /* iInterface */
	},
	/* bulk IN endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_IN,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(512),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk OUT endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_OUT,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(512),			/* wMaxPacketSize */
		0,				/* bInterval (every uframe) */
	},
};

const ss_config_desc_t ss_config_desc = {
	/* config descriptor */
	{
		USB_CONFIG_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_CONFIG,			/* bDescriptorType */
		UCONSTW(0),			/* wTotalLength (filled in later) */
		1,				/* bNumInterface */
		1,				/* bConfigurationValue */
		0,				/* iConfiguration */
		UC_ATT_ONE | DWC_POWER_FLAGS_ADD,
		DWC_MAX_POWER_SS / 8,
	},
	/* interface descriptor */
	{
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		0,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		2,				/* bNumEndpoints */
		UICLASS_VENDOR,			/* bInterfaceClass */
		0,				/* bInterfaceSubClass */
		0,				/* bInterfaceProtocol */
		DWC_STRING_PROP_CTRL, /* iInterface */
	},
	/* bulk IN endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_IN,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(1024),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk IN companion descriptor */
	{
		USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE, /* bLength */
		UDESC_SS_USB_COMPANION,		/* bDescriptorType */
		15,				/* bMaxBurst */
		0,				/* bmAttributes */
		UCONSTW(0),			/* wBytesPerInterval */
	},
	/* bulk OUT endpoint descriptor */
	{
		USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_ENDPOINT,			/* bDescriptorType */
		1 | UE_DIR_OUT,			/* bEndpointAddress */
		UE_BULK,			/* bmAttributes */
		UCONSTW(1024),			/* wMaxPacketSize */
		0,				/* bInterval */
	},
	/* bulk OUT companion descriptor */
	{
		USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE, /* bLength */
		UDESC_SS_USB_COMPANION,		/* bDescriptorType */
		15,				/* bMaxBurst */
		0,				/* bmAttributes */
		UCONSTW(0),			/* wBytesPerInterval */
	},
};

/** The BOS Descriptor */
const static usb_dev_cap_20_ext_desc_t cap1_desc = {
	sizeof(usb_dev_cap_20_ext_desc_t),	/* bLength */
	UDESC_DEVICE_CAPABILITY,		/* bDescriptorType */
	USB_DEVICE_CAPABILITY_20_EXTENSION,	/* bDevCapabilityType */
	UCONSTDW(USB_20_EXT_LPM | USB_20_EXT_BESL),		/* bmAttributes */

};

const static usb_dev_cap_ss_usb_t cap2_desc = {
	sizeof(usb_dev_cap_ss_usb_t),		/* bLength */
	UDESC_DEVICE_CAPABILITY,		/* bDescriptorType */
	USB_DEVICE_CAPABILITY_SS_USB,		/* bDevCapabilityType */
	0x0,					/* bmAttributes */
	UCONSTW(USB_DC_SS_USB_SPEED_SUPPORT_SS	/* wSpeedsSupported */
	    | USB_DC_SS_USB_SPEED_SUPPORT_HIGH),
	USB_DC_SS_USB_SPEED_SUPPORT_FULL,	/* bFunctionalitySupport */
	/* @todo set these to correct values */
	10,					/* bU1DevExitLat (10 us) */
	UCONSTW(256),				/* wU2DevExitLat (256 us) */
};

	/* The Denali host (and Win8) chokes on the optional container ID */
const static usb_dev_cap_container_id_t cap3_desc = {
	sizeof(usb_dev_cap_container_id_t),	/* bLength */
	UDESC_DEVICE_CAPABILITY,		/* bDescriptorType */
	USB_DEVICE_CAPABILITY_CONTAINER_ID,	/* bDevCapabilityType */
	0,					/* bReserved */
	GUID_INUITIVE_CONTAINER_BASE,
};

const static wusb_bos_desc_t bos_desc = {
	sizeof(wusb_bos_desc_t),		/* bLength */
	WUDESC_BOS,				/* bDescriptorType */

	UCONSTW(sizeof(wusb_bos_desc_t)		/* wTotalLength */
		+ sizeof(cap1_desc) + sizeof(cap2_desc) + sizeof(cap3_desc)),
	3,					/* bNumDeviceCaps */
};


const uWord nu3000_strings[DWC_MAX_STRINGS][24] =  {
	{
		UCONSTW(0x0409),
	},
	{
		UCONSTW('I'), UCONSTW('n'), UCONSTW('u'), UCONSTW('i'),
		UCONSTW('t'), UCONSTW('i'), UCONSTW('v'), UCONSTW('e'),
	},
	{
		UCONSTW('N'), UCONSTW('U'), UCONSTW('4'), UCONSTW('1'),
		UCONSTW('0'), UCONSTW('0')
	},
	{
		UCONSTW('0')
	},
	{
		UCONSTW('I'), UCONSTW('N'), UCONSTW('U'), UCONSTW('_'),
		UCONSTW('c'), UCONSTW('o'), UCONSTW('n'), UCONSTW('t'),
		UCONSTW('r'), UCONSTW('o'), UCONSTW('l')
	},
#if 0
	{
		UCONSTW('I'), UCONSTW('N'), UCONSTW('U'), UCONSTW('_'),
		UCONSTW('s'), UCONSTW('t'), UCONSTW('r'), UCONSTW('e'),
		UCONSTW('a'), UCONSTW('m')
	},
	{
		UCONSTW('I'), UCONSTW('N'), UCONSTW('U'), UCONSTW('_'),
		UCONSTW('c'), UCONSTW('o'), UCONSTW('l'), UCONSTW('o'),
		UCONSTW('r'), UCONSTW('_'), UCONSTW('c'), UCONSTW('a'),
		UCONSTW('m')
	},
	{
		UCONSTW('I'), UCONSTW('N'), UCONSTW('U'), UCONSTW('_'),
		UCONSTW('a'), UCONSTW('u'), UCONSTW('d'), UCONSTW('i'),
		UCONSTW('o')
	},
#endif
};

/* writable structures */
usb_string_descriptor_t nu3000_string_descriptors[DWC_MAX_STRINGS];
u8 hs_config_desc_buf[MAX_USB_CONF_TREE_SIZE];
int  hs_config_desc_buf_size;
u8 ss_config_desc_buf[MAX_USB_CONF_TREE_SIZE];
int  ss_config_desc_buf_size;
u8 fs_config_desc_buf[MAX_USB_CONF_TREE_SIZE];
int  fs_config_desc_buf_size;
u8 bos_desc_buf[MAX_USB_BOS_TREE_SIZE];
int  bos_desc_buf_size;
usb_device_descriptor_t device_desc_buf;
usb_device_qualifier_t dev_qualifier_buf;
nu3000_descriptor_override_t nu3000_descriptor_override;
u16 nu3000_iso_rate[DWC_ISO_MAX_ALT_SETTINGS];
/* define export overrides */
nu3000_usb_descriptors_export_t nu3000_usb_descriptors_export;

int pass_desc_rest(int ss, int (*f)(int, usb_descriptor_header_t *, int, int, void *), void *ctx)
{
	u8 *p;
	usb_descriptor_header_t *hdr;
	int curintf = 0, curalt = 0, len;

	if (ss) {
		p = ss_config_desc_buf + sizeof (ss_config_desc);
		len = ss_config_desc_buf_size - sizeof (ss_config_desc);
	}else {
		p = hs_config_desc_buf + sizeof (hs_config_desc);
		len = hs_config_desc_buf_size - sizeof (hs_config_desc);
	}

	for(; len > 0; len -= hdr->bLength, p += hdr->bLength)
	{
		hdr = (void *)p;
		if (hdr->bDescriptorType == DESC_INTERFACE_ASSOCIATION)
		{
			curintf = ((usb_interface_descriptor_t *)hdr)->bInterfaceNumber; /* bFirstInterface */
			curalt = -1;
		}
		else if (hdr->bDescriptorType == UDESC_INTERFACE)
		{
			curintf = ((usb_interface_descriptor_t *)hdr)->bInterfaceNumber;
			curalt = ((usb_interface_descriptor_t *)hdr)->bAlternateSetting;
		}

		if (f(ss, hdr, curintf, curalt, ctx))
			return -1;
	}
	return 0;
}

/**
 * This routine handles Get Descriptor requests.
 */
static int no_os_get_descriptor(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	u8 desc_type = UGETW(ctrl->wValue) >> 8;
	u8 desc_idx = UGETW(ctrl->wValue);
	u16 max = UGETW(ctrl->wLength);
	u8 *buf = pcd->ep0_status_buf;
	u16 len;

#ifdef DEBUG_EP0
	dwc_debug5(pcd->usb3_dev, "GET_DESCRIPTOR %02x.%02x v%04x i%04x l%04x\n",
		   ctrl->bmRequestType, ctrl->bRequest, UGETW(ctrl->wValue),
		   UGETW(ctrl->wIndex), UGETW(ctrl->wLength));
#endif

#ifdef USB_FOR_NU3000
	if (usb_pp.get_descriptor)
	{
		int err;
		if (usb_pp.get_descriptor(0, pcd, ctrl, &err))
			return err;
	}
#endif

	switch (desc_type) {
	case UDESC_DEVICE:
		if (desc_idx)
			return -DWC_E_NOT_SUPPORTED;

		switch (pcd->speed) {
		case USB_SPEED_SUPER:
			USETW(device_desc_buf.bcdUSB, 0x310);
			device_desc_buf.bMaxPacketSize = 9;
			break;

		case USB_SPEED_HIGH:
			USETW(device_desc_buf.bcdUSB, 0x210);
			device_desc_buf.bMaxPacketSize = 64;
			break;

		default:
			USETW(device_desc_buf.bcdUSB, 0x210);
			device_desc_buf.bMaxPacketSize = 64;
			break;
		}

		memcpy(buf, &device_desc_buf, sizeof(device_desc_buf));
		len = sizeof(device_desc_buf);
		break;

	case UDESC_CONFIG:
		if (desc_idx)
			return -DWC_E_NOT_SUPPORTED;

		buf = pcd->ep0_status_buf;

		switch (pcd->speed) {
		case USB_SPEED_SUPER:
			len = ss_config_desc_buf_size;
			memcpy(buf, ss_config_desc_buf, len);
			break;

		case USB_SPEED_HIGH:
			len = hs_config_desc_buf_size;
			memcpy(buf, hs_config_desc_buf, len);
			break;

		default:
			memcpy(buf, &fs_config_desc_buf, sizeof(fs_config_desc_buf_size));
			len = fs_config_desc_buf_size;
			break;
		}

		break;

	case UDESC_STRING:
		((usb_descriptor_header_t *)buf)->bDescriptorType = 0;
		if (desc_idx < DWC_MAX_STRINGS) {
			usb_string_descriptor_t *str_desc = &nu3000_string_descriptors[desc_idx];
			if (str_desc)
				memcpy(buf, (u8 *)(str_desc), str_desc->bLength);
		}
		if (((usb_descriptor_header_t *)buf)->bDescriptorType != UDESC_STRING)
			return -DWC_E_NOT_SUPPORTED;

		len = ((usb_descriptor_header_t *)buf)->bLength;
		break;

	case UDESC_DEVICE_QUALIFIER:
		if (desc_idx || pcd->speed == USB_SPEED_SUPER)
			return -DWC_E_NOT_SUPPORTED;

		memcpy(buf, &dev_qualifier_buf, sizeof(dev_qualifier));
		len = sizeof(dev_qualifier);
		break;

	case UDESC_OTHER_SPEED_CONFIGURATION:
		if (desc_idx || pcd->speed == USB_SPEED_SUPER)
			return -DWC_E_NOT_SUPPORTED;

		buf = pcd->ep0_status_buf;
		switch (pcd->speed) {
		case USB_SPEED_HIGH:
			memcpy(buf, fs_config_desc_buf, fs_config_desc_buf_size);
			len = fs_config_desc_buf_size;
			((fs_config_desc_t *)buf)->config_desc.bDescriptorType = UDESC_OTHER_SPEED_CONFIGURATION;
			break;

		default:
			memcpy(buf, hs_config_desc_buf, hs_config_desc_buf_size);
			len = hs_config_desc_buf_size;
			((hs_config_desc_t *)buf)->config_desc.bDescriptorType = UDESC_OTHER_SPEED_CONFIGURATION;
			break;

			break;
		}

		break;

	default:
		return -DWC_E_NOT_SUPPORTED;
	}

#ifdef USB_FOR_NU3000
	if (usb_pp.get_descriptor)
	{
		int err;
		if (usb_pp.get_descriptor(1, pcd, ctrl, &err))
			return err;
	}
#endif

	pcd->ep0state = EP0_IN_DATA_PHASE;
	pcd->ep0_req->dwc_req.length = len < max ? len : max;
	pcd->ep0_status_pending = 1;
	pcd->ep0_req->dwc_req.buf[0] = (char *)pcd->ep0_status_buf;
	pcd->ep0_req->dwc_req.bufdma[0] = pcd->ep0_status_buf_dma;
	pcd->ep0_req->dwc_req.actual = 0;
	dwc_usb3_pcd_ep0_start_transfer(pcd, pcd->ep0_req);

	return 0;
}

/**
 * This routine processes SETUP commands. The USB Command processing is
 * done in two places - the first being the PCD and the second being the
 * Gadget driver (for example, the File-Backed Storage Gadget driver).
 *
 * <table>
 * <tr><td> Command </td><td> Driver </td><td> Description </td></tr>
 *
 * <tr><td> SET_FEATURE </td><td> PCD / Gadget driver </td><td> Device
 * and Endpoint requests are processed by the PCD. Interface requests
 * are passed to the Gadget driver. </td></tr>
 *
 * <tr><td> CLEAR_FEATURE </td><td> PCD </td><td> Device and Endpoint
 * requests are processed by the PCD. Interface requests are ignored.
 * The only Endpoint feature handled is ENDPOINT_HALT. </td></tr>
 *
 * <tr><td> GET_DESCRIPTOR </td><td> Gadget driver </td><td> Return the
 * requested descriptor. </td></tr>
 *
 * <tr><td> SET_DESCRIPTOR </td><td> Gadget driver </td><td> Optional -
 * not implemented by any of the existing Gadget drivers. </td></tr>
 *
 * <tr><td> GET_CONFIGURATION </td><td> Gadget driver </td><td> Return
 * the current configuration. </td></tr>
 *
 * <tr><td> SET_CONFIGURATION </td><td> Gadget driver </td><td> Disable
 * all EPs and enable EPs for new configuration. </td></tr>
 *
 * <tr><td> GET_INTERFACE </td><td> Gadget driver </td><td> Return the
 * current interface. </td></tr>
 *
 * <tr><td> SET_INTERFACE </td><td> Gadget driver </td><td> Disable all
 * EPs and enable EPs for new interface. </td></tr>
 * </table>
 *
 * When the SETUP Phase Done interrupt occurs, the generic SETUP commands
 * are processed by dwc_usb3_do_setup(). Calling the gadget driver's
 * dwc_usb3_gadget_setup() routine from dwc_usb3_do_setup() in turn calls
 * this routine to process the gadget-specific SETUP commands. Any requests
 * not handled here are passed to the Function Driver's
 * dwc_usb3_function_setup() routine.
 */
int dwc_usb3_no_os_setup(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	dwc_debug2(pcd->usb3_dev, "%s(%lx)\n", __func__, (unsigned long)pcd);

	if ((UT_GET_TYPE(ctrl->bmRequestType)) != UT_STANDARD) {
		/* handle non-standard (class/vendor) requests
		 * in the Function Driver
		 */
		return dwc_usb3_function_setup(pcd, ctrl);
	}

	/* @todo NGS: Handle bad setup packet? */

///////////////////////////////////////////
//// --- Standard Request handling --- ////

	switch (ctrl->bRequest) {
	case UR_SET_FEATURE:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_SET_INTERFACE:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_GET_INTERFACE:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_SET_CONFIG:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_GET_CONFIG:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_SYNCH_FRAME:
		return dwc_usb3_function_setup(pcd, ctrl);

	case UR_GET_DESCRIPTOR:
		return no_os_get_descriptor(pcd, ctrl);

	default:
		/* Call the Function Driver setup routines */
		return dwc_usb3_function_setup(pcd, ctrl);
	}

	return 0;
}

//#define INCLUDE_ISO_INTERFACE
#ifdef INCLUDE_ISO_INTERFACE
static usb_interface_descriptor_t iso_iface;
static hs_config_desc_alt_t hs_iso_alt_setting;
static ss_config_desc_alt_t ss_iso_alt_setting;
static unsigned int create_config_iso_interface(u8 *buffer, int is_ss)
{
	int i;
	int max_packet_size, mult, burst, bytes_in_interval;
	unsigned int offset=0;

	iso_iface = (usb_interface_descriptor_t){

					USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
					UDESC_INTERFACE,		/* bDescriptorType */
					DWC_ISO_INTERFACE_NUMBER,	/* bInterfaceNumber */
					0,				/* bAlternateSetting */
					0,				/* bNumEndpoints */
					0xff,			/* bInterfaceClass - Vendor */
					0,				/* bInterfaceSubClass - Video Streaming */
					0,				/* bInterfaceProtocol */
					DWC_STRING_PROP_STREAM,		/* iInterface */
	};
	hs_iso_alt_setting = (hs_config_desc_alt_t){
			{
					USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
					UDESC_INTERFACE,		/* bDescriptorType */
					DWC_ISO_INTERFACE_NUMBER,	/* bInterfaceNumber */
					0,				/* bAlternateSetting */
					1,				/* bNumEndpoints */
					0xff,			/* bInterfaceClass */
					0,				/* bInterfaceSubClass */
					0,				/* bInterfaceProtocol */
					0,				/* iInterface */
			},
			{
					USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
					UDESC_ENDPOINT,			/* bDescriptorType */
					DWC_ISO_ENDPOINT_NUMBER  | UE_DIR_IN,	/* bEndpointAddress */
					UE_ISOCHRONOUS | UE_ISO_ASYNC,	/* bmAttributes */
					UCONSTW(0),			/* wMaxPacketSize */
					1,				/* bInterval */
			}
	};

	ss_iso_alt_setting = (ss_config_desc_alt_t){
			{
					USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
					UDESC_INTERFACE,		/* bDescriptorType */
					DWC_ISO_INTERFACE_NUMBER,	/* bInterfaceNumber */
					0,				/* bAlternateSetting */
					1,				/* bNumEndpoints */
					0xff,			/* bInterfaceClass */
					0,				/* bInterfaceSubClass */
					0,				/* bInterfaceProtocol */
					0,				/* iInterface */
			},
			{
					USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */
					UDESC_ENDPOINT,			/* bDescriptorType */
					DWC_ISO_ENDPOINT_NUMBER  | UE_DIR_IN,	/* bEndpointAddress */
					UE_ISOCHRONOUS | UE_ISO_ASYNC,	/* bmAttributes */
					UCONSTW(0),			/* wMaxPacketSize */
					1,				/* bInterval */
			},
			{
					USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,/* bLength */
					UDESC_SS_USB_COMPANION,		/* bDescriptorType */
					0,			/* bMaxBurst */
					0,			/* bmAttributes */
					UCONSTW(0),		/* wBytesPerInterval */
			}
	};

	memcpy(buffer+offset, &iso_iface, sizeof(iso_iface));
	offset += sizeof(iso_iface);

	for (i=0; i< DWC_ISO_MAX_ALT_SETTINGS; i++) {
		bytes_in_interval = nu3000_iso_rate[i];
		if (bytes_in_interval == 0)
			break;
		if (!is_ss && (bytes_in_interval > 3072))
			break;
		if (bytes_in_interval <= 1024) {
			max_packet_size = bytes_in_interval;
			mult = 0;
			burst = 0;
		}else if (bytes_in_interval >= 49152) {
			max_packet_size = 1024;
			mult = 2;
			burst = 15;
		}else {
			max_packet_size = 1024;
			mult = bytes_in_interval/1024/16;
			if ((bytes_in_interval%(1024*(mult+1)) != 0))
				burst = (int)(bytes_in_interval/(1024*(mult+1)))+1;
			else
				burst = bytes_in_interval/1024/(mult+1);
			burst--; //fill descriptor with burst-1 value
		}

		if (is_ss) {
			// Deal with Q1’09 USB 3.0 Errata ( if burst = 0, mult must be 0 either)
			if (burst == 0) {
				burst = mult;
				mult = 0;
			}
			ss_iso_alt_setting.iso_intf_desc.bAlternateSetting = i+1;
			USETW(ss_iso_alt_setting.iso_in_ep_desc.wMaxPacketSize, max_packet_size);
			ss_iso_alt_setting.iso_in_ss_ep_comp_desc.bMaxBurst = burst;
			ss_iso_alt_setting.iso_in_ss_ep_comp_desc.bmAttributes = mult;

			memcpy(buffer+offset, &ss_iso_alt_setting, sizeof(ss_iso_alt_setting));
			offset += sizeof(ss_iso_alt_setting);
		} else {
			hs_iso_alt_setting.iso_intf_desc.bAlternateSetting = i+1;
			USETW(hs_iso_alt_setting.iso_in_ep_desc.wMaxPacketSize, max_packet_size | (mult << 11));

			memcpy(buffer+offset, &hs_iso_alt_setting, sizeof(hs_iso_alt_setting));
			offset += sizeof(hs_iso_alt_setting);
		}

	}
	return offset;
}
#endif

static int fill_configuration_descriptor(int ss)
{
	int desc_len, len=0;
	u8 *target_desc;
	usb_config_descriptor_t *config_desc;

	// Apply  config descriptor overrides
	if (ss) {
		// for SS config header
		target_desc = ss_config_desc_buf;
		config_desc = &((ss_config_desc_t *)target_desc)->config_desc;
		desc_len = sizeof(ss_config_desc);
		len += desc_len;
		memcpy(target_desc, (void *)&ss_config_desc, len);
		((ss_config_desc_t *)target_desc)->config_desc.bMaxPower = nu3000_descriptor_override.bConfigMaxPowerSS;
		((ss_config_desc_t *)target_desc)->config_desc.bmAttributes = nu3000_descriptor_override.bConfigPowerFlag;
		target_desc += desc_len;

#ifdef INCLUDE_ISO_INTERFACE
		// copy iso interface
		desc_len = create_config_iso_interface(target_desc, ss);
		target_desc += desc_len;
		len += desc_len;
		config_desc->bNumInterface++;
#endif

#ifdef DEBUG_DESCRIPTOR_DUMP
		target_desc = (u8 *)(ss_config_desc_buf);
		int j=0,k=0;
		for (j=0;j<len; j++) {
			if (k == j) {
				debug_printf("\tdesc_len=%d, desc_type=%d\n",((u8*)target_desc)[k],((u8*)target_desc)[k+1]);
				k += ((u8*)target_desc)[k];
			}
			debug_printf("desc[%d]=%x\n",j,((u8*)target_desc)[j]);
		}
#endif

		USETW(config_desc->wTotalLength, len);

	} else {
		// for HS config header
		target_desc = hs_config_desc_buf;
		config_desc = &((hs_config_desc_t *)target_desc)->config_desc;
		desc_len = sizeof(hs_config_desc);
		len += desc_len;
		memcpy(target_desc, (void *)&hs_config_desc, len);
		((hs_config_desc_t *)target_desc)->config_desc.bMaxPower = nu3000_descriptor_override.bConfigMaxPowerHS;
		((hs_config_desc_t *)target_desc)->config_desc.bmAttributes = nu3000_descriptor_override.bConfigPowerFlag;
		target_desc += desc_len;

#ifdef INCLUDE_ISO_INTERFACE
		// copy iso interface
		desc_len = create_config_iso_interface(target_desc, ss);
		target_desc += desc_len;
		len += desc_len;
		config_desc->bNumInterface++;
#endif
		USETW(config_desc->wTotalLength, len);

		// Prepare default full speed descriptor
		memcpy(fs_config_desc_buf, (void *)&fs_config_desc, sizeof(fs_config_desc));
		fs_config_desc_buf_size = sizeof(fs_config_desc);
	}
	len = UGETW(config_desc->wTotalLength);

	if (ss)
		ss_config_desc_buf_size = len;
	else
		hs_config_desc_buf_size = len;

	return len;
}

static int fill_bos_descriptor(u32 cap1Attribute)
{
	u8 *buf = bos_desc_buf;
	wusb_bos_desc_t * bos_desc_header = (wusb_bos_desc_t *)bos_desc_buf;
	int len=0,i;

	memcpy(buf + len, &bos_desc, sizeof(bos_desc));
	len += sizeof(bos_desc);

	memcpy(buf + len, &cap1_desc, sizeof(cap1_desc));
	if (cap1Attribute)
		USETDW(((usb_dev_cap_20_ext_desc_t *)(buf + len))->bmAttributes,cap1Attribute);
	len += sizeof(cap1_desc);

	memcpy(buf + len, &cap2_desc, sizeof(cap2_desc));
	((usb_dev_cap_ss_usb_t *)(buf + len))->bU1DevExitLat = nu3000_descriptor_override.bBOSU1ExitLatency;
	USETW(((usb_dev_cap_ss_usb_t *)(buf + len))->wU2DevExitLat,nu3000_descriptor_override.wBOSU2ExitLatency);
	len += sizeof(cap2_desc);

	memcpy(buf + len, &cap3_desc, sizeof(cap3_desc));
	// embed customizable unique container id into container GUID, to get the unique container GUID per device
	for (i=0; i < 6; i++)
		((usb_dev_cap_container_id_t *)(buf + len))->containerID[10+i] = nu3000_descriptor_override.containerUniqueId[i];
	len += sizeof(cap3_desc);

	// update total length of BOS
	USETW(bos_desc_header->wTotalLength, len);

	// Check if we need override full BOS descriptor
	len = UGETW(bos_desc_header->wTotalLength);
	bos_desc_buf_size = len;

	return len;
}

static void fill_device_descriptor()
{
	memcpy(&device_desc_buf, &device_desc, sizeof(device_desc));
	// Apply gen descriptors override from default or eefuse
	USETW(device_desc_buf.idVendor, nu3000_descriptor_override.wDeviceVendorId);
	USETW(device_desc_buf.idProduct, nu3000_descriptor_override.wDeviceProductId);
}

static void fill_device_qualifier_descriptor()
{
	memcpy(&dev_qualifier_buf, &dev_qualifier, sizeof(dev_qualifier));
	USETW(dev_qualifier_buf.bcdUSB, 0x210);
	dev_qualifier_buf.bMaxPacketSize0 = 64;
}
#ifdef ENABLE_SERIAL_STRING
// Implementation of itoa()
char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
#endif

static void fill_string_descriptors()
{
	int i,j,size;
	for (i=0; i < DWC_MAX_STRINGS; i++)
	{
#ifdef ENABLE_SERIAL_STRING
      if (i == DWC_STRING_SERIAL )
      {
         char buf[5] = {0};
         char seperator = '_';
         int stringLength;

         nu3000_string_descriptors[i].bDescriptorType = UDESC_STRING;
         nu3000_string_descriptors[i].bLength = 2; //header

         //copy serial number
         size = PRODUCTION_STRING_SIZE;
         for (j=0;j<size;j++)
         {
            if (productionHdrG.serialNumber[j] == 0)
               break;

            memcpy((u8 *)(nu3000_string_descriptors[i].bString[j]), (u8 *)&(productionHdrG.serialNumber[j]), 1);
         }
         memcpy((u8 *)(nu3000_string_descriptors[i].bString[j]), &seperator, 1);
         j++;
         nu3000_string_descriptors[i].bLength = j*2;
         stringLength = j;


         //copy model number
         for (j=0;j<size;j++)
         {
            if (productionHdrG.modelNumber[j] == 0)
               break;

            memcpy((u8 *)(nu3000_string_descriptors[i].bString[j+stringLength]), (u8 *)&(productionHdrG.modelNumber[j]), 1);
         }
         memcpy((u8 *)(nu3000_string_descriptors[i].bString[j+stringLength]), &seperator, 1);
         j++;
         nu3000_string_descriptors[i].bLength += j*2;
         stringLength += j;


         //copy bootId
         itoa(productionHdrG.bootId,buf);
         //copy model number
         for (j=0;j<5;j++)
         {
            if (buf[j] == 0)
               break;

            memcpy((u8 *)(nu3000_string_descriptors[i].bString[j+stringLength]), (u8 *)&(buf[j]), 1);
         }
         j++;
         nu3000_string_descriptors[i].bLength += j*2;

#ifdef DEBUG_DESCRIPTOR_DUMP
			debug_printf("dump string idx=%d:\n",i);
			debug_printf("\t length=%d, type=%d\n",nu3000_string_descriptors[i].bLength,nu3000_string_descriptors[i].bDescriptorType);
			for (j=0; j < nu3000_string_descriptors[i].bLength -2 ; j++)
				debug_printf("string[%d]=%c\n",j,((u8 *)(nu3000_string_descriptors[i].bString))[j]);
#endif
      }
      else
      {
#endif
   		size = 0;
   		// Calculate exact size of the string by looking for the 0x0000 as terminating character
   		for (j=0; j < sizeof(nu3000_strings[i]); j++, size++) {
   			if (UGETW(nu3000_strings[i][j]) == 0x0)
   				break;
   		}
   		// add 2 bytes for the string descriptor header
   		nu3000_string_descriptors[i].bLength = size*2 + 2;
   		nu3000_string_descriptors[i].bDescriptorType = UDESC_STRING;
   		memcpy((u8 *)(nu3000_string_descriptors[i].bString), (u8 *)(nu3000_strings[i]), size*2);
#ifdef DEBUG_DESCRIPTOR_DUMP
//   		debug_printf("dump string idx=%d:\n",i);
//   		debug_printf("\t length=%d, type=%d\n",nu3000_string_descriptors[i].bLength,nu3000_string_descriptors[i].bDescriptorType);
//   		for (j=0; j < nu3000_string_descriptors[i].bLength -2 ; j++)
//   			debug_printf("string[%d]=%c\n",j,((u8 *)(nu3000_string_descriptors[i].bString))[j]);
#endif
#ifdef ENABLE_SERIAL_STRING
      }
#endif
	}
}

void fill_descr_gen_customization()
{
	nu3000_descriptor_override = (nu3000_descriptor_override_t){
			DWC_VENDOR_ID,
			DWC_PRODUCT_ID,
			DWC_MAX_POWER_HS,
			DWC_MAX_POWER_SS,
			DWC_POWER_FLAGS,
			DWC_U1_EXIT_LATENCY,
			DWC_U2_EXIT_LATENCY,
			GUID_INUITIVE_CONTAINER_UNIQUE
	};
}

void fill_descr_iso_customization()
{
	nu3000_iso_rate[0] = DWC_ISO_RATE_0;
	nu3000_iso_rate[1] = DWC_ISO_RATE_1;
	nu3000_iso_rate[2] = DWC_ISO_RATE_2;
	nu3000_iso_rate[3] = DWC_ISO_RATE_3;
	nu3000_iso_rate[4] = DWC_ISO_RATE_4;
	nu3000_iso_rate[5] = DWC_ISO_RATE_5;
	nu3000_iso_rate[6] = DWC_ISO_RATE_6;
	nu3000_iso_rate[7] = DWC_ISO_RATE_7;
	nu3000_iso_rate[8] = DWC_ISO_RATE_8;
	nu3000_iso_rate[9] = DWC_ISO_RATE_9;
	nu3000_iso_rate[10] = DWC_ISO_RATE_10;
	nu3000_iso_rate[11] = DWC_ISO_RATE_11;
	nu3000_iso_rate[12] = DWC_ISO_RATE_12;
	nu3000_iso_rate[13] = DWC_ISO_RATE_13;
	nu3000_iso_rate[14] = DWC_ISO_RATE_14;
	nu3000_iso_rate[15] = DWC_ISO_RATE_15;
	nu3000_iso_rate[16] = DWC_ISO_RATE_16;
	nu3000_iso_rate[17] = DWC_ISO_RATE_17;
	nu3000_iso_rate[18] = DWC_ISO_RATE_18;
	nu3000_iso_rate[19] = DWC_ISO_RATE_19;
	nu3000_iso_rate[20] = DWC_ISO_RATE_20;
	nu3000_iso_rate[21] = DWC_ISO_RATE_21;
}

extern void GMEG_writeSaveAndRestoreReg(unsigned int id, unsigned int val);
void create_descriptors_customization(void)
{
	fill_descr_gen_customization();
	fill_descr_iso_customization();
	fill_string_descriptors();
	fill_device_descriptor();
	fill_device_qualifier_descriptor();
	fill_configuration_descriptor(0);
	fill_configuration_descriptor(1);
	//fill_bos_descriptor();

	// prepare usb descriptors export structure
	nu3000_usb_descriptors_export = (nu3000_usb_descriptors_export_t){
			(u8 *)nu3000_string_descriptors,
			DWC_MAX_STRINGS,
			sizeof(usb_string_descriptor_t),
			hs_config_desc_buf,
			hs_config_desc_buf_size,
			ss_config_desc_buf,
			ss_config_desc_buf_size,
			bos_desc_buf,
			bos_desc_buf_size,
			(u8 *)(&device_desc_buf),
			sizeof(usb_device_descriptor_t),
			(u8 *)(&dev_qualifier_buf),
			sizeof(usb_device_qualifier_t),
	};
	// store address of nu3000_usb_descriptors_export in GME_SAVE_AND_RESTORE_6
	GMEG_writeSaveAndRestoreReg(6,((u32)&nu3000_usb_descriptors_export));
}



int create_bos_descriptor(dwc_usb3_pcd_t *pcd, int max_len)
{
	u8 *buf = pcd->ep0_status_buf;
	int len;

	if (pcd->usb3_dev->core_params->besl) {
		u32 d = UGETDW(cap1_desc.bmAttributes);
		d |= (u32)(USB_20_EXT_BESL | USB_20_EXT_BASELINE_BESL_VALID | USB_20_EXT_DEEP_BESL_VALID);
		d &= ~((u32)(USB_20_EXT_BASELINE_BESL_BITS | USB_20_EXT_DEEP_BESL_BITS));
		d |= (u32)(pcd->usb3_dev->core_params->baseline_besl << USB_20_EXT_BASELINE_BESL_SHIFT);
		d |= (u32)(pcd->usb3_dev->core_params->deep_besl << USB_20_EXT_DEEP_BESL_SHIFT);
		len = fill_bos_descriptor(d);
	} else {
		len = fill_bos_descriptor(0);
	}
	len = len > max_len ? max_len : len;
	memcpy(buf, bos_desc_buf, len);
	return len;
}



