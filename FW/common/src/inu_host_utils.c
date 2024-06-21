#include "inu_host_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#ifdef UNICODE
#include <codecvt>
#endif

#if (defined _WIN32) || (defined _WIN64)
   const char path_separator[] = "\\";
#elif defined(__linux__)
   const char path_separator[] = "/";
#endif

#define INU_DEFSG_BOOT_FACTOR (10000)

int host_millisleep(unsigned ms)
{
#if (defined _WIN32) || (defined _WIN64)
  SetLastError(0);
  Sleep(ms);
  return GetLastError() ?-1 :0;
#elif defined(__linux__)
  usleep(1000 * ms);
  return 0;
#else
#error ("no milli sleep available for platform")
  return -1;
#endif
}

const char *get_path_separator()
{
   return path_separator;
}

int is_file_exists(char *name)
{
#ifdef UNICODE
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> convert;
    std::string utf8Name(name);
    struct _stat   buffer;
    return (_wstat (convert.from_bytes(utf8Name).c_str(), &buffer) == 0);
#else
    struct stat   buffer;
    return (stat (name, &buffer) == 0);
#endif
}

#define BOOTSPL_FILE_NAME           "bootspl"
#define BOOTSPL_VERSION_STRING_B0      "-nu3000b0"
#define BOOTSPL_VERSION_STRING_4000_A0 "-nu4000a0"
#define BOOTSPL_VERSION_STRING_4000_B0 "-nu4000b0"
#define BOOTSPL_VERSION_STRING_4000_C0 "-nu4000c0"
#define BOOTSPL_VERSION_STRING_4000_C1 "-nu4000c1"
#define BOOTSPL_VERSION_STRING_4100    "-nu4100"
#define BOOTSPL_FILE_EXTENSION         ".bin"
#define DTB_FILE_NAME               "nu3000"
#define DTB_FILE_NAME_4000_A0       "nu4000"
#define DTB_FILE_NAME_4000_B0       "nu4000b0"
#define DTB_FILE_NAME_4000_C0       "nu4000c0"
#define DTB_FILE_NAME_4100          "nu4100"
#define DTB_FILE_NAME_LEGACY        "nu3000-m3"
#define DTB_FILE_EXTENSION          ".dtb"
#define KERNEL_FILE_NAME            "zImage"
#define KERNEL_UVC_FILE_NAME        "zImage_uvc"
#define KERNEL_RNDIS_FILE_NAME      "zImage_rndis"
#define KERNEL_FILE_EXTENSION       ""
#define APP_FILE_NAME               "nu3000"
#define APP_FILE_NAME_4000          "nu4000"
#define APP_FILE_NAME_4000b0        "nu4000b0"
#define APP_FILE_NAME_4000c0        "nu4000c0"
#define APP_FILE_NAME_4100          "nu4100"
#define APP_FILE_SA_NAME_4100       "nu4100_sa"
#define APP_FILE_EXTENSION          ".zip"
#define INITRAMFS_FILE_NAME         "initramfs"
#define INITRAMFS_FILE_EXT_NAME     ".gz"
#define CNN_LOADER_FILE_NAME        "cnn_boot_loader.bin"
#define CNN_FILE_NAME               "cnn_boot_app.bin"
#define CEVA_FILE_NAME              "inu_app.cva"
#define CONFIG_BOARD_FILE           "boardConfig.txt"
#define MAX_ID_STR_SIZE             (15)
#define SA_BOOT_ID_87               (87)
#define RNDIS_CLIENT_ID             "boot700"
ComponentsErrorE get_components(ComponentsParamsT* componentsParamsP, int id, const char* boot_path, int version)
{
    int  client_id;
    char str_boot_client[MAX_ID_STR_SIZE];
    char str_boot_id[MAX_ID_STR_SIZE],str_bootId_without10k[MAX_ID_STR_SIZE];
    char str_chip[MAX_ID_STR_SIZE];
    char boot_path_ext[BOOT_PATH_LENGTH_MAX];

    // detect client id of boot folder
    strcpy(boot_path_ext, boot_path);
    // add chip subfolder
    if (version == NU4000_B0_CHIP_VERSION)
        sprintf(str_chip, "nu%xb0", (version >> 16) & 0xFFFF);
    else if ((version == NU4000_C0_CHIP_VERSION) || (version == NU4000_C1_CHIP_VERSION))
        sprintf(str_chip, "nu%xc0", (version >> 16) & 0xFFFF);
    else if (version == NU4100_CHIP_VERSION)
        sprintf(str_chip, "nu%x", (version >> 16) & 0xFFFF);
    else sprintf(str_chip, "nu%x", (version >> 16) & 0xFFFF);

    strcat(boot_path_ext, get_path_separator());
    strcat(boot_path_ext, str_chip);

    client_id = ((id % INU_DEFSG_BOOT_FACTOR) / 100) * 100;
    // re-create path to boot folder
    sprintf(str_boot_client, "boot%d", client_id);
    sprintf(str_boot_id, "boot%d", id);
    sprintf(str_bootId_without10k, "boot%d", id % 10000);
    strcat(boot_path_ext, get_path_separator());
    strcat(boot_path_ext, str_boot_client);

    // check the path  for boot files
    strcpy(componentsParamsP->linux_file_name, boot_path_ext);
    strcat(componentsParamsP->linux_file_name, get_path_separator());
    strcat(componentsParamsP->linux_file_name, KERNEL_FILE_NAME);
    strcat(componentsParamsP->linux_file_name, "-");
    strcat(componentsParamsP->linux_file_name, str_boot_id);
    if (!is_file_exists(componentsParamsP->linux_file_name)) 
    {
        strcpy(componentsParamsP->linux_file_name, boot_path_ext);
        strcat(componentsParamsP->linux_file_name, get_path_separator());
        if (id == SA_BOOT_ID_87)
          strcat(componentsParamsP->linux_file_name, KERNEL_UVC_FILE_NAME);
        else if (!strcmp(str_boot_client, RNDIS_CLIENT_ID))
          strcat(componentsParamsP->linux_file_name, KERNEL_RNDIS_FILE_NAME);
        else 
            strcat(componentsParamsP->linux_file_name, KERNEL_FILE_NAME);
        if (!is_file_exists(componentsParamsP->linux_file_name)) {
            return COMPONENTS_ERR_KERNEL_FILE;
        }
    }
    strcpy(componentsParamsP->config_file_name, boot_path_ext);
    strcat(componentsParamsP->config_file_name, get_path_separator());
    strcat(componentsParamsP->config_file_name, CONFIG_BOARD_FILE);

    strcpy(componentsParamsP->dtb_file_name, boot_path_ext);
    strcat(componentsParamsP->dtb_file_name, get_path_separator());
    if (version == NU3000_B0_CHIP_VERSION)
        strcat(componentsParamsP->dtb_file_name, DTB_FILE_NAME);
    else if (version == NU4000_A0_CHIP_VERSION)
        strcat(componentsParamsP->dtb_file_name, DTB_FILE_NAME_4000_A0);
    else if (version == NU4000_B0_CHIP_VERSION)
        strcat(componentsParamsP->dtb_file_name, DTB_FILE_NAME_4000_B0);
    else if ((version == NU4000_C0_CHIP_VERSION) || (version == NU4000_C1_CHIP_VERSION))
        strcat(componentsParamsP->dtb_file_name, DTB_FILE_NAME_4000_C0);
    else if (version == NU4100_CHIP_VERSION)
        strcat(componentsParamsP->dtb_file_name, DTB_FILE_NAME_4100);
    strcat(componentsParamsP->dtb_file_name, "-");
    strcat(componentsParamsP->dtb_file_name, str_boot_id);
    strcat(componentsParamsP->dtb_file_name, DTB_FILE_EXTENSION);

    if (!is_file_exists(componentsParamsP->dtb_file_name)) {
        return COMPONENTS_ERR_DTB_FILE;
    }

    strcpy(componentsParamsP->app_file_name, boot_path_ext);
    strcat(componentsParamsP->app_file_name, get_path_separator());
    if (version == NU3000_B0_CHIP_VERSION)
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME);
    else if (version == NU4000_A0_CHIP_VERSION)
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4000);
    else if (version == NU4000_B0_CHIP_VERSION)
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4000b0);
    else if ((version == NU4000_C0_CHIP_VERSION) || (version == NU4000_C1_CHIP_VERSION))
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4000c0);
    else if (version == NU4100_CHIP_VERSION)
    {
        if ((id == SA_BOOT_ID_87))
            strcat(componentsParamsP->app_file_name, APP_FILE_SA_NAME_4100);
        else strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4100);
   }
 
   strcat(componentsParamsP->app_file_name, "-");
   strcat(componentsParamsP->app_file_name, str_boot_id);
   strcat(componentsParamsP->app_file_name, APP_FILE_EXTENSION);

   if (!is_file_exists(componentsParamsP->app_file_name)) {
      strcpy(componentsParamsP->app_file_name, boot_path_ext);
      strcat(componentsParamsP->app_file_name, get_path_separator());
     if (version == NU3000_B0_CHIP_VERSION)
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME);
     else if (version == NU4000_A0_CHIP_VERSION)
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4000);
     else if((version == NU4000_B0_CHIP_VERSION))
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4000b0);
     else if((version == NU4000_C0_CHIP_VERSION) || (version == NU4000_C1_CHIP_VERSION))
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4000c0);
     else if (version == NU4100_CHIP_VERSION)
        strcat(componentsParamsP->app_file_name, APP_FILE_NAME_4100);
      strcat(componentsParamsP->app_file_name, APP_FILE_EXTENSION);

      if (!is_file_exists(componentsParamsP->app_file_name)) {
         //return COMPONENTS_ERR_APP_FILE; // will replace app in initramfs if exist.
      }
   }

      // do not load bootspl for bootfix
      strcpy(componentsParamsP->spl_file_name, boot_path_ext);
      strcat(componentsParamsP->spl_file_name, get_path_separator());
      strcat(componentsParamsP->spl_file_name, BOOTSPL_FILE_NAME);
      if (version == NU3000_B0_CHIP_VERSION)
         strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_B0);
      else if (version == NU4000_A0_CHIP_VERSION)
         strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_A0);
      else if (version == NU4000_B0_CHIP_VERSION)
         strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_B0);
      else if (version == NU4000_C0_CHIP_VERSION)
         strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_C0);
      else if (version == NU4000_C1_CHIP_VERSION)
         strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_C1);
      else if (version == NU4100_CHIP_VERSION)
         strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4100);
      strcat(componentsParamsP->spl_file_name, "-");
      strcat(componentsParamsP->spl_file_name, str_bootId_without10k);
      strcat(componentsParamsP->spl_file_name, BOOTSPL_FILE_EXTENSION);

      if (!is_file_exists(componentsParamsP->spl_file_name)) {
         strcpy(componentsParamsP->spl_file_name, boot_path_ext);
         strcat(componentsParamsP->spl_file_name, get_path_separator());
         strcat(componentsParamsP->spl_file_name, BOOTSPL_FILE_NAME);
         if (version == NU3000_B0_CHIP_VERSION)
            strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_B0);
         else if (version == NU4000_A0_CHIP_VERSION)
            strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_A0);
         else if (version == NU4000_B0_CHIP_VERSION)
            strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_B0);
         else if (version == NU4000_C0_CHIP_VERSION)
            strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_C0);
         else if (version == NU4000_C1_CHIP_VERSION)
            strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4000_C1);
         else if (version == NU4100_CHIP_VERSION)
            strcat(componentsParamsP->spl_file_name, BOOTSPL_VERSION_STRING_4100);
         strcat(componentsParamsP->spl_file_name, BOOTSPL_FILE_EXTENSION);
         if (!is_file_exists(componentsParamsP->spl_file_name)) {
            return COMPONENTS_ERR_SPL_FILE;
         }
      }

      // initramfs
      strcpy(componentsParamsP->initramfs_file_name, boot_path_ext);
      strcat(componentsParamsP->initramfs_file_name, get_path_separator());
      strcat(componentsParamsP->initramfs_file_name, INITRAMFS_FILE_NAME);
      strcat(componentsParamsP->initramfs_file_name, "_");
      strcat(componentsParamsP->initramfs_file_name, str_boot_id);
      strcat(componentsParamsP->initramfs_file_name, INITRAMFS_FILE_EXT_NAME);

      if (!is_file_exists(componentsParamsP->initramfs_file_name))
      {
         strcpy(componentsParamsP->initramfs_file_name, boot_path_ext);
         strcat(componentsParamsP->initramfs_file_name, get_path_separator());
         strcat(componentsParamsP->initramfs_file_name, INITRAMFS_FILE_NAME);
         strcat(componentsParamsP->initramfs_file_name, INITRAMFS_FILE_EXT_NAME);
         if (!is_file_exists(componentsParamsP->initramfs_file_name))
            return COMPONENTS_ERR_INITRAMFS_FILE;
      }

      strcpy(componentsParamsP->cnn_loader_file_name, boot_path_ext);
      strcat(componentsParamsP->cnn_loader_file_name, get_path_separator());
      strcat(componentsParamsP->cnn_loader_file_name, CNN_LOADER_FILE_NAME);

      strcpy(componentsParamsP->cnn_file_name, boot_path_ext);
      strcat(componentsParamsP->cnn_file_name, get_path_separator());
      strcat(componentsParamsP->cnn_file_name, CNN_FILE_NAME);

      strcpy(componentsParamsP->ceva_file_name, boot_path_ext);
      strcat(componentsParamsP->ceva_file_name, get_path_separator());
      strcat(componentsParamsP->ceva_file_name, CEVA_FILE_NAME);

   return COMPONENTS_SUCCESS;
}
