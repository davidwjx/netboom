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
 *
 * Loopback/source-sink Function Driver which uses the application-specific
 * interface in no_os_gadget.c
 */

#include "os_defs.h"
#include "hw.h"
#include "usb.h"
#include "pcd.h"
#include "dev.h"
#include "os_dev.h"
#include "cil.h"
#include "verify.h"
#include "nu_regs.h"

#ifdef USB_FOR_NU3000
#include "usb_drv.h"
#include "sys_calls.h"
#include "fw_helpers.h"

USB_PP_ST usb_pp;
#endif

/** Driver context struct - defined in no_os_init.c */
extern dwc_usb3_device_t	g_usb3_dev;
int g_usb_is_loopbk_is_active = 0;


#ifndef LINUXTEST
#ifdef USB_FOR_NU3000
/** @{ */
/** Transfer buffers - 4KB each */
static char			g_in_buf[1024 * 4];
static char			g_out_buf[1024 * 4];
/** @} */
#else
/** @{ */
/** Transfer buffers - 1MB each */
static char			g_in_buf[1024 * 1024];
static char			g_out_buf[1024 * 1024];
/** @} */
#endif
#endif

#ifdef USB_FOR_NU3000
#define DWC_NUM_REQ	1
#define DWC_REQ_SIZ	(4 * 1024)
#else
/**
 * The FPGA configuration is limited to a maximum transfer size of 128K by
 * default, so allocate 8 128K requests for each 1MB buffer
 */
/** @{ */
#define DWC_NUM_REQ	8
#define DWC_REQ_SIZ	(128 * 1024)
/** @} */
#endif

/**
 * Function Driver context struct
 */
typedef struct dwc_usb3_loopbk {
	usb_ep_t	*in_ep, *out_ep;
	char		*in_buf, *out_buf;
	dwc_dma_t	in_dma, out_dma;
	int		speed, maxp;
	u8		cfg, ifc, next0, src_sink;
#ifdef USB_FOR_NU3000
#define MAX_NUM_OF_SPARE_EPS_IN 5
#define MAX_NUM_OF_SPARE_EPS_OUT 2
	usb_ep_t	*epin[MAX_NUM_OF_SPARE_EPS_IN];
	usb_ep_t	*epout[MAX_NUM_OF_SPARE_EPS_OUT];
#define MAX_NUM_OF_INTERFACES 16
	u8		sifc[MAX_NUM_OF_INTERFACES];
	u32		video_flag;
	u32		audio_flag;
#endif
} dwc_usb3_loopbk_t;

/**
 * The Function Driver context
 */
static dwc_usb3_loopbk_t	loopbk;
void (*fw_entry)(void);

#ifdef USB_FOR_NU3000
static void reset_other_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk);

typedef enum {
	NU3000_MC_READ_MEM_RANGE = 0x0100,
	NU3000_MC_WRITE_MEM_RANGE = 0x0120,
	NU3000_MC_BOOT_AT = 0x0310,
} NU3000_MONITOR_COMMANDS_ET;

usb_ep_t **nu3000_get_ep_array(int in, int *len)
{
	*len = in ? MAX_NUM_OF_SPARE_EPS_IN : MAX_NUM_OF_SPARE_EPS_OUT;
	return in ? loopbk.epin : loopbk.epout;
}

int nu3000_get_cur_device_state(dwc_usb3_device_t *dev)
{
	return dev->pcd.state;
}

/**
 * parse request and prepare reply in the same buffer
 */
