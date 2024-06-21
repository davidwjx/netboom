/****************************************************************************
 * 
 *   FileName: usb_ctrl_com.c
 *
 *   Author: Konstantin Sinyuk
 *
 *   Date: 
 *
 *   Description: USB control com interface 
 *
 ****************************************************************************/
/****************************************************************************
 ***************       I N C L U D E    F I L E S             ***************
 ****************************************************************************/
#define INUUSB_EXPORTS


#include "inu_usb.h"
#include "inu_boot.h"
#include "libusb-1.0/libusb.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif


#define USB_EVENT_THREAD_SLEEP_USEC  (100*1000)
#define USB_CLOSE_WAIT_USEC  (80*1000)
#define USB_CLOSE_WAIT_RETRIES (10)
#define USB_READ_TIMEOUT_MSEC (500)
#define USB_INT_READ_TIMEOUT_MSEC (1)

#define MAX_INT_BUF_SIZE (1024)
#define MAX_INT_ALLOWED_TRANSFERS (20)
#define USB_INTERFCAE_CTRL_IDX  0

#define USB3_BULK_EP_SIZE (1024)

#define USB_NUM_INTERFACE (7)

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef pthread_mutex_t mutexT;

typedef struct
{
  char buf[USB3_BULK_EP_SIZE];
  unsigned int cnt;
  unsigned int idx;
} bulkEpBufT;

typedef struct {   
   libusb_hotplug_callback_handle   	hpHandle;   
   pthread_t                  		hp_usb_event_thread;
   pthread_t 		      		int_poll_thread;
   libusb_context                	*hpCntx;
   libusb_context                	*devCntx;
   libusb_device                	*dev;
   libusb_device_handle         	*devHandle;
   InuUsbParamsT 			inuUsbParams;
   InuHotplugCallback 			hpCallback;
   InuIntCallback 			intCallback;
   char 				bootPath[1024];
   InuLog   				log;
   mutexT 				devm;
   int 					closeInProg;
   int 					rdInProg;
   int 					usbDetectionThreadStatus;
   int 					usbIntThreadStatus;
   int 					isUsbInserted;
   int 					usbUseInterruptEndpoint;
   unsigned char 			intBufs[MAX_INT_BUF_SIZE * MAX_INT_ALLOWED_TRANSFERS];
   struct libusb_transfer 		*intXfrs[MAX_INT_ALLOWED_TRANSFERS]; 
   int 					intNumOfTransfers;
   bulkEpBufT        buf[USB_NUM_INTERFACE];
   int               thrSleepTimeUs;
} UsbDeviceInfoT;





/****************************************************************************
 ***************       G L O B A L        D A T A              ***************
 ****************************************************************************/

UsbDeviceInfoT g_inuUsDeviceInfo = {0};
/****************************************************************************
 ***************       L O C A L         F U N C              ***************
 ****************************************************************************/

static int minit(mutexT *mutexP)
{
   int ret;
   pthread_mutexattr_t attr;

   //initialize attributes
   ret = pthread_mutexattr_init(&attr);
   if(!ret) 
   {
      //init mutex with attributes
      ret = pthread_mutex_init(mutexP, &attr);
      if(!ret)
         ret = pthread_mutexattr_destroy(&attr);
      else
         (void)pthread_mutexattr_destroy(&attr);
   }
   return ret;
}

static void mlock(mutexT *mutexP)
{
   pthread_mutex_lock(mutexP);
}

static void munlock(mutexT *mutexP)
{
   pthread_mutex_unlock(mutexP);
}

static void mdeinit(mutexT *mutexP)
{
   pthread_mutex_destroy(mutexP);
}

static inline void set_locked(int *var, int value, mutexT *m)
{
   mlock(m);
   *var = value;
   munlock(m);
}

