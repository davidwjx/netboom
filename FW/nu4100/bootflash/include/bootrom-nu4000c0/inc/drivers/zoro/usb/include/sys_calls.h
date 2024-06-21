/**
	@file   sys_calls.h

	@brief  API exported by ROM for USB driver usage

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

#ifndef _SYS_CALLS_H_
#define _SYS_CALLS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "zr_common.h"
#include "system.h"

/* =============================== USB HW state ============================= */

/**
  return 1 if the USB2.0 PHY type is UTMI, 0 otherwise (ULPI).
 */
int usb_hw_phy2_is_utmi(void);

#ifdef __cplusplus
}
#endif
#endif /* _SYS_CALLS_H_ */