static int nu3000_monitor_process(unsigned *buf, int len) __attribute__((noinline));
static int nu3000_monitor_process(unsigned *buf, int len)
{
   unsigned addr;
   unsigned length;
   unsigned op;
   int err = 0;
   unsigned i;

   if (usb_pp.monitor_extension)
      if (usb_pp.monitor_extension(buf, &len, &err))
         goto Exit;

   if ((unsigned)len < 3 * sizeof(unsigned))
   {
      err = EINVAL;
      len = 0;
      goto Exit;
   }
   op = buf[0];
   length = buf[1];
   addr = buf[2];

   buf[0] |= 0x3;
   buf[1] = 0;

   switch(op)
   {
      case NU3000_MC_READ_MEM_RANGE:
         if (!((getEfuseSecureControlData()!=NOT_SECURED_IMAGE) && ((addr>=(UINT32)GME_FUSE_SHADOW_0_REG && addr<=(UINT32)GME_FUSE_SHADOW_31_REG ) || (addr==(UINT32)GME_FUSE_CONTROL_REG ) || (addr==(UINT32)GME_FUSE_PROGRAM_VAL_REG))))//when on secure boot, do not allow read from efuse registers
         {
            if ((unsigned)len < 4 * sizeof(unsigned))
            {
               err = EINVAL;
               break;
            }
            length = buf[3];
            if (length > DWC_REQ_SIZ - 3 * sizeof(unsigned) || length % sizeof(unsigned))
            {
               err = ERANGE;
               break;
            }
            for(i = 0; i < length / sizeof(unsigned); i++)
               buf[3 + i] = REG32(addr + i * sizeof(unsigned));
            len = i * sizeof(unsigned);
         }
         break;
      case NU3000_MC_WRITE_MEM_RANGE:
         if (!((getEfuseSecureControlData()!=NOT_SECURED_IMAGE) && ((addr==(UINT32)GME_FUSE_CONTROL_REG ) || (addr==(UINT32)GME_FUSE_PROGRAM_VAL_REG))))//when on secure boot, do not allow read from efuse registers
         {
            length -= sizeof(unsigned);
            for(i = 0; i < length / sizeof(unsigned); i++)
               REG32(addr + i * sizeof(unsigned)) = buf[3 + i];
            len = 0;
         }
         break;
      case NU3000_MC_BOOT_AT:
         fw_entry = (void *)addr;
         len = 0;
         break;
      default:
         err = ENOENT;
         break;
   }

   if (err)
      len = 0;
Exit:
   buf[2] = err;
   buf[1] = len + sizeof(unsigned);
   return 3 * sizeof(unsigned) + len;
}

static void nu3000_send_to_host_complete(usb_ep_t *ep, usb_request_t *req)
{
	NU3000_USB_SEND_TO_HOST_ST *ctx = (void *)((char *)req->buf - sizeof(NU3000_USB_SEND_TO_HOST_ST));
	dwc_usb3_free_request(&g_usb3_dev, ep, req);
	ctx->complete(ctx);
}

int nu3000_usb_send_to_host(NU3000_USB_SEND_TO_HOST_ST *ctx)
{
	usb_request_t *req = dwc_usb3_alloc_request(&g_usb3_dev, loopbk.in_ep);
	if (!req)
		return EBUSY;
	req->buf = ctx->buf;
	req->dma = (dwc_dma_t)ctx->buf; /* assumming dma coherency! */
	req->length = ctx->len;
	req->complete = nu3000_send_to_host_complete;
	return dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);
}

void nu3000_bg_process(void)
{
   void (*temp_fw_entry)(void);

   if (fw_entry != 0) 
   {
      temp_fw_entry  = fw_entry;
      fw_entry       = 0;
      if (getEfuseSecureControlData()!=NOT_SECURED_IMAGE)//support integrity format or integrity & encryption
      {
         secureBoot((UINT32)temp_fw_entry);
      }
      temp_fw_entry();
   }
}

/* forward declaration */
static void disable_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk);
#endif

/**
 * Function Driver transfer complete callback routine
 */