static inline unsigned int get_locked(int *var, mutexT *m)
{
   unsigned int value;

   mlock(m);
   value = *var;
   munlock(m);

   return value;
}
static int matchMultiDevice(libusb_device *dev)
{
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	struct libusb_device_descriptor desc = {0};
	int rc;
	if (dev != NULL) {
		rc = libusb_get_device_descriptor(dev, &desc);
		if (rc != 0) {
			usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"can't get device descriptor rc=%d\n",rc);
			return 0;
		} else {			
			if (usbDevice->inuUsbParams.usbDeviceAddress) {
				if (usbDevice->inuUsbParams.usbDeviceAddress != desc.bDeviceSubClass) {
					usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"hotplug_multi_device: expecting usbDeviceAddress %d, but found %d skipping ...\n",usbDevice->inuUsbParams.usbDeviceAddress,desc.bDeviceSubClass);
					return 0;
				}
			}			
			return 1;	
		}
	}
	return 0;	
}
static int isKernelInsertion(libusb_device *dev)
{
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	struct libusb_device_descriptor desc = { 0 };
	int rc;

	if (dev != NULL) {
		rc = libusb_get_device_descriptor(dev, &desc);
		if (rc != 0) {
			usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "can't get device descriptor rc=%d\n", rc);
			return 0;
		}
		else {
			if ((desc.bcdDevice & KERNEL_BCD_DEVICE_MASK) == 0x0) {
				usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "kernel reenumertion detected\n");
				return 1;
			}
		}
	}
	return 0;
}


static int LIBUSB_CALL hotplug_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   
   if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED)
   {      
      // open USB instance on each insertion both external and internal        
	  if (inu_usb_open(USB_INTERFCAE_CTRL_IDX) == INU_USB_ERR_SUCCESS)
	  {
		  usbDevice->isUsbInserted = 1;
#ifdef __ANDROID__      
		  if (isKernelInsertion(libusb_get_device(usbDevice->devHandle)))
#else
      if (isKernelInsertion(usbDevice->dev))
#endif      
			 inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_OS, usbDevice->bootPath, usbDevice->hpCallback);
		  else
		  {
              if (strlen(usbDevice->bootPath) == 0)
                 inu_usb_boot_hotplug_handler(INU_USB_MODE_DETECT_CHIP_VERSION, usbDevice->bootPath, usbDevice->hpCallback);
              else inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_RAW, usbDevice->bootPath, usbDevice->hpCallback);


		  }
	  }
   }
   else if (event == LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT)
   {
      usbDevice->isUsbInserted = 0;
      inu_usb_boot_hotplug_handler(INU_USB_MODE_UNPLUGGED, usbDevice->bootPath, usbDevice->hpCallback);
      // close USB instance on each removal      
      inu_usb_close(USB_INTERFCAE_CTRL_IDX);
   }
   else
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Unhandled event %d\n", event);
   }
   return 0;
}

static void inu_usb_get_chip_id()
{
	const char *delim1 = "_";
	char *token;
	char chip_id_str[40];
	char string[128];
	unsigned int chip_id = 0;
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	//get the device string
	if (libusb_get_string_descriptor_ascii(usbDevice->devHandle, 3, (unsigned char*)string, 128) > 0)
	{
		token = strtok(string, "$");
		//usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "token = %s \n", token);
		while (token != NULL)
		{
			if (strcmp(token, "K") == 0)
			{
				token = strtok(NULL, delim1);
				strcpy(chip_id_str, token);
				break;
			}
			token = strtok(NULL, delim1);
		}
		if (strcmp(chip_id_str, "3000") == 0)
		{
			chip_id = 0x30000000;
		}
		else if (strcmp(chip_id_str, "4000") == 0)
		{
			chip_id = 0x40000000;
		}
      else if (strcmp(chip_id_str, "4001") == 0)
      {
         chip_id = 0x40000001;
      }
		else if (strcmp(chip_id_str, "4002") == 0)
		{
			chip_id = 0x40000002;
		}
      else if (strcmp(chip_id_str, "4100") == 0)
      {
         chip_id = 0x41000000;
      }
		usbDevice->inuUsbParams.version = chip_id;
		usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "chip_id_str = %s chip_id_val = %x\n", chip_id_str, chip_id);
	}
}

/****************************************************************************
 ***************       G L O B A L       F U N C              ***************
 ****************************************************************************/

