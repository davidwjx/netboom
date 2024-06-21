/*
 * Copyright 2022 IMDT
 * All rights reserved.
 *
 */
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#include "os_lyr.h"
#include "inu_common.h"
#include "cls_com.h"
#include "inu_types.h"
#include "serial.h"
#include "uart_com.h"
#include "nuserial.h"
#include "que.h"
#include "uni_interruptable.h"
#include "uni_communication.h"
#include <queue>
#include <vector>
#include <memory>
#include "blocking_queue.hpp"
#define MAX_COUNT
#define PORTNAME            "/dev/ttyS1"
#define BAUDRATE            921600

#define SYNC_CMD            0x99
#define QUEUE_POP_TIMEOUT (50) /*50ms timeout*/
#define MAX_QUEUE_SIZE 10
typedef struct {
    uint16_t msgID;
    uint16_t  frmLen;
    std::shared_ptr<std::vector<uint8_t >> dataPtr; /*Using shared pointers to make sure we don't get memory leaks*/
} Frame_t;

typedef struct{
    Frame_t writeFrame;
    nuserial_write_notify_t writeCompleteCb;
}Write_Queue_Entry_t;

typedef struct{
    PUART uart0H;
    pthread_mutex_t uartHMutex;
}UART_Handle_t;

typedef struct{

    pthread_mutex_t queueMutex;
}Queue_Data_t;

typedef struct{
    UART_Handle_t uartHandle;
    InterruptHandle interrupt_handle;
    Queue_Data_t wrQueueData;
    Queue_Data_t rdQueueData;
    nuserial_receive_handler_t userRecvHandler;
    CommAttribute attr;
    bool bgThreadActive;
    bool recvThreadActive;
    UINT64 syncCounter;     /*Synchronization counter that starts from 0 */
    UINT64 lastRxSyncTime;    /*Last synchronization time in microseconds  */
    bool synchronized;
    UINT64 lastTxSyncTime;  /*Last time we have sent a sync command (ns)*/
    UINT64 startTxTime_ns;   /*Time when we started a UART tx*/
    UINT64 endTxTime_ns;     /*Time we finished a UART Tx*/
    UINT64 txCounter;        /*Number of UART messages transmitted*/
    UINT64 rxCounter;        /*Number of UART messages received */
}NuSerial_Info_t;

static NuSerial_Info_t nuserialInfo;
blocking_queue::queue<std::shared_ptr<Write_Queue_Entry_t>> queue(10); /*Using shared pointers to make sure we don't get memory leaks*/
static int _uart_write_mock_api(char *buff, int len) 
{
     UINT64 startTime = 0;
    UINT64 endTime = 0;
    ERRG_codeE  retCode;
    pthread_mutex_lock(&nuserialInfo.uartHandle.uartHMutex);
    OS_LYRG_getUsecTime(&startTime);
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Sending buffer of length %lu \n", len);
    retCode = SERIALG_send(&nuserialInfo.uartHandle.uart0H, (INT8 *)buff, len, 0xFFFFFFFF);
    OS_LYRG_getUsecTime(&endTime);
    pthread_mutex_unlock(&nuserialInfo.uartHandle.uartHMutex);
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Transmit time was %llu \n", (endTime-startTime) );
    if(!ERRG_SUCCEEDED(retCode)){
        LOGG_PRINT(LOG_ERROR_E, retCode, "UART write failed !!!\n");
    }
    return 0;
}

int nuserial_queue_frame(uint16_t msgID, void *framePayload, uint16_t payloadLen, nuserial_write_notify_t notifyFunc)
{
    std::shared_ptr<Write_Queue_Entry_t> queueEntryPtr = NULL;
    ERRG_codeE  retCode = UART_COM__RET_SUCCESS;
    if(!nuserial_connection_healthy())
    {   
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Cannot queue frame due to the connection not being healthy \n");
        return SERIAL__ERR_GENERAL_ERR;
    }
   
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Queueing frame, last sync time is %llu \n", nuserialInfo.lastRxSyncTime);
    if(framePayload){
        /*Use shared pointer to make sure that we don't get a memory leak*/
        queueEntryPtr = std::make_shared<Write_Queue_Entry_t>();
        if(!queueEntryPtr){
            retCode = SERIAL__ERR_GENERAL_ERR;
            LOGG_PRINT(LOG_ERROR_E, retCode, "Memory Allocation failed. Cannot queue frame !!!\n");
            return retCode;
        }

        /*Use a shared pointer to an std::vector with a preinitialized size*/
        queueEntryPtr->writeFrame.dataPtr = std::make_shared<std::vector<uint8_t>>(payloadLen);
        if(!queueEntryPtr->writeFrame.dataPtr){
            retCode = SERIAL__ERR_GENERAL_ERR;
            LOGG_PRINT(LOG_ERROR_E, retCode, "Data Memory Allocation failed. Cannot queue frame data !!!\n");
            return retCode;
        }
    }

    queueEntryPtr->writeCompleteCb = notifyFunc;
    queueEntryPtr->writeFrame.msgID = msgID;
    queueEntryPtr->writeFrame.frmLen = payloadLen;
    memcpy(queueEntryPtr->writeFrame.dataPtr->data(), framePayload, payloadLen);

    queue.try_push(std::move(queueEntryPtr));

    if(ERRG_FAILED(retCode)){
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to queue Frame, retCode =%d.\n", retCode);
        return  SERIAL__ERR_GENERAL_ERR;
    }

    return 0;
}

