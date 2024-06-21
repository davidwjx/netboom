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
#define _WINSOCKAPI_

#include "inu_usb.h"
#include "inu_boot.h"
#include "libusb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

#define REGISTERED_DEVICE_FILE_NAME      "RegisteredDevices.txt"
#define REGISTERED_DEVICE_TEMP_FILE_NAME "RegisteredDevicesTemp.txt"

#define DESTROY_CNTX_KERNEL_REENUM_WA
#define MOVE_OPEN_INSIDE_REGISTER

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef HANDLE mutexT;

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
   void*                               hp_usb_event_thread;
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
char * MName = "interProcessMutex";
UsbDeviceInfoT g_inuUsDeviceInfo = {0};

static int isKernelInsertion(libusb_device *dev);
char register_device_file_name[MAX_PATH + 20]="";
char register_device_file_tmp_name[MAX_PATH + 20]="";

/****************************************************************************
 ***************       L O C A L         F U N C              ***************
 ****************************************************************************/

static int minit(mutexT *mutexP)
{
   int ret = 0;
   *mutexP = CreateMutex(NULL, FALSE, NULL);
   return ret;
}

static void mlock(mutexT *mutexP)
{
   (WaitForSingleObject(*mutexP, INFINITE) == WAIT_OBJECT_0) ? 0 : 1;
}

static void munlock(mutexT *mutexP)
{
   ReleaseMutex(*mutexP);
}

static void mdeinit(mutexT *mutexP)
{
   CloseHandle(*mutexP);
}

static void inu_usb_sleep(int usec)
{
	Sleep(usec/1000);
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
	int msec = GetTickCount();
	return msec / 1000;
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

   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() Serial = %s, Model = %s, bootId = %d\n", _FILE_, __func__,
       usbProductInfo->serialNumber,usbProductInfo->modelNumber,usbProductInfo->bootId);
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
		//usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() token = %s \n", _FILE_, __func__, token);
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
		usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() chip_id_str = %s chip_id_val = %x\n", _FILE_, __func__, 
            chip_id_str, chip_id);
	}
}

/*
   The function will check if the device meets the filter.
   returns 0 if not, 1 if devices meets the filter
*/
static int inu_usb_bus_filter()
{
    UsbDeviceInfoT* usbDevice = &g_inuUsDeviceInfo;

    int mFilterId = usbDevice->inuUsbParams.filterId;
    int busNum = usbDevice->bus_num;
    int portNum = usbDevice->port_num;

    if ( (mFilterId == busNum * 1000 + portNum) || (mFilterId == -1))
        return 1;

    return 0;
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
   int usb_id,file_dev_address,file_bus_num,file_port_num,time,current_time,found = -1;
   FILE *fp,*tempFp;

   fp     = fopen(register_device_file_name, "r+");
   if (fp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() open failed file %s(%s)\n", _FILE_, __func__, 
           register_device_file_name, strerror(errno));
       return -1;
   }

   tempFp = fopen(register_device_file_tmp_name, "w+");
   if (tempFp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() open failed file %s(%s)\n", _FILE_, __func__, 
           register_device_file_tmp_name, strerror(errno));
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
         usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG,"%s, %s() found, clean line - %d %d %d %d time: %d\n", _FILE_, 
             __func__, usb_id,file_dev_address,file_bus_num, file_port_num,time);
      }
   }

   fclose(fp);
   fclose(tempFp);
   
   remove(register_device_file_name);
   rename(register_device_file_tmp_name, register_device_file_name);

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

   fp     = fopen(register_device_file_name, "r");
   if (fp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() open failed file %s(%s)\n", _FILE_, __func__, 
           register_device_file_name, strerror(errno));
       return 0;
   }

   tempFp = fopen(register_device_file_tmp_name, "w+");
   if (tempFp == NULL)
   {
       usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() open failed file %s(%s)\n", _FILE_, __func__, 
           register_device_file_tmp_name, strerror(errno));
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
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() found, removing line - %d %d %d %d time: %d\n", _FILE_,
             __func__, file_process_id,file_dev_address,file_bus_num, file_port_num,time);
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

   remove(register_device_file_name);
   rename(register_device_file_tmp_name, register_device_file_name);

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

   fp=fopen(register_device_file_name, "r+");

   if (fp == NULL)
   {
       //if file does not exist, create one
       fp=fopen(register_device_file_name, "w+");
       if (fp == NULL)
       {
          usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() open and create failed for file %s(%s)\n", _FILE_, 
              __func__, register_device_file_name, strerror(errno));
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
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() old device %d %d %d registered, claiming it for this process\n",
                _FILE_, __func__, dev_address,bus_num, port_num);
         }
         else
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG,"%s, %s() device %d %d %d already registered\n", _FILE_, 
                __func__, dev_address,bus_num, port_num);
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

   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() device %d %d %d time: %d registered\n", _FILE_, __func__,
       dev_address, bus_num, port_num,time);

   return 1;
}

