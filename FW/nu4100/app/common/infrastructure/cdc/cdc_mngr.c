
/****************************************************************************
 *
 *   FileName: cdc_mngr.c
 *
 *   Author:  Oshri A.
 *
 *   Date:
 *
 *   Description: Control CDC sequences
 *
 ****************************************************************************/

/****************************************************************************
 ***************               I N C L U D E   F I L E S        *************
 ****************************************************************************/
#include "assert.h"
#include "stdarg.h"

#include "inu_common.h"
#include "cdc_mngr.h"
#include "ve_mngr.h"
#include "inu_fw_update_api.h"
#include "cdc_serial.h"
#include "log.h"

#if DEFSG_IS_GP
#include "inu_graph.h"
#include "wd.h"
#endif

/****************************************************************************
 ***************         L O C A L       D E F N I T I O N S  ***************
 ****************************************************************************/
#define CDC_MNGRP_RECEIVE_TIMEOUT               (1000)
#define CDC_MNGRP_EXECUTION_TIMEOUT             (8000)
#define CDC_MNGRP_SEND_TIMEOUT                  (2000)
#define CDC_MNGRP_LONG_MSG_SLEEP_US             (10000)

#define CDC_MNGRP_VALID_CMD_TAG                 (0xA5A5A5A5)

#define CDC_MNGRP_MAX_DATA_LENGTH               (8*1024)
#define CDC_MNGRP_MAX_STR_LOG_LENGTH            (2000)
#define CDC_MNGRP_MAX_SYS_MSG_LENGTH            (32)
#define CDC_MNGRP_MAX_RESP_LENGTH               (64)

/****************************************************************************
 ***************            L O C A L    T Y P E D E F S      ***************
 ****************************************************************************/
typedef enum
{
    CDC_MNGRP_INCOMING_RESP_E = 0,
    CDC_MNGRP_READ_RESP_E,
    CDC_MNGRP_NUM_OF_EVENTS_E,
} CDC_MNGRP_msgEventE;

typedef struct
{
    PUART                       puart;
} CDC_MNGRP_handleT;

typedef enum
{
    CDC_MNGRP_SYS_CMD_PING_E = 0,
    CDC_MNGRP_SYS_CMD_EN_LOG_E,
    CDC_MNGRP_SYS_CMD_SW_RESET_E,
    CDC_MNGRP_SYS_CMD_MAX_E,
} CDC_MNGRP_sysMsgCmdE;

typedef struct
{
    CDC_MNGRP_sysMsgCmdE        cmd;
    UINT32                      msgLen;
} CDC_MNGRP_sysMsgHdrT;

typedef struct
{
    CDC_MNGRP_sysMsgHdrT        hdr;
    UINT8                       data[CDC_MNGRP_MAX_SYS_MSG_LENGTH];
} CDC_MNGRP_sysMsgT;

typedef struct
{
    ERRG_codeE                 retVal;
    UINT32                     respLen;
} CDC_MNGRP_responseHdrT;

typedef struct
{
    CDC_MNGRP_responseHdrT     hdr;
    UINT8                      data[CDC_MNGRP_MAX_RESP_LENGTH];
} CDC_MNGRP_responseT;


typedef enum
{
    CDC_MNGRG_CMD_SYS_E = 0,
    CDC_MNGRG_CMD_LOG_E,
    CDC_MNGRG_CMD_RESPONSE_E,
    CDC_MNGRG_CMD_VE_E,
    CDC_MNGRG_CMD_UPDATE_E,
    CDC_MNGRG_CMD_LONG_MSG_E,
    CDC_MNGRG_CMD_MAX_E,
} CDC_MNGRP_cmdTagE;

typedef struct
{
    UINT32                      tag;
    CDC_MNGRP_cmdTagE           cmd;
    UINT32                      msgLen;
} CDC_MNGRP_msgHdrT;

typedef struct
{
    CDC_MNGRP_msgHdrT           hdr;
    UINT8*                      data;
} CDC_MNGRP_msgT;

#define CDC_MNGRP_LOGG_PRINT(level, errCode, ...) CDC_MNGRP_outputStrLogCdc(CURR_PID, (INU_DEFSG_logLevelE)level, (ERRG_codeE)errCode, _FILE_, __func__, __LINE__, __VA_ARGS__)