InuUsbParamsT *inu_usb_get_contex()
{   
   return &g_inuUsDeviceInfo.inuUsbParams;
}

InuUsbErrorE inu_usb_set_mode(unsigned char usbInterfaceIdx, InuUsbModeE mode)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"inu_usb_set_mode: mode=%d\n",mode);

   if (mode == INU_USB_MODE_UNPLUGGED) {   
      return INU_USB_ERR_SUCCESS;
   } else if (mode == INU_USB_MODE_PLUGGED_OS) {
      return INU_USB_ERR_SUCCESS;
   } else {
      return INU_USB_ERR_SUCCESS;
   }   
   return INU_USB_ERR_SUCCESS;
}

static InuUsbErrorE inu_usb_wait_close(void)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   InuUsbErrorE ret =INU_USB_ERR_SUCCESS;
   unsigned int retry = 0;

   //wait for read to end
   while(get_locked(&usbDevice->rdInProg, &usbDevice->devm))
   {
      usleep(USB_CLOSE_WAIT_USEC); 
      if(retry++ == USB_CLOSE_WAIT_RETRIES) {
         ret = INU_USB_ERR_CLOSE_TIMEOUT;
         break;
      }
   }

   return ret;
}

static void inu_usb_try_close_sync()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   InuUsbErrorE ret;

   if (usbDevice->devHandle) {
     if(get_locked(&usbDevice->closeInProg, &usbDevice->devm)) {
         ret = inu_usb_wait_close();
         if(ret) {
            usbDevice->inuUsbParams.log(INU_USB_LOG_WARN, "libusb force close while read/write in progress (waited %u msec)\n", 
               (USB_CLOSE_WAIT_USEC/1000)*USB_CLOSE_WAIT_RETRIES);
         }
	 if (usbDevice->usbIntThreadStatus) {
	 	//TODO
		inu_usb_deinit_interrupt_async(USB_INTERFCAE_CTRL_IDX);
	 }
	
         inu_usb_set_mode(0, INU_USB_MODE_UNPLUGGED);
         libusb_release_interface(usbDevice->devHandle, 0);
	 if (usbDevice->usbUseInterruptEndpoint)
	 	libusb_release_interface(usbDevice->devHandle, 1);


	 	libusb_close(usbDevice->devHandle);
         libusb_exit(usbDevice->devCntx);
         usbDevice->devHandle = NULL;
         usbDevice->devCntx = NULL; 
		 // update usb status to disconnected
		 usbDevice->isUsbInserted = 0;
         set_locked(&usbDevice->closeInProg, 0, &usbDevice->devm);		 
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_close success\n");		 
      }	  
   }   
}


void *usb_event_thread_func(void *x)
{
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   
   while (usbDevice->usbDetectionThreadStatus) {
      struct timeval tv;
      if (!usbDevice->inuUsbParams.usbUseExternalHpCallback) {
	      tv.tv_sec = 0;
	      tv.tv_usec = USB_EVENT_THREAD_SLEEP_USEC;
	      rc = libusb_handle_events_timeout(usbDevice->hpCntx, &tv);
	      if (rc < 0) {
	         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"libusb_handle_events_timeout() failed: %s\n", libusb_error_name(rc));
	         break;
	      }
      }
	
	  inu_usb_try_close_sync();
#ifdef  USE_HOTPLUG_POLLING	  
      // poll for plugin status, used to work around the short reconnect cycle (that happens with watchdog)
	  if (!usbDevice->isUsbInserted) {
		   if (inu_usb_open(USB_INTERFCAE_CTRL_IDX) == INU_USB_ERR_SUCCESS) 
			{
				usbDevice->isUsbInserted = 1;
#ifdef __ANDROID__      
			   if (isKernelInsertion(libusb_get_device(usbDevice->devHandle)))
#else
   	      if (isKernelInsertion(usbDevice->dev))
#endif
				{
			   	inu_usb_get_chip_id();
					inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_OS, usbDevice->bootPath, usbDevice->hpCallback);
				}
				else
				{
		            if (strlen(usbDevice->bootPath) == 0)
		               inu_usb_boot_hotplug_handler(INU_USB_MODE_DETECT_CHIP_VERSION, usbDevice->bootPath, usbDevice->hpCallback);
		            else inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_RAW, usbDevice->bootPath, usbDevice->hpCallback);
				}
			}
      }
