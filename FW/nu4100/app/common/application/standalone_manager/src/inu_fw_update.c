#ifdef ENABLE_FW_UPDATE
/****************************************************************************
 *
 *   FileName: inu_fw_update.c
 *
 *   Author: Eldad M.
 *
 *   Date:
 *
 *   Description: firmware update manager
 *
 ****************************************************************************/

/****************************************************************************
 ***************                     I N C L U D E   F I L E S       *************
 ****************************************************************************/

#include "assert.h"

#include "inu_fw_update_api.h"
#include "cdc_mngr.h"
#include "inu_common.h"
#include "inu_device.h"
#include "inu_function.h"
#include "log.h"
#include "inu_storage_layout.h"
#if DEFSG_IS_GP
#include "inu_fw_update.h"
#include "spi_flash_drv.h"
#include "spi_flash.h"
#include "wd.h"
#include "gpio_drv.h"
#include "version.h"
#include "gme_drv.h"
#include "cmem.h"
#include "mem_map.h"
#include "verify.h"
#else
#define LIBARCHIVE_STATIC
#include"archive.h"
#include"archive_entry.h"
#endif //DEFSG_IS_GP

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/

/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
#if DEFSG_IS_GP
typedef struct
{
    UINT32 phyAddress;
    UINT32 virtAddress;
    UINT32 size;
}inu_fw_update_workSpaceAddress;
#endif

/****************************************************************************
 ***************       G L O B A L       D A T A              ***************
 ****************************************************************************/


/****************************************************************************
 ***************      E X T E R N A L   F U N C T I O N S     ***************
 ****************************************************************************/