/****************************************************************************
 ***************       L O C A L         D A T A              ***************
 ****************************************************************************/
static OS_LYRG_threadParams         cdcThreadRxParams;
static CDC_MNGRP_responseT*         cdcResponse;
static BOOL                         cdcThreadIsActive;
static CDC_MNGRP_handleT            cdcPrivHandle;
static OS_LYRG_mutexT               cdcWriteMutex;
static OS_LYRG_event                cdcEvents[CDC_MNGRP_NUM_OF_EVENTS_E];

static const INU_DEFSG_logLevelE    cdcDbgLevel = LOG_INFO_E;
static const char*                  cdcPortName = "/dev/ttyGS0";

#if DEFSG_IS_HOST
static CDC_MNGRG_logCallback        cdcLogCallback;
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
 ***************     L O C A L         F U N C T I O N S      ***************
 ****************************************************************************/
static void CDC_MNGRP_outputStrLogCdc(INU_DEFSG_pidE pid, INU_DEFSG_logLevelE level, ERRG_codeE errCode, const char *fileP, const char *funcP, UINT32 lineNum, const char* fmt, ...)
{
    char    logStr[CDC_MNGRP_MAX_STR_LOG_LENGTH];
    va_list args;

    FIX_UNUSED_PARAM_WARN(errCode);

    if (level <= cdcDbgLevel)
    {
        UINT32 sec;
        UINT16 msec;

        OS_LYRG_getTime(&sec, &msec);

        //        title  pid      timestamp          file   line     func
        printf("%s (%s):        %u.%03u sec     %s (%d),        %s()             ", "CDC MNGR LOG",
                                                                                    pid == GP_ID ? "GP" : "HOST",
                                                                                    (UINT16)sec, msec,
                                                                                    fileP, lineNum, funcP);
        va_start(args,fmt);
        vsprintf(logStr ,fmt, args);
        printf(logStr);
        va_end(args);
    }
}

static ERRG_codeE CDC_MNGRP_pullResponse(CDC_MNGRP_responseT* resp)
{
    ERRG_codeE   ret = CDC_MNGR__RET_SUCCESS;

    CDC_MNGRP_LOGG_PRINT(LOG_DEBUG_E, NULL, "wait for response...\n");

    if (OS_LYRG_waitEvent(cdcEvents[CDC_MNGRP_INCOMING_RESP_E], CDC_MNGRP_EXECUTION_TIMEOUT) != 0)
    {
        CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to receive response\n");
        return CDC_MNGR__ERR_TIMEOUT;
    }

    CDC_MNGRP_LOGG_PRINT(LOG_DEBUG_E, NULL, "response is ready (len = %d, ret = 0x%x)\n", cdcResponse->hdr.respLen, cdcResponse->hdr.retVal);

    if (resp)
    {
        if ((sizeof(cdcResponse->hdr) + cdcResponse->hdr.respLen) <= resp->hdr.respLen)
        {
            memcpy(resp, cdcResponse, sizeof(resp->hdr) + resp->hdr.respLen);
        }
        else
        {
            CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "illegal response\n");
            ret = VE_MNGR__ERR_INVALID_ARGS;
        }
    }
    cdcResponse = NULL;

    OS_LYRG_setEvent(cdcEvents[CDC_MNGRP_READ_RESP_E]);

    return ret;
}

static ERRG_codeE CDC_MNGRP_sendMsg(CDC_MNGRP_msgT* msg)
{
    ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

    CDC_MNGRP_LOGG_PRINT(LOG_DEBUG_E, NULL, "tag = 0x%x, cmd = 0x%x, msgLen = %d\n", msg->hdr.tag, msg->hdr.cmd, msg->hdr.msgLen);

    ret = CDC_SERIALG_send(&cdcPrivHandle.puart, (INT8*)&msg->hdr, sizeof(msg->hdr), CDC_MNGRP_SEND_TIMEOUT);
    if (ERRG_FAILED(ret))
    {
        CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to send msg\n");
    }
    else if(msg->hdr.msgLen && msg->data)
    {
        ret = CDC_SERIALG_send(&cdcPrivHandle.puart, (INT8*)msg->data, msg->hdr.msgLen, CDC_MNGRP_SEND_TIMEOUT);
        if (ERRG_FAILED(ret))
        {
            CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to send msg\n");
        }
    }
    return ret;
}

