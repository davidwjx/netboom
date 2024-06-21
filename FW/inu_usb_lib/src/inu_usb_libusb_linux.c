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
#include "libusb.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 
#include <unistd.h>
#include <string.h>
#include <sys/file.h>
#include <errno.h>


#ifdef __cplusplus
extern "C" {
#endif

#define USB_EVENT_THREAD_SLEEP_USEC    (100*1000)
#define USB_CLOSE_WAIT_USEC            (80*1000)
#define USB_CLOSE_WAIT_RETRIES         (10)
#define USB_READ_TIMEOUT_MSEC          (500)
#define USB_INT_READ_TIMEOUT_MSEC      (1)
#define MAX_INT_BUF_SIZE               (1024)
#define MAX_INT_ALLOWED_TRANSFERS      (20)
#define USB_INTERFCAE_CTRL_IDX         0
#define USB3_BULK_EP_SIZE              (1024)
#define USB_NUM_INTERFACE              (7)
//TODO: need to think how to make it work with inu_host_test
#define REGISTERED_DEVICE_FILE_NAME      "../config/RegisteredDevices.txt"
#define REGISTERED_DEVICE_TEMP_FILE_NAME "../config/RegisteredDevicesTemp.txt"
#define LOCK_FILE_FILE_NAME              "../config/lockFile"


/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef pthread_mutex_t mutexT;

typedef struct
{
  unsigned char buf[USB3_BULK_EP_SIZE];
  unsigned int cnt;
  unsigned int idx;
} bulkEpBufT;

typedef struct {
   char                                serialNumber[16];
   char                                modelNumber[16];
   int                                 bootId;
} UsbProductInfoT;

#define BOOT_FILTER_SERIAL_NUM_MAX_NUM (5)

typedef struct {
   char                                serialNumber[BOOT_FILTER_SERIAL_NUM_MAX_NUM][16];
   char                                modelNumber[BOOT_FILTER_SERIAL_NUM_MAX_NUM][16];
   int                                 bootId;
} UsbBootFilterInfoT;

typedef struct {   
   libusb_hotplug_callback_handle      hpHandle;   
   pthread_t                           hp_usb_event_thread;
   libusb_context                      *devCntx;
   libusb_device                       *dev;
   libusb_device_handle                *devHandle;
   InuUsbParamsT                       inuUsbParams;
   InuHotplugCallback                  hpCallback;
   char                                bootPath[1024];
   InuLog                              log;
   mutexT                              devm;
   int                                 closeInProg;
   int                                 wrInProg[USB_NUM_INTERFACE];
   int                                 rdInProg[USB_NUM_INTERFACE];
   int                                 usbDetectionThreadStatus;
   int                                 isUsbInserted;
   unsigned char                       intBufs[MAX_INT_BUF_SIZE * MAX_INT_ALLOWED_TRANSFERS];
   struct libusb_transfer              *intXfrs[MAX_INT_ALLOWED_TRANSFERS]; 
   int                                 intNumOfTransfers;
   bulkEpBufT                          buf[USB_NUM_INTERFACE];
   int                                 dev_address;
   int                                 port_num;
   int                                 bus_num;
   UsbProductInfoT                     productInfo;
   UsbBootFilterInfoT                  bootFilter;
   int                                 thrSleepTimeUs;
} UsbDeviceInfoT;

/****************************************************************************
 ***************       G L O B A L        D A T A              ***************
 ****************************************************************************/
int interProcessMutex;
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
      {
         ret = pthread_mutexattr_destroy(&attr);
      }
      else
      {
         (void)pthread_mutexattr_destroy(&attr);
      }
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

static void inu_usb_sleep(int usec)
{
   usleep(usec);
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

static int inu_usb_get_sec_time()
{
   struct timeval tv;

   gettimeofday(&tv, NULL);

   return tv.tv_sec;
}

static void inu_usb_parse_product_info(UsbProductInfoT *usbProductInfo, char *str)
{
   const char *delim = "_";
   char *token;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   /* if there is no production data, exit*/
   if (strcmp(str, "0") == 0)
      return;

   token = strtok(str, delim);
   if (!token)
      return;
   strcpy(usbProductInfo->serialNumber, token);

   token = strtok(NULL, delim);
   if (!token)
      return;
   strcpy(usbProductInfo->modelNumber, token);

   token = strtok(NULL, delim);
   if (!token)
      return;
   usbProductInfo->bootId = atoi(token);

   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "Serial = %s, Model = %s, bootId = %d\n",usbProductInfo->serialNumber,usbProductInfo->modelNumber,usbProductInfo->bootId);
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

/*
   The function will check if the device meets the filter.
   returns 0 if not, 1 if devices meets the filter 
*/
static int inu_usb_boot_filter()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   char string[128];
   int i;
   const char *delim = "$";
   char *token;

   memset(&usbDevice->productInfo,0,sizeof(UsbProductInfoT));

   //get the device string
   if (libusb_get_string_descriptor_ascii(usbDevice->devHandle, 3, (unsigned char*)string, 128) > 0)
   {
	   token = strtok(string, delim);
      //Parse and save the device info
      inu_usb_parse_product_info(&usbDevice->productInfo, token);
   }

   /* check if the device exists in the filter:
          1. check bootId - bootId 0 always pass, incase of boards with no production data
          2. check serial number from a given list
     */
   if ((usbDevice->productInfo.bootId == 0) || (usbDevice->bootFilter.bootId == 0))
      return 1;

   if (usbDevice->productInfo.bootId == usbDevice->bootFilter.bootId)
      return 1;

   for (i = 0; i < BOOT_FILTER_SERIAL_NUM_MAX_NUM; i++)
   {
      if (strcmp(usbDevice->productInfo.serialNumber,usbDevice->bootFilter.serialNumber[i]) == 0)
         return 1;
   }

   return 0;
}

/*
Search the file if there is entry for this process instance already -
left over from previous run or exception
*/
static int inu_usb_cleanDeviceFromFile()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   int usb_id,file_dev_address,file_bus_num,file_port_num,time,current_time;
   FILE *fp,*tempFp;

   fp     = fopen(REGISTERED_DEVICE_FILE_NAME, "r+");  
   if (fp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"open failed file %s(%s)\n",REGISTERED_DEVICE_FILE_NAME, strerror(errno));
       return -1;
   }

   tempFp = fopen(REGISTERED_DEVICE_TEMP_FILE_NAME, "w+");
   if (tempFp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"open failed file %s(%s)\n",REGISTERED_DEVICE_TEMP_FILE_NAME, strerror(errno));
       fclose(fp);
       return -1;
   }

   current_time = inu_usb_get_sec_time();
   while(fscanf(fp, "%d %d %d %d %d\n",&usb_id,&file_dev_address,&file_bus_num,&file_port_num,&time) != EOF )
   {
     /* 1. Check if there is an entry with same processId, clean it
             2. Check if the device keepalive time. If it wasn't updated then the thread(process) is probably dead and 
                 we can clean the device */
      if ((usb_id != usbDevice->inuUsbParams.usbDeviceAddress) && (time > (current_time - 10)))
      {
         fprintf(tempFp, "%d %d %d %d %d\n",usb_id,file_dev_address,file_bus_num, file_port_num, time);
      }
      else
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG,"found, clean line - %d %d %d %d time: %d\n",usb_id,file_dev_address,file_bus_num, file_port_num,time);
      }
   }

   fclose(fp);
   fclose(tempFp);
   
   remove(REGISTERED_DEVICE_FILE_NAME);
   rename(REGISTERED_DEVICE_TEMP_FILE_NAME, REGISTERED_DEVICE_FILE_NAME);

   return 0;
}