/****************************************************************************
 ***************     P R E    D E F I N I T I O N     OF      ***************
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/


/****************************************************************************
 ***************     G L O B A L         F U N C T I O N S    ***************
 ****************************************************************************/


 /****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#ifndef MAX_PATH
#define MAX_PATH      (260)
#endif

#define READ_EFUSE 1

#define FW_UPDATE_SECTION_START      INU_STORAGE_SECTION_BOOTSPL
#define FW_UPDATE_SECTION_END        INU_STORAGE_SECTION_APP

#define PATH_SEPARATOR  "\\"

char* storageFileName[FW_UPDATE_SECTION_END + 1] = {

   "bootfixFile.bin",
   "bootfixMetaFile.bin",
   "productionFile.bin",
   "calibrationFile.bin",
   "bootsplFile.bin",
   "dtbFile.bin",
   "kernelFile.bin",
   "rootfsFile.bin",
   "cevaFile.bin",
   "cnnLdFile.bin",
   "cnnFile.bin",
   "appFile.bin"
};

// char *storageFileName[FW_UPDATE_SECTION_END + 1] = {
//     "bootfixFile.bin",
//     "bootfixMetaFile.bin",
//     "productionFile.bin",
//     "calibrationFile.bin",
//     "bootspl-nu4100.bin",
//     "nu4100-boot10085.dtb",
//     "zImage",
//     "initramfs.gz",
//     "cevaFile.bin",
//     "cnn_boot_loader.bin",
//     "cnn_boot_app.bin",
//     "nu4100.zip"
// };

#if DEFSG_IS_GP
#define MAX_FILE_NAME (256)

#define FW_UPDATE_TEMP_DIRECTORY "/tmp/update/"
#define FW_UPDATE_DIRECTORY "/media/inuitive/update/"
#define WORKING_DIRECTORY "/media/inuitive/"
#define FW_UPDATE_FILES_MAX_SIZE (31*1024*1024)
#define ERASE_ALL_FLASH 1
#define FW_UPDATE_SECTION_BOOTSPL_BIT  (1<<INU_STORAGE_SECTION_BOOTSPL)
#define FW_UPDATE_SECTION_DTB_BIT      (1<<INU_STORAGE_SECTION_DTB)
#define FW_UPDATE_SECTION_KERNEL_BIT   (1<<INU_STORAGE_SECTION_KERNEL)
#define FW_UPDATE_SECTION_ROOTFS_BIT   (1<<INU_STORAGE_SECTION_ROOTFS)
#define FW_UPDATE_SECTION_CNN_LD_BIT   (1<<INU_STORAGE_SECTION_CNN_LD)
#define FW_UPDATE_SECTION_CNN_BIT      (1<<INU_STORAGE_SECTION_CNN)
#define FW_UPDATE_SECTION_APP_BIT      (1<<INU_STORAGE_SECTION_APP)

UINT32 storageMagicNumber[FW_UPDATE_SECTION_END + 1] = {
    SECTION_BOOTFIX_MAGIC_NUMBER,
    SECTION_BOOTFIX_META_MAGIC_NUMBER,
    SECTION_PRODUCTION_MAGIC_NUMBER,
    SECTION_CALIBRATION_MAGIC_NUMBER,
    SECTION_BOOTSPL_MAGIC_NUMBER,
    SECTION_DTB_MAGIC_NUMBER,
    SECTION_KERNEL_MAGIC_NUMBER,
    SECTION_ROOTFS_MAGIC_NUMBER,
    SECTION_CEVA_MAGIC_NUMBER,
    SECTION_CNN_LD_MAGIC_NUMBER,
    SECTION_CNN_MAGIC_NUMBER,
    SECTION_APP_MAGIC_NUMBER
};

unsigned int storageMustHaveComponent[FW_UPDATE_SECTION_END + 1] = {
    1,     //   INU_STORAGE_SECTION_BOOTFIX          = 0,
    1,     //   INU_STORAGE_SECTION_BOOTFIX_METADATA = 1,
    1,     //   INU_STORAGE_SECTION_PRODUCTION_DATA  = 2,
    1,     //   INU_STORAGE_SECTION_CALIBRATION_DATA = 3,
    1,     //   INU_STORAGE_SECTION_BOOTSPL          = 4,
    1,     //   INU_STORAGE_SECTION_DTB              = 5,
    1,     //   INU_STORAGE_SECTION_KERNEL           = 6,
    1,     //   INU_STORAGE_SECTION_ROOTFS           = 7,
    0,     //   INU_STORAGE_SECTION_CEVA             = 8,
    1,     //   INU_STORAGE_SECTION_CNN_LD           = 9,
    1,     //   INU_STORAGE_SECTION_CNN              = 10,
    1      //   INU_STORAGE_SECTION_APP              = 11
};

inu_fw_update_workSpaceAddress workSpaceAddress[FW_UPDATE_SECTION_END + 1];

//#define SECTION_PBSS __attribute__((__section__(".pbss")))
//SECTION_PBSS
extern SB_EfuseArea_t eFUSEDB;

inu_device__fwVersionIdU gFwVersionId = {0};

typedef struct
{
   UINT32                     deviceBaseAddress;
} GME_DRVP_deviceDescT;

void inu_fw_update__setFlashGpio(UINT8 flashNum)
{
    UINT32 gpioNum=60;
    GPIO_DRVG_gpioSetDirParamsT         gpioDir;
    GPIO_DRVG_gpioSetValParamsT         gpioParams;
    static UINT8 lastRdFlashNum=MAIN_FLASH;

    if (flashNum != lastRdFlashNum)
    {
        gpioDir.direction = GPIO_DRVG_OUT_DIRECTION_E;
        gpioDir.gpioNum = gpioNum;
        IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_DIR_CMD_E, &gpioDir);
        OS_LYRG_usleep(50);
        gpioParams.gpioNum = gpioNum;
        if (flashNum != MAIN_FLASH)
        {
            gpioParams.val = 1;
        }
        else gpioParams.val = 0;
        IO_PALG_ioctl(IO_PALG_getHandle(IO_GPIO_E), GPIO_DRVG_SET_GPIO_VAL_CMD_E, &gpioParams);
    }
    lastRdFlashNum = flashNum;
}

static BOOLEAN inu_fw_update__readBinFile(const char *fileName, inu_device_memory_t *buffer)
{
    FILE *file;
    file = fopen(fileName, "rb");  // r for read, b for binary
    if (file)
    {
        fseek(file, 0, SEEK_END);
        buffer->bufsize = ftell(file);
        fseek(file, 0, SEEK_SET);
        buffer->bufP = (char *)malloc((buffer->bufsize) + 1);
        fread(buffer->bufP, buffer->bufsize, 1, file);
        fclose(file);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/****************************************************************************
*
*  Function Name: inu_fw_update__gpWriteFlashSection
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
ERRG_codeE inu_fw_update__gpWriteFlashSection(InuStorageSectionTypeE sectionType, UINT8 *flashBufP, UINT32 flashBufSize, UINT32 sectionStart, UINT32 *currSectionSize)
{
    ERRG_codeE  retCode  = INU_FW_UPDATE__RET_SUCCESS;
    SPI_FLASH_DRVG_flashParametersT flashParameters;
    UINT32 currentSec;
    struct timespec tv;
    InuSectionHeaderT inuSectionHeader;
    UINT32 inuSectionHeaderSize = sizeof(InuSectionHeaderT);
    UINT32 sectionFormat = INU_STORAGE_SECTION_FORMAT_RAW;
    UINT32 blockSize = SPI_FLASHG_getBlockSize(MAIN_FLASH);
    UINT32 startFlashAddress = sectionStart * blockSize;
    UINT32 sectionSize = ((flashBufSize + inuSectionHeaderSize) / (blockSize));

    if ((flashBufSize + inuSectionHeaderSize) % (blockSize))
    {
        sectionSize++;
    }

    inu_fw_update__setFlashGpio(MAIN_FLASH);
#if (ERASE_ALL_FLASH == 0)
    memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

    flashParameters.flashEraseBlock.startFlashAddress  = startFlashAddress;
    flashParameters.flashEraseBlock.eraseBlockSize     = flashBufSize + inuSectionHeaderSize;

    retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E, &flashParameters);
    if (ERRG_FAILED(retCode))
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
    }
#endif
    memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

    flashParameters.flashProgram.startFlashAddress  = startFlashAddress + inuSectionHeaderSize;
    flashParameters.flashProgram.writeFlashSize     = flashBufSize;
    flashParameters.flashProgram.flashBufP          = flashBufP;

    retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
    if (ERRG_FAILED(retCode))
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: Data - FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
    }
#if 1
    //OS_LYRG_usleep(50000);
    memset(&inuSectionHeader, 0, sizeof(InuSectionHeaderT));
    memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

    clock_gettime(CLOCK_REALTIME, &tv);
    currentSec = ((UINT32)tv.tv_sec * 1000000) + ((UINT32)tv.tv_nsec / 1000);

    if (sectionType == INU_STORAGE_SECTION_APP)
    {
        sectionFormat = INU_STORAGE_SECTION_FORMAT_ZIP;
    }

    inuSectionHeader.magicNumber        = storageMagicNumber[sectionType];
    inuSectionHeader.sectionDataSize    = flashBufSize;
    inuSectionHeader.sectionFormat      = sectionFormat;
    inuSectionHeader.timestamp          = currentSec;
    inuSectionHeader.sectionSize        = sectionSize;

    sprintf(inuSectionHeader.versionString, "%d.%d.%d.%d", gFwVersionId.fields.major, gFwVersionId.fields.minor, gFwVersionId.fields.build, gFwVersionId.fields.subBuild);

    flashParameters.flashProgram.startFlashAddress  = startFlashAddress;
    flashParameters.flashProgram.writeFlashSize     = inuSectionHeaderSize;
    flashParameters.flashProgram.flashBufP          = (UINT8 *)&inuSectionHeader;
    retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
    if (ERRG_FAILED(retCode))
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "fail: Header - FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: Header - FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
    }
#endif
    *currSectionSize = sectionSize;

    return(retCode);
}

void inu_fw_update__writeMetaFwUpdate()
{

    ERRG_codeE  retCode  = INU_FW_UPDATE__RET_SUCCESS;
    SPI_FLASH_DRVG_flashParametersT flashParameters;
    UINT32 startOffset;
    InuSectionHeaderT inuSectionHeader;
    InuBootfixHeaderT bootfixMetadata;
    UINT32 sectionHeaderSize = sizeof(InuSectionHeaderT);
    UINT32 bootfixHeaderSize = sizeof(InuBootfixHeaderT);
    UINT32 blockSize = SPI_FLASHG_getBlockSize(MAIN_FLASH);

    startOffset = SECTION_BOOTFIX_META_START_BLOCK * blockSize;

    memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));
    flashParameters.flashRead.startFlashAddress  = startOffset;
    flashParameters.flashRead.readFlashSize      = sectionHeaderSize;
    flashParameters.flashRead.flashBufP          = (UINT8 *)&inuSectionHeader;

    LOGG_PRINT(LOG_INFO_E, retCode, "burn BOOTFIX_META SECTION to Flash. Set FW_UPDATE flag.\n");

    retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
    if (ERRG_FAILED(retCode))
    {
        LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
    }
    else
    {
        LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
    }


    if (ERRG_SUCCEEDED(retCode))
    {
        if (inuSectionHeader.magicNumber == SECTION_BOOTFIX_META_MAGIC_NUMBER)
        {
            memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));
            flashParameters.flashRead.startFlashAddress  = startOffset + sectionHeaderSize;
            flashParameters.flashRead.readFlashSize      = bootfixHeaderSize;
            flashParameters.flashRead.flashBufP          = (UINT8 *)&bootfixMetadata;

            retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_READ_CMD_E, &flashParameters);
            if (ERRG_FAILED(retCode))
            {
                LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
            }
            else
            {
                LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_READ_CMD_E\n");
            }

            if (ERRG_SUCCEEDED(retCode))
            {
                //
                bootfixMetadata.flashFlags |= INU_STORAGE_LAYOUT__FLASH_FLAGS_FW_UPDATE_STATUS;

                memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

                flashParameters.flashEraseBlock.startFlashAddress  = startOffset;
                flashParameters.flashEraseBlock.eraseBlockSize     = SECTION_BOOTFIX_META_SIZE_IN_BLOCKS * blockSize;

                LOGG_PRINT(LOG_INFO_E, retCode, "Call SPI_FLASH_DRVP_eraseBlock()\n");

                retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E, &flashParameters);

                if (ERRG_FAILED(retCode))
                {
                    LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
                }
                else
                {
                    LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
                }

                memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

                flashParameters.flashProgram.startFlashAddress  = startOffset + sectionHeaderSize;
                flashParameters.flashProgram.writeFlashSize     = bootfixHeaderSize;
                flashParameters.flashProgram.flashBufP          = (UINT8 *)&bootfixMetadata;

                retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
                if (ERRG_FAILED(retCode))
                {
                    LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
                }
                else
                {
                    LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: Data - FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
                }

                memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

                flashParameters.flashProgram.startFlashAddress  = startOffset;
                flashParameters.flashProgram.writeFlashSize     = sectionHeaderSize;
                flashParameters.flashProgram.flashBufP          = (UINT8 *)&inuSectionHeader;

                retCode = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_PROGRAM_CMD_E, &flashParameters);
                if (ERRG_FAILED(retCode))
                {
                    LOGG_PRINT(LOG_ERROR_E, retCode, "fail: FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
                }
                else
                {
                    LOGG_PRINT(LOG_DEBUG_E, retCode, "SUCCESS: Data - FLASH COMMAND SPI_FLASH_DRVG_PROGRAM_CMD_E\n");
                }
            }
        }
    }
}

/****************************************************************************
*
*  Function Name: inu_fw_update__gpRawNorWrite
*
*  Description:
*
*  Inputs:
*
*  Outputs:
*
*  Returns:
*
*  Context: system service
*
****************************************************************************/
static ERRG_codeE inu_fw_update__gpRawNorWrite()
{
    ERRG_codeE ret = INU_DEVICE__RET_SUCCESS;
    inu_device_memory_t buf;
    UINT32 sectionSize = 0;
    SPI_FLASH_DRVG_flashParametersT flashParameters;
    UINT32 numOfBlocks = SPI_FLASHG_getNumBlocks(MAIN_FLASH);
    UINT32 sectionStart = SECTION_BOOTSPL_START_BLOCK;
    //UINT32 sectionStart = numOfBlocks/2;
    UINT32 blockSize = SPI_FLASHG_getBlockSize(MAIN_FLASH);
    UINT32 startFlashAddress = sectionStart * blockSize;
    UINT32 eraseBlockSize = (numOfBlocks/2 - sectionStart - 1) * blockSize;
    //UINT32 eraseBlockSize = (numOfBlocks/2 - SECTION_BOOTSPL_START_BLOCK - 1) * blockSize;
    InuStorageSectionTypeE sectionType;
    char flashFile[MAX_FILE_NAME];

#if (ERASE_ALL_FLASH == 1)
    memset(&flashParameters, 0x00, sizeof(SPI_FLASH_DRVG_flashParametersT));

    flashParameters.flashEraseBlock.startFlashAddress  = startFlashAddress;
    flashParameters.flashEraseBlock.eraseBlockSize     = eraseBlockSize;

    LOGG_PRINT(LOG_INFO_E, ret, "Call SPI_FLASH_DRVP_eraseBlock()\n");

    ret = IO_PALG_ioctl(IO_PALG_getHandle(IO_SPI_FLASH_1_E), SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E, &flashParameters);

    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "fail: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
    }
    else
    {
        LOGG_PRINT(LOG_INFO_E, ret, "SUCCESS: FLASH COMMAND SPI_FLASH_DRVG_ERASE_BLOCK_CMD_E\n");
    }
