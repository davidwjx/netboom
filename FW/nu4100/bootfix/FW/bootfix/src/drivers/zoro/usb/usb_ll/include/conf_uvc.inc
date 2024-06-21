#define ADD_ALT_BOTH(intfc, ep, alt, hs_ps, hs_mult, hs_intv, ss_ps, ss_bursts, ss_mult, ss_exp) \
CONF_DESC(intf##intfc##_alt##alt, usb_interface_descriptor_t,\
	AR({\
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */\
		UDESC_INTERFACE,		/* bDescriptorType */\
		intfc,				/* bInterfaceNumber */\
		alt,				/* bAlternateSetting */\
		1,				/* bNumEndpoints */\
		14,				/* bInterfaceClass - Video */\
		2,				/* bInterfaceSubClass - Video Streaming */\
		0,				/* bInterfaceProtocol */\
		0,				/* iInterface */\
	})\
)\
CONF_DESC(intf##intfc##_alt##alt##_ep, usb_endpoint_descriptor_t,\
		AR({\
			USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */\
			UDESC_ENDPOINT,			/* bDescriptorType */\
			ep | UE_DIR_IN,			/* bEndpointAddress */\
			UE_ISOCHRONOUS | UE_ISO_ASYNC,	/* bmAttributes */\
			UCONSTW(CONF_HS(hs_ps | (hs_mult << 11), ss_ps)),/* wMaxPacketSize */\
			hs_intv,				/* bInterval */\
		})\
)\
CONF_HS( ,CONF_DESC(intf##intfc##_alt##alt##_ep_comp, ss_endpoint_companion_descriptor_t,\
		AR({\
			USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,/* bLength */\
			UDESC_SS_USB_COMPANION,		/* bDescriptorType */\
			ss_bursts,			/* bMaxBurst */\
			ss_mult,			/* bmAttributes */\
			UCONSTW(ss_exp),		/* wBytesPerInterval */\
		})\
))

#define ADD_ALT_SS(intfc, ep, alt, ss_burst, ss_mult, ss_exp) \
CONF_HS(, CONF_DESC(intf##intfc##_alt##alt, usb_interface_descriptor_t,\
	AR({\
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */\
		UDESC_INTERFACE,		/* bDescriptorType */\
		intfc,				/* bInterfaceNumber */\
		alt,				/* bAlternateSetting */\
		1,				/* bNumEndpoints */\
		14,				/* bInterfaceClass - Video */\
		2,				/* bInterfaceSubClass - Video Streaming */\
		0,				/* bInterfaceProtocol */\
		0,				/* iInterface */\
	})\
)\
CONF_DESC(intf##intfc##_alt##alt##_ep, usb_endpoint_descriptor_t,\
		AR({\
			USB_ENDPOINT_DESCRIPTOR_SIZE,	/* bLength */\
			UDESC_ENDPOINT,			/* bDescriptorType */\
			ep | UE_DIR_IN,			/* bEndpointAddress */\
			UE_ISOCHRONOUS | UE_ISO_ASYNC,	/* bmAttributes */\
			UCONSTW(1024),			/* wMaxPacketSize */\
			1,				/* bInterval */\
		})\
)\
CONF_DESC(intf##intfc##_alt##alt##_ep_comp, ss_endpoint_companion_descriptor_t,\
		AR({\
			USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,/* bLength */\
			UDESC_SS_USB_COMPANION,		/* bDescriptorType */\
			ss_burst,			/* bMaxBurst */\
			ss_mult,			/* bmAttributes */\
			UCONSTW(ss_exp),		/* wBytesPerInterval */\
		})\
))