void inu_usb_list(UsbDeviceInfoT* usbDevice, libusb_context* ctx)
{
    libusb_context* ctx_c = NULL;
    int ret;
    struct libusb_device** devs;
    struct libusb_device* dev;
    struct libusb_device_descriptor desc;
    int i = 0;

    if (ctx)
        ctx_c = ctx;
    else
        ret = libusb_init(&ctx_c);
    if (!ctx_c && usbDevice) {
        usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb_init error %d\n", ret);
        return;
    }

    if (ret = libusb_get_device_list(ctx_c, &devs) < 0) {
        if (usbDevice)
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb_get_device_list error %d\n", ret);
        return;
    }

    while ((dev = devs[i++]) != NULL) {
        ret = libusb_get_device_descriptor(dev, &desc);
        if (ret < 0) {
            if (usbDevice)
                usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "libusb_get_device_descriptor error %d\n", ret);
            continue;
        }
        if (usbDevice && INU_VENDOR_ID == desc.idVendor)
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "-- VID: %d(0x%x) PID: %d(0x%x) addre: %d bus: %d port: %d bcd: 0x%x\n", desc.idVendor, desc.idVendor, desc.idProduct, desc.idProduct, 
                libusb_get_device_address(dev), libusb_get_bus_number(dev), libusb_get_port_number(dev), desc.bcdDevice);
    }
    libusb_free_device_list(devs, 1);
    if (!ctx) {
        libusb_exit(ctx_c);
    }
}

/*
The function will search the current device list if the device we are working with still exists in it.
If exists, it will update the time
*/
static libusb_device* inu_usb_check_device_exists(libusb_context* ctx, uint16_t vendor_id)
{
    UsbDeviceInfoT* usbDevice = &g_inuUsDeviceInfo;
    struct libusb_device** devs;
    struct libusb_device* found = NULL;
    struct libusb_device* dev;
    int dev_address;
    int bus_num;
    int port_num;
    int file_dev_address, file_bus_num, file_port_num, time, usb_id;
    FILE* fp, * tempFp;
    size_t i = 0;
    int r;
    libusb_context* new_ctx = NULL;
    if (ctx)
        new_ctx = ctx;
    else {
        libusb_init(&new_ctx);
    }

   if (libusb_get_device_list(new_ctx, &devs) < 0)
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
         if (((desc.bcdDevice & KERNEL_BCD_DEVICE_MASK) == 0x0) && (!isKernelInsertion(usbDevice->dev)))
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() kernel reenumertion detected (without device disconnection)\n", 
                _FILE_, __func__);
            dev = NULL;
         }
         found = dev;
         break;
      }
   }

   if (found == NULL)
      goto out;

   fp = fopen(register_device_file_name, "r");
   if (fp == NULL)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() open failed file %s(%s)\n", _FILE_, __func__, 
          register_device_file_name, strerror(errno));
      if (!ctx) {
          libusb_exit(new_ctx);
      }
      return NULL;
   }

   tempFp = fopen(register_device_file_tmp_name, "w+");
   if (tempFp == NULL)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() open failed file %s(%s)\n", _FILE_, __func__, 
          register_device_file_tmp_name, strerror(errno));
      fclose(fp);
      if (!ctx) {
          libusb_exit(new_ctx);
      }
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

   remove(register_device_file_name);
   rename(register_device_file_tmp_name, register_device_file_name);


out:
   libusb_free_device_list(devs, 1);
   if (!ctx) {
       libusb_exit(new_ctx);
   }
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
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() can't get device descriptor rc=%d\n", _FILE_, 
             __func__, rc);
         return 0;
      }
      else 
      {
         if ((desc.bcdDevice & KERNEL_BCD_DEVICE_MASK) == 0x0) 
         {
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
            usbDevice->inuUsbParams.log(INU_USB_LOG_WARN, "%s, %s() libusb force close while read/write in progress (waited %u msec)\n", 
                _FILE_, __func__, (USB_CLOSE_WAIT_USEC/1000)*USB_CLOSE_WAIT_RETRIES);
         }
         inu_usb_set_mode(0, INU_USB_MODE_UNPLUGGED);
         libusb_release_interface(usbDevice->devHandle, 0);
         libusb_release_interface(usbDevice->devHandle, 1);
         libusb_release_interface(usbDevice->devHandle, 2);
         libusb_release_interface(usbDevice->devHandle, 3);
         libusb_release_interface(usbDevice->devHandle, 4);
         libusb_release_interface(usbDevice->devHandle, 5);

         //libusb_reset_device(usbDevice->devHandle);

         libusb_close(usbDevice->devHandle);
         if (!inu_usb_removeDevice())
         {
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() close failed! did not find device in file! %d %d %d %d\n",
                _FILE_, __func__, usbDevice->inuUsbParams.usbDeviceAddress, usbDevice->dev_address, 
                usbDevice->bus_num, usbDevice->port_num);
         }