/*
Searchs the given address,bus and port in the file.
If found,remove it and return 1, If not, return 0.
*/
static int inu_usb_removeDevice()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   int file_dev_address,file_bus_num,file_port_num,file_process_id,time,found = 0;
   FILE *fp,*tempFp;

   fp     = fopen(REGISTERED_DEVICE_FILE_NAME, "r");  
   if (fp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"open failed file %s(%s)\n",REGISTERED_DEVICE_FILE_NAME, strerror(errno));
       return 0;
   }

   tempFp = fopen(REGISTERED_DEVICE_TEMP_FILE_NAME, "w+");
   if (tempFp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"open failed file %s(%s)\n",REGISTERED_DEVICE_TEMP_FILE_NAME, strerror(errno));
       fclose(fp);
       return 0;
   }

   while(fscanf(fp, "%d %d %d %d %d\n",&file_process_id,&file_dev_address,&file_bus_num,&file_port_num,&time) != EOF )
   {
      if ((file_dev_address == usbDevice->dev_address) && 
          (file_bus_num == usbDevice->bus_num) && 
          (file_port_num == usbDevice->port_num) && 
          (file_process_id == usbDevice->inuUsbParams.usbDeviceAddress))
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"found, removing line - %d %d %d %d time: %d\n",file_process_id,file_dev_address,file_bus_num, file_port_num,time);
         usbDevice->dev_address = 0;
         usbDevice->bus_num     = 0;
         usbDevice->port_num    = 0;
         found = 1;
      }
      else
      {
         fprintf(tempFp, "%d %d %d %d %d\n",file_process_id,file_dev_address,file_bus_num, file_port_num,time);
      }
   }

   fclose(fp);
   fclose(tempFp);

   remove(REGISTERED_DEVICE_FILE_NAME);
   rename(REGISTERED_DEVICE_TEMP_FILE_NAME, REGISTERED_DEVICE_FILE_NAME);

   return found;
}


