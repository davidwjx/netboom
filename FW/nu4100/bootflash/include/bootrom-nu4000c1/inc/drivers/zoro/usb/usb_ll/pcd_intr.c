/* ==========================================================================
 * $ File: //dwh/usb_iip/dev/software/DWC_usb3/driver/pcd_intr.c $
 * $ Revision: #105 $
 * $ Date: 2012/11/13 $
 * $ Change: 2104430 $
 *
 * Synopsys SS USB3 Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * ========================================================================== */
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
 * This file contains the implementation of the PCD Interrupt handlers.
 *
 * The PCD handles the device interrupts. Many conditions can cause a
 * device interrupt. When an interrupt occurs, the device interrupt
 * service routine determines the cause of the interrupt and
 * dispatches handling to the appropriate routine. These interrupt
 * handling routines are described below.
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"

#ifdef DWC_UTE
# include "ute_if.h"
#endif

#ifdef USB_FOR_NU3000
#include "usb_drv.h"
#endif

/**
 * This interrupt indicates that the USB link state has changed to L2, U3, or
 * (if L1 Hibernation is enabled) L1, and software intervention is required.
 */
static int handle_hiber_req_intr(dwc_usb3_pcd_t *pcd, u32 event)
{
	int hird, is_superspeed;
	u32 state;
	UNUSED(hird);

	//dwc_print0(pcd->usb3_dev, "HIBERNATION REQUEST\n");

	is_superspeed = !!(event & DWC_DEVT_HIBER_SS_BIT);
	state = event >> DWC_DEVT_HIBER_STATE_SHIFT &
		DWC_DEVT_HIBER_STATE_BITS >> DWC_DEVT_HIBER_STATE_SHIFT;
	dwc_print2(pcd->usb3_dev, "state=%x, is_superspeed=%d\n",
		   state, is_superspeed);

	/* TODO: Workaround */
	if (!(state == DWC_LINK_STATE_U3 ||
	      //state == DWC_LINK_STATE_SS_DIS ||
	      (!is_superspeed && (state == DWC_LINK_STATE_SLEEP)))) {
		dwc_print0(pcd->usb3_dev, "HIBERNATION not handled\n");
		return 1;
	} /* End workaround */

	hird = event >> DWC_DEVT_HIBER_HIRD_SHIFT &
	       DWC_DEVT_HIBER_HIRD_BITS >> DWC_DEVT_HIBER_HIRD_SHIFT;
	dwc_debug5(pcd->usb3_dev, "%s(%u), state=%d hird=%d ss=%d\n",
		   __func__, event, state, hird, is_superspeed);

	/* Enter hibernation if supported */
	if (pcd->usb3_dev->core_params->hibernate &&
	    (pcd->usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		/* Tell kernel thread to save state and enter hibernation */
		pcd->usb3_dev->hibernate = DWC_HIBER_ENTER_SAVE;
		return 1;
	}

	return 0;
}

/**
 * This interrupt indicates that the device has been disconnected.
 */
static int handle_disconnect_intr(dwc_usb3_pcd_t *pcd)
{
#ifdef USB_FOR_NU3000
	int turn_off_flag = pcd->state == DWC_STATE_CONFIGURED;
#endif
	//dwc_print0(pcd->usb3_dev, "DISCONNECT\n");

	dwc_usb3_clr_eps_enabled(pcd);
	dwc_usb3_pcd_stop(pcd);
	pcd->state = DWC_STATE_UNCONNECTED;
#ifdef USB_FOR_NU3000
	if (usb_pp.system_full_power_cb && turn_off_flag)
		usb_pp.system_full_power_cb(pcd->state);
#endif

	/* Enter hibernation if supported */
	if (pcd->usb3_dev->core_params->hibernate &&
	    pcd->usb3_dev->core_params->hiberdisc &&
	    (pcd->usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) ==
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT) {
		/* Tell kernel thread to enter hibernation */
		pcd->usb3_dev->hibernate = DWC_HIBER_ENTER_NOSAVE;
		return 1;
	}

	return 0;
}

/**
 * This interrupt occurs when a USB Reset is detected. When the USB Reset
 * Interrupt occurs, all transfers are stopped and the device state is set
 * to DEFAULT.
 */
static void handle_usb_reset_intr(dwc_usb3_pcd_t *pcd)
{
	dwc_usb3_pcd_ep_t *ep;
	int i;

	//dwc_print0(pcd->usb3_dev, "USB RESET\n");

	/* If UsbReset comes without Disconnect first, fake it, because the
	 * gadget may need to see a disconnect first. The Synopsys UAS gadget
	 * needs this.
	 */
#if 1
	if (pcd->state != DWC_STATE_UNCONNECTED) {
#ifndef USB_FOR_NU3000
		int hibersave = pcd->usb3_dev->core_params->hibernate;

		pcd->usb3_dev->core_params->hibernate = 0;
#endif
		handle_disconnect_intr(pcd);
#ifndef USB_FOR_NU3000
		pcd->usb3_dev->core_params->hibernate = hibersave;
#endif
	} else
#endif
	{
		/* Stop any active xfers on the non-EP0 endpoints */
		dwc_usb3_stop_all_xfers(pcd);
	}

	dwc_usb3_clr_eps_enabled(pcd);

	/* Clear stall on each EP */
	for (i = 0; i < pcd->num_in_eps; i++) {
		ep = pcd->in_ep[i];
		if (ep->dwc_ep.stopped)
			dwc_usb3_pcd_ep_clear_stall(pcd, ep);
	}
	for (i = 0; i < pcd->num_out_eps; i++) {
		ep = pcd->out_ep[i];
		if (ep->dwc_ep.stopped)
			dwc_usb3_pcd_ep_clear_stall(pcd, ep);
	}

#ifndef SELA_PLATFORM_NOCTL
	/* Set Device Address to 0 */
	dwc_usb3_set_address(pcd, 0);
#endif

	pcd->remote_wakeup_enable = 0;
	pcd->ltm_enable = 0;
}

/**
 * This interrupt occurs when a Connect Done is detected.
 * Read the device status register and set the device speed in the data
 * structure. Set up EP0 to receive SETUP packets.
 */
void dwc_usb3_handle_connect_done_intr(dwc_usb3_pcd_t *pcd)
{
	dwc_usb3_pcd_ep_t *ep0 = pcd->ep0;
#ifndef SELA_PLATFORM_NOCTL
	u32 diepcfg0, doepcfg0, diepcfg1, doepcfg1;
	dwc_usb3_dev_ep_regs_t __iomem *ep_reg;
#endif
	int speed;

#ifndef SELA_PLATFORM_NOCTL
	/* The timing on reconnect after hibernation is so tight that we
	 * cannot afford the overhead of printing this to the dmesg log!
	 */
	
	if (!pcd->usb3_dev->core_params->hibernate ||
	    (pcd->usb3_dev->hwparams1 & DWC_HWP1_EN_PWROPT_BITS) !=
	    DWC_EN_PWROPT_HIBERNATION << DWC_HWP1_EN_PWROPT_SHIFT)
		;//dwc_print0(pcd->usb3_dev, "CONNECT\n");
   
#ifdef DEBUG_EP0
	dwc_usb3_print_ep0_state(pcd);
#endif
#endif

	ep0->dwc_ep.stopped = 0;
	speed = dwc_usb3_get_device_speed(pcd);
	pcd->speed = speed;
	dwc_info2(pcd->usb3_dev, "at speed=%d dsts=%x\n", speed, dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts));

#ifndef SELA_PLATFORM_NOCTL
#ifdef DWC_STAR_9000483510_WORKAROUND
	if (speed == USB_SPEED_SUPER)
		handle_usb_reset_intr(pcd);
#endif
	/* Set the MPS of EP0 based on the connection speed */
	switch (speed) {
	case USB_SPEED_SUPER:
		pcd->ep0->dwc_ep.maxpacket = 512;
		break;

	case USB_SPEED_HIGH:
	case USB_SPEED_FULL:
		pcd->ep0->dwc_ep.maxpacket = 64;
		break;

	case USB_SPEED_LOW:
		pcd->ep0->dwc_ep.maxpacket = 8;
		break;
	}

	diepcfg0 = DWC_USB3_EP_TYPE_CONTROL << DWC_EPCFG0_EPTYPE_SHIFT;
	diepcfg0 |= DWC_CFG_ACTION_MODIFY << DWC_EPCFG0_CFG_ACTION_SHIFT;
	diepcfg1 = DWC_EPCFG1_XFER_CMPL_BIT | DWC_EPCFG1_XFER_IN_PROG_BIT |
		   DWC_EPCFG1_XFER_NRDY_BIT | DWC_EPCFG1_EP_DIR_BIT;

	doepcfg0 = DWC_USB3_EP_TYPE_CONTROL << DWC_EPCFG0_EPTYPE_SHIFT;
	doepcfg0 |= DWC_CFG_ACTION_MODIFY << DWC_EPCFG0_CFG_ACTION_SHIFT;
	doepcfg1 = DWC_EPCFG1_XFER_CMPL_BIT | DWC_EPCFG1_XFER_IN_PROG_BIT |
		   DWC_EPCFG1_XFER_NRDY_BIT;

	diepcfg0 |= pcd->ep0->dwc_ep.maxpacket << DWC_EPCFG0_MPS_SHIFT;
	doepcfg0 |= pcd->ep0->dwc_ep.maxpacket << DWC_EPCFG0_MPS_SHIFT;

#ifdef DWC_UTE
	ep0->dwc_ep.tx_fifo_num = pcd->txf_map[0];
#endif
	diepcfg0 |= ep0->dwc_ep.tx_fifo_num << DWC_EPCFG0_TXFNUM_SHIFT;

	dwc_usb3_dis_usb2_phy_suspend(pcd);

	/* Issue "DEPCFG" command to EP0-OUT */
	ep_reg = &pcd->out_ep_regs[0];
	dwc_usb3_dep_cfg(pcd, ep_reg, doepcfg0, doepcfg1, 0);

	/* Issue "DEPCFG" command to EP0-IN */
	ep_reg = &pcd->in_ep_regs[0];
	dwc_usb3_dep_cfg(pcd, ep_reg, diepcfg0, diepcfg1, 0);

	dwc_usb3_ena_usb2_phy_suspend(pcd);
#endif

	if (pcd->state == DWC_STATE_UNCONNECTED)
		pcd->state = DWC_STATE_DEFAULT;

	/* Inform the gadget of the connection and the speed */
	dwc_usb3_gadget_connect(pcd, speed);

	if (pcd->usb3_dev->hiber_wait_connect) {
		/* Already did 'Perform the steps in Section 9.1.3
		 * "Initialization on Connect Done" in [DWSS]'.
		 */

		dwc_debug0(pcd->usb3_dev, "Hibernation wait connect\n");
		pcd->usb3_dev->hiber_wait_connect = 0;
		dwc_exit_hibernation_after_connect(pcd, 0);
	}
}

