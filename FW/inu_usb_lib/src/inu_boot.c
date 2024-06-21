// inu_boot_lib.cpp : Defines the exported functions for the DLL application.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include "inu_host_utils.h"

#include "inu_boot_common.h"
#include "inu_bootdetect.h"
#define INUUSB_EXPORTS
#include "inu_usb.h"
#include "inu_boot.h"


#define VERSION_STR   "1.02"

//#define NU3K_TEST_VERBOSE

#define SPL_COMPLETE_SLEEP_MSEC (20)
#define SPL_COMPLETE_SLEEP_CNTMAX (100)
#define CHIP_VERSION_TO_STRING(chipRevId) ((chipRevId == NU4000_A0_CHIP_VERSION) ? "NU4000" :(chipRevId == NU4000_B0_CHIP_VERSION) ? "NU4000B0": (chipRevId == NU4000_C0_CHIP_VERSION) ? "NU4000C0" : (chipRevId == NU3000_B0_CHIP_VERSION) ? "NU3000" : (chipRevId == NU4000_C1_CHIP_VERSION) ? "NU4000C1" : (chipRevId == NU4100_CHIP_VERSION) ? "NU4100" : "")

#define INU_DEFSG_BOOT_FACTOR (10000)
#define BOOT_FOLDER_GRANULARITY 100
#define PARSE_BOOT_FOLDER(bootId) (((bootId % INU_DEFSG_BOOT_FACTOR) / BOOT_FOLDER_GRANULARITY) * BOOT_FOLDER_GRANULARITY)

//#define NU3K_BOOT_READBACK_CHECK
//little endian to local conversion
#define U32_LE2LOC(buf) ((unsigned int)(buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24)))
//local to little endian converstion
#define U32_LOC2LE(val) ((val & 0xFF) | (val & 0xFF00) | (val & 0xFFFF00) | (val & 0xFFFFFF00))

//forward declarations
InuBootsplInfoHeaderT bootsplInfo;
static int reset_in_progress = 0;


int g_hotPlugCount=0;


//wchar_t  utf16fileNameForMicrosoft[1024];   // an array of wchar_t is UTF-16 in Microsoft land
//MultiByteToWideChar( CP_UTF8 , 0 , (LPCCH)filename, -1, utf16fileNameForMicrosoft , strlen( );

int inu_usb_boot_is_reset_in_progress()
{
   return reset_in_progress;
}

void inu_usb_boot_set_reset_in_progress(int reset_flag)
{
   reset_in_progress = reset_flag;
}


char log_message[1024];
void inu_console_log(InuUsbLogLevelE level, const char * fmt, ...)
{
   va_list  args;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
   if (level <= inuUsbParams->logLevel)
   {
      va_start(args,fmt);
      // variable args string
      vsprintf(log_message,fmt, args);

      printf("%s",log_message);
      va_end(args);
   }
}

static unsigned char *load_file(char *file_name, unsigned int *file_size)
{
   size_t read_size;
   int size;
   unsigned char *pBuffer = NULL;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

#ifdef UNICODE
   std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
   std::string utf8Name(file_name);
   FILE* fw = _wfopen(convert.from_bytes(utf8Name).c_str(), L"rb");
#else
   FILE* fw = fopen(file_name, "rb");
#endif

   if (fw) {
      // check file size
      fseek(fw, 0, SEEK_END);
      size = ftell(fw);
      fseek(fw, 0, SEEK_SET);

      inuUsbParams->log(INU_USB_LOG_INFO,"%s, %s() %s size is: %d\n", _FILE_, __func__, file_name, size);
      pBuffer = (unsigned char *) malloc(size);
      if (pBuffer) {
         read_size = fread(pBuffer, sizeof(char), size, fw);
         if (read_size == 0) {
            inuUsbParams->log(INU_USB_LOG_ERROR,"%s, %s() ERROR: read file %s error or eof\n", _FILE_, __func__, 
                file_name);
            free(pBuffer);
            pBuffer = NULL;
         }else {
            *file_size = size;
         }
      }else {
         inuUsbParams->log(INU_USB_LOG_ERROR,"%s, %s() ERROR: failed to malloc buffer\n", _FILE_, __func__);
      }
      fclose(fw);
   }else {
      inuUsbParams->log(INU_USB_LOG_ERROR,"%s, %s() ERROR: Failed to open file %s\n", _FILE_, __func__, file_name);
   }

   return pBuffer;
}