#endif
      usleep(usbDevice->thrSleepTimeUs);
   }
   return NULL;
}

void *int_event_thread_func(void *x)
{
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   
   while (usbDevice->usbIntThreadStatus) {      
	struct timeval tv = { 0, USB_EVENT_THREAD_SLEEP_USEC };
	rc = libusb_handle_events_timeout(usbDevice->devCntx, &tv);
	if (rc < 0) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"int_event_thread_func exiting ...\n");
		break;
	}
   }
   return NULL;
}


libusb_device *find_device_with_vid(libusb_context *ctx, uint16_t vendor_id)
{
   struct libusb_device **devs;
   struct libusb_device *found = NULL;
   struct libusb_device *dev;   
   size_t i = 0;
   int r;

   if (libusb_get_device_list(ctx, &devs) < 0)
      return NULL;

   while ((dev = devs[i++]) != NULL) {
      struct libusb_device_descriptor desc;
      r = libusb_get_device_descriptor(dev, &desc);
      if (r < 0)
         goto out;
      if (desc.idVendor == vendor_id) {
		 if (matchMultiDevice(dev)) {
			found = dev;
			break;
		 }
      }
   }   

out:
   libusb_free_device_list(devs, 1);
   return found;
}

static unsigned int read_ep_buf(unsigned char *buf, bulkEpBufT *epBuf, unsigned int max)
{
   unsigned int readlen = 0;

   if((epBuf->cnt) > 0)
   {
      readlen = (epBuf->cnt > max) ? max : epBuf->cnt;
      memcpy(buf, &epBuf->buf[epBuf->idx], readlen);
      epBuf->cnt -= readlen;
      epBuf->idx += readlen;
   }
   
   return readlen;
}

static void update_ep_buf(bulkEpBufT *epBuf, unsigned int cnt,unsigned int idx)
{
   epBuf->cnt = cnt;
   epBuf->idx = idx;
}

/*
* function: buffered_bulk_read
* 
* libusb can have internal overflows when not reading a multiple of the endpoint size.
* This function read multiples of endpoint size only (1KB is good for USB2/3) to avoid overflows.
* An internal buffer is used toi save extra data read from device (up to 1KB).The extra is copied to user on the next read.
*
* May return with less bytes than requested (and success or timeout status).
*
*/
static int buffered_bulk_read(unsigned char interfaceIdx, unsigned char *bufP, unsigned int len, unsigned int *transferred_size)
{
   int rc = LIBUSB_SUCCESS;
   unsigned int result = 0;
   unsigned int rem;
   unsigned int lenToRead;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   bulkEpBufT *remBuf = &usbDevice->buf[interfaceIdx];

   *transferred_size = 0;

   usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "libusb: begin buffered read %d \n", len);

   //Read from internal buffer first
   result = read_ep_buf(bufP, remBuf, len);
   if(result > 0)
   {
      *transferred_size += result;
      len -= result;
      bufP += result;
      result = 0;
   }

   if(len == 0)
     return rc;
   
   update_ep_buf(remBuf,0,0); //At this point internal buffer is empty

   rem = len % USB3_BULK_EP_SIZE;
   lenToRead = len-rem;
   if(lenToRead)
   {   
      //Read multiple of ep size
      result = 0;
      rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), bufP, lenToRead, (int *)&result, USB_READ_TIMEOUT_MSEC);
      if((rc == LIBUSB_SUCCESS) || ((rc == LIBUSB_ERROR_TIMEOUT) && (result > 0) && (result < lenToRead)))
      {
         *transferred_size += result;
         usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "libusb: read rc= %d result=%u,%u\n", rc, result, lenToRead);
      }
      else
      {
         if(rc != LIBUSB_ERROR_TIMEOUT)
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb: error read rc=%d result=%d len=%d(%d)\n", rc, result, lenToRead, len);
      }
   }

   //Continue to read remainder only if read everything, otherwise return and caller will retry
   if((result == lenToRead) && (rem > 0))
   {
      //Read remainder. Avoid libusb overflow by reading up to ep size
      result = 0;
      rc =libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), (unsigned char *)remBuf->buf, USB3_BULK_EP_SIZE, (int *)&result, USB_READ_TIMEOUT_MSEC);
      if((rc == LIBUSB_SUCCESS) || ((rc == LIBUSB_ERROR_TIMEOUT) && (result > 0) && (result < USB3_BULK_EP_SIZE)))
      {
         unsigned int copyLen = (result > rem) ? rem : result;
         memcpy(bufP+lenToRead, remBuf->buf, copyLen);
         *transferred_size += copyLen;

         if(result > rem)
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "libusb: read rem rc= %d result=%d,%d rem=%d@%d\n", rc, result, lenToRead,result-rem,rem);
            update_ep_buf(remBuf, (result-rem), rem);
         }
         else
            usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "libusb: read rem rc= %d result=%d,%d rem=%d\n", rc, result, lenToRead, rem);
      }
      else
      {
         if(rc != LIBUSB_ERROR_TIMEOUT)
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb: error read rem rc=%d result=%d len=%d(%d)\n", rc, result, lenToRead, len);
      }
   }

   return rc;
}