/**
 * This interrupt indicates that the USB link state has changed.
 */
static void handle_link_status_change_intr(dwc_usb3_pcd_t *pcd)
{
	int state;
	int speed;

	if (pcd->usb3_dev->snpsid >= 0x5533230a)
		return;

	state = dwc_usb3_pcd_get_link_state(pcd);
	speed = dwc_usb3_get_device_speed(pcd);
	pcd->speed = speed;
	dwc_debug2(pcd->usb3_dev, "LINK state=%d speed=%d\n", state, speed);

	switch (state) {
	case DWC_LINK_STATE_U0:
		if (pcd->usb3_dev->hiber_wait_u0) {
			pcd->speed = speed;
			if (pcd->remote_wakeup_enable)
				dwc_usb3_pcd_remote_wake(pcd, 0);
			pcd->usb3_dev->hiber_wait_u0 = 0;
		}

		/* If transitioning from 3->0 */
		if (pcd->link_state == DWC_LINK_STATE_U3) {
			dwc_debug0(pcd->usb3_dev,
				   "Enabling function remote wake\n");
			pcd->wkup_rdy = 1;
		} else {
			pcd->wkup_rdy = 0;
		}

		pcd->link_state = state;
		break;

	case DWC_LINK_STATE_U3:
		/* If transitioning to 3 */
		if (pcd->link_state != DWC_LINK_STATE_U3)
			dwc_usb3_gadget_suspend(pcd);
		/* FALL-THRU */

	default:
		pcd->link_state = state;
		pcd->wkup_rdy = 0;
		break;
	}
}