/*
Searchs the given address,bus and port in the already used devices file list.
If found, return 0, If not, add the device to the file and return 1.
*/
static int inu_usb_markDevice(libusb_device *dev)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   int file_dev_address=0,dev_address = libusb_get_device_address (dev);
   int file_bus_num=0,bus_num    = libusb_get_bus_number(dev);
   int file_port_num=0,port_num  = libusb_get_port_number(dev);
   int file_process_id=0,time,current_time ;
   FILE *fp;

   fp=fopen(REGISTERED_DEVICE_FILE_NAME, "r+");

   if (fp == NULL)
   {
       //if file does not exist, create one
       fp=fopen(REGISTERED_DEVICE_FILE_NAME, "w+");
       if (fp == NULL)
       {
          usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"open and create failed for file %s(%s)\n",REGISTERED_DEVICE_FILE_NAME, strerror(errno));
          return 0;
       }
   }

   current_time = inu_usb_get_sec_time();
   while(fscanf(fp, "%d %d %d %d %d\n",&file_process_id,&file_dev_address,&file_bus_num,&file_port_num,&time) != EOF )
   {
      if ((file_dev_address == dev_address) && 
          (file_bus_num == bus_num) && 
          (file_port_num == port_num))
      {
         /* the device exists in the file, first check keepalive status, could that this device belonged to a killed process */
         if ((time <= (current_time - (10))))
         {
            /* the device is no longer active, we can claim it */
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"old device %d %d %d registered, claiming it for this process\n",dev_address,bus_num, port_num);
         }
         else
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG,"device %d %d %d already registered\n",dev_address,bus_num, port_num);
            fclose(fp);
            return 0;
         }
      }
   }

   usbDevice->dev_address = dev_address;
   usbDevice->bus_num     = bus_num;
   usbDevice->port_num    = port_num;

   //add new device to file
   time = inu_usb_get_sec_time();
   fprintf(fp, "%d %d %d %d %d\n",usbDevice->inuUsbParams.usbDeviceAddress,dev_address,bus_num, port_num,time);
   fclose(fp);

   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "device %d %d %d time: %d registered\n", dev_address, bus_num, port_num,time);

   return 1;
}