#endif

   for (sectionType = FW_UPDATE_SECTION_START; sectionType <= FW_UPDATE_SECTION_END; sectionType++)
   {
        LOGG_PRINT(LOG_INFO_E, ret, "\n\nburn [%s] to Flash\n\n", storageFileName[sectionType]);

        if (sectionType == INU_STORAGE_SECTION_CEVA)
        {
            continue;
        }

        LOGG_PRINT(LOG_INFO_E, NULL, "phyAddress = 0x%x virtAddress = 0x%x size = %d\n", workSpaceAddress[sectionType].phyAddress, workSpaceAddress[sectionType].virtAddress, workSpaceAddress[sectionType].size);

        inu_fw_update__gpWriteFlashSection(sectionType, (UINT8*)workSpaceAddress[sectionType].virtAddress, workSpaceAddress[sectionType].size, sectionStart, &sectionSize);

        // MEM_MAPG_free((INT32*)workSpaceAddress[sectionType].virtAddress);

        sectionStart += sectionSize;
        OS_LYRG_usleep(5000);
        LOGG_PRINT(LOG_INFO_E, ret,"sectionStart = %d, prevSectionSize = %d\n\n", sectionStart, sectionSize);
    }

    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, ret, "Flashing new FW Update ended with failure. Rebooting.\n");
    }
    else
    {
        inu_fw_update__writeMetaFwUpdate();
        LOGG_PRINT(LOG_INFO_E, NULL, "Flashing new FW Update ended successfully. Rebooting.\n");
    }
    WDG_cmd(0xFFFFFFFF);

    return ret;
}
#endif // DEFSG_IS_GP