static void convertBootInfo(unsigned int *buffer, unsigned int bufferSize, unsigned int *outBuf)
{
   unsigned int i;
   unsigned int val;
   unsigned char *valP =(unsigned char *)&val;

   for(i = 0; i < bufferSize/sizeof(unsigned int); i++)
   {
      memcpy(&val, &buffer[i], 4);
      outBuf[i]  = U32_LE2LOC(valP);
   }
}


static InuUsbErrorE waitForSpl(int sleepRetry)
{
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
   unsigned int command, address, val;
   int sleepCnt = 0;
   unsigned int transfered_size;
   InuUsbErrorE splComplete = INU_USB_ERR_BOOT_FAILURE;
   InuBootsplInfoHeaderT tmpBootsplInfo;
   InuUsbErrorE err;

   while(sleepCnt++ < sleepRetry)
   {
      //Read value to verify spl is done and get jump address
      if(inu_usb_monitor_read(BOOTSPL_CMD_REG, (unsigned char *)&val, sizeof(val), &transfered_size) != INU_USB_ERR_SUCCESS)
         break;
      convertBootInfo(&val, sizeof(val), &command);

      if(command == BOOTSPL_CMD_SPL_INIT_DONE)
      {
      if (inu_usb_monitor_read(BOOTSPL_ADDRESS_REG, (unsigned char *)&val, sizeof(val), &transfered_size) != INU_USB_ERR_SUCCESS)
         break;
      convertBootInfo(&val, sizeof(val), &address);
      if (inu_usb_monitor_read(address, (unsigned char *)&tmpBootsplInfo, sizeof(tmpBootsplInfo), &transfered_size) != INU_USB_ERR_SUCCESS)
         break;
      convertBootInfo((unsigned int *)&tmpBootsplInfo, sizeof(tmpBootsplInfo), (unsigned int *)&bootsplInfo);
         splComplete = INU_USB_ERR_SUCCESS;
         break;
      }
      host_millisleep(SPL_COMPLETE_SLEEP_MSEC);
      //dump_buf((unsigned char *)linux_boot_info,sizeof(buffer));
   }

   if(splComplete == INU_USB_ERR_SUCCESS)
   {
      inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() SPL complete( >= %d ms ): state=%x zImage=0x%08x dtb=0x%08x jump=0x%08x app=0x%08x initram=0x%08x cnn=0x%08x\n",
          _FILE_, __func__, (sleepCnt-1)*SPL_COMPLETE_SLEEP_MSEC, command, bootsplInfo.kernelStartAddress, bootsplInfo.dtbStartAddress,
        bootsplInfo.invokeLinuxBootAddress, bootsplInfo.appStartAddress, bootsplInfo.initramfsStartAddress, bootsplInfo.cnnStartAddress);
     // Current linux kerenl ramdisk assumes nu3000.zip location at GME5(BOOTSPL_CMD_REG)
     val = U32_LOC2LE(bootsplInfo.appStartAddress);
     err = inu_usb_monitor_write(BOOTSPL_CMD_REG, (unsigned char *)&val, 4);
     if (err != INU_USB_ERR_SUCCESS) {
        inuUsbParams->log(INU_USB_LOG_ERROR, "%s, %s() inu_usb_boot: can't write to chip...\n", _FILE_, __func__);
        return err;
     }
   }
   else
   {
      memset(&bootsplInfo, 0, sizeof(bootsplInfo));
      inuUsbParams->log(INU_USB_LOG_ERROR,"%s, %s() Failed read or target timeout on SPL complete(%d tries X %d msec)\n", 
          _FILE_, __func__, sleepCnt-1, SPL_COMPLETE_SLEEP_MSEC);
   }

   return splComplete;
}

