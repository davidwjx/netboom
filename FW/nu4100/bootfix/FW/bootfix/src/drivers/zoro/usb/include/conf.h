/**
	@file   conf.h

	@brief  defines for default descriptor customizations

	@date October, 2014

	@author Konstatnin Sinyuk, Inuitive Ltd
*/

#ifndef _CONF_H_
#define _CONF_H_
#ifdef __cplusplus
extern "C" {
#endif

//#include "os_defs.h"
#include "usb.h"
//#include "zr_common.h"

//#define DEBUG_DESCRIPTOR_DUMP
#define DWC_VENDOR_ID 		0x2959
#define DWC_PRODUCT_ID 		0x4100
#define DWC_MAX_POWER_HS 	(500 / 2) 	// in 2 mA units
#define DWC_MAX_POWER_SS 	(900 / 8)	// in 8 mA units
#define DWC_POWER_FLAGS_ADD	0
#define DWC_POWER_FLAGS 	(UC_ATT_ONE | DWC_POWER_FLAGS_ADD)
#define DWC_U1_EXIT_LATENCY 10
#define DWC_U2_EXIT_LATENCY 256


#define DWC_ISO_MAX_ALT_SETTINGS 32
#define	DWC_ISO_RATE_0		96
#define	DWC_ISO_RATE_1		384
#define	DWC_ISO_RATE_2		576
#define	DWC_ISO_RATE_3		768
#define	DWC_ISO_RATE_4		1024
#define	DWC_ISO_RATE_5		2048
#define	DWC_ISO_RATE_6		3072
#define	DWC_ISO_RATE_7		3456
#define	DWC_ISO_RATE_8		4608
#define	DWC_ISO_RATE_9		6144
#define	DWC_ISO_RATE_10		6912
#define	DWC_ISO_RATE_11		9216
#define	DWC_ISO_RATE_12		12288
#define	DWC_ISO_RATE_13		13824
#define	DWC_ISO_RATE_14		15552
#define	DWC_ISO_RATE_15		18432
#define	DWC_ISO_RATE_16		24576
#define	DWC_ISO_RATE_17		27648
#define	DWC_ISO_RATE_18		31104
#define	DWC_ISO_RATE_19		36864
#define	DWC_ISO_RATE_20		41472
#define	DWC_ISO_RATE_21		46656

#define DWC_ISO_INTERFACE_NUMBER 1
#define DWC_ISO_ENDPOINT_NUMBER 2

#define DWC_UVC_INTERFACE_NUMBER DWC_ISO_INTERFACE_NUMBER + 1  // 3, see conf_uvc.inc
#define DWC_UVC_ENDPOINT_NUMBER DWC_ISO_ENDPOINT_NUMBER + 1   //  4, see conf_uvc.inc

#define DWC_STRING_LANGUAGE 	0
#define DWC_STRING_MANUFACTURER 1
#define DWC_STRING_PRODUCT		2
#define DWC_STRING_SERIAL		3
#define DWC_STRING_PROP_CTRL 	4
//#define DWC_STRING_PROP_STREAM 	5
//#define DWC_STRING_WEBCAM 		6
//#define DWC_STRING_MICROPHONE   7
#define DWC_MAX_STRINGS			DWC_STRING_PROP_CTRL+1
#define DWC_MAX_SUPPORTED_LANGUAGES		1

#define MAX_USB_DESCRIPTOR_SIZE 48
#define MAX_USB_CONF_TREE_SIZE 2048
#define MAX_USB_BOS_TREE_SIZE 512


//#define GUID_INUITIVE_CONTAINER {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,1} /* default nonzero */
#define GUID_INUITIVE_CONTAINER_BASE {0x09,0x85,0x5d,0x83 ,0x99,0x66, 0x0e,0x4f, 0x3e,0x87, 0x88,0xcb,0xeb,0xbc,0xfa,0x4e}/*09855d83-6699-4f0e-873e-88cbebbcfa4e*/
#define GUID_INUITIVE_CONTAINER_UNIQUE {0x88,0xcb,0xeb,0xbc,0xfa,0x4e}/*88cbebbcfa4e*/
#define GUID_INUITIVE_EXT {0x65,0x30,0x8f,0xdb, 0x4d,0x72, 0x83,0x4e, 0xa9,0x16, 0xe3,0x58,0x92,0xdf,0x3f,0xa6} /* db8f3065-724d-4e83-a916-e35892df3fa6 */
#define GUID_INUITIVE_FMT {0x8d,0x89,0x28,0x4f, 0x43,0x1b, 0xae,0x4c, 0x81,0x29, 0x99,0xe7,0x93,0x99,0x6a,0x14} /* 4f28898d-1b43-4cae-8129-99e793996a14 */
#define GUID_INUITIVE_WEBCAM {0x48,0x42,0x78,0xe7, 0x2e,0x66, 0x61,0x40, 0x80,0x01, 0xf4,0xde,0x21,0x58,0x8b,0xb1} /* e7784248-662e-4061-8001-f4de21588bb1 */


#ifdef __cplusplus
}
#endif
#endif /* _ARRAY_CONF_H_ */