ERRG_codeE inu_fw_update__execute(INU_FW_UPDATEG_messageT* msg, UINT8* output, UINT32* outputLen)
{
    ERRG_codeE  ret = INU_FW_UPDATE__RET_SUCCESS;
    UINT8*      fwUpdateInput = NULL;

    if (msg->hdr.msgLen)
    {
        fwUpdateInput = msg->data;
    }

    switch(msg->hdr.cmd)
    {
        case INU_FW_UPDATEG_GET_FW_VERSION_E:
        {
            ret = inu_fw_update__getFwVersion((UINT32 *)output);
            *outputLen = 4;
            break;
        }

        case INU_FW_UPDATEG_GET_BOOT_FLAGS_E:
        {
            ret = inu_fw_update__getBootFlags((UINT32*)output);
            *outputLen = 4;
            break;
        }

        case INU_FW_UPDATEG_FLASH_FW_IMAGE_E:
        {
            ret = inu_fw_update__flashFwImage();
            *outputLen = 0;
            break;
        }

        case INU_FW_UPDATEG_COPY_FW_IMAGE_E:
        {
            INU_FW_UPDATEG_copyFwImageParamsT copyFwImageParams;

            copyFwImageParams.fwUpdateInput = fwUpdateInput;
            copyFwImageParams.dataLen = msg->hdr.msgLen;
            copyFwImageParams.sectionId = msg->hdr.sectionId;
            copyFwImageParams.fwVersion = msg->hdr.fwVersionId;
            ret = inu_fw_update__copyFwImage(&copyFwImageParams);
            *outputLen = 0;
            break;
        }

        default:
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "command %d is not valid\n", msg->hdr.cmd);
            ret = INU_FW_UPDATE__ERR_INVALID_ARGS;
           *outputLen = 0;
        }
    }

    return ret;
}