/*
The function will search the current device list if the device we are working with still exists in it.
If exists, it will update the time
*/
static libusb_device *inu_usb_check_device_exists(libusb_context *ctx, uint16_t vendor_id)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   struct libusb_device **devs;
   struct libusb_device *found = NULL;
   struct libusb_device *dev;
   int dev_address;
   int bus_num;
   int port_num;
   int file_dev_address, file_bus_num, file_port_num, time, usb_id;
   FILE *fp, *tempFp;
   size_t i = 0;
   int r;

   if (libusb_get_device_list(ctx, &devs) < 0)
      return NULL;

   while ((dev = devs[i++]) != NULL)
   {
      struct libusb_device_descriptor desc;
      r = libusb_get_device_descriptor(dev, &desc);
      if (r < 0)
         goto out;

      dev_address = libusb_get_device_address(dev);
      bus_num = libusb_get_bus_number(dev);
      port_num = libusb_get_port_number(dev);
      if ((desc.idVendor == vendor_id) && (usbDevice->dev_address == dev_address) && (usbDevice->bus_num == bus_num) && (usbDevice->port_num == port_num))
      {
         found = dev;
         break;
      }
   }

   if (found == NULL)
      goto out;

   fp = fopen(REGISTERED_DEVICE_FILE_NAME, "r");
   if (fp == NULL)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "open failed file %s(%s)\n", REGISTERED_DEVICE_FILE_NAME, strerror(errno));
      return NULL;
   }

   tempFp = fopen(REGISTERED_DEVICE_TEMP_FILE_NAME, "w+");
   if (tempFp == NULL)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "open failed file %s(%s)\n", REGISTERED_DEVICE_TEMP_FILE_NAME, strerror(errno));
      fclose(fp);
      return NULL;
   }

   while (fscanf(fp, "%d %d %d %d %d\n", &usb_id, &file_dev_address, &file_bus_num, &file_port_num, &time) != EOF)
   {
      /* find our device in the file and update its keepalive time */
      if ((usbDevice->inuUsbParams.usbDeviceAddress == usb_id) && (usbDevice->dev_address == file_dev_address) && (usbDevice->bus_num == file_bus_num) && (usbDevice->port_num == file_port_num))
      {
         fprintf(tempFp, "%d %d %d %d %d\n", usb_id, file_dev_address, file_bus_num, file_port_num, inu_usb_get_sec_time());
      }
      else
      {
         fprintf(tempFp, "%d %d %d %d %d\n", usb_id, file_dev_address, file_bus_num, file_port_num, time);
      }
   }

   fclose(fp);
   fclose(tempFp);

   remove(REGISTERED_DEVICE_FILE_NAME);
   rename(REGISTERED_DEVICE_TEMP_FILE_NAME, REGISTERED_DEVICE_FILE_NAME);


out:
   libusb_free_device_list(devs, 1);
   return found;
}

static int isKernelInsertion(libusb_device *dev)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   struct libusb_device_descriptor desc = { 0 };
   int rc;

   if (dev != NULL) {
      rc = libusb_get_device_descriptor(dev, &desc);
      if (rc != 0)
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "can't get device descriptor rc=%d\n", rc);
         return 0;
      }
      else 
      {
         if ((desc.bcdDevice & KERNEL_BCD_DEVICE_MASK) == 0x0) 
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "kernel reenumertion detected\n");
            return 1;
         }
      }
   }
   return 0;
}

static InuUsbErrorE inu_usb_wait_close(void)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   InuUsbErrorE ret =INU_USB_ERR_SUCCESS;
   unsigned int retry = 0;

   //wait for read to end
   while(get_locked(&usbDevice->rdInProg[0], &usbDevice->devm) || get_locked(&usbDevice->rdInProg[1], &usbDevice->devm) || 
         get_locked(&usbDevice->wrInProg[0], &usbDevice->devm) || get_locked(&usbDevice->wrInProg[1], &usbDevice->devm))
   {
      inu_usb_sleep(USB_CLOSE_WAIT_USEC);
      if(retry++ == USB_CLOSE_WAIT_RETRIES) {
         ret = INU_USB_ERR_CLOSE_TIMEOUT;
         break;
      }
   }
   return ret;
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