static ERRG_codeE CDC_MNGRP_sendLongMsg(CDC_MNGRP_msgT* msg)
{
    ERRG_codeE     ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT longMsg;

    CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "send long msg (%d bytes)\n", msg->hdr.msgLen);

    longMsg.hdr.tag = CDC_MNGRP_VALID_CMD_TAG;
    longMsg.hdr.cmd = CDC_MNGRG_CMD_LONG_MSG_E;
    longMsg.hdr.msgLen = sizeof(msg->hdr);
    longMsg.data = (UINT8*)&msg->hdr;

    ret = CDC_MNGRP_sendMsg(&longMsg);
    if (ERRG_SUCCEEDED(ret) && msg->data)
    {
        UINT8* dataP = (UINT8*)msg->data;
        UINT32 numOfChunks;
        UINT32 size = msg->hdr.msgLen;
        UINT32 i;

        numOfChunks = (size + CDC_MNGRP_MAX_DATA_LENGTH - 1) / CDC_MNGRP_MAX_DATA_LENGTH;

        for (i = 0; i < numOfChunks; i++)
        {
            msg->hdr.msgLen = MIN(CDC_MNGRP_MAX_DATA_LENGTH, size);
            msg->data = dataP;

            ret = CDC_MNGRP_sendMsg(msg);
            if (ERRG_FAILED(ret))
            {
                break;
            }

            dataP += msg->hdr.msgLen;
            size  -= msg->hdr.msgLen;

            if (i % 1000 == 0)
            {
                CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "chunk %d sent succefully\n", i);
            }

            OS_LYRG_usleep(CDC_MNGRP_LONG_MSG_SLEEP_US);
        }

        CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "done sending long msg (%d / %d chunks)\n", i, numOfChunks);
    }
    return ret;
}

static ERRG_codeE CDC_MNGRP_sendCmd(CDC_MNGRP_msgT* msg, CDC_MNGRP_responseT* resp)
{
    ERRG_codeE ret = (ERRG_codeE)RET_SUCCESS;

    OS_LYRG_lockMutex(&cdcWriteMutex);

    if (msg->hdr.msgLen > CDC_MNGRP_MAX_DATA_LENGTH)
    {
        ret = CDC_MNGRP_sendLongMsg(msg);
    }
    else
    {
        ret = CDC_MNGRP_sendMsg(msg);
    }

    OS_LYRG_unlockMutex(&cdcWriteMutex);

    if (ERRG_SUCCEEDED(ret) && resp)
    {
        ret = CDC_MNGRP_pullResponse(resp);
        if (ERRG_SUCCEEDED(ret))
        {
            ret = resp->hdr.retVal;
        }
    }

    return ret;
}

static void CDC_MNGRP_sendResponse(CDC_MNGRP_responseT* resp)
{
    ERRG_codeE     ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT msg;

    msg.hdr.tag = CDC_MNGRP_VALID_CMD_TAG;
    msg.hdr.cmd = CDC_MNGRG_CMD_RESPONSE_E;
    msg.hdr.msgLen = sizeof(CDC_MNGRP_responseHdrT) + resp->hdr.respLen;
    msg.data = (UINT8*)resp;

    ret = CDC_MNGRP_sendCmd(&msg, NULL);
    if(ERRG_FAILED(ret))
    {
        CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to send response\n");
    }
}