ERRG_codeE inu_fw_update__getFwVersion(UINT32* fwVersion)
{
    ERRG_codeE ret = INU_FW_UPDATE__RET_SUCCESS;
    inu_device__fwVersionIdU fwVerId;
#if DEFSG_IS_GP

    // FW version
    fwVerId.fields.major    = MAJOR_VERSION;
    fwVerId.fields.minor    = MINOR_VERSION;
    fwVerId.fields.build    = BUILD_VERSION;
    fwVerId.fields.subBuild = SUB_BUILD_VERSION;

    *fwVersion = fwVerId.val;

    LOGG_PRINT(LOG_INFO_E, NULL, "FW GP version: %d.%d.%d.%d\n", fwVerId.fields.major, fwVerId.fields.minor, fwVerId.fields.build, fwVerId.fields.subBuild);

    LOGG_PRINT(LOG_INFO_E, NULL, "FW version %d\n", *fwVersion);

#else // HOST side
    INU_FW_UPDATEG_messageT   fwUpdateMsg;

    if (!fwVersion)
    {
        return CDC_MNGR__ERR_INVALID_ARGS;
    }

    fwUpdateMsg.hdr.cmd = INU_FW_UPDATEG_GET_FW_VERSION_E;
    fwUpdateMsg.hdr.msgLen = 0;
    ret = CDC_MNGRG_sendFwUpdateCmd(&fwUpdateMsg, (UINT8*)fwVersion, 4);

    fwVerId.val = *fwVersion;
    LOGG_PRINT(LOG_INFO_E, NULL, "FW HOST version: %d.%d.%d.%d \n", fwVerId.fields.major, fwVerId.fields.minor, fwVerId.fields.build, fwVerId.fields.subBuild);

#endif

    return ret;
}

ERRG_codeE inu_fw_update__getBootFlags(UINT32* bootFlags)
{
    ERRG_codeE ret = INU_FW_UPDATE__RET_SUCCESS;

#if DEFSG_IS_GP
    UINT32 GME_SAVE_AND_RESTORE_7_VAL = GME_DRVG_getSr7();
    if (GME_SAVE_AND_RESTORE_7_VAL == FW_UPDATE_FROM_ACTIVE_PARTITION)
    {
        *bootFlags = 0;
        LOGG_PRINT(LOG_DEBUG_E, NULL, "Boot from Active Partition.\n");
    }
    else if (GME_SAVE_AND_RESTORE_7_VAL == FW_UPDATE_FROM_BACKUP_PARTITION)
    {
        *bootFlags = 1;
        LOGG_PRINT(LOG_ERROR_E, NULL, "Boot from Backup Partition. FW Update procedure failed.\n");
    }
    else
    {
        *bootFlags = 0xFFFFFFFF;
    }
    LOGG_PRINT(LOG_DEBUG_E, NULL, "INU_FW_UPDATE_getBootFlags %d\n", *bootFlags);
#else // HOST side
    INU_FW_UPDATEG_messageT   fwUpdateMsg;

    if (!bootFlags)
    {
        return CDC_MNGR__ERR_INVALID_ARGS;
    }

    fwUpdateMsg.hdr.cmd = INU_FW_UPDATEG_GET_BOOT_FLAGS_E;
    fwUpdateMsg.hdr.msgLen = 0;
    ret = CDC_MNGRG_sendFwUpdateCmd(&fwUpdateMsg, (UINT8*)bootFlags, 4);

#endif

    return ret;
}

