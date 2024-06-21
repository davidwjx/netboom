/**
	@brief  configuration 2nd part

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


#ifdef __cplusplus
extern "C" {
#endif

#if 0

#include "os_defs.h"
#include "usb.h"
#include "zr_common.h"
#include "conf.h"

/**
  define the structs of different special descriptors
 */
#define AR(x...) x
#define DEF_DESC(name) typedef struct {
#define DEF_FIELD(stname, type, name, mult) type name mult;
#define DEF_END(name) } UPACKED name;

#include "array_conf.inc"

#undef DEF_DESC
#undef DEF_FIELD
#undef DEF_END


#define CONF_HS(hs, ss) hs
static const struct {
#define CONF_DESC(name, type, contents) type name;
#include "conf_uvc.inc"
#undef CONF_DESC
} UPACKED hs_config_desc_rest_st_uvc = {
#define CONF_DESC(name, type, contents) contents,
#include "conf_uvc.inc"
#undef CONF_DESC
};
#undef CONF_HS

const void * const hs_config_desc_rest_uvc = &hs_config_desc_rest_st_uvc;
const int hs_config_desc_rest_len_uvc = sizeof(hs_config_desc_rest_st_uvc);

/**
  define later part of SS config tree: struct of structs, then contents.
 */
#define CONF_HS(hs, ss) ss
static const struct {
#define CONF_DESC(name, type, contents) type name;
#include "conf_uvc.inc"
#undef CONF_DESC
} UPACKED ss_config_desc_rest_st_uvc = {
#define CONF_DESC(name, type, contents) contents,
#include "conf_uvc.inc"
#undef CONF_DESC
};
#undef CONF_HS

const void * const ss_config_desc_rest_uvc = &ss_config_desc_rest_st_uvc;
const int ss_config_desc_rest_len_uvc = sizeof(ss_config_desc_rest_st_uvc);

#endif

#ifdef __cplusplus
}
#endif