static ERRG_codeE CDC_MNGRP_pullLongMsg(CDC_MNGRP_msgT* msg)
{
    ERRG_codeE  ret = CDC_MNGR__RET_SUCCESS;
    UINT8*      pBuff;
    UINT32      numOfChunks;
    UINT32      readBytes;
    UINT32      i;

    // receive original CDC header
    ret = CDC_SERIALG_recv(&cdcPrivHandle.puart, (UINT8*)&msg->hdr, sizeof(CDC_MNGRP_msgHdrT), &readBytes, CDC_MNGRP_RECEIVE_TIMEOUT);
    if(ERRG_FAILED(ret))
    {
        CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read massage data\n");
        return CDC_MNGR__ERR_IO_ERROR;
    }

    msg->data = (UINT8*)malloc(msg->hdr.msgLen);
    if (!msg->data)
    {
        CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "allocation failed\n");
        return CDC_MNGR__ERR_OUT_OF_MEM;
    }

    pBuff = msg->data;
    numOfChunks = (msg->hdr.msgLen + CDC_MNGRP_MAX_DATA_LENGTH - 1) / CDC_MNGRP_MAX_DATA_LENGTH;

    CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "start pulling long msg cmd = 0x%x, msgLen = %d (%d chunks)\n", msg->hdr.cmd, msg->hdr.msgLen, numOfChunks);

    // receive data in chunks
    for (i = 0; i < numOfChunks; i++)
    {
        CDC_MNGRP_msgHdrT chunkHdr;

        ret = CDC_SERIALG_recv(&cdcPrivHandle.puart, (UINT8*)&chunkHdr, sizeof(chunkHdr), &readBytes, CDC_MNGRP_RECEIVE_TIMEOUT);
        if(ERRG_FAILED(ret))
        {
            CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read massage header (received %d)\n", readBytes);
            break;
        }

        ret = CDC_SERIALG_recv(&cdcPrivHandle.puart, pBuff, chunkHdr.msgLen, &readBytes, CDC_MNGRP_RECEIVE_TIMEOUT);
        if(ERRG_FAILED(ret))
        {
            CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read massage data (received %d)\n", readBytes);
            break;
        }

        pBuff += chunkHdr.msgLen;

        if (i % 5000 == 0)
        {
            CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "chunk %d received succefully\n", i);
        }
    }

    CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "done pulling long msg (%d/%d chunks)\n", i, numOfChunks);

    return ret;
}

static ERRG_codeE CDC_MNGRP_pullMsg(CDC_MNGRP_msgT* msg)
{
    ERRG_codeE                  ret = CDC_MNGR__RET_SUCCESS;
    ERRG_codeE                  retVal = CDC_MNGR__RET_SUCCESS;
    UINT8*                      pBuff;
    UINT32                      len = 0;
    UINT32                      readBytes = 0;

    pBuff = (UINT8*)&msg->hdr;
    while(len < sizeof(CDC_MNGRP_msgHdrT))
    {
        ret = CDC_SERIALG_recv(&cdcPrivHandle.puart, &pBuff[len], sizeof(CDC_MNGRP_msgHdrT), &readBytes, CDC_MNGRP_RECEIVE_TIMEOUT);
        if (ERRG_FAILED(ret) && readBytes == 0)
        {
            if (!cdcThreadIsActive)
            {
                return VE_MNGR__ERR_ILLEGAL_STATE;
            }
            continue;
        }
        else if (msg->hdr.tag != CDC_MNGRP_VALID_CMD_TAG)
        {
            UINT8*              pBuffEnd;
            CDC_MNGRP_msgHdrT*  hdr;

            CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "invalid tag, trying to sync\n");

            pBuff = (UINT8*)msg->data;
            pBuffEnd = pBuff + CDC_MNGRP_MAX_DATA_LENGTH;

            ret = CDC_SERIALG_recv(&cdcPrivHandle.puart, &pBuff[0], 4, &readBytes, 1);
            pBuff += 4;

            while (ERRG_SUCCEEDED(ret))
            {
                hdr = (CDC_MNGRP_msgHdrT*)(pBuff - 4);
                if (hdr->tag == CDC_MNGRP_VALID_CMD_TAG)
                {
                    CDC_MNGRP_LOGG_PRINT(LOG_WARN_E, NULL, "now is sync, some messages might have been lost\n");
                    retVal = CDC_SERIALG_recv(&cdcPrivHandle.puart, (UINT8*)&msg->hdr.cmd, sizeof(CDC_MNGRP_msgHdrT) - 4, &readBytes, CDC_MNGRP_RECEIVE_TIMEOUT);
                    break;
                }
                else if (pBuff >= pBuffEnd)
                {
                    CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "sync failed\n");
                    retVal = VE_MNGR__ERR_ILLEGAL_STATE;
                    break;
                }
                retVal = CDC_SERIALG_recv(&cdcPrivHandle.puart, pBuff++, 1, &readBytes, 1);
            }
            break;
        }

        len += readBytes;
    }

    CDC_MNGRP_LOGG_PRINT(LOG_DEBUG_E, NULL, "tag = 0x%x, cmd = 0x%x, msgLen = %d\n", msg->hdr.tag, msg->hdr.cmd, msg->hdr.msgLen);

    if (ERRG_SUCCEEDED(retVal) && msg->hdr.msgLen)
    {
        if (msg->hdr.cmd == CDC_MNGRG_CMD_LONG_MSG_E)
        {
            return CDC_MNGRP_pullLongMsg(msg);
        }

        len = 0;
        pBuff = (UINT8*)msg->data;
        while (len < msg->hdr.msgLen)
        {
            ret = CDC_SERIALG_recv(&cdcPrivHandle.puart, &pBuff[len], msg->hdr.msgLen, &readBytes, CDC_MNGRP_RECEIVE_TIMEOUT);
            if (ERRG_FAILED(ret) && readBytes == 0)
            {
                continue;
            }
            else if (ERRG_FAILED(ret))
            {
                retVal = ret;
                CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to read massage data\n");
                break;
            }
            len += readBytes;
        }
    }
    else
    {
        msg->data = NULL;
    }

    return retVal;
}