static InuUsbErrorE usb_read_bulk(unsigned char interfaceIdx, int isBuffered, unsigned char *buffer, unsigned int size, unsigned int *transferred_size)
{
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   InuUsbErrorE ret;
   int closing = 0;
   
   if (!usbDevice->isUsbInserted)
      return INU_USB_ERR_DISCONNECTED;

   set_locked(&usbDevice->rdInProg, 1, &usbDevice->devm);
   closing = get_locked(&usbDevice->closeInProg, &usbDevice->devm);
   while(!closing)
   {
      if(isBuffered)
      {
         rc = buffered_bulk_read(interfaceIdx, buffer, size, transferred_size);
      }
      else
      {
         rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, (int *)transferred_size, USB_READ_TIMEOUT_MSEC);
      }
      if((rc == LIBUSB_SUCCESS) || (rc == LIBUSB_ERROR_TIMEOUT))
      {
          ret = INU_USB_ERR_SUCCESS;
          break;
      }
      else
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"libusb read error %d\n",rc);
         if (rc == LIBUSB_ERROR_NO_DEVICE) {
            usbDevice->isUsbInserted = 0;
            ret = INU_USB_ERR_DISCONNECTED;
         }
         else if (rc == LIBUSB_ERROR_OVERFLOW) {
            ret = INU_USB_ERR_OVERFLOW;
         }
         else {
            ret = INU_USB_ERR_IO_FAILED;
         }
         break;
      }
      closing = get_locked(&usbDevice->closeInProg, &usbDevice->devm);
   }
   set_locked(&usbDevice->rdInProg, 0, &usbDevice->devm);
   
   if(closing)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"exiting read bulk - detected closing\n");
      ret = INU_USB_ERR_IO_FAILED;
   }
   
   return ret;
}


/****************************************************************************
 ***************       E X P O R T E D       F U N C          ***************
 ****************************************************************************/
int inu_usb_get_descriptor_bootId()
{
   return 0;
}

INUUSB_API
InuUsbErrorE inu_usb_hotplug_callback_external(InuUsbRawStatusE usbEvent, int usbFd)
{
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	
	if (usbEvent == INU_USB_EVT_RAW_INSERTION) {
		usbDevice->inuUsbParams.usbFd = usbFd;
		hotplug_callback(NULL, NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED,0);
	} else {
		hotplug_callback(NULL, NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT,0);
	}
	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_get_boot_path(char *bootPath)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   strcpy(bootPath, usbDevice->bootPath);
   
   return INU_USB_ERR_SUCCESS;
}