static void inu_usb_try_close_sync()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   InuUsbErrorE ret;
   if (usbDevice->devHandle) 
   {
     if(get_locked(&usbDevice->closeInProg, &usbDevice->devm)) 
     {
         ret = inu_usb_wait_close();
         if(ret) 
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_WARN, "libusb force close while read/write in progress (waited %u msec)\n", (USB_CLOSE_WAIT_USEC/1000)*USB_CLOSE_WAIT_RETRIES);
         }
         inu_usb_set_mode(0, INU_USB_MODE_UNPLUGGED);
         libusb_release_interface(usbDevice->devHandle, 0);
         libusb_release_interface(usbDevice->devHandle, 1);
         libusb_close(usbDevice->devHandle);
         if (!inu_usb_removeDevice())
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "close failed! did not find device in file! %d %d %d %d\n",
               usbDevice->inuUsbParams.usbDeviceAddress,usbDevice->dev_address,usbDevice->bus_num,usbDevice->port_num);
         }
         usbDevice->devHandle = NULL;
         usbDevice->dev = NULL;
         // update usb status to disconnected
         usbDevice->isUsbInserted = 0;
         set_locked(&usbDevice->closeInProg, 0, &usbDevice->devm);
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_close success\n");
      }
   }   
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
   int result = 0;
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
      rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), bufP, lenToRead, &result, USB_READ_TIMEOUT_MSEC);      
      if((rc == LIBUSB_SUCCESS) || ((rc == LIBUSB_ERROR_TIMEOUT) && (result > 0) && (result < lenToRead)))
      {
         *transferred_size += result;
         usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "libusb: read rc= %d result=%d,%d\n", rc, result, lenToRead);
      }
      else
      {
         int retries = 0;
         while (rc == LIBUSB_ERROR_PIPE && retries < 3) {
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb: Recv Pipe error - Retrying -  read rem rc=%d result=%d len=%d(%d),retries %d\n", rc, result, lenToRead, len, retries);
            /* Try clearing the pipe stall and retry the transfer
            Taken from https://github.com/openocd-org/openocd/blob/2e60e2eca9d06dcb99a4adb81ebe435a72ab0c7f/src/jtag/drivers/xds110.c#L484 */
            libusb_clear_halt(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx));
            rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), bufP, lenToRead, &result, USB_READ_TIMEOUT_MSEC);    
            retries++;
         }
         if(rc != LIBUSB_ERROR_TIMEOUT)
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb: error read rc=%d result=%d len=%d(%d)\n", rc, result, lenToRead, len);
      }
   }

   //Continue to read remainder only if read everything, otherwise return and caller will retry
   if((result == lenToRead) && (rem > 0))
   {
      //Read remainder. Avoid libusb overflow by reading up to ep size
      result = 0;
      rc =libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), remBuf->buf, USB3_BULK_EP_SIZE, &result, USB_READ_TIMEOUT_MSEC);
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
         int retries = 0;
         while (rc == LIBUSB_ERROR_PIPE && retries < 3) {
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb: Recv Pipe error - Retrying -  read rem rc=%d result=%d len=%d(%d),retries %d\n", rc, result, lenToRead, len, retries);
            /* Try clearing the pipe stall and retry the transfer
            Taken from https://github.com/openocd-org/openocd/blob/2e60e2eca9d06dcb99a4adb81ebe435a72ab0c7f/src/jtag/drivers/xds110.c#L484 */
            libusb_clear_halt(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx));
            rc =libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), remBuf->buf, USB3_BULK_EP_SIZE, &result, USB_READ_TIMEOUT_MSEC); 
            retries++;
         }
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

   set_locked(&usbDevice->rdInProg[interfaceIdx], 1, &usbDevice->devm);
   closing = get_locked(&usbDevice->closeInProg, &usbDevice->devm);
   while(!closing)
   {
      if(isBuffered)
      {
         rc = buffered_bulk_read(interfaceIdx, buffer, size, transferred_size);
      }
      else
      {
         rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, (int*)transferred_size, USB_READ_TIMEOUT_MSEC);

         int retries = 0;
         while (rc == LIBUSB_ERROR_PIPE && retries < 3) {
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"libusb ERROR PIPE  %d\n",rc);
            rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, (int*)transferred_size, USB_READ_TIMEOUT_MSEC);
            /* Try clearing the pipe stall and retry the transfer
            Taken from https://github.com/openocd-org/openocd/blob/2e60e2eca9d06dcb99a4adb81ebe435a72ab0c7f/src/jtag/drivers/xds110.c#L484 */
            libusb_clear_halt(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx));
            rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, (int*)transferred_size, USB_READ_TIMEOUT_MSEC);
            retries++;
         }
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
   set_locked(&usbDevice->rdInProg[interfaceIdx], 0, &usbDevice->devm);
   
   if(closing)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"exiting read bulk - detected closing\n");
      ret = INU_USB_ERR_IO_FAILED;
   }
   
   return ret;
}


static libusb_device *inu_usb_register_device(libusb_context *ctx, uint16_t vendor_id)
{
   struct libusb_device **devs;
   struct libusb_device *found = NULL;
   struct libusb_device *dev;   
   size_t i = 0;
   int r;

   if (libusb_get_device_list(ctx, &devs) < 0)
      return NULL;

   while ((dev = devs[i++]) != NULL) 
   {
      struct libusb_device_descriptor desc;
      r = libusb_get_device_descriptor(dev, &desc);
      if (r < 0)
         goto out;
      if (desc.idVendor == vendor_id) 
      {
         if (inu_usb_markDevice(dev))
         {
            found = dev;
            if (found)
            {
               UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
               int rc;
               usbDevice->dev = found;
               rc = libusb_open(usbDevice->dev, &(usbDevice->devHandle));
               if (rc < 0)
               {
                   usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "Could not open USB device,rc=%d\n", rc);
                   inu_usb_removeDevice();
                   found = NULL;
                   usbDevice->dev = NULL;
               }
               else if (inu_usb_boot_filter() == 0)
               {
                  usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "Device does not include in filter, close it\n");
                  libusb_close(usbDevice->devHandle);
                  usbDevice->devHandle = NULL;
                  inu_usb_removeDevice();
                  found = NULL;
                  usbDevice->dev = NULL;
               }
               else
               {
                  break;
               }
            }
         }
      }
   }