#if DEFSG_IS_GP
static ERRG_codeE CDC_MNGRP_sysCmdHandler(CDC_MNGRP_sysMsgT* sysMsg, CDC_MNGRP_responseT* resp)
{
    ERRG_codeE                  ret = CDC_MNGR__RET_SUCCESS;
    UINT8*                      pBuff;
    UINT32                      len = 0;
    UINT32                      readBytes = 0;

    if(!sysMsg)
    {
        return ret;
    }

    switch(sysMsg->hdr.cmd)
    {
        case CDC_MNGRP_SYS_CMD_PING_E:
        {
            CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "ping received\n");
            resp->hdr.retVal = CDC_MNGR__RET_SUCCESS;
            resp->hdr.respLen = 0;

            break;
        }

        case CDC_MNGRP_SYS_CMD_SW_RESET_E:
        {
            CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "sw reset by WD\n");
            resp->hdr.retVal = WDG_cmd(0xFFFFFFFF);
            resp->hdr.respLen = 0;

            break;
        }

        case CDC_MNGRP_SYS_CMD_EN_LOG_E:
        {
            BOOLEAN enable = *(BOOLEAN*)sysMsg->data;

            CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "cdc log %s\n", enable ? "enabled" : "disabled");
            LOGG_setSendLogCdcFlag(enable);
            resp->hdr.retVal = CDC_MNGR__RET_SUCCESS;
            resp->hdr.respLen = 0;

            break;
        }

        default:
        {
            CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "system command %d is not valid\n", sysMsg->hdr.cmd);

            resp->hdr.retVal = VE_MNGR__ERR_INVALID_ARGS;
            resp->hdr.respLen = 0;
        }
    }

    return ret;
}

#endif
static int CDC_MNGRP_rxThread(void *params)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT      msg;
    CDC_MNGRP_responseT resp;
    UINT8               dataInput[CDC_MNGRP_MAX_DATA_LENGTH];

    cdcThreadIsActive = TRUE;

    cdcEvents[CDC_MNGRP_INCOMING_RESP_E] = OS_LYRG_createEvent(CDC_MNGRP_INCOMING_RESP_E);
    cdcEvents[CDC_MNGRP_READ_RESP_E]     = OS_LYRG_createEvent(CDC_MNGRP_READ_RESP_E);

    while(cdcThreadIsActive)
    {
        msg.data = dataInput;
        resp.hdr.respLen = sizeof(resp.data);

        ret = CDC_MNGRP_pullMsg(&msg);
        if(ERRG_SUCCEEDED(ret))
        {
            CDC_MNGRP_LOGG_PRINT(LOG_DEBUG_E, NULL, "received cdc command 0x%x\n", msg.hdr.cmd);

            switch(msg.hdr.cmd)
            {
                case CDC_MNGRG_CMD_RESPONSE_E:
                {
                    cdcResponse = (CDC_MNGRP_responseT*)msg.data;
                    OS_LYRG_setEvent(cdcEvents[CDC_MNGRP_INCOMING_RESP_E]);
                    OS_LYRG_waitEvent(cdcEvents[CDC_MNGRP_READ_RESP_E], CDC_MNGRP_RECEIVE_TIMEOUT);
                    break;
                }
#if DEFSG_IS_GP
#ifdef ENABLE_VISION_EFFECT
                case CDC_MNGRG_CMD_VE_E:
                {
                    resp.hdr.retVal = VE_MNGRG_execute((VE_MNGRG_messageT*)msg.data, resp.data, &resp.hdr.respLen);
                    CDC_MNGRP_sendResponse(&resp);
                    break;
                }
#endif
#ifdef ENABLE_FW_UPDATE
                case CDC_MNGRG_CMD_UPDATE_E:
                {
                    resp.hdr.retVal = inu_fw_update__execute((INU_FW_UPDATEG_messageT*)msg.data, resp.data, &resp.hdr.respLen);
                    CDC_MNGRP_sendResponse(&resp);
                    break;
                }
#endif
                case CDC_MNGRG_CMD_SYS_E:
                {
                    resp.hdr.retVal = CDC_MNGRP_sysCmdHandler((CDC_MNGRP_sysMsgT*)msg.data, &resp);
                    CDC_MNGRP_sendResponse(&resp);
                    break;
                }
#else
                case CDC_MNGRG_CMD_LOG_E:
                {
                    (void)CDC_MNGRG_log((const char*)msg.data);
                    break;
                }
#endif
                default:
                {
                    CDC_MNGRP_LOGG_PRINT(LOG_WARN_E, NULL, "wrong CDC command (0x%x)\n", msg.hdr.cmd);
                }
            }

            // free allocated buffer
            if (msg.data && msg.data != dataInput)
            {
                free(msg.data);
            }
        }
    }
    return ret;
}

