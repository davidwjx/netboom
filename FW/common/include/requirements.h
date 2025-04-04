#ifndef __REQUIREMENTS__
#define __REQUIREMENTS__

#include "inu2.h"
#include <stdint.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#if (defined _WIN32) || (defined _WIN64)
#include <io.h>
#include <stdio.h>
#include <process.h>
#define PATH_SEPARATOR  "\\"
#define getpid() _getpid()
#elif defined(__linux__)
#include    <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PATH_SEPARATOR  "/"
#endif

#define CHIP_VER_TO_STRING(version) (version == INU_DEVICE__CHIP_VERSION_B_E ? "b0" : "c0" )


#define CALIB_NUM_PATH (1)
#if (defined _WIN32) || (defined _WIN64)
#define CALIB_PATH "C:\\program files\\Inuitive\\InuDev\\config\\InuSensors\\ALE19090047\\Rev001\\"
//#define CALIB_PATH "C:\\program files\\Inuitive\\InuDev\\config\\InuSensors\\NST18350016\\Rev001\\"
#define CALIB_PATH_FULL "C:\\program files\\Inuitive\\InuDev\\config\\InuSensors\\ALE19090047\\Rev001\\Full"
#define CALIB_PATH_BINNING "C:\\program files\\Inuitive\\InuDev\\config\\InuSensors\\ALE19090047\\Rev001\\Binning"
//#define CALIB_PATH_VERTICAL_BINNING "C:\\program files\\Inuitive\\InuDev\\config\\InuSensors\\NST18260049\\Rev001\\VerticalBinning"
#define TEST_FOLDER_REV_001         ("\\Rev001\\")
#define TEST_FOLDER_HOST_35         ("\\Hot_35\\")
#define TEST_FOLDER_TEMPERATURE_LOW ("\\temperature_low\\")
#define CREATE_SENSORS_FOLDER ("\"\"mkdir Sensors")
#define UNZIP_SYSTEM_CMD ("\"\"C:\\Program Files\\7-Zip\\7z.exe\"\" x -oSensors -aos calib.7z")
#else
#define CALIB_PATH "/opt/Inuitive/InuDev/config/InuSensors/NGC17432173/Rev001/"
#define CALIB_PATH_FULL "/opt/Inuitive/InuDev/config/InuSensors/NST18260049/Rev001/Full"
#define CALIB_PATH_BINNING "/opt/Inuitive/InuDev/config/InuSensors/NST18260049/Rev001/Binning"
#define TEST_FOLDER_REV_001         ("/Rev001/")
#define TEST_FOLDER_HOST_35         ("/Hot_35/")
#define TEST_FOLDER_TEMPERATURE_LOW ("/temperature_low/")
#define UNZIP_SYSTEM_CMD ("unzip -o calib.7z -d /tmp/Sensors")
#define CREATE_SENSORS_FOLDER ("mkdir /tmp/Sensors")
#endif

#define BOOT_FOLDER_GRANULARITY 100
#define PARSE_BOOT_FOLDER(bootId) (((bootId % INU_DEFSG_BOOT_FACTOR) / BOOT_FOLDER_GRANULARITY) * BOOT_FOLDER_GRANULARITY)