static InuUsbErrorE inu_usb_get_chip_version()
{
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
   unsigned int size;
   InuUsbErrorE err;

   err = inu_usb_monitor_read(GME_CHIP_VERSION, (unsigned char *)&(inuUsbParams->version), 4, &size);
   if (err != INU_USB_ERR_SUCCESS) {
      inuUsbParams->log(INU_USB_LOG_ERROR, "%s, %s() inu_usb: can't read version from the chip, check USB connection...\n",
          _FILE_, __func__);
      return err;
   }

   inuUsbParams->log(INU_USB_LOG_ERROR, "%s, %s() inu_usb: %s detected\n", _FILE_, __func__, 
       CHIP_VERSION_TO_STRING(inuUsbParams->version));

   return err;
}
unsigned char inu_boot_updateBootCfg( char *bufP, INU_BOOT_boardConfigT *bootCfg)
{
   unsigned int ind,readParam;
   unsigned char paramFound=0;
   char *readStr,str2[256];
   int ret;
   char *paramStr[INU_BOOT_MAX_PARAMS] = { INU_BOOT_CPUMHZFREQ_STR,INU_BOOT_CPUFRAQ_STR,
                                           INU_BOOT_SYSMHZFREQ_STR ,INU_BOOT_DDRHZFREQ_STR,
                                           INU_BOOT_DSPMHZFREQ_STR ,INU_BOOT_AUDIOMHZFREQ_STR};

   bootCfg->magicNum = BOOTSPLB0_C0_PARAM_MAGIC;

   for (ind = 0; ind < INU_BOOT_MAX_PARAMS;ind++)
   {
       readStr = strstr(bufP,paramStr[ind]);
       if (readStr != NULL)
       {
          ret = sscanf(readStr,"%100s %d",str2,&readParam);
         if (ret == 2)
         {
            bootCfg->param[ind] = readParam;
            paramFound = 1;
         }

       }
   }
   return paramFound;
}

static void inu_boot_updateSplDdrFilename(INU_BOOT_boardConfigT *bootCfg,char *splName)
{
   char *extPos;
   if (bootCfg->param[INU_BOOT_PARAM_DDRFREQ] != 0xffffffff)
   {
      extPos = strstr(splName,".bin");
      if (extPos != NULL)
      {
         extPos[0] = 0;
         sprintf(extPos,"_freq%d.bin",bootCfg->param[INU_BOOT_PARAM_DDRFREQ]);
      }
      printf("spl used %s\n",splName);
   }

}

static void inu_usb_printComponentsErr(ComponentsErrorE err)
{

   unsigned int compInd = ~err + 1;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();

   char *componentName[8] = { "unknown","kernel","dtb","app","bootspl","initramfs","cnn","cnnLoader" };
   if (compInd < 8)
      inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() error !!! %s file doesn't exist\n", _FILE_, __func__, 
          componentName[compInd]);
}