/****************************************************************************
 ***************        G L O B A L           F U N C T I O N S   ***************
 ****************************************************************************/
ERRG_codeE CDC_MNGRG_init(void)
{
    ERRG_codeE ret = CDC_MNGR__RET_SUCCESS;

    CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "CDC initialization\n");

    memset(&cdcPrivHandle, 0, sizeof(cdcPrivHandle));
    memset(&cdcThreadRxParams, 0, sizeof(cdcThreadRxParams));

    cdcThreadIsActive = FALSE;

#if DEFSG_IS_HOST
    cdcLogCallback = NULL;
#endif

    return ret ;
}

ERRG_codeE CDC_MNGRG_deinit(void)
{
    ERRG_codeE ret = CDC_MNGR__RET_SUCCESS;

    CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "CDC deinit\n");

    cdcThreadIsActive = FALSE;

    if (cdcThreadRxParams.threadHandle)
    {
        OS_LYRG_waitForThread(cdcThreadRxParams.threadHandle, OS_LYRG_INFINITE);
        cdcThreadRxParams.threadHandle = NULL;
    }
    return ret ;
}

ERRG_codeE CDC_MNGRG_open(CDC_MNGRG_handle* handle, UINT32 port, UINT32 baudrate)
{
    ERRG_codeE                  ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_handleT*      privHandle = (CDC_MNGRP_handleT*)&cdcPrivHandle;
#if DEFSG_IS_GP
    char* portName = (char*)cdcPortName;
    FIX_UNUSED_PARAM_WARN(port);
#else
    char portName[32];
    sprintf(portName, "%d", port);
    //sprintf(portName, "COM%d", port);
#endif

    ret = CDC_SERIALG_open(&privHandle->puart, portName, baudrate, SERIALG_TYPE_CDC);
    if(ERRG_FAILED(ret))
    {
        CDC_MNGRP_LOGG_PRINT(LOG_ERROR_E, NULL, "failed to open start CDC listener\n");
    }
    else
    {
        cdcThreadRxParams.func = (OS_LYRG_threadFunction)CDC_MNGRP_rxThread;
        cdcThreadRxParams.param = NULL;
        cdcThreadRxParams.id = OS_LYRG_CDC_MNGR_THREAD_ID_E;
        cdcThreadRxParams.threadHandle = OS_LYRG_createThread(&cdcThreadRxParams);
        if(!cdcThreadRxParams.threadHandle)
        {
            ret = CDC_MNGR__ERR_THREAD_ERROR;
        }
        else
        {
            CDC_MNGRP_LOGG_PRINT(LOG_INFO_E, NULL, "start CDC listener\n");
        }
    }

    if(handle)
    {
        *handle = privHandle;
    }

    OS_LYRG_aquireMutex(&cdcWriteMutex);

    return ret ;
}