#ifdef DESTROY_CNTX_KERNEL_REENUM_WA
         /*  On windows the libusb does not "see" the new device descriptor after the linux enumeration.
             only if we restart the context, then the descriptor is updated. we need this to be able to detect
             kernel enumerations
         */
         if (usbDevice->devCntx != NULL)
         {
            libusb_exit(usbDevice->devCntx);
            usbDevice->devCntx = NULL;
         }
#endif
         usbDevice->devHandle = NULL;
         usbDevice->dev = NULL;
         // update usb status to disconnected
         usbDevice->isUsbInserted = 0;
         set_locked(&usbDevice->closeInProg, 0, &usbDevice->devm);
         usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() inu_usb_close success\n", _FILE_, __func__);
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
static int buffered_bulk_read(unsigned char interfaceIdx, unsigned char *bufP, unsigned int len, int *transferred_size)
{
   int rc = LIBUSB_SUCCESS;
   int result = 0;
   int rem;
   int lenToRead;
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   bulkEpBufT *remBuf = &usbDevice->buf[interfaceIdx];

   *transferred_size = 0;

   usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "%s, %s() libusb: begin buffered read %d \n", _FILE_, __func__, len);

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
         usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "%s, %s() libusb: read rc= %d result=%d,%d\n", _FILE_, __func__, 
             rc, result, lenToRead);
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
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() libusb: error read rc=%d result=%d len=%d(%d)\n", 
                _FILE_, __func__, rc, result, lenToRead, len);
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
            usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "%s, %s() libusb: read rem rc= %d result=%d,%d rem=%d@%d\n", 
                _FILE_, __func__, rc, result, lenToRead,result-rem,rem);
            update_ep_buf(remBuf, (result-rem), rem);
         }
         else
            usbDevice->inuUsbParams.log(INU_USB_LOG_DEBUG, "%s, %s() libusb: read rem rc= %d result=%d,%d rem=%d\n", 
                _FILE_, __func__, rc, result, lenToRead, rem);
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
            usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() libusb: error read rem rc=%d result=%d len=%d(%d)\n", 
                _FILE_, __func__, rc, result, lenToRead, len);
      }
   }

   return rc;
}

static InuUsbErrorE usb_read_bulk(unsigned char interfaceIdx, int isBuffered, unsigned char *buffer, unsigned int size, int *transferred_size)
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
         rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, transferred_size, USB_READ_TIMEOUT_MSEC);
         int retries = 0;
         while (rc == LIBUSB_ERROR_PIPE && retries < 3) {
            usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"libusb ERROR PIPE  %d\n",rc);
            rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, (int*)transferred_size, USB_READ_TIMEOUT_MSEC);
            /* Try clearing the pipe stall and retry the transfer
            Taken from https://github.com/openocd-org/openocd/blob/2e60e2eca9d06dcb99a4adb81ebe435a72ab0c7f/src/jtag/drivers/xds110.c#L484 */
            libusb_clear_halt(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx));
            rc = libusb_bulk_transfer(usbDevice->devHandle, (CTL_IN_EP+interfaceIdx), buffer, size, transferred_size, USB_READ_TIMEOUT_MSEC);; 
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
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() libusb read error %d\n", _FILE_, __func__, rc);
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
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() exiting read bulk - detected closing\n", _FILE_, __func__);
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
                   usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() Could not open USB device,rc=%d\n", 
                       _FILE_, __func__, rc);
                   inu_usb_removeDevice();
                   found = NULL;
                   usbDevice->dev = NULL;
               }
               else if ((inu_usb_boot_filter() == 0)|| (inu_usb_bus_filter() == 0))
               {
                  usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() Device does not include in boot filter, close it\n", 
                      _FILE_, __func__);
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
#ifdef MOVE_OPEN_INSIDE_REGISTER
   /*On windows system, if freeing the device list before open we get an error from the libusb.
     on linux this does not happen. The libusb_get_device_list specifies not to free the device before 
     opening it, TODO: put the libusb_free_device_list after opening the device
   */
   libusb_free_device_list(devs, 1);
