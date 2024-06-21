/**
	@file   usb_drv.h

	@brief  API for USB driver

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

#ifndef _USB_DRV_H_
#define _USB_DRV_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "os_defs.h"
#include "zr_common.h"

#ifdef USB_FOR_VDK
#define CONTROLLER_BASE_ADDR 0x40004000
#define CONTROLLER_INTERRUPT 36
#else
#define CONTROLLER_BASE_ADDR 0x09000000
#define CONTROLLER_INTERRUPT 38
#endif

struct dwc_usb3_device;
struct usb_ep;

/**
  init the function driver, with a given controller base address and irq number.
 */
struct dwc_usb3_device *dwc_usb3_driver_init(u32 base_addr_dwc, u8 irq_num);


/**
  get a pointer to the EP array of the ROM function driver, for either the IN
  or the OUT direction.
 */
struct usb_ep **nu3000_get_ep_array(int in, int *len);

/**
  send a message to the host via the proprietary bulk monitor interface.
  the message buffer must be prefixed by the following struct:
  - completion callback with a context.
  - message buffer length.
 */
typedef struct {
	void (*complete)(void *ctx);
	int len;
	char buf[0];
} NU3000_USB_SEND_TO_HOST_ST;
int nu3000_usb_send_to_host(NU3000_USB_SEND_TO_HOST_ST *ctx);

/**
  Wakeup callback for USB driver, can be trigerred by BootROM resume
 */
extern void dwc_usb3_common_wakeup(void);

/**
  ROM should call this on every iteration of the idle main loop
 */
extern void nu3000_bg_process(void);

/**
  Register irq handler. Called on USB resume.
 */
extern void nu3000_usb_irq_register(int irq_num);

/**
  return current device state (pcdstate_e).
  assuming only single configuration exists, so we know what it is...
 */
extern int nu3000_get_cur_device_state(struct dwc_usb3_device *dev);

#ifdef __cplusplus
}
#endif
#endif /* _USB_DRV_H_ */