ERRG_codeE inu_fw_update__flashFwImage()
{
    ERRG_codeE ret = INU_FW_UPDATE__RET_SUCCESS;

#if DEFSG_IS_GP
   OS_LYRG_threadParams paramsP;

   paramsP.id           = OS_LYRG_FW_UPDATE_THREAD_ID_E;
   paramsP.func         = (OS_LYRG_threadFunction)inu_fw_update__gpRawNorWrite;
   paramsP.event        = NULL;
   paramsP.param        = NULL;

   paramsP.threadHandle = OS_LYRG_createThread(&paramsP);

    LOGG_PRINT(LOG_INFO_E, NULL, "Open FW Update Thread\n");
#else // HOST side
    INU_FW_UPDATEG_messageT   fwUpdateMsg;

    fwUpdateMsg.hdr.cmd = INU_FW_UPDATEG_FLASH_FW_IMAGE_E;
    fwUpdateMsg.hdr.msgLen = 0;
    ret = CDC_MNGRG_sendFwUpdateCmd(&fwUpdateMsg, NULL, 0);

#endif

    return ret;
}

#if DEFSG_IS_HOST
int inu_fw_update__archiveCopyData(struct archive* ar, struct archive* aw)
{
    int r;
    //const void* buff;
    const void* buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);
        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
        {
            //char* xxx;
            const char* xxx = archive_error_string(ar);
            return (r);
        }
        r = archive_write_data_block(aw, buff, size, offset);
        if (r < ARCHIVE_OK) {
            return (r);
        }
    }
}

bool inu_fw_update__archiveExtract(const char* filename, const char* destination)
{
    struct archive* a;
    struct archive* ext;
    struct archive_entry* entry;
    int flags;
    int r;

    /* Select which attributes we want to restore. */
    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    //archive_read_support_compression_all(a);
    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);
    if ((r = archive_read_open_filename(a, filename, 10240)))
        return true;
    for (;;)
    {
        r = archive_read_next_header(a, &entry);
        if (r == ARCHIVE_EOF)
            break;
        //if (r < ARCHIVE_OK)
        //  fprintf(stderr, "%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN)
            return true;
        //save to path
        const char* currentFile = archive_entry_pathname(entry);
        //const std::string fullOutputPath = destination + currentFile;
        char fullOutputPath[500];
        strcpy(fullOutputPath, destination);
        strcat(fullOutputPath, currentFile);
        archive_entry_set_pathname(entry, fullOutputPath);
        r = archive_write_header(ext, entry);
        //if (r < ARCHIVE_OK)
        //  fprintf(stderr, "%s\n", archive_error_string(ext));
        if (archive_entry_size(entry) > 0)
        {
            r = inu_fw_update__archiveCopyData(a, ext);
            if (r < ARCHIVE_WARN)
                return true;
        }

        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_WARN)
            return true;
    }
    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
    return false;
}
#endif

ERRG_codeE inu_fw_update__validateFwVersion (UINT32 sectionAddress, UINT32 sectionDataSize, InuStorageSectionTypeE sectionType, UINT32 fwVersion)
{
   ERRG_codeE ret = INU_FW_UPDATE__RET_SUCCESS;
   InuSecurityFooterT inuSecurityFooter = {0};

   memcpy(&inuSecurityFooter, (UINT32*)(sectionAddress + sectionDataSize - sizeof(InuSecurityFooterT)), sizeof(InuSecurityFooterT));

   if (fwVersion != inuSecurityFooter.fwVersion)
    {
        LOGG_PRINT(LOG_INFO_E, NULL, "ERROR! FW Version in %s [0x%x] is not equal to FW version [0x%x].\n", storageFileName[sectionType], inuSecurityFooter.fwVersion, fwVersion);
        ret = INU_FW_UPDATE__ERR_VERIFICATION;
    }

   return ret;
}