static void loopbk_complete(usb_ep_t *ep, usb_request_t *req)
{
	int ret;

	dwc_debug1(&g_usb3_dev, "%s()\n", __func__);

	switch (req->status) {
	case 0:
		if (loopbk.src_sink) {
			if (ep == loopbk.out_ep) {
				dwc_debug0(&g_usb3_dev, "OUT req, requeuing\n");
				req->length = DWC_REQ_SIZ;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.out_ep, req);
				if (ret == 0)
					return;
				dwc_error0(&g_usb3_dev, "Failed to requeue OUT req\n");
			} else {
				dwc_debug0(&g_usb3_dev, "IN req, requeuing\n");
				req->length = DWC_REQ_SIZ;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);
				if (ret == 0)
					return;
				dwc_error0(&g_usb3_dev, "Failed to requeue IN req\n");
			}
		} else {
			if (ep == loopbk.out_ep) {
				dwc_debug0(&g_usb3_dev, "OUT req, requeuing on IN\n");

#ifndef USB_FOR_NU3000
				/* Handle 0-length marker packet */
				if (req->actual && (req->actual & (loopbk.maxp - 1)) == 0)
					loopbk.next0 = 1;

				/* Echo packet back to host on IN EP */
				req->length = req->actual;
#else
				req->length = nu3000_monitor_process(req->buf, req->actual);
#endif
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);

				if (ret == 0)
					return;
				loopbk.next0 = 0;
				dwc_error0(&g_usb3_dev, "Failed to queue IN req, requeuing on OUT\n");
			} else {
				dwc_debug0(&g_usb3_dev, "IN req, requeuing on OUT\n");
			}

			/* Handle 0-length marker packet */
			if (loopbk.next0) {
				loopbk.next0 = 0;
				req->length = 0;
				ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.in_ep, req);
				if (ret == 0)
					return;
				dwc_error0(&g_usb3_dev, "Failed to queue IN 0-length req, requeuing on OUT\n");
			}

			/* Requeue for a future OUT EP transfer */
			req->length = DWC_REQ_SIZ;
			ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.out_ep, req);
			if (ret == 0)
				return;
			dwc_error0(&g_usb3_dev, "Failed to queue OUT req\n");
		}

		dwc_usb3_free_request(&g_usb3_dev, ep, req);
		break;

	default:
		dwc_error0(&g_usb3_dev, "Bad completion status\n");

		/* Requeue for a future OUT EP transfer */
		req->length = DWC_REQ_SIZ;
		ret = dwc_usb3_ep_queue(&g_usb3_dev, loopbk.out_ep, req);
		if (ret == 0)
			return;
		dwc_error0(&g_usb3_dev, "Failed to queue OUT req\n");
		/* FALL-THRU */

	case -DWC_E_SHUTDOWN:
		dwc_debug0(&g_usb3_dev, "Shutdown status\n");
		dwc_usb3_free_request(&g_usb3_dev, ep, req);
		break;
	}
}

/**
 * Function Driver EP enable routine
 */
static int enable_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk)
{
	usb_ep_t *in_ep, *out_ep;

	switch (loopbk->speed) {
	case USB_SPEED_SUPER:
		in_ep = dwc_usb3_ep_enable(usb3_dev, &ss_config_desc.bulk_in_ep_desc,
					   &ss_config_desc.bulk_in_ss_ep_comp_desc);
		if (!in_ep)
			return -DWC_E_INVALID;

		out_ep = dwc_usb3_ep_enable(usb3_dev, &ss_config_desc.bulk_out_ep_desc,
					    &ss_config_desc.bulk_out_ss_ep_comp_desc);
		if (!out_ep) {
			dwc_usb3_ep_disable(usb3_dev, in_ep);
			return -DWC_E_INVALID;
		}

		break;

	case USB_SPEED_HIGH:
		in_ep = dwc_usb3_ep_enable(usb3_dev, &hs_config_desc.bulk_in_ep_desc, NULL);
		if (!in_ep)
			return -DWC_E_INVALID;

		out_ep = dwc_usb3_ep_enable(usb3_dev, &hs_config_desc.bulk_out_ep_desc, NULL);
		if (!out_ep) {
			dwc_usb3_ep_disable(usb3_dev, in_ep);
			return -DWC_E_INVALID;
		}

		break;

	case USB_SPEED_FULL:
		in_ep = dwc_usb3_ep_enable(usb3_dev, &fs_config_desc.bulk_in_ep_desc, NULL);
		if (!in_ep)
			return -DWC_E_INVALID;

		out_ep = dwc_usb3_ep_enable(usb3_dev, &fs_config_desc.bulk_out_ep_desc, NULL);
		if (!out_ep) {
			dwc_usb3_ep_disable(usb3_dev, in_ep);
			return -DWC_E_INVALID;
		}

		break;

	default:
		return -DWC_E_INVALID;
	}

	loopbk->in_ep = in_ep;
	loopbk->out_ep = out_ep;
	g_usb_is_loopbk_is_active = 1;

	return 0;
}