static InuUsbErrorE inu_usb_boot_target(char *boot_path)
{
    unsigned char* bufP = NULL;
    unsigned int size, lramLoadAdd, securedOffset = 0, transferSize;
    int id, splSleepRetry;
    InuUsbErrorE err;
    InuUsbParamsT* inuUsbParams = inu_usb_get_contex();
    ComponentsParamsT componentsParams;
    ComponentsErrorE componentErr;
    unsigned int  testC0RomData, testC0RomAddr = 0x3de0; // C0 ROM data in this address

   err = inu_usb_get_chip_version();
   if (err != INU_USB_ERR_SUCCESS)
   {
      return err;
   }

   if ((inuUsbParams->version == NU4000_C0_CHIP_VERSION) || (inuUsbParams->version == NU4000_B0_CHIP_VERSION))
   {
      err = inu_usb_monitor_read(testC0RomAddr, (unsigned char *)&testC0RomData, sizeof(unsigned int), &transferSize);
      if (err != INU_USB_ERR_SUCCESS)
      {
         return err;
      }
      else
      {
         if (testC0RomData == 0xe24dd00c)            // bug in C0 have B0 version name
            inuUsbParams->version = NU4000_C0_CHIP_VERSION;
         else if (testC0RomData == 0xe59d3004)  // could be C1 version
            inuUsbParams->version = NU4000_C1_CHIP_VERSION;
      }

   }
   if (inuUsbParams->bootId < 0)
   {
      //bootId was not specified, check if USB descriptor has it
      if (inu_usb_get_descriptor_bootId())
      {
         id = inu_usb_get_descriptor_bootId();
         inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() inu_usb_boot: using bootId from usb descriptor. bootId=%d\n", 
             _FILE_, __func__, id);
      }
      else
      {
         inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() inu_usb_boot: performing automatic board detection...\n", 
             _FILE_, __func__);
         // run bootdetect on target
         inu_bootdetect_init();
         id = inu_bootdetect_get_bootid();
         inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() inu_usb_boot: detected bootId=%d\n", _FILE_, __func__, id);
      }
   }
   else
   {
      id = inuUsbParams->bootId;
      inuUsbParams->log(INU_USB_LOG_ERROR, "%s, %s() inu_usb_boot: bootId override by InuServiceParams.xml, bootId=%d\n", 
          _FILE_, __func__, id);
   }

   // get file names according to bootId
   componentErr = get_components(&componentsParams, id, boot_path, inuUsbParams->version);
   if (componentErr < 0)
   {
      inu_usb_printComponentsErr(componentErr);
      return INU_USB_ERR_FILE_ACCESS;
   }

   do
   {
      // do not load bootspl for bootfix
      //SPL load and run
      INU_BOOT_boardConfigT bootCfg;

      if ((bufP = load_file(componentsParams.config_file_name, &size)) != NULL)
      {
       memset(&bootCfg, 0xff, sizeof(bootCfg));
      if (inu_boot_updateBootCfg((char *)bufP, &bootCfg) != 0)
      {
         inu_boot_updateSplDdrFilename(&bootCfg,componentsParams.spl_file_name);
         err = inu_usb_monitor_write(BOOTSPLB0_C0_PARAM_ADDRESS, (unsigned char *)&bootCfg, sizeof(INU_BOOT_boardConfigT));
            if (err != INU_USB_ERR_SUCCESS)
            inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() error!!!! write\n", _FILE_, __func__);
      }
        free(bufP);
      }


      if ((bufP = load_file(componentsParams.spl_file_name, &size)) == NULL) {
         err = INU_USB_ERR_FILE_ACCESS;
         break;
      }

     if ((inuUsbParams->version == NU4000_B0_CHIP_VERSION) ||
         (inuUsbParams->version == NU4000_C0_CHIP_VERSION) || (inuUsbParams->version == NU4000_C1_CHIP_VERSION)
         || (inuUsbParams->version == NU4100_CHIP_VERSION))
     {
        lramLoadAdd = BOOTSPLB0_C0_LOAD_ADDRESS;
        if (inuUsbParams->version != NU4000_B0_CHIP_VERSION)
        {
          err = inu_usb_monitor_read(GME_SAVE_AND_RESTORE7_ADDR, (unsigned char *)&securedOffset, sizeof(unsigned int), &transferSize);
          if (err != INU_USB_ERR_SUCCESS)
            inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() error!!!!\n", _FILE_, __func__);
          if (securedOffset != 0)
          {
            lramLoadAdd = lramLoadAdd + securedOffset;
            err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE6_ADDR, (unsigned char *)&lramLoadAdd, sizeof(unsigned int));
          }
          if (err != INU_USB_ERR_SUCCESS)
            inuUsbParams->log(INU_USB_LOG_ERROR, "%s, %s() inu_storage: Fail writng ispiDividor to USB...\n", 
                _FILE_, __func__);
        }

        err = inu_usb_monitor_write(lramLoadAdd, bufP, size);
        free(bufP);
        if (err != INU_USB_ERR_SUCCESS)
           break;

        err = inu_usb_monitor_jump(BOOTSPLB0_C0_LOAD_ADDRESS);
        if (err != INU_USB_ERR_SUCCESS)
           break;
      }
      else
      {
         err = inu_usb_monitor_write(BOOTSPL_LOAD_ADDRESS, bufP, size);
         free(bufP);
         if (err != INU_USB_ERR_SUCCESS)
            break;

         err = inu_usb_monitor_jump(BOOTSPL_LOAD_ADDRESS);
         if (err != INU_USB_ERR_SUCCESS)
            break;
      }

        if ( PARSE_BOOT_FOLDER(id)  == 500 )
        {
         host_millisleep(200);
            splSleepRetry = SPL_COMPLETE_SLEEP_CNTMAX * 30; //allow more time for memory test
        }
        else
        {
            splSleepRetry = SPL_COMPLETE_SLEEP_CNTMAX;
        }

      //Wait for spl completion
      err = waitForSpl(splSleepRetry);
      if(err != INU_USB_ERR_SUCCESS)
         break;

   //Load application files (optional)
   if ((bufP = load_file(componentsParams.app_file_name, &size)) == NULL) {
      unsigned int val = 0;
      err = inu_usb_monitor_write(bootsplInfo.appStartAddress, (unsigned char *)&val, sizeof(val));
   }
   else
   {
      //Write size of app file to first 4 bytes
      unsigned int val = U32_LOC2LE(size);
      err = inu_usb_monitor_write(bootsplInfo.appStartAddress, (unsigned char *)&val, sizeof(val));
      if (err == INU_USB_ERR_SUCCESS) {
         //write application file
         err = inu_usb_monitor_write(bootsplInfo.appStartAddress + sizeof(val), bufP, val);
      }
      free(bufP);
   }

      if(err != INU_USB_ERR_SUCCESS)
         break;

      //Load dtb
      if ((bufP = load_file(componentsParams.dtb_file_name, &size)) == NULL) {
         err = INU_USB_ERR_FILE_ACCESS;
         break;
      }

      err = inu_usb_monitor_write(bootsplInfo.dtbStartAddress, bufP, size);
      free(bufP);
      if(err != INU_USB_ERR_SUCCESS)
         break;

      //Load linux
      if ((bufP = load_file(componentsParams.linux_file_name, &size)) == NULL) {
         err = INU_USB_ERR_FILE_ACCESS;
         break;
      }

      err = inu_usb_monitor_write(bootsplInfo.kernelStartAddress, bufP, size);
      free(bufP);
      if(err != INU_USB_ERR_SUCCESS)
         break;

     //Load initramfs
     if ((bufP = load_file(componentsParams.initramfs_file_name, &size)) == NULL) {
        err = INU_USB_ERR_FILE_ACCESS;
        break;
     }
     unsigned int val = U32_LOC2LE(size);
     err = inu_usb_monitor_write(bootsplInfo.initramfsStartAddress, (unsigned char *)&val, sizeof(val));
     if (err == INU_USB_ERR_SUCCESS) {
        //write application file
        err = inu_usb_monitor_write(bootsplInfo.initramfsStartAddress + sizeof(val), bufP, val);
     }
     free(bufP);
     if (err != INU_USB_ERR_SUCCESS)
        break;

     //Load CNN loader (address is static due to HW limitation)
     if ((bufP = load_file(componentsParams.cnn_loader_file_name, &size)) != NULL) {

        if (securedOffset)
         err = inu_usb_monitor_write(0x2004000 , bufP, size);
        else err = inu_usb_monitor_write(0x2000000 , bufP, size);
        free(bufP);
        if (err != INU_USB_ERR_SUCCESS)
           break;
     }

     //Load CNN
     if ((bufP = load_file(componentsParams.cnn_file_name, &size)) != NULL) {
        err = inu_usb_monitor_write(bootsplInfo.cnnStartAddress, bufP, size);
        free(bufP);
        if (err != INU_USB_ERR_SUCCESS)
           break;
     }

     //Load CEVA
     if(!(inuUsbParams->version == NU4100_CHIP_VERSION))
     {
        if ((bufP = load_file(componentsParams.ceva_file_name, &size)) != NULL) {
           err = inu_usb_monitor_write(bootsplInfo.cevaStartAddress, bufP, size);
           free(bufP);
           if (err != INU_USB_ERR_SUCCESS)
              break;
        }
     }
     else  //In case of nu4100, use ceva memory area to store cnn app backup
     {
        if ((bufP = load_file(componentsParams.cnn_file_name, &size)) != NULL) {
           if ((bootsplInfo.cevaStartAddress + size) > bootsplInfo.cnnStartAddress) {
              inuUsbParams->log(INU_USB_LOG_ERROR, "%s, %s() Warning: CNN backup size exceeds its limit, skip backup\n",
                  _FILE_, __func__);
           } else {
              err = inu_usb_monitor_write(bootsplInfo.cevaStartAddress, bufP, size);
           }
           free(bufP);
           if (err != INU_USB_ERR_SUCCESS) {
              break;
           }
        }
     }

      //Jump to the kernel on the target
     if ((securedOffset != 0) && ((inuUsbParams->version == NU4000_C0_CHIP_VERSION) || (inuUsbParams->version == NU4000_C1_CHIP_VERSION) || (inuUsbParams->version == NU4100_CHIP_VERSION)))
     {
        val = 0x1234;
        err = inu_usb_monitor_write(GME_SAVE_AND_RESTORE6_ADDR, (unsigned char *)&val, sizeof(unsigned int));
     }
      else err = inu_usb_monitor_jump(bootsplInfo.invokeLinuxBootAddress);

     if(err != INU_USB_ERR_SUCCESS)
         break;

   } while(0);


   return err;
}