ERRG_codeE inu_fw_update__copyFwImage(INU_FW_UPDATEG_copyFwImageParamsT* copyFwImageParams)
{
    ERRG_codeE ret = INU_FW_UPDATE__RET_SUCCESS;
#if DEFSG_IS_GP
    size_t sizeSaved;
    static UINT16 filesRcvMask = 0;
    char filename[500] = FW_UPDATE_TEMP_DIRECTORY;
    InuStorageSectionTypeE sectionType = copyFwImageParams->sectionId;
    inu_fw_update_workSpaceAddress *wsAddress = &workSpaceAddress[sectionType];
    UINT32 fwUpdateSectionsBitMap = FW_UPDATE_SECTION_BOOTSPL_BIT +
                                    FW_UPDATE_SECTION_DTB_BIT +
                                    FW_UPDATE_SECTION_KERNEL_BIT +
                                    FW_UPDATE_SECTION_ROOTFS_BIT +
                                    FW_UPDATE_SECTION_CNN_LD_BIT +
                                    FW_UPDATE_SECTION_CNN_BIT +
                                    FW_UPDATE_SECTION_APP_BIT;
    UINT32 imageLen = copyFwImageParams->dataLen;
    UINT8* imageAddress = copyFwImageParams->fwUpdateInput;
    UINT8 ConfidentialityKeyType;
    UINT8* unsecuredImageAddress = NULL;;
    UINT8* securedImageAddress = NULL;
    UINT8 securityControl = 0;
    UINT32 eFuseArray[SB_TOTAL_NUM_BYTES / 4];
    UINT32 i;
    UINT8 eFuseArrayBytes[SB_TOTAL_NUM_BYTES];

    LOGG_PRINT(LOG_INFO_E, NULL, "INU_FW_UPDATEG_copyFwImage. sectionType %d size %d\n", sectionType, copyFwImageParams->dataLen);

    if(sectionType == FW_UPDATE_SECTION_START)
    {
#if READ_EFUSE
        //init db for verify
        GME_DRVP_deviceDescT GME_DRVP_deviceDesc;
        MEM_MAPG_addrT memVirtAddr;
        MEM_MAPG_getVirtAddr(MEM_MAPG_REG_GME_E, (&memVirtAddr));
        GME_DRVP_deviceDesc.deviceBaseAddress = (UINT32)memVirtAddr;
        securityControl = GME_DRVG_getFuseSecurityControl();
        //printf("securityControl %x\n",securityControl);
        for(i=0;i<(SB_TOTAL_NUM_BYTES / 4);i++)
        {
            //eFuseArray[i]=*(GME_FUSE_SHADOW_6_REG+i);
            eFuseArray[i] = GME_DRVG_getFuse32(6+i);
        }
        memcpy(eFuseArrayBytes, eFuseArray, SB_TOTAL_NUM_BYTES);
#endif
        fillSBEfuseDB(securityControl, eFuseArrayBytes);
    }

    //VERIFY
    ConfidentialityKeyType = getEfuseSecureControlData();
    LOGG_PRINT(LOG_INFO_E, NULL, "******************************* VERIFY *******************************\n");
    LOGG_PRINT(LOG_DEBUG_E, NULL, "ConfidentialityKeyType %d\n", ConfidentialityKeyType);
    if (ConfidentialityKeyType != NOT_SECURED_IMAGE)
    {
        UINT32 phyAddress;
        UINT32 virtAddress;

        ret = inu_fw_update__validateFwVersion ((UINT32)copyFwImageParams->fwUpdateInput, copyFwImageParams->dataLen, sectionType, copyFwImageParams->fwVersion);
        if (ERRG_FAILED(ret))
        {
            return INU_FW_UPDATE__ERR_VERIFICATION;
        }

        ret = MEM_MAPG_alloc(imageLen + SB_CHUNK_SIZE, &phyAddress, &virtAddress, 0);

        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, 0, "ERROR: Failed to init memory for inu_fw_update workspace (%d)\n", imageLen + SB_CHUNK_SIZE);
            return INU_FW_UPDATE__ERR_OUT_OF_MEM;
        }
        unsecuredImageAddress = (UINT8*)virtAddress;
        securedImageAddress = unsecuredImageAddress + SB_CHUNK_SIZE;
        memcpy(securedImageAddress, imageAddress, imageLen);

        ret = verifyFWupdate(securedImageAddress);

        MEM_MAPG_free((INT32*)virtAddress);

        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "SB FAIL ret = 0x%x\n", ret);
            return INU_FW_UPDATE__ERR_VERIFICATION;
        }
        else
        {
            LOGG_PRINT(LOG_INFO_E, NULL, "SB SUCCESS\n");
        }

        LOGG_PRINT(LOG_INFO_E, NULL, "******************************* COPY *******************************\n");
    }

    wsAddress->size = copyFwImageParams->dataLen;
    // EliG: Allocate double the size requested to avoid an unhandled Page error. Need to fix it properly.
    ret = MEM_MAPG_alloc((wsAddress->size * 2), &wsAddress->phyAddress, &wsAddress->virtAddress, 0);

    LOGG_PRINT(LOG_INFO_E, NULL, "phyAddress = 0x%x virtAddress = 0x%x size = %d\n", wsAddress->phyAddress, wsAddress->virtAddress, wsAddress->size);

    if (ERRG_FAILED(ret))
    {
        LOGG_PRINT(LOG_ERROR_E, 0, "ERROR: Failed to init memory for inu_fw_update workspace (%d)\n", copyFwImageParams->dataLen);
        return INU_FW_UPDATE__ERR_OUT_OF_MEM;
    }

    memcpy((UINT8*)wsAddress->virtAddress, copyFwImageParams->fwUpdateInput, wsAddress->size);


    filesRcvMask += (1 << sectionType);

    //all fw update files recieved and verified
    if (filesRcvMask == fwUpdateSectionsBitMap)
    {
        char buff[1024];
        UINT32 overallUpdateSize = 0;
        UINT32 i = 0;

        for (sectionType = FW_UPDATE_SECTION_START; sectionType <= FW_UPDATE_SECTION_END; sectionType++)
        {
            // FILE* file;
            if (sectionType == INU_STORAGE_SECTION_CEVA)
            {
                continue;
            }

            overallUpdateSize += workSpaceAddress[sectionType].size;
        }

        LOGG_PRINT(LOG_DEBUG_E, NULL, "overallUpdateSize: %d\n", overallUpdateSize);

        if (overallUpdateSize > FW_UPDATE_FILES_MAX_SIZE)
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "overallUpdateSize: %d exceeds maximum allowed size = %d\n", overallUpdateSize, FW_UPDATE_FILES_MAX_SIZE);
            return INU_FW_UPDATE__ERR_OUT_OF_MEM;
        }
        // Update global FW Version ID, once all files are verified:
        gFwVersionId.val = copyFwImageParams->fwVersion;
        LOGG_PRINT(LOG_INFO_E, NULL, "FW Update new GP version: %d.%d.%d.%d\n", gFwVersionId.fields.major, gFwVersionId.fields.minor, gFwVersionId.fields.build, gFwVersionId.fields.subBuild);
    }