/**
 * Function Driver EP disable routine
 */
static void disable_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk)
{
	if (loopbk->out_ep) {
		dwc_usb3_ep_disable(usb3_dev, loopbk->out_ep);
		loopbk->out_ep = NULL;
	}

	if (loopbk->in_ep) {
		dwc_usb3_ep_disable(usb3_dev, loopbk->in_ep);
		loopbk->in_ep = NULL;
	}
}

/**
 * Function Driver SET_INTERFACE routine
 */
static int set_interface(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk, int alt)
{
	usb_request_t	*req;
	int		i, ret = 0;

	if (alt == -1)
		goto cleanup;

	/* Already set? */
	if (loopbk->out_ep)
		return 0;

	ret = enable_eps(usb3_dev, loopbk);
	if (ret)
		return ret;

	ret = -DWC_E_NO_MEMORY;

	if (loopbk->src_sink) {
		for (i = 0; i < DWC_NUM_REQ; i++) {
			req = dwc_usb3_alloc_request(usb3_dev, loopbk->in_ep);
			if (!req)
				goto cleanup;
			req->buf = loopbk->in_buf + DWC_REQ_SIZ * i;
			req->dma = loopbk->in_dma + DWC_REQ_SIZ * i;
			req->length = DWC_REQ_SIZ;
			req->complete = loopbk_complete;
			ret = dwc_usb3_ep_queue(usb3_dev, loopbk->in_ep, req);
			if (ret)
				goto cleanup;
		}
	}

	for (i = 0; i < DWC_NUM_REQ; i++) {
		req = dwc_usb3_alloc_request(usb3_dev, loopbk->out_ep);
		if (!req)
			goto cleanup;
		req->buf = loopbk->out_buf + DWC_REQ_SIZ * i;
		req->dma = loopbk->out_dma + DWC_REQ_SIZ * i;
		req->length = DWC_REQ_SIZ;
		req->complete = loopbk_complete;
		ret = dwc_usb3_ep_queue(usb3_dev, loopbk->out_ep, req);
		if (ret)
			goto cleanup;
	}

	return 0;

cleanup:
	/* disable_eps() will eventually dequeue all requests queued on each EP,
	 * and call the ->complete routine with -DWC_E_SHUTDOWN status for each
	 * one. That in turn will free the request. So all cleanup is done for
	 * us by this one call.
	 */
	disable_eps(usb3_dev, loopbk);
	loopbk->next0 = 0;
	g_usb_is_loopbk_is_active = 0;
	return ret;
}

#ifdef USB_FOR_NU3000
static void reset_other_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk)
{
	int i;
	if (usb_pp.reset_other_eps && usb_pp.reset_other_eps(usb3_dev, loopbk))
		return;

	for(i = 0; i < MAX_NUM_OF_SPARE_EPS_IN; i++)
	{
		if (loopbk->epin[i])
			dwc_usb3_ep_disable(usb3_dev, loopbk->epin[i]);
		loopbk->epin[i] = NULL;
	}
	for(i = 0; i < MAX_NUM_OF_SPARE_EPS_OUT; i++)
	{
		if (loopbk->epout[i])
			dwc_usb3_ep_disable(usb3_dev, loopbk->epout[i]);
		loopbk->epout[i] = NULL;
	}
	for(i = 0; i < MAX_NUM_OF_INTERFACES; i++)
		loopbk->sifc[i] = 0xff;
	loopbk->video_flag = loopbk->audio_flag = 0;
}