InuUsbErrorE inu_usb_boot_hotplug_handler(InuUsbModeE mode, char *boot_path, InuHotplugCallback hp_callback)
{
   InuUsbErrorE ret = INU_USB_ERR_SUCCESS;
   InuUsbParamsT *inuUsbParams = inu_usb_get_contex();
   if(reset_in_progress == 1)
   {
      return ret;
   }

   if (INU_USB_MODE_DETECT_CHIP_VERSION == mode)
   {
        // deal with raw insertion
        // here just for get chip version then notify back to FWManger. david @ 2022.10.04
        ret = inu_usb_get_chip_version();
        if (ret != INU_USB_ERR_SUCCESS)
            return ret;

        if (hp_callback)
        {
            hp_callback(INU_USB_EVT_INSERTION, inuUsbParams->version);
            hp_callback(INU_USB_EVT_FILTERID_UPDATED, inuUsbParams->filterId);
        }

        return ret;
   }
   else if (mode == INU_USB_MODE_PLUGGED_RAW) 
   {
      ret = inu_usb_boot_target(boot_path);
      if (ret != INU_USB_ERR_SUCCESS)
      {
         inu_usb_set_mode(0, mode);
         inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() %s insertion: boot failed, updating g_hotPlugCount... \n",
             _FILE_, __func__, CHIP_VERSION_TO_STRING(inuUsbParams->version));
         if (hp_callback)
            hp_callback(INU_USB_EVT_BOOT_FAILED, inuUsbParams->version);
      }
      else {
            inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() %s insertion: boot success, waiting for re-enumeration... \n",
                _FILE_, __func__, CHIP_VERSION_TO_STRING(inuUsbParams->version));
      }
   }
   else if (mode == INU_USB_MODE_PLUGGED_OS) {
      inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() %s insertion: kernel boot success, system ready for work... \n", 
          _FILE_, __func__, CHIP_VERSION_TO_STRING(inuUsbParams->version));
      inu_usb_set_mode(0, mode);
      inu_usb_open_non_ctl_enp();
      if (hp_callback)
         hp_callback(INU_USB_EVT_BOOT_COMPLETE, inuUsbParams->version);
   }
   else if (mode == INU_USB_MODE_UNPLUGGED) 
   {
       ret = INU_USB_ERR_DISCONNECTED;
      if (strlen(boot_path) == 0) {
         // deal with raw insertion
         if (hp_callback) {
            inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() %s removal: raw removal detected\n", _FILE_, __func__,
                CHIP_VERSION_TO_STRING(inuUsbParams->version));
            hp_callback(INU_USB_EVT_REMOVAL, inuUsbParams->version);
         }
         return ret;
      }
      if (hp_callback) {
         inuUsbParams->log(INU_USB_LOG_INFO, "%s, %s() %s: removal detected version\n", _FILE_, __func__,
             CHIP_VERSION_TO_STRING(inuUsbParams->version));
         //inu_usb_close_non_ctl_enp();
         hp_callback(INU_USB_EVT_REMOVAL, inuUsbParams->version);
      }
      return ret;
   }

   return ret;
}
