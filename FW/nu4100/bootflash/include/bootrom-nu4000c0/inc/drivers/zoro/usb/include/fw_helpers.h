/**
	@file   fw_helpers.h

	@brief  structs of pointers to ROM elements usefull for firmware

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

#ifndef _FW_HELPERS_H_
#define _FW_HELPERS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "os_defs.h"
#include "usb.h"
#include "zr_common.h"
#include "sys_calls.h"



typedef struct nu3000_usb_descriptors_export_st{
	unsigned char *nu3000_string_descriptors;
	int  string_desc_number;
	int  string_desc_size;
	unsigned char *hs_config_desc_buf;
	int  hs_config_desc_buf_size;
	unsigned char *ss_config_desc_buf;
	int  ss_config_desc_buf_size;
	unsigned char *bos_desc_buf;
	int  bos_desc_buf_size;
	unsigned char *device_desc_buf;
	int  device_desc_buf_size;
	unsigned char *dev_qualifier_buf;
	int  dev_qualifier_buf_size;
} UPACKED nu3000_usb_descriptors_export_t;


#ifdef __cplusplus
}
#endif
#endif /* _FW_HELPERS_H_ */