#if 1
CONF_DESC(intf_assoc78, usb_interface_assoc_descriptor_t,
AR({
	8,				/* bLength */
	DESC_INTERFACE_ASSOCIATION,	/* bDescriptorType */
	2,				/* bFirstInterface */
	2,				/* bInterfaceCount */
	14,				/* bFunctionClass - Video */
	3,				/* bFunctionSubClass - Video Interface Collection */
	0,				/* bFunctionProtocol */
	DWC_STRING_WEBCAM,		/* iFunction */
}))
CONF_DESC(intf7, usb_interface_descriptor_t,
	AR({
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		2,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		0,				/* bNumEndpoints */
		14,				/* bInterfaceClass - Video */
		1,				/* bInterfaceSubClass - Video Control */
		0,				/* bInterfaceProtocol */
		DWC_STRING_WEBCAM,		/* iInterface */
	})
)
CONF_DESC(intf7_uvch, uvc_interface_descriptor_header_t,
		AR({
			13,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			1,			/* bDescriptorSubtype - HEADER */
			UCONSTW(0x110),		/* bcdUVC */
			UCONSTW(80),		/* wTotalLength */
			UCONSTDW(30000000),	/* dwClockFrequency */
			1,			/* bInCollection */
			{3},			/* baInterfaceNr[0] */
		})
)
CONF_DESC(intf7_uvcit, uvc_interface_descriptor_input_camera_t,
		AR({
			18,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			2,			/* bDescriptorSubtype - INPUT_TERMINAL */
			1,			/* bTerminalID */
			UCONSTW(0x0201),	/* wTerminalType - Camera Sensor*/
			4,			/* bAssocTerminal */
			0,			/* iTerminal */
			UCONSTW(0),		/* wObjectiveFocalLengthMin */
			UCONSTW(0),		/* wObjectiveFocalLengthMax */
			UCONSTW(0),		/* wOcularFocalLength */
			3,			/* bControlSize */
			{0x00,0x00,0x00},	/* bmControls[0,1,2] */
		})
)
CONF_DESC(intf7_uvcproc, uvc_interface_descriptor_processing_t,
		AR({
			13,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			5,			/* bDescriptorSubtype - PROCESSING_UNIT */
			2,			/* bUnitID */
			1,			/* bSourceID */
			UCONSTW(0),		/* bMaxMultiplier */
			3,			/* bControlSize */
			{0x00,0x00,0x00},	/* bmControls[0,1,2] */
			0,			/* iProcessing */
			0,			/* bmVideoStandards */
		})
)
CONF_DESC(intf7_uvcext, uvc_interface_descriptor_ext_t,
		AR({
			27,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			6,			/* bDescriptorSubtype - EXTENSION_UNIT */
			3,			/* bUnitID */
			GUID_INUITIVE_WEBCAM,	/* guidExtensionCode */
			8,			/* bNumControls */
			1,			/* bNrInPins */
			{2},			/* baSourceID[0] */
			2,			/* bControlSize */
			{0xff,0xff},		/* bmControls[0,1] */
			0,			/* iExtension */
		})
)
CONF_DESC(intf7_uvco, uvc_interface_descriptor_output_stream_t,
		AR({
			9,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			3,			/* bDescriptorSubtype - OUTPUT_TERMINAL */
			4,			/* bTerminalID */
			UCONSTW(0x0101),	/* wTerminalType - USB Streaming */
			0,			/* bAssocTerminal */
			3,			/* bSourceID */
			0,			/* iTerminal */
		})
)
CONF_DESC(intf8, usb_interface_descriptor_t,
	AR({
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		3,				/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		0,				/* bNumEndpoints */
		14,				/* bInterfaceClass - Video */
		2,				/* bInterfaceSubClass - Video Streaming */
		0,				/* bInterfaceProtocol */
		0,				/* iInterface */
	})
)
CONF_DESC(intf8_uvsh, uvs_interface_descriptor_input_header_t,
		AR({
			14,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			1,			/* bDescriptorSubtype - INPUT_HEADER */
			1,			/* bNumFormats */
			UCONSTW(47 + 46*10),	/* wTotalLength */
			3 | UE_DIR_IN,		/* bEndpointAddress XXX 5*/
			0,			/* bmInfo */
			4,			/* bTerminalLink */
			1,			/* bStillCaptureMethod */
			0,			/* bTriggerSupport */
			0,			/* bTriggerUsage */
			1,			/* bControlSize */
			{0x00},			/* bmaControls[0] */
		})
)
CONF_DESC(intf8_uvsfmt1, uvs_interface_descriptor_format_uncompressed_t,
		AR({
			27,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			4,			/* bDescriptorSubtype - FORMAT_UNCOMPRESSED */
			1,			/* bFormatIndex */
			10,			/* bNumFrameDescriptors */
			{0x59,0x55,0x59,0x32, 0x00,0x00, 0x10,0x00, 0x80,0x00, 0x00,0xaa,0x00,0x38,0x9b,0x71},/* guidFormat - YUY2 */
			16,			/* bBitsPerPixel */
			1,			/* bDefaultFrameIndex */
			0,			/* bAspectRatioX */
			0,			/* bAspectRatioY */
			0x00,			/* bmInterlaceFlags */
			0,			/* bCopyProtect */
		})
)
#define ADD_FRAME(frm, w, h) \
CONF_DESC(intf8_uvsfmt1_frm##frm, uvs_interface_descriptor_frame_uncompressed_t,\
			AR({\
				46,			/* bLength */\
				UDESC_CS_INTERFACE,	/* bDescriptorType */\
				5,			/* bDescriptorSubtype - FRAME_UNCOMPRESSED */\
				frm,			/* bFrameIndex */\
				0x1,			/* bmCapabilities */\
				UCONSTW(w),		/* wWidth */\
				UCONSTW(h),		/* wHeight */\
				UCONSTDW(w*h*16*24),	/* dwMinBitRate */\
				UCONSTDW(w*h*16*60),	/* dwMaxBitRate */\
				UCONSTDW(w*h*2),	/* dwMaxVideoFrameBufferSize */\
				UCONSTDW(10000000 / 30),/* dwDefaultFrameInterval */\
				5,			/* bFrameIntervalType */\
				{			/* dwFrameInterval[] */\
				 UCONSTDW(10000000 / 60),\
				 UCONSTDW(10000000 / 50),\
				 UCONSTDW(10000000 / 30),\
				 UCONSTDW(10000000 / 25),\
				 UCONSTDW(10000000 / 24),\
				},\
			})\
)
#define ADD_FRAME_HS(frm, w, h, rate1, rate2, rate3, rate4, rate5, rate_5) \
CONF_DESC(intf8_uvsfmt1_frm##frm, uvs_interface_descriptor_frame_uncompressed_t,\
			AR({\
				46,			/* bLength */\
				UDESC_CS_INTERFACE,	/* bDescriptorType */\
				5,			/* bDescriptorSubtype - FRAME_UNCOMPRESSED */\
				frm,			/* bFrameIndex */\
				0x1,			/* bmCapabilities */\
				UCONSTW(w),		/* wWidth */\
				UCONSTW(h),		/* wHeight */\
				UCONSTDW(w*h*16*rate1),	/* dwMinBitRate */\
				UCONSTDW(w*h*16*rate5),	/* dwMaxBitRate */\
				UCONSTDW(w*h*2),	/* dwMaxVideoFrameBufferSize */\
				UCONSTDW(10000000 * rate_5),/* dwDefaultFrameInterval */\
				5,			/* bFrameIntervalType */\
				{			/* dwFrameInterval[] */\
				 UCONSTDW(10000000 * rate_5),\
				 UCONSTDW(10000000 / rate4),\
				 UCONSTDW(10000000 / rate3),\
				 UCONSTDW(10000000 / rate2),\
				 UCONSTDW(10000000 / rate1),\
				},\
			})\
)
CONF_HS(ADD_FRAME_HS(1, 640, 480, 12, 15, 24, 25, 30, 1/30), ADD_FRAME(1, 640, 480))
ADD_FRAME(2, 176, 144)
ADD_FRAME(3, 160, 120)
ADD_FRAME(4, 352, 288)
ADD_FRAME(5, 320, 240)
CONF_HS(ADD_FRAME_HS(6, 800, 600, 6, 12, 15, 24, 25, 1/25), ADD_FRAME(6, 800, 600))
CONF_HS(ADD_FRAME_HS(7, 1024, 768, 2, 3, 5, 6, 12, 1/12), ADD_FRAME(7, 1024, 768))
CONF_HS(ADD_FRAME_HS(8, 1280, 720, 2, 3, 5, 6, 12, 1/12), ADD_FRAME(8, 1280, 720))
CONF_HS(ADD_FRAME_HS(9, 1280, 800, 2, 3, 5, 6, 15/2, 2/15), ADD_FRAME(9, 1280, 800))
CONF_HS(ADD_FRAME_HS(10, 1920, 1080, 1, 2, 3, 4, 5, 1/5), ADD_FRAME(10, 1920, 1080))
CONF_DESC(intf8_uvsfmt1_color, uvs_interface_descriptor_color_matching_t,
		AR({
			6,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			13,			/* bDescriptorSubtype - COLOR_FORMAT */
			0,			/* bColorPrimaries */
			0,			/* bTransferCharacteristics */
			0,			/* bMatrixCoefficients */
		})
)
ADD_ALT_BOTH(3, 3, 1, 210, 0, 1, 210, 0, 0, 210)
ADD_ALT_BOTH(3, 3, 2, 400, 0, 1, 400, 0, 0, 400)
ADD_ALT_BOTH(3, 3, 3, 780, 0, 1, 780, 0, 0, 780)
ADD_ALT_BOTH(3, 3, 4, 780, 1, 1, 1024, 1, 0, 1540)
ADD_ALT_BOTH(3, 3, 5, 968, 1, 1, 1024, 3, 0, 3650)
ADD_ALT_BOTH(3, 3, 6, 804, 2, 1, 1024, 5, 0, 5950)
ADD_ALT_BOTH(3, 3, 7, 926, 2, 1, 1024, 6, 0, 6950)
ADD_ALT_BOTH(3, 3, 8, 1024, 2, 1, 1024, 7, 0, 7700)
ADD_ALT_SS(3, 3, 9, 14, 0, 14450)
ADD_ALT_SS(3, 3, 10, 15, 0, 15600)
ADD_ALT_SS(3, 3, 11, 15, 1, 31200)
#endif
#if 1
CONF_DESC(intf_assoc910, usb_interface_assoc_descriptor_t,
AR({
	8,				/* bLength */
	DESC_INTERFACE_ASSOCIATION,	/* bDescriptorType */
	4,	/* bFirstInterface */
	2,				/* bInterfaceCount */
	1,				/* bFunctionClass - Audio */
	0,				/* bFunctionSubClass */
	0,				/* bFunctionProtocol */
	DWC_STRING_MICROPHONE,		/* iFunction */
}))
CONF_DESC(intf9, usb_interface_descriptor_t,
	AR({
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		4,		/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		0,				/* bNumEndpoints */
		1,				/* bInterfaceClass - Audio */
		1,				/* bInterfaceSubClass - Control Device */
		0,				/* bInterfaceProtocol */
		DWC_STRING_MICROPHONE,		/* iInterface */
	})
)
CONF_DESC(intf9_uach, uac_interface_descriptor_header_t,
		AR({
			9,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			1,			/* bDescriptorSubtype - HEADER */
			UCONSTW(0x100),		/* bcdADC */
			UCONSTW(58),		/* wTotalLength */
			1,			/* bInCollection */
			{5},			/* baInterfaceNr[0] */
		})
)
CONF_DESC(intf9_uacit, uac_interface_descriptor_input_t,
		AR({
			12,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			2,			/* bDescriptorSubtype - INPUT_TERMINAL */
			1,			/* bTerminalID */
			UCONSTW(0x0201),	/* wTerminalType - Microphone */
			0,			/* bAssocTerminal */
			2,			/* bNrChannels */
			UCONSTW(0),		/* wChannelConfig */
			0,			/* iChannelNames */
			0,			/* iTerminal */
		})
)
CONF_DESC(intf9_uacfeat, uac_interface_descriptor_feature_t,
		AR({
			13,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			6,			/* bDescriptorSubtype */
			2,			/* bUnitID */
			1,			/* bSourceID */
			2,			/* bControlSize */
			{UCONSTW(0x0001),UCONSTW(0),UCONSTW(0)},/* bmaControls[0,1,2] */
			0,			/* iFeature */
		})
)
CONF_DESC(intf9_uacext, uac_interface_descriptor_ext_t,
		AR({
			15,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			8,			/* bDescriptorSubtype - EXTENSION_UNIT */
			3,			/* bUnitID */
			UCONSTW(0),		/* wExtensionCode */
			1,			/* bNrInPins */
			{2},			/* baSourceID[0] */
			2,			/* bNrChannels */
			UCONSTW(0),		/* wChannelConfig */
			0,			/* iChannelNames */
			1,			/* bControlSize */
			{0xff},			/* bmControls */
			0,			/* iExtension */
		})
)
CONF_DESC(intf9_uaco, uac_interface_descriptor_output_t,
		AR({
			9,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			3,			/* bDescriptorSubtype - OUTPUT_TERMINAL */
			4,			/* bTerminalID */
			UCONSTW(0x0101),	/* wTerminalType - USB Streaming */
			0,			/* bAssocTerminal */
			3,			/* bSourceID */
			0,			/* iTerminal */
		})
)
CONF_DESC(intf10, usb_interface_descriptor_t,
	AR({
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		5,	/* bInterfaceNumber */
		0,				/* bAlternateSetting */
		0,				/* bNumEndpoints */
		1,				/* bInterfaceClass - Audio */
		2,				/* bInterfaceSubClass - Streaming */
		0,				/* bInterfaceProtocol */
		0,				/* iInterface */
	})
)
CONF_DESC(intf10_alt1, usb_interface_descriptor_t,
	AR({
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		5,	/* bInterfaceNumber */
		1,				/* bAlternateSetting */
		1,				/* bNumEndpoints */
		1,				/* bInterfaceClass - Audio */
		2,				/* bInterfaceSubClass - Streaming */
		0,				/* bInterfaceProtocol */
		0,				/* iInterface */
	})
)
CONF_DESC(intf10_alt1_uasg, uas_interface_descriptor_general_t,
		AR({
			7,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			1,			/* bDescriptorSubtype - AS_GENERAL */
			4,			/* bTerminalLink */
			0,			/* bDelay */
			UCONSTW(1),		/* wFormatTag - PCM */
		})
)
CONF_DESC(intf10_alt1_uasfmt1, uas_interface_descriptor_format_type1_t,
		AR({
			14,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			2,			/* bDescriptorSubtype - FORMAT_TYPE */
			1,			/* bFormatType - FORMAT_TYPE_1 */
			2,			/* bNrChannels */
			2,			/* bSubFrameSize */
			16,			/* bBitResolution */
			2,			/* bSamFreqType */
			{0x80, 0x3e, 0x00},	/* tSamFreq - 16000 */
			{0x80, 0xbb, 0x00},	/* tSamFreq - 48000 */
		})
)
CONF_DESC(intf10_alt1_ep, usb_endpoint_descriptor_uas_t,
		AR({
			USB_ENDPOINT_DESCRIPTOR_SIZE + 2,/* bLength */
			UDESC_ENDPOINT,			/* bDescriptorType */
			4 | UE_DIR_IN,			/* bEndpointAddress */
			UE_ISOCHRONOUS | UE_ISO_ASYNC,	/* bmAttributes */
			UCONSTW(192),			/* wMaxPacketSize - 2chan * 2bps * 6spf * 8frm */
			4,				/* bInterval */
			0,				/* bRefresh */
			0,				/* bSynchAddress */
		})
)
CONF_HS( ,CONF_DESC(intf10_alt1_ep_comp, ss_endpoint_companion_descriptor_t,
		AR({
			USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,/* bLength */
			UDESC_SS_USB_COMPANION,		/* bDescriptorType */
			0,				/* bMaxBurst */
			0,				/* bmAttributes */
			UCONSTW(192),			/* wBytesPerInterval */
		})
))
CONF_DESC(intf10_alt1_ep_uas, uas_endpoint_descriptor_general_t,
			AR({
				7,			/* bLength */
				UDESC_CS_ENDPOINT,	/* bDescriptorType */
				1,			/* bDescriptorSubtype - EP_GENERAL */
				0,			/* bmAttributes */
				0,			/* bLockDelayUnits */
				UCONSTW(0),		/* wLockDelay */
			})
)
CONF_DESC(intf10_alt2, usb_interface_descriptor_t,
	AR({
		USB_INTERFACE_DESCRIPTOR_SIZE,	/* bLength */
		UDESC_INTERFACE,		/* bDescriptorType */
		5, /* bInterfaceNumber */
		2,				/* bAlternateSetting */
		1,				/* bNumEndpoints */
		1,				/* bInterfaceClass - Audio */
		2,				/* bInterfaceSubClass - Streaming */
		0,				/* bInterfaceProtocol */
		0,				/* iInterface */
	})
)
CONF_DESC(intf10_alt2_uasg, uas_interface_descriptor_general_t,
		AR({
			7,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			1,			/* bDescriptorSubtype - AS_GENERAL */
			4,			/* bTerminalLink */
			0,			/* bDelay */
			UCONSTW(1),		/* wFormatTag - PCM */
		})
)
CONF_DESC(intf10_alt2_uasfmt1, uas_interface_descriptor_format_type1_t,
		AR({
			14,			/* bLength */
			UDESC_CS_INTERFACE,	/* bDescriptorType */
			2,			/* bDescriptorSubtype - FORMAT_TYPE */
			1,			/* bFormatType - FORMAT_TYPE_1 */
			2,			/* bNrChannels */
			3,			/* bSubFrameSize */
			24,			/* bBitResolution */
			2,			/* bSamFreqType */
			{0x80, 0x3e, 0x00},	/* tSamFreq - 16000 */
			{0x80, 0xbb, 0x00},	/* tSamFreq - 48000 */
		})
)
CONF_DESC(intf10_alt2_ep, usb_endpoint_descriptor_uas_t,
		AR({
			USB_ENDPOINT_DESCRIPTOR_SIZE + 2,/* bLength */
			UDESC_ENDPOINT,			/* bDescriptorType */
			4 | UE_DIR_IN,			/* bEndpointAddress*/
			UE_ISOCHRONOUS | UE_ISO_ASYNC,	/* bmAttributes */
			UCONSTW(288),			/* wMaxPacketSize - 2chan * 3bps * 6spf * 8frm */
			4,				/* bInterval */
			0,				/* bRefresh */
			0,				/* bSynchAddress */
		})
)
CONF_HS( ,CONF_DESC(intf10_alt2_ep_comp, ss_endpoint_companion_descriptor_t,
		AR({
			USB_SS_ENDPOINT_COMPANION_DESCRIPTOR_SIZE,/* bLength */
			UDESC_SS_USB_COMPANION,		/* bDescriptorType */
			0,				/* bMaxBurst */
			0,				/* bmAttributes */
			UCONSTW(288),			/* wBytesPerInterval */
		})
))
CONF_DESC(intf10_alt2_ep_uas, uas_endpoint_descriptor_general_t,
			AR({
				7,			/* bLength */
				UDESC_CS_ENDPOINT,	/* bDescriptorType */
				1,			/* bDescriptorSubtype - EP_GENERAL */
				0,			/* bmAttributes */
				0,			/* bLockDelayUnits */
				UCONSTW(0),		/* wLockDelay */
			})
)
#endif