ERRG_codeE CDC_MNGRG_close(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_handleT* privHandle;

    FIX_UNUSED_PARAM_WARN(handle);

    if (handle)
    {
        privHandle = (CDC_MNGRP_handleT*)handle;
    }
    else
    {
        privHandle = (CDC_MNGRP_handleT*)&cdcPrivHandle;
    }

    if (privHandle)
    {
        ret = CDC_SERIALG_close(&privHandle->puart);
    }
    return ret ;
}

#if DEFSG_IS_GP
ERRG_codeE CDC_MNGRG_log(const char *str)
{
    ERRG_codeE                      ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT              msg;

    msg.hdr.tag = CDC_MNGRP_VALID_CMD_TAG;
    msg.hdr.cmd = CDC_MNGRG_CMD_LOG_E;
    msg.hdr.msgLen = strlen(str) + 1;
    msg.data = (UINT8*)str;

    ret = CDC_MNGRP_sendCmd(&msg, NULL);

    return ret;
}

#else // HOST side
ERRG_codeE CDC_MNGRG_log(const char *str)
{
    ERRG_codeE ret = CDC_MNGR__RET_SUCCESS;

    if (cdcLogCallback)
    {
        cdcLogCallback(str);
    }
    else
    {
        printf(str);
    }

    return ret;
}

static ERRG_codeE CDC_MNGRG_sendSysCmd(CDC_MNGRP_sysMsgT* sysMsg, CDC_MNGRP_responseT* resp)
{
    ERRG_codeE      ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT  msg;

    msg.hdr.tag = CDC_MNGRP_VALID_CMD_TAG;
    msg.hdr.cmd = CDC_MNGRG_CMD_SYS_E;
    msg.hdr.msgLen = sizeof(sysMsg->hdr) + sysMsg->hdr.msgLen;
    msg.data = (UINT8*)sysMsg;

    ret = CDC_MNGRP_sendCmd(&msg, resp);

    return ret;
}

ERRG_codeE CDC_MNGRG_ping(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_sysMsgT   sysMsg;
    CDC_MNGRP_responseT resp;

    FIX_UNUSED_PARAM_WARN(handle);

    sysMsg.hdr.cmd = CDC_MNGRP_SYS_CMD_PING_E;
    sysMsg.hdr.msgLen = 0;

    resp.hdr.respLen = sizeof(resp.data);

    ret = CDC_MNGRG_sendSysCmd(&sysMsg, &resp);

    return ret;
}

ERRG_codeE CDC_MNGRG_swReset(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_sysMsgT   sysMsg;
    CDC_MNGRP_responseT resp;

    FIX_UNUSED_PARAM_WARN(handle);

    sysMsg.hdr.cmd = CDC_MNGRP_SYS_CMD_SW_RESET_E;
    sysMsg.hdr.msgLen = 0;

    resp.hdr.respLen = sizeof(resp.data);

    ret = CDC_MNGRG_sendSysCmd(&sysMsg, &resp);

    return ret;
}

ERRG_codeE CDC_MNGRG_logEnable(CDC_MNGRG_handle handle, BOOLEAN enable, CDC_MNGRG_logCallback logCallback)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_sysMsgT   sysMsg;
    CDC_MNGRP_responseT resp;

    FIX_UNUSED_PARAM_WARN(handle);

    sysMsg.hdr.cmd = CDC_MNGRP_SYS_CMD_EN_LOG_E;
    sysMsg.hdr.msgLen = sizeof(BOOLEAN);
    *(BOOLEAN*)sysMsg.data = enable;

    resp.hdr.respLen = sizeof(resp.data);

    ret = CDC_MNGRG_sendSysCmd(&sysMsg, &resp);

    if (logCallback)
    {
        cdcLogCallback = logCallback;
    }

    return ret;
}

ERRG_codeE CDC_MNGRG_sendVeCmd(VE_MNGRG_messageT* veMsg, UINT8* output, UINT32 outputLen)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT      msg;
    CDC_MNGRP_responseT resp;

    msg.hdr.tag = CDC_MNGRP_VALID_CMD_TAG;
    msg.hdr.cmd = CDC_MNGRG_CMD_VE_E;
    msg.hdr.msgLen = sizeof(veMsg->hdr) + veMsg->hdr.msgLen;
    msg.data = (UINT8*)veMsg;

    resp.hdr.respLen = sizeof(resp.data);

    ret = CDC_MNGRP_sendCmd(&msg, &resp);
    if (ERRG_SUCCEEDED(ret) && output && outputLen <= resp.hdr.respLen)
    {
         memcpy(output, resp.data, resp.hdr.respLen);
    }

    return ret;
}