typedef struct {
	dwc_usb3_loopbk_t *loopbk;
	dwc_usb3_device_t *usb3_dev;
} set_other_ep_ctx_t;
static void mark_video(int curintf, void *hdr, dwc_usb3_loopbk_t *loopbk)
{
	/* couting on having no altsetting for video control interface */
	usb_interface_descriptor_t *ihdr = hdr;
	if (ihdr->bInterfaceClass == 14 && ihdr->bInterfaceSubClass == 1)
		loopbk->video_flag |= 1 << (curintf - 1);
	else if (ihdr->bInterfaceSubClass == 1 && ihdr->bInterfaceSubClass == 1)
		loopbk->audio_flag |= 1 << (curintf - 1);
}
static int is_video(u16 curintf, dwc_usb3_loopbk_t *loopbk, int vid)
{
	return (vid ? loopbk->video_flag : loopbk->audio_flag) & (1 << (curintf - 1));
}
static int set_other_ep(int ss, usb_descriptor_header_t *hdr, int curintf, int curalt, void *ctx)
{
	set_other_ep_ctx_t *ictx = ctx;
	int epn, epd;
	usb_ep_t **ep;

	if (curalt)
		return 0;
	ictx->loopbk->sifc[curintf - 1] = 0;
	if (hdr->bDescriptorType == UDESC_INTERFACE)
	       mark_video(curintf, hdr, ictx->loopbk);

	if (hdr->bDescriptorType != UDESC_ENDPOINT)
		return 0;
	epn = ((usb_endpoint_descriptor_t *)hdr)->bEndpointAddress;
	epd = UE_GET_DIR(epn);
	epn = UE_GET_ADDR(epn) - 2;
	ep = epd ? &ictx->loopbk->epin[epn] : &ictx->loopbk->epout[epn];

	if (*ep)
		return 1;
	*ep = dwc_usb3_ep_enable(ictx->usb3_dev, hdr, ss ? (char *)hdr + hdr->bLength : NULL);
	return *ep ? 0 : 1;
}
static int set_other_eps(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk)
{
	set_other_ep_ctx_t ictx = {loopbk, usb3_dev, };
	if (loopbk->speed == USB_SPEED_FULL)
		return 0;

	if (usb_pp.set_other_eps)
	{
		int ret;
		if (usb_pp.set_other_eps(usb3_dev, loopbk, &ret))
			return ret;
	}

	if (pass_desc_rest(loopbk->speed == USB_SPEED_SUPER, set_other_ep, &ictx))
	{
		reset_other_eps(usb3_dev, loopbk);
		return -DWC_E_INVALID;
	}
	return 0;
}