out:
   libusb_free_device_list(devs, 1);
   return found;
}

static void inu_usb_open_interprocess_mutex()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   interProcessMutex = open(LOCK_FILE_FILE_NAME,O_RDWR | O_CREAT); /* Open file to be locked, will be used as inter process mutex */
   if (interProcessMutex == -1)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"open failed %s\n", strerror(errno));
   }
}

static void inu_usb_lock_interprocess_mutex()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   if (flock(interProcessMutex, LOCK_EX) == -1) 
   {
      if (errno == EWOULDBLOCK)
         usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG,"PID %ld: already locked - bye!\n", (long) getpid());
      else
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"lock failed(PID=%ld)\n", (long) getpid());
   }
}

static void inu_usb_unlock_interprocess_mutex()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   if (flock(interProcessMutex, LOCK_UN) == -1)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"unlock failed(PID=%ld)\n",(long) getpid());
   }
}


static void *usb_event_thread_func(void *x)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   int isFirstLoop = 1;

   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"create thread %d\n",(long)pthread_self() );
   inu_usb_open_interprocess_mutex();

   inu_usb_lock_interprocess_mutex();
   inu_usb_cleanDeviceFromFile();
   inu_usb_unlock_interprocess_mutex();

   while (usbDevice->usbDetectionThreadStatus) 
   {
      inu_usb_lock_interprocess_mutex();
      inu_usb_try_close_sync();
      // poll for plugin status, used to work around the short reconnect cycle (that happens with watchdog)
      if (!usbDevice->isUsbInserted) 
      {
         if (inu_usb_open(USB_INTERFCAE_CTRL_IDX) == INU_USB_ERR_SUCCESS) 
         {
            usbDevice->isUsbInserted = 1;
            if (isKernelInsertion(usbDevice->dev))
			{
			   inu_usb_get_chip_id();
               usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "kernel reenumertion detected\n");
               inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_OS, usbDevice->bootPath, usbDevice->hpCallback);
            }
            else
            {
               inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_RAW, usbDevice->bootPath, usbDevice->hpCallback);
            }
         }
     }
     else
     {
        libusb_device *dev = inu_usb_check_device_exists(usbDevice->devCntx, INU_VENDOR_ID);
        if (dev == NULL) 
        {
           if (isFirstLoop)
           {
              inu_usb_boot_hotplug_handler(INU_USB_MODE_UNPLUGGED, usbDevice->bootPath, usbDevice->hpCallback);
              inu_usb_close(USB_INTERFCAE_CTRL_IDX);
              isFirstLoop = 0;
           }
        }
     }
     inu_usb_unlock_interprocess_mutex();
     inu_usb_sleep(usbDevice->thrSleepTimeUs);
   }

   inu_usb_lock_interprocess_mutex();
   inu_usb_close(USB_INTERFCAE_CTRL_IDX);
   inu_usb_try_close_sync();
   inu_usb_unlock_interprocess_mutex();

   return NULL;
}

static void inu_usb_create_thread()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   pthread_create(&(usbDevice->hp_usb_event_thread),NULL,usb_event_thread_func,NULL);
}

/****************************************************************************
 ***************       G L O B A L       F U N C              ***************
 ****************************************************************************/
int inu_usb_get_descriptor_bootId()
{
   return g_inuUsDeviceInfo.productInfo.bootId;
}


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

/****************************************************************************
 ***************       E X P O R T E D       F U N C          ***************
 ****************************************************************************/
 
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
   if(rc)
   {
      return INU_USB_ERR_INIT_FAILED;
   }   
   memset(usbDevice, 0, sizeof(UsbDeviceInfoT));
   memcpy(&usbDevice->inuUsbParams, inuUsbParams, sizeof(InuUsbParamsT));   
   usbDevice->hpCallback = inuUsbParams->hp_callback;
   usbDevice->thrSleepTimeUs = inuUsbParams->thrSleepTimeUs;
   if (inuUsbParams->boot_path)
   {
      strcpy(usbDevice->bootPath, inuUsbParams->boot_path);
   }

  // if (!usbDevice->inuUsbParams.log)