INUUSB_API
InuUsbErrorE inu_usb_init(InuUsbParamsT *inuUsbParams)
{         
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;   

   rc = minit(&usbDevice->devm);
   if(rc) {
      return INU_USB_ERR_INIT_FAILED;
   }
   
   memset(usbDevice, 0, sizeof(UsbDeviceInfoT));
   memcpy(&usbDevice->inuUsbParams, inuUsbParams, sizeof(InuUsbParamsT));   
   usbDevice->hpCallback = inuUsbParams->hp_callback;
   usbDevice->thrSleepTimeUs = inuUsbParams->thrSleepTimeUs;

   strcpy(usbDevice->bootPath, inuUsbParams->boot_path);

   if (!usbDevice->inuUsbParams.log)       
      usbDevice->inuUsbParams.log = inu_console_log;
   
   if (!inuUsbParams->usbUseExternalHpCallback)
   {
	   rc = libusb_init (&usbDevice->hpCntx);   
	   if (rc < 0)
	   {
		  usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"failed to initialise libusb: %s\n", libusb_error_name(rc));
		  return INU_USB_ERR_INIT_FAILED;
	   }

	   if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
		  usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Hotplug capabilities are not supported on this platform\n");
		  libusb_exit (NULL);
		  return INU_USB_ERR_INIT_FAILED;
	   }

#ifdef  USE_HOTPLUG_POLLING
	   rc = libusb_hotplug_register_callback (usbDevice->hpCntx, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, INU_VENDOR_ID,
		  LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &(usbDevice->hpHandle));
	   if (LIBUSB_SUCCESS != rc) {
		  usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Error registering callback 0\n");
		  libusb_exit (NULL);
		  return INU_USB_ERR_INIT_FAILED;
	   }
#else
	  rc = libusb_hotplug_register_callback (usbDevice->hpCntx, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED | LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, 0, INU_VENDOR_ID,
		 LIBUSB_HOTPLUG_MATCH_ANY, LIBUSB_HOTPLUG_MATCH_ANY, hotplug_callback, NULL, &(usbDevice->hpHandle));
	  if (LIBUSB_SUCCESS != rc) {
		 usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Error registering callback 0\n");
		 libusb_exit (NULL);
		 return INU_USB_ERR_INIT_FAILED;
	  }

	  if (inu_usb_open(USB_INTERFCAE_CTRL_IDX) == INU_USB_ERR_SUCCESS) {      
		hotplug_callback(usbDevice->hpCntx, NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, NULL);
   	  }	  
#endif	  
   }   
   
   usbDevice->usbDetectionThreadStatus=1;
   pthread_create(&(usbDevice->hp_usb_event_thread),NULL,usb_event_thread_func,NULL);
   
   return INU_USB_ERR_SUCCESS;
}


INUUSB_API
InuUsbErrorE inu_usb_change_params(InuUsbParamsT *inuUsbParams)
{         
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;   
   
   strcpy(usbDevice->bootPath, inuUsbParams->boot_path);
   
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_deinit()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   if (!usbDevice->inuUsbParams.usbUseExternalHpCallback)
   {	 
	   libusb_hotplug_deregister_callback(usbDevice->hpCntx, usbDevice->hpHandle);   
	   libusb_exit (usbDevice->hpCntx);	
   }
   usbDevice->usbDetectionThreadStatus=0;
   if (usbDevice->hp_usb_event_thread != NULL)
   {
	   pthread_join(usbDevice->hp_usb_event_thread, NULL);
	   usbDevice->hp_usb_event_thread = NULL;
   }
   
   mdeinit(&usbDevice->devm);
   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"HotK monitor closed successfully!\n");
   return INU_USB_ERR_SUCCESS;
}


