/**
	@file   zr_patch.h

	@brief  Patch points for NU3000

	@date September, 2010

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

#ifndef _ZR_PATCH_H_
#define _ZR_PATCH_H_

#ifdef __cplusplus
extern "C"
{
#endif

struct dwc_usb3_device;
struct dwc_usb3_pcd_ep;
struct dwc_usb3_pcd;
struct usb_device_request;

typedef union {
	struct {
		/**
		  SLOT 0: handle messages on the proprietary bulk monitor.
		  @param [INOUT] buf - input buffer, output to fill in.
		  @param [INOUT] len - length of input buffer, set to length of output buffer.
		  @param [OUT] err - set to error code, if any.
		  @returns: 0 to continue processing the input, 1 to mark it handled.
		 */
		int (*monitor_extension)(unsigned *buf, int *len, int *err);

		/**
		  SLOT 1: handle proprietary control messages.
		  called twice - before main function body, and after it just before sending.
		  @param [IN] after - 0 before main function body, 1 after it (before sending the data).
		  @param [IN] pcd.
		  @param [IN] ctrl - request struct.
		  @param [OUT] err - error to return, only in case we stop the default processing.
		  @returns: 0 to continue the default handling, 1 to stop it (in which case err should be filled).
		 */
		int (*ep0_extension)(int after, struct dwc_usb3_pcd *pcd, struct usb_device_request *ctrl, int *err);

		/**
		  SLOT 2: alternative device event handler.
		  @param [IN] pcd.
		  @param [IN] event - event code.
		  @param [OUT] err - error code, if returning 1.
		  @returns: 0 to continue the default handling, 1 to stop it (in which case you err should be filled too).
		 */
		int (*dev_event_extension)(struct dwc_usb3_pcd *pcd, uint32 event, int *err);

		/**
		  SLOT 3: alternative endpoint event handler.
		  @param [IN] pcd.
		  @param [IN] ep - endpoint struct.
		  @param [IN] event - event code.
		  @returns: 0 to continue the default handling, 1 to stop it.
		 */
		int (*ep_event_extension)(struct dwc_usb3_pcd *pcd, struct dwc_usb3_pcd_ep *ep, uint32 event);

		/**
		  SLOT 4: manipulate conf_buf just before using it.
		  @param [IN] ss - 1 for super-speed, 0 for high-speed.
		  @param [INOUT] len - length of buffer.
		 */
		void (*desc_cpy)(int ss, int *len);

		/**
		  SLOT 5: alternative get_descriptor handlers.
		  called twice - before entering the main function, and just before sending the results.
		  @param [IN] after - 0 before main function body, 1 after it (before sending the data).
		  @param [IN] pcd.
		  @param [IN] ctrl - request struct.
		  @param [OUT] err - error to return, only in case we stop the default processing.
		  @returns: 0 to continue the default handling, 1 to stop it (in which case err should be filled).
		 */
		int (*get_descriptor)(int after, struct dwc_usb3_pcd *pcd, struct usb_device_request *ctrl, int *err);

		/**
		  SLOT 6: reset of all EPs of interfaces other than 0 (called when deconfiguring).
		  @param [IN] usb3_dev.
		  @param [IN] loopbk - private device struct.
		  @returns: 0 to continue handling, 1 to stop.
		 */
		int (*reset_other_eps)(struct dwc_usb3_device *usb3_dev, void *loopbk);

		/**
		  SLOT 7: set all EPs of interfaces other than 0 (called when configuring).
		  @param [IN] usb3_dev.
		  @param [IN] loopbk - private device struct.
		  @param [OUT] ret - error to return, in case we stop default processing.
		  @returns: 0 to continue handling, 1 to stop (in which case ret should be filled).
		 */
		int (*set_other_eps)(struct dwc_usb3_device *usb3_dev, void *loopbk, int *ret);

		/**
		  SLOT 8: set interface to any altsetting different from its current one.
		  @param [IN] usb3_dev.
		  @param [IN] loopbk - private device struct.
		  @param [IN] intf - interface number.
		  @param [IN] alt - requested altsetting.
		  @param [OUT] ret - error to return, in case we stop default processing.
		  @returns: 0 to continue handling, 1 to stop (in which case ret should be filled).
		 */
		int (*set_single_intf)(struct dwc_usb3_device *usb3_dev, void *loopbk, uint8 intf, uint8 alt, int *ret);

		/**
		  SLOT 9: flags.
		 */
		uint32 flags;
#define USB_PP_FLAG_SKIP_BOS (1 << 0) /** < skip the special BOS descriptor fetching, falling to the default GET_DESCRIPTOR routine */

		/**
		  SLOT 10: called on suspension process, just before powering off.
		  @returns: nonzero to prevent power-off.		  
		 */
		int (*suspension_cb)(void);

		/**
		  SLOT 11: callback to register for device state changes, that may affect power mode:
		  @param [IN] state: of type pcdstate_e:
			- DWC_STATE_DEFAULT after reset
			- DWC_STATE_ADDRESSED after deconfiguring
			- DWC_STATE_CONFIGURED after configuring
		  assuming only single configuration exists, so no extra argument is needed for that.
		 */
		void (*system_full_power_cb)(int state);
	};

	void *slots[16];
} USB_PP_ST;

extern USB_PP_ST usb_pp __attribute__((section(".patch")));

#ifdef __cplusplus
}
#endif
#endif /* _ZR_PATCH_H_ */