typedef struct {
	dwc_usb3_loopbk_t *loopbk;
	dwc_usb3_device_t *usb3_dev;
	u8 single_intf;
	u8 single_alt; /* if single_intf: bit 0..6 - alt; 7 - set(0)/unset(1) */
	u8 found_alt;
} set_single_intf_ctx_t;
static int set_single_intf_ep(int ss, usb_descriptor_header_t *hdr, int curintf, int curalt, void *ctx)
{
	set_single_intf_ctx_t *ictx = ctx;
	int epn, epd;
	usb_ep_t **ep;
	u8 single_alt = ictx->single_alt & 0x7f;
	u8 disable = ictx->single_alt & 0x80;

	if (curintf != ictx->single_intf || curalt != single_alt)
		return 0;
	ictx->found_alt = 1;
	ictx->loopbk->sifc[curintf - 1] = curalt;

	if (hdr->bDescriptorType != UDESC_ENDPOINT)
		return 0;
	epn = ((usb_endpoint_descriptor_t *)hdr)->bEndpointAddress;
	epd = UE_GET_DIR(epn);
	epn = UE_GET_ADDR(epn) - 2;
	ep = epd ? &ictx->loopbk->epin[epn] : &ictx->loopbk->epout[epn];

	if (disable)
	{
		if (!*ep)
			return 0;
		dwc_usb3_ep_disable(ictx->usb3_dev, *ep);
		*ep = NULL;
		return 0;
	}
	else
	{
		if (*ep)
			return 1;
		*ep = dwc_usb3_ep_enable(ictx->usb3_dev, hdr, ss ? (char *)hdr + hdr->bLength : NULL);
		return *ep ? 0 : 1;
	}
}
static int set_single_intf(dwc_usb3_device_t *usb3_dev, dwc_usb3_loopbk_t *loopbk, u8 intf, u8 alt)
{
	set_single_intf_ctx_t ictx = {loopbk, usb3_dev, intf, };
	if (loopbk->speed == USB_SPEED_FULL)
		return 0;

	if (usb_pp.set_single_intf)
	{
		int ret;
		if (usb_pp.set_single_intf(usb3_dev, loopbk, intf, alt, &ret))
			return ret;
	}

	/* disable current alt */
	ictx.single_alt = loopbk->sifc[intf - 1] | 0x80;
	pass_desc_rest(loopbk->speed == USB_SPEED_SUPER, set_single_intf_ep, &ictx);
	/* enable new alt */
	ictx.found_alt = 0;
	ictx.single_alt = alt;
	if (pass_desc_rest(loopbk->speed == USB_SPEED_SUPER, set_single_intf_ep, &ictx))
	{
		/* disable it back */
		ictx.single_alt |= 0x80;
		pass_desc_rest(loopbk->speed == USB_SPEED_SUPER, set_single_intf_ep, &ictx);
		return -DWC_E_INVALID;
	}
	return ictx.found_alt ? 0 : -DWC_E_NOT_SUPPORTED;
}

int nu3000_class_request(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl, int *len)
{
	u16 windex = UGETW(ctrl->wIndex);
	u16 wvalue = UGETW(ctrl->wValue);

	if (ctrl->bmRequestType == 0xa1 && ctrl->bRequest == 0x81 && wvalue == 0x0200 && !(windex & 0xff00) && is_video(windex, &loopbk, 1))
	{
		/* GET_CUR for VC_REQUEST_ERROR_CODE_CONTROL on a UVC interface */
		pcd->ep0_status_buf[0] = 0x2; /* wrong state */
		*len = 1;
		return 0;
	}
	else if (ctrl->bmRequestType == 0xa1 && ctrl->bRequest == 0x81 && wvalue == 0x0100 && is_video(windex & 0xff, &loopbk, 0))
	{
		/* GET_CUR for control 1 on any unit of UAC interface:
		   - COPY_PROTECT_CONTROL on the terminals: 0 is disabled.
		   - MUTE_CONTROL on feature unit: 0 is not-muted.
		   - XU_ENABLE_CONTROL on extension unit: 0 is disabled.
		 */
		pcd->ep0_status_buf[0] = 0;
		*len = 1;
		return 0;
	}

	return -1;
}
#endif

/**
 * This routine handles Function Driver specific Setup requests. Generic
 * requests are handled in ep0.c and no_os_ep0.c.
 *
 * @param pcd   Programming view of DWC_usb3 peripheral controller.
 * @param ctrl  Pointer to the Setup packet for the request.
 * @return      0 for success, else negative error code.
 */