#else // HOST side
    INU_FW_UPDATEG_messageT*  fwUpdateMsg;
    FILE* file;
    UINT32 fwImagebufsize;
    UINT8* fwImagebufData;
    UINT32 retVal = 0;
    char lpTempPathBuffer[MAX_PATH];
    UINT32 sectionType;
    bool sysRet = 0;

    retVal = GetTempPath(MAX_PATH,          // length of the buffer
        lpTempPathBuffer); // buffer for path
    if (retVal > MAX_PATH || (retVal == 0))
    {
        printf("GetTempPath failed\n");
        return INU_FW_UPDATE__ERR_INVALID_PATH;
    }
    strcat(lpTempPathBuffer, "InuitiveUpdate\\");

    sysRet = inu_fw_update__archiveExtract(copyFwImageParams->filebin, lpTempPathBuffer);
    if (sysRet)
    {
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to unzip FW Update zip file\n");
        return INU_FW_UPDATE__ERR_INVALID_BIN_FILE;
    }

    for (sectionType = FW_UPDATE_SECTION_START; sectionType <= FW_UPDATE_SECTION_END; sectionType++)
    {
        if (sectionType == INU_STORAGE_SECTION_CEVA)
        {
            continue;
        }
        char filename[500];
        strcpy(filename, lpTempPathBuffer);
        strcat(filename, PATH_SEPARATOR);
        strcat(filename, storageFileName[sectionType]);
        file = fopen(filename, "rb");
        if (file)
        {
            fseek(file, 0, SEEK_END);
            fwImagebufsize = ftell(file);
            fseek(file, 0, SEEK_SET);
        }
        else
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "Could not find file %s\n", filename);
            return INU_FW_UPDATE__ERR_UNEXPECTED;
        }

        fwImagebufData = (UINT8*)malloc(fwImagebufsize + sizeof(fwUpdateMsg->hdr));

        fwUpdateMsg = (INU_FW_UPDATEG_messageT*)fwImagebufData;
        fwUpdateMsg->hdr.cmd = INU_FW_UPDATEG_COPY_FW_IMAGE_E;
        fwUpdateMsg->hdr.sectionId = sectionType;
        fwUpdateMsg->hdr.fwVersionId = copyFwImageParams->fwVersion;
        fwUpdateMsg->hdr.msgLen = fwImagebufsize;
        fread(fwUpdateMsg->data, 1, fwImagebufsize, file);
        fclose(file);

        ret = CDC_MNGRG_sendFwUpdateCmd(fwUpdateMsg, NULL, 0);
        if (ERRG_FAILED(ret))
        {
            LOGG_PRINT(LOG_ERROR_E, NULL, "fw update copy error in %s\n", storageFileName[sectionType]);
            return ret;
        }
    }

#endif

    return ret;
}



#endif