static void _recv_comm_packet(CommPacket *packet) 
{
    nuserialInfo.rxCounter++;
    if(packet->cmd == SYNC_CMD){
        // Received sync packet.
        nuserialInfo.syncCounter++;
        /*Flag to say that we have synchronized at least once*/
        nuserialInfo.synchronized = true;
        /*Store the time of the last sync time*/
        OS_LYRG_getUsecTime(&nuserialInfo.lastRxSyncTime); 
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Received a sync packet \n");
    }
    else{
        if(nuserialInfo.userRecvHandler)
            nuserialInfo.userRecvHandler(packet->cmd, packet->payload, packet->payload_len);
    }    
}

static int nuserial_recv_task(void *args) {
  int read_len;
  unsigned char buf[2048];

  LOGG_PRINT(LOG_INFO_E, NULL, "NuSerial Recv task started.\n\n");

  while (nuserialInfo.recvThreadActive) {
    memset(buf,0,sizeof(buf));
    read_len = read(nuserialInfo.uartHandle.uart0H.uartHandle, buf, sizeof(buf));
    if(read_len > 0)
        CommProtocolReceiveUartData(buf, read_len);

  }

  return 0;
}

int nuserial_send(uint16_t msgID, void *framePayload, uint16_t payloadLen)
{
    ERRG_codeE retCode = UART_COM__RET_SUCCESS;
    if(!framePayload){
        retCode = SERIAL__ERR_GENERAL_ERR;
        LOGG_PRINT(LOG_ERROR_E, NULL, "Invalid frame buffer.\n\n");
        return retCode;
    }

    retCode = (ERRG_codeE)CommProtocolPacketAssembleAndSend(msgID, (char *)framePayload, payloadLen, &nuserialInfo.attr);

    if(ERRG_FAILED(retCode)){
        LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to send. Error code - %d.\n\n", retCode);
        retCode = SERIAL__ERR_GENERAL_ERR;
    }

    return retCode;

}
void nuserial_sendSync()
{
    uint16_t syncCMD = SYNC_CMD;
    uint8_t *syncPayload = (uint8_t *)"NuSerial";
    uint16_t PayloadLen = strlen((char *)syncPayload);
    nuserial_send(syncCMD, syncPayload, PayloadLen);
}
void nuserial_synchronise(void)
{
    uint16_t syncCMD = SYNC_CMD;
    uint8_t *syncPayload = (uint8_t *)"NuSerial";
    uint16_t PayloadLen = strlen((char *)syncPayload);
    while(!nuserialInfo.synchronized){
        nuserial_send(syncCMD, syncPayload, PayloadLen);
        usleep(10000); // Wait for 10 milliseconds
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Trying to sync \n");
    }
}
static void check_send_sync_needed()
{
    UINT64 currentTime = 0;
    OS_LYRG_getUsecTime(&currentTime);
    if(currentTime - nuserialInfo.lastTxSyncTime >= SYNC_PERIOD)
    {
        /*Send the Sync packet every Sync Period, the other NU4k will timeout if it hasn't received a Sync packet within the timeout time*/
        nuserial_sendSync();
        nuserialInfo.lastTxSyncTime = currentTime;
    }
}
static int nuserialbg_task(void *args)
{

    ERRG_codeE retCode = UART_COM__RET_SUCCESS;


    LOGG_PRINT(LOG_INFO_E,NULL,"Starting NuSerial background task\n");
    while(nuserialInfo.bgThreadActive){
        int q_size = 0;
        std::shared_ptr<Write_Queue_Entry_t> queueEntryPtr;
        bool entry = queue.pop_timed(queueEntryPtr,QUEUE_POP_TIMEOUT); /*Use a 50 ms timeout*/
        // Check if anything in write queue to send
        if(entry)
        {
            LOGG_PRINT(LOG_DEBUG_E,NULL,"Sending frame \n");
            OS_LYRG_getTimeNsec(&nuserialInfo.startTxTime_ns );
            retCode = ( ERRG_codeE) CommProtocolPacketAssembleAndSend(queueEntryPtr->writeFrame.msgID,(char *) queueEntryPtr->writeFrame.dataPtr->data(),
                         queueEntryPtr->writeFrame.frmLen, &nuserialInfo.attr);
            if(ERRG_FAILED(retCode)){
                LOGG_PRINT(LOG_ERROR_E, NULL, "Failed to send frame. Frame ID - %d.\n\n", queueEntryPtr->writeFrame.msgID);

                // move on to next queue entry. We ahve tried waiting for ACK including retries to fail.
                continue;
            }
            else
            {
                nuserialInfo.txCounter++;
            }
            OS_LYRG_getTimeNsec(&nuserialInfo.endTxTime_ns );
            UINT64 transmitTime = nuserialInfo.endTxTime_ns - nuserialInfo.startTxTime_ns;
            //printf("Transmit time for this packet was %llu us for %lu bytes \n",transmitTime/1000,queueEntryPtr->writeFrame.frmLen );
            // notify the callback function on write complete.
            if(queueEntryPtr->writeCompleteCb && (nuserialInfo.attr.reliable == 1))
                queueEntryPtr->writeCompleteCb(queueEntryPtr->writeFrame.msgID,retCode);
        }
        check_send_sync_needed();
    }

    return 0;
}