/**
 * This interrupt indicates that the DWC_usb3 controller has detected a
 * resume or remote wakeup sequence.
 */
static void handle_wakeup_detected_intr(dwc_usb3_pcd_t *pcd)
{
	int state;

	dwc_debug0(pcd->usb3_dev,
		   "++Resume or Remote Wakeup Detected Interrupt++\n");
	dwc_debug1(pcd->usb3_dev, "DSTS=0x%01x\n",
		   dwc_rd32(pcd->usb3_dev, &pcd->dev_global_regs->dsts));
	state = dwc_usb3_pcd_get_link_state(pcd);
	pcd->link_state = state;

	if (state == DWC_LINK_STATE_U0)
		dwc_usb3_gadget_resume(pcd);
}

/**
 * This interrupt indicates that a U3/L2-L1 Suspend event has occurred.
 */
static void handle_u3_l2l1_susp_intr(dwc_usb3_pcd_t *pcd)
{
	int state;

	if (pcd->usb3_dev->snpsid < 0x5533230a)
		return;

	state = dwc_usb3_pcd_get_link_state(pcd);
	dwc_debug2(pcd->usb3_dev, "suspend %d->%d\n", pcd->link_state, state);

	switch (state) {
	case DWC_LINK_STATE_U0:
		/* If transitioning from 3->0 */
		if (pcd->link_state == DWC_LINK_STATE_U3) {
			dwc_debug0(pcd->usb3_dev,
				   "Enabling function remote wake\n");
			pcd->wkup_rdy = 1;
		} else {
			pcd->wkup_rdy = 0;
		}

		pcd->link_state = state;
		break;

	case DWC_LINK_STATE_U3:
		/* If transitioning to 3 */
		if (pcd->link_state != DWC_LINK_STATE_U3)
			dwc_usb3_gadget_suspend(pcd);
		/* FALL-THRU */

	default:
		pcd->link_state = state;
		pcd->wkup_rdy = 0;
		break;
	}
}