INUUSB_API
InuUsbErrorE inu_usb_open_non_ctl_enp()
{
	return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_close_non_ctl_enp()
{
	return INU_USB_ERR_SUCCESS;
}


INUUSB_API
InuUsbErrorE inu_usb_open(unsigned char interfaceIdx)
{
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo; 

   //first check for possible closing in progress and wait for completion
   while(get_locked(&usbDevice->closeInProg, &usbDevice->devm)) {
      usleep(USB_CLOSE_WAIT_USEC);
   }

   if (usbDevice->devHandle == NULL) {

		rc = libusb_init (&usbDevice->devCntx);

      if (rc < 0)
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"failed to initialise libusb: %s\n", libusb_error_name(rc));
         return INU_USB_ERR_INIT_FAILED;
      }

#ifdef __ANDROID__
    usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"wrapping...\n");		 
	  if (usbDevice->inuUsbParams.usbFd)
		  rc = libusb_wrap_fd(usbDevice->devCntx, usbDevice->inuUsbParams.usbFd, &(usbDevice->devHandle));
#else
      usbDevice->dev = find_device_with_vid(usbDevice->devCntx, INU_VENDOR_ID);
      if (usbDevice->dev == NULL) {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"No connected device found...\n");		 
		 libusb_exit(usbDevice->devCntx);
         usbDevice->devCntx = NULL;
         return INU_USB_ERR_NO_DEVICE;
      }

		rc = libusb_open(usbDevice->dev, &(usbDevice->devHandle));
#endif

      if (LIBUSB_SUCCESS != rc) {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Could not open USB device,rc=%d\n",rc);
         return INU_USB_ERR_INIT_FAILED;
      }
	rc = libusb_claim_interface(usbDevice->devHandle, 0);
	if (rc < 0) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Error claiming interface,rc=%d\n",rc);
	}
	rc = libusb_claim_interface(usbDevice->devHandle, 1);
	if (rc < 0) {	
		usbDevice-> usbUseInterruptEndpoint = 0;
	}  else {
		usbDevice->usbUseInterruptEndpoint = 1;
	}

        usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"inu_usb_open: success\n");
   }
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_close(unsigned char interfaceIdx)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   if (usbDevice->devHandle)
   {
      set_locked(&usbDevice->closeInProg, 1, &usbDevice->devm);
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"inu_usb_close begin\n");
   }

   return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_write_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size,  unsigned int *transfered_size)
{   
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   if (!usbDevice->isUsbInserted)
      return INU_USB_ERR_DISCONNECTED;

   rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_OUT_EP+interfaceIdx), buffer, size, (int *)transfered_size,0);

    if(rc == 0 && *transfered_size == size)
      return INU_USB_ERR_SUCCESS;   
   return INU_USB_ERR_IO_FAILED;   
}

INUUSB_API
InuUsbErrorE inu_usb_write_interrupt(unsigned char interfaceIdx,unsigned char *buffer, unsigned int size,  unsigned int *transfered_size)
{   
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   if (!usbDevice->isUsbInserted)
      return INU_USB_ERR_DISCONNECTED;

   rc = libusb_interrupt_transfer(usbDevice->devHandle, INT_OUT_EP, buffer, size, (int *)transfered_size,0);

    if(rc == 0 && *transfered_size == size)
      return INU_USB_ERR_SUCCESS;   
   return INU_USB_ERR_IO_FAILED;   
}

INUUSB_API 
InuUsbErrorE inu_usb_read_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
   return usb_read_bulk(interfaceIdx, 0, buffer, size, transfered_size);
}

INUUSB_API 
InuUsbErrorE inu_usb_buffered_read_bulk(unsigned char interfaceIdx,unsigned char *buffer, unsigned int size, unsigned int *transferred_size)
{
   return usb_read_bulk(interfaceIdx, 1, buffer, size, transferred_size);
}