bool DirectoryExists(const char* absolutePath) {
#if (defined _WIN32) || (defined _WIN64)
    if (_access(absolutePath, 0) == 0) {
#else
    if (access(absolutePath, 0) == 0) {
#endif
        struct stat status;
        stat(absolutePath, &status);

        return (status.st_mode & S_IFDIR) != 0;
    }
    return false;
}

static int is_folder_exist(char *baseFolder, char *subfolder)
{
    char temp[256];
    strcpy(temp,baseFolder);
    strcat(temp, subfolder);
    return DirectoryExists(temp);
}

static inline inu_graph__calibrationPathT *get_calib_path(const char *serialNumber, char *alternativePath)
{
    inu_graph__calibrationPathT *calibPathP = (inu_graph__calibrationPathT*)malloc(sizeof(inu_graph__calibrationPathT));
    char **temp;
    char temp2[384] = { 0 };
    char *test_folder_rev001 = strdup(TEST_FOLDER_REV_001);
    char* test_folder_host35 = strdup(TEST_FOLDER_HOST_35);
    char* test_folder_temperature_low = strdup(TEST_FOLDER_TEMPERATURE_LOW);

    calibPathP->calibrationPath = (char**)malloc(sizeof(char*)*CALIB_NUM_PATH);
    temp = calibPathP->calibrationPath;

    if (*alternativePath)
    {
        strcpy(temp2,alternativePath);
    }
    else
    {
#if (defined _WIN32) || (defined _WIN64)
        strcat(temp2, ".\\Sensors");
#else
        strcat(temp2, "/tmp/Sensors");
#endif
        strcat(temp2, PATH_SEPARATOR);
        strcat(temp2, serialNumber);
    }

    if (is_folder_exist(temp2, test_folder_rev001))
    {
        strcat(temp2, test_folder_rev001);
    }
    else if (is_folder_exist(temp2, test_folder_host35))
    {
        strcat(temp2, test_folder_host35);
    }
    else if (is_folder_exist(temp2, test_folder_temperature_low))
    {
        strcat(temp2, test_folder_temperature_low);
    }

    *calibPathP->calibrationPath = strdup(temp2);
    calibPathP->numOfPaths = CALIB_NUM_PATH;
    calibPathP->calibrationPath = temp;

    free(test_folder_rev001);
    free(test_folder_host35);
    free(test_folder_temperature_low);

   return calibPathP;
}

#if (defined _WIN32) || (defined _WIN64)
static inline const char *get_installation_path(void) { return "C:\\program files\\Inuitive\\InuDev"; }
static inline const char *get_boot_path(void) { return "C:\\program files\\Inuitive\\InuDev\\bin"; }
#else
static inline const char *get_installation_path(void) { return "/opt/Inuitive/InuDev"; }
static inline const char *get_boot_path(void) { return "/opt/Inuitive/InuDev/bin"; }
#endif

static inline const char *get_socxml_path(const inu_device__version *versionP, const char* alternativePath, const char* alternativeName)
{
    char *out_xmlPath;
    char str_boot_id[15];
    char str_path_boot_id[15];
    const char *boot_path;

    out_xmlPath = (char*)malloc(1024);
    sprintf(str_boot_id, "boot%d", versionP->bootId);

    if (!alternativePath || !(*alternativePath))
    {
        boot_path = get_boot_path();
        sprintf(str_path_boot_id, "boot%d", PARSE_BOOT_FOLDER(versionP->bootId));
        strcpy(out_xmlPath, boot_path);
        strcat(out_xmlPath, PATH_SEPARATOR);
        if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
        {
            strcat(out_xmlPath, "NU4000");
            strcat(out_xmlPath, CHIP_VER_TO_STRING(versionP->hwVerId.fields.versionId));
        }
        else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
        {
            strcat(out_xmlPath, "NU4100");
        }
        strcat(out_xmlPath, PATH_SEPARATOR);
        strcat(out_xmlPath, str_path_boot_id);
    }
    else
    {
        strcpy(out_xmlPath, alternativePath);
    }

    if(!alternativeName || !(*alternativeName))
    {
        strcat(out_xmlPath, PATH_SEPARATOR);
        if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
        {
            strcat(out_xmlPath, "nu4k");
        }
        else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
        {
            strcat(out_xmlPath, "nu4100");
        }
        strcat(out_xmlPath, "_");
        strcat(out_xmlPath, str_boot_id);
        strcat(out_xmlPath, ".xml");
    }
    else
    {
        strcat(out_xmlPath, PATH_SEPARATOR);
        strcat(out_xmlPath, alternativeName);
    }

    return out_xmlPath;
}

static inline const char *get_swxml_path(const inu_device__version *versionP, const char *alternativePath, const char* alternativeName)
{
   char *out_xmlPath;
   char str_boot_id[15];
   char str_path_boot_id[15];
   const char *boot_path;

   out_xmlPath = (char*)malloc(1024);
   sprintf(str_boot_id, "boot%d", versionP->bootId);
    if (!alternativePath || !(*alternativePath))
    {
        boot_path = get_boot_path();
        sprintf(str_path_boot_id, "boot%d", PARSE_BOOT_FOLDER(versionP->bootId));

        strcpy(out_xmlPath, boot_path);
        strcat(out_xmlPath, PATH_SEPARATOR);
        if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
        {
            strcat(out_xmlPath, "NU4000");
            strcat(out_xmlPath, CHIP_VER_TO_STRING(versionP->hwVerId.fields.versionId));
        }
        else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
        {
            strcat(out_xmlPath, "NU4100");
        }
        strcat(out_xmlPath, PATH_SEPARATOR);
        strcat(out_xmlPath, str_path_boot_id);
    }
    else
    {
        strcpy(out_xmlPath, alternativePath);
    }

    if (!alternativeName || !(*alternativeName))
    {
        strcat(out_xmlPath, PATH_SEPARATOR);
        strcat(out_xmlPath, "sw_graph_");
        strcat(out_xmlPath, str_boot_id);
        strcat(out_xmlPath, ".xml");
    }
    else
    {
        strcat(out_xmlPath, PATH_SEPARATOR);
        strcat(out_xmlPath, alternativeName);
    }

    return out_xmlPath;
}

static inline const char *get_inject_socxml_path(const inu_device__version *versionP, const char *alternativePath)
{
    char *out_xmlPath;
    const char *boot_path;

    boot_path = get_boot_path();
    out_xmlPath = (char*)malloc(1024);
     if (!(*alternativePath))
     {
        strcpy(out_xmlPath, boot_path);
        strcat(out_xmlPath, PATH_SEPARATOR);
        if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
        {
            strcat(out_xmlPath, "NU4000");
            strcat(out_xmlPath, CHIP_VER_TO_STRING(versionP->hwVerId.fields.versionId));
        }
        else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
        {
            strcat(out_xmlPath, "NU4100");
        }
        strcat(out_xmlPath, PATH_SEPARATOR);
        // nu4k_inject.xml is boot indifferent, so the file exists only in boot0
        strcat(out_xmlPath, "boot0");
     }
     else
     {
         strcpy(out_xmlPath, alternativePath);
     }
     strcat(out_xmlPath, PATH_SEPARATOR);
     if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
     {
        strcat(out_xmlPath, "nu4k_inject.xml");
     }
     else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
     {
         strcat(out_xmlPath, "nu4100_inject.xml");
     }

    return out_xmlPath;
}

static inline const char *get_inject_swxml_path(const inu_device__version *versionP, const char *alternativePath)
{
   char *out_xmlPath;
   const char *boot_path;

   boot_path = get_boot_path();
   out_xmlPath = (char*)malloc(1024);
    if (!(*alternativePath))
    {
        strcpy(out_xmlPath, boot_path);
        strcat(out_xmlPath, PATH_SEPARATOR);
        if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
        {
            strcat(out_xmlPath, "NU4000");
            strcat(out_xmlPath, CHIP_VER_TO_STRING(versionP->hwVerId.fields.versionId));
        }
        else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
        {
            strcat(out_xmlPath, "NU4100");
        }
        strcat(out_xmlPath, PATH_SEPARATOR);
        // sw_graph_inject.xml is boot indifferent, so the file exists only in boot0
        strcat(out_xmlPath, "boot0");
    }
    else
    {
        strcpy(out_xmlPath, alternativePath);
    }
   strcat(out_xmlPath, PATH_SEPARATOR);
   strcat(out_xmlPath, "sw_graph_inject.xml");

   return out_xmlPath;
}

static inline char *get_dpe_mode_full_path(const inu_device__version *versionP, const char *fileName, const char *alternativePath)
{
    char *out_fullpath;
    const char *installation_path;
    out_fullpath = (char*)malloc(1024);
    if (!(*alternativePath))
    {
        installation_path = get_installation_path();
        strcpy(out_fullpath, installation_path);
        strcat(out_fullpath, PATH_SEPARATOR);
        strcat(out_fullpath, "config");
        strcat(out_fullpath, PATH_SEPARATOR);
        strcat(out_fullpath, "Presets");
        strcat(out_fullpath, PATH_SEPARATOR);
        if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4000_E)
        {
            strcat(out_fullpath, "NU4000C0");
        }
        else if (versionP->hwVerId.fields.revisionId == INU_DEVICE__CHIP_REVISION_NU4100_E)
        {
            strcat(out_fullpath, "NU4100");
        }
    }
    else
    {
        strcpy(out_fullpath, alternativePath);
    }
    strcat(out_fullpath, PATH_SEPARATOR);
    strcat(out_fullpath, fileName);
    return out_fullpath;
}