/**
 * This routine handles the SOF Interrupts. At this time the SOF Interrupt
 * is disabled.
 */
static void handle_sof_intr(dwc_usb3_pcd_t *pcd)
{
	dwc_debug0(pcd->usb3_dev, "SOF\n");
}

/**
 * This interrupt indicates that an EP has a pending interrupt.
 */
void dwc_usb3_handle_ep_intr(dwc_usb3_pcd_t *pcd, int physep, u32 event)
{
	dwc_usb3_pcd_ep_t *ep;
	int epnum, is_in, temp;
	char *dir;
	UNUSED(dir);

	dwc_debug4(pcd->usb3_dev, "%s(%lx,%d,0x%08x)\n", __func__,
		   (unsigned long)pcd, physep, event);

	/* Physical Out EPs are even, physical In EPs are odd */
	is_in = physep & 1;
	epnum = physep >> 1 & 0xf;

	/* Get EP pointer */
	if (is_in) {
		ep = dwc_usb3_get_in_ep(pcd, epnum);
		dir = "IN";
	} else {
		ep = dwc_usb3_get_out_ep(pcd, epnum);
		dir = "OUT";
	}

	dwc_debug1(pcd->usb3_dev, "%s EP intr\n", dir);

#ifdef VERBOSE
	dwc_debug4(pcd->usb3_dev, "EP%d-%s: type=%d mps=%d\n",
		   ep->dwc_ep.num, (ep->dwc_ep.is_in ? "IN" : "OUT"),
		   ep->dwc_ep.type, ep->dwc_ep.maxpacket);
#endif
#ifdef SELA_PLATFORM
	if (pcd->ep_event) {
		int evt = event >> DWC_DEPEVT_INTTYPE_SHIFT &
			  DWC_DEPEVT_INTTYPE_BITS >> DWC_DEPEVT_INTTYPE_SHIFT;
		evt |= epnum << 8 | is_in << 15;
		*pcd->ep_event = evt;
	}
#endif

	temp = pcd->usb3_dev->hibernate;
	if (temp >= DWC_HIBER_SLEEPING && temp != DWC_HIBER_WAIT_U0 &&
	    temp != DWC_HIBER_SS_DIS_QUIRK) {
		dwc_info3(pcd->usb3_dev,
			  "EP%d-%s: got event 0x%08x while hibernating\n",
			  ep->dwc_ep.num, (ep->dwc_ep.is_in ? "IN" : "OUT"),
			  event);
		return;
	}

#ifdef USB_FOR_NU3000
	if (usb_pp.ep_event_extension && usb_pp.ep_event_extension(pcd, ep, event))
			return;
#endif

	switch (event & DWC_DEPEVT_INTTYPE_BITS) {
	case DWC_DEPEVT_XFER_CMPL << DWC_DEPEVT_INTTYPE_SHIFT:
#ifdef VERBOSE
		dwc_debug2(pcd->usb3_dev, "[EP%d] %s xfer complete\n",
			   epnum, dir);
#endif
		ep->dwc_ep.xfer_started = 0;

		if (ep->dwc_ep.type != UE_ISOCHRONOUS) {
			/* Complete the transfer */
			if (epnum == 0)
				dwc_usb3_handle_ep0_xfer(pcd, event);
			else
				dwc_usb3_complete_request(pcd, ep, event);
		} else {
			dwc_print2(pcd->usb3_dev,
				   "[EP%d] %s xfer complete for ISOC EP!\n",
				   epnum, dir);
		}

		break;

	case DWC_DEPEVT_XFER_IN_PROG << DWC_DEPEVT_INTTYPE_SHIFT:
#ifdef VERBOSE
		dwc_debug2(pcd->usb3_dev, "[EP%d] %s xfer in progress\n",
			   epnum, dir);
#endif
		if (ep->dwc_ep.type == UE_ISOCHRONOUS) {
			/* Complete the transfer */
			dwc_usb3_complete_request(pcd, ep, event);
		} else {
//			dwc_print2(pcd->usb3_dev,
//				"[EP%d] %s xfer in progress for non-ISOC EP!\n",
//				 epnum, dir);

			/* Complete the transfer */
			if (epnum == 0)
				dwc_usb3_handle_ep0_xfer(pcd, event);
			else
				dwc_usb3_complete_request(pcd, ep, event);
		}

		break;

	case DWC_DEPEVT_XFER_NRDY << DWC_DEPEVT_INTTYPE_SHIFT:
		dwc_debug2(pcd->usb3_dev, "[EP%d] %s xfer not ready\n",
			   epnum, dir);

		if (epnum == 0) {
			switch (pcd->ep0state) {
			case EP0_IN_WAIT_GADGET:
			case EP0_IN_WAIT_NRDY:
				if (is_in)
					dwc_usb3_handle_ep0_xfer(pcd, event);
				break;
			case EP0_OUT_WAIT_GADGET:
			case EP0_OUT_WAIT_NRDY:
				if (!is_in)
					dwc_usb3_handle_ep0_xfer(pcd, event);
				break;
			default:
				break;
			}
		} else if (ep->dwc_ep.type == UE_ISOCHRONOUS) {
			dwc_isocdbg2(pcd->usb3_dev,
				     "[EP%d] %s xfer not ready for ISOC EP!\n",
				     epnum, dir);
			if (!ep->dwc_ep.xfer_started)
				dwc_usb3_gadget_isoc_ep_start(pcd, ep, event);
		}

		break;

	case DWC_DEPEVT_FIFOXRUN << DWC_DEPEVT_INTTYPE_SHIFT:
		dwc_error2(pcd->usb3_dev, "[EP%d] %s FIFO Underrun Error!\n",
			   epnum, dir);
		break;

	case DWC_DEPEVT_EPCMD_CMPL << DWC_DEPEVT_INTTYPE_SHIFT:
		dwc_error2(pcd->usb3_dev, "[EP%d] %s Command Complete!\n",
			   epnum, dir);
		break;

	default:
		dwc_error2(pcd->usb3_dev, "[EP%d] %s Unknown event!\n",
			   epnum, dir);
		break;
	}
}