int dwc_usb3_function_setup(dwc_usb3_pcd_t *pcd, usb_device_request_t *ctrl)
{
	u16	windex = UGETW(ctrl->wIndex);
	u16	wvalue = UGETW(ctrl->wValue);
	u16	wlength = UGETW(ctrl->wLength);
	int	len = 0, ret = 0;

#ifdef USB_FOR_NU3000
	if (usb_pp.ep0_extension && usb_pp.ep0_extension(0, pcd, ctrl, &ret))
		return ret;
#endif

	switch (ctrl->bRequest) {
	case UR_SET_FEATURE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_FEATURE\n");

		/* We don't need to do anything for this */
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	case UR_SET_INTERFACE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_INTERFACE\n");
#ifdef USB_FOR_NU3000
		if (windex > MAX_NUM_OF_INTERFACES || loopbk.sifc[windex - 1] == 0xff)
			return -DWC_E_NOT_SUPPORTED;
		if (!loopbk.cfg)
			return -DWC_E_NOT_SUPPORTED;
		if (windex)
		{
			if (loopbk.sifc[windex - 1] != wvalue)
			{
				ret = set_single_intf(pcd->usb3_dev, &loopbk, windex, wvalue);
				if (ret)
					return ret;
			}
			pcd->ep0->dwc_ep.is_in = 1;
			pcd->ep0state = EP0_IN_WAIT_NRDY;
			// if interface 0 is active re-enable it
			if (g_usb_is_loopbk_is_active) {
				set_interface(pcd->usb3_dev, &loopbk, -1);
				set_interface(pcd->usb3_dev, &loopbk, 0);
			}
			return 0;
		}
#else
		if (!loopbk.cfg || windex) {
			dwc_error2(pcd->usb3_dev, "cfg=%x wIndex=%x\n", loopbk.cfg, windex);
			return -DWC_E_NOT_SUPPORTED;
		}
#endif

		dwc_debug1(pcd->usb3_dev, "ifc=%x\n", wvalue);

		/* If interface has changed, disable the old EPs and enable the new ones */
		if (loopbk.ifc != wvalue) {
			set_interface(pcd->usb3_dev, &loopbk, -1);
			ret = set_interface(pcd->usb3_dev, &loopbk, wvalue);
			if (ret)
				return ret;
		}

		loopbk.ifc = wvalue;
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	case UR_GET_INTERFACE:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_GET_INTERFACE\n");
		if (!loopbk.cfg) {
			dwc_error1(pcd->usb3_dev, "cfg=%x\n", loopbk.cfg);
			return -DWC_E_NOT_SUPPORTED;
		}

#ifdef USB_FOR_NU3000
		if (windex && windex <= MAX_NUM_OF_INTERFACES && loopbk.sifc[windex - 1] != 0xff) {
			pcd->ep0_status_buf[0] = loopbk.sifc[windex - 1];
			len = 1;
			break;
		}
#endif
		if (windex) {
			dwc_error1(pcd->usb3_dev, "wIndex=%x\n", windex);
			return -DWC_E_DOMAIN;
		}

		pcd->ep0_status_buf[0] = loopbk.ifc;
		len = 1;
		break;

	case UR_SET_CONFIG:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SET_CONFIG\n");
		if (wvalue != 0 && wvalue != 1) { // we only have one configuration
			dwc_error1(pcd->usb3_dev, "wValue=%x\n", wvalue);
			return -DWC_E_NOT_SUPPORTED;
		}

		/* If config already set, clear it and disable the EPs */
		if (loopbk.cfg) {
			loopbk.cfg = 0;
			loopbk.ifc = 0;
			set_interface(pcd->usb3_dev, &loopbk, -1);
#ifdef USB_FOR_NU3000
			reset_other_eps(pcd->usb3_dev, &loopbk);
#endif
		}

		/* If new config is 1, enable the EPs for interface 0 */
		if (wvalue) {
			loopbk.cfg = wvalue;
			loopbk.ifc = 0;
			ret = set_interface(pcd->usb3_dev, &loopbk, 0);
			if (ret)
				loopbk.cfg = 0;
#ifdef USB_FOR_NU3000
			else
			{
				ret = set_other_eps(pcd->usb3_dev, &loopbk);
				if (ret)
				{
					loopbk.cfg = 0;
					set_interface(pcd->usb3_dev, &loopbk, -1);
				}
			}
#endif
		}

		dwc_debug1(pcd->usb3_dev, "cfg=%x\n", loopbk.cfg);
		if (ret)
			return ret;
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	case UR_GET_CONFIG:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_GET_CONFIG\n");
		pcd->ep0_status_buf[0] = loopbk.cfg;
		len = 1;
		break;

	case UR_SYNCH_FRAME:
		dwc_debug0(pcd->usb3_dev, "USB_REQ_SYNCH_FRAME\n");

		/* We don't need to do anything for this */
		pcd->ep0->dwc_ep.is_in = 1;
		pcd->ep0state = EP0_IN_WAIT_NRDY;
		return 0;

	default:
#ifdef USB_FOR_NU3000
		if (!nu3000_class_request(pcd, ctrl, &len))
			break;
#endif
		dwc_debug0(pcd->usb3_dev, "Unknown request!\n");
		return -DWC_E_NOT_SUPPORTED;
	}

#ifdef USB_FOR_NU3000
	if (usb_pp.ep0_extension && usb_pp.ep0_extension(1, pcd, ctrl, &ret))
		return ret;
#endif

	/* Start the data phase for 3-stage transfers */
	pcd->ep0state = EP0_IN_DATA_PHASE;
	dwc_usb3_pcd_ep0_data_stage(pcd, len < wlength ? len : wlength);

	return 0;
}