//   {
      usbDevice->inuUsbParams.log = inu_console_log;
//   }
   rc = libusb_init (&usbDevice->devCntx);
   if (rc < 0)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"failed to initialise libusb: %s\n", libusb_error_name(rc));
      usbDevice->devCntx = NULL; 
      return INU_USB_ERR_INIT_FAILED;
   }

   usbDevice->usbDetectionThreadStatus=1;
   inu_usb_create_thread();

   return INU_USB_ERR_SUCCESS;
}


INUUSB_API
InuUsbErrorE inu_usb_change_params(InuUsbParamsT *inuUsbParams)
{         
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;   
   
   if (inuUsbParams->boot_path) {
      strcpy(usbDevice->bootPath, inuUsbParams->boot_path);
   }
   
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_deinit()
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;

   usbDevice->usbDetectionThreadStatus=0;
   if (pthread_join(usbDevice->hp_usb_event_thread, NULL) !=0 )
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"inu_usb_deinit: %s\n",strerror(errno));
   }
   mdeinit(&usbDevice->devm);
   if (usbDevice->devCntx !=NULL)   
   {
      libusb_exit(usbDevice->devCntx);
      usbDevice->devCntx = NULL; 
   }
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
   while(get_locked(&usbDevice->closeInProg, &usbDevice->devm)) 
   {
      inu_usb_sleep(USB_CLOSE_WAIT_USEC);
   }

   if (usbDevice->devHandle == NULL && usbDevice->devCntx !=NULL)   
   {
      usbDevice->dev = inu_usb_register_device(usbDevice->devCntx, INU_VENDOR_ID);
      if (usbDevice->dev == NULL) 
      {
         //usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"No connected device found...\n");      
         return INU_USB_ERR_NO_DEVICE;
      }
      rc = libusb_open(usbDevice->dev, &(usbDevice->devHandle));
      if (LIBUSB_SUCCESS != rc) 
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Could not open USB device,rc=%d\n",rc);
         return INU_USB_ERR_INIT_FAILED;
      }
      rc = libusb_claim_interface(usbDevice->devHandle, 0);
      if (rc < 0)
      {
         /* in some cases, the claim for interface 0 (ctrl) fails, even if we passed the open 
            which is inside inu_usb_register_device(). close and return error. upper loop will
            retry */
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Error claiming interface,rc=%d\n",rc);
         libusb_close(usbDevice->devHandle);
         usbDevice->devHandle = NULL;
         inu_usb_removeDevice();
         usbDevice->dev = NULL;
         return INU_USB_ERR_NO_DEVICE;		 
      }
      rc = libusb_claim_interface(usbDevice->devHandle, 1);
      if (rc < 0)
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"Error claiming interface,rc=%d\n",rc);
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
InuUsbErrorE inu_usb_write_bulk(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{   
   int rc;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   if (!usbDevice->isUsbInserted)
   {
      return INU_USB_ERR_DISCONNECTED;
   }

   set_locked(&usbDevice->wrInProg[interfaceIdx], 1, &usbDevice->devm);
   
   rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_OUT_EP+interfaceIdx), buffer, size, (int*)transfered_size,0);

	set_locked(&usbDevice->wrInProg[interfaceIdx], 0, &usbDevice->devm);

    if(rc == 0 && *transfered_size == size)
    {
      return INU_USB_ERR_SUCCESS;   
    }
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
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API 
InuUsbErrorE inu_usb_init_interrupt_async(unsigned char interfaceIdx, unsigned int transfer_size, unsigned int num_of_transfers, InuIntCallback intCallback)
{
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API 
InuUsbErrorE inu_usb_deinit_interrupt_async(unsigned char interfaceIdx)
{
   return INU_USB_ERR_SUCCESS;
}

INUUSB_API
InuUsbErrorE inu_usb_hotplug_callback_external(InuUsbRawStatusE usbEvent, int usbFd)
{
   return INU_USB_ERR_SUCCESS;
}

#ifdef __cplusplus
}
#endif