#endif
   return found;
}

static void inu_usb_open_interprocess_mutex()
{
   HANDLE hMutex;

   hMutex = CreateMutex(
      NULL,
      FALSE,
      MName);

   if (hMutex == NULL)
      printf("CreateMutex(): error: %d.\n", GetLastError());
   else
   {
      if (GetLastError() == ERROR_ALREADY_EXISTS)
         printf("CreateMutex(): opened existing %s mutex.\n", MName);
      else
         printf("CreateMutex(): new %s mutex successfully created.\n", MName);
	}
}

static int inu_usb_lock_interprocess_mutex()
{
   HANDLE hMutex1;
   BOOL retCode;

   hMutex1 = OpenMutex(
      MUTEX_ALL_ACCESS, // request full access
      FALSE,                              // handle not inheritable
      MName);                            // object name

   if (hMutex1 == NULL)
   {
       printf("OpenMutex(): error: %d.\n", GetLastError());
       return 1;
   }
   retCode = (WaitForSingleObject(hMutex1, INFINITE) == WAIT_OBJECT_0) ? 0 : 1;
   CloseHandle(hMutex1);

   return  retCode;
}

static int inu_usb_unlock_interprocess_mutex()
{
    HANDLE hMutex1;
    BOOL retCode;

    hMutex1 = OpenMutex(
        MUTEX_ALL_ACCESS, // request full access
        FALSE,                              // handle not inheritable
        MName);                            // object name

    if (hMutex1 == NULL)
    {
       printf("OpenMutex(): error: %d.\n", GetLastError());
       return 1;
    }
    retCode = (ReleaseMutex(hMutex1));
    CloseHandle(hMutex1);
    return retCode? 0 : 1;
}


static void *usb_event_thread_func(void *x)
{
   UsbDeviceInfoT *usbDevice = &g_inuUsDeviceInfo;
   UINT8 isFirstLoop = 1;

   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() create thread %p\n", _FILE_, __func__, GetCurrentThread() );
   inu_usb_open_interprocess_mutex();

   inu_usb_lock_interprocess_mutex();
   inu_usb_cleanDeviceFromFile();
   inu_usb_unlock_interprocess_mutex();

   while (usbDevice->usbDetectionThreadStatus) 
   {
      inu_usb_lock_interprocess_mutex();
      inu_usb_try_close_sync();
      // poll for plugin status, used to work around the short reconnect cycle (that happens with watchdog)
      //usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "--> isUsbInserted: %d\n", usbDevice->isUsbInserted);
      if (!usbDevice->isUsbInserted) 
      {
#ifdef DESTROY_CNTX_KERNEL_REENUM_WA
         /*  On windows the libusb does not "see" the new device descriptor after the enumeration.
             only if we restart the context, then the descriptor is updated. we need this to be able to detect
             kernel enumerations
         */
         if (usbDevice->devCntx == NULL)
         {
            int rc;
            rc = libusb_init(&usbDevice->devCntx);
            if (rc < 0)
            {
               usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() failed to initialise libusb: %s\n", _FILE_, 
                   __func__, libusb_error_name(rc));
               usbDevice->devCntx = NULL;
            }
         }
#endif
         if (inu_usb_open(USB_INTERFCAE_CTRL_IDX) == INU_USB_ERR_SUCCESS) 
         {
            usbDevice->isUsbInserted = 1;
            if (isKernelInsertion(usbDevice->dev))
            {
               inu_usb_get_chip_id();
               usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "%s, %s() kernel reenumertion detected\n", _FILE_, __func__);
               inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_OS, usbDevice->bootPath, usbDevice->hpCallback);
            }
            else
            {
                isFirstLoop = 1;
                if (strlen(usbDevice->bootPath) == 0)
                   inu_usb_boot_hotplug_handler(INU_USB_MODE_DETECT_CHIP_VERSION, usbDevice->bootPath, usbDevice->hpCallback);
                else
                   inu_usb_boot_hotplug_handler(INU_USB_MODE_PLUGGED_RAW, usbDevice->bootPath, usbDevice->hpCallback);

            }
         }
     }
     else
     {
        libusb_device *dev = inu_usb_check_device_exists(NULL, INU_VENDOR_ID);
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
   DWORD threadID;
   usbDevice->hp_usb_event_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)usb_event_thread_func, NULL, 0, &threadID);
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
   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() inu_usb_set_mode: mode=%d\n", _FILE_, __func__, mode);

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
   DWORD dwRetVal = 0;
   char lpTempPathBuffer[MAX_PATH];




   dwRetVal = GetTempPath(MAX_PATH,          // length of the buffer
	   lpTempPathBuffer); // buffer for path
   if (dwRetVal > MAX_PATH || (dwRetVal == 0))
   {
	   usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "%s, %s() GetTempPath failed\n", _FILE_, __func__);
	   return (INU_USB_ERR_INIT_FAILED);
   }
   else
   {
	   sprintf(register_device_file_name, "%s%s" ,lpTempPathBuffer,REGISTERED_DEVICE_FILE_NAME);
	   sprintf(register_device_file_tmp_name, "%s%s", lpTempPathBuffer, REGISTERED_DEVICE_TEMP_FILE_NAME);
   }

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

   if (!usbDevice->inuUsbParams.log)
   {
      usbDevice->inuUsbParams.log = inu_console_log;
   }
   rc = libusb_init (&usbDevice->devCntx);
   if (rc < 0)
   {
      usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() failed to initialise libusb: %s\n", _FILE_, __func__, 
          libusb_error_name(rc));
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
   WaitForSingleObject(usbDevice->hp_usb_event_thread, INFINITE);
   mdeinit(&usbDevice->devm);
   if (usbDevice->devCntx !=NULL)   
   {
      libusb_exit(usbDevice->devCntx);
      usbDevice->devCntx = NULL; 
   }
   usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() HotK monitor closed successfully!\n", _FILE_, __func__);
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
         //usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() No connected device found...\n", _FILE_, __func__);
         return INU_USB_ERR_NO_DEVICE;
      }