/**
 * Function Driver CONNECT routine
 */
int dwc_usb3_function_connect(struct dwc_usb3_device *usb3_dev, int speed)
{
	loopbk.speed = speed;

	switch (speed) {
	case USB_SPEED_SUPER:
		loopbk.maxp = 1024;
		break;

	case USB_SPEED_HIGH:
		loopbk.maxp = 512;
		break;

	case USB_SPEED_FULL:
		loopbk.maxp = 64;
		break;
	}

	return 0;
}

/**
 * Function Driver DISCONNECT routine
 */
int dwc_usb3_function_disconnect(struct dwc_usb3_device *usb3_dev)
{
	return 0;
}

/**
 * Function Driver initialization routine
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 * @return              0 for success, else negative error code.
 */
int dwc_usb3_function_init(dwc_usb3_device_t *usb3_dev)
{
	dwc_debug1(usb3_dev, "%s()\n", __func__);

	loopbk.src_sink = 0;	/* for now */

#ifndef LINUXTEST
	if (loopbk.src_sink) {
		loopbk.in_buf = g_in_buf;
		loopbk.in_dma = (dwc_dma_t)g_in_buf;
	}

	loopbk.out_buf = g_out_buf;
	loopbk.out_dma = (dwc_dma_t)g_out_buf;

	return 0;
#else
	if (loopbk.src_sink) {
		loopbk.in_buf = dma_alloc_coherent(&usb3_dev->pcidev->dev, 1024 * 1024,
						   &loopbk.in_dma, GFP_KERNEL | GFP_DMA32);
		if (!loopbk.in_buf)
			goto out0;
	}

	loopbk.out_buf = dma_alloc_coherent(&usb3_dev->pcidev->dev, 1024 * 1024,
					    &loopbk.out_dma, GFP_KERNEL | GFP_DMA32);
	if (!loopbk.out_buf)
		goto out1;

	return 0;

out1:
	if (loopbk.src_sink)
		dma_free_coherent(&usb3_dev->pcidev->dev, 1024 * 1024, loopbk.in_buf,
				  loopbk.in_dma);
out0:
	return -DWC_E_NO_MEMORY;
#endif
}

/**
 * Function Driver removal routine
 *
 * @param usb3_dev      Programming view of DWC_usb3 device.
 */
void dwc_usb3_function_remove(dwc_usb3_device_t *usb3_dev)
{
	dwc_debug1(usb3_dev, "%s()\n", __func__);

#ifdef LINUXTEST
	dma_free_coherent(&usb3_dev->pcidev->dev, 1024 * 1024, loopbk.out_buf, loopbk.out_dma);
	if (loopbk.src_sink)
		dma_free_coherent(&usb3_dev->pcidev->dev, 1024 * 1024, loopbk.in_buf, loopbk.in_dma);
#endif
}
