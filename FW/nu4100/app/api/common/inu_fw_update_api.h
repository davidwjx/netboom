
#ifndef __INU_FW_UPDATE_API_H__
#define __INU_FW_UPDATE_API_H__

#include "inu2.h"

#ifdef __cplusplus
    extern "C" {
#endif

#define INU_FW_UPDATEG_MAX_MSG_LENGTH      32

typedef enum
{
    INU_FW_UPDATEG_COPY_FW_IMAGE_E = 20,    // 20
    INU_FW_UPDATEG_FLASH_FW_IMAGE_E,        // 21
    INU_FW_UPDATEG_GET_BOOT_FLAGS_E,        // 22
    INU_FW_UPDATEG_GET_FW_VERSION_E,        // 23
}INU_FW_UPDATEG_messageCmdE;

typedef struct
{
    INU_FW_UPDATEG_messageCmdE        cmd;
    UINT32                            sectionId;
    UINT32                            fwVersionId;
    UINT32                            msgLen;
} INU_FW_UPDATEG_messageHdrT;

typedef struct
{
    INU_FW_UPDATEG_messageHdrT        hdr;
    UINT8                             data[INU_FW_UPDATEG_MAX_MSG_LENGTH];
} INU_FW_UPDATEG_messageT;

typedef struct
{
    char*                             filebin;
    UINT32                            fwVersion;
    UINT8                             securityVersion;
    UINT8*                            fwUpdateInput;
    UINT32                            dataLen;
    UINT32                            sectionId;
} INU_FW_UPDATEG_copyFwImageParamsT;

ERRG_codeE inu_fw_update__execute(INU_FW_UPDATEG_messageT* msg, UINT8* output, UINT32* outputLen);
ERRG_codeE inu_fw_update__copyFwImage(INU_FW_UPDATEG_copyFwImageParamsT *copyFwImageParams);
ERRG_codeE inu_fw_update__flashFwImage();
ERRG_codeE inu_fw_update__getBootFlags(UINT32* bootFlags);
ERRG_codeE inu_fw_update__getFwVersion(UINT32* fwVersion);

#ifdef __cplusplus
}
#endif

#endif //__INU_FW_UPDATE_API_H__