INUUSB_API 
InuUsbErrorE inu_usb_read_interrupt(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   InuUsbErrorE ret;
   int closing = 0;

   if (!usbDevice->isUsbInserted)
      return INU_USB_ERR_DISCONNECTED;

   set_locked(&usbDevice->rdInProg, 1, &usbDevice->devm);
   closing = get_locked(&usbDevice->closeInProg, &usbDevice->devm);
   while(!closing)
   {
      rc = libusb_interrupt_transfer(usbDevice->devHandle, INT_IN_EP, buffer, size, (int *)transfered_size, USB_INT_READ_TIMEOUT_MSEC);
      if((rc == LIBUSB_SUCCESS) || (rc == LIBUSB_ERROR_TIMEOUT))
      {
          ret = INU_USB_ERR_SUCCESS;
          break;
      }
      else
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"libusb read error %d\n",rc);
         if (rc == LIBUSB_ERROR_NO_DEVICE) {
            usbDevice->isUsbInserted = 0;
            ret = INU_USB_ERR_DISCONNECTED;
         }
         else if (rc == LIBUSB_ERROR_OVERFLOW) {
            ret = INU_USB_ERR_OVERFLOW;
         }
         else {
            ret = INU_USB_ERR_IO_FAILED;
         }
         break;
      }
      closing = get_locked(&usbDevice->closeInProg, &usbDevice->devm);
   }
   set_locked(&usbDevice->rdInProg, 0, &usbDevice->devm);
   
   if(closing)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"exiting read bulk - detected closing\n");
      ret = INU_USB_ERR_IO_FAILED;
   }
   
   return ret;
}

static void cb_interrupt(struct libusb_transfer *xfr)
{
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	if (xfr->status != LIBUSB_TRANSFER_COMPLETED) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"int transfer error status=%d\n", xfr->status);
		libusb_free_transfer(xfr);
		return;
	}

	// do not resubmit request if user callback returns non zero value
	if (usbDevice->intCallback(xfr->buffer,xfr->actual_length))
		return;
	// do not submit while exiting ...
	if (!usbDevice->usbIntThreadStatus)
		return;

	if (libusb_submit_transfer(xfr) < 0) {
		usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"can't resubmit int transfers\n");
		return;
	}

}


INUUSB_API 
InuUsbErrorE inu_usb_init_interrupt_async(unsigned char interfaceIdx, unsigned int transfer_size, unsigned int num_of_transfers, InuIntCallback intCallback)
{
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	InuUsbErrorE ret = INU_USB_ERR_SUCCESS;
	int i;

	if (!usbDevice->usbUseInterruptEndpoint)
	    return INU_USB_ERR_INIT_FAILED;

	if (num_of_transfers > MAX_INT_ALLOWED_TRANSFERS || transfer_size > MAX_INT_BUF_SIZE)
		return INU_USB_ERR_INIT_FAILED;

	usbDevice->intNumOfTransfers  = num_of_transfers;

	usbDevice->usbIntThreadStatus=1;
	pthread_create(&(usbDevice->int_poll_thread),NULL,int_event_thread_func,NULL);
	usbDevice->intCallback = intCallback;
	for (i=0; i<num_of_transfers; i++) 
	{
		usbDevice->intXfrs[i] = libusb_alloc_transfer(0);
		if (!usbDevice->intXfrs[i])
			return INU_USB_ERR_ALLOC_FAILURE;
		libusb_fill_interrupt_transfer(usbDevice->intXfrs[i], usbDevice->devHandle, INT_IN_EP, usbDevice->intBufs + i*transfer_size, transfer_size, cb_interrupt, NULL, 0);
	}

	for (i=0; i<num_of_transfers; i++) 
	{
		if (libusb_submit_transfer(usbDevice->intXfrs[i]) <0) {
			usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"can't submit int transfers\n");
			return INU_USB_ERR_TRANSFER_FAILURE;
		}
	}
   
   return ret;
}

INUUSB_API 
InuUsbErrorE inu_usb_deinit_interrupt_async(unsigned char interfaceIdx)
{
	UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
	InuUsbErrorE ret = INU_USB_ERR_SUCCESS;
	int i;

	if (!usbDevice->usbUseInterruptEndpoint)
	    return INU_USB_ERR_INIT_FAILED;

	usbDevice->usbIntThreadStatus=0;
	pthread_join(usbDevice->int_poll_thread, NULL);

	for (i=0; i < usbDevice->intNumOfTransfers; i++) 
	{
		if (libusb_cancel_transfer(usbDevice->intXfrs[i]) <0) {
			usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"can't cancel int transfers\n");
			continue;
		}	
	}
	return ret;
}


#ifdef __cplusplus
}
#endif