#ifndef MOVE_OPEN_INSIDE_REGISTER
      rc = libusb_open(usbDevice->dev, &(usbDevice->devHandle));
      if (LIBUSB_SUCCESS != rc) 
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() Could not open USB device,rc=%d\n", _FILE_, __func__, rc);
         return INU_USB_ERR_INIT_FAILED;
      }
#endif
      rc = libusb_claim_interface(usbDevice->devHandle, 0);
      if (rc < 0)
      {
         /* in some cases, the claim for interface 0 (ctrl) fails, even if we passed the open 
            which is inside inu_usb_register_device(). close and return error. upper loop will
            retry */
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() Error claiming interface %d,rc=%d\n", _FILE_, __func__, 0, rc);
         libusb_close(usbDevice->devHandle);
         usbDevice->devHandle = NULL;
         inu_usb_removeDevice();
         usbDevice->dev = NULL;
         return INU_USB_ERR_NO_DEVICE;
      }
      rc = libusb_claim_interface(usbDevice->devHandle, 1);
      if (rc < 0)
      {
         usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR,"%s, %s() Error claiming interface,rc=%d\n", _FILE_, __func__, rc);
      }
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"inu_usb_open: success\n");
      /*HELSINKI - On Windows we need to claim the new endpoints*/
      rc = libusb_claim_interface(usbDevice->devHandle, 2);
      if (rc < 0)
      {
          usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "Error claiming interface,rc=%d\n", rc);
      }
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_open: success\n");

      rc = libusb_claim_interface(usbDevice->devHandle, 3);
      if (rc < 0)
      {
          usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "Error claiming interface,rc=%d\n", rc);
      }
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_open: success\n");

      rc = libusb_claim_interface(usbDevice->devHandle, 4);
      if (rc < 0)
      {
          usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "Error claiming interface,rc=%d\n", rc);
      }
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_open: success\n");

      rc = libusb_claim_interface(usbDevice->devHandle, 5);
      if (rc < 0)
      {
          usbDevice->inuUsbParams.log(INU_USB_LOG_ERROR, "Error claiming interface,rc=%d\n", rc);
      }
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO, "inu_usb_open: success\n");
   
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
      usbDevice->inuUsbParams.log(INU_USB_LOG_INFO,"%s, %s() inu_usb_close begin\n", _FILE_, __func__);
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
   return usb_read_bulk(interfaceIdx, 0, buffer, size, (int*)transfered_size);
}

INUUSB_API 
InuUsbErrorE inu_usb_buffered_read_bulk(unsigned char interfaceIdx,unsigned char *buffer, unsigned int size, unsigned int *transferred_size)
{
   return usb_read_bulk(interfaceIdx, 1, buffer, size, (int*)transferred_size);
}

INUUSB_API 
InuUsbErrorE inu_usb_read_interrupt(unsigned char interfaceIdx, unsigned char *buffer, unsigned int size, unsigned int *transfered_size)
{
   return INU_USB_ERR_SUCCESS;
}
static void cb_interrupt(struct libusb_transfer *xfr)
{
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