ERRG_codeE CDC_MNGRG_enableBokeh(CDC_MNGRG_handle handle, VE_MNGRG_bokehParamsT* bokehParams)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    veMsg.hdr.cmd = VE_MNGRG_ENABLE_BOKEH_E;

    if(bokehParams)
    {
        veMsg.hdr.msgLen = sizeof(VE_MNGRG_bokehParamsT);
        memcpy(veMsg.data, (UINT8*)bokehParams, veMsg.hdr.msgLen);
    }
    else
    {
        veMsg.hdr.msgLen = 0;
    }

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_disableBokeh(CDC_MNGRG_handle handle, VE_MNGRG_disableParamsT* disableParams)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    veMsg.hdr.cmd = VE_MNGRG_DISABLE_BOKEH_E;

    if(disableParams)
    {
        veMsg.hdr.msgLen = sizeof(VE_MNGRG_disableParamsT);
        memcpy(veMsg.data, (UINT8*)disableParams, veMsg.hdr.msgLen);
    }
    else
    {
        veMsg.hdr.msgLen = 0;
    }

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_setBokehParams(CDC_MNGRG_handle handle, VE_MNGRG_bokehParamsT* bokehParams)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    if(!bokehParams)
    {
        return CDC_MNGR__ERR_INVALID_ARGS;
    }

    veMsg.hdr.cmd = VE_MNGRG_SET_BOKEH_PARAMS_E;
    veMsg.hdr.msgLen = sizeof(VE_MNGRG_bokehParamsT);
    memcpy(veMsg.data, (UINT8*)bokehParams, veMsg.hdr.msgLen);

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_getBokehParams(CDC_MNGRG_handle handle, VE_MNGRG_bokehParamsT* bokehParams)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    if(!bokehParams)
    {
        return CDC_MNGR__ERR_INVALID_ARGS;
    }

    veMsg.hdr.cmd = VE_MNGRG_GET_BOKEH_PARAMS_E;
    veMsg.hdr.msgLen = 0;

    ret = CDC_MNGRG_sendVeCmd(&veMsg, (UINT8*)bokehParams, sizeof(VE_MNGRG_bokehParamsT));

    return ret;
}

ERRG_codeE CDC_MNGRG_initVeMngr(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    veMsg.hdr.cmd = VE_MNGRG_INIT_E;
    veMsg.hdr.msgLen = 0;

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_startStream(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    veMsg.hdr.cmd = VE_MNGRG_START_STREAM_E;
    veMsg.hdr.msgLen = 0;

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_stopStream(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    veMsg.hdr.cmd = VE_MNGRG_STOP_STREAM_E;
    veMsg.hdr.msgLen = 0;

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_test(CDC_MNGRG_handle handle)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    VE_MNGRG_messageT   veMsg;

    FIX_UNUSED_PARAM_WARN(handle);

    veMsg.hdr.cmd = VE_MNGRG_TEST_E;
    veMsg.hdr.msgLen = 0;

    ret = CDC_MNGRG_sendVeCmd(&veMsg, NULL, 0);

    return ret;
}

ERRG_codeE CDC_MNGRG_sendFwUpdateCmd(INU_FW_UPDATEG_messageT* fWUpdateCmd, UINT8* output, UINT32 outputLen)
{
    ERRG_codeE          ret = CDC_MNGR__RET_SUCCESS;
    CDC_MNGRP_msgT      msg;
    CDC_MNGRP_responseT resp;

    msg.hdr.tag = CDC_MNGRP_VALID_CMD_TAG;
    msg.hdr.cmd = CDC_MNGRG_CMD_UPDATE_E;
    msg.hdr.msgLen = sizeof(fWUpdateCmd->hdr) + fWUpdateCmd->hdr.msgLen;
    msg.data = (UINT8*)fWUpdateCmd;

    resp.hdr.respLen = sizeof(resp.data);

    ret = CDC_MNGRP_sendCmd(&msg, &resp);
    if (ERRG_SUCCEEDED(ret) && output && outputLen <= resp.hdr.respLen)
    {
        memcpy(output, resp.data, resp.hdr.respLen);
    }

    return ret;
}
#endif