static inline INT32 get_interface0(void) {return 0;}
static inline void get_interface_0_setup(void *setup) {(void)setup;}
static inline int get_external_hp(void) { return 0; }
static inline int get_thread_sleep_time_us(void) { return 5000; }
static inline int get_usb_id(void) { return getpid(); }
static inline void os_sleep(UINT32 msec) {(void)msec;}

static inline void store_calib_data(void *calib_data, int size)
{
    FILE *calibfile = fopen("calib.7z", "wb");
    if (calibfile)
    {
        fwrite(calib_data, size, 1, calibfile);
        fflush(calibfile);
        fclose(calibfile);
    }

    system(CREATE_SENSORS_FOLDER);

    //need to unzip it
    system(UNZIP_SYSTEM_CMD);
}

inline int readBinFile(const char *name, inu_device_memory_t *buffer)
{
    FILE *file;
    file = fopen(name, "rb");  // r for read, b for binary
    if (file)
    {
        fseek(file, 0, SEEK_END);
        buffer->bufsize = ftell(file);
        fseek(file, 0, SEEK_SET);
    }
    else
    {
        printf("ERROR fopen file %s\n",name);
        return -1;
    }

    buffer->bufP = (char *)malloc((buffer->bufsize) + 1);
    fread(buffer->bufP, buffer->bufsize, 1, file);
    fclose(file);
    return 0;
}

const char *usbEventToString(INU_DEFSG_usbStatusE event)
{
   switch (event)
   {
   case(INU_DEFSG_USB_EVT_REMOVAL):
        return "INU_DEFSG_USB_EVT_REMOVAL";
   case(INU_DEFSG_USB_EVT_INSERTION):
        return "INU_DEFSG_USB_EVT_INSERTION";
   case(INU_DEFSG_USB_EVT_BOOT_COMPLETE):
        return "INU_DEFSG_USB_EVT_BOOT_COMPLETE";
   case(INU_DEFSG_USB_EVT_BOOT_FAILED):
        return "INU_DEFSG_USB_EVT_BOOT_FAILED";
    default:
        return "UNKNOWN EVENT";
   }
}

#ifdef __cplusplus
}
#endif

#endif //__REQUIREMENTS__
