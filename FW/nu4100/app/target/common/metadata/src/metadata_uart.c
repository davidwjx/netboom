#include "metadata_uart.h"
#include "err_defs.h"
#include "nuserial.h"
#include "os_lyr.h"
OS_LYRG_mutexT METADATA_UART_mtx;   /*Mutex to protect sensorUpdateStats*/
METADATA_UART_sensorUpdateStats sensorUpdateStats;
static void METADATA_UART_msgCompleteCb(uint16_t msgID, int32_t retVal)
{
    LOGG_PRINT(LOG_INFO_E,NULL,"MsgID %lu Complete with retVal:%d \n", msgID, retVal);
    OS_LYRG_lockMutex(&METADATA_UART_mtx);
    if(retVal!=0)
    {
        sensorUpdateStats.messagesFailed++;
    }
    else
    {
        sensorUpdateStats.messagesSent++;
    }
    OS_LYRG_unlockMutex(&METADATA_UART_mtx);
}
static void METADATA_UART_msgRxCb(uint16_t msgID, void *buf, uint16_t readLen)
{
    OS_LYRG_lockMutex(&METADATA_UART_mtx);
    if(msgID == SENSOR_UPDATE_MESSAGE)
    {
        LOGG_PRINT(LOG_DEBUG_E,NULL,"Received sensor update over UART \n");
        sensorUpdateStats.messagesReceived++;
        METADATA_UPDATER_processIncomingSensorUpdateRequest(buf,readLen,false,false);
    }
    OS_LYRG_unlockMutex(&METADATA_UART_mtx);
}
ERRG_codeE METADATA_UART_repeatSensorUpdateFromMasterToSlave(char * buffer, size_t size)
{
    LOGG_PRINT(LOG_DEBUG_E,NULL,"Repeating message of size %lu from the master to the slave \n", size);
    OS_LYRG_lockMutex(&METADATA_UART_mtx);
    sensorUpdateStats.previousMessageCounter = sensorUpdateStats.currentMessageCounter;
    sensorUpdateStats.currentMessageCounter++;
    OS_LYRG_unlockMutex(&METADATA_UART_mtx);
    int ret = nuserial_queue_frame(SENSOR_UPDATE_MESSAGE,(void *) buffer,size,METADATA_UART_msgCompleteCb);
    if(ret == 0)
    {
        return INU_METADATA_UART__RET_SUCCESS;
    }
    else
    {
        return INU_METADATA_UART__ERR_ILLEGAL_STATE;
    }
}
/**
 * @brief Initialises the Metadata UART module
 * 
 * @return ERRG_codeE Returns an error code
 */
ERRG_codeE METADATA_UART_init()
{
    /*Resets the sensor update stats*/
    sensorUpdateStats.messagesSent = 0;
    sensorUpdateStats.messagesFailed = 0;
    sensorUpdateStats.currentMessageCounter = 0;
    sensorUpdateStats.previousMessageCounter = 0;
    sensorUpdateStats.messagesReceived = 0;
    /*Create mutex to protect sensorUpdateStats*/
    OS_LYRG_aquireMutex(&METADATA_UART_mtx);    
    int ret = nuserial_init(METADATA_UART_msgRxCb);
    if(ERRG_SUCCEEDED(ret))
    {
        LOGG_PRINT(LOG_INFO_E,NULL,"Successfully initialized the METADATA UART module \n");
        return INU_METADATA_UART__RET_SUCCESS;
    }
    else
    {   
        LOGG_PRINT(LOG_ERROR_E,NULL,"Failed to initialize the METADATA UART module [Error:%d]\n", ret);
        return INU_METADATA__ERR_ILLEGAL_STATE;
    }
}