bool nuserial_is_init_complete(void)
{
    return nuserialInfo.synchronized;
}
bool nuserial_connection_healthy(void)
{
    UINT64 currentTime = 0;
    OS_LYRG_getUsecTime(&currentTime);
    UINT64 timeDifference = (currentTime - nuserialInfo.lastRxSyncTime);
    return nuserialInfo.synchronized && (timeDifference  < SYNC_TIME_OUT );
}
bool nuserial_getUartDebugInformation(UINT64 *transmitCounter, 
    UINT64 *receiveCounter, 
    UINT64 *crcErrors, 
    UINT64 *transmitTimeNs,
    UINT64 *lastRxSyncTimeUsec)
{
    pthread_mutex_lock(&nuserialInfo.uartHandle.uartHMutex);
    *transmitCounter = nuserialInfo.txCounter;
    *receiveCounter = nuserialInfo.rxCounter;
    *crcErrors = 0;
    *transmitTimeNs = nuserialInfo.endTxTime_ns -  nuserialInfo.startTxTime_ns;
    *lastRxSyncTimeUsec = nuserialInfo.lastRxSyncTime;
    pthread_mutex_unlock(&nuserialInfo.uartHandle.uartHMutex);
}

int nuserial_init(nuserial_receive_handler_t recvHandler)
{
  ERRG_codeE  retVal = UART_COM__RET_SUCCESS;
  OS_LYRG_threadParams thrdParams;

  nuserialInfo.interrupt_handle = InterruptCreate();

  retVal = SERIALG_open(&nuserialInfo.uartHandle.uart0H, PORTNAME, BAUDRATE, SERIALG_TYPE_BLOCKING);

   if(ERRG_SUCCEEDED(retVal)){
        LOGG_PRINT(LOG_DEBUG_E, NULL, "UART0 Open succeeded.\n\n");
    }
    else{
        LOGG_PRINT(LOG_ERROR_E, NULL, "UART0 open failed.retval - %d\n\n", retVal);
        return SERIAL__ERR_GENERAL_ERR;
    }

  pthread_mutex_init(&nuserialInfo.uartHandle.uartHMutex, NULL);
  pthread_mutex_init(&nuserialInfo.wrQueueData.queueMutex, NULL);
  pthread_mutex_init(&nuserialInfo.rdQueueData.queueMutex, NULL);

  CommProtocolInit(_uart_write_mock_api, _recv_comm_packet);

  nuserialInfo.attr.reliable = 0;

  nuserialInfo.userRecvHandler = recvHandler;
  nuserialInfo.synchronized = false;
  nuserialInfo.lastRxSyncTime = 0;
  nuserialInfo.lastTxSyncTime = 0;
    {
        nuserialInfo.recvThreadActive = true;
        OS_LYRG_threadParams threadParams;
        /*Thread parameters*/
        threadParams.func = nuserial_recv_task;
        threadParams.id = OS_LYR_NUSERIAL_RECV_TASK_ID_E;
        threadParams.event = NULL;
        threadParams.param = NULL;

        //create thread
        OS_LYRG_threadHandle  threadHandle = OS_LYRG_createThread(&threadParams);
    }

    {
        nuserialInfo.bgThreadActive = true;
        OS_LYRG_threadParams threadParams;
        /*Thread parameters*/
        threadParams.func = nuserialbg_task;
        threadParams.id = OS_LYR_NUSERIAL_BG_TASK_ID_E;
        threadParams.event = NULL;
        threadParams.param = NULL;

        //create thread
        OS_LYRG_threadHandle threadHandle_bg = OS_LYRG_createThread(&threadParams);
        }

  return retVal;
}