void dwc_usb3_handle_dev_resume(dwc_usb3_pcd_t *pcd)
{
	pcd->link_state = dwc_usb3_pcd_get_link_state(pcd);

	if (pcd->link_state == DWC_LINK_STATE_U0)
		dwc_usb3_gadget_resume(pcd);
	return;
}

/**
 * PCD interrupt handler.
 *
 * The PCD handles the device interrupts. Many conditions can cause a
 * device interrupt. When an interrupt occurs, the device interrupt
 * service routine determines the cause of the interrupt and
 * dispatches handling to the appropriate routine.
 */
int dwc_usb3_handle_dev_intr(dwc_usb3_pcd_t *pcd, u32 event)
{
	u32 dint = event >> DWC_DEVT_SHIFT &
			DWC_DEVT_BITS >> DWC_DEVT_SHIFT;
	int temp, ret = 0;

#ifdef VERBOSE
	dwc_debug2(pcd->usb3_dev, "%s() event=%08x\n", __func__, event);
#endif
#ifdef SELA_PLATFORM
	if (pcd->dev_event)
		*pcd->dev_event = dint;
#endif

	temp = pcd->usb3_dev->hibernate;
	if (temp >= DWC_HIBER_SLEEPING && temp != DWC_HIBER_WAIT_U0 &&
	    temp != DWC_HIBER_SS_DIS_QUIRK) {
		dwc_info1(pcd->usb3_dev,
			  "Device: got event 0x%08x while hibernating\n",
			  event);
		return 0;
	}

	switch (dint) {
	case DWC_DEVT_DISCONN:
		dwc_info0(pcd->usb3_dev, "event Disconnect\n");
		ret = handle_disconnect_intr(pcd);
		break;

	case DWC_DEVT_USBRESET:
		dwc_info0(pcd->usb3_dev, "event USB Reset\n");
		handle_usb_reset_intr(pcd);
		break;

	case DWC_DEVT_CONNDONE:
		dwc_info0(pcd->usb3_dev, "event Connect Done\n");
		dwc_usb3_handle_connect_done_intr(pcd);
		break;

	case DWC_DEVT_ULST_CHNG:
		dwc_debug0(pcd->usb3_dev, "event Link Change\n");
		handle_link_status_change_intr(pcd);
		break;

	case DWC_DEVT_WKUP:
		dwc_info0(pcd->usb3_dev, "event Wakeup\n");
		handle_wakeup_detected_intr(pcd);
		break;

	case DWC_DEVT_HIBER_REQ:
		dwc_info0(pcd->usb3_dev, "event Hibernation Request\n");
		ret = handle_hiber_req_intr(pcd, event);
		break;

	case DWC_DEVT_U3_L2L1_SUSP:
		dwc_info0(pcd->usb3_dev, "event U3/L2-L1 Suspend Event\n");
		handle_u3_l2l1_susp_intr(pcd);
		break;

	case DWC_DEVT_SOF:
		dwc_debug0(pcd->usb3_dev, "Start of Frame\n");
		handle_sof_intr(pcd);
		break;

	case DWC_DEVT_ERRATICERR:
		dwc_info0(pcd->usb3_dev, "event Erratic Error\n");
		break;

	case DWC_DEVT_CMD_CMPL:
		dwc_debug0(pcd->usb3_dev, "Command Complete\n");
		break;

	case DWC_DEVT_OVERFLOW:
		dwc_info0(pcd->usb3_dev, "event Overflow\n");
		break;

	default:
      dwc_info0(pcd->usb3_dev, "event unknown\n"); //Inuitive Modification
		break;
	}

	return ret